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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_OPTION_C
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "optiondefs.h"

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_options_default(ਪੂਰਨ opt_flags);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_string_default_esc(ਅੱਖਰ *name, char_u *val, ਪੂਰਨ escape);
ਸਥਿਰ char_u *ਲੱਭੋ_dup_ਵਸਤੂ(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
ਸਥਿਰ char_u *option_expand(ਪੂਰਨ opt_idx, char_u *val);
ਸਥਿਰ ਖਾਲੀ didset_options(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ didset_options2(ਖਾਲੀ);
#ਜੇ defined(FEAT_EVAL)
ਸਥਿਰ long_u *insecure_flag(win_T *wp, ਪੂਰਨ opt_idx, ਪੂਰਨ opt_flags);
#ਨਹੀਂਤਾਂ
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ *ਸੈੱਟ_bool_option(ਪੂਰਨ opt_idx, char_u *varp, ਪੂਰਨ ਮੁੱਲ, ਪੂਰਨ opt_flags);
ਸਥਿਰ ਅੱਖਰ *ਸੈੱਟ_ਅੰਕ_option(ਪੂਰਨ opt_idx, char_u *varp, ਲੰਮਾ ਮੁੱਲ, ਅੱਖਰ *errbuf, size_t errbuflen, ਪੂਰਨ opt_flags);
ਸਥਿਰ ਪੂਰਨ ਲੱਭੋ_ਕੁੰਜੀ_option(char_u *arg_arg, ਪੂਰਨ has_lt);
ਸਥਿਰ ਖਾਲੀ showoptions(ਪੂਰਨ all, ਪੂਰਨ opt_flags);
ਸਥਿਰ ਪੂਰਨ optval_default(ਬਣਤਰ vimoption *, char_u *varp, ਪੂਰਨ compatible);
ਸਥਿਰ ਖਾਲੀ showoneopt(ਬਣਤਰ vimoption *, ਪੂਰਨ opt_flags);
ਸਥਿਰ ਪੂਰਨ put_setstring(FILE *fd, bool legacy, ਅੱਖਰ *cmd, ਅੱਖਰ *name, char_u **valuep, long_u flags);
ਸਥਿਰ ਪੂਰਨ put_setnum(FILE *fd, ਅੱਖਰ *cmd, ਅੱਖਰ *name, ਲੰਮਾ *valuep);
ਸਥਿਰ ਪੂਰਨ put_setbool(FILE *fd, ਅੱਖਰ *cmd, ਅੱਖਰ *name, ਪੂਰਨ ਮੁੱਲ);
ਸਥਿਰ ਪੂਰਨ istermoption(ਬਣਤਰ vimoption *p);
ਸਥਿਰ ਪੂਰਨ istermoption_idx(ਪੂਰਨ opt_idx);
ਸਥਿਰ char_u *get_varp_scope(ਬਣਤਰ vimoption *p, ਪੂਰਨ scope);
ਸਥਿਰ char_u *get_varp(ਬਣਤਰ vimoption *);
ਸਥਿਰ ਖਾਲੀ check_win_options(win_T *win);
ਸਥਿਰ ਖਾਲੀ option_value2string(ਬਣਤਰ vimoption *, ਪੂਰਨ scope);
ਸਥਿਰ ਖਾਲੀ check_winopt(winopt_T *wop);
ਸਥਿਰ ਪੂਰਨ wc_use_keyname(char_u *varp, ਲੰਮਾ *wcp);
ਸਥਿਰ ਖਾਲੀ compatible_ਸੈੱਟ(ਖਾਲੀ);

#ਜੇ defined(FEAT_EVAL)
ਸਥਿਰ ਅੱਖਰ *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    ਨਿਰਾ};
ਸਥਿਰ ਖਾਲੀ didset_options_sctx(ਪੂਰਨ opt_flags, ਅੱਖਰ **buf);
#ਜੇ_ਅੰਤ


/*
 * Initialize the 'shell' option to a default value.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_init_default_shell(ਖਾਲੀ)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    ਜੇ (((p = mch_getenv((char_u *)"SHELL")) != ਨਿਰਾ && *p != NUL)
#ਜੇ defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != ਨਿਰਾ && *p != NUL)
	    || ((p = (char_u *)default_shell()) != ਨਿਰਾ && *p != NUL)
#ਜੇ_ਅੰਤ
	    )
#ਜੇ defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	ਜੇ (vim_strchr(p, ' ') != ਨਿਰਾ)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    ਜੇ (cmd != ਨਿਰਾ)
	    {
		vim_snprintf((ਅੱਖਰ *)cmd, len, "\"%s\"", p);
		ਸੈੱਟ_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਸੈੱਟ_string_default("sh", p);
    }
#ਨਹੀਂਤਾਂ
	ਸੈੱਟ_string_default_esc("sh", p, TRUE);
#ਜੇ_ਅੰਤ
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_init_default_backupskip(ਖਾਲੀ)
{
    ਪੂਰਨ		opt_idx;
    ਪੂਰਨ		i;
    char_u	*p;
    ਪੂਰਨ		plen;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਸਥਿਰ ਅੱਖਰ	*(ਨਾਂ[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#ਨਹੀਂਤਾਂ
    ਸਥਿਰ ਅੱਖਰ	*(ਨਾਂ[3]) = {"TMPDIR", "TEMP", "TMP"};
#ਜੇ_ਅੰਤ
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(ਨਾਂ); ++i)
    {
	ਪੂਰਨ		mustfree = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	ਜੇ (*ਨਾਂ[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (ਪੂਰਨ)STRLEN_LITERAL("/private/tmp");
# ਨਹੀਂਤਾਂ
	    p = (char_u *)"/tmp";
	    plen = (ਪੂਰਨ)STRLEN_LITERAL("/tmp");
# endif
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    p = vim_getenv((char_u *)ਨਾਂ[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	ਜੇ (p != ਨਿਰਾ && *p != NUL)
	{
	    char_u  *ਵਸਤੂ;
	    size_t  itemsize;
	    ਪੂਰਨ	    has_trailing_ਰਸਤਾ_sep = FALSE;

	    ਜੇ (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (ਪੂਰਨ)STRLEN(p);
		// does the value include a trailing path separator?
		ਜੇ (after_pathsep(p, p + plen))
		    has_trailing_ਰਸਤਾ_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_ਰਸਤਾ_sep ? 0 : 1) + 2;
	    ਵਸਤੂ = alloc(itemsize);
	    ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((ਅੱਖਰ *)ਵਸਤੂ, itemsize, "%s%s*", p, (has_trailing_ਰਸਤਾ_sep) ? "" : PATHSEPSTR);

		ਜੇ (ਲੱਭੋ_dup_ਵਸਤੂ(ga.ga_ਅੰਕੜੇ, ਵਸਤੂ, itemlen, options[opt_idx].flags) == ਨਿਰਾ
			&& ga_grow(&ga, (ਪੂਰਨ)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", ਵਸਤੂ);
		}
		vim_free(ਵਸਤੂ);
	    }
	}
	ਜੇ (mustfree)
	    vim_free(p);
    }
    ਜੇ (ga.ga_ਅੰਕੜੇ != ਨਿਰਾ)
    {
	ਸੈੱਟ_string_default("bsk", ga.ga_ਅੰਕੜੇ);
	vim_free(ga.ga_ਅੰਕੜੇ);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_init_default_maxmemtot(ਖਾਲੀ)
{
    ਪੂਰਨ		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    ਜੇ (opt_idx < 0)
	ਵਾਪਸ;

#ਜੇ !defined(HAVE_AVAIL_MEM) && !defined(HAVE_ਕੁੱਲ_MEM)
    ਜੇ (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#ਜੇ_ਅੰਤ
    {
#ਜੇ defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#ਜਾਂਜੇ defined(HAVE_ਕੁੱਲ_MEM)
	// Use amount of memory available to Vim.
	n = (mch_ਕੁੱਲ_mem(FALSE) >> 1);
#ਨਹੀਂਤਾਂ
	n = (0x7fffffff >> 11);
#ਜੇ_ਅੰਤ
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	ਜੇ (opt_idx >= 0)
	{
#ਜੇ !defined(HAVE_AVAIL_MEM) && !defined(HAVE_ਕੁੱਲ_MEM)
	    ਜੇ ((ਲੰਮਾ)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (ਲੰਮਾ)n
		    || (ਲੰਮਾ)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#ਜੇ_ਅੰਤ
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_init_default_cdpath(ਖਾਲੀ)
{
    ਪੂਰਨ		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    ਪੂਰਨ	i;
    ਪੂਰਨ	j;
    ਪੂਰਨ	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    ਜੇ (cdpath == ਨਿਰਾ)
	ਵਾਪਸ;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    ਜੇ (buf != ਨਿਰਾ)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	ਲਈ (i = 0; cdpath[i] != NUL; ++i)
	{
	    ਜੇ (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	ਜੇ (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	ਨਹੀਂਤਾਂ
	    vim_free(buf); // cannot happen
    }
    ਜੇ (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_init_default_printencoding(ਖਾਲੀ)
{
#ਜੇ defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    ਸੈੱਟ_string_default("penc",
# ਜੇ defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# ਨਹੀਂਤਾਂ // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_init_default_printexpr(ਖਾਲੀ)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    ਸੈੱਟ_string_default("pexpr",
# ਜੇ defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# ਨਹੀਂਤਾਂ
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_init_restricted_ਬਹੁਲਕ(ਖਾਲੀ)
{
    char_u	*p;

    p = get_isolated_shell_name();
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_init_clean_rtp(ਖਾਲੀ)
{
    ਪੂਰਨ		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    ਜੇ (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    ਜੇ (opt_idx < 0)
	ਵਾਪਸ;
    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
    p_pp = (char_u *)CLEAN_RUNTIMEPATH;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
/*
 * Change 'runtimepath' and 'packdir' to '$XDG_CONFIG_HOME/vim' if the only
 * vimrc found is located in '$XDG_CONFIG_HOME/vim/vimrc'.
 * In case the '$XDG_CONFIG_HOME' variable is not set, '$HOME/.config' is used
 * as a fallback as is defined in the XDG base dir specification:
 * <https://specifications.freedesktop.org/basedir/latest/>
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_init_xdg_rtp(ਖਾਲੀ)
{
    ਪੂਰਨ		opt_idx;
    ਪੂਰਨ		has_xdg_env = TRUE;
    ਪੂਰਨ		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = ਨਿਰਾ;
    char_u	*vimrc2 = ਨਿਰਾ;
    char_u	*xdg_dir = ਨਿਰਾ;
    char_u	*xdg_rtp = ਨਿਰਾ;
    string_T	vimrc_xdg = {ਨਿਰਾ, 0};

    // initialize chartab, so we can expand $HOME
    (ਖਾਲੀ)init_chartab();
    vimrc1 = expand_env_save((char_u *)USR_VIMRC_FILE);
    vimrc2 = expand_env_save((char_u *)USR_VIMRC_FILE2);

    xdg_dir = mch_getenv("XDG_CONFIG_HOME");
    ਜੇ (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    ਜੇ (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	ਜਾਓ theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    ਜੇ ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	ਜਾਓ theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    ਜੇ ((opt_idx = findoption((char_u *)"packpath")) < 0)
	ਜਾਓ theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# ਜੇ defined(XDG_VDIR) && defined(FEAT_SESSION)
    ਜੇ ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	ਜਾਓ theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    ਜੇ (should_free_xdg_dir)
	vim_free(xdg_dir);
}
#ਜੇ_ਅੰਤ

/*
 * Expand environment variables and things like "~" for the defaults.
 * If option_expand() returns non-NULL the variable is expanded.  This can
 * only happen for non-indirect options.
 * Also set the default to the expanded value, so ":set" does not list
 * them.
 * Don't set the P_ALLOCED flag, because we don't want to free the
 * default.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_init_expand_env(ਖਾਲੀ)
{
    ਪੂਰਨ		opt_idx;
    char_u	*p;

    ਲਈ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	ਜੇ ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != ਨਿਰਾ)
	    p = (char_u *)_(*(ਅੱਖਰ **)options[opt_idx].var);
	ਨਹੀਂਤਾਂ
	    p = option_expand(opt_idx, ਨਿਰਾ);
	ਜੇ (p != ਨਿਰਾ && (p = vim_strsave(p)) != ਨਿਰਾ)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    ਜੇ (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#ਜੇ defined(MSWIN) && defined(FEAT_GETTEXT)
/*
 * Get the display language of Windows and the languages to fall back on, as a
 * colon separated list for gettext, e.g. "ja_JP:en_US".  The list stops after
 * English, untranslated messages are English already.
 * Returns NULL when it cannot be obtained.  The result must be freed.
 */
    ਸਥਿਰ char_u *
get_ui_langs(ਖਾਲੀ)
{
    ULONG	ਅੰਕ_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*ਬਫਰ;
    char_u	*langs = ਨਿਰਾ;

    ਜੇ (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &ਅੰਕ_languages, ਨਿਰਾ,
								    &bufsize)
	    || bufsize == 0)
	ਵਾਪਸ ਨਿਰਾ;

    ਬਫਰ = ALLOC_MULT(WCHAR, bufsize);
    ਜੇ (ਬਫਰ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &ਅੰਕ_languages, ਬਫਰ,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	ਜੇ (langs != ਨਿਰਾ)
	{
	    char_u	*d = langs;
	    WCHAR	*s = ਬਫਰ;

	    ਜਦੋਂ (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		ਜੇ (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		ਲਈ ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		ਜੇ (english)
		    ਰੋਕੋ;
	    }
	    *d = NUL;
	}
    }
    vim_free(ਬਫਰ);

    ਵਾਪਸ langs;
}
#ਜੇ_ਅੰਤ

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_init_lang_env(ਖਾਲੀ)
{
#ਜੇ defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    ਜੇ (mch_getenv((char_u *)"LANG") == ਨਿਰਾ
	    && mch_getenv((char_u *)"LANGUAGE") == ਨਿਰਾ
	    && mch_getenv((char_u *)"LC_ALL") == ਨਿਰਾ
	    && mch_getenv((char_u *)"LC_MESSAGES") == ਨਿਰਾ)
    {
	char_u	*langs = get_ui_langs();

	ਜੇ (langs != ਨਿਰਾ && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    ਜੇ (colon != ਨਿਰਾ)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#ਜਾਂਜੇ defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#ਜੇ_ਅੰਤ
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_init_default_encoding(ਖਾਲੀ)
{
    char_u	*p;
    ਪੂਰਨ		opt_idx;

#ਜੇ defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#ਨਹੀਂਤਾਂ
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#ਜੇ_ਅੰਤ
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    ਜੇ (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    ਜੇ (mb_init() == ਨਿਰਾ)
    {
	opt_idx = findoption((char_u *)"encoding");
	ਜੇ (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#ਜੇ defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	ਜੇ (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    ਜੇ (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    ਜੇ (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (ਖਾਲੀ)init_chartab();
	}
#ਜੇ_ਅੰਤ

#ਜੇ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	ਜੇ (
# ifdef VIMDLL
		(!gui.ਵਿੱਚ_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    ਅੱਖਰ	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    ਜੇ (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, ਆਕਾਰ(buf), "cp%ld", (ਲੰਮਾ)GetACP());
	    ਨਹੀਂਤਾਂ
		buflen = vim_snprintf(buf, ਆਕਾਰ(buf), "cp%ld", (ਲੰਮਾ)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    ਜੇ (p_tenc != ਨਿਰਾ)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		ਜੇ (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_ਤਿਆਰੀ(&ਨਿਵੇਸ਼_conv, p_tenc, p_enc);
		convert_ਤਿਆਰੀ(&output_conv, p_enc, p_tenc);
	    }
	    ਨਹੀਂਤਾਂ
		p_tenc = ਖਾਲੀ_ਥਾਵਾਂ_option;
	}
#ਜੇ_ਅੰਤ
#ਜੇ defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
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
    ਖਾਲੀ
ਸੈੱਟ_init_1(ਪੂਰਨ clean_arg)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
    langmap_init();
#ਜੇ_ਅੰਤ

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    ਜੇ (mch_getenv((char_u *)"VIM_POSIX") != ਨਿਰਾ)
    {
	ਸੈੱਟ_string_default("cpo", (char_u *)CPO_ALL);
	ਸੈੱਟ_string_default("shm", (char_u *)SHM_POSIX);
    }

    ਸੈੱਟ_init_default_shell();
    ਸੈੱਟ_init_default_backupskip();
    ਸੈੱਟ_init_default_maxmemtot();
    ਸੈੱਟ_init_default_cdpath();
    ਸੈੱਟ_init_default_printencoding();
#ਜੇ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    ਸੈੱਟ_init_default_printexpr();
#ਜੇ_ਅੰਤ

    /*
     * Set all the options (except the terminal options) to their default
     * value.  Also set the global value for local options.
     */
    ਸੈੱਟ_options_default(0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਸੈੱਟ_init_xdg_rtp();
    ਸੈੱਟ_init_restricted_ਬਹੁਲਕ();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CLEAN_RUNTIMEPATH
    ਜੇ (clean_arg)
	ਸੈੱਟ_init_clean_rtp();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (ਮਿਲਿਆ_ਉਲਟ_arg)
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
#ਜੇ_ਅੰਤ

    curbuf->b_p_initialized = true;
    curbuf->b_p_ac = -1;
    curbuf->b_p_ar = -1;	// no local 'autoread' value
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FSYNC
    curbuf->b_p_fs = -1;	// no local 'fsync' value
#ਜੇ_ਅੰਤ
    curbuf->b_p_ul = NO_LOCAL_UNDOLEVEL;
    check_buf_options(curbuf);
    check_win_options(curwin);
    check_options();

    // Must be before option_expand(), because that one needs vim_isIDc()
    didset_options();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    // Use the current chartab for the generic chartab. This is not in
    // didset_options() because it only depends on 'encoding'.
    init_spell_chartab();
#ਜੇ_ਅੰਤ

    ਸੈੱਟ_init_default_encoding();

    // Expand environment variables and things like "~" for the defaults.
    ਸੈੱਟ_init_expand_env();

    save_file_ff(curbuf);	// Buffer is unchanged

#ਜੇ defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    ਜੇ (mch_getenv((char_u *)"MLTERM") != ਨਿਰਾ)
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"tbidi", 1L, ਨਿਰਾ, 0);
#ਜੇ_ਅੰਤ

    didset_options2();

    ਸੈੱਟ_init_lang_env();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    ਸੈੱਟ_helplang_default(get_mess_lang());
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    ਖਾਲੀ
ਸੈੱਟ_fencs_unicode(ਖਾਲੀ)
{
    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_option_default(
    ਪੂਰਨ		opt_idx,
    ਪੂਰਨ		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    ਪੂਰਨ		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    ਪੂਰਨ		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    ਪੂਰਨ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    ਜੇ (varp != ਨਿਰਾ)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	ਜੇ (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    ਜੇ (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		ਸੈੱਟ_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    ਨਹੀਂਤਾਂ ਜੇ (options[opt_idx].indir != PV_NONE)
		ਸੈੱਟ_string_option_ਸਿੱਧਾ(ਨਿਰਾ, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (flags & P_ਅੰਕ)
	{
	    ਜੇ (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    ਨਹੀਂਤਾਂ
	    {
		ਲੰਮਾ def_val = (ਲੰਮਾ)(long_i)options[opt_idx].def_val[dvi];

		ਜੇ ((ਲੰਮਾ *)varp == &curwin->w_p_so
			|| (ਲੰਮਾ *)varp == &curwin->w_p_siso
			|| (ਲੰਮਾ *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(ਲੰਮਾ *)varp = -1;
		ਨਹੀਂਤਾਂ
		    *(ਲੰਮਾ *)varp = def_val;
		// May also set global value for local option.
		ਜੇ (both)
		    *(ਲੰਮਾ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	ਨਹੀਂਤਾਂ	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(ਪੂਰਨ *)varp = (ਪੂਰਨ)(ਲੰਮਾ)(long_i)options[opt_idx].def_val[dvi];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	    // 'modeline' defaults to off for root
	    ਜੇ (options[opt_idx].indir == PV_ML && getuid() == ਜੜ੍ਹ_UID)
		*(ਪੂਰਨ *)varp = FALSE;
#ਜੇ_ਅੰਤ
	    // May also set global value for local option.
	    ਜੇ (both)
		*(ਪੂਰਨ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(ਪੂਰਨ *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	ਜੇ (both)
	{
	    flagsp = insecure_flag(curwin, opt_idx, OPT_LOCAL);
	    *flagsp = *flagsp & ~P_INSECURE;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸੈੱਟ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#ਜੇ_ਅੰਤ
}

/*
 * Set all options (except terminal options) to their default value.
 * When "opt_flags" is non-zero skip 'encoding'.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_options_default(
    ਪੂਰਨ		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    ਪੂਰਨ		i;
    win_T	*wp;
    tabpage_T	*tp;

    ਲਈ (i = 0; !istermoption_idx(i); i++)
	ਜੇ (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#ਜੇ defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_ਕੁੰਜੀ
#ਜੇ_ਅੰਤ
			)))
	    ਸੈੱਟ_option_default(i, opt_flags, p_cp);

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
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_string_default_esc(ਅੱਖਰ *name, char_u *val, ਪੂਰਨ escape)
{
    char_u	*p;
    ਪੂਰਨ		opt_idx;

    ਜੇ (escape && vim_strchr(val, ' ') != ਨਿਰਾ)
	p = vim_strsave_escaped(val, (char_u *)" ");
    ਨਹੀਂਤਾਂ
	p = vim_strsave(val);
    ਜੇ (p == ਨਿਰਾ)		// we don't want a NULL
	ਵਾਪਸ;

    opt_idx = findoption((char_u *)name);
    ਜੇ (opt_idx < 0)
    {
	vim_free(p);
	ਵਾਪਸ;
    }

    ਜੇ (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    ਖਾਲੀ
ਸੈੱਟ_string_default(ਅੱਖਰ *name, char_u *val)
{
    ਸੈੱਟ_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    ਸਥਿਰ char_u *
ਲੱਭੋ_dup_ਵਸਤੂ(char_u *origval, char_u *newval, size_t newvallen, long_u flags)
{
    ਪੂਰਨ	    bs = 0;
    char_u  *s;

    ਜੇ (origval == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (s = origval; *s != NUL; ++s)
    {
	ਜੇ ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    ਵਾਪਸ s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	ਜੇ ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	ਨਹੀਂਤਾਂ
	    bs = 0;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    ਖਾਲੀ
ਸੈੱਟ_ਸੰਖਿਆ_default(ਅੱਖਰ *name, ਲੰਮਾ val)
{
    ਪੂਰਨ		opt_idx;

    opt_idx = findoption((char_u *)name);
    ਜੇ (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#ਜੇ defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    ਖਾਲੀ
ਸੈੱਟ_local_options_default(win_T *wp, ਪੂਰਨ do_ਬਫਰ)
{
    win_T	*save_curwin = curwin;
    ਪੂਰਨ		i;

    curwin = wp;
    curbuf = curwin->w_ਬਫਰ;
    ਖੰਡ_autocmds();

    ਲਈ (i = 0; !istermoption_idx(i); i++)
    {
	ਬਣਤਰ vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	ਜੇ (p->indir != PV_NONE
		&& (do_ਬਫਰ || (p->indir & PV_BUF) == 0)
		&& !(options[i].flags & P_NODEFAULT)
		&& !optval_default(p, varp, FALSE))
	    ਸੈੱਟ_option_default(i, OPT_FREE|OPT_LOCAL, FALSE);
    }

    unblock_autocmds();
    curwin = save_curwin;
    curbuf = curwin->w_ਬਫਰ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(EXITFREE)
/*
 * Free all options.
 */
    ਖਾਲੀ
free_all_options(ਖਾਲੀ)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; !istermoption_idx(i); i++)
    {
	ਜੇ (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    ਜੇ ((options[i].flags & P_ALLOCED) && options[i].var != ਨਿਰਾ)
		free_string_option(*(char_u **)options[i].var);
	    ਜੇ (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	ਨਹੀਂਤਾਂ ਜੇ (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# ਜੇ defined(FEAT_EVAL)
    free_findfunc_option();
# endif
}
#ਜੇ_ਅੰਤ


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    ਖਾਲੀ
ਸੈੱਟ_init_2(ਖਾਲੀ)
{
    ਪੂਰਨ		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    ਜੇ (idx >= 0 && !(options[idx].flags & P_WAS_ਸੈੱਟ))
	ਸੈੱਟ_option_default(idx, OPT_LOCAL, p_cp);
    comp_ਖਾਨਾ();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    ਜੇ (!option_was_ਸੈੱਟ((char_u *)"window"))
	p_ਖਿੜਕੀ = ਪੰਕਤੀਆਂ - 1;
    ਸੈੱਟ_ਸੰਖਿਆ_default("window", ਪੰਕਤੀਆਂ - 1);

    // For DOS console the default is always black.
#ਜੇ !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    ਜੇ (idx >= 0 && !(options[idx].flags & P_WAS_ਸੈੱਟ)
						 && *ਪਦ_bg_default() == 'd')
    {
	ਸੈੱਟ_string_option_ਸਿੱਧਾ(ਨਿਰਾ, idx, (char_u *)"dark", OPT_FREE, 0);
	// don't mark it as set, when starting the GUI it may be
	// changed again
	options[idx].flags &= ~P_WAS_ਸੈੱਟ;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
    parse_shape_opt(SHAPE_CURSOR);	// set cursor shapes from 'guicursor'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
    parse_shape_opt(SHAPE_MOUSE);	// set mouse shapes from 'mouseshape'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINTER
    (ਖਾਲੀ)parse_printoptions(ਨਿਰਾ);	// parse 'printoptions' default value
#ਜੇ_ਅੰਤ
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    ਖਾਲੀ
ਸੈੱਟ_init_3(ਖਾਲੀ)
{
#ਜੇ defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    ਪੂਰਨ	    idx_srr;
    ਪੂਰਨ	    do_srr;
# ifdef FEAT_QUICKFIX
    ਪੂਰਨ	    idx_sp;
    ਪੂਰਨ	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    ਜੇ (idx_srr < 0)
	do_srr = FALSE;
    ਨਹੀਂਤਾਂ
	do_srr = !(options[idx_srr].flags & P_WAS_ਸੈੱਟ);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    ਜੇ (idx_sp < 0)
	do_sp = FALSE;
    ਨਹੀਂਤਾਂ
	do_sp = !(options[idx_sp].flags & P_WAS_ਸੈੱਟ);
# endif
    p = get_isolated_shell_name();
    ਜੇ (p != ਨਿਰਾ)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	ਜੇ (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# ਜੇ defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# ਜੇ defined(FEAT_QUICKFIX)
	    ਜੇ (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  ਨਹੀਂਤਾਂ
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    ਜੇ (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	ਨਹੀਂਤਾਂ ਜੇ (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  ਜੇ defined(FEAT_QUICKFIX)
		ਜੇ (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		ਜੇ (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	ਨਹੀਂਤਾਂ
	    // Always use POSIX shell style redirection if we reach this
	    ਜੇ (       fnamecmp(p, "sh") == 0
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
# ਜੇ defined(FEAT_QUICKFIX)
		ਜੇ (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  ਨਹੀਂਤਾਂ
		    ਜੇ (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    ਨਹੀਂਤਾਂ
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		ਜੇ (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(MSWIN)
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
    ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)gettail(p_sh), "powershell") != ਨਿਰਾ)
    {
	ਪੂਰਨ	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	ਜੇ (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_ਸੈੱਟ))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	ਜੇ (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_ਸੈੱਟ))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)gettail(p_sh), "sh") != ਨਿਰਾ)
    {
	ਪੂਰਨ	idx3;

	idx3 = findoption((char_u *)"shcf");
	ਜੇ (idx3 >= 0 && !(options[idx3].flags & P_WAS_ਸੈੱਟ))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	ਜੇ (idx3 >= 0 && !(options[idx3].flags & P_WAS_ਸੈੱਟ))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)gettail(p_sh), "cmd.exe") != ਨਿਰਾ)
    {
	ਪੂਰਨ	idx3;

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
	ਜੇ (idx3 >= 0 && !(options[idx3].flags & P_WAS_ਸੈੱਟ))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	ਜੇ (idx3 >= 0 && !(options[idx3].flags & P_WAS_ਸੈੱਟ))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#ਜੇ_ਅੰਤ

    ਜੇ (BUFEMPTY())
    {
	ਪੂਰਨ idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	ਜੇ (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_ਸੈੱਟ))
	    ਸੈੱਟ_fileformat(default_fileformat(), OPT_LOCAL);
    }

    ਸੈੱਟ_title_defaults();
}

#ਜੇ defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    ਖਾਲੀ
ਸੈੱਟ_helplang_default(char_u *lang)
{
    ਪੂਰਨ		idx;
    size_t	langlen;

    ਜੇ (lang == ਨਿਰਾ)	// safety check
	ਵਾਪਸ;

    langlen = STRLEN(lang);
    ਜੇ (langlen < 2)	// safety check
	ਵਾਪਸ;

    idx = findoption((char_u *)"hlg");
    ਜੇ (idx < 0 || (options[idx].flags & P_WAS_ਸੈੱਟ))
	ਵਾਪਸ;

    ਜੇ (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    ਜੇ (p_hlg == ਨਿਰਾ)
	p_hlg = ਖਾਲੀ_ਥਾਵਾਂ_option;
    ਨਹੀਂਤਾਂ
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	ਜੇ (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	ਨਹੀਂਤਾਂ ਜੇ (langlen >= 1 && *p_hlg == 'C')
	{
	    p_hlg[0] = 'e';
	    p_hlg[1] = 'n';
	}
	p_hlg[2] = NUL;
    }
    options[idx].flags |= P_ALLOCED;
}
#ਜੇ_ਅੰਤ

/*
 * 'title' and 'icon' only default to true if they have not been set or reset
 * in .vimrc and we can read the old value.
 * When 'title' and 'icon' have been reset in .vimrc, we won't even check if
 * they can be reset.  This reduces startup time when using X on a remote
 * machine.
 */
    ਖਾਲੀ
ਸੈੱਟ_title_defaults(ਖਾਲੀ)
{
    ਪੂਰਨ	    idx1;
    ਲੰਮਾ    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    ਜੇ (idx1 >= 0 && !(options[idx1].flags & P_WAS_ਸੈੱਟ))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (gui.starting || gui.ਵਿੱਚ_use)
	    val = TRUE;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    ਜੇ (idx1 < 0 || (options[idx1].flags & P_WAS_ਸੈੱਟ))
    {
	ਵਾਪਸ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.starting || gui.ਵਿੱਚ_use)
	val = TRUE;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    ਖਾਲੀ
ex_ਸੈੱਟ(exarg_T *eap)
{
    ਪੂਰਨ		flags = 0;

    ਜੇ (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#ਜੇ defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    ਜੇ ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	ਜੇ (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(ਖਾਲੀ)do_ਸੈੱਟ(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    ਅਗੇਤਰ_NO = 0,	// "no" prefix
    ਅਗੇਤਰ_NONE,	// no prefix
    ਅਗੇਤਰ_INV,		// "inv" prefix
} ਸੈੱਟ_ਅਗੇਤਰ_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    ਸਥਿਰ ਸੈੱਟ_ਅਗੇਤਰ_T
get_option_ਅਗੇਤਰ(char_u **argp)
{
    ਪੂਰਨ		ਅਗੇਤਰ = ਅਗੇਤਰ_NONE;
    char_u	*arg = *argp;

    ਜੇ (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	ਅਗੇਤਰ = ਅਗੇਤਰ_NO;
	arg += 2;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "inv", 3) == 0)
    {
	ਅਗੇਤਰ = ਅਗੇਤਰ_INV;
	arg += 3;
    }

    *argp = arg;
    ਵਾਪਸ ਅਗੇਤਰ;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    ਸਥਿਰ ਪੂਰਨ
parse_option_name(char_u *arg, ਪੂਰਨ *opt_idxp, ਪੂਰਨ *lenp, ਪੂਰਨ *keyp)
{
    ਪੂਰਨ	ਕੁੰਜੀ = 0;
    ਪੂਰਨ	len;
    ਪੂਰਨ	opt_idx;

    ਜੇ (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	ਜੇ (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	ਨਹੀਂਤਾਂ
	{
	    len = 1;
	    ਜਦੋਂ (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	ਜੇ (arg[len] != '>')
	    ਵਾਪਸ FAIL;

	arg[len] = NUL;			    // put NUL after name
	ਜੇ (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	ਜੇ (opt_idx == -1)
	    ਕੁੰਜੀ = ਲੱਭੋ_ਕੁੰਜੀ_option(arg + 1, TRUE);
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	ਜੇ (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	ਨਹੀਂਤਾਂ
	    ਜਦੋਂ (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	ਜੇ (opt_idx == -1)
	    ਕੁੰਜੀ = ਲੱਭੋ_ਕੁੰਜੀ_option(arg, FALSE);
    }

    *keyp = ਕੁੰਜੀ;
    *lenp = len;
    *opt_idxp = opt_idx;

    ਵਾਪਸ OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    ਸਥਿਰ ਸੈੱਟ_op_T
get_opt_op(char_u *arg)
{
    ਸੈੱਟ_op_T op = OP_NONE;

    ਜੇ (*arg != NUL && *(arg + 1) == '=')
    {
	ਜੇ (*arg == '+')
	    op = OP_ADDING;		// "+="
	ਨਹੀਂਤਾਂ ਜੇ (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	ਨਹੀਂਤਾਂ ਜੇ (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    ਵਾਪਸ op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
ਸਥਿਰ ਅੱਖਰ *modeline_whitelist[] =
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
    ਨਿਰਾ
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    ਸਥਿਰ bool
is_modeline_whitelisted(ਅੱਖਰ *name)
{
    ਲਈ (ਪੂਰਨ i = 0; modeline_whitelist[i] != ਨਿਰਾ; i++)
	ਜੇ (STRCMP(name, modeline_whitelist[i]) == 0)
	    ਵਾਪਸ true;
    ਵਾਪਸ false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    ਸਥਿਰ ਪੂਰਨ
validate_opt_idx(
	ਪੂਰਨ opt_idx,
	ਪੂਰਨ opt_flags,
	long_u flags,
	ਅੱਖਰ **errmsg,
	ਸੈੱਟ_ਅਗੇਤਰ_T ਅਗੇਤਰ)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    ਜੇ ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	ਵਾਪਸ FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    ਜੇ ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	ਵਾਪਸ FAIL;

    // Disallow changing some options from modelines.
    ਜੇ (opt_flags & OPT_MODELINE)
    {
	ਜੇ (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_ਵਿੱਚ_modeline;
	    ਵਾਪਸ FAIL;
	}
	ਜੇ ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_ਵਿੱਚ_modeline_when_modelineexpr_is_off;
	    ਵਾਪਸ FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	ਜੇ (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    ਜੇ (options[opt_idx].indir == PV_ML && ਅਗੇਤਰ == ਅਗੇਤਰ_NO)
		ਵਾਪਸ OK;
	    ਨਹੀਂਤਾਂ ਜੇ (!is_modeline_whitelisted(options[opt_idx].fullname))
		ਵਾਪਸ FAIL;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	ਜੇ (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    ਜੇ (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_ਵਿੱਚ_sandbox;
	ਵਾਪਸ FAIL;
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    ਸਥਿਰ char_u *
stropt_get_default_val(
    ਪੂਰਨ		opt_idx,
    char_u	*varp,
    ਪੂਰਨ		flags,
    ਪੂਰਨ		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    ਜੇ ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (gui.ਵਿੱਚ_use)
	    newval = gui_bg_default();
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    newval = ਪਦ_bg_default();
    }
    ਨਹੀਂਤਾਂ ਜੇ ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    ਜੇ (newval == ਨਿਰਾ)
	newval = ਖਾਲੀ_ਥਾਵਾਂ_option;
    ਨਹੀਂਤਾਂ
    {
	char_u *s = option_expand(opt_idx, newval);
	ਜੇ (s == ਨਿਰਾ)
	    s = newval;
	newval = vim_strsave(s);
    }

    ਵਾਪਸ newval;
}

/*
 * Convert the 'backspace' option number value to a string: for adding,
 * prepending and removing string.
 */
    ਸਥਿਰ ਖਾਲੀ
opt_backspace_nr2str(
    char_u	*varp,
    char_u	**origval_p,
    char_u	**origval_l_p,
    char_u	**origval_g_p,
    char_u	**oldval_p)
{
    ਪੂਰਨ		i = getdigits((char_u **)varp);

    ਚੋਣ (i)
    {
	ਹਾਲਤ 0:
	    *(char_u **)varp = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    ਰੋਕੋ;
	ਹਾਲਤ 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    ਰੋਕੋ;
	ਹਾਲਤ 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    ਰੋਕੋ;
	ਹਾਲਤ 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    ਰੋਕੋ;
    }
    vim_free(*oldval_p);
    ਜੇ (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    ਜੇ (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    ਜੇ (*origval_g_p == *oldval_p)
	*origval_g_p = *(char_u **)varp;
    *oldval_p = *(char_u **)varp;
}

/*
 * Convert the 'whichwrap' option number value to a string, for backwards
 * compatibility with Vim 3.0.
 * Note: 'argp' is a pointer to a char_u pointer and is updated.
 */
    ਸਥਿਰ char_u *
opt_whichwrap_nr2str(char_u **argp, char_u *whichwrap)
{
    size_t  len = 0;

    *whichwrap = NUL;
    ਪੂਰਨ i = getdigits(argp);
    ਜੇ (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    ਜੇ (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    ਜੇ (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    ਜੇ (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    ਜੇ (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    ਜੇ (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    ਵਾਪਸ whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    ਸਥਿਰ char_u *
stropt_copy_ਮੁੱਲ(
    char_u	*origval,
    char_u	**argp,
    ਸੈੱਟ_op_T	op,
    ਪੂਰਨ		flags ਅਣਵਰਤਿਆ)
{
    char_u	*arg = *argp;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	newlen;
    char_u	*newval;
    char_u	*s = ਨਿਰਾ;

    // get a bit too much
    newlen = (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(arg) + 1;
    ਜੇ (op != OP_NONE)
	newlen += (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(origval) + 1;
    newval = alloc(newlen);
    ਜੇ (newval == ਨਿਰਾ)  // out of mem, don't change
	ਵਾਪਸ ਨਿਰਾ;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    ਜਦੋਂ (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	ਪੂਰਨ i;

	ਜੇ (*arg == '\\' && arg[1] != NUL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#ਜੇ_ਅੰਤ
	   )
	    ++arg;	// remove backslash
	ਜੇ (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	ਨਹੀਂਤਾਂ
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    ਵਾਪਸ newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    ਸਥਿਰ char_u *
stropt_expand_envvar(
    ਪੂਰਨ		opt_idx,
    char_u	*origval,
    char_u	*newval,
    ਸੈੱਟ_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    ਜੇ (s == ਨਿਰਾ)
	ਵਾਪਸ newval;

    vim_free(newval);
    ਬਿਨਾਨਿਸ਼ਾਨੀ newlen = (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(s) + 1;
    ਜੇ (op != OP_NONE)
	newlen += (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(origval) + 1;

    newval = alloc(newlen);
    ਜੇ (newval == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    STRCPY(newval, s);

    ਵਾਪਸ newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    ਸਥਿਰ ਖਾਲੀ
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    ਸੈੱਟ_op_T	op,
    ਪੂਰਨ		flags)
{
    ਪੂਰਨ		len = 0;

    ਪੂਰਨ comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    ਜੇ (op == OP_ADDING)
    {
	len = (ਪੂਰਨ)STRLEN(origval);
	// strip a trailing comma, would get 2
	ਜੇ (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    ਨਹੀਂਤਾਂ
    {
	len = (ਪੂਰਨ)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    ਜੇ (comma)
	newval[len] = ',';
}

/*
 * Remove a value from a string option.  Copy string option value in "origval"
 * to "newval" and then remove the string "strval" of length "len".
 */
    ਸਥਿਰ ਖਾਲੀ
stropt_remove_val(
    char_u	*origval,
    char_u	*newval,
    ਪੂਰਨ		flags,
    char_u	*strval,
    ਪੂਰਨ		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    ਜੇ (*strval)
    {
	// may need to remove a comma
	ਜੇ (flags & P_COMMA)
	{
	    ਜੇ (strval == origval)
	    {
		// include comma after string
		ਜੇ (strval[len] == ',')
		    ++len;
	    }
	    ਨਹੀਂਤਾਂ
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
    ਸਥਿਰ char_u *
ਲੱਭੋ_ਕੁੰਜੀ_ਵਸਤੂ(char_u *src, char_u *ਕੁੰਜੀ, ਪੂਰਨ keylen, ਪੂਰਨ *itemlenp)
{
    char_u  *p = src;

    ਜਦੋਂ (*p != NUL)
    {
	// Check if this item starts with the same key
	ਜੇ ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, ਕੁੰਜੀ, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *ਸਿਰਾ = vim_strchr(p, ',');
	    ਜੇ (ਸਿਰਾ == ਨਿਰਾ)
		ਸਿਰਾ = p + STRLEN(p);
	    *itemlenp = (ਪੂਰਨ)(ਸਿਰਾ - p);
	    ਵਾਪਸ p;
	}
	++p;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    ਸਥਿਰ ਖਾਲੀ
remove_comma_ਵਸਤੂ(char_u *str, char_u *ਵਸਤੂ, ਪੂਰਨ itemlen)
{
    ਜੇ (ਵਸਤੂ[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(ਵਸਤੂ, ਵਸਤੂ + itemlen + 1);
    ਨਹੀਂਤਾਂ ਜੇ (ਵਸਤੂ > str && *(ਵਸਤੂ - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(ਵਸਤੂ - 1, ਵਸਤੂ + itemlen);
    ਨਹੀਂਤਾਂ
	// Only item
	*ਵਸਤੂ = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    ਸਥਿਰ ਖਾਲੀ
remove_ਕੁੰਜੀ_ਵਸਤੂ(char_u *str, char_u *ਕੁੰਜੀ, ਪੂਰਨ keylen, char_u *skip)
{
    ਪੂਰਨ	    itemlen;
    char_u  *ਮਿਲਿਆ;

    ਜਦੋਂ ((ਮਿਲਿਆ = ਲੱਭੋ_ਕੁੰਜੀ_ਵਸਤੂ(str, ਕੁੰਜੀ, keylen, &itemlen)) != ਨਿਰਾ)
    {
	ਜੇ (ਮਿਲਿਆ == skip)
	{
	    // Search for the next match after this one.
	    char_u *ਅਗਲਾ = ਮਿਲਿਆ + itemlen;
	    ਜੇ (*ਅਗਲਾ == ',')
		++ਅਗਲਾ;
	    ਮਿਲਿਆ = ਲੱਭੋ_ਕੁੰਜੀ_ਵਸਤੂ(ਅਗਲਾ, ਕੁੰਜੀ, keylen, &itemlen);
	    ਜੇ (ਮਿਲਿਆ == ਨਿਰਾ)
		ਰੋਕੋ;
	}

	remove_comma_ਵਸਤੂ(str, ਮਿਲਿਆ, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    ਸਥਿਰ ਖਾਲੀ
append_ਵਸਤੂ(char_u *str, char_u *ਵਸਤੂ, ਪੂਰਨ ਵਸਤੂ_len)
{
    ਪੂਰਨ len = (ਪੂਰਨ)STRLEN(str);

    ਜੇ (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, ਵਸਤੂ, (size_t)ਵਸਤੂ_len);
    str[len + ਵਸਤੂ_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    ਸਥਿਰ ਖਾਲੀ
prepend_ਵਸਤੂ(char_u *str, char_u *ਵਸਤੂ, ਪੂਰਨ ਵਸਤੂ_len)
{
    ਪੂਰਨ len = (ਪੂਰਨ)STRLEN(str);
    ਪੂਰਨ comma = (len > 0) ? 1 : 0;

    mch_memmove(str + ਵਸਤੂ_len + comma, str, (size_t)len + 1);
    mch_memmove(str, ਵਸਤੂ, (size_t)ਵਸਤੂ_len);
    ਜੇ (comma)
	str[ਵਸਤੂ_len] = ',';
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
    ਸਥਿਰ bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    ਸੈੱਟ_op_T	op,
    ਪੂਰਨ		flags ਅਣਵਰਤਿਆ)
{
    char_u  *p;
    char_u  *ਵਸਤੂ_ਸ਼ੁਰੂ;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    ਜੇ (vim_strchr(newval, ':') == ਨਿਰਾ && vim_strchr(newval, ',') == ਨਿਰਾ)
	ਵਾਪਸ false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    ਜੇ (newval_copy == ਨਿਰਾ)
	ਵਾਪਸ false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    ਵਸਤੂ_ਸ਼ੁਰੂ = newval_copy;
    ਲਈ (;;)
    {
	p = vim_strchr(ਵਸਤੂ_ਸ਼ੁਰੂ, ',');
	ਪੂਰਨ ਵਸਤੂ_len = (p == ਨਿਰਾ)
			    ? (ਪੂਰਨ)STRLEN(ਵਸਤੂ_ਸ਼ੁਰੂ) : (ਪੂਰਨ)(p - ਵਸਤੂ_ਸ਼ੁਰੂ);

	ਜੇ (ਵਸਤੂ_len > 0)
	{
	    char_u *colon = vim_strchr(ਵਸਤੂ_ਸ਼ੁਰੂ, ':');
	    ਜੇ (colon != ਨਿਰਾ && colon < ਵਸਤੂ_ਸ਼ੁਰੂ + ਵਸਤੂ_len)
	    {
		ਪੂਰਨ keylen = (ਪੂਰਨ)(colon - ਵਸਤੂ_ਸ਼ੁਰੂ) + 1;

		ਜੇ (op == OP_ADDING || op == OP_PREPENDING)
		{
		    ਪੂਰਨ old_itemlen;
		    char_u *ਮਿਲਿਆ = ਲੱਭੋ_ਕੁੰਜੀ_ਵਸਤੂ(newval, ਵਸਤੂ_ਸ਼ੁਰੂ,
						       keylen, &old_itemlen);
		    ਜੇ (ਮਿਲਿਆ != ਨਿਰਾ)
		    {
			ਜੇ (old_itemlen == ਵਸਤੂ_len
				&& STRNCMP(ਮਿਲਿਆ, ਵਸਤੂ_ਸ਼ੁਰੂ,
							 ਵਸਤੂ_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_ਕੁੰਜੀ_ਵਸਤੂ(newval, ਵਸਤੂ_ਸ਼ੁਰੂ,
							   keylen, ਮਿਲਿਆ);
			}
			ਨਹੀਂਤਾਂ
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_ਕੁੰਜੀ_ਵਸਤੂ(newval, ਵਸਤੂ_ਸ਼ੁਰੂ,
							    keylen, ਨਿਰਾ);
			    ਜੇ (op == OP_PREPENDING)
				prepend_ਵਸਤੂ(newval, ਵਸਤੂ_ਸ਼ੁਰੂ, ਵਸਤੂ_len);
			    ਨਹੀਂਤਾਂ
				append_ਵਸਤੂ(newval, ਵਸਤੂ_ਸ਼ੁਰੂ, ਵਸਤੂ_len);
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// New item.
			ਜੇ (op == OP_PREPENDING)
			    prepend_ਵਸਤੂ(newval, ਵਸਤੂ_ਸ਼ੁਰੂ, ਵਸਤੂ_len);
			ਨਹੀਂਤਾਂ
			    append_ਵਸਤੂ(newval, ਵਸਤੂ_ਸ਼ੁਰੂ, ਵਸਤੂ_len);
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (op == OP_REMOVING)
		    remove_ਕੁੰਜੀ_ਵਸਤੂ(newval, ਵਸਤੂ_ਸ਼ੁਰੂ, keylen, ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *ਮਿਲਿਆ = ਲੱਭੋ_dup_ਵਸਤੂ(newval, ਵਸਤੂ_ਸ਼ੁਰੂ,
							   ਵਸਤੂ_len, P_COMMA);
		    ਜੇ (ਮਿਲਿਆ == ਨਿਰਾ)
		    {
			// New item.
			ਜੇ (op == OP_PREPENDING)
			    prepend_ਵਸਤੂ(newval, ਵਸਤੂ_ਸ਼ੁਰੂ, ਵਸਤੂ_len);
			ਨਹੀਂਤਾਂ
			    append_ਵਸਤੂ(newval, ਵਸਤੂ_ਸ਼ੁਰੂ, ਵਸਤੂ_len);
		    }
		    // else: exact duplicate — do nothing
		}
		ਨਹੀਂਤਾਂ ਜੇ (op == OP_REMOVING)
		{
		    char_u *ਮਿਲਿਆ = ਲੱਭੋ_dup_ਵਸਤੂ(newval, ਵਸਤੂ_ਸ਼ੁਰੂ,
							   ਵਸਤੂ_len, P_COMMA);
		    ਜੇ (ਮਿਲਿਆ != ਨਿਰਾ)
			remove_comma_ਵਸਤੂ(newval, ਮਿਲਿਆ, ਵਸਤੂ_len);
		}
	    }
	}

	ਜੇ (p == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਵਸਤੂ_ਸ਼ੁਰੂ = p + 1;
    }

    vim_free(newval_copy);

    ਵਾਪਸ true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    ਸਥਿਰ ਖਾਲੀ
stropt_remove_dupflags(char_u *newval, ਪੂਰਨ flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    ਜਦੋਂ (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	ਜੇ (flags & P_ONECOMMA)
	{
	    ਜੇ (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != ਨਿਰਾ)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		ਜਾਰੀ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != ਨਿਰਾ)
	    {
		STRMOVE(s, s + 1);
		ਜਾਰੀ;
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
    ਸਥਿਰ char_u *
stropt_get_newval(
    ਪੂਰਨ		nextchar,
    ਪੂਰਨ		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    ਸੈੱਟ_op_T    *op_arg,
    ਪੂਰਨ		flags,
    ਪੂਰਨ		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    ਸੈੱਟ_op_T    op = *op_arg;
    char_u	*save_arg = ਨਿਰਾ;
    char_u	*newval;
    char_u	*s = ਨਿਰਾ;
    char_u	whichwrap[80];

    ਜੇ (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    ਨਹੀਂਤਾਂ ਜੇ (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    ਨਹੀਂਤਾਂ
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	ਜੇ (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	ਨਹੀਂਤਾਂ ਜੇ (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	ਨਹੀਂਤਾਂ ਜੇ (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	ਨਹੀਂਤਾਂ ਜੇ (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_ਮੁੱਲ(origval, &arg, op, flags);
	ਜੇ (newval == ਨਿਰਾ)
	    ਜਾਓ ਪੂਰਾ;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	ਜੇ (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    ਜੇ (newval == ਨਿਰਾ)
		ਜਾਓ ਪੂਰਾ;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	ਜੇ ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	ਨਹੀਂਤਾਂ
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    ਪੂਰਨ len = 0;
	    ਜੇ (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (ਪੂਰਨ)STRLEN(newval);
		s = ਲੱਭੋ_dup_ਵਸਤੂ(origval, newval, len, flags);

		// do not add if already there
		ਜੇ ((op == OP_ADDING || op == OP_PREPENDING) && s != ਨਿਰਾ)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		ਜੇ (s == ਨਿਰਾ)
		    s = origval + (ਪੂਰਨ)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    ਜੇ (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    ਨਹੀਂਤਾਂ ਜੇ (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	ਜੇ (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

ਪੂਰਾ:
    ਜੇ (save_arg != ਨਿਰਾ)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    ਵਾਪਸ newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    ਸਥਿਰ ਪੂਰਨ
do_ਸੈੱਟ_option_string(
	ਪੂਰਨ	    opt_idx,
	ਪੂਰਨ	    opt_flags,
	char_u	    **argp,
	ਪੂਰਨ	    nextchar,
	ਸੈੱਟ_op_T    op_arg,
	long_u	    flags,
	ਪੂਰਨ	    cp_val,
	char_u	    *varp_arg,
	ਅੱਖਰ	    *errbuf,
	size_t	    errbuflen,
	ਪੂਰਨ	    *ਮੁੱਲ_checked,
	ਅੱਖਰ	    **errmsg)
{
    char_u	*arg = *argp;
    ਸੈੱਟ_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = ਨਿਰਾ; // previous value if *varp
    char_u	*newval;
    char_u	*origval = ਨਿਰਾ;
    char_u	*origval_l = ਨਿਰਾ;
    char_u	*origval_g = ਨਿਰਾ;
#ਜੇ defined(FEAT_EVAL)
    char_u	*saved_origval = ਨਿਰਾ;
    char_u	*saved_origval_l = ਨਿਰਾ;
    char_u	*saved_origval_g = ਨਿਰਾ;
    char_u	*saved_newval = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    ਜੇ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((ਪੂਰਨ)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    ਜੇ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	ਜੇ (((ਪੂਰਨ)options[opt_idx].indir & PV_BOTH)
						  && origval_l == ਖਾਲੀ_ਥਾਵਾਂ_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    ਜੇ (((ਪੂਰਨ)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    ਨਹੀਂਤਾਂ
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    ਜੇ (newval == ਨਿਰਾ)
	*(char_u **)(varp) = ਖਾਲੀ_ਥਾਵਾਂ_option;

#ਜੇ defined(FEAT_EVAL)
    ਜੇ (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_ਕੁੰਜੀ
# endif
		      && origval != ਨਿਰਾ && newval != ਨਿਰਾ)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	ਜੇ (origval_l != ਨਿਰਾ)
	    saved_origval_l = vim_strsave(origval_l);
	ਜੇ (origval_g != ਨਿਰਾ)
	    saved_origval_g = vim_strsave(origval_g);
    }
#ਜੇ_ਅੰਤ

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	ਪੂਰਨ	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	ਜੇ ((opt_flags & OPT_MODELINE)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
	      || sandbox != 0
#ਜੇ_ਅੰਤ
	      || (op != OP_NONE && (*p & P_INSECURE)))
	    secure = 1;

	// Handle side effects, and set the global value for ":set" on local
	// options. Note: when setting 'syntax' or 'filetype' autocommands may
	// be triggered that can cause havoc.
	*errmsg = did_ਸੈੱਟ_string_option(
			opt_idx, (char_u **)varp, oldval, newval, errbuf,
			errbuflen, opt_flags, op, ਮੁੱਲ_checked);

	secure = secure_saved;
    }

#ਜੇ defined(FEAT_EVAL)
    ਜੇ (*errmsg == ਨਿਰਾ)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#ਜੇ_ਅੰਤ

    *argp = arg;
    ਵਾਪਸ *errmsg == ਨਿਰਾ ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    ਸਥਿਰ ਅੱਖਰ *
do_ਸੈੱਟ_option_bool(
    ਪੂਰਨ		opt_idx,
    ਪੂਰਨ		opt_flags,
    ਸੈੱਟ_ਅਗੇਤਰ_T ਅਗੇਤਰ,
    long_u	flags,
    char_u	*varp,
    ਪੂਰਨ		nextchar,
    ਪੂਰਨ		afterchar,
    ਪੂਰਨ		cp_val)

{
    varnumber_T	ਮੁੱਲ;

    ਜੇ (nextchar == '=' || nextchar == ':')
	ਵਾਪਸ e_invalid_ਦਲੀਲ;
    ਜੇ (opt_idx < 0 || varp == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    ਜੇ (nextchar == '!')
	ਮੁੱਲ = *(ਪੂਰਨ *)(varp) ^ 1;
    ਨਹੀਂਤਾਂ ਜੇ (nextchar == '&')
	ਮੁੱਲ = (ਪੂਰਨ)(ਲੰਮਾ)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    ਨਹੀਂਤਾਂ ਜੇ (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	ਜੇ ((ਪੂਰਨ *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    ਮੁੱਲ = -1;
	ਨਹੀਂਤਾਂ ਜੇ ((ਪੂਰਨ *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    ਮੁੱਲ = -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FSYNC
	ਨਹੀਂਤਾਂ ਜੇ ((ਪੂਰਨ *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    ਮੁੱਲ = -1;
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ
	    ਮੁੱਲ = *(ਪੂਰਨ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    ਨਹੀਂਤਾਂ
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	ਜੇ (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    ਵਾਪਸ e_trailing_characters;
	ਜੇ (ਅਗੇਤਰ == ਅਗੇਤਰ_INV)
	    ਮੁੱਲ = *(ਪੂਰਨ *)(varp) ^ 1;
	ਨਹੀਂਤਾਂ
	    ਮੁੱਲ = ਅਗੇਤਰ == ਅਗੇਤਰ_NO ? 0 : 1;
    }

    ਵਾਪਸ ਸੈੱਟ_bool_option(opt_idx, varp, (ਪੂਰਨ)ਮੁੱਲ, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    ਸਥਿਰ ਅੱਖਰ *
do_ਸੈੱਟ_option_numeric(
    ਪੂਰਨ		opt_idx,
    ਪੂਰਨ		opt_flags,
    char_u	**argp,
    ਪੂਰਨ		nextchar,
    ਸੈੱਟ_op_T	op,
    long_u	flags,
    ਪੂਰਨ		cp_val,
    char_u	*varp,
    ਅੱਖਰ	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		ਮੁੱਲ;
    ਪੂਰਨ			i;
    ਅੱਖਰ		*errmsg = ਨਿਰਾ;

    ਜੇ (opt_idx < 0 || varp == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;  // "cannot happen"
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
    ਜੇ (nextchar == '&')
	ਮੁੱਲ = (ਲੰਮਾ)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    ਨਹੀਂਤਾਂ ਜੇ (nextchar == '<')
    {
	ਜੇ ((ਲੰਮਾ *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    ਮੁੱਲ = NO_LOCAL_UNDOLEVEL;
	ਨਹੀਂਤਾਂ ਜੇ (opt_flags == OPT_LOCAL
		    && ((ਲੰਮਾ *)varp == &curwin->w_p_siso
		     || (ਲੰਮਾ *)varp == &curwin->w_p_so
		     || (ਲੰਮਾ *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    ਮੁੱਲ = -1;
	ਨਹੀਂਤਾਂ
	    ਮੁੱਲ = *(ਲੰਮਾ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    ਨਹੀਂਤਾਂ ਜੇ (((ਲੰਮਾ *)varp == &p_wc || (ਲੰਮਾ *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	ਮੁੱਲ = string_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ(arg, FALSE);
	ਜੇ (ਮੁੱਲ == 0)
	{
	    errmsg = e_invalid_ਦਲੀਲ;
	    ਜਾਓ skip;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, ਨਿਰਾ, &i, STR2NR_ALL, &ਮੁੱਲ, ਨਿਰਾ, 0, TRUE, ਨਿਰਾ);
	ਜੇ (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_ਸੰਖਿਆ_required_after_equal;
	    ਜਾਓ skip;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	errmsg = e_ਸੰਖਿਆ_required_after_equal;
	ਜਾਓ skip;
    }

    ਜੇ (op == OP_ADDING)
	ਮੁੱਲ = *(ਲੰਮਾ *)varp + ਮੁੱਲ;
    ਨਹੀਂਤਾਂ ਜੇ (op == OP_PREPENDING)
	ਮੁੱਲ = *(ਲੰਮਾ *)varp * ਮੁੱਲ;
    ਨਹੀਂਤਾਂ ਜੇ (op == OP_REMOVING)
	ਮੁੱਲ = *(ਲੰਮਾ *)varp - ਮੁੱਲ;

    ਜੇ ((ਲੰਮਾ *)varp == &curwin->w_p_sop && ਮੁੱਲ < -1)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	ਜਾਓ skip;
    }

    errmsg = ਸੈੱਟ_ਅੰਕ_option(opt_idx, varp, ਮੁੱਲ, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    ਵਾਪਸ errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    ਸਥਿਰ ਅੱਖਰ *
do_ਸੈੱਟ_option_keycode(char_u **argp, char_u *ਕੁੰਜੀ_name, ਪੂਰਨ nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    ਜੇ (nextchar == '&')
    {
	ਜੇ (ਜੋੜੋ_termcap_ਇੰਦਰਾਜ(ਕੁੰਜੀ_name, TRUE) == FAIL)
	    ਵਾਪਸ e_not_ਮਿਲਿਆ_ਵਿੱਚ_termcap;
    }
    ਨਹੀਂਤਾਂ
    {
	++arg; // jump to after the '=' or ':'
	ਲਈ (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    ਜੇ (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	ਜੋੜੋ_termcode(ਕੁੰਜੀ_name, arg, FALSE);
	*p = nextchar;
    }
    ਜੇ (ਭਰਿਆ_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Set an option to a new value.
 */
    ਸਥਿਰ ਅੱਖਰ *
do_ਸੈੱਟ_option_ਮੁੱਲ(
    ਪੂਰਨ		opt_idx,
    ਪੂਰਨ		opt_flags,
    char_u	**argp,
    ਸੈੱਟ_ਅਗੇਤਰ_T ਅਗੇਤਰ,
    ਸੈੱਟ_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*ਕੁੰਜੀ_name,
    ਪੂਰਨ		nextchar,
    ਪੂਰਨ		afterchar,
    ਪੂਰਨ		cp_val,
    ਪੂਰਨ		*stopopteval,
    ਅੱਖਰ	*errbuf,
    size_t	errbuflen)
{
    ਪੂਰਨ		ਮੁੱਲ_checked = FALSE;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;
    char_u	*arg = *argp;

    ਜੇ (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_ਸੈੱਟ_option_bool(opt_idx, opt_flags, ਅਗੇਤਰ, flags, varp,
						nextchar, afterchar, cp_val);
	ਜੇ (errmsg != ਨਿਰਾ)
	    ਜਾਓ skip;
    }
    ਨਹੀਂਤਾਂ
    {
	// numeric or string option
	ਜੇ (vim_strchr((char_u *)"=:&<", nextchar) == ਨਿਰਾ
		|| ਅਗੇਤਰ != ਅਗੇਤਰ_NONE)
	{
	    errmsg = e_invalid_ਦਲੀਲ;
	    ਜਾਓ skip;
	}

	ਜੇ (flags & P_ਅੰਕ)
	{
	    // numeric option
	    errmsg = do_ਸੈੱਟ_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    ਜੇ (errmsg != ਨਿਰਾ)
		ਜਾਓ skip;
	}
	ਨਹੀਂਤਾਂ ਜੇ (opt_idx >= 0)
	{
	    // string option
	    ਜੇ (do_ਸੈੱਟ_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&ਮੁੱਲ_checked, &errmsg) == FAIL)
	    {
		ਜੇ (errmsg != ਨਿਰਾ)
		    ਜਾਓ skip;
		*stopopteval = TRUE;
		ਜਾਓ skip;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // key code option
	    errmsg = do_ਸੈੱਟ_option_keycode(&arg, ਕੁੰਜੀ_name, nextchar);
	    ਜੇ (errmsg != ਨਿਰਾ)
		ਜਾਓ skip;
	}
    }

    ਜੇ (opt_idx >= 0)
	did_ਸੈੱਟ_option(opt_idx, opt_flags, op == OP_NONE, ਮੁੱਲ_checked);

skip:
    *argp = arg;
    ਵਾਪਸ errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    ਸਥਿਰ ਅੱਖਰ *
do_ਸੈੱਟ_option(
    ਪੂਰਨ		opt_flags,
    char_u	**argp,
    char_u	*arg_ਸ਼ੁਰੂ,
    char_u	**startarg,
    ਪੂਰਨ		*did_show,
    ਪੂਰਨ		*stopopteval,
    ਅੱਖਰ	*errbuf,
    size_t	errbuflen)
{
    ਪੂਰਨ		opt_idx;
    char_u	*arg;
    ਸੈੱਟ_ਅਗੇਤਰ_T ਅਗੇਤਰ;	// no prefix, "no" prefix or "inv" prefix
    ਸੈੱਟ_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	ਕੁੰਜੀ_name[2];
    ਪੂਰਨ		nextchar;	// next non-white char after option name
    ਪੂਰਨ		afterchar;	// character just after option name
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;
    ਪੂਰਨ		ਕੁੰਜੀ;
    ਪੂਰਨ		len;

    ਅਗੇਤਰ = get_option_ਅਗੇਤਰ(argp);
    arg = *argp;

    // find end of name
    ਕੁੰਜੀ = 0;
    ਜੇ (parse_option_name(arg, &opt_idx, &len, &ਕੁੰਜੀ) == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    // remember character after option name
    afterchar = arg[len];

    ਜੇ (ਵਿੱਚ_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	ਜੇ (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != ਨਿਰਾ
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    ਜਾਓ skip;
	}
    }
    ਨਹੀਂਤਾਂ
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	ਜਦੋਂ (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    ਜੇ (op != OP_NONE)
	len++;

    nextchar = arg[len];

    ਜੇ (opt_idx == -1 && ਕੁੰਜੀ == 0)	// found a mismatch: skip
    {
	ਜੇ (ਵਿੱਚ_vim9script() && arg > arg_ਸ਼ੁਰੂ
		&& vim_strchr((char_u *)"!&<", *arg) != ਨਿਰਾ)
	    errmsg = e_no_white_space_allowed_between_option_and;
	ਨਹੀਂਤਾਂ
	    errmsg = e_unknown_option;
	ਜਾਓ skip;
    }

    ਜੇ (opt_idx >= 0)
    {
	ਜੇ (options[opt_idx].var == ਨਿਰਾ)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    ਜੇ (vim_strchr((char_u *)"=:!&<", nextchar) == ਨਿਰਾ
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    ਜਾਓ skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    ਨਹੀਂਤਾਂ
    {
	flags = P_STRING;
	varp = ਨਿਰਾ;
	ਜੇ (ਕੁੰਜੀ < 0)
	{
	    ਕੁੰਜੀ_name[0] = KEY2TERMCAP0(ਕੁੰਜੀ);
	    ਕੁੰਜੀ_name[1] = KEY2TERMCAP1(ਕੁੰਜੀ);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਕੁੰਜੀ_name[0] = KS_ਕੁੰਜੀ;
	    ਕੁੰਜੀ_name[1] = (ਕੁੰਜੀ & 0xff);
	}
    }

    // Make sure the option value can be changed.
    ਜੇ (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, ਅਗੇਤਰ) == FAIL)
	ਜਾਓ skip;

    ਪੂਰਨ cp_val = p_cp;
    ਜੇ (vim_strchr((char_u *)"?=:!&<", nextchar) != ਨਿਰਾ)
    {
	arg += len;
	ਜੇ (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    ਜੇ (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    ਨਹੀਂਤਾਂ		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	ਜੇ (vim_strchr((char_u *)"?!&<", nextchar) != ਨਿਰਾ
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    ਜਾਓ skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    ਜੇ (nextchar == '?'
	    || (ਅਗੇਤਰ == ਅਗੇਤਰ_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == ਨਿਰਾ
		&& !(flags & P_BOOL)))
    {
	// print value
	ਜੇ (*did_show)
	    ਸੁਨੇਹਾ_putchar('\n');	    // cursor below last one
	ਨਹੀਂਤਾਂ
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	ਜੇ (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਜੇ (p_verbose > 0)
	    {
		// Mention where the option was last set.
		ਜੇ (varp == options[opt_idx].var)
		    ਆਖਰੀ_ਸੈੱਟ_ਸੁਨੇਹਾ(options[opt_idx].script_ctx);
		ਨਹੀਂਤਾਂ ਜੇ ((ਪੂਰਨ)options[opt_idx].indir & PV_WIN)
		    ਆਖਰੀ_ਸੈੱਟ_ਸੁਨੇਹਾ(curwin->w_p_script_ctx[
			    (ਪੂਰਨ)options[opt_idx].indir & PV_ਨਕਾਬ]);
		ਨਹੀਂਤਾਂ ਜੇ ((ਪੂਰਨ)options[opt_idx].indir & PV_BUF)
		    ਆਖਰੀ_ਸੈੱਟ_ਸੁਨੇਹਾ(curbuf->b_p_script_ctx[
			    (ਪੂਰਨ)options[opt_idx].indir & PV_ਨਕਾਬ]);
	    }
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u	    *p;

	    p = ਲੱਭੋ_termcode(ਕੁੰਜੀ_name);
	    ਜੇ (p == ਨਿਰਾ)
	    {
		errmsg = e_ਕੁੰਜੀ_ਕੋਡ_not_ਸੈੱਟ;
		ਜਾਓ skip;
	    }
	    ਨਹੀਂਤਾਂ
		(ਖਾਲੀ)show_one_termcode(ਕੁੰਜੀ_name, p, TRUE);
	}
	ਜੇ (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    ਨਹੀਂਤਾਂ
    {
	errmsg = do_ਸੈੱਟ_option_ਮੁੱਲ(opt_idx, opt_flags, &arg, ਅਗੇਤਰ, op,
					flags, varp, ਕੁੰਜੀ_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    ਵਾਪਸ errmsg;
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
    ਪੂਰਨ
do_ਸੈੱਟ(
    char_u	*arg_ਸ਼ੁਰੂ,	// option string (may be written to!)
    ਪੂਰਨ		opt_flags)
{
    char_u	*arg = arg_ਸ਼ੁਰੂ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		did_show = FALSE;   // already showed one value

    ਜੇ (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	ਜਾਓ theend;
    }

    ਜਦੋਂ (*arg != NUL)		// loop to process all options
    {
	ਜੇ (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    ਜੇ (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		ਸੈੱਟ_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ		stopopteval = FALSE;
	    ਅੱਖਰ	*errmsg = ਨਿਰਾ;
	    ਅੱਖਰ	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_ਸੈੱਟ_option(opt_flags, &arg, arg_ਸ਼ੁਰੂ, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    ਜੇ (stopopteval)
		ਰੋਕੋ;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    ਲਈ (i = 0; i < 2 ; ++i)
	    {
		ਜਦੋਂ (*arg != NUL && !VIM_ISWHITE(*arg))
		    ਜੇ (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		ਜੇ (*arg != '=')
		    ਰੋਕੋ;
	    }

	    ਜੇ (errmsg != ਨਿਰਾ)
	    {
		i = vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		ਜੇ (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((ਅੱਖਰ *)IObuff);	// show error highlighted
		--no_wait_return;

		ਵਾਪਸ FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    ਜੇ (silent_ਬਹੁਲਕ && did_show)
    {
	// After displaying option values in silent mode.
	silent_ਬਹੁਲਕ = FALSE;
	info_ਸੁਨੇਹਾ = TRUE;	// use mch_msg(), not mch_errmsg()
	ਸੁਨੇਹਾ_putchar('\n');
	cursor_on();		// msg_start() switches it off
	ਬਾਹਰਲਾ_flush();
	silent_ਬਹੁਲਕ = TRUE;
	info_ਸੁਨੇਹਾ = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    ਵਾਪਸ OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    ਖਾਲੀ
did_ਸੈੱਟ_option(
    ਪੂਰਨ	    opt_idx,
    ਪੂਰਨ	    opt_flags,	    // possibly with OPT_MODELINE
    ਪੂਰਨ	    new_ਮੁੱਲ_2,	    // value was replaced completely
    ਪੂਰਨ	    ਮੁੱਲ_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = ਨਿਰਾ;
    ਪੂਰਨ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_ਸੈੱਟ;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    ਜੇ (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    ਜੇ (!ਮੁੱਲ_checked && (secure
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
	    || sandbox != 0
#ਜੇ_ਅੰਤ
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	ਜੇ (flagsp_local != ਨਿਰਾ)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (new_ਮੁੱਲ_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	ਜੇ (flagsp_local != ਨਿਰਾ)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    ਪੂਰਨ
string_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ(char_u *arg, ਪੂਰਨ multi_byte)
{
    ਜੇ (*arg == '<' && arg[1])
	ਵਾਪਸ ਲੱਭੋ_ਕੁੰਜੀ_option(arg + 1, TRUE);
    ਜੇ (*arg == '^' && arg[1])
    {
	ਪੂਰਨ ਕੁੰਜੀ = Ctrl_chr(arg[1]);
	ਜੇ (ਕੁੰਜੀ == 0)		// ^@ is <Nul>
	    ਕੁੰਜੀ = K_ZERO;
	ਵਾਪਸ ਕੁੰਜੀ;
    }
    ਜੇ (multi_byte)
	ਵਾਪਸ PTR2CHAR(arg);
    ਵਾਪਸ *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    ਖਾਲੀ
did_ਸੈੱਟ_title(ਖਾਲੀ)
{
    ਜੇ (starting != NO_SCREEN
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    && !gui.starting
#ਜੇ_ਅੰਤ
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    ਖਾਲੀ
ਸੈੱਟ_options_bin(
    ਪੂਰਨ		oldval,
    ਪੂਰਨ		newval,
    ਪੂਰਨ		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    ਜੇ (newval)
    {
	ਜੇ (!oldval)		// switched on
	{
	    ਜੇ (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    ਜੇ (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	ਜੇ (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	ਜੇ (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (oldval)		// switched off
    {
	ਜੇ (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	ਜੇ (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#ਜੇ defined(FEAT_EVAL)
    // Remember where the dependent option were reset
    didset_options_sctx(opt_flags, p_bin_dep_opts);
#ਜੇ_ਅੰਤ
}

/*
 * Expand environment variables for some string options.
 * These string options cannot be indirect!
 * If "val" is NULL expand the current value of the option.
 * Return pointer to NameBuff, or NULL when not expanded.
 */
    ਸਥਿਰ char_u *
option_expand(ਪੂਰਨ opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    ਜੇ (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    ਜੇ (val != ਨਿਰਾ && STRLEN(val) > MAXPATHL)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (val == ਨਿਰਾ)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    ਪੂਰਨ esc = var == &p_ਟੈਗ_ਸੂਚੀ || var == &p_ਰਸਤਾ;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : ਨਿਰਾ, FALSE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#ਜੇ_ਅੰਤ
				  ਨਿਰਾ);
    ਜੇ (STRCMP(NameBuff, val) == 0)   // they are the same
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ NameBuff;
}

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    ਸਥਿਰ ਖਾਲੀ
didset_options(ਖਾਲੀ)
{
    // initialize the table for 'iskeyword' et.al.
    (ਖਾਲੀ)init_chartab();

    didset_string_options();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    (ਖਾਲੀ)spell_check_msm();
    (ਖਾਲੀ)spell_check_sps();
    (ਖਾਲੀ)compile_cap_prog(curwin->w_s);
    (ਖਾਲੀ)did_ਸੈੱਟ_spell_option();
#ਜੇ_ਅੰਤ
    // set cedit_key
    (ਖਾਲੀ)did_ਸੈੱਟ_cedit(ਨਿਰਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_ਸੈੱਟ_breakat(ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    ਸਥਿਰ ਖਾਲੀ
didset_options2(ਖਾਲੀ)
{
    // Initialize the highlight_attr[] table.
    (ਖਾਲੀ)highlight_ਬਦਲਿਆ();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (ਖਾਲੀ)ਸੈੱਟ_listchars_option(curwin, curwin->w_p_lcs, TRUE, ਨਿਰਾ, 0);

    // Parse default for 'fillchars'.
    (ਖਾਲੀ)ਸੈੱਟ_fillchars_option(curwin, curwin->w_p_fcs, TRUE, ਨਿਰਾ, 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (ਖਾਲੀ)did_ਸੈੱਟ_clipboard(ਨਿਰਾ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (ਖਾਲੀ)tabstop_ਸੈੱਟ(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (ਖਾਲੀ)tabstop_ਸੈੱਟ(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#ਜੇ_ਅੰਤ
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    ਖਾਲੀ
check_options(ਖਾਲੀ)
{
    ਪੂਰਨ		opt_idx;

    ਲਈ (opt_idx = 0; options[opt_idx].fullname != ਨਿਰਾ; opt_idx++)
	ਜੇ ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != ਨਿਰਾ)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    ਪੂਰਨ
get_ਪਦ_opt_idx(char_u **p)
{
    ਪੂਰਨ opt_idx;

    ਲਈ (opt_idx = 1; options[opt_idx].fullname != ਨਿਰਾ; opt_idx++)
	ਜੇ (options[opt_idx].var == (char_u *)p)
	    ਵਾਪਸ opt_idx;
    ਵਾਪਸ -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    ਪੂਰਨ
ਸੈੱਟ_ਪਦ_option_alloced(char_u **p)
{
    ਪੂਰਨ		opt_idx = get_ਪਦ_opt_idx(p);

    ਜੇ (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    ਵਾਪਸ opt_idx;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    ਪੂਰਨ
was_ਸੈੱਟ_insecurely(win_T *wp, char_u *opt, ਪੂਰਨ opt_flags)
{
    ਪੂਰਨ	    idx = findoption(opt);
    long_u  *flagp;

    ਜੇ (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	ਵਾਪਸ (*flagp & P_INSECURE) != 0;
    }
    ਅੰਦਰੂਨੀ_error("was_set_insecurely()");
    ਵਾਪਸ -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    ਸਥਿਰ long_u *
insecure_flag(win_T *wp, ਪੂਰਨ opt_idx, ਪੂਰਨ opt_flags)
{
    ਜੇ (opt_flags & OPT_LOCAL)
	ਚੋਣ ((ਪੂਰਨ)options[opt_idx].indir)
	{
	    ਹਾਲਤ PV_WRAP:	ਵਾਪਸ &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    ਹਾਲਤ PV_STL:	ਵਾਪਸ &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    ਹਾਲਤ PV_FDE:	ਵਾਪਸ &wp->w_p_fde_flags;
	    ਹਾਲਤ PV_FDT:	ਵਾਪਸ &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    ਹਾਲਤ PV_BEXPR:	ਵਾਪਸ &wp->w_ਬਫਰ->b_p_bexpr_flags;
#  endif
	    ਹਾਲਤ PV_INDE:	ਵਾਪਸ &wp->w_ਬਫਰ->b_p_inde_flags;
	    ਹਾਲਤ PV_FEX:	ਵਾਪਸ &wp->w_ਬਫਰ->b_p_fex_flags;
#  ifdef FEAT_ਲੱਭੋ_ID
	    ਹਾਲਤ PV_INEX:	ਵਾਪਸ &wp->w_ਬਫਰ->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    ਹਾਲਤ PV_CPT:	ਵਾਪਸ &wp->w_ਬਫਰ->b_p_cpt_flags;
#  endif
# endif
	}
    ਨਹੀਂਤਾਂ
	// For global value of window-local options, use flags in w_allbuf_opt.
	ਚੋਣ ((ਪੂਰਨ)options[opt_idx].indir)
	{
	    ਹਾਲਤ PV_WRAP:	ਵਾਪਸ &wp->w_allbuf_opt.wo_wrap_flags;
# ਜੇ defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    ਹਾਲਤ PV_FDE:	ਵਾਪਸ &wp->w_allbuf_opt.wo_fde_flags;
	    ਹਾਲਤ PV_FDT:	ਵਾਪਸ &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    ਵਾਪਸ &options[opt_idx].flags;
}
#ਜੇ_ਅੰਤ

/*
 * Redraw the window title and/or tab page text later.
 */
    ਖਾਲੀ
redraw_titles(ਖਾਲੀ)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    ਪੂਰਨ
valid_name(char_u *val, ਅੱਖਰ *allowed)
{
    char_u *s;

    ਲਈ (s = val; *s != NUL; ++s)
	ਜੇ (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == ਨਿਰਾ)
	    ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    ਖਾਲੀ
ਸੈੱਟ_option_sctx_idx(ਪੂਰਨ opt_idx, ਪੂਰਨ opt_flags, sctx_T script_ctx)
{
    ਪੂਰਨ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    ਪੂਰਨ		indir = (ਪੂਰਨ)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    ਜੇ (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    ਜੇ (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_VIM9;
    ਜੇ (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    ਜੇ (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    ਜੇ (both || (opt_flags & OPT_LOCAL))
    {
	ਜੇ (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_ਨਕਾਬ] = new_script_ctx;
	ਨਹੀਂਤਾਂ ਜੇ (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_ਨਕਾਬ] = new_script_ctx;
	    ਜੇ (both)
		// also setting the "all buffers" value
		curwin->w_allbuf_opt.wo_script_ctx[indir & PV_ਨਕਾਬ] =
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
is_option_ਮੁੱਲ_vim9(ਪੂਰਨ opt_idx, ਪੂਰਨ opt_flags)
{
    ਪੂਰਨ		indir = (ਪੂਰਨ)options[opt_idx].indir;
    sctx_T	*sctx = ਨਿਰਾ;

    ਜੇ ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    ਜੇ ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	ਜੇ (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_ਨਕਾਬ];
	ਨਹੀਂਤਾਂ ਜੇ (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_ਨਕਾਬ];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    ਵਾਪਸ !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_ਸੰਸਕਰਣ >= SCRIPT_ਸੰਸਕਰਣ_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(ਅੱਖਰ *name)
{
    ਪੂਰਨ idx = findoption((char_u *)name);

    ਜੇ (idx >= 0)
	ਵਾਪਸ &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    ਖਾਲੀ
ਸੈੱਟ_ਪਦ_option_sctx_idx(ਅੱਖਰ *name, ਪੂਰਨ opt_idx)
{
    char_u  buf[5];
    ਪੂਰਨ	    idx;

    ਜੇ (name == ਨਿਰਾ)
	idx = opt_idx;
    ਨਹੀਂਤਾਂ
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    ਜੇ (idx >= 0)
	ਸੈੱਟ_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    ਸਥਿਰ ਖਾਲੀ
apply_optionset_autocmd(
	ਪੂਰਨ	opt_idx,
	ਲੰਮਾ	opt_flags,
	ਲੰਮਾ	oldval,
	ਲੰਮਾ	oldval_g,
	ਲੰਮਾ	newval,
	ਅੱਖਰ	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    ਜੇ (starting || errmsg != ਨਿਰਾ || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	ਵਾਪਸ;

    len = vim_snprintf_safelen((ਅੱਖਰ *)buf_new, ਆਕਾਰ(buf_new), "%ld", newval);
    ਸੈੱਟ_vim_var_string(VV_OPTION_NEW, buf_new, (ਪੂਰਨ)len);
    len = vim_snprintf_safelen((ਅੱਖਰ *)buf_type, ਆਕਾਰ(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    ਸੈੱਟ_vim_var_string(VV_OPTION_TYPE, buf_type, (ਪੂਰਨ)len);

    buf_oldlen = vim_snprintf_safelen((ਅੱਖਰ *)buf_old, ਆਕਾਰ(buf_old), "%ld", oldval);
    ਸੈੱਟ_vim_var_string(VV_OPTION_OLD, buf_old, (ਪੂਰਨ)buf_oldlen);

    ਜੇ (opt_flags & OPT_LOCAL)
    {
	ਸੈੱਟ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	ਸੈੱਟ_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ਪੂਰਨ)buf_oldlen);
    }
    ਜੇ (opt_flags & OPT_GLOBAL)
    {
	ਸੈੱਟ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	ਸੈੱਟ_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (ਪੂਰਨ)buf_oldlen);
    }
    ਜੇ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((ਅੱਖਰ *)buf_old_global,
	    ਆਕਾਰ(buf_old_global), "%ld", oldval_g);

	ਸੈੱਟ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	ਸੈੱਟ_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ਪੂਰਨ)buf_oldlen);
	ਸੈੱਟ_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (ਪੂਰਨ)buf_old_globallen);
    }
    ਜੇ (opt_flags & OPT_MODELINE)
    {
	ਸੈੱਟ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	ਸੈੱਟ_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ਪੂਰਨ)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    ਨਿਰਾ, FALSE, ਨਿਰਾ);
    reset_v_option_vars();
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_arabic(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	ਜੇ (!p_tbidi)
	{
	    // set rightleft mode
	    ਜੇ (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		ਬਦਲਿਆ_ਖਿੜਕੀ_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    ਜੇ (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	ਜੇ (STRCMP(p_enc, "utf-8") != 0)
	{
	    ਸਥਿਰ ਅੱਖਰ *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    ਸੁਨੇਹਾ_ਸਰੋਤ(HL_ATTR(HLF_W));
	    ਸੁਨੇਹਾ_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    ਸੈੱਟ_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = ਸੈੱਟ_option_ਮੁੱਲ((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    ਨਹੀਂਤਾਂ
    {
	// 'arabic' is reset, handle various sub-settings.
	ਜੇ (!p_tbidi)
	{
	    // reset rightleft mode
	    ਜੇ (curwin->w_p_rl)
	    {
		curwin->w_p_rl = FALSE;
		ਬਦਲਿਆ_ਖਿੜਕੀ_setting();
	    }

	    // 'arabicshape' isn't reset, it is a global option and
	    // another window may still need it "on".
	}

	// 'delcombine' isn't reset, it is a global option and another
	// window may still want it "on".

# ifdef FEAT_KEYMAP
	// Revert to the default keymap
	curbuf->b_p_iminsert = B_IMODE_NONE;
	curbuf->b_p_imsearch = B_IMODE_USE_ਪਾਓ;
# endif
    }

    ਵਾਪਸ errmsg;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_autochdir(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_ballooneval(optset_T *ਦਲੀਲਾਂ)
{
    ਜੇ (balloon_Eval_For_ਪਦ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (p_beval && !ਦਲੀਲਾਂ->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    ਨਹੀਂਤਾਂ ਜੇ (!p_beval && ਦਲੀਲਾਂ->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_BEVAL_ਪਦ)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_balloonevalterm(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    mch_bevalterm_ਬਦਲਿਆ();
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Process the updated 'binary' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_ਦੁਆਧਾਰੀ(optset_T *ਦਲੀਲਾਂ)
{
    // when 'bin' is set also set some other options
    ਸੈੱਟ_options_bin(ਦਲੀਲਾਂ->os_oldval.boolean, curbuf->b_p_bin, ਦਲੀਲਾਂ->os_flags);
    redraw_titles();

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the updated 'buflisted' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_buflisted(optset_T *ਦਲੀਲਾਂ)
{
    // when 'buflisted' changes, trigger autocommands
    ਜੇ (ਦਲੀਲਾਂ->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						ਨਿਰਾ, ਨਿਰਾ, TRUE, curbuf);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the new 'cmdheight' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_cmdheight(optset_T *ਦਲੀਲਾਂ)
{
    ਲੰਮਾ old_ਮੁੱਲ = ਦਲੀਲਾਂ->os_oldval.ਸੰਖਿਆ;
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    // if p_ch changed value, change the command line height
    ਜੇ (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_ch = MIN_CMDHEIGHT;
    }
    ਜੇ (p_ch > ਪੰਕਤੀਆਂ - min_ਪੰਕਤੀਆਂ() + MIN_CMDHEIGHT)
	p_ch = ਪੰਕਤੀਆਂ - min_ਪੰਕਤੀਆਂ() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    ਜੇ ((p_ch != old_ਮੁੱਲ
		|| tabline_ਉਚਾਈ() + topframe->fr_ਉਚਾਈ != ਪੰਕਤੀਆਂ - p_ch)
	    && ਭਰਿਆ_screen
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    && !gui.starting
#ਜੇ_ਅੰਤ
       )
	command_ਉਚਾਈ();

    ਵਾਪਸ errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_compatible(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    compatible_ਸੈੱਟ();
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_conceallevel(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (curwin->w_p_cole < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	curwin->w_p_cole = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	curwin->w_p_cole = 3;
    }
    ਜੇ (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    ਵਾਪਸ errmsg;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_diff(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    ਜੇ (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_eof_eol_fixeol_bomb(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // redraw the window title and tab page text
    redraw_titles();
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the updated 'equalalways' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_equalalways(optset_T *ਦਲੀਲਾਂ)
{
    ਜੇ (p_ea && !ਦਲੀਲਾਂ->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_foldcolumn(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (curwin->w_p_fdc < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	curwin->w_p_fdc = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	curwin->w_p_fdc = 12;
    }
    ਜੇ (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    ਵਾਪਸ errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_foldlevel(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the new 'foldminlines' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_foldminlines(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    foldUpdateAll(curwin);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_foldnestmax(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_ਖੋਜ_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_hlsearch(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    ਸੈੱਟ_no_hlsearch(FALSE);
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Process the updated 'ignorecase' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_ignorecase(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    ਜੇ (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ)
/*
 * Process the updated 'imdisable' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_imdisable(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // Only de-activate it here, it will be enabled when changing mode.
    ਜੇ (p_imdisable)
	im_ਸੈੱਟ_ਸਰਗਰਮ(FALSE);
    ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_ਸੈੱਟ_ਸਰਗਰਮ(curbuf->b_p_iminsert == B_IMODE_IM);
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Process the new 'iminsert' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_iminsert(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_ਆਖਰੀ)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    ਜੇ (termcap_ਸਰਗਰਮ)	// don't do this in the alternate screen
	showmode();
#ਜੇ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    ਸਥਿਤੀ_redraw_curbuf();
#ਜੇ_ਅੰਤ

    ਵਾਪਸ errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_imsearch(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_ਆਖਰੀ)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    ਵਾਪਸ errmsg;
}

#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_imstyle(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_ਦਲੀਲ;

    ਵਾਪਸ errmsg;
}
#ਜੇ_ਅੰਤ

/*
 * Process the updated 'insertmode' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_insertmode(optset_T *ਦਲੀਲਾਂ)
{
    // when 'insertmode' is set from an autocommand need to do work here
    ਜੇ (p_im)
    {
	ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) == 0)
	    need_ਸ਼ੁਰੂ_insertmode = TRUE;
	stop_ਪਾਓ_ਬਹੁਲਕ = FALSE;
    }
    // only reset if it was set previously
    ਨਹੀਂਤਾਂ ਜੇ (ਦਲੀਲਾਂ->os_oldval.boolean)
    {
	need_ਸ਼ੁਰੂ_insertmode = FALSE;
	stop_ਪਾਓ_ਬਹੁਲਕ = TRUE;
	ਜੇ (restart_edit != 0 && ਬਹੁਲਕ_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_langnoremap(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the updated 'langremap' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_langremap(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Process the new 'laststatus' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_laststatus(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਆਖਰੀ_ਸਥਿਤੀ(FALSE);	// (re)set last window status line
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_linespace(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    ਜੇ (gui.ਵਿੱਚ_use && gui_mch_adjust_charheight() == OK)
	gui_ਸੈੱਟ_shellsize(FALSE, FALSE, RESIZE_VERT);
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Process the updated 'lisp' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_lisp(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (ਖਾਲੀ)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the new 'maxcombine' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_maxcombine(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    ਨਹੀਂਤਾਂ ਜੇ (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the updated 'modifiable' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_modifiable(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // when 'modifiable' is changed, redraw the window title

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    ਜੇ (curbuf->b_p_ma && (ਪਦ_ਵਿੱਚ_ਸਾਧਾਰਨ_ਬਹੁਲਕ(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_ਪਦ != ਨਿਰਾ && !ਪਦ_is_ਮੁਕੰਮਲ(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	ਵਾਪਸ e_cannot_make_terminal_with_running_ਕੰਮ_modifiable;
    }
#ਜੇ_ਅੰਤ
    redraw_titles();

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the updated 'modified' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_modified(optset_T *ਦਲੀਲਾਂ)
{
    ਜੇ (!ਦਲੀਲਾਂ->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_ਸੈੱਟ = !!ਦਲੀਲਾਂ->os_newval.boolean;
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_mousehide(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (!p_mh)
	gui_mch_mousehide(FALSE);
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_ਸੰਖਿਆ_relativenumber(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
#ਜੇ defined(FEAT_SIGNS) && defined(FEAT_GUI)
    ਜੇ (gui.ਵਿੱਚ_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != ਨਿਰਾ)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	ਜੇ (!(curwin->w_p_nu && ((ਪੂਰਨ *)ਦਲੀਲਾਂ->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_numberwidth(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    // 'numberwidth' must be positive
    ਜੇ (curwin->w_p_nuw < 1)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	curwin->w_p_nuw = 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	curwin->w_p_nuw = 20;
    }
    ਜੇ (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_ਸਤਰ_ਗਿਣਤੀ = 0; // trigger a redraw

    ਵਾਪਸ errmsg;
}
#ਜੇ_ਅੰਤ

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_osctimeoutlen(optset_T *ਦਲੀਲਾਂ)
{
    ਜੇ (p_ost < 0)
    {
	p_ost = ਦਲੀਲਾਂ->os_oldval.ਸੰਖਿਆ;
	ਵਾਪਸ e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_paste(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਸਥਿਰ ਪੂਰਨ	old_p_paste = FALSE;
    ਸਥਿਰ ਪੂਰਨ	save_sm = 0;
    ਸਥਿਰ ਪੂਰਨ	save_sta = 0;
    ਸਥਿਰ ਪੂਰਨ	save_ru = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਸਥਿਰ ਪੂਰਨ	save_ri = 0;
    ਸਥਿਰ ਪੂਰਨ	save_hkmap = 0;
#ਜੇ_ਅੰਤ
    buf_T	*buf;

    ਜੇ (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	ਜੇ (!old_p_paste)
	{
	    // save options for each buffer
	    FOR_ALL_BUFFERS(buf)
	    {
		buf->b_p_tw_nopaste = buf->b_p_tw;
		buf->b_p_wm_nopaste = buf->b_p_wm;
		buf->b_p_sts_nopaste = buf->b_p_sts;
		buf->b_p_ai_nopaste = buf->b_p_ai;
		buf->b_p_et_nopaste = buf->b_p_et;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		ਜੇ (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != ਖਾਲੀ_ਥਾਵਾਂ_option
					   ? vim_strsave(buf->b_p_vsts) : ਨਿਰਾ;
#ਜੇ_ਅੰਤ
	    }

	    // save global options
	    save_sm = p_sm;
	    save_sta = p_sta;
	    save_ru = p_ru;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    save_ri = p_ri;
	    save_hkmap = p_hkmap;
#ਜੇ_ਅੰਤ
	    // save global values for local buffer options
	    p_ai_nopaste = p_ai;
	    p_et_nopaste = p_et;
	    p_sts_nopaste = p_sts;
	    p_tw_nopaste = p_tw;
	    p_wm_nopaste = p_wm;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	    ਜੇ (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != ਖਾਲੀ_ਥਾਵਾਂ_option
						  ? vim_strsave(p_vsts) : ਨਿਰਾ;
#ਜੇ_ਅੰਤ
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	    ਜੇ (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#ਜੇ_ਅੰਤ
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	ਜੇ (p_ru)
	    ਸਥਿਤੀ_redraw_all();    // redraw to remove the ruler
	p_ru = 0;		    // no ruler
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	p_ri = 0;		    // no reverse insert
	p_hkmap = 0;		    // no Hebrew keyboard
#ਜੇ_ਅੰਤ
	// set global values for local buffer options
	p_tw = 0;
	p_wm = 0;
	p_sts = 0;
	p_ai = 0;
	p_et = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	ਜੇ (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ
    }

    // Paste switched from on to off: Restore saved values.
    ਨਹੀਂਤਾਂ ਜੇ (old_p_paste)
    {
	// restore options for each buffer
	FOR_ALL_BUFFERS(buf)
	{
	    buf->b_p_tw = buf->b_p_tw_nopaste;
	    buf->b_p_wm = buf->b_p_wm_nopaste;
	    buf->b_p_sts = buf->b_p_sts_nopaste;
	    buf->b_p_ai = buf->b_p_ai_nopaste;
	    buf->b_p_et = buf->b_p_et_nopaste;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	    ਜੇ (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : ਖਾਲੀ_ਥਾਵਾਂ_option;
	    vim_free(buf->b_p_vsts_array);
	    ਜੇ (buf->b_p_vsts && buf->b_p_vsts != ਖਾਲੀ_ਥਾਵਾਂ_option)
		(ਖਾਲੀ)tabstop_ਸੈੱਟ(buf->b_p_vsts, &buf->b_p_vsts_array);
	    ਨਹੀਂਤਾਂ
		buf->b_p_vsts_array = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	ਜੇ (p_ru != save_ru)
	    ਸਥਿਤੀ_redraw_all();    // redraw to draw the ruler
	p_ru = save_ru;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	p_ri = save_ri;
	p_hkmap = save_hkmap;
#ਜੇ_ਅੰਤ
	// set global values for local buffer options
	p_ai = p_ai_nopaste;
	p_et = p_et_nopaste;
	p_sts = p_sts_nopaste;
	p_tw = p_tw_nopaste;
	p_wm = p_wm_nopaste;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	ਜੇ (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ
    }

    old_p_paste = p_paste;

#ਜੇ defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਨਿਰਾ;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_previewwindow(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (!curwin->w_p_pvw)
	ਵਾਪਸ ਨਿਰਾ;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	ਜੇ (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    ਵਾਪਸ e_preview_ਖਿੜਕੀ_already_exists;
	}

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_pyxversion(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_ਦਲੀਲ;

    ਵਾਪਸ errmsg;
}
#ਜੇ_ਅੰਤ

/*
 * Process the updated 'readonly' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_readonly(optset_T *ਦਲੀਲਾਂ)
{
    // when 'readonly' is reset globally, also reset readonlymode
    ਜੇ (!curbuf->b_p_ro && (ਦਲੀਲਾਂ->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    ਜੇ (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_scrollbind(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    ਜੇ (!curwin->w_p_scb)
	ਵਾਪਸ ਨਿਰਾ;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_maxsearchcount(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ਖੋਜ_ਗਿਣਤੀ 9999

    ਜੇ (p_msc <= 0)
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
    ਨਹੀਂਤਾਂ ਜੇ (p_msc > MAX_ਖੋਜ_ਗਿਣਤੀ)
	errmsg = e_invalid_ਦਲੀਲ;

    ਜੇ (errmsg != ਨਿਰਾ)
	p_msc = 99;

    ਵਾਪਸ errmsg;
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ MAX_ਖੋਜ_ਗਿਣਤੀ
}

#ਜੇ defined(BACKSLASH_ਵਿੱਚ_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_shellslash(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    ਨਹੀਂਤਾਂ
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
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_shiftwidth_tabstop(optset_T *ਦਲੀਲਾਂ)
{
    ਲੰਮਾ	*pp = (ਲੰਮਾ *)ਦਲੀਲਾਂ->os_varp;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;

    ਜੇ (curbuf->b_p_ts <= 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	curbuf->b_p_ts = 8;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	curbuf->b_p_ts = 8;
    }
    ਜੇ (p_ts <= 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_ts = 8;
    }
    ਨਹੀਂਤਾਂ ਜੇ (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	p_ts = 8;
    }

    ਜੇ (curbuf->b_p_sw < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_ਗਿਣਤੀ(curbuf->b_p_vts_array) > 0
		       ? tabstop_ਪਹਿਲਾ(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#ਨਹੀਂਤਾਂ
	curbuf->b_p_sw = curbuf->b_p_ts;
#ਜੇ_ਅੰਤ
    }
    ਜੇ (p_sw < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_ਗਿਣਤੀ(curbuf->b_p_vts_array) > 0
	     ? tabstop_ਪਹਿਲਾ(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#ਨਹੀਂਤਾਂ
	p_sw = curbuf->b_p_ts;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#ਜੇ_ਅੰਤ
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    ਜੇ (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    ਵਾਪਸ errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_showtabline(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    shell_new_ਪੰਕਤੀਆਂ();	// recompute window positions and heights
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_smoothscroll(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_spell(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (curwin->w_p_spell)
	ਵਾਪਸ parse_spelllang(curwin);

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Process the updated 'swapfile' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_swapfile(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    ਜੇ (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    ਨਹੀਂਤਾਂ
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_TERMGUICOLORS)
    ਅੱਖਰ *
did_ਸੈੱਟ_termguicolors(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    ਜੇ (
#  ifdef VIMDLL
	    !gui.ਵਿੱਚ_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_ਸੈੱਟ = TRUE;
	p_tgc = 0;
	ਵਾਪਸ e_24_ਦੋਆਧਾਰੀ_ਅੰਕ_colors_are_not_supported_on_this_environment;
    }
    ਜੇ (is_ਪਦ_win32())
	ਅਦਲਾ_ਬਦਲੀ_tcap();
# endif
# ifdef FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    ਜੇ (is_ਪਦ_win32())
    {
	control_console_color_rgb();
	ਸੈੱਟ_termname(T_NAME);
	init_highlight(TRUE, FALSE);
    }
# endif
# ifdef FEAT_TERMINAL
    ਪਦ_update_colors_all();
    ਪਦ_update_palette_all();
    ਪਦ_update_hlfwin_all();
# endif
    p_tgc_ਸੈੱਟ = TRUE;

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

    ਅੱਖਰ *
did_ਸੈੱਟ_termsync(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (!p_tsy)
	ਪਦ_ਸੈੱਟ_sync_output(ਪਦ_SYNC_OUTPUT_OFF);
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_termwinscroll(optset_T *ਦਲੀਲਾਂ)
{
    ਲੰਮਾ	*pp = (ਲੰਮਾ *)ਦਲੀਲਾਂ->os_varp;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;

    ਜੇ (*pp < 1)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	*pp = 1;
    }

    ਵਾਪਸ errmsg;
}
#ਜੇ_ਅੰਤ

/*
 * Process the updated 'terse' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_terse(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_ਖੋਜ);

    // insert 's' in p_shm
    ਜੇ (p_terse && p == ਨਿਰਾ)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    ਨਹੀਂਤਾਂ ਜੇ (!p_terse && p != ਨਿਰਾ)
	STRMOVE(p, p + 1);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the updated 'textauto' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_textauto(optset_T *ਦਲੀਲਾਂ)
{
    // when 'textauto' is set or reset also change 'fileformats'
    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | ਦਲੀਲਾਂ->os_flags, 0);

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the updated 'textmode' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_textmode(optset_T *ਦਲੀਲਾਂ)
{
    // when 'textmode' is set or reset also change 'fileformat'
    ਸੈੱਟ_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, ਦਲੀਲਾਂ->os_flags);

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the new 'textwidth' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_textwidth(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (curbuf->b_p_tw < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	curbuf->b_p_tw = 0;
    }
    ਜੇ (p_tw < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_tw = 0;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(ਨਿਰਾ, wp);
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_title_icon(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_ਸੈੱਟ_title();
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the new 'titlelen' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_titlelen(optset_T *ਦਲੀਲਾਂ)
{
    ਲੰਮਾ old_ਮੁੱਲ = ਦਲੀਲਾਂ->os_oldval.ਸੰਖਿਆ;
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    // if 'titlelen' has changed, redraw the title
    ਜੇ (p_titlelen < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_titlelen = 85;
    }
    ਜੇ (starting != NO_SCREEN && old_ਮੁੱਲ != p_titlelen)
	need_maketitle = TRUE;

    ਵਾਪਸ errmsg;
}

#ਜੇ defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_undofile(optset_T *ਦਲੀਲਾਂ)
{
    // Only take action when the option was set.
    ਜੇ (!curbuf->b_p_udf && !p_udf)
	ਵਾਪਸ ਨਿਰਾ;

    // When reset we do not delete the undo file, the option may be set again
    // without making any changes in between.
    char_u	hash[UNDO_HASH_ਮਾਪ];
    buf_T	*save_curbuf = curbuf;

    FOR_ALL_BUFFERS(curbuf)
    {
	// When 'undofile' is set globally: for every buffer, otherwise
	// only for the current buffer: Try to read in the undofile,
	// if one exists, the buffer wasn't changed and the buffer was
	// loaded
	ਜੇ ((curbuf == save_curbuf
		    || (ਦਲੀਲਾਂ->os_flags & OPT_GLOBAL)
		    || ਦਲੀਲਾਂ->os_flags == 0)
		&& !curbuf_Is_ਬਦਲਿਆ() && curbuf->b_ml.ml_mfp != ਨਿਰਾ)
	{
# ifdef FEAT_CRYPT
	    ਜੇ (crypt_ਵਿਧੀ_is_sodium(crypt_get_ਵਿਧੀ_nr(curbuf)))
		ਜਾਰੀ;
# endif
	    u_compute_hash(hash);
	    u_read_undo(ਨਿਰਾ, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Process the new global 'undolevels' option value.
 */
    ਸਥਿਰ ਖਾਲੀ
update_global_undolevels(ਲੰਮਾ ਮੁੱਲ, ਲੰਮਾ old_ਮੁੱਲ)
{
    // sync undo before 'undolevels' changes

    // use the old value, otherwise u_sync() may not work properly
    p_ul = old_ਮੁੱਲ;
    u_sync(TRUE);
    p_ul = ਮੁੱਲ;
}

/*
 * Process the new buffer local 'undolevels' option value.
 */
    ਸਥਿਰ ਖਾਲੀ
update_buflocal_undolevels(ਲੰਮਾ ਮੁੱਲ, ਲੰਮਾ old_ਮੁੱਲ)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_ਮੁੱਲ;
    u_sync(TRUE);
    curbuf->b_p_ul = ਮੁੱਲ;
}

/*
 * Process the new 'undolevels' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_undolevels(optset_T *ਦਲੀਲਾਂ)
{
    ਲੰਮਾ *pp = (ਲੰਮਾ *)ਦਲੀਲਾਂ->os_varp;

    ਜੇ (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(ਦਲੀਲਾਂ->os_newval.ਸੰਖਿਆ,
						       ਦਲੀਲਾਂ->os_oldval.ਸੰਖਿਆ);
    ਨਹੀਂਤਾਂ ਜੇ (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(ਦਲੀਲਾਂ->os_newval.ਸੰਖਿਆ,
						       ਦਲੀਲਾਂ->os_oldval.ਸੰਖਿਆ);

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the new 'updatecount' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_updatecount(optset_T *ਦਲੀਲਾਂ)
{
    ਲੰਮਾ old_ਮੁੱਲ = ਦਲੀਲਾਂ->os_oldval.ਸੰਖਿਆ;
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    // when 'updatecount' changes from zero to non-zero, open swap files
    ਜੇ (p_uc < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_uc = 100;
    }
    ਜੇ (p_uc && !old_ਮੁੱਲ)
	ml_open_files();

    ਵਾਪਸ errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_weirdinvert(optset_T *ਦਲੀਲਾਂ)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    ਜੇ (p_wiv && !ਦਲੀਲਾਂ->os_oldval.boolean)
	T_XS = (char_u *)"y";
    ਨਹੀਂਤਾਂ ਜੇ (!p_wiv && ਦਲੀਲਾਂ->os_oldval.boolean)
	T_XS = ਖਾਲੀ_ਥਾਵਾਂ_option;
    p_wiv = (*T_XS != NUL);

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_wildchar(optset_T *ਦਲੀਲਾਂ)
{
    ਲੰਮਾ c = *(ਲੰਮਾ *)ਦਲੀਲਾਂ->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    ਜੇ (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the new 'window' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_ਖਿੜਕੀ(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (p_ਖਿੜਕੀ < 1)
	p_ਖਿੜਕੀ = 1;
    ਨਹੀਂਤਾਂ ਜੇ (p_ਖਿੜਕੀ >= ਪੰਕਤੀਆਂ)
	p_ਖਿੜਕੀ = ਪੰਕਤੀਆਂ - 1;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_winheight_helpheight(optset_T *ਦਲੀਲਾਂ)
{
    ਲੰਮਾ	*pp = (ਲੰਮਾ *)ਦਲੀਲਾਂ->os_varp;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;

    ਜੇ (p_wh < 1)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_wh = 1;
    }
    ਜੇ (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    ਜੇ (p_hh < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_hh = 0;
    }

    // Change window height NOW
    ਜੇ (!ONE_ਖਿੜਕੀ)
    {
	ਜੇ (pp == &p_wh && curwin->w_ਉਚਾਈ < p_wh)
	    win_setheight((ਪੂਰਨ)p_wh);
	ਜੇ (pp == &p_hh && curbuf->b_help && curwin->w_ਉਚਾਈ < p_hh)
	    win_setheight((ਪੂਰਨ)p_hh);
    }

    ਵਾਪਸ errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_winminheight(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (p_wmh < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_wmh = 0;
    }
    ਜੇ (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    ਵਾਪਸ errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_winminwidth(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (p_wmw < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_wmw = 0;
    }
    ਜੇ (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    ਵਾਪਸ errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_winwidth(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (p_wiw < 1)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_wiw = 1;
    }
    ਜੇ (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    ਜੇ (!ONE_ਖਿੜਕੀ && curwin->w_ਚੌੜਾਈ < p_wiw)
	win_setwidth((ਪੂਰਨ)p_wiw);

    ਵਾਪਸ errmsg;
}

#ਜੇ defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_wlsteal(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    clip_reset_wayland();

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_wltimeoutlen(optset_T *ਦਲੀਲਾਂ)
{
    ਜੇ (p_wtm < 0)
    {
	p_wtm = ਦਲੀਲਾਂ->os_oldval.ਸੰਖਿਆ;
	ਵਾਪਸ e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
    }

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Process the updated 'wrap' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_wrap(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // Set w_leftcol or w_skipcol to zero.
    ਜੇ (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    ਨਹੀਂਤਾਂ
	curwin->w_skipcol = 0;

    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_xhistory(optset_T *ਦਲੀਲਾਂ)
{
    ਪੂਰਨ is_p_chi = (ਲੰਮਾ*)ਦਲੀਲਾਂ->os_varp == &p_chi;
    ਪੂਰਨ err;
    ਲੰਮਾ *arg = (is_p_chi) ? &p_chi :(ਲੰਮਾ*)ਦਲੀਲਾਂ->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    ਜੇ (*arg < 1)
    {
	*arg = ਦਲੀਲਾਂ->os_oldval.ਸੰਖਿਆ;
	ਵਾਪਸ e_cannot_have_negative_or_zero_ਸੰਖਿਆ_ਦਾ_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    ਜੇ (*arg > 100)
    {
	*arg = ਦਲੀਲਾਂ->os_oldval.ਸੰਖਿਆ;
	ਵਾਪਸ e_cannot_have_ਹੋਰ_than_hundred_quickfix;
    }

    ਜੇ (is_p_chi)
	err = qf_resize_ਢੇਰ(*arg);
    ਨਹੀਂਤਾਂ
	err = ll_resize_ਢੇਰ(curwin, *arg);

    ਜੇ (err == FAIL)
	ਵਾਪਸ e_failed_resizing_quickfix_ਢੇਰ;

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ਸਥਿਰ ਅੱਖਰ *
ਸੈੱਟ_bool_option(
    ਪੂਰਨ		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    ਪੂਰਨ		ਮੁੱਲ,			// new value
    ਪੂਰਨ		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    ਪੂਰਨ		old_ਮੁੱਲ = *(ਪੂਰਨ *)varp;
#ਜੇ defined(FEAT_EVAL)
    ਪੂਰਨ		old_global_ਮੁੱਲ = 0;
#ਜੇ_ਅੰਤ
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;

    // Disallow changing some options from secure mode
    ਜੇ ((secure
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
		|| sandbox != 0
#ਜੇ_ਅੰਤ
		) && (options[opt_idx].flags & P_SECURE))
	ਵਾਪਸ e_not_allowed_ਇੱਥੇ;

#ਜੇ defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    ਜੇ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_ਮੁੱਲ = *(ਪੂਰਨ *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#ਜੇ_ਅੰਤ

    *(ਪੂਰਨ *)varp = ਮੁੱਲ;	    // set the new value
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Remember where the option was set.
    ਸੈੱਟ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

    // May set global value for local option.
    ਜੇ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(ਪੂਰਨ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = ਮੁੱਲ;

    // Handle side effects of changing a bool option.
    ਜੇ (options[opt_idx].opt_did_ਸੈੱਟ_cb != ਨਿਰਾ)
    {
	optset_T ਦਲੀਲਾਂ;

	CLEAR_ਖੇਤਰ(ਦਲੀਲਾਂ);
	ਦਲੀਲਾਂ.os_varp = varp;
	ਦਲੀਲਾਂ.os_flags = opt_flags;
	ਦਲੀਲਾਂ.os_oldval.boolean = old_ਮੁੱਲ;
	ਦਲੀਲਾਂ.os_newval.boolean = ਮੁੱਲ;
	ਦਲੀਲਾਂ.os_errbuf = ਨਿਰਾ;
	errmsg = options[opt_idx].opt_did_ਸੈੱਟ_cb(&ਦਲੀਲਾਂ);
	ਜੇ (errmsg != ਨਿਰਾ)
	    ਵਾਪਸ errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_ਸੈੱਟ;

#ਜੇ defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(ਲੰਮਾ)(old_ਮੁੱਲ ? TRUE : FALSE),
				(ਲੰਮਾ)(old_global_ਮੁੱਲ ? TRUE : FALSE),
				(ਲੰਮਾ)(ਮੁੱਲ ? TRUE : FALSE), ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    comp_ਖਾਨਾ();			    // in case 'ruler' or 'showcmd' changed

    ਜੇ (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_ਸੈੱਟ_curswant = true;

    ਜੇ ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    ਵਾਪਸ errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    ਸਥਿਰ ਅੱਖਰ *
check_ਅੰਕ_option_bounds(
    ਲੰਮਾ	*pp,
    ਲੰਮਾ	old_ਮੁੱਲ,
    ਲੰਮਾ	old_ਪੰਕਤੀਆਂ_2,
    ਲੰਮਾ	old_Columns,
    ਅੱਖਰ	*errbuf,
    size_t	errbuflen,
    ਅੱਖਰ	*errmsg)
{
    ਜੇ (ਪੰਕਤੀਆਂ < min_ਪੰਕਤੀਆਂ_for_all_tabpages() && ਭਰਿਆ_screen)
    {
	ਜੇ (errbuf != ਨਿਰਾ)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_ਘੱਟ_ਤੋਂ_ਘੱਟ_nr_ਸਤਰਾਂ), min_ਪੰਕਤੀਆਂ_for_all_tabpages());
	    errmsg = errbuf;
	}
	ਪੰਕਤੀਆਂ = min_ਪੰਕਤੀਆਂ_for_all_tabpages();
    }
    ਜੇ (Columns < MIN_COLUMNS && ਭਰਿਆ_screen)
    {
	ਜੇ (errbuf != ਨਿਰਾ)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_ਘੱਟ_ਤੋਂ_ਘੱਟ_nr_columns), MIN_COLUMNS);
	    errmsg = errbuf;
	}
	Columns = MIN_COLUMNS;
    }
    ਹੱਦਬੰਦੀ_screen_ਮਾਪ();

    // If the screen (shell) height has been changed, assume it is the
    // physical screenheight.
    ਜੇ (old_ਪੰਕਤੀਆਂ_2 != ਪੰਕਤੀਆਂ || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	ਜੇ (updating_screen)
	    *pp = old_ਮੁੱਲ;
	ਨਹੀਂਤਾਂ ਜੇ (ਭਰਿਆ_screen
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		&& !gui.starting
#ਜੇ_ਅੰਤ
		)
	    ਸੈੱਟ_shellsize((ਪੂਰਨ)Columns, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, TRUE);
	ਨਹੀਂਤਾਂ
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    ਜੇ (cmdline_ਪੰਕਤੀ > ਪੰਕਤੀਆਂ - p_ch && ਪੰਕਤੀਆਂ > p_ch)
		cmdline_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - p_ch;
	}
	ਜੇ (p_ਖਿੜਕੀ >= ਪੰਕਤੀਆਂ || !option_was_ਸੈੱਟ((char_u *)"window"))
	    p_ਖਿੜਕੀ = ਪੰਕਤੀਆਂ - 1;
    }

    ਜੇ (p_tm < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_tm = 0;
    }
    ਜੇ ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_ਉਚਾਈ
		    && curwin->w_ਉਚਾਈ > 0))
	    && ਭਰਿਆ_screen)
    {
	ਜੇ (pp == &(curwin->w_p_scr))
	{
	    ਜੇ (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_ਮਾਪ;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	ਨਹੀਂਤਾਂ ਜੇ (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	ਨਹੀਂਤਾਂ // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_ਉਚਾਈ;
    }
    ਜੇ (p_hi < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_hi = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (p_hi > 10000)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	p_hi = 10000;
    }
    ਜੇ (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	p_re = 0;
    }
    ਜੇ (p_report < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_report = 1;
    }
    ਜੇ ((p_sj < -100 || p_sj >= ਪੰਕਤੀਆਂ) && ਭਰਿਆ_screen)
    {
	ਜੇ (ਪੰਕਤੀਆਂ != old_ਪੰਕਤੀਆਂ_2)	// Rows changed, just adjust p_sj
	    p_sj = ਪੰਕਤੀਆਂ / 2;
	ਨਹੀਂਤਾਂ
	{
	    errmsg = e_invalid_scroll_ਮਾਪ;
	    p_sj = 1;
	}
    }
    ਜੇ (p_so < 0 && ਭਰਿਆ_screen)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_so = 0;
    }
    ਜੇ (p_sop < 0 && ਭਰਿਆ_screen)
    {
	errmsg = e_invalid_ਦਲੀਲ;
	p_sop = 0;
    }
    ਜੇ (p_siso < 0 && ਭਰਿਆ_screen)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_siso = 0;
    }
    ਜੇ (p_cwh < 1)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_cwh = 1;
    }
    ਜੇ (p_ut < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_ut = 2000;
    }
    ਜੇ (p_ss < 0)
    {
	errmsg = e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ;
	p_ss = 0;
    }

    ਵਾਪਸ errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ਸਥਿਰ ਅੱਖਰ *
ਸੈੱਟ_ਅੰਕ_option(
    ਪੂਰਨ		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    ਲੰਮਾ	ਮੁੱਲ,			// new value
    ਅੱਖਰ	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    ਪੂਰਨ		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;
    ਲੰਮਾ	old_ਮੁੱਲ = *(ਲੰਮਾ *)varp;
#ਜੇ defined(FEAT_EVAL)
    ਲੰਮਾ	old_global_ਮੁੱਲ = 0;	// only used when setting a local and
					// global option
#ਜੇ_ਅੰਤ
    ਲੰਮਾ	old_ਪੰਕਤੀਆਂ_2 = ਪੰਕਤੀਆਂ;	// remember old Rows
    ਲੰਮਾ	old_Columns = Columns;	// remember old Columns
    ਲੰਮਾ	*pp = (ਲੰਮਾ *)varp;

    // Disallow changing some options from secure mode.
    ਜੇ ((secure
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
		|| sandbox != 0
#ਜੇ_ਅੰਤ
		) && (options[opt_idx].flags & P_SECURE))
	ਵਾਪਸ e_not_allowed_ਇੱਥੇ;

#ਜੇ defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    ਜੇ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_ਮੁੱਲ = *(ਲੰਮਾ *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#ਜੇ_ਅੰਤ

    *pp = ਮੁੱਲ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Remember where the option was set.
    ਸੈੱਟ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

    // May set global value for local option.
    ਜੇ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(ਲੰਮਾ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = ਮੁੱਲ;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    ਜੇ (options[opt_idx].opt_did_ਸੈੱਟ_cb != ਨਿਰਾ)
    {
	optset_T ਦਲੀਲਾਂ;

	CLEAR_ਖੇਤਰ(ਦਲੀਲਾਂ);
	ਦਲੀਲਾਂ.os_varp = varp;
	ਦਲੀਲਾਂ.os_flags = opt_flags;
	ਦਲੀਲਾਂ.os_oldval.ਸੰਖਿਆ = old_ਮੁੱਲ;
	ਦਲੀਲਾਂ.os_newval.ਸੰਖਿਆ = ਮੁੱਲ;
	ਦਲੀਲਾਂ.os_errbuf = ਨਿਰਾ;
	errmsg = options[opt_idx].opt_did_ਸੈੱਟ_cb(&ਦਲੀਲਾਂ);
    }

    // Check the bounds for numeric options here
    errmsg = check_ਅੰਕ_option_bounds(pp, old_ਮੁੱਲ, old_ਪੰਕਤੀਆਂ_2, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_ਸੈੱਟ;

#ਜੇ defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_ਮੁੱਲ, old_global_ਮੁੱਲ,
								ਮੁੱਲ, errmsg);
#ਜੇ_ਅੰਤ

    comp_ਖਾਨਾ();			    // in case 'columns' or 'ls' changed

    ਜੇ (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_ਸੈੱਟ_curswant = true;

    ਜੇ ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    ਵਾਪਸ errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    ਖਾਲੀ
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    ਪੂਰਨ		doclear = (flags & P_RCLR) == P_RCLR;
    ਪੂਰਨ		all = ((flags & P_RALL) == P_RALL || doclear);

    ਜੇ ((flags & P_RSTAT) || all)	// mark all status lines dirty
	ਸਥਿਤੀ_redraw_all();

    ਜੇ ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	ਜੇ (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	ਨਹੀਂਤਾਂ
	    ਬਦਲਿਆ_ਖਿੜਕੀ_setting();
    }
    ਜੇ (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    ਜੇ (doclear)
	redraw_all_later(UPD_CLEAR);
    ਨਹੀਂਤਾਂ ਜੇ (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    ਪੂਰਨ
findoption(char_u *arg)
{
    ਪੂਰਨ		    opt_idx;
    ਅੱਖਰ	    *s, *p;
    ਸਥਿਰ ਛੋਟਾ    ਤੇਜ਼_tab[27] = {0, 0};	// quick access table
    ਪੂਰਨ		    is_ਪਦ_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    ਜੇ (ਤੇਜ਼_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	ਪੂਰਨ ਆਖਰੀ_opt_idx;
	ਲਈ (ਆਖਰੀ_opt_idx = 0; options[ਆਖਰੀ_opt_idx].fullname != ਨਿਰਾ;
		ਆਖਰੀ_opt_idx++)
	    ;
	ਲਈ (ਪੂਰਨ tab_idx = 1; tab_idx < 27; tab_idx++)
	    ਤੇਜ਼_tab[tab_idx] = ਆਖਰੀ_opt_idx;

	p = options[0].fullname;
	ਲਈ (opt_idx = 1; opt_idx < ਆਖਰੀ_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    ਜੇ (s[0] != p[0])
	    {
		ਜੇ (s[0] == 't' && s[1] == '_')
		    ਤੇਜ਼_tab[26] = opt_idx;
		ਨਹੀਂਤਾਂ
		    ਤੇਜ਼_tab[Char_Ord_ਨੀਵਾਂ(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    ਜੇ (arg[0] < 'a' || arg[0] > 'z')
	ਵਾਪਸ -1;

    is_ਪਦ_opt = (arg[0] == 't' && arg[1] == '_');
    ਜੇ (is_ਪਦ_opt)
	opt_idx = ਤੇਜ਼_tab[26];
    ਨਹੀਂਤਾਂ
	opt_idx = ਤੇਜ਼_tab[Char_Ord_ਨੀਵਾਂ(arg[0])];
    ਲਈ (; (s = options[opt_idx].fullname) != ਨਿਰਾ && s[0] == arg[0]; opt_idx++)
    {
	ਜੇ (STRCMP(arg, s) == 0)		    // match full name
	    ਰੋਕੋ;
    }
    ਜੇ (s != ਨਿਰਾ && s[0] != arg[0])
	s = ਨਿਰਾ;
    ਜੇ (s == ਨਿਰਾ && !is_ਪਦ_opt)
    {
	opt_idx = ਤੇਜ਼_tab[Char_Ord_ਨੀਵਾਂ(arg[0])];
	ਲਈ ( ; options[opt_idx].fullname != ਨਿਰਾ; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    ਜੇ (s != ਨਿਰਾ && STRCMP(arg, s) == 0)   // match short name
		ਰੋਕੋ;
	    s = ਨਿਰਾ;
	}
    }
    ਜੇ (s == ਨਿਰਾ)
	opt_idx = -1;
    ਵਾਪਸ opt_idx;
}

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
get_option_ਮੁੱਲ(
    char_u	*name,
    ਲੰਮਾ	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    ਪੂਰਨ		*flagsp,
    ਪੂਰਨ		scope)
{
    ਪੂਰਨ		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    ਜੇ (opt_idx < 0)		    // option not in the table
    {
	ਪੂਰਨ ਕੁੰਜੀ;

	ਜੇ (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (ਕੁੰਜੀ = ਲੱਭੋ_ਕੁੰਜੀ_option(name, FALSE)) != 0)
	{
	    char_u ਕੁੰਜੀ_name[2];
	    char_u *p;

	    ਜੇ (flagsp != ਨਿਰਾ)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    ਜੇ (ਕੁੰਜੀ < 0)
	    {
		ਕੁੰਜੀ_name[0] = KEY2TERMCAP0(ਕੁੰਜੀ);
		ਕੁੰਜੀ_name[1] = KEY2TERMCAP1(ਕੁੰਜੀ);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਕੁੰਜੀ_name[0] = KS_ਕੁੰਜੀ;
		ਕੁੰਜੀ_name[1] = (ਕੁੰਜੀ & 0xff);
	    }
	    p = ਲੱਭੋ_termcode(ਕੁੰਜੀ_name);
	    ਜੇ (p != ਨਿਰਾ)
	    {
		ਜੇ (stringval != ਨਿਰਾ)
		    *stringval = vim_strsave(p);
		ਵਾਪਸ gov_string;
	    }
	}
	ਵਾਪਸ gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    ਜੇ (flagsp != ਨਿਰਾ)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    ਜੇ (options[opt_idx].flags & P_STRING)
    {
	ਜੇ (varp == ਨਿਰਾ)		    // hidden option
	    ਵਾਪਸ gov_hidden_string;
	ਜੇ (stringval != ਨਿਰਾ)
	{
	    ਜੇ ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    ਨਹੀਂਤਾਂ ਜੇ ((char_u **)varp == &curbuf->b_p_ਕੁੰਜੀ
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    ਨਹੀਂਤਾਂ
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	ਵਾਪਸ gov_string;
    }

    ਜੇ (varp == ਨਿਰਾ)		    // hidden option
	ਵਾਪਸ (options[opt_idx].flags & P_ਅੰਕ)
					 ? gov_hidden_ਸੰਖਿਆ : gov_hidden_bool;
    ਜੇ (options[opt_idx].flags & P_ਅੰਕ)
	*numval = *(ਲੰਮਾ *)varp;
    ਨਹੀਂਤਾਂ
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	ਜੇ ((ਪੂਰਨ *)varp == &curbuf->b_ਬਦਲਿਆ)
	    *numval = curbuf_Is_ਬਦਲਿਆ();
	ਨਹੀਂਤਾਂ
	    *numval = (ਲੰਮਾ) *(ਪੂਰਨ *)varp;
    }
    ਵਾਪਸ (options[opt_idx].flags & P_ਅੰਕ) ? gov_ਸੰਖਿਆ : gov_bool;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    ਪੂਰਨ
get_option_ਮੁੱਲ_strict(
    char_u	*name,
    ਲੰਮਾ	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    ਪੂਰਨ		opt_type,
    ਖਾਲੀ	*from)
{
    ਪੂਰਨ		opt_idx;
    char_u	*varp = ਨਿਰਾ;
    ਬਣਤਰ vimoption *p;
    ਪੂਰਨ		r = 0;

    opt_idx = findoption(name);
    ਜੇ (opt_idx < 0)
	ਵਾਪਸ 0;

    p = &(options[opt_idx]);

    // Hidden option
    ਜੇ (p->var == ਨਿਰਾ)
	ਵਾਪਸ 0;

    ਜੇ (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    ਨਹੀਂਤਾਂ ਜੇ (p->flags & P_ਅੰਕ)
	r |= SOPT_ਅੰਕ;
    ਨਹੀਂਤਾਂ ਜੇ (p->flags & P_STRING)
	r |= SOPT_STRING;

    ਜੇ (p->indir == PV_NONE)
    {
	ਜੇ (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ 0; // Did not request global-only option
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	ਨਹੀਂਤਾਂ ਜੇ (opt_type == SREQ_GLOBAL)
	    ਵਾਪਸ 0; // Requested global option

	ਜੇ (p->indir & PV_WIN)
	{
	    ਜੇ (opt_type == SREQ_BUF)
		ਵਾਪਸ 0; // Did not request window-local option
	    ਨਹੀਂਤਾਂ
		r |= SOPT_WIN;
	}
	ਨਹੀਂਤਾਂ ਜੇ (p->indir & PV_BUF)
	{
	    ਜੇ (opt_type == SREQ_WIN)
		ਵਾਪਸ 0; // Did not request buffer-local option
	    ਨਹੀਂਤਾਂ
		r |= SOPT_BUF;
	}
    }

    ਜੇ (stringval == ਨਿਰਾ)
	ਵਾਪਸ r;

    ਜੇ (opt_type == SREQ_GLOBAL)
	varp = p->var;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    ਜੇ (p->indir == PV_MOD)
	    {
		*numval = buf_Is_ਬਦਲਿਆ((buf_T *)from);
		varp = ਨਿਰਾ;
	    }
# ifdef FEAT_CRYPT
	    ਨਹੀਂਤਾਂ ਜੇ (p->indir == PV_ਕੁੰਜੀ)
	    {
		// never return the value of the crypt key
		*stringval = ਨਿਰਾ;
		varp = ਨਿਰਾ;
	    }
# endif
	    ਨਹੀਂਤਾਂ
	    {
		buf_T *save_curbuf = curbuf;

		// only getting a pointer, no need to use aucmd_prepbuf()
		curbuf = (buf_T *)from;
		curwin->w_ਬਫਰ = curbuf;
		varp = get_varp(p);
		curbuf = save_curbuf;
		curwin->w_ਬਫਰ = curbuf;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_ਬਫਰ;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_ਬਫਰ;
	}
	ਜੇ (varp == p->var)
	    ਵਾਪਸ (r | SOPT_UNSET);
    }

    ਜੇ (varp != ਨਿਰਾ)
    {
	ਜੇ (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	ਨਹੀਂਤਾਂ ਜੇ (p->flags & P_ਅੰਕ)
	    *numval = *(ਲੰਮਾ *) varp;
	ਨਹੀਂਤਾਂ
	    *numval = *(ਪੂਰਨ *)varp;
    }

    ਵਾਪਸ r;
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
option_iter_ਅਗਲਾ(ਖਾਲੀ **option, ਪੂਰਨ opt_type)
{
    ਬਣਤਰ vimoption	*ret = ਨਿਰਾ;
    ਕਰੋ
    {
	ਜੇ (*option == ਨਿਰਾ)
	    *option = (ਖਾਲੀ *) options;
	ਨਹੀਂਤਾਂ ਜੇ (((ਬਣਤਰ vimoption *) (*option))->fullname == ਨਿਰਾ)
	{
	    *option = ਨਿਰਾ;
	    ਵਾਪਸ ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	    *option = (ਖਾਲੀ *) (((ਬਣਤਰ vimoption *) (*option)) + 1);

	ret = ((ਬਣਤਰ vimoption *) (*option));

	// Hidden option
	ਜੇ (ret->var == ਨਿਰਾ)
	{
	    ret = ਨਿਰਾ;
	    ਜਾਰੀ;
	}

	ਚੋਣ (opt_type)
	{
	    ਹਾਲਤ SREQ_GLOBAL:
		ਜੇ (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = ਨਿਰਾ;
		ਰੋਕੋ;
	    ਹਾਲਤ SREQ_BUF:
		ਜੇ (!(ret->indir & PV_BUF))
		    ret = ਨਿਰਾ;
		ਰੋਕੋ;
	    ਹਾਲਤ SREQ_WIN:
		ਜੇ (!(ret->indir & PV_WIN))
		    ret = ਨਿਰਾ;
		ਰੋਕੋ;
	    ਮੂਲ:
		ਅੰਦਰੂਨੀ_error("option_iter_next()");
		ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਜਦੋਂ (ret == ਨਿਰਾ);

    ਵਾਪਸ (char_u *)ret->fullname;
}
#ਜੇ_ਅੰਤ

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(ਪੂਰਨ opt_idx)
{
    ਵਾਪਸ options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    ਖਾਲੀ
ਸੈੱਟ_option_flag(ਪੂਰਨ opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    ਪੂਰਨ
is_global_option(ਪੂਰਨ opt_idx)
{
    ਵਾਪਸ options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    ਪੂਰਨ
is_global_local_option(ਪੂਰਨ opt_idx)
{
    ਵਾਪਸ options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    ਪੂਰਨ
is_ਖਿੜਕੀ_local_option(ਪੂਰਨ opt_idx)
{
    ਵਾਪਸ options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    ਪੂਰਨ
is_hidden_option(ਪੂਰਨ opt_idx)
{
    ਵਾਪਸ options[opt_idx].var == ਨਿਰਾ;
}

#ਜੇ defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    ਪੂਰਨ
is_crypt_ਕੁੰਜੀ_option(ਪੂਰਨ opt_idx)
{
    ਵਾਪਸ options[opt_idx].indir == PV_ਕੁੰਜੀ;
}
#ਜੇ_ਅੰਤ

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    ਅੱਖਰ *
ਸੈੱਟ_option_ਮੁੱਲ(
    char_u	*name,
    ਲੰਮਾ	ਸੰਖਿਆ,
    char_u	*string,
    ਪੂਰਨ		opt_flags)	// OPT_LOCAL or 0 (both)
{
    ਪੂਰਨ		opt_idx;
    char_u	*varp;
    long_u	flags;
    ਸਥਿਰ ਅੱਖਰ	errbuf[ERR_BUFLEN];
    ਪੂਰਨ		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    ਜੇ (opt_idx < 0)
    {
	ਪੂਰਨ ਕੁੰਜੀ;

	ਜੇ (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (ਕੁੰਜੀ = ਲੱਭੋ_ਕੁੰਜੀ_option(name, FALSE)) != 0)
	{
	    char_u ਕੁੰਜੀ_name[2];

	    ਜੇ (ਕੁੰਜੀ < 0)
	    {
		ਕੁੰਜੀ_name[0] = KEY2TERMCAP0(ਕੁੰਜੀ);
		ਕੁੰਜੀ_name[1] = KEY2TERMCAP1(ਕੁੰਜੀ);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਕੁੰਜੀ_name[0] = KS_ਕੁੰਜੀ;
		ਕੁੰਜੀ_name[1] = (ਕੁੰਜੀ & 0xff);
	    }
	    ਜੋੜੋ_termcode(ਕੁੰਜੀ_name, string, FALSE);
	    ਜੇ (ਭਰਿਆ_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    ਨਹੀਂਤਾਂ
    {
	flags = options[opt_idx].flags;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	ਜੇ (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_ਵਿੱਚ_sandbox));
	    ਵਾਪਸ ਨਿਰਾ;
	}
#ਜੇ_ਅੰਤ
	ਜੇ (flags & P_STRING)
	    ਵਾਪਸ ਸੈੱਟ_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	ਜੇ (varp != ਨਿਰਾ)	// hidden option is not changed
	{
	    ਜੇ (ਸੰਖਿਆ == 0 && string != ਨਿਰਾ)
	    {
		ਪੂਰਨ idx;

		// Either we are given a string or we are setting option
		// to zero.
		ਲਈ (idx = 0; string[idx] == '0'; ++idx)
		    ;
		ਜੇ (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_ਸੰਖਿਆ_required_after_str_equal_str),
								 name, string);
		    ਵਾਪਸ ਨਿਰਾ;     // do nothing as we hit an error

		}
	    }
	    ਜੇ (flags & P_ਅੰਕ)
	    {
		ਵਾਪਸ ਸੈੱਟ_ਅੰਕ_option(opt_idx, varp, ਸੰਖਿਆ,
					   errbuf, ਆਕਾਰ(errbuf), opt_flags);
	    }
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ ਸੈੱਟ_bool_option(opt_idx, varp, (ਪੂਰਨ)ਸੰਖਿਆ, opt_flags);
	}

    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    ਖਾਲੀ
ਸੈੱਟ_option_ਮੁੱਲ_give_err(
    char_u	*name,
    ਲੰਮਾ	ਸੰਖਿਆ,
    char_u	*string,
    ਪੂਰਨ		opt_flags)	// OPT_LOCAL or 0 (both)
{
    ਅੱਖਰ *errmsg = ਸੈੱਟ_option_ਮੁੱਲ(name, ਸੰਖਿਆ, string, opt_flags);

    ਜੇ (errmsg != ਨਿਰਾ)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_ਪਦ_ਕੋਡ(char_u *tname)
{
    ਪੂਰਨ	    opt_idx;
    char_u  *varp;

    ਜੇ (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	ਜੇ (varp != ਨਿਰਾ)
	    varp = *(char_u **)(varp);
	ਵਾਪਸ varp;
    }
    ਵਾਪਸ ਲੱਭੋ_termcode(tname + 2);
}

    char_u *
get_highlight_default(ਖਾਲੀ)
{
    ਪੂਰਨ i;

    i = findoption((char_u *)"hl");
    ਜੇ (i >= 0)
	ਵਾਪਸ options[i].def_val[VI_DEFAULT];
    ਵਾਪਸ (char_u *)ਨਿਰਾ;
}

    char_u *
get_encoding_default(ਖਾਲੀ)
{
    ਪੂਰਨ i;

    i = findoption((char_u *)"enc");
    ਜੇ (i >= 0)
	ਵਾਪਸ options[i].def_val[VI_DEFAULT];
    ਵਾਪਸ (char_u *)ਨਿਰਾ;
}

#ਜੇ defined(FEAT_QUICKFIX)
    ਪੂਰਨ
is_option_allocated(ਅੱਖਰ *name)
{
    ਪੂਰਨ idx = findoption((char_u *)name);

    ਵਾਪਸ idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#ਜੇ_ਅੰਤ

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_ਕੁੰਜੀ_option(char_u *arg_arg, ਪੂਰਨ has_lt)
{
    ਪੂਰਨ		ਕੁੰਜੀ = 0;
    ਪੂਰਨ		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    ਜੇ (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	ਜੇ (!has_lt || arg[4] == '>')
	    ਕੁੰਜੀ = TERMCAP2_ਕੁੰਜੀ(arg[2], arg[3]);
    }
    ਨਹੀਂਤਾਂ ਜੇ (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	ਕੁੰਜੀ = ਲੱਭੋ_special_ਕੁੰਜੀ(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_ਕੁੰਜੀ | FSK_SIMPLIFY, ਨਿਰਾ);
	ਜੇ (modifiers)		    // can't handle modifiers here
	    ਕੁੰਜੀ = 0;
    }
    ਵਾਪਸ ਕੁੰਜੀ;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    ਸਥਿਰ ਖਾਲੀ
showoptions(
    ਪੂਰਨ		all,
    ਪੂਰਨ		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    ਬਣਤਰ vimoption	*p;
    ਪੂਰਨ			ਖਾਨਾ;
    ਪੂਰਨ			isterm;
    char_u		*varp;
    ਬਣਤਰ vimoption	**ਚੀਜ਼ਾਂ;
    ਪੂਰਨ			ਵਸਤੂ_ਗਿਣਤੀ;
    ਪੂਰਨ			ਲੜੀਵਾਰ;
    ਪੂਰਨ			ਪੰਕਤੀ, ਪੰਕਤੀਆਂ_2;
    ਪੂਰਨ			ਖਾਨੇ_ਗਿਣਤੀ;
    ਪੂਰਨ			i;
    ਪੂਰਨ			len;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INC 20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GAP 3

    ਚੀਜ਼ਾਂ = ALLOC_MULT(ਬਣਤਰ vimoption *, OPTION_ਗਿਣਤੀ);
    ਜੇ (ਚੀਜ਼ਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;

    // Highlight title
    ਜੇ (all == 2)
	ਸੁਨੇਹਾ_puts_title(_("\n--- Terminal codes ---"));
    ਨਹੀਂਤਾਂ ਜੇ (opt_flags & OPT_GLOBAL)
	ਸੁਨੇਹਾ_puts_title(_("\n--- Global option values ---"));
    ਨਹੀਂਤਾਂ ਜੇ (opt_flags & OPT_LOCAL)
	ਸੁਨੇਹਾ_puts_title(_("\n--- Local option values ---"));
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    ਲਈ (ਲੜੀਵਾਰ = 1; ਲੜੀਵਾਰ <= 2 && !got_int; ++ਲੜੀਵਾਰ)
    {
	// collect the items in items[]
	ਵਸਤੂ_ਗਿਣਤੀ = 0;
	ਲਈ (p = &options[0]; p->fullname != ਨਿਰਾ; p++)
	{
	    // apply :filter /pat/
	    ਜੇ (ਸੁਨੇਹਾ_filtered((char_u *)p->fullname))
		ਜਾਰੀ;

	    varp = ਨਿਰਾ;
	    isterm = istermoption(p);
	    ਜੇ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		ਜੇ (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    ਨਹੀਂਤਾਂ
		varp = get_varp(p);
	    ਜੇ (varp != ਨਿਰਾ
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		ਜੇ (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		ਨਹੀਂਤਾਂ ਜੇ (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		ਨਹੀਂਤਾਂ
		{
		    option_value2string(p, opt_flags);
		    len = (ਪੂਰਨ)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		ਜੇ ((len <= INC - GAP && ਲੜੀਵਾਰ == 1) ||
						(len > INC - GAP && ਲੜੀਵਾਰ == 2))
		    ਚੀਜ਼ਾਂ[ਵਸਤੂ_ਗਿਣਤੀ++] = p;
	    }
	}

	// display the items
	ਜੇ (ਲੜੀਵਾਰ == 1)
	{
	    ਖਾਨੇ_ਗਿਣਤੀ = (Columns + GAP - 3) / INC;
	    ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ == 0)
		ਖਾਨੇ_ਗਿਣਤੀ = 1;
	    ਪੰਕਤੀਆਂ_2 = (ਵਸਤੂ_ਗਿਣਤੀ + ਖਾਨੇ_ਗਿਣਤੀ - 1) / ਖਾਨੇ_ਗਿਣਤੀ;
	}
	ਨਹੀਂਤਾਂ	// run == 2
	    ਪੰਕਤੀਆਂ_2 = ਵਸਤੂ_ਗਿਣਤੀ;
	ਲਈ (ਪੰਕਤੀ = 0; ਪੰਕਤੀ < ਪੰਕਤੀਆਂ_2 && !got_int; ++ਪੰਕਤੀ)
	{
	    ਸੁਨੇਹਾ_putchar('\n');			// go to next line
	    ਜੇ (got_int)			// 'q' typed in more
		ਰੋਕੋ;
	    ਖਾਨਾ = 0;
	    ਲਈ (i = ਪੰਕਤੀ; i < ਵਸਤੂ_ਗਿਣਤੀ; i += ਪੰਕਤੀਆਂ_2)
	    {
		ਸੁਨੇਹਾ_ਖਾਨਾ = ਖਾਨਾ;			// make columns
		showoneopt(ਚੀਜ਼ਾਂ[i], opt_flags);
		ਖਾਨਾ += INC;
	    }
	    ਬਾਹਰਲਾ_flush();
	    ui_breakcheck();
	}
    }
    vim_free(ਚੀਜ਼ਾਂ);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    ਸਥਿਰ ਪੂਰਨ
optval_default(ਬਣਤਰ vimoption *p, char_u *varp, ਪੂਰਨ compatible)
{
    ਪੂਰਨ		dvi;

    ਜੇ (varp == ਨਿਰਾ)
	ਵਾਪਸ TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    ਜੇ (p->flags & P_ਅੰਕ)
	ਵਾਪਸ (*(ਲੰਮਾ *)varp == (ਲੰਮਾ)(long_i)p->def_val[dvi]);
    ਜੇ (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	ਵਾਪਸ (*(ਪੂਰਨ *)varp == (ਪੂਰਨ)(ਲੰਮਾ)(long_i)p->def_val[dvi]);
    // P_STRING
    ਵਾਪਸ (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    ਸਥਿਰ ਖਾਲੀ
showoneopt(
    ਬਣਤਰ vimoption	*p,
    ਪੂਰਨ			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    ਪੂਰਨ		save_silent = silent_ਬਹੁਲਕ;

    silent_ਬਹੁਲਕ = FALSE;
    info_ਸੁਨੇਹਾ = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    ਜੇ ((p->flags & P_BOOL) && ((ਪੂਰਨ *)varp == &curbuf->b_ਬਦਲਿਆ
					? !curbuf_Is_ਬਦਲਿਆ() : !*(ਪੂਰਨ *)varp))
	ਸੁਨੇਹਾ_puts("no");
    ਨਹੀਂਤਾਂ ਜੇ ((p->flags & P_BOOL) && *(ਪੂਰਨ *)varp < 0)
	ਸੁਨੇਹਾ_puts("--");
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_puts("  ");
    ਸੁਨੇਹਾ_puts(p->fullname);
    ਜੇ (!(p->flags & P_BOOL))
    {
	ਸੁਨੇਹਾ_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	ਸੁਨੇਹਾ_outtrans(NameBuff);
    }

    silent_ਬਹੁਲਕ = save_silent;
    info_ਸੁਨੇਹਾ = FALSE;
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
    ਪੂਰਨ
makeset(FILE *fd, ਪੂਰਨ opt_flags, ਪੂਰਨ local_only)
{
    ਬਣਤਰ vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = ਨਿਰਾ;	// fresh value
    ਅੱਖਰ		*cmd;
    ਪੂਰਨ			ਗੇੜਾ;
    ਪੂਰਨ			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    ਲਈ (pri = 1; pri >= 0; --pri)
    {
      ਲਈ (p = &options[0]; !istermoption(p); p++)
	ਜੇ (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    ਜੇ (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		ਜਾਰੀ;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    ਜੇ ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		ਜਾਰੀ;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    ਜੇ ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		ਜਾਰੀ;

	    ਜੇ ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		ਜਾਰੀ;

	    ਗੇੜਾ = 2;
	    ਜੇ (p->indir != PV_NONE)
	    {
		ਜੇ (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    ਜੇ (!(opt_flags & OPT_LOCAL))
			ਜਾਰੀ;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    ਜੇ (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			ਜੇ (!optval_default(p, varp_fresh, p_cp))
			{
			    ਗੇੜਾ = 1;
			    varp_local = varp;
			    varp = varp_fresh;
			}
		    }
		}
	    }

	    // Round 1: fresh value for window-local options.
	    // Round 2: other values
	    ਲਈ ( ; ਗੇੜਾ <= 2; varp = varp_local, ++ਗੇੜਾ)
	    {
		ਜੇ (ਗੇੜਾ == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		ਨਹੀਂਤਾਂ
		    cmd = "setlocal";

		ਜੇ (p->flags & P_BOOL)
		{
		    ਜੇ (put_setbool(fd, cmd, p->fullname, *(ਪੂਰਨ *)varp) == FAIL)
			ਵਾਪਸ FAIL;
		}
		ਨਹੀਂਤਾਂ ਜੇ (p->flags & P_ਅੰਕ)
		{
		    ਜੇ (put_setnum(fd, cmd, p->fullname, (ਲੰਮਾ *)varp) == FAIL)
			ਵਾਪਸ FAIL;
		}
		ਨਹੀਂਤਾਂ    // P_STRING
		{
		    ਪੂਰਨ		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    ਜੇ ((p->flags & P_FUNC) && *(char_u **)varp != ਨਿਰਾ
			    && ਲੜੀ_ਖੋਜ(*(ਅੱਖਰ **)varp, "<lambda>") != ਨਿਰਾ)
			ਜਾਰੀ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    legacy = !is_option_ਮੁੱਲ_vim9(p - &options[0],
			    ਗੇੜਾ == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#ਨਹੀਂਤਾਂ
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#ਜੇ_ਅੰਤ

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    ਜੇ (
#ਜੇ defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#ਜੇ_ਅੰਤ
			    p->indir == PV_FT)
		    {
			ਜੇ (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    ਵਾਪਸ FAIL;
			do_endif = TRUE;
		    }
		    ਜੇ (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			ਵਾਪਸ FAIL;
		    ਜੇ (do_endif)
		    {
			ਜੇ (put_ਸਤਰ(fd, "endif") == FAIL)
			    ਵਾਪਸ FAIL;
		    }
		}
	    }
	}
    }
    ਵਾਪਸ OK;
}

#ਜੇ defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    ਪੂਰਨ
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_ਸੰਸਕਰਣ < SCRIPT_ਸੰਸਕਰਣ_VIM9;
# endif

    ਜੇ (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਪੂਰਨ
put_setstring(
    FILE	*fd,
    bool	legacy,
    ਅੱਖਰ	*cmd,
    ਅੱਖਰ	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = ਨਿਰਾ;
    char_u	*part = ਨਿਰਾ;
    char_u	*p;

    ਜੇ (legacy && fprintf(fd, "legacy ") < 0)
	ਵਾਪਸ FAIL;
    ਜੇ (fprintf(fd, "%s %s=", cmd, name) < 0)
	ਵਾਪਸ FAIL;
    ਜੇ (*valuep != ਨਿਰਾ)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	ਜੇ (valuep == &p_pt)
	{
	    s = *valuep;
	    ਜਦੋਂ (*s != NUL)
		ਜੇ (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    ਵਾਪਸ FAIL;
	}
	// expand the option value, replace $HOME by ~
	ਨਹੀਂਤਾਂ ਜੇ ((flags & P_EXPAND) != 0)
	{
	    ਪੂਰਨ  ਮਾਪ = (ਪੂਰਨ)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(ਮਾਪ);
	    ਜੇ (buf == ਨਿਰਾ)
		ਜਾਓ fail;
	    home_replace(ਨਿਰਾ, *valuep, buf, ਮਾਪ, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    ਜੇ (ਮਾਪ >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != ਨਿਰਾ)
	    {
		part = alloc(ਮਾਪ);
		ਜੇ (part == ਨਿਰਾ)
		    ਜਾਓ fail;

		// write line break to clear the option, e.g. ':set rtp='
		ਜੇ (put_eol(fd) == FAIL)
		    ਜਾਓ fail;

		p = buf;
		ਜਦੋਂ (*p != NUL)
		{
		    ਜੇ (legacy && fprintf(fd, "legacy ") < 0)
			ਵਾਪਸ FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    ਜੇ (fprintf(fd, "%s %s+=", cmd, name) < 0)
			ਜਾਓ fail;
		    (ਖਾਲੀ)copy_option_part(&p, part, ਮਾਪ,  ",");
		    ਜੇ (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			ਜਾਓ fail;
		}
		vim_free(buf);
		vim_free(part);
		ਵਾਪਸ OK;
	    }
	    ਜੇ (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		ਵਾਪਸ FAIL;
	    }
	    vim_free(buf);
	}
	ਨਹੀਂਤਾਂ ਜੇ (put_escstr(fd, *valuep, 2) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਜੇ (put_eol(fd) < 0)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
fail:
    vim_free(buf);
    vim_free(part);
    ਵਾਪਸ FAIL;
}

    ਸਥਿਰ ਪੂਰਨ
put_setnum(
    FILE	*fd,
    ਅੱਖਰ	*cmd,
    ਅੱਖਰ	*name,
    ਲੰਮਾ	*valuep)
{
    ਲੰਮਾ	wc;

    ਜੇ (fprintf(fd, "%s %s=", cmd, name) < 0)
	ਵਾਪਸ FAIL;
    ਜੇ (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	ਜੇ (fputs((ਅੱਖਰ *)get_special_ਕੁੰਜੀ_name((ਪੂਰਨ)wc, 0), fd) < 0)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (fprintf(fd, "%ld", *valuep) < 0)
	ਵਾਪਸ FAIL;
    ਜੇ (put_eol(fd) < 0)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
put_setbool(
    FILE	*fd,
    ਅੱਖਰ	*cmd,
    ਅੱਖਰ	*name,
    ਪੂਰਨ		ਮੁੱਲ)
{
    ਜੇ (ਮੁੱਲ < 0)	// global/local option using global value
	ਵਾਪਸ OK;
    ਜੇ (fprintf(fd, "%s %s%s", cmd, ਮੁੱਲ ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    ਖਾਲੀ
clear_termoptions(ਖਾਲੀ)
{
    // Reset a few things before clearing the old options. This may cause
    // outputting a few things that the terminal doesn't understand, but the
    // screen will be cleared later, so this is OK.
    mch_setmouse(FALSE);	    // switch mouse off
    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window titles
#ਜੇ defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    ਜੇ (gui.starting)
	clear_xterm_clip();
#ਜੇ_ਅੰਤ
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    ਖਾਲੀ
free_termoptions(ਖਾਲੀ)
{
    ਬਣਤਰ vimoption   *p;

    ਲਈ (p = options; p->fullname != ਨਿਰਾ; p++)
	ਜੇ (istermoption(p))
	{
	    ਜੇ (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    ਜੇ (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    p->def_val[VI_DEFAULT] = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    // remember where the option was cleared
	    ਸੈੱਟ_option_sctx_idx((ਪੂਰਨ)(p - options), OPT_GLOBAL, current_sctx);
#ਜੇ_ਅੰਤ
	}
    clear_termcodes();
}

/*
 * Free the string for one term option, if it was allocated.
 * Set the string to empty_option and clear allocated flag.
 * "var" points to the option value.
 */
    ਖਾਲੀ
free_one_termoption(char_u *var)
{
    ਬਣਤਰ vimoption   *p;

    ਲਈ (p = &options[0]; p->fullname != ਨਿਰਾ; p++)
	ਜੇ (p->var == var)
	{
	    ਜੇ (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    p->flags &= ~P_ALLOCED;
	    ਰੋਕੋ;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    ਖਾਲੀ
ਸੈੱਟ_ਪਦ_defaults(ਖਾਲੀ)
{
    ਬਣਤਰ vimoption   *p;

    ਲਈ (p = &options[0]; p->fullname != ਨਿਰਾ; p++)
    {
	ਜੇ (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    ਜੇ (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    ਜੇ (p->flags & P_ALLOCED)
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
    ਸਥਿਰ ਪੂਰਨ
istermoption(ਬਣਤਰ vimoption *p)
{
    ਵਾਪਸ (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    ਸਥਿਰ ਪੂਰਨ
istermoption_idx(ਪੂਰਨ opt_idx)
{
    ਵਾਪਸ istermoption(&options[opt_idx]);
}

#ਜੇ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    ਖਾਲੀ
unset_global_local_option(char_u *name, ਖਾਲੀ *from)
{
    ਬਣਤਰ vimoption *p;
    ਪੂਰਨ		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    ਜੇ (opt_idx < 0)
	ਵਾਪਸ;
    p = &(options[opt_idx]);

    ਚੋਣ ((ਪੂਰਨ)p->indir)
    {
	// global option with local value: use local value if it's been set
	ਹਾਲਤ PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_ਰਸਤਾ:
	    clear_string_option(&buf->b_p_ਰਸਤਾ);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_AC:
	    buf->b_p_ac = -1;
	    ਰੋਕੋ;
	ਹਾਲਤ PV_AR:
	    buf->b_p_ar = -1;
	    ਰੋਕੋ;
# ifdef HAVE_FSYNC
	ਹਾਲਤ PV_FS:
	    buf->b_p_fs = -1;
	    ਰੋਕੋ;
# endif
	ਹਾਲਤ PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    ਰੋਕੋ;
	ਹਾਲਤ PV_ਟੈਗ_ਸੂਚੀ:
	    clear_string_option(&buf->b_p_ਟੈਗ_ਸੂਚੀ);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    ਰੋਕੋ;
	ਹਾਲਤ PV_SISO:
	    curwin->w_p_siso = -1;
	    ਰੋਕੋ;
	ਹਾਲਤ PV_SO:
	    curwin->w_p_so = -1;
	    ਰੋਕੋ;
	ਹਾਲਤ PV_SOP:
	    curwin->w_p_sop = -1;
	    ਰੋਕੋ;
# ifdef FEAT_ਲੱਭੋ_ID
	ਹਾਲਤ PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    ਰੋਕੋ;
# endif
	ਹਾਲਤ PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    ਰੋਕੋ;
	ਹਾਲਤ PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    ਰੋਕੋ;
# ifdef FEAT_DIFF
	ਹਾਲਤ PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    ਰੋਕੋ;
# endif
	ਹਾਲਤ PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    ਰੋਕੋ;
# ifdef FEAT_COMPL_FUNC
	ਹਾਲਤ PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    ਰੋਕੋ;
# endif
	ਹਾਲਤ PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    ਰੋਕੋ;
# ifdef FEAT_EVAL
	ਹਾਲਤ PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    ਰੋਕੋ;
# endif
# ifdef FEAT_QUICKFIX
	ਹਾਲਤ PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    ਰੋਕੋ;
# endif
# ਜੇ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	ਹਾਲਤ PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    ਰੋਕੋ;
# endif
# ਜੇ defined(FEAT_CRYPT)
	ਹਾਲਤ PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    ਰੋਕੋ;
# endif
# ifdef FEAT_LINEBREAK
	ਹਾਲਤ PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    ਰੋਕੋ;
# endif
# ifdef FEAT_STL_OPT
	ਹਾਲਤ PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    ਰੋਕੋ;
# endif
	ਹਾਲਤ PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    ਰੋਕੋ;
	ਹਾਲਤ PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    ਸੈੱਟ_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      ਨਿਰਾ, 0);
	    redraw_later(UPD_NOT_VALID);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    ਸੈੱਟ_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      ਨਿਰਾ, 0);
	    redraw_later(UPD_NOT_VALID);
	    ਰੋਕੋ;
	ਹਾਲਤ PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    ਰੋਕੋ;
    }
}
#ਜੇ_ਅੰਤ

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    ਸਥਿਰ char_u *
get_varp_scope(ਬਣਤਰ vimoption *p, ਪੂਰਨ scope)
{
    ਜੇ ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	ਜੇ (p->var == VAR_WIN)
	    ਵਾਪਸ (char_u *)GLOBAL_WO(get_varp(p));
	ਵਾਪਸ p->var;
    }
    ਜੇ ((scope & OPT_LOCAL) && ((ਪੂਰਨ)p->indir & PV_BOTH))
    {
	ਚੋਣ ((ਪੂਰਨ)p->indir)
	{
	    ਹਾਲਤ PV_FP:   ਵਾਪਸ (char_u *)&(curbuf->b_p_fp);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FSYNC
	    ਹਾਲਤ PV_FS:	ਵਾਪਸ (char_u *)&(curbuf->b_p_fs);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਹਾਲਤ PV_FFU: ਵਾਪਸ (char_u *)&(curbuf->b_p_ffu);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	    ਹਾਲਤ PV_EFM:  ਵਾਪਸ (char_u *)&(curbuf->b_p_efm);
	    ਹਾਲਤ PV_GEFM: ਵਾਪਸ (char_u *)&(curbuf->b_p_gefm);
	    ਹਾਲਤ PV_GP:   ਵਾਪਸ (char_u *)&(curbuf->b_p_gp);
	    ਹਾਲਤ PV_MP:   ਵਾਪਸ (char_u *)&(curbuf->b_p_mp);
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ PV_EP:   ਵਾਪਸ (char_u *)&(curbuf->b_p_ep);
	    ਹਾਲਤ PV_KP:   ਵਾਪਸ (char_u *)&(curbuf->b_p_kp);
	    ਹਾਲਤ PV_ਰਸਤਾ: ਵਾਪਸ (char_u *)&(curbuf->b_p_ਰਸਤਾ);
	    ਹਾਲਤ PV_AC:   ਵਾਪਸ (char_u *)&(curbuf->b_p_ac);
	    ਹਾਲਤ PV_AR:   ਵਾਪਸ (char_u *)&(curbuf->b_p_ar);
	    ਹਾਲਤ PV_ਟੈਗ_ਸੂਚੀ: ਵਾਪਸ (char_u *)&(curbuf->b_p_ਟੈਗ_ਸੂਚੀ);
	    ਹਾਲਤ PV_TC:   ਵਾਪਸ (char_u *)&(curbuf->b_p_tc);
	    ਹਾਲਤ PV_SISO: ਵਾਪਸ (char_u *)&(curwin->w_p_siso);
	    ਹਾਲਤ PV_SO:   ਵਾਪਸ (char_u *)&(curwin->w_p_so);
	    ਹਾਲਤ PV_SOP:  ਵਾਪਸ (char_u *)&(curwin->w_p_sop);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
	    ਹਾਲਤ PV_DEF:  ਵਾਪਸ (char_u *)&(curbuf->b_p_def);
	    ਹਾਲਤ PV_INC:  ਵਾਪਸ (char_u *)&(curbuf->b_p_inc);
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ PV_COT:  ਵਾਪਸ (char_u *)&(curbuf->b_p_cot);
	    ਹਾਲਤ PV_DICT: ਵਾਪਸ (char_u *)&(curbuf->b_p_dict);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    ਹਾਲਤ PV_DIA:  ਵਾਪਸ (char_u *)&(curbuf->b_p_dia);
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ PV_TSR:  ਵਾਪਸ (char_u *)&(curbuf->b_p_tsr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
	    ਹਾਲਤ PV_TSRFU: ਵਾਪਸ (char_u *)&(curbuf->b_p_tsrfu);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    ਹਾਲਤ PV_BEXPR: ਵਾਪਸ (char_u *)&(curbuf->b_p_bexpr);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_CRYPT)
	    ਹਾਲਤ PV_CM:	  ਵਾਪਸ (char_u *)&(curbuf->b_p_cm);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	    ਹਾਲਤ PV_SBR:  ਵਾਪਸ (char_u *)&(curwin->w_p_sbr);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
	    ਹਾਲਤ PV_STL:  ਵਾਪਸ (char_u *)&(curwin->w_p_stl);
	    ਹਾਲਤ PV_STLO: ਵਾਪਸ (char_u *)&(curwin->w_p_stlo);
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ PV_UL:   ਵਾਪਸ (char_u *)&(curbuf->b_p_ul);
	    ਹਾਲਤ PV_LW:   ਵਾਪਸ (char_u *)&(curbuf->b_p_lw);
	    ਹਾਲਤ PV_BKC:  ਵਾਪਸ (char_u *)&(curbuf->b_p_bkc);
	    ਹਾਲਤ PV_MENC: ਵਾਪਸ (char_u *)&(curbuf->b_p_menc);
	    ਹਾਲਤ PV_LCS:  ਵਾਪਸ (char_u *)&(curwin->w_p_lcs);
	    ਹਾਲਤ PV_FCS:  ਵਾਪਸ (char_u *)&(curwin->w_p_fcs);
	    ਹਾਲਤ PV_VE:	  ਵਾਪਸ (char_u *)&(curwin->w_p_ve);

	}
	ਵਾਪਸ ਨਿਰਾ; // "cannot happen"
    }
    ਵਾਪਸ get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(ਪੂਰਨ opt_idx, ਪੂਰਨ scope)
{
    ਵਾਪਸ get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    ਸਥਿਰ char_u *
get_varp(ਬਣਤਰ vimoption *p)
{
    // hidden option, always return NULL
    ਜੇ (p->var == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਚੋਣ ((ਪੂਰਨ)p->indir)
    {
	ਹਾਲਤ PV_NONE:	ਵਾਪਸ p->var;

	// global option with local value: use local value if it's been set
	ਹਾਲਤ PV_EP:	ਵਾਪਸ *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	ਹਾਲਤ PV_KP:	ਵਾਪਸ *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	ਹਾਲਤ PV_ਰਸਤਾ:	ਵਾਪਸ *curbuf->b_p_ਰਸਤਾ != NUL
				    ? (char_u *)&(curbuf->b_p_ਰਸਤਾ) : p->var;
	ਹਾਲਤ PV_AC:	ਵਾਪਸ curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	ਹਾਲਤ PV_AR:	ਵਾਪਸ curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	ਹਾਲਤ PV_ਟੈਗ_ਸੂਚੀ:	ਵਾਪਸ *curbuf->b_p_ਟੈਗ_ਸੂਚੀ != NUL
				    ? (char_u *)&(curbuf->b_p_ਟੈਗ_ਸੂਚੀ) : p->var;
	ਹਾਲਤ PV_TC:	ਵਾਪਸ *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	ਹਾਲਤ PV_BKC:	ਵਾਪਸ *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	ਹਾਲਤ PV_SISO:	ਵਾਪਸ curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	ਹਾਲਤ PV_SO:	ਵਾਪਸ curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	ਹਾਲਤ PV_SOP:	ਵਾਪਸ curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
	ਹਾਲਤ PV_DEF:	ਵਾਪਸ *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	ਹਾਲਤ PV_INC:	ਵਾਪਸ *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_COT:	ਵਾਪਸ *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	ਹਾਲਤ PV_DICT:	ਵਾਪਸ *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	ਹਾਲਤ PV_DIA:	ਵਾਪਸ *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_TSR:	ਵਾਪਸ *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
	ਹਾਲਤ PV_TSRFU:	ਵਾਪਸ *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_FP:	ਵਾਪਸ *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FSYNC
	ਹਾਲਤ PV_FS:	ਵਾਪਸ curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਹਾਲਤ PV_FFU:	ਵਾਪਸ *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	ਹਾਲਤ PV_EFM:	ਵਾਪਸ *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	ਹਾਲਤ PV_GEFM:	ਵਾਪਸ *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	ਹਾਲਤ PV_GP:	ਵਾਪਸ *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	ਹਾਲਤ PV_MP:	ਵਾਪਸ *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	ਹਾਲਤ PV_BEXPR:	ਵਾਪਸ *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_CRYPT)
	ਹਾਲਤ PV_CM:	ਵਾਪਸ *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	ਹਾਲਤ PV_SBR:	ਵਾਪਸ *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
	ਹਾਲਤ PV_STL:	ਵਾਪਸ *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	ਹਾਲਤ PV_STLO:	ਵਾਪਸ *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_UL:	ਵਾਪਸ curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	ਹਾਲਤ PV_LW:	ਵਾਪਸ *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	ਹਾਲਤ PV_MENC:	ਵਾਪਸ *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
	ਹਾਲਤ PV_ARAB:	ਵਾਪਸ (char_u *)&(curwin->w_p_arab);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_LIST:	ਵਾਪਸ (char_u *)&(curwin->w_p_list);
	ਹਾਲਤ PV_LCS:	ਵਾਪਸ *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	ਹਾਲਤ PV_FCS:	ਵਾਪਸ *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	ਹਾਲਤ PV_VE:	ਵਾਪਸ *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	ਹਾਲਤ PV_SPELL:	ਵਾਪਸ (char_u *)&(curwin->w_p_spell);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਹਾਲਤ PV_CUC:	ਵਾਪਸ (char_u *)&(curwin->w_p_cuc);
	ਹਾਲਤ PV_CUL:	ਵਾਪਸ (char_u *)&(curwin->w_p_cul);
	ਹਾਲਤ PV_CULOPT:	ਵਾਪਸ (char_u *)&(curwin->w_p_culopt);
	ਹਾਲਤ PV_CC:	ਵਾਪਸ (char_u *)&(curwin->w_p_cc);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	ਹਾਲਤ PV_DIFF:	ਵਾਪਸ (char_u *)&(curwin->w_p_diff);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_EIW:	ਵਾਪਸ (char_u *)&(curwin->w_p_eiw);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	ਹਾਲਤ PV_FDC:	ਵਾਪਸ (char_u *)&(curwin->w_p_fdc);
	ਹਾਲਤ PV_FEN:	ਵਾਪਸ (char_u *)&(curwin->w_p_fen);
	ਹਾਲਤ PV_FDI:	ਵਾਪਸ (char_u *)&(curwin->w_p_fdi);
	ਹਾਲਤ PV_FDL:	ਵਾਪਸ (char_u *)&(curwin->w_p_fdl);
	ਹਾਲਤ PV_FDM:	ਵਾਪਸ (char_u *)&(curwin->w_p_fdm);
	ਹਾਲਤ PV_FML:	ਵਾਪਸ (char_u *)&(curwin->w_p_fml);
	ਹਾਲਤ PV_FDN:	ਵਾਪਸ (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	ਹਾਲਤ PV_FDE:	ਵਾਪਸ (char_u *)&(curwin->w_p_fde);
	ਹਾਲਤ PV_FDT:	ਵਾਪਸ (char_u *)&(curwin->w_p_fdt);
# endif
	ਹਾਲਤ PV_FMR:	ਵਾਪਸ (char_u *)&(curwin->w_p_fmr);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_NU:	ਵਾਪਸ (char_u *)&(curwin->w_p_nu);
	ਹਾਲਤ PV_RNU:	ਵਾਪਸ (char_u *)&(curwin->w_p_rnu);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	ਹਾਲਤ PV_NUW:	ਵਾਪਸ (char_u *)&(curwin->w_p_nuw);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_WFB:	ਵਾਪਸ (char_u *)&(curwin->w_p_wfb);
	ਹਾਲਤ PV_WFH:	ਵਾਪਸ (char_u *)&(curwin->w_p_wfh);
	ਹਾਲਤ PV_WFW:	ਵਾਪਸ (char_u *)&(curwin->w_p_wfw);
#ਜੇ defined(FEAT_QUICKFIX)
	ਹਾਲਤ PV_PVW:	ਵਾਪਸ (char_u *)&(curwin->w_p_pvw);
	ਹਾਲਤ PV_LHI:	ਵਾਪਸ (char_u *)&(curwin->w_p_lhi);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਹਾਲਤ PV_RL:	ਵਾਪਸ (char_u *)&(curwin->w_p_rl);
	ਹਾਲਤ PV_RLC:	ਵਾਪਸ (char_u *)&(curwin->w_p_rlc);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_SCROLL:	ਵਾਪਸ (char_u *)&(curwin->w_p_scr);
	ਹਾਲਤ PV_SMS:	ਵਾਪਸ (char_u *)&(curwin->w_p_sms);
	ਹਾਲਤ PV_WRAP:	ਵਾਪਸ (char_u *)&(curwin->w_p_wrap);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	ਹਾਲਤ PV_LBR:	ਵਾਪਸ (char_u *)&(curwin->w_p_lbr);
	ਹਾਲਤ PV_BRI:	ਵਾਪਸ (char_u *)&(curwin->w_p_bri);
	ਹਾਲਤ PV_BRIOPT: ਵਾਪਸ (char_u *)&(curwin->w_p_briopt);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_WCR:	ਵਾਪਸ (char_u *)&(curwin->w_p_wcr);
	ਹਾਲਤ PV_SCBIND: ਵਾਪਸ (char_u *)&(curwin->w_p_scb);
	ਹਾਲਤ PV_CRBIND: ਵਾਪਸ (char_u *)&(curwin->w_p_crb);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	ਹਾਲਤ PV_COCU:   ਵਾਪਸ (char_u *)&(curwin->w_p_cocu);
	ਹਾਲਤ PV_COLE:   ਵਾਪਸ (char_u *)&(curwin->w_p_cole);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_WHL:	ਵਾਪਸ (char_u *)&(curwin->w_p_whl);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	ਹਾਲਤ PV_TWK:    ਵਾਪਸ (char_u *)&(curwin->w_p_twk);
	ਹਾਲਤ PV_TWS:    ਵਾਪਸ (char_u *)&(curwin->w_p_tws);
	ਹਾਲਤ PV_TWSL:	ਵਾਪਸ (char_u *)&(curbuf->b_p_twsl);
#ਜੇ_ਅੰਤ

	ਹਾਲਤ PV_AI:	ਵਾਪਸ (char_u *)&(curbuf->b_p_ai);
	ਹਾਲਤ PV_BIN:	ਵਾਪਸ (char_u *)&(curbuf->b_p_bin);
	ਹਾਲਤ PV_BOMB:	ਵਾਪਸ (char_u *)&(curbuf->b_p_bomb);
	ਹਾਲਤ PV_BH:	ਵਾਪਸ (char_u *)&(curbuf->b_p_bh);
	ਹਾਲਤ PV_BT:	ਵਾਪਸ (char_u *)&(curbuf->b_p_bt);
	ਹਾਲਤ PV_BL:	ਵਾਪਸ (char_u *)&(curbuf->b_p_bl);
	ਹਾਲਤ PV_CI:	ਵਾਪਸ (char_u *)&(curbuf->b_p_ci);
	ਹਾਲਤ PV_CIN:	ਵਾਪਸ (char_u *)&(curbuf->b_p_cin);
	ਹਾਲਤ PV_CINK:	ਵਾਪਸ (char_u *)&(curbuf->b_p_cink);
	ਹਾਲਤ PV_CINO:	ਵਾਪਸ (char_u *)&(curbuf->b_p_cino);
	ਹਾਲਤ PV_CINSD:	ਵਾਪਸ (char_u *)&(curbuf->b_p_cinsd);
	ਹਾਲਤ PV_CINW:	ਵਾਪਸ (char_u *)&(curbuf->b_p_cinw);
	ਹਾਲਤ PV_COM:	ਵਾਪਸ (char_u *)&(curbuf->b_p_com);
	ਹਾਲਤ PV_CMS:	ਵਾਪਸ (char_u *)&(curbuf->b_p_cms);
	ਹਾਲਤ PV_CPT:	ਵਾਪਸ (char_u *)&(curbuf->b_p_cpt);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	ਹਾਲਤ PV_CSL:	ਵਾਪਸ (char_u *)&(curbuf->b_p_csl);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
	ਹਾਲਤ PV_CFU:	ਵਾਪਸ (char_u *)&(curbuf->b_p_cfu);
	ਹਾਲਤ PV_OFU:	ਵਾਪਸ (char_u *)&(curbuf->b_p_ofu);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਹਾਲਤ PV_TFU:	ਵਾਪਸ (char_u *)&(curbuf->b_p_tfu);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_EOF:	ਵਾਪਸ (char_u *)&(curbuf->b_p_eof);
	ਹਾਲਤ PV_EOL:	ਵਾਪਸ (char_u *)&(curbuf->b_p_eol);
	ਹਾਲਤ PV_FIXEOL:	ਵਾਪਸ (char_u *)&(curbuf->b_p_fixeol);
	ਹਾਲਤ PV_ET:	ਵਾਪਸ (char_u *)&(curbuf->b_p_et);
	ਹਾਲਤ PV_FENC:	ਵਾਪਸ (char_u *)&(curbuf->b_p_fenc);
	ਹਾਲਤ PV_FF:	ਵਾਪਸ (char_u *)&(curbuf->b_p_ff);
	ਹਾਲਤ PV_FT:	ਵਾਪਸ (char_u *)&(curbuf->b_p_ft);
	ਹਾਲਤ PV_FO:	ਵਾਪਸ (char_u *)&(curbuf->b_p_fo);
	ਹਾਲਤ PV_FLP:	ਵਾਪਸ (char_u *)&(curbuf->b_p_flp);
	ਹਾਲਤ PV_IMI:	ਵਾਪਸ (char_u *)&(curbuf->b_p_iminsert);
	ਹਾਲਤ PV_IMS:	ਵਾਪਸ (char_u *)&(curbuf->b_p_imsearch);
	ਹਾਲਤ PV_INF:	ਵਾਪਸ (char_u *)&(curbuf->b_p_inf);
	ਹਾਲਤ PV_ISK:	ਵਾਪਸ (char_u *)&(curbuf->b_p_isk);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
# ifdef FEAT_EVAL
	ਹਾਲਤ PV_INEX:	ਵਾਪਸ (char_u *)&(curbuf->b_p_inex);
# endif
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_EVAL)
	ਹਾਲਤ PV_INDE:	ਵਾਪਸ (char_u *)&(curbuf->b_p_inde);
	ਹਾਲਤ PV_INDK:	ਵਾਪਸ (char_u *)&(curbuf->b_p_indk);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਹਾਲਤ PV_FEX:	ਵਾਪਸ (char_u *)&(curbuf->b_p_fex);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	ਹਾਲਤ PV_ਕੁੰਜੀ:	ਵਾਪਸ (char_u *)&(curbuf->b_p_ਕੁੰਜੀ);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_LISP:	ਵਾਪਸ (char_u *)&(curbuf->b_p_lisp);
	ਹਾਲਤ PV_LOP:	ਵਾਪਸ (char_u *)&(curbuf->b_p_lop);
	ਹਾਲਤ PV_ML:	ਵਾਪਸ (char_u *)&(curbuf->b_p_ml);
	ਹਾਲਤ PV_MPS:	ਵਾਪਸ (char_u *)&(curbuf->b_p_mps);
	ਹਾਲਤ PV_MA:	ਵਾਪਸ (char_u *)&(curbuf->b_p_ma);
	ਹਾਲਤ PV_MOD:	ਵਾਪਸ (char_u *)&(curbuf->b_ਬਦਲਿਆ);
	ਹਾਲਤ PV_NF:	ਵਾਪਸ (char_u *)&(curbuf->b_p_nf);
	ਹਾਲਤ PV_PI:	ਵਾਪਸ (char_u *)&(curbuf->b_p_pi);
	ਹਾਲਤ PV_QE:	ਵਾਪਸ (char_u *)&(curbuf->b_p_qe);
	ਹਾਲਤ PV_RO:	ਵਾਪਸ (char_u *)&(curbuf->b_p_ro);
	ਹਾਲਤ PV_SI:	ਵਾਪਸ (char_u *)&(curbuf->b_p_si);
	ਹਾਲਤ PV_SN:	ਵਾਪਸ (char_u *)&(curbuf->b_p_sn);
	ਹਾਲਤ PV_STS:	ਵਾਪਸ (char_u *)&(curbuf->b_p_sts);
	ਹਾਲਤ PV_SUA:	ਵਾਪਸ (char_u *)&(curbuf->b_p_sua);
	ਹਾਲਤ PV_SWF:	ਵਾਪਸ (char_u *)&(curbuf->b_p_swf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਹਾਲਤ PV_SMC:	ਵਾਪਸ (char_u *)&(curbuf->b_p_smc);
	ਹਾਲਤ PV_SYN:	ਵਾਪਸ (char_u *)&(curbuf->b_p_syn);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	ਹਾਲਤ PV_SPC:	ਵਾਪਸ (char_u *)&(curwin->w_s->b_p_spc);
	ਹਾਲਤ PV_SPF:	ਵਾਪਸ (char_u *)&(curwin->w_s->b_p_spf);
	ਹਾਲਤ PV_SPL:	ਵਾਪਸ (char_u *)&(curwin->w_s->b_p_spl);
	ਹਾਲਤ PV_SPO:	ਵਾਪਸ (char_u *)&(curwin->w_s->b_p_spo);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_SW:	ਵਾਪਸ (char_u *)&(curbuf->b_p_sw);
	ਹਾਲਤ PV_TS:	ਵਾਪਸ (char_u *)&(curbuf->b_p_ts);
	ਹਾਲਤ PV_TW:	ਵਾਪਸ (char_u *)&(curbuf->b_p_tw);
	ਹਾਲਤ PV_TX:	ਵਾਪਸ (char_u *)&(curbuf->b_p_tx);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
	ਹਾਲਤ PV_UDF:	ਵਾਪਸ (char_u *)&(curbuf->b_p_udf);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ PV_WM:	ਵਾਪਸ (char_u *)&(curbuf->b_p_wm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
	ਹਾਲਤ PV_KMAP:	ਵਾਪਸ (char_u *)&(curbuf->b_p_keymap);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	ਹਾਲਤ PV_SCL:	ਵਾਪਸ (char_u *)&(curwin->w_p_scl);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	ਹਾਲਤ PV_VSTS:	ਵਾਪਸ (char_u *)&(curbuf->b_p_vsts);
	ਹਾਲਤ PV_VTS:	ਵਾਪਸ (char_u *)&(curbuf->b_p_vts);
#ਜੇ_ਅੰਤ
	ਮੂਲ:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    ਵਾਪਸ (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(ਪੂਰਨ opt_idx)
{
    ਵਾਪਸ options[opt_idx].var;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(ਪੂਰਨ opt_idx)
{
    ਵਾਪਸ (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    ਪੂਰਨ
option_info(char_u *name, dict_T *d)
{
    ਪੂਰਨ			opt_idx;
    ਬਣਤਰ vimoption	*p;
    ਪੂਰਨ			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    ਜੇ ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    ਜੇ (opt_idx < 0)
	ਵਾਪਸ FAIL;

    p = &options[opt_idx];
    dict_ਜੋੜੋ_string(d, "name", (char_u *)p->fullname);
    dict_ਜੋੜੋ_string(d, "shortname",
		    (char_u *)(p->shortname == ਨਿਰਾ ? "" : p->shortname));
    dict_ਜੋੜੋ_bool(d, "available", p->var != ਨਿਰਾ);
    dict_ਜੋੜੋ_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_ਅੰਕ) ? "number" : "string"));
    dict_ਜੋੜੋ_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    ਜੇ (p->flags & P_BOOL)
	dict_ਜੋੜੋ_bool(d, "default", (ਪੂਰਨ)(long_i)p->def_val[dvi]);
    ਨਹੀਂਤਾਂ ਜੇ (p->flags & P_ਅੰਕ)
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "default", (ਲੰਮਾ)(long_i)p->def_val[dvi]);
    ਨਹੀਂਤਾਂ
	dict_ਜੋੜੋ_string(d, "default", p->def_val[dvi] == ਨਿਰਾ
					? (char_u *)"" : p->def_val[dvi]);
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_ਸੈੱਟ_cb_T
get_option_did_ਸੈੱਟ_cb(ਪੂਰਨ opt_idx)
{
    ਵਾਪਸ options[opt_idx].opt_did_ਸੈੱਟ_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(ਖਾਲੀ)
{
    ਜੇ (*curbuf->b_p_ep == NUL)
	ਵਾਪਸ p_ep;
    ਵਾਪਸ curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (*curbuf->b_p_ffu == NUL)
	ਵਾਪਸ p_ffu;
    ਵਾਪਸ curbuf->b_p_ffu;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ (char_u *)"";
#ਜੇ_ਅੰਤ
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    ਖਾਲੀ
win_copy_options(win_T *wp_from, win_T *wp_ਮੰਜ਼ਿਲ)
{
    copy_winopt(&wp_from->w_onebuf_opt, &wp_ਮੰਜ਼ਿਲ->w_onebuf_opt);
    copy_winopt(&wp_from->w_allbuf_opt, &wp_ਮੰਜ਼ਿਲ->w_allbuf_opt);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    // w_stl_rendered_height is in win_T directly, not in winvar_T, so it is
    // not copied by copy_winopt().  Copy it explicitly here.
    wp_ਮੰਜ਼ਿਲ->w_stl_rendered_ਉਚਾਈ = wp_from->w_stl_rendered_ਉਚਾਈ;
#ਜੇ_ਅੰਤ
    after_copy_winopt(wp_ਮੰਜ਼ਿਲ);
}

/*
 * After copying window options: update variables depending on options.
 */
    ਖਾਲੀ
after_copy_winopt(win_T *wp)
{
    ਅੱਖਰ *errmsg = update_winhighlight(wp, wp->w_p_whl);

    ਜੇ (errmsg != ਨਿਰਾ)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    ਜੇ (wp->w_p_wrap)
	wp->w_leftcol = 0;
    ਨਹੀਂਤਾਂ
	wp->w_skipcol = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    briopt_check(ਨਿਰਾ, wp);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    fill_culopt_flags(ਨਿਰਾ, wp);
    check_colorcolumn(ਨਿਰਾ, wp);
#ਜੇ_ਅੰਤ
    ਸੈੱਟ_listchars_option(wp, wp->w_p_lcs, TRUE, ਨਿਰਾ, 0);
    ਸੈੱਟ_fillchars_option(wp, wp->w_p_fcs, TRUE, ਨਿਰਾ, 0);
}

    ਸਥਿਰ char_u *
copy_option_val(char_u *val)
{
    ਜੇ (val == ਖਾਲੀ_ਥਾਵਾਂ_option)
	ਵਾਪਸ ਖਾਲੀ_ਥਾਵਾਂ_option;  // no need to allocate memory
    ਵਾਪਸ vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    ਖਾਲੀ
copy_winopt(winopt_T *from, winopt_T *ਮੰਜ਼ਿਲ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
    ਮੰਜ਼ਿਲ->wo_arab = from->wo_arab;
#ਜੇ_ਅੰਤ
    ਮੰਜ਼ਿਲ->wo_list = from->wo_list;
    ਮੰਜ਼ਿਲ->wo_lcs = copy_option_val(from->wo_lcs);
    ਮੰਜ਼ਿਲ->wo_fcs = copy_option_val(from->wo_fcs);
    ਮੰਜ਼ਿਲ->wo_nu = from->wo_nu;
    ਮੰਜ਼ਿਲ->wo_rnu = from->wo_rnu;
    ਮੰਜ਼ਿਲ->wo_ve = copy_option_val(from->wo_ve);
    ਮੰਜ਼ਿਲ->wo_ve_flags = from->wo_ve_flags;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਮੰਜ਼ਿਲ->wo_nuw = from->wo_nuw;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਮੰਜ਼ਿਲ->wo_rl  = from->wo_rl;
    ਮੰਜ਼ਿਲ->wo_rlc = copy_option_val(from->wo_rlc);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਮੰਜ਼ਿਲ->wo_sbr = copy_option_val(from->wo_sbr);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    ਮੰਜ਼ਿਲ->wo_stl = copy_option_val(from->wo_stl);
    ਮੰਜ਼ਿਲ->wo_stlo = copy_option_val(from->wo_stlo);
    ਮੰਜ਼ਿਲ->wo_stlo_fh = from->wo_stlo_fh;
    ਮੰਜ਼ਿਲ->wo_stlo_mh = from->wo_stlo_mh;
#ਜੇ_ਅੰਤ
    ਮੰਜ਼ਿਲ->wo_wrap = from->wo_wrap;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਮੰਜ਼ਿਲ->wo_wrap_save = from->wo_wrap_save;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਮੰਜ਼ਿਲ->wo_lbr = from->wo_lbr;
    ਮੰਜ਼ਿਲ->wo_bri = from->wo_bri;
    ਮੰਜ਼ਿਲ->wo_briopt = copy_option_val(from->wo_briopt);
#ਜੇ_ਅੰਤ
    ਮੰਜ਼ਿਲ->wo_wcr = copy_option_val(from->wo_wcr);
    ਮੰਜ਼ਿਲ->wo_scb = from->wo_scb;
    ਮੰਜ਼ਿਲ->wo_scb_save = from->wo_scb_save;
    ਮੰਜ਼ਿਲ->wo_sms = from->wo_sms;
    ਮੰਜ਼ਿਲ->wo_crb = from->wo_crb;
    ਮੰਜ਼ਿਲ->wo_crb_save = from->wo_crb_save;
    ਮੰਜ਼ਿਲ->wo_siso = from->wo_siso;
    ਮੰਜ਼ਿਲ->wo_so = from->wo_so;
    ਮੰਜ਼ਿਲ->wo_sop = from->wo_sop;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    ਮੰਜ਼ਿਲ->wo_spell = from->wo_spell;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਮੰਜ਼ਿਲ->wo_cuc = from->wo_cuc;
    ਮੰਜ਼ਿਲ->wo_cul = from->wo_cul;
    ਮੰਜ਼ਿਲ->wo_culopt = copy_option_val(from->wo_culopt);
    ਮੰਜ਼ਿਲ->wo_cc = copy_option_val(from->wo_cc);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਮੰਜ਼ਿਲ->wo_diff = from->wo_diff;
    ਮੰਜ਼ਿਲ->wo_diff_saved = from->wo_diff_saved;
#ਜੇ_ਅੰਤ
    ਮੰਜ਼ਿਲ->wo_eiw = copy_option_val(from->wo_eiw);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਮੰਜ਼ਿਲ->wo_cocu = copy_option_val(from->wo_cocu);
    ਮੰਜ਼ਿਲ->wo_cole = from->wo_cole;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਮੰਜ਼ਿਲ->wo_twk = copy_option_val(from->wo_twk);
    ਮੰਜ਼ਿਲ->wo_tws = copy_option_val(from->wo_tws);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਮੰਜ਼ਿਲ->wo_fdc = from->wo_fdc;
    ਮੰਜ਼ਿਲ->wo_fdc_save = from->wo_fdc_save;
    ਮੰਜ਼ਿਲ->wo_fen = from->wo_fen;
    ਮੰਜ਼ਿਲ->wo_fen_save = from->wo_fen_save;
    ਮੰਜ਼ਿਲ->wo_fdi = copy_option_val(from->wo_fdi);
    ਮੰਜ਼ਿਲ->wo_fml = from->wo_fml;
    ਮੰਜ਼ਿਲ->wo_fdl = from->wo_fdl;
    ਮੰਜ਼ਿਲ->wo_fdl_save = from->wo_fdl_save;
    ਮੰਜ਼ਿਲ->wo_fdm = copy_option_val(from->wo_fdm);
    ਮੰਜ਼ਿਲ->wo_fdm_save = from->wo_diff_saved
			       ? vim_strsave(from->wo_fdm_save) : ਖਾਲੀ_ਥਾਵਾਂ_option;
    ਮੰਜ਼ਿਲ->wo_fdn = from->wo_fdn;
# ifdef FEAT_EVAL
    ਮੰਜ਼ਿਲ->wo_fde = copy_option_val(from->wo_fde);
    ਮੰਜ਼ਿਲ->wo_fdt = copy_option_val(from->wo_fdt);
# endif
    ਮੰਜ਼ਿਲ->wo_fmr = copy_option_val(from->wo_fmr);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    ਮੰਜ਼ਿਲ->wo_scl = copy_option_val(from->wo_scl);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    ਮੰਜ਼ਿਲ->wo_lhi = from->wo_lhi;
#ਜੇ_ਅੰਤ

    ਮੰਜ਼ਿਲ->wo_wrap_flags = from->wo_wrap_flags;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    ਮੰਜ਼ਿਲ->wo_stl_flags = from->wo_stl_flags;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਮੰਜ਼ਿਲ->wo_fde_flags = from->wo_fde_flags;
    ਮੰਜ਼ਿਲ->wo_fdt_flags = from->wo_fdt_flags;
#ਜੇ_ਅੰਤ
    ਮੰਜ਼ਿਲ->wo_whl = copy_option_val(from->wo_whl);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Copy the script context so that we know where the value was last set.
    mch_memmove(ਮੰਜ਼ਿਲ->wo_script_ctx, from->wo_script_ctx,
						    ਆਕਾਰ(ਮੰਜ਼ਿਲ->wo_script_ctx));
#ਜੇ_ਅੰਤ
    check_winopt(ਮੰਜ਼ਿਲ);		// don't want NULL pointers
}

/*
 * Check string options in a window for a NULL value.
 */
    ਸਥਿਰ ਖਾਲੀ
check_win_options(win_T *win)
{
    check_winopt(&win->w_onebuf_opt);
    check_winopt(&win->w_allbuf_opt);
}

/*
 * Check for NULL pointers in a winopt_T and replace them with empty_option.
 */
    ਸਥਿਰ ਖਾਲੀ
check_winopt(winopt_T *wop ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    check_string_option(&wop->wo_fdi);
    check_string_option(&wop->wo_fdm);
    check_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    check_string_option(&wop->wo_fde);
    check_string_option(&wop->wo_fdt);
# endif
    check_string_option(&wop->wo_fmr);
#ਜੇ_ਅੰਤ
    check_string_option(&wop->wo_eiw);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    check_string_option(&wop->wo_scl);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    check_string_option(&wop->wo_rlc);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    check_string_option(&wop->wo_sbr);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    check_string_option(&wop->wo_stl);
    check_string_option(&wop->wo_stlo);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    check_string_option(&wop->wo_culopt);
    check_string_option(&wop->wo_cc);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    check_string_option(&wop->wo_cocu);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    check_string_option(&wop->wo_twk);
    check_string_option(&wop->wo_tws);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    check_string_option(&wop->wo_briopt);
#ਜੇ_ਅੰਤ
    check_string_option(&wop->wo_wcr);
    check_string_option(&wop->wo_lcs);
    check_string_option(&wop->wo_fcs);
    check_string_option(&wop->wo_ve);
    check_string_option(&wop->wo_whl);
}

/*
 * Free the allocated memory inside a winopt_T.
 */
    ਖਾਲੀ
clear_winopt(winopt_T *wop ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    clear_string_option(&wop->wo_fdi);
    clear_string_option(&wop->wo_fdm);
    clear_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    clear_string_option(&wop->wo_fde);
    clear_string_option(&wop->wo_fdt);
# endif
    clear_string_option(&wop->wo_fmr);
#ਜੇ_ਅੰਤ
    clear_string_option(&wop->wo_eiw);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    clear_string_option(&wop->wo_scl);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    clear_string_option(&wop->wo_briopt);
#ਜੇ_ਅੰਤ
    clear_string_option(&wop->wo_wcr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    clear_string_option(&wop->wo_rlc);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    clear_string_option(&wop->wo_sbr);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    clear_string_option(&wop->wo_stl);
    clear_string_option(&wop->wo_stlo);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    clear_string_option(&wop->wo_culopt);
    clear_string_option(&wop->wo_cc);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    clear_string_option(&wop->wo_cocu);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    clear_string_option(&wop->wo_twk);
    clear_string_option(&wop->wo_tws);
#ਜੇ_ਅੰਤ
    clear_string_option(&wop->wo_lcs);
    clear_string_option(&wop->wo_fcs);
    clear_string_option(&wop->wo_ve);
    clear_string_option(&wop->wo_whl);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
// Index into the options table for a buffer-local option enum.
ਸਥਿਰ ਪੂਰਨ buf_opt_idx[BV_ਗਿਣਤੀ];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    ਸਥਿਰ ਖਾਲੀ
init_buf_opt_idx(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ did_init_buf_opt_idx = FALSE;
    ਪੂਰਨ i;

    ਜੇ (did_init_buf_opt_idx)
	ਵਾਪਸ;
    did_init_buf_opt_idx = TRUE;
    ਲਈ (i = 0; !istermoption_idx(i); i++)
	ਜੇ (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_ਨਕਾਬ] = i;
}
#ਨਹੀਂਤਾਂ
# define COPY_OPT_SCTX(buf, bv)
# define COPY_OPT_INSECURE(flagsfield, bv)
#ਜੇ_ਅੰਤ

/*
 * Copy global option values to local options for one buffer.
 * Used when creating a new buffer and sometimes when entering a buffer.
 * flags:
 * BCO_ENTER	We will enter the buffer "buf".
 * BCO_ALWAYS	Always copy the options, but only set b_p_initialized when
 *		appropriate.
 * BCO_NOHELP	Don't copy the values to a help buffer.
 */
    ਖਾਲੀ
buf_copy_options(buf_T *buf, ਪੂਰਨ flags)
{
    ਪੂਰਨ		should_copy = TRUE;
    char_u	*save_p_isk = ਨਿਰਾ;	    // init for GCC
    ਪੂਰਨ		dont_do_help;
    ਪੂਰਨ		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    ਜੇ (p_cpo != ਨਿਰਾ)
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
	ਜੇ ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == ਨਿਰਾ || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != ਨਿਰਾ)))
	    should_copy = FALSE;

	ਜੇ (should_copy || (flags & BCO_ALWAYS))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    CLEAR_ਖੇਤਰ(buf->b_p_script_ctx);
	    init_buf_opt_idx();
#ਜੇ_ਅੰਤ
	    // Don't copy the options specific to a help buffer when
	    // BCO_NOHELP is given or the options were initialized already
	    // (jumping back to a help file with CTRL-T or CTRL-O)
	    dont_do_help = ((flags & BCO_NOHELP) && buf->b_help)
						       || buf->b_p_initialized;
	    ਜੇ (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = ਨਿਰਾ;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    ਜੇ (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		ਚੋਣ (*p_ffs)
		{
		    ਹਾਲਤ 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); ਰੋਕੋ;
		    ਹਾਲਤ 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); ਰੋਕੋ;
		    ਹਾਲਤ 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); ਰੋਕੋ;
		    ਮੂਲ:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		ਜੇ (buf->b_p_ff != ਨਿਰਾ)
		    buf->b_ਸ਼ੁਰੂ_ffc = *buf->b_p_ff;
		buf->b_p_bh = ਖਾਲੀ_ਥਾਵਾਂ_option;
		buf->b_p_bt = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    }
	    ਨਹੀਂਤਾਂ
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
	    ਜੇ (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    ਨਹੀਂਤਾਂ
	    {
		buf->b_p_swf = p_swf;
		COPY_OPT_SCTX(buf, BV_SWF);
	    }
	    buf->b_p_cpt = vim_strsave(p_cpt);
	    COPY_OPT_SCTX(buf, BV_CPT);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
	    COPY_OPT_INSECURE(buf->b_p_cpt_flags, BV_CPT);
	    ਸੈੱਟ_buflocal_cpt_callbacks(buf);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	    buf->b_p_csl = vim_strsave(p_csl);
	    COPY_OPT_SCTX(buf, BV_CSL);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
	    buf->b_p_cfu = vim_strsave(p_cfu);
	    COPY_OPT_SCTX(buf, BV_CFU);
	    ਸੈੱਟ_buflocal_cfu_callback(buf);
	    buf->b_p_ofu = vim_strsave(p_ofu);
	    COPY_OPT_SCTX(buf, BV_OFU);
	    ਸੈੱਟ_buflocal_ofu_callback(buf);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    buf->b_p_tfu = vim_strsave(p_tfu);
	    COPY_OPT_SCTX(buf, BV_TFU);
	    ਸੈੱਟ_buflocal_tfu_callback(buf);
#ਜੇ_ਅੰਤ
	    buf->b_p_sts = p_sts;
	    COPY_OPT_SCTX(buf, BV_STS);
	    buf->b_p_sts_nopaste = p_sts_nopaste;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	    buf->b_p_vsts = vim_strsave(p_vsts);
	    COPY_OPT_SCTX(buf, BV_VSTS);
	    ਜੇ (p_vsts && p_vsts != ਖਾਲੀ_ਥਾਵਾਂ_option)
		(ਖਾਲੀ)tabstop_ਸੈੱਟ(p_vsts, &buf->b_p_vsts_array);
	    ਨਹੀਂਤਾਂ
		buf->b_p_vsts_array = ਨਿਰਾ;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : ਨਿਰਾ;
#ਜੇ_ਅੰਤ
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
	    buf->b_p_ft = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_p_pi = p_pi;
	    COPY_OPT_SCTX(buf, BV_PI);
	    buf->b_p_cinw = vim_strsave(p_cinw);
	    COPY_OPT_SCTX(buf, BV_CINW);
	    buf->b_p_lisp = p_lisp;
	    COPY_OPT_SCTX(buf, BV_LISP);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    // Don't copy 'syntax', it must be set
	    buf->b_p_syn = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_p_smc = p_smc;
	    COPY_OPT_SCTX(buf, BV_SMC);
	    buf->b_s.b_syn_isk = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	    buf->b_s.b_p_spc = vim_strsave(p_spc);
	    COPY_OPT_SCTX(buf, BV_SPC);
	    (ਖਾਲੀ)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#ਜੇ_ਅੰਤ
	    buf->b_p_fp = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	    buf->b_p_ਕੁੰਜੀ = vim_strsave(p_ਕੁੰਜੀ);
	    COPY_OPT_SCTX(buf, BV_ਕੁੰਜੀ);
#ਜੇ_ਅੰਤ
	    buf->b_p_sua = vim_strsave(p_sua);
	    COPY_OPT_SCTX(buf, BV_SUA);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
	    buf->b_p_keymap = vim_strsave(p_keymap);
	    COPY_OPT_SCTX(buf, BV_KMAP);
	    buf->b_kmap_state |= KEYMAP_INIT;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    buf->b_p_twsl = p_twsl;
	    COPY_OPT_SCTX(buf, BV_TWSL);
#ਜੇ_ਅੰਤ
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FSYNC
	    buf->b_p_fs = -1;
#ਜੇ_ਅੰਤ
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    buf->b_p_bkc = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_bkc_flags = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	    buf->b_p_gefm = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_p_gp = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_p_mp = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_p_efm = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ
	    buf->b_p_ep = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ defined(FEAT_EVAL)
	    buf->b_p_ffu = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ
	    buf->b_p_kp = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_p_ਰਸਤਾ = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_p_ਟੈਗ_ਸੂਚੀ = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_p_tc = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_tc_flags = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
	    buf->b_p_def = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_p_inc = ਖਾਲੀ_ਥਾਵਾਂ_option;
# ifdef FEAT_EVAL
	    buf->b_p_inex = vim_strsave(p_inex);
	    COPY_OPT_SCTX(buf, BV_INEX);
	    COPY_OPT_INSECURE(buf->b_p_inex_flags, BV_INEX);
# endif
#ਜੇ_ਅੰਤ
	    buf->b_p_cot = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_cot_flags = 0;
	    buf->b_p_dict = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    buf->b_p_dia = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ
	    buf->b_p_tsr = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
	    buf->b_p_tsrfu = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ
	    buf->b_p_qe = vim_strsave(p_qe);
	    COPY_OPT_SCTX(buf, BV_QE);
#ਜੇ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_CRYPT)
	    buf->b_p_cm = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
	    buf->b_p_udf = p_udf;
	    COPY_OPT_SCTX(buf, BV_UDF);
#ਜੇ_ਅੰਤ
	    buf->b_p_lw = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    buf->b_p_menc = ਖਾਲੀ_ਥਾਵਾਂ_option;

	    // Don't copy the options set by ex_help(), use the saved values,
	    // when going from a help buffer to a non-help buffer.
	    // Don't touch these at all when BCO_NOHELP is used and going from
	    // or to a help buffer.
	    ਜੇ (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		ਜੇ (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (ਖਾਲੀ)tabstop_ਸੈੱਟ(p_vts, &buf->b_p_vts_array);
		ਨਹੀਂਤਾਂ
		    buf->b_p_vts_array = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		ਜੇ (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (ਖਾਲੀ)tabstop_ਸੈੱਟ(p_vts, &buf->b_p_vts_array);
		ਨਹੀਂਤਾਂ
		    buf->b_p_vts_array = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
		buf->b_help = false;
		ਜੇ (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	ਜੇ (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    ਜੇ (did_isk)
	(ਖਾਲੀ)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    ਖਾਲੀ
reset_modifiable(ਖਾਲੀ)
{
    ਪੂਰਨ		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    ਜੇ (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}

/*
 * Set the global value for 'iminsert' to the local value.
 */
    ਖਾਲੀ
ਸੈੱਟ_iminsert_global(ਖਾਲੀ)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    ਖਾਲੀ
ਸੈੱਟ_imsearch_global(ਖਾਲੀ)
{
    p_imsearch = curbuf->b_p_imsearch;
}

ਸਥਿਰ ਪੂਰਨ expand_option_idx = -1;
ਸਥਿਰ ਪੂਰਨ expand_option_ਸ਼ੁਰੂ_ਖਾਨਾ = 0;
ਸਥਿਰ char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
ਸਥਿਰ ਪੂਰਨ expand_option_flags = 0;
ਸਥਿਰ ਪੂਰਨ expand_option_append = FALSE;

    ਖਾਲੀ
ਸੈੱਟ_context_ਵਿੱਚ_ਸੈੱਟ_cmd(
    expand_T	*xp,
    char_u	*arg,
    ਪੂਰਨ		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    ਪੂਰਨ		nextchar;
    long_u	flags = 0;	// init for GCC
    ਪੂਰਨ		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    ਪੂਰਨ		is_ਪਦ_option = FALSE;
    ਪੂਰਨ		ਕੁੰਜੀ;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    ਜੇ (*arg == NUL)
    {
	xp->xp_ਨਮੂਨਾ = arg;
	ਵਾਪਸ;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    ਜੇ (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_ਨਮੂਨਾ = p + 1;
	ਵਾਪਸ;
    }
    ਜਦੋਂ (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	ਜੇ (*p == ' ' || *p == ',')
	{
	    ਜਦੋਂ (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	ਜੇ (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    ਰੋਕੋ;
	}
	--p;
    }
    ਜੇ (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_ਅਗੇਤਰ = XP_ਅਗੇਤਰ_NO;
	p += 2;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_ਅਗੇਤਰ = XP_ਅਗੇਤਰ_INV;
	p += 3;
    }
    xp->xp_ਨਮੂਨਾ = arg = p;
    ਜੇ (*arg == '<')
    {
	ਜਦੋਂ (*p != '>')
	    ਜੇ (*p++ == NUL)	    // expand terminal option name
		ਵਾਪਸ;
	ਕੁੰਜੀ = get_special_ਕੁੰਜੀ_ਕੋਡ(arg + 1);
	ਜੇ (ਕੁੰਜੀ == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    ਵਾਪਸ;
	}
	nextchar = *++p;
	is_ਪਦ_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(ਕੁੰਜੀ);
	expand_option_name[3] = KEY2TERMCAP1(ਕੁੰਜੀ);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    ਜੇ (*p != NUL)
		++p;
	    ਜੇ (*p == NUL)
		ਵਾਪਸ;		// expand option name
	    nextchar = *++p;
	    is_ਪਦ_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	ਨਹੀਂਤਾਂ
	{
	    // Allow * wildcard
	    ਜਦੋਂ (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    ਜੇ (*p == NUL)
		ਵਾਪਸ;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    ਜੇ (opt_idx == -1 || options[opt_idx].var == ਨਿਰਾ)
	    {
		xp->xp_context = EXPAND_NOTHING;
		ਵਾਪਸ;
	    }
	    flags = options[opt_idx].flags;
	    ਜੇ (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		ਵਾਪਸ;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    ਪੂਰਨ expand_option_ਘਟਾਓ = FALSE;
    ਜੇ ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	ਜੇ (nextchar == '-')
	    expand_option_ਘਟਾਓ = TRUE;
	ਜੇ (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    ਜੇ ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ਵਾਪਸ;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    ਜੇ (is_ਪਦ_option)
	expand_option_idx = -1;
    ਨਹੀਂਤਾਂ
	expand_option_idx = opt_idx;

    ਜੇ (!is_ਪਦ_option)
    {
	ਜੇ (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    ਵਾਪਸ;
	}
    }

    xp->xp_ਨਮੂਨਾ = p + 1;
    expand_option_ਸ਼ੁਰੂ_ਖਾਨਾ = (ਪੂਰਨ)(p + 1 - xp->xp_ਸਤਰ);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਜੇ (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	ਵਾਪਸ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
    ਜੇ (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    ਜੇ (expand_option_ਘਟਾਓ)
    {
	xp->xp_context = EXPAND_SETTING_ਘਟਾਓ;
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != ਨਿਰਾ)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*xp->xp_ਨਮੂਨਾ == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ
	xp->xp_context = EXPAND_NOTHING;

    ਜੇ (is_ਪਦ_option || (flags & P_ਅੰਕ))
	ਵਾਪਸ;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    ਜੇ (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	ਜੇ (p == (char_u *)&p_bdir
		|| p == (char_u *)&p_dir
		|| p == (char_u *)&p_ਰਸਤਾ
		|| p == (char_u *)&p_pp
		|| p == (char_u *)&p_rtp
		|| p == (char_u *)&p_cdpath
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
		|| p == (char_u *)&p_vdir
#ਜੇ_ਅੰਤ
		)
	{
	    xp->xp_context = EXPAND_DIRECTORIES;
	    ਜੇ (p == (char_u *)&p_ਰਸਤਾ || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    ਨਹੀਂਤਾਂ
		xp->xp_backslash = XP_BS_ONE;
	}
	ਨਹੀਂਤਾਂ
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    ਜੇ (p == (char_u *)&p_ਟੈਗ_ਸੂਚੀ)
		xp->xp_backslash = XP_BS_THREE;
	    ਨਹੀਂਤਾਂ
		xp->xp_backslash = XP_BS_ONE;
	}
	ਜੇ (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    ਜੇ ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	ਲਈ (p = argend - 1; p >= xp->xp_ਨਮੂਨਾ; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    ਜੇ (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		ਜਦੋਂ (s > xp->xp_ਨਮੂਨਾ && *(s - 1) == '\\')
		    --s;
		ਜੇ ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#ਜੇ defined(BACKSLASH_ਵਿੱਚ_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#ਨਹੀਂਤਾਂ
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#ਜੇ_ਅੰਤ
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_ਨਮੂਨਾ = p + 1;
		    ਰੋਕੋ;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    ਜੇ (flags & P_FLAGLIST)
	xp->xp_ਨਮੂਨਾ = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    ਜੇ (options[opt_idx].var == (char_u *)&p_sps)
    {
	ਜੇ (STRNCMP(xp->xp_ਨਮੂਨਾ, "file:", 5) == 0)
	{
	    xp->xp_ਨਮੂਨਾ += 5;
	    ਵਾਪਸ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (options[expand_option_idx].opt_expand_cb != ਨਿਰਾ)
	{
	    xp->xp_context = EXPAND_STRING_SETTING;
	}
    }
#ਜੇ_ਅੰਤ
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
    ਸਥਿਰ ਪੂਰਨ
ਮੇਲ_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	ਪੂਰਨ		idx,
	ਪੂਰਨ		test_only,
	ਪੂਰਨ		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    ਜੇ (!fuzzy)
    {
	ਜੇ (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    ਜੇ (!test_only)
		matches[idx] = vim_strsave(str);
	    ਵਾਪਸ TRUE;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ score;

	score = fuzzy_ਮੇਲ_str(str, fuzzystr);
	ਜੇ (score != FUZZY_SCORE_NONE)
	{
	    ਜੇ (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    ਵਾਪਸ TRUE;
	}
    }

    ਵਾਪਸ FALSE;
}

    ਪੂਰਨ
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    ਪੂਰਨ		*ਅੰਕ_Matches,
    char_u	***matches,
    ਪੂਰਨ		can_fuzzy)
{
    ਪੂਰਨ		ਅੰਕ_ਸਾਧਾਰਨ = 0;	    // Nr of matching non-term-code settings
    ਪੂਰਨ		ਅੰਕ_ਪਦ = 0;	    // Nr of matching terminal code settings
    ਪੂਰਨ		opt_idx;
    ਪੂਰਨ		ਮੇਲ;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    char_u	*str;
    ਪੂਰਨ		loop;
    ਪੂਰਨ		is_ਪਦ_opt;
    char_u	name_buf[MAX_ਕੁੰਜੀ_NAME_LEN];
    ਸਥਿਰ ਅੱਖਰ *(ਨਾਂ[]) = {"all", "termcap"};
    ਪੂਰਨ		ic = regmatch->rm_ic;	// remember the ignore-case flag
    ਪੂਰਨ		fuzzy;
    fuzmatch_str_T  *fuzmatch = ਨਿਰਾ;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    ਲਈ (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	ਜੇ (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    ਲਈ (ਮੇਲ = 0; ਮੇਲ < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(ਨਾਂ); ++ਮੇਲ)
	    {
		ਜੇ (ਮੇਲ_str((char_u *)ਨਾਂ[ਮੇਲ], regmatch, *matches,
			    ਗਿਣਤੀ_2, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    ਜੇ (loop == 0)
			ਅੰਕ_ਸਾਧਾਰਨ++;
		    ਨਹੀਂਤਾਂ
			ਗਿਣਤੀ_2++;
		}
	    }
	}
	ਲਈ (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != ਨਿਰਾ;
								    opt_idx++)
	{
	    ਜੇ (options[opt_idx].var == ਨਿਰਾ)
		ਜਾਰੀ;
	    ਜੇ (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		ਜਾਰੀ;
	    is_ਪਦ_opt = istermoption_idx(opt_idx);
	    ਜੇ (is_ਪਦ_opt && ਅੰਕ_ਸਾਧਾਰਨ > 0)
		ਜਾਰੀ;

	    ਜੇ (ਮੇਲ_str(str, regmatch, *matches, ਗਿਣਤੀ_2, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		ਜੇ (loop == 0)
		{
		    ਜੇ (is_ਪਦ_opt)
			ਅੰਕ_ਪਦ++;
		    ਨਹੀਂਤਾਂ
			ਅੰਕ_ਸਾਧਾਰਨ++;
		}
		ਨਹੀਂਤਾਂ
		    ਗਿਣਤੀ_2++;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!fuzzy && options[opt_idx].shortname != ਨਿਰਾ
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		ਜੇ (loop == 0)
		{
		    ਜੇ (is_ਪਦ_opt)
			ਅੰਕ_ਪਦ++;
		    ਨਹੀਂਤਾਂ
			ਅੰਕ_ਸਾਧਾਰਨ++;
		}
		ਨਹੀਂਤਾਂ
		    (*matches)[ਗਿਣਤੀ_2++] = vim_strsave(str);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_ਪਦ_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		ਜੇ (ਮੇਲ_str(name_buf, regmatch, *matches, ਗਿਣਤੀ_2, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    ਜੇ (loop == 0)
			ਅੰਕ_ਪਦ++;
		    ਨਹੀਂਤਾਂ
			ਗਿਣਤੀ_2++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	ਜੇ (xp->xp_context != EXPAND_BOOL_SETTINGS  && ਅੰਕ_ਸਾਧਾਰਨ == 0)
	{
	    ਲਈ (opt_idx = 0; (str = get_termcode(opt_idx)) != ਨਿਰਾ; opt_idx++)
	    {
		ਜੇ (!ਸੁਰੱਖਿਅਤ_isprint(str[0]) || !ਸੁਰੱਖਿਅਤ_isprint(str[1]))
		    ਜਾਰੀ;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		ਜੇ (ਮੇਲ_str(name_buf, regmatch, *matches, ਗਿਣਤੀ_2,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    ਜੇ (loop == 0)
			ਅੰਕ_ਪਦ++;
		    ਨਹੀਂਤਾਂ
			ਗਿਣਤੀ_2++;
		}
		ਨਹੀਂਤਾਂ
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    ਜੇ (ਮੇਲ_str(name_buf, regmatch, *matches, ਗਿਣਤੀ_2,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			ਜੇ (loop == 0)
			    ਅੰਕ_ਪਦ++;
			ਨਹੀਂਤਾਂ
			    ਗਿਣਤੀ_2++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    ਲਈ (opt_idx = 0; (str = get_ਕੁੰਜੀ_name(opt_idx)) != ਨਿਰਾ; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		ਜੇ (ਮੇਲ_str(name_buf, regmatch, *matches, ਗਿਣਤੀ_2, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    ਜੇ (loop == 0)
			ਅੰਕ_ਪਦ++;
		    ਨਹੀਂਤਾਂ
			ਗਿਣਤੀ_2++;
		}
	    }
	}
	ਜੇ (loop == 0)
	{
	    ਜੇ (ਅੰਕ_ਸਾਧਾਰਨ > 0)
		*ਅੰਕ_Matches = ਅੰਕ_ਸਾਧਾਰਨ;
	    ਨਹੀਂਤਾਂ ਜੇ (ਅੰਕ_ਪਦ > 0)
		*ਅੰਕ_Matches = ਅੰਕ_ਪਦ;
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ OK;
	    ਜੇ (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *ਅੰਕ_Matches);
		ਜੇ (*matches == ਨਿਰਾ)
		{
		    *matches = (char_u **)"";
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *ਅੰਕ_Matches);
		ਜੇ (fuzmatch == ਨਿਰਾ)
		{
		    *matches = (char_u **)"";
		    ਵਾਪਸ FAIL;
		}
	    }
	}
    }

    ਜੇ (fuzzy &&
	fuzzymatches_ਮੰਜ਼ਿਲ_strmatches(fuzmatch, matches, ਗਿਣਤੀ_2, FALSE) == FAIL)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

// Escape an option value that can be used on the command-line with :set.
// Caller needs to free the returned string, unless NULL is returned.
    ਸਥਿਰ char_u*
escape_option_str_cmdline(char_u *var)
{
    char_u *buf;

    // A backslash is required before some characters.  This is the reverse of
    // what happens in do_set().
    buf = vim_strsave_escaped(var, escape_chars);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    ਲਈ (var = buf; *var != NUL; MB_PTR_ADV(var))
	ਜੇ (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    ਪੂਰਨ
ExpandOldSetting(ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    char_u  *var = ਨਿਰਾ;	// init for GCC
    char_u  *buf;

    *ਅੰਕ_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    ਜੇ (*matches == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // For a terminal key code expand_option_idx is < 0.
    ਜੇ (expand_option_idx < 0)
    {
	var = ਲੱਭੋ_termcode(expand_option_name + 2);
	ਜੇ (var == ਨਿਰਾ)
	    expand_option_idx = findoption(expand_option_name);
    }

    ਜੇ (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    ਨਹੀਂਤਾਂ ਜੇ (var == ਨਿਰਾ)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    ਜੇ (buf == ਨਿਰਾ)
    {
	VIM_CLEAR(*matches);
	ਵਾਪਸ FAIL;
    }

    (*matches)[0] = buf;
    *ਅੰਕ_Matches = 1;
    ਵਾਪਸ OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    ਪੂਰਨ
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    ਪੂਰਨ		*ਅੰਕ_Matches,
    char_u	***matches)
{
    char_u  *var = ਨਿਰਾ;	// init for GCC
    char_u  *buf;

    ਜੇ (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == ਨਿਰਾ)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	ਵਾਪਸ FAIL;
    }

    optexpand_T ਦਲੀਲਾਂ;
    ਦਲੀਲਾਂ.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    ਦਲੀਲਾਂ.oe_append = expand_option_append;
    ਦਲੀਲਾਂ.oe_regmatch = regmatch;
    ਦਲੀਲਾਂ.oe_xp = xp;
    ਦਲੀਲਾਂ.oe_ਸੈੱਟ_arg = xp->xp_ਸਤਰ + expand_option_ਸ਼ੁਰੂ_ਖਾਨਾ;
    ਦਲੀਲਾਂ.oe_include_orig_val =
	!expand_option_append &&
	(*ਦਲੀਲਾਂ.oe_ਸੈੱਟ_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਦਲੀਲਾਂ.oe_opt_ਮੁੱਲ = buf;

    ਪੂਰਨ ਅੰਕ_ret = options[expand_option_idx].opt_expand_cb(&ਦਲੀਲਾਂ, ਅੰਕ_Matches, matches);

    vim_free(buf);
    ਵਾਪਸ ਅੰਕ_ret;
}

/*
 * Expansion handler for :set-=
 */
    ਪੂਰਨ
Expand_Setting_ਘਟਾਓ(
    expand_T	*xp,
    regmatch_T	*regmatch,
    ਪੂਰਨ		*ਅੰਕ_Matches,
    char_u	***matches)
{
    ਜੇ (expand_option_idx < 0)
	// term option
	ਵਾਪਸ ExpandOldSetting(ਅੰਕ_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    ਜੇ (option_flags & P_ਅੰਕ)
	ਵਾਪਸ ExpandOldSetting(ਅੰਕ_Matches, matches);
    ਨਹੀਂਤਾਂ ਜੇ (option_flags & P_COMMA)
    {
	// Split the option by comma, then present each option to the user if
	// it matches the pattern.
	// This condition needs to go first, because 'whichwrap' has both
	// P_COMMA and P_FLAGLIST.
	garray_T    ga;

	char_u	    *ਵਸਤੂ;
	char_u	    *option_copy;
	char_u	    *ਅਗਲਾ_val;
	char_u	    *comma;

	ਜੇ (*option_val == NUL)
	    ਵਾਪਸ FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	ਜੇ (option_copy == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	ਅਗਲਾ_val = option_copy;

	ga_init2(&ga, ਆਕਾਰ(char_u *), 10);

	ਕਰੋ
	{
	    ਵਸਤੂ = ਅਗਲਾ_val;
	    comma = vim_strchr(ਅਗਲਾ_val, ',');
	    ਜਦੋਂ (comma != ਨਿਰਾ && comma != ਅਗਲਾ_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    ਜੇ (comma != ਨਿਰਾ)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		ਅਗਲਾ_val = comma + 1;
	    }
	    ਨਹੀਂਤਾਂ
		ਅਗਲਾ_val = ਨਿਰਾ;

	    ਜੇ (*ਵਸਤੂ == NUL)
		// empty value, don't add to list
		ਜਾਰੀ;

	    ਜੇ (!vim_regexec(regmatch, ਵਸਤੂ, (colnr_T)0))
		ਜਾਰੀ;

	    char_u *buf = escape_option_str_cmdline(ਵਸਤੂ);
	    ਜੇ (buf == ਨਿਰਾ)
	    {
		vim_free(option_copy);
		ga_clear_ਲੜੀਆਂ(&ga);
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (ga_ਜੋੜੋ_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		ਰੋਕੋ;
	    }
	} ਜਦੋਂ (ਅਗਲਾ_val != ਨਿਰਾ);

	vim_free(option_copy);

	*matches = ga.ga_ਅੰਕੜੇ;
	*ਅੰਕ_Matches = ga.ga_len;
	ਵਾਪਸ OK;
    }
    ਨਹੀਂਤਾਂ ਜੇ (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	ਜੇ (*xp->xp_ਨਮੂਨਾ != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    ਵਾਪਸ FAIL;
	}

	size_t ਅੰਕ_flags = STRLEN(option_val);
	ਜੇ (ਅੰਕ_flags == 0)
	    ਵਾਪਸ FAIL;

	*matches = ALLOC_MULT(char_u *, ਅੰਕ_flags + 1);
	ਜੇ (*matches == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	ਪੂਰਨ ਗਿਣਤੀ_2 = 0;
	char_u *p;

	p = vim_strnsave(option_val, ਅੰਕ_flags);
	ਜੇ (p == ਨਿਰਾ)
	{
	    VIM_CLEAR(*matches);
	    ਵਾਪਸ FAIL;
	}
	(*matches)[ਗਿਣਤੀ_2++] = p;

	ਜੇ (ਅੰਕ_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    ਲਈ (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		ਜੇ (p == ਨਿਰਾ)
		    ਰੋਕੋ;
		(*matches)[ਗਿਣਤੀ_2++] = p;
	    }
	}

	*ਅੰਕ_Matches = ਗਿਣਤੀ_2;
	ਵਾਪਸ OK;
    }

    ਵਾਪਸ ExpandOldSetting(ਅੰਕ_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    ਸਥਿਰ ਖਾਲੀ
option_value2string(
    ਬਣਤਰ vimoption	*opp,
    ਪੂਰਨ			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    ਜੇ (opp->flags & P_ਅੰਕ)
    {
	ਲੰਮਾ wc = 0;

	ਜੇ (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_ਕੁੰਜੀ_name((ਪੂਰਨ)wc, 0));
	ਨਹੀਂਤਾਂ ਜੇ (wc != 0)
	    STRCPY(NameBuff, transchar((ਪੂਰਨ)wc));
	ਨਹੀਂਤਾਂ
	    sprintf((ਅੱਖਰ *)NameBuff, "%ld", *(ਲੰਮਾ *)varp);
    }
    ਨਹੀਂਤਾਂ    // P_STRING
    {
	varp = *(char_u **)(varp);
	ਜੇ (varp == ਨਿਰਾ)		    // just in case
	    NameBuff[0] = NUL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	ਨਹੀਂਤਾਂ ਜੇ (opp->var == (char_u *)&p_ਕੁੰਜੀ && *varp)
	    STRCPY(NameBuff, "*****");
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ ਜੇ (opp->flags & P_EXPAND)
	    home_replace(ਨਿਰਾ, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	ਨਹੀਂਤਾਂ ਜੇ ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	ਨਹੀਂਤਾਂ
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    ਸਥਿਰ ਪੂਰਨ
wc_use_keyname(char_u *varp, ਲੰਮਾ *wcp)
{
    ਜੇ (((ਲੰਮਾ *)varp == &p_wc) || ((ਲੰਮਾ *)varp == &p_wcm))
    {
	*wcp = *(ਲੰਮਾ *)varp;
	ਜੇ (IS_SPECIAL(*wcp) || ਲੱਭੋ_special_ਕੁੰਜੀ_ਵਿੱਚ_ਸਾਰਣੀ((ਪੂਰਨ)*wcp) >= 0)
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    ਪੂਰਨ
shortmess(ਪੂਰਨ x)
{
    ਵਾਪਸ p_shm != ਨਿਰਾ &&
	    (   vim_strchr(p_shm, x) != ਨਿਰਾ
	    || (vim_strchr(p_shm, 'a') != ਨਿਰਾ
		&& vim_strchr((char_u *)SHM_A, x) != ਨਿਰਾ));
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 * When "fname" is not NULL, use it to set $"envname" when it wasn't set yet.
 */
    ਖਾਲੀ
vimrc_ਮਿਲਿਆ(char_u *fname, char_u *envname)
{
    ਪੂਰਨ		opt_idx;
    ਪੂਰਨ		dofree = FALSE;
    char_u	*p;

    ਜੇ (!option_was_ਸੈੱਟ((char_u *)"cp"))
    {
	p_cp = FALSE;
	ਲਈ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    ਜੇ (!(options[opt_idx].flags & (P_WAS_ਸੈੱਟ|P_VI_DEF)))
		ਸੈੱਟ_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    ਜੇ (fname != ਨਿਰਾ)
    {
	p = vim_getenv(envname, &dofree);
	ਜੇ (p == ਨਿਰਾ)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = ਭਰਿਆ_Name_save(fname, FALSE);
	    ਜੇ (p != ਨਿਰਾ)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	ਨਹੀਂਤਾਂ ਜੇ (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    ਖਾਲੀ
change_compatible(ਪੂਰਨ on)
{
    ਪੂਰਨ	    opt_idx;

    ਜੇ (p_cp != on)
    {
	p_cp = on;
	compatible_ਸੈੱਟ();
    }
    opt_idx = findoption((char_u *)"cp");
    ਜੇ (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_ਸੈੱਟ;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    ਪੂਰਨ
option_was_ਸੈੱਟ(char_u *name)
{
    ਪੂਰਨ idx;

    idx = findoption(name);
    ਜੇ (idx < 0)	// unknown option
	ਵਾਪਸ FALSE;
    ਜੇ (options[idx].flags & P_WAS_ਸੈੱਟ)
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    ਪੂਰਨ
reset_option_was_ਸੈੱਟ(char_u *name)
{
    ਪੂਰਨ idx = findoption(name);

    ਜੇ (idx < 0)
	ਵਾਪਸ FAIL;

    options[idx].flags &= ~P_WAS_ਸੈੱਟ;
    ਵਾਪਸ OK;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    ਸਥਿਰ ਖਾਲੀ
compatible_ਸੈੱਟ(ਖਾਲੀ)
{
    ਪੂਰਨ	    opt_idx;

    ਲਈ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	ਜੇ (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    ਸੈੱਟ_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    ਪੂਰਨ
can_bs(
    ਪੂਰਨ		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਜੇ (what == BS_ਸ਼ੁਰੂ && bt_prompt(curbuf))
	ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
    ਚੋਣ (*p_bs)
    {
	ਹਾਲਤ '3':       ਵਾਪਸ TRUE;
	ਹਾਲਤ '2':	ਵਾਪਸ (what != BS_NOSTOP);
	ਹਾਲਤ '1':	ਵਾਪਸ (what != BS_ਸ਼ੁਰੂ);
	ਹਾਲਤ '0':	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ vim_strchr(p_bs, what) != ਨਿਰਾ;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    ਲੰਮਾ
get_scrolloff_ਮੁੱਲ(ਖਾਲੀ)
{
    ਵਾਪਸ curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    ਲੰਮਾ
get_scrolloffpad_ਮੁੱਲ(ਖਾਲੀ)
{
    ਵਾਪਸ curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    ਲੰਮਾ
get_sidescrolloff_ਮੁੱਲ(ਖਾਲੀ)
{
    ਵਾਪਸ curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ
get_bkc_flags(buf_T *buf)
{
    ਵਾਪਸ buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#ਜੇ defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_ਮੁੱਲ(buf_T *buf)
{
    ਜੇ (buf->b_p_flp == ਨਿਰਾ || *buf->b_p_flp == NUL)
	ਵਾਪਸ p_flp;
    ਵਾਪਸ buf->b_p_flp;
}
#ਜੇ_ਅੰਤ

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ
get_ve_flags(ਖਾਲੀ)
{
    ਵਾਪਸ (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#ਜੇ defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_ਮੁੱਲ(win_T *win)
{
    ਜੇ (win->w_p_sbr == ਨਿਰਾ || *win->w_p_sbr == NUL)
	ਵਾਪਸ p_sbr;
    ਜੇ (STRCMP(win->w_p_sbr, "NONE") == 0)
	ਵਾਪਸ ਖਾਲੀ_ਥਾਵਾਂ_option;
    ਵਾਪਸ win->w_p_sbr;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(ਪੂਰਨ bufopt)
{
    dict_T	*d;
    ਪੂਰਨ		opt_idx;

    d = dict_alloc();
    ਜੇ (d == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	ਬਣਤਰ vimoption *opt = &options[opt_idx];

	ਜੇ ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    ਜੇ (varp != ਨਿਰਾ)
	    {
		ਜੇ (opt->flags & P_STRING)
		    dict_ਜੋੜੋ_string(d, opt->fullname, *(char_u **)varp);
		ਨਹੀਂਤਾਂ ਜੇ (opt->flags & P_ਅੰਕ)
		    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, opt->fullname, *(ਲੰਮਾ *)varp);
		ਨਹੀਂਤਾਂ
		    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, opt->fullname, *(ਪੂਰਨ *)varp);
	    }
	}
    }

    ਵਾਪਸ d;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    ਪੂਰਨ
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    ਜੇ (val == ਨਿਰਾ)
	p = wp->w_p_culopt;
    ਨਹੀਂਤਾਂ
	p = val;
    ਜਦੋਂ (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	ਜੇ (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_ਸਤਰ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_ਸਤਰ | CULOPT_NBR;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	ਜੇ (*p != ',' && *p != NUL)
	    ਵਾਪਸ FAIL;
	ਜੇ (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    ਜੇ ((culopt_flags_new & CULOPT_ਸਤਰ) && (culopt_flags_new & CULOPT_SCRLINE))
	ਵਾਪਸ FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    ਪੂਰਨ
magic_isset(ਖਾਲੀ)
{
    ਚੋਣ (magic_overruled)
    {
	ਹਾਲਤ OPTION_MAGIC_ON:      ਵਾਪਸ TRUE;
	ਹਾਲਤ OPTION_MAGIC_OFF:     ਵਾਪਸ FALSE;
	ਹਾਲਤ OPTION_MAGIC_NOT_ਸੈੱਟ: ਰੋਕੋ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (ਵਿੱਚ_vim9script())
	ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    ਪੂਰਨ
option_ਸੈੱਟ_callback_func(char_u *optval ਅਣਵਰਤਿਆ, callback_T *optcb ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    ਪੂਰਨ		funcname = FALSE;

    ਜੇ (optval == ਨਿਰਾ || *optval == NUL)
    {
	free_callback(optcb);
	ਵਾਪਸ OK;
    }

    ਜੇ (*optval == '{' || (ਵਿੱਚ_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_ਵਿਅੰਜਕ(optval, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    ਜੇ (tv == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    cb = get_callback(tv);
    ਜੇ (cb.cb_name == ਨਿਰਾ || *cb.cb_name == NUL)
    {
	free_tv(tv);
	ਵਾਪਸ FAIL;
    }

    free_callback(optcb);
    ਸੈੱਟ_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = ਨਿਰਾ;
    ਜੇ (ਵਿੱਚ_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != ਨਿਰਾ))
    {
	ਜੇ (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = ਲੱਭੋ_imported(optval, dot - optval, FALSE);
	ਜੇ (import != ਨਿਰਾ && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((ਅੱਖਰ *)fnamebuf, ਆਕਾਰ(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	ਨਹੀਂਤਾਂ
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

    ਵਾਪਸ OK;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_showtabpanel(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    shell_new_columns();
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
    ਸਥਿਰ ਖਾਲੀ
didset_options_sctx(ਪੂਰਨ opt_flags, ਅੱਖਰ **buf)
{
	ਲਈ (ਪੂਰਨ i = 0; ; ++i)
	{
	    ਜੇ (buf[i] == ਨਿਰਾ)
		ਰੋਕੋ;

	    ਪੂਰਨ idx = findoption((char_u *)buf[i]);
	    ਜੇ (idx >= 0)
		ਸੈੱਟ_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#ਜੇ_ਅੰਤ
