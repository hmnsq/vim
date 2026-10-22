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
#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "optiondefs.h"

ସ୍ଥିର ଶୂନ୍ୟ set_options_default(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags);
ସ୍ଥିର ଶୂନ୍ୟ set_string_default_esc(ଅକ୍ଷର *name, char_u *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା escape);
ସ୍ଥିର char_u *find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
ସ୍ଥିର char_u *option_expand(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx, char_u *val);
ସ୍ଥିର ଶୂନ୍ୟ didset_options(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ didset_options2(ଶୂନ୍ୟ);
#ଯଦି defined(FEAT_EVAL)
ସ୍ଥିର long_u *insecure_flag(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags);
#ଅନ୍ୟଥା
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#endif
ସ୍ଥିର ଅକ୍ଷର *set_bool_option(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx, char_u *varp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags);
ସ୍ଥିର ଅକ୍ଷର *set_ଅଙ୍କ_option(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx, char_u *varp, long ମୂଲ୍ୟ, ଅକ୍ଷର *errbuf, size_t errbuflen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା find_ଚାବି_option(char_u *arg_arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_lt);
ସ୍ଥିର ଶୂନ୍ୟ showoptions(ପୂର୍ଣ୍ଣସଂଖ୍ୟା all, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା optval_default(ସଂରଚନା vimoption *, char_u *varp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା compatible);
ସ୍ଥିର ଶୂନ୍ୟ showoneopt(ସଂରଚନା vimoption *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା put_setstring(FILE *fd, bool legacy, ଅକ୍ଷର *cmd, ଅକ୍ଷର *name, char_u **valuep, long_u flags);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା put_setnum(FILE *fd, ଅକ୍ଷର *cmd, ଅକ୍ଷର *name, long *valuep);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା put_setbool(FILE *fd, ଅକ୍ଷର *cmd, ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା istermoption(ସଂରଚନା vimoption *p);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା istermoption_idx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx);
ସ୍ଥିର char_u *get_varp_scope(ସଂରଚନା vimoption *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା scope);
ସ୍ଥିର char_u *get_varp(ସଂରଚନା vimoption *);
ସ୍ଥିର ଶୂନ୍ୟ check_win_options(win_T *win);
ସ୍ଥିର ଶୂନ୍ୟ option_value2string(ସଂରଚନା vimoption *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା scope);
ସ୍ଥିର ଶୂନ୍ୟ check_winopt(winopt_T *wop);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା wc_use_keyname(char_u *varp, long *wcp);
ସ୍ଥିର ଶୂନ୍ୟ compatible_set(ଶୂନ୍ୟ);

#ଯଦି defined(FEAT_EVAL)
ସ୍ଥିର ଅକ୍ଷର *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    ଖାଲି};
ସ୍ଥିର ଶୂନ୍ୟ didset_options_sctx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags, ଅକ୍ଷର **buf);
#endif


/*
 * Initialize the 'shell' option to a default value.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_init_default_shell(ଶୂନ୍ୟ)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    ଯଦି (((p = mch_getenv((char_u *)"SHELL")) != ଖାଲି && *p != NUL)
#ଯଦି defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != ଖାଲି && *p != NUL)
	    || ((p = (char_u *)default_shell()) != ଖାଲି && *p != NUL)
#endif
	    )
#ଯଦି defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	ଯଦି (vim_strchr(p, ' ') != ଖାଲି)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    ଯଦି (cmd != ଖାଲି)
	    {
		vim_snprintf((ଅକ୍ଷର *)cmd, len, "\"%s\"", p);
		set_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	ଅନ୍ୟଥା
	    set_string_default("sh", p);
    }
#ଅନ୍ୟଥା
	set_string_default_esc("sh", p, TRUE);
#endif
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_init_default_backupskip(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		plen;
#ifdef UNIX
    ସ୍ଥିର ଅକ୍ଷର	*(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#ଅନ୍ୟଥା
    ସ୍ଥିର ଅକ୍ଷର	*(names[3]) = {"TMPDIR", "TEMP", "TMP"};
#endif
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(names); ++i)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mustfree = FALSE;
#ifdef UNIX
	ଯଦି (*names[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL("/private/tmp");
# ଅନ୍ୟଥା
	    p = (char_u *)"/tmp";
	    plen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL("/tmp");
# endif
	}
	ଅନ୍ୟଥା
#endif
	{
	    p = vim_getenv((char_u *)names[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	ଯଦି (p != ଖାଲି && *p != NUL)
	{
	    char_u  *item;
	    size_t  itemsize;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_trailing_path_sep = FALSE;

	    ଯଦି (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
		// does the value include a trailing path separator?
		ଯଦି (after_pathsep(p, p + plen))
		    has_trailing_path_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_path_sep ? 0 : 1) + 2;
	    item = alloc(itemsize);
	    ଯଦି (item != ଖାଲି)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((ଅକ୍ଷର *)item, itemsize, "%s%s*", p, (has_trailing_path_sep) ? "" : PATHSEPSTR);

		ଯଦି (find_dup_item(ga.ga_data, item, itemlen, options[opt_idx].flags) == ଖାଲି
			&& ga_grow(&ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((ଅକ୍ଷର *)ga.ga_data + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", item);
		}
		vim_free(item);
	    }
	}
	ଯଦି (mustfree)
	    vim_free(p);
    }
    ଯଦି (ga.ga_data != ଖାଲି)
    {
	set_string_default("bsk", ga.ga_data);
	vim_free(ga.ga_data);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_init_default_maxmemtot(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    ଯଦି (opt_idx < 0)
	ଫେରନ୍ତୁ;

#ଯଦି !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
    ଯଦି (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#endif
    {
#ଯଦି defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#ନଚେତଯଦି defined(HAVE_TOTAL_MEM)
	// Use amount of memory available to Vim.
	n = (mch_total_mem(FALSE) >> 1);
#ଅନ୍ୟଥା
	n = (0x7fffffff >> 11);
#endif
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	ଯଦି (opt_idx >= 0)
	{
#ଯଦି !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
	    ଯଦି ((long)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (long)n
		    || (long)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#endif
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_init_default_cdpath(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    ଯଦି (cdpath == ଖାଲି)
	ଫେରନ୍ତୁ;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    ଯଦି (buf != ଖାଲି)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	ପାଇଁ (i = 0; cdpath[i] != NUL; ++i)
	{
	    ଯଦି (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    ଅନ୍ୟଥା
	    {
		ଯଦି (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	ଯଦି (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	ଅନ୍ୟଥା
	    vim_free(buf); // cannot happen
    }
    ଯଦି (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_init_default_printencoding(ଶୂନ୍ୟ)
{
#ଯଦି defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    set_string_default("penc",
# ଯଦି defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# ଅନ୍ୟଥା // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#endif
}

#ଯଦି defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_init_default_printexpr(ଶୂନ୍ୟ)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    set_string_default("pexpr",
# ଯଦି defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# ଅନ୍ୟଥା
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#endif

#ifdef UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_init_restricted_mode(ଶୂନ୍ୟ)
{
    char_u	*p;

    p = get_isolated_shell_name();
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#endif

#ifdef CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_init_clean_rtp(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    ଯଦି (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    ଯଦି (opt_idx < 0)
	ଫେରନ୍ତୁ;
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
    ସ୍ଥିର ଶୂନ୍ୟ
set_init_xdg_rtp(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_xdg_env = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = ଖାଲି;
    char_u	*vimrc2 = ଖାଲି;
    char_u	*xdg_dir = ଖାଲି;
    char_u	*xdg_rtp = ଖାଲି;
    string_T	vimrc_xdg = {ଖାଲି, 0};

    // initialize chartab, so we can expand $HOME
    (ଶୂନ୍ୟ)init_chartab();
    vimrc1 = expand_env_save((char_u *)USR_VIMRC_FILE);
    vimrc2 = expand_env_save((char_u *)USR_VIMRC_FILE2);

    xdg_dir = mch_getenv("XDG_CONFIG_HOME");
    ଯଦି (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    ଯଦି (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	ଯାଅ theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    ଯଦି ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	ଯାଅ theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    ଯଦି ((opt_idx = findoption((char_u *)"packpath")) < 0)
	ଯାଅ theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# ଯଦି defined(XDG_VDIR) && defined(FEAT_SESSION)
    ଯଦି ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	ଯାଅ theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    ଯଦି (should_free_xdg_dir)
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
    ସ୍ଥିର ଶୂନ୍ୟ
set_init_expand_env(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;
    char_u	*p;

    ପାଇଁ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	ଯଦି ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != ଖାଲି)
	    p = (char_u *)_(*(ଅକ୍ଷର **)options[opt_idx].var);
	ଅନ୍ୟଥା
	    p = option_expand(opt_idx, ଖାଲି);
	ଯଦି (p != ଖାଲି && (p = vim_strsave(p)) != ଖାଲି)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    ଯଦି (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#ଯଦି defined(MSWIN) && defined(FEAT_GETTEXT)
/*
 * Get the display language of Windows and the languages to fall back on, as a
 * colon separated list for gettext, e.g. "ja_JP:en_US".  The list stops after
 * English, untranslated messages are English already.
 * Returns NULL when it cannot be obtained.  The result must be freed.
 */
    ସ୍ଥିର char_u *
get_ui_langs(ଶୂନ୍ୟ)
{
    ULONG	ଅଙ୍କ_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*buffer;
    char_u	*langs = ଖାଲି;

    ଯଦି (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &ଅଙ୍କ_languages, ଖାଲି,
								    &bufsize)
	    || bufsize == 0)
	ଫେରନ୍ତୁ ଖାଲି;

    buffer = ALLOC_MULT(WCHAR, bufsize);
    ଯଦି (buffer == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &ଅଙ୍କ_languages, buffer,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	ଯଦି (langs != ଖାଲି)
	{
	    char_u	*d = langs;
	    WCHAR	*s = buffer;

	    ଯେତେବେଳେ (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		ଯଦି (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		ପାଇଁ ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		ଯଦି (english)
		    ଭାଙ୍ଗ;
	    }
	    *d = NUL;
	}
    }
    vim_free(buffer);

    ଫେରନ୍ତୁ langs;
}
#endif

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_init_lang_env(ଶୂନ୍ୟ)
{
#ଯଦି defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    ଯଦି (mch_getenv((char_u *)"LANG") == ଖାଲି
	    && mch_getenv((char_u *)"LANGUAGE") == ଖାଲି
	    && mch_getenv((char_u *)"LC_ALL") == ଖାଲି
	    && mch_getenv((char_u *)"LC_MESSAGES") == ଖାଲି)
    {
	char_u	*langs = get_ui_langs();

	ଯଦି (langs != ଖାଲି && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    ଯଦି (colon != ଖାଲି)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#ନଚେତଯଦି defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#endif
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_init_default_encoding(ଶୂନ୍ୟ)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;

#ଯଦି defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#ଅନ୍ୟଥା
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#endif
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    ଯଦି (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    ଯଦି (mb_init() == ଖାଲି)
    {
	opt_idx = findoption((char_u *)"encoding");
	ଯଦି (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#ଯଦି defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	ଯଦି (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    set_string_option_direct((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    set_string_option_direct((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    ଯଦି (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    ଯଦି (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (ଶୂନ୍ୟ)init_chartab();
	}
#endif

#ଯଦି defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	ଯଦି (
# ifdef VIMDLL
		(!gui.in_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    ଅକ୍ଷର	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    ଯଦି (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, ଆକାର(buf), "cp%ld", (long)GetACP());
	    ଅନ୍ୟଥା
		buflen = vim_snprintf(buf, ଆକାର(buf), "cp%ld", (long)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    ଯଦି (p_tenc != ଖାଲି)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		ଯଦି (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_setup(&ନିବେଶ_conv, p_tenc, p_enc);
		convert_setup(&output_conv, p_enc, p_tenc);
	    }
	    ଅନ୍ୟଥା
		p_tenc = empty_option;
	}
#endif
#ଯଦି defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#endif
    }
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
set_init_1(ପୂର୍ଣ୍ଣସଂଖ୍ୟା clean_arg)
{
#ifdef FEAT_LANGMAP
    langmap_init();
#endif

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    ଯଦି (mch_getenv((char_u *)"VIM_POSIX") != ଖାଲି)
    {
	set_string_default("cpo", (char_u *)CPO_ALL);
	set_string_default("shm", (char_u *)SHM_POSIX);
    }

    set_init_default_shell();
    set_init_default_backupskip();
    set_init_default_maxmemtot();
    set_init_default_cdpath();
    set_init_default_printencoding();
#ଯଦି defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
    ଯଦି (clean_arg)
	set_init_clean_rtp();
#endif

#ifdef FEAT_GUI
    ଯଦି (found_reverse_arg)
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
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

#ଯଦି defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    ଯଦି (mch_getenv((char_u *)"MLTERM") != ଖାଲି)
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"tbidi", 1L, ଖାଲି, 0);
#endif

    didset_options2();

    set_init_lang_env();

#ifdef FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    set_helplang_default(get_mess_lang());
#endif
}

ସ୍ଥିର char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    ଶୂନ୍ୟ
set_fencs_unicode(ଶୂନ୍ୟ)
{
    set_string_option_direct((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_option_default(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    ଯଦି (varp != ଖାଲି)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	ଯଦି (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    ଯଦି (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		set_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    ଅନ୍ୟଥା ଯଦି (options[opt_idx].indir != PV_NONE)
		set_string_option_direct(ଖାଲି, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    ଅନ୍ୟଥା
	    {
		ଯଦି ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (flags & P_ଅଙ୍କ)
	{
	    ଯଦି (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    ଅନ୍ୟଥା
	    {
		long def_val = (long)(long_i)options[opt_idx].def_val[dvi];

		ଯଦି ((long *)varp == &curwin->w_p_so
			|| (long *)varp == &curwin->w_p_siso
			|| (long *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(long *)varp = -1;
		ଅନ୍ୟଥା
		    *(long *)varp = def_val;
		// May also set global value for local option.
		ଯଦି (both)
		    *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	ଅନ୍ୟଥା	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(long)(long_i)options[opt_idx].def_val[dvi];
#ifdef UNIX
	    // 'modeline' defaults to off for root
	    ଯଦି (options[opt_idx].indir == PV_ML && getuid() == ମୂଳ_UID)
		*(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp = FALSE;
#endif
	    // May also set global value for local option.
	    ଯଦି (both)
		*(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	ଯଦି (both)
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
    ସ୍ଥିର ଶୂନ୍ୟ
set_options_default(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    win_T	*wp;
    tabpage_T	*tp;

    ପାଇଁ (i = 0; !istermoption_idx(i); i++)
	ଯଦି (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#ଯଦି defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_ଚାବି
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
    ସ୍ଥିର ଶୂନ୍ୟ
set_string_default_esc(ଅକ୍ଷର *name, char_u *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା escape)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;

    ଯଦି (escape && vim_strchr(val, ' ') != ଖାଲି)
	p = vim_strsave_escaped(val, (char_u *)" ");
    ଅନ୍ୟଥା
	p = vim_strsave(val);
    ଯଦି (p == ଖାଲି)		// we don't want a NULL
	ଫେରନ୍ତୁ;

    opt_idx = findoption((char_u *)name);
    ଯଦି (opt_idx < 0)
    {
	vim_free(p);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    ଶୂନ୍ୟ
set_string_default(ଅକ୍ଷର *name, char_u *val)
{
    set_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    ସ୍ଥିର char_u *
find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    bs = 0;
    char_u  *s;

    ଯଦି (origval == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (s = origval; *s != NUL; ++s)
    {
	ଯଦି ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    ଫେରନ୍ତୁ s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	ଯଦି ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	ଅନ୍ୟଥା
	    bs = 0;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    ଶୂନ୍ୟ
set_ସଂଖ୍ୟା_default(ଅକ୍ଷର *name, long val)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;

    opt_idx = findoption((char_u *)name);
    ଯଦି (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#ଯଦି defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    ଶୂନ୍ୟ
set_local_options_default(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_buffer)
{
    win_T	*save_curwin = curwin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    curwin = wp;
    curbuf = curwin->w_buffer;
    block_autocmds();

    ପାଇଁ (i = 0; !istermoption_idx(i); i++)
    {
	ସଂରଚନା vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	ଯଦି (p->indir != PV_NONE
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

#ଯଦି defined(EXITFREE)
/*
 * Free all options.
 */
    ଶୂନ୍ୟ
free_all_options(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; !istermoption_idx(i); i++)
    {
	ଯଦି (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    ଯଦି ((options[i].flags & P_ALLOCED) && options[i].var != ଖାଲି)
		free_string_option(*(char_u **)options[i].var);
	    ଯଦି (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	ଅନ୍ୟଥା ଯଦି (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# ଯଦି defined(FEAT_EVAL)
    free_findfunc_option();
# endif
}
#endif


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    ଶୂନ୍ୟ
set_init_2(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    ଯଦି (idx >= 0 && !(options[idx].flags & P_WAS_SET))
	set_option_default(idx, OPT_LOCAL, p_cp);
    comp_col();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    ଯଦି (!option_was_set((char_u *)"window"))
	p_window = Rows - 1;
    set_ସଂଖ୍ୟା_default("window", Rows - 1);

    // For DOS console the default is always black.
#ଯଦି !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    ଯଦି (idx >= 0 && !(options[idx].flags & P_WAS_SET)
						 && *term_bg_default() == 'd')
    {
	set_string_option_direct(ଖାଲି, idx, (char_u *)"dark", OPT_FREE, 0);
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
    (ଶୂନ୍ୟ)parse_printoptions(ଖାଲି);	// parse 'printoptions' default value
#endif
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    ଶୂନ୍ୟ
set_init_3(ଶୂନ୍ୟ)
{
#ଯଦି defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx_srr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    do_srr;
# ifdef FEAT_QUICKFIX
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx_sp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    ଯଦି (idx_srr < 0)
	do_srr = FALSE;
    ଅନ୍ୟଥା
	do_srr = !(options[idx_srr].flags & P_WAS_SET);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    ଯଦି (idx_sp < 0)
	do_sp = FALSE;
    ଅନ୍ୟଥା
	do_sp = !(options[idx_sp].flags & P_WAS_SET);
# endif
    p = get_isolated_shell_name();
    ଯଦି (p != ଖାଲି)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	ଯଦି (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# ଯଦି defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# ଯଦି defined(FEAT_QUICKFIX)
	    ଯଦି (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  ଅନ୍ୟଥା
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    ଯଦି (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	ଅନ୍ୟଥା ଯଦି (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  ଯଦି defined(FEAT_QUICKFIX)
		ଯଦି (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		ଯଦି (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	ଅନ୍ୟଥା
	    // Always use POSIX shell style redirection if we reach this
	    ଯଦି (       fnamecmp(p, "sh") == 0
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
# ଯଦି defined(FEAT_QUICKFIX)
		ଯଦି (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  ଅନ୍ୟଥା
		    ଯଦି (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    ଅନ୍ୟଥା
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		ଯଦି (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#endif

#ଯଦି defined(MSWIN)
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
    ଯଦି (strstr((ଅକ୍ଷର *)gettail(p_sh), "powershell") != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	ଯଦି (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	ଯଦି (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    ଅନ୍ୟଥା ଯଦି (strstr((ଅକ୍ଷର *)gettail(p_sh), "sh") != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx3;

	idx3 = findoption((char_u *)"shcf");
	ଯଦି (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	ଯଦି (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    ଅନ୍ୟଥା ଯଦି (strstr((ଅକ୍ଷର *)gettail(p_sh), "cmd.exe") != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx3;

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
	ଯଦି (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	ଯଦି (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#endif

    ଯଦି (BUFEMPTY())
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	ଯଦି (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_SET))
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    set_title_defaults();
}

#ଯଦି defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    ଶୂନ୍ୟ
set_helplang_default(char_u *lang)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    size_t	langlen;

    ଯଦି (lang == ଖାଲି)	// safety check
	ଫେରନ୍ତୁ;

    langlen = STRLEN(lang);
    ଯଦି (langlen < 2)	// safety check
	ଫେରନ୍ତୁ;

    idx = findoption((char_u *)"hlg");
    ଯଦି (idx < 0 || (options[idx].flags & P_WAS_SET))
	ଫେରନ୍ତୁ;

    ଯଦି (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    ଯଦି (p_hlg == ଖାଲି)
	p_hlg = empty_option;
    ଅନ୍ୟଥା
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	ଯଦି (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	ଅନ୍ୟଥା ଯଦି (langlen >= 1 && *p_hlg == 'C')
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
    ଶୂନ୍ୟ
set_title_defaults(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx1;
    long    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    ଯଦି (idx1 >= 0 && !(options[idx1].flags & P_WAS_SET))
    {
#ifdef FEAT_GUI
	ଯଦି (gui.starting || gui.in_use)
	    val = TRUE;
	ଅନ୍ୟଥା
#endif
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    ଯଦି (idx1 < 0 || (options[idx1].flags & P_WAS_SET))
    {
	ଫେରନ୍ତୁ;
    }

#ifdef FEAT_GUI
    ଯଦି (gui.starting || gui.in_use)
	val = TRUE;
    ଅନ୍ୟଥା
#endif
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    ଶୂନ୍ୟ
ex_set(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;

    ଯଦି (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#ଯଦି defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    ଯଦି ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    ଅନ୍ୟଥା
#endif
    {
	ଯଦି (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(ଶୂନ୍ୟ)do_set(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    PREFIX_NO = 0,	// "no" prefix
    PREFIX_NONE,	// no prefix
    PREFIX_INV,		// "inv" prefix
} set_prefix_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    ସ୍ଥିର set_prefix_T
get_option_prefix(char_u **argp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prefix = PREFIX_NONE;
    char_u	*arg = *argp;

    ଯଦି (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	prefix = PREFIX_NO;
	arg += 2;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "inv", 3) == 0)
    {
	prefix = PREFIX_INV;
	arg += 3;
    }

    *argp = arg;
    ଫେରନ୍ତୁ prefix;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_option_name(char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *opt_idxp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *keyp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଚାବି = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	opt_idx;

    ଯଦି (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	ଯଦି (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	ଅନ୍ୟଥା
	{
	    len = 1;
	    ଯେତେବେଳେ (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	ଯଦି (arg[len] != '>')
	    ଫେରନ୍ତୁ FAIL;

	arg[len] = NUL;			    // put NUL after name
	ଯଦି (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	ଯଦି (opt_idx == -1)
	    ଚାବି = find_ଚାବି_option(arg + 1, TRUE);
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	ଯଦି (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	ଅନ୍ୟଥା
	    ଯେତେବେଳେ (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	ଯଦି (opt_idx == -1)
	    ଚାବି = find_ଚାବି_option(arg, FALSE);
    }

    *keyp = ଚାବି;
    *lenp = len;
    *opt_idxp = opt_idx;

    ଫେରନ୍ତୁ OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    ସ୍ଥିର set_op_T
get_opt_op(char_u *arg)
{
    set_op_T op = OP_NONE;

    ଯଦି (*arg != NUL && *(arg + 1) == '=')
    {
	ଯଦି (*arg == '+')
	    op = OP_ADDING;		// "+="
	ଅନ୍ୟଥା ଯଦି (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	ଅନ୍ୟଥା ଯଦି (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    ଫେରନ୍ତୁ op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
ସ୍ଥିର ଅକ୍ଷର *modeline_whitelist[] =
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
    ଖାଲି
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    ସ୍ଥିର bool
is_modeline_whitelisted(ଅକ୍ଷର *name)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; modeline_whitelist[i] != ଖାଲି; i++)
	ଯଦି (STRCMP(name, modeline_whitelist[i]) == 0)
	    ଫେରନ୍ତୁ true;
    ଫେରନ୍ତୁ false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
validate_opt_idx(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags,
	long_u flags,
	ଅକ୍ଷର **errmsg,
	set_prefix_T prefix)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    ଯଦି ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	ଫେରନ୍ତୁ FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    ଯଦି ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	ଫେରନ୍ତୁ FAIL;

    // Disallow changing some options from modelines.
    ଯଦି (opt_flags & OPT_MODELINE)
    {
	ଯଦି (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_in_modeline;
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_in_modeline_when_modelineexpr_is_off;
	    ଫେରନ୍ତୁ FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	ଯଦି (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    ଯଦି (options[opt_idx].indir == PV_ML && prefix == PREFIX_NO)
		ଫେରନ୍ତୁ OK;
	    ଅନ୍ୟଥା ଯଦି (!is_modeline_whitelisted(options[opt_idx].fullname))
		ଫେରନ୍ତୁ FAIL;
	}
#ifdef FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	ଯଦି (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    ଫେରନ୍ତୁ FAIL;
#endif
    }

#ifdef HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    ଯଦି (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_in_sandbox;
	ଫେରନ୍ତୁ FAIL;
    }
#endif

    ଫେରନ୍ତୁ OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    ସ୍ଥିର char_u *
stropt_get_default_val(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,
    char_u	*varp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    ଯଦି ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ifdef FEAT_GUI
	ଯଦି (gui.in_use)
	    newval = gui_bg_default();
	ଅନ୍ୟଥା
#endif
	    newval = term_bg_default();
    }
    ଅନ୍ୟଥା ଯଦି ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    ଯଦି (newval == ଖାଲି)
	newval = empty_option;
    ଅନ୍ୟଥା
    {
	char_u *s = option_expand(opt_idx, newval);
	ଯଦି (s == ଖାଲି)
	    s = newval;
	newval = vim_strsave(s);
    }

    ଫେରନ୍ତୁ newval;
}

/*
 * Convert the 'backspace' option number value to a string: for adding,
 * prepending and removing string.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
opt_backspace_nr2str(
    char_u	*varp,
    char_u	**origval_p,
    char_u	**origval_l_p,
    char_u	**origval_g_p,
    char_u	**oldval_p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = getdigits((char_u **)varp);

    ବାଛ (i)
    {
	ସ୍ଥିତି 0:
	    *(char_u **)varp = empty_option;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    ଭାଙ୍ଗ;
    }
    vim_free(*oldval_p);
    ଯଦି (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    ଯଦି (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    ଯଦି (*origval_g_p == *oldval_p)
	*origval_g_p = *(char_u **)varp;
    *oldval_p = *(char_u **)varp;
}

/*
 * Convert the 'whichwrap' option number value to a string, for backwards
 * compatibility with Vim 3.0.
 * Note: 'argp' is a pointer to a char_u pointer and is updated.
 */
    ସ୍ଥିର char_u *
opt_whichwrap_nr2str(char_u **argp, char_u *whichwrap)
{
    size_t  len = 0;

    *whichwrap = NUL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = getdigits(argp);
    ଯଦି (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    ଯଦି (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    ଯଦି (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    ଯଦି (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    ଯଦି (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    ଯଦି (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    ଫେରନ୍ତୁ whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    ସ୍ଥିର char_u *
stropt_copy_ମୂଲ୍ୟ(
    char_u	*origval,
    char_u	**argp,
    set_op_T	op,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags UNUSED)
{
    char_u	*arg = *argp;
    unsigned	newlen;
    char_u	*newval;
    char_u	*s = ଖାଲି;

    // get a bit too much
    newlen = (unsigned)STRLEN(arg) + 1;
    ଯଦି (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;
    newval = alloc(newlen);
    ଯଦି (newval == ଖାଲି)  // out of mem, don't change
	ଫେରନ୍ତୁ ଖାଲି;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    ଯେତେବେଳେ (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ଯଦି (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#endif
	   )
	    ++arg;	// remove backslash
	ଯଦି (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	ଅନ୍ୟଥା
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    ଫେରନ୍ତୁ newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    ସ୍ଥିର char_u *
stropt_expand_envvar(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,
    char_u	*origval,
    char_u	*newval,
    set_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    ଯଦି (s == ଖାଲି)
	ଫେରନ୍ତୁ newval;

    vim_free(newval);
    unsigned newlen = (unsigned)STRLEN(s) + 1;
    ଯଦି (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;

    newval = alloc(newlen);
    ଯଦି (newval == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    STRCPY(newval, s);

    ଫେରନ୍ତୁ newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    ଯଦି (op == OP_ADDING)
    {
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(origval);
	// strip a trailing comma, would get 2
	ଯଦି (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    ଅନ୍ୟଥା
    {
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    ଯଦି (comma)
	newval[len] = ',';
}

/*
 * Remove a value from a string option.  Copy string option value in "origval"
 * to "newval" and then remove the string "strval" of length "len".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
stropt_remove_val(
    char_u	*origval,
    char_u	*newval,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    char_u	*strval,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    ଯଦି (*strval)
    {
	// may need to remove a comma
	ଯଦି (flags & P_COMMA)
	{
	    ଯଦି (strval == origval)
	    {
		// include comma after string
		ଯଦି (strval[len] == ',')
		    ++len;
	    }
	    ଅନ୍ୟଥା
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
    ସ୍ଥିର char_u *
find_ଚାବି_item(char_u *src, char_u *ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା keylen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *itemlenp)
{
    char_u  *p = src;

    ଯେତେବେଳେ (*p != NUL)
    {
	// Check if this item starts with the same key
	ଯଦି ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, ଚାବି, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *end = vim_strchr(p, ',');
	    ଯଦି (end == ଖାଲି)
		end = p + STRLEN(p);
	    *itemlenp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - p);
	    ଫେରନ୍ତୁ p;
	}
	++p;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
remove_comma_item(char_u *str, char_u *item, ପୂର୍ଣ୍ଣସଂଖ୍ୟା itemlen)
{
    ଯଦି (item[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(item, item + itemlen + 1);
    ଅନ୍ୟଥା ଯଦି (item > str && *(item - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(item - 1, item + itemlen);
    ଅନ୍ୟଥା
	// Only item
	*item = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
remove_ଚାବି_item(char_u *str, char_u *ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା keylen, char_u *skip)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    itemlen;
    char_u  *found;

    ଯେତେବେଳେ ((found = find_ଚାବି_item(str, ଚାବି, keylen, &itemlen)) != ଖାଲି)
    {
	ଯଦି (found == skip)
	{
	    // Search for the next match after this one.
	    char_u *next = found + itemlen;
	    ଯଦି (*next == ',')
		++next;
	    found = find_ଚାବି_item(next, ଚାବି, keylen, &itemlen);
	    ଯଦି (found == ଖାଲି)
		ଭାଙ୍ଗ;
	}

	remove_comma_item(str, found, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
append_item(char_u *str, char_u *item, ପୂର୍ଣ୍ଣସଂଖ୍ୟା item_len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str);

    ଯଦି (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, item, (size_t)item_len);
    str[len + item_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prepend_item(char_u *str, char_u *item, ପୂର୍ଣ୍ଣସଂଖ୍ୟା item_len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା comma = (len > 0) ? 1 : 0;

    mch_memmove(str + item_len + comma, str, (size_t)len + 1);
    mch_memmove(str, item, (size_t)item_len);
    ଯଦି (comma)
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
    ସ୍ଥିର bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags UNUSED)
{
    char_u  *p;
    char_u  *item_start;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    ଯଦି (vim_strchr(newval, ':') == ଖାଲି && vim_strchr(newval, ',') == ଖାଲି)
	ଫେରନ୍ତୁ false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    ଯଦି (newval_copy == ଖାଲି)
	ଫେରନ୍ତୁ false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    item_start = newval_copy;
    ପାଇଁ (;;)
    {
	p = vim_strchr(item_start, ',');
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା item_len = (p == ଖାଲି)
			    ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(item_start) : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - item_start);

	ଯଦି (item_len > 0)
	{
	    char_u *colon = vim_strchr(item_start, ':');
	    ଯଦି (colon != ଖାଲି && colon < item_start + item_len)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା keylen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(colon - item_start) + 1;

		ଯଦି (op == OP_ADDING || op == OP_PREPENDING)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_itemlen;
		    char_u *found = find_ଚାବି_item(newval, item_start,
						       keylen, &old_itemlen);
		    ଯଦି (found != ଖାଲି)
		    {
			ଯଦି (old_itemlen == item_len
				&& STRNCMP(found, item_start,
							 item_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_ଚାବି_item(newval, item_start,
							   keylen, found);
			}
			ଅନ୍ୟଥା
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_ଚାବି_item(newval, item_start,
							    keylen, ଖାଲି);
			    ଯଦି (op == OP_PREPENDING)
				prepend_item(newval, item_start, item_len);
			    ଅନ୍ୟଥା
				append_item(newval, item_start, item_len);
			}
		    }
		    ଅନ୍ୟଥା
		    {
			// New item.
			ଯଦି (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			ଅନ୍ୟଥା
			    append_item(newval, item_start, item_len);
		    }
		}
		ଅନ୍ୟଥା ଯଦି (op == OP_REMOVING)
		    remove_ଚାବି_item(newval, item_start, keylen, ଖାଲି);
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    ଯଦି (found == ଖାଲି)
		    {
			// New item.
			ଯଦି (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			ଅନ୍ୟଥା
			    append_item(newval, item_start, item_len);
		    }
		    // else: exact duplicate — do nothing
		}
		ଅନ୍ୟଥା ଯଦି (op == OP_REMOVING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    ଯଦି (found != ଖାଲି)
			remove_comma_item(newval, found, item_len);
		}
	    }
	}

	ଯଦି (p == ଖାଲି)
	    ଭାଙ୍ଗ;
	item_start = p + 1;
    }

    vim_free(newval_copy);

    ଫେରନ୍ତୁ true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
stropt_remove_dupflags(char_u *newval, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    ଯେତେବେଳେ (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	ଯଦି (flags & P_ONECOMMA)
	{
	    ଯଦି (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != ଖାଲି)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		ଜାରି;
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != ଖାଲି)
	    {
		STRMOVE(s, s + 1);
		ଜାରି;
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
    ସ୍ଥିର char_u *
stropt_get_newval(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nextchar,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    set_op_T    *op_arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    set_op_T    op = *op_arg;
    char_u	*save_arg = ଖାଲି;
    char_u	*newval;
    char_u	*s = ଖାଲି;
    char_u	whichwrap[80];

    ଯଦି (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    ଅନ୍ୟଥା ଯଦି (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    ଅନ୍ୟଥା
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	ଯଦି (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	ଅନ୍ୟଥା ଯଦି (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	ଅନ୍ୟଥା ଯଦି (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	ଅନ୍ୟଥା ଯଦି (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_ମୂଲ୍ୟ(origval, &arg, op, flags);
	ଯଦି (newval == ଖାଲି)
	    ଯାଅ done;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	ଯଦି (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    ଯଦି (newval == ଖାଲି)
		ଯାଅ done;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	ଯଦି ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	ଅନ୍ୟଥା
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = 0;
	    ଯଦି (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(newval);
		s = find_dup_item(origval, newval, len, flags);

		// do not add if already there
		ଯଦି ((op == OP_ADDING || op == OP_PREPENDING) && s != ଖାଲି)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		ଯଦି (s == ଖାଲି)
		    s = origval + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    ଯଦି (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    ଅନ୍ୟଥା ଯଦି (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	ଯଦି (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

done:
    ଯଦି (save_arg != ଖାଲି)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    ଫେରନ୍ତୁ newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_set_option_string(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    opt_idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    opt_flags,
	char_u	    **argp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nextchar,
	set_op_T    op_arg,
	long_u	    flags,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cp_val,
	char_u	    *varp_arg,
	ଅକ୍ଷର	    *errbuf,
	size_t	    errbuflen,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *ମୂଲ୍ୟ_checked,
	ଅକ୍ଷର	    **errmsg)
{
    char_u	*arg = *argp;
    set_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = ଖାଲି; // previous value if *varp
    char_u	*newval;
    char_u	*origval = ଖାଲି;
    char_u	*origval_l = ଖାଲି;
    char_u	*origval_g = ଖାଲି;
#ଯଦି defined(FEAT_EVAL)
    char_u	*saved_origval = ଖାଲି;
    char_u	*saved_origval_l = ଖାଲି;
    char_u	*saved_origval_g = ଖାଲି;
    char_u	*saved_newval = ଖାଲି;
#endif

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    ଯଦି ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    ଯଦି ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	ଯଦି (((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)options[opt_idx].indir & PV_BOTH)
						  && origval_l == empty_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    ଯଦି (((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    ଅନ୍ୟଥା
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    ଯଦି (newval == ଖାଲି)
	*(char_u **)(varp) = empty_option;

#ଯଦି defined(FEAT_EVAL)
    ଯଦି (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_ଚାବି
# endif
		      && origval != ଖାଲି && newval != ଖାଲି)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	ଯଦି (origval_l != ଖାଲି)
	    saved_origval_l = vim_strsave(origval_l);
	ଯଦି (origval_g != ଖାଲି)
	    saved_origval_g = vim_strsave(origval_g);
    }
#endif

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	ଯଦି ((opt_flags & OPT_MODELINE)
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
			errbuflen, opt_flags, op, ମୂଲ୍ୟ_checked);

	secure = secure_saved;
    }

#ଯଦି defined(FEAT_EVAL)
    ଯଦି (*errmsg == ଖାଲି)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#endif

    *argp = arg;
    ଫେରନ୍ତୁ *errmsg == ଖାଲି ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    ସ୍ଥିର ଅକ୍ଷର *
do_set_option_bool(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags,
    set_prefix_T prefix,
    long_u	flags,
    char_u	*varp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nextchar,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		afterchar,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_val)

{
    varnumber_T	ମୂଲ୍ୟ;

    ଯଦି (nextchar == '=' || nextchar == ':')
	ଫେରନ୍ତୁ e_invalid_argument;
    ଯଦି (opt_idx < 0 || varp == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    ଯଦି (nextchar == '!')
	ମୂଲ୍ୟ = *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)(varp) ^ 1;
    ଅନ୍ୟଥା ଯଦି (nextchar == '&')
	ମୂଲ୍ୟ = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    ଅନ୍ୟଥା ଯଦି (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    ମୂଲ୍ୟ = -1;
	ଅନ୍ୟଥା ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    ମୂଲ୍ୟ = -1;
#ifdef HAVE_FSYNC
	ଅନ୍ୟଥା ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    ମୂଲ୍ୟ = -1;
#endif
	ଅନ୍ୟଥା
	    ମୂଲ୍ୟ = *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    ଅନ୍ୟଥା
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	ଯଦି (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    ଫେରନ୍ତୁ e_trailing_characters;
	ଯଦି (prefix == PREFIX_INV)
	    ମୂଲ୍ୟ = *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)(varp) ^ 1;
	ଅନ୍ୟଥା
	    ମୂଲ୍ୟ = prefix == PREFIX_NO ? 0 : 1;
    }

    ଫେରନ୍ତୁ set_bool_option(opt_idx, varp, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ମୂଲ୍ୟ, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    ସ୍ଥିର ଅକ୍ଷର *
do_set_option_numeric(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags,
    char_u	**argp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nextchar,
    set_op_T	op,
    long_u	flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_val,
    char_u	*varp,
    ଅକ୍ଷର	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		ମୂଲ୍ୟ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    ଅକ୍ଷର		*errmsg = ଖାଲି;

    ଯଦି (opt_idx < 0 || varp == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;  // "cannot happen"
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
    ଯଦି (nextchar == '&')
	ମୂଲ୍ୟ = (long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    ଅନ୍ୟଥା ଯଦି (nextchar == '<')
    {
	ଯଦି ((long *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    ମୂଲ୍ୟ = NO_LOCAL_UNDOLEVEL;
	ଅନ୍ୟଥା ଯଦି (opt_flags == OPT_LOCAL
		    && ((long *)varp == &curwin->w_p_siso
		     || (long *)varp == &curwin->w_p_so
		     || (long *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    ମୂଲ୍ୟ = -1;
	ଅନ୍ୟଥା
	    ମୂଲ୍ୟ = *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    ଅନ୍ୟଥା ଯଦି (((long *)varp == &p_wc || (long *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	ମୂଲ୍ୟ = string_to_ଚାବି(arg, FALSE);
	ଯଦି (ମୂଲ୍ୟ == 0)
	{
	    errmsg = e_invalid_argument;
	    ଯାଅ skip;
	}
    }
    ଅନ୍ୟଥା ଯଦି (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, ଖାଲି, &i, STR2NR_ALL, &ମୂଲ୍ୟ, ଖାଲି, 0, TRUE, ଖାଲି);
	ଯଦି (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_ସଂଖ୍ୟା_required_after_equal;
	    ଯାଅ skip;
	}
    }
    ଅନ୍ୟଥା
    {
	errmsg = e_ସଂଖ୍ୟା_required_after_equal;
	ଯାଅ skip;
    }

    ଯଦି (op == OP_ADDING)
	ମୂଲ୍ୟ = *(long *)varp + ମୂଲ୍ୟ;
    ଅନ୍ୟଥା ଯଦି (op == OP_PREPENDING)
	ମୂଲ୍ୟ = *(long *)varp * ମୂଲ୍ୟ;
    ଅନ୍ୟଥା ଯଦି (op == OP_REMOVING)
	ମୂଲ୍ୟ = *(long *)varp - ମୂଲ୍ୟ;

    ଯଦି ((long *)varp == &curwin->w_p_sop && ମୂଲ୍ୟ < -1)
    {
	errmsg = e_invalid_argument;
	ଯାଅ skip;
    }

    errmsg = set_ଅଙ୍କ_option(opt_idx, varp, ମୂଲ୍ୟ, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    ଫେରନ୍ତୁ errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    ସ୍ଥିର ଅକ୍ଷର *
do_set_option_keycode(char_u **argp, char_u *ଚାବି_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    ଯଦି (nextchar == '&')
    {
	ଯଦି (add_termcap_entry(ଚାବି_name, TRUE) == FAIL)
	    ଫେରନ୍ତୁ e_not_found_in_termcap;
    }
    ଅନ୍ୟଥା
    {
	++arg; // jump to after the '=' or ':'
	ପାଇଁ (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    ଯଦି (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	add_termcode(ଚାବି_name, arg, FALSE);
	*p = nextchar;
    }
    ଯଦି (full_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Set an option to a new value.
 */
    ସ୍ଥିର ଅକ୍ଷର *
do_set_option_ମୂଲ୍ୟ(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags,
    char_u	**argp,
    set_prefix_T prefix,
    set_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*ଚାବି_name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nextchar,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		afterchar,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cp_val,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*stopopteval,
    ଅକ୍ଷର	*errbuf,
    size_t	errbuflen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ମୂଲ୍ୟ_checked = FALSE;
    ଅକ୍ଷର	*errmsg = ଖାଲି;
    char_u	*arg = *argp;

    ଯଦି (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_set_option_bool(opt_idx, opt_flags, prefix, flags, varp,
						nextchar, afterchar, cp_val);
	ଯଦି (errmsg != ଖାଲି)
	    ଯାଅ skip;
    }
    ଅନ୍ୟଥା
    {
	// numeric or string option
	ଯଦି (vim_strchr((char_u *)"=:&<", nextchar) == ଖାଲି
		|| prefix != PREFIX_NONE)
	{
	    errmsg = e_invalid_argument;
	    ଯାଅ skip;
	}

	ଯଦି (flags & P_ଅଙ୍କ)
	{
	    // numeric option
	    errmsg = do_set_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    ଯଦି (errmsg != ଖାଲି)
		ଯାଅ skip;
	}
	ଅନ୍ୟଥା ଯଦି (opt_idx >= 0)
	{
	    // string option
	    ଯଦି (do_set_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&ମୂଲ୍ୟ_checked, &errmsg) == FAIL)
	    {
		ଯଦି (errmsg != ଖାଲି)
		    ଯାଅ skip;
		*stopopteval = TRUE;
		ଯାଅ skip;
	    }
	}
	ଅନ୍ୟଥା
	{
	    // key code option
	    errmsg = do_set_option_keycode(&arg, ଚାବି_name, nextchar);
	    ଯଦି (errmsg != ଖାଲି)
		ଯାଅ skip;
	}
    }

    ଯଦି (opt_idx >= 0)
	did_set_option(opt_idx, opt_flags, op == OP_NONE, ମୂଲ୍ୟ_checked);

skip:
    *argp = arg;
    ଫେରନ୍ତୁ errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    ସ୍ଥିର ଅକ୍ଷର *
do_set_option(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags,
    char_u	**argp,
    char_u	*arg_start,
    char_u	**startarg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*did_show,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*stopopteval,
    ଅକ୍ଷର	*errbuf,
    size_t	errbuflen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;
    char_u	*arg;
    set_prefix_T prefix;	// no prefix, "no" prefix or "inv" prefix
    set_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	ଚାବି_name[2];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nextchar;	// next non-white char after option name
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		afterchar;	// character just after option name
    ଅକ୍ଷର	*errmsg = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଚାବି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    prefix = get_option_prefix(argp);
    arg = *argp;

    // find end of name
    ଚାବି = 0;
    ଯଦି (parse_option_name(arg, &opt_idx, &len, &ଚାବି) == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    // remember character after option name
    afterchar = arg[len];

    ଯଦି (in_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	ଯଦି (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != ଖାଲି
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    ଯାଅ skip;
	}
    }
    ଅନ୍ୟଥା
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	ଯେତେବେଳେ (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    ଯଦି (op != OP_NONE)
	len++;

    nextchar = arg[len];

    ଯଦି (opt_idx == -1 && ଚାବି == 0)	// found a mismatch: skip
    {
	ଯଦି (in_vim9script() && arg > arg_start
		&& vim_strchr((char_u *)"!&<", *arg) != ଖାଲି)
	    errmsg = e_no_white_space_allowed_between_option_and;
	ଅନ୍ୟଥା
	    errmsg = e_unknown_option;
	ଯାଅ skip;
    }

    ଯଦି (opt_idx >= 0)
    {
	ଯଦି (options[opt_idx].var == ଖାଲି)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    ଯଦି (vim_strchr((char_u *)"=:!&<", nextchar) == ଖାଲି
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    ଯାଅ skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    ଅନ୍ୟଥା
    {
	flags = P_STRING;
	varp = ଖାଲି;
	ଯଦି (ଚାବି < 0)
	{
	    ଚାବି_name[0] = KEY2TERMCAP0(ଚାବି);
	    ଚାବି_name[1] = KEY2TERMCAP1(ଚାବି);
	}
	ଅନ୍ୟଥା
	{
	    ଚାବି_name[0] = KS_ଚାବି;
	    ଚାବି_name[1] = (ଚାବି & 0xff);
	}
    }

    // Make sure the option value can be changed.
    ଯଦି (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, prefix) == FAIL)
	ଯାଅ skip;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cp_val = p_cp;
    ଯଦି (vim_strchr((char_u *)"?=:!&<", nextchar) != ଖାଲି)
    {
	arg += len;
	ଯଦି (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    ଯଦି (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    ଅନ୍ୟଥା		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	ଯଦି (vim_strchr((char_u *)"?!&<", nextchar) != ଖାଲି
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    ଯାଅ skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    ଯଦି (nextchar == '?'
	    || (prefix == PREFIX_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == ଖାଲି
		&& !(flags & P_BOOL)))
    {
	// print value
	ଯଦି (*did_show)
	    ସନ୍ଦେଶ_putchar('\n');	    // cursor below last one
	ଅନ୍ୟଥା
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	ଯଦି (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ifdef FEAT_EVAL
	    ଯଦି (p_verbose > 0)
	    {
		// Mention where the option was last set.
		ଯଦି (varp == options[opt_idx].var)
		    last_set_ସନ୍ଦେଶ(options[opt_idx].script_ctx);
		ଅନ୍ୟଥା ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)options[opt_idx].indir & PV_WIN)
		    last_set_ସନ୍ଦେଶ(curwin->w_p_script_ctx[
			    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)options[opt_idx].indir & PV_MASK]);
		ଅନ୍ୟଥା ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)options[opt_idx].indir & PV_BUF)
		    last_set_ସନ୍ଦେଶ(curbuf->b_p_script_ctx[
			    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)options[opt_idx].indir & PV_MASK]);
	    }
#endif
	}
	ଅନ୍ୟଥା
	{
	    char_u	    *p;

	    p = find_termcode(ଚାବି_name);
	    ଯଦି (p == ଖାଲି)
	    {
		errmsg = e_ଚାବି_code_not_set;
		ଯାଅ skip;
	    }
	    ଅନ୍ୟଥା
		(ଶୂନ୍ୟ)show_one_termcode(ଚାବି_name, p, TRUE);
	}
	ଯଦି (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    ଅନ୍ୟଥା
    {
	errmsg = do_set_option_ମୂଲ୍ୟ(opt_idx, opt_flags, &arg, prefix, op,
					flags, varp, ଚାବି_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    ଫେରନ୍ତୁ errmsg;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_set(
    char_u	*arg_start,	// option string (may be written to!)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags)
{
    char_u	*arg = arg_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_show = FALSE;   // already showed one value

    ଯଦି (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	ଯାଅ theend;
    }

    ଯେତେବେଳେ (*arg != NUL)		// loop to process all options
    {
	ଯଦି (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    ଯଦି (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		set_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    ଅନ୍ୟଥା
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stopopteval = FALSE;
	    ଅକ୍ଷର	*errmsg = ଖାଲି;
	    ଅକ୍ଷର	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_set_option(opt_flags, &arg, arg_start, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    ଯଦି (stopopteval)
		ଭାଙ୍ଗ;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    ପାଇଁ (i = 0; i < 2 ; ++i)
	    {
		ଯେତେବେଳେ (*arg != NUL && !VIM_ISWHITE(*arg))
		    ଯଦି (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		ଯଦି (*arg != '=')
		    ଭାଙ୍ଗ;
	    }

	    ଯଦି (errmsg != ଖାଲି)
	    {
		i = vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		ଯଦି (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((ଅକ୍ଷର *)IObuff);	// show error highlighted
		--no_wait_return;

		ଫେରନ୍ତୁ FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    ଯଦି (silent_mode && did_show)
    {
	// After displaying option values in silent mode.
	silent_mode = FALSE;
	info_message = TRUE;	// use mch_msg(), not mch_errmsg()
	ସନ୍ଦେଶ_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = TRUE;
	info_message = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    ଶୂନ୍ୟ
did_set_option(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    opt_idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    opt_flags,	    // possibly with OPT_MODELINE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    new_ମୂଲ୍ୟ_2,	    // value was replaced completely
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ମୂଲ୍ୟ_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_SET;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    ଯଦି (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    ଯଦି (!ମୂଲ୍ୟ_checked && (secure
#ifdef HAVE_SANDBOX
	    || sandbox != 0
#endif
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	ଯଦି (flagsp_local != ଖାଲି)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    ଅନ୍ୟଥା ଯଦି (new_ମୂଲ୍ୟ_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	ଯଦି (flagsp_local != ଖାଲି)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
string_to_ଚାବି(char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା multi_byte)
{
    ଯଦି (*arg == '<' && arg[1])
	ଫେରନ୍ତୁ find_ଚାବି_option(arg + 1, TRUE);
    ଯଦି (*arg == '^' && arg[1])
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି = Ctrl_chr(arg[1]);
	ଯଦି (ଚାବି == 0)		// ^@ is <Nul>
	    ଚାବି = K_ZERO;
	ଫେରନ୍ତୁ ଚାବି;
    }
    ଯଦି (multi_byte)
	ଫେରନ୍ତୁ PTR2CHAR(arg);
    ଫେରନ୍ତୁ *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    ଶୂନ୍ୟ
did_set_title(ଶୂନ୍ୟ)
{
    ଯଦି (starting != NO_SCREEN
#ifdef FEAT_GUI
	    && !gui.starting
#endif
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    ଶୂନ୍ୟ
set_options_bin(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oldval,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newval,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    ଯଦି (newval)
    {
	ଯଦି (!oldval)		// switched on
	{
	    ଯଦି (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    ଯଦି (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	ଯଦି (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	ଯଦି (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    ଅନ୍ୟଥା ଯଦି (oldval)		// switched off
    {
	ଯଦି (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	ଯଦି (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#ଯଦି defined(FEAT_EVAL)
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
    ସ୍ଥିର char_u *
option_expand(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    ଯଦି (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    ଯଦି (val != ଖାଲି && STRLEN(val) > MAXPATHL)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (val == ଖାଲି)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା esc = var == &p_tags || var == &p_path;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : ଖାଲି, FALSE,
#ifdef FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#endif
				  ଖାଲି);
    ଯଦି (STRCMP(NameBuff, val) == 0)   // they are the same
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ NameBuff;
}

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
didset_options(ଶୂନ୍ୟ)
{
    // initialize the table for 'iskeyword' et.al.
    (ଶୂନ୍ୟ)init_chartab();

    didset_string_options();

#ifdef FEAT_SPELL
    (ଶୂନ୍ୟ)spell_check_msm();
    (ଶୂନ୍ୟ)spell_check_sps();
    (ଶୂନ୍ୟ)compile_cap_prog(curwin->w_s);
    (ଶୂନ୍ୟ)did_set_spell_option();
#endif
    // set cedit_key
    (ଶୂନ୍ୟ)did_set_cedit(ଖାଲି);
#ifdef FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_set_breakat(ଖାଲି);
#endif
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
didset_options2(ଶୂନ୍ୟ)
{
    // Initialize the highlight_attr[] table.
    (ଶୂନ୍ୟ)highlight_changed();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (ଶୂନ୍ୟ)set_listchars_option(curwin, curwin->w_p_lcs, TRUE, ଖାଲି, 0);

    // Parse default for 'fillchars'.
    (ଶୂନ୍ୟ)set_fillchars_option(curwin, curwin->w_p_fcs, TRUE, ଖାଲି, 0);

#ifdef FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (ଶୂନ୍ୟ)did_set_clipboard(ଖାଲି);
#endif
#ifdef FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (ଶୂନ୍ୟ)tabstop_set(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (ଶୂନ୍ୟ)tabstop_set(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#endif
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    ଶୂନ୍ୟ
check_options(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;

    ପାଇଁ (opt_idx = 0; options[opt_idx].fullname != ଖାଲି; opt_idx++)
	ଯଦି ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != ଖାଲି)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_term_opt_idx(char_u **p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx;

    ପାଇଁ (opt_idx = 1; options[opt_idx].fullname != ଖାଲି; opt_idx++)
	ଯଦି (options[opt_idx].var == (char_u *)p)
	    ଫେରନ୍ତୁ opt_idx;
    ଫେରନ୍ତୁ -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_term_option_alloced(char_u **p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx = get_term_opt_idx(p);

    ଯଦି (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    ଫେରନ୍ତୁ opt_idx;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
was_set_insecurely(win_T *wp, char_u *opt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx = findoption(opt);
    long_u  *flagp;

    ଯଦି (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	ଫେରନ୍ତୁ (*flagp & P_INSECURE) != 0;
    }
    internal_error("was_set_insecurely()");
    ଫେରନ୍ତୁ -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    ସ୍ଥିର long_u *
insecure_flag(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags)
{
    ଯଦି (opt_flags & OPT_LOCAL)
	ବାଛ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)options[opt_idx].indir)
	{
	    ସ୍ଥିତି PV_WRAP:	ଫେରନ୍ତୁ &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    ସ୍ଥିତି PV_STL:	ଫେରନ୍ତୁ &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    ସ୍ଥିତି PV_FDE:	ଫେରନ୍ତୁ &wp->w_p_fde_flags;
	    ସ୍ଥିତି PV_FDT:	ଫେରନ୍ତୁ &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    ସ୍ଥିତି PV_BEXPR:	ଫେରନ୍ତୁ &wp->w_buffer->b_p_bexpr_flags;
#  endif
	    ସ୍ଥିତି PV_INDE:	ଫେରନ୍ତୁ &wp->w_buffer->b_p_inde_flags;
	    ସ୍ଥିତି PV_FEX:	ଫେରନ୍ତୁ &wp->w_buffer->b_p_fex_flags;
#  ifdef FEAT_FIND_ID
	    ସ୍ଥିତି PV_INEX:	ଫେରନ୍ତୁ &wp->w_buffer->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    ସ୍ଥିତି PV_CPT:	ଫେରନ୍ତୁ &wp->w_buffer->b_p_cpt_flags;
#  endif
# endif
	}
    ଅନ୍ୟଥା
	// For global value of window-local options, use flags in w_allbuf_opt.
	ବାଛ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)options[opt_idx].indir)
	{
	    ସ୍ଥିତି PV_WRAP:	ଫେରନ୍ତୁ &wp->w_allbuf_opt.wo_wrap_flags;
# ଯଦି defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    ସ୍ଥିତି PV_FDE:	ଫେରନ୍ତୁ &wp->w_allbuf_opt.wo_fde_flags;
	    ସ୍ଥିତି PV_FDT:	ଫେରନ୍ତୁ &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    ଫେରନ୍ତୁ &options[opt_idx].flags;
}
#endif

/*
 * Redraw the window title and/or tab page text later.
 */
    ଶୂନ୍ୟ
redraw_titles(ଶୂନ୍ୟ)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#ଯଦି defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_name(char_u *val, ଅକ୍ଷର *allowed)
{
    char_u *s;

    ପାଇଁ (s = val; *s != NUL; ++s)
	ଯଦି (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == ଖାଲି)
	    ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    ଶୂନ୍ୟ
set_option_sctx_idx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags, sctx_T script_ctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indir = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    ଯଦି (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    ଯଦି (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    ଯଦି (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    ଯଦି (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    ଯଦି (both || (opt_flags & OPT_LOCAL))
    {
	ଯଦି (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	ଅନ୍ୟଥା ଯଦି (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	    ଯଦି (both)
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
is_option_ମୂଲ୍ୟ_vim9(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indir = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)options[opt_idx].indir;
    sctx_T	*sctx = ଖାଲି;

    ଯଦି ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    ଯଦି ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	ଯଦି (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_MASK];
	ଅନ୍ୟଥା ଯଦି (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_MASK];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    ଫେରନ୍ତୁ !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_version >= SCRIPT_VERSION_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(ଅକ୍ଷର *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = findoption((char_u *)name);

    ଯଦି (idx >= 0)
	ଫେରନ୍ତୁ &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    ଶୂନ୍ୟ
set_term_option_sctx_idx(ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx)
{
    char_u  buf[5];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx;

    ଯଦି (name == ଖାଲି)
	idx = opt_idx;
    ଅନ୍ୟଥା
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    ଯଦି (idx >= 0)
	set_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#endif

#ଯଦି defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
apply_optionset_autocmd(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	opt_idx,
	long	opt_flags,
	long	oldval,
	long	oldval_g,
	long	newval,
	ଅକ୍ଷର	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    ଯଦି (starting || errmsg != ଖାଲି || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	ଫେରନ୍ତୁ;

    len = vim_snprintf_safelen((ଅକ୍ଷର *)buf_new, ଆକାର(buf_new), "%ld", newval);
    set_vim_var_string(VV_OPTION_NEW, buf_new, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len);
    len = vim_snprintf_safelen((ଅକ୍ଷର *)buf_type, ଆକାର(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len);

    buf_oldlen = vim_snprintf_safelen((ଅକ୍ଷର *)buf_old, ଆକାର(buf_old), "%ld", oldval);
    set_vim_var_string(VV_OPTION_OLD, buf_old, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buf_oldlen);

    ଯଦି (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buf_oldlen);
    }
    ଯଦି (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buf_oldlen);
    }
    ଯଦି ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((ଅକ୍ଷର *)buf_old_global,
	    ଆକାର(buf_old_global), "%ld", oldval_g);

	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buf_oldlen);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buf_old_globallen);
    }
    ଯଦି (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    ଖାଲି, FALSE, ଖାଲି);
    reset_v_option_vars();
}
#endif

#ଯଦି defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    ଅକ୍ଷର *
did_set_arabic(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	ଯଦି (!p_tbidi)
	{
	    // set rightleft mode
	    ଯଦି (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		changed_window_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    ଯଦି (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	ଯଦି (STRCMP(p_enc, "utf-8") != 0)
	{
	    ସ୍ଥିର ଅକ୍ଷର *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    ସନ୍ଦେଶ_source(HL_ATTR(HLF_W));
	    ସନ୍ଦେଶ_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = set_option_ମୂଲ୍ୟ((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    ଅନ୍ୟଥା
    {
	// 'arabic' is reset, handle various sub-settings.
	ଯଦି (!p_tbidi)
	{
	    // reset rightleft mode
	    ଯଦି (curwin->w_p_rl)
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
	curbuf->b_p_imsearch = B_IMODE_USE_ଭର୍ତ୍ତି;
# endif
    }

    ଫେରନ୍ତୁ errmsg;
}
#endif

#ଯଦି defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    ଅକ୍ଷର *
did_set_autochdir(optset_T *args UNUSED)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    ଅକ୍ଷର *
did_set_ballooneval(optset_T *args)
{
    ଯଦି (balloonEvalForTerm)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (p_beval && !args->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    ଅନ୍ୟଥା ଯଦି (!p_beval && args->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_BEVAL_TERM)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    ଅକ୍ଷର *
did_set_balloonevalterm(optset_T *args UNUSED)
{
    mch_bevalterm_changed();
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Process the updated 'binary' option value.
 */
    ଅକ୍ଷର *
did_set_ଦ୍ୱିମିକ(optset_T *args)
{
    // when 'bin' is set also set some other options
    set_options_bin(args->os_oldval.boolean, curbuf->b_p_bin, args->os_flags);
    redraw_titles();

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the updated 'buflisted' option value.
 */
    ଅକ୍ଷର *
did_set_buflisted(optset_T *args)
{
    // when 'buflisted' changes, trigger autocommands
    ଯଦି (args->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						ଖାଲି, ଖାଲି, TRUE, curbuf);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the new 'cmdheight' option value.
 */
    ଅକ୍ଷର *
did_set_cmdheight(optset_T *args)
{
    long old_ମୂଲ୍ୟ = args->os_oldval.ସଂଖ୍ୟା;
    ଅକ୍ଷର *errmsg = ଖାଲି;

    // if p_ch changed value, change the command line height
    ଯଦି (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_argument_must_be_positive;
	p_ch = MIN_CMDHEIGHT;
    }
    ଯଦି (p_ch > Rows - min_rows() + MIN_CMDHEIGHT)
	p_ch = Rows - min_rows() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    ଯଦି ((p_ch != old_ମୂଲ୍ୟ
		|| tabline_height() + topframe->fr_height != Rows - p_ch)
	    && full_screen
#ifdef FEAT_GUI
	    && !gui.starting
#endif
       )
	command_height();

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    ଅକ୍ଷର *
did_set_compatible(optset_T *args UNUSED)
{
    compatible_set();
    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    ଅକ୍ଷର *
did_set_conceallevel(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (curwin->w_p_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_cole = 0;
    }
    ଅନ୍ୟଥା ଯଦି (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_cole = 3;
    }
    ଯଦି (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    ଅନ୍ୟଥା ଯଦି (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    ଫେରନ୍ତୁ errmsg;
}
#endif

#ଯଦି defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    ଅକ୍ଷର *
did_set_diff(optset_T *args UNUSED)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    ଯଦି (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    ଅକ୍ଷର *
did_set_eof_eol_fixeol_bomb(optset_T *args UNUSED)
{
    // redraw the window title and tab page text
    redraw_titles();
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the updated 'equalalways' option value.
 */
    ଅକ୍ଷର *
did_set_equalalways(optset_T *args)
{
    ଯଦି (p_ea && !args->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    ଅକ୍ଷର *
did_set_foldcolumn(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (curwin->w_p_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_fdc = 0;
    }
    ଅନ୍ୟଥା ଯଦି (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_fdc = 12;
    }
    ଯଦି (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    ଅନ୍ୟଥା ଯଦି (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    ଅକ୍ଷର *
did_set_foldlevel(optset_T *args UNUSED)
{
    ଯଦି (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the new 'foldminlines' option value.
 */
    ଅକ୍ଷର *
did_set_foldminlines(optset_T *args UNUSED)
{
    foldUpdateAll(curwin);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    ଅକ୍ଷର *
did_set_foldnestmax(optset_T *args UNUSED)
{
    ଯଦି (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_ସନ୍ଧାନ_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    ଅକ୍ଷର *
did_set_hlsearch(optset_T *args UNUSED)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    set_no_hlsearch(FALSE);
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Process the updated 'ignorecase' option value.
 */
    ଅକ୍ଷର *
did_set_ignorecase(optset_T *args UNUSED)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    ଯଦି (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(HAVE_ନିବେଶ_METHOD)
/*
 * Process the updated 'imdisable' option value.
 */
    ଅକ୍ଷର *
did_set_imdisable(optset_T *args UNUSED)
{
    // Only de-activate it here, it will be enabled when changing mode.
    ଯଦି (p_imdisable)
	im_set_active(FALSE);
    ଅନ୍ୟଥା ଯଦି (State & MODE_ଭର୍ତ୍ତି)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Process the new 'iminsert' option value.
 */
    ଅକ୍ଷର *
did_set_iminsert(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    ଯଦି (termcap_active)	// don't do this in the alternate screen
	showmode();
#ଯଦି defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    ଅକ୍ଷର *
did_set_imsearch(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    ଫେରନ୍ତୁ errmsg;
}

#ଯଦି defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    ଅକ୍ଷର *
did_set_imstyle(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_argument;

    ଫେରନ୍ତୁ errmsg;
}
#endif

/*
 * Process the updated 'insertmode' option value.
 */
    ଅକ୍ଷର *
did_set_insertmode(optset_T *args)
{
    // when 'insertmode' is set from an autocommand need to do work here
    ଯଦି (p_im)
    {
	ଯଦି ((State & MODE_ଭର୍ତ୍ତି) == 0)
	    need_start_insertmode = TRUE;
	stop_ଭର୍ତ୍ତି_mode = FALSE;
    }
    // only reset if it was set previously
    ଅନ୍ୟଥା ଯଦି (args->os_oldval.boolean)
    {
	need_start_insertmode = FALSE;
	stop_ଭର୍ତ୍ତି_mode = TRUE;
	ଯଦି (restart_edit != 0 && mode_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    ଅକ୍ଷର *
did_set_langnoremap(optset_T *args UNUSED)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the updated 'langremap' option value.
 */
    ଅକ୍ଷର *
did_set_langremap(optset_T *args UNUSED)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Process the new 'laststatus' option value.
 */
    ଅକ୍ଷର *
did_set_laststatus(optset_T *args UNUSED)
{
    last_status(FALSE);	// (re)set last window status line
    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    ଅକ୍ଷର *
did_set_linespace(optset_T *args UNUSED)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    ଯଦି (gui.in_use && gui_mch_adjust_charheight() == OK)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Process the updated 'lisp' option value.
 */
    ଅକ୍ଷର *
did_set_lisp(optset_T *args UNUSED)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (ଶୂନ୍ୟ)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the new 'maxcombine' option value.
 */
    ଅକ୍ଷର *
did_set_maxcombine(optset_T *args UNUSED)
{
    ଯଦି (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    ଅନ୍ୟଥା ଯଦି (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the updated 'modifiable' option value.
 */
    ଅକ୍ଷର *
did_set_modifiable(optset_T *args UNUSED)
{
    // when 'modifiable' is changed, redraw the window title

#ifdef FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    ଯଦି (curbuf->b_p_ma && (term_in_normal_mode(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_term != ଖାଲି && !term_is_finished(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	ଫେରନ୍ତୁ e_cannot_make_terminal_with_running_job_modifiable;
    }
#endif
    redraw_titles();

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the updated 'modified' option value.
 */
    ଅକ୍ଷର *
did_set_modified(optset_T *args)
{
    ଯଦି (!args->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_set = !!args->os_newval.boolean;
    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    ଅକ୍ଷର *
did_set_mousehide(optset_T *args UNUSED)
{
    ଯଦି (!p_mh)
	gui_mch_mousehide(FALSE);
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    ଅକ୍ଷର *
did_set_ସଂଖ୍ୟା_relativenumber(optset_T *args UNUSED)
{
#ଯଦି defined(FEAT_SIGNS) && defined(FEAT_GUI)
    ଯଦି (gui.in_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != ଖାଲି)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	ଯଦି (!(curwin->w_p_nu && ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)args->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#endif
    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    ଅକ୍ଷର *
did_set_numberwidth(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    // 'numberwidth' must be positive
    ଯଦି (curwin->w_p_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_nuw = 1;
    }
    ଅନ୍ୟଥା ଯଦି (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_nuw = 20;
    }
    ଯଦି (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    ଅନ୍ୟଥା ଯଦି (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_ଧାଡ଼ି_ଗଣନା = 0; // trigger a redraw

    ଫେରନ୍ତୁ errmsg;
}
#endif

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    ଅକ୍ଷର *
did_set_osctimeoutlen(optset_T *args)
{
    ଯଦି (p_ost < 0)
    {
	p_ost = args->os_oldval.ସଂଖ୍ୟା;
	ଫେରନ୍ତୁ e_argument_must_be_positive;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    ଅକ୍ଷର *
did_set_paste(optset_T *args UNUSED)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	old_p_paste = FALSE;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_sm = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_sta = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_ru = 0;
#ifdef FEAT_RIGHTLEFT
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_ri = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_hkmap = 0;
#endif
    buf_T	*buf;

    ଯଦି (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	ଯଦି (!old_p_paste)
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
		ଯଦି (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != empty_option
					   ? vim_strsave(buf->b_p_vsts) : ଖାଲି;
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
	    ଯଦି (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != empty_option
						  ? vim_strsave(p_vsts) : ଖାଲି;
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
	    ଯଦି (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = empty_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#endif
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	ଯଦି (p_ru)
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
	ଯଦି (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = empty_option;
#endif
    }

    // Paste switched from on to off: Restore saved values.
    ଅନ୍ୟଥା ଯଦି (old_p_paste)
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
	    ଯଦି (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : empty_option;
	    vim_free(buf->b_p_vsts_array);
	    ଯଦି (buf->b_p_vsts && buf->b_p_vsts != empty_option)
		(ଶୂନ୍ୟ)tabstop_set(buf->b_p_vsts, &buf->b_p_vsts_array);
	    ଅନ୍ୟଥା
		buf->b_p_vsts_array = ଖାଲି;
#endif
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	ଯଦି (p_ru != save_ru)
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
	ଯଦି (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : empty_option;
#endif
    }

    old_p_paste = p_paste;

#ଯଦି defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#endif

    ଫେରନ୍ତୁ ଖାଲି;
}


#ifdef FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    ଅକ୍ଷର *
did_set_previewwindow(optset_T *args UNUSED)
{
    ଯଦି (!curwin->w_p_pvw)
	ଫେରନ୍ତୁ ଖାଲି;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	ଯଦି (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    ଫେରନ୍ତୁ e_preview_window_already_exists;
	}

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    ଅକ୍ଷର *
did_set_pyxversion(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_argument;

    ଫେରନ୍ତୁ errmsg;
}
#endif

/*
 * Process the updated 'readonly' option value.
 */
    ଅକ୍ଷର *
did_set_readonly(optset_T *args)
{
    // when 'readonly' is reset globally, also reset readonlymode
    ଯଦି (!curbuf->b_p_ro && (args->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    ଯଦି (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    ଅକ୍ଷର *
did_set_scrollbind(optset_T *args UNUSED)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    ଯଦି (!curwin->w_p_scb)
	ଫେରନ୍ତୁ ଖାଲି;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    ଅକ୍ଷର *
did_set_maxsearchcount(optset_T *args UNUSED)
{
    ଅକ୍ଷର	*errmsg = ଖାଲି;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#define MAX_ସନ୍ଧାନ_ଗଣନା 9999

    ଯଦି (p_msc <= 0)
	errmsg = e_argument_must_be_positive;
    ଅନ୍ୟଥା ଯଦି (p_msc > MAX_ସନ୍ଧାନ_ଗଣନା)
	errmsg = e_invalid_argument;

    ଯଦି (errmsg != ଖାଲି)
	p_msc = 99;

    ଫେରନ୍ତୁ errmsg;
#undef MAX_ସନ୍ଧାନ_ଗଣନା
}

#ଯଦି defined(BACKSLASH_IN_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    ଅକ୍ଷର *
did_set_shellslash(optset_T *args UNUSED)
{
    ଯଦି (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    ଅନ୍ୟଥା
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
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    ଅକ୍ଷର *
did_set_shiftwidth_tabstop(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    ଅକ୍ଷର	*errmsg = ଖାଲି;

    ଯଦି (curbuf->b_p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_ts = 8;
    }
    ଅନ୍ୟଥା ଯଦି (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_ts = 8;
    }
    ଯଦି (p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ts = 8;
    }
    ଅନ୍ୟଥା ଯଦି (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	p_ts = 8;
    }

    ଯଦି (curbuf->b_p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_ଗଣନା(curbuf->b_p_vts_array) > 0
		       ? tabstop_first(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#ଅନ୍ୟଥା
	curbuf->b_p_sw = curbuf->b_p_ts;
#endif
    }
    ଯଦି (p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_ଗଣନା(curbuf->b_p_vts_array) > 0
	     ? tabstop_first(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#ଅନ୍ୟଥା
	p_sw = curbuf->b_p_ts;
#endif
    }

#ifdef FEAT_FOLDING
    ଯଦି (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#endif
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    ଯଦି (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    ଅକ୍ଷର *
did_set_showtabline(optset_T *args UNUSED)
{
    shell_new_rows();	// recompute window positions and heights
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    ଅକ୍ଷର *
did_set_smoothscroll(optset_T *args UNUSED)
{
    ଯଦି (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    ଅକ୍ଷର *
did_set_spell(optset_T *args UNUSED)
{
    ଯଦି (curwin->w_p_spell)
	ଫେରନ୍ତୁ parse_spelllang(curwin);

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Process the updated 'swapfile' option value.
 */
    ଅକ୍ଷର *
did_set_swapfile(optset_T *args UNUSED)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    ଯଦି (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    ଅନ୍ୟଥା
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_TERMGUICOLORS)
    ଅକ୍ଷର *
did_set_termguicolors(optset_T *args UNUSED)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    ଯଦି (
#  ifdef VIMDLL
	    !gui.in_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_set = TRUE;
	p_tgc = 0;
	ଫେରନ୍ତୁ e_24_bit_colors_are_not_supported_on_this_environment;
    }
    ଯଦି (is_term_win32())
	swap_tcap();
# endif
# ifdef FEAT_GUI
    ଯଦି (!gui.in_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    ଯଦି (is_term_win32())
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

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

    ଅକ୍ଷର *
did_set_termsync(optset_T *args UNUSED)
{
    ଯଦି (!p_tsy)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);
    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    ଅକ୍ଷର *
did_set_termwinscroll(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    ଅକ୍ଷର	*errmsg = ଖାଲି;

    ଯଦି (*pp < 1)
    {
	errmsg = e_argument_must_be_positive;
	*pp = 1;
    }

    ଫେରନ୍ତୁ errmsg;
}
#endif

/*
 * Process the updated 'terse' option value.
 */
    ଅକ୍ଷର *
did_set_terse(optset_T *args UNUSED)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_ସନ୍ଧାନ);

    // insert 's' in p_shm
    ଯଦି (p_terse && p == ଖାଲି)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    ଅନ୍ୟଥା ଯଦି (!p_terse && p != ଖାଲି)
	STRMOVE(p, p + 1);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the updated 'textauto' option value.
 */
    ଅକ୍ଷର *
did_set_textauto(optset_T *args)
{
    // when 'textauto' is set or reset also change 'fileformats'
    set_string_option_direct((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | args->os_flags, 0);

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the updated 'textmode' option value.
 */
    ଅକ୍ଷର *
did_set_textmode(optset_T *args)
{
    // when 'textmode' is set or reset also change 'fileformat'
    set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, args->os_flags);

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the new 'textwidth' option value.
 */
    ଅକ୍ଷର *
did_set_textwidth(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (curbuf->b_p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_tw = 0;
    }
    ଯଦି (p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tw = 0;
    }
#ifdef FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(ଖାଲି, wp);
    }
#endif

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    ଅକ୍ଷର *
did_set_title_icon(optset_T *args UNUSED)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_set_title();
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the new 'titlelen' option value.
 */
    ଅକ୍ଷର *
did_set_titlelen(optset_T *args)
{
    long old_ମୂଲ୍ୟ = args->os_oldval.ସଂଖ୍ୟା;
    ଅକ୍ଷର *errmsg = ଖାଲି;

    // if 'titlelen' has changed, redraw the title
    ଯଦି (p_titlelen < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_titlelen = 85;
    }
    ଯଦି (starting != NO_SCREEN && old_ମୂଲ୍ୟ != p_titlelen)
	need_maketitle = TRUE;

    ଫେରନ୍ତୁ errmsg;
}

#ଯଦି defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    ଅକ୍ଷର *
did_set_undofile(optset_T *args)
{
    // Only take action when the option was set.
    ଯଦି (!curbuf->b_p_udf && !p_udf)
	ଫେରନ୍ତୁ ଖାଲି;

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
	ଯଦି ((curbuf == save_curbuf
		    || (args->os_flags & OPT_GLOBAL)
		    || args->os_flags == 0)
		&& !curbufIsChanged() && curbuf->b_ml.ml_mfp != ଖାଲି)
	{
# ifdef FEAT_CRYPT
	    ଯଦି (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
		ଜାରି;
# endif
	    u_compute_hash(hash);
	    u_read_undo(ଖାଲି, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Process the new global 'undolevels' option value.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
update_global_undolevels(long ମୂଲ୍ୟ, long old_ମୂଲ୍ୟ)
{
    // sync undo before 'undolevels' changes

    // use the old value, otherwise u_sync() may not work properly
    p_ul = old_ମୂଲ୍ୟ;
    u_sync(TRUE);
    p_ul = ମୂଲ୍ୟ;
}

/*
 * Process the new buffer local 'undolevels' option value.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
update_buflocal_undolevels(long ମୂଲ୍ୟ, long old_ମୂଲ୍ୟ)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_ମୂଲ୍ୟ;
    u_sync(TRUE);
    curbuf->b_p_ul = ମୂଲ୍ୟ;
}

/*
 * Process the new 'undolevels' option value.
 */
    ଅକ୍ଷର *
did_set_undolevels(optset_T *args)
{
    long *pp = (long *)args->os_varp;

    ଯଦି (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(args->os_newval.ସଂଖ୍ୟା,
						       args->os_oldval.ସଂଖ୍ୟା);
    ଅନ୍ୟଥା ଯଦି (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(args->os_newval.ସଂଖ୍ୟା,
						       args->os_oldval.ସଂଖ୍ୟା);

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the new 'updatecount' option value.
 */
    ଅକ୍ଷର *
did_set_updatecount(optset_T *args)
{
    long old_ମୂଲ୍ୟ = args->os_oldval.ସଂଖ୍ୟା;
    ଅକ୍ଷର *errmsg = ଖାଲି;

    // when 'updatecount' changes from zero to non-zero, open swap files
    ଯଦି (p_uc < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_uc = 100;
    }
    ଯଦି (p_uc && !old_ମୂଲ୍ୟ)
	ml_open_files();

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    ଅକ୍ଷର *
did_set_weirdinvert(optset_T *args)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    ଯଦି (p_wiv && !args->os_oldval.boolean)
	T_XS = (char_u *)"y";
    ଅନ୍ୟଥା ଯଦି (!p_wiv && args->os_oldval.boolean)
	T_XS = empty_option;
    p_wiv = (*T_XS != NUL);

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    ଅକ୍ଷର *
did_set_wildchar(optset_T *args)
{
    long c = *(long *)args->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    ଯଦି (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the new 'window' option value.
 */
    ଅକ୍ଷର *
did_set_window(optset_T *args UNUSED)
{
    ଯଦି (p_window < 1)
	p_window = 1;
    ଅନ୍ୟଥା ଯଦି (p_window >= Rows)
	p_window = Rows - 1;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    ଅକ୍ଷର *
did_set_winheight_helpheight(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    ଅକ୍ଷର	*errmsg = ଖାଲି;

    ଯଦି (p_wh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wh = 1;
    }
    ଯଦି (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    ଯଦି (p_hh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hh = 0;
    }

    // Change window height NOW
    ଯଦି (!ONE_WINDOW)
    {
	ଯଦି (pp == &p_wh && curwin->w_height < p_wh)
	    win_setheight((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_wh);
	ଯଦି (pp == &p_hh && curbuf->b_help && curwin->w_height < p_hh)
	    win_setheight((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_hh);
    }

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    ଅକ୍ଷର *
did_set_winminheight(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (p_wmh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmh = 0;
    }
    ଯଦି (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    ଅକ୍ଷର *
did_set_winminwidth(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (p_wmw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmw = 0;
    }
    ଯଦି (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    ଅକ୍ଷର *
did_set_winwidth(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (p_wiw < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wiw = 1;
    }
    ଯଦି (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    ଯଦି (!ONE_WINDOW && curwin->w_width < p_wiw)
	win_setwidth((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_wiw);

    ଫେରନ୍ତୁ errmsg;
}

#ଯଦି defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    ଅକ୍ଷର *
did_set_wlsteal(optset_T *args UNUSED)
{
    clip_reset_wayland();

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    ଅକ୍ଷର *
did_set_wltimeoutlen(optset_T *args)
{
    ଯଦି (p_wtm < 0)
    {
	p_wtm = args->os_oldval.ସଂଖ୍ୟା;
	ଫେରନ୍ତୁ e_argument_must_be_positive;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Process the updated 'wrap' option value.
 */
    ଅକ୍ଷର *
did_set_wrap(optset_T *args UNUSED)
{
    // Set w_leftcol or w_skipcol to zero.
    ଯଦି (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    ଅନ୍ୟଥା
	curwin->w_skipcol = 0;

    ଫେରନ୍ତୁ ଖାଲି;
}

#ifdef FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    ଅକ୍ଷର *
did_set_xhistory(optset_T *args)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_p_chi = (long*)args->os_varp == &p_chi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
    long *arg = (is_p_chi) ? &p_chi :(long*)args->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    ଯଦି (*arg < 1)
    {
	*arg = args->os_oldval.ସଂଖ୍ୟା;
	ଫେରନ୍ତୁ e_cannot_have_negative_or_zero_ସଂଖ୍ୟା_of_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    ଯଦି (*arg > 100)
    {
	*arg = args->os_oldval.ସଂଖ୍ୟା;
	ଫେରନ୍ତୁ e_cannot_have_more_than_hundred_quickfix;
    }

    ଯଦି (is_p_chi)
	err = qf_resize_ଗଦା(*arg);
    ଅନ୍ୟଥା
	err = ll_resize_ଗଦା(curwin, *arg);

    ଯଦି (err == FAIL)
	ଫେରନ୍ତୁ e_failed_resizing_quickfix_ଗଦା;

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ସ୍ଥିର ଅକ୍ଷର *
set_bool_option(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ମୂଲ୍ୟ,			// new value
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_ମୂଲ୍ୟ = *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp;
#ଯଦି defined(FEAT_EVAL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_global_ମୂଲ୍ୟ = 0;
#endif
    ଅକ୍ଷର	*errmsg = ଖାଲି;

    // Disallow changing some options from secure mode
    ଯଦି ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	ଫେରନ୍ତୁ e_not_allowed_here;

#ଯଦି defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    ଯଦି ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_ମୂଲ୍ୟ = *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp = ମୂଲ୍ୟ;	    // set the new value
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    ଯଦି ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = ମୂଲ୍ୟ;

    // Handle side effects of changing a bool option.
    ଯଦି (options[opt_idx].opt_did_set_cb != ଖାଲି)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.boolean = old_ମୂଲ୍ୟ;
	args.os_newval.boolean = ମୂଲ୍ୟ;
	args.os_errbuf = ଖାଲି;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
	ଯଦି (errmsg != ଖାଲି)
	    ଫେରନ୍ତୁ errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_SET;

#ଯଦି defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(long)(old_ମୂଲ୍ୟ ? TRUE : FALSE),
				(long)(old_global_ମୂଲ୍ୟ ? TRUE : FALSE),
				(long)(ମୂଲ୍ୟ ? TRUE : FALSE), ଖାଲି);
#endif

    comp_col();			    // in case 'ruler' or 'showcmd' changed

    ଯଦି (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    ଯଦି ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    ସ୍ଥିର ଅକ୍ଷର *
check_ଅଙ୍କ_option_bounds(
    long	*pp,
    long	old_ମୂଲ୍ୟ,
    long	old_Rows,
    long	old_Columns,
    ଅକ୍ଷର	*errbuf,
    size_t	errbuflen,
    ଅକ୍ଷର	*errmsg)
{
    ଯଦି (Rows < min_rows_for_all_tabpages() && full_screen)
    {
	ଯଦି (errbuf != ଖାଲି)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_ଧାଡ଼ିଗୁଡ଼ିକ), min_rows_for_all_tabpages());
	    errmsg = errbuf;
	}
	Rows = min_rows_for_all_tabpages();
    }
    ଯଦି (Columns < MIN_COLUMNS && full_screen)
    {
	ଯଦି (errbuf != ଖାଲି)
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
    ଯଦି (old_Rows != Rows || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	ଯଦି (updating_screen)
	    *pp = old_ମୂଲ୍ୟ;
	ଅନ୍ୟଥା ଯଦି (full_screen
#ifdef FEAT_GUI
		&& !gui.starting
#endif
		)
	    set_shellsize((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows, TRUE);
	ଅନ୍ୟଥା
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    ଯଦି (cmdline_row > Rows - p_ch && Rows > p_ch)
		cmdline_row = Rows - p_ch;
	}
	ଯଦି (p_window >= Rows || !option_was_set((char_u *)"window"))
	    p_window = Rows - 1;
    }

    ଯଦି (p_tm < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tm = 0;
    }
    ଯଦି ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_height
		    && curwin->w_height > 0))
	    && full_screen)
    {
	ଯଦି (pp == &(curwin->w_p_scr))
	{
	    ଯଦି (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_size;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	ଅନ୍ୟଥା ଯଦି (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	ଅନ୍ୟଥା // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_height;
    }
    ଯଦି (p_hi < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hi = 0;
    }
    ଅନ୍ୟଥା ଯଦି (p_hi > 10000)
    {
	errmsg = e_invalid_argument;
	p_hi = 10000;
    }
    ଯଦି (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_argument;
	p_re = 0;
    }
    ଯଦି (p_report < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_report = 1;
    }
    ଯଦି ((p_sj < -100 || p_sj >= Rows) && full_screen)
    {
	ଯଦି (Rows != old_Rows)	// Rows changed, just adjust p_sj
	    p_sj = Rows / 2;
	ଅନ୍ୟଥା
	{
	    errmsg = e_invalid_scroll_size;
	    p_sj = 1;
	}
    }
    ଯଦି (p_so < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_so = 0;
    }
    ଯଦି (p_sop < 0 && full_screen)
    {
	errmsg = e_invalid_argument;
	p_sop = 0;
    }
    ଯଦି (p_siso < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_siso = 0;
    }
    ଯଦି (p_cwh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_cwh = 1;
    }
    ଯଦି (p_ut < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ut = 2000;
    }
    ଯଦି (p_ss < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ss = 0;
    }

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ସ୍ଥିର ଅକ୍ଷର *
set_ଅଙ୍କ_option(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    long	ମୂଲ୍ୟ,			// new value
    ଅକ୍ଷର	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    ଅକ୍ଷର	*errmsg = ଖାଲି;
    long	old_ମୂଲ୍ୟ = *(long *)varp;
#ଯଦି defined(FEAT_EVAL)
    long	old_global_ମୂଲ୍ୟ = 0;	// only used when setting a local and
					// global option
#endif
    long	old_Rows = Rows;	// remember old Rows
    long	old_Columns = Columns;	// remember old Columns
    long	*pp = (long *)varp;

    // Disallow changing some options from secure mode.
    ଯଦି ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	ଫେରନ୍ତୁ e_not_allowed_here;

#ଯଦି defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    ଯଦି ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_ମୂଲ୍ୟ = *(long *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *pp = ମୂଲ୍ୟ;
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    ଯଦି ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = ମୂଲ୍ୟ;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    ଯଦି (options[opt_idx].opt_did_set_cb != ଖାଲି)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.ସଂଖ୍ୟା = old_ମୂଲ୍ୟ;
	args.os_newval.ସଂଖ୍ୟା = ମୂଲ୍ୟ;
	args.os_errbuf = ଖାଲି;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
    }

    // Check the bounds for numeric options here
    errmsg = check_ଅଙ୍କ_option_bounds(pp, old_ମୂଲ୍ୟ, old_Rows, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_SET;

#ଯଦି defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_ମୂଲ୍ୟ, old_global_ମୂଲ୍ୟ,
								ମୂଲ୍ୟ, errmsg);
#endif

    comp_col();			    // in case 'columns' or 'ls' changed

    ଯଦି (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    ଯଦି ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    ଶୂନ୍ୟ
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		doclear = (flags & P_RCLR) == P_RCLR;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		all = ((flags & P_RALL) == P_RALL || doclear);

    ଯଦି ((flags & P_RSTAT) || all)	// mark all status lines dirty
	status_redraw_all();

    ଯଦି ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	ଯଦି (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	ଅନ୍ୟଥା
	    changed_window_setting();
    }
    ଯଦି (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    ଯଦି (doclear)
	redraw_all_later(UPD_CLEAR);
    ଅନ୍ୟଥା ଯଦି (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
findoption(char_u *arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    opt_idx;
    ଅକ୍ଷର	    *s, *p;
    ସ୍ଥିର short    quick_tab[27] = {0, 0};	// quick access table
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    is_term_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    ଯଦି (quick_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_opt_idx;
	ପାଇଁ (last_opt_idx = 0; options[last_opt_idx].fullname != ଖାଲି;
		last_opt_idx++)
	    ;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା tab_idx = 1; tab_idx < 27; tab_idx++)
	    quick_tab[tab_idx] = last_opt_idx;

	p = options[0].fullname;
	ପାଇଁ (opt_idx = 1; opt_idx < last_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    ଯଦି (s[0] != p[0])
	    {
		ଯଦି (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		ଅନ୍ୟଥା
		    quick_tab[Char_Ord_ତଳ(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    ଯଦି (arg[0] < 'a' || arg[0] > 'z')
	ଫେରନ୍ତୁ -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    ଯଦି (is_term_opt)
	opt_idx = quick_tab[26];
    ଅନ୍ୟଥା
	opt_idx = quick_tab[Char_Ord_ତଳ(arg[0])];
    ପାଇଁ (; (s = options[opt_idx].fullname) != ଖାଲି && s[0] == arg[0]; opt_idx++)
    {
	ଯଦି (STRCMP(arg, s) == 0)		    // match full name
	    ଭାଙ୍ଗ;
    }
    ଯଦି (s != ଖାଲି && s[0] != arg[0])
	s = ଖାଲି;
    ଯଦି (s == ଖାଲି && !is_term_opt)
    {
	opt_idx = quick_tab[Char_Ord_ତଳ(arg[0])];
	ପାଇଁ ( ; options[opt_idx].fullname != ଖାଲି; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    ଯଦି (s != ଖାଲି && STRCMP(arg, s) == 0)   // match short name
		ଭାଙ୍ଗ;
	    s = ଖାଲି;
	}
    }
    ଯଦି (s == ଖାଲି)
	opt_idx = -1;
    ଫେରନ୍ତୁ opt_idx;
}

#ଯଦି defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
get_option_ମୂଲ୍ୟ(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*flagsp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scope)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    ଯଦି (opt_idx < 0)		    // option not in the table
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି;

	ଯଦି (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (ଚାବି = find_ଚାବି_option(name, FALSE)) != 0)
	{
	    char_u ଚାବି_name[2];
	    char_u *p;

	    ଯଦି (flagsp != ଖାଲି)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    ଯଦି (ଚାବି < 0)
	    {
		ଚାବି_name[0] = KEY2TERMCAP0(ଚାବି);
		ଚାବି_name[1] = KEY2TERMCAP1(ଚାବି);
	    }
	    ଅନ୍ୟଥା
	    {
		ଚାବି_name[0] = KS_ଚାବି;
		ଚାବି_name[1] = (ଚାବି & 0xff);
	    }
	    p = find_termcode(ଚାବି_name);
	    ଯଦି (p != ଖାଲି)
	    {
		ଯଦି (stringval != ଖାଲି)
		    *stringval = vim_strsave(p);
		ଫେରନ୍ତୁ gov_string;
	    }
	}
	ଫେରନ୍ତୁ gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    ଯଦି (flagsp != ଖାଲି)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    ଯଦି (options[opt_idx].flags & P_STRING)
    {
	ଯଦି (varp == ଖାଲି)		    // hidden option
	    ଫେରନ୍ତୁ gov_hidden_string;
	ଯଦି (stringval != ଖାଲି)
	{
	    ଯଦି ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    ଅନ୍ୟଥା ଯଦି ((char_u **)varp == &curbuf->b_p_ଚାବି
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    ଅନ୍ୟଥା
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	ଫେରନ୍ତୁ gov_string;
    }

    ଯଦି (varp == ଖାଲି)		    // hidden option
	ଫେରନ୍ତୁ (options[opt_idx].flags & P_ଅଙ୍କ)
					 ? gov_hidden_ସଂଖ୍ୟା : gov_hidden_bool;
    ଯଦି (options[opt_idx].flags & P_ଅଙ୍କ)
	*numval = *(long *)varp;
    ଅନ୍ୟଥା
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp == &curbuf->b_changed)
	    *numval = curbufIsChanged();
	ଅନ୍ୟଥା
	    *numval = (long) *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp;
    }
    ଫେରନ୍ତୁ (options[opt_idx].flags & P_ଅଙ୍କ) ? gov_ସଂଖ୍ୟା : gov_bool;
}
#endif

#ଯଦି defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_option_ମୂଲ୍ୟ_strict(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_type,
    ଶୂନ୍ୟ	*from)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;
    char_u	*varp = ଖାଲି;
    ସଂରଚନା vimoption *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r = 0;

    opt_idx = findoption(name);
    ଯଦି (opt_idx < 0)
	ଫେରନ୍ତୁ 0;

    p = &(options[opt_idx]);

    // Hidden option
    ଯଦି (p->var == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    ଯଦି (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    ଅନ୍ୟଥା ଯଦି (p->flags & P_ଅଙ୍କ)
	r |= SOPT_ଅଙ୍କ;
    ଅନ୍ୟଥା ଯଦି (p->flags & P_STRING)
	r |= SOPT_STRING;

    ଯଦି (p->indir == PV_NONE)
    {
	ଯଦି (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ 0; // Did not request global-only option
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	ଅନ୍ୟଥା ଯଦି (opt_type == SREQ_GLOBAL)
	    ଫେରନ୍ତୁ 0; // Requested global option

	ଯଦି (p->indir & PV_WIN)
	{
	    ଯଦି (opt_type == SREQ_BUF)
		ଫେରନ୍ତୁ 0; // Did not request window-local option
	    ଅନ୍ୟଥା
		r |= SOPT_WIN;
	}
	ଅନ୍ୟଥା ଯଦି (p->indir & PV_BUF)
	{
	    ଯଦି (opt_type == SREQ_WIN)
		ଫେରନ୍ତୁ 0; // Did not request buffer-local option
	    ଅନ୍ୟଥା
		r |= SOPT_BUF;
	}
    }

    ଯଦି (stringval == ଖାଲି)
	ଫେରନ୍ତୁ r;

    ଯଦି (opt_type == SREQ_GLOBAL)
	varp = p->var;
    ଅନ୍ୟଥା
    {
	ଯଦି (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    ଯଦି (p->indir == PV_MOD)
	    {
		*numval = bufIsChanged((buf_T *)from);
		varp = ଖାଲି;
	    }
# ifdef FEAT_CRYPT
	    ଅନ୍ୟଥା ଯଦି (p->indir == PV_ଚାବି)
	    {
		// never return the value of the crypt key
		*stringval = ଖାଲି;
		varp = ଖାଲି;
	    }
# endif
	    ଅନ୍ୟଥା
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
	ଅନ୍ୟଥା ଯଦି (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_buffer;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	}
	ଯଦି (varp == p->var)
	    ଫେରନ୍ତୁ (r | SOPT_UNSET);
    }

    ଯଦି (varp != ଖାଲି)
    {
	ଯଦି (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	ଅନ୍ୟଥା ଯଦି (p->flags & P_ଅଙ୍କ)
	    *numval = *(long *) varp;
	ଅନ୍ୟଥା
	    *numval = *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp;
    }

    ଫେରନ୍ତୁ r;
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
option_iter_next(ଶୂନ୍ୟ **option, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_type)
{
    ସଂରଚନା vimoption	*ret = ଖାଲି;
    କର
    {
	ଯଦି (*option == ଖାଲି)
	    *option = (ଶୂନ୍ୟ *) options;
	ଅନ୍ୟଥା ଯଦି (((ସଂରଚନା vimoption *) (*option))->fullname == ଖାଲି)
	{
	    *option = ଖାଲି;
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	ଅନ୍ୟଥା
	    *option = (ଶୂନ୍ୟ *) (((ସଂରଚନା vimoption *) (*option)) + 1);

	ret = ((ସଂରଚନା vimoption *) (*option));

	// Hidden option
	ଯଦି (ret->var == ଖାଲି)
	{
	    ret = ଖାଲି;
	    ଜାରି;
	}

	ବାଛ (opt_type)
	{
	    ସ୍ଥିତି SREQ_GLOBAL:
		ଯଦି (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = ଖାଲି;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି SREQ_BUF:
		ଯଦି (!(ret->indir & PV_BUF))
		    ret = ଖାଲି;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି SREQ_WIN:
		ଯଦି (!(ret->indir & PV_WIN))
		    ret = ଖାଲି;
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		internal_error("option_iter_next()");
		ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଯେତେବେଳେ (ret == ଖାଲି);

    ଫେରନ୍ତୁ (char_u *)ret->fullname;
}
#endif

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx)
{
    ଫେରନ୍ତୁ options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    ଶୂନ୍ୟ
set_option_flag(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_global_option(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx)
{
    ଫେରନ୍ତୁ options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_global_local_option(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx)
{
    ଫେରନ୍ତୁ options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_window_local_option(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx)
{
    ଫେରନ୍ତୁ options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_hidden_option(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx)
{
    ଫେରନ୍ତୁ options[opt_idx].var == ଖାଲି;
}

#ଯଦି defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_crypt_ଚାବି_option(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx)
{
    ଫେରନ୍ତୁ options[opt_idx].indir == PV_ଚାବି;
}
#endif

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    ଅକ୍ଷର *
set_option_ମୂଲ୍ୟ(
    char_u	*name,
    long	ସଂଖ୍ୟା,
    char_u	*string,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags)	// OPT_LOCAL or 0 (both)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;
    char_u	*varp;
    long_u	flags;
    ସ୍ଥିର ଅକ୍ଷର	errbuf[ERR_BUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    ଯଦି (opt_idx < 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି;

	ଯଦି (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (ଚାବି = find_ଚାବି_option(name, FALSE)) != 0)
	{
	    char_u ଚାବି_name[2];

	    ଯଦି (ଚାବି < 0)
	    {
		ଚାବି_name[0] = KEY2TERMCAP0(ଚାବି);
		ଚାବି_name[1] = KEY2TERMCAP1(ଚାବି);
	    }
	    ଅନ୍ୟଥା
	    {
		ଚାବି_name[0] = KS_ଚାବି;
		ଚାବି_name[1] = (ଚାବି & 0xff);
	    }
	    add_termcode(ଚାବି_name, string, FALSE);
	    ଯଦି (full_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    ଅନ୍ୟଥା
    {
	flags = options[opt_idx].flags;
#ifdef HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	ଯଦି (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_in_sandbox));
	    ଫେରନ୍ତୁ ଖାଲି;
	}
#endif
	ଯଦି (flags & P_STRING)
	    ଫେରନ୍ତୁ set_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	ଯଦି (varp != ଖାଲି)	// hidden option is not changed
	{
	    ଯଦି (ସଂଖ୍ୟା == 0 && string != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

		// Either we are given a string or we are setting option
		// to zero.
		ପାଇଁ (idx = 0; string[idx] == '0'; ++idx)
		    ;
		ଯଦି (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_ସଂଖ୍ୟା_required_after_str_equal_str),
								 name, string);
		    ଫେରନ୍ତୁ ଖାଲି;     // do nothing as we hit an error

		}
	    }
	    ଯଦି (flags & P_ଅଙ୍କ)
	    {
		ଫେରନ୍ତୁ set_ଅଙ୍କ_option(opt_idx, varp, ସଂଖ୍ୟା,
					   errbuf, ଆକାର(errbuf), opt_flags);
	    }
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ set_bool_option(opt_idx, varp, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ସଂଖ୍ୟା, opt_flags);
	}

    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    ଶୂନ୍ୟ
set_option_ମୂଲ୍ୟ_give_err(
    char_u	*name,
    long	ସଂଖ୍ୟା,
    char_u	*string,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags)	// OPT_LOCAL or 0 (both)
{
    ଅକ୍ଷର *errmsg = set_option_ମୂଲ୍ୟ(name, ସଂଖ୍ୟା, string, opt_flags);

    ଯଦି (errmsg != ଖାଲି)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_term_code(char_u *tname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    opt_idx;
    char_u  *varp;

    ଯଦି (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	ଯଦି (varp != ଖାଲି)
	    varp = *(char_u **)(varp);
	ଫେରନ୍ତୁ varp;
    }
    ଫେରନ୍ତୁ find_termcode(tname + 2);
}

    char_u *
get_highlight_default(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    i = findoption((char_u *)"hl");
    ଯଦି (i >= 0)
	ଫେରନ୍ତୁ options[i].def_val[VI_DEFAULT];
    ଫେରନ୍ତୁ (char_u *)ଖାଲି;
}

    char_u *
get_encoding_default(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    i = findoption((char_u *)"enc");
    ଯଦି (i >= 0)
	ଫେରନ୍ତୁ options[i].def_val[VI_DEFAULT];
    ଫେରନ୍ତୁ (char_u *)ଖାଲି;
}

#ଯଦି defined(FEAT_QUICKFIX)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_option_allocated(ଅକ୍ଷର *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = findoption((char_u *)name);

    ଫେରନ୍ତୁ idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#endif

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_ଚାବି_option(char_u *arg_arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_lt)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଚାବି = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    ଯଦି (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	ଯଦି (!has_lt || arg[4] == '>')
	    ଚାବି = TERMCAP2_ଚାବି(arg[2], arg[3]);
    }
    ଅନ୍ୟଥା ଯଦି (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	ଚାବି = find_special_ଚାବି(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_ଚାବି | FSK_SIMPLIFY, ଖାଲି);
	ଯଦି (modifiers)		    // can't handle modifiers here
	    ଚାବି = 0;
    }
    ଫେରନ୍ତୁ ଚାବି;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    ସ୍ଥିର ଶୂନ୍ୟ
showoptions(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		all,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    ସଂରଚନା vimoption	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			isterm;
    char_u		*varp;
    ସଂରଚନା vimoption	**ବସ୍ତୁଗୁଡ଼ିକ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			item_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			run;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			row, rows;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			cols;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			len;

#define INC 20
#define GAP 3

    ବସ୍ତୁଗୁଡ଼ିକ = ALLOC_MULT(ସଂରଚନା vimoption *, OPTION_ଗଣନା);
    ଯଦି (ବସ୍ତୁଗୁଡ଼ିକ == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Highlight title
    ଯଦି (all == 2)
	ସନ୍ଦେଶ_puts_title(_("\n--- Terminal codes ---"));
    ଅନ୍ୟଥା ଯଦି (opt_flags & OPT_GLOBAL)
	ସନ୍ଦେଶ_puts_title(_("\n--- Global option values ---"));
    ଅନ୍ୟଥା ଯଦି (opt_flags & OPT_LOCAL)
	ସନ୍ଦେଶ_puts_title(_("\n--- Local option values ---"));
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    ପାଇଁ (run = 1; run <= 2 && !got_int; ++run)
    {
	// collect the items in items[]
	item_ଗଣନା = 0;
	ପାଇଁ (p = &options[0]; p->fullname != ଖାଲି; p++)
	{
	    // apply :filter /pat/
	    ଯଦି (message_filtered((char_u *)p->fullname))
		ଜାରି;

	    varp = ଖାଲି;
	    isterm = istermoption(p);
	    ଯଦି ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		ଯଦି (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    ଅନ୍ୟଥା
		varp = get_varp(p);
	    ଯଦି (varp != ଖାଲି
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		ଯଦି (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		ଅନ୍ୟଥା ଯଦି (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		ଅନ୍ୟଥା
		{
		    option_value2string(p, opt_flags);
		    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		ଯଦି ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    ବସ୍ତୁଗୁଡ଼ିକ[item_ଗଣନା++] = p;
	    }
	}

	// display the items
	ଯଦି (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    ଯଦି (cols == 0)
		cols = 1;
	    rows = (item_ଗଣନା + cols - 1) / cols;
	}
	ଅନ୍ୟଥା	// run == 2
	    rows = item_ଗଣନା;
	ପାଇଁ (row = 0; row < rows && !got_int; ++row)
	{
	    ସନ୍ଦେଶ_putchar('\n');			// go to next line
	    ଯଦି (got_int)			// 'q' typed in more
		ଭାଙ୍ଗ;
	    col = 0;
	    ପାଇଁ (i = row; i < item_ଗଣନା; i += rows)
	    {
		ସନ୍ଦେଶ_col = col;			// make columns
		showoneopt(ବସ୍ତୁଗୁଡ଼ିକ[i], opt_flags);
		col += INC;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(ବସ୍ତୁଗୁଡ଼ିକ);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
optval_default(ସଂରଚନା vimoption *p, char_u *varp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା compatible)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dvi;

    ଯଦି (varp == ଖାଲି)
	ଫେରନ୍ତୁ TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    ଯଦି (p->flags & P_ଅଙ୍କ)
	ଫେରନ୍ତୁ (*(long *)varp == (long)(long_i)p->def_val[dvi]);
    ଯଦି (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	ଫେରନ୍ତୁ (*(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(long)(long_i)p->def_val[dvi]);
    // P_STRING
    ଫେରନ୍ତୁ (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    ସ୍ଥିର ଶୂନ୍ୟ
showoneopt(
    ସଂରଚନା vimoption	*p,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_silent = silent_mode;

    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    ଯଦି ((p->flags & P_BOOL) && ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp == &curbuf->b_changed
					? !curbufIsChanged() : !*(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp))
	ସନ୍ଦେଶ_puts("no");
    ଅନ୍ୟଥା ଯଦି ((p->flags & P_BOOL) && *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp < 0)
	ସନ୍ଦେଶ_puts("--");
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ_puts("  ");
    ସନ୍ଦେଶ_puts(p->fullname);
    ଯଦି (!(p->flags & P_BOOL))
    {
	ସନ୍ଦେଶ_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	ସନ୍ଦେଶ_outtrans(NameBuff);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
makeset(FILE *fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା local_only)
{
    ସଂରଚନା vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = ଖାଲି;	// fresh value
    ଅକ୍ଷର		*cmd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			round;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    ପାଇଁ (pri = 1; pri >= 0; --pri)
    {
      ପାଇଁ (p = &options[0]; !istermoption(p); p++)
	ଯଦି (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    ଯଦି (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		ଜାରି;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    ଯଦି ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		ଜାରି;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    ଯଦି ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		ଜାରି;

	    ଯଦି ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		ଜାରି;

	    round = 2;
	    ଯଦି (p->indir != PV_NONE)
	    {
		ଯଦି (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    ଯଦି (!(opt_flags & OPT_LOCAL))
			ଜାରି;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    ଯଦି (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			ଯଦି (!optval_default(p, varp_fresh, p_cp))
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
	    ପାଇଁ ( ; round <= 2; varp = varp_local, ++round)
	    {
		ଯଦି (round == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		ଅନ୍ୟଥା
		    cmd = "setlocal";

		ଯଦି (p->flags & P_BOOL)
		{
		    ଯଦି (put_setbool(fd, cmd, p->fullname, *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		}
		ଅନ୍ୟଥା ଯଦି (p->flags & P_ଅଙ୍କ)
		{
		    ଯଦି (put_setnum(fd, cmd, p->fullname, (long *)varp) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		}
		ଅନ୍ୟଥା    // P_STRING
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    ଯଦି ((p->flags & P_FUNC) && *(char_u **)varp != ଖାଲି
			    && strstr(*(ଅକ୍ଷର **)varp, "<lambda>") != ଖାଲି)
			ଜାରି;

#ifdef FEAT_EVAL
		    legacy = !is_option_ମୂଲ୍ୟ_vim9(p - &options[0],
			    round == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#ଅନ୍ୟଥା
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#endif

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    ଯଦି (
#ଯଦି defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#endif
			    p->indir == PV_FT)
		    {
			ଯଦି (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    ଫେରନ୍ତୁ FAIL;
			do_endif = TRUE;
		    }
		    ଯଦି (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    ଯଦି (do_endif)
		    {
			ଯଦି (put_ଧାଡ଼ି(fd, "endif") == FAIL)
			    ଫେରନ୍ତୁ FAIL;
		    }
		}
	    }
	}
    }
    ଫେରନ୍ତୁ OK;
}

#ଯଦି defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_version < SCRIPT_VERSION_VIM9;
# endif

    ଯଦି (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}
#endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
put_setstring(
    FILE	*fd,
    bool	legacy,
    ଅକ୍ଷର	*cmd,
    ଅକ୍ଷର	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = ଖାଲି;
    char_u	*part = ଖାଲି;
    char_u	*p;

    ଯଦି (legacy && fprintf(fd, "legacy ") < 0)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (fprintf(fd, "%s %s=", cmd, name) < 0)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (*valuep != ଖାଲି)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	ଯଦି (valuep == &p_pt)
	{
	    s = *valuep;
	    ଯେତେବେଳେ (*s != NUL)
		ଯଦି (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	}
	// expand the option value, replace $HOME by ~
	ଅନ୍ୟଥା ଯଦି ((flags & P_EXPAND) != 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା  size = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(size);
	    ଯଦି (buf == ଖାଲି)
		ଯାଅ fail;
	    home_replace(ଖାଲି, *valuep, buf, size, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    ଯଦି (size >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != ଖାଲି)
	    {
		part = alloc(size);
		ଯଦି (part == ଖାଲି)
		    ଯାଅ fail;

		// write line break to clear the option, e.g. ':set rtp='
		ଯଦି (put_eol(fd) == FAIL)
		    ଯାଅ fail;

		p = buf;
		ଯେତେବେଳେ (*p != NUL)
		{
		    ଯଦି (legacy && fprintf(fd, "legacy ") < 0)
			ଫେରନ୍ତୁ FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    ଯଦି (fprintf(fd, "%s %s+=", cmd, name) < 0)
			ଯାଅ fail;
		    (ଶୂନ୍ୟ)copy_option_part(&p, part, size,  ",");
		    ଯଦି (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			ଯାଅ fail;
		}
		vim_free(buf);
		vim_free(part);
		ଫେରନ୍ତୁ OK;
	    }
	    ଯଦି (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		ଫେରନ୍ତୁ FAIL;
	    }
	    vim_free(buf);
	}
	ଅନ୍ୟଥା ଯଦି (put_escstr(fd, *valuep, 2) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (put_eol(fd) < 0)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
fail:
    vim_free(buf);
    vim_free(part);
    ଫେରନ୍ତୁ FAIL;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
put_setnum(
    FILE	*fd,
    ଅକ୍ଷର	*cmd,
    ଅକ୍ଷର	*name,
    long	*valuep)
{
    long	wc;

    ଯଦି (fprintf(fd, "%s %s=", cmd, name) < 0)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	ଯଦି (fputs((ଅକ୍ଷର *)get_special_ଚାବି_name((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wc, 0), fd) < 0)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (fprintf(fd, "%ld", *valuep) < 0)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (put_eol(fd) < 0)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
put_setbool(
    FILE	*fd,
    ଅକ୍ଷର	*cmd,
    ଅକ୍ଷର	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ମୂଲ୍ୟ)
{
    ଯଦି (ମୂଲ୍ୟ < 0)	// global/local option using global value
	ଫେରନ୍ତୁ OK;
    ଯଦି (fprintf(fd, "%s %s%s", cmd, ମୂଲ୍ୟ ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    ଶୂନ୍ୟ
clear_termoptions(ଶୂନ୍ୟ)
{
    // Reset a few things before clearing the old options. This may cause
    // outputting a few things that the terminal doesn't understand, but the
    // screen will be cleared later, so this is OK.
    mch_setmouse(FALSE);	    // switch mouse off
    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window titles
#ଯଦି defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    ଯଦି (gui.starting)
	clear_xterm_clip();
#endif
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    ଶୂନ୍ୟ
free_termoptions(ଶୂନ୍ୟ)
{
    ସଂରଚନା vimoption   *p;

    ପାଇଁ (p = options; p->fullname != ଖାଲି; p++)
	ଯଦି (istermoption(p))
	{
	    ଯଦି (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    ଯଦି (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = empty_option;
	    p->def_val[VI_DEFAULT] = empty_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ifdef FEAT_EVAL
	    // remember where the option was cleared
	    set_option_sctx_idx((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - options), OPT_GLOBAL, current_sctx);
#endif
	}
    clear_termcodes();
}

/*
 * Free the string for one term option, if it was allocated.
 * Set the string to empty_option and clear allocated flag.
 * "var" points to the option value.
 */
    ଶୂନ୍ୟ
free_one_termoption(char_u *var)
{
    ସଂରଚନା vimoption   *p;

    ପାଇଁ (p = &options[0]; p->fullname != ଖାଲି; p++)
	ଯଦି (p->var == var)
	{
	    ଯଦି (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = empty_option;
	    p->flags &= ~P_ALLOCED;
	    ଭାଙ୍ଗ;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    ଶୂନ୍ୟ
set_term_defaults(ଶୂନ୍ୟ)
{
    ସଂରଚନା vimoption   *p;

    ପାଇଁ (p = &options[0]; p->fullname != ଖାଲି; p++)
    {
	ଯଦି (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    ଯଦି (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    ଯଦି (p->flags & P_ALLOCED)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
istermoption(ସଂରଚନା vimoption *p)
{
    ଫେରନ୍ତୁ (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
istermoption_idx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx)
{
    ଫେରନ୍ତୁ istermoption(&options[opt_idx]);
}

#ଯଦି defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    ଶୂନ୍ୟ
unset_global_local_option(char_u *name, ଶୂନ୍ୟ *from)
{
    ସଂରଚନା vimoption *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    ଯଦି (opt_idx < 0)
	ଫେରନ୍ତୁ;
    p = &(options[opt_idx]);

    ବାଛ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p->indir)
    {
	// global option with local value: use local value if it's been set
	ସ୍ଥିତି PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_PATH:
	    clear_string_option(&buf->b_p_path);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_AC:
	    buf->b_p_ac = -1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_AR:
	    buf->b_p_ar = -1;
	    ଭାଙ୍ଗ;
# ifdef HAVE_FSYNC
	ସ୍ଥିତି PV_FS:
	    buf->b_p_fs = -1;
	    ଭାଙ୍ଗ;
# endif
	ସ୍ଥିତି PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_TAGS:
	    clear_string_option(&buf->b_p_tags);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_SISO:
	    curwin->w_p_siso = -1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_SO:
	    curwin->w_p_so = -1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_SOP:
	    curwin->w_p_sop = -1;
	    ଭାଙ୍ଗ;
# ifdef FEAT_FIND_ID
	ସ୍ଥିତି PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    ଭାଙ୍ଗ;
# endif
	ସ୍ଥିତି PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    ଭାଙ୍ଗ;
# ifdef FEAT_DIFF
	ସ୍ଥିତି PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    ଭାଙ୍ଗ;
# endif
	ସ୍ଥିତି PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    ଭାଙ୍ଗ;
# ifdef FEAT_COMPL_FUNC
	ସ୍ଥିତି PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    ଭାଙ୍ଗ;
# endif
	ସ୍ଥିତି PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    ଭାଙ୍ଗ;
# ifdef FEAT_EVAL
	ସ୍ଥିତି PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    ଭାଙ୍ଗ;
# endif
# ifdef FEAT_QUICKFIX
	ସ୍ଥିତି PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    ଭାଙ୍ଗ;
# endif
# ଯଦି defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	ସ୍ଥିତି PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    ଭାଙ୍ଗ;
# endif
# ଯଦି defined(FEAT_CRYPT)
	ସ୍ଥିତି PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    ଭାଙ୍ଗ;
# endif
# ifdef FEAT_LINEBREAK
	ସ୍ଥିତି PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    ଭାଙ୍ଗ;
# endif
# ifdef FEAT_STL_OPT
	ସ୍ଥିତି PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    ଭାଙ୍ଗ;
# endif
	ସ୍ଥିତି PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    set_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      ଖାଲି, 0);
	    redraw_later(UPD_NOT_VALID);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    set_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      ଖାଲି, 0);
	    redraw_later(UPD_NOT_VALID);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    ଭାଙ୍ଗ;
    }
}
#endif

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    ସ୍ଥିର char_u *
get_varp_scope(ସଂରଚନା vimoption *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା scope)
{
    ଯଦି ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	ଯଦି (p->var == VAR_WIN)
	    ଫେରନ୍ତୁ (char_u *)GLOBAL_WO(get_varp(p));
	ଫେରନ୍ତୁ p->var;
    }
    ଯଦି ((scope & OPT_LOCAL) && ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p->indir & PV_BOTH))
    {
	ବାଛ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p->indir)
	{
	    ସ୍ଥିତି PV_FP:   ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_fp);
#ifdef HAVE_FSYNC
	    ସ୍ଥିତି PV_FS:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_fs);
#endif
#ifdef FEAT_EVAL
	    ସ୍ଥିତି PV_FFU: ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ffu);
#endif
#ifdef FEAT_QUICKFIX
	    ସ୍ଥିତି PV_EFM:  ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_efm);
	    ସ୍ଥିତି PV_GEFM: ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_gefm);
	    ସ୍ଥିତି PV_GP:   ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_gp);
	    ସ୍ଥିତି PV_MP:   ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_mp);
#endif
	    ସ୍ଥିତି PV_EP:   ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ep);
	    ସ୍ଥିତି PV_KP:   ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_kp);
	    ସ୍ଥିତି PV_PATH: ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_path);
	    ସ୍ଥିତି PV_AC:   ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ac);
	    ସ୍ଥିତି PV_AR:   ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ar);
	    ସ୍ଥିତି PV_TAGS: ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_tags);
	    ସ୍ଥିତି PV_TC:   ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_tc);
	    ସ୍ଥିତି PV_SISO: ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_siso);
	    ସ୍ଥିତି PV_SO:   ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_so);
	    ସ୍ଥିତି PV_SOP:  ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_sop);
#ifdef FEAT_FIND_ID
	    ସ୍ଥିତି PV_DEF:  ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_def);
	    ସ୍ଥିତି PV_INC:  ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_inc);
#endif
	    ସ୍ଥିତି PV_COT:  ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_cot);
	    ସ୍ଥିତି PV_DICT: ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_dict);
#ifdef FEAT_DIFF
	    ସ୍ଥିତି PV_DIA:  ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_dia);
#endif
	    ସ୍ଥିତି PV_TSR:  ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_tsr);
#ifdef FEAT_COMPL_FUNC
	    ସ୍ଥିତି PV_TSRFU: ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_tsrfu);
#endif
#ଯଦି defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    ସ୍ଥିତି PV_BEXPR: ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_bexpr);
#endif
#ଯଦି defined(FEAT_CRYPT)
	    ସ୍ଥିତି PV_CM:	  ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_cm);
#endif
#ifdef FEAT_LINEBREAK
	    ସ୍ଥିତି PV_SBR:  ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_sbr);
#endif
#ifdef FEAT_STL_OPT
	    ସ୍ଥିତି PV_STL:  ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_stl);
	    ସ୍ଥିତି PV_STLO: ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_stlo);
#endif
	    ସ୍ଥିତି PV_UL:   ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ul);
	    ସ୍ଥିତି PV_LW:   ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_lw);
	    ସ୍ଥିତି PV_BKC:  ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_bkc);
	    ସ୍ଥିତି PV_MENC: ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_menc);
	    ସ୍ଥିତି PV_LCS:  ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_lcs);
	    ସ୍ଥିତି PV_FCS:  ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_fcs);
	    ସ୍ଥିତି PV_VE:	  ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_ve);

	}
	ଫେରନ୍ତୁ ଖାଲି; // "cannot happen"
    }
    ଫେରନ୍ତୁ get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା scope)
{
    ଫେରନ୍ତୁ get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    ସ୍ଥିର char_u *
get_varp(ସଂରଚନା vimoption *p)
{
    // hidden option, always return NULL
    ଯଦି (p->var == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ବାଛ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p->indir)
    {
	ସ୍ଥିତି PV_NONE:	ଫେରନ୍ତୁ p->var;

	// global option with local value: use local value if it's been set
	ସ୍ଥିତି PV_EP:	ଫେରନ୍ତୁ *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	ସ୍ଥିତି PV_KP:	ଫେରନ୍ତୁ *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	ସ୍ଥିତି PV_PATH:	ଫେରନ୍ତୁ *curbuf->b_p_path != NUL
				    ? (char_u *)&(curbuf->b_p_path) : p->var;
	ସ୍ଥିତି PV_AC:	ଫେରନ୍ତୁ curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	ସ୍ଥିତି PV_AR:	ଫେରନ୍ତୁ curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	ସ୍ଥିତି PV_TAGS:	ଫେରନ୍ତୁ *curbuf->b_p_tags != NUL
				    ? (char_u *)&(curbuf->b_p_tags) : p->var;
	ସ୍ଥିତି PV_TC:	ଫେରନ୍ତୁ *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	ସ୍ଥିତି PV_BKC:	ଫେରନ୍ତୁ *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	ସ୍ଥିତି PV_SISO:	ଫେରନ୍ତୁ curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	ସ୍ଥିତି PV_SO:	ଫେରନ୍ତୁ curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	ସ୍ଥିତି PV_SOP:	ଫେରନ୍ତୁ curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ifdef FEAT_FIND_ID
	ସ୍ଥିତି PV_DEF:	ଫେରନ୍ତୁ *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	ସ୍ଥିତି PV_INC:	ଫେରନ୍ତୁ *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#endif
	ସ୍ଥିତି PV_COT:	ଫେରନ୍ତୁ *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	ସ୍ଥିତି PV_DICT:	ଫେରନ୍ତୁ *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ifdef FEAT_DIFF
	ସ୍ଥିତି PV_DIA:	ଫେରନ୍ତୁ *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#endif
	ସ୍ଥିତି PV_TSR:	ଫେରନ୍ତୁ *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ifdef FEAT_COMPL_FUNC
	ସ୍ଥିତି PV_TSRFU:	ଫେରନ୍ତୁ *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#endif
	ସ୍ଥିତି PV_FP:	ଫେରନ୍ତୁ *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ifdef HAVE_FSYNC
	ସ୍ଥିତି PV_FS:	ଫେରନ୍ତୁ curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#endif
#ifdef FEAT_EVAL
	ସ୍ଥିତି PV_FFU:	ଫେରନ୍ତୁ *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#endif
#ifdef FEAT_QUICKFIX
	ସ୍ଥିତି PV_EFM:	ଫେରନ୍ତୁ *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	ସ୍ଥିତି PV_GEFM:	ଫେରନ୍ତୁ *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	ସ୍ଥିତି PV_GP:	ଫେରନ୍ତୁ *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	ସ୍ଥିତି PV_MP:	ଫେରନ୍ତୁ *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#endif
#ଯଦି defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	ସ୍ଥିତି PV_BEXPR:	ଫେରନ୍ତୁ *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#endif
#ଯଦି defined(FEAT_CRYPT)
	ସ୍ଥିତି PV_CM:	ଫେରନ୍ତୁ *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#endif
#ifdef FEAT_LINEBREAK
	ସ୍ଥିତି PV_SBR:	ଫେରନ୍ତୁ *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#endif
#ifdef FEAT_STL_OPT
	ସ୍ଥିତି PV_STL:	ଫେରନ୍ତୁ *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	ସ୍ଥିତି PV_STLO:	ଫେରନ୍ତୁ *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#endif
	ସ୍ଥିତି PV_UL:	ଫେରନ୍ତୁ curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	ସ୍ଥିତି PV_LW:	ଫେରନ୍ତୁ *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	ସ୍ଥିତି PV_MENC:	ଫେରନ୍ତୁ *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ifdef FEAT_ARABIC
	ସ୍ଥିତି PV_ARAB:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_arab);
#endif
	ସ୍ଥିତି PV_LIST:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_list);
	ସ୍ଥିତି PV_LCS:	ଫେରନ୍ତୁ *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	ସ୍ଥିତି PV_FCS:	ଫେରନ୍ତୁ *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	ସ୍ଥିତି PV_VE:	ଫେରନ୍ତୁ *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ifdef FEAT_SPELL
	ସ୍ଥିତି PV_SPELL:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_spell);
#endif
#ifdef FEAT_SYN_HL
	ସ୍ଥିତି PV_CUC:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_cuc);
	ସ୍ଥିତି PV_CUL:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_cul);
	ସ୍ଥିତି PV_CULOPT:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_culopt);
	ସ୍ଥିତି PV_CC:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_cc);
#endif
#ifdef FEAT_DIFF
	ସ୍ଥିତି PV_DIFF:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_diff);
#endif
	ସ୍ଥିତି PV_EIW:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_eiw);
#ifdef FEAT_FOLDING
	ସ୍ଥିତି PV_FDC:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_fdc);
	ସ୍ଥିତି PV_FEN:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_fen);
	ସ୍ଥିତି PV_FDI:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_fdi);
	ସ୍ଥିତି PV_FDL:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_fdl);
	ସ୍ଥିତି PV_FDM:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_fdm);
	ସ୍ଥିତି PV_FML:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_fml);
	ସ୍ଥିତି PV_FDN:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	ସ୍ଥିତି PV_FDE:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_fde);
	ସ୍ଥିତି PV_FDT:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_fdt);
# endif
	ସ୍ଥିତି PV_FMR:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_fmr);
#endif
	ସ୍ଥିତି PV_NU:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_nu);
	ସ୍ଥିତି PV_RNU:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_rnu);
#ifdef FEAT_LINEBREAK
	ସ୍ଥିତି PV_NUW:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_nuw);
#endif
	ସ୍ଥିତି PV_WFB:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_wfb);
	ସ୍ଥିତି PV_WFH:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_wfh);
	ସ୍ଥିତି PV_WFW:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_wfw);
#ଯଦି defined(FEAT_QUICKFIX)
	ସ୍ଥିତି PV_PVW:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_pvw);
	ସ୍ଥିତି PV_LHI:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_lhi);
#endif
#ifdef FEAT_RIGHTLEFT
	ସ୍ଥିତି PV_RL:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_rl);
	ସ୍ଥିତି PV_RLC:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_rlc);
#endif
	ସ୍ଥିତି PV_SCROLL:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_scr);
	ସ୍ଥିତି PV_SMS:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_sms);
	ସ୍ଥିତି PV_WRAP:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_wrap);
#ifdef FEAT_LINEBREAK
	ସ୍ଥିତି PV_LBR:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_lbr);
	ସ୍ଥିତି PV_BRI:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_bri);
	ସ୍ଥିତି PV_BRIOPT: ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_briopt);
#endif
	ସ୍ଥିତି PV_WCR:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_wcr);
	ସ୍ଥିତି PV_SCBIND: ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_scb);
	ସ୍ଥିତି PV_CRBIND: ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_crb);
#ifdef FEAT_CONCEAL
	ସ୍ଥିତି PV_COCU:   ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_cocu);
	ସ୍ଥିତି PV_COLE:   ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_cole);
#endif
	ସ୍ଥିତି PV_WHL:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_whl);

#ifdef FEAT_TERMINAL
	ସ୍ଥିତି PV_TWK:    ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_twk);
	ସ୍ଥିତି PV_TWS:    ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_tws);
	ସ୍ଥିତି PV_TWSL:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_twsl);
#endif

	ସ୍ଥିତି PV_AI:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ai);
	ସ୍ଥିତି PV_BIN:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_bin);
	ସ୍ଥିତି PV_BOMB:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_bomb);
	ସ୍ଥିତି PV_BH:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_bh);
	ସ୍ଥିତି PV_BT:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_bt);
	ସ୍ଥିତି PV_BL:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_bl);
	ସ୍ଥିତି PV_CI:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ci);
	ସ୍ଥିତି PV_CIN:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_cin);
	ସ୍ଥିତି PV_CINK:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_cink);
	ସ୍ଥିତି PV_CINO:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_cino);
	ସ୍ଥିତି PV_CINSD:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_cinsd);
	ସ୍ଥିତି PV_CINW:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_cinw);
	ସ୍ଥିତି PV_COM:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_com);
	ସ୍ଥିତି PV_CMS:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_cms);
	ସ୍ଥିତି PV_CPT:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_cpt);
#ifdef BACKSLASH_IN_FILENAME
	ସ୍ଥିତି PV_CSL:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_csl);
#endif
#ifdef FEAT_COMPL_FUNC
	ସ୍ଥିତି PV_CFU:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_cfu);
	ସ୍ଥିତି PV_OFU:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ofu);
#endif
#ifdef FEAT_EVAL
	ସ୍ଥିତି PV_TFU:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_tfu);
#endif
	ସ୍ଥିତି PV_EOF:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_eof);
	ସ୍ଥିତି PV_EOL:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_eol);
	ସ୍ଥିତି PV_FIXEOL:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_fixeol);
	ସ୍ଥିତି PV_ET:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_et);
	ସ୍ଥିତି PV_FENC:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_fenc);
	ସ୍ଥିତି PV_FF:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ff);
	ସ୍ଥିତି PV_FT:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ft);
	ସ୍ଥିତି PV_FO:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_fo);
	ସ୍ଥିତି PV_FLP:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_flp);
	ସ୍ଥିତି PV_IMI:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_iminsert);
	ସ୍ଥିତି PV_IMS:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_imsearch);
	ସ୍ଥିତି PV_INF:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_inf);
	ସ୍ଥିତି PV_ISK:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_isk);
#ifdef FEAT_FIND_ID
# ifdef FEAT_EVAL
	ସ୍ଥିତି PV_INEX:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_inex);
# endif
#endif
#ଯଦି defined(FEAT_EVAL)
	ସ୍ଥିତି PV_INDE:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_inde);
	ସ୍ଥିତି PV_INDK:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_indk);
#endif
#ifdef FEAT_EVAL
	ସ୍ଥିତି PV_FEX:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
	ସ୍ଥିତି PV_ଚାବି:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ଚାବି);
#endif
	ସ୍ଥିତି PV_LISP:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_lisp);
	ସ୍ଥିତି PV_LOP:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_lop);
	ସ୍ଥିତି PV_ML:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ml);
	ସ୍ଥିତି PV_MPS:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_mps);
	ସ୍ଥିତି PV_MA:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ma);
	ସ୍ଥିତି PV_MOD:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_changed);
	ସ୍ଥିତି PV_NF:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_nf);
	ସ୍ଥିତି PV_PI:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_pi);
	ସ୍ଥିତି PV_QE:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_qe);
	ସ୍ଥିତି PV_RO:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ro);
	ସ୍ଥିତି PV_SI:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_si);
	ସ୍ଥିତି PV_SN:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_sn);
	ସ୍ଥିତି PV_STS:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_sts);
	ସ୍ଥିତି PV_SUA:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_sua);
	ସ୍ଥିତି PV_SWF:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_swf);
#ifdef FEAT_SYN_HL
	ସ୍ଥିତି PV_SMC:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_smc);
	ସ୍ଥିତି PV_SYN:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_syn);
#endif
#ifdef FEAT_SPELL
	ସ୍ଥିତି PV_SPC:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_s->b_p_spc);
	ସ୍ଥିତି PV_SPF:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_s->b_p_spf);
	ସ୍ଥିତି PV_SPL:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_s->b_p_spl);
	ସ୍ଥିତି PV_SPO:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_s->b_p_spo);
#endif
	ସ୍ଥିତି PV_SW:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_sw);
	ସ୍ଥିତି PV_TS:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_ts);
	ସ୍ଥିତି PV_TW:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_tw);
	ସ୍ଥିତି PV_TX:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_tx);
#ifdef FEAT_PERSISTENT_UNDO
	ସ୍ଥିତି PV_UDF:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_udf);
#endif
	ସ୍ଥିତି PV_WM:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_wm);
#ifdef FEAT_KEYMAP
	ସ୍ଥିତି PV_KMAP:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_keymap);
#endif
#ifdef FEAT_SIGNS
	ସ୍ଥିତି PV_SCL:	ଫେରନ୍ତୁ (char_u *)&(curwin->w_p_scl);
#endif
#ifdef FEAT_VARTABS
	ସ୍ଥିତି PV_VSTS:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_vsts);
	ସ୍ଥିତି PV_VTS:	ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_vts);
#endif
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    ଫେରନ୍ତୁ (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx)
{
    ଫେରନ୍ତୁ options[opt_idx].var;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx)
{
    ଫେରନ୍ତୁ (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
option_info(char_u *name, dict_T *d)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			opt_idx;
    ସଂରଚନା vimoption	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    ଯଦି ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    ଯଦି (opt_idx < 0)
	ଫେରନ୍ତୁ FAIL;

    p = &options[opt_idx];
    dict_add_string(d, "name", (char_u *)p->fullname);
    dict_add_string(d, "shortname",
		    (char_u *)(p->shortname == ଖାଲି ? "" : p->shortname));
    dict_add_bool(d, "available", p->var != ଖାଲି);
    dict_add_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_ଅଙ୍କ) ? "number" : "string"));
    dict_add_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    ଯଦି (p->flags & P_BOOL)
	dict_add_bool(d, "default", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(long_i)p->def_val[dvi]);
    ଅନ୍ୟଥା ଯଦି (p->flags & P_ଅଙ୍କ)
	dict_add_ସଂଖ୍ୟା(d, "default", (long)(long_i)p->def_val[dvi]);
    ଅନ୍ୟଥା
	dict_add_string(d, "default", p->def_val[dvi] == ଖାଲି
					? (char_u *)"" : p->def_val[dvi]);
    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_set_cb_T
get_option_did_set_cb(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx)
{
    ଫେରନ୍ତୁ options[opt_idx].opt_did_set_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(ଶୂନ୍ୟ)
{
    ଯଦି (*curbuf->b_p_ep == NUL)
	ଫେରନ୍ତୁ p_ep;
    ଫେରନ୍ତୁ curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(ଶୂନ୍ୟ)
{
#ifdef FEAT_EVAL
    ଯଦି (*curbuf->b_p_ffu == NUL)
	ଫେରନ୍ତୁ p_ffu;
    ଫେରନ୍ତୁ curbuf->b_p_ffu;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ (char_u *)"";
#endif
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
after_copy_winopt(win_T *wp)
{
    ଅକ୍ଷର *errmsg = update_winhighlight(wp, wp->w_p_whl);

    ଯଦି (errmsg != ଖାଲି)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    ଯଦି (wp->w_p_wrap)
	wp->w_leftcol = 0;
    ଅନ୍ୟଥା
	wp->w_skipcol = 0;
#ifdef FEAT_LINEBREAK
    briopt_check(ଖାଲି, wp);
#endif
#ifdef FEAT_SYN_HL
    fill_culopt_flags(ଖାଲି, wp);
    check_colorcolumn(ଖାଲି, wp);
#endif
    set_listchars_option(wp, wp->w_p_lcs, TRUE, ଖାଲି, 0);
    set_fillchars_option(wp, wp->w_p_fcs, TRUE, ଖାଲି, 0);
}

    ସ୍ଥିର char_u *
copy_option_val(char_u *val)
{
    ଯଦି (val == empty_option)
	ଫେରନ୍ତୁ empty_option;  // no need to allocate memory
    ଫେରନ୍ତୁ vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    ଶୂନ୍ୟ
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
						    ଆକାର(to->wo_script_ctx));
#endif
    check_winopt(to);		// don't want NULL pointers
}

/*
 * Check string options in a window for a NULL value.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
check_win_options(win_T *win)
{
    check_winopt(&win->w_onebuf_opt);
    check_winopt(&win->w_allbuf_opt);
}

/*
 * Check for NULL pointers in a winopt_T and replace them with empty_option.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା buf_opt_idx[BV_ଗଣନା];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_buf_opt_idx(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_init_buf_opt_idx = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (did_init_buf_opt_idx)
	ଫେରନ୍ତୁ;
    did_init_buf_opt_idx = TRUE;
    ପାଇଁ (i = 0; !istermoption_idx(i); i++)
	ଯଦି (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_MASK] = i;
}
#ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
buf_copy_options(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		should_copy = TRUE;
    char_u	*save_p_isk = ଖାଲି;	    // init for GCC
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dont_do_help;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    ଯଦି (p_cpo != ଖାଲି)
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
	ଯଦି ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == ଖାଲି || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != ଖାଲି)))
	    should_copy = FALSE;

	ଯଦି (should_copy || (flags & BCO_ALWAYS))
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
	    ଯଦି (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = ଖାଲି;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    ଯଦି (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		ବାଛ (*p_ffs)
		{
		    ସ୍ଥିତି 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); ଭାଙ୍ଗ;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		ଯଦି (buf->b_p_ff != ଖାଲି)
		    buf->b_start_ffc = *buf->b_p_ff;
		buf->b_p_bh = empty_option;
		buf->b_p_bt = empty_option;
	    }
	    ଅନ୍ୟଥା
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
	    ଯଦି (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    ଅନ୍ୟଥା
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
	    ଯଦି (p_vsts && p_vsts != empty_option)
		(ଶୂନ୍ୟ)tabstop_set(p_vsts, &buf->b_p_vsts_array);
	    ଅନ୍ୟଥା
		buf->b_p_vsts_array = ଖାଲି;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : ଖାଲି;
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
	    (ଶୂନ୍ୟ)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#endif
#ଯଦି defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#endif
	    buf->b_p_fp = empty_option;
#ଯଦି defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#endif
#ifdef FEAT_CRYPT
	    buf->b_p_ଚାବି = vim_strsave(p_ଚାବି);
	    COPY_OPT_SCTX(buf, BV_ଚାବି);
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
#ଯଦି defined(FEAT_EVAL)
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
#ଯଦି defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = empty_option;
#endif
#ଯଦି defined(FEAT_CRYPT)
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
	    ଯଦି (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ifdef FEAT_VARTABS
		ଯଦି (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (ଶୂନ୍ୟ)tabstop_set(p_vts, &buf->b_p_vts_array);
		ଅନ୍ୟଥା
		    buf->b_p_vts_array = ଖାଲି;
#endif
	    }
	    ଅନ୍ୟଥା
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#ifdef FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		ଯଦି (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (ଶୂନ୍ୟ)tabstop_set(p_vts, &buf->b_p_vts_array);
		ଅନ୍ୟଥା
		    buf->b_p_vts_array = ଖାଲି;
#endif
		buf->b_help = false;
		ଯଦି (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	ଯଦି (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    ଯଦି (did_isk)
	(ଶୂନ୍ୟ)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    ଶୂନ୍ୟ
reset_modifiable(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    ଯଦି (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}

/*
 * Set the global value for 'iminsert' to the local value.
 */
    ଶୂନ୍ୟ
set_iminsert_global(ଶୂନ୍ୟ)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    ଶୂନ୍ୟ
set_imsearch_global(ଶୂନ୍ୟ)
{
    p_imsearch = curbuf->b_p_imsearch;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand_option_idx = -1;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand_option_start_col = 0;
ସ୍ଥିର char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand_option_flags = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand_option_append = FALSE;

    ଶୂନ୍ୟ
set_context_in_set_cmd(
    expand_T	*xp,
    char_u	*arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nextchar;
    long_u	flags = 0;	// init for GCC
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_term_option = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଚାବି;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    ଯଦି (*arg == NUL)
    {
	xp->xp_pattern = arg;
	ଫେରନ୍ତୁ;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    ଯଦି (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_pattern = p + 1;
	ଫେରନ୍ତୁ;
    }
    ଯେତେବେଳେ (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	ଯଦି (*p == ' ' || *p == ',')
	{
	    ଯେତେବେଳେ (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	ଯଦି (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    ଭାଙ୍ଗ;
	}
	--p;
    }
    ଯଦି (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_NO;
	p += 2;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_INV;
	p += 3;
    }
    xp->xp_pattern = arg = p;
    ଯଦି (*arg == '<')
    {
	ଯେତେବେଳେ (*p != '>')
	    ଯଦି (*p++ == NUL)	    // expand terminal option name
		ଫେରନ୍ତୁ;
	ଚାବି = get_special_ଚାବି_code(arg + 1);
	ଯଦି (ଚାବି == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    ଫେରନ୍ତୁ;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(ଚାବି);
	expand_option_name[3] = KEY2TERMCAP1(ଚାବି);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    ଯଦି (*p != NUL)
		++p;
	    ଯଦି (*p == NUL)
		ଫେରନ୍ତୁ;		// expand option name
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	ଅନ୍ୟଥା
	{
	    // Allow * wildcard
	    ଯେତେବେଳେ (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    ଯଦି (*p == NUL)
		ଫେରନ୍ତୁ;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    ଯଦି (opt_idx == -1 || options[opt_idx].var == ଖାଲି)
	    {
		xp->xp_context = EXPAND_NOTHING;
		ଫେରନ୍ତୁ;
	    }
	    flags = options[opt_idx].flags;
	    ଯଦି (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		ଫେରନ୍ତୁ;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand_option_subtract = FALSE;
    ଯଦି ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	ଯଦି (nextchar == '-')
	    expand_option_subtract = TRUE;
	ଯଦି (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    ଯଦି ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ଫେରନ୍ତୁ;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    ଯଦି (is_term_option)
	expand_option_idx = -1;
    ଅନ୍ୟଥା
	expand_option_idx = opt_idx;

    ଯଦି (!is_term_option)
    {
	ଯଦି (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    ଫେରନ୍ତୁ;
	}
    }

    xp->xp_pattern = p + 1;
    expand_option_start_col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p + 1 - xp->xp_ଧାଡ଼ି);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ifdef FEAT_SYN_HL
    ଯଦି (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	ଫେରନ୍ତୁ;
    }
#endif
    ଯଦି (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	ଫେରନ୍ତୁ;
    }
#ifdef FEAT_KEYMAP
    ଯଦି (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	ଫେରନ୍ତୁ;
    }
#endif

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    ଯଦି (expand_option_subtract)
    {
	xp->xp_context = EXPAND_SETTING_SUBTRACT;
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା ଯଦି (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != ଖାଲି)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    ଅନ୍ୟଥା ଯଦି (*xp->xp_pattern == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା
	xp->xp_context = EXPAND_NOTHING;

    ଯଦି (is_term_option || (flags & P_ଅଙ୍କ))
	ଫେରନ୍ତୁ;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    ଯଦି (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	ଯଦି (p == (char_u *)&p_bdir
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
	    ଯଦି (p == (char_u *)&p_path || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    ଅନ୍ୟଥା
		xp->xp_backslash = XP_BS_ONE;
	}
	ଅନ୍ୟଥା
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    ଯଦି (p == (char_u *)&p_tags)
		xp->xp_backslash = XP_BS_THREE;
	    ଅନ୍ୟଥା
		xp->xp_backslash = XP_BS_ONE;
	}
	ଯଦି (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    ଯଦି ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	ପାଇଁ (p = argend - 1; p >= xp->xp_pattern; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    ଯଦି (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		ଯେତେବେଳେ (s > xp->xp_pattern && *(s - 1) == '\\')
		    --s;
		ଯଦି ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#ଯଦି defined(BACKSLASH_IN_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#ଅନ୍ୟଥା
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#endif
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_pattern = p + 1;
		    ଭାଙ୍ଗ;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    ଯଦି (flags & P_FLAGLIST)
	xp->xp_pattern = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ifdef FEAT_SPELL
    ଯଦି (options[opt_idx].var == (char_u *)&p_sps)
    {
	ଯଦି (STRNCMP(xp->xp_pattern, "file:", 5) == 0)
	{
	    xp->xp_pattern += 5;
	    ଫେରନ୍ତୁ;
	}
	ଅନ୍ୟଥା ଯଦି (options[expand_option_idx].opt_expand_cb != ଖାଲି)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		test_only,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    ଯଦି (!fuzzy)
    {
	ଯଦି (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    ଯଦି (!test_only)
		matches[idx] = vim_strsave(str);
	    ଫେରନ୍ତୁ TRUE;
	}
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା score;

	score = fuzzy_match_str(str, fuzzystr);
	ଯଦି (score != FUZZY_SCORE_NONE)
	{
	    ଯଦି (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    ଫେରନ୍ତୁ TRUE;
	}
    }

    ଫେରନ୍ତୁ FALSE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches,
    char_u	***matches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		can_fuzzy)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_normal = 0;	    // Nr of matching non-term-code settings
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_term = 0;	    // Nr of matching terminal code settings
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    char_u	*str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		loop;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_term_opt;
    char_u	name_buf[MAX_ଚାବି_NAME_LEN];
    ସ୍ଥିର ଅକ୍ଷର *(names[]) = {"all", "termcap"};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic = regmatch->rm_ic;	// remember the ignore-case flag
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fuzzy;
    fuzmatch_str_T  *fuzmatch = ଖାଲି;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    ପାଇଁ (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	ଯଦି (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    ପାଇଁ (match = 0; match < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(names); ++match)
	    {
		ଯଦି (match_str((char_u *)names[match], regmatch, *matches,
			    ଗଣନା_2, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    ଯଦି (loop == 0)
			ଅଙ୍କ_normal++;
		    ଅନ୍ୟଥା
			ଗଣନା_2++;
		}
	    }
	}
	ପାଇଁ (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != ଖାଲି;
								    opt_idx++)
	{
	    ଯଦି (options[opt_idx].var == ଖାଲି)
		ଜାରି;
	    ଯଦି (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		ଜାରି;
	    is_term_opt = istermoption_idx(opt_idx);
	    ଯଦି (is_term_opt && ଅଙ୍କ_normal > 0)
		ଜାରି;

	    ଯଦି (match_str(str, regmatch, *matches, ଗଣନା_2, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		ଯଦି (loop == 0)
		{
		    ଯଦି (is_term_opt)
			ଅଙ୍କ_term++;
		    ଅନ୍ୟଥା
			ଅଙ୍କ_normal++;
		}
		ଅନ୍ୟଥା
		    ଗଣନା_2++;
	    }
	    ଅନ୍ୟଥା ଯଦି (!fuzzy && options[opt_idx].shortname != ଖାଲି
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		ଯଦି (loop == 0)
		{
		    ଯଦି (is_term_opt)
			ଅଙ୍କ_term++;
		    ଅନ୍ୟଥା
			ଅଙ୍କ_normal++;
		}
		ଅନ୍ୟଥା
		    (*matches)[ଗଣନା_2++] = vim_strsave(str);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		ଯଦି (match_str(name_buf, regmatch, *matches, ଗଣନା_2, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    ଯଦି (loop == 0)
			ଅଙ୍କ_term++;
		    ଅନ୍ୟଥା
			ଗଣନା_2++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	ଯଦି (xp->xp_context != EXPAND_BOOL_SETTINGS  && ଅଙ୍କ_normal == 0)
	{
	    ପାଇଁ (opt_idx = 0; (str = get_termcode(opt_idx)) != ଖାଲି; opt_idx++)
	    {
		ଯଦି (!SAFE_isprint(str[0]) || !SAFE_isprint(str[1]))
		    ଜାରି;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		ଯଦି (match_str(name_buf, regmatch, *matches, ଗଣନା_2,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    ଯଦି (loop == 0)
			ଅଙ୍କ_term++;
		    ଅନ୍ୟଥା
			ଗଣନା_2++;
		}
		ଅନ୍ୟଥା
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    ଯଦି (match_str(name_buf, regmatch, *matches, ଗଣନା_2,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			ଯଦି (loop == 0)
			    ଅଙ୍କ_term++;
			ଅନ୍ୟଥା
			    ଗଣନା_2++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    ପାଇଁ (opt_idx = 0; (str = get_ଚାବି_name(opt_idx)) != ଖାଲି; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		ଯଦି (match_str(name_buf, regmatch, *matches, ଗଣନା_2, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    ଯଦି (loop == 0)
			ଅଙ୍କ_term++;
		    ଅନ୍ୟଥା
			ଗଣନା_2++;
		}
	    }
	}
	ଯଦି (loop == 0)
	{
	    ଯଦି (ଅଙ୍କ_normal > 0)
		*ଅଙ୍କ_Matches = ଅଙ୍କ_normal;
	    ଅନ୍ୟଥା ଯଦି (ଅଙ୍କ_term > 0)
		*ଅଙ୍କ_Matches = ଅଙ୍କ_term;
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ OK;
	    ଯଦି (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *ଅଙ୍କ_Matches);
		ଯଦି (*matches == ଖାଲି)
		{
		    *matches = (char_u **)"";
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *ଅଙ୍କ_Matches);
		ଯଦି (fuzmatch == ଖାଲି)
		{
		    *matches = (char_u **)"";
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	}
    }

    ଯଦି (fuzzy &&
	fuzzymatches_to_strmatches(fuzmatch, matches, ଗଣନା_2, FALSE) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

// Escape an option value that can be used on the command-line with :set.
// Caller needs to free the returned string, unless NULL is returned.
    ସ୍ଥିର char_u*
escape_option_str_cmdline(char_u *var)
{
    char_u *buf;

    // A backslash is required before some characters.  This is the reverse of
    // what happens in do_set().
    buf = vim_strsave_escaped(var, escape_chars);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

#ifdef BACKSLASH_IN_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    ପାଇଁ (var = buf; *var != NUL; MB_PTR_ADV(var))
	ଯଦି (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#endif
    ଫେରନ୍ତୁ buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandOldSetting(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    char_u  *var = ଖାଲି;	// init for GCC
    char_u  *buf;

    *ଅଙ୍କ_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    ଯଦି (*matches == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // For a terminal key code expand_option_idx is < 0.
    ଯଦି (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	ଯଦି (var == ଖାଲି)
	    expand_option_idx = findoption(expand_option_name);
    }

    ଯଦି (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    ଅନ୍ୟଥା ଯଦି (var == ଖାଲି)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    ଯଦି (buf == ଖାଲି)
    {
	VIM_CLEAR(*matches);
	ଫେରନ୍ତୁ FAIL;
    }

    (*matches)[0] = buf;
    *ଅଙ୍କ_Matches = 1;
    ଫେରନ୍ତୁ OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches,
    char_u	***matches)
{
    char_u  *var = ଖାଲି;	// init for GCC
    char_u  *buf;

    ଯଦି (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == ଖାଲି)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	ଫେରନ୍ତୁ FAIL;
    }

    optexpand_T args;
    args.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    args.oe_append = expand_option_append;
    args.oe_regmatch = regmatch;
    args.oe_xp = xp;
    args.oe_set_arg = xp->xp_ଧାଡ଼ି + expand_option_start_col;
    args.oe_include_orig_val =
	!expand_option_append &&
	(*args.oe_set_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    args.oe_opt_ମୂଲ୍ୟ = buf;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ret = options[expand_option_idx].opt_expand_cb(&args, ଅଙ୍କ_Matches, matches);

    vim_free(buf);
    ଫେରନ୍ତୁ ଅଙ୍କ_ret;
}

/*
 * Expansion handler for :set-=
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandSettingSubtract(
    expand_T	*xp,
    regmatch_T	*regmatch,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches,
    char_u	***matches)
{
    ଯଦି (expand_option_idx < 0)
	// term option
	ଫେରନ୍ତୁ ExpandOldSetting(ଅଙ୍କ_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    ଯଦି (option_flags & P_ଅଙ୍କ)
	ଫେରନ୍ତୁ ExpandOldSetting(ଅଙ୍କ_Matches, matches);
    ଅନ୍ୟଥା ଯଦି (option_flags & P_COMMA)
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

	ଯଦି (*option_val == NUL)
	    ଫେରନ୍ତୁ FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	ଯଦି (option_copy == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	next_val = option_copy;

	ga_init2(&ga, ଆକାର(char_u *), 10);

	କର
	{
	    item = next_val;
	    comma = vim_strchr(next_val, ',');
	    ଯେତେବେଳେ (comma != ଖାଲି && comma != next_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    ଯଦି (comma != ଖାଲି)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		next_val = comma + 1;
	    }
	    ଅନ୍ୟଥା
		next_val = ଖାଲି;

	    ଯଦି (*item == NUL)
		// empty value, don't add to list
		ଜାରି;

	    ଯଦି (!vim_regexec(regmatch, item, (colnr_T)0))
		ଜାରି;

	    char_u *buf = escape_option_str_cmdline(item);
	    ଯଦି (buf == ଖାଲି)
	    {
		vim_free(option_copy);
		ga_clear_strings(&ga);
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (ga_add_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		ଭାଙ୍ଗ;
	    }
	} ଯେତେବେଳେ (next_val != ଖାଲି);

	vim_free(option_copy);

	*matches = ga.ga_data;
	*ଅଙ୍କ_Matches = ga.ga_len;
	ଫେରନ୍ତୁ OK;
    }
    ଅନ୍ୟଥା ଯଦି (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	ଯଦି (*xp->xp_pattern != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    ଫେରନ୍ତୁ FAIL;
	}

	size_t ଅଙ୍କ_flags = STRLEN(option_val);
	ଯଦି (ଅଙ୍କ_flags == 0)
	    ଫେରନ୍ତୁ FAIL;

	*matches = ALLOC_MULT(char_u *, ଅଙ୍କ_flags + 1);
	ଯଦି (*matches == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;
	char_u *p;

	p = vim_strnsave(option_val, ଅଙ୍କ_flags);
	ଯଦି (p == ଖାଲି)
	{
	    VIM_CLEAR(*matches);
	    ଫେରନ୍ତୁ FAIL;
	}
	(*matches)[ଗଣନା_2++] = p;

	ଯଦି (ଅଙ୍କ_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    ପାଇଁ (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		ଯଦି (p == ଖାଲି)
		    ଭାଙ୍ଗ;
		(*matches)[ଗଣନା_2++] = p;
	    }
	}

	*ଅଙ୍କ_Matches = ଗଣନା_2;
	ଫେରନ୍ତୁ OK;
    }

    ଫେରନ୍ତୁ ExpandOldSetting(ଅଙ୍କ_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    ସ୍ଥିର ଶୂନ୍ୟ
option_value2string(
    ସଂରଚନା vimoption	*opp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    ଯଦି (opp->flags & P_ଅଙ୍କ)
    {
	long wc = 0;

	ଯଦି (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_ଚାବି_name((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wc, 0));
	ଅନ୍ୟଥା ଯଦି (wc != 0)
	    STRCPY(NameBuff, transchar((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wc));
	ଅନ୍ୟଥା
	    sprintf((ଅକ୍ଷର *)NameBuff, "%ld", *(long *)varp);
    }
    ଅନ୍ୟଥା    // P_STRING
    {
	varp = *(char_u **)(varp);
	ଯଦି (varp == ଖାଲି)		    // just in case
	    NameBuff[0] = NUL;
#ifdef FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	ଅନ୍ୟଥା ଯଦି (opp->var == (char_u *)&p_ଚାବି && *varp)
	    STRCPY(NameBuff, "*****");
#endif
	ଅନ୍ୟଥା ଯଦି (opp->flags & P_EXPAND)
	    home_replace(ଖାଲି, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	ଅନ୍ୟଥା ଯଦି ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	ଅନ୍ୟଥା
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
wc_use_keyname(char_u *varp, long *wcp)
{
    ଯଦି (((long *)varp == &p_wc) || ((long *)varp == &p_wcm))
    {
	*wcp = *(long *)varp;
	ଯଦି (IS_SPECIAL(*wcp) || find_special_ଚାବି_in_table((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*wcp) >= 0)
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
shortmess(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x)
{
    ଫେରନ୍ତୁ p_shm != ଖାଲି &&
	    (   vim_strchr(p_shm, x) != ଖାଲି
	    || (vim_strchr(p_shm, 'a') != ଖାଲି
		&& vim_strchr((char_u *)SHM_A, x) != ଖାଲି));
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 * When "fname" is not NULL, use it to set $"envname" when it wasn't set yet.
 */
    ଶୂନ୍ୟ
vimrc_found(char_u *fname, char_u *envname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dofree = FALSE;
    char_u	*p;

    ଯଦି (!option_was_set((char_u *)"cp"))
    {
	p_cp = FALSE;
	ପାଇଁ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    ଯଦି (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    ଯଦି (fname != ଖାଲି)
    {
	p = vim_getenv(envname, &dofree);
	ଯଦି (p == ଖାଲି)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = FullName_save(fname, FALSE);
	    ଯଦି (p != ଖାଲି)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	ଅନ୍ୟଥା ଯଦି (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    ଶୂନ୍ୟ
change_compatible(ପୂର୍ଣ୍ଣସଂଖ୍ୟା on)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    opt_idx;

    ଯଦି (p_cp != on)
    {
	p_cp = on;
	compatible_set();
    }
    opt_idx = findoption((char_u *)"cp");
    ଯଦି (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_SET;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
option_was_set(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

    idx = findoption(name);
    ଯଦି (idx < 0)	// unknown option
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (options[idx].flags & P_WAS_SET)
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
reset_option_was_set(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = findoption(name);

    ଯଦି (idx < 0)
	ଫେରନ୍ତୁ FAIL;

    options[idx].flags &= ~P_WAS_SET;
    ଫେରନ୍ତୁ OK;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
compatible_set(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    opt_idx;

    ପାଇଁ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	ଯଦି (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    set_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
can_bs(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ifdef FEAT_JOB_CHANNEL
    ଯଦି (what == BS_START && bt_prompt(curbuf))
	ଫେରନ୍ତୁ FALSE;
#endif
    ବାଛ (*p_bs)
    {
	ସ୍ଥିତି '3':       ଫେରନ୍ତୁ TRUE;
	ସ୍ଥିତି '2':	ଫେରନ୍ତୁ (what != BS_NOSTOP);
	ସ୍ଥିତି '1':	ଫେରନ୍ତୁ (what != BS_START);
	ସ୍ଥିତି '0':	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ vim_strchr(p_bs, what) != ଖାଲି;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloff_ମୂଲ୍ୟ(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloffpad_ମୂଲ୍ୟ(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_sidescrolloff_ମୂଲ୍ୟ(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_bkc_flags(buf_T *buf)
{
    ଫେରନ୍ତୁ buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#ଯଦି defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_ମୂଲ୍ୟ(buf_T *buf)
{
    ଯଦି (buf->b_p_flp == ଖାଲି || *buf->b_p_flp == NUL)
	ଫେରନ୍ତୁ p_flp;
    ଫେରନ୍ତୁ buf->b_p_flp;
}
#endif

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_ve_flags(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#ଯଦି defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_ମୂଲ୍ୟ(win_T *win)
{
    ଯଦି (win->w_p_sbr == ଖାଲି || *win->w_p_sbr == NUL)
	ଫେରନ୍ତୁ p_sbr;
    ଯଦି (STRCMP(win->w_p_sbr, "NONE") == 0)
	ଫେରନ୍ତୁ empty_option;
    ଫେରନ୍ତୁ win->w_p_sbr;
}
#endif

#ଯଦି defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufopt)
{
    dict_T	*d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx;

    d = dict_alloc();
    ଯଦି (d == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	ସଂରଚନା vimoption *opt = &options[opt_idx];

	ଯଦି ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    ଯଦି (varp != ଖାଲି)
	    {
		ଯଦି (opt->flags & P_STRING)
		    dict_add_string(d, opt->fullname, *(char_u **)varp);
		ଅନ୍ୟଥା ଯଦି (opt->flags & P_ଅଙ୍କ)
		    dict_add_ସଂଖ୍ୟା(d, opt->fullname, *(long *)varp);
		ଅନ୍ୟଥା
		    dict_add_ସଂଖ୍ୟା(d, opt->fullname, *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)varp);
	    }
	}
    }

    ଫେରନ୍ତୁ d;
}
#endif

#ଯଦି defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    ଯଦି (val == ଖାଲି)
	p = wp->w_p_culopt;
    ଅନ୍ୟଥା
	p = val;
    ଯେତେବେଳେ (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	ଯଦି (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_ଧାଡ଼ି;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_ଧାଡ଼ି | CULOPT_NBR;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	ଯଦି (*p != ',' && *p != NUL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    ଯଦି ((culopt_flags_new & CULOPT_ଧାଡ଼ି) && (culopt_flags_new & CULOPT_SCRLINE))
	ଫେରନ୍ତୁ FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
magic_isset(ଶୂନ୍ୟ)
{
    ବାଛ (magic_overruled)
    {
	ସ୍ଥିତି OPTION_MAGIC_ON:      ଫେରନ୍ତୁ TRUE;
	ସ୍ଥିତି OPTION_MAGIC_OFF:     ଫେରନ୍ତୁ FALSE;
	ସ୍ଥିତି OPTION_MAGIC_NOT_SET: ଭାଙ୍ଗ;
    }
#ifdef FEAT_EVAL
    ଯଦି (in_vim9script())
	ଫେରନ୍ତୁ TRUE;
#endif
    ଫେରନ୍ତୁ p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
option_set_callback_func(char_u *optval UNUSED, callback_T *optcb UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		funcname = FALSE;

    ଯଦି (optval == ଖାଲି || *optval == NUL)
    {
	free_callback(optcb);
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି (*optval == '{' || (in_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_ପଦସମଷ୍ଟି(optval, ଖାଲି);
    ଅନ୍ୟଥା
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    ଯଦି (tv == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    cb = get_callback(tv);
    ଯଦି (cb.cb_name == ଖାଲି || *cb.cb_name == NUL)
    {
	free_tv(tv);
	ଫେରନ୍ତୁ FAIL;
    }

    free_callback(optcb);
    set_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = ଖାଲି;
    ଯଦି (in_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != ଖାଲି))
    {
	ଯଦି (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = find_imported(optval, dot - optval, FALSE);
	ଯଦି (import != ଖାଲି && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((ଅକ୍ଷର *)fnamebuf, ଆକାର(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	ଅନ୍ୟଥା
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

    ଫେରନ୍ତୁ OK;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ FAIL;
#endif
}

#ଯଦି defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    ଅକ୍ଷର *
did_set_showtabpanel(optset_T *args UNUSED)
{
    shell_new_columns();
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_EVAL)
    ସ୍ଥିର ଶୂନ୍ୟ
didset_options_sctx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags, ଅକ୍ଷର **buf)
{
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; ; ++i)
	{
	    ଯଦି (buf[i] == ଖାଲି)
		ଭାଙ୍ଗ;

	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = findoption((char_u *)buf[i]);
	    ଯଦି (idx >= 0)
		set_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#endif
