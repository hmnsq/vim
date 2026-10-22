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
#अन्तर्भाव "vim.h"
#अन्तर्भाव "optiondefs.h"

स्थिर शून्य set_options_default(पूर्णाङ्क opt_flags);
स्थिर शून्य set_string_default_esc(वर्ण *name, char_u *val, पूर्णाङ्क escape);
स्थिर char_u *find_dup_item(char_u *origval, char_u *newval, माप_प्रकार newvallen, long_u flags);
स्थिर char_u *option_expand(पूर्णाङ्क opt_idx, char_u *val);
स्थिर शून्य didset_options(शून्य);
स्थिर शून्य didset_options2(शून्य);
#यदि defined(FEAT_EVAL)
स्थिर long_u *insecure_flag(win_T *wp, पूर्णाङ्क opt_idx, पूर्णाङ्क opt_flags);
#अन्यथा
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#endif
स्थिर वर्ण *set_bool_option(पूर्णाङ्क opt_idx, char_u *varp, पूर्णाङ्क मान, पूर्णाङ्क opt_flags);
स्थिर वर्ण *set_अङ्क_option(पूर्णाङ्क opt_idx, char_u *varp, दीर्घ मान, वर्ण *errbuf, माप_प्रकार errbuflen, पूर्णाङ्क opt_flags);
स्थिर पूर्णाङ्क find_साँचो_option(char_u *arg_arg, पूर्णाङ्क has_lt);
स्थिर शून्य showoptions(पूर्णाङ्क all, पूर्णाङ्क opt_flags);
स्थिर पूर्णाङ्क optval_default(संरचना vimoption *, char_u *varp, पूर्णाङ्क compatible);
स्थिर शून्य showoneopt(संरचना vimoption *, पूर्णाङ्क opt_flags);
स्थिर पूर्णाङ्क put_setstring(खाता *fd, bool legacy, वर्ण *cmd, वर्ण *name, char_u **valuep, long_u flags);
स्थिर पूर्णाङ्क put_setnum(खाता *fd, वर्ण *cmd, वर्ण *name, दीर्घ *valuep);
स्थिर पूर्णाङ्क put_setbool(खाता *fd, वर्ण *cmd, वर्ण *name, पूर्णाङ्क मान);
स्थिर पूर्णाङ्क istermoption(संरचना vimoption *p);
स्थिर पूर्णाङ्क istermoption_idx(पूर्णाङ्क opt_idx);
स्थिर char_u *get_varp_scope(संरचना vimoption *p, पूर्णाङ्क scope);
स्थिर char_u *get_varp(संरचना vimoption *);
स्थिर शून्य check_win_options(win_T *win);
स्थिर शून्य option_value2string(संरचना vimoption *, पूर्णाङ्क scope);
स्थिर शून्य check_winopt(winopt_T *wop);
स्थिर पूर्णाङ्क wc_use_keyname(char_u *varp, दीर्घ *wcp);
स्थिर शून्य compatible_set(शून्य);

#यदि defined(FEAT_EVAL)
स्थिर वर्ण *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", रिक्त};
स्थिर वर्ण *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    रिक्त};
स्थिर शून्य didset_options_sctx(पूर्णाङ्क opt_flags, वर्ण **buf);
#endif


/*
 * Initialize the 'shell' option to a default value.
 */
    स्थिर शून्य
set_init_default_shell(शून्य)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    यदि (((p = mch_getenv((char_u *)"SHELL")) != रिक्त && *p != NUL)
#यदि defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != रिक्त && *p != NUL)
	    || ((p = (char_u *)default_shell()) != रिक्त && *p != NUL)
#endif
	    )
#यदि defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	माप_प्रकार	    len;

	यदि (vim_strchr(p, ' ') != रिक्त)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    यदि (cmd != रिक्त)
	    {
		vim_snprintf((वर्ण *)cmd, len, "\"%s\"", p);
		set_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	अन्यथा
	    set_string_default("sh", p);
    }
#अन्यथा
	set_string_default_esc("sh", p, TRUE);
#endif
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    स्थिर शून्य
set_init_default_backupskip(शून्य)
{
    पूर्णाङ्क		opt_idx;
    पूर्णाङ्क		i;
    char_u	*p;
    पूर्णाङ्क		plen;
#ifdef UNIX
    स्थिर वर्ण	*(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#अन्यथा
    स्थिर वर्ण	*(names[3]) = {"TMPDIR", "TEMP", "TMP"};
#endif
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    लागि (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(names); ++i)
    {
	पूर्णाङ्क		mustfree = FALSE;
#ifdef UNIX
	यदि (*names[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (पूर्णाङ्क)STRLEN_LITERAL("/private/tmp");
# अन्यथा
	    p = (char_u *)"/tmp";
	    plen = (पूर्णाङ्क)STRLEN_LITERAL("/tmp");
# endif
	}
	अन्यथा
#endif
	{
	    p = vim_getenv((char_u *)names[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	यदि (p != रिक्त && *p != NUL)
	{
	    char_u  *item;
	    माप_प्रकार  itemsize;
	    पूर्णाङ्क	    has_trailing_path_sep = FALSE;

	    यदि (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (पूर्णाङ्क)STRLEN(p);
		// does the value include a trailing path separator?
		यदि (after_pathsep(p, p + plen))
		    has_trailing_path_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_path_sep ? 0 : 1) + 2;
	    item = alloc(itemsize);
	    यदि (item != रिक्त)
	    {
		// add a preceding comma as a separator after the first item
		माप_प्रकार	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		माप_प्रकार	itemlen;

		itemlen = vim_snprintf((वर्ण *)item, itemsize, "%s%s*", p, (has_trailing_path_sep) ? "" : PATHSEPSTR);

		यदि (find_dup_item(ga.ga_data, item, itemlen, options[opt_idx].flags) == रिक्त
			&& ga_grow(&ga, (पूर्णाङ्क)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((वर्ण *)ga.ga_data + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", item);
		}
		vim_free(item);
	    }
	}
	यदि (mustfree)
	    vim_free(p);
    }
    यदि (ga.ga_data != रिक्त)
    {
	set_string_default("bsk", ga.ga_data);
	vim_free(ga.ga_data);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    स्थिर शून्य
set_init_default_maxmemtot(शून्य)
{
    पूर्णाङ्क		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    यदि (opt_idx < 0)
	फर्काऊ;

#यदि !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
    यदि (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#endif
    {
#यदि defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#अथवायदि defined(HAVE_TOTAL_MEM)
	// Use amount of memory available to Vim.
	n = (mch_total_mem(FALSE) >> 1);
#अन्यथा
	n = (0x7fffffff >> 11);
#endif
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	यदि (opt_idx >= 0)
	{
#यदि !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
	    यदि ((दीर्घ)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (दीर्घ)n
		    || (दीर्घ)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#endif
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    स्थिर शून्य
set_init_default_cdpath(शून्य)
{
    पूर्णाङ्क		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    पूर्णाङ्क	i;
    पूर्णाङ्क	j;
    पूर्णाङ्क	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    यदि (cdpath == रिक्त)
	फर्काऊ;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    यदि (buf != रिक्त)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	लागि (i = 0; cdpath[i] != NUL; ++i)
	{
	    यदि (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    अन्यथा
	    {
		यदि (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	यदि (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	अन्यथा
	    vim_free(buf); // cannot happen
    }
    यदि (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    स्थिर शून्य
set_init_default_printencoding(शून्य)
{
#यदि defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    set_string_default("penc",
# यदि defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# अन्यथा // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#endif
}

#यदि defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    स्थिर शून्य
set_init_default_printexpr(शून्य)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    set_string_default("pexpr",
# यदि defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# अन्यथा
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#endif

#ifdef UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    स्थिर शून्य
set_init_restricted_mode(शून्य)
{
    char_u	*p;

    p = get_isolated_shell_name();
    यदि (p == रिक्त)
	फर्काऊ;
    यदि (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#endif

#ifdef CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    स्थिर शून्य
set_init_clean_rtp(शून्य)
{
    पूर्णाङ्क		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    यदि (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    यदि (opt_idx < 0)
	फर्काऊ;
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
    स्थिर शून्य
set_init_xdg_rtp(शून्य)
{
    पूर्णाङ्क		opt_idx;
    पूर्णाङ्क		has_xdg_env = TRUE;
    पूर्णाङ्क		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = रिक्त;
    char_u	*vimrc2 = रिक्त;
    char_u	*xdg_dir = रिक्त;
    char_u	*xdg_rtp = रिक्त;
    string_T	vimrc_xdg = {रिक्त, 0};

    // initialize chartab, so we can expand $HOME
    (शून्य)init_chartab();
    vimrc1 = expand_env_save((char_u *)USR_VIMRC_FILE);
    vimrc2 = expand_env_save((char_u *)USR_VIMRC_FILE2);

    xdg_dir = mch_getenv("XDG_CONFIG_HOME");
    यदि (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    यदि (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	गच्छ theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    यदि ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	गच्छ theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    यदि ((opt_idx = findoption((char_u *)"packpath")) < 0)
	गच्छ theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# यदि defined(XDG_VDIR) && defined(FEAT_SESSION)
    यदि ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	गच्छ theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    यदि (should_free_xdg_dir)
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
    स्थिर शून्य
set_init_expand_env(शून्य)
{
    पूर्णाङ्क		opt_idx;
    char_u	*p;

    लागि (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	यदि ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != रिक्त)
	    p = (char_u *)_(*(वर्ण **)options[opt_idx].var);
	अन्यथा
	    p = option_expand(opt_idx, रिक्त);
	यदि (p != रिक्त && (p = vim_strsave(p)) != रिक्त)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    यदि (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#यदि defined(MSWIN) && defined(FEAT_GETTEXT)
/*
 * Get the display language of Windows and the languages to fall back on, as a
 * colon separated list for gettext, e.g. "ja_JP:en_US".  The list stops after
 * English, untranslated messages are English already.
 * Returns NULL when it cannot be obtained.  The result must be freed.
 */
    स्थिर char_u *
get_ui_langs(शून्य)
{
    ULONG	अङ्क_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*buffer;
    char_u	*langs = रिक्त;

    यदि (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &अङ्क_languages, रिक्त,
								    &bufsize)
	    || bufsize == 0)
	फर्काऊ रिक्त;

    buffer = ALLOC_MULT(WCHAR, bufsize);
    यदि (buffer == रिक्त)
	फर्काऊ रिक्त;

    यदि (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &अङ्क_languages, buffer,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	यदि (langs != रिक्त)
	{
	    char_u	*d = langs;
	    WCHAR	*s = buffer;

	    जबसम्म (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		यदि (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		लागि ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		यदि (english)
		    रोक;
	    }
	    *d = NUL;
	}
    }
    vim_free(buffer);

    फर्काऊ langs;
}
#endif

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    स्थिर शून्य
set_init_lang_env(शून्य)
{
#यदि defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    यदि (mch_getenv((char_u *)"LANG") == रिक्त
	    && mch_getenv((char_u *)"LANGUAGE") == रिक्त
	    && mch_getenv((char_u *)"LC_ALL") == रिक्त
	    && mch_getenv((char_u *)"LC_MESSAGES") == रिक्त)
    {
	char_u	*langs = get_ui_langs();

	यदि (langs != रिक्त && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    यदि (colon != रिक्त)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#अथवायदि defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#endif
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    स्थिर शून्य
set_init_default_encoding(शून्य)
{
    char_u	*p;
    पूर्णाङ्क		opt_idx;

#यदि defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#अन्यथा
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#endif
    यदि (p == रिक्त)
	फर्काऊ;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    यदि (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    यदि (mb_init() == रिक्त)
    {
	opt_idx = findoption((char_u *)"encoding");
	यदि (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#यदि defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	यदि (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    set_string_option_direct((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    set_string_option_direct((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    यदि (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    यदि (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (शून्य)init_chartab();
	}
#endif

#यदि defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	यदि (
# ifdef VIMDLL
		(!gui.in_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    वर्ण	buf[50];
	    माप_प्रकार	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    यदि (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, परिमाण(buf), "cp%ld", (दीर्घ)GetACP());
	    अन्यथा
		buflen = vim_snprintf(buf, परिमाण(buf), "cp%ld", (दीर्घ)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    यदि (p_tenc != रिक्त)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		यदि (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_setup(&आगत_conv, p_tenc, p_enc);
		convert_setup(&output_conv, p_enc, p_tenc);
	    }
	    अन्यथा
		p_tenc = empty_option;
	}
#endif
#यदि defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#endif
    }
    अन्यथा
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
    शून्य
set_init_1(पूर्णाङ्क clean_arg)
{
#ifdef FEAT_LANGMAP
    langmap_init();
#endif

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    यदि (mch_getenv((char_u *)"VIM_POSIX") != रिक्त)
    {
	set_string_default("cpo", (char_u *)CPO_ALL);
	set_string_default("shm", (char_u *)SHM_POSIX);
    }

    set_init_default_shell();
    set_init_default_backupskip();
    set_init_default_maxmemtot();
    set_init_default_cdpath();
    set_init_default_printencoding();
#यदि defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
    यदि (clean_arg)
	set_init_clean_rtp();
#endif

#ifdef FEAT_GUI
    यदि (found_reverse_arg)
	set_option_मान_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
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

#यदि defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    यदि (mch_getenv((char_u *)"MLTERM") != रिक्त)
	set_option_मान_give_err((char_u *)"tbidi", 1L, रिक्त, 0);
#endif

    didset_options2();

    set_init_lang_env();

#ifdef FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    set_helplang_default(get_mess_lang());
#endif
}

स्थिर char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    शून्य
set_fencs_unicode(शून्य)
{
    set_string_option_direct((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    स्थिर शून्य
set_option_default(
    पूर्णाङ्क		opt_idx,
    पूर्णाङ्क		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    पूर्णाङ्क		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    पूर्णाङ्क		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    पूर्णाङ्क		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    यदि (varp != रिक्त)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	यदि (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    यदि (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		set_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    अन्यथा यदि (options[opt_idx].indir != PV_NONE)
		set_string_option_direct(रिक्त, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    अन्यथा
	    {
		यदि ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	अन्यथा यदि (flags & P_अङ्क)
	{
	    यदि (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    अन्यथा
	    {
		दीर्घ def_val = (दीर्घ)(long_i)options[opt_idx].def_val[dvi];

		यदि ((दीर्घ *)varp == &curwin->w_p_so
			|| (दीर्घ *)varp == &curwin->w_p_siso
			|| (दीर्घ *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(दीर्घ *)varp = -1;
		अन्यथा
		    *(दीर्घ *)varp = def_val;
		// May also set global value for local option.
		यदि (both)
		    *(दीर्घ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	अन्यथा	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(पूर्णाङ्क *)varp = (पूर्णाङ्क)(दीर्घ)(long_i)options[opt_idx].def_val[dvi];
#ifdef UNIX
	    // 'modeline' defaults to off for root
	    यदि (options[opt_idx].indir == PV_ML && getuid() == जरा_UID)
		*(पूर्णाङ्क *)varp = FALSE;
#endif
	    // May also set global value for local option.
	    यदि (both)
		*(पूर्णाङ्क *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(पूर्णाङ्क *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	यदि (both)
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
    स्थिर शून्य
set_options_default(
    पूर्णाङ्क		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    पूर्णाङ्क		i;
    win_T	*wp;
    tabpage_T	*tp;

    लागि (i = 0; !istermoption_idx(i); i++)
	यदि (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#यदि defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_साँचो
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
    स्थिर शून्य
set_string_default_esc(वर्ण *name, char_u *val, पूर्णाङ्क escape)
{
    char_u	*p;
    पूर्णाङ्क		opt_idx;

    यदि (escape && vim_strchr(val, ' ') != रिक्त)
	p = vim_strsave_escaped(val, (char_u *)" ");
    अन्यथा
	p = vim_strsave(val);
    यदि (p == रिक्त)		// we don't want a NULL
	फर्काऊ;

    opt_idx = findoption((char_u *)name);
    यदि (opt_idx < 0)
    {
	vim_free(p);
	फर्काऊ;
    }

    यदि (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    शून्य
set_string_default(वर्ण *name, char_u *val)
{
    set_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    स्थिर char_u *
find_dup_item(char_u *origval, char_u *newval, माप_प्रकार newvallen, long_u flags)
{
    पूर्णाङ्क	    bs = 0;
    char_u  *s;

    यदि (origval == रिक्त)
	फर्काऊ रिक्त;

    लागि (s = origval; *s != NUL; ++s)
    {
	यदि ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    फर्काऊ s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	यदि ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	अन्यथा
	    bs = 0;
    }
    फर्काऊ रिक्त;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    शून्य
set_सङ्ख्या_default(वर्ण *name, दीर्घ val)
{
    पूर्णाङ्क		opt_idx;

    opt_idx = findoption((char_u *)name);
    यदि (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#यदि defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    शून्य
set_local_options_default(win_T *wp, पूर्णाङ्क do_buffer)
{
    win_T	*save_curwin = curwin;
    पूर्णाङ्क		i;

    curwin = wp;
    curbuf = curwin->w_buffer;
    block_autocmds();

    लागि (i = 0; !istermoption_idx(i); i++)
    {
	संरचना vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	यदि (p->indir != PV_NONE
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

#यदि defined(EXITFREE)
/*
 * Free all options.
 */
    शून्य
free_all_options(शून्य)
{
    पूर्णाङ्क		i;

    लागि (i = 0; !istermoption_idx(i); i++)
    {
	यदि (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    यदि ((options[i].flags & P_ALLOCED) && options[i].var != रिक्त)
		free_string_option(*(char_u **)options[i].var);
	    यदि (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	अन्यथा यदि (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# यदि defined(FEAT_EVAL)
    free_findfunc_option();
# endif
}
#endif


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    शून्य
set_init_2(शून्य)
{
    पूर्णाङ्क		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    यदि (idx >= 0 && !(options[idx].flags & P_WAS_SET))
	set_option_default(idx, OPT_LOCAL, p_cp);
    comp_col();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    यदि (!option_was_set((char_u *)"window"))
	p_window = Rows - 1;
    set_सङ्ख्या_default("window", Rows - 1);

    // For DOS console the default is always black.
#यदि !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    यदि (idx >= 0 && !(options[idx].flags & P_WAS_SET)
						 && *term_bg_default() == 'd')
    {
	set_string_option_direct(रिक्त, idx, (char_u *)"dark", OPT_FREE, 0);
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
    (शून्य)parse_printoptions(रिक्त);	// parse 'printoptions' default value
#endif
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    शून्य
set_init_3(शून्य)
{
#यदि defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    पूर्णाङ्क	    idx_srr;
    पूर्णाङ्क	    do_srr;
# ifdef FEAT_QUICKFIX
    पूर्णाङ्क	    idx_sp;
    पूर्णाङ्क	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    यदि (idx_srr < 0)
	do_srr = FALSE;
    अन्यथा
	do_srr = !(options[idx_srr].flags & P_WAS_SET);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    यदि (idx_sp < 0)
	do_sp = FALSE;
    अन्यथा
	do_sp = !(options[idx_sp].flags & P_WAS_SET);
# endif
    p = get_isolated_shell_name();
    यदि (p != रिक्त)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	यदि (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# यदि defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# यदि defined(FEAT_QUICKFIX)
	    यदि (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  अन्यथा
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    यदि (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	अन्यथा यदि (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  यदि defined(FEAT_QUICKFIX)
		यदि (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		यदि (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	अन्यथा
	    // Always use POSIX shell style redirection if we reach this
	    यदि (       fnamecmp(p, "sh") == 0
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
# यदि defined(FEAT_QUICKFIX)
		यदि (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  अन्यथा
		    यदि (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    अन्यथा
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		यदि (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#endif

#यदि defined(MSWIN)
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
    यदि (म_माला((वर्ण *)gettail(p_sh), "powershell") != रिक्त)
    {
	पूर्णाङ्क	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	यदि (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	यदि (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    अन्यथा यदि (म_माला((वर्ण *)gettail(p_sh), "sh") != रिक्त)
    {
	पूर्णाङ्क	idx3;

	idx3 = findoption((char_u *)"shcf");
	यदि (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	यदि (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    अन्यथा यदि (म_माला((वर्ण *)gettail(p_sh), "cmd.exe") != रिक्त)
    {
	पूर्णाङ्क	idx3;

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
	यदि (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	यदि (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#endif

    यदि (BUFEMPTY())
    {
	पूर्णाङ्क idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	यदि (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_SET))
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    set_title_defaults();
}

#यदि defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    शून्य
set_helplang_default(char_u *lang)
{
    पूर्णाङ्क		idx;
    माप_प्रकार	langlen;

    यदि (lang == रिक्त)	// safety check
	फर्काऊ;

    langlen = STRLEN(lang);
    यदि (langlen < 2)	// safety check
	फर्काऊ;

    idx = findoption((char_u *)"hlg");
    यदि (idx < 0 || (options[idx].flags & P_WAS_SET))
	फर्काऊ;

    यदि (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    यदि (p_hlg == रिक्त)
	p_hlg = empty_option;
    अन्यथा
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	यदि (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	अन्यथा यदि (langlen >= 1 && *p_hlg == 'C')
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
    शून्य
set_title_defaults(शून्य)
{
    पूर्णाङ्क	    idx1;
    दीर्घ    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    यदि (idx1 >= 0 && !(options[idx1].flags & P_WAS_SET))
    {
#ifdef FEAT_GUI
	यदि (gui.starting || gui.in_use)
	    val = TRUE;
	अन्यथा
#endif
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    यदि (idx1 < 0 || (options[idx1].flags & P_WAS_SET))
    {
	फर्काऊ;
    }

#ifdef FEAT_GUI
    यदि (gui.starting || gui.in_use)
	val = TRUE;
    अन्यथा
#endif
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    शून्य
ex_set(exarg_T *eap)
{
    पूर्णाङ्क		flags = 0;

    यदि (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    अन्यथा यदि (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#यदि defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    यदि ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    अन्यथा
#endif
    {
	यदि (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(शून्य)do_set(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
प्रकारनिर्देश गणना {
    PREFIX_NO = 0,	// "no" prefix
    PREFIX_NONE,	// no prefix
    PREFIX_INV,		// "inv" prefix
} set_prefix_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    स्थिर set_prefix_T
get_option_prefix(char_u **argp)
{
    पूर्णाङ्क		prefix = PREFIX_NONE;
    char_u	*arg = *argp;

    यदि (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	prefix = PREFIX_NO;
	arg += 2;
    }
    अन्यथा यदि (STRNCMP(arg, "inv", 3) == 0)
    {
	prefix = PREFIX_INV;
	arg += 3;
    }

    *argp = arg;
    फर्काऊ prefix;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    स्थिर पूर्णाङ्क
parse_option_name(char_u *arg, पूर्णाङ्क *opt_idxp, पूर्णाङ्क *lenp, पूर्णाङ्क *keyp)
{
    पूर्णाङ्क	साँचो = 0;
    पूर्णाङ्क	len;
    पूर्णाङ्क	opt_idx;

    यदि (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	यदि (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	अन्यथा
	{
	    len = 1;
	    जबसम्म (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	यदि (arg[len] != '>')
	    फर्काऊ FAIL;

	arg[len] = NUL;			    // put NUL after name
	यदि (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	यदि (opt_idx == -1)
	    साँचो = find_साँचो_option(arg + 1, TRUE);
    }
    अन्यथा
    {
	पूर्णाङ्क	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	यदि (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	अन्यथा
	    जबसम्म (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	यदि (opt_idx == -1)
	    साँचो = find_साँचो_option(arg, FALSE);
    }

    *keyp = साँचो;
    *lenp = len;
    *opt_idxp = opt_idx;

    फर्काऊ OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    स्थिर set_op_T
get_opt_op(char_u *arg)
{
    set_op_T op = OP_NONE;

    यदि (*arg != NUL && *(arg + 1) == '=')
    {
	यदि (*arg == '+')
	    op = OP_ADDING;		// "+="
	अन्यथा यदि (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	अन्यथा यदि (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    फर्काऊ op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
स्थिर वर्ण *modeline_whitelist[] =
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
    रिक्त
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    स्थिर bool
is_modeline_whitelisted(वर्ण *name)
{
    लागि (पूर्णाङ्क i = 0; modeline_whitelist[i] != रिक्त; i++)
	यदि (STRCMP(name, modeline_whitelist[i]) == 0)
	    फर्काऊ true;
    फर्काऊ false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    स्थिर पूर्णाङ्क
validate_opt_idx(
	पूर्णाङ्क opt_idx,
	पूर्णाङ्क opt_flags,
	long_u flags,
	वर्ण **errmsg,
	set_prefix_T prefix)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    यदि ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	फर्काऊ FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    यदि ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	फर्काऊ FAIL;

    // Disallow changing some options from modelines.
    यदि (opt_flags & OPT_MODELINE)
    {
	यदि (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_in_modeline;
	    फर्काऊ FAIL;
	}
	यदि ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_in_modeline_when_modelineexpr_is_off;
	    फर्काऊ FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	यदि (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    यदि (options[opt_idx].indir == PV_ML && prefix == PREFIX_NO)
		फर्काऊ OK;
	    अन्यथा यदि (!is_modeline_whitelisted(options[opt_idx].fullname))
		फर्काऊ FAIL;
	}
#ifdef FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	यदि (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    फर्काऊ FAIL;
#endif
    }

#ifdef HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    यदि (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_in_sandbox;
	फर्काऊ FAIL;
    }
#endif

    फर्काऊ OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    स्थिर char_u *
stropt_get_default_val(
    पूर्णाङ्क		opt_idx,
    char_u	*varp,
    पूर्णाङ्क		flags,
    पूर्णाङ्क		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    यदि ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ifdef FEAT_GUI
	यदि (gui.in_use)
	    newval = gui_bg_default();
	अन्यथा
#endif
	    newval = term_bg_default();
    }
    अन्यथा यदि ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    यदि (newval == रिक्त)
	newval = empty_option;
    अन्यथा
    {
	char_u *s = option_expand(opt_idx, newval);
	यदि (s == रिक्त)
	    s = newval;
	newval = vim_strsave(s);
    }

    फर्काऊ newval;
}

/*
 * Convert the 'backspace' option number value to a string: for adding,
 * prepending and removing string.
 */
    स्थिर शून्य
opt_backspace_nr2str(
    char_u	*varp,
    char_u	**origval_p,
    char_u	**origval_l_p,
    char_u	**origval_g_p,
    char_u	**oldval_p)
{
    पूर्णाङ्क		i = getdigits((char_u **)varp);

    विकल्प (i)
    {
	पक्ष 0:
	    *(char_u **)varp = empty_option;
	    रोक;
	पक्ष 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    रोक;
	पक्ष 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    रोक;
	पक्ष 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    रोक;
    }
    vim_free(*oldval_p);
    यदि (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    यदि (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    यदि (*origval_g_p == *oldval_p)
	*origval_g_p = *(char_u **)varp;
    *oldval_p = *(char_u **)varp;
}

/*
 * Convert the 'whichwrap' option number value to a string, for backwards
 * compatibility with Vim 3.0.
 * Note: 'argp' is a pointer to a char_u pointer and is updated.
 */
    स्थिर char_u *
opt_whichwrap_nr2str(char_u **argp, char_u *whichwrap)
{
    माप_प्रकार  len = 0;

    *whichwrap = NUL;
    पूर्णाङ्क i = getdigits(argp);
    यदि (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    यदि (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    यदि (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    यदि (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    यदि (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    यदि (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    फर्काऊ whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    स्थिर char_u *
stropt_copy_मान(
    char_u	*origval,
    char_u	**argp,
    set_op_T	op,
    पूर्णाङ्क		flags UNUSED)
{
    char_u	*arg = *argp;
    अचिन्हित	newlen;
    char_u	*newval;
    char_u	*s = रिक्त;

    // get a bit too much
    newlen = (अचिन्हित)STRLEN(arg) + 1;
    यदि (op != OP_NONE)
	newlen += (अचिन्हित)STRLEN(origval) + 1;
    newval = alloc(newlen);
    यदि (newval == रिक्त)  // out of mem, don't change
	फर्काऊ रिक्त;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    जबसम्म (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	पूर्णाङ्क i;

	यदि (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#endif
	   )
	    ++arg;	// remove backslash
	यदि (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (माप_प्रकार)i);
	    arg += i;
	    s += i;
	}
	अन्यथा
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    फर्काऊ newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    स्थिर char_u *
stropt_expand_envvar(
    पूर्णाङ्क		opt_idx,
    char_u	*origval,
    char_u	*newval,
    set_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    यदि (s == रिक्त)
	फर्काऊ newval;

    vim_free(newval);
    अचिन्हित newlen = (अचिन्हित)STRLEN(s) + 1;
    यदि (op != OP_NONE)
	newlen += (अचिन्हित)STRLEN(origval) + 1;

    newval = alloc(newlen);
    यदि (newval == रिक्त)
	फर्काऊ रिक्त;

    STRCPY(newval, s);

    फर्काऊ newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    स्थिर शून्य
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    पूर्णाङ्क		flags)
{
    पूर्णाङ्क		len = 0;

    पूर्णाङ्क comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    यदि (op == OP_ADDING)
    {
	len = (पूर्णाङ्क)STRLEN(origval);
	// strip a trailing comma, would get 2
	यदि (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (माप_प्रकार)len);
    }
    अन्यथा
    {
	len = (पूर्णाङ्क)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    यदि (comma)
	newval[len] = ',';
}

/*
 * Remove a value from a string option.  Copy string option value in "origval"
 * to "newval" and then remove the string "strval" of length "len".
 */
    स्थिर शून्य
stropt_remove_val(
    char_u	*origval,
    char_u	*newval,
    पूर्णाङ्क		flags,
    char_u	*strval,
    पूर्णाङ्क		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    यदि (*strval)
    {
	// may need to remove a comma
	यदि (flags & P_COMMA)
	{
	    यदि (strval == origval)
	    {
		// include comma after string
		यदि (strval[len] == ',')
		    ++len;
	    }
	    अन्यथा
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
    स्थिर char_u *
find_साँचो_item(char_u *src, char_u *साँचो, पूर्णाङ्क keylen, पूर्णाङ्क *itemlenp)
{
    char_u  *p = src;

    जबसम्म (*p != NUL)
    {
	// Check if this item starts with the same key
	यदि ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, साँचो, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *end = vim_strchr(p, ',');
	    यदि (end == रिक्त)
		end = p + STRLEN(p);
	    *itemlenp = (पूर्णाङ्क)(end - p);
	    फर्काऊ p;
	}
	++p;
    }
    फर्काऊ रिक्त;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    स्थिर शून्य
remove_comma_item(char_u *str, char_u *item, पूर्णाङ्क itemlen)
{
    यदि (item[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(item, item + itemlen + 1);
    अन्यथा यदि (item > str && *(item - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(item - 1, item + itemlen);
    अन्यथा
	// Only item
	*item = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    स्थिर शून्य
remove_साँचो_item(char_u *str, char_u *साँचो, पूर्णाङ्क keylen, char_u *skip)
{
    पूर्णाङ्क	    itemlen;
    char_u  *found;

    जबसम्म ((found = find_साँचो_item(str, साँचो, keylen, &itemlen)) != रिक्त)
    {
	यदि (found == skip)
	{
	    // Search for the next match after this one.
	    char_u *next = found + itemlen;
	    यदि (*next == ',')
		++next;
	    found = find_साँचो_item(next, साँचो, keylen, &itemlen);
	    यदि (found == रिक्त)
		रोक;
	}

	remove_comma_item(str, found, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    स्थिर शून्य
append_item(char_u *str, char_u *item, पूर्णाङ्क item_len)
{
    पूर्णाङ्क len = (पूर्णाङ्क)STRLEN(str);

    यदि (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, item, (माप_प्रकार)item_len);
    str[len + item_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    स्थिर शून्य
prepend_item(char_u *str, char_u *item, पूर्णाङ्क item_len)
{
    पूर्णाङ्क len = (पूर्णाङ्क)STRLEN(str);
    पूर्णाङ्क comma = (len > 0) ? 1 : 0;

    mch_memmove(str + item_len + comma, str, (माप_प्रकार)len + 1);
    mch_memmove(str, item, (माप_प्रकार)item_len);
    यदि (comma)
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
    स्थिर bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    पूर्णाङ्क		flags UNUSED)
{
    char_u  *p;
    char_u  *item_start;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    यदि (vim_strchr(newval, ':') == रिक्त && vim_strchr(newval, ',') == रिक्त)
	फर्काऊ false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    यदि (newval_copy == रिक्त)
	फर्काऊ false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    item_start = newval_copy;
    लागि (;;)
    {
	p = vim_strchr(item_start, ',');
	पूर्णाङ्क item_len = (p == रिक्त)
			    ? (पूर्णाङ्क)STRLEN(item_start) : (पूर्णाङ्क)(p - item_start);

	यदि (item_len > 0)
	{
	    char_u *colon = vim_strchr(item_start, ':');
	    यदि (colon != रिक्त && colon < item_start + item_len)
	    {
		पूर्णाङ्क keylen = (पूर्णाङ्क)(colon - item_start) + 1;

		यदि (op == OP_ADDING || op == OP_PREPENDING)
		{
		    पूर्णाङ्क old_itemlen;
		    char_u *found = find_साँचो_item(newval, item_start,
						       keylen, &old_itemlen);
		    यदि (found != रिक्त)
		    {
			यदि (old_itemlen == item_len
				&& STRNCMP(found, item_start,
							 item_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_साँचो_item(newval, item_start,
							   keylen, found);
			}
			अन्यथा
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_साँचो_item(newval, item_start,
							    keylen, रिक्त);
			    यदि (op == OP_PREPENDING)
				prepend_item(newval, item_start, item_len);
			    अन्यथा
				append_item(newval, item_start, item_len);
			}
		    }
		    अन्यथा
		    {
			// New item.
			यदि (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			अन्यथा
			    append_item(newval, item_start, item_len);
		    }
		}
		अन्यथा यदि (op == OP_REMOVING)
		    remove_साँचो_item(newval, item_start, keylen, रिक्त);
	    }
	    अन्यथा
	    {
		यदि (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    यदि (found == रिक्त)
		    {
			// New item.
			यदि (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			अन्यथा
			    append_item(newval, item_start, item_len);
		    }
		    // else: exact duplicate — do nothing
		}
		अन्यथा यदि (op == OP_REMOVING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    यदि (found != रिक्त)
			remove_comma_item(newval, found, item_len);
		}
	    }
	}

	यदि (p == रिक्त)
	    रोक;
	item_start = p + 1;
    }

    vim_free(newval_copy);

    फर्काऊ true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    स्थिर शून्य
stropt_remove_dupflags(char_u *newval, पूर्णाङ्क flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    जबसम्म (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	यदि (flags & P_ONECOMMA)
	{
	    यदि (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != रिक्त)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		जारी;
	    }
	}
	अन्यथा
	{
	    यदि ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != रिक्त)
	    {
		STRMOVE(s, s + 1);
		जारी;
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
    स्थिर char_u *
stropt_get_newval(
    पूर्णाङ्क		nextchar,
    पूर्णाङ्क		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    set_op_T    *op_arg,
    पूर्णाङ्क		flags,
    पूर्णाङ्क		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    set_op_T    op = *op_arg;
    char_u	*save_arg = रिक्त;
    char_u	*newval;
    char_u	*s = रिक्त;
    char_u	whichwrap[80];

    यदि (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    अन्यथा यदि (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    अन्यथा
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	यदि (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	अन्यथा यदि (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	अन्यथा यदि (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	अन्यथा यदि (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_मान(origval, &arg, op, flags);
	यदि (newval == रिक्त)
	    गच्छ done;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	यदि (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    यदि (newval == रिक्त)
		गच्छ done;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	यदि ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	अन्यथा
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    पूर्णाङ्क len = 0;
	    यदि (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (पूर्णाङ्क)STRLEN(newval);
		s = find_dup_item(origval, newval, len, flags);

		// do not add if already there
		यदि ((op == OP_ADDING || op == OP_PREPENDING) && s != रिक्त)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		यदि (s == रिक्त)
		    s = origval + (पूर्णाङ्क)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    यदि (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    अन्यथा यदि (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	यदि (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

done:
    यदि (save_arg != रिक्त)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    फर्काऊ newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    स्थिर पूर्णाङ्क
do_set_option_string(
	पूर्णाङ्क	    opt_idx,
	पूर्णाङ्क	    opt_flags,
	char_u	    **argp,
	पूर्णाङ्क	    nextchar,
	set_op_T    op_arg,
	long_u	    flags,
	पूर्णाङ्क	    cp_val,
	char_u	    *varp_arg,
	वर्ण	    *errbuf,
	माप_प्रकार	    errbuflen,
	पूर्णाङ्क	    *मान_checked,
	वर्ण	    **errmsg)
{
    char_u	*arg = *argp;
    set_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = रिक्त; // previous value if *varp
    char_u	*newval;
    char_u	*origval = रिक्त;
    char_u	*origval_l = रिक्त;
    char_u	*origval_g = रिक्त;
#यदि defined(FEAT_EVAL)
    char_u	*saved_origval = रिक्त;
    char_u	*saved_origval_l = रिक्त;
    char_u	*saved_origval_g = रिक्त;
    char_u	*saved_newval = रिक्त;
#endif

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    यदि ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((पूर्णाङ्क)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    यदि ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	यदि (((पूर्णाङ्क)options[opt_idx].indir & PV_BOTH)
						  && origval_l == empty_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    यदि (((पूर्णाङ्क)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    अन्यथा
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    यदि (newval == रिक्त)
	*(char_u **)(varp) = empty_option;

#यदि defined(FEAT_EVAL)
    यदि (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_साँचो
# endif
		      && origval != रिक्त && newval != रिक्त)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	यदि (origval_l != रिक्त)
	    saved_origval_l = vim_strsave(origval_l);
	यदि (origval_g != रिक्त)
	    saved_origval_g = vim_strsave(origval_g);
    }
#endif

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	पूर्णाङ्क	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	यदि ((opt_flags & OPT_MODELINE)
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
			errbuflen, opt_flags, op, मान_checked);

	secure = secure_saved;
    }

#यदि defined(FEAT_EVAL)
    यदि (*errmsg == रिक्त)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#endif

    *argp = arg;
    फर्काऊ *errmsg == रिक्त ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    स्थिर वर्ण *
do_set_option_bool(
    पूर्णाङ्क		opt_idx,
    पूर्णाङ्क		opt_flags,
    set_prefix_T prefix,
    long_u	flags,
    char_u	*varp,
    पूर्णाङ्क		nextchar,
    पूर्णाङ्क		afterchar,
    पूर्णाङ्क		cp_val)

{
    varnumber_T	मान;

    यदि (nextchar == '=' || nextchar == ':')
	फर्काऊ e_invalid_argument;
    यदि (opt_idx < 0 || varp == रिक्त)
	फर्काऊ रिक्त;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    यदि (nextchar == '!')
	मान = *(पूर्णाङ्क *)(varp) ^ 1;
    अन्यथा यदि (nextchar == '&')
	मान = (पूर्णाङ्क)(दीर्घ)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    अन्यथा यदि (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	यदि ((पूर्णाङ्क *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    मान = -1;
	अन्यथा यदि ((पूर्णाङ्क *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    मान = -1;
#ifdef HAVE_FSYNC
	अन्यथा यदि ((पूर्णाङ्क *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    मान = -1;
#endif
	अन्यथा
	    मान = *(पूर्णाङ्क *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    अन्यथा
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	यदि (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    फर्काऊ e_trailing_characters;
	यदि (prefix == PREFIX_INV)
	    मान = *(पूर्णाङ्क *)(varp) ^ 1;
	अन्यथा
	    मान = prefix == PREFIX_NO ? 0 : 1;
    }

    फर्काऊ set_bool_option(opt_idx, varp, (पूर्णाङ्क)मान, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    स्थिर वर्ण *
do_set_option_numeric(
    पूर्णाङ्क		opt_idx,
    पूर्णाङ्क		opt_flags,
    char_u	**argp,
    पूर्णाङ्क		nextchar,
    set_op_T	op,
    long_u	flags,
    पूर्णाङ्क		cp_val,
    char_u	*varp,
    वर्ण	*errbuf,
    माप_प्रकार	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		मान;
    पूर्णाङ्क			i;
    वर्ण		*errmsg = रिक्त;

    यदि (opt_idx < 0 || varp == रिक्त)
	फर्काऊ रिक्त;  // "cannot happen"
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
    यदि (nextchar == '&')
	मान = (दीर्घ)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    अन्यथा यदि (nextchar == '<')
    {
	यदि ((दीर्घ *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    मान = NO_LOCAL_UNDOLEVEL;
	अन्यथा यदि (opt_flags == OPT_LOCAL
		    && ((दीर्घ *)varp == &curwin->w_p_siso
		     || (दीर्घ *)varp == &curwin->w_p_so
		     || (दीर्घ *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    मान = -1;
	अन्यथा
	    मान = *(दीर्घ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    अन्यथा यदि (((दीर्घ *)varp == &p_wc || (दीर्घ *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	मान = string_to_साँचो(arg, FALSE);
	यदि (मान == 0)
	{
	    errmsg = e_invalid_argument;
	    गच्छ skip;
	}
    }
    अन्यथा यदि (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, रिक्त, &i, STR2NR_ALL, &मान, रिक्त, 0, TRUE, रिक्त);
	यदि (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_सङ्ख्या_required_after_equal;
	    गच्छ skip;
	}
    }
    अन्यथा
    {
	errmsg = e_सङ्ख्या_required_after_equal;
	गच्छ skip;
    }

    यदि (op == OP_ADDING)
	मान = *(दीर्घ *)varp + मान;
    अन्यथा यदि (op == OP_PREPENDING)
	मान = *(दीर्घ *)varp * मान;
    अन्यथा यदि (op == OP_REMOVING)
	मान = *(दीर्घ *)varp - मान;

    यदि ((दीर्घ *)varp == &curwin->w_p_sop && मान < -1)
    {
	errmsg = e_invalid_argument;
	गच्छ skip;
    }

    errmsg = set_अङ्क_option(opt_idx, varp, मान, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    फर्काऊ errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    स्थिर वर्ण *
do_set_option_keycode(char_u **argp, char_u *साँचो_name, पूर्णाङ्क nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    यदि (nextchar == '&')
    {
	यदि (add_termcap_entry(साँचो_name, TRUE) == FAIL)
	    फर्काऊ e_not_found_in_termcap;
    }
    अन्यथा
    {
	++arg; // jump to after the '=' or ':'
	लागि (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    यदि (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	add_termcode(साँचो_name, arg, FALSE);
	*p = nextchar;
    }
    यदि (full_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    फर्काऊ रिक्त;
}

/*
 * Set an option to a new value.
 */
    स्थिर वर्ण *
do_set_option_मान(
    पूर्णाङ्क		opt_idx,
    पूर्णाङ्क		opt_flags,
    char_u	**argp,
    set_prefix_T prefix,
    set_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*साँचो_name,
    पूर्णाङ्क		nextchar,
    पूर्णाङ्क		afterchar,
    पूर्णाङ्क		cp_val,
    पूर्णाङ्क		*stopopteval,
    वर्ण	*errbuf,
    माप_प्रकार	errbuflen)
{
    पूर्णाङ्क		मान_checked = FALSE;
    वर्ण	*errmsg = रिक्त;
    char_u	*arg = *argp;

    यदि (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_set_option_bool(opt_idx, opt_flags, prefix, flags, varp,
						nextchar, afterchar, cp_val);
	यदि (errmsg != रिक्त)
	    गच्छ skip;
    }
    अन्यथा
    {
	// numeric or string option
	यदि (vim_strchr((char_u *)"=:&<", nextchar) == रिक्त
		|| prefix != PREFIX_NONE)
	{
	    errmsg = e_invalid_argument;
	    गच्छ skip;
	}

	यदि (flags & P_अङ्क)
	{
	    // numeric option
	    errmsg = do_set_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    यदि (errmsg != रिक्त)
		गच्छ skip;
	}
	अन्यथा यदि (opt_idx >= 0)
	{
	    // string option
	    यदि (do_set_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&मान_checked, &errmsg) == FAIL)
	    {
		यदि (errmsg != रिक्त)
		    गच्छ skip;
		*stopopteval = TRUE;
		गच्छ skip;
	    }
	}
	अन्यथा
	{
	    // key code option
	    errmsg = do_set_option_keycode(&arg, साँचो_name, nextchar);
	    यदि (errmsg != रिक्त)
		गच्छ skip;
	}
    }

    यदि (opt_idx >= 0)
	did_set_option(opt_idx, opt_flags, op == OP_NONE, मान_checked);

skip:
    *argp = arg;
    फर्काऊ errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    स्थिर वर्ण *
do_set_option(
    पूर्णाङ्क		opt_flags,
    char_u	**argp,
    char_u	*arg_start,
    char_u	**startarg,
    पूर्णाङ्क		*did_show,
    पूर्णाङ्क		*stopopteval,
    वर्ण	*errbuf,
    माप_प्रकार	errbuflen)
{
    पूर्णाङ्क		opt_idx;
    char_u	*arg;
    set_prefix_T prefix;	// no prefix, "no" prefix or "inv" prefix
    set_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	साँचो_name[2];
    पूर्णाङ्क		nextchar;	// next non-white char after option name
    पूर्णाङ्क		afterchar;	// character just after option name
    वर्ण	*errmsg = रिक्त;
    पूर्णाङ्क		साँचो;
    पूर्णाङ्क		len;

    prefix = get_option_prefix(argp);
    arg = *argp;

    // find end of name
    साँचो = 0;
    यदि (parse_option_name(arg, &opt_idx, &len, &साँचो) == FAIL)
	फर्काऊ e_invalid_argument;

    // remember character after option name
    afterchar = arg[len];

    यदि (in_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	यदि (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != रिक्त
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    गच्छ skip;
	}
    }
    अन्यथा
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	जबसम्म (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    यदि (op != OP_NONE)
	len++;

    nextchar = arg[len];

    यदि (opt_idx == -1 && साँचो == 0)	// found a mismatch: skip
    {
	यदि (in_vim9script() && arg > arg_start
		&& vim_strchr((char_u *)"!&<", *arg) != रिक्त)
	    errmsg = e_no_white_space_allowed_between_option_and;
	अन्यथा
	    errmsg = e_unknown_option;
	गच्छ skip;
    }

    यदि (opt_idx >= 0)
    {
	यदि (options[opt_idx].var == रिक्त)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    यदि (vim_strchr((char_u *)"=:!&<", nextchar) == रिक्त
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    गच्छ skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    अन्यथा
    {
	flags = P_STRING;
	varp = रिक्त;
	यदि (साँचो < 0)
	{
	    साँचो_name[0] = KEY2TERMCAP0(साँचो);
	    साँचो_name[1] = KEY2TERMCAP1(साँचो);
	}
	अन्यथा
	{
	    साँचो_name[0] = KS_साँचो;
	    साँचो_name[1] = (साँचो & 0xff);
	}
    }

    // Make sure the option value can be changed.
    यदि (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, prefix) == FAIL)
	गच्छ skip;

    पूर्णाङ्क cp_val = p_cp;
    यदि (vim_strchr((char_u *)"?=:!&<", nextchar) != रिक्त)
    {
	arg += len;
	यदि (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    यदि (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    अन्यथा		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	यदि (vim_strchr((char_u *)"?!&<", nextchar) != रिक्त
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    गच्छ skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    यदि (nextchar == '?'
	    || (prefix == PREFIX_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == रिक्त
		&& !(flags & P_BOOL)))
    {
	// print value
	यदि (*did_show)
	    सन्देश_putchar('\n');	    // cursor below last one
	अन्यथा
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	यदि (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ifdef FEAT_EVAL
	    यदि (p_verbose > 0)
	    {
		// Mention where the option was last set.
		यदि (varp == options[opt_idx].var)
		    last_set_सन्देश(options[opt_idx].script_ctx);
		अन्यथा यदि ((पूर्णाङ्क)options[opt_idx].indir & PV_WIN)
		    last_set_सन्देश(curwin->w_p_script_ctx[
			    (पूर्णाङ्क)options[opt_idx].indir & PV_MASK]);
		अन्यथा यदि ((पूर्णाङ्क)options[opt_idx].indir & PV_BUF)
		    last_set_सन्देश(curbuf->b_p_script_ctx[
			    (पूर्णाङ्क)options[opt_idx].indir & PV_MASK]);
	    }
#endif
	}
	अन्यथा
	{
	    char_u	    *p;

	    p = find_termcode(साँचो_name);
	    यदि (p == रिक्त)
	    {
		errmsg = e_साँचो_code_not_set;
		गच्छ skip;
	    }
	    अन्यथा
		(शून्य)show_one_termcode(साँचो_name, p, TRUE);
	}
	यदि (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    अन्यथा
    {
	errmsg = do_set_option_मान(opt_idx, opt_flags, &arg, prefix, op,
					flags, varp, साँचो_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    फर्काऊ errmsg;
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
    पूर्णाङ्क
do_set(
    char_u	*arg_start,	// option string (may be written to!)
    पूर्णाङ्क		opt_flags)
{
    char_u	*arg = arg_start;
    पूर्णाङ्क		i;
    पूर्णाङ्क		did_show = FALSE;   // already showed one value

    यदि (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	गच्छ theend;
    }

    जबसम्म (*arg != NUL)		// loop to process all options
    {
	यदि (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    यदि (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		set_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    अन्यथा
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	अन्यथा यदि (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	अन्यथा
	{
	    पूर्णाङ्क		stopopteval = FALSE;
	    वर्ण	*errmsg = रिक्त;
	    वर्ण	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_set_option(opt_flags, &arg, arg_start, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    यदि (stopopteval)
		रोक;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    लागि (i = 0; i < 2 ; ++i)
	    {
		जबसम्म (*arg != NUL && !VIM_ISWHITE(*arg))
		    यदि (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		यदि (*arg != '=')
		    रोक;
	    }

	    यदि (errmsg != रिक्त)
	    {
		i = vim_snprintf((वर्ण *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		यदि (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((वर्ण *)IObuff);	// show error highlighted
		--no_wait_return;

		फर्काऊ FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    यदि (silent_mode && did_show)
    {
	// After displaying option values in silent mode.
	silent_mode = FALSE;
	info_message = TRUE;	// use mch_msg(), not mch_errmsg()
	सन्देश_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = TRUE;
	info_message = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    फर्काऊ OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    शून्य
did_set_option(
    पूर्णाङ्क	    opt_idx,
    पूर्णाङ्क	    opt_flags,	    // possibly with OPT_MODELINE
    पूर्णाङ्क	    new_मान_2,	    // value was replaced completely
    पूर्णाङ्क	    मान_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = रिक्त;
    पूर्णाङ्क		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_SET;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    यदि (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    यदि (!मान_checked && (secure
#ifdef HAVE_SANDBOX
	    || sandbox != 0
#endif
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	यदि (flagsp_local != रिक्त)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    अन्यथा यदि (new_मान_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	यदि (flagsp_local != रिक्त)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    पूर्णाङ्क
string_to_साँचो(char_u *arg, पूर्णाङ्क multi_byte)
{
    यदि (*arg == '<' && arg[1])
	फर्काऊ find_साँचो_option(arg + 1, TRUE);
    यदि (*arg == '^' && arg[1])
    {
	पूर्णाङ्क साँचो = Ctrl_chr(arg[1]);
	यदि (साँचो == 0)		// ^@ is <Nul>
	    साँचो = K_ZERO;
	फर्काऊ साँचो;
    }
    यदि (multi_byte)
	फर्काऊ PTR2CHAR(arg);
    फर्काऊ *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    शून्य
did_set_title(शून्य)
{
    यदि (starting != NO_SCREEN
#ifdef FEAT_GUI
	    && !gui.starting
#endif
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    शून्य
set_options_bin(
    पूर्णाङ्क		oldval,
    पूर्णाङ्क		newval,
    पूर्णाङ्क		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    यदि (newval)
    {
	यदि (!oldval)		// switched on
	{
	    यदि (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    यदि (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	यदि (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	यदि (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    अन्यथा यदि (oldval)		// switched off
    {
	यदि (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	यदि (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#यदि defined(FEAT_EVAL)
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
    स्थिर char_u *
option_expand(पूर्णाङ्क opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    यदि (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == रिक्त)
	फर्काऊ रिक्त;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    यदि (val != रिक्त && STRLEN(val) > MAXPATHL)
	फर्काऊ रिक्त;

    यदि (val == रिक्त)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    पूर्णाङ्क esc = var == &p_tags || var == &p_path;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : रिक्त, FALSE,
#ifdef FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#endif
				  रिक्त);
    यदि (STRCMP(NameBuff, val) == 0)   // they are the same
	फर्काऊ रिक्त;

    फर्काऊ NameBuff;
}

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    स्थिर शून्य
didset_options(शून्य)
{
    // initialize the table for 'iskeyword' et.al.
    (शून्य)init_chartab();

    didset_string_options();

#ifdef FEAT_SPELL
    (शून्य)spell_check_msm();
    (शून्य)spell_check_sps();
    (शून्य)compile_cap_prog(curwin->w_s);
    (शून्य)did_set_spell_option();
#endif
    // set cedit_key
    (शून्य)did_set_cedit(रिक्त);
#ifdef FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_set_breakat(रिक्त);
#endif
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    स्थिर शून्य
didset_options2(शून्य)
{
    // Initialize the highlight_attr[] table.
    (शून्य)highlight_changed();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (शून्य)set_listchars_option(curwin, curwin->w_p_lcs, TRUE, रिक्त, 0);

    // Parse default for 'fillchars'.
    (शून्य)set_fillchars_option(curwin, curwin->w_p_fcs, TRUE, रिक्त, 0);

#ifdef FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (शून्य)did_set_clipboard(रिक्त);
#endif
#ifdef FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (शून्य)tabstop_set(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (शून्य)tabstop_set(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#endif
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    शून्य
check_options(शून्य)
{
    पूर्णाङ्क		opt_idx;

    लागि (opt_idx = 0; options[opt_idx].fullname != रिक्त; opt_idx++)
	यदि ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != रिक्त)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    पूर्णाङ्क
get_term_opt_idx(char_u **p)
{
    पूर्णाङ्क opt_idx;

    लागि (opt_idx = 1; options[opt_idx].fullname != रिक्त; opt_idx++)
	यदि (options[opt_idx].var == (char_u *)p)
	    फर्काऊ opt_idx;
    फर्काऊ -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    पूर्णाङ्क
set_term_option_alloced(char_u **p)
{
    पूर्णाङ्क		opt_idx = get_term_opt_idx(p);

    यदि (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    फर्काऊ opt_idx;
}

#यदि defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    पूर्णाङ्क
was_set_insecurely(win_T *wp, char_u *opt, पूर्णाङ्क opt_flags)
{
    पूर्णाङ्क	    idx = findoption(opt);
    long_u  *flagp;

    यदि (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	फर्काऊ (*flagp & P_INSECURE) != 0;
    }
    internal_error("was_set_insecurely()");
    फर्काऊ -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    स्थिर long_u *
insecure_flag(win_T *wp, पूर्णाङ्क opt_idx, पूर्णाङ्क opt_flags)
{
    यदि (opt_flags & OPT_LOCAL)
	विकल्प ((पूर्णाङ्क)options[opt_idx].indir)
	{
	    पक्ष PV_WRAP:	फर्काऊ &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    पक्ष PV_STL:	फर्काऊ &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    पक्ष PV_FDE:	फर्काऊ &wp->w_p_fde_flags;
	    पक्ष PV_FDT:	फर्काऊ &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    पक्ष PV_BEXPR:	फर्काऊ &wp->w_buffer->b_p_bexpr_flags;
#  endif
	    पक्ष PV_INDE:	फर्काऊ &wp->w_buffer->b_p_inde_flags;
	    पक्ष PV_FEX:	फर्काऊ &wp->w_buffer->b_p_fex_flags;
#  ifdef FEAT_FIND_ID
	    पक्ष PV_INEX:	फर्काऊ &wp->w_buffer->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    पक्ष PV_CPT:	फर्काऊ &wp->w_buffer->b_p_cpt_flags;
#  endif
# endif
	}
    अन्यथा
	// For global value of window-local options, use flags in w_allbuf_opt.
	विकल्प ((पूर्णाङ्क)options[opt_idx].indir)
	{
	    पक्ष PV_WRAP:	फर्काऊ &wp->w_allbuf_opt.wo_wrap_flags;
# यदि defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    पक्ष PV_FDE:	फर्काऊ &wp->w_allbuf_opt.wo_fde_flags;
	    पक्ष PV_FDT:	फर्काऊ &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    फर्काऊ &options[opt_idx].flags;
}
#endif

/*
 * Redraw the window title and/or tab page text later.
 */
    शून्य
redraw_titles(शून्य)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#यदि defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    पूर्णाङ्क
valid_name(char_u *val, वर्ण *allowed)
{
    char_u *s;

    लागि (s = val; *s != NUL; ++s)
	यदि (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == रिक्त)
	    फर्काऊ FALSE;
    फर्काऊ TRUE;
}

#यदि defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    शून्य
set_option_sctx_idx(पूर्णाङ्क opt_idx, पूर्णाङ्क opt_flags, sctx_T script_ctx)
{
    पूर्णाङ्क		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    पूर्णाङ्क		indir = (पूर्णाङ्क)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    यदि (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    यदि (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    यदि (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    यदि (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    यदि (both || (opt_flags & OPT_LOCAL))
    {
	यदि (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	अन्यथा यदि (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	    यदि (both)
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
is_option_मान_vim9(पूर्णाङ्क opt_idx, पूर्णाङ्क opt_flags)
{
    पूर्णाङ्क		indir = (पूर्णाङ्क)options[opt_idx].indir;
    sctx_T	*sctx = रिक्त;

    यदि ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    यदि ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	यदि (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_MASK];
	अन्यथा यदि (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_MASK];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    फर्काऊ !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_version >= SCRIPT_VERSION_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(वर्ण *name)
{
    पूर्णाङ्क idx = findoption((char_u *)name);

    यदि (idx >= 0)
	फर्काऊ &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    फर्काऊ रिक्त;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    शून्य
set_term_option_sctx_idx(वर्ण *name, पूर्णाङ्क opt_idx)
{
    char_u  buf[5];
    पूर्णाङ्क	    idx;

    यदि (name == रिक्त)
	idx = opt_idx;
    अन्यथा
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    यदि (idx >= 0)
	set_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#endif

#यदि defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    स्थिर शून्य
apply_optionset_autocmd(
	पूर्णाङ्क	opt_idx,
	दीर्घ	opt_flags,
	दीर्घ	oldval,
	दीर्घ	oldval_g,
	दीर्घ	newval,
	वर्ण	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    माप_प्रकार  buf_oldlen;
    माप_प्रकार  len;

    // Don't do this while starting up, failure or recursively.
    यदि (starting || errmsg != रिक्त || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	फर्काऊ;

    len = vim_snprintf_safelen((वर्ण *)buf_new, परिमाण(buf_new), "%ld", newval);
    set_vim_var_string(VV_OPTION_NEW, buf_new, (पूर्णाङ्क)len);
    len = vim_snprintf_safelen((वर्ण *)buf_type, परिमाण(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (पूर्णाङ्क)len);

    buf_oldlen = vim_snprintf_safelen((वर्ण *)buf_old, परिमाण(buf_old), "%ld", oldval);
    set_vim_var_string(VV_OPTION_OLD, buf_old, (पूर्णाङ्क)buf_oldlen);

    यदि (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (पूर्णाङ्क)buf_oldlen);
    }
    यदि (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (पूर्णाङ्क)buf_oldlen);
    }
    यदि ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	माप_प्रकार	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((वर्ण *)buf_old_global,
	    परिमाण(buf_old_global), "%ld", oldval_g);

	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (पूर्णाङ्क)buf_oldlen);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (पूर्णाङ्क)buf_old_globallen);
    }
    यदि (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (पूर्णाङ्क)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    रिक्त, FALSE, रिक्त);
    reset_v_option_vars();
}
#endif

#यदि defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    वर्ण *
did_set_arabic(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    यदि (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	यदि (!p_tbidi)
	{
	    // set rightleft mode
	    यदि (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		changed_window_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    यदि (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	यदि (STRCMP(p_enc, "utf-8") != 0)
	{
	    स्थिर वर्ण *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    सन्देश_source(HL_ATTR(HLF_W));
	    सन्देश_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = set_option_मान((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    अन्यथा
    {
	// 'arabic' is reset, handle various sub-settings.
	यदि (!p_tbidi)
	{
	    // reset rightleft mode
	    यदि (curwin->w_p_rl)
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
	curbuf->b_p_imsearch = B_IMODE_USE_घुसाउ;
# endif
    }

    फर्काऊ errmsg;
}
#endif

#यदि defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    वर्ण *
did_set_autochdir(optset_T *args UNUSED)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    फर्काऊ रिक्त;
}
#endif

#यदि defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    वर्ण *
did_set_ballooneval(optset_T *args)
{
    यदि (balloonEvalForTerm)
	फर्काऊ रिक्त;

    यदि (p_beval && !args->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    अन्यथा यदि (!p_beval && args->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    फर्काऊ रिक्त;
}
#endif

#यदि defined(FEAT_BEVAL_TERM)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    वर्ण *
did_set_balloonevalterm(optset_T *args UNUSED)
{
    mch_bevalterm_changed();
    फर्काऊ रिक्त;
}
#endif

/*
 * Process the updated 'binary' option value.
 */
    वर्ण *
did_set_द्विआधारी(optset_T *args)
{
    // when 'bin' is set also set some other options
    set_options_bin(args->os_oldval.boolean, curbuf->b_p_bin, args->os_flags);
    redraw_titles();

    फर्काऊ रिक्त;
}

/*
 * Process the updated 'buflisted' option value.
 */
    वर्ण *
did_set_buflisted(optset_T *args)
{
    // when 'buflisted' changes, trigger autocommands
    यदि (args->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						रिक्त, रिक्त, TRUE, curbuf);
    फर्काऊ रिक्त;
}

/*
 * Process the new 'cmdheight' option value.
 */
    वर्ण *
did_set_cmdheight(optset_T *args)
{
    दीर्घ old_मान = args->os_oldval.सङ्ख्या;
    वर्ण *errmsg = रिक्त;

    // if p_ch changed value, change the command line height
    यदि (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_argument_must_be_positive;
	p_ch = MIN_CMDHEIGHT;
    }
    यदि (p_ch > Rows - min_rows() + MIN_CMDHEIGHT)
	p_ch = Rows - min_rows() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    यदि ((p_ch != old_मान
		|| tabline_height() + topframe->fr_height != Rows - p_ch)
	    && full_screen
#ifdef FEAT_GUI
	    && !gui.starting
#endif
       )
	command_height();

    फर्काऊ errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    वर्ण *
did_set_compatible(optset_T *args UNUSED)
{
    compatible_set();
    फर्काऊ रिक्त;
}

#यदि defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    वर्ण *
did_set_conceallevel(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    यदि (curwin->w_p_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_cole = 0;
    }
    अन्यथा यदि (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_cole = 3;
    }
    यदि (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    अन्यथा यदि (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    फर्काऊ errmsg;
}
#endif

#यदि defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    वर्ण *
did_set_diff(optset_T *args UNUSED)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    यदि (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    फर्काऊ रिक्त;
}
#endif

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    वर्ण *
did_set_eof_eol_fixeol_bomb(optset_T *args UNUSED)
{
    // redraw the window title and tab page text
    redraw_titles();
    फर्काऊ रिक्त;
}

/*
 * Process the updated 'equalalways' option value.
 */
    वर्ण *
did_set_equalalways(optset_T *args)
{
    यदि (p_ea && !args->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    फर्काऊ रिक्त;
}

#यदि defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    वर्ण *
did_set_foldcolumn(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    यदि (curwin->w_p_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_fdc = 0;
    }
    अन्यथा यदि (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_fdc = 12;
    }
    यदि (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    अन्यथा यदि (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    फर्काऊ errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    वर्ण *
did_set_foldlevel(optset_T *args UNUSED)
{
    यदि (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    फर्काऊ रिक्त;
}

/*
 * Process the new 'foldminlines' option value.
 */
    वर्ण *
did_set_foldminlines(optset_T *args UNUSED)
{
    foldUpdateAll(curwin);
    फर्काऊ रिक्त;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    वर्ण *
did_set_foldnestmax(optset_T *args UNUSED)
{
    यदि (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    फर्काऊ रिक्त;
}
#endif

#यदि defined(FEAT_खोज_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    वर्ण *
did_set_hlsearch(optset_T *args UNUSED)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    set_no_hlsearch(FALSE);
    फर्काऊ रिक्त;
}
#endif

/*
 * Process the updated 'ignorecase' option value.
 */
    वर्ण *
did_set_ignorecase(optset_T *args UNUSED)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    यदि (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    फर्काऊ रिक्त;
}

#यदि defined(HAVE_आगत_METHOD)
/*
 * Process the updated 'imdisable' option value.
 */
    वर्ण *
did_set_imdisable(optset_T *args UNUSED)
{
    // Only de-activate it here, it will be enabled when changing mode.
    यदि (p_imdisable)
	im_set_active(FALSE);
    अन्यथा यदि (State & MODE_घुसाउ)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
    फर्काऊ रिक्त;
}
#endif

/*
 * Process the new 'iminsert' option value.
 */
    वर्ण *
did_set_iminsert(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    यदि (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    यदि (termcap_active)	// don't do this in the alternate screen
	showmode();
#यदि defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif

    फर्काऊ errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    वर्ण *
did_set_imsearch(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    यदि (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    फर्काऊ errmsg;
}

#यदि defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    वर्ण *
did_set_imstyle(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    यदि (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_argument;

    फर्काऊ errmsg;
}
#endif

/*
 * Process the updated 'insertmode' option value.
 */
    वर्ण *
did_set_insertmode(optset_T *args)
{
    // when 'insertmode' is set from an autocommand need to do work here
    यदि (p_im)
    {
	यदि ((State & MODE_घुसाउ) == 0)
	    need_start_insertmode = TRUE;
	stop_घुसाउ_mode = FALSE;
    }
    // only reset if it was set previously
    अन्यथा यदि (args->os_oldval.boolean)
    {
	need_start_insertmode = FALSE;
	stop_घुसाउ_mode = TRUE;
	यदि (restart_edit != 0 && mode_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    फर्काऊ रिक्त;
}

#यदि defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    वर्ण *
did_set_langnoremap(optset_T *args UNUSED)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    फर्काऊ रिक्त;
}

/*
 * Process the updated 'langremap' option value.
 */
    वर्ण *
did_set_langremap(optset_T *args UNUSED)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    फर्काऊ रिक्त;
}
#endif

/*
 * Process the new 'laststatus' option value.
 */
    वर्ण *
did_set_laststatus(optset_T *args UNUSED)
{
    last_status(FALSE);	// (re)set last window status line
    फर्काऊ रिक्त;
}

#यदि defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    वर्ण *
did_set_linespace(optset_T *args UNUSED)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    यदि (gui.in_use && gui_mch_adjust_charheight() == OK)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    फर्काऊ रिक्त;
}
#endif

/*
 * Process the updated 'lisp' option value.
 */
    वर्ण *
did_set_lisp(optset_T *args UNUSED)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (शून्य)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    फर्काऊ रिक्त;
}

/*
 * Process the new 'maxcombine' option value.
 */
    वर्ण *
did_set_maxcombine(optset_T *args UNUSED)
{
    यदि (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    अन्यथा यदि (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    फर्काऊ रिक्त;
}

/*
 * Process the updated 'modifiable' option value.
 */
    वर्ण *
did_set_modifiable(optset_T *args UNUSED)
{
    // when 'modifiable' is changed, redraw the window title

#ifdef FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    यदि (curbuf->b_p_ma && (term_in_normal_mode(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_term != रिक्त && !term_is_finished(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	फर्काऊ e_cannot_make_terminal_with_running_job_modifiable;
    }
#endif
    redraw_titles();

    फर्काऊ रिक्त;
}

/*
 * Process the updated 'modified' option value.
 */
    वर्ण *
did_set_modified(optset_T *args)
{
    यदि (!args->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_set = !!args->os_newval.boolean;
    फर्काऊ रिक्त;
}

#यदि defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    वर्ण *
did_set_mousehide(optset_T *args UNUSED)
{
    यदि (!p_mh)
	gui_mch_mousehide(FALSE);
    फर्काऊ रिक्त;
}
#endif

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    वर्ण *
did_set_सङ्ख्या_relativenumber(optset_T *args UNUSED)
{
#यदि defined(FEAT_SIGNS) && defined(FEAT_GUI)
    यदि (gui.in_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != रिक्त)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	यदि (!(curwin->w_p_nu && ((पूर्णाङ्क *)args->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#endif
    फर्काऊ रिक्त;
}

#यदि defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    वर्ण *
did_set_numberwidth(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    // 'numberwidth' must be positive
    यदि (curwin->w_p_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_nuw = 1;
    }
    अन्यथा यदि (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_nuw = 20;
    }
    यदि (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    अन्यथा यदि (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_पङ्क्ति_गन्ती = 0; // trigger a redraw

    फर्काऊ errmsg;
}
#endif

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    वर्ण *
did_set_osctimeoutlen(optset_T *args)
{
    यदि (p_ost < 0)
    {
	p_ost = args->os_oldval.सङ्ख्या;
	फर्काऊ e_argument_must_be_positive;
    }

    फर्काऊ रिक्त;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    वर्ण *
did_set_paste(optset_T *args UNUSED)
{
    स्थिर पूर्णाङ्क	old_p_paste = FALSE;
    स्थिर पूर्णाङ्क	save_sm = 0;
    स्थिर पूर्णाङ्क	save_sta = 0;
    स्थिर पूर्णाङ्क	save_ru = 0;
#ifdef FEAT_RIGHTLEFT
    स्थिर पूर्णाङ्क	save_ri = 0;
    स्थिर पूर्णाङ्क	save_hkmap = 0;
#endif
    buf_T	*buf;

    यदि (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	यदि (!old_p_paste)
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
		यदि (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != empty_option
					   ? vim_strsave(buf->b_p_vsts) : रिक्त;
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
	    यदि (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != empty_option
						  ? vim_strsave(p_vsts) : रिक्त;
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
	    यदि (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = empty_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#endif
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	यदि (p_ru)
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
	यदि (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = empty_option;
#endif
    }

    // Paste switched from on to off: Restore saved values.
    अन्यथा यदि (old_p_paste)
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
	    यदि (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : empty_option;
	    vim_free(buf->b_p_vsts_array);
	    यदि (buf->b_p_vsts && buf->b_p_vsts != empty_option)
		(शून्य)tabstop_set(buf->b_p_vsts, &buf->b_p_vsts_array);
	    अन्यथा
		buf->b_p_vsts_array = रिक्त;
#endif
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	यदि (p_ru != save_ru)
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
	यदि (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : empty_option;
#endif
    }

    old_p_paste = p_paste;

#यदि defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#endif

    फर्काऊ रिक्त;
}


#ifdef FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    वर्ण *
did_set_previewwindow(optset_T *args UNUSED)
{
    यदि (!curwin->w_p_pvw)
	फर्काऊ रिक्त;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	यदि (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    फर्काऊ e_preview_window_already_exists;
	}

    फर्काऊ रिक्त;
}
#endif

#यदि defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    वर्ण *
did_set_pyxversion(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    यदि (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_argument;

    फर्काऊ errmsg;
}
#endif

/*
 * Process the updated 'readonly' option value.
 */
    वर्ण *
did_set_readonly(optset_T *args)
{
    // when 'readonly' is reset globally, also reset readonlymode
    यदि (!curbuf->b_p_ro && (args->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    यदि (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    फर्काऊ रिक्त;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    वर्ण *
did_set_scrollbind(optset_T *args UNUSED)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    यदि (!curwin->w_p_scb)
	फर्काऊ रिक्त;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    फर्काऊ रिक्त;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    वर्ण *
did_set_maxsearchcount(optset_T *args UNUSED)
{
    वर्ण	*errmsg = रिक्त;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#define MAX_खोज_गन्ती 9999

    यदि (p_msc <= 0)
	errmsg = e_argument_must_be_positive;
    अन्यथा यदि (p_msc > MAX_खोज_गन्ती)
	errmsg = e_invalid_argument;

    यदि (errmsg != रिक्त)
	p_msc = 99;

    फर्काऊ errmsg;
#undef MAX_खोज_गन्ती
}

#यदि defined(BACKSLASH_IN_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    वर्ण *
did_set_shellslash(optset_T *args UNUSED)
{
    यदि (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    अन्यथा
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
    फर्काऊ रिक्त;
}
#endif

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    वर्ण *
did_set_shiftwidth_tabstop(optset_T *args)
{
    दीर्घ	*pp = (दीर्घ *)args->os_varp;
    वर्ण	*errmsg = रिक्त;

    यदि (curbuf->b_p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_ts = 8;
    }
    अन्यथा यदि (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_ts = 8;
    }
    यदि (p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ts = 8;
    }
    अन्यथा यदि (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	p_ts = 8;
    }

    यदि (curbuf->b_p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_गन्ती(curbuf->b_p_vts_array) > 0
		       ? tabstop_first(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#अन्यथा
	curbuf->b_p_sw = curbuf->b_p_ts;
#endif
    }
    यदि (p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_गन्ती(curbuf->b_p_vts_array) > 0
	     ? tabstop_first(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#अन्यथा
	p_sw = curbuf->b_p_ts;
#endif
    }

#ifdef FEAT_FOLDING
    यदि (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#endif
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    यदि (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    फर्काऊ errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    वर्ण *
did_set_showtabline(optset_T *args UNUSED)
{
    shell_new_rows();	// recompute window positions and heights
    फर्काऊ रिक्त;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    वर्ण *
did_set_smoothscroll(optset_T *args UNUSED)
{
    यदि (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    फर्काऊ रिक्त;
}

#यदि defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    वर्ण *
did_set_spell(optset_T *args UNUSED)
{
    यदि (curwin->w_p_spell)
	फर्काऊ parse_spelllang(curwin);

    फर्काऊ रिक्त;
}
#endif

/*
 * Process the updated 'swapfile' option value.
 */
    वर्ण *
did_set_swapfile(optset_T *args UNUSED)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    यदि (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    अन्यथा
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    फर्काऊ रिक्त;
}

#यदि defined(FEAT_TERMGUICOLORS)
    वर्ण *
did_set_termguicolors(optset_T *args UNUSED)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    यदि (
#  ifdef VIMDLL
	    !gui.in_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_set = TRUE;
	p_tgc = 0;
	फर्काऊ e_24_bit_colors_are_not_supported_on_this_environment;
    }
    यदि (is_term_win32())
	swap_tcap();
# endif
# ifdef FEAT_GUI
    यदि (!gui.in_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    यदि (is_term_win32())
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

    फर्काऊ रिक्त;
}
#endif

    वर्ण *
did_set_termsync(optset_T *args UNUSED)
{
    यदि (!p_tsy)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);
    फर्काऊ रिक्त;
}

#यदि defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    वर्ण *
did_set_termwinscroll(optset_T *args)
{
    दीर्घ	*pp = (दीर्घ *)args->os_varp;
    वर्ण	*errmsg = रिक्त;

    यदि (*pp < 1)
    {
	errmsg = e_argument_must_be_positive;
	*pp = 1;
    }

    फर्काऊ errmsg;
}
#endif

/*
 * Process the updated 'terse' option value.
 */
    वर्ण *
did_set_terse(optset_T *args UNUSED)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_खोज);

    // insert 's' in p_shm
    यदि (p_terse && p == रिक्त)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    अन्यथा यदि (!p_terse && p != रिक्त)
	STRMOVE(p, p + 1);
    फर्काऊ रिक्त;
}

/*
 * Process the updated 'textauto' option value.
 */
    वर्ण *
did_set_textauto(optset_T *args)
{
    // when 'textauto' is set or reset also change 'fileformats'
    set_string_option_direct((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | args->os_flags, 0);

    फर्काऊ रिक्त;
}

/*
 * Process the updated 'textmode' option value.
 */
    वर्ण *
did_set_textmode(optset_T *args)
{
    // when 'textmode' is set or reset also change 'fileformat'
    set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, args->os_flags);

    फर्काऊ रिक्त;
}

/*
 * Process the new 'textwidth' option value.
 */
    वर्ण *
did_set_textwidth(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    यदि (curbuf->b_p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_tw = 0;
    }
    यदि (p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tw = 0;
    }
#ifdef FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(रिक्त, wp);
    }
#endif

    फर्काऊ errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    वर्ण *
did_set_title_icon(optset_T *args UNUSED)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_set_title();
    फर्काऊ रिक्त;
}

/*
 * Process the new 'titlelen' option value.
 */
    वर्ण *
did_set_titlelen(optset_T *args)
{
    दीर्घ old_मान = args->os_oldval.सङ्ख्या;
    वर्ण *errmsg = रिक्त;

    // if 'titlelen' has changed, redraw the title
    यदि (p_titlelen < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_titlelen = 85;
    }
    यदि (starting != NO_SCREEN && old_मान != p_titlelen)
	need_maketitle = TRUE;

    फर्काऊ errmsg;
}

#यदि defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    वर्ण *
did_set_undofile(optset_T *args)
{
    // Only take action when the option was set.
    यदि (!curbuf->b_p_udf && !p_udf)
	फर्काऊ रिक्त;

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
	यदि ((curbuf == save_curbuf
		    || (args->os_flags & OPT_GLOBAL)
		    || args->os_flags == 0)
		&& !curbufIsChanged() && curbuf->b_ml.ml_mfp != रिक्त)
	{
# ifdef FEAT_CRYPT
	    यदि (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
		जारी;
# endif
	    u_compute_hash(hash);
	    u_read_undo(रिक्त, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    फर्काऊ रिक्त;
}
#endif

/*
 * Process the new global 'undolevels' option value.
 */
    स्थिर शून्य
update_global_undolevels(दीर्घ मान, दीर्घ old_मान)
{
    // sync undo before 'undolevels' changes

    // use the old value, otherwise u_sync() may not work properly
    p_ul = old_मान;
    u_sync(TRUE);
    p_ul = मान;
}

/*
 * Process the new buffer local 'undolevels' option value.
 */
    स्थिर शून्य
update_buflocal_undolevels(दीर्घ मान, दीर्घ old_मान)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_मान;
    u_sync(TRUE);
    curbuf->b_p_ul = मान;
}

/*
 * Process the new 'undolevels' option value.
 */
    वर्ण *
did_set_undolevels(optset_T *args)
{
    दीर्घ *pp = (दीर्घ *)args->os_varp;

    यदि (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(args->os_newval.सङ्ख्या,
						       args->os_oldval.सङ्ख्या);
    अन्यथा यदि (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(args->os_newval.सङ्ख्या,
						       args->os_oldval.सङ्ख्या);

    फर्काऊ रिक्त;
}

/*
 * Process the new 'updatecount' option value.
 */
    वर्ण *
did_set_updatecount(optset_T *args)
{
    दीर्घ old_मान = args->os_oldval.सङ्ख्या;
    वर्ण *errmsg = रिक्त;

    // when 'updatecount' changes from zero to non-zero, open swap files
    यदि (p_uc < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_uc = 100;
    }
    यदि (p_uc && !old_मान)
	ml_open_files();

    फर्काऊ errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    वर्ण *
did_set_weirdinvert(optset_T *args)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    यदि (p_wiv && !args->os_oldval.boolean)
	T_XS = (char_u *)"y";
    अन्यथा यदि (!p_wiv && args->os_oldval.boolean)
	T_XS = empty_option;
    p_wiv = (*T_XS != NUL);

    फर्काऊ रिक्त;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    वर्ण *
did_set_wildchar(optset_T *args)
{
    दीर्घ c = *(दीर्घ *)args->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    यदि (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	फर्काऊ e_invalid_argument;

    फर्काऊ रिक्त;
}

/*
 * Process the new 'window' option value.
 */
    वर्ण *
did_set_window(optset_T *args UNUSED)
{
    यदि (p_window < 1)
	p_window = 1;
    अन्यथा यदि (p_window >= Rows)
	p_window = Rows - 1;
    फर्काऊ रिक्त;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    वर्ण *
did_set_winheight_helpheight(optset_T *args)
{
    दीर्घ	*pp = (दीर्घ *)args->os_varp;
    वर्ण	*errmsg = रिक्त;

    यदि (p_wh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wh = 1;
    }
    यदि (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    यदि (p_hh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hh = 0;
    }

    // Change window height NOW
    यदि (!ONE_WINDOW)
    {
	यदि (pp == &p_wh && curwin->w_height < p_wh)
	    win_setheight((पूर्णाङ्क)p_wh);
	यदि (pp == &p_hh && curbuf->b_help && curwin->w_height < p_hh)
	    win_setheight((पूर्णाङ्क)p_hh);
    }

    फर्काऊ errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    वर्ण *
did_set_winminheight(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    यदि (p_wmh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmh = 0;
    }
    यदि (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    फर्काऊ errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    वर्ण *
did_set_winminwidth(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    यदि (p_wmw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmw = 0;
    }
    यदि (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    फर्काऊ errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    वर्ण *
did_set_winwidth(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    यदि (p_wiw < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wiw = 1;
    }
    यदि (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    यदि (!ONE_WINDOW && curwin->w_width < p_wiw)
	win_setwidth((पूर्णाङ्क)p_wiw);

    फर्काऊ errmsg;
}

#यदि defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    वर्ण *
did_set_wlsteal(optset_T *args UNUSED)
{
    clip_reset_wayland();

    फर्काऊ रिक्त;
}
#endif

#यदि defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    वर्ण *
did_set_wltimeoutlen(optset_T *args)
{
    यदि (p_wtm < 0)
    {
	p_wtm = args->os_oldval.सङ्ख्या;
	फर्काऊ e_argument_must_be_positive;
    }

    फर्काऊ रिक्त;
}
#endif

/*
 * Process the updated 'wrap' option value.
 */
    वर्ण *
did_set_wrap(optset_T *args UNUSED)
{
    // Set w_leftcol or w_skipcol to zero.
    यदि (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    अन्यथा
	curwin->w_skipcol = 0;

    फर्काऊ रिक्त;
}

#ifdef FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    वर्ण *
did_set_xhistory(optset_T *args)
{
    पूर्णाङ्क is_p_chi = (दीर्घ*)args->os_varp == &p_chi;
    पूर्णाङ्क err;
    दीर्घ *arg = (is_p_chi) ? &p_chi :(दीर्घ*)args->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    यदि (*arg < 1)
    {
	*arg = args->os_oldval.सङ्ख्या;
	फर्काऊ e_cannot_have_negative_or_zero_सङ्ख्या_of_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    यदि (*arg > 100)
    {
	*arg = args->os_oldval.सङ्ख्या;
	फर्काऊ e_cannot_have_more_than_hundred_quickfix;
    }

    यदि (is_p_chi)
	err = qf_resize_थाक(*arg);
    अन्यथा
	err = ll_resize_थाक(curwin, *arg);

    यदि (err == FAIL)
	फर्काऊ e_failed_resizing_quickfix_थाक;

    फर्काऊ रिक्त;
}
#endif

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    स्थिर वर्ण *
set_bool_option(
    पूर्णाङ्क		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    पूर्णाङ्क		मान,			// new value
    पूर्णाङ्क		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    पूर्णाङ्क		old_मान = *(पूर्णाङ्क *)varp;
#यदि defined(FEAT_EVAL)
    पूर्णाङ्क		old_global_मान = 0;
#endif
    वर्ण	*errmsg = रिक्त;

    // Disallow changing some options from secure mode
    यदि ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	फर्काऊ e_not_allowed_here;

#यदि defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    यदि ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_मान = *(पूर्णाङ्क *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *(पूर्णाङ्क *)varp = मान;	    // set the new value
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    यदि ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(पूर्णाङ्क *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = मान;

    // Handle side effects of changing a bool option.
    यदि (options[opt_idx].opt_did_set_cb != रिक्त)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.boolean = old_मान;
	args.os_newval.boolean = मान;
	args.os_errbuf = रिक्त;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
	यदि (errmsg != रिक्त)
	    फर्काऊ errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_SET;

#यदि defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(दीर्घ)(old_मान ? TRUE : FALSE),
				(दीर्घ)(old_global_मान ? TRUE : FALSE),
				(दीर्घ)(मान ? TRUE : FALSE), रिक्त);
#endif

    comp_col();			    // in case 'ruler' or 'showcmd' changed

    यदि (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    यदि ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    फर्काऊ errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    स्थिर वर्ण *
check_अङ्क_option_bounds(
    दीर्घ	*pp,
    दीर्घ	old_मान,
    दीर्घ	old_Rows,
    दीर्घ	old_Columns,
    वर्ण	*errbuf,
    माप_प्रकार	errbuflen,
    वर्ण	*errmsg)
{
    यदि (Rows < min_rows_for_all_tabpages() && full_screen)
    {
	यदि (errbuf != रिक्त)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_पङ्क्तिहरू), min_rows_for_all_tabpages());
	    errmsg = errbuf;
	}
	Rows = min_rows_for_all_tabpages();
    }
    यदि (Columns < MIN_COLUMNS && full_screen)
    {
	यदि (errbuf != रिक्त)
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
    यदि (old_Rows != Rows || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	यदि (updating_screen)
	    *pp = old_मान;
	अन्यथा यदि (full_screen
#ifdef FEAT_GUI
		&& !gui.starting
#endif
		)
	    set_shellsize((पूर्णाङ्क)Columns, (पूर्णाङ्क)Rows, TRUE);
	अन्यथा
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    यदि (cmdline_row > Rows - p_ch && Rows > p_ch)
		cmdline_row = Rows - p_ch;
	}
	यदि (p_window >= Rows || !option_was_set((char_u *)"window"))
	    p_window = Rows - 1;
    }

    यदि (p_tm < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tm = 0;
    }
    यदि ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_height
		    && curwin->w_height > 0))
	    && full_screen)
    {
	यदि (pp == &(curwin->w_p_scr))
	{
	    यदि (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_size;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	अन्यथा यदि (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	अन्यथा // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_height;
    }
    यदि (p_hi < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hi = 0;
    }
    अन्यथा यदि (p_hi > 10000)
    {
	errmsg = e_invalid_argument;
	p_hi = 10000;
    }
    यदि (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_argument;
	p_re = 0;
    }
    यदि (p_report < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_report = 1;
    }
    यदि ((p_sj < -100 || p_sj >= Rows) && full_screen)
    {
	यदि (Rows != old_Rows)	// Rows changed, just adjust p_sj
	    p_sj = Rows / 2;
	अन्यथा
	{
	    errmsg = e_invalid_scroll_size;
	    p_sj = 1;
	}
    }
    यदि (p_so < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_so = 0;
    }
    यदि (p_sop < 0 && full_screen)
    {
	errmsg = e_invalid_argument;
	p_sop = 0;
    }
    यदि (p_siso < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_siso = 0;
    }
    यदि (p_cwh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_cwh = 1;
    }
    यदि (p_ut < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ut = 2000;
    }
    यदि (p_ss < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ss = 0;
    }

    फर्काऊ errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    स्थिर वर्ण *
set_अङ्क_option(
    पूर्णाङ्क		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    दीर्घ	मान,			// new value
    वर्ण	*errbuf,		// buffer for error messages
    माप_प्रकार	errbuflen,		// length of "errbuf"
    पूर्णाङ्क		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    वर्ण	*errmsg = रिक्त;
    दीर्घ	old_मान = *(दीर्घ *)varp;
#यदि defined(FEAT_EVAL)
    दीर्घ	old_global_मान = 0;	// only used when setting a local and
					// global option
#endif
    दीर्घ	old_Rows = Rows;	// remember old Rows
    दीर्घ	old_Columns = Columns;	// remember old Columns
    दीर्घ	*pp = (दीर्घ *)varp;

    // Disallow changing some options from secure mode.
    यदि ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	फर्काऊ e_not_allowed_here;

#यदि defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    यदि ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_मान = *(दीर्घ *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *pp = मान;
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    यदि ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(दीर्घ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = मान;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    यदि (options[opt_idx].opt_did_set_cb != रिक्त)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.सङ्ख्या = old_मान;
	args.os_newval.सङ्ख्या = मान;
	args.os_errbuf = रिक्त;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
    }

    // Check the bounds for numeric options here
    errmsg = check_अङ्क_option_bounds(pp, old_मान, old_Rows, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_SET;

#यदि defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_मान, old_global_मान,
								मान, errmsg);
#endif

    comp_col();			    // in case 'columns' or 'ls' changed

    यदि (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    यदि ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    फर्काऊ errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    शून्य
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    पूर्णाङ्क		doclear = (flags & P_RCLR) == P_RCLR;
    पूर्णाङ्क		all = ((flags & P_RALL) == P_RALL || doclear);

    यदि ((flags & P_RSTAT) || all)	// mark all status lines dirty
	status_redraw_all();

    यदि ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	यदि (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	अन्यथा
	    changed_window_setting();
    }
    यदि (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    यदि (doclear)
	redraw_all_later(UPD_CLEAR);
    अन्यथा यदि (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    पूर्णाङ्क
findoption(char_u *arg)
{
    पूर्णाङ्क		    opt_idx;
    वर्ण	    *s, *p;
    स्थिर लघु    quick_tab[27] = {0, 0};	// quick access table
    पूर्णाङ्क		    is_term_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    यदि (quick_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	पूर्णाङ्क last_opt_idx;
	लागि (last_opt_idx = 0; options[last_opt_idx].fullname != रिक्त;
		last_opt_idx++)
	    ;
	लागि (पूर्णाङ्क tab_idx = 1; tab_idx < 27; tab_idx++)
	    quick_tab[tab_idx] = last_opt_idx;

	p = options[0].fullname;
	लागि (opt_idx = 1; opt_idx < last_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    यदि (s[0] != p[0])
	    {
		यदि (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		अन्यथा
		    quick_tab[Char_Ord_तल्लो(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    यदि (arg[0] < 'a' || arg[0] > 'z')
	फर्काऊ -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    यदि (is_term_opt)
	opt_idx = quick_tab[26];
    अन्यथा
	opt_idx = quick_tab[Char_Ord_तल्लो(arg[0])];
    लागि (; (s = options[opt_idx].fullname) != रिक्त && s[0] == arg[0]; opt_idx++)
    {
	यदि (STRCMP(arg, s) == 0)		    // match full name
	    रोक;
    }
    यदि (s != रिक्त && s[0] != arg[0])
	s = रिक्त;
    यदि (s == रिक्त && !is_term_opt)
    {
	opt_idx = quick_tab[Char_Ord_तल्लो(arg[0])];
	लागि ( ; options[opt_idx].fullname != रिक्त; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    यदि (s != रिक्त && STRCMP(arg, s) == 0)   // match short name
		रोक;
	    s = रिक्त;
	}
    }
    यदि (s == रिक्त)
	opt_idx = -1;
    फर्काऊ opt_idx;
}

#यदि defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
get_option_मान(
    char_u	*name,
    दीर्घ	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    पूर्णाङ्क		*flagsp,
    पूर्णाङ्क		scope)
{
    पूर्णाङ्क		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    यदि (opt_idx < 0)		    // option not in the table
    {
	पूर्णाङ्क साँचो;

	यदि (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (साँचो = find_साँचो_option(name, FALSE)) != 0)
	{
	    char_u साँचो_name[2];
	    char_u *p;

	    यदि (flagsp != रिक्त)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    यदि (साँचो < 0)
	    {
		साँचो_name[0] = KEY2TERMCAP0(साँचो);
		साँचो_name[1] = KEY2TERMCAP1(साँचो);
	    }
	    अन्यथा
	    {
		साँचो_name[0] = KS_साँचो;
		साँचो_name[1] = (साँचो & 0xff);
	    }
	    p = find_termcode(साँचो_name);
	    यदि (p != रिक्त)
	    {
		यदि (stringval != रिक्त)
		    *stringval = vim_strsave(p);
		फर्काऊ gov_string;
	    }
	}
	फर्काऊ gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    यदि (flagsp != रिक्त)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    यदि (options[opt_idx].flags & P_STRING)
    {
	यदि (varp == रिक्त)		    // hidden option
	    फर्काऊ gov_hidden_string;
	यदि (stringval != रिक्त)
	{
	    यदि ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    अन्यथा यदि ((char_u **)varp == &curbuf->b_p_साँचो
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    अन्यथा
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	फर्काऊ gov_string;
    }

    यदि (varp == रिक्त)		    // hidden option
	फर्काऊ (options[opt_idx].flags & P_अङ्क)
					 ? gov_hidden_सङ्ख्या : gov_hidden_bool;
    यदि (options[opt_idx].flags & P_अङ्क)
	*numval = *(दीर्घ *)varp;
    अन्यथा
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	यदि ((पूर्णाङ्क *)varp == &curbuf->b_changed)
	    *numval = curbufIsChanged();
	अन्यथा
	    *numval = (दीर्घ) *(पूर्णाङ्क *)varp;
    }
    फर्काऊ (options[opt_idx].flags & P_अङ्क) ? gov_सङ्ख्या : gov_bool;
}
#endif

#यदि defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    पूर्णाङ्क
get_option_मान_strict(
    char_u	*name,
    दीर्घ	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    पूर्णाङ्क		opt_type,
    शून्य	*from)
{
    पूर्णाङ्क		opt_idx;
    char_u	*varp = रिक्त;
    संरचना vimoption *p;
    पूर्णाङ्क		r = 0;

    opt_idx = findoption(name);
    यदि (opt_idx < 0)
	फर्काऊ 0;

    p = &(options[opt_idx]);

    // Hidden option
    यदि (p->var == रिक्त)
	फर्काऊ 0;

    यदि (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    अन्यथा यदि (p->flags & P_अङ्क)
	r |= SOPT_अङ्क;
    अन्यथा यदि (p->flags & P_STRING)
	r |= SOPT_STRING;

    यदि (p->indir == PV_NONE)
    {
	यदि (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	अन्यथा
	    फर्काऊ 0; // Did not request global-only option
    }
    अन्यथा
    {
	यदि (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	अन्यथा यदि (opt_type == SREQ_GLOBAL)
	    फर्काऊ 0; // Requested global option

	यदि (p->indir & PV_WIN)
	{
	    यदि (opt_type == SREQ_BUF)
		फर्काऊ 0; // Did not request window-local option
	    अन्यथा
		r |= SOPT_WIN;
	}
	अन्यथा यदि (p->indir & PV_BUF)
	{
	    यदि (opt_type == SREQ_WIN)
		फर्काऊ 0; // Did not request buffer-local option
	    अन्यथा
		r |= SOPT_BUF;
	}
    }

    यदि (stringval == रिक्त)
	फर्काऊ r;

    यदि (opt_type == SREQ_GLOBAL)
	varp = p->var;
    अन्यथा
    {
	यदि (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    यदि (p->indir == PV_MOD)
	    {
		*numval = bufIsChanged((buf_T *)from);
		varp = रिक्त;
	    }
# ifdef FEAT_CRYPT
	    अन्यथा यदि (p->indir == PV_साँचो)
	    {
		// never return the value of the crypt key
		*stringval = रिक्त;
		varp = रिक्त;
	    }
# endif
	    अन्यथा
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
	अन्यथा यदि (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_buffer;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	}
	यदि (varp == p->var)
	    फर्काऊ (r | SOPT_UNSET);
    }

    यदि (varp != रिक्त)
    {
	यदि (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	अन्यथा यदि (p->flags & P_अङ्क)
	    *numval = *(दीर्घ *) varp;
	अन्यथा
	    *numval = *(पूर्णाङ्क *)varp;
    }

    फर्काऊ r;
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
option_iter_next(शून्य **option, पूर्णाङ्क opt_type)
{
    संरचना vimoption	*ret = रिक्त;
    गर
    {
	यदि (*option == रिक्त)
	    *option = (शून्य *) options;
	अन्यथा यदि (((संरचना vimoption *) (*option))->fullname == रिक्त)
	{
	    *option = रिक्त;
	    फर्काऊ रिक्त;
	}
	अन्यथा
	    *option = (शून्य *) (((संरचना vimoption *) (*option)) + 1);

	ret = ((संरचना vimoption *) (*option));

	// Hidden option
	यदि (ret->var == रिक्त)
	{
	    ret = रिक्त;
	    जारी;
	}

	विकल्प (opt_type)
	{
	    पक्ष SREQ_GLOBAL:
		यदि (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = रिक्त;
		रोक;
	    पक्ष SREQ_BUF:
		यदि (!(ret->indir & PV_BUF))
		    ret = रिक्त;
		रोक;
	    पक्ष SREQ_WIN:
		यदि (!(ret->indir & PV_WIN))
		    ret = रिक्त;
		रोक;
	    स्वभाव:
		internal_error("option_iter_next()");
		फर्काऊ रिक्त;
	}
    }
    जबसम्म (ret == रिक्त);

    फर्काऊ (char_u *)ret->fullname;
}
#endif

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(पूर्णाङ्क opt_idx)
{
    फर्काऊ options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    शून्य
set_option_flag(पूर्णाङ्क opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    पूर्णाङ्क
is_global_option(पूर्णाङ्क opt_idx)
{
    फर्काऊ options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    पूर्णाङ्क
is_global_local_option(पूर्णाङ्क opt_idx)
{
    फर्काऊ options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    पूर्णाङ्क
is_window_local_option(पूर्णाङ्क opt_idx)
{
    फर्काऊ options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    पूर्णाङ्क
is_hidden_option(पूर्णाङ्क opt_idx)
{
    फर्काऊ options[opt_idx].var == रिक्त;
}

#यदि defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    पूर्णाङ्क
is_crypt_साँचो_option(पूर्णाङ्क opt_idx)
{
    फर्काऊ options[opt_idx].indir == PV_साँचो;
}
#endif

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    वर्ण *
set_option_मान(
    char_u	*name,
    दीर्घ	सङ्ख्या,
    char_u	*string,
    पूर्णाङ्क		opt_flags)	// OPT_LOCAL or 0 (both)
{
    पूर्णाङ्क		opt_idx;
    char_u	*varp;
    long_u	flags;
    स्थिर वर्ण	errbuf[ERR_BUFLEN];
    पूर्णाङ्क		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    यदि (opt_idx < 0)
    {
	पूर्णाङ्क साँचो;

	यदि (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (साँचो = find_साँचो_option(name, FALSE)) != 0)
	{
	    char_u साँचो_name[2];

	    यदि (साँचो < 0)
	    {
		साँचो_name[0] = KEY2TERMCAP0(साँचो);
		साँचो_name[1] = KEY2TERMCAP1(साँचो);
	    }
	    अन्यथा
	    {
		साँचो_name[0] = KS_साँचो;
		साँचो_name[1] = (साँचो & 0xff);
	    }
	    add_termcode(साँचो_name, string, FALSE);
	    यदि (full_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    फर्काऊ रिक्त;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    अन्यथा
    {
	flags = options[opt_idx].flags;
#ifdef HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	यदि (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_in_sandbox));
	    फर्काऊ रिक्त;
	}
#endif
	यदि (flags & P_STRING)
	    फर्काऊ set_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	यदि (varp != रिक्त)	// hidden option is not changed
	{
	    यदि (सङ्ख्या == 0 && string != रिक्त)
	    {
		पूर्णाङ्क idx;

		// Either we are given a string or we are setting option
		// to zero.
		लागि (idx = 0; string[idx] == '0'; ++idx)
		    ;
		यदि (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_सङ्ख्या_required_after_str_equal_str),
								 name, string);
		    फर्काऊ रिक्त;     // do nothing as we hit an error

		}
	    }
	    यदि (flags & P_अङ्क)
	    {
		फर्काऊ set_अङ्क_option(opt_idx, varp, सङ्ख्या,
					   errbuf, परिमाण(errbuf), opt_flags);
	    }
	    अन्यथा
		फर्काऊ set_bool_option(opt_idx, varp, (पूर्णाङ्क)सङ्ख्या, opt_flags);
	}

    }
    फर्काऊ रिक्त;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    शून्य
set_option_मान_give_err(
    char_u	*name,
    दीर्घ	सङ्ख्या,
    char_u	*string,
    पूर्णाङ्क		opt_flags)	// OPT_LOCAL or 0 (both)
{
    वर्ण *errmsg = set_option_मान(name, सङ्ख्या, string, opt_flags);

    यदि (errmsg != रिक्त)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_term_code(char_u *tname)
{
    पूर्णाङ्क	    opt_idx;
    char_u  *varp;

    यदि (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	फर्काऊ रिक्त;
    यदि ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	यदि (varp != रिक्त)
	    varp = *(char_u **)(varp);
	फर्काऊ varp;
    }
    फर्काऊ find_termcode(tname + 2);
}

    char_u *
get_highlight_default(शून्य)
{
    पूर्णाङ्क i;

    i = findoption((char_u *)"hl");
    यदि (i >= 0)
	फर्काऊ options[i].def_val[VI_DEFAULT];
    फर्काऊ (char_u *)रिक्त;
}

    char_u *
get_encoding_default(शून्य)
{
    पूर्णाङ्क i;

    i = findoption((char_u *)"enc");
    यदि (i >= 0)
	फर्काऊ options[i].def_val[VI_DEFAULT];
    फर्काऊ (char_u *)रिक्त;
}

#यदि defined(FEAT_QUICKFIX)
    पूर्णाङ्क
is_option_allocated(वर्ण *name)
{
    पूर्णाङ्क idx = findoption((char_u *)name);

    फर्काऊ idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#endif

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    स्थिर पूर्णाङ्क
find_साँचो_option(char_u *arg_arg, पूर्णाङ्क has_lt)
{
    पूर्णाङ्क		साँचो = 0;
    पूर्णाङ्क		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    यदि (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	यदि (!has_lt || arg[4] == '>')
	    साँचो = TERMCAP2_साँचो(arg[2], arg[3]);
    }
    अन्यथा यदि (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	साँचो = find_special_साँचो(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_साँचो | FSK_SIMPLIFY, रिक्त);
	यदि (modifiers)		    // can't handle modifiers here
	    साँचो = 0;
    }
    फर्काऊ साँचो;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    स्थिर शून्य
showoptions(
    पूर्णाङ्क		all,
    पूर्णाङ्क		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    संरचना vimoption	*p;
    पूर्णाङ्क			col;
    पूर्णाङ्क			isterm;
    char_u		*varp;
    संरचना vimoption	**वस्तुहरू;
    पूर्णाङ्क			item_गन्ती;
    पूर्णाङ्क			run;
    पूर्णाङ्क			row, rows;
    पूर्णाङ्क			cols;
    पूर्णाङ्क			i;
    पूर्णाङ्क			len;

#define INC 20
#define GAP 3

    वस्तुहरू = ALLOC_MULT(संरचना vimoption *, OPTION_गन्ती);
    यदि (वस्तुहरू == रिक्त)
	फर्काऊ;

    // Highlight title
    यदि (all == 2)
	सन्देश_puts_title(_("\n--- Terminal codes ---"));
    अन्यथा यदि (opt_flags & OPT_GLOBAL)
	सन्देश_puts_title(_("\n--- Global option values ---"));
    अन्यथा यदि (opt_flags & OPT_LOCAL)
	सन्देश_puts_title(_("\n--- Local option values ---"));
    अन्यथा
	सन्देश_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    लागि (run = 1; run <= 2 && !got_int; ++run)
    {
	// collect the items in items[]
	item_गन्ती = 0;
	लागि (p = &options[0]; p->fullname != रिक्त; p++)
	{
	    // apply :filter /pat/
	    यदि (message_filtered((char_u *)p->fullname))
		जारी;

	    varp = रिक्त;
	    isterm = istermoption(p);
	    यदि ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		यदि (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    अन्यथा
		varp = get_varp(p);
	    यदि (varp != रिक्त
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		यदि (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		अन्यथा यदि (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		अन्यथा
		{
		    option_value2string(p, opt_flags);
		    len = (पूर्णाङ्क)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		यदि ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    वस्तुहरू[item_गन्ती++] = p;
	    }
	}

	// display the items
	यदि (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    यदि (cols == 0)
		cols = 1;
	    rows = (item_गन्ती + cols - 1) / cols;
	}
	अन्यथा	// run == 2
	    rows = item_गन्ती;
	लागि (row = 0; row < rows && !got_int; ++row)
	{
	    सन्देश_putchar('\n');			// go to next line
	    यदि (got_int)			// 'q' typed in more
		रोक;
	    col = 0;
	    लागि (i = row; i < item_गन्ती; i += rows)
	    {
		सन्देश_col = col;			// make columns
		showoneopt(वस्तुहरू[i], opt_flags);
		col += INC;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(वस्तुहरू);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    स्थिर पूर्णाङ्क
optval_default(संरचना vimoption *p, char_u *varp, पूर्णाङ्क compatible)
{
    पूर्णाङ्क		dvi;

    यदि (varp == रिक्त)
	फर्काऊ TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    यदि (p->flags & P_अङ्क)
	फर्काऊ (*(दीर्घ *)varp == (दीर्घ)(long_i)p->def_val[dvi]);
    यदि (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	फर्काऊ (*(पूर्णाङ्क *)varp == (पूर्णाङ्क)(दीर्घ)(long_i)p->def_val[dvi]);
    // P_STRING
    फर्काऊ (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    स्थिर शून्य
showoneopt(
    संरचना vimoption	*p,
    पूर्णाङ्क			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    पूर्णाङ्क		save_silent = silent_mode;

    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    यदि ((p->flags & P_BOOL) && ((पूर्णाङ्क *)varp == &curbuf->b_changed
					? !curbufIsChanged() : !*(पूर्णाङ्क *)varp))
	सन्देश_puts("no");
    अन्यथा यदि ((p->flags & P_BOOL) && *(पूर्णाङ्क *)varp < 0)
	सन्देश_puts("--");
    अन्यथा
	सन्देश_puts("  ");
    सन्देश_puts(p->fullname);
    यदि (!(p->flags & P_BOOL))
    {
	सन्देश_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	सन्देश_outtrans(NameBuff);
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
    पूर्णाङ्क
makeset(खाता *fd, पूर्णाङ्क opt_flags, पूर्णाङ्क local_only)
{
    संरचना vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = रिक्त;	// fresh value
    वर्ण		*cmd;
    पूर्णाङ्क			round;
    पूर्णाङ्क			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    लागि (pri = 1; pri >= 0; --pri)
    {
      लागि (p = &options[0]; !istermoption(p); p++)
	यदि (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    यदि (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		जारी;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    यदि ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		जारी;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    यदि ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		जारी;

	    यदि ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		जारी;

	    round = 2;
	    यदि (p->indir != PV_NONE)
	    {
		यदि (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    यदि (!(opt_flags & OPT_LOCAL))
			जारी;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    यदि (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			यदि (!optval_default(p, varp_fresh, p_cp))
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
	    लागि ( ; round <= 2; varp = varp_local, ++round)
	    {
		यदि (round == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		अन्यथा
		    cmd = "setlocal";

		यदि (p->flags & P_BOOL)
		{
		    यदि (put_setbool(fd, cmd, p->fullname, *(पूर्णाङ्क *)varp) == FAIL)
			फर्काऊ FAIL;
		}
		अन्यथा यदि (p->flags & P_अङ्क)
		{
		    यदि (put_setnum(fd, cmd, p->fullname, (दीर्घ *)varp) == FAIL)
			फर्काऊ FAIL;
		}
		अन्यथा    // P_STRING
		{
		    पूर्णाङ्क		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    यदि ((p->flags & P_FUNC) && *(char_u **)varp != रिक्त
			    && म_माला(*(वर्ण **)varp, "<lambda>") != रिक्त)
			जारी;

#ifdef FEAT_EVAL
		    legacy = !is_option_मान_vim9(p - &options[0],
			    round == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#अन्यथा
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#endif

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    यदि (
#यदि defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#endif
			    p->indir == PV_FT)
		    {
			यदि (ख_लिखो(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    फर्काऊ FAIL;
			do_endif = TRUE;
		    }
		    यदि (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			फर्काऊ FAIL;
		    यदि (do_endif)
		    {
			यदि (put_पङ्क्ति(fd, "endif") == FAIL)
			    फर्काऊ FAIL;
		    }
		}
	    }
	}
    }
    फर्काऊ OK;
}

#यदि defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    पूर्णाङ्क
makefoldset(खाता *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_version < SCRIPT_VERSION_VIM9;
# endif

    यदि (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	फर्काऊ FAIL;

    फर्काऊ OK;
}
#endif

    स्थिर पूर्णाङ्क
put_setstring(
    खाता	*fd,
    bool	legacy,
    वर्ण	*cmd,
    वर्ण	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = रिक्त;
    char_u	*part = रिक्त;
    char_u	*p;

    यदि (legacy && ख_लिखो(fd, "legacy ") < 0)
	फर्काऊ FAIL;
    यदि (ख_लिखो(fd, "%s %s=", cmd, name) < 0)
	फर्काऊ FAIL;
    यदि (*valuep != रिक्त)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	यदि (valuep == &p_pt)
	{
	    s = *valuep;
	    जबसम्म (*s != NUL)
		यदि (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    फर्काऊ FAIL;
	}
	// expand the option value, replace $HOME by ~
	अन्यथा यदि ((flags & P_EXPAND) != 0)
	{
	    पूर्णाङ्क  size = (पूर्णाङ्क)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(size);
	    यदि (buf == रिक्त)
		गच्छ fail;
	    home_replace(रिक्त, *valuep, buf, size, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    यदि (size >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != रिक्त)
	    {
		part = alloc(size);
		यदि (part == रिक्त)
		    गच्छ fail;

		// write line break to clear the option, e.g. ':set rtp='
		यदि (put_eol(fd) == FAIL)
		    गच्छ fail;

		p = buf;
		जबसम्म (*p != NUL)
		{
		    यदि (legacy && ख_लिखो(fd, "legacy ") < 0)
			फर्काऊ FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    यदि (ख_लिखो(fd, "%s %s+=", cmd, name) < 0)
			गच्छ fail;
		    (शून्य)copy_option_part(&p, part, size,  ",");
		    यदि (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			गच्छ fail;
		}
		vim_free(buf);
		vim_free(part);
		फर्काऊ OK;
	    }
	    यदि (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		फर्काऊ FAIL;
	    }
	    vim_free(buf);
	}
	अन्यथा यदि (put_escstr(fd, *valuep, 2) == FAIL)
	    फर्काऊ FAIL;
    }
    यदि (put_eol(fd) < 0)
	फर्काऊ FAIL;
    फर्काऊ OK;
fail:
    vim_free(buf);
    vim_free(part);
    फर्काऊ FAIL;
}

    स्थिर पूर्णाङ्क
put_setnum(
    खाता	*fd,
    वर्ण	*cmd,
    वर्ण	*name,
    दीर्घ	*valuep)
{
    दीर्घ	wc;

    यदि (ख_लिखो(fd, "%s %s=", cmd, name) < 0)
	फर्काऊ FAIL;
    यदि (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	यदि (ख_माला_दो((वर्ण *)get_special_साँचो_name((पूर्णाङ्क)wc, 0), fd) < 0)
	    फर्काऊ FAIL;
    }
    अन्यथा यदि (ख_लिखो(fd, "%ld", *valuep) < 0)
	फर्काऊ FAIL;
    यदि (put_eol(fd) < 0)
	फर्काऊ FAIL;
    फर्काऊ OK;
}

    स्थिर पूर्णाङ्क
put_setbool(
    खाता	*fd,
    वर्ण	*cmd,
    वर्ण	*name,
    पूर्णाङ्क		मान)
{
    यदि (मान < 0)	// global/local option using global value
	फर्काऊ OK;
    यदि (ख_लिखो(fd, "%s %s%s", cmd, मान ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	फर्काऊ FAIL;
    फर्काऊ OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    शून्य
clear_termoptions(शून्य)
{
    // Reset a few things before clearing the old options. This may cause
    // outputting a few things that the terminal doesn't understand, but the
    // screen will be cleared later, so this is OK.
    mch_setmouse(FALSE);	    // switch mouse off
    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window titles
#यदि defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    यदि (gui.starting)
	clear_xterm_clip();
#endif
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    शून्य
free_termoptions(शून्य)
{
    संरचना vimoption   *p;

    लागि (p = options; p->fullname != रिक्त; p++)
	यदि (istermoption(p))
	{
	    यदि (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    यदि (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = empty_option;
	    p->def_val[VI_DEFAULT] = empty_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ifdef FEAT_EVAL
	    // remember where the option was cleared
	    set_option_sctx_idx((पूर्णाङ्क)(p - options), OPT_GLOBAL, current_sctx);
#endif
	}
    clear_termcodes();
}

/*
 * Free the string for one term option, if it was allocated.
 * Set the string to empty_option and clear allocated flag.
 * "var" points to the option value.
 */
    शून्य
free_one_termoption(char_u *var)
{
    संरचना vimoption   *p;

    लागि (p = &options[0]; p->fullname != रिक्त; p++)
	यदि (p->var == var)
	{
	    यदि (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = empty_option;
	    p->flags &= ~P_ALLOCED;
	    रोक;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    शून्य
set_term_defaults(शून्य)
{
    संरचना vimoption   *p;

    लागि (p = &options[0]; p->fullname != रिक्त; p++)
    {
	यदि (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    यदि (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    यदि (p->flags & P_ALLOCED)
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
    स्थिर पूर्णाङ्क
istermoption(संरचना vimoption *p)
{
    फर्काऊ (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    स्थिर पूर्णाङ्क
istermoption_idx(पूर्णाङ्क opt_idx)
{
    फर्काऊ istermoption(&options[opt_idx]);
}

#यदि defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    शून्य
unset_global_local_option(char_u *name, शून्य *from)
{
    संरचना vimoption *p;
    पूर्णाङ्क		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    यदि (opt_idx < 0)
	फर्काऊ;
    p = &(options[opt_idx]);

    विकल्प ((पूर्णाङ्क)p->indir)
    {
	// global option with local value: use local value if it's been set
	पक्ष PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    रोक;
	पक्ष PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    रोक;
	पक्ष PV_PATH:
	    clear_string_option(&buf->b_p_path);
	    रोक;
	पक्ष PV_AC:
	    buf->b_p_ac = -1;
	    रोक;
	पक्ष PV_AR:
	    buf->b_p_ar = -1;
	    रोक;
# ifdef HAVE_FSYNC
	पक्ष PV_FS:
	    buf->b_p_fs = -1;
	    रोक;
# endif
	पक्ष PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    रोक;
	पक्ष PV_TAGS:
	    clear_string_option(&buf->b_p_tags);
	    रोक;
	पक्ष PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    रोक;
	पक्ष PV_SISO:
	    curwin->w_p_siso = -1;
	    रोक;
	पक्ष PV_SO:
	    curwin->w_p_so = -1;
	    रोक;
	पक्ष PV_SOP:
	    curwin->w_p_sop = -1;
	    रोक;
# ifdef FEAT_FIND_ID
	पक्ष PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    रोक;
	पक्ष PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    रोक;
# endif
	पक्ष PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    रोक;
	पक्ष PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    रोक;
# ifdef FEAT_DIFF
	पक्ष PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    रोक;
# endif
	पक्ष PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    रोक;
# ifdef FEAT_COMPL_FUNC
	पक्ष PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    रोक;
# endif
	पक्ष PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    रोक;
# ifdef FEAT_EVAL
	पक्ष PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    रोक;
# endif
# ifdef FEAT_QUICKFIX
	पक्ष PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    रोक;
	पक्ष PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    रोक;
	पक्ष PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    रोक;
	पक्ष PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    रोक;
# endif
# यदि defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	पक्ष PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    रोक;
# endif
# यदि defined(FEAT_CRYPT)
	पक्ष PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    रोक;
# endif
# ifdef FEAT_LINEBREAK
	पक्ष PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    रोक;
# endif
# ifdef FEAT_STL_OPT
	पक्ष PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    रोक;
	पक्ष PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    रोक;
# endif
	पक्ष PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    रोक;
	पक्ष PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    रोक;
	पक्ष PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    रोक;
	पक्ष PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    set_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      रिक्त, 0);
	    redraw_later(UPD_NOT_VALID);
	    रोक;
	पक्ष PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    set_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      रिक्त, 0);
	    redraw_later(UPD_NOT_VALID);
	    रोक;
	पक्ष PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    रोक;
    }
}
#endif

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    स्थिर char_u *
get_varp_scope(संरचना vimoption *p, पूर्णाङ्क scope)
{
    यदि ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	यदि (p->var == VAR_WIN)
	    फर्काऊ (char_u *)GLOBAL_WO(get_varp(p));
	फर्काऊ p->var;
    }
    यदि ((scope & OPT_LOCAL) && ((पूर्णाङ्क)p->indir & PV_BOTH))
    {
	विकल्प ((पूर्णाङ्क)p->indir)
	{
	    पक्ष PV_FP:   फर्काऊ (char_u *)&(curbuf->b_p_fp);
#ifdef HAVE_FSYNC
	    पक्ष PV_FS:	फर्काऊ (char_u *)&(curbuf->b_p_fs);
#endif
#ifdef FEAT_EVAL
	    पक्ष PV_FFU: फर्काऊ (char_u *)&(curbuf->b_p_ffu);
#endif
#ifdef FEAT_QUICKFIX
	    पक्ष PV_EFM:  फर्काऊ (char_u *)&(curbuf->b_p_efm);
	    पक्ष PV_GEFM: फर्काऊ (char_u *)&(curbuf->b_p_gefm);
	    पक्ष PV_GP:   फर्काऊ (char_u *)&(curbuf->b_p_gp);
	    पक्ष PV_MP:   फर्काऊ (char_u *)&(curbuf->b_p_mp);
#endif
	    पक्ष PV_EP:   फर्काऊ (char_u *)&(curbuf->b_p_ep);
	    पक्ष PV_KP:   फर्काऊ (char_u *)&(curbuf->b_p_kp);
	    पक्ष PV_PATH: फर्काऊ (char_u *)&(curbuf->b_p_path);
	    पक्ष PV_AC:   फर्काऊ (char_u *)&(curbuf->b_p_ac);
	    पक्ष PV_AR:   फर्काऊ (char_u *)&(curbuf->b_p_ar);
	    पक्ष PV_TAGS: फर्काऊ (char_u *)&(curbuf->b_p_tags);
	    पक्ष PV_TC:   फर्काऊ (char_u *)&(curbuf->b_p_tc);
	    पक्ष PV_SISO: फर्काऊ (char_u *)&(curwin->w_p_siso);
	    पक्ष PV_SO:   फर्काऊ (char_u *)&(curwin->w_p_so);
	    पक्ष PV_SOP:  फर्काऊ (char_u *)&(curwin->w_p_sop);
#ifdef FEAT_FIND_ID
	    पक्ष PV_DEF:  फर्काऊ (char_u *)&(curbuf->b_p_def);
	    पक्ष PV_INC:  फर्काऊ (char_u *)&(curbuf->b_p_inc);
#endif
	    पक्ष PV_COT:  फर्काऊ (char_u *)&(curbuf->b_p_cot);
	    पक्ष PV_DICT: फर्काऊ (char_u *)&(curbuf->b_p_dict);
#ifdef FEAT_DIFF
	    पक्ष PV_DIA:  फर्काऊ (char_u *)&(curbuf->b_p_dia);
#endif
	    पक्ष PV_TSR:  फर्काऊ (char_u *)&(curbuf->b_p_tsr);
#ifdef FEAT_COMPL_FUNC
	    पक्ष PV_TSRFU: फर्काऊ (char_u *)&(curbuf->b_p_tsrfu);
#endif
#यदि defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    पक्ष PV_BEXPR: फर्काऊ (char_u *)&(curbuf->b_p_bexpr);
#endif
#यदि defined(FEAT_CRYPT)
	    पक्ष PV_CM:	  फर्काऊ (char_u *)&(curbuf->b_p_cm);
#endif
#ifdef FEAT_LINEBREAK
	    पक्ष PV_SBR:  फर्काऊ (char_u *)&(curwin->w_p_sbr);
#endif
#ifdef FEAT_STL_OPT
	    पक्ष PV_STL:  फर्काऊ (char_u *)&(curwin->w_p_stl);
	    पक्ष PV_STLO: फर्काऊ (char_u *)&(curwin->w_p_stlo);
#endif
	    पक्ष PV_UL:   फर्काऊ (char_u *)&(curbuf->b_p_ul);
	    पक्ष PV_LW:   फर्काऊ (char_u *)&(curbuf->b_p_lw);
	    पक्ष PV_BKC:  फर्काऊ (char_u *)&(curbuf->b_p_bkc);
	    पक्ष PV_MENC: फर्काऊ (char_u *)&(curbuf->b_p_menc);
	    पक्ष PV_LCS:  फर्काऊ (char_u *)&(curwin->w_p_lcs);
	    पक्ष PV_FCS:  फर्काऊ (char_u *)&(curwin->w_p_fcs);
	    पक्ष PV_VE:	  फर्काऊ (char_u *)&(curwin->w_p_ve);

	}
	फर्काऊ रिक्त; // "cannot happen"
    }
    फर्काऊ get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(पूर्णाङ्क opt_idx, पूर्णाङ्क scope)
{
    फर्काऊ get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    स्थिर char_u *
get_varp(संरचना vimoption *p)
{
    // hidden option, always return NULL
    यदि (p->var == रिक्त)
	फर्काऊ रिक्त;

    विकल्प ((पूर्णाङ्क)p->indir)
    {
	पक्ष PV_NONE:	फर्काऊ p->var;

	// global option with local value: use local value if it's been set
	पक्ष PV_EP:	फर्काऊ *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	पक्ष PV_KP:	फर्काऊ *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	पक्ष PV_PATH:	फर्काऊ *curbuf->b_p_path != NUL
				    ? (char_u *)&(curbuf->b_p_path) : p->var;
	पक्ष PV_AC:	फर्काऊ curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	पक्ष PV_AR:	फर्काऊ curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	पक्ष PV_TAGS:	फर्काऊ *curbuf->b_p_tags != NUL
				    ? (char_u *)&(curbuf->b_p_tags) : p->var;
	पक्ष PV_TC:	फर्काऊ *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	पक्ष PV_BKC:	फर्काऊ *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	पक्ष PV_SISO:	फर्काऊ curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	पक्ष PV_SO:	फर्काऊ curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	पक्ष PV_SOP:	फर्काऊ curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ifdef FEAT_FIND_ID
	पक्ष PV_DEF:	फर्काऊ *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	पक्ष PV_INC:	फर्काऊ *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#endif
	पक्ष PV_COT:	फर्काऊ *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	पक्ष PV_DICT:	फर्काऊ *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ifdef FEAT_DIFF
	पक्ष PV_DIA:	फर्काऊ *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#endif
	पक्ष PV_TSR:	फर्काऊ *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ifdef FEAT_COMPL_FUNC
	पक्ष PV_TSRFU:	फर्काऊ *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#endif
	पक्ष PV_FP:	फर्काऊ *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ifdef HAVE_FSYNC
	पक्ष PV_FS:	फर्काऊ curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#endif
#ifdef FEAT_EVAL
	पक्ष PV_FFU:	फर्काऊ *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#endif
#ifdef FEAT_QUICKFIX
	पक्ष PV_EFM:	फर्काऊ *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	पक्ष PV_GEFM:	फर्काऊ *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	पक्ष PV_GP:	फर्काऊ *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	पक्ष PV_MP:	फर्काऊ *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#endif
#यदि defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	पक्ष PV_BEXPR:	फर्काऊ *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#endif
#यदि defined(FEAT_CRYPT)
	पक्ष PV_CM:	फर्काऊ *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#endif
#ifdef FEAT_LINEBREAK
	पक्ष PV_SBR:	फर्काऊ *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#endif
#ifdef FEAT_STL_OPT
	पक्ष PV_STL:	फर्काऊ *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	पक्ष PV_STLO:	फर्काऊ *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#endif
	पक्ष PV_UL:	फर्काऊ curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	पक्ष PV_LW:	फर्काऊ *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	पक्ष PV_MENC:	फर्काऊ *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ifdef FEAT_ARABIC
	पक्ष PV_ARAB:	फर्काऊ (char_u *)&(curwin->w_p_arab);
#endif
	पक्ष PV_LIST:	फर्काऊ (char_u *)&(curwin->w_p_list);
	पक्ष PV_LCS:	फर्काऊ *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	पक्ष PV_FCS:	फर्काऊ *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	पक्ष PV_VE:	फर्काऊ *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ifdef FEAT_SPELL
	पक्ष PV_SPELL:	फर्काऊ (char_u *)&(curwin->w_p_spell);
#endif
#ifdef FEAT_SYN_HL
	पक्ष PV_CUC:	फर्काऊ (char_u *)&(curwin->w_p_cuc);
	पक्ष PV_CUL:	फर्काऊ (char_u *)&(curwin->w_p_cul);
	पक्ष PV_CULOPT:	फर्काऊ (char_u *)&(curwin->w_p_culopt);
	पक्ष PV_CC:	फर्काऊ (char_u *)&(curwin->w_p_cc);
#endif
#ifdef FEAT_DIFF
	पक्ष PV_DIFF:	फर्काऊ (char_u *)&(curwin->w_p_diff);
#endif
	पक्ष PV_EIW:	फर्काऊ (char_u *)&(curwin->w_p_eiw);
#ifdef FEAT_FOLDING
	पक्ष PV_FDC:	फर्काऊ (char_u *)&(curwin->w_p_fdc);
	पक्ष PV_FEN:	फर्काऊ (char_u *)&(curwin->w_p_fen);
	पक्ष PV_FDI:	फर्काऊ (char_u *)&(curwin->w_p_fdi);
	पक्ष PV_FDL:	फर्काऊ (char_u *)&(curwin->w_p_fdl);
	पक्ष PV_FDM:	फर्काऊ (char_u *)&(curwin->w_p_fdm);
	पक्ष PV_FML:	फर्काऊ (char_u *)&(curwin->w_p_fml);
	पक्ष PV_FDN:	फर्काऊ (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	पक्ष PV_FDE:	फर्काऊ (char_u *)&(curwin->w_p_fde);
	पक्ष PV_FDT:	फर्काऊ (char_u *)&(curwin->w_p_fdt);
# endif
	पक्ष PV_FMR:	फर्काऊ (char_u *)&(curwin->w_p_fmr);
#endif
	पक्ष PV_NU:	फर्काऊ (char_u *)&(curwin->w_p_nu);
	पक्ष PV_RNU:	फर्काऊ (char_u *)&(curwin->w_p_rnu);
#ifdef FEAT_LINEBREAK
	पक्ष PV_NUW:	फर्काऊ (char_u *)&(curwin->w_p_nuw);
#endif
	पक्ष PV_WFB:	फर्काऊ (char_u *)&(curwin->w_p_wfb);
	पक्ष PV_WFH:	फर्काऊ (char_u *)&(curwin->w_p_wfh);
	पक्ष PV_WFW:	फर्काऊ (char_u *)&(curwin->w_p_wfw);
#यदि defined(FEAT_QUICKFIX)
	पक्ष PV_PVW:	फर्काऊ (char_u *)&(curwin->w_p_pvw);
	पक्ष PV_LHI:	फर्काऊ (char_u *)&(curwin->w_p_lhi);
#endif
#ifdef FEAT_RIGHTLEFT
	पक्ष PV_RL:	फर्काऊ (char_u *)&(curwin->w_p_rl);
	पक्ष PV_RLC:	फर्काऊ (char_u *)&(curwin->w_p_rlc);
#endif
	पक्ष PV_SCROLL:	फर्काऊ (char_u *)&(curwin->w_p_scr);
	पक्ष PV_SMS:	फर्काऊ (char_u *)&(curwin->w_p_sms);
	पक्ष PV_WRAP:	फर्काऊ (char_u *)&(curwin->w_p_wrap);
#ifdef FEAT_LINEBREAK
	पक्ष PV_LBR:	फर्काऊ (char_u *)&(curwin->w_p_lbr);
	पक्ष PV_BRI:	फर्काऊ (char_u *)&(curwin->w_p_bri);
	पक्ष PV_BRIOPT: फर्काऊ (char_u *)&(curwin->w_p_briopt);
#endif
	पक्ष PV_WCR:	फर्काऊ (char_u *)&(curwin->w_p_wcr);
	पक्ष PV_SCBIND: फर्काऊ (char_u *)&(curwin->w_p_scb);
	पक्ष PV_CRBIND: फर्काऊ (char_u *)&(curwin->w_p_crb);
#ifdef FEAT_CONCEAL
	पक्ष PV_COCU:   फर्काऊ (char_u *)&(curwin->w_p_cocu);
	पक्ष PV_COLE:   फर्काऊ (char_u *)&(curwin->w_p_cole);
#endif
	पक्ष PV_WHL:	फर्काऊ (char_u *)&(curwin->w_p_whl);

#ifdef FEAT_TERMINAL
	पक्ष PV_TWK:    फर्काऊ (char_u *)&(curwin->w_p_twk);
	पक्ष PV_TWS:    फर्काऊ (char_u *)&(curwin->w_p_tws);
	पक्ष PV_TWSL:	फर्काऊ (char_u *)&(curbuf->b_p_twsl);
#endif

	पक्ष PV_AI:	फर्काऊ (char_u *)&(curbuf->b_p_ai);
	पक्ष PV_BIN:	फर्काऊ (char_u *)&(curbuf->b_p_bin);
	पक्ष PV_BOMB:	फर्काऊ (char_u *)&(curbuf->b_p_bomb);
	पक्ष PV_BH:	फर्काऊ (char_u *)&(curbuf->b_p_bh);
	पक्ष PV_BT:	फर्काऊ (char_u *)&(curbuf->b_p_bt);
	पक्ष PV_BL:	फर्काऊ (char_u *)&(curbuf->b_p_bl);
	पक्ष PV_CI:	फर्काऊ (char_u *)&(curbuf->b_p_ci);
	पक्ष PV_CIN:	फर्काऊ (char_u *)&(curbuf->b_p_cin);
	पक्ष PV_CINK:	फर्काऊ (char_u *)&(curbuf->b_p_cink);
	पक्ष PV_CINO:	फर्काऊ (char_u *)&(curbuf->b_p_cino);
	पक्ष PV_CINSD:	फर्काऊ (char_u *)&(curbuf->b_p_cinsd);
	पक्ष PV_CINW:	फर्काऊ (char_u *)&(curbuf->b_p_cinw);
	पक्ष PV_COM:	फर्काऊ (char_u *)&(curbuf->b_p_com);
	पक्ष PV_CMS:	फर्काऊ (char_u *)&(curbuf->b_p_cms);
	पक्ष PV_CPT:	फर्काऊ (char_u *)&(curbuf->b_p_cpt);
#ifdef BACKSLASH_IN_FILENAME
	पक्ष PV_CSL:	फर्काऊ (char_u *)&(curbuf->b_p_csl);
#endif
#ifdef FEAT_COMPL_FUNC
	पक्ष PV_CFU:	फर्काऊ (char_u *)&(curbuf->b_p_cfu);
	पक्ष PV_OFU:	फर्काऊ (char_u *)&(curbuf->b_p_ofu);
#endif
#ifdef FEAT_EVAL
	पक्ष PV_TFU:	फर्काऊ (char_u *)&(curbuf->b_p_tfu);
#endif
	पक्ष PV_EOF:	फर्काऊ (char_u *)&(curbuf->b_p_eof);
	पक्ष PV_EOL:	फर्काऊ (char_u *)&(curbuf->b_p_eol);
	पक्ष PV_FIXEOL:	फर्काऊ (char_u *)&(curbuf->b_p_fixeol);
	पक्ष PV_ET:	फर्काऊ (char_u *)&(curbuf->b_p_et);
	पक्ष PV_FENC:	फर्काऊ (char_u *)&(curbuf->b_p_fenc);
	पक्ष PV_FF:	फर्काऊ (char_u *)&(curbuf->b_p_ff);
	पक्ष PV_FT:	फर्काऊ (char_u *)&(curbuf->b_p_ft);
	पक्ष PV_FO:	फर्काऊ (char_u *)&(curbuf->b_p_fo);
	पक्ष PV_FLP:	फर्काऊ (char_u *)&(curbuf->b_p_flp);
	पक्ष PV_IMI:	फर्काऊ (char_u *)&(curbuf->b_p_iminsert);
	पक्ष PV_IMS:	फर्काऊ (char_u *)&(curbuf->b_p_imsearch);
	पक्ष PV_INF:	फर्काऊ (char_u *)&(curbuf->b_p_inf);
	पक्ष PV_ISK:	फर्काऊ (char_u *)&(curbuf->b_p_isk);
#ifdef FEAT_FIND_ID
# ifdef FEAT_EVAL
	पक्ष PV_INEX:	फर्काऊ (char_u *)&(curbuf->b_p_inex);
# endif
#endif
#यदि defined(FEAT_EVAL)
	पक्ष PV_INDE:	फर्काऊ (char_u *)&(curbuf->b_p_inde);
	पक्ष PV_INDK:	फर्काऊ (char_u *)&(curbuf->b_p_indk);
#endif
#ifdef FEAT_EVAL
	पक्ष PV_FEX:	फर्काऊ (char_u *)&(curbuf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
	पक्ष PV_साँचो:	फर्काऊ (char_u *)&(curbuf->b_p_साँचो);
#endif
	पक्ष PV_LISP:	फर्काऊ (char_u *)&(curbuf->b_p_lisp);
	पक्ष PV_LOP:	फर्काऊ (char_u *)&(curbuf->b_p_lop);
	पक्ष PV_ML:	फर्काऊ (char_u *)&(curbuf->b_p_ml);
	पक्ष PV_MPS:	फर्काऊ (char_u *)&(curbuf->b_p_mps);
	पक्ष PV_MA:	फर्काऊ (char_u *)&(curbuf->b_p_ma);
	पक्ष PV_MOD:	फर्काऊ (char_u *)&(curbuf->b_changed);
	पक्ष PV_NF:	फर्काऊ (char_u *)&(curbuf->b_p_nf);
	पक्ष PV_PI:	फर्काऊ (char_u *)&(curbuf->b_p_pi);
	पक्ष PV_QE:	फर्काऊ (char_u *)&(curbuf->b_p_qe);
	पक्ष PV_RO:	फर्काऊ (char_u *)&(curbuf->b_p_ro);
	पक्ष PV_SI:	फर्काऊ (char_u *)&(curbuf->b_p_si);
	पक्ष PV_SN:	फर्काऊ (char_u *)&(curbuf->b_p_sn);
	पक्ष PV_STS:	फर्काऊ (char_u *)&(curbuf->b_p_sts);
	पक्ष PV_SUA:	फर्काऊ (char_u *)&(curbuf->b_p_sua);
	पक्ष PV_SWF:	फर्काऊ (char_u *)&(curbuf->b_p_swf);
#ifdef FEAT_SYN_HL
	पक्ष PV_SMC:	फर्काऊ (char_u *)&(curbuf->b_p_smc);
	पक्ष PV_SYN:	फर्काऊ (char_u *)&(curbuf->b_p_syn);
#endif
#ifdef FEAT_SPELL
	पक्ष PV_SPC:	फर्काऊ (char_u *)&(curwin->w_s->b_p_spc);
	पक्ष PV_SPF:	फर्काऊ (char_u *)&(curwin->w_s->b_p_spf);
	पक्ष PV_SPL:	फर्काऊ (char_u *)&(curwin->w_s->b_p_spl);
	पक्ष PV_SPO:	फर्काऊ (char_u *)&(curwin->w_s->b_p_spo);
#endif
	पक्ष PV_SW:	फर्काऊ (char_u *)&(curbuf->b_p_sw);
	पक्ष PV_TS:	फर्काऊ (char_u *)&(curbuf->b_p_ts);
	पक्ष PV_TW:	फर्काऊ (char_u *)&(curbuf->b_p_tw);
	पक्ष PV_TX:	फर्काऊ (char_u *)&(curbuf->b_p_tx);
#ifdef FEAT_PERSISTENT_UNDO
	पक्ष PV_UDF:	फर्काऊ (char_u *)&(curbuf->b_p_udf);
#endif
	पक्ष PV_WM:	फर्काऊ (char_u *)&(curbuf->b_p_wm);
#ifdef FEAT_KEYMAP
	पक्ष PV_KMAP:	फर्काऊ (char_u *)&(curbuf->b_p_keymap);
#endif
#ifdef FEAT_SIGNS
	पक्ष PV_SCL:	फर्काऊ (char_u *)&(curwin->w_p_scl);
#endif
#ifdef FEAT_VARTABS
	पक्ष PV_VSTS:	फर्काऊ (char_u *)&(curbuf->b_p_vsts);
	पक्ष PV_VTS:	फर्काऊ (char_u *)&(curbuf->b_p_vts);
#endif
	स्वभाव:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    फर्काऊ (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(पूर्णाङ्क opt_idx)
{
    फर्काऊ options[opt_idx].var;
}

#यदि defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(पूर्णाङ्क opt_idx)
{
    फर्काऊ (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    पूर्णाङ्क
option_info(char_u *name, dict_T *d)
{
    पूर्णाङ्क			opt_idx;
    संरचना vimoption	*p;
    पूर्णाङ्क			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    यदि ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    यदि (opt_idx < 0)
	फर्काऊ FAIL;

    p = &options[opt_idx];
    dict_add_string(d, "name", (char_u *)p->fullname);
    dict_add_string(d, "shortname",
		    (char_u *)(p->shortname == रिक्त ? "" : p->shortname));
    dict_add_bool(d, "available", p->var != रिक्त);
    dict_add_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_अङ्क) ? "number" : "string"));
    dict_add_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    यदि (p->flags & P_BOOL)
	dict_add_bool(d, "default", (पूर्णाङ्क)(long_i)p->def_val[dvi]);
    अन्यथा यदि (p->flags & P_अङ्क)
	dict_add_सङ्ख्या(d, "default", (दीर्घ)(long_i)p->def_val[dvi]);
    अन्यथा
	dict_add_string(d, "default", p->def_val[dvi] == रिक्त
					? (char_u *)"" : p->def_val[dvi]);
    फर्काऊ OK;
}
#endif

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_set_cb_T
get_option_did_set_cb(पूर्णाङ्क opt_idx)
{
    फर्काऊ options[opt_idx].opt_did_set_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(शून्य)
{
    यदि (*curbuf->b_p_ep == NUL)
	फर्काऊ p_ep;
    फर्काऊ curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(शून्य)
{
#ifdef FEAT_EVAL
    यदि (*curbuf->b_p_ffu == NUL)
	फर्काऊ p_ffu;
    फर्काऊ curbuf->b_p_ffu;
#अन्यथा
    फर्काऊ (char_u *)"";
#endif
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    शून्य
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
    शून्य
after_copy_winopt(win_T *wp)
{
    वर्ण *errmsg = update_winhighlight(wp, wp->w_p_whl);

    यदि (errmsg != रिक्त)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    यदि (wp->w_p_wrap)
	wp->w_leftcol = 0;
    अन्यथा
	wp->w_skipcol = 0;
#ifdef FEAT_LINEBREAK
    briopt_check(रिक्त, wp);
#endif
#ifdef FEAT_SYN_HL
    fill_culopt_flags(रिक्त, wp);
    check_colorcolumn(रिक्त, wp);
#endif
    set_listchars_option(wp, wp->w_p_lcs, TRUE, रिक्त, 0);
    set_fillchars_option(wp, wp->w_p_fcs, TRUE, रिक्त, 0);
}

    स्थिर char_u *
copy_option_val(char_u *val)
{
    यदि (val == empty_option)
	फर्काऊ empty_option;  // no need to allocate memory
    फर्काऊ vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    शून्य
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
						    परिमाण(to->wo_script_ctx));
#endif
    check_winopt(to);		// don't want NULL pointers
}

/*
 * Check string options in a window for a NULL value.
 */
    स्थिर शून्य
check_win_options(win_T *win)
{
    check_winopt(&win->w_onebuf_opt);
    check_winopt(&win->w_allbuf_opt);
}

/*
 * Check for NULL pointers in a winopt_T and replace them with empty_option.
 */
    स्थिर शून्य
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
    शून्य
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
स्थिर पूर्णाङ्क buf_opt_idx[BV_गन्ती];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    स्थिर शून्य
init_buf_opt_idx(शून्य)
{
    स्थिर पूर्णाङ्क did_init_buf_opt_idx = FALSE;
    पूर्णाङ्क i;

    यदि (did_init_buf_opt_idx)
	फर्काऊ;
    did_init_buf_opt_idx = TRUE;
    लागि (i = 0; !istermoption_idx(i); i++)
	यदि (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_MASK] = i;
}
#अन्यथा
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
    शून्य
buf_copy_options(buf_T *buf, पूर्णाङ्क flags)
{
    पूर्णाङ्क		should_copy = TRUE;
    char_u	*save_p_isk = रिक्त;	    // init for GCC
    पूर्णाङ्क		dont_do_help;
    पूर्णाङ्क		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    यदि (p_cpo != रिक्त)
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
	यदि ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == रिक्त || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != रिक्त)))
	    should_copy = FALSE;

	यदि (should_copy || (flags & BCO_ALWAYS))
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
	    यदि (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = रिक्त;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    यदि (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		विकल्प (*p_ffs)
		{
		    पक्ष 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); रोक;
		    पक्ष 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); रोक;
		    पक्ष 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); रोक;
		    स्वभाव:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		यदि (buf->b_p_ff != रिक्त)
		    buf->b_start_ffc = *buf->b_p_ff;
		buf->b_p_bh = empty_option;
		buf->b_p_bt = empty_option;
	    }
	    अन्यथा
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
	    यदि (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    अन्यथा
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
	    यदि (p_vsts && p_vsts != empty_option)
		(शून्य)tabstop_set(p_vsts, &buf->b_p_vsts_array);
	    अन्यथा
		buf->b_p_vsts_array = रिक्त;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : रिक्त;
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
	    (शून्य)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#endif
#यदि defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#endif
	    buf->b_p_fp = empty_option;
#यदि defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#endif
#ifdef FEAT_CRYPT
	    buf->b_p_साँचो = vim_strsave(p_साँचो);
	    COPY_OPT_SCTX(buf, BV_साँचो);
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
#यदि defined(FEAT_EVAL)
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
#यदि defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = empty_option;
#endif
#यदि defined(FEAT_CRYPT)
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
	    यदि (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ifdef FEAT_VARTABS
		यदि (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (शून्य)tabstop_set(p_vts, &buf->b_p_vts_array);
		अन्यथा
		    buf->b_p_vts_array = रिक्त;
#endif
	    }
	    अन्यथा
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#ifdef FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		यदि (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (शून्य)tabstop_set(p_vts, &buf->b_p_vts_array);
		अन्यथा
		    buf->b_p_vts_array = रिक्त;
#endif
		buf->b_help = false;
		यदि (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	यदि (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    यदि (did_isk)
	(शून्य)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    शून्य
reset_modifiable(शून्य)
{
    पूर्णाङ्क		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    यदि (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}

/*
 * Set the global value for 'iminsert' to the local value.
 */
    शून्य
set_iminsert_global(शून्य)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    शून्य
set_imsearch_global(शून्य)
{
    p_imsearch = curbuf->b_p_imsearch;
}

स्थिर पूर्णाङ्क expand_option_idx = -1;
स्थिर पूर्णाङ्क expand_option_start_col = 0;
स्थिर char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
स्थिर पूर्णाङ्क expand_option_flags = 0;
स्थिर पूर्णाङ्क expand_option_append = FALSE;

    शून्य
set_context_in_set_cmd(
    expand_T	*xp,
    char_u	*arg,
    पूर्णाङ्क		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    पूर्णाङ्क		nextchar;
    long_u	flags = 0;	// init for GCC
    पूर्णाङ्क		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    पूर्णाङ्क		is_term_option = FALSE;
    पूर्णाङ्क		साँचो;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    यदि (*arg == NUL)
    {
	xp->xp_pattern = arg;
	फर्काऊ;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    यदि (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_pattern = p + 1;
	फर्काऊ;
    }
    जबसम्म (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	यदि (*p == ' ' || *p == ',')
	{
	    जबसम्म (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	यदि (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    रोक;
	}
	--p;
    }
    यदि (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_NO;
	p += 2;
    }
    अन्यथा यदि (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_INV;
	p += 3;
    }
    xp->xp_pattern = arg = p;
    यदि (*arg == '<')
    {
	जबसम्म (*p != '>')
	    यदि (*p++ == NUL)	    // expand terminal option name
		फर्काऊ;
	साँचो = get_special_साँचो_code(arg + 1);
	यदि (साँचो == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    फर्काऊ;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(साँचो);
	expand_option_name[3] = KEY2TERMCAP1(साँचो);
    }
    अन्यथा
    {
	यदि (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    यदि (*p != NUL)
		++p;
	    यदि (*p == NUL)
		फर्काऊ;		// expand option name
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	अन्यथा
	{
	    // Allow * wildcard
	    जबसम्म (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    यदि (*p == NUL)
		फर्काऊ;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    यदि (opt_idx == -1 || options[opt_idx].var == रिक्त)
	    {
		xp->xp_context = EXPAND_NOTHING;
		फर्काऊ;
	    }
	    flags = options[opt_idx].flags;
	    यदि (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		फर्काऊ;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    पूर्णाङ्क expand_option_subtract = FALSE;
    यदि ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	यदि (nextchar == '-')
	    expand_option_subtract = TRUE;
	यदि (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    यदि ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	फर्काऊ;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    यदि (is_term_option)
	expand_option_idx = -1;
    अन्यथा
	expand_option_idx = opt_idx;

    यदि (!is_term_option)
    {
	यदि (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    फर्काऊ;
	}
    }

    xp->xp_pattern = p + 1;
    expand_option_start_col = (पूर्णाङ्क)(p + 1 - xp->xp_पङ्क्ति);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ifdef FEAT_SYN_HL
    यदि (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	फर्काऊ;
    }
#endif
    यदि (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	फर्काऊ;
    }
#ifdef FEAT_KEYMAP
    यदि (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	फर्काऊ;
    }
#endif

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    यदि (expand_option_subtract)
    {
	xp->xp_context = EXPAND_SETTING_SUBTRACT;
	फर्काऊ;
    }
    अन्यथा यदि (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != रिक्त)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    अन्यथा यदि (*xp->xp_pattern == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	फर्काऊ;
    }
    अन्यथा
	xp->xp_context = EXPAND_NOTHING;

    यदि (is_term_option || (flags & P_अङ्क))
	फर्काऊ;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    यदि (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	यदि (p == (char_u *)&p_bdir
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
	    यदि (p == (char_u *)&p_path || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    अन्यथा
		xp->xp_backslash = XP_BS_ONE;
	}
	अन्यथा
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    यदि (p == (char_u *)&p_tags)
		xp->xp_backslash = XP_BS_THREE;
	    अन्यथा
		xp->xp_backslash = XP_BS_ONE;
	}
	यदि (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    यदि ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	लागि (p = argend - 1; p >= xp->xp_pattern; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    यदि (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		जबसम्म (s > xp->xp_pattern && *(s - 1) == '\\')
		    --s;
		यदि ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#यदि defined(BACKSLASH_IN_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#अन्यथा
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#endif
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_pattern = p + 1;
		    रोक;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    यदि (flags & P_FLAGLIST)
	xp->xp_pattern = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ifdef FEAT_SPELL
    यदि (options[opt_idx].var == (char_u *)&p_sps)
    {
	यदि (STRNCMP(xp->xp_pattern, "file:", 5) == 0)
	{
	    xp->xp_pattern += 5;
	    फर्काऊ;
	}
	अन्यथा यदि (options[expand_option_idx].opt_expand_cb != रिक्त)
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
    स्थिर पूर्णाङ्क
match_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	पूर्णाङ्क		idx,
	पूर्णाङ्क		test_only,
	पूर्णाङ्क		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    यदि (!fuzzy)
    {
	यदि (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    यदि (!test_only)
		matches[idx] = vim_strsave(str);
	    फर्काऊ TRUE;
	}
    }
    अन्यथा
    {
	पूर्णाङ्क score;

	score = fuzzy_match_str(str, fuzzystr);
	यदि (score != FUZZY_SCORE_NONE)
	{
	    यदि (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    फर्काऊ TRUE;
	}
    }

    फर्काऊ FALSE;
}

    पूर्णाङ्क
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    पूर्णाङ्क		*अङ्क_Matches,
    char_u	***matches,
    पूर्णाङ्क		can_fuzzy)
{
    पूर्णाङ्क		अङ्क_normal = 0;	    // Nr of matching non-term-code settings
    पूर्णाङ्क		अङ्क_term = 0;	    // Nr of matching terminal code settings
    पूर्णाङ्क		opt_idx;
    पूर्णाङ्क		match;
    पूर्णाङ्क		गन्ती = 0;
    char_u	*str;
    पूर्णाङ्क		loop;
    पूर्णाङ्क		is_term_opt;
    char_u	name_buf[MAX_साँचो_NAME_LEN];
    स्थिर वर्ण *(names[]) = {"all", "termcap"};
    पूर्णाङ्क		ic = regmatch->rm_ic;	// remember the ignore-case flag
    पूर्णाङ्क		fuzzy;
    fuzmatch_str_T  *fuzmatch = रिक्त;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    लागि (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	यदि (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    लागि (match = 0; match < (पूर्णाङ्क)ARRAY_LENGTH(names); ++match)
	    {
		यदि (match_str((char_u *)names[match], regmatch, *matches,
			    गन्ती, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    यदि (loop == 0)
			अङ्क_normal++;
		    अन्यथा
			गन्ती++;
		}
	    }
	}
	लागि (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != रिक्त;
								    opt_idx++)
	{
	    यदि (options[opt_idx].var == रिक्त)
		जारी;
	    यदि (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		जारी;
	    is_term_opt = istermoption_idx(opt_idx);
	    यदि (is_term_opt && अङ्क_normal > 0)
		जारी;

	    यदि (match_str(str, regmatch, *matches, गन्ती, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		यदि (loop == 0)
		{
		    यदि (is_term_opt)
			अङ्क_term++;
		    अन्यथा
			अङ्क_normal++;
		}
		अन्यथा
		    गन्ती++;
	    }
	    अन्यथा यदि (!fuzzy && options[opt_idx].shortname != रिक्त
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		यदि (loop == 0)
		{
		    यदि (is_term_opt)
			अङ्क_term++;
		    अन्यथा
			अङ्क_normal++;
		}
		अन्यथा
		    (*matches)[गन्ती++] = vim_strsave(str);
	    }
	    अन्यथा यदि (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		यदि (match_str(name_buf, regmatch, *matches, गन्ती, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    यदि (loop == 0)
			अङ्क_term++;
		    अन्यथा
			गन्ती++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	यदि (xp->xp_context != EXPAND_BOOL_SETTINGS  && अङ्क_normal == 0)
	{
	    लागि (opt_idx = 0; (str = get_termcode(opt_idx)) != रिक्त; opt_idx++)
	    {
		यदि (!SAFE_isprint(str[0]) || !SAFE_isprint(str[1]))
		    जारी;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		यदि (match_str(name_buf, regmatch, *matches, गन्ती,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    यदि (loop == 0)
			अङ्क_term++;
		    अन्यथा
			गन्ती++;
		}
		अन्यथा
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    यदि (match_str(name_buf, regmatch, *matches, गन्ती,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			यदि (loop == 0)
			    अङ्क_term++;
			अन्यथा
			    गन्ती++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    लागि (opt_idx = 0; (str = get_साँचो_name(opt_idx)) != रिक्त; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		यदि (match_str(name_buf, regmatch, *matches, गन्ती, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    यदि (loop == 0)
			अङ्क_term++;
		    अन्यथा
			गन्ती++;
		}
	    }
	}
	यदि (loop == 0)
	{
	    यदि (अङ्क_normal > 0)
		*अङ्क_Matches = अङ्क_normal;
	    अन्यथा यदि (अङ्क_term > 0)
		*अङ्क_Matches = अङ्क_term;
	    अन्यथा
		फर्काऊ OK;
	    यदि (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *अङ्क_Matches);
		यदि (*matches == रिक्त)
		{
		    *matches = (char_u **)"";
		    फर्काऊ FAIL;
		}
	    }
	    अन्यथा
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *अङ्क_Matches);
		यदि (fuzmatch == रिक्त)
		{
		    *matches = (char_u **)"";
		    फर्काऊ FAIL;
		}
	    }
	}
    }

    यदि (fuzzy &&
	fuzzymatches_to_strmatches(fuzmatch, matches, गन्ती, FALSE) == FAIL)
	फर्काऊ FAIL;

    फर्काऊ OK;
}

// Escape an option value that can be used on the command-line with :set.
// Caller needs to free the returned string, unless NULL is returned.
    स्थिर char_u*
escape_option_str_cmdline(char_u *var)
{
    char_u *buf;

    // A backslash is required before some characters.  This is the reverse of
    // what happens in do_set().
    buf = vim_strsave_escaped(var, escape_chars);
    यदि (buf == रिक्त)
	फर्काऊ रिक्त;

#ifdef BACKSLASH_IN_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    लागि (var = buf; *var != NUL; MB_PTR_ADV(var))
	यदि (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#endif
    फर्काऊ buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    पूर्णाङ्क
ExpandOldSetting(पूर्णाङ्क *अङ्क_Matches, char_u ***matches)
{
    char_u  *var = रिक्त;	// init for GCC
    char_u  *buf;

    *अङ्क_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    यदि (*matches == रिक्त)
	फर्काऊ FAIL;

    // For a terminal key code expand_option_idx is < 0.
    यदि (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	यदि (var == रिक्त)
	    expand_option_idx = findoption(expand_option_name);
    }

    यदि (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    अन्यथा यदि (var == रिक्त)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    यदि (buf == रिक्त)
    {
	VIM_CLEAR(*matches);
	फर्काऊ FAIL;
    }

    (*matches)[0] = buf;
    *अङ्क_Matches = 1;
    फर्काऊ OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    पूर्णाङ्क
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    पूर्णाङ्क		*अङ्क_Matches,
    char_u	***matches)
{
    char_u  *var = रिक्त;	// init for GCC
    char_u  *buf;

    यदि (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == रिक्त)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	फर्काऊ FAIL;
    }

    optexpand_T args;
    args.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    args.oe_append = expand_option_append;
    args.oe_regmatch = regmatch;
    args.oe_xp = xp;
    args.oe_set_arg = xp->xp_पङ्क्ति + expand_option_start_col;
    args.oe_include_orig_val =
	!expand_option_append &&
	(*args.oe_set_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    यदि (buf == रिक्त)
	फर्काऊ FAIL;

    args.oe_opt_मान = buf;

    पूर्णाङ्क अङ्क_ret = options[expand_option_idx].opt_expand_cb(&args, अङ्क_Matches, matches);

    vim_free(buf);
    फर्काऊ अङ्क_ret;
}

/*
 * Expansion handler for :set-=
 */
    पूर्णाङ्क
ExpandSettingSubtract(
    expand_T	*xp,
    regmatch_T	*regmatch,
    पूर्णाङ्क		*अङ्क_Matches,
    char_u	***matches)
{
    यदि (expand_option_idx < 0)
	// term option
	फर्काऊ ExpandOldSetting(अङ्क_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    यदि (option_flags & P_अङ्क)
	फर्काऊ ExpandOldSetting(अङ्क_Matches, matches);
    अन्यथा यदि (option_flags & P_COMMA)
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

	यदि (*option_val == NUL)
	    फर्काऊ FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	यदि (option_copy == रिक्त)
	    फर्काऊ FAIL;
	next_val = option_copy;

	ga_init2(&ga, परिमाण(char_u *), 10);

	गर
	{
	    item = next_val;
	    comma = vim_strchr(next_val, ',');
	    जबसम्म (comma != रिक्त && comma != next_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    यदि (comma != रिक्त)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		next_val = comma + 1;
	    }
	    अन्यथा
		next_val = रिक्त;

	    यदि (*item == NUL)
		// empty value, don't add to list
		जारी;

	    यदि (!vim_regexec(regmatch, item, (colnr_T)0))
		जारी;

	    char_u *buf = escape_option_str_cmdline(item);
	    यदि (buf == रिक्त)
	    {
		vim_free(option_copy);
		ga_clear_strings(&ga);
		फर्काऊ FAIL;
	    }
	    यदि (ga_add_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		रोक;
	    }
	} जबसम्म (next_val != रिक्त);

	vim_free(option_copy);

	*matches = ga.ga_data;
	*अङ्क_Matches = ga.ga_len;
	फर्काऊ OK;
    }
    अन्यथा यदि (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	यदि (*xp->xp_pattern != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    फर्काऊ FAIL;
	}

	माप_प्रकार अङ्क_flags = STRLEN(option_val);
	यदि (अङ्क_flags == 0)
	    फर्काऊ FAIL;

	*matches = ALLOC_MULT(char_u *, अङ्क_flags + 1);
	यदि (*matches == रिक्त)
	    फर्काऊ FAIL;

	पूर्णाङ्क गन्ती = 0;
	char_u *p;

	p = vim_strnsave(option_val, अङ्क_flags);
	यदि (p == रिक्त)
	{
	    VIM_CLEAR(*matches);
	    फर्काऊ FAIL;
	}
	(*matches)[गन्ती++] = p;

	यदि (अङ्क_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    लागि (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		यदि (p == रिक्त)
		    रोक;
		(*matches)[गन्ती++] = p;
	    }
	}

	*अङ्क_Matches = गन्ती;
	फर्काऊ OK;
    }

    फर्काऊ ExpandOldSetting(अङ्क_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    स्थिर शून्य
option_value2string(
    संरचना vimoption	*opp,
    पूर्णाङ्क			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    यदि (opp->flags & P_अङ्क)
    {
	दीर्घ wc = 0;

	यदि (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_साँचो_name((पूर्णाङ्क)wc, 0));
	अन्यथा यदि (wc != 0)
	    STRCPY(NameBuff, transchar((पूर्णाङ्क)wc));
	अन्यथा
	    प्र_लिखो((वर्ण *)NameBuff, "%ld", *(दीर्घ *)varp);
    }
    अन्यथा    // P_STRING
    {
	varp = *(char_u **)(varp);
	यदि (varp == रिक्त)		    // just in case
	    NameBuff[0] = NUL;
#ifdef FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	अन्यथा यदि (opp->var == (char_u *)&p_साँचो && *varp)
	    STRCPY(NameBuff, "*****");
#endif
	अन्यथा यदि (opp->flags & P_EXPAND)
	    home_replace(रिक्त, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	अन्यथा यदि ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	अन्यथा
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    स्थिर पूर्णाङ्क
wc_use_keyname(char_u *varp, दीर्घ *wcp)
{
    यदि (((दीर्घ *)varp == &p_wc) || ((दीर्घ *)varp == &p_wcm))
    {
	*wcp = *(दीर्घ *)varp;
	यदि (IS_SPECIAL(*wcp) || find_special_साँचो_in_table((पूर्णाङ्क)*wcp) >= 0)
	    फर्काऊ TRUE;
    }
    फर्काऊ FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    पूर्णाङ्क
shortmess(पूर्णाङ्क x)
{
    फर्काऊ p_shm != रिक्त &&
	    (   vim_strchr(p_shm, x) != रिक्त
	    || (vim_strchr(p_shm, 'a') != रिक्त
		&& vim_strchr((char_u *)SHM_A, x) != रिक्त));
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 * When "fname" is not NULL, use it to set $"envname" when it wasn't set yet.
 */
    शून्य
vimrc_found(char_u *fname, char_u *envname)
{
    पूर्णाङ्क		opt_idx;
    पूर्णाङ्क		dofree = FALSE;
    char_u	*p;

    यदि (!option_was_set((char_u *)"cp"))
    {
	p_cp = FALSE;
	लागि (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    यदि (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    यदि (fname != रिक्त)
    {
	p = vim_getenv(envname, &dofree);
	यदि (p == रिक्त)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = FullName_save(fname, FALSE);
	    यदि (p != रिक्त)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	अन्यथा यदि (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    शून्य
change_compatible(पूर्णाङ्क on)
{
    पूर्णाङ्क	    opt_idx;

    यदि (p_cp != on)
    {
	p_cp = on;
	compatible_set();
    }
    opt_idx = findoption((char_u *)"cp");
    यदि (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_SET;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    पूर्णाङ्क
option_was_set(char_u *name)
{
    पूर्णाङ्क idx;

    idx = findoption(name);
    यदि (idx < 0)	// unknown option
	फर्काऊ FALSE;
    यदि (options[idx].flags & P_WAS_SET)
	फर्काऊ TRUE;
    फर्काऊ FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    पूर्णाङ्क
reset_option_was_set(char_u *name)
{
    पूर्णाङ्क idx = findoption(name);

    यदि (idx < 0)
	फर्काऊ FAIL;

    options[idx].flags &= ~P_WAS_SET;
    फर्काऊ OK;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    स्थिर शून्य
compatible_set(शून्य)
{
    पूर्णाङ्क	    opt_idx;

    लागि (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	यदि (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    set_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    पूर्णाङ्क
can_bs(
    पूर्णाङ्क		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ifdef FEAT_JOB_CHANNEL
    यदि (what == BS_START && bt_prompt(curbuf))
	फर्काऊ FALSE;
#endif
    विकल्प (*p_bs)
    {
	पक्ष '3':       फर्काऊ TRUE;
	पक्ष '2':	फर्काऊ (what != BS_NOSTOP);
	पक्ष '1':	फर्काऊ (what != BS_START);
	पक्ष '0':	फर्काऊ FALSE;
    }
    फर्काऊ vim_strchr(p_bs, what) != रिक्त;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    दीर्घ
get_scrolloff_मान(शून्य)
{
    फर्काऊ curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    दीर्घ
get_scrolloffpad_मान(शून्य)
{
    फर्काऊ curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    दीर्घ
get_sidescrolloff_मान(शून्य)
{
    फर्काऊ curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    अचिन्हित पूर्णाङ्क
get_bkc_flags(buf_T *buf)
{
    फर्काऊ buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#यदि defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_मान(buf_T *buf)
{
    यदि (buf->b_p_flp == रिक्त || *buf->b_p_flp == NUL)
	फर्काऊ p_flp;
    फर्काऊ buf->b_p_flp;
}
#endif

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    अचिन्हित पूर्णाङ्क
get_ve_flags(शून्य)
{
    फर्काऊ (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#यदि defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_मान(win_T *win)
{
    यदि (win->w_p_sbr == रिक्त || *win->w_p_sbr == NUL)
	फर्काऊ p_sbr;
    यदि (STRCMP(win->w_p_sbr, "NONE") == 0)
	फर्काऊ empty_option;
    फर्काऊ win->w_p_sbr;
}
#endif

#यदि defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(पूर्णाङ्क bufopt)
{
    dict_T	*d;
    पूर्णाङ्क		opt_idx;

    d = dict_alloc();
    यदि (d == रिक्त)
	फर्काऊ रिक्त;

    लागि (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	संरचना vimoption *opt = &options[opt_idx];

	यदि ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    यदि (varp != रिक्त)
	    {
		यदि (opt->flags & P_STRING)
		    dict_add_string(d, opt->fullname, *(char_u **)varp);
		अन्यथा यदि (opt->flags & P_अङ्क)
		    dict_add_सङ्ख्या(d, opt->fullname, *(दीर्घ *)varp);
		अन्यथा
		    dict_add_सङ्ख्या(d, opt->fullname, *(पूर्णाङ्क *)varp);
	    }
	}
    }

    फर्काऊ d;
}
#endif

#यदि defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    पूर्णाङ्क
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    यदि (val == रिक्त)
	p = wp->w_p_culopt;
    अन्यथा
	p = val;
    जबसम्म (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	यदि (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_पङ्क्ति;
	}
	अन्यथा यदि (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_पङ्क्ति | CULOPT_NBR;
	}
	अन्यथा यदि (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	अन्यथा यदि (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	यदि (*p != ',' && *p != NUL)
	    फर्काऊ FAIL;
	यदि (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    यदि ((culopt_flags_new & CULOPT_पङ्क्ति) && (culopt_flags_new & CULOPT_SCRLINE))
	फर्काऊ FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    फर्काऊ OK;
}
#endif

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    पूर्णाङ्क
magic_isset(शून्य)
{
    विकल्प (magic_overruled)
    {
	पक्ष OPTION_MAGIC_ON:      फर्काऊ TRUE;
	पक्ष OPTION_MAGIC_OFF:     फर्काऊ FALSE;
	पक्ष OPTION_MAGIC_NOT_SET: रोक;
    }
#ifdef FEAT_EVAL
    यदि (in_vim9script())
	फर्काऊ TRUE;
#endif
    फर्काऊ p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    पूर्णाङ्क
option_set_callback_func(char_u *optval UNUSED, callback_T *optcb UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    पूर्णाङ्क		funcname = FALSE;

    यदि (optval == रिक्त || *optval == NUL)
    {
	free_callback(optcb);
	फर्काऊ OK;
    }

    यदि (*optval == '{' || (in_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_अभिव्यक्ति(optval, रिक्त);
    अन्यथा
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    यदि (tv == रिक्त)
	फर्काऊ FAIL;

    cb = get_callback(tv);
    यदि (cb.cb_name == रिक्त || *cb.cb_name == NUL)
    {
	free_tv(tv);
	फर्काऊ FAIL;
    }

    free_callback(optcb);
    set_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = रिक्त;
    यदि (in_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != रिक्त))
    {
	यदि (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = find_imported(optval, dot - optval, FALSE);
	यदि (import != रिक्त && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((वर्ण *)fnamebuf, परिमाण(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	अन्यथा
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

    फर्काऊ OK;
#अन्यथा
    फर्काऊ FAIL;
#endif
}

#यदि defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    वर्ण *
did_set_showtabpanel(optset_T *args UNUSED)
{
    shell_new_columns();
    फर्काऊ रिक्त;
}
#endif

#यदि defined(FEAT_EVAL)
    स्थिर शून्य
didset_options_sctx(पूर्णाङ्क opt_flags, वर्ण **buf)
{
	लागि (पूर्णाङ्क i = 0; ; ++i)
	{
	    यदि (buf[i] == रिक्त)
		रोक;

	    पूर्णाङ्क idx = findoption((char_u *)buf[i]);
	    यदि (idx >= 0)
		set_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#endif
