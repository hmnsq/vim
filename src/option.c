/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Code to handle user-settable options. This is all pretty much table-
 * driven. Checklist for adding a new option:
 * - Put it in the options array in optiondefs.h (copy an existing entry).
 * - For a global option: Add a variable for it in option.h.
 * - For a buffer or window local option:
 *   - Add a PV_XX macro definition to the optiondefs.h file.
 *   - Add a variable to the window or buffer struct in structs.h.
 *   - For a window option, add some code to copy_winopt().
 *   - For a window string option, add code to check_win_options() and
 *     clear_winopt().
 *   - For a buffer option, add some code to buf_copy_options().
 *   - For a buffer string option, add code to check_buf_options().
 * - If it's a numeric option, add any necessary bounds checks to
 *   set_num_option().
 * - If it's a list of flags, add some code in did_set_string_option(), search
 *   for WW_ALL.
 * - When adding an option with expansion (P_EXPAND), but with a different
 *   default for Vi and Vim (no P_VI_DEF), add some code at VIMEXP.
 * - Add documentation!  One line in doc/quickref.txt, full description in
 *   options.txt, and any other related places.
 * - Add an entry in runtime/optwin.vim.
 * When making changes:
 * - Adjust the help for the option in doc/option.txt.
 * - When an entry has the P_VIM flag, or is lacking the P_VI_DEF flag, add a
 *   comment at the help for the 'compatible' option.
 */

#నిర్వచించు లోపల_OPTION_C
#అన్తర్భావ "vim.h"
#అన్తర్భావ "optiondefs.h"

స్థిర శూన్య సమితి_options_default(పూర్ణాంకం opt_flags);
స్థిర శూన్య సమితి_string_default_esc(అక్షరం *name, char_u *val, పూర్ణాంకం escape);
స్థిర char_u *కనుగొను_dup_అంశం(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
స్థిర char_u *option_expand(పూర్ణాంకం opt_idx, char_u *val);
స్థిర శూన్య didset_options(శూన్య);
స్థిర శూన్య didset_options2(శూన్య);
#ఒకవేళ defined(FEAT_EVAL)
స్థిర long_u *insecure_flag(win_T *wp, పూర్ణాంకం opt_idx, పూర్ణాంకం opt_flags);
#లేకపోతే
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#అయితే_ముగింపు
స్థిర అక్షరం *సమితి_bool_option(పూర్ణాంకం opt_idx, char_u *varp, పూర్ణాంకం విలువ, పూర్ణాంకం opt_flags);
స్థిర అక్షరం *సమితి_అంకె_option(పూర్ణాంకం opt_idx, char_u *varp, దీర్ఘ విలువ, అక్షరం *errbuf, size_t errbuflen, పూర్ణాంకం opt_flags);
స్థిర పూర్ణాంకం కనుగొను_కీలకం_option(char_u *arg_arg, పూర్ణాంకం has_lt);
స్థిర శూన్య showoptions(పూర్ణాంకం all, పూర్ణాంకం opt_flags);
స్థిర పూర్ణాంకం optval_default(నిర్మాణం vimoption *, char_u *varp, పూర్ణాంకం compatible);
స్థిర శూన్య showoneopt(నిర్మాణం vimoption *, పూర్ణాంకం opt_flags);
స్థిర పూర్ణాంకం put_setstring(FILE *fd, bool legacy, అక్షరం *cmd, అక్షరం *name, char_u **valuep, long_u flags);
స్థిర పూర్ణాంకం put_setnum(FILE *fd, అక్షరం *cmd, అక్షరం *name, దీర్ఘ *valuep);
స్థిర పూర్ణాంకం put_setbool(FILE *fd, అక్షరం *cmd, అక్షరం *name, పూర్ణాంకం విలువ);
స్థిర పూర్ణాంకం istermoption(నిర్మాణం vimoption *p);
స్థిర పూర్ణాంకం istermoption_idx(పూర్ణాంకం opt_idx);
స్థిర char_u *get_varp_scope(నిర్మాణం vimoption *p, పూర్ణాంకం scope);
స్థిర char_u *get_varp(నిర్మాణం vimoption *);
స్థిర శూన్య check_win_options(win_T *win);
స్థిర శూన్య option_value2string(నిర్మాణం vimoption *, పూర్ణాంకం scope);
స్థిర శూన్య check_winopt(winopt_T *wop);
స్థిర పూర్ణాంకం wc_use_keyname(char_u *varp, దీర్ఘ *wcp);
స్థిర శూన్య compatible_సమితి(శూన్య);

#ఒకవేళ defined(FEAT_EVAL)
స్థిర అక్షరం *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", ఖాళీ};
స్థిర అక్షరం *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    ఖాళీ};
స్థిర శూన్య didset_options_sctx(పూర్ణాంకం opt_flags, అక్షరం **buf);
#అయితే_ముగింపు


/*
 * Initialize the 'shell' option to a default value.
 */
    స్థిర శూన్య
సమితి_init_default_shell(శూన్య)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    ఒకవేళ (((p = mch_getenv((char_u *)"SHELL")) != ఖాళీ && *p != NUL)
#ఒకవేళ defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != ఖాళీ && *p != NUL)
	    || ((p = (char_u *)default_shell()) != ఖాళీ && *p != NUL)
#అయితే_ముగింపు
	    )
#ఒకవేళ defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	ఒకవేళ (vim_strchr(p, ' ') != ఖాళీ)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    ఒకవేళ (cmd != ఖాళీ)
	    {
		vim_snprintf((అక్షరం *)cmd, len, "\"%s\"", p);
		సమితి_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	లేకపోతే
	    సమితి_string_default("sh", p);
    }
#లేకపోతే
	సమితి_string_default_esc("sh", p, TRUE);
#అయితే_ముగింపు
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    స్థిర శూన్య
సమితి_init_default_backupskip(శూన్య)
{
    పూర్ణాంకం		opt_idx;
    పూర్ణాంకం		i;
    char_u	*p;
    పూర్ణాంకం		plen;
#నిర్వచితమైతే UNIX
    స్థిర అక్షరం	*(పేర్లు[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#లేకపోతే
    స్థిర అక్షరం	*(పేర్లు[3]) = {"TMPDIR", "TEMP", "TMP"};
#అయితే_ముగింపు
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(పేర్లు); ++i)
    {
	పూర్ణాంకం		mustfree = FALSE;
#నిర్వచితమైతే UNIX
	ఒకవేళ (*పేర్లు[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (పూర్ణాంకం)STRLEN_LITERAL("/private/tmp");
# లేకపోతే
	    p = (char_u *)"/tmp";
	    plen = (పూర్ణాంకం)STRLEN_LITERAL("/tmp");
# endif
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    p = vim_getenv((char_u *)పేర్లు[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	ఒకవేళ (p != ఖాళీ && *p != NUL)
	{
	    char_u  *అంశం;
	    size_t  itemsize;
	    పూర్ణాంకం	    has_trailing_మార్గం_sep = FALSE;

	    ఒకవేళ (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (పూర్ణాంకం)STRLEN(p);
		// does the value include a trailing path separator?
		ఒకవేళ (after_pathsep(p, p + plen))
		    has_trailing_మార్గం_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_మార్గం_sep ? 0 : 1) + 2;
	    అంశం = alloc(itemsize);
	    ఒకవేళ (అంశం != ఖాళీ)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((అక్షరం *)అంశం, itemsize, "%s%s*", p, (has_trailing_మార్గం_sep) ? "" : PATHSEPSTR);

		ఒకవేళ (కనుగొను_dup_అంశం(ga.ga_దత్తాంశం, అంశం, itemlen, options[opt_idx].flags) == ఖాళీ
			&& ga_grow(&ga, (పూర్ణాంకం)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", అంశం);
		}
		vim_free(అంశం);
	    }
	}
	ఒకవేళ (mustfree)
	    vim_free(p);
    }
    ఒకవేళ (ga.ga_దత్తాంశం != ఖాళీ)
    {
	సమితి_string_default("bsk", ga.ga_దత్తాంశం);
	vim_free(ga.ga_దత్తాంశం);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    స్థిర శూన్య
సమితి_init_default_maxmemtot(శూన్య)
{
    పూర్ణాంకం		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    ఒకవేళ (opt_idx < 0)
	తిరిగి;

#ఒకవేళ !defined(HAVE_AVAIL_MEM) && !defined(HAVE_మొత్తం_MEM)
    ఒకవేళ (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#అయితే_ముగింపు
    {
#ఒకవేళ defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#లేదావేళ defined(HAVE_మొత్తం_MEM)
	// Use amount of memory available to Vim.
	n = (mch_మొత్తం_mem(FALSE) >> 1);
#లేకపోతే
	n = (0x7fffffff >> 11);
#అయితే_ముగింపు
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	ఒకవేళ (opt_idx >= 0)
	{
#ఒకవేళ !defined(HAVE_AVAIL_MEM) && !defined(HAVE_మొత్తం_MEM)
	    ఒకవేళ ((దీర్ఘ)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (దీర్ఘ)n
		    || (దీర్ఘ)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#అయితే_ముగింపు
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    స్థిర శూన్య
సమితి_init_default_cdpath(శూన్య)
{
    పూర్ణాంకం		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    పూర్ణాంకం	i;
    పూర్ణాంకం	j;
    పూర్ణాంకం	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    ఒకవేళ (cdpath == ఖాళీ)
	తిరిగి;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    ఒకవేళ (buf != ఖాళీ)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	కొరకు (i = 0; cdpath[i] != NUL; ++i)
	{
	    ఒకవేళ (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    లేకపోతే
	    {
		ఒకవేళ (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	ఒకవేళ (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	లేకపోతే
	    vim_free(buf); // cannot happen
    }
    ఒకవేళ (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    స్థిర శూన్య
సమితి_init_default_printencoding(శూన్య)
{
#ఒకవేళ defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    సమితి_string_default("penc",
# ఒకవేళ defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# లేకపోతే // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    స్థిర శూన్య
సమితి_init_default_printexpr(శూన్య)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    సమితి_string_default("pexpr",
# ఒకవేళ defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# లేకపోతే
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#అయితే_ముగింపు

#నిర్వచితమైతే UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    స్థిర శూన్య
సమితి_init_restricted_బహుళకం(శూన్య)
{
    char_u	*p;

    p = get_isolated_shell_name();
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;
    ఒకవేళ (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#అయితే_ముగింపు

#నిర్వచితమైతే CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    స్థిర శూన్య
సమితి_init_clean_rtp(శూన్య)
{
    పూర్ణాంకం		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    ఒకవేళ (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    ఒకవేళ (opt_idx < 0)
	తిరిగి;
    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
    p_pp = (char_u *)CLEAN_RUNTIMEPATH;
}
#అయితే_ముగింపు

#నిర్వచితమైతే UNIX
/*
 * Change 'runtimepath' and 'packdir' to '$XDG_CONFIG_HOME/vim' if the only
 * vimrc found is located in '$XDG_CONFIG_HOME/vim/vimrc'.
 * In case the '$XDG_CONFIG_HOME' variable is not set, '$HOME/.config' is used
 * as a fallback as is defined in the XDG base dir specification:
 * <https://specifications.freedesktop.org/basedir/latest/>
 */
    స్థిర శూన్య
సమితి_init_xdg_rtp(శూన్య)
{
    పూర్ణాంకం		opt_idx;
    పూర్ణాంకం		has_xdg_env = TRUE;
    పూర్ణాంకం		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = ఖాళీ;
    char_u	*vimrc2 = ఖాళీ;
    char_u	*xdg_dir = ఖాళీ;
    char_u	*xdg_rtp = ఖాళీ;
    string_T	vimrc_xdg = {ఖాళీ, 0};

    // initialize chartab, so we can expand $HOME
    (శూన్య)init_chartab();
    vimrc1 = expand_env_save((char_u *)USR_VIMRC_FILE);
    vimrc2 = expand_env_save((char_u *)USR_VIMRC_FILE2);

    xdg_dir = mch_getenv("XDG_CONFIG_HOME");
    ఒకవేళ (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    ఒకవేళ (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	వెళ్ళు theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    ఒకవేళ ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	వెళ్ళు theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    ఒకవేళ ((opt_idx = findoption((char_u *)"packpath")) < 0)
	వెళ్ళు theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# ఒకవేళ defined(XDG_VDIR) && defined(FEAT_SESSION)
    ఒకవేళ ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	వెళ్ళు theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    ఒకవేళ (should_free_xdg_dir)
	vim_free(xdg_dir);
}
#అయితే_ముగింపు

/*
 * Expand environment variables and things like "~" for the defaults.
 * If option_expand() returns non-NULL the variable is expanded.  This can
 * only happen for non-indirect options.
 * Also set the default to the expanded value, so ":set" does not list
 * them.
 * Don't set the P_ALLOCED flag, because we don't want to free the
 * default.
 */
    స్థిర శూన్య
సమితి_init_expand_env(శూన్య)
{
    పూర్ణాంకం		opt_idx;
    char_u	*p;

    కొరకు (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	ఒకవేళ ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != ఖాళీ)
	    p = (char_u *)_(*(అక్షరం **)options[opt_idx].var);
	లేకపోతే
	    p = option_expand(opt_idx, ఖాళీ);
	ఒకవేళ (p != ఖాళీ && (p = vim_strsave(p)) != ఖాళీ)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    ఒకవేళ (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#ఒకవేళ defined(MSWIN) && defined(FEAT_GETTEXT)
/*
 * Get the display language of Windows and the languages to fall back on, as a
 * colon separated list for gettext, e.g. "ja_JP:en_US".  The list stops after
 * English, untranslated messages are English already.
 * Returns NULL when it cannot be obtained.  The result must be freed.
 */
    స్థిర char_u *
get_ui_langs(శూన్య)
{
    ULONG	అంకె_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*బఫరు;
    char_u	*langs = ఖాళీ;

    ఒకవేళ (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &అంకె_languages, ఖాళీ,
								    &bufsize)
	    || bufsize == 0)
	తిరిగి ఖాళీ;

    బఫరు = ALLOC_MULT(WCHAR, bufsize);
    ఒకవేళ (బఫరు == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &అంకె_languages, బఫరు,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	ఒకవేళ (langs != ఖాళీ)
	{
	    char_u	*d = langs;
	    WCHAR	*s = బఫరు;

	    అయితే (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		ఒకవేళ (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		కొరకు ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		ఒకవేళ (english)
		    ఆపు;
	    }
	    *d = NUL;
	}
    }
    vim_free(బఫరు);

    తిరిగి langs;
}
#అయితే_ముగింపు

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    స్థిర శూన్య
సమితి_init_lang_env(శూన్య)
{
#ఒకవేళ defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    ఒకవేళ (mch_getenv((char_u *)"LANG") == ఖాళీ
	    && mch_getenv((char_u *)"LANGUAGE") == ఖాళీ
	    && mch_getenv((char_u *)"LC_ALL") == ఖాళీ
	    && mch_getenv((char_u *)"LC_MESSAGES") == ఖాళీ)
    {
	char_u	*langs = get_ui_langs();

	ఒకవేళ (langs != ఖాళీ && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    ఒకవేళ (colon != ఖాళీ)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#లేదావేళ defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#అయితే_ముగింపు
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    స్థిర శూన్య
సమితి_init_default_encoding(శూన్య)
{
    char_u	*p;
    పూర్ణాంకం		opt_idx;

#ఒకవేళ defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#లేకపోతే
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#అయితే_ముగింపు
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    ఒకవేళ (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    ఒకవేళ (mb_init() == ఖాళీ)
    {
	opt_idx = findoption((char_u *)"encoding");
	ఒకవేళ (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#ఒకవేళ defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	ఒకవేళ (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    సమితి_string_option_ప్రత్యక్ష((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    సమితి_string_option_ప్రత్యక్ష((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    ఒకవేళ (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    ఒకవేళ (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (శూన్య)init_chartab();
	}
#అయితే_ముగింపు

#ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	ఒకవేళ (
# ifdef VIMDLL
		(!gui.లోపల_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    అక్షరం	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    ఒకవేళ (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, పరిమాణం(buf), "cp%ld", (దీర్ఘ)GetACP());
	    లేకపోతే
		buflen = vim_snprintf(buf, పరిమాణం(buf), "cp%ld", (దీర్ఘ)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    ఒకవేళ (p_tenc != ఖాళీ)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		ఒకవేళ (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_అమరిక(&ఆదానం_conv, p_tenc, p_enc);
		convert_అమరిక(&output_conv, p_enc, p_tenc);
	    }
	    లేకపోతే
		p_tenc = ఖాళీలు_option;
	}
#అయితే_ముగింపు
#ఒకవేళ defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	vim_free(p_enc);
	p_enc = save_enc;
    }
}

/*
 * Initialize the options, first part.
 *
 * Called only once from main(), just after creating the first buffer.
 * If "clean_arg" is TRUE Vim was started with --clean.
 */
    శూన్య
సమితి_init_1(పూర్ణాంకం clean_arg)
{
#నిర్వచితమైతే FEAT_LANGMAP
    langmap_init();
#అయితే_ముగింపు

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    ఒకవేళ (mch_getenv((char_u *)"VIM_POSIX") != ఖాళీ)
    {
	సమితి_string_default("cpo", (char_u *)CPO_ALL);
	సమితి_string_default("shm", (char_u *)SHM_POSIX);
    }

    సమితి_init_default_shell();
    సమితి_init_default_backupskip();
    సమితి_init_default_maxmemtot();
    సమితి_init_default_cdpath();
    సమితి_init_default_printencoding();
#ఒకవేళ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    సమితి_init_default_printexpr();
#అయితే_ముగింపు

    /*
     * Set all the options (except the terminal options) to their default
     * value.  Also set the global value for local options.
     */
    సమితి_options_default(0);

#నిర్వచితమైతే UNIX
    సమితి_init_xdg_rtp();
    సమితి_init_restricted_బహుళకం();
#అయితే_ముగింపు

#నిర్వచితమైతే CLEAN_RUNTIMEPATH
    ఒకవేళ (clean_arg)
	సమితి_init_clean_rtp();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (దొరికింది_తిరగేయి_arg)
	సమితి_option_విలువ_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
#అయితే_ముగింపు

    curbuf->b_p_initialized = true;
    curbuf->b_p_ac = -1;
    curbuf->b_p_ar = -1;	// no local 'autoread' value
#నిర్వచితమైతే HAVE_FSYNC
    curbuf->b_p_fs = -1;	// no local 'fsync' value
#అయితే_ముగింపు
    curbuf->b_p_ul = NO_LOCAL_UNDOLEVEL;
    check_buf_options(curbuf);
    check_win_options(curwin);
    check_options();

    // Must be before option_expand(), because that one needs vim_isIDc()
    didset_options();

#నిర్వచితమైతే FEAT_SPELL
    // Use the current chartab for the generic chartab. This is not in
    // didset_options() because it only depends on 'encoding'.
    init_spell_chartab();
#అయితే_ముగింపు

    సమితి_init_default_encoding();

    // Expand environment variables and things like "~" for the defaults.
    సమితి_init_expand_env();

    save_file_ff(curbuf);	// Buffer is unchanged

#ఒకవేళ defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    ఒకవేళ (mch_getenv((char_u *)"MLTERM") != ఖాళీ)
	సమితి_option_విలువ_give_err((char_u *)"tbidi", 1L, ఖాళీ, 0);
#అయితే_ముగింపు

    didset_options2();

    సమితి_init_lang_env();

#నిర్వచితమైతే FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    సమితి_helplang_default(get_mess_lang());
#అయితే_ముగింపు
}

స్థిర char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    శూన్య
సమితి_fencs_unicode(శూన్య)
{
    సమితి_string_option_ప్రత్యక్ష((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    స్థిర శూన్య
సమితి_option_default(
    పూర్ణాంకం		opt_idx,
    పూర్ణాంకం		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    పూర్ణాంకం		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    పూర్ణాంకం		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    పూర్ణాంకం		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    ఒకవేళ (varp != ఖాళీ)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	ఒకవేళ (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    ఒకవేళ (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		సమితి_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    లేకపోతే ఒకవేళ (options[opt_idx].indir != PV_NONE)
		సమితి_string_option_ప్రత్యక్ష(ఖాళీ, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    లేకపోతే
	    {
		ఒకవేళ ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	లేకపోతే ఒకవేళ (flags & P_అంకె)
	{
	    ఒకవేళ (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    లేకపోతే
	    {
		దీర్ఘ def_val = (దీర్ఘ)(long_i)options[opt_idx].def_val[dvi];

		ఒకవేళ ((దీర్ఘ *)varp == &curwin->w_p_so
			|| (దీర్ఘ *)varp == &curwin->w_p_siso
			|| (దీర్ఘ *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(దీర్ఘ *)varp = -1;
		లేకపోతే
		    *(దీర్ఘ *)varp = def_val;
		// May also set global value for local option.
		ఒకవేళ (both)
		    *(దీర్ఘ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	లేకపోతే	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(పూర్ణాంకం *)varp = (పూర్ణాంకం)(దీర్ఘ)(long_i)options[opt_idx].def_val[dvi];
#నిర్వచితమైతే UNIX
	    // 'modeline' defaults to off for root
	    ఒకవేళ (options[opt_idx].indir == PV_ML && getuid() == మూలం_UID)
		*(పూర్ణాంకం *)varp = FALSE;
#అయితే_ముగింపు
	    // May also set global value for local option.
	    ఒకవేళ (both)
		*(పూర్ణాంకం *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(పూర్ణాంకం *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	ఒకవేళ (both)
	{
	    flagsp = insecure_flag(curwin, opt_idx, OPT_LOCAL);
	    *flagsp = *flagsp & ~P_INSECURE;
	}
    }

#నిర్వచితమైతే FEAT_EVAL
    సమితి_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#అయితే_ముగింపు
}

/*
 * Set all options (except terminal options) to their default value.
 * When "opt_flags" is non-zero skip 'encoding'.
 */
    స్థిర శూన్య
సమితి_options_default(
    పూర్ణాంకం		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    పూర్ణాంకం		i;
    win_T	*wp;
    tabpage_T	*tp;

    కొరకు (i = 0; !istermoption_idx(i); i++)
	ఒకవేళ (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#ఒకవేళ defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_కీలకం
#అయితే_ముగింపు
			)))
	    సమితి_option_default(i, opt_flags, p_cp);

    // The 'scroll' option must be computed for all windows.
    FOR_ALL_TAB_WINDOWS(tp, wp)
	win_comp_scroll(wp);
    parse_cino(curbuf);
}

/*
 * Set the Vi-default value of a string option.
 * Used for 'sh', 'backupskip' and 'term'.
 * When "escape" is TRUE escape spaces with a backslash.
 */
    స్థిర శూన్య
సమితి_string_default_esc(అక్షరం *name, char_u *val, పూర్ణాంకం escape)
{
    char_u	*p;
    పూర్ణాంకం		opt_idx;

    ఒకవేళ (escape && vim_strchr(val, ' ') != ఖాళీ)
	p = vim_strsave_escaped(val, (char_u *)" ");
    లేకపోతే
	p = vim_strsave(val);
    ఒకవేళ (p == ఖాళీ)		// we don't want a NULL
	తిరిగి;

    opt_idx = findoption((char_u *)name);
    ఒకవేళ (opt_idx < 0)
    {
	vim_free(p);
	తిరిగి;
    }

    ఒకవేళ (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    శూన్య
సమితి_string_default(అక్షరం *name, char_u *val)
{
    సమితి_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    స్థిర char_u *
కనుగొను_dup_అంశం(char_u *origval, char_u *newval, size_t newvallen, long_u flags)
{
    పూర్ణాంకం	    bs = 0;
    char_u  *s;

    ఒకవేళ (origval == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (s = origval; *s != NUL; ++s)
    {
	ఒకవేళ ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    తిరిగి s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	ఒకవేళ ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	లేకపోతే
	    bs = 0;
    }
    తిరిగి ఖాళీ;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    శూన్య
సమితి_సంఖ్య_default(అక్షరం *name, దీర్ఘ val)
{
    పూర్ణాంకం		opt_idx;

    opt_idx = findoption((char_u *)name);
    ఒకవేళ (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#ఒకవేళ defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    శూన్య
సమితి_local_options_default(win_T *wp, పూర్ణాంకం do_బఫరు)
{
    win_T	*save_curwin = curwin;
    పూర్ణాంకం		i;

    curwin = wp;
    curbuf = curwin->w_బఫరు;
    ఖండం_autocmds();

    కొరకు (i = 0; !istermoption_idx(i); i++)
    {
	నిర్మాణం vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	ఒకవేళ (p->indir != PV_NONE
		&& (do_బఫరు || (p->indir & PV_BUF) == 0)
		&& !(options[i].flags & P_NODEFAULT)
		&& !optval_default(p, varp, FALSE))
	    సమితి_option_default(i, OPT_FREE|OPT_LOCAL, FALSE);
    }

    unblock_autocmds();
    curwin = save_curwin;
    curbuf = curwin->w_బఫరు;
}
#అయితే_ముగింపు

#ఒకవేళ defined(EXITFREE)
/*
 * Free all options.
 */
    శూన్య
free_all_options(శూన్య)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; !istermoption_idx(i); i++)
    {
	ఒకవేళ (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    ఒకవేళ ((options[i].flags & P_ALLOCED) && options[i].var != ఖాళీ)
		free_string_option(*(char_u **)options[i].var);
	    ఒకవేళ (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	లేకపోతే ఒకవేళ (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# ఒకవేళ defined(FEAT_EVAL)
    free_findfunc_option();
# endif
}
#అయితే_ముగింపు


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    శూన్య
సమితి_init_2(శూన్య)
{
    పూర్ణాంకం		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    ఒకవేళ (idx >= 0 && !(options[idx].flags & P_WAS_సమితి))
	సమితి_option_default(idx, OPT_LOCAL, p_cp);
    comp_నిలువు();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    ఒకవేళ (!option_was_సమితి((char_u *)"window"))
	p_కిటికీ = అడ్డువరుసలు - 1;
    సమితి_సంఖ్య_default("window", అడ్డువరుసలు - 1);

    // For DOS console the default is always black.
#ఒకవేళ !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    ఒకవేళ (idx >= 0 && !(options[idx].flags & P_WAS_సమితి)
						 && *పదం_bg_default() == 'd')
    {
	సమితి_string_option_ప్రత్యక్ష(ఖాళీ, idx, (char_u *)"dark", OPT_FREE, 0);
	// don't mark it as set, when starting the GUI it may be
	// changed again
	options[idx].flags &= ~P_WAS_సమితి;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే CURSOR_SHAPE
    parse_shape_opt(SHAPE_CURSOR);	// set cursor shapes from 'guicursor'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSESHAPE
    parse_shape_opt(SHAPE_MOUSE);	// set mouse shapes from 'mouseshape'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PRINTER
    (శూన్య)parse_printoptions(ఖాళీ);	// parse 'printoptions' default value
#అయితే_ముగింపు
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    శూన్య
సమితి_init_3(శూన్య)
{
#ఒకవేళ defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    పూర్ణాంకం	    idx_srr;
    పూర్ణాంకం	    do_srr;
# ifdef FEAT_QUICKFIX
    పూర్ణాంకం	    idx_sp;
    పూర్ణాంకం	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    ఒకవేళ (idx_srr < 0)
	do_srr = FALSE;
    లేకపోతే
	do_srr = !(options[idx_srr].flags & P_WAS_సమితి);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    ఒకవేళ (idx_sp < 0)
	do_sp = FALSE;
    లేకపోతే
	do_sp = !(options[idx_sp].flags & P_WAS_సమితి);
# endif
    p = get_isolated_shell_name();
    ఒకవేళ (p != ఖాళీ)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	ఒకవేళ (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# ఒకవేళ defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# ఒకవేళ defined(FEAT_QUICKFIX)
	    ఒకవేళ (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  లేకపోతే
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    ఒకవేళ (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	లేకపోతే ఒకవేళ (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  ఒకవేళ defined(FEAT_QUICKFIX)
		ఒకవేళ (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		ఒకవేళ (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	లేకపోతే
	    // Always use POSIX shell style redirection if we reach this
	    ఒకవేళ (       fnamecmp(p, "sh") == 0
		    || fnamecmp(p, "ksh") == 0
		    || fnamecmp(p, "mksh") == 0
		    || fnamecmp(p, "pdksh") == 0
		    || fnamecmp(p, "zsh") == 0
		    || fnamecmp(p, "zsh-beta") == 0
		    || fnamecmp(p, "bash") == 0
		    || fnamecmp(p, "fish") == 0
		    || fnamecmp(p, "ash") == 0
		    || fnamecmp(p, "dash") == 0
		    || fnamecmp(p, "pwsh") == 0
# ifdef MSWIN
		    || fnamecmp(p, "cmd") == 0
		    || fnamecmp(p, "sh.exe") == 0
		    || fnamecmp(p, "ksh.exe") == 0
		    || fnamecmp(p, "mksh.exe") == 0
		    || fnamecmp(p, "pdksh.exe") == 0
		    || fnamecmp(p, "zsh.exe") == 0
		    || fnamecmp(p, "zsh-beta.exe") == 0
		    || fnamecmp(p, "bash.exe") == 0
		    || fnamecmp(p, "cmd.exe") == 0
		    || fnamecmp(p, "dash.exe") == 0
		    || fnamecmp(p, "pwsh.exe") == 0
# endif
		    )
	    {
# ఒకవేళ defined(FEAT_QUICKFIX)
		ఒకవేళ (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  లేకపోతే
		    ఒకవేళ (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    లేకపోతే
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		ఒకవేళ (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#అయితే_ముగింపు

#ఒకవేళ defined(MSWIN)
    /*
     * Set 'shellcmdflag', 'shellxquote', and 'shellquote' depending on the
     * 'shell' option.
     * This is done after other initializations, where 'shell' might have been
     * set, but only if they have not been set before.
     * Default values depend on shell (cmd.exe is default shell):
     *
     *			    p_shcf	p_sxq
     * cmd.exe		-   "/c"	"("
     * powershell.exe   -   "-Command"	"\""
     * pwsh.exe		-   "-c"	"\""
     * "sh" like shells -   "-c"	"\""
     *
     * For Win32 p_sxq is set instead of p_shq to include shell redirection.
     */
    ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)gettail(p_sh), "powershell") != ఖాళీ)
    {
	పూర్ణాంకం	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	ఒకవేళ (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_సమితి))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	ఒకవేళ (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_సమితి))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    లేకపోతే ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)gettail(p_sh), "sh") != ఖాళీ)
    {
	పూర్ణాంకం	idx3;

	idx3 = findoption((char_u *)"shcf");
	ఒకవేళ (idx3 >= 0 && !(options[idx3].flags & P_WAS_సమితి))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	ఒకవేళ (idx3 >= 0 && !(options[idx3].flags & P_WAS_సమితి))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    లేకపోతే ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)gettail(p_sh), "cmd.exe") != ఖాళీ)
    {
	పూర్ణాంకం	idx3;

	/*
	 * cmd.exe on Windows will strip the first and last double quote given
	 * on the command line, e.g. most of the time things like:
	 *   cmd /c "my path/to/echo" "my args to echo"
	 * become:
	 *   my path/to/echo" "my args to echo
	 * when executed.
	 *
	 * To avoid this, set shellxquote to surround the command in
	 * parenthesis.  This appears to make most commands work, without
	 * breaking commands that worked previously, such as
	 * '"path with spaces/cmd" "a&b"'.
	 */
	idx3 = findoption((char_u *)"sxq");
	ఒకవేళ (idx3 >= 0 && !(options[idx3].flags & P_WAS_సమితి))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	ఒకవేళ (idx3 >= 0 && !(options[idx3].flags & P_WAS_సమితి))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#అయితే_ముగింపు

    ఒకవేళ (BUFEMPTY())
    {
	పూర్ణాంకం idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	ఒకవేళ (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_సమితి))
	    సమితి_fileformat(default_fileformat(), OPT_LOCAL);
    }

    సమితి_title_defaults();
}

#ఒకవేళ defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    శూన్య
సమితి_helplang_default(char_u *lang)
{
    పూర్ణాంకం		idx;
    size_t	langlen;

    ఒకవేళ (lang == ఖాళీ)	// safety check
	తిరిగి;

    langlen = STRLEN(lang);
    ఒకవేళ (langlen < 2)	// safety check
	తిరిగి;

    idx = findoption((char_u *)"hlg");
    ఒకవేళ (idx < 0 || (options[idx].flags & P_WAS_సమితి))
	తిరిగి;

    ఒకవేళ (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    ఒకవేళ (p_hlg == ఖాళీ)
	p_hlg = ఖాళీలు_option;
    లేకపోతే
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	ఒకవేళ (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	లేకపోతే ఒకవేళ (langlen >= 1 && *p_hlg == 'C')
	{
	    p_hlg[0] = 'e';
	    p_hlg[1] = 'n';
	}
	p_hlg[2] = NUL;
    }
    options[idx].flags |= P_ALLOCED;
}
#అయితే_ముగింపు

/*
 * 'title' and 'icon' only default to true if they have not been set or reset
 * in .vimrc and we can read the old value.
 * When 'title' and 'icon' have been reset in .vimrc, we won't even check if
 * they can be reset.  This reduces startup time when using X on a remote
 * machine.
 */
    శూన్య
సమితి_title_defaults(శూన్య)
{
    పూర్ణాంకం	    idx1;
    దీర్ఘ    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    ఒకవేళ (idx1 >= 0 && !(options[idx1].flags & P_WAS_సమితి))
    {
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (gui.starting || gui.లోపల_use)
	    val = TRUE;
	లేకపోతే
#అయితే_ముగింపు
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    ఒకవేళ (idx1 < 0 || (options[idx1].flags & P_WAS_సమితి))
    {
	తిరిగి;
    }

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.starting || gui.లోపల_use)
	val = TRUE;
    లేకపోతే
#అయితే_ముగింపు
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    శూన్య
ex_సమితి(exarg_T *eap)
{
    పూర్ణాంకం		flags = 0;

    ఒకవేళ (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    లేకపోతే ఒకవేళ (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#ఒకవేళ defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    ఒకవేళ ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    లేకపోతే
#అయితే_ముగింపు
    {
	ఒకవేళ (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(శూన్య)do_సమితి(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
రకనిర్వచనం గణన {
    ఉపసర్గ_NO = 0,	// "no" prefix
    ఉపసర్గ_NONE,	// no prefix
    ఉపసర్గ_INV,		// "inv" prefix
} సమితి_ఉపసర్గ_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    స్థిర సమితి_ఉపసర్గ_T
get_option_ఉపసర్గ(char_u **argp)
{
    పూర్ణాంకం		ఉపసర్గ = ఉపసర్గ_NONE;
    char_u	*arg = *argp;

    ఒకవేళ (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	ఉపసర్గ = ఉపసర్గ_NO;
	arg += 2;
    }
    లేకపోతే ఒకవేళ (STRNCMP(arg, "inv", 3) == 0)
    {
	ఉపసర్గ = ఉపసర్గ_INV;
	arg += 3;
    }

    *argp = arg;
    తిరిగి ఉపసర్గ;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    స్థిర పూర్ణాంకం
parse_option_name(char_u *arg, పూర్ణాంకం *opt_idxp, పూర్ణాంకం *lenp, పూర్ణాంకం *keyp)
{
    పూర్ణాంకం	కీలకం = 0;
    పూర్ణాంకం	len;
    పూర్ణాంకం	opt_idx;

    ఒకవేళ (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	ఒకవేళ (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	లేకపోతే
	{
	    len = 1;
	    అయితే (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	ఒకవేళ (arg[len] != '>')
	    తిరిగి FAIL;

	arg[len] = NUL;			    // put NUL after name
	ఒకవేళ (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	ఒకవేళ (opt_idx == -1)
	    కీలకం = కనుగొను_కీలకం_option(arg + 1, TRUE);
    }
    లేకపోతే
    {
	పూర్ణాంకం	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	ఒకవేళ (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	లేకపోతే
	    అయితే (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	ఒకవేళ (opt_idx == -1)
	    కీలకం = కనుగొను_కీలకం_option(arg, FALSE);
    }

    *keyp = కీలకం;
    *lenp = len;
    *opt_idxp = opt_idx;

    తిరిగి OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    స్థిర సమితి_op_T
get_opt_op(char_u *arg)
{
    సమితి_op_T op = OP_NONE;

    ఒకవేళ (*arg != NUL && *(arg + 1) == '=')
    {
	ఒకవేళ (*arg == '+')
	    op = OP_ADDING;		// "+="
	లేకపోతే ఒకవేళ (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	లేకపోతే ఒకవేళ (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    తిరిగి op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
స్థిర అక్షరం *modeline_whitelist[] =
{
    "autoindent",
    "cindent",
    "commentstring",
    "expandtab",
    "filetype",
    "foldcolumn",
    "foldenable",
    "foldmarker",
    "foldmethod",
    "modifiable",
    "readonly",
    "rightleft",
    "shiftwidth",
    "smartindent",
    "softtabstop",
    "spell",
    "spelllang",
    "tabstop",
    "textwidth",
    "varsofttabstop",
    "vartabstop",
    ఖాళీ
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    స్థిర bool
is_modeline_whitelisted(అక్షరం *name)
{
    కొరకు (పూర్ణాంకం i = 0; modeline_whitelist[i] != ఖాళీ; i++)
	ఒకవేళ (STRCMP(name, modeline_whitelist[i]) == 0)
	    తిరిగి true;
    తిరిగి false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    స్థిర పూర్ణాంకం
validate_opt_idx(
	పూర్ణాంకం opt_idx,
	పూర్ణాంకం opt_flags,
	long_u flags,
	అక్షరం **errmsg,
	సమితి_ఉపసర్గ_T ఉపసర్గ)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    ఒకవేళ ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	తిరిగి FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    ఒకవేళ ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	తిరిగి FAIL;

    // Disallow changing some options from modelines.
    ఒకవేళ (opt_flags & OPT_MODELINE)
    {
	ఒకవేళ (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_లోపల_modeline;
	    తిరిగి FAIL;
	}
	ఒకవేళ ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_లోపల_modeline_when_modelineexpr_is_off;
	    తిరిగి FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	ఒకవేళ (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    ఒకవేళ (options[opt_idx].indir == PV_ML && ఉపసర్గ == ఉపసర్గ_NO)
		తిరిగి OK;
	    లేకపోతే ఒకవేళ (!is_modeline_whitelisted(options[opt_idx].fullname))
		తిరిగి FAIL;
	}
#నిర్వచితమైతే FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	ఒకవేళ (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    తిరిగి FAIL;
#అయితే_ముగింపు
    }

#నిర్వచితమైతే HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    ఒకవేళ (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_లోపల_sandbox;
	తిరిగి FAIL;
    }
#అయితే_ముగింపు

    తిరిగి OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    స్థిర char_u *
stropt_get_default_val(
    పూర్ణాంకం		opt_idx,
    char_u	*varp,
    పూర్ణాంకం		flags,
    పూర్ణాంకం		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    ఒకవేళ ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (gui.లోపల_use)
	    newval = gui_bg_default();
	లేకపోతే
#అయితే_ముగింపు
	    newval = పదం_bg_default();
    }
    లేకపోతే ఒకవేళ ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    ఒకవేళ (newval == ఖాళీ)
	newval = ఖాళీలు_option;
    లేకపోతే
    {
	char_u *s = option_expand(opt_idx, newval);
	ఒకవేళ (s == ఖాళీ)
	    s = newval;
	newval = vim_strsave(s);
    }

    తిరిగి newval;
}

/*
 * Convert the 'backspace' option number value to a string: for adding,
 * prepending and removing string.
 */
    స్థిర శూన్య
opt_backspace_nr2str(
    char_u	*varp,
    char_u	**origval_p,
    char_u	**origval_l_p,
    char_u	**origval_g_p,
    char_u	**oldval_p)
{
    పూర్ణాంకం		i = getdigits((char_u **)varp);

    ఎంపిక (i)
    {
	స్థితి 0:
	    *(char_u **)varp = ఖాళీలు_option;
	    ఆపు;
	స్థితి 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    ఆపు;
	స్థితి 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    ఆపు;
	స్థితి 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    ఆపు;
    }
    vim_free(*oldval_p);
    ఒకవేళ (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    ఒకవేళ (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    ఒకవేళ (*origval_g_p == *oldval_p)
	*origval_g_p = *(char_u **)varp;
    *oldval_p = *(char_u **)varp;
}

/*
 * Convert the 'whichwrap' option number value to a string, for backwards
 * compatibility with Vim 3.0.
 * Note: 'argp' is a pointer to a char_u pointer and is updated.
 */
    స్థిర char_u *
opt_whichwrap_nr2str(char_u **argp, char_u *whichwrap)
{
    size_t  len = 0;

    *whichwrap = NUL;
    పూర్ణాంకం i = getdigits(argp);
    ఒకవేళ (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    ఒకవేళ (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    ఒకవేళ (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    ఒకవేళ (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    ఒకవేళ (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    ఒకవేళ (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    తిరిగి whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    స్థిర char_u *
stropt_copy_విలువ(
    char_u	*origval,
    char_u	**argp,
    సమితి_op_T	op,
    పూర్ణాంకం		flags వాడని)
{
    char_u	*arg = *argp;
    గుర్తులేని	newlen;
    char_u	*newval;
    char_u	*s = ఖాళీ;

    // get a bit too much
    newlen = (గుర్తులేని)STRLEN(arg) + 1;
    ఒకవేళ (op != OP_NONE)
	newlen += (గుర్తులేని)STRLEN(origval) + 1;
    newval = alloc(newlen);
    ఒకవేళ (newval == ఖాళీ)  // out of mem, don't change
	తిరిగి ఖాళీ;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    అయితే (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	పూర్ణాంకం i;

	ఒకవేళ (*arg == '\\' && arg[1] != NUL
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#అయితే_ముగింపు
	   )
	    ++arg;	// remove backslash
	ఒకవేళ (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	లేకపోతే
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    తిరిగి newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    స్థిర char_u *
stropt_expand_envvar(
    పూర్ణాంకం		opt_idx,
    char_u	*origval,
    char_u	*newval,
    సమితి_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    ఒకవేళ (s == ఖాళీ)
	తిరిగి newval;

    vim_free(newval);
    గుర్తులేని newlen = (గుర్తులేని)STRLEN(s) + 1;
    ఒకవేళ (op != OP_NONE)
	newlen += (గుర్తులేని)STRLEN(origval) + 1;

    newval = alloc(newlen);
    ఒకవేళ (newval == ఖాళీ)
	తిరిగి ఖాళీ;

    STRCPY(newval, s);

    తిరిగి newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    స్థిర శూన్య
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    సమితి_op_T	op,
    పూర్ణాంకం		flags)
{
    పూర్ణాంకం		len = 0;

    పూర్ణాంకం comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    ఒకవేళ (op == OP_ADDING)
    {
	len = (పూర్ణాంకం)STRLEN(origval);
	// strip a trailing comma, would get 2
	ఒకవేళ (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    లేకపోతే
    {
	len = (పూర్ణాంకం)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    ఒకవేళ (comma)
	newval[len] = ',';
}

/*
 * Remove a value from a string option.  Copy string option value in "origval"
 * to "newval" and then remove the string "strval" of length "len".
 */
    స్థిర శూన్య
stropt_remove_val(
    char_u	*origval,
    char_u	*newval,
    పూర్ణాంకం		flags,
    char_u	*strval,
    పూర్ణాంకం		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    ఒకవేళ (*strval)
    {
	// may need to remove a comma
	ఒకవేళ (flags & P_COMMA)
	{
	    ఒకవేళ (strval == origval)
	    {
		// include comma after string
		ఒకవేళ (strval[len] == ',')
		    ++len;
	    }
	    లేకపోతే
	    {
		// include comma before string
		--strval;
		++len;
	    }
	}
	STRMOVE(newval + (strval - origval), strval + len);
    }
}

/*
 * Find a comma-separated item in "src" that matches the key part of "key".
 * The key is the part before ':'.  "keylen" is the length including ':'.
 * Returns a pointer to the found item in "src", or NULL if not found.
 * Sets "*itemlenp" to the length of the found item (up to ',' or NUL).
 */
    స్థిర char_u *
కనుగొను_కీలకం_అంశం(char_u *src, char_u *కీలకం, పూర్ణాంకం keylen, పూర్ణాంకం *itemlenp)
{
    char_u  *p = src;

    అయితే (*p != NUL)
    {
	// Check if this item starts with the same key
	ఒకవేళ ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, కీలకం, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *చివర = vim_strchr(p, ',');
	    ఒకవేళ (చివర == ఖాళీ)
		చివర = p + STRLEN(p);
	    *itemlenp = (పూర్ణాంకం)(చివర - p);
	    తిరిగి p;
	}
	++p;
    }
    తిరిగి ఖాళీ;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    స్థిర శూన్య
remove_comma_అంశం(char_u *str, char_u *అంశం, పూర్ణాంకం itemlen)
{
    ఒకవేళ (అంశం[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(అంశం, అంశం + itemlen + 1);
    లేకపోతే ఒకవేళ (అంశం > str && *(అంశం - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(అంశం - 1, అంశం + itemlen);
    లేకపోతే
	// Only item
	*అంశం = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    స్థిర శూన్య
remove_కీలకం_అంశం(char_u *str, char_u *కీలకం, పూర్ణాంకం keylen, char_u *skip)
{
    పూర్ణాంకం	    itemlen;
    char_u  *దొరికింది;

    అయితే ((దొరికింది = కనుగొను_కీలకం_అంశం(str, కీలకం, keylen, &itemlen)) != ఖాళీ)
    {
	ఒకవేళ (దొరికింది == skip)
	{
	    // Search for the next match after this one.
	    char_u *తదుపరి = దొరికింది + itemlen;
	    ఒకవేళ (*తదుపరి == ',')
		++తదుపరి;
	    దొరికింది = కనుగొను_కీలకం_అంశం(తదుపరి, కీలకం, keylen, &itemlen);
	    ఒకవేళ (దొరికింది == ఖాళీ)
		ఆపు;
	}

	remove_comma_అంశం(str, దొరికింది, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    స్థిర శూన్య
append_అంశం(char_u *str, char_u *అంశం, పూర్ణాంకం అంశం_len)
{
    పూర్ణాంకం len = (పూర్ణాంకం)STRLEN(str);

    ఒకవేళ (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, అంశం, (size_t)అంశం_len);
    str[len + అంశం_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    స్థిర శూన్య
prepend_అంశం(char_u *str, char_u *అంశం, పూర్ణాంకం అంశం_len)
{
    పూర్ణాంకం len = (పూర్ణాంకం)STRLEN(str);
    పూర్ణాంకం comma = (len > 0) ? 1 : 0;

    mch_memmove(str + అంశం_len + comma, str, (size_t)len + 1);
    mch_memmove(str, అంశం, (size_t)అంశం_len);
    ఒకవేళ (comma)
	str[అంశం_len] = ',';
}

/*
 * For a P_COMMA option: process "key:value" items in "newval" individually.
 * Each comma-separated item in "newval" is checked against "origval":
 *
 * For OP_ADDING/OP_PREPENDING, each item is handled as follows:
 *   - colon item, key exists with different value: replace (remove old, add)
 *   - colon item, exact duplicate: do nothing
 *   - colon item, not found: add to end
 *   - non-colon item, exists: do nothing
 *   - non-colon item, not found: add to end
 *
 * For OP_REMOVING, each item is handled as follows:
 *   - colon item: remove by key match
 *   - non-colon item: remove by exact match
 *
 * The result is written to "newval".
 * Returns true if the operation was fully handled (caller should skip the
 * normal add/remove logic).  Returns false if newval is a single non-colon
 * item, meaning the caller should use the existing code path.
 */
    స్థిర bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    సమితి_op_T	op,
    పూర్ణాంకం		flags వాడని)
{
    char_u  *p;
    char_u  *అంశం_ప్రారంభం;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    ఒకవేళ (vim_strchr(newval, ':') == ఖాళీ && vim_strchr(newval, ',') == ఖాళీ)
	తిరిగి false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    ఒకవేళ (newval_copy == ఖాళీ)
	తిరిగి false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    అంశం_ప్రారంభం = newval_copy;
    కొరకు (;;)
    {
	p = vim_strchr(అంశం_ప్రారంభం, ',');
	పూర్ణాంకం అంశం_len = (p == ఖాళీ)
			    ? (పూర్ణాంకం)STRLEN(అంశం_ప్రారంభం) : (పూర్ణాంకం)(p - అంశం_ప్రారంభం);

	ఒకవేళ (అంశం_len > 0)
	{
	    char_u *colon = vim_strchr(అంశం_ప్రారంభం, ':');
	    ఒకవేళ (colon != ఖాళీ && colon < అంశం_ప్రారంభం + అంశం_len)
	    {
		పూర్ణాంకం keylen = (పూర్ణాంకం)(colon - అంశం_ప్రారంభం) + 1;

		ఒకవేళ (op == OP_ADDING || op == OP_PREPENDING)
		{
		    పూర్ణాంకం old_itemlen;
		    char_u *దొరికింది = కనుగొను_కీలకం_అంశం(newval, అంశం_ప్రారంభం,
						       keylen, &old_itemlen);
		    ఒకవేళ (దొరికింది != ఖాళీ)
		    {
			ఒకవేళ (old_itemlen == అంశం_len
				&& STRNCMP(దొరికింది, అంశం_ప్రారంభం,
							 అంశం_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_కీలకం_అంశం(newval, అంశం_ప్రారంభం,
							   keylen, దొరికింది);
			}
			లేకపోతే
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_కీలకం_అంశం(newval, అంశం_ప్రారంభం,
							    keylen, ఖాళీ);
			    ఒకవేళ (op == OP_PREPENDING)
				prepend_అంశం(newval, అంశం_ప్రారంభం, అంశం_len);
			    లేకపోతే
				append_అంశం(newval, అంశం_ప్రారంభం, అంశం_len);
			}
		    }
		    లేకపోతే
		    {
			// New item.
			ఒకవేళ (op == OP_PREPENDING)
			    prepend_అంశం(newval, అంశం_ప్రారంభం, అంశం_len);
			లేకపోతే
			    append_అంశం(newval, అంశం_ప్రారంభం, అంశం_len);
		    }
		}
		లేకపోతే ఒకవేళ (op == OP_REMOVING)
		    remove_కీలకం_అంశం(newval, అంశం_ప్రారంభం, keylen, ఖాళీ);
	    }
	    లేకపోతే
	    {
		ఒకవేళ (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *దొరికింది = కనుగొను_dup_అంశం(newval, అంశం_ప్రారంభం,
							   అంశం_len, P_COMMA);
		    ఒకవేళ (దొరికింది == ఖాళీ)
		    {
			// New item.
			ఒకవేళ (op == OP_PREPENDING)
			    prepend_అంశం(newval, అంశం_ప్రారంభం, అంశం_len);
			లేకపోతే
			    append_అంశం(newval, అంశం_ప్రారంభం, అంశం_len);
		    }
		    // else: exact duplicate — do nothing
		}
		లేకపోతే ఒకవేళ (op == OP_REMOVING)
		{
		    char_u *దొరికింది = కనుగొను_dup_అంశం(newval, అంశం_ప్రారంభం,
							   అంశం_len, P_COMMA);
		    ఒకవేళ (దొరికింది != ఖాళీ)
			remove_comma_అంశం(newval, దొరికింది, అంశం_len);
		}
	    }
	}

	ఒకవేళ (p == ఖాళీ)
	    ఆపు;
	అంశం_ప్రారంభం = p + 1;
    }

    vim_free(newval_copy);

    తిరిగి true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    స్థిర శూన్య
stropt_remove_dupflags(char_u *newval, పూర్ణాంకం flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    అయితే (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	ఒకవేళ (flags & P_ONECOMMA)
	{
	    ఒకవేళ (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != ఖాళీ)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		కొనసాగు;
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != ఖాళీ)
	    {
		STRMOVE(s, s + 1);
		కొనసాగు;
	    }
	}
	++s;
    }
}

/*
 * Get the string value specified for a ":set" command.  The following set
 * options are supported:
 *	set {opt}&
 *	set {opt}<
 *	set {opt}={val}
 *	set {opt}:{val}
 */
    స్థిర char_u *
stropt_get_newval(
    పూర్ణాంకం		nextchar,
    పూర్ణాంకం		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    సమితి_op_T    *op_arg,
    పూర్ణాంకం		flags,
    పూర్ణాంకం		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    సమితి_op_T    op = *op_arg;
    char_u	*save_arg = ఖాళీ;
    char_u	*newval;
    char_u	*s = ఖాళీ;
    char_u	whichwrap[80];

    ఒకవేళ (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    లేకపోతే ఒకవేళ (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    లేకపోతే
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	ఒకవేళ (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	లేకపోతే ఒకవేళ (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	లేకపోతే ఒకవేళ (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	లేకపోతే ఒకవేళ (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_విలువ(origval, &arg, op, flags);
	ఒకవేళ (newval == ఖాళీ)
	    వెళ్ళు పూర్తి;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	ఒకవేళ (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    ఒకవేళ (newval == ఖాళీ)
		వెళ్ళు పూర్తి;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	ఒకవేళ ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	లేకపోతే
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    పూర్ణాంకం len = 0;
	    ఒకవేళ (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (పూర్ణాంకం)STRLEN(newval);
		s = కనుగొను_dup_అంశం(origval, newval, len, flags);

		// do not add if already there
		ఒకవేళ ((op == OP_ADDING || op == OP_PREPENDING) && s != ఖాళీ)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		ఒకవేళ (s == ఖాళీ)
		    s = origval + (పూర్ణాంకం)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    ఒకవేళ (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    లేకపోతే ఒకవేళ (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	ఒకవేళ (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

పూర్తి:
    ఒకవేళ (save_arg != ఖాళీ)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    తిరిగి newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    స్థిర పూర్ణాంకం
do_సమితి_option_string(
	పూర్ణాంకం	    opt_idx,
	పూర్ణాంకం	    opt_flags,
	char_u	    **argp,
	పూర్ణాంకం	    nextchar,
	సమితి_op_T    op_arg,
	long_u	    flags,
	పూర్ణాంకం	    cp_val,
	char_u	    *varp_arg,
	అక్షరం	    *errbuf,
	size_t	    errbuflen,
	పూర్ణాంకం	    *విలువ_checked,
	అక్షరం	    **errmsg)
{
    char_u	*arg = *argp;
    సమితి_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = ఖాళీ; // previous value if *varp
    char_u	*newval;
    char_u	*origval = ఖాళీ;
    char_u	*origval_l = ఖాళీ;
    char_u	*origval_g = ఖాళీ;
#ఒకవేళ defined(FEAT_EVAL)
    char_u	*saved_origval = ఖాళీ;
    char_u	*saved_origval_l = ఖాళీ;
    char_u	*saved_origval_g = ఖాళీ;
    char_u	*saved_newval = ఖాళీ;
#అయితే_ముగింపు

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    ఒకవేళ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((పూర్ణాంకం)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    ఒకవేళ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	ఒకవేళ (((పూర్ణాంకం)options[opt_idx].indir & PV_BOTH)
						  && origval_l == ఖాళీలు_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    ఒకవేళ (((పూర్ణాంకం)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    లేకపోతే
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    ఒకవేళ (newval == ఖాళీ)
	*(char_u **)(varp) = ఖాళీలు_option;

#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_కీలకం
# endif
		      && origval != ఖాళీ && newval != ఖాళీ)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	ఒకవేళ (origval_l != ఖాళీ)
	    saved_origval_l = vim_strsave(origval_l);
	ఒకవేళ (origval_g != ఖాళీ)
	    saved_origval_g = vim_strsave(origval_g);
    }
#అయితే_ముగింపు

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	పూర్ణాంకం	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	ఒకవేళ ((opt_flags & OPT_MODELINE)
#నిర్వచితమైతే HAVE_SANDBOX
	      || sandbox != 0
#అయితే_ముగింపు
	      || (op != OP_NONE && (*p & P_INSECURE)))
	    secure = 1;

	// Handle side effects, and set the global value for ":set" on local
	// options. Note: when setting 'syntax' or 'filetype' autocommands may
	// be triggered that can cause havoc.
	*errmsg = did_సమితి_string_option(
			opt_idx, (char_u **)varp, oldval, newval, errbuf,
			errbuflen, opt_flags, op, విలువ_checked);

	secure = secure_saved;
    }

#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ (*errmsg == ఖాళీ)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#అయితే_ముగింపు

    *argp = arg;
    తిరిగి *errmsg == ఖాళీ ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    స్థిర అక్షరం *
do_సమితి_option_bool(
    పూర్ణాంకం		opt_idx,
    పూర్ణాంకం		opt_flags,
    సమితి_ఉపసర్గ_T ఉపసర్గ,
    long_u	flags,
    char_u	*varp,
    పూర్ణాంకం		nextchar,
    పూర్ణాంకం		afterchar,
    పూర్ణాంకం		cp_val)

{
    varnumber_T	విలువ;

    ఒకవేళ (nextchar == '=' || nextchar == ':')
	తిరిగి e_invalid_వాదం;
    ఒకవేళ (opt_idx < 0 || varp == ఖాళీ)
	తిరిగి ఖాళీ;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    ఒకవేళ (nextchar == '!')
	విలువ = *(పూర్ణాంకం *)(varp) ^ 1;
    లేకపోతే ఒకవేళ (nextchar == '&')
	విలువ = (పూర్ణాంకం)(దీర్ఘ)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    లేకపోతే ఒకవేళ (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	ఒకవేళ ((పూర్ణాంకం *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    విలువ = -1;
	లేకపోతే ఒకవేళ ((పూర్ణాంకం *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    విలువ = -1;
#నిర్వచితమైతే HAVE_FSYNC
	లేకపోతే ఒకవేళ ((పూర్ణాంకం *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    విలువ = -1;
#అయితే_ముగింపు
	లేకపోతే
	    విలువ = *(పూర్ణాంకం *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    లేకపోతే
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	ఒకవేళ (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    తిరిగి e_trailing_characters;
	ఒకవేళ (ఉపసర్గ == ఉపసర్గ_INV)
	    విలువ = *(పూర్ణాంకం *)(varp) ^ 1;
	లేకపోతే
	    విలువ = ఉపసర్గ == ఉపసర్గ_NO ? 0 : 1;
    }

    తిరిగి సమితి_bool_option(opt_idx, varp, (పూర్ణాంకం)విలువ, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    స్థిర అక్షరం *
do_సమితి_option_numeric(
    పూర్ణాంకం		opt_idx,
    పూర్ణాంకం		opt_flags,
    char_u	**argp,
    పూర్ణాంకం		nextchar,
    సమితి_op_T	op,
    long_u	flags,
    పూర్ణాంకం		cp_val,
    char_u	*varp,
    అక్షరం	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		విలువ;
    పూర్ణాంకం			i;
    అక్షరం		*errmsg = ఖాళీ;

    ఒకవేళ (opt_idx < 0 || varp == ఖాళీ)
	తిరిగి ఖాళీ;  // "cannot happen"
		      //
    /*
     * Different ways to set a number option:
     * &	    set to default value
     * <	    set to global value
     * <xx>	    accept special key codes for 'wildchar' or 'wildcharm'
     * ^x	    accept ctrl key codes for 'wildchar' or 'wildcharm'
     * c	    accept any non-digit for 'wildchar' or 'wildcharm'
     * [-]0-9	    set number
     * other	    error
     */
    ++arg;
    ఒకవేళ (nextchar == '&')
	విలువ = (దీర్ఘ)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    లేకపోతే ఒకవేళ (nextchar == '<')
    {
	ఒకవేళ ((దీర్ఘ *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    విలువ = NO_LOCAL_UNDOLEVEL;
	లేకపోతే ఒకవేళ (opt_flags == OPT_LOCAL
		    && ((దీర్ఘ *)varp == &curwin->w_p_siso
		     || (దీర్ఘ *)varp == &curwin->w_p_so
		     || (దీర్ఘ *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    విలువ = -1;
	లేకపోతే
	    విలువ = *(దీర్ఘ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    లేకపోతే ఒకవేళ (((దీర్ఘ *)varp == &p_wc || (దీర్ఘ *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	విలువ = string_గమ్యం_కీలకం(arg, FALSE);
	ఒకవేళ (విలువ == 0)
	{
	    errmsg = e_invalid_వాదం;
	    వెళ్ళు skip;
	}
    }
    లేకపోతే ఒకవేళ (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, ఖాళీ, &i, STR2NR_ALL, &విలువ, ఖాళీ, 0, TRUE, ఖాళీ);
	ఒకవేళ (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_సంఖ్య_required_after_equal;
	    వెళ్ళు skip;
	}
    }
    లేకపోతే
    {
	errmsg = e_సంఖ్య_required_after_equal;
	వెళ్ళు skip;
    }

    ఒకవేళ (op == OP_ADDING)
	విలువ = *(దీర్ఘ *)varp + విలువ;
    లేకపోతే ఒకవేళ (op == OP_PREPENDING)
	విలువ = *(దీర్ఘ *)varp * విలువ;
    లేకపోతే ఒకవేళ (op == OP_REMOVING)
	విలువ = *(దీర్ఘ *)varp - విలువ;

    ఒకవేళ ((దీర్ఘ *)varp == &curwin->w_p_sop && విలువ < -1)
    {
	errmsg = e_invalid_వాదం;
	వెళ్ళు skip;
    }

    errmsg = సమితి_అంకె_option(opt_idx, varp, విలువ, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    తిరిగి errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    స్థిర అక్షరం *
do_సమితి_option_keycode(char_u **argp, char_u *కీలకం_name, పూర్ణాంకం nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    ఒకవేళ (nextchar == '&')
    {
	ఒకవేళ (కూడు_termcap_నమోదు(కీలకం_name, TRUE) == FAIL)
	    తిరిగి e_not_దొరికింది_లోపల_termcap;
    }
    లేకపోతే
    {
	++arg; // jump to after the '=' or ':'
	కొరకు (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    ఒకవేళ (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	కూడు_termcode(కీలకం_name, arg, FALSE);
	*p = nextchar;
    }
    ఒకవేళ (నిండిన_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    తిరిగి ఖాళీ;
}

/*
 * Set an option to a new value.
 */
    స్థిర అక్షరం *
do_సమితి_option_విలువ(
    పూర్ణాంకం		opt_idx,
    పూర్ణాంకం		opt_flags,
    char_u	**argp,
    సమితి_ఉపసర్గ_T ఉపసర్గ,
    సమితి_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*కీలకం_name,
    పూర్ణాంకం		nextchar,
    పూర్ణాంకం		afterchar,
    పూర్ణాంకం		cp_val,
    పూర్ణాంకం		*stopopteval,
    అక్షరం	*errbuf,
    size_t	errbuflen)
{
    పూర్ణాంకం		విలువ_checked = FALSE;
    అక్షరం	*errmsg = ఖాళీ;
    char_u	*arg = *argp;

    ఒకవేళ (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_సమితి_option_bool(opt_idx, opt_flags, ఉపసర్గ, flags, varp,
						nextchar, afterchar, cp_val);
	ఒకవేళ (errmsg != ఖాళీ)
	    వెళ్ళు skip;
    }
    లేకపోతే
    {
	// numeric or string option
	ఒకవేళ (vim_strchr((char_u *)"=:&<", nextchar) == ఖాళీ
		|| ఉపసర్గ != ఉపసర్గ_NONE)
	{
	    errmsg = e_invalid_వాదం;
	    వెళ్ళు skip;
	}

	ఒకవేళ (flags & P_అంకె)
	{
	    // numeric option
	    errmsg = do_సమితి_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    ఒకవేళ (errmsg != ఖాళీ)
		వెళ్ళు skip;
	}
	లేకపోతే ఒకవేళ (opt_idx >= 0)
	{
	    // string option
	    ఒకవేళ (do_సమితి_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&విలువ_checked, &errmsg) == FAIL)
	    {
		ఒకవేళ (errmsg != ఖాళీ)
		    వెళ్ళు skip;
		*stopopteval = TRUE;
		వెళ్ళు skip;
	    }
	}
	లేకపోతే
	{
	    // key code option
	    errmsg = do_సమితి_option_keycode(&arg, కీలకం_name, nextchar);
	    ఒకవేళ (errmsg != ఖాళీ)
		వెళ్ళు skip;
	}
    }

    ఒకవేళ (opt_idx >= 0)
	did_సమితి_option(opt_idx, opt_flags, op == OP_NONE, విలువ_checked);

skip:
    *argp = arg;
    తిరిగి errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    స్థిర అక్షరం *
do_సమితి_option(
    పూర్ణాంకం		opt_flags,
    char_u	**argp,
    char_u	*arg_ప్రారంభం,
    char_u	**startarg,
    పూర్ణాంకం		*did_show,
    పూర్ణాంకం		*stopopteval,
    అక్షరం	*errbuf,
    size_t	errbuflen)
{
    పూర్ణాంకం		opt_idx;
    char_u	*arg;
    సమితి_ఉపసర్గ_T ఉపసర్గ;	// no prefix, "no" prefix or "inv" prefix
    సమితి_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	కీలకం_name[2];
    పూర్ణాంకం		nextchar;	// next non-white char after option name
    పూర్ణాంకం		afterchar;	// character just after option name
    అక్షరం	*errmsg = ఖాళీ;
    పూర్ణాంకం		కీలకం;
    పూర్ణాంకం		len;

    ఉపసర్గ = get_option_ఉపసర్గ(argp);
    arg = *argp;

    // find end of name
    కీలకం = 0;
    ఒకవేళ (parse_option_name(arg, &opt_idx, &len, &కీలకం) == FAIL)
	తిరిగి e_invalid_వాదం;

    // remember character after option name
    afterchar = arg[len];

    ఒకవేళ (లోపల_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	ఒకవేళ (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != ఖాళీ
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    వెళ్ళు skip;
	}
    }
    లేకపోతే
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	అయితే (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    ఒకవేళ (op != OP_NONE)
	len++;

    nextchar = arg[len];

    ఒకవేళ (opt_idx == -1 && కీలకం == 0)	// found a mismatch: skip
    {
	ఒకవేళ (లోపల_vim9script() && arg > arg_ప్రారంభం
		&& vim_strchr((char_u *)"!&<", *arg) != ఖాళీ)
	    errmsg = e_no_white_space_allowed_between_option_and;
	లేకపోతే
	    errmsg = e_unknown_option;
	వెళ్ళు skip;
    }

    ఒకవేళ (opt_idx >= 0)
    {
	ఒకవేళ (options[opt_idx].var == ఖాళీ)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    ఒకవేళ (vim_strchr((char_u *)"=:!&<", nextchar) == ఖాళీ
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    వెళ్ళు skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    లేకపోతే
    {
	flags = P_STRING;
	varp = ఖాళీ;
	ఒకవేళ (కీలకం < 0)
	{
	    కీలకం_name[0] = KEY2TERMCAP0(కీలకం);
	    కీలకం_name[1] = KEY2TERMCAP1(కీలకం);
	}
	లేకపోతే
	{
	    కీలకం_name[0] = KS_కీలకం;
	    కీలకం_name[1] = (కీలకం & 0xff);
	}
    }

    // Make sure the option value can be changed.
    ఒకవేళ (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, ఉపసర్గ) == FAIL)
	వెళ్ళు skip;

    పూర్ణాంకం cp_val = p_cp;
    ఒకవేళ (vim_strchr((char_u *)"?=:!&<", nextchar) != ఖాళీ)
    {
	arg += len;
	ఒకవేళ (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    ఒకవేళ (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    లేకపోతే		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	ఒకవేళ (vim_strchr((char_u *)"?!&<", nextchar) != ఖాళీ
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    వెళ్ళు skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    ఒకవేళ (nextchar == '?'
	    || (ఉపసర్గ == ఉపసర్గ_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == ఖాళీ
		&& !(flags & P_BOOL)))
    {
	// print value
	ఒకవేళ (*did_show)
	    సందేశం_putchar('\n');	    // cursor below last one
	లేకపోతే
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	ఒకవేళ (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#నిర్వచితమైతే FEAT_EVAL
	    ఒకవేళ (p_verbose > 0)
	    {
		// Mention where the option was last set.
		ఒకవేళ (varp == options[opt_idx].var)
		    చివరి_సమితి_సందేశం(options[opt_idx].script_ctx);
		లేకపోతే ఒకవేళ ((పూర్ణాంకం)options[opt_idx].indir & PV_WIN)
		    చివరి_సమితి_సందేశం(curwin->w_p_script_ctx[
			    (పూర్ణాంకం)options[opt_idx].indir & PV_ముసుగు]);
		లేకపోతే ఒకవేళ ((పూర్ణాంకం)options[opt_idx].indir & PV_BUF)
		    చివరి_సమితి_సందేశం(curbuf->b_p_script_ctx[
			    (పూర్ణాంకం)options[opt_idx].indir & PV_ముసుగు]);
	    }
#అయితే_ముగింపు
	}
	లేకపోతే
	{
	    char_u	    *p;

	    p = కనుగొను_termcode(కీలకం_name);
	    ఒకవేళ (p == ఖాళీ)
	    {
		errmsg = e_కీలకం_సంకేతం_not_సమితి;
		వెళ్ళు skip;
	    }
	    లేకపోతే
		(శూన్య)show_one_termcode(కీలకం_name, p, TRUE);
	}
	ఒకవేళ (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    లేకపోతే
    {
	errmsg = do_సమితి_option_విలువ(opt_idx, opt_flags, &arg, ఉపసర్గ, op,
					flags, varp, కీలకం_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    తిరిగి errmsg;
}

/*
 * Parse 'arg' for option settings.
 *
 * 'arg' may be IObuff, but only when no errors can be present and option
 * does not need to be expanded with option_expand().
 * "opt_flags":
 * 0 for ":set"
 * OPT_GLOBAL     for ":setglobal"
 * OPT_LOCAL      for ":setlocal" and a modeline
 * OPT_MODELINE   for a modeline
 * OPT_WINONLY    to only set window-local options
 * OPT_NOWIN	  to skip setting window-local options
 * OPT_ONECOLUMN  do not use multiple columns
 *
 * Returns FAIL if an error is detected, OK otherwise.
 */
    పూర్ణాంకం
do_సమితి(
    char_u	*arg_ప్రారంభం,	// option string (may be written to!)
    పూర్ణాంకం		opt_flags)
{
    char_u	*arg = arg_ప్రారంభం;
    పూర్ణాంకం		i;
    పూర్ణాంకం		did_show = FALSE;   // already showed one value

    ఒకవేళ (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	వెళ్ళు theend;
    }

    అయితే (*arg != NUL)		// loop to process all options
    {
	ఒకవేళ (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    ఒకవేళ (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		సమితి_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    లేకపోతే
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	లేకపోతే ఒకవేళ (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	లేకపోతే
	{
	    పూర్ణాంకం		stopopteval = FALSE;
	    అక్షరం	*errmsg = ఖాళీ;
	    అక్షరం	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_సమితి_option(opt_flags, &arg, arg_ప్రారంభం, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    ఒకవేళ (stopopteval)
		ఆపు;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    కొరకు (i = 0; i < 2 ; ++i)
	    {
		అయితే (*arg != NUL && !VIM_ISWHITE(*arg))
		    ఒకవేళ (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		ఒకవేళ (*arg != '=')
		    ఆపు;
	    }

	    ఒకవేళ (errmsg != ఖాళీ)
	    {
		i = vim_snprintf((అక్షరం *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		ఒకవేళ (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((అక్షరం *)IObuff);	// show error highlighted
		--no_wait_return;

		తిరిగి FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    ఒకవేళ (silent_బహుళకం && did_show)
    {
	// After displaying option values in silent mode.
	silent_బహుళకం = FALSE;
	info_సందేశం = TRUE;	// use mch_msg(), not mch_errmsg()
	సందేశం_putchar('\n');
	cursor_on();		// msg_start() switches it off
	బహిర్గతం_flush();
	silent_బహుళకం = TRUE;
	info_సందేశం = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    తిరిగి OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    శూన్య
did_సమితి_option(
    పూర్ణాంకం	    opt_idx,
    పూర్ణాంకం	    opt_flags,	    // possibly with OPT_MODELINE
    పూర్ణాంకం	    new_విలువ_2,	    // value was replaced completely
    పూర్ణాంకం	    విలువ_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = ఖాళీ;
    పూర్ణాంకం		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_సమితి;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    ఒకవేళ (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    ఒకవేళ (!విలువ_checked && (secure
#నిర్వచితమైతే HAVE_SANDBOX
	    || sandbox != 0
#అయితే_ముగింపు
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	ఒకవేళ (flagsp_local != ఖాళీ)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    లేకపోతే ఒకవేళ (new_విలువ_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	ఒకవేళ (flagsp_local != ఖాళీ)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    పూర్ణాంకం
string_గమ్యం_కీలకం(char_u *arg, పూర్ణాంకం multi_byte)
{
    ఒకవేళ (*arg == '<' && arg[1])
	తిరిగి కనుగొను_కీలకం_option(arg + 1, TRUE);
    ఒకవేళ (*arg == '^' && arg[1])
    {
	పూర్ణాంకం కీలకం = Ctrl_chr(arg[1]);
	ఒకవేళ (కీలకం == 0)		// ^@ is <Nul>
	    కీలకం = K_ZERO;
	తిరిగి కీలకం;
    }
    ఒకవేళ (multi_byte)
	తిరిగి PTR2CHAR(arg);
    తిరిగి *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    శూన్య
did_సమితి_title(శూన్య)
{
    ఒకవేళ (starting != NO_SCREEN
#నిర్వచితమైతే FEAT_GUI
	    && !gui.starting
#అయితే_ముగింపు
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    శూన్య
సమితి_options_bin(
    పూర్ణాంకం		oldval,
    పూర్ణాంకం		newval,
    పూర్ణాంకం		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    ఒకవేళ (newval)
    {
	ఒకవేళ (!oldval)		// switched on
	{
	    ఒకవేళ (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    ఒకవేళ (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	ఒకవేళ (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	ఒకవేళ (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    లేకపోతే ఒకవేళ (oldval)		// switched off
    {
	ఒకవేళ (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	ఒకవేళ (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#ఒకవేళ defined(FEAT_EVAL)
    // Remember where the dependent option were reset
    didset_options_sctx(opt_flags, p_bin_dep_opts);
#అయితే_ముగింపు
}

/*
 * Expand environment variables for some string options.
 * These string options cannot be indirect!
 * If "val" is NULL expand the current value of the option.
 * Return pointer to NameBuff, or NULL when not expanded.
 */
    స్థిర char_u *
option_expand(పూర్ణాంకం opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    ఒకవేళ (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == ఖాళీ)
	తిరిగి ఖాళీ;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    ఒకవేళ (val != ఖాళీ && STRLEN(val) > MAXPATHL)
	తిరిగి ఖాళీ;

    ఒకవేళ (val == ఖాళీ)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    పూర్ణాంకం esc = var == &p_ట్యాగులు || var == &p_మార్గం;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : ఖాళీ, FALSE,
#నిర్వచితమైతే FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#అయితే_ముగింపు
				  ఖాళీ);
    ఒకవేళ (STRCMP(NameBuff, val) == 0)   // they are the same
	తిరిగి ఖాళీ;

    తిరిగి NameBuff;
}

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    స్థిర శూన్య
didset_options(శూన్య)
{
    // initialize the table for 'iskeyword' et.al.
    (శూన్య)init_chartab();

    didset_string_options();

#నిర్వచితమైతే FEAT_SPELL
    (శూన్య)spell_check_msm();
    (శూన్య)spell_check_sps();
    (శూన్య)compile_cap_prog(curwin->w_s);
    (శూన్య)did_సమితి_spell_option();
#అయితే_ముగింపు
    // set cedit_key
    (శూన్య)did_సమితి_cedit(ఖాళీ);
#నిర్వచితమైతే FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_సమితి_breakat(ఖాళీ);
#అయితే_ముగింపు
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    స్థిర శూన్య
didset_options2(శూన్య)
{
    // Initialize the highlight_attr[] table.
    (శూన్య)highlight_మారింది();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (శూన్య)సమితి_listchars_option(curwin, curwin->w_p_lcs, TRUE, ఖాళీ, 0);

    // Parse default for 'fillchars'.
    (శూన్య)సమితి_fillchars_option(curwin, curwin->w_p_fcs, TRUE, ఖాళీ, 0);

#నిర్వచితమైతే FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (శూన్య)did_సమితి_clipboard(ఖాళీ);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (శూన్య)tabstop_సమితి(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (శూన్య)tabstop_సమితి(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#అయితే_ముగింపు
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    శూన్య
check_options(శూన్య)
{
    పూర్ణాంకం		opt_idx;

    కొరకు (opt_idx = 0; options[opt_idx].fullname != ఖాళీ; opt_idx++)
	ఒకవేళ ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != ఖాళీ)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    పూర్ణాంకం
get_పదం_opt_idx(char_u **p)
{
    పూర్ణాంకం opt_idx;

    కొరకు (opt_idx = 1; options[opt_idx].fullname != ఖాళీ; opt_idx++)
	ఒకవేళ (options[opt_idx].var == (char_u *)p)
	    తిరిగి opt_idx;
    తిరిగి -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    పూర్ణాంకం
సమితి_పదం_option_alloced(char_u **p)
{
    పూర్ణాంకం		opt_idx = get_పదం_opt_idx(p);

    ఒకవేళ (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    తిరిగి opt_idx;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    పూర్ణాంకం
was_సమితి_insecurely(win_T *wp, char_u *opt, పూర్ణాంకం opt_flags)
{
    పూర్ణాంకం	    idx = findoption(opt);
    long_u  *flagp;

    ఒకవేళ (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	తిరిగి (*flagp & P_INSECURE) != 0;
    }
    అంతర్గత_error("was_set_insecurely()");
    తిరిగి -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    స్థిర long_u *
insecure_flag(win_T *wp, పూర్ణాంకం opt_idx, పూర్ణాంకం opt_flags)
{
    ఒకవేళ (opt_flags & OPT_LOCAL)
	ఎంపిక ((పూర్ణాంకం)options[opt_idx].indir)
	{
	    స్థితి PV_WRAP:	తిరిగి &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    స్థితి PV_STL:	తిరిగి &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    స్థితి PV_FDE:	తిరిగి &wp->w_p_fde_flags;
	    స్థితి PV_FDT:	తిరిగి &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    స్థితి PV_BEXPR:	తిరిగి &wp->w_బఫరు->b_p_bexpr_flags;
#  endif
	    స్థితి PV_INDE:	తిరిగి &wp->w_బఫరు->b_p_inde_flags;
	    స్థితి PV_FEX:	తిరిగి &wp->w_బఫరు->b_p_fex_flags;
#  ifdef FEAT_కనుగొను_ID
	    స్థితి PV_INEX:	తిరిగి &wp->w_బఫరు->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    స్థితి PV_CPT:	తిరిగి &wp->w_బఫరు->b_p_cpt_flags;
#  endif
# endif
	}
    లేకపోతే
	// For global value of window-local options, use flags in w_allbuf_opt.
	ఎంపిక ((పూర్ణాంకం)options[opt_idx].indir)
	{
	    స్థితి PV_WRAP:	తిరిగి &wp->w_allbuf_opt.wo_wrap_flags;
# ఒకవేళ defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    స్థితి PV_FDE:	తిరిగి &wp->w_allbuf_opt.wo_fde_flags;
	    స్థితి PV_FDT:	తిరిగి &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    తిరిగి &options[opt_idx].flags;
}
#అయితే_ముగింపు

/*
 * Redraw the window title and/or tab page text later.
 */
    శూన్య
redraw_titles(శూన్య)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#అయితే_ముగింపు
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    పూర్ణాంకం
valid_name(char_u *val, అక్షరం *allowed)
{
    char_u *s;

    కొరకు (s = val; *s != NUL; ++s)
	ఒకవేళ (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == ఖాళీ)
	    తిరిగి FALSE;
    తిరిగి TRUE;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    శూన్య
సమితి_option_sctx_idx(పూర్ణాంకం opt_idx, పూర్ణాంకం opt_flags, sctx_T script_ctx)
{
    పూర్ణాంకం		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    పూర్ణాంకం		indir = (పూర్ణాంకం)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    ఒకవేళ (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    ఒకవేళ (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_సంస్కరణ = SCRIPT_సంస్కరణ_VIM9;
    ఒకవేళ (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_సంస్కరణ = SCRIPT_సంస్కరణ_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    ఒకవేళ (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    ఒకవేళ (both || (opt_flags & OPT_LOCAL))
    {
	ఒకవేళ (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_ముసుగు] = new_script_ctx;
	లేకపోతే ఒకవేళ (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_ముసుగు] = new_script_ctx;
	    ఒకవేళ (both)
		// also setting the "all buffers" value
		curwin->w_allbuf_opt.wo_script_ctx[indir & PV_ముసుగు] =
								new_script_ctx;
	}
    }
}

/*
 * Returns true if an option value will be evaluated as a Vim9 script.
 * Checks stored script context for the option.  For options that have values in
 * multiple contexts, opt_flags selects the desired context with OPT_GLOBAL, or
 * OPT_LOCAL, selecting global or buffer/window context, respectively.
 */
    bool
is_option_విలువ_vim9(పూర్ణాంకం opt_idx, పూర్ణాంకం opt_flags)
{
    పూర్ణాంకం		indir = (పూర్ణాంకం)options[opt_idx].indir;
    sctx_T	*sctx = ఖాళీ;

    ఒకవేళ ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    ఒకవేళ ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	ఒకవేళ (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_ముసుగు];
	లేకపోతే ఒకవేళ (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_ముసుగు];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    తిరిగి !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_సంస్కరణ >= SCRIPT_సంస్కరణ_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(అక్షరం *name)
{
    పూర్ణాంకం idx = findoption((char_u *)name);

    ఒకవేళ (idx >= 0)
	తిరిగి &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    తిరిగి ఖాళీ;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    శూన్య
సమితి_పదం_option_sctx_idx(అక్షరం *name, పూర్ణాంకం opt_idx)
{
    char_u  buf[5];
    పూర్ణాంకం	    idx;

    ఒకవేళ (name == ఖాళీ)
	idx = opt_idx;
    లేకపోతే
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    ఒకవేళ (idx >= 0)
	సమితి_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    స్థిర శూన్య
apply_optionset_autocmd(
	పూర్ణాంకం	opt_idx,
	దీర్ఘ	opt_flags,
	దీర్ఘ	oldval,
	దీర్ఘ	oldval_g,
	దీర్ఘ	newval,
	అక్షరం	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    ఒకవేళ (starting || errmsg != ఖాళీ || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	తిరిగి;

    len = vim_snprintf_safelen((అక్షరం *)buf_new, పరిమాణం(buf_new), "%ld", newval);
    సమితి_vim_var_string(VV_OPTION_NEW, buf_new, (పూర్ణాంకం)len);
    len = vim_snprintf_safelen((అక్షరం *)buf_type, పరిమాణం(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    సమితి_vim_var_string(VV_OPTION_TYPE, buf_type, (పూర్ణాంకం)len);

    buf_oldlen = vim_snprintf_safelen((అక్షరం *)buf_old, పరిమాణం(buf_old), "%ld", oldval);
    సమితి_vim_var_string(VV_OPTION_OLD, buf_old, (పూర్ణాంకం)buf_oldlen);

    ఒకవేళ (opt_flags & OPT_LOCAL)
    {
	సమితి_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	సమితి_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (పూర్ణాంకం)buf_oldlen);
    }
    ఒకవేళ (opt_flags & OPT_GLOBAL)
    {
	సమితి_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	సమితి_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (పూర్ణాంకం)buf_oldlen);
    }
    ఒకవేళ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((అక్షరం *)buf_old_global,
	    పరిమాణం(buf_old_global), "%ld", oldval_g);

	సమితి_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	సమితి_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (పూర్ణాంకం)buf_oldlen);
	సమితి_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (పూర్ణాంకం)buf_old_globallen);
    }
    ఒకవేళ (opt_flags & OPT_MODELINE)
    {
	సమితి_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	సమితి_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (పూర్ణాంకం)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    ఖాళీ, FALSE, ఖాళీ);
    reset_v_option_vars();
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    అక్షరం *
did_సమితి_arabic(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	ఒకవేళ (!p_tbidi)
	{
	    // set rightleft mode
	    ఒకవేళ (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		మారింది_కిటికీ_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    ఒకవేళ (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	ఒకవేళ (STRCMP(p_enc, "utf-8") != 0)
	{
	    స్థిర అక్షరం *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    సందేశం_ఆధారం(HL_ATTR(HLF_W));
	    సందేశం_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    సమితి_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = సమితి_option_విలువ((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    లేకపోతే
    {
	// 'arabic' is reset, handle various sub-settings.
	ఒకవేళ (!p_tbidi)
	{
	    // reset rightleft mode
	    ఒకవేళ (curwin->w_p_rl)
	    {
		curwin->w_p_rl = FALSE;
		మారింది_కిటికీ_setting();
	    }

	    // 'arabicshape' isn't reset, it is a global option and
	    // another window may still need it "on".
	}

	// 'delcombine' isn't reset, it is a global option and another
	// window may still want it "on".

# ifdef FEAT_KEYMAP
	// Revert to the default keymap
	curbuf->b_p_iminsert = B_IMODE_NONE;
	curbuf->b_p_imsearch = B_IMODE_USE_చొప్పించు;
# endif
    }

    తిరిగి errmsg;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    అక్షరం *
did_సమితి_autochdir(optset_T *వాదనలు వాడని)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    అక్షరం *
did_సమితి_ballooneval(optset_T *వాదనలు)
{
    ఒకవేళ (balloon_Eval_For_పదం)
	తిరిగి ఖాళీ;

    ఒకవేళ (p_beval && !వాదనలు->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    లేకపోతే ఒకవేళ (!p_beval && వాదనలు->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_BEVAL_పదం)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    అక్షరం *
did_సమితి_balloonevalterm(optset_T *వాదనలు వాడని)
{
    mch_bevalterm_మారింది();
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Process the updated 'binary' option value.
 */
    అక్షరం *
did_సమితి_ద్విమాన(optset_T *వాదనలు)
{
    // when 'bin' is set also set some other options
    సమితి_options_bin(వాదనలు->os_oldval.boolean, curbuf->b_p_bin, వాదనలు->os_flags);
    redraw_titles();

    తిరిగి ఖాళీ;
}

/*
 * Process the updated 'buflisted' option value.
 */
    అక్షరం *
did_సమితి_buflisted(optset_T *వాదనలు)
{
    // when 'buflisted' changes, trigger autocommands
    ఒకవేళ (వాదనలు->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						ఖాళీ, ఖాళీ, TRUE, curbuf);
    తిరిగి ఖాళీ;
}

/*
 * Process the new 'cmdheight' option value.
 */
    అక్షరం *
did_సమితి_cmdheight(optset_T *వాదనలు)
{
    దీర్ఘ old_విలువ = వాదనలు->os_oldval.సంఖ్య;
    అక్షరం *errmsg = ఖాళీ;

    // if p_ch changed value, change the command line height
    ఒకవేళ (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_ch = MIN_CMDHEIGHT;
    }
    ఒకవేళ (p_ch > అడ్డువరుసలు - min_అడ్డువరుసలు() + MIN_CMDHEIGHT)
	p_ch = అడ్డువరుసలు - min_అడ్డువరుసలు() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    ఒకవేళ ((p_ch != old_విలువ
		|| tabline_ఎత్తు() + topframe->fr_ఎత్తు != అడ్డువరుసలు - p_ch)
	    && నిండిన_screen
#నిర్వచితమైతే FEAT_GUI
	    && !gui.starting
#అయితే_ముగింపు
       )
	command_ఎత్తు();

    తిరిగి errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    అక్షరం *
did_సమితి_compatible(optset_T *వాదనలు వాడని)
{
    compatible_సమితి();
    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    అక్షరం *
did_సమితి_conceallevel(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (curwin->w_p_cole < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	curwin->w_p_cole = 0;
    }
    లేకపోతే ఒకవేళ (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_వాదం;
	curwin->w_p_cole = 3;
    }
    ఒకవేళ (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    లేకపోతే ఒకవేళ (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_వాదం;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    తిరిగి errmsg;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    అక్షరం *
did_సమితి_diff(optset_T *వాదనలు వాడని)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    ఒకవేళ (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    అక్షరం *
did_సమితి_eof_eol_fixeol_bomb(optset_T *వాదనలు వాడని)
{
    // redraw the window title and tab page text
    redraw_titles();
    తిరిగి ఖాళీ;
}

/*
 * Process the updated 'equalalways' option value.
 */
    అక్షరం *
did_సమితి_equalalways(optset_T *వాదనలు)
{
    ఒకవేళ (p_ea && !వాదనలు->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    అక్షరం *
did_సమితి_foldcolumn(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (curwin->w_p_fdc < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	curwin->w_p_fdc = 0;
    }
    లేకపోతే ఒకవేళ (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_వాదం;
	curwin->w_p_fdc = 12;
    }
    ఒకవేళ (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    లేకపోతే ఒకవేళ (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_వాదం;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    తిరిగి errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    అక్షరం *
did_సమితి_foldlevel(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    తిరిగి ఖాళీ;
}

/*
 * Process the new 'foldminlines' option value.
 */
    అక్షరం *
did_సమితి_foldminlines(optset_T *వాదనలు వాడని)
{
    foldUpdateAll(curwin);
    తిరిగి ఖాళీ;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    అక్షరం *
did_సమితి_foldnestmax(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_శోధన_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    అక్షరం *
did_సమితి_hlsearch(optset_T *వాదనలు వాడని)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    సమితి_no_hlsearch(FALSE);
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Process the updated 'ignorecase' option value.
 */
    అక్షరం *
did_సమితి_ignorecase(optset_T *వాదనలు వాడని)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    ఒకవేళ (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(HAVE_ఆదానం_పద్ధతి)
/*
 * Process the updated 'imdisable' option value.
 */
    అక్షరం *
did_సమితి_imdisable(optset_T *వాదనలు వాడని)
{
    // Only de-activate it here, it will be enabled when changing mode.
    ఒకవేళ (p_imdisable)
	im_సమితి_చురుకైన(FALSE);
    లేకపోతే ఒకవేళ (State & బహుళకం_చొప్పించు)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_సమితి_చురుకైన(curbuf->b_p_iminsert == B_IMODE_IM);
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Process the new 'iminsert' option value.
 */
    అక్షరం *
did_సమితి_iminsert(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_చివరి)
    {
	errmsg = e_invalid_వాదం;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    ఒకవేళ (termcap_చురుకైన)	// don't do this in the alternate screen
	showmode();
#ఒకవేళ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    స్థితి_redraw_curbuf();
#అయితే_ముగింపు

    తిరిగి errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    అక్షరం *
did_సమితి_imsearch(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_చివరి)
    {
	errmsg = e_invalid_వాదం;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    తిరిగి errmsg;
}

#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    అక్షరం *
did_సమితి_imstyle(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_వాదం;

    తిరిగి errmsg;
}
#అయితే_ముగింపు

/*
 * Process the updated 'insertmode' option value.
 */
    అక్షరం *
did_సమితి_insertmode(optset_T *వాదనలు)
{
    // when 'insertmode' is set from an autocommand need to do work here
    ఒకవేళ (p_im)
    {
	ఒకవేళ ((State & బహుళకం_చొప్పించు) == 0)
	    need_ప్రారంభం_insertmode = TRUE;
	stop_చొప్పించు_బహుళకం = FALSE;
    }
    // only reset if it was set previously
    లేకపోతే ఒకవేళ (వాదనలు->os_oldval.boolean)
    {
	need_ప్రారంభం_insertmode = FALSE;
	stop_చొప్పించు_బహుళకం = TRUE;
	ఒకవేళ (restart_edit != 0 && బహుళకం_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    అక్షరం *
did_సమితి_langnoremap(optset_T *వాదనలు వాడని)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    తిరిగి ఖాళీ;
}

/*
 * Process the updated 'langremap' option value.
 */
    అక్షరం *
did_సమితి_langremap(optset_T *వాదనలు వాడని)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Process the new 'laststatus' option value.
 */
    అక్షరం *
did_సమితి_laststatus(optset_T *వాదనలు వాడని)
{
    చివరి_స్థితి(FALSE);	// (re)set last window status line
    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    అక్షరం *
did_సమితి_linespace(optset_T *వాదనలు వాడని)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    ఒకవేళ (gui.లోపల_use && gui_mch_adjust_charheight() == OK)
	gui_సమితి_shellsize(FALSE, FALSE, RESIZE_VERT);
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Process the updated 'lisp' option value.
 */
    అక్షరం *
did_సమితి_lisp(optset_T *వాదనలు వాడని)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (శూన్య)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    తిరిగి ఖాళీ;
}

/*
 * Process the new 'maxcombine' option value.
 */
    అక్షరం *
did_సమితి_maxcombine(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    లేకపోతే ఒకవేళ (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    తిరిగి ఖాళీ;
}

/*
 * Process the updated 'modifiable' option value.
 */
    అక్షరం *
did_సమితి_modifiable(optset_T *వాదనలు వాడని)
{
    // when 'modifiable' is changed, redraw the window title

#నిర్వచితమైతే FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    ఒకవేళ (curbuf->b_p_ma && (పదం_లోపల_సాధారణ_బహుళకం(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_పదం != ఖాళీ && !పదం_is_ముగిసిన(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	తిరిగి e_cannot_make_terminal_with_running_పని_modifiable;
    }
#అయితే_ముగింపు
    redraw_titles();

    తిరిగి ఖాళీ;
}

/*
 * Process the updated 'modified' option value.
 */
    అక్షరం *
did_సమితి_modified(optset_T *వాదనలు)
{
    ఒకవేళ (!వాదనలు->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_సమితి = !!వాదనలు->os_newval.boolean;
    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    అక్షరం *
did_సమితి_mousehide(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (!p_mh)
	gui_mch_mousehide(FALSE);
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    అక్షరం *
did_సమితి_సంఖ్య_relativenumber(optset_T *వాదనలు వాడని)
{
#ఒకవేళ defined(FEAT_SIGNS) && defined(FEAT_GUI)
    ఒకవేళ (gui.లోపల_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != ఖాళీ)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	ఒకవేళ (!(curwin->w_p_nu && ((పూర్ణాంకం *)వాదనలు->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#అయితే_ముగింపు
    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    అక్షరం *
did_సమితి_numberwidth(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    // 'numberwidth' must be positive
    ఒకవేళ (curwin->w_p_nuw < 1)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	curwin->w_p_nuw = 1;
    }
    లేకపోతే ఒకవేళ (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_వాదం;
	curwin->w_p_nuw = 20;
    }
    ఒకవేళ (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    లేకపోతే ఒకవేళ (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_వాదం;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_పంక్తి_లెక్క = 0; // trigger a redraw

    తిరిగి errmsg;
}
#అయితే_ముగింపు

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    అక్షరం *
did_సమితి_osctimeoutlen(optset_T *వాదనలు)
{
    ఒకవేళ (p_ost < 0)
    {
	p_ost = వాదనలు->os_oldval.సంఖ్య;
	తిరిగి e_వాదం_must_be_ధనాత్మక;
    }

    తిరిగి ఖాళీ;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    అక్షరం *
did_సమితి_paste(optset_T *వాదనలు వాడని)
{
    స్థిర పూర్ణాంకం	old_p_paste = FALSE;
    స్థిర పూర్ణాంకం	save_sm = 0;
    స్థిర పూర్ణాంకం	save_sta = 0;
    స్థిర పూర్ణాంకం	save_ru = 0;
#నిర్వచితమైతే FEAT_RIGHTLEFT
    స్థిర పూర్ణాంకం	save_ri = 0;
    స్థిర పూర్ణాంకం	save_hkmap = 0;
#అయితే_ముగింపు
    buf_T	*buf;

    ఒకవేళ (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	ఒకవేళ (!old_p_paste)
	{
	    // save options for each buffer
	    FOR_ALL_BUFFERS(buf)
	    {
		buf->b_p_tw_nopaste = buf->b_p_tw;
		buf->b_p_wm_nopaste = buf->b_p_wm;
		buf->b_p_sts_nopaste = buf->b_p_sts;
		buf->b_p_ai_nopaste = buf->b_p_ai;
		buf->b_p_et_nopaste = buf->b_p_et;
#నిర్వచితమైతే FEAT_VARTABS
		ఒకవేళ (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != ఖాళీలు_option
					   ? vim_strsave(buf->b_p_vsts) : ఖాళీ;
#అయితే_ముగింపు
	    }

	    // save global options
	    save_sm = p_sm;
	    save_sta = p_sta;
	    save_ru = p_ru;
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    save_ri = p_ri;
	    save_hkmap = p_hkmap;
#అయితే_ముగింపు
	    // save global values for local buffer options
	    p_ai_nopaste = p_ai;
	    p_et_nopaste = p_et;
	    p_sts_nopaste = p_sts;
	    p_tw_nopaste = p_tw;
	    p_wm_nopaste = p_wm;
#నిర్వచితమైతే FEAT_VARTABS
	    ఒకవేళ (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != ఖాళీలు_option
						  ? vim_strsave(p_vsts) : ఖాళీ;
#అయితే_ముగింపు
	}

	// Always set the option values, also when 'paste' is set when it is
	// already on.  Set options for each buffer.
	FOR_ALL_BUFFERS(buf)
	{
	    buf->b_p_tw = 0;	    // textwidth is 0
	    buf->b_p_wm = 0;	    // wrapmargin is 0
	    buf->b_p_sts = 0;	    // softtabstop is 0
	    buf->b_p_ai = 0;	    // no auto-indent
	    buf->b_p_et = 0;	    // no expandtab
#నిర్వచితమైతే FEAT_VARTABS
	    ఒకవేళ (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = ఖాళీలు_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#అయితే_ముగింపు
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	ఒకవేళ (p_ru)
	    స్థితి_redraw_all();    // redraw to remove the ruler
	p_ru = 0;		    // no ruler
#నిర్వచితమైతే FEAT_RIGHTLEFT
	p_ri = 0;		    // no reverse insert
	p_hkmap = 0;		    // no Hebrew keyboard
#అయితే_ముగింపు
	// set global values for local buffer options
	p_tw = 0;
	p_wm = 0;
	p_sts = 0;
	p_ai = 0;
	p_et = 0;
#నిర్వచితమైతే FEAT_VARTABS
	ఒకవేళ (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = ఖాళీలు_option;
#అయితే_ముగింపు
    }

    // Paste switched from on to off: Restore saved values.
    లేకపోతే ఒకవేళ (old_p_paste)
    {
	// restore options for each buffer
	FOR_ALL_BUFFERS(buf)
	{
	    buf->b_p_tw = buf->b_p_tw_nopaste;
	    buf->b_p_wm = buf->b_p_wm_nopaste;
	    buf->b_p_sts = buf->b_p_sts_nopaste;
	    buf->b_p_ai = buf->b_p_ai_nopaste;
	    buf->b_p_et = buf->b_p_et_nopaste;
#నిర్వచితమైతే FEAT_VARTABS
	    ఒకవేళ (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : ఖాళీలు_option;
	    vim_free(buf->b_p_vsts_array);
	    ఒకవేళ (buf->b_p_vsts && buf->b_p_vsts != ఖాళీలు_option)
		(శూన్య)tabstop_సమితి(buf->b_p_vsts, &buf->b_p_vsts_array);
	    లేకపోతే
		buf->b_p_vsts_array = ఖాళీ;
#అయితే_ముగింపు
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	ఒకవేళ (p_ru != save_ru)
	    స్థితి_redraw_all();    // redraw to draw the ruler
	p_ru = save_ru;
#నిర్వచితమైతే FEAT_RIGHTLEFT
	p_ri = save_ri;
	p_hkmap = save_hkmap;
#అయితే_ముగింపు
	// set global values for local buffer options
	p_ai = p_ai_nopaste;
	p_et = p_et_nopaste;
	p_sts = p_sts_nopaste;
	p_tw = p_tw_nopaste;
	p_wm = p_wm_nopaste;
#నిర్వచితమైతే FEAT_VARTABS
	ఒకవేళ (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : ఖాళీలు_option;
#అయితే_ముగింపు
    }

    old_p_paste = p_paste;

#ఒకవేళ defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#అయితే_ముగింపు

    తిరిగి ఖాళీ;
}


#నిర్వచితమైతే FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    అక్షరం *
did_సమితి_previewwindow(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (!curwin->w_p_pvw)
	తిరిగి ఖాళీ;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	ఒకవేళ (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    తిరిగి e_preview_కిటికీ_already_exists;
	}

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    అక్షరం *
did_సమితి_pyxversion(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_వాదం;

    తిరిగి errmsg;
}
#అయితే_ముగింపు

/*
 * Process the updated 'readonly' option value.
 */
    అక్షరం *
did_సమితి_readonly(optset_T *వాదనలు)
{
    // when 'readonly' is reset globally, also reset readonlymode
    ఒకవేళ (!curbuf->b_p_ro && (వాదనలు->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    ఒకవేళ (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    తిరిగి ఖాళీ;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    అక్షరం *
did_సమితి_scrollbind(optset_T *వాదనలు వాడని)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    ఒకవేళ (!curwin->w_p_scb)
	తిరిగి ఖాళీ;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    తిరిగి ఖాళీ;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    అక్షరం *
did_సమితి_maxsearchcount(optset_T *వాదనలు వాడని)
{
    అక్షరం	*errmsg = ఖాళీ;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#నిర్వచించు MAX_శోధన_లెక్క 9999

    ఒకవేళ (p_msc <= 0)
	errmsg = e_వాదం_must_be_ధనాత్మక;
    లేకపోతే ఒకవేళ (p_msc > MAX_శోధన_లెక్క)
	errmsg = e_invalid_వాదం;

    ఒకవేళ (errmsg != ఖాళీ)
	p_msc = 99;

    తిరిగి errmsg;
#నిర్వచనం_రద్దు MAX_శోధన_లెక్క
}

#ఒకవేళ defined(BACKSLASH_లోపల_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    అక్షరం *
did_సమితి_shellslash(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    లేకపోతే
    {
	psepc = '\\';
	psepcN = '/';
	pseps[0] = '\\';
    }

    // need to adjust the file name arguments and buffer names.
    buflist_slash_adjust();
    alist_slash_adjust();
# ifdef FEAT_EVAL
    scriptnames_slash_adjust();
# endif
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    అక్షరం *
did_సమితి_shiftwidth_tabstop(optset_T *వాదనలు)
{
    దీర్ఘ	*pp = (దీర్ఘ *)వాదనలు->os_varp;
    అక్షరం	*errmsg = ఖాళీ;

    ఒకవేళ (curbuf->b_p_ts <= 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	curbuf->b_p_ts = 8;
    }
    లేకపోతే ఒకవేళ (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_వాదం;
	curbuf->b_p_ts = 8;
    }
    ఒకవేళ (p_ts <= 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_ts = 8;
    }
    లేకపోతే ఒకవేళ (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_వాదం;
	p_ts = 8;
    }

    ఒకవేళ (curbuf->b_p_sw < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
#నిర్వచితమైతే FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_లెక్క(curbuf->b_p_vts_array) > 0
		       ? tabstop_మొదటి(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#లేకపోతే
	curbuf->b_p_sw = curbuf->b_p_ts;
#అయితే_ముగింపు
    }
    ఒకవేళ (p_sw < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
#నిర్వచితమైతే FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_లెక్క(curbuf->b_p_vts_array) > 0
	     ? tabstop_మొదటి(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#లేకపోతే
	p_sw = curbuf->b_p_ts;
#అయితే_ముగింపు
    }

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#అయితే_ముగింపు
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    ఒకవేళ (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    తిరిగి errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    అక్షరం *
did_సమితి_showtabline(optset_T *వాదనలు వాడని)
{
    shell_new_అడ్డువరుసలు();	// recompute window positions and heights
    తిరిగి ఖాళీ;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    అక్షరం *
did_సమితి_smoothscroll(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    అక్షరం *
did_సమితి_spell(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (curwin->w_p_spell)
	తిరిగి parse_spelllang(curwin);

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Process the updated 'swapfile' option value.
 */
    అక్షరం *
did_సమితి_swapfile(optset_T *వాదనలు వాడని)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    ఒకవేళ (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    లేకపోతే
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_TERMGUICOLORS)
    అక్షరం *
did_సమితి_termguicolors(optset_T *వాదనలు వాడని)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    ఒకవేళ (
#  ifdef VIMDLL
	    !gui.లోపల_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_సమితి = TRUE;
	p_tgc = 0;
	తిరిగి e_24_బిట్టు_colors_are_not_supported_on_this_environment;
    }
    ఒకవేళ (is_పదం_win32())
	మార్పిడి_tcap();
# endif
# ifdef FEAT_GUI
    ఒకవేళ (!gui.లోపల_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    ఒకవేళ (is_పదం_win32())
    {
	control_console_color_rgb();
	సమితి_termname(T_NAME);
	init_highlight(TRUE, FALSE);
    }
# endif
# ifdef FEAT_TERMINAL
    పదం_update_colors_all();
    పదం_update_palette_all();
    పదం_update_hlfwin_all();
# endif
    p_tgc_సమితి = TRUE;

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

    అక్షరం *
did_సమితి_termsync(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (!p_tsy)
	పదం_సమితి_sync_output(పదం_SYNC_OUTPUT_OFF);
    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    అక్షరం *
did_సమితి_termwinscroll(optset_T *వాదనలు)
{
    దీర్ఘ	*pp = (దీర్ఘ *)వాదనలు->os_varp;
    అక్షరం	*errmsg = ఖాళీ;

    ఒకవేళ (*pp < 1)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	*pp = 1;
    }

    తిరిగి errmsg;
}
#అయితే_ముగింపు

/*
 * Process the updated 'terse' option value.
 */
    అక్షరం *
did_సమితి_terse(optset_T *వాదనలు వాడని)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_శోధన);

    // insert 's' in p_shm
    ఒకవేళ (p_terse && p == ఖాళీ)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	సమితి_string_option_ప్రత్యక్ష((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    లేకపోతే ఒకవేళ (!p_terse && p != ఖాళీ)
	STRMOVE(p, p + 1);
    తిరిగి ఖాళీ;
}

/*
 * Process the updated 'textauto' option value.
 */
    అక్షరం *
did_సమితి_textauto(optset_T *వాదనలు)
{
    // when 'textauto' is set or reset also change 'fileformats'
    సమితి_string_option_ప్రత్యక్ష((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | వాదనలు->os_flags, 0);

    తిరిగి ఖాళీ;
}

/*
 * Process the updated 'textmode' option value.
 */
    అక్షరం *
did_సమితి_textmode(optset_T *వాదనలు)
{
    // when 'textmode' is set or reset also change 'fileformat'
    సమితి_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, వాదనలు->os_flags);

    తిరిగి ఖాళీ;
}

/*
 * Process the new 'textwidth' option value.
 */
    అక్షరం *
did_సమితి_textwidth(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (curbuf->b_p_tw < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	curbuf->b_p_tw = 0;
    }
    ఒకవేళ (p_tw < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_tw = 0;
    }
#నిర్వచితమైతే FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(ఖాళీ, wp);
    }
#అయితే_ముగింపు

    తిరిగి errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    అక్షరం *
did_సమితి_title_icon(optset_T *వాదనలు వాడని)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_సమితి_title();
    తిరిగి ఖాళీ;
}

/*
 * Process the new 'titlelen' option value.
 */
    అక్షరం *
did_సమితి_titlelen(optset_T *వాదనలు)
{
    దీర్ఘ old_విలువ = వాదనలు->os_oldval.సంఖ్య;
    అక్షరం *errmsg = ఖాళీ;

    // if 'titlelen' has changed, redraw the title
    ఒకవేళ (p_titlelen < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_titlelen = 85;
    }
    ఒకవేళ (starting != NO_SCREEN && old_విలువ != p_titlelen)
	need_maketitle = TRUE;

    తిరిగి errmsg;
}

#ఒకవేళ defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    అక్షరం *
did_సమితి_undofile(optset_T *వాదనలు)
{
    // Only take action when the option was set.
    ఒకవేళ (!curbuf->b_p_udf && !p_udf)
	తిరిగి ఖాళీ;

    // When reset we do not delete the undo file, the option may be set again
    // without making any changes in between.
    char_u	hash[UNDO_HASH_కొలత];
    buf_T	*save_curbuf = curbuf;

    FOR_ALL_BUFFERS(curbuf)
    {
	// When 'undofile' is set globally: for every buffer, otherwise
	// only for the current buffer: Try to read in the undofile,
	// if one exists, the buffer wasn't changed and the buffer was
	// loaded
	ఒకవేళ ((curbuf == save_curbuf
		    || (వాదనలు->os_flags & OPT_GLOBAL)
		    || వాదనలు->os_flags == 0)
		&& !curbuf_Is_మారింది() && curbuf->b_ml.ml_mfp != ఖాళీ)
	{
# ifdef FEAT_CRYPT
	    ఒకవేళ (crypt_పద్ధతి_is_sodium(crypt_get_పద్ధతి_nr(curbuf)))
		కొనసాగు;
# endif
	    u_compute_hash(hash);
	    u_read_undo(ఖాళీ, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Process the new global 'undolevels' option value.
 */
    స్థిర శూన్య
update_global_undolevels(దీర్ఘ విలువ, దీర్ఘ old_విలువ)
{
    // sync undo before 'undolevels' changes

    // use the old value, otherwise u_sync() may not work properly
    p_ul = old_విలువ;
    u_sync(TRUE);
    p_ul = విలువ;
}

/*
 * Process the new buffer local 'undolevels' option value.
 */
    స్థిర శూన్య
update_buflocal_undolevels(దీర్ఘ విలువ, దీర్ఘ old_విలువ)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_విలువ;
    u_sync(TRUE);
    curbuf->b_p_ul = విలువ;
}

/*
 * Process the new 'undolevels' option value.
 */
    అక్షరం *
did_సమితి_undolevels(optset_T *వాదనలు)
{
    దీర్ఘ *pp = (దీర్ఘ *)వాదనలు->os_varp;

    ఒకవేళ (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(వాదనలు->os_newval.సంఖ్య,
						       వాదనలు->os_oldval.సంఖ్య);
    లేకపోతే ఒకవేళ (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(వాదనలు->os_newval.సంఖ్య,
						       వాదనలు->os_oldval.సంఖ్య);

    తిరిగి ఖాళీ;
}

/*
 * Process the new 'updatecount' option value.
 */
    అక్షరం *
did_సమితి_updatecount(optset_T *వాదనలు)
{
    దీర్ఘ old_విలువ = వాదనలు->os_oldval.సంఖ్య;
    అక్షరం *errmsg = ఖాళీ;

    // when 'updatecount' changes from zero to non-zero, open swap files
    ఒకవేళ (p_uc < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_uc = 100;
    }
    ఒకవేళ (p_uc && !old_విలువ)
	ml_open_files();

    తిరిగి errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    అక్షరం *
did_సమితి_weirdinvert(optset_T *వాదనలు)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    ఒకవేళ (p_wiv && !వాదనలు->os_oldval.boolean)
	T_XS = (char_u *)"y";
    లేకపోతే ఒకవేళ (!p_wiv && వాదనలు->os_oldval.boolean)
	T_XS = ఖాళీలు_option;
    p_wiv = (*T_XS != NUL);

    తిరిగి ఖాళీ;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    అక్షరం *
did_సమితి_wildchar(optset_T *వాదనలు)
{
    దీర్ఘ c = *(దీర్ఘ *)వాదనలు->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    ఒకవేళ (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

/*
 * Process the new 'window' option value.
 */
    అక్షరం *
did_సమితి_కిటికీ(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (p_కిటికీ < 1)
	p_కిటికీ = 1;
    లేకపోతే ఒకవేళ (p_కిటికీ >= అడ్డువరుసలు)
	p_కిటికీ = అడ్డువరుసలు - 1;
    తిరిగి ఖాళీ;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    అక్షరం *
did_సమితి_winheight_helpheight(optset_T *వాదనలు)
{
    దీర్ఘ	*pp = (దీర్ఘ *)వాదనలు->os_varp;
    అక్షరం	*errmsg = ఖాళీ;

    ఒకవేళ (p_wh < 1)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_wh = 1;
    }
    ఒకవేళ (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    ఒకవేళ (p_hh < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_hh = 0;
    }

    // Change window height NOW
    ఒకవేళ (!ONE_కిటికీ)
    {
	ఒకవేళ (pp == &p_wh && curwin->w_ఎత్తు < p_wh)
	    win_setheight((పూర్ణాంకం)p_wh);
	ఒకవేళ (pp == &p_hh && curbuf->b_help && curwin->w_ఎత్తు < p_hh)
	    win_setheight((పూర్ణాంకం)p_hh);
    }

    తిరిగి errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    అక్షరం *
did_సమితి_winminheight(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (p_wmh < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_wmh = 0;
    }
    ఒకవేళ (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    తిరిగి errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    అక్షరం *
did_సమితి_winminwidth(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (p_wmw < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_wmw = 0;
    }
    ఒకవేళ (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    తిరిగి errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    అక్షరం *
did_సమితి_winwidth(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (p_wiw < 1)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_wiw = 1;
    }
    ఒకవేళ (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    ఒకవేళ (!ONE_కిటికీ && curwin->w_వెడల్పు < p_wiw)
	win_setwidth((పూర్ణాంకం)p_wiw);

    తిరిగి errmsg;
}

#ఒకవేళ defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    అక్షరం *
did_సమితి_wlsteal(optset_T *వాదనలు వాడని)
{
    clip_reset_wayland();

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    అక్షరం *
did_సమితి_wltimeoutlen(optset_T *వాదనలు)
{
    ఒకవేళ (p_wtm < 0)
    {
	p_wtm = వాదనలు->os_oldval.సంఖ్య;
	తిరిగి e_వాదం_must_be_ధనాత్మక;
    }

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Process the updated 'wrap' option value.
 */
    అక్షరం *
did_సమితి_wrap(optset_T *వాదనలు వాడని)
{
    // Set w_leftcol or w_skipcol to zero.
    ఒకవేళ (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    లేకపోతే
	curwin->w_skipcol = 0;

    తిరిగి ఖాళీ;
}

#నిర్వచితమైతే FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    అక్షరం *
did_సమితి_xhistory(optset_T *వాదనలు)
{
    పూర్ణాంకం is_p_chi = (దీర్ఘ*)వాదనలు->os_varp == &p_chi;
    పూర్ణాంకం err;
    దీర్ఘ *arg = (is_p_chi) ? &p_chi :(దీర్ఘ*)వాదనలు->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    ఒకవేళ (*arg < 1)
    {
	*arg = వాదనలు->os_oldval.సంఖ్య;
	తిరిగి e_cannot_have_negative_or_zero_సంఖ్య_యొక్క_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    ఒకవేళ (*arg > 100)
    {
	*arg = వాదనలు->os_oldval.సంఖ్య;
	తిరిగి e_cannot_have_మరిన్ని_than_hundred_quickfix;
    }

    ఒకవేళ (is_p_chi)
	err = qf_resize_దొంతర(*arg);
    లేకపోతే
	err = ll_resize_దొంతర(curwin, *arg);

    ఒకవేళ (err == FAIL)
	తిరిగి e_failed_resizing_quickfix_దొంతర;

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    స్థిర అక్షరం *
సమితి_bool_option(
    పూర్ణాంకం		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    పూర్ణాంకం		విలువ,			// new value
    పూర్ణాంకం		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    పూర్ణాంకం		old_విలువ = *(పూర్ణాంకం *)varp;
#ఒకవేళ defined(FEAT_EVAL)
    పూర్ణాంకం		old_global_విలువ = 0;
#అయితే_ముగింపు
    అక్షరం	*errmsg = ఖాళీ;

    // Disallow changing some options from secure mode
    ఒకవేళ ((secure
#నిర్వచితమైతే HAVE_SANDBOX
		|| sandbox != 0
#అయితే_ముగింపు
		) && (options[opt_idx].flags & P_SECURE))
	తిరిగి e_not_allowed_ఇక్కడ;

#ఒకవేళ defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    ఒకవేళ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_విలువ = *(పూర్ణాంకం *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#అయితే_ముగింపు

    *(పూర్ణాంకం *)varp = విలువ;	    // set the new value
#నిర్వచితమైతే FEAT_EVAL
    // Remember where the option was set.
    సమితి_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

    // May set global value for local option.
    ఒకవేళ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(పూర్ణాంకం *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = విలువ;

    // Handle side effects of changing a bool option.
    ఒకవేళ (options[opt_idx].opt_did_సమితి_cb != ఖాళీ)
    {
	optset_T వాదనలు;

	CLEAR_క్షేత్రం(వాదనలు);
	వాదనలు.os_varp = varp;
	వాదనలు.os_flags = opt_flags;
	వాదనలు.os_oldval.boolean = old_విలువ;
	వాదనలు.os_newval.boolean = విలువ;
	వాదనలు.os_errbuf = ఖాళీ;
	errmsg = options[opt_idx].opt_did_సమితి_cb(&వాదనలు);
	ఒకవేళ (errmsg != ఖాళీ)
	    తిరిగి errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_సమితి;

#ఒకవేళ defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(దీర్ఘ)(old_విలువ ? TRUE : FALSE),
				(దీర్ఘ)(old_global_విలువ ? TRUE : FALSE),
				(దీర్ఘ)(విలువ ? TRUE : FALSE), ఖాళీ);
#అయితే_ముగింపు

    comp_నిలువు();			    // in case 'ruler' or 'showcmd' changed

    ఒకవేళ (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_సమితి_curswant = true;

    ఒకవేళ ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    తిరిగి errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    స్థిర అక్షరం *
check_అంకె_option_bounds(
    దీర్ఘ	*pp,
    దీర్ఘ	old_విలువ,
    దీర్ఘ	old_అడ్డువరుసలు_2,
    దీర్ఘ	old_Columns,
    అక్షరం	*errbuf,
    size_t	errbuflen,
    అక్షరం	*errmsg)
{
    ఒకవేళ (అడ్డువరుసలు < min_అడ్డువరుసలు_for_all_tabpages() && నిండిన_screen)
    {
	ఒకవేళ (errbuf != ఖాళీ)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_కనిష్ఠ_nr_పంక్తులు), min_అడ్డువరుసలు_for_all_tabpages());
	    errmsg = errbuf;
	}
	అడ్డువరుసలు = min_అడ్డువరుసలు_for_all_tabpages();
    }
    ఒకవేళ (Columns < MIN_COLUMNS && నిండిన_screen)
    {
	ఒకవేళ (errbuf != ఖాళీ)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_కనిష్ఠ_nr_columns), MIN_COLUMNS);
	    errmsg = errbuf;
	}
	Columns = MIN_COLUMNS;
    }
    హద్దు_screen_కొలత();

    // If the screen (shell) height has been changed, assume it is the
    // physical screenheight.
    ఒకవేళ (old_అడ్డువరుసలు_2 != అడ్డువరుసలు || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	ఒకవేళ (updating_screen)
	    *pp = old_విలువ;
	లేకపోతే ఒకవేళ (నిండిన_screen
#నిర్వచితమైతే FEAT_GUI
		&& !gui.starting
#అయితే_ముగింపు
		)
	    సమితి_shellsize((పూర్ణాంకం)Columns, (పూర్ణాంకం)అడ్డువరుసలు, TRUE);
	లేకపోతే
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    ఒకవేళ (cmdline_అడ్డువరుస > అడ్డువరుసలు - p_ch && అడ్డువరుసలు > p_ch)
		cmdline_అడ్డువరుస = అడ్డువరుసలు - p_ch;
	}
	ఒకవేళ (p_కిటికీ >= అడ్డువరుసలు || !option_was_సమితి((char_u *)"window"))
	    p_కిటికీ = అడ్డువరుసలు - 1;
    }

    ఒకవేళ (p_tm < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_tm = 0;
    }
    ఒకవేళ ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_ఎత్తు
		    && curwin->w_ఎత్తు > 0))
	    && నిండిన_screen)
    {
	ఒకవేళ (pp == &(curwin->w_p_scr))
	{
	    ఒకవేళ (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_కొలత;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	లేకపోతే ఒకవేళ (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	లేకపోతే // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_ఎత్తు;
    }
    ఒకవేళ (p_hi < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_hi = 0;
    }
    లేకపోతే ఒకవేళ (p_hi > 10000)
    {
	errmsg = e_invalid_వాదం;
	p_hi = 10000;
    }
    ఒకవేళ (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_వాదం;
	p_re = 0;
    }
    ఒకవేళ (p_report < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_report = 1;
    }
    ఒకవేళ ((p_sj < -100 || p_sj >= అడ్డువరుసలు) && నిండిన_screen)
    {
	ఒకవేళ (అడ్డువరుసలు != old_అడ్డువరుసలు_2)	// Rows changed, just adjust p_sj
	    p_sj = అడ్డువరుసలు / 2;
	లేకపోతే
	{
	    errmsg = e_invalid_scroll_కొలత;
	    p_sj = 1;
	}
    }
    ఒకవేళ (p_so < 0 && నిండిన_screen)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_so = 0;
    }
    ఒకవేళ (p_sop < 0 && నిండిన_screen)
    {
	errmsg = e_invalid_వాదం;
	p_sop = 0;
    }
    ఒకవేళ (p_siso < 0 && నిండిన_screen)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_siso = 0;
    }
    ఒకవేళ (p_cwh < 1)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_cwh = 1;
    }
    ఒకవేళ (p_ut < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_ut = 2000;
    }
    ఒకవేళ (p_ss < 0)
    {
	errmsg = e_వాదం_must_be_ధనాత్మక;
	p_ss = 0;
    }

    తిరిగి errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    స్థిర అక్షరం *
సమితి_అంకె_option(
    పూర్ణాంకం		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    దీర్ఘ	విలువ,			// new value
    అక్షరం	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    పూర్ణాంకం		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    అక్షరం	*errmsg = ఖాళీ;
    దీర్ఘ	old_విలువ = *(దీర్ఘ *)varp;
#ఒకవేళ defined(FEAT_EVAL)
    దీర్ఘ	old_global_విలువ = 0;	// only used when setting a local and
					// global option
#అయితే_ముగింపు
    దీర్ఘ	old_అడ్డువరుసలు_2 = అడ్డువరుసలు;	// remember old Rows
    దీర్ఘ	old_Columns = Columns;	// remember old Columns
    దీర్ఘ	*pp = (దీర్ఘ *)varp;

    // Disallow changing some options from secure mode.
    ఒకవేళ ((secure
#నిర్వచితమైతే HAVE_SANDBOX
		|| sandbox != 0
#అయితే_ముగింపు
		) && (options[opt_idx].flags & P_SECURE))
	తిరిగి e_not_allowed_ఇక్కడ;

#ఒకవేళ defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    ఒకవేళ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_విలువ = *(దీర్ఘ *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#అయితే_ముగింపు

    *pp = విలువ;
#నిర్వచితమైతే FEAT_EVAL
    // Remember where the option was set.
    సమితి_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

    // May set global value for local option.
    ఒకవేళ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(దీర్ఘ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = విలువ;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    ఒకవేళ (options[opt_idx].opt_did_సమితి_cb != ఖాళీ)
    {
	optset_T వాదనలు;

	CLEAR_క్షేత్రం(వాదనలు);
	వాదనలు.os_varp = varp;
	వాదనలు.os_flags = opt_flags;
	వాదనలు.os_oldval.సంఖ్య = old_విలువ;
	వాదనలు.os_newval.సంఖ్య = విలువ;
	వాదనలు.os_errbuf = ఖాళీ;
	errmsg = options[opt_idx].opt_did_సమితి_cb(&వాదనలు);
    }

    // Check the bounds for numeric options here
    errmsg = check_అంకె_option_bounds(pp, old_విలువ, old_అడ్డువరుసలు_2, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_సమితి;

#ఒకవేళ defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_విలువ, old_global_విలువ,
								విలువ, errmsg);
#అయితే_ముగింపు

    comp_నిలువు();			    // in case 'columns' or 'ls' changed

    ఒకవేళ (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_సమితి_curswant = true;

    ఒకవేళ ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    తిరిగి errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    శూన్య
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    పూర్ణాంకం		doclear = (flags & P_RCLR) == P_RCLR;
    పూర్ణాంకం		all = ((flags & P_RALL) == P_RALL || doclear);

    ఒకవేళ ((flags & P_RSTAT) || all)	// mark all status lines dirty
	స్థితి_redraw_all();

    ఒకవేళ ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	ఒకవేళ (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	లేకపోతే
	    మారింది_కిటికీ_setting();
    }
    ఒకవేళ (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    ఒకవేళ (doclear)
	redraw_all_later(UPD_CLEAR);
    లేకపోతే ఒకవేళ (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    పూర్ణాంకం
findoption(char_u *arg)
{
    పూర్ణాంకం		    opt_idx;
    అక్షరం	    *s, *p;
    స్థిర పొట్టి    శీఘ్ర_tab[27] = {0, 0};	// quick access table
    పూర్ణాంకం		    is_పదం_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    ఒకవేళ (శీఘ్ర_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	పూర్ణాంకం చివరి_opt_idx;
	కొరకు (చివరి_opt_idx = 0; options[చివరి_opt_idx].fullname != ఖాళీ;
		చివరి_opt_idx++)
	    ;
	కొరకు (పూర్ణాంకం tab_idx = 1; tab_idx < 27; tab_idx++)
	    శీఘ్ర_tab[tab_idx] = చివరి_opt_idx;

	p = options[0].fullname;
	కొరకు (opt_idx = 1; opt_idx < చివరి_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    ఒకవేళ (s[0] != p[0])
	    {
		ఒకవేళ (s[0] == 't' && s[1] == '_')
		    శీఘ్ర_tab[26] = opt_idx;
		లేకపోతే
		    శీఘ్ర_tab[Char_Ord_దిగువ(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    ఒకవేళ (arg[0] < 'a' || arg[0] > 'z')
	తిరిగి -1;

    is_పదం_opt = (arg[0] == 't' && arg[1] == '_');
    ఒకవేళ (is_పదం_opt)
	opt_idx = శీఘ్ర_tab[26];
    లేకపోతే
	opt_idx = శీఘ్ర_tab[Char_Ord_దిగువ(arg[0])];
    కొరకు (; (s = options[opt_idx].fullname) != ఖాళీ && s[0] == arg[0]; opt_idx++)
    {
	ఒకవేళ (STRCMP(arg, s) == 0)		    // match full name
	    ఆపు;
    }
    ఒకవేళ (s != ఖాళీ && s[0] != arg[0])
	s = ఖాళీ;
    ఒకవేళ (s == ఖాళీ && !is_పదం_opt)
    {
	opt_idx = శీఘ్ర_tab[Char_Ord_దిగువ(arg[0])];
	కొరకు ( ; options[opt_idx].fullname != ఖాళీ; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    ఒకవేళ (s != ఖాళీ && STRCMP(arg, s) == 0)   // match short name
		ఆపు;
	    s = ఖాళీ;
	}
    }
    ఒకవేళ (s == ఖాళీ)
	opt_idx = -1;
    తిరిగి opt_idx;
}

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
	|| defined(FEAT_SPELL)
/*
 * Get the value for an option.
 *
 * Returns:
 * Number option: gov_number, *numval gets value.
 * Toggle option: gov_bool,   *numval gets value.
 * String option: gov_string, *stringval gets allocated string.
 * Hidden Number option: gov_hidden_number.
 * Hidden Toggle option: gov_hidden_bool.
 * Hidden String option: gov_hidden_string.
 * Unknown option: gov_unknown.
 *
 * "flagsp" (if not NULL) is set to the option flags (P_xxxx).
 */
    getoption_T
get_option_విలువ(
    char_u	*name,
    దీర్ఘ	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    పూర్ణాంకం		*flagsp,
    పూర్ణాంకం		scope)
{
    పూర్ణాంకం		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    ఒకవేళ (opt_idx < 0)		    // option not in the table
    {
	పూర్ణాంకం కీలకం;

	ఒకవేళ (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (కీలకం = కనుగొను_కీలకం_option(name, FALSE)) != 0)
	{
	    char_u కీలకం_name[2];
	    char_u *p;

	    ఒకవేళ (flagsp != ఖాళీ)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    ఒకవేళ (కీలకం < 0)
	    {
		కీలకం_name[0] = KEY2TERMCAP0(కీలకం);
		కీలకం_name[1] = KEY2TERMCAP1(కీలకం);
	    }
	    లేకపోతే
	    {
		కీలకం_name[0] = KS_కీలకం;
		కీలకం_name[1] = (కీలకం & 0xff);
	    }
	    p = కనుగొను_termcode(కీలకం_name);
	    ఒకవేళ (p != ఖాళీ)
	    {
		ఒకవేళ (stringval != ఖాళీ)
		    *stringval = vim_strsave(p);
		తిరిగి gov_string;
	    }
	}
	తిరిగి gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    ఒకవేళ (flagsp != ఖాళీ)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    ఒకవేళ (options[opt_idx].flags & P_STRING)
    {
	ఒకవేళ (varp == ఖాళీ)		    // hidden option
	    తిరిగి gov_hidden_string;
	ఒకవేళ (stringval != ఖాళీ)
	{
	    ఒకవేళ ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    లేకపోతే ఒకవేళ ((char_u **)varp == &curbuf->b_p_కీలకం
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    లేకపోతే
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	తిరిగి gov_string;
    }

    ఒకవేళ (varp == ఖాళీ)		    // hidden option
	తిరిగి (options[opt_idx].flags & P_అంకె)
					 ? gov_hidden_సంఖ్య : gov_hidden_bool;
    ఒకవేళ (options[opt_idx].flags & P_అంకె)
	*numval = *(దీర్ఘ *)varp;
    లేకపోతే
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	ఒకవేళ ((పూర్ణాంకం *)varp == &curbuf->b_మారింది)
	    *numval = curbuf_Is_మారింది();
	లేకపోతే
	    *numval = (దీర్ఘ) *(పూర్ణాంకం *)varp;
    }
    తిరిగి (options[opt_idx].flags & P_అంకె) ? gov_సంఖ్య : gov_bool;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Returns the option attributes and its value. Unlike the above function it
 * will return either global value or local value of the option depending on
 * what was requested, but it will never return global value if it was
 * requested to return local one and vice versa. Neither it will return
 * buffer-local value if it was requested to return window-local one.
 *
 * Pretends that option is absent if it is not present in the requested scope
 * (i.e. has no global, window-local or buffer-local value depending on
 * opt_type). Uses
 *
 * Returned flags:
 *       0 hidden or unknown option, also option that does not have requested
 *	   type (see SREQ_* in vim.h)
 *  see SOPT_* in vim.h for other flags
 *
 * Possible opt_type values: see SREQ_* in vim.h
 */
    పూర్ణాంకం
get_option_విలువ_strict(
    char_u	*name,
    దీర్ఘ	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    పూర్ణాంకం		opt_type,
    శూన్య	*from)
{
    పూర్ణాంకం		opt_idx;
    char_u	*varp = ఖాళీ;
    నిర్మాణం vimoption *p;
    పూర్ణాంకం		r = 0;

    opt_idx = findoption(name);
    ఒకవేళ (opt_idx < 0)
	తిరిగి 0;

    p = &(options[opt_idx]);

    // Hidden option
    ఒకవేళ (p->var == ఖాళీ)
	తిరిగి 0;

    ఒకవేళ (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    లేకపోతే ఒకవేళ (p->flags & P_అంకె)
	r |= SOPT_అంకె;
    లేకపోతే ఒకవేళ (p->flags & P_STRING)
	r |= SOPT_STRING;

    ఒకవేళ (p->indir == PV_NONE)
    {
	ఒకవేళ (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	లేకపోతే
	    తిరిగి 0; // Did not request global-only option
    }
    లేకపోతే
    {
	ఒకవేళ (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	లేకపోతే ఒకవేళ (opt_type == SREQ_GLOBAL)
	    తిరిగి 0; // Requested global option

	ఒకవేళ (p->indir & PV_WIN)
	{
	    ఒకవేళ (opt_type == SREQ_BUF)
		తిరిగి 0; // Did not request window-local option
	    లేకపోతే
		r |= SOPT_WIN;
	}
	లేకపోతే ఒకవేళ (p->indir & PV_BUF)
	{
	    ఒకవేళ (opt_type == SREQ_WIN)
		తిరిగి 0; // Did not request buffer-local option
	    లేకపోతే
		r |= SOPT_BUF;
	}
    }

    ఒకవేళ (stringval == ఖాళీ)
	తిరిగి r;

    ఒకవేళ (opt_type == SREQ_GLOBAL)
	varp = p->var;
    లేకపోతే
    {
	ఒకవేళ (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    ఒకవేళ (p->indir == PV_MOD)
	    {
		*numval = buf_Is_మారింది((buf_T *)from);
		varp = ఖాళీ;
	    }
# ifdef FEAT_CRYPT
	    లేకపోతే ఒకవేళ (p->indir == PV_కీలకం)
	    {
		// never return the value of the crypt key
		*stringval = ఖాళీ;
		varp = ఖాళీ;
	    }
# endif
	    లేకపోతే
	    {
		buf_T *save_curbuf = curbuf;

		// only getting a pointer, no need to use aucmd_prepbuf()
		curbuf = (buf_T *)from;
		curwin->w_బఫరు = curbuf;
		varp = get_varp(p);
		curbuf = save_curbuf;
		curwin->w_బఫరు = curbuf;
	    }
	}
	లేకపోతే ఒకవేళ (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_బఫరు;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_బఫరు;
	}
	ఒకవేళ (varp == p->var)
	    తిరిగి (r | SOPT_UNSET);
    }

    ఒకవేళ (varp != ఖాళీ)
    {
	ఒకవేళ (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	లేకపోతే ఒకవేళ (p->flags & P_అంకె)
	    *numval = *(దీర్ఘ *) varp;
	లేకపోతే
	    *numval = *(పూర్ణాంకం *)varp;
    }

    తిరిగి r;
}

/*
 * Iterate over options. First argument is a pointer to a pointer to a
 * structure inside options[] array, second is option type like in the above
 * function.
 *
 * If first argument points to NULL it is assumed that iteration just started
 * and caller needs the very first value.
 * If first argument points to the end marker function returns NULL and sets
 * first argument to NULL.
 *
 * Returns full option name for current option on each call.
 */
    char_u *
option_iter_తదుపరి(శూన్య **option, పూర్ణాంకం opt_type)
{
    నిర్మాణం vimoption	*ret = ఖాళీ;
    చేయి
    {
	ఒకవేళ (*option == ఖాళీ)
	    *option = (శూన్య *) options;
	లేకపోతే ఒకవేళ (((నిర్మాణం vimoption *) (*option))->fullname == ఖాళీ)
	{
	    *option = ఖాళీ;
	    తిరిగి ఖాళీ;
	}
	లేకపోతే
	    *option = (శూన్య *) (((నిర్మాణం vimoption *) (*option)) + 1);

	ret = ((నిర్మాణం vimoption *) (*option));

	// Hidden option
	ఒకవేళ (ret->var == ఖాళీ)
	{
	    ret = ఖాళీ;
	    కొనసాగు;
	}

	ఎంపిక (opt_type)
	{
	    స్థితి SREQ_GLOBAL:
		ఒకవేళ (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = ఖాళీ;
		ఆపు;
	    స్థితి SREQ_BUF:
		ఒకవేళ (!(ret->indir & PV_BUF))
		    ret = ఖాళీ;
		ఆపు;
	    స్థితి SREQ_WIN:
		ఒకవేళ (!(ret->indir & PV_WIN))
		    ret = ఖాళీ;
		ఆపు;
	    అప్రమేయ:
		అంతర్గత_error("option_iter_next()");
		తిరిగి ఖాళీ;
	}
    }
    అయితే (ret == ఖాళీ);

    తిరిగి (char_u *)ret->fullname;
}
#అయితే_ముగింపు

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(పూర్ణాంకం opt_idx)
{
    తిరిగి options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    శూన్య
సమితి_option_flag(పూర్ణాంకం opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    పూర్ణాంకం
is_global_option(పూర్ణాంకం opt_idx)
{
    తిరిగి options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    పూర్ణాంకం
is_global_local_option(పూర్ణాంకం opt_idx)
{
    తిరిగి options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    పూర్ణాంకం
is_కిటికీ_local_option(పూర్ణాంకం opt_idx)
{
    తిరిగి options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    పూర్ణాంకం
is_hidden_option(పూర్ణాంకం opt_idx)
{
    తిరిగి options[opt_idx].var == ఖాళీ;
}

#ఒకవేళ defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    పూర్ణాంకం
is_crypt_కీలకం_option(పూర్ణాంకం opt_idx)
{
    తిరిగి options[opt_idx].indir == PV_కీలకం;
}
#అయితే_ముగింపు

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    అక్షరం *
సమితి_option_విలువ(
    char_u	*name,
    దీర్ఘ	సంఖ్య,
    char_u	*string,
    పూర్ణాంకం		opt_flags)	// OPT_LOCAL or 0 (both)
{
    పూర్ణాంకం		opt_idx;
    char_u	*varp;
    long_u	flags;
    స్థిర అక్షరం	errbuf[ERR_BUFLEN];
    పూర్ణాంకం		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    ఒకవేళ (opt_idx < 0)
    {
	పూర్ణాంకం కీలకం;

	ఒకవేళ (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (కీలకం = కనుగొను_కీలకం_option(name, FALSE)) != 0)
	{
	    char_u కీలకం_name[2];

	    ఒకవేళ (కీలకం < 0)
	    {
		కీలకం_name[0] = KEY2TERMCAP0(కీలకం);
		కీలకం_name[1] = KEY2TERMCAP1(కీలకం);
	    }
	    లేకపోతే
	    {
		కీలకం_name[0] = KS_కీలకం;
		కీలకం_name[1] = (కీలకం & 0xff);
	    }
	    కూడు_termcode(కీలకం_name, string, FALSE);
	    ఒకవేళ (నిండిన_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    తిరిగి ఖాళీ;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    లేకపోతే
    {
	flags = options[opt_idx].flags;
#నిర్వచితమైతే HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	ఒకవేళ (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_లోపల_sandbox));
	    తిరిగి ఖాళీ;
	}
#అయితే_ముగింపు
	ఒకవేళ (flags & P_STRING)
	    తిరిగి సమితి_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	ఒకవేళ (varp != ఖాళీ)	// hidden option is not changed
	{
	    ఒకవేళ (సంఖ్య == 0 && string != ఖాళీ)
	    {
		పూర్ణాంకం idx;

		// Either we are given a string or we are setting option
		// to zero.
		కొరకు (idx = 0; string[idx] == '0'; ++idx)
		    ;
		ఒకవేళ (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_సంఖ్య_required_after_str_equal_str),
								 name, string);
		    తిరిగి ఖాళీ;     // do nothing as we hit an error

		}
	    }
	    ఒకవేళ (flags & P_అంకె)
	    {
		తిరిగి సమితి_అంకె_option(opt_idx, varp, సంఖ్య,
					   errbuf, పరిమాణం(errbuf), opt_flags);
	    }
	    లేకపోతే
		తిరిగి సమితి_bool_option(opt_idx, varp, (పూర్ణాంకం)సంఖ్య, opt_flags);
	}

    }
    తిరిగి ఖాళీ;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    శూన్య
సమితి_option_విలువ_give_err(
    char_u	*name,
    దీర్ఘ	సంఖ్య,
    char_u	*string,
    పూర్ణాంకం		opt_flags)	// OPT_LOCAL or 0 (both)
{
    అక్షరం *errmsg = సమితి_option_విలువ(name, సంఖ్య, string, opt_flags);

    ఒకవేళ (errmsg != ఖాళీ)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_పదం_సంకేతం(char_u *tname)
{
    పూర్ణాంకం	    opt_idx;
    char_u  *varp;

    ఒకవేళ (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	తిరిగి ఖాళీ;
    ఒకవేళ ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	ఒకవేళ (varp != ఖాళీ)
	    varp = *(char_u **)(varp);
	తిరిగి varp;
    }
    తిరిగి కనుగొను_termcode(tname + 2);
}

    char_u *
get_highlight_default(శూన్య)
{
    పూర్ణాంకం i;

    i = findoption((char_u *)"hl");
    ఒకవేళ (i >= 0)
	తిరిగి options[i].def_val[VI_DEFAULT];
    తిరిగి (char_u *)ఖాళీ;
}

    char_u *
get_encoding_default(శూన్య)
{
    పూర్ణాంకం i;

    i = findoption((char_u *)"enc");
    ఒకవేళ (i >= 0)
	తిరిగి options[i].def_val[VI_DEFAULT];
    తిరిగి (char_u *)ఖాళీ;
}

#ఒకవేళ defined(FEAT_QUICKFIX)
    పూర్ణాంకం
is_option_allocated(అక్షరం *name)
{
    పూర్ణాంకం idx = findoption((char_u *)name);

    తిరిగి idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#అయితే_ముగింపు

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    స్థిర పూర్ణాంకం
కనుగొను_కీలకం_option(char_u *arg_arg, పూర్ణాంకం has_lt)
{
    పూర్ణాంకం		కీలకం = 0;
    పూర్ణాంకం		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    ఒకవేళ (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	ఒకవేళ (!has_lt || arg[4] == '>')
	    కీలకం = TERMCAP2_కీలకం(arg[2], arg[3]);
    }
    లేకపోతే ఒకవేళ (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	కీలకం = కనుగొను_special_కీలకం(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_కీలకం | FSK_SIMPLIFY, ఖాళీ);
	ఒకవేళ (modifiers)		    // can't handle modifiers here
	    కీలకం = 0;
    }
    తిరిగి కీలకం;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    స్థిర శూన్య
showoptions(
    పూర్ణాంకం		all,
    పూర్ణాంకం		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    నిర్మాణం vimoption	*p;
    పూర్ణాంకం			నిలువు;
    పూర్ణాంకం			isterm;
    char_u		*varp;
    నిర్మాణం vimoption	**వస్తువులు;
    పూర్ణాంకం			అంశం_లెక్క;
    పూర్ణాంకం			నిరంతరం;
    పూర్ణాంకం			అడ్డువరుస, అడ్డువరుసలు_2;
    పూర్ణాంకం			నిలువుల_సంఖ్య;
    పూర్ణాంకం			i;
    పూర్ణాంకం			len;

#నిర్వచించు INC 20
#నిర్వచించు GAP 3

    వస్తువులు = ALLOC_MULT(నిర్మాణం vimoption *, OPTION_లెక్క);
    ఒకవేళ (వస్తువులు == ఖాళీ)
	తిరిగి;

    // Highlight title
    ఒకవేళ (all == 2)
	సందేశం_puts_title(_("\n--- Terminal codes ---"));
    లేకపోతే ఒకవేళ (opt_flags & OPT_GLOBAL)
	సందేశం_puts_title(_("\n--- Global option values ---"));
    లేకపోతే ఒకవేళ (opt_flags & OPT_LOCAL)
	సందేశం_puts_title(_("\n--- Local option values ---"));
    లేకపోతే
	సందేశం_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    కొరకు (నిరంతరం = 1; నిరంతరం <= 2 && !got_int; ++నిరంతరం)
    {
	// collect the items in items[]
	అంశం_లెక్క = 0;
	కొరకు (p = &options[0]; p->fullname != ఖాళీ; p++)
	{
	    // apply :filter /pat/
	    ఒకవేళ (సందేశం_filtered((char_u *)p->fullname))
		కొనసాగు;

	    varp = ఖాళీ;
	    isterm = istermoption(p);
	    ఒకవేళ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		ఒకవేళ (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    లేకపోతే
		varp = get_varp(p);
	    ఒకవేళ (varp != ఖాళీ
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		ఒకవేళ (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		లేకపోతే ఒకవేళ (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		లేకపోతే
		{
		    option_value2string(p, opt_flags);
		    len = (పూర్ణాంకం)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		ఒకవేళ ((len <= INC - GAP && నిరంతరం == 1) ||
						(len > INC - GAP && నిరంతరం == 2))
		    వస్తువులు[అంశం_లెక్క++] = p;
	    }
	}

	// display the items
	ఒకవేళ (నిరంతరం == 1)
	{
	    నిలువుల_సంఖ్య = (Columns + GAP - 3) / INC;
	    ఒకవేళ (నిలువుల_సంఖ్య == 0)
		నిలువుల_సంఖ్య = 1;
	    అడ్డువరుసలు_2 = (అంశం_లెక్క + నిలువుల_సంఖ్య - 1) / నిలువుల_సంఖ్య;
	}
	లేకపోతే	// run == 2
	    అడ్డువరుసలు_2 = అంశం_లెక్క;
	కొరకు (అడ్డువరుస = 0; అడ్డువరుస < అడ్డువరుసలు_2 && !got_int; ++అడ్డువరుస)
	{
	    సందేశం_putchar('\n');			// go to next line
	    ఒకవేళ (got_int)			// 'q' typed in more
		ఆపు;
	    నిలువు = 0;
	    కొరకు (i = అడ్డువరుస; i < అంశం_లెక్క; i += అడ్డువరుసలు_2)
	    {
		సందేశం_నిలువు = నిలువు;			// make columns
		showoneopt(వస్తువులు[i], opt_flags);
		నిలువు += INC;
	    }
	    బహిర్గతం_flush();
	    ui_breakcheck();
	}
    }
    vim_free(వస్తువులు);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    స్థిర పూర్ణాంకం
optval_default(నిర్మాణం vimoption *p, char_u *varp, పూర్ణాంకం compatible)
{
    పూర్ణాంకం		dvi;

    ఒకవేళ (varp == ఖాళీ)
	తిరిగి TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    ఒకవేళ (p->flags & P_అంకె)
	తిరిగి (*(దీర్ఘ *)varp == (దీర్ఘ)(long_i)p->def_val[dvi]);
    ఒకవేళ (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	తిరిగి (*(పూర్ణాంకం *)varp == (పూర్ణాంకం)(దీర్ఘ)(long_i)p->def_val[dvi]);
    // P_STRING
    తిరిగి (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    స్థిర శూన్య
showoneopt(
    నిర్మాణం vimoption	*p,
    పూర్ణాంకం			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    పూర్ణాంకం		save_silent = silent_బహుళకం;

    silent_బహుళకం = FALSE;
    info_సందేశం = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    ఒకవేళ ((p->flags & P_BOOL) && ((పూర్ణాంకం *)varp == &curbuf->b_మారింది
					? !curbuf_Is_మారింది() : !*(పూర్ణాంకం *)varp))
	సందేశం_puts("no");
    లేకపోతే ఒకవేళ ((p->flags & P_BOOL) && *(పూర్ణాంకం *)varp < 0)
	సందేశం_puts("--");
    లేకపోతే
	సందేశం_puts("  ");
    సందేశం_puts(p->fullname);
    ఒకవేళ (!(p->flags & P_BOOL))
    {
	సందేశం_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	సందేశం_outtrans(NameBuff);
    }

    silent_బహుళకం = save_silent;
    info_సందేశం = FALSE;
}

/*
 * Write modified options as ":set" commands to a file.
 *
 * There are three values for "opt_flags":
 * OPT_GLOBAL:		   Write global option values and fresh values of
 *			   buffer-local options (used for start of a session
 *			   file).
 * OPT_GLOBAL + OPT_LOCAL: Idem, add fresh values of window-local options for
 *			   curwin (used for a vimrc file).
 * OPT_LOCAL:		   Write buffer-local option values for curbuf, fresh
 *			   and local values for window-local options of
 *			   curwin.  Local values are also written when at the
 *			   default value, because a modeline or autocommand
 *			   may have set them when doing ":edit file" and the
 *			   user has set them back at the default or fresh
 *			   value.
 *			   When "local_only" is TRUE, don't write fresh
 *			   values, only local values (for ":mkview").
 * (fresh value = value used for a new buffer or window for a local option).
 *
 * Return FAIL on error, OK otherwise.
 */
    పూర్ణాంకం
makeset(FILE *fd, పూర్ణాంకం opt_flags, పూర్ణాంకం local_only)
{
    నిర్మాణం vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = ఖాళీ;	// fresh value
    అక్షరం		*cmd;
    పూర్ణాంకం			విడత;
    పూర్ణాంకం			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    కొరకు (pri = 1; pri >= 0; --pri)
    {
      కొరకు (p = &options[0]; !istermoption(p); p++)
	ఒకవేళ (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    ఒకవేళ (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		కొనసాగు;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    ఒకవేళ ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		కొనసాగు;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    ఒకవేళ ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		కొనసాగు;

	    ఒకవేళ ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		కొనసాగు;

	    విడత = 2;
	    ఒకవేళ (p->indir != PV_NONE)
	    {
		ఒకవేళ (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    ఒకవేళ (!(opt_flags & OPT_LOCAL))
			కొనసాగు;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    ఒకవేళ (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			ఒకవేళ (!optval_default(p, varp_fresh, p_cp))
			{
			    విడత = 1;
			    varp_local = varp;
			    varp = varp_fresh;
			}
		    }
		}
	    }

	    // Round 1: fresh value for window-local options.
	    // Round 2: other values
	    కొరకు ( ; విడత <= 2; varp = varp_local, ++విడత)
	    {
		ఒకవేళ (విడత == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		లేకపోతే
		    cmd = "setlocal";

		ఒకవేళ (p->flags & P_BOOL)
		{
		    ఒకవేళ (put_setbool(fd, cmd, p->fullname, *(పూర్ణాంకం *)varp) == FAIL)
			తిరిగి FAIL;
		}
		లేకపోతే ఒకవేళ (p->flags & P_అంకె)
		{
		    ఒకవేళ (put_setnum(fd, cmd, p->fullname, (దీర్ఘ *)varp) == FAIL)
			తిరిగి FAIL;
		}
		లేకపోతే    // P_STRING
		{
		    పూర్ణాంకం		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    ఒకవేళ ((p->flags & P_FUNC) && *(char_u **)varp != ఖాళీ
			    && అక్షరశ్రేణి_శోధన(*(అక్షరం **)varp, "<lambda>") != ఖాళీ)
			కొనసాగు;

#నిర్వచితమైతే FEAT_EVAL
		    legacy = !is_option_విలువ_vim9(p - &options[0],
			    విడత == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#లేకపోతే
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#అయితే_ముగింపు

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    ఒకవేళ (
#ఒకవేళ defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#అయితే_ముగింపు
			    p->indir == PV_FT)
		    {
			ఒకవేళ (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    తిరిగి FAIL;
			do_endif = TRUE;
		    }
		    ఒకవేళ (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			తిరిగి FAIL;
		    ఒకవేళ (do_endif)
		    {
			ఒకవేళ (put_పంక్తి(fd, "endif") == FAIL)
			    తిరిగి FAIL;
		    }
		}
	    }
	}
    }
    తిరిగి OK;
}

#ఒకవేళ defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    పూర్ణాంకం
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_సంస్కరణ < SCRIPT_సంస్కరణ_VIM9;
# endif

    ఒకవేళ (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
								       == FAIL
# ifdef FEAT_EVAL
	    || put_setstring(fd, fde_is_legacy, "setlocal", "fde",
						  &curwin->w_p_fde, 0) == FAIL
# endif
	    || put_setstring(fd, false, "setlocal", "fmr", &curwin->w_p_fmr, 0)
								       == FAIL
	    || put_setstring(fd, false, "setlocal", "fdi", &curwin->w_p_fdi, 0)
								       == FAIL
	    || put_setnum(fd, "setlocal", "fdl", &curwin->w_p_fdl) == FAIL
	    || put_setnum(fd, "setlocal", "fml", &curwin->w_p_fml) == FAIL
	    || put_setnum(fd, "setlocal", "fdn", &curwin->w_p_fdn) == FAIL
	    || put_setbool(fd, "setlocal", "fen", curwin->w_p_fen) == FAIL
	    )
	తిరిగి FAIL;

    తిరిగి OK;
}
#అయితే_ముగింపు

    స్థిర పూర్ణాంకం
put_setstring(
    FILE	*fd,
    bool	legacy,
    అక్షరం	*cmd,
    అక్షరం	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = ఖాళీ;
    char_u	*part = ఖాళీ;
    char_u	*p;

    ఒకవేళ (legacy && fprintf(fd, "legacy ") < 0)
	తిరిగి FAIL;
    ఒకవేళ (fprintf(fd, "%s %s=", cmd, name) < 0)
	తిరిగి FAIL;
    ఒకవేళ (*valuep != ఖాళీ)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	ఒకవేళ (valuep == &p_pt)
	{
	    s = *valuep;
	    అయితే (*s != NUL)
		ఒకవేళ (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    తిరిగి FAIL;
	}
	// expand the option value, replace $HOME by ~
	లేకపోతే ఒకవేళ ((flags & P_EXPAND) != 0)
	{
	    పూర్ణాంకం  కొలత = (పూర్ణాంకం)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(కొలత);
	    ఒకవేళ (buf == ఖాళీ)
		వెళ్ళు fail;
	    home_replace(ఖాళీ, *valuep, buf, కొలత, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    ఒకవేళ (కొలత >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != ఖాళీ)
	    {
		part = alloc(కొలత);
		ఒకవేళ (part == ఖాళీ)
		    వెళ్ళు fail;

		// write line break to clear the option, e.g. ':set rtp='
		ఒకవేళ (put_eol(fd) == FAIL)
		    వెళ్ళు fail;

		p = buf;
		అయితే (*p != NUL)
		{
		    ఒకవేళ (legacy && fprintf(fd, "legacy ") < 0)
			తిరిగి FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    ఒకవేళ (fprintf(fd, "%s %s+=", cmd, name) < 0)
			వెళ్ళు fail;
		    (శూన్య)copy_option_part(&p, part, కొలత,  ",");
		    ఒకవేళ (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			వెళ్ళు fail;
		}
		vim_free(buf);
		vim_free(part);
		తిరిగి OK;
	    }
	    ఒకవేళ (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		తిరిగి FAIL;
	    }
	    vim_free(buf);
	}
	లేకపోతే ఒకవేళ (put_escstr(fd, *valuep, 2) == FAIL)
	    తిరిగి FAIL;
    }
    ఒకవేళ (put_eol(fd) < 0)
	తిరిగి FAIL;
    తిరిగి OK;
fail:
    vim_free(buf);
    vim_free(part);
    తిరిగి FAIL;
}

    స్థిర పూర్ణాంకం
put_setnum(
    FILE	*fd,
    అక్షరం	*cmd,
    అక్షరం	*name,
    దీర్ఘ	*valuep)
{
    దీర్ఘ	wc;

    ఒకవేళ (fprintf(fd, "%s %s=", cmd, name) < 0)
	తిరిగి FAIL;
    ఒకవేళ (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	ఒకవేళ (fputs((అక్షరం *)get_special_కీలకం_name((పూర్ణాంకం)wc, 0), fd) < 0)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (fprintf(fd, "%ld", *valuep) < 0)
	తిరిగి FAIL;
    ఒకవేళ (put_eol(fd) < 0)
	తిరిగి FAIL;
    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
put_setbool(
    FILE	*fd,
    అక్షరం	*cmd,
    అక్షరం	*name,
    పూర్ణాంకం		విలువ)
{
    ఒకవేళ (విలువ < 0)	// global/local option using global value
	తిరిగి OK;
    ఒకవేళ (fprintf(fd, "%s %s%s", cmd, విలువ ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    శూన్య
clear_termoptions(శూన్య)
{
    // Reset a few things before clearing the old options. This may cause
    // outputting a few things that the terminal doesn't understand, but the
    // screen will be cleared later, so this is OK.
    mch_setmouse(FALSE);	    // switch mouse off
    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window titles
#ఒకవేళ defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    ఒకవేళ (gui.starting)
	clear_xterm_clip();
#అయితే_ముగింపు
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    శూన్య
free_termoptions(శూన్య)
{
    నిర్మాణం vimoption   *p;

    కొరకు (p = options; p->fullname != ఖాళీ; p++)
	ఒకవేళ (istermoption(p))
	{
	    ఒకవేళ (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    ఒకవేళ (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = ఖాళీలు_option;
	    p->def_val[VI_DEFAULT] = ఖాళీలు_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#నిర్వచితమైతే FEAT_EVAL
	    // remember where the option was cleared
	    సమితి_option_sctx_idx((పూర్ణాంకం)(p - options), OPT_GLOBAL, current_sctx);
#అయితే_ముగింపు
	}
    clear_termcodes();
}

/*
 * Free the string for one term option, if it was allocated.
 * Set the string to empty_option and clear allocated flag.
 * "var" points to the option value.
 */
    శూన్య
free_one_termoption(char_u *var)
{
    నిర్మాణం vimoption   *p;

    కొరకు (p = &options[0]; p->fullname != ఖాళీ; p++)
	ఒకవేళ (p->var == var)
	{
	    ఒకవేళ (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = ఖాళీలు_option;
	    p->flags &= ~P_ALLOCED;
	    ఆపు;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    శూన్య
సమితి_పదం_defaults(శూన్య)
{
    నిర్మాణం vimoption   *p;

    కొరకు (p = &options[0]; p->fullname != ఖాళీ; p++)
    {
	ఒకవేళ (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    ఒకవేళ (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    ఒకవేళ (p->flags & P_ALLOCED)
	    {
		p->flags |= P_DEF_ALLOCED;
		p->flags &= ~P_ALLOCED;	 // don't free the value now
	    }
	}
    }
}

/*
 * return TRUE if 'p' starts with 't_'
 */
    స్థిర పూర్ణాంకం
istermoption(నిర్మాణం vimoption *p)
{
    తిరిగి (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    స్థిర పూర్ణాంకం
istermoption_idx(పూర్ణాంకం opt_idx)
{
    తిరిగి istermoption(&options[opt_idx]);
}

#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    శూన్య
unset_global_local_option(char_u *name, శూన్య *from)
{
    నిర్మాణం vimoption *p;
    పూర్ణాంకం		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    ఒకవేళ (opt_idx < 0)
	తిరిగి;
    p = &(options[opt_idx]);

    ఎంపిక ((పూర్ణాంకం)p->indir)
    {
	// global option with local value: use local value if it's been set
	స్థితి PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    ఆపు;
	స్థితి PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    ఆపు;
	స్థితి PV_మార్గం:
	    clear_string_option(&buf->b_p_మార్గం);
	    ఆపు;
	స్థితి PV_AC:
	    buf->b_p_ac = -1;
	    ఆపు;
	స్థితి PV_AR:
	    buf->b_p_ar = -1;
	    ఆపు;
# ifdef HAVE_FSYNC
	స్థితి PV_FS:
	    buf->b_p_fs = -1;
	    ఆపు;
# endif
	స్థితి PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    ఆపు;
	స్థితి PV_ట్యాగులు:
	    clear_string_option(&buf->b_p_ట్యాగులు);
	    ఆపు;
	స్థితి PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    ఆపు;
	స్థితి PV_SISO:
	    curwin->w_p_siso = -1;
	    ఆపు;
	స్థితి PV_SO:
	    curwin->w_p_so = -1;
	    ఆపు;
	స్థితి PV_SOP:
	    curwin->w_p_sop = -1;
	    ఆపు;
# ifdef FEAT_కనుగొను_ID
	స్థితి PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    ఆపు;
	స్థితి PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    ఆపు;
# endif
	స్థితి PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    ఆపు;
	స్థితి PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    ఆపు;
# ifdef FEAT_DIFF
	స్థితి PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    ఆపు;
# endif
	స్థితి PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    ఆపు;
# ifdef FEAT_COMPL_FUNC
	స్థితి PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    ఆపు;
# endif
	స్థితి PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    ఆపు;
# ifdef FEAT_EVAL
	స్థితి PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    ఆపు;
# endif
# ifdef FEAT_QUICKFIX
	స్థితి PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    ఆపు;
	స్థితి PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    ఆపు;
	స్థితి PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    ఆపు;
	స్థితి PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    ఆపు;
# endif
# ఒకవేళ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	స్థితి PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    ఆపు;
# endif
# ఒకవేళ defined(FEAT_CRYPT)
	స్థితి PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    ఆపు;
# endif
# ifdef FEAT_LINEBREAK
	స్థితి PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    ఆపు;
# endif
# ifdef FEAT_STL_OPT
	స్థితి PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    ఆపు;
	స్థితి PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    ఆపు;
# endif
	స్థితి PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    ఆపు;
	స్థితి PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    ఆపు;
	స్థితి PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    ఆపు;
	స్థితి PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    సమితి_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      ఖాళీ, 0);
	    redraw_later(UPD_NOT_VALID);
	    ఆపు;
	స్థితి PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    సమితి_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      ఖాళీ, 0);
	    redraw_later(UPD_NOT_VALID);
	    ఆపు;
	స్థితి PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    ఆపు;
    }
}
#అయితే_ముగింపు

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    స్థిర char_u *
get_varp_scope(నిర్మాణం vimoption *p, పూర్ణాంకం scope)
{
    ఒకవేళ ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	ఒకవేళ (p->var == VAR_WIN)
	    తిరిగి (char_u *)GLOBAL_WO(get_varp(p));
	తిరిగి p->var;
    }
    ఒకవేళ ((scope & OPT_LOCAL) && ((పూర్ణాంకం)p->indir & PV_BOTH))
    {
	ఎంపిక ((పూర్ణాంకం)p->indir)
	{
	    స్థితి PV_FP:   తిరిగి (char_u *)&(curbuf->b_p_fp);
#నిర్వచితమైతే HAVE_FSYNC
	    స్థితి PV_FS:	తిరిగి (char_u *)&(curbuf->b_p_fs);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	    స్థితి PV_FFU: తిరిగి (char_u *)&(curbuf->b_p_ffu);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
	    స్థితి PV_EFM:  తిరిగి (char_u *)&(curbuf->b_p_efm);
	    స్థితి PV_GEFM: తిరిగి (char_u *)&(curbuf->b_p_gefm);
	    స్థితి PV_GP:   తిరిగి (char_u *)&(curbuf->b_p_gp);
	    స్థితి PV_MP:   తిరిగి (char_u *)&(curbuf->b_p_mp);
#అయితే_ముగింపు
	    స్థితి PV_EP:   తిరిగి (char_u *)&(curbuf->b_p_ep);
	    స్థితి PV_KP:   తిరిగి (char_u *)&(curbuf->b_p_kp);
	    స్థితి PV_మార్గం: తిరిగి (char_u *)&(curbuf->b_p_మార్గం);
	    స్థితి PV_AC:   తిరిగి (char_u *)&(curbuf->b_p_ac);
	    స్థితి PV_AR:   తిరిగి (char_u *)&(curbuf->b_p_ar);
	    స్థితి PV_ట్యాగులు: తిరిగి (char_u *)&(curbuf->b_p_ట్యాగులు);
	    స్థితి PV_TC:   తిరిగి (char_u *)&(curbuf->b_p_tc);
	    స్థితి PV_SISO: తిరిగి (char_u *)&(curwin->w_p_siso);
	    స్థితి PV_SO:   తిరిగి (char_u *)&(curwin->w_p_so);
	    స్థితి PV_SOP:  తిరిగి (char_u *)&(curwin->w_p_sop);
#నిర్వచితమైతే FEAT_కనుగొను_ID
	    స్థితి PV_DEF:  తిరిగి (char_u *)&(curbuf->b_p_def);
	    స్థితి PV_INC:  తిరిగి (char_u *)&(curbuf->b_p_inc);
#అయితే_ముగింపు
	    స్థితి PV_COT:  తిరిగి (char_u *)&(curbuf->b_p_cot);
	    స్థితి PV_DICT: తిరిగి (char_u *)&(curbuf->b_p_dict);
#నిర్వచితమైతే FEAT_DIFF
	    స్థితి PV_DIA:  తిరిగి (char_u *)&(curbuf->b_p_dia);
#అయితే_ముగింపు
	    స్థితి PV_TSR:  తిరిగి (char_u *)&(curbuf->b_p_tsr);
#నిర్వచితమైతే FEAT_COMPL_FUNC
	    స్థితి PV_TSRFU: తిరిగి (char_u *)&(curbuf->b_p_tsrfu);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    స్థితి PV_BEXPR: తిరిగి (char_u *)&(curbuf->b_p_bexpr);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_CRYPT)
	    స్థితి PV_CM:	  తిరిగి (char_u *)&(curbuf->b_p_cm);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
	    స్థితి PV_SBR:  తిరిగి (char_u *)&(curwin->w_p_sbr);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_STL_OPT
	    స్థితి PV_STL:  తిరిగి (char_u *)&(curwin->w_p_stl);
	    స్థితి PV_STLO: తిరిగి (char_u *)&(curwin->w_p_stlo);
#అయితే_ముగింపు
	    స్థితి PV_UL:   తిరిగి (char_u *)&(curbuf->b_p_ul);
	    స్థితి PV_LW:   తిరిగి (char_u *)&(curbuf->b_p_lw);
	    స్థితి PV_BKC:  తిరిగి (char_u *)&(curbuf->b_p_bkc);
	    స్థితి PV_MENC: తిరిగి (char_u *)&(curbuf->b_p_menc);
	    స్థితి PV_LCS:  తిరిగి (char_u *)&(curwin->w_p_lcs);
	    స్థితి PV_FCS:  తిరిగి (char_u *)&(curwin->w_p_fcs);
	    స్థితి PV_VE:	  తిరిగి (char_u *)&(curwin->w_p_ve);

	}
	తిరిగి ఖాళీ; // "cannot happen"
    }
    తిరిగి get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(పూర్ణాంకం opt_idx, పూర్ణాంకం scope)
{
    తిరిగి get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    స్థిర char_u *
get_varp(నిర్మాణం vimoption *p)
{
    // hidden option, always return NULL
    ఒకవేళ (p->var == ఖాళీ)
	తిరిగి ఖాళీ;

    ఎంపిక ((పూర్ణాంకం)p->indir)
    {
	స్థితి PV_NONE:	తిరిగి p->var;

	// global option with local value: use local value if it's been set
	స్థితి PV_EP:	తిరిగి *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	స్థితి PV_KP:	తిరిగి *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	స్థితి PV_మార్గం:	తిరిగి *curbuf->b_p_మార్గం != NUL
				    ? (char_u *)&(curbuf->b_p_మార్గం) : p->var;
	స్థితి PV_AC:	తిరిగి curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	స్థితి PV_AR:	తిరిగి curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	స్థితి PV_ట్యాగులు:	తిరిగి *curbuf->b_p_ట్యాగులు != NUL
				    ? (char_u *)&(curbuf->b_p_ట్యాగులు) : p->var;
	స్థితి PV_TC:	తిరిగి *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	స్థితి PV_BKC:	తిరిగి *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	స్థితి PV_SISO:	తిరిగి curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	స్థితి PV_SO:	తిరిగి curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	స్థితి PV_SOP:	తిరిగి curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#నిర్వచితమైతే FEAT_కనుగొను_ID
	స్థితి PV_DEF:	తిరిగి *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	స్థితి PV_INC:	తిరిగి *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#అయితే_ముగింపు
	స్థితి PV_COT:	తిరిగి *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	స్థితి PV_DICT:	తిరిగి *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#నిర్వచితమైతే FEAT_DIFF
	స్థితి PV_DIA:	తిరిగి *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#అయితే_ముగింపు
	స్థితి PV_TSR:	తిరిగి *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#నిర్వచితమైతే FEAT_COMPL_FUNC
	స్థితి PV_TSRFU:	తిరిగి *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#అయితే_ముగింపు
	స్థితి PV_FP:	తిరిగి *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#నిర్వచితమైతే HAVE_FSYNC
	స్థితి PV_FS:	తిరిగి curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	స్థితి PV_FFU:	తిరిగి *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
	స్థితి PV_EFM:	తిరిగి *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	స్థితి PV_GEFM:	తిరిగి *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	స్థితి PV_GP:	తిరిగి *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	స్థితి PV_MP:	తిరిగి *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	స్థితి PV_BEXPR:	తిరిగి *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_CRYPT)
	స్థితి PV_CM:	తిరిగి *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
	స్థితి PV_SBR:	తిరిగి *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_STL_OPT
	స్థితి PV_STL:	తిరిగి *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	స్థితి PV_STLO:	తిరిగి *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#అయితే_ముగింపు
	స్థితి PV_UL:	తిరిగి curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	స్థితి PV_LW:	తిరిగి *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	స్థితి PV_MENC:	తిరిగి *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#నిర్వచితమైతే FEAT_ARABIC
	స్థితి PV_ARAB:	తిరిగి (char_u *)&(curwin->w_p_arab);
#అయితే_ముగింపు
	స్థితి PV_LIST:	తిరిగి (char_u *)&(curwin->w_p_list);
	స్థితి PV_LCS:	తిరిగి *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	స్థితి PV_FCS:	తిరిగి *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	స్థితి PV_VE:	తిరిగి *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#నిర్వచితమైతే FEAT_SPELL
	స్థితి PV_SPELL:	తిరిగి (char_u *)&(curwin->w_p_spell);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
	స్థితి PV_CUC:	తిరిగి (char_u *)&(curwin->w_p_cuc);
	స్థితి PV_CUL:	తిరిగి (char_u *)&(curwin->w_p_cul);
	స్థితి PV_CULOPT:	తిరిగి (char_u *)&(curwin->w_p_culopt);
	స్థితి PV_CC:	తిరిగి (char_u *)&(curwin->w_p_cc);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
	స్థితి PV_DIFF:	తిరిగి (char_u *)&(curwin->w_p_diff);
#అయితే_ముగింపు
	స్థితి PV_EIW:	తిరిగి (char_u *)&(curwin->w_p_eiw);
#నిర్వచితమైతే FEAT_FOLDING
	స్థితి PV_FDC:	తిరిగి (char_u *)&(curwin->w_p_fdc);
	స్థితి PV_FEN:	తిరిగి (char_u *)&(curwin->w_p_fen);
	స్థితి PV_FDI:	తిరిగి (char_u *)&(curwin->w_p_fdi);
	స్థితి PV_FDL:	తిరిగి (char_u *)&(curwin->w_p_fdl);
	స్థితి PV_FDM:	తిరిగి (char_u *)&(curwin->w_p_fdm);
	స్థితి PV_FML:	తిరిగి (char_u *)&(curwin->w_p_fml);
	స్థితి PV_FDN:	తిరిగి (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	స్థితి PV_FDE:	తిరిగి (char_u *)&(curwin->w_p_fde);
	స్థితి PV_FDT:	తిరిగి (char_u *)&(curwin->w_p_fdt);
# endif
	స్థితి PV_FMR:	తిరిగి (char_u *)&(curwin->w_p_fmr);
#అయితే_ముగింపు
	స్థితి PV_NU:	తిరిగి (char_u *)&(curwin->w_p_nu);
	స్థితి PV_RNU:	తిరిగి (char_u *)&(curwin->w_p_rnu);
#నిర్వచితమైతే FEAT_LINEBREAK
	స్థితి PV_NUW:	తిరిగి (char_u *)&(curwin->w_p_nuw);
#అయితే_ముగింపు
	స్థితి PV_WFB:	తిరిగి (char_u *)&(curwin->w_p_wfb);
	స్థితి PV_WFH:	తిరిగి (char_u *)&(curwin->w_p_wfh);
	స్థితి PV_WFW:	తిరిగి (char_u *)&(curwin->w_p_wfw);
#ఒకవేళ defined(FEAT_QUICKFIX)
	స్థితి PV_PVW:	తిరిగి (char_u *)&(curwin->w_p_pvw);
	స్థితి PV_LHI:	తిరిగి (char_u *)&(curwin->w_p_lhi);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RIGHTLEFT
	స్థితి PV_RL:	తిరిగి (char_u *)&(curwin->w_p_rl);
	స్థితి PV_RLC:	తిరిగి (char_u *)&(curwin->w_p_rlc);
#అయితే_ముగింపు
	స్థితి PV_SCROLL:	తిరిగి (char_u *)&(curwin->w_p_scr);
	స్థితి PV_SMS:	తిరిగి (char_u *)&(curwin->w_p_sms);
	స్థితి PV_WRAP:	తిరిగి (char_u *)&(curwin->w_p_wrap);
#నిర్వచితమైతే FEAT_LINEBREAK
	స్థితి PV_LBR:	తిరిగి (char_u *)&(curwin->w_p_lbr);
	స్థితి PV_BRI:	తిరిగి (char_u *)&(curwin->w_p_bri);
	స్థితి PV_BRIOPT: తిరిగి (char_u *)&(curwin->w_p_briopt);
#అయితే_ముగింపు
	స్థితి PV_WCR:	తిరిగి (char_u *)&(curwin->w_p_wcr);
	స్థితి PV_SCBIND: తిరిగి (char_u *)&(curwin->w_p_scb);
	స్థితి PV_CRBIND: తిరిగి (char_u *)&(curwin->w_p_crb);
#నిర్వచితమైతే FEAT_CONCEAL
	స్థితి PV_COCU:   తిరిగి (char_u *)&(curwin->w_p_cocu);
	స్థితి PV_COLE:   తిరిగి (char_u *)&(curwin->w_p_cole);
#అయితే_ముగింపు
	స్థితి PV_WHL:	తిరిగి (char_u *)&(curwin->w_p_whl);

#నిర్వచితమైతే FEAT_TERMINAL
	స్థితి PV_TWK:    తిరిగి (char_u *)&(curwin->w_p_twk);
	స్థితి PV_TWS:    తిరిగి (char_u *)&(curwin->w_p_tws);
	స్థితి PV_TWSL:	తిరిగి (char_u *)&(curbuf->b_p_twsl);
#అయితే_ముగింపు

	స్థితి PV_AI:	తిరిగి (char_u *)&(curbuf->b_p_ai);
	స్థితి PV_BIN:	తిరిగి (char_u *)&(curbuf->b_p_bin);
	స్థితి PV_BOMB:	తిరిగి (char_u *)&(curbuf->b_p_bomb);
	స్థితి PV_BH:	తిరిగి (char_u *)&(curbuf->b_p_bh);
	స్థితి PV_BT:	తిరిగి (char_u *)&(curbuf->b_p_bt);
	స్థితి PV_BL:	తిరిగి (char_u *)&(curbuf->b_p_bl);
	స్థితి PV_CI:	తిరిగి (char_u *)&(curbuf->b_p_ci);
	స్థితి PV_CIN:	తిరిగి (char_u *)&(curbuf->b_p_cin);
	స్థితి PV_CINK:	తిరిగి (char_u *)&(curbuf->b_p_cink);
	స్థితి PV_CINO:	తిరిగి (char_u *)&(curbuf->b_p_cino);
	స్థితి PV_CINSD:	తిరిగి (char_u *)&(curbuf->b_p_cinsd);
	స్థితి PV_CINW:	తిరిగి (char_u *)&(curbuf->b_p_cinw);
	స్థితి PV_COM:	తిరిగి (char_u *)&(curbuf->b_p_com);
	స్థితి PV_CMS:	తిరిగి (char_u *)&(curbuf->b_p_cms);
	స్థితి PV_CPT:	తిరిగి (char_u *)&(curbuf->b_p_cpt);
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	స్థితి PV_CSL:	తిరిగి (char_u *)&(curbuf->b_p_csl);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_COMPL_FUNC
	స్థితి PV_CFU:	తిరిగి (char_u *)&(curbuf->b_p_cfu);
	స్థితి PV_OFU:	తిరిగి (char_u *)&(curbuf->b_p_ofu);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	స్థితి PV_TFU:	తిరిగి (char_u *)&(curbuf->b_p_tfu);
#అయితే_ముగింపు
	స్థితి PV_EOF:	తిరిగి (char_u *)&(curbuf->b_p_eof);
	స్థితి PV_EOL:	తిరిగి (char_u *)&(curbuf->b_p_eol);
	స్థితి PV_FIXEOL:	తిరిగి (char_u *)&(curbuf->b_p_fixeol);
	స్థితి PV_ET:	తిరిగి (char_u *)&(curbuf->b_p_et);
	స్థితి PV_FENC:	తిరిగి (char_u *)&(curbuf->b_p_fenc);
	స్థితి PV_FF:	తిరిగి (char_u *)&(curbuf->b_p_ff);
	స్థితి PV_FT:	తిరిగి (char_u *)&(curbuf->b_p_ft);
	స్థితి PV_FO:	తిరిగి (char_u *)&(curbuf->b_p_fo);
	స్థితి PV_FLP:	తిరిగి (char_u *)&(curbuf->b_p_flp);
	స్థితి PV_IMI:	తిరిగి (char_u *)&(curbuf->b_p_iminsert);
	స్థితి PV_IMS:	తిరిగి (char_u *)&(curbuf->b_p_imsearch);
	స్థితి PV_INF:	తిరిగి (char_u *)&(curbuf->b_p_inf);
	స్థితి PV_ISK:	తిరిగి (char_u *)&(curbuf->b_p_isk);
#నిర్వచితమైతే FEAT_కనుగొను_ID
# ifdef FEAT_EVAL
	స్థితి PV_INEX:	తిరిగి (char_u *)&(curbuf->b_p_inex);
# endif
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_EVAL)
	స్థితి PV_INDE:	తిరిగి (char_u *)&(curbuf->b_p_inde);
	స్థితి PV_INDK:	తిరిగి (char_u *)&(curbuf->b_p_indk);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	స్థితి PV_FEX:	తిరిగి (char_u *)&(curbuf->b_p_fex);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
	స్థితి PV_కీలకం:	తిరిగి (char_u *)&(curbuf->b_p_కీలకం);
#అయితే_ముగింపు
	స్థితి PV_LISP:	తిరిగి (char_u *)&(curbuf->b_p_lisp);
	స్థితి PV_LOP:	తిరిగి (char_u *)&(curbuf->b_p_lop);
	స్థితి PV_ML:	తిరిగి (char_u *)&(curbuf->b_p_ml);
	స్థితి PV_MPS:	తిరిగి (char_u *)&(curbuf->b_p_mps);
	స్థితి PV_MA:	తిరిగి (char_u *)&(curbuf->b_p_ma);
	స్థితి PV_MOD:	తిరిగి (char_u *)&(curbuf->b_మారింది);
	స్థితి PV_NF:	తిరిగి (char_u *)&(curbuf->b_p_nf);
	స్థితి PV_PI:	తిరిగి (char_u *)&(curbuf->b_p_pi);
	స్థితి PV_QE:	తిరిగి (char_u *)&(curbuf->b_p_qe);
	స్థితి PV_RO:	తిరిగి (char_u *)&(curbuf->b_p_ro);
	స్థితి PV_SI:	తిరిగి (char_u *)&(curbuf->b_p_si);
	స్థితి PV_SN:	తిరిగి (char_u *)&(curbuf->b_p_sn);
	స్థితి PV_STS:	తిరిగి (char_u *)&(curbuf->b_p_sts);
	స్థితి PV_SUA:	తిరిగి (char_u *)&(curbuf->b_p_sua);
	స్థితి PV_SWF:	తిరిగి (char_u *)&(curbuf->b_p_swf);
#నిర్వచితమైతే FEAT_SYN_HL
	స్థితి PV_SMC:	తిరిగి (char_u *)&(curbuf->b_p_smc);
	స్థితి PV_SYN:	తిరిగి (char_u *)&(curbuf->b_p_syn);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
	స్థితి PV_SPC:	తిరిగి (char_u *)&(curwin->w_s->b_p_spc);
	స్థితి PV_SPF:	తిరిగి (char_u *)&(curwin->w_s->b_p_spf);
	స్థితి PV_SPL:	తిరిగి (char_u *)&(curwin->w_s->b_p_spl);
	స్థితి PV_SPO:	తిరిగి (char_u *)&(curwin->w_s->b_p_spo);
#అయితే_ముగింపు
	స్థితి PV_SW:	తిరిగి (char_u *)&(curbuf->b_p_sw);
	స్థితి PV_TS:	తిరిగి (char_u *)&(curbuf->b_p_ts);
	స్థితి PV_TW:	తిరిగి (char_u *)&(curbuf->b_p_tw);
	స్థితి PV_TX:	తిరిగి (char_u *)&(curbuf->b_p_tx);
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
	స్థితి PV_UDF:	తిరిగి (char_u *)&(curbuf->b_p_udf);
#అయితే_ముగింపు
	స్థితి PV_WM:	తిరిగి (char_u *)&(curbuf->b_p_wm);
#నిర్వచితమైతే FEAT_KEYMAP
	స్థితి PV_KMAP:	తిరిగి (char_u *)&(curbuf->b_p_keymap);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
	స్థితి PV_SCL:	తిరిగి (char_u *)&(curwin->w_p_scl);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_VARTABS
	స్థితి PV_VSTS:	తిరిగి (char_u *)&(curbuf->b_p_vsts);
	స్థితి PV_VTS:	తిరిగి (char_u *)&(curbuf->b_p_vts);
#అయితే_ముగింపు
	అప్రమేయ:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    తిరిగి (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(పూర్ణాంకం opt_idx)
{
    తిరిగి options[opt_idx].var;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(పూర్ణాంకం opt_idx)
{
    తిరిగి (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    పూర్ణాంకం
option_info(char_u *name, dict_T *d)
{
    పూర్ణాంకం			opt_idx;
    నిర్మాణం vimoption	*p;
    పూర్ణాంకం			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    ఒకవేళ ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    ఒకవేళ (opt_idx < 0)
	తిరిగి FAIL;

    p = &options[opt_idx];
    dict_కూడు_string(d, "name", (char_u *)p->fullname);
    dict_కూడు_string(d, "shortname",
		    (char_u *)(p->shortname == ఖాళీ ? "" : p->shortname));
    dict_కూడు_bool(d, "available", p->var != ఖాళీ);
    dict_కూడు_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_అంకె) ? "number" : "string"));
    dict_కూడు_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    ఒకవేళ (p->flags & P_BOOL)
	dict_కూడు_bool(d, "default", (పూర్ణాంకం)(long_i)p->def_val[dvi]);
    లేకపోతే ఒకవేళ (p->flags & P_అంకె)
	dict_కూడు_సంఖ్య(d, "default", (దీర్ఘ)(long_i)p->def_val[dvi]);
    లేకపోతే
	dict_కూడు_string(d, "default", p->def_val[dvi] == ఖాళీ
					? (char_u *)"" : p->def_val[dvi]);
    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_సమితి_cb_T
get_option_did_సమితి_cb(పూర్ణాంకం opt_idx)
{
    తిరిగి options[opt_idx].opt_did_సమితి_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(శూన్య)
{
    ఒకవేళ (*curbuf->b_p_ep == NUL)
	తిరిగి p_ep;
    తిరిగి curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(శూన్య)
{
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (*curbuf->b_p_ffu == NUL)
	తిరిగి p_ffu;
    తిరిగి curbuf->b_p_ffu;
#లేకపోతే
    తిరిగి (char_u *)"";
#అయితే_ముగింపు
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    శూన్య
win_copy_options(win_T *wp_from, win_T *wp_గమ్యం)
{
    copy_winopt(&wp_from->w_onebuf_opt, &wp_గమ్యం->w_onebuf_opt);
    copy_winopt(&wp_from->w_allbuf_opt, &wp_గమ్యం->w_allbuf_opt);
#నిర్వచితమైతే FEAT_STL_OPT
    // w_stl_rendered_height is in win_T directly, not in winvar_T, so it is
    // not copied by copy_winopt().  Copy it explicitly here.
    wp_గమ్యం->w_stl_rendered_ఎత్తు = wp_from->w_stl_rendered_ఎత్తు;
#అయితే_ముగింపు
    after_copy_winopt(wp_గమ్యం);
}

/*
 * After copying window options: update variables depending on options.
 */
    శూన్య
after_copy_winopt(win_T *wp)
{
    అక్షరం *errmsg = update_winhighlight(wp, wp->w_p_whl);

    ఒకవేళ (errmsg != ఖాళీ)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    ఒకవేళ (wp->w_p_wrap)
	wp->w_leftcol = 0;
    లేకపోతే
	wp->w_skipcol = 0;
#నిర్వచితమైతే FEAT_LINEBREAK
    briopt_check(ఖాళీ, wp);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
    fill_culopt_flags(ఖాళీ, wp);
    check_colorcolumn(ఖాళీ, wp);
#అయితే_ముగింపు
    సమితి_listchars_option(wp, wp->w_p_lcs, TRUE, ఖాళీ, 0);
    సమితి_fillchars_option(wp, wp->w_p_fcs, TRUE, ఖాళీ, 0);
}

    స్థిర char_u *
copy_option_val(char_u *val)
{
    ఒకవేళ (val == ఖాళీలు_option)
	తిరిగి ఖాళీలు_option;  // no need to allocate memory
    తిరిగి vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    శూన్య
copy_winopt(winopt_T *from, winopt_T *గమ్యం)
{
#నిర్వచితమైతే FEAT_ARABIC
    గమ్యం->wo_arab = from->wo_arab;
#అయితే_ముగింపు
    గమ్యం->wo_list = from->wo_list;
    గమ్యం->wo_lcs = copy_option_val(from->wo_lcs);
    గమ్యం->wo_fcs = copy_option_val(from->wo_fcs);
    గమ్యం->wo_nu = from->wo_nu;
    గమ్యం->wo_rnu = from->wo_rnu;
    గమ్యం->wo_ve = copy_option_val(from->wo_ve);
    గమ్యం->wo_ve_flags = from->wo_ve_flags;
#నిర్వచితమైతే FEAT_LINEBREAK
    గమ్యం->wo_nuw = from->wo_nuw;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RIGHTLEFT
    గమ్యం->wo_rl  = from->wo_rl;
    గమ్యం->wo_rlc = copy_option_val(from->wo_rlc);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    గమ్యం->wo_sbr = copy_option_val(from->wo_sbr);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_STL_OPT
    గమ్యం->wo_stl = copy_option_val(from->wo_stl);
    గమ్యం->wo_stlo = copy_option_val(from->wo_stlo);
    గమ్యం->wo_stlo_fh = from->wo_stlo_fh;
    గమ్యం->wo_stlo_mh = from->wo_stlo_mh;
#అయితే_ముగింపు
    గమ్యం->wo_wrap = from->wo_wrap;
#నిర్వచితమైతే FEAT_DIFF
    గమ్యం->wo_wrap_save = from->wo_wrap_save;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    గమ్యం->wo_lbr = from->wo_lbr;
    గమ్యం->wo_bri = from->wo_bri;
    గమ్యం->wo_briopt = copy_option_val(from->wo_briopt);
#అయితే_ముగింపు
    గమ్యం->wo_wcr = copy_option_val(from->wo_wcr);
    గమ్యం->wo_scb = from->wo_scb;
    గమ్యం->wo_scb_save = from->wo_scb_save;
    గమ్యం->wo_sms = from->wo_sms;
    గమ్యం->wo_crb = from->wo_crb;
    గమ్యం->wo_crb_save = from->wo_crb_save;
    గమ్యం->wo_siso = from->wo_siso;
    గమ్యం->wo_so = from->wo_so;
    గమ్యం->wo_sop = from->wo_sop;
#నిర్వచితమైతే FEAT_SPELL
    గమ్యం->wo_spell = from->wo_spell;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
    గమ్యం->wo_cuc = from->wo_cuc;
    గమ్యం->wo_cul = from->wo_cul;
    గమ్యం->wo_culopt = copy_option_val(from->wo_culopt);
    గమ్యం->wo_cc = copy_option_val(from->wo_cc);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
    గమ్యం->wo_diff = from->wo_diff;
    గమ్యం->wo_diff_saved = from->wo_diff_saved;
#అయితే_ముగింపు
    గమ్యం->wo_eiw = copy_option_val(from->wo_eiw);
#నిర్వచితమైతే FEAT_CONCEAL
    గమ్యం->wo_cocu = copy_option_val(from->wo_cocu);
    గమ్యం->wo_cole = from->wo_cole;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
    గమ్యం->wo_twk = copy_option_val(from->wo_twk);
    గమ్యం->wo_tws = copy_option_val(from->wo_tws);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
    గమ్యం->wo_fdc = from->wo_fdc;
    గమ్యం->wo_fdc_save = from->wo_fdc_save;
    గమ్యం->wo_fen = from->wo_fen;
    గమ్యం->wo_fen_save = from->wo_fen_save;
    గమ్యం->wo_fdi = copy_option_val(from->wo_fdi);
    గమ్యం->wo_fml = from->wo_fml;
    గమ్యం->wo_fdl = from->wo_fdl;
    గమ్యం->wo_fdl_save = from->wo_fdl_save;
    గమ్యం->wo_fdm = copy_option_val(from->wo_fdm);
    గమ్యం->wo_fdm_save = from->wo_diff_saved
			       ? vim_strsave(from->wo_fdm_save) : ఖాళీలు_option;
    గమ్యం->wo_fdn = from->wo_fdn;
# ifdef FEAT_EVAL
    గమ్యం->wo_fde = copy_option_val(from->wo_fde);
    గమ్యం->wo_fdt = copy_option_val(from->wo_fdt);
# endif
    గమ్యం->wo_fmr = copy_option_val(from->wo_fmr);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
    గమ్యం->wo_scl = copy_option_val(from->wo_scl);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
    గమ్యం->wo_lhi = from->wo_lhi;
#అయితే_ముగింపు

    గమ్యం->wo_wrap_flags = from->wo_wrap_flags;
#నిర్వచితమైతే FEAT_STL_OPT
    గమ్యం->wo_stl_flags = from->wo_stl_flags;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
    గమ్యం->wo_fde_flags = from->wo_fde_flags;
    గమ్యం->wo_fdt_flags = from->wo_fdt_flags;
#అయితే_ముగింపు
    గమ్యం->wo_whl = copy_option_val(from->wo_whl);

#నిర్వచితమైతే FEAT_EVAL
    // Copy the script context so that we know where the value was last set.
    mch_memmove(గమ్యం->wo_script_ctx, from->wo_script_ctx,
						    పరిమాణం(గమ్యం->wo_script_ctx));
#అయితే_ముగింపు
    check_winopt(గమ్యం);		// don't want NULL pointers
}

/*
 * Check string options in a window for a NULL value.
 */
    స్థిర శూన్య
check_win_options(win_T *win)
{
    check_winopt(&win->w_onebuf_opt);
    check_winopt(&win->w_allbuf_opt);
}

/*
 * Check for NULL pointers in a winopt_T and replace them with empty_option.
 */
    స్థిర శూన్య
check_winopt(winopt_T *wop వాడని)
{
#నిర్వచితమైతే FEAT_FOLDING
    check_string_option(&wop->wo_fdi);
    check_string_option(&wop->wo_fdm);
    check_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    check_string_option(&wop->wo_fde);
    check_string_option(&wop->wo_fdt);
# endif
    check_string_option(&wop->wo_fmr);
#అయితే_ముగింపు
    check_string_option(&wop->wo_eiw);
#నిర్వచితమైతే FEAT_SIGNS
    check_string_option(&wop->wo_scl);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RIGHTLEFT
    check_string_option(&wop->wo_rlc);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    check_string_option(&wop->wo_sbr);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_STL_OPT
    check_string_option(&wop->wo_stl);
    check_string_option(&wop->wo_stlo);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
    check_string_option(&wop->wo_culopt);
    check_string_option(&wop->wo_cc);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
    check_string_option(&wop->wo_cocu);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
    check_string_option(&wop->wo_twk);
    check_string_option(&wop->wo_tws);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    check_string_option(&wop->wo_briopt);
#అయితే_ముగింపు
    check_string_option(&wop->wo_wcr);
    check_string_option(&wop->wo_lcs);
    check_string_option(&wop->wo_fcs);
    check_string_option(&wop->wo_ve);
    check_string_option(&wop->wo_whl);
}

/*
 * Free the allocated memory inside a winopt_T.
 */
    శూన్య
clear_winopt(winopt_T *wop వాడని)
{
#నిర్వచితమైతే FEAT_FOLDING
    clear_string_option(&wop->wo_fdi);
    clear_string_option(&wop->wo_fdm);
    clear_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    clear_string_option(&wop->wo_fde);
    clear_string_option(&wop->wo_fdt);
# endif
    clear_string_option(&wop->wo_fmr);
#అయితే_ముగింపు
    clear_string_option(&wop->wo_eiw);
#నిర్వచితమైతే FEAT_SIGNS
    clear_string_option(&wop->wo_scl);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    clear_string_option(&wop->wo_briopt);
#అయితే_ముగింపు
    clear_string_option(&wop->wo_wcr);
#నిర్వచితమైతే FEAT_RIGHTLEFT
    clear_string_option(&wop->wo_rlc);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    clear_string_option(&wop->wo_sbr);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_STL_OPT
    clear_string_option(&wop->wo_stl);
    clear_string_option(&wop->wo_stlo);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
    clear_string_option(&wop->wo_culopt);
    clear_string_option(&wop->wo_cc);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
    clear_string_option(&wop->wo_cocu);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
    clear_string_option(&wop->wo_twk);
    clear_string_option(&wop->wo_tws);
#అయితే_ముగింపు
    clear_string_option(&wop->wo_lcs);
    clear_string_option(&wop->wo_fcs);
    clear_string_option(&wop->wo_ve);
    clear_string_option(&wop->wo_whl);
}

#నిర్వచితమైతే FEAT_EVAL
// Index into the options table for a buffer-local option enum.
స్థిర పూర్ణాంకం buf_opt_idx[BV_లెక్క];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    స్థిర శూన్య
init_buf_opt_idx(శూన్య)
{
    స్థిర పూర్ణాంకం did_init_buf_opt_idx = FALSE;
    పూర్ణాంకం i;

    ఒకవేళ (did_init_buf_opt_idx)
	తిరిగి;
    did_init_buf_opt_idx = TRUE;
    కొరకు (i = 0; !istermoption_idx(i); i++)
	ఒకవేళ (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_ముసుగు] = i;
}
#లేకపోతే
# define COPY_OPT_SCTX(buf, bv)
# define COPY_OPT_INSECURE(flagsfield, bv)
#అయితే_ముగింపు

/*
 * Copy global option values to local options for one buffer.
 * Used when creating a new buffer and sometimes when entering a buffer.
 * flags:
 * BCO_ENTER	We will enter the buffer "buf".
 * BCO_ALWAYS	Always copy the options, but only set b_p_initialized when
 *		appropriate.
 * BCO_NOHELP	Don't copy the values to a help buffer.
 */
    శూన్య
buf_copy_options(buf_T *buf, పూర్ణాంకం flags)
{
    పూర్ణాంకం		should_copy = TRUE;
    char_u	*save_p_isk = ఖాళీ;	    // init for GCC
    పూర్ణాంకం		dont_do_help;
    పూర్ణాంకం		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    ఒకవేళ (p_cpo != ఖాళీ)
    {
	/*
	 * Always copy when entering and 'cpo' contains 'S'.
	 * Don't copy when already initialized.
	 * Don't copy when 'cpo' contains 's' and not entering.
	 * 'S'	BCO_ENTER  initialized	's'  should_copy
	 * yes	  yes	       X	 X	TRUE
	 * yes	  no	      yes	 X	FALSE
	 * no	   X	      yes	 X	FALSE
	 *  X	  no	      no	yes	FALSE
	 *  X	  no	      no	no	TRUE
	 * no	  yes	      no	 X	TRUE
	 */
	ఒకవేళ ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == ఖాళీ || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != ఖాళీ)))
	    should_copy = FALSE;

	ఒకవేళ (should_copy || (flags & BCO_ALWAYS))
	{
#నిర్వచితమైతే FEAT_EVAL
	    CLEAR_క్షేత్రం(buf->b_p_script_ctx);
	    init_buf_opt_idx();
#అయితే_ముగింపు
	    // Don't copy the options specific to a help buffer when
	    // BCO_NOHELP is given or the options were initialized already
	    // (jumping back to a help file with CTRL-T or CTRL-O)
	    dont_do_help = ((flags & BCO_NOHELP) && buf->b_help)
						       || buf->b_p_initialized;
	    ఒకవేళ (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = ఖాళీ;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    ఒకవేళ (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		ఎంపిక (*p_ffs)
		{
		    స్థితి 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); ఆపు;
		    స్థితి 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); ఆపు;
		    స్థితి 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); ఆపు;
		    అప్రమేయ:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		ఒకవేళ (buf->b_p_ff != ఖాళీ)
		    buf->b_ప్రారంభం_ffc = *buf->b_p_ff;
		buf->b_p_bh = ఖాళీలు_option;
		buf->b_p_bt = ఖాళీలు_option;
	    }
	    లేకపోతే
		free_buf_options(buf, FALSE);

	    buf->b_p_ai = p_ai;
	    COPY_OPT_SCTX(buf, BV_AI);
	    buf->b_p_ai_nopaste = p_ai_nopaste;
	    buf->b_p_sw = p_sw;
	    COPY_OPT_SCTX(buf, BV_SW);
	    buf->b_p_tw = p_tw;
	    COPY_OPT_SCTX(buf, BV_TW);
	    buf->b_p_tw_nopaste = p_tw_nopaste;
	    buf->b_p_tw_nobin = p_tw_nobin;
	    buf->b_p_wm = p_wm;
	    COPY_OPT_SCTX(buf, BV_WM);
	    buf->b_p_wm_nopaste = p_wm_nopaste;
	    buf->b_p_wm_nobin = p_wm_nobin;
	    buf->b_p_bin = p_bin;
	    COPY_OPT_SCTX(buf, BV_BIN);
	    buf->b_p_bomb = p_bomb;
	    COPY_OPT_SCTX(buf, BV_BOMB);
	    buf->b_p_fixeol = p_fixeol;
	    COPY_OPT_SCTX(buf, BV_FIXEOL);
	    buf->b_p_et = p_et;
	    COPY_OPT_SCTX(buf, BV_ET);
	    buf->b_p_et_nobin = p_et_nobin;
	    buf->b_p_et_nopaste = p_et_nopaste;
	    buf->b_p_ml = p_ml;
	    COPY_OPT_SCTX(buf, BV_ML);
	    buf->b_p_ml_nobin = p_ml_nobin;
	    buf->b_p_inf = p_inf;
	    COPY_OPT_SCTX(buf, BV_INF);
	    ఒకవేళ (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    లేకపోతే
	    {
		buf->b_p_swf = p_swf;
		COPY_OPT_SCTX(buf, BV_SWF);
	    }
	    buf->b_p_cpt = vim_strsave(p_cpt);
	    COPY_OPT_SCTX(buf, BV_CPT);
#నిర్వచితమైతే FEAT_COMPL_FUNC
	    COPY_OPT_INSECURE(buf->b_p_cpt_flags, BV_CPT);
	    సమితి_buflocal_cpt_callbacks(buf);
#అయితే_ముగింపు
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	    buf->b_p_csl = vim_strsave(p_csl);
	    COPY_OPT_SCTX(buf, BV_CSL);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_COMPL_FUNC
	    buf->b_p_cfu = vim_strsave(p_cfu);
	    COPY_OPT_SCTX(buf, BV_CFU);
	    సమితి_buflocal_cfu_callback(buf);
	    buf->b_p_ofu = vim_strsave(p_ofu);
	    COPY_OPT_SCTX(buf, BV_OFU);
	    సమితి_buflocal_ofu_callback(buf);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	    buf->b_p_tfu = vim_strsave(p_tfu);
	    COPY_OPT_SCTX(buf, BV_TFU);
	    సమితి_buflocal_tfu_callback(buf);
#అయితే_ముగింపు
	    buf->b_p_sts = p_sts;
	    COPY_OPT_SCTX(buf, BV_STS);
	    buf->b_p_sts_nopaste = p_sts_nopaste;
#నిర్వచితమైతే FEAT_VARTABS
	    buf->b_p_vsts = vim_strsave(p_vsts);
	    COPY_OPT_SCTX(buf, BV_VSTS);
	    ఒకవేళ (p_vsts && p_vsts != ఖాళీలు_option)
		(శూన్య)tabstop_సమితి(p_vsts, &buf->b_p_vsts_array);
	    లేకపోతే
		buf->b_p_vsts_array = ఖాళీ;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : ఖాళీ;
#అయితే_ముగింపు
	    buf->b_p_sn = p_sn;
	    COPY_OPT_SCTX(buf, BV_SN);
	    buf->b_p_com = vim_strsave(p_com);
	    COPY_OPT_SCTX(buf, BV_COM);
	    buf->b_p_cms = vim_strsave(p_cms);
	    COPY_OPT_SCTX(buf, BV_CMS);
	    buf->b_p_fo = vim_strsave(p_fo);
	    COPY_OPT_SCTX(buf, BV_FO);
	    buf->b_p_flp = vim_strsave(p_flp);
	    COPY_OPT_SCTX(buf, BV_FLP);
	    // NOTE: Valgrind may report a bogus memory leak for 'nrformats'
	    // when it is set to 8 bytes in defaults.vim.
	    buf->b_p_nf = vim_strsave(p_nf);
	    COPY_OPT_SCTX(buf, BV_NF);
	    buf->b_p_mps = vim_strsave(p_mps);
	    COPY_OPT_SCTX(buf, BV_MPS);
	    buf->b_p_si = p_si;
	    COPY_OPT_SCTX(buf, BV_SI);
	    buf->b_p_ci = p_ci;
	    COPY_OPT_SCTX(buf, BV_CI);

	    buf->b_p_cin = p_cin;
	    COPY_OPT_SCTX(buf, BV_CIN);
	    buf->b_p_cink = vim_strsave(p_cink);
	    COPY_OPT_SCTX(buf, BV_CINK);
	    buf->b_p_cino = vim_strsave(p_cino);
	    COPY_OPT_SCTX(buf, BV_CINO);
	    buf->b_p_cinsd = vim_strsave(p_cinsd);
	    COPY_OPT_SCTX(buf, BV_CINSD);
	    buf->b_p_lop = vim_strsave(p_lop);
	    COPY_OPT_SCTX(buf, BV_LOP);

	    // Don't copy 'filetype', it must be detected
	    buf->b_p_ft = ఖాళీలు_option;
	    buf->b_p_pi = p_pi;
	    COPY_OPT_SCTX(buf, BV_PI);
	    buf->b_p_cinw = vim_strsave(p_cinw);
	    COPY_OPT_SCTX(buf, BV_CINW);
	    buf->b_p_lisp = p_lisp;
	    COPY_OPT_SCTX(buf, BV_LISP);
#నిర్వచితమైతే FEAT_SYN_HL
	    // Don't copy 'syntax', it must be set
	    buf->b_p_syn = ఖాళీలు_option;
	    buf->b_p_smc = p_smc;
	    COPY_OPT_SCTX(buf, BV_SMC);
	    buf->b_s.b_syn_isk = ఖాళీలు_option;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
	    buf->b_s.b_p_spc = vim_strsave(p_spc);
	    COPY_OPT_SCTX(buf, BV_SPC);
	    (శూన్య)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#అయితే_ముగింపు
	    buf->b_p_fp = ఖాళీలు_option;
#ఒకవేళ defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
	    buf->b_p_కీలకం = vim_strsave(p_కీలకం);
	    COPY_OPT_SCTX(buf, BV_కీలకం);
#అయితే_ముగింపు
	    buf->b_p_sua = vim_strsave(p_sua);
	    COPY_OPT_SCTX(buf, BV_SUA);
#నిర్వచితమైతే FEAT_KEYMAP
	    buf->b_p_keymap = vim_strsave(p_keymap);
	    COPY_OPT_SCTX(buf, BV_KMAP);
	    buf->b_kmap_state |= KEYMAP_INIT;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
	    buf->b_p_twsl = p_twsl;
	    COPY_OPT_SCTX(buf, BV_TWSL);
#అయితే_ముగింపు
	    // This isn't really an option, but copying the langmap and IME
	    // state from the current buffer is better than resetting it.
	    buf->b_p_iminsert = p_iminsert;
	    COPY_OPT_SCTX(buf, BV_IMI);
	    buf->b_p_imsearch = p_imsearch;
	    COPY_OPT_SCTX(buf, BV_IMS);

	    // options that are normally global but also have a local value
	    // are not copied, start using the global value
	    buf->b_p_ac = -1;
	    buf->b_p_ar = -1;
#నిర్వచితమైతే HAVE_FSYNC
	    buf->b_p_fs = -1;
#అయితే_ముగింపు
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    buf->b_p_bkc = ఖాళీలు_option;
	    buf->b_bkc_flags = 0;
#నిర్వచితమైతే FEAT_QUICKFIX
	    buf->b_p_gefm = ఖాళీలు_option;
	    buf->b_p_gp = ఖాళీలు_option;
	    buf->b_p_mp = ఖాళీలు_option;
	    buf->b_p_efm = ఖాళీలు_option;
#అయితే_ముగింపు
	    buf->b_p_ep = ఖాళీలు_option;
#ఒకవేళ defined(FEAT_EVAL)
	    buf->b_p_ffu = ఖాళీలు_option;
#అయితే_ముగింపు
	    buf->b_p_kp = ఖాళీలు_option;
	    buf->b_p_మార్గం = ఖాళీలు_option;
	    buf->b_p_ట్యాగులు = ఖాళీలు_option;
	    buf->b_p_tc = ఖాళీలు_option;
	    buf->b_tc_flags = 0;
#నిర్వచితమైతే FEAT_కనుగొను_ID
	    buf->b_p_def = ఖాళీలు_option;
	    buf->b_p_inc = ఖాళీలు_option;
# ifdef FEAT_EVAL
	    buf->b_p_inex = vim_strsave(p_inex);
	    COPY_OPT_SCTX(buf, BV_INEX);
	    COPY_OPT_INSECURE(buf->b_p_inex_flags, BV_INEX);
# endif
#అయితే_ముగింపు
	    buf->b_p_cot = ఖాళీలు_option;
	    buf->b_cot_flags = 0;
	    buf->b_p_dict = ఖాళీలు_option;
#నిర్వచితమైతే FEAT_DIFF
	    buf->b_p_dia = ఖాళీలు_option;
#అయితే_ముగింపు
	    buf->b_p_tsr = ఖాళీలు_option;
#నిర్వచితమైతే FEAT_COMPL_FUNC
	    buf->b_p_tsrfu = ఖాళీలు_option;
#అయితే_ముగింపు
	    buf->b_p_qe = vim_strsave(p_qe);
	    COPY_OPT_SCTX(buf, BV_QE);
#ఒకవేళ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = ఖాళీలు_option;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_CRYPT)
	    buf->b_p_cm = ఖాళీలు_option;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
	    buf->b_p_udf = p_udf;
	    COPY_OPT_SCTX(buf, BV_UDF);
#అయితే_ముగింపు
	    buf->b_p_lw = ఖాళీలు_option;
	    buf->b_p_menc = ఖాళీలు_option;

	    // Don't copy the options set by ex_help(), use the saved values,
	    // when going from a help buffer to a non-help buffer.
	    // Don't touch these at all when BCO_NOHELP is used and going from
	    // or to a help buffer.
	    ఒకవేళ (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#నిర్వచితమైతే FEAT_VARTABS
		ఒకవేళ (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (శూన్య)tabstop_సమితి(p_vts, &buf->b_p_vts_array);
		లేకపోతే
		    buf->b_p_vts_array = ఖాళీ;
#అయితే_ముగింపు
	    }
	    లేకపోతే
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#నిర్వచితమైతే FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		ఒకవేళ (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (శూన్య)tabstop_సమితి(p_vts, &buf->b_p_vts_array);
		లేకపోతే
		    buf->b_p_vts_array = ఖాళీ;
#అయితే_ముగింపు
		buf->b_help = false;
		ఒకవేళ (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	ఒకవేళ (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    ఒకవేళ (did_isk)
	(శూన్య)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    శూన్య
reset_modifiable(శూన్య)
{
    పూర్ణాంకం		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    ఒకవేళ (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}

/*
 * Set the global value for 'iminsert' to the local value.
 */
    శూన్య
సమితి_iminsert_global(శూన్య)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    శూన్య
సమితి_imsearch_global(శూన్య)
{
    p_imsearch = curbuf->b_p_imsearch;
}

స్థిర పూర్ణాంకం expand_option_idx = -1;
స్థిర పూర్ణాంకం expand_option_ప్రారంభం_నిలువు = 0;
స్థిర char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
స్థిర పూర్ణాంకం expand_option_flags = 0;
స్థిర పూర్ణాంకం expand_option_append = FALSE;

    శూన్య
సమితి_context_లోపల_సమితి_cmd(
    expand_T	*xp,
    char_u	*arg,
    పూర్ణాంకం		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    పూర్ణాంకం		nextchar;
    long_u	flags = 0;	// init for GCC
    పూర్ణాంకం		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    పూర్ణాంకం		is_పదం_option = FALSE;
    పూర్ణాంకం		కీలకం;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    ఒకవేళ (*arg == NUL)
    {
	xp->xp_నమూనా = arg;
	తిరిగి;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    ఒకవేళ (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_నమూనా = p + 1;
	తిరిగి;
    }
    అయితే (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	ఒకవేళ (*p == ' ' || *p == ',')
	{
	    అయితే (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	ఒకవేళ (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    ఆపు;
	}
	--p;
    }
    ఒకవేళ (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_ఉపసర్గ = XP_ఉపసర్గ_NO;
	p += 2;
    }
    లేకపోతే ఒకవేళ (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_ఉపసర్గ = XP_ఉపసర్గ_INV;
	p += 3;
    }
    xp->xp_నమూనా = arg = p;
    ఒకవేళ (*arg == '<')
    {
	అయితే (*p != '>')
	    ఒకవేళ (*p++ == NUL)	    // expand terminal option name
		తిరిగి;
	కీలకం = get_special_కీలకం_సంకేతం(arg + 1);
	ఒకవేళ (కీలకం == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    తిరిగి;
	}
	nextchar = *++p;
	is_పదం_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(కీలకం);
	expand_option_name[3] = KEY2TERMCAP1(కీలకం);
    }
    లేకపోతే
    {
	ఒకవేళ (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    ఒకవేళ (*p != NUL)
		++p;
	    ఒకవేళ (*p == NUL)
		తిరిగి;		// expand option name
	    nextchar = *++p;
	    is_పదం_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	లేకపోతే
	{
	    // Allow * wildcard
	    అయితే (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    ఒకవేళ (*p == NUL)
		తిరిగి;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    ఒకవేళ (opt_idx == -1 || options[opt_idx].var == ఖాళీ)
	    {
		xp->xp_context = EXPAND_NOTHING;
		తిరిగి;
	    }
	    flags = options[opt_idx].flags;
	    ఒకవేళ (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		తిరిగి;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    పూర్ణాంకం expand_option_వ్యవకలనం = FALSE;
    ఒకవేళ ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	ఒకవేళ (nextchar == '-')
	    expand_option_వ్యవకలనం = TRUE;
	ఒకవేళ (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    ఒకవేళ ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	తిరిగి;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    ఒకవేళ (is_పదం_option)
	expand_option_idx = -1;
    లేకపోతే
	expand_option_idx = opt_idx;

    ఒకవేళ (!is_పదం_option)
    {
	ఒకవేళ (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    తిరిగి;
	}
    }

    xp->xp_నమూనా = p + 1;
    expand_option_ప్రారంభం_నిలువు = (పూర్ణాంకం)(p + 1 - xp->xp_పంక్తి);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#నిర్వచితమైతే FEAT_SYN_HL
    ఒకవేళ (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	తిరిగి;
    }
#అయితే_ముగింపు
    ఒకవేళ (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	తిరిగి;
    }
#నిర్వచితమైతే FEAT_KEYMAP
    ఒకవేళ (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	తిరిగి;
    }
#అయితే_ముగింపు

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    ఒకవేళ (expand_option_వ్యవకలనం)
    {
	xp->xp_context = EXPAND_SETTING_వ్యవకలనం;
	తిరిగి;
    }
    లేకపోతే ఒకవేళ (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != ఖాళీ)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    లేకపోతే ఒకవేళ (*xp->xp_నమూనా == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	తిరిగి;
    }
    లేకపోతే
	xp->xp_context = EXPAND_NOTHING;

    ఒకవేళ (is_పదం_option || (flags & P_అంకె))
	తిరిగి;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    ఒకవేళ (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	ఒకవేళ (p == (char_u *)&p_bdir
		|| p == (char_u *)&p_dir
		|| p == (char_u *)&p_మార్గం
		|| p == (char_u *)&p_pp
		|| p == (char_u *)&p_rtp
		|| p == (char_u *)&p_cdpath
#నిర్వచితమైతే FEAT_SESSION
		|| p == (char_u *)&p_vdir
#అయితే_ముగింపు
		)
	{
	    xp->xp_context = EXPAND_DIRECTORIES;
	    ఒకవేళ (p == (char_u *)&p_మార్గం || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    లేకపోతే
		xp->xp_backslash = XP_BS_ONE;
	}
	లేకపోతే
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    ఒకవేళ (p == (char_u *)&p_ట్యాగులు)
		xp->xp_backslash = XP_BS_THREE;
	    లేకపోతే
		xp->xp_backslash = XP_BS_ONE;
	}
	ఒకవేళ (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    ఒకవేళ ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	కొరకు (p = argend - 1; p >= xp->xp_నమూనా; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    ఒకవేళ (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		అయితే (s > xp->xp_నమూనా && *(s - 1) == '\\')
		    --s;
		ఒకవేళ ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#ఒకవేళ defined(BACKSLASH_లోపల_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#లేకపోతే
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#అయితే_ముగింపు
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_నమూనా = p + 1;
		    ఆపు;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    ఒకవేళ (flags & P_FLAGLIST)
	xp->xp_నమూనా = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#నిర్వచితమైతే FEAT_SPELL
    ఒకవేళ (options[opt_idx].var == (char_u *)&p_sps)
    {
	ఒకవేళ (STRNCMP(xp->xp_నమూనా, "file:", 5) == 0)
	{
	    xp->xp_నమూనా += 5;
	    తిరిగి;
	}
	లేకపోతే ఒకవేళ (options[expand_option_idx].opt_expand_cb != ఖాళీ)
	{
	    xp->xp_context = EXPAND_STRING_SETTING;
	}
    }
#అయితే_ముగింపు
}

/*
 * Returns TRUE if 'str' either matches 'regmatch' or fuzzy matches 'pat'.
 *
 * If 'test_only' is TRUE and 'fuzzy' is FALSE and if 'str' matches the regular
 * expression 'regmatch', then returns TRUE.  Otherwise returns FALSE.
 *
 * If 'test_only' is FALSE and 'fuzzy' is FALSE and if 'str' matches the
 * regular expression 'regmatch', then stores the match in matches[idx] and
 * returns TRUE.
 *
 * If 'test_only' is TRUE and 'fuzzy' is TRUE and if 'str' fuzzy matches
 * 'fuzzystr', then returns TRUE. Otherwise returns FALSE.
 *
 * If 'test_only' is FALSE and 'fuzzy' is TRUE and if 'str' fuzzy matches
 * 'fuzzystr', then stores the match details in fuzmatch[idx] and returns TRUE.
 */
    స్థిర పూర్ణాంకం
సరిపోలిక_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	పూర్ణాంకం		idx,
	పూర్ణాంకం		test_only,
	పూర్ణాంకం		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    ఒకవేళ (!fuzzy)
    {
	ఒకవేళ (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    ఒకవేళ (!test_only)
		matches[idx] = vim_strsave(str);
	    తిరిగి TRUE;
	}
    }
    లేకపోతే
    {
	పూర్ణాంకం score;

	score = fuzzy_సరిపోలిక_str(str, fuzzystr);
	ఒకవేళ (score != FUZZY_SCORE_NONE)
	{
	    ఒకవేళ (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    తిరిగి TRUE;
	}
    }

    తిరిగి FALSE;
}

    పూర్ణాంకం
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    పూర్ణాంకం		*అంకె_Matches,
    char_u	***matches,
    పూర్ణాంకం		can_fuzzy)
{
    పూర్ణాంకం		అంకె_సాధారణ = 0;	    // Nr of matching non-term-code settings
    పూర్ణాంకం		అంకె_పదం = 0;	    // Nr of matching terminal code settings
    పూర్ణాంకం		opt_idx;
    పూర్ణాంకం		సరిపోలిక;
    పూర్ణాంకం		లెక్క = 0;
    char_u	*str;
    పూర్ణాంకం		loop;
    పూర్ణాంకం		is_పదం_opt;
    char_u	name_buf[MAX_కీలకం_NAME_LEN];
    స్థిర అక్షరం *(పేర్లు[]) = {"all", "termcap"};
    పూర్ణాంకం		ic = regmatch->rm_ic;	// remember the ignore-case flag
    పూర్ణాంకం		fuzzy;
    fuzmatch_str_T  *fuzmatch = ఖాళీ;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    కొరకు (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	ఒకవేళ (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    కొరకు (సరిపోలిక = 0; సరిపోలిక < (పూర్ణాంకం)ARRAY_నిడివి(పేర్లు); ++సరిపోలిక)
	    {
		ఒకవేళ (సరిపోలిక_str((char_u *)పేర్లు[సరిపోలిక], regmatch, *matches,
			    లెక్క, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    ఒకవేళ (loop == 0)
			అంకె_సాధారణ++;
		    లేకపోతే
			లెక్క++;
		}
	    }
	}
	కొరకు (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != ఖాళీ;
								    opt_idx++)
	{
	    ఒకవేళ (options[opt_idx].var == ఖాళీ)
		కొనసాగు;
	    ఒకవేళ (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		కొనసాగు;
	    is_పదం_opt = istermoption_idx(opt_idx);
	    ఒకవేళ (is_పదం_opt && అంకె_సాధారణ > 0)
		కొనసాగు;

	    ఒకవేళ (సరిపోలిక_str(str, regmatch, *matches, లెక్క, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		ఒకవేళ (loop == 0)
		{
		    ఒకవేళ (is_పదం_opt)
			అంకె_పదం++;
		    లేకపోతే
			అంకె_సాధారణ++;
		}
		లేకపోతే
		    లెక్క++;
	    }
	    లేకపోతే ఒకవేళ (!fuzzy && options[opt_idx].shortname != ఖాళీ
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		ఒకవేళ (loop == 0)
		{
		    ఒకవేళ (is_పదం_opt)
			అంకె_పదం++;
		    లేకపోతే
			అంకె_సాధారణ++;
		}
		లేకపోతే
		    (*matches)[లెక్క++] = vim_strsave(str);
	    }
	    లేకపోతే ఒకవేళ (is_పదం_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		ఒకవేళ (సరిపోలిక_str(name_buf, regmatch, *matches, లెక్క, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    ఒకవేళ (loop == 0)
			అంకె_పదం++;
		    లేకపోతే
			లెక్క++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	ఒకవేళ (xp->xp_context != EXPAND_BOOL_SETTINGS  && అంకె_సాధారణ == 0)
	{
	    కొరకు (opt_idx = 0; (str = get_termcode(opt_idx)) != ఖాళీ; opt_idx++)
	    {
		ఒకవేళ (!సురక్షిత_isprint(str[0]) || !సురక్షిత_isprint(str[1]))
		    కొనసాగు;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		ఒకవేళ (సరిపోలిక_str(name_buf, regmatch, *matches, లెక్క,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    ఒకవేళ (loop == 0)
			అంకె_పదం++;
		    లేకపోతే
			లెక్క++;
		}
		లేకపోతే
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    ఒకవేళ (సరిపోలిక_str(name_buf, regmatch, *matches, లెక్క,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			ఒకవేళ (loop == 0)
			    అంకె_పదం++;
			లేకపోతే
			    లెక్క++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    కొరకు (opt_idx = 0; (str = get_కీలకం_name(opt_idx)) != ఖాళీ; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		ఒకవేళ (సరిపోలిక_str(name_buf, regmatch, *matches, లెక్క, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    ఒకవేళ (loop == 0)
			అంకె_పదం++;
		    లేకపోతే
			లెక్క++;
		}
	    }
	}
	ఒకవేళ (loop == 0)
	{
	    ఒకవేళ (అంకె_సాధారణ > 0)
		*అంకె_Matches = అంకె_సాధారణ;
	    లేకపోతే ఒకవేళ (అంకె_పదం > 0)
		*అంకె_Matches = అంకె_పదం;
	    లేకపోతే
		తిరిగి OK;
	    ఒకవేళ (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *అంకె_Matches);
		ఒకవేళ (*matches == ఖాళీ)
		{
		    *matches = (char_u **)"";
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *అంకె_Matches);
		ఒకవేళ (fuzmatch == ఖాళీ)
		{
		    *matches = (char_u **)"";
		    తిరిగి FAIL;
		}
	    }
	}
    }

    ఒకవేళ (fuzzy &&
	fuzzymatches_గమ్యం_strmatches(fuzmatch, matches, లెక్క, FALSE) == FAIL)
	తిరిగి FAIL;

    తిరిగి OK;
}

// Escape an option value that can be used on the command-line with :set.
// Caller needs to free the returned string, unless NULL is returned.
    స్థిర char_u*
escape_option_str_cmdline(char_u *var)
{
    char_u *buf;

    // A backslash is required before some characters.  This is the reverse of
    // what happens in do_set().
    buf = vim_strsave_escaped(var, escape_chars);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి ఖాళీ;

#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    కొరకు (var = buf; *var != NUL; MB_PTR_ADV(var))
	ఒకవేళ (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#అయితే_ముగింపు
    తిరిగి buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    పూర్ణాంకం
ExpandOldSetting(పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    char_u  *var = ఖాళీ;	// init for GCC
    char_u  *buf;

    *అంకె_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    ఒకవేళ (*matches == ఖాళీ)
	తిరిగి FAIL;

    // For a terminal key code expand_option_idx is < 0.
    ఒకవేళ (expand_option_idx < 0)
    {
	var = కనుగొను_termcode(expand_option_name + 2);
	ఒకవేళ (var == ఖాళీ)
	    expand_option_idx = findoption(expand_option_name);
    }

    ఒకవేళ (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    లేకపోతే ఒకవేళ (var == ఖాళీ)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    ఒకవేళ (buf == ఖాళీ)
    {
	VIM_CLEAR(*matches);
	తిరిగి FAIL;
    }

    (*matches)[0] = buf;
    *అంకె_Matches = 1;
    తిరిగి OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    పూర్ణాంకం
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    పూర్ణాంకం		*అంకె_Matches,
    char_u	***matches)
{
    char_u  *var = ఖాళీ;	// init for GCC
    char_u  *buf;

    ఒకవేళ (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == ఖాళీ)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	తిరిగి FAIL;
    }

    optexpand_T వాదనలు;
    వాదనలు.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    వాదనలు.oe_append = expand_option_append;
    వాదనలు.oe_regmatch = regmatch;
    వాదనలు.oe_xp = xp;
    వాదనలు.oe_సమితి_arg = xp->xp_పంక్తి + expand_option_ప్రారంభం_నిలువు;
    వాదనలు.oe_include_orig_val =
	!expand_option_append &&
	(*వాదనలు.oe_సమితి_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి FAIL;

    వాదనలు.oe_opt_విలువ = buf;

    పూర్ణాంకం అంకె_ret = options[expand_option_idx].opt_expand_cb(&వాదనలు, అంకె_Matches, matches);

    vim_free(buf);
    తిరిగి అంకె_ret;
}

/*
 * Expansion handler for :set-=
 */
    పూర్ణాంకం
Expand_Setting_వ్యవకలనం(
    expand_T	*xp,
    regmatch_T	*regmatch,
    పూర్ణాంకం		*అంకె_Matches,
    char_u	***matches)
{
    ఒకవేళ (expand_option_idx < 0)
	// term option
	తిరిగి ExpandOldSetting(అంకె_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    ఒకవేళ (option_flags & P_అంకె)
	తిరిగి ExpandOldSetting(అంకె_Matches, matches);
    లేకపోతే ఒకవేళ (option_flags & P_COMMA)
    {
	// Split the option by comma, then present each option to the user if
	// it matches the pattern.
	// This condition needs to go first, because 'whichwrap' has both
	// P_COMMA and P_FLAGLIST.
	garray_T    ga;

	char_u	    *అంశం;
	char_u	    *option_copy;
	char_u	    *తదుపరి_val;
	char_u	    *comma;

	ఒకవేళ (*option_val == NUL)
	    తిరిగి FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	ఒకవేళ (option_copy == ఖాళీ)
	    తిరిగి FAIL;
	తదుపరి_val = option_copy;

	ga_init2(&ga, పరిమాణం(char_u *), 10);

	చేయి
	{
	    అంశం = తదుపరి_val;
	    comma = vim_strchr(తదుపరి_val, ',');
	    అయితే (comma != ఖాళీ && comma != తదుపరి_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    ఒకవేళ (comma != ఖాళీ)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		తదుపరి_val = comma + 1;
	    }
	    లేకపోతే
		తదుపరి_val = ఖాళీ;

	    ఒకవేళ (*అంశం == NUL)
		// empty value, don't add to list
		కొనసాగు;

	    ఒకవేళ (!vim_regexec(regmatch, అంశం, (colnr_T)0))
		కొనసాగు;

	    char_u *buf = escape_option_str_cmdline(అంశం);
	    ఒకవేళ (buf == ఖాళీ)
	    {
		vim_free(option_copy);
		ga_clear_అక్షరశ్రేణులు(&ga);
		తిరిగి FAIL;
	    }
	    ఒకవేళ (ga_కూడు_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		ఆపు;
	    }
	} అయితే (తదుపరి_val != ఖాళీ);

	vim_free(option_copy);

	*matches = ga.ga_దత్తాంశం;
	*అంకె_Matches = ga.ga_len;
	తిరిగి OK;
    }
    లేకపోతే ఒకవేళ (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	ఒకవేళ (*xp->xp_నమూనా != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    తిరిగి FAIL;
	}

	size_t అంకె_flags = STRLEN(option_val);
	ఒకవేళ (అంకె_flags == 0)
	    తిరిగి FAIL;

	*matches = ALLOC_MULT(char_u *, అంకె_flags + 1);
	ఒకవేళ (*matches == ఖాళీ)
	    తిరిగి FAIL;

	పూర్ణాంకం లెక్క = 0;
	char_u *p;

	p = vim_strnsave(option_val, అంకె_flags);
	ఒకవేళ (p == ఖాళీ)
	{
	    VIM_CLEAR(*matches);
	    తిరిగి FAIL;
	}
	(*matches)[లెక్క++] = p;

	ఒకవేళ (అంకె_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    కొరకు (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		ఒకవేళ (p == ఖాళీ)
		    ఆపు;
		(*matches)[లెక్క++] = p;
	    }
	}

	*అంకె_Matches = లెక్క;
	తిరిగి OK;
    }

    తిరిగి ExpandOldSetting(అంకె_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    స్థిర శూన్య
option_value2string(
    నిర్మాణం vimoption	*opp,
    పూర్ణాంకం			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    ఒకవేళ (opp->flags & P_అంకె)
    {
	దీర్ఘ wc = 0;

	ఒకవేళ (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_కీలకం_name((పూర్ణాంకం)wc, 0));
	లేకపోతే ఒకవేళ (wc != 0)
	    STRCPY(NameBuff, transchar((పూర్ణాంకం)wc));
	లేకపోతే
	    sprintf((అక్షరం *)NameBuff, "%ld", *(దీర్ఘ *)varp);
    }
    లేకపోతే    // P_STRING
    {
	varp = *(char_u **)(varp);
	ఒకవేళ (varp == ఖాళీ)		    // just in case
	    NameBuff[0] = NUL;
#నిర్వచితమైతే FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	లేకపోతే ఒకవేళ (opp->var == (char_u *)&p_కీలకం && *varp)
	    STRCPY(NameBuff, "*****");
#అయితే_ముగింపు
	లేకపోతే ఒకవేళ (opp->flags & P_EXPAND)
	    home_replace(ఖాళీ, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	లేకపోతే ఒకవేళ ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	లేకపోతే
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    స్థిర పూర్ణాంకం
wc_use_keyname(char_u *varp, దీర్ఘ *wcp)
{
    ఒకవేళ (((దీర్ఘ *)varp == &p_wc) || ((దీర్ఘ *)varp == &p_wcm))
    {
	*wcp = *(దీర్ఘ *)varp;
	ఒకవేళ (IS_SPECIAL(*wcp) || కనుగొను_special_కీలకం_లోపల_పట్టిక((పూర్ణాంకం)*wcp) >= 0)
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    పూర్ణాంకం
shortmess(పూర్ణాంకం x)
{
    తిరిగి p_shm != ఖాళీ &&
	    (   vim_strchr(p_shm, x) != ఖాళీ
	    || (vim_strchr(p_shm, 'a') != ఖాళీ
		&& vim_strchr((char_u *)SHM_A, x) != ఖాళీ));
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 * When "fname" is not NULL, use it to set $"envname" when it wasn't set yet.
 */
    శూన్య
vimrc_దొరికింది(char_u *fname, char_u *envname)
{
    పూర్ణాంకం		opt_idx;
    పూర్ణాంకం		dofree = FALSE;
    char_u	*p;

    ఒకవేళ (!option_was_సమితి((char_u *)"cp"))
    {
	p_cp = FALSE;
	కొరకు (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    ఒకవేళ (!(options[opt_idx].flags & (P_WAS_సమితి|P_VI_DEF)))
		సమితి_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    ఒకవేళ (fname != ఖాళీ)
    {
	p = vim_getenv(envname, &dofree);
	ఒకవేళ (p == ఖాళీ)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = నిండిన_Name_save(fname, FALSE);
	    ఒకవేళ (p != ఖాళీ)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	లేకపోతే ఒకవేళ (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    శూన్య
change_compatible(పూర్ణాంకం on)
{
    పూర్ణాంకం	    opt_idx;

    ఒకవేళ (p_cp != on)
    {
	p_cp = on;
	compatible_సమితి();
    }
    opt_idx = findoption((char_u *)"cp");
    ఒకవేళ (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_సమితి;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    పూర్ణాంకం
option_was_సమితి(char_u *name)
{
    పూర్ణాంకం idx;

    idx = findoption(name);
    ఒకవేళ (idx < 0)	// unknown option
	తిరిగి FALSE;
    ఒకవేళ (options[idx].flags & P_WAS_సమితి)
	తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    పూర్ణాంకం
reset_option_was_సమితి(char_u *name)
{
    పూర్ణాంకం idx = findoption(name);

    ఒకవేళ (idx < 0)
	తిరిగి FAIL;

    options[idx].flags &= ~P_WAS_సమితి;
    తిరిగి OK;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    స్థిర శూన్య
compatible_సమితి(శూన్య)
{
    పూర్ణాంకం	    opt_idx;

    కొరకు (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	ఒకవేళ (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    సమితి_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    పూర్ణాంకం
can_bs(
    పూర్ణాంకం		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#నిర్వచితమైతే FEAT_పని_CHANNEL
    ఒకవేళ (what == BS_ప్రారంభం && bt_prompt(curbuf))
	తిరిగి FALSE;
#అయితే_ముగింపు
    ఎంపిక (*p_bs)
    {
	స్థితి '3':       తిరిగి TRUE;
	స్థితి '2':	తిరిగి (what != BS_NOSTOP);
	స్థితి '1':	తిరిగి (what != BS_ప్రారంభం);
	స్థితి '0':	తిరిగి FALSE;
    }
    తిరిగి vim_strchr(p_bs, what) != ఖాళీ;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    దీర్ఘ
get_scrolloff_విలువ(శూన్య)
{
    తిరిగి curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    దీర్ఘ
get_scrolloffpad_విలువ(శూన్య)
{
    తిరిగి curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    దీర్ఘ
get_sidescrolloff_విలువ(శూన్య)
{
    తిరిగి curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    గుర్తులేని పూర్ణాంకం
get_bkc_flags(buf_T *buf)
{
    తిరిగి buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#ఒకవేళ defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_విలువ(buf_T *buf)
{
    ఒకవేళ (buf->b_p_flp == ఖాళీ || *buf->b_p_flp == NUL)
	తిరిగి p_flp;
    తిరిగి buf->b_p_flp;
}
#అయితే_ముగింపు

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    గుర్తులేని పూర్ణాంకం
get_ve_flags(శూన్య)
{
    తిరిగి (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#ఒకవేళ defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_విలువ(win_T *win)
{
    ఒకవేళ (win->w_p_sbr == ఖాళీ || *win->w_p_sbr == NUL)
	తిరిగి p_sbr;
    ఒకవేళ (STRCMP(win->w_p_sbr, "NONE") == 0)
	తిరిగి ఖాళీలు_option;
    తిరిగి win->w_p_sbr;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(పూర్ణాంకం bufopt)
{
    dict_T	*d;
    పూర్ణాంకం		opt_idx;

    d = dict_alloc();
    ఒకవేళ (d == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	నిర్మాణం vimoption *opt = &options[opt_idx];

	ఒకవేళ ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    ఒకవేళ (varp != ఖాళీ)
	    {
		ఒకవేళ (opt->flags & P_STRING)
		    dict_కూడు_string(d, opt->fullname, *(char_u **)varp);
		లేకపోతే ఒకవేళ (opt->flags & P_అంకె)
		    dict_కూడు_సంఖ్య(d, opt->fullname, *(దీర్ఘ *)varp);
		లేకపోతే
		    dict_కూడు_సంఖ్య(d, opt->fullname, *(పూర్ణాంకం *)varp);
	    }
	}
    }

    తిరిగి d;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    పూర్ణాంకం
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    ఒకవేళ (val == ఖాళీ)
	p = wp->w_p_culopt;
    లేకపోతే
	p = val;
    అయితే (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	ఒకవేళ (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_పంక్తి;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_పంక్తి | CULOPT_NBR;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	ఒకవేళ (*p != ',' && *p != NUL)
	    తిరిగి FAIL;
	ఒకవేళ (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    ఒకవేళ ((culopt_flags_new & CULOPT_పంక్తి) && (culopt_flags_new & CULOPT_SCRLINE))
	తిరిగి FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    పూర్ణాంకం
magic_isset(శూన్య)
{
    ఎంపిక (magic_overruled)
    {
	స్థితి OPTION_MAGIC_ON:      తిరిగి TRUE;
	స్థితి OPTION_MAGIC_OFF:     తిరిగి FALSE;
	స్థితి OPTION_MAGIC_NOT_సమితి: ఆపు;
    }
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (లోపల_vim9script())
	తిరిగి TRUE;
#అయితే_ముగింపు
    తిరిగి p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    పూర్ణాంకం
option_సమితి_callback_func(char_u *optval వాడని, callback_T *optcb వాడని)
{
#నిర్వచితమైతే FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    పూర్ణాంకం		funcname = FALSE;

    ఒకవేళ (optval == ఖాళీ || *optval == NUL)
    {
	free_callback(optcb);
	తిరిగి OK;
    }

    ఒకవేళ (*optval == '{' || (లోపల_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_సమాసం(optval, ఖాళీ);
    లేకపోతే
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    ఒకవేళ (tv == ఖాళీ)
	తిరిగి FAIL;

    cb = get_callback(tv);
    ఒకవేళ (cb.cb_name == ఖాళీ || *cb.cb_name == NUL)
    {
	free_tv(tv);
	తిరిగి FAIL;
    }

    free_callback(optcb);
    సమితి_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = ఖాళీ;
    ఒకవేళ (లోపల_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != ఖాళీ))
    {
	ఒకవేళ (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = కనుగొను_imported(optval, dot - optval, FALSE);
	ఒకవేళ (import != ఖాళీ && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((అక్షరం *)fnamebuf, పరిమాణం(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	లేకపోతే
	{
	    // Imported autoloaded function.  Store the function name as
	    // "import.funcname".
	    optcb->cb_name = vim_strsave(optval);
	    optcb->cb_free_name = TRUE;
	}
    }
    // when using Vim9 style "import.funcname" it needs to be expanded to
    // "import#funcname".
    expand_autload_callback(optcb);

    తిరిగి OK;
#లేకపోతే
    తిరిగి FAIL;
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    అక్షరం *
did_సమితి_showtabpanel(optset_T *వాదనలు వాడని)
{
    shell_new_columns();
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
    స్థిర శూన్య
didset_options_sctx(పూర్ణాంకం opt_flags, అక్షరం **buf)
{
	కొరకు (పూర్ణాంకం i = 0; ; ++i)
	{
	    ఒకవేళ (buf[i] == ఖాళీ)
		ఆపు;

	    పూర్ణాంకం idx = findoption((char_u *)buf[i]);
	    ఒకవేళ (idx >= 0)
		సమితి_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#అయితే_ముగింపు
