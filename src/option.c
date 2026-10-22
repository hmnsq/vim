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
#অন্তর্ভাব "vim.h"
#অন্তর্ভাব "optiondefs.h"

স্থির শূন্য set_options_default(পূর্ণসংখ্যা opt_flags);
স্থির শূন্য set_string_default_esc(অক্ষর *name, char_u *val, পূর্ণসংখ্যা escape);
স্থির char_u *find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
স্থির char_u *option_expand(পূর্ণসংখ্যা opt_idx, char_u *val);
স্থির শূন্য didset_options(শূন্য);
স্থির শূন্য didset_options2(শূন্য);
#যদি defined(FEAT_EVAL)
স্থির long_u *insecure_flag(win_T *wp, পূর্ণসংখ্যা opt_idx, পূর্ণসংখ্যা opt_flags);
#অন্যথা
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#endif
স্থির অক্ষর *set_bool_option(পূর্ণসংখ্যা opt_idx, char_u *varp, পূর্ণসংখ্যা মান, পূর্ণসংখ্যা opt_flags);
স্থির অক্ষর *set_অঙ্ক_option(পূর্ণসংখ্যা opt_idx, char_u *varp, long মান, অক্ষর *errbuf, size_t errbuflen, পূর্ণসংখ্যা opt_flags);
স্থির পূর্ণসংখ্যা find_চাবি_option(char_u *arg_arg, পূর্ণসংখ্যা has_lt);
স্থির শূন্য showoptions(পূর্ণসংখ্যা all, পূর্ণসংখ্যা opt_flags);
স্থির পূর্ণসংখ্যা optval_default(কাঠামো vimoption *, char_u *varp, পূর্ণসংখ্যা compatible);
স্থির শূন্য showoneopt(কাঠামো vimoption *, পূর্ণসংখ্যা opt_flags);
স্থির পূর্ণসংখ্যা put_setstring(FILE *fd, bool legacy, অক্ষর *cmd, অক্ষর *name, char_u **valuep, long_u flags);
স্থির পূর্ণসংখ্যা put_setnum(FILE *fd, অক্ষর *cmd, অক্ষর *name, long *valuep);
স্থির পূর্ণসংখ্যা put_setbool(FILE *fd, অক্ষর *cmd, অক্ষর *name, পূর্ণসংখ্যা মান);
স্থির পূর্ণসংখ্যা istermoption(কাঠামো vimoption *p);
স্থির পূর্ণসংখ্যা istermoption_idx(পূর্ণসংখ্যা opt_idx);
স্থির char_u *get_varp_scope(কাঠামো vimoption *p, পূর্ণসংখ্যা scope);
স্থির char_u *get_varp(কাঠামো vimoption *);
স্থির শূন্য check_win_options(win_T *win);
স্থির শূন্য option_value2string(কাঠামো vimoption *, পূর্ণসংখ্যা scope);
স্থির শূন্য check_winopt(winopt_T *wop);
স্থির পূর্ণসংখ্যা wc_use_keyname(char_u *varp, long *wcp);
স্থির শূন্য compatible_set(শূন্য);

#যদি defined(FEAT_EVAL)
স্থির অক্ষর *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", ফাঁকা};
স্থির অক্ষর *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    ফাঁকা};
স্থির শূন্য didset_options_sctx(পূর্ণসংখ্যা opt_flags, অক্ষর **buf);
#endif


/*
 * Initialize the 'shell' option to a default value.
 */
    স্থির শূন্য
set_init_default_shell(শূন্য)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    যদি (((p = mch_getenv((char_u *)"SHELL")) != ফাঁকা && *p != NUL)
#যদি defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != ফাঁকা && *p != NUL)
	    || ((p = (char_u *)default_shell()) != ফাঁকা && *p != NUL)
#endif
	    )
#যদি defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	যদি (vim_strchr(p, ' ') != ফাঁকা)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    যদি (cmd != ফাঁকা)
	    {
		vim_snprintf((অক্ষর *)cmd, len, "\"%s\"", p);
		set_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	অন্যথা
	    set_string_default("sh", p);
    }
#অন্যথা
	set_string_default_esc("sh", p, TRUE);
#endif
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    স্থির শূন্য
set_init_default_backupskip(শূন্য)
{
    পূর্ণসংখ্যা		opt_idx;
    পূর্ণসংখ্যা		i;
    char_u	*p;
    পূর্ণসংখ্যা		plen;
#ifdef UNIX
    স্থির অক্ষর	*(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#অন্যথা
    স্থির অক্ষর	*(names[3]) = {"TMPDIR", "TEMP", "TMP"};
#endif
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(names); ++i)
    {
	পূর্ণসংখ্যা		mustfree = FALSE;
#ifdef UNIX
	যদি (*names[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (পূর্ণসংখ্যা)STRLEN_LITERAL("/private/tmp");
# অন্যথা
	    p = (char_u *)"/tmp";
	    plen = (পূর্ণসংখ্যা)STRLEN_LITERAL("/tmp");
# endif
	}
	অন্যথা
#endif
	{
	    p = vim_getenv((char_u *)names[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	যদি (p != ফাঁকা && *p != NUL)
	{
	    char_u  *item;
	    size_t  itemsize;
	    পূর্ণসংখ্যা	    has_trailing_path_sep = FALSE;

	    যদি (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (পূর্ণসংখ্যা)STRLEN(p);
		// does the value include a trailing path separator?
		যদি (after_pathsep(p, p + plen))
		    has_trailing_path_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_path_sep ? 0 : 1) + 2;
	    item = alloc(itemsize);
	    যদি (item != ফাঁকা)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((অক্ষর *)item, itemsize, "%s%s*", p, (has_trailing_path_sep) ? "" : PATHSEPSTR);

		যদি (find_dup_item(ga.ga_তথ্য, item, itemlen, options[opt_idx].flags) == ফাঁকা
			&& ga_grow(&ga, (পূর্ণসংখ্যা)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((অক্ষর *)ga.ga_তথ্য + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", item);
		}
		vim_free(item);
	    }
	}
	যদি (mustfree)
	    vim_free(p);
    }
    যদি (ga.ga_তথ্য != ফাঁকা)
    {
	set_string_default("bsk", ga.ga_তথ্য);
	vim_free(ga.ga_তথ্য);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    স্থির শূন্য
set_init_default_maxmemtot(শূন্য)
{
    পূর্ণসংখ্যা		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    যদি (opt_idx < 0)
	ফেরত;

#যদি !defined(HAVE_AVAIL_MEM) && !defined(HAVE_যোগ_MEM)
    যদি (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#endif
    {
#যদি defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#নতুবা_যদি defined(HAVE_যোগ_MEM)
	// Use amount of memory available to Vim.
	n = (mch_যোগ_mem(FALSE) >> 1);
#অন্যথা
	n = (0x7fffffff >> 11);
#endif
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	যদি (opt_idx >= 0)
	{
#যদি !defined(HAVE_AVAIL_MEM) && !defined(HAVE_যোগ_MEM)
	    যদি ((long)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (long)n
		    || (long)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#endif
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    স্থির শূন্য
set_init_default_cdpath(শূন্য)
{
    পূর্ণসংখ্যা		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    পূর্ণসংখ্যা	i;
    পূর্ণসংখ্যা	j;
    পূর্ণসংখ্যা	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    যদি (cdpath == ফাঁকা)
	ফেরত;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    যদি (buf != ফাঁকা)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	জন্য (i = 0; cdpath[i] != NUL; ++i)
	{
	    যদি (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    অন্যথা
	    {
		যদি (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	যদি (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	অন্যথা
	    vim_free(buf); // cannot happen
    }
    যদি (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    স্থির শূন্য
set_init_default_printencoding(শূন্য)
{
#যদি defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    set_string_default("penc",
# যদি defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# অন্যথা // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#endif
}

#যদি defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    স্থির শূন্য
set_init_default_printexpr(শূন্য)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    set_string_default("pexpr",
# যদি defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# অন্যথা
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#endif

#ifdef UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    স্থির শূন্য
set_init_restricted_mode(শূন্য)
{
    char_u	*p;

    p = get_isolated_shell_name();
    যদি (p == ফাঁকা)
	ফেরত;
    যদি (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#endif

#ifdef CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    স্থির শূন্য
set_init_clean_rtp(শূন্য)
{
    পূর্ণসংখ্যা		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    যদি (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    যদি (opt_idx < 0)
	ফেরত;
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
    স্থির শূন্য
set_init_xdg_rtp(শূন্য)
{
    পূর্ণসংখ্যা		opt_idx;
    পূর্ণসংখ্যা		has_xdg_env = TRUE;
    পূর্ণসংখ্যা		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = ফাঁকা;
    char_u	*vimrc2 = ফাঁকা;
    char_u	*xdg_dir = ফাঁকা;
    char_u	*xdg_rtp = ফাঁকা;
    string_T	vimrc_xdg = {ফাঁকা, 0};

    // initialize chartab, so we can expand $HOME
    (শূন্য)init_chartab();
    vimrc1 = expand_env_save((char_u *)USR_VIMRC_FILE);
    vimrc2 = expand_env_save((char_u *)USR_VIMRC_FILE2);

    xdg_dir = mch_getenv("XDG_CONFIG_HOME");
    যদি (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    যদি (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	যাও theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    যদি ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	যাও theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    যদি ((opt_idx = findoption((char_u *)"packpath")) < 0)
	যাও theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# যদি defined(XDG_VDIR) && defined(FEAT_SESSION)
    যদি ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	যাও theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    যদি (should_free_xdg_dir)
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
    স্থির শূন্য
set_init_expand_env(শূন্য)
{
    পূর্ণসংখ্যা		opt_idx;
    char_u	*p;

    জন্য (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	যদি ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != ফাঁকা)
	    p = (char_u *)_(*(অক্ষর **)options[opt_idx].var);
	অন্যথা
	    p = option_expand(opt_idx, ফাঁকা);
	যদি (p != ফাঁকা && (p = vim_strsave(p)) != ফাঁকা)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    যদি (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#যদি defined(MSWIN) && defined(FEAT_GETTEXT)
/*
 * Get the display language of Windows and the languages to fall back on, as a
 * colon separated list for gettext, e.g. "ja_JP:en_US".  The list stops after
 * English, untranslated messages are English already.
 * Returns NULL when it cannot be obtained.  The result must be freed.
 */
    স্থির char_u *
get_ui_langs(শূন্য)
{
    ULONG	অঙ্ক_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*বাফার;
    char_u	*langs = ফাঁকা;

    যদি (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &অঙ্ক_languages, ফাঁকা,
								    &bufsize)
	    || bufsize == 0)
	ফেরত ফাঁকা;

    বাফার = ALLOC_MULT(WCHAR, bufsize);
    যদি (বাফার == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &অঙ্ক_languages, বাফার,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	যদি (langs != ফাঁকা)
	{
	    char_u	*d = langs;
	    WCHAR	*s = বাফার;

	    যখন (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		যদি (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		জন্য ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		যদি (english)
		    বিরতি;
	    }
	    *d = NUL;
	}
    }
    vim_free(বাফার);

    ফেরত langs;
}
#endif

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    স্থির শূন্য
set_init_lang_env(শূন্য)
{
#যদি defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    যদি (mch_getenv((char_u *)"LANG") == ফাঁকা
	    && mch_getenv((char_u *)"LANGUAGE") == ফাঁকা
	    && mch_getenv((char_u *)"LC_ALL") == ফাঁকা
	    && mch_getenv((char_u *)"LC_MESSAGES") == ফাঁকা)
    {
	char_u	*langs = get_ui_langs();

	যদি (langs != ফাঁকা && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    যদি (colon != ফাঁকা)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#নতুবা_যদি defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#endif
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    স্থির শূন্য
set_init_default_encoding(শূন্য)
{
    char_u	*p;
    পূর্ণসংখ্যা		opt_idx;

#যদি defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#অন্যথা
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#endif
    যদি (p == ফাঁকা)
	ফেরত;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    যদি (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    যদি (mb_init() == ফাঁকা)
    {
	opt_idx = findoption((char_u *)"encoding");
	যদি (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#যদি defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	যদি (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    set_string_option_direct((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    set_string_option_direct((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    যদি (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    যদি (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (শূন্য)init_chartab();
	}
#endif

#যদি defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	যদি (
# ifdef VIMDLL
		(!gui.in_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    অক্ষর	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    যদি (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, আকার(buf), "cp%ld", (long)GetACP());
	    অন্যথা
		buflen = vim_snprintf(buf, আকার(buf), "cp%ld", (long)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    যদি (p_tenc != ফাঁকা)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		যদি (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_setup(&প্রবেশ_conv, p_tenc, p_enc);
		convert_setup(&output_conv, p_enc, p_tenc);
	    }
	    অন্যথা
		p_tenc = empty_option;
	}
#endif
#যদি defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#endif
    }
    অন্যথা
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
    শূন্য
set_init_1(পূর্ণসংখ্যা clean_arg)
{
#ifdef FEAT_LANGMAP
    langmap_init();
#endif

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    যদি (mch_getenv((char_u *)"VIM_POSIX") != ফাঁকা)
    {
	set_string_default("cpo", (char_u *)CPO_ALL);
	set_string_default("shm", (char_u *)SHM_POSIX);
    }

    set_init_default_shell();
    set_init_default_backupskip();
    set_init_default_maxmemtot();
    set_init_default_cdpath();
    set_init_default_printencoding();
#যদি defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
    যদি (clean_arg)
	set_init_clean_rtp();
#endif

#ifdef FEAT_GUI
    যদি (found_reverse_arg)
	set_option_মান_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
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

#যদি defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    যদি (mch_getenv((char_u *)"MLTERM") != ফাঁকা)
	set_option_মান_give_err((char_u *)"tbidi", 1L, ফাঁকা, 0);
#endif

    didset_options2();

    set_init_lang_env();

#ifdef FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    set_helplang_default(get_mess_lang());
#endif
}

স্থির char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    শূন্য
set_fencs_unicode(শূন্য)
{
    set_string_option_direct((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    স্থির শূন্য
set_option_default(
    পূর্ণসংখ্যা		opt_idx,
    পূর্ণসংখ্যা		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    পূর্ণসংখ্যা		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    পূর্ণসংখ্যা		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    পূর্ণসংখ্যা		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    যদি (varp != ফাঁকা)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	যদি (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    যদি (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		set_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    অন্যথা যদি (options[opt_idx].indir != PV_NONE)
		set_string_option_direct(ফাঁকা, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    অন্যথা
	    {
		যদি ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	অন্যথা যদি (flags & P_অঙ্ক)
	{
	    যদি (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    অন্যথা
	    {
		long def_val = (long)(long_i)options[opt_idx].def_val[dvi];

		যদি ((long *)varp == &curwin->w_p_so
			|| (long *)varp == &curwin->w_p_siso
			|| (long *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(long *)varp = -1;
		অন্যথা
		    *(long *)varp = def_val;
		// May also set global value for local option.
		যদি (both)
		    *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	অন্যথা	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(পূর্ণসংখ্যা *)varp = (পূর্ণসংখ্যা)(long)(long_i)options[opt_idx].def_val[dvi];
#ifdef UNIX
	    // 'modeline' defaults to off for root
	    যদি (options[opt_idx].indir == PV_ML && getuid() == মূল_UID)
		*(পূর্ণসংখ্যা *)varp = FALSE;
#endif
	    // May also set global value for local option.
	    যদি (both)
		*(পূর্ণসংখ্যা *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(পূর্ণসংখ্যা *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	যদি (both)
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
    স্থির শূন্য
set_options_default(
    পূর্ণসংখ্যা		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    পূর্ণসংখ্যা		i;
    win_T	*wp;
    tabpage_T	*tp;

    জন্য (i = 0; !istermoption_idx(i); i++)
	যদি (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#যদি defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_চাবি
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
    স্থির শূন্য
set_string_default_esc(অক্ষর *name, char_u *val, পূর্ণসংখ্যা escape)
{
    char_u	*p;
    পূর্ণসংখ্যা		opt_idx;

    যদি (escape && vim_strchr(val, ' ') != ফাঁকা)
	p = vim_strsave_escaped(val, (char_u *)" ");
    অন্যথা
	p = vim_strsave(val);
    যদি (p == ফাঁকা)		// we don't want a NULL
	ফেরত;

    opt_idx = findoption((char_u *)name);
    যদি (opt_idx < 0)
    {
	vim_free(p);
	ফেরত;
    }

    যদি (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    শূন্য
set_string_default(অক্ষর *name, char_u *val)
{
    set_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    স্থির char_u *
find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags)
{
    পূর্ণসংখ্যা	    bs = 0;
    char_u  *s;

    যদি (origval == ফাঁকা)
	ফেরত ফাঁকা;

    জন্য (s = origval; *s != NUL; ++s)
    {
	যদি ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    ফেরত s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	যদি ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	অন্যথা
	    bs = 0;
    }
    ফেরত ফাঁকা;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    শূন্য
set_সংখ্যা_default(অক্ষর *name, long val)
{
    পূর্ণসংখ্যা		opt_idx;

    opt_idx = findoption((char_u *)name);
    যদি (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#যদি defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    শূন্য
set_local_options_default(win_T *wp, পূর্ণসংখ্যা do_বাফার)
{
    win_T	*save_curwin = curwin;
    পূর্ণসংখ্যা		i;

    curwin = wp;
    curbuf = curwin->w_বাফার;
    block_autocmds();

    জন্য (i = 0; !istermoption_idx(i); i++)
    {
	কাঠামো vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	যদি (p->indir != PV_NONE
		&& (do_বাফার || (p->indir & PV_BUF) == 0)
		&& !(options[i].flags & P_NODEFAULT)
		&& !optval_default(p, varp, FALSE))
	    set_option_default(i, OPT_FREE|OPT_LOCAL, FALSE);
    }

    unblock_autocmds();
    curwin = save_curwin;
    curbuf = curwin->w_বাফার;
}
#endif

#যদি defined(EXITFREE)
/*
 * Free all options.
 */
    শূন্য
free_all_options(শূন্য)
{
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; !istermoption_idx(i); i++)
    {
	যদি (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    যদি ((options[i].flags & P_ALLOCED) && options[i].var != ফাঁকা)
		free_string_option(*(char_u **)options[i].var);
	    যদি (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	অন্যথা যদি (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# যদি defined(FEAT_EVAL)
    free_findfunc_option();
# endif
}
#endif


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    শূন্য
set_init_2(শূন্য)
{
    পূর্ণসংখ্যা		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    যদি (idx >= 0 && !(options[idx].flags & P_WAS_SET))
	set_option_default(idx, OPT_LOCAL, p_cp);
    comp_col();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    যদি (!option_was_set((char_u *)"window"))
	p_window = Rows - 1;
    set_সংখ্যা_default("window", Rows - 1);

    // For DOS console the default is always black.
#যদি !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    যদি (idx >= 0 && !(options[idx].flags & P_WAS_SET)
						 && *term_bg_default() == 'd')
    {
	set_string_option_direct(ফাঁকা, idx, (char_u *)"dark", OPT_FREE, 0);
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
    (শূন্য)parse_printoptions(ফাঁকা);	// parse 'printoptions' default value
#endif
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    শূন্য
set_init_3(শূন্য)
{
#যদি defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    পূর্ণসংখ্যা	    idx_srr;
    পূর্ণসংখ্যা	    do_srr;
# ifdef FEAT_QUICKFIX
    পূর্ণসংখ্যা	    idx_sp;
    পূর্ণসংখ্যা	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    যদি (idx_srr < 0)
	do_srr = FALSE;
    অন্যথা
	do_srr = !(options[idx_srr].flags & P_WAS_SET);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    যদি (idx_sp < 0)
	do_sp = FALSE;
    অন্যথা
	do_sp = !(options[idx_sp].flags & P_WAS_SET);
# endif
    p = get_isolated_shell_name();
    যদি (p != ফাঁকা)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	যদি (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# যদি defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# যদি defined(FEAT_QUICKFIX)
	    যদি (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  অন্যথা
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    যদি (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	অন্যথা যদি (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  যদি defined(FEAT_QUICKFIX)
		যদি (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		যদি (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	অন্যথা
	    // Always use POSIX shell style redirection if we reach this
	    যদি (       fnamecmp(p, "sh") == 0
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
# যদি defined(FEAT_QUICKFIX)
		যদি (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  অন্যথা
		    যদি (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    অন্যথা
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		যদি (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#endif

#যদি defined(MSWIN)
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
    যদি (strstr((অক্ষর *)gettail(p_sh), "powershell") != ফাঁকা)
    {
	পূর্ণসংখ্যা	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	যদি (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	যদি (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    অন্যথা যদি (strstr((অক্ষর *)gettail(p_sh), "sh") != ফাঁকা)
    {
	পূর্ণসংখ্যা	idx3;

	idx3 = findoption((char_u *)"shcf");
	যদি (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	যদি (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    অন্যথা যদি (strstr((অক্ষর *)gettail(p_sh), "cmd.exe") != ফাঁকা)
    {
	পূর্ণসংখ্যা	idx3;

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
	যদি (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	যদি (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#endif

    যদি (BUFEMPTY())
    {
	পূর্ণসংখ্যা idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	যদি (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_SET))
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    set_title_defaults();
}

#যদি defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    শূন্য
set_helplang_default(char_u *lang)
{
    পূর্ণসংখ্যা		idx;
    size_t	langlen;

    যদি (lang == ফাঁকা)	// safety check
	ফেরত;

    langlen = STRLEN(lang);
    যদি (langlen < 2)	// safety check
	ফেরত;

    idx = findoption((char_u *)"hlg");
    যদি (idx < 0 || (options[idx].flags & P_WAS_SET))
	ফেরত;

    যদি (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    যদি (p_hlg == ফাঁকা)
	p_hlg = empty_option;
    অন্যথা
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	যদি (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	অন্যথা যদি (langlen >= 1 && *p_hlg == 'C')
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
    শূন্য
set_title_defaults(শূন্য)
{
    পূর্ণসংখ্যা	    idx1;
    long    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    যদি (idx1 >= 0 && !(options[idx1].flags & P_WAS_SET))
    {
#ifdef FEAT_GUI
	যদি (gui.starting || gui.in_use)
	    val = TRUE;
	অন্যথা
#endif
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    যদি (idx1 < 0 || (options[idx1].flags & P_WAS_SET))
    {
	ফেরত;
    }

#ifdef FEAT_GUI
    যদি (gui.starting || gui.in_use)
	val = TRUE;
    অন্যথা
#endif
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    শূন্য
ex_set(exarg_T *eap)
{
    পূর্ণসংখ্যা		flags = 0;

    যদি (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    অন্যথা যদি (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#যদি defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    যদি ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    অন্যথা
#endif
    {
	যদি (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(শূন্য)do_set(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
প্রকারলেখ গণনা {
    PREFIX_NO = 0,	// "no" prefix
    PREFIX_NONE,	// no prefix
    PREFIX_INV,		// "inv" prefix
} set_prefix_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    স্থির set_prefix_T
get_option_prefix(char_u **argp)
{
    পূর্ণসংখ্যা		prefix = PREFIX_NONE;
    char_u	*arg = *argp;

    যদি (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	prefix = PREFIX_NO;
	arg += 2;
    }
    অন্যথা যদি (STRNCMP(arg, "inv", 3) == 0)
    {
	prefix = PREFIX_INV;
	arg += 3;
    }

    *argp = arg;
    ফেরত prefix;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    স্থির পূর্ণসংখ্যা
parse_option_name(char_u *arg, পূর্ণসংখ্যা *opt_idxp, পূর্ণসংখ্যা *lenp, পূর্ণসংখ্যা *keyp)
{
    পূর্ণসংখ্যা	চাবি = 0;
    পূর্ণসংখ্যা	len;
    পূর্ণসংখ্যা	opt_idx;

    যদি (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	যদি (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	অন্যথা
	{
	    len = 1;
	    যখন (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	যদি (arg[len] != '>')
	    ফেরত FAIL;

	arg[len] = NUL;			    // put NUL after name
	যদি (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	যদি (opt_idx == -1)
	    চাবি = find_চাবি_option(arg + 1, TRUE);
    }
    অন্যথা
    {
	পূর্ণসংখ্যা	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	যদি (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	অন্যথা
	    যখন (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	যদি (opt_idx == -1)
	    চাবি = find_চাবি_option(arg, FALSE);
    }

    *keyp = চাবি;
    *lenp = len;
    *opt_idxp = opt_idx;

    ফেরত OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    স্থির set_op_T
get_opt_op(char_u *arg)
{
    set_op_T op = OP_NONE;

    যদি (*arg != NUL && *(arg + 1) == '=')
    {
	যদি (*arg == '+')
	    op = OP_ADDING;		// "+="
	অন্যথা যদি (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	অন্যথা যদি (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    ফেরত op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
স্থির অক্ষর *modeline_whitelist[] =
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
    ফাঁকা
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    স্থির bool
is_modeline_whitelisted(অক্ষর *name)
{
    জন্য (পূর্ণসংখ্যা i = 0; modeline_whitelist[i] != ফাঁকা; i++)
	যদি (STRCMP(name, modeline_whitelist[i]) == 0)
	    ফেরত true;
    ফেরত false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    স্থির পূর্ণসংখ্যা
validate_opt_idx(
	পূর্ণসংখ্যা opt_idx,
	পূর্ণসংখ্যা opt_flags,
	long_u flags,
	অক্ষর **errmsg,
	set_prefix_T prefix)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    যদি ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	ফেরত FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    যদি ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	ফেরত FAIL;

    // Disallow changing some options from modelines.
    যদি (opt_flags & OPT_MODELINE)
    {
	যদি (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_in_modeline;
	    ফেরত FAIL;
	}
	যদি ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_in_modeline_when_modelineexpr_is_off;
	    ফেরত FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	যদি (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    যদি (options[opt_idx].indir == PV_ML && prefix == PREFIX_NO)
		ফেরত OK;
	    অন্যথা যদি (!is_modeline_whitelisted(options[opt_idx].fullname))
		ফেরত FAIL;
	}
#ifdef FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	যদি (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    ফেরত FAIL;
#endif
    }

#ifdef HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    যদি (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_in_sandbox;
	ফেরত FAIL;
    }
#endif

    ফেরত OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    স্থির char_u *
stropt_get_default_val(
    পূর্ণসংখ্যা		opt_idx,
    char_u	*varp,
    পূর্ণসংখ্যা		flags,
    পূর্ণসংখ্যা		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    যদি ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ifdef FEAT_GUI
	যদি (gui.in_use)
	    newval = gui_bg_default();
	অন্যথা
#endif
	    newval = term_bg_default();
    }
    অন্যথা যদি ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    যদি (newval == ফাঁকা)
	newval = empty_option;
    অন্যথা
    {
	char_u *s = option_expand(opt_idx, newval);
	যদি (s == ফাঁকা)
	    s = newval;
	newval = vim_strsave(s);
    }

    ফেরত newval;
}

/*
 * Convert the 'backspace' option number value to a string: for adding,
 * prepending and removing string.
 */
    স্থির শূন্য
opt_backspace_nr2str(
    char_u	*varp,
    char_u	**origval_p,
    char_u	**origval_l_p,
    char_u	**origval_g_p,
    char_u	**oldval_p)
{
    পূর্ণসংখ্যা		i = getdigits((char_u **)varp);

    নির্বাচন (i)
    {
	ক্ষেত্র 0:
	    *(char_u **)varp = empty_option;
	    বিরতি;
	ক্ষেত্র 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    বিরতি;
	ক্ষেত্র 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    বিরতি;
	ক্ষেত্র 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    বিরতি;
    }
    vim_free(*oldval_p);
    যদি (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    যদি (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    যদি (*origval_g_p == *oldval_p)
	*origval_g_p = *(char_u **)varp;
    *oldval_p = *(char_u **)varp;
}

/*
 * Convert the 'whichwrap' option number value to a string, for backwards
 * compatibility with Vim 3.0.
 * Note: 'argp' is a pointer to a char_u pointer and is updated.
 */
    স্থির char_u *
opt_whichwrap_nr2str(char_u **argp, char_u *whichwrap)
{
    size_t  len = 0;

    *whichwrap = NUL;
    পূর্ণসংখ্যা i = getdigits(argp);
    যদি (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    যদি (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    যদি (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    যদি (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    যদি (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    যদি (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    ফেরত whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    স্থির char_u *
stropt_copy_মান(
    char_u	*origval,
    char_u	**argp,
    set_op_T	op,
    পূর্ণসংখ্যা		flags UNUSED)
{
    char_u	*arg = *argp;
    unsigned	newlen;
    char_u	*newval;
    char_u	*s = ফাঁকা;

    // get a bit too much
    newlen = (unsigned)STRLEN(arg) + 1;
    যদি (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;
    newval = alloc(newlen);
    যদি (newval == ফাঁকা)  // out of mem, don't change
	ফেরত ফাঁকা;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    যখন (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	পূর্ণসংখ্যা i;

	যদি (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#endif
	   )
	    ++arg;	// remove backslash
	যদি (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	অন্যথা
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    ফেরত newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    স্থির char_u *
stropt_expand_envvar(
    পূর্ণসংখ্যা		opt_idx,
    char_u	*origval,
    char_u	*newval,
    set_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    যদি (s == ফাঁকা)
	ফেরত newval;

    vim_free(newval);
    unsigned newlen = (unsigned)STRLEN(s) + 1;
    যদি (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;

    newval = alloc(newlen);
    যদি (newval == ফাঁকা)
	ফেরত ফাঁকা;

    STRCPY(newval, s);

    ফেরত newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    স্থির শূন্য
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    পূর্ণসংখ্যা		flags)
{
    পূর্ণসংখ্যা		len = 0;

    পূর্ণসংখ্যা comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    যদি (op == OP_ADDING)
    {
	len = (পূর্ণসংখ্যা)STRLEN(origval);
	// strip a trailing comma, would get 2
	যদি (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    অন্যথা
    {
	len = (পূর্ণসংখ্যা)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    যদি (comma)
	newval[len] = ',';
}

/*
 * Remove a value from a string option.  Copy string option value in "origval"
 * to "newval" and then remove the string "strval" of length "len".
 */
    স্থির শূন্য
stropt_remove_val(
    char_u	*origval,
    char_u	*newval,
    পূর্ণসংখ্যা		flags,
    char_u	*strval,
    পূর্ণসংখ্যা		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    যদি (*strval)
    {
	// may need to remove a comma
	যদি (flags & P_COMMA)
	{
	    যদি (strval == origval)
	    {
		// include comma after string
		যদি (strval[len] == ',')
		    ++len;
	    }
	    অন্যথা
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
    স্থির char_u *
find_চাবি_item(char_u *src, char_u *চাবি, পূর্ণসংখ্যা keylen, পূর্ণসংখ্যা *itemlenp)
{
    char_u  *p = src;

    যখন (*p != NUL)
    {
	// Check if this item starts with the same key
	যদি ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, চাবি, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *end = vim_strchr(p, ',');
	    যদি (end == ফাঁকা)
		end = p + STRLEN(p);
	    *itemlenp = (পূর্ণসংখ্যা)(end - p);
	    ফেরত p;
	}
	++p;
    }
    ফেরত ফাঁকা;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    স্থির শূন্য
remove_comma_item(char_u *str, char_u *item, পূর্ণসংখ্যা itemlen)
{
    যদি (item[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(item, item + itemlen + 1);
    অন্যথা যদি (item > str && *(item - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(item - 1, item + itemlen);
    অন্যথা
	// Only item
	*item = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    স্থির শূন্য
remove_চাবি_item(char_u *str, char_u *চাবি, পূর্ণসংখ্যা keylen, char_u *skip)
{
    পূর্ণসংখ্যা	    itemlen;
    char_u  *found;

    যখন ((found = find_চাবি_item(str, চাবি, keylen, &itemlen)) != ফাঁকা)
    {
	যদি (found == skip)
	{
	    // Search for the next match after this one.
	    char_u *পরবর্তী = found + itemlen;
	    যদি (*পরবর্তী == ',')
		++পরবর্তী;
	    found = find_চাবি_item(পরবর্তী, চাবি, keylen, &itemlen);
	    যদি (found == ফাঁকা)
		বিরতি;
	}

	remove_comma_item(str, found, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    স্থির শূন্য
append_item(char_u *str, char_u *item, পূর্ণসংখ্যা item_len)
{
    পূর্ণসংখ্যা len = (পূর্ণসংখ্যা)STRLEN(str);

    যদি (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, item, (size_t)item_len);
    str[len + item_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    স্থির শূন্য
prepend_item(char_u *str, char_u *item, পূর্ণসংখ্যা item_len)
{
    পূর্ণসংখ্যা len = (পূর্ণসংখ্যা)STRLEN(str);
    পূর্ণসংখ্যা comma = (len > 0) ? 1 : 0;

    mch_memmove(str + item_len + comma, str, (size_t)len + 1);
    mch_memmove(str, item, (size_t)item_len);
    যদি (comma)
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
    স্থির bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    পূর্ণসংখ্যা		flags UNUSED)
{
    char_u  *p;
    char_u  *item_start;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    যদি (vim_strchr(newval, ':') == ফাঁকা && vim_strchr(newval, ',') == ফাঁকা)
	ফেরত false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    যদি (newval_copy == ফাঁকা)
	ফেরত false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    item_start = newval_copy;
    জন্য (;;)
    {
	p = vim_strchr(item_start, ',');
	পূর্ণসংখ্যা item_len = (p == ফাঁকা)
			    ? (পূর্ণসংখ্যা)STRLEN(item_start) : (পূর্ণসংখ্যা)(p - item_start);

	যদি (item_len > 0)
	{
	    char_u *colon = vim_strchr(item_start, ':');
	    যদি (colon != ফাঁকা && colon < item_start + item_len)
	    {
		পূর্ণসংখ্যা keylen = (পূর্ণসংখ্যা)(colon - item_start) + 1;

		যদি (op == OP_ADDING || op == OP_PREPENDING)
		{
		    পূর্ণসংখ্যা old_itemlen;
		    char_u *found = find_চাবি_item(newval, item_start,
						       keylen, &old_itemlen);
		    যদি (found != ফাঁকা)
		    {
			যদি (old_itemlen == item_len
				&& STRNCMP(found, item_start,
							 item_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_চাবি_item(newval, item_start,
							   keylen, found);
			}
			অন্যথা
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_চাবি_item(newval, item_start,
							    keylen, ফাঁকা);
			    যদি (op == OP_PREPENDING)
				prepend_item(newval, item_start, item_len);
			    অন্যথা
				append_item(newval, item_start, item_len);
			}
		    }
		    অন্যথা
		    {
			// New item.
			যদি (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			অন্যথা
			    append_item(newval, item_start, item_len);
		    }
		}
		অন্যথা যদি (op == OP_REMOVING)
		    remove_চাবি_item(newval, item_start, keylen, ফাঁকা);
	    }
	    অন্যথা
	    {
		যদি (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    যদি (found == ফাঁকা)
		    {
			// New item.
			যদি (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			অন্যথা
			    append_item(newval, item_start, item_len);
		    }
		    // else: exact duplicate — do nothing
		}
		অন্যথা যদি (op == OP_REMOVING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    যদি (found != ফাঁকা)
			remove_comma_item(newval, found, item_len);
		}
	    }
	}

	যদি (p == ফাঁকা)
	    বিরতি;
	item_start = p + 1;
    }

    vim_free(newval_copy);

    ফেরত true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    স্থির শূন্য
stropt_remove_dupflags(char_u *newval, পূর্ণসংখ্যা flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    যখন (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	যদি (flags & P_ONECOMMA)
	{
	    যদি (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != ফাঁকা)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		চালু;
	    }
	}
	অন্যথা
	{
	    যদি ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != ফাঁকা)
	    {
		STRMOVE(s, s + 1);
		চালু;
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
    স্থির char_u *
stropt_get_newval(
    পূর্ণসংখ্যা		nextchar,
    পূর্ণসংখ্যা		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    set_op_T    *op_arg,
    পূর্ণসংখ্যা		flags,
    পূর্ণসংখ্যা		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    set_op_T    op = *op_arg;
    char_u	*save_arg = ফাঁকা;
    char_u	*newval;
    char_u	*s = ফাঁকা;
    char_u	whichwrap[80];

    যদি (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    অন্যথা যদি (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    অন্যথা
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	যদি (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	অন্যথা যদি (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	অন্যথা যদি (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	অন্যথা যদি (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_মান(origval, &arg, op, flags);
	যদি (newval == ফাঁকা)
	    যাও done;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	যদি (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    যদি (newval == ফাঁকা)
		যাও done;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	যদি ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	অন্যথা
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    পূর্ণসংখ্যা len = 0;
	    যদি (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (পূর্ণসংখ্যা)STRLEN(newval);
		s = find_dup_item(origval, newval, len, flags);

		// do not add if already there
		যদি ((op == OP_ADDING || op == OP_PREPENDING) && s != ফাঁকা)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		যদি (s == ফাঁকা)
		    s = origval + (পূর্ণসংখ্যা)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    যদি (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    অন্যথা যদি (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	যদি (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

done:
    যদি (save_arg != ফাঁকা)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    ফেরত newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    স্থির পূর্ণসংখ্যা
do_set_option_string(
	পূর্ণসংখ্যা	    opt_idx,
	পূর্ণসংখ্যা	    opt_flags,
	char_u	    **argp,
	পূর্ণসংখ্যা	    nextchar,
	set_op_T    op_arg,
	long_u	    flags,
	পূর্ণসংখ্যা	    cp_val,
	char_u	    *varp_arg,
	অক্ষর	    *errbuf,
	size_t	    errbuflen,
	পূর্ণসংখ্যা	    *মান_checked,
	অক্ষর	    **errmsg)
{
    char_u	*arg = *argp;
    set_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = ফাঁকা; // previous value if *varp
    char_u	*newval;
    char_u	*origval = ফাঁকা;
    char_u	*origval_l = ফাঁকা;
    char_u	*origval_g = ফাঁকা;
#যদি defined(FEAT_EVAL)
    char_u	*saved_origval = ফাঁকা;
    char_u	*saved_origval_l = ফাঁকা;
    char_u	*saved_origval_g = ফাঁকা;
    char_u	*saved_newval = ফাঁকা;
#endif

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    যদি ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((পূর্ণসংখ্যা)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    যদি ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	যদি (((পূর্ণসংখ্যা)options[opt_idx].indir & PV_BOTH)
						  && origval_l == empty_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    যদি (((পূর্ণসংখ্যা)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    অন্যথা
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    যদি (newval == ফাঁকা)
	*(char_u **)(varp) = empty_option;

#যদি defined(FEAT_EVAL)
    যদি (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_চাবি
# endif
		      && origval != ফাঁকা && newval != ফাঁকা)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	যদি (origval_l != ফাঁকা)
	    saved_origval_l = vim_strsave(origval_l);
	যদি (origval_g != ফাঁকা)
	    saved_origval_g = vim_strsave(origval_g);
    }
#endif

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	পূর্ণসংখ্যা	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	যদি ((opt_flags & OPT_MODELINE)
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
			errbuflen, opt_flags, op, মান_checked);

	secure = secure_saved;
    }

#যদি defined(FEAT_EVAL)
    যদি (*errmsg == ফাঁকা)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#endif

    *argp = arg;
    ফেরত *errmsg == ফাঁকা ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    স্থির অক্ষর *
do_set_option_bool(
    পূর্ণসংখ্যা		opt_idx,
    পূর্ণসংখ্যা		opt_flags,
    set_prefix_T prefix,
    long_u	flags,
    char_u	*varp,
    পূর্ণসংখ্যা		nextchar,
    পূর্ণসংখ্যা		afterchar,
    পূর্ণসংখ্যা		cp_val)

{
    varnumber_T	মান;

    যদি (nextchar == '=' || nextchar == ':')
	ফেরত e_invalid_argument;
    যদি (opt_idx < 0 || varp == ফাঁকা)
	ফেরত ফাঁকা;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    যদি (nextchar == '!')
	মান = *(পূর্ণসংখ্যা *)(varp) ^ 1;
    অন্যথা যদি (nextchar == '&')
	মান = (পূর্ণসংখ্যা)(long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    অন্যথা যদি (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	যদি ((পূর্ণসংখ্যা *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    মান = -1;
	অন্যথা যদি ((পূর্ণসংখ্যা *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    মান = -1;
#ifdef HAVE_FSYNC
	অন্যথা যদি ((পূর্ণসংখ্যা *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    মান = -1;
#endif
	অন্যথা
	    মান = *(পূর্ণসংখ্যা *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    অন্যথা
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	যদি (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    ফেরত e_trailing_characters;
	যদি (prefix == PREFIX_INV)
	    মান = *(পূর্ণসংখ্যা *)(varp) ^ 1;
	অন্যথা
	    মান = prefix == PREFIX_NO ? 0 : 1;
    }

    ফেরত set_bool_option(opt_idx, varp, (পূর্ণসংখ্যা)মান, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    স্থির অক্ষর *
do_set_option_numeric(
    পূর্ণসংখ্যা		opt_idx,
    পূর্ণসংখ্যা		opt_flags,
    char_u	**argp,
    পূর্ণসংখ্যা		nextchar,
    set_op_T	op,
    long_u	flags,
    পূর্ণসংখ্যা		cp_val,
    char_u	*varp,
    অক্ষর	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		মান;
    পূর্ণসংখ্যা			i;
    অক্ষর		*errmsg = ফাঁকা;

    যদি (opt_idx < 0 || varp == ফাঁকা)
	ফেরত ফাঁকা;  // "cannot happen"
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
    যদি (nextchar == '&')
	মান = (long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    অন্যথা যদি (nextchar == '<')
    {
	যদি ((long *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    মান = NO_LOCAL_UNDOLEVEL;
	অন্যথা যদি (opt_flags == OPT_LOCAL
		    && ((long *)varp == &curwin->w_p_siso
		     || (long *)varp == &curwin->w_p_so
		     || (long *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    মান = -1;
	অন্যথা
	    মান = *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    অন্যথা যদি (((long *)varp == &p_wc || (long *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	মান = string_to_চাবি(arg, FALSE);
	যদি (মান == 0)
	{
	    errmsg = e_invalid_argument;
	    যাও skip;
	}
    }
    অন্যথা যদি (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, ফাঁকা, &i, STR2NR_ALL, &মান, ফাঁকা, 0, TRUE, ফাঁকা);
	যদি (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_সংখ্যা_required_after_equal;
	    যাও skip;
	}
    }
    অন্যথা
    {
	errmsg = e_সংখ্যা_required_after_equal;
	যাও skip;
    }

    যদি (op == OP_ADDING)
	মান = *(long *)varp + মান;
    অন্যথা যদি (op == OP_PREPENDING)
	মান = *(long *)varp * মান;
    অন্যথা যদি (op == OP_REMOVING)
	মান = *(long *)varp - মান;

    যদি ((long *)varp == &curwin->w_p_sop && মান < -1)
    {
	errmsg = e_invalid_argument;
	যাও skip;
    }

    errmsg = set_অঙ্ক_option(opt_idx, varp, মান, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    ফেরত errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    স্থির অক্ষর *
do_set_option_keycode(char_u **argp, char_u *চাবি_name, পূর্ণসংখ্যা nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    যদি (nextchar == '&')
    {
	যদি (add_termcap_entry(চাবি_name, TRUE) == FAIL)
	    ফেরত e_not_found_in_termcap;
    }
    অন্যথা
    {
	++arg; // jump to after the '=' or ':'
	জন্য (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    যদি (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	add_termcode(চাবি_name, arg, FALSE);
	*p = nextchar;
    }
    যদি (full_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    ফেরত ফাঁকা;
}

/*
 * Set an option to a new value.
 */
    স্থির অক্ষর *
do_set_option_মান(
    পূর্ণসংখ্যা		opt_idx,
    পূর্ণসংখ্যা		opt_flags,
    char_u	**argp,
    set_prefix_T prefix,
    set_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*চাবি_name,
    পূর্ণসংখ্যা		nextchar,
    পূর্ণসংখ্যা		afterchar,
    পূর্ণসংখ্যা		cp_val,
    পূর্ণসংখ্যা		*stopopteval,
    অক্ষর	*errbuf,
    size_t	errbuflen)
{
    পূর্ণসংখ্যা		মান_checked = FALSE;
    অক্ষর	*errmsg = ফাঁকা;
    char_u	*arg = *argp;

    যদি (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_set_option_bool(opt_idx, opt_flags, prefix, flags, varp,
						nextchar, afterchar, cp_val);
	যদি (errmsg != ফাঁকা)
	    যাও skip;
    }
    অন্যথা
    {
	// numeric or string option
	যদি (vim_strchr((char_u *)"=:&<", nextchar) == ফাঁকা
		|| prefix != PREFIX_NONE)
	{
	    errmsg = e_invalid_argument;
	    যাও skip;
	}

	যদি (flags & P_অঙ্ক)
	{
	    // numeric option
	    errmsg = do_set_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    যদি (errmsg != ফাঁকা)
		যাও skip;
	}
	অন্যথা যদি (opt_idx >= 0)
	{
	    // string option
	    যদি (do_set_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&মান_checked, &errmsg) == FAIL)
	    {
		যদি (errmsg != ফাঁকা)
		    যাও skip;
		*stopopteval = TRUE;
		যাও skip;
	    }
	}
	অন্যথা
	{
	    // key code option
	    errmsg = do_set_option_keycode(&arg, চাবি_name, nextchar);
	    যদি (errmsg != ফাঁকা)
		যাও skip;
	}
    }

    যদি (opt_idx >= 0)
	did_set_option(opt_idx, opt_flags, op == OP_NONE, মান_checked);

skip:
    *argp = arg;
    ফেরত errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    স্থির অক্ষর *
do_set_option(
    পূর্ণসংখ্যা		opt_flags,
    char_u	**argp,
    char_u	*arg_start,
    char_u	**startarg,
    পূর্ণসংখ্যা		*did_show,
    পূর্ণসংখ্যা		*stopopteval,
    অক্ষর	*errbuf,
    size_t	errbuflen)
{
    পূর্ণসংখ্যা		opt_idx;
    char_u	*arg;
    set_prefix_T prefix;	// no prefix, "no" prefix or "inv" prefix
    set_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	চাবি_name[2];
    পূর্ণসংখ্যা		nextchar;	// next non-white char after option name
    পূর্ণসংখ্যা		afterchar;	// character just after option name
    অক্ষর	*errmsg = ফাঁকা;
    পূর্ণসংখ্যা		চাবি;
    পূর্ণসংখ্যা		len;

    prefix = get_option_prefix(argp);
    arg = *argp;

    // find end of name
    চাবি = 0;
    যদি (parse_option_name(arg, &opt_idx, &len, &চাবি) == FAIL)
	ফেরত e_invalid_argument;

    // remember character after option name
    afterchar = arg[len];

    যদি (in_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	যদি (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != ফাঁকা
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    যাও skip;
	}
    }
    অন্যথা
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	যখন (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    যদি (op != OP_NONE)
	len++;

    nextchar = arg[len];

    যদি (opt_idx == -1 && চাবি == 0)	// found a mismatch: skip
    {
	যদি (in_vim9script() && arg > arg_start
		&& vim_strchr((char_u *)"!&<", *arg) != ফাঁকা)
	    errmsg = e_no_white_space_allowed_between_option_and;
	অন্যথা
	    errmsg = e_unknown_option;
	যাও skip;
    }

    যদি (opt_idx >= 0)
    {
	যদি (options[opt_idx].var == ফাঁকা)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    যদি (vim_strchr((char_u *)"=:!&<", nextchar) == ফাঁকা
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    যাও skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    অন্যথা
    {
	flags = P_STRING;
	varp = ফাঁকা;
	যদি (চাবি < 0)
	{
	    চাবি_name[0] = KEY2TERMCAP0(চাবি);
	    চাবি_name[1] = KEY2TERMCAP1(চাবি);
	}
	অন্যথা
	{
	    চাবি_name[0] = KS_চাবি;
	    চাবি_name[1] = (চাবি & 0xff);
	}
    }

    // Make sure the option value can be changed.
    যদি (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, prefix) == FAIL)
	যাও skip;

    পূর্ণসংখ্যা cp_val = p_cp;
    যদি (vim_strchr((char_u *)"?=:!&<", nextchar) != ফাঁকা)
    {
	arg += len;
	যদি (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    যদি (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    অন্যথা		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	যদি (vim_strchr((char_u *)"?!&<", nextchar) != ফাঁকা
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    যাও skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    যদি (nextchar == '?'
	    || (prefix == PREFIX_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == ফাঁকা
		&& !(flags & P_BOOL)))
    {
	// print value
	যদি (*did_show)
	    বার্তা_putchar('\n');	    // cursor below last one
	অন্যথা
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	যদি (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ifdef FEAT_EVAL
	    যদি (p_verbose > 0)
	    {
		// Mention where the option was last set.
		যদি (varp == options[opt_idx].var)
		    last_set_বার্তা(options[opt_idx].script_ctx);
		অন্যথা যদি ((পূর্ণসংখ্যা)options[opt_idx].indir & PV_WIN)
		    last_set_বার্তা(curwin->w_p_script_ctx[
			    (পূর্ণসংখ্যা)options[opt_idx].indir & PV_MASK]);
		অন্যথা যদি ((পূর্ণসংখ্যা)options[opt_idx].indir & PV_BUF)
		    last_set_বার্তা(curbuf->b_p_script_ctx[
			    (পূর্ণসংখ্যা)options[opt_idx].indir & PV_MASK]);
	    }
#endif
	}
	অন্যথা
	{
	    char_u	    *p;

	    p = find_termcode(চাবি_name);
	    যদি (p == ফাঁকা)
	    {
		errmsg = e_চাবি_code_not_set;
		যাও skip;
	    }
	    অন্যথা
		(শূন্য)show_one_termcode(চাবি_name, p, TRUE);
	}
	যদি (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    অন্যথা
    {
	errmsg = do_set_option_মান(opt_idx, opt_flags, &arg, prefix, op,
					flags, varp, চাবি_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    ফেরত errmsg;
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
    পূর্ণসংখ্যা
do_set(
    char_u	*arg_start,	// option string (may be written to!)
    পূর্ণসংখ্যা		opt_flags)
{
    char_u	*arg = arg_start;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		did_show = FALSE;   // already showed one value

    যদি (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	যাও theend;
    }

    যখন (*arg != NUL)		// loop to process all options
    {
	যদি (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    যদি (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		set_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    অন্যথা
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	অন্যথা যদি (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা		stopopteval = FALSE;
	    অক্ষর	*errmsg = ফাঁকা;
	    অক্ষর	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_set_option(opt_flags, &arg, arg_start, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    যদি (stopopteval)
		বিরতি;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    জন্য (i = 0; i < 2 ; ++i)
	    {
		যখন (*arg != NUL && !VIM_ISWHITE(*arg))
		    যদি (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		যদি (*arg != '=')
		    বিরতি;
	    }

	    যদি (errmsg != ফাঁকা)
	    {
		i = vim_snprintf((অক্ষর *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		যদি (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((অক্ষর *)IObuff);	// show error highlighted
		--no_wait_return;

		ফেরত FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    যদি (silent_mode && did_show)
    {
	// After displaying option values in silent mode.
	silent_mode = FALSE;
	info_message = TRUE;	// use mch_msg(), not mch_errmsg()
	বার্তা_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = TRUE;
	info_message = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    ফেরত OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    শূন্য
did_set_option(
    পূর্ণসংখ্যা	    opt_idx,
    পূর্ণসংখ্যা	    opt_flags,	    // possibly with OPT_MODELINE
    পূর্ণসংখ্যা	    new_মান_2,	    // value was replaced completely
    পূর্ণসংখ্যা	    মান_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = ফাঁকা;
    পূর্ণসংখ্যা		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_SET;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    যদি (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    যদি (!মান_checked && (secure
#ifdef HAVE_SANDBOX
	    || sandbox != 0
#endif
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	যদি (flagsp_local != ফাঁকা)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    অন্যথা যদি (new_মান_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	যদি (flagsp_local != ফাঁকা)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    পূর্ণসংখ্যা
string_to_চাবি(char_u *arg, পূর্ণসংখ্যা multi_byte)
{
    যদি (*arg == '<' && arg[1])
	ফেরত find_চাবি_option(arg + 1, TRUE);
    যদি (*arg == '^' && arg[1])
    {
	পূর্ণসংখ্যা চাবি = Ctrl_chr(arg[1]);
	যদি (চাবি == 0)		// ^@ is <Nul>
	    চাবি = K_ZERO;
	ফেরত চাবি;
    }
    যদি (multi_byte)
	ফেরত PTR2CHAR(arg);
    ফেরত *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    শূন্য
did_set_title(শূন্য)
{
    যদি (starting != NO_SCREEN
#ifdef FEAT_GUI
	    && !gui.starting
#endif
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    শূন্য
set_options_bin(
    পূর্ণসংখ্যা		oldval,
    পূর্ণসংখ্যা		newval,
    পূর্ণসংখ্যা		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    যদি (newval)
    {
	যদি (!oldval)		// switched on
	{
	    যদি (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    যদি (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	যদি (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	যদি (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    অন্যথা যদি (oldval)		// switched off
    {
	যদি (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	যদি (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#যদি defined(FEAT_EVAL)
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
    স্থির char_u *
option_expand(পূর্ণসংখ্যা opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    যদি (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == ফাঁকা)
	ফেরত ফাঁকা;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    যদি (val != ফাঁকা && STRLEN(val) > MAXPATHL)
	ফেরত ফাঁকা;

    যদি (val == ফাঁকা)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    পূর্ণসংখ্যা esc = var == &p_tags || var == &p_path;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : ফাঁকা, FALSE,
#ifdef FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#endif
				  ফাঁকা);
    যদি (STRCMP(NameBuff, val) == 0)   // they are the same
	ফেরত ফাঁকা;

    ফেরত NameBuff;
}

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    স্থির শূন্য
didset_options(শূন্য)
{
    // initialize the table for 'iskeyword' et.al.
    (শূন্য)init_chartab();

    didset_string_options();

#ifdef FEAT_SPELL
    (শূন্য)spell_check_msm();
    (শূন্য)spell_check_sps();
    (শূন্য)compile_cap_prog(curwin->w_s);
    (শূন্য)did_set_spell_option();
#endif
    // set cedit_key
    (শূন্য)did_set_cedit(ফাঁকা);
#ifdef FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_set_breakat(ফাঁকা);
#endif
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    স্থির শূন্য
didset_options2(শূন্য)
{
    // Initialize the highlight_attr[] table.
    (শূন্য)highlight_changed();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (শূন্য)set_listchars_option(curwin, curwin->w_p_lcs, TRUE, ফাঁকা, 0);

    // Parse default for 'fillchars'.
    (শূন্য)set_fillchars_option(curwin, curwin->w_p_fcs, TRUE, ফাঁকা, 0);

#ifdef FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (শূন্য)did_set_clipboard(ফাঁকা);
#endif
#ifdef FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (শূন্য)tabstop_set(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (শূন্য)tabstop_set(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#endif
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    শূন্য
check_options(শূন্য)
{
    পূর্ণসংখ্যা		opt_idx;

    জন্য (opt_idx = 0; options[opt_idx].fullname != ফাঁকা; opt_idx++)
	যদি ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != ফাঁকা)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    পূর্ণসংখ্যা
get_term_opt_idx(char_u **p)
{
    পূর্ণসংখ্যা opt_idx;

    জন্য (opt_idx = 1; options[opt_idx].fullname != ফাঁকা; opt_idx++)
	যদি (options[opt_idx].var == (char_u *)p)
	    ফেরত opt_idx;
    ফেরত -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    পূর্ণসংখ্যা
set_term_option_alloced(char_u **p)
{
    পূর্ণসংখ্যা		opt_idx = get_term_opt_idx(p);

    যদি (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    ফেরত opt_idx;
}

#যদি defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    পূর্ণসংখ্যা
was_set_insecurely(win_T *wp, char_u *opt, পূর্ণসংখ্যা opt_flags)
{
    পূর্ণসংখ্যা	    idx = findoption(opt);
    long_u  *flagp;

    যদি (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	ফেরত (*flagp & P_INSECURE) != 0;
    }
    internal_error("was_set_insecurely()");
    ফেরত -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    স্থির long_u *
insecure_flag(win_T *wp, পূর্ণসংখ্যা opt_idx, পূর্ণসংখ্যা opt_flags)
{
    যদি (opt_flags & OPT_LOCAL)
	নির্বাচন ((পূর্ণসংখ্যা)options[opt_idx].indir)
	{
	    ক্ষেত্র PV_WRAP:	ফেরত &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    ক্ষেত্র PV_STL:	ফেরত &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    ক্ষেত্র PV_FDE:	ফেরত &wp->w_p_fde_flags;
	    ক্ষেত্র PV_FDT:	ফেরত &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    ক্ষেত্র PV_BEXPR:	ফেরত &wp->w_বাফার->b_p_bexpr_flags;
#  endif
	    ক্ষেত্র PV_INDE:	ফেরত &wp->w_বাফার->b_p_inde_flags;
	    ক্ষেত্র PV_FEX:	ফেরত &wp->w_বাফার->b_p_fex_flags;
#  ifdef FEAT_FIND_ID
	    ক্ষেত্র PV_INEX:	ফেরত &wp->w_বাফার->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    ক্ষেত্র PV_CPT:	ফেরত &wp->w_বাফার->b_p_cpt_flags;
#  endif
# endif
	}
    অন্যথা
	// For global value of window-local options, use flags in w_allbuf_opt.
	নির্বাচন ((পূর্ণসংখ্যা)options[opt_idx].indir)
	{
	    ক্ষেত্র PV_WRAP:	ফেরত &wp->w_allbuf_opt.wo_wrap_flags;
# যদি defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    ক্ষেত্র PV_FDE:	ফেরত &wp->w_allbuf_opt.wo_fde_flags;
	    ক্ষেত্র PV_FDT:	ফেরত &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    ফেরত &options[opt_idx].flags;
}
#endif

/*
 * Redraw the window title and/or tab page text later.
 */
    শূন্য
redraw_titles(শূন্য)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#যদি defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    পূর্ণসংখ্যা
valid_name(char_u *val, অক্ষর *allowed)
{
    char_u *s;

    জন্য (s = val; *s != NUL; ++s)
	যদি (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == ফাঁকা)
	    ফেরত FALSE;
    ফেরত TRUE;
}

#যদি defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    শূন্য
set_option_sctx_idx(পূর্ণসংখ্যা opt_idx, পূর্ণসংখ্যা opt_flags, sctx_T script_ctx)
{
    পূর্ণসংখ্যা		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    পূর্ণসংখ্যা		indir = (পূর্ণসংখ্যা)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    যদি (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    যদি (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    যদি (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    যদি (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    যদি (both || (opt_flags & OPT_LOCAL))
    {
	যদি (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	অন্যথা যদি (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	    যদি (both)
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
is_option_মান_vim9(পূর্ণসংখ্যা opt_idx, পূর্ণসংখ্যা opt_flags)
{
    পূর্ণসংখ্যা		indir = (পূর্ণসংখ্যা)options[opt_idx].indir;
    sctx_T	*sctx = ফাঁকা;

    যদি ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    যদি ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	যদি (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_MASK];
	অন্যথা যদি (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_MASK];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    ফেরত !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_version >= SCRIPT_VERSION_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(অক্ষর *name)
{
    পূর্ণসংখ্যা idx = findoption((char_u *)name);

    যদি (idx >= 0)
	ফেরত &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    ফেরত ফাঁকা;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    শূন্য
set_term_option_sctx_idx(অক্ষর *name, পূর্ণসংখ্যা opt_idx)
{
    char_u  buf[5];
    পূর্ণসংখ্যা	    idx;

    যদি (name == ফাঁকা)
	idx = opt_idx;
    অন্যথা
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    যদি (idx >= 0)
	set_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    স্থির শূন্য
apply_optionset_autocmd(
	পূর্ণসংখ্যা	opt_idx,
	long	opt_flags,
	long	oldval,
	long	oldval_g,
	long	newval,
	অক্ষর	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    যদি (starting || errmsg != ফাঁকা || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	ফেরত;

    len = vim_snprintf_safelen((অক্ষর *)buf_new, আকার(buf_new), "%ld", newval);
    set_vim_var_string(VV_OPTION_NEW, buf_new, (পূর্ণসংখ্যা)len);
    len = vim_snprintf_safelen((অক্ষর *)buf_type, আকার(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (পূর্ণসংখ্যা)len);

    buf_oldlen = vim_snprintf_safelen((অক্ষর *)buf_old, আকার(buf_old), "%ld", oldval);
    set_vim_var_string(VV_OPTION_OLD, buf_old, (পূর্ণসংখ্যা)buf_oldlen);

    যদি (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (পূর্ণসংখ্যা)buf_oldlen);
    }
    যদি (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (পূর্ণসংখ্যা)buf_oldlen);
    }
    যদি ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((অক্ষর *)buf_old_global,
	    আকার(buf_old_global), "%ld", oldval_g);

	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (পূর্ণসংখ্যা)buf_oldlen);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (পূর্ণসংখ্যা)buf_old_globallen);
    }
    যদি (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (পূর্ণসংখ্যা)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    ফাঁকা, FALSE, ফাঁকা);
    reset_v_option_vars();
}
#endif

#যদি defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    অক্ষর *
did_set_arabic(optset_T *args UNUSED)
{
    অক্ষর *errmsg = ফাঁকা;

    যদি (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	যদি (!p_tbidi)
	{
	    // set rightleft mode
	    যদি (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		changed_window_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    যদি (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	যদি (STRCMP(p_enc, "utf-8") != 0)
	{
	    স্থির অক্ষর *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    বার্তা_source(HL_ATTR(HLF_W));
	    বার্তা_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = set_option_মান((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    অন্যথা
    {
	// 'arabic' is reset, handle various sub-settings.
	যদি (!p_tbidi)
	{
	    // reset rightleft mode
	    যদি (curwin->w_p_rl)
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
	curbuf->b_p_imsearch = B_IMODE_USE_ঢোকাও;
# endif
    }

    ফেরত errmsg;
}
#endif

#যদি defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    অক্ষর *
did_set_autochdir(optset_T *args UNUSED)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    ফেরত ফাঁকা;
}
#endif

#যদি defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    অক্ষর *
did_set_ballooneval(optset_T *args)
{
    যদি (balloonEvalForTerm)
	ফেরত ফাঁকা;

    যদি (p_beval && !args->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    অন্যথা যদি (!p_beval && args->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    ফেরত ফাঁকা;
}
#endif

#যদি defined(FEAT_BEVAL_TERM)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    অক্ষর *
did_set_balloonevalterm(optset_T *args UNUSED)
{
    mch_bevalterm_changed();
    ফেরত ফাঁকা;
}
#endif

/*
 * Process the updated 'binary' option value.
 */
    অক্ষর *
did_set_দ্বিমিক(optset_T *args)
{
    // when 'bin' is set also set some other options
    set_options_bin(args->os_oldval.boolean, curbuf->b_p_bin, args->os_flags);
    redraw_titles();

    ফেরত ফাঁকা;
}

/*
 * Process the updated 'buflisted' option value.
 */
    অক্ষর *
did_set_buflisted(optset_T *args)
{
    // when 'buflisted' changes, trigger autocommands
    যদি (args->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						ফাঁকা, ফাঁকা, TRUE, curbuf);
    ফেরত ফাঁকা;
}

/*
 * Process the new 'cmdheight' option value.
 */
    অক্ষর *
did_set_cmdheight(optset_T *args)
{
    long old_মান = args->os_oldval.সংখ্যা;
    অক্ষর *errmsg = ফাঁকা;

    // if p_ch changed value, change the command line height
    যদি (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_argument_must_be_positive;
	p_ch = MIN_CMDHEIGHT;
    }
    যদি (p_ch > Rows - min_rows() + MIN_CMDHEIGHT)
	p_ch = Rows - min_rows() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    যদি ((p_ch != old_মান
		|| tabline_height() + topframe->fr_height != Rows - p_ch)
	    && full_screen
#ifdef FEAT_GUI
	    && !gui.starting
#endif
       )
	command_height();

    ফেরত errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    অক্ষর *
did_set_compatible(optset_T *args UNUSED)
{
    compatible_set();
    ফেরত ফাঁকা;
}

#যদি defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    অক্ষর *
did_set_conceallevel(optset_T *args UNUSED)
{
    অক্ষর *errmsg = ফাঁকা;

    যদি (curwin->w_p_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_cole = 0;
    }
    অন্যথা যদি (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_cole = 3;
    }
    যদি (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    অন্যথা যদি (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    ফেরত errmsg;
}
#endif

#যদি defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    অক্ষর *
did_set_diff(optset_T *args UNUSED)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    যদি (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    ফেরত ফাঁকা;
}
#endif

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    অক্ষর *
did_set_eof_eol_fixeol_bomb(optset_T *args UNUSED)
{
    // redraw the window title and tab page text
    redraw_titles();
    ফেরত ফাঁকা;
}

/*
 * Process the updated 'equalalways' option value.
 */
    অক্ষর *
did_set_equalalways(optset_T *args)
{
    যদি (p_ea && !args->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    ফেরত ফাঁকা;
}

#যদি defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    অক্ষর *
did_set_foldcolumn(optset_T *args UNUSED)
{
    অক্ষর *errmsg = ফাঁকা;

    যদি (curwin->w_p_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_fdc = 0;
    }
    অন্যথা যদি (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_fdc = 12;
    }
    যদি (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    অন্যথা যদি (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    ফেরত errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    অক্ষর *
did_set_foldlevel(optset_T *args UNUSED)
{
    যদি (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    ফেরত ফাঁকা;
}

/*
 * Process the new 'foldminlines' option value.
 */
    অক্ষর *
did_set_foldminlines(optset_T *args UNUSED)
{
    foldUpdateAll(curwin);
    ফেরত ফাঁকা;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    অক্ষর *
did_set_foldnestmax(optset_T *args UNUSED)
{
    যদি (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    ফেরত ফাঁকা;
}
#endif

#যদি defined(FEAT_অনুসন্ধান_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    অক্ষর *
did_set_hlsearch(optset_T *args UNUSED)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    set_no_hlsearch(FALSE);
    ফেরত ফাঁকা;
}
#endif

/*
 * Process the updated 'ignorecase' option value.
 */
    অক্ষর *
did_set_ignorecase(optset_T *args UNUSED)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    যদি (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    ফেরত ফাঁকা;
}

#যদি defined(HAVE_প্রবেশ_METHOD)
/*
 * Process the updated 'imdisable' option value.
 */
    অক্ষর *
did_set_imdisable(optset_T *args UNUSED)
{
    // Only de-activate it here, it will be enabled when changing mode.
    যদি (p_imdisable)
	im_set_active(FALSE);
    অন্যথা যদি (State & MODE_ঢোকাও)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
    ফেরত ফাঁকা;
}
#endif

/*
 * Process the new 'iminsert' option value.
 */
    অক্ষর *
did_set_iminsert(optset_T *args UNUSED)
{
    অক্ষর *errmsg = ফাঁকা;

    যদি (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    যদি (termcap_active)	// don't do this in the alternate screen
	showmode();
#যদি defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif

    ফেরত errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    অক্ষর *
did_set_imsearch(optset_T *args UNUSED)
{
    অক্ষর *errmsg = ফাঁকা;

    যদি (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    ফেরত errmsg;
}

#যদি defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    অক্ষর *
did_set_imstyle(optset_T *args UNUSED)
{
    অক্ষর *errmsg = ফাঁকা;

    যদি (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_argument;

    ফেরত errmsg;
}
#endif

/*
 * Process the updated 'insertmode' option value.
 */
    অক্ষর *
did_set_insertmode(optset_T *args)
{
    // when 'insertmode' is set from an autocommand need to do work here
    যদি (p_im)
    {
	যদি ((State & MODE_ঢোকাও) == 0)
	    need_start_insertmode = TRUE;
	stop_ঢোকাও_mode = FALSE;
    }
    // only reset if it was set previously
    অন্যথা যদি (args->os_oldval.boolean)
    {
	need_start_insertmode = FALSE;
	stop_ঢোকাও_mode = TRUE;
	যদি (restart_edit != 0 && mode_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    ফেরত ফাঁকা;
}

#যদি defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    অক্ষর *
did_set_langnoremap(optset_T *args UNUSED)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    ফেরত ফাঁকা;
}

/*
 * Process the updated 'langremap' option value.
 */
    অক্ষর *
did_set_langremap(optset_T *args UNUSED)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    ফেরত ফাঁকা;
}
#endif

/*
 * Process the new 'laststatus' option value.
 */
    অক্ষর *
did_set_laststatus(optset_T *args UNUSED)
{
    last_status(FALSE);	// (re)set last window status line
    ফেরত ফাঁকা;
}

#যদি defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    অক্ষর *
did_set_linespace(optset_T *args UNUSED)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    যদি (gui.in_use && gui_mch_adjust_charheight() == OK)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    ফেরত ফাঁকা;
}
#endif

/*
 * Process the updated 'lisp' option value.
 */
    অক্ষর *
did_set_lisp(optset_T *args UNUSED)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (শূন্য)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    ফেরত ফাঁকা;
}

/*
 * Process the new 'maxcombine' option value.
 */
    অক্ষর *
did_set_maxcombine(optset_T *args UNUSED)
{
    যদি (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    অন্যথা যদি (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    ফেরত ফাঁকা;
}

/*
 * Process the updated 'modifiable' option value.
 */
    অক্ষর *
did_set_modifiable(optset_T *args UNUSED)
{
    // when 'modifiable' is changed, redraw the window title

#ifdef FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    যদি (curbuf->b_p_ma && (term_in_normal_mode(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_term != ফাঁকা && !term_is_finished(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	ফেরত e_cannot_make_terminal_with_running_job_modifiable;
    }
#endif
    redraw_titles();

    ফেরত ফাঁকা;
}

/*
 * Process the updated 'modified' option value.
 */
    অক্ষর *
did_set_modified(optset_T *args)
{
    যদি (!args->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_set = !!args->os_newval.boolean;
    ফেরত ফাঁকা;
}

#যদি defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    অক্ষর *
did_set_mousehide(optset_T *args UNUSED)
{
    যদি (!p_mh)
	gui_mch_mousehide(FALSE);
    ফেরত ফাঁকা;
}
#endif

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    অক্ষর *
did_set_সংখ্যা_relativenumber(optset_T *args UNUSED)
{
#যদি defined(FEAT_SIGNS) && defined(FEAT_GUI)
    যদি (gui.in_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != ফাঁকা)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	যদি (!(curwin->w_p_nu && ((পূর্ণসংখ্যা *)args->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#endif
    ফেরত ফাঁকা;
}

#যদি defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    অক্ষর *
did_set_numberwidth(optset_T *args UNUSED)
{
    অক্ষর *errmsg = ফাঁকা;

    // 'numberwidth' must be positive
    যদি (curwin->w_p_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_nuw = 1;
    }
    অন্যথা যদি (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_nuw = 20;
    }
    যদি (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    অন্যথা যদি (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_সারি_গুনতি = 0; // trigger a redraw

    ফেরত errmsg;
}
#endif

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    অক্ষর *
did_set_osctimeoutlen(optset_T *args)
{
    যদি (p_ost < 0)
    {
	p_ost = args->os_oldval.সংখ্যা;
	ফেরত e_argument_must_be_positive;
    }

    ফেরত ফাঁকা;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    অক্ষর *
did_set_paste(optset_T *args UNUSED)
{
    স্থির পূর্ণসংখ্যা	old_p_paste = FALSE;
    স্থির পূর্ণসংখ্যা	save_sm = 0;
    স্থির পূর্ণসংখ্যা	save_sta = 0;
    স্থির পূর্ণসংখ্যা	save_ru = 0;
#ifdef FEAT_RIGHTLEFT
    স্থির পূর্ণসংখ্যা	save_ri = 0;
    স্থির পূর্ণসংখ্যা	save_hkmap = 0;
#endif
    buf_T	*buf;

    যদি (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	যদি (!old_p_paste)
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
		যদি (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != empty_option
					   ? vim_strsave(buf->b_p_vsts) : ফাঁকা;
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
	    যদি (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != empty_option
						  ? vim_strsave(p_vsts) : ফাঁকা;
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
	    যদি (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = empty_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#endif
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	যদি (p_ru)
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
	যদি (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = empty_option;
#endif
    }

    // Paste switched from on to off: Restore saved values.
    অন্যথা যদি (old_p_paste)
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
	    যদি (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : empty_option;
	    vim_free(buf->b_p_vsts_array);
	    যদি (buf->b_p_vsts && buf->b_p_vsts != empty_option)
		(শূন্য)tabstop_set(buf->b_p_vsts, &buf->b_p_vsts_array);
	    অন্যথা
		buf->b_p_vsts_array = ফাঁকা;
#endif
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	যদি (p_ru != save_ru)
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
	যদি (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : empty_option;
#endif
    }

    old_p_paste = p_paste;

#যদি defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#endif

    ফেরত ফাঁকা;
}


#ifdef FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    অক্ষর *
did_set_previewwindow(optset_T *args UNUSED)
{
    যদি (!curwin->w_p_pvw)
	ফেরত ফাঁকা;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	যদি (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    ফেরত e_preview_window_already_exists;
	}

    ফেরত ফাঁকা;
}
#endif

#যদি defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    অক্ষর *
did_set_pyxversion(optset_T *args UNUSED)
{
    অক্ষর *errmsg = ফাঁকা;

    যদি (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_argument;

    ফেরত errmsg;
}
#endif

/*
 * Process the updated 'readonly' option value.
 */
    অক্ষর *
did_set_readonly(optset_T *args)
{
    // when 'readonly' is reset globally, also reset readonlymode
    যদি (!curbuf->b_p_ro && (args->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    যদি (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    ফেরত ফাঁকা;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    অক্ষর *
did_set_scrollbind(optset_T *args UNUSED)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    যদি (!curwin->w_p_scb)
	ফেরত ফাঁকা;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    ফেরত ফাঁকা;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    অক্ষর *
did_set_maxsearchcount(optset_T *args UNUSED)
{
    অক্ষর	*errmsg = ফাঁকা;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#define MAX_অনুসন্ধান_গুনতি 9999

    যদি (p_msc <= 0)
	errmsg = e_argument_must_be_positive;
    অন্যথা যদি (p_msc > MAX_অনুসন্ধান_গুনতি)
	errmsg = e_invalid_argument;

    যদি (errmsg != ফাঁকা)
	p_msc = 99;

    ফেরত errmsg;
#undef MAX_অনুসন্ধান_গুনতি
}

#যদি defined(BACKSLASH_IN_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    অক্ষর *
did_set_shellslash(optset_T *args UNUSED)
{
    যদি (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    অন্যথা
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
    ফেরত ফাঁকা;
}
#endif

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    অক্ষর *
did_set_shiftwidth_tabstop(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    অক্ষর	*errmsg = ফাঁকা;

    যদি (curbuf->b_p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_ts = 8;
    }
    অন্যথা যদি (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_ts = 8;
    }
    যদি (p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ts = 8;
    }
    অন্যথা যদি (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	p_ts = 8;
    }

    যদি (curbuf->b_p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_গুনতি(curbuf->b_p_vts_array) > 0
		       ? tabstop_first(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#অন্যথা
	curbuf->b_p_sw = curbuf->b_p_ts;
#endif
    }
    যদি (p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_গুনতি(curbuf->b_p_vts_array) > 0
	     ? tabstop_first(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#অন্যথা
	p_sw = curbuf->b_p_ts;
#endif
    }

#ifdef FEAT_FOLDING
    যদি (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#endif
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    যদি (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    ফেরত errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    অক্ষর *
did_set_showtabline(optset_T *args UNUSED)
{
    shell_new_rows();	// recompute window positions and heights
    ফেরত ফাঁকা;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    অক্ষর *
did_set_smoothscroll(optset_T *args UNUSED)
{
    যদি (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    ফেরত ফাঁকা;
}

#যদি defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    অক্ষর *
did_set_spell(optset_T *args UNUSED)
{
    যদি (curwin->w_p_spell)
	ফেরত parse_spelllang(curwin);

    ফেরত ফাঁকা;
}
#endif

/*
 * Process the updated 'swapfile' option value.
 */
    অক্ষর *
did_set_swapfile(optset_T *args UNUSED)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    যদি (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    অন্যথা
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    ফেরত ফাঁকা;
}

#যদি defined(FEAT_TERMGUICOLORS)
    অক্ষর *
did_set_termguicolors(optset_T *args UNUSED)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    যদি (
#  ifdef VIMDLL
	    !gui.in_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_set = TRUE;
	p_tgc = 0;
	ফেরত e_24_bit_colors_are_not_supported_on_this_environment;
    }
    যদি (is_term_win32())
	swap_tcap();
# endif
# ifdef FEAT_GUI
    যদি (!gui.in_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    যদি (is_term_win32())
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

    ফেরত ফাঁকা;
}
#endif

    অক্ষর *
did_set_termsync(optset_T *args UNUSED)
{
    যদি (!p_tsy)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);
    ফেরত ফাঁকা;
}

#যদি defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    অক্ষর *
did_set_termwinscroll(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    অক্ষর	*errmsg = ফাঁকা;

    যদি (*pp < 1)
    {
	errmsg = e_argument_must_be_positive;
	*pp = 1;
    }

    ফেরত errmsg;
}
#endif

/*
 * Process the updated 'terse' option value.
 */
    অক্ষর *
did_set_terse(optset_T *args UNUSED)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_অনুসন্ধান);

    // insert 's' in p_shm
    যদি (p_terse && p == ফাঁকা)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    অন্যথা যদি (!p_terse && p != ফাঁকা)
	STRMOVE(p, p + 1);
    ফেরত ফাঁকা;
}

/*
 * Process the updated 'textauto' option value.
 */
    অক্ষর *
did_set_textauto(optset_T *args)
{
    // when 'textauto' is set or reset also change 'fileformats'
    set_string_option_direct((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | args->os_flags, 0);

    ফেরত ফাঁকা;
}

/*
 * Process the updated 'textmode' option value.
 */
    অক্ষর *
did_set_textmode(optset_T *args)
{
    // when 'textmode' is set or reset also change 'fileformat'
    set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, args->os_flags);

    ফেরত ফাঁকা;
}

/*
 * Process the new 'textwidth' option value.
 */
    অক্ষর *
did_set_textwidth(optset_T *args UNUSED)
{
    অক্ষর *errmsg = ফাঁকা;

    যদি (curbuf->b_p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_tw = 0;
    }
    যদি (p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tw = 0;
    }
#ifdef FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(ফাঁকা, wp);
    }
#endif

    ফেরত errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    অক্ষর *
did_set_title_icon(optset_T *args UNUSED)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_set_title();
    ফেরত ফাঁকা;
}

/*
 * Process the new 'titlelen' option value.
 */
    অক্ষর *
did_set_titlelen(optset_T *args)
{
    long old_মান = args->os_oldval.সংখ্যা;
    অক্ষর *errmsg = ফাঁকা;

    // if 'titlelen' has changed, redraw the title
    যদি (p_titlelen < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_titlelen = 85;
    }
    যদি (starting != NO_SCREEN && old_মান != p_titlelen)
	need_maketitle = TRUE;

    ফেরত errmsg;
}

#যদি defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    অক্ষর *
did_set_undofile(optset_T *args)
{
    // Only take action when the option was set.
    যদি (!curbuf->b_p_udf && !p_udf)
	ফেরত ফাঁকা;

    // When reset we do not delete the undo file, the option may be set again
    // without making any changes in between.
    char_u	hash[UNDO_HASH_আকার];
    buf_T	*save_curbuf = curbuf;

    FOR_ALL_BUFFERS(curbuf)
    {
	// When 'undofile' is set globally: for every buffer, otherwise
	// only for the current buffer: Try to read in the undofile,
	// if one exists, the buffer wasn't changed and the buffer was
	// loaded
	যদি ((curbuf == save_curbuf
		    || (args->os_flags & OPT_GLOBAL)
		    || args->os_flags == 0)
		&& !curbufIsChanged() && curbuf->b_ml.ml_mfp != ফাঁকা)
	{
# ifdef FEAT_CRYPT
	    যদি (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
		চালু;
# endif
	    u_compute_hash(hash);
	    u_read_undo(ফাঁকা, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    ফেরত ফাঁকা;
}
#endif

/*
 * Process the new global 'undolevels' option value.
 */
    স্থির শূন্য
update_global_undolevels(long মান, long old_মান)
{
    // sync undo before 'undolevels' changes

    // use the old value, otherwise u_sync() may not work properly
    p_ul = old_মান;
    u_sync(TRUE);
    p_ul = মান;
}

/*
 * Process the new buffer local 'undolevels' option value.
 */
    স্থির শূন্য
update_buflocal_undolevels(long মান, long old_মান)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_মান;
    u_sync(TRUE);
    curbuf->b_p_ul = মান;
}

/*
 * Process the new 'undolevels' option value.
 */
    অক্ষর *
did_set_undolevels(optset_T *args)
{
    long *pp = (long *)args->os_varp;

    যদি (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(args->os_newval.সংখ্যা,
						       args->os_oldval.সংখ্যা);
    অন্যথা যদি (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(args->os_newval.সংখ্যা,
						       args->os_oldval.সংখ্যা);

    ফেরত ফাঁকা;
}

/*
 * Process the new 'updatecount' option value.
 */
    অক্ষর *
did_set_updatecount(optset_T *args)
{
    long old_মান = args->os_oldval.সংখ্যা;
    অক্ষর *errmsg = ফাঁকা;

    // when 'updatecount' changes from zero to non-zero, open swap files
    যদি (p_uc < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_uc = 100;
    }
    যদি (p_uc && !old_মান)
	ml_open_files();

    ফেরত errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    অক্ষর *
did_set_weirdinvert(optset_T *args)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    যদি (p_wiv && !args->os_oldval.boolean)
	T_XS = (char_u *)"y";
    অন্যথা যদি (!p_wiv && args->os_oldval.boolean)
	T_XS = empty_option;
    p_wiv = (*T_XS != NUL);

    ফেরত ফাঁকা;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    অক্ষর *
did_set_wildchar(optset_T *args)
{
    long c = *(long *)args->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    যদি (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	ফেরত e_invalid_argument;

    ফেরত ফাঁকা;
}

/*
 * Process the new 'window' option value.
 */
    অক্ষর *
did_set_window(optset_T *args UNUSED)
{
    যদি (p_window < 1)
	p_window = 1;
    অন্যথা যদি (p_window >= Rows)
	p_window = Rows - 1;
    ফেরত ফাঁকা;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    অক্ষর *
did_set_winheight_helpheight(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    অক্ষর	*errmsg = ফাঁকা;

    যদি (p_wh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wh = 1;
    }
    যদি (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    যদি (p_hh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hh = 0;
    }

    // Change window height NOW
    যদি (!ONE_WINDOW)
    {
	যদি (pp == &p_wh && curwin->w_height < p_wh)
	    win_setheight((পূর্ণসংখ্যা)p_wh);
	যদি (pp == &p_hh && curbuf->b_help && curwin->w_height < p_hh)
	    win_setheight((পূর্ণসংখ্যা)p_hh);
    }

    ফেরত errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    অক্ষর *
did_set_winminheight(optset_T *args UNUSED)
{
    অক্ষর *errmsg = ফাঁকা;

    যদি (p_wmh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmh = 0;
    }
    যদি (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    ফেরত errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    অক্ষর *
did_set_winminwidth(optset_T *args UNUSED)
{
    অক্ষর *errmsg = ফাঁকা;

    যদি (p_wmw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmw = 0;
    }
    যদি (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    ফেরত errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    অক্ষর *
did_set_winwidth(optset_T *args UNUSED)
{
    অক্ষর *errmsg = ফাঁকা;

    যদি (p_wiw < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wiw = 1;
    }
    যদি (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    যদি (!ONE_WINDOW && curwin->w_width < p_wiw)
	win_setwidth((পূর্ণসংখ্যা)p_wiw);

    ফেরত errmsg;
}

#যদি defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    অক্ষর *
did_set_wlsteal(optset_T *args UNUSED)
{
    clip_reset_wayland();

    ফেরত ফাঁকা;
}
#endif

#যদি defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    অক্ষর *
did_set_wltimeoutlen(optset_T *args)
{
    যদি (p_wtm < 0)
    {
	p_wtm = args->os_oldval.সংখ্যা;
	ফেরত e_argument_must_be_positive;
    }

    ফেরত ফাঁকা;
}
#endif

/*
 * Process the updated 'wrap' option value.
 */
    অক্ষর *
did_set_wrap(optset_T *args UNUSED)
{
    // Set w_leftcol or w_skipcol to zero.
    যদি (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    অন্যথা
	curwin->w_skipcol = 0;

    ফেরত ফাঁকা;
}

#ifdef FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    অক্ষর *
did_set_xhistory(optset_T *args)
{
    পূর্ণসংখ্যা is_p_chi = (long*)args->os_varp == &p_chi;
    পূর্ণসংখ্যা err;
    long *arg = (is_p_chi) ? &p_chi :(long*)args->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    যদি (*arg < 1)
    {
	*arg = args->os_oldval.সংখ্যা;
	ফেরত e_cannot_have_negative_or_zero_সংখ্যা_of_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    যদি (*arg > 100)
    {
	*arg = args->os_oldval.সংখ্যা;
	ফেরত e_cannot_have_more_than_hundred_quickfix;
    }

    যদি (is_p_chi)
	err = qf_resize_স্তূপ(*arg);
    অন্যথা
	err = ll_resize_স্তূপ(curwin, *arg);

    যদি (err == FAIL)
	ফেরত e_failed_resizing_quickfix_স্তূপ;

    ফেরত ফাঁকা;
}
#endif

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    স্থির অক্ষর *
set_bool_option(
    পূর্ণসংখ্যা		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    পূর্ণসংখ্যা		মান,			// new value
    পূর্ণসংখ্যা		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    পূর্ণসংখ্যা		old_মান = *(পূর্ণসংখ্যা *)varp;
#যদি defined(FEAT_EVAL)
    পূর্ণসংখ্যা		old_global_মান = 0;
#endif
    অক্ষর	*errmsg = ফাঁকা;

    // Disallow changing some options from secure mode
    যদি ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	ফেরত e_not_allowed_here;

#যদি defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    যদি ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_মান = *(পূর্ণসংখ্যা *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *(পূর্ণসংখ্যা *)varp = মান;	    // set the new value
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    যদি ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(পূর্ণসংখ্যা *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = মান;

    // Handle side effects of changing a bool option.
    যদি (options[opt_idx].opt_did_set_cb != ফাঁকা)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.boolean = old_মান;
	args.os_newval.boolean = মান;
	args.os_errbuf = ফাঁকা;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
	যদি (errmsg != ফাঁকা)
	    ফেরত errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_SET;

#যদি defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(long)(old_মান ? TRUE : FALSE),
				(long)(old_global_মান ? TRUE : FALSE),
				(long)(মান ? TRUE : FALSE), ফাঁকা);
#endif

    comp_col();			    // in case 'ruler' or 'showcmd' changed

    যদি (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    যদি ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    ফেরত errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    স্থির অক্ষর *
check_অঙ্ক_option_bounds(
    long	*pp,
    long	old_মান,
    long	old_Rows,
    long	old_Columns,
    অক্ষর	*errbuf,
    size_t	errbuflen,
    অক্ষর	*errmsg)
{
    যদি (Rows < min_rows_for_all_tabpages() && full_screen)
    {
	যদি (errbuf != ফাঁকা)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_সারিগুলি), min_rows_for_all_tabpages());
	    errmsg = errbuf;
	}
	Rows = min_rows_for_all_tabpages();
    }
    যদি (Columns < MIN_COLUMNS && full_screen)
    {
	যদি (errbuf != ফাঁকা)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_columns), MIN_COLUMNS);
	    errmsg = errbuf;
	}
	Columns = MIN_COLUMNS;
    }
    limit_screen_আকার();

    // If the screen (shell) height has been changed, assume it is the
    // physical screenheight.
    যদি (old_Rows != Rows || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	যদি (updating_screen)
	    *pp = old_মান;
	অন্যথা যদি (full_screen
#ifdef FEAT_GUI
		&& !gui.starting
#endif
		)
	    set_shellsize((পূর্ণসংখ্যা)Columns, (পূর্ণসংখ্যা)Rows, TRUE);
	অন্যথা
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    যদি (cmdline_row > Rows - p_ch && Rows > p_ch)
		cmdline_row = Rows - p_ch;
	}
	যদি (p_window >= Rows || !option_was_set((char_u *)"window"))
	    p_window = Rows - 1;
    }

    যদি (p_tm < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tm = 0;
    }
    যদি ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_height
		    && curwin->w_height > 0))
	    && full_screen)
    {
	যদি (pp == &(curwin->w_p_scr))
	{
	    যদি (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_আকার;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	অন্যথা যদি (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	অন্যথা // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_height;
    }
    যদি (p_hi < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hi = 0;
    }
    অন্যথা যদি (p_hi > 10000)
    {
	errmsg = e_invalid_argument;
	p_hi = 10000;
    }
    যদি (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_argument;
	p_re = 0;
    }
    যদি (p_report < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_report = 1;
    }
    যদি ((p_sj < -100 || p_sj >= Rows) && full_screen)
    {
	যদি (Rows != old_Rows)	// Rows changed, just adjust p_sj
	    p_sj = Rows / 2;
	অন্যথা
	{
	    errmsg = e_invalid_scroll_আকার;
	    p_sj = 1;
	}
    }
    যদি (p_so < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_so = 0;
    }
    যদি (p_sop < 0 && full_screen)
    {
	errmsg = e_invalid_argument;
	p_sop = 0;
    }
    যদি (p_siso < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_siso = 0;
    }
    যদি (p_cwh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_cwh = 1;
    }
    যদি (p_ut < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ut = 2000;
    }
    যদি (p_ss < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ss = 0;
    }

    ফেরত errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    স্থির অক্ষর *
set_অঙ্ক_option(
    পূর্ণসংখ্যা		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    long	মান,			// new value
    অক্ষর	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    পূর্ণসংখ্যা		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    অক্ষর	*errmsg = ফাঁকা;
    long	old_মান = *(long *)varp;
#যদি defined(FEAT_EVAL)
    long	old_global_মান = 0;	// only used when setting a local and
					// global option
#endif
    long	old_Rows = Rows;	// remember old Rows
    long	old_Columns = Columns;	// remember old Columns
    long	*pp = (long *)varp;

    // Disallow changing some options from secure mode.
    যদি ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	ফেরত e_not_allowed_here;

#যদি defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    যদি ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_মান = *(long *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *pp = মান;
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    যদি ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = মান;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    যদি (options[opt_idx].opt_did_set_cb != ফাঁকা)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.সংখ্যা = old_মান;
	args.os_newval.সংখ্যা = মান;
	args.os_errbuf = ফাঁকা;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
    }

    // Check the bounds for numeric options here
    errmsg = check_অঙ্ক_option_bounds(pp, old_মান, old_Rows, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_SET;

#যদি defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_মান, old_global_মান,
								মান, errmsg);
#endif

    comp_col();			    // in case 'columns' or 'ls' changed

    যদি (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    যদি ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    ফেরত errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    শূন্য
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    পূর্ণসংখ্যা		doclear = (flags & P_RCLR) == P_RCLR;
    পূর্ণসংখ্যা		all = ((flags & P_RALL) == P_RALL || doclear);

    যদি ((flags & P_RSTAT) || all)	// mark all status lines dirty
	status_redraw_all();

    যদি ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	যদি (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	অন্যথা
	    changed_window_setting();
    }
    যদি (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    যদি (doclear)
	redraw_all_later(UPD_CLEAR);
    অন্যথা যদি (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    পূর্ণসংখ্যা
findoption(char_u *arg)
{
    পূর্ণসংখ্যা		    opt_idx;
    অক্ষর	    *s, *p;
    স্থির short    quick_tab[27] = {0, 0};	// quick access table
    পূর্ণসংখ্যা		    is_term_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    যদি (quick_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	পূর্ণসংখ্যা last_opt_idx;
	জন্য (last_opt_idx = 0; options[last_opt_idx].fullname != ফাঁকা;
		last_opt_idx++)
	    ;
	জন্য (পূর্ণসংখ্যা tab_idx = 1; tab_idx < 27; tab_idx++)
	    quick_tab[tab_idx] = last_opt_idx;

	p = options[0].fullname;
	জন্য (opt_idx = 1; opt_idx < last_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    যদি (s[0] != p[0])
	    {
		যদি (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		অন্যথা
		    quick_tab[Char_Ord_নিচের(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    যদি (arg[0] < 'a' || arg[0] > 'z')
	ফেরত -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    যদি (is_term_opt)
	opt_idx = quick_tab[26];
    অন্যথা
	opt_idx = quick_tab[Char_Ord_নিচের(arg[0])];
    জন্য (; (s = options[opt_idx].fullname) != ফাঁকা && s[0] == arg[0]; opt_idx++)
    {
	যদি (STRCMP(arg, s) == 0)		    // match full name
	    বিরতি;
    }
    যদি (s != ফাঁকা && s[0] != arg[0])
	s = ফাঁকা;
    যদি (s == ফাঁকা && !is_term_opt)
    {
	opt_idx = quick_tab[Char_Ord_নিচের(arg[0])];
	জন্য ( ; options[opt_idx].fullname != ফাঁকা; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    যদি (s != ফাঁকা && STRCMP(arg, s) == 0)   // match short name
		বিরতি;
	    s = ফাঁকা;
	}
    }
    যদি (s == ফাঁকা)
	opt_idx = -1;
    ফেরত opt_idx;
}

#যদি defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
get_option_মান(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    পূর্ণসংখ্যা		*flagsp,
    পূর্ণসংখ্যা		scope)
{
    পূর্ণসংখ্যা		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    যদি (opt_idx < 0)		    // option not in the table
    {
	পূর্ণসংখ্যা চাবি;

	যদি (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (চাবি = find_চাবি_option(name, FALSE)) != 0)
	{
	    char_u চাবি_name[2];
	    char_u *p;

	    যদি (flagsp != ফাঁকা)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    যদি (চাবি < 0)
	    {
		চাবি_name[0] = KEY2TERMCAP0(চাবি);
		চাবি_name[1] = KEY2TERMCAP1(চাবি);
	    }
	    অন্যথা
	    {
		চাবি_name[0] = KS_চাবি;
		চাবি_name[1] = (চাবি & 0xff);
	    }
	    p = find_termcode(চাবি_name);
	    যদি (p != ফাঁকা)
	    {
		যদি (stringval != ফাঁকা)
		    *stringval = vim_strsave(p);
		ফেরত gov_string;
	    }
	}
	ফেরত gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    যদি (flagsp != ফাঁকা)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    যদি (options[opt_idx].flags & P_STRING)
    {
	যদি (varp == ফাঁকা)		    // hidden option
	    ফেরত gov_hidden_string;
	যদি (stringval != ফাঁকা)
	{
	    যদি ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    অন্যথা যদি ((char_u **)varp == &curbuf->b_p_চাবি
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    অন্যথা
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	ফেরত gov_string;
    }

    যদি (varp == ফাঁকা)		    // hidden option
	ফেরত (options[opt_idx].flags & P_অঙ্ক)
					 ? gov_hidden_সংখ্যা : gov_hidden_bool;
    যদি (options[opt_idx].flags & P_অঙ্ক)
	*numval = *(long *)varp;
    অন্যথা
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	যদি ((পূর্ণসংখ্যা *)varp == &curbuf->b_changed)
	    *numval = curbufIsChanged();
	অন্যথা
	    *numval = (long) *(পূর্ণসংখ্যা *)varp;
    }
    ফেরত (options[opt_idx].flags & P_অঙ্ক) ? gov_সংখ্যা : gov_bool;
}
#endif

#যদি defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    পূর্ণসংখ্যা
get_option_মান_strict(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    পূর্ণসংখ্যা		opt_type,
    শূন্য	*from)
{
    পূর্ণসংখ্যা		opt_idx;
    char_u	*varp = ফাঁকা;
    কাঠামো vimoption *p;
    পূর্ণসংখ্যা		r = 0;

    opt_idx = findoption(name);
    যদি (opt_idx < 0)
	ফেরত 0;

    p = &(options[opt_idx]);

    // Hidden option
    যদি (p->var == ফাঁকা)
	ফেরত 0;

    যদি (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    অন্যথা যদি (p->flags & P_অঙ্ক)
	r |= SOPT_অঙ্ক;
    অন্যথা যদি (p->flags & P_STRING)
	r |= SOPT_STRING;

    যদি (p->indir == PV_NONE)
    {
	যদি (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	অন্যথা
	    ফেরত 0; // Did not request global-only option
    }
    অন্যথা
    {
	যদি (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	অন্যথা যদি (opt_type == SREQ_GLOBAL)
	    ফেরত 0; // Requested global option

	যদি (p->indir & PV_WIN)
	{
	    যদি (opt_type == SREQ_BUF)
		ফেরত 0; // Did not request window-local option
	    অন্যথা
		r |= SOPT_WIN;
	}
	অন্যথা যদি (p->indir & PV_BUF)
	{
	    যদি (opt_type == SREQ_WIN)
		ফেরত 0; // Did not request buffer-local option
	    অন্যথা
		r |= SOPT_BUF;
	}
    }

    যদি (stringval == ফাঁকা)
	ফেরত r;

    যদি (opt_type == SREQ_GLOBAL)
	varp = p->var;
    অন্যথা
    {
	যদি (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    যদি (p->indir == PV_MOD)
	    {
		*numval = bufIsChanged((buf_T *)from);
		varp = ফাঁকা;
	    }
# ifdef FEAT_CRYPT
	    অন্যথা যদি (p->indir == PV_চাবি)
	    {
		// never return the value of the crypt key
		*stringval = ফাঁকা;
		varp = ফাঁকা;
	    }
# endif
	    অন্যথা
	    {
		buf_T *save_curbuf = curbuf;

		// only getting a pointer, no need to use aucmd_prepbuf()
		curbuf = (buf_T *)from;
		curwin->w_বাফার = curbuf;
		varp = get_varp(p);
		curbuf = save_curbuf;
		curwin->w_বাফার = curbuf;
	    }
	}
	অন্যথা যদি (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_বাফার;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_বাফার;
	}
	যদি (varp == p->var)
	    ফেরত (r | SOPT_UNSET);
    }

    যদি (varp != ফাঁকা)
    {
	যদি (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	অন্যথা যদি (p->flags & P_অঙ্ক)
	    *numval = *(long *) varp;
	অন্যথা
	    *numval = *(পূর্ণসংখ্যা *)varp;
    }

    ফেরত r;
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
option_iter_পরবর্তী(শূন্য **option, পূর্ণসংখ্যা opt_type)
{
    কাঠামো vimoption	*ret = ফাঁকা;
    করো
    {
	যদি (*option == ফাঁকা)
	    *option = (শূন্য *) options;
	অন্যথা যদি (((কাঠামো vimoption *) (*option))->fullname == ফাঁকা)
	{
	    *option = ফাঁকা;
	    ফেরত ফাঁকা;
	}
	অন্যথা
	    *option = (শূন্য *) (((কাঠামো vimoption *) (*option)) + 1);

	ret = ((কাঠামো vimoption *) (*option));

	// Hidden option
	যদি (ret->var == ফাঁকা)
	{
	    ret = ফাঁকা;
	    চালু;
	}

	নির্বাচন (opt_type)
	{
	    ক্ষেত্র SREQ_GLOBAL:
		যদি (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = ফাঁকা;
		বিরতি;
	    ক্ষেত্র SREQ_BUF:
		যদি (!(ret->indir & PV_BUF))
		    ret = ফাঁকা;
		বিরতি;
	    ক্ষেত্র SREQ_WIN:
		যদি (!(ret->indir & PV_WIN))
		    ret = ফাঁকা;
		বিরতি;
	    পূর্বনির্ধারিত:
		internal_error("option_iter_next()");
		ফেরত ফাঁকা;
	}
    }
    যখন (ret == ফাঁকা);

    ফেরত (char_u *)ret->fullname;
}
#endif

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(পূর্ণসংখ্যা opt_idx)
{
    ফেরত options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    শূন্য
set_option_flag(পূর্ণসংখ্যা opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    পূর্ণসংখ্যা
is_global_option(পূর্ণসংখ্যা opt_idx)
{
    ফেরত options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    পূর্ণসংখ্যা
is_global_local_option(পূর্ণসংখ্যা opt_idx)
{
    ফেরত options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    পূর্ণসংখ্যা
is_window_local_option(পূর্ণসংখ্যা opt_idx)
{
    ফেরত options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    পূর্ণসংখ্যা
is_hidden_option(পূর্ণসংখ্যা opt_idx)
{
    ফেরত options[opt_idx].var == ফাঁকা;
}

#যদি defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    পূর্ণসংখ্যা
is_crypt_চাবি_option(পূর্ণসংখ্যা opt_idx)
{
    ফেরত options[opt_idx].indir == PV_চাবি;
}
#endif

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    অক্ষর *
set_option_মান(
    char_u	*name,
    long	সংখ্যা,
    char_u	*string,
    পূর্ণসংখ্যা		opt_flags)	// OPT_LOCAL or 0 (both)
{
    পূর্ণসংখ্যা		opt_idx;
    char_u	*varp;
    long_u	flags;
    স্থির অক্ষর	errbuf[ERR_BUFLEN];
    পূর্ণসংখ্যা		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    যদি (opt_idx < 0)
    {
	পূর্ণসংখ্যা চাবি;

	যদি (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (চাবি = find_চাবি_option(name, FALSE)) != 0)
	{
	    char_u চাবি_name[2];

	    যদি (চাবি < 0)
	    {
		চাবি_name[0] = KEY2TERMCAP0(চাবি);
		চাবি_name[1] = KEY2TERMCAP1(চাবি);
	    }
	    অন্যথা
	    {
		চাবি_name[0] = KS_চাবি;
		চাবি_name[1] = (চাবি & 0xff);
	    }
	    add_termcode(চাবি_name, string, FALSE);
	    যদি (full_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    ফেরত ফাঁকা;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    অন্যথা
    {
	flags = options[opt_idx].flags;
#ifdef HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	যদি (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_in_sandbox));
	    ফেরত ফাঁকা;
	}
#endif
	যদি (flags & P_STRING)
	    ফেরত set_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	যদি (varp != ফাঁকা)	// hidden option is not changed
	{
	    যদি (সংখ্যা == 0 && string != ফাঁকা)
	    {
		পূর্ণসংখ্যা idx;

		// Either we are given a string or we are setting option
		// to zero.
		জন্য (idx = 0; string[idx] == '0'; ++idx)
		    ;
		যদি (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_সংখ্যা_required_after_str_equal_str),
								 name, string);
		    ফেরত ফাঁকা;     // do nothing as we hit an error

		}
	    }
	    যদি (flags & P_অঙ্ক)
	    {
		ফেরত set_অঙ্ক_option(opt_idx, varp, সংখ্যা,
					   errbuf, আকার(errbuf), opt_flags);
	    }
	    অন্যথা
		ফেরত set_bool_option(opt_idx, varp, (পূর্ণসংখ্যা)সংখ্যা, opt_flags);
	}

    }
    ফেরত ফাঁকা;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    শূন্য
set_option_মান_give_err(
    char_u	*name,
    long	সংখ্যা,
    char_u	*string,
    পূর্ণসংখ্যা		opt_flags)	// OPT_LOCAL or 0 (both)
{
    অক্ষর *errmsg = set_option_মান(name, সংখ্যা, string, opt_flags);

    যদি (errmsg != ফাঁকা)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_term_code(char_u *tname)
{
    পূর্ণসংখ্যা	    opt_idx;
    char_u  *varp;

    যদি (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	ফেরত ফাঁকা;
    যদি ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	যদি (varp != ফাঁকা)
	    varp = *(char_u **)(varp);
	ফেরত varp;
    }
    ফেরত find_termcode(tname + 2);
}

    char_u *
get_highlight_default(শূন্য)
{
    পূর্ণসংখ্যা i;

    i = findoption((char_u *)"hl");
    যদি (i >= 0)
	ফেরত options[i].def_val[VI_DEFAULT];
    ফেরত (char_u *)ফাঁকা;
}

    char_u *
get_encoding_default(শূন্য)
{
    পূর্ণসংখ্যা i;

    i = findoption((char_u *)"enc");
    যদি (i >= 0)
	ফেরত options[i].def_val[VI_DEFAULT];
    ফেরত (char_u *)ফাঁকা;
}

#যদি defined(FEAT_QUICKFIX)
    পূর্ণসংখ্যা
is_option_allocated(অক্ষর *name)
{
    পূর্ণসংখ্যা idx = findoption((char_u *)name);

    ফেরত idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#endif

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    স্থির পূর্ণসংখ্যা
find_চাবি_option(char_u *arg_arg, পূর্ণসংখ্যা has_lt)
{
    পূর্ণসংখ্যা		চাবি = 0;
    পূর্ণসংখ্যা		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    যদি (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	যদি (!has_lt || arg[4] == '>')
	    চাবি = TERMCAP2_চাবি(arg[2], arg[3]);
    }
    অন্যথা যদি (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	চাবি = find_special_চাবি(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_চাবি | FSK_SIMPLIFY, ফাঁকা);
	যদি (modifiers)		    // can't handle modifiers here
	    চাবি = 0;
    }
    ফেরত চাবি;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    স্থির শূন্য
showoptions(
    পূর্ণসংখ্যা		all,
    পূর্ণসংখ্যা		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    কাঠামো vimoption	*p;
    পূর্ণসংখ্যা			col;
    পূর্ণসংখ্যা			isterm;
    char_u		*varp;
    কাঠামো vimoption	**বস্তুসমূহ;
    পূর্ণসংখ্যা			item_গুনতি;
    পূর্ণসংখ্যা			run;
    পূর্ণসংখ্যা			row, rows;
    পূর্ণসংখ্যা			cols;
    পূর্ণসংখ্যা			i;
    পূর্ণসংখ্যা			len;

#define INC 20
#define GAP 3

    বস্তুসমূহ = ALLOC_MULT(কাঠামো vimoption *, OPTION_গুনতি);
    যদি (বস্তুসমূহ == ফাঁকা)
	ফেরত;

    // Highlight title
    যদি (all == 2)
	বার্তা_puts_title(_("\n--- Terminal codes ---"));
    অন্যথা যদি (opt_flags & OPT_GLOBAL)
	বার্তা_puts_title(_("\n--- Global option values ---"));
    অন্যথা যদি (opt_flags & OPT_LOCAL)
	বার্তা_puts_title(_("\n--- Local option values ---"));
    অন্যথা
	বার্তা_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    জন্য (run = 1; run <= 2 && !got_int; ++run)
    {
	// collect the items in items[]
	item_গুনতি = 0;
	জন্য (p = &options[0]; p->fullname != ফাঁকা; p++)
	{
	    // apply :filter /pat/
	    যদি (message_filtered((char_u *)p->fullname))
		চালু;

	    varp = ফাঁকা;
	    isterm = istermoption(p);
	    যদি ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		যদি (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    অন্যথা
		varp = get_varp(p);
	    যদি (varp != ফাঁকা
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		যদি (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		অন্যথা যদি (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		অন্যথা
		{
		    option_value2string(p, opt_flags);
		    len = (পূর্ণসংখ্যা)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		যদি ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    বস্তুসমূহ[item_গুনতি++] = p;
	    }
	}

	// display the items
	যদি (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    যদি (cols == 0)
		cols = 1;
	    rows = (item_গুনতি + cols - 1) / cols;
	}
	অন্যথা	// run == 2
	    rows = item_গুনতি;
	জন্য (row = 0; row < rows && !got_int; ++row)
	{
	    বার্তা_putchar('\n');			// go to next line
	    যদি (got_int)			// 'q' typed in more
		বিরতি;
	    col = 0;
	    জন্য (i = row; i < item_গুনতি; i += rows)
	    {
		বার্তা_col = col;			// make columns
		showoneopt(বস্তুসমূহ[i], opt_flags);
		col += INC;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(বস্তুসমূহ);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    স্থির পূর্ণসংখ্যা
optval_default(কাঠামো vimoption *p, char_u *varp, পূর্ণসংখ্যা compatible)
{
    পূর্ণসংখ্যা		dvi;

    যদি (varp == ফাঁকা)
	ফেরত TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    যদি (p->flags & P_অঙ্ক)
	ফেরত (*(long *)varp == (long)(long_i)p->def_val[dvi]);
    যদি (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	ফেরত (*(পূর্ণসংখ্যা *)varp == (পূর্ণসংখ্যা)(long)(long_i)p->def_val[dvi]);
    // P_STRING
    ফেরত (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    স্থির শূন্য
showoneopt(
    কাঠামো vimoption	*p,
    পূর্ণসংখ্যা			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    পূর্ণসংখ্যা		save_silent = silent_mode;

    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    যদি ((p->flags & P_BOOL) && ((পূর্ণসংখ্যা *)varp == &curbuf->b_changed
					? !curbufIsChanged() : !*(পূর্ণসংখ্যা *)varp))
	বার্তা_puts("no");
    অন্যথা যদি ((p->flags & P_BOOL) && *(পূর্ণসংখ্যা *)varp < 0)
	বার্তা_puts("--");
    অন্যথা
	বার্তা_puts("  ");
    বার্তা_puts(p->fullname);
    যদি (!(p->flags & P_BOOL))
    {
	বার্তা_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	বার্তা_outtrans(NameBuff);
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
    পূর্ণসংখ্যা
makeset(FILE *fd, পূর্ণসংখ্যা opt_flags, পূর্ণসংখ্যা local_only)
{
    কাঠামো vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = ফাঁকা;	// fresh value
    অক্ষর		*cmd;
    পূর্ণসংখ্যা			round;
    পূর্ণসংখ্যা			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    জন্য (pri = 1; pri >= 0; --pri)
    {
      জন্য (p = &options[0]; !istermoption(p); p++)
	যদি (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    যদি (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		চালু;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    যদি ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		চালু;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    যদি ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		চালু;

	    যদি ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		চালু;

	    round = 2;
	    যদি (p->indir != PV_NONE)
	    {
		যদি (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    যদি (!(opt_flags & OPT_LOCAL))
			চালু;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    যদি (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			যদি (!optval_default(p, varp_fresh, p_cp))
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
	    জন্য ( ; round <= 2; varp = varp_local, ++round)
	    {
		যদি (round == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		অন্যথা
		    cmd = "setlocal";

		যদি (p->flags & P_BOOL)
		{
		    যদি (put_setbool(fd, cmd, p->fullname, *(পূর্ণসংখ্যা *)varp) == FAIL)
			ফেরত FAIL;
		}
		অন্যথা যদি (p->flags & P_অঙ্ক)
		{
		    যদি (put_setnum(fd, cmd, p->fullname, (long *)varp) == FAIL)
			ফেরত FAIL;
		}
		অন্যথা    // P_STRING
		{
		    পূর্ণসংখ্যা		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    যদি ((p->flags & P_FUNC) && *(char_u **)varp != ফাঁকা
			    && strstr(*(অক্ষর **)varp, "<lambda>") != ফাঁকা)
			চালু;

#ifdef FEAT_EVAL
		    legacy = !is_option_মান_vim9(p - &options[0],
			    round == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#অন্যথা
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#endif

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    যদি (
#যদি defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#endif
			    p->indir == PV_FT)
		    {
			যদি (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    ফেরত FAIL;
			do_endif = TRUE;
		    }
		    যদি (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			ফেরত FAIL;
		    যদি (do_endif)
		    {
			যদি (put_সারি(fd, "endif") == FAIL)
			    ফেরত FAIL;
		    }
		}
	    }
	}
    }
    ফেরত OK;
}

#যদি defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    পূর্ণসংখ্যা
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_version < SCRIPT_VERSION_VIM9;
# endif

    যদি (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	ফেরত FAIL;

    ফেরত OK;
}
#endif

    স্থির পূর্ণসংখ্যা
put_setstring(
    FILE	*fd,
    bool	legacy,
    অক্ষর	*cmd,
    অক্ষর	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = ফাঁকা;
    char_u	*part = ফাঁকা;
    char_u	*p;

    যদি (legacy && fprintf(fd, "legacy ") < 0)
	ফেরত FAIL;
    যদি (fprintf(fd, "%s %s=", cmd, name) < 0)
	ফেরত FAIL;
    যদি (*valuep != ফাঁকা)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	যদি (valuep == &p_pt)
	{
	    s = *valuep;
	    যখন (*s != NUL)
		যদি (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    ফেরত FAIL;
	}
	// expand the option value, replace $HOME by ~
	অন্যথা যদি ((flags & P_EXPAND) != 0)
	{
	    পূর্ণসংখ্যা  আকার_2 = (পূর্ণসংখ্যা)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(আকার_2);
	    যদি (buf == ফাঁকা)
		যাও fail;
	    home_replace(ফাঁকা, *valuep, buf, আকার_2, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    যদি (আকার_2 >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != ফাঁকা)
	    {
		part = alloc(আকার_2);
		যদি (part == ফাঁকা)
		    যাও fail;

		// write line break to clear the option, e.g. ':set rtp='
		যদি (put_eol(fd) == FAIL)
		    যাও fail;

		p = buf;
		যখন (*p != NUL)
		{
		    যদি (legacy && fprintf(fd, "legacy ") < 0)
			ফেরত FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    যদি (fprintf(fd, "%s %s+=", cmd, name) < 0)
			যাও fail;
		    (শূন্য)copy_option_part(&p, part, আকার_2,  ",");
		    যদি (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			যাও fail;
		}
		vim_free(buf);
		vim_free(part);
		ফেরত OK;
	    }
	    যদি (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		ফেরত FAIL;
	    }
	    vim_free(buf);
	}
	অন্যথা যদি (put_escstr(fd, *valuep, 2) == FAIL)
	    ফেরত FAIL;
    }
    যদি (put_eol(fd) < 0)
	ফেরত FAIL;
    ফেরত OK;
fail:
    vim_free(buf);
    vim_free(part);
    ফেরত FAIL;
}

    স্থির পূর্ণসংখ্যা
put_setnum(
    FILE	*fd,
    অক্ষর	*cmd,
    অক্ষর	*name,
    long	*valuep)
{
    long	wc;

    যদি (fprintf(fd, "%s %s=", cmd, name) < 0)
	ফেরত FAIL;
    যদি (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	যদি (fputs((অক্ষর *)get_special_চাবি_name((পূর্ণসংখ্যা)wc, 0), fd) < 0)
	    ফেরত FAIL;
    }
    অন্যথা যদি (fprintf(fd, "%ld", *valuep) < 0)
	ফেরত FAIL;
    যদি (put_eol(fd) < 0)
	ফেরত FAIL;
    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
put_setbool(
    FILE	*fd,
    অক্ষর	*cmd,
    অক্ষর	*name,
    পূর্ণসংখ্যা		মান)
{
    যদি (মান < 0)	// global/local option using global value
	ফেরত OK;
    যদি (fprintf(fd, "%s %s%s", cmd, মান ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	ফেরত FAIL;
    ফেরত OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    শূন্য
clear_termoptions(শূন্য)
{
    // Reset a few things before clearing the old options. This may cause
    // outputting a few things that the terminal doesn't understand, but the
    // screen will be cleared later, so this is OK.
    mch_setmouse(FALSE);	    // switch mouse off
    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window titles
#যদি defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    যদি (gui.starting)
	clear_xterm_clip();
#endif
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    শূন্য
free_termoptions(শূন্য)
{
    কাঠামো vimoption   *p;

    জন্য (p = options; p->fullname != ফাঁকা; p++)
	যদি (istermoption(p))
	{
	    যদি (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    যদি (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = empty_option;
	    p->def_val[VI_DEFAULT] = empty_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ifdef FEAT_EVAL
	    // remember where the option was cleared
	    set_option_sctx_idx((পূর্ণসংখ্যা)(p - options), OPT_GLOBAL, current_sctx);
#endif
	}
    clear_termcodes();
}

/*
 * Free the string for one term option, if it was allocated.
 * Set the string to empty_option and clear allocated flag.
 * "var" points to the option value.
 */
    শূন্য
free_one_termoption(char_u *var)
{
    কাঠামো vimoption   *p;

    জন্য (p = &options[0]; p->fullname != ফাঁকা; p++)
	যদি (p->var == var)
	{
	    যদি (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = empty_option;
	    p->flags &= ~P_ALLOCED;
	    বিরতি;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    শূন্য
set_term_defaults(শূন্য)
{
    কাঠামো vimoption   *p;

    জন্য (p = &options[0]; p->fullname != ফাঁকা; p++)
    {
	যদি (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    যদি (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    যদি (p->flags & P_ALLOCED)
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
    স্থির পূর্ণসংখ্যা
istermoption(কাঠামো vimoption *p)
{
    ফেরত (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    স্থির পূর্ণসংখ্যা
istermoption_idx(পূর্ণসংখ্যা opt_idx)
{
    ফেরত istermoption(&options[opt_idx]);
}

#যদি defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    শূন্য
unset_global_local_option(char_u *name, শূন্য *from)
{
    কাঠামো vimoption *p;
    পূর্ণসংখ্যা		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    যদি (opt_idx < 0)
	ফেরত;
    p = &(options[opt_idx]);

    নির্বাচন ((পূর্ণসংখ্যা)p->indir)
    {
	// global option with local value: use local value if it's been set
	ক্ষেত্র PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    বিরতি;
	ক্ষেত্র PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    বিরতি;
	ক্ষেত্র PV_PATH:
	    clear_string_option(&buf->b_p_path);
	    বিরতি;
	ক্ষেত্র PV_AC:
	    buf->b_p_ac = -1;
	    বিরতি;
	ক্ষেত্র PV_AR:
	    buf->b_p_ar = -1;
	    বিরতি;
# ifdef HAVE_FSYNC
	ক্ষেত্র PV_FS:
	    buf->b_p_fs = -1;
	    বিরতি;
# endif
	ক্ষেত্র PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    বিরতি;
	ক্ষেত্র PV_TAGS:
	    clear_string_option(&buf->b_p_tags);
	    বিরতি;
	ক্ষেত্র PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    বিরতি;
	ক্ষেত্র PV_SISO:
	    curwin->w_p_siso = -1;
	    বিরতি;
	ক্ষেত্র PV_SO:
	    curwin->w_p_so = -1;
	    বিরতি;
	ক্ষেত্র PV_SOP:
	    curwin->w_p_sop = -1;
	    বিরতি;
# ifdef FEAT_FIND_ID
	ক্ষেত্র PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    বিরতি;
	ক্ষেত্র PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    বিরতি;
# endif
	ক্ষেত্র PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    বিরতি;
	ক্ষেত্র PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    বিরতি;
# ifdef FEAT_DIFF
	ক্ষেত্র PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    বিরতি;
# endif
	ক্ষেত্র PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    বিরতি;
# ifdef FEAT_COMPL_FUNC
	ক্ষেত্র PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    বিরতি;
# endif
	ক্ষেত্র PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    বিরতি;
# ifdef FEAT_EVAL
	ক্ষেত্র PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    বিরতি;
# endif
# ifdef FEAT_QUICKFIX
	ক্ষেত্র PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    বিরতি;
	ক্ষেত্র PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    বিরতি;
	ক্ষেত্র PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    বিরতি;
	ক্ষেত্র PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    বিরতি;
# endif
# যদি defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	ক্ষেত্র PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    বিরতি;
# endif
# যদি defined(FEAT_CRYPT)
	ক্ষেত্র PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    বিরতি;
# endif
# ifdef FEAT_LINEBREAK
	ক্ষেত্র PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    বিরতি;
# endif
# ifdef FEAT_STL_OPT
	ক্ষেত্র PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    বিরতি;
	ক্ষেত্র PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    বিরতি;
# endif
	ক্ষেত্র PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    বিরতি;
	ক্ষেত্র PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    বিরতি;
	ক্ষেত্র PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    বিরতি;
	ক্ষেত্র PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    set_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      ফাঁকা, 0);
	    redraw_later(UPD_NOT_VALID);
	    বিরতি;
	ক্ষেত্র PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    set_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      ফাঁকা, 0);
	    redraw_later(UPD_NOT_VALID);
	    বিরতি;
	ক্ষেত্র PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    বিরতি;
    }
}
#endif

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    স্থির char_u *
get_varp_scope(কাঠামো vimoption *p, পূর্ণসংখ্যা scope)
{
    যদি ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	যদি (p->var == VAR_WIN)
	    ফেরত (char_u *)GLOBAL_WO(get_varp(p));
	ফেরত p->var;
    }
    যদি ((scope & OPT_LOCAL) && ((পূর্ণসংখ্যা)p->indir & PV_BOTH))
    {
	নির্বাচন ((পূর্ণসংখ্যা)p->indir)
	{
	    ক্ষেত্র PV_FP:   ফেরত (char_u *)&(curbuf->b_p_fp);
#ifdef HAVE_FSYNC
	    ক্ষেত্র PV_FS:	ফেরত (char_u *)&(curbuf->b_p_fs);
#endif
#ifdef FEAT_EVAL
	    ক্ষেত্র PV_FFU: ফেরত (char_u *)&(curbuf->b_p_ffu);
#endif
#ifdef FEAT_QUICKFIX
	    ক্ষেত্র PV_EFM:  ফেরত (char_u *)&(curbuf->b_p_efm);
	    ক্ষেত্র PV_GEFM: ফেরত (char_u *)&(curbuf->b_p_gefm);
	    ক্ষেত্র PV_GP:   ফেরত (char_u *)&(curbuf->b_p_gp);
	    ক্ষেত্র PV_MP:   ফেরত (char_u *)&(curbuf->b_p_mp);
#endif
	    ক্ষেত্র PV_EP:   ফেরত (char_u *)&(curbuf->b_p_ep);
	    ক্ষেত্র PV_KP:   ফেরত (char_u *)&(curbuf->b_p_kp);
	    ক্ষেত্র PV_PATH: ফেরত (char_u *)&(curbuf->b_p_path);
	    ক্ষেত্র PV_AC:   ফেরত (char_u *)&(curbuf->b_p_ac);
	    ক্ষেত্র PV_AR:   ফেরত (char_u *)&(curbuf->b_p_ar);
	    ক্ষেত্র PV_TAGS: ফেরত (char_u *)&(curbuf->b_p_tags);
	    ক্ষেত্র PV_TC:   ফেরত (char_u *)&(curbuf->b_p_tc);
	    ক্ষেত্র PV_SISO: ফেরত (char_u *)&(curwin->w_p_siso);
	    ক্ষেত্র PV_SO:   ফেরত (char_u *)&(curwin->w_p_so);
	    ক্ষেত্র PV_SOP:  ফেরত (char_u *)&(curwin->w_p_sop);
#ifdef FEAT_FIND_ID
	    ক্ষেত্র PV_DEF:  ফেরত (char_u *)&(curbuf->b_p_def);
	    ক্ষেত্র PV_INC:  ফেরত (char_u *)&(curbuf->b_p_inc);
#endif
	    ক্ষেত্র PV_COT:  ফেরত (char_u *)&(curbuf->b_p_cot);
	    ক্ষেত্র PV_DICT: ফেরত (char_u *)&(curbuf->b_p_dict);
#ifdef FEAT_DIFF
	    ক্ষেত্র PV_DIA:  ফেরত (char_u *)&(curbuf->b_p_dia);
#endif
	    ক্ষেত্র PV_TSR:  ফেরত (char_u *)&(curbuf->b_p_tsr);
#ifdef FEAT_COMPL_FUNC
	    ক্ষেত্র PV_TSRFU: ফেরত (char_u *)&(curbuf->b_p_tsrfu);
#endif
#যদি defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    ক্ষেত্র PV_BEXPR: ফেরত (char_u *)&(curbuf->b_p_bexpr);
#endif
#যদি defined(FEAT_CRYPT)
	    ক্ষেত্র PV_CM:	  ফেরত (char_u *)&(curbuf->b_p_cm);
#endif
#ifdef FEAT_LINEBREAK
	    ক্ষেত্র PV_SBR:  ফেরত (char_u *)&(curwin->w_p_sbr);
#endif
#ifdef FEAT_STL_OPT
	    ক্ষেত্র PV_STL:  ফেরত (char_u *)&(curwin->w_p_stl);
	    ক্ষেত্র PV_STLO: ফেরত (char_u *)&(curwin->w_p_stlo);
#endif
	    ক্ষেত্র PV_UL:   ফেরত (char_u *)&(curbuf->b_p_ul);
	    ক্ষেত্র PV_LW:   ফেরত (char_u *)&(curbuf->b_p_lw);
	    ক্ষেত্র PV_BKC:  ফেরত (char_u *)&(curbuf->b_p_bkc);
	    ক্ষেত্র PV_MENC: ফেরত (char_u *)&(curbuf->b_p_menc);
	    ক্ষেত্র PV_LCS:  ফেরত (char_u *)&(curwin->w_p_lcs);
	    ক্ষেত্র PV_FCS:  ফেরত (char_u *)&(curwin->w_p_fcs);
	    ক্ষেত্র PV_VE:	  ফেরত (char_u *)&(curwin->w_p_ve);

	}
	ফেরত ফাঁকা; // "cannot happen"
    }
    ফেরত get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(পূর্ণসংখ্যা opt_idx, পূর্ণসংখ্যা scope)
{
    ফেরত get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    স্থির char_u *
get_varp(কাঠামো vimoption *p)
{
    // hidden option, always return NULL
    যদি (p->var == ফাঁকা)
	ফেরত ফাঁকা;

    নির্বাচন ((পূর্ণসংখ্যা)p->indir)
    {
	ক্ষেত্র PV_NONE:	ফেরত p->var;

	// global option with local value: use local value if it's been set
	ক্ষেত্র PV_EP:	ফেরত *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	ক্ষেত্র PV_KP:	ফেরত *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	ক্ষেত্র PV_PATH:	ফেরত *curbuf->b_p_path != NUL
				    ? (char_u *)&(curbuf->b_p_path) : p->var;
	ক্ষেত্র PV_AC:	ফেরত curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	ক্ষেত্র PV_AR:	ফেরত curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	ক্ষেত্র PV_TAGS:	ফেরত *curbuf->b_p_tags != NUL
				    ? (char_u *)&(curbuf->b_p_tags) : p->var;
	ক্ষেত্র PV_TC:	ফেরত *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	ক্ষেত্র PV_BKC:	ফেরত *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	ক্ষেত্র PV_SISO:	ফেরত curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	ক্ষেত্র PV_SO:	ফেরত curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	ক্ষেত্র PV_SOP:	ফেরত curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ifdef FEAT_FIND_ID
	ক্ষেত্র PV_DEF:	ফেরত *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	ক্ষেত্র PV_INC:	ফেরত *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#endif
	ক্ষেত্র PV_COT:	ফেরত *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	ক্ষেত্র PV_DICT:	ফেরত *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ifdef FEAT_DIFF
	ক্ষেত্র PV_DIA:	ফেরত *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#endif
	ক্ষেত্র PV_TSR:	ফেরত *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ifdef FEAT_COMPL_FUNC
	ক্ষেত্র PV_TSRFU:	ফেরত *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#endif
	ক্ষেত্র PV_FP:	ফেরত *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ifdef HAVE_FSYNC
	ক্ষেত্র PV_FS:	ফেরত curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#endif
#ifdef FEAT_EVAL
	ক্ষেত্র PV_FFU:	ফেরত *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#endif
#ifdef FEAT_QUICKFIX
	ক্ষেত্র PV_EFM:	ফেরত *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	ক্ষেত্র PV_GEFM:	ফেরত *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	ক্ষেত্র PV_GP:	ফেরত *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	ক্ষেত্র PV_MP:	ফেরত *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#endif
#যদি defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	ক্ষেত্র PV_BEXPR:	ফেরত *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#endif
#যদি defined(FEAT_CRYPT)
	ক্ষেত্র PV_CM:	ফেরত *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#endif
#ifdef FEAT_LINEBREAK
	ক্ষেত্র PV_SBR:	ফেরত *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#endif
#ifdef FEAT_STL_OPT
	ক্ষেত্র PV_STL:	ফেরত *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	ক্ষেত্র PV_STLO:	ফেরত *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#endif
	ক্ষেত্র PV_UL:	ফেরত curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	ক্ষেত্র PV_LW:	ফেরত *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	ক্ষেত্র PV_MENC:	ফেরত *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ifdef FEAT_ARABIC
	ক্ষেত্র PV_ARAB:	ফেরত (char_u *)&(curwin->w_p_arab);
#endif
	ক্ষেত্র PV_LIST:	ফেরত (char_u *)&(curwin->w_p_list);
	ক্ষেত্র PV_LCS:	ফেরত *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	ক্ষেত্র PV_FCS:	ফেরত *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	ক্ষেত্র PV_VE:	ফেরত *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ifdef FEAT_SPELL
	ক্ষেত্র PV_SPELL:	ফেরত (char_u *)&(curwin->w_p_spell);
#endif
#ifdef FEAT_SYN_HL
	ক্ষেত্র PV_CUC:	ফেরত (char_u *)&(curwin->w_p_cuc);
	ক্ষেত্র PV_CUL:	ফেরত (char_u *)&(curwin->w_p_cul);
	ক্ষেত্র PV_CULOPT:	ফেরত (char_u *)&(curwin->w_p_culopt);
	ক্ষেত্র PV_CC:	ফেরত (char_u *)&(curwin->w_p_cc);
#endif
#ifdef FEAT_DIFF
	ক্ষেত্র PV_DIFF:	ফেরত (char_u *)&(curwin->w_p_diff);
#endif
	ক্ষেত্র PV_EIW:	ফেরত (char_u *)&(curwin->w_p_eiw);
#ifdef FEAT_FOLDING
	ক্ষেত্র PV_FDC:	ফেরত (char_u *)&(curwin->w_p_fdc);
	ক্ষেত্র PV_FEN:	ফেরত (char_u *)&(curwin->w_p_fen);
	ক্ষেত্র PV_FDI:	ফেরত (char_u *)&(curwin->w_p_fdi);
	ক্ষেত্র PV_FDL:	ফেরত (char_u *)&(curwin->w_p_fdl);
	ক্ষেত্র PV_FDM:	ফেরত (char_u *)&(curwin->w_p_fdm);
	ক্ষেত্র PV_FML:	ফেরত (char_u *)&(curwin->w_p_fml);
	ক্ষেত্র PV_FDN:	ফেরত (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	ক্ষেত্র PV_FDE:	ফেরত (char_u *)&(curwin->w_p_fde);
	ক্ষেত্র PV_FDT:	ফেরত (char_u *)&(curwin->w_p_fdt);
# endif
	ক্ষেত্র PV_FMR:	ফেরত (char_u *)&(curwin->w_p_fmr);
#endif
	ক্ষেত্র PV_NU:	ফেরত (char_u *)&(curwin->w_p_nu);
	ক্ষেত্র PV_RNU:	ফেরত (char_u *)&(curwin->w_p_rnu);
#ifdef FEAT_LINEBREAK
	ক্ষেত্র PV_NUW:	ফেরত (char_u *)&(curwin->w_p_nuw);
#endif
	ক্ষেত্র PV_WFB:	ফেরত (char_u *)&(curwin->w_p_wfb);
	ক্ষেত্র PV_WFH:	ফেরত (char_u *)&(curwin->w_p_wfh);
	ক্ষেত্র PV_WFW:	ফেরত (char_u *)&(curwin->w_p_wfw);
#যদি defined(FEAT_QUICKFIX)
	ক্ষেত্র PV_PVW:	ফেরত (char_u *)&(curwin->w_p_pvw);
	ক্ষেত্র PV_LHI:	ফেরত (char_u *)&(curwin->w_p_lhi);
#endif
#ifdef FEAT_RIGHTLEFT
	ক্ষেত্র PV_RL:	ফেরত (char_u *)&(curwin->w_p_rl);
	ক্ষেত্র PV_RLC:	ফেরত (char_u *)&(curwin->w_p_rlc);
#endif
	ক্ষেত্র PV_SCROLL:	ফেরত (char_u *)&(curwin->w_p_scr);
	ক্ষেত্র PV_SMS:	ফেরত (char_u *)&(curwin->w_p_sms);
	ক্ষেত্র PV_WRAP:	ফেরত (char_u *)&(curwin->w_p_wrap);
#ifdef FEAT_LINEBREAK
	ক্ষেত্র PV_LBR:	ফেরত (char_u *)&(curwin->w_p_lbr);
	ক্ষেত্র PV_BRI:	ফেরত (char_u *)&(curwin->w_p_bri);
	ক্ষেত্র PV_BRIOPT: ফেরত (char_u *)&(curwin->w_p_briopt);
#endif
	ক্ষেত্র PV_WCR:	ফেরত (char_u *)&(curwin->w_p_wcr);
	ক্ষেত্র PV_SCBIND: ফেরত (char_u *)&(curwin->w_p_scb);
	ক্ষেত্র PV_CRBIND: ফেরত (char_u *)&(curwin->w_p_crb);
#ifdef FEAT_CONCEAL
	ক্ষেত্র PV_COCU:   ফেরত (char_u *)&(curwin->w_p_cocu);
	ক্ষেত্র PV_COLE:   ফেরত (char_u *)&(curwin->w_p_cole);
#endif
	ক্ষেত্র PV_WHL:	ফেরত (char_u *)&(curwin->w_p_whl);

#ifdef FEAT_TERMINAL
	ক্ষেত্র PV_TWK:    ফেরত (char_u *)&(curwin->w_p_twk);
	ক্ষেত্র PV_TWS:    ফেরত (char_u *)&(curwin->w_p_tws);
	ক্ষেত্র PV_TWSL:	ফেরত (char_u *)&(curbuf->b_p_twsl);
#endif

	ক্ষেত্র PV_AI:	ফেরত (char_u *)&(curbuf->b_p_ai);
	ক্ষেত্র PV_BIN:	ফেরত (char_u *)&(curbuf->b_p_bin);
	ক্ষেত্র PV_BOMB:	ফেরত (char_u *)&(curbuf->b_p_bomb);
	ক্ষেত্র PV_BH:	ফেরত (char_u *)&(curbuf->b_p_bh);
	ক্ষেত্র PV_BT:	ফেরত (char_u *)&(curbuf->b_p_bt);
	ক্ষেত্র PV_BL:	ফেরত (char_u *)&(curbuf->b_p_bl);
	ক্ষেত্র PV_CI:	ফেরত (char_u *)&(curbuf->b_p_ci);
	ক্ষেত্র PV_CIN:	ফেরত (char_u *)&(curbuf->b_p_cin);
	ক্ষেত্র PV_CINK:	ফেরত (char_u *)&(curbuf->b_p_cink);
	ক্ষেত্র PV_CINO:	ফেরত (char_u *)&(curbuf->b_p_cino);
	ক্ষেত্র PV_CINSD:	ফেরত (char_u *)&(curbuf->b_p_cinsd);
	ক্ষেত্র PV_CINW:	ফেরত (char_u *)&(curbuf->b_p_cinw);
	ক্ষেত্র PV_COM:	ফেরত (char_u *)&(curbuf->b_p_com);
	ক্ষেত্র PV_CMS:	ফেরত (char_u *)&(curbuf->b_p_cms);
	ক্ষেত্র PV_CPT:	ফেরত (char_u *)&(curbuf->b_p_cpt);
#ifdef BACKSLASH_IN_FILENAME
	ক্ষেত্র PV_CSL:	ফেরত (char_u *)&(curbuf->b_p_csl);
#endif
#ifdef FEAT_COMPL_FUNC
	ক্ষেত্র PV_CFU:	ফেরত (char_u *)&(curbuf->b_p_cfu);
	ক্ষেত্র PV_OFU:	ফেরত (char_u *)&(curbuf->b_p_ofu);
#endif
#ifdef FEAT_EVAL
	ক্ষেত্র PV_TFU:	ফেরত (char_u *)&(curbuf->b_p_tfu);
#endif
	ক্ষেত্র PV_EOF:	ফেরত (char_u *)&(curbuf->b_p_eof);
	ক্ষেত্র PV_EOL:	ফেরত (char_u *)&(curbuf->b_p_eol);
	ক্ষেত্র PV_FIXEOL:	ফেরত (char_u *)&(curbuf->b_p_fixeol);
	ক্ষেত্র PV_ET:	ফেরত (char_u *)&(curbuf->b_p_et);
	ক্ষেত্র PV_FENC:	ফেরত (char_u *)&(curbuf->b_p_fenc);
	ক্ষেত্র PV_FF:	ফেরত (char_u *)&(curbuf->b_p_ff);
	ক্ষেত্র PV_FT:	ফেরত (char_u *)&(curbuf->b_p_ft);
	ক্ষেত্র PV_FO:	ফেরত (char_u *)&(curbuf->b_p_fo);
	ক্ষেত্র PV_FLP:	ফেরত (char_u *)&(curbuf->b_p_flp);
	ক্ষেত্র PV_IMI:	ফেরত (char_u *)&(curbuf->b_p_iminsert);
	ক্ষেত্র PV_IMS:	ফেরত (char_u *)&(curbuf->b_p_imsearch);
	ক্ষেত্র PV_INF:	ফেরত (char_u *)&(curbuf->b_p_inf);
	ক্ষেত্র PV_ISK:	ফেরত (char_u *)&(curbuf->b_p_isk);
#ifdef FEAT_FIND_ID
# ifdef FEAT_EVAL
	ক্ষেত্র PV_INEX:	ফেরত (char_u *)&(curbuf->b_p_inex);
# endif
#endif
#যদি defined(FEAT_EVAL)
	ক্ষেত্র PV_INDE:	ফেরত (char_u *)&(curbuf->b_p_inde);
	ক্ষেত্র PV_INDK:	ফেরত (char_u *)&(curbuf->b_p_indk);
#endif
#ifdef FEAT_EVAL
	ক্ষেত্র PV_FEX:	ফেরত (char_u *)&(curbuf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
	ক্ষেত্র PV_চাবি:	ফেরত (char_u *)&(curbuf->b_p_চাবি);
#endif
	ক্ষেত্র PV_LISP:	ফেরত (char_u *)&(curbuf->b_p_lisp);
	ক্ষেত্র PV_LOP:	ফেরত (char_u *)&(curbuf->b_p_lop);
	ক্ষেত্র PV_ML:	ফেরত (char_u *)&(curbuf->b_p_ml);
	ক্ষেত্র PV_MPS:	ফেরত (char_u *)&(curbuf->b_p_mps);
	ক্ষেত্র PV_MA:	ফেরত (char_u *)&(curbuf->b_p_ma);
	ক্ষেত্র PV_MOD:	ফেরত (char_u *)&(curbuf->b_changed);
	ক্ষেত্র PV_NF:	ফেরত (char_u *)&(curbuf->b_p_nf);
	ক্ষেত্র PV_PI:	ফেরত (char_u *)&(curbuf->b_p_pi);
	ক্ষেত্র PV_QE:	ফেরত (char_u *)&(curbuf->b_p_qe);
	ক্ষেত্র PV_RO:	ফেরত (char_u *)&(curbuf->b_p_ro);
	ক্ষেত্র PV_SI:	ফেরত (char_u *)&(curbuf->b_p_si);
	ক্ষেত্র PV_SN:	ফেরত (char_u *)&(curbuf->b_p_sn);
	ক্ষেত্র PV_STS:	ফেরত (char_u *)&(curbuf->b_p_sts);
	ক্ষেত্র PV_SUA:	ফেরত (char_u *)&(curbuf->b_p_sua);
	ক্ষেত্র PV_SWF:	ফেরত (char_u *)&(curbuf->b_p_swf);
#ifdef FEAT_SYN_HL
	ক্ষেত্র PV_SMC:	ফেরত (char_u *)&(curbuf->b_p_smc);
	ক্ষেত্র PV_SYN:	ফেরত (char_u *)&(curbuf->b_p_syn);
#endif
#ifdef FEAT_SPELL
	ক্ষেত্র PV_SPC:	ফেরত (char_u *)&(curwin->w_s->b_p_spc);
	ক্ষেত্র PV_SPF:	ফেরত (char_u *)&(curwin->w_s->b_p_spf);
	ক্ষেত্র PV_SPL:	ফেরত (char_u *)&(curwin->w_s->b_p_spl);
	ক্ষেত্র PV_SPO:	ফেরত (char_u *)&(curwin->w_s->b_p_spo);
#endif
	ক্ষেত্র PV_SW:	ফেরত (char_u *)&(curbuf->b_p_sw);
	ক্ষেত্র PV_TS:	ফেরত (char_u *)&(curbuf->b_p_ts);
	ক্ষেত্র PV_TW:	ফেরত (char_u *)&(curbuf->b_p_tw);
	ক্ষেত্র PV_TX:	ফেরত (char_u *)&(curbuf->b_p_tx);
#ifdef FEAT_PERSISTENT_UNDO
	ক্ষেত্র PV_UDF:	ফেরত (char_u *)&(curbuf->b_p_udf);
#endif
	ক্ষেত্র PV_WM:	ফেরত (char_u *)&(curbuf->b_p_wm);
#ifdef FEAT_KEYMAP
	ক্ষেত্র PV_KMAP:	ফেরত (char_u *)&(curbuf->b_p_keymap);
#endif
#ifdef FEAT_SIGNS
	ক্ষেত্র PV_SCL:	ফেরত (char_u *)&(curwin->w_p_scl);
#endif
#ifdef FEAT_VARTABS
	ক্ষেত্র PV_VSTS:	ফেরত (char_u *)&(curbuf->b_p_vsts);
	ক্ষেত্র PV_VTS:	ফেরত (char_u *)&(curbuf->b_p_vts);
#endif
	পূর্বনির্ধারিত:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    ফেরত (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(পূর্ণসংখ্যা opt_idx)
{
    ফেরত options[opt_idx].var;
}

#যদি defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(পূর্ণসংখ্যা opt_idx)
{
    ফেরত (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    পূর্ণসংখ্যা
option_info(char_u *name, dict_T *d)
{
    পূর্ণসংখ্যা			opt_idx;
    কাঠামো vimoption	*p;
    পূর্ণসংখ্যা			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    যদি ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    যদি (opt_idx < 0)
	ফেরত FAIL;

    p = &options[opt_idx];
    dict_add_string(d, "name", (char_u *)p->fullname);
    dict_add_string(d, "shortname",
		    (char_u *)(p->shortname == ফাঁকা ? "" : p->shortname));
    dict_add_bool(d, "available", p->var != ফাঁকা);
    dict_add_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_অঙ্ক) ? "number" : "string"));
    dict_add_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    যদি (p->flags & P_BOOL)
	dict_add_bool(d, "default", (পূর্ণসংখ্যা)(long_i)p->def_val[dvi]);
    অন্যথা যদি (p->flags & P_অঙ্ক)
	dict_add_সংখ্যা(d, "default", (long)(long_i)p->def_val[dvi]);
    অন্যথা
	dict_add_string(d, "default", p->def_val[dvi] == ফাঁকা
					? (char_u *)"" : p->def_val[dvi]);
    ফেরত OK;
}
#endif

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_set_cb_T
get_option_did_set_cb(পূর্ণসংখ্যা opt_idx)
{
    ফেরত options[opt_idx].opt_did_set_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(শূন্য)
{
    যদি (*curbuf->b_p_ep == NUL)
	ফেরত p_ep;
    ফেরত curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(শূন্য)
{
#ifdef FEAT_EVAL
    যদি (*curbuf->b_p_ffu == NUL)
	ফেরত p_ffu;
    ফেরত curbuf->b_p_ffu;
#অন্যথা
    ফেরত (char_u *)"";
#endif
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    শূন্য
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
    শূন্য
after_copy_winopt(win_T *wp)
{
    অক্ষর *errmsg = update_winhighlight(wp, wp->w_p_whl);

    যদি (errmsg != ফাঁকা)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    যদি (wp->w_p_wrap)
	wp->w_leftcol = 0;
    অন্যথা
	wp->w_skipcol = 0;
#ifdef FEAT_LINEBREAK
    briopt_check(ফাঁকা, wp);
#endif
#ifdef FEAT_SYN_HL
    fill_culopt_flags(ফাঁকা, wp);
    check_colorcolumn(ফাঁকা, wp);
#endif
    set_listchars_option(wp, wp->w_p_lcs, TRUE, ফাঁকা, 0);
    set_fillchars_option(wp, wp->w_p_fcs, TRUE, ফাঁকা, 0);
}

    স্থির char_u *
copy_option_val(char_u *val)
{
    যদি (val == empty_option)
	ফেরত empty_option;  // no need to allocate memory
    ফেরত vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    শূন্য
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
						    আকার(to->wo_script_ctx));
#endif
    check_winopt(to);		// don't want NULL pointers
}

/*
 * Check string options in a window for a NULL value.
 */
    স্থির শূন্য
check_win_options(win_T *win)
{
    check_winopt(&win->w_onebuf_opt);
    check_winopt(&win->w_allbuf_opt);
}

/*
 * Check for NULL pointers in a winopt_T and replace them with empty_option.
 */
    স্থির শূন্য
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
    শূন্য
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
স্থির পূর্ণসংখ্যা buf_opt_idx[BV_গুনতি];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    স্থির শূন্য
init_buf_opt_idx(শূন্য)
{
    স্থির পূর্ণসংখ্যা did_init_buf_opt_idx = FALSE;
    পূর্ণসংখ্যা i;

    যদি (did_init_buf_opt_idx)
	ফেরত;
    did_init_buf_opt_idx = TRUE;
    জন্য (i = 0; !istermoption_idx(i); i++)
	যদি (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_MASK] = i;
}
#অন্যথা
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
    শূন্য
buf_copy_options(buf_T *buf, পূর্ণসংখ্যা flags)
{
    পূর্ণসংখ্যা		should_copy = TRUE;
    char_u	*save_p_isk = ফাঁকা;	    // init for GCC
    পূর্ণসংখ্যা		dont_do_help;
    পূর্ণসংখ্যা		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    যদি (p_cpo != ফাঁকা)
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
	যদি ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == ফাঁকা || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != ফাঁকা)))
	    should_copy = FALSE;

	যদি (should_copy || (flags & BCO_ALWAYS))
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
	    যদি (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = ফাঁকা;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    যদি (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		নির্বাচন (*p_ffs)
		{
		    ক্ষেত্র 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); বিরতি;
		    ক্ষেত্র 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); বিরতি;
		    ক্ষেত্র 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); বিরতি;
		    পূর্বনির্ধারিত:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		যদি (buf->b_p_ff != ফাঁকা)
		    buf->b_start_ffc = *buf->b_p_ff;
		buf->b_p_bh = empty_option;
		buf->b_p_bt = empty_option;
	    }
	    অন্যথা
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
	    যদি (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    অন্যথা
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
	    যদি (p_vsts && p_vsts != empty_option)
		(শূন্য)tabstop_set(p_vsts, &buf->b_p_vsts_array);
	    অন্যথা
		buf->b_p_vsts_array = ফাঁকা;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : ফাঁকা;
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
	    (শূন্য)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#endif
#যদি defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#endif
	    buf->b_p_fp = empty_option;
#যদি defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#endif
#ifdef FEAT_CRYPT
	    buf->b_p_চাবি = vim_strsave(p_চাবি);
	    COPY_OPT_SCTX(buf, BV_চাবি);
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
#যদি defined(FEAT_EVAL)
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
#যদি defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = empty_option;
#endif
#যদি defined(FEAT_CRYPT)
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
	    যদি (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ifdef FEAT_VARTABS
		যদি (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (শূন্য)tabstop_set(p_vts, &buf->b_p_vts_array);
		অন্যথা
		    buf->b_p_vts_array = ফাঁকা;
#endif
	    }
	    অন্যথা
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#ifdef FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		যদি (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (শূন্য)tabstop_set(p_vts, &buf->b_p_vts_array);
		অন্যথা
		    buf->b_p_vts_array = ফাঁকা;
#endif
		buf->b_help = false;
		যদি (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	যদি (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    যদি (did_isk)
	(শূন্য)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    শূন্য
reset_modifiable(শূন্য)
{
    পূর্ণসংখ্যা		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    যদি (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}

/*
 * Set the global value for 'iminsert' to the local value.
 */
    শূন্য
set_iminsert_global(শূন্য)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    শূন্য
set_imsearch_global(শূন্য)
{
    p_imsearch = curbuf->b_p_imsearch;
}

স্থির পূর্ণসংখ্যা expand_option_idx = -1;
স্থির পূর্ণসংখ্যা expand_option_start_col = 0;
স্থির char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
স্থির পূর্ণসংখ্যা expand_option_flags = 0;
স্থির পূর্ণসংখ্যা expand_option_append = FALSE;

    শূন্য
set_context_in_set_cmd(
    expand_T	*xp,
    char_u	*arg,
    পূর্ণসংখ্যা		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    পূর্ণসংখ্যা		nextchar;
    long_u	flags = 0;	// init for GCC
    পূর্ণসংখ্যা		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    পূর্ণসংখ্যা		is_term_option = FALSE;
    পূর্ণসংখ্যা		চাবি;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    যদি (*arg == NUL)
    {
	xp->xp_pattern = arg;
	ফেরত;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    যদি (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_pattern = p + 1;
	ফেরত;
    }
    যখন (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	যদি (*p == ' ' || *p == ',')
	{
	    যখন (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	যদি (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    বিরতি;
	}
	--p;
    }
    যদি (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_NO;
	p += 2;
    }
    অন্যথা যদি (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_INV;
	p += 3;
    }
    xp->xp_pattern = arg = p;
    যদি (*arg == '<')
    {
	যখন (*p != '>')
	    যদি (*p++ == NUL)	    // expand terminal option name
		ফেরত;
	চাবি = get_special_চাবি_code(arg + 1);
	যদি (চাবি == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    ফেরত;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(চাবি);
	expand_option_name[3] = KEY2TERMCAP1(চাবি);
    }
    অন্যথা
    {
	যদি (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    যদি (*p != NUL)
		++p;
	    যদি (*p == NUL)
		ফেরত;		// expand option name
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	অন্যথা
	{
	    // Allow * wildcard
	    যখন (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    যদি (*p == NUL)
		ফেরত;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    যদি (opt_idx == -1 || options[opt_idx].var == ফাঁকা)
	    {
		xp->xp_context = EXPAND_NOTHING;
		ফেরত;
	    }
	    flags = options[opt_idx].flags;
	    যদি (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		ফেরত;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    পূর্ণসংখ্যা expand_option_subtract = FALSE;
    যদি ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	যদি (nextchar == '-')
	    expand_option_subtract = TRUE;
	যদি (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    যদি ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ফেরত;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    যদি (is_term_option)
	expand_option_idx = -1;
    অন্যথা
	expand_option_idx = opt_idx;

    যদি (!is_term_option)
    {
	যদি (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    ফেরত;
	}
    }

    xp->xp_pattern = p + 1;
    expand_option_start_col = (পূর্ণসংখ্যা)(p + 1 - xp->xp_সারি);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ifdef FEAT_SYN_HL
    যদি (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	ফেরত;
    }
#endif
    যদি (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	ফেরত;
    }
#ifdef FEAT_KEYMAP
    যদি (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	ফেরত;
    }
#endif

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    যদি (expand_option_subtract)
    {
	xp->xp_context = EXPAND_SETTING_SUBTRACT;
	ফেরত;
    }
    অন্যথা যদি (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != ফাঁকা)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    অন্যথা যদি (*xp->xp_pattern == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	ফেরত;
    }
    অন্যথা
	xp->xp_context = EXPAND_NOTHING;

    যদি (is_term_option || (flags & P_অঙ্ক))
	ফেরত;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    যদি (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	যদি (p == (char_u *)&p_bdir
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
	    যদি (p == (char_u *)&p_path || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    অন্যথা
		xp->xp_backslash = XP_BS_ONE;
	}
	অন্যথা
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    যদি (p == (char_u *)&p_tags)
		xp->xp_backslash = XP_BS_THREE;
	    অন্যথা
		xp->xp_backslash = XP_BS_ONE;
	}
	যদি (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    যদি ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	জন্য (p = argend - 1; p >= xp->xp_pattern; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    যদি (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		যখন (s > xp->xp_pattern && *(s - 1) == '\\')
		    --s;
		যদি ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#যদি defined(BACKSLASH_IN_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#অন্যথা
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#endif
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_pattern = p + 1;
		    বিরতি;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    যদি (flags & P_FLAGLIST)
	xp->xp_pattern = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ifdef FEAT_SPELL
    যদি (options[opt_idx].var == (char_u *)&p_sps)
    {
	যদি (STRNCMP(xp->xp_pattern, "file:", 5) == 0)
	{
	    xp->xp_pattern += 5;
	    ফেরত;
	}
	অন্যথা যদি (options[expand_option_idx].opt_expand_cb != ফাঁকা)
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
    স্থির পূর্ণসংখ্যা
match_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	পূর্ণসংখ্যা		idx,
	পূর্ণসংখ্যা		test_only,
	পূর্ণসংখ্যা		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    যদি (!fuzzy)
    {
	যদি (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    যদি (!test_only)
		matches[idx] = vim_strsave(str);
	    ফেরত TRUE;
	}
    }
    অন্যথা
    {
	পূর্ণসংখ্যা score;

	score = fuzzy_match_str(str, fuzzystr);
	যদি (score != FUZZY_SCORE_NONE)
	{
	    যদি (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    ফেরত TRUE;
	}
    }

    ফেরত FALSE;
}

    পূর্ণসংখ্যা
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    পূর্ণসংখ্যা		*অঙ্ক_Matches,
    char_u	***matches,
    পূর্ণসংখ্যা		can_fuzzy)
{
    পূর্ণসংখ্যা		অঙ্ক_normal = 0;	    // Nr of matching non-term-code settings
    পূর্ণসংখ্যা		অঙ্ক_term = 0;	    // Nr of matching terminal code settings
    পূর্ণসংখ্যা		opt_idx;
    পূর্ণসংখ্যা		match;
    পূর্ণসংখ্যা		গুনতি = 0;
    char_u	*str;
    পূর্ণসংখ্যা		loop;
    পূর্ণসংখ্যা		is_term_opt;
    char_u	name_buf[MAX_চাবি_NAME_LEN];
    স্থির অক্ষর *(names[]) = {"all", "termcap"};
    পূর্ণসংখ্যা		ic = regmatch->rm_ic;	// remember the ignore-case flag
    পূর্ণসংখ্যা		fuzzy;
    fuzmatch_str_T  *fuzmatch = ফাঁকা;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    জন্য (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	যদি (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    জন্য (match = 0; match < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(names); ++match)
	    {
		যদি (match_str((char_u *)names[match], regmatch, *matches,
			    গুনতি, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    যদি (loop == 0)
			অঙ্ক_normal++;
		    অন্যথা
			গুনতি++;
		}
	    }
	}
	জন্য (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != ফাঁকা;
								    opt_idx++)
	{
	    যদি (options[opt_idx].var == ফাঁকা)
		চালু;
	    যদি (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		চালু;
	    is_term_opt = istermoption_idx(opt_idx);
	    যদি (is_term_opt && অঙ্ক_normal > 0)
		চালু;

	    যদি (match_str(str, regmatch, *matches, গুনতি, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		যদি (loop == 0)
		{
		    যদি (is_term_opt)
			অঙ্ক_term++;
		    অন্যথা
			অঙ্ক_normal++;
		}
		অন্যথা
		    গুনতি++;
	    }
	    অন্যথা যদি (!fuzzy && options[opt_idx].shortname != ফাঁকা
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		যদি (loop == 0)
		{
		    যদি (is_term_opt)
			অঙ্ক_term++;
		    অন্যথা
			অঙ্ক_normal++;
		}
		অন্যথা
		    (*matches)[গুনতি++] = vim_strsave(str);
	    }
	    অন্যথা যদি (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		যদি (match_str(name_buf, regmatch, *matches, গুনতি, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    যদি (loop == 0)
			অঙ্ক_term++;
		    অন্যথা
			গুনতি++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	যদি (xp->xp_context != EXPAND_BOOL_SETTINGS  && অঙ্ক_normal == 0)
	{
	    জন্য (opt_idx = 0; (str = get_termcode(opt_idx)) != ফাঁকা; opt_idx++)
	    {
		যদি (!SAFE_isprint(str[0]) || !SAFE_isprint(str[1]))
		    চালু;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		যদি (match_str(name_buf, regmatch, *matches, গুনতি,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    যদি (loop == 0)
			অঙ্ক_term++;
		    অন্যথা
			গুনতি++;
		}
		অন্যথা
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    যদি (match_str(name_buf, regmatch, *matches, গুনতি,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			যদি (loop == 0)
			    অঙ্ক_term++;
			অন্যথা
			    গুনতি++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    জন্য (opt_idx = 0; (str = get_চাবি_name(opt_idx)) != ফাঁকা; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		যদি (match_str(name_buf, regmatch, *matches, গুনতি, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    যদি (loop == 0)
			অঙ্ক_term++;
		    অন্যথা
			গুনতি++;
		}
	    }
	}
	যদি (loop == 0)
	{
	    যদি (অঙ্ক_normal > 0)
		*অঙ্ক_Matches = অঙ্ক_normal;
	    অন্যথা যদি (অঙ্ক_term > 0)
		*অঙ্ক_Matches = অঙ্ক_term;
	    অন্যথা
		ফেরত OK;
	    যদি (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *অঙ্ক_Matches);
		যদি (*matches == ফাঁকা)
		{
		    *matches = (char_u **)"";
		    ফেরত FAIL;
		}
	    }
	    অন্যথা
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *অঙ্ক_Matches);
		যদি (fuzmatch == ফাঁকা)
		{
		    *matches = (char_u **)"";
		    ফেরত FAIL;
		}
	    }
	}
    }

    যদি (fuzzy &&
	fuzzymatches_to_strmatches(fuzmatch, matches, গুনতি, FALSE) == FAIL)
	ফেরত FAIL;

    ফেরত OK;
}

// Escape an option value that can be used on the command-line with :set.
// Caller needs to free the returned string, unless NULL is returned.
    স্থির char_u*
escape_option_str_cmdline(char_u *var)
{
    char_u *buf;

    // A backslash is required before some characters.  This is the reverse of
    // what happens in do_set().
    buf = vim_strsave_escaped(var, escape_chars);
    যদি (buf == ফাঁকা)
	ফেরত ফাঁকা;

#ifdef BACKSLASH_IN_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    জন্য (var = buf; *var != NUL; MB_PTR_ADV(var))
	যদি (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#endif
    ফেরত buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    পূর্ণসংখ্যা
ExpandOldSetting(পূর্ণসংখ্যা *অঙ্ক_Matches, char_u ***matches)
{
    char_u  *var = ফাঁকা;	// init for GCC
    char_u  *buf;

    *অঙ্ক_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    যদি (*matches == ফাঁকা)
	ফেরত FAIL;

    // For a terminal key code expand_option_idx is < 0.
    যদি (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	যদি (var == ফাঁকা)
	    expand_option_idx = findoption(expand_option_name);
    }

    যদি (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    অন্যথা যদি (var == ফাঁকা)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    যদি (buf == ফাঁকা)
    {
	VIM_CLEAR(*matches);
	ফেরত FAIL;
    }

    (*matches)[0] = buf;
    *অঙ্ক_Matches = 1;
    ফেরত OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    পূর্ণসংখ্যা
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    পূর্ণসংখ্যা		*অঙ্ক_Matches,
    char_u	***matches)
{
    char_u  *var = ফাঁকা;	// init for GCC
    char_u  *buf;

    যদি (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == ফাঁকা)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	ফেরত FAIL;
    }

    optexpand_T args;
    args.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    args.oe_append = expand_option_append;
    args.oe_regmatch = regmatch;
    args.oe_xp = xp;
    args.oe_set_arg = xp->xp_সারি + expand_option_start_col;
    args.oe_include_orig_val =
	!expand_option_append &&
	(*args.oe_set_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    যদি (buf == ফাঁকা)
	ফেরত FAIL;

    args.oe_opt_মান = buf;

    পূর্ণসংখ্যা অঙ্ক_ret = options[expand_option_idx].opt_expand_cb(&args, অঙ্ক_Matches, matches);

    vim_free(buf);
    ফেরত অঙ্ক_ret;
}

/*
 * Expansion handler for :set-=
 */
    পূর্ণসংখ্যা
ExpandSettingSubtract(
    expand_T	*xp,
    regmatch_T	*regmatch,
    পূর্ণসংখ্যা		*অঙ্ক_Matches,
    char_u	***matches)
{
    যদি (expand_option_idx < 0)
	// term option
	ফেরত ExpandOldSetting(অঙ্ক_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    যদি (option_flags & P_অঙ্ক)
	ফেরত ExpandOldSetting(অঙ্ক_Matches, matches);
    অন্যথা যদি (option_flags & P_COMMA)
    {
	// Split the option by comma, then present each option to the user if
	// it matches the pattern.
	// This condition needs to go first, because 'whichwrap' has both
	// P_COMMA and P_FLAGLIST.
	garray_T    ga;

	char_u	    *item;
	char_u	    *option_copy;
	char_u	    *পরবর্তী_val;
	char_u	    *comma;

	যদি (*option_val == NUL)
	    ফেরত FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	যদি (option_copy == ফাঁকা)
	    ফেরত FAIL;
	পরবর্তী_val = option_copy;

	ga_init2(&ga, আকার(char_u *), 10);

	করো
	{
	    item = পরবর্তী_val;
	    comma = vim_strchr(পরবর্তী_val, ',');
	    যখন (comma != ফাঁকা && comma != পরবর্তী_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    যদি (comma != ফাঁকা)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		পরবর্তী_val = comma + 1;
	    }
	    অন্যথা
		পরবর্তী_val = ফাঁকা;

	    যদি (*item == NUL)
		// empty value, don't add to list
		চালু;

	    যদি (!vim_regexec(regmatch, item, (colnr_T)0))
		চালু;

	    char_u *buf = escape_option_str_cmdline(item);
	    যদি (buf == ফাঁকা)
	    {
		vim_free(option_copy);
		ga_clear_strings(&ga);
		ফেরত FAIL;
	    }
	    যদি (ga_add_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		বিরতি;
	    }
	} যখন (পরবর্তী_val != ফাঁকা);

	vim_free(option_copy);

	*matches = ga.ga_তথ্য;
	*অঙ্ক_Matches = ga.ga_len;
	ফেরত OK;
    }
    অন্যথা যদি (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	যদি (*xp->xp_pattern != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    ফেরত FAIL;
	}

	size_t অঙ্ক_flags = STRLEN(option_val);
	যদি (অঙ্ক_flags == 0)
	    ফেরত FAIL;

	*matches = ALLOC_MULT(char_u *, অঙ্ক_flags + 1);
	যদি (*matches == ফাঁকা)
	    ফেরত FAIL;

	পূর্ণসংখ্যা গুনতি = 0;
	char_u *p;

	p = vim_strnsave(option_val, অঙ্ক_flags);
	যদি (p == ফাঁকা)
	{
	    VIM_CLEAR(*matches);
	    ফেরত FAIL;
	}
	(*matches)[গুনতি++] = p;

	যদি (অঙ্ক_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    জন্য (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		যদি (p == ফাঁকা)
		    বিরতি;
		(*matches)[গুনতি++] = p;
	    }
	}

	*অঙ্ক_Matches = গুনতি;
	ফেরত OK;
    }

    ফেরত ExpandOldSetting(অঙ্ক_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    স্থির শূন্য
option_value2string(
    কাঠামো vimoption	*opp,
    পূর্ণসংখ্যা			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    যদি (opp->flags & P_অঙ্ক)
    {
	long wc = 0;

	যদি (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_চাবি_name((পূর্ণসংখ্যা)wc, 0));
	অন্যথা যদি (wc != 0)
	    STRCPY(NameBuff, transchar((পূর্ণসংখ্যা)wc));
	অন্যথা
	    sprintf((অক্ষর *)NameBuff, "%ld", *(long *)varp);
    }
    অন্যথা    // P_STRING
    {
	varp = *(char_u **)(varp);
	যদি (varp == ফাঁকা)		    // just in case
	    NameBuff[0] = NUL;
#ifdef FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	অন্যথা যদি (opp->var == (char_u *)&p_চাবি && *varp)
	    STRCPY(NameBuff, "*****");
#endif
	অন্যথা যদি (opp->flags & P_EXPAND)
	    home_replace(ফাঁকা, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	অন্যথা যদি ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	অন্যথা
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    স্থির পূর্ণসংখ্যা
wc_use_keyname(char_u *varp, long *wcp)
{
    যদি (((long *)varp == &p_wc) || ((long *)varp == &p_wcm))
    {
	*wcp = *(long *)varp;
	যদি (IS_SPECIAL(*wcp) || find_special_চাবি_in_table((পূর্ণসংখ্যা)*wcp) >= 0)
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    পূর্ণসংখ্যা
shortmess(পূর্ণসংখ্যা x)
{
    ফেরত p_shm != ফাঁকা &&
	    (   vim_strchr(p_shm, x) != ফাঁকা
	    || (vim_strchr(p_shm, 'a') != ফাঁকা
		&& vim_strchr((char_u *)SHM_A, x) != ফাঁকা));
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 * When "fname" is not NULL, use it to set $"envname" when it wasn't set yet.
 */
    শূন্য
vimrc_found(char_u *fname, char_u *envname)
{
    পূর্ণসংখ্যা		opt_idx;
    পূর্ণসংখ্যা		dofree = FALSE;
    char_u	*p;

    যদি (!option_was_set((char_u *)"cp"))
    {
	p_cp = FALSE;
	জন্য (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    যদি (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    যদি (fname != ফাঁকা)
    {
	p = vim_getenv(envname, &dofree);
	যদি (p == ফাঁকা)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = FullName_save(fname, FALSE);
	    যদি (p != ফাঁকা)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	অন্যথা যদি (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    শূন্য
change_compatible(পূর্ণসংখ্যা on)
{
    পূর্ণসংখ্যা	    opt_idx;

    যদি (p_cp != on)
    {
	p_cp = on;
	compatible_set();
    }
    opt_idx = findoption((char_u *)"cp");
    যদি (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_SET;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    পূর্ণসংখ্যা
option_was_set(char_u *name)
{
    পূর্ণসংখ্যা idx;

    idx = findoption(name);
    যদি (idx < 0)	// unknown option
	ফেরত FALSE;
    যদি (options[idx].flags & P_WAS_SET)
	ফেরত TRUE;
    ফেরত FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    পূর্ণসংখ্যা
reset_option_was_set(char_u *name)
{
    পূর্ণসংখ্যা idx = findoption(name);

    যদি (idx < 0)
	ফেরত FAIL;

    options[idx].flags &= ~P_WAS_SET;
    ফেরত OK;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    স্থির শূন্য
compatible_set(শূন্য)
{
    পূর্ণসংখ্যা	    opt_idx;

    জন্য (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	যদি (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    set_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    পূর্ণসংখ্যা
can_bs(
    পূর্ণসংখ্যা		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ifdef FEAT_JOB_CHANNEL
    যদি (what == BS_START && bt_prompt(curbuf))
	ফেরত FALSE;
#endif
    নির্বাচন (*p_bs)
    {
	ক্ষেত্র '3':       ফেরত TRUE;
	ক্ষেত্র '2':	ফেরত (what != BS_NOSTOP);
	ক্ষেত্র '1':	ফেরত (what != BS_START);
	ক্ষেত্র '0':	ফেরত FALSE;
    }
    ফেরত vim_strchr(p_bs, what) != ফাঁকা;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloff_মান(শূন্য)
{
    ফেরত curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloffpad_মান(শূন্য)
{
    ফেরত curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_sidescrolloff_মান(শূন্য)
{
    ফেরত curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    unsigned পূর্ণসংখ্যা
get_bkc_flags(buf_T *buf)
{
    ফেরত buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#যদি defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_মান(buf_T *buf)
{
    যদি (buf->b_p_flp == ফাঁকা || *buf->b_p_flp == NUL)
	ফেরত p_flp;
    ফেরত buf->b_p_flp;
}
#endif

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    unsigned পূর্ণসংখ্যা
get_ve_flags(শূন্য)
{
    ফেরত (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#যদি defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_মান(win_T *win)
{
    যদি (win->w_p_sbr == ফাঁকা || *win->w_p_sbr == NUL)
	ফেরত p_sbr;
    যদি (STRCMP(win->w_p_sbr, "NONE") == 0)
	ফেরত empty_option;
    ফেরত win->w_p_sbr;
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(পূর্ণসংখ্যা bufopt)
{
    dict_T	*d;
    পূর্ণসংখ্যা		opt_idx;

    d = dict_alloc();
    যদি (d == ফাঁকা)
	ফেরত ফাঁকা;

    জন্য (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	কাঠামো vimoption *opt = &options[opt_idx];

	যদি ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    যদি (varp != ফাঁকা)
	    {
		যদি (opt->flags & P_STRING)
		    dict_add_string(d, opt->fullname, *(char_u **)varp);
		অন্যথা যদি (opt->flags & P_অঙ্ক)
		    dict_add_সংখ্যা(d, opt->fullname, *(long *)varp);
		অন্যথা
		    dict_add_সংখ্যা(d, opt->fullname, *(পূর্ণসংখ্যা *)varp);
	    }
	}
    }

    ফেরত d;
}
#endif

#যদি defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    পূর্ণসংখ্যা
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    যদি (val == ফাঁকা)
	p = wp->w_p_culopt;
    অন্যথা
	p = val;
    যখন (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	যদি (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_সারি;
	}
	অন্যথা যদি (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_সারি | CULOPT_NBR;
	}
	অন্যথা যদি (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	অন্যথা যদি (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	যদি (*p != ',' && *p != NUL)
	    ফেরত FAIL;
	যদি (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    যদি ((culopt_flags_new & CULOPT_সারি) && (culopt_flags_new & CULOPT_SCRLINE))
	ফেরত FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    ফেরত OK;
}
#endif

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    পূর্ণসংখ্যা
magic_isset(শূন্য)
{
    নির্বাচন (magic_overruled)
    {
	ক্ষেত্র OPTION_MAGIC_ON:      ফেরত TRUE;
	ক্ষেত্র OPTION_MAGIC_OFF:     ফেরত FALSE;
	ক্ষেত্র OPTION_MAGIC_NOT_SET: বিরতি;
    }
#ifdef FEAT_EVAL
    যদি (in_vim9script())
	ফেরত TRUE;
#endif
    ফেরত p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    পূর্ণসংখ্যা
option_set_callback_func(char_u *optval UNUSED, callback_T *optcb UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    পূর্ণসংখ্যা		funcname = FALSE;

    যদি (optval == ফাঁকা || *optval == NUL)
    {
	free_callback(optcb);
	ফেরত OK;
    }

    যদি (*optval == '{' || (in_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_রাশিমালা(optval, ফাঁকা);
    অন্যথা
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    যদি (tv == ফাঁকা)
	ফেরত FAIL;

    cb = get_callback(tv);
    যদি (cb.cb_name == ফাঁকা || *cb.cb_name == NUL)
    {
	free_tv(tv);
	ফেরত FAIL;
    }

    free_callback(optcb);
    set_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = ফাঁকা;
    যদি (in_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != ফাঁকা))
    {
	যদি (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = find_imported(optval, dot - optval, FALSE);
	যদি (import != ফাঁকা && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((অক্ষর *)fnamebuf, আকার(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	অন্যথা
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

    ফেরত OK;
#অন্যথা
    ফেরত FAIL;
#endif
}

#যদি defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    অক্ষর *
did_set_showtabpanel(optset_T *args UNUSED)
{
    shell_new_columns();
    ফেরত ফাঁকা;
}
#endif

#যদি defined(FEAT_EVAL)
    স্থির শূন্য
didset_options_sctx(পূর্ণসংখ্যা opt_flags, অক্ষর **buf)
{
	জন্য (পূর্ণসংখ্যা i = 0; ; ++i)
	{
	    যদি (buf[i] == ফাঁকা)
		বিরতি;

	    পূর্ণসংখ্যা idx = findoption((char_u *)buf[i]);
	    যদি (idx >= 0)
		set_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#endif
