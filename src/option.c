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

स्थिर शून्य set_options_default(पूर्णांक opt_flags);
स्थिर शून्य set_string_default_esc(अक्षर *name, char_u *val, पूर्णांक escape);
स्थिर char_u *find_dup_item(char_u *origval, char_u *newval, माप_प्रकार newvallen, long_u flags);
स्थिर char_u *option_expand(पूर्णांक opt_idx, char_u *val);
स्थिर शून्य didset_options(शून्य);
स्थिर शून्य didset_options2(शून्य);
#जर defined(FEAT_EVAL)
स्थिर long_u *insecure_flag(win_T *wp, पूर्णांक opt_idx, पूर्णांक opt_flags);
#अन्यथा
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#endif
स्थिर अक्षर *set_bool_option(पूर्णांक opt_idx, char_u *varp, पूर्णांक मूल्य, पूर्णांक opt_flags);
स्थिर अक्षर *set_अंक_option(पूर्णांक opt_idx, char_u *varp, दीर्घ मूल्य, अक्षर *errbuf, माप_प्रकार errbuflen, पूर्णांक opt_flags);
स्थिर पूर्णांक find_किल्ली_option(char_u *arg_arg, पूर्णांक has_lt);
स्थिर शून्य showoptions(पूर्णांक all, पूर्णांक opt_flags);
स्थिर पूर्णांक optval_default(संरचना vimoption *, char_u *varp, पूर्णांक compatible);
स्थिर शून्य showoneopt(संरचना vimoption *, पूर्णांक opt_flags);
स्थिर पूर्णांक put_setstring(खाता *fd, bool legacy, अक्षर *cmd, अक्षर *name, char_u **valuep, long_u flags);
स्थिर पूर्णांक put_setnum(खाता *fd, अक्षर *cmd, अक्षर *name, दीर्घ *valuep);
स्थिर पूर्णांक put_setbool(खाता *fd, अक्षर *cmd, अक्षर *name, पूर्णांक मूल्य);
स्थिर पूर्णांक istermoption(संरचना vimoption *p);
स्थिर पूर्णांक istermoption_idx(पूर्णांक opt_idx);
स्थिर char_u *get_varp_scope(संरचना vimoption *p, पूर्णांक scope);
स्थिर char_u *get_varp(संरचना vimoption *);
स्थिर शून्य check_win_options(win_T *win);
स्थिर शून्य option_value2string(संरचना vimoption *, पूर्णांक scope);
स्थिर शून्य check_winopt(winopt_T *wop);
स्थिर पूर्णांक wc_use_keyname(char_u *varp, दीर्घ *wcp);
स्थिर शून्य compatible_set(शून्य);

#जर defined(FEAT_EVAL)
स्थिर अक्षर *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", रिक्त};
स्थिर अक्षर *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    रिक्त};
स्थिर शून्य didset_options_sctx(पूर्णांक opt_flags, अक्षर **buf);
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
    जर (((p = mch_getenv((char_u *)"SHELL")) != रिक्त && *p != NUL)
#जर defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != रिक्त && *p != NUL)
	    || ((p = (char_u *)default_shell()) != रिक्त && *p != NUL)
#endif
	    )
#जर defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	माप_प्रकार	    len;

	जर (vim_strchr(p, ' ') != रिक्त)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    जर (cmd != रिक्त)
	    {
		vim_snprintf((अक्षर *)cmd, len, "\"%s\"", p);
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
    पूर्णांक		opt_idx;
    पूर्णांक		i;
    char_u	*p;
    पूर्णांक		plen;
#ifdef UNIX
    स्थिर अक्षर	*(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#अन्यथा
    स्थिर अक्षर	*(names[3]) = {"TMPDIR", "TEMP", "TMP"};
#endif
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(names); ++i)
    {
	पूर्णांक		mustfree = FALSE;
#ifdef UNIX
	जर (*names[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (पूर्णांक)STRLEN_LITERAL("/private/tmp");
# अन्यथा
	    p = (char_u *)"/tmp";
	    plen = (पूर्णांक)STRLEN_LITERAL("/tmp");
# endif
	}
	अन्यथा
#endif
	{
	    p = vim_getenv((char_u *)names[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	जर (p != रिक्त && *p != NUL)
	{
	    char_u  *item;
	    माप_प्रकार  itemsize;
	    पूर्णांक	    has_trailing_path_sep = FALSE;

	    जर (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (पूर्णांक)STRLEN(p);
		// does the value include a trailing path separator?
		जर (after_pathsep(p, p + plen))
		    has_trailing_path_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_path_sep ? 0 : 1) + 2;
	    item = alloc(itemsize);
	    जर (item != रिक्त)
	    {
		// add a preceding comma as a separator after the first item
		माप_प्रकार	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		माप_प्रकार	itemlen;

		itemlen = vim_snprintf((अक्षर *)item, itemsize, "%s%s*", p, (has_trailing_path_sep) ? "" : PATHSEPSTR);

		जर (find_dup_item(ga.ga_data, item, itemlen, options[opt_idx].flags) == रिक्त
			&& ga_grow(&ga, (पूर्णांक)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((अक्षर *)ga.ga_data + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", item);
		}
		vim_free(item);
	    }
	}
	जर (mustfree)
	    vim_free(p);
    }
    जर (ga.ga_data != रिक्त)
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
    पूर्णांक		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    जर (opt_idx < 0)
	परत;

#जर !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
    जर (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#endif
    {
#जर defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#किंवाजर defined(HAVE_TOTAL_MEM)
	// Use amount of memory available to Vim.
	n = (mch_total_mem(FALSE) >> 1);
#अन्यथा
	n = (0x7fffffff >> 11);
#endif
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	जर (opt_idx >= 0)
	{
#जर !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
	    जर ((दीर्घ)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (दीर्घ)n
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
    पूर्णांक		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    पूर्णांक	i;
    पूर्णांक	j;
    पूर्णांक	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    जर (cdpath == रिक्त)
	परत;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    जर (buf != रिक्त)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	साठी (i = 0; cdpath[i] != NUL; ++i)
	{
	    जर (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    अन्यथा
	    {
		जर (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	जर (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	अन्यथा
	    vim_free(buf); // cannot happen
    }
    जर (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    स्थिर शून्य
set_init_default_printencoding(शून्य)
{
#जर defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    set_string_default("penc",
# जर defined(MSWIN)
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

#जर defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    स्थिर शून्य
set_init_default_printexpr(शून्य)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    set_string_default("pexpr",
# जर defined(MSWIN)
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
    जर (p == रिक्त)
	परत;
    जर (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
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
    पूर्णांक		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    जर (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    जर (opt_idx < 0)
	परत;
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
    पूर्णांक		opt_idx;
    पूर्णांक		has_xdg_env = TRUE;
    पूर्णांक		should_free_xdg_dir = FALSE;
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
    जर (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    जर (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	जा theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    जर ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	जा theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    जर ((opt_idx = findoption((char_u *)"packpath")) < 0)
	जा theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# जर defined(XDG_VDIR) && defined(FEAT_SESSION)
    जर ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	जा theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    जर (should_free_xdg_dir)
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
    पूर्णांक		opt_idx;
    char_u	*p;

    साठी (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	जर ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != रिक्त)
	    p = (char_u *)_(*(अक्षर **)options[opt_idx].var);
	अन्यथा
	    p = option_expand(opt_idx, रिक्त);
	जर (p != रिक्त && (p = vim_strsave(p)) != रिक्त)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    जर (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#जर defined(MSWIN) && defined(FEAT_GETTEXT)
/*
 * Get the display language of Windows and the languages to fall back on, as a
 * colon separated list for gettext, e.g. "ja_JP:en_US".  The list stops after
 * English, untranslated messages are English already.
 * Returns NULL when it cannot be obtained.  The result must be freed.
 */
    स्थिर char_u *
get_ui_langs(शून्य)
{
    ULONG	अंक_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*buffer;
    char_u	*langs = रिक्त;

    जर (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &अंक_languages, रिक्त,
								    &bufsize)
	    || bufsize == 0)
	परत रिक्त;

    buffer = ALLOC_MULT(WCHAR, bufsize);
    जर (buffer == रिक्त)
	परत रिक्त;

    जर (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &अंक_languages, buffer,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	जर (langs != रिक्त)
	{
	    char_u	*d = langs;
	    WCHAR	*s = buffer;

	    पर्यंत (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		जर (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		साठी ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		जर (english)
		    थांबा;
	    }
	    *d = NUL;
	}
    }
    vim_free(buffer);

    परत langs;
}
#endif

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    स्थिर शून्य
set_init_lang_env(शून्य)
{
#जर defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    जर (mch_getenv((char_u *)"LANG") == रिक्त
	    && mch_getenv((char_u *)"LANGUAGE") == रिक्त
	    && mch_getenv((char_u *)"LC_ALL") == रिक्त
	    && mch_getenv((char_u *)"LC_MESSAGES") == रिक्त)
    {
	char_u	*langs = get_ui_langs();

	जर (langs != रिक्त && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    जर (colon != रिक्त)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#किंवाजर defined(MACOS_CONVERT)
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
    पूर्णांक		opt_idx;

#जर defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#अन्यथा
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#endif
    जर (p == रिक्त)
	परत;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    जर (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    जर (mb_init() == रिक्त)
    {
	opt_idx = findoption((char_u *)"encoding");
	जर (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#जर defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	जर (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    set_string_option_direct((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    set_string_option_direct((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    जर (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    जर (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (शून्य)init_chartab();
	}
#endif

#जर defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	जर (
# ifdef VIMDLL
		(!gui.in_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    अक्षर	buf[50];
	    माप_प्रकार	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    जर (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, आकार(buf), "cp%ld", (दीर्घ)GetACP());
	    अन्यथा
		buflen = vim_snprintf(buf, आकार(buf), "cp%ld", (दीर्घ)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    जर (p_tenc != रिक्त)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		जर (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_setup(&आदान_conv, p_tenc, p_enc);
		convert_setup(&output_conv, p_enc, p_tenc);
	    }
	    अन्यथा
		p_tenc = empty_option;
	}
#endif
#जर defined(MSWIN)
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
set_init_1(पूर्णांक clean_arg)
{
#ifdef FEAT_LANGMAP
    langmap_init();
#endif

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    जर (mch_getenv((char_u *)"VIM_POSIX") != रिक्त)
    {
	set_string_default("cpo", (char_u *)CPO_ALL);
	set_string_default("shm", (char_u *)SHM_POSIX);
    }

    set_init_default_shell();
    set_init_default_backupskip();
    set_init_default_maxmemtot();
    set_init_default_cdpath();
    set_init_default_printencoding();
#जर defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
    जर (clean_arg)
	set_init_clean_rtp();
#endif

#ifdef FEAT_GUI
    जर (found_reverse_arg)
	set_option_मूल्य_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
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

#जर defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    जर (mch_getenv((char_u *)"MLTERM") != रिक्त)
	set_option_मूल्य_give_err((char_u *)"tbidi", 1L, रिक्त, 0);
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
    पूर्णांक		opt_idx,
    पूर्णांक		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    पूर्णांक		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    पूर्णांक		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    पूर्णांक		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    जर (varp != रिक्त)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	जर (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    जर (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		set_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    अन्यथा जर (options[opt_idx].indir != PV_NONE)
		set_string_option_direct(रिक्त, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    अन्यथा
	    {
		जर ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	अन्यथा जर (flags & P_अंक)
	{
	    जर (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    अन्यथा
	    {
		दीर्घ def_val = (दीर्घ)(long_i)options[opt_idx].def_val[dvi];

		जर ((दीर्घ *)varp == &curwin->w_p_so
			|| (दीर्घ *)varp == &curwin->w_p_siso
			|| (दीर्घ *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(दीर्घ *)varp = -1;
		अन्यथा
		    *(दीर्घ *)varp = def_val;
		// May also set global value for local option.
		जर (both)
		    *(दीर्घ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	अन्यथा	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(पूर्णांक *)varp = (पूर्णांक)(दीर्घ)(long_i)options[opt_idx].def_val[dvi];
#ifdef UNIX
	    // 'modeline' defaults to off for root
	    जर (options[opt_idx].indir == PV_ML && getuid() == मूळ_UID)
		*(पूर्णांक *)varp = FALSE;
#endif
	    // May also set global value for local option.
	    जर (both)
		*(पूर्णांक *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(पूर्णांक *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	जर (both)
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
    पूर्णांक		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    पूर्णांक		i;
    win_T	*wp;
    tabpage_T	*tp;

    साठी (i = 0; !istermoption_idx(i); i++)
	जर (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#जर defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_किल्ली
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
set_string_default_esc(अक्षर *name, char_u *val, पूर्णांक escape)
{
    char_u	*p;
    पूर्णांक		opt_idx;

    जर (escape && vim_strchr(val, ' ') != रिक्त)
	p = vim_strsave_escaped(val, (char_u *)" ");
    अन्यथा
	p = vim_strsave(val);
    जर (p == रिक्त)		// we don't want a NULL
	परत;

    opt_idx = findoption((char_u *)name);
    जर (opt_idx < 0)
    {
	vim_free(p);
	परत;
    }

    जर (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    शून्य
set_string_default(अक्षर *name, char_u *val)
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
    पूर्णांक	    bs = 0;
    char_u  *s;

    जर (origval == रिक्त)
	परत रिक्त;

    साठी (s = origval; *s != NUL; ++s)
    {
	जर ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    परत s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	जर ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	अन्यथा
	    bs = 0;
    }
    परत रिक्त;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    शून्य
set_संख्या_default(अक्षर *name, दीर्घ val)
{
    पूर्णांक		opt_idx;

    opt_idx = findoption((char_u *)name);
    जर (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#जर defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    शून्य
set_local_options_default(win_T *wp, पूर्णांक do_buffer)
{
    win_T	*save_curwin = curwin;
    पूर्णांक		i;

    curwin = wp;
    curbuf = curwin->w_buffer;
    block_autocmds();

    साठी (i = 0; !istermoption_idx(i); i++)
    {
	संरचना vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	जर (p->indir != PV_NONE
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

#जर defined(EXITFREE)
/*
 * Free all options.
 */
    शून्य
free_all_options(शून्य)
{
    पूर्णांक		i;

    साठी (i = 0; !istermoption_idx(i); i++)
    {
	जर (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    जर ((options[i].flags & P_ALLOCED) && options[i].var != रिक्त)
		free_string_option(*(char_u **)options[i].var);
	    जर (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	अन्यथा जर (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# जर defined(FEAT_EVAL)
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
    पूर्णांक		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    जर (idx >= 0 && !(options[idx].flags & P_WAS_SET))
	set_option_default(idx, OPT_LOCAL, p_cp);
    comp_col();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    जर (!option_was_set((char_u *)"window"))
	p_window = Rows - 1;
    set_संख्या_default("window", Rows - 1);

    // For DOS console the default is always black.
#जर !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    जर (idx >= 0 && !(options[idx].flags & P_WAS_SET)
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
#जर defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    पूर्णांक	    idx_srr;
    पूर्णांक	    do_srr;
# ifdef FEAT_QUICKFIX
    पूर्णांक	    idx_sp;
    पूर्णांक	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    जर (idx_srr < 0)
	do_srr = FALSE;
    अन्यथा
	do_srr = !(options[idx_srr].flags & P_WAS_SET);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    जर (idx_sp < 0)
	do_sp = FALSE;
    अन्यथा
	do_sp = !(options[idx_sp].flags & P_WAS_SET);
# endif
    p = get_isolated_shell_name();
    जर (p != रिक्त)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	जर (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# जर defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# जर defined(FEAT_QUICKFIX)
	    जर (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  अन्यथा
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    जर (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	अन्यथा जर (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  जर defined(FEAT_QUICKFIX)
		जर (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		जर (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	अन्यथा
	    // Always use POSIX shell style redirection if we reach this
	    जर (       fnamecmp(p, "sh") == 0
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
# जर defined(FEAT_QUICKFIX)
		जर (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  अन्यथा
		    जर (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    अन्यथा
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		जर (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#endif

#जर defined(MSWIN)
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
    जर (म_माला((अक्षर *)gettail(p_sh), "powershell") != रिक्त)
    {
	पूर्णांक	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	जर (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	जर (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    अन्यथा जर (म_माला((अक्षर *)gettail(p_sh), "sh") != रिक्त)
    {
	पूर्णांक	idx3;

	idx3 = findoption((char_u *)"shcf");
	जर (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	जर (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    अन्यथा जर (म_माला((अक्षर *)gettail(p_sh), "cmd.exe") != रिक्त)
    {
	पूर्णांक	idx3;

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
	जर (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	जर (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#endif

    जर (BUFEMPTY())
    {
	पूर्णांक idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	जर (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_SET))
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    set_title_defaults();
}

#जर defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    शून्य
set_helplang_default(char_u *lang)
{
    पूर्णांक		idx;
    माप_प्रकार	langlen;

    जर (lang == रिक्त)	// safety check
	परत;

    langlen = STRLEN(lang);
    जर (langlen < 2)	// safety check
	परत;

    idx = findoption((char_u *)"hlg");
    जर (idx < 0 || (options[idx].flags & P_WAS_SET))
	परत;

    जर (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    जर (p_hlg == रिक्त)
	p_hlg = empty_option;
    अन्यथा
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	जर (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	अन्यथा जर (langlen >= 1 && *p_hlg == 'C')
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
    पूर्णांक	    idx1;
    दीर्घ    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    जर (idx1 >= 0 && !(options[idx1].flags & P_WAS_SET))
    {
#ifdef FEAT_GUI
	जर (gui.starting || gui.in_use)
	    val = TRUE;
	अन्यथा
#endif
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    जर (idx1 < 0 || (options[idx1].flags & P_WAS_SET))
    {
	परत;
    }

#ifdef FEAT_GUI
    जर (gui.starting || gui.in_use)
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
    पूर्णांक		flags = 0;

    जर (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    अन्यथा जर (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#जर defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    जर ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    अन्यथा
#endif
    {
	जर (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(शून्य)do_set(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
प्रकारव्याख्या गणना {
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
    पूर्णांक		prefix = PREFIX_NONE;
    char_u	*arg = *argp;

    जर (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	prefix = PREFIX_NO;
	arg += 2;
    }
    अन्यथा जर (STRNCMP(arg, "inv", 3) == 0)
    {
	prefix = PREFIX_INV;
	arg += 3;
    }

    *argp = arg;
    परत prefix;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    स्थिर पूर्णांक
parse_option_name(char_u *arg, पूर्णांक *opt_idxp, पूर्णांक *lenp, पूर्णांक *keyp)
{
    पूर्णांक	किल्ली = 0;
    पूर्णांक	len;
    पूर्णांक	opt_idx;

    जर (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	जर (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	अन्यथा
	{
	    len = 1;
	    पर्यंत (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	जर (arg[len] != '>')
	    परत FAIL;

	arg[len] = NUL;			    // put NUL after name
	जर (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	जर (opt_idx == -1)
	    किल्ली = find_किल्ली_option(arg + 1, TRUE);
    }
    अन्यथा
    {
	पूर्णांक	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	जर (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	अन्यथा
	    पर्यंत (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	जर (opt_idx == -1)
	    किल्ली = find_किल्ली_option(arg, FALSE);
    }

    *keyp = किल्ली;
    *lenp = len;
    *opt_idxp = opt_idx;

    परत OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    स्थिर set_op_T
get_opt_op(char_u *arg)
{
    set_op_T op = OP_NONE;

    जर (*arg != NUL && *(arg + 1) == '=')
    {
	जर (*arg == '+')
	    op = OP_ADDING;		// "+="
	अन्यथा जर (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	अन्यथा जर (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    परत op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
स्थिर अक्षर *modeline_whitelist[] =
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
is_modeline_whitelisted(अक्षर *name)
{
    साठी (पूर्णांक i = 0; modeline_whitelist[i] != रिक्त; i++)
	जर (STRCMP(name, modeline_whitelist[i]) == 0)
	    परत true;
    परत false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    स्थिर पूर्णांक
validate_opt_idx(
	पूर्णांक opt_idx,
	पूर्णांक opt_flags,
	long_u flags,
	अक्षर **errmsg,
	set_prefix_T prefix)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    जर ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	परत FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    जर ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	परत FAIL;

    // Disallow changing some options from modelines.
    जर (opt_flags & OPT_MODELINE)
    {
	जर (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_in_modeline;
	    परत FAIL;
	}
	जर ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_in_modeline_when_modelineexpr_is_off;
	    परत FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	जर (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    जर (options[opt_idx].indir == PV_ML && prefix == PREFIX_NO)
		परत OK;
	    अन्यथा जर (!is_modeline_whitelisted(options[opt_idx].fullname))
		परत FAIL;
	}
#ifdef FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	जर (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    परत FAIL;
#endif
    }

#ifdef HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    जर (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_in_sandbox;
	परत FAIL;
    }
#endif

    परत OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    स्थिर char_u *
stropt_get_default_val(
    पूर्णांक		opt_idx,
    char_u	*varp,
    पूर्णांक		flags,
    पूर्णांक		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    जर ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ifdef FEAT_GUI
	जर (gui.in_use)
	    newval = gui_bg_default();
	अन्यथा
#endif
	    newval = term_bg_default();
    }
    अन्यथा जर ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    जर (newval == रिक्त)
	newval = empty_option;
    अन्यथा
    {
	char_u *s = option_expand(opt_idx, newval);
	जर (s == रिक्त)
	    s = newval;
	newval = vim_strsave(s);
    }

    परत newval;
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
    पूर्णांक		i = getdigits((char_u **)varp);

    निवड (i)
    {
	स्थिती 0:
	    *(char_u **)varp = empty_option;
	    थांबा;
	स्थिती 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    थांबा;
	स्थिती 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    थांबा;
	स्थिती 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    थांबा;
    }
    vim_free(*oldval_p);
    जर (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    जर (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    जर (*origval_g_p == *oldval_p)
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
    पूर्णांक i = getdigits(argp);
    जर (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    जर (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    जर (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    जर (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    जर (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    जर (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    परत whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    स्थिर char_u *
stropt_copy_मूल्य(
    char_u	*origval,
    char_u	**argp,
    set_op_T	op,
    पूर्णांक		flags UNUSED)
{
    char_u	*arg = *argp;
    अचिन्हित	newlen;
    char_u	*newval;
    char_u	*s = रिक्त;

    // get a bit too much
    newlen = (अचिन्हित)STRLEN(arg) + 1;
    जर (op != OP_NONE)
	newlen += (अचिन्हित)STRLEN(origval) + 1;
    newval = alloc(newlen);
    जर (newval == रिक्त)  // out of mem, don't change
	परत रिक्त;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    पर्यंत (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	पूर्णांक i;

	जर (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#endif
	   )
	    ++arg;	// remove backslash
	जर (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
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
    परत newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    स्थिर char_u *
stropt_expand_envvar(
    पूर्णांक		opt_idx,
    char_u	*origval,
    char_u	*newval,
    set_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    जर (s == रिक्त)
	परत newval;

    vim_free(newval);
    अचिन्हित newlen = (अचिन्हित)STRLEN(s) + 1;
    जर (op != OP_NONE)
	newlen += (अचिन्हित)STRLEN(origval) + 1;

    newval = alloc(newlen);
    जर (newval == रिक्त)
	परत रिक्त;

    STRCPY(newval, s);

    परत newval;
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
    पूर्णांक		flags)
{
    पूर्णांक		len = 0;

    पूर्णांक comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    जर (op == OP_ADDING)
    {
	len = (पूर्णांक)STRLEN(origval);
	// strip a trailing comma, would get 2
	जर (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (माप_प्रकार)len);
    }
    अन्यथा
    {
	len = (पूर्णांक)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    जर (comma)
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
    पूर्णांक		flags,
    char_u	*strval,
    पूर्णांक		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    जर (*strval)
    {
	// may need to remove a comma
	जर (flags & P_COMMA)
	{
	    जर (strval == origval)
	    {
		// include comma after string
		जर (strval[len] == ',')
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
find_किल्ली_item(char_u *src, char_u *किल्ली, पूर्णांक keylen, पूर्णांक *itemlenp)
{
    char_u  *p = src;

    पर्यंत (*p != NUL)
    {
	// Check if this item starts with the same key
	जर ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, किल्ली, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *end = vim_strchr(p, ',');
	    जर (end == रिक्त)
		end = p + STRLEN(p);
	    *itemlenp = (पूर्णांक)(end - p);
	    परत p;
	}
	++p;
    }
    परत रिक्त;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    स्थिर शून्य
remove_comma_item(char_u *str, char_u *item, पूर्णांक itemlen)
{
    जर (item[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(item, item + itemlen + 1);
    अन्यथा जर (item > str && *(item - 1) == ',')
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
remove_किल्ली_item(char_u *str, char_u *किल्ली, पूर्णांक keylen, char_u *skip)
{
    पूर्णांक	    itemlen;
    char_u  *found;

    पर्यंत ((found = find_किल्ली_item(str, किल्ली, keylen, &itemlen)) != रिक्त)
    {
	जर (found == skip)
	{
	    // Search for the next match after this one.
	    char_u *next = found + itemlen;
	    जर (*next == ',')
		++next;
	    found = find_किल्ली_item(next, किल्ली, keylen, &itemlen);
	    जर (found == रिक्त)
		थांबा;
	}

	remove_comma_item(str, found, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    स्थिर शून्य
append_item(char_u *str, char_u *item, पूर्णांक item_len)
{
    पूर्णांक len = (पूर्णांक)STRLEN(str);

    जर (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, item, (माप_प्रकार)item_len);
    str[len + item_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    स्थिर शून्य
prepend_item(char_u *str, char_u *item, पूर्णांक item_len)
{
    पूर्णांक len = (पूर्णांक)STRLEN(str);
    पूर्णांक comma = (len > 0) ? 1 : 0;

    mch_memmove(str + item_len + comma, str, (माप_प्रकार)len + 1);
    mch_memmove(str, item, (माप_प्रकार)item_len);
    जर (comma)
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
    पूर्णांक		flags UNUSED)
{
    char_u  *p;
    char_u  *item_start;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    जर (vim_strchr(newval, ':') == रिक्त && vim_strchr(newval, ',') == रिक्त)
	परत false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    जर (newval_copy == रिक्त)
	परत false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    item_start = newval_copy;
    साठी (;;)
    {
	p = vim_strchr(item_start, ',');
	पूर्णांक item_len = (p == रिक्त)
			    ? (पूर्णांक)STRLEN(item_start) : (पूर्णांक)(p - item_start);

	जर (item_len > 0)
	{
	    char_u *colon = vim_strchr(item_start, ':');
	    जर (colon != रिक्त && colon < item_start + item_len)
	    {
		पूर्णांक keylen = (पूर्णांक)(colon - item_start) + 1;

		जर (op == OP_ADDING || op == OP_PREPENDING)
		{
		    पूर्णांक old_itemlen;
		    char_u *found = find_किल्ली_item(newval, item_start,
						       keylen, &old_itemlen);
		    जर (found != रिक्त)
		    {
			जर (old_itemlen == item_len
				&& STRNCMP(found, item_start,
							 item_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_किल्ली_item(newval, item_start,
							   keylen, found);
			}
			अन्यथा
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_किल्ली_item(newval, item_start,
							    keylen, रिक्त);
			    जर (op == OP_PREPENDING)
				prepend_item(newval, item_start, item_len);
			    अन्यथा
				append_item(newval, item_start, item_len);
			}
		    }
		    अन्यथा
		    {
			// New item.
			जर (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			अन्यथा
			    append_item(newval, item_start, item_len);
		    }
		}
		अन्यथा जर (op == OP_REMOVING)
		    remove_किल्ली_item(newval, item_start, keylen, रिक्त);
	    }
	    अन्यथा
	    {
		जर (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    जर (found == रिक्त)
		    {
			// New item.
			जर (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			अन्यथा
			    append_item(newval, item_start, item_len);
		    }
		    // else: exact duplicate — do nothing
		}
		अन्यथा जर (op == OP_REMOVING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    जर (found != रिक्त)
			remove_comma_item(newval, found, item_len);
		}
	    }
	}

	जर (p == रिक्त)
	    थांबा;
	item_start = p + 1;
    }

    vim_free(newval_copy);

    परत true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    स्थिर शून्य
stropt_remove_dupflags(char_u *newval, पूर्णांक flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    पर्यंत (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	जर (flags & P_ONECOMMA)
	{
	    जर (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != रिक्त)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		सुरूठेवा;
	    }
	}
	अन्यथा
	{
	    जर ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != रिक्त)
	    {
		STRMOVE(s, s + 1);
		सुरूठेवा;
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
    पूर्णांक		nextchar,
    पूर्णांक		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    set_op_T    *op_arg,
    पूर्णांक		flags,
    पूर्णांक		cp_val)
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

    जर (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    अन्यथा जर (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    अन्यथा
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	जर (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	अन्यथा जर (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	अन्यथा जर (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	अन्यथा जर (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_मूल्य(origval, &arg, op, flags);
	जर (newval == रिक्त)
	    जा done;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	जर (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    जर (newval == रिक्त)
		जा done;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	जर ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	अन्यथा
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    पूर्णांक len = 0;
	    जर (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (पूर्णांक)STRLEN(newval);
		s = find_dup_item(origval, newval, len, flags);

		// do not add if already there
		जर ((op == OP_ADDING || op == OP_PREPENDING) && s != रिक्त)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		जर (s == रिक्त)
		    s = origval + (पूर्णांक)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    जर (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    अन्यथा जर (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	जर (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

done:
    जर (save_arg != रिक्त)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    परत newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    स्थिर पूर्णांक
do_set_option_string(
	पूर्णांक	    opt_idx,
	पूर्णांक	    opt_flags,
	char_u	    **argp,
	पूर्णांक	    nextchar,
	set_op_T    op_arg,
	long_u	    flags,
	पूर्णांक	    cp_val,
	char_u	    *varp_arg,
	अक्षर	    *errbuf,
	माप_प्रकार	    errbuflen,
	पूर्णांक	    *मूल्य_checked,
	अक्षर	    **errmsg)
{
    char_u	*arg = *argp;
    set_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = रिक्त; // previous value if *varp
    char_u	*newval;
    char_u	*origval = रिक्त;
    char_u	*origval_l = रिक्त;
    char_u	*origval_g = रिक्त;
#जर defined(FEAT_EVAL)
    char_u	*saved_origval = रिक्त;
    char_u	*saved_origval_l = रिक्त;
    char_u	*saved_origval_g = रिक्त;
    char_u	*saved_newval = रिक्त;
#endif

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    जर ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((पूर्णांक)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    जर ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	जर (((पूर्णांक)options[opt_idx].indir & PV_BOTH)
						  && origval_l == empty_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    जर (((पूर्णांक)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    अन्यथा
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    जर (newval == रिक्त)
	*(char_u **)(varp) = empty_option;

#जर defined(FEAT_EVAL)
    जर (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_किल्ली
# endif
		      && origval != रिक्त && newval != रिक्त)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	जर (origval_l != रिक्त)
	    saved_origval_l = vim_strsave(origval_l);
	जर (origval_g != रिक्त)
	    saved_origval_g = vim_strsave(origval_g);
    }
#endif

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	पूर्णांक	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	जर ((opt_flags & OPT_MODELINE)
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
			errbuflen, opt_flags, op, मूल्य_checked);

	secure = secure_saved;
    }

#जर defined(FEAT_EVAL)
    जर (*errmsg == रिक्त)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#endif

    *argp = arg;
    परत *errmsg == रिक्त ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    स्थिर अक्षर *
do_set_option_bool(
    पूर्णांक		opt_idx,
    पूर्णांक		opt_flags,
    set_prefix_T prefix,
    long_u	flags,
    char_u	*varp,
    पूर्णांक		nextchar,
    पूर्णांक		afterchar,
    पूर्णांक		cp_val)

{
    varnumber_T	मूल्य;

    जर (nextchar == '=' || nextchar == ':')
	परत e_invalid_argument;
    जर (opt_idx < 0 || varp == रिक्त)
	परत रिक्त;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    जर (nextchar == '!')
	मूल्य = *(पूर्णांक *)(varp) ^ 1;
    अन्यथा जर (nextchar == '&')
	मूल्य = (पूर्णांक)(दीर्घ)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    अन्यथा जर (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	जर ((पूर्णांक *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    मूल्य = -1;
	अन्यथा जर ((पूर्णांक *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    मूल्य = -1;
#ifdef HAVE_FSYNC
	अन्यथा जर ((पूर्णांक *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    मूल्य = -1;
#endif
	अन्यथा
	    मूल्य = *(पूर्णांक *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    अन्यथा
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	जर (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    परत e_trailing_characters;
	जर (prefix == PREFIX_INV)
	    मूल्य = *(पूर्णांक *)(varp) ^ 1;
	अन्यथा
	    मूल्य = prefix == PREFIX_NO ? 0 : 1;
    }

    परत set_bool_option(opt_idx, varp, (पूर्णांक)मूल्य, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    स्थिर अक्षर *
do_set_option_numeric(
    पूर्णांक		opt_idx,
    पूर्णांक		opt_flags,
    char_u	**argp,
    पूर्णांक		nextchar,
    set_op_T	op,
    long_u	flags,
    पूर्णांक		cp_val,
    char_u	*varp,
    अक्षर	*errbuf,
    माप_प्रकार	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		मूल्य;
    पूर्णांक			i;
    अक्षर		*errmsg = रिक्त;

    जर (opt_idx < 0 || varp == रिक्त)
	परत रिक्त;  // "cannot happen"
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
    जर (nextchar == '&')
	मूल्य = (दीर्घ)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    अन्यथा जर (nextchar == '<')
    {
	जर ((दीर्घ *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    मूल्य = NO_LOCAL_UNDOLEVEL;
	अन्यथा जर (opt_flags == OPT_LOCAL
		    && ((दीर्घ *)varp == &curwin->w_p_siso
		     || (दीर्घ *)varp == &curwin->w_p_so
		     || (दीर्घ *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    मूल्य = -1;
	अन्यथा
	    मूल्य = *(दीर्घ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    अन्यथा जर (((दीर्घ *)varp == &p_wc || (दीर्घ *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	मूल्य = string_to_किल्ली(arg, FALSE);
	जर (मूल्य == 0)
	{
	    errmsg = e_invalid_argument;
	    जा skip;
	}
    }
    अन्यथा जर (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, रिक्त, &i, STR2NR_ALL, &मूल्य, रिक्त, 0, TRUE, रिक्त);
	जर (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_संख्या_required_after_equal;
	    जा skip;
	}
    }
    अन्यथा
    {
	errmsg = e_संख्या_required_after_equal;
	जा skip;
    }

    जर (op == OP_ADDING)
	मूल्य = *(दीर्घ *)varp + मूल्य;
    अन्यथा जर (op == OP_PREPENDING)
	मूल्य = *(दीर्घ *)varp * मूल्य;
    अन्यथा जर (op == OP_REMOVING)
	मूल्य = *(दीर्घ *)varp - मूल्य;

    जर ((दीर्घ *)varp == &curwin->w_p_sop && मूल्य < -1)
    {
	errmsg = e_invalid_argument;
	जा skip;
    }

    errmsg = set_अंक_option(opt_idx, varp, मूल्य, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    परत errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    स्थिर अक्षर *
do_set_option_keycode(char_u **argp, char_u *किल्ली_name, पूर्णांक nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    जर (nextchar == '&')
    {
	जर (add_termcap_entry(किल्ली_name, TRUE) == FAIL)
	    परत e_not_found_in_termcap;
    }
    अन्यथा
    {
	++arg; // jump to after the '=' or ':'
	साठी (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    जर (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	add_termcode(किल्ली_name, arg, FALSE);
	*p = nextchar;
    }
    जर (full_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    परत रिक्त;
}

/*
 * Set an option to a new value.
 */
    स्थिर अक्षर *
do_set_option_मूल्य(
    पूर्णांक		opt_idx,
    पूर्णांक		opt_flags,
    char_u	**argp,
    set_prefix_T prefix,
    set_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*किल्ली_name,
    पूर्णांक		nextchar,
    पूर्णांक		afterchar,
    पूर्णांक		cp_val,
    पूर्णांक		*stopopteval,
    अक्षर	*errbuf,
    माप_प्रकार	errbuflen)
{
    पूर्णांक		मूल्य_checked = FALSE;
    अक्षर	*errmsg = रिक्त;
    char_u	*arg = *argp;

    जर (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_set_option_bool(opt_idx, opt_flags, prefix, flags, varp,
						nextchar, afterchar, cp_val);
	जर (errmsg != रिक्त)
	    जा skip;
    }
    अन्यथा
    {
	// numeric or string option
	जर (vim_strchr((char_u *)"=:&<", nextchar) == रिक्त
		|| prefix != PREFIX_NONE)
	{
	    errmsg = e_invalid_argument;
	    जा skip;
	}

	जर (flags & P_अंक)
	{
	    // numeric option
	    errmsg = do_set_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    जर (errmsg != रिक्त)
		जा skip;
	}
	अन्यथा जर (opt_idx >= 0)
	{
	    // string option
	    जर (do_set_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&मूल्य_checked, &errmsg) == FAIL)
	    {
		जर (errmsg != रिक्त)
		    जा skip;
		*stopopteval = TRUE;
		जा skip;
	    }
	}
	अन्यथा
	{
	    // key code option
	    errmsg = do_set_option_keycode(&arg, किल्ली_name, nextchar);
	    जर (errmsg != रिक्त)
		जा skip;
	}
    }

    जर (opt_idx >= 0)
	did_set_option(opt_idx, opt_flags, op == OP_NONE, मूल्य_checked);

skip:
    *argp = arg;
    परत errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    स्थिर अक्षर *
do_set_option(
    पूर्णांक		opt_flags,
    char_u	**argp,
    char_u	*arg_start,
    char_u	**startarg,
    पूर्णांक		*did_show,
    पूर्णांक		*stopopteval,
    अक्षर	*errbuf,
    माप_प्रकार	errbuflen)
{
    पूर्णांक		opt_idx;
    char_u	*arg;
    set_prefix_T prefix;	// no prefix, "no" prefix or "inv" prefix
    set_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	किल्ली_name[2];
    पूर्णांक		nextchar;	// next non-white char after option name
    पूर्णांक		afterchar;	// character just after option name
    अक्षर	*errmsg = रिक्त;
    पूर्णांक		किल्ली;
    पूर्णांक		len;

    prefix = get_option_prefix(argp);
    arg = *argp;

    // find end of name
    किल्ली = 0;
    जर (parse_option_name(arg, &opt_idx, &len, &किल्ली) == FAIL)
	परत e_invalid_argument;

    // remember character after option name
    afterchar = arg[len];

    जर (in_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	जर (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != रिक्त
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    जा skip;
	}
    }
    अन्यथा
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	पर्यंत (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    जर (op != OP_NONE)
	len++;

    nextchar = arg[len];

    जर (opt_idx == -1 && किल्ली == 0)	// found a mismatch: skip
    {
	जर (in_vim9script() && arg > arg_start
		&& vim_strchr((char_u *)"!&<", *arg) != रिक्त)
	    errmsg = e_no_white_space_allowed_between_option_and;
	अन्यथा
	    errmsg = e_unknown_option;
	जा skip;
    }

    जर (opt_idx >= 0)
    {
	जर (options[opt_idx].var == रिक्त)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    जर (vim_strchr((char_u *)"=:!&<", nextchar) == रिक्त
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    जा skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    अन्यथा
    {
	flags = P_STRING;
	varp = रिक्त;
	जर (किल्ली < 0)
	{
	    किल्ली_name[0] = KEY2TERMCAP0(किल्ली);
	    किल्ली_name[1] = KEY2TERMCAP1(किल्ली);
	}
	अन्यथा
	{
	    किल्ली_name[0] = KS_किल्ली;
	    किल्ली_name[1] = (किल्ली & 0xff);
	}
    }

    // Make sure the option value can be changed.
    जर (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, prefix) == FAIL)
	जा skip;

    पूर्णांक cp_val = p_cp;
    जर (vim_strchr((char_u *)"?=:!&<", nextchar) != रिक्त)
    {
	arg += len;
	जर (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    जर (arg[3] == 'm')	// "opt&vim": set to Vim default
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
	जर (vim_strchr((char_u *)"?!&<", nextchar) != रिक्त
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    जा skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    जर (nextchar == '?'
	    || (prefix == PREFIX_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == रिक्त
		&& !(flags & P_BOOL)))
    {
	// print value
	जर (*did_show)
	    संदेश_putchar('\n');	    // cursor below last one
	अन्यथा
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	जर (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ifdef FEAT_EVAL
	    जर (p_verbose > 0)
	    {
		// Mention where the option was last set.
		जर (varp == options[opt_idx].var)
		    last_set_संदेश(options[opt_idx].script_ctx);
		अन्यथा जर ((पूर्णांक)options[opt_idx].indir & PV_WIN)
		    last_set_संदेश(curwin->w_p_script_ctx[
			    (पूर्णांक)options[opt_idx].indir & PV_MASK]);
		अन्यथा जर ((पूर्णांक)options[opt_idx].indir & PV_BUF)
		    last_set_संदेश(curbuf->b_p_script_ctx[
			    (पूर्णांक)options[opt_idx].indir & PV_MASK]);
	    }
#endif
	}
	अन्यथा
	{
	    char_u	    *p;

	    p = find_termcode(किल्ली_name);
	    जर (p == रिक्त)
	    {
		errmsg = e_किल्ली_code_not_set;
		जा skip;
	    }
	    अन्यथा
		(शून्य)show_one_termcode(किल्ली_name, p, TRUE);
	}
	जर (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    अन्यथा
    {
	errmsg = do_set_option_मूल्य(opt_idx, opt_flags, &arg, prefix, op,
					flags, varp, किल्ली_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    परत errmsg;
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
    पूर्णांक
do_set(
    char_u	*arg_start,	// option string (may be written to!)
    पूर्णांक		opt_flags)
{
    char_u	*arg = arg_start;
    पूर्णांक		i;
    पूर्णांक		did_show = FALSE;   // already showed one value

    जर (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	जा theend;
    }

    पर्यंत (*arg != NUL)		// loop to process all options
    {
	जर (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    जर (*arg == '&')
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
	अन्यथा जर (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	अन्यथा
	{
	    पूर्णांक		stopopteval = FALSE;
	    अक्षर	*errmsg = रिक्त;
	    अक्षर	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_set_option(opt_flags, &arg, arg_start, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    जर (stopopteval)
		थांबा;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    साठी (i = 0; i < 2 ; ++i)
	    {
		पर्यंत (*arg != NUL && !VIM_ISWHITE(*arg))
		    जर (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		जर (*arg != '=')
		    थांबा;
	    }

	    जर (errmsg != रिक्त)
	    {
		i = vim_snprintf((अक्षर *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		जर (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((अक्षर *)IObuff);	// show error highlighted
		--no_wait_return;

		परत FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    जर (silent_mode && did_show)
    {
	// After displaying option values in silent mode.
	silent_mode = FALSE;
	info_message = TRUE;	// use mch_msg(), not mch_errmsg()
	संदेश_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = TRUE;
	info_message = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    परत OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    शून्य
did_set_option(
    पूर्णांक	    opt_idx,
    पूर्णांक	    opt_flags,	    // possibly with OPT_MODELINE
    पूर्णांक	    new_मूल्य_2,	    // value was replaced completely
    पूर्णांक	    मूल्य_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = रिक्त;
    पूर्णांक		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_SET;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    जर (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    जर (!मूल्य_checked && (secure
#ifdef HAVE_SANDBOX
	    || sandbox != 0
#endif
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	जर (flagsp_local != रिक्त)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    अन्यथा जर (new_मूल्य_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	जर (flagsp_local != रिक्त)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    पूर्णांक
string_to_किल्ली(char_u *arg, पूर्णांक multi_byte)
{
    जर (*arg == '<' && arg[1])
	परत find_किल्ली_option(arg + 1, TRUE);
    जर (*arg == '^' && arg[1])
    {
	पूर्णांक किल्ली = Ctrl_chr(arg[1]);
	जर (किल्ली == 0)		// ^@ is <Nul>
	    किल्ली = K_ZERO;
	परत किल्ली;
    }
    जर (multi_byte)
	परत PTR2CHAR(arg);
    परत *arg;
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
    जर (starting != NO_SCREEN
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
    पूर्णांक		oldval,
    पूर्णांक		newval,
    पूर्णांक		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    जर (newval)
    {
	जर (!oldval)		// switched on
	{
	    जर (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    जर (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	जर (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	जर (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    अन्यथा जर (oldval)		// switched off
    {
	जर (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	जर (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#जर defined(FEAT_EVAL)
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
option_expand(पूर्णांक opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    जर (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == रिक्त)
	परत रिक्त;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    जर (val != रिक्त && STRLEN(val) > MAXPATHL)
	परत रिक्त;

    जर (val == रिक्त)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    पूर्णांक esc = var == &p_tags || var == &p_path;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : रिक्त, FALSE,
#ifdef FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#endif
				  रिक्त);
    जर (STRCMP(NameBuff, val) == 0)   // they are the same
	परत रिक्त;

    परत NameBuff;
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
    पूर्णांक		opt_idx;

    साठी (opt_idx = 0; options[opt_idx].fullname != रिक्त; opt_idx++)
	जर ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != रिक्त)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    पूर्णांक
get_term_opt_idx(char_u **p)
{
    पूर्णांक opt_idx;

    साठी (opt_idx = 1; options[opt_idx].fullname != रिक्त; opt_idx++)
	जर (options[opt_idx].var == (char_u *)p)
	    परत opt_idx;
    परत -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    पूर्णांक
set_term_option_alloced(char_u **p)
{
    पूर्णांक		opt_idx = get_term_opt_idx(p);

    जर (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    परत opt_idx;
}

#जर defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    पूर्णांक
was_set_insecurely(win_T *wp, char_u *opt, पूर्णांक opt_flags)
{
    पूर्णांक	    idx = findoption(opt);
    long_u  *flagp;

    जर (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	परत (*flagp & P_INSECURE) != 0;
    }
    internal_error("was_set_insecurely()");
    परत -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    स्थिर long_u *
insecure_flag(win_T *wp, पूर्णांक opt_idx, पूर्णांक opt_flags)
{
    जर (opt_flags & OPT_LOCAL)
	निवड ((पूर्णांक)options[opt_idx].indir)
	{
	    स्थिती PV_WRAP:	परत &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    स्थिती PV_STL:	परत &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    स्थिती PV_FDE:	परत &wp->w_p_fde_flags;
	    स्थिती PV_FDT:	परत &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    स्थिती PV_BEXPR:	परत &wp->w_buffer->b_p_bexpr_flags;
#  endif
	    स्थिती PV_INDE:	परत &wp->w_buffer->b_p_inde_flags;
	    स्थिती PV_FEX:	परत &wp->w_buffer->b_p_fex_flags;
#  ifdef FEAT_FIND_ID
	    स्थिती PV_INEX:	परत &wp->w_buffer->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    स्थिती PV_CPT:	परत &wp->w_buffer->b_p_cpt_flags;
#  endif
# endif
	}
    अन्यथा
	// For global value of window-local options, use flags in w_allbuf_opt.
	निवड ((पूर्णांक)options[opt_idx].indir)
	{
	    स्थिती PV_WRAP:	परत &wp->w_allbuf_opt.wo_wrap_flags;
# जर defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    स्थिती PV_FDE:	परत &wp->w_allbuf_opt.wo_fde_flags;
	    स्थिती PV_FDT:	परत &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    परत &options[opt_idx].flags;
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
#जर defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    पूर्णांक
valid_name(char_u *val, अक्षर *allowed)
{
    char_u *s;

    साठी (s = val; *s != NUL; ++s)
	जर (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == रिक्त)
	    परत FALSE;
    परत TRUE;
}

#जर defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    शून्य
set_option_sctx_idx(पूर्णांक opt_idx, पूर्णांक opt_flags, sctx_T script_ctx)
{
    पूर्णांक		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    पूर्णांक		indir = (पूर्णांक)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    जर (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    जर (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    जर (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    जर (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    जर (both || (opt_flags & OPT_LOCAL))
    {
	जर (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	अन्यथा जर (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	    जर (both)
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
is_option_मूल्य_vim9(पूर्णांक opt_idx, पूर्णांक opt_flags)
{
    पूर्णांक		indir = (पूर्णांक)options[opt_idx].indir;
    sctx_T	*sctx = रिक्त;

    जर ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    जर ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	जर (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_MASK];
	अन्यथा जर (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_MASK];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    परत !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_version >= SCRIPT_VERSION_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(अक्षर *name)
{
    पूर्णांक idx = findoption((char_u *)name);

    जर (idx >= 0)
	परत &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    परत रिक्त;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    शून्य
set_term_option_sctx_idx(अक्षर *name, पूर्णांक opt_idx)
{
    char_u  buf[5];
    पूर्णांक	    idx;

    जर (name == रिक्त)
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
    जर (idx >= 0)
	set_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#endif

#जर defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    स्थिर शून्य
apply_optionset_autocmd(
	पूर्णांक	opt_idx,
	दीर्घ	opt_flags,
	दीर्घ	oldval,
	दीर्घ	oldval_g,
	दीर्घ	newval,
	अक्षर	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    माप_प्रकार  buf_oldlen;
    माप_प्रकार  len;

    // Don't do this while starting up, failure or recursively.
    जर (starting || errmsg != रिक्त || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	परत;

    len = vim_snprintf_safelen((अक्षर *)buf_new, आकार(buf_new), "%ld", newval);
    set_vim_var_string(VV_OPTION_NEW, buf_new, (पूर्णांक)len);
    len = vim_snprintf_safelen((अक्षर *)buf_type, आकार(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (पूर्णांक)len);

    buf_oldlen = vim_snprintf_safelen((अक्षर *)buf_old, आकार(buf_old), "%ld", oldval);
    set_vim_var_string(VV_OPTION_OLD, buf_old, (पूर्णांक)buf_oldlen);

    जर (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (पूर्णांक)buf_oldlen);
    }
    जर (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (पूर्णांक)buf_oldlen);
    }
    जर ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	माप_प्रकार	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((अक्षर *)buf_old_global,
	    आकार(buf_old_global), "%ld", oldval_g);

	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (पूर्णांक)buf_oldlen);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (पूर्णांक)buf_old_globallen);
    }
    जर (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (पूर्णांक)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    रिक्त, FALSE, रिक्त);
    reset_v_option_vars();
}
#endif

#जर defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    अक्षर *
did_set_arabic(optset_T *args UNUSED)
{
    अक्षर *errmsg = रिक्त;

    जर (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	जर (!p_tbidi)
	{
	    // set rightleft mode
	    जर (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		changed_window_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    जर (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	जर (STRCMP(p_enc, "utf-8") != 0)
	{
	    स्थिर अक्षर *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    संदेश_source(HL_ATTR(HLF_W));
	    संदेश_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = set_option_मूल्य((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    अन्यथा
    {
	// 'arabic' is reset, handle various sub-settings.
	जर (!p_tbidi)
	{
	    // reset rightleft mode
	    जर (curwin->w_p_rl)
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
	curbuf->b_p_imsearch = B_IMODE_USE_घाला;
# endif
    }

    परत errmsg;
}
#endif

#जर defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    अक्षर *
did_set_autochdir(optset_T *args UNUSED)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    परत रिक्त;
}
#endif

#जर defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    अक्षर *
did_set_ballooneval(optset_T *args)
{
    जर (balloonEvalForTerm)
	परत रिक्त;

    जर (p_beval && !args->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    अन्यथा जर (!p_beval && args->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    परत रिक्त;
}
#endif

#जर defined(FEAT_BEVAL_TERM)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    अक्षर *
did_set_balloonevalterm(optset_T *args UNUSED)
{
    mch_bevalterm_changed();
    परत रिक्त;
}
#endif

/*
 * Process the updated 'binary' option value.
 */
    अक्षर *
did_set_द्विमान(optset_T *args)
{
    // when 'bin' is set also set some other options
    set_options_bin(args->os_oldval.boolean, curbuf->b_p_bin, args->os_flags);
    redraw_titles();

    परत रिक्त;
}

/*
 * Process the updated 'buflisted' option value.
 */
    अक्षर *
did_set_buflisted(optset_T *args)
{
    // when 'buflisted' changes, trigger autocommands
    जर (args->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						रिक्त, रिक्त, TRUE, curbuf);
    परत रिक्त;
}

/*
 * Process the new 'cmdheight' option value.
 */
    अक्षर *
did_set_cmdheight(optset_T *args)
{
    दीर्घ old_मूल्य = args->os_oldval.संख्या;
    अक्षर *errmsg = रिक्त;

    // if p_ch changed value, change the command line height
    जर (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_argument_must_be_positive;
	p_ch = MIN_CMDHEIGHT;
    }
    जर (p_ch > Rows - min_rows() + MIN_CMDHEIGHT)
	p_ch = Rows - min_rows() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    जर ((p_ch != old_मूल्य
		|| tabline_height() + topframe->fr_height != Rows - p_ch)
	    && full_screen
#ifdef FEAT_GUI
	    && !gui.starting
#endif
       )
	command_height();

    परत errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    अक्षर *
did_set_compatible(optset_T *args UNUSED)
{
    compatible_set();
    परत रिक्त;
}

#जर defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    अक्षर *
did_set_conceallevel(optset_T *args UNUSED)
{
    अक्षर *errmsg = रिक्त;

    जर (curwin->w_p_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_cole = 0;
    }
    अन्यथा जर (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_cole = 3;
    }
    जर (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    अन्यथा जर (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    परत errmsg;
}
#endif

#जर defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    अक्षर *
did_set_diff(optset_T *args UNUSED)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    जर (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    परत रिक्त;
}
#endif

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    अक्षर *
did_set_eof_eol_fixeol_bomb(optset_T *args UNUSED)
{
    // redraw the window title and tab page text
    redraw_titles();
    परत रिक्त;
}

/*
 * Process the updated 'equalalways' option value.
 */
    अक्षर *
did_set_equalalways(optset_T *args)
{
    जर (p_ea && !args->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    परत रिक्त;
}

#जर defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    अक्षर *
did_set_foldcolumn(optset_T *args UNUSED)
{
    अक्षर *errmsg = रिक्त;

    जर (curwin->w_p_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_fdc = 0;
    }
    अन्यथा जर (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_fdc = 12;
    }
    जर (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    अन्यथा जर (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    परत errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    अक्षर *
did_set_foldlevel(optset_T *args UNUSED)
{
    जर (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    परत रिक्त;
}

/*
 * Process the new 'foldminlines' option value.
 */
    अक्षर *
did_set_foldminlines(optset_T *args UNUSED)
{
    foldUpdateAll(curwin);
    परत रिक्त;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    अक्षर *
did_set_foldnestmax(optset_T *args UNUSED)
{
    जर (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    परत रिक्त;
}
#endif

#जर defined(FEAT_शोध_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    अक्षर *
did_set_hlsearch(optset_T *args UNUSED)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    set_no_hlsearch(FALSE);
    परत रिक्त;
}
#endif

/*
 * Process the updated 'ignorecase' option value.
 */
    अक्षर *
did_set_ignorecase(optset_T *args UNUSED)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    जर (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    परत रिक्त;
}

#जर defined(HAVE_आदान_METHOD)
/*
 * Process the updated 'imdisable' option value.
 */
    अक्षर *
did_set_imdisable(optset_T *args UNUSED)
{
    // Only de-activate it here, it will be enabled when changing mode.
    जर (p_imdisable)
	im_set_active(FALSE);
    अन्यथा जर (State & MODE_घाला)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
    परत रिक्त;
}
#endif

/*
 * Process the new 'iminsert' option value.
 */
    अक्षर *
did_set_iminsert(optset_T *args UNUSED)
{
    अक्षर *errmsg = रिक्त;

    जर (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    जर (termcap_active)	// don't do this in the alternate screen
	showmode();
#जर defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif

    परत errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    अक्षर *
did_set_imsearch(optset_T *args UNUSED)
{
    अक्षर *errmsg = रिक्त;

    जर (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    परत errmsg;
}

#जर defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    अक्षर *
did_set_imstyle(optset_T *args UNUSED)
{
    अक्षर *errmsg = रिक्त;

    जर (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_argument;

    परत errmsg;
}
#endif

/*
 * Process the updated 'insertmode' option value.
 */
    अक्षर *
did_set_insertmode(optset_T *args)
{
    // when 'insertmode' is set from an autocommand need to do work here
    जर (p_im)
    {
	जर ((State & MODE_घाला) == 0)
	    need_start_insertmode = TRUE;
	stop_घाला_mode = FALSE;
    }
    // only reset if it was set previously
    अन्यथा जर (args->os_oldval.boolean)
    {
	need_start_insertmode = FALSE;
	stop_घाला_mode = TRUE;
	जर (restart_edit != 0 && mode_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    परत रिक्त;
}

#जर defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    अक्षर *
did_set_langnoremap(optset_T *args UNUSED)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    परत रिक्त;
}

/*
 * Process the updated 'langremap' option value.
 */
    अक्षर *
did_set_langremap(optset_T *args UNUSED)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    परत रिक्त;
}
#endif

/*
 * Process the new 'laststatus' option value.
 */
    अक्षर *
did_set_laststatus(optset_T *args UNUSED)
{
    last_status(FALSE);	// (re)set last window status line
    परत रिक्त;
}

#जर defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    अक्षर *
did_set_linespace(optset_T *args UNUSED)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    जर (gui.in_use && gui_mch_adjust_charheight() == OK)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    परत रिक्त;
}
#endif

/*
 * Process the updated 'lisp' option value.
 */
    अक्षर *
did_set_lisp(optset_T *args UNUSED)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (शून्य)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    परत रिक्त;
}

/*
 * Process the new 'maxcombine' option value.
 */
    अक्षर *
did_set_maxcombine(optset_T *args UNUSED)
{
    जर (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    अन्यथा जर (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    परत रिक्त;
}

/*
 * Process the updated 'modifiable' option value.
 */
    अक्षर *
did_set_modifiable(optset_T *args UNUSED)
{
    // when 'modifiable' is changed, redraw the window title

#ifdef FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    जर (curbuf->b_p_ma && (term_in_normal_mode(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_term != रिक्त && !term_is_finished(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	परत e_cannot_make_terminal_with_running_job_modifiable;
    }
#endif
    redraw_titles();

    परत रिक्त;
}

/*
 * Process the updated 'modified' option value.
 */
    अक्षर *
did_set_modified(optset_T *args)
{
    जर (!args->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_set = !!args->os_newval.boolean;
    परत रिक्त;
}

#जर defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    अक्षर *
did_set_mousehide(optset_T *args UNUSED)
{
    जर (!p_mh)
	gui_mch_mousehide(FALSE);
    परत रिक्त;
}
#endif

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    अक्षर *
did_set_संख्या_relativenumber(optset_T *args UNUSED)
{
#जर defined(FEAT_SIGNS) && defined(FEAT_GUI)
    जर (gui.in_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != रिक्त)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	जर (!(curwin->w_p_nu && ((पूर्णांक *)args->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#endif
    परत रिक्त;
}

#जर defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    अक्षर *
did_set_numberwidth(optset_T *args UNUSED)
{
    अक्षर *errmsg = रिक्त;

    // 'numberwidth' must be positive
    जर (curwin->w_p_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_nuw = 1;
    }
    अन्यथा जर (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_nuw = 20;
    }
    जर (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    अन्यथा जर (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_ओळ_मोजणी = 0; // trigger a redraw

    परत errmsg;
}
#endif

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    अक्षर *
did_set_osctimeoutlen(optset_T *args)
{
    जर (p_ost < 0)
    {
	p_ost = args->os_oldval.संख्या;
	परत e_argument_must_be_positive;
    }

    परत रिक्त;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    अक्षर *
did_set_paste(optset_T *args UNUSED)
{
    स्थिर पूर्णांक	old_p_paste = FALSE;
    स्थिर पूर्णांक	save_sm = 0;
    स्थिर पूर्णांक	save_sta = 0;
    स्थिर पूर्णांक	save_ru = 0;
#ifdef FEAT_RIGHTLEFT
    स्थिर पूर्णांक	save_ri = 0;
    स्थिर पूर्णांक	save_hkmap = 0;
#endif
    buf_T	*buf;

    जर (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	जर (!old_p_paste)
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
		जर (buf->b_p_vsts_nopaste)
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
	    जर (p_vsts_nopaste)
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
	    जर (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = empty_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#endif
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	जर (p_ru)
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
	जर (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = empty_option;
#endif
    }

    // Paste switched from on to off: Restore saved values.
    अन्यथा जर (old_p_paste)
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
	    जर (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : empty_option;
	    vim_free(buf->b_p_vsts_array);
	    जर (buf->b_p_vsts && buf->b_p_vsts != empty_option)
		(शून्य)tabstop_set(buf->b_p_vsts, &buf->b_p_vsts_array);
	    अन्यथा
		buf->b_p_vsts_array = रिक्त;
#endif
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	जर (p_ru != save_ru)
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
	जर (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : empty_option;
#endif
    }

    old_p_paste = p_paste;

#जर defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#endif

    परत रिक्त;
}


#ifdef FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    अक्षर *
did_set_previewwindow(optset_T *args UNUSED)
{
    जर (!curwin->w_p_pvw)
	परत रिक्त;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	जर (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    परत e_preview_window_already_exists;
	}

    परत रिक्त;
}
#endif

#जर defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    अक्षर *
did_set_pyxversion(optset_T *args UNUSED)
{
    अक्षर *errmsg = रिक्त;

    जर (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_argument;

    परत errmsg;
}
#endif

/*
 * Process the updated 'readonly' option value.
 */
    अक्षर *
did_set_readonly(optset_T *args)
{
    // when 'readonly' is reset globally, also reset readonlymode
    जर (!curbuf->b_p_ro && (args->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    जर (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    परत रिक्त;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    अक्षर *
did_set_scrollbind(optset_T *args UNUSED)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    जर (!curwin->w_p_scb)
	परत रिक्त;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    परत रिक्त;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    अक्षर *
did_set_maxsearchcount(optset_T *args UNUSED)
{
    अक्षर	*errmsg = रिक्त;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#define MAX_शोध_मोजणी 9999

    जर (p_msc <= 0)
	errmsg = e_argument_must_be_positive;
    अन्यथा जर (p_msc > MAX_शोध_मोजणी)
	errmsg = e_invalid_argument;

    जर (errmsg != रिक्त)
	p_msc = 99;

    परत errmsg;
#undef MAX_शोध_मोजणी
}

#जर defined(BACKSLASH_IN_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    अक्षर *
did_set_shellslash(optset_T *args UNUSED)
{
    जर (p_ssl)
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
    परत रिक्त;
}
#endif

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    अक्षर *
did_set_shiftwidth_tabstop(optset_T *args)
{
    दीर्घ	*pp = (दीर्घ *)args->os_varp;
    अक्षर	*errmsg = रिक्त;

    जर (curbuf->b_p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_ts = 8;
    }
    अन्यथा जर (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_ts = 8;
    }
    जर (p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ts = 8;
    }
    अन्यथा जर (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	p_ts = 8;
    }

    जर (curbuf->b_p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_मोजणी(curbuf->b_p_vts_array) > 0
		       ? tabstop_first(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#अन्यथा
	curbuf->b_p_sw = curbuf->b_p_ts;
#endif
    }
    जर (p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_मोजणी(curbuf->b_p_vts_array) > 0
	     ? tabstop_first(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#अन्यथा
	p_sw = curbuf->b_p_ts;
#endif
    }

#ifdef FEAT_FOLDING
    जर (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#endif
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    जर (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    परत errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    अक्षर *
did_set_showtabline(optset_T *args UNUSED)
{
    shell_new_rows();	// recompute window positions and heights
    परत रिक्त;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    अक्षर *
did_set_smoothscroll(optset_T *args UNUSED)
{
    जर (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    परत रिक्त;
}

#जर defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    अक्षर *
did_set_spell(optset_T *args UNUSED)
{
    जर (curwin->w_p_spell)
	परत parse_spelllang(curwin);

    परत रिक्त;
}
#endif

/*
 * Process the updated 'swapfile' option value.
 */
    अक्षर *
did_set_swapfile(optset_T *args UNUSED)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    जर (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    अन्यथा
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    परत रिक्त;
}

#जर defined(FEAT_TERMGUICOLORS)
    अक्षर *
did_set_termguicolors(optset_T *args UNUSED)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    जर (
#  ifdef VIMDLL
	    !gui.in_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_set = TRUE;
	p_tgc = 0;
	परत e_24_bit_colors_are_not_supported_on_this_environment;
    }
    जर (is_term_win32())
	swap_tcap();
# endif
# ifdef FEAT_GUI
    जर (!gui.in_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    जर (is_term_win32())
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

    परत रिक्त;
}
#endif

    अक्षर *
did_set_termsync(optset_T *args UNUSED)
{
    जर (!p_tsy)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);
    परत रिक्त;
}

#जर defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    अक्षर *
did_set_termwinscroll(optset_T *args)
{
    दीर्घ	*pp = (दीर्घ *)args->os_varp;
    अक्षर	*errmsg = रिक्त;

    जर (*pp < 1)
    {
	errmsg = e_argument_must_be_positive;
	*pp = 1;
    }

    परत errmsg;
}
#endif

/*
 * Process the updated 'terse' option value.
 */
    अक्षर *
did_set_terse(optset_T *args UNUSED)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_शोध);

    // insert 's' in p_shm
    जर (p_terse && p == रिक्त)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    अन्यथा जर (!p_terse && p != रिक्त)
	STRMOVE(p, p + 1);
    परत रिक्त;
}

/*
 * Process the updated 'textauto' option value.
 */
    अक्षर *
did_set_textauto(optset_T *args)
{
    // when 'textauto' is set or reset also change 'fileformats'
    set_string_option_direct((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | args->os_flags, 0);

    परत रिक्त;
}

/*
 * Process the updated 'textmode' option value.
 */
    अक्षर *
did_set_textmode(optset_T *args)
{
    // when 'textmode' is set or reset also change 'fileformat'
    set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, args->os_flags);

    परत रिक्त;
}

/*
 * Process the new 'textwidth' option value.
 */
    अक्षर *
did_set_textwidth(optset_T *args UNUSED)
{
    अक्षर *errmsg = रिक्त;

    जर (curbuf->b_p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_tw = 0;
    }
    जर (p_tw < 0)
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

    परत errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    अक्षर *
did_set_title_icon(optset_T *args UNUSED)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_set_title();
    परत रिक्त;
}

/*
 * Process the new 'titlelen' option value.
 */
    अक्षर *
did_set_titlelen(optset_T *args)
{
    दीर्घ old_मूल्य = args->os_oldval.संख्या;
    अक्षर *errmsg = रिक्त;

    // if 'titlelen' has changed, redraw the title
    जर (p_titlelen < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_titlelen = 85;
    }
    जर (starting != NO_SCREEN && old_मूल्य != p_titlelen)
	need_maketitle = TRUE;

    परत errmsg;
}

#जर defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    अक्षर *
did_set_undofile(optset_T *args)
{
    // Only take action when the option was set.
    जर (!curbuf->b_p_udf && !p_udf)
	परत रिक्त;

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
	जर ((curbuf == save_curbuf
		    || (args->os_flags & OPT_GLOBAL)
		    || args->os_flags == 0)
		&& !curbufIsChanged() && curbuf->b_ml.ml_mfp != रिक्त)
	{
# ifdef FEAT_CRYPT
	    जर (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
		सुरूठेवा;
# endif
	    u_compute_hash(hash);
	    u_read_undo(रिक्त, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    परत रिक्त;
}
#endif

/*
 * Process the new global 'undolevels' option value.
 */
    स्थिर शून्य
update_global_undolevels(दीर्घ मूल्य, दीर्घ old_मूल्य)
{
    // sync undo before 'undolevels' changes

    // use the old value, otherwise u_sync() may not work properly
    p_ul = old_मूल्य;
    u_sync(TRUE);
    p_ul = मूल्य;
}

/*
 * Process the new buffer local 'undolevels' option value.
 */
    स्थिर शून्य
update_buflocal_undolevels(दीर्घ मूल्य, दीर्घ old_मूल्य)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_मूल्य;
    u_sync(TRUE);
    curbuf->b_p_ul = मूल्य;
}

/*
 * Process the new 'undolevels' option value.
 */
    अक्षर *
did_set_undolevels(optset_T *args)
{
    दीर्घ *pp = (दीर्घ *)args->os_varp;

    जर (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(args->os_newval.संख्या,
						       args->os_oldval.संख्या);
    अन्यथा जर (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(args->os_newval.संख्या,
						       args->os_oldval.संख्या);

    परत रिक्त;
}

/*
 * Process the new 'updatecount' option value.
 */
    अक्षर *
did_set_updatecount(optset_T *args)
{
    दीर्घ old_मूल्य = args->os_oldval.संख्या;
    अक्षर *errmsg = रिक्त;

    // when 'updatecount' changes from zero to non-zero, open swap files
    जर (p_uc < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_uc = 100;
    }
    जर (p_uc && !old_मूल्य)
	ml_open_files();

    परत errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    अक्षर *
did_set_weirdinvert(optset_T *args)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    जर (p_wiv && !args->os_oldval.boolean)
	T_XS = (char_u *)"y";
    अन्यथा जर (!p_wiv && args->os_oldval.boolean)
	T_XS = empty_option;
    p_wiv = (*T_XS != NUL);

    परत रिक्त;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    अक्षर *
did_set_wildchar(optset_T *args)
{
    दीर्घ c = *(दीर्घ *)args->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    जर (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	परत e_invalid_argument;

    परत रिक्त;
}

/*
 * Process the new 'window' option value.
 */
    अक्षर *
did_set_window(optset_T *args UNUSED)
{
    जर (p_window < 1)
	p_window = 1;
    अन्यथा जर (p_window >= Rows)
	p_window = Rows - 1;
    परत रिक्त;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    अक्षर *
did_set_winheight_helpheight(optset_T *args)
{
    दीर्घ	*pp = (दीर्घ *)args->os_varp;
    अक्षर	*errmsg = रिक्त;

    जर (p_wh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wh = 1;
    }
    जर (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    जर (p_hh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hh = 0;
    }

    // Change window height NOW
    जर (!ONE_WINDOW)
    {
	जर (pp == &p_wh && curwin->w_height < p_wh)
	    win_setheight((पूर्णांक)p_wh);
	जर (pp == &p_hh && curbuf->b_help && curwin->w_height < p_hh)
	    win_setheight((पूर्णांक)p_hh);
    }

    परत errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    अक्षर *
did_set_winminheight(optset_T *args UNUSED)
{
    अक्षर *errmsg = रिक्त;

    जर (p_wmh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmh = 0;
    }
    जर (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    परत errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    अक्षर *
did_set_winminwidth(optset_T *args UNUSED)
{
    अक्षर *errmsg = रिक्त;

    जर (p_wmw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmw = 0;
    }
    जर (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    परत errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    अक्षर *
did_set_winwidth(optset_T *args UNUSED)
{
    अक्षर *errmsg = रिक्त;

    जर (p_wiw < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wiw = 1;
    }
    जर (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    जर (!ONE_WINDOW && curwin->w_width < p_wiw)
	win_setwidth((पूर्णांक)p_wiw);

    परत errmsg;
}

#जर defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    अक्षर *
did_set_wlsteal(optset_T *args UNUSED)
{
    clip_reset_wayland();

    परत रिक्त;
}
#endif

#जर defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    अक्षर *
did_set_wltimeoutlen(optset_T *args)
{
    जर (p_wtm < 0)
    {
	p_wtm = args->os_oldval.संख्या;
	परत e_argument_must_be_positive;
    }

    परत रिक्त;
}
#endif

/*
 * Process the updated 'wrap' option value.
 */
    अक्षर *
did_set_wrap(optset_T *args UNUSED)
{
    // Set w_leftcol or w_skipcol to zero.
    जर (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    अन्यथा
	curwin->w_skipcol = 0;

    परत रिक्त;
}

#ifdef FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    अक्षर *
did_set_xhistory(optset_T *args)
{
    पूर्णांक is_p_chi = (दीर्घ*)args->os_varp == &p_chi;
    पूर्णांक err;
    दीर्घ *arg = (is_p_chi) ? &p_chi :(दीर्घ*)args->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    जर (*arg < 1)
    {
	*arg = args->os_oldval.संख्या;
	परत e_cannot_have_negative_or_zero_संख्या_of_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    जर (*arg > 100)
    {
	*arg = args->os_oldval.संख्या;
	परत e_cannot_have_more_than_hundred_quickfix;
    }

    जर (is_p_chi)
	err = qf_resize_चळत(*arg);
    अन्यथा
	err = ll_resize_चळत(curwin, *arg);

    जर (err == FAIL)
	परत e_failed_resizing_quickfix_चळत;

    परत रिक्त;
}
#endif

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    स्थिर अक्षर *
set_bool_option(
    पूर्णांक		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    पूर्णांक		मूल्य,			// new value
    पूर्णांक		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    पूर्णांक		old_मूल्य = *(पूर्णांक *)varp;
#जर defined(FEAT_EVAL)
    पूर्णांक		old_global_मूल्य = 0;
#endif
    अक्षर	*errmsg = रिक्त;

    // Disallow changing some options from secure mode
    जर ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	परत e_not_allowed_here;

#जर defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    जर ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_मूल्य = *(पूर्णांक *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *(पूर्णांक *)varp = मूल्य;	    // set the new value
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    जर ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(पूर्णांक *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = मूल्य;

    // Handle side effects of changing a bool option.
    जर (options[opt_idx].opt_did_set_cb != रिक्त)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.boolean = old_मूल्य;
	args.os_newval.boolean = मूल्य;
	args.os_errbuf = रिक्त;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
	जर (errmsg != रिक्त)
	    परत errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_SET;

#जर defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(दीर्घ)(old_मूल्य ? TRUE : FALSE),
				(दीर्घ)(old_global_मूल्य ? TRUE : FALSE),
				(दीर्घ)(मूल्य ? TRUE : FALSE), रिक्त);
#endif

    comp_col();			    // in case 'ruler' or 'showcmd' changed

    जर (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    जर ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    परत errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    स्थिर अक्षर *
check_अंक_option_bounds(
    दीर्घ	*pp,
    दीर्घ	old_मूल्य,
    दीर्घ	old_Rows,
    दीर्घ	old_Columns,
    अक्षर	*errbuf,
    माप_प्रकार	errbuflen,
    अक्षर	*errmsg)
{
    जर (Rows < min_rows_for_all_tabpages() && full_screen)
    {
	जर (errbuf != रिक्त)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_ओळी), min_rows_for_all_tabpages());
	    errmsg = errbuf;
	}
	Rows = min_rows_for_all_tabpages();
    }
    जर (Columns < MIN_COLUMNS && full_screen)
    {
	जर (errbuf != रिक्त)
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
    जर (old_Rows != Rows || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	जर (updating_screen)
	    *pp = old_मूल्य;
	अन्यथा जर (full_screen
#ifdef FEAT_GUI
		&& !gui.starting
#endif
		)
	    set_shellsize((पूर्णांक)Columns, (पूर्णांक)Rows, TRUE);
	अन्यथा
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    जर (cmdline_row > Rows - p_ch && Rows > p_ch)
		cmdline_row = Rows - p_ch;
	}
	जर (p_window >= Rows || !option_was_set((char_u *)"window"))
	    p_window = Rows - 1;
    }

    जर (p_tm < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tm = 0;
    }
    जर ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_height
		    && curwin->w_height > 0))
	    && full_screen)
    {
	जर (pp == &(curwin->w_p_scr))
	{
	    जर (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_size;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	अन्यथा जर (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	अन्यथा // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_height;
    }
    जर (p_hi < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hi = 0;
    }
    अन्यथा जर (p_hi > 10000)
    {
	errmsg = e_invalid_argument;
	p_hi = 10000;
    }
    जर (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_argument;
	p_re = 0;
    }
    जर (p_report < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_report = 1;
    }
    जर ((p_sj < -100 || p_sj >= Rows) && full_screen)
    {
	जर (Rows != old_Rows)	// Rows changed, just adjust p_sj
	    p_sj = Rows / 2;
	अन्यथा
	{
	    errmsg = e_invalid_scroll_size;
	    p_sj = 1;
	}
    }
    जर (p_so < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_so = 0;
    }
    जर (p_sop < 0 && full_screen)
    {
	errmsg = e_invalid_argument;
	p_sop = 0;
    }
    जर (p_siso < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_siso = 0;
    }
    जर (p_cwh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_cwh = 1;
    }
    जर (p_ut < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ut = 2000;
    }
    जर (p_ss < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ss = 0;
    }

    परत errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    स्थिर अक्षर *
set_अंक_option(
    पूर्णांक		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    दीर्घ	मूल्य,			// new value
    अक्षर	*errbuf,		// buffer for error messages
    माप_प्रकार	errbuflen,		// length of "errbuf"
    पूर्णांक		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    अक्षर	*errmsg = रिक्त;
    दीर्घ	old_मूल्य = *(दीर्घ *)varp;
#जर defined(FEAT_EVAL)
    दीर्घ	old_global_मूल्य = 0;	// only used when setting a local and
					// global option
#endif
    दीर्घ	old_Rows = Rows;	// remember old Rows
    दीर्घ	old_Columns = Columns;	// remember old Columns
    दीर्घ	*pp = (दीर्घ *)varp;

    // Disallow changing some options from secure mode.
    जर ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	परत e_not_allowed_here;

#जर defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    जर ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_मूल्य = *(दीर्घ *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *pp = मूल्य;
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    जर ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(दीर्घ *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = मूल्य;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    जर (options[opt_idx].opt_did_set_cb != रिक्त)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.संख्या = old_मूल्य;
	args.os_newval.संख्या = मूल्य;
	args.os_errbuf = रिक्त;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
    }

    // Check the bounds for numeric options here
    errmsg = check_अंक_option_bounds(pp, old_मूल्य, old_Rows, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_SET;

#जर defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_मूल्य, old_global_मूल्य,
								मूल्य, errmsg);
#endif

    comp_col();			    // in case 'columns' or 'ls' changed

    जर (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    जर ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    परत errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    शून्य
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    पूर्णांक		doclear = (flags & P_RCLR) == P_RCLR;
    पूर्णांक		all = ((flags & P_RALL) == P_RALL || doclear);

    जर ((flags & P_RSTAT) || all)	// mark all status lines dirty
	status_redraw_all();

    जर ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	जर (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	अन्यथा
	    changed_window_setting();
    }
    जर (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    जर (doclear)
	redraw_all_later(UPD_CLEAR);
    अन्यथा जर (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    पूर्णांक
findoption(char_u *arg)
{
    पूर्णांक		    opt_idx;
    अक्षर	    *s, *p;
    स्थिर लघु    quick_tab[27] = {0, 0};	// quick access table
    पूर्णांक		    is_term_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    जर (quick_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	पूर्णांक last_opt_idx;
	साठी (last_opt_idx = 0; options[last_opt_idx].fullname != रिक्त;
		last_opt_idx++)
	    ;
	साठी (पूर्णांक tab_idx = 1; tab_idx < 27; tab_idx++)
	    quick_tab[tab_idx] = last_opt_idx;

	p = options[0].fullname;
	साठी (opt_idx = 1; opt_idx < last_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    जर (s[0] != p[0])
	    {
		जर (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		अन्यथा
		    quick_tab[Char_Ord_खालचा(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    जर (arg[0] < 'a' || arg[0] > 'z')
	परत -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    जर (is_term_opt)
	opt_idx = quick_tab[26];
    अन्यथा
	opt_idx = quick_tab[Char_Ord_खालचा(arg[0])];
    साठी (; (s = options[opt_idx].fullname) != रिक्त && s[0] == arg[0]; opt_idx++)
    {
	जर (STRCMP(arg, s) == 0)		    // match full name
	    थांबा;
    }
    जर (s != रिक्त && s[0] != arg[0])
	s = रिक्त;
    जर (s == रिक्त && !is_term_opt)
    {
	opt_idx = quick_tab[Char_Ord_खालचा(arg[0])];
	साठी ( ; options[opt_idx].fullname != रिक्त; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    जर (s != रिक्त && STRCMP(arg, s) == 0)   // match short name
		थांबा;
	    s = रिक्त;
	}
    }
    जर (s == रिक्त)
	opt_idx = -1;
    परत opt_idx;
}

#जर defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
get_option_मूल्य(
    char_u	*name,
    दीर्घ	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    पूर्णांक		*flagsp,
    पूर्णांक		scope)
{
    पूर्णांक		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    जर (opt_idx < 0)		    // option not in the table
    {
	पूर्णांक किल्ली;

	जर (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (किल्ली = find_किल्ली_option(name, FALSE)) != 0)
	{
	    char_u किल्ली_name[2];
	    char_u *p;

	    जर (flagsp != रिक्त)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    जर (किल्ली < 0)
	    {
		किल्ली_name[0] = KEY2TERMCAP0(किल्ली);
		किल्ली_name[1] = KEY2TERMCAP1(किल्ली);
	    }
	    अन्यथा
	    {
		किल्ली_name[0] = KS_किल्ली;
		किल्ली_name[1] = (किल्ली & 0xff);
	    }
	    p = find_termcode(किल्ली_name);
	    जर (p != रिक्त)
	    {
		जर (stringval != रिक्त)
		    *stringval = vim_strsave(p);
		परत gov_string;
	    }
	}
	परत gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    जर (flagsp != रिक्त)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    जर (options[opt_idx].flags & P_STRING)
    {
	जर (varp == रिक्त)		    // hidden option
	    परत gov_hidden_string;
	जर (stringval != रिक्त)
	{
	    जर ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    अन्यथा जर ((char_u **)varp == &curbuf->b_p_किल्ली
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    अन्यथा
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	परत gov_string;
    }

    जर (varp == रिक्त)		    // hidden option
	परत (options[opt_idx].flags & P_अंक)
					 ? gov_hidden_संख्या : gov_hidden_bool;
    जर (options[opt_idx].flags & P_अंक)
	*numval = *(दीर्घ *)varp;
    अन्यथा
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	जर ((पूर्णांक *)varp == &curbuf->b_changed)
	    *numval = curbufIsChanged();
	अन्यथा
	    *numval = (दीर्घ) *(पूर्णांक *)varp;
    }
    परत (options[opt_idx].flags & P_अंक) ? gov_संख्या : gov_bool;
}
#endif

#जर defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    पूर्णांक
get_option_मूल्य_strict(
    char_u	*name,
    दीर्घ	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    पूर्णांक		opt_type,
    शून्य	*from)
{
    पूर्णांक		opt_idx;
    char_u	*varp = रिक्त;
    संरचना vimoption *p;
    पूर्णांक		r = 0;

    opt_idx = findoption(name);
    जर (opt_idx < 0)
	परत 0;

    p = &(options[opt_idx]);

    // Hidden option
    जर (p->var == रिक्त)
	परत 0;

    जर (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    अन्यथा जर (p->flags & P_अंक)
	r |= SOPT_अंक;
    अन्यथा जर (p->flags & P_STRING)
	r |= SOPT_STRING;

    जर (p->indir == PV_NONE)
    {
	जर (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	अन्यथा
	    परत 0; // Did not request global-only option
    }
    अन्यथा
    {
	जर (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	अन्यथा जर (opt_type == SREQ_GLOBAL)
	    परत 0; // Requested global option

	जर (p->indir & PV_WIN)
	{
	    जर (opt_type == SREQ_BUF)
		परत 0; // Did not request window-local option
	    अन्यथा
		r |= SOPT_WIN;
	}
	अन्यथा जर (p->indir & PV_BUF)
	{
	    जर (opt_type == SREQ_WIN)
		परत 0; // Did not request buffer-local option
	    अन्यथा
		r |= SOPT_BUF;
	}
    }

    जर (stringval == रिक्त)
	परत r;

    जर (opt_type == SREQ_GLOBAL)
	varp = p->var;
    अन्यथा
    {
	जर (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    जर (p->indir == PV_MOD)
	    {
		*numval = bufIsChanged((buf_T *)from);
		varp = रिक्त;
	    }
# ifdef FEAT_CRYPT
	    अन्यथा जर (p->indir == PV_किल्ली)
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
	अन्यथा जर (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_buffer;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	}
	जर (varp == p->var)
	    परत (r | SOPT_UNSET);
    }

    जर (varp != रिक्त)
    {
	जर (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	अन्यथा जर (p->flags & P_अंक)
	    *numval = *(दीर्घ *) varp;
	अन्यथा
	    *numval = *(पूर्णांक *)varp;
    }

    परत r;
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
option_iter_next(शून्य **option, पूर्णांक opt_type)
{
    संरचना vimoption	*ret = रिक्त;
    करा
    {
	जर (*option == रिक्त)
	    *option = (शून्य *) options;
	अन्यथा जर (((संरचना vimoption *) (*option))->fullname == रिक्त)
	{
	    *option = रिक्त;
	    परत रिक्त;
	}
	अन्यथा
	    *option = (शून्य *) (((संरचना vimoption *) (*option)) + 1);

	ret = ((संरचना vimoption *) (*option));

	// Hidden option
	जर (ret->var == रिक्त)
	{
	    ret = रिक्त;
	    सुरूठेवा;
	}

	निवड (opt_type)
	{
	    स्थिती SREQ_GLOBAL:
		जर (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = रिक्त;
		थांबा;
	    स्थिती SREQ_BUF:
		जर (!(ret->indir & PV_BUF))
		    ret = रिक्त;
		थांबा;
	    स्थिती SREQ_WIN:
		जर (!(ret->indir & PV_WIN))
		    ret = रिक्त;
		थांबा;
	    पूर्वनिर्धारित:
		internal_error("option_iter_next()");
		परत रिक्त;
	}
    }
    पर्यंत (ret == रिक्त);

    परत (char_u *)ret->fullname;
}
#endif

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(पूर्णांक opt_idx)
{
    परत options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    शून्य
set_option_flag(पूर्णांक opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    पूर्णांक
is_global_option(पूर्णांक opt_idx)
{
    परत options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    पूर्णांक
is_global_local_option(पूर्णांक opt_idx)
{
    परत options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    पूर्णांक
is_window_local_option(पूर्णांक opt_idx)
{
    परत options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    पूर्णांक
is_hidden_option(पूर्णांक opt_idx)
{
    परत options[opt_idx].var == रिक्त;
}

#जर defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    पूर्णांक
is_crypt_किल्ली_option(पूर्णांक opt_idx)
{
    परत options[opt_idx].indir == PV_किल्ली;
}
#endif

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    अक्षर *
set_option_मूल्य(
    char_u	*name,
    दीर्घ	संख्या,
    char_u	*string,
    पूर्णांक		opt_flags)	// OPT_LOCAL or 0 (both)
{
    पूर्णांक		opt_idx;
    char_u	*varp;
    long_u	flags;
    स्थिर अक्षर	errbuf[ERR_BUFLEN];
    पूर्णांक		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    जर (opt_idx < 0)
    {
	पूर्णांक किल्ली;

	जर (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (किल्ली = find_किल्ली_option(name, FALSE)) != 0)
	{
	    char_u किल्ली_name[2];

	    जर (किल्ली < 0)
	    {
		किल्ली_name[0] = KEY2TERMCAP0(किल्ली);
		किल्ली_name[1] = KEY2TERMCAP1(किल्ली);
	    }
	    अन्यथा
	    {
		किल्ली_name[0] = KS_किल्ली;
		किल्ली_name[1] = (किल्ली & 0xff);
	    }
	    add_termcode(किल्ली_name, string, FALSE);
	    जर (full_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    परत रिक्त;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    अन्यथा
    {
	flags = options[opt_idx].flags;
#ifdef HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	जर (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_in_sandbox));
	    परत रिक्त;
	}
#endif
	जर (flags & P_STRING)
	    परत set_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	जर (varp != रिक्त)	// hidden option is not changed
	{
	    जर (संख्या == 0 && string != रिक्त)
	    {
		पूर्णांक idx;

		// Either we are given a string or we are setting option
		// to zero.
		साठी (idx = 0; string[idx] == '0'; ++idx)
		    ;
		जर (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_संख्या_required_after_str_equal_str),
								 name, string);
		    परत रिक्त;     // do nothing as we hit an error

		}
	    }
	    जर (flags & P_अंक)
	    {
		परत set_अंक_option(opt_idx, varp, संख्या,
					   errbuf, आकार(errbuf), opt_flags);
	    }
	    अन्यथा
		परत set_bool_option(opt_idx, varp, (पूर्णांक)संख्या, opt_flags);
	}

    }
    परत रिक्त;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    शून्य
set_option_मूल्य_give_err(
    char_u	*name,
    दीर्घ	संख्या,
    char_u	*string,
    पूर्णांक		opt_flags)	// OPT_LOCAL or 0 (both)
{
    अक्षर *errmsg = set_option_मूल्य(name, संख्या, string, opt_flags);

    जर (errmsg != रिक्त)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_term_code(char_u *tname)
{
    पूर्णांक	    opt_idx;
    char_u  *varp;

    जर (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	परत रिक्त;
    जर ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	जर (varp != रिक्त)
	    varp = *(char_u **)(varp);
	परत varp;
    }
    परत find_termcode(tname + 2);
}

    char_u *
get_highlight_default(शून्य)
{
    पूर्णांक i;

    i = findoption((char_u *)"hl");
    जर (i >= 0)
	परत options[i].def_val[VI_DEFAULT];
    परत (char_u *)रिक्त;
}

    char_u *
get_encoding_default(शून्य)
{
    पूर्णांक i;

    i = findoption((char_u *)"enc");
    जर (i >= 0)
	परत options[i].def_val[VI_DEFAULT];
    परत (char_u *)रिक्त;
}

#जर defined(FEAT_QUICKFIX)
    पूर्णांक
is_option_allocated(अक्षर *name)
{
    पूर्णांक idx = findoption((char_u *)name);

    परत idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#endif

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    स्थिर पूर्णांक
find_किल्ली_option(char_u *arg_arg, पूर्णांक has_lt)
{
    पूर्णांक		किल्ली = 0;
    पूर्णांक		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    जर (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	जर (!has_lt || arg[4] == '>')
	    किल्ली = TERMCAP2_किल्ली(arg[2], arg[3]);
    }
    अन्यथा जर (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	किल्ली = find_special_किल्ली(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_किल्ली | FSK_SIMPLIFY, रिक्त);
	जर (modifiers)		    // can't handle modifiers here
	    किल्ली = 0;
    }
    परत किल्ली;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    स्थिर शून्य
showoptions(
    पूर्णांक		all,
    पूर्णांक		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    संरचना vimoption	*p;
    पूर्णांक			col;
    पूर्णांक			isterm;
    char_u		*varp;
    संरचना vimoption	**वस्तुसंच;
    पूर्णांक			item_मोजणी;
    पूर्णांक			run;
    पूर्णांक			row, rows;
    पूर्णांक			cols;
    पूर्णांक			i;
    पूर्णांक			len;

#define INC 20
#define GAP 3

    वस्तुसंच = ALLOC_MULT(संरचना vimoption *, OPTION_मोजणी);
    जर (वस्तुसंच == रिक्त)
	परत;

    // Highlight title
    जर (all == 2)
	संदेश_puts_title(_("\n--- Terminal codes ---"));
    अन्यथा जर (opt_flags & OPT_GLOBAL)
	संदेश_puts_title(_("\n--- Global option values ---"));
    अन्यथा जर (opt_flags & OPT_LOCAL)
	संदेश_puts_title(_("\n--- Local option values ---"));
    अन्यथा
	संदेश_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    साठी (run = 1; run <= 2 && !got_int; ++run)
    {
	// collect the items in items[]
	item_मोजणी = 0;
	साठी (p = &options[0]; p->fullname != रिक्त; p++)
	{
	    // apply :filter /pat/
	    जर (message_filtered((char_u *)p->fullname))
		सुरूठेवा;

	    varp = रिक्त;
	    isterm = istermoption(p);
	    जर ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		जर (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    अन्यथा
		varp = get_varp(p);
	    जर (varp != रिक्त
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		जर (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		अन्यथा जर (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		अन्यथा
		{
		    option_value2string(p, opt_flags);
		    len = (पूर्णांक)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		जर ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    वस्तुसंच[item_मोजणी++] = p;
	    }
	}

	// display the items
	जर (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    जर (cols == 0)
		cols = 1;
	    rows = (item_मोजणी + cols - 1) / cols;
	}
	अन्यथा	// run == 2
	    rows = item_मोजणी;
	साठी (row = 0; row < rows && !got_int; ++row)
	{
	    संदेश_putchar('\n');			// go to next line
	    जर (got_int)			// 'q' typed in more
		थांबा;
	    col = 0;
	    साठी (i = row; i < item_मोजणी; i += rows)
	    {
		संदेश_col = col;			// make columns
		showoneopt(वस्तुसंच[i], opt_flags);
		col += INC;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(वस्तुसंच);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    स्थिर पूर्णांक
optval_default(संरचना vimoption *p, char_u *varp, पूर्णांक compatible)
{
    पूर्णांक		dvi;

    जर (varp == रिक्त)
	परत TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    जर (p->flags & P_अंक)
	परत (*(दीर्घ *)varp == (दीर्घ)(long_i)p->def_val[dvi]);
    जर (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	परत (*(पूर्णांक *)varp == (पूर्णांक)(दीर्घ)(long_i)p->def_val[dvi]);
    // P_STRING
    परत (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    स्थिर शून्य
showoneopt(
    संरचना vimoption	*p,
    पूर्णांक			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    पूर्णांक		save_silent = silent_mode;

    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    जर ((p->flags & P_BOOL) && ((पूर्णांक *)varp == &curbuf->b_changed
					? !curbufIsChanged() : !*(पूर्णांक *)varp))
	संदेश_puts("no");
    अन्यथा जर ((p->flags & P_BOOL) && *(पूर्णांक *)varp < 0)
	संदेश_puts("--");
    अन्यथा
	संदेश_puts("  ");
    संदेश_puts(p->fullname);
    जर (!(p->flags & P_BOOL))
    {
	संदेश_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	संदेश_outtrans(NameBuff);
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
    पूर्णांक
makeset(खाता *fd, पूर्णांक opt_flags, पूर्णांक local_only)
{
    संरचना vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = रिक्त;	// fresh value
    अक्षर		*cmd;
    पूर्णांक			round;
    पूर्णांक			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    साठी (pri = 1; pri >= 0; --pri)
    {
      साठी (p = &options[0]; !istermoption(p); p++)
	जर (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    जर (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		सुरूठेवा;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    जर ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		सुरूठेवा;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    जर ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		सुरूठेवा;

	    जर ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		सुरूठेवा;

	    round = 2;
	    जर (p->indir != PV_NONE)
	    {
		जर (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    जर (!(opt_flags & OPT_LOCAL))
			सुरूठेवा;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    जर (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			जर (!optval_default(p, varp_fresh, p_cp))
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
	    साठी ( ; round <= 2; varp = varp_local, ++round)
	    {
		जर (round == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		अन्यथा
		    cmd = "setlocal";

		जर (p->flags & P_BOOL)
		{
		    जर (put_setbool(fd, cmd, p->fullname, *(पूर्णांक *)varp) == FAIL)
			परत FAIL;
		}
		अन्यथा जर (p->flags & P_अंक)
		{
		    जर (put_setnum(fd, cmd, p->fullname, (दीर्घ *)varp) == FAIL)
			परत FAIL;
		}
		अन्यथा    // P_STRING
		{
		    पूर्णांक		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    जर ((p->flags & P_FUNC) && *(char_u **)varp != रिक्त
			    && म_माला(*(अक्षर **)varp, "<lambda>") != रिक्त)
			सुरूठेवा;

#ifdef FEAT_EVAL
		    legacy = !is_option_मूल्य_vim9(p - &options[0],
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
		    जर (
#जर defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#endif
			    p->indir == PV_FT)
		    {
			जर (ख_लिखो(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    परत FAIL;
			do_endif = TRUE;
		    }
		    जर (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			परत FAIL;
		    जर (do_endif)
		    {
			जर (put_ओळ(fd, "endif") == FAIL)
			    परत FAIL;
		    }
		}
	    }
	}
    }
    परत OK;
}

#जर defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    पूर्णांक
makefoldset(खाता *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_version < SCRIPT_VERSION_VIM9;
# endif

    जर (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	परत FAIL;

    परत OK;
}
#endif

    स्थिर पूर्णांक
put_setstring(
    खाता	*fd,
    bool	legacy,
    अक्षर	*cmd,
    अक्षर	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = रिक्त;
    char_u	*part = रिक्त;
    char_u	*p;

    जर (legacy && ख_लिखो(fd, "legacy ") < 0)
	परत FAIL;
    जर (ख_लिखो(fd, "%s %s=", cmd, name) < 0)
	परत FAIL;
    जर (*valuep != रिक्त)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	जर (valuep == &p_pt)
	{
	    s = *valuep;
	    पर्यंत (*s != NUL)
		जर (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    परत FAIL;
	}
	// expand the option value, replace $HOME by ~
	अन्यथा जर ((flags & P_EXPAND) != 0)
	{
	    पूर्णांक  size = (पूर्णांक)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(size);
	    जर (buf == रिक्त)
		जा fail;
	    home_replace(रिक्त, *valuep, buf, size, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    जर (size >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != रिक्त)
	    {
		part = alloc(size);
		जर (part == रिक्त)
		    जा fail;

		// write line break to clear the option, e.g. ':set rtp='
		जर (put_eol(fd) == FAIL)
		    जा fail;

		p = buf;
		पर्यंत (*p != NUL)
		{
		    जर (legacy && ख_लिखो(fd, "legacy ") < 0)
			परत FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    जर (ख_लिखो(fd, "%s %s+=", cmd, name) < 0)
			जा fail;
		    (शून्य)copy_option_part(&p, part, size,  ",");
		    जर (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			जा fail;
		}
		vim_free(buf);
		vim_free(part);
		परत OK;
	    }
	    जर (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		परत FAIL;
	    }
	    vim_free(buf);
	}
	अन्यथा जर (put_escstr(fd, *valuep, 2) == FAIL)
	    परत FAIL;
    }
    जर (put_eol(fd) < 0)
	परत FAIL;
    परत OK;
fail:
    vim_free(buf);
    vim_free(part);
    परत FAIL;
}

    स्थिर पूर्णांक
put_setnum(
    खाता	*fd,
    अक्षर	*cmd,
    अक्षर	*name,
    दीर्घ	*valuep)
{
    दीर्घ	wc;

    जर (ख_लिखो(fd, "%s %s=", cmd, name) < 0)
	परत FAIL;
    जर (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	जर (ख_माला_दो((अक्षर *)get_special_किल्ली_name((पूर्णांक)wc, 0), fd) < 0)
	    परत FAIL;
    }
    अन्यथा जर (ख_लिखो(fd, "%ld", *valuep) < 0)
	परत FAIL;
    जर (put_eol(fd) < 0)
	परत FAIL;
    परत OK;
}

    स्थिर पूर्णांक
put_setbool(
    खाता	*fd,
    अक्षर	*cmd,
    अक्षर	*name,
    पूर्णांक		मूल्य)
{
    जर (मूल्य < 0)	// global/local option using global value
	परत OK;
    जर (ख_लिखो(fd, "%s %s%s", cmd, मूल्य ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	परत FAIL;
    परत OK;
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
#जर defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    जर (gui.starting)
	clear_xterm_clip();
#endif
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    शून्य
free_termoptions(शून्य)
{
    संरचना vimoption   *p;

    साठी (p = options; p->fullname != रिक्त; p++)
	जर (istermoption(p))
	{
	    जर (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    जर (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = empty_option;
	    p->def_val[VI_DEFAULT] = empty_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ifdef FEAT_EVAL
	    // remember where the option was cleared
	    set_option_sctx_idx((पूर्णांक)(p - options), OPT_GLOBAL, current_sctx);
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

    साठी (p = &options[0]; p->fullname != रिक्त; p++)
	जर (p->var == var)
	{
	    जर (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = empty_option;
	    p->flags &= ~P_ALLOCED;
	    थांबा;
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

    साठी (p = &options[0]; p->fullname != रिक्त; p++)
    {
	जर (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    जर (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    जर (p->flags & P_ALLOCED)
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
    स्थिर पूर्णांक
istermoption(संरचना vimoption *p)
{
    परत (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    स्थिर पूर्णांक
istermoption_idx(पूर्णांक opt_idx)
{
    परत istermoption(&options[opt_idx]);
}

#जर defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    शून्य
unset_global_local_option(char_u *name, शून्य *from)
{
    संरचना vimoption *p;
    पूर्णांक		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    जर (opt_idx < 0)
	परत;
    p = &(options[opt_idx]);

    निवड ((पूर्णांक)p->indir)
    {
	// global option with local value: use local value if it's been set
	स्थिती PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    थांबा;
	स्थिती PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    थांबा;
	स्थिती PV_PATH:
	    clear_string_option(&buf->b_p_path);
	    थांबा;
	स्थिती PV_AC:
	    buf->b_p_ac = -1;
	    थांबा;
	स्थिती PV_AR:
	    buf->b_p_ar = -1;
	    थांबा;
# ifdef HAVE_FSYNC
	स्थिती PV_FS:
	    buf->b_p_fs = -1;
	    थांबा;
# endif
	स्थिती PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    थांबा;
	स्थिती PV_TAGS:
	    clear_string_option(&buf->b_p_tags);
	    थांबा;
	स्थिती PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    थांबा;
	स्थिती PV_SISO:
	    curwin->w_p_siso = -1;
	    थांबा;
	स्थिती PV_SO:
	    curwin->w_p_so = -1;
	    थांबा;
	स्थिती PV_SOP:
	    curwin->w_p_sop = -1;
	    थांबा;
# ifdef FEAT_FIND_ID
	स्थिती PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    थांबा;
	स्थिती PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    थांबा;
# endif
	स्थिती PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    थांबा;
	स्थिती PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    थांबा;
# ifdef FEAT_DIFF
	स्थिती PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    थांबा;
# endif
	स्थिती PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    थांबा;
# ifdef FEAT_COMPL_FUNC
	स्थिती PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    थांबा;
# endif
	स्थिती PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    थांबा;
# ifdef FEAT_EVAL
	स्थिती PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    थांबा;
# endif
# ifdef FEAT_QUICKFIX
	स्थिती PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    थांबा;
	स्थिती PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    थांबा;
	स्थिती PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    थांबा;
	स्थिती PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    थांबा;
# endif
# जर defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	स्थिती PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    थांबा;
# endif
# जर defined(FEAT_CRYPT)
	स्थिती PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    थांबा;
# endif
# ifdef FEAT_LINEBREAK
	स्थिती PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    थांबा;
# endif
# ifdef FEAT_STL_OPT
	स्थिती PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    थांबा;
	स्थिती PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    थांबा;
# endif
	स्थिती PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    थांबा;
	स्थिती PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    थांबा;
	स्थिती PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    थांबा;
	स्थिती PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    set_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      रिक्त, 0);
	    redraw_later(UPD_NOT_VALID);
	    थांबा;
	स्थिती PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    set_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      रिक्त, 0);
	    redraw_later(UPD_NOT_VALID);
	    थांबा;
	स्थिती PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    थांबा;
    }
}
#endif

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    स्थिर char_u *
get_varp_scope(संरचना vimoption *p, पूर्णांक scope)
{
    जर ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	जर (p->var == VAR_WIN)
	    परत (char_u *)GLOBAL_WO(get_varp(p));
	परत p->var;
    }
    जर ((scope & OPT_LOCAL) && ((पूर्णांक)p->indir & PV_BOTH))
    {
	निवड ((पूर्णांक)p->indir)
	{
	    स्थिती PV_FP:   परत (char_u *)&(curbuf->b_p_fp);
#ifdef HAVE_FSYNC
	    स्थिती PV_FS:	परत (char_u *)&(curbuf->b_p_fs);
#endif
#ifdef FEAT_EVAL
	    स्थिती PV_FFU: परत (char_u *)&(curbuf->b_p_ffu);
#endif
#ifdef FEAT_QUICKFIX
	    स्थिती PV_EFM:  परत (char_u *)&(curbuf->b_p_efm);
	    स्थिती PV_GEFM: परत (char_u *)&(curbuf->b_p_gefm);
	    स्थिती PV_GP:   परत (char_u *)&(curbuf->b_p_gp);
	    स्थिती PV_MP:   परत (char_u *)&(curbuf->b_p_mp);
#endif
	    स्थिती PV_EP:   परत (char_u *)&(curbuf->b_p_ep);
	    स्थिती PV_KP:   परत (char_u *)&(curbuf->b_p_kp);
	    स्थिती PV_PATH: परत (char_u *)&(curbuf->b_p_path);
	    स्थिती PV_AC:   परत (char_u *)&(curbuf->b_p_ac);
	    स्थिती PV_AR:   परत (char_u *)&(curbuf->b_p_ar);
	    स्थिती PV_TAGS: परत (char_u *)&(curbuf->b_p_tags);
	    स्थिती PV_TC:   परत (char_u *)&(curbuf->b_p_tc);
	    स्थिती PV_SISO: परत (char_u *)&(curwin->w_p_siso);
	    स्थिती PV_SO:   परत (char_u *)&(curwin->w_p_so);
	    स्थिती PV_SOP:  परत (char_u *)&(curwin->w_p_sop);
#ifdef FEAT_FIND_ID
	    स्थिती PV_DEF:  परत (char_u *)&(curbuf->b_p_def);
	    स्थिती PV_INC:  परत (char_u *)&(curbuf->b_p_inc);
#endif
	    स्थिती PV_COT:  परत (char_u *)&(curbuf->b_p_cot);
	    स्थिती PV_DICT: परत (char_u *)&(curbuf->b_p_dict);
#ifdef FEAT_DIFF
	    स्थिती PV_DIA:  परत (char_u *)&(curbuf->b_p_dia);
#endif
	    स्थिती PV_TSR:  परत (char_u *)&(curbuf->b_p_tsr);
#ifdef FEAT_COMPL_FUNC
	    स्थिती PV_TSRFU: परत (char_u *)&(curbuf->b_p_tsrfu);
#endif
#जर defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    स्थिती PV_BEXPR: परत (char_u *)&(curbuf->b_p_bexpr);
#endif
#जर defined(FEAT_CRYPT)
	    स्थिती PV_CM:	  परत (char_u *)&(curbuf->b_p_cm);
#endif
#ifdef FEAT_LINEBREAK
	    स्थिती PV_SBR:  परत (char_u *)&(curwin->w_p_sbr);
#endif
#ifdef FEAT_STL_OPT
	    स्थिती PV_STL:  परत (char_u *)&(curwin->w_p_stl);
	    स्थिती PV_STLO: परत (char_u *)&(curwin->w_p_stlo);
#endif
	    स्थिती PV_UL:   परत (char_u *)&(curbuf->b_p_ul);
	    स्थिती PV_LW:   परत (char_u *)&(curbuf->b_p_lw);
	    स्थिती PV_BKC:  परत (char_u *)&(curbuf->b_p_bkc);
	    स्थिती PV_MENC: परत (char_u *)&(curbuf->b_p_menc);
	    स्थिती PV_LCS:  परत (char_u *)&(curwin->w_p_lcs);
	    स्थिती PV_FCS:  परत (char_u *)&(curwin->w_p_fcs);
	    स्थिती PV_VE:	  परत (char_u *)&(curwin->w_p_ve);

	}
	परत रिक्त; // "cannot happen"
    }
    परत get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(पूर्णांक opt_idx, पूर्णांक scope)
{
    परत get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    स्थिर char_u *
get_varp(संरचना vimoption *p)
{
    // hidden option, always return NULL
    जर (p->var == रिक्त)
	परत रिक्त;

    निवड ((पूर्णांक)p->indir)
    {
	स्थिती PV_NONE:	परत p->var;

	// global option with local value: use local value if it's been set
	स्थिती PV_EP:	परत *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	स्थिती PV_KP:	परत *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	स्थिती PV_PATH:	परत *curbuf->b_p_path != NUL
				    ? (char_u *)&(curbuf->b_p_path) : p->var;
	स्थिती PV_AC:	परत curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	स्थिती PV_AR:	परत curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	स्थिती PV_TAGS:	परत *curbuf->b_p_tags != NUL
				    ? (char_u *)&(curbuf->b_p_tags) : p->var;
	स्थिती PV_TC:	परत *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	स्थिती PV_BKC:	परत *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	स्थिती PV_SISO:	परत curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	स्थिती PV_SO:	परत curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	स्थिती PV_SOP:	परत curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ifdef FEAT_FIND_ID
	स्थिती PV_DEF:	परत *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	स्थिती PV_INC:	परत *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#endif
	स्थिती PV_COT:	परत *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	स्थिती PV_DICT:	परत *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ifdef FEAT_DIFF
	स्थिती PV_DIA:	परत *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#endif
	स्थिती PV_TSR:	परत *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ifdef FEAT_COMPL_FUNC
	स्थिती PV_TSRFU:	परत *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#endif
	स्थिती PV_FP:	परत *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ifdef HAVE_FSYNC
	स्थिती PV_FS:	परत curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#endif
#ifdef FEAT_EVAL
	स्थिती PV_FFU:	परत *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#endif
#ifdef FEAT_QUICKFIX
	स्थिती PV_EFM:	परत *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	स्थिती PV_GEFM:	परत *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	स्थिती PV_GP:	परत *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	स्थिती PV_MP:	परत *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#endif
#जर defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	स्थिती PV_BEXPR:	परत *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#endif
#जर defined(FEAT_CRYPT)
	स्थिती PV_CM:	परत *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#endif
#ifdef FEAT_LINEBREAK
	स्थिती PV_SBR:	परत *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#endif
#ifdef FEAT_STL_OPT
	स्थिती PV_STL:	परत *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	स्थिती PV_STLO:	परत *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#endif
	स्थिती PV_UL:	परत curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	स्थिती PV_LW:	परत *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	स्थिती PV_MENC:	परत *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ifdef FEAT_ARABIC
	स्थिती PV_ARAB:	परत (char_u *)&(curwin->w_p_arab);
#endif
	स्थिती PV_LIST:	परत (char_u *)&(curwin->w_p_list);
	स्थिती PV_LCS:	परत *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	स्थिती PV_FCS:	परत *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	स्थिती PV_VE:	परत *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ifdef FEAT_SPELL
	स्थिती PV_SPELL:	परत (char_u *)&(curwin->w_p_spell);
#endif
#ifdef FEAT_SYN_HL
	स्थिती PV_CUC:	परत (char_u *)&(curwin->w_p_cuc);
	स्थिती PV_CUL:	परत (char_u *)&(curwin->w_p_cul);
	स्थिती PV_CULOPT:	परत (char_u *)&(curwin->w_p_culopt);
	स्थिती PV_CC:	परत (char_u *)&(curwin->w_p_cc);
#endif
#ifdef FEAT_DIFF
	स्थिती PV_DIFF:	परत (char_u *)&(curwin->w_p_diff);
#endif
	स्थिती PV_EIW:	परत (char_u *)&(curwin->w_p_eiw);
#ifdef FEAT_FOLDING
	स्थिती PV_FDC:	परत (char_u *)&(curwin->w_p_fdc);
	स्थिती PV_FEN:	परत (char_u *)&(curwin->w_p_fen);
	स्थिती PV_FDI:	परत (char_u *)&(curwin->w_p_fdi);
	स्थिती PV_FDL:	परत (char_u *)&(curwin->w_p_fdl);
	स्थिती PV_FDM:	परत (char_u *)&(curwin->w_p_fdm);
	स्थिती PV_FML:	परत (char_u *)&(curwin->w_p_fml);
	स्थिती PV_FDN:	परत (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	स्थिती PV_FDE:	परत (char_u *)&(curwin->w_p_fde);
	स्थिती PV_FDT:	परत (char_u *)&(curwin->w_p_fdt);
# endif
	स्थिती PV_FMR:	परत (char_u *)&(curwin->w_p_fmr);
#endif
	स्थिती PV_NU:	परत (char_u *)&(curwin->w_p_nu);
	स्थिती PV_RNU:	परत (char_u *)&(curwin->w_p_rnu);
#ifdef FEAT_LINEBREAK
	स्थिती PV_NUW:	परत (char_u *)&(curwin->w_p_nuw);
#endif
	स्थिती PV_WFB:	परत (char_u *)&(curwin->w_p_wfb);
	स्थिती PV_WFH:	परत (char_u *)&(curwin->w_p_wfh);
	स्थिती PV_WFW:	परत (char_u *)&(curwin->w_p_wfw);
#जर defined(FEAT_QUICKFIX)
	स्थिती PV_PVW:	परत (char_u *)&(curwin->w_p_pvw);
	स्थिती PV_LHI:	परत (char_u *)&(curwin->w_p_lhi);
#endif
#ifdef FEAT_RIGHTLEFT
	स्थिती PV_RL:	परत (char_u *)&(curwin->w_p_rl);
	स्थिती PV_RLC:	परत (char_u *)&(curwin->w_p_rlc);
#endif
	स्थिती PV_SCROLL:	परत (char_u *)&(curwin->w_p_scr);
	स्थिती PV_SMS:	परत (char_u *)&(curwin->w_p_sms);
	स्थिती PV_WRAP:	परत (char_u *)&(curwin->w_p_wrap);
#ifdef FEAT_LINEBREAK
	स्थिती PV_LBR:	परत (char_u *)&(curwin->w_p_lbr);
	स्थिती PV_BRI:	परत (char_u *)&(curwin->w_p_bri);
	स्थिती PV_BRIOPT: परत (char_u *)&(curwin->w_p_briopt);
#endif
	स्थिती PV_WCR:	परत (char_u *)&(curwin->w_p_wcr);
	स्थिती PV_SCBIND: परत (char_u *)&(curwin->w_p_scb);
	स्थिती PV_CRBIND: परत (char_u *)&(curwin->w_p_crb);
#ifdef FEAT_CONCEAL
	स्थिती PV_COCU:   परत (char_u *)&(curwin->w_p_cocu);
	स्थिती PV_COLE:   परत (char_u *)&(curwin->w_p_cole);
#endif
	स्थिती PV_WHL:	परत (char_u *)&(curwin->w_p_whl);

#ifdef FEAT_TERMINAL
	स्थिती PV_TWK:    परत (char_u *)&(curwin->w_p_twk);
	स्थिती PV_TWS:    परत (char_u *)&(curwin->w_p_tws);
	स्थिती PV_TWSL:	परत (char_u *)&(curbuf->b_p_twsl);
#endif

	स्थिती PV_AI:	परत (char_u *)&(curbuf->b_p_ai);
	स्थिती PV_BIN:	परत (char_u *)&(curbuf->b_p_bin);
	स्थिती PV_BOMB:	परत (char_u *)&(curbuf->b_p_bomb);
	स्थिती PV_BH:	परत (char_u *)&(curbuf->b_p_bh);
	स्थिती PV_BT:	परत (char_u *)&(curbuf->b_p_bt);
	स्थिती PV_BL:	परत (char_u *)&(curbuf->b_p_bl);
	स्थिती PV_CI:	परत (char_u *)&(curbuf->b_p_ci);
	स्थिती PV_CIN:	परत (char_u *)&(curbuf->b_p_cin);
	स्थिती PV_CINK:	परत (char_u *)&(curbuf->b_p_cink);
	स्थिती PV_CINO:	परत (char_u *)&(curbuf->b_p_cino);
	स्थिती PV_CINSD:	परत (char_u *)&(curbuf->b_p_cinsd);
	स्थिती PV_CINW:	परत (char_u *)&(curbuf->b_p_cinw);
	स्थिती PV_COM:	परत (char_u *)&(curbuf->b_p_com);
	स्थिती PV_CMS:	परत (char_u *)&(curbuf->b_p_cms);
	स्थिती PV_CPT:	परत (char_u *)&(curbuf->b_p_cpt);
#ifdef BACKSLASH_IN_FILENAME
	स्थिती PV_CSL:	परत (char_u *)&(curbuf->b_p_csl);
#endif
#ifdef FEAT_COMPL_FUNC
	स्थिती PV_CFU:	परत (char_u *)&(curbuf->b_p_cfu);
	स्थिती PV_OFU:	परत (char_u *)&(curbuf->b_p_ofu);
#endif
#ifdef FEAT_EVAL
	स्थिती PV_TFU:	परत (char_u *)&(curbuf->b_p_tfu);
#endif
	स्थिती PV_EOF:	परत (char_u *)&(curbuf->b_p_eof);
	स्थिती PV_EOL:	परत (char_u *)&(curbuf->b_p_eol);
	स्थिती PV_FIXEOL:	परत (char_u *)&(curbuf->b_p_fixeol);
	स्थिती PV_ET:	परत (char_u *)&(curbuf->b_p_et);
	स्थिती PV_FENC:	परत (char_u *)&(curbuf->b_p_fenc);
	स्थिती PV_FF:	परत (char_u *)&(curbuf->b_p_ff);
	स्थिती PV_FT:	परत (char_u *)&(curbuf->b_p_ft);
	स्थिती PV_FO:	परत (char_u *)&(curbuf->b_p_fo);
	स्थिती PV_FLP:	परत (char_u *)&(curbuf->b_p_flp);
	स्थिती PV_IMI:	परत (char_u *)&(curbuf->b_p_iminsert);
	स्थिती PV_IMS:	परत (char_u *)&(curbuf->b_p_imsearch);
	स्थिती PV_INF:	परत (char_u *)&(curbuf->b_p_inf);
	स्थिती PV_ISK:	परत (char_u *)&(curbuf->b_p_isk);
#ifdef FEAT_FIND_ID
# ifdef FEAT_EVAL
	स्थिती PV_INEX:	परत (char_u *)&(curbuf->b_p_inex);
# endif
#endif
#जर defined(FEAT_EVAL)
	स्थिती PV_INDE:	परत (char_u *)&(curbuf->b_p_inde);
	स्थिती PV_INDK:	परत (char_u *)&(curbuf->b_p_indk);
#endif
#ifdef FEAT_EVAL
	स्थिती PV_FEX:	परत (char_u *)&(curbuf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
	स्थिती PV_किल्ली:	परत (char_u *)&(curbuf->b_p_किल्ली);
#endif
	स्थिती PV_LISP:	परत (char_u *)&(curbuf->b_p_lisp);
	स्थिती PV_LOP:	परत (char_u *)&(curbuf->b_p_lop);
	स्थिती PV_ML:	परत (char_u *)&(curbuf->b_p_ml);
	स्थिती PV_MPS:	परत (char_u *)&(curbuf->b_p_mps);
	स्थिती PV_MA:	परत (char_u *)&(curbuf->b_p_ma);
	स्थिती PV_MOD:	परत (char_u *)&(curbuf->b_changed);
	स्थिती PV_NF:	परत (char_u *)&(curbuf->b_p_nf);
	स्थिती PV_PI:	परत (char_u *)&(curbuf->b_p_pi);
	स्थिती PV_QE:	परत (char_u *)&(curbuf->b_p_qe);
	स्थिती PV_RO:	परत (char_u *)&(curbuf->b_p_ro);
	स्थिती PV_SI:	परत (char_u *)&(curbuf->b_p_si);
	स्थिती PV_SN:	परत (char_u *)&(curbuf->b_p_sn);
	स्थिती PV_STS:	परत (char_u *)&(curbuf->b_p_sts);
	स्थिती PV_SUA:	परत (char_u *)&(curbuf->b_p_sua);
	स्थिती PV_SWF:	परत (char_u *)&(curbuf->b_p_swf);
#ifdef FEAT_SYN_HL
	स्थिती PV_SMC:	परत (char_u *)&(curbuf->b_p_smc);
	स्थिती PV_SYN:	परत (char_u *)&(curbuf->b_p_syn);
#endif
#ifdef FEAT_SPELL
	स्थिती PV_SPC:	परत (char_u *)&(curwin->w_s->b_p_spc);
	स्थिती PV_SPF:	परत (char_u *)&(curwin->w_s->b_p_spf);
	स्थिती PV_SPL:	परत (char_u *)&(curwin->w_s->b_p_spl);
	स्थिती PV_SPO:	परत (char_u *)&(curwin->w_s->b_p_spo);
#endif
	स्थिती PV_SW:	परत (char_u *)&(curbuf->b_p_sw);
	स्थिती PV_TS:	परत (char_u *)&(curbuf->b_p_ts);
	स्थिती PV_TW:	परत (char_u *)&(curbuf->b_p_tw);
	स्थिती PV_TX:	परत (char_u *)&(curbuf->b_p_tx);
#ifdef FEAT_PERSISTENT_UNDO
	स्थिती PV_UDF:	परत (char_u *)&(curbuf->b_p_udf);
#endif
	स्थिती PV_WM:	परत (char_u *)&(curbuf->b_p_wm);
#ifdef FEAT_KEYMAP
	स्थिती PV_KMAP:	परत (char_u *)&(curbuf->b_p_keymap);
#endif
#ifdef FEAT_SIGNS
	स्थिती PV_SCL:	परत (char_u *)&(curwin->w_p_scl);
#endif
#ifdef FEAT_VARTABS
	स्थिती PV_VSTS:	परत (char_u *)&(curbuf->b_p_vsts);
	स्थिती PV_VTS:	परत (char_u *)&(curbuf->b_p_vts);
#endif
	पूर्वनिर्धारित:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    परत (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(पूर्णांक opt_idx)
{
    परत options[opt_idx].var;
}

#जर defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(पूर्णांक opt_idx)
{
    परत (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    पूर्णांक
option_info(char_u *name, dict_T *d)
{
    पूर्णांक			opt_idx;
    संरचना vimoption	*p;
    पूर्णांक			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    जर ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    जर (opt_idx < 0)
	परत FAIL;

    p = &options[opt_idx];
    dict_add_string(d, "name", (char_u *)p->fullname);
    dict_add_string(d, "shortname",
		    (char_u *)(p->shortname == रिक्त ? "" : p->shortname));
    dict_add_bool(d, "available", p->var != रिक्त);
    dict_add_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_अंक) ? "number" : "string"));
    dict_add_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    जर (p->flags & P_BOOL)
	dict_add_bool(d, "default", (पूर्णांक)(long_i)p->def_val[dvi]);
    अन्यथा जर (p->flags & P_अंक)
	dict_add_संख्या(d, "default", (दीर्घ)(long_i)p->def_val[dvi]);
    अन्यथा
	dict_add_string(d, "default", p->def_val[dvi] == रिक्त
					? (char_u *)"" : p->def_val[dvi]);
    परत OK;
}
#endif

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_set_cb_T
get_option_did_set_cb(पूर्णांक opt_idx)
{
    परत options[opt_idx].opt_did_set_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(शून्य)
{
    जर (*curbuf->b_p_ep == NUL)
	परत p_ep;
    परत curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(शून्य)
{
#ifdef FEAT_EVAL
    जर (*curbuf->b_p_ffu == NUL)
	परत p_ffu;
    परत curbuf->b_p_ffu;
#अन्यथा
    परत (char_u *)"";
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
    अक्षर *errmsg = update_winhighlight(wp, wp->w_p_whl);

    जर (errmsg != रिक्त)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    जर (wp->w_p_wrap)
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
    जर (val == empty_option)
	परत empty_option;  // no need to allocate memory
    परत vim_strsave(val);
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
						    आकार(to->wo_script_ctx));
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
स्थिर पूर्णांक buf_opt_idx[BV_मोजणी];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    स्थिर शून्य
init_buf_opt_idx(शून्य)
{
    स्थिर पूर्णांक did_init_buf_opt_idx = FALSE;
    पूर्णांक i;

    जर (did_init_buf_opt_idx)
	परत;
    did_init_buf_opt_idx = TRUE;
    साठी (i = 0; !istermoption_idx(i); i++)
	जर (options[i].indir & PV_BUF)
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
buf_copy_options(buf_T *buf, पूर्णांक flags)
{
    पूर्णांक		should_copy = TRUE;
    char_u	*save_p_isk = रिक्त;	    // init for GCC
    पूर्णांक		dont_do_help;
    पूर्णांक		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    जर (p_cpo != रिक्त)
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
	जर ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == रिक्त || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != रिक्त)))
	    should_copy = FALSE;

	जर (should_copy || (flags & BCO_ALWAYS))
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
	    जर (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = रिक्त;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    जर (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		निवड (*p_ffs)
		{
		    स्थिती 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); थांबा;
		    स्थिती 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); थांबा;
		    स्थिती 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); थांबा;
		    पूर्वनिर्धारित:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		जर (buf->b_p_ff != रिक्त)
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
	    जर (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
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
	    जर (p_vsts && p_vsts != empty_option)
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
#जर defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#endif
	    buf->b_p_fp = empty_option;
#जर defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#endif
#ifdef FEAT_CRYPT
	    buf->b_p_किल्ली = vim_strsave(p_किल्ली);
	    COPY_OPT_SCTX(buf, BV_किल्ली);
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
#जर defined(FEAT_EVAL)
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
#जर defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = empty_option;
#endif
#जर defined(FEAT_CRYPT)
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
	    जर (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ifdef FEAT_VARTABS
		जर (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
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
		जर (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (शून्य)tabstop_set(p_vts, &buf->b_p_vts_array);
		अन्यथा
		    buf->b_p_vts_array = रिक्त;
#endif
		buf->b_help = false;
		जर (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	जर (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    जर (did_isk)
	(शून्य)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    शून्य
reset_modifiable(शून्य)
{
    पूर्णांक		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    जर (opt_idx >= 0)
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

स्थिर पूर्णांक expand_option_idx = -1;
स्थिर पूर्णांक expand_option_start_col = 0;
स्थिर char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
स्थिर पूर्णांक expand_option_flags = 0;
स्थिर पूर्णांक expand_option_append = FALSE;

    शून्य
set_context_in_set_cmd(
    expand_T	*xp,
    char_u	*arg,
    पूर्णांक		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    पूर्णांक		nextchar;
    long_u	flags = 0;	// init for GCC
    पूर्णांक		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    पूर्णांक		is_term_option = FALSE;
    पूर्णांक		किल्ली;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    जर (*arg == NUL)
    {
	xp->xp_pattern = arg;
	परत;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    जर (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_pattern = p + 1;
	परत;
    }
    पर्यंत (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	जर (*p == ' ' || *p == ',')
	{
	    पर्यंत (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	जर (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    थांबा;
	}
	--p;
    }
    जर (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_NO;
	p += 2;
    }
    अन्यथा जर (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_INV;
	p += 3;
    }
    xp->xp_pattern = arg = p;
    जर (*arg == '<')
    {
	पर्यंत (*p != '>')
	    जर (*p++ == NUL)	    // expand terminal option name
		परत;
	किल्ली = get_special_किल्ली_code(arg + 1);
	जर (किल्ली == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    परत;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(किल्ली);
	expand_option_name[3] = KEY2TERMCAP1(किल्ली);
    }
    अन्यथा
    {
	जर (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    जर (*p != NUL)
		++p;
	    जर (*p == NUL)
		परत;		// expand option name
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	अन्यथा
	{
	    // Allow * wildcard
	    पर्यंत (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    जर (*p == NUL)
		परत;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    जर (opt_idx == -1 || options[opt_idx].var == रिक्त)
	    {
		xp->xp_context = EXPAND_NOTHING;
		परत;
	    }
	    flags = options[opt_idx].flags;
	    जर (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		परत;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    पूर्णांक expand_option_subtract = FALSE;
    जर ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	जर (nextchar == '-')
	    expand_option_subtract = TRUE;
	जर (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    जर ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	परत;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    जर (is_term_option)
	expand_option_idx = -1;
    अन्यथा
	expand_option_idx = opt_idx;

    जर (!is_term_option)
    {
	जर (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    परत;
	}
    }

    xp->xp_pattern = p + 1;
    expand_option_start_col = (पूर्णांक)(p + 1 - xp->xp_ओळ);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ifdef FEAT_SYN_HL
    जर (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	परत;
    }
#endif
    जर (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	परत;
    }
#ifdef FEAT_KEYMAP
    जर (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	परत;
    }
#endif

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    जर (expand_option_subtract)
    {
	xp->xp_context = EXPAND_SETTING_SUBTRACT;
	परत;
    }
    अन्यथा जर (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != रिक्त)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    अन्यथा जर (*xp->xp_pattern == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	परत;
    }
    अन्यथा
	xp->xp_context = EXPAND_NOTHING;

    जर (is_term_option || (flags & P_अंक))
	परत;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    जर (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	जर (p == (char_u *)&p_bdir
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
	    जर (p == (char_u *)&p_path || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    अन्यथा
		xp->xp_backslash = XP_BS_ONE;
	}
	अन्यथा
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    जर (p == (char_u *)&p_tags)
		xp->xp_backslash = XP_BS_THREE;
	    अन्यथा
		xp->xp_backslash = XP_BS_ONE;
	}
	जर (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    जर ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	साठी (p = argend - 1; p >= xp->xp_pattern; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    जर (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		पर्यंत (s > xp->xp_pattern && *(s - 1) == '\\')
		    --s;
		जर ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#जर defined(BACKSLASH_IN_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#अन्यथा
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#endif
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_pattern = p + 1;
		    थांबा;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    जर (flags & P_FLAGLIST)
	xp->xp_pattern = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ifdef FEAT_SPELL
    जर (options[opt_idx].var == (char_u *)&p_sps)
    {
	जर (STRNCMP(xp->xp_pattern, "file:", 5) == 0)
	{
	    xp->xp_pattern += 5;
	    परत;
	}
	अन्यथा जर (options[expand_option_idx].opt_expand_cb != रिक्त)
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
    स्थिर पूर्णांक
match_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	पूर्णांक		idx,
	पूर्णांक		test_only,
	पूर्णांक		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    जर (!fuzzy)
    {
	जर (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    जर (!test_only)
		matches[idx] = vim_strsave(str);
	    परत TRUE;
	}
    }
    अन्यथा
    {
	पूर्णांक score;

	score = fuzzy_match_str(str, fuzzystr);
	जर (score != FUZZY_SCORE_NONE)
	{
	    जर (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    परत TRUE;
	}
    }

    परत FALSE;
}

    पूर्णांक
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    पूर्णांक		*अंक_Matches,
    char_u	***matches,
    पूर्णांक		can_fuzzy)
{
    पूर्णांक		अंक_normal = 0;	    // Nr of matching non-term-code settings
    पूर्णांक		अंक_term = 0;	    // Nr of matching terminal code settings
    पूर्णांक		opt_idx;
    पूर्णांक		match;
    पूर्णांक		मोजणी = 0;
    char_u	*str;
    पूर्णांक		loop;
    पूर्णांक		is_term_opt;
    char_u	name_buf[MAX_किल्ली_NAME_LEN];
    स्थिर अक्षर *(names[]) = {"all", "termcap"};
    पूर्णांक		ic = regmatch->rm_ic;	// remember the ignore-case flag
    पूर्णांक		fuzzy;
    fuzmatch_str_T  *fuzmatch = रिक्त;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    साठी (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	जर (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    साठी (match = 0; match < (पूर्णांक)ARRAY_LENGTH(names); ++match)
	    {
		जर (match_str((char_u *)names[match], regmatch, *matches,
			    मोजणी, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    जर (loop == 0)
			अंक_normal++;
		    अन्यथा
			मोजणी++;
		}
	    }
	}
	साठी (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != रिक्त;
								    opt_idx++)
	{
	    जर (options[opt_idx].var == रिक्त)
		सुरूठेवा;
	    जर (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		सुरूठेवा;
	    is_term_opt = istermoption_idx(opt_idx);
	    जर (is_term_opt && अंक_normal > 0)
		सुरूठेवा;

	    जर (match_str(str, regmatch, *matches, मोजणी, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		जर (loop == 0)
		{
		    जर (is_term_opt)
			अंक_term++;
		    अन्यथा
			अंक_normal++;
		}
		अन्यथा
		    मोजणी++;
	    }
	    अन्यथा जर (!fuzzy && options[opt_idx].shortname != रिक्त
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		जर (loop == 0)
		{
		    जर (is_term_opt)
			अंक_term++;
		    अन्यथा
			अंक_normal++;
		}
		अन्यथा
		    (*matches)[मोजणी++] = vim_strsave(str);
	    }
	    अन्यथा जर (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		जर (match_str(name_buf, regmatch, *matches, मोजणी, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    जर (loop == 0)
			अंक_term++;
		    अन्यथा
			मोजणी++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	जर (xp->xp_context != EXPAND_BOOL_SETTINGS  && अंक_normal == 0)
	{
	    साठी (opt_idx = 0; (str = get_termcode(opt_idx)) != रिक्त; opt_idx++)
	    {
		जर (!SAFE_isprint(str[0]) || !SAFE_isprint(str[1]))
		    सुरूठेवा;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		जर (match_str(name_buf, regmatch, *matches, मोजणी,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    जर (loop == 0)
			अंक_term++;
		    अन्यथा
			मोजणी++;
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

		    जर (match_str(name_buf, regmatch, *matches, मोजणी,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			जर (loop == 0)
			    अंक_term++;
			अन्यथा
			    मोजणी++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    साठी (opt_idx = 0; (str = get_किल्ली_name(opt_idx)) != रिक्त; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		जर (match_str(name_buf, regmatch, *matches, मोजणी, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    जर (loop == 0)
			अंक_term++;
		    अन्यथा
			मोजणी++;
		}
	    }
	}
	जर (loop == 0)
	{
	    जर (अंक_normal > 0)
		*अंक_Matches = अंक_normal;
	    अन्यथा जर (अंक_term > 0)
		*अंक_Matches = अंक_term;
	    अन्यथा
		परत OK;
	    जर (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *अंक_Matches);
		जर (*matches == रिक्त)
		{
		    *matches = (char_u **)"";
		    परत FAIL;
		}
	    }
	    अन्यथा
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *अंक_Matches);
		जर (fuzmatch == रिक्त)
		{
		    *matches = (char_u **)"";
		    परत FAIL;
		}
	    }
	}
    }

    जर (fuzzy &&
	fuzzymatches_to_strmatches(fuzmatch, matches, मोजणी, FALSE) == FAIL)
	परत FAIL;

    परत OK;
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
    जर (buf == रिक्त)
	परत रिक्त;

#ifdef BACKSLASH_IN_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    साठी (var = buf; *var != NUL; MB_PTR_ADV(var))
	जर (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#endif
    परत buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    पूर्णांक
ExpandOldSetting(पूर्णांक *अंक_Matches, char_u ***matches)
{
    char_u  *var = रिक्त;	// init for GCC
    char_u  *buf;

    *अंक_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    जर (*matches == रिक्त)
	परत FAIL;

    // For a terminal key code expand_option_idx is < 0.
    जर (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	जर (var == रिक्त)
	    expand_option_idx = findoption(expand_option_name);
    }

    जर (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    अन्यथा जर (var == रिक्त)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    जर (buf == रिक्त)
    {
	VIM_CLEAR(*matches);
	परत FAIL;
    }

    (*matches)[0] = buf;
    *अंक_Matches = 1;
    परत OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    पूर्णांक
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    पूर्णांक		*अंक_Matches,
    char_u	***matches)
{
    char_u  *var = रिक्त;	// init for GCC
    char_u  *buf;

    जर (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == रिक्त)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	परत FAIL;
    }

    optexpand_T args;
    args.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    args.oe_append = expand_option_append;
    args.oe_regmatch = regmatch;
    args.oe_xp = xp;
    args.oe_set_arg = xp->xp_ओळ + expand_option_start_col;
    args.oe_include_orig_val =
	!expand_option_append &&
	(*args.oe_set_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    जर (buf == रिक्त)
	परत FAIL;

    args.oe_opt_मूल्य = buf;

    पूर्णांक अंक_ret = options[expand_option_idx].opt_expand_cb(&args, अंक_Matches, matches);

    vim_free(buf);
    परत अंक_ret;
}

/*
 * Expansion handler for :set-=
 */
    पूर्णांक
ExpandSettingSubtract(
    expand_T	*xp,
    regmatch_T	*regmatch,
    पूर्णांक		*अंक_Matches,
    char_u	***matches)
{
    जर (expand_option_idx < 0)
	// term option
	परत ExpandOldSetting(अंक_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    जर (option_flags & P_अंक)
	परत ExpandOldSetting(अंक_Matches, matches);
    अन्यथा जर (option_flags & P_COMMA)
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

	जर (*option_val == NUL)
	    परत FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	जर (option_copy == रिक्त)
	    परत FAIL;
	next_val = option_copy;

	ga_init2(&ga, आकार(char_u *), 10);

	करा
	{
	    item = next_val;
	    comma = vim_strchr(next_val, ',');
	    पर्यंत (comma != रिक्त && comma != next_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    जर (comma != रिक्त)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		next_val = comma + 1;
	    }
	    अन्यथा
		next_val = रिक्त;

	    जर (*item == NUL)
		// empty value, don't add to list
		सुरूठेवा;

	    जर (!vim_regexec(regmatch, item, (colnr_T)0))
		सुरूठेवा;

	    char_u *buf = escape_option_str_cmdline(item);
	    जर (buf == रिक्त)
	    {
		vim_free(option_copy);
		ga_clear_strings(&ga);
		परत FAIL;
	    }
	    जर (ga_add_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		थांबा;
	    }
	} पर्यंत (next_val != रिक्त);

	vim_free(option_copy);

	*matches = ga.ga_data;
	*अंक_Matches = ga.ga_len;
	परत OK;
    }
    अन्यथा जर (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	जर (*xp->xp_pattern != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    परत FAIL;
	}

	माप_प्रकार अंक_flags = STRLEN(option_val);
	जर (अंक_flags == 0)
	    परत FAIL;

	*matches = ALLOC_MULT(char_u *, अंक_flags + 1);
	जर (*matches == रिक्त)
	    परत FAIL;

	पूर्णांक मोजणी = 0;
	char_u *p;

	p = vim_strnsave(option_val, अंक_flags);
	जर (p == रिक्त)
	{
	    VIM_CLEAR(*matches);
	    परत FAIL;
	}
	(*matches)[मोजणी++] = p;

	जर (अंक_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    साठी (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		जर (p == रिक्त)
		    थांबा;
		(*matches)[मोजणी++] = p;
	    }
	}

	*अंक_Matches = मोजणी;
	परत OK;
    }

    परत ExpandOldSetting(अंक_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    स्थिर शून्य
option_value2string(
    संरचना vimoption	*opp,
    पूर्णांक			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    जर (opp->flags & P_अंक)
    {
	दीर्घ wc = 0;

	जर (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_किल्ली_name((पूर्णांक)wc, 0));
	अन्यथा जर (wc != 0)
	    STRCPY(NameBuff, transchar((पूर्णांक)wc));
	अन्यथा
	    प्र_लिखो((अक्षर *)NameBuff, "%ld", *(दीर्घ *)varp);
    }
    अन्यथा    // P_STRING
    {
	varp = *(char_u **)(varp);
	जर (varp == रिक्त)		    // just in case
	    NameBuff[0] = NUL;
#ifdef FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	अन्यथा जर (opp->var == (char_u *)&p_किल्ली && *varp)
	    STRCPY(NameBuff, "*****");
#endif
	अन्यथा जर (opp->flags & P_EXPAND)
	    home_replace(रिक्त, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	अन्यथा जर ((char_u **)opp->var == &p_pt)
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
    स्थिर पूर्णांक
wc_use_keyname(char_u *varp, दीर्घ *wcp)
{
    जर (((दीर्घ *)varp == &p_wc) || ((दीर्घ *)varp == &p_wcm))
    {
	*wcp = *(दीर्घ *)varp;
	जर (IS_SPECIAL(*wcp) || find_special_किल्ली_in_table((पूर्णांक)*wcp) >= 0)
	    परत TRUE;
    }
    परत FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    पूर्णांक
shortmess(पूर्णांक x)
{
    परत p_shm != रिक्त &&
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
    पूर्णांक		opt_idx;
    पूर्णांक		dofree = FALSE;
    char_u	*p;

    जर (!option_was_set((char_u *)"cp"))
    {
	p_cp = FALSE;
	साठी (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    जर (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    जर (fname != रिक्त)
    {
	p = vim_getenv(envname, &dofree);
	जर (p == रिक्त)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = FullName_save(fname, FALSE);
	    जर (p != रिक्त)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	अन्यथा जर (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    शून्य
change_compatible(पूर्णांक on)
{
    पूर्णांक	    opt_idx;

    जर (p_cp != on)
    {
	p_cp = on;
	compatible_set();
    }
    opt_idx = findoption((char_u *)"cp");
    जर (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_SET;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    पूर्णांक
option_was_set(char_u *name)
{
    पूर्णांक idx;

    idx = findoption(name);
    जर (idx < 0)	// unknown option
	परत FALSE;
    जर (options[idx].flags & P_WAS_SET)
	परत TRUE;
    परत FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    पूर्णांक
reset_option_was_set(char_u *name)
{
    पूर्णांक idx = findoption(name);

    जर (idx < 0)
	परत FAIL;

    options[idx].flags &= ~P_WAS_SET;
    परत OK;
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
    पूर्णांक	    opt_idx;

    साठी (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	जर (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    set_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    पूर्णांक
can_bs(
    पूर्णांक		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ifdef FEAT_JOB_CHANNEL
    जर (what == BS_START && bt_prompt(curbuf))
	परत FALSE;
#endif
    निवड (*p_bs)
    {
	स्थिती '3':       परत TRUE;
	स्थिती '2':	परत (what != BS_NOSTOP);
	स्थिती '1':	परत (what != BS_START);
	स्थिती '0':	परत FALSE;
    }
    परत vim_strchr(p_bs, what) != रिक्त;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    दीर्घ
get_scrolloff_मूल्य(शून्य)
{
    परत curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    दीर्घ
get_scrolloffpad_मूल्य(शून्य)
{
    परत curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    दीर्घ
get_sidescrolloff_मूल्य(शून्य)
{
    परत curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    अचिन्हित पूर्णांक
get_bkc_flags(buf_T *buf)
{
    परत buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#जर defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_मूल्य(buf_T *buf)
{
    जर (buf->b_p_flp == रिक्त || *buf->b_p_flp == NUL)
	परत p_flp;
    परत buf->b_p_flp;
}
#endif

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    अचिन्हित पूर्णांक
get_ve_flags(शून्य)
{
    परत (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#जर defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_मूल्य(win_T *win)
{
    जर (win->w_p_sbr == रिक्त || *win->w_p_sbr == NUL)
	परत p_sbr;
    जर (STRCMP(win->w_p_sbr, "NONE") == 0)
	परत empty_option;
    परत win->w_p_sbr;
}
#endif

#जर defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(पूर्णांक bufopt)
{
    dict_T	*d;
    पूर्णांक		opt_idx;

    d = dict_alloc();
    जर (d == रिक्त)
	परत रिक्त;

    साठी (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	संरचना vimoption *opt = &options[opt_idx];

	जर ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    जर (varp != रिक्त)
	    {
		जर (opt->flags & P_STRING)
		    dict_add_string(d, opt->fullname, *(char_u **)varp);
		अन्यथा जर (opt->flags & P_अंक)
		    dict_add_संख्या(d, opt->fullname, *(दीर्घ *)varp);
		अन्यथा
		    dict_add_संख्या(d, opt->fullname, *(पूर्णांक *)varp);
	    }
	}
    }

    परत d;
}
#endif

#जर defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    पूर्णांक
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    जर (val == रिक्त)
	p = wp->w_p_culopt;
    अन्यथा
	p = val;
    पर्यंत (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	जर (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_ओळ;
	}
	अन्यथा जर (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_ओळ | CULOPT_NBR;
	}
	अन्यथा जर (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	अन्यथा जर (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	जर (*p != ',' && *p != NUL)
	    परत FAIL;
	जर (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    जर ((culopt_flags_new & CULOPT_ओळ) && (culopt_flags_new & CULOPT_SCRLINE))
	परत FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    परत OK;
}
#endif

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    पूर्णांक
magic_isset(शून्य)
{
    निवड (magic_overruled)
    {
	स्थिती OPTION_MAGIC_ON:      परत TRUE;
	स्थिती OPTION_MAGIC_OFF:     परत FALSE;
	स्थिती OPTION_MAGIC_NOT_SET: थांबा;
    }
#ifdef FEAT_EVAL
    जर (in_vim9script())
	परत TRUE;
#endif
    परत p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    पूर्णांक
option_set_callback_func(char_u *optval UNUSED, callback_T *optcb UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    पूर्णांक		funcname = FALSE;

    जर (optval == रिक्त || *optval == NUL)
    {
	free_callback(optcb);
	परत OK;
    }

    जर (*optval == '{' || (in_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_अभिव्यक्ती(optval, रिक्त);
    अन्यथा
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    जर (tv == रिक्त)
	परत FAIL;

    cb = get_callback(tv);
    जर (cb.cb_name == रिक्त || *cb.cb_name == NUL)
    {
	free_tv(tv);
	परत FAIL;
    }

    free_callback(optcb);
    set_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = रिक्त;
    जर (in_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != रिक्त))
    {
	जर (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = find_imported(optval, dot - optval, FALSE);
	जर (import != रिक्त && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((अक्षर *)fnamebuf, आकार(fnamebuf), "<SNR>%d_%s",
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

    परत OK;
#अन्यथा
    परत FAIL;
#endif
}

#जर defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    अक्षर *
did_set_showtabpanel(optset_T *args UNUSED)
{
    shell_new_columns();
    परत रिक्त;
}
#endif

#जर defined(FEAT_EVAL)
    स्थिर शून्य
didset_options_sctx(पूर्णांक opt_flags, अक्षर **buf)
{
	साठी (पूर्णांक i = 0; ; ++i)
	{
	    जर (buf[i] == रिक्त)
		थांबा;

	    पूर्णांक idx = findoption((char_u *)buf[i]);
	    जर (idx >= 0)
		set_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#endif
