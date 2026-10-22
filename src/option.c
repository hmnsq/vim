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
#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "optiondefs.h"

ಸ್ಥಿರ ಶೂನ್ಯ set_options_default(ಪೂರ್ಣಾಙ್ಕ opt_flags);
ಸ್ಥಿರ ಶೂನ್ಯ set_string_default_esc(ವರ್ಣ *name, char_u *val, ಪೂರ್ಣಾಙ್ಕ escape);
ಸ್ಥಿರ char_u *find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
ಸ್ಥಿರ char_u *option_expand(ಪೂರ್ಣಾಙ್ಕ opt_idx, char_u *val);
ಸ್ಥಿರ ಶೂನ್ಯ didset_options(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ didset_options2(ಶೂನ್ಯ);
#ಯದಿ defined(FEAT_EVAL)
ಸ್ಥಿರ long_u *insecure_flag(win_T *wp, ಪೂರ್ಣಾಙ್ಕ opt_idx, ಪೂರ್ಣಾಙ್ಕ opt_flags);
#ಅನ್ಯಥಾ
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#endif
ಸ್ಥಿರ ವರ್ಣ *set_bool_option(ಪೂರ್ಣಾಙ್ಕ opt_idx, char_u *varp, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ opt_flags);
ಸ್ಥಿರ ವರ್ಣ *set_ಅಂಕೆ_option(ಪೂರ್ಣಾಙ್ಕ opt_idx, char_u *varp, long ಮೌಲ್ಯ, ವರ್ಣ *errbuf, size_t errbuflen, ಪೂರ್ಣಾಙ್ಕ opt_flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ find_ಕೀಲಿ_option(char_u *arg_arg, ಪೂರ್ಣಾಙ್ಕ has_lt);
ಸ್ಥಿರ ಶೂನ್ಯ showoptions(ಪೂರ್ಣಾಙ್ಕ all, ಪೂರ್ಣಾಙ್ಕ opt_flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ optval_default(ಸಂರಚನಾ vimoption *, char_u *varp, ಪೂರ್ಣಾಙ್ಕ compatible);
ಸ್ಥಿರ ಶೂನ್ಯ showoneopt(ಸಂರಚನಾ vimoption *, ಪೂರ್ಣಾಙ್ಕ opt_flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ put_setstring(FILE *fd, bool legacy, ವರ್ಣ *cmd, ವರ್ಣ *name, char_u **valuep, long_u flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ put_setnum(FILE *fd, ವರ್ಣ *cmd, ವರ್ಣ *name, long *valuep);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ put_setbool(FILE *fd, ವರ್ಣ *cmd, ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ istermoption(ಸಂರಚನಾ vimoption *p);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ istermoption_idx(ಪೂರ್ಣಾಙ್ಕ opt_idx);
ಸ್ಥಿರ char_u *get_varp_scope(ಸಂರಚನಾ vimoption *p, ಪೂರ್ಣಾಙ್ಕ scope);
ಸ್ಥಿರ char_u *get_varp(ಸಂರಚನಾ vimoption *);
ಸ್ಥಿರ ಶೂನ್ಯ check_win_options(win_T *win);
ಸ್ಥಿರ ಶೂನ್ಯ option_value2string(ಸಂರಚನಾ vimoption *, ಪೂರ್ಣಾಙ್ಕ scope);
ಸ್ಥಿರ ಶೂನ್ಯ check_winopt(winopt_T *wop);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ wc_use_keyname(char_u *varp, long *wcp);
ಸ್ಥಿರ ಶೂನ್ಯ compatible_set(ಶೂನ್ಯ);

#ಯದಿ defined(FEAT_EVAL)
ಸ್ಥಿರ ವರ್ಣ *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    ರಿಕ್ತ};
ಸ್ಥಿರ ಶೂನ್ಯ didset_options_sctx(ಪೂರ್ಣಾಙ್ಕ opt_flags, ವರ್ಣ **buf);
#endif


/*
 * Initialize the 'shell' option to a default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_init_default_shell(ಶೂನ್ಯ)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    ಯದಿ (((p = mch_getenv((char_u *)"SHELL")) != ರಿಕ್ತ && *p != NUL)
#ಯದಿ defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != ರಿಕ್ತ && *p != NUL)
	    || ((p = (char_u *)default_shell()) != ರಿಕ್ತ && *p != NUL)
#endif
	    )
#ಯದಿ defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	ಯದಿ (vim_strchr(p, ' ') != ರಿಕ್ತ)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    ಯದಿ (cmd != ರಿಕ್ತ)
	    {
		vim_snprintf((ವರ್ಣ *)cmd, len, "\"%s\"", p);
		set_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	ಅನ್ಯಥಾ
	    set_string_default("sh", p);
    }
#ಅನ್ಯಥಾ
	set_string_default_esc("sh", p, TRUE);
#endif
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_init_default_backupskip(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		plen;
#ifdef UNIX
    ಸ್ಥಿರ ವರ್ಣ	*(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#ಅನ್ಯಥಾ
    ಸ್ಥಿರ ವರ್ಣ	*(names[3]) = {"TMPDIR", "TEMP", "TMP"};
#endif
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(names); ++i)
    {
	ಪೂರ್ಣಾಙ್ಕ		mustfree = FALSE;
#ifdef UNIX
	ಯದಿ (*names[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL("/private/tmp");
# ಅನ್ಯಥಾ
	    p = (char_u *)"/tmp";
	    plen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL("/tmp");
# endif
	}
	ಅನ್ಯಥಾ
#endif
	{
	    p = vim_getenv((char_u *)names[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	ಯದಿ (p != ರಿಕ್ತ && *p != NUL)
	{
	    char_u  *item;
	    size_t  itemsize;
	    ಪೂರ್ಣಾಙ್ಕ	    has_trailing_path_sep = FALSE;

	    ಯದಿ (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
		// does the value include a trailing path separator?
		ಯದಿ (after_pathsep(p, p + plen))
		    has_trailing_path_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_path_sep ? 0 : 1) + 2;
	    item = alloc(itemsize);
	    ಯದಿ (item != ರಿಕ್ತ)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((ವರ್ಣ *)item, itemsize, "%s%s*", p, (has_trailing_path_sep) ? "" : PATHSEPSTR);

		ಯದಿ (find_dup_item(ga.ga_data, item, itemlen, options[opt_idx].flags) == ರಿಕ್ತ
			&& ga_grow(&ga, (ಪೂರ್ಣಾಙ್ಕ)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((ವರ್ಣ *)ga.ga_data + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", item);
		}
		vim_free(item);
	    }
	}
	ಯದಿ (mustfree)
	    vim_free(p);
    }
    ಯದಿ (ga.ga_data != ರಿಕ್ತ)
    {
	set_string_default("bsk", ga.ga_data);
	vim_free(ga.ga_data);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_init_default_maxmemtot(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    ಯದಿ (opt_idx < 0)
	ಪ್ರತ್ಯಾಗಮ;

#ಯದಿ !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
    ಯದಿ (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#endif
    {
#ಯದಿ defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#ನೋಚೇದ್ಯದಿ defined(HAVE_TOTAL_MEM)
	// Use amount of memory available to Vim.
	n = (mch_total_mem(FALSE) >> 1);
#ಅನ್ಯಥಾ
	n = (0x7fffffff >> 11);
#endif
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	ಯದಿ (opt_idx >= 0)
	{
#ಯದಿ !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
	    ಯದಿ ((long)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (long)n
		    || (long)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#endif
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_init_default_cdpath(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    ಪೂರ್ಣಾಙ್ಕ	i;
    ಪೂರ್ಣಾಙ್ಕ	j;
    ಪೂರ್ಣಾಙ್ಕ	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    ಯದಿ (cdpath == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    ಯದಿ (buf != ರಿಕ್ತ)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	ಪ್ರತಿ (i = 0; cdpath[i] != NUL; ++i)
	{
	    ಯದಿ (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	ಯದಿ (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	ಅನ್ಯಥಾ
	    vim_free(buf); // cannot happen
    }
    ಯದಿ (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_init_default_printencoding(ಶೂನ್ಯ)
{
#ಯದಿ defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    set_string_default("penc",
# ಯದಿ defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# ಅನ್ಯಥಾ // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#endif
}

#ಯದಿ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_init_default_printexpr(ಶೂನ್ಯ)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    set_string_default("pexpr",
# ಯದಿ defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# ಅನ್ಯಥಾ
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#endif

#ifdef UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_init_restricted_mode(ಶೂನ್ಯ)
{
    char_u	*p;

    p = get_isolated_shell_name();
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#endif

#ifdef CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_init_clean_rtp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    ಯದಿ (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    ಯದಿ (opt_idx < 0)
	ಪ್ರತ್ಯಾಗಮ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
set_init_xdg_rtp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;
    ಪೂರ್ಣಾಙ್ಕ		has_xdg_env = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = ರಿಕ್ತ;
    char_u	*vimrc2 = ರಿಕ್ತ;
    char_u	*xdg_dir = ರಿಕ್ತ;
    char_u	*xdg_rtp = ರಿಕ್ತ;
    string_T	vimrc_xdg = {ರಿಕ್ತ, 0};

    // initialize chartab, so we can expand $HOME
    (ಶೂನ್ಯ)init_chartab();
    vimrc1 = expand_env_save((char_u *)USR_VIMRC_FILE);
    vimrc2 = expand_env_save((char_u *)USR_VIMRC_FILE2);

    xdg_dir = mch_getenv("XDG_CONFIG_HOME");
    ಯದಿ (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    ಯದಿ (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	ಗಚ್ಛ theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    ಯದಿ ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	ಗಚ್ಛ theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    ಯದಿ ((opt_idx = findoption((char_u *)"packpath")) < 0)
	ಗಚ್ಛ theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# ಯದಿ defined(XDG_VDIR) && defined(FEAT_SESSION)
    ಯದಿ ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	ಗಚ್ಛ theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    ಯದಿ (should_free_xdg_dir)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
set_init_expand_env(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;
    char_u	*p;

    ಪ್ರತಿ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	ಯದಿ ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != ರಿಕ್ತ)
	    p = (char_u *)_(*(ವರ್ಣ **)options[opt_idx].var);
	ಅನ್ಯಥಾ
	    p = option_expand(opt_idx, ರಿಕ್ತ);
	ಯದಿ (p != ರಿಕ್ತ && (p = vim_strsave(p)) != ರಿಕ್ತ)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    ಯದಿ (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#ಯದಿ defined(MSWIN) && defined(FEAT_GETTEXT)
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
    WCHAR	*buffer;
    char_u	*langs = ರಿಕ್ತ;

    ಯದಿ (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &ಅಂಕೆ_languages, ರಿಕ್ತ,
								    &bufsize)
	    || bufsize == 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    buffer = ALLOC_MULT(WCHAR, bufsize);
    ಯದಿ (buffer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &ಅಂಕೆ_languages, buffer,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	ಯದಿ (langs != ರಿಕ್ತ)
	{
	    char_u	*d = langs;
	    WCHAR	*s = buffer;

	    ಯಾವತ್ (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		ಯದಿ (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		ಪ್ರತಿ ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		ಯದಿ (english)
		    ಭಙ್ಗ;
	    }
	    *d = NUL;
	}
    }
    vim_free(buffer);

    ಪ್ರತ್ಯಾಗಮ langs;
}
#endif

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_init_lang_env(ಶೂನ್ಯ)
{
#ಯದಿ defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    ಯದಿ (mch_getenv((char_u *)"LANG") == ರಿಕ್ತ
	    && mch_getenv((char_u *)"LANGUAGE") == ರಿಕ್ತ
	    && mch_getenv((char_u *)"LC_ALL") == ರಿಕ್ತ
	    && mch_getenv((char_u *)"LC_MESSAGES") == ರಿಕ್ತ)
    {
	char_u	*langs = get_ui_langs();

	ಯದಿ (langs != ರಿಕ್ತ && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    ಯದಿ (colon != ರಿಕ್ತ)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#ನೋಚೇದ್ಯದಿ defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#endif
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_init_default_encoding(ಶೂನ್ಯ)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;

#ಯದಿ defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#ಅನ್ಯಥಾ
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#endif
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    ಯದಿ (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    ಯದಿ (mb_init() == ರಿಕ್ತ)
    {
	opt_idx = findoption((char_u *)"encoding");
	ಯದಿ (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#ಯದಿ defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	ಯದಿ (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    set_string_option_direct((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    set_string_option_direct((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    ಯದಿ (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    ಯದಿ (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (ಶೂನ್ಯ)init_chartab();
	}
#endif

#ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	ಯದಿ (
# ifdef VIMDLL
		(!gui.in_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    ವರ್ಣ	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    ಯದಿ (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, ಪರಿಮಾಣ(buf), "cp%ld", (long)GetACP());
	    ಅನ್ಯಥಾ
		buflen = vim_snprintf(buf, ಪರಿಮಾಣ(buf), "cp%ld", (long)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    ಯದಿ (p_tenc != ರಿಕ್ತ)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		ಯದಿ (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_setup(&ಒಳಹರಿವು_conv, p_tenc, p_enc);
		convert_setup(&output_conv, p_enc, p_tenc);
	    }
	    ಅನ್ಯಥಾ
		p_tenc = empty_option;
	}
#endif
#ಯದಿ defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#endif
    }
    ಅನ್ಯಥಾ
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
set_init_1(ಪೂರ್ಣಾಙ್ಕ clean_arg)
{
#ifdef FEAT_LANGMAP
    langmap_init();
#endif

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    ಯದಿ (mch_getenv((char_u *)"VIM_POSIX") != ರಿಕ್ತ)
    {
	set_string_default("cpo", (char_u *)CPO_ALL);
	set_string_default("shm", (char_u *)SHM_POSIX);
    }

    set_init_default_shell();
    set_init_default_backupskip();
    set_init_default_maxmemtot();
    set_init_default_cdpath();
    set_init_default_printencoding();
#ಯದಿ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
    ಯದಿ (clean_arg)
	set_init_clean_rtp();
#endif

#ifdef FEAT_GUI
    ಯದಿ (found_reverse_arg)
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
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

#ಯದಿ defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    ಯದಿ (mch_getenv((char_u *)"MLTERM") != ರಿಕ್ತ)
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"tbidi", 1L, ರಿಕ್ತ, 0);
#endif

    didset_options2();

    set_init_lang_env();

#ifdef FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    set_helplang_default(get_mess_lang());
#endif
}

ಸ್ಥಿರ char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    ಶೂನ್ಯ
set_fencs_unicode(ಶೂನ್ಯ)
{
    set_string_option_direct((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_option_default(
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,
    ಪೂರ್ಣಾಙ್ಕ		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    ಪೂರ್ಣಾಙ್ಕ		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    ಪೂರ್ಣಾಙ್ಕ		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    ಪೂರ್ಣಾಙ್ಕ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    ಯದಿ (varp != ರಿಕ್ತ)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	ಯದಿ (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    ಯದಿ (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		set_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    ಅನ್ಯಥಾ ಯದಿ (options[opt_idx].indir != PV_NONE)
		set_string_option_direct(ರಿಕ್ತ, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (flags & P_ಅಂಕೆ)
	{
	    ಯದಿ (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    ಅನ್ಯಥಾ
	    {
		long def_val = (long)(long_i)options[opt_idx].def_val[dvi];

		ಯದಿ ((long *)varp == &curwin->w_p_so
			|| (long *)varp == &curwin->w_p_siso
			|| (long *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(long *)varp = -1;
		ಅನ್ಯಥಾ
		    *(long *)varp = def_val;
		// May also set global value for local option.
		ಯದಿ (both)
		    *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	ಅನ್ಯಥಾ	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(ಪೂರ್ಣಾಙ್ಕ *)varp = (ಪೂರ್ಣಾಙ್ಕ)(long)(long_i)options[opt_idx].def_val[dvi];
#ifdef UNIX
	    // 'modeline' defaults to off for root
	    ಯದಿ (options[opt_idx].indir == PV_ML && getuid() == ಬೇರು_UID)
		*(ಪೂರ್ಣಾಙ್ಕ *)varp = FALSE;
#endif
	    // May also set global value for local option.
	    ಯದಿ (both)
		*(ಪೂರ್ಣಾಙ್ಕ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(ಪೂರ್ಣಾಙ್ಕ *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	ಯದಿ (both)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
set_options_default(
    ಪೂರ್ಣಾಙ್ಕ		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    win_T	*wp;
    tabpage_T	*tp;

    ಪ್ರತಿ (i = 0; !istermoption_idx(i); i++)
	ಯದಿ (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#ಯದಿ defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_ಕೀಲಿ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
set_string_default_esc(ವರ್ಣ *name, char_u *val, ಪೂರ್ಣಾಙ್ಕ escape)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;

    ಯದಿ (escape && vim_strchr(val, ' ') != ರಿಕ್ತ)
	p = vim_strsave_escaped(val, (char_u *)" ");
    ಅನ್ಯಥಾ
	p = vim_strsave(val);
    ಯದಿ (p == ರಿಕ್ತ)		// we don't want a NULL
	ಪ್ರತ್ಯಾಗಮ;

    opt_idx = findoption((char_u *)name);
    ಯದಿ (opt_idx < 0)
    {
	vim_free(p);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    ಶೂನ್ಯ
set_string_default(ವರ್ಣ *name, char_u *val)
{
    set_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    ಸ್ಥಿರ char_u *
find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags)
{
    ಪೂರ್ಣಾಙ್ಕ	    bs = 0;
    char_u  *s;

    ಯದಿ (origval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (s = origval; *s != NUL; ++s)
    {
	ಯದಿ ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    ಪ್ರತ್ಯಾಗಮ s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	ಯದಿ ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	ಅನ್ಯಥಾ
	    bs = 0;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    ಶೂನ್ಯ
set_ಸಂಖ್ಯೆ_default(ವರ್ಣ *name, long val)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;

    opt_idx = findoption((char_u *)name);
    ಯದಿ (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#ಯದಿ defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    ಶೂನ್ಯ
set_local_options_default(win_T *wp, ಪೂರ್ಣಾಙ್ಕ do_buffer)
{
    win_T	*save_curwin = curwin;
    ಪೂರ್ಣಾಙ್ಕ		i;

    curwin = wp;
    curbuf = curwin->w_buffer;
    block_autocmds();

    ಪ್ರತಿ (i = 0; !istermoption_idx(i); i++)
    {
	ಸಂರಚನಾ vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	ಯದಿ (p->indir != PV_NONE
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

#ಯದಿ defined(EXITFREE)
/*
 * Free all options.
 */
    ಶೂನ್ಯ
free_all_options(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; !istermoption_idx(i); i++)
    {
	ಯದಿ (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    ಯದಿ ((options[i].flags & P_ALLOCED) && options[i].var != ರಿಕ್ತ)
		free_string_option(*(char_u **)options[i].var);
	    ಯದಿ (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	ಅನ್ಯಥಾ ಯದಿ (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# ಯದಿ defined(FEAT_EVAL)
    free_findfunc_option();
# endif
}
#endif


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    ಶೂನ್ಯ
set_init_2(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    ಯದಿ (idx >= 0 && !(options[idx].flags & P_WAS_SET))
	set_option_default(idx, OPT_LOCAL, p_cp);
    comp_col();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    ಯದಿ (!option_was_set((char_u *)"window"))
	p_window = Rows - 1;
    set_ಸಂಖ್ಯೆ_default("window", Rows - 1);

    // For DOS console the default is always black.
#ಯದಿ !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    ಯದಿ (idx >= 0 && !(options[idx].flags & P_WAS_SET)
						 && *term_bg_default() == 'd')
    {
	set_string_option_direct(ರಿಕ್ತ, idx, (char_u *)"dark", OPT_FREE, 0);
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
    (ಶೂನ್ಯ)parse_printoptions(ರಿಕ್ತ);	// parse 'printoptions' default value
#endif
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    ಶೂನ್ಯ
set_init_3(ಶೂನ್ಯ)
{
#ಯದಿ defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    ಪೂರ್ಣಾಙ್ಕ	    idx_srr;
    ಪೂರ್ಣಾಙ್ಕ	    do_srr;
# ifdef FEAT_QUICKFIX
    ಪೂರ್ಣಾಙ್ಕ	    idx_sp;
    ಪೂರ್ಣಾಙ್ಕ	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    ಯದಿ (idx_srr < 0)
	do_srr = FALSE;
    ಅನ್ಯಥಾ
	do_srr = !(options[idx_srr].flags & P_WAS_SET);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    ಯದಿ (idx_sp < 0)
	do_sp = FALSE;
    ಅನ್ಯಥಾ
	do_sp = !(options[idx_sp].flags & P_WAS_SET);
# endif
    p = get_isolated_shell_name();
    ಯದಿ (p != ರಿಕ್ತ)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	ಯದಿ (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# ಯದಿ defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# ಯದಿ defined(FEAT_QUICKFIX)
	    ಯದಿ (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  ಅನ್ಯಥಾ
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    ಯದಿ (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	ಅನ್ಯಥಾ ಯದಿ (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  ಯದಿ defined(FEAT_QUICKFIX)
		ಯದಿ (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		ಯದಿ (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	ಅನ್ಯಥಾ
	    // Always use POSIX shell style redirection if we reach this
	    ಯದಿ (       fnamecmp(p, "sh") == 0
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
# ಯದಿ defined(FEAT_QUICKFIX)
		ಯದಿ (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  ಅನ್ಯಥಾ
		    ಯದಿ (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    ಅನ್ಯಥಾ
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		ಯದಿ (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#endif

#ಯದಿ defined(MSWIN)
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
    ಯದಿ (strstr((ವರ್ಣ *)gettail(p_sh), "powershell") != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	ಯದಿ (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	ಯದಿ (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (strstr((ವರ್ಣ *)gettail(p_sh), "sh") != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	idx3;

	idx3 = findoption((char_u *)"shcf");
	ಯದಿ (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	ಯದಿ (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (strstr((ವರ್ಣ *)gettail(p_sh), "cmd.exe") != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	idx3;

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
	ಯದಿ (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	ಯದಿ (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#endif

    ಯದಿ (BUFEMPTY())
    {
	ಪೂರ್ಣಾಙ್ಕ idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	ಯದಿ (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_SET))
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    set_title_defaults();
}

#ಯದಿ defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    ಶೂನ್ಯ
set_helplang_default(char_u *lang)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    size_t	langlen;

    ಯದಿ (lang == ರಿಕ್ತ)	// safety check
	ಪ್ರತ್ಯಾಗಮ;

    langlen = STRLEN(lang);
    ಯದಿ (langlen < 2)	// safety check
	ಪ್ರತ್ಯಾಗಮ;

    idx = findoption((char_u *)"hlg");
    ಯದಿ (idx < 0 || (options[idx].flags & P_WAS_SET))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    ಯದಿ (p_hlg == ರಿಕ್ತ)
	p_hlg = empty_option;
    ಅನ್ಯಥಾ
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	ಯದಿ (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	ಅನ್ಯಥಾ ಯದಿ (langlen >= 1 && *p_hlg == 'C')
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
    ಶೂನ್ಯ
set_title_defaults(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    idx1;
    long    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    ಯದಿ (idx1 >= 0 && !(options[idx1].flags & P_WAS_SET))
    {
#ifdef FEAT_GUI
	ಯದಿ (gui.starting || gui.in_use)
	    val = TRUE;
	ಅನ್ಯಥಾ
#endif
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    ಯದಿ (idx1 < 0 || (options[idx1].flags & P_WAS_SET))
    {
	ಪ್ರತ್ಯಾಗಮ;
    }

#ifdef FEAT_GUI
    ಯದಿ (gui.starting || gui.in_use)
	val = TRUE;
    ಅನ್ಯಥಾ
#endif
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    ಶೂನ್ಯ
ex_set(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;

    ಯದಿ (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#ಯದಿ defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    ಯದಿ ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    ಅನ್ಯಥಾ
#endif
    {
	ಯದಿ (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(ಶೂನ್ಯ)do_set(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    PREFIX_NO = 0,	// "no" prefix
    PREFIX_NONE,	// no prefix
    PREFIX_INV,		// "inv" prefix
} set_prefix_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    ಸ್ಥಿರ set_prefix_T
get_option_prefix(char_u **argp)
{
    ಪೂರ್ಣಾಙ್ಕ		prefix = PREFIX_NONE;
    char_u	*arg = *argp;

    ಯದಿ (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	prefix = PREFIX_NO;
	arg += 2;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "inv", 3) == 0)
    {
	prefix = PREFIX_INV;
	arg += 3;
    }

    *argp = arg;
    ಪ್ರತ್ಯಾಗಮ prefix;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_option_name(char_u *arg, ಪೂರ್ಣಾಙ್ಕ *opt_idxp, ಪೂರ್ಣಾಙ್ಕ *lenp, ಪೂರ್ಣಾಙ್ಕ *keyp)
{
    ಪೂರ್ಣಾಙ್ಕ	ಕೀಲಿ = 0;
    ಪೂರ್ಣಾಙ್ಕ	len;
    ಪೂರ್ಣಾಙ್ಕ	opt_idx;

    ಯದಿ (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	ಯದಿ (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	ಅನ್ಯಥಾ
	{
	    len = 1;
	    ಯಾವತ್ (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	ಯದಿ (arg[len] != '>')
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	arg[len] = NUL;			    // put NUL after name
	ಯದಿ (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	ಯದಿ (opt_idx == -1)
	    ಕೀಲಿ = find_ಕೀಲಿ_option(arg + 1, TRUE);
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	ಯದಿ (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	ಅನ್ಯಥಾ
	    ಯಾವತ್ (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	ಯದಿ (opt_idx == -1)
	    ಕೀಲಿ = find_ಕೀಲಿ_option(arg, FALSE);
    }

    *keyp = ಕೀಲಿ;
    *lenp = len;
    *opt_idxp = opt_idx;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    ಸ್ಥಿರ set_op_T
get_opt_op(char_u *arg)
{
    set_op_T op = OP_NONE;

    ಯದಿ (*arg != NUL && *(arg + 1) == '=')
    {
	ಯದಿ (*arg == '+')
	    op = OP_ADDING;		// "+="
	ಅನ್ಯಥಾ ಯದಿ (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	ಅನ್ಯಥಾ ಯದಿ (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    ಪ್ರತ್ಯಾಗಮ op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
ಸ್ಥಿರ ವರ್ಣ *modeline_whitelist[] =
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
    ರಿಕ್ತ
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    ಸ್ಥಿರ bool
is_modeline_whitelisted(ವರ್ಣ *name)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; modeline_whitelist[i] != ರಿಕ್ತ; i++)
	ಯದಿ (STRCMP(name, modeline_whitelist[i]) == 0)
	    ಪ್ರತ್ಯಾಗಮ true;
    ಪ್ರತ್ಯಾಗಮ false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
validate_opt_idx(
	ಪೂರ್ಣಾಙ್ಕ opt_idx,
	ಪೂರ್ಣಾಙ್ಕ opt_flags,
	long_u flags,
	ವರ್ಣ **errmsg,
	set_prefix_T prefix)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    ಯದಿ ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    ಯದಿ ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Disallow changing some options from modelines.
    ಯದಿ (opt_flags & OPT_MODELINE)
    {
	ಯದಿ (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_in_modeline;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_in_modeline_when_modelineexpr_is_off;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	ಯದಿ (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    ಯದಿ (options[opt_idx].indir == PV_ML && prefix == PREFIX_NO)
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಅನ್ಯಥಾ ಯದಿ (!is_modeline_whitelisted(options[opt_idx].fullname))
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
#ifdef FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	ಯದಿ (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    ಪ್ರತ್ಯಾಗಮ FAIL;
#endif
    }

#ifdef HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    ಯದಿ (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_in_sandbox;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
#endif

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    ಸ್ಥಿರ char_u *
stropt_get_default_val(
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,
    char_u	*varp,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    ಪೂರ್ಣಾಙ್ಕ		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    ಯದಿ ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ifdef FEAT_GUI
	ಯದಿ (gui.in_use)
	    newval = gui_bg_default();
	ಅನ್ಯಥಾ
#endif
	    newval = term_bg_default();
    }
    ಅನ್ಯಥಾ ಯದಿ ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    ಯದಿ (newval == ರಿಕ್ತ)
	newval = empty_option;
    ಅನ್ಯಥಾ
    {
	char_u *s = option_expand(opt_idx, newval);
	ಯದಿ (s == ರಿಕ್ತ)
	    s = newval;
	newval = vim_strsave(s);
    }

    ಪ್ರತ್ಯಾಗಮ newval;
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
    ಪೂರ್ಣಾಙ್ಕ		i = getdigits((char_u **)varp);

    ವಿಕಲ್ಪ (i)
    {
	ಪಕ್ಷ 0:
	    *(char_u **)varp = empty_option;
	    ಭಙ್ಗ;
	ಪಕ್ಷ 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    ಭಙ್ಗ;
	ಪಕ್ಷ 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    ಭಙ್ಗ;
	ಪಕ್ಷ 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    ಭಙ್ಗ;
    }
    vim_free(*oldval_p);
    ಯದಿ (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    ಯದಿ (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    ಯದಿ (*origval_g_p == *oldval_p)
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
    ಪೂರ್ಣಾಙ್ಕ i = getdigits(argp);
    ಯದಿ (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    ಯದಿ (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    ಯದಿ (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    ಯದಿ (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    ಯದಿ (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    ಯದಿ (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    ಪ್ರತ್ಯಾಗಮ whichwrap;
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
    set_op_T	op,
    ಪೂರ್ಣಾಙ್ಕ		flags UNUSED)
{
    char_u	*arg = *argp;
    unsigned	newlen;
    char_u	*newval;
    char_u	*s = ರಿಕ್ತ;

    // get a bit too much
    newlen = (unsigned)STRLEN(arg) + 1;
    ಯದಿ (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;
    newval = alloc(newlen);
    ಯದಿ (newval == ರಿಕ್ತ)  // out of mem, don't change
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    ಯಾವತ್ (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	ಪೂರ್ಣಾಙ್ಕ i;

	ಯದಿ (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#endif
	   )
	    ++arg;	// remove backslash
	ಯದಿ (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	ಅನ್ಯಥಾ
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    ಪ್ರತ್ಯಾಗಮ newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    ಸ್ಥಿರ char_u *
stropt_expand_envvar(
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,
    char_u	*origval,
    char_u	*newval,
    set_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    ಯದಿ (s == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ newval;

    vim_free(newval);
    unsigned newlen = (unsigned)STRLEN(s) + 1;
    ಯದಿ (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;

    newval = alloc(newlen);
    ಯದಿ (newval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    STRCPY(newval, s);

    ಪ್ರತ್ಯಾಗಮ newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    ಪೂರ್ಣಾಙ್ಕ		len = 0;

    ಪೂರ್ಣಾಙ್ಕ comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    ಯದಿ (op == OP_ADDING)
    {
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(origval);
	// strip a trailing comma, would get 2
	ಯದಿ (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    ಅನ್ಯಥಾ
    {
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    ಯದಿ (comma)
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
    ಪೂರ್ಣಾಙ್ಕ		flags,
    char_u	*strval,
    ಪೂರ್ಣಾಙ್ಕ		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    ಯದಿ (*strval)
    {
	// may need to remove a comma
	ಯದಿ (flags & P_COMMA)
	{
	    ಯದಿ (strval == origval)
	    {
		// include comma after string
		ಯದಿ (strval[len] == ',')
		    ++len;
	    }
	    ಅನ್ಯಥಾ
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
find_ಕೀಲಿ_item(char_u *src, char_u *ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ keylen, ಪೂರ್ಣಾಙ್ಕ *itemlenp)
{
    char_u  *p = src;

    ಯಾವತ್ (*p != NUL)
    {
	// Check if this item starts with the same key
	ಯದಿ ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, ಕೀಲಿ, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *end = vim_strchr(p, ',');
	    ಯದಿ (end == ರಿಕ್ತ)
		end = p + STRLEN(p);
	    *itemlenp = (ಪೂರ್ಣಾಙ್ಕ)(end - p);
	    ಪ್ರತ್ಯಾಗಮ p;
	}
	++p;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_comma_item(char_u *str, char_u *item, ಪೂರ್ಣಾಙ್ಕ itemlen)
{
    ಯದಿ (item[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(item, item + itemlen + 1);
    ಅನ್ಯಥಾ ಯದಿ (item > str && *(item - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(item - 1, item + itemlen);
    ಅನ್ಯಥಾ
	// Only item
	*item = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_ಕೀಲಿ_item(char_u *str, char_u *ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ keylen, char_u *skip)
{
    ಪೂರ್ಣಾಙ್ಕ	    itemlen;
    char_u  *found;

    ಯಾವತ್ ((found = find_ಕೀಲಿ_item(str, ಕೀಲಿ, keylen, &itemlen)) != ರಿಕ್ತ)
    {
	ಯದಿ (found == skip)
	{
	    // Search for the next match after this one.
	    char_u *next = found + itemlen;
	    ಯದಿ (*next == ',')
		++next;
	    found = find_ಕೀಲಿ_item(next, ಕೀಲಿ, keylen, &itemlen);
	    ಯದಿ (found == ರಿಕ್ತ)
		ಭಙ್ಗ;
	}

	remove_comma_item(str, found, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
append_item(char_u *str, char_u *item, ಪೂರ್ಣಾಙ್ಕ item_len)
{
    ಪೂರ್ಣಾಙ್ಕ len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str);

    ಯದಿ (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, item, (size_t)item_len);
    str[len + item_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prepend_item(char_u *str, char_u *item, ಪೂರ್ಣಾಙ್ಕ item_len)
{
    ಪೂರ್ಣಾಙ್ಕ len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str);
    ಪೂರ್ಣಾಙ್ಕ comma = (len > 0) ? 1 : 0;

    mch_memmove(str + item_len + comma, str, (size_t)len + 1);
    mch_memmove(str, item, (size_t)item_len);
    ಯದಿ (comma)
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
    ಸ್ಥಿರ bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    ಪೂರ್ಣಾಙ್ಕ		flags UNUSED)
{
    char_u  *p;
    char_u  *item_start;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    ಯದಿ (vim_strchr(newval, ':') == ರಿಕ್ತ && vim_strchr(newval, ',') == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    ಯದಿ (newval_copy == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    item_start = newval_copy;
    ಪ್ರತಿ (;;)
    {
	p = vim_strchr(item_start, ',');
	ಪೂರ್ಣಾಙ್ಕ item_len = (p == ರಿಕ್ತ)
			    ? (ಪೂರ್ಣಾಙ್ಕ)STRLEN(item_start) : (ಪೂರ್ಣಾಙ್ಕ)(p - item_start);

	ಯದಿ (item_len > 0)
	{
	    char_u *colon = vim_strchr(item_start, ':');
	    ಯದಿ (colon != ರಿಕ್ತ && colon < item_start + item_len)
	    {
		ಪೂರ್ಣಾಙ್ಕ keylen = (ಪೂರ್ಣಾಙ್ಕ)(colon - item_start) + 1;

		ಯದಿ (op == OP_ADDING || op == OP_PREPENDING)
		{
		    ಪೂರ್ಣಾಙ್ಕ old_itemlen;
		    char_u *found = find_ಕೀಲಿ_item(newval, item_start,
						       keylen, &old_itemlen);
		    ಯದಿ (found != ರಿಕ್ತ)
		    {
			ಯದಿ (old_itemlen == item_len
				&& STRNCMP(found, item_start,
							 item_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_ಕೀಲಿ_item(newval, item_start,
							   keylen, found);
			}
			ಅನ್ಯಥಾ
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_ಕೀಲಿ_item(newval, item_start,
							    keylen, ರಿಕ್ತ);
			    ಯದಿ (op == OP_PREPENDING)
				prepend_item(newval, item_start, item_len);
			    ಅನ್ಯಥಾ
				append_item(newval, item_start, item_len);
			}
		    }
		    ಅನ್ಯಥಾ
		    {
			// New item.
			ಯದಿ (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			ಅನ್ಯಥಾ
			    append_item(newval, item_start, item_len);
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (op == OP_REMOVING)
		    remove_ಕೀಲಿ_item(newval, item_start, keylen, ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    ಯದಿ (found == ರಿಕ್ತ)
		    {
			// New item.
			ಯದಿ (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			ಅನ್ಯಥಾ
			    append_item(newval, item_start, item_len);
		    }
		    // else: exact duplicate — do nothing
		}
		ಅನ್ಯಥಾ ಯದಿ (op == OP_REMOVING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    ಯದಿ (found != ರಿಕ್ತ)
			remove_comma_item(newval, found, item_len);
		}
	    }
	}

	ಯದಿ (p == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	item_start = p + 1;
    }

    vim_free(newval_copy);

    ಪ್ರತ್ಯಾಗಮ true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
stropt_remove_dupflags(char_u *newval, ಪೂರ್ಣಾಙ್ಕ flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    ಯಾವತ್ (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	ಯದಿ (flags & P_ONECOMMA)
	{
	    ಯದಿ (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != ರಿಕ್ತ)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		ಅನುವರ್ತ;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != ರಿಕ್ತ)
	    {
		STRMOVE(s, s + 1);
		ಅನುವರ್ತ;
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
    ಪೂರ್ಣಾಙ್ಕ		nextchar,
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    set_op_T    *op_arg,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    ಪೂರ್ಣಾಙ್ಕ		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    set_op_T    op = *op_arg;
    char_u	*save_arg = ರಿಕ್ತ;
    char_u	*newval;
    char_u	*s = ರಿಕ್ತ;
    char_u	whichwrap[80];

    ಯದಿ (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    ಅನ್ಯಥಾ ಯದಿ (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    ಅನ್ಯಥಾ
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	ಯದಿ (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	ಅನ್ಯಥಾ ಯದಿ (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	ಅನ್ಯಥಾ ಯದಿ (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	ಅನ್ಯಥಾ ಯದಿ (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_ಮೌಲ್ಯ(origval, &arg, op, flags);
	ಯದಿ (newval == ರಿಕ್ತ)
	    ಗಚ್ಛ done;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	ಯದಿ (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    ಯದಿ (newval == ರಿಕ್ತ)
		ಗಚ್ಛ done;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	ಯದಿ ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	ಅನ್ಯಥಾ
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    ಪೂರ್ಣಾಙ್ಕ len = 0;
	    ಯದಿ (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(newval);
		s = find_dup_item(origval, newval, len, flags);

		// do not add if already there
		ಯದಿ ((op == OP_ADDING || op == OP_PREPENDING) && s != ರಿಕ್ತ)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		ಯದಿ (s == ರಿಕ್ತ)
		    s = origval + (ಪೂರ್ಣಾಙ್ಕ)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    ಯದಿ (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    ಅನ್ಯಥಾ ಯದಿ (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	ಯದಿ (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

done:
    ಯದಿ (save_arg != ರಿಕ್ತ)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    ಪ್ರತ್ಯಾಗಮ newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
do_set_option_string(
	ಪೂರ್ಣಾಙ್ಕ	    opt_idx,
	ಪೂರ್ಣಾಙ್ಕ	    opt_flags,
	char_u	    **argp,
	ಪೂರ್ಣಾಙ್ಕ	    nextchar,
	set_op_T    op_arg,
	long_u	    flags,
	ಪೂರ್ಣಾಙ್ಕ	    cp_val,
	char_u	    *varp_arg,
	ವರ್ಣ	    *errbuf,
	size_t	    errbuflen,
	ಪೂರ್ಣಾಙ್ಕ	    *ಮೌಲ್ಯ_checked,
	ವರ್ಣ	    **errmsg)
{
    char_u	*arg = *argp;
    set_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = ರಿಕ್ತ; // previous value if *varp
    char_u	*newval;
    char_u	*origval = ರಿಕ್ತ;
    char_u	*origval_l = ರಿಕ್ತ;
    char_u	*origval_g = ರಿಕ್ತ;
#ಯದಿ defined(FEAT_EVAL)
    char_u	*saved_origval = ರಿಕ್ತ;
    char_u	*saved_origval_l = ರಿಕ್ತ;
    char_u	*saved_origval_g = ರಿಕ್ತ;
    char_u	*saved_newval = ರಿಕ್ತ;
#endif

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    ಯದಿ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((ಪೂರ್ಣಾಙ್ಕ)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    ಯದಿ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	ಯದಿ (((ಪೂರ್ಣಾಙ್ಕ)options[opt_idx].indir & PV_BOTH)
						  && origval_l == empty_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    ಯದಿ (((ಪೂರ್ಣಾಙ್ಕ)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    ಅನ್ಯಥಾ
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    ಯದಿ (newval == ರಿಕ್ತ)
	*(char_u **)(varp) = empty_option;

#ಯದಿ defined(FEAT_EVAL)
    ಯದಿ (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_ಕೀಲಿ
# endif
		      && origval != ರಿಕ್ತ && newval != ರಿಕ್ತ)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	ಯದಿ (origval_l != ರಿಕ್ತ)
	    saved_origval_l = vim_strsave(origval_l);
	ಯದಿ (origval_g != ರಿಕ್ತ)
	    saved_origval_g = vim_strsave(origval_g);
    }
#endif

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	ಪೂರ್ಣಾಙ್ಕ	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	ಯದಿ ((opt_flags & OPT_MODELINE)
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
			errbuflen, opt_flags, op, ಮೌಲ್ಯ_checked);

	secure = secure_saved;
    }

#ಯದಿ defined(FEAT_EVAL)
    ಯದಿ (*errmsg == ರಿಕ್ತ)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#endif

    *argp = arg;
    ಪ್ರತ್ಯಾಗಮ *errmsg == ರಿಕ್ತ ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    ಸ್ಥಿರ ವರ್ಣ *
do_set_option_bool(
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,
    ಪೂರ್ಣಾಙ್ಕ		opt_flags,
    set_prefix_T prefix,
    long_u	flags,
    char_u	*varp,
    ಪೂರ್ಣಾಙ್ಕ		nextchar,
    ಪೂರ್ಣಾಙ್ಕ		afterchar,
    ಪೂರ್ಣಾಙ್ಕ		cp_val)

{
    varnumber_T	ಮೌಲ್ಯ;

    ಯದಿ (nextchar == '=' || nextchar == ':')
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    ಯದಿ (opt_idx < 0 || varp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    ಯದಿ (nextchar == '!')
	ಮೌಲ್ಯ = *(ಪೂರ್ಣಾಙ್ಕ *)(varp) ^ 1;
    ಅನ್ಯಥಾ ಯದಿ (nextchar == '&')
	ಮೌಲ್ಯ = (ಪೂರ್ಣಾಙ್ಕ)(long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    ಅನ್ಯಥಾ ಯದಿ (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    ಮೌಲ್ಯ = -1;
	ಅನ್ಯಥಾ ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    ಮೌಲ್ಯ = -1;
#ifdef HAVE_FSYNC
	ಅನ್ಯಥಾ ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    ಮೌಲ್ಯ = -1;
#endif
	ಅನ್ಯಥಾ
	    ಮೌಲ್ಯ = *(ಪೂರ್ಣಾಙ್ಕ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    ಅನ್ಯಥಾ
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	ಯದಿ (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    ಪ್ರತ್ಯಾಗಮ e_trailing_characters;
	ಯದಿ (prefix == PREFIX_INV)
	    ಮೌಲ್ಯ = *(ಪೂರ್ಣಾಙ್ಕ *)(varp) ^ 1;
	ಅನ್ಯಥಾ
	    ಮೌಲ್ಯ = prefix == PREFIX_NO ? 0 : 1;
    }

    ಪ್ರತ್ಯಾಗಮ set_bool_option(opt_idx, varp, (ಪೂರ್ಣಾಙ್ಕ)ಮೌಲ್ಯ, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    ಸ್ಥಿರ ವರ್ಣ *
do_set_option_numeric(
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,
    ಪೂರ್ಣಾಙ್ಕ		opt_flags,
    char_u	**argp,
    ಪೂರ್ಣಾಙ್ಕ		nextchar,
    set_op_T	op,
    long_u	flags,
    ಪೂರ್ಣಾಙ್ಕ		cp_val,
    char_u	*varp,
    ವರ್ಣ	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಙ್ಕ			i;
    ವರ್ಣ		*errmsg = ರಿಕ್ತ;

    ಯದಿ (opt_idx < 0 || varp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;  // "cannot happen"
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
    ಯದಿ (nextchar == '&')
	ಮೌಲ್ಯ = (long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    ಅನ್ಯಥಾ ಯದಿ (nextchar == '<')
    {
	ಯದಿ ((long *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    ಮೌಲ್ಯ = NO_LOCAL_UNDOLEVEL;
	ಅನ್ಯಥಾ ಯದಿ (opt_flags == OPT_LOCAL
		    && ((long *)varp == &curwin->w_p_siso
		     || (long *)varp == &curwin->w_p_so
		     || (long *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    ಮೌಲ್ಯ = -1;
	ಅನ್ಯಥಾ
	    ಮೌಲ್ಯ = *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    ಅನ್ಯಥಾ ಯದಿ (((long *)varp == &p_wc || (long *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	ಮೌಲ್ಯ = string_to_ಕೀಲಿ(arg, FALSE);
	ಯದಿ (ಮೌಲ್ಯ == 0)
	{
	    errmsg = e_invalid_argument;
	    ಗಚ್ಛ skip;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, ರಿಕ್ತ, &i, STR2NR_ALL, &ಮೌಲ್ಯ, ರಿಕ್ತ, 0, TRUE, ರಿಕ್ತ);
	ಯದಿ (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_ಸಂಖ್ಯೆ_required_after_equal;
	    ಗಚ್ಛ skip;
	}
    }
    ಅನ್ಯಥಾ
    {
	errmsg = e_ಸಂಖ್ಯೆ_required_after_equal;
	ಗಚ್ಛ skip;
    }

    ಯದಿ (op == OP_ADDING)
	ಮೌಲ್ಯ = *(long *)varp + ಮೌಲ್ಯ;
    ಅನ್ಯಥಾ ಯದಿ (op == OP_PREPENDING)
	ಮೌಲ್ಯ = *(long *)varp * ಮೌಲ್ಯ;
    ಅನ್ಯಥಾ ಯದಿ (op == OP_REMOVING)
	ಮೌಲ್ಯ = *(long *)varp - ಮೌಲ್ಯ;

    ಯದಿ ((long *)varp == &curwin->w_p_sop && ಮೌಲ್ಯ < -1)
    {
	errmsg = e_invalid_argument;
	ಗಚ್ಛ skip;
    }

    errmsg = set_ಅಂಕೆ_option(opt_idx, varp, ಮೌಲ್ಯ, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    ಸ್ಥಿರ ವರ್ಣ *
do_set_option_keycode(char_u **argp, char_u *ಕೀಲಿ_name, ಪೂರ್ಣಾಙ್ಕ nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    ಯದಿ (nextchar == '&')
    {
	ಯದಿ (add_termcap_entry(ಕೀಲಿ_name, TRUE) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ e_not_found_in_termcap;
    }
    ಅನ್ಯಥಾ
    {
	++arg; // jump to after the '=' or ':'
	ಪ್ರತಿ (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    ಯದಿ (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	add_termcode(ಕೀಲಿ_name, arg, FALSE);
	*p = nextchar;
    }
    ಯದಿ (full_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Set an option to a new value.
 */
    ಸ್ಥಿರ ವರ್ಣ *
do_set_option_ಮೌಲ್ಯ(
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,
    ಪೂರ್ಣಾಙ್ಕ		opt_flags,
    char_u	**argp,
    set_prefix_T prefix,
    set_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*ಕೀಲಿ_name,
    ಪೂರ್ಣಾಙ್ಕ		nextchar,
    ಪೂರ್ಣಾಙ್ಕ		afterchar,
    ಪೂರ್ಣಾಙ್ಕ		cp_val,
    ಪೂರ್ಣಾಙ್ಕ		*stopopteval,
    ವರ್ಣ	*errbuf,
    size_t	errbuflen)
{
    ಪೂರ್ಣಾಙ್ಕ		ಮೌಲ್ಯ_checked = FALSE;
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;
    char_u	*arg = *argp;

    ಯದಿ (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_set_option_bool(opt_idx, opt_flags, prefix, flags, varp,
						nextchar, afterchar, cp_val);
	ಯದಿ (errmsg != ರಿಕ್ತ)
	    ಗಚ್ಛ skip;
    }
    ಅನ್ಯಥಾ
    {
	// numeric or string option
	ಯದಿ (vim_strchr((char_u *)"=:&<", nextchar) == ರಿಕ್ತ
		|| prefix != PREFIX_NONE)
	{
	    errmsg = e_invalid_argument;
	    ಗಚ್ಛ skip;
	}

	ಯದಿ (flags & P_ಅಂಕೆ)
	{
	    // numeric option
	    errmsg = do_set_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    ಯದಿ (errmsg != ರಿಕ್ತ)
		ಗಚ್ಛ skip;
	}
	ಅನ್ಯಥಾ ಯದಿ (opt_idx >= 0)
	{
	    // string option
	    ಯದಿ (do_set_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&ಮೌಲ್ಯ_checked, &errmsg) == FAIL)
	    {
		ಯದಿ (errmsg != ರಿಕ್ತ)
		    ಗಚ್ಛ skip;
		*stopopteval = TRUE;
		ಗಚ್ಛ skip;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // key code option
	    errmsg = do_set_option_keycode(&arg, ಕೀಲಿ_name, nextchar);
	    ಯದಿ (errmsg != ರಿಕ್ತ)
		ಗಚ್ಛ skip;
	}
    }

    ಯದಿ (opt_idx >= 0)
	did_set_option(opt_idx, opt_flags, op == OP_NONE, ಮೌಲ್ಯ_checked);

skip:
    *argp = arg;
    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    ಸ್ಥಿರ ವರ್ಣ *
do_set_option(
    ಪೂರ್ಣಾಙ್ಕ		opt_flags,
    char_u	**argp,
    char_u	*arg_start,
    char_u	**startarg,
    ಪೂರ್ಣಾಙ್ಕ		*did_show,
    ಪೂರ್ಣಾಙ್ಕ		*stopopteval,
    ವರ್ಣ	*errbuf,
    size_t	errbuflen)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;
    char_u	*arg;
    set_prefix_T prefix;	// no prefix, "no" prefix or "inv" prefix
    set_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	ಕೀಲಿ_name[2];
    ಪೂರ್ಣಾಙ್ಕ		nextchar;	// next non-white char after option name
    ಪೂರ್ಣಾಙ್ಕ		afterchar;	// character just after option name
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ಕೀಲಿ;
    ಪೂರ್ಣಾಙ್ಕ		len;

    prefix = get_option_prefix(argp);
    arg = *argp;

    // find end of name
    ಕೀಲಿ = 0;
    ಯದಿ (parse_option_name(arg, &opt_idx, &len, &ಕೀಲಿ) == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    // remember character after option name
    afterchar = arg[len];

    ಯದಿ (in_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	ಯದಿ (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != ರಿಕ್ತ
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    ಗಚ್ಛ skip;
	}
    }
    ಅನ್ಯಥಾ
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	ಯಾವತ್ (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    ಯದಿ (op != OP_NONE)
	len++;

    nextchar = arg[len];

    ಯದಿ (opt_idx == -1 && ಕೀಲಿ == 0)	// found a mismatch: skip
    {
	ಯದಿ (in_vim9script() && arg > arg_start
		&& vim_strchr((char_u *)"!&<", *arg) != ರಿಕ್ತ)
	    errmsg = e_no_white_space_allowed_between_option_and;
	ಅನ್ಯಥಾ
	    errmsg = e_unknown_option;
	ಗಚ್ಛ skip;
    }

    ಯದಿ (opt_idx >= 0)
    {
	ಯದಿ (options[opt_idx].var == ರಿಕ್ತ)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    ಯದಿ (vim_strchr((char_u *)"=:!&<", nextchar) == ರಿಕ್ತ
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    ಗಚ್ಛ skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    ಅನ್ಯಥಾ
    {
	flags = P_STRING;
	varp = ರಿಕ್ತ;
	ಯದಿ (ಕೀಲಿ < 0)
	{
	    ಕೀಲಿ_name[0] = KEY2TERMCAP0(ಕೀಲಿ);
	    ಕೀಲಿ_name[1] = KEY2TERMCAP1(ಕೀಲಿ);
	}
	ಅನ್ಯಥಾ
	{
	    ಕೀಲಿ_name[0] = KS_ಕೀಲಿ;
	    ಕೀಲಿ_name[1] = (ಕೀಲಿ & 0xff);
	}
    }

    // Make sure the option value can be changed.
    ಯದಿ (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, prefix) == FAIL)
	ಗಚ್ಛ skip;

    ಪೂರ್ಣಾಙ್ಕ cp_val = p_cp;
    ಯದಿ (vim_strchr((char_u *)"?=:!&<", nextchar) != ರಿಕ್ತ)
    {
	arg += len;
	ಯದಿ (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    ಯದಿ (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    ಅನ್ಯಥಾ		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	ಯದಿ (vim_strchr((char_u *)"?!&<", nextchar) != ರಿಕ್ತ
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    ಗಚ್ಛ skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    ಯದಿ (nextchar == '?'
	    || (prefix == PREFIX_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == ರಿಕ್ತ
		&& !(flags & P_BOOL)))
    {
	// print value
	ಯದಿ (*did_show)
	    ಸಂದೇಶ_putchar('\n');	    // cursor below last one
	ಅನ್ಯಥಾ
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	ಯದಿ (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ifdef FEAT_EVAL
	    ಯದಿ (p_verbose > 0)
	    {
		// Mention where the option was last set.
		ಯದಿ (varp == options[opt_idx].var)
		    last_set_ಸಂದೇಶ(options[opt_idx].script_ctx);
		ಅನ್ಯಥಾ ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)options[opt_idx].indir & PV_WIN)
		    last_set_ಸಂದೇಶ(curwin->w_p_script_ctx[
			    (ಪೂರ್ಣಾಙ್ಕ)options[opt_idx].indir & PV_MASK]);
		ಅನ್ಯಥಾ ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)options[opt_idx].indir & PV_BUF)
		    last_set_ಸಂದೇಶ(curbuf->b_p_script_ctx[
			    (ಪೂರ್ಣಾಙ್ಕ)options[opt_idx].indir & PV_MASK]);
	    }
#endif
	}
	ಅನ್ಯಥಾ
	{
	    char_u	    *p;

	    p = find_termcode(ಕೀಲಿ_name);
	    ಯದಿ (p == ರಿಕ್ತ)
	    {
		errmsg = e_ಕೀಲಿ_code_not_set;
		ಗಚ್ಛ skip;
	    }
	    ಅನ್ಯಥಾ
		(ಶೂನ್ಯ)show_one_termcode(ಕೀಲಿ_name, p, TRUE);
	}
	ಯದಿ (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    ಅನ್ಯಥಾ
    {
	errmsg = do_set_option_ಮೌಲ್ಯ(opt_idx, opt_flags, &arg, prefix, op,
					flags, varp, ಕೀಲಿ_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    ಪ್ರತ್ಯಾಗಮ errmsg;
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
    ಪೂರ್ಣಾಙ್ಕ
do_set(
    char_u	*arg_start,	// option string (may be written to!)
    ಪೂರ್ಣಾಙ್ಕ		opt_flags)
{
    char_u	*arg = arg_start;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		did_show = FALSE;   // already showed one value

    ಯದಿ (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	ಗಚ್ಛ theend;
    }

    ಯಾವತ್ (*arg != NUL)		// loop to process all options
    {
	ಯದಿ (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    ಯದಿ (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		set_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    ಅನ್ಯಥಾ
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ		stopopteval = FALSE;
	    ವರ್ಣ	*errmsg = ರಿಕ್ತ;
	    ವರ್ಣ	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_set_option(opt_flags, &arg, arg_start, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    ಯದಿ (stopopteval)
		ಭಙ್ಗ;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    ಪ್ರತಿ (i = 0; i < 2 ; ++i)
	    {
		ಯಾವತ್ (*arg != NUL && !VIM_ISWHITE(*arg))
		    ಯದಿ (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		ಯದಿ (*arg != '=')
		    ಭಙ್ಗ;
	    }

	    ಯದಿ (errmsg != ರಿಕ್ತ)
	    {
		i = vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		ಯದಿ (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((ವರ್ಣ *)IObuff);	// show error highlighted
		--no_wait_return;

		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    ಯದಿ (silent_mode && did_show)
    {
	// After displaying option values in silent mode.
	silent_mode = FALSE;
	info_message = TRUE;	// use mch_msg(), not mch_errmsg()
	ಸಂದೇಶ_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = TRUE;
	info_message = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    ಶೂನ್ಯ
did_set_option(
    ಪೂರ್ಣಾಙ್ಕ	    opt_idx,
    ಪೂರ್ಣಾಙ್ಕ	    opt_flags,	    // possibly with OPT_MODELINE
    ಪೂರ್ಣಾಙ್ಕ	    new_ಮೌಲ್ಯ_2,	    // value was replaced completely
    ಪೂರ್ಣಾಙ್ಕ	    ಮೌಲ್ಯ_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_SET;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    ಯದಿ (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    ಯದಿ (!ಮೌಲ್ಯ_checked && (secure
#ifdef HAVE_SANDBOX
	    || sandbox != 0
#endif
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	ಯದಿ (flagsp_local != ರಿಕ್ತ)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    ಅನ್ಯಥಾ ಯದಿ (new_ಮೌಲ್ಯ_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	ಯದಿ (flagsp_local != ರಿಕ್ತ)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    ಪೂರ್ಣಾಙ್ಕ
string_to_ಕೀಲಿ(char_u *arg, ಪೂರ್ಣಾಙ್ಕ multi_byte)
{
    ಯದಿ (*arg == '<' && arg[1])
	ಪ್ರತ್ಯಾಗಮ find_ಕೀಲಿ_option(arg + 1, TRUE);
    ಯದಿ (*arg == '^' && arg[1])
    {
	ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ = Ctrl_chr(arg[1]);
	ಯದಿ (ಕೀಲಿ == 0)		// ^@ is <Nul>
	    ಕೀಲಿ = K_ZERO;
	ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
    }
    ಯದಿ (multi_byte)
	ಪ್ರತ್ಯಾಗಮ PTR2CHAR(arg);
    ಪ್ರತ್ಯಾಗಮ *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    ಶೂನ್ಯ
did_set_title(ಶೂನ್ಯ)
{
    ಯದಿ (starting != NO_SCREEN
#ifdef FEAT_GUI
	    && !gui.starting
#endif
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    ಶೂನ್ಯ
set_options_bin(
    ಪೂರ್ಣಾಙ್ಕ		oldval,
    ಪೂರ್ಣಾಙ್ಕ		newval,
    ಪೂರ್ಣಾಙ್ಕ		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    ಯದಿ (newval)
    {
	ಯದಿ (!oldval)		// switched on
	{
	    ಯದಿ (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    ಯದಿ (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	ಯದಿ (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	ಯದಿ (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (oldval)		// switched off
    {
	ಯದಿ (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	ಯದಿ (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#ಯದಿ defined(FEAT_EVAL)
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
    ಸ್ಥಿರ char_u *
option_expand(ಪೂರ್ಣಾಙ್ಕ opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    ಯದಿ (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    ಯದಿ (val != ರಿಕ್ತ && STRLEN(val) > MAXPATHL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (val == ರಿಕ್ತ)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    ಪೂರ್ಣಾಙ್ಕ esc = var == &p_tags || var == &p_path;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : ರಿಕ್ತ, FALSE,
#ifdef FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#endif
				  ರಿಕ್ತ);
    ಯದಿ (STRCMP(NameBuff, val) == 0)   // they are the same
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ NameBuff;
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

#ifdef FEAT_SPELL
    (ಶೂನ್ಯ)spell_check_msm();
    (ಶೂನ್ಯ)spell_check_sps();
    (ಶೂನ್ಯ)compile_cap_prog(curwin->w_s);
    (ಶೂನ್ಯ)did_set_spell_option();
#endif
    // set cedit_key
    (ಶೂನ್ಯ)did_set_cedit(ರಿಕ್ತ);
#ifdef FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_set_breakat(ರಿಕ್ತ);
#endif
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
didset_options2(ಶೂನ್ಯ)
{
    // Initialize the highlight_attr[] table.
    (ಶೂನ್ಯ)highlight_changed();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (ಶೂನ್ಯ)set_listchars_option(curwin, curwin->w_p_lcs, TRUE, ರಿಕ್ತ, 0);

    // Parse default for 'fillchars'.
    (ಶೂನ್ಯ)set_fillchars_option(curwin, curwin->w_p_fcs, TRUE, ರಿಕ್ತ, 0);

#ifdef FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (ಶೂನ್ಯ)did_set_clipboard(ರಿಕ್ತ);
#endif
#ifdef FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (ಶೂನ್ಯ)tabstop_set(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (ಶೂನ್ಯ)tabstop_set(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#endif
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    ಶೂನ್ಯ
check_options(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;

    ಪ್ರತಿ (opt_idx = 0; options[opt_idx].fullname != ರಿಕ್ತ; opt_idx++)
	ಯದಿ ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != ರಿಕ್ತ)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_term_opt_idx(char_u **p)
{
    ಪೂರ್ಣಾಙ್ಕ opt_idx;

    ಪ್ರತಿ (opt_idx = 1; options[opt_idx].fullname != ರಿಕ್ತ; opt_idx++)
	ಯದಿ (options[opt_idx].var == (char_u *)p)
	    ಪ್ರತ್ಯಾಗಮ opt_idx;
    ಪ್ರತ್ಯಾಗಮ -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_term_option_alloced(char_u **p)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx = get_term_opt_idx(p);

    ಯದಿ (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    ಪ್ರತ್ಯಾಗಮ opt_idx;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    ಪೂರ್ಣಾಙ್ಕ
was_set_insecurely(win_T *wp, char_u *opt, ಪೂರ್ಣಾಙ್ಕ opt_flags)
{
    ಪೂರ್ಣಾಙ್ಕ	    idx = findoption(opt);
    long_u  *flagp;

    ಯದಿ (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	ಪ್ರತ್ಯಾಗಮ (*flagp & P_INSECURE) != 0;
    }
    internal_error("was_set_insecurely()");
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    ಸ್ಥಿರ long_u *
insecure_flag(win_T *wp, ಪೂರ್ಣಾಙ್ಕ opt_idx, ಪೂರ್ಣಾಙ್ಕ opt_flags)
{
    ಯದಿ (opt_flags & OPT_LOCAL)
	ವಿಕಲ್ಪ ((ಪೂರ್ಣಾಙ್ಕ)options[opt_idx].indir)
	{
	    ಪಕ್ಷ PV_WRAP:	ಪ್ರತ್ಯಾಗಮ &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    ಪಕ್ಷ PV_STL:	ಪ್ರತ್ಯಾಗಮ &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    ಪಕ್ಷ PV_FDE:	ಪ್ರತ್ಯಾಗಮ &wp->w_p_fde_flags;
	    ಪಕ್ಷ PV_FDT:	ಪ್ರತ್ಯಾಗಮ &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    ಪಕ್ಷ PV_BEXPR:	ಪ್ರತ್ಯಾಗಮ &wp->w_buffer->b_p_bexpr_flags;
#  endif
	    ಪಕ್ಷ PV_INDE:	ಪ್ರತ್ಯಾಗಮ &wp->w_buffer->b_p_inde_flags;
	    ಪಕ್ಷ PV_FEX:	ಪ್ರತ್ಯಾಗಮ &wp->w_buffer->b_p_fex_flags;
#  ifdef FEAT_FIND_ID
	    ಪಕ್ಷ PV_INEX:	ಪ್ರತ್ಯಾಗಮ &wp->w_buffer->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    ಪಕ್ಷ PV_CPT:	ಪ್ರತ್ಯಾಗಮ &wp->w_buffer->b_p_cpt_flags;
#  endif
# endif
	}
    ಅನ್ಯಥಾ
	// For global value of window-local options, use flags in w_allbuf_opt.
	ವಿಕಲ್ಪ ((ಪೂರ್ಣಾಙ್ಕ)options[opt_idx].indir)
	{
	    ಪಕ್ಷ PV_WRAP:	ಪ್ರತ್ಯಾಗಮ &wp->w_allbuf_opt.wo_wrap_flags;
# ಯದಿ defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    ಪಕ್ಷ PV_FDE:	ಪ್ರತ್ಯಾಗಮ &wp->w_allbuf_opt.wo_fde_flags;
	    ಪಕ್ಷ PV_FDT:	ಪ್ರತ್ಯಾಗಮ &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    ಪ್ರತ್ಯಾಗಮ &options[opt_idx].flags;
}
#endif

/*
 * Redraw the window title and/or tab page text later.
 */
    ಶೂನ್ಯ
redraw_titles(ಶೂನ್ಯ)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#ಯದಿ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    ಪೂರ್ಣಾಙ್ಕ
valid_name(char_u *val, ವರ್ಣ *allowed)
{
    char_u *s;

    ಪ್ರತಿ (s = val; *s != NUL; ++s)
	ಯದಿ (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    ಶೂನ್ಯ
set_option_sctx_idx(ಪೂರ್ಣಾಙ್ಕ opt_idx, ಪೂರ್ಣಾಙ್ಕ opt_flags, sctx_T script_ctx)
{
    ಪೂರ್ಣಾಙ್ಕ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    ಪೂರ್ಣಾಙ್ಕ		indir = (ಪೂರ್ಣಾಙ್ಕ)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    ಯದಿ (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    ಯದಿ (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    ಯದಿ (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    ಯದಿ (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    ಯದಿ (both || (opt_flags & OPT_LOCAL))
    {
	ಯದಿ (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	ಅನ್ಯಥಾ ಯದಿ (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	    ಯದಿ (both)
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
is_option_ಮೌಲ್ಯ_vim9(ಪೂರ್ಣಾಙ್ಕ opt_idx, ಪೂರ್ಣಾಙ್ಕ opt_flags)
{
    ಪೂರ್ಣಾಙ್ಕ		indir = (ಪೂರ್ಣಾಙ್ಕ)options[opt_idx].indir;
    sctx_T	*sctx = ರಿಕ್ತ;

    ಯದಿ ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    ಯದಿ ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	ಯದಿ (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_MASK];
	ಅನ್ಯಥಾ ಯದಿ (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_MASK];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    ಪ್ರತ್ಯಾಗಮ !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_version >= SCRIPT_VERSION_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(ವರ್ಣ *name)
{
    ಪೂರ್ಣಾಙ್ಕ idx = findoption((char_u *)name);

    ಯದಿ (idx >= 0)
	ಪ್ರತ್ಯಾಗಮ &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    ಶೂನ್ಯ
set_term_option_sctx_idx(ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ opt_idx)
{
    char_u  buf[5];
    ಪೂರ್ಣಾಙ್ಕ	    idx;

    ಯದಿ (name == ರಿಕ್ತ)
	idx = opt_idx;
    ಅನ್ಯಥಾ
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    ಯದಿ (idx >= 0)
	set_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
apply_optionset_autocmd(
	ಪೂರ್ಣಾಙ್ಕ	opt_idx,
	long	opt_flags,
	long	oldval,
	long	oldval_g,
	long	newval,
	ವರ್ಣ	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    ಯದಿ (starting || errmsg != ರಿಕ್ತ || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	ಪ್ರತ್ಯಾಗಮ;

    len = vim_snprintf_safelen((ವರ್ಣ *)buf_new, ಪರಿಮಾಣ(buf_new), "%ld", newval);
    set_vim_var_string(VV_OPTION_NEW, buf_new, (ಪೂರ್ಣಾಙ್ಕ)len);
    len = vim_snprintf_safelen((ವರ್ಣ *)buf_type, ಪರಿಮಾಣ(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (ಪೂರ್ಣಾಙ್ಕ)len);

    buf_oldlen = vim_snprintf_safelen((ವರ್ಣ *)buf_old, ಪರಿಮಾಣ(buf_old), "%ld", oldval);
    set_vim_var_string(VV_OPTION_OLD, buf_old, (ಪೂರ್ಣಾಙ್ಕ)buf_oldlen);

    ಯದಿ (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ಪೂರ್ಣಾಙ್ಕ)buf_oldlen);
    }
    ಯದಿ (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (ಪೂರ್ಣಾಙ್ಕ)buf_oldlen);
    }
    ಯದಿ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((ವರ್ಣ *)buf_old_global,
	    ಪರಿಮಾಣ(buf_old_global), "%ld", oldval_g);

	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ಪೂರ್ಣಾಙ್ಕ)buf_oldlen);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (ಪೂರ್ಣಾಙ್ಕ)buf_old_globallen);
    }
    ಯದಿ (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ಪೂರ್ಣಾಙ್ಕ)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    ರಿಕ್ತ, FALSE, ರಿಕ್ತ);
    reset_v_option_vars();
}
#endif

#ಯದಿ defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    ವರ್ಣ *
did_set_arabic(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	ಯದಿ (!p_tbidi)
	{
	    // set rightleft mode
	    ಯದಿ (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		changed_window_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    ಯದಿ (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	ಯದಿ (STRCMP(p_enc, "utf-8") != 0)
	{
	    ಸ್ಥಿರ ವರ್ಣ *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    ಸಂದೇಶ_source(HL_ATTR(HLF_W));
	    ಸಂದೇಶ_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = set_option_ಮೌಲ್ಯ((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    ಅನ್ಯಥಾ
    {
	// 'arabic' is reset, handle various sub-settings.
	ಯದಿ (!p_tbidi)
	{
	    // reset rightleft mode
	    ಯದಿ (curwin->w_p_rl)
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
	curbuf->b_p_imsearch = B_IMODE_USE_ಸೇರಿಸು;
# endif
    }

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
#endif

#ಯದಿ defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    ವರ್ಣ *
did_set_autochdir(optset_T *args UNUSED)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    ವರ್ಣ *
did_set_ballooneval(optset_T *args)
{
    ಯದಿ (balloonEvalForTerm)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (p_beval && !args->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    ಅನ್ಯಥಾ ಯದಿ (!p_beval && args->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_BEVAL_TERM)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    ವರ್ಣ *
did_set_balloonevalterm(optset_T *args UNUSED)
{
    mch_bevalterm_changed();
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Process the updated 'binary' option value.
 */
    ವರ್ಣ *
did_set_ದ್ವಿಮಾನ(optset_T *args)
{
    // when 'bin' is set also set some other options
    set_options_bin(args->os_oldval.boolean, curbuf->b_p_bin, args->os_flags);
    redraw_titles();

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the updated 'buflisted' option value.
 */
    ವರ್ಣ *
did_set_buflisted(optset_T *args)
{
    // when 'buflisted' changes, trigger autocommands
    ಯದಿ (args->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						ರಿಕ್ತ, ರಿಕ್ತ, TRUE, curbuf);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the new 'cmdheight' option value.
 */
    ವರ್ಣ *
did_set_cmdheight(optset_T *args)
{
    long old_ಮೌಲ್ಯ = args->os_oldval.ಸಂಖ್ಯೆ;
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    // if p_ch changed value, change the command line height
    ಯದಿ (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_argument_must_be_positive;
	p_ch = MIN_CMDHEIGHT;
    }
    ಯದಿ (p_ch > Rows - min_rows() + MIN_CMDHEIGHT)
	p_ch = Rows - min_rows() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    ಯದಿ ((p_ch != old_ಮೌಲ್ಯ
		|| tabline_height() + topframe->fr_height != Rows - p_ch)
	    && full_screen
#ifdef FEAT_GUI
	    && !gui.starting
#endif
       )
	command_height();

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    ವರ್ಣ *
did_set_compatible(optset_T *args UNUSED)
{
    compatible_set();
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    ವರ್ಣ *
did_set_conceallevel(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (curwin->w_p_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_cole = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_cole = 3;
    }
    ಯದಿ (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
#endif

#ಯದಿ defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    ವರ್ಣ *
did_set_diff(optset_T *args UNUSED)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    ಯದಿ (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    ವರ್ಣ *
did_set_eof_eol_fixeol_bomb(optset_T *args UNUSED)
{
    // redraw the window title and tab page text
    redraw_titles();
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the updated 'equalalways' option value.
 */
    ವರ್ಣ *
did_set_equalalways(optset_T *args)
{
    ಯದಿ (p_ea && !args->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    ವರ್ಣ *
did_set_foldcolumn(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (curwin->w_p_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_fdc = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_fdc = 12;
    }
    ಯದಿ (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    ವರ್ಣ *
did_set_foldlevel(optset_T *args UNUSED)
{
    ಯದಿ (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the new 'foldminlines' option value.
 */
    ವರ್ಣ *
did_set_foldminlines(optset_T *args UNUSED)
{
    foldUpdateAll(curwin);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    ವರ್ಣ *
did_set_foldnestmax(optset_T *args UNUSED)
{
    ಯದಿ (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_ಹುಡುಕು_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    ವರ್ಣ *
did_set_hlsearch(optset_T *args UNUSED)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    set_no_hlsearch(FALSE);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Process the updated 'ignorecase' option value.
 */
    ವರ್ಣ *
did_set_ignorecase(optset_T *args UNUSED)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    ಯದಿ (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(HAVE_ಒಳಹರಿವು_METHOD)
/*
 * Process the updated 'imdisable' option value.
 */
    ವರ್ಣ *
did_set_imdisable(optset_T *args UNUSED)
{
    // Only de-activate it here, it will be enabled when changing mode.
    ಯದಿ (p_imdisable)
	im_set_active(FALSE);
    ಅನ್ಯಥಾ ಯದಿ (State & MODE_ಸೇರಿಸು)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Process the new 'iminsert' option value.
 */
    ವರ್ಣ *
did_set_iminsert(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    ಯದಿ (termcap_active)	// don't do this in the alternate screen
	showmode();
#ಯದಿ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    ವರ್ಣ *
did_set_imsearch(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

#ಯದಿ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    ವರ್ಣ *
did_set_imstyle(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
#endif

/*
 * Process the updated 'insertmode' option value.
 */
    ವರ್ಣ *
did_set_insertmode(optset_T *args)
{
    // when 'insertmode' is set from an autocommand need to do work here
    ಯದಿ (p_im)
    {
	ಯದಿ ((State & MODE_ಸೇರಿಸು) == 0)
	    need_start_insertmode = TRUE;
	stop_ಸೇರಿಸು_mode = FALSE;
    }
    // only reset if it was set previously
    ಅನ್ಯಥಾ ಯದಿ (args->os_oldval.boolean)
    {
	need_start_insertmode = FALSE;
	stop_ಸೇರಿಸು_mode = TRUE;
	ಯದಿ (restart_edit != 0 && mode_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    ವರ್ಣ *
did_set_langnoremap(optset_T *args UNUSED)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the updated 'langremap' option value.
 */
    ವರ್ಣ *
did_set_langremap(optset_T *args UNUSED)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Process the new 'laststatus' option value.
 */
    ವರ್ಣ *
did_set_laststatus(optset_T *args UNUSED)
{
    last_status(FALSE);	// (re)set last window status line
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    ವರ್ಣ *
did_set_linespace(optset_T *args UNUSED)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    ಯದಿ (gui.in_use && gui_mch_adjust_charheight() == OK)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Process the updated 'lisp' option value.
 */
    ವರ್ಣ *
did_set_lisp(optset_T *args UNUSED)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (ಶೂನ್ಯ)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the new 'maxcombine' option value.
 */
    ವರ್ಣ *
did_set_maxcombine(optset_T *args UNUSED)
{
    ಯದಿ (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    ಅನ್ಯಥಾ ಯದಿ (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the updated 'modifiable' option value.
 */
    ವರ್ಣ *
did_set_modifiable(optset_T *args UNUSED)
{
    // when 'modifiable' is changed, redraw the window title

#ifdef FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    ಯದಿ (curbuf->b_p_ma && (term_in_normal_mode(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_term != ರಿಕ್ತ && !term_is_finished(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	ಪ್ರತ್ಯಾಗಮ e_cannot_make_terminal_with_running_job_modifiable;
    }
#endif
    redraw_titles();

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the updated 'modified' option value.
 */
    ವರ್ಣ *
did_set_modified(optset_T *args)
{
    ಯದಿ (!args->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_set = !!args->os_newval.boolean;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    ವರ್ಣ *
did_set_mousehide(optset_T *args UNUSED)
{
    ಯದಿ (!p_mh)
	gui_mch_mousehide(FALSE);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    ವರ್ಣ *
did_set_ಸಂಖ್ಯೆ_relativenumber(optset_T *args UNUSED)
{
#ಯದಿ defined(FEAT_SIGNS) && defined(FEAT_GUI)
    ಯದಿ (gui.in_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != ರಿಕ್ತ)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	ಯದಿ (!(curwin->w_p_nu && ((ಪೂರ್ಣಾಙ್ಕ *)args->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#endif
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    ವರ್ಣ *
did_set_numberwidth(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    // 'numberwidth' must be positive
    ಯದಿ (curwin->w_p_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_nuw = 1;
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_nuw = 20;
    }
    ಯದಿ (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_ಸಾಲು_ಎಣಿಕೆ = 0; // trigger a redraw

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
#endif

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    ವರ್ಣ *
did_set_osctimeoutlen(optset_T *args)
{
    ಯದಿ (p_ost < 0)
    {
	p_ost = args->os_oldval.ಸಂಖ್ಯೆ;
	ಪ್ರತ್ಯಾಗಮ e_argument_must_be_positive;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    ವರ್ಣ *
did_set_paste(optset_T *args UNUSED)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	old_p_paste = FALSE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	save_sm = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	save_sta = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	save_ru = 0;
#ifdef FEAT_RIGHTLEFT
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	save_ri = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	save_hkmap = 0;
#endif
    buf_T	*buf;

    ಯದಿ (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	ಯದಿ (!old_p_paste)
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
		ಯದಿ (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != empty_option
					   ? vim_strsave(buf->b_p_vsts) : ರಿಕ್ತ;
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
	    ಯದಿ (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != empty_option
						  ? vim_strsave(p_vsts) : ರಿಕ್ತ;
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
	    ಯದಿ (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = empty_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#endif
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	ಯದಿ (p_ru)
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
	ಯದಿ (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = empty_option;
#endif
    }

    // Paste switched from on to off: Restore saved values.
    ಅನ್ಯಥಾ ಯದಿ (old_p_paste)
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
	    ಯದಿ (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : empty_option;
	    vim_free(buf->b_p_vsts_array);
	    ಯದಿ (buf->b_p_vsts && buf->b_p_vsts != empty_option)
		(ಶೂನ್ಯ)tabstop_set(buf->b_p_vsts, &buf->b_p_vsts_array);
	    ಅನ್ಯಥಾ
		buf->b_p_vsts_array = ರಿಕ್ತ;
#endif
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	ಯದಿ (p_ru != save_ru)
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
	ಯದಿ (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : empty_option;
#endif
    }

    old_p_paste = p_paste;

#ಯದಿ defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#endif

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}


#ifdef FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    ವರ್ಣ *
did_set_previewwindow(optset_T *args UNUSED)
{
    ಯದಿ (!curwin->w_p_pvw)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	ಯದಿ (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    ಪ್ರತ್ಯಾಗಮ e_preview_window_already_exists;
	}

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    ವರ್ಣ *
did_set_pyxversion(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
#endif

/*
 * Process the updated 'readonly' option value.
 */
    ವರ್ಣ *
did_set_readonly(optset_T *args)
{
    // when 'readonly' is reset globally, also reset readonlymode
    ಯದಿ (!curbuf->b_p_ro && (args->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    ಯದಿ (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    ವರ್ಣ *
did_set_scrollbind(optset_T *args UNUSED)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    ಯದಿ (!curwin->w_p_scb)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    ವರ್ಣ *
did_set_maxsearchcount(optset_T *args UNUSED)
{
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#define MAX_ಹುಡುಕು_ಎಣಿಕೆ 9999

    ಯದಿ (p_msc <= 0)
	errmsg = e_argument_must_be_positive;
    ಅನ್ಯಥಾ ಯದಿ (p_msc > MAX_ಹುಡುಕು_ಎಣಿಕೆ)
	errmsg = e_invalid_argument;

    ಯದಿ (errmsg != ರಿಕ್ತ)
	p_msc = 99;

    ಪ್ರತ್ಯಾಗಮ errmsg;
#undef MAX_ಹುಡುಕು_ಎಣಿಕೆ
}

#ಯದಿ defined(BACKSLASH_IN_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    ವರ್ಣ *
did_set_shellslash(optset_T *args UNUSED)
{
    ಯದಿ (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    ಅನ್ಯಥಾ
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
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    ವರ್ಣ *
did_set_shiftwidth_tabstop(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;

    ಯದಿ (curbuf->b_p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_ts = 8;
    }
    ಅನ್ಯಥಾ ಯದಿ (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_ts = 8;
    }
    ಯದಿ (p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ts = 8;
    }
    ಅನ್ಯಥಾ ಯದಿ (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	p_ts = 8;
    }

    ಯದಿ (curbuf->b_p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) > 0
		       ? tabstop_first(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#ಅನ್ಯಥಾ
	curbuf->b_p_sw = curbuf->b_p_ts;
#endif
    }
    ಯದಿ (p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) > 0
	     ? tabstop_first(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#ಅನ್ಯಥಾ
	p_sw = curbuf->b_p_ts;
#endif
    }

#ifdef FEAT_FOLDING
    ಯದಿ (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#endif
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    ಯದಿ (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    ವರ್ಣ *
did_set_showtabline(optset_T *args UNUSED)
{
    shell_new_rows();	// recompute window positions and heights
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    ವರ್ಣ *
did_set_smoothscroll(optset_T *args UNUSED)
{
    ಯದಿ (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    ವರ್ಣ *
did_set_spell(optset_T *args UNUSED)
{
    ಯದಿ (curwin->w_p_spell)
	ಪ್ರತ್ಯಾಗಮ parse_spelllang(curwin);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Process the updated 'swapfile' option value.
 */
    ವರ್ಣ *
did_set_swapfile(optset_T *args UNUSED)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    ಯದಿ (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    ಅನ್ಯಥಾ
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_TERMGUICOLORS)
    ವರ್ಣ *
did_set_termguicolors(optset_T *args UNUSED)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    ಯದಿ (
#  ifdef VIMDLL
	    !gui.in_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_set = TRUE;
	p_tgc = 0;
	ಪ್ರತ್ಯಾಗಮ e_24_bit_colors_are_not_supported_on_this_environment;
    }
    ಯದಿ (is_term_win32())
	swap_tcap();
# endif
# ifdef FEAT_GUI
    ಯದಿ (!gui.in_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    ಯದಿ (is_term_win32())
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

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

    ವರ್ಣ *
did_set_termsync(optset_T *args UNUSED)
{
    ಯದಿ (!p_tsy)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    ವರ್ಣ *
did_set_termwinscroll(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;

    ಯದಿ (*pp < 1)
    {
	errmsg = e_argument_must_be_positive;
	*pp = 1;
    }

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
#endif

/*
 * Process the updated 'terse' option value.
 */
    ವರ್ಣ *
did_set_terse(optset_T *args UNUSED)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_ಹುಡುಕು);

    // insert 's' in p_shm
    ಯದಿ (p_terse && p == ರಿಕ್ತ)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    ಅನ್ಯಥಾ ಯದಿ (!p_terse && p != ರಿಕ್ತ)
	STRMOVE(p, p + 1);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the updated 'textauto' option value.
 */
    ವರ್ಣ *
did_set_textauto(optset_T *args)
{
    // when 'textauto' is set or reset also change 'fileformats'
    set_string_option_direct((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | args->os_flags, 0);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the updated 'textmode' option value.
 */
    ವರ್ಣ *
did_set_textmode(optset_T *args)
{
    // when 'textmode' is set or reset also change 'fileformat'
    set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, args->os_flags);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the new 'textwidth' option value.
 */
    ವರ್ಣ *
did_set_textwidth(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (curbuf->b_p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_tw = 0;
    }
    ಯದಿ (p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tw = 0;
    }
#ifdef FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(ರಿಕ್ತ, wp);
    }
#endif

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    ವರ್ಣ *
did_set_title_icon(optset_T *args UNUSED)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_set_title();
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the new 'titlelen' option value.
 */
    ವರ್ಣ *
did_set_titlelen(optset_T *args)
{
    long old_ಮೌಲ್ಯ = args->os_oldval.ಸಂಖ್ಯೆ;
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    // if 'titlelen' has changed, redraw the title
    ಯದಿ (p_titlelen < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_titlelen = 85;
    }
    ಯದಿ (starting != NO_SCREEN && old_ಮೌಲ್ಯ != p_titlelen)
	need_maketitle = TRUE;

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

#ಯದಿ defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    ವರ್ಣ *
did_set_undofile(optset_T *args)
{
    // Only take action when the option was set.
    ಯದಿ (!curbuf->b_p_udf && !p_udf)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

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
	ಯದಿ ((curbuf == save_curbuf
		    || (args->os_flags & OPT_GLOBAL)
		    || args->os_flags == 0)
		&& !curbufIsChanged() && curbuf->b_ml.ml_mfp != ರಿಕ್ತ)
	{
# ifdef FEAT_CRYPT
	    ಯದಿ (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
		ಅನುವರ್ತ;
# endif
	    u_compute_hash(hash);
	    u_read_undo(ರಿಕ್ತ, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Process the new global 'undolevels' option value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_global_undolevels(long ಮೌಲ್ಯ, long old_ಮೌಲ್ಯ)
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
update_buflocal_undolevels(long ಮೌಲ್ಯ, long old_ಮೌಲ್ಯ)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_ಮೌಲ್ಯ;
    u_sync(TRUE);
    curbuf->b_p_ul = ಮೌಲ್ಯ;
}

/*
 * Process the new 'undolevels' option value.
 */
    ವರ್ಣ *
did_set_undolevels(optset_T *args)
{
    long *pp = (long *)args->os_varp;

    ಯದಿ (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(args->os_newval.ಸಂಖ್ಯೆ,
						       args->os_oldval.ಸಂಖ್ಯೆ);
    ಅನ್ಯಥಾ ಯದಿ (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(args->os_newval.ಸಂಖ್ಯೆ,
						       args->os_oldval.ಸಂಖ್ಯೆ);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the new 'updatecount' option value.
 */
    ವರ್ಣ *
did_set_updatecount(optset_T *args)
{
    long old_ಮೌಲ್ಯ = args->os_oldval.ಸಂಖ್ಯೆ;
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    // when 'updatecount' changes from zero to non-zero, open swap files
    ಯದಿ (p_uc < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_uc = 100;
    }
    ಯದಿ (p_uc && !old_ಮೌಲ್ಯ)
	ml_open_files();

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    ವರ್ಣ *
did_set_weirdinvert(optset_T *args)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    ಯದಿ (p_wiv && !args->os_oldval.boolean)
	T_XS = (char_u *)"y";
    ಅನ್ಯಥಾ ಯದಿ (!p_wiv && args->os_oldval.boolean)
	T_XS = empty_option;
    p_wiv = (*T_XS != NUL);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    ವರ್ಣ *
did_set_wildchar(optset_T *args)
{
    long c = *(long *)args->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    ಯದಿ (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the new 'window' option value.
 */
    ವರ್ಣ *
did_set_window(optset_T *args UNUSED)
{
    ಯದಿ (p_window < 1)
	p_window = 1;
    ಅನ್ಯಥಾ ಯದಿ (p_window >= Rows)
	p_window = Rows - 1;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    ವರ್ಣ *
did_set_winheight_helpheight(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;

    ಯದಿ (p_wh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wh = 1;
    }
    ಯದಿ (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    ಯದಿ (p_hh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hh = 0;
    }

    // Change window height NOW
    ಯದಿ (!ONE_WINDOW)
    {
	ಯದಿ (pp == &p_wh && curwin->w_height < p_wh)
	    win_setheight((ಪೂರ್ಣಾಙ್ಕ)p_wh);
	ಯದಿ (pp == &p_hh && curbuf->b_help && curwin->w_height < p_hh)
	    win_setheight((ಪೂರ್ಣಾಙ್ಕ)p_hh);
    }

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    ವರ್ಣ *
did_set_winminheight(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (p_wmh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmh = 0;
    }
    ಯದಿ (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    ವರ್ಣ *
did_set_winminwidth(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (p_wmw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmw = 0;
    }
    ಯದಿ (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    ವರ್ಣ *
did_set_winwidth(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (p_wiw < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wiw = 1;
    }
    ಯದಿ (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    ಯದಿ (!ONE_WINDOW && curwin->w_width < p_wiw)
	win_setwidth((ಪೂರ್ಣಾಙ್ಕ)p_wiw);

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

#ಯದಿ defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    ವರ್ಣ *
did_set_wlsteal(optset_T *args UNUSED)
{
    clip_reset_wayland();

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    ವರ್ಣ *
did_set_wltimeoutlen(optset_T *args)
{
    ಯದಿ (p_wtm < 0)
    {
	p_wtm = args->os_oldval.ಸಂಖ್ಯೆ;
	ಪ್ರತ್ಯಾಗಮ e_argument_must_be_positive;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Process the updated 'wrap' option value.
 */
    ವರ್ಣ *
did_set_wrap(optset_T *args UNUSED)
{
    // Set w_leftcol or w_skipcol to zero.
    ಯದಿ (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    ಅನ್ಯಥಾ
	curwin->w_skipcol = 0;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ifdef FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    ವರ್ಣ *
did_set_xhistory(optset_T *args)
{
    ಪೂರ್ಣಾಙ್ಕ is_p_chi = (long*)args->os_varp == &p_chi;
    ಪೂರ್ಣಾಙ್ಕ err;
    long *arg = (is_p_chi) ? &p_chi :(long*)args->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    ಯದಿ (*arg < 1)
    {
	*arg = args->os_oldval.ಸಂಖ್ಯೆ;
	ಪ್ರತ್ಯಾಗಮ e_cannot_have_negative_or_zero_ಸಂಖ್ಯೆ_of_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    ಯದಿ (*arg > 100)
    {
	*arg = args->os_oldval.ಸಂಖ್ಯೆ;
	ಪ್ರತ್ಯಾಗಮ e_cannot_have_more_than_hundred_quickfix;
    }

    ಯದಿ (is_p_chi)
	err = qf_resize_ರಾಶಿ(*arg);
    ಅನ್ಯಥಾ
	err = ll_resize_ರಾಶಿ(curwin, *arg);

    ಯದಿ (err == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_failed_resizing_quickfix_ರಾಶಿ;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ಸ್ಥಿರ ವರ್ಣ *
set_bool_option(
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    ಪೂರ್ಣಾಙ್ಕ		ಮೌಲ್ಯ,			// new value
    ಪೂರ್ಣಾಙ್ಕ		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    ಪೂರ್ಣಾಙ್ಕ		old_ಮೌಲ್ಯ = *(ಪೂರ್ಣಾಙ್ಕ *)varp;
#ಯದಿ defined(FEAT_EVAL)
    ಪೂರ್ಣಾಙ್ಕ		old_global_ಮೌಲ್ಯ = 0;
#endif
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;

    // Disallow changing some options from secure mode
    ಯದಿ ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	ಪ್ರತ್ಯಾಗಮ e_not_allowed_here;

#ಯದಿ defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    ಯದಿ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_ಮೌಲ್ಯ = *(ಪೂರ್ಣಾಙ್ಕ *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *(ಪೂರ್ಣಾಙ್ಕ *)varp = ಮೌಲ್ಯ;	    // set the new value
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    ಯದಿ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(ಪೂರ್ಣಾಙ್ಕ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = ಮೌಲ್ಯ;

    // Handle side effects of changing a bool option.
    ಯದಿ (options[opt_idx].opt_did_set_cb != ರಿಕ್ತ)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.boolean = old_ಮೌಲ್ಯ;
	args.os_newval.boolean = ಮೌಲ್ಯ;
	args.os_errbuf = ರಿಕ್ತ;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
	ಯದಿ (errmsg != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_SET;

#ಯದಿ defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(long)(old_ಮೌಲ್ಯ ? TRUE : FALSE),
				(long)(old_global_ಮೌಲ್ಯ ? TRUE : FALSE),
				(long)(ಮೌಲ್ಯ ? TRUE : FALSE), ರಿಕ್ತ);
#endif

    comp_col();			    // in case 'ruler' or 'showcmd' changed

    ಯದಿ (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    ಯದಿ ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    ಸ್ಥಿರ ವರ್ಣ *
check_ಅಂಕೆ_option_bounds(
    long	*pp,
    long	old_ಮೌಲ್ಯ,
    long	old_Rows,
    long	old_Columns,
    ವರ್ಣ	*errbuf,
    size_t	errbuflen,
    ವರ್ಣ	*errmsg)
{
    ಯದಿ (Rows < min_rows_for_all_tabpages() && full_screen)
    {
	ಯದಿ (errbuf != ರಿಕ್ತ)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_ಸಾಲುಗಳು), min_rows_for_all_tabpages());
	    errmsg = errbuf;
	}
	Rows = min_rows_for_all_tabpages();
    }
    ಯದಿ (Columns < MIN_COLUMNS && full_screen)
    {
	ಯದಿ (errbuf != ರಿಕ್ತ)
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
    ಯದಿ (old_Rows != Rows || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	ಯದಿ (updating_screen)
	    *pp = old_ಮೌಲ್ಯ;
	ಅನ್ಯಥಾ ಯದಿ (full_screen
#ifdef FEAT_GUI
		&& !gui.starting
#endif
		)
	    set_shellsize((ಪೂರ್ಣಾಙ್ಕ)Columns, (ಪೂರ್ಣಾಙ್ಕ)Rows, TRUE);
	ಅನ್ಯಥಾ
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    ಯದಿ (cmdline_row > Rows - p_ch && Rows > p_ch)
		cmdline_row = Rows - p_ch;
	}
	ಯದಿ (p_window >= Rows || !option_was_set((char_u *)"window"))
	    p_window = Rows - 1;
    }

    ಯದಿ (p_tm < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tm = 0;
    }
    ಯದಿ ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_height
		    && curwin->w_height > 0))
	    && full_screen)
    {
	ಯದಿ (pp == &(curwin->w_p_scr))
	{
	    ಯದಿ (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_size;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	ಅನ್ಯಥಾ ಯದಿ (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	ಅನ್ಯಥಾ // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_height;
    }
    ಯದಿ (p_hi < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hi = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (p_hi > 10000)
    {
	errmsg = e_invalid_argument;
	p_hi = 10000;
    }
    ಯದಿ (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_argument;
	p_re = 0;
    }
    ಯದಿ (p_report < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_report = 1;
    }
    ಯದಿ ((p_sj < -100 || p_sj >= Rows) && full_screen)
    {
	ಯದಿ (Rows != old_Rows)	// Rows changed, just adjust p_sj
	    p_sj = Rows / 2;
	ಅನ್ಯಥಾ
	{
	    errmsg = e_invalid_scroll_size;
	    p_sj = 1;
	}
    }
    ಯದಿ (p_so < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_so = 0;
    }
    ಯದಿ (p_sop < 0 && full_screen)
    {
	errmsg = e_invalid_argument;
	p_sop = 0;
    }
    ಯದಿ (p_siso < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_siso = 0;
    }
    ಯದಿ (p_cwh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_cwh = 1;
    }
    ಯದಿ (p_ut < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ut = 2000;
    }
    ಯದಿ (p_ss < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ss = 0;
    }

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ಸ್ಥಿರ ವರ್ಣ *
set_ಅಂಕೆ_option(
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    long	ಮೌಲ್ಯ,			// new value
    ವರ್ಣ	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    ಪೂರ್ಣಾಙ್ಕ		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;
    long	old_ಮೌಲ್ಯ = *(long *)varp;
#ಯದಿ defined(FEAT_EVAL)
    long	old_global_ಮೌಲ್ಯ = 0;	// only used when setting a local and
					// global option
#endif
    long	old_Rows = Rows;	// remember old Rows
    long	old_Columns = Columns;	// remember old Columns
    long	*pp = (long *)varp;

    // Disallow changing some options from secure mode.
    ಯದಿ ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	ಪ್ರತ್ಯಾಗಮ e_not_allowed_here;

#ಯದಿ defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    ಯದಿ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_ಮೌಲ್ಯ = *(long *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *pp = ಮೌಲ್ಯ;
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    ಯದಿ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = ಮೌಲ್ಯ;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    ಯದಿ (options[opt_idx].opt_did_set_cb != ರಿಕ್ತ)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.ಸಂಖ್ಯೆ = old_ಮೌಲ್ಯ;
	args.os_newval.ಸಂಖ್ಯೆ = ಮೌಲ್ಯ;
	args.os_errbuf = ರಿಕ್ತ;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
    }

    // Check the bounds for numeric options here
    errmsg = check_ಅಂಕೆ_option_bounds(pp, old_ಮೌಲ್ಯ, old_Rows, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_SET;

#ಯದಿ defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_ಮೌಲ್ಯ, old_global_ಮೌಲ್ಯ,
								ಮೌಲ್ಯ, errmsg);
#endif

    comp_col();			    // in case 'columns' or 'ls' changed

    ಯದಿ (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    ಯದಿ ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    ಶೂನ್ಯ
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    ಪೂರ್ಣಾಙ್ಕ		doclear = (flags & P_RCLR) == P_RCLR;
    ಪೂರ್ಣಾಙ್ಕ		all = ((flags & P_RALL) == P_RALL || doclear);

    ಯದಿ ((flags & P_RSTAT) || all)	// mark all status lines dirty
	status_redraw_all();

    ಯದಿ ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	ಯದಿ (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	ಅನ್ಯಥಾ
	    changed_window_setting();
    }
    ಯದಿ (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    ಯದಿ (doclear)
	redraw_all_later(UPD_CLEAR);
    ಅನ್ಯಥಾ ಯದಿ (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
findoption(char_u *arg)
{
    ಪೂರ್ಣಾಙ್ಕ		    opt_idx;
    ವರ್ಣ	    *s, *p;
    ಸ್ಥಿರ short    quick_tab[27] = {0, 0};	// quick access table
    ಪೂರ್ಣಾಙ್ಕ		    is_term_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    ಯದಿ (quick_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	ಪೂರ್ಣಾಙ್ಕ last_opt_idx;
	ಪ್ರತಿ (last_opt_idx = 0; options[last_opt_idx].fullname != ರಿಕ್ತ;
		last_opt_idx++)
	    ;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ tab_idx = 1; tab_idx < 27; tab_idx++)
	    quick_tab[tab_idx] = last_opt_idx;

	p = options[0].fullname;
	ಪ್ರತಿ (opt_idx = 1; opt_idx < last_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    ಯದಿ (s[0] != p[0])
	    {
		ಯದಿ (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		ಅನ್ಯಥಾ
		    quick_tab[Char_Ord_ಕೆಳಗಿನ(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    ಯದಿ (arg[0] < 'a' || arg[0] > 'z')
	ಪ್ರತ್ಯಾಗಮ -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    ಯದಿ (is_term_opt)
	opt_idx = quick_tab[26];
    ಅನ್ಯಥಾ
	opt_idx = quick_tab[Char_Ord_ಕೆಳಗಿನ(arg[0])];
    ಪ್ರತಿ (; (s = options[opt_idx].fullname) != ರಿಕ್ತ && s[0] == arg[0]; opt_idx++)
    {
	ಯದಿ (STRCMP(arg, s) == 0)		    // match full name
	    ಭಙ್ಗ;
    }
    ಯದಿ (s != ರಿಕ್ತ && s[0] != arg[0])
	s = ರಿಕ್ತ;
    ಯದಿ (s == ರಿಕ್ತ && !is_term_opt)
    {
	opt_idx = quick_tab[Char_Ord_ಕೆಳಗಿನ(arg[0])];
	ಪ್ರತಿ ( ; options[opt_idx].fullname != ರಿಕ್ತ; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    ಯದಿ (s != ರಿಕ್ತ && STRCMP(arg, s) == 0)   // match short name
		ಭಙ್ಗ;
	    s = ರಿಕ್ತ;
	}
    }
    ಯದಿ (s == ರಿಕ್ತ)
	opt_idx = -1;
    ಪ್ರತ್ಯಾಗಮ opt_idx;
}

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
    long	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    ಪೂರ್ಣಾಙ್ಕ		*flagsp,
    ಪೂರ್ಣಾಙ್ಕ		scope)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    ಯದಿ (opt_idx < 0)		    // option not in the table
    {
	ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ;

	ಯದಿ (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (ಕೀಲಿ = find_ಕೀಲಿ_option(name, FALSE)) != 0)
	{
	    char_u ಕೀಲಿ_name[2];
	    char_u *p;

	    ಯದಿ (flagsp != ರಿಕ್ತ)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    ಯದಿ (ಕೀಲಿ < 0)
	    {
		ಕೀಲಿ_name[0] = KEY2TERMCAP0(ಕೀಲಿ);
		ಕೀಲಿ_name[1] = KEY2TERMCAP1(ಕೀಲಿ);
	    }
	    ಅನ್ಯಥಾ
	    {
		ಕೀಲಿ_name[0] = KS_ಕೀಲಿ;
		ಕೀಲಿ_name[1] = (ಕೀಲಿ & 0xff);
	    }
	    p = find_termcode(ಕೀಲಿ_name);
	    ಯದಿ (p != ರಿಕ್ತ)
	    {
		ಯದಿ (stringval != ರಿಕ್ತ)
		    *stringval = vim_strsave(p);
		ಪ್ರತ್ಯಾಗಮ gov_string;
	    }
	}
	ಪ್ರತ್ಯಾಗಮ gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    ಯದಿ (flagsp != ರಿಕ್ತ)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    ಯದಿ (options[opt_idx].flags & P_STRING)
    {
	ಯದಿ (varp == ರಿಕ್ತ)		    // hidden option
	    ಪ್ರತ್ಯಾಗಮ gov_hidden_string;
	ಯದಿ (stringval != ರಿಕ್ತ)
	{
	    ಯದಿ ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    ಅನ್ಯಥಾ ಯದಿ ((char_u **)varp == &curbuf->b_p_ಕೀಲಿ
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    ಅನ್ಯಥಾ
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	ಪ್ರತ್ಯಾಗಮ gov_string;
    }

    ಯದಿ (varp == ರಿಕ್ತ)		    // hidden option
	ಪ್ರತ್ಯಾಗಮ (options[opt_idx].flags & P_ಅಂಕೆ)
					 ? gov_hidden_ಸಂಖ್ಯೆ : gov_hidden_bool;
    ಯದಿ (options[opt_idx].flags & P_ಅಂಕೆ)
	*numval = *(long *)varp;
    ಅನ್ಯಥಾ
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ *)varp == &curbuf->b_changed)
	    *numval = curbufIsChanged();
	ಅನ್ಯಥಾ
	    *numval = (long) *(ಪೂರ್ಣಾಙ್ಕ *)varp;
    }
    ಪ್ರತ್ಯಾಗಮ (options[opt_idx].flags & P_ಅಂಕೆ) ? gov_ಸಂಖ್ಯೆ : gov_bool;
}
#endif

#ಯದಿ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    ಪೂರ್ಣಾಙ್ಕ
get_option_ಮೌಲ್ಯ_strict(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    ಪೂರ್ಣಾಙ್ಕ		opt_type,
    ಶೂನ್ಯ	*from)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;
    char_u	*varp = ರಿಕ್ತ;
    ಸಂರಚನಾ vimoption *p;
    ಪೂರ್ಣಾಙ್ಕ		r = 0;

    opt_idx = findoption(name);
    ಯದಿ (opt_idx < 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    p = &(options[opt_idx]);

    // Hidden option
    ಯದಿ (p->var == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    ಅನ್ಯಥಾ ಯದಿ (p->flags & P_ಅಂಕೆ)
	r |= SOPT_ಅಂಕೆ;
    ಅನ್ಯಥಾ ಯದಿ (p->flags & P_STRING)
	r |= SOPT_STRING;

    ಯದಿ (p->indir == PV_NONE)
    {
	ಯದಿ (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ 0; // Did not request global-only option
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	ಅನ್ಯಥಾ ಯದಿ (opt_type == SREQ_GLOBAL)
	    ಪ್ರತ್ಯಾಗಮ 0; // Requested global option

	ಯದಿ (p->indir & PV_WIN)
	{
	    ಯದಿ (opt_type == SREQ_BUF)
		ಪ್ರತ್ಯಾಗಮ 0; // Did not request window-local option
	    ಅನ್ಯಥಾ
		r |= SOPT_WIN;
	}
	ಅನ್ಯಥಾ ಯದಿ (p->indir & PV_BUF)
	{
	    ಯದಿ (opt_type == SREQ_WIN)
		ಪ್ರತ್ಯಾಗಮ 0; // Did not request buffer-local option
	    ಅನ್ಯಥಾ
		r |= SOPT_BUF;
	}
    }

    ಯದಿ (stringval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ r;

    ಯದಿ (opt_type == SREQ_GLOBAL)
	varp = p->var;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    ಯದಿ (p->indir == PV_MOD)
	    {
		*numval = bufIsChanged((buf_T *)from);
		varp = ರಿಕ್ತ;
	    }
# ifdef FEAT_CRYPT
	    ಅನ್ಯಥಾ ಯದಿ (p->indir == PV_ಕೀಲಿ)
	    {
		// never return the value of the crypt key
		*stringval = ರಿಕ್ತ;
		varp = ರಿಕ್ತ;
	    }
# endif
	    ಅನ್ಯಥಾ
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
	ಅನ್ಯಥಾ ಯದಿ (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_buffer;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	}
	ಯದಿ (varp == p->var)
	    ಪ್ರತ್ಯಾಗಮ (r | SOPT_UNSET);
    }

    ಯದಿ (varp != ರಿಕ್ತ)
    {
	ಯದಿ (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	ಅನ್ಯಥಾ ಯದಿ (p->flags & P_ಅಂಕೆ)
	    *numval = *(long *) varp;
	ಅನ್ಯಥಾ
	    *numval = *(ಪೂರ್ಣಾಙ್ಕ *)varp;
    }

    ಪ್ರತ್ಯಾಗಮ r;
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
option_iter_next(ಶೂನ್ಯ **option, ಪೂರ್ಣಾಙ್ಕ opt_type)
{
    ಸಂರಚನಾ vimoption	*ret = ರಿಕ್ತ;
    ಕುರು
    {
	ಯದಿ (*option == ರಿಕ್ತ)
	    *option = (ಶೂನ್ಯ *) options;
	ಅನ್ಯಥಾ ಯದಿ (((ಸಂರಚನಾ vimoption *) (*option))->fullname == ರಿಕ್ತ)
	{
	    *option = ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	    *option = (ಶೂನ್ಯ *) (((ಸಂರಚನಾ vimoption *) (*option)) + 1);

	ret = ((ಸಂರಚನಾ vimoption *) (*option));

	// Hidden option
	ಯದಿ (ret->var == ರಿಕ್ತ)
	{
	    ret = ರಿಕ್ತ;
	    ಅನುವರ್ತ;
	}

	ವಿಕಲ್ಪ (opt_type)
	{
	    ಪಕ್ಷ SREQ_GLOBAL:
		ಯದಿ (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = ರಿಕ್ತ;
		ಭಙ್ಗ;
	    ಪಕ್ಷ SREQ_BUF:
		ಯದಿ (!(ret->indir & PV_BUF))
		    ret = ರಿಕ್ತ;
		ಭಙ್ಗ;
	    ಪಕ್ಷ SREQ_WIN:
		ಯದಿ (!(ret->indir & PV_WIN))
		    ret = ರಿಕ್ತ;
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		internal_error("option_iter_next()");
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ಯಾವತ್ (ret == ರಿಕ್ತ);

    ಪ್ರತ್ಯಾಗಮ (char_u *)ret->fullname;
}
#endif

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(ಪೂರ್ಣಾಙ್ಕ opt_idx)
{
    ಪ್ರತ್ಯಾಗಮ options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    ಶೂನ್ಯ
set_option_flag(ಪೂರ್ಣಾಙ್ಕ opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    ಪೂರ್ಣಾಙ್ಕ
is_global_option(ಪೂರ್ಣಾಙ್ಕ opt_idx)
{
    ಪ್ರತ್ಯಾಗಮ options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    ಪೂರ್ಣಾಙ್ಕ
is_global_local_option(ಪೂರ್ಣಾಙ್ಕ opt_idx)
{
    ಪ್ರತ್ಯಾಗಮ options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    ಪೂರ್ಣಾಙ್ಕ
is_window_local_option(ಪೂರ್ಣಾಙ್ಕ opt_idx)
{
    ಪ್ರತ್ಯಾಗಮ options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    ಪೂರ್ಣಾಙ್ಕ
is_hidden_option(ಪೂರ್ಣಾಙ್ಕ opt_idx)
{
    ಪ್ರತ್ಯಾಗಮ options[opt_idx].var == ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    ಪೂರ್ಣಾಙ್ಕ
is_crypt_ಕೀಲಿ_option(ಪೂರ್ಣಾಙ್ಕ opt_idx)
{
    ಪ್ರತ್ಯಾಗಮ options[opt_idx].indir == PV_ಕೀಲಿ;
}
#endif

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    ವರ್ಣ *
set_option_ಮೌಲ್ಯ(
    char_u	*name,
    long	ಸಂಖ್ಯೆ,
    char_u	*string,
    ಪೂರ್ಣಾಙ್ಕ		opt_flags)	// OPT_LOCAL or 0 (both)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;
    char_u	*varp;
    long_u	flags;
    ಸ್ಥಿರ ವರ್ಣ	errbuf[ERR_BUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    ಯದಿ (opt_idx < 0)
    {
	ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ;

	ಯದಿ (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (ಕೀಲಿ = find_ಕೀಲಿ_option(name, FALSE)) != 0)
	{
	    char_u ಕೀಲಿ_name[2];

	    ಯದಿ (ಕೀಲಿ < 0)
	    {
		ಕೀಲಿ_name[0] = KEY2TERMCAP0(ಕೀಲಿ);
		ಕೀಲಿ_name[1] = KEY2TERMCAP1(ಕೀಲಿ);
	    }
	    ಅನ್ಯಥಾ
	    {
		ಕೀಲಿ_name[0] = KS_ಕೀಲಿ;
		ಕೀಲಿ_name[1] = (ಕೀಲಿ & 0xff);
	    }
	    add_termcode(ಕೀಲಿ_name, string, FALSE);
	    ಯದಿ (full_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    ಅನ್ಯಥಾ
    {
	flags = options[opt_idx].flags;
#ifdef HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	ಯದಿ (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_in_sandbox));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
#endif
	ಯದಿ (flags & P_STRING)
	    ಪ್ರತ್ಯಾಗಮ set_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	ಯದಿ (varp != ರಿಕ್ತ)	// hidden option is not changed
	{
	    ಯದಿ (ಸಂಖ್ಯೆ == 0 && string != ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ idx;

		// Either we are given a string or we are setting option
		// to zero.
		ಪ್ರತಿ (idx = 0; string[idx] == '0'; ++idx)
		    ;
		ಯದಿ (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_ಸಂಖ್ಯೆ_required_after_str_equal_str),
								 name, string);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;     // do nothing as we hit an error

		}
	    }
	    ಯದಿ (flags & P_ಅಂಕೆ)
	    {
		ಪ್ರತ್ಯಾಗಮ set_ಅಂಕೆ_option(opt_idx, varp, ಸಂಖ್ಯೆ,
					   errbuf, ಪರಿಮಾಣ(errbuf), opt_flags);
	    }
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ set_bool_option(opt_idx, varp, (ಪೂರ್ಣಾಙ್ಕ)ಸಂಖ್ಯೆ, opt_flags);
	}

    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    ಶೂನ್ಯ
set_option_ಮೌಲ್ಯ_give_err(
    char_u	*name,
    long	ಸಂಖ್ಯೆ,
    char_u	*string,
    ಪೂರ್ಣಾಙ್ಕ		opt_flags)	// OPT_LOCAL or 0 (both)
{
    ವರ್ಣ *errmsg = set_option_ಮೌಲ್ಯ(name, ಸಂಖ್ಯೆ, string, opt_flags);

    ಯದಿ (errmsg != ರಿಕ್ತ)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_term_code(char_u *tname)
{
    ಪೂರ್ಣಾಙ್ಕ	    opt_idx;
    char_u  *varp;

    ಯದಿ (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	ಯದಿ (varp != ರಿಕ್ತ)
	    varp = *(char_u **)(varp);
	ಪ್ರತ್ಯಾಗಮ varp;
    }
    ಪ್ರತ್ಯಾಗಮ find_termcode(tname + 2);
}

    char_u *
get_highlight_default(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    i = findoption((char_u *)"hl");
    ಯದಿ (i >= 0)
	ಪ್ರತ್ಯಾಗಮ options[i].def_val[VI_DEFAULT];
    ಪ್ರತ್ಯಾಗಮ (char_u *)ರಿಕ್ತ;
}

    char_u *
get_encoding_default(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    i = findoption((char_u *)"enc");
    ಯದಿ (i >= 0)
	ಪ್ರತ್ಯಾಗಮ options[i].def_val[VI_DEFAULT];
    ಪ್ರತ್ಯಾಗಮ (char_u *)ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_QUICKFIX)
    ಪೂರ್ಣಾಙ್ಕ
is_option_allocated(ವರ್ಣ *name)
{
    ಪೂರ್ಣಾಙ್ಕ idx = findoption((char_u *)name);

    ಪ್ರತ್ಯಾಗಮ idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#endif

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_ಕೀಲಿ_option(char_u *arg_arg, ಪೂರ್ಣಾಙ್ಕ has_lt)
{
    ಪೂರ್ಣಾಙ್ಕ		ಕೀಲಿ = 0;
    ಪೂರ್ಣಾಙ್ಕ		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    ಯದಿ (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	ಯದಿ (!has_lt || arg[4] == '>')
	    ಕೀಲಿ = TERMCAP2_ಕೀಲಿ(arg[2], arg[3]);
    }
    ಅನ್ಯಥಾ ಯದಿ (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	ಕೀಲಿ = find_special_ಕೀಲಿ(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_ಕೀಲಿ | FSK_SIMPLIFY, ರಿಕ್ತ);
	ಯದಿ (modifiers)		    // can't handle modifiers here
	    ಕೀಲಿ = 0;
    }
    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
showoptions(
    ಪೂರ್ಣಾಙ್ಕ		all,
    ಪೂರ್ಣಾಙ್ಕ		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    ಸಂರಚನಾ vimoption	*p;
    ಪೂರ್ಣಾಙ್ಕ			col;
    ಪೂರ್ಣಾಙ್ಕ			isterm;
    char_u		*varp;
    ಸಂರಚನಾ vimoption	**ವಸ್ತುಗಳು;
    ಪೂರ್ಣಾಙ್ಕ			item_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ			run;
    ಪೂರ್ಣಾಙ್ಕ			row, rows;
    ಪೂರ್ಣಾಙ್ಕ			cols;
    ಪೂರ್ಣಾಙ್ಕ			i;
    ಪೂರ್ಣಾಙ್ಕ			len;

#define INC 20
#define GAP 3

    ವಸ್ತುಗಳು = ALLOC_MULT(ಸಂರಚನಾ vimoption *, OPTION_ಎಣಿಕೆ);
    ಯದಿ (ವಸ್ತುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Highlight title
    ಯದಿ (all == 2)
	ಸಂದೇಶ_puts_title(_("\n--- Terminal codes ---"));
    ಅನ್ಯಥಾ ಯದಿ (opt_flags & OPT_GLOBAL)
	ಸಂದೇಶ_puts_title(_("\n--- Global option values ---"));
    ಅನ್ಯಥಾ ಯದಿ (opt_flags & OPT_LOCAL)
	ಸಂದೇಶ_puts_title(_("\n--- Local option values ---"));
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    ಪ್ರತಿ (run = 1; run <= 2 && !got_int; ++run)
    {
	// collect the items in items[]
	item_ಎಣಿಕೆ = 0;
	ಪ್ರತಿ (p = &options[0]; p->fullname != ರಿಕ್ತ; p++)
	{
	    // apply :filter /pat/
	    ಯದಿ (message_filtered((char_u *)p->fullname))
		ಅನುವರ್ತ;

	    varp = ರಿಕ್ತ;
	    isterm = istermoption(p);
	    ಯದಿ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		ಯದಿ (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    ಅನ್ಯಥಾ
		varp = get_varp(p);
	    ಯದಿ (varp != ರಿಕ್ತ
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		ಯದಿ (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		ಅನ್ಯಥಾ ಯದಿ (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		ಅನ್ಯಥಾ
		{
		    option_value2string(p, opt_flags);
		    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		ಯದಿ ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    ವಸ್ತುಗಳು[item_ಎಣಿಕೆ++] = p;
	    }
	}

	// display the items
	ಯದಿ (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    ಯದಿ (cols == 0)
		cols = 1;
	    rows = (item_ಎಣಿಕೆ + cols - 1) / cols;
	}
	ಅನ್ಯಥಾ	// run == 2
	    rows = item_ಎಣಿಕೆ;
	ಪ್ರತಿ (row = 0; row < rows && !got_int; ++row)
	{
	    ಸಂದೇಶ_putchar('\n');			// go to next line
	    ಯದಿ (got_int)			// 'q' typed in more
		ಭಙ್ಗ;
	    col = 0;
	    ಪ್ರತಿ (i = row; i < item_ಎಣಿಕೆ; i += rows)
	    {
		ಸಂದೇಶ_col = col;			// make columns
		showoneopt(ವಸ್ತುಗಳು[i], opt_flags);
		col += INC;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(ವಸ್ತುಗಳು);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
optval_default(ಸಂರಚನಾ vimoption *p, char_u *varp, ಪೂರ್ಣಾಙ್ಕ compatible)
{
    ಪೂರ್ಣಾಙ್ಕ		dvi;

    ಯದಿ (varp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    ಯದಿ (p->flags & P_ಅಂಕೆ)
	ಪ್ರತ್ಯಾಗಮ (*(long *)varp == (long)(long_i)p->def_val[dvi]);
    ಯದಿ (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	ಪ್ರತ್ಯಾಗಮ (*(ಪೂರ್ಣಾಙ್ಕ *)varp == (ಪೂರ್ಣಾಙ್ಕ)(long)(long_i)p->def_val[dvi]);
    // P_STRING
    ಪ್ರತ್ಯಾಗಮ (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
showoneopt(
    ಸಂರಚನಾ vimoption	*p,
    ಪೂರ್ಣಾಙ್ಕ			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    ಪೂರ್ಣಾಙ್ಕ		save_silent = silent_mode;

    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    ಯದಿ ((p->flags & P_BOOL) && ((ಪೂರ್ಣಾಙ್ಕ *)varp == &curbuf->b_changed
					? !curbufIsChanged() : !*(ಪೂರ್ಣಾಙ್ಕ *)varp))
	ಸಂದೇಶ_puts("no");
    ಅನ್ಯಥಾ ಯದಿ ((p->flags & P_BOOL) && *(ಪೂರ್ಣಾಙ್ಕ *)varp < 0)
	ಸಂದೇಶ_puts("--");
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_puts("  ");
    ಸಂದೇಶ_puts(p->fullname);
    ಯದಿ (!(p->flags & P_BOOL))
    {
	ಸಂದೇಶ_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	ಸಂದೇಶ_outtrans(NameBuff);
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
    ಪೂರ್ಣಾಙ್ಕ
makeset(FILE *fd, ಪೂರ್ಣಾಙ್ಕ opt_flags, ಪೂರ್ಣಾಙ್ಕ local_only)
{
    ಸಂರಚನಾ vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = ರಿಕ್ತ;	// fresh value
    ವರ್ಣ		*cmd;
    ಪೂರ್ಣಾಙ್ಕ			round;
    ಪೂರ್ಣಾಙ್ಕ			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    ಪ್ರತಿ (pri = 1; pri >= 0; --pri)
    {
      ಪ್ರತಿ (p = &options[0]; !istermoption(p); p++)
	ಯದಿ (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    ಯದಿ (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		ಅನುವರ್ತ;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    ಯದಿ ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		ಅನುವರ್ತ;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    ಯದಿ ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		ಅನುವರ್ತ;

	    ಯದಿ ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		ಅನುವರ್ತ;

	    round = 2;
	    ಯದಿ (p->indir != PV_NONE)
	    {
		ಯದಿ (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    ಯದಿ (!(opt_flags & OPT_LOCAL))
			ಅನುವರ್ತ;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    ಯದಿ (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			ಯದಿ (!optval_default(p, varp_fresh, p_cp))
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
	    ಪ್ರತಿ ( ; round <= 2; varp = varp_local, ++round)
	    {
		ಯದಿ (round == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		ಅನ್ಯಥಾ
		    cmd = "setlocal";

		ಯದಿ (p->flags & P_BOOL)
		{
		    ಯದಿ (put_setbool(fd, cmd, p->fullname, *(ಪೂರ್ಣಾಙ್ಕ *)varp) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಅನ್ಯಥಾ ಯದಿ (p->flags & P_ಅಂಕೆ)
		{
		    ಯದಿ (put_setnum(fd, cmd, p->fullname, (long *)varp) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಅನ್ಯಥಾ    // P_STRING
		{
		    ಪೂರ್ಣಾಙ್ಕ		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    ಯದಿ ((p->flags & P_FUNC) && *(char_u **)varp != ರಿಕ್ತ
			    && strstr(*(ವರ್ಣ **)varp, "<lambda>") != ರಿಕ್ತ)
			ಅನುವರ್ತ;

#ifdef FEAT_EVAL
		    legacy = !is_option_ಮೌಲ್ಯ_vim9(p - &options[0],
			    round == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#ಅನ್ಯಥಾ
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#endif

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    ಯದಿ (
#ಯದಿ defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#endif
			    p->indir == PV_FT)
		    {
			ಯದಿ (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    ಪ್ರತ್ಯಾಗಮ FAIL;
			do_endif = TRUE;
		    }
		    ಯದಿ (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ಯದಿ (do_endif)
		    {
			ಯದಿ (put_ಸಾಲು(fd, "endif") == FAIL)
			    ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		}
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    ಪೂರ್ಣಾಙ್ಕ
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_version < SCRIPT_VERSION_VIM9;
# endif

    ಯದಿ (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
put_setstring(
    FILE	*fd,
    bool	legacy,
    ವರ್ಣ	*cmd,
    ವರ್ಣ	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = ರಿಕ್ತ;
    char_u	*part = ರಿಕ್ತ;
    char_u	*p;

    ಯದಿ (legacy && fprintf(fd, "legacy ") < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (fprintf(fd, "%s %s=", cmd, name) < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (*valuep != ರಿಕ್ತ)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	ಯದಿ (valuep == &p_pt)
	{
	    s = *valuep;
	    ಯಾವತ್ (*s != NUL)
		ಯದಿ (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	// expand the option value, replace $HOME by ~
	ಅನ್ಯಥಾ ಯದಿ ((flags & P_EXPAND) != 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ  size = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(size);
	    ಯದಿ (buf == ರಿಕ್ತ)
		ಗಚ್ಛ fail;
	    home_replace(ರಿಕ್ತ, *valuep, buf, size, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    ಯದಿ (size >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != ರಿಕ್ತ)
	    {
		part = alloc(size);
		ಯದಿ (part == ರಿಕ್ತ)
		    ಗಚ್ಛ fail;

		// write line break to clear the option, e.g. ':set rtp='
		ಯದಿ (put_eol(fd) == FAIL)
		    ಗಚ್ಛ fail;

		p = buf;
		ಯಾವತ್ (*p != NUL)
		{
		    ಯದಿ (legacy && fprintf(fd, "legacy ") < 0)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    ಯದಿ (fprintf(fd, "%s %s+=", cmd, name) < 0)
			ಗಚ್ಛ fail;
		    (ಶೂನ್ಯ)copy_option_part(&p, part, size,  ",");
		    ಯದಿ (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			ಗಚ್ಛ fail;
		}
		vim_free(buf);
		vim_free(part);
		ಪ್ರತ್ಯಾಗಮ OK;
	    }
	    ಯದಿ (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    vim_free(buf);
	}
	ಅನ್ಯಥಾ ಯದಿ (put_escstr(fd, *valuep, 2) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (put_eol(fd) < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
fail:
    vim_free(buf);
    vim_free(part);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
put_setnum(
    FILE	*fd,
    ವರ್ಣ	*cmd,
    ವರ್ಣ	*name,
    long	*valuep)
{
    long	wc;

    ಯದಿ (fprintf(fd, "%s %s=", cmd, name) < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	ಯದಿ (fputs((ವರ್ಣ *)get_special_ಕೀಲಿ_name((ಪೂರ್ಣಾಙ್ಕ)wc, 0), fd) < 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (fprintf(fd, "%ld", *valuep) < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (put_eol(fd) < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
put_setbool(
    FILE	*fd,
    ವರ್ಣ	*cmd,
    ವರ್ಣ	*name,
    ಪೂರ್ಣಾಙ್ಕ		ಮೌಲ್ಯ)
{
    ಯದಿ (ಮೌಲ್ಯ < 0)	// global/local option using global value
	ಪ್ರತ್ಯಾಗಮ OK;
    ಯದಿ (fprintf(fd, "%s %s%s", cmd, ಮೌಲ್ಯ ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
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
#ಯದಿ defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    ಯದಿ (gui.starting)
	clear_xterm_clip();
#endif
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    ಶೂನ್ಯ
free_termoptions(ಶೂನ್ಯ)
{
    ಸಂರಚನಾ vimoption   *p;

    ಪ್ರತಿ (p = options; p->fullname != ರಿಕ್ತ; p++)
	ಯದಿ (istermoption(p))
	{
	    ಯದಿ (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    ಯದಿ (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = empty_option;
	    p->def_val[VI_DEFAULT] = empty_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ifdef FEAT_EVAL
	    // remember where the option was cleared
	    set_option_sctx_idx((ಪೂರ್ಣಾಙ್ಕ)(p - options), OPT_GLOBAL, current_sctx);
#endif
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
    ಸಂರಚನಾ vimoption   *p;

    ಪ್ರತಿ (p = &options[0]; p->fullname != ರಿಕ್ತ; p++)
	ಯದಿ (p->var == var)
	{
	    ಯದಿ (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = empty_option;
	    p->flags &= ~P_ALLOCED;
	    ಭಙ್ಗ;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    ಶೂನ್ಯ
set_term_defaults(ಶೂನ್ಯ)
{
    ಸಂರಚನಾ vimoption   *p;

    ಪ್ರತಿ (p = &options[0]; p->fullname != ರಿಕ್ತ; p++)
    {
	ಯದಿ (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    ಯದಿ (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    ಯದಿ (p->flags & P_ALLOCED)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
istermoption(ಸಂರಚನಾ vimoption *p)
{
    ಪ್ರತ್ಯಾಗಮ (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
istermoption_idx(ಪೂರ್ಣಾಙ್ಕ opt_idx)
{
    ಪ್ರತ್ಯಾಗಮ istermoption(&options[opt_idx]);
}

#ಯದಿ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    ಶೂನ್ಯ
unset_global_local_option(char_u *name, ಶೂನ್ಯ *from)
{
    ಸಂರಚನಾ vimoption *p;
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    ಯದಿ (opt_idx < 0)
	ಪ್ರತ್ಯಾಗಮ;
    p = &(options[opt_idx]);

    ವಿಕಲ್ಪ ((ಪೂರ್ಣಾಙ್ಕ)p->indir)
    {
	// global option with local value: use local value if it's been set
	ಪಕ್ಷ PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_PATH:
	    clear_string_option(&buf->b_p_path);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_AC:
	    buf->b_p_ac = -1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_AR:
	    buf->b_p_ar = -1;
	    ಭಙ್ಗ;
# ifdef HAVE_FSYNC
	ಪಕ್ಷ PV_FS:
	    buf->b_p_fs = -1;
	    ಭಙ್ಗ;
# endif
	ಪಕ್ಷ PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_TAGS:
	    clear_string_option(&buf->b_p_tags);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_SISO:
	    curwin->w_p_siso = -1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_SO:
	    curwin->w_p_so = -1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_SOP:
	    curwin->w_p_sop = -1;
	    ಭಙ್ಗ;
# ifdef FEAT_FIND_ID
	ಪಕ್ಷ PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    ಭಙ್ಗ;
# endif
	ಪಕ್ಷ PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    ಭಙ್ಗ;
# ifdef FEAT_DIFF
	ಪಕ್ಷ PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    ಭಙ್ಗ;
# endif
	ಪಕ್ಷ PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    ಭಙ್ಗ;
# ifdef FEAT_COMPL_FUNC
	ಪಕ್ಷ PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    ಭಙ್ಗ;
# endif
	ಪಕ್ಷ PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    ಭಙ್ಗ;
# ifdef FEAT_EVAL
	ಪಕ್ಷ PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    ಭಙ್ಗ;
# endif
# ifdef FEAT_QUICKFIX
	ಪಕ್ಷ PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    ಭಙ್ಗ;
# endif
# ಯದಿ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	ಪಕ್ಷ PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    ಭಙ್ಗ;
# endif
# ಯದಿ defined(FEAT_CRYPT)
	ಪಕ್ಷ PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    ಭಙ್ಗ;
# endif
# ifdef FEAT_LINEBREAK
	ಪಕ್ಷ PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    ಭಙ್ಗ;
# endif
# ifdef FEAT_STL_OPT
	ಪಕ್ಷ PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    ಭಙ್ಗ;
# endif
	ಪಕ್ಷ PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    set_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      ರಿಕ್ತ, 0);
	    redraw_later(UPD_NOT_VALID);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    set_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      ರಿಕ್ತ, 0);
	    redraw_later(UPD_NOT_VALID);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    ಭಙ್ಗ;
    }
}
#endif

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    ಸ್ಥಿರ char_u *
get_varp_scope(ಸಂರಚನಾ vimoption *p, ಪೂರ್ಣಾಙ್ಕ scope)
{
    ಯದಿ ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	ಯದಿ (p->var == VAR_WIN)
	    ಪ್ರತ್ಯಾಗಮ (char_u *)GLOBAL_WO(get_varp(p));
	ಪ್ರತ್ಯಾಗಮ p->var;
    }
    ಯದಿ ((scope & OPT_LOCAL) && ((ಪೂರ್ಣಾಙ್ಕ)p->indir & PV_BOTH))
    {
	ವಿಕಲ್ಪ ((ಪೂರ್ಣಾಙ್ಕ)p->indir)
	{
	    ಪಕ್ಷ PV_FP:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_fp);
#ifdef HAVE_FSYNC
	    ಪಕ್ಷ PV_FS:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_fs);
#endif
#ifdef FEAT_EVAL
	    ಪಕ್ಷ PV_FFU: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ffu);
#endif
#ifdef FEAT_QUICKFIX
	    ಪಕ್ಷ PV_EFM:  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_efm);
	    ಪಕ್ಷ PV_GEFM: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_gefm);
	    ಪಕ್ಷ PV_GP:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_gp);
	    ಪಕ್ಷ PV_MP:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_mp);
#endif
	    ಪಕ್ಷ PV_EP:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ep);
	    ಪಕ್ಷ PV_KP:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_kp);
	    ಪಕ್ಷ PV_PATH: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_path);
	    ಪಕ್ಷ PV_AC:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ac);
	    ಪಕ್ಷ PV_AR:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ar);
	    ಪಕ್ಷ PV_TAGS: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_tags);
	    ಪಕ್ಷ PV_TC:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_tc);
	    ಪಕ್ಷ PV_SISO: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_siso);
	    ಪಕ್ಷ PV_SO:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_so);
	    ಪಕ್ಷ PV_SOP:  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_sop);
#ifdef FEAT_FIND_ID
	    ಪಕ್ಷ PV_DEF:  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_def);
	    ಪಕ್ಷ PV_INC:  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_inc);
#endif
	    ಪಕ್ಷ PV_COT:  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_cot);
	    ಪಕ್ಷ PV_DICT: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_dict);
#ifdef FEAT_DIFF
	    ಪಕ್ಷ PV_DIA:  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_dia);
#endif
	    ಪಕ್ಷ PV_TSR:  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_tsr);
#ifdef FEAT_COMPL_FUNC
	    ಪಕ್ಷ PV_TSRFU: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_tsrfu);
#endif
#ಯದಿ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    ಪಕ್ಷ PV_BEXPR: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_bexpr);
#endif
#ಯದಿ defined(FEAT_CRYPT)
	    ಪಕ್ಷ PV_CM:	  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_cm);
#endif
#ifdef FEAT_LINEBREAK
	    ಪಕ್ಷ PV_SBR:  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_sbr);
#endif
#ifdef FEAT_STL_OPT
	    ಪಕ್ಷ PV_STL:  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_stl);
	    ಪಕ್ಷ PV_STLO: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_stlo);
#endif
	    ಪಕ್ಷ PV_UL:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ul);
	    ಪಕ್ಷ PV_LW:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_lw);
	    ಪಕ್ಷ PV_BKC:  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_bkc);
	    ಪಕ್ಷ PV_MENC: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_menc);
	    ಪಕ್ಷ PV_LCS:  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_lcs);
	    ಪಕ್ಷ PV_FCS:  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_fcs);
	    ಪಕ್ಷ PV_VE:	  ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_ve);

	}
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; // "cannot happen"
    }
    ಪ್ರತ್ಯಾಗಮ get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(ಪೂರ್ಣಾಙ್ಕ opt_idx, ಪೂರ್ಣಾಙ್ಕ scope)
{
    ಪ್ರತ್ಯಾಗಮ get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    ಸ್ಥಿರ char_u *
get_varp(ಸಂರಚನಾ vimoption *p)
{
    // hidden option, always return NULL
    ಯದಿ (p->var == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ವಿಕಲ್ಪ ((ಪೂರ್ಣಾಙ್ಕ)p->indir)
    {
	ಪಕ್ಷ PV_NONE:	ಪ್ರತ್ಯಾಗಮ p->var;

	// global option with local value: use local value if it's been set
	ಪಕ್ಷ PV_EP:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	ಪಕ್ಷ PV_KP:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	ಪಕ್ಷ PV_PATH:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_path != NUL
				    ? (char_u *)&(curbuf->b_p_path) : p->var;
	ಪಕ್ಷ PV_AC:	ಪ್ರತ್ಯಾಗಮ curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	ಪಕ್ಷ PV_AR:	ಪ್ರತ್ಯಾಗಮ curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	ಪಕ್ಷ PV_TAGS:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_tags != NUL
				    ? (char_u *)&(curbuf->b_p_tags) : p->var;
	ಪಕ್ಷ PV_TC:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	ಪಕ್ಷ PV_BKC:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	ಪಕ್ಷ PV_SISO:	ಪ್ರತ್ಯಾಗಮ curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	ಪಕ್ಷ PV_SO:	ಪ್ರತ್ಯಾಗಮ curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	ಪಕ್ಷ PV_SOP:	ಪ್ರತ್ಯಾಗಮ curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ifdef FEAT_FIND_ID
	ಪಕ್ಷ PV_DEF:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	ಪಕ್ಷ PV_INC:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#endif
	ಪಕ್ಷ PV_COT:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	ಪಕ್ಷ PV_DICT:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ifdef FEAT_DIFF
	ಪಕ್ಷ PV_DIA:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#endif
	ಪಕ್ಷ PV_TSR:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ifdef FEAT_COMPL_FUNC
	ಪಕ್ಷ PV_TSRFU:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#endif
	ಪಕ್ಷ PV_FP:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ifdef HAVE_FSYNC
	ಪಕ್ಷ PV_FS:	ಪ್ರತ್ಯಾಗಮ curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#endif
#ifdef FEAT_EVAL
	ಪಕ್ಷ PV_FFU:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#endif
#ifdef FEAT_QUICKFIX
	ಪಕ್ಷ PV_EFM:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	ಪಕ್ಷ PV_GEFM:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	ಪಕ್ಷ PV_GP:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	ಪಕ್ಷ PV_MP:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#endif
#ಯದಿ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	ಪಕ್ಷ PV_BEXPR:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#endif
#ಯದಿ defined(FEAT_CRYPT)
	ಪಕ್ಷ PV_CM:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#endif
#ifdef FEAT_LINEBREAK
	ಪಕ್ಷ PV_SBR:	ಪ್ರತ್ಯಾಗಮ *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#endif
#ifdef FEAT_STL_OPT
	ಪಕ್ಷ PV_STL:	ಪ್ರತ್ಯಾಗಮ *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	ಪಕ್ಷ PV_STLO:	ಪ್ರತ್ಯಾಗಮ *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#endif
	ಪಕ್ಷ PV_UL:	ಪ್ರತ್ಯಾಗಮ curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	ಪಕ್ಷ PV_LW:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	ಪಕ್ಷ PV_MENC:	ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ifdef FEAT_ARABIC
	ಪಕ್ಷ PV_ARAB:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_arab);
#endif
	ಪಕ್ಷ PV_LIST:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_list);
	ಪಕ್ಷ PV_LCS:	ಪ್ರತ್ಯಾಗಮ *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	ಪಕ್ಷ PV_FCS:	ಪ್ರತ್ಯಾಗಮ *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	ಪಕ್ಷ PV_VE:	ಪ್ರತ್ಯಾಗಮ *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ifdef FEAT_SPELL
	ಪಕ್ಷ PV_SPELL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_spell);
#endif
#ifdef FEAT_SYN_HL
	ಪಕ್ಷ PV_CUC:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_cuc);
	ಪಕ್ಷ PV_CUL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_cul);
	ಪಕ್ಷ PV_CULOPT:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_culopt);
	ಪಕ್ಷ PV_CC:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_cc);
#endif
#ifdef FEAT_DIFF
	ಪಕ್ಷ PV_DIFF:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_diff);
#endif
	ಪಕ್ಷ PV_EIW:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_eiw);
#ifdef FEAT_FOLDING
	ಪಕ್ಷ PV_FDC:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_fdc);
	ಪಕ್ಷ PV_FEN:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_fen);
	ಪಕ್ಷ PV_FDI:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_fdi);
	ಪಕ್ಷ PV_FDL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_fdl);
	ಪಕ್ಷ PV_FDM:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_fdm);
	ಪಕ್ಷ PV_FML:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_fml);
	ಪಕ್ಷ PV_FDN:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	ಪಕ್ಷ PV_FDE:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_fde);
	ಪಕ್ಷ PV_FDT:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_fdt);
# endif
	ಪಕ್ಷ PV_FMR:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_fmr);
#endif
	ಪಕ್ಷ PV_NU:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_nu);
	ಪಕ್ಷ PV_RNU:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_rnu);
#ifdef FEAT_LINEBREAK
	ಪಕ್ಷ PV_NUW:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_nuw);
#endif
	ಪಕ್ಷ PV_WFB:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_wfb);
	ಪಕ್ಷ PV_WFH:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_wfh);
	ಪಕ್ಷ PV_WFW:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_wfw);
#ಯದಿ defined(FEAT_QUICKFIX)
	ಪಕ್ಷ PV_PVW:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_pvw);
	ಪಕ್ಷ PV_LHI:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_lhi);
#endif
#ifdef FEAT_RIGHTLEFT
	ಪಕ್ಷ PV_RL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_rl);
	ಪಕ್ಷ PV_RLC:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_rlc);
#endif
	ಪಕ್ಷ PV_SCROLL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_scr);
	ಪಕ್ಷ PV_SMS:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_sms);
	ಪಕ್ಷ PV_WRAP:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_wrap);
#ifdef FEAT_LINEBREAK
	ಪಕ್ಷ PV_LBR:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_lbr);
	ಪಕ್ಷ PV_BRI:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_bri);
	ಪಕ್ಷ PV_BRIOPT: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_briopt);
#endif
	ಪಕ್ಷ PV_WCR:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_wcr);
	ಪಕ್ಷ PV_SCBIND: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_scb);
	ಪಕ್ಷ PV_CRBIND: ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_crb);
#ifdef FEAT_CONCEAL
	ಪಕ್ಷ PV_COCU:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_cocu);
	ಪಕ್ಷ PV_COLE:   ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_cole);
#endif
	ಪಕ್ಷ PV_WHL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_whl);

#ifdef FEAT_TERMINAL
	ಪಕ್ಷ PV_TWK:    ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_twk);
	ಪಕ್ಷ PV_TWS:    ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_tws);
	ಪಕ್ಷ PV_TWSL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_twsl);
#endif

	ಪಕ್ಷ PV_AI:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ai);
	ಪಕ್ಷ PV_BIN:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_bin);
	ಪಕ್ಷ PV_BOMB:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_bomb);
	ಪಕ್ಷ PV_BH:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_bh);
	ಪಕ್ಷ PV_BT:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_bt);
	ಪಕ್ಷ PV_BL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_bl);
	ಪಕ್ಷ PV_CI:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ci);
	ಪಕ್ಷ PV_CIN:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_cin);
	ಪಕ್ಷ PV_CINK:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_cink);
	ಪಕ್ಷ PV_CINO:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_cino);
	ಪಕ್ಷ PV_CINSD:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_cinsd);
	ಪಕ್ಷ PV_CINW:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_cinw);
	ಪಕ್ಷ PV_COM:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_com);
	ಪಕ್ಷ PV_CMS:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_cms);
	ಪಕ್ಷ PV_CPT:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_cpt);
#ifdef BACKSLASH_IN_FILENAME
	ಪಕ್ಷ PV_CSL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_csl);
#endif
#ifdef FEAT_COMPL_FUNC
	ಪಕ್ಷ PV_CFU:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_cfu);
	ಪಕ್ಷ PV_OFU:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ofu);
#endif
#ifdef FEAT_EVAL
	ಪಕ್ಷ PV_TFU:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_tfu);
#endif
	ಪಕ್ಷ PV_EOF:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_eof);
	ಪಕ್ಷ PV_EOL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_eol);
	ಪಕ್ಷ PV_FIXEOL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_fixeol);
	ಪಕ್ಷ PV_ET:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_et);
	ಪಕ್ಷ PV_FENC:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_fenc);
	ಪಕ್ಷ PV_FF:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ff);
	ಪಕ್ಷ PV_FT:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ft);
	ಪಕ್ಷ PV_FO:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_fo);
	ಪಕ್ಷ PV_FLP:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_flp);
	ಪಕ್ಷ PV_IMI:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_iminsert);
	ಪಕ್ಷ PV_IMS:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_imsearch);
	ಪಕ್ಷ PV_INF:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_inf);
	ಪಕ್ಷ PV_ISK:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_isk);
#ifdef FEAT_FIND_ID
# ifdef FEAT_EVAL
	ಪಕ್ಷ PV_INEX:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_inex);
# endif
#endif
#ಯದಿ defined(FEAT_EVAL)
	ಪಕ್ಷ PV_INDE:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_inde);
	ಪಕ್ಷ PV_INDK:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_indk);
#endif
#ifdef FEAT_EVAL
	ಪಕ್ಷ PV_FEX:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
	ಪಕ್ಷ PV_ಕೀಲಿ:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ಕೀಲಿ);
#endif
	ಪಕ್ಷ PV_LISP:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_lisp);
	ಪಕ್ಷ PV_LOP:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_lop);
	ಪಕ್ಷ PV_ML:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ml);
	ಪಕ್ಷ PV_MPS:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_mps);
	ಪಕ್ಷ PV_MA:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ma);
	ಪಕ್ಷ PV_MOD:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_changed);
	ಪಕ್ಷ PV_NF:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_nf);
	ಪಕ್ಷ PV_PI:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_pi);
	ಪಕ್ಷ PV_QE:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_qe);
	ಪಕ್ಷ PV_RO:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ro);
	ಪಕ್ಷ PV_SI:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_si);
	ಪಕ್ಷ PV_SN:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_sn);
	ಪಕ್ಷ PV_STS:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_sts);
	ಪಕ್ಷ PV_SUA:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_sua);
	ಪಕ್ಷ PV_SWF:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_swf);
#ifdef FEAT_SYN_HL
	ಪಕ್ಷ PV_SMC:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_smc);
	ಪಕ್ಷ PV_SYN:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_syn);
#endif
#ifdef FEAT_SPELL
	ಪಕ್ಷ PV_SPC:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_s->b_p_spc);
	ಪಕ್ಷ PV_SPF:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_s->b_p_spf);
	ಪಕ್ಷ PV_SPL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_s->b_p_spl);
	ಪಕ್ಷ PV_SPO:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_s->b_p_spo);
#endif
	ಪಕ್ಷ PV_SW:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_sw);
	ಪಕ್ಷ PV_TS:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_ts);
	ಪಕ್ಷ PV_TW:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_tw);
	ಪಕ್ಷ PV_TX:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_tx);
#ifdef FEAT_PERSISTENT_UNDO
	ಪಕ್ಷ PV_UDF:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_udf);
#endif
	ಪಕ್ಷ PV_WM:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_wm);
#ifdef FEAT_KEYMAP
	ಪಕ್ಷ PV_KMAP:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_keymap);
#endif
#ifdef FEAT_SIGNS
	ಪಕ್ಷ PV_SCL:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curwin->w_p_scl);
#endif
#ifdef FEAT_VARTABS
	ಪಕ್ಷ PV_VSTS:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_vsts);
	ಪಕ್ಷ PV_VTS:	ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_vts);
#endif
	ಸ್ವಭಾವ:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    ಪ್ರತ್ಯಾಗಮ (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(ಪೂರ್ಣಾಙ್ಕ opt_idx)
{
    ಪ್ರತ್ಯಾಗಮ options[opt_idx].var;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(ಪೂರ್ಣಾಙ್ಕ opt_idx)
{
    ಪ್ರತ್ಯಾಗಮ (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    ಪೂರ್ಣಾಙ್ಕ
option_info(char_u *name, dict_T *d)
{
    ಪೂರ್ಣಾಙ್ಕ			opt_idx;
    ಸಂರಚನಾ vimoption	*p;
    ಪೂರ್ಣಾಙ್ಕ			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    ಯದಿ ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    ಯದಿ (opt_idx < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    p = &options[opt_idx];
    dict_add_string(d, "name", (char_u *)p->fullname);
    dict_add_string(d, "shortname",
		    (char_u *)(p->shortname == ರಿಕ್ತ ? "" : p->shortname));
    dict_add_bool(d, "available", p->var != ರಿಕ್ತ);
    dict_add_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_ಅಂಕೆ) ? "number" : "string"));
    dict_add_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    ಯದಿ (p->flags & P_BOOL)
	dict_add_bool(d, "default", (ಪೂರ್ಣಾಙ್ಕ)(long_i)p->def_val[dvi]);
    ಅನ್ಯಥಾ ಯದಿ (p->flags & P_ಅಂಕೆ)
	dict_add_ಸಂಖ್ಯೆ(d, "default", (long)(long_i)p->def_val[dvi]);
    ಅನ್ಯಥಾ
	dict_add_string(d, "default", p->def_val[dvi] == ರಿಕ್ತ
					? (char_u *)"" : p->def_val[dvi]);
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_set_cb_T
get_option_did_set_cb(ಪೂರ್ಣಾಙ್ಕ opt_idx)
{
    ಪ್ರತ್ಯಾಗಮ options[opt_idx].opt_did_set_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(ಶೂನ್ಯ)
{
    ಯದಿ (*curbuf->b_p_ep == NUL)
	ಪ್ರತ್ಯಾಗಮ p_ep;
    ಪ್ರತ್ಯಾಗಮ curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(ಶೂನ್ಯ)
{
#ifdef FEAT_EVAL
    ಯದಿ (*curbuf->b_p_ffu == NUL)
	ಪ್ರತ್ಯಾಗಮ p_ffu;
    ಪ್ರತ್ಯಾಗಮ curbuf->b_p_ffu;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ (char_u *)"";
#endif
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    ಶೂನ್ಯ
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
    ಶೂನ್ಯ
after_copy_winopt(win_T *wp)
{
    ವರ್ಣ *errmsg = update_winhighlight(wp, wp->w_p_whl);

    ಯದಿ (errmsg != ರಿಕ್ತ)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    ಯದಿ (wp->w_p_wrap)
	wp->w_leftcol = 0;
    ಅನ್ಯಥಾ
	wp->w_skipcol = 0;
#ifdef FEAT_LINEBREAK
    briopt_check(ರಿಕ್ತ, wp);
#endif
#ifdef FEAT_SYN_HL
    fill_culopt_flags(ರಿಕ್ತ, wp);
    check_colorcolumn(ರಿಕ್ತ, wp);
#endif
    set_listchars_option(wp, wp->w_p_lcs, TRUE, ರಿಕ್ತ, 0);
    set_fillchars_option(wp, wp->w_p_fcs, TRUE, ರಿಕ್ತ, 0);
}

    ಸ್ಥಿರ char_u *
copy_option_val(char_u *val)
{
    ಯದಿ (val == empty_option)
	ಪ್ರತ್ಯಾಗಮ empty_option;  // no need to allocate memory
    ಪ್ರತ್ಯಾಗಮ vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    ಶೂನ್ಯ
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
						    ಪರಿಮಾಣ(to->wo_script_ctx));
#endif
    check_winopt(to);		// don't want NULL pointers
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
    ಶೂನ್ಯ
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ buf_opt_idx[BV_ಎಣಿಕೆ];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_buf_opt_idx(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ did_init_buf_opt_idx = FALSE;
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (did_init_buf_opt_idx)
	ಪ್ರತ್ಯಾಗಮ;
    did_init_buf_opt_idx = TRUE;
    ಪ್ರತಿ (i = 0; !istermoption_idx(i); i++)
	ಯದಿ (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_MASK] = i;
}
#ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
buf_copy_options(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಪೂರ್ಣಾಙ್ಕ		should_copy = TRUE;
    char_u	*save_p_isk = ರಿಕ್ತ;	    // init for GCC
    ಪೂರ್ಣಾಙ್ಕ		dont_do_help;
    ಪೂರ್ಣಾಙ್ಕ		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    ಯದಿ (p_cpo != ರಿಕ್ತ)
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
	ಯದಿ ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == ರಿಕ್ತ || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != ರಿಕ್ತ)))
	    should_copy = FALSE;

	ಯದಿ (should_copy || (flags & BCO_ALWAYS))
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
	    ಯದಿ (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = ರಿಕ್ತ;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    ಯದಿ (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		ವಿಕಲ್ಪ (*p_ffs)
		{
		    ಪಕ್ಷ 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); ಭಙ್ಗ;
		    ಪಕ್ಷ 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); ಭಙ್ಗ;
		    ಪಕ್ಷ 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); ಭಙ್ಗ;
		    ಸ್ವಭಾವ:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		ಯದಿ (buf->b_p_ff != ರಿಕ್ತ)
		    buf->b_start_ffc = *buf->b_p_ff;
		buf->b_p_bh = empty_option;
		buf->b_p_bt = empty_option;
	    }
	    ಅನ್ಯಥಾ
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
	    ಯದಿ (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    ಅನ್ಯಥಾ
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
	    ಯದಿ (p_vsts && p_vsts != empty_option)
		(ಶೂನ್ಯ)tabstop_set(p_vsts, &buf->b_p_vsts_array);
	    ಅನ್ಯಥಾ
		buf->b_p_vsts_array = ರಿಕ್ತ;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : ರಿಕ್ತ;
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
	    (ಶೂನ್ಯ)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#endif
#ಯದಿ defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#endif
	    buf->b_p_fp = empty_option;
#ಯದಿ defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#endif
#ifdef FEAT_CRYPT
	    buf->b_p_ಕೀಲಿ = vim_strsave(p_ಕೀಲಿ);
	    COPY_OPT_SCTX(buf, BV_ಕೀಲಿ);
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
#ಯದಿ defined(FEAT_EVAL)
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
#ಯದಿ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = empty_option;
#endif
#ಯದಿ defined(FEAT_CRYPT)
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
	    ಯದಿ (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ifdef FEAT_VARTABS
		ಯದಿ (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (ಶೂನ್ಯ)tabstop_set(p_vts, &buf->b_p_vts_array);
		ಅನ್ಯಥಾ
		    buf->b_p_vts_array = ರಿಕ್ತ;
#endif
	    }
	    ಅನ್ಯಥಾ
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#ifdef FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		ಯದಿ (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (ಶೂನ್ಯ)tabstop_set(p_vts, &buf->b_p_vts_array);
		ಅನ್ಯಥಾ
		    buf->b_p_vts_array = ರಿಕ್ತ;
#endif
		buf->b_help = false;
		ಯದಿ (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	ಯದಿ (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    ಯದಿ (did_isk)
	(ಶೂನ್ಯ)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    ಶೂನ್ಯ
reset_modifiable(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    ಯದಿ (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}

/*
 * Set the global value for 'iminsert' to the local value.
 */
    ಶೂನ್ಯ
set_iminsert_global(ಶೂನ್ಯ)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    ಶೂನ್ಯ
set_imsearch_global(ಶೂನ್ಯ)
{
    p_imsearch = curbuf->b_p_imsearch;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ expand_option_idx = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ expand_option_start_col = 0;
ಸ್ಥಿರ char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ expand_option_flags = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ expand_option_append = FALSE;

    ಶೂನ್ಯ
set_context_in_set_cmd(
    expand_T	*xp,
    char_u	*arg,
    ಪೂರ್ಣಾಙ್ಕ		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    ಪೂರ್ಣಾಙ್ಕ		nextchar;
    long_u	flags = 0;	// init for GCC
    ಪೂರ್ಣಾಙ್ಕ		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		is_term_option = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		ಕೀಲಿ;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    ಯದಿ (*arg == NUL)
    {
	xp->xp_pattern = arg;
	ಪ್ರತ್ಯಾಗಮ;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    ಯದಿ (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_pattern = p + 1;
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯಾವತ್ (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	ಯದಿ (*p == ' ' || *p == ',')
	{
	    ಯಾವತ್ (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	ಯದಿ (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    ಭಙ್ಗ;
	}
	--p;
    }
    ಯದಿ (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_NO;
	p += 2;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_INV;
	p += 3;
    }
    xp->xp_pattern = arg = p;
    ಯದಿ (*arg == '<')
    {
	ಯಾವತ್ (*p != '>')
	    ಯದಿ (*p++ == NUL)	    // expand terminal option name
		ಪ್ರತ್ಯಾಗಮ;
	ಕೀಲಿ = get_special_ಕೀಲಿ_code(arg + 1);
	ಯದಿ (ಕೀಲಿ == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(ಕೀಲಿ);
	expand_option_name[3] = KEY2TERMCAP1(ಕೀಲಿ);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    ಯದಿ (*p != NUL)
		++p;
	    ಯದಿ (*p == NUL)
		ಪ್ರತ್ಯಾಗಮ;		// expand option name
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	ಅನ್ಯಥಾ
	{
	    // Allow * wildcard
	    ಯಾವತ್ (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    ಯದಿ (*p == NUL)
		ಪ್ರತ್ಯಾಗಮ;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    ಯದಿ (opt_idx == -1 || options[opt_idx].var == ರಿಕ್ತ)
	    {
		xp->xp_context = EXPAND_NOTHING;
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    flags = options[opt_idx].flags;
	    ಯದಿ (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    ಪೂರ್ಣಾಙ್ಕ expand_option_subtract = FALSE;
    ಯದಿ ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	ಯದಿ (nextchar == '-')
	    expand_option_subtract = TRUE;
	ಯದಿ (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    ಯದಿ ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    ಯದಿ (is_term_option)
	expand_option_idx = -1;
    ಅನ್ಯಥಾ
	expand_option_idx = opt_idx;

    ಯದಿ (!is_term_option)
    {
	ಯದಿ (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    xp->xp_pattern = p + 1;
    expand_option_start_col = (ಪೂರ್ಣಾಙ್ಕ)(p + 1 - xp->xp_ಸಾಲು);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ifdef FEAT_SYN_HL
    ಯದಿ (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif
    ಯದಿ (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	ಪ್ರತ್ಯಾಗಮ;
    }
#ifdef FEAT_KEYMAP
    ಯದಿ (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    ಯದಿ (expand_option_subtract)
    {
	xp->xp_context = EXPAND_SETTING_SUBTRACT;
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ ಯದಿ (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != ರಿಕ್ತ)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    ಅನ್ಯಥಾ ಯದಿ (*xp->xp_pattern == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ
	xp->xp_context = EXPAND_NOTHING;

    ಯದಿ (is_term_option || (flags & P_ಅಂಕೆ))
	ಪ್ರತ್ಯಾಗಮ;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    ಯದಿ (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	ಯದಿ (p == (char_u *)&p_bdir
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
	    ಯದಿ (p == (char_u *)&p_path || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    ಅನ್ಯಥಾ
		xp->xp_backslash = XP_BS_ONE;
	}
	ಅನ್ಯಥಾ
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    ಯದಿ (p == (char_u *)&p_tags)
		xp->xp_backslash = XP_BS_THREE;
	    ಅನ್ಯಥಾ
		xp->xp_backslash = XP_BS_ONE;
	}
	ಯದಿ (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    ಯದಿ ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	ಪ್ರತಿ (p = argend - 1; p >= xp->xp_pattern; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    ಯದಿ (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		ಯಾವತ್ (s > xp->xp_pattern && *(s - 1) == '\\')
		    --s;
		ಯದಿ ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#ಯದಿ defined(BACKSLASH_IN_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#ಅನ್ಯಥಾ
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#endif
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_pattern = p + 1;
		    ಭಙ್ಗ;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    ಯದಿ (flags & P_FLAGLIST)
	xp->xp_pattern = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ifdef FEAT_SPELL
    ಯದಿ (options[opt_idx].var == (char_u *)&p_sps)
    {
	ಯದಿ (STRNCMP(xp->xp_pattern, "file:", 5) == 0)
	{
	    xp->xp_pattern += 5;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಅನ್ಯಥಾ ಯದಿ (options[expand_option_idx].opt_expand_cb != ರಿಕ್ತ)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
match_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	ಪೂರ್ಣಾಙ್ಕ		idx,
	ಪೂರ್ಣಾಙ್ಕ		test_only,
	ಪೂರ್ಣಾಙ್ಕ		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    ಯದಿ (!fuzzy)
    {
	ಯದಿ (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    ಯದಿ (!test_only)
		matches[idx] = vim_strsave(str);
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ score;

	score = fuzzy_match_str(str, fuzzystr);
	ಯದಿ (score != FUZZY_SCORE_NONE)
	{
	    ಯದಿ (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಪೂರ್ಣಾಙ್ಕ
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches,
    char_u	***matches,
    ಪೂರ್ಣಾಙ್ಕ		can_fuzzy)
{
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_normal = 0;	    // Nr of matching non-term-code settings
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_term = 0;	    // Nr of matching terminal code settings
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;
    ಪೂರ್ಣಾಙ್ಕ		match;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    char_u	*str;
    ಪೂರ್ಣಾಙ್ಕ		loop;
    ಪೂರ್ಣಾಙ್ಕ		is_term_opt;
    char_u	name_buf[MAX_ಕೀಲಿ_NAME_LEN];
    ಸ್ಥಿರ ವರ್ಣ *(names[]) = {"all", "termcap"};
    ಪೂರ್ಣಾಙ್ಕ		ic = regmatch->rm_ic;	// remember the ignore-case flag
    ಪೂರ್ಣಾಙ್ಕ		fuzzy;
    fuzmatch_str_T  *fuzmatch = ರಿಕ್ತ;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    ಪ್ರತಿ (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	ಯದಿ (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    ಪ್ರತಿ (match = 0; match < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(names); ++match)
	    {
		ಯದಿ (match_str((char_u *)names[match], regmatch, *matches,
			    ಎಣಿಕೆ, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    ಯದಿ (loop == 0)
			ಅಂಕೆ_normal++;
		    ಅನ್ಯಥಾ
			ಎಣಿಕೆ++;
		}
	    }
	}
	ಪ್ರತಿ (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != ರಿಕ್ತ;
								    opt_idx++)
	{
	    ಯದಿ (options[opt_idx].var == ರಿಕ್ತ)
		ಅನುವರ್ತ;
	    ಯದಿ (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		ಅನುವರ್ತ;
	    is_term_opt = istermoption_idx(opt_idx);
	    ಯದಿ (is_term_opt && ಅಂಕೆ_normal > 0)
		ಅನುವರ್ತ;

	    ಯದಿ (match_str(str, regmatch, *matches, ಎಣಿಕೆ, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		ಯದಿ (loop == 0)
		{
		    ಯದಿ (is_term_opt)
			ಅಂಕೆ_term++;
		    ಅನ್ಯಥಾ
			ಅಂಕೆ_normal++;
		}
		ಅನ್ಯಥಾ
		    ಎಣಿಕೆ++;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (!fuzzy && options[opt_idx].shortname != ರಿಕ್ತ
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		ಯದಿ (loop == 0)
		{
		    ಯದಿ (is_term_opt)
			ಅಂಕೆ_term++;
		    ಅನ್ಯಥಾ
			ಅಂಕೆ_normal++;
		}
		ಅನ್ಯಥಾ
		    (*matches)[ಎಣಿಕೆ++] = vim_strsave(str);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		ಯದಿ (match_str(name_buf, regmatch, *matches, ಎಣಿಕೆ, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    ಯದಿ (loop == 0)
			ಅಂಕೆ_term++;
		    ಅನ್ಯಥಾ
			ಎಣಿಕೆ++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	ಯದಿ (xp->xp_context != EXPAND_BOOL_SETTINGS  && ಅಂಕೆ_normal == 0)
	{
	    ಪ್ರತಿ (opt_idx = 0; (str = get_termcode(opt_idx)) != ರಿಕ್ತ; opt_idx++)
	    {
		ಯದಿ (!SAFE_isprint(str[0]) || !SAFE_isprint(str[1]))
		    ಅನುವರ್ತ;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		ಯದಿ (match_str(name_buf, regmatch, *matches, ಎಣಿಕೆ,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    ಯದಿ (loop == 0)
			ಅಂಕೆ_term++;
		    ಅನ್ಯಥಾ
			ಎಣಿಕೆ++;
		}
		ಅನ್ಯಥಾ
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    ಯದಿ (match_str(name_buf, regmatch, *matches, ಎಣಿಕೆ,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			ಯದಿ (loop == 0)
			    ಅಂಕೆ_term++;
			ಅನ್ಯಥಾ
			    ಎಣಿಕೆ++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    ಪ್ರತಿ (opt_idx = 0; (str = get_ಕೀಲಿ_name(opt_idx)) != ರಿಕ್ತ; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		ಯದಿ (match_str(name_buf, regmatch, *matches, ಎಣಿಕೆ, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    ಯದಿ (loop == 0)
			ಅಂಕೆ_term++;
		    ಅನ್ಯಥಾ
			ಎಣಿಕೆ++;
		}
	    }
	}
	ಯದಿ (loop == 0)
	{
	    ಯದಿ (ಅಂಕೆ_normal > 0)
		*ಅಂಕೆ_Matches = ಅಂಕೆ_normal;
	    ಅನ್ಯಥಾ ಯದಿ (ಅಂಕೆ_term > 0)
		*ಅಂಕೆ_Matches = ಅಂಕೆ_term;
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಯದಿ (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *ಅಂಕೆ_Matches);
		ಯದಿ (*matches == ರಿಕ್ತ)
		{
		    *matches = (char_u **)"";
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *ಅಂಕೆ_Matches);
		ಯದಿ (fuzmatch == ರಿಕ್ತ)
		{
		    *matches = (char_u **)"";
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	}
    }

    ಯದಿ (fuzzy &&
	fuzzymatches_to_strmatches(fuzmatch, matches, ಎಣಿಕೆ, FALSE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

#ifdef BACKSLASH_IN_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    ಪ್ರತಿ (var = buf; *var != NUL; MB_PTR_ADV(var))
	ಯದಿ (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#endif
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    ಪೂರ್ಣಾಙ್ಕ
ExpandOldSetting(ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    char_u  *var = ರಿಕ್ತ;	// init for GCC
    char_u  *buf;

    *ಅಂಕೆ_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    ಯದಿ (*matches == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // For a terminal key code expand_option_idx is < 0.
    ಯದಿ (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	ಯದಿ (var == ರಿಕ್ತ)
	    expand_option_idx = findoption(expand_option_name);
    }

    ಯದಿ (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    ಅನ್ಯಥಾ ಯದಿ (var == ರಿಕ್ತ)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    ಯದಿ (buf == ರಿಕ್ತ)
    {
	VIM_CLEAR(*matches);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    (*matches)[0] = buf;
    *ಅಂಕೆ_Matches = 1;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    ಪೂರ್ಣಾಙ್ಕ
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches,
    char_u	***matches)
{
    char_u  *var = ರಿಕ್ತ;	// init for GCC
    char_u  *buf;

    ಯದಿ (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == ರಿಕ್ತ)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    optexpand_T args;
    args.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    args.oe_append = expand_option_append;
    args.oe_regmatch = regmatch;
    args.oe_xp = xp;
    args.oe_set_arg = xp->xp_ಸಾಲು + expand_option_start_col;
    args.oe_include_orig_val =
	!expand_option_append &&
	(*args.oe_set_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    args.oe_opt_ಮೌಲ್ಯ = buf;

    ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ret = options[expand_option_idx].opt_expand_cb(&args, ಅಂಕೆ_Matches, matches);

    vim_free(buf);
    ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_ret;
}

/*
 * Expansion handler for :set-=
 */
    ಪೂರ್ಣಾಙ್ಕ
ExpandSettingSubtract(
    expand_T	*xp,
    regmatch_T	*regmatch,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches,
    char_u	***matches)
{
    ಯದಿ (expand_option_idx < 0)
	// term option
	ಪ್ರತ್ಯಾಗಮ ExpandOldSetting(ಅಂಕೆ_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    ಯದಿ (option_flags & P_ಅಂಕೆ)
	ಪ್ರತ್ಯಾಗಮ ExpandOldSetting(ಅಂಕೆ_Matches, matches);
    ಅನ್ಯಥಾ ಯದಿ (option_flags & P_COMMA)
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

	ಯದಿ (*option_val == NUL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	ಯದಿ (option_copy == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	next_val = option_copy;

	ga_init2(&ga, ಪರಿಮಾಣ(char_u *), 10);

	ಕುರು
	{
	    item = next_val;
	    comma = vim_strchr(next_val, ',');
	    ಯಾವತ್ (comma != ರಿಕ್ತ && comma != next_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    ಯದಿ (comma != ರಿಕ್ತ)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		next_val = comma + 1;
	    }
	    ಅನ್ಯಥಾ
		next_val = ರಿಕ್ತ;

	    ಯದಿ (*item == NUL)
		// empty value, don't add to list
		ಅನುವರ್ತ;

	    ಯದಿ (!vim_regexec(regmatch, item, (colnr_T)0))
		ಅನುವರ್ತ;

	    char_u *buf = escape_option_str_cmdline(item);
	    ಯದಿ (buf == ರಿಕ್ತ)
	    {
		vim_free(option_copy);
		ga_clear_strings(&ga);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (ga_add_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		ಭಙ್ಗ;
	    }
	} ಯಾವತ್ (next_val != ರಿಕ್ತ);

	vim_free(option_copy);

	*matches = ga.ga_data;
	*ಅಂಕೆ_Matches = ga.ga_len;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಅನ್ಯಥಾ ಯದಿ (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	ಯದಿ (*xp->xp_pattern != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	size_t ಅಂಕೆ_flags = STRLEN(option_val);
	ಯದಿ (ಅಂಕೆ_flags == 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	*matches = ALLOC_MULT(char_u *, ಅಂಕೆ_flags + 1);
	ಯದಿ (*matches == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;
	char_u *p;

	p = vim_strnsave(option_val, ಅಂಕೆ_flags);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    VIM_CLEAR(*matches);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	(*matches)[ಎಣಿಕೆ++] = p;

	ಯದಿ (ಅಂಕೆ_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    ಪ್ರತಿ (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		ಯದಿ (p == ರಿಕ್ತ)
		    ಭಙ್ಗ;
		(*matches)[ಎಣಿಕೆ++] = p;
	    }
	}

	*ಅಂಕೆ_Matches = ಎಣಿಕೆ;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಪ್ರತ್ಯಾಗಮ ExpandOldSetting(ಅಂಕೆ_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
option_value2string(
    ಸಂರಚನಾ vimoption	*opp,
    ಪೂರ್ಣಾಙ್ಕ			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    ಯದಿ (opp->flags & P_ಅಂಕೆ)
    {
	long wc = 0;

	ಯದಿ (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_ಕೀಲಿ_name((ಪೂರ್ಣಾಙ್ಕ)wc, 0));
	ಅನ್ಯಥಾ ಯದಿ (wc != 0)
	    STRCPY(NameBuff, transchar((ಪೂರ್ಣಾಙ್ಕ)wc));
	ಅನ್ಯಥಾ
	    sprintf((ವರ್ಣ *)NameBuff, "%ld", *(long *)varp);
    }
    ಅನ್ಯಥಾ    // P_STRING
    {
	varp = *(char_u **)(varp);
	ಯದಿ (varp == ರಿಕ್ತ)		    // just in case
	    NameBuff[0] = NUL;
#ifdef FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	ಅನ್ಯಥಾ ಯದಿ (opp->var == (char_u *)&p_ಕೀಲಿ && *varp)
	    STRCPY(NameBuff, "*****");
#endif
	ಅನ್ಯಥಾ ಯದಿ (opp->flags & P_EXPAND)
	    home_replace(ರಿಕ್ತ, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	ಅನ್ಯಥಾ ಯದಿ ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	ಅನ್ಯಥಾ
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
wc_use_keyname(char_u *varp, long *wcp)
{
    ಯದಿ (((long *)varp == &p_wc) || ((long *)varp == &p_wcm))
    {
	*wcp = *(long *)varp;
	ಯದಿ (IS_SPECIAL(*wcp) || find_special_ಕೀಲಿ_in_table((ಪೂರ್ಣಾಙ್ಕ)*wcp) >= 0)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    ಪೂರ್ಣಾಙ್ಕ
shortmess(ಪೂರ್ಣಾಙ್ಕ x)
{
    ಪ್ರತ್ಯಾಗಮ p_shm != ರಿಕ್ತ &&
	    (   vim_strchr(p_shm, x) != ರಿಕ್ತ
	    || (vim_strchr(p_shm, 'a') != ರಿಕ್ತ
		&& vim_strchr((char_u *)SHM_A, x) != ರಿಕ್ತ));
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
vimrc_found(char_u *fname, char_u *envname)
{
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;
    ಪೂರ್ಣಾಙ್ಕ		dofree = FALSE;
    char_u	*p;

    ಯದಿ (!option_was_set((char_u *)"cp"))
    {
	p_cp = FALSE;
	ಪ್ರತಿ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    ಯದಿ (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    ಯದಿ (fname != ರಿಕ್ತ)
    {
	p = vim_getenv(envname, &dofree);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = FullName_save(fname, FALSE);
	    ಯದಿ (p != ರಿಕ್ತ)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	ಅನ್ಯಥಾ ಯದಿ (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    ಶೂನ್ಯ
change_compatible(ಪೂರ್ಣಾಙ್ಕ on)
{
    ಪೂರ್ಣಾಙ್ಕ	    opt_idx;

    ಯದಿ (p_cp != on)
    {
	p_cp = on;
	compatible_set();
    }
    opt_idx = findoption((char_u *)"cp");
    ಯದಿ (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_SET;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    ಪೂರ್ಣಾಙ್ಕ
option_was_set(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ idx;

    idx = findoption(name);
    ಯದಿ (idx < 0)	// unknown option
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (options[idx].flags & P_WAS_SET)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    ಪೂರ್ಣಾಙ್ಕ
reset_option_was_set(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ idx = findoption(name);

    ಯದಿ (idx < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    options[idx].flags &= ~P_WAS_SET;
    ಪ್ರತ್ಯಾಗಮ OK;
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
compatible_set(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    opt_idx;

    ಪ್ರತಿ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	ಯದಿ (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    set_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    ಪೂರ್ಣಾಙ್ಕ
can_bs(
    ಪೂರ್ಣಾಙ್ಕ		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ifdef FEAT_JOB_CHANNEL
    ಯದಿ (what == BS_START && bt_prompt(curbuf))
	ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
    ವಿಕಲ್ಪ (*p_bs)
    {
	ಪಕ್ಷ '3':       ಪ್ರತ್ಯಾಗಮ TRUE;
	ಪಕ್ಷ '2':	ಪ್ರತ್ಯಾಗಮ (what != BS_NOSTOP);
	ಪಕ್ಷ '1':	ಪ್ರತ್ಯಾಗಮ (what != BS_START);
	ಪಕ್ಷ '0':	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ vim_strchr(p_bs, what) != ರಿಕ್ತ;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloff_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloffpad_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_sidescrolloff_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    unsigned ಪೂರ್ಣಾಙ್ಕ
get_bkc_flags(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#ಯದಿ defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_ಮೌಲ್ಯ(buf_T *buf)
{
    ಯದಿ (buf->b_p_flp == ರಿಕ್ತ || *buf->b_p_flp == NUL)
	ಪ್ರತ್ಯಾಗಮ p_flp;
    ಪ್ರತ್ಯಾಗಮ buf->b_p_flp;
}
#endif

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    unsigned ಪೂರ್ಣಾಙ್ಕ
get_ve_flags(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#ಯದಿ defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_ಮೌಲ್ಯ(win_T *win)
{
    ಯದಿ (win->w_p_sbr == ರಿಕ್ತ || *win->w_p_sbr == NUL)
	ಪ್ರತ್ಯಾಗಮ p_sbr;
    ಯದಿ (STRCMP(win->w_p_sbr, "NONE") == 0)
	ಪ್ರತ್ಯಾಗಮ empty_option;
    ಪ್ರತ್ಯಾಗಮ win->w_p_sbr;
}
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(ಪೂರ್ಣಾಙ್ಕ bufopt)
{
    dict_T	*d;
    ಪೂರ್ಣಾಙ್ಕ		opt_idx;

    d = dict_alloc();
    ಯದಿ (d == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	ಸಂರಚನಾ vimoption *opt = &options[opt_idx];

	ಯದಿ ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    ಯದಿ (varp != ರಿಕ್ತ)
	    {
		ಯದಿ (opt->flags & P_STRING)
		    dict_add_string(d, opt->fullname, *(char_u **)varp);
		ಅನ್ಯಥಾ ಯದಿ (opt->flags & P_ಅಂಕೆ)
		    dict_add_ಸಂಖ್ಯೆ(d, opt->fullname, *(long *)varp);
		ಅನ್ಯಥಾ
		    dict_add_ಸಂಖ್ಯೆ(d, opt->fullname, *(ಪೂರ್ಣಾಙ್ಕ *)varp);
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ d;
}
#endif

#ಯದಿ defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    ಪೂರ್ಣಾಙ್ಕ
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    ಯದಿ (val == ರಿಕ್ತ)
	p = wp->w_p_culopt;
    ಅನ್ಯಥಾ
	p = val;
    ಯಾವತ್ (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	ಯದಿ (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_ಸಾಲು;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_ಸಾಲು | CULOPT_NBR;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	ಯದಿ (*p != ',' && *p != NUL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    ಯದಿ ((culopt_flags_new & CULOPT_ಸಾಲು) && (culopt_flags_new & CULOPT_SCRLINE))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    ಪೂರ್ಣಾಙ್ಕ
magic_isset(ಶೂನ್ಯ)
{
    ವಿಕಲ್ಪ (magic_overruled)
    {
	ಪಕ್ಷ OPTION_MAGIC_ON:      ಪ್ರತ್ಯಾಗಮ TRUE;
	ಪಕ್ಷ OPTION_MAGIC_OFF:     ಪ್ರತ್ಯಾಗಮ FALSE;
	ಪಕ್ಷ OPTION_MAGIC_NOT_SET: ಭಙ್ಗ;
    }
#ifdef FEAT_EVAL
    ಯದಿ (in_vim9script())
	ಪ್ರತ್ಯಾಗಮ TRUE;
#endif
    ಪ್ರತ್ಯಾಗಮ p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
option_set_callback_func(char_u *optval UNUSED, callback_T *optcb UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    ಪೂರ್ಣಾಙ್ಕ		funcname = FALSE;

    ಯದಿ (optval == ರಿಕ್ತ || *optval == NUL)
    {
	free_callback(optcb);
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಯದಿ (*optval == '{' || (in_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_ಅಭಿವ್ಯಕ್ತಿ(optval, ರಿಕ್ತ);
    ಅನ್ಯಥಾ
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    ಯದಿ (tv == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    cb = get_callback(tv);
    ಯದಿ (cb.cb_name == ರಿಕ್ತ || *cb.cb_name == NUL)
    {
	free_tv(tv);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    free_callback(optcb);
    set_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = ರಿಕ್ತ;
    ಯದಿ (in_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != ರಿಕ್ತ))
    {
	ಯದಿ (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = find_imported(optval, dot - optval, FALSE);
	ಯದಿ (import != ರಿಕ್ತ && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((ವರ್ಣ *)fnamebuf, ಪರಿಮಾಣ(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	ಅನ್ಯಥಾ
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

    ಪ್ರತ್ಯಾಗಮ OK;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ FAIL;
#endif
}

#ಯದಿ defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    ವರ್ಣ *
did_set_showtabpanel(optset_T *args UNUSED)
{
    shell_new_columns();
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_EVAL)
    ಸ್ಥಿರ ಶೂನ್ಯ
didset_options_sctx(ಪೂರ್ಣಾಙ್ಕ opt_flags, ವರ್ಣ **buf)
{
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; ; ++i)
	{
	    ಯದಿ (buf[i] == ರಿಕ್ತ)
		ಭಙ್ಗ;

	    ಪೂರ್ಣಾಙ್ಕ idx = findoption((char_u *)buf[i]);
	    ಯದಿ (idx >= 0)
		set_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#endif
