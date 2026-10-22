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
#شامل "vim.h"
#شامل "optiondefs.h"

ایستا تهی set_options_default(صحیح opt_flags);
ایستا تهی set_string_default_esc(نویسه *name, char_u *val, صحیح escape);
ایستا char_u *find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
ایستا char_u *option_expand(صحیح opt_idx, char_u *val);
ایستا تهی didset_options(تهی);
ایستا تهی didset_options2(تهی);
#اگر defined(FEAT_EVAL)
ایستا long_u *insecure_flag(win_T *wp, صحیح opt_idx, صحیح opt_flags);
#وگرنه
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#endif
ایستا نویسه *set_bool_option(صحیح opt_idx, char_u *varp, صحیح مقدار, صحیح opt_flags);
ایستا نویسه *set_رقم_option(صحیح opt_idx, char_u *varp, long مقدار, نویسه *errbuf, size_t errbuflen, صحیح opt_flags);
ایستا صحیح find_کلید_option(char_u *arg_arg, صحیح has_lt);
ایستا تهی showoptions(صحیح all, صحیح opt_flags);
ایستا صحیح optval_default(ساختار vimoption *, char_u *varp, صحیح compatible);
ایستا تهی showoneopt(ساختار vimoption *, صحیح opt_flags);
ایستا صحیح put_setstring(FILE *fd, bool legacy, نویسه *cmd, نویسه *name, char_u **valuep, long_u flags);
ایستا صحیح put_setnum(FILE *fd, نویسه *cmd, نویسه *name, long *valuep);
ایستا صحیح put_setbool(FILE *fd, نویسه *cmd, نویسه *name, صحیح مقدار);
ایستا صحیح istermoption(ساختار vimoption *p);
ایستا صحیح istermoption_idx(صحیح opt_idx);
ایستا char_u *get_varp_scope(ساختار vimoption *p, صحیح scope);
ایستا char_u *get_varp(ساختار vimoption *);
ایستا تهی check_win_options(win_T *win);
ایستا تهی option_value2string(ساختار vimoption *, صحیح scope);
ایستا تهی check_winopt(winopt_T *wop);
ایستا صحیح wc_use_keyname(char_u *varp, long *wcp);
ایستا تهی compatible_set(تهی);

#اگر defined(FEAT_EVAL)
ایستا نویسه *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", پوچ};
ایستا نویسه *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    پوچ};
ایستا تهی didset_options_sctx(صحیح opt_flags, نویسه **buf);
#endif


/*
 * Initialize the 'shell' option to a default value.
 */
    ایستا تهی
set_init_default_shell(تهی)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    اگر (((p = mch_getenv((char_u *)"SHELL")) != پوچ && *p != NUL)
#اگر defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != پوچ && *p != NUL)
	    || ((p = (char_u *)default_shell()) != پوچ && *p != NUL)
#endif
	    )
#اگر defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	اگر (vim_strchr(p, ' ') != پوچ)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    اگر (cmd != پوچ)
	    {
		vim_snprintf((نویسه *)cmd, len, "\"%s\"", p);
		set_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	وگرنه
	    set_string_default("sh", p);
    }
#وگرنه
	set_string_default_esc("sh", p, TRUE);
#endif
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    ایستا تهی
set_init_default_backupskip(تهی)
{
    صحیح		opt_idx;
    صحیح		i;
    char_u	*p;
    صحیح		plen;
#ifdef UNIX
    ایستا نویسه	*(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#وگرنه
    ایستا نویسه	*(names[3]) = {"TMPDIR", "TEMP", "TMP"};
#endif
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    برای (i = 0; i < (صحیح)ARRAY_LENGTH(names); ++i)
    {
	صحیح		mustfree = FALSE;
#ifdef UNIX
	اگر (*names[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (صحیح)STRLEN_LITERAL("/private/tmp");
# وگرنه
	    p = (char_u *)"/tmp";
	    plen = (صحیح)STRLEN_LITERAL("/tmp");
# endif
	}
	وگرنه
#endif
	{
	    p = vim_getenv((char_u *)names[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	اگر (p != پوچ && *p != NUL)
	{
	    char_u  *item;
	    size_t  itemsize;
	    صحیح	    has_trailing_path_sep = FALSE;

	    اگر (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (صحیح)STRLEN(p);
		// does the value include a trailing path separator?
		اگر (after_pathsep(p, p + plen))
		    has_trailing_path_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_path_sep ? 0 : 1) + 2;
	    item = alloc(itemsize);
	    اگر (item != پوچ)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((نویسه *)item, itemsize, "%s%s*", p, (has_trailing_path_sep) ? "" : PATHSEPSTR);

		اگر (find_dup_item(ga.ga_data, item, itemlen, options[opt_idx].flags) == پوچ
			&& ga_grow(&ga, (صحیح)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((نویسه *)ga.ga_data + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", item);
		}
		vim_free(item);
	    }
	}
	اگر (mustfree)
	    vim_free(p);
    }
    اگر (ga.ga_data != پوچ)
    {
	set_string_default("bsk", ga.ga_data);
	vim_free(ga.ga_data);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    ایستا تهی
set_init_default_maxmemtot(تهی)
{
    صحیح		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    اگر (opt_idx < 0)
	بازگشت;

#اگر !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
    اگر (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#endif
    {
#اگر defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#وگرنهاگر defined(HAVE_TOTAL_MEM)
	// Use amount of memory available to Vim.
	n = (mch_total_mem(FALSE) >> 1);
#وگرنه
	n = (0x7fffffff >> 11);
#endif
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	اگر (opt_idx >= 0)
	{
#اگر !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
	    اگر ((long)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (long)n
		    || (long)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#endif
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    ایستا تهی
set_init_default_cdpath(تهی)
{
    صحیح		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    صحیح	i;
    صحیح	j;
    صحیح	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    اگر (cdpath == پوچ)
	بازگشت;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    اگر (buf != پوچ)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	برای (i = 0; cdpath[i] != NUL; ++i)
	{
	    اگر (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    وگرنه
	    {
		اگر (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	اگر (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	وگرنه
	    vim_free(buf); // cannot happen
    }
    اگر (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    ایستا تهی
set_init_default_printencoding(تهی)
{
#اگر defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    set_string_default("penc",
# اگر defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# وگرنه // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#endif
}

#اگر defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    ایستا تهی
set_init_default_printexpr(تهی)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    set_string_default("pexpr",
# اگر defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# وگرنه
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#endif

#ifdef UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    ایستا تهی
set_init_restricted_mode(تهی)
{
    char_u	*p;

    p = get_isolated_shell_name();
    اگر (p == پوچ)
	بازگشت;
    اگر (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#endif

#ifdef CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    ایستا تهی
set_init_clean_rtp(تهی)
{
    صحیح		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    اگر (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    اگر (opt_idx < 0)
	بازگشت;
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
    ایستا تهی
set_init_xdg_rtp(تهی)
{
    صحیح		opt_idx;
    صحیح		has_xdg_env = TRUE;
    صحیح		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = پوچ;
    char_u	*vimrc2 = پوچ;
    char_u	*xdg_dir = پوچ;
    char_u	*xdg_rtp = پوچ;
    string_T	vimrc_xdg = {پوچ, 0};

    // initialize chartab, so we can expand $HOME
    (تهی)init_chartab();
    vimrc1 = expand_env_save((char_u *)USR_VIMRC_FILE);
    vimrc2 = expand_env_save((char_u *)USR_VIMRC_FILE2);

    xdg_dir = mch_getenv("XDG_CONFIG_HOME");
    اگر (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    اگر (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	برو theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    اگر ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	برو theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    اگر ((opt_idx = findoption((char_u *)"packpath")) < 0)
	برو theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# اگر defined(XDG_VDIR) && defined(FEAT_SESSION)
    اگر ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	برو theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    اگر (should_free_xdg_dir)
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
    ایستا تهی
set_init_expand_env(تهی)
{
    صحیح		opt_idx;
    char_u	*p;

    برای (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	اگر ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != پوچ)
	    p = (char_u *)_(*(نویسه **)options[opt_idx].var);
	وگرنه
	    p = option_expand(opt_idx, پوچ);
	اگر (p != پوچ && (p = vim_strsave(p)) != پوچ)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    اگر (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#اگر defined(MSWIN) && defined(FEAT_GETTEXT)
/*
 * Get the display language of Windows and the languages to fall back on, as a
 * colon separated list for gettext, e.g. "ja_JP:en_US".  The list stops after
 * English, untranslated messages are English already.
 * Returns NULL when it cannot be obtained.  The result must be freed.
 */
    ایستا char_u *
get_ui_langs(تهی)
{
    ULONG	رقم_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*buffer;
    char_u	*langs = پوچ;

    اگر (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &رقم_languages, پوچ,
								    &bufsize)
	    || bufsize == 0)
	بازگشت پوچ;

    buffer = ALLOC_MULT(WCHAR, bufsize);
    اگر (buffer == پوچ)
	بازگشت پوچ;

    اگر (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &رقم_languages, buffer,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	اگر (langs != پوچ)
	{
	    char_u	*d = langs;
	    WCHAR	*s = buffer;

	    وقتی (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		اگر (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		برای ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		اگر (english)
		    بشکن;
	    }
	    *d = NUL;
	}
    }
    vim_free(buffer);

    بازگشت langs;
}
#endif

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    ایستا تهی
set_init_lang_env(تهی)
{
#اگر defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    اگر (mch_getenv((char_u *)"LANG") == پوچ
	    && mch_getenv((char_u *)"LANGUAGE") == پوچ
	    && mch_getenv((char_u *)"LC_ALL") == پوچ
	    && mch_getenv((char_u *)"LC_MESSAGES") == پوچ)
    {
	char_u	*langs = get_ui_langs();

	اگر (langs != پوچ && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    اگر (colon != پوچ)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#وگرنهاگر defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#endif
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    ایستا تهی
set_init_default_encoding(تهی)
{
    char_u	*p;
    صحیح		opt_idx;

#اگر defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#وگرنه
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#endif
    اگر (p == پوچ)
	بازگشت;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    اگر (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    اگر (mb_init() == پوچ)
    {
	opt_idx = findoption((char_u *)"encoding");
	اگر (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#اگر defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	اگر (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    set_string_option_direct((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    set_string_option_direct((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    اگر (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    اگر (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (تهی)init_chartab();
	}
#endif

#اگر defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	اگر (
# ifdef VIMDLL
		(!gui.in_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    نویسه	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    اگر (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, اندازه(buf), "cp%ld", (long)GetACP());
	    وگرنه
		buflen = vim_snprintf(buf, اندازه(buf), "cp%ld", (long)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    اگر (p_tenc != پوچ)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		اگر (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_setup(&درون_داد_conv, p_tenc, p_enc);
		convert_setup(&output_conv, p_enc, p_tenc);
	    }
	    وگرنه
		p_tenc = empty_option;
	}
#endif
#اگر defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#endif
    }
    وگرنه
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
    تهی
set_init_1(صحیح clean_arg)
{
#ifdef FEAT_LANGMAP
    langmap_init();
#endif

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    اگر (mch_getenv((char_u *)"VIM_POSIX") != پوچ)
    {
	set_string_default("cpo", (char_u *)CPO_ALL);
	set_string_default("shm", (char_u *)SHM_POSIX);
    }

    set_init_default_shell();
    set_init_default_backupskip();
    set_init_default_maxmemtot();
    set_init_default_cdpath();
    set_init_default_printencoding();
#اگر defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
    اگر (clean_arg)
	set_init_clean_rtp();
#endif

#ifdef FEAT_GUI
    اگر (found_reverse_arg)
	set_option_مقدار_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
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

#اگر defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    اگر (mch_getenv((char_u *)"MLTERM") != پوچ)
	set_option_مقدار_give_err((char_u *)"tbidi", 1L, پوچ, 0);
#endif

    didset_options2();

    set_init_lang_env();

#ifdef FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    set_helplang_default(get_mess_lang());
#endif
}

ایستا char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    تهی
set_fencs_unicode(تهی)
{
    set_string_option_direct((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    ایستا تهی
set_option_default(
    صحیح		opt_idx,
    صحیح		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    صحیح		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    صحیح		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    صحیح		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    اگر (varp != پوچ)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	اگر (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    اگر (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		set_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    وگرنه اگر (options[opt_idx].indir != PV_NONE)
		set_string_option_direct(پوچ, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    وگرنه
	    {
		اگر ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	وگرنه اگر (flags & P_رقم)
	{
	    اگر (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    وگرنه
	    {
		long def_val = (long)(long_i)options[opt_idx].def_val[dvi];

		اگر ((long *)varp == &curwin->w_p_so
			|| (long *)varp == &curwin->w_p_siso
			|| (long *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(long *)varp = -1;
		وگرنه
		    *(long *)varp = def_val;
		// May also set global value for local option.
		اگر (both)
		    *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	وگرنه	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(صحیح *)varp = (صحیح)(long)(long_i)options[opt_idx].def_val[dvi];
#ifdef UNIX
	    // 'modeline' defaults to off for root
	    اگر (options[opt_idx].indir == PV_ML && getuid() == ریشه_UID)
		*(صحیح *)varp = FALSE;
#endif
	    // May also set global value for local option.
	    اگر (both)
		*(صحیح *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(صحیح *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	اگر (both)
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
    ایستا تهی
set_options_default(
    صحیح		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    صحیح		i;
    win_T	*wp;
    tabpage_T	*tp;

    برای (i = 0; !istermoption_idx(i); i++)
	اگر (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#اگر defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_کلید
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
    ایستا تهی
set_string_default_esc(نویسه *name, char_u *val, صحیح escape)
{
    char_u	*p;
    صحیح		opt_idx;

    اگر (escape && vim_strchr(val, ' ') != پوچ)
	p = vim_strsave_escaped(val, (char_u *)" ");
    وگرنه
	p = vim_strsave(val);
    اگر (p == پوچ)		// we don't want a NULL
	بازگشت;

    opt_idx = findoption((char_u *)name);
    اگر (opt_idx < 0)
    {
	vim_free(p);
	بازگشت;
    }

    اگر (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    تهی
set_string_default(نویسه *name, char_u *val)
{
    set_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    ایستا char_u *
find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags)
{
    صحیح	    bs = 0;
    char_u  *s;

    اگر (origval == پوچ)
	بازگشت پوچ;

    برای (s = origval; *s != NUL; ++s)
    {
	اگر ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    بازگشت s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	اگر ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	وگرنه
	    bs = 0;
    }
    بازگشت پوچ;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    تهی
set_عدد_default(نویسه *name, long val)
{
    صحیح		opt_idx;

    opt_idx = findoption((char_u *)name);
    اگر (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    تهی
set_local_options_default(win_T *wp, صحیح do_buffer)
{
    win_T	*save_curwin = curwin;
    صحیح		i;

    curwin = wp;
    curbuf = curwin->w_buffer;
    block_autocmds();

    برای (i = 0; !istermoption_idx(i); i++)
    {
	ساختار vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	اگر (p->indir != PV_NONE
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

#اگر defined(EXITFREE)
/*
 * Free all options.
 */
    تهی
free_all_options(تهی)
{
    صحیح		i;

    برای (i = 0; !istermoption_idx(i); i++)
    {
	اگر (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    اگر ((options[i].flags & P_ALLOCED) && options[i].var != پوچ)
		free_string_option(*(char_u **)options[i].var);
	    اگر (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	وگرنه اگر (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# اگر defined(FEAT_EVAL)
    free_findfunc_option();
# endif
}
#endif


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    تهی
set_init_2(تهی)
{
    صحیح		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    اگر (idx >= 0 && !(options[idx].flags & P_WAS_SET))
	set_option_default(idx, OPT_LOCAL, p_cp);
    comp_col();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    اگر (!option_was_set((char_u *)"window"))
	p_window = Rows - 1;
    set_عدد_default("window", Rows - 1);

    // For DOS console the default is always black.
#اگر !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    اگر (idx >= 0 && !(options[idx].flags & P_WAS_SET)
						 && *term_bg_default() == 'd')
    {
	set_string_option_direct(پوچ, idx, (char_u *)"dark", OPT_FREE, 0);
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
    (تهی)parse_printoptions(پوچ);	// parse 'printoptions' default value
#endif
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    تهی
set_init_3(تهی)
{
#اگر defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    صحیح	    idx_srr;
    صحیح	    do_srr;
# ifdef FEAT_QUICKFIX
    صحیح	    idx_sp;
    صحیح	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    اگر (idx_srr < 0)
	do_srr = FALSE;
    وگرنه
	do_srr = !(options[idx_srr].flags & P_WAS_SET);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    اگر (idx_sp < 0)
	do_sp = FALSE;
    وگرنه
	do_sp = !(options[idx_sp].flags & P_WAS_SET);
# endif
    p = get_isolated_shell_name();
    اگر (p != پوچ)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	اگر (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# اگر defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# اگر defined(FEAT_QUICKFIX)
	    اگر (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  وگرنه
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    اگر (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	وگرنه اگر (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  اگر defined(FEAT_QUICKFIX)
		اگر (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		اگر (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	وگرنه
	    // Always use POSIX shell style redirection if we reach this
	    اگر (       fnamecmp(p, "sh") == 0
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
# اگر defined(FEAT_QUICKFIX)
		اگر (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  وگرنه
		    اگر (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    وگرنه
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		اگر (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#endif

#اگر defined(MSWIN)
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
    اگر (strstr((نویسه *)gettail(p_sh), "powershell") != پوچ)
    {
	صحیح	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	اگر (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	اگر (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    وگرنه اگر (strstr((نویسه *)gettail(p_sh), "sh") != پوچ)
    {
	صحیح	idx3;

	idx3 = findoption((char_u *)"shcf");
	اگر (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	اگر (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    وگرنه اگر (strstr((نویسه *)gettail(p_sh), "cmd.exe") != پوچ)
    {
	صحیح	idx3;

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
	اگر (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	اگر (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#endif

    اگر (BUFEMPTY())
    {
	صحیح idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	اگر (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_SET))
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    set_title_defaults();
}

#اگر defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    تهی
set_helplang_default(char_u *lang)
{
    صحیح		idx;
    size_t	langlen;

    اگر (lang == پوچ)	// safety check
	بازگشت;

    langlen = STRLEN(lang);
    اگر (langlen < 2)	// safety check
	بازگشت;

    idx = findoption((char_u *)"hlg");
    اگر (idx < 0 || (options[idx].flags & P_WAS_SET))
	بازگشت;

    اگر (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    اگر (p_hlg == پوچ)
	p_hlg = empty_option;
    وگرنه
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	اگر (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	وگرنه اگر (langlen >= 1 && *p_hlg == 'C')
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
    تهی
set_title_defaults(تهی)
{
    صحیح	    idx1;
    long    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    اگر (idx1 >= 0 && !(options[idx1].flags & P_WAS_SET))
    {
#ifdef FEAT_GUI
	اگر (gui.starting || gui.in_use)
	    val = TRUE;
	وگرنه
#endif
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    اگر (idx1 < 0 || (options[idx1].flags & P_WAS_SET))
    {
	بازگشت;
    }

#ifdef FEAT_GUI
    اگر (gui.starting || gui.in_use)
	val = TRUE;
    وگرنه
#endif
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    تهی
ex_set(exarg_T *eap)
{
    صحیح		flags = 0;

    اگر (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    وگرنه اگر (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#اگر defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    اگر ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    وگرنه
#endif
    {
	اگر (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(تهی)do_set(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
تعریفنوع شمارش {
    PREFIX_NO = 0,	// "no" prefix
    PREFIX_NONE,	// no prefix
    PREFIX_INV,		// "inv" prefix
} set_prefix_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    ایستا set_prefix_T
get_option_prefix(char_u **argp)
{
    صحیح		prefix = PREFIX_NONE;
    char_u	*arg = *argp;

    اگر (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	prefix = PREFIX_NO;
	arg += 2;
    }
    وگرنه اگر (STRNCMP(arg, "inv", 3) == 0)
    {
	prefix = PREFIX_INV;
	arg += 3;
    }

    *argp = arg;
    بازگشت prefix;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    ایستا صحیح
parse_option_name(char_u *arg, صحیح *opt_idxp, صحیح *lenp, صحیح *keyp)
{
    صحیح	کلید = 0;
    صحیح	len;
    صحیح	opt_idx;

    اگر (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	اگر (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	وگرنه
	{
	    len = 1;
	    وقتی (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	اگر (arg[len] != '>')
	    بازگشت FAIL;

	arg[len] = NUL;			    // put NUL after name
	اگر (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	اگر (opt_idx == -1)
	    کلید = find_کلید_option(arg + 1, TRUE);
    }
    وگرنه
    {
	صحیح	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	اگر (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	وگرنه
	    وقتی (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	اگر (opt_idx == -1)
	    کلید = find_کلید_option(arg, FALSE);
    }

    *keyp = کلید;
    *lenp = len;
    *opt_idxp = opt_idx;

    بازگشت OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    ایستا set_op_T
get_opt_op(char_u *arg)
{
    set_op_T op = OP_NONE;

    اگر (*arg != NUL && *(arg + 1) == '=')
    {
	اگر (*arg == '+')
	    op = OP_ADDING;		// "+="
	وگرنه اگر (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	وگرنه اگر (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    بازگشت op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
ایستا نویسه *modeline_whitelist[] =
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
    پوچ
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    ایستا bool
is_modeline_whitelisted(نویسه *name)
{
    برای (صحیح i = 0; modeline_whitelist[i] != پوچ; i++)
	اگر (STRCMP(name, modeline_whitelist[i]) == 0)
	    بازگشت true;
    بازگشت false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    ایستا صحیح
validate_opt_idx(
	صحیح opt_idx,
	صحیح opt_flags,
	long_u flags,
	نویسه **errmsg,
	set_prefix_T prefix)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    اگر ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	بازگشت FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    اگر ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	بازگشت FAIL;

    // Disallow changing some options from modelines.
    اگر (opt_flags & OPT_MODELINE)
    {
	اگر (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_in_modeline;
	    بازگشت FAIL;
	}
	اگر ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_in_modeline_when_modelineexpr_is_off;
	    بازگشت FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	اگر (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    اگر (options[opt_idx].indir == PV_ML && prefix == PREFIX_NO)
		بازگشت OK;
	    وگرنه اگر (!is_modeline_whitelisted(options[opt_idx].fullname))
		بازگشت FAIL;
	}
#ifdef FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	اگر (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    بازگشت FAIL;
#endif
    }

#ifdef HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    اگر (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_in_sandbox;
	بازگشت FAIL;
    }
#endif

    بازگشت OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    ایستا char_u *
stropt_get_default_val(
    صحیح		opt_idx,
    char_u	*varp,
    صحیح		flags,
    صحیح		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    اگر ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ifdef FEAT_GUI
	اگر (gui.in_use)
	    newval = gui_bg_default();
	وگرنه
#endif
	    newval = term_bg_default();
    }
    وگرنه اگر ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    اگر (newval == پوچ)
	newval = empty_option;
    وگرنه
    {
	char_u *s = option_expand(opt_idx, newval);
	اگر (s == پوچ)
	    s = newval;
	newval = vim_strsave(s);
    }

    بازگشت newval;
}

/*
 * Convert the 'backspace' option number value to a string: for adding,
 * prepending and removing string.
 */
    ایستا تهی
opt_backspace_nr2str(
    char_u	*varp,
    char_u	**origval_p,
    char_u	**origval_l_p,
    char_u	**origval_g_p,
    char_u	**oldval_p)
{
    صحیح		i = getdigits((char_u **)varp);

    انتخاب (i)
    {
	حالت 0:
	    *(char_u **)varp = empty_option;
	    بشکن;
	حالت 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    بشکن;
	حالت 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    بشکن;
	حالت 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    بشکن;
    }
    vim_free(*oldval_p);
    اگر (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    اگر (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    اگر (*origval_g_p == *oldval_p)
	*origval_g_p = *(char_u **)varp;
    *oldval_p = *(char_u **)varp;
}

/*
 * Convert the 'whichwrap' option number value to a string, for backwards
 * compatibility with Vim 3.0.
 * Note: 'argp' is a pointer to a char_u pointer and is updated.
 */
    ایستا char_u *
opt_whichwrap_nr2str(char_u **argp, char_u *whichwrap)
{
    size_t  len = 0;

    *whichwrap = NUL;
    صحیح i = getdigits(argp);
    اگر (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    اگر (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    اگر (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    اگر (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    اگر (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    اگر (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    بازگشت whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    ایستا char_u *
stropt_copy_مقدار(
    char_u	*origval,
    char_u	**argp,
    set_op_T	op,
    صحیح		flags UNUSED)
{
    char_u	*arg = *argp;
    unsigned	newlen;
    char_u	*newval;
    char_u	*s = پوچ;

    // get a bit too much
    newlen = (unsigned)STRLEN(arg) + 1;
    اگر (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;
    newval = alloc(newlen);
    اگر (newval == پوچ)  // out of mem, don't change
	بازگشت پوچ;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    وقتی (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	صحیح i;

	اگر (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#endif
	   )
	    ++arg;	// remove backslash
	اگر (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	وگرنه
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    بازگشت newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    ایستا char_u *
stropt_expand_envvar(
    صحیح		opt_idx,
    char_u	*origval,
    char_u	*newval,
    set_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    اگر (s == پوچ)
	بازگشت newval;

    vim_free(newval);
    unsigned newlen = (unsigned)STRLEN(s) + 1;
    اگر (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;

    newval = alloc(newlen);
    اگر (newval == پوچ)
	بازگشت پوچ;

    STRCPY(newval, s);

    بازگشت newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    ایستا تهی
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    صحیح		flags)
{
    صحیح		len = 0;

    صحیح comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    اگر (op == OP_ADDING)
    {
	len = (صحیح)STRLEN(origval);
	// strip a trailing comma, would get 2
	اگر (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    وگرنه
    {
	len = (صحیح)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    اگر (comma)
	newval[len] = ',';
}

/*
 * Remove a value from a string option.  Copy string option value in "origval"
 * to "newval" and then remove the string "strval" of length "len".
 */
    ایستا تهی
stropt_remove_val(
    char_u	*origval,
    char_u	*newval,
    صحیح		flags,
    char_u	*strval,
    صحیح		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    اگر (*strval)
    {
	// may need to remove a comma
	اگر (flags & P_COMMA)
	{
	    اگر (strval == origval)
	    {
		// include comma after string
		اگر (strval[len] == ',')
		    ++len;
	    }
	    وگرنه
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
    ایستا char_u *
find_کلید_item(char_u *src, char_u *کلید, صحیح keylen, صحیح *itemlenp)
{
    char_u  *p = src;

    وقتی (*p != NUL)
    {
	// Check if this item starts with the same key
	اگر ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, کلید, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *end = vim_strchr(p, ',');
	    اگر (end == پوچ)
		end = p + STRLEN(p);
	    *itemlenp = (صحیح)(end - p);
	    بازگشت p;
	}
	++p;
    }
    بازگشت پوچ;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    ایستا تهی
remove_comma_item(char_u *str, char_u *item, صحیح itemlen)
{
    اگر (item[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(item, item + itemlen + 1);
    وگرنه اگر (item > str && *(item - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(item - 1, item + itemlen);
    وگرنه
	// Only item
	*item = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    ایستا تهی
remove_کلید_item(char_u *str, char_u *کلید, صحیح keylen, char_u *skip)
{
    صحیح	    itemlen;
    char_u  *found;

    وقتی ((found = find_کلید_item(str, کلید, keylen, &itemlen)) != پوچ)
    {
	اگر (found == skip)
	{
	    // Search for the next match after this one.
	    char_u *next = found + itemlen;
	    اگر (*next == ',')
		++next;
	    found = find_کلید_item(next, کلید, keylen, &itemlen);
	    اگر (found == پوچ)
		بشکن;
	}

	remove_comma_item(str, found, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    ایستا تهی
append_item(char_u *str, char_u *item, صحیح item_len)
{
    صحیح len = (صحیح)STRLEN(str);

    اگر (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, item, (size_t)item_len);
    str[len + item_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    ایستا تهی
prepend_item(char_u *str, char_u *item, صحیح item_len)
{
    صحیح len = (صحیح)STRLEN(str);
    صحیح comma = (len > 0) ? 1 : 0;

    mch_memmove(str + item_len + comma, str, (size_t)len + 1);
    mch_memmove(str, item, (size_t)item_len);
    اگر (comma)
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
    ایستا bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    صحیح		flags UNUSED)
{
    char_u  *p;
    char_u  *item_start;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    اگر (vim_strchr(newval, ':') == پوچ && vim_strchr(newval, ',') == پوچ)
	بازگشت false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    اگر (newval_copy == پوچ)
	بازگشت false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    item_start = newval_copy;
    برای (;;)
    {
	p = vim_strchr(item_start, ',');
	صحیح item_len = (p == پوچ)
			    ? (صحیح)STRLEN(item_start) : (صحیح)(p - item_start);

	اگر (item_len > 0)
	{
	    char_u *colon = vim_strchr(item_start, ':');
	    اگر (colon != پوچ && colon < item_start + item_len)
	    {
		صحیح keylen = (صحیح)(colon - item_start) + 1;

		اگر (op == OP_ADDING || op == OP_PREPENDING)
		{
		    صحیح old_itemlen;
		    char_u *found = find_کلید_item(newval, item_start,
						       keylen, &old_itemlen);
		    اگر (found != پوچ)
		    {
			اگر (old_itemlen == item_len
				&& STRNCMP(found, item_start,
							 item_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_کلید_item(newval, item_start,
							   keylen, found);
			}
			وگرنه
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_کلید_item(newval, item_start,
							    keylen, پوچ);
			    اگر (op == OP_PREPENDING)
				prepend_item(newval, item_start, item_len);
			    وگرنه
				append_item(newval, item_start, item_len);
			}
		    }
		    وگرنه
		    {
			// New item.
			اگر (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			وگرنه
			    append_item(newval, item_start, item_len);
		    }
		}
		وگرنه اگر (op == OP_REMOVING)
		    remove_کلید_item(newval, item_start, keylen, پوچ);
	    }
	    وگرنه
	    {
		اگر (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    اگر (found == پوچ)
		    {
			// New item.
			اگر (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			وگرنه
			    append_item(newval, item_start, item_len);
		    }
		    // else: exact duplicate — do nothing
		}
		وگرنه اگر (op == OP_REMOVING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    اگر (found != پوچ)
			remove_comma_item(newval, found, item_len);
		}
	    }
	}

	اگر (p == پوچ)
	    بشکن;
	item_start = p + 1;
    }

    vim_free(newval_copy);

    بازگشت true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    ایستا تهی
stropt_remove_dupflags(char_u *newval, صحیح flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    وقتی (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	اگر (flags & P_ONECOMMA)
	{
	    اگر (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != پوچ)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		ادامه;
	    }
	}
	وگرنه
	{
	    اگر ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != پوچ)
	    {
		STRMOVE(s, s + 1);
		ادامه;
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
    ایستا char_u *
stropt_get_newval(
    صحیح		nextchar,
    صحیح		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    set_op_T    *op_arg,
    صحیح		flags,
    صحیح		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    set_op_T    op = *op_arg;
    char_u	*save_arg = پوچ;
    char_u	*newval;
    char_u	*s = پوچ;
    char_u	whichwrap[80];

    اگر (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    وگرنه اگر (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    وگرنه
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	اگر (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	وگرنه اگر (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	وگرنه اگر (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	وگرنه اگر (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_مقدار(origval, &arg, op, flags);
	اگر (newval == پوچ)
	    برو done;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	اگر (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    اگر (newval == پوچ)
		برو done;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	اگر ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	وگرنه
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    صحیح len = 0;
	    اگر (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (صحیح)STRLEN(newval);
		s = find_dup_item(origval, newval, len, flags);

		// do not add if already there
		اگر ((op == OP_ADDING || op == OP_PREPENDING) && s != پوچ)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		اگر (s == پوچ)
		    s = origval + (صحیح)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    اگر (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    وگرنه اگر (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	اگر (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

done:
    اگر (save_arg != پوچ)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    بازگشت newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    ایستا صحیح
do_set_option_string(
	صحیح	    opt_idx,
	صحیح	    opt_flags,
	char_u	    **argp,
	صحیح	    nextchar,
	set_op_T    op_arg,
	long_u	    flags,
	صحیح	    cp_val,
	char_u	    *varp_arg,
	نویسه	    *errbuf,
	size_t	    errbuflen,
	صحیح	    *مقدار_checked,
	نویسه	    **errmsg)
{
    char_u	*arg = *argp;
    set_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = پوچ; // previous value if *varp
    char_u	*newval;
    char_u	*origval = پوچ;
    char_u	*origval_l = پوچ;
    char_u	*origval_g = پوچ;
#اگر defined(FEAT_EVAL)
    char_u	*saved_origval = پوچ;
    char_u	*saved_origval_l = پوچ;
    char_u	*saved_origval_g = پوچ;
    char_u	*saved_newval = پوچ;
#endif

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((صحیح)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	اگر (((صحیح)options[opt_idx].indir & PV_BOTH)
						  && origval_l == empty_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    اگر (((صحیح)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    وگرنه
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    اگر (newval == پوچ)
	*(char_u **)(varp) = empty_option;

#اگر defined(FEAT_EVAL)
    اگر (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_کلید
# endif
		      && origval != پوچ && newval != پوچ)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	اگر (origval_l != پوچ)
	    saved_origval_l = vim_strsave(origval_l);
	اگر (origval_g != پوچ)
	    saved_origval_g = vim_strsave(origval_g);
    }
#endif

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	صحیح	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	اگر ((opt_flags & OPT_MODELINE)
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
			errbuflen, opt_flags, op, مقدار_checked);

	secure = secure_saved;
    }

#اگر defined(FEAT_EVAL)
    اگر (*errmsg == پوچ)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#endif

    *argp = arg;
    بازگشت *errmsg == پوچ ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    ایستا نویسه *
do_set_option_bool(
    صحیح		opt_idx,
    صحیح		opt_flags,
    set_prefix_T prefix,
    long_u	flags,
    char_u	*varp,
    صحیح		nextchar,
    صحیح		afterchar,
    صحیح		cp_val)

{
    varnumber_T	مقدار;

    اگر (nextchar == '=' || nextchar == ':')
	بازگشت e_invalid_argument;
    اگر (opt_idx < 0 || varp == پوچ)
	بازگشت پوچ;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    اگر (nextchar == '!')
	مقدار = *(صحیح *)(varp) ^ 1;
    وگرنه اگر (nextchar == '&')
	مقدار = (صحیح)(long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    وگرنه اگر (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	اگر ((صحیح *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    مقدار = -1;
	وگرنه اگر ((صحیح *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    مقدار = -1;
#ifdef HAVE_FSYNC
	وگرنه اگر ((صحیح *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    مقدار = -1;
#endif
	وگرنه
	    مقدار = *(صحیح *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    وگرنه
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	اگر (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    بازگشت e_trailing_characters;
	اگر (prefix == PREFIX_INV)
	    مقدار = *(صحیح *)(varp) ^ 1;
	وگرنه
	    مقدار = prefix == PREFIX_NO ? 0 : 1;
    }

    بازگشت set_bool_option(opt_idx, varp, (صحیح)مقدار, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    ایستا نویسه *
do_set_option_numeric(
    صحیح		opt_idx,
    صحیح		opt_flags,
    char_u	**argp,
    صحیح		nextchar,
    set_op_T	op,
    long_u	flags,
    صحیح		cp_val,
    char_u	*varp,
    نویسه	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		مقدار;
    صحیح			i;
    نویسه		*errmsg = پوچ;

    اگر (opt_idx < 0 || varp == پوچ)
	بازگشت پوچ;  // "cannot happen"
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
    اگر (nextchar == '&')
	مقدار = (long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    وگرنه اگر (nextchar == '<')
    {
	اگر ((long *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    مقدار = NO_LOCAL_UNDOLEVEL;
	وگرنه اگر (opt_flags == OPT_LOCAL
		    && ((long *)varp == &curwin->w_p_siso
		     || (long *)varp == &curwin->w_p_so
		     || (long *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    مقدار = -1;
	وگرنه
	    مقدار = *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    وگرنه اگر (((long *)varp == &p_wc || (long *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	مقدار = string_to_کلید(arg, FALSE);
	اگر (مقدار == 0)
	{
	    errmsg = e_invalid_argument;
	    برو skip;
	}
    }
    وگرنه اگر (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, پوچ, &i, STR2NR_ALL, &مقدار, پوچ, 0, TRUE, پوچ);
	اگر (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_عدد_required_after_equal;
	    برو skip;
	}
    }
    وگرنه
    {
	errmsg = e_عدد_required_after_equal;
	برو skip;
    }

    اگر (op == OP_ADDING)
	مقدار = *(long *)varp + مقدار;
    وگرنه اگر (op == OP_PREPENDING)
	مقدار = *(long *)varp * مقدار;
    وگرنه اگر (op == OP_REMOVING)
	مقدار = *(long *)varp - مقدار;

    اگر ((long *)varp == &curwin->w_p_sop && مقدار < -1)
    {
	errmsg = e_invalid_argument;
	برو skip;
    }

    errmsg = set_رقم_option(opt_idx, varp, مقدار, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    بازگشت errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    ایستا نویسه *
do_set_option_keycode(char_u **argp, char_u *کلید_name, صحیح nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    اگر (nextchar == '&')
    {
	اگر (add_termcap_entry(کلید_name, TRUE) == FAIL)
	    بازگشت e_not_found_in_termcap;
    }
    وگرنه
    {
	++arg; // jump to after the '=' or ':'
	برای (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    اگر (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	add_termcode(کلید_name, arg, FALSE);
	*p = nextchar;
    }
    اگر (full_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    بازگشت پوچ;
}

/*
 * Set an option to a new value.
 */
    ایستا نویسه *
do_set_option_مقدار(
    صحیح		opt_idx,
    صحیح		opt_flags,
    char_u	**argp,
    set_prefix_T prefix,
    set_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*کلید_name,
    صحیح		nextchar,
    صحیح		afterchar,
    صحیح		cp_val,
    صحیح		*stopopteval,
    نویسه	*errbuf,
    size_t	errbuflen)
{
    صحیح		مقدار_checked = FALSE;
    نویسه	*errmsg = پوچ;
    char_u	*arg = *argp;

    اگر (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_set_option_bool(opt_idx, opt_flags, prefix, flags, varp,
						nextchar, afterchar, cp_val);
	اگر (errmsg != پوچ)
	    برو skip;
    }
    وگرنه
    {
	// numeric or string option
	اگر (vim_strchr((char_u *)"=:&<", nextchar) == پوچ
		|| prefix != PREFIX_NONE)
	{
	    errmsg = e_invalid_argument;
	    برو skip;
	}

	اگر (flags & P_رقم)
	{
	    // numeric option
	    errmsg = do_set_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    اگر (errmsg != پوچ)
		برو skip;
	}
	وگرنه اگر (opt_idx >= 0)
	{
	    // string option
	    اگر (do_set_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&مقدار_checked, &errmsg) == FAIL)
	    {
		اگر (errmsg != پوچ)
		    برو skip;
		*stopopteval = TRUE;
		برو skip;
	    }
	}
	وگرنه
	{
	    // key code option
	    errmsg = do_set_option_keycode(&arg, کلید_name, nextchar);
	    اگر (errmsg != پوچ)
		برو skip;
	}
    }

    اگر (opt_idx >= 0)
	did_set_option(opt_idx, opt_flags, op == OP_NONE, مقدار_checked);

skip:
    *argp = arg;
    بازگشت errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    ایستا نویسه *
do_set_option(
    صحیح		opt_flags,
    char_u	**argp,
    char_u	*arg_start,
    char_u	**startarg,
    صحیح		*did_show,
    صحیح		*stopopteval,
    نویسه	*errbuf,
    size_t	errbuflen)
{
    صحیح		opt_idx;
    char_u	*arg;
    set_prefix_T prefix;	// no prefix, "no" prefix or "inv" prefix
    set_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	کلید_name[2];
    صحیح		nextchar;	// next non-white char after option name
    صحیح		afterchar;	// character just after option name
    نویسه	*errmsg = پوچ;
    صحیح		کلید;
    صحیح		len;

    prefix = get_option_prefix(argp);
    arg = *argp;

    // find end of name
    کلید = 0;
    اگر (parse_option_name(arg, &opt_idx, &len, &کلید) == FAIL)
	بازگشت e_invalid_argument;

    // remember character after option name
    afterchar = arg[len];

    اگر (in_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	اگر (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != پوچ
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    برو skip;
	}
    }
    وگرنه
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	وقتی (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    اگر (op != OP_NONE)
	len++;

    nextchar = arg[len];

    اگر (opt_idx == -1 && کلید == 0)	// found a mismatch: skip
    {
	اگر (in_vim9script() && arg > arg_start
		&& vim_strchr((char_u *)"!&<", *arg) != پوچ)
	    errmsg = e_no_white_space_allowed_between_option_and;
	وگرنه
	    errmsg = e_unknown_option;
	برو skip;
    }

    اگر (opt_idx >= 0)
    {
	اگر (options[opt_idx].var == پوچ)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    اگر (vim_strchr((char_u *)"=:!&<", nextchar) == پوچ
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    برو skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    وگرنه
    {
	flags = P_STRING;
	varp = پوچ;
	اگر (کلید < 0)
	{
	    کلید_name[0] = KEY2TERMCAP0(کلید);
	    کلید_name[1] = KEY2TERMCAP1(کلید);
	}
	وگرنه
	{
	    کلید_name[0] = KS_کلید;
	    کلید_name[1] = (کلید & 0xff);
	}
    }

    // Make sure the option value can be changed.
    اگر (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, prefix) == FAIL)
	برو skip;

    صحیح cp_val = p_cp;
    اگر (vim_strchr((char_u *)"?=:!&<", nextchar) != پوچ)
    {
	arg += len;
	اگر (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    اگر (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    وگرنه		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	اگر (vim_strchr((char_u *)"?!&<", nextchar) != پوچ
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    برو skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    اگر (nextchar == '?'
	    || (prefix == PREFIX_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == پوچ
		&& !(flags & P_BOOL)))
    {
	// print value
	اگر (*did_show)
	    پیام_putchar('\n');	    // cursor below last one
	وگرنه
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	اگر (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ifdef FEAT_EVAL
	    اگر (p_verbose > 0)
	    {
		// Mention where the option was last set.
		اگر (varp == options[opt_idx].var)
		    last_set_پیام(options[opt_idx].script_ctx);
		وگرنه اگر ((صحیح)options[opt_idx].indir & PV_WIN)
		    last_set_پیام(curwin->w_p_script_ctx[
			    (صحیح)options[opt_idx].indir & PV_MASK]);
		وگرنه اگر ((صحیح)options[opt_idx].indir & PV_BUF)
		    last_set_پیام(curbuf->b_p_script_ctx[
			    (صحیح)options[opt_idx].indir & PV_MASK]);
	    }
#endif
	}
	وگرنه
	{
	    char_u	    *p;

	    p = find_termcode(کلید_name);
	    اگر (p == پوچ)
	    {
		errmsg = e_کلید_code_not_set;
		برو skip;
	    }
	    وگرنه
		(تهی)show_one_termcode(کلید_name, p, TRUE);
	}
	اگر (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    وگرنه
    {
	errmsg = do_set_option_مقدار(opt_idx, opt_flags, &arg, prefix, op,
					flags, varp, کلید_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    بازگشت errmsg;
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
    صحیح
do_set(
    char_u	*arg_start,	// option string (may be written to!)
    صحیح		opt_flags)
{
    char_u	*arg = arg_start;
    صحیح		i;
    صحیح		did_show = FALSE;   // already showed one value

    اگر (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	برو theend;
    }

    وقتی (*arg != NUL)		// loop to process all options
    {
	اگر (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    اگر (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		set_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    وگرنه
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	وگرنه اگر (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	وگرنه
	{
	    صحیح		stopopteval = FALSE;
	    نویسه	*errmsg = پوچ;
	    نویسه	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_set_option(opt_flags, &arg, arg_start, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    اگر (stopopteval)
		بشکن;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    برای (i = 0; i < 2 ; ++i)
	    {
		وقتی (*arg != NUL && !VIM_ISWHITE(*arg))
		    اگر (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		اگر (*arg != '=')
		    بشکن;
	    }

	    اگر (errmsg != پوچ)
	    {
		i = vim_snprintf((نویسه *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		اگر (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((نویسه *)IObuff);	// show error highlighted
		--no_wait_return;

		بازگشت FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    اگر (silent_mode && did_show)
    {
	// After displaying option values in silent mode.
	silent_mode = FALSE;
	info_message = TRUE;	// use mch_msg(), not mch_errmsg()
	پیام_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = TRUE;
	info_message = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    بازگشت OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    تهی
did_set_option(
    صحیح	    opt_idx,
    صحیح	    opt_flags,	    // possibly with OPT_MODELINE
    صحیح	    new_مقدار_2,	    // value was replaced completely
    صحیح	    مقدار_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = پوچ;
    صحیح		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_SET;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    اگر (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    اگر (!مقدار_checked && (secure
#ifdef HAVE_SANDBOX
	    || sandbox != 0
#endif
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	اگر (flagsp_local != پوچ)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    وگرنه اگر (new_مقدار_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	اگر (flagsp_local != پوچ)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    صحیح
string_to_کلید(char_u *arg, صحیح multi_byte)
{
    اگر (*arg == '<' && arg[1])
	بازگشت find_کلید_option(arg + 1, TRUE);
    اگر (*arg == '^' && arg[1])
    {
	صحیح کلید = Ctrl_chr(arg[1]);
	اگر (کلید == 0)		// ^@ is <Nul>
	    کلید = K_ZERO;
	بازگشت کلید;
    }
    اگر (multi_byte)
	بازگشت PTR2CHAR(arg);
    بازگشت *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    تهی
did_set_title(تهی)
{
    اگر (starting != NO_SCREEN
#ifdef FEAT_GUI
	    && !gui.starting
#endif
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    تهی
set_options_bin(
    صحیح		oldval,
    صحیح		newval,
    صحیح		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    اگر (newval)
    {
	اگر (!oldval)		// switched on
	{
	    اگر (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    اگر (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	اگر (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	اگر (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    وگرنه اگر (oldval)		// switched off
    {
	اگر (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	اگر (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#اگر defined(FEAT_EVAL)
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
    ایستا char_u *
option_expand(صحیح opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    اگر (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == پوچ)
	بازگشت پوچ;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    اگر (val != پوچ && STRLEN(val) > MAXPATHL)
	بازگشت پوچ;

    اگر (val == پوچ)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    صحیح esc = var == &p_tags || var == &p_path;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : پوچ, FALSE,
#ifdef FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#endif
				  پوچ);
    اگر (STRCMP(NameBuff, val) == 0)   // they are the same
	بازگشت پوچ;

    بازگشت NameBuff;
}

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    ایستا تهی
didset_options(تهی)
{
    // initialize the table for 'iskeyword' et.al.
    (تهی)init_chartab();

    didset_string_options();

#ifdef FEAT_SPELL
    (تهی)spell_check_msm();
    (تهی)spell_check_sps();
    (تهی)compile_cap_prog(curwin->w_s);
    (تهی)did_set_spell_option();
#endif
    // set cedit_key
    (تهی)did_set_cedit(پوچ);
#ifdef FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_set_breakat(پوچ);
#endif
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    ایستا تهی
didset_options2(تهی)
{
    // Initialize the highlight_attr[] table.
    (تهی)highlight_changed();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (تهی)set_listchars_option(curwin, curwin->w_p_lcs, TRUE, پوچ, 0);

    // Parse default for 'fillchars'.
    (تهی)set_fillchars_option(curwin, curwin->w_p_fcs, TRUE, پوچ, 0);

#ifdef FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (تهی)did_set_clipboard(پوچ);
#endif
#ifdef FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (تهی)tabstop_set(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (تهی)tabstop_set(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#endif
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    تهی
check_options(تهی)
{
    صحیح		opt_idx;

    برای (opt_idx = 0; options[opt_idx].fullname != پوچ; opt_idx++)
	اگر ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != پوچ)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    صحیح
get_term_opt_idx(char_u **p)
{
    صحیح opt_idx;

    برای (opt_idx = 1; options[opt_idx].fullname != پوچ; opt_idx++)
	اگر (options[opt_idx].var == (char_u *)p)
	    بازگشت opt_idx;
    بازگشت -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    صحیح
set_term_option_alloced(char_u **p)
{
    صحیح		opt_idx = get_term_opt_idx(p);

    اگر (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    بازگشت opt_idx;
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    صحیح
was_set_insecurely(win_T *wp, char_u *opt, صحیح opt_flags)
{
    صحیح	    idx = findoption(opt);
    long_u  *flagp;

    اگر (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	بازگشت (*flagp & P_INSECURE) != 0;
    }
    internal_error("was_set_insecurely()");
    بازگشت -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    ایستا long_u *
insecure_flag(win_T *wp, صحیح opt_idx, صحیح opt_flags)
{
    اگر (opt_flags & OPT_LOCAL)
	انتخاب ((صحیح)options[opt_idx].indir)
	{
	    حالت PV_WRAP:	بازگشت &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    حالت PV_STL:	بازگشت &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    حالت PV_FDE:	بازگشت &wp->w_p_fde_flags;
	    حالت PV_FDT:	بازگشت &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    حالت PV_BEXPR:	بازگشت &wp->w_buffer->b_p_bexpr_flags;
#  endif
	    حالت PV_INDE:	بازگشت &wp->w_buffer->b_p_inde_flags;
	    حالت PV_FEX:	بازگشت &wp->w_buffer->b_p_fex_flags;
#  ifdef FEAT_FIND_ID
	    حالت PV_INEX:	بازگشت &wp->w_buffer->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    حالت PV_CPT:	بازگشت &wp->w_buffer->b_p_cpt_flags;
#  endif
# endif
	}
    وگرنه
	// For global value of window-local options, use flags in w_allbuf_opt.
	انتخاب ((صحیح)options[opt_idx].indir)
	{
	    حالت PV_WRAP:	بازگشت &wp->w_allbuf_opt.wo_wrap_flags;
# اگر defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    حالت PV_FDE:	بازگشت &wp->w_allbuf_opt.wo_fde_flags;
	    حالت PV_FDT:	بازگشت &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    بازگشت &options[opt_idx].flags;
}
#endif

/*
 * Redraw the window title and/or tab page text later.
 */
    تهی
redraw_titles(تهی)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    صحیح
valid_name(char_u *val, نویسه *allowed)
{
    char_u *s;

    برای (s = val; *s != NUL; ++s)
	اگر (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == پوچ)
	    بازگشت FALSE;
    بازگشت TRUE;
}

#اگر defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    تهی
set_option_sctx_idx(صحیح opt_idx, صحیح opt_flags, sctx_T script_ctx)
{
    صحیح		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    صحیح		indir = (صحیح)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    اگر (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    اگر (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    اگر (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    اگر (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    اگر (both || (opt_flags & OPT_LOCAL))
    {
	اگر (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	وگرنه اگر (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	    اگر (both)
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
is_option_مقدار_vim9(صحیح opt_idx, صحیح opt_flags)
{
    صحیح		indir = (صحیح)options[opt_idx].indir;
    sctx_T	*sctx = پوچ;

    اگر ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    اگر ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	اگر (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_MASK];
	وگرنه اگر (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_MASK];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    بازگشت !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_version >= SCRIPT_VERSION_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(نویسه *name)
{
    صحیح idx = findoption((char_u *)name);

    اگر (idx >= 0)
	بازگشت &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    بازگشت پوچ;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    تهی
set_term_option_sctx_idx(نویسه *name, صحیح opt_idx)
{
    char_u  buf[5];
    صحیح	    idx;

    اگر (name == پوچ)
	idx = opt_idx;
    وگرنه
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    اگر (idx >= 0)
	set_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#endif

#اگر defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    ایستا تهی
apply_optionset_autocmd(
	صحیح	opt_idx,
	long	opt_flags,
	long	oldval,
	long	oldval_g,
	long	newval,
	نویسه	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    اگر (starting || errmsg != پوچ || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	بازگشت;

    len = vim_snprintf_safelen((نویسه *)buf_new, اندازه(buf_new), "%ld", newval);
    set_vim_var_string(VV_OPTION_NEW, buf_new, (صحیح)len);
    len = vim_snprintf_safelen((نویسه *)buf_type, اندازه(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (صحیح)len);

    buf_oldlen = vim_snprintf_safelen((نویسه *)buf_old, اندازه(buf_old), "%ld", oldval);
    set_vim_var_string(VV_OPTION_OLD, buf_old, (صحیح)buf_oldlen);

    اگر (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (صحیح)buf_oldlen);
    }
    اگر (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (صحیح)buf_oldlen);
    }
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((نویسه *)buf_old_global,
	    اندازه(buf_old_global), "%ld", oldval_g);

	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (صحیح)buf_oldlen);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (صحیح)buf_old_globallen);
    }
    اگر (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (صحیح)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    پوچ, FALSE, پوچ);
    reset_v_option_vars();
}
#endif

#اگر defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    نویسه *
did_set_arabic(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	اگر (!p_tbidi)
	{
	    // set rightleft mode
	    اگر (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		changed_window_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    اگر (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	اگر (STRCMP(p_enc, "utf-8") != 0)
	{
	    ایستا نویسه *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    پیام_source(HL_ATTR(HLF_W));
	    پیام_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = set_option_مقدار((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    وگرنه
    {
	// 'arabic' is reset, handle various sub-settings.
	اگر (!p_tbidi)
	{
	    // reset rightleft mode
	    اگر (curwin->w_p_rl)
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
	curbuf->b_p_imsearch = B_IMODE_USE_درج;
# endif
    }

    بازگشت errmsg;
}
#endif

#اگر defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    نویسه *
did_set_autochdir(optset_T *args UNUSED)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    نویسه *
did_set_ballooneval(optset_T *args)
{
    اگر (balloonEvalForTerm)
	بازگشت پوچ;

    اگر (p_beval && !args->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    وگرنه اگر (!p_beval && args->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_BEVAL_TERM)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    نویسه *
did_set_balloonevalterm(optset_T *args UNUSED)
{
    mch_bevalterm_changed();
    بازگشت پوچ;
}
#endif

/*
 * Process the updated 'binary' option value.
 */
    نویسه *
did_set_دودویی(optset_T *args)
{
    // when 'bin' is set also set some other options
    set_options_bin(args->os_oldval.boolean, curbuf->b_p_bin, args->os_flags);
    redraw_titles();

    بازگشت پوچ;
}

/*
 * Process the updated 'buflisted' option value.
 */
    نویسه *
did_set_buflisted(optset_T *args)
{
    // when 'buflisted' changes, trigger autocommands
    اگر (args->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						پوچ, پوچ, TRUE, curbuf);
    بازگشت پوچ;
}

/*
 * Process the new 'cmdheight' option value.
 */
    نویسه *
did_set_cmdheight(optset_T *args)
{
    long old_مقدار = args->os_oldval.عدد;
    نویسه *errmsg = پوچ;

    // if p_ch changed value, change the command line height
    اگر (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_argument_must_be_positive;
	p_ch = MIN_CMDHEIGHT;
    }
    اگر (p_ch > Rows - min_rows() + MIN_CMDHEIGHT)
	p_ch = Rows - min_rows() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    اگر ((p_ch != old_مقدار
		|| tabline_height() + topframe->fr_height != Rows - p_ch)
	    && full_screen
#ifdef FEAT_GUI
	    && !gui.starting
#endif
       )
	command_height();

    بازگشت errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    نویسه *
did_set_compatible(optset_T *args UNUSED)
{
    compatible_set();
    بازگشت پوچ;
}

#اگر defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    نویسه *
did_set_conceallevel(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (curwin->w_p_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_cole = 0;
    }
    وگرنه اگر (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_cole = 3;
    }
    اگر (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    وگرنه اگر (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    بازگشت errmsg;
}
#endif

#اگر defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    نویسه *
did_set_diff(optset_T *args UNUSED)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    اگر (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    بازگشت پوچ;
}
#endif

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    نویسه *
did_set_eof_eol_fixeol_bomb(optset_T *args UNUSED)
{
    // redraw the window title and tab page text
    redraw_titles();
    بازگشت پوچ;
}

/*
 * Process the updated 'equalalways' option value.
 */
    نویسه *
did_set_equalalways(optset_T *args)
{
    اگر (p_ea && !args->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    بازگشت پوچ;
}

#اگر defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    نویسه *
did_set_foldcolumn(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (curwin->w_p_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_fdc = 0;
    }
    وگرنه اگر (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_fdc = 12;
    }
    اگر (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    وگرنه اگر (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    بازگشت errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    نویسه *
did_set_foldlevel(optset_T *args UNUSED)
{
    اگر (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    بازگشت پوچ;
}

/*
 * Process the new 'foldminlines' option value.
 */
    نویسه *
did_set_foldminlines(optset_T *args UNUSED)
{
    foldUpdateAll(curwin);
    بازگشت پوچ;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    نویسه *
did_set_foldnestmax(optset_T *args UNUSED)
{
    اگر (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_جستجو_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    نویسه *
did_set_hlsearch(optset_T *args UNUSED)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    set_no_hlsearch(FALSE);
    بازگشت پوچ;
}
#endif

/*
 * Process the updated 'ignorecase' option value.
 */
    نویسه *
did_set_ignorecase(optset_T *args UNUSED)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    اگر (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    بازگشت پوچ;
}

#اگر defined(HAVE_درون_داد_METHOD)
/*
 * Process the updated 'imdisable' option value.
 */
    نویسه *
did_set_imdisable(optset_T *args UNUSED)
{
    // Only de-activate it here, it will be enabled when changing mode.
    اگر (p_imdisable)
	im_set_active(FALSE);
    وگرنه اگر (State & MODE_درج)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
    بازگشت پوچ;
}
#endif

/*
 * Process the new 'iminsert' option value.
 */
    نویسه *
did_set_iminsert(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    اگر (termcap_active)	// don't do this in the alternate screen
	showmode();
#اگر defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif

    بازگشت errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    نویسه *
did_set_imsearch(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    بازگشت errmsg;
}

#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    نویسه *
did_set_imstyle(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_argument;

    بازگشت errmsg;
}
#endif

/*
 * Process the updated 'insertmode' option value.
 */
    نویسه *
did_set_insertmode(optset_T *args)
{
    // when 'insertmode' is set from an autocommand need to do work here
    اگر (p_im)
    {
	اگر ((State & MODE_درج) == 0)
	    need_start_insertmode = TRUE;
	stop_درج_mode = FALSE;
    }
    // only reset if it was set previously
    وگرنه اگر (args->os_oldval.boolean)
    {
	need_start_insertmode = FALSE;
	stop_درج_mode = TRUE;
	اگر (restart_edit != 0 && mode_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    بازگشت پوچ;
}

#اگر defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    نویسه *
did_set_langnoremap(optset_T *args UNUSED)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    بازگشت پوچ;
}

/*
 * Process the updated 'langremap' option value.
 */
    نویسه *
did_set_langremap(optset_T *args UNUSED)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    بازگشت پوچ;
}
#endif

/*
 * Process the new 'laststatus' option value.
 */
    نویسه *
did_set_laststatus(optset_T *args UNUSED)
{
    last_status(FALSE);	// (re)set last window status line
    بازگشت پوچ;
}

#اگر defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    نویسه *
did_set_linespace(optset_T *args UNUSED)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    اگر (gui.in_use && gui_mch_adjust_charheight() == OK)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    بازگشت پوچ;
}
#endif

/*
 * Process the updated 'lisp' option value.
 */
    نویسه *
did_set_lisp(optset_T *args UNUSED)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (تهی)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    بازگشت پوچ;
}

/*
 * Process the new 'maxcombine' option value.
 */
    نویسه *
did_set_maxcombine(optset_T *args UNUSED)
{
    اگر (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    وگرنه اگر (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    بازگشت پوچ;
}

/*
 * Process the updated 'modifiable' option value.
 */
    نویسه *
did_set_modifiable(optset_T *args UNUSED)
{
    // when 'modifiable' is changed, redraw the window title

#ifdef FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    اگر (curbuf->b_p_ma && (term_in_normal_mode(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_term != پوچ && !term_is_finished(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	بازگشت e_cannot_make_terminal_with_running_job_modifiable;
    }
#endif
    redraw_titles();

    بازگشت پوچ;
}

/*
 * Process the updated 'modified' option value.
 */
    نویسه *
did_set_modified(optset_T *args)
{
    اگر (!args->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_set = !!args->os_newval.boolean;
    بازگشت پوچ;
}

#اگر defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    نویسه *
did_set_mousehide(optset_T *args UNUSED)
{
    اگر (!p_mh)
	gui_mch_mousehide(FALSE);
    بازگشت پوچ;
}
#endif

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    نویسه *
did_set_عدد_relativenumber(optset_T *args UNUSED)
{
#اگر defined(FEAT_SIGNS) && defined(FEAT_GUI)
    اگر (gui.in_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != پوچ)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	اگر (!(curwin->w_p_nu && ((صحیح *)args->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#endif
    بازگشت پوچ;
}

#اگر defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    نویسه *
did_set_numberwidth(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    // 'numberwidth' must be positive
    اگر (curwin->w_p_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_nuw = 1;
    }
    وگرنه اگر (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_nuw = 20;
    }
    اگر (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    وگرنه اگر (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_سطر_شمار = 0; // trigger a redraw

    بازگشت errmsg;
}
#endif

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    نویسه *
did_set_osctimeoutlen(optset_T *args)
{
    اگر (p_ost < 0)
    {
	p_ost = args->os_oldval.عدد;
	بازگشت e_argument_must_be_positive;
    }

    بازگشت پوچ;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    نویسه *
did_set_paste(optset_T *args UNUSED)
{
    ایستا صحیح	old_p_paste = FALSE;
    ایستا صحیح	save_sm = 0;
    ایستا صحیح	save_sta = 0;
    ایستا صحیح	save_ru = 0;
#ifdef FEAT_RIGHTLEFT
    ایستا صحیح	save_ri = 0;
    ایستا صحیح	save_hkmap = 0;
#endif
    buf_T	*buf;

    اگر (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	اگر (!old_p_paste)
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
		اگر (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != empty_option
					   ? vim_strsave(buf->b_p_vsts) : پوچ;
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
	    اگر (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != empty_option
						  ? vim_strsave(p_vsts) : پوچ;
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
	    اگر (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = empty_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#endif
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	اگر (p_ru)
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
	اگر (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = empty_option;
#endif
    }

    // Paste switched from on to off: Restore saved values.
    وگرنه اگر (old_p_paste)
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
	    اگر (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : empty_option;
	    vim_free(buf->b_p_vsts_array);
	    اگر (buf->b_p_vsts && buf->b_p_vsts != empty_option)
		(تهی)tabstop_set(buf->b_p_vsts, &buf->b_p_vsts_array);
	    وگرنه
		buf->b_p_vsts_array = پوچ;
#endif
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	اگر (p_ru != save_ru)
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
	اگر (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : empty_option;
#endif
    }

    old_p_paste = p_paste;

#اگر defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#endif

    بازگشت پوچ;
}


#ifdef FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    نویسه *
did_set_previewwindow(optset_T *args UNUSED)
{
    اگر (!curwin->w_p_pvw)
	بازگشت پوچ;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	اگر (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    بازگشت e_preview_window_already_exists;
	}

    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    نویسه *
did_set_pyxversion(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_argument;

    بازگشت errmsg;
}
#endif

/*
 * Process the updated 'readonly' option value.
 */
    نویسه *
did_set_readonly(optset_T *args)
{
    // when 'readonly' is reset globally, also reset readonlymode
    اگر (!curbuf->b_p_ro && (args->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    اگر (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    بازگشت پوچ;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    نویسه *
did_set_scrollbind(optset_T *args UNUSED)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    اگر (!curwin->w_p_scb)
	بازگشت پوچ;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    بازگشت پوچ;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    نویسه *
did_set_maxsearchcount(optset_T *args UNUSED)
{
    نویسه	*errmsg = پوچ;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#define MAX_جستجو_شمار 9999

    اگر (p_msc <= 0)
	errmsg = e_argument_must_be_positive;
    وگرنه اگر (p_msc > MAX_جستجو_شمار)
	errmsg = e_invalid_argument;

    اگر (errmsg != پوچ)
	p_msc = 99;

    بازگشت errmsg;
#undef MAX_جستجو_شمار
}

#اگر defined(BACKSLASH_IN_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    نویسه *
did_set_shellslash(optset_T *args UNUSED)
{
    اگر (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    وگرنه
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
    بازگشت پوچ;
}
#endif

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    نویسه *
did_set_shiftwidth_tabstop(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    نویسه	*errmsg = پوچ;

    اگر (curbuf->b_p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_ts = 8;
    }
    وگرنه اگر (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_ts = 8;
    }
    اگر (p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ts = 8;
    }
    وگرنه اگر (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	p_ts = 8;
    }

    اگر (curbuf->b_p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_شمار(curbuf->b_p_vts_array) > 0
		       ? tabstop_first(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#وگرنه
	curbuf->b_p_sw = curbuf->b_p_ts;
#endif
    }
    اگر (p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_شمار(curbuf->b_p_vts_array) > 0
	     ? tabstop_first(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#وگرنه
	p_sw = curbuf->b_p_ts;
#endif
    }

#ifdef FEAT_FOLDING
    اگر (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#endif
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    اگر (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    بازگشت errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    نویسه *
did_set_showtabline(optset_T *args UNUSED)
{
    shell_new_rows();	// recompute window positions and heights
    بازگشت پوچ;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    نویسه *
did_set_smoothscroll(optset_T *args UNUSED)
{
    اگر (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    بازگشت پوچ;
}

#اگر defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    نویسه *
did_set_spell(optset_T *args UNUSED)
{
    اگر (curwin->w_p_spell)
	بازگشت parse_spelllang(curwin);

    بازگشت پوچ;
}
#endif

/*
 * Process the updated 'swapfile' option value.
 */
    نویسه *
did_set_swapfile(optset_T *args UNUSED)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    اگر (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    وگرنه
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    بازگشت پوچ;
}

#اگر defined(FEAT_TERMGUICOLORS)
    نویسه *
did_set_termguicolors(optset_T *args UNUSED)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    اگر (
#  ifdef VIMDLL
	    !gui.in_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_set = TRUE;
	p_tgc = 0;
	بازگشت e_24_bit_colors_are_not_supported_on_this_environment;
    }
    اگر (is_term_win32())
	swap_tcap();
# endif
# ifdef FEAT_GUI
    اگر (!gui.in_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    اگر (is_term_win32())
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

    بازگشت پوچ;
}
#endif

    نویسه *
did_set_termsync(optset_T *args UNUSED)
{
    اگر (!p_tsy)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);
    بازگشت پوچ;
}

#اگر defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    نویسه *
did_set_termwinscroll(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    نویسه	*errmsg = پوچ;

    اگر (*pp < 1)
    {
	errmsg = e_argument_must_be_positive;
	*pp = 1;
    }

    بازگشت errmsg;
}
#endif

/*
 * Process the updated 'terse' option value.
 */
    نویسه *
did_set_terse(optset_T *args UNUSED)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_جستجو);

    // insert 's' in p_shm
    اگر (p_terse && p == پوچ)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    وگرنه اگر (!p_terse && p != پوچ)
	STRMOVE(p, p + 1);
    بازگشت پوچ;
}

/*
 * Process the updated 'textauto' option value.
 */
    نویسه *
did_set_textauto(optset_T *args)
{
    // when 'textauto' is set or reset also change 'fileformats'
    set_string_option_direct((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | args->os_flags, 0);

    بازگشت پوچ;
}

/*
 * Process the updated 'textmode' option value.
 */
    نویسه *
did_set_textmode(optset_T *args)
{
    // when 'textmode' is set or reset also change 'fileformat'
    set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, args->os_flags);

    بازگشت پوچ;
}

/*
 * Process the new 'textwidth' option value.
 */
    نویسه *
did_set_textwidth(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (curbuf->b_p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_tw = 0;
    }
    اگر (p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tw = 0;
    }
#ifdef FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(پوچ, wp);
    }
#endif

    بازگشت errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    نویسه *
did_set_title_icon(optset_T *args UNUSED)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_set_title();
    بازگشت پوچ;
}

/*
 * Process the new 'titlelen' option value.
 */
    نویسه *
did_set_titlelen(optset_T *args)
{
    long old_مقدار = args->os_oldval.عدد;
    نویسه *errmsg = پوچ;

    // if 'titlelen' has changed, redraw the title
    اگر (p_titlelen < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_titlelen = 85;
    }
    اگر (starting != NO_SCREEN && old_مقدار != p_titlelen)
	need_maketitle = TRUE;

    بازگشت errmsg;
}

#اگر defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    نویسه *
did_set_undofile(optset_T *args)
{
    // Only take action when the option was set.
    اگر (!curbuf->b_p_udf && !p_udf)
	بازگشت پوچ;

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
	اگر ((curbuf == save_curbuf
		    || (args->os_flags & OPT_GLOBAL)
		    || args->os_flags == 0)
		&& !curbufIsChanged() && curbuf->b_ml.ml_mfp != پوچ)
	{
# ifdef FEAT_CRYPT
	    اگر (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
		ادامه;
# endif
	    u_compute_hash(hash);
	    u_read_undo(پوچ, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    بازگشت پوچ;
}
#endif

/*
 * Process the new global 'undolevels' option value.
 */
    ایستا تهی
update_global_undolevels(long مقدار, long old_مقدار)
{
    // sync undo before 'undolevels' changes

    // use the old value, otherwise u_sync() may not work properly
    p_ul = old_مقدار;
    u_sync(TRUE);
    p_ul = مقدار;
}

/*
 * Process the new buffer local 'undolevels' option value.
 */
    ایستا تهی
update_buflocal_undolevels(long مقدار, long old_مقدار)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_مقدار;
    u_sync(TRUE);
    curbuf->b_p_ul = مقدار;
}

/*
 * Process the new 'undolevels' option value.
 */
    نویسه *
did_set_undolevels(optset_T *args)
{
    long *pp = (long *)args->os_varp;

    اگر (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(args->os_newval.عدد,
						       args->os_oldval.عدد);
    وگرنه اگر (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(args->os_newval.عدد,
						       args->os_oldval.عدد);

    بازگشت پوچ;
}

/*
 * Process the new 'updatecount' option value.
 */
    نویسه *
did_set_updatecount(optset_T *args)
{
    long old_مقدار = args->os_oldval.عدد;
    نویسه *errmsg = پوچ;

    // when 'updatecount' changes from zero to non-zero, open swap files
    اگر (p_uc < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_uc = 100;
    }
    اگر (p_uc && !old_مقدار)
	ml_open_files();

    بازگشت errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    نویسه *
did_set_weirdinvert(optset_T *args)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    اگر (p_wiv && !args->os_oldval.boolean)
	T_XS = (char_u *)"y";
    وگرنه اگر (!p_wiv && args->os_oldval.boolean)
	T_XS = empty_option;
    p_wiv = (*T_XS != NUL);

    بازگشت پوچ;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    نویسه *
did_set_wildchar(optset_T *args)
{
    long c = *(long *)args->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    اگر (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

/*
 * Process the new 'window' option value.
 */
    نویسه *
did_set_window(optset_T *args UNUSED)
{
    اگر (p_window < 1)
	p_window = 1;
    وگرنه اگر (p_window >= Rows)
	p_window = Rows - 1;
    بازگشت پوچ;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    نویسه *
did_set_winheight_helpheight(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    نویسه	*errmsg = پوچ;

    اگر (p_wh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wh = 1;
    }
    اگر (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    اگر (p_hh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hh = 0;
    }

    // Change window height NOW
    اگر (!ONE_WINDOW)
    {
	اگر (pp == &p_wh && curwin->w_height < p_wh)
	    win_setheight((صحیح)p_wh);
	اگر (pp == &p_hh && curbuf->b_help && curwin->w_height < p_hh)
	    win_setheight((صحیح)p_hh);
    }

    بازگشت errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    نویسه *
did_set_winminheight(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (p_wmh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmh = 0;
    }
    اگر (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    بازگشت errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    نویسه *
did_set_winminwidth(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (p_wmw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmw = 0;
    }
    اگر (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    بازگشت errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    نویسه *
did_set_winwidth(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (p_wiw < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wiw = 1;
    }
    اگر (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    اگر (!ONE_WINDOW && curwin->w_width < p_wiw)
	win_setwidth((صحیح)p_wiw);

    بازگشت errmsg;
}

#اگر defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    نویسه *
did_set_wlsteal(optset_T *args UNUSED)
{
    clip_reset_wayland();

    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    نویسه *
did_set_wltimeoutlen(optset_T *args)
{
    اگر (p_wtm < 0)
    {
	p_wtm = args->os_oldval.عدد;
	بازگشت e_argument_must_be_positive;
    }

    بازگشت پوچ;
}
#endif

/*
 * Process the updated 'wrap' option value.
 */
    نویسه *
did_set_wrap(optset_T *args UNUSED)
{
    // Set w_leftcol or w_skipcol to zero.
    اگر (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    وگرنه
	curwin->w_skipcol = 0;

    بازگشت پوچ;
}

#ifdef FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    نویسه *
did_set_xhistory(optset_T *args)
{
    صحیح is_p_chi = (long*)args->os_varp == &p_chi;
    صحیح err;
    long *arg = (is_p_chi) ? &p_chi :(long*)args->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    اگر (*arg < 1)
    {
	*arg = args->os_oldval.عدد;
	بازگشت e_cannot_have_negative_or_zero_عدد_of_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    اگر (*arg > 100)
    {
	*arg = args->os_oldval.عدد;
	بازگشت e_cannot_have_more_than_hundred_quickfix;
    }

    اگر (is_p_chi)
	err = qf_resize_پشته(*arg);
    وگرنه
	err = ll_resize_پشته(curwin, *arg);

    اگر (err == FAIL)
	بازگشت e_failed_resizing_quickfix_پشته;

    بازگشت پوچ;
}
#endif

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ایستا نویسه *
set_bool_option(
    صحیح		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    صحیح		مقدار,			// new value
    صحیح		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    صحیح		old_مقدار = *(صحیح *)varp;
#اگر defined(FEAT_EVAL)
    صحیح		old_global_مقدار = 0;
#endif
    نویسه	*errmsg = پوچ;

    // Disallow changing some options from secure mode
    اگر ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	بازگشت e_not_allowed_here;

#اگر defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_مقدار = *(صحیح *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *(صحیح *)varp = مقدار;	    // set the new value
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(صحیح *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = مقدار;

    // Handle side effects of changing a bool option.
    اگر (options[opt_idx].opt_did_set_cb != پوچ)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.boolean = old_مقدار;
	args.os_newval.boolean = مقدار;
	args.os_errbuf = پوچ;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
	اگر (errmsg != پوچ)
	    بازگشت errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_SET;

#اگر defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(long)(old_مقدار ? TRUE : FALSE),
				(long)(old_global_مقدار ? TRUE : FALSE),
				(long)(مقدار ? TRUE : FALSE), پوچ);
#endif

    comp_col();			    // in case 'ruler' or 'showcmd' changed

    اگر (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    اگر ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    بازگشت errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    ایستا نویسه *
check_رقم_option_bounds(
    long	*pp,
    long	old_مقدار,
    long	old_Rows,
    long	old_Columns,
    نویسه	*errbuf,
    size_t	errbuflen,
    نویسه	*errmsg)
{
    اگر (Rows < min_rows_for_all_tabpages() && full_screen)
    {
	اگر (errbuf != پوچ)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_سطرها), min_rows_for_all_tabpages());
	    errmsg = errbuf;
	}
	Rows = min_rows_for_all_tabpages();
    }
    اگر (Columns < MIN_COLUMNS && full_screen)
    {
	اگر (errbuf != پوچ)
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
    اگر (old_Rows != Rows || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	اگر (updating_screen)
	    *pp = old_مقدار;
	وگرنه اگر (full_screen
#ifdef FEAT_GUI
		&& !gui.starting
#endif
		)
	    set_shellsize((صحیح)Columns, (صحیح)Rows, TRUE);
	وگرنه
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    اگر (cmdline_row > Rows - p_ch && Rows > p_ch)
		cmdline_row = Rows - p_ch;
	}
	اگر (p_window >= Rows || !option_was_set((char_u *)"window"))
	    p_window = Rows - 1;
    }

    اگر (p_tm < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tm = 0;
    }
    اگر ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_height
		    && curwin->w_height > 0))
	    && full_screen)
    {
	اگر (pp == &(curwin->w_p_scr))
	{
	    اگر (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_size;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	وگرنه اگر (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	وگرنه // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_height;
    }
    اگر (p_hi < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hi = 0;
    }
    وگرنه اگر (p_hi > 10000)
    {
	errmsg = e_invalid_argument;
	p_hi = 10000;
    }
    اگر (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_argument;
	p_re = 0;
    }
    اگر (p_report < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_report = 1;
    }
    اگر ((p_sj < -100 || p_sj >= Rows) && full_screen)
    {
	اگر (Rows != old_Rows)	// Rows changed, just adjust p_sj
	    p_sj = Rows / 2;
	وگرنه
	{
	    errmsg = e_invalid_scroll_size;
	    p_sj = 1;
	}
    }
    اگر (p_so < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_so = 0;
    }
    اگر (p_sop < 0 && full_screen)
    {
	errmsg = e_invalid_argument;
	p_sop = 0;
    }
    اگر (p_siso < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_siso = 0;
    }
    اگر (p_cwh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_cwh = 1;
    }
    اگر (p_ut < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ut = 2000;
    }
    اگر (p_ss < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ss = 0;
    }

    بازگشت errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ایستا نویسه *
set_رقم_option(
    صحیح		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    long	مقدار,			// new value
    نویسه	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    صحیح		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    نویسه	*errmsg = پوچ;
    long	old_مقدار = *(long *)varp;
#اگر defined(FEAT_EVAL)
    long	old_global_مقدار = 0;	// only used when setting a local and
					// global option
#endif
    long	old_Rows = Rows;	// remember old Rows
    long	old_Columns = Columns;	// remember old Columns
    long	*pp = (long *)varp;

    // Disallow changing some options from secure mode.
    اگر ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	بازگشت e_not_allowed_here;

#اگر defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_مقدار = *(long *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *pp = مقدار;
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = مقدار;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    اگر (options[opt_idx].opt_did_set_cb != پوچ)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.عدد = old_مقدار;
	args.os_newval.عدد = مقدار;
	args.os_errbuf = پوچ;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
    }

    // Check the bounds for numeric options here
    errmsg = check_رقم_option_bounds(pp, old_مقدار, old_Rows, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_SET;

#اگر defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_مقدار, old_global_مقدار,
								مقدار, errmsg);
#endif

    comp_col();			    // in case 'columns' or 'ls' changed

    اگر (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    اگر ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    بازگشت errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    تهی
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    صحیح		doclear = (flags & P_RCLR) == P_RCLR;
    صحیح		all = ((flags & P_RALL) == P_RALL || doclear);

    اگر ((flags & P_RSTAT) || all)	// mark all status lines dirty
	status_redraw_all();

    اگر ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	اگر (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	وگرنه
	    changed_window_setting();
    }
    اگر (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    اگر (doclear)
	redraw_all_later(UPD_CLEAR);
    وگرنه اگر (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    صحیح
findoption(char_u *arg)
{
    صحیح		    opt_idx;
    نویسه	    *s, *p;
    ایستا short    quick_tab[27] = {0, 0};	// quick access table
    صحیح		    is_term_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    اگر (quick_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	صحیح last_opt_idx;
	برای (last_opt_idx = 0; options[last_opt_idx].fullname != پوچ;
		last_opt_idx++)
	    ;
	برای (صحیح tab_idx = 1; tab_idx < 27; tab_idx++)
	    quick_tab[tab_idx] = last_opt_idx;

	p = options[0].fullname;
	برای (opt_idx = 1; opt_idx < last_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    اگر (s[0] != p[0])
	    {
		اگر (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		وگرنه
		    quick_tab[Char_Ord_کم(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    اگر (arg[0] < 'a' || arg[0] > 'z')
	بازگشت -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    اگر (is_term_opt)
	opt_idx = quick_tab[26];
    وگرنه
	opt_idx = quick_tab[Char_Ord_کم(arg[0])];
    برای (; (s = options[opt_idx].fullname) != پوچ && s[0] == arg[0]; opt_idx++)
    {
	اگر (STRCMP(arg, s) == 0)		    // match full name
	    بشکن;
    }
    اگر (s != پوچ && s[0] != arg[0])
	s = پوچ;
    اگر (s == پوچ && !is_term_opt)
    {
	opt_idx = quick_tab[Char_Ord_کم(arg[0])];
	برای ( ; options[opt_idx].fullname != پوچ; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    اگر (s != پوچ && STRCMP(arg, s) == 0)   // match short name
		بشکن;
	    s = پوچ;
	}
    }
    اگر (s == پوچ)
	opt_idx = -1;
    بازگشت opt_idx;
}

#اگر defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
get_option_مقدار(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    صحیح		*flagsp,
    صحیح		scope)
{
    صحیح		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    اگر (opt_idx < 0)		    // option not in the table
    {
	صحیح کلید;

	اگر (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (کلید = find_کلید_option(name, FALSE)) != 0)
	{
	    char_u کلید_name[2];
	    char_u *p;

	    اگر (flagsp != پوچ)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    اگر (کلید < 0)
	    {
		کلید_name[0] = KEY2TERMCAP0(کلید);
		کلید_name[1] = KEY2TERMCAP1(کلید);
	    }
	    وگرنه
	    {
		کلید_name[0] = KS_کلید;
		کلید_name[1] = (کلید & 0xff);
	    }
	    p = find_termcode(کلید_name);
	    اگر (p != پوچ)
	    {
		اگر (stringval != پوچ)
		    *stringval = vim_strsave(p);
		بازگشت gov_string;
	    }
	}
	بازگشت gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    اگر (flagsp != پوچ)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    اگر (options[opt_idx].flags & P_STRING)
    {
	اگر (varp == پوچ)		    // hidden option
	    بازگشت gov_hidden_string;
	اگر (stringval != پوچ)
	{
	    اگر ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    وگرنه اگر ((char_u **)varp == &curbuf->b_p_کلید
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    وگرنه
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	بازگشت gov_string;
    }

    اگر (varp == پوچ)		    // hidden option
	بازگشت (options[opt_idx].flags & P_رقم)
					 ? gov_hidden_عدد : gov_hidden_bool;
    اگر (options[opt_idx].flags & P_رقم)
	*numval = *(long *)varp;
    وگرنه
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	اگر ((صحیح *)varp == &curbuf->b_changed)
	    *numval = curbufIsChanged();
	وگرنه
	    *numval = (long) *(صحیح *)varp;
    }
    بازگشت (options[opt_idx].flags & P_رقم) ? gov_عدد : gov_bool;
}
#endif

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    صحیح
get_option_مقدار_strict(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    صحیح		opt_type,
    تهی	*from)
{
    صحیح		opt_idx;
    char_u	*varp = پوچ;
    ساختار vimoption *p;
    صحیح		r = 0;

    opt_idx = findoption(name);
    اگر (opt_idx < 0)
	بازگشت 0;

    p = &(options[opt_idx]);

    // Hidden option
    اگر (p->var == پوچ)
	بازگشت 0;

    اگر (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    وگرنه اگر (p->flags & P_رقم)
	r |= SOPT_رقم;
    وگرنه اگر (p->flags & P_STRING)
	r |= SOPT_STRING;

    اگر (p->indir == PV_NONE)
    {
	اگر (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	وگرنه
	    بازگشت 0; // Did not request global-only option
    }
    وگرنه
    {
	اگر (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	وگرنه اگر (opt_type == SREQ_GLOBAL)
	    بازگشت 0; // Requested global option

	اگر (p->indir & PV_WIN)
	{
	    اگر (opt_type == SREQ_BUF)
		بازگشت 0; // Did not request window-local option
	    وگرنه
		r |= SOPT_WIN;
	}
	وگرنه اگر (p->indir & PV_BUF)
	{
	    اگر (opt_type == SREQ_WIN)
		بازگشت 0; // Did not request buffer-local option
	    وگرنه
		r |= SOPT_BUF;
	}
    }

    اگر (stringval == پوچ)
	بازگشت r;

    اگر (opt_type == SREQ_GLOBAL)
	varp = p->var;
    وگرنه
    {
	اگر (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    اگر (p->indir == PV_MOD)
	    {
		*numval = bufIsChanged((buf_T *)from);
		varp = پوچ;
	    }
# ifdef FEAT_CRYPT
	    وگرنه اگر (p->indir == PV_کلید)
	    {
		// never return the value of the crypt key
		*stringval = پوچ;
		varp = پوچ;
	    }
# endif
	    وگرنه
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
	وگرنه اگر (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_buffer;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	}
	اگر (varp == p->var)
	    بازگشت (r | SOPT_UNSET);
    }

    اگر (varp != پوچ)
    {
	اگر (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	وگرنه اگر (p->flags & P_رقم)
	    *numval = *(long *) varp;
	وگرنه
	    *numval = *(صحیح *)varp;
    }

    بازگشت r;
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
option_iter_next(تهی **option, صحیح opt_type)
{
    ساختار vimoption	*ret = پوچ;
    انجامبده
    {
	اگر (*option == پوچ)
	    *option = (تهی *) options;
	وگرنه اگر (((ساختار vimoption *) (*option))->fullname == پوچ)
	{
	    *option = پوچ;
	    بازگشت پوچ;
	}
	وگرنه
	    *option = (تهی *) (((ساختار vimoption *) (*option)) + 1);

	ret = ((ساختار vimoption *) (*option));

	// Hidden option
	اگر (ret->var == پوچ)
	{
	    ret = پوچ;
	    ادامه;
	}

	انتخاب (opt_type)
	{
	    حالت SREQ_GLOBAL:
		اگر (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = پوچ;
		بشکن;
	    حالت SREQ_BUF:
		اگر (!(ret->indir & PV_BUF))
		    ret = پوچ;
		بشکن;
	    حالت SREQ_WIN:
		اگر (!(ret->indir & PV_WIN))
		    ret = پوچ;
		بشکن;
	    پیشفرض:
		internal_error("option_iter_next()");
		بازگشت پوچ;
	}
    }
    وقتی (ret == پوچ);

    بازگشت (char_u *)ret->fullname;
}
#endif

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(صحیح opt_idx)
{
    بازگشت options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    تهی
set_option_flag(صحیح opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    صحیح
is_global_option(صحیح opt_idx)
{
    بازگشت options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    صحیح
is_global_local_option(صحیح opt_idx)
{
    بازگشت options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    صحیح
is_window_local_option(صحیح opt_idx)
{
    بازگشت options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    صحیح
is_hidden_option(صحیح opt_idx)
{
    بازگشت options[opt_idx].var == پوچ;
}

#اگر defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    صحیح
is_crypt_کلید_option(صحیح opt_idx)
{
    بازگشت options[opt_idx].indir == PV_کلید;
}
#endif

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    نویسه *
set_option_مقدار(
    char_u	*name,
    long	عدد,
    char_u	*string,
    صحیح		opt_flags)	// OPT_LOCAL or 0 (both)
{
    صحیح		opt_idx;
    char_u	*varp;
    long_u	flags;
    ایستا نویسه	errbuf[ERR_BUFLEN];
    صحیح		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    اگر (opt_idx < 0)
    {
	صحیح کلید;

	اگر (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (کلید = find_کلید_option(name, FALSE)) != 0)
	{
	    char_u کلید_name[2];

	    اگر (کلید < 0)
	    {
		کلید_name[0] = KEY2TERMCAP0(کلید);
		کلید_name[1] = KEY2TERMCAP1(کلید);
	    }
	    وگرنه
	    {
		کلید_name[0] = KS_کلید;
		کلید_name[1] = (کلید & 0xff);
	    }
	    add_termcode(کلید_name, string, FALSE);
	    اگر (full_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    بازگشت پوچ;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    وگرنه
    {
	flags = options[opt_idx].flags;
#ifdef HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	اگر (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_in_sandbox));
	    بازگشت پوچ;
	}
#endif
	اگر (flags & P_STRING)
	    بازگشت set_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	اگر (varp != پوچ)	// hidden option is not changed
	{
	    اگر (عدد == 0 && string != پوچ)
	    {
		صحیح idx;

		// Either we are given a string or we are setting option
		// to zero.
		برای (idx = 0; string[idx] == '0'; ++idx)
		    ;
		اگر (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_عدد_required_after_str_equal_str),
								 name, string);
		    بازگشت پوچ;     // do nothing as we hit an error

		}
	    }
	    اگر (flags & P_رقم)
	    {
		بازگشت set_رقم_option(opt_idx, varp, عدد,
					   errbuf, اندازه(errbuf), opt_flags);
	    }
	    وگرنه
		بازگشت set_bool_option(opt_idx, varp, (صحیح)عدد, opt_flags);
	}

    }
    بازگشت پوچ;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    تهی
set_option_مقدار_give_err(
    char_u	*name,
    long	عدد,
    char_u	*string,
    صحیح		opt_flags)	// OPT_LOCAL or 0 (both)
{
    نویسه *errmsg = set_option_مقدار(name, عدد, string, opt_flags);

    اگر (errmsg != پوچ)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_term_code(char_u *tname)
{
    صحیح	    opt_idx;
    char_u  *varp;

    اگر (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	بازگشت پوچ;
    اگر ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	اگر (varp != پوچ)
	    varp = *(char_u **)(varp);
	بازگشت varp;
    }
    بازگشت find_termcode(tname + 2);
}

    char_u *
get_highlight_default(تهی)
{
    صحیح i;

    i = findoption((char_u *)"hl");
    اگر (i >= 0)
	بازگشت options[i].def_val[VI_DEFAULT];
    بازگشت (char_u *)پوچ;
}

    char_u *
get_encoding_default(تهی)
{
    صحیح i;

    i = findoption((char_u *)"enc");
    اگر (i >= 0)
	بازگشت options[i].def_val[VI_DEFAULT];
    بازگشت (char_u *)پوچ;
}

#اگر defined(FEAT_QUICKFIX)
    صحیح
is_option_allocated(نویسه *name)
{
    صحیح idx = findoption((char_u *)name);

    بازگشت idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#endif

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    ایستا صحیح
find_کلید_option(char_u *arg_arg, صحیح has_lt)
{
    صحیح		کلید = 0;
    صحیح		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    اگر (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	اگر (!has_lt || arg[4] == '>')
	    کلید = TERMCAP2_کلید(arg[2], arg[3]);
    }
    وگرنه اگر (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	کلید = find_special_کلید(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_کلید | FSK_SIMPLIFY, پوچ);
	اگر (modifiers)		    // can't handle modifiers here
	    کلید = 0;
    }
    بازگشت کلید;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    ایستا تهی
showoptions(
    صحیح		all,
    صحیح		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    ساختار vimoption	*p;
    صحیح			col;
    صحیح			isterm;
    char_u		*varp;
    ساختار vimoption	**اقلام;
    صحیح			item_شمار;
    صحیح			run;
    صحیح			row, rows;
    صحیح			cols;
    صحیح			i;
    صحیح			len;

#define INC 20
#define GAP 3

    اقلام = ALLOC_MULT(ساختار vimoption *, OPTION_شمار);
    اگر (اقلام == پوچ)
	بازگشت;

    // Highlight title
    اگر (all == 2)
	پیام_puts_title(_("\n--- Terminal codes ---"));
    وگرنه اگر (opt_flags & OPT_GLOBAL)
	پیام_puts_title(_("\n--- Global option values ---"));
    وگرنه اگر (opt_flags & OPT_LOCAL)
	پیام_puts_title(_("\n--- Local option values ---"));
    وگرنه
	پیام_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    برای (run = 1; run <= 2 && !got_int; ++run)
    {
	// collect the items in items[]
	item_شمار = 0;
	برای (p = &options[0]; p->fullname != پوچ; p++)
	{
	    // apply :filter /pat/
	    اگر (message_filtered((char_u *)p->fullname))
		ادامه;

	    varp = پوچ;
	    isterm = istermoption(p);
	    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		اگر (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    وگرنه
		varp = get_varp(p);
	    اگر (varp != پوچ
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		اگر (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		وگرنه اگر (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		وگرنه
		{
		    option_value2string(p, opt_flags);
		    len = (صحیح)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		اگر ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    اقلام[item_شمار++] = p;
	    }
	}

	// display the items
	اگر (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    اگر (cols == 0)
		cols = 1;
	    rows = (item_شمار + cols - 1) / cols;
	}
	وگرنه	// run == 2
	    rows = item_شمار;
	برای (row = 0; row < rows && !got_int; ++row)
	{
	    پیام_putchar('\n');			// go to next line
	    اگر (got_int)			// 'q' typed in more
		بشکن;
	    col = 0;
	    برای (i = row; i < item_شمار; i += rows)
	    {
		پیام_col = col;			// make columns
		showoneopt(اقلام[i], opt_flags);
		col += INC;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(اقلام);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    ایستا صحیح
optval_default(ساختار vimoption *p, char_u *varp, صحیح compatible)
{
    صحیح		dvi;

    اگر (varp == پوچ)
	بازگشت TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    اگر (p->flags & P_رقم)
	بازگشت (*(long *)varp == (long)(long_i)p->def_val[dvi]);
    اگر (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	بازگشت (*(صحیح *)varp == (صحیح)(long)(long_i)p->def_val[dvi]);
    // P_STRING
    بازگشت (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    ایستا تهی
showoneopt(
    ساختار vimoption	*p,
    صحیح			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    صحیح		save_silent = silent_mode;

    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    اگر ((p->flags & P_BOOL) && ((صحیح *)varp == &curbuf->b_changed
					? !curbufIsChanged() : !*(صحیح *)varp))
	پیام_puts("no");
    وگرنه اگر ((p->flags & P_BOOL) && *(صحیح *)varp < 0)
	پیام_puts("--");
    وگرنه
	پیام_puts("  ");
    پیام_puts(p->fullname);
    اگر (!(p->flags & P_BOOL))
    {
	پیام_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	پیام_outtrans(NameBuff);
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
    صحیح
makeset(FILE *fd, صحیح opt_flags, صحیح local_only)
{
    ساختار vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = پوچ;	// fresh value
    نویسه		*cmd;
    صحیح			round;
    صحیح			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    برای (pri = 1; pri >= 0; --pri)
    {
      برای (p = &options[0]; !istermoption(p); p++)
	اگر (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    اگر (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		ادامه;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    اگر ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		ادامه;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    اگر ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		ادامه;

	    اگر ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		ادامه;

	    round = 2;
	    اگر (p->indir != PV_NONE)
	    {
		اگر (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    اگر (!(opt_flags & OPT_LOCAL))
			ادامه;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    اگر (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			اگر (!optval_default(p, varp_fresh, p_cp))
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
	    برای ( ; round <= 2; varp = varp_local, ++round)
	    {
		اگر (round == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		وگرنه
		    cmd = "setlocal";

		اگر (p->flags & P_BOOL)
		{
		    اگر (put_setbool(fd, cmd, p->fullname, *(صحیح *)varp) == FAIL)
			بازگشت FAIL;
		}
		وگرنه اگر (p->flags & P_رقم)
		{
		    اگر (put_setnum(fd, cmd, p->fullname, (long *)varp) == FAIL)
			بازگشت FAIL;
		}
		وگرنه    // P_STRING
		{
		    صحیح		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    اگر ((p->flags & P_FUNC) && *(char_u **)varp != پوچ
			    && strstr(*(نویسه **)varp, "<lambda>") != پوچ)
			ادامه;

#ifdef FEAT_EVAL
		    legacy = !is_option_مقدار_vim9(p - &options[0],
			    round == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#وگرنه
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#endif

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    اگر (
#اگر defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#endif
			    p->indir == PV_FT)
		    {
			اگر (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    بازگشت FAIL;
			do_endif = TRUE;
		    }
		    اگر (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			بازگشت FAIL;
		    اگر (do_endif)
		    {
			اگر (put_سطر(fd, "endif") == FAIL)
			    بازگشت FAIL;
		    }
		}
	    }
	}
    }
    بازگشت OK;
}

#اگر defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    صحیح
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_version < SCRIPT_VERSION_VIM9;
# endif

    اگر (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	بازگشت FAIL;

    بازگشت OK;
}
#endif

    ایستا صحیح
put_setstring(
    FILE	*fd,
    bool	legacy,
    نویسه	*cmd,
    نویسه	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = پوچ;
    char_u	*part = پوچ;
    char_u	*p;

    اگر (legacy && fprintf(fd, "legacy ") < 0)
	بازگشت FAIL;
    اگر (fprintf(fd, "%s %s=", cmd, name) < 0)
	بازگشت FAIL;
    اگر (*valuep != پوچ)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	اگر (valuep == &p_pt)
	{
	    s = *valuep;
	    وقتی (*s != NUL)
		اگر (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    بازگشت FAIL;
	}
	// expand the option value, replace $HOME by ~
	وگرنه اگر ((flags & P_EXPAND) != 0)
	{
	    صحیح  size = (صحیح)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(size);
	    اگر (buf == پوچ)
		برو fail;
	    home_replace(پوچ, *valuep, buf, size, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    اگر (size >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != پوچ)
	    {
		part = alloc(size);
		اگر (part == پوچ)
		    برو fail;

		// write line break to clear the option, e.g. ':set rtp='
		اگر (put_eol(fd) == FAIL)
		    برو fail;

		p = buf;
		وقتی (*p != NUL)
		{
		    اگر (legacy && fprintf(fd, "legacy ") < 0)
			بازگشت FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    اگر (fprintf(fd, "%s %s+=", cmd, name) < 0)
			برو fail;
		    (تهی)copy_option_part(&p, part, size,  ",");
		    اگر (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			برو fail;
		}
		vim_free(buf);
		vim_free(part);
		بازگشت OK;
	    }
	    اگر (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		بازگشت FAIL;
	    }
	    vim_free(buf);
	}
	وگرنه اگر (put_escstr(fd, *valuep, 2) == FAIL)
	    بازگشت FAIL;
    }
    اگر (put_eol(fd) < 0)
	بازگشت FAIL;
    بازگشت OK;
fail:
    vim_free(buf);
    vim_free(part);
    بازگشت FAIL;
}

    ایستا صحیح
put_setnum(
    FILE	*fd,
    نویسه	*cmd,
    نویسه	*name,
    long	*valuep)
{
    long	wc;

    اگر (fprintf(fd, "%s %s=", cmd, name) < 0)
	بازگشت FAIL;
    اگر (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	اگر (fputs((نویسه *)get_special_کلید_name((صحیح)wc, 0), fd) < 0)
	    بازگشت FAIL;
    }
    وگرنه اگر (fprintf(fd, "%ld", *valuep) < 0)
	بازگشت FAIL;
    اگر (put_eol(fd) < 0)
	بازگشت FAIL;
    بازگشت OK;
}

    ایستا صحیح
put_setbool(
    FILE	*fd,
    نویسه	*cmd,
    نویسه	*name,
    صحیح		مقدار)
{
    اگر (مقدار < 0)	// global/local option using global value
	بازگشت OK;
    اگر (fprintf(fd, "%s %s%s", cmd, مقدار ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    تهی
clear_termoptions(تهی)
{
    // Reset a few things before clearing the old options. This may cause
    // outputting a few things that the terminal doesn't understand, but the
    // screen will be cleared later, so this is OK.
    mch_setmouse(FALSE);	    // switch mouse off
    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window titles
#اگر defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    اگر (gui.starting)
	clear_xterm_clip();
#endif
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    تهی
free_termoptions(تهی)
{
    ساختار vimoption   *p;

    برای (p = options; p->fullname != پوچ; p++)
	اگر (istermoption(p))
	{
	    اگر (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    اگر (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = empty_option;
	    p->def_val[VI_DEFAULT] = empty_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ifdef FEAT_EVAL
	    // remember where the option was cleared
	    set_option_sctx_idx((صحیح)(p - options), OPT_GLOBAL, current_sctx);
#endif
	}
    clear_termcodes();
}

/*
 * Free the string for one term option, if it was allocated.
 * Set the string to empty_option and clear allocated flag.
 * "var" points to the option value.
 */
    تهی
free_one_termoption(char_u *var)
{
    ساختار vimoption   *p;

    برای (p = &options[0]; p->fullname != پوچ; p++)
	اگر (p->var == var)
	{
	    اگر (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = empty_option;
	    p->flags &= ~P_ALLOCED;
	    بشکن;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    تهی
set_term_defaults(تهی)
{
    ساختار vimoption   *p;

    برای (p = &options[0]; p->fullname != پوچ; p++)
    {
	اگر (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    اگر (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    اگر (p->flags & P_ALLOCED)
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
    ایستا صحیح
istermoption(ساختار vimoption *p)
{
    بازگشت (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    ایستا صحیح
istermoption_idx(صحیح opt_idx)
{
    بازگشت istermoption(&options[opt_idx]);
}

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    تهی
unset_global_local_option(char_u *name, تهی *from)
{
    ساختار vimoption *p;
    صحیح		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    اگر (opt_idx < 0)
	بازگشت;
    p = &(options[opt_idx]);

    انتخاب ((صحیح)p->indir)
    {
	// global option with local value: use local value if it's been set
	حالت PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    بشکن;
	حالت PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    بشکن;
	حالت PV_PATH:
	    clear_string_option(&buf->b_p_path);
	    بشکن;
	حالت PV_AC:
	    buf->b_p_ac = -1;
	    بشکن;
	حالت PV_AR:
	    buf->b_p_ar = -1;
	    بشکن;
# ifdef HAVE_FSYNC
	حالت PV_FS:
	    buf->b_p_fs = -1;
	    بشکن;
# endif
	حالت PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    بشکن;
	حالت PV_TAGS:
	    clear_string_option(&buf->b_p_tags);
	    بشکن;
	حالت PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    بشکن;
	حالت PV_SISO:
	    curwin->w_p_siso = -1;
	    بشکن;
	حالت PV_SO:
	    curwin->w_p_so = -1;
	    بشکن;
	حالت PV_SOP:
	    curwin->w_p_sop = -1;
	    بشکن;
# ifdef FEAT_FIND_ID
	حالت PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    بشکن;
	حالت PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    بشکن;
# endif
	حالت PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    بشکن;
	حالت PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    بشکن;
# ifdef FEAT_DIFF
	حالت PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    بشکن;
# endif
	حالت PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    بشکن;
# ifdef FEAT_COMPL_FUNC
	حالت PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    بشکن;
# endif
	حالت PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    بشکن;
# ifdef FEAT_EVAL
	حالت PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    بشکن;
# endif
# ifdef FEAT_QUICKFIX
	حالت PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    بشکن;
	حالت PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    بشکن;
	حالت PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    بشکن;
	حالت PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    بشکن;
# endif
# اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	حالت PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    بشکن;
# endif
# اگر defined(FEAT_CRYPT)
	حالت PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    بشکن;
# endif
# ifdef FEAT_LINEBREAK
	حالت PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    بشکن;
# endif
# ifdef FEAT_STL_OPT
	حالت PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    بشکن;
	حالت PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    بشکن;
# endif
	حالت PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    بشکن;
	حالت PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    بشکن;
	حالت PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    بشکن;
	حالت PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    set_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      پوچ, 0);
	    redraw_later(UPD_NOT_VALID);
	    بشکن;
	حالت PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    set_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      پوچ, 0);
	    redraw_later(UPD_NOT_VALID);
	    بشکن;
	حالت PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    بشکن;
    }
}
#endif

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    ایستا char_u *
get_varp_scope(ساختار vimoption *p, صحیح scope)
{
    اگر ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	اگر (p->var == VAR_WIN)
	    بازگشت (char_u *)GLOBAL_WO(get_varp(p));
	بازگشت p->var;
    }
    اگر ((scope & OPT_LOCAL) && ((صحیح)p->indir & PV_BOTH))
    {
	انتخاب ((صحیح)p->indir)
	{
	    حالت PV_FP:   بازگشت (char_u *)&(curbuf->b_p_fp);
#ifdef HAVE_FSYNC
	    حالت PV_FS:	بازگشت (char_u *)&(curbuf->b_p_fs);
#endif
#ifdef FEAT_EVAL
	    حالت PV_FFU: بازگشت (char_u *)&(curbuf->b_p_ffu);
#endif
#ifdef FEAT_QUICKFIX
	    حالت PV_EFM:  بازگشت (char_u *)&(curbuf->b_p_efm);
	    حالت PV_GEFM: بازگشت (char_u *)&(curbuf->b_p_gefm);
	    حالت PV_GP:   بازگشت (char_u *)&(curbuf->b_p_gp);
	    حالت PV_MP:   بازگشت (char_u *)&(curbuf->b_p_mp);
#endif
	    حالت PV_EP:   بازگشت (char_u *)&(curbuf->b_p_ep);
	    حالت PV_KP:   بازگشت (char_u *)&(curbuf->b_p_kp);
	    حالت PV_PATH: بازگشت (char_u *)&(curbuf->b_p_path);
	    حالت PV_AC:   بازگشت (char_u *)&(curbuf->b_p_ac);
	    حالت PV_AR:   بازگشت (char_u *)&(curbuf->b_p_ar);
	    حالت PV_TAGS: بازگشت (char_u *)&(curbuf->b_p_tags);
	    حالت PV_TC:   بازگشت (char_u *)&(curbuf->b_p_tc);
	    حالت PV_SISO: بازگشت (char_u *)&(curwin->w_p_siso);
	    حالت PV_SO:   بازگشت (char_u *)&(curwin->w_p_so);
	    حالت PV_SOP:  بازگشت (char_u *)&(curwin->w_p_sop);
#ifdef FEAT_FIND_ID
	    حالت PV_DEF:  بازگشت (char_u *)&(curbuf->b_p_def);
	    حالت PV_INC:  بازگشت (char_u *)&(curbuf->b_p_inc);
#endif
	    حالت PV_COT:  بازگشت (char_u *)&(curbuf->b_p_cot);
	    حالت PV_DICT: بازگشت (char_u *)&(curbuf->b_p_dict);
#ifdef FEAT_DIFF
	    حالت PV_DIA:  بازگشت (char_u *)&(curbuf->b_p_dia);
#endif
	    حالت PV_TSR:  بازگشت (char_u *)&(curbuf->b_p_tsr);
#ifdef FEAT_COMPL_FUNC
	    حالت PV_TSRFU: بازگشت (char_u *)&(curbuf->b_p_tsrfu);
#endif
#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    حالت PV_BEXPR: بازگشت (char_u *)&(curbuf->b_p_bexpr);
#endif
#اگر defined(FEAT_CRYPT)
	    حالت PV_CM:	  بازگشت (char_u *)&(curbuf->b_p_cm);
#endif
#ifdef FEAT_LINEBREAK
	    حالت PV_SBR:  بازگشت (char_u *)&(curwin->w_p_sbr);
#endif
#ifdef FEAT_STL_OPT
	    حالت PV_STL:  بازگشت (char_u *)&(curwin->w_p_stl);
	    حالت PV_STLO: بازگشت (char_u *)&(curwin->w_p_stlo);
#endif
	    حالت PV_UL:   بازگشت (char_u *)&(curbuf->b_p_ul);
	    حالت PV_LW:   بازگشت (char_u *)&(curbuf->b_p_lw);
	    حالت PV_BKC:  بازگشت (char_u *)&(curbuf->b_p_bkc);
	    حالت PV_MENC: بازگشت (char_u *)&(curbuf->b_p_menc);
	    حالت PV_LCS:  بازگشت (char_u *)&(curwin->w_p_lcs);
	    حالت PV_FCS:  بازگشت (char_u *)&(curwin->w_p_fcs);
	    حالت PV_VE:	  بازگشت (char_u *)&(curwin->w_p_ve);

	}
	بازگشت پوچ; // "cannot happen"
    }
    بازگشت get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(صحیح opt_idx, صحیح scope)
{
    بازگشت get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    ایستا char_u *
get_varp(ساختار vimoption *p)
{
    // hidden option, always return NULL
    اگر (p->var == پوچ)
	بازگشت پوچ;

    انتخاب ((صحیح)p->indir)
    {
	حالت PV_NONE:	بازگشت p->var;

	// global option with local value: use local value if it's been set
	حالت PV_EP:	بازگشت *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	حالت PV_KP:	بازگشت *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	حالت PV_PATH:	بازگشت *curbuf->b_p_path != NUL
				    ? (char_u *)&(curbuf->b_p_path) : p->var;
	حالت PV_AC:	بازگشت curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	حالت PV_AR:	بازگشت curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	حالت PV_TAGS:	بازگشت *curbuf->b_p_tags != NUL
				    ? (char_u *)&(curbuf->b_p_tags) : p->var;
	حالت PV_TC:	بازگشت *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	حالت PV_BKC:	بازگشت *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	حالت PV_SISO:	بازگشت curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	حالت PV_SO:	بازگشت curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	حالت PV_SOP:	بازگشت curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ifdef FEAT_FIND_ID
	حالت PV_DEF:	بازگشت *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	حالت PV_INC:	بازگشت *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#endif
	حالت PV_COT:	بازگشت *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	حالت PV_DICT:	بازگشت *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ifdef FEAT_DIFF
	حالت PV_DIA:	بازگشت *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#endif
	حالت PV_TSR:	بازگشت *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ifdef FEAT_COMPL_FUNC
	حالت PV_TSRFU:	بازگشت *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#endif
	حالت PV_FP:	بازگشت *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ifdef HAVE_FSYNC
	حالت PV_FS:	بازگشت curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#endif
#ifdef FEAT_EVAL
	حالت PV_FFU:	بازگشت *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#endif
#ifdef FEAT_QUICKFIX
	حالت PV_EFM:	بازگشت *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	حالت PV_GEFM:	بازگشت *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	حالت PV_GP:	بازگشت *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	حالت PV_MP:	بازگشت *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#endif
#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	حالت PV_BEXPR:	بازگشت *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#endif
#اگر defined(FEAT_CRYPT)
	حالت PV_CM:	بازگشت *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#endif
#ifdef FEAT_LINEBREAK
	حالت PV_SBR:	بازگشت *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#endif
#ifdef FEAT_STL_OPT
	حالت PV_STL:	بازگشت *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	حالت PV_STLO:	بازگشت *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#endif
	حالت PV_UL:	بازگشت curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	حالت PV_LW:	بازگشت *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	حالت PV_MENC:	بازگشت *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ifdef FEAT_ARABIC
	حالت PV_ARAB:	بازگشت (char_u *)&(curwin->w_p_arab);
#endif
	حالت PV_LIST:	بازگشت (char_u *)&(curwin->w_p_list);
	حالت PV_LCS:	بازگشت *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	حالت PV_FCS:	بازگشت *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	حالت PV_VE:	بازگشت *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ifdef FEAT_SPELL
	حالت PV_SPELL:	بازگشت (char_u *)&(curwin->w_p_spell);
#endif
#ifdef FEAT_SYN_HL
	حالت PV_CUC:	بازگشت (char_u *)&(curwin->w_p_cuc);
	حالت PV_CUL:	بازگشت (char_u *)&(curwin->w_p_cul);
	حالت PV_CULOPT:	بازگشت (char_u *)&(curwin->w_p_culopt);
	حالت PV_CC:	بازگشت (char_u *)&(curwin->w_p_cc);
#endif
#ifdef FEAT_DIFF
	حالت PV_DIFF:	بازگشت (char_u *)&(curwin->w_p_diff);
#endif
	حالت PV_EIW:	بازگشت (char_u *)&(curwin->w_p_eiw);
#ifdef FEAT_FOLDING
	حالت PV_FDC:	بازگشت (char_u *)&(curwin->w_p_fdc);
	حالت PV_FEN:	بازگشت (char_u *)&(curwin->w_p_fen);
	حالت PV_FDI:	بازگشت (char_u *)&(curwin->w_p_fdi);
	حالت PV_FDL:	بازگشت (char_u *)&(curwin->w_p_fdl);
	حالت PV_FDM:	بازگشت (char_u *)&(curwin->w_p_fdm);
	حالت PV_FML:	بازگشت (char_u *)&(curwin->w_p_fml);
	حالت PV_FDN:	بازگشت (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	حالت PV_FDE:	بازگشت (char_u *)&(curwin->w_p_fde);
	حالت PV_FDT:	بازگشت (char_u *)&(curwin->w_p_fdt);
# endif
	حالت PV_FMR:	بازگشت (char_u *)&(curwin->w_p_fmr);
#endif
	حالت PV_NU:	بازگشت (char_u *)&(curwin->w_p_nu);
	حالت PV_RNU:	بازگشت (char_u *)&(curwin->w_p_rnu);
#ifdef FEAT_LINEBREAK
	حالت PV_NUW:	بازگشت (char_u *)&(curwin->w_p_nuw);
#endif
	حالت PV_WFB:	بازگشت (char_u *)&(curwin->w_p_wfb);
	حالت PV_WFH:	بازگشت (char_u *)&(curwin->w_p_wfh);
	حالت PV_WFW:	بازگشت (char_u *)&(curwin->w_p_wfw);
#اگر defined(FEAT_QUICKFIX)
	حالت PV_PVW:	بازگشت (char_u *)&(curwin->w_p_pvw);
	حالت PV_LHI:	بازگشت (char_u *)&(curwin->w_p_lhi);
#endif
#ifdef FEAT_RIGHTLEFT
	حالت PV_RL:	بازگشت (char_u *)&(curwin->w_p_rl);
	حالت PV_RLC:	بازگشت (char_u *)&(curwin->w_p_rlc);
#endif
	حالت PV_SCROLL:	بازگشت (char_u *)&(curwin->w_p_scr);
	حالت PV_SMS:	بازگشت (char_u *)&(curwin->w_p_sms);
	حالت PV_WRAP:	بازگشت (char_u *)&(curwin->w_p_wrap);
#ifdef FEAT_LINEBREAK
	حالت PV_LBR:	بازگشت (char_u *)&(curwin->w_p_lbr);
	حالت PV_BRI:	بازگشت (char_u *)&(curwin->w_p_bri);
	حالت PV_BRIOPT: بازگشت (char_u *)&(curwin->w_p_briopt);
#endif
	حالت PV_WCR:	بازگشت (char_u *)&(curwin->w_p_wcr);
	حالت PV_SCBIND: بازگشت (char_u *)&(curwin->w_p_scb);
	حالت PV_CRBIND: بازگشت (char_u *)&(curwin->w_p_crb);
#ifdef FEAT_CONCEAL
	حالت PV_COCU:   بازگشت (char_u *)&(curwin->w_p_cocu);
	حالت PV_COLE:   بازگشت (char_u *)&(curwin->w_p_cole);
#endif
	حالت PV_WHL:	بازگشت (char_u *)&(curwin->w_p_whl);

#ifdef FEAT_TERMINAL
	حالت PV_TWK:    بازگشت (char_u *)&(curwin->w_p_twk);
	حالت PV_TWS:    بازگشت (char_u *)&(curwin->w_p_tws);
	حالت PV_TWSL:	بازگشت (char_u *)&(curbuf->b_p_twsl);
#endif

	حالت PV_AI:	بازگشت (char_u *)&(curbuf->b_p_ai);
	حالت PV_BIN:	بازگشت (char_u *)&(curbuf->b_p_bin);
	حالت PV_BOMB:	بازگشت (char_u *)&(curbuf->b_p_bomb);
	حالت PV_BH:	بازگشت (char_u *)&(curbuf->b_p_bh);
	حالت PV_BT:	بازگشت (char_u *)&(curbuf->b_p_bt);
	حالت PV_BL:	بازگشت (char_u *)&(curbuf->b_p_bl);
	حالت PV_CI:	بازگشت (char_u *)&(curbuf->b_p_ci);
	حالت PV_CIN:	بازگشت (char_u *)&(curbuf->b_p_cin);
	حالت PV_CINK:	بازگشت (char_u *)&(curbuf->b_p_cink);
	حالت PV_CINO:	بازگشت (char_u *)&(curbuf->b_p_cino);
	حالت PV_CINSD:	بازگشت (char_u *)&(curbuf->b_p_cinsd);
	حالت PV_CINW:	بازگشت (char_u *)&(curbuf->b_p_cinw);
	حالت PV_COM:	بازگشت (char_u *)&(curbuf->b_p_com);
	حالت PV_CMS:	بازگشت (char_u *)&(curbuf->b_p_cms);
	حالت PV_CPT:	بازگشت (char_u *)&(curbuf->b_p_cpt);
#ifdef BACKSLASH_IN_FILENAME
	حالت PV_CSL:	بازگشت (char_u *)&(curbuf->b_p_csl);
#endif
#ifdef FEAT_COMPL_FUNC
	حالت PV_CFU:	بازگشت (char_u *)&(curbuf->b_p_cfu);
	حالت PV_OFU:	بازگشت (char_u *)&(curbuf->b_p_ofu);
#endif
#ifdef FEAT_EVAL
	حالت PV_TFU:	بازگشت (char_u *)&(curbuf->b_p_tfu);
#endif
	حالت PV_EOF:	بازگشت (char_u *)&(curbuf->b_p_eof);
	حالت PV_EOL:	بازگشت (char_u *)&(curbuf->b_p_eol);
	حالت PV_FIXEOL:	بازگشت (char_u *)&(curbuf->b_p_fixeol);
	حالت PV_ET:	بازگشت (char_u *)&(curbuf->b_p_et);
	حالت PV_FENC:	بازگشت (char_u *)&(curbuf->b_p_fenc);
	حالت PV_FF:	بازگشت (char_u *)&(curbuf->b_p_ff);
	حالت PV_FT:	بازگشت (char_u *)&(curbuf->b_p_ft);
	حالت PV_FO:	بازگشت (char_u *)&(curbuf->b_p_fo);
	حالت PV_FLP:	بازگشت (char_u *)&(curbuf->b_p_flp);
	حالت PV_IMI:	بازگشت (char_u *)&(curbuf->b_p_iminsert);
	حالت PV_IMS:	بازگشت (char_u *)&(curbuf->b_p_imsearch);
	حالت PV_INF:	بازگشت (char_u *)&(curbuf->b_p_inf);
	حالت PV_ISK:	بازگشت (char_u *)&(curbuf->b_p_isk);
#ifdef FEAT_FIND_ID
# ifdef FEAT_EVAL
	حالت PV_INEX:	بازگشت (char_u *)&(curbuf->b_p_inex);
# endif
#endif
#اگر defined(FEAT_EVAL)
	حالت PV_INDE:	بازگشت (char_u *)&(curbuf->b_p_inde);
	حالت PV_INDK:	بازگشت (char_u *)&(curbuf->b_p_indk);
#endif
#ifdef FEAT_EVAL
	حالت PV_FEX:	بازگشت (char_u *)&(curbuf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
	حالت PV_کلید:	بازگشت (char_u *)&(curbuf->b_p_کلید);
#endif
	حالت PV_LISP:	بازگشت (char_u *)&(curbuf->b_p_lisp);
	حالت PV_LOP:	بازگشت (char_u *)&(curbuf->b_p_lop);
	حالت PV_ML:	بازگشت (char_u *)&(curbuf->b_p_ml);
	حالت PV_MPS:	بازگشت (char_u *)&(curbuf->b_p_mps);
	حالت PV_MA:	بازگشت (char_u *)&(curbuf->b_p_ma);
	حالت PV_MOD:	بازگشت (char_u *)&(curbuf->b_changed);
	حالت PV_NF:	بازگشت (char_u *)&(curbuf->b_p_nf);
	حالت PV_PI:	بازگشت (char_u *)&(curbuf->b_p_pi);
	حالت PV_QE:	بازگشت (char_u *)&(curbuf->b_p_qe);
	حالت PV_RO:	بازگشت (char_u *)&(curbuf->b_p_ro);
	حالت PV_SI:	بازگشت (char_u *)&(curbuf->b_p_si);
	حالت PV_SN:	بازگشت (char_u *)&(curbuf->b_p_sn);
	حالت PV_STS:	بازگشت (char_u *)&(curbuf->b_p_sts);
	حالت PV_SUA:	بازگشت (char_u *)&(curbuf->b_p_sua);
	حالت PV_SWF:	بازگشت (char_u *)&(curbuf->b_p_swf);
#ifdef FEAT_SYN_HL
	حالت PV_SMC:	بازگشت (char_u *)&(curbuf->b_p_smc);
	حالت PV_SYN:	بازگشت (char_u *)&(curbuf->b_p_syn);
#endif
#ifdef FEAT_SPELL
	حالت PV_SPC:	بازگشت (char_u *)&(curwin->w_s->b_p_spc);
	حالت PV_SPF:	بازگشت (char_u *)&(curwin->w_s->b_p_spf);
	حالت PV_SPL:	بازگشت (char_u *)&(curwin->w_s->b_p_spl);
	حالت PV_SPO:	بازگشت (char_u *)&(curwin->w_s->b_p_spo);
#endif
	حالت PV_SW:	بازگشت (char_u *)&(curbuf->b_p_sw);
	حالت PV_TS:	بازگشت (char_u *)&(curbuf->b_p_ts);
	حالت PV_TW:	بازگشت (char_u *)&(curbuf->b_p_tw);
	حالت PV_TX:	بازگشت (char_u *)&(curbuf->b_p_tx);
#ifdef FEAT_PERSISTENT_UNDO
	حالت PV_UDF:	بازگشت (char_u *)&(curbuf->b_p_udf);
#endif
	حالت PV_WM:	بازگشت (char_u *)&(curbuf->b_p_wm);
#ifdef FEAT_KEYMAP
	حالت PV_KMAP:	بازگشت (char_u *)&(curbuf->b_p_keymap);
#endif
#ifdef FEAT_SIGNS
	حالت PV_SCL:	بازگشت (char_u *)&(curwin->w_p_scl);
#endif
#ifdef FEAT_VARTABS
	حالت PV_VSTS:	بازگشت (char_u *)&(curbuf->b_p_vsts);
	حالت PV_VTS:	بازگشت (char_u *)&(curbuf->b_p_vts);
#endif
	پیشفرض:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    بازگشت (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(صحیح opt_idx)
{
    بازگشت options[opt_idx].var;
}

#اگر defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(صحیح opt_idx)
{
    بازگشت (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    صحیح
option_info(char_u *name, dict_T *d)
{
    صحیح			opt_idx;
    ساختار vimoption	*p;
    صحیح			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    اگر ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    اگر (opt_idx < 0)
	بازگشت FAIL;

    p = &options[opt_idx];
    dict_add_string(d, "name", (char_u *)p->fullname);
    dict_add_string(d, "shortname",
		    (char_u *)(p->shortname == پوچ ? "" : p->shortname));
    dict_add_bool(d, "available", p->var != پوچ);
    dict_add_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_رقم) ? "number" : "string"));
    dict_add_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    اگر (p->flags & P_BOOL)
	dict_add_bool(d, "default", (صحیح)(long_i)p->def_val[dvi]);
    وگرنه اگر (p->flags & P_رقم)
	dict_add_عدد(d, "default", (long)(long_i)p->def_val[dvi]);
    وگرنه
	dict_add_string(d, "default", p->def_val[dvi] == پوچ
					? (char_u *)"" : p->def_val[dvi]);
    بازگشت OK;
}
#endif

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_set_cb_T
get_option_did_set_cb(صحیح opt_idx)
{
    بازگشت options[opt_idx].opt_did_set_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(تهی)
{
    اگر (*curbuf->b_p_ep == NUL)
	بازگشت p_ep;
    بازگشت curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(تهی)
{
#ifdef FEAT_EVAL
    اگر (*curbuf->b_p_ffu == NUL)
	بازگشت p_ffu;
    بازگشت curbuf->b_p_ffu;
#وگرنه
    بازگشت (char_u *)"";
#endif
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    تهی
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
    تهی
after_copy_winopt(win_T *wp)
{
    نویسه *errmsg = update_winhighlight(wp, wp->w_p_whl);

    اگر (errmsg != پوچ)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    اگر (wp->w_p_wrap)
	wp->w_leftcol = 0;
    وگرنه
	wp->w_skipcol = 0;
#ifdef FEAT_LINEBREAK
    briopt_check(پوچ, wp);
#endif
#ifdef FEAT_SYN_HL
    fill_culopt_flags(پوچ, wp);
    check_colorcolumn(پوچ, wp);
#endif
    set_listchars_option(wp, wp->w_p_lcs, TRUE, پوچ, 0);
    set_fillchars_option(wp, wp->w_p_fcs, TRUE, پوچ, 0);
}

    ایستا char_u *
copy_option_val(char_u *val)
{
    اگر (val == empty_option)
	بازگشت empty_option;  // no need to allocate memory
    بازگشت vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    تهی
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
						    اندازه(to->wo_script_ctx));
#endif
    check_winopt(to);		// don't want NULL pointers
}

/*
 * Check string options in a window for a NULL value.
 */
    ایستا تهی
check_win_options(win_T *win)
{
    check_winopt(&win->w_onebuf_opt);
    check_winopt(&win->w_allbuf_opt);
}

/*
 * Check for NULL pointers in a winopt_T and replace them with empty_option.
 */
    ایستا تهی
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
    تهی
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
ایستا صحیح buf_opt_idx[BV_شمار];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    ایستا تهی
init_buf_opt_idx(تهی)
{
    ایستا صحیح did_init_buf_opt_idx = FALSE;
    صحیح i;

    اگر (did_init_buf_opt_idx)
	بازگشت;
    did_init_buf_opt_idx = TRUE;
    برای (i = 0; !istermoption_idx(i); i++)
	اگر (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_MASK] = i;
}
#وگرنه
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
    تهی
buf_copy_options(buf_T *buf, صحیح flags)
{
    صحیح		should_copy = TRUE;
    char_u	*save_p_isk = پوچ;	    // init for GCC
    صحیح		dont_do_help;
    صحیح		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    اگر (p_cpo != پوچ)
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
	اگر ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == پوچ || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != پوچ)))
	    should_copy = FALSE;

	اگر (should_copy || (flags & BCO_ALWAYS))
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
	    اگر (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = پوچ;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    اگر (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		انتخاب (*p_ffs)
		{
		    حالت 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); بشکن;
		    حالت 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); بشکن;
		    حالت 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); بشکن;
		    پیشفرض:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		اگر (buf->b_p_ff != پوچ)
		    buf->b_start_ffc = *buf->b_p_ff;
		buf->b_p_bh = empty_option;
		buf->b_p_bt = empty_option;
	    }
	    وگرنه
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
	    اگر (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    وگرنه
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
	    اگر (p_vsts && p_vsts != empty_option)
		(تهی)tabstop_set(p_vsts, &buf->b_p_vsts_array);
	    وگرنه
		buf->b_p_vsts_array = پوچ;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : پوچ;
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
	    (تهی)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#endif
#اگر defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#endif
	    buf->b_p_fp = empty_option;
#اگر defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#endif
#ifdef FEAT_CRYPT
	    buf->b_p_کلید = vim_strsave(p_کلید);
	    COPY_OPT_SCTX(buf, BV_کلید);
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
#اگر defined(FEAT_EVAL)
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
#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = empty_option;
#endif
#اگر defined(FEAT_CRYPT)
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
	    اگر (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ifdef FEAT_VARTABS
		اگر (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (تهی)tabstop_set(p_vts, &buf->b_p_vts_array);
		وگرنه
		    buf->b_p_vts_array = پوچ;
#endif
	    }
	    وگرنه
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#ifdef FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		اگر (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (تهی)tabstop_set(p_vts, &buf->b_p_vts_array);
		وگرنه
		    buf->b_p_vts_array = پوچ;
#endif
		buf->b_help = false;
		اگر (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	اگر (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    اگر (did_isk)
	(تهی)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    تهی
reset_modifiable(تهی)
{
    صحیح		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    اگر (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}

/*
 * Set the global value for 'iminsert' to the local value.
 */
    تهی
set_iminsert_global(تهی)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    تهی
set_imsearch_global(تهی)
{
    p_imsearch = curbuf->b_p_imsearch;
}

ایستا صحیح expand_option_idx = -1;
ایستا صحیح expand_option_start_col = 0;
ایستا char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
ایستا صحیح expand_option_flags = 0;
ایستا صحیح expand_option_append = FALSE;

    تهی
set_context_in_set_cmd(
    expand_T	*xp,
    char_u	*arg,
    صحیح		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    صحیح		nextchar;
    long_u	flags = 0;	// init for GCC
    صحیح		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    صحیح		is_term_option = FALSE;
    صحیح		کلید;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    اگر (*arg == NUL)
    {
	xp->xp_pattern = arg;
	بازگشت;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    اگر (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_pattern = p + 1;
	بازگشت;
    }
    وقتی (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	اگر (*p == ' ' || *p == ',')
	{
	    وقتی (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	اگر (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    بشکن;
	}
	--p;
    }
    اگر (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_NO;
	p += 2;
    }
    وگرنه اگر (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_INV;
	p += 3;
    }
    xp->xp_pattern = arg = p;
    اگر (*arg == '<')
    {
	وقتی (*p != '>')
	    اگر (*p++ == NUL)	    // expand terminal option name
		بازگشت;
	کلید = get_special_کلید_code(arg + 1);
	اگر (کلید == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    بازگشت;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(کلید);
	expand_option_name[3] = KEY2TERMCAP1(کلید);
    }
    وگرنه
    {
	اگر (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    اگر (*p != NUL)
		++p;
	    اگر (*p == NUL)
		بازگشت;		// expand option name
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	وگرنه
	{
	    // Allow * wildcard
	    وقتی (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    اگر (*p == NUL)
		بازگشت;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    اگر (opt_idx == -1 || options[opt_idx].var == پوچ)
	    {
		xp->xp_context = EXPAND_NOTHING;
		بازگشت;
	    }
	    flags = options[opt_idx].flags;
	    اگر (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		بازگشت;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    صحیح expand_option_subtract = FALSE;
    اگر ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	اگر (nextchar == '-')
	    expand_option_subtract = TRUE;
	اگر (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    اگر ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	بازگشت;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    اگر (is_term_option)
	expand_option_idx = -1;
    وگرنه
	expand_option_idx = opt_idx;

    اگر (!is_term_option)
    {
	اگر (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    بازگشت;
	}
    }

    xp->xp_pattern = p + 1;
    expand_option_start_col = (صحیح)(p + 1 - xp->xp_سطر);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ifdef FEAT_SYN_HL
    اگر (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	بازگشت;
    }
#endif
    اگر (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	بازگشت;
    }
#ifdef FEAT_KEYMAP
    اگر (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	بازگشت;
    }
#endif

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    اگر (expand_option_subtract)
    {
	xp->xp_context = EXPAND_SETTING_SUBTRACT;
	بازگشت;
    }
    وگرنه اگر (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != پوچ)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    وگرنه اگر (*xp->xp_pattern == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	بازگشت;
    }
    وگرنه
	xp->xp_context = EXPAND_NOTHING;

    اگر (is_term_option || (flags & P_رقم))
	بازگشت;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    اگر (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	اگر (p == (char_u *)&p_bdir
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
	    اگر (p == (char_u *)&p_path || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    وگرنه
		xp->xp_backslash = XP_BS_ONE;
	}
	وگرنه
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    اگر (p == (char_u *)&p_tags)
		xp->xp_backslash = XP_BS_THREE;
	    وگرنه
		xp->xp_backslash = XP_BS_ONE;
	}
	اگر (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    اگر ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	برای (p = argend - 1; p >= xp->xp_pattern; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    اگر (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		وقتی (s > xp->xp_pattern && *(s - 1) == '\\')
		    --s;
		اگر ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#اگر defined(BACKSLASH_IN_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#وگرنه
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#endif
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_pattern = p + 1;
		    بشکن;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    اگر (flags & P_FLAGLIST)
	xp->xp_pattern = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ifdef FEAT_SPELL
    اگر (options[opt_idx].var == (char_u *)&p_sps)
    {
	اگر (STRNCMP(xp->xp_pattern, "file:", 5) == 0)
	{
	    xp->xp_pattern += 5;
	    بازگشت;
	}
	وگرنه اگر (options[expand_option_idx].opt_expand_cb != پوچ)
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
    ایستا صحیح
match_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	صحیح		idx,
	صحیح		test_only,
	صحیح		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    اگر (!fuzzy)
    {
	اگر (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    اگر (!test_only)
		matches[idx] = vim_strsave(str);
	    بازگشت TRUE;
	}
    }
    وگرنه
    {
	صحیح score;

	score = fuzzy_match_str(str, fuzzystr);
	اگر (score != FUZZY_SCORE_NONE)
	{
	    اگر (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    بازگشت TRUE;
	}
    }

    بازگشت FALSE;
}

    صحیح
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    صحیح		*رقم_Matches,
    char_u	***matches,
    صحیح		can_fuzzy)
{
    صحیح		رقم_normal = 0;	    // Nr of matching non-term-code settings
    صحیح		رقم_term = 0;	    // Nr of matching terminal code settings
    صحیح		opt_idx;
    صحیح		match;
    صحیح		شمار = 0;
    char_u	*str;
    صحیح		loop;
    صحیح		is_term_opt;
    char_u	name_buf[MAX_کلید_NAME_LEN];
    ایستا نویسه *(names[]) = {"all", "termcap"};
    صحیح		ic = regmatch->rm_ic;	// remember the ignore-case flag
    صحیح		fuzzy;
    fuzmatch_str_T  *fuzmatch = پوچ;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    برای (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	اگر (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    برای (match = 0; match < (صحیح)ARRAY_LENGTH(names); ++match)
	    {
		اگر (match_str((char_u *)names[match], regmatch, *matches,
			    شمار, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    اگر (loop == 0)
			رقم_normal++;
		    وگرنه
			شمار++;
		}
	    }
	}
	برای (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != پوچ;
								    opt_idx++)
	{
	    اگر (options[opt_idx].var == پوچ)
		ادامه;
	    اگر (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		ادامه;
	    is_term_opt = istermoption_idx(opt_idx);
	    اگر (is_term_opt && رقم_normal > 0)
		ادامه;

	    اگر (match_str(str, regmatch, *matches, شمار, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		اگر (loop == 0)
		{
		    اگر (is_term_opt)
			رقم_term++;
		    وگرنه
			رقم_normal++;
		}
		وگرنه
		    شمار++;
	    }
	    وگرنه اگر (!fuzzy && options[opt_idx].shortname != پوچ
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		اگر (loop == 0)
		{
		    اگر (is_term_opt)
			رقم_term++;
		    وگرنه
			رقم_normal++;
		}
		وگرنه
		    (*matches)[شمار++] = vim_strsave(str);
	    }
	    وگرنه اگر (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		اگر (match_str(name_buf, regmatch, *matches, شمار, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    اگر (loop == 0)
			رقم_term++;
		    وگرنه
			شمار++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	اگر (xp->xp_context != EXPAND_BOOL_SETTINGS  && رقم_normal == 0)
	{
	    برای (opt_idx = 0; (str = get_termcode(opt_idx)) != پوچ; opt_idx++)
	    {
		اگر (!SAFE_isprint(str[0]) || !SAFE_isprint(str[1]))
		    ادامه;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		اگر (match_str(name_buf, regmatch, *matches, شمار,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    اگر (loop == 0)
			رقم_term++;
		    وگرنه
			شمار++;
		}
		وگرنه
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    اگر (match_str(name_buf, regmatch, *matches, شمار,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			اگر (loop == 0)
			    رقم_term++;
			وگرنه
			    شمار++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    برای (opt_idx = 0; (str = get_کلید_name(opt_idx)) != پوچ; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		اگر (match_str(name_buf, regmatch, *matches, شمار, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    اگر (loop == 0)
			رقم_term++;
		    وگرنه
			شمار++;
		}
	    }
	}
	اگر (loop == 0)
	{
	    اگر (رقم_normal > 0)
		*رقم_Matches = رقم_normal;
	    وگرنه اگر (رقم_term > 0)
		*رقم_Matches = رقم_term;
	    وگرنه
		بازگشت OK;
	    اگر (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *رقم_Matches);
		اگر (*matches == پوچ)
		{
		    *matches = (char_u **)"";
		    بازگشت FAIL;
		}
	    }
	    وگرنه
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *رقم_Matches);
		اگر (fuzmatch == پوچ)
		{
		    *matches = (char_u **)"";
		    بازگشت FAIL;
		}
	    }
	}
    }

    اگر (fuzzy &&
	fuzzymatches_to_strmatches(fuzmatch, matches, شمار, FALSE) == FAIL)
	بازگشت FAIL;

    بازگشت OK;
}

// Escape an option value that can be used on the command-line with :set.
// Caller needs to free the returned string, unless NULL is returned.
    ایستا char_u*
escape_option_str_cmdline(char_u *var)
{
    char_u *buf;

    // A backslash is required before some characters.  This is the reverse of
    // what happens in do_set().
    buf = vim_strsave_escaped(var, escape_chars);
    اگر (buf == پوچ)
	بازگشت پوچ;

#ifdef BACKSLASH_IN_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    برای (var = buf; *var != NUL; MB_PTR_ADV(var))
	اگر (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#endif
    بازگشت buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    صحیح
ExpandOldSetting(صحیح *رقم_Matches, char_u ***matches)
{
    char_u  *var = پوچ;	// init for GCC
    char_u  *buf;

    *رقم_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    اگر (*matches == پوچ)
	بازگشت FAIL;

    // For a terminal key code expand_option_idx is < 0.
    اگر (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	اگر (var == پوچ)
	    expand_option_idx = findoption(expand_option_name);
    }

    اگر (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    وگرنه اگر (var == پوچ)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    اگر (buf == پوچ)
    {
	VIM_CLEAR(*matches);
	بازگشت FAIL;
    }

    (*matches)[0] = buf;
    *رقم_Matches = 1;
    بازگشت OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    صحیح
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    صحیح		*رقم_Matches,
    char_u	***matches)
{
    char_u  *var = پوچ;	// init for GCC
    char_u  *buf;

    اگر (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == پوچ)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	بازگشت FAIL;
    }

    optexpand_T args;
    args.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    args.oe_append = expand_option_append;
    args.oe_regmatch = regmatch;
    args.oe_xp = xp;
    args.oe_set_arg = xp->xp_سطر + expand_option_start_col;
    args.oe_include_orig_val =
	!expand_option_append &&
	(*args.oe_set_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    اگر (buf == پوچ)
	بازگشت FAIL;

    args.oe_opt_مقدار = buf;

    صحیح رقم_ret = options[expand_option_idx].opt_expand_cb(&args, رقم_Matches, matches);

    vim_free(buf);
    بازگشت رقم_ret;
}

/*
 * Expansion handler for :set-=
 */
    صحیح
ExpandSettingSubtract(
    expand_T	*xp,
    regmatch_T	*regmatch,
    صحیح		*رقم_Matches,
    char_u	***matches)
{
    اگر (expand_option_idx < 0)
	// term option
	بازگشت ExpandOldSetting(رقم_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    اگر (option_flags & P_رقم)
	بازگشت ExpandOldSetting(رقم_Matches, matches);
    وگرنه اگر (option_flags & P_COMMA)
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

	اگر (*option_val == NUL)
	    بازگشت FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	اگر (option_copy == پوچ)
	    بازگشت FAIL;
	next_val = option_copy;

	ga_init2(&ga, اندازه(char_u *), 10);

	انجامبده
	{
	    item = next_val;
	    comma = vim_strchr(next_val, ',');
	    وقتی (comma != پوچ && comma != next_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    اگر (comma != پوچ)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		next_val = comma + 1;
	    }
	    وگرنه
		next_val = پوچ;

	    اگر (*item == NUL)
		// empty value, don't add to list
		ادامه;

	    اگر (!vim_regexec(regmatch, item, (colnr_T)0))
		ادامه;

	    char_u *buf = escape_option_str_cmdline(item);
	    اگر (buf == پوچ)
	    {
		vim_free(option_copy);
		ga_clear_strings(&ga);
		بازگشت FAIL;
	    }
	    اگر (ga_add_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		بشکن;
	    }
	} وقتی (next_val != پوچ);

	vim_free(option_copy);

	*matches = ga.ga_data;
	*رقم_Matches = ga.ga_len;
	بازگشت OK;
    }
    وگرنه اگر (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	اگر (*xp->xp_pattern != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    بازگشت FAIL;
	}

	size_t رقم_flags = STRLEN(option_val);
	اگر (رقم_flags == 0)
	    بازگشت FAIL;

	*matches = ALLOC_MULT(char_u *, رقم_flags + 1);
	اگر (*matches == پوچ)
	    بازگشت FAIL;

	صحیح شمار = 0;
	char_u *p;

	p = vim_strnsave(option_val, رقم_flags);
	اگر (p == پوچ)
	{
	    VIM_CLEAR(*matches);
	    بازگشت FAIL;
	}
	(*matches)[شمار++] = p;

	اگر (رقم_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    برای (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		اگر (p == پوچ)
		    بشکن;
		(*matches)[شمار++] = p;
	    }
	}

	*رقم_Matches = شمار;
	بازگشت OK;
    }

    بازگشت ExpandOldSetting(رقم_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    ایستا تهی
option_value2string(
    ساختار vimoption	*opp,
    صحیح			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    اگر (opp->flags & P_رقم)
    {
	long wc = 0;

	اگر (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_کلید_name((صحیح)wc, 0));
	وگرنه اگر (wc != 0)
	    STRCPY(NameBuff, transchar((صحیح)wc));
	وگرنه
	    sprintf((نویسه *)NameBuff, "%ld", *(long *)varp);
    }
    وگرنه    // P_STRING
    {
	varp = *(char_u **)(varp);
	اگر (varp == پوچ)		    // just in case
	    NameBuff[0] = NUL;
#ifdef FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	وگرنه اگر (opp->var == (char_u *)&p_کلید && *varp)
	    STRCPY(NameBuff, "*****");
#endif
	وگرنه اگر (opp->flags & P_EXPAND)
	    home_replace(پوچ, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	وگرنه اگر ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	وگرنه
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    ایستا صحیح
wc_use_keyname(char_u *varp, long *wcp)
{
    اگر (((long *)varp == &p_wc) || ((long *)varp == &p_wcm))
    {
	*wcp = *(long *)varp;
	اگر (IS_SPECIAL(*wcp) || find_special_کلید_in_table((صحیح)*wcp) >= 0)
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    صحیح
shortmess(صحیح x)
{
    بازگشت p_shm != پوچ &&
	    (   vim_strchr(p_shm, x) != پوچ
	    || (vim_strchr(p_shm, 'a') != پوچ
		&& vim_strchr((char_u *)SHM_A, x) != پوچ));
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 * When "fname" is not NULL, use it to set $"envname" when it wasn't set yet.
 */
    تهی
vimrc_found(char_u *fname, char_u *envname)
{
    صحیح		opt_idx;
    صحیح		dofree = FALSE;
    char_u	*p;

    اگر (!option_was_set((char_u *)"cp"))
    {
	p_cp = FALSE;
	برای (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    اگر (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    اگر (fname != پوچ)
    {
	p = vim_getenv(envname, &dofree);
	اگر (p == پوچ)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = FullName_save(fname, FALSE);
	    اگر (p != پوچ)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	وگرنه اگر (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    تهی
change_compatible(صحیح on)
{
    صحیح	    opt_idx;

    اگر (p_cp != on)
    {
	p_cp = on;
	compatible_set();
    }
    opt_idx = findoption((char_u *)"cp");
    اگر (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_SET;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    صحیح
option_was_set(char_u *name)
{
    صحیح idx;

    idx = findoption(name);
    اگر (idx < 0)	// unknown option
	بازگشت FALSE;
    اگر (options[idx].flags & P_WAS_SET)
	بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    صحیح
reset_option_was_set(char_u *name)
{
    صحیح idx = findoption(name);

    اگر (idx < 0)
	بازگشت FAIL;

    options[idx].flags &= ~P_WAS_SET;
    بازگشت OK;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    ایستا تهی
compatible_set(تهی)
{
    صحیح	    opt_idx;

    برای (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	اگر (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    set_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    صحیح
can_bs(
    صحیح		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ifdef FEAT_JOB_CHANNEL
    اگر (what == BS_START && bt_prompt(curbuf))
	بازگشت FALSE;
#endif
    انتخاب (*p_bs)
    {
	حالت '3':       بازگشت TRUE;
	حالت '2':	بازگشت (what != BS_NOSTOP);
	حالت '1':	بازگشت (what != BS_START);
	حالت '0':	بازگشت FALSE;
    }
    بازگشت vim_strchr(p_bs, what) != پوچ;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloff_مقدار(تهی)
{
    بازگشت curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloffpad_مقدار(تهی)
{
    بازگشت curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_sidescrolloff_مقدار(تهی)
{
    بازگشت curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    unsigned صحیح
get_bkc_flags(buf_T *buf)
{
    بازگشت buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#اگر defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_مقدار(buf_T *buf)
{
    اگر (buf->b_p_flp == پوچ || *buf->b_p_flp == NUL)
	بازگشت p_flp;
    بازگشت buf->b_p_flp;
}
#endif

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    unsigned صحیح
get_ve_flags(تهی)
{
    بازگشت (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#اگر defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_مقدار(win_T *win)
{
    اگر (win->w_p_sbr == پوچ || *win->w_p_sbr == NUL)
	بازگشت p_sbr;
    اگر (STRCMP(win->w_p_sbr, "NONE") == 0)
	بازگشت empty_option;
    بازگشت win->w_p_sbr;
}
#endif

#اگر defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(صحیح bufopt)
{
    dict_T	*d;
    صحیح		opt_idx;

    d = dict_alloc();
    اگر (d == پوچ)
	بازگشت پوچ;

    برای (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	ساختار vimoption *opt = &options[opt_idx];

	اگر ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    اگر (varp != پوچ)
	    {
		اگر (opt->flags & P_STRING)
		    dict_add_string(d, opt->fullname, *(char_u **)varp);
		وگرنه اگر (opt->flags & P_رقم)
		    dict_add_عدد(d, opt->fullname, *(long *)varp);
		وگرنه
		    dict_add_عدد(d, opt->fullname, *(صحیح *)varp);
	    }
	}
    }

    بازگشت d;
}
#endif

#اگر defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    صحیح
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    اگر (val == پوچ)
	p = wp->w_p_culopt;
    وگرنه
	p = val;
    وقتی (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	اگر (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_سطر;
	}
	وگرنه اگر (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_سطر | CULOPT_NBR;
	}
	وگرنه اگر (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	وگرنه اگر (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	اگر (*p != ',' && *p != NUL)
	    بازگشت FAIL;
	اگر (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    اگر ((culopt_flags_new & CULOPT_سطر) && (culopt_flags_new & CULOPT_SCRLINE))
	بازگشت FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    بازگشت OK;
}
#endif

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    صحیح
magic_isset(تهی)
{
    انتخاب (magic_overruled)
    {
	حالت OPTION_MAGIC_ON:      بازگشت TRUE;
	حالت OPTION_MAGIC_OFF:     بازگشت FALSE;
	حالت OPTION_MAGIC_NOT_SET: بشکن;
    }
#ifdef FEAT_EVAL
    اگر (in_vim9script())
	بازگشت TRUE;
#endif
    بازگشت p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    صحیح
option_set_callback_func(char_u *optval UNUSED, callback_T *optcb UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    صحیح		funcname = FALSE;

    اگر (optval == پوچ || *optval == NUL)
    {
	free_callback(optcb);
	بازگشت OK;
    }

    اگر (*optval == '{' || (in_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_عبارت(optval, پوچ);
    وگرنه
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    اگر (tv == پوچ)
	بازگشت FAIL;

    cb = get_callback(tv);
    اگر (cb.cb_name == پوچ || *cb.cb_name == NUL)
    {
	free_tv(tv);
	بازگشت FAIL;
    }

    free_callback(optcb);
    set_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = پوچ;
    اگر (in_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != پوچ))
    {
	اگر (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = find_imported(optval, dot - optval, FALSE);
	اگر (import != پوچ && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((نویسه *)fnamebuf, اندازه(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	وگرنه
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

    بازگشت OK;
#وگرنه
    بازگشت FAIL;
#endif
}

#اگر defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    نویسه *
did_set_showtabpanel(optset_T *args UNUSED)
{
    shell_new_columns();
    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_EVAL)
    ایستا تهی
didset_options_sctx(صحیح opt_flags, نویسه **buf)
{
	برای (صحیح i = 0; ; ++i)
	{
	    اگر (buf[i] == پوچ)
		بشکن;

	    صحیح idx = findoption((char_u *)buf[i]);
	    اگر (idx >= 0)
		set_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#endif
