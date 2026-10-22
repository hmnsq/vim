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

#تعریف_کن درون_OPTION_C
#ضمن "vim.h"
#ضمن "optiondefs.h"

ساكن فراغ مجموعه_options_default(صحيح opt_flags);
ساكن فراغ مجموعه_string_default_esc(حرف *name, char_u *val, صحيح escape);
ساكن char_u *پیدا_کن_dup_قلم(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
ساكن char_u *option_expand(صحيح opt_idx, char_u *val);
ساكن فراغ didset_options(فراغ);
ساكن فراغ didset_options2(فراغ);
#إذا defined(FEAT_EVAL)
ساكن long_u *insecure_flag(win_T *wp, صحيح opt_idx, صحيح opt_flags);
#وإلا
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#پایان_اگر
ساكن حرف *مجموعه_bool_option(صحيح opt_idx, char_u *varp, صحيح مقدار, صحيح opt_flags);
ساكن حرف *مجموعه_رقم_option(صحيح opt_idx, char_u *varp, دراز مقدار, حرف *errbuf, size_t errbuflen, صحيح opt_flags);
ساكن صحيح پیدا_کن_کلید_option(char_u *arg_arg, صحيح has_lt);
ساكن فراغ showoptions(صحيح all, صحيح opt_flags);
ساكن صحيح optval_default(بنية vimoption *, char_u *varp, صحيح compatible);
ساكن فراغ showoneopt(بنية vimoption *, صحيح opt_flags);
ساكن صحيح put_setstring(FILE *fd, bool legacy, حرف *cmd, حرف *name, char_u **valuep, long_u flags);
ساكن صحيح put_setnum(FILE *fd, حرف *cmd, حرف *name, دراز *valuep);
ساكن صحيح put_setbool(FILE *fd, حرف *cmd, حرف *name, صحيح مقدار);
ساكن صحيح istermoption(بنية vimoption *p);
ساكن صحيح istermoption_idx(صحيح opt_idx);
ساكن char_u *get_varp_scope(بنية vimoption *p, صحيح scope);
ساكن char_u *get_varp(بنية vimoption *);
ساكن فراغ check_win_options(win_T *win);
ساكن فراغ option_value2string(بنية vimoption *, صحيح scope);
ساكن فراغ check_winopt(winopt_T *wop);
ساكن صحيح wc_use_keyname(char_u *varp, دراز *wcp);
ساكن فراغ compatible_مجموعه(فراغ);

#إذا defined(FEAT_EVAL)
ساكن حرف *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", عدم};
ساكن حرف *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    عدم};
ساكن فراغ didset_options_sctx(صحيح opt_flags, حرف **buf);
#پایان_اگر


/*
 * Initialize the 'shell' option to a default value.
 */
    ساكن فراغ
مجموعه_init_default_shell(فراغ)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    إذا (((p = mch_getenv((char_u *)"SHELL")) != عدم && *p != NUL)
#إذا defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != عدم && *p != NUL)
	    || ((p = (char_u *)default_shell()) != عدم && *p != NUL)
#پایان_اگر
	    )
#إذا defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	إذا (vim_strchr(p, ' ') != عدم)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    إذا (cmd != عدم)
	    {
		vim_snprintf((حرف *)cmd, len, "\"%s\"", p);
		مجموعه_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	وإلا
	    مجموعه_string_default("sh", p);
    }
#وإلا
	مجموعه_string_default_esc("sh", p, TRUE);
#پایان_اگر
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    ساكن فراغ
مجموعه_init_default_backupskip(فراغ)
{
    صحيح		opt_idx;
    صحيح		i;
    char_u	*p;
    صحيح		plen;
#اگر_تعریف_شده UNIX
    ساكن حرف	*(نامها[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#وإلا
    ساكن حرف	*(نامها[3]) = {"TMPDIR", "TEMP", "TMP"};
#پایان_اگر
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    لكل (i = 0; i < (صحيح)ARRAY_درازی(نامها); ++i)
    {
	صحيح		mustfree = FALSE;
#اگر_تعریف_شده UNIX
	إذا (*نامها[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (صحيح)STRLEN_LITERAL("/private/tmp");
# وإلا
	    p = (char_u *)"/tmp";
	    plen = (صحيح)STRLEN_LITERAL("/tmp");
# endif
	}
	وإلا
#پایان_اگر
	{
	    p = vim_getenv((char_u *)نامها[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	إذا (p != عدم && *p != NUL)
	{
	    char_u  *قلم;
	    size_t  itemsize;
	    صحيح	    has_trailing_مسیر_sep = FALSE;

	    إذا (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (صحيح)STRLEN(p);
		// does the value include a trailing path separator?
		إذا (after_pathsep(p, p + plen))
		    has_trailing_مسیر_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_مسیر_sep ? 0 : 1) + 2;
	    قلم = alloc(itemsize);
	    إذا (قلم != عدم)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((حرف *)قلم, itemsize, "%s%s*", p, (has_trailing_مسیر_sep) ? "" : PATHSEPSTR);

		إذا (پیدا_کن_dup_قلم(ga.ga_داده, قلم, itemlen, options[opt_idx].flags) == عدم
			&& ga_grow(&ga, (صحيح)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((حرف *)ga.ga_داده + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", قلم);
		}
		vim_free(قلم);
	    }
	}
	إذا (mustfree)
	    vim_free(p);
    }
    إذا (ga.ga_داده != عدم)
    {
	مجموعه_string_default("bsk", ga.ga_داده);
	vim_free(ga.ga_داده);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    ساكن فراغ
مجموعه_init_default_maxmemtot(فراغ)
{
    صحيح		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    إذا (opt_idx < 0)
	أرجع;

#إذا !defined(HAVE_AVAIL_MEM) && !defined(HAVE_مجموع_MEM)
    إذا (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#پایان_اگر
    {
#إذا defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#وإلاإذا defined(HAVE_مجموع_MEM)
	// Use amount of memory available to Vim.
	n = (mch_مجموع_mem(FALSE) >> 1);
#وإلا
	n = (0x7fffffff >> 11);
#پایان_اگر
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	إذا (opt_idx >= 0)
	{
#إذا !defined(HAVE_AVAIL_MEM) && !defined(HAVE_مجموع_MEM)
	    إذا ((دراز)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (دراز)n
		    || (دراز)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#پایان_اگر
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    ساكن فراغ
مجموعه_init_default_cdpath(فراغ)
{
    صحيح		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    صحيح	i;
    صحيح	j;
    صحيح	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    إذا (cdpath == عدم)
	أرجع;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    إذا (buf != عدم)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	لكل (i = 0; cdpath[i] != NUL; ++i)
	{
	    إذا (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    وإلا
	    {
		إذا (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	إذا (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	وإلا
	    vim_free(buf); // cannot happen
    }
    إذا (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    ساكن فراغ
مجموعه_init_default_printencoding(فراغ)
{
#إذا defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    مجموعه_string_default("penc",
# إذا defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# وإلا // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#پایان_اگر
}

#إذا defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    ساكن فراغ
مجموعه_init_default_printexpr(فراغ)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    مجموعه_string_default("pexpr",
# إذا defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# وإلا
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#پایان_اگر

#اگر_تعریف_شده UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    ساكن فراغ
مجموعه_init_restricted_نما(فراغ)
{
    char_u	*p;

    p = get_isolated_shell_name();
    إذا (p == عدم)
	أرجع;
    إذا (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#پایان_اگر

#اگر_تعریف_شده CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    ساكن فراغ
مجموعه_init_clean_rtp(فراغ)
{
    صحيح		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    إذا (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    إذا (opt_idx < 0)
	أرجع;
    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
    p_pp = (char_u *)CLEAN_RUNTIMEPATH;
}
#پایان_اگر

#اگر_تعریف_شده UNIX
/*
 * Change 'runtimepath' and 'packdir' to '$XDG_CONFIG_HOME/vim' if the only
 * vimrc found is located in '$XDG_CONFIG_HOME/vim/vimrc'.
 * In case the '$XDG_CONFIG_HOME' variable is not set, '$HOME/.config' is used
 * as a fallback as is defined in the XDG base dir specification:
 * <https://specifications.freedesktop.org/basedir/latest/>
 */
    ساكن فراغ
مجموعه_init_xdg_rtp(فراغ)
{
    صحيح		opt_idx;
    صحيح		has_xdg_env = TRUE;
    صحيح		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = عدم;
    char_u	*vimrc2 = عدم;
    char_u	*xdg_dir = عدم;
    char_u	*xdg_rtp = عدم;
    string_T	vimrc_xdg = {عدم, 0};

    // initialize chartab, so we can expand $HOME
    (فراغ)init_chartab();
    vimrc1 = expand_env_save((char_u *)USR_VIMRC_FILE);
    vimrc2 = expand_env_save((char_u *)USR_VIMRC_FILE2);

    xdg_dir = mch_getenv("XDG_CONFIG_HOME");
    إذا (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    إذا (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	اذهب theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    إذا ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	اذهب theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    إذا ((opt_idx = findoption((char_u *)"packpath")) < 0)
	اذهب theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# إذا defined(XDG_VDIR) && defined(FEAT_SESSION)
    إذا ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	اذهب theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    إذا (should_free_xdg_dir)
	vim_free(xdg_dir);
}
#پایان_اگر

/*
 * Expand environment variables and things like "~" for the defaults.
 * If option_expand() returns non-NULL the variable is expanded.  This can
 * only happen for non-indirect options.
 * Also set the default to the expanded value, so ":set" does not list
 * them.
 * Don't set the P_ALLOCED flag, because we don't want to free the
 * default.
 */
    ساكن فراغ
مجموعه_init_expand_env(فراغ)
{
    صحيح		opt_idx;
    char_u	*p;

    لكل (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	إذا ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != عدم)
	    p = (char_u *)_(*(حرف **)options[opt_idx].var);
	وإلا
	    p = option_expand(opt_idx, عدم);
	إذا (p != عدم && (p = vim_strsave(p)) != عدم)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    إذا (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#إذا defined(MSWIN) && defined(FEAT_GETTEXT)
/*
 * Get the display language of Windows and the languages to fall back on, as a
 * colon separated list for gettext, e.g. "ja_JP:en_US".  The list stops after
 * English, untranslated messages are English already.
 * Returns NULL when it cannot be obtained.  The result must be freed.
 */
    ساكن char_u *
get_ui_langs(فراغ)
{
    ULONG	رقم_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*میانگیر;
    char_u	*langs = عدم;

    إذا (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &رقم_languages, عدم,
								    &bufsize)
	    || bufsize == 0)
	أرجع عدم;

    میانگیر = ALLOC_MULT(WCHAR, bufsize);
    إذا (میانگیر == عدم)
	أرجع عدم;

    إذا (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &رقم_languages, میانگیر,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	إذا (langs != عدم)
	{
	    char_u	*d = langs;
	    WCHAR	*s = میانگیر;

	    طالما (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		إذا (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		لكل ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		إذا (english)
		    اكسر;
	    }
	    *d = NUL;
	}
    }
    vim_free(میانگیر);

    أرجع langs;
}
#پایان_اگر

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    ساكن فراغ
مجموعه_init_lang_env(فراغ)
{
#إذا defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    إذا (mch_getenv((char_u *)"LANG") == عدم
	    && mch_getenv((char_u *)"LANGUAGE") == عدم
	    && mch_getenv((char_u *)"LC_ALL") == عدم
	    && mch_getenv((char_u *)"LC_MESSAGES") == عدم)
    {
	char_u	*langs = get_ui_langs();

	إذا (langs != عدم && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    إذا (colon != عدم)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#وإلاإذا defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#پایان_اگر
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    ساكن فراغ
مجموعه_init_default_encoding(فراغ)
{
    char_u	*p;
    صحيح		opt_idx;

#إذا defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#وإلا
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#پایان_اگر
    إذا (p == عدم)
	أرجع;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    إذا (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    إذا (mb_init() == عدم)
    {
	opt_idx = findoption((char_u *)"encoding");
	إذا (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#إذا defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	إذا (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    مجموعه_string_option_مستقیم((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    مجموعه_string_option_مستقیم((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    إذا (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    إذا (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (فراغ)init_chartab();
	}
#پایان_اگر

#إذا defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	إذا (
# ifdef VIMDLL
		(!gui.درون_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    حرف	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    إذا (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, حجم(buf), "cp%ld", (دراز)GetACP());
	    وإلا
		buflen = vim_snprintf(buf, حجم(buf), "cp%ld", (دراز)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    إذا (p_tenc != عدم)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		إذا (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_راهاندازی(&درون_داد_conv, p_tenc, p_enc);
		convert_راهاندازی(&output_conv, p_enc, p_tenc);
	    }
	    وإلا
		p_tenc = خالی_option;
	}
#پایان_اگر
#إذا defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#پایان_اگر
    }
    وإلا
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
    فراغ
مجموعه_init_1(صحيح clean_arg)
{
#اگر_تعریف_شده FEAT_LANGMAP
    langmap_init();
#پایان_اگر

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    إذا (mch_getenv((char_u *)"VIM_POSIX") != عدم)
    {
	مجموعه_string_default("cpo", (char_u *)CPO_ALL);
	مجموعه_string_default("shm", (char_u *)SHM_POSIX);
    }

    مجموعه_init_default_shell();
    مجموعه_init_default_backupskip();
    مجموعه_init_default_maxmemtot();
    مجموعه_init_default_cdpath();
    مجموعه_init_default_printencoding();
#إذا defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    مجموعه_init_default_printexpr();
#پایان_اگر

    /*
     * Set all the options (except the terminal options) to their default
     * value.  Also set the global value for local options.
     */
    مجموعه_options_default(0);

#اگر_تعریف_شده UNIX
    مجموعه_init_xdg_rtp();
    مجموعه_init_restricted_نما();
#پایان_اگر

#اگر_تعریف_شده CLEAN_RUNTIMEPATH
    إذا (clean_arg)
	مجموعه_init_clean_rtp();
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI
    إذا (یافت_معکوس_arg)
	مجموعه_option_مقدار_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
#پایان_اگر

    curbuf->b_p_initialized = true;
    curbuf->b_p_ac = -1;
    curbuf->b_p_ar = -1;	// no local 'autoread' value
#اگر_تعریف_شده HAVE_FSYNC
    curbuf->b_p_fs = -1;	// no local 'fsync' value
#پایان_اگر
    curbuf->b_p_ul = NO_LOCAL_UNDOLEVEL;
    check_buf_options(curbuf);
    check_win_options(curwin);
    check_options();

    // Must be before option_expand(), because that one needs vim_isIDc()
    didset_options();

#اگر_تعریف_شده FEAT_SPELL
    // Use the current chartab for the generic chartab. This is not in
    // didset_options() because it only depends on 'encoding'.
    init_spell_chartab();
#پایان_اگر

    مجموعه_init_default_encoding();

    // Expand environment variables and things like "~" for the defaults.
    مجموعه_init_expand_env();

    save_file_ff(curbuf);	// Buffer is unchanged

#إذا defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    إذا (mch_getenv((char_u *)"MLTERM") != عدم)
	مجموعه_option_مقدار_give_err((char_u *)"tbidi", 1L, عدم, 0);
#پایان_اگر

    didset_options2();

    مجموعه_init_lang_env();

#اگر_تعریف_شده FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    مجموعه_helplang_default(get_mess_lang());
#پایان_اگر
}

ساكن char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    فراغ
مجموعه_fencs_unicode(فراغ)
{
    مجموعه_string_option_مستقیم((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    ساكن فراغ
مجموعه_option_default(
    صحيح		opt_idx,
    صحيح		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    صحيح		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    صحيح		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    صحيح		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    إذا (varp != عدم)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	إذا (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    إذا (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		مجموعه_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    وإلا إذا (options[opt_idx].indir != PV_NONE)
		مجموعه_string_option_مستقیم(عدم, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    وإلا
	    {
		إذا ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	وإلا إذا (flags & P_رقم)
	{
	    إذا (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    وإلا
	    {
		دراز def_val = (دراز)(long_i)options[opt_idx].def_val[dvi];

		إذا ((دراز *)varp == &curwin->w_p_so
			|| (دراز *)varp == &curwin->w_p_siso
			|| (دراز *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(دراز *)varp = -1;
		وإلا
		    *(دراز *)varp = def_val;
		// May also set global value for local option.
		إذا (both)
		    *(دراز *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	وإلا	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(صحيح *)varp = (صحيح)(دراز)(long_i)options[opt_idx].def_val[dvi];
#اگر_تعریف_شده UNIX
	    // 'modeline' defaults to off for root
	    إذا (options[opt_idx].indir == PV_ML && getuid() == ریشه_UID)
		*(صحيح *)varp = FALSE;
#پایان_اگر
	    // May also set global value for local option.
	    إذا (both)
		*(صحيح *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(صحيح *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	إذا (both)
	{
	    flagsp = insecure_flag(curwin, opt_idx, OPT_LOCAL);
	    *flagsp = *flagsp & ~P_INSECURE;
	}
    }

#اگر_تعریف_شده FEAT_EVAL
    مجموعه_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#پایان_اگر
}

/*
 * Set all options (except terminal options) to their default value.
 * When "opt_flags" is non-zero skip 'encoding'.
 */
    ساكن فراغ
مجموعه_options_default(
    صحيح		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    صحيح		i;
    win_T	*wp;
    tabpage_T	*tp;

    لكل (i = 0; !istermoption_idx(i); i++)
	إذا (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#إذا defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_کلید
#پایان_اگر
			)))
	    مجموعه_option_default(i, opt_flags, p_cp);

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
    ساكن فراغ
مجموعه_string_default_esc(حرف *name, char_u *val, صحيح escape)
{
    char_u	*p;
    صحيح		opt_idx;

    إذا (escape && vim_strchr(val, ' ') != عدم)
	p = vim_strsave_escaped(val, (char_u *)" ");
    وإلا
	p = vim_strsave(val);
    إذا (p == عدم)		// we don't want a NULL
	أرجع;

    opt_idx = findoption((char_u *)name);
    إذا (opt_idx < 0)
    {
	vim_free(p);
	أرجع;
    }

    إذا (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    فراغ
مجموعه_string_default(حرف *name, char_u *val)
{
    مجموعه_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    ساكن char_u *
پیدا_کن_dup_قلم(char_u *origval, char_u *newval, size_t newvallen, long_u flags)
{
    صحيح	    bs = 0;
    char_u  *s;

    إذا (origval == عدم)
	أرجع عدم;

    لكل (s = origval; *s != NUL; ++s)
    {
	إذا ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    أرجع s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	إذا ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	وإلا
	    bs = 0;
    }
    أرجع عدم;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    فراغ
مجموعه_عدد_default(حرف *name, دراز val)
{
    صحيح		opt_idx;

    opt_idx = findoption((char_u *)name);
    إذا (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    فراغ
مجموعه_local_options_default(win_T *wp, صحيح do_میانگیر)
{
    win_T	*save_curwin = curwin;
    صحيح		i;

    curwin = wp;
    curbuf = curwin->w_میانگیر;
    بلوک_autocmds();

    لكل (i = 0; !istermoption_idx(i); i++)
    {
	بنية vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	إذا (p->indir != PV_NONE
		&& (do_میانگیر || (p->indir & PV_BUF) == 0)
		&& !(options[i].flags & P_NODEFAULT)
		&& !optval_default(p, varp, FALSE))
	    مجموعه_option_default(i, OPT_FREE|OPT_LOCAL, FALSE);
    }

    unblock_autocmds();
    curwin = save_curwin;
    curbuf = curwin->w_میانگیر;
}
#پایان_اگر

#إذا defined(EXITFREE)
/*
 * Free all options.
 */
    فراغ
free_all_options(فراغ)
{
    صحيح		i;

    لكل (i = 0; !istermoption_idx(i); i++)
    {
	إذا (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    إذا ((options[i].flags & P_ALLOCED) && options[i].var != عدم)
		free_string_option(*(char_u **)options[i].var);
	    إذا (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	وإلا إذا (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# إذا defined(FEAT_EVAL)
    free_findfunc_option();
# endif
}
#پایان_اگر


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    فراغ
مجموعه_init_2(فراغ)
{
    صحيح		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    إذا (idx >= 0 && !(options[idx].flags & P_WAS_مجموعه))
	مجموعه_option_default(idx, OPT_LOCAL, p_cp);
    comp_ستون();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    إذا (!option_was_مجموعه((char_u *)"window"))
	p_پنجره = ردیفها - 1;
    مجموعه_عدد_default("window", ردیفها - 1);

    // For DOS console the default is always black.
#إذا !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    إذا (idx >= 0 && !(options[idx].flags & P_WAS_مجموعه)
						 && *جمله_bg_default() == 'd')
    {
	مجموعه_string_option_مستقیم(عدم, idx, (char_u *)"dark", OPT_FREE, 0);
	// don't mark it as set, when starting the GUI it may be
	// changed again
	options[idx].flags &= ~P_WAS_مجموعه;
    }
#پایان_اگر

#اگر_تعریف_شده CURSOR_SHAPE
    parse_shape_opt(SHAPE_CURSOR);	// set cursor shapes from 'guicursor'
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSESHAPE
    parse_shape_opt(SHAPE_MOUSE);	// set mouse shapes from 'mouseshape'
#پایان_اگر
#اگر_تعریف_شده FEAT_PRINTER
    (فراغ)parse_printoptions(عدم);	// parse 'printoptions' default value
#پایان_اگر
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    فراغ
مجموعه_init_3(فراغ)
{
#إذا defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    صحيح	    idx_srr;
    صحيح	    do_srr;
# ifdef FEAT_QUICKFIX
    صحيح	    idx_sp;
    صحيح	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    إذا (idx_srr < 0)
	do_srr = FALSE;
    وإلا
	do_srr = !(options[idx_srr].flags & P_WAS_مجموعه);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    إذا (idx_sp < 0)
	do_sp = FALSE;
    وإلا
	do_sp = !(options[idx_sp].flags & P_WAS_مجموعه);
# endif
    p = get_isolated_shell_name();
    إذا (p != عدم)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	إذا (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# إذا defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# إذا defined(FEAT_QUICKFIX)
	    إذا (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  وإلا
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    إذا (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	وإلا إذا (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  إذا defined(FEAT_QUICKFIX)
		إذا (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		إذا (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	وإلا
	    // Always use POSIX shell style redirection if we reach this
	    إذا (       fnamecmp(p, "sh") == 0
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
# إذا defined(FEAT_QUICKFIX)
		إذا (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  وإلا
		    إذا (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    وإلا
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		إذا (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#پایان_اگر

#إذا defined(MSWIN)
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
    إذا (جستجوی_رشته((حرف *)gettail(p_sh), "powershell") != عدم)
    {
	صحيح	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	إذا (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_مجموعه))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	إذا (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_مجموعه))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    وإلا إذا (جستجوی_رشته((حرف *)gettail(p_sh), "sh") != عدم)
    {
	صحيح	idx3;

	idx3 = findoption((char_u *)"shcf");
	إذا (idx3 >= 0 && !(options[idx3].flags & P_WAS_مجموعه))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	إذا (idx3 >= 0 && !(options[idx3].flags & P_WAS_مجموعه))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    وإلا إذا (جستجوی_رشته((حرف *)gettail(p_sh), "cmd.exe") != عدم)
    {
	صحيح	idx3;

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
	إذا (idx3 >= 0 && !(options[idx3].flags & P_WAS_مجموعه))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	إذا (idx3 >= 0 && !(options[idx3].flags & P_WAS_مجموعه))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#پایان_اگر

    إذا (BUFEMPTY())
    {
	صحيح idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	إذا (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_مجموعه))
	    مجموعه_fileformat(default_fileformat(), OPT_LOCAL);
    }

    مجموعه_title_defaults();
}

#إذا defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    فراغ
مجموعه_helplang_default(char_u *lang)
{
    صحيح		idx;
    size_t	langlen;

    إذا (lang == عدم)	// safety check
	أرجع;

    langlen = STRLEN(lang);
    إذا (langlen < 2)	// safety check
	أرجع;

    idx = findoption((char_u *)"hlg");
    إذا (idx < 0 || (options[idx].flags & P_WAS_مجموعه))
	أرجع;

    إذا (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    إذا (p_hlg == عدم)
	p_hlg = خالی_option;
    وإلا
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	إذا (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	وإلا إذا (langlen >= 1 && *p_hlg == 'C')
	{
	    p_hlg[0] = 'e';
	    p_hlg[1] = 'n';
	}
	p_hlg[2] = NUL;
    }
    options[idx].flags |= P_ALLOCED;
}
#پایان_اگر

/*
 * 'title' and 'icon' only default to true if they have not been set or reset
 * in .vimrc and we can read the old value.
 * When 'title' and 'icon' have been reset in .vimrc, we won't even check if
 * they can be reset.  This reduces startup time when using X on a remote
 * machine.
 */
    فراغ
مجموعه_title_defaults(فراغ)
{
    صحيح	    idx1;
    دراز    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    إذا (idx1 >= 0 && !(options[idx1].flags & P_WAS_مجموعه))
    {
#اگر_تعریف_شده FEAT_GUI
	إذا (gui.starting || gui.درون_use)
	    val = TRUE;
	وإلا
#پایان_اگر
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    إذا (idx1 < 0 || (options[idx1].flags & P_WAS_مجموعه))
    {
	أرجع;
    }

#اگر_تعریف_شده FEAT_GUI
    إذا (gui.starting || gui.درون_use)
	val = TRUE;
    وإلا
#پایان_اگر
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    فراغ
ex_مجموعه(exarg_T *eap)
{
    صحيح		flags = 0;

    إذا (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    وإلا إذا (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#إذا defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    إذا ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    وإلا
#پایان_اگر
    {
	إذا (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(فراغ)do_مجموعه(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
تعريفنوع تعداد {
    پیشوند_NO = 0,	// "no" prefix
    پیشوند_NONE,	// no prefix
    پیشوند_INV,		// "inv" prefix
} مجموعه_پیشوند_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    ساكن مجموعه_پیشوند_T
get_option_پیشوند(char_u **argp)
{
    صحيح		پیشوند = پیشوند_NONE;
    char_u	*arg = *argp;

    إذا (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	پیشوند = پیشوند_NO;
	arg += 2;
    }
    وإلا إذا (STRNCMP(arg, "inv", 3) == 0)
    {
	پیشوند = پیشوند_INV;
	arg += 3;
    }

    *argp = arg;
    أرجع پیشوند;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    ساكن صحيح
parse_option_name(char_u *arg, صحيح *opt_idxp, صحيح *lenp, صحيح *keyp)
{
    صحيح	کلید = 0;
    صحيح	len;
    صحيح	opt_idx;

    إذا (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	إذا (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	وإلا
	{
	    len = 1;
	    طالما (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	إذا (arg[len] != '>')
	    أرجع FAIL;

	arg[len] = NUL;			    // put NUL after name
	إذا (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	إذا (opt_idx == -1)
	    کلید = پیدا_کن_کلید_option(arg + 1, TRUE);
    }
    وإلا
    {
	صحيح	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	إذا (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	وإلا
	    طالما (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	إذا (opt_idx == -1)
	    کلید = پیدا_کن_کلید_option(arg, FALSE);
    }

    *keyp = کلید;
    *lenp = len;
    *opt_idxp = opt_idx;

    أرجع OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    ساكن مجموعه_op_T
get_opt_op(char_u *arg)
{
    مجموعه_op_T op = OP_NONE;

    إذا (*arg != NUL && *(arg + 1) == '=')
    {
	إذا (*arg == '+')
	    op = OP_ADDING;		// "+="
	وإلا إذا (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	وإلا إذا (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    أرجع op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
ساكن حرف *modeline_whitelist[] =
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
    عدم
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    ساكن bool
is_modeline_whitelisted(حرف *name)
{
    لكل (صحيح i = 0; modeline_whitelist[i] != عدم; i++)
	إذا (STRCMP(name, modeline_whitelist[i]) == 0)
	    أرجع true;
    أرجع false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    ساكن صحيح
validate_opt_idx(
	صحيح opt_idx,
	صحيح opt_flags,
	long_u flags,
	حرف **errmsg,
	مجموعه_پیشوند_T پیشوند)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    إذا ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	أرجع FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    إذا ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	أرجع FAIL;

    // Disallow changing some options from modelines.
    إذا (opt_flags & OPT_MODELINE)
    {
	إذا (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_درون_modeline;
	    أرجع FAIL;
	}
	إذا ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_درون_modeline_when_modelineexpr_is_off;
	    أرجع FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	إذا (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    إذا (options[opt_idx].indir == PV_ML && پیشوند == پیشوند_NO)
		أرجع OK;
	    وإلا إذا (!is_modeline_whitelisted(options[opt_idx].fullname))
		أرجع FAIL;
	}
#اگر_تعریف_شده FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	إذا (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    أرجع FAIL;
#پایان_اگر
    }

#اگر_تعریف_شده HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    إذا (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_درون_sandbox;
	أرجع FAIL;
    }
#پایان_اگر

    أرجع OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    ساكن char_u *
stropt_get_default_val(
    صحيح		opt_idx,
    char_u	*varp,
    صحيح		flags,
    صحيح		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    إذا ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#اگر_تعریف_شده FEAT_GUI
	إذا (gui.درون_use)
	    newval = gui_bg_default();
	وإلا
#پایان_اگر
	    newval = جمله_bg_default();
    }
    وإلا إذا ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    إذا (newval == عدم)
	newval = خالی_option;
    وإلا
    {
	char_u *s = option_expand(opt_idx, newval);
	إذا (s == عدم)
	    s = newval;
	newval = vim_strsave(s);
    }

    أرجع newval;
}

/*
 * Convert the 'backspace' option number value to a string: for adding,
 * prepending and removing string.
 */
    ساكن فراغ
opt_backspace_nr2str(
    char_u	*varp,
    char_u	**origval_p,
    char_u	**origval_l_p,
    char_u	**origval_g_p,
    char_u	**oldval_p)
{
    صحيح		i = getdigits((char_u **)varp);

    بدل (i)
    {
	حالة 0:
	    *(char_u **)varp = خالی_option;
	    اكسر;
	حالة 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    اكسر;
	حالة 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    اكسر;
	حالة 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    اكسر;
    }
    vim_free(*oldval_p);
    إذا (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    إذا (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    إذا (*origval_g_p == *oldval_p)
	*origval_g_p = *(char_u **)varp;
    *oldval_p = *(char_u **)varp;
}

/*
 * Convert the 'whichwrap' option number value to a string, for backwards
 * compatibility with Vim 3.0.
 * Note: 'argp' is a pointer to a char_u pointer and is updated.
 */
    ساكن char_u *
opt_whichwrap_nr2str(char_u **argp, char_u *whichwrap)
{
    size_t  len = 0;

    *whichwrap = NUL;
    صحيح i = getdigits(argp);
    إذا (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    إذا (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    إذا (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    إذا (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    إذا (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    إذا (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    أرجع whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    ساكن char_u *
stropt_copy_مقدار(
    char_u	*origval,
    char_u	**argp,
    مجموعه_op_T	op,
    صحيح		flags استفادهنشده)
{
    char_u	*arg = *argp;
    بیعلامت	newlen;
    char_u	*newval;
    char_u	*s = عدم;

    // get a bit too much
    newlen = (بیعلامت)STRLEN(arg) + 1;
    إذا (op != OP_NONE)
	newlen += (بیعلامت)STRLEN(origval) + 1;
    newval = alloc(newlen);
    إذا (newval == عدم)  // out of mem, don't change
	أرجع عدم;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    طالما (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	صحيح i;

	إذا (*arg == '\\' && arg[1] != NUL
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#پایان_اگر
	   )
	    ++arg;	// remove backslash
	إذا (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	وإلا
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    أرجع newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    ساكن char_u *
stropt_expand_envvar(
    صحيح		opt_idx,
    char_u	*origval,
    char_u	*newval,
    مجموعه_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    إذا (s == عدم)
	أرجع newval;

    vim_free(newval);
    بیعلامت newlen = (بیعلامت)STRLEN(s) + 1;
    إذا (op != OP_NONE)
	newlen += (بیعلامت)STRLEN(origval) + 1;

    newval = alloc(newlen);
    إذا (newval == عدم)
	أرجع عدم;

    STRCPY(newval, s);

    أرجع newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    ساكن فراغ
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    مجموعه_op_T	op,
    صحيح		flags)
{
    صحيح		len = 0;

    صحيح comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    إذا (op == OP_ADDING)
    {
	len = (صحيح)STRLEN(origval);
	// strip a trailing comma, would get 2
	إذا (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    وإلا
    {
	len = (صحيح)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    إذا (comma)
	newval[len] = ',';
}

/*
 * Remove a value from a string option.  Copy string option value in "origval"
 * to "newval" and then remove the string "strval" of length "len".
 */
    ساكن فراغ
stropt_remove_val(
    char_u	*origval,
    char_u	*newval,
    صحيح		flags,
    char_u	*strval,
    صحيح		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    إذا (*strval)
    {
	// may need to remove a comma
	إذا (flags & P_COMMA)
	{
	    إذا (strval == origval)
	    {
		// include comma after string
		إذا (strval[len] == ',')
		    ++len;
	    }
	    وإلا
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
    ساكن char_u *
پیدا_کن_کلید_قلم(char_u *src, char_u *کلید, صحيح keylen, صحيح *itemlenp)
{
    char_u  *p = src;

    طالما (*p != NUL)
    {
	// Check if this item starts with the same key
	إذا ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, کلید, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *انتها = vim_strchr(p, ',');
	    إذا (انتها == عدم)
		انتها = p + STRLEN(p);
	    *itemlenp = (صحيح)(انتها - p);
	    أرجع p;
	}
	++p;
    }
    أرجع عدم;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    ساكن فراغ
remove_comma_قلم(char_u *str, char_u *قلم, صحيح itemlen)
{
    إذا (قلم[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(قلم, قلم + itemlen + 1);
    وإلا إذا (قلم > str && *(قلم - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(قلم - 1, قلم + itemlen);
    وإلا
	// Only item
	*قلم = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    ساكن فراغ
remove_کلید_قلم(char_u *str, char_u *کلید, صحيح keylen, char_u *skip)
{
    صحيح	    itemlen;
    char_u  *یافت;

    طالما ((یافت = پیدا_کن_کلید_قلم(str, کلید, keylen, &itemlen)) != عدم)
    {
	إذا (یافت == skip)
	{
	    // Search for the next match after this one.
	    char_u *بعدی = یافت + itemlen;
	    إذا (*بعدی == ',')
		++بعدی;
	    یافت = پیدا_کن_کلید_قلم(بعدی, کلید, keylen, &itemlen);
	    إذا (یافت == عدم)
		اكسر;
	}

	remove_comma_قلم(str, یافت, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    ساكن فراغ
append_قلم(char_u *str, char_u *قلم, صحيح قلم_len)
{
    صحيح len = (صحيح)STRLEN(str);

    إذا (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, قلم, (size_t)قلم_len);
    str[len + قلم_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    ساكن فراغ
prepend_قلم(char_u *str, char_u *قلم, صحيح قلم_len)
{
    صحيح len = (صحيح)STRLEN(str);
    صحيح comma = (len > 0) ? 1 : 0;

    mch_memmove(str + قلم_len + comma, str, (size_t)len + 1);
    mch_memmove(str, قلم, (size_t)قلم_len);
    إذا (comma)
	str[قلم_len] = ',';
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
    ساكن bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    مجموعه_op_T	op,
    صحيح		flags استفادهنشده)
{
    char_u  *p;
    char_u  *قلم_شروع;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    إذا (vim_strchr(newval, ':') == عدم && vim_strchr(newval, ',') == عدم)
	أرجع false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    إذا (newval_copy == عدم)
	أرجع false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    قلم_شروع = newval_copy;
    لكل (;;)
    {
	p = vim_strchr(قلم_شروع, ',');
	صحيح قلم_len = (p == عدم)
			    ? (صحيح)STRLEN(قلم_شروع) : (صحيح)(p - قلم_شروع);

	إذا (قلم_len > 0)
	{
	    char_u *colon = vim_strchr(قلم_شروع, ':');
	    إذا (colon != عدم && colon < قلم_شروع + قلم_len)
	    {
		صحيح keylen = (صحيح)(colon - قلم_شروع) + 1;

		إذا (op == OP_ADDING || op == OP_PREPENDING)
		{
		    صحيح old_itemlen;
		    char_u *یافت = پیدا_کن_کلید_قلم(newval, قلم_شروع,
						       keylen, &old_itemlen);
		    إذا (یافت != عدم)
		    {
			إذا (old_itemlen == قلم_len
				&& STRNCMP(یافت, قلم_شروع,
							 قلم_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_کلید_قلم(newval, قلم_شروع,
							   keylen, یافت);
			}
			وإلا
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_کلید_قلم(newval, قلم_شروع,
							    keylen, عدم);
			    إذا (op == OP_PREPENDING)
				prepend_قلم(newval, قلم_شروع, قلم_len);
			    وإلا
				append_قلم(newval, قلم_شروع, قلم_len);
			}
		    }
		    وإلا
		    {
			// New item.
			إذا (op == OP_PREPENDING)
			    prepend_قلم(newval, قلم_شروع, قلم_len);
			وإلا
			    append_قلم(newval, قلم_شروع, قلم_len);
		    }
		}
		وإلا إذا (op == OP_REMOVING)
		    remove_کلید_قلم(newval, قلم_شروع, keylen, عدم);
	    }
	    وإلا
	    {
		إذا (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *یافت = پیدا_کن_dup_قلم(newval, قلم_شروع,
							   قلم_len, P_COMMA);
		    إذا (یافت == عدم)
		    {
			// New item.
			إذا (op == OP_PREPENDING)
			    prepend_قلم(newval, قلم_شروع, قلم_len);
			وإلا
			    append_قلم(newval, قلم_شروع, قلم_len);
		    }
		    // else: exact duplicate — do nothing
		}
		وإلا إذا (op == OP_REMOVING)
		{
		    char_u *یافت = پیدا_کن_dup_قلم(newval, قلم_شروع,
							   قلم_len, P_COMMA);
		    إذا (یافت != عدم)
			remove_comma_قلم(newval, یافت, قلم_len);
		}
	    }
	}

	إذا (p == عدم)
	    اكسر;
	قلم_شروع = p + 1;
    }

    vim_free(newval_copy);

    أرجع true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    ساكن فراغ
stropt_remove_dupflags(char_u *newval, صحيح flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    طالما (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	إذا (flags & P_ONECOMMA)
	{
	    إذا (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != عدم)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		استمر;
	    }
	}
	وإلا
	{
	    إذا ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != عدم)
	    {
		STRMOVE(s, s + 1);
		استمر;
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
    ساكن char_u *
stropt_get_newval(
    صحيح		nextchar,
    صحيح		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    مجموعه_op_T    *op_arg,
    صحيح		flags,
    صحيح		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    مجموعه_op_T    op = *op_arg;
    char_u	*save_arg = عدم;
    char_u	*newval;
    char_u	*s = عدم;
    char_u	whichwrap[80];

    إذا (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    وإلا إذا (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    وإلا
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	إذا (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	وإلا إذا (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	وإلا إذا (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	وإلا إذا (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_مقدار(origval, &arg, op, flags);
	إذا (newval == عدم)
	    اذهب انجامشده;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	إذا (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    إذا (newval == عدم)
		اذهب انجامشده;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	إذا ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	وإلا
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    صحيح len = 0;
	    إذا (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (صحيح)STRLEN(newval);
		s = پیدا_کن_dup_قلم(origval, newval, len, flags);

		// do not add if already there
		إذا ((op == OP_ADDING || op == OP_PREPENDING) && s != عدم)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		إذا (s == عدم)
		    s = origval + (صحيح)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    إذا (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    وإلا إذا (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	إذا (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

انجامشده:
    إذا (save_arg != عدم)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    أرجع newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    ساكن صحيح
do_مجموعه_option_string(
	صحيح	    opt_idx,
	صحيح	    opt_flags,
	char_u	    **argp,
	صحيح	    nextchar,
	مجموعه_op_T    op_arg,
	long_u	    flags,
	صحيح	    cp_val,
	char_u	    *varp_arg,
	حرف	    *errbuf,
	size_t	    errbuflen,
	صحيح	    *مقدار_checked,
	حرف	    **errmsg)
{
    char_u	*arg = *argp;
    مجموعه_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = عدم; // previous value if *varp
    char_u	*newval;
    char_u	*origval = عدم;
    char_u	*origval_l = عدم;
    char_u	*origval_g = عدم;
#إذا defined(FEAT_EVAL)
    char_u	*saved_origval = عدم;
    char_u	*saved_origval_l = عدم;
    char_u	*saved_origval_g = عدم;
    char_u	*saved_newval = عدم;
#پایان_اگر

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((صحيح)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	إذا (((صحيح)options[opt_idx].indir & PV_BOTH)
						  && origval_l == خالی_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    إذا (((صحيح)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    وإلا
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    إذا (newval == عدم)
	*(char_u **)(varp) = خالی_option;

#إذا defined(FEAT_EVAL)
    إذا (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_کلید
# endif
		      && origval != عدم && newval != عدم)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	إذا (origval_l != عدم)
	    saved_origval_l = vim_strsave(origval_l);
	إذا (origval_g != عدم)
	    saved_origval_g = vim_strsave(origval_g);
    }
#پایان_اگر

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	صحيح	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	إذا ((opt_flags & OPT_MODELINE)
#اگر_تعریف_شده HAVE_SANDBOX
	      || sandbox != 0
#پایان_اگر
	      || (op != OP_NONE && (*p & P_INSECURE)))
	    secure = 1;

	// Handle side effects, and set the global value for ":set" on local
	// options. Note: when setting 'syntax' or 'filetype' autocommands may
	// be triggered that can cause havoc.
	*errmsg = did_مجموعه_string_option(
			opt_idx, (char_u **)varp, oldval, newval, errbuf,
			errbuflen, opt_flags, op, مقدار_checked);

	secure = secure_saved;
    }

#إذا defined(FEAT_EVAL)
    إذا (*errmsg == عدم)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#پایان_اگر

    *argp = arg;
    أرجع *errmsg == عدم ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    ساكن حرف *
do_مجموعه_option_bool(
    صحيح		opt_idx,
    صحيح		opt_flags,
    مجموعه_پیشوند_T پیشوند,
    long_u	flags,
    char_u	*varp,
    صحيح		nextchar,
    صحيح		afterchar,
    صحيح		cp_val)

{
    varnumber_T	مقدار;

    إذا (nextchar == '=' || nextchar == ':')
	أرجع e_invalid_آرگومان;
    إذا (opt_idx < 0 || varp == عدم)
	أرجع عدم;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    إذا (nextchar == '!')
	مقدار = *(صحيح *)(varp) ^ 1;
    وإلا إذا (nextchar == '&')
	مقدار = (صحيح)(دراز)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    وإلا إذا (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	إذا ((صحيح *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    مقدار = -1;
	وإلا إذا ((صحيح *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    مقدار = -1;
#اگر_تعریف_شده HAVE_FSYNC
	وإلا إذا ((صحيح *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    مقدار = -1;
#پایان_اگر
	وإلا
	    مقدار = *(صحيح *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    وإلا
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	إذا (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    أرجع e_trailing_characters;
	إذا (پیشوند == پیشوند_INV)
	    مقدار = *(صحيح *)(varp) ^ 1;
	وإلا
	    مقدار = پیشوند == پیشوند_NO ? 0 : 1;
    }

    أرجع مجموعه_bool_option(opt_idx, varp, (صحيح)مقدار, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    ساكن حرف *
do_مجموعه_option_numeric(
    صحيح		opt_idx,
    صحيح		opt_flags,
    char_u	**argp,
    صحيح		nextchar,
    مجموعه_op_T	op,
    long_u	flags,
    صحيح		cp_val,
    char_u	*varp,
    حرف	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		مقدار;
    صحيح			i;
    حرف		*errmsg = عدم;

    إذا (opt_idx < 0 || varp == عدم)
	أرجع عدم;  // "cannot happen"
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
    إذا (nextchar == '&')
	مقدار = (دراز)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    وإلا إذا (nextchar == '<')
    {
	إذا ((دراز *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    مقدار = NO_LOCAL_UNDOLEVEL;
	وإلا إذا (opt_flags == OPT_LOCAL
		    && ((دراز *)varp == &curwin->w_p_siso
		     || (دراز *)varp == &curwin->w_p_so
		     || (دراز *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    مقدار = -1;
	وإلا
	    مقدار = *(دراز *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    وإلا إذا (((دراز *)varp == &p_wc || (دراز *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	مقدار = string_مقصد_کلید(arg, FALSE);
	إذا (مقدار == 0)
	{
	    errmsg = e_invalid_آرگومان;
	    اذهب skip;
	}
    }
    وإلا إذا (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, عدم, &i, STR2NR_ALL, &مقدار, عدم, 0, TRUE, عدم);
	إذا (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_عدد_required_after_equal;
	    اذهب skip;
	}
    }
    وإلا
    {
	errmsg = e_عدد_required_after_equal;
	اذهب skip;
    }

    إذا (op == OP_ADDING)
	مقدار = *(دراز *)varp + مقدار;
    وإلا إذا (op == OP_PREPENDING)
	مقدار = *(دراز *)varp * مقدار;
    وإلا إذا (op == OP_REMOVING)
	مقدار = *(دراز *)varp - مقدار;

    إذا ((دراز *)varp == &curwin->w_p_sop && مقدار < -1)
    {
	errmsg = e_invalid_آرگومان;
	اذهب skip;
    }

    errmsg = مجموعه_رقم_option(opt_idx, varp, مقدار, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    أرجع errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    ساكن حرف *
do_مجموعه_option_keycode(char_u **argp, char_u *کلید_name, صحيح nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    إذا (nextchar == '&')
    {
	إذا (جمع_کن_termcap_مدخل(کلید_name, TRUE) == FAIL)
	    أرجع e_not_یافت_درون_termcap;
    }
    وإلا
    {
	++arg; // jump to after the '=' or ':'
	لكل (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    إذا (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	جمع_کن_termcode(کلید_name, arg, FALSE);
	*p = nextchar;
    }
    إذا (پر_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    أرجع عدم;
}

/*
 * Set an option to a new value.
 */
    ساكن حرف *
do_مجموعه_option_مقدار(
    صحيح		opt_idx,
    صحيح		opt_flags,
    char_u	**argp,
    مجموعه_پیشوند_T پیشوند,
    مجموعه_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*کلید_name,
    صحيح		nextchar,
    صحيح		afterchar,
    صحيح		cp_val,
    صحيح		*stopopteval,
    حرف	*errbuf,
    size_t	errbuflen)
{
    صحيح		مقدار_checked = FALSE;
    حرف	*errmsg = عدم;
    char_u	*arg = *argp;

    إذا (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_مجموعه_option_bool(opt_idx, opt_flags, پیشوند, flags, varp,
						nextchar, afterchar, cp_val);
	إذا (errmsg != عدم)
	    اذهب skip;
    }
    وإلا
    {
	// numeric or string option
	إذا (vim_strchr((char_u *)"=:&<", nextchar) == عدم
		|| پیشوند != پیشوند_NONE)
	{
	    errmsg = e_invalid_آرگومان;
	    اذهب skip;
	}

	إذا (flags & P_رقم)
	{
	    // numeric option
	    errmsg = do_مجموعه_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    إذا (errmsg != عدم)
		اذهب skip;
	}
	وإلا إذا (opt_idx >= 0)
	{
	    // string option
	    إذا (do_مجموعه_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&مقدار_checked, &errmsg) == FAIL)
	    {
		إذا (errmsg != عدم)
		    اذهب skip;
		*stopopteval = TRUE;
		اذهب skip;
	    }
	}
	وإلا
	{
	    // key code option
	    errmsg = do_مجموعه_option_keycode(&arg, کلید_name, nextchar);
	    إذا (errmsg != عدم)
		اذهب skip;
	}
    }

    إذا (opt_idx >= 0)
	did_مجموعه_option(opt_idx, opt_flags, op == OP_NONE, مقدار_checked);

skip:
    *argp = arg;
    أرجع errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    ساكن حرف *
do_مجموعه_option(
    صحيح		opt_flags,
    char_u	**argp,
    char_u	*arg_شروع,
    char_u	**startarg,
    صحيح		*did_show,
    صحيح		*stopopteval,
    حرف	*errbuf,
    size_t	errbuflen)
{
    صحيح		opt_idx;
    char_u	*arg;
    مجموعه_پیشوند_T پیشوند;	// no prefix, "no" prefix or "inv" prefix
    مجموعه_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	کلید_name[2];
    صحيح		nextchar;	// next non-white char after option name
    صحيح		afterchar;	// character just after option name
    حرف	*errmsg = عدم;
    صحيح		کلید;
    صحيح		len;

    پیشوند = get_option_پیشوند(argp);
    arg = *argp;

    // find end of name
    کلید = 0;
    إذا (parse_option_name(arg, &opt_idx, &len, &کلید) == FAIL)
	أرجع e_invalid_آرگومان;

    // remember character after option name
    afterchar = arg[len];

    إذا (درون_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	إذا (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != عدم
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    اذهب skip;
	}
    }
    وإلا
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	طالما (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    إذا (op != OP_NONE)
	len++;

    nextchar = arg[len];

    إذا (opt_idx == -1 && کلید == 0)	// found a mismatch: skip
    {
	إذا (درون_vim9script() && arg > arg_شروع
		&& vim_strchr((char_u *)"!&<", *arg) != عدم)
	    errmsg = e_no_white_space_allowed_between_option_and;
	وإلا
	    errmsg = e_unknown_option;
	اذهب skip;
    }

    إذا (opt_idx >= 0)
    {
	إذا (options[opt_idx].var == عدم)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    إذا (vim_strchr((char_u *)"=:!&<", nextchar) == عدم
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    اذهب skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    وإلا
    {
	flags = P_STRING;
	varp = عدم;
	إذا (کلید < 0)
	{
	    کلید_name[0] = KEY2TERMCAP0(کلید);
	    کلید_name[1] = KEY2TERMCAP1(کلید);
	}
	وإلا
	{
	    کلید_name[0] = KS_کلید;
	    کلید_name[1] = (کلید & 0xff);
	}
    }

    // Make sure the option value can be changed.
    إذا (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, پیشوند) == FAIL)
	اذهب skip;

    صحيح cp_val = p_cp;
    إذا (vim_strchr((char_u *)"?=:!&<", nextchar) != عدم)
    {
	arg += len;
	إذا (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    إذا (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    وإلا		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	إذا (vim_strchr((char_u *)"?!&<", nextchar) != عدم
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    اذهب skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    إذا (nextchar == '?'
	    || (پیشوند == پیشوند_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == عدم
		&& !(flags & P_BOOL)))
    {
	// print value
	إذا (*did_show)
	    پیام_putchar('\n');	    // cursor below last one
	وإلا
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	إذا (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#اگر_تعریف_شده FEAT_EVAL
	    إذا (p_verbose > 0)
	    {
		// Mention where the option was last set.
		إذا (varp == options[opt_idx].var)
		    آخرین_مجموعه_پیام(options[opt_idx].script_ctx);
		وإلا إذا ((صحيح)options[opt_idx].indir & PV_WIN)
		    آخرین_مجموعه_پیام(curwin->w_p_script_ctx[
			    (صحيح)options[opt_idx].indir & PV_ماسک]);
		وإلا إذا ((صحيح)options[opt_idx].indir & PV_BUF)
		    آخرین_مجموعه_پیام(curbuf->b_p_script_ctx[
			    (صحيح)options[opt_idx].indir & PV_ماسک]);
	    }
#پایان_اگر
	}
	وإلا
	{
	    char_u	    *p;

	    p = پیدا_کن_termcode(کلید_name);
	    إذا (p == عدم)
	    {
		errmsg = e_کلید_کد_not_مجموعه;
		اذهب skip;
	    }
	    وإلا
		(فراغ)show_one_termcode(کلید_name, p, TRUE);
	}
	إذا (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    وإلا
    {
	errmsg = do_مجموعه_option_مقدار(opt_idx, opt_flags, &arg, پیشوند, op,
					flags, varp, کلید_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    أرجع errmsg;
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
    صحيح
do_مجموعه(
    char_u	*arg_شروع,	// option string (may be written to!)
    صحيح		opt_flags)
{
    char_u	*arg = arg_شروع;
    صحيح		i;
    صحيح		did_show = FALSE;   // already showed one value

    إذا (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	اذهب theend;
    }

    طالما (*arg != NUL)		// loop to process all options
    {
	إذا (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    إذا (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		مجموعه_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    وإلا
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	وإلا إذا (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	وإلا
	{
	    صحيح		stopopteval = FALSE;
	    حرف	*errmsg = عدم;
	    حرف	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_مجموعه_option(opt_flags, &arg, arg_شروع, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    إذا (stopopteval)
		اكسر;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    لكل (i = 0; i < 2 ; ++i)
	    {
		طالما (*arg != NUL && !VIM_ISWHITE(*arg))
		    إذا (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		إذا (*arg != '=')
		    اكسر;
	    }

	    إذا (errmsg != عدم)
	    {
		i = vim_snprintf((حرف *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		إذا (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((حرف *)IObuff);	// show error highlighted
		--no_wait_return;

		أرجع FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    إذا (silent_نما && did_show)
    {
	// After displaying option values in silent mode.
	silent_نما = FALSE;
	info_پیام = TRUE;	// use mch_msg(), not mch_errmsg()
	پیام_putchar('\n');
	cursor_on();		// msg_start() switches it off
	بیرون_flush();
	silent_نما = TRUE;
	info_پیام = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    أرجع OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    فراغ
did_مجموعه_option(
    صحيح	    opt_idx,
    صحيح	    opt_flags,	    // possibly with OPT_MODELINE
    صحيح	    new_مقدار_2,	    // value was replaced completely
    صحيح	    مقدار_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = عدم;
    صحيح		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_مجموعه;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    إذا (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    إذا (!مقدار_checked && (secure
#اگر_تعریف_شده HAVE_SANDBOX
	    || sandbox != 0
#پایان_اگر
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	إذا (flagsp_local != عدم)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    وإلا إذا (new_مقدار_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	إذا (flagsp_local != عدم)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    صحيح
string_مقصد_کلید(char_u *arg, صحيح multi_byte)
{
    إذا (*arg == '<' && arg[1])
	أرجع پیدا_کن_کلید_option(arg + 1, TRUE);
    إذا (*arg == '^' && arg[1])
    {
	صحيح کلید = Ctrl_chr(arg[1]);
	إذا (کلید == 0)		// ^@ is <Nul>
	    کلید = K_ZERO;
	أرجع کلید;
    }
    إذا (multi_byte)
	أرجع PTR2CHAR(arg);
    أرجع *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    فراغ
did_مجموعه_title(فراغ)
{
    إذا (starting != NO_SCREEN
#اگر_تعریف_شده FEAT_GUI
	    && !gui.starting
#پایان_اگر
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    فراغ
مجموعه_options_bin(
    صحيح		oldval,
    صحيح		newval,
    صحيح		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    إذا (newval)
    {
	إذا (!oldval)		// switched on
	{
	    إذا (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    إذا (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	إذا (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	إذا (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    وإلا إذا (oldval)		// switched off
    {
	إذا (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	إذا (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#إذا defined(FEAT_EVAL)
    // Remember where the dependent option were reset
    didset_options_sctx(opt_flags, p_bin_dep_opts);
#پایان_اگر
}

/*
 * Expand environment variables for some string options.
 * These string options cannot be indirect!
 * If "val" is NULL expand the current value of the option.
 * Return pointer to NameBuff, or NULL when not expanded.
 */
    ساكن char_u *
option_expand(صحيح opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    إذا (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == عدم)
	أرجع عدم;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    إذا (val != عدم && STRLEN(val) > MAXPATHL)
	أرجع عدم;

    إذا (val == عدم)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    صحيح esc = var == &p_برچسبها || var == &p_مسیر;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : عدم, FALSE,
#اگر_تعریف_شده FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#پایان_اگر
				  عدم);
    إذا (STRCMP(NameBuff, val) == 0)   // they are the same
	أرجع عدم;

    أرجع NameBuff;
}

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    ساكن فراغ
didset_options(فراغ)
{
    // initialize the table for 'iskeyword' et.al.
    (فراغ)init_chartab();

    didset_string_options();

#اگر_تعریف_شده FEAT_SPELL
    (فراغ)spell_check_msm();
    (فراغ)spell_check_sps();
    (فراغ)compile_cap_prog(curwin->w_s);
    (فراغ)did_مجموعه_spell_option();
#پایان_اگر
    // set cedit_key
    (فراغ)did_مجموعه_cedit(عدم);
#اگر_تعریف_شده FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_مجموعه_breakat(عدم);
#پایان_اگر
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    ساكن فراغ
didset_options2(فراغ)
{
    // Initialize the highlight_attr[] table.
    (فراغ)highlight_تغییریافته();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (فراغ)مجموعه_listchars_option(curwin, curwin->w_p_lcs, TRUE, عدم, 0);

    // Parse default for 'fillchars'.
    (فراغ)مجموعه_fillchars_option(curwin, curwin->w_p_fcs, TRUE, عدم, 0);

#اگر_تعریف_شده FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (فراغ)did_مجموعه_clipboard(عدم);
#پایان_اگر
#اگر_تعریف_شده FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (فراغ)tabstop_مجموعه(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (فراغ)tabstop_مجموعه(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#پایان_اگر
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    فراغ
check_options(فراغ)
{
    صحيح		opt_idx;

    لكل (opt_idx = 0; options[opt_idx].fullname != عدم; opt_idx++)
	إذا ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != عدم)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    صحيح
get_جمله_opt_idx(char_u **p)
{
    صحيح opt_idx;

    لكل (opt_idx = 1; options[opt_idx].fullname != عدم; opt_idx++)
	إذا (options[opt_idx].var == (char_u *)p)
	    أرجع opt_idx;
    أرجع -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    صحيح
مجموعه_جمله_option_alloced(char_u **p)
{
    صحيح		opt_idx = get_جمله_opt_idx(p);

    إذا (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    أرجع opt_idx;
}

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    صحيح
was_مجموعه_insecurely(win_T *wp, char_u *opt, صحيح opt_flags)
{
    صحيح	    idx = findoption(opt);
    long_u  *flagp;

    إذا (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	أرجع (*flagp & P_INSECURE) != 0;
    }
    داخلی_error("was_set_insecurely()");
    أرجع -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    ساكن long_u *
insecure_flag(win_T *wp, صحيح opt_idx, صحيح opt_flags)
{
    إذا (opt_flags & OPT_LOCAL)
	بدل ((صحيح)options[opt_idx].indir)
	{
	    حالة PV_WRAP:	أرجع &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    حالة PV_STL:	أرجع &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    حالة PV_FDE:	أرجع &wp->w_p_fde_flags;
	    حالة PV_FDT:	أرجع &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    حالة PV_BEXPR:	أرجع &wp->w_میانگیر->b_p_bexpr_flags;
#  endif
	    حالة PV_INDE:	أرجع &wp->w_میانگیر->b_p_inde_flags;
	    حالة PV_FEX:	أرجع &wp->w_میانگیر->b_p_fex_flags;
#  ifdef FEAT_پیدا_کن_ID
	    حالة PV_INEX:	أرجع &wp->w_میانگیر->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    حالة PV_CPT:	أرجع &wp->w_میانگیر->b_p_cpt_flags;
#  endif
# endif
	}
    وإلا
	// For global value of window-local options, use flags in w_allbuf_opt.
	بدل ((صحيح)options[opt_idx].indir)
	{
	    حالة PV_WRAP:	أرجع &wp->w_allbuf_opt.wo_wrap_flags;
# إذا defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    حالة PV_FDE:	أرجع &wp->w_allbuf_opt.wo_fde_flags;
	    حالة PV_FDT:	أرجع &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    أرجع &options[opt_idx].flags;
}
#پایان_اگر

/*
 * Redraw the window title and/or tab page text later.
 */
    فراغ
redraw_titles(فراغ)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#پایان_اگر
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    صحيح
valid_name(char_u *val, حرف *allowed)
{
    char_u *s;

    لكل (s = val; *s != NUL; ++s)
	إذا (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == عدم)
	    أرجع FALSE;
    أرجع TRUE;
}

#إذا defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    فراغ
مجموعه_option_sctx_idx(صحيح opt_idx, صحيح opt_flags, sctx_T script_ctx)
{
    صحيح		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    صحيح		indir = (صحيح)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    إذا (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    إذا (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_نسخه = SCRIPT_نسخه_VIM9;
    إذا (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_نسخه = SCRIPT_نسخه_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    إذا (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    إذا (both || (opt_flags & OPT_LOCAL))
    {
	إذا (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_ماسک] = new_script_ctx;
	وإلا إذا (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_ماسک] = new_script_ctx;
	    إذا (both)
		// also setting the "all buffers" value
		curwin->w_allbuf_opt.wo_script_ctx[indir & PV_ماسک] =
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
is_option_مقدار_vim9(صحيح opt_idx, صحيح opt_flags)
{
    صحيح		indir = (صحيح)options[opt_idx].indir;
    sctx_T	*sctx = عدم;

    إذا ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    إذا ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	إذا (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_ماسک];
	وإلا إذا (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_ماسک];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    أرجع !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_نسخه >= SCRIPT_نسخه_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(حرف *name)
{
    صحيح idx = findoption((char_u *)name);

    إذا (idx >= 0)
	أرجع &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    أرجع عدم;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    فراغ
مجموعه_جمله_option_sctx_idx(حرف *name, صحيح opt_idx)
{
    char_u  buf[5];
    صحيح	    idx;

    إذا (name == عدم)
	idx = opt_idx;
    وإلا
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    إذا (idx >= 0)
	مجموعه_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#پایان_اگر

#إذا defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    ساكن فراغ
apply_optionset_autocmd(
	صحيح	opt_idx,
	دراز	opt_flags,
	دراز	oldval,
	دراز	oldval_g,
	دراز	newval,
	حرف	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    إذا (starting || errmsg != عدم || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	أرجع;

    len = vim_snprintf_safelen((حرف *)buf_new, حجم(buf_new), "%ld", newval);
    مجموعه_vim_var_string(VV_OPTION_NEW, buf_new, (صحيح)len);
    len = vim_snprintf_safelen((حرف *)buf_type, حجم(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    مجموعه_vim_var_string(VV_OPTION_TYPE, buf_type, (صحيح)len);

    buf_oldlen = vim_snprintf_safelen((حرف *)buf_old, حجم(buf_old), "%ld", oldval);
    مجموعه_vim_var_string(VV_OPTION_OLD, buf_old, (صحيح)buf_oldlen);

    إذا (opt_flags & OPT_LOCAL)
    {
	مجموعه_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	مجموعه_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (صحيح)buf_oldlen);
    }
    إذا (opt_flags & OPT_GLOBAL)
    {
	مجموعه_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	مجموعه_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (صحيح)buf_oldlen);
    }
    إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((حرف *)buf_old_global,
	    حجم(buf_old_global), "%ld", oldval_g);

	مجموعه_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	مجموعه_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (صحيح)buf_oldlen);
	مجموعه_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (صحيح)buf_old_globallen);
    }
    إذا (opt_flags & OPT_MODELINE)
    {
	مجموعه_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	مجموعه_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (صحيح)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    عدم, FALSE, عدم);
    reset_v_option_vars();
}
#پایان_اگر

#إذا defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    حرف *
did_مجموعه_arabic(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	إذا (!p_tbidi)
	{
	    // set rightleft mode
	    إذا (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		تغییریافته_پنجره_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    إذا (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	إذا (STRCMP(p_enc, "utf-8") != 0)
	{
	    ساكن حرف *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    پیام_منبع(HL_ATTR(HLF_W));
	    پیام_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    مجموعه_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = مجموعه_option_مقدار((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    وإلا
    {
	// 'arabic' is reset, handle various sub-settings.
	إذا (!p_tbidi)
	{
	    // reset rightleft mode
	    إذا (curwin->w_p_rl)
	    {
		curwin->w_p_rl = FALSE;
		تغییریافته_پنجره_setting();
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

    أرجع errmsg;
}
#پایان_اگر

#إذا defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    حرف *
did_مجموعه_autochdir(optset_T *آرگومانها استفادهنشده)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    أرجع عدم;
}
#پایان_اگر

#إذا defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    حرف *
did_مجموعه_ballooneval(optset_T *آرگومانها)
{
    إذا (balloon_Eval_For_جمله)
	أرجع عدم;

    إذا (p_beval && !آرگومانها->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    وإلا إذا (!p_beval && آرگومانها->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    أرجع عدم;
}
#پایان_اگر

#إذا defined(FEAT_BEVAL_جمله)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    حرف *
did_مجموعه_balloonevalterm(optset_T *آرگومانها استفادهنشده)
{
    mch_bevalterm_تغییریافته();
    أرجع عدم;
}
#پایان_اگر

/*
 * Process the updated 'binary' option value.
 */
    حرف *
did_مجموعه_دودویی(optset_T *آرگومانها)
{
    // when 'bin' is set also set some other options
    مجموعه_options_bin(آرگومانها->os_oldval.boolean, curbuf->b_p_bin, آرگومانها->os_flags);
    redraw_titles();

    أرجع عدم;
}

/*
 * Process the updated 'buflisted' option value.
 */
    حرف *
did_مجموعه_buflisted(optset_T *آرگومانها)
{
    // when 'buflisted' changes, trigger autocommands
    إذا (آرگومانها->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						عدم, عدم, TRUE, curbuf);
    أرجع عدم;
}

/*
 * Process the new 'cmdheight' option value.
 */
    حرف *
did_مجموعه_cmdheight(optset_T *آرگومانها)
{
    دراز old_مقدار = آرگومانها->os_oldval.عدد;
    حرف *errmsg = عدم;

    // if p_ch changed value, change the command line height
    إذا (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_ch = MIN_CMDHEIGHT;
    }
    إذا (p_ch > ردیفها - min_ردیفها() + MIN_CMDHEIGHT)
	p_ch = ردیفها - min_ردیفها() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    إذا ((p_ch != old_مقدار
		|| tabline_ارتفاع() + topframe->fr_ارتفاع != ردیفها - p_ch)
	    && پر_screen
#اگر_تعریف_شده FEAT_GUI
	    && !gui.starting
#پایان_اگر
       )
	command_ارتفاع();

    أرجع errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    حرف *
did_مجموعه_compatible(optset_T *آرگومانها استفادهنشده)
{
    compatible_مجموعه();
    أرجع عدم;
}

#إذا defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    حرف *
did_مجموعه_conceallevel(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (curwin->w_p_cole < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	curwin->w_p_cole = 0;
    }
    وإلا إذا (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_آرگومان;
	curwin->w_p_cole = 3;
    }
    إذا (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    وإلا إذا (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_آرگومان;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    أرجع errmsg;
}
#پایان_اگر

#إذا defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    حرف *
did_مجموعه_diff(optset_T *آرگومانها استفادهنشده)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    إذا (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    أرجع عدم;
}
#پایان_اگر

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    حرف *
did_مجموعه_eof_eol_fixeol_bomb(optset_T *آرگومانها استفادهنشده)
{
    // redraw the window title and tab page text
    redraw_titles();
    أرجع عدم;
}

/*
 * Process the updated 'equalalways' option value.
 */
    حرف *
did_مجموعه_equalalways(optset_T *آرگومانها)
{
    إذا (p_ea && !آرگومانها->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    أرجع عدم;
}

#إذا defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    حرف *
did_مجموعه_foldcolumn(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (curwin->w_p_fdc < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	curwin->w_p_fdc = 0;
    }
    وإلا إذا (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_آرگومان;
	curwin->w_p_fdc = 12;
    }
    إذا (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    وإلا إذا (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_آرگومان;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    أرجع errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    حرف *
did_مجموعه_foldlevel(optset_T *آرگومانها استفادهنشده)
{
    إذا (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    أرجع عدم;
}

/*
 * Process the new 'foldminlines' option value.
 */
    حرف *
did_مجموعه_foldminlines(optset_T *آرگومانها استفادهنشده)
{
    foldUpdateAll(curwin);
    أرجع عدم;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    حرف *
did_مجموعه_foldnestmax(optset_T *آرگومانها استفادهنشده)
{
    إذا (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    أرجع عدم;
}
#پایان_اگر

#إذا defined(FEAT_جستجو_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    حرف *
did_مجموعه_hlsearch(optset_T *آرگومانها استفادهنشده)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    مجموعه_no_hlsearch(FALSE);
    أرجع عدم;
}
#پایان_اگر

/*
 * Process the updated 'ignorecase' option value.
 */
    حرف *
did_مجموعه_ignorecase(optset_T *آرگومانها استفادهنشده)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    إذا (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    أرجع عدم;
}

#إذا defined(HAVE_درون_داد_روش)
/*
 * Process the updated 'imdisable' option value.
 */
    حرف *
did_مجموعه_imdisable(optset_T *آرگومانها استفادهنشده)
{
    // Only de-activate it here, it will be enabled when changing mode.
    إذا (p_imdisable)
	im_مجموعه_فعال(FALSE);
    وإلا إذا (State & نما_درج)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_مجموعه_فعال(curbuf->b_p_iminsert == B_IMODE_IM);
    أرجع عدم;
}
#پایان_اگر

/*
 * Process the new 'iminsert' option value.
 */
    حرف *
did_مجموعه_iminsert(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_آخرین)
    {
	errmsg = e_invalid_آرگومان;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    إذا (termcap_فعال)	// don't do this in the alternate screen
	showmode();
#إذا defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    وضعیت_redraw_curbuf();
#پایان_اگر

    أرجع errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    حرف *
did_مجموعه_imsearch(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_آخرین)
    {
	errmsg = e_invalid_آرگومان;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    أرجع errmsg;
}

#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    حرف *
did_مجموعه_imstyle(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_آرگومان;

    أرجع errmsg;
}
#پایان_اگر

/*
 * Process the updated 'insertmode' option value.
 */
    حرف *
did_مجموعه_insertmode(optset_T *آرگومانها)
{
    // when 'insertmode' is set from an autocommand need to do work here
    إذا (p_im)
    {
	إذا ((State & نما_درج) == 0)
	    need_شروع_insertmode = TRUE;
	stop_درج_نما = FALSE;
    }
    // only reset if it was set previously
    وإلا إذا (آرگومانها->os_oldval.boolean)
    {
	need_شروع_insertmode = FALSE;
	stop_درج_نما = TRUE;
	إذا (restart_edit != 0 && نما_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    أرجع عدم;
}

#إذا defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    حرف *
did_مجموعه_langnoremap(optset_T *آرگومانها استفادهنشده)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    أرجع عدم;
}

/*
 * Process the updated 'langremap' option value.
 */
    حرف *
did_مجموعه_langremap(optset_T *آرگومانها استفادهنشده)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    أرجع عدم;
}
#پایان_اگر

/*
 * Process the new 'laststatus' option value.
 */
    حرف *
did_مجموعه_laststatus(optset_T *آرگومانها استفادهنشده)
{
    آخرین_وضعیت(FALSE);	// (re)set last window status line
    أرجع عدم;
}

#إذا defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    حرف *
did_مجموعه_linespace(optset_T *آرگومانها استفادهنشده)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    إذا (gui.درون_use && gui_mch_adjust_charheight() == OK)
	gui_مجموعه_shellsize(FALSE, FALSE, RESIZE_VERT);
    أرجع عدم;
}
#پایان_اگر

/*
 * Process the updated 'lisp' option value.
 */
    حرف *
did_مجموعه_lisp(optset_T *آرگومانها استفادهنشده)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (فراغ)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    أرجع عدم;
}

/*
 * Process the new 'maxcombine' option value.
 */
    حرف *
did_مجموعه_maxcombine(optset_T *آرگومانها استفادهنشده)
{
    إذا (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    وإلا إذا (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    أرجع عدم;
}

/*
 * Process the updated 'modifiable' option value.
 */
    حرف *
did_مجموعه_modifiable(optset_T *آرگومانها استفادهنشده)
{
    // when 'modifiable' is changed, redraw the window title

#اگر_تعریف_شده FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    إذا (curbuf->b_p_ma && (جمله_درون_نرمال_نما(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_جمله != عدم && !جمله_is_پایانیافته(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	أرجع e_cannot_make_terminal_with_running_کار_modifiable;
    }
#پایان_اگر
    redraw_titles();

    أرجع عدم;
}

/*
 * Process the updated 'modified' option value.
 */
    حرف *
did_مجموعه_modified(optset_T *آرگومانها)
{
    إذا (!آرگومانها->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_مجموعه = !!آرگومانها->os_newval.boolean;
    أرجع عدم;
}

#إذا defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    حرف *
did_مجموعه_mousehide(optset_T *آرگومانها استفادهنشده)
{
    إذا (!p_mh)
	gui_mch_mousehide(FALSE);
    أرجع عدم;
}
#پایان_اگر

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    حرف *
did_مجموعه_عدد_relativenumber(optset_T *آرگومانها استفادهنشده)
{
#إذا defined(FEAT_SIGNS) && defined(FEAT_GUI)
    إذا (gui.درون_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != عدم)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	إذا (!(curwin->w_p_nu && ((صحيح *)آرگومانها->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#پایان_اگر
    أرجع عدم;
}

#إذا defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    حرف *
did_مجموعه_numberwidth(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    // 'numberwidth' must be positive
    إذا (curwin->w_p_nuw < 1)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	curwin->w_p_nuw = 1;
    }
    وإلا إذا (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_آرگومان;
	curwin->w_p_nuw = 20;
    }
    إذا (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    وإلا إذا (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_آرگومان;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_سطر_شمار = 0; // trigger a redraw

    أرجع errmsg;
}
#پایان_اگر

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    حرف *
did_مجموعه_osctimeoutlen(optset_T *آرگومانها)
{
    إذا (p_ost < 0)
    {
	p_ost = آرگومانها->os_oldval.عدد;
	أرجع e_آرگومان_must_be_مثبت;
    }

    أرجع عدم;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    حرف *
did_مجموعه_paste(optset_T *آرگومانها استفادهنشده)
{
    ساكن صحيح	old_p_paste = FALSE;
    ساكن صحيح	save_sm = 0;
    ساكن صحيح	save_sta = 0;
    ساكن صحيح	save_ru = 0;
#اگر_تعریف_شده FEAT_RIGHTLEFT
    ساكن صحيح	save_ri = 0;
    ساكن صحيح	save_hkmap = 0;
#پایان_اگر
    buf_T	*buf;

    إذا (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	إذا (!old_p_paste)
	{
	    // save options for each buffer
	    FOR_ALL_BUFFERS(buf)
	    {
		buf->b_p_tw_nopaste = buf->b_p_tw;
		buf->b_p_wm_nopaste = buf->b_p_wm;
		buf->b_p_sts_nopaste = buf->b_p_sts;
		buf->b_p_ai_nopaste = buf->b_p_ai;
		buf->b_p_et_nopaste = buf->b_p_et;
#اگر_تعریف_شده FEAT_VARTABS
		إذا (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != خالی_option
					   ? vim_strsave(buf->b_p_vsts) : عدم;
#پایان_اگر
	    }

	    // save global options
	    save_sm = p_sm;
	    save_sta = p_sta;
	    save_ru = p_ru;
#اگر_تعریف_شده FEAT_RIGHTLEFT
	    save_ri = p_ri;
	    save_hkmap = p_hkmap;
#پایان_اگر
	    // save global values for local buffer options
	    p_ai_nopaste = p_ai;
	    p_et_nopaste = p_et;
	    p_sts_nopaste = p_sts;
	    p_tw_nopaste = p_tw;
	    p_wm_nopaste = p_wm;
#اگر_تعریف_شده FEAT_VARTABS
	    إذا (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != خالی_option
						  ? vim_strsave(p_vsts) : عدم;
#پایان_اگر
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
#اگر_تعریف_شده FEAT_VARTABS
	    إذا (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = خالی_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#پایان_اگر
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	إذا (p_ru)
	    وضعیت_redraw_all();    // redraw to remove the ruler
	p_ru = 0;		    // no ruler
#اگر_تعریف_شده FEAT_RIGHTLEFT
	p_ri = 0;		    // no reverse insert
	p_hkmap = 0;		    // no Hebrew keyboard
#پایان_اگر
	// set global values for local buffer options
	p_tw = 0;
	p_wm = 0;
	p_sts = 0;
	p_ai = 0;
	p_et = 0;
#اگر_تعریف_شده FEAT_VARTABS
	إذا (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = خالی_option;
#پایان_اگر
    }

    // Paste switched from on to off: Restore saved values.
    وإلا إذا (old_p_paste)
    {
	// restore options for each buffer
	FOR_ALL_BUFFERS(buf)
	{
	    buf->b_p_tw = buf->b_p_tw_nopaste;
	    buf->b_p_wm = buf->b_p_wm_nopaste;
	    buf->b_p_sts = buf->b_p_sts_nopaste;
	    buf->b_p_ai = buf->b_p_ai_nopaste;
	    buf->b_p_et = buf->b_p_et_nopaste;
#اگر_تعریف_شده FEAT_VARTABS
	    إذا (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : خالی_option;
	    vim_free(buf->b_p_vsts_array);
	    إذا (buf->b_p_vsts && buf->b_p_vsts != خالی_option)
		(فراغ)tabstop_مجموعه(buf->b_p_vsts, &buf->b_p_vsts_array);
	    وإلا
		buf->b_p_vsts_array = عدم;
#پایان_اگر
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	إذا (p_ru != save_ru)
	    وضعیت_redraw_all();    // redraw to draw the ruler
	p_ru = save_ru;
#اگر_تعریف_شده FEAT_RIGHTLEFT
	p_ri = save_ri;
	p_hkmap = save_hkmap;
#پایان_اگر
	// set global values for local buffer options
	p_ai = p_ai_nopaste;
	p_et = p_et_nopaste;
	p_sts = p_sts_nopaste;
	p_tw = p_tw_nopaste;
	p_wm = p_wm_nopaste;
#اگر_تعریف_شده FEAT_VARTABS
	إذا (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : خالی_option;
#پایان_اگر
    }

    old_p_paste = p_paste;

#إذا defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#پایان_اگر

    أرجع عدم;
}


#اگر_تعریف_شده FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    حرف *
did_مجموعه_previewwindow(optset_T *آرگومانها استفادهنشده)
{
    إذا (!curwin->w_p_pvw)
	أرجع عدم;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	إذا (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    أرجع e_preview_پنجره_already_exists;
	}

    أرجع عدم;
}
#پایان_اگر

#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    حرف *
did_مجموعه_pyxversion(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_آرگومان;

    أرجع errmsg;
}
#پایان_اگر

/*
 * Process the updated 'readonly' option value.
 */
    حرف *
did_مجموعه_readonly(optset_T *آرگومانها)
{
    // when 'readonly' is reset globally, also reset readonlymode
    إذا (!curbuf->b_p_ro && (آرگومانها->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    إذا (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    أرجع عدم;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    حرف *
did_مجموعه_scrollbind(optset_T *آرگومانها استفادهنشده)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    إذا (!curwin->w_p_scb)
	أرجع عدم;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    أرجع عدم;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    حرف *
did_مجموعه_maxsearchcount(optset_T *آرگومانها استفادهنشده)
{
    حرف	*errmsg = عدم;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#تعریف_کن MAX_جستجو_شمار 9999

    إذا (p_msc <= 0)
	errmsg = e_آرگومان_must_be_مثبت;
    وإلا إذا (p_msc > MAX_جستجو_شمار)
	errmsg = e_invalid_آرگومان;

    إذا (errmsg != عدم)
	p_msc = 99;

    أرجع errmsg;
#لغو_تعریف MAX_جستجو_شمار
}

#إذا defined(BACKSLASH_درون_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    حرف *
did_مجموعه_shellslash(optset_T *آرگومانها استفادهنشده)
{
    إذا (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    وإلا
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
    أرجع عدم;
}
#پایان_اگر

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    حرف *
did_مجموعه_shiftwidth_tabstop(optset_T *آرگومانها)
{
    دراز	*pp = (دراز *)آرگومانها->os_varp;
    حرف	*errmsg = عدم;

    إذا (curbuf->b_p_ts <= 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	curbuf->b_p_ts = 8;
    }
    وإلا إذا (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_آرگومان;
	curbuf->b_p_ts = 8;
    }
    إذا (p_ts <= 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_ts = 8;
    }
    وإلا إذا (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_آرگومان;
	p_ts = 8;
    }

    إذا (curbuf->b_p_sw < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
#اگر_تعریف_شده FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_شمار(curbuf->b_p_vts_array) > 0
		       ? tabstop_اولی(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#وإلا
	curbuf->b_p_sw = curbuf->b_p_ts;
#پایان_اگر
    }
    إذا (p_sw < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
#اگر_تعریف_شده FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_شمار(curbuf->b_p_vts_array) > 0
	     ? tabstop_اولی(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#وإلا
	p_sw = curbuf->b_p_ts;
#پایان_اگر
    }

#اگر_تعریف_شده FEAT_FOLDING
    إذا (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#پایان_اگر
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    إذا (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    أرجع errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    حرف *
did_مجموعه_showtabline(optset_T *آرگومانها استفادهنشده)
{
    shell_new_ردیفها();	// recompute window positions and heights
    أرجع عدم;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    حرف *
did_مجموعه_smoothscroll(optset_T *آرگومانها استفادهنشده)
{
    إذا (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    أرجع عدم;
}

#إذا defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    حرف *
did_مجموعه_spell(optset_T *آرگومانها استفادهنشده)
{
    إذا (curwin->w_p_spell)
	أرجع parse_spelllang(curwin);

    أرجع عدم;
}
#پایان_اگر

/*
 * Process the updated 'swapfile' option value.
 */
    حرف *
did_مجموعه_swapfile(optset_T *آرگومانها استفادهنشده)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    إذا (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    وإلا
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    أرجع عدم;
}

#إذا defined(FEAT_TERMGUICOLORS)
    حرف *
did_مجموعه_termguicolors(optset_T *آرگومانها استفادهنشده)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    إذا (
#  ifdef VIMDLL
	    !gui.درون_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_مجموعه = TRUE;
	p_tgc = 0;
	أرجع e_24_بیت_colors_are_not_supported_on_this_environment;
    }
    إذا (is_جمله_win32())
	جابجا_tcap();
# endif
# ifdef FEAT_GUI
    إذا (!gui.درون_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    إذا (is_جمله_win32())
    {
	control_console_color_rgb();
	مجموعه_termname(T_NAME);
	init_highlight(TRUE, FALSE);
    }
# endif
# ifdef FEAT_TERMINAL
    جمله_update_colors_all();
    جمله_update_palette_all();
    جمله_update_hlfwin_all();
# endif
    p_tgc_مجموعه = TRUE;

    أرجع عدم;
}
#پایان_اگر

    حرف *
did_مجموعه_termsync(optset_T *آرگومانها استفادهنشده)
{
    إذا (!p_tsy)
	جمله_مجموعه_sync_output(جمله_SYNC_OUTPUT_OFF);
    أرجع عدم;
}

#إذا defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    حرف *
did_مجموعه_termwinscroll(optset_T *آرگومانها)
{
    دراز	*pp = (دراز *)آرگومانها->os_varp;
    حرف	*errmsg = عدم;

    إذا (*pp < 1)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	*pp = 1;
    }

    أرجع errmsg;
}
#پایان_اگر

/*
 * Process the updated 'terse' option value.
 */
    حرف *
did_مجموعه_terse(optset_T *آرگومانها استفادهنشده)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_جستجو);

    // insert 's' in p_shm
    إذا (p_terse && p == عدم)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	مجموعه_string_option_مستقیم((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    وإلا إذا (!p_terse && p != عدم)
	STRMOVE(p, p + 1);
    أرجع عدم;
}

/*
 * Process the updated 'textauto' option value.
 */
    حرف *
did_مجموعه_textauto(optset_T *آرگومانها)
{
    // when 'textauto' is set or reset also change 'fileformats'
    مجموعه_string_option_مستقیم((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | آرگومانها->os_flags, 0);

    أرجع عدم;
}

/*
 * Process the updated 'textmode' option value.
 */
    حرف *
did_مجموعه_textmode(optset_T *آرگومانها)
{
    // when 'textmode' is set or reset also change 'fileformat'
    مجموعه_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, آرگومانها->os_flags);

    أرجع عدم;
}

/*
 * Process the new 'textwidth' option value.
 */
    حرف *
did_مجموعه_textwidth(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (curbuf->b_p_tw < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	curbuf->b_p_tw = 0;
    }
    إذا (p_tw < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_tw = 0;
    }
#اگر_تعریف_شده FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(عدم, wp);
    }
#پایان_اگر

    أرجع errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    حرف *
did_مجموعه_title_icon(optset_T *آرگومانها استفادهنشده)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_مجموعه_title();
    أرجع عدم;
}

/*
 * Process the new 'titlelen' option value.
 */
    حرف *
did_مجموعه_titlelen(optset_T *آرگومانها)
{
    دراز old_مقدار = آرگومانها->os_oldval.عدد;
    حرف *errmsg = عدم;

    // if 'titlelen' has changed, redraw the title
    إذا (p_titlelen < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_titlelen = 85;
    }
    إذا (starting != NO_SCREEN && old_مقدار != p_titlelen)
	need_maketitle = TRUE;

    أرجع errmsg;
}

#إذا defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    حرف *
did_مجموعه_undofile(optset_T *آرگومانها)
{
    // Only take action when the option was set.
    إذا (!curbuf->b_p_udf && !p_udf)
	أرجع عدم;

    // When reset we do not delete the undo file, the option may be set again
    // without making any changes in between.
    char_u	hash[UNDO_HASH_اندازه];
    buf_T	*save_curbuf = curbuf;

    FOR_ALL_BUFFERS(curbuf)
    {
	// When 'undofile' is set globally: for every buffer, otherwise
	// only for the current buffer: Try to read in the undofile,
	// if one exists, the buffer wasn't changed and the buffer was
	// loaded
	إذا ((curbuf == save_curbuf
		    || (آرگومانها->os_flags & OPT_GLOBAL)
		    || آرگومانها->os_flags == 0)
		&& !curbuf_Is_تغییریافته() && curbuf->b_ml.ml_mfp != عدم)
	{
# ifdef FEAT_CRYPT
	    إذا (crypt_روش_is_sodium(crypt_get_روش_nr(curbuf)))
		استمر;
# endif
	    u_compute_hash(hash);
	    u_read_undo(عدم, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    أرجع عدم;
}
#پایان_اگر

/*
 * Process the new global 'undolevels' option value.
 */
    ساكن فراغ
update_global_undolevels(دراز مقدار, دراز old_مقدار)
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
    ساكن فراغ
update_buflocal_undolevels(دراز مقدار, دراز old_مقدار)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_مقدار;
    u_sync(TRUE);
    curbuf->b_p_ul = مقدار;
}

/*
 * Process the new 'undolevels' option value.
 */
    حرف *
did_مجموعه_undolevels(optset_T *آرگومانها)
{
    دراز *pp = (دراز *)آرگومانها->os_varp;

    إذا (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(آرگومانها->os_newval.عدد,
						       آرگومانها->os_oldval.عدد);
    وإلا إذا (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(آرگومانها->os_newval.عدد,
						       آرگومانها->os_oldval.عدد);

    أرجع عدم;
}

/*
 * Process the new 'updatecount' option value.
 */
    حرف *
did_مجموعه_updatecount(optset_T *آرگومانها)
{
    دراز old_مقدار = آرگومانها->os_oldval.عدد;
    حرف *errmsg = عدم;

    // when 'updatecount' changes from zero to non-zero, open swap files
    إذا (p_uc < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_uc = 100;
    }
    إذا (p_uc && !old_مقدار)
	ml_open_files();

    أرجع errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    حرف *
did_مجموعه_weirdinvert(optset_T *آرگومانها)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    إذا (p_wiv && !آرگومانها->os_oldval.boolean)
	T_XS = (char_u *)"y";
    وإلا إذا (!p_wiv && آرگومانها->os_oldval.boolean)
	T_XS = خالی_option;
    p_wiv = (*T_XS != NUL);

    أرجع عدم;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    حرف *
did_مجموعه_wildchar(optset_T *آرگومانها)
{
    دراز c = *(دراز *)آرگومانها->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    إذا (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

/*
 * Process the new 'window' option value.
 */
    حرف *
did_مجموعه_پنجره(optset_T *آرگومانها استفادهنشده)
{
    إذا (p_پنجره < 1)
	p_پنجره = 1;
    وإلا إذا (p_پنجره >= ردیفها)
	p_پنجره = ردیفها - 1;
    أرجع عدم;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    حرف *
did_مجموعه_winheight_helpheight(optset_T *آرگومانها)
{
    دراز	*pp = (دراز *)آرگومانها->os_varp;
    حرف	*errmsg = عدم;

    إذا (p_wh < 1)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_wh = 1;
    }
    إذا (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    إذا (p_hh < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_hh = 0;
    }

    // Change window height NOW
    إذا (!ONE_پنجره)
    {
	إذا (pp == &p_wh && curwin->w_ارتفاع < p_wh)
	    win_setheight((صحيح)p_wh);
	إذا (pp == &p_hh && curbuf->b_help && curwin->w_ارتفاع < p_hh)
	    win_setheight((صحيح)p_hh);
    }

    أرجع errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    حرف *
did_مجموعه_winminheight(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (p_wmh < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_wmh = 0;
    }
    إذا (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    أرجع errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    حرف *
did_مجموعه_winminwidth(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (p_wmw < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_wmw = 0;
    }
    إذا (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    أرجع errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    حرف *
did_مجموعه_winwidth(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (p_wiw < 1)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_wiw = 1;
    }
    إذا (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    إذا (!ONE_پنجره && curwin->w_عرض < p_wiw)
	win_setwidth((صحيح)p_wiw);

    أرجع errmsg;
}

#إذا defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    حرف *
did_مجموعه_wlsteal(optset_T *آرگومانها استفادهنشده)
{
    clip_reset_wayland();

    أرجع عدم;
}
#پایان_اگر

#إذا defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    حرف *
did_مجموعه_wltimeoutlen(optset_T *آرگومانها)
{
    إذا (p_wtm < 0)
    {
	p_wtm = آرگومانها->os_oldval.عدد;
	أرجع e_آرگومان_must_be_مثبت;
    }

    أرجع عدم;
}
#پایان_اگر

/*
 * Process the updated 'wrap' option value.
 */
    حرف *
did_مجموعه_wrap(optset_T *آرگومانها استفادهنشده)
{
    // Set w_leftcol or w_skipcol to zero.
    إذا (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    وإلا
	curwin->w_skipcol = 0;

    أرجع عدم;
}

#اگر_تعریف_شده FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    حرف *
did_مجموعه_xhistory(optset_T *آرگومانها)
{
    صحيح is_p_chi = (دراز*)آرگومانها->os_varp == &p_chi;
    صحيح err;
    دراز *arg = (is_p_chi) ? &p_chi :(دراز*)آرگومانها->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    إذا (*arg < 1)
    {
	*arg = آرگومانها->os_oldval.عدد;
	أرجع e_cannot_have_negative_or_zero_عدد_از_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    إذا (*arg > 100)
    {
	*arg = آرگومانها->os_oldval.عدد;
	أرجع e_cannot_have_بیشتر_than_hundred_quickfix;
    }

    إذا (is_p_chi)
	err = qf_resize_پشته(*arg);
    وإلا
	err = ll_resize_پشته(curwin, *arg);

    إذا (err == FAIL)
	أرجع e_failed_resizing_quickfix_پشته;

    أرجع عدم;
}
#پایان_اگر

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ساكن حرف *
مجموعه_bool_option(
    صحيح		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    صحيح		مقدار,			// new value
    صحيح		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    صحيح		old_مقدار = *(صحيح *)varp;
#إذا defined(FEAT_EVAL)
    صحيح		old_global_مقدار = 0;
#پایان_اگر
    حرف	*errmsg = عدم;

    // Disallow changing some options from secure mode
    إذا ((secure
#اگر_تعریف_شده HAVE_SANDBOX
		|| sandbox != 0
#پایان_اگر
		) && (options[opt_idx].flags & P_SECURE))
	أرجع e_not_allowed_اینجا;

#إذا defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_مقدار = *(صحيح *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#پایان_اگر

    *(صحيح *)varp = مقدار;	    // set the new value
#اگر_تعریف_شده FEAT_EVAL
    // Remember where the option was set.
    مجموعه_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI
    need_mouse_correct = TRUE;
#پایان_اگر

    // May set global value for local option.
    إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(صحيح *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = مقدار;

    // Handle side effects of changing a bool option.
    إذا (options[opt_idx].opt_did_مجموعه_cb != عدم)
    {
	optset_T آرگومانها;

	CLEAR_فیلد(آرگومانها);
	آرگومانها.os_varp = varp;
	آرگومانها.os_flags = opt_flags;
	آرگومانها.os_oldval.boolean = old_مقدار;
	آرگومانها.os_newval.boolean = مقدار;
	آرگومانها.os_errbuf = عدم;
	errmsg = options[opt_idx].opt_did_مجموعه_cb(&آرگومانها);
	إذا (errmsg != عدم)
	    أرجع errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_مجموعه;

#إذا defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(دراز)(old_مقدار ? TRUE : FALSE),
				(دراز)(old_global_مقدار ? TRUE : FALSE),
				(دراز)(مقدار ? TRUE : FALSE), عدم);
#پایان_اگر

    comp_ستون();			    // in case 'ruler' or 'showcmd' changed

    إذا (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_مجموعه_curswant = true;

    إذا ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    أرجع errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    ساكن حرف *
check_رقم_option_bounds(
    دراز	*pp,
    دراز	old_مقدار,
    دراز	old_ردیفها_2,
    دراز	old_Columns,
    حرف	*errbuf,
    size_t	errbuflen,
    حرف	*errmsg)
{
    إذا (ردیفها < min_ردیفها_for_all_tabpages() && پر_screen)
    {
	إذا (errbuf != عدم)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_کمترین_nr_سطرها), min_ردیفها_for_all_tabpages());
	    errmsg = errbuf;
	}
	ردیفها = min_ردیفها_for_all_tabpages();
    }
    إذا (Columns < MIN_COLUMNS && پر_screen)
    {
	إذا (errbuf != عدم)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_کمترین_nr_columns), MIN_COLUMNS);
	    errmsg = errbuf;
	}
	Columns = MIN_COLUMNS;
    }
    حد_screen_اندازه();

    // If the screen (shell) height has been changed, assume it is the
    // physical screenheight.
    إذا (old_ردیفها_2 != ردیفها || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	إذا (updating_screen)
	    *pp = old_مقدار;
	وإلا إذا (پر_screen
#اگر_تعریف_شده FEAT_GUI
		&& !gui.starting
#پایان_اگر
		)
	    مجموعه_shellsize((صحيح)Columns, (صحيح)ردیفها, TRUE);
	وإلا
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    إذا (cmdline_ردیف > ردیفها - p_ch && ردیفها > p_ch)
		cmdline_ردیف = ردیفها - p_ch;
	}
	إذا (p_پنجره >= ردیفها || !option_was_مجموعه((char_u *)"window"))
	    p_پنجره = ردیفها - 1;
    }

    إذا (p_tm < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_tm = 0;
    }
    إذا ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_ارتفاع
		    && curwin->w_ارتفاع > 0))
	    && پر_screen)
    {
	إذا (pp == &(curwin->w_p_scr))
	{
	    إذا (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_اندازه;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	وإلا إذا (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	وإلا // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_ارتفاع;
    }
    إذا (p_hi < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_hi = 0;
    }
    وإلا إذا (p_hi > 10000)
    {
	errmsg = e_invalid_آرگومان;
	p_hi = 10000;
    }
    إذا (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_آرگومان;
	p_re = 0;
    }
    إذا (p_report < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_report = 1;
    }
    إذا ((p_sj < -100 || p_sj >= ردیفها) && پر_screen)
    {
	إذا (ردیفها != old_ردیفها_2)	// Rows changed, just adjust p_sj
	    p_sj = ردیفها / 2;
	وإلا
	{
	    errmsg = e_invalid_scroll_اندازه;
	    p_sj = 1;
	}
    }
    إذا (p_so < 0 && پر_screen)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_so = 0;
    }
    إذا (p_sop < 0 && پر_screen)
    {
	errmsg = e_invalid_آرگومان;
	p_sop = 0;
    }
    إذا (p_siso < 0 && پر_screen)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_siso = 0;
    }
    إذا (p_cwh < 1)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_cwh = 1;
    }
    إذا (p_ut < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_ut = 2000;
    }
    إذا (p_ss < 0)
    {
	errmsg = e_آرگومان_must_be_مثبت;
	p_ss = 0;
    }

    أرجع errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ساكن حرف *
مجموعه_رقم_option(
    صحيح		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    دراز	مقدار,			// new value
    حرف	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    صحيح		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    حرف	*errmsg = عدم;
    دراز	old_مقدار = *(دراز *)varp;
#إذا defined(FEAT_EVAL)
    دراز	old_global_مقدار = 0;	// only used when setting a local and
					// global option
#پایان_اگر
    دراز	old_ردیفها_2 = ردیفها;	// remember old Rows
    دراز	old_Columns = Columns;	// remember old Columns
    دراز	*pp = (دراز *)varp;

    // Disallow changing some options from secure mode.
    إذا ((secure
#اگر_تعریف_شده HAVE_SANDBOX
		|| sandbox != 0
#پایان_اگر
		) && (options[opt_idx].flags & P_SECURE))
	أرجع e_not_allowed_اینجا;

#إذا defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_مقدار = *(دراز *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#پایان_اگر

    *pp = مقدار;
#اگر_تعریف_شده FEAT_EVAL
    // Remember where the option was set.
    مجموعه_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI
    need_mouse_correct = TRUE;
#پایان_اگر

    // May set global value for local option.
    إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(دراز *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = مقدار;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    إذا (options[opt_idx].opt_did_مجموعه_cb != عدم)
    {
	optset_T آرگومانها;

	CLEAR_فیلد(آرگومانها);
	آرگومانها.os_varp = varp;
	آرگومانها.os_flags = opt_flags;
	آرگومانها.os_oldval.عدد = old_مقدار;
	آرگومانها.os_newval.عدد = مقدار;
	آرگومانها.os_errbuf = عدم;
	errmsg = options[opt_idx].opt_did_مجموعه_cb(&آرگومانها);
    }

    // Check the bounds for numeric options here
    errmsg = check_رقم_option_bounds(pp, old_مقدار, old_ردیفها_2, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_مجموعه;

#إذا defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_مقدار, old_global_مقدار,
								مقدار, errmsg);
#پایان_اگر

    comp_ستون();			    // in case 'columns' or 'ls' changed

    إذا (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_مجموعه_curswant = true;

    إذا ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    أرجع errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    فراغ
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    صحيح		doclear = (flags & P_RCLR) == P_RCLR;
    صحيح		all = ((flags & P_RALL) == P_RALL || doclear);

    إذا ((flags & P_RSTAT) || all)	// mark all status lines dirty
	وضعیت_redraw_all();

    إذا ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	إذا (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	وإلا
	    تغییریافته_پنجره_setting();
    }
    إذا (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    إذا (doclear)
	redraw_all_later(UPD_CLEAR);
    وإلا إذا (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    صحيح
findoption(char_u *arg)
{
    صحيح		    opt_idx;
    حرف	    *s, *p;
    ساكن کوتاه    سریع_tab[27] = {0, 0};	// quick access table
    صحيح		    is_جمله_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    إذا (سریع_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	صحيح آخرین_opt_idx;
	لكل (آخرین_opt_idx = 0; options[آخرین_opt_idx].fullname != عدم;
		آخرین_opt_idx++)
	    ;
	لكل (صحيح tab_idx = 1; tab_idx < 27; tab_idx++)
	    سریع_tab[tab_idx] = آخرین_opt_idx;

	p = options[0].fullname;
	لكل (opt_idx = 1; opt_idx < آخرین_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    إذا (s[0] != p[0])
	    {
		إذا (s[0] == 't' && s[1] == '_')
		    سریع_tab[26] = opt_idx;
		وإلا
		    سریع_tab[Char_Ord_کم(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    إذا (arg[0] < 'a' || arg[0] > 'z')
	أرجع -1;

    is_جمله_opt = (arg[0] == 't' && arg[1] == '_');
    إذا (is_جمله_opt)
	opt_idx = سریع_tab[26];
    وإلا
	opt_idx = سریع_tab[Char_Ord_کم(arg[0])];
    لكل (; (s = options[opt_idx].fullname) != عدم && s[0] == arg[0]; opt_idx++)
    {
	إذا (STRCMP(arg, s) == 0)		    // match full name
	    اكسر;
    }
    إذا (s != عدم && s[0] != arg[0])
	s = عدم;
    إذا (s == عدم && !is_جمله_opt)
    {
	opt_idx = سریع_tab[Char_Ord_کم(arg[0])];
	لكل ( ; options[opt_idx].fullname != عدم; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    إذا (s != عدم && STRCMP(arg, s) == 0)   // match short name
		اكسر;
	    s = عدم;
	}
    }
    إذا (s == عدم)
	opt_idx = -1;
    أرجع opt_idx;
}

#إذا defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
    دراز	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    صحيح		*flagsp,
    صحيح		scope)
{
    صحيح		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    إذا (opt_idx < 0)		    // option not in the table
    {
	صحيح کلید;

	إذا (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (کلید = پیدا_کن_کلید_option(name, FALSE)) != 0)
	{
	    char_u کلید_name[2];
	    char_u *p;

	    إذا (flagsp != عدم)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    إذا (کلید < 0)
	    {
		کلید_name[0] = KEY2TERMCAP0(کلید);
		کلید_name[1] = KEY2TERMCAP1(کلید);
	    }
	    وإلا
	    {
		کلید_name[0] = KS_کلید;
		کلید_name[1] = (کلید & 0xff);
	    }
	    p = پیدا_کن_termcode(کلید_name);
	    إذا (p != عدم)
	    {
		إذا (stringval != عدم)
		    *stringval = vim_strsave(p);
		أرجع gov_string;
	    }
	}
	أرجع gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    إذا (flagsp != عدم)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    إذا (options[opt_idx].flags & P_STRING)
    {
	إذا (varp == عدم)		    // hidden option
	    أرجع gov_hidden_string;
	إذا (stringval != عدم)
	{
	    إذا ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    وإلا إذا ((char_u **)varp == &curbuf->b_p_کلید
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    وإلا
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	أرجع gov_string;
    }

    إذا (varp == عدم)		    // hidden option
	أرجع (options[opt_idx].flags & P_رقم)
					 ? gov_hidden_عدد : gov_hidden_bool;
    إذا (options[opt_idx].flags & P_رقم)
	*numval = *(دراز *)varp;
    وإلا
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	إذا ((صحيح *)varp == &curbuf->b_تغییریافته)
	    *numval = curbuf_Is_تغییریافته();
	وإلا
	    *numval = (دراز) *(صحيح *)varp;
    }
    أرجع (options[opt_idx].flags & P_رقم) ? gov_عدد : gov_bool;
}
#پایان_اگر

#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    صحيح
get_option_مقدار_strict(
    char_u	*name,
    دراز	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    صحيح		opt_type,
    فراغ	*from)
{
    صحيح		opt_idx;
    char_u	*varp = عدم;
    بنية vimoption *p;
    صحيح		r = 0;

    opt_idx = findoption(name);
    إذا (opt_idx < 0)
	أرجع 0;

    p = &(options[opt_idx]);

    // Hidden option
    إذا (p->var == عدم)
	أرجع 0;

    إذا (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    وإلا إذا (p->flags & P_رقم)
	r |= SOPT_رقم;
    وإلا إذا (p->flags & P_STRING)
	r |= SOPT_STRING;

    إذا (p->indir == PV_NONE)
    {
	إذا (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	وإلا
	    أرجع 0; // Did not request global-only option
    }
    وإلا
    {
	إذا (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	وإلا إذا (opt_type == SREQ_GLOBAL)
	    أرجع 0; // Requested global option

	إذا (p->indir & PV_WIN)
	{
	    إذا (opt_type == SREQ_BUF)
		أرجع 0; // Did not request window-local option
	    وإلا
		r |= SOPT_WIN;
	}
	وإلا إذا (p->indir & PV_BUF)
	{
	    إذا (opt_type == SREQ_WIN)
		أرجع 0; // Did not request buffer-local option
	    وإلا
		r |= SOPT_BUF;
	}
    }

    إذا (stringval == عدم)
	أرجع r;

    إذا (opt_type == SREQ_GLOBAL)
	varp = p->var;
    وإلا
    {
	إذا (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    إذا (p->indir == PV_MOD)
	    {
		*numval = buf_Is_تغییریافته((buf_T *)from);
		varp = عدم;
	    }
# ifdef FEAT_CRYPT
	    وإلا إذا (p->indir == PV_کلید)
	    {
		// never return the value of the crypt key
		*stringval = عدم;
		varp = عدم;
	    }
# endif
	    وإلا
	    {
		buf_T *save_curbuf = curbuf;

		// only getting a pointer, no need to use aucmd_prepbuf()
		curbuf = (buf_T *)from;
		curwin->w_میانگیر = curbuf;
		varp = get_varp(p);
		curbuf = save_curbuf;
		curwin->w_میانگیر = curbuf;
	    }
	}
	وإلا إذا (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_میانگیر;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_میانگیر;
	}
	إذا (varp == p->var)
	    أرجع (r | SOPT_UNSET);
    }

    إذا (varp != عدم)
    {
	إذا (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	وإلا إذا (p->flags & P_رقم)
	    *numval = *(دراز *) varp;
	وإلا
	    *numval = *(صحيح *)varp;
    }

    أرجع r;
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
option_iter_بعدی(فراغ **option, صحيح opt_type)
{
    بنية vimoption	*ret = عدم;
    افعل
    {
	إذا (*option == عدم)
	    *option = (فراغ *) options;
	وإلا إذا (((بنية vimoption *) (*option))->fullname == عدم)
	{
	    *option = عدم;
	    أرجع عدم;
	}
	وإلا
	    *option = (فراغ *) (((بنية vimoption *) (*option)) + 1);

	ret = ((بنية vimoption *) (*option));

	// Hidden option
	إذا (ret->var == عدم)
	{
	    ret = عدم;
	    استمر;
	}

	بدل (opt_type)
	{
	    حالة SREQ_GLOBAL:
		إذا (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = عدم;
		اكسر;
	    حالة SREQ_BUF:
		إذا (!(ret->indir & PV_BUF))
		    ret = عدم;
		اكسر;
	    حالة SREQ_WIN:
		إذا (!(ret->indir & PV_WIN))
		    ret = عدم;
		اكسر;
	    افتراضي:
		داخلی_error("option_iter_next()");
		أرجع عدم;
	}
    }
    طالما (ret == عدم);

    أرجع (char_u *)ret->fullname;
}
#پایان_اگر

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(صحيح opt_idx)
{
    أرجع options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    فراغ
مجموعه_option_flag(صحيح opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    صحيح
is_global_option(صحيح opt_idx)
{
    أرجع options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    صحيح
is_global_local_option(صحيح opt_idx)
{
    أرجع options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    صحيح
is_پنجره_local_option(صحيح opt_idx)
{
    أرجع options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    صحيح
is_hidden_option(صحيح opt_idx)
{
    أرجع options[opt_idx].var == عدم;
}

#إذا defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    صحيح
is_crypt_کلید_option(صحيح opt_idx)
{
    أرجع options[opt_idx].indir == PV_کلید;
}
#پایان_اگر

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    حرف *
مجموعه_option_مقدار(
    char_u	*name,
    دراز	عدد,
    char_u	*string,
    صحيح		opt_flags)	// OPT_LOCAL or 0 (both)
{
    صحيح		opt_idx;
    char_u	*varp;
    long_u	flags;
    ساكن حرف	errbuf[ERR_BUFLEN];
    صحيح		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    إذا (opt_idx < 0)
    {
	صحيح کلید;

	إذا (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (کلید = پیدا_کن_کلید_option(name, FALSE)) != 0)
	{
	    char_u کلید_name[2];

	    إذا (کلید < 0)
	    {
		کلید_name[0] = KEY2TERMCAP0(کلید);
		کلید_name[1] = KEY2TERMCAP1(کلید);
	    }
	    وإلا
	    {
		کلید_name[0] = KS_کلید;
		کلید_name[1] = (کلید & 0xff);
	    }
	    جمع_کن_termcode(کلید_name, string, FALSE);
	    إذا (پر_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    أرجع عدم;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    وإلا
    {
	flags = options[opt_idx].flags;
#اگر_تعریف_شده HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	إذا (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_درون_sandbox));
	    أرجع عدم;
	}
#پایان_اگر
	إذا (flags & P_STRING)
	    أرجع مجموعه_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	إذا (varp != عدم)	// hidden option is not changed
	{
	    إذا (عدد == 0 && string != عدم)
	    {
		صحيح idx;

		// Either we are given a string or we are setting option
		// to zero.
		لكل (idx = 0; string[idx] == '0'; ++idx)
		    ;
		إذا (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_عدد_required_after_str_equal_str),
								 name, string);
		    أرجع عدم;     // do nothing as we hit an error

		}
	    }
	    إذا (flags & P_رقم)
	    {
		أرجع مجموعه_رقم_option(opt_idx, varp, عدد,
					   errbuf, حجم(errbuf), opt_flags);
	    }
	    وإلا
		أرجع مجموعه_bool_option(opt_idx, varp, (صحيح)عدد, opt_flags);
	}

    }
    أرجع عدم;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    فراغ
مجموعه_option_مقدار_give_err(
    char_u	*name,
    دراز	عدد,
    char_u	*string,
    صحيح		opt_flags)	// OPT_LOCAL or 0 (both)
{
    حرف *errmsg = مجموعه_option_مقدار(name, عدد, string, opt_flags);

    إذا (errmsg != عدم)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_جمله_کد(char_u *tname)
{
    صحيح	    opt_idx;
    char_u  *varp;

    إذا (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	أرجع عدم;
    إذا ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	إذا (varp != عدم)
	    varp = *(char_u **)(varp);
	أرجع varp;
    }
    أرجع پیدا_کن_termcode(tname + 2);
}

    char_u *
get_highlight_default(فراغ)
{
    صحيح i;

    i = findoption((char_u *)"hl");
    إذا (i >= 0)
	أرجع options[i].def_val[VI_DEFAULT];
    أرجع (char_u *)عدم;
}

    char_u *
get_encoding_default(فراغ)
{
    صحيح i;

    i = findoption((char_u *)"enc");
    إذا (i >= 0)
	أرجع options[i].def_val[VI_DEFAULT];
    أرجع (char_u *)عدم;
}

#إذا defined(FEAT_QUICKFIX)
    صحيح
is_option_allocated(حرف *name)
{
    صحيح idx = findoption((char_u *)name);

    أرجع idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#پایان_اگر

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    ساكن صحيح
پیدا_کن_کلید_option(char_u *arg_arg, صحيح has_lt)
{
    صحيح		کلید = 0;
    صحيح		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    إذا (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	إذا (!has_lt || arg[4] == '>')
	    کلید = TERMCAP2_کلید(arg[2], arg[3]);
    }
    وإلا إذا (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	کلید = پیدا_کن_special_کلید(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_کلید | FSK_SIMPLIFY, عدم);
	إذا (modifiers)		    // can't handle modifiers here
	    کلید = 0;
    }
    أرجع کلید;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    ساكن فراغ
showoptions(
    صحيح		all,
    صحيح		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    بنية vimoption	*p;
    صحيح			ستون;
    صحيح			isterm;
    char_u		*varp;
    بنية vimoption	**اقلام;
    صحيح			قلم_شمار;
    صحيح			پیاپی;
    صحيح			ردیف, ردیفها_2;
    صحيح			تعداد_ستون;
    صحيح			i;
    صحيح			len;

#تعریف_کن INC 20
#تعریف_کن GAP 3

    اقلام = ALLOC_MULT(بنية vimoption *, OPTION_شمار);
    إذا (اقلام == عدم)
	أرجع;

    // Highlight title
    إذا (all == 2)
	پیام_puts_title(_("\n--- Terminal codes ---"));
    وإلا إذا (opt_flags & OPT_GLOBAL)
	پیام_puts_title(_("\n--- Global option values ---"));
    وإلا إذا (opt_flags & OPT_LOCAL)
	پیام_puts_title(_("\n--- Local option values ---"));
    وإلا
	پیام_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    لكل (پیاپی = 1; پیاپی <= 2 && !got_int; ++پیاپی)
    {
	// collect the items in items[]
	قلم_شمار = 0;
	لكل (p = &options[0]; p->fullname != عدم; p++)
	{
	    // apply :filter /pat/
	    إذا (پیام_filtered((char_u *)p->fullname))
		استمر;

	    varp = عدم;
	    isterm = istermoption(p);
	    إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		إذا (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    وإلا
		varp = get_varp(p);
	    إذا (varp != عدم
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		إذا (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		وإلا إذا (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		وإلا
		{
		    option_value2string(p, opt_flags);
		    len = (صحيح)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		إذا ((len <= INC - GAP && پیاپی == 1) ||
						(len > INC - GAP && پیاپی == 2))
		    اقلام[قلم_شمار++] = p;
	    }
	}

	// display the items
	إذا (پیاپی == 1)
	{
	    تعداد_ستون = (Columns + GAP - 3) / INC;
	    إذا (تعداد_ستون == 0)
		تعداد_ستون = 1;
	    ردیفها_2 = (قلم_شمار + تعداد_ستون - 1) / تعداد_ستون;
	}
	وإلا	// run == 2
	    ردیفها_2 = قلم_شمار;
	لكل (ردیف = 0; ردیف < ردیفها_2 && !got_int; ++ردیف)
	{
	    پیام_putchar('\n');			// go to next line
	    إذا (got_int)			// 'q' typed in more
		اكسر;
	    ستون = 0;
	    لكل (i = ردیف; i < قلم_شمار; i += ردیفها_2)
	    {
		پیام_ستون = ستون;			// make columns
		showoneopt(اقلام[i], opt_flags);
		ستون += INC;
	    }
	    بیرون_flush();
	    ui_breakcheck();
	}
    }
    vim_free(اقلام);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    ساكن صحيح
optval_default(بنية vimoption *p, char_u *varp, صحيح compatible)
{
    صحيح		dvi;

    إذا (varp == عدم)
	أرجع TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    إذا (p->flags & P_رقم)
	أرجع (*(دراز *)varp == (دراز)(long_i)p->def_val[dvi]);
    إذا (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	أرجع (*(صحيح *)varp == (صحيح)(دراز)(long_i)p->def_val[dvi]);
    // P_STRING
    أرجع (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    ساكن فراغ
showoneopt(
    بنية vimoption	*p,
    صحيح			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    صحيح		save_silent = silent_نما;

    silent_نما = FALSE;
    info_پیام = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    إذا ((p->flags & P_BOOL) && ((صحيح *)varp == &curbuf->b_تغییریافته
					? !curbuf_Is_تغییریافته() : !*(صحيح *)varp))
	پیام_puts("no");
    وإلا إذا ((p->flags & P_BOOL) && *(صحيح *)varp < 0)
	پیام_puts("--");
    وإلا
	پیام_puts("  ");
    پیام_puts(p->fullname);
    إذا (!(p->flags & P_BOOL))
    {
	پیام_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	پیام_outtrans(NameBuff);
    }

    silent_نما = save_silent;
    info_پیام = FALSE;
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
    صحيح
makeset(FILE *fd, صحيح opt_flags, صحيح local_only)
{
    بنية vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = عدم;	// fresh value
    حرف		*cmd;
    صحيح			دور;
    صحيح			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    لكل (pri = 1; pri >= 0; --pri)
    {
      لكل (p = &options[0]; !istermoption(p); p++)
	إذا (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    إذا (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		استمر;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    إذا ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		استمر;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    إذا ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		استمر;

	    إذا ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		استمر;

	    دور = 2;
	    إذا (p->indir != PV_NONE)
	    {
		إذا (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    إذا (!(opt_flags & OPT_LOCAL))
			استمر;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    إذا (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			إذا (!optval_default(p, varp_fresh, p_cp))
			{
			    دور = 1;
			    varp_local = varp;
			    varp = varp_fresh;
			}
		    }
		}
	    }

	    // Round 1: fresh value for window-local options.
	    // Round 2: other values
	    لكل ( ; دور <= 2; varp = varp_local, ++دور)
	    {
		إذا (دور == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		وإلا
		    cmd = "setlocal";

		إذا (p->flags & P_BOOL)
		{
		    إذا (put_setbool(fd, cmd, p->fullname, *(صحيح *)varp) == FAIL)
			أرجع FAIL;
		}
		وإلا إذا (p->flags & P_رقم)
		{
		    إذا (put_setnum(fd, cmd, p->fullname, (دراز *)varp) == FAIL)
			أرجع FAIL;
		}
		وإلا    // P_STRING
		{
		    صحيح		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    إذا ((p->flags & P_FUNC) && *(char_u **)varp != عدم
			    && جستجوی_رشته(*(حرف **)varp, "<lambda>") != عدم)
			استمر;

#اگر_تعریف_شده FEAT_EVAL
		    legacy = !is_option_مقدار_vim9(p - &options[0],
			    دور == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#وإلا
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#پایان_اگر

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    إذا (
#إذا defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#پایان_اگر
			    p->indir == PV_FT)
		    {
			إذا (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    أرجع FAIL;
			do_endif = TRUE;
		    }
		    إذا (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			أرجع FAIL;
		    إذا (do_endif)
		    {
			إذا (put_سطر(fd, "endif") == FAIL)
			    أرجع FAIL;
		    }
		}
	    }
	}
    }
    أرجع OK;
}

#إذا defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    صحيح
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_نسخه < SCRIPT_نسخه_VIM9;
# endif

    إذا (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	أرجع FAIL;

    أرجع OK;
}
#پایان_اگر

    ساكن صحيح
put_setstring(
    FILE	*fd,
    bool	legacy,
    حرف	*cmd,
    حرف	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = عدم;
    char_u	*part = عدم;
    char_u	*p;

    إذا (legacy && fprintf(fd, "legacy ") < 0)
	أرجع FAIL;
    إذا (fprintf(fd, "%s %s=", cmd, name) < 0)
	أرجع FAIL;
    إذا (*valuep != عدم)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	إذا (valuep == &p_pt)
	{
	    s = *valuep;
	    طالما (*s != NUL)
		إذا (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    أرجع FAIL;
	}
	// expand the option value, replace $HOME by ~
	وإلا إذا ((flags & P_EXPAND) != 0)
	{
	    صحيح  اندازه = (صحيح)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(اندازه);
	    إذا (buf == عدم)
		اذهب fail;
	    home_replace(عدم, *valuep, buf, اندازه, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    إذا (اندازه >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != عدم)
	    {
		part = alloc(اندازه);
		إذا (part == عدم)
		    اذهب fail;

		// write line break to clear the option, e.g. ':set rtp='
		إذا (put_eol(fd) == FAIL)
		    اذهب fail;

		p = buf;
		طالما (*p != NUL)
		{
		    إذا (legacy && fprintf(fd, "legacy ") < 0)
			أرجع FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    إذا (fprintf(fd, "%s %s+=", cmd, name) < 0)
			اذهب fail;
		    (فراغ)copy_option_part(&p, part, اندازه,  ",");
		    إذا (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			اذهب fail;
		}
		vim_free(buf);
		vim_free(part);
		أرجع OK;
	    }
	    إذا (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		أرجع FAIL;
	    }
	    vim_free(buf);
	}
	وإلا إذا (put_escstr(fd, *valuep, 2) == FAIL)
	    أرجع FAIL;
    }
    إذا (put_eol(fd) < 0)
	أرجع FAIL;
    أرجع OK;
fail:
    vim_free(buf);
    vim_free(part);
    أرجع FAIL;
}

    ساكن صحيح
put_setnum(
    FILE	*fd,
    حرف	*cmd,
    حرف	*name,
    دراز	*valuep)
{
    دراز	wc;

    إذا (fprintf(fd, "%s %s=", cmd, name) < 0)
	أرجع FAIL;
    إذا (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	إذا (fputs((حرف *)get_special_کلید_name((صحيح)wc, 0), fd) < 0)
	    أرجع FAIL;
    }
    وإلا إذا (fprintf(fd, "%ld", *valuep) < 0)
	أرجع FAIL;
    إذا (put_eol(fd) < 0)
	أرجع FAIL;
    أرجع OK;
}

    ساكن صحيح
put_setbool(
    FILE	*fd,
    حرف	*cmd,
    حرف	*name,
    صحيح		مقدار)
{
    إذا (مقدار < 0)	// global/local option using global value
	أرجع OK;
    إذا (fprintf(fd, "%s %s%s", cmd, مقدار ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	أرجع FAIL;
    أرجع OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    فراغ
clear_termoptions(فراغ)
{
    // Reset a few things before clearing the old options. This may cause
    // outputting a few things that the terminal doesn't understand, but the
    // screen will be cleared later, so this is OK.
    mch_setmouse(FALSE);	    // switch mouse off
    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window titles
#إذا defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    إذا (gui.starting)
	clear_xterm_clip();
#پایان_اگر
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    فراغ
free_termoptions(فراغ)
{
    بنية vimoption   *p;

    لكل (p = options; p->fullname != عدم; p++)
	إذا (istermoption(p))
	{
	    إذا (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    إذا (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = خالی_option;
	    p->def_val[VI_DEFAULT] = خالی_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#اگر_تعریف_شده FEAT_EVAL
	    // remember where the option was cleared
	    مجموعه_option_sctx_idx((صحيح)(p - options), OPT_GLOBAL, current_sctx);
#پایان_اگر
	}
    clear_termcodes();
}

/*
 * Free the string for one term option, if it was allocated.
 * Set the string to empty_option and clear allocated flag.
 * "var" points to the option value.
 */
    فراغ
free_one_termoption(char_u *var)
{
    بنية vimoption   *p;

    لكل (p = &options[0]; p->fullname != عدم; p++)
	إذا (p->var == var)
	{
	    إذا (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = خالی_option;
	    p->flags &= ~P_ALLOCED;
	    اكسر;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    فراغ
مجموعه_جمله_defaults(فراغ)
{
    بنية vimoption   *p;

    لكل (p = &options[0]; p->fullname != عدم; p++)
    {
	إذا (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    إذا (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    إذا (p->flags & P_ALLOCED)
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
    ساكن صحيح
istermoption(بنية vimoption *p)
{
    أرجع (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    ساكن صحيح
istermoption_idx(صحيح opt_idx)
{
    أرجع istermoption(&options[opt_idx]);
}

#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    فراغ
unset_global_local_option(char_u *name, فراغ *from)
{
    بنية vimoption *p;
    صحيح		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    إذا (opt_idx < 0)
	أرجع;
    p = &(options[opt_idx]);

    بدل ((صحيح)p->indir)
    {
	// global option with local value: use local value if it's been set
	حالة PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    اكسر;
	حالة PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    اكسر;
	حالة PV_مسیر:
	    clear_string_option(&buf->b_p_مسیر);
	    اكسر;
	حالة PV_AC:
	    buf->b_p_ac = -1;
	    اكسر;
	حالة PV_AR:
	    buf->b_p_ar = -1;
	    اكسر;
# ifdef HAVE_FSYNC
	حالة PV_FS:
	    buf->b_p_fs = -1;
	    اكسر;
# endif
	حالة PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    اكسر;
	حالة PV_برچسبها:
	    clear_string_option(&buf->b_p_برچسبها);
	    اكسر;
	حالة PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    اكسر;
	حالة PV_SISO:
	    curwin->w_p_siso = -1;
	    اكسر;
	حالة PV_SO:
	    curwin->w_p_so = -1;
	    اكسر;
	حالة PV_SOP:
	    curwin->w_p_sop = -1;
	    اكسر;
# ifdef FEAT_پیدا_کن_ID
	حالة PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    اكسر;
	حالة PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    اكسر;
# endif
	حالة PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    اكسر;
	حالة PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    اكسر;
# ifdef FEAT_DIFF
	حالة PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    اكسر;
# endif
	حالة PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    اكسر;
# ifdef FEAT_COMPL_FUNC
	حالة PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    اكسر;
# endif
	حالة PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    اكسر;
# ifdef FEAT_EVAL
	حالة PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    اكسر;
# endif
# ifdef FEAT_QUICKFIX
	حالة PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    اكسر;
	حالة PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    اكسر;
	حالة PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    اكسر;
	حالة PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    اكسر;
# endif
# إذا defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	حالة PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    اكسر;
# endif
# إذا defined(FEAT_CRYPT)
	حالة PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    اكسر;
# endif
# ifdef FEAT_LINEBREAK
	حالة PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    اكسر;
# endif
# ifdef FEAT_STL_OPT
	حالة PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    اكسر;
	حالة PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    اكسر;
# endif
	حالة PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    اكسر;
	حالة PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    اكسر;
	حالة PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    اكسر;
	حالة PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    مجموعه_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      عدم, 0);
	    redraw_later(UPD_NOT_VALID);
	    اكسر;
	حالة PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    مجموعه_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      عدم, 0);
	    redraw_later(UPD_NOT_VALID);
	    اكسر;
	حالة PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    اكسر;
    }
}
#پایان_اگر

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    ساكن char_u *
get_varp_scope(بنية vimoption *p, صحيح scope)
{
    إذا ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	إذا (p->var == VAR_WIN)
	    أرجع (char_u *)GLOBAL_WO(get_varp(p));
	أرجع p->var;
    }
    إذا ((scope & OPT_LOCAL) && ((صحيح)p->indir & PV_BOTH))
    {
	بدل ((صحيح)p->indir)
	{
	    حالة PV_FP:   أرجع (char_u *)&(curbuf->b_p_fp);
#اگر_تعریف_شده HAVE_FSYNC
	    حالة PV_FS:	أرجع (char_u *)&(curbuf->b_p_fs);
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
	    حالة PV_FFU: أرجع (char_u *)&(curbuf->b_p_ffu);
#پایان_اگر
#اگر_تعریف_شده FEAT_QUICKFIX
	    حالة PV_EFM:  أرجع (char_u *)&(curbuf->b_p_efm);
	    حالة PV_GEFM: أرجع (char_u *)&(curbuf->b_p_gefm);
	    حالة PV_GP:   أرجع (char_u *)&(curbuf->b_p_gp);
	    حالة PV_MP:   أرجع (char_u *)&(curbuf->b_p_mp);
#پایان_اگر
	    حالة PV_EP:   أرجع (char_u *)&(curbuf->b_p_ep);
	    حالة PV_KP:   أرجع (char_u *)&(curbuf->b_p_kp);
	    حالة PV_مسیر: أرجع (char_u *)&(curbuf->b_p_مسیر);
	    حالة PV_AC:   أرجع (char_u *)&(curbuf->b_p_ac);
	    حالة PV_AR:   أرجع (char_u *)&(curbuf->b_p_ar);
	    حالة PV_برچسبها: أرجع (char_u *)&(curbuf->b_p_برچسبها);
	    حالة PV_TC:   أرجع (char_u *)&(curbuf->b_p_tc);
	    حالة PV_SISO: أرجع (char_u *)&(curwin->w_p_siso);
	    حالة PV_SO:   أرجع (char_u *)&(curwin->w_p_so);
	    حالة PV_SOP:  أرجع (char_u *)&(curwin->w_p_sop);
#اگر_تعریف_شده FEAT_پیدا_کن_ID
	    حالة PV_DEF:  أرجع (char_u *)&(curbuf->b_p_def);
	    حالة PV_INC:  أرجع (char_u *)&(curbuf->b_p_inc);
#پایان_اگر
	    حالة PV_COT:  أرجع (char_u *)&(curbuf->b_p_cot);
	    حالة PV_DICT: أرجع (char_u *)&(curbuf->b_p_dict);
#اگر_تعریف_شده FEAT_DIFF
	    حالة PV_DIA:  أرجع (char_u *)&(curbuf->b_p_dia);
#پایان_اگر
	    حالة PV_TSR:  أرجع (char_u *)&(curbuf->b_p_tsr);
#اگر_تعریف_شده FEAT_COMPL_FUNC
	    حالة PV_TSRFU: أرجع (char_u *)&(curbuf->b_p_tsrfu);
#پایان_اگر
#إذا defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    حالة PV_BEXPR: أرجع (char_u *)&(curbuf->b_p_bexpr);
#پایان_اگر
#إذا defined(FEAT_CRYPT)
	    حالة PV_CM:	  أرجع (char_u *)&(curbuf->b_p_cm);
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
	    حالة PV_SBR:  أرجع (char_u *)&(curwin->w_p_sbr);
#پایان_اگر
#اگر_تعریف_شده FEAT_STL_OPT
	    حالة PV_STL:  أرجع (char_u *)&(curwin->w_p_stl);
	    حالة PV_STLO: أرجع (char_u *)&(curwin->w_p_stlo);
#پایان_اگر
	    حالة PV_UL:   أرجع (char_u *)&(curbuf->b_p_ul);
	    حالة PV_LW:   أرجع (char_u *)&(curbuf->b_p_lw);
	    حالة PV_BKC:  أرجع (char_u *)&(curbuf->b_p_bkc);
	    حالة PV_MENC: أرجع (char_u *)&(curbuf->b_p_menc);
	    حالة PV_LCS:  أرجع (char_u *)&(curwin->w_p_lcs);
	    حالة PV_FCS:  أرجع (char_u *)&(curwin->w_p_fcs);
	    حالة PV_VE:	  أرجع (char_u *)&(curwin->w_p_ve);

	}
	أرجع عدم; // "cannot happen"
    }
    أرجع get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(صحيح opt_idx, صحيح scope)
{
    أرجع get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    ساكن char_u *
get_varp(بنية vimoption *p)
{
    // hidden option, always return NULL
    إذا (p->var == عدم)
	أرجع عدم;

    بدل ((صحيح)p->indir)
    {
	حالة PV_NONE:	أرجع p->var;

	// global option with local value: use local value if it's been set
	حالة PV_EP:	أرجع *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	حالة PV_KP:	أرجع *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	حالة PV_مسیر:	أرجع *curbuf->b_p_مسیر != NUL
				    ? (char_u *)&(curbuf->b_p_مسیر) : p->var;
	حالة PV_AC:	أرجع curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	حالة PV_AR:	أرجع curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	حالة PV_برچسبها:	أرجع *curbuf->b_p_برچسبها != NUL
				    ? (char_u *)&(curbuf->b_p_برچسبها) : p->var;
	حالة PV_TC:	أرجع *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	حالة PV_BKC:	أرجع *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	حالة PV_SISO:	أرجع curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	حالة PV_SO:	أرجع curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	حالة PV_SOP:	أرجع curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#اگر_تعریف_شده FEAT_پیدا_کن_ID
	حالة PV_DEF:	أرجع *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	حالة PV_INC:	أرجع *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#پایان_اگر
	حالة PV_COT:	أرجع *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	حالة PV_DICT:	أرجع *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#اگر_تعریف_شده FEAT_DIFF
	حالة PV_DIA:	أرجع *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#پایان_اگر
	حالة PV_TSR:	أرجع *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#اگر_تعریف_شده FEAT_COMPL_FUNC
	حالة PV_TSRFU:	أرجع *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#پایان_اگر
	حالة PV_FP:	أرجع *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#اگر_تعریف_شده HAVE_FSYNC
	حالة PV_FS:	أرجع curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
	حالة PV_FFU:	أرجع *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#پایان_اگر
#اگر_تعریف_شده FEAT_QUICKFIX
	حالة PV_EFM:	أرجع *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	حالة PV_GEFM:	أرجع *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	حالة PV_GP:	أرجع *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	حالة PV_MP:	أرجع *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#پایان_اگر
#إذا defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	حالة PV_BEXPR:	أرجع *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#پایان_اگر
#إذا defined(FEAT_CRYPT)
	حالة PV_CM:	أرجع *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
	حالة PV_SBR:	أرجع *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#پایان_اگر
#اگر_تعریف_شده FEAT_STL_OPT
	حالة PV_STL:	أرجع *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	حالة PV_STLO:	أرجع *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#پایان_اگر
	حالة PV_UL:	أرجع curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	حالة PV_LW:	أرجع *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	حالة PV_MENC:	أرجع *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#اگر_تعریف_شده FEAT_ARABIC
	حالة PV_ARAB:	أرجع (char_u *)&(curwin->w_p_arab);
#پایان_اگر
	حالة PV_LIST:	أرجع (char_u *)&(curwin->w_p_list);
	حالة PV_LCS:	أرجع *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	حالة PV_FCS:	أرجع *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	حالة PV_VE:	أرجع *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#اگر_تعریف_شده FEAT_SPELL
	حالة PV_SPELL:	أرجع (char_u *)&(curwin->w_p_spell);
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
	حالة PV_CUC:	أرجع (char_u *)&(curwin->w_p_cuc);
	حالة PV_CUL:	أرجع (char_u *)&(curwin->w_p_cul);
	حالة PV_CULOPT:	أرجع (char_u *)&(curwin->w_p_culopt);
	حالة PV_CC:	أرجع (char_u *)&(curwin->w_p_cc);
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
	حالة PV_DIFF:	أرجع (char_u *)&(curwin->w_p_diff);
#پایان_اگر
	حالة PV_EIW:	أرجع (char_u *)&(curwin->w_p_eiw);
#اگر_تعریف_شده FEAT_FOLDING
	حالة PV_FDC:	أرجع (char_u *)&(curwin->w_p_fdc);
	حالة PV_FEN:	أرجع (char_u *)&(curwin->w_p_fen);
	حالة PV_FDI:	أرجع (char_u *)&(curwin->w_p_fdi);
	حالة PV_FDL:	أرجع (char_u *)&(curwin->w_p_fdl);
	حالة PV_FDM:	أرجع (char_u *)&(curwin->w_p_fdm);
	حالة PV_FML:	أرجع (char_u *)&(curwin->w_p_fml);
	حالة PV_FDN:	أرجع (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	حالة PV_FDE:	أرجع (char_u *)&(curwin->w_p_fde);
	حالة PV_FDT:	أرجع (char_u *)&(curwin->w_p_fdt);
# endif
	حالة PV_FMR:	أرجع (char_u *)&(curwin->w_p_fmr);
#پایان_اگر
	حالة PV_NU:	أرجع (char_u *)&(curwin->w_p_nu);
	حالة PV_RNU:	أرجع (char_u *)&(curwin->w_p_rnu);
#اگر_تعریف_شده FEAT_LINEBREAK
	حالة PV_NUW:	أرجع (char_u *)&(curwin->w_p_nuw);
#پایان_اگر
	حالة PV_WFB:	أرجع (char_u *)&(curwin->w_p_wfb);
	حالة PV_WFH:	أرجع (char_u *)&(curwin->w_p_wfh);
	حالة PV_WFW:	أرجع (char_u *)&(curwin->w_p_wfw);
#إذا defined(FEAT_QUICKFIX)
	حالة PV_PVW:	أرجع (char_u *)&(curwin->w_p_pvw);
	حالة PV_LHI:	أرجع (char_u *)&(curwin->w_p_lhi);
#پایان_اگر
#اگر_تعریف_شده FEAT_RIGHTLEFT
	حالة PV_RL:	أرجع (char_u *)&(curwin->w_p_rl);
	حالة PV_RLC:	أرجع (char_u *)&(curwin->w_p_rlc);
#پایان_اگر
	حالة PV_SCROLL:	أرجع (char_u *)&(curwin->w_p_scr);
	حالة PV_SMS:	أرجع (char_u *)&(curwin->w_p_sms);
	حالة PV_WRAP:	أرجع (char_u *)&(curwin->w_p_wrap);
#اگر_تعریف_شده FEAT_LINEBREAK
	حالة PV_LBR:	أرجع (char_u *)&(curwin->w_p_lbr);
	حالة PV_BRI:	أرجع (char_u *)&(curwin->w_p_bri);
	حالة PV_BRIOPT: أرجع (char_u *)&(curwin->w_p_briopt);
#پایان_اگر
	حالة PV_WCR:	أرجع (char_u *)&(curwin->w_p_wcr);
	حالة PV_SCBIND: أرجع (char_u *)&(curwin->w_p_scb);
	حالة PV_CRBIND: أرجع (char_u *)&(curwin->w_p_crb);
#اگر_تعریف_شده FEAT_CONCEAL
	حالة PV_COCU:   أرجع (char_u *)&(curwin->w_p_cocu);
	حالة PV_COLE:   أرجع (char_u *)&(curwin->w_p_cole);
#پایان_اگر
	حالة PV_WHL:	أرجع (char_u *)&(curwin->w_p_whl);

#اگر_تعریف_شده FEAT_TERMINAL
	حالة PV_TWK:    أرجع (char_u *)&(curwin->w_p_twk);
	حالة PV_TWS:    أرجع (char_u *)&(curwin->w_p_tws);
	حالة PV_TWSL:	أرجع (char_u *)&(curbuf->b_p_twsl);
#پایان_اگر

	حالة PV_AI:	أرجع (char_u *)&(curbuf->b_p_ai);
	حالة PV_BIN:	أرجع (char_u *)&(curbuf->b_p_bin);
	حالة PV_BOMB:	أرجع (char_u *)&(curbuf->b_p_bomb);
	حالة PV_BH:	أرجع (char_u *)&(curbuf->b_p_bh);
	حالة PV_BT:	أرجع (char_u *)&(curbuf->b_p_bt);
	حالة PV_BL:	أرجع (char_u *)&(curbuf->b_p_bl);
	حالة PV_CI:	أرجع (char_u *)&(curbuf->b_p_ci);
	حالة PV_CIN:	أرجع (char_u *)&(curbuf->b_p_cin);
	حالة PV_CINK:	أرجع (char_u *)&(curbuf->b_p_cink);
	حالة PV_CINO:	أرجع (char_u *)&(curbuf->b_p_cino);
	حالة PV_CINSD:	أرجع (char_u *)&(curbuf->b_p_cinsd);
	حالة PV_CINW:	أرجع (char_u *)&(curbuf->b_p_cinw);
	حالة PV_COM:	أرجع (char_u *)&(curbuf->b_p_com);
	حالة PV_CMS:	أرجع (char_u *)&(curbuf->b_p_cms);
	حالة PV_CPT:	أرجع (char_u *)&(curbuf->b_p_cpt);
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
	حالة PV_CSL:	أرجع (char_u *)&(curbuf->b_p_csl);
#پایان_اگر
#اگر_تعریف_شده FEAT_COMPL_FUNC
	حالة PV_CFU:	أرجع (char_u *)&(curbuf->b_p_cfu);
	حالة PV_OFU:	أرجع (char_u *)&(curbuf->b_p_ofu);
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
	حالة PV_TFU:	أرجع (char_u *)&(curbuf->b_p_tfu);
#پایان_اگر
	حالة PV_EOF:	أرجع (char_u *)&(curbuf->b_p_eof);
	حالة PV_EOL:	أرجع (char_u *)&(curbuf->b_p_eol);
	حالة PV_FIXEOL:	أرجع (char_u *)&(curbuf->b_p_fixeol);
	حالة PV_ET:	أرجع (char_u *)&(curbuf->b_p_et);
	حالة PV_FENC:	أرجع (char_u *)&(curbuf->b_p_fenc);
	حالة PV_FF:	أرجع (char_u *)&(curbuf->b_p_ff);
	حالة PV_FT:	أرجع (char_u *)&(curbuf->b_p_ft);
	حالة PV_FO:	أرجع (char_u *)&(curbuf->b_p_fo);
	حالة PV_FLP:	أرجع (char_u *)&(curbuf->b_p_flp);
	حالة PV_IMI:	أرجع (char_u *)&(curbuf->b_p_iminsert);
	حالة PV_IMS:	أرجع (char_u *)&(curbuf->b_p_imsearch);
	حالة PV_INF:	أرجع (char_u *)&(curbuf->b_p_inf);
	حالة PV_ISK:	أرجع (char_u *)&(curbuf->b_p_isk);
#اگر_تعریف_شده FEAT_پیدا_کن_ID
# ifdef FEAT_EVAL
	حالة PV_INEX:	أرجع (char_u *)&(curbuf->b_p_inex);
# endif
#پایان_اگر
#إذا defined(FEAT_EVAL)
	حالة PV_INDE:	أرجع (char_u *)&(curbuf->b_p_inde);
	حالة PV_INDK:	أرجع (char_u *)&(curbuf->b_p_indk);
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
	حالة PV_FEX:	أرجع (char_u *)&(curbuf->b_p_fex);
#پایان_اگر
#اگر_تعریف_شده FEAT_CRYPT
	حالة PV_کلید:	أرجع (char_u *)&(curbuf->b_p_کلید);
#پایان_اگر
	حالة PV_LISP:	أرجع (char_u *)&(curbuf->b_p_lisp);
	حالة PV_LOP:	أرجع (char_u *)&(curbuf->b_p_lop);
	حالة PV_ML:	أرجع (char_u *)&(curbuf->b_p_ml);
	حالة PV_MPS:	أرجع (char_u *)&(curbuf->b_p_mps);
	حالة PV_MA:	أرجع (char_u *)&(curbuf->b_p_ma);
	حالة PV_MOD:	أرجع (char_u *)&(curbuf->b_تغییریافته);
	حالة PV_NF:	أرجع (char_u *)&(curbuf->b_p_nf);
	حالة PV_PI:	أرجع (char_u *)&(curbuf->b_p_pi);
	حالة PV_QE:	أرجع (char_u *)&(curbuf->b_p_qe);
	حالة PV_RO:	أرجع (char_u *)&(curbuf->b_p_ro);
	حالة PV_SI:	أرجع (char_u *)&(curbuf->b_p_si);
	حالة PV_SN:	أرجع (char_u *)&(curbuf->b_p_sn);
	حالة PV_STS:	أرجع (char_u *)&(curbuf->b_p_sts);
	حالة PV_SUA:	أرجع (char_u *)&(curbuf->b_p_sua);
	حالة PV_SWF:	أرجع (char_u *)&(curbuf->b_p_swf);
#اگر_تعریف_شده FEAT_SYN_HL
	حالة PV_SMC:	أرجع (char_u *)&(curbuf->b_p_smc);
	حالة PV_SYN:	أرجع (char_u *)&(curbuf->b_p_syn);
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
	حالة PV_SPC:	أرجع (char_u *)&(curwin->w_s->b_p_spc);
	حالة PV_SPF:	أرجع (char_u *)&(curwin->w_s->b_p_spf);
	حالة PV_SPL:	أرجع (char_u *)&(curwin->w_s->b_p_spl);
	حالة PV_SPO:	أرجع (char_u *)&(curwin->w_s->b_p_spo);
#پایان_اگر
	حالة PV_SW:	أرجع (char_u *)&(curbuf->b_p_sw);
	حالة PV_TS:	أرجع (char_u *)&(curbuf->b_p_ts);
	حالة PV_TW:	أرجع (char_u *)&(curbuf->b_p_tw);
	حالة PV_TX:	أرجع (char_u *)&(curbuf->b_p_tx);
#اگر_تعریف_شده FEAT_PERSISTENT_UNDO
	حالة PV_UDF:	أرجع (char_u *)&(curbuf->b_p_udf);
#پایان_اگر
	حالة PV_WM:	أرجع (char_u *)&(curbuf->b_p_wm);
#اگر_تعریف_شده FEAT_KEYMAP
	حالة PV_KMAP:	أرجع (char_u *)&(curbuf->b_p_keymap);
#پایان_اگر
#اگر_تعریف_شده FEAT_SIGNS
	حالة PV_SCL:	أرجع (char_u *)&(curwin->w_p_scl);
#پایان_اگر
#اگر_تعریف_شده FEAT_VARTABS
	حالة PV_VSTS:	أرجع (char_u *)&(curbuf->b_p_vsts);
	حالة PV_VTS:	أرجع (char_u *)&(curbuf->b_p_vts);
#پایان_اگر
	افتراضي:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    أرجع (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(صحيح opt_idx)
{
    أرجع options[opt_idx].var;
}

#إذا defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(صحيح opt_idx)
{
    أرجع (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    صحيح
option_info(char_u *name, dict_T *d)
{
    صحيح			opt_idx;
    بنية vimoption	*p;
    صحيح			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    إذا ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    إذا (opt_idx < 0)
	أرجع FAIL;

    p = &options[opt_idx];
    dict_جمع_کن_string(d, "name", (char_u *)p->fullname);
    dict_جمع_کن_string(d, "shortname",
		    (char_u *)(p->shortname == عدم ? "" : p->shortname));
    dict_جمع_کن_bool(d, "available", p->var != عدم);
    dict_جمع_کن_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_رقم) ? "number" : "string"));
    dict_جمع_کن_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    إذا (p->flags & P_BOOL)
	dict_جمع_کن_bool(d, "default", (صحيح)(long_i)p->def_val[dvi]);
    وإلا إذا (p->flags & P_رقم)
	dict_جمع_کن_عدد(d, "default", (دراز)(long_i)p->def_val[dvi]);
    وإلا
	dict_جمع_کن_string(d, "default", p->def_val[dvi] == عدم
					? (char_u *)"" : p->def_val[dvi]);
    أرجع OK;
}
#پایان_اگر

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_مجموعه_cb_T
get_option_did_مجموعه_cb(صحيح opt_idx)
{
    أرجع options[opt_idx].opt_did_مجموعه_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(فراغ)
{
    إذا (*curbuf->b_p_ep == NUL)
	أرجع p_ep;
    أرجع curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(فراغ)
{
#اگر_تعریف_شده FEAT_EVAL
    إذا (*curbuf->b_p_ffu == NUL)
	أرجع p_ffu;
    أرجع curbuf->b_p_ffu;
#وإلا
    أرجع (char_u *)"";
#پایان_اگر
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    فراغ
win_copy_options(win_T *wp_from, win_T *wp_مقصد)
{
    copy_winopt(&wp_from->w_onebuf_opt, &wp_مقصد->w_onebuf_opt);
    copy_winopt(&wp_from->w_allbuf_opt, &wp_مقصد->w_allbuf_opt);
#اگر_تعریف_شده FEAT_STL_OPT
    // w_stl_rendered_height is in win_T directly, not in winvar_T, so it is
    // not copied by copy_winopt().  Copy it explicitly here.
    wp_مقصد->w_stl_rendered_ارتفاع = wp_from->w_stl_rendered_ارتفاع;
#پایان_اگر
    after_copy_winopt(wp_مقصد);
}

/*
 * After copying window options: update variables depending on options.
 */
    فراغ
after_copy_winopt(win_T *wp)
{
    حرف *errmsg = update_winhighlight(wp, wp->w_p_whl);

    إذا (errmsg != عدم)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    إذا (wp->w_p_wrap)
	wp->w_leftcol = 0;
    وإلا
	wp->w_skipcol = 0;
#اگر_تعریف_شده FEAT_LINEBREAK
    briopt_check(عدم, wp);
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
    fill_culopt_flags(عدم, wp);
    check_colorcolumn(عدم, wp);
#پایان_اگر
    مجموعه_listchars_option(wp, wp->w_p_lcs, TRUE, عدم, 0);
    مجموعه_fillchars_option(wp, wp->w_p_fcs, TRUE, عدم, 0);
}

    ساكن char_u *
copy_option_val(char_u *val)
{
    إذا (val == خالی_option)
	أرجع خالی_option;  // no need to allocate memory
    أرجع vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    فراغ
copy_winopt(winopt_T *from, winopt_T *مقصد)
{
#اگر_تعریف_شده FEAT_ARABIC
    مقصد->wo_arab = from->wo_arab;
#پایان_اگر
    مقصد->wo_list = from->wo_list;
    مقصد->wo_lcs = copy_option_val(from->wo_lcs);
    مقصد->wo_fcs = copy_option_val(from->wo_fcs);
    مقصد->wo_nu = from->wo_nu;
    مقصد->wo_rnu = from->wo_rnu;
    مقصد->wo_ve = copy_option_val(from->wo_ve);
    مقصد->wo_ve_flags = from->wo_ve_flags;
#اگر_تعریف_شده FEAT_LINEBREAK
    مقصد->wo_nuw = from->wo_nuw;
#پایان_اگر
#اگر_تعریف_شده FEAT_RIGHTLEFT
    مقصد->wo_rl  = from->wo_rl;
    مقصد->wo_rlc = copy_option_val(from->wo_rlc);
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
    مقصد->wo_sbr = copy_option_val(from->wo_sbr);
#پایان_اگر
#اگر_تعریف_شده FEAT_STL_OPT
    مقصد->wo_stl = copy_option_val(from->wo_stl);
    مقصد->wo_stlo = copy_option_val(from->wo_stlo);
    مقصد->wo_stlo_fh = from->wo_stlo_fh;
    مقصد->wo_stlo_mh = from->wo_stlo_mh;
#پایان_اگر
    مقصد->wo_wrap = from->wo_wrap;
#اگر_تعریف_شده FEAT_DIFF
    مقصد->wo_wrap_save = from->wo_wrap_save;
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
    مقصد->wo_lbr = from->wo_lbr;
    مقصد->wo_bri = from->wo_bri;
    مقصد->wo_briopt = copy_option_val(from->wo_briopt);
#پایان_اگر
    مقصد->wo_wcr = copy_option_val(from->wo_wcr);
    مقصد->wo_scb = from->wo_scb;
    مقصد->wo_scb_save = from->wo_scb_save;
    مقصد->wo_sms = from->wo_sms;
    مقصد->wo_crb = from->wo_crb;
    مقصد->wo_crb_save = from->wo_crb_save;
    مقصد->wo_siso = from->wo_siso;
    مقصد->wo_so = from->wo_so;
    مقصد->wo_sop = from->wo_sop;
#اگر_تعریف_شده FEAT_SPELL
    مقصد->wo_spell = from->wo_spell;
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
    مقصد->wo_cuc = from->wo_cuc;
    مقصد->wo_cul = from->wo_cul;
    مقصد->wo_culopt = copy_option_val(from->wo_culopt);
    مقصد->wo_cc = copy_option_val(from->wo_cc);
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
    مقصد->wo_diff = from->wo_diff;
    مقصد->wo_diff_saved = from->wo_diff_saved;
#پایان_اگر
    مقصد->wo_eiw = copy_option_val(from->wo_eiw);
#اگر_تعریف_شده FEAT_CONCEAL
    مقصد->wo_cocu = copy_option_val(from->wo_cocu);
    مقصد->wo_cole = from->wo_cole;
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
    مقصد->wo_twk = copy_option_val(from->wo_twk);
    مقصد->wo_tws = copy_option_val(from->wo_tws);
#پایان_اگر
#اگر_تعریف_شده FEAT_FOLDING
    مقصد->wo_fdc = from->wo_fdc;
    مقصد->wo_fdc_save = from->wo_fdc_save;
    مقصد->wo_fen = from->wo_fen;
    مقصد->wo_fen_save = from->wo_fen_save;
    مقصد->wo_fdi = copy_option_val(from->wo_fdi);
    مقصد->wo_fml = from->wo_fml;
    مقصد->wo_fdl = from->wo_fdl;
    مقصد->wo_fdl_save = from->wo_fdl_save;
    مقصد->wo_fdm = copy_option_val(from->wo_fdm);
    مقصد->wo_fdm_save = from->wo_diff_saved
			       ? vim_strsave(from->wo_fdm_save) : خالی_option;
    مقصد->wo_fdn = from->wo_fdn;
# ifdef FEAT_EVAL
    مقصد->wo_fde = copy_option_val(from->wo_fde);
    مقصد->wo_fdt = copy_option_val(from->wo_fdt);
# endif
    مقصد->wo_fmr = copy_option_val(from->wo_fmr);
#پایان_اگر
#اگر_تعریف_شده FEAT_SIGNS
    مقصد->wo_scl = copy_option_val(from->wo_scl);
#پایان_اگر
#اگر_تعریف_شده FEAT_QUICKFIX
    مقصد->wo_lhi = from->wo_lhi;
#پایان_اگر

    مقصد->wo_wrap_flags = from->wo_wrap_flags;
#اگر_تعریف_شده FEAT_STL_OPT
    مقصد->wo_stl_flags = from->wo_stl_flags;
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
    مقصد->wo_fde_flags = from->wo_fde_flags;
    مقصد->wo_fdt_flags = from->wo_fdt_flags;
#پایان_اگر
    مقصد->wo_whl = copy_option_val(from->wo_whl);

#اگر_تعریف_شده FEAT_EVAL
    // Copy the script context so that we know where the value was last set.
    mch_memmove(مقصد->wo_script_ctx, from->wo_script_ctx,
						    حجم(مقصد->wo_script_ctx));
#پایان_اگر
    check_winopt(مقصد);		// don't want NULL pointers
}

/*
 * Check string options in a window for a NULL value.
 */
    ساكن فراغ
check_win_options(win_T *win)
{
    check_winopt(&win->w_onebuf_opt);
    check_winopt(&win->w_allbuf_opt);
}

/*
 * Check for NULL pointers in a winopt_T and replace them with empty_option.
 */
    ساكن فراغ
check_winopt(winopt_T *wop استفادهنشده)
{
#اگر_تعریف_شده FEAT_FOLDING
    check_string_option(&wop->wo_fdi);
    check_string_option(&wop->wo_fdm);
    check_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    check_string_option(&wop->wo_fde);
    check_string_option(&wop->wo_fdt);
# endif
    check_string_option(&wop->wo_fmr);
#پایان_اگر
    check_string_option(&wop->wo_eiw);
#اگر_تعریف_شده FEAT_SIGNS
    check_string_option(&wop->wo_scl);
#پایان_اگر
#اگر_تعریف_شده FEAT_RIGHTLEFT
    check_string_option(&wop->wo_rlc);
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
    check_string_option(&wop->wo_sbr);
#پایان_اگر
#اگر_تعریف_شده FEAT_STL_OPT
    check_string_option(&wop->wo_stl);
    check_string_option(&wop->wo_stlo);
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
    check_string_option(&wop->wo_culopt);
    check_string_option(&wop->wo_cc);
#پایان_اگر
#اگر_تعریف_شده FEAT_CONCEAL
    check_string_option(&wop->wo_cocu);
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
    check_string_option(&wop->wo_twk);
    check_string_option(&wop->wo_tws);
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
    check_string_option(&wop->wo_briopt);
#پایان_اگر
    check_string_option(&wop->wo_wcr);
    check_string_option(&wop->wo_lcs);
    check_string_option(&wop->wo_fcs);
    check_string_option(&wop->wo_ve);
    check_string_option(&wop->wo_whl);
}

/*
 * Free the allocated memory inside a winopt_T.
 */
    فراغ
clear_winopt(winopt_T *wop استفادهنشده)
{
#اگر_تعریف_شده FEAT_FOLDING
    clear_string_option(&wop->wo_fdi);
    clear_string_option(&wop->wo_fdm);
    clear_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    clear_string_option(&wop->wo_fde);
    clear_string_option(&wop->wo_fdt);
# endif
    clear_string_option(&wop->wo_fmr);
#پایان_اگر
    clear_string_option(&wop->wo_eiw);
#اگر_تعریف_شده FEAT_SIGNS
    clear_string_option(&wop->wo_scl);
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
    clear_string_option(&wop->wo_briopt);
#پایان_اگر
    clear_string_option(&wop->wo_wcr);
#اگر_تعریف_شده FEAT_RIGHTLEFT
    clear_string_option(&wop->wo_rlc);
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
    clear_string_option(&wop->wo_sbr);
#پایان_اگر
#اگر_تعریف_شده FEAT_STL_OPT
    clear_string_option(&wop->wo_stl);
    clear_string_option(&wop->wo_stlo);
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
    clear_string_option(&wop->wo_culopt);
    clear_string_option(&wop->wo_cc);
#پایان_اگر
#اگر_تعریف_شده FEAT_CONCEAL
    clear_string_option(&wop->wo_cocu);
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
    clear_string_option(&wop->wo_twk);
    clear_string_option(&wop->wo_tws);
#پایان_اگر
    clear_string_option(&wop->wo_lcs);
    clear_string_option(&wop->wo_fcs);
    clear_string_option(&wop->wo_ve);
    clear_string_option(&wop->wo_whl);
}

#اگر_تعریف_شده FEAT_EVAL
// Index into the options table for a buffer-local option enum.
ساكن صحيح buf_opt_idx[BV_شمار];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    ساكن فراغ
init_buf_opt_idx(فراغ)
{
    ساكن صحيح did_init_buf_opt_idx = FALSE;
    صحيح i;

    إذا (did_init_buf_opt_idx)
	أرجع;
    did_init_buf_opt_idx = TRUE;
    لكل (i = 0; !istermoption_idx(i); i++)
	إذا (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_ماسک] = i;
}
#وإلا
# define COPY_OPT_SCTX(buf, bv)
# define COPY_OPT_INSECURE(flagsfield, bv)
#پایان_اگر

/*
 * Copy global option values to local options for one buffer.
 * Used when creating a new buffer and sometimes when entering a buffer.
 * flags:
 * BCO_ENTER	We will enter the buffer "buf".
 * BCO_ALWAYS	Always copy the options, but only set b_p_initialized when
 *		appropriate.
 * BCO_NOHELP	Don't copy the values to a help buffer.
 */
    فراغ
buf_copy_options(buf_T *buf, صحيح flags)
{
    صحيح		should_copy = TRUE;
    char_u	*save_p_isk = عدم;	    // init for GCC
    صحيح		dont_do_help;
    صحيح		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    إذا (p_cpo != عدم)
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
	إذا ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == عدم || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != عدم)))
	    should_copy = FALSE;

	إذا (should_copy || (flags & BCO_ALWAYS))
	{
#اگر_تعریف_شده FEAT_EVAL
	    CLEAR_فیلد(buf->b_p_script_ctx);
	    init_buf_opt_idx();
#پایان_اگر
	    // Don't copy the options specific to a help buffer when
	    // BCO_NOHELP is given or the options were initialized already
	    // (jumping back to a help file with CTRL-T or CTRL-O)
	    dont_do_help = ((flags & BCO_NOHELP) && buf->b_help)
						       || buf->b_p_initialized;
	    إذا (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = عدم;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    إذا (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		بدل (*p_ffs)
		{
		    حالة 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); اكسر;
		    حالة 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); اكسر;
		    حالة 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); اكسر;
		    افتراضي:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		إذا (buf->b_p_ff != عدم)
		    buf->b_شروع_ffc = *buf->b_p_ff;
		buf->b_p_bh = خالی_option;
		buf->b_p_bt = خالی_option;
	    }
	    وإلا
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
	    إذا (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    وإلا
	    {
		buf->b_p_swf = p_swf;
		COPY_OPT_SCTX(buf, BV_SWF);
	    }
	    buf->b_p_cpt = vim_strsave(p_cpt);
	    COPY_OPT_SCTX(buf, BV_CPT);
#اگر_تعریف_شده FEAT_COMPL_FUNC
	    COPY_OPT_INSECURE(buf->b_p_cpt_flags, BV_CPT);
	    مجموعه_buflocal_cpt_callbacks(buf);
#پایان_اگر
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
	    buf->b_p_csl = vim_strsave(p_csl);
	    COPY_OPT_SCTX(buf, BV_CSL);
#پایان_اگر
#اگر_تعریف_شده FEAT_COMPL_FUNC
	    buf->b_p_cfu = vim_strsave(p_cfu);
	    COPY_OPT_SCTX(buf, BV_CFU);
	    مجموعه_buflocal_cfu_callback(buf);
	    buf->b_p_ofu = vim_strsave(p_ofu);
	    COPY_OPT_SCTX(buf, BV_OFU);
	    مجموعه_buflocal_ofu_callback(buf);
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
	    buf->b_p_tfu = vim_strsave(p_tfu);
	    COPY_OPT_SCTX(buf, BV_TFU);
	    مجموعه_buflocal_tfu_callback(buf);
#پایان_اگر
	    buf->b_p_sts = p_sts;
	    COPY_OPT_SCTX(buf, BV_STS);
	    buf->b_p_sts_nopaste = p_sts_nopaste;
#اگر_تعریف_شده FEAT_VARTABS
	    buf->b_p_vsts = vim_strsave(p_vsts);
	    COPY_OPT_SCTX(buf, BV_VSTS);
	    إذا (p_vsts && p_vsts != خالی_option)
		(فراغ)tabstop_مجموعه(p_vsts, &buf->b_p_vsts_array);
	    وإلا
		buf->b_p_vsts_array = عدم;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : عدم;
#پایان_اگر
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
	    buf->b_p_ft = خالی_option;
	    buf->b_p_pi = p_pi;
	    COPY_OPT_SCTX(buf, BV_PI);
	    buf->b_p_cinw = vim_strsave(p_cinw);
	    COPY_OPT_SCTX(buf, BV_CINW);
	    buf->b_p_lisp = p_lisp;
	    COPY_OPT_SCTX(buf, BV_LISP);
#اگر_تعریف_شده FEAT_SYN_HL
	    // Don't copy 'syntax', it must be set
	    buf->b_p_syn = خالی_option;
	    buf->b_p_smc = p_smc;
	    COPY_OPT_SCTX(buf, BV_SMC);
	    buf->b_s.b_syn_isk = خالی_option;
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
	    buf->b_s.b_p_spc = vim_strsave(p_spc);
	    COPY_OPT_SCTX(buf, BV_SPC);
	    (فراغ)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#پایان_اگر
#إذا defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#پایان_اگر
	    buf->b_p_fp = خالی_option;
#إذا defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#پایان_اگر
#اگر_تعریف_شده FEAT_CRYPT
	    buf->b_p_کلید = vim_strsave(p_کلید);
	    COPY_OPT_SCTX(buf, BV_کلید);
#پایان_اگر
	    buf->b_p_sua = vim_strsave(p_sua);
	    COPY_OPT_SCTX(buf, BV_SUA);
#اگر_تعریف_شده FEAT_KEYMAP
	    buf->b_p_keymap = vim_strsave(p_keymap);
	    COPY_OPT_SCTX(buf, BV_KMAP);
	    buf->b_kmap_state |= KEYMAP_INIT;
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
	    buf->b_p_twsl = p_twsl;
	    COPY_OPT_SCTX(buf, BV_TWSL);
#پایان_اگر
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
#اگر_تعریف_شده HAVE_FSYNC
	    buf->b_p_fs = -1;
#پایان_اگر
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    buf->b_p_bkc = خالی_option;
	    buf->b_bkc_flags = 0;
#اگر_تعریف_شده FEAT_QUICKFIX
	    buf->b_p_gefm = خالی_option;
	    buf->b_p_gp = خالی_option;
	    buf->b_p_mp = خالی_option;
	    buf->b_p_efm = خالی_option;
#پایان_اگر
	    buf->b_p_ep = خالی_option;
#إذا defined(FEAT_EVAL)
	    buf->b_p_ffu = خالی_option;
#پایان_اگر
	    buf->b_p_kp = خالی_option;
	    buf->b_p_مسیر = خالی_option;
	    buf->b_p_برچسبها = خالی_option;
	    buf->b_p_tc = خالی_option;
	    buf->b_tc_flags = 0;
#اگر_تعریف_شده FEAT_پیدا_کن_ID
	    buf->b_p_def = خالی_option;
	    buf->b_p_inc = خالی_option;
# ifdef FEAT_EVAL
	    buf->b_p_inex = vim_strsave(p_inex);
	    COPY_OPT_SCTX(buf, BV_INEX);
	    COPY_OPT_INSECURE(buf->b_p_inex_flags, BV_INEX);
# endif
#پایان_اگر
	    buf->b_p_cot = خالی_option;
	    buf->b_cot_flags = 0;
	    buf->b_p_dict = خالی_option;
#اگر_تعریف_شده FEAT_DIFF
	    buf->b_p_dia = خالی_option;
#پایان_اگر
	    buf->b_p_tsr = خالی_option;
#اگر_تعریف_شده FEAT_COMPL_FUNC
	    buf->b_p_tsrfu = خالی_option;
#پایان_اگر
	    buf->b_p_qe = vim_strsave(p_qe);
	    COPY_OPT_SCTX(buf, BV_QE);
#إذا defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = خالی_option;
#پایان_اگر
#إذا defined(FEAT_CRYPT)
	    buf->b_p_cm = خالی_option;
#پایان_اگر
#اگر_تعریف_شده FEAT_PERSISTENT_UNDO
	    buf->b_p_udf = p_udf;
	    COPY_OPT_SCTX(buf, BV_UDF);
#پایان_اگر
	    buf->b_p_lw = خالی_option;
	    buf->b_p_menc = خالی_option;

	    // Don't copy the options set by ex_help(), use the saved values,
	    // when going from a help buffer to a non-help buffer.
	    // Don't touch these at all when BCO_NOHELP is used and going from
	    // or to a help buffer.
	    إذا (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#اگر_تعریف_شده FEAT_VARTABS
		إذا (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (فراغ)tabstop_مجموعه(p_vts, &buf->b_p_vts_array);
		وإلا
		    buf->b_p_vts_array = عدم;
#پایان_اگر
	    }
	    وإلا
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#اگر_تعریف_شده FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		إذا (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (فراغ)tabstop_مجموعه(p_vts, &buf->b_p_vts_array);
		وإلا
		    buf->b_p_vts_array = عدم;
#پایان_اگر
		buf->b_help = false;
		إذا (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	إذا (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    إذا (did_isk)
	(فراغ)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    فراغ
reset_modifiable(فراغ)
{
    صحيح		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    إذا (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}

/*
 * Set the global value for 'iminsert' to the local value.
 */
    فراغ
مجموعه_iminsert_global(فراغ)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    فراغ
مجموعه_imsearch_global(فراغ)
{
    p_imsearch = curbuf->b_p_imsearch;
}

ساكن صحيح expand_option_idx = -1;
ساكن صحيح expand_option_شروع_ستون = 0;
ساكن char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
ساكن صحيح expand_option_flags = 0;
ساكن صحيح expand_option_append = FALSE;

    فراغ
مجموعه_context_درون_مجموعه_cmd(
    expand_T	*xp,
    char_u	*arg,
    صحيح		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    صحيح		nextchar;
    long_u	flags = 0;	// init for GCC
    صحيح		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    صحيح		is_جمله_option = FALSE;
    صحيح		کلید;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    إذا (*arg == NUL)
    {
	xp->xp_الگو = arg;
	أرجع;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    إذا (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_الگو = p + 1;
	أرجع;
    }
    طالما (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	إذا (*p == ' ' || *p == ',')
	{
	    طالما (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	إذا (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    اكسر;
	}
	--p;
    }
    إذا (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_پیشوند = XP_پیشوند_NO;
	p += 2;
    }
    وإلا إذا (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_پیشوند = XP_پیشوند_INV;
	p += 3;
    }
    xp->xp_الگو = arg = p;
    إذا (*arg == '<')
    {
	طالما (*p != '>')
	    إذا (*p++ == NUL)	    // expand terminal option name
		أرجع;
	کلید = get_special_کلید_کد(arg + 1);
	إذا (کلید == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    أرجع;
	}
	nextchar = *++p;
	is_جمله_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(کلید);
	expand_option_name[3] = KEY2TERMCAP1(کلید);
    }
    وإلا
    {
	إذا (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    إذا (*p != NUL)
		++p;
	    إذا (*p == NUL)
		أرجع;		// expand option name
	    nextchar = *++p;
	    is_جمله_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	وإلا
	{
	    // Allow * wildcard
	    طالما (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    إذا (*p == NUL)
		أرجع;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    إذا (opt_idx == -1 || options[opt_idx].var == عدم)
	    {
		xp->xp_context = EXPAND_NOTHING;
		أرجع;
	    }
	    flags = options[opt_idx].flags;
	    إذا (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		أرجع;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    صحيح expand_option_تفریق = FALSE;
    إذا ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	إذا (nextchar == '-')
	    expand_option_تفریق = TRUE;
	إذا (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    إذا ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	أرجع;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    إذا (is_جمله_option)
	expand_option_idx = -1;
    وإلا
	expand_option_idx = opt_idx;

    إذا (!is_جمله_option)
    {
	إذا (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    أرجع;
	}
    }

    xp->xp_الگو = p + 1;
    expand_option_شروع_ستون = (صحيح)(p + 1 - xp->xp_سطر);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#اگر_تعریف_شده FEAT_SYN_HL
    إذا (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	أرجع;
    }
#پایان_اگر
    إذا (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	أرجع;
    }
#اگر_تعریف_شده FEAT_KEYMAP
    إذا (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	أرجع;
    }
#پایان_اگر

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    إذا (expand_option_تفریق)
    {
	xp->xp_context = EXPAND_SETTING_تفریق;
	أرجع;
    }
    وإلا إذا (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != عدم)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    وإلا إذا (*xp->xp_الگو == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	أرجع;
    }
    وإلا
	xp->xp_context = EXPAND_NOTHING;

    إذا (is_جمله_option || (flags & P_رقم))
	أرجع;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    إذا (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	إذا (p == (char_u *)&p_bdir
		|| p == (char_u *)&p_dir
		|| p == (char_u *)&p_مسیر
		|| p == (char_u *)&p_pp
		|| p == (char_u *)&p_rtp
		|| p == (char_u *)&p_cdpath
#اگر_تعریف_شده FEAT_SESSION
		|| p == (char_u *)&p_vdir
#پایان_اگر
		)
	{
	    xp->xp_context = EXPAND_DIRECTORIES;
	    إذا (p == (char_u *)&p_مسیر || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    وإلا
		xp->xp_backslash = XP_BS_ONE;
	}
	وإلا
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    إذا (p == (char_u *)&p_برچسبها)
		xp->xp_backslash = XP_BS_THREE;
	    وإلا
		xp->xp_backslash = XP_BS_ONE;
	}
	إذا (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    إذا ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	لكل (p = argend - 1; p >= xp->xp_الگو; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    إذا (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		طالما (s > xp->xp_الگو && *(s - 1) == '\\')
		    --s;
		إذا ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#إذا defined(BACKSLASH_درون_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#وإلا
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#پایان_اگر
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_الگو = p + 1;
		    اكسر;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    إذا (flags & P_FLAGLIST)
	xp->xp_الگو = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#اگر_تعریف_شده FEAT_SPELL
    إذا (options[opt_idx].var == (char_u *)&p_sps)
    {
	إذا (STRNCMP(xp->xp_الگو, "file:", 5) == 0)
	{
	    xp->xp_الگو += 5;
	    أرجع;
	}
	وإلا إذا (options[expand_option_idx].opt_expand_cb != عدم)
	{
	    xp->xp_context = EXPAND_STRING_SETTING;
	}
    }
#پایان_اگر
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
    ساكن صحيح
تطابق_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	صحيح		idx,
	صحيح		test_only,
	صحيح		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    إذا (!fuzzy)
    {
	إذا (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    إذا (!test_only)
		matches[idx] = vim_strsave(str);
	    أرجع TRUE;
	}
    }
    وإلا
    {
	صحيح score;

	score = fuzzy_تطابق_str(str, fuzzystr);
	إذا (score != FUZZY_SCORE_NONE)
	{
	    إذا (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    أرجع TRUE;
	}
    }

    أرجع FALSE;
}

    صحيح
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    صحيح		*رقم_Matches,
    char_u	***matches,
    صحيح		can_fuzzy)
{
    صحيح		رقم_نرمال = 0;	    // Nr of matching non-term-code settings
    صحيح		رقم_جمله = 0;	    // Nr of matching terminal code settings
    صحيح		opt_idx;
    صحيح		تطابق;
    صحيح		شمار = 0;
    char_u	*str;
    صحيح		loop;
    صحيح		is_جمله_opt;
    char_u	name_buf[MAX_کلید_NAME_LEN];
    ساكن حرف *(نامها[]) = {"all", "termcap"};
    صحيح		ic = regmatch->rm_ic;	// remember the ignore-case flag
    صحيح		fuzzy;
    fuzmatch_str_T  *fuzmatch = عدم;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    لكل (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	إذا (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    لكل (تطابق = 0; تطابق < (صحيح)ARRAY_درازی(نامها); ++تطابق)
	    {
		إذا (تطابق_str((char_u *)نامها[تطابق], regmatch, *matches,
			    شمار, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    إذا (loop == 0)
			رقم_نرمال++;
		    وإلا
			شمار++;
		}
	    }
	}
	لكل (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != عدم;
								    opt_idx++)
	{
	    إذا (options[opt_idx].var == عدم)
		استمر;
	    إذا (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		استمر;
	    is_جمله_opt = istermoption_idx(opt_idx);
	    إذا (is_جمله_opt && رقم_نرمال > 0)
		استمر;

	    إذا (تطابق_str(str, regmatch, *matches, شمار, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		إذا (loop == 0)
		{
		    إذا (is_جمله_opt)
			رقم_جمله++;
		    وإلا
			رقم_نرمال++;
		}
		وإلا
		    شمار++;
	    }
	    وإلا إذا (!fuzzy && options[opt_idx].shortname != عدم
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		إذا (loop == 0)
		{
		    إذا (is_جمله_opt)
			رقم_جمله++;
		    وإلا
			رقم_نرمال++;
		}
		وإلا
		    (*matches)[شمار++] = vim_strsave(str);
	    }
	    وإلا إذا (is_جمله_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		إذا (تطابق_str(name_buf, regmatch, *matches, شمار, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    إذا (loop == 0)
			رقم_جمله++;
		    وإلا
			شمار++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	إذا (xp->xp_context != EXPAND_BOOL_SETTINGS  && رقم_نرمال == 0)
	{
	    لكل (opt_idx = 0; (str = get_termcode(opt_idx)) != عدم; opt_idx++)
	    {
		إذا (!امن_isprint(str[0]) || !امن_isprint(str[1]))
		    استمر;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		إذا (تطابق_str(name_buf, regmatch, *matches, شمار,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    إذا (loop == 0)
			رقم_جمله++;
		    وإلا
			شمار++;
		}
		وإلا
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    إذا (تطابق_str(name_buf, regmatch, *matches, شمار,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			إذا (loop == 0)
			    رقم_جمله++;
			وإلا
			    شمار++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    لكل (opt_idx = 0; (str = get_کلید_name(opt_idx)) != عدم; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		إذا (تطابق_str(name_buf, regmatch, *matches, شمار, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    إذا (loop == 0)
			رقم_جمله++;
		    وإلا
			شمار++;
		}
	    }
	}
	إذا (loop == 0)
	{
	    إذا (رقم_نرمال > 0)
		*رقم_Matches = رقم_نرمال;
	    وإلا إذا (رقم_جمله > 0)
		*رقم_Matches = رقم_جمله;
	    وإلا
		أرجع OK;
	    إذا (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *رقم_Matches);
		إذا (*matches == عدم)
		{
		    *matches = (char_u **)"";
		    أرجع FAIL;
		}
	    }
	    وإلا
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *رقم_Matches);
		إذا (fuzmatch == عدم)
		{
		    *matches = (char_u **)"";
		    أرجع FAIL;
		}
	    }
	}
    }

    إذا (fuzzy &&
	fuzzymatches_مقصد_strmatches(fuzmatch, matches, شمار, FALSE) == FAIL)
	أرجع FAIL;

    أرجع OK;
}

// Escape an option value that can be used on the command-line with :set.
// Caller needs to free the returned string, unless NULL is returned.
    ساكن char_u*
escape_option_str_cmdline(char_u *var)
{
    char_u *buf;

    // A backslash is required before some characters.  This is the reverse of
    // what happens in do_set().
    buf = vim_strsave_escaped(var, escape_chars);
    إذا (buf == عدم)
	أرجع عدم;

#اگر_تعریف_شده BACKSLASH_درون_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    لكل (var = buf; *var != NUL; MB_PTR_ADV(var))
	إذا (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#پایان_اگر
    أرجع buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    صحيح
ExpandOldSetting(صحيح *رقم_Matches, char_u ***matches)
{
    char_u  *var = عدم;	// init for GCC
    char_u  *buf;

    *رقم_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    إذا (*matches == عدم)
	أرجع FAIL;

    // For a terminal key code expand_option_idx is < 0.
    إذا (expand_option_idx < 0)
    {
	var = پیدا_کن_termcode(expand_option_name + 2);
	إذا (var == عدم)
	    expand_option_idx = findoption(expand_option_name);
    }

    إذا (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    وإلا إذا (var == عدم)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    إذا (buf == عدم)
    {
	VIM_CLEAR(*matches);
	أرجع FAIL;
    }

    (*matches)[0] = buf;
    *رقم_Matches = 1;
    أرجع OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    صحيح
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    صحيح		*رقم_Matches,
    char_u	***matches)
{
    char_u  *var = عدم;	// init for GCC
    char_u  *buf;

    إذا (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == عدم)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	أرجع FAIL;
    }

    optexpand_T آرگومانها;
    آرگومانها.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    آرگومانها.oe_append = expand_option_append;
    آرگومانها.oe_regmatch = regmatch;
    آرگومانها.oe_xp = xp;
    آرگومانها.oe_مجموعه_arg = xp->xp_سطر + expand_option_شروع_ستون;
    آرگومانها.oe_include_orig_val =
	!expand_option_append &&
	(*آرگومانها.oe_مجموعه_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    إذا (buf == عدم)
	أرجع FAIL;

    آرگومانها.oe_opt_مقدار = buf;

    صحيح رقم_ret = options[expand_option_idx].opt_expand_cb(&آرگومانها, رقم_Matches, matches);

    vim_free(buf);
    أرجع رقم_ret;
}

/*
 * Expansion handler for :set-=
 */
    صحيح
Expand_Setting_تفریق(
    expand_T	*xp,
    regmatch_T	*regmatch,
    صحيح		*رقم_Matches,
    char_u	***matches)
{
    إذا (expand_option_idx < 0)
	// term option
	أرجع ExpandOldSetting(رقم_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    إذا (option_flags & P_رقم)
	أرجع ExpandOldSetting(رقم_Matches, matches);
    وإلا إذا (option_flags & P_COMMA)
    {
	// Split the option by comma, then present each option to the user if
	// it matches the pattern.
	// This condition needs to go first, because 'whichwrap' has both
	// P_COMMA and P_FLAGLIST.
	garray_T    ga;

	char_u	    *قلم;
	char_u	    *option_copy;
	char_u	    *بعدی_val;
	char_u	    *comma;

	إذا (*option_val == NUL)
	    أرجع FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	إذا (option_copy == عدم)
	    أرجع FAIL;
	بعدی_val = option_copy;

	ga_init2(&ga, حجم(char_u *), 10);

	افعل
	{
	    قلم = بعدی_val;
	    comma = vim_strchr(بعدی_val, ',');
	    طالما (comma != عدم && comma != بعدی_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    إذا (comma != عدم)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		بعدی_val = comma + 1;
	    }
	    وإلا
		بعدی_val = عدم;

	    إذا (*قلم == NUL)
		// empty value, don't add to list
		استمر;

	    إذا (!vim_regexec(regmatch, قلم, (colnr_T)0))
		استمر;

	    char_u *buf = escape_option_str_cmdline(قلم);
	    إذا (buf == عدم)
	    {
		vim_free(option_copy);
		ga_clear_رشتهها(&ga);
		أرجع FAIL;
	    }
	    إذا (ga_جمع_کن_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		اكسر;
	    }
	} طالما (بعدی_val != عدم);

	vim_free(option_copy);

	*matches = ga.ga_داده;
	*رقم_Matches = ga.ga_len;
	أرجع OK;
    }
    وإلا إذا (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	إذا (*xp->xp_الگو != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    أرجع FAIL;
	}

	size_t رقم_flags = STRLEN(option_val);
	إذا (رقم_flags == 0)
	    أرجع FAIL;

	*matches = ALLOC_MULT(char_u *, رقم_flags + 1);
	إذا (*matches == عدم)
	    أرجع FAIL;

	صحيح شمار = 0;
	char_u *p;

	p = vim_strnsave(option_val, رقم_flags);
	إذا (p == عدم)
	{
	    VIM_CLEAR(*matches);
	    أرجع FAIL;
	}
	(*matches)[شمار++] = p;

	إذا (رقم_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    لكل (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		إذا (p == عدم)
		    اكسر;
		(*matches)[شمار++] = p;
	    }
	}

	*رقم_Matches = شمار;
	أرجع OK;
    }

    أرجع ExpandOldSetting(رقم_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    ساكن فراغ
option_value2string(
    بنية vimoption	*opp,
    صحيح			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    إذا (opp->flags & P_رقم)
    {
	دراز wc = 0;

	إذا (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_کلید_name((صحيح)wc, 0));
	وإلا إذا (wc != 0)
	    STRCPY(NameBuff, transchar((صحيح)wc));
	وإلا
	    sprintf((حرف *)NameBuff, "%ld", *(دراز *)varp);
    }
    وإلا    // P_STRING
    {
	varp = *(char_u **)(varp);
	إذا (varp == عدم)		    // just in case
	    NameBuff[0] = NUL;
#اگر_تعریف_شده FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	وإلا إذا (opp->var == (char_u *)&p_کلید && *varp)
	    STRCPY(NameBuff, "*****");
#پایان_اگر
	وإلا إذا (opp->flags & P_EXPAND)
	    home_replace(عدم, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	وإلا إذا ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	وإلا
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    ساكن صحيح
wc_use_keyname(char_u *varp, دراز *wcp)
{
    إذا (((دراز *)varp == &p_wc) || ((دراز *)varp == &p_wcm))
    {
	*wcp = *(دراز *)varp;
	إذا (IS_SPECIAL(*wcp) || پیدا_کن_special_کلید_درون_جدول((صحيح)*wcp) >= 0)
	    أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    صحيح
shortmess(صحيح x)
{
    أرجع p_shm != عدم &&
	    (   vim_strchr(p_shm, x) != عدم
	    || (vim_strchr(p_shm, 'a') != عدم
		&& vim_strchr((char_u *)SHM_A, x) != عدم));
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 * When "fname" is not NULL, use it to set $"envname" when it wasn't set yet.
 */
    فراغ
vimrc_یافت(char_u *fname, char_u *envname)
{
    صحيح		opt_idx;
    صحيح		dofree = FALSE;
    char_u	*p;

    إذا (!option_was_مجموعه((char_u *)"cp"))
    {
	p_cp = FALSE;
	لكل (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    إذا (!(options[opt_idx].flags & (P_WAS_مجموعه|P_VI_DEF)))
		مجموعه_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    إذا (fname != عدم)
    {
	p = vim_getenv(envname, &dofree);
	إذا (p == عدم)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = پر_Name_save(fname, FALSE);
	    إذا (p != عدم)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	وإلا إذا (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    فراغ
change_compatible(صحيح on)
{
    صحيح	    opt_idx;

    إذا (p_cp != on)
    {
	p_cp = on;
	compatible_مجموعه();
    }
    opt_idx = findoption((char_u *)"cp");
    إذا (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_مجموعه;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    صحيح
option_was_مجموعه(char_u *name)
{
    صحيح idx;

    idx = findoption(name);
    إذا (idx < 0)	// unknown option
	أرجع FALSE;
    إذا (options[idx].flags & P_WAS_مجموعه)
	أرجع TRUE;
    أرجع FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    صحيح
reset_option_was_مجموعه(char_u *name)
{
    صحيح idx = findoption(name);

    إذا (idx < 0)
	أرجع FAIL;

    options[idx].flags &= ~P_WAS_مجموعه;
    أرجع OK;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    ساكن فراغ
compatible_مجموعه(فراغ)
{
    صحيح	    opt_idx;

    لكل (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	إذا (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    مجموعه_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    صحيح
can_bs(
    صحيح		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#اگر_تعریف_شده FEAT_کار_CHANNEL
    إذا (what == BS_شروع && bt_prompt(curbuf))
	أرجع FALSE;
#پایان_اگر
    بدل (*p_bs)
    {
	حالة '3':       أرجع TRUE;
	حالة '2':	أرجع (what != BS_NOSTOP);
	حالة '1':	أرجع (what != BS_شروع);
	حالة '0':	أرجع FALSE;
    }
    أرجع vim_strchr(p_bs, what) != عدم;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    دراز
get_scrolloff_مقدار(فراغ)
{
    أرجع curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    دراز
get_scrolloffpad_مقدار(فراغ)
{
    أرجع curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    دراز
get_sidescrolloff_مقدار(فراغ)
{
    أرجع curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    بیعلامت صحيح
get_bkc_flags(buf_T *buf)
{
    أرجع buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#إذا defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_مقدار(buf_T *buf)
{
    إذا (buf->b_p_flp == عدم || *buf->b_p_flp == NUL)
	أرجع p_flp;
    أرجع buf->b_p_flp;
}
#پایان_اگر

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    بیعلامت صحيح
get_ve_flags(فراغ)
{
    أرجع (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#إذا defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_مقدار(win_T *win)
{
    إذا (win->w_p_sbr == عدم || *win->w_p_sbr == NUL)
	أرجع p_sbr;
    إذا (STRCMP(win->w_p_sbr, "NONE") == 0)
	أرجع خالی_option;
    أرجع win->w_p_sbr;
}
#پایان_اگر

#إذا defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(صحيح bufopt)
{
    dict_T	*d;
    صحيح		opt_idx;

    d = dict_alloc();
    إذا (d == عدم)
	أرجع عدم;

    لكل (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	بنية vimoption *opt = &options[opt_idx];

	إذا ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    إذا (varp != عدم)
	    {
		إذا (opt->flags & P_STRING)
		    dict_جمع_کن_string(d, opt->fullname, *(char_u **)varp);
		وإلا إذا (opt->flags & P_رقم)
		    dict_جمع_کن_عدد(d, opt->fullname, *(دراز *)varp);
		وإلا
		    dict_جمع_کن_عدد(d, opt->fullname, *(صحيح *)varp);
	    }
	}
    }

    أرجع d;
}
#پایان_اگر

#إذا defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    صحيح
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    إذا (val == عدم)
	p = wp->w_p_culopt;
    وإلا
	p = val;
    طالما (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	إذا (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_سطر;
	}
	وإلا إذا (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_سطر | CULOPT_NBR;
	}
	وإلا إذا (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	وإلا إذا (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	إذا (*p != ',' && *p != NUL)
	    أرجع FAIL;
	إذا (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    إذا ((culopt_flags_new & CULOPT_سطر) && (culopt_flags_new & CULOPT_SCRLINE))
	أرجع FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    أرجع OK;
}
#پایان_اگر

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    صحيح
magic_isset(فراغ)
{
    بدل (magic_overruled)
    {
	حالة OPTION_MAGIC_ON:      أرجع TRUE;
	حالة OPTION_MAGIC_OFF:     أرجع FALSE;
	حالة OPTION_MAGIC_NOT_مجموعه: اكسر;
    }
#اگر_تعریف_شده FEAT_EVAL
    إذا (درون_vim9script())
	أرجع TRUE;
#پایان_اگر
    أرجع p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    صحيح
option_مجموعه_callback_func(char_u *optval استفادهنشده, callback_T *optcb استفادهنشده)
{
#اگر_تعریف_شده FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    صحيح		funcname = FALSE;

    إذا (optval == عدم || *optval == NUL)
    {
	free_callback(optcb);
	أرجع OK;
    }

    إذا (*optval == '{' || (درون_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_عبارت(optval, عدم);
    وإلا
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    إذا (tv == عدم)
	أرجع FAIL;

    cb = get_callback(tv);
    إذا (cb.cb_name == عدم || *cb.cb_name == NUL)
    {
	free_tv(tv);
	أرجع FAIL;
    }

    free_callback(optcb);
    مجموعه_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = عدم;
    إذا (درون_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != عدم))
    {
	إذا (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = پیدا_کن_imported(optval, dot - optval, FALSE);
	إذا (import != عدم && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((حرف *)fnamebuf, حجم(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	وإلا
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

    أرجع OK;
#وإلا
    أرجع FAIL;
#پایان_اگر
}

#إذا defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    حرف *
did_مجموعه_showtabpanel(optset_T *آرگومانها استفادهنشده)
{
    shell_new_columns();
    أرجع عدم;
}
#پایان_اگر

#إذا defined(FEAT_EVAL)
    ساكن فراغ
didset_options_sctx(صحيح opt_flags, حرف **buf)
{
	لكل (صحيح i = 0; ; ++i)
	{
	    إذا (buf[i] == عدم)
		اكسر;

	    صحيح idx = findoption((char_u *)buf[i]);
	    إذا (idx >= 0)
		مجموعه_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#پایان_اگر
