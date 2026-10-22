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

#define IN_OPTION_C
#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "optiondefs.h"

സ്ഥിതം ശൂന്യം set_options_default(പൂർണ്ണസംഖ്യ opt_flags);
സ്ഥിതം ശൂന്യം set_string_default_esc(അക്ഷരം *name, char_u *val, പൂർണ്ണസംഖ്യ escape);
സ്ഥിതം char_u *find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
സ്ഥിതം char_u *option_expand(പൂർണ്ണസംഖ്യ opt_idx, char_u *val);
സ്ഥിതം ശൂന്യം didset_options(ശൂന്യം);
സ്ഥിതം ശൂന്യം didset_options2(ശൂന്യം);
#എങ്കിൽ defined(FEAT_EVAL)
സ്ഥിതം long_u *insecure_flag(win_T *wp, പൂർണ്ണസംഖ്യ opt_idx, പൂർണ്ണസംഖ്യ opt_flags);
#അല്ലെങ്കിൽ
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#endif
സ്ഥിതം അക്ഷരം *set_bool_option(പൂർണ്ണസംഖ്യ opt_idx, char_u *varp, പൂർണ്ണസംഖ്യ മൂല്യം, പൂർണ്ണസംഖ്യ opt_flags);
സ്ഥിതം അക്ഷരം *set_അക്കം_option(പൂർണ്ണസംഖ്യ opt_idx, char_u *varp, long മൂല്യം, അക്ഷരം *errbuf, size_t errbuflen, പൂർണ്ണസംഖ്യ opt_flags);
സ്ഥിതം പൂർണ്ണസംഖ്യ find_താക്കോൽ_option(char_u *arg_arg, പൂർണ്ണസംഖ്യ has_lt);
സ്ഥിതം ശൂന്യം showoptions(പൂർണ്ണസംഖ്യ all, പൂർണ്ണസംഖ്യ opt_flags);
സ്ഥിതം പൂർണ്ണസംഖ്യ optval_default(ഘടന vimoption *, char_u *varp, പൂർണ്ണസംഖ്യ compatible);
സ്ഥിതം ശൂന്യം showoneopt(ഘടന vimoption *, പൂർണ്ണസംഖ്യ opt_flags);
സ്ഥിതം പൂർണ്ണസംഖ്യ put_setstring(FILE *fd, bool legacy, അക്ഷരം *cmd, അക്ഷരം *name, char_u **valuep, long_u flags);
സ്ഥിതം പൂർണ്ണസംഖ്യ put_setnum(FILE *fd, അക്ഷരം *cmd, അക്ഷരം *name, long *valuep);
സ്ഥിതം പൂർണ്ണസംഖ്യ put_setbool(FILE *fd, അക്ഷരം *cmd, അക്ഷരം *name, പൂർണ്ണസംഖ്യ മൂല്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ istermoption(ഘടന vimoption *p);
സ്ഥിതം പൂർണ്ണസംഖ്യ istermoption_idx(പൂർണ്ണസംഖ്യ opt_idx);
സ്ഥിതം char_u *get_varp_scope(ഘടന vimoption *p, പൂർണ്ണസംഖ്യ scope);
സ്ഥിതം char_u *get_varp(ഘടന vimoption *);
സ്ഥിതം ശൂന്യം check_win_options(win_T *win);
സ്ഥിതം ശൂന്യം option_value2string(ഘടന vimoption *, പൂർണ്ണസംഖ്യ scope);
സ്ഥിതം ശൂന്യം check_winopt(winopt_T *wop);
സ്ഥിതം പൂർണ്ണസംഖ്യ wc_use_keyname(char_u *varp, long *wcp);
സ്ഥിതം ശൂന്യം compatible_set(ശൂന്യം);

#എങ്കിൽ defined(FEAT_EVAL)
സ്ഥിതം അക്ഷരം *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    ശൂന്യ};
സ്ഥിതം ശൂന്യം didset_options_sctx(പൂർണ്ണസംഖ്യ opt_flags, അക്ഷരം **buf);
#endif


/*
 * Initialize the 'shell' option to a default value.
 */
    സ്ഥിതം ശൂന്യം
set_init_default_shell(ശൂന്യം)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    എങ്കിൽ (((p = mch_getenv((char_u *)"SHELL")) != ശൂന്യ && *p != NUL)
#എങ്കിൽ defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != ശൂന്യ && *p != NUL)
	    || ((p = (char_u *)default_shell()) != ശൂന്യ && *p != NUL)
#endif
	    )
#എങ്കിൽ defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	എങ്കിൽ (vim_strchr(p, ' ') != ശൂന്യ)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    എങ്കിൽ (cmd != ശൂന്യ)
	    {
		vim_snprintf((അക്ഷരം *)cmd, len, "\"%s\"", p);
		set_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	അല്ലെങ്കിൽ
	    set_string_default("sh", p);
    }
#അല്ലെങ്കിൽ
	set_string_default_esc("sh", p, TRUE);
#endif
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    സ്ഥിതം ശൂന്യം
set_init_default_backupskip(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		opt_idx;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		plen;
#ifdef UNIX
    സ്ഥിതം അക്ഷരം	*(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#അല്ലെങ്കിൽ
    സ്ഥിതം അക്ഷരം	*(names[3]) = {"TMPDIR", "TEMP", "TMP"};
#endif
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(names); ++i)
    {
	പൂർണ്ണസംഖ്യ		mustfree = FALSE;
#ifdef UNIX
	എങ്കിൽ (*names[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL("/private/tmp");
# അല്ലെങ്കിൽ
	    p = (char_u *)"/tmp";
	    plen = (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL("/tmp");
# endif
	}
	അല്ലെങ്കിൽ
#endif
	{
	    p = vim_getenv((char_u *)names[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	എങ്കിൽ (p != ശൂന്യ && *p != NUL)
	{
	    char_u  *item;
	    size_t  itemsize;
	    പൂർണ്ണസംഖ്യ	    has_trailing_path_sep = FALSE;

	    എങ്കിൽ (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (പൂർണ്ണസംഖ്യ)STRLEN(p);
		// does the value include a trailing path separator?
		എങ്കിൽ (after_pathsep(p, p + plen))
		    has_trailing_path_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_path_sep ? 0 : 1) + 2;
	    item = alloc(itemsize);
	    എങ്കിൽ (item != ശൂന്യ)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((അക്ഷരം *)item, itemsize, "%s%s*", p, (has_trailing_path_sep) ? "" : PATHSEPSTR);

		എങ്കിൽ (find_dup_item(ga.ga_data, item, itemlen, options[opt_idx].flags) == ശൂന്യ
			&& ga_grow(&ga, (പൂർണ്ണസംഖ്യ)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((അക്ഷരം *)ga.ga_data + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", item);
		}
		vim_free(item);
	    }
	}
	എങ്കിൽ (mustfree)
	    vim_free(p);
    }
    എങ്കിൽ (ga.ga_data != ശൂന്യ)
    {
	set_string_default("bsk", ga.ga_data);
	vim_free(ga.ga_data);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    സ്ഥിതം ശൂന്യം
set_init_default_maxmemtot(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    എങ്കിൽ (opt_idx < 0)
	തിരികെ;

#എങ്കിൽ !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
    എങ്കിൽ (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#endif
    {
#എങ്കിൽ defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_TOTAL_MEM)
	// Use amount of memory available to Vim.
	n = (mch_total_mem(FALSE) >> 1);
#അല്ലെങ്കിൽ
	n = (0x7fffffff >> 11);
#endif
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	എങ്കിൽ (opt_idx >= 0)
	{
#എങ്കിൽ !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
	    എങ്കിൽ ((long)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (long)n
		    || (long)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#endif
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    സ്ഥിതം ശൂന്യം
set_init_default_cdpath(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    പൂർണ്ണസംഖ്യ	i;
    പൂർണ്ണസംഖ്യ	j;
    പൂർണ്ണസംഖ്യ	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    എങ്കിൽ (cdpath == ശൂന്യ)
	തിരികെ;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    എങ്കിൽ (buf != ശൂന്യ)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	വേണ്ടി (i = 0; cdpath[i] != NUL; ++i)
	{
	    എങ്കിൽ (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	എങ്കിൽ (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	അല്ലെങ്കിൽ
	    vim_free(buf); // cannot happen
    }
    എങ്കിൽ (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    സ്ഥിതം ശൂന്യം
set_init_default_printencoding(ശൂന്യം)
{
#എങ്കിൽ defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    set_string_default("penc",
# എങ്കിൽ defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# അല്ലെങ്കിൽ // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#endif
}

#എങ്കിൽ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    സ്ഥിതം ശൂന്യം
set_init_default_printexpr(ശൂന്യം)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    set_string_default("pexpr",
# എങ്കിൽ defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# അല്ലെങ്കിൽ
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#endif

#ifdef UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    സ്ഥിതം ശൂന്യം
set_init_restricted_mode(ശൂന്യം)
{
    char_u	*p;

    p = get_isolated_shell_name();
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#endif

#ifdef CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    സ്ഥിതം ശൂന്യം
set_init_clean_rtp(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    എങ്കിൽ (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    എങ്കിൽ (opt_idx < 0)
	തിരികെ;
    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
    p_pp = (char_u *)CLEAN_RUNTIMEPATH;
}
#endif

#ifdef UNIX
/*
 * Change 'runtimepath' and 'packdir' to '$XDG_CONFIG_HOME/vim' if the only
 * vimrc found is located in '$XDG_CONFIG_HOME/vim/vimrc'.
 * In case the '$XDG_CONFIG_HOME' variable is not set, '$HOME/.config' is used
 * as a fallback as is defined in the XDG base dir specification:
 * <https://specifications.freedesktop.org/basedir/latest/>
 */
    സ്ഥിതം ശൂന്യം
set_init_xdg_rtp(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		opt_idx;
    പൂർണ്ണസംഖ്യ		has_xdg_env = TRUE;
    പൂർണ്ണസംഖ്യ		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = ശൂന്യ;
    char_u	*vimrc2 = ശൂന്യ;
    char_u	*xdg_dir = ശൂന്യ;
    char_u	*xdg_rtp = ശൂന്യ;
    string_T	vimrc_xdg = {ശൂന്യ, 0};

    // initialize chartab, so we can expand $HOME
    (ശൂന്യം)init_chartab();
    vimrc1 = expand_env_save((char_u *)USR_VIMRC_FILE);
    vimrc2 = expand_env_save((char_u *)USR_VIMRC_FILE2);

    xdg_dir = mch_getenv("XDG_CONFIG_HOME");
    എങ്കിൽ (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    എങ്കിൽ (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	പോകുക theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    എങ്കിൽ ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	പോകുക theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    എങ്കിൽ ((opt_idx = findoption((char_u *)"packpath")) < 0)
	പോകുക theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# എങ്കിൽ defined(XDG_VDIR) && defined(FEAT_SESSION)
    എങ്കിൽ ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	പോകുക theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    എങ്കിൽ (should_free_xdg_dir)
	vim_free(xdg_dir);
}
#endif

/*
 * Expand environment variables and things like "~" for the defaults.
 * If option_expand() returns non-NULL the variable is expanded.  This can
 * only happen for non-indirect options.
 * Also set the default to the expanded value, so ":set" does not list
 * them.
 * Don't set the P_ALLOCED flag, because we don't want to free the
 * default.
 */
    സ്ഥിതം ശൂന്യം
set_init_expand_env(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		opt_idx;
    char_u	*p;

    വേണ്ടി (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	എങ്കിൽ ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != ശൂന്യ)
	    p = (char_u *)_(*(അക്ഷരം **)options[opt_idx].var);
	അല്ലെങ്കിൽ
	    p = option_expand(opt_idx, ശൂന്യ);
	എങ്കിൽ (p != ശൂന്യ && (p = vim_strsave(p)) != ശൂന്യ)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    എങ്കിൽ (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#എങ്കിൽ defined(MSWIN) && defined(FEAT_GETTEXT)
/*
 * Get the display language of Windows and the languages to fall back on, as a
 * colon separated list for gettext, e.g. "ja_JP:en_US".  The list stops after
 * English, untranslated messages are English already.
 * Returns NULL when it cannot be obtained.  The result must be freed.
 */
    സ്ഥിതം char_u *
get_ui_langs(ശൂന്യം)
{
    ULONG	അക്കം_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*buffer;
    char_u	*langs = ശൂന്യ;

    എങ്കിൽ (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &അക്കം_languages, ശൂന്യ,
								    &bufsize)
	    || bufsize == 0)
	തിരികെ ശൂന്യ;

    buffer = ALLOC_MULT(WCHAR, bufsize);
    എങ്കിൽ (buffer == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &അക്കം_languages, buffer,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	എങ്കിൽ (langs != ശൂന്യ)
	{
	    char_u	*d = langs;
	    WCHAR	*s = buffer;

	    ആകുമ്പോൾ (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		എങ്കിൽ (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		വേണ്ടി ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		എങ്കിൽ (english)
		    നിർത്തുക;
	    }
	    *d = NUL;
	}
    }
    vim_free(buffer);

    തിരികെ langs;
}
#endif

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    സ്ഥിതം ശൂന്യം
set_init_lang_env(ശൂന്യം)
{
#എങ്കിൽ defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    എങ്കിൽ (mch_getenv((char_u *)"LANG") == ശൂന്യ
	    && mch_getenv((char_u *)"LANGUAGE") == ശൂന്യ
	    && mch_getenv((char_u *)"LC_ALL") == ശൂന്യ
	    && mch_getenv((char_u *)"LC_MESSAGES") == ശൂന്യ)
    {
	char_u	*langs = get_ui_langs();

	എങ്കിൽ (langs != ശൂന്യ && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    എങ്കിൽ (colon != ശൂന്യ)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#അല്ലെങ്കിലെങ്കിൽ defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#endif
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    സ്ഥിതം ശൂന്യം
set_init_default_encoding(ശൂന്യം)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		opt_idx;

#എങ്കിൽ defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#അല്ലെങ്കിൽ
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#endif
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    എങ്കിൽ (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    എങ്കിൽ (mb_init() == ശൂന്യ)
    {
	opt_idx = findoption((char_u *)"encoding");
	എങ്കിൽ (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#എങ്കിൽ defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	എങ്കിൽ (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    set_string_option_direct((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    set_string_option_direct((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    എങ്കിൽ (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    എങ്കിൽ (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (ശൂന്യം)init_chartab();
	}
#endif

#എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	എങ്കിൽ (
# ifdef VIMDLL
		(!gui.in_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    അക്ഷരം	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    എങ്കിൽ (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, വലുപ്പം(buf), "cp%ld", (long)GetACP());
	    അല്ലെങ്കിൽ
		buflen = vim_snprintf(buf, വലുപ്പം(buf), "cp%ld", (long)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    എങ്കിൽ (p_tenc != ശൂന്യ)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		എങ്കിൽ (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_setup(&നിവേശം_conv, p_tenc, p_enc);
		convert_setup(&output_conv, p_enc, p_tenc);
	    }
	    അല്ലെങ്കിൽ
		p_tenc = empty_option;
	}
#endif
#എങ്കിൽ defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#endif
    }
    അല്ലെങ്കിൽ
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
    ശൂന്യം
set_init_1(പൂർണ്ണസംഖ്യ clean_arg)
{
#ifdef FEAT_LANGMAP
    langmap_init();
#endif

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    എങ്കിൽ (mch_getenv((char_u *)"VIM_POSIX") != ശൂന്യ)
    {
	set_string_default("cpo", (char_u *)CPO_ALL);
	set_string_default("shm", (char_u *)SHM_POSIX);
    }

    set_init_default_shell();
    set_init_default_backupskip();
    set_init_default_maxmemtot();
    set_init_default_cdpath();
    set_init_default_printencoding();
#എങ്കിൽ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    set_init_default_printexpr();
#endif

    /*
     * Set all the options (except the terminal options) to their default
     * value.  Also set the global value for local options.
     */
    set_options_default(0);

#ifdef UNIX
    set_init_xdg_rtp();
    set_init_restricted_mode();
#endif

#ifdef CLEAN_RUNTIMEPATH
    എങ്കിൽ (clean_arg)
	set_init_clean_rtp();
#endif

#ifdef FEAT_GUI
    എങ്കിൽ (found_reverse_arg)
	set_option_മൂല്യം_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
#endif

    curbuf->b_p_initialized = true;
    curbuf->b_p_ac = -1;
    curbuf->b_p_ar = -1;	// no local 'autoread' value
#ifdef HAVE_FSYNC
    curbuf->b_p_fs = -1;	// no local 'fsync' value
#endif
    curbuf->b_p_ul = NO_LOCAL_UNDOLEVEL;
    check_buf_options(curbuf);
    check_win_options(curwin);
    check_options();

    // Must be before option_expand(), because that one needs vim_isIDc()
    didset_options();

#ifdef FEAT_SPELL
    // Use the current chartab for the generic chartab. This is not in
    // didset_options() because it only depends on 'encoding'.
    init_spell_chartab();
#endif

    set_init_default_encoding();

    // Expand environment variables and things like "~" for the defaults.
    set_init_expand_env();

    save_file_ff(curbuf);	// Buffer is unchanged

#എങ്കിൽ defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    എങ്കിൽ (mch_getenv((char_u *)"MLTERM") != ശൂന്യ)
	set_option_മൂല്യം_give_err((char_u *)"tbidi", 1L, ശൂന്യ, 0);
#endif

    didset_options2();

    set_init_lang_env();

#ifdef FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    set_helplang_default(get_mess_lang());
#endif
}

സ്ഥിതം char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    ശൂന്യം
set_fencs_unicode(ശൂന്യം)
{
    set_string_option_direct((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    സ്ഥിതം ശൂന്യം
set_option_default(
    പൂർണ്ണസംഖ്യ		opt_idx,
    പൂർണ്ണസംഖ്യ		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    പൂർണ്ണസംഖ്യ		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    പൂർണ്ണസംഖ്യ		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    പൂർണ്ണസംഖ്യ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    എങ്കിൽ (varp != ശൂന്യ)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	എങ്കിൽ (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    എങ്കിൽ (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		set_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    അല്ലെങ്കിൽ എങ്കിൽ (options[opt_idx].indir != PV_NONE)
		set_string_option_direct(ശൂന്യ, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (flags & P_അക്കം)
	{
	    എങ്കിൽ (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    അല്ലെങ്കിൽ
	    {
		long def_val = (long)(long_i)options[opt_idx].def_val[dvi];

		എങ്കിൽ ((long *)varp == &curwin->w_p_so
			|| (long *)varp == &curwin->w_p_siso
			|| (long *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(long *)varp = -1;
		അല്ലെങ്കിൽ
		    *(long *)varp = def_val;
		// May also set global value for local option.
		എങ്കിൽ (both)
		    *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	അല്ലെങ്കിൽ	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(പൂർണ്ണസംഖ്യ *)varp = (പൂർണ്ണസംഖ്യ)(long)(long_i)options[opt_idx].def_val[dvi];
#ifdef UNIX
	    // 'modeline' defaults to off for root
	    എങ്കിൽ (options[opt_idx].indir == PV_ML && getuid() == വേര്_UID)
		*(പൂർണ്ണസംഖ്യ *)varp = FALSE;
#endif
	    // May also set global value for local option.
	    എങ്കിൽ (both)
		*(പൂർണ്ണസംഖ്യ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(പൂർണ്ണസംഖ്യ *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	എങ്കിൽ (both)
	{
	    flagsp = insecure_flag(curwin, opt_idx, OPT_LOCAL);
	    *flagsp = *flagsp & ~P_INSECURE;
	}
    }

#ifdef FEAT_EVAL
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
}

/*
 * Set all options (except terminal options) to their default value.
 * When "opt_flags" is non-zero skip 'encoding'.
 */
    സ്ഥിതം ശൂന്യം
set_options_default(
    പൂർണ്ണസംഖ്യ		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    പൂർണ്ണസംഖ്യ		i;
    win_T	*wp;
    tabpage_T	*tp;

    വേണ്ടി (i = 0; !istermoption_idx(i); i++)
	എങ്കിൽ (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#എങ്കിൽ defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_താക്കോൽ
#endif
			)))
	    set_option_default(i, opt_flags, p_cp);

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
    സ്ഥിതം ശൂന്യം
set_string_default_esc(അക്ഷരം *name, char_u *val, പൂർണ്ണസംഖ്യ escape)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		opt_idx;

    എങ്കിൽ (escape && vim_strchr(val, ' ') != ശൂന്യ)
	p = vim_strsave_escaped(val, (char_u *)" ");
    അല്ലെങ്കിൽ
	p = vim_strsave(val);
    എങ്കിൽ (p == ശൂന്യ)		// we don't want a NULL
	തിരികെ;

    opt_idx = findoption((char_u *)name);
    എങ്കിൽ (opt_idx < 0)
    {
	vim_free(p);
	തിരികെ;
    }

    എങ്കിൽ (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    ശൂന്യം
set_string_default(അക്ഷരം *name, char_u *val)
{
    set_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    സ്ഥിതം char_u *
find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags)
{
    പൂർണ്ണസംഖ്യ	    bs = 0;
    char_u  *s;

    എങ്കിൽ (origval == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (s = origval; *s != NUL; ++s)
    {
	എങ്കിൽ ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    തിരികെ s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	എങ്കിൽ ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	അല്ലെങ്കിൽ
	    bs = 0;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    ശൂന്യം
set_സംഖ്യ_default(അക്ഷരം *name, long val)
{
    പൂർണ്ണസംഖ്യ		opt_idx;

    opt_idx = findoption((char_u *)name);
    എങ്കിൽ (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#എങ്കിൽ defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    ശൂന്യം
set_local_options_default(win_T *wp, പൂർണ്ണസംഖ്യ do_buffer)
{
    win_T	*save_curwin = curwin;
    പൂർണ്ണസംഖ്യ		i;

    curwin = wp;
    curbuf = curwin->w_buffer;
    block_autocmds();

    വേണ്ടി (i = 0; !istermoption_idx(i); i++)
    {
	ഘടന vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	എങ്കിൽ (p->indir != PV_NONE
		&& (do_buffer || (p->indir & PV_BUF) == 0)
		&& !(options[i].flags & P_NODEFAULT)
		&& !optval_default(p, varp, FALSE))
	    set_option_default(i, OPT_FREE|OPT_LOCAL, FALSE);
    }

    unblock_autocmds();
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
}
#endif

#എങ്കിൽ defined(EXITFREE)
/*
 * Free all options.
 */
    ശൂന്യം
free_all_options(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; !istermoption_idx(i); i++)
    {
	എങ്കിൽ (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    എങ്കിൽ ((options[i].flags & P_ALLOCED) && options[i].var != ശൂന്യ)
		free_string_option(*(char_u **)options[i].var);
	    എങ്കിൽ (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# എങ്കിൽ defined(FEAT_EVAL)
    free_findfunc_option();
# endif
}
#endif


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    ശൂന്യം
set_init_2(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    എങ്കിൽ (idx >= 0 && !(options[idx].flags & P_WAS_SET))
	set_option_default(idx, OPT_LOCAL, p_cp);
    comp_col();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    എങ്കിൽ (!option_was_set((char_u *)"window"))
	p_window = Rows - 1;
    set_സംഖ്യ_default("window", Rows - 1);

    // For DOS console the default is always black.
#എങ്കിൽ !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    എങ്കിൽ (idx >= 0 && !(options[idx].flags & P_WAS_SET)
						 && *term_bg_default() == 'd')
    {
	set_string_option_direct(ശൂന്യ, idx, (char_u *)"dark", OPT_FREE, 0);
	// don't mark it as set, when starting the GUI it may be
	// changed again
	options[idx].flags &= ~P_WAS_SET;
    }
#endif

#ifdef CURSOR_SHAPE
    parse_shape_opt(SHAPE_CURSOR);	// set cursor shapes from 'guicursor'
#endif
#ifdef FEAT_MOUSESHAPE
    parse_shape_opt(SHAPE_MOUSE);	// set mouse shapes from 'mouseshape'
#endif
#ifdef FEAT_PRINTER
    (ശൂന്യം)parse_printoptions(ശൂന്യ);	// parse 'printoptions' default value
#endif
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    ശൂന്യം
set_init_3(ശൂന്യം)
{
#എങ്കിൽ defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    idx_srr;
    പൂർണ്ണസംഖ്യ	    do_srr;
# ifdef FEAT_QUICKFIX
    പൂർണ്ണസംഖ്യ	    idx_sp;
    പൂർണ്ണസംഖ്യ	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    എങ്കിൽ (idx_srr < 0)
	do_srr = FALSE;
    അല്ലെങ്കിൽ
	do_srr = !(options[idx_srr].flags & P_WAS_SET);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    എങ്കിൽ (idx_sp < 0)
	do_sp = FALSE;
    അല്ലെങ്കിൽ
	do_sp = !(options[idx_sp].flags & P_WAS_SET);
# endif
    p = get_isolated_shell_name();
    എങ്കിൽ (p != ശൂന്യ)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	എങ്കിൽ (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# എങ്കിൽ defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# എങ്കിൽ defined(FEAT_QUICKFIX)
	    എങ്കിൽ (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  അല്ലെങ്കിൽ
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    എങ്കിൽ (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	അല്ലെങ്കിൽ എങ്കിൽ (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  എങ്കിൽ defined(FEAT_QUICKFIX)
		എങ്കിൽ (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		എങ്കിൽ (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	അല്ലെങ്കിൽ
	    // Always use POSIX shell style redirection if we reach this
	    എങ്കിൽ (       fnamecmp(p, "sh") == 0
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
# എങ്കിൽ defined(FEAT_QUICKFIX)
		എങ്കിൽ (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  അല്ലെങ്കിൽ
		    എങ്കിൽ (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    അല്ലെങ്കിൽ
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		എങ്കിൽ (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#endif

#എങ്കിൽ defined(MSWIN)
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
    എങ്കിൽ (strstr((അക്ഷരം *)gettail(p_sh), "powershell") != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	എങ്കിൽ (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	എങ്കിൽ (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strstr((അക്ഷരം *)gettail(p_sh), "sh") != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	idx3;

	idx3 = findoption((char_u *)"shcf");
	എങ്കിൽ (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	എങ്കിൽ (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strstr((അക്ഷരം *)gettail(p_sh), "cmd.exe") != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	idx3;

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
	എങ്കിൽ (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	എങ്കിൽ (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#endif

    എങ്കിൽ (BUFEMPTY())
    {
	പൂർണ്ണസംഖ്യ idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	എങ്കിൽ (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_SET))
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    set_title_defaults();
}

#എങ്കിൽ defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    ശൂന്യം
set_helplang_default(char_u *lang)
{
    പൂർണ്ണസംഖ്യ		idx;
    size_t	langlen;

    എങ്കിൽ (lang == ശൂന്യ)	// safety check
	തിരികെ;

    langlen = STRLEN(lang);
    എങ്കിൽ (langlen < 2)	// safety check
	തിരികെ;

    idx = findoption((char_u *)"hlg");
    എങ്കിൽ (idx < 0 || (options[idx].flags & P_WAS_SET))
	തിരികെ;

    എങ്കിൽ (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    എങ്കിൽ (p_hlg == ശൂന്യ)
	p_hlg = empty_option;
    അല്ലെങ്കിൽ
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	എങ്കിൽ (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	അല്ലെങ്കിൽ എങ്കിൽ (langlen >= 1 && *p_hlg == 'C')
	{
	    p_hlg[0] = 'e';
	    p_hlg[1] = 'n';
	}
	p_hlg[2] = NUL;
    }
    options[idx].flags |= P_ALLOCED;
}
#endif

/*
 * 'title' and 'icon' only default to true if they have not been set or reset
 * in .vimrc and we can read the old value.
 * When 'title' and 'icon' have been reset in .vimrc, we won't even check if
 * they can be reset.  This reduces startup time when using X on a remote
 * machine.
 */
    ശൂന്യം
set_title_defaults(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    idx1;
    long    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    എങ്കിൽ (idx1 >= 0 && !(options[idx1].flags & P_WAS_SET))
    {
#ifdef FEAT_GUI
	എങ്കിൽ (gui.starting || gui.in_use)
	    val = TRUE;
	അല്ലെങ്കിൽ
#endif
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    എങ്കിൽ (idx1 < 0 || (options[idx1].flags & P_WAS_SET))
    {
	തിരികെ;
    }

#ifdef FEAT_GUI
    എങ്കിൽ (gui.starting || gui.in_use)
	val = TRUE;
    അല്ലെങ്കിൽ
#endif
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    ശൂന്യം
ex_set(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		flags = 0;

    എങ്കിൽ (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#എങ്കിൽ defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    അല്ലെങ്കിൽ
#endif
    {
	എങ്കിൽ (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(ശൂന്യം)do_set(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
തരനിർവചനം എണ്ണൽ {
    PREFIX_NO = 0,	// "no" prefix
    PREFIX_NONE,	// no prefix
    PREFIX_INV,		// "inv" prefix
} set_prefix_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    സ്ഥിതം set_prefix_T
get_option_prefix(char_u **argp)
{
    പൂർണ്ണസംഖ്യ		prefix = PREFIX_NONE;
    char_u	*arg = *argp;

    എങ്കിൽ (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	prefix = PREFIX_NO;
	arg += 2;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "inv", 3) == 0)
    {
	prefix = PREFIX_INV;
	arg += 3;
    }

    *argp = arg;
    തിരികെ prefix;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_option_name(char_u *arg, പൂർണ്ണസംഖ്യ *opt_idxp, പൂർണ്ണസംഖ്യ *lenp, പൂർണ്ണസംഖ്യ *keyp)
{
    പൂർണ്ണസംഖ്യ	താക്കോൽ = 0;
    പൂർണ്ണസംഖ്യ	len;
    പൂർണ്ണസംഖ്യ	opt_idx;

    എങ്കിൽ (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	എങ്കിൽ (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	അല്ലെങ്കിൽ
	{
	    len = 1;
	    ആകുമ്പോൾ (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	എങ്കിൽ (arg[len] != '>')
	    തിരികെ FAIL;

	arg[len] = NUL;			    // put NUL after name
	എങ്കിൽ (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	എങ്കിൽ (opt_idx == -1)
	    താക്കോൽ = find_താക്കോൽ_option(arg + 1, TRUE);
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	എങ്കിൽ (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	അല്ലെങ്കിൽ
	    ആകുമ്പോൾ (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	എങ്കിൽ (opt_idx == -1)
	    താക്കോൽ = find_താക്കോൽ_option(arg, FALSE);
    }

    *keyp = താക്കോൽ;
    *lenp = len;
    *opt_idxp = opt_idx;

    തിരികെ OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    സ്ഥിതം set_op_T
get_opt_op(char_u *arg)
{
    set_op_T op = OP_NONE;

    എങ്കിൽ (*arg != NUL && *(arg + 1) == '=')
    {
	എങ്കിൽ (*arg == '+')
	    op = OP_ADDING;		// "+="
	അല്ലെങ്കിൽ എങ്കിൽ (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	അല്ലെങ്കിൽ എങ്കിൽ (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    തിരികെ op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
സ്ഥിതം അക്ഷരം *modeline_whitelist[] =
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
    ശൂന്യ
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    സ്ഥിതം bool
is_modeline_whitelisted(അക്ഷരം *name)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; modeline_whitelist[i] != ശൂന്യ; i++)
	എങ്കിൽ (STRCMP(name, modeline_whitelist[i]) == 0)
	    തിരികെ true;
    തിരികെ false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
validate_opt_idx(
	പൂർണ്ണസംഖ്യ opt_idx,
	പൂർണ്ണസംഖ്യ opt_flags,
	long_u flags,
	അക്ഷരം **errmsg,
	set_prefix_T prefix)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    എങ്കിൽ ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	തിരികെ FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    എങ്കിൽ ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	തിരികെ FAIL;

    // Disallow changing some options from modelines.
    എങ്കിൽ (opt_flags & OPT_MODELINE)
    {
	എങ്കിൽ (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_in_modeline;
	    തിരികെ FAIL;
	}
	എങ്കിൽ ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_in_modeline_when_modelineexpr_is_off;
	    തിരികെ FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	എങ്കിൽ (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    എങ്കിൽ (options[opt_idx].indir == PV_ML && prefix == PREFIX_NO)
		തിരികെ OK;
	    അല്ലെങ്കിൽ എങ്കിൽ (!is_modeline_whitelisted(options[opt_idx].fullname))
		തിരികെ FAIL;
	}
#ifdef FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	എങ്കിൽ (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    തിരികെ FAIL;
#endif
    }

#ifdef HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    എങ്കിൽ (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_in_sandbox;
	തിരികെ FAIL;
    }
#endif

    തിരികെ OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    സ്ഥിതം char_u *
stropt_get_default_val(
    പൂർണ്ണസംഖ്യ		opt_idx,
    char_u	*varp,
    പൂർണ്ണസംഖ്യ		flags,
    പൂർണ്ണസംഖ്യ		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    എങ്കിൽ ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ifdef FEAT_GUI
	എങ്കിൽ (gui.in_use)
	    newval = gui_bg_default();
	അല്ലെങ്കിൽ
#endif
	    newval = term_bg_default();
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    എങ്കിൽ (newval == ശൂന്യ)
	newval = empty_option;
    അല്ലെങ്കിൽ
    {
	char_u *s = option_expand(opt_idx, newval);
	എങ്കിൽ (s == ശൂന്യ)
	    s = newval;
	newval = vim_strsave(s);
    }

    തിരികെ newval;
}

/*
 * Convert the 'backspace' option number value to a string: for adding,
 * prepending and removing string.
 */
    സ്ഥിതം ശൂന്യം
opt_backspace_nr2str(
    char_u	*varp,
    char_u	**origval_p,
    char_u	**origval_l_p,
    char_u	**origval_g_p,
    char_u	**oldval_p)
{
    പൂർണ്ണസംഖ്യ		i = getdigits((char_u **)varp);

    തിരഞ്ഞെടുപ്പ് (i)
    {
	അവസ്ഥ 0:
	    *(char_u **)varp = empty_option;
	    നിർത്തുക;
	അവസ്ഥ 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    നിർത്തുക;
	അവസ്ഥ 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    നിർത്തുക;
	അവസ്ഥ 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    നിർത്തുക;
    }
    vim_free(*oldval_p);
    എങ്കിൽ (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    എങ്കിൽ (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    എങ്കിൽ (*origval_g_p == *oldval_p)
	*origval_g_p = *(char_u **)varp;
    *oldval_p = *(char_u **)varp;
}

/*
 * Convert the 'whichwrap' option number value to a string, for backwards
 * compatibility with Vim 3.0.
 * Note: 'argp' is a pointer to a char_u pointer and is updated.
 */
    സ്ഥിതം char_u *
opt_whichwrap_nr2str(char_u **argp, char_u *whichwrap)
{
    size_t  len = 0;

    *whichwrap = NUL;
    പൂർണ്ണസംഖ്യ i = getdigits(argp);
    എങ്കിൽ (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    എങ്കിൽ (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    എങ്കിൽ (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    എങ്കിൽ (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    എങ്കിൽ (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    എങ്കിൽ (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    തിരികെ whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    സ്ഥിതം char_u *
stropt_copy_മൂല്യം(
    char_u	*origval,
    char_u	**argp,
    set_op_T	op,
    പൂർണ്ണസംഖ്യ		flags UNUSED)
{
    char_u	*arg = *argp;
    unsigned	newlen;
    char_u	*newval;
    char_u	*s = ശൂന്യ;

    // get a bit too much
    newlen = (unsigned)STRLEN(arg) + 1;
    എങ്കിൽ (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;
    newval = alloc(newlen);
    എങ്കിൽ (newval == ശൂന്യ)  // out of mem, don't change
	തിരികെ ശൂന്യ;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    ആകുമ്പോൾ (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	പൂർണ്ണസംഖ്യ i;

	എങ്കിൽ (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#endif
	   )
	    ++arg;	// remove backslash
	എങ്കിൽ (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	അല്ലെങ്കിൽ
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    തിരികെ newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    സ്ഥിതം char_u *
stropt_expand_envvar(
    പൂർണ്ണസംഖ്യ		opt_idx,
    char_u	*origval,
    char_u	*newval,
    set_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    എങ്കിൽ (s == ശൂന്യ)
	തിരികെ newval;

    vim_free(newval);
    unsigned newlen = (unsigned)STRLEN(s) + 1;
    എങ്കിൽ (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;

    newval = alloc(newlen);
    എങ്കിൽ (newval == ശൂന്യ)
	തിരികെ ശൂന്യ;

    STRCPY(newval, s);

    തിരികെ newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    സ്ഥിതം ശൂന്യം
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    പൂർണ്ണസംഖ്യ		flags)
{
    പൂർണ്ണസംഖ്യ		len = 0;

    പൂർണ്ണസംഖ്യ comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    എങ്കിൽ (op == OP_ADDING)
    {
	len = (പൂർണ്ണസംഖ്യ)STRLEN(origval);
	// strip a trailing comma, would get 2
	എങ്കിൽ (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    അല്ലെങ്കിൽ
    {
	len = (പൂർണ്ണസംഖ്യ)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    എങ്കിൽ (comma)
	newval[len] = ',';
}

/*
 * Remove a value from a string option.  Copy string option value in "origval"
 * to "newval" and then remove the string "strval" of length "len".
 */
    സ്ഥിതം ശൂന്യം
stropt_remove_val(
    char_u	*origval,
    char_u	*newval,
    പൂർണ്ണസംഖ്യ		flags,
    char_u	*strval,
    പൂർണ്ണസംഖ്യ		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    എങ്കിൽ (*strval)
    {
	// may need to remove a comma
	എങ്കിൽ (flags & P_COMMA)
	{
	    എങ്കിൽ (strval == origval)
	    {
		// include comma after string
		എങ്കിൽ (strval[len] == ',')
		    ++len;
	    }
	    അല്ലെങ്കിൽ
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
    സ്ഥിതം char_u *
find_താക്കോൽ_item(char_u *src, char_u *താക്കോൽ, പൂർണ്ണസംഖ്യ keylen, പൂർണ്ണസംഖ്യ *itemlenp)
{
    char_u  *p = src;

    ആകുമ്പോൾ (*p != NUL)
    {
	// Check if this item starts with the same key
	എങ്കിൽ ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, താക്കോൽ, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *end = vim_strchr(p, ',');
	    എങ്കിൽ (end == ശൂന്യ)
		end = p + STRLEN(p);
	    *itemlenp = (പൂർണ്ണസംഖ്യ)(end - p);
	    തിരികെ p;
	}
	++p;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    സ്ഥിതം ശൂന്യം
remove_comma_item(char_u *str, char_u *item, പൂർണ്ണസംഖ്യ itemlen)
{
    എങ്കിൽ (item[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(item, item + itemlen + 1);
    അല്ലെങ്കിൽ എങ്കിൽ (item > str && *(item - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(item - 1, item + itemlen);
    അല്ലെങ്കിൽ
	// Only item
	*item = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    സ്ഥിതം ശൂന്യം
remove_താക്കോൽ_item(char_u *str, char_u *താക്കോൽ, പൂർണ്ണസംഖ്യ keylen, char_u *skip)
{
    പൂർണ്ണസംഖ്യ	    itemlen;
    char_u  *found;

    ആകുമ്പോൾ ((found = find_താക്കോൽ_item(str, താക്കോൽ, keylen, &itemlen)) != ശൂന്യ)
    {
	എങ്കിൽ (found == skip)
	{
	    // Search for the next match after this one.
	    char_u *next = found + itemlen;
	    എങ്കിൽ (*next == ',')
		++next;
	    found = find_താക്കോൽ_item(next, താക്കോൽ, keylen, &itemlen);
	    എങ്കിൽ (found == ശൂന്യ)
		നിർത്തുക;
	}

	remove_comma_item(str, found, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    സ്ഥിതം ശൂന്യം
append_item(char_u *str, char_u *item, പൂർണ്ണസംഖ്യ item_len)
{
    പൂർണ്ണസംഖ്യ len = (പൂർണ്ണസംഖ്യ)STRLEN(str);

    എങ്കിൽ (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, item, (size_t)item_len);
    str[len + item_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    സ്ഥിതം ശൂന്യം
prepend_item(char_u *str, char_u *item, പൂർണ്ണസംഖ്യ item_len)
{
    പൂർണ്ണസംഖ്യ len = (പൂർണ്ണസംഖ്യ)STRLEN(str);
    പൂർണ്ണസംഖ്യ comma = (len > 0) ? 1 : 0;

    mch_memmove(str + item_len + comma, str, (size_t)len + 1);
    mch_memmove(str, item, (size_t)item_len);
    എങ്കിൽ (comma)
	str[item_len] = ',';
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
    സ്ഥിതം bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    പൂർണ്ണസംഖ്യ		flags UNUSED)
{
    char_u  *p;
    char_u  *item_start;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    എങ്കിൽ (vim_strchr(newval, ':') == ശൂന്യ && vim_strchr(newval, ',') == ശൂന്യ)
	തിരികെ false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    എങ്കിൽ (newval_copy == ശൂന്യ)
	തിരികെ false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    item_start = newval_copy;
    വേണ്ടി (;;)
    {
	p = vim_strchr(item_start, ',');
	പൂർണ്ണസംഖ്യ item_len = (p == ശൂന്യ)
			    ? (പൂർണ്ണസംഖ്യ)STRLEN(item_start) : (പൂർണ്ണസംഖ്യ)(p - item_start);

	എങ്കിൽ (item_len > 0)
	{
	    char_u *colon = vim_strchr(item_start, ':');
	    എങ്കിൽ (colon != ശൂന്യ && colon < item_start + item_len)
	    {
		പൂർണ്ണസംഖ്യ keylen = (പൂർണ്ണസംഖ്യ)(colon - item_start) + 1;

		എങ്കിൽ (op == OP_ADDING || op == OP_PREPENDING)
		{
		    പൂർണ്ണസംഖ്യ old_itemlen;
		    char_u *found = find_താക്കോൽ_item(newval, item_start,
						       keylen, &old_itemlen);
		    എങ്കിൽ (found != ശൂന്യ)
		    {
			എങ്കിൽ (old_itemlen == item_len
				&& STRNCMP(found, item_start,
							 item_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_താക്കോൽ_item(newval, item_start,
							   keylen, found);
			}
			അല്ലെങ്കിൽ
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_താക്കോൽ_item(newval, item_start,
							    keylen, ശൂന്യ);
			    എങ്കിൽ (op == OP_PREPENDING)
				prepend_item(newval, item_start, item_len);
			    അല്ലെങ്കിൽ
				append_item(newval, item_start, item_len);
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			// New item.
			എങ്കിൽ (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			അല്ലെങ്കിൽ
			    append_item(newval, item_start, item_len);
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (op == OP_REMOVING)
		    remove_താക്കോൽ_item(newval, item_start, keylen, ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    എങ്കിൽ (found == ശൂന്യ)
		    {
			// New item.
			എങ്കിൽ (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			അല്ലെങ്കിൽ
			    append_item(newval, item_start, item_len);
		    }
		    // else: exact duplicate — do nothing
		}
		അല്ലെങ്കിൽ എങ്കിൽ (op == OP_REMOVING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    എങ്കിൽ (found != ശൂന്യ)
			remove_comma_item(newval, found, item_len);
		}
	    }
	}

	എങ്കിൽ (p == ശൂന്യ)
	    നിർത്തുക;
	item_start = p + 1;
    }

    vim_free(newval_copy);

    തിരികെ true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    സ്ഥിതം ശൂന്യം
stropt_remove_dupflags(char_u *newval, പൂർണ്ണസംഖ്യ flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    ആകുമ്പോൾ (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	എങ്കിൽ (flags & P_ONECOMMA)
	{
	    എങ്കിൽ (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != ശൂന്യ)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		തുടരുക;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != ശൂന്യ)
	    {
		STRMOVE(s, s + 1);
		തുടരുക;
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
    സ്ഥിതം char_u *
stropt_get_newval(
    പൂർണ്ണസംഖ്യ		nextchar,
    പൂർണ്ണസംഖ്യ		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    set_op_T    *op_arg,
    പൂർണ്ണസംഖ്യ		flags,
    പൂർണ്ണസംഖ്യ		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    set_op_T    op = *op_arg;
    char_u	*save_arg = ശൂന്യ;
    char_u	*newval;
    char_u	*s = ശൂന്യ;
    char_u	whichwrap[80];

    എങ്കിൽ (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    അല്ലെങ്കിൽ എങ്കിൽ (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    അല്ലെങ്കിൽ
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	എങ്കിൽ (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	അല്ലെങ്കിൽ എങ്കിൽ (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	അല്ലെങ്കിൽ എങ്കിൽ (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	അല്ലെങ്കിൽ എങ്കിൽ (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_മൂല്യം(origval, &arg, op, flags);
	എങ്കിൽ (newval == ശൂന്യ)
	    പോകുക done;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	എങ്കിൽ (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    എങ്കിൽ (newval == ശൂന്യ)
		പോകുക done;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	എങ്കിൽ ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	അല്ലെങ്കിൽ
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    പൂർണ്ണസംഖ്യ len = 0;
	    എങ്കിൽ (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (പൂർണ്ണസംഖ്യ)STRLEN(newval);
		s = find_dup_item(origval, newval, len, flags);

		// do not add if already there
		എങ്കിൽ ((op == OP_ADDING || op == OP_PREPENDING) && s != ശൂന്യ)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		എങ്കിൽ (s == ശൂന്യ)
		    s = origval + (പൂർണ്ണസംഖ്യ)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    എങ്കിൽ (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    അല്ലെങ്കിൽ എങ്കിൽ (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	എങ്കിൽ (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

done:
    എങ്കിൽ (save_arg != ശൂന്യ)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    തിരികെ newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
do_set_option_string(
	പൂർണ്ണസംഖ്യ	    opt_idx,
	പൂർണ്ണസംഖ്യ	    opt_flags,
	char_u	    **argp,
	പൂർണ്ണസംഖ്യ	    nextchar,
	set_op_T    op_arg,
	long_u	    flags,
	പൂർണ്ണസംഖ്യ	    cp_val,
	char_u	    *varp_arg,
	അക്ഷരം	    *errbuf,
	size_t	    errbuflen,
	പൂർണ്ണസംഖ്യ	    *മൂല്യം_checked,
	അക്ഷരം	    **errmsg)
{
    char_u	*arg = *argp;
    set_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = ശൂന്യ; // previous value if *varp
    char_u	*newval;
    char_u	*origval = ശൂന്യ;
    char_u	*origval_l = ശൂന്യ;
    char_u	*origval_g = ശൂന്യ;
#എങ്കിൽ defined(FEAT_EVAL)
    char_u	*saved_origval = ശൂന്യ;
    char_u	*saved_origval_l = ശൂന്യ;
    char_u	*saved_origval_g = ശൂന്യ;
    char_u	*saved_newval = ശൂന്യ;
#endif

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    എങ്കിൽ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((പൂർണ്ണസംഖ്യ)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    എങ്കിൽ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	എങ്കിൽ (((പൂർണ്ണസംഖ്യ)options[opt_idx].indir & PV_BOTH)
						  && origval_l == empty_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    എങ്കിൽ (((പൂർണ്ണസംഖ്യ)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    അല്ലെങ്കിൽ
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    എങ്കിൽ (newval == ശൂന്യ)
	*(char_u **)(varp) = empty_option;

#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_താക്കോൽ
# endif
		      && origval != ശൂന്യ && newval != ശൂന്യ)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	എങ്കിൽ (origval_l != ശൂന്യ)
	    saved_origval_l = vim_strsave(origval_l);
	എങ്കിൽ (origval_g != ശൂന്യ)
	    saved_origval_g = vim_strsave(origval_g);
    }
#endif

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	പൂർണ്ണസംഖ്യ	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	എങ്കിൽ ((opt_flags & OPT_MODELINE)
#ifdef HAVE_SANDBOX
	      || sandbox != 0
#endif
	      || (op != OP_NONE && (*p & P_INSECURE)))
	    secure = 1;

	// Handle side effects, and set the global value for ":set" on local
	// options. Note: when setting 'syntax' or 'filetype' autocommands may
	// be triggered that can cause havoc.
	*errmsg = did_set_string_option(
			opt_idx, (char_u **)varp, oldval, newval, errbuf,
			errbuflen, opt_flags, op, മൂല്യം_checked);

	secure = secure_saved;
    }

#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ (*errmsg == ശൂന്യ)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#endif

    *argp = arg;
    തിരികെ *errmsg == ശൂന്യ ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    സ്ഥിതം അക്ഷരം *
do_set_option_bool(
    പൂർണ്ണസംഖ്യ		opt_idx,
    പൂർണ്ണസംഖ്യ		opt_flags,
    set_prefix_T prefix,
    long_u	flags,
    char_u	*varp,
    പൂർണ്ണസംഖ്യ		nextchar,
    പൂർണ്ണസംഖ്യ		afterchar,
    പൂർണ്ണസംഖ്യ		cp_val)

{
    varnumber_T	മൂല്യം;

    എങ്കിൽ (nextchar == '=' || nextchar == ':')
	തിരികെ e_invalid_argument;
    എങ്കിൽ (opt_idx < 0 || varp == ശൂന്യ)
	തിരികെ ശൂന്യ;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    എങ്കിൽ (nextchar == '!')
	മൂല്യം = *(പൂർണ്ണസംഖ്യ *)(varp) ^ 1;
    അല്ലെങ്കിൽ എങ്കിൽ (nextchar == '&')
	മൂല്യം = (പൂർണ്ണസംഖ്യ)(long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    അല്ലെങ്കിൽ എങ്കിൽ (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	എങ്കിൽ ((പൂർണ്ണസംഖ്യ *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    മൂല്യം = -1;
	അല്ലെങ്കിൽ എങ്കിൽ ((പൂർണ്ണസംഖ്യ *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    മൂല്യം = -1;
#ifdef HAVE_FSYNC
	അല്ലെങ്കിൽ എങ്കിൽ ((പൂർണ്ണസംഖ്യ *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    മൂല്യം = -1;
#endif
	അല്ലെങ്കിൽ
	    മൂല്യം = *(പൂർണ്ണസംഖ്യ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    അല്ലെങ്കിൽ
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	എങ്കിൽ (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    തിരികെ e_trailing_characters;
	എങ്കിൽ (prefix == PREFIX_INV)
	    മൂല്യം = *(പൂർണ്ണസംഖ്യ *)(varp) ^ 1;
	അല്ലെങ്കിൽ
	    മൂല്യം = prefix == PREFIX_NO ? 0 : 1;
    }

    തിരികെ set_bool_option(opt_idx, varp, (പൂർണ്ണസംഖ്യ)മൂല്യം, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    സ്ഥിതം അക്ഷരം *
do_set_option_numeric(
    പൂർണ്ണസംഖ്യ		opt_idx,
    പൂർണ്ണസംഖ്യ		opt_flags,
    char_u	**argp,
    പൂർണ്ണസംഖ്യ		nextchar,
    set_op_T	op,
    long_u	flags,
    പൂർണ്ണസംഖ്യ		cp_val,
    char_u	*varp,
    അക്ഷരം	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		മൂല്യം;
    പൂർണ്ണസംഖ്യ			i;
    അക്ഷരം		*errmsg = ശൂന്യ;

    എങ്കിൽ (opt_idx < 0 || varp == ശൂന്യ)
	തിരികെ ശൂന്യ;  // "cannot happen"
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
    എങ്കിൽ (nextchar == '&')
	മൂല്യം = (long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    അല്ലെങ്കിൽ എങ്കിൽ (nextchar == '<')
    {
	എങ്കിൽ ((long *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    മൂല്യം = NO_LOCAL_UNDOLEVEL;
	അല്ലെങ്കിൽ എങ്കിൽ (opt_flags == OPT_LOCAL
		    && ((long *)varp == &curwin->w_p_siso
		     || (long *)varp == &curwin->w_p_so
		     || (long *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    മൂല്യം = -1;
	അല്ലെങ്കിൽ
	    മൂല്യം = *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (((long *)varp == &p_wc || (long *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	മൂല്യം = string_to_താക്കോൽ(arg, FALSE);
	എങ്കിൽ (മൂല്യം == 0)
	{
	    errmsg = e_invalid_argument;
	    പോകുക skip;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, ശൂന്യ, &i, STR2NR_ALL, &മൂല്യം, ശൂന്യ, 0, TRUE, ശൂന്യ);
	എങ്കിൽ (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_സംഖ്യ_required_after_equal;
	    പോകുക skip;
	}
    }
    അല്ലെങ്കിൽ
    {
	errmsg = e_സംഖ്യ_required_after_equal;
	പോകുക skip;
    }

    എങ്കിൽ (op == OP_ADDING)
	മൂല്യം = *(long *)varp + മൂല്യം;
    അല്ലെങ്കിൽ എങ്കിൽ (op == OP_PREPENDING)
	മൂല്യം = *(long *)varp * മൂല്യം;
    അല്ലെങ്കിൽ എങ്കിൽ (op == OP_REMOVING)
	മൂല്യം = *(long *)varp - മൂല്യം;

    എങ്കിൽ ((long *)varp == &curwin->w_p_sop && മൂല്യം < -1)
    {
	errmsg = e_invalid_argument;
	പോകുക skip;
    }

    errmsg = set_അക്കം_option(opt_idx, varp, മൂല്യം, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    തിരികെ errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    സ്ഥിതം അക്ഷരം *
do_set_option_keycode(char_u **argp, char_u *താക്കോൽ_name, പൂർണ്ണസംഖ്യ nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    എങ്കിൽ (nextchar == '&')
    {
	എങ്കിൽ (add_termcap_entry(താക്കോൽ_name, TRUE) == FAIL)
	    തിരികെ e_not_found_in_termcap;
    }
    അല്ലെങ്കിൽ
    {
	++arg; // jump to after the '=' or ':'
	വേണ്ടി (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    എങ്കിൽ (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	add_termcode(താക്കോൽ_name, arg, FALSE);
	*p = nextchar;
    }
    എങ്കിൽ (full_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    തിരികെ ശൂന്യ;
}

/*
 * Set an option to a new value.
 */
    സ്ഥിതം അക്ഷരം *
do_set_option_മൂല്യം(
    പൂർണ്ണസംഖ്യ		opt_idx,
    പൂർണ്ണസംഖ്യ		opt_flags,
    char_u	**argp,
    set_prefix_T prefix,
    set_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*താക്കോൽ_name,
    പൂർണ്ണസംഖ്യ		nextchar,
    പൂർണ്ണസംഖ്യ		afterchar,
    പൂർണ്ണസംഖ്യ		cp_val,
    പൂർണ്ണസംഖ്യ		*stopopteval,
    അക്ഷരം	*errbuf,
    size_t	errbuflen)
{
    പൂർണ്ണസംഖ്യ		മൂല്യം_checked = FALSE;
    അക്ഷരം	*errmsg = ശൂന്യ;
    char_u	*arg = *argp;

    എങ്കിൽ (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_set_option_bool(opt_idx, opt_flags, prefix, flags, varp,
						nextchar, afterchar, cp_val);
	എങ്കിൽ (errmsg != ശൂന്യ)
	    പോകുക skip;
    }
    അല്ലെങ്കിൽ
    {
	// numeric or string option
	എങ്കിൽ (vim_strchr((char_u *)"=:&<", nextchar) == ശൂന്യ
		|| prefix != PREFIX_NONE)
	{
	    errmsg = e_invalid_argument;
	    പോകുക skip;
	}

	എങ്കിൽ (flags & P_അക്കം)
	{
	    // numeric option
	    errmsg = do_set_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    എങ്കിൽ (errmsg != ശൂന്യ)
		പോകുക skip;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (opt_idx >= 0)
	{
	    // string option
	    എങ്കിൽ (do_set_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&മൂല്യം_checked, &errmsg) == FAIL)
	    {
		എങ്കിൽ (errmsg != ശൂന്യ)
		    പോകുക skip;
		*stopopteval = TRUE;
		പോകുക skip;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    // key code option
	    errmsg = do_set_option_keycode(&arg, താക്കോൽ_name, nextchar);
	    എങ്കിൽ (errmsg != ശൂന്യ)
		പോകുക skip;
	}
    }

    എങ്കിൽ (opt_idx >= 0)
	did_set_option(opt_idx, opt_flags, op == OP_NONE, മൂല്യം_checked);

skip:
    *argp = arg;
    തിരികെ errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    സ്ഥിതം അക്ഷരം *
do_set_option(
    പൂർണ്ണസംഖ്യ		opt_flags,
    char_u	**argp,
    char_u	*arg_start,
    char_u	**startarg,
    പൂർണ്ണസംഖ്യ		*did_show,
    പൂർണ്ണസംഖ്യ		*stopopteval,
    അക്ഷരം	*errbuf,
    size_t	errbuflen)
{
    പൂർണ്ണസംഖ്യ		opt_idx;
    char_u	*arg;
    set_prefix_T prefix;	// no prefix, "no" prefix or "inv" prefix
    set_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	താക്കോൽ_name[2];
    പൂർണ്ണസംഖ്യ		nextchar;	// next non-white char after option name
    പൂർണ്ണസംഖ്യ		afterchar;	// character just after option name
    അക്ഷരം	*errmsg = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		താക്കോൽ;
    പൂർണ്ണസംഖ്യ		len;

    prefix = get_option_prefix(argp);
    arg = *argp;

    // find end of name
    താക്കോൽ = 0;
    എങ്കിൽ (parse_option_name(arg, &opt_idx, &len, &താക്കോൽ) == FAIL)
	തിരികെ e_invalid_argument;

    // remember character after option name
    afterchar = arg[len];

    എങ്കിൽ (in_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	എങ്കിൽ (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != ശൂന്യ
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    പോകുക skip;
	}
    }
    അല്ലെങ്കിൽ
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	ആകുമ്പോൾ (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    എങ്കിൽ (op != OP_NONE)
	len++;

    nextchar = arg[len];

    എങ്കിൽ (opt_idx == -1 && താക്കോൽ == 0)	// found a mismatch: skip
    {
	എങ്കിൽ (in_vim9script() && arg > arg_start
		&& vim_strchr((char_u *)"!&<", *arg) != ശൂന്യ)
	    errmsg = e_no_white_space_allowed_between_option_and;
	അല്ലെങ്കിൽ
	    errmsg = e_unknown_option;
	പോകുക skip;
    }

    എങ്കിൽ (opt_idx >= 0)
    {
	എങ്കിൽ (options[opt_idx].var == ശൂന്യ)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    എങ്കിൽ (vim_strchr((char_u *)"=:!&<", nextchar) == ശൂന്യ
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    പോകുക skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    അല്ലെങ്കിൽ
    {
	flags = P_STRING;
	varp = ശൂന്യ;
	എങ്കിൽ (താക്കോൽ < 0)
	{
	    താക്കോൽ_name[0] = KEY2TERMCAP0(താക്കോൽ);
	    താക്കോൽ_name[1] = KEY2TERMCAP1(താക്കോൽ);
	}
	അല്ലെങ്കിൽ
	{
	    താക്കോൽ_name[0] = KS_താക്കോൽ;
	    താക്കോൽ_name[1] = (താക്കോൽ & 0xff);
	}
    }

    // Make sure the option value can be changed.
    എങ്കിൽ (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, prefix) == FAIL)
	പോകുക skip;

    പൂർണ്ണസംഖ്യ cp_val = p_cp;
    എങ്കിൽ (vim_strchr((char_u *)"?=:!&<", nextchar) != ശൂന്യ)
    {
	arg += len;
	എങ്കിൽ (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    എങ്കിൽ (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    അല്ലെങ്കിൽ		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	എങ്കിൽ (vim_strchr((char_u *)"?!&<", nextchar) != ശൂന്യ
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    പോകുക skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    എങ്കിൽ (nextchar == '?'
	    || (prefix == PREFIX_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == ശൂന്യ
		&& !(flags & P_BOOL)))
    {
	// print value
	എങ്കിൽ (*did_show)
	    സന്ദേശം_putchar('\n');	    // cursor below last one
	അല്ലെങ്കിൽ
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	എങ്കിൽ (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ifdef FEAT_EVAL
	    എങ്കിൽ (p_verbose > 0)
	    {
		// Mention where the option was last set.
		എങ്കിൽ (varp == options[opt_idx].var)
		    last_set_സന്ദേശം(options[opt_idx].script_ctx);
		അല്ലെങ്കിൽ എങ്കിൽ ((പൂർണ്ണസംഖ്യ)options[opt_idx].indir & PV_WIN)
		    last_set_സന്ദേശം(curwin->w_p_script_ctx[
			    (പൂർണ്ണസംഖ്യ)options[opt_idx].indir & PV_MASK]);
		അല്ലെങ്കിൽ എങ്കിൽ ((പൂർണ്ണസംഖ്യ)options[opt_idx].indir & PV_BUF)
		    last_set_സന്ദേശം(curbuf->b_p_script_ctx[
			    (പൂർണ്ണസംഖ്യ)options[opt_idx].indir & PV_MASK]);
	    }
#endif
	}
	അല്ലെങ്കിൽ
	{
	    char_u	    *p;

	    p = find_termcode(താക്കോൽ_name);
	    എങ്കിൽ (p == ശൂന്യ)
	    {
		errmsg = e_താക്കോൽ_code_not_set;
		പോകുക skip;
	    }
	    അല്ലെങ്കിൽ
		(ശൂന്യം)show_one_termcode(താക്കോൽ_name, p, TRUE);
	}
	എങ്കിൽ (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    അല്ലെങ്കിൽ
    {
	errmsg = do_set_option_മൂല്യം(opt_idx, opt_flags, &arg, prefix, op,
					flags, varp, താക്കോൽ_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    തിരികെ errmsg;
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
    പൂർണ്ണസംഖ്യ
do_set(
    char_u	*arg_start,	// option string (may be written to!)
    പൂർണ്ണസംഖ്യ		opt_flags)
{
    char_u	*arg = arg_start;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		did_show = FALSE;   // already showed one value

    എങ്കിൽ (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	പോകുക theend;
    }

    ആകുമ്പോൾ (*arg != NUL)		// loop to process all options
    {
	എങ്കിൽ (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    എങ്കിൽ (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		set_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    അല്ലെങ്കിൽ
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ		stopopteval = FALSE;
	    അക്ഷരം	*errmsg = ശൂന്യ;
	    അക്ഷരം	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_set_option(opt_flags, &arg, arg_start, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    എങ്കിൽ (stopopteval)
		നിർത്തുക;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    വേണ്ടി (i = 0; i < 2 ; ++i)
	    {
		ആകുമ്പോൾ (*arg != NUL && !VIM_ISWHITE(*arg))
		    എങ്കിൽ (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		എങ്കിൽ (*arg != '=')
		    നിർത്തുക;
	    }

	    എങ്കിൽ (errmsg != ശൂന്യ)
	    {
		i = vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		എങ്കിൽ (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((അക്ഷരം *)IObuff);	// show error highlighted
		--no_wait_return;

		തിരികെ FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    എങ്കിൽ (silent_mode && did_show)
    {
	// After displaying option values in silent mode.
	silent_mode = FALSE;
	info_message = TRUE;	// use mch_msg(), not mch_errmsg()
	സന്ദേശം_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = TRUE;
	info_message = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    തിരികെ OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    ശൂന്യം
did_set_option(
    പൂർണ്ണസംഖ്യ	    opt_idx,
    പൂർണ്ണസംഖ്യ	    opt_flags,	    // possibly with OPT_MODELINE
    പൂർണ്ണസംഖ്യ	    new_മൂല്യം_2,	    // value was replaced completely
    പൂർണ്ണസംഖ്യ	    മൂല്യം_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_SET;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    എങ്കിൽ (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    എങ്കിൽ (!മൂല്യം_checked && (secure
#ifdef HAVE_SANDBOX
	    || sandbox != 0
#endif
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	എങ്കിൽ (flagsp_local != ശൂന്യ)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (new_മൂല്യം_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	എങ്കിൽ (flagsp_local != ശൂന്യ)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    പൂർണ്ണസംഖ്യ
string_to_താക്കോൽ(char_u *arg, പൂർണ്ണസംഖ്യ multi_byte)
{
    എങ്കിൽ (*arg == '<' && arg[1])
	തിരികെ find_താക്കോൽ_option(arg + 1, TRUE);
    എങ്കിൽ (*arg == '^' && arg[1])
    {
	പൂർണ്ണസംഖ്യ താക്കോൽ = Ctrl_chr(arg[1]);
	എങ്കിൽ (താക്കോൽ == 0)		// ^@ is <Nul>
	    താക്കോൽ = K_ZERO;
	തിരികെ താക്കോൽ;
    }
    എങ്കിൽ (multi_byte)
	തിരികെ PTR2CHAR(arg);
    തിരികെ *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    ശൂന്യം
did_set_title(ശൂന്യം)
{
    എങ്കിൽ (starting != NO_SCREEN
#ifdef FEAT_GUI
	    && !gui.starting
#endif
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    ശൂന്യം
set_options_bin(
    പൂർണ്ണസംഖ്യ		oldval,
    പൂർണ്ണസംഖ്യ		newval,
    പൂർണ്ണസംഖ്യ		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    എങ്കിൽ (newval)
    {
	എങ്കിൽ (!oldval)		// switched on
	{
	    എങ്കിൽ (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    എങ്കിൽ (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	എങ്കിൽ (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	എങ്കിൽ (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (oldval)		// switched off
    {
	എങ്കിൽ (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	എങ്കിൽ (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#എങ്കിൽ defined(FEAT_EVAL)
    // Remember where the dependent option were reset
    didset_options_sctx(opt_flags, p_bin_dep_opts);
#endif
}

/*
 * Expand environment variables for some string options.
 * These string options cannot be indirect!
 * If "val" is NULL expand the current value of the option.
 * Return pointer to NameBuff, or NULL when not expanded.
 */
    സ്ഥിതം char_u *
option_expand(പൂർണ്ണസംഖ്യ opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    എങ്കിൽ (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    എങ്കിൽ (val != ശൂന്യ && STRLEN(val) > MAXPATHL)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (val == ശൂന്യ)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    പൂർണ്ണസംഖ്യ esc = var == &p_tags || var == &p_path;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : ശൂന്യ, FALSE,
#ifdef FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#endif
				  ശൂന്യ);
    എങ്കിൽ (STRCMP(NameBuff, val) == 0)   // they are the same
	തിരികെ ശൂന്യ;

    തിരികെ NameBuff;
}

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    സ്ഥിതം ശൂന്യം
didset_options(ശൂന്യം)
{
    // initialize the table for 'iskeyword' et.al.
    (ശൂന്യം)init_chartab();

    didset_string_options();

#ifdef FEAT_SPELL
    (ശൂന്യം)spell_check_msm();
    (ശൂന്യം)spell_check_sps();
    (ശൂന്യം)compile_cap_prog(curwin->w_s);
    (ശൂന്യം)did_set_spell_option();
#endif
    // set cedit_key
    (ശൂന്യം)did_set_cedit(ശൂന്യ);
#ifdef FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_set_breakat(ശൂന്യ);
#endif
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    സ്ഥിതം ശൂന്യം
didset_options2(ശൂന്യം)
{
    // Initialize the highlight_attr[] table.
    (ശൂന്യം)highlight_changed();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (ശൂന്യം)set_listchars_option(curwin, curwin->w_p_lcs, TRUE, ശൂന്യ, 0);

    // Parse default for 'fillchars'.
    (ശൂന്യം)set_fillchars_option(curwin, curwin->w_p_fcs, TRUE, ശൂന്യ, 0);

#ifdef FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (ശൂന്യം)did_set_clipboard(ശൂന്യ);
#endif
#ifdef FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (ശൂന്യം)tabstop_set(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (ശൂന്യം)tabstop_set(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#endif
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    ശൂന്യം
check_options(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		opt_idx;

    വേണ്ടി (opt_idx = 0; options[opt_idx].fullname != ശൂന്യ; opt_idx++)
	എങ്കിൽ ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != ശൂന്യ)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    പൂർണ്ണസംഖ്യ
get_term_opt_idx(char_u **p)
{
    പൂർണ്ണസംഖ്യ opt_idx;

    വേണ്ടി (opt_idx = 1; options[opt_idx].fullname != ശൂന്യ; opt_idx++)
	എങ്കിൽ (options[opt_idx].var == (char_u *)p)
	    തിരികെ opt_idx;
    തിരികെ -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    പൂർണ്ണസംഖ്യ
set_term_option_alloced(char_u **p)
{
    പൂർണ്ണസംഖ്യ		opt_idx = get_term_opt_idx(p);

    എങ്കിൽ (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    തിരികെ opt_idx;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    പൂർണ്ണസംഖ്യ
was_set_insecurely(win_T *wp, char_u *opt, പൂർണ്ണസംഖ്യ opt_flags)
{
    പൂർണ്ണസംഖ്യ	    idx = findoption(opt);
    long_u  *flagp;

    എങ്കിൽ (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	തിരികെ (*flagp & P_INSECURE) != 0;
    }
    internal_error("was_set_insecurely()");
    തിരികെ -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    സ്ഥിതം long_u *
insecure_flag(win_T *wp, പൂർണ്ണസംഖ്യ opt_idx, പൂർണ്ണസംഖ്യ opt_flags)
{
    എങ്കിൽ (opt_flags & OPT_LOCAL)
	തിരഞ്ഞെടുപ്പ് ((പൂർണ്ണസംഖ്യ)options[opt_idx].indir)
	{
	    അവസ്ഥ PV_WRAP:	തിരികെ &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    അവസ്ഥ PV_STL:	തിരികെ &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    അവസ്ഥ PV_FDE:	തിരികെ &wp->w_p_fde_flags;
	    അവസ്ഥ PV_FDT:	തിരികെ &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    അവസ്ഥ PV_BEXPR:	തിരികെ &wp->w_buffer->b_p_bexpr_flags;
#  endif
	    അവസ്ഥ PV_INDE:	തിരികെ &wp->w_buffer->b_p_inde_flags;
	    അവസ്ഥ PV_FEX:	തിരികെ &wp->w_buffer->b_p_fex_flags;
#  ifdef FEAT_FIND_ID
	    അവസ്ഥ PV_INEX:	തിരികെ &wp->w_buffer->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    അവസ്ഥ PV_CPT:	തിരികെ &wp->w_buffer->b_p_cpt_flags;
#  endif
# endif
	}
    അല്ലെങ്കിൽ
	// For global value of window-local options, use flags in w_allbuf_opt.
	തിരഞ്ഞെടുപ്പ് ((പൂർണ്ണസംഖ്യ)options[opt_idx].indir)
	{
	    അവസ്ഥ PV_WRAP:	തിരികെ &wp->w_allbuf_opt.wo_wrap_flags;
# എങ്കിൽ defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    അവസ്ഥ PV_FDE:	തിരികെ &wp->w_allbuf_opt.wo_fde_flags;
	    അവസ്ഥ PV_FDT:	തിരികെ &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    തിരികെ &options[opt_idx].flags;
}
#endif

/*
 * Redraw the window title and/or tab page text later.
 */
    ശൂന്യം
redraw_titles(ശൂന്യം)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#എങ്കിൽ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    പൂർണ്ണസംഖ്യ
valid_name(char_u *val, അക്ഷരം *allowed)
{
    char_u *s;

    വേണ്ടി (s = val; *s != NUL; ++s)
	എങ്കിൽ (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == ശൂന്യ)
	    തിരികെ FALSE;
    തിരികെ TRUE;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    ശൂന്യം
set_option_sctx_idx(പൂർണ്ണസംഖ്യ opt_idx, പൂർണ്ണസംഖ്യ opt_flags, sctx_T script_ctx)
{
    പൂർണ്ണസംഖ്യ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    പൂർണ്ണസംഖ്യ		indir = (പൂർണ്ണസംഖ്യ)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    എങ്കിൽ (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    എങ്കിൽ (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    എങ്കിൽ (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    എങ്കിൽ (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    എങ്കിൽ (both || (opt_flags & OPT_LOCAL))
    {
	എങ്കിൽ (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	അല്ലെങ്കിൽ എങ്കിൽ (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	    എങ്കിൽ (both)
		// also setting the "all buffers" value
		curwin->w_allbuf_opt.wo_script_ctx[indir & PV_MASK] =
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
is_option_മൂല്യം_vim9(പൂർണ്ണസംഖ്യ opt_idx, പൂർണ്ണസംഖ്യ opt_flags)
{
    പൂർണ്ണസംഖ്യ		indir = (പൂർണ്ണസംഖ്യ)options[opt_idx].indir;
    sctx_T	*sctx = ശൂന്യ;

    എങ്കിൽ ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    എങ്കിൽ ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	എങ്കിൽ (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_MASK];
	അല്ലെങ്കിൽ എങ്കിൽ (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_MASK];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    തിരികെ !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_version >= SCRIPT_VERSION_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(അക്ഷരം *name)
{
    പൂർണ്ണസംഖ്യ idx = findoption((char_u *)name);

    എങ്കിൽ (idx >= 0)
	തിരികെ &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    തിരികെ ശൂന്യ;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    ശൂന്യം
set_term_option_sctx_idx(അക്ഷരം *name, പൂർണ്ണസംഖ്യ opt_idx)
{
    char_u  buf[5];
    പൂർണ്ണസംഖ്യ	    idx;

    എങ്കിൽ (name == ശൂന്യ)
	idx = opt_idx;
    അല്ലെങ്കിൽ
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    എങ്കിൽ (idx >= 0)
	set_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#endif

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    സ്ഥിതം ശൂന്യം
apply_optionset_autocmd(
	പൂർണ്ണസംഖ്യ	opt_idx,
	long	opt_flags,
	long	oldval,
	long	oldval_g,
	long	newval,
	അക്ഷരം	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    എങ്കിൽ (starting || errmsg != ശൂന്യ || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	തിരികെ;

    len = vim_snprintf_safelen((അക്ഷരം *)buf_new, വലുപ്പം(buf_new), "%ld", newval);
    set_vim_var_string(VV_OPTION_NEW, buf_new, (പൂർണ്ണസംഖ്യ)len);
    len = vim_snprintf_safelen((അക്ഷരം *)buf_type, വലുപ്പം(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (പൂർണ്ണസംഖ്യ)len);

    buf_oldlen = vim_snprintf_safelen((അക്ഷരം *)buf_old, വലുപ്പം(buf_old), "%ld", oldval);
    set_vim_var_string(VV_OPTION_OLD, buf_old, (പൂർണ്ണസംഖ്യ)buf_oldlen);

    എങ്കിൽ (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (പൂർണ്ണസംഖ്യ)buf_oldlen);
    }
    എങ്കിൽ (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (പൂർണ്ണസംഖ്യ)buf_oldlen);
    }
    എങ്കിൽ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((അക്ഷരം *)buf_old_global,
	    വലുപ്പം(buf_old_global), "%ld", oldval_g);

	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (പൂർണ്ണസംഖ്യ)buf_oldlen);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (പൂർണ്ണസംഖ്യ)buf_old_globallen);
    }
    എങ്കിൽ (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (പൂർണ്ണസംഖ്യ)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    ശൂന്യ, FALSE, ശൂന്യ);
    reset_v_option_vars();
}
#endif

#എങ്കിൽ defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    അക്ഷരം *
did_set_arabic(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	എങ്കിൽ (!p_tbidi)
	{
	    // set rightleft mode
	    എങ്കിൽ (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		changed_window_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    എങ്കിൽ (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	എങ്കിൽ (STRCMP(p_enc, "utf-8") != 0)
	{
	    സ്ഥിതം അക്ഷരം *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    സന്ദേശം_source(HL_ATTR(HLF_W));
	    സന്ദേശം_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = set_option_മൂല്യം((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    അല്ലെങ്കിൽ
    {
	// 'arabic' is reset, handle various sub-settings.
	എങ്കിൽ (!p_tbidi)
	{
	    // reset rightleft mode
	    എങ്കിൽ (curwin->w_p_rl)
	    {
		curwin->w_p_rl = FALSE;
		changed_window_setting();
	    }

	    // 'arabicshape' isn't reset, it is a global option and
	    // another window may still need it "on".
	}

	// 'delcombine' isn't reset, it is a global option and another
	// window may still want it "on".

# ifdef FEAT_KEYMAP
	// Revert to the default keymap
	curbuf->b_p_iminsert = B_IMODE_NONE;
	curbuf->b_p_imsearch = B_IMODE_USE_തിരുകുക;
# endif
    }

    തിരികെ errmsg;
}
#endif

#എങ്കിൽ defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    അക്ഷരം *
did_set_autochdir(optset_T *args UNUSED)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    അക്ഷരം *
did_set_ballooneval(optset_T *args)
{
    എങ്കിൽ (balloonEvalForTerm)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (p_beval && !args->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    അല്ലെങ്കിൽ എങ്കിൽ (!p_beval && args->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_BEVAL_TERM)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    അക്ഷരം *
did_set_balloonevalterm(optset_T *args UNUSED)
{
    mch_bevalterm_changed();
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Process the updated 'binary' option value.
 */
    അക്ഷരം *
did_set_ദ്വിമാന(optset_T *args)
{
    // when 'bin' is set also set some other options
    set_options_bin(args->os_oldval.boolean, curbuf->b_p_bin, args->os_flags);
    redraw_titles();

    തിരികെ ശൂന്യ;
}

/*
 * Process the updated 'buflisted' option value.
 */
    അക്ഷരം *
did_set_buflisted(optset_T *args)
{
    // when 'buflisted' changes, trigger autocommands
    എങ്കിൽ (args->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						ശൂന്യ, ശൂന്യ, TRUE, curbuf);
    തിരികെ ശൂന്യ;
}

/*
 * Process the new 'cmdheight' option value.
 */
    അക്ഷരം *
did_set_cmdheight(optset_T *args)
{
    long old_മൂല്യം = args->os_oldval.സംഖ്യ;
    അക്ഷരം *errmsg = ശൂന്യ;

    // if p_ch changed value, change the command line height
    എങ്കിൽ (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_argument_must_be_positive;
	p_ch = MIN_CMDHEIGHT;
    }
    എങ്കിൽ (p_ch > Rows - min_rows() + MIN_CMDHEIGHT)
	p_ch = Rows - min_rows() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    എങ്കിൽ ((p_ch != old_മൂല്യം
		|| tabline_height() + topframe->fr_height != Rows - p_ch)
	    && full_screen
#ifdef FEAT_GUI
	    && !gui.starting
#endif
       )
	command_height();

    തിരികെ errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    അക്ഷരം *
did_set_compatible(optset_T *args UNUSED)
{
    compatible_set();
    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    അക്ഷരം *
did_set_conceallevel(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (curwin->w_p_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_cole = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_cole = 3;
    }
    എങ്കിൽ (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    തിരികെ errmsg;
}
#endif

#എങ്കിൽ defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    അക്ഷരം *
did_set_diff(optset_T *args UNUSED)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    എങ്കിൽ (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    അക്ഷരം *
did_set_eof_eol_fixeol_bomb(optset_T *args UNUSED)
{
    // redraw the window title and tab page text
    redraw_titles();
    തിരികെ ശൂന്യ;
}

/*
 * Process the updated 'equalalways' option value.
 */
    അക്ഷരം *
did_set_equalalways(optset_T *args)
{
    എങ്കിൽ (p_ea && !args->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    അക്ഷരം *
did_set_foldcolumn(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (curwin->w_p_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_fdc = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_fdc = 12;
    }
    എങ്കിൽ (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    തിരികെ errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    അക്ഷരം *
did_set_foldlevel(optset_T *args UNUSED)
{
    എങ്കിൽ (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    തിരികെ ശൂന്യ;
}

/*
 * Process the new 'foldminlines' option value.
 */
    അക്ഷരം *
did_set_foldminlines(optset_T *args UNUSED)
{
    foldUpdateAll(curwin);
    തിരികെ ശൂന്യ;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    അക്ഷരം *
did_set_foldnestmax(optset_T *args UNUSED)
{
    എങ്കിൽ (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_തിരയൽ_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    അക്ഷരം *
did_set_hlsearch(optset_T *args UNUSED)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    set_no_hlsearch(FALSE);
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Process the updated 'ignorecase' option value.
 */
    അക്ഷരം *
did_set_ignorecase(optset_T *args UNUSED)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    എങ്കിൽ (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(HAVE_നിവേശം_METHOD)
/*
 * Process the updated 'imdisable' option value.
 */
    അക്ഷരം *
did_set_imdisable(optset_T *args UNUSED)
{
    // Only de-activate it here, it will be enabled when changing mode.
    എങ്കിൽ (p_imdisable)
	im_set_active(FALSE);
    അല്ലെങ്കിൽ എങ്കിൽ (State & MODE_തിരുകുക)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Process the new 'iminsert' option value.
 */
    അക്ഷരം *
did_set_iminsert(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    എങ്കിൽ (termcap_active)	// don't do this in the alternate screen
	showmode();
#എങ്കിൽ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif

    തിരികെ errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    അക്ഷരം *
did_set_imsearch(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    തിരികെ errmsg;
}

#എങ്കിൽ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    അക്ഷരം *
did_set_imstyle(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_argument;

    തിരികെ errmsg;
}
#endif

/*
 * Process the updated 'insertmode' option value.
 */
    അക്ഷരം *
did_set_insertmode(optset_T *args)
{
    // when 'insertmode' is set from an autocommand need to do work here
    എങ്കിൽ (p_im)
    {
	എങ്കിൽ ((State & MODE_തിരുകുക) == 0)
	    need_start_insertmode = TRUE;
	stop_തിരുകുക_mode = FALSE;
    }
    // only reset if it was set previously
    അല്ലെങ്കിൽ എങ്കിൽ (args->os_oldval.boolean)
    {
	need_start_insertmode = FALSE;
	stop_തിരുകുക_mode = TRUE;
	എങ്കിൽ (restart_edit != 0 && mode_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    അക്ഷരം *
did_set_langnoremap(optset_T *args UNUSED)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    തിരികെ ശൂന്യ;
}

/*
 * Process the updated 'langremap' option value.
 */
    അക്ഷരം *
did_set_langremap(optset_T *args UNUSED)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Process the new 'laststatus' option value.
 */
    അക്ഷരം *
did_set_laststatus(optset_T *args UNUSED)
{
    last_status(FALSE);	// (re)set last window status line
    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    അക്ഷരം *
did_set_linespace(optset_T *args UNUSED)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    എങ്കിൽ (gui.in_use && gui_mch_adjust_charheight() == OK)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Process the updated 'lisp' option value.
 */
    അക്ഷരം *
did_set_lisp(optset_T *args UNUSED)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (ശൂന്യം)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    തിരികെ ശൂന്യ;
}

/*
 * Process the new 'maxcombine' option value.
 */
    അക്ഷരം *
did_set_maxcombine(optset_T *args UNUSED)
{
    എങ്കിൽ (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    അല്ലെങ്കിൽ എങ്കിൽ (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    തിരികെ ശൂന്യ;
}

/*
 * Process the updated 'modifiable' option value.
 */
    അക്ഷരം *
did_set_modifiable(optset_T *args UNUSED)
{
    // when 'modifiable' is changed, redraw the window title

#ifdef FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    എങ്കിൽ (curbuf->b_p_ma && (term_in_normal_mode(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_term != ശൂന്യ && !term_is_finished(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	തിരികെ e_cannot_make_terminal_with_running_job_modifiable;
    }
#endif
    redraw_titles();

    തിരികെ ശൂന്യ;
}

/*
 * Process the updated 'modified' option value.
 */
    അക്ഷരം *
did_set_modified(optset_T *args)
{
    എങ്കിൽ (!args->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_set = !!args->os_newval.boolean;
    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    അക്ഷരം *
did_set_mousehide(optset_T *args UNUSED)
{
    എങ്കിൽ (!p_mh)
	gui_mch_mousehide(FALSE);
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    അക്ഷരം *
did_set_സംഖ്യ_relativenumber(optset_T *args UNUSED)
{
#എങ്കിൽ defined(FEAT_SIGNS) && defined(FEAT_GUI)
    എങ്കിൽ (gui.in_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != ശൂന്യ)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	എങ്കിൽ (!(curwin->w_p_nu && ((പൂർണ്ണസംഖ്യ *)args->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#endif
    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    അക്ഷരം *
did_set_numberwidth(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    // 'numberwidth' must be positive
    എങ്കിൽ (curwin->w_p_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_nuw = 1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_nuw = 20;
    }
    എങ്കിൽ (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_വരി_എണ്ണം = 0; // trigger a redraw

    തിരികെ errmsg;
}
#endif

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    അക്ഷരം *
did_set_osctimeoutlen(optset_T *args)
{
    എങ്കിൽ (p_ost < 0)
    {
	p_ost = args->os_oldval.സംഖ്യ;
	തിരികെ e_argument_must_be_positive;
    }

    തിരികെ ശൂന്യ;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    അക്ഷരം *
did_set_paste(optset_T *args UNUSED)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	old_p_paste = FALSE;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	save_sm = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	save_sta = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	save_ru = 0;
#ifdef FEAT_RIGHTLEFT
    സ്ഥിതം പൂർണ്ണസംഖ്യ	save_ri = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	save_hkmap = 0;
#endif
    buf_T	*buf;

    എങ്കിൽ (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	എങ്കിൽ (!old_p_paste)
	{
	    // save options for each buffer
	    FOR_ALL_BUFFERS(buf)
	    {
		buf->b_p_tw_nopaste = buf->b_p_tw;
		buf->b_p_wm_nopaste = buf->b_p_wm;
		buf->b_p_sts_nopaste = buf->b_p_sts;
		buf->b_p_ai_nopaste = buf->b_p_ai;
		buf->b_p_et_nopaste = buf->b_p_et;
#ifdef FEAT_VARTABS
		എങ്കിൽ (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != empty_option
					   ? vim_strsave(buf->b_p_vsts) : ശൂന്യ;
#endif
	    }

	    // save global options
	    save_sm = p_sm;
	    save_sta = p_sta;
	    save_ru = p_ru;
#ifdef FEAT_RIGHTLEFT
	    save_ri = p_ri;
	    save_hkmap = p_hkmap;
#endif
	    // save global values for local buffer options
	    p_ai_nopaste = p_ai;
	    p_et_nopaste = p_et;
	    p_sts_nopaste = p_sts;
	    p_tw_nopaste = p_tw;
	    p_wm_nopaste = p_wm;
#ifdef FEAT_VARTABS
	    എങ്കിൽ (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != empty_option
						  ? vim_strsave(p_vsts) : ശൂന്യ;
#endif
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
#ifdef FEAT_VARTABS
	    എങ്കിൽ (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = empty_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#endif
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	എങ്കിൽ (p_ru)
	    status_redraw_all();    // redraw to remove the ruler
	p_ru = 0;		    // no ruler
#ifdef FEAT_RIGHTLEFT
	p_ri = 0;		    // no reverse insert
	p_hkmap = 0;		    // no Hebrew keyboard
#endif
	// set global values for local buffer options
	p_tw = 0;
	p_wm = 0;
	p_sts = 0;
	p_ai = 0;
	p_et = 0;
#ifdef FEAT_VARTABS
	എങ്കിൽ (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = empty_option;
#endif
    }

    // Paste switched from on to off: Restore saved values.
    അല്ലെങ്കിൽ എങ്കിൽ (old_p_paste)
    {
	// restore options for each buffer
	FOR_ALL_BUFFERS(buf)
	{
	    buf->b_p_tw = buf->b_p_tw_nopaste;
	    buf->b_p_wm = buf->b_p_wm_nopaste;
	    buf->b_p_sts = buf->b_p_sts_nopaste;
	    buf->b_p_ai = buf->b_p_ai_nopaste;
	    buf->b_p_et = buf->b_p_et_nopaste;
#ifdef FEAT_VARTABS
	    എങ്കിൽ (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : empty_option;
	    vim_free(buf->b_p_vsts_array);
	    എങ്കിൽ (buf->b_p_vsts && buf->b_p_vsts != empty_option)
		(ശൂന്യം)tabstop_set(buf->b_p_vsts, &buf->b_p_vsts_array);
	    അല്ലെങ്കിൽ
		buf->b_p_vsts_array = ശൂന്യ;
#endif
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	എങ്കിൽ (p_ru != save_ru)
	    status_redraw_all();    // redraw to draw the ruler
	p_ru = save_ru;
#ifdef FEAT_RIGHTLEFT
	p_ri = save_ri;
	p_hkmap = save_hkmap;
#endif
	// set global values for local buffer options
	p_ai = p_ai_nopaste;
	p_et = p_et_nopaste;
	p_sts = p_sts_nopaste;
	p_tw = p_tw_nopaste;
	p_wm = p_wm_nopaste;
#ifdef FEAT_VARTABS
	എങ്കിൽ (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : empty_option;
#endif
    }

    old_p_paste = p_paste;

#എങ്കിൽ defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#endif

    തിരികെ ശൂന്യ;
}


#ifdef FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    അക്ഷരം *
did_set_previewwindow(optset_T *args UNUSED)
{
    എങ്കിൽ (!curwin->w_p_pvw)
	തിരികെ ശൂന്യ;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	എങ്കിൽ (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    തിരികെ e_preview_window_already_exists;
	}

    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    അക്ഷരം *
did_set_pyxversion(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_argument;

    തിരികെ errmsg;
}
#endif

/*
 * Process the updated 'readonly' option value.
 */
    അക്ഷരം *
did_set_readonly(optset_T *args)
{
    // when 'readonly' is reset globally, also reset readonlymode
    എങ്കിൽ (!curbuf->b_p_ro && (args->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    എങ്കിൽ (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    തിരികെ ശൂന്യ;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    അക്ഷരം *
did_set_scrollbind(optset_T *args UNUSED)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    എങ്കിൽ (!curwin->w_p_scb)
	തിരികെ ശൂന്യ;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    തിരികെ ശൂന്യ;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    അക്ഷരം *
did_set_maxsearchcount(optset_T *args UNUSED)
{
    അക്ഷരം	*errmsg = ശൂന്യ;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#define MAX_തിരയൽ_എണ്ണം 9999

    എങ്കിൽ (p_msc <= 0)
	errmsg = e_argument_must_be_positive;
    അല്ലെങ്കിൽ എങ്കിൽ (p_msc > MAX_തിരയൽ_എണ്ണം)
	errmsg = e_invalid_argument;

    എങ്കിൽ (errmsg != ശൂന്യ)
	p_msc = 99;

    തിരികെ errmsg;
#undef MAX_തിരയൽ_എണ്ണം
}

#എങ്കിൽ defined(BACKSLASH_IN_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    അക്ഷരം *
did_set_shellslash(optset_T *args UNUSED)
{
    എങ്കിൽ (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    അല്ലെങ്കിൽ
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
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    അക്ഷരം *
did_set_shiftwidth_tabstop(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    അക്ഷരം	*errmsg = ശൂന്യ;

    എങ്കിൽ (curbuf->b_p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_ts = 8;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_ts = 8;
    }
    എങ്കിൽ (p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ts = 8;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	p_ts = 8;
    }

    എങ്കിൽ (curbuf->b_p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_എണ്ണം(curbuf->b_p_vts_array) > 0
		       ? tabstop_first(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#അല്ലെങ്കിൽ
	curbuf->b_p_sw = curbuf->b_p_ts;
#endif
    }
    എങ്കിൽ (p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_എണ്ണം(curbuf->b_p_vts_array) > 0
	     ? tabstop_first(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#അല്ലെങ്കിൽ
	p_sw = curbuf->b_p_ts;
#endif
    }

#ifdef FEAT_FOLDING
    എങ്കിൽ (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#endif
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    എങ്കിൽ (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    തിരികെ errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    അക്ഷരം *
did_set_showtabline(optset_T *args UNUSED)
{
    shell_new_rows();	// recompute window positions and heights
    തിരികെ ശൂന്യ;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    അക്ഷരം *
did_set_smoothscroll(optset_T *args UNUSED)
{
    എങ്കിൽ (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    അക്ഷരം *
did_set_spell(optset_T *args UNUSED)
{
    എങ്കിൽ (curwin->w_p_spell)
	തിരികെ parse_spelllang(curwin);

    തിരികെ ശൂന്യ;
}
#endif

/*
 * Process the updated 'swapfile' option value.
 */
    അക്ഷരം *
did_set_swapfile(optset_T *args UNUSED)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    എങ്കിൽ (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    അല്ലെങ്കിൽ
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_TERMGUICOLORS)
    അക്ഷരം *
did_set_termguicolors(optset_T *args UNUSED)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    എങ്കിൽ (
#  ifdef VIMDLL
	    !gui.in_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_set = TRUE;
	p_tgc = 0;
	തിരികെ e_24_bit_colors_are_not_supported_on_this_environment;
    }
    എങ്കിൽ (is_term_win32())
	swap_tcap();
# endif
# ifdef FEAT_GUI
    എങ്കിൽ (!gui.in_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    എങ്കിൽ (is_term_win32())
    {
	control_console_color_rgb();
	set_termname(T_NAME);
	init_highlight(TRUE, FALSE);
    }
# endif
# ifdef FEAT_TERMINAL
    term_update_colors_all();
    term_update_palette_all();
    term_update_hlfwin_all();
# endif
    p_tgc_set = TRUE;

    തിരികെ ശൂന്യ;
}
#endif

    അക്ഷരം *
did_set_termsync(optset_T *args UNUSED)
{
    എങ്കിൽ (!p_tsy)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);
    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    അക്ഷരം *
did_set_termwinscroll(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    അക്ഷരം	*errmsg = ശൂന്യ;

    എങ്കിൽ (*pp < 1)
    {
	errmsg = e_argument_must_be_positive;
	*pp = 1;
    }

    തിരികെ errmsg;
}
#endif

/*
 * Process the updated 'terse' option value.
 */
    അക്ഷരം *
did_set_terse(optset_T *args UNUSED)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_തിരയൽ);

    // insert 's' in p_shm
    എങ്കിൽ (p_terse && p == ശൂന്യ)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    അല്ലെങ്കിൽ എങ്കിൽ (!p_terse && p != ശൂന്യ)
	STRMOVE(p, p + 1);
    തിരികെ ശൂന്യ;
}

/*
 * Process the updated 'textauto' option value.
 */
    അക്ഷരം *
did_set_textauto(optset_T *args)
{
    // when 'textauto' is set or reset also change 'fileformats'
    set_string_option_direct((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | args->os_flags, 0);

    തിരികെ ശൂന്യ;
}

/*
 * Process the updated 'textmode' option value.
 */
    അക്ഷരം *
did_set_textmode(optset_T *args)
{
    // when 'textmode' is set or reset also change 'fileformat'
    set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, args->os_flags);

    തിരികെ ശൂന്യ;
}

/*
 * Process the new 'textwidth' option value.
 */
    അക്ഷരം *
did_set_textwidth(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (curbuf->b_p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_tw = 0;
    }
    എങ്കിൽ (p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tw = 0;
    }
#ifdef FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(ശൂന്യ, wp);
    }
#endif

    തിരികെ errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    അക്ഷരം *
did_set_title_icon(optset_T *args UNUSED)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_set_title();
    തിരികെ ശൂന്യ;
}

/*
 * Process the new 'titlelen' option value.
 */
    അക്ഷരം *
did_set_titlelen(optset_T *args)
{
    long old_മൂല്യം = args->os_oldval.സംഖ്യ;
    അക്ഷരം *errmsg = ശൂന്യ;

    // if 'titlelen' has changed, redraw the title
    എങ്കിൽ (p_titlelen < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_titlelen = 85;
    }
    എങ്കിൽ (starting != NO_SCREEN && old_മൂല്യം != p_titlelen)
	need_maketitle = TRUE;

    തിരികെ errmsg;
}

#എങ്കിൽ defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    അക്ഷരം *
did_set_undofile(optset_T *args)
{
    // Only take action when the option was set.
    എങ്കിൽ (!curbuf->b_p_udf && !p_udf)
	തിരികെ ശൂന്യ;

    // When reset we do not delete the undo file, the option may be set again
    // without making any changes in between.
    char_u	hash[UNDO_HASH_SIZE];
    buf_T	*save_curbuf = curbuf;

    FOR_ALL_BUFFERS(curbuf)
    {
	// When 'undofile' is set globally: for every buffer, otherwise
	// only for the current buffer: Try to read in the undofile,
	// if one exists, the buffer wasn't changed and the buffer was
	// loaded
	എങ്കിൽ ((curbuf == save_curbuf
		    || (args->os_flags & OPT_GLOBAL)
		    || args->os_flags == 0)
		&& !curbufIsChanged() && curbuf->b_ml.ml_mfp != ശൂന്യ)
	{
# ifdef FEAT_CRYPT
	    എങ്കിൽ (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
		തുടരുക;
# endif
	    u_compute_hash(hash);
	    u_read_undo(ശൂന്യ, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    തിരികെ ശൂന്യ;
}
#endif

/*
 * Process the new global 'undolevels' option value.
 */
    സ്ഥിതം ശൂന്യം
update_global_undolevels(long മൂല്യം, long old_മൂല്യം)
{
    // sync undo before 'undolevels' changes

    // use the old value, otherwise u_sync() may not work properly
    p_ul = old_മൂല്യം;
    u_sync(TRUE);
    p_ul = മൂല്യം;
}

/*
 * Process the new buffer local 'undolevels' option value.
 */
    സ്ഥിതം ശൂന്യം
update_buflocal_undolevels(long മൂല്യം, long old_മൂല്യം)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_മൂല്യം;
    u_sync(TRUE);
    curbuf->b_p_ul = മൂല്യം;
}

/*
 * Process the new 'undolevels' option value.
 */
    അക്ഷരം *
did_set_undolevels(optset_T *args)
{
    long *pp = (long *)args->os_varp;

    എങ്കിൽ (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(args->os_newval.സംഖ്യ,
						       args->os_oldval.സംഖ്യ);
    അല്ലെങ്കിൽ എങ്കിൽ (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(args->os_newval.സംഖ്യ,
						       args->os_oldval.സംഖ്യ);

    തിരികെ ശൂന്യ;
}

/*
 * Process the new 'updatecount' option value.
 */
    അക്ഷരം *
did_set_updatecount(optset_T *args)
{
    long old_മൂല്യം = args->os_oldval.സംഖ്യ;
    അക്ഷരം *errmsg = ശൂന്യ;

    // when 'updatecount' changes from zero to non-zero, open swap files
    എങ്കിൽ (p_uc < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_uc = 100;
    }
    എങ്കിൽ (p_uc && !old_മൂല്യം)
	ml_open_files();

    തിരികെ errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    അക്ഷരം *
did_set_weirdinvert(optset_T *args)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    എങ്കിൽ (p_wiv && !args->os_oldval.boolean)
	T_XS = (char_u *)"y";
    അല്ലെങ്കിൽ എങ്കിൽ (!p_wiv && args->os_oldval.boolean)
	T_XS = empty_option;
    p_wiv = (*T_XS != NUL);

    തിരികെ ശൂന്യ;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    അക്ഷരം *
did_set_wildchar(optset_T *args)
{
    long c = *(long *)args->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    എങ്കിൽ (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

/*
 * Process the new 'window' option value.
 */
    അക്ഷരം *
did_set_window(optset_T *args UNUSED)
{
    എങ്കിൽ (p_window < 1)
	p_window = 1;
    അല്ലെങ്കിൽ എങ്കിൽ (p_window >= Rows)
	p_window = Rows - 1;
    തിരികെ ശൂന്യ;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    അക്ഷരം *
did_set_winheight_helpheight(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    അക്ഷരം	*errmsg = ശൂന്യ;

    എങ്കിൽ (p_wh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wh = 1;
    }
    എങ്കിൽ (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    എങ്കിൽ (p_hh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hh = 0;
    }

    // Change window height NOW
    എങ്കിൽ (!ONE_WINDOW)
    {
	എങ്കിൽ (pp == &p_wh && curwin->w_height < p_wh)
	    win_setheight((പൂർണ്ണസംഖ്യ)p_wh);
	എങ്കിൽ (pp == &p_hh && curbuf->b_help && curwin->w_height < p_hh)
	    win_setheight((പൂർണ്ണസംഖ്യ)p_hh);
    }

    തിരികെ errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    അക്ഷരം *
did_set_winminheight(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (p_wmh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmh = 0;
    }
    എങ്കിൽ (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    തിരികെ errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    അക്ഷരം *
did_set_winminwidth(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (p_wmw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmw = 0;
    }
    എങ്കിൽ (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    തിരികെ errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    അക്ഷരം *
did_set_winwidth(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (p_wiw < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wiw = 1;
    }
    എങ്കിൽ (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    എങ്കിൽ (!ONE_WINDOW && curwin->w_width < p_wiw)
	win_setwidth((പൂർണ്ണസംഖ്യ)p_wiw);

    തിരികെ errmsg;
}

#എങ്കിൽ defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    അക്ഷരം *
did_set_wlsteal(optset_T *args UNUSED)
{
    clip_reset_wayland();

    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    അക്ഷരം *
did_set_wltimeoutlen(optset_T *args)
{
    എങ്കിൽ (p_wtm < 0)
    {
	p_wtm = args->os_oldval.സംഖ്യ;
	തിരികെ e_argument_must_be_positive;
    }

    തിരികെ ശൂന്യ;
}
#endif

/*
 * Process the updated 'wrap' option value.
 */
    അക്ഷരം *
did_set_wrap(optset_T *args UNUSED)
{
    // Set w_leftcol or w_skipcol to zero.
    എങ്കിൽ (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    അല്ലെങ്കിൽ
	curwin->w_skipcol = 0;

    തിരികെ ശൂന്യ;
}

#ifdef FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    അക്ഷരം *
did_set_xhistory(optset_T *args)
{
    പൂർണ്ണസംഖ്യ is_p_chi = (long*)args->os_varp == &p_chi;
    പൂർണ്ണസംഖ്യ err;
    long *arg = (is_p_chi) ? &p_chi :(long*)args->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    എങ്കിൽ (*arg < 1)
    {
	*arg = args->os_oldval.സംഖ്യ;
	തിരികെ e_cannot_have_negative_or_zero_സംഖ്യ_of_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    എങ്കിൽ (*arg > 100)
    {
	*arg = args->os_oldval.സംഖ്യ;
	തിരികെ e_cannot_have_more_than_hundred_quickfix;
    }

    എങ്കിൽ (is_p_chi)
	err = qf_resize_അട്ടി(*arg);
    അല്ലെങ്കിൽ
	err = ll_resize_അട്ടി(curwin, *arg);

    എങ്കിൽ (err == FAIL)
	തിരികെ e_failed_resizing_quickfix_അട്ടി;

    തിരികെ ശൂന്യ;
}
#endif

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    സ്ഥിതം അക്ഷരം *
set_bool_option(
    പൂർണ്ണസംഖ്യ		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    പൂർണ്ണസംഖ്യ		മൂല്യം,			// new value
    പൂർണ്ണസംഖ്യ		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    പൂർണ്ണസംഖ്യ		old_മൂല്യം = *(പൂർണ്ണസംഖ്യ *)varp;
#എങ്കിൽ defined(FEAT_EVAL)
    പൂർണ്ണസംഖ്യ		old_global_മൂല്യം = 0;
#endif
    അക്ഷരം	*errmsg = ശൂന്യ;

    // Disallow changing some options from secure mode
    എങ്കിൽ ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	തിരികെ e_not_allowed_here;

#എങ്കിൽ defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    എങ്കിൽ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_മൂല്യം = *(പൂർണ്ണസംഖ്യ *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *(പൂർണ്ണസംഖ്യ *)varp = മൂല്യം;	    // set the new value
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    എങ്കിൽ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(പൂർണ്ണസംഖ്യ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = മൂല്യം;

    // Handle side effects of changing a bool option.
    എങ്കിൽ (options[opt_idx].opt_did_set_cb != ശൂന്യ)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.boolean = old_മൂല്യം;
	args.os_newval.boolean = മൂല്യം;
	args.os_errbuf = ശൂന്യ;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
	എങ്കിൽ (errmsg != ശൂന്യ)
	    തിരികെ errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_SET;

#എങ്കിൽ defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(long)(old_മൂല്യം ? TRUE : FALSE),
				(long)(old_global_മൂല്യം ? TRUE : FALSE),
				(long)(മൂല്യം ? TRUE : FALSE), ശൂന്യ);
#endif

    comp_col();			    // in case 'ruler' or 'showcmd' changed

    എങ്കിൽ (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    എങ്കിൽ ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    തിരികെ errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    സ്ഥിതം അക്ഷരം *
check_അക്കം_option_bounds(
    long	*pp,
    long	old_മൂല്യം,
    long	old_Rows,
    long	old_Columns,
    അക്ഷരം	*errbuf,
    size_t	errbuflen,
    അക്ഷരം	*errmsg)
{
    എങ്കിൽ (Rows < min_rows_for_all_tabpages() && full_screen)
    {
	എങ്കിൽ (errbuf != ശൂന്യ)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_വരികൾ), min_rows_for_all_tabpages());
	    errmsg = errbuf;
	}
	Rows = min_rows_for_all_tabpages();
    }
    എങ്കിൽ (Columns < MIN_COLUMNS && full_screen)
    {
	എങ്കിൽ (errbuf != ശൂന്യ)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_columns), MIN_COLUMNS);
	    errmsg = errbuf;
	}
	Columns = MIN_COLUMNS;
    }
    limit_screen_size();

    // If the screen (shell) height has been changed, assume it is the
    // physical screenheight.
    എങ്കിൽ (old_Rows != Rows || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	എങ്കിൽ (updating_screen)
	    *pp = old_മൂല്യം;
	അല്ലെങ്കിൽ എങ്കിൽ (full_screen
#ifdef FEAT_GUI
		&& !gui.starting
#endif
		)
	    set_shellsize((പൂർണ്ണസംഖ്യ)Columns, (പൂർണ്ണസംഖ്യ)Rows, TRUE);
	അല്ലെങ്കിൽ
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    എങ്കിൽ (cmdline_row > Rows - p_ch && Rows > p_ch)
		cmdline_row = Rows - p_ch;
	}
	എങ്കിൽ (p_window >= Rows || !option_was_set((char_u *)"window"))
	    p_window = Rows - 1;
    }

    എങ്കിൽ (p_tm < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tm = 0;
    }
    എങ്കിൽ ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_height
		    && curwin->w_height > 0))
	    && full_screen)
    {
	എങ്കിൽ (pp == &(curwin->w_p_scr))
	{
	    എങ്കിൽ (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_size;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	അല്ലെങ്കിൽ // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_height;
    }
    എങ്കിൽ (p_hi < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hi = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p_hi > 10000)
    {
	errmsg = e_invalid_argument;
	p_hi = 10000;
    }
    എങ്കിൽ (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_argument;
	p_re = 0;
    }
    എങ്കിൽ (p_report < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_report = 1;
    }
    എങ്കിൽ ((p_sj < -100 || p_sj >= Rows) && full_screen)
    {
	എങ്കിൽ (Rows != old_Rows)	// Rows changed, just adjust p_sj
	    p_sj = Rows / 2;
	അല്ലെങ്കിൽ
	{
	    errmsg = e_invalid_scroll_size;
	    p_sj = 1;
	}
    }
    എങ്കിൽ (p_so < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_so = 0;
    }
    എങ്കിൽ (p_sop < 0 && full_screen)
    {
	errmsg = e_invalid_argument;
	p_sop = 0;
    }
    എങ്കിൽ (p_siso < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_siso = 0;
    }
    എങ്കിൽ (p_cwh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_cwh = 1;
    }
    എങ്കിൽ (p_ut < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ut = 2000;
    }
    എങ്കിൽ (p_ss < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ss = 0;
    }

    തിരികെ errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    സ്ഥിതം അക്ഷരം *
set_അക്കം_option(
    പൂർണ്ണസംഖ്യ		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    long	മൂല്യം,			// new value
    അക്ഷരം	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    പൂർണ്ണസംഖ്യ		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    അക്ഷരം	*errmsg = ശൂന്യ;
    long	old_മൂല്യം = *(long *)varp;
#എങ്കിൽ defined(FEAT_EVAL)
    long	old_global_മൂല്യം = 0;	// only used when setting a local and
					// global option
#endif
    long	old_Rows = Rows;	// remember old Rows
    long	old_Columns = Columns;	// remember old Columns
    long	*pp = (long *)varp;

    // Disallow changing some options from secure mode.
    എങ്കിൽ ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	തിരികെ e_not_allowed_here;

#എങ്കിൽ defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    എങ്കിൽ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_മൂല്യം = *(long *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *pp = മൂല്യം;
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    എങ്കിൽ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = മൂല്യം;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    എങ്കിൽ (options[opt_idx].opt_did_set_cb != ശൂന്യ)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.സംഖ്യ = old_മൂല്യം;
	args.os_newval.സംഖ്യ = മൂല്യം;
	args.os_errbuf = ശൂന്യ;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
    }

    // Check the bounds for numeric options here
    errmsg = check_അക്കം_option_bounds(pp, old_മൂല്യം, old_Rows, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_SET;

#എങ്കിൽ defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_മൂല്യം, old_global_മൂല്യം,
								മൂല്യം, errmsg);
#endif

    comp_col();			    // in case 'columns' or 'ls' changed

    എങ്കിൽ (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    എങ്കിൽ ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    തിരികെ errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    ശൂന്യം
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    പൂർണ്ണസംഖ്യ		doclear = (flags & P_RCLR) == P_RCLR;
    പൂർണ്ണസംഖ്യ		all = ((flags & P_RALL) == P_RALL || doclear);

    എങ്കിൽ ((flags & P_RSTAT) || all)	// mark all status lines dirty
	status_redraw_all();

    എങ്കിൽ ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	എങ്കിൽ (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	അല്ലെങ്കിൽ
	    changed_window_setting();
    }
    എങ്കിൽ (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    എങ്കിൽ (doclear)
	redraw_all_later(UPD_CLEAR);
    അല്ലെങ്കിൽ എങ്കിൽ (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    പൂർണ്ണസംഖ്യ
findoption(char_u *arg)
{
    പൂർണ്ണസംഖ്യ		    opt_idx;
    അക്ഷരം	    *s, *p;
    സ്ഥിതം short    quick_tab[27] = {0, 0};	// quick access table
    പൂർണ്ണസംഖ്യ		    is_term_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    എങ്കിൽ (quick_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	പൂർണ്ണസംഖ്യ last_opt_idx;
	വേണ്ടി (last_opt_idx = 0; options[last_opt_idx].fullname != ശൂന്യ;
		last_opt_idx++)
	    ;
	വേണ്ടി (പൂർണ്ണസംഖ്യ tab_idx = 1; tab_idx < 27; tab_idx++)
	    quick_tab[tab_idx] = last_opt_idx;

	p = options[0].fullname;
	വേണ്ടി (opt_idx = 1; opt_idx < last_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    എങ്കിൽ (s[0] != p[0])
	    {
		എങ്കിൽ (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		അല്ലെങ്കിൽ
		    quick_tab[Char_Ord_താഴെയുള്ള(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    എങ്കിൽ (arg[0] < 'a' || arg[0] > 'z')
	തിരികെ -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    എങ്കിൽ (is_term_opt)
	opt_idx = quick_tab[26];
    അല്ലെങ്കിൽ
	opt_idx = quick_tab[Char_Ord_താഴെയുള്ള(arg[0])];
    വേണ്ടി (; (s = options[opt_idx].fullname) != ശൂന്യ && s[0] == arg[0]; opt_idx++)
    {
	എങ്കിൽ (STRCMP(arg, s) == 0)		    // match full name
	    നിർത്തുക;
    }
    എങ്കിൽ (s != ശൂന്യ && s[0] != arg[0])
	s = ശൂന്യ;
    എങ്കിൽ (s == ശൂന്യ && !is_term_opt)
    {
	opt_idx = quick_tab[Char_Ord_താഴെയുള്ള(arg[0])];
	വേണ്ടി ( ; options[opt_idx].fullname != ശൂന്യ; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    എങ്കിൽ (s != ശൂന്യ && STRCMP(arg, s) == 0)   // match short name
		നിർത്തുക;
	    s = ശൂന്യ;
	}
    }
    എങ്കിൽ (s == ശൂന്യ)
	opt_idx = -1;
    തിരികെ opt_idx;
}

#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
get_option_മൂല്യം(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    പൂർണ്ണസംഖ്യ		*flagsp,
    പൂർണ്ണസംഖ്യ		scope)
{
    പൂർണ്ണസംഖ്യ		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    എങ്കിൽ (opt_idx < 0)		    // option not in the table
    {
	പൂർണ്ണസംഖ്യ താക്കോൽ;

	എങ്കിൽ (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (താക്കോൽ = find_താക്കോൽ_option(name, FALSE)) != 0)
	{
	    char_u താക്കോൽ_name[2];
	    char_u *p;

	    എങ്കിൽ (flagsp != ശൂന്യ)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    എങ്കിൽ (താക്കോൽ < 0)
	    {
		താക്കോൽ_name[0] = KEY2TERMCAP0(താക്കോൽ);
		താക്കോൽ_name[1] = KEY2TERMCAP1(താക്കോൽ);
	    }
	    അല്ലെങ്കിൽ
	    {
		താക്കോൽ_name[0] = KS_താക്കോൽ;
		താക്കോൽ_name[1] = (താക്കോൽ & 0xff);
	    }
	    p = find_termcode(താക്കോൽ_name);
	    എങ്കിൽ (p != ശൂന്യ)
	    {
		എങ്കിൽ (stringval != ശൂന്യ)
		    *stringval = vim_strsave(p);
		തിരികെ gov_string;
	    }
	}
	തിരികെ gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    എങ്കിൽ (flagsp != ശൂന്യ)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    എങ്കിൽ (options[opt_idx].flags & P_STRING)
    {
	എങ്കിൽ (varp == ശൂന്യ)		    // hidden option
	    തിരികെ gov_hidden_string;
	എങ്കിൽ (stringval != ശൂന്യ)
	{
	    എങ്കിൽ ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    അല്ലെങ്കിൽ എങ്കിൽ ((char_u **)varp == &curbuf->b_p_താക്കോൽ
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    അല്ലെങ്കിൽ
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	തിരികെ gov_string;
    }

    എങ്കിൽ (varp == ശൂന്യ)		    // hidden option
	തിരികെ (options[opt_idx].flags & P_അക്കം)
					 ? gov_hidden_സംഖ്യ : gov_hidden_bool;
    എങ്കിൽ (options[opt_idx].flags & P_അക്കം)
	*numval = *(long *)varp;
    അല്ലെങ്കിൽ
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	എങ്കിൽ ((പൂർണ്ണസംഖ്യ *)varp == &curbuf->b_changed)
	    *numval = curbufIsChanged();
	അല്ലെങ്കിൽ
	    *numval = (long) *(പൂർണ്ണസംഖ്യ *)varp;
    }
    തിരികെ (options[opt_idx].flags & P_അക്കം) ? gov_സംഖ്യ : gov_bool;
}
#endif

#എങ്കിൽ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    പൂർണ്ണസംഖ്യ
get_option_മൂല്യം_strict(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    പൂർണ്ണസംഖ്യ		opt_type,
    ശൂന്യം	*from)
{
    പൂർണ്ണസംഖ്യ		opt_idx;
    char_u	*varp = ശൂന്യ;
    ഘടന vimoption *p;
    പൂർണ്ണസംഖ്യ		r = 0;

    opt_idx = findoption(name);
    എങ്കിൽ (opt_idx < 0)
	തിരികെ 0;

    p = &(options[opt_idx]);

    // Hidden option
    എങ്കിൽ (p->var == ശൂന്യ)
	തിരികെ 0;

    എങ്കിൽ (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    അല്ലെങ്കിൽ എങ്കിൽ (p->flags & P_അക്കം)
	r |= SOPT_അക്കം;
    അല്ലെങ്കിൽ എങ്കിൽ (p->flags & P_STRING)
	r |= SOPT_STRING;

    എങ്കിൽ (p->indir == PV_NONE)
    {
	എങ്കിൽ (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	അല്ലെങ്കിൽ
	    തിരികെ 0; // Did not request global-only option
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	അല്ലെങ്കിൽ എങ്കിൽ (opt_type == SREQ_GLOBAL)
	    തിരികെ 0; // Requested global option

	എങ്കിൽ (p->indir & PV_WIN)
	{
	    എങ്കിൽ (opt_type == SREQ_BUF)
		തിരികെ 0; // Did not request window-local option
	    അല്ലെങ്കിൽ
		r |= SOPT_WIN;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (p->indir & PV_BUF)
	{
	    എങ്കിൽ (opt_type == SREQ_WIN)
		തിരികെ 0; // Did not request buffer-local option
	    അല്ലെങ്കിൽ
		r |= SOPT_BUF;
	}
    }

    എങ്കിൽ (stringval == ശൂന്യ)
	തിരികെ r;

    എങ്കിൽ (opt_type == SREQ_GLOBAL)
	varp = p->var;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    എങ്കിൽ (p->indir == PV_MOD)
	    {
		*numval = bufIsChanged((buf_T *)from);
		varp = ശൂന്യ;
	    }
# ifdef FEAT_CRYPT
	    അല്ലെങ്കിൽ എങ്കിൽ (p->indir == PV_താക്കോൽ)
	    {
		// never return the value of the crypt key
		*stringval = ശൂന്യ;
		varp = ശൂന്യ;
	    }
# endif
	    അല്ലെങ്കിൽ
	    {
		buf_T *save_curbuf = curbuf;

		// only getting a pointer, no need to use aucmd_prepbuf()
		curbuf = (buf_T *)from;
		curwin->w_buffer = curbuf;
		varp = get_varp(p);
		curbuf = save_curbuf;
		curwin->w_buffer = curbuf;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_buffer;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	}
	എങ്കിൽ (varp == p->var)
	    തിരികെ (r | SOPT_UNSET);
    }

    എങ്കിൽ (varp != ശൂന്യ)
    {
	എങ്കിൽ (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	അല്ലെങ്കിൽ എങ്കിൽ (p->flags & P_അക്കം)
	    *numval = *(long *) varp;
	അല്ലെങ്കിൽ
	    *numval = *(പൂർണ്ണസംഖ്യ *)varp;
    }

    തിരികെ r;
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
option_iter_next(ശൂന്യം **option, പൂർണ്ണസംഖ്യ opt_type)
{
    ഘടന vimoption	*ret = ശൂന്യ;
    ചെയ്യുക
    {
	എങ്കിൽ (*option == ശൂന്യ)
	    *option = (ശൂന്യം *) options;
	അല്ലെങ്കിൽ എങ്കിൽ (((ഘടന vimoption *) (*option))->fullname == ശൂന്യ)
	{
	    *option = ശൂന്യ;
	    തിരികെ ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	    *option = (ശൂന്യം *) (((ഘടന vimoption *) (*option)) + 1);

	ret = ((ഘടന vimoption *) (*option));

	// Hidden option
	എങ്കിൽ (ret->var == ശൂന്യ)
	{
	    ret = ശൂന്യ;
	    തുടരുക;
	}

	തിരഞ്ഞെടുപ്പ് (opt_type)
	{
	    അവസ്ഥ SREQ_GLOBAL:
		എങ്കിൽ (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = ശൂന്യ;
		നിർത്തുക;
	    അവസ്ഥ SREQ_BUF:
		എങ്കിൽ (!(ret->indir & PV_BUF))
		    ret = ശൂന്യ;
		നിർത്തുക;
	    അവസ്ഥ SREQ_WIN:
		എങ്കിൽ (!(ret->indir & PV_WIN))
		    ret = ശൂന്യ;
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		internal_error("option_iter_next()");
		തിരികെ ശൂന്യ;
	}
    }
    ആകുമ്പോൾ (ret == ശൂന്യ);

    തിരികെ (char_u *)ret->fullname;
}
#endif

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(പൂർണ്ണസംഖ്യ opt_idx)
{
    തിരികെ options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    ശൂന്യം
set_option_flag(പൂർണ്ണസംഖ്യ opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    പൂർണ്ണസംഖ്യ
is_global_option(പൂർണ്ണസംഖ്യ opt_idx)
{
    തിരികെ options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    പൂർണ്ണസംഖ്യ
is_global_local_option(പൂർണ്ണസംഖ്യ opt_idx)
{
    തിരികെ options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    പൂർണ്ണസംഖ്യ
is_window_local_option(പൂർണ്ണസംഖ്യ opt_idx)
{
    തിരികെ options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    പൂർണ്ണസംഖ്യ
is_hidden_option(പൂർണ്ണസംഖ്യ opt_idx)
{
    തിരികെ options[opt_idx].var == ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    പൂർണ്ണസംഖ്യ
is_crypt_താക്കോൽ_option(പൂർണ്ണസംഖ്യ opt_idx)
{
    തിരികെ options[opt_idx].indir == PV_താക്കോൽ;
}
#endif

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    അക്ഷരം *
set_option_മൂല്യം(
    char_u	*name,
    long	സംഖ്യ,
    char_u	*string,
    പൂർണ്ണസംഖ്യ		opt_flags)	// OPT_LOCAL or 0 (both)
{
    പൂർണ്ണസംഖ്യ		opt_idx;
    char_u	*varp;
    long_u	flags;
    സ്ഥിതം അക്ഷരം	errbuf[ERR_BUFLEN];
    പൂർണ്ണസംഖ്യ		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    എങ്കിൽ (opt_idx < 0)
    {
	പൂർണ്ണസംഖ്യ താക്കോൽ;

	എങ്കിൽ (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (താക്കോൽ = find_താക്കോൽ_option(name, FALSE)) != 0)
	{
	    char_u താക്കോൽ_name[2];

	    എങ്കിൽ (താക്കോൽ < 0)
	    {
		താക്കോൽ_name[0] = KEY2TERMCAP0(താക്കോൽ);
		താക്കോൽ_name[1] = KEY2TERMCAP1(താക്കോൽ);
	    }
	    അല്ലെങ്കിൽ
	    {
		താക്കോൽ_name[0] = KS_താക്കോൽ;
		താക്കോൽ_name[1] = (താക്കോൽ & 0xff);
	    }
	    add_termcode(താക്കോൽ_name, string, FALSE);
	    എങ്കിൽ (full_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    തിരികെ ശൂന്യ;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    അല്ലെങ്കിൽ
    {
	flags = options[opt_idx].flags;
#ifdef HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	എങ്കിൽ (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_in_sandbox));
	    തിരികെ ശൂന്യ;
	}
#endif
	എങ്കിൽ (flags & P_STRING)
	    തിരികെ set_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	എങ്കിൽ (varp != ശൂന്യ)	// hidden option is not changed
	{
	    എങ്കിൽ (സംഖ്യ == 0 && string != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ idx;

		// Either we are given a string or we are setting option
		// to zero.
		വേണ്ടി (idx = 0; string[idx] == '0'; ++idx)
		    ;
		എങ്കിൽ (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_സംഖ്യ_required_after_str_equal_str),
								 name, string);
		    തിരികെ ശൂന്യ;     // do nothing as we hit an error

		}
	    }
	    എങ്കിൽ (flags & P_അക്കം)
	    {
		തിരികെ set_അക്കം_option(opt_idx, varp, സംഖ്യ,
					   errbuf, വലുപ്പം(errbuf), opt_flags);
	    }
	    അല്ലെങ്കിൽ
		തിരികെ set_bool_option(opt_idx, varp, (പൂർണ്ണസംഖ്യ)സംഖ്യ, opt_flags);
	}

    }
    തിരികെ ശൂന്യ;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    ശൂന്യം
set_option_മൂല്യം_give_err(
    char_u	*name,
    long	സംഖ്യ,
    char_u	*string,
    പൂർണ്ണസംഖ്യ		opt_flags)	// OPT_LOCAL or 0 (both)
{
    അക്ഷരം *errmsg = set_option_മൂല്യം(name, സംഖ്യ, string, opt_flags);

    എങ്കിൽ (errmsg != ശൂന്യ)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_term_code(char_u *tname)
{
    പൂർണ്ണസംഖ്യ	    opt_idx;
    char_u  *varp;

    എങ്കിൽ (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	തിരികെ ശൂന്യ;
    എങ്കിൽ ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	എങ്കിൽ (varp != ശൂന്യ)
	    varp = *(char_u **)(varp);
	തിരികെ varp;
    }
    തിരികെ find_termcode(tname + 2);
}

    char_u *
get_highlight_default(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;

    i = findoption((char_u *)"hl");
    എങ്കിൽ (i >= 0)
	തിരികെ options[i].def_val[VI_DEFAULT];
    തിരികെ (char_u *)ശൂന്യ;
}

    char_u *
get_encoding_default(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;

    i = findoption((char_u *)"enc");
    എങ്കിൽ (i >= 0)
	തിരികെ options[i].def_val[VI_DEFAULT];
    തിരികെ (char_u *)ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_QUICKFIX)
    പൂർണ്ണസംഖ്യ
is_option_allocated(അക്ഷരം *name)
{
    പൂർണ്ണസംഖ്യ idx = findoption((char_u *)name);

    തിരികെ idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#endif

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_താക്കോൽ_option(char_u *arg_arg, പൂർണ്ണസംഖ്യ has_lt)
{
    പൂർണ്ണസംഖ്യ		താക്കോൽ = 0;
    പൂർണ്ണസംഖ്യ		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    എങ്കിൽ (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	എങ്കിൽ (!has_lt || arg[4] == '>')
	    താക്കോൽ = TERMCAP2_താക്കോൽ(arg[2], arg[3]);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	താക്കോൽ = find_special_താക്കോൽ(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_താക്കോൽ | FSK_SIMPLIFY, ശൂന്യ);
	എങ്കിൽ (modifiers)		    // can't handle modifiers here
	    താക്കോൽ = 0;
    }
    തിരികെ താക്കോൽ;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    സ്ഥിതം ശൂന്യം
showoptions(
    പൂർണ്ണസംഖ്യ		all,
    പൂർണ്ണസംഖ്യ		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    ഘടന vimoption	*p;
    പൂർണ്ണസംഖ്യ			col;
    പൂർണ്ണസംഖ്യ			isterm;
    char_u		*varp;
    ഘടന vimoption	**ഇനങ്ങൾ;
    പൂർണ്ണസംഖ്യ			item_എണ്ണം;
    പൂർണ്ണസംഖ്യ			run;
    പൂർണ്ണസംഖ്യ			row, rows;
    പൂർണ്ണസംഖ്യ			cols;
    പൂർണ്ണസംഖ്യ			i;
    പൂർണ്ണസംഖ്യ			len;

#define INC 20
#define GAP 3

    ഇനങ്ങൾ = ALLOC_MULT(ഘടന vimoption *, OPTION_എണ്ണം);
    എങ്കിൽ (ഇനങ്ങൾ == ശൂന്യ)
	തിരികെ;

    // Highlight title
    എങ്കിൽ (all == 2)
	സന്ദേശം_puts_title(_("\n--- Terminal codes ---"));
    അല്ലെങ്കിൽ എങ്കിൽ (opt_flags & OPT_GLOBAL)
	സന്ദേശം_puts_title(_("\n--- Global option values ---"));
    അല്ലെങ്കിൽ എങ്കിൽ (opt_flags & OPT_LOCAL)
	സന്ദേശം_puts_title(_("\n--- Local option values ---"));
    അല്ലെങ്കിൽ
	സന്ദേശം_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    വേണ്ടി (run = 1; run <= 2 && !got_int; ++run)
    {
	// collect the items in items[]
	item_എണ്ണം = 0;
	വേണ്ടി (p = &options[0]; p->fullname != ശൂന്യ; p++)
	{
	    // apply :filter /pat/
	    എങ്കിൽ (message_filtered((char_u *)p->fullname))
		തുടരുക;

	    varp = ശൂന്യ;
	    isterm = istermoption(p);
	    എങ്കിൽ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		എങ്കിൽ (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    അല്ലെങ്കിൽ
		varp = get_varp(p);
	    എങ്കിൽ (varp != ശൂന്യ
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		എങ്കിൽ (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		അല്ലെങ്കിൽ എങ്കിൽ (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		അല്ലെങ്കിൽ
		{
		    option_value2string(p, opt_flags);
		    len = (പൂർണ്ണസംഖ്യ)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		എങ്കിൽ ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    ഇനങ്ങൾ[item_എണ്ണം++] = p;
	    }
	}

	// display the items
	എങ്കിൽ (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    എങ്കിൽ (cols == 0)
		cols = 1;
	    rows = (item_എണ്ണം + cols - 1) / cols;
	}
	അല്ലെങ്കിൽ	// run == 2
	    rows = item_എണ്ണം;
	വേണ്ടി (row = 0; row < rows && !got_int; ++row)
	{
	    സന്ദേശം_putchar('\n');			// go to next line
	    എങ്കിൽ (got_int)			// 'q' typed in more
		നിർത്തുക;
	    col = 0;
	    വേണ്ടി (i = row; i < item_എണ്ണം; i += rows)
	    {
		സന്ദേശം_col = col;			// make columns
		showoneopt(ഇനങ്ങൾ[i], opt_flags);
		col += INC;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(ഇനങ്ങൾ);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
optval_default(ഘടന vimoption *p, char_u *varp, പൂർണ്ണസംഖ്യ compatible)
{
    പൂർണ്ണസംഖ്യ		dvi;

    എങ്കിൽ (varp == ശൂന്യ)
	തിരികെ TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    എങ്കിൽ (p->flags & P_അക്കം)
	തിരികെ (*(long *)varp == (long)(long_i)p->def_val[dvi]);
    എങ്കിൽ (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	തിരികെ (*(പൂർണ്ണസംഖ്യ *)varp == (പൂർണ്ണസംഖ്യ)(long)(long_i)p->def_val[dvi]);
    // P_STRING
    തിരികെ (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    സ്ഥിതം ശൂന്യം
showoneopt(
    ഘടന vimoption	*p,
    പൂർണ്ണസംഖ്യ			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    പൂർണ്ണസംഖ്യ		save_silent = silent_mode;

    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    എങ്കിൽ ((p->flags & P_BOOL) && ((പൂർണ്ണസംഖ്യ *)varp == &curbuf->b_changed
					? !curbufIsChanged() : !*(പൂർണ്ണസംഖ്യ *)varp))
	സന്ദേശം_puts("no");
    അല്ലെങ്കിൽ എങ്കിൽ ((p->flags & P_BOOL) && *(പൂർണ്ണസംഖ്യ *)varp < 0)
	സന്ദേശം_puts("--");
    അല്ലെങ്കിൽ
	സന്ദേശം_puts("  ");
    സന്ദേശം_puts(p->fullname);
    എങ്കിൽ (!(p->flags & P_BOOL))
    {
	സന്ദേശം_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	സന്ദേശം_outtrans(NameBuff);
    }

    silent_mode = save_silent;
    info_message = FALSE;
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
    പൂർണ്ണസംഖ്യ
makeset(FILE *fd, പൂർണ്ണസംഖ്യ opt_flags, പൂർണ്ണസംഖ്യ local_only)
{
    ഘടന vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = ശൂന്യ;	// fresh value
    അക്ഷരം		*cmd;
    പൂർണ്ണസംഖ്യ			round;
    പൂർണ്ണസംഖ്യ			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    വേണ്ടി (pri = 1; pri >= 0; --pri)
    {
      വേണ്ടി (p = &options[0]; !istermoption(p); p++)
	എങ്കിൽ (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    എങ്കിൽ (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		തുടരുക;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    എങ്കിൽ ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		തുടരുക;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    എങ്കിൽ ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		തുടരുക;

	    എങ്കിൽ ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		തുടരുക;

	    round = 2;
	    എങ്കിൽ (p->indir != PV_NONE)
	    {
		എങ്കിൽ (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    എങ്കിൽ (!(opt_flags & OPT_LOCAL))
			തുടരുക;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    എങ്കിൽ (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			എങ്കിൽ (!optval_default(p, varp_fresh, p_cp))
			{
			    round = 1;
			    varp_local = varp;
			    varp = varp_fresh;
			}
		    }
		}
	    }

	    // Round 1: fresh value for window-local options.
	    // Round 2: other values
	    വേണ്ടി ( ; round <= 2; varp = varp_local, ++round)
	    {
		എങ്കിൽ (round == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		അല്ലെങ്കിൽ
		    cmd = "setlocal";

		എങ്കിൽ (p->flags & P_BOOL)
		{
		    എങ്കിൽ (put_setbool(fd, cmd, p->fullname, *(പൂർണ്ണസംഖ്യ *)varp) == FAIL)
			തിരികെ FAIL;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (p->flags & P_അക്കം)
		{
		    എങ്കിൽ (put_setnum(fd, cmd, p->fullname, (long *)varp) == FAIL)
			തിരികെ FAIL;
		}
		അല്ലെങ്കിൽ    // P_STRING
		{
		    പൂർണ്ണസംഖ്യ		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    എങ്കിൽ ((p->flags & P_FUNC) && *(char_u **)varp != ശൂന്യ
			    && strstr(*(അക്ഷരം **)varp, "<lambda>") != ശൂന്യ)
			തുടരുക;

#ifdef FEAT_EVAL
		    legacy = !is_option_മൂല്യം_vim9(p - &options[0],
			    round == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#അല്ലെങ്കിൽ
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#endif

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    എങ്കിൽ (
#എങ്കിൽ defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#endif
			    p->indir == PV_FT)
		    {
			എങ്കിൽ (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    തിരികെ FAIL;
			do_endif = TRUE;
		    }
		    എങ്കിൽ (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			തിരികെ FAIL;
		    എങ്കിൽ (do_endif)
		    {
			എങ്കിൽ (put_വരി(fd, "endif") == FAIL)
			    തിരികെ FAIL;
		    }
		}
	    }
	}
    }
    തിരികെ OK;
}

#എങ്കിൽ defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    പൂർണ്ണസംഖ്യ
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_version < SCRIPT_VERSION_VIM9;
# endif

    എങ്കിൽ (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	തിരികെ FAIL;

    തിരികെ OK;
}
#endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
put_setstring(
    FILE	*fd,
    bool	legacy,
    അക്ഷരം	*cmd,
    അക്ഷരം	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = ശൂന്യ;
    char_u	*part = ശൂന്യ;
    char_u	*p;

    എങ്കിൽ (legacy && fprintf(fd, "legacy ") < 0)
	തിരികെ FAIL;
    എങ്കിൽ (fprintf(fd, "%s %s=", cmd, name) < 0)
	തിരികെ FAIL;
    എങ്കിൽ (*valuep != ശൂന്യ)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	എങ്കിൽ (valuep == &p_pt)
	{
	    s = *valuep;
	    ആകുമ്പോൾ (*s != NUL)
		എങ്കിൽ (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    തിരികെ FAIL;
	}
	// expand the option value, replace $HOME by ~
	അല്ലെങ്കിൽ എങ്കിൽ ((flags & P_EXPAND) != 0)
	{
	    പൂർണ്ണസംഖ്യ  size = (പൂർണ്ണസംഖ്യ)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(size);
	    എങ്കിൽ (buf == ശൂന്യ)
		പോകുക fail;
	    home_replace(ശൂന്യ, *valuep, buf, size, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    എങ്കിൽ (size >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != ശൂന്യ)
	    {
		part = alloc(size);
		എങ്കിൽ (part == ശൂന്യ)
		    പോകുക fail;

		// write line break to clear the option, e.g. ':set rtp='
		എങ്കിൽ (put_eol(fd) == FAIL)
		    പോകുക fail;

		p = buf;
		ആകുമ്പോൾ (*p != NUL)
		{
		    എങ്കിൽ (legacy && fprintf(fd, "legacy ") < 0)
			തിരികെ FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    എങ്കിൽ (fprintf(fd, "%s %s+=", cmd, name) < 0)
			പോകുക fail;
		    (ശൂന്യം)copy_option_part(&p, part, size,  ",");
		    എങ്കിൽ (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			പോകുക fail;
		}
		vim_free(buf);
		vim_free(part);
		തിരികെ OK;
	    }
	    എങ്കിൽ (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		തിരികെ FAIL;
	    }
	    vim_free(buf);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (put_escstr(fd, *valuep, 2) == FAIL)
	    തിരികെ FAIL;
    }
    എങ്കിൽ (put_eol(fd) < 0)
	തിരികെ FAIL;
    തിരികെ OK;
fail:
    vim_free(buf);
    vim_free(part);
    തിരികെ FAIL;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
put_setnum(
    FILE	*fd,
    അക്ഷരം	*cmd,
    അക്ഷരം	*name,
    long	*valuep)
{
    long	wc;

    എങ്കിൽ (fprintf(fd, "%s %s=", cmd, name) < 0)
	തിരികെ FAIL;
    എങ്കിൽ (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	എങ്കിൽ (fputs((അക്ഷരം *)get_special_താക്കോൽ_name((പൂർണ്ണസംഖ്യ)wc, 0), fd) < 0)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (fprintf(fd, "%ld", *valuep) < 0)
	തിരികെ FAIL;
    എങ്കിൽ (put_eol(fd) < 0)
	തിരികെ FAIL;
    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
put_setbool(
    FILE	*fd,
    അക്ഷരം	*cmd,
    അക്ഷരം	*name,
    പൂർണ്ണസംഖ്യ		മൂല്യം)
{
    എങ്കിൽ (മൂല്യം < 0)	// global/local option using global value
	തിരികെ OK;
    എങ്കിൽ (fprintf(fd, "%s %s%s", cmd, മൂല്യം ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    ശൂന്യം
clear_termoptions(ശൂന്യം)
{
    // Reset a few things before clearing the old options. This may cause
    // outputting a few things that the terminal doesn't understand, but the
    // screen will be cleared later, so this is OK.
    mch_setmouse(FALSE);	    // switch mouse off
    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window titles
#എങ്കിൽ defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    എങ്കിൽ (gui.starting)
	clear_xterm_clip();
#endif
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    ശൂന്യം
free_termoptions(ശൂന്യം)
{
    ഘടന vimoption   *p;

    വേണ്ടി (p = options; p->fullname != ശൂന്യ; p++)
	എങ്കിൽ (istermoption(p))
	{
	    എങ്കിൽ (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    എങ്കിൽ (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = empty_option;
	    p->def_val[VI_DEFAULT] = empty_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ifdef FEAT_EVAL
	    // remember where the option was cleared
	    set_option_sctx_idx((പൂർണ്ണസംഖ്യ)(p - options), OPT_GLOBAL, current_sctx);
#endif
	}
    clear_termcodes();
}

/*
 * Free the string for one term option, if it was allocated.
 * Set the string to empty_option and clear allocated flag.
 * "var" points to the option value.
 */
    ശൂന്യം
free_one_termoption(char_u *var)
{
    ഘടന vimoption   *p;

    വേണ്ടി (p = &options[0]; p->fullname != ശൂന്യ; p++)
	എങ്കിൽ (p->var == var)
	{
	    എങ്കിൽ (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = empty_option;
	    p->flags &= ~P_ALLOCED;
	    നിർത്തുക;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    ശൂന്യം
set_term_defaults(ശൂന്യം)
{
    ഘടന vimoption   *p;

    വേണ്ടി (p = &options[0]; p->fullname != ശൂന്യ; p++)
    {
	എങ്കിൽ (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    എങ്കിൽ (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    എങ്കിൽ (p->flags & P_ALLOCED)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
istermoption(ഘടന vimoption *p)
{
    തിരികെ (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
istermoption_idx(പൂർണ്ണസംഖ്യ opt_idx)
{
    തിരികെ istermoption(&options[opt_idx]);
}

#എങ്കിൽ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    ശൂന്യം
unset_global_local_option(char_u *name, ശൂന്യം *from)
{
    ഘടന vimoption *p;
    പൂർണ്ണസംഖ്യ		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    എങ്കിൽ (opt_idx < 0)
	തിരികെ;
    p = &(options[opt_idx]);

    തിരഞ്ഞെടുപ്പ് ((പൂർണ്ണസംഖ്യ)p->indir)
    {
	// global option with local value: use local value if it's been set
	അവസ്ഥ PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    നിർത്തുക;
	അവസ്ഥ PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    നിർത്തുക;
	അവസ്ഥ PV_PATH:
	    clear_string_option(&buf->b_p_path);
	    നിർത്തുക;
	അവസ്ഥ PV_AC:
	    buf->b_p_ac = -1;
	    നിർത്തുക;
	അവസ്ഥ PV_AR:
	    buf->b_p_ar = -1;
	    നിർത്തുക;
# ifdef HAVE_FSYNC
	അവസ്ഥ PV_FS:
	    buf->b_p_fs = -1;
	    നിർത്തുക;
# endif
	അവസ്ഥ PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    നിർത്തുക;
	അവസ്ഥ PV_TAGS:
	    clear_string_option(&buf->b_p_tags);
	    നിർത്തുക;
	അവസ്ഥ PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    നിർത്തുക;
	അവസ്ഥ PV_SISO:
	    curwin->w_p_siso = -1;
	    നിർത്തുക;
	അവസ്ഥ PV_SO:
	    curwin->w_p_so = -1;
	    നിർത്തുക;
	അവസ്ഥ PV_SOP:
	    curwin->w_p_sop = -1;
	    നിർത്തുക;
# ifdef FEAT_FIND_ID
	അവസ്ഥ PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    നിർത്തുക;
	അവസ്ഥ PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    നിർത്തുക;
# endif
	അവസ്ഥ PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    നിർത്തുക;
	അവസ്ഥ PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    നിർത്തുക;
# ifdef FEAT_DIFF
	അവസ്ഥ PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    നിർത്തുക;
# endif
	അവസ്ഥ PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    നിർത്തുക;
# ifdef FEAT_COMPL_FUNC
	അവസ്ഥ PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    നിർത്തുക;
# endif
	അവസ്ഥ PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    നിർത്തുക;
# ifdef FEAT_EVAL
	അവസ്ഥ PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    നിർത്തുക;
# endif
# ifdef FEAT_QUICKFIX
	അവസ്ഥ PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    നിർത്തുക;
	അവസ്ഥ PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    നിർത്തുക;
	അവസ്ഥ PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    നിർത്തുക;
	അവസ്ഥ PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    നിർത്തുക;
# endif
# എങ്കിൽ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	അവസ്ഥ PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    നിർത്തുക;
# endif
# എങ്കിൽ defined(FEAT_CRYPT)
	അവസ്ഥ PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    നിർത്തുക;
# endif
# ifdef FEAT_LINEBREAK
	അവസ്ഥ PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    നിർത്തുക;
# endif
# ifdef FEAT_STL_OPT
	അവസ്ഥ PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    നിർത്തുക;
	അവസ്ഥ PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    നിർത്തുക;
# endif
	അവസ്ഥ PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    നിർത്തുക;
	അവസ്ഥ PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    നിർത്തുക;
	അവസ്ഥ PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    നിർത്തുക;
	അവസ്ഥ PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    set_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      ശൂന്യ, 0);
	    redraw_later(UPD_NOT_VALID);
	    നിർത്തുക;
	അവസ്ഥ PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    set_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      ശൂന്യ, 0);
	    redraw_later(UPD_NOT_VALID);
	    നിർത്തുക;
	അവസ്ഥ PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    നിർത്തുക;
    }
}
#endif

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    സ്ഥിതം char_u *
get_varp_scope(ഘടന vimoption *p, പൂർണ്ണസംഖ്യ scope)
{
    എങ്കിൽ ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	എങ്കിൽ (p->var == VAR_WIN)
	    തിരികെ (char_u *)GLOBAL_WO(get_varp(p));
	തിരികെ p->var;
    }
    എങ്കിൽ ((scope & OPT_LOCAL) && ((പൂർണ്ണസംഖ്യ)p->indir & PV_BOTH))
    {
	തിരഞ്ഞെടുപ്പ് ((പൂർണ്ണസംഖ്യ)p->indir)
	{
	    അവസ്ഥ PV_FP:   തിരികെ (char_u *)&(curbuf->b_p_fp);
#ifdef HAVE_FSYNC
	    അവസ്ഥ PV_FS:	തിരികെ (char_u *)&(curbuf->b_p_fs);
#endif
#ifdef FEAT_EVAL
	    അവസ്ഥ PV_FFU: തിരികെ (char_u *)&(curbuf->b_p_ffu);
#endif
#ifdef FEAT_QUICKFIX
	    അവസ്ഥ PV_EFM:  തിരികെ (char_u *)&(curbuf->b_p_efm);
	    അവസ്ഥ PV_GEFM: തിരികെ (char_u *)&(curbuf->b_p_gefm);
	    അവസ്ഥ PV_GP:   തിരികെ (char_u *)&(curbuf->b_p_gp);
	    അവസ്ഥ PV_MP:   തിരികെ (char_u *)&(curbuf->b_p_mp);
#endif
	    അവസ്ഥ PV_EP:   തിരികെ (char_u *)&(curbuf->b_p_ep);
	    അവസ്ഥ PV_KP:   തിരികെ (char_u *)&(curbuf->b_p_kp);
	    അവസ്ഥ PV_PATH: തിരികെ (char_u *)&(curbuf->b_p_path);
	    അവസ്ഥ PV_AC:   തിരികെ (char_u *)&(curbuf->b_p_ac);
	    അവസ്ഥ PV_AR:   തിരികെ (char_u *)&(curbuf->b_p_ar);
	    അവസ്ഥ PV_TAGS: തിരികെ (char_u *)&(curbuf->b_p_tags);
	    അവസ്ഥ PV_TC:   തിരികെ (char_u *)&(curbuf->b_p_tc);
	    അവസ്ഥ PV_SISO: തിരികെ (char_u *)&(curwin->w_p_siso);
	    അവസ്ഥ PV_SO:   തിരികെ (char_u *)&(curwin->w_p_so);
	    അവസ്ഥ PV_SOP:  തിരികെ (char_u *)&(curwin->w_p_sop);
#ifdef FEAT_FIND_ID
	    അവസ്ഥ PV_DEF:  തിരികെ (char_u *)&(curbuf->b_p_def);
	    അവസ്ഥ PV_INC:  തിരികെ (char_u *)&(curbuf->b_p_inc);
#endif
	    അവസ്ഥ PV_COT:  തിരികെ (char_u *)&(curbuf->b_p_cot);
	    അവസ്ഥ PV_DICT: തിരികെ (char_u *)&(curbuf->b_p_dict);
#ifdef FEAT_DIFF
	    അവസ്ഥ PV_DIA:  തിരികെ (char_u *)&(curbuf->b_p_dia);
#endif
	    അവസ്ഥ PV_TSR:  തിരികെ (char_u *)&(curbuf->b_p_tsr);
#ifdef FEAT_COMPL_FUNC
	    അവസ്ഥ PV_TSRFU: തിരികെ (char_u *)&(curbuf->b_p_tsrfu);
#endif
#എങ്കിൽ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    അവസ്ഥ PV_BEXPR: തിരികെ (char_u *)&(curbuf->b_p_bexpr);
#endif
#എങ്കിൽ defined(FEAT_CRYPT)
	    അവസ്ഥ PV_CM:	  തിരികെ (char_u *)&(curbuf->b_p_cm);
#endif
#ifdef FEAT_LINEBREAK
	    അവസ്ഥ PV_SBR:  തിരികെ (char_u *)&(curwin->w_p_sbr);
#endif
#ifdef FEAT_STL_OPT
	    അവസ്ഥ PV_STL:  തിരികെ (char_u *)&(curwin->w_p_stl);
	    അവസ്ഥ PV_STLO: തിരികെ (char_u *)&(curwin->w_p_stlo);
#endif
	    അവസ്ഥ PV_UL:   തിരികെ (char_u *)&(curbuf->b_p_ul);
	    അവസ്ഥ PV_LW:   തിരികെ (char_u *)&(curbuf->b_p_lw);
	    അവസ്ഥ PV_BKC:  തിരികെ (char_u *)&(curbuf->b_p_bkc);
	    അവസ്ഥ PV_MENC: തിരികെ (char_u *)&(curbuf->b_p_menc);
	    അവസ്ഥ PV_LCS:  തിരികെ (char_u *)&(curwin->w_p_lcs);
	    അവസ്ഥ PV_FCS:  തിരികെ (char_u *)&(curwin->w_p_fcs);
	    അവസ്ഥ PV_VE:	  തിരികെ (char_u *)&(curwin->w_p_ve);

	}
	തിരികെ ശൂന്യ; // "cannot happen"
    }
    തിരികെ get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(പൂർണ്ണസംഖ്യ opt_idx, പൂർണ്ണസംഖ്യ scope)
{
    തിരികെ get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    സ്ഥിതം char_u *
get_varp(ഘടന vimoption *p)
{
    // hidden option, always return NULL
    എങ്കിൽ (p->var == ശൂന്യ)
	തിരികെ ശൂന്യ;

    തിരഞ്ഞെടുപ്പ് ((പൂർണ്ണസംഖ്യ)p->indir)
    {
	അവസ്ഥ PV_NONE:	തിരികെ p->var;

	// global option with local value: use local value if it's been set
	അവസ്ഥ PV_EP:	തിരികെ *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	അവസ്ഥ PV_KP:	തിരികെ *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	അവസ്ഥ PV_PATH:	തിരികെ *curbuf->b_p_path != NUL
				    ? (char_u *)&(curbuf->b_p_path) : p->var;
	അവസ്ഥ PV_AC:	തിരികെ curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	അവസ്ഥ PV_AR:	തിരികെ curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	അവസ്ഥ PV_TAGS:	തിരികെ *curbuf->b_p_tags != NUL
				    ? (char_u *)&(curbuf->b_p_tags) : p->var;
	അവസ്ഥ PV_TC:	തിരികെ *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	അവസ്ഥ PV_BKC:	തിരികെ *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	അവസ്ഥ PV_SISO:	തിരികെ curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	അവസ്ഥ PV_SO:	തിരികെ curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	അവസ്ഥ PV_SOP:	തിരികെ curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ifdef FEAT_FIND_ID
	അവസ്ഥ PV_DEF:	തിരികെ *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	അവസ്ഥ PV_INC:	തിരികെ *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#endif
	അവസ്ഥ PV_COT:	തിരികെ *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	അവസ്ഥ PV_DICT:	തിരികെ *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ifdef FEAT_DIFF
	അവസ്ഥ PV_DIA:	തിരികെ *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#endif
	അവസ്ഥ PV_TSR:	തിരികെ *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ifdef FEAT_COMPL_FUNC
	അവസ്ഥ PV_TSRFU:	തിരികെ *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#endif
	അവസ്ഥ PV_FP:	തിരികെ *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ifdef HAVE_FSYNC
	അവസ്ഥ PV_FS:	തിരികെ curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#endif
#ifdef FEAT_EVAL
	അവസ്ഥ PV_FFU:	തിരികെ *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#endif
#ifdef FEAT_QUICKFIX
	അവസ്ഥ PV_EFM:	തിരികെ *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	അവസ്ഥ PV_GEFM:	തിരികെ *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	അവസ്ഥ PV_GP:	തിരികെ *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	അവസ്ഥ PV_MP:	തിരികെ *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#endif
#എങ്കിൽ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	അവസ്ഥ PV_BEXPR:	തിരികെ *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#endif
#എങ്കിൽ defined(FEAT_CRYPT)
	അവസ്ഥ PV_CM:	തിരികെ *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#endif
#ifdef FEAT_LINEBREAK
	അവസ്ഥ PV_SBR:	തിരികെ *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#endif
#ifdef FEAT_STL_OPT
	അവസ്ഥ PV_STL:	തിരികെ *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	അവസ്ഥ PV_STLO:	തിരികെ *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#endif
	അവസ്ഥ PV_UL:	തിരികെ curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	അവസ്ഥ PV_LW:	തിരികെ *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	അവസ്ഥ PV_MENC:	തിരികെ *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ifdef FEAT_ARABIC
	അവസ്ഥ PV_ARAB:	തിരികെ (char_u *)&(curwin->w_p_arab);
#endif
	അവസ്ഥ PV_LIST:	തിരികെ (char_u *)&(curwin->w_p_list);
	അവസ്ഥ PV_LCS:	തിരികെ *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	അവസ്ഥ PV_FCS:	തിരികെ *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	അവസ്ഥ PV_VE:	തിരികെ *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ifdef FEAT_SPELL
	അവസ്ഥ PV_SPELL:	തിരികെ (char_u *)&(curwin->w_p_spell);
#endif
#ifdef FEAT_SYN_HL
	അവസ്ഥ PV_CUC:	തിരികെ (char_u *)&(curwin->w_p_cuc);
	അവസ്ഥ PV_CUL:	തിരികെ (char_u *)&(curwin->w_p_cul);
	അവസ്ഥ PV_CULOPT:	തിരികെ (char_u *)&(curwin->w_p_culopt);
	അവസ്ഥ PV_CC:	തിരികെ (char_u *)&(curwin->w_p_cc);
#endif
#ifdef FEAT_DIFF
	അവസ്ഥ PV_DIFF:	തിരികെ (char_u *)&(curwin->w_p_diff);
#endif
	അവസ്ഥ PV_EIW:	തിരികെ (char_u *)&(curwin->w_p_eiw);
#ifdef FEAT_FOLDING
	അവസ്ഥ PV_FDC:	തിരികെ (char_u *)&(curwin->w_p_fdc);
	അവസ്ഥ PV_FEN:	തിരികെ (char_u *)&(curwin->w_p_fen);
	അവസ്ഥ PV_FDI:	തിരികെ (char_u *)&(curwin->w_p_fdi);
	അവസ്ഥ PV_FDL:	തിരികെ (char_u *)&(curwin->w_p_fdl);
	അവസ്ഥ PV_FDM:	തിരികെ (char_u *)&(curwin->w_p_fdm);
	അവസ്ഥ PV_FML:	തിരികെ (char_u *)&(curwin->w_p_fml);
	അവസ്ഥ PV_FDN:	തിരികെ (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	അവസ്ഥ PV_FDE:	തിരികെ (char_u *)&(curwin->w_p_fde);
	അവസ്ഥ PV_FDT:	തിരികെ (char_u *)&(curwin->w_p_fdt);
# endif
	അവസ്ഥ PV_FMR:	തിരികെ (char_u *)&(curwin->w_p_fmr);
#endif
	അവസ്ഥ PV_NU:	തിരികെ (char_u *)&(curwin->w_p_nu);
	അവസ്ഥ PV_RNU:	തിരികെ (char_u *)&(curwin->w_p_rnu);
#ifdef FEAT_LINEBREAK
	അവസ്ഥ PV_NUW:	തിരികെ (char_u *)&(curwin->w_p_nuw);
#endif
	അവസ്ഥ PV_WFB:	തിരികെ (char_u *)&(curwin->w_p_wfb);
	അവസ്ഥ PV_WFH:	തിരികെ (char_u *)&(curwin->w_p_wfh);
	അവസ്ഥ PV_WFW:	തിരികെ (char_u *)&(curwin->w_p_wfw);
#എങ്കിൽ defined(FEAT_QUICKFIX)
	അവസ്ഥ PV_PVW:	തിരികെ (char_u *)&(curwin->w_p_pvw);
	അവസ്ഥ PV_LHI:	തിരികെ (char_u *)&(curwin->w_p_lhi);
#endif
#ifdef FEAT_RIGHTLEFT
	അവസ്ഥ PV_RL:	തിരികെ (char_u *)&(curwin->w_p_rl);
	അവസ്ഥ PV_RLC:	തിരികെ (char_u *)&(curwin->w_p_rlc);
#endif
	അവസ്ഥ PV_SCROLL:	തിരികെ (char_u *)&(curwin->w_p_scr);
	അവസ്ഥ PV_SMS:	തിരികെ (char_u *)&(curwin->w_p_sms);
	അവസ്ഥ PV_WRAP:	തിരികെ (char_u *)&(curwin->w_p_wrap);
#ifdef FEAT_LINEBREAK
	അവസ്ഥ PV_LBR:	തിരികെ (char_u *)&(curwin->w_p_lbr);
	അവസ്ഥ PV_BRI:	തിരികെ (char_u *)&(curwin->w_p_bri);
	അവസ്ഥ PV_BRIOPT: തിരികെ (char_u *)&(curwin->w_p_briopt);
#endif
	അവസ്ഥ PV_WCR:	തിരികെ (char_u *)&(curwin->w_p_wcr);
	അവസ്ഥ PV_SCBIND: തിരികെ (char_u *)&(curwin->w_p_scb);
	അവസ്ഥ PV_CRBIND: തിരികെ (char_u *)&(curwin->w_p_crb);
#ifdef FEAT_CONCEAL
	അവസ്ഥ PV_COCU:   തിരികെ (char_u *)&(curwin->w_p_cocu);
	അവസ്ഥ PV_COLE:   തിരികെ (char_u *)&(curwin->w_p_cole);
#endif
	അവസ്ഥ PV_WHL:	തിരികെ (char_u *)&(curwin->w_p_whl);

#ifdef FEAT_TERMINAL
	അവസ്ഥ PV_TWK:    തിരികെ (char_u *)&(curwin->w_p_twk);
	അവസ്ഥ PV_TWS:    തിരികെ (char_u *)&(curwin->w_p_tws);
	അവസ്ഥ PV_TWSL:	തിരികെ (char_u *)&(curbuf->b_p_twsl);
#endif

	അവസ്ഥ PV_AI:	തിരികെ (char_u *)&(curbuf->b_p_ai);
	അവസ്ഥ PV_BIN:	തിരികെ (char_u *)&(curbuf->b_p_bin);
	അവസ്ഥ PV_BOMB:	തിരികെ (char_u *)&(curbuf->b_p_bomb);
	അവസ്ഥ PV_BH:	തിരികെ (char_u *)&(curbuf->b_p_bh);
	അവസ്ഥ PV_BT:	തിരികെ (char_u *)&(curbuf->b_p_bt);
	അവസ്ഥ PV_BL:	തിരികെ (char_u *)&(curbuf->b_p_bl);
	അവസ്ഥ PV_CI:	തിരികെ (char_u *)&(curbuf->b_p_ci);
	അവസ്ഥ PV_CIN:	തിരികെ (char_u *)&(curbuf->b_p_cin);
	അവസ്ഥ PV_CINK:	തിരികെ (char_u *)&(curbuf->b_p_cink);
	അവസ്ഥ PV_CINO:	തിരികെ (char_u *)&(curbuf->b_p_cino);
	അവസ്ഥ PV_CINSD:	തിരികെ (char_u *)&(curbuf->b_p_cinsd);
	അവസ്ഥ PV_CINW:	തിരികെ (char_u *)&(curbuf->b_p_cinw);
	അവസ്ഥ PV_COM:	തിരികെ (char_u *)&(curbuf->b_p_com);
	അവസ്ഥ PV_CMS:	തിരികെ (char_u *)&(curbuf->b_p_cms);
	അവസ്ഥ PV_CPT:	തിരികെ (char_u *)&(curbuf->b_p_cpt);
#ifdef BACKSLASH_IN_FILENAME
	അവസ്ഥ PV_CSL:	തിരികെ (char_u *)&(curbuf->b_p_csl);
#endif
#ifdef FEAT_COMPL_FUNC
	അവസ്ഥ PV_CFU:	തിരികെ (char_u *)&(curbuf->b_p_cfu);
	അവസ്ഥ PV_OFU:	തിരികെ (char_u *)&(curbuf->b_p_ofu);
#endif
#ifdef FEAT_EVAL
	അവസ്ഥ PV_TFU:	തിരികെ (char_u *)&(curbuf->b_p_tfu);
#endif
	അവസ്ഥ PV_EOF:	തിരികെ (char_u *)&(curbuf->b_p_eof);
	അവസ്ഥ PV_EOL:	തിരികെ (char_u *)&(curbuf->b_p_eol);
	അവസ്ഥ PV_FIXEOL:	തിരികെ (char_u *)&(curbuf->b_p_fixeol);
	അവസ്ഥ PV_ET:	തിരികെ (char_u *)&(curbuf->b_p_et);
	അവസ്ഥ PV_FENC:	തിരികെ (char_u *)&(curbuf->b_p_fenc);
	അവസ്ഥ PV_FF:	തിരികെ (char_u *)&(curbuf->b_p_ff);
	അവസ്ഥ PV_FT:	തിരികെ (char_u *)&(curbuf->b_p_ft);
	അവസ്ഥ PV_FO:	തിരികെ (char_u *)&(curbuf->b_p_fo);
	അവസ്ഥ PV_FLP:	തിരികെ (char_u *)&(curbuf->b_p_flp);
	അവസ്ഥ PV_IMI:	തിരികെ (char_u *)&(curbuf->b_p_iminsert);
	അവസ്ഥ PV_IMS:	തിരികെ (char_u *)&(curbuf->b_p_imsearch);
	അവസ്ഥ PV_INF:	തിരികെ (char_u *)&(curbuf->b_p_inf);
	അവസ്ഥ PV_ISK:	തിരികെ (char_u *)&(curbuf->b_p_isk);
#ifdef FEAT_FIND_ID
# ifdef FEAT_EVAL
	അവസ്ഥ PV_INEX:	തിരികെ (char_u *)&(curbuf->b_p_inex);
# endif
#endif
#എങ്കിൽ defined(FEAT_EVAL)
	അവസ്ഥ PV_INDE:	തിരികെ (char_u *)&(curbuf->b_p_inde);
	അവസ്ഥ PV_INDK:	തിരികെ (char_u *)&(curbuf->b_p_indk);
#endif
#ifdef FEAT_EVAL
	അവസ്ഥ PV_FEX:	തിരികെ (char_u *)&(curbuf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
	അവസ്ഥ PV_താക്കോൽ:	തിരികെ (char_u *)&(curbuf->b_p_താക്കോൽ);
#endif
	അവസ്ഥ PV_LISP:	തിരികെ (char_u *)&(curbuf->b_p_lisp);
	അവസ്ഥ PV_LOP:	തിരികെ (char_u *)&(curbuf->b_p_lop);
	അവസ്ഥ PV_ML:	തിരികെ (char_u *)&(curbuf->b_p_ml);
	അവസ്ഥ PV_MPS:	തിരികെ (char_u *)&(curbuf->b_p_mps);
	അവസ്ഥ PV_MA:	തിരികെ (char_u *)&(curbuf->b_p_ma);
	അവസ്ഥ PV_MOD:	തിരികെ (char_u *)&(curbuf->b_changed);
	അവസ്ഥ PV_NF:	തിരികെ (char_u *)&(curbuf->b_p_nf);
	അവസ്ഥ PV_PI:	തിരികെ (char_u *)&(curbuf->b_p_pi);
	അവസ്ഥ PV_QE:	തിരികെ (char_u *)&(curbuf->b_p_qe);
	അവസ്ഥ PV_RO:	തിരികെ (char_u *)&(curbuf->b_p_ro);
	അവസ്ഥ PV_SI:	തിരികെ (char_u *)&(curbuf->b_p_si);
	അവസ്ഥ PV_SN:	തിരികെ (char_u *)&(curbuf->b_p_sn);
	അവസ്ഥ PV_STS:	തിരികെ (char_u *)&(curbuf->b_p_sts);
	അവസ്ഥ PV_SUA:	തിരികെ (char_u *)&(curbuf->b_p_sua);
	അവസ്ഥ PV_SWF:	തിരികെ (char_u *)&(curbuf->b_p_swf);
#ifdef FEAT_SYN_HL
	അവസ്ഥ PV_SMC:	തിരികെ (char_u *)&(curbuf->b_p_smc);
	അവസ്ഥ PV_SYN:	തിരികെ (char_u *)&(curbuf->b_p_syn);
#endif
#ifdef FEAT_SPELL
	അവസ്ഥ PV_SPC:	തിരികെ (char_u *)&(curwin->w_s->b_p_spc);
	അവസ്ഥ PV_SPF:	തിരികെ (char_u *)&(curwin->w_s->b_p_spf);
	അവസ്ഥ PV_SPL:	തിരികെ (char_u *)&(curwin->w_s->b_p_spl);
	അവസ്ഥ PV_SPO:	തിരികെ (char_u *)&(curwin->w_s->b_p_spo);
#endif
	അവസ്ഥ PV_SW:	തിരികെ (char_u *)&(curbuf->b_p_sw);
	അവസ്ഥ PV_TS:	തിരികെ (char_u *)&(curbuf->b_p_ts);
	അവസ്ഥ PV_TW:	തിരികെ (char_u *)&(curbuf->b_p_tw);
	അവസ്ഥ PV_TX:	തിരികെ (char_u *)&(curbuf->b_p_tx);
#ifdef FEAT_PERSISTENT_UNDO
	അവസ്ഥ PV_UDF:	തിരികെ (char_u *)&(curbuf->b_p_udf);
#endif
	അവസ്ഥ PV_WM:	തിരികെ (char_u *)&(curbuf->b_p_wm);
#ifdef FEAT_KEYMAP
	അവസ്ഥ PV_KMAP:	തിരികെ (char_u *)&(curbuf->b_p_keymap);
#endif
#ifdef FEAT_SIGNS
	അവസ്ഥ PV_SCL:	തിരികെ (char_u *)&(curwin->w_p_scl);
#endif
#ifdef FEAT_VARTABS
	അവസ്ഥ PV_VSTS:	തിരികെ (char_u *)&(curbuf->b_p_vsts);
	അവസ്ഥ PV_VTS:	തിരികെ (char_u *)&(curbuf->b_p_vts);
#endif
	സ്ഥിരസ്ഥിതി:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    തിരികെ (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(പൂർണ്ണസംഖ്യ opt_idx)
{
    തിരികെ options[opt_idx].var;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(പൂർണ്ണസംഖ്യ opt_idx)
{
    തിരികെ (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    പൂർണ്ണസംഖ്യ
option_info(char_u *name, dict_T *d)
{
    പൂർണ്ണസംഖ്യ			opt_idx;
    ഘടന vimoption	*p;
    പൂർണ്ണസംഖ്യ			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    എങ്കിൽ ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    എങ്കിൽ (opt_idx < 0)
	തിരികെ FAIL;

    p = &options[opt_idx];
    dict_add_string(d, "name", (char_u *)p->fullname);
    dict_add_string(d, "shortname",
		    (char_u *)(p->shortname == ശൂന്യ ? "" : p->shortname));
    dict_add_bool(d, "available", p->var != ശൂന്യ);
    dict_add_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_അക്കം) ? "number" : "string"));
    dict_add_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    എങ്കിൽ (p->flags & P_BOOL)
	dict_add_bool(d, "default", (പൂർണ്ണസംഖ്യ)(long_i)p->def_val[dvi]);
    അല്ലെങ്കിൽ എങ്കിൽ (p->flags & P_അക്കം)
	dict_add_സംഖ്യ(d, "default", (long)(long_i)p->def_val[dvi]);
    അല്ലെങ്കിൽ
	dict_add_string(d, "default", p->def_val[dvi] == ശൂന്യ
					? (char_u *)"" : p->def_val[dvi]);
    തിരികെ OK;
}
#endif

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_set_cb_T
get_option_did_set_cb(പൂർണ്ണസംഖ്യ opt_idx)
{
    തിരികെ options[opt_idx].opt_did_set_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(ശൂന്യം)
{
    എങ്കിൽ (*curbuf->b_p_ep == NUL)
	തിരികെ p_ep;
    തിരികെ curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(ശൂന്യം)
{
#ifdef FEAT_EVAL
    എങ്കിൽ (*curbuf->b_p_ffu == NUL)
	തിരികെ p_ffu;
    തിരികെ curbuf->b_p_ffu;
#അല്ലെങ്കിൽ
    തിരികെ (char_u *)"";
#endif
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    ശൂന്യം
win_copy_options(win_T *wp_from, win_T *wp_to)
{
    copy_winopt(&wp_from->w_onebuf_opt, &wp_to->w_onebuf_opt);
    copy_winopt(&wp_from->w_allbuf_opt, &wp_to->w_allbuf_opt);
#ifdef FEAT_STL_OPT
    // w_stl_rendered_height is in win_T directly, not in winvar_T, so it is
    // not copied by copy_winopt().  Copy it explicitly here.
    wp_to->w_stl_rendered_height = wp_from->w_stl_rendered_height;
#endif
    after_copy_winopt(wp_to);
}

/*
 * After copying window options: update variables depending on options.
 */
    ശൂന്യം
after_copy_winopt(win_T *wp)
{
    അക്ഷരം *errmsg = update_winhighlight(wp, wp->w_p_whl);

    എങ്കിൽ (errmsg != ശൂന്യ)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    എങ്കിൽ (wp->w_p_wrap)
	wp->w_leftcol = 0;
    അല്ലെങ്കിൽ
	wp->w_skipcol = 0;
#ifdef FEAT_LINEBREAK
    briopt_check(ശൂന്യ, wp);
#endif
#ifdef FEAT_SYN_HL
    fill_culopt_flags(ശൂന്യ, wp);
    check_colorcolumn(ശൂന്യ, wp);
#endif
    set_listchars_option(wp, wp->w_p_lcs, TRUE, ശൂന്യ, 0);
    set_fillchars_option(wp, wp->w_p_fcs, TRUE, ശൂന്യ, 0);
}

    സ്ഥിതം char_u *
copy_option_val(char_u *val)
{
    എങ്കിൽ (val == empty_option)
	തിരികെ empty_option;  // no need to allocate memory
    തിരികെ vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    ശൂന്യം
copy_winopt(winopt_T *from, winopt_T *to)
{
#ifdef FEAT_ARABIC
    to->wo_arab = from->wo_arab;
#endif
    to->wo_list = from->wo_list;
    to->wo_lcs = copy_option_val(from->wo_lcs);
    to->wo_fcs = copy_option_val(from->wo_fcs);
    to->wo_nu = from->wo_nu;
    to->wo_rnu = from->wo_rnu;
    to->wo_ve = copy_option_val(from->wo_ve);
    to->wo_ve_flags = from->wo_ve_flags;
#ifdef FEAT_LINEBREAK
    to->wo_nuw = from->wo_nuw;
#endif
#ifdef FEAT_RIGHTLEFT
    to->wo_rl  = from->wo_rl;
    to->wo_rlc = copy_option_val(from->wo_rlc);
#endif
#ifdef FEAT_LINEBREAK
    to->wo_sbr = copy_option_val(from->wo_sbr);
#endif
#ifdef FEAT_STL_OPT
    to->wo_stl = copy_option_val(from->wo_stl);
    to->wo_stlo = copy_option_val(from->wo_stlo);
    to->wo_stlo_fh = from->wo_stlo_fh;
    to->wo_stlo_mh = from->wo_stlo_mh;
#endif
    to->wo_wrap = from->wo_wrap;
#ifdef FEAT_DIFF
    to->wo_wrap_save = from->wo_wrap_save;
#endif
#ifdef FEAT_LINEBREAK
    to->wo_lbr = from->wo_lbr;
    to->wo_bri = from->wo_bri;
    to->wo_briopt = copy_option_val(from->wo_briopt);
#endif
    to->wo_wcr = copy_option_val(from->wo_wcr);
    to->wo_scb = from->wo_scb;
    to->wo_scb_save = from->wo_scb_save;
    to->wo_sms = from->wo_sms;
    to->wo_crb = from->wo_crb;
    to->wo_crb_save = from->wo_crb_save;
    to->wo_siso = from->wo_siso;
    to->wo_so = from->wo_so;
    to->wo_sop = from->wo_sop;
#ifdef FEAT_SPELL
    to->wo_spell = from->wo_spell;
#endif
#ifdef FEAT_SYN_HL
    to->wo_cuc = from->wo_cuc;
    to->wo_cul = from->wo_cul;
    to->wo_culopt = copy_option_val(from->wo_culopt);
    to->wo_cc = copy_option_val(from->wo_cc);
#endif
#ifdef FEAT_DIFF
    to->wo_diff = from->wo_diff;
    to->wo_diff_saved = from->wo_diff_saved;
#endif
    to->wo_eiw = copy_option_val(from->wo_eiw);
#ifdef FEAT_CONCEAL
    to->wo_cocu = copy_option_val(from->wo_cocu);
    to->wo_cole = from->wo_cole;
#endif
#ifdef FEAT_TERMINAL
    to->wo_twk = copy_option_val(from->wo_twk);
    to->wo_tws = copy_option_val(from->wo_tws);
#endif
#ifdef FEAT_FOLDING
    to->wo_fdc = from->wo_fdc;
    to->wo_fdc_save = from->wo_fdc_save;
    to->wo_fen = from->wo_fen;
    to->wo_fen_save = from->wo_fen_save;
    to->wo_fdi = copy_option_val(from->wo_fdi);
    to->wo_fml = from->wo_fml;
    to->wo_fdl = from->wo_fdl;
    to->wo_fdl_save = from->wo_fdl_save;
    to->wo_fdm = copy_option_val(from->wo_fdm);
    to->wo_fdm_save = from->wo_diff_saved
			       ? vim_strsave(from->wo_fdm_save) : empty_option;
    to->wo_fdn = from->wo_fdn;
# ifdef FEAT_EVAL
    to->wo_fde = copy_option_val(from->wo_fde);
    to->wo_fdt = copy_option_val(from->wo_fdt);
# endif
    to->wo_fmr = copy_option_val(from->wo_fmr);
#endif
#ifdef FEAT_SIGNS
    to->wo_scl = copy_option_val(from->wo_scl);
#endif
#ifdef FEAT_QUICKFIX
    to->wo_lhi = from->wo_lhi;
#endif

    to->wo_wrap_flags = from->wo_wrap_flags;
#ifdef FEAT_STL_OPT
    to->wo_stl_flags = from->wo_stl_flags;
#endif
#ifdef FEAT_EVAL
    to->wo_fde_flags = from->wo_fde_flags;
    to->wo_fdt_flags = from->wo_fdt_flags;
#endif
    to->wo_whl = copy_option_val(from->wo_whl);

#ifdef FEAT_EVAL
    // Copy the script context so that we know where the value was last set.
    mch_memmove(to->wo_script_ctx, from->wo_script_ctx,
						    വലുപ്പം(to->wo_script_ctx));
#endif
    check_winopt(to);		// don't want NULL pointers
}

/*
 * Check string options in a window for a NULL value.
 */
    സ്ഥിതം ശൂന്യം
check_win_options(win_T *win)
{
    check_winopt(&win->w_onebuf_opt);
    check_winopt(&win->w_allbuf_opt);
}

/*
 * Check for NULL pointers in a winopt_T and replace them with empty_option.
 */
    സ്ഥിതം ശൂന്യം
check_winopt(winopt_T *wop UNUSED)
{
#ifdef FEAT_FOLDING
    check_string_option(&wop->wo_fdi);
    check_string_option(&wop->wo_fdm);
    check_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    check_string_option(&wop->wo_fde);
    check_string_option(&wop->wo_fdt);
# endif
    check_string_option(&wop->wo_fmr);
#endif
    check_string_option(&wop->wo_eiw);
#ifdef FEAT_SIGNS
    check_string_option(&wop->wo_scl);
#endif
#ifdef FEAT_RIGHTLEFT
    check_string_option(&wop->wo_rlc);
#endif
#ifdef FEAT_LINEBREAK
    check_string_option(&wop->wo_sbr);
#endif
#ifdef FEAT_STL_OPT
    check_string_option(&wop->wo_stl);
    check_string_option(&wop->wo_stlo);
#endif
#ifdef FEAT_SYN_HL
    check_string_option(&wop->wo_culopt);
    check_string_option(&wop->wo_cc);
#endif
#ifdef FEAT_CONCEAL
    check_string_option(&wop->wo_cocu);
#endif
#ifdef FEAT_TERMINAL
    check_string_option(&wop->wo_twk);
    check_string_option(&wop->wo_tws);
#endif
#ifdef FEAT_LINEBREAK
    check_string_option(&wop->wo_briopt);
#endif
    check_string_option(&wop->wo_wcr);
    check_string_option(&wop->wo_lcs);
    check_string_option(&wop->wo_fcs);
    check_string_option(&wop->wo_ve);
    check_string_option(&wop->wo_whl);
}

/*
 * Free the allocated memory inside a winopt_T.
 */
    ശൂന്യം
clear_winopt(winopt_T *wop UNUSED)
{
#ifdef FEAT_FOLDING
    clear_string_option(&wop->wo_fdi);
    clear_string_option(&wop->wo_fdm);
    clear_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    clear_string_option(&wop->wo_fde);
    clear_string_option(&wop->wo_fdt);
# endif
    clear_string_option(&wop->wo_fmr);
#endif
    clear_string_option(&wop->wo_eiw);
#ifdef FEAT_SIGNS
    clear_string_option(&wop->wo_scl);
#endif
#ifdef FEAT_LINEBREAK
    clear_string_option(&wop->wo_briopt);
#endif
    clear_string_option(&wop->wo_wcr);
#ifdef FEAT_RIGHTLEFT
    clear_string_option(&wop->wo_rlc);
#endif
#ifdef FEAT_LINEBREAK
    clear_string_option(&wop->wo_sbr);
#endif
#ifdef FEAT_STL_OPT
    clear_string_option(&wop->wo_stl);
    clear_string_option(&wop->wo_stlo);
#endif
#ifdef FEAT_SYN_HL
    clear_string_option(&wop->wo_culopt);
    clear_string_option(&wop->wo_cc);
#endif
#ifdef FEAT_CONCEAL
    clear_string_option(&wop->wo_cocu);
#endif
#ifdef FEAT_TERMINAL
    clear_string_option(&wop->wo_twk);
    clear_string_option(&wop->wo_tws);
#endif
    clear_string_option(&wop->wo_lcs);
    clear_string_option(&wop->wo_fcs);
    clear_string_option(&wop->wo_ve);
    clear_string_option(&wop->wo_whl);
}

#ifdef FEAT_EVAL
// Index into the options table for a buffer-local option enum.
സ്ഥിതം പൂർണ്ണസംഖ്യ buf_opt_idx[BV_എണ്ണം];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    സ്ഥിതം ശൂന്യം
init_buf_opt_idx(ശൂന്യം)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ did_init_buf_opt_idx = FALSE;
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (did_init_buf_opt_idx)
	തിരികെ;
    did_init_buf_opt_idx = TRUE;
    വേണ്ടി (i = 0; !istermoption_idx(i); i++)
	എങ്കിൽ (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_MASK] = i;
}
#അല്ലെങ്കിൽ
# define COPY_OPT_SCTX(buf, bv)
# define COPY_OPT_INSECURE(flagsfield, bv)
#endif

/*
 * Copy global option values to local options for one buffer.
 * Used when creating a new buffer and sometimes when entering a buffer.
 * flags:
 * BCO_ENTER	We will enter the buffer "buf".
 * BCO_ALWAYS	Always copy the options, but only set b_p_initialized when
 *		appropriate.
 * BCO_NOHELP	Don't copy the values to a help buffer.
 */
    ശൂന്യം
buf_copy_options(buf_T *buf, പൂർണ്ണസംഖ്യ flags)
{
    പൂർണ്ണസംഖ്യ		should_copy = TRUE;
    char_u	*save_p_isk = ശൂന്യ;	    // init for GCC
    പൂർണ്ണസംഖ്യ		dont_do_help;
    പൂർണ്ണസംഖ്യ		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    എങ്കിൽ (p_cpo != ശൂന്യ)
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
	എങ്കിൽ ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == ശൂന്യ || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != ശൂന്യ)))
	    should_copy = FALSE;

	എങ്കിൽ (should_copy || (flags & BCO_ALWAYS))
	{
#ifdef FEAT_EVAL
	    CLEAR_FIELD(buf->b_p_script_ctx);
	    init_buf_opt_idx();
#endif
	    // Don't copy the options specific to a help buffer when
	    // BCO_NOHELP is given or the options were initialized already
	    // (jumping back to a help file with CTRL-T or CTRL-O)
	    dont_do_help = ((flags & BCO_NOHELP) && buf->b_help)
						       || buf->b_p_initialized;
	    എങ്കിൽ (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = ശൂന്യ;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    എങ്കിൽ (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		തിരഞ്ഞെടുപ്പ് (*p_ffs)
		{
		    അവസ്ഥ 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); നിർത്തുക;
		    അവസ്ഥ 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); നിർത്തുക;
		    അവസ്ഥ 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); നിർത്തുക;
		    സ്ഥിരസ്ഥിതി:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		എങ്കിൽ (buf->b_p_ff != ശൂന്യ)
		    buf->b_start_ffc = *buf->b_p_ff;
		buf->b_p_bh = empty_option;
		buf->b_p_bt = empty_option;
	    }
	    അല്ലെങ്കിൽ
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
	    എങ്കിൽ (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    അല്ലെങ്കിൽ
	    {
		buf->b_p_swf = p_swf;
		COPY_OPT_SCTX(buf, BV_SWF);
	    }
	    buf->b_p_cpt = vim_strsave(p_cpt);
	    COPY_OPT_SCTX(buf, BV_CPT);
#ifdef FEAT_COMPL_FUNC
	    COPY_OPT_INSECURE(buf->b_p_cpt_flags, BV_CPT);
	    set_buflocal_cpt_callbacks(buf);
#endif
#ifdef BACKSLASH_IN_FILENAME
	    buf->b_p_csl = vim_strsave(p_csl);
	    COPY_OPT_SCTX(buf, BV_CSL);
#endif
#ifdef FEAT_COMPL_FUNC
	    buf->b_p_cfu = vim_strsave(p_cfu);
	    COPY_OPT_SCTX(buf, BV_CFU);
	    set_buflocal_cfu_callback(buf);
	    buf->b_p_ofu = vim_strsave(p_ofu);
	    COPY_OPT_SCTX(buf, BV_OFU);
	    set_buflocal_ofu_callback(buf);
#endif
#ifdef FEAT_EVAL
	    buf->b_p_tfu = vim_strsave(p_tfu);
	    COPY_OPT_SCTX(buf, BV_TFU);
	    set_buflocal_tfu_callback(buf);
#endif
	    buf->b_p_sts = p_sts;
	    COPY_OPT_SCTX(buf, BV_STS);
	    buf->b_p_sts_nopaste = p_sts_nopaste;
#ifdef FEAT_VARTABS
	    buf->b_p_vsts = vim_strsave(p_vsts);
	    COPY_OPT_SCTX(buf, BV_VSTS);
	    എങ്കിൽ (p_vsts && p_vsts != empty_option)
		(ശൂന്യം)tabstop_set(p_vsts, &buf->b_p_vsts_array);
	    അല്ലെങ്കിൽ
		buf->b_p_vsts_array = ശൂന്യ;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : ശൂന്യ;
#endif
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
	    buf->b_p_ft = empty_option;
	    buf->b_p_pi = p_pi;
	    COPY_OPT_SCTX(buf, BV_PI);
	    buf->b_p_cinw = vim_strsave(p_cinw);
	    COPY_OPT_SCTX(buf, BV_CINW);
	    buf->b_p_lisp = p_lisp;
	    COPY_OPT_SCTX(buf, BV_LISP);
#ifdef FEAT_SYN_HL
	    // Don't copy 'syntax', it must be set
	    buf->b_p_syn = empty_option;
	    buf->b_p_smc = p_smc;
	    COPY_OPT_SCTX(buf, BV_SMC);
	    buf->b_s.b_syn_isk = empty_option;
#endif
#ifdef FEAT_SPELL
	    buf->b_s.b_p_spc = vim_strsave(p_spc);
	    COPY_OPT_SCTX(buf, BV_SPC);
	    (ശൂന്യം)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#endif
#എങ്കിൽ defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#endif
	    buf->b_p_fp = empty_option;
#എങ്കിൽ defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#endif
#ifdef FEAT_CRYPT
	    buf->b_p_താക്കോൽ = vim_strsave(p_താക്കോൽ);
	    COPY_OPT_SCTX(buf, BV_താക്കോൽ);
#endif
	    buf->b_p_sua = vim_strsave(p_sua);
	    COPY_OPT_SCTX(buf, BV_SUA);
#ifdef FEAT_KEYMAP
	    buf->b_p_keymap = vim_strsave(p_keymap);
	    COPY_OPT_SCTX(buf, BV_KMAP);
	    buf->b_kmap_state |= KEYMAP_INIT;
#endif
#ifdef FEAT_TERMINAL
	    buf->b_p_twsl = p_twsl;
	    COPY_OPT_SCTX(buf, BV_TWSL);
#endif
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
#ifdef HAVE_FSYNC
	    buf->b_p_fs = -1;
#endif
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    buf->b_p_bkc = empty_option;
	    buf->b_bkc_flags = 0;
#ifdef FEAT_QUICKFIX
	    buf->b_p_gefm = empty_option;
	    buf->b_p_gp = empty_option;
	    buf->b_p_mp = empty_option;
	    buf->b_p_efm = empty_option;
#endif
	    buf->b_p_ep = empty_option;
#എങ്കിൽ defined(FEAT_EVAL)
	    buf->b_p_ffu = empty_option;
#endif
	    buf->b_p_kp = empty_option;
	    buf->b_p_path = empty_option;
	    buf->b_p_tags = empty_option;
	    buf->b_p_tc = empty_option;
	    buf->b_tc_flags = 0;
#ifdef FEAT_FIND_ID
	    buf->b_p_def = empty_option;
	    buf->b_p_inc = empty_option;
# ifdef FEAT_EVAL
	    buf->b_p_inex = vim_strsave(p_inex);
	    COPY_OPT_SCTX(buf, BV_INEX);
	    COPY_OPT_INSECURE(buf->b_p_inex_flags, BV_INEX);
# endif
#endif
	    buf->b_p_cot = empty_option;
	    buf->b_cot_flags = 0;
	    buf->b_p_dict = empty_option;
#ifdef FEAT_DIFF
	    buf->b_p_dia = empty_option;
#endif
	    buf->b_p_tsr = empty_option;
#ifdef FEAT_COMPL_FUNC
	    buf->b_p_tsrfu = empty_option;
#endif
	    buf->b_p_qe = vim_strsave(p_qe);
	    COPY_OPT_SCTX(buf, BV_QE);
#എങ്കിൽ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = empty_option;
#endif
#എങ്കിൽ defined(FEAT_CRYPT)
	    buf->b_p_cm = empty_option;
#endif
#ifdef FEAT_PERSISTENT_UNDO
	    buf->b_p_udf = p_udf;
	    COPY_OPT_SCTX(buf, BV_UDF);
#endif
	    buf->b_p_lw = empty_option;
	    buf->b_p_menc = empty_option;

	    // Don't copy the options set by ex_help(), use the saved values,
	    // when going from a help buffer to a non-help buffer.
	    // Don't touch these at all when BCO_NOHELP is used and going from
	    // or to a help buffer.
	    എങ്കിൽ (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ifdef FEAT_VARTABS
		എങ്കിൽ (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (ശൂന്യം)tabstop_set(p_vts, &buf->b_p_vts_array);
		അല്ലെങ്കിൽ
		    buf->b_p_vts_array = ശൂന്യ;
#endif
	    }
	    അല്ലെങ്കിൽ
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#ifdef FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		എങ്കിൽ (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (ശൂന്യം)tabstop_set(p_vts, &buf->b_p_vts_array);
		അല്ലെങ്കിൽ
		    buf->b_p_vts_array = ശൂന്യ;
#endif
		buf->b_help = false;
		എങ്കിൽ (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	എങ്കിൽ (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    എങ്കിൽ (did_isk)
	(ശൂന്യം)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    ശൂന്യം
reset_modifiable(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    എങ്കിൽ (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}

/*
 * Set the global value for 'iminsert' to the local value.
 */
    ശൂന്യം
set_iminsert_global(ശൂന്യം)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    ശൂന്യം
set_imsearch_global(ശൂന്യം)
{
    p_imsearch = curbuf->b_p_imsearch;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ expand_option_idx = -1;
സ്ഥിതം പൂർണ്ണസംഖ്യ expand_option_start_col = 0;
സ്ഥിതം char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
സ്ഥിതം പൂർണ്ണസംഖ്യ expand_option_flags = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ expand_option_append = FALSE;

    ശൂന്യം
set_context_in_set_cmd(
    expand_T	*xp,
    char_u	*arg,
    പൂർണ്ണസംഖ്യ		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    പൂർണ്ണസംഖ്യ		nextchar;
    long_u	flags = 0;	// init for GCC
    പൂർണ്ണസംഖ്യ		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		is_term_option = FALSE;
    പൂർണ്ണസംഖ്യ		താക്കോൽ;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    എങ്കിൽ (*arg == NUL)
    {
	xp->xp_pattern = arg;
	തിരികെ;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    എങ്കിൽ (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_pattern = p + 1;
	തിരികെ;
    }
    ആകുമ്പോൾ (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	എങ്കിൽ (*p == ' ' || *p == ',')
	{
	    ആകുമ്പോൾ (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	എങ്കിൽ (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    നിർത്തുക;
	}
	--p;
    }
    എങ്കിൽ (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_NO;
	p += 2;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_INV;
	p += 3;
    }
    xp->xp_pattern = arg = p;
    എങ്കിൽ (*arg == '<')
    {
	ആകുമ്പോൾ (*p != '>')
	    എങ്കിൽ (*p++ == NUL)	    // expand terminal option name
		തിരികെ;
	താക്കോൽ = get_special_താക്കോൽ_code(arg + 1);
	എങ്കിൽ (താക്കോൽ == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    തിരികെ;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(താക്കോൽ);
	expand_option_name[3] = KEY2TERMCAP1(താക്കോൽ);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    എങ്കിൽ (*p != NUL)
		++p;
	    എങ്കിൽ (*p == NUL)
		തിരികെ;		// expand option name
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	അല്ലെങ്കിൽ
	{
	    // Allow * wildcard
	    ആകുമ്പോൾ (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    എങ്കിൽ (*p == NUL)
		തിരികെ;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    എങ്കിൽ (opt_idx == -1 || options[opt_idx].var == ശൂന്യ)
	    {
		xp->xp_context = EXPAND_NOTHING;
		തിരികെ;
	    }
	    flags = options[opt_idx].flags;
	    എങ്കിൽ (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		തിരികെ;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    പൂർണ്ണസംഖ്യ expand_option_subtract = FALSE;
    എങ്കിൽ ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	എങ്കിൽ (nextchar == '-')
	    expand_option_subtract = TRUE;
	എങ്കിൽ (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    എങ്കിൽ ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	തിരികെ;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    എങ്കിൽ (is_term_option)
	expand_option_idx = -1;
    അല്ലെങ്കിൽ
	expand_option_idx = opt_idx;

    എങ്കിൽ (!is_term_option)
    {
	എങ്കിൽ (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    തിരികെ;
	}
    }

    xp->xp_pattern = p + 1;
    expand_option_start_col = (പൂർണ്ണസംഖ്യ)(p + 1 - xp->xp_വരി);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ifdef FEAT_SYN_HL
    എങ്കിൽ (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	തിരികെ;
    }
#endif
    എങ്കിൽ (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	തിരികെ;
    }
#ifdef FEAT_KEYMAP
    എങ്കിൽ (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	തിരികെ;
    }
#endif

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    എങ്കിൽ (expand_option_subtract)
    {
	xp->xp_context = EXPAND_SETTING_SUBTRACT;
	തിരികെ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != ശൂന്യ)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*xp->xp_pattern == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	തിരികെ;
    }
    അല്ലെങ്കിൽ
	xp->xp_context = EXPAND_NOTHING;

    എങ്കിൽ (is_term_option || (flags & P_അക്കം))
	തിരികെ;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    എങ്കിൽ (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	എങ്കിൽ (p == (char_u *)&p_bdir
		|| p == (char_u *)&p_dir
		|| p == (char_u *)&p_path
		|| p == (char_u *)&p_pp
		|| p == (char_u *)&p_rtp
		|| p == (char_u *)&p_cdpath
#ifdef FEAT_SESSION
		|| p == (char_u *)&p_vdir
#endif
		)
	{
	    xp->xp_context = EXPAND_DIRECTORIES;
	    എങ്കിൽ (p == (char_u *)&p_path || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    അല്ലെങ്കിൽ
		xp->xp_backslash = XP_BS_ONE;
	}
	അല്ലെങ്കിൽ
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    എങ്കിൽ (p == (char_u *)&p_tags)
		xp->xp_backslash = XP_BS_THREE;
	    അല്ലെങ്കിൽ
		xp->xp_backslash = XP_BS_ONE;
	}
	എങ്കിൽ (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    എങ്കിൽ ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	വേണ്ടി (p = argend - 1; p >= xp->xp_pattern; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    എങ്കിൽ (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		ആകുമ്പോൾ (s > xp->xp_pattern && *(s - 1) == '\\')
		    --s;
		എങ്കിൽ ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#എങ്കിൽ defined(BACKSLASH_IN_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#അല്ലെങ്കിൽ
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#endif
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_pattern = p + 1;
		    നിർത്തുക;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    എങ്കിൽ (flags & P_FLAGLIST)
	xp->xp_pattern = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ifdef FEAT_SPELL
    എങ്കിൽ (options[opt_idx].var == (char_u *)&p_sps)
    {
	എങ്കിൽ (STRNCMP(xp->xp_pattern, "file:", 5) == 0)
	{
	    xp->xp_pattern += 5;
	    തിരികെ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (options[expand_option_idx].opt_expand_cb != ശൂന്യ)
	{
	    xp->xp_context = EXPAND_STRING_SETTING;
	}
    }
#endif
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
match_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	പൂർണ്ണസംഖ്യ		idx,
	പൂർണ്ണസംഖ്യ		test_only,
	പൂർണ്ണസംഖ്യ		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    എങ്കിൽ (!fuzzy)
    {
	എങ്കിൽ (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    എങ്കിൽ (!test_only)
		matches[idx] = vim_strsave(str);
	    തിരികെ TRUE;
	}
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ score;

	score = fuzzy_match_str(str, fuzzystr);
	എങ്കിൽ (score != FUZZY_SCORE_NONE)
	{
	    എങ്കിൽ (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    തിരികെ TRUE;
	}
    }

    തിരികെ FALSE;
}

    പൂർണ്ണസംഖ്യ
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches,
    char_u	***matches,
    പൂർണ്ണസംഖ്യ		can_fuzzy)
{
    പൂർണ്ണസംഖ്യ		അക്കം_normal = 0;	    // Nr of matching non-term-code settings
    പൂർണ്ണസംഖ്യ		അക്കം_term = 0;	    // Nr of matching terminal code settings
    പൂർണ്ണസംഖ്യ		opt_idx;
    പൂർണ്ണസംഖ്യ		match;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    char_u	*str;
    പൂർണ്ണസംഖ്യ		loop;
    പൂർണ്ണസംഖ്യ		is_term_opt;
    char_u	name_buf[MAX_താക്കോൽ_NAME_LEN];
    സ്ഥിതം അക്ഷരം *(names[]) = {"all", "termcap"};
    പൂർണ്ണസംഖ്യ		ic = regmatch->rm_ic;	// remember the ignore-case flag
    പൂർണ്ണസംഖ്യ		fuzzy;
    fuzmatch_str_T  *fuzmatch = ശൂന്യ;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    വേണ്ടി (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	എങ്കിൽ (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    വേണ്ടി (match = 0; match < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(names); ++match)
	    {
		എങ്കിൽ (match_str((char_u *)names[match], regmatch, *matches,
			    എണ്ണം, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    എങ്കിൽ (loop == 0)
			അക്കം_normal++;
		    അല്ലെങ്കിൽ
			എണ്ണം++;
		}
	    }
	}
	വേണ്ടി (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != ശൂന്യ;
								    opt_idx++)
	{
	    എങ്കിൽ (options[opt_idx].var == ശൂന്യ)
		തുടരുക;
	    എങ്കിൽ (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		തുടരുക;
	    is_term_opt = istermoption_idx(opt_idx);
	    എങ്കിൽ (is_term_opt && അക്കം_normal > 0)
		തുടരുക;

	    എങ്കിൽ (match_str(str, regmatch, *matches, എണ്ണം, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		എങ്കിൽ (loop == 0)
		{
		    എങ്കിൽ (is_term_opt)
			അക്കം_term++;
		    അല്ലെങ്കിൽ
			അക്കം_normal++;
		}
		അല്ലെങ്കിൽ
		    എണ്ണം++;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (!fuzzy && options[opt_idx].shortname != ശൂന്യ
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		എങ്കിൽ (loop == 0)
		{
		    എങ്കിൽ (is_term_opt)
			അക്കം_term++;
		    അല്ലെങ്കിൽ
			അക്കം_normal++;
		}
		അല്ലെങ്കിൽ
		    (*matches)[എണ്ണം++] = vim_strsave(str);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		എങ്കിൽ (match_str(name_buf, regmatch, *matches, എണ്ണം, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    എങ്കിൽ (loop == 0)
			അക്കം_term++;
		    അല്ലെങ്കിൽ
			എണ്ണം++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	എങ്കിൽ (xp->xp_context != EXPAND_BOOL_SETTINGS  && അക്കം_normal == 0)
	{
	    വേണ്ടി (opt_idx = 0; (str = get_termcode(opt_idx)) != ശൂന്യ; opt_idx++)
	    {
		എങ്കിൽ (!SAFE_isprint(str[0]) || !SAFE_isprint(str[1]))
		    തുടരുക;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		എങ്കിൽ (match_str(name_buf, regmatch, *matches, എണ്ണം,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    എങ്കിൽ (loop == 0)
			അക്കം_term++;
		    അല്ലെങ്കിൽ
			എണ്ണം++;
		}
		അല്ലെങ്കിൽ
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    എങ്കിൽ (match_str(name_buf, regmatch, *matches, എണ്ണം,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			എങ്കിൽ (loop == 0)
			    അക്കം_term++;
			അല്ലെങ്കിൽ
			    എണ്ണം++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    വേണ്ടി (opt_idx = 0; (str = get_താക്കോൽ_name(opt_idx)) != ശൂന്യ; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		എങ്കിൽ (match_str(name_buf, regmatch, *matches, എണ്ണം, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    എങ്കിൽ (loop == 0)
			അക്കം_term++;
		    അല്ലെങ്കിൽ
			എണ്ണം++;
		}
	    }
	}
	എങ്കിൽ (loop == 0)
	{
	    എങ്കിൽ (അക്കം_normal > 0)
		*അക്കം_Matches = അക്കം_normal;
	    അല്ലെങ്കിൽ എങ്കിൽ (അക്കം_term > 0)
		*അക്കം_Matches = അക്കം_term;
	    അല്ലെങ്കിൽ
		തിരികെ OK;
	    എങ്കിൽ (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *അക്കം_Matches);
		എങ്കിൽ (*matches == ശൂന്യ)
		{
		    *matches = (char_u **)"";
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *അക്കം_Matches);
		എങ്കിൽ (fuzmatch == ശൂന്യ)
		{
		    *matches = (char_u **)"";
		    തിരികെ FAIL;
		}
	    }
	}
    }

    എങ്കിൽ (fuzzy &&
	fuzzymatches_to_strmatches(fuzmatch, matches, എണ്ണം, FALSE) == FAIL)
	തിരികെ FAIL;

    തിരികെ OK;
}

// Escape an option value that can be used on the command-line with :set.
// Caller needs to free the returned string, unless NULL is returned.
    സ്ഥിതം char_u*
escape_option_str_cmdline(char_u *var)
{
    char_u *buf;

    // A backslash is required before some characters.  This is the reverse of
    // what happens in do_set().
    buf = vim_strsave_escaped(var, escape_chars);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ ശൂന്യ;

#ifdef BACKSLASH_IN_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    വേണ്ടി (var = buf; *var != NUL; MB_PTR_ADV(var))
	എങ്കിൽ (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#endif
    തിരികെ buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    പൂർണ്ണസംഖ്യ
ExpandOldSetting(പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    char_u  *var = ശൂന്യ;	// init for GCC
    char_u  *buf;

    *അക്കം_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    എങ്കിൽ (*matches == ശൂന്യ)
	തിരികെ FAIL;

    // For a terminal key code expand_option_idx is < 0.
    എങ്കിൽ (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	എങ്കിൽ (var == ശൂന്യ)
	    expand_option_idx = findoption(expand_option_name);
    }

    എങ്കിൽ (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (var == ശൂന്യ)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    എങ്കിൽ (buf == ശൂന്യ)
    {
	VIM_CLEAR(*matches);
	തിരികെ FAIL;
    }

    (*matches)[0] = buf;
    *അക്കം_Matches = 1;
    തിരികെ OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    പൂർണ്ണസംഖ്യ
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches,
    char_u	***matches)
{
    char_u  *var = ശൂന്യ;	// init for GCC
    char_u  *buf;

    എങ്കിൽ (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == ശൂന്യ)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	തിരികെ FAIL;
    }

    optexpand_T args;
    args.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    args.oe_append = expand_option_append;
    args.oe_regmatch = regmatch;
    args.oe_xp = xp;
    args.oe_set_arg = xp->xp_വരി + expand_option_start_col;
    args.oe_include_orig_val =
	!expand_option_append &&
	(*args.oe_set_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ FAIL;

    args.oe_opt_മൂല്യം = buf;

    പൂർണ്ണസംഖ്യ അക്കം_ret = options[expand_option_idx].opt_expand_cb(&args, അക്കം_Matches, matches);

    vim_free(buf);
    തിരികെ അക്കം_ret;
}

/*
 * Expansion handler for :set-=
 */
    പൂർണ്ണസംഖ്യ
ExpandSettingSubtract(
    expand_T	*xp,
    regmatch_T	*regmatch,
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches,
    char_u	***matches)
{
    എങ്കിൽ (expand_option_idx < 0)
	// term option
	തിരികെ ExpandOldSetting(അക്കം_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    എങ്കിൽ (option_flags & P_അക്കം)
	തിരികെ ExpandOldSetting(അക്കം_Matches, matches);
    അല്ലെങ്കിൽ എങ്കിൽ (option_flags & P_COMMA)
    {
	// Split the option by comma, then present each option to the user if
	// it matches the pattern.
	// This condition needs to go first, because 'whichwrap' has both
	// P_COMMA and P_FLAGLIST.
	garray_T    ga;

	char_u	    *item;
	char_u	    *option_copy;
	char_u	    *next_val;
	char_u	    *comma;

	എങ്കിൽ (*option_val == NUL)
	    തിരികെ FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	എങ്കിൽ (option_copy == ശൂന്യ)
	    തിരികെ FAIL;
	next_val = option_copy;

	ga_init2(&ga, വലുപ്പം(char_u *), 10);

	ചെയ്യുക
	{
	    item = next_val;
	    comma = vim_strchr(next_val, ',');
	    ആകുമ്പോൾ (comma != ശൂന്യ && comma != next_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    എങ്കിൽ (comma != ശൂന്യ)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		next_val = comma + 1;
	    }
	    അല്ലെങ്കിൽ
		next_val = ശൂന്യ;

	    എങ്കിൽ (*item == NUL)
		// empty value, don't add to list
		തുടരുക;

	    എങ്കിൽ (!vim_regexec(regmatch, item, (colnr_T)0))
		തുടരുക;

	    char_u *buf = escape_option_str_cmdline(item);
	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		vim_free(option_copy);
		ga_clear_strings(&ga);
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (ga_add_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		നിർത്തുക;
	    }
	} ആകുമ്പോൾ (next_val != ശൂന്യ);

	vim_free(option_copy);

	*matches = ga.ga_data;
	*അക്കം_Matches = ga.ga_len;
	തിരികെ OK;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	എങ്കിൽ (*xp->xp_pattern != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    തിരികെ FAIL;
	}

	size_t അക്കം_flags = STRLEN(option_val);
	എങ്കിൽ (അക്കം_flags == 0)
	    തിരികെ FAIL;

	*matches = ALLOC_MULT(char_u *, അക്കം_flags + 1);
	എങ്കിൽ (*matches == ശൂന്യ)
	    തിരികെ FAIL;

	പൂർണ്ണസംഖ്യ എണ്ണം = 0;
	char_u *p;

	p = vim_strnsave(option_val, അക്കം_flags);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    VIM_CLEAR(*matches);
	    തിരികെ FAIL;
	}
	(*matches)[എണ്ണം++] = p;

	എങ്കിൽ (അക്കം_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    വേണ്ടി (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		എങ്കിൽ (p == ശൂന്യ)
		    നിർത്തുക;
		(*matches)[എണ്ണം++] = p;
	    }
	}

	*അക്കം_Matches = എണ്ണം;
	തിരികെ OK;
    }

    തിരികെ ExpandOldSetting(അക്കം_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    സ്ഥിതം ശൂന്യം
option_value2string(
    ഘടന vimoption	*opp,
    പൂർണ്ണസംഖ്യ			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    എങ്കിൽ (opp->flags & P_അക്കം)
    {
	long wc = 0;

	എങ്കിൽ (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_താക്കോൽ_name((പൂർണ്ണസംഖ്യ)wc, 0));
	അല്ലെങ്കിൽ എങ്കിൽ (wc != 0)
	    STRCPY(NameBuff, transchar((പൂർണ്ണസംഖ്യ)wc));
	അല്ലെങ്കിൽ
	    sprintf((അക്ഷരം *)NameBuff, "%ld", *(long *)varp);
    }
    അല്ലെങ്കിൽ    // P_STRING
    {
	varp = *(char_u **)(varp);
	എങ്കിൽ (varp == ശൂന്യ)		    // just in case
	    NameBuff[0] = NUL;
#ifdef FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	അല്ലെങ്കിൽ എങ്കിൽ (opp->var == (char_u *)&p_താക്കോൽ && *varp)
	    STRCPY(NameBuff, "*****");
#endif
	അല്ലെങ്കിൽ എങ്കിൽ (opp->flags & P_EXPAND)
	    home_replace(ശൂന്യ, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	അല്ലെങ്കിൽ എങ്കിൽ ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	അല്ലെങ്കിൽ
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
wc_use_keyname(char_u *varp, long *wcp)
{
    എങ്കിൽ (((long *)varp == &p_wc) || ((long *)varp == &p_wcm))
    {
	*wcp = *(long *)varp;
	എങ്കിൽ (IS_SPECIAL(*wcp) || find_special_താക്കോൽ_in_table((പൂർണ്ണസംഖ്യ)*wcp) >= 0)
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    പൂർണ്ണസംഖ്യ
shortmess(പൂർണ്ണസംഖ്യ x)
{
    തിരികെ p_shm != ശൂന്യ &&
	    (   vim_strchr(p_shm, x) != ശൂന്യ
	    || (vim_strchr(p_shm, 'a') != ശൂന്യ
		&& vim_strchr((char_u *)SHM_A, x) != ശൂന്യ));
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 * When "fname" is not NULL, use it to set $"envname" when it wasn't set yet.
 */
    ശൂന്യം
vimrc_found(char_u *fname, char_u *envname)
{
    പൂർണ്ണസംഖ്യ		opt_idx;
    പൂർണ്ണസംഖ്യ		dofree = FALSE;
    char_u	*p;

    എങ്കിൽ (!option_was_set((char_u *)"cp"))
    {
	p_cp = FALSE;
	വേണ്ടി (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    എങ്കിൽ (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    എങ്കിൽ (fname != ശൂന്യ)
    {
	p = vim_getenv(envname, &dofree);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = FullName_save(fname, FALSE);
	    എങ്കിൽ (p != ശൂന്യ)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    ശൂന്യം
change_compatible(പൂർണ്ണസംഖ്യ on)
{
    പൂർണ്ണസംഖ്യ	    opt_idx;

    എങ്കിൽ (p_cp != on)
    {
	p_cp = on;
	compatible_set();
    }
    opt_idx = findoption((char_u *)"cp");
    എങ്കിൽ (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_SET;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    പൂർണ്ണസംഖ്യ
option_was_set(char_u *name)
{
    പൂർണ്ണസംഖ്യ idx;

    idx = findoption(name);
    എങ്കിൽ (idx < 0)	// unknown option
	തിരികെ FALSE;
    എങ്കിൽ (options[idx].flags & P_WAS_SET)
	തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    പൂർണ്ണസംഖ്യ
reset_option_was_set(char_u *name)
{
    പൂർണ്ണസംഖ്യ idx = findoption(name);

    എങ്കിൽ (idx < 0)
	തിരികെ FAIL;

    options[idx].flags &= ~P_WAS_SET;
    തിരികെ OK;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    സ്ഥിതം ശൂന്യം
compatible_set(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    opt_idx;

    വേണ്ടി (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	എങ്കിൽ (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    set_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    പൂർണ്ണസംഖ്യ
can_bs(
    പൂർണ്ണസംഖ്യ		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ifdef FEAT_JOB_CHANNEL
    എങ്കിൽ (what == BS_START && bt_prompt(curbuf))
	തിരികെ FALSE;
#endif
    തിരഞ്ഞെടുപ്പ് (*p_bs)
    {
	അവസ്ഥ '3':       തിരികെ TRUE;
	അവസ്ഥ '2':	തിരികെ (what != BS_NOSTOP);
	അവസ്ഥ '1':	തിരികെ (what != BS_START);
	അവസ്ഥ '0':	തിരികെ FALSE;
    }
    തിരികെ vim_strchr(p_bs, what) != ശൂന്യ;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloff_മൂല്യം(ശൂന്യം)
{
    തിരികെ curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloffpad_മൂല്യം(ശൂന്യം)
{
    തിരികെ curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_sidescrolloff_മൂല്യം(ശൂന്യം)
{
    തിരികെ curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    unsigned പൂർണ്ണസംഖ്യ
get_bkc_flags(buf_T *buf)
{
    തിരികെ buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#എങ്കിൽ defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_മൂല്യം(buf_T *buf)
{
    എങ്കിൽ (buf->b_p_flp == ശൂന്യ || *buf->b_p_flp == NUL)
	തിരികെ p_flp;
    തിരികെ buf->b_p_flp;
}
#endif

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    unsigned പൂർണ്ണസംഖ്യ
get_ve_flags(ശൂന്യം)
{
    തിരികെ (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#എങ്കിൽ defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_മൂല്യം(win_T *win)
{
    എങ്കിൽ (win->w_p_sbr == ശൂന്യ || *win->w_p_sbr == NUL)
	തിരികെ p_sbr;
    എങ്കിൽ (STRCMP(win->w_p_sbr, "NONE") == 0)
	തിരികെ empty_option;
    തിരികെ win->w_p_sbr;
}
#endif

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(പൂർണ്ണസംഖ്യ bufopt)
{
    dict_T	*d;
    പൂർണ്ണസംഖ്യ		opt_idx;

    d = dict_alloc();
    എങ്കിൽ (d == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	ഘടന vimoption *opt = &options[opt_idx];

	എങ്കിൽ ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    എങ്കിൽ (varp != ശൂന്യ)
	    {
		എങ്കിൽ (opt->flags & P_STRING)
		    dict_add_string(d, opt->fullname, *(char_u **)varp);
		അല്ലെങ്കിൽ എങ്കിൽ (opt->flags & P_അക്കം)
		    dict_add_സംഖ്യ(d, opt->fullname, *(long *)varp);
		അല്ലെങ്കിൽ
		    dict_add_സംഖ്യ(d, opt->fullname, *(പൂർണ്ണസംഖ്യ *)varp);
	    }
	}
    }

    തിരികെ d;
}
#endif

#എങ്കിൽ defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    പൂർണ്ണസംഖ്യ
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    എങ്കിൽ (val == ശൂന്യ)
	p = wp->w_p_culopt;
    അല്ലെങ്കിൽ
	p = val;
    ആകുമ്പോൾ (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	എങ്കിൽ (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_വരി;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_വരി | CULOPT_NBR;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	എങ്കിൽ (*p != ',' && *p != NUL)
	    തിരികെ FAIL;
	എങ്കിൽ (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    എങ്കിൽ ((culopt_flags_new & CULOPT_വരി) && (culopt_flags_new & CULOPT_SCRLINE))
	തിരികെ FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    തിരികെ OK;
}
#endif

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    പൂർണ്ണസംഖ്യ
magic_isset(ശൂന്യം)
{
    തിരഞ്ഞെടുപ്പ് (magic_overruled)
    {
	അവസ്ഥ OPTION_MAGIC_ON:      തിരികെ TRUE;
	അവസ്ഥ OPTION_MAGIC_OFF:     തിരികെ FALSE;
	അവസ്ഥ OPTION_MAGIC_NOT_SET: നിർത്തുക;
    }
#ifdef FEAT_EVAL
    എങ്കിൽ (in_vim9script())
	തിരികെ TRUE;
#endif
    തിരികെ p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    പൂർണ്ണസംഖ്യ
option_set_callback_func(char_u *optval UNUSED, callback_T *optcb UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    പൂർണ്ണസംഖ്യ		funcname = FALSE;

    എങ്കിൽ (optval == ശൂന്യ || *optval == NUL)
    {
	free_callback(optcb);
	തിരികെ OK;
    }

    എങ്കിൽ (*optval == '{' || (in_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_വ്യഞ്ജകം(optval, ശൂന്യ);
    അല്ലെങ്കിൽ
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    എങ്കിൽ (tv == ശൂന്യ)
	തിരികെ FAIL;

    cb = get_callback(tv);
    എങ്കിൽ (cb.cb_name == ശൂന്യ || *cb.cb_name == NUL)
    {
	free_tv(tv);
	തിരികെ FAIL;
    }

    free_callback(optcb);
    set_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = ശൂന്യ;
    എങ്കിൽ (in_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != ശൂന്യ))
    {
	എങ്കിൽ (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = find_imported(optval, dot - optval, FALSE);
	എങ്കിൽ (import != ശൂന്യ && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((അക്ഷരം *)fnamebuf, വലുപ്പം(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	അല്ലെങ്കിൽ
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

    തിരികെ OK;
#അല്ലെങ്കിൽ
    തിരികെ FAIL;
#endif
}

#എങ്കിൽ defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    അക്ഷരം *
did_set_showtabpanel(optset_T *args UNUSED)
{
    shell_new_columns();
    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_EVAL)
    സ്ഥിതം ശൂന്യം
didset_options_sctx(പൂർണ്ണസംഖ്യ opt_flags, അക്ഷരം **buf)
{
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; ; ++i)
	{
	    എങ്കിൽ (buf[i] == ശൂന്യ)
		നിർത്തുക;

	    പൂർണ്ണസംഖ്യ idx = findoption((char_u *)buf[i]);
	    എങ്കിൽ (idx >= 0)
		set_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#endif
