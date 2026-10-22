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

#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_OPTION_C
#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "optiondefs.h"

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_options_default(ಪೂರ್ಣಾಂಕ opt_flags);
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_string_default_esc(ಅಕ್ಷರ *name, char_u *val, ಪೂರ್ಣಾಂಕ escape);
ಸ್ಥಿರ char_u *ಕಂಡುಹಿಡಿ_dup_ಅಂಶ(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
ಸ್ಥಿರ char_u *option_expand(ಪೂರ್ಣಾಂಕ opt_idx, char_u *val);
ಸ್ಥಿರ ಶೂನ್ಯ didset_options(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ didset_options2(ಶೂನ್ಯ);
#ಆದರೆ defined(FEAT_EVAL)
ಸ್ಥಿರ long_u *insecure_flag(win_T *wp, ಪೂರ್ಣಾಂಕ opt_idx, ಪೂರ್ಣಾಂಕ opt_flags);
#ಇಲ್ಲದಿದ್ದರೆ
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ *ಗಣ_bool_option(ಪೂರ್ಣಾಂಕ opt_idx, char_u *varp, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ opt_flags);
ಸ್ಥಿರ ಅಕ್ಷರ *ಗಣ_ಅಂಕೆ_option(ಪೂರ್ಣಾಂಕ opt_idx, char_u *varp, ದೀರ್ಘ ಮೌಲ್ಯ, ಅಕ್ಷರ *errbuf, size_t errbuflen, ಪೂರ್ಣಾಂಕ opt_flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_ಕೀಲಿ_option(char_u *arg_arg, ಪೂರ್ಣಾಂಕ has_lt);
ಸ್ಥಿರ ಶೂನ್ಯ showoptions(ಪೂರ್ಣಾಂಕ all, ಪೂರ್ಣಾಂಕ opt_flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ optval_default(ರಚನೆ vimoption *, char_u *varp, ಪೂರ್ಣಾಂಕ compatible);
ಸ್ಥಿರ ಶೂನ್ಯ showoneopt(ರಚನೆ vimoption *, ಪೂರ್ಣಾಂಕ opt_flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ put_setstring(FILE *fd, bool legacy, ಅಕ್ಷರ *cmd, ಅಕ್ಷರ *name, char_u **valuep, long_u flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ put_setnum(FILE *fd, ಅಕ್ಷರ *cmd, ಅಕ್ಷರ *name, ದೀರ್ಘ *valuep);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ put_setbool(FILE *fd, ಅಕ್ಷರ *cmd, ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ istermoption(ರಚನೆ vimoption *p);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ istermoption_idx(ಪೂರ್ಣಾಂಕ opt_idx);
ಸ್ಥಿರ char_u *get_varp_scope(ರಚನೆ vimoption *p, ಪೂರ್ಣಾಂಕ scope);
ಸ್ಥಿರ char_u *get_varp(ರಚನೆ vimoption *);
ಸ್ಥಿರ ಶೂನ್ಯ check_win_options(win_T *win);
ಸ್ಥಿರ ಶೂನ್ಯ option_value2string(ರಚನೆ vimoption *, ಪೂರ್ಣಾಂಕ scope);
ಸ್ಥಿರ ಶೂನ್ಯ check_winopt(winopt_T *wop);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ wc_use_keyname(char_u *varp, ದೀರ್ಘ *wcp);
ಸ್ಥಿರ ಶೂನ್ಯ compatible_ಗಣ(ಶೂನ್ಯ);

#ಆದರೆ defined(FEAT_EVAL)
ಸ್ಥಿರ ಅಕ್ಷರ *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    ಖಾಲಿ};
ಸ್ಥಿರ ಶೂನ್ಯ didset_options_sctx(ಪೂರ್ಣಾಂಕ opt_flags, ಅಕ್ಷರ **buf);
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Initialize the 'shell' option to a default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_init_default_shell(ಶೂನ್ಯ)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    ಆದರೆ (((p = mch_getenv((char_u *)"SHELL")) != ಖಾಲಿ && *p != NUL)
#ಆದರೆ defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != ಖಾಲಿ && *p != NUL)
	    || ((p = (char_u *)default_shell()) != ಖಾಲಿ && *p != NUL)
#ಆದರೆ_ಅಂತ್ಯ
	    )
#ಆದರೆ defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	ಆದರೆ (vim_strchr(p, ' ') != ಖಾಲಿ)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    ಆದರೆ (cmd != ಖಾಲಿ)
	    {
		vim_snprintf((ಅಕ್ಷರ *)cmd, len, "\"%s\"", p);
		ಗಣ_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಣ_string_default("sh", p);
    }
#ಇಲ್ಲದಿದ್ದರೆ
	ಗಣ_string_default_esc("sh", p, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_init_default_backupskip(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		plen;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಸ್ಥಿರ ಅಕ್ಷರ	*(ಹೆಸರುಗಳು[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ ಅಕ್ಷರ	*(ಹೆಸರುಗಳು[3]) = {"TMPDIR", "TEMP", "TMP"};
#ಆದರೆ_ಅಂತ್ಯ
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(ಹೆಸರುಗಳು); ++i)
    {
	ಪೂರ್ಣಾಂಕ		mustfree = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	ಆದರೆ (*ಹೆಸರುಗಳು[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL("/private/tmp");
# ಇಲ್ಲದಿದ್ದರೆ
	    p = (char_u *)"/tmp";
	    plen = (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL("/tmp");
# endif
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    p = vim_getenv((char_u *)ಹೆಸರುಗಳು[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	ಆದರೆ (p != ಖಾಲಿ && *p != NUL)
	{
	    char_u  *ಅಂಶ;
	    size_t  itemsize;
	    ಪೂರ್ಣಾಂಕ	    has_trailing_ಮಾರ್ಗ_sep = FALSE;

	    ಆದರೆ (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (ಪೂರ್ಣಾಂಕ)STRLEN(p);
		// does the value include a trailing path separator?
		ಆದರೆ (after_pathsep(p, p + plen))
		    has_trailing_ಮಾರ್ಗ_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_ಮಾರ್ಗ_sep ? 0 : 1) + 2;
	    ಅಂಶ = alloc(itemsize);
	    ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((ಅಕ್ಷರ *)ಅಂಶ, itemsize, "%s%s*", p, (has_trailing_ಮಾರ್ಗ_sep) ? "" : PATHSEPSTR);

		ಆದರೆ (ಕಂಡುಹಿಡಿ_dup_ಅಂಶ(ga.ga_ದತ್ತಾಂಶ, ಅಂಶ, itemlen, options[opt_idx].flags) == ಖಾಲಿ
			&& ga_grow(&ga, (ಪೂರ್ಣಾಂಕ)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", ಅಂಶ);
		}
		vim_free(ಅಂಶ);
	    }
	}
	ಆದರೆ (mustfree)
	    vim_free(p);
    }
    ಆದರೆ (ga.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
    {
	ಗಣ_string_default("bsk", ga.ga_ದತ್ತಾಂಶ);
	vim_free(ga.ga_ದತ್ತಾಂಶ);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_init_default_maxmemtot(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    ಆದರೆ (opt_idx < 0)
	ಹಿಂತಿರುಗಿ;

#ಆದರೆ !defined(HAVE_AVAIL_MEM) && !defined(HAVE_ಒಟ್ಟು_MEM)
    ಆದರೆ (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#ಆದರೆ_ಅಂತ್ಯ
    {
#ಆದರೆ defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#ಅಥವಾಆದರೆ defined(HAVE_ಒಟ್ಟು_MEM)
	// Use amount of memory available to Vim.
	n = (mch_ಒಟ್ಟು_mem(FALSE) >> 1);
#ಇಲ್ಲದಿದ್ದರೆ
	n = (0x7fffffff >> 11);
#ಆದರೆ_ಅಂತ್ಯ
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	ಆದರೆ (opt_idx >= 0)
	{
#ಆದರೆ !defined(HAVE_AVAIL_MEM) && !defined(HAVE_ಒಟ್ಟು_MEM)
	    ಆದರೆ ((ದೀರ್ಘ)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (ದೀರ್ಘ)n
		    || (ದೀರ್ಘ)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#ಆದರೆ_ಅಂತ್ಯ
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_init_default_cdpath(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    ಪೂರ್ಣಾಂಕ	i;
    ಪೂರ್ಣಾಂಕ	j;
    ಪೂರ್ಣಾಂಕ	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    ಆದರೆ (cdpath == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    ಆದರೆ (buf != ಖಾಲಿ)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	ಗಾಗಿ (i = 0; cdpath[i] != NUL; ++i)
	{
	    ಆದರೆ (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	ಆದರೆ (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_free(buf); // cannot happen
    }
    ಆದರೆ (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_init_default_printencoding(ಶೂನ್ಯ)
{
#ಆದರೆ defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    ಗಣ_string_default("penc",
# ಆದರೆ defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# ಇಲ್ಲದಿದ್ದರೆ // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_init_default_printexpr(ಶೂನ್ಯ)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    ಗಣ_string_default("pexpr",
# ಆದರೆ defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# ಇಲ್ಲದಿದ್ದರೆ
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_init_restricted_ಬಹುಲಕ(ಶೂನ್ಯ)
{
    char_u	*p;

    p = get_isolated_shell_name();
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_init_clean_rtp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    ಆದರೆ (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    ಆದರೆ (opt_idx < 0)
	ಹಿಂತಿರುಗಿ;
    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
    p_pp = (char_u *)CLEAN_RUNTIMEPATH;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
/*
 * Change 'runtimepath' and 'packdir' to '$XDG_CONFIG_HOME/vim' if the only
 * vimrc found is located in '$XDG_CONFIG_HOME/vim/vimrc'.
 * In case the '$XDG_CONFIG_HOME' variable is not set, '$HOME/.config' is used
 * as a fallback as is defined in the XDG base dir specification:
 * <https://specifications.freedesktop.org/basedir/latest/>
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_init_xdg_rtp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;
    ಪೂರ್ಣಾಂಕ		has_xdg_env = TRUE;
    ಪೂರ್ಣಾಂಕ		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = ಖಾಲಿ;
    char_u	*vimrc2 = ಖಾಲಿ;
    char_u	*xdg_dir = ಖಾಲಿ;
    char_u	*xdg_rtp = ಖಾಲಿ;
    string_T	vimrc_xdg = {ಖಾಲಿ, 0};

    // initialize chartab, so we can expand $HOME
    (ಶೂನ್ಯ)init_chartab();
    vimrc1 = expand_env_save((char_u *)USR_VIMRC_FILE);
    vimrc2 = expand_env_save((char_u *)USR_VIMRC_FILE2);

    xdg_dir = mch_getenv("XDG_CONFIG_HOME");
    ಆದರೆ (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    ಆದರೆ (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	ಹೋಗು theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    ಆದರೆ ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	ಹೋಗು theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    ಆದರೆ ((opt_idx = findoption((char_u *)"packpath")) < 0)
	ಹೋಗು theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# ಆದರೆ defined(XDG_VDIR) && defined(FEAT_SESSION)
    ಆದರೆ ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	ಹೋಗು theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    ಆದರೆ (should_free_xdg_dir)
	vim_free(xdg_dir);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Expand environment variables and things like "~" for the defaults.
 * If option_expand() returns non-NULL the variable is expanded.  This can
 * only happen for non-indirect options.
 * Also set the default to the expanded value, so ":set" does not list
 * them.
 * Don't set the P_ALLOCED flag, because we don't want to free the
 * default.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_init_expand_env(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;
    char_u	*p;

    ಗಾಗಿ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	ಆದರೆ ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != ಖಾಲಿ)
	    p = (char_u *)_(*(ಅಕ್ಷರ **)options[opt_idx].var);
	ಇಲ್ಲದಿದ್ದರೆ
	    p = option_expand(opt_idx, ಖಾಲಿ);
	ಆದರೆ (p != ಖಾಲಿ && (p = vim_strsave(p)) != ಖಾಲಿ)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    ಆದರೆ (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#ಆದರೆ defined(MSWIN) && defined(FEAT_GETTEXT)
/*
 * Get the display language of Windows and the languages to fall back on, as a
 * colon separated list for gettext, e.g. "ja_JP:en_US".  The list stops after
 * English, untranslated messages are English already.
 * Returns NULL when it cannot be obtained.  The result must be freed.
 */
    ಸ್ಥಿರ char_u *
get_ui_langs(ಶೂನ್ಯ)
{
    ULONG	ಅಂಕೆ_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*ಬಫರ್;
    char_u	*langs = ಖಾಲಿ;

    ಆದರೆ (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &ಅಂಕೆ_languages, ಖಾಲಿ,
								    &bufsize)
	    || bufsize == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಬಫರ್ = ALLOC_MULT(WCHAR, bufsize);
    ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &ಅಂಕೆ_languages, ಬಫರ್,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	ಆದರೆ (langs != ಖಾಲಿ)
	{
	    char_u	*d = langs;
	    WCHAR	*s = ಬಫರ್;

	    ಆಗ (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		ಆದರೆ (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		ಗಾಗಿ ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		ಆದರೆ (english)
		    ನಿಲ್ಲಿಸು;
	    }
	    *d = NUL;
	}
    }
    vim_free(ಬಫರ್);

    ಹಿಂತಿರುಗಿ langs;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_init_lang_env(ಶೂನ್ಯ)
{
#ಆದರೆ defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    ಆದರೆ (mch_getenv((char_u *)"LANG") == ಖಾಲಿ
	    && mch_getenv((char_u *)"LANGUAGE") == ಖಾಲಿ
	    && mch_getenv((char_u *)"LC_ALL") == ಖಾಲಿ
	    && mch_getenv((char_u *)"LC_MESSAGES") == ಖಾಲಿ)
    {
	char_u	*langs = get_ui_langs();

	ಆದರೆ (langs != ಖಾಲಿ && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    ಆದರೆ (colon != ಖಾಲಿ)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#ಅಥವಾಆದರೆ defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_init_default_encoding(ಶೂನ್ಯ)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		opt_idx;

#ಆದರೆ defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#ಇಲ್ಲದಿದ್ದರೆ
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    ಆದರೆ (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    ಆದರೆ (mb_init() == ಖಾಲಿ)
    {
	opt_idx = findoption((char_u *)"encoding");
	ಆದರೆ (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#ಆದರೆ defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	ಆದರೆ (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    ಗಣ_string_option_ನೇರ((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    ಗಣ_string_option_ನೇರ((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    ಆದರೆ (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    ಆದರೆ (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (ಶೂನ್ಯ)init_chartab();
	}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	ಆದರೆ (
# ifdef VIMDLL
		(!gui.ಒಳ_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    ಅಕ್ಷರ	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    ಆದರೆ (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, ಗಾತ್ರ(buf), "cp%ld", (ದೀರ್ಘ)GetACP());
	    ಇಲ್ಲದಿದ್ದರೆ
		buflen = vim_snprintf(buf, ಗಾತ್ರ(buf), "cp%ld", (ದೀರ್ಘ)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    ಆದರೆ (p_tenc != ಖಾಲಿ)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		ಆದರೆ (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_ಸಿದ್ಧತೆ(&ಒಳಹರಿವು_conv, p_tenc, p_enc);
		convert_ಸಿದ್ಧತೆ(&output_conv, p_enc, p_tenc);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		p_tenc = ಖಾಲಿಗಳು_option;
	}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಶೂನ್ಯ
ಗಣ_init_1(ಪೂರ್ಣಾಂಕ clean_arg)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
    langmap_init();
#ಆದರೆ_ಅಂತ್ಯ

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    ಆದರೆ (mch_getenv((char_u *)"VIM_POSIX") != ಖಾಲಿ)
    {
	ಗಣ_string_default("cpo", (char_u *)CPO_ALL);
	ಗಣ_string_default("shm", (char_u *)SHM_POSIX);
    }

    ಗಣ_init_default_shell();
    ಗಣ_init_default_backupskip();
    ಗಣ_init_default_maxmemtot();
    ಗಣ_init_default_cdpath();
    ಗಣ_init_default_printencoding();
#ಆದರೆ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    ಗಣ_init_default_printexpr();
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Set all the options (except the terminal options) to their default
     * value.  Also set the global value for local options.
     */
    ಗಣ_options_default(0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಗಣ_init_xdg_rtp();
    ಗಣ_init_restricted_ಬಹುಲಕ();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CLEAN_RUNTIMEPATH
    ಆದರೆ (clean_arg)
	ಗಣ_init_clean_rtp();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (ಸಿಕ್ಕಿತು_ಹಿಮ್ಮುಖ_arg)
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
#ಆದರೆ_ಅಂತ್ಯ

    curbuf->b_p_initialized = true;
    curbuf->b_p_ac = -1;
    curbuf->b_p_ar = -1;	// no local 'autoread' value
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
    curbuf->b_p_fs = -1;	// no local 'fsync' value
#ಆದರೆ_ಅಂತ್ಯ
    curbuf->b_p_ul = NO_LOCAL_UNDOLEVEL;
    check_buf_options(curbuf);
    check_win_options(curwin);
    check_options();

    // Must be before option_expand(), because that one needs vim_isIDc()
    didset_options();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    // Use the current chartab for the generic chartab. This is not in
    // didset_options() because it only depends on 'encoding'.
    init_spell_chartab();
#ಆದರೆ_ಅಂತ್ಯ

    ಗಣ_init_default_encoding();

    // Expand environment variables and things like "~" for the defaults.
    ಗಣ_init_expand_env();

    save_file_ff(curbuf);	// Buffer is unchanged

#ಆದರೆ defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    ಆದರೆ (mch_getenv((char_u *)"MLTERM") != ಖಾಲಿ)
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"tbidi", 1L, ಖಾಲಿ, 0);
#ಆದರೆ_ಅಂತ್ಯ

    didset_options2();

    ಗಣ_init_lang_env();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    ಗಣ_helplang_default(get_mess_lang());
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    ಶೂನ್ಯ
ಗಣ_fencs_unicode(ಶೂನ್ಯ)
{
    ಗಣ_string_option_ನೇರ((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_option_default(
    ಪೂರ್ಣಾಂಕ		opt_idx,
    ಪೂರ್ಣಾಂಕ		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    ಪೂರ್ಣಾಂಕ		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    ಪೂರ್ಣಾಂಕ		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    ಪೂರ್ಣಾಂಕ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    ಆದರೆ (varp != ಖಾಲಿ)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	ಆದರೆ (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    ಆದರೆ (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		ಗಣ_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (options[opt_idx].indir != PV_NONE)
		ಗಣ_string_option_ನೇರ(ಖಾಲಿ, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & P_ಅಂಕೆ)
	{
	    ಆದರೆ (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ದೀರ್ಘ def_val = (ದೀರ್ಘ)(long_i)options[opt_idx].def_val[dvi];

		ಆದರೆ ((ದೀರ್ಘ *)varp == &curwin->w_p_so
			|| (ದೀರ್ಘ *)varp == &curwin->w_p_siso
			|| (ದೀರ್ಘ *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(ದೀರ್ಘ *)varp = -1;
		ಇಲ್ಲದಿದ್ದರೆ
		    *(ದೀರ್ಘ *)varp = def_val;
		// May also set global value for local option.
		ಆದರೆ (both)
		    *(ದೀರ್ಘ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(ಪೂರ್ಣಾಂಕ *)varp = (ಪೂರ್ಣಾಂಕ)(ದೀರ್ಘ)(long_i)options[opt_idx].def_val[dvi];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	    // 'modeline' defaults to off for root
	    ಆದರೆ (options[opt_idx].indir == PV_ML && getuid() == ಬೇರು_UID)
		*(ಪೂರ್ಣಾಂಕ *)varp = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
	    // May also set global value for local option.
	    ಆದರೆ (both)
		*(ಪೂರ್ಣಾಂಕ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(ಪೂರ್ಣಾಂಕ *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	ಆದರೆ (both)
	{
	    flagsp = insecure_flag(curwin, opt_idx, OPT_LOCAL);
	    *flagsp = *flagsp & ~P_INSECURE;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಗಣ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Set all options (except terminal options) to their default value.
 * When "opt_flags" is non-zero skip 'encoding'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_options_default(
    ಪೂರ್ಣಾಂಕ		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    ಪೂರ್ಣಾಂಕ		i;
    win_T	*wp;
    tabpage_T	*tp;

    ಗಾಗಿ (i = 0; !istermoption_idx(i); i++)
	ಆದರೆ (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#ಆದರೆ defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_ಕೀಲಿ
#ಆದರೆ_ಅಂತ್ಯ
			)))
	    ಗಣ_option_default(i, opt_flags, p_cp);

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
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_string_default_esc(ಅಕ್ಷರ *name, char_u *val, ಪೂರ್ಣಾಂಕ escape)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		opt_idx;

    ಆದರೆ (escape && vim_strchr(val, ' ') != ಖಾಲಿ)
	p = vim_strsave_escaped(val, (char_u *)" ");
    ಇಲ್ಲದಿದ್ದರೆ
	p = vim_strsave(val);
    ಆದರೆ (p == ಖಾಲಿ)		// we don't want a NULL
	ಹಿಂತಿರುಗಿ;

    opt_idx = findoption((char_u *)name);
    ಆದರೆ (opt_idx < 0)
    {
	vim_free(p);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    ಶೂನ್ಯ
ಗಣ_string_default(ಅಕ್ಷರ *name, char_u *val)
{
    ಗಣ_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    ಸ್ಥಿರ char_u *
ಕಂಡುಹಿಡಿ_dup_ಅಂಶ(char_u *origval, char_u *newval, size_t newvallen, long_u flags)
{
    ಪೂರ್ಣಾಂಕ	    bs = 0;
    char_u  *s;

    ಆದರೆ (origval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (s = origval; *s != NUL; ++s)
    {
	ಆದರೆ ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    ಹಿಂತಿರುಗಿ s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	ಆದರೆ ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	ಇಲ್ಲದಿದ್ದರೆ
	    bs = 0;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    ಶೂನ್ಯ
ಗಣ_ಸಂಖ್ಯೆ_default(ಅಕ್ಷರ *name, ದೀರ್ಘ val)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;

    opt_idx = findoption((char_u *)name);
    ಆದರೆ (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#ಆದರೆ defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    ಶೂನ್ಯ
ಗಣ_local_options_default(win_T *wp, ಪೂರ್ಣಾಂಕ do_ಬಫರ್)
{
    win_T	*save_curwin = curwin;
    ಪೂರ್ಣಾಂಕ		i;

    curwin = wp;
    curbuf = curwin->w_ಬಫರ್;
    ಖಂಡ_autocmds();

    ಗಾಗಿ (i = 0; !istermoption_idx(i); i++)
    {
	ರಚನೆ vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	ಆದರೆ (p->indir != PV_NONE
		&& (do_ಬಫರ್ || (p->indir & PV_BUF) == 0)
		&& !(options[i].flags & P_NODEFAULT)
		&& !optval_default(p, varp, FALSE))
	    ಗಣ_option_default(i, OPT_FREE|OPT_LOCAL, FALSE);
    }

    unblock_autocmds();
    curwin = save_curwin;
    curbuf = curwin->w_ಬಫರ್;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(EXITFREE)
/*
 * Free all options.
 */
    ಶೂನ್ಯ
free_all_options(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; !istermoption_idx(i); i++)
    {
	ಆದರೆ (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    ಆದರೆ ((options[i].flags & P_ALLOCED) && options[i].var != ಖಾಲಿ)
		free_string_option(*(char_u **)options[i].var);
	    ಆದರೆ (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# ಆದರೆ defined(FEAT_EVAL)
    free_findfunc_option();
# endif
}
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    ಶೂನ್ಯ
ಗಣ_init_2(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    ಆದರೆ (idx >= 0 && !(options[idx].flags & P_WAS_ಗಣ))
	ಗಣ_option_default(idx, OPT_LOCAL, p_cp);
    comp_ಲಂಬಸಾಲು();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    ಆದರೆ (!option_was_ಗಣ((char_u *)"window"))
	p_ಕಿಟಕಿ = ಅಡ್ಡಸಾಲುಗಳು - 1;
    ಗಣ_ಸಂಖ್ಯೆ_default("window", ಅಡ್ಡಸಾಲುಗಳು - 1);

    // For DOS console the default is always black.
#ಆದರೆ !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    ಆದರೆ (idx >= 0 && !(options[idx].flags & P_WAS_ಗಣ)
						 && *ಪದ_bg_default() == 'd')
    {
	ಗಣ_string_option_ನೇರ(ಖಾಲಿ, idx, (char_u *)"dark", OPT_FREE, 0);
	// don't mark it as set, when starting the GUI it may be
	// changed again
	options[idx].flags &= ~P_WAS_ಗಣ;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
    parse_shape_opt(SHAPE_CURSOR);	// set cursor shapes from 'guicursor'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
    parse_shape_opt(SHAPE_MOUSE);	// set mouse shapes from 'mouseshape'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINTER
    (ಶೂನ್ಯ)parse_printoptions(ಖಾಲಿ);	// parse 'printoptions' default value
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    ಶೂನ್ಯ
ಗಣ_init_3(ಶೂನ್ಯ)
{
#ಆದರೆ defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    idx_srr;
    ಪೂರ್ಣಾಂಕ	    do_srr;
# ifdef FEAT_QUICKFIX
    ಪೂರ್ಣಾಂಕ	    idx_sp;
    ಪೂರ್ಣಾಂಕ	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    ಆದರೆ (idx_srr < 0)
	do_srr = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
	do_srr = !(options[idx_srr].flags & P_WAS_ಗಣ);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    ಆದರೆ (idx_sp < 0)
	do_sp = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
	do_sp = !(options[idx_sp].flags & P_WAS_ಗಣ);
# endif
    p = get_isolated_shell_name();
    ಆದರೆ (p != ಖಾಲಿ)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	ಆದರೆ (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# ಆದರೆ defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# ಆದರೆ defined(FEAT_QUICKFIX)
	    ಆದರೆ (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  ಇಲ್ಲದಿದ್ದರೆ
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    ಆದರೆ (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  ಆದರೆ defined(FEAT_QUICKFIX)
		ಆದರೆ (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		ಆದರೆ (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	ಇಲ್ಲದಿದ್ದರೆ
	    // Always use POSIX shell style redirection if we reach this
	    ಆದರೆ (       fnamecmp(p, "sh") == 0
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
# ಆದರೆ defined(FEAT_QUICKFIX)
		ಆದರೆ (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  ಇಲ್ಲದಿದ್ದರೆ
		    ಆದರೆ (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    ಇಲ್ಲದಿದ್ದರೆ
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		ಆದರೆ (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MSWIN)
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
    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)gettail(p_sh), "powershell") != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	ಆದರೆ (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_ಗಣ))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	ಆದರೆ (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_ಗಣ))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)gettail(p_sh), "sh") != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	idx3;

	idx3 = findoption((char_u *)"shcf");
	ಆದರೆ (idx3 >= 0 && !(options[idx3].flags & P_WAS_ಗಣ))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	ಆದರೆ (idx3 >= 0 && !(options[idx3].flags & P_WAS_ಗಣ))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)gettail(p_sh), "cmd.exe") != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	idx3;

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
	ಆದರೆ (idx3 >= 0 && !(options[idx3].flags & P_WAS_ಗಣ))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	ಆದರೆ (idx3 >= 0 && !(options[idx3].flags & P_WAS_ಗಣ))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (BUFEMPTY())
    {
	ಪೂರ್ಣಾಂಕ idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	ಆದರೆ (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_ಗಣ))
	    ಗಣ_fileformat(default_fileformat(), OPT_LOCAL);
    }

    ಗಣ_title_defaults();
}

#ಆದರೆ defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    ಶೂನ್ಯ
ಗಣ_helplang_default(char_u *lang)
{
    ಪೂರ್ಣಾಂಕ		idx;
    size_t	langlen;

    ಆದರೆ (lang == ಖಾಲಿ)	// safety check
	ಹಿಂತಿರುಗಿ;

    langlen = STRLEN(lang);
    ಆದರೆ (langlen < 2)	// safety check
	ಹಿಂತಿರುಗಿ;

    idx = findoption((char_u *)"hlg");
    ಆದರೆ (idx < 0 || (options[idx].flags & P_WAS_ಗಣ))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    ಆದರೆ (p_hlg == ಖಾಲಿ)
	p_hlg = ಖಾಲಿಗಳು_option;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	ಆದರೆ (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (langlen >= 1 && *p_hlg == 'C')
	{
	    p_hlg[0] = 'e';
	    p_hlg[1] = 'n';
	}
	p_hlg[2] = NUL;
    }
    options[idx].flags |= P_ALLOCED;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * 'title' and 'icon' only default to true if they have not been set or reset
 * in .vimrc and we can read the old value.
 * When 'title' and 'icon' have been reset in .vimrc, we won't even check if
 * they can be reset.  This reduces startup time when using X on a remote
 * machine.
 */
    ಶೂನ್ಯ
ಗಣ_title_defaults(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    idx1;
    ದೀರ್ಘ    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    ಆದರೆ (idx1 >= 0 && !(options[idx1].flags & P_WAS_ಗಣ))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (gui.starting || gui.ಒಳ_use)
	    val = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    ಆದರೆ (idx1 < 0 || (options[idx1].flags & P_WAS_ಗಣ))
    {
	ಹಿಂತಿರುಗಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.starting || gui.ಒಳ_use)
	val = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    ಶೂನ್ಯ
ex_ಗಣ(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		flags = 0;

    ಆದರೆ (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#ಆದರೆ defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    ಆದರೆ ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಆದರೆ (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(ಶೂನ್ಯ)do_ಗಣ(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    ಪೂರ್ವಪ್ರತ್ಯಯ_NO = 0,	// "no" prefix
    ಪೂರ್ವಪ್ರತ್ಯಯ_NONE,	// no prefix
    ಪೂರ್ವಪ್ರತ್ಯಯ_INV,		// "inv" prefix
} ಗಣ_ಪೂರ್ವಪ್ರತ್ಯಯ_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    ಸ್ಥಿರ ಗಣ_ಪೂರ್ವಪ್ರತ್ಯಯ_T
get_option_ಪೂರ್ವಪ್ರತ್ಯಯ(char_u **argp)
{
    ಪೂರ್ಣಾಂಕ		ಪೂರ್ವಪ್ರತ್ಯಯ = ಪೂರ್ವಪ್ರತ್ಯಯ_NONE;
    char_u	*arg = *argp;

    ಆದರೆ (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	ಪೂರ್ವಪ್ರತ್ಯಯ = ಪೂರ್ವಪ್ರತ್ಯಯ_NO;
	arg += 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "inv", 3) == 0)
    {
	ಪೂರ್ವಪ್ರತ್ಯಯ = ಪೂರ್ವಪ್ರತ್ಯಯ_INV;
	arg += 3;
    }

    *argp = arg;
    ಹಿಂತಿರುಗಿ ಪೂರ್ವಪ್ರತ್ಯಯ;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_option_name(char_u *arg, ಪೂರ್ಣಾಂಕ *opt_idxp, ಪೂರ್ಣಾಂಕ *lenp, ಪೂರ್ಣಾಂಕ *keyp)
{
    ಪೂರ್ಣಾಂಕ	ಕೀಲಿ = 0;
    ಪೂರ್ಣಾಂಕ	len;
    ಪೂರ್ಣಾಂಕ	opt_idx;

    ಆದರೆ (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	ಆದರೆ (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    len = 1;
	    ಆಗ (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	ಆದರೆ (arg[len] != '>')
	    ಹಿಂತಿರುಗಿ FAIL;

	arg[len] = NUL;			    // put NUL after name
	ಆದರೆ (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	ಆದರೆ (opt_idx == -1)
	    ಕೀಲಿ = ಕಂಡುಹಿಡಿ_ಕೀಲಿ_option(arg + 1, TRUE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	ಆದರೆ (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಆಗ (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	ಆದರೆ (opt_idx == -1)
	    ಕೀಲಿ = ಕಂಡುಹಿಡಿ_ಕೀಲಿ_option(arg, FALSE);
    }

    *keyp = ಕೀಲಿ;
    *lenp = len;
    *opt_idxp = opt_idx;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    ಸ್ಥಿರ ಗಣ_op_T
get_opt_op(char_u *arg)
{
    ಗಣ_op_T op = OP_NONE;

    ಆದರೆ (*arg != NUL && *(arg + 1) == '=')
    {
	ಆದರೆ (*arg == '+')
	    op = OP_ADDING;		// "+="
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    ಹಿಂತಿರುಗಿ op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
ಸ್ಥಿರ ಅಕ್ಷರ *modeline_whitelist[] =
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
    ಖಾಲಿ
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    ಸ್ಥಿರ bool
is_modeline_whitelisted(ಅಕ್ಷರ *name)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; modeline_whitelist[i] != ಖಾಲಿ; i++)
	ಆದರೆ (STRCMP(name, modeline_whitelist[i]) == 0)
	    ಹಿಂತಿರುಗಿ true;
    ಹಿಂತಿರುಗಿ false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
validate_opt_idx(
	ಪೂರ್ಣಾಂಕ opt_idx,
	ಪೂರ್ಣಾಂಕ opt_flags,
	long_u flags,
	ಅಕ್ಷರ **errmsg,
	ಗಣ_ಪೂರ್ವಪ್ರತ್ಯಯ_T ಪೂರ್ವಪ್ರತ್ಯಯ)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    ಆದರೆ ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	ಹಿಂತಿರುಗಿ FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    ಆದರೆ ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	ಹಿಂತಿರುಗಿ FAIL;

    // Disallow changing some options from modelines.
    ಆದರೆ (opt_flags & OPT_MODELINE)
    {
	ಆದರೆ (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_ಒಳ_modeline;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_ಒಳ_modeline_when_modelineexpr_is_off;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	ಆದರೆ (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    ಆದರೆ (options[opt_idx].indir == PV_ML && ಪೂರ್ವಪ್ರತ್ಯಯ == ಪೂರ್ವಪ್ರತ್ಯಯ_NO)
		ಹಿಂತಿರುಗಿ OK;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!is_modeline_whitelisted(options[opt_idx].fullname))
		ಹಿಂತಿರುಗಿ FAIL;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	ಆದರೆ (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    ಆದರೆ (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_ಒಳ_sandbox;
	ಹಿಂತಿರುಗಿ FAIL;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    ಸ್ಥಿರ char_u *
stropt_get_default_val(
    ಪೂರ್ಣಾಂಕ		opt_idx,
    char_u	*varp,
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    ಆದರೆ ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (gui.ಒಳ_use)
	    newval = gui_bg_default();
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    newval = ಪದ_bg_default();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    ಆದರೆ (newval == ಖಾಲಿ)
	newval = ಖಾಲಿಗಳು_option;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *s = option_expand(opt_idx, newval);
	ಆದರೆ (s == ಖಾಲಿ)
	    s = newval;
	newval = vim_strsave(s);
    }

    ಹಿಂತಿರುಗಿ newval;
}

/*
 * Convert the 'backspace' option number value to a string: for adding,
 * prepending and removing string.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
opt_backspace_nr2str(
    char_u	*varp,
    char_u	**origval_p,
    char_u	**origval_l_p,
    char_u	**origval_g_p,
    char_u	**oldval_p)
{
    ಪೂರ್ಣಾಂಕ		i = getdigits((char_u **)varp);

    ಆಯ್ಕೆ (i)
    {
	ಸ್ಥಿತಿ 0:
	    *(char_u **)varp = ಖಾಲಿಗಳು_option;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    ನಿಲ್ಲಿಸು;
    }
    vim_free(*oldval_p);
    ಆದರೆ (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    ಆದರೆ (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    ಆದರೆ (*origval_g_p == *oldval_p)
	*origval_g_p = *(char_u **)varp;
    *oldval_p = *(char_u **)varp;
}

/*
 * Convert the 'whichwrap' option number value to a string, for backwards
 * compatibility with Vim 3.0.
 * Note: 'argp' is a pointer to a char_u pointer and is updated.
 */
    ಸ್ಥಿರ char_u *
opt_whichwrap_nr2str(char_u **argp, char_u *whichwrap)
{
    size_t  len = 0;

    *whichwrap = NUL;
    ಪೂರ್ಣಾಂಕ i = getdigits(argp);
    ಆದರೆ (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    ಆದರೆ (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    ಆದರೆ (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    ಆದರೆ (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    ಆದರೆ (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    ಆದರೆ (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    ಹಿಂತಿರುಗಿ whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    ಸ್ಥಿರ char_u *
stropt_copy_ಮೌಲ್ಯ(
    char_u	*origval,
    char_u	**argp,
    ಗಣ_op_T	op,
    ಪೂರ್ಣಾಂಕ		flags ಬಳಸದ)
{
    char_u	*arg = *argp;
    ಚಿಹ್ನೆರಹಿತ	newlen;
    char_u	*newval;
    char_u	*s = ಖಾಲಿ;

    // get a bit too much
    newlen = (ಚಿಹ್ನೆರಹಿತ)STRLEN(arg) + 1;
    ಆದರೆ (op != OP_NONE)
	newlen += (ಚಿಹ್ನೆರಹಿತ)STRLEN(origval) + 1;
    newval = alloc(newlen);
    ಆದರೆ (newval == ಖಾಲಿ)  // out of mem, don't change
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    ಆಗ (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	ಪೂರ್ಣಾಂಕ i;

	ಆದರೆ (*arg == '\\' && arg[1] != NUL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#ಆದರೆ_ಅಂತ್ಯ
	   )
	    ++arg;	// remove backslash
	ಆದರೆ (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    ಹಿಂತಿರುಗಿ newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    ಸ್ಥಿರ char_u *
stropt_expand_envvar(
    ಪೂರ್ಣಾಂಕ		opt_idx,
    char_u	*origval,
    char_u	*newval,
    ಗಣ_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    ಆದರೆ (s == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ newval;

    vim_free(newval);
    ಚಿಹ್ನೆರಹಿತ newlen = (ಚಿಹ್ನೆರಹಿತ)STRLEN(s) + 1;
    ಆದರೆ (op != OP_NONE)
	newlen += (ಚಿಹ್ನೆರಹಿತ)STRLEN(origval) + 1;

    newval = alloc(newlen);
    ಆದರೆ (newval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    STRCPY(newval, s);

    ಹಿಂತಿರುಗಿ newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    ಗಣ_op_T	op,
    ಪೂರ್ಣಾಂಕ		flags)
{
    ಪೂರ್ಣಾಂಕ		len = 0;

    ಪೂರ್ಣಾಂಕ comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    ಆದರೆ (op == OP_ADDING)
    {
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(origval);
	// strip a trailing comma, would get 2
	ಆದರೆ (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    ಆದರೆ (comma)
	newval[len] = ',';
}

/*
 * Remove a value from a string option.  Copy string option value in "origval"
 * to "newval" and then remove the string "strval" of length "len".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
stropt_remove_val(
    char_u	*origval,
    char_u	*newval,
    ಪೂರ್ಣಾಂಕ		flags,
    char_u	*strval,
    ಪೂರ್ಣಾಂಕ		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    ಆದರೆ (*strval)
    {
	// may need to remove a comma
	ಆದರೆ (flags & P_COMMA)
	{
	    ಆದರೆ (strval == origval)
	    {
		// include comma after string
		ಆದರೆ (strval[len] == ',')
		    ++len;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
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
    ಸ್ಥಿರ char_u *
ಕಂಡುಹಿಡಿ_ಕೀಲಿ_ಅಂಶ(char_u *src, char_u *ಕೀಲಿ, ಪೂರ್ಣಾಂಕ keylen, ಪೂರ್ಣಾಂಕ *itemlenp)
{
    char_u  *p = src;

    ಆಗ (*p != NUL)
    {
	// Check if this item starts with the same key
	ಆದರೆ ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, ಕೀಲಿ, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *ಕೊನೆ = vim_strchr(p, ',');
	    ಆದರೆ (ಕೊನೆ == ಖಾಲಿ)
		ಕೊನೆ = p + STRLEN(p);
	    *itemlenp = (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - p);
	    ಹಿಂತಿರುಗಿ p;
	}
	++p;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_comma_ಅಂಶ(char_u *str, char_u *ಅಂಶ, ಪೂರ್ಣಾಂಕ itemlen)
{
    ಆದರೆ (ಅಂಶ[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(ಅಂಶ, ಅಂಶ + itemlen + 1);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಶ > str && *(ಅಂಶ - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(ಅಂಶ - 1, ಅಂಶ + itemlen);
    ಇಲ್ಲದಿದ್ದರೆ
	// Only item
	*ಅಂಶ = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_ಕೀಲಿ_ಅಂಶ(char_u *str, char_u *ಕೀಲಿ, ಪೂರ್ಣಾಂಕ keylen, char_u *skip)
{
    ಪೂರ್ಣಾಂಕ	    itemlen;
    char_u  *ಸಿಕ್ಕಿತು;

    ಆಗ ((ಸಿಕ್ಕಿತು = ಕಂಡುಹಿಡಿ_ಕೀಲಿ_ಅಂಶ(str, ಕೀಲಿ, keylen, &itemlen)) != ಖಾಲಿ)
    {
	ಆದರೆ (ಸಿಕ್ಕಿತು == skip)
	{
	    // Search for the next match after this one.
	    char_u *ಮುಂದಿನ = ಸಿಕ್ಕಿತು + itemlen;
	    ಆದರೆ (*ಮುಂದಿನ == ',')
		++ಮುಂದಿನ;
	    ಸಿಕ್ಕಿತು = ಕಂಡುಹಿಡಿ_ಕೀಲಿ_ಅಂಶ(ಮುಂದಿನ, ಕೀಲಿ, keylen, &itemlen);
	    ಆದರೆ (ಸಿಕ್ಕಿತು == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	}

	remove_comma_ಅಂಶ(str, ಸಿಕ್ಕಿತು, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
append_ಅಂಶ(char_u *str, char_u *ಅಂಶ, ಪೂರ್ಣಾಂಕ ಅಂಶ_len)
{
    ಪೂರ್ಣಾಂಕ len = (ಪೂರ್ಣಾಂಕ)STRLEN(str);

    ಆದರೆ (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, ಅಂಶ, (size_t)ಅಂಶ_len);
    str[len + ಅಂಶ_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prepend_ಅಂಶ(char_u *str, char_u *ಅಂಶ, ಪೂರ್ಣಾಂಕ ಅಂಶ_len)
{
    ಪೂರ್ಣಾಂಕ len = (ಪೂರ್ಣಾಂಕ)STRLEN(str);
    ಪೂರ್ಣಾಂಕ comma = (len > 0) ? 1 : 0;

    mch_memmove(str + ಅಂಶ_len + comma, str, (size_t)len + 1);
    mch_memmove(str, ಅಂಶ, (size_t)ಅಂಶ_len);
    ಆದರೆ (comma)
	str[ಅಂಶ_len] = ',';
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
    ಸ್ಥಿರ bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    ಗಣ_op_T	op,
    ಪೂರ್ಣಾಂಕ		flags ಬಳಸದ)
{
    char_u  *p;
    char_u  *ಅಂಶ_ಆರಂಭ;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    ಆದರೆ (vim_strchr(newval, ':') == ಖಾಲಿ && vim_strchr(newval, ',') == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    ಆದರೆ (newval_copy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    ಅಂಶ_ಆರಂಭ = newval_copy;
    ಗಾಗಿ (;;)
    {
	p = vim_strchr(ಅಂಶ_ಆರಂಭ, ',');
	ಪೂರ್ಣಾಂಕ ಅಂಶ_len = (p == ಖಾಲಿ)
			    ? (ಪೂರ್ಣಾಂಕ)STRLEN(ಅಂಶ_ಆರಂಭ) : (ಪೂರ್ಣಾಂಕ)(p - ಅಂಶ_ಆರಂಭ);

	ಆದರೆ (ಅಂಶ_len > 0)
	{
	    char_u *colon = vim_strchr(ಅಂಶ_ಆರಂಭ, ':');
	    ಆದರೆ (colon != ಖಾಲಿ && colon < ಅಂಶ_ಆರಂಭ + ಅಂಶ_len)
	    {
		ಪೂರ್ಣಾಂಕ keylen = (ಪೂರ್ಣಾಂಕ)(colon - ಅಂಶ_ಆರಂಭ) + 1;

		ಆದರೆ (op == OP_ADDING || op == OP_PREPENDING)
		{
		    ಪೂರ್ಣಾಂಕ old_itemlen;
		    char_u *ಸಿಕ್ಕಿತು = ಕಂಡುಹಿಡಿ_ಕೀಲಿ_ಅಂಶ(newval, ಅಂಶ_ಆರಂಭ,
						       keylen, &old_itemlen);
		    ಆದರೆ (ಸಿಕ್ಕಿತು != ಖಾಲಿ)
		    {
			ಆದರೆ (old_itemlen == ಅಂಶ_len
				&& STRNCMP(ಸಿಕ್ಕಿತು, ಅಂಶ_ಆರಂಭ,
							 ಅಂಶ_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_ಕೀಲಿ_ಅಂಶ(newval, ಅಂಶ_ಆರಂಭ,
							   keylen, ಸಿಕ್ಕಿತು);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_ಕೀಲಿ_ಅಂಶ(newval, ಅಂಶ_ಆರಂಭ,
							    keylen, ಖಾಲಿ);
			    ಆದರೆ (op == OP_PREPENDING)
				prepend_ಅಂಶ(newval, ಅಂಶ_ಆರಂಭ, ಅಂಶ_len);
			    ಇಲ್ಲದಿದ್ದರೆ
				append_ಅಂಶ(newval, ಅಂಶ_ಆರಂಭ, ಅಂಶ_len);
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// New item.
			ಆದರೆ (op == OP_PREPENDING)
			    prepend_ಅಂಶ(newval, ಅಂಶ_ಆರಂಭ, ಅಂಶ_len);
			ಇಲ್ಲದಿದ್ದರೆ
			    append_ಅಂಶ(newval, ಅಂಶ_ಆರಂಭ, ಅಂಶ_len);
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == OP_REMOVING)
		    remove_ಕೀಲಿ_ಅಂಶ(newval, ಅಂಶ_ಆರಂಭ, keylen, ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *ಸಿಕ್ಕಿತು = ಕಂಡುಹಿಡಿ_dup_ಅಂಶ(newval, ಅಂಶ_ಆರಂಭ,
							   ಅಂಶ_len, P_COMMA);
		    ಆದರೆ (ಸಿಕ್ಕಿತು == ಖಾಲಿ)
		    {
			// New item.
			ಆದರೆ (op == OP_PREPENDING)
			    prepend_ಅಂಶ(newval, ಅಂಶ_ಆರಂಭ, ಅಂಶ_len);
			ಇಲ್ಲದಿದ್ದರೆ
			    append_ಅಂಶ(newval, ಅಂಶ_ಆರಂಭ, ಅಂಶ_len);
		    }
		    // else: exact duplicate — do nothing
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == OP_REMOVING)
		{
		    char_u *ಸಿಕ್ಕಿತು = ಕಂಡುಹಿಡಿ_dup_ಅಂಶ(newval, ಅಂಶ_ಆರಂಭ,
							   ಅಂಶ_len, P_COMMA);
		    ಆದರೆ (ಸಿಕ್ಕಿತು != ಖಾಲಿ)
			remove_comma_ಅಂಶ(newval, ಸಿಕ್ಕಿತು, ಅಂಶ_len);
		}
	    }
	}

	ಆದರೆ (p == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಅಂಶ_ಆರಂಭ = p + 1;
    }

    vim_free(newval_copy);

    ಹಿಂತಿರುಗಿ true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
stropt_remove_dupflags(char_u *newval, ಪೂರ್ಣಾಂಕ flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    ಆಗ (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	ಆದರೆ (flags & P_ONECOMMA)
	{
	    ಆದರೆ (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != ಖಾಲಿ)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		ಮುಂದುವರಿ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != ಖಾಲಿ)
	    {
		STRMOVE(s, s + 1);
		ಮುಂದುವರಿ;
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
    ಸ್ಥಿರ char_u *
stropt_get_newval(
    ಪೂರ್ಣಾಂಕ		nextchar,
    ಪೂರ್ಣಾಂಕ		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    ಗಣ_op_T    *op_arg,
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    ಗಣ_op_T    op = *op_arg;
    char_u	*save_arg = ಖಾಲಿ;
    char_u	*newval;
    char_u	*s = ಖಾಲಿ;
    char_u	whichwrap[80];

    ಆದರೆ (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	ಆದರೆ (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_ಮೌಲ್ಯ(origval, &arg, op, flags);
	ಆದರೆ (newval == ಖಾಲಿ)
	    ಹೋಗು ಮುಗಿದ;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	ಆದರೆ (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    ಆದರೆ (newval == ಖಾಲಿ)
		ಹೋಗು ಮುಗಿದ;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	ಆದರೆ ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    ಪೂರ್ಣಾಂಕ len = 0;
	    ಆದರೆ (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (ಪೂರ್ಣಾಂಕ)STRLEN(newval);
		s = ಕಂಡುಹಿಡಿ_dup_ಅಂಶ(origval, newval, len, flags);

		// do not add if already there
		ಆದರೆ ((op == OP_ADDING || op == OP_PREPENDING) && s != ಖಾಲಿ)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		ಆದರೆ (s == ಖಾಲಿ)
		    s = origval + (ಪೂರ್ಣಾಂಕ)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    ಆದರೆ (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	ಆದರೆ (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

ಮುಗಿದ:
    ಆದರೆ (save_arg != ಖಾಲಿ)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    ಹಿಂತಿರುಗಿ newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_ಗಣ_option_string(
	ಪೂರ್ಣಾಂಕ	    opt_idx,
	ಪೂರ್ಣಾಂಕ	    opt_flags,
	char_u	    **argp,
	ಪೂರ್ಣಾಂಕ	    nextchar,
	ಗಣ_op_T    op_arg,
	long_u	    flags,
	ಪೂರ್ಣಾಂಕ	    cp_val,
	char_u	    *varp_arg,
	ಅಕ್ಷರ	    *errbuf,
	size_t	    errbuflen,
	ಪೂರ್ಣಾಂಕ	    *ಮೌಲ್ಯ_checked,
	ಅಕ್ಷರ	    **errmsg)
{
    char_u	*arg = *argp;
    ಗಣ_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = ಖಾಲಿ; // previous value if *varp
    char_u	*newval;
    char_u	*origval = ಖಾಲಿ;
    char_u	*origval_l = ಖಾಲಿ;
    char_u	*origval_g = ಖಾಲಿ;
#ಆದರೆ defined(FEAT_EVAL)
    char_u	*saved_origval = ಖಾಲಿ;
    char_u	*saved_origval_l = ಖಾಲಿ;
    char_u	*saved_origval_g = ಖಾಲಿ;
    char_u	*saved_newval = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    ಆದರೆ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((ಪೂರ್ಣಾಂಕ)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    ಆದರೆ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	ಆದರೆ (((ಪೂರ್ಣಾಂಕ)options[opt_idx].indir & PV_BOTH)
						  && origval_l == ಖಾಲಿಗಳು_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    ಆದರೆ (((ಪೂರ್ಣಾಂಕ)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    ಇಲ್ಲದಿದ್ದರೆ
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    ಆದರೆ (newval == ಖಾಲಿ)
	*(char_u **)(varp) = ಖಾಲಿಗಳು_option;

#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_ಕೀಲಿ
# endif
		      && origval != ಖಾಲಿ && newval != ಖಾಲಿ)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	ಆದರೆ (origval_l != ಖಾಲಿ)
	    saved_origval_l = vim_strsave(origval_l);
	ಆದರೆ (origval_g != ಖಾಲಿ)
	    saved_origval_g = vim_strsave(origval_g);
    }
#ಆದರೆ_ಅಂತ್ಯ

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	ಪೂರ್ಣಾಂಕ	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	ಆದರೆ ((opt_flags & OPT_MODELINE)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
	      || sandbox != 0
#ಆದರೆ_ಅಂತ್ಯ
	      || (op != OP_NONE && (*p & P_INSECURE)))
	    secure = 1;

	// Handle side effects, and set the global value for ":set" on local
	// options. Note: when setting 'syntax' or 'filetype' autocommands may
	// be triggered that can cause havoc.
	*errmsg = did_ಗಣ_string_option(
			opt_idx, (char_u **)varp, oldval, newval, errbuf,
			errbuflen, opt_flags, op, ಮೌಲ್ಯ_checked);

	secure = secure_saved;
    }

#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ (*errmsg == ಖಾಲಿ)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#ಆದರೆ_ಅಂತ್ಯ

    *argp = arg;
    ಹಿಂತಿರುಗಿ *errmsg == ಖಾಲಿ ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
do_ಗಣ_option_bool(
    ಪೂರ್ಣಾಂಕ		opt_idx,
    ಪೂರ್ಣಾಂಕ		opt_flags,
    ಗಣ_ಪೂರ್ವಪ್ರತ್ಯಯ_T ಪೂರ್ವಪ್ರತ್ಯಯ,
    long_u	flags,
    char_u	*varp,
    ಪೂರ್ಣಾಂಕ		nextchar,
    ಪೂರ್ಣಾಂಕ		afterchar,
    ಪೂರ್ಣಾಂಕ		cp_val)

{
    varnumber_T	ಮೌಲ್ಯ;

    ಆದರೆ (nextchar == '=' || nextchar == ':')
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    ಆದರೆ (opt_idx < 0 || varp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    ಆದರೆ (nextchar == '!')
	ಮೌಲ್ಯ = *(ಪೂರ್ಣಾಂಕ *)(varp) ^ 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nextchar == '&')
	ಮೌಲ್ಯ = (ಪೂರ್ಣಾಂಕ)(ದೀರ್ಘ)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	ಆದರೆ ((ಪೂರ್ಣಾಂಕ *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    ಮೌಲ್ಯ = -1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಪೂರ್ಣಾಂಕ *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    ಮೌಲ್ಯ = -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಪೂರ್ಣಾಂಕ *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    ಮೌಲ್ಯ = -1;
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ
	    ಮೌಲ್ಯ = *(ಪೂರ್ಣಾಂಕ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	ಆದರೆ (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    ಹಿಂತಿರುಗಿ e_trailing_characters;
	ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ == ಪೂರ್ವಪ್ರತ್ಯಯ_INV)
	    ಮೌಲ್ಯ = *(ಪೂರ್ಣಾಂಕ *)(varp) ^ 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಮೌಲ್ಯ = ಪೂರ್ವಪ್ರತ್ಯಯ == ಪೂರ್ವಪ್ರತ್ಯಯ_NO ? 0 : 1;
    }

    ಹಿಂತಿರುಗಿ ಗಣ_bool_option(opt_idx, varp, (ಪೂರ್ಣಾಂಕ)ಮೌಲ್ಯ, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
do_ಗಣ_option_numeric(
    ಪೂರ್ಣಾಂಕ		opt_idx,
    ಪೂರ್ಣಾಂಕ		opt_flags,
    char_u	**argp,
    ಪೂರ್ಣಾಂಕ		nextchar,
    ಗಣ_op_T	op,
    long_u	flags,
    ಪೂರ್ಣಾಂಕ		cp_val,
    char_u	*varp,
    ಅಕ್ಷರ	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಂಕ			i;
    ಅಕ್ಷರ		*errmsg = ಖಾಲಿ;

    ಆದರೆ (opt_idx < 0 || varp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;  // "cannot happen"
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
    ಆದರೆ (nextchar == '&')
	ಮೌಲ್ಯ = (ದೀರ್ಘ)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nextchar == '<')
    {
	ಆದರೆ ((ದೀರ್ಘ *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    ಮೌಲ್ಯ = NO_LOCAL_UNDOLEVEL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt_flags == OPT_LOCAL
		    && ((ದೀರ್ಘ *)varp == &curwin->w_p_siso
		     || (ದೀರ್ಘ *)varp == &curwin->w_p_so
		     || (ದೀರ್ಘ *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    ಮೌಲ್ಯ = -1;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಮೌಲ್ಯ = *(ದೀರ್ಘ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (((ದೀರ್ಘ *)varp == &p_wc || (ದೀರ್ಘ *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	ಮೌಲ್ಯ = string_ಗಮ್ಯ_ಕೀಲಿ(arg, FALSE);
	ಆದರೆ (ಮೌಲ್ಯ == 0)
	{
	    errmsg = e_invalid_ವಾದ;
	    ಹೋಗು skip;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, ಖಾಲಿ, &i, STR2NR_ALL, &ಮೌಲ್ಯ, ಖಾಲಿ, 0, TRUE, ಖಾಲಿ);
	ಆದರೆ (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_ಸಂಖ್ಯೆ_required_after_equal;
	    ಹೋಗು skip;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	errmsg = e_ಸಂಖ್ಯೆ_required_after_equal;
	ಹೋಗು skip;
    }

    ಆದರೆ (op == OP_ADDING)
	ಮೌಲ್ಯ = *(ದೀರ್ಘ *)varp + ಮೌಲ್ಯ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == OP_PREPENDING)
	ಮೌಲ್ಯ = *(ದೀರ್ಘ *)varp * ಮೌಲ್ಯ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == OP_REMOVING)
	ಮೌಲ್ಯ = *(ದೀರ್ಘ *)varp - ಮೌಲ್ಯ;

    ಆದರೆ ((ದೀರ್ಘ *)varp == &curwin->w_p_sop && ಮೌಲ್ಯ < -1)
    {
	errmsg = e_invalid_ವಾದ;
	ಹೋಗು skip;
    }

    errmsg = ಗಣ_ಅಂಕೆ_option(opt_idx, varp, ಮೌಲ್ಯ, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
do_ಗಣ_option_keycode(char_u **argp, char_u *ಕೀಲಿ_name, ಪೂರ್ಣಾಂಕ nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    ಆದರೆ (nextchar == '&')
    {
	ಆದರೆ (ಕೂಡಿಸು_termcap_ನಮೂದು(ಕೀಲಿ_name, TRUE) == FAIL)
	    ಹಿಂತಿರುಗಿ e_not_ಸಿಕ್ಕಿತು_ಒಳ_termcap;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	++arg; // jump to after the '=' or ':'
	ಗಾಗಿ (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    ಆದರೆ (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	ಕೂಡಿಸು_termcode(ಕೀಲಿ_name, arg, FALSE);
	*p = nextchar;
    }
    ಆದರೆ (ತುಂಬಿದ_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Set an option to a new value.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
do_ಗಣ_option_ಮೌಲ್ಯ(
    ಪೂರ್ಣಾಂಕ		opt_idx,
    ಪೂರ್ಣಾಂಕ		opt_flags,
    char_u	**argp,
    ಗಣ_ಪೂರ್ವಪ್ರತ್ಯಯ_T ಪೂರ್ವಪ್ರತ್ಯಯ,
    ಗಣ_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*ಕೀಲಿ_name,
    ಪೂರ್ಣಾಂಕ		nextchar,
    ಪೂರ್ಣಾಂಕ		afterchar,
    ಪೂರ್ಣಾಂಕ		cp_val,
    ಪೂರ್ಣಾಂಕ		*stopopteval,
    ಅಕ್ಷರ	*errbuf,
    size_t	errbuflen)
{
    ಪೂರ್ಣಾಂಕ		ಮೌಲ್ಯ_checked = FALSE;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;
    char_u	*arg = *argp;

    ಆದರೆ (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_ಗಣ_option_bool(opt_idx, opt_flags, ಪೂರ್ವಪ್ರತ್ಯಯ, flags, varp,
						nextchar, afterchar, cp_val);
	ಆದರೆ (errmsg != ಖಾಲಿ)
	    ಹೋಗು skip;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// numeric or string option
	ಆದರೆ (vim_strchr((char_u *)"=:&<", nextchar) == ಖಾಲಿ
		|| ಪೂರ್ವಪ್ರತ್ಯಯ != ಪೂರ್ವಪ್ರತ್ಯಯ_NONE)
	{
	    errmsg = e_invalid_ವಾದ;
	    ಹೋಗು skip;
	}

	ಆದರೆ (flags & P_ಅಂಕೆ)
	{
	    // numeric option
	    errmsg = do_ಗಣ_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    ಆದರೆ (errmsg != ಖಾಲಿ)
		ಹೋಗು skip;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt_idx >= 0)
	{
	    // string option
	    ಆದರೆ (do_ಗಣ_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&ಮೌಲ್ಯ_checked, &errmsg) == FAIL)
	    {
		ಆದರೆ (errmsg != ಖಾಲಿ)
		    ಹೋಗು skip;
		*stopopteval = TRUE;
		ಹೋಗು skip;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // key code option
	    errmsg = do_ಗಣ_option_keycode(&arg, ಕೀಲಿ_name, nextchar);
	    ಆದರೆ (errmsg != ಖಾಲಿ)
		ಹೋಗು skip;
	}
    }

    ಆದರೆ (opt_idx >= 0)
	did_ಗಣ_option(opt_idx, opt_flags, op == OP_NONE, ಮೌಲ್ಯ_checked);

skip:
    *argp = arg;
    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
do_ಗಣ_option(
    ಪೂರ್ಣಾಂಕ		opt_flags,
    char_u	**argp,
    char_u	*arg_ಆರಂಭ,
    char_u	**startarg,
    ಪೂರ್ಣಾಂಕ		*did_show,
    ಪೂರ್ಣಾಂಕ		*stopopteval,
    ಅಕ್ಷರ	*errbuf,
    size_t	errbuflen)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;
    char_u	*arg;
    ಗಣ_ಪೂರ್ವಪ್ರತ್ಯಯ_T ಪೂರ್ವಪ್ರತ್ಯಯ;	// no prefix, "no" prefix or "inv" prefix
    ಗಣ_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	ಕೀಲಿ_name[2];
    ಪೂರ್ಣಾಂಕ		nextchar;	// next non-white char after option name
    ಪೂರ್ಣಾಂಕ		afterchar;	// character just after option name
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಕೀಲಿ;
    ಪೂರ್ಣಾಂಕ		len;

    ಪೂರ್ವಪ್ರತ್ಯಯ = get_option_ಪೂರ್ವಪ್ರತ್ಯಯ(argp);
    arg = *argp;

    // find end of name
    ಕೀಲಿ = 0;
    ಆದರೆ (parse_option_name(arg, &opt_idx, &len, &ಕೀಲಿ) == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    // remember character after option name
    afterchar = arg[len];

    ಆದರೆ (ಒಳ_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	ಆದರೆ (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != ಖಾಲಿ
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    ಹೋಗು skip;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	ಆಗ (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    ಆದರೆ (op != OP_NONE)
	len++;

    nextchar = arg[len];

    ಆದರೆ (opt_idx == -1 && ಕೀಲಿ == 0)	// found a mismatch: skip
    {
	ಆದರೆ (ಒಳ_vim9script() && arg > arg_ಆರಂಭ
		&& vim_strchr((char_u *)"!&<", *arg) != ಖಾಲಿ)
	    errmsg = e_no_white_space_allowed_between_option_and;
	ಇಲ್ಲದಿದ್ದರೆ
	    errmsg = e_unknown_option;
	ಹೋಗು skip;
    }

    ಆದರೆ (opt_idx >= 0)
    {
	ಆದರೆ (options[opt_idx].var == ಖಾಲಿ)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    ಆದರೆ (vim_strchr((char_u *)"=:!&<", nextchar) == ಖಾಲಿ
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    ಹೋಗು skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	flags = P_STRING;
	varp = ಖಾಲಿ;
	ಆದರೆ (ಕೀಲಿ < 0)
	{
	    ಕೀಲಿ_name[0] = KEY2TERMCAP0(ಕೀಲಿ);
	    ಕೀಲಿ_name[1] = KEY2TERMCAP1(ಕೀಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಕೀಲಿ_name[0] = KS_ಕೀಲಿ;
	    ಕೀಲಿ_name[1] = (ಕೀಲಿ & 0xff);
	}
    }

    // Make sure the option value can be changed.
    ಆದರೆ (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, ಪೂರ್ವಪ್ರತ್ಯಯ) == FAIL)
	ಹೋಗು skip;

    ಪೂರ್ಣಾಂಕ cp_val = p_cp;
    ಆದರೆ (vim_strchr((char_u *)"?=:!&<", nextchar) != ಖಾಲಿ)
    {
	arg += len;
	ಆದರೆ (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    ಆದರೆ (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	ಆದರೆ (vim_strchr((char_u *)"?!&<", nextchar) != ಖಾಲಿ
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    ಹೋಗು skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    ಆದರೆ (nextchar == '?'
	    || (ಪೂರ್ವಪ್ರತ್ಯಯ == ಪೂರ್ವಪ್ರತ್ಯಯ_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == ಖಾಲಿ
		&& !(flags & P_BOOL)))
    {
	// print value
	ಆದರೆ (*did_show)
	    ಸಂದೇಶ_putchar('\n');	    // cursor below last one
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	ಆದರೆ (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಆದರೆ (p_verbose > 0)
	    {
		// Mention where the option was last set.
		ಆದರೆ (varp == options[opt_idx].var)
		    ಕೊನೆಯ_ಗಣ_ಸಂದೇಶ(options[opt_idx].script_ctx);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಪೂರ್ಣಾಂಕ)options[opt_idx].indir & PV_WIN)
		    ಕೊನೆಯ_ಗಣ_ಸಂದೇಶ(curwin->w_p_script_ctx[
			    (ಪೂರ್ಣಾಂಕ)options[opt_idx].indir & PV_ಮುಖವಾಡ]);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಪೂರ್ಣಾಂಕ)options[opt_idx].indir & PV_BUF)
		    ಕೊನೆಯ_ಗಣ_ಸಂದೇಶ(curbuf->b_p_script_ctx[
			    (ಪೂರ್ಣಾಂಕ)options[opt_idx].indir & PV_ಮುಖವಾಡ]);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u	    *p;

	    p = ಕಂಡುಹಿಡಿ_termcode(ಕೀಲಿ_name);
	    ಆದರೆ (p == ಖಾಲಿ)
	    {
		errmsg = e_ಕೀಲಿ_ಸಂಕೇತ_not_ಗಣ;
		ಹೋಗು skip;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		(ಶೂನ್ಯ)show_one_termcode(ಕೀಲಿ_name, p, TRUE);
	}
	ಆದರೆ (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	errmsg = do_ಗಣ_option_ಮೌಲ್ಯ(opt_idx, opt_flags, &arg, ಪೂರ್ವಪ್ರತ್ಯಯ, op,
					flags, varp, ಕೀಲಿ_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    ಹಿಂತಿರುಗಿ errmsg;
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
    ಪೂರ್ಣಾಂಕ
do_ಗಣ(
    char_u	*arg_ಆರಂಭ,	// option string (may be written to!)
    ಪೂರ್ಣಾಂಕ		opt_flags)
{
    char_u	*arg = arg_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		did_show = FALSE;   // already showed one value

    ಆದರೆ (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	ಹೋಗು theend;
    }

    ಆಗ (*arg != NUL)		// loop to process all options
    {
	ಆದರೆ (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    ಆದರೆ (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		ಗಣ_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ		stopopteval = FALSE;
	    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;
	    ಅಕ್ಷರ	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_ಗಣ_option(opt_flags, &arg, arg_ಆರಂಭ, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    ಆದರೆ (stopopteval)
		ನಿಲ್ಲಿಸು;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    ಗಾಗಿ (i = 0; i < 2 ; ++i)
	    {
		ಆಗ (*arg != NUL && !VIM_ISWHITE(*arg))
		    ಆದರೆ (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		ಆದರೆ (*arg != '=')
		    ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (errmsg != ಖಾಲಿ)
	    {
		i = vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		ಆದರೆ (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((ಅಕ್ಷರ *)IObuff);	// show error highlighted
		--no_wait_return;

		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    ಆದರೆ (silent_ಬಹುಲಕ && did_show)
    {
	// After displaying option values in silent mode.
	silent_ಬಹುಲಕ = FALSE;
	info_ಸಂದೇಶ = TRUE;	// use mch_msg(), not mch_errmsg()
	ಸಂದೇಶ_putchar('\n');
	cursor_on();		// msg_start() switches it off
	ಹೊರಹೋಗು_flush();
	silent_ಬಹುಲಕ = TRUE;
	info_ಸಂದೇಶ = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    ಶೂನ್ಯ
did_ಗಣ_option(
    ಪೂರ್ಣಾಂಕ	    opt_idx,
    ಪೂರ್ಣಾಂಕ	    opt_flags,	    // possibly with OPT_MODELINE
    ಪೂರ್ಣಾಂಕ	    new_ಮೌಲ್ಯ_2,	    // value was replaced completely
    ಪೂರ್ಣಾಂಕ	    ಮೌಲ್ಯ_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_ಗಣ;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    ಆದರೆ (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    ಆದರೆ (!ಮೌಲ್ಯ_checked && (secure
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
	    || sandbox != 0
#ಆದರೆ_ಅಂತ್ಯ
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	ಆದರೆ (flagsp_local != ಖಾಲಿ)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (new_ಮೌಲ್ಯ_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	ಆದರೆ (flagsp_local != ಖಾಲಿ)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    ಪೂರ್ಣಾಂಕ
string_ಗಮ್ಯ_ಕೀಲಿ(char_u *arg, ಪೂರ್ಣಾಂಕ multi_byte)
{
    ಆದರೆ (*arg == '<' && arg[1])
	ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ಕೀಲಿ_option(arg + 1, TRUE);
    ಆದರೆ (*arg == '^' && arg[1])
    {
	ಪೂರ್ಣಾಂಕ ಕೀಲಿ = Ctrl_chr(arg[1]);
	ಆದರೆ (ಕೀಲಿ == 0)		// ^@ is <Nul>
	    ಕೀಲಿ = K_ZERO;
	ಹಿಂತಿರುಗಿ ಕೀಲಿ;
    }
    ಆದರೆ (multi_byte)
	ಹಿಂತಿರುಗಿ PTR2CHAR(arg);
    ಹಿಂತಿರುಗಿ *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    ಶೂನ್ಯ
did_ಗಣ_title(ಶೂನ್ಯ)
{
    ಆದರೆ (starting != NO_SCREEN
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    && !gui.starting
#ಆದರೆ_ಅಂತ್ಯ
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    ಶೂನ್ಯ
ಗಣ_options_bin(
    ಪೂರ್ಣಾಂಕ		oldval,
    ಪೂರ್ಣಾಂಕ		newval,
    ಪೂರ್ಣಾಂಕ		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    ಆದರೆ (newval)
    {
	ಆದರೆ (!oldval)		// switched on
	{
	    ಆದರೆ (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    ಆದರೆ (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	ಆದರೆ (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	ಆದರೆ (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oldval)		// switched off
    {
	ಆದರೆ (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	ಆದರೆ (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#ಆದರೆ defined(FEAT_EVAL)
    // Remember where the dependent option were reset
    didset_options_sctx(opt_flags, p_bin_dep_opts);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Expand environment variables for some string options.
 * These string options cannot be indirect!
 * If "val" is NULL expand the current value of the option.
 * Return pointer to NameBuff, or NULL when not expanded.
 */
    ಸ್ಥಿರ char_u *
option_expand(ಪೂರ್ಣಾಂಕ opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    ಆದರೆ (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    ಆದರೆ (val != ಖಾಲಿ && STRLEN(val) > MAXPATHL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (val == ಖಾಲಿ)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    ಪೂರ್ಣಾಂಕ esc = var == &p_ಟ್ಯಾಗುಗಳು || var == &p_ಮಾರ್ಗ;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : ಖಾಲಿ, FALSE,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#ಆದರೆ_ಅಂತ್ಯ
				  ಖಾಲಿ);
    ಆದರೆ (STRCMP(NameBuff, val) == 0)   // they are the same
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ NameBuff;
}

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
didset_options(ಶೂನ್ಯ)
{
    // initialize the table for 'iskeyword' et.al.
    (ಶೂನ್ಯ)init_chartab();

    didset_string_options();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    (ಶೂನ್ಯ)spell_check_msm();
    (ಶೂನ್ಯ)spell_check_sps();
    (ಶೂನ್ಯ)compile_cap_prog(curwin->w_s);
    (ಶೂನ್ಯ)did_ಗಣ_spell_option();
#ಆದರೆ_ಅಂತ್ಯ
    // set cedit_key
    (ಶೂನ್ಯ)did_ಗಣ_cedit(ಖಾಲಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_ಗಣ_breakat(ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
didset_options2(ಶೂನ್ಯ)
{
    // Initialize the highlight_attr[] table.
    (ಶೂನ್ಯ)highlight_ಬದಲಾಯಿತು();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (ಶೂನ್ಯ)ಗಣ_listchars_option(curwin, curwin->w_p_lcs, TRUE, ಖಾಲಿ, 0);

    // Parse default for 'fillchars'.
    (ಶೂನ್ಯ)ಗಣ_fillchars_option(curwin, curwin->w_p_fcs, TRUE, ಖಾಲಿ, 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (ಶೂನ್ಯ)did_ಗಣ_clipboard(ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (ಶೂನ್ಯ)tabstop_ಗಣ(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (ಶೂನ್ಯ)tabstop_ಗಣ(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    ಶೂನ್ಯ
check_options(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;

    ಗಾಗಿ (opt_idx = 0; options[opt_idx].fullname != ಖಾಲಿ; opt_idx++)
	ಆದರೆ ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != ಖಾಲಿ)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    ಪೂರ್ಣಾಂಕ
get_ಪದ_opt_idx(char_u **p)
{
    ಪೂರ್ಣಾಂಕ opt_idx;

    ಗಾಗಿ (opt_idx = 1; options[opt_idx].fullname != ಖಾಲಿ; opt_idx++)
	ಆದರೆ (options[opt_idx].var == (char_u *)p)
	    ಹಿಂತಿರುಗಿ opt_idx;
    ಹಿಂತಿರುಗಿ -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ಪದ_option_alloced(char_u **p)
{
    ಪೂರ್ಣಾಂಕ		opt_idx = get_ಪದ_opt_idx(p);

    ಆದರೆ (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    ಹಿಂತಿರುಗಿ opt_idx;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    ಪೂರ್ಣಾಂಕ
was_ಗಣ_insecurely(win_T *wp, char_u *opt, ಪೂರ್ಣಾಂಕ opt_flags)
{
    ಪೂರ್ಣಾಂಕ	    idx = findoption(opt);
    long_u  *flagp;

    ಆದರೆ (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	ಹಿಂತಿರುಗಿ (*flagp & P_INSECURE) != 0;
    }
    ಆಂತರಿಕ_error("was_set_insecurely()");
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    ಸ್ಥಿರ long_u *
insecure_flag(win_T *wp, ಪೂರ್ಣಾಂಕ opt_idx, ಪೂರ್ಣಾಂಕ opt_flags)
{
    ಆದರೆ (opt_flags & OPT_LOCAL)
	ಆಯ್ಕೆ ((ಪೂರ್ಣಾಂಕ)options[opt_idx].indir)
	{
	    ಸ್ಥಿತಿ PV_WRAP:	ಹಿಂತಿರುಗಿ &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    ಸ್ಥಿತಿ PV_STL:	ಹಿಂತಿರುಗಿ &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    ಸ್ಥಿತಿ PV_FDE:	ಹಿಂತಿರುಗಿ &wp->w_p_fde_flags;
	    ಸ್ಥಿತಿ PV_FDT:	ಹಿಂತಿರುಗಿ &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    ಸ್ಥಿತಿ PV_BEXPR:	ಹಿಂತಿರುಗಿ &wp->w_ಬಫರ್->b_p_bexpr_flags;
#  endif
	    ಸ್ಥಿತಿ PV_INDE:	ಹಿಂತಿರುಗಿ &wp->w_ಬಫರ್->b_p_inde_flags;
	    ಸ್ಥಿತಿ PV_FEX:	ಹಿಂತಿರುಗಿ &wp->w_ಬಫರ್->b_p_fex_flags;
#  ifdef FEAT_ಕಂಡುಹಿಡಿ_ID
	    ಸ್ಥಿತಿ PV_INEX:	ಹಿಂತಿರುಗಿ &wp->w_ಬಫರ್->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    ಸ್ಥಿತಿ PV_CPT:	ಹಿಂತಿರುಗಿ &wp->w_ಬಫರ್->b_p_cpt_flags;
#  endif
# endif
	}
    ಇಲ್ಲದಿದ್ದರೆ
	// For global value of window-local options, use flags in w_allbuf_opt.
	ಆಯ್ಕೆ ((ಪೂರ್ಣಾಂಕ)options[opt_idx].indir)
	{
	    ಸ್ಥಿತಿ PV_WRAP:	ಹಿಂತಿರುಗಿ &wp->w_allbuf_opt.wo_wrap_flags;
# ಆದರೆ defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    ಸ್ಥಿತಿ PV_FDE:	ಹಿಂತಿರುಗಿ &wp->w_allbuf_opt.wo_fde_flags;
	    ಸ್ಥಿತಿ PV_FDT:	ಹಿಂತಿರುಗಿ &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    ಹಿಂತಿರುಗಿ &options[opt_idx].flags;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Redraw the window title and/or tab page text later.
 */
    ಶೂನ್ಯ
redraw_titles(ಶೂನ್ಯ)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    ಪೂರ್ಣಾಂಕ
valid_name(char_u *val, ಅಕ್ಷರ *allowed)
{
    char_u *s;

    ಗಾಗಿ (s = val; *s != NUL; ++s)
	ಆದರೆ (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    ಶೂನ್ಯ
ಗಣ_option_sctx_idx(ಪೂರ್ಣಾಂಕ opt_idx, ಪೂರ್ಣಾಂಕ opt_flags, sctx_T script_ctx)
{
    ಪೂರ್ಣಾಂಕ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    ಪೂರ್ಣಾಂಕ		indir = (ಪೂರ್ಣಾಂಕ)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    ಆದರೆ (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    ಆದರೆ (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_VIM9;
    ಆದರೆ (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    ಆದರೆ (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    ಆದರೆ (both || (opt_flags & OPT_LOCAL))
    {
	ಆದರೆ (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_ಮುಖವಾಡ] = new_script_ctx;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_ಮುಖವಾಡ] = new_script_ctx;
	    ಆದರೆ (both)
		// also setting the "all buffers" value
		curwin->w_allbuf_opt.wo_script_ctx[indir & PV_ಮುಖವಾಡ] =
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
is_option_ಮೌಲ್ಯ_vim9(ಪೂರ್ಣಾಂಕ opt_idx, ಪೂರ್ಣಾಂಕ opt_flags)
{
    ಪೂರ್ಣಾಂಕ		indir = (ಪೂರ್ಣಾಂಕ)options[opt_idx].indir;
    sctx_T	*sctx = ಖಾಲಿ;

    ಆದರೆ ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    ಆದರೆ ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	ಆದರೆ (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_ಮುಖವಾಡ];
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_ಮುಖವಾಡ];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    ಹಿಂತಿರುಗಿ !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_ಆವೃತ್ತಿ >= SCRIPT_ಆವೃತ್ತಿ_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(ಅಕ್ಷರ *name)
{
    ಪೂರ್ಣಾಂಕ idx = findoption((char_u *)name);

    ಆದರೆ (idx >= 0)
	ಹಿಂತಿರುಗಿ &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    ಶೂನ್ಯ
ಗಣ_ಪದ_option_sctx_idx(ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ opt_idx)
{
    char_u  buf[5];
    ಪೂರ್ಣಾಂಕ	    idx;

    ಆದರೆ (name == ಖಾಲಿ)
	idx = opt_idx;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    ಆದರೆ (idx >= 0)
	ಗಣ_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
apply_optionset_autocmd(
	ಪೂರ್ಣಾಂಕ	opt_idx,
	ದೀರ್ಘ	opt_flags,
	ದೀರ್ಘ	oldval,
	ದೀರ್ಘ	oldval_g,
	ದೀರ್ಘ	newval,
	ಅಕ್ಷರ	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    ಆದರೆ (starting || errmsg != ಖಾಲಿ || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	ಹಿಂತಿರುಗಿ;

    len = vim_snprintf_safelen((ಅಕ್ಷರ *)buf_new, ಗಾತ್ರ(buf_new), "%ld", newval);
    ಗಣ_vim_var_string(VV_OPTION_NEW, buf_new, (ಪೂರ್ಣಾಂಕ)len);
    len = vim_snprintf_safelen((ಅಕ್ಷರ *)buf_type, ಗಾತ್ರ(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    ಗಣ_vim_var_string(VV_OPTION_TYPE, buf_type, (ಪೂರ್ಣಾಂಕ)len);

    buf_oldlen = vim_snprintf_safelen((ಅಕ್ಷರ *)buf_old, ಗಾತ್ರ(buf_old), "%ld", oldval);
    ಗಣ_vim_var_string(VV_OPTION_OLD, buf_old, (ಪೂರ್ಣಾಂಕ)buf_oldlen);

    ಆದರೆ (opt_flags & OPT_LOCAL)
    {
	ಗಣ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	ಗಣ_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ಪೂರ್ಣಾಂಕ)buf_oldlen);
    }
    ಆದರೆ (opt_flags & OPT_GLOBAL)
    {
	ಗಣ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	ಗಣ_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (ಪೂರ್ಣಾಂಕ)buf_oldlen);
    }
    ಆದರೆ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((ಅಕ್ಷರ *)buf_old_global,
	    ಗಾತ್ರ(buf_old_global), "%ld", oldval_g);

	ಗಣ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	ಗಣ_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ಪೂರ್ಣಾಂಕ)buf_oldlen);
	ಗಣ_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (ಪೂರ್ಣಾಂಕ)buf_old_globallen);
    }
    ಆದರೆ (opt_flags & OPT_MODELINE)
    {
	ಗಣ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	ಗಣ_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ಪೂರ್ಣಾಂಕ)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    ಖಾಲಿ, FALSE, ಖಾಲಿ);
    reset_v_option_vars();
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_arabic(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	ಆದರೆ (!p_tbidi)
	{
	    // set rightleft mode
	    ಆದರೆ (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    ಆದರೆ (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	ಆದರೆ (STRCMP(p_enc, "utf-8") != 0)
	{
	    ಸ್ಥಿರ ಅಕ್ಷರ *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    ಸಂದೇಶ_ಮೂಲ(HL_ATTR(HLF_W));
	    ಸಂದೇಶ_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    ಗಣ_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = ಗಣ_option_ಮೌಲ್ಯ((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// 'arabic' is reset, handle various sub-settings.
	ಆದರೆ (!p_tbidi)
	{
	    // reset rightleft mode
	    ಆದರೆ (curwin->w_p_rl)
	    {
		curwin->w_p_rl = FALSE;
		ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting();
	    }

	    // 'arabicshape' isn't reset, it is a global option and
	    // another window may still need it "on".
	}

	// 'delcombine' isn't reset, it is a global option and another
	// window may still want it "on".

# ifdef FEAT_KEYMAP
	// Revert to the default keymap
	curbuf->b_p_iminsert = B_IMODE_NONE;
	curbuf->b_p_imsearch = B_IMODE_USE_ಸೇರಿಸು;
# endif
    }

    ಹಿಂತಿರುಗಿ errmsg;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_autochdir(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_ballooneval(optset_T *ವಾದಗಳು)
{
    ಆದರೆ (balloon_Eval_For_ಪದ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (p_beval && !ವಾದಗಳು->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!p_beval && ವಾದಗಳು->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BEVAL_ಪದ)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_balloonevalterm(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    mch_bevalterm_ಬದಲಾಯಿತು();
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the updated 'binary' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_ದ್ವಿಮಾನ(optset_T *ವಾದಗಳು)
{
    // when 'bin' is set also set some other options
    ಗಣ_options_bin(ವಾದಗಳು->os_oldval.boolean, curbuf->b_p_bin, ವಾದಗಳು->os_flags);
    redraw_titles();

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the updated 'buflisted' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_buflisted(optset_T *ವಾದಗಳು)
{
    // when 'buflisted' changes, trigger autocommands
    ಆದರೆ (ವಾದಗಳು->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						ಖಾಲಿ, ಖಾಲಿ, TRUE, curbuf);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the new 'cmdheight' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_cmdheight(optset_T *ವಾದಗಳು)
{
    ದೀರ್ಘ old_ಮೌಲ್ಯ = ವಾದಗಳು->os_oldval.ಸಂಖ್ಯೆ;
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    // if p_ch changed value, change the command line height
    ಆದರೆ (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_ch = MIN_CMDHEIGHT;
    }
    ಆದರೆ (p_ch > ಅಡ್ಡಸಾಲುಗಳು - min_ಅಡ್ಡಸಾಲುಗಳು() + MIN_CMDHEIGHT)
	p_ch = ಅಡ್ಡಸಾಲುಗಳು - min_ಅಡ್ಡಸಾಲುಗಳು() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    ಆದರೆ ((p_ch != old_ಮೌಲ್ಯ
		|| tabline_ಎತ್ತರ() + topframe->fr_ಎತ್ತರ != ಅಡ್ಡಸಾಲುಗಳು - p_ch)
	    && ತುಂಬಿದ_screen
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    && !gui.starting
#ಆದರೆ_ಅಂತ್ಯ
       )
	command_ಎತ್ತರ();

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_compatible(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    compatible_ಗಣ();
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_conceallevel(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (curwin->w_p_cole < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	curwin->w_p_cole = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_ವಾದ;
	curwin->w_p_cole = 3;
    }
    ಆದರೆ (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_ವಾದ;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    ಹಿಂತಿರುಗಿ errmsg;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_diff(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    ಆದರೆ (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_eof_eol_fixeol_bomb(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // redraw the window title and tab page text
    redraw_titles();
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the updated 'equalalways' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_equalalways(optset_T *ವಾದಗಳು)
{
    ಆದರೆ (p_ea && !ವಾದಗಳು->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_foldcolumn(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (curwin->w_p_fdc < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	curwin->w_p_fdc = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_ವಾದ;
	curwin->w_p_fdc = 12;
    }
    ಆದರೆ (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_ವಾದ;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_foldlevel(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the new 'foldminlines' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_foldminlines(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    foldUpdateAll(curwin);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_foldnestmax(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_hlsearch(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    ಗಣ_no_hlsearch(FALSE);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the updated 'ignorecase' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_ignorecase(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    ಆದರೆ (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(HAVE_ಒಳಹರಿವು_ವಿಧಾನ)
/*
 * Process the updated 'imdisable' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_imdisable(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // Only de-activate it here, it will be enabled when changing mode.
    ಆದರೆ (p_imdisable)
	im_ಗಣ_ಸಕ್ರಿಯ(FALSE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_ಗಣ_ಸಕ್ರಿಯ(curbuf->b_p_iminsert == B_IMODE_IM);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the new 'iminsert' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_iminsert(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_ಕೊನೆಯ)
    {
	errmsg = e_invalid_ವಾದ;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    ಆದರೆ (termcap_ಸಕ್ರಿಯ)	// don't do this in the alternate screen
	showmode();
#ಆದರೆ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    ಸ್ಥಿತಿ_redraw_curbuf();
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_imsearch(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_ಕೊನೆಯ)
    {
	errmsg = e_invalid_ವಾದ;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    ಹಿಂತಿರುಗಿ errmsg;
}

#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_imstyle(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ errmsg;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the updated 'insertmode' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_insertmode(optset_T *ವಾದಗಳು)
{
    // when 'insertmode' is set from an autocommand need to do work here
    ಆದರೆ (p_im)
    {
	ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) == 0)
	    need_ಆರಂಭ_insertmode = TRUE;
	stop_ಸೇರಿಸು_ಬಹುಲಕ = FALSE;
    }
    // only reset if it was set previously
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಾದಗಳು->os_oldval.boolean)
    {
	need_ಆರಂಭ_insertmode = FALSE;
	stop_ಸೇರಿಸು_ಬಹುಲಕ = TRUE;
	ಆದರೆ (restart_edit != 0 && ಬಹುಲಕ_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_langnoremap(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the updated 'langremap' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_langremap(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the new 'laststatus' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_laststatus(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಕೊನೆಯ_ಸ್ಥಿತಿ(FALSE);	// (re)set last window status line
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_linespace(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    ಆದರೆ (gui.ಒಳ_use && gui_mch_adjust_charheight() == OK)
	gui_ಗಣ_shellsize(FALSE, FALSE, RESIZE_VERT);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the updated 'lisp' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_lisp(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (ಶೂನ್ಯ)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the new 'maxcombine' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_maxcombine(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the updated 'modifiable' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_modifiable(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // when 'modifiable' is changed, redraw the window title

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    ಆದರೆ (curbuf->b_p_ma && (ಪದ_ಒಳ_ಸಾಮಾನ್ಯ_ಬಹುಲಕ(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_ಪದ != ಖಾಲಿ && !ಪದ_is_ಪೂರ್ಣಗೊಂಡ(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	ಹಿಂತಿರುಗಿ e_cannot_make_terminal_with_running_ಕೆಲಸ_modifiable;
    }
#ಆದರೆ_ಅಂತ್ಯ
    redraw_titles();

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the updated 'modified' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_modified(optset_T *ವಾದಗಳು)
{
    ಆದರೆ (!ವಾದಗಳು->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_ಗಣ = !!ವಾದಗಳು->os_newval.boolean;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_mousehide(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (!p_mh)
	gui_mch_mousehide(FALSE);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_ಸಂಖ್ಯೆ_relativenumber(optset_T *ವಾದಗಳು ಬಳಸದ)
{
#ಆದರೆ defined(FEAT_SIGNS) && defined(FEAT_GUI)
    ಆದರೆ (gui.ಒಳ_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != ಖಾಲಿ)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	ಆದರೆ (!(curwin->w_p_nu && ((ಪೂರ್ಣಾಂಕ *)ವಾದಗಳು->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_numberwidth(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    // 'numberwidth' must be positive
    ಆದರೆ (curwin->w_p_nuw < 1)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	curwin->w_p_nuw = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_ವಾದ;
	curwin->w_p_nuw = 20;
    }
    ಆದರೆ (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_ವಾದ;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_ಸಾಲು_ಎಣಿಕೆ = 0; // trigger a redraw

    ಹಿಂತಿರುಗಿ errmsg;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_osctimeoutlen(optset_T *ವಾದಗಳು)
{
    ಆದರೆ (p_ost < 0)
    {
	p_ost = ವಾದಗಳು->os_oldval.ಸಂಖ್ಯೆ;
	ಹಿಂತಿರುಗಿ e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    ಅಕ್ಷರ *
did_ಗಣ_paste(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	old_p_paste = FALSE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	save_sm = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	save_sta = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	save_ru = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	save_ri = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	save_hkmap = 0;
#ಆದರೆ_ಅಂತ್ಯ
    buf_T	*buf;

    ಆದರೆ (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	ಆದರೆ (!old_p_paste)
	{
	    // save options for each buffer
	    FOR_ALL_BUFFERS(buf)
	    {
		buf->b_p_tw_nopaste = buf->b_p_tw;
		buf->b_p_wm_nopaste = buf->b_p_wm;
		buf->b_p_sts_nopaste = buf->b_p_sts;
		buf->b_p_ai_nopaste = buf->b_p_ai;
		buf->b_p_et_nopaste = buf->b_p_et;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		ಆದರೆ (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != ಖಾಲಿಗಳು_option
					   ? vim_strsave(buf->b_p_vsts) : ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    // save global options
	    save_sm = p_sm;
	    save_sta = p_sta;
	    save_ru = p_ru;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    save_ri = p_ri;
	    save_hkmap = p_hkmap;
#ಆದರೆ_ಅಂತ್ಯ
	    // save global values for local buffer options
	    p_ai_nopaste = p_ai;
	    p_et_nopaste = p_et;
	    p_sts_nopaste = p_sts;
	    p_tw_nopaste = p_tw;
	    p_wm_nopaste = p_wm;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	    ಆದರೆ (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != ಖಾಲಿಗಳು_option
						  ? vim_strsave(p_vsts) : ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	    ಆದರೆ (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = ಖಾಲಿಗಳು_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#ಆದರೆ_ಅಂತ್ಯ
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	ಆದರೆ (p_ru)
	    ಸ್ಥಿತಿ_redraw_all();    // redraw to remove the ruler
	p_ru = 0;		    // no ruler
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	p_ri = 0;		    // no reverse insert
	p_hkmap = 0;		    // no Hebrew keyboard
#ಆದರೆ_ಅಂತ್ಯ
	// set global values for local buffer options
	p_tw = 0;
	p_wm = 0;
	p_sts = 0;
	p_ai = 0;
	p_et = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	ಆದರೆ (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Paste switched from on to off: Restore saved values.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (old_p_paste)
    {
	// restore options for each buffer
	FOR_ALL_BUFFERS(buf)
	{
	    buf->b_p_tw = buf->b_p_tw_nopaste;
	    buf->b_p_wm = buf->b_p_wm_nopaste;
	    buf->b_p_sts = buf->b_p_sts_nopaste;
	    buf->b_p_ai = buf->b_p_ai_nopaste;
	    buf->b_p_et = buf->b_p_et_nopaste;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	    ಆದರೆ (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : ಖಾಲಿಗಳು_option;
	    vim_free(buf->b_p_vsts_array);
	    ಆದರೆ (buf->b_p_vsts && buf->b_p_vsts != ಖಾಲಿಗಳು_option)
		(ಶೂನ್ಯ)tabstop_ಗಣ(buf->b_p_vsts, &buf->b_p_vsts_array);
	    ಇಲ್ಲದಿದ್ದರೆ
		buf->b_p_vsts_array = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	ಆದರೆ (p_ru != save_ru)
	    ಸ್ಥಿತಿ_redraw_all();    // redraw to draw the ruler
	p_ru = save_ru;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	p_ri = save_ri;
	p_hkmap = save_hkmap;
#ಆದರೆ_ಅಂತ್ಯ
	// set global values for local buffer options
	p_ai = p_ai_nopaste;
	p_et = p_et_nopaste;
	p_sts = p_sts_nopaste;
	p_tw = p_tw_nopaste;
	p_wm = p_wm_nopaste;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	ಆದರೆ (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ
    }

    old_p_paste = p_paste;

#ಆದರೆ defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_previewwindow(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (!curwin->w_p_pvw)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	ಆದರೆ (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    ಹಿಂತಿರುಗಿ e_preview_ಕಿಟಕಿ_already_exists;
	}

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_pyxversion(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ errmsg;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the updated 'readonly' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_readonly(optset_T *ವಾದಗಳು)
{
    // when 'readonly' is reset globally, also reset readonlymode
    ಆದರೆ (!curbuf->b_p_ro && (ವಾದಗಳು->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    ಆದರೆ (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_scrollbind(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    ಆದರೆ (!curwin->w_p_scb)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_maxsearchcount(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ಹುಡುಕು_ಎಣಿಕೆ 9999

    ಆದರೆ (p_msc <= 0)
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_msc > MAX_ಹುಡುಕು_ಎಣಿಕೆ)
	errmsg = e_invalid_ವಾದ;

    ಆದರೆ (errmsg != ಖಾಲಿ)
	p_msc = 99;

    ಹಿಂತಿರುಗಿ errmsg;
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು MAX_ಹುಡುಕು_ಎಣಿಕೆ
}

#ಆದರೆ defined(BACKSLASH_ಒಳ_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_shellslash(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_shiftwidth_tabstop(optset_T *ವಾದಗಳು)
{
    ದೀರ್ಘ	*pp = (ದೀರ್ಘ *)ವಾದಗಳು->os_varp;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;

    ಆದರೆ (curbuf->b_p_ts <= 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	curbuf->b_p_ts = 8;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_ವಾದ;
	curbuf->b_p_ts = 8;
    }
    ಆದರೆ (p_ts <= 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_ts = 8;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_ವಾದ;
	p_ts = 8;
    }

    ಆದರೆ (curbuf->b_p_sw < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) > 0
		       ? tabstop_ಮೊದಲ(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#ಇಲ್ಲದಿದ್ದರೆ
	curbuf->b_p_sw = curbuf->b_p_ts;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (p_sw < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) > 0
	     ? tabstop_ಮೊದಲ(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#ಇಲ್ಲದಿದ್ದರೆ
	p_sw = curbuf->b_p_ts;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#ಆದರೆ_ಅಂತ್ಯ
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    ಆದರೆ (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_showtabline(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    shell_new_ಅಡ್ಡಸಾಲುಗಳು();	// recompute window positions and heights
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_smoothscroll(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_spell(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (curwin->w_p_spell)
	ಹಿಂತಿರುಗಿ parse_spelllang(curwin);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the updated 'swapfile' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_swapfile(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    ಆದರೆ (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    ಇಲ್ಲದಿದ್ದರೆ
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_TERMGUICOLORS)
    ಅಕ್ಷರ *
did_ಗಣ_termguicolors(optset_T *ವಾದಗಳು ಬಳಸದ)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    ಆದರೆ (
#  ifdef VIMDLL
	    !gui.ಒಳ_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_ಗಣ = TRUE;
	p_tgc = 0;
	ಹಿಂತಿರುಗಿ e_24_ಬಿಟ್_colors_are_not_supported_on_this_environment;
    }
    ಆದರೆ (is_ಪದ_win32())
	ಅದಲುಬದಲು_tcap();
# endif
# ifdef FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    ಆದರೆ (is_ಪದ_win32())
    {
	control_console_color_rgb();
	ಗಣ_termname(T_NAME);
	init_highlight(TRUE, FALSE);
    }
# endif
# ifdef FEAT_TERMINAL
    ಪದ_update_colors_all();
    ಪದ_update_palette_all();
    ಪದ_update_hlfwin_all();
# endif
    p_tgc_ಗಣ = TRUE;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಅಕ್ಷರ *
did_ಗಣ_termsync(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (!p_tsy)
	ಪದ_ಗಣ_sync_output(ಪದ_SYNC_OUTPUT_OFF);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_termwinscroll(optset_T *ವಾದಗಳು)
{
    ದೀರ್ಘ	*pp = (ದೀರ್ಘ *)ವಾದಗಳು->os_varp;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;

    ಆದರೆ (*pp < 1)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	*pp = 1;
    }

    ಹಿಂತಿರುಗಿ errmsg;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the updated 'terse' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_terse(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_ಹುಡುಕು);

    // insert 's' in p_shm
    ಆದರೆ (p_terse && p == ಖಾಲಿ)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	ಗಣ_string_option_ನೇರ((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!p_terse && p != ಖಾಲಿ)
	STRMOVE(p, p + 1);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the updated 'textauto' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_textauto(optset_T *ವಾದಗಳು)
{
    // when 'textauto' is set or reset also change 'fileformats'
    ಗಣ_string_option_ನೇರ((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | ವಾದಗಳು->os_flags, 0);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the updated 'textmode' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_textmode(optset_T *ವಾದಗಳು)
{
    // when 'textmode' is set or reset also change 'fileformat'
    ಗಣ_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, ವಾದಗಳು->os_flags);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the new 'textwidth' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_textwidth(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (curbuf->b_p_tw < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	curbuf->b_p_tw = 0;
    }
    ಆದರೆ (p_tw < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_tw = 0;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(ಖಾಲಿ, wp);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_title_icon(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_ಗಣ_title();
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the new 'titlelen' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_titlelen(optset_T *ವಾದಗಳು)
{
    ದೀರ್ಘ old_ಮೌಲ್ಯ = ವಾದಗಳು->os_oldval.ಸಂಖ್ಯೆ;
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    // if 'titlelen' has changed, redraw the title
    ಆದರೆ (p_titlelen < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_titlelen = 85;
    }
    ಆದರೆ (starting != NO_SCREEN && old_ಮೌಲ್ಯ != p_titlelen)
	need_maketitle = TRUE;

    ಹಿಂತಿರುಗಿ errmsg;
}

#ಆದರೆ defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_undofile(optset_T *ವಾದಗಳು)
{
    // Only take action when the option was set.
    ಆದರೆ (!curbuf->b_p_udf && !p_udf)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // When reset we do not delete the undo file, the option may be set again
    // without making any changes in between.
    char_u	hash[UNDO_HASH_ಗಾತ್ರ];
    buf_T	*save_curbuf = curbuf;

    FOR_ALL_BUFFERS(curbuf)
    {
	// When 'undofile' is set globally: for every buffer, otherwise
	// only for the current buffer: Try to read in the undofile,
	// if one exists, the buffer wasn't changed and the buffer was
	// loaded
	ಆದರೆ ((curbuf == save_curbuf
		    || (ವಾದಗಳು->os_flags & OPT_GLOBAL)
		    || ವಾದಗಳು->os_flags == 0)
		&& !curbuf_Is_ಬದಲಾಯಿತು() && curbuf->b_ml.ml_mfp != ಖಾಲಿ)
	{
# ifdef FEAT_CRYPT
	    ಆದರೆ (crypt_ವಿಧಾನ_is_sodium(crypt_get_ವಿಧಾನ_nr(curbuf)))
		ಮುಂದುವರಿ;
# endif
	    u_compute_hash(hash);
	    u_read_undo(ಖಾಲಿ, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the new global 'undolevels' option value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_global_undolevels(ದೀರ್ಘ ಮೌಲ್ಯ, ದೀರ್ಘ old_ಮೌಲ್ಯ)
{
    // sync undo before 'undolevels' changes

    // use the old value, otherwise u_sync() may not work properly
    p_ul = old_ಮೌಲ್ಯ;
    u_sync(TRUE);
    p_ul = ಮೌಲ್ಯ;
}

/*
 * Process the new buffer local 'undolevels' option value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_buflocal_undolevels(ದೀರ್ಘ ಮೌಲ್ಯ, ದೀರ್ಘ old_ಮೌಲ್ಯ)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_ಮೌಲ್ಯ;
    u_sync(TRUE);
    curbuf->b_p_ul = ಮೌಲ್ಯ;
}

/*
 * Process the new 'undolevels' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_undolevels(optset_T *ವಾದಗಳು)
{
    ದೀರ್ಘ *pp = (ದೀರ್ಘ *)ವಾದಗಳು->os_varp;

    ಆದರೆ (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(ವಾದಗಳು->os_newval.ಸಂಖ್ಯೆ,
						       ವಾದಗಳು->os_oldval.ಸಂಖ್ಯೆ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(ವಾದಗಳು->os_newval.ಸಂಖ್ಯೆ,
						       ವಾದಗಳು->os_oldval.ಸಂಖ್ಯೆ);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the new 'updatecount' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_updatecount(optset_T *ವಾದಗಳು)
{
    ದೀರ್ಘ old_ಮೌಲ್ಯ = ವಾದಗಳು->os_oldval.ಸಂಖ್ಯೆ;
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    // when 'updatecount' changes from zero to non-zero, open swap files
    ಆದರೆ (p_uc < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_uc = 100;
    }
    ಆದರೆ (p_uc && !old_ಮೌಲ್ಯ)
	ml_open_files();

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_weirdinvert(optset_T *ವಾದಗಳು)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    ಆದರೆ (p_wiv && !ವಾದಗಳು->os_oldval.boolean)
	T_XS = (char_u *)"y";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!p_wiv && ವಾದಗಳು->os_oldval.boolean)
	T_XS = ಖಾಲಿಗಳು_option;
    p_wiv = (*T_XS != NUL);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_wildchar(optset_T *ವಾದಗಳು)
{
    ದೀರ್ಘ c = *(ದೀರ್ಘ *)ವಾದಗಳು->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    ಆದರೆ (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the new 'window' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_ಕಿಟಕಿ(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (p_ಕಿಟಕಿ < 1)
	p_ಕಿಟಕಿ = 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_ಕಿಟಕಿ >= ಅಡ್ಡಸಾಲುಗಳು)
	p_ಕಿಟಕಿ = ಅಡ್ಡಸಾಲುಗಳು - 1;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_winheight_helpheight(optset_T *ವಾದಗಳು)
{
    ದೀರ್ಘ	*pp = (ದೀರ್ಘ *)ವಾದಗಳು->os_varp;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;

    ಆದರೆ (p_wh < 1)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_wh = 1;
    }
    ಆದರೆ (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    ಆದರೆ (p_hh < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_hh = 0;
    }

    // Change window height NOW
    ಆದರೆ (!ONE_ಕಿಟಕಿ)
    {
	ಆದರೆ (pp == &p_wh && curwin->w_ಎತ್ತರ < p_wh)
	    win_setheight((ಪೂರ್ಣಾಂಕ)p_wh);
	ಆದರೆ (pp == &p_hh && curbuf->b_help && curwin->w_ಎತ್ತರ < p_hh)
	    win_setheight((ಪೂರ್ಣಾಂಕ)p_hh);
    }

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_winminheight(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (p_wmh < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_wmh = 0;
    }
    ಆದರೆ (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_winminwidth(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (p_wmw < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_wmw = 0;
    }
    ಆದರೆ (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_winwidth(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (p_wiw < 1)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_wiw = 1;
    }
    ಆದರೆ (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    ಆದರೆ (!ONE_ಕಿಟಕಿ && curwin->w_ಅಗಲ < p_wiw)
	win_setwidth((ಪೂರ್ಣಾಂಕ)p_wiw);

    ಹಿಂತಿರುಗಿ errmsg;
}

#ಆದರೆ defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_wlsteal(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    clip_reset_wayland();

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_wltimeoutlen(optset_T *ವಾದಗಳು)
{
    ಆದರೆ (p_wtm < 0)
    {
	p_wtm = ವಾದಗಳು->os_oldval.ಸಂಖ್ಯೆ;
	ಹಿಂತಿರುಗಿ e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Process the updated 'wrap' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_wrap(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // Set w_leftcol or w_skipcol to zero.
    ಆದರೆ (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	curwin->w_skipcol = 0;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    ಅಕ್ಷರ *
did_ಗಣ_xhistory(optset_T *ವಾದಗಳು)
{
    ಪೂರ್ಣಾಂಕ is_p_chi = (ದೀರ್ಘ*)ವಾದಗಳು->os_varp == &p_chi;
    ಪೂರ್ಣಾಂಕ err;
    ದೀರ್ಘ *arg = (is_p_chi) ? &p_chi :(ದೀರ್ಘ*)ವಾದಗಳು->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    ಆದರೆ (*arg < 1)
    {
	*arg = ವಾದಗಳು->os_oldval.ಸಂಖ್ಯೆ;
	ಹಿಂತಿರುಗಿ e_cannot_have_negative_or_zero_ಸಂಖ್ಯೆ_ದ_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    ಆದರೆ (*arg > 100)
    {
	*arg = ವಾದಗಳು->os_oldval.ಸಂಖ್ಯೆ;
	ಹಿಂತಿರುಗಿ e_cannot_have_ಇನ್ನಷ್ಟು_than_hundred_quickfix;
    }

    ಆದರೆ (is_p_chi)
	err = qf_resize_ರಾಶಿ(*arg);
    ಇಲ್ಲದಿದ್ದರೆ
	err = ll_resize_ರಾಶಿ(curwin, *arg);

    ಆದರೆ (err == FAIL)
	ಹಿಂತಿರುಗಿ e_failed_resizing_quickfix_ರಾಶಿ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
ಗಣ_bool_option(
    ಪೂರ್ಣಾಂಕ		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    ಪೂರ್ಣಾಂಕ		ಮೌಲ್ಯ,			// new value
    ಪೂರ್ಣಾಂಕ		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    ಪೂರ್ಣಾಂಕ		old_ಮೌಲ್ಯ = *(ಪೂರ್ಣಾಂಕ *)varp;
#ಆದರೆ defined(FEAT_EVAL)
    ಪೂರ್ಣಾಂಕ		old_global_ಮೌಲ್ಯ = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;

    // Disallow changing some options from secure mode
    ಆದರೆ ((secure
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
		|| sandbox != 0
#ಆದರೆ_ಅಂತ್ಯ
		) && (options[opt_idx].flags & P_SECURE))
	ಹಿಂತಿರುಗಿ e_not_allowed_ಇಲ್ಲಿ;

#ಆದರೆ defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    ಆದರೆ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_ಮೌಲ್ಯ = *(ಪೂರ್ಣಾಂಕ *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#ಆದರೆ_ಅಂತ್ಯ

    *(ಪೂರ್ಣಾಂಕ *)varp = ಮೌಲ್ಯ;	    // set the new value
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Remember where the option was set.
    ಗಣ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    // May set global value for local option.
    ಆದರೆ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(ಪೂರ್ಣಾಂಕ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = ಮೌಲ್ಯ;

    // Handle side effects of changing a bool option.
    ಆದರೆ (options[opt_idx].opt_did_ಗಣ_cb != ಖಾಲಿ)
    {
	optset_T ವಾದಗಳು;

	CLEAR_ಕ್ಷೇತ್ರ(ವಾದಗಳು);
	ವಾದಗಳು.os_varp = varp;
	ವಾದಗಳು.os_flags = opt_flags;
	ವಾದಗಳು.os_oldval.boolean = old_ಮೌಲ್ಯ;
	ವಾದಗಳು.os_newval.boolean = ಮೌಲ್ಯ;
	ವಾದಗಳು.os_errbuf = ಖಾಲಿ;
	errmsg = options[opt_idx].opt_did_ಗಣ_cb(&ವಾದಗಳು);
	ಆದರೆ (errmsg != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_ಗಣ;

#ಆದರೆ defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(ದೀರ್ಘ)(old_ಮೌಲ್ಯ ? TRUE : FALSE),
				(ದೀರ್ಘ)(old_global_ಮೌಲ್ಯ ? TRUE : FALSE),
				(ದೀರ್ಘ)(ಮೌಲ್ಯ ? TRUE : FALSE), ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    comp_ಲಂಬಸಾಲು();			    // in case 'ruler' or 'showcmd' changed

    ಆದರೆ (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_ಗಣ_curswant = true;

    ಆದರೆ ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
check_ಅಂಕೆ_option_bounds(
    ದೀರ್ಘ	*pp,
    ದೀರ್ಘ	old_ಮೌಲ್ಯ,
    ದೀರ್ಘ	old_ಅಡ್ಡಸಾಲುಗಳು_2,
    ದೀರ್ಘ	old_Columns,
    ಅಕ್ಷರ	*errbuf,
    size_t	errbuflen,
    ಅಕ್ಷರ	*errmsg)
{
    ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು < min_ಅಡ್ಡಸಾಲುಗಳು_for_all_tabpages() && ತುಂಬಿದ_screen)
    {
	ಆದರೆ (errbuf != ಖಾಲಿ)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_ಕನಿಷ್ಠ_nr_ಸಾಲುಗಳು), min_ಅಡ್ಡಸಾಲುಗಳು_for_all_tabpages());
	    errmsg = errbuf;
	}
	ಅಡ್ಡಸಾಲುಗಳು = min_ಅಡ್ಡಸಾಲುಗಳು_for_all_tabpages();
    }
    ಆದರೆ (Columns < MIN_COLUMNS && ತುಂಬಿದ_screen)
    {
	ಆದರೆ (errbuf != ಖಾಲಿ)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_ಕನಿಷ್ಠ_nr_columns), MIN_COLUMNS);
	    errmsg = errbuf;
	}
	Columns = MIN_COLUMNS;
    }
    ಎಲ್ಲೆ_screen_ಗಾತ್ರ();

    // If the screen (shell) height has been changed, assume it is the
    // physical screenheight.
    ಆದರೆ (old_ಅಡ್ಡಸಾಲುಗಳು_2 != ಅಡ್ಡಸಾಲುಗಳು || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	ಆದರೆ (updating_screen)
	    *pp = old_ಮೌಲ್ಯ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ತುಂಬಿದ_screen
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		&& !gui.starting
#ಆದರೆ_ಅಂತ್ಯ
		)
	    ಗಣ_shellsize((ಪೂರ್ಣಾಂಕ)Columns, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, TRUE);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    ಆದರೆ (cmdline_ಅಡ್ಡಸಾಲು > ಅಡ್ಡಸಾಲುಗಳು - p_ch && ಅಡ್ಡಸಾಲುಗಳು > p_ch)
		cmdline_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - p_ch;
	}
	ಆದರೆ (p_ಕಿಟಕಿ >= ಅಡ್ಡಸಾಲುಗಳು || !option_was_ಗಣ((char_u *)"window"))
	    p_ಕಿಟಕಿ = ಅಡ್ಡಸಾಲುಗಳು - 1;
    }

    ಆದರೆ (p_tm < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_tm = 0;
    }
    ಆದರೆ ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_ಎತ್ತರ
		    && curwin->w_ಎತ್ತರ > 0))
	    && ತುಂಬಿದ_screen)
    {
	ಆದರೆ (pp == &(curwin->w_p_scr))
	{
	    ಆದರೆ (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_ಗಾತ್ರ;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	ಇಲ್ಲದಿದ್ದರೆ // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_ಎತ್ತರ;
    }
    ಆದರೆ (p_hi < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_hi = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_hi > 10000)
    {
	errmsg = e_invalid_ವಾದ;
	p_hi = 10000;
    }
    ಆದರೆ (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_ವಾದ;
	p_re = 0;
    }
    ಆದರೆ (p_report < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_report = 1;
    }
    ಆದರೆ ((p_sj < -100 || p_sj >= ಅಡ್ಡಸಾಲುಗಳು) && ತುಂಬಿದ_screen)
    {
	ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು != old_ಅಡ್ಡಸಾಲುಗಳು_2)	// Rows changed, just adjust p_sj
	    p_sj = ಅಡ್ಡಸಾಲುಗಳು / 2;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    errmsg = e_invalid_scroll_ಗಾತ್ರ;
	    p_sj = 1;
	}
    }
    ಆದರೆ (p_so < 0 && ತುಂಬಿದ_screen)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_so = 0;
    }
    ಆದರೆ (p_sop < 0 && ತುಂಬಿದ_screen)
    {
	errmsg = e_invalid_ವಾದ;
	p_sop = 0;
    }
    ಆದರೆ (p_siso < 0 && ತುಂಬಿದ_screen)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_siso = 0;
    }
    ಆದರೆ (p_cwh < 1)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_cwh = 1;
    }
    ಆದರೆ (p_ut < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_ut = 2000;
    }
    ಆದರೆ (p_ss < 0)
    {
	errmsg = e_ವಾದ_must_be_ಧನಾತ್ಮಕ;
	p_ss = 0;
    }

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
ಗಣ_ಅಂಕೆ_option(
    ಪೂರ್ಣಾಂಕ		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    ದೀರ್ಘ	ಮೌಲ್ಯ,			// new value
    ಅಕ್ಷರ	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    ಪೂರ್ಣಾಂಕ		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;
    ದೀರ್ಘ	old_ಮೌಲ್ಯ = *(ದೀರ್ಘ *)varp;
#ಆದರೆ defined(FEAT_EVAL)
    ದೀರ್ಘ	old_global_ಮೌಲ್ಯ = 0;	// only used when setting a local and
					// global option
#ಆದರೆ_ಅಂತ್ಯ
    ದೀರ್ಘ	old_ಅಡ್ಡಸಾಲುಗಳು_2 = ಅಡ್ಡಸಾಲುಗಳು;	// remember old Rows
    ದೀರ್ಘ	old_Columns = Columns;	// remember old Columns
    ದೀರ್ಘ	*pp = (ದೀರ್ಘ *)varp;

    // Disallow changing some options from secure mode.
    ಆದರೆ ((secure
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
		|| sandbox != 0
#ಆದರೆ_ಅಂತ್ಯ
		) && (options[opt_idx].flags & P_SECURE))
	ಹಿಂತಿರುಗಿ e_not_allowed_ಇಲ್ಲಿ;

#ಆದರೆ defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    ಆದರೆ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_ಮೌಲ್ಯ = *(ದೀರ್ಘ *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#ಆದರೆ_ಅಂತ್ಯ

    *pp = ಮೌಲ್ಯ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Remember where the option was set.
    ಗಣ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    // May set global value for local option.
    ಆದರೆ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(ದೀರ್ಘ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = ಮೌಲ್ಯ;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    ಆದರೆ (options[opt_idx].opt_did_ಗಣ_cb != ಖಾಲಿ)
    {
	optset_T ವಾದಗಳು;

	CLEAR_ಕ್ಷೇತ್ರ(ವಾದಗಳು);
	ವಾದಗಳು.os_varp = varp;
	ವಾದಗಳು.os_flags = opt_flags;
	ವಾದಗಳು.os_oldval.ಸಂಖ್ಯೆ = old_ಮೌಲ್ಯ;
	ವಾದಗಳು.os_newval.ಸಂಖ್ಯೆ = ಮೌಲ್ಯ;
	ವಾದಗಳು.os_errbuf = ಖಾಲಿ;
	errmsg = options[opt_idx].opt_did_ಗಣ_cb(&ವಾದಗಳು);
    }

    // Check the bounds for numeric options here
    errmsg = check_ಅಂಕೆ_option_bounds(pp, old_ಮೌಲ್ಯ, old_ಅಡ್ಡಸಾಲುಗಳು_2, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_ಗಣ;

#ಆದರೆ defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_ಮೌಲ್ಯ, old_global_ಮೌಲ್ಯ,
								ಮೌಲ್ಯ, errmsg);
#ಆದರೆ_ಅಂತ್ಯ

    comp_ಲಂಬಸಾಲು();			    // in case 'columns' or 'ls' changed

    ಆದರೆ (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_ಗಣ_curswant = true;

    ಆದರೆ ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    ಶೂನ್ಯ
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    ಪೂರ್ಣಾಂಕ		doclear = (flags & P_RCLR) == P_RCLR;
    ಪೂರ್ಣಾಂಕ		all = ((flags & P_RALL) == P_RALL || doclear);

    ಆದರೆ ((flags & P_RSTAT) || all)	// mark all status lines dirty
	ಸ್ಥಿತಿ_redraw_all();

    ಆದರೆ ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	ಆದರೆ (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting();
    }
    ಆದರೆ (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    ಆದರೆ (doclear)
	redraw_all_later(UPD_CLEAR);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    ಪೂರ್ಣಾಂಕ
findoption(char_u *arg)
{
    ಪೂರ್ಣಾಂಕ		    opt_idx;
    ಅಕ್ಷರ	    *s, *p;
    ಸ್ಥಿರ ಗಿಡ್ಡ    ಶೀಘ್ರ_tab[27] = {0, 0};	// quick access table
    ಪೂರ್ಣಾಂಕ		    is_ಪದ_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    ಆದರೆ (ಶೀಘ್ರ_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_opt_idx;
	ಗಾಗಿ (ಕೊನೆಯ_opt_idx = 0; options[ಕೊನೆಯ_opt_idx].fullname != ಖಾಲಿ;
		ಕೊನೆಯ_opt_idx++)
	    ;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ tab_idx = 1; tab_idx < 27; tab_idx++)
	    ಶೀಘ್ರ_tab[tab_idx] = ಕೊನೆಯ_opt_idx;

	p = options[0].fullname;
	ಗಾಗಿ (opt_idx = 1; opt_idx < ಕೊನೆಯ_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    ಆದರೆ (s[0] != p[0])
	    {
		ಆದರೆ (s[0] == 't' && s[1] == '_')
		    ಶೀಘ್ರ_tab[26] = opt_idx;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಶೀಘ್ರ_tab[Char_Ord_ಕೆಳಗಿನ(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    ಆದರೆ (arg[0] < 'a' || arg[0] > 'z')
	ಹಿಂತಿರುಗಿ -1;

    is_ಪದ_opt = (arg[0] == 't' && arg[1] == '_');
    ಆದರೆ (is_ಪದ_opt)
	opt_idx = ಶೀಘ್ರ_tab[26];
    ಇಲ್ಲದಿದ್ದರೆ
	opt_idx = ಶೀಘ್ರ_tab[Char_Ord_ಕೆಳಗಿನ(arg[0])];
    ಗಾಗಿ (; (s = options[opt_idx].fullname) != ಖಾಲಿ && s[0] == arg[0]; opt_idx++)
    {
	ಆದರೆ (STRCMP(arg, s) == 0)		    // match full name
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (s != ಖಾಲಿ && s[0] != arg[0])
	s = ಖಾಲಿ;
    ಆದರೆ (s == ಖಾಲಿ && !is_ಪದ_opt)
    {
	opt_idx = ಶೀಘ್ರ_tab[Char_Ord_ಕೆಳಗಿನ(arg[0])];
	ಗಾಗಿ ( ; options[opt_idx].fullname != ಖಾಲಿ; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    ಆದರೆ (s != ಖಾಲಿ && STRCMP(arg, s) == 0)   // match short name
		ನಿಲ್ಲಿಸು;
	    s = ಖಾಲಿ;
	}
    }
    ಆದರೆ (s == ಖಾಲಿ)
	opt_idx = -1;
    ಹಿಂತಿರುಗಿ opt_idx;
}

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
get_option_ಮೌಲ್ಯ(
    char_u	*name,
    ದೀರ್ಘ	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    ಪೂರ್ಣಾಂಕ		*flagsp,
    ಪೂರ್ಣಾಂಕ		scope)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    ಆದರೆ (opt_idx < 0)		    // option not in the table
    {
	ಪೂರ್ಣಾಂಕ ಕೀಲಿ;

	ಆದರೆ (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (ಕೀಲಿ = ಕಂಡುಹಿಡಿ_ಕೀಲಿ_option(name, FALSE)) != 0)
	{
	    char_u ಕೀಲಿ_name[2];
	    char_u *p;

	    ಆದರೆ (flagsp != ಖಾಲಿ)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    ಆದರೆ (ಕೀಲಿ < 0)
	    {
		ಕೀಲಿ_name[0] = KEY2TERMCAP0(ಕೀಲಿ);
		ಕೀಲಿ_name[1] = KEY2TERMCAP1(ಕೀಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಕೀಲಿ_name[0] = KS_ಕೀಲಿ;
		ಕೀಲಿ_name[1] = (ಕೀಲಿ & 0xff);
	    }
	    p = ಕಂಡುಹಿಡಿ_termcode(ಕೀಲಿ_name);
	    ಆದರೆ (p != ಖಾಲಿ)
	    {
		ಆದರೆ (stringval != ಖಾಲಿ)
		    *stringval = vim_strsave(p);
		ಹಿಂತಿರುಗಿ gov_string;
	    }
	}
	ಹಿಂತಿರುಗಿ gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    ಆದರೆ (flagsp != ಖಾಲಿ)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    ಆದರೆ (options[opt_idx].flags & P_STRING)
    {
	ಆದರೆ (varp == ಖಾಲಿ)		    // hidden option
	    ಹಿಂತಿರುಗಿ gov_hidden_string;
	ಆದರೆ (stringval != ಖಾಲಿ)
	{
	    ಆದರೆ ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((char_u **)varp == &curbuf->b_p_ಕೀಲಿ
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    ಇಲ್ಲದಿದ್ದರೆ
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	ಹಿಂತಿರುಗಿ gov_string;
    }

    ಆದರೆ (varp == ಖಾಲಿ)		    // hidden option
	ಹಿಂತಿರುಗಿ (options[opt_idx].flags & P_ಅಂಕೆ)
					 ? gov_hidden_ಸಂಖ್ಯೆ : gov_hidden_bool;
    ಆದರೆ (options[opt_idx].flags & P_ಅಂಕೆ)
	*numval = *(ದೀರ್ಘ *)varp;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	ಆದರೆ ((ಪೂರ್ಣಾಂಕ *)varp == &curbuf->b_ಬದಲಾಯಿತು)
	    *numval = curbuf_Is_ಬದಲಾಯಿತು();
	ಇಲ್ಲದಿದ್ದರೆ
	    *numval = (ದೀರ್ಘ) *(ಪೂರ್ಣಾಂಕ *)varp;
    }
    ಹಿಂತಿರುಗಿ (options[opt_idx].flags & P_ಅಂಕೆ) ? gov_ಸಂಖ್ಯೆ : gov_bool;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    ಪೂರ್ಣಾಂಕ
get_option_ಮೌಲ್ಯ_strict(
    char_u	*name,
    ದೀರ್ಘ	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    ಪೂರ್ಣಾಂಕ		opt_type,
    ಶೂನ್ಯ	*from)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;
    char_u	*varp = ಖಾಲಿ;
    ರಚನೆ vimoption *p;
    ಪೂರ್ಣಾಂಕ		r = 0;

    opt_idx = findoption(name);
    ಆದರೆ (opt_idx < 0)
	ಹಿಂತಿರುಗಿ 0;

    p = &(options[opt_idx]);

    // Hidden option
    ಆದರೆ (p->var == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->flags & P_ಅಂಕೆ)
	r |= SOPT_ಅಂಕೆ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->flags & P_STRING)
	r |= SOPT_STRING;

    ಆದರೆ (p->indir == PV_NONE)
    {
	ಆದರೆ (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ 0; // Did not request global-only option
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt_type == SREQ_GLOBAL)
	    ಹಿಂತಿರುಗಿ 0; // Requested global option

	ಆದರೆ (p->indir & PV_WIN)
	{
	    ಆದರೆ (opt_type == SREQ_BUF)
		ಹಿಂತಿರುಗಿ 0; // Did not request window-local option
	    ಇಲ್ಲದಿದ್ದರೆ
		r |= SOPT_WIN;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->indir & PV_BUF)
	{
	    ಆದರೆ (opt_type == SREQ_WIN)
		ಹಿಂತಿರುಗಿ 0; // Did not request buffer-local option
	    ಇಲ್ಲದಿದ್ದರೆ
		r |= SOPT_BUF;
	}
    }

    ಆದರೆ (stringval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ r;

    ಆದರೆ (opt_type == SREQ_GLOBAL)
	varp = p->var;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    ಆದರೆ (p->indir == PV_MOD)
	    {
		*numval = buf_Is_ಬದಲಾಯಿತು((buf_T *)from);
		varp = ಖಾಲಿ;
	    }
# ifdef FEAT_CRYPT
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->indir == PV_ಕೀಲಿ)
	    {
		// never return the value of the crypt key
		*stringval = ಖಾಲಿ;
		varp = ಖಾಲಿ;
	    }
# endif
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		buf_T *save_curbuf = curbuf;

		// only getting a pointer, no need to use aucmd_prepbuf()
		curbuf = (buf_T *)from;
		curwin->w_ಬಫರ್ = curbuf;
		varp = get_varp(p);
		curbuf = save_curbuf;
		curwin->w_ಬಫರ್ = curbuf;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_ಬಫರ್;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_ಬಫರ್;
	}
	ಆದರೆ (varp == p->var)
	    ಹಿಂತಿರುಗಿ (r | SOPT_UNSET);
    }

    ಆದರೆ (varp != ಖಾಲಿ)
    {
	ಆದರೆ (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->flags & P_ಅಂಕೆ)
	    *numval = *(ದೀರ್ಘ *) varp;
	ಇಲ್ಲದಿದ್ದರೆ
	    *numval = *(ಪೂರ್ಣಾಂಕ *)varp;
    }

    ಹಿಂತಿರುಗಿ r;
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
option_iter_ಮುಂದಿನ(ಶೂನ್ಯ **option, ಪೂರ್ಣಾಂಕ opt_type)
{
    ರಚನೆ vimoption	*ret = ಖಾಲಿ;
    ಮಾಡು
    {
	ಆದರೆ (*option == ಖಾಲಿ)
	    *option = (ಶೂನ್ಯ *) options;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (((ರಚನೆ vimoption *) (*option))->fullname == ಖಾಲಿ)
	{
	    *option = ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *option = (ಶೂನ್ಯ *) (((ರಚನೆ vimoption *) (*option)) + 1);

	ret = ((ರಚನೆ vimoption *) (*option));

	// Hidden option
	ಆದರೆ (ret->var == ಖಾಲಿ)
	{
	    ret = ಖಾಲಿ;
	    ಮುಂದುವರಿ;
	}

	ಆಯ್ಕೆ (opt_type)
	{
	    ಸ್ಥಿತಿ SREQ_GLOBAL:
		ಆದರೆ (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = ಖಾಲಿ;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ SREQ_BUF:
		ಆದರೆ (!(ret->indir & PV_BUF))
		    ret = ಖಾಲಿ;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ SREQ_WIN:
		ಆದರೆ (!(ret->indir & PV_WIN))
		    ret = ಖಾಲಿ;
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಆಂತರಿಕ_error("option_iter_next()");
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಆಗ (ret == ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ (char_u *)ret->fullname;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(ಪೂರ್ಣಾಂಕ opt_idx)
{
    ಹಿಂತಿರುಗಿ options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    ಶೂನ್ಯ
ಗಣ_option_flag(ಪೂರ್ಣಾಂಕ opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    ಪೂರ್ಣಾಂಕ
is_global_option(ಪೂರ್ಣಾಂಕ opt_idx)
{
    ಹಿಂತಿರುಗಿ options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    ಪೂರ್ಣಾಂಕ
is_global_local_option(ಪೂರ್ಣಾಂಕ opt_idx)
{
    ಹಿಂತಿರುಗಿ options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    ಪೂರ್ಣಾಂಕ
is_ಕಿಟಕಿ_local_option(ಪೂರ್ಣಾಂಕ opt_idx)
{
    ಹಿಂತಿರುಗಿ options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    ಪೂರ್ಣಾಂಕ
is_hidden_option(ಪೂರ್ಣಾಂಕ opt_idx)
{
    ಹಿಂತಿರುಗಿ options[opt_idx].var == ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    ಪೂರ್ಣಾಂಕ
is_crypt_ಕೀಲಿ_option(ಪೂರ್ಣಾಂಕ opt_idx)
{
    ಹಿಂತಿರುಗಿ options[opt_idx].indir == PV_ಕೀಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    ಅಕ್ಷರ *
ಗಣ_option_ಮೌಲ್ಯ(
    char_u	*name,
    ದೀರ್ಘ	ಸಂಖ್ಯೆ,
    char_u	*string,
    ಪೂರ್ಣಾಂಕ		opt_flags)	// OPT_LOCAL or 0 (both)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;
    char_u	*varp;
    long_u	flags;
    ಸ್ಥಿರ ಅಕ್ಷರ	errbuf[ERR_BUFLEN];
    ಪೂರ್ಣಾಂಕ		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    ಆದರೆ (opt_idx < 0)
    {
	ಪೂರ್ಣಾಂಕ ಕೀಲಿ;

	ಆದರೆ (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (ಕೀಲಿ = ಕಂಡುಹಿಡಿ_ಕೀಲಿ_option(name, FALSE)) != 0)
	{
	    char_u ಕೀಲಿ_name[2];

	    ಆದರೆ (ಕೀಲಿ < 0)
	    {
		ಕೀಲಿ_name[0] = KEY2TERMCAP0(ಕೀಲಿ);
		ಕೀಲಿ_name[1] = KEY2TERMCAP1(ಕೀಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಕೀಲಿ_name[0] = KS_ಕೀಲಿ;
		ಕೀಲಿ_name[1] = (ಕೀಲಿ & 0xff);
	    }
	    ಕೂಡಿಸು_termcode(ಕೀಲಿ_name, string, FALSE);
	    ಆದರೆ (ತುಂಬಿದ_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	flags = options[opt_idx].flags;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	ಆದರೆ (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_ಒಳ_sandbox));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (flags & P_STRING)
	    ಹಿಂತಿರುಗಿ ಗಣ_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	ಆದರೆ (varp != ಖಾಲಿ)	// hidden option is not changed
	{
	    ಆದರೆ (ಸಂಖ್ಯೆ == 0 && string != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ idx;

		// Either we are given a string or we are setting option
		// to zero.
		ಗಾಗಿ (idx = 0; string[idx] == '0'; ++idx)
		    ;
		ಆದರೆ (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_ಸಂಖ್ಯೆ_required_after_str_equal_str),
								 name, string);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;     // do nothing as we hit an error

		}
	    }
	    ಆದರೆ (flags & P_ಅಂಕೆ)
	    {
		ಹಿಂತಿರುಗಿ ಗಣ_ಅಂಕೆ_option(opt_idx, varp, ಸಂಖ್ಯೆ,
					   errbuf, ಗಾತ್ರ(errbuf), opt_flags);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ ಗಣ_bool_option(opt_idx, varp, (ಪೂರ್ಣಾಂಕ)ಸಂಖ್ಯೆ, opt_flags);
	}

    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    ಶೂನ್ಯ
ಗಣ_option_ಮೌಲ್ಯ_give_err(
    char_u	*name,
    ದೀರ್ಘ	ಸಂಖ್ಯೆ,
    char_u	*string,
    ಪೂರ್ಣಾಂಕ		opt_flags)	// OPT_LOCAL or 0 (both)
{
    ಅಕ್ಷರ *errmsg = ಗಣ_option_ಮೌಲ್ಯ(name, ಸಂಖ್ಯೆ, string, opt_flags);

    ಆದರೆ (errmsg != ಖಾಲಿ)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_ಪದ_ಸಂಕೇತ(char_u *tname)
{
    ಪೂರ್ಣಾಂಕ	    opt_idx;
    char_u  *varp;

    ಆದರೆ (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	ಆದರೆ (varp != ಖಾಲಿ)
	    varp = *(char_u **)(varp);
	ಹಿಂತಿರುಗಿ varp;
    }
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_termcode(tname + 2);
}

    char_u *
get_highlight_default(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    i = findoption((char_u *)"hl");
    ಆದರೆ (i >= 0)
	ಹಿಂತಿರುಗಿ options[i].def_val[VI_DEFAULT];
    ಹಿಂತಿರುಗಿ (char_u *)ಖಾಲಿ;
}

    char_u *
get_encoding_default(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    i = findoption((char_u *)"enc");
    ಆದರೆ (i >= 0)
	ಹಿಂತಿರುಗಿ options[i].def_val[VI_DEFAULT];
    ಹಿಂತಿರುಗಿ (char_u *)ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_QUICKFIX)
    ಪೂರ್ಣಾಂಕ
is_option_allocated(ಅಕ್ಷರ *name)
{
    ಪೂರ್ಣಾಂಕ idx = findoption((char_u *)name);

    ಹಿಂತಿರುಗಿ idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_ಕೀಲಿ_option(char_u *arg_arg, ಪೂರ್ಣಾಂಕ has_lt)
{
    ಪೂರ್ಣಾಂಕ		ಕೀಲಿ = 0;
    ಪೂರ್ಣಾಂಕ		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    ಆದರೆ (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	ಆದರೆ (!has_lt || arg[4] == '>')
	    ಕೀಲಿ = TERMCAP2_ಕೀಲಿ(arg[2], arg[3]);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	ಕೀಲಿ = ಕಂಡುಹಿಡಿ_special_ಕೀಲಿ(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_ಕೀಲಿ | FSK_SIMPLIFY, ಖಾಲಿ);
	ಆದರೆ (modifiers)		    // can't handle modifiers here
	    ಕೀಲಿ = 0;
    }
    ಹಿಂತಿರುಗಿ ಕೀಲಿ;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
showoptions(
    ಪೂರ್ಣಾಂಕ		all,
    ಪೂರ್ಣಾಂಕ		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    ರಚನೆ vimoption	*p;
    ಪೂರ್ಣಾಂಕ			ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ			isterm;
    char_u		*varp;
    ರಚನೆ vimoption	**ವಸ್ತುಗಳು;
    ಪೂರ್ಣಾಂಕ			ಅಂಶ_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ			ಸತತ;
    ಪೂರ್ಣಾಂಕ			ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲುಗಳು_2;
    ಪೂರ್ಣಾಂಕ			ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಪೂರ್ಣಾಂಕ			i;
    ಪೂರ್ಣಾಂಕ			len;

#ವ್ಯಾಖ್ಯಾನಿಸು INC 20
#ವ್ಯಾಖ್ಯಾನಿಸು GAP 3

    ವಸ್ತುಗಳು = ALLOC_MULT(ರಚನೆ vimoption *, OPTION_ಎಣಿಕೆ);
    ಆದರೆ (ವಸ್ತುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Highlight title
    ಆದರೆ (all == 2)
	ಸಂದೇಶ_puts_title(_("\n--- Terminal codes ---"));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt_flags & OPT_GLOBAL)
	ಸಂದೇಶ_puts_title(_("\n--- Global option values ---"));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt_flags & OPT_LOCAL)
	ಸಂದೇಶ_puts_title(_("\n--- Local option values ---"));
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    ಗಾಗಿ (ಸತತ = 1; ಸತತ <= 2 && !got_int; ++ಸತತ)
    {
	// collect the items in items[]
	ಅಂಶ_ಎಣಿಕೆ = 0;
	ಗಾಗಿ (p = &options[0]; p->fullname != ಖಾಲಿ; p++)
	{
	    // apply :filter /pat/
	    ಆದರೆ (ಸಂದೇಶ_filtered((char_u *)p->fullname))
		ಮುಂದುವರಿ;

	    varp = ಖಾಲಿ;
	    isterm = istermoption(p);
	    ಆದರೆ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		ಆದರೆ (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		varp = get_varp(p);
	    ಆದರೆ (varp != ಖಾಲಿ
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		ಆದರೆ (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    option_value2string(p, opt_flags);
		    len = (ಪೂರ್ಣಾಂಕ)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		ಆದರೆ ((len <= INC - GAP && ಸತತ == 1) ||
						(len > INC - GAP && ಸತತ == 2))
		    ವಸ್ತುಗಳು[ಅಂಶ_ಎಣಿಕೆ++] = p;
	    }
	}

	// display the items
	ಆದರೆ (ಸತತ == 1)
	{
	    ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = (Columns + GAP - 3) / INC;
	    ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ == 0)
		ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 1;
	    ಅಡ್ಡಸಾಲುಗಳು_2 = (ಅಂಶ_ಎಣಿಕೆ + ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1) / ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ	// run == 2
	    ಅಡ್ಡಸಾಲುಗಳು_2 = ಅಂಶ_ಎಣಿಕೆ;
	ಗಾಗಿ (ಅಡ್ಡಸಾಲು = 0; ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲುಗಳು_2 && !got_int; ++ಅಡ್ಡಸಾಲು)
	{
	    ಸಂದೇಶ_putchar('\n');			// go to next line
	    ಆದರೆ (got_int)			// 'q' typed in more
		ನಿಲ್ಲಿಸು;
	    ಲಂಬಸಾಲು = 0;
	    ಗಾಗಿ (i = ಅಡ್ಡಸಾಲು; i < ಅಂಶ_ಎಣಿಕೆ; i += ಅಡ್ಡಸಾಲುಗಳು_2)
	    {
		ಸಂದೇಶ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;			// make columns
		showoneopt(ವಸ್ತುಗಳು[i], opt_flags);
		ಲಂಬಸಾಲು += INC;
	    }
	    ಹೊರಹೋಗು_flush();
	    ui_breakcheck();
	}
    }
    vim_free(ವಸ್ತುಗಳು);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
optval_default(ರಚನೆ vimoption *p, char_u *varp, ಪೂರ್ಣಾಂಕ compatible)
{
    ಪೂರ್ಣಾಂಕ		dvi;

    ಆದರೆ (varp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    ಆದರೆ (p->flags & P_ಅಂಕೆ)
	ಹಿಂತಿರುಗಿ (*(ದೀರ್ಘ *)varp == (ದೀರ್ಘ)(long_i)p->def_val[dvi]);
    ಆದರೆ (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	ಹಿಂತಿರುಗಿ (*(ಪೂರ್ಣಾಂಕ *)varp == (ಪೂರ್ಣಾಂಕ)(ದೀರ್ಘ)(long_i)p->def_val[dvi]);
    // P_STRING
    ಹಿಂತಿರುಗಿ (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
showoneopt(
    ರಚನೆ vimoption	*p,
    ಪೂರ್ಣಾಂಕ			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    ಪೂರ್ಣಾಂಕ		save_silent = silent_ಬಹುಲಕ;

    silent_ಬಹುಲಕ = FALSE;
    info_ಸಂದೇಶ = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    ಆದರೆ ((p->flags & P_BOOL) && ((ಪೂರ್ಣಾಂಕ *)varp == &curbuf->b_ಬದಲಾಯಿತು
					? !curbuf_Is_ಬದಲಾಯಿತು() : !*(ಪೂರ್ಣಾಂಕ *)varp))
	ಸಂದೇಶ_puts("no");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((p->flags & P_BOOL) && *(ಪೂರ್ಣಾಂಕ *)varp < 0)
	ಸಂದೇಶ_puts("--");
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_puts("  ");
    ಸಂದೇಶ_puts(p->fullname);
    ಆದರೆ (!(p->flags & P_BOOL))
    {
	ಸಂದೇಶ_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	ಸಂದೇಶ_outtrans(NameBuff);
    }

    silent_ಬಹುಲಕ = save_silent;
    info_ಸಂದೇಶ = FALSE;
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
    ಪೂರ್ಣಾಂಕ
makeset(FILE *fd, ಪೂರ್ಣಾಂಕ opt_flags, ಪೂರ್ಣಾಂಕ local_only)
{
    ರಚನೆ vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = ಖಾಲಿ;	// fresh value
    ಅಕ್ಷರ		*cmd;
    ಪೂರ್ಣಾಂಕ			ಸುತ್ತು;
    ಪೂರ್ಣಾಂಕ			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    ಗಾಗಿ (pri = 1; pri >= 0; --pri)
    {
      ಗಾಗಿ (p = &options[0]; !istermoption(p); p++)
	ಆದರೆ (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    ಆದರೆ (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		ಮುಂದುವರಿ;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    ಆದರೆ ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		ಮುಂದುವರಿ;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    ಆದರೆ ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		ಮುಂದುವರಿ;

	    ಆದರೆ ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		ಮುಂದುವರಿ;

	    ಸುತ್ತು = 2;
	    ಆದರೆ (p->indir != PV_NONE)
	    {
		ಆದರೆ (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    ಆದರೆ (!(opt_flags & OPT_LOCAL))
			ಮುಂದುವರಿ;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    ಆದರೆ (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			ಆದರೆ (!optval_default(p, varp_fresh, p_cp))
			{
			    ಸುತ್ತು = 1;
			    varp_local = varp;
			    varp = varp_fresh;
			}
		    }
		}
	    }

	    // Round 1: fresh value for window-local options.
	    // Round 2: other values
	    ಗಾಗಿ ( ; ಸುತ್ತು <= 2; varp = varp_local, ++ಸುತ್ತು)
	    {
		ಆದರೆ (ಸುತ್ತು == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		ಇಲ್ಲದಿದ್ದರೆ
		    cmd = "setlocal";

		ಆದರೆ (p->flags & P_BOOL)
		{
		    ಆದರೆ (put_setbool(fd, cmd, p->fullname, *(ಪೂರ್ಣಾಂಕ *)varp) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->flags & P_ಅಂಕೆ)
		{
		    ಆದರೆ (put_setnum(fd, cmd, p->fullname, (ದೀರ್ಘ *)varp) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ    // P_STRING
		{
		    ಪೂರ್ಣಾಂಕ		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    ಆದರೆ ((p->flags & P_FUNC) && *(char_u **)varp != ಖಾಲಿ
			    && ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(*(ಅಕ್ಷರ **)varp, "<lambda>") != ಖಾಲಿ)
			ಮುಂದುವರಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    legacy = !is_option_ಮೌಲ್ಯ_vim9(p - &options[0],
			    ಸುತ್ತು == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#ಇಲ್ಲದಿದ್ದರೆ
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#ಆದರೆ_ಅಂತ್ಯ

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    ಆದರೆ (
#ಆದರೆ defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#ಆದರೆ_ಅಂತ್ಯ
			    p->indir == PV_FT)
		    {
			ಆದರೆ (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    ಹಿಂತಿರುಗಿ FAIL;
			do_endif = TRUE;
		    }
		    ಆದರೆ (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    ಆದರೆ (do_endif)
		    {
			ಆದರೆ (put_ಸಾಲು(fd, "endif") == FAIL)
			    ಹಿಂತಿರುಗಿ FAIL;
		    }
		}
	    }
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    ಪೂರ್ಣಾಂಕ
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_ಆವೃತ್ತಿ < SCRIPT_ಆವೃತ್ತಿ_VIM9;
# endif

    ಆದರೆ (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
put_setstring(
    FILE	*fd,
    bool	legacy,
    ಅಕ್ಷರ	*cmd,
    ಅಕ್ಷರ	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = ಖಾಲಿ;
    char_u	*part = ಖಾಲಿ;
    char_u	*p;

    ಆದರೆ (legacy && fprintf(fd, "legacy ") < 0)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (fprintf(fd, "%s %s=", cmd, name) < 0)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (*valuep != ಖಾಲಿ)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	ಆದರೆ (valuep == &p_pt)
	{
	    s = *valuep;
	    ಆಗ (*s != NUL)
		ಆದರೆ (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	}
	// expand the option value, replace $HOME by ~
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((flags & P_EXPAND) != 0)
	{
	    ಪೂರ್ಣಾಂಕ  ಗಾತ್ರ_2 = (ಪೂರ್ಣಾಂಕ)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(ಗಾತ್ರ_2);
	    ಆದರೆ (buf == ಖಾಲಿ)
		ಹೋಗು fail;
	    home_replace(ಖಾಲಿ, *valuep, buf, ಗಾತ್ರ_2, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    ಆದರೆ (ಗಾತ್ರ_2 >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != ಖಾಲಿ)
	    {
		part = alloc(ಗಾತ್ರ_2);
		ಆದರೆ (part == ಖಾಲಿ)
		    ಹೋಗು fail;

		// write line break to clear the option, e.g. ':set rtp='
		ಆದರೆ (put_eol(fd) == FAIL)
		    ಹೋಗು fail;

		p = buf;
		ಆಗ (*p != NUL)
		{
		    ಆದರೆ (legacy && fprintf(fd, "legacy ") < 0)
			ಹಿಂತಿರುಗಿ FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    ಆದರೆ (fprintf(fd, "%s %s+=", cmd, name) < 0)
			ಹೋಗು fail;
		    (ಶೂನ್ಯ)copy_option_part(&p, part, ಗಾತ್ರ_2,  ",");
		    ಆದರೆ (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			ಹೋಗು fail;
		}
		vim_free(buf);
		vim_free(part);
		ಹಿಂತಿರುಗಿ OK;
	    }
	    ಆದರೆ (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    vim_free(buf);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (put_escstr(fd, *valuep, 2) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (put_eol(fd) < 0)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
fail:
    vim_free(buf);
    vim_free(part);
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
put_setnum(
    FILE	*fd,
    ಅಕ್ಷರ	*cmd,
    ಅಕ್ಷರ	*name,
    ದೀರ್ಘ	*valuep)
{
    ದೀರ್ಘ	wc;

    ಆದರೆ (fprintf(fd, "%s %s=", cmd, name) < 0)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	ಆದರೆ (fputs((ಅಕ್ಷರ *)get_special_ಕೀಲಿ_name((ಪೂರ್ಣಾಂಕ)wc, 0), fd) < 0)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fprintf(fd, "%ld", *valuep) < 0)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (put_eol(fd) < 0)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
put_setbool(
    FILE	*fd,
    ಅಕ್ಷರ	*cmd,
    ಅಕ್ಷರ	*name,
    ಪೂರ್ಣಾಂಕ		ಮೌಲ್ಯ)
{
    ಆದರೆ (ಮೌಲ್ಯ < 0)	// global/local option using global value
	ಹಿಂತಿರುಗಿ OK;
    ಆದರೆ (fprintf(fd, "%s %s%s", cmd, ಮೌಲ್ಯ ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    ಶೂನ್ಯ
clear_termoptions(ಶೂನ್ಯ)
{
    // Reset a few things before clearing the old options. This may cause
    // outputting a few things that the terminal doesn't understand, but the
    // screen will be cleared later, so this is OK.
    mch_setmouse(FALSE);	    // switch mouse off
    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window titles
#ಆದರೆ defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    ಆದರೆ (gui.starting)
	clear_xterm_clip();
#ಆದರೆ_ಅಂತ್ಯ
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    ಶೂನ್ಯ
free_termoptions(ಶೂನ್ಯ)
{
    ರಚನೆ vimoption   *p;

    ಗಾಗಿ (p = options; p->fullname != ಖಾಲಿ; p++)
	ಆದರೆ (istermoption(p))
	{
	    ಆದರೆ (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    ಆದರೆ (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = ಖಾಲಿಗಳು_option;
	    p->def_val[VI_DEFAULT] = ಖಾಲಿಗಳು_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    // remember where the option was cleared
	    ಗಣ_option_sctx_idx((ಪೂರ್ಣಾಂಕ)(p - options), OPT_GLOBAL, current_sctx);
#ಆದರೆ_ಅಂತ್ಯ
	}
    clear_termcodes();
}

/*
 * Free the string for one term option, if it was allocated.
 * Set the string to empty_option and clear allocated flag.
 * "var" points to the option value.
 */
    ಶೂನ್ಯ
free_one_termoption(char_u *var)
{
    ರಚನೆ vimoption   *p;

    ಗಾಗಿ (p = &options[0]; p->fullname != ಖಾಲಿ; p++)
	ಆದರೆ (p->var == var)
	{
	    ಆದರೆ (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = ಖಾಲಿಗಳು_option;
	    p->flags &= ~P_ALLOCED;
	    ನಿಲ್ಲಿಸು;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    ಶೂನ್ಯ
ಗಣ_ಪದ_defaults(ಶೂನ್ಯ)
{
    ರಚನೆ vimoption   *p;

    ಗಾಗಿ (p = &options[0]; p->fullname != ಖಾಲಿ; p++)
    {
	ಆದರೆ (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    ಆದರೆ (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    ಆದರೆ (p->flags & P_ALLOCED)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
istermoption(ರಚನೆ vimoption *p)
{
    ಹಿಂತಿರುಗಿ (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
istermoption_idx(ಪೂರ್ಣಾಂಕ opt_idx)
{
    ಹಿಂತಿರುಗಿ istermoption(&options[opt_idx]);
}

#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    ಶೂನ್ಯ
unset_global_local_option(char_u *name, ಶೂನ್ಯ *from)
{
    ರಚನೆ vimoption *p;
    ಪೂರ್ಣಾಂಕ		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    ಆದರೆ (opt_idx < 0)
	ಹಿಂತಿರುಗಿ;
    p = &(options[opt_idx]);

    ಆಯ್ಕೆ ((ಪೂರ್ಣಾಂಕ)p->indir)
    {
	// global option with local value: use local value if it's been set
	ಸ್ಥಿತಿ PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_ಮಾರ್ಗ:
	    clear_string_option(&buf->b_p_ಮಾರ್ಗ);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_AC:
	    buf->b_p_ac = -1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_AR:
	    buf->b_p_ar = -1;
	    ನಿಲ್ಲಿಸು;
# ifdef HAVE_FSYNC
	ಸ್ಥಿತಿ PV_FS:
	    buf->b_p_fs = -1;
	    ನಿಲ್ಲಿಸು;
# endif
	ಸ್ಥಿತಿ PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_ಟ್ಯಾಗುಗಳು:
	    clear_string_option(&buf->b_p_ಟ್ಯಾಗುಗಳು);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_SISO:
	    curwin->w_p_siso = -1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_SO:
	    curwin->w_p_so = -1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_SOP:
	    curwin->w_p_sop = -1;
	    ನಿಲ್ಲಿಸು;
# ifdef FEAT_ಕಂಡುಹಿಡಿ_ID
	ಸ್ಥಿತಿ PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    ನಿಲ್ಲಿಸು;
# endif
	ಸ್ಥಿತಿ PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    ನಿಲ್ಲಿಸು;
# ifdef FEAT_DIFF
	ಸ್ಥಿತಿ PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    ನಿಲ್ಲಿಸು;
# endif
	ಸ್ಥಿತಿ PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    ನಿಲ್ಲಿಸು;
# ifdef FEAT_COMPL_FUNC
	ಸ್ಥಿತಿ PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    ನಿಲ್ಲಿಸು;
# endif
	ಸ್ಥಿತಿ PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    ನಿಲ್ಲಿಸು;
# ifdef FEAT_EVAL
	ಸ್ಥಿತಿ PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    ನಿಲ್ಲಿಸು;
# endif
# ifdef FEAT_QUICKFIX
	ಸ್ಥಿತಿ PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    ನಿಲ್ಲಿಸು;
# endif
# ಆದರೆ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	ಸ್ಥಿತಿ PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    ನಿಲ್ಲಿಸು;
# endif
# ಆದರೆ defined(FEAT_CRYPT)
	ಸ್ಥಿತಿ PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    ನಿಲ್ಲಿಸು;
# endif
# ifdef FEAT_LINEBREAK
	ಸ್ಥಿತಿ PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    ನಿಲ್ಲಿಸು;
# endif
# ifdef FEAT_STL_OPT
	ಸ್ಥಿತಿ PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    ನಿಲ್ಲಿಸು;
# endif
	ಸ್ಥಿತಿ PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    ಗಣ_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      ಖಾಲಿ, 0);
	    redraw_later(UPD_NOT_VALID);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    ಗಣ_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      ಖಾಲಿ, 0);
	    redraw_later(UPD_NOT_VALID);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    ನಿಲ್ಲಿಸು;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    ಸ್ಥಿರ char_u *
get_varp_scope(ರಚನೆ vimoption *p, ಪೂರ್ಣಾಂಕ scope)
{
    ಆದರೆ ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	ಆದರೆ (p->var == VAR_WIN)
	    ಹಿಂತಿರುಗಿ (char_u *)GLOBAL_WO(get_varp(p));
	ಹಿಂತಿರುಗಿ p->var;
    }
    ಆದರೆ ((scope & OPT_LOCAL) && ((ಪೂರ್ಣಾಂಕ)p->indir & PV_BOTH))
    {
	ಆಯ್ಕೆ ((ಪೂರ್ಣಾಂಕ)p->indir)
	{
	    ಸ್ಥಿತಿ PV_FP:   ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_fp);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
	    ಸ್ಥಿತಿ PV_FS:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_fs);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಸ್ಥಿತಿ PV_FFU: ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ffu);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	    ಸ್ಥಿತಿ PV_EFM:  ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_efm);
	    ಸ್ಥಿತಿ PV_GEFM: ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_gefm);
	    ಸ್ಥಿತಿ PV_GP:   ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_gp);
	    ಸ್ಥಿತಿ PV_MP:   ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_mp);
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ PV_EP:   ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ep);
	    ಸ್ಥಿತಿ PV_KP:   ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_kp);
	    ಸ್ಥಿತಿ PV_ಮಾರ್ಗ: ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ಮಾರ್ಗ);
	    ಸ್ಥಿತಿ PV_AC:   ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ac);
	    ಸ್ಥಿತಿ PV_AR:   ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ar);
	    ಸ್ಥಿತಿ PV_ಟ್ಯಾಗುಗಳು: ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ಟ್ಯಾಗುಗಳು);
	    ಸ್ಥಿತಿ PV_TC:   ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_tc);
	    ಸ್ಥಿತಿ PV_SISO: ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_siso);
	    ಸ್ಥಿತಿ PV_SO:   ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_so);
	    ಸ್ಥಿತಿ PV_SOP:  ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_sop);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
	    ಸ್ಥಿತಿ PV_DEF:  ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_def);
	    ಸ್ಥಿತಿ PV_INC:  ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_inc);
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ PV_COT:  ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_cot);
	    ಸ್ಥಿತಿ PV_DICT: ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_dict);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    ಸ್ಥಿತಿ PV_DIA:  ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_dia);
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ PV_TSR:  ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_tsr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
	    ಸ್ಥಿತಿ PV_TSRFU: ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_tsrfu);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    ಸ್ಥಿತಿ PV_BEXPR: ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_bexpr);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_CRYPT)
	    ಸ್ಥಿತಿ PV_CM:	  ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_cm);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	    ಸ್ಥಿತಿ PV_SBR:  ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_sbr);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
	    ಸ್ಥಿತಿ PV_STL:  ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_stl);
	    ಸ್ಥಿತಿ PV_STLO: ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_stlo);
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ PV_UL:   ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ul);
	    ಸ್ಥಿತಿ PV_LW:   ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_lw);
	    ಸ್ಥಿತಿ PV_BKC:  ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_bkc);
	    ಸ್ಥಿತಿ PV_MENC: ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_menc);
	    ಸ್ಥಿತಿ PV_LCS:  ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_lcs);
	    ಸ್ಥಿತಿ PV_FCS:  ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_fcs);
	    ಸ್ಥಿತಿ PV_VE:	  ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_ve);

	}
	ಹಿಂತಿರುಗಿ ಖಾಲಿ; // "cannot happen"
    }
    ಹಿಂತಿರುಗಿ get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(ಪೂರ್ಣಾಂಕ opt_idx, ಪೂರ್ಣಾಂಕ scope)
{
    ಹಿಂತಿರುಗಿ get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    ಸ್ಥಿರ char_u *
get_varp(ರಚನೆ vimoption *p)
{
    // hidden option, always return NULL
    ಆದರೆ (p->var == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆಯ್ಕೆ ((ಪೂರ್ಣಾಂಕ)p->indir)
    {
	ಸ್ಥಿತಿ PV_NONE:	ಹಿಂತಿರುಗಿ p->var;

	// global option with local value: use local value if it's been set
	ಸ್ಥಿತಿ PV_EP:	ಹಿಂತಿರುಗಿ *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	ಸ್ಥಿತಿ PV_KP:	ಹಿಂತಿರುಗಿ *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	ಸ್ಥಿತಿ PV_ಮಾರ್ಗ:	ಹಿಂತಿರುಗಿ *curbuf->b_p_ಮಾರ್ಗ != NUL
				    ? (char_u *)&(curbuf->b_p_ಮಾರ್ಗ) : p->var;
	ಸ್ಥಿತಿ PV_AC:	ಹಿಂತಿರುಗಿ curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	ಸ್ಥಿತಿ PV_AR:	ಹಿಂತಿರುಗಿ curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	ಸ್ಥಿತಿ PV_ಟ್ಯಾಗುಗಳು:	ಹಿಂತಿರುಗಿ *curbuf->b_p_ಟ್ಯಾಗುಗಳು != NUL
				    ? (char_u *)&(curbuf->b_p_ಟ್ಯಾಗುಗಳು) : p->var;
	ಸ್ಥಿತಿ PV_TC:	ಹಿಂತಿರುಗಿ *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	ಸ್ಥಿತಿ PV_BKC:	ಹಿಂತಿರುಗಿ *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	ಸ್ಥಿತಿ PV_SISO:	ಹಿಂತಿರುಗಿ curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	ಸ್ಥಿತಿ PV_SO:	ಹಿಂತಿರುಗಿ curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	ಸ್ಥಿತಿ PV_SOP:	ಹಿಂತಿರುಗಿ curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
	ಸ್ಥಿತಿ PV_DEF:	ಹಿಂತಿರುಗಿ *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	ಸ್ಥಿತಿ PV_INC:	ಹಿಂತಿರುಗಿ *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_COT:	ಹಿಂತಿರುಗಿ *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	ಸ್ಥಿತಿ PV_DICT:	ಹಿಂತಿರುಗಿ *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	ಸ್ಥಿತಿ PV_DIA:	ಹಿಂತಿರುಗಿ *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_TSR:	ಹಿಂತಿರುಗಿ *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
	ಸ್ಥಿತಿ PV_TSRFU:	ಹಿಂತಿರುಗಿ *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_FP:	ಹಿಂತಿರುಗಿ *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
	ಸ್ಥಿತಿ PV_FS:	ಹಿಂತಿರುಗಿ curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಸ್ಥಿತಿ PV_FFU:	ಹಿಂತಿರುಗಿ *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	ಸ್ಥಿತಿ PV_EFM:	ಹಿಂತಿರುಗಿ *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	ಸ್ಥಿತಿ PV_GEFM:	ಹಿಂತಿರುಗಿ *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	ಸ್ಥಿತಿ PV_GP:	ಹಿಂತಿರುಗಿ *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	ಸ್ಥಿತಿ PV_MP:	ಹಿಂತಿರುಗಿ *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	ಸ್ಥಿತಿ PV_BEXPR:	ಹಿಂತಿರುಗಿ *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_CRYPT)
	ಸ್ಥಿತಿ PV_CM:	ಹಿಂತಿರುಗಿ *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	ಸ್ಥಿತಿ PV_SBR:	ಹಿಂತಿರುಗಿ *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
	ಸ್ಥಿತಿ PV_STL:	ಹಿಂತಿರುಗಿ *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	ಸ್ಥಿತಿ PV_STLO:	ಹಿಂತಿರುಗಿ *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_UL:	ಹಿಂತಿರುಗಿ curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	ಸ್ಥಿತಿ PV_LW:	ಹಿಂತಿರುಗಿ *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	ಸ್ಥಿತಿ PV_MENC:	ಹಿಂತಿರುಗಿ *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
	ಸ್ಥಿತಿ PV_ARAB:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_arab);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_LIST:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_list);
	ಸ್ಥಿತಿ PV_LCS:	ಹಿಂತಿರುಗಿ *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	ಸ್ಥಿತಿ PV_FCS:	ಹಿಂತಿರುಗಿ *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	ಸ್ಥಿತಿ PV_VE:	ಹಿಂತಿರುಗಿ *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	ಸ್ಥಿತಿ PV_SPELL:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_spell);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಸ್ಥಿತಿ PV_CUC:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_cuc);
	ಸ್ಥಿತಿ PV_CUL:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_cul);
	ಸ್ಥಿತಿ PV_CULOPT:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_culopt);
	ಸ್ಥಿತಿ PV_CC:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_cc);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	ಸ್ಥಿತಿ PV_DIFF:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_diff);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_EIW:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_eiw);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	ಸ್ಥಿತಿ PV_FDC:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_fdc);
	ಸ್ಥಿತಿ PV_FEN:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_fen);
	ಸ್ಥಿತಿ PV_FDI:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_fdi);
	ಸ್ಥಿತಿ PV_FDL:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_fdl);
	ಸ್ಥಿತಿ PV_FDM:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_fdm);
	ಸ್ಥಿತಿ PV_FML:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_fml);
	ಸ್ಥಿತಿ PV_FDN:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	ಸ್ಥಿತಿ PV_FDE:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_fde);
	ಸ್ಥಿತಿ PV_FDT:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_fdt);
# endif
	ಸ್ಥಿತಿ PV_FMR:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_fmr);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_NU:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_nu);
	ಸ್ಥಿತಿ PV_RNU:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_rnu);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	ಸ್ಥಿತಿ PV_NUW:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_nuw);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_WFB:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_wfb);
	ಸ್ಥಿತಿ PV_WFH:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_wfh);
	ಸ್ಥಿತಿ PV_WFW:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_wfw);
#ಆದರೆ defined(FEAT_QUICKFIX)
	ಸ್ಥಿತಿ PV_PVW:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_pvw);
	ಸ್ಥಿತಿ PV_LHI:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_lhi);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಸ್ಥಿತಿ PV_RL:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_rl);
	ಸ್ಥಿತಿ PV_RLC:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_rlc);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_SCROLL:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_scr);
	ಸ್ಥಿತಿ PV_SMS:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_sms);
	ಸ್ಥಿತಿ PV_WRAP:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_wrap);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	ಸ್ಥಿತಿ PV_LBR:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_lbr);
	ಸ್ಥಿತಿ PV_BRI:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_bri);
	ಸ್ಥಿತಿ PV_BRIOPT: ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_briopt);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_WCR:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_wcr);
	ಸ್ಥಿತಿ PV_SCBIND: ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_scb);
	ಸ್ಥಿತಿ PV_CRBIND: ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_crb);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	ಸ್ಥಿತಿ PV_COCU:   ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_cocu);
	ಸ್ಥಿತಿ PV_COLE:   ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_cole);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_WHL:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_whl);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	ಸ್ಥಿತಿ PV_TWK:    ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_twk);
	ಸ್ಥಿತಿ PV_TWS:    ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_tws);
	ಸ್ಥಿತಿ PV_TWSL:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_twsl);
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ PV_AI:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ai);
	ಸ್ಥಿತಿ PV_BIN:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_bin);
	ಸ್ಥಿತಿ PV_BOMB:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_bomb);
	ಸ್ಥಿತಿ PV_BH:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_bh);
	ಸ್ಥಿತಿ PV_BT:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_bt);
	ಸ್ಥಿತಿ PV_BL:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_bl);
	ಸ್ಥಿತಿ PV_CI:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ci);
	ಸ್ಥಿತಿ PV_CIN:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_cin);
	ಸ್ಥಿತಿ PV_CINK:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_cink);
	ಸ್ಥಿತಿ PV_CINO:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_cino);
	ಸ್ಥಿತಿ PV_CINSD:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_cinsd);
	ಸ್ಥಿತಿ PV_CINW:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_cinw);
	ಸ್ಥಿತಿ PV_COM:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_com);
	ಸ್ಥಿತಿ PV_CMS:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_cms);
	ಸ್ಥಿತಿ PV_CPT:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_cpt);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	ಸ್ಥಿತಿ PV_CSL:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_csl);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
	ಸ್ಥಿತಿ PV_CFU:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_cfu);
	ಸ್ಥಿತಿ PV_OFU:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ofu);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಸ್ಥಿತಿ PV_TFU:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_tfu);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_EOF:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_eof);
	ಸ್ಥಿತಿ PV_EOL:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_eol);
	ಸ್ಥಿತಿ PV_FIXEOL:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_fixeol);
	ಸ್ಥಿತಿ PV_ET:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_et);
	ಸ್ಥಿತಿ PV_FENC:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_fenc);
	ಸ್ಥಿತಿ PV_FF:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ff);
	ಸ್ಥಿತಿ PV_FT:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ft);
	ಸ್ಥಿತಿ PV_FO:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_fo);
	ಸ್ಥಿತಿ PV_FLP:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_flp);
	ಸ್ಥಿತಿ PV_IMI:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_iminsert);
	ಸ್ಥಿತಿ PV_IMS:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_imsearch);
	ಸ್ಥಿತಿ PV_INF:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_inf);
	ಸ್ಥಿತಿ PV_ISK:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_isk);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
# ifdef FEAT_EVAL
	ಸ್ಥಿತಿ PV_INEX:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_inex);
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL)
	ಸ್ಥಿತಿ PV_INDE:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_inde);
	ಸ್ಥಿತಿ PV_INDK:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_indk);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಸ್ಥಿತಿ PV_FEX:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_fex);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	ಸ್ಥಿತಿ PV_ಕೀಲಿ:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ಕೀಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_LISP:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_lisp);
	ಸ್ಥಿತಿ PV_LOP:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_lop);
	ಸ್ಥಿತಿ PV_ML:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ml);
	ಸ್ಥಿತಿ PV_MPS:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_mps);
	ಸ್ಥಿತಿ PV_MA:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ma);
	ಸ್ಥಿತಿ PV_MOD:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_ಬದಲಾಯಿತು);
	ಸ್ಥಿತಿ PV_NF:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_nf);
	ಸ್ಥಿತಿ PV_PI:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_pi);
	ಸ್ಥಿತಿ PV_QE:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_qe);
	ಸ್ಥಿತಿ PV_RO:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ro);
	ಸ್ಥಿತಿ PV_SI:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_si);
	ಸ್ಥಿತಿ PV_SN:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_sn);
	ಸ್ಥಿತಿ PV_STS:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_sts);
	ಸ್ಥಿತಿ PV_SUA:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_sua);
	ಸ್ಥಿತಿ PV_SWF:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_swf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಸ್ಥಿತಿ PV_SMC:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_smc);
	ಸ್ಥಿತಿ PV_SYN:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_syn);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	ಸ್ಥಿತಿ PV_SPC:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_s->b_p_spc);
	ಸ್ಥಿತಿ PV_SPF:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_s->b_p_spf);
	ಸ್ಥಿತಿ PV_SPL:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_s->b_p_spl);
	ಸ್ಥಿತಿ PV_SPO:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_s->b_p_spo);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_SW:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_sw);
	ಸ್ಥಿತಿ PV_TS:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_ts);
	ಸ್ಥಿತಿ PV_TW:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_tw);
	ಸ್ಥಿತಿ PV_TX:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_tx);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
	ಸ್ಥಿತಿ PV_UDF:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_udf);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ PV_WM:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_wm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
	ಸ್ಥಿತಿ PV_KMAP:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_keymap);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	ಸ್ಥಿತಿ PV_SCL:	ಹಿಂತಿರುಗಿ (char_u *)&(curwin->w_p_scl);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	ಸ್ಥಿತಿ PV_VSTS:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_vsts);
	ಸ್ಥಿತಿ PV_VTS:	ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_vts);
#ಆದರೆ_ಅಂತ್ಯ
	ಪೂರ್ವನಿಯೋಜಿತ:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    ಹಿಂತಿರುಗಿ (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(ಪೂರ್ಣಾಂಕ opt_idx)
{
    ಹಿಂತಿರುಗಿ options[opt_idx].var;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(ಪೂರ್ಣಾಂಕ opt_idx)
{
    ಹಿಂತಿರುಗಿ (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    ಪೂರ್ಣಾಂಕ
option_info(char_u *name, dict_T *d)
{
    ಪೂರ್ಣಾಂಕ			opt_idx;
    ರಚನೆ vimoption	*p;
    ಪೂರ್ಣಾಂಕ			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    ಆದರೆ ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    ಆದರೆ (opt_idx < 0)
	ಹಿಂತಿರುಗಿ FAIL;

    p = &options[opt_idx];
    dict_ಕೂಡಿಸು_string(d, "name", (char_u *)p->fullname);
    dict_ಕೂಡಿಸು_string(d, "shortname",
		    (char_u *)(p->shortname == ಖಾಲಿ ? "" : p->shortname));
    dict_ಕೂಡಿಸು_bool(d, "available", p->var != ಖಾಲಿ);
    dict_ಕೂಡಿಸು_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_ಅಂಕೆ) ? "number" : "string"));
    dict_ಕೂಡಿಸು_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    ಆದರೆ (p->flags & P_BOOL)
	dict_ಕೂಡಿಸು_bool(d, "default", (ಪೂರ್ಣಾಂಕ)(long_i)p->def_val[dvi]);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->flags & P_ಅಂಕೆ)
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "default", (ದೀರ್ಘ)(long_i)p->def_val[dvi]);
    ಇಲ್ಲದಿದ್ದರೆ
	dict_ಕೂಡಿಸು_string(d, "default", p->def_val[dvi] == ಖಾಲಿ
					? (char_u *)"" : p->def_val[dvi]);
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_ಗಣ_cb_T
get_option_did_ಗಣ_cb(ಪೂರ್ಣಾಂಕ opt_idx)
{
    ಹಿಂತಿರುಗಿ options[opt_idx].opt_did_ಗಣ_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(ಶೂನ್ಯ)
{
    ಆದರೆ (*curbuf->b_p_ep == NUL)
	ಹಿಂತಿರುಗಿ p_ep;
    ಹಿಂತಿರುಗಿ curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (*curbuf->b_p_ffu == NUL)
	ಹಿಂತಿರುಗಿ p_ffu;
    ಹಿಂತಿರುಗಿ curbuf->b_p_ffu;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ (char_u *)"";
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    ಶೂನ್ಯ
win_copy_options(win_T *wp_from, win_T *wp_ಗಮ್ಯ)
{
    copy_winopt(&wp_from->w_onebuf_opt, &wp_ಗಮ್ಯ->w_onebuf_opt);
    copy_winopt(&wp_from->w_allbuf_opt, &wp_ಗಮ್ಯ->w_allbuf_opt);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    // w_stl_rendered_height is in win_T directly, not in winvar_T, so it is
    // not copied by copy_winopt().  Copy it explicitly here.
    wp_ಗಮ್ಯ->w_stl_rendered_ಎತ್ತರ = wp_from->w_stl_rendered_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ
    after_copy_winopt(wp_ಗಮ್ಯ);
}

/*
 * After copying window options: update variables depending on options.
 */
    ಶೂನ್ಯ
after_copy_winopt(win_T *wp)
{
    ಅಕ್ಷರ *errmsg = update_winhighlight(wp, wp->w_p_whl);

    ಆದರೆ (errmsg != ಖಾಲಿ)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    ಆದರೆ (wp->w_p_wrap)
	wp->w_leftcol = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	wp->w_skipcol = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    briopt_check(ಖಾಲಿ, wp);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    fill_culopt_flags(ಖಾಲಿ, wp);
    check_colorcolumn(ಖಾಲಿ, wp);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ_listchars_option(wp, wp->w_p_lcs, TRUE, ಖಾಲಿ, 0);
    ಗಣ_fillchars_option(wp, wp->w_p_fcs, TRUE, ಖಾಲಿ, 0);
}

    ಸ್ಥಿರ char_u *
copy_option_val(char_u *val)
{
    ಆದರೆ (val == ಖಾಲಿಗಳು_option)
	ಹಿಂತಿರುಗಿ ಖಾಲಿಗಳು_option;  // no need to allocate memory
    ಹಿಂತಿರುಗಿ vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    ಶೂನ್ಯ
copy_winopt(winopt_T *from, winopt_T *ಗಮ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
    ಗಮ್ಯ->wo_arab = from->wo_arab;
#ಆದರೆ_ಅಂತ್ಯ
    ಗಮ್ಯ->wo_list = from->wo_list;
    ಗಮ್ಯ->wo_lcs = copy_option_val(from->wo_lcs);
    ಗಮ್ಯ->wo_fcs = copy_option_val(from->wo_fcs);
    ಗಮ್ಯ->wo_nu = from->wo_nu;
    ಗಮ್ಯ->wo_rnu = from->wo_rnu;
    ಗಮ್ಯ->wo_ve = copy_option_val(from->wo_ve);
    ಗಮ್ಯ->wo_ve_flags = from->wo_ve_flags;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಗಮ್ಯ->wo_nuw = from->wo_nuw;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಗಮ್ಯ->wo_rl  = from->wo_rl;
    ಗಮ್ಯ->wo_rlc = copy_option_val(from->wo_rlc);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಗಮ್ಯ->wo_sbr = copy_option_val(from->wo_sbr);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    ಗಮ್ಯ->wo_stl = copy_option_val(from->wo_stl);
    ಗಮ್ಯ->wo_stlo = copy_option_val(from->wo_stlo);
    ಗಮ್ಯ->wo_stlo_fh = from->wo_stlo_fh;
    ಗಮ್ಯ->wo_stlo_mh = from->wo_stlo_mh;
#ಆದರೆ_ಅಂತ್ಯ
    ಗಮ್ಯ->wo_wrap = from->wo_wrap;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಗಮ್ಯ->wo_wrap_save = from->wo_wrap_save;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಗಮ್ಯ->wo_lbr = from->wo_lbr;
    ಗಮ್ಯ->wo_bri = from->wo_bri;
    ಗಮ್ಯ->wo_briopt = copy_option_val(from->wo_briopt);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಮ್ಯ->wo_wcr = copy_option_val(from->wo_wcr);
    ಗಮ್ಯ->wo_scb = from->wo_scb;
    ಗಮ್ಯ->wo_scb_save = from->wo_scb_save;
    ಗಮ್ಯ->wo_sms = from->wo_sms;
    ಗಮ್ಯ->wo_crb = from->wo_crb;
    ಗಮ್ಯ->wo_crb_save = from->wo_crb_save;
    ಗಮ್ಯ->wo_siso = from->wo_siso;
    ಗಮ್ಯ->wo_so = from->wo_so;
    ಗಮ್ಯ->wo_sop = from->wo_sop;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    ಗಮ್ಯ->wo_spell = from->wo_spell;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಗಮ್ಯ->wo_cuc = from->wo_cuc;
    ಗಮ್ಯ->wo_cul = from->wo_cul;
    ಗಮ್ಯ->wo_culopt = copy_option_val(from->wo_culopt);
    ಗಮ್ಯ->wo_cc = copy_option_val(from->wo_cc);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಗಮ್ಯ->wo_diff = from->wo_diff;
    ಗಮ್ಯ->wo_diff_saved = from->wo_diff_saved;
#ಆದರೆ_ಅಂತ್ಯ
    ಗಮ್ಯ->wo_eiw = copy_option_val(from->wo_eiw);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಗಮ್ಯ->wo_cocu = copy_option_val(from->wo_cocu);
    ಗಮ್ಯ->wo_cole = from->wo_cole;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಗಮ್ಯ->wo_twk = copy_option_val(from->wo_twk);
    ಗಮ್ಯ->wo_tws = copy_option_val(from->wo_tws);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಗಮ್ಯ->wo_fdc = from->wo_fdc;
    ಗಮ್ಯ->wo_fdc_save = from->wo_fdc_save;
    ಗಮ್ಯ->wo_fen = from->wo_fen;
    ಗಮ್ಯ->wo_fen_save = from->wo_fen_save;
    ಗಮ್ಯ->wo_fdi = copy_option_val(from->wo_fdi);
    ಗಮ್ಯ->wo_fml = from->wo_fml;
    ಗಮ್ಯ->wo_fdl = from->wo_fdl;
    ಗಮ್ಯ->wo_fdl_save = from->wo_fdl_save;
    ಗಮ್ಯ->wo_fdm = copy_option_val(from->wo_fdm);
    ಗಮ್ಯ->wo_fdm_save = from->wo_diff_saved
			       ? vim_strsave(from->wo_fdm_save) : ಖಾಲಿಗಳು_option;
    ಗಮ್ಯ->wo_fdn = from->wo_fdn;
# ifdef FEAT_EVAL
    ಗಮ್ಯ->wo_fde = copy_option_val(from->wo_fde);
    ಗಮ್ಯ->wo_fdt = copy_option_val(from->wo_fdt);
# endif
    ಗಮ್ಯ->wo_fmr = copy_option_val(from->wo_fmr);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    ಗಮ್ಯ->wo_scl = copy_option_val(from->wo_scl);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    ಗಮ್ಯ->wo_lhi = from->wo_lhi;
#ಆದರೆ_ಅಂತ್ಯ

    ಗಮ್ಯ->wo_wrap_flags = from->wo_wrap_flags;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    ಗಮ್ಯ->wo_stl_flags = from->wo_stl_flags;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಗಮ್ಯ->wo_fde_flags = from->wo_fde_flags;
    ಗಮ್ಯ->wo_fdt_flags = from->wo_fdt_flags;
#ಆದರೆ_ಅಂತ್ಯ
    ಗಮ್ಯ->wo_whl = copy_option_val(from->wo_whl);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Copy the script context so that we know where the value was last set.
    mch_memmove(ಗಮ್ಯ->wo_script_ctx, from->wo_script_ctx,
						    ಗಾತ್ರ(ಗಮ್ಯ->wo_script_ctx));
#ಆದರೆ_ಅಂತ್ಯ
    check_winopt(ಗಮ್ಯ);		// don't want NULL pointers
}

/*
 * Check string options in a window for a NULL value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_win_options(win_T *win)
{
    check_winopt(&win->w_onebuf_opt);
    check_winopt(&win->w_allbuf_opt);
}

/*
 * Check for NULL pointers in a winopt_T and replace them with empty_option.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_winopt(winopt_T *wop ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    check_string_option(&wop->wo_fdi);
    check_string_option(&wop->wo_fdm);
    check_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    check_string_option(&wop->wo_fde);
    check_string_option(&wop->wo_fdt);
# endif
    check_string_option(&wop->wo_fmr);
#ಆದರೆ_ಅಂತ್ಯ
    check_string_option(&wop->wo_eiw);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    check_string_option(&wop->wo_scl);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    check_string_option(&wop->wo_rlc);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    check_string_option(&wop->wo_sbr);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    check_string_option(&wop->wo_stl);
    check_string_option(&wop->wo_stlo);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    check_string_option(&wop->wo_culopt);
    check_string_option(&wop->wo_cc);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    check_string_option(&wop->wo_cocu);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    check_string_option(&wop->wo_twk);
    check_string_option(&wop->wo_tws);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    check_string_option(&wop->wo_briopt);
#ಆದರೆ_ಅಂತ್ಯ
    check_string_option(&wop->wo_wcr);
    check_string_option(&wop->wo_lcs);
    check_string_option(&wop->wo_fcs);
    check_string_option(&wop->wo_ve);
    check_string_option(&wop->wo_whl);
}

/*
 * Free the allocated memory inside a winopt_T.
 */
    ಶೂನ್ಯ
clear_winopt(winopt_T *wop ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    clear_string_option(&wop->wo_fdi);
    clear_string_option(&wop->wo_fdm);
    clear_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    clear_string_option(&wop->wo_fde);
    clear_string_option(&wop->wo_fdt);
# endif
    clear_string_option(&wop->wo_fmr);
#ಆದರೆ_ಅಂತ್ಯ
    clear_string_option(&wop->wo_eiw);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    clear_string_option(&wop->wo_scl);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    clear_string_option(&wop->wo_briopt);
#ಆದರೆ_ಅಂತ್ಯ
    clear_string_option(&wop->wo_wcr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    clear_string_option(&wop->wo_rlc);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    clear_string_option(&wop->wo_sbr);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    clear_string_option(&wop->wo_stl);
    clear_string_option(&wop->wo_stlo);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    clear_string_option(&wop->wo_culopt);
    clear_string_option(&wop->wo_cc);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    clear_string_option(&wop->wo_cocu);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    clear_string_option(&wop->wo_twk);
    clear_string_option(&wop->wo_tws);
#ಆದರೆ_ಅಂತ್ಯ
    clear_string_option(&wop->wo_lcs);
    clear_string_option(&wop->wo_fcs);
    clear_string_option(&wop->wo_ve);
    clear_string_option(&wop->wo_whl);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
// Index into the options table for a buffer-local option enum.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ buf_opt_idx[BV_ಎಣಿಕೆ];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_buf_opt_idx(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ did_init_buf_opt_idx = FALSE;
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (did_init_buf_opt_idx)
	ಹಿಂತಿರುಗಿ;
    did_init_buf_opt_idx = TRUE;
    ಗಾಗಿ (i = 0; !istermoption_idx(i); i++)
	ಆದರೆ (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_ಮುಖವಾಡ] = i;
}
#ಇಲ್ಲದಿದ್ದರೆ
# define COPY_OPT_SCTX(buf, bv)
# define COPY_OPT_INSECURE(flagsfield, bv)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Copy global option values to local options for one buffer.
 * Used when creating a new buffer and sometimes when entering a buffer.
 * flags:
 * BCO_ENTER	We will enter the buffer "buf".
 * BCO_ALWAYS	Always copy the options, but only set b_p_initialized when
 *		appropriate.
 * BCO_NOHELP	Don't copy the values to a help buffer.
 */
    ಶೂನ್ಯ
buf_copy_options(buf_T *buf, ಪೂರ್ಣಾಂಕ flags)
{
    ಪೂರ್ಣಾಂಕ		should_copy = TRUE;
    char_u	*save_p_isk = ಖಾಲಿ;	    // init for GCC
    ಪೂರ್ಣಾಂಕ		dont_do_help;
    ಪೂರ್ಣಾಂಕ		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    ಆದರೆ (p_cpo != ಖಾಲಿ)
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
	ಆದರೆ ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == ಖಾಲಿ || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != ಖಾಲಿ)))
	    should_copy = FALSE;

	ಆದರೆ (should_copy || (flags & BCO_ALWAYS))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    CLEAR_ಕ್ಷೇತ್ರ(buf->b_p_script_ctx);
	    init_buf_opt_idx();
#ಆದರೆ_ಅಂತ್ಯ
	    // Don't copy the options specific to a help buffer when
	    // BCO_NOHELP is given or the options were initialized already
	    // (jumping back to a help file with CTRL-T or CTRL-O)
	    dont_do_help = ((flags & BCO_NOHELP) && buf->b_help)
						       || buf->b_p_initialized;
	    ಆದರೆ (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = ಖಾಲಿ;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    ಆದರೆ (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		ಆಯ್ಕೆ (*p_ffs)
		{
		    ಸ್ಥಿತಿ 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); ನಿಲ್ಲಿಸು;
		    ಪೂರ್ವನಿಯೋಜಿತ:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		ಆದರೆ (buf->b_p_ff != ಖಾಲಿ)
		    buf->b_ಆರಂಭ_ffc = *buf->b_p_ff;
		buf->b_p_bh = ಖಾಲಿಗಳು_option;
		buf->b_p_bt = ಖಾಲಿಗಳು_option;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
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
	    ಆದರೆ (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		buf->b_p_swf = p_swf;
		COPY_OPT_SCTX(buf, BV_SWF);
	    }
	    buf->b_p_cpt = vim_strsave(p_cpt);
	    COPY_OPT_SCTX(buf, BV_CPT);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
	    COPY_OPT_INSECURE(buf->b_p_cpt_flags, BV_CPT);
	    ಗಣ_buflocal_cpt_callbacks(buf);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	    buf->b_p_csl = vim_strsave(p_csl);
	    COPY_OPT_SCTX(buf, BV_CSL);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
	    buf->b_p_cfu = vim_strsave(p_cfu);
	    COPY_OPT_SCTX(buf, BV_CFU);
	    ಗಣ_buflocal_cfu_callback(buf);
	    buf->b_p_ofu = vim_strsave(p_ofu);
	    COPY_OPT_SCTX(buf, BV_OFU);
	    ಗಣ_buflocal_ofu_callback(buf);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    buf->b_p_tfu = vim_strsave(p_tfu);
	    COPY_OPT_SCTX(buf, BV_TFU);
	    ಗಣ_buflocal_tfu_callback(buf);
#ಆದರೆ_ಅಂತ್ಯ
	    buf->b_p_sts = p_sts;
	    COPY_OPT_SCTX(buf, BV_STS);
	    buf->b_p_sts_nopaste = p_sts_nopaste;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	    buf->b_p_vsts = vim_strsave(p_vsts);
	    COPY_OPT_SCTX(buf, BV_VSTS);
	    ಆದರೆ (p_vsts && p_vsts != ಖಾಲಿಗಳು_option)
		(ಶೂನ್ಯ)tabstop_ಗಣ(p_vsts, &buf->b_p_vsts_array);
	    ಇಲ್ಲದಿದ್ದರೆ
		buf->b_p_vsts_array = ಖಾಲಿ;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
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
	    buf->b_p_ft = ಖಾಲಿಗಳು_option;
	    buf->b_p_pi = p_pi;
	    COPY_OPT_SCTX(buf, BV_PI);
	    buf->b_p_cinw = vim_strsave(p_cinw);
	    COPY_OPT_SCTX(buf, BV_CINW);
	    buf->b_p_lisp = p_lisp;
	    COPY_OPT_SCTX(buf, BV_LISP);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    // Don't copy 'syntax', it must be set
	    buf->b_p_syn = ಖಾಲಿಗಳು_option;
	    buf->b_p_smc = p_smc;
	    COPY_OPT_SCTX(buf, BV_SMC);
	    buf->b_s.b_syn_isk = ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	    buf->b_s.b_p_spc = vim_strsave(p_spc);
	    COPY_OPT_SCTX(buf, BV_SPC);
	    (ಶೂನ್ಯ)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#ಆದರೆ_ಅಂತ್ಯ
	    buf->b_p_fp = ಖಾಲಿಗಳು_option;
#ಆದರೆ defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	    buf->b_p_ಕೀಲಿ = vim_strsave(p_ಕೀಲಿ);
	    COPY_OPT_SCTX(buf, BV_ಕೀಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
	    buf->b_p_sua = vim_strsave(p_sua);
	    COPY_OPT_SCTX(buf, BV_SUA);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
	    buf->b_p_keymap = vim_strsave(p_keymap);
	    COPY_OPT_SCTX(buf, BV_KMAP);
	    buf->b_kmap_state |= KEYMAP_INIT;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    buf->b_p_twsl = p_twsl;
	    COPY_OPT_SCTX(buf, BV_TWSL);
#ಆದರೆ_ಅಂತ್ಯ
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
	    buf->b_p_fs = -1;
#ಆದರೆ_ಅಂತ್ಯ
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    buf->b_p_bkc = ಖಾಲಿಗಳು_option;
	    buf->b_bkc_flags = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	    buf->b_p_gefm = ಖಾಲಿಗಳು_option;
	    buf->b_p_gp = ಖಾಲಿಗಳು_option;
	    buf->b_p_mp = ಖಾಲಿಗಳು_option;
	    buf->b_p_efm = ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ
	    buf->b_p_ep = ಖಾಲಿಗಳು_option;
#ಆದರೆ defined(FEAT_EVAL)
	    buf->b_p_ffu = ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ
	    buf->b_p_kp = ಖಾಲಿಗಳು_option;
	    buf->b_p_ಮಾರ್ಗ = ಖಾಲಿಗಳು_option;
	    buf->b_p_ಟ್ಯಾಗುಗಳು = ಖಾಲಿಗಳು_option;
	    buf->b_p_tc = ಖಾಲಿಗಳು_option;
	    buf->b_tc_flags = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
	    buf->b_p_def = ಖಾಲಿಗಳು_option;
	    buf->b_p_inc = ಖಾಲಿಗಳು_option;
# ifdef FEAT_EVAL
	    buf->b_p_inex = vim_strsave(p_inex);
	    COPY_OPT_SCTX(buf, BV_INEX);
	    COPY_OPT_INSECURE(buf->b_p_inex_flags, BV_INEX);
# endif
#ಆದರೆ_ಅಂತ್ಯ
	    buf->b_p_cot = ಖಾಲಿಗಳು_option;
	    buf->b_cot_flags = 0;
	    buf->b_p_dict = ಖಾಲಿಗಳು_option;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    buf->b_p_dia = ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ
	    buf->b_p_tsr = ಖಾಲಿಗಳು_option;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
	    buf->b_p_tsrfu = ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ
	    buf->b_p_qe = vim_strsave(p_qe);
	    COPY_OPT_SCTX(buf, BV_QE);
#ಆದರೆ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_CRYPT)
	    buf->b_p_cm = ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
	    buf->b_p_udf = p_udf;
	    COPY_OPT_SCTX(buf, BV_UDF);
#ಆದರೆ_ಅಂತ್ಯ
	    buf->b_p_lw = ಖಾಲಿಗಳು_option;
	    buf->b_p_menc = ಖಾಲಿಗಳು_option;

	    // Don't copy the options set by ex_help(), use the saved values,
	    // when going from a help buffer to a non-help buffer.
	    // Don't touch these at all when BCO_NOHELP is used and going from
	    // or to a help buffer.
	    ಆದರೆ (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		ಆದರೆ (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (ಶೂನ್ಯ)tabstop_ಗಣ(p_vts, &buf->b_p_vts_array);
		ಇಲ್ಲದಿದ್ದರೆ
		    buf->b_p_vts_array = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		ಆದರೆ (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (ಶೂನ್ಯ)tabstop_ಗಣ(p_vts, &buf->b_p_vts_array);
		ಇಲ್ಲದಿದ್ದರೆ
		    buf->b_p_vts_array = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
		buf->b_help = false;
		ಆದರೆ (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	ಆದರೆ (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    ಆದರೆ (did_isk)
	(ಶೂನ್ಯ)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    ಶೂನ್ಯ
reset_modifiable(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    ಆದರೆ (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}

/*
 * Set the global value for 'iminsert' to the local value.
 */
    ಶೂನ್ಯ
ಗಣ_iminsert_global(ಶೂನ್ಯ)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    ಶೂನ್ಯ
ಗಣ_imsearch_global(ಶೂನ್ಯ)
{
    p_imsearch = curbuf->b_p_imsearch;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ expand_option_idx = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ expand_option_ಆರಂಭ_ಲಂಬಸಾಲು = 0;
ಸ್ಥಿರ char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ expand_option_flags = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ expand_option_append = FALSE;

    ಶೂನ್ಯ
ಗಣ_context_ಒಳ_ಗಣ_cmd(
    expand_T	*xp,
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    ಪೂರ್ಣಾಂಕ		nextchar;
    long_u	flags = 0;	// init for GCC
    ಪೂರ್ಣಾಂಕ		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		is_ಪದ_option = FALSE;
    ಪೂರ್ಣಾಂಕ		ಕೀಲಿ;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    ಆದರೆ (*arg == NUL)
    {
	xp->xp_ನಮೂನೆ = arg;
	ಹಿಂತಿರುಗಿ;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    ಆದರೆ (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_ನಮೂನೆ = p + 1;
	ಹಿಂತಿರುಗಿ;
    }
    ಆಗ (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	ಆದರೆ (*p == ' ' || *p == ',')
	{
	    ಆಗ (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	ಆದರೆ (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    ನಿಲ್ಲಿಸು;
	}
	--p;
    }
    ಆದರೆ (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_ಪೂರ್ವಪ್ರತ್ಯಯ = XP_ಪೂರ್ವಪ್ರತ್ಯಯ_NO;
	p += 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_ಪೂರ್ವಪ್ರತ್ಯಯ = XP_ಪೂರ್ವಪ್ರತ್ಯಯ_INV;
	p += 3;
    }
    xp->xp_ನಮೂನೆ = arg = p;
    ಆದರೆ (*arg == '<')
    {
	ಆಗ (*p != '>')
	    ಆದರೆ (*p++ == NUL)	    // expand terminal option name
		ಹಿಂತಿರುಗಿ;
	ಕೀಲಿ = get_special_ಕೀಲಿ_ಸಂಕೇತ(arg + 1);
	ಆದರೆ (ಕೀಲಿ == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    ಹಿಂತಿರುಗಿ;
	}
	nextchar = *++p;
	is_ಪದ_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(ಕೀಲಿ);
	expand_option_name[3] = KEY2TERMCAP1(ಕೀಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    ಆದರೆ (*p != NUL)
		++p;
	    ಆದರೆ (*p == NUL)
		ಹಿಂತಿರುಗಿ;		// expand option name
	    nextchar = *++p;
	    is_ಪದ_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Allow * wildcard
	    ಆಗ (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    ಆದರೆ (*p == NUL)
		ಹಿಂತಿರುಗಿ;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    ಆದರೆ (opt_idx == -1 || options[opt_idx].var == ಖಾಲಿ)
	    {
		xp->xp_context = EXPAND_NOTHING;
		ಹಿಂತಿರುಗಿ;
	    }
	    flags = options[opt_idx].flags;
	    ಆದರೆ (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		ಹಿಂತಿರುಗಿ;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    ಪೂರ್ಣಾಂಕ expand_option_ಕಳೆ = FALSE;
    ಆದರೆ ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	ಆದರೆ (nextchar == '-')
	    expand_option_ಕಳೆ = TRUE;
	ಆದರೆ (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    ಆದರೆ ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ಹಿಂತಿರುಗಿ;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    ಆದರೆ (is_ಪದ_option)
	expand_option_idx = -1;
    ಇಲ್ಲದಿದ್ದರೆ
	expand_option_idx = opt_idx;

    ಆದರೆ (!is_ಪದ_option)
    {
	ಆದರೆ (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    ಹಿಂತಿರುಗಿ;
	}
    }

    xp->xp_ನಮೂನೆ = p + 1;
    expand_option_ಆರಂಭ_ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(p + 1 - xp->xp_ಸಾಲು);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಆದರೆ (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	ಹಿಂತಿರುಗಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
    ಆದರೆ (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    ಆದರೆ (expand_option_ಕಳೆ)
    {
	xp->xp_context = EXPAND_SETTING_ಕಳೆ;
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != ಖಾಲಿ)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*xp->xp_ನಮೂನೆ == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	xp->xp_context = EXPAND_NOTHING;

    ಆದರೆ (is_ಪದ_option || (flags & P_ಅಂಕೆ))
	ಹಿಂತಿರುಗಿ;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    ಆದರೆ (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	ಆದರೆ (p == (char_u *)&p_bdir
		|| p == (char_u *)&p_dir
		|| p == (char_u *)&p_ಮಾರ್ಗ
		|| p == (char_u *)&p_pp
		|| p == (char_u *)&p_rtp
		|| p == (char_u *)&p_cdpath
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
		|| p == (char_u *)&p_vdir
#ಆದರೆ_ಅಂತ್ಯ
		)
	{
	    xp->xp_context = EXPAND_DIRECTORIES;
	    ಆದರೆ (p == (char_u *)&p_ಮಾರ್ಗ || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    ಇಲ್ಲದಿದ್ದರೆ
		xp->xp_backslash = XP_BS_ONE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    ಆದರೆ (p == (char_u *)&p_ಟ್ಯಾಗುಗಳು)
		xp->xp_backslash = XP_BS_THREE;
	    ಇಲ್ಲದಿದ್ದರೆ
		xp->xp_backslash = XP_BS_ONE;
	}
	ಆದರೆ (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    ಆದರೆ ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	ಗಾಗಿ (p = argend - 1; p >= xp->xp_ನಮೂನೆ; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    ಆದರೆ (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		ಆಗ (s > xp->xp_ನಮೂನೆ && *(s - 1) == '\\')
		    --s;
		ಆದರೆ ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#ಆದರೆ defined(BACKSLASH_ಒಳ_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#ಇಲ್ಲದಿದ್ದರೆ
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#ಆದರೆ_ಅಂತ್ಯ
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_ನಮೂನೆ = p + 1;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    ಆದರೆ (flags & P_FLAGLIST)
	xp->xp_ನಮೂನೆ = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    ಆದರೆ (options[opt_idx].var == (char_u *)&p_sps)
    {
	ಆದರೆ (STRNCMP(xp->xp_ನಮೂನೆ, "file:", 5) == 0)
	{
	    xp->xp_ನಮೂನೆ += 5;
	    ಹಿಂತಿರುಗಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (options[expand_option_idx].opt_expand_cb != ಖಾಲಿ)
	{
	    xp->xp_context = EXPAND_STRING_SETTING;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	ಪೂರ್ಣಾಂಕ		idx,
	ಪೂರ್ಣಾಂಕ		test_only,
	ಪೂರ್ಣಾಂಕ		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    ಆದರೆ (!fuzzy)
    {
	ಆದರೆ (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    ಆದರೆ (!test_only)
		matches[idx] = vim_strsave(str);
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ score;

	score = fuzzy_ಹೊಂದಿಕೆ_str(str, fuzzystr);
	ಆದರೆ (score != FUZZY_SCORE_NONE)
	{
	    ಆದರೆ (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    ಹಿಂತಿರುಗಿ TRUE;
	}
    }

    ಹಿಂತಿರುಗಿ FALSE;
}

    ಪೂರ್ಣಾಂಕ
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches,
    char_u	***matches,
    ಪೂರ್ಣಾಂಕ		can_fuzzy)
{
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಸಾಮಾನ್ಯ = 0;	    // Nr of matching non-term-code settings
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಪದ = 0;	    // Nr of matching terminal code settings
    ಪೂರ್ಣಾಂಕ		opt_idx;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    char_u	*str;
    ಪೂರ್ಣಾಂಕ		loop;
    ಪೂರ್ಣಾಂಕ		is_ಪದ_opt;
    char_u	name_buf[MAX_ಕೀಲಿ_NAME_LEN];
    ಸ್ಥಿರ ಅಕ್ಷರ *(ಹೆಸರುಗಳು[]) = {"all", "termcap"};
    ಪೂರ್ಣಾಂಕ		ic = regmatch->rm_ic;	// remember the ignore-case flag
    ಪೂರ್ಣಾಂಕ		fuzzy;
    fuzmatch_str_T  *fuzmatch = ಖಾಲಿ;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    ಗಾಗಿ (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	ಆದರೆ (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    ಗಾಗಿ (ಹೊಂದಿಕೆ = 0; ಹೊಂದಿಕೆ < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(ಹೆಸರುಗಳು); ++ಹೊಂದಿಕೆ)
	    {
		ಆದರೆ (ಹೊಂದಿಕೆ_str((char_u *)ಹೆಸರುಗಳು[ಹೊಂದಿಕೆ], regmatch, *matches,
			    ಎಣಿಕೆ_2, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    ಆದರೆ (loop == 0)
			ಅಂಕೆ_ಸಾಮಾನ್ಯ++;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಎಣಿಕೆ_2++;
		}
	    }
	}
	ಗಾಗಿ (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != ಖಾಲಿ;
								    opt_idx++)
	{
	    ಆದರೆ (options[opt_idx].var == ಖಾಲಿ)
		ಮುಂದುವರಿ;
	    ಆದರೆ (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		ಮುಂದುವರಿ;
	    is_ಪದ_opt = istermoption_idx(opt_idx);
	    ಆದರೆ (is_ಪದ_opt && ಅಂಕೆ_ಸಾಮಾನ್ಯ > 0)
		ಮುಂದುವರಿ;

	    ಆದರೆ (ಹೊಂದಿಕೆ_str(str, regmatch, *matches, ಎಣಿಕೆ_2, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		ಆದರೆ (loop == 0)
		{
		    ಆದರೆ (is_ಪದ_opt)
			ಅಂಕೆ_ಪದ++;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಅಂಕೆ_ಸಾಮಾನ್ಯ++;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಎಣಿಕೆ_2++;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!fuzzy && options[opt_idx].shortname != ಖಾಲಿ
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		ಆದರೆ (loop == 0)
		{
		    ಆದರೆ (is_ಪದ_opt)
			ಅಂಕೆ_ಪದ++;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಅಂಕೆ_ಸಾಮಾನ್ಯ++;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    (*matches)[ಎಣಿಕೆ_2++] = vim_strsave(str);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_ಪದ_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		ಆದರೆ (ಹೊಂದಿಕೆ_str(name_buf, regmatch, *matches, ಎಣಿಕೆ_2, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    ಆದರೆ (loop == 0)
			ಅಂಕೆ_ಪದ++;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಎಣಿಕೆ_2++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	ಆದರೆ (xp->xp_context != EXPAND_BOOL_SETTINGS  && ಅಂಕೆ_ಸಾಮಾನ್ಯ == 0)
	{
	    ಗಾಗಿ (opt_idx = 0; (str = get_termcode(opt_idx)) != ಖಾಲಿ; opt_idx++)
	    {
		ಆದರೆ (!ಸುರಕ್ಷಿತ_isprint(str[0]) || !ಸುರಕ್ಷಿತ_isprint(str[1]))
		    ಮುಂದುವರಿ;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		ಆದರೆ (ಹೊಂದಿಕೆ_str(name_buf, regmatch, *matches, ಎಣಿಕೆ_2,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    ಆದರೆ (loop == 0)
			ಅಂಕೆ_ಪದ++;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಎಣಿಕೆ_2++;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    ಆದರೆ (ಹೊಂದಿಕೆ_str(name_buf, regmatch, *matches, ಎಣಿಕೆ_2,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			ಆದರೆ (loop == 0)
			    ಅಂಕೆ_ಪದ++;
			ಇಲ್ಲದಿದ್ದರೆ
			    ಎಣಿಕೆ_2++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    ಗಾಗಿ (opt_idx = 0; (str = get_ಕೀಲಿ_name(opt_idx)) != ಖಾಲಿ; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		ಆದರೆ (ಹೊಂದಿಕೆ_str(name_buf, regmatch, *matches, ಎಣಿಕೆ_2, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    ಆದರೆ (loop == 0)
			ಅಂಕೆ_ಪದ++;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಎಣಿಕೆ_2++;
		}
	    }
	}
	ಆದರೆ (loop == 0)
	{
	    ಆದರೆ (ಅಂಕೆ_ಸಾಮಾನ್ಯ > 0)
		*ಅಂಕೆ_Matches = ಅಂಕೆ_ಸಾಮಾನ್ಯ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಕೆ_ಪದ > 0)
		*ಅಂಕೆ_Matches = ಅಂಕೆ_ಪದ;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ OK;
	    ಆದರೆ (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *ಅಂಕೆ_Matches);
		ಆದರೆ (*matches == ಖಾಲಿ)
		{
		    *matches = (char_u **)"";
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *ಅಂಕೆ_Matches);
		ಆದರೆ (fuzmatch == ಖಾಲಿ)
		{
		    *matches = (char_u **)"";
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	}
    }

    ಆದರೆ (fuzzy &&
	fuzzymatches_ಗಮ್ಯ_strmatches(fuzmatch, matches, ಎಣಿಕೆ_2, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

// Escape an option value that can be used on the command-line with :set.
// Caller needs to free the returned string, unless NULL is returned.
    ಸ್ಥಿರ char_u*
escape_option_str_cmdline(char_u *var)
{
    char_u *buf;

    // A backslash is required before some characters.  This is the reverse of
    // what happens in do_set().
    buf = vim_strsave_escaped(var, escape_chars);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    ಗಾಗಿ (var = buf; *var != NUL; MB_PTR_ADV(var))
	ಆದರೆ (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    ಪೂರ್ಣಾಂಕ
ExpandOldSetting(ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    char_u  *var = ಖಾಲಿ;	// init for GCC
    char_u  *buf;

    *ಅಂಕೆ_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    ಆದರೆ (*matches == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // For a terminal key code expand_option_idx is < 0.
    ಆದರೆ (expand_option_idx < 0)
    {
	var = ಕಂಡುಹಿಡಿ_termcode(expand_option_name + 2);
	ಆದರೆ (var == ಖಾಲಿ)
	    expand_option_idx = findoption(expand_option_name);
    }

    ಆದರೆ (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (var == ಖಾಲಿ)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    ಆದರೆ (buf == ಖಾಲಿ)
    {
	VIM_CLEAR(*matches);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    (*matches)[0] = buf;
    *ಅಂಕೆ_Matches = 1;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    ಪೂರ್ಣಾಂಕ
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches,
    char_u	***matches)
{
    char_u  *var = ಖಾಲಿ;	// init for GCC
    char_u  *buf;

    ಆದರೆ (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == ಖಾಲಿ)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	ಹಿಂತಿರುಗಿ FAIL;
    }

    optexpand_T ವಾದಗಳು;
    ವಾದಗಳು.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    ವಾದಗಳು.oe_append = expand_option_append;
    ವಾದಗಳು.oe_regmatch = regmatch;
    ವಾದಗಳು.oe_xp = xp;
    ವಾದಗಳು.oe_ಗಣ_arg = xp->xp_ಸಾಲು + expand_option_ಆರಂಭ_ಲಂಬಸಾಲು;
    ವಾದಗಳು.oe_include_orig_val =
	!expand_option_append &&
	(*ವಾದಗಳು.oe_ಗಣ_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ವಾದಗಳು.oe_opt_ಮೌಲ್ಯ = buf;

    ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ret = options[expand_option_idx].opt_expand_cb(&ವಾದಗಳು, ಅಂಕೆ_Matches, matches);

    vim_free(buf);
    ಹಿಂತಿರುಗಿ ಅಂಕೆ_ret;
}

/*
 * Expansion handler for :set-=
 */
    ಪೂರ್ಣಾಂಕ
Expand_Setting_ಕಳೆ(
    expand_T	*xp,
    regmatch_T	*regmatch,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches,
    char_u	***matches)
{
    ಆದರೆ (expand_option_idx < 0)
	// term option
	ಹಿಂತಿರುಗಿ ExpandOldSetting(ಅಂಕೆ_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    ಆದರೆ (option_flags & P_ಅಂಕೆ)
	ಹಿಂತಿರುಗಿ ExpandOldSetting(ಅಂಕೆ_Matches, matches);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (option_flags & P_COMMA)
    {
	// Split the option by comma, then present each option to the user if
	// it matches the pattern.
	// This condition needs to go first, because 'whichwrap' has both
	// P_COMMA and P_FLAGLIST.
	garray_T    ga;

	char_u	    *ಅಂಶ;
	char_u	    *option_copy;
	char_u	    *ಮುಂದಿನ_val;
	char_u	    *comma;

	ಆದರೆ (*option_val == NUL)
	    ಹಿಂತಿರುಗಿ FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	ಆದರೆ (option_copy == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಮುಂದಿನ_val = option_copy;

	ga_init2(&ga, ಗಾತ್ರ(char_u *), 10);

	ಮಾಡು
	{
	    ಅಂಶ = ಮುಂದಿನ_val;
	    comma = vim_strchr(ಮುಂದಿನ_val, ',');
	    ಆಗ (comma != ಖಾಲಿ && comma != ಮುಂದಿನ_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    ಆದರೆ (comma != ಖಾಲಿ)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		ಮುಂದಿನ_val = comma + 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಮುಂದಿನ_val = ಖಾಲಿ;

	    ಆದರೆ (*ಅಂಶ == NUL)
		// empty value, don't add to list
		ಮುಂದುವರಿ;

	    ಆದರೆ (!vim_regexec(regmatch, ಅಂಶ, (colnr_T)0))
		ಮುಂದುವರಿ;

	    char_u *buf = escape_option_str_cmdline(ಅಂಶ);
	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		vim_free(option_copy);
		ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (ga_ಕೂಡಿಸು_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		ನಿಲ್ಲಿಸು;
	    }
	} ಆಗ (ಮುಂದಿನ_val != ಖಾಲಿ);

	vim_free(option_copy);

	*matches = ga.ga_ದತ್ತಾಂಶ;
	*ಅಂಕೆ_Matches = ga.ga_len;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	ಆದರೆ (*xp->xp_ನಮೂನೆ != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	size_t ಅಂಕೆ_flags = STRLEN(option_val);
	ಆದರೆ (ಅಂಕೆ_flags == 0)
	    ಹಿಂತಿರುಗಿ FAIL;

	*matches = ALLOC_MULT(char_u *, ಅಂಕೆ_flags + 1);
	ಆದರೆ (*matches == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;
	char_u *p;

	p = vim_strnsave(option_val, ಅಂಕೆ_flags);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    VIM_CLEAR(*matches);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	(*matches)[ಎಣಿಕೆ_2++] = p;

	ಆದರೆ (ಅಂಕೆ_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    ಗಾಗಿ (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		ಆದರೆ (p == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		(*matches)[ಎಣಿಕೆ_2++] = p;
	    }
	}

	*ಅಂಕೆ_Matches = ಎಣಿಕೆ_2;
	ಹಿಂತಿರುಗಿ OK;
    }

    ಹಿಂತಿರುಗಿ ExpandOldSetting(ಅಂಕೆ_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
option_value2string(
    ರಚನೆ vimoption	*opp,
    ಪೂರ್ಣಾಂಕ			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    ಆದರೆ (opp->flags & P_ಅಂಕೆ)
    {
	ದೀರ್ಘ wc = 0;

	ಆದರೆ (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_ಕೀಲಿ_name((ಪೂರ್ಣಾಂಕ)wc, 0));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wc != 0)
	    STRCPY(NameBuff, transchar((ಪೂರ್ಣಾಂಕ)wc));
	ಇಲ್ಲದಿದ್ದರೆ
	    sprintf((ಅಕ್ಷರ *)NameBuff, "%ld", *(ದೀರ್ಘ *)varp);
    }
    ಇಲ್ಲದಿದ್ದರೆ    // P_STRING
    {
	varp = *(char_u **)(varp);
	ಆದರೆ (varp == ಖಾಲಿ)		    // just in case
	    NameBuff[0] = NUL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opp->var == (char_u *)&p_ಕೀಲಿ && *varp)
	    STRCPY(NameBuff, "*****");
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opp->flags & P_EXPAND)
	    home_replace(ಖಾಲಿ, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
wc_use_keyname(char_u *varp, ದೀರ್ಘ *wcp)
{
    ಆದರೆ (((ದೀರ್ಘ *)varp == &p_wc) || ((ದೀರ್ಘ *)varp == &p_wcm))
    {
	*wcp = *(ದೀರ್ಘ *)varp;
	ಆದರೆ (IS_SPECIAL(*wcp) || ಕಂಡುಹಿಡಿ_special_ಕೀಲಿ_ಒಳ_ಕೋಷ್ಟಕ((ಪೂರ್ಣಾಂಕ)*wcp) >= 0)
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    ಪೂರ್ಣಾಂಕ
shortmess(ಪೂರ್ಣಾಂಕ x)
{
    ಹಿಂತಿರುಗಿ p_shm != ಖಾಲಿ &&
	    (   vim_strchr(p_shm, x) != ಖಾಲಿ
	    || (vim_strchr(p_shm, 'a') != ಖಾಲಿ
		&& vim_strchr((char_u *)SHM_A, x) != ಖಾಲಿ));
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 * When "fname" is not NULL, use it to set $"envname" when it wasn't set yet.
 */
    ಶೂನ್ಯ
vimrc_ಸಿಕ್ಕಿತು(char_u *fname, char_u *envname)
{
    ಪೂರ್ಣಾಂಕ		opt_idx;
    ಪೂರ್ಣಾಂಕ		dofree = FALSE;
    char_u	*p;

    ಆದರೆ (!option_was_ಗಣ((char_u *)"cp"))
    {
	p_cp = FALSE;
	ಗಾಗಿ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    ಆದರೆ (!(options[opt_idx].flags & (P_WAS_ಗಣ|P_VI_DEF)))
		ಗಣ_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    ಆದರೆ (fname != ಖಾಲಿ)
    {
	p = vim_getenv(envname, &dofree);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = ತುಂಬಿದ_Name_save(fname, FALSE);
	    ಆದರೆ (p != ಖಾಲಿ)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    ಶೂನ್ಯ
change_compatible(ಪೂರ್ಣಾಂಕ on)
{
    ಪೂರ್ಣಾಂಕ	    opt_idx;

    ಆದರೆ (p_cp != on)
    {
	p_cp = on;
	compatible_ಗಣ();
    }
    opt_idx = findoption((char_u *)"cp");
    ಆದರೆ (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_ಗಣ;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    ಪೂರ್ಣಾಂಕ
option_was_ಗಣ(char_u *name)
{
    ಪೂರ್ಣಾಂಕ idx;

    idx = findoption(name);
    ಆದರೆ (idx < 0)	// unknown option
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (options[idx].flags & P_WAS_ಗಣ)
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    ಪೂರ್ಣಾಂಕ
reset_option_was_ಗಣ(char_u *name)
{
    ಪೂರ್ಣಾಂಕ idx = findoption(name);

    ಆದರೆ (idx < 0)
	ಹಿಂತಿರುಗಿ FAIL;

    options[idx].flags &= ~P_WAS_ಗಣ;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compatible_ಗಣ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    opt_idx;

    ಗಾಗಿ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	ಆದರೆ (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    ಗಣ_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    ಪೂರ್ಣಾಂಕ
can_bs(
    ಪೂರ್ಣಾಂಕ		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (what == BS_ಆರಂಭ && bt_prompt(curbuf))
	ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಆಯ್ಕೆ (*p_bs)
    {
	ಸ್ಥಿತಿ '3':       ಹಿಂತಿರುಗಿ TRUE;
	ಸ್ಥಿತಿ '2':	ಹಿಂತಿರುಗಿ (what != BS_NOSTOP);
	ಸ್ಥಿತಿ '1':	ಹಿಂತಿರುಗಿ (what != BS_ಆರಂಭ);
	ಸ್ಥಿತಿ '0':	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ vim_strchr(p_bs, what) != ಖಾಲಿ;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    ದೀರ್ಘ
get_scrolloff_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    ದೀರ್ಘ
get_scrolloffpad_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    ದೀರ್ಘ
get_sidescrolloff_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ
get_bkc_flags(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#ಆದರೆ defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_ಮೌಲ್ಯ(buf_T *buf)
{
    ಆದರೆ (buf->b_p_flp == ಖಾಲಿ || *buf->b_p_flp == NUL)
	ಹಿಂತಿರುಗಿ p_flp;
    ಹಿಂತಿರುಗಿ buf->b_p_flp;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ
get_ve_flags(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#ಆದರೆ defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_ಮೌಲ್ಯ(win_T *win)
{
    ಆದರೆ (win->w_p_sbr == ಖಾಲಿ || *win->w_p_sbr == NUL)
	ಹಿಂತಿರುಗಿ p_sbr;
    ಆದರೆ (STRCMP(win->w_p_sbr, "NONE") == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿಗಳು_option;
    ಹಿಂತಿರುಗಿ win->w_p_sbr;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(ಪೂರ್ಣಾಂಕ bufopt)
{
    dict_T	*d;
    ಪೂರ್ಣಾಂಕ		opt_idx;

    d = dict_alloc();
    ಆದರೆ (d == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	ರಚನೆ vimoption *opt = &options[opt_idx];

	ಆದರೆ ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    ಆದರೆ (varp != ಖಾಲಿ)
	    {
		ಆದರೆ (opt->flags & P_STRING)
		    dict_ಕೂಡಿಸು_string(d, opt->fullname, *(char_u **)varp);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt->flags & P_ಅಂಕೆ)
		    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, opt->fullname, *(ದೀರ್ಘ *)varp);
		ಇಲ್ಲದಿದ್ದರೆ
		    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, opt->fullname, *(ಪೂರ್ಣಾಂಕ *)varp);
	    }
	}
    }

    ಹಿಂತಿರುಗಿ d;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    ಪೂರ್ಣಾಂಕ
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    ಆದರೆ (val == ಖಾಲಿ)
	p = wp->w_p_culopt;
    ಇಲ್ಲದಿದ್ದರೆ
	p = val;
    ಆಗ (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	ಆದರೆ (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_ಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_ಸಾಲು | CULOPT_NBR;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	ಆದರೆ (*p != ',' && *p != NUL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    ಆದರೆ ((culopt_flags_new & CULOPT_ಸಾಲು) && (culopt_flags_new & CULOPT_SCRLINE))
	ಹಿಂತಿರುಗಿ FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    ಪೂರ್ಣಾಂಕ
magic_isset(ಶೂನ್ಯ)
{
    ಆಯ್ಕೆ (magic_overruled)
    {
	ಸ್ಥಿತಿ OPTION_MAGIC_ON:      ಹಿಂತಿರುಗಿ TRUE;
	ಸ್ಥಿತಿ OPTION_MAGIC_OFF:     ಹಿಂತಿರುಗಿ FALSE;
	ಸ್ಥಿತಿ OPTION_MAGIC_NOT_ಗಣ: ನಿಲ್ಲಿಸು;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (ಒಳ_vim9script())
	ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    ಪೂರ್ಣಾಂಕ
option_ಗಣ_callback_func(char_u *optval ಬಳಸದ, callback_T *optcb ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    ಪೂರ್ಣಾಂಕ		funcname = FALSE;

    ಆದರೆ (optval == ಖಾಲಿ || *optval == NUL)
    {
	free_callback(optcb);
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ (*optval == '{' || (ಒಳ_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_ಅಭಿವ್ಯಕ್ತಿ(optval, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    ಆದರೆ (tv == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    cb = get_callback(tv);
    ಆದರೆ (cb.cb_name == ಖಾಲಿ || *cb.cb_name == NUL)
    {
	free_tv(tv);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    free_callback(optcb);
    ಗಣ_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = ಖಾಲಿ;
    ಆದರೆ (ಒಳ_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != ಖಾಲಿ))
    {
	ಆದರೆ (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = ಕಂಡುಹಿಡಿ_imported(optval, dot - optval, FALSE);
	ಆದರೆ (import != ಖಾಲಿ && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((ಅಕ್ಷರ *)fnamebuf, ಗಾತ್ರ(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
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

    ಹಿಂತಿರುಗಿ OK;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_showtabpanel(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    shell_new_columns();
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
    ಸ್ಥಿರ ಶೂನ್ಯ
didset_options_sctx(ಪೂರ್ಣಾಂಕ opt_flags, ಅಕ್ಷರ **buf)
{
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; ; ++i)
	{
	    ಆದರೆ (buf[i] == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

	    ಪೂರ್ಣಾಂಕ idx = findoption((char_u *)buf[i]);
	    ಆದರೆ (idx >= 0)
		ಗಣ_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#ಆದರೆ_ಅಂತ್ಯ
