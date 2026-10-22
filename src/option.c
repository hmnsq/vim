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

#تعریف_کرو اندر_OPTION_C
#شامل "vim.h"
#شامل "optiondefs.h"

ساکن خالی سیٹ_options_default(صحیح opt_flags);
ساکن خالی سیٹ_string_default_esc(حرف *name, char_u *val, صحیح escape);
ساکن char_u *ڈھونڈو_dup_شے(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
ساکن char_u *option_expand(صحیح opt_idx, char_u *val);
ساکن خالی didset_options(خالی);
ساکن خالی didset_options2(خالی);
#اگر defined(FEAT_EVAL)
ساکن long_u *insecure_flag(win_T *wp, صحیح opt_idx, صحیح opt_flags);
#ورنہ
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#اگر_ختم
ساکن حرف *سیٹ_bool_option(صحیح opt_idx, char_u *varp, صحیح قدر, صحیح opt_flags);
ساکن حرف *سیٹ_عدد_option(صحیح opt_idx, char_u *varp, طویل قدر, حرف *errbuf, size_t errbuflen, صحیح opt_flags);
ساکن صحیح ڈھونڈو_کلید_option(char_u *arg_arg, صحیح has_lt);
ساکن خالی showoptions(صحیح all, صحیح opt_flags);
ساکن صحیح optval_default(ساخت vimoption *, char_u *varp, صحیح compatible);
ساکن خالی showoneopt(ساخت vimoption *, صحیح opt_flags);
ساکن صحیح put_setstring(FILE *fd, bool legacy, حرف *cmd, حرف *name, char_u **valuep, long_u flags);
ساکن صحیح put_setnum(FILE *fd, حرف *cmd, حرف *name, طویل *valuep);
ساکن صحیح put_setbool(FILE *fd, حرف *cmd, حرف *name, صحیح قدر);
ساکن صحیح istermoption(ساخت vimoption *p);
ساکن صحیح istermoption_idx(صحیح opt_idx);
ساکن char_u *get_varp_scope(ساخت vimoption *p, صحیح scope);
ساکن char_u *get_varp(ساخت vimoption *);
ساکن خالی check_win_options(win_T *win);
ساکن خالی option_value2string(ساخت vimoption *, صحیح scope);
ساکن خالی check_winopt(winopt_T *wop);
ساکن صحیح wc_use_keyname(char_u *varp, طویل *wcp);
ساکن خالی compatible_سیٹ(خالی);

#اگر defined(FEAT_EVAL)
ساکن حرف *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", تہی};
ساکن حرف *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    تہی};
ساکن خالی didset_options_sctx(صحیح opt_flags, حرف **buf);
#اگر_ختم


/*
 * Initialize the 'shell' option to a default value.
 */
    ساکن خالی
سیٹ_init_default_shell(خالی)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    اگر (((p = mch_getenv((char_u *)"SHELL")) != تہی && *p != NUL)
#اگر defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != تہی && *p != NUL)
	    || ((p = (char_u *)default_shell()) != تہی && *p != NUL)
#اگر_ختم
	    )
#اگر defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	اگر (vim_strchr(p, ' ') != تہی)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    اگر (cmd != تہی)
	    {
		vim_snprintf((حرف *)cmd, len, "\"%s\"", p);
		سیٹ_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	ورنہ
	    سیٹ_string_default("sh", p);
    }
#ورنہ
	سیٹ_string_default_esc("sh", p, TRUE);
#اگر_ختم
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    ساکن خالی
سیٹ_init_default_backupskip(خالی)
{
    صحیح		opt_idx;
    صحیح		i;
    char_u	*p;
    صحیح		plen;
#اگر_تعریف_شدہ UNIX
    ساکن حرف	*(نام[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#ورنہ
    ساکن حرف	*(نام[3]) = {"TMPDIR", "TEMP", "TMP"};
#اگر_ختم
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(نام); ++i)
    {
	صحیح		mustfree = FALSE;
#اگر_تعریف_شدہ UNIX
	اگر (*نام[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (صحیح)STRLEN_LITERAL("/private/tmp");
# ورنہ
	    p = (char_u *)"/tmp";
	    plen = (صحیح)STRLEN_LITERAL("/tmp");
# endif
	}
	ورنہ
#اگر_ختم
	{
	    p = vim_getenv((char_u *)نام[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	اگر (p != تہی && *p != NUL)
	{
	    char_u  *شے;
	    size_t  itemsize;
	    صحیح	    has_trailing_راستہ_sep = FALSE;

	    اگر (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (صحیح)STRLEN(p);
		// does the value include a trailing path separator?
		اگر (after_pathsep(p, p + plen))
		    has_trailing_راستہ_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_راستہ_sep ? 0 : 1) + 2;
	    شے = alloc(itemsize);
	    اگر (شے != تہی)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((حرف *)شے, itemsize, "%s%s*", p, (has_trailing_راستہ_sep) ? "" : PATHSEPSTR);

		اگر (ڈھونڈو_dup_شے(ga.ga_ڈیٹا, شے, itemlen, options[opt_idx].flags) == تہی
			&& ga_grow(&ga, (صحیح)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((حرف *)ga.ga_ڈیٹا + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", شے);
		}
		vim_free(شے);
	    }
	}
	اگر (mustfree)
	    vim_free(p);
    }
    اگر (ga.ga_ڈیٹا != تہی)
    {
	سیٹ_string_default("bsk", ga.ga_ڈیٹا);
	vim_free(ga.ga_ڈیٹا);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    ساکن خالی
سیٹ_init_default_maxmemtot(خالی)
{
    صحیح		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    اگر (opt_idx < 0)
	واپس;

#اگر !defined(HAVE_AVAIL_MEM) && !defined(HAVE_کل_MEM)
    اگر (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#اگر_ختم
    {
#اگر defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#وگرنہ_اگر defined(HAVE_کل_MEM)
	// Use amount of memory available to Vim.
	n = (mch_کل_mem(FALSE) >> 1);
#ورنہ
	n = (0x7fffffff >> 11);
#اگر_ختم
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	اگر (opt_idx >= 0)
	{
#اگر !defined(HAVE_AVAIL_MEM) && !defined(HAVE_کل_MEM)
	    اگر ((طویل)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (طویل)n
		    || (طویل)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#اگر_ختم
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    ساکن خالی
سیٹ_init_default_cdpath(خالی)
{
    صحیح		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    صحیح	i;
    صحیح	j;
    صحیح	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    اگر (cdpath == تہی)
	واپس;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    اگر (buf != تہی)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	برائے (i = 0; cdpath[i] != NUL; ++i)
	{
	    اگر (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    ورنہ
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
	ورنہ
	    vim_free(buf); // cannot happen
    }
    اگر (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    ساکن خالی
سیٹ_init_default_printencoding(خالی)
{
#اگر defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    سیٹ_string_default("penc",
# اگر defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# ورنہ // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#اگر_ختم
}

#اگر defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    ساکن خالی
سیٹ_init_default_printexpr(خالی)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    سیٹ_string_default("pexpr",
# اگر defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# ورنہ
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#اگر_ختم

#اگر_تعریف_شدہ UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    ساکن خالی
سیٹ_init_restricted_منوال(خالی)
{
    char_u	*p;

    p = get_isolated_shell_name();
    اگر (p == تہی)
	واپس;
    اگر (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#اگر_ختم

#اگر_تعریف_شدہ CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    ساکن خالی
سیٹ_init_clean_rtp(خالی)
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
	واپس;
    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
    p_pp = (char_u *)CLEAN_RUNTIMEPATH;
}
#اگر_ختم

#اگر_تعریف_شدہ UNIX
/*
 * Change 'runtimepath' and 'packdir' to '$XDG_CONFIG_HOME/vim' if the only
 * vimrc found is located in '$XDG_CONFIG_HOME/vim/vimrc'.
 * In case the '$XDG_CONFIG_HOME' variable is not set, '$HOME/.config' is used
 * as a fallback as is defined in the XDG base dir specification:
 * <https://specifications.freedesktop.org/basedir/latest/>
 */
    ساکن خالی
سیٹ_init_xdg_rtp(خالی)
{
    صحیح		opt_idx;
    صحیح		has_xdg_env = TRUE;
    صحیح		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = تہی;
    char_u	*vimrc2 = تہی;
    char_u	*xdg_dir = تہی;
    char_u	*xdg_rtp = تہی;
    string_T	vimrc_xdg = {تہی, 0};

    // initialize chartab, so we can expand $HOME
    (خالی)init_chartab();
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
	جاؤ theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    اگر ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	جاؤ theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    اگر ((opt_idx = findoption((char_u *)"packpath")) < 0)
	جاؤ theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# اگر defined(XDG_VDIR) && defined(FEAT_SESSION)
    اگر ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	جاؤ theend;

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
#اگر_ختم

/*
 * Expand environment variables and things like "~" for the defaults.
 * If option_expand() returns non-NULL the variable is expanded.  This can
 * only happen for non-indirect options.
 * Also set the default to the expanded value, so ":set" does not list
 * them.
 * Don't set the P_ALLOCED flag, because we don't want to free the
 * default.
 */
    ساکن خالی
سیٹ_init_expand_env(خالی)
{
    صحیح		opt_idx;
    char_u	*p;

    برائے (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	اگر ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != تہی)
	    p = (char_u *)_(*(حرف **)options[opt_idx].var);
	ورنہ
	    p = option_expand(opt_idx, تہی);
	اگر (p != تہی && (p = vim_strsave(p)) != تہی)
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
    ساکن char_u *
get_ui_langs(خالی)
{
    ULONG	عدد_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*بفر;
    char_u	*langs = تہی;

    اگر (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &عدد_languages, تہی,
								    &bufsize)
	    || bufsize == 0)
	واپس تہی;

    بفر = ALLOC_MULT(WCHAR, bufsize);
    اگر (بفر == تہی)
	واپس تہی;

    اگر (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &عدد_languages, بفر,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	اگر (langs != تہی)
	{
	    char_u	*d = langs;
	    WCHAR	*s = بفر;

	    جب (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		اگر (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		برائے ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		اگر (english)
		    توڑو;
	    }
	    *d = NUL;
	}
    }
    vim_free(بفر);

    واپس langs;
}
#اگر_ختم

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    ساکن خالی
سیٹ_init_lang_env(خالی)
{
#اگر defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    اگر (mch_getenv((char_u *)"LANG") == تہی
	    && mch_getenv((char_u *)"LANGUAGE") == تہی
	    && mch_getenv((char_u *)"LC_ALL") == تہی
	    && mch_getenv((char_u *)"LC_MESSAGES") == تہی)
    {
	char_u	*langs = get_ui_langs();

	اگر (langs != تہی && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    اگر (colon != تہی)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#وگرنہ_اگر defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#اگر_ختم
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    ساکن خالی
سیٹ_init_default_encoding(خالی)
{
    char_u	*p;
    صحیح		opt_idx;

#اگر defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#ورنہ
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#اگر_ختم
    اگر (p == تہی)
	واپس;

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
    اگر (mb_init() == تہی)
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
	    سیٹ_string_option_براہ_راست((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    سیٹ_string_option_براہ_راست((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    اگر (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    اگر (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (خالی)init_chartab();
	}
#اگر_ختم

#اگر defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	اگر (
# ifdef VIMDLL
		(!gui.اندر_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    حرف	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    اگر (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, حجم(buf), "cp%ld", (طویل)GetACP());
	    ورنہ
		buflen = vim_snprintf(buf, حجم(buf), "cp%ld", (طویل)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    اگر (p_tenc != تہی)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		اگر (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_تیاری(&داخلہ_conv, p_tenc, p_enc);
		convert_تیاری(&output_conv, p_enc, p_tenc);
	    }
	    ورنہ
		p_tenc = خالی_خانے_option;
	}
#اگر_ختم
#اگر defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#اگر_ختم
    }
    ورنہ
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
    خالی
سیٹ_init_1(صحیح clean_arg)
{
#اگر_تعریف_شدہ FEAT_LANGMAP
    langmap_init();
#اگر_ختم

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    اگر (mch_getenv((char_u *)"VIM_POSIX") != تہی)
    {
	سیٹ_string_default("cpo", (char_u *)CPO_ALL);
	سیٹ_string_default("shm", (char_u *)SHM_POSIX);
    }

    سیٹ_init_default_shell();
    سیٹ_init_default_backupskip();
    سیٹ_init_default_maxmemtot();
    سیٹ_init_default_cdpath();
    سیٹ_init_default_printencoding();
#اگر defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    سیٹ_init_default_printexpr();
#اگر_ختم

    /*
     * Set all the options (except the terminal options) to their default
     * value.  Also set the global value for local options.
     */
    سیٹ_options_default(0);

#اگر_تعریف_شدہ UNIX
    سیٹ_init_xdg_rtp();
    سیٹ_init_restricted_منوال();
#اگر_ختم

#اگر_تعریف_شدہ CLEAN_RUNTIMEPATH
    اگر (clean_arg)
	سیٹ_init_clean_rtp();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    اگر (ملا_الٹ_arg)
	سیٹ_option_قدر_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
#اگر_ختم

    curbuf->b_p_initialized = true;
    curbuf->b_p_ac = -1;
    curbuf->b_p_ar = -1;	// no local 'autoread' value
#اگر_تعریف_شدہ HAVE_FSYNC
    curbuf->b_p_fs = -1;	// no local 'fsync' value
#اگر_ختم
    curbuf->b_p_ul = NO_LOCAL_UNDOLEVEL;
    check_buf_options(curbuf);
    check_win_options(curwin);
    check_options();

    // Must be before option_expand(), because that one needs vim_isIDc()
    didset_options();

#اگر_تعریف_شدہ FEAT_SPELL
    // Use the current chartab for the generic chartab. This is not in
    // didset_options() because it only depends on 'encoding'.
    init_spell_chartab();
#اگر_ختم

    سیٹ_init_default_encoding();

    // Expand environment variables and things like "~" for the defaults.
    سیٹ_init_expand_env();

    save_file_ff(curbuf);	// Buffer is unchanged

#اگر defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    اگر (mch_getenv((char_u *)"MLTERM") != تہی)
	سیٹ_option_قدر_give_err((char_u *)"tbidi", 1L, تہی, 0);
#اگر_ختم

    didset_options2();

    سیٹ_init_lang_env();

#اگر_تعریف_شدہ FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    سیٹ_helplang_default(get_mess_lang());
#اگر_ختم
}

ساکن char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    خالی
سیٹ_fencs_unicode(خالی)
{
    سیٹ_string_option_براہ_راست((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    ساکن خالی
سیٹ_option_default(
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
    اگر (varp != تہی)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	اگر (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    اگر (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		سیٹ_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    ورنہ اگر (options[opt_idx].indir != PV_NONE)
		سیٹ_string_option_براہ_راست(تہی, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    ورنہ
	    {
		اگر ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	ورنہ اگر (flags & P_عدد)
	{
	    اگر (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    ورنہ
	    {
		طویل def_val = (طویل)(long_i)options[opt_idx].def_val[dvi];

		اگر ((طویل *)varp == &curwin->w_p_so
			|| (طویل *)varp == &curwin->w_p_siso
			|| (طویل *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(طویل *)varp = -1;
		ورنہ
		    *(طویل *)varp = def_val;
		// May also set global value for local option.
		اگر (both)
		    *(طویل *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	ورنہ	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(صحیح *)varp = (صحیح)(طویل)(long_i)options[opt_idx].def_val[dvi];
#اگر_تعریف_شدہ UNIX
	    // 'modeline' defaults to off for root
	    اگر (options[opt_idx].indir == PV_ML && getuid() == جڑ_UID)
		*(صحیح *)varp = FALSE;
#اگر_ختم
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

#اگر_تعریف_شدہ FEAT_EVAL
    سیٹ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#اگر_ختم
}

/*
 * Set all options (except terminal options) to their default value.
 * When "opt_flags" is non-zero skip 'encoding'.
 */
    ساکن خالی
سیٹ_options_default(
    صحیح		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    صحیح		i;
    win_T	*wp;
    tabpage_T	*tp;

    برائے (i = 0; !istermoption_idx(i); i++)
	اگر (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#اگر defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_کلید
#اگر_ختم
			)))
	    سیٹ_option_default(i, opt_flags, p_cp);

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
    ساکن خالی
سیٹ_string_default_esc(حرف *name, char_u *val, صحیح escape)
{
    char_u	*p;
    صحیح		opt_idx;

    اگر (escape && vim_strchr(val, ' ') != تہی)
	p = vim_strsave_escaped(val, (char_u *)" ");
    ورنہ
	p = vim_strsave(val);
    اگر (p == تہی)		// we don't want a NULL
	واپس;

    opt_idx = findoption((char_u *)name);
    اگر (opt_idx < 0)
    {
	vim_free(p);
	واپس;
    }

    اگر (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    خالی
سیٹ_string_default(حرف *name, char_u *val)
{
    سیٹ_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    ساکن char_u *
ڈھونڈو_dup_شے(char_u *origval, char_u *newval, size_t newvallen, long_u flags)
{
    صحیح	    bs = 0;
    char_u  *s;

    اگر (origval == تہی)
	واپس تہی;

    برائے (s = origval; *s != NUL; ++s)
    {
	اگر ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    واپس s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	اگر ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	ورنہ
	    bs = 0;
    }
    واپس تہی;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    خالی
سیٹ_نمبر_default(حرف *name, طویل val)
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
    خالی
سیٹ_local_options_default(win_T *wp, صحیح do_بفر)
{
    win_T	*save_curwin = curwin;
    صحیح		i;

    curwin = wp;
    curbuf = curwin->w_بفر;
    بلاک_autocmds();

    برائے (i = 0; !istermoption_idx(i); i++)
    {
	ساخت vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	اگر (p->indir != PV_NONE
		&& (do_بفر || (p->indir & PV_BUF) == 0)
		&& !(options[i].flags & P_NODEFAULT)
		&& !optval_default(p, varp, FALSE))
	    سیٹ_option_default(i, OPT_FREE|OPT_LOCAL, FALSE);
    }

    unblock_autocmds();
    curwin = save_curwin;
    curbuf = curwin->w_بفر;
}
#اگر_ختم

#اگر defined(EXITFREE)
/*
 * Free all options.
 */
    خالی
free_all_options(خالی)
{
    صحیح		i;

    برائے (i = 0; !istermoption_idx(i); i++)
    {
	اگر (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    اگر ((options[i].flags & P_ALLOCED) && options[i].var != تہی)
		free_string_option(*(char_u **)options[i].var);
	    اگر (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	ورنہ اگر (options[i].var != VAR_WIN
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
#اگر_ختم


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    خالی
سیٹ_init_2(خالی)
{
    صحیح		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    اگر (idx >= 0 && !(options[idx].flags & P_WAS_سیٹ))
	سیٹ_option_default(idx, OPT_LOCAL, p_cp);
    comp_ستون();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    اگر (!option_was_سیٹ((char_u *)"window"))
	p_کھڑکی = صفیں - 1;
    سیٹ_نمبر_default("window", صفیں - 1);

    // For DOS console the default is always black.
#اگر !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    اگر (idx >= 0 && !(options[idx].flags & P_WAS_سیٹ)
						 && *رکن_bg_default() == 'd')
    {
	سیٹ_string_option_براہ_راست(تہی, idx, (char_u *)"dark", OPT_FREE, 0);
	// don't mark it as set, when starting the GUI it may be
	// changed again
	options[idx].flags &= ~P_WAS_سیٹ;
    }
#اگر_ختم

#اگر_تعریف_شدہ CURSOR_SHAPE
    parse_shape_opt(SHAPE_CURSOR);	// set cursor shapes from 'guicursor'
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
    parse_shape_opt(SHAPE_MOUSE);	// set mouse shapes from 'mouseshape'
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PRINTER
    (خالی)parse_printoptions(تہی);	// parse 'printoptions' default value
#اگر_ختم
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    خالی
سیٹ_init_3(خالی)
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
    ورنہ
	do_srr = !(options[idx_srr].flags & P_WAS_سیٹ);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    اگر (idx_sp < 0)
	do_sp = FALSE;
    ورنہ
	do_sp = !(options[idx_sp].flags & P_WAS_سیٹ);
# endif
    p = get_isolated_shell_name();
    اگر (p != تہی)
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
#  ورنہ
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
	ورنہ اگر (   fnamecmp(p, "powershell") == 0
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
	ورنہ
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
#  ورنہ
		    اگر (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    ورنہ
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
#اگر_ختم

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
    اگر (لڑی_تلاش((حرف *)gettail(p_sh), "powershell") != تہی)
    {
	صحیح	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	اگر (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_سیٹ))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	اگر (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_سیٹ))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    ورنہ اگر (لڑی_تلاش((حرف *)gettail(p_sh), "sh") != تہی)
    {
	صحیح	idx3;

	idx3 = findoption((char_u *)"shcf");
	اگر (idx3 >= 0 && !(options[idx3].flags & P_WAS_سیٹ))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	اگر (idx3 >= 0 && !(options[idx3].flags & P_WAS_سیٹ))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    ورنہ اگر (لڑی_تلاش((حرف *)gettail(p_sh), "cmd.exe") != تہی)
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
	اگر (idx3 >= 0 && !(options[idx3].flags & P_WAS_سیٹ))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	اگر (idx3 >= 0 && !(options[idx3].flags & P_WAS_سیٹ))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#اگر_ختم

    اگر (BUFEMPTY())
    {
	صحیح idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	اگر (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_سیٹ))
	    سیٹ_fileformat(default_fileformat(), OPT_LOCAL);
    }

    سیٹ_title_defaults();
}

#اگر defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    خالی
سیٹ_helplang_default(char_u *lang)
{
    صحیح		idx;
    size_t	langlen;

    اگر (lang == تہی)	// safety check
	واپس;

    langlen = STRLEN(lang);
    اگر (langlen < 2)	// safety check
	واپس;

    idx = findoption((char_u *)"hlg");
    اگر (idx < 0 || (options[idx].flags & P_WAS_سیٹ))
	واپس;

    اگر (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    اگر (p_hlg == تہی)
	p_hlg = خالی_خانے_option;
    ورنہ
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	اگر (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	ورنہ اگر (langlen >= 1 && *p_hlg == 'C')
	{
	    p_hlg[0] = 'e';
	    p_hlg[1] = 'n';
	}
	p_hlg[2] = NUL;
    }
    options[idx].flags |= P_ALLOCED;
}
#اگر_ختم

/*
 * 'title' and 'icon' only default to true if they have not been set or reset
 * in .vimrc and we can read the old value.
 * When 'title' and 'icon' have been reset in .vimrc, we won't even check if
 * they can be reset.  This reduces startup time when using X on a remote
 * machine.
 */
    خالی
سیٹ_title_defaults(خالی)
{
    صحیح	    idx1;
    طویل    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    اگر (idx1 >= 0 && !(options[idx1].flags & P_WAS_سیٹ))
    {
#اگر_تعریف_شدہ FEAT_GUI
	اگر (gui.starting || gui.اندر_use)
	    val = TRUE;
	ورنہ
#اگر_ختم
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    اگر (idx1 < 0 || (options[idx1].flags & P_WAS_سیٹ))
    {
	واپس;
    }

#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.starting || gui.اندر_use)
	val = TRUE;
    ورنہ
#اگر_ختم
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    خالی
ex_سیٹ(exarg_T *eap)
{
    صحیح		flags = 0;

    اگر (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    ورنہ اگر (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#اگر defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    اگر ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    ورنہ
#اگر_ختم
    {
	اگر (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(خالی)do_سیٹ(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
قسمتعریف شمار {
    سابقہ_NO = 0,	// "no" prefix
    سابقہ_NONE,	// no prefix
    سابقہ_INV,		// "inv" prefix
} سیٹ_سابقہ_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    ساکن سیٹ_سابقہ_T
get_option_سابقہ(char_u **argp)
{
    صحیح		سابقہ = سابقہ_NONE;
    char_u	*arg = *argp;

    اگر (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	سابقہ = سابقہ_NO;
	arg += 2;
    }
    ورنہ اگر (STRNCMP(arg, "inv", 3) == 0)
    {
	سابقہ = سابقہ_INV;
	arg += 3;
    }

    *argp = arg;
    واپس سابقہ;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    ساکن صحیح
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
	ورنہ
	{
	    len = 1;
	    جب (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	اگر (arg[len] != '>')
	    واپس FAIL;

	arg[len] = NUL;			    // put NUL after name
	اگر (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	اگر (opt_idx == -1)
	    کلید = ڈھونڈو_کلید_option(arg + 1, TRUE);
    }
    ورنہ
    {
	صحیح	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	اگر (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	ورنہ
	    جب (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	اگر (opt_idx == -1)
	    کلید = ڈھونڈو_کلید_option(arg, FALSE);
    }

    *keyp = کلید;
    *lenp = len;
    *opt_idxp = opt_idx;

    واپس OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    ساکن سیٹ_op_T
get_opt_op(char_u *arg)
{
    سیٹ_op_T op = OP_NONE;

    اگر (*arg != NUL && *(arg + 1) == '=')
    {
	اگر (*arg == '+')
	    op = OP_ADDING;		// "+="
	ورنہ اگر (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	ورنہ اگر (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    واپس op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
ساکن حرف *modeline_whitelist[] =
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
    تہی
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    ساکن bool
is_modeline_whitelisted(حرف *name)
{
    برائے (صحیح i = 0; modeline_whitelist[i] != تہی; i++)
	اگر (STRCMP(name, modeline_whitelist[i]) == 0)
	    واپس true;
    واپس false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    ساکن صحیح
validate_opt_idx(
	صحیح opt_idx,
	صحیح opt_flags,
	long_u flags,
	حرف **errmsg,
	سیٹ_سابقہ_T سابقہ)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    اگر ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	واپس FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    اگر ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	واپس FAIL;

    // Disallow changing some options from modelines.
    اگر (opt_flags & OPT_MODELINE)
    {
	اگر (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_اندر_modeline;
	    واپس FAIL;
	}
	اگر ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_اندر_modeline_when_modelineexpr_is_off;
	    واپس FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	اگر (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    اگر (options[opt_idx].indir == PV_ML && سابقہ == سابقہ_NO)
		واپس OK;
	    ورنہ اگر (!is_modeline_whitelisted(options[opt_idx].fullname))
		واپس FAIL;
	}
#اگر_تعریف_شدہ FEAT_DIFF
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
	    واپس FAIL;
#اگر_ختم
    }

#اگر_تعریف_شدہ HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    اگر (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_اندر_sandbox;
	واپس FAIL;
    }
#اگر_ختم

    واپس OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    ساکن char_u *
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
#اگر_تعریف_شدہ FEAT_GUI
	اگر (gui.اندر_use)
	    newval = gui_bg_default();
	ورنہ
#اگر_ختم
	    newval = رکن_bg_default();
    }
    ورنہ اگر ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    اگر (newval == تہی)
	newval = خالی_خانے_option;
    ورنہ
    {
	char_u *s = option_expand(opt_idx, newval);
	اگر (s == تہی)
	    s = newval;
	newval = vim_strsave(s);
    }

    واپس newval;
}

/*
 * Convert the 'backspace' option number value to a string: for adding,
 * prepending and removing string.
 */
    ساکن خالی
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
	    *(char_u **)varp = خالی_خانے_option;
	    توڑو;
	حالت 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    توڑو;
	حالت 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    توڑو;
	حالت 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    توڑو;
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
    ساکن char_u *
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

    واپس whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    ساکن char_u *
stropt_copy_قدر(
    char_u	*origval,
    char_u	**argp,
    سیٹ_op_T	op,
    صحیح		flags غیر_مستعمل)
{
    char_u	*arg = *argp;
    غیرعلامتی	newlen;
    char_u	*newval;
    char_u	*s = تہی;

    // get a bit too much
    newlen = (غیرعلامتی)STRLEN(arg) + 1;
    اگر (op != OP_NONE)
	newlen += (غیرعلامتی)STRLEN(origval) + 1;
    newval = alloc(newlen);
    اگر (newval == تہی)  // out of mem, don't change
	واپس تہی;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    جب (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	صحیح i;

	اگر (*arg == '\\' && arg[1] != NUL
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#اگر_ختم
	   )
	    ++arg;	// remove backslash
	اگر (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	ورنہ
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    واپس newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    ساکن char_u *
stropt_expand_envvar(
    صحیح		opt_idx,
    char_u	*origval,
    char_u	*newval,
    سیٹ_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    اگر (s == تہی)
	واپس newval;

    vim_free(newval);
    غیرعلامتی newlen = (غیرعلامتی)STRLEN(s) + 1;
    اگر (op != OP_NONE)
	newlen += (غیرعلامتی)STRLEN(origval) + 1;

    newval = alloc(newlen);
    اگر (newval == تہی)
	واپس تہی;

    STRCPY(newval, s);

    واپس newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    ساکن خالی
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    سیٹ_op_T	op,
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
    ورنہ
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
    ساکن خالی
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
	    ورنہ
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
    ساکن char_u *
ڈھونڈو_کلید_شے(char_u *src, char_u *کلید, صحیح keylen, صحیح *itemlenp)
{
    char_u  *p = src;

    جب (*p != NUL)
    {
	// Check if this item starts with the same key
	اگر ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, کلید, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *انتہا = vim_strchr(p, ',');
	    اگر (انتہا == تہی)
		انتہا = p + STRLEN(p);
	    *itemlenp = (صحیح)(انتہا - p);
	    واپس p;
	}
	++p;
    }
    واپس تہی;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    ساکن خالی
remove_comma_شے(char_u *str, char_u *شے, صحیح itemlen)
{
    اگر (شے[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(شے, شے + itemlen + 1);
    ورنہ اگر (شے > str && *(شے - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(شے - 1, شے + itemlen);
    ورنہ
	// Only item
	*شے = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    ساکن خالی
remove_کلید_شے(char_u *str, char_u *کلید, صحیح keylen, char_u *skip)
{
    صحیح	    itemlen;
    char_u  *ملا;

    جب ((ملا = ڈھونڈو_کلید_شے(str, کلید, keylen, &itemlen)) != تہی)
    {
	اگر (ملا == skip)
	{
	    // Search for the next match after this one.
	    char_u *اگلا = ملا + itemlen;
	    اگر (*اگلا == ',')
		++اگلا;
	    ملا = ڈھونڈو_کلید_شے(اگلا, کلید, keylen, &itemlen);
	    اگر (ملا == تہی)
		توڑو;
	}

	remove_comma_شے(str, ملا, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    ساکن خالی
append_شے(char_u *str, char_u *شے, صحیح شے_len)
{
    صحیح len = (صحیح)STRLEN(str);

    اگر (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, شے, (size_t)شے_len);
    str[len + شے_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    ساکن خالی
prepend_شے(char_u *str, char_u *شے, صحیح شے_len)
{
    صحیح len = (صحیح)STRLEN(str);
    صحیح comma = (len > 0) ? 1 : 0;

    mch_memmove(str + شے_len + comma, str, (size_t)len + 1);
    mch_memmove(str, شے, (size_t)شے_len);
    اگر (comma)
	str[شے_len] = ',';
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
    ساکن bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    سیٹ_op_T	op,
    صحیح		flags غیر_مستعمل)
{
    char_u  *p;
    char_u  *شے_آغاز;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    اگر (vim_strchr(newval, ':') == تہی && vim_strchr(newval, ',') == تہی)
	واپس false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    اگر (newval_copy == تہی)
	واپس false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    شے_آغاز = newval_copy;
    برائے (;;)
    {
	p = vim_strchr(شے_آغاز, ',');
	صحیح شے_len = (p == تہی)
			    ? (صحیح)STRLEN(شے_آغاز) : (صحیح)(p - شے_آغاز);

	اگر (شے_len > 0)
	{
	    char_u *colon = vim_strchr(شے_آغاز, ':');
	    اگر (colon != تہی && colon < شے_آغاز + شے_len)
	    {
		صحیح keylen = (صحیح)(colon - شے_آغاز) + 1;

		اگر (op == OP_ADDING || op == OP_PREPENDING)
		{
		    صحیح old_itemlen;
		    char_u *ملا = ڈھونڈو_کلید_شے(newval, شے_آغاز,
						       keylen, &old_itemlen);
		    اگر (ملا != تہی)
		    {
			اگر (old_itemlen == شے_len
				&& STRNCMP(ملا, شے_آغاز,
							 شے_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_کلید_شے(newval, شے_آغاز,
							   keylen, ملا);
			}
			ورنہ
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_کلید_شے(newval, شے_آغاز,
							    keylen, تہی);
			    اگر (op == OP_PREPENDING)
				prepend_شے(newval, شے_آغاز, شے_len);
			    ورنہ
				append_شے(newval, شے_آغاز, شے_len);
			}
		    }
		    ورنہ
		    {
			// New item.
			اگر (op == OP_PREPENDING)
			    prepend_شے(newval, شے_آغاز, شے_len);
			ورنہ
			    append_شے(newval, شے_آغاز, شے_len);
		    }
		}
		ورنہ اگر (op == OP_REMOVING)
		    remove_کلید_شے(newval, شے_آغاز, keylen, تہی);
	    }
	    ورنہ
	    {
		اگر (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *ملا = ڈھونڈو_dup_شے(newval, شے_آغاز,
							   شے_len, P_COMMA);
		    اگر (ملا == تہی)
		    {
			// New item.
			اگر (op == OP_PREPENDING)
			    prepend_شے(newval, شے_آغاز, شے_len);
			ورنہ
			    append_شے(newval, شے_آغاز, شے_len);
		    }
		    // else: exact duplicate — do nothing
		}
		ورنہ اگر (op == OP_REMOVING)
		{
		    char_u *ملا = ڈھونڈو_dup_شے(newval, شے_آغاز,
							   شے_len, P_COMMA);
		    اگر (ملا != تہی)
			remove_comma_شے(newval, ملا, شے_len);
		}
	    }
	}

	اگر (p == تہی)
	    توڑو;
	شے_آغاز = p + 1;
    }

    vim_free(newval_copy);

    واپس true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    ساکن خالی
stropt_remove_dupflags(char_u *newval, صحیح flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    جب (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	اگر (flags & P_ONECOMMA)
	{
	    اگر (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != تہی)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		جاری;
	    }
	}
	ورنہ
	{
	    اگر ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != تہی)
	    {
		STRMOVE(s, s + 1);
		جاری;
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
    ساکن char_u *
stropt_get_newval(
    صحیح		nextchar,
    صحیح		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    سیٹ_op_T    *op_arg,
    صحیح		flags,
    صحیح		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    سیٹ_op_T    op = *op_arg;
    char_u	*save_arg = تہی;
    char_u	*newval;
    char_u	*s = تہی;
    char_u	whichwrap[80];

    اگر (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    ورنہ اگر (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    ورنہ
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
	ورنہ اگر (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	ورنہ اگر (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	ورنہ اگر (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_قدر(origval, &arg, op, flags);
	اگر (newval == تہی)
	    جاؤ مکمل;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	اگر (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    اگر (newval == تہی)
		جاؤ مکمل;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	اگر ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	ورنہ
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    صحیح len = 0;
	    اگر (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (صحیح)STRLEN(newval);
		s = ڈھونڈو_dup_شے(origval, newval, len, flags);

		// do not add if already there
		اگر ((op == OP_ADDING || op == OP_PREPENDING) && s != تہی)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		اگر (s == تہی)
		    s = origval + (صحیح)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    اگر (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    ورنہ اگر (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	اگر (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

مکمل:
    اگر (save_arg != تہی)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    واپس newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    ساکن صحیح
do_سیٹ_option_string(
	صحیح	    opt_idx,
	صحیح	    opt_flags,
	char_u	    **argp,
	صحیح	    nextchar,
	سیٹ_op_T    op_arg,
	long_u	    flags,
	صحیح	    cp_val,
	char_u	    *varp_arg,
	حرف	    *errbuf,
	size_t	    errbuflen,
	صحیح	    *قدر_checked,
	حرف	    **errmsg)
{
    char_u	*arg = *argp;
    سیٹ_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = تہی; // previous value if *varp
    char_u	*newval;
    char_u	*origval = تہی;
    char_u	*origval_l = تہی;
    char_u	*origval_g = تہی;
#اگر defined(FEAT_EVAL)
    char_u	*saved_origval = تہی;
    char_u	*saved_origval_l = تہی;
    char_u	*saved_origval_g = تہی;
    char_u	*saved_newval = تہی;
#اگر_ختم

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
						  && origval_l == خالی_خانے_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    اگر (((صحیح)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    ورنہ
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    اگر (newval == تہی)
	*(char_u **)(varp) = خالی_خانے_option;

#اگر defined(FEAT_EVAL)
    اگر (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_کلید
# endif
		      && origval != تہی && newval != تہی)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	اگر (origval_l != تہی)
	    saved_origval_l = vim_strsave(origval_l);
	اگر (origval_g != تہی)
	    saved_origval_g = vim_strsave(origval_g);
    }
#اگر_ختم

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	صحیح	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	اگر ((opt_flags & OPT_MODELINE)
#اگر_تعریف_شدہ HAVE_SANDBOX
	      || sandbox != 0
#اگر_ختم
	      || (op != OP_NONE && (*p & P_INSECURE)))
	    secure = 1;

	// Handle side effects, and set the global value for ":set" on local
	// options. Note: when setting 'syntax' or 'filetype' autocommands may
	// be triggered that can cause havoc.
	*errmsg = did_سیٹ_string_option(
			opt_idx, (char_u **)varp, oldval, newval, errbuf,
			errbuflen, opt_flags, op, قدر_checked);

	secure = secure_saved;
    }

#اگر defined(FEAT_EVAL)
    اگر (*errmsg == تہی)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#اگر_ختم

    *argp = arg;
    واپس *errmsg == تہی ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    ساکن حرف *
do_سیٹ_option_bool(
    صحیح		opt_idx,
    صحیح		opt_flags,
    سیٹ_سابقہ_T سابقہ,
    long_u	flags,
    char_u	*varp,
    صحیح		nextchar,
    صحیح		afterchar,
    صحیح		cp_val)

{
    varnumber_T	قدر;

    اگر (nextchar == '=' || nextchar == ':')
	واپس e_invalid_دلیل;
    اگر (opt_idx < 0 || varp == تہی)
	واپس تہی;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    اگر (nextchar == '!')
	قدر = *(صحیح *)(varp) ^ 1;
    ورنہ اگر (nextchar == '&')
	قدر = (صحیح)(طویل)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    ورنہ اگر (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	اگر ((صحیح *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    قدر = -1;
	ورنہ اگر ((صحیح *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    قدر = -1;
#اگر_تعریف_شدہ HAVE_FSYNC
	ورنہ اگر ((صحیح *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    قدر = -1;
#اگر_ختم
	ورنہ
	    قدر = *(صحیح *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    ورنہ
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	اگر (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    واپس e_trailing_characters;
	اگر (سابقہ == سابقہ_INV)
	    قدر = *(صحیح *)(varp) ^ 1;
	ورنہ
	    قدر = سابقہ == سابقہ_NO ? 0 : 1;
    }

    واپس سیٹ_bool_option(opt_idx, varp, (صحیح)قدر, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    ساکن حرف *
do_سیٹ_option_numeric(
    صحیح		opt_idx,
    صحیح		opt_flags,
    char_u	**argp,
    صحیح		nextchar,
    سیٹ_op_T	op,
    long_u	flags,
    صحیح		cp_val,
    char_u	*varp,
    حرف	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		قدر;
    صحیح			i;
    حرف		*errmsg = تہی;

    اگر (opt_idx < 0 || varp == تہی)
	واپس تہی;  // "cannot happen"
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
	قدر = (طویل)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    ورنہ اگر (nextchar == '<')
    {
	اگر ((طویل *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    قدر = NO_LOCAL_UNDOLEVEL;
	ورنہ اگر (opt_flags == OPT_LOCAL
		    && ((طویل *)varp == &curwin->w_p_siso
		     || (طویل *)varp == &curwin->w_p_so
		     || (طویل *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    قدر = -1;
	ورنہ
	    قدر = *(طویل *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    ورنہ اگر (((طویل *)varp == &p_wc || (طویل *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	قدر = string_مقصد_کلید(arg, FALSE);
	اگر (قدر == 0)
	{
	    errmsg = e_invalid_دلیل;
	    جاؤ skip;
	}
    }
    ورنہ اگر (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, تہی, &i, STR2NR_ALL, &قدر, تہی, 0, TRUE, تہی);
	اگر (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_نمبر_required_after_equal;
	    جاؤ skip;
	}
    }
    ورنہ
    {
	errmsg = e_نمبر_required_after_equal;
	جاؤ skip;
    }

    اگر (op == OP_ADDING)
	قدر = *(طویل *)varp + قدر;
    ورنہ اگر (op == OP_PREPENDING)
	قدر = *(طویل *)varp * قدر;
    ورنہ اگر (op == OP_REMOVING)
	قدر = *(طویل *)varp - قدر;

    اگر ((طویل *)varp == &curwin->w_p_sop && قدر < -1)
    {
	errmsg = e_invalid_دلیل;
	جاؤ skip;
    }

    errmsg = سیٹ_عدد_option(opt_idx, varp, قدر, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    واپس errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    ساکن حرف *
do_سیٹ_option_keycode(char_u **argp, char_u *کلید_name, صحیح nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    اگر (nextchar == '&')
    {
	اگر (جوڑو_termcap_اندراج(کلید_name, TRUE) == FAIL)
	    واپس e_not_ملا_اندر_termcap;
    }
    ورنہ
    {
	++arg; // jump to after the '=' or ':'
	برائے (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    اگر (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	جوڑو_termcode(کلید_name, arg, FALSE);
	*p = nextchar;
    }
    اگر (بھرا_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    واپس تہی;
}

/*
 * Set an option to a new value.
 */
    ساکن حرف *
do_سیٹ_option_قدر(
    صحیح		opt_idx,
    صحیح		opt_flags,
    char_u	**argp,
    سیٹ_سابقہ_T سابقہ,
    سیٹ_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*کلید_name,
    صحیح		nextchar,
    صحیح		afterchar,
    صحیح		cp_val,
    صحیح		*stopopteval,
    حرف	*errbuf,
    size_t	errbuflen)
{
    صحیح		قدر_checked = FALSE;
    حرف	*errmsg = تہی;
    char_u	*arg = *argp;

    اگر (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_سیٹ_option_bool(opt_idx, opt_flags, سابقہ, flags, varp,
						nextchar, afterchar, cp_val);
	اگر (errmsg != تہی)
	    جاؤ skip;
    }
    ورنہ
    {
	// numeric or string option
	اگر (vim_strchr((char_u *)"=:&<", nextchar) == تہی
		|| سابقہ != سابقہ_NONE)
	{
	    errmsg = e_invalid_دلیل;
	    جاؤ skip;
	}

	اگر (flags & P_عدد)
	{
	    // numeric option
	    errmsg = do_سیٹ_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    اگر (errmsg != تہی)
		جاؤ skip;
	}
	ورنہ اگر (opt_idx >= 0)
	{
	    // string option
	    اگر (do_سیٹ_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&قدر_checked, &errmsg) == FAIL)
	    {
		اگر (errmsg != تہی)
		    جاؤ skip;
		*stopopteval = TRUE;
		جاؤ skip;
	    }
	}
	ورنہ
	{
	    // key code option
	    errmsg = do_سیٹ_option_keycode(&arg, کلید_name, nextchar);
	    اگر (errmsg != تہی)
		جاؤ skip;
	}
    }

    اگر (opt_idx >= 0)
	did_سیٹ_option(opt_idx, opt_flags, op == OP_NONE, قدر_checked);

skip:
    *argp = arg;
    واپس errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    ساکن حرف *
do_سیٹ_option(
    صحیح		opt_flags,
    char_u	**argp,
    char_u	*arg_آغاز,
    char_u	**startarg,
    صحیح		*did_show,
    صحیح		*stopopteval,
    حرف	*errbuf,
    size_t	errbuflen)
{
    صحیح		opt_idx;
    char_u	*arg;
    سیٹ_سابقہ_T سابقہ;	// no prefix, "no" prefix or "inv" prefix
    سیٹ_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	کلید_name[2];
    صحیح		nextchar;	// next non-white char after option name
    صحیح		afterchar;	// character just after option name
    حرف	*errmsg = تہی;
    صحیح		کلید;
    صحیح		len;

    سابقہ = get_option_سابقہ(argp);
    arg = *argp;

    // find end of name
    کلید = 0;
    اگر (parse_option_name(arg, &opt_idx, &len, &کلید) == FAIL)
	واپس e_invalid_دلیل;

    // remember character after option name
    afterchar = arg[len];

    اگر (اندر_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	اگر (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != تہی
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    جاؤ skip;
	}
    }
    ورنہ
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	جب (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    اگر (op != OP_NONE)
	len++;

    nextchar = arg[len];

    اگر (opt_idx == -1 && کلید == 0)	// found a mismatch: skip
    {
	اگر (اندر_vim9script() && arg > arg_آغاز
		&& vim_strchr((char_u *)"!&<", *arg) != تہی)
	    errmsg = e_no_white_space_allowed_between_option_and;
	ورنہ
	    errmsg = e_unknown_option;
	جاؤ skip;
    }

    اگر (opt_idx >= 0)
    {
	اگر (options[opt_idx].var == تہی)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    اگر (vim_strchr((char_u *)"=:!&<", nextchar) == تہی
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    جاؤ skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    ورنہ
    {
	flags = P_STRING;
	varp = تہی;
	اگر (کلید < 0)
	{
	    کلید_name[0] = KEY2TERMCAP0(کلید);
	    کلید_name[1] = KEY2TERMCAP1(کلید);
	}
	ورنہ
	{
	    کلید_name[0] = KS_کلید;
	    کلید_name[1] = (کلید & 0xff);
	}
    }

    // Make sure the option value can be changed.
    اگر (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, سابقہ) == FAIL)
	جاؤ skip;

    صحیح cp_val = p_cp;
    اگر (vim_strchr((char_u *)"?=:!&<", nextchar) != تہی)
    {
	arg += len;
	اگر (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    اگر (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    ورنہ		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	اگر (vim_strchr((char_u *)"?!&<", nextchar) != تہی
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    جاؤ skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    اگر (nextchar == '?'
	    || (سابقہ == سابقہ_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == تہی
		&& !(flags & P_BOOL)))
    {
	// print value
	اگر (*did_show)
	    پیغام_putchar('\n');	    // cursor below last one
	ورنہ
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	اگر (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#اگر_تعریف_شدہ FEAT_EVAL
	    اگر (p_verbose > 0)
	    {
		// Mention where the option was last set.
		اگر (varp == options[opt_idx].var)
		    آخری_سیٹ_پیغام(options[opt_idx].script_ctx);
		ورنہ اگر ((صحیح)options[opt_idx].indir & PV_WIN)
		    آخری_سیٹ_پیغام(curwin->w_p_script_ctx[
			    (صحیح)options[opt_idx].indir & PV_نقاب]);
		ورنہ اگر ((صحیح)options[opt_idx].indir & PV_BUF)
		    آخری_سیٹ_پیغام(curbuf->b_p_script_ctx[
			    (صحیح)options[opt_idx].indir & PV_نقاب]);
	    }
#اگر_ختم
	}
	ورنہ
	{
	    char_u	    *p;

	    p = ڈھونڈو_termcode(کلید_name);
	    اگر (p == تہی)
	    {
		errmsg = e_کلید_رمز_not_سیٹ;
		جاؤ skip;
	    }
	    ورنہ
		(خالی)show_one_termcode(کلید_name, p, TRUE);
	}
	اگر (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    ورنہ
    {
	errmsg = do_سیٹ_option_قدر(opt_idx, opt_flags, &arg, سابقہ, op,
					flags, varp, کلید_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    واپس errmsg;
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
do_سیٹ(
    char_u	*arg_آغاز,	// option string (may be written to!)
    صحیح		opt_flags)
{
    char_u	*arg = arg_آغاز;
    صحیح		i;
    صحیح		did_show = FALSE;   // already showed one value

    اگر (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	جاؤ theend;
    }

    جب (*arg != NUL)		// loop to process all options
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
		سیٹ_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    ورنہ
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	ورنہ اگر (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	ورنہ
	{
	    صحیح		stopopteval = FALSE;
	    حرف	*errmsg = تہی;
	    حرف	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_سیٹ_option(opt_flags, &arg, arg_آغاز, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    اگر (stopopteval)
		توڑو;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    برائے (i = 0; i < 2 ; ++i)
	    {
		جب (*arg != NUL && !VIM_ISWHITE(*arg))
		    اگر (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		اگر (*arg != '=')
		    توڑو;
	    }

	    اگر (errmsg != تہی)
	    {
		i = vim_snprintf((حرف *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
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
		emsg((حرف *)IObuff);	// show error highlighted
		--no_wait_return;

		واپس FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    اگر (silent_منوال && did_show)
    {
	// After displaying option values in silent mode.
	silent_منوال = FALSE;
	info_پیغام = TRUE;	// use mch_msg(), not mch_errmsg()
	پیغام_putchar('\n');
	cursor_on();		// msg_start() switches it off
	برآمد_flush();
	silent_منوال = TRUE;
	info_پیغام = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    واپس OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    خالی
did_سیٹ_option(
    صحیح	    opt_idx,
    صحیح	    opt_flags,	    // possibly with OPT_MODELINE
    صحیح	    new_قدر_2,	    // value was replaced completely
    صحیح	    قدر_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = تہی;
    صحیح		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_سیٹ;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    اگر (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    اگر (!قدر_checked && (secure
#اگر_تعریف_شدہ HAVE_SANDBOX
	    || sandbox != 0
#اگر_ختم
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	اگر (flagsp_local != تہی)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    ورنہ اگر (new_قدر_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	اگر (flagsp_local != تہی)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    صحیح
string_مقصد_کلید(char_u *arg, صحیح multi_byte)
{
    اگر (*arg == '<' && arg[1])
	واپس ڈھونڈو_کلید_option(arg + 1, TRUE);
    اگر (*arg == '^' && arg[1])
    {
	صحیح کلید = Ctrl_chr(arg[1]);
	اگر (کلید == 0)		// ^@ is <Nul>
	    کلید = K_ZERO;
	واپس کلید;
    }
    اگر (multi_byte)
	واپس PTR2CHAR(arg);
    واپس *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    خالی
did_سیٹ_title(خالی)
{
    اگر (starting != NO_SCREEN
#اگر_تعریف_شدہ FEAT_GUI
	    && !gui.starting
#اگر_ختم
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    خالی
سیٹ_options_bin(
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
    ورنہ اگر (oldval)		// switched off
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
#اگر_ختم
}

/*
 * Expand environment variables for some string options.
 * These string options cannot be indirect!
 * If "val" is NULL expand the current value of the option.
 * Return pointer to NameBuff, or NULL when not expanded.
 */
    ساکن char_u *
option_expand(صحیح opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    اگر (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == تہی)
	واپس تہی;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    اگر (val != تہی && STRLEN(val) > MAXPATHL)
	واپس تہی;

    اگر (val == تہی)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    صحیح esc = var == &p_لیبل_فہرست || var == &p_راستہ;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : تہی, FALSE,
#اگر_تعریف_شدہ FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#اگر_ختم
				  تہی);
    اگر (STRCMP(NameBuff, val) == 0)   // they are the same
	واپس تہی;

    واپس NameBuff;
}

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    ساکن خالی
didset_options(خالی)
{
    // initialize the table for 'iskeyword' et.al.
    (خالی)init_chartab();

    didset_string_options();

#اگر_تعریف_شدہ FEAT_SPELL
    (خالی)spell_check_msm();
    (خالی)spell_check_sps();
    (خالی)compile_cap_prog(curwin->w_s);
    (خالی)did_سیٹ_spell_option();
#اگر_ختم
    // set cedit_key
    (خالی)did_سیٹ_cedit(تہی);
#اگر_تعریف_شدہ FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_سیٹ_breakat(تہی);
#اگر_ختم
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    ساکن خالی
didset_options2(خالی)
{
    // Initialize the highlight_attr[] table.
    (خالی)highlight_بدلا();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (خالی)سیٹ_listchars_option(curwin, curwin->w_p_lcs, TRUE, تہی, 0);

    // Parse default for 'fillchars'.
    (خالی)سیٹ_fillchars_option(curwin, curwin->w_p_fcs, TRUE, تہی, 0);

#اگر_تعریف_شدہ FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (خالی)did_سیٹ_clipboard(تہی);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (خالی)tabstop_سیٹ(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (خالی)tabstop_سیٹ(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#اگر_ختم
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    خالی
check_options(خالی)
{
    صحیح		opt_idx;

    برائے (opt_idx = 0; options[opt_idx].fullname != تہی; opt_idx++)
	اگر ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != تہی)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    صحیح
get_رکن_opt_idx(char_u **p)
{
    صحیح opt_idx;

    برائے (opt_idx = 1; options[opt_idx].fullname != تہی; opt_idx++)
	اگر (options[opt_idx].var == (char_u *)p)
	    واپس opt_idx;
    واپس -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    صحیح
سیٹ_رکن_option_alloced(char_u **p)
{
    صحیح		opt_idx = get_رکن_opt_idx(p);

    اگر (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    واپس opt_idx;
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    صحیح
was_سیٹ_insecurely(win_T *wp, char_u *opt, صحیح opt_flags)
{
    صحیح	    idx = findoption(opt);
    long_u  *flagp;

    اگر (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	واپس (*flagp & P_INSECURE) != 0;
    }
    اندرونی_error("was_set_insecurely()");
    واپس -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    ساکن long_u *
insecure_flag(win_T *wp, صحیح opt_idx, صحیح opt_flags)
{
    اگر (opt_flags & OPT_LOCAL)
	انتخاب ((صحیح)options[opt_idx].indir)
	{
	    حالت PV_WRAP:	واپس &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    حالت PV_STL:	واپس &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    حالت PV_FDE:	واپس &wp->w_p_fde_flags;
	    حالت PV_FDT:	واپس &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    حالت PV_BEXPR:	واپس &wp->w_بفر->b_p_bexpr_flags;
#  endif
	    حالت PV_INDE:	واپس &wp->w_بفر->b_p_inde_flags;
	    حالت PV_FEX:	واپس &wp->w_بفر->b_p_fex_flags;
#  ifdef FEAT_ڈھونڈو_ID
	    حالت PV_INEX:	واپس &wp->w_بفر->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    حالت PV_CPT:	واپس &wp->w_بفر->b_p_cpt_flags;
#  endif
# endif
	}
    ورنہ
	// For global value of window-local options, use flags in w_allbuf_opt.
	انتخاب ((صحیح)options[opt_idx].indir)
	{
	    حالت PV_WRAP:	واپس &wp->w_allbuf_opt.wo_wrap_flags;
# اگر defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    حالت PV_FDE:	واپس &wp->w_allbuf_opt.wo_fde_flags;
	    حالت PV_FDT:	واپس &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    واپس &options[opt_idx].flags;
}
#اگر_ختم

/*
 * Redraw the window title and/or tab page text later.
 */
    خالی
redraw_titles(خالی)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#اگر_ختم
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    صحیح
valid_name(char_u *val, حرف *allowed)
{
    char_u *s;

    برائے (s = val; *s != NUL; ++s)
	اگر (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == تہی)
	    واپس FALSE;
    واپس TRUE;
}

#اگر defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    خالی
سیٹ_option_sctx_idx(صحیح opt_idx, صحیح opt_flags, sctx_T script_ctx)
{
    صحیح		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    صحیح		indir = (صحیح)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    اگر (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    اگر (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_نسخہ = SCRIPT_نسخہ_VIM9;
    اگر (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_نسخہ = SCRIPT_نسخہ_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    اگر (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    اگر (both || (opt_flags & OPT_LOCAL))
    {
	اگر (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_نقاب] = new_script_ctx;
	ورنہ اگر (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_نقاب] = new_script_ctx;
	    اگر (both)
		// also setting the "all buffers" value
		curwin->w_allbuf_opt.wo_script_ctx[indir & PV_نقاب] =
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
is_option_قدر_vim9(صحیح opt_idx, صحیح opt_flags)
{
    صحیح		indir = (صحیح)options[opt_idx].indir;
    sctx_T	*sctx = تہی;

    اگر ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    اگر ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	اگر (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_نقاب];
	ورنہ اگر (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_نقاب];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    واپس !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_نسخہ >= SCRIPT_نسخہ_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(حرف *name)
{
    صحیح idx = findoption((char_u *)name);

    اگر (idx >= 0)
	واپس &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    واپس تہی;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    خالی
سیٹ_رکن_option_sctx_idx(حرف *name, صحیح opt_idx)
{
    char_u  buf[5];
    صحیح	    idx;

    اگر (name == تہی)
	idx = opt_idx;
    ورنہ
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    اگر (idx >= 0)
	سیٹ_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    ساکن خالی
apply_optionset_autocmd(
	صحیح	opt_idx,
	طویل	opt_flags,
	طویل	oldval,
	طویل	oldval_g,
	طویل	newval,
	حرف	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    اگر (starting || errmsg != تہی || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	واپس;

    len = vim_snprintf_safelen((حرف *)buf_new, حجم(buf_new), "%ld", newval);
    سیٹ_vim_var_string(VV_OPTION_NEW, buf_new, (صحیح)len);
    len = vim_snprintf_safelen((حرف *)buf_type, حجم(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    سیٹ_vim_var_string(VV_OPTION_TYPE, buf_type, (صحیح)len);

    buf_oldlen = vim_snprintf_safelen((حرف *)buf_old, حجم(buf_old), "%ld", oldval);
    سیٹ_vim_var_string(VV_OPTION_OLD, buf_old, (صحیح)buf_oldlen);

    اگر (opt_flags & OPT_LOCAL)
    {
	سیٹ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	سیٹ_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (صحیح)buf_oldlen);
    }
    اگر (opt_flags & OPT_GLOBAL)
    {
	سیٹ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	سیٹ_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (صحیح)buf_oldlen);
    }
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((حرف *)buf_old_global,
	    حجم(buf_old_global), "%ld", oldval_g);

	سیٹ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	سیٹ_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (صحیح)buf_oldlen);
	سیٹ_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (صحیح)buf_old_globallen);
    }
    اگر (opt_flags & OPT_MODELINE)
    {
	سیٹ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	سیٹ_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (صحیح)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    تہی, FALSE, تہی);
    reset_v_option_vars();
}
#اگر_ختم

#اگر defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    حرف *
did_سیٹ_arabic(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	اگر (!p_tbidi)
	{
	    // set rightleft mode
	    اگر (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		بدلا_کھڑکی_setting();
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
	    ساکن حرف *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    پیغام_ماخذ(HL_ATTR(HLF_W));
	    پیغام_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    سیٹ_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = سیٹ_option_قدر((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    ورنہ
    {
	// 'arabic' is reset, handle various sub-settings.
	اگر (!p_tbidi)
	{
	    // reset rightleft mode
	    اگر (curwin->w_p_rl)
	    {
		curwin->w_p_rl = FALSE;
		بدلا_کھڑکی_setting();
	    }

	    // 'arabicshape' isn't reset, it is a global option and
	    // another window may still need it "on".
	}

	// 'delcombine' isn't reset, it is a global option and another
	// window may still want it "on".

# ifdef FEAT_KEYMAP
	// Revert to the default keymap
	curbuf->b_p_iminsert = B_IMODE_NONE;
	curbuf->b_p_imsearch = B_IMODE_USE_داخل_کرو;
# endif
    }

    واپس errmsg;
}
#اگر_ختم

#اگر defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    حرف *
did_سیٹ_autochdir(optset_T *دلائل غیر_مستعمل)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    حرف *
did_سیٹ_ballooneval(optset_T *دلائل)
{
    اگر (balloon_Eval_For_رکن)
	واپس تہی;

    اگر (p_beval && !دلائل->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    ورنہ اگر (!p_beval && دلائل->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_BEVAL_رکن)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    حرف *
did_سیٹ_balloonevalterm(optset_T *دلائل غیر_مستعمل)
{
    mch_bevalterm_بدلا();
    واپس تہی;
}
#اگر_ختم

/*
 * Process the updated 'binary' option value.
 */
    حرف *
did_سیٹ_ثنائی(optset_T *دلائل)
{
    // when 'bin' is set also set some other options
    سیٹ_options_bin(دلائل->os_oldval.boolean, curbuf->b_p_bin, دلائل->os_flags);
    redraw_titles();

    واپس تہی;
}

/*
 * Process the updated 'buflisted' option value.
 */
    حرف *
did_سیٹ_buflisted(optset_T *دلائل)
{
    // when 'buflisted' changes, trigger autocommands
    اگر (دلائل->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						تہی, تہی, TRUE, curbuf);
    واپس تہی;
}

/*
 * Process the new 'cmdheight' option value.
 */
    حرف *
did_سیٹ_cmdheight(optset_T *دلائل)
{
    طویل old_قدر = دلائل->os_oldval.نمبر;
    حرف *errmsg = تہی;

    // if p_ch changed value, change the command line height
    اگر (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_ch = MIN_CMDHEIGHT;
    }
    اگر (p_ch > صفیں - min_صفیں() + MIN_CMDHEIGHT)
	p_ch = صفیں - min_صفیں() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    اگر ((p_ch != old_قدر
		|| tabline_اونچائی() + topframe->fr_اونچائی != صفیں - p_ch)
	    && بھرا_screen
#اگر_تعریف_شدہ FEAT_GUI
	    && !gui.starting
#اگر_ختم
       )
	command_اونچائی();

    واپس errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    حرف *
did_سیٹ_compatible(optset_T *دلائل غیر_مستعمل)
{
    compatible_سیٹ();
    واپس تہی;
}

#اگر defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    حرف *
did_سیٹ_conceallevel(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (curwin->w_p_cole < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	curwin->w_p_cole = 0;
    }
    ورنہ اگر (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_دلیل;
	curwin->w_p_cole = 3;
    }
    اگر (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    ورنہ اگر (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_دلیل;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    واپس errmsg;
}
#اگر_ختم

#اگر defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    حرف *
did_سیٹ_diff(optset_T *دلائل غیر_مستعمل)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    اگر (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    واپس تہی;
}
#اگر_ختم

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    حرف *
did_سیٹ_eof_eol_fixeol_bomb(optset_T *دلائل غیر_مستعمل)
{
    // redraw the window title and tab page text
    redraw_titles();
    واپس تہی;
}

/*
 * Process the updated 'equalalways' option value.
 */
    حرف *
did_سیٹ_equalalways(optset_T *دلائل)
{
    اگر (p_ea && !دلائل->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    واپس تہی;
}

#اگر defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    حرف *
did_سیٹ_foldcolumn(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (curwin->w_p_fdc < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	curwin->w_p_fdc = 0;
    }
    ورنہ اگر (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_دلیل;
	curwin->w_p_fdc = 12;
    }
    اگر (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    ورنہ اگر (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_دلیل;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    واپس errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    حرف *
did_سیٹ_foldlevel(optset_T *دلائل غیر_مستعمل)
{
    اگر (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    واپس تہی;
}

/*
 * Process the new 'foldminlines' option value.
 */
    حرف *
did_سیٹ_foldminlines(optset_T *دلائل غیر_مستعمل)
{
    foldUpdateAll(curwin);
    واپس تہی;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    حرف *
did_سیٹ_foldnestmax(optset_T *دلائل غیر_مستعمل)
{
    اگر (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_تلاش_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    حرف *
did_سیٹ_hlsearch(optset_T *دلائل غیر_مستعمل)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    سیٹ_no_hlsearch(FALSE);
    واپس تہی;
}
#اگر_ختم

/*
 * Process the updated 'ignorecase' option value.
 */
    حرف *
did_سیٹ_ignorecase(optset_T *دلائل غیر_مستعمل)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    اگر (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    واپس تہی;
}

#اگر defined(HAVE_داخلہ_طریقہ)
/*
 * Process the updated 'imdisable' option value.
 */
    حرف *
did_سیٹ_imdisable(optset_T *دلائل غیر_مستعمل)
{
    // Only de-activate it here, it will be enabled when changing mode.
    اگر (p_imdisable)
	im_سیٹ_فعال(FALSE);
    ورنہ اگر (State & منوال_داخل_کرو)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_سیٹ_فعال(curbuf->b_p_iminsert == B_IMODE_IM);
    واپس تہی;
}
#اگر_ختم

/*
 * Process the new 'iminsert' option value.
 */
    حرف *
did_سیٹ_iminsert(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_آخری)
    {
	errmsg = e_invalid_دلیل;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    اگر (termcap_فعال)	// don't do this in the alternate screen
	showmode();
#اگر defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    حالت_redraw_curbuf();
#اگر_ختم

    واپس errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    حرف *
did_سیٹ_imsearch(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_آخری)
    {
	errmsg = e_invalid_دلیل;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    واپس errmsg;
}

#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    حرف *
did_سیٹ_imstyle(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_دلیل;

    واپس errmsg;
}
#اگر_ختم

/*
 * Process the updated 'insertmode' option value.
 */
    حرف *
did_سیٹ_insertmode(optset_T *دلائل)
{
    // when 'insertmode' is set from an autocommand need to do work here
    اگر (p_im)
    {
	اگر ((State & منوال_داخل_کرو) == 0)
	    need_آغاز_insertmode = TRUE;
	stop_داخل_کرو_منوال = FALSE;
    }
    // only reset if it was set previously
    ورنہ اگر (دلائل->os_oldval.boolean)
    {
	need_آغاز_insertmode = FALSE;
	stop_داخل_کرو_منوال = TRUE;
	اگر (restart_edit != 0 && منوال_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    واپس تہی;
}

#اگر defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    حرف *
did_سیٹ_langnoremap(optset_T *دلائل غیر_مستعمل)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    واپس تہی;
}

/*
 * Process the updated 'langremap' option value.
 */
    حرف *
did_سیٹ_langremap(optset_T *دلائل غیر_مستعمل)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    واپس تہی;
}
#اگر_ختم

/*
 * Process the new 'laststatus' option value.
 */
    حرف *
did_سیٹ_laststatus(optset_T *دلائل غیر_مستعمل)
{
    آخری_حالت(FALSE);	// (re)set last window status line
    واپس تہی;
}

#اگر defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    حرف *
did_سیٹ_linespace(optset_T *دلائل غیر_مستعمل)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    اگر (gui.اندر_use && gui_mch_adjust_charheight() == OK)
	gui_سیٹ_shellsize(FALSE, FALSE, RESIZE_VERT);
    واپس تہی;
}
#اگر_ختم

/*
 * Process the updated 'lisp' option value.
 */
    حرف *
did_سیٹ_lisp(optset_T *دلائل غیر_مستعمل)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (خالی)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    واپس تہی;
}

/*
 * Process the new 'maxcombine' option value.
 */
    حرف *
did_سیٹ_maxcombine(optset_T *دلائل غیر_مستعمل)
{
    اگر (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    ورنہ اگر (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    واپس تہی;
}

/*
 * Process the updated 'modifiable' option value.
 */
    حرف *
did_سیٹ_modifiable(optset_T *دلائل غیر_مستعمل)
{
    // when 'modifiable' is changed, redraw the window title

#اگر_تعریف_شدہ FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    اگر (curbuf->b_p_ma && (رکن_اندر_معمول_منوال(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_رکن != تہی && !رکن_is_ختم_شدہ(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	واپس e_cannot_make_terminal_with_running_کام_modifiable;
    }
#اگر_ختم
    redraw_titles();

    واپس تہی;
}

/*
 * Process the updated 'modified' option value.
 */
    حرف *
did_سیٹ_modified(optset_T *دلائل)
{
    اگر (!دلائل->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_سیٹ = !!دلائل->os_newval.boolean;
    واپس تہی;
}

#اگر defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    حرف *
did_سیٹ_mousehide(optset_T *دلائل غیر_مستعمل)
{
    اگر (!p_mh)
	gui_mch_mousehide(FALSE);
    واپس تہی;
}
#اگر_ختم

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    حرف *
did_سیٹ_نمبر_relativenumber(optset_T *دلائل غیر_مستعمل)
{
#اگر defined(FEAT_SIGNS) && defined(FEAT_GUI)
    اگر (gui.اندر_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != تہی)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	اگر (!(curwin->w_p_nu && ((صحیح *)دلائل->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#اگر_ختم
    واپس تہی;
}

#اگر defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    حرف *
did_سیٹ_numberwidth(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    // 'numberwidth' must be positive
    اگر (curwin->w_p_nuw < 1)
    {
	errmsg = e_دلیل_must_be_مثبت;
	curwin->w_p_nuw = 1;
    }
    ورنہ اگر (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_دلیل;
	curwin->w_p_nuw = 20;
    }
    اگر (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_دلیل_must_be_مثبت;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    ورنہ اگر (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_دلیل;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_سطر_گنتی = 0; // trigger a redraw

    واپس errmsg;
}
#اگر_ختم

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    حرف *
did_سیٹ_osctimeoutlen(optset_T *دلائل)
{
    اگر (p_ost < 0)
    {
	p_ost = دلائل->os_oldval.نمبر;
	واپس e_دلیل_must_be_مثبت;
    }

    واپس تہی;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    حرف *
did_سیٹ_paste(optset_T *دلائل غیر_مستعمل)
{
    ساکن صحیح	old_p_paste = FALSE;
    ساکن صحیح	save_sm = 0;
    ساکن صحیح	save_sta = 0;
    ساکن صحیح	save_ru = 0;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    ساکن صحیح	save_ri = 0;
    ساکن صحیح	save_hkmap = 0;
#اگر_ختم
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
#اگر_تعریف_شدہ FEAT_VARTABS
		اگر (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != خالی_خانے_option
					   ? vim_strsave(buf->b_p_vsts) : تہی;
#اگر_ختم
	    }

	    // save global options
	    save_sm = p_sm;
	    save_sta = p_sta;
	    save_ru = p_ru;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    save_ri = p_ri;
	    save_hkmap = p_hkmap;
#اگر_ختم
	    // save global values for local buffer options
	    p_ai_nopaste = p_ai;
	    p_et_nopaste = p_et;
	    p_sts_nopaste = p_sts;
	    p_tw_nopaste = p_tw;
	    p_wm_nopaste = p_wm;
#اگر_تعریف_شدہ FEAT_VARTABS
	    اگر (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != خالی_خانے_option
						  ? vim_strsave(p_vsts) : تہی;
#اگر_ختم
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
#اگر_تعریف_شدہ FEAT_VARTABS
	    اگر (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = خالی_خانے_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#اگر_ختم
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	اگر (p_ru)
	    حالت_redraw_all();    // redraw to remove the ruler
	p_ru = 0;		    // no ruler
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	p_ri = 0;		    // no reverse insert
	p_hkmap = 0;		    // no Hebrew keyboard
#اگر_ختم
	// set global values for local buffer options
	p_tw = 0;
	p_wm = 0;
	p_sts = 0;
	p_ai = 0;
	p_et = 0;
#اگر_تعریف_شدہ FEAT_VARTABS
	اگر (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = خالی_خانے_option;
#اگر_ختم
    }

    // Paste switched from on to off: Restore saved values.
    ورنہ اگر (old_p_paste)
    {
	// restore options for each buffer
	FOR_ALL_BUFFERS(buf)
	{
	    buf->b_p_tw = buf->b_p_tw_nopaste;
	    buf->b_p_wm = buf->b_p_wm_nopaste;
	    buf->b_p_sts = buf->b_p_sts_nopaste;
	    buf->b_p_ai = buf->b_p_ai_nopaste;
	    buf->b_p_et = buf->b_p_et_nopaste;
#اگر_تعریف_شدہ FEAT_VARTABS
	    اگر (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : خالی_خانے_option;
	    vim_free(buf->b_p_vsts_array);
	    اگر (buf->b_p_vsts && buf->b_p_vsts != خالی_خانے_option)
		(خالی)tabstop_سیٹ(buf->b_p_vsts, &buf->b_p_vsts_array);
	    ورنہ
		buf->b_p_vsts_array = تہی;
#اگر_ختم
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	اگر (p_ru != save_ru)
	    حالت_redraw_all();    // redraw to draw the ruler
	p_ru = save_ru;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	p_ri = save_ri;
	p_hkmap = save_hkmap;
#اگر_ختم
	// set global values for local buffer options
	p_ai = p_ai_nopaste;
	p_et = p_et_nopaste;
	p_sts = p_sts_nopaste;
	p_tw = p_tw_nopaste;
	p_wm = p_wm_nopaste;
#اگر_تعریف_شدہ FEAT_VARTABS
	اگر (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : خالی_خانے_option;
#اگر_ختم
    }

    old_p_paste = p_paste;

#اگر defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#اگر_ختم

    واپس تہی;
}


#اگر_تعریف_شدہ FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    حرف *
did_سیٹ_previewwindow(optset_T *دلائل غیر_مستعمل)
{
    اگر (!curwin->w_p_pvw)
	واپس تہی;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	اگر (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    واپس e_preview_کھڑکی_already_exists;
	}

    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    حرف *
did_سیٹ_pyxversion(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_دلیل;

    واپس errmsg;
}
#اگر_ختم

/*
 * Process the updated 'readonly' option value.
 */
    حرف *
did_سیٹ_readonly(optset_T *دلائل)
{
    // when 'readonly' is reset globally, also reset readonlymode
    اگر (!curbuf->b_p_ro && (دلائل->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    اگر (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    واپس تہی;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    حرف *
did_سیٹ_scrollbind(optset_T *دلائل غیر_مستعمل)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    اگر (!curwin->w_p_scb)
	واپس تہی;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    واپس تہی;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    حرف *
did_سیٹ_maxsearchcount(optset_T *دلائل غیر_مستعمل)
{
    حرف	*errmsg = تہی;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#تعریف_کرو MAX_تلاش_گنتی 9999

    اگر (p_msc <= 0)
	errmsg = e_دلیل_must_be_مثبت;
    ورنہ اگر (p_msc > MAX_تلاش_گنتی)
	errmsg = e_invalid_دلیل;

    اگر (errmsg != تہی)
	p_msc = 99;

    واپس errmsg;
#تعریف_منسوخ MAX_تلاش_گنتی
}

#اگر defined(BACKSLASH_اندر_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    حرف *
did_سیٹ_shellslash(optset_T *دلائل غیر_مستعمل)
{
    اگر (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    ورنہ
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
    واپس تہی;
}
#اگر_ختم

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    حرف *
did_سیٹ_shiftwidth_tabstop(optset_T *دلائل)
{
    طویل	*pp = (طویل *)دلائل->os_varp;
    حرف	*errmsg = تہی;

    اگر (curbuf->b_p_ts <= 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	curbuf->b_p_ts = 8;
    }
    ورنہ اگر (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_دلیل;
	curbuf->b_p_ts = 8;
    }
    اگر (p_ts <= 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_ts = 8;
    }
    ورنہ اگر (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_دلیل;
	p_ts = 8;
    }

    اگر (curbuf->b_p_sw < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
#اگر_تعریف_شدہ FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_گنتی(curbuf->b_p_vts_array) > 0
		       ? tabstop_پہلا(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#ورنہ
	curbuf->b_p_sw = curbuf->b_p_ts;
#اگر_ختم
    }
    اگر (p_sw < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
#اگر_تعریف_شدہ FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_گنتی(curbuf->b_p_vts_array) > 0
	     ? tabstop_پہلا(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#ورنہ
	p_sw = curbuf->b_p_ts;
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#اگر_ختم
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    اگر (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    واپس errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    حرف *
did_سیٹ_showtabline(optset_T *دلائل غیر_مستعمل)
{
    shell_new_صفیں();	// recompute window positions and heights
    واپس تہی;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    حرف *
did_سیٹ_smoothscroll(optset_T *دلائل غیر_مستعمل)
{
    اگر (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    واپس تہی;
}

#اگر defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    حرف *
did_سیٹ_spell(optset_T *دلائل غیر_مستعمل)
{
    اگر (curwin->w_p_spell)
	واپس parse_spelllang(curwin);

    واپس تہی;
}
#اگر_ختم

/*
 * Process the updated 'swapfile' option value.
 */
    حرف *
did_سیٹ_swapfile(optset_T *دلائل غیر_مستعمل)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    اگر (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    ورنہ
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    واپس تہی;
}

#اگر defined(FEAT_TERMGUICOLORS)
    حرف *
did_سیٹ_termguicolors(optset_T *دلائل غیر_مستعمل)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    اگر (
#  ifdef VIMDLL
	    !gui.اندر_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_سیٹ = TRUE;
	p_tgc = 0;
	واپس e_24_ثنائی_ہندسہ_colors_are_not_supported_on_this_environment;
    }
    اگر (is_رکن_win32())
	تبادلہ_tcap();
# endif
# ifdef FEAT_GUI
    اگر (!gui.اندر_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    اگر (is_رکن_win32())
    {
	control_console_color_rgb();
	سیٹ_termname(T_NAME);
	init_highlight(TRUE, FALSE);
    }
# endif
# ifdef FEAT_TERMINAL
    رکن_update_colors_all();
    رکن_update_palette_all();
    رکن_update_hlfwin_all();
# endif
    p_tgc_سیٹ = TRUE;

    واپس تہی;
}
#اگر_ختم

    حرف *
did_سیٹ_termsync(optset_T *دلائل غیر_مستعمل)
{
    اگر (!p_tsy)
	رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_OFF);
    واپس تہی;
}

#اگر defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    حرف *
did_سیٹ_termwinscroll(optset_T *دلائل)
{
    طویل	*pp = (طویل *)دلائل->os_varp;
    حرف	*errmsg = تہی;

    اگر (*pp < 1)
    {
	errmsg = e_دلیل_must_be_مثبت;
	*pp = 1;
    }

    واپس errmsg;
}
#اگر_ختم

/*
 * Process the updated 'terse' option value.
 */
    حرف *
did_سیٹ_terse(optset_T *دلائل غیر_مستعمل)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_تلاش);

    // insert 's' in p_shm
    اگر (p_terse && p == تہی)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	سیٹ_string_option_براہ_راست((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    ورنہ اگر (!p_terse && p != تہی)
	STRMOVE(p, p + 1);
    واپس تہی;
}

/*
 * Process the updated 'textauto' option value.
 */
    حرف *
did_سیٹ_textauto(optset_T *دلائل)
{
    // when 'textauto' is set or reset also change 'fileformats'
    سیٹ_string_option_براہ_راست((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | دلائل->os_flags, 0);

    واپس تہی;
}

/*
 * Process the updated 'textmode' option value.
 */
    حرف *
did_سیٹ_textmode(optset_T *دلائل)
{
    // when 'textmode' is set or reset also change 'fileformat'
    سیٹ_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, دلائل->os_flags);

    واپس تہی;
}

/*
 * Process the new 'textwidth' option value.
 */
    حرف *
did_سیٹ_textwidth(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (curbuf->b_p_tw < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	curbuf->b_p_tw = 0;
    }
    اگر (p_tw < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_tw = 0;
    }
#اگر_تعریف_شدہ FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(تہی, wp);
    }
#اگر_ختم

    واپس errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    حرف *
did_سیٹ_title_icon(optset_T *دلائل غیر_مستعمل)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_سیٹ_title();
    واپس تہی;
}

/*
 * Process the new 'titlelen' option value.
 */
    حرف *
did_سیٹ_titlelen(optset_T *دلائل)
{
    طویل old_قدر = دلائل->os_oldval.نمبر;
    حرف *errmsg = تہی;

    // if 'titlelen' has changed, redraw the title
    اگر (p_titlelen < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_titlelen = 85;
    }
    اگر (starting != NO_SCREEN && old_قدر != p_titlelen)
	need_maketitle = TRUE;

    واپس errmsg;
}

#اگر defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    حرف *
did_سیٹ_undofile(optset_T *دلائل)
{
    // Only take action when the option was set.
    اگر (!curbuf->b_p_udf && !p_udf)
	واپس تہی;

    // When reset we do not delete the undo file, the option may be set again
    // without making any changes in between.
    char_u	hash[UNDO_HASH_سائز];
    buf_T	*save_curbuf = curbuf;

    FOR_ALL_BUFFERS(curbuf)
    {
	// When 'undofile' is set globally: for every buffer, otherwise
	// only for the current buffer: Try to read in the undofile,
	// if one exists, the buffer wasn't changed and the buffer was
	// loaded
	اگر ((curbuf == save_curbuf
		    || (دلائل->os_flags & OPT_GLOBAL)
		    || دلائل->os_flags == 0)
		&& !curbuf_Is_بدلا() && curbuf->b_ml.ml_mfp != تہی)
	{
# ifdef FEAT_CRYPT
	    اگر (crypt_طریقہ_is_sodium(crypt_get_طریقہ_nr(curbuf)))
		جاری;
# endif
	    u_compute_hash(hash);
	    u_read_undo(تہی, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    واپس تہی;
}
#اگر_ختم

/*
 * Process the new global 'undolevels' option value.
 */
    ساکن خالی
update_global_undolevels(طویل قدر, طویل old_قدر)
{
    // sync undo before 'undolevels' changes

    // use the old value, otherwise u_sync() may not work properly
    p_ul = old_قدر;
    u_sync(TRUE);
    p_ul = قدر;
}

/*
 * Process the new buffer local 'undolevels' option value.
 */
    ساکن خالی
update_buflocal_undolevels(طویل قدر, طویل old_قدر)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_قدر;
    u_sync(TRUE);
    curbuf->b_p_ul = قدر;
}

/*
 * Process the new 'undolevels' option value.
 */
    حرف *
did_سیٹ_undolevels(optset_T *دلائل)
{
    طویل *pp = (طویل *)دلائل->os_varp;

    اگر (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(دلائل->os_newval.نمبر,
						       دلائل->os_oldval.نمبر);
    ورنہ اگر (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(دلائل->os_newval.نمبر,
						       دلائل->os_oldval.نمبر);

    واپس تہی;
}

/*
 * Process the new 'updatecount' option value.
 */
    حرف *
did_سیٹ_updatecount(optset_T *دلائل)
{
    طویل old_قدر = دلائل->os_oldval.نمبر;
    حرف *errmsg = تہی;

    // when 'updatecount' changes from zero to non-zero, open swap files
    اگر (p_uc < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_uc = 100;
    }
    اگر (p_uc && !old_قدر)
	ml_open_files();

    واپس errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    حرف *
did_سیٹ_weirdinvert(optset_T *دلائل)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    اگر (p_wiv && !دلائل->os_oldval.boolean)
	T_XS = (char_u *)"y";
    ورنہ اگر (!p_wiv && دلائل->os_oldval.boolean)
	T_XS = خالی_خانے_option;
    p_wiv = (*T_XS != NUL);

    واپس تہی;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    حرف *
did_سیٹ_wildchar(optset_T *دلائل)
{
    طویل c = *(طویل *)دلائل->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    اگر (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	واپس e_invalid_دلیل;

    واپس تہی;
}

/*
 * Process the new 'window' option value.
 */
    حرف *
did_سیٹ_کھڑکی(optset_T *دلائل غیر_مستعمل)
{
    اگر (p_کھڑکی < 1)
	p_کھڑکی = 1;
    ورنہ اگر (p_کھڑکی >= صفیں)
	p_کھڑکی = صفیں - 1;
    واپس تہی;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    حرف *
did_سیٹ_winheight_helpheight(optset_T *دلائل)
{
    طویل	*pp = (طویل *)دلائل->os_varp;
    حرف	*errmsg = تہی;

    اگر (p_wh < 1)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_wh = 1;
    }
    اگر (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    اگر (p_hh < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_hh = 0;
    }

    // Change window height NOW
    اگر (!ONE_کھڑکی)
    {
	اگر (pp == &p_wh && curwin->w_اونچائی < p_wh)
	    win_setheight((صحیح)p_wh);
	اگر (pp == &p_hh && curbuf->b_help && curwin->w_اونچائی < p_hh)
	    win_setheight((صحیح)p_hh);
    }

    واپس errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    حرف *
did_سیٹ_winminheight(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (p_wmh < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_wmh = 0;
    }
    اگر (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    واپس errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    حرف *
did_سیٹ_winminwidth(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (p_wmw < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_wmw = 0;
    }
    اگر (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    واپس errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    حرف *
did_سیٹ_winwidth(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (p_wiw < 1)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_wiw = 1;
    }
    اگر (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    اگر (!ONE_کھڑکی && curwin->w_چوڑائی < p_wiw)
	win_setwidth((صحیح)p_wiw);

    واپس errmsg;
}

#اگر defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    حرف *
did_سیٹ_wlsteal(optset_T *دلائل غیر_مستعمل)
{
    clip_reset_wayland();

    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    حرف *
did_سیٹ_wltimeoutlen(optset_T *دلائل)
{
    اگر (p_wtm < 0)
    {
	p_wtm = دلائل->os_oldval.نمبر;
	واپس e_دلیل_must_be_مثبت;
    }

    واپس تہی;
}
#اگر_ختم

/*
 * Process the updated 'wrap' option value.
 */
    حرف *
did_سیٹ_wrap(optset_T *دلائل غیر_مستعمل)
{
    // Set w_leftcol or w_skipcol to zero.
    اگر (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    ورنہ
	curwin->w_skipcol = 0;

    واپس تہی;
}

#اگر_تعریف_شدہ FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    حرف *
did_سیٹ_xhistory(optset_T *دلائل)
{
    صحیح is_p_chi = (طویل*)دلائل->os_varp == &p_chi;
    صحیح err;
    طویل *arg = (is_p_chi) ? &p_chi :(طویل*)دلائل->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    اگر (*arg < 1)
    {
	*arg = دلائل->os_oldval.نمبر;
	واپس e_cannot_have_negative_or_zero_نمبر_کا_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    اگر (*arg > 100)
    {
	*arg = دلائل->os_oldval.نمبر;
	واپس e_cannot_have_مزید_than_hundred_quickfix;
    }

    اگر (is_p_chi)
	err = qf_resize_ڈھیر(*arg);
    ورنہ
	err = ll_resize_ڈھیر(curwin, *arg);

    اگر (err == FAIL)
	واپس e_failed_resizing_quickfix_ڈھیر;

    واپس تہی;
}
#اگر_ختم

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ساکن حرف *
سیٹ_bool_option(
    صحیح		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    صحیح		قدر,			// new value
    صحیح		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    صحیح		old_قدر = *(صحیح *)varp;
#اگر defined(FEAT_EVAL)
    صحیح		old_global_قدر = 0;
#اگر_ختم
    حرف	*errmsg = تہی;

    // Disallow changing some options from secure mode
    اگر ((secure
#اگر_تعریف_شدہ HAVE_SANDBOX
		|| sandbox != 0
#اگر_ختم
		) && (options[opt_idx].flags & P_SECURE))
	واپس e_not_allowed_یہاں;

#اگر defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_قدر = *(صحیح *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#اگر_ختم

    *(صحیح *)varp = قدر;	    // set the new value
#اگر_تعریف_شدہ FEAT_EVAL
    // Remember where the option was set.
    سیٹ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

    // May set global value for local option.
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(صحیح *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = قدر;

    // Handle side effects of changing a bool option.
    اگر (options[opt_idx].opt_did_سیٹ_cb != تہی)
    {
	optset_T دلائل;

	CLEAR_میدان(دلائل);
	دلائل.os_varp = varp;
	دلائل.os_flags = opt_flags;
	دلائل.os_oldval.boolean = old_قدر;
	دلائل.os_newval.boolean = قدر;
	دلائل.os_errbuf = تہی;
	errmsg = options[opt_idx].opt_did_سیٹ_cb(&دلائل);
	اگر (errmsg != تہی)
	    واپس errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_سیٹ;

#اگر defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(طویل)(old_قدر ? TRUE : FALSE),
				(طویل)(old_global_قدر ? TRUE : FALSE),
				(طویل)(قدر ? TRUE : FALSE), تہی);
#اگر_ختم

    comp_ستون();			    // in case 'ruler' or 'showcmd' changed

    اگر (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_سیٹ_curswant = true;

    اگر ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    واپس errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    ساکن حرف *
check_عدد_option_bounds(
    طویل	*pp,
    طویل	old_قدر,
    طویل	old_صفیں_2,
    طویل	old_Columns,
    حرف	*errbuf,
    size_t	errbuflen,
    حرف	*errmsg)
{
    اگر (صفیں < min_صفیں_for_all_tabpages() && بھرا_screen)
    {
	اگر (errbuf != تہی)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_کم_سے_کم_nr_سطریں), min_صفیں_for_all_tabpages());
	    errmsg = errbuf;
	}
	صفیں = min_صفیں_for_all_tabpages();
    }
    اگر (Columns < MIN_COLUMNS && بھرا_screen)
    {
	اگر (errbuf != تہی)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_کم_سے_کم_nr_columns), MIN_COLUMNS);
	    errmsg = errbuf;
	}
	Columns = MIN_COLUMNS;
    }
    حد_بندی_screen_سائز();

    // If the screen (shell) height has been changed, assume it is the
    // physical screenheight.
    اگر (old_صفیں_2 != صفیں || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	اگر (updating_screen)
	    *pp = old_قدر;
	ورنہ اگر (بھرا_screen
#اگر_تعریف_شدہ FEAT_GUI
		&& !gui.starting
#اگر_ختم
		)
	    سیٹ_shellsize((صحیح)Columns, (صحیح)صفیں, TRUE);
	ورنہ
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    اگر (cmdline_صف > صفیں - p_ch && صفیں > p_ch)
		cmdline_صف = صفیں - p_ch;
	}
	اگر (p_کھڑکی >= صفیں || !option_was_سیٹ((char_u *)"window"))
	    p_کھڑکی = صفیں - 1;
    }

    اگر (p_tm < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_tm = 0;
    }
    اگر ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_اونچائی
		    && curwin->w_اونچائی > 0))
	    && بھرا_screen)
    {
	اگر (pp == &(curwin->w_p_scr))
	{
	    اگر (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_سائز;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	ورنہ اگر (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	ورنہ // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_اونچائی;
    }
    اگر (p_hi < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_hi = 0;
    }
    ورنہ اگر (p_hi > 10000)
    {
	errmsg = e_invalid_دلیل;
	p_hi = 10000;
    }
    اگر (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_دلیل;
	p_re = 0;
    }
    اگر (p_report < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_report = 1;
    }
    اگر ((p_sj < -100 || p_sj >= صفیں) && بھرا_screen)
    {
	اگر (صفیں != old_صفیں_2)	// Rows changed, just adjust p_sj
	    p_sj = صفیں / 2;
	ورنہ
	{
	    errmsg = e_invalid_scroll_سائز;
	    p_sj = 1;
	}
    }
    اگر (p_so < 0 && بھرا_screen)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_so = 0;
    }
    اگر (p_sop < 0 && بھرا_screen)
    {
	errmsg = e_invalid_دلیل;
	p_sop = 0;
    }
    اگر (p_siso < 0 && بھرا_screen)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_siso = 0;
    }
    اگر (p_cwh < 1)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_cwh = 1;
    }
    اگر (p_ut < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_ut = 2000;
    }
    اگر (p_ss < 0)
    {
	errmsg = e_دلیل_must_be_مثبت;
	p_ss = 0;
    }

    واپس errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    ساکن حرف *
سیٹ_عدد_option(
    صحیح		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    طویل	قدر,			// new value
    حرف	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    صحیح		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    حرف	*errmsg = تہی;
    طویل	old_قدر = *(طویل *)varp;
#اگر defined(FEAT_EVAL)
    طویل	old_global_قدر = 0;	// only used when setting a local and
					// global option
#اگر_ختم
    طویل	old_صفیں_2 = صفیں;	// remember old Rows
    طویل	old_Columns = Columns;	// remember old Columns
    طویل	*pp = (طویل *)varp;

    // Disallow changing some options from secure mode.
    اگر ((secure
#اگر_تعریف_شدہ HAVE_SANDBOX
		|| sandbox != 0
#اگر_ختم
		) && (options[opt_idx].flags & P_SECURE))
	واپس e_not_allowed_یہاں;

#اگر defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_قدر = *(طویل *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#اگر_ختم

    *pp = قدر;
#اگر_تعریف_شدہ FEAT_EVAL
    // Remember where the option was set.
    سیٹ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

    // May set global value for local option.
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(طویل *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = قدر;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    اگر (options[opt_idx].opt_did_سیٹ_cb != تہی)
    {
	optset_T دلائل;

	CLEAR_میدان(دلائل);
	دلائل.os_varp = varp;
	دلائل.os_flags = opt_flags;
	دلائل.os_oldval.نمبر = old_قدر;
	دلائل.os_newval.نمبر = قدر;
	دلائل.os_errbuf = تہی;
	errmsg = options[opt_idx].opt_did_سیٹ_cb(&دلائل);
    }

    // Check the bounds for numeric options here
    errmsg = check_عدد_option_bounds(pp, old_قدر, old_صفیں_2, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_سیٹ;

#اگر defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_قدر, old_global_قدر,
								قدر, errmsg);
#اگر_ختم

    comp_ستون();			    // in case 'columns' or 'ls' changed

    اگر (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_سیٹ_curswant = true;

    اگر ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    واپس errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    خالی
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    صحیح		doclear = (flags & P_RCLR) == P_RCLR;
    صحیح		all = ((flags & P_RALL) == P_RALL || doclear);

    اگر ((flags & P_RSTAT) || all)	// mark all status lines dirty
	حالت_redraw_all();

    اگر ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	اگر (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	ورنہ
	    بدلا_کھڑکی_setting();
    }
    اگر (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    اگر (doclear)
	redraw_all_later(UPD_CLEAR);
    ورنہ اگر (all)
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
    حرف	    *s, *p;
    ساکن مختصر    تیز_tab[27] = {0, 0};	// quick access table
    صحیح		    is_رکن_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    اگر (تیز_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	صحیح آخری_opt_idx;
	برائے (آخری_opt_idx = 0; options[آخری_opt_idx].fullname != تہی;
		آخری_opt_idx++)
	    ;
	برائے (صحیح tab_idx = 1; tab_idx < 27; tab_idx++)
	    تیز_tab[tab_idx] = آخری_opt_idx;

	p = options[0].fullname;
	برائے (opt_idx = 1; opt_idx < آخری_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    اگر (s[0] != p[0])
	    {
		اگر (s[0] == 't' && s[1] == '_')
		    تیز_tab[26] = opt_idx;
		ورنہ
		    تیز_tab[Char_Ord_کم(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    اگر (arg[0] < 'a' || arg[0] > 'z')
	واپس -1;

    is_رکن_opt = (arg[0] == 't' && arg[1] == '_');
    اگر (is_رکن_opt)
	opt_idx = تیز_tab[26];
    ورنہ
	opt_idx = تیز_tab[Char_Ord_کم(arg[0])];
    برائے (; (s = options[opt_idx].fullname) != تہی && s[0] == arg[0]; opt_idx++)
    {
	اگر (STRCMP(arg, s) == 0)		    // match full name
	    توڑو;
    }
    اگر (s != تہی && s[0] != arg[0])
	s = تہی;
    اگر (s == تہی && !is_رکن_opt)
    {
	opt_idx = تیز_tab[Char_Ord_کم(arg[0])];
	برائے ( ; options[opt_idx].fullname != تہی; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    اگر (s != تہی && STRCMP(arg, s) == 0)   // match short name
		توڑو;
	    s = تہی;
	}
    }
    اگر (s == تہی)
	opt_idx = -1;
    واپس opt_idx;
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
get_option_قدر(
    char_u	*name,
    طویل	*numval,
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
				  && (کلید = ڈھونڈو_کلید_option(name, FALSE)) != 0)
	{
	    char_u کلید_name[2];
	    char_u *p;

	    اگر (flagsp != تہی)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    اگر (کلید < 0)
	    {
		کلید_name[0] = KEY2TERMCAP0(کلید);
		کلید_name[1] = KEY2TERMCAP1(کلید);
	    }
	    ورنہ
	    {
		کلید_name[0] = KS_کلید;
		کلید_name[1] = (کلید & 0xff);
	    }
	    p = ڈھونڈو_termcode(کلید_name);
	    اگر (p != تہی)
	    {
		اگر (stringval != تہی)
		    *stringval = vim_strsave(p);
		واپس gov_string;
	    }
	}
	واپس gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    اگر (flagsp != تہی)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    اگر (options[opt_idx].flags & P_STRING)
    {
	اگر (varp == تہی)		    // hidden option
	    واپس gov_hidden_string;
	اگر (stringval != تہی)
	{
	    اگر ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    ورنہ اگر ((char_u **)varp == &curbuf->b_p_کلید
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    ورنہ
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	واپس gov_string;
    }

    اگر (varp == تہی)		    // hidden option
	واپس (options[opt_idx].flags & P_عدد)
					 ? gov_hidden_نمبر : gov_hidden_bool;
    اگر (options[opt_idx].flags & P_عدد)
	*numval = *(طویل *)varp;
    ورنہ
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	اگر ((صحیح *)varp == &curbuf->b_بدلا)
	    *numval = curbuf_Is_بدلا();
	ورنہ
	    *numval = (طویل) *(صحیح *)varp;
    }
    واپس (options[opt_idx].flags & P_عدد) ? gov_نمبر : gov_bool;
}
#اگر_ختم

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
get_option_قدر_strict(
    char_u	*name,
    طویل	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    صحیح		opt_type,
    خالی	*from)
{
    صحیح		opt_idx;
    char_u	*varp = تہی;
    ساخت vimoption *p;
    صحیح		r = 0;

    opt_idx = findoption(name);
    اگر (opt_idx < 0)
	واپس 0;

    p = &(options[opt_idx]);

    // Hidden option
    اگر (p->var == تہی)
	واپس 0;

    اگر (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    ورنہ اگر (p->flags & P_عدد)
	r |= SOPT_عدد;
    ورنہ اگر (p->flags & P_STRING)
	r |= SOPT_STRING;

    اگر (p->indir == PV_NONE)
    {
	اگر (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	ورنہ
	    واپس 0; // Did not request global-only option
    }
    ورنہ
    {
	اگر (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	ورنہ اگر (opt_type == SREQ_GLOBAL)
	    واپس 0; // Requested global option

	اگر (p->indir & PV_WIN)
	{
	    اگر (opt_type == SREQ_BUF)
		واپس 0; // Did not request window-local option
	    ورنہ
		r |= SOPT_WIN;
	}
	ورنہ اگر (p->indir & PV_BUF)
	{
	    اگر (opt_type == SREQ_WIN)
		واپس 0; // Did not request buffer-local option
	    ورنہ
		r |= SOPT_BUF;
	}
    }

    اگر (stringval == تہی)
	واپس r;

    اگر (opt_type == SREQ_GLOBAL)
	varp = p->var;
    ورنہ
    {
	اگر (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    اگر (p->indir == PV_MOD)
	    {
		*numval = buf_Is_بدلا((buf_T *)from);
		varp = تہی;
	    }
# ifdef FEAT_CRYPT
	    ورنہ اگر (p->indir == PV_کلید)
	    {
		// never return the value of the crypt key
		*stringval = تہی;
		varp = تہی;
	    }
# endif
	    ورنہ
	    {
		buf_T *save_curbuf = curbuf;

		// only getting a pointer, no need to use aucmd_prepbuf()
		curbuf = (buf_T *)from;
		curwin->w_بفر = curbuf;
		varp = get_varp(p);
		curbuf = save_curbuf;
		curwin->w_بفر = curbuf;
	    }
	}
	ورنہ اگر (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_بفر;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_بفر;
	}
	اگر (varp == p->var)
	    واپس (r | SOPT_UNSET);
    }

    اگر (varp != تہی)
    {
	اگر (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	ورنہ اگر (p->flags & P_عدد)
	    *numval = *(طویل *) varp;
	ورنہ
	    *numval = *(صحیح *)varp;
    }

    واپس r;
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
option_iter_اگلا(خالی **option, صحیح opt_type)
{
    ساخت vimoption	*ret = تہی;
    کرو
    {
	اگر (*option == تہی)
	    *option = (خالی *) options;
	ورنہ اگر (((ساخت vimoption *) (*option))->fullname == تہی)
	{
	    *option = تہی;
	    واپس تہی;
	}
	ورنہ
	    *option = (خالی *) (((ساخت vimoption *) (*option)) + 1);

	ret = ((ساخت vimoption *) (*option));

	// Hidden option
	اگر (ret->var == تہی)
	{
	    ret = تہی;
	    جاری;
	}

	انتخاب (opt_type)
	{
	    حالت SREQ_GLOBAL:
		اگر (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = تہی;
		توڑو;
	    حالت SREQ_BUF:
		اگر (!(ret->indir & PV_BUF))
		    ret = تہی;
		توڑو;
	    حالت SREQ_WIN:
		اگر (!(ret->indir & PV_WIN))
		    ret = تہی;
		توڑو;
	    طےشدہ:
		اندرونی_error("option_iter_next()");
		واپس تہی;
	}
    }
    جب (ret == تہی);

    واپس (char_u *)ret->fullname;
}
#اگر_ختم

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(صحیح opt_idx)
{
    واپس options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    خالی
سیٹ_option_flag(صحیح opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    صحیح
is_global_option(صحیح opt_idx)
{
    واپس options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    صحیح
is_global_local_option(صحیح opt_idx)
{
    واپس options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    صحیح
is_کھڑکی_local_option(صحیح opt_idx)
{
    واپس options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    صحیح
is_hidden_option(صحیح opt_idx)
{
    واپس options[opt_idx].var == تہی;
}

#اگر defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    صحیح
is_crypt_کلید_option(صحیح opt_idx)
{
    واپس options[opt_idx].indir == PV_کلید;
}
#اگر_ختم

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    حرف *
سیٹ_option_قدر(
    char_u	*name,
    طویل	نمبر,
    char_u	*string,
    صحیح		opt_flags)	// OPT_LOCAL or 0 (both)
{
    صحیح		opt_idx;
    char_u	*varp;
    long_u	flags;
    ساکن حرف	errbuf[ERR_BUFLEN];
    صحیح		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    اگر (opt_idx < 0)
    {
	صحیح کلید;

	اگر (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (کلید = ڈھونڈو_کلید_option(name, FALSE)) != 0)
	{
	    char_u کلید_name[2];

	    اگر (کلید < 0)
	    {
		کلید_name[0] = KEY2TERMCAP0(کلید);
		کلید_name[1] = KEY2TERMCAP1(کلید);
	    }
	    ورنہ
	    {
		کلید_name[0] = KS_کلید;
		کلید_name[1] = (کلید & 0xff);
	    }
	    جوڑو_termcode(کلید_name, string, FALSE);
	    اگر (بھرا_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    واپس تہی;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    ورنہ
    {
	flags = options[opt_idx].flags;
#اگر_تعریف_شدہ HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	اگر (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_اندر_sandbox));
	    واپس تہی;
	}
#اگر_ختم
	اگر (flags & P_STRING)
	    واپس سیٹ_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	اگر (varp != تہی)	// hidden option is not changed
	{
	    اگر (نمبر == 0 && string != تہی)
	    {
		صحیح idx;

		// Either we are given a string or we are setting option
		// to zero.
		برائے (idx = 0; string[idx] == '0'; ++idx)
		    ;
		اگر (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_نمبر_required_after_str_equal_str),
								 name, string);
		    واپس تہی;     // do nothing as we hit an error

		}
	    }
	    اگر (flags & P_عدد)
	    {
		واپس سیٹ_عدد_option(opt_idx, varp, نمبر,
					   errbuf, حجم(errbuf), opt_flags);
	    }
	    ورنہ
		واپس سیٹ_bool_option(opt_idx, varp, (صحیح)نمبر, opt_flags);
	}

    }
    واپس تہی;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    خالی
سیٹ_option_قدر_give_err(
    char_u	*name,
    طویل	نمبر,
    char_u	*string,
    صحیح		opt_flags)	// OPT_LOCAL or 0 (both)
{
    حرف *errmsg = سیٹ_option_قدر(name, نمبر, string, opt_flags);

    اگر (errmsg != تہی)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_رکن_رمز(char_u *tname)
{
    صحیح	    opt_idx;
    char_u  *varp;

    اگر (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	واپس تہی;
    اگر ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	اگر (varp != تہی)
	    varp = *(char_u **)(varp);
	واپس varp;
    }
    واپس ڈھونڈو_termcode(tname + 2);
}

    char_u *
get_highlight_default(خالی)
{
    صحیح i;

    i = findoption((char_u *)"hl");
    اگر (i >= 0)
	واپس options[i].def_val[VI_DEFAULT];
    واپس (char_u *)تہی;
}

    char_u *
get_encoding_default(خالی)
{
    صحیح i;

    i = findoption((char_u *)"enc");
    اگر (i >= 0)
	واپس options[i].def_val[VI_DEFAULT];
    واپس (char_u *)تہی;
}

#اگر defined(FEAT_QUICKFIX)
    صحیح
is_option_allocated(حرف *name)
{
    صحیح idx = findoption((char_u *)name);

    واپس idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#اگر_ختم

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    ساکن صحیح
ڈھونڈو_کلید_option(char_u *arg_arg, صحیح has_lt)
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
    ورنہ اگر (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	کلید = ڈھونڈو_special_کلید(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_کلید | FSK_SIMPLIFY, تہی);
	اگر (modifiers)		    // can't handle modifiers here
	    کلید = 0;
    }
    واپس کلید;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    ساکن خالی
showoptions(
    صحیح		all,
    صحیح		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    ساخت vimoption	*p;
    صحیح			ستون;
    صحیح			isterm;
    char_u		*varp;
    ساخت vimoption	**اشیاء;
    صحیح			شے_گنتی;
    صحیح			تسلسل;
    صحیح			صف, صفیں_2;
    صحیح			ستون_تعداد;
    صحیح			i;
    صحیح			len;

#تعریف_کرو INC 20
#تعریف_کرو GAP 3

    اشیاء = ALLOC_MULT(ساخت vimoption *, OPTION_گنتی);
    اگر (اشیاء == تہی)
	واپس;

    // Highlight title
    اگر (all == 2)
	پیغام_puts_title(_("\n--- Terminal codes ---"));
    ورنہ اگر (opt_flags & OPT_GLOBAL)
	پیغام_puts_title(_("\n--- Global option values ---"));
    ورنہ اگر (opt_flags & OPT_LOCAL)
	پیغام_puts_title(_("\n--- Local option values ---"));
    ورنہ
	پیغام_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    برائے (تسلسل = 1; تسلسل <= 2 && !got_int; ++تسلسل)
    {
	// collect the items in items[]
	شے_گنتی = 0;
	برائے (p = &options[0]; p->fullname != تہی; p++)
	{
	    // apply :filter /pat/
	    اگر (پیغام_filtered((char_u *)p->fullname))
		جاری;

	    varp = تہی;
	    isterm = istermoption(p);
	    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		اگر (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    ورنہ
		varp = get_varp(p);
	    اگر (varp != تہی
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		اگر (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		ورنہ اگر (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		ورنہ
		{
		    option_value2string(p, opt_flags);
		    len = (صحیح)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		اگر ((len <= INC - GAP && تسلسل == 1) ||
						(len > INC - GAP && تسلسل == 2))
		    اشیاء[شے_گنتی++] = p;
	    }
	}

	// display the items
	اگر (تسلسل == 1)
	{
	    ستون_تعداد = (Columns + GAP - 3) / INC;
	    اگر (ستون_تعداد == 0)
		ستون_تعداد = 1;
	    صفیں_2 = (شے_گنتی + ستون_تعداد - 1) / ستون_تعداد;
	}
	ورنہ	// run == 2
	    صفیں_2 = شے_گنتی;
	برائے (صف = 0; صف < صفیں_2 && !got_int; ++صف)
	{
	    پیغام_putchar('\n');			// go to next line
	    اگر (got_int)			// 'q' typed in more
		توڑو;
	    ستون = 0;
	    برائے (i = صف; i < شے_گنتی; i += صفیں_2)
	    {
		پیغام_ستون = ستون;			// make columns
		showoneopt(اشیاء[i], opt_flags);
		ستون += INC;
	    }
	    برآمد_flush();
	    ui_breakcheck();
	}
    }
    vim_free(اشیاء);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    ساکن صحیح
optval_default(ساخت vimoption *p, char_u *varp, صحیح compatible)
{
    صحیح		dvi;

    اگر (varp == تہی)
	واپس TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    اگر (p->flags & P_عدد)
	واپس (*(طویل *)varp == (طویل)(long_i)p->def_val[dvi]);
    اگر (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	واپس (*(صحیح *)varp == (صحیح)(طویل)(long_i)p->def_val[dvi]);
    // P_STRING
    واپس (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    ساکن خالی
showoneopt(
    ساخت vimoption	*p,
    صحیح			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    صحیح		save_silent = silent_منوال;

    silent_منوال = FALSE;
    info_پیغام = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    اگر ((p->flags & P_BOOL) && ((صحیح *)varp == &curbuf->b_بدلا
					? !curbuf_Is_بدلا() : !*(صحیح *)varp))
	پیغام_puts("no");
    ورنہ اگر ((p->flags & P_BOOL) && *(صحیح *)varp < 0)
	پیغام_puts("--");
    ورنہ
	پیغام_puts("  ");
    پیغام_puts(p->fullname);
    اگر (!(p->flags & P_BOOL))
    {
	پیغام_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	پیغام_outtrans(NameBuff);
    }

    silent_منوال = save_silent;
    info_پیغام = FALSE;
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
    ساخت vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = تہی;	// fresh value
    حرف		*cmd;
    صحیح			باری;
    صحیح			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    برائے (pri = 1; pri >= 0; --pri)
    {
      برائے (p = &options[0]; !istermoption(p); p++)
	اگر (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    اگر (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		جاری;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    اگر ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		جاری;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    اگر ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		جاری;

	    اگر ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		جاری;

	    باری = 2;
	    اگر (p->indir != PV_NONE)
	    {
		اگر (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    اگر (!(opt_flags & OPT_LOCAL))
			جاری;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    اگر (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			اگر (!optval_default(p, varp_fresh, p_cp))
			{
			    باری = 1;
			    varp_local = varp;
			    varp = varp_fresh;
			}
		    }
		}
	    }

	    // Round 1: fresh value for window-local options.
	    // Round 2: other values
	    برائے ( ; باری <= 2; varp = varp_local, ++باری)
	    {
		اگر (باری == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		ورنہ
		    cmd = "setlocal";

		اگر (p->flags & P_BOOL)
		{
		    اگر (put_setbool(fd, cmd, p->fullname, *(صحیح *)varp) == FAIL)
			واپس FAIL;
		}
		ورنہ اگر (p->flags & P_عدد)
		{
		    اگر (put_setnum(fd, cmd, p->fullname, (طویل *)varp) == FAIL)
			واپس FAIL;
		}
		ورنہ    // P_STRING
		{
		    صحیح		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    اگر ((p->flags & P_FUNC) && *(char_u **)varp != تہی
			    && لڑی_تلاش(*(حرف **)varp, "<lambda>") != تہی)
			جاری;

#اگر_تعریف_شدہ FEAT_EVAL
		    legacy = !is_option_قدر_vim9(p - &options[0],
			    باری == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#ورنہ
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#اگر_ختم

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    اگر (
#اگر defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#اگر_ختم
			    p->indir == PV_FT)
		    {
			اگر (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    واپس FAIL;
			do_endif = TRUE;
		    }
		    اگر (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			واپس FAIL;
		    اگر (do_endif)
		    {
			اگر (put_سطر(fd, "endif") == FAIL)
			    واپس FAIL;
		    }
		}
	    }
	}
    }
    واپس OK;
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
			&& fde_script_ctx->sc_نسخہ < SCRIPT_نسخہ_VIM9;
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
	واپس FAIL;

    واپس OK;
}
#اگر_ختم

    ساکن صحیح
put_setstring(
    FILE	*fd,
    bool	legacy,
    حرف	*cmd,
    حرف	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = تہی;
    char_u	*part = تہی;
    char_u	*p;

    اگر (legacy && fprintf(fd, "legacy ") < 0)
	واپس FAIL;
    اگر (fprintf(fd, "%s %s=", cmd, name) < 0)
	واپس FAIL;
    اگر (*valuep != تہی)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	اگر (valuep == &p_pt)
	{
	    s = *valuep;
	    جب (*s != NUL)
		اگر (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    واپس FAIL;
	}
	// expand the option value, replace $HOME by ~
	ورنہ اگر ((flags & P_EXPAND) != 0)
	{
	    صحیح  سائز = (صحیح)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(سائز);
	    اگر (buf == تہی)
		جاؤ fail;
	    home_replace(تہی, *valuep, buf, سائز, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    اگر (سائز >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != تہی)
	    {
		part = alloc(سائز);
		اگر (part == تہی)
		    جاؤ fail;

		// write line break to clear the option, e.g. ':set rtp='
		اگر (put_eol(fd) == FAIL)
		    جاؤ fail;

		p = buf;
		جب (*p != NUL)
		{
		    اگر (legacy && fprintf(fd, "legacy ") < 0)
			واپس FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    اگر (fprintf(fd, "%s %s+=", cmd, name) < 0)
			جاؤ fail;
		    (خالی)copy_option_part(&p, part, سائز,  ",");
		    اگر (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			جاؤ fail;
		}
		vim_free(buf);
		vim_free(part);
		واپس OK;
	    }
	    اگر (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		واپس FAIL;
	    }
	    vim_free(buf);
	}
	ورنہ اگر (put_escstr(fd, *valuep, 2) == FAIL)
	    واپس FAIL;
    }
    اگر (put_eol(fd) < 0)
	واپس FAIL;
    واپس OK;
fail:
    vim_free(buf);
    vim_free(part);
    واپس FAIL;
}

    ساکن صحیح
put_setnum(
    FILE	*fd,
    حرف	*cmd,
    حرف	*name,
    طویل	*valuep)
{
    طویل	wc;

    اگر (fprintf(fd, "%s %s=", cmd, name) < 0)
	واپس FAIL;
    اگر (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	اگر (fputs((حرف *)get_special_کلید_name((صحیح)wc, 0), fd) < 0)
	    واپس FAIL;
    }
    ورنہ اگر (fprintf(fd, "%ld", *valuep) < 0)
	واپس FAIL;
    اگر (put_eol(fd) < 0)
	واپس FAIL;
    واپس OK;
}

    ساکن صحیح
put_setbool(
    FILE	*fd,
    حرف	*cmd,
    حرف	*name,
    صحیح		قدر)
{
    اگر (قدر < 0)	// global/local option using global value
	واپس OK;
    اگر (fprintf(fd, "%s %s%s", cmd, قدر ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	واپس FAIL;
    واپس OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    خالی
clear_termoptions(خالی)
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
#اگر_ختم
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    خالی
free_termoptions(خالی)
{
    ساخت vimoption   *p;

    برائے (p = options; p->fullname != تہی; p++)
	اگر (istermoption(p))
	{
	    اگر (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    اگر (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = خالی_خانے_option;
	    p->def_val[VI_DEFAULT] = خالی_خانے_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#اگر_تعریف_شدہ FEAT_EVAL
	    // remember where the option was cleared
	    سیٹ_option_sctx_idx((صحیح)(p - options), OPT_GLOBAL, current_sctx);
#اگر_ختم
	}
    clear_termcodes();
}

/*
 * Free the string for one term option, if it was allocated.
 * Set the string to empty_option and clear allocated flag.
 * "var" points to the option value.
 */
    خالی
free_one_termoption(char_u *var)
{
    ساخت vimoption   *p;

    برائے (p = &options[0]; p->fullname != تہی; p++)
	اگر (p->var == var)
	{
	    اگر (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = خالی_خانے_option;
	    p->flags &= ~P_ALLOCED;
	    توڑو;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    خالی
سیٹ_رکن_defaults(خالی)
{
    ساخت vimoption   *p;

    برائے (p = &options[0]; p->fullname != تہی; p++)
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
    ساکن صحیح
istermoption(ساخت vimoption *p)
{
    واپس (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    ساکن صحیح
istermoption_idx(صحیح opt_idx)
{
    واپس istermoption(&options[opt_idx]);
}

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    خالی
unset_global_local_option(char_u *name, خالی *from)
{
    ساخت vimoption *p;
    صحیح		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    اگر (opt_idx < 0)
	واپس;
    p = &(options[opt_idx]);

    انتخاب ((صحیح)p->indir)
    {
	// global option with local value: use local value if it's been set
	حالت PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    توڑو;
	حالت PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    توڑو;
	حالت PV_راستہ:
	    clear_string_option(&buf->b_p_راستہ);
	    توڑو;
	حالت PV_AC:
	    buf->b_p_ac = -1;
	    توڑو;
	حالت PV_AR:
	    buf->b_p_ar = -1;
	    توڑو;
# ifdef HAVE_FSYNC
	حالت PV_FS:
	    buf->b_p_fs = -1;
	    توڑو;
# endif
	حالت PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    توڑو;
	حالت PV_لیبل_فہرست:
	    clear_string_option(&buf->b_p_لیبل_فہرست);
	    توڑو;
	حالت PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    توڑو;
	حالت PV_SISO:
	    curwin->w_p_siso = -1;
	    توڑو;
	حالت PV_SO:
	    curwin->w_p_so = -1;
	    توڑو;
	حالت PV_SOP:
	    curwin->w_p_sop = -1;
	    توڑو;
# ifdef FEAT_ڈھونڈو_ID
	حالت PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    توڑو;
	حالت PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    توڑو;
# endif
	حالت PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    توڑو;
	حالت PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    توڑو;
# ifdef FEAT_DIFF
	حالت PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    توڑو;
# endif
	حالت PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    توڑو;
# ifdef FEAT_COMPL_FUNC
	حالت PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    توڑو;
# endif
	حالت PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    توڑو;
# ifdef FEAT_EVAL
	حالت PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    توڑو;
# endif
# ifdef FEAT_QUICKFIX
	حالت PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    توڑو;
	حالت PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    توڑو;
	حالت PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    توڑو;
	حالت PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    توڑو;
# endif
# اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	حالت PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    توڑو;
# endif
# اگر defined(FEAT_CRYPT)
	حالت PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    توڑو;
# endif
# ifdef FEAT_LINEBREAK
	حالت PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    توڑو;
# endif
# ifdef FEAT_STL_OPT
	حالت PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    توڑو;
	حالت PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    توڑو;
# endif
	حالت PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    توڑو;
	حالت PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    توڑو;
	حالت PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    توڑو;
	حالت PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    سیٹ_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      تہی, 0);
	    redraw_later(UPD_NOT_VALID);
	    توڑو;
	حالت PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    سیٹ_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      تہی, 0);
	    redraw_later(UPD_NOT_VALID);
	    توڑو;
	حالت PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    توڑو;
    }
}
#اگر_ختم

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    ساکن char_u *
get_varp_scope(ساخت vimoption *p, صحیح scope)
{
    اگر ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	اگر (p->var == VAR_WIN)
	    واپس (char_u *)GLOBAL_WO(get_varp(p));
	واپس p->var;
    }
    اگر ((scope & OPT_LOCAL) && ((صحیح)p->indir & PV_BOTH))
    {
	انتخاب ((صحیح)p->indir)
	{
	    حالت PV_FP:   واپس (char_u *)&(curbuf->b_p_fp);
#اگر_تعریف_شدہ HAVE_FSYNC
	    حالت PV_FS:	واپس (char_u *)&(curbuf->b_p_fs);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
	    حالت PV_FFU: واپس (char_u *)&(curbuf->b_p_ffu);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_QUICKFIX
	    حالت PV_EFM:  واپس (char_u *)&(curbuf->b_p_efm);
	    حالت PV_GEFM: واپس (char_u *)&(curbuf->b_p_gefm);
	    حالت PV_GP:   واپس (char_u *)&(curbuf->b_p_gp);
	    حالت PV_MP:   واپس (char_u *)&(curbuf->b_p_mp);
#اگر_ختم
	    حالت PV_EP:   واپس (char_u *)&(curbuf->b_p_ep);
	    حالت PV_KP:   واپس (char_u *)&(curbuf->b_p_kp);
	    حالت PV_راستہ: واپس (char_u *)&(curbuf->b_p_راستہ);
	    حالت PV_AC:   واپس (char_u *)&(curbuf->b_p_ac);
	    حالت PV_AR:   واپس (char_u *)&(curbuf->b_p_ar);
	    حالت PV_لیبل_فہرست: واپس (char_u *)&(curbuf->b_p_لیبل_فہرست);
	    حالت PV_TC:   واپس (char_u *)&(curbuf->b_p_tc);
	    حالت PV_SISO: واپس (char_u *)&(curwin->w_p_siso);
	    حالت PV_SO:   واپس (char_u *)&(curwin->w_p_so);
	    حالت PV_SOP:  واپس (char_u *)&(curwin->w_p_sop);
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
	    حالت PV_DEF:  واپس (char_u *)&(curbuf->b_p_def);
	    حالت PV_INC:  واپس (char_u *)&(curbuf->b_p_inc);
#اگر_ختم
	    حالت PV_COT:  واپس (char_u *)&(curbuf->b_p_cot);
	    حالت PV_DICT: واپس (char_u *)&(curbuf->b_p_dict);
#اگر_تعریف_شدہ FEAT_DIFF
	    حالت PV_DIA:  واپس (char_u *)&(curbuf->b_p_dia);
#اگر_ختم
	    حالت PV_TSR:  واپس (char_u *)&(curbuf->b_p_tsr);
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
	    حالت PV_TSRFU: واپس (char_u *)&(curbuf->b_p_tsrfu);
#اگر_ختم
#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    حالت PV_BEXPR: واپس (char_u *)&(curbuf->b_p_bexpr);
#اگر_ختم
#اگر defined(FEAT_CRYPT)
	    حالت PV_CM:	  واپس (char_u *)&(curbuf->b_p_cm);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
	    حالت PV_SBR:  واپس (char_u *)&(curwin->w_p_sbr);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_STL_OPT
	    حالت PV_STL:  واپس (char_u *)&(curwin->w_p_stl);
	    حالت PV_STLO: واپس (char_u *)&(curwin->w_p_stlo);
#اگر_ختم
	    حالت PV_UL:   واپس (char_u *)&(curbuf->b_p_ul);
	    حالت PV_LW:   واپس (char_u *)&(curbuf->b_p_lw);
	    حالت PV_BKC:  واپس (char_u *)&(curbuf->b_p_bkc);
	    حالت PV_MENC: واپس (char_u *)&(curbuf->b_p_menc);
	    حالت PV_LCS:  واپس (char_u *)&(curwin->w_p_lcs);
	    حالت PV_FCS:  واپس (char_u *)&(curwin->w_p_fcs);
	    حالت PV_VE:	  واپس (char_u *)&(curwin->w_p_ve);

	}
	واپس تہی; // "cannot happen"
    }
    واپس get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(صحیح opt_idx, صحیح scope)
{
    واپس get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    ساکن char_u *
get_varp(ساخت vimoption *p)
{
    // hidden option, always return NULL
    اگر (p->var == تہی)
	واپس تہی;

    انتخاب ((صحیح)p->indir)
    {
	حالت PV_NONE:	واپس p->var;

	// global option with local value: use local value if it's been set
	حالت PV_EP:	واپس *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	حالت PV_KP:	واپس *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	حالت PV_راستہ:	واپس *curbuf->b_p_راستہ != NUL
				    ? (char_u *)&(curbuf->b_p_راستہ) : p->var;
	حالت PV_AC:	واپس curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	حالت PV_AR:	واپس curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	حالت PV_لیبل_فہرست:	واپس *curbuf->b_p_لیبل_فہرست != NUL
				    ? (char_u *)&(curbuf->b_p_لیبل_فہرست) : p->var;
	حالت PV_TC:	واپس *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	حالت PV_BKC:	واپس *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	حالت PV_SISO:	واپس curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	حالت PV_SO:	واپس curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	حالت PV_SOP:	واپس curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
	حالت PV_DEF:	واپس *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	حالت PV_INC:	واپس *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#اگر_ختم
	حالت PV_COT:	واپس *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	حالت PV_DICT:	واپس *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#اگر_تعریف_شدہ FEAT_DIFF
	حالت PV_DIA:	واپس *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#اگر_ختم
	حالت PV_TSR:	واپس *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
	حالت PV_TSRFU:	واپس *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#اگر_ختم
	حالت PV_FP:	واپس *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#اگر_تعریف_شدہ HAVE_FSYNC
	حالت PV_FS:	واپس curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
	حالت PV_FFU:	واپس *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_QUICKFIX
	حالت PV_EFM:	واپس *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	حالت PV_GEFM:	واپس *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	حالت PV_GP:	واپس *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	حالت PV_MP:	واپس *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#اگر_ختم
#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	حالت PV_BEXPR:	واپس *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#اگر_ختم
#اگر defined(FEAT_CRYPT)
	حالت PV_CM:	واپس *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
	حالت PV_SBR:	واپس *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_STL_OPT
	حالت PV_STL:	واپس *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	حالت PV_STLO:	واپس *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#اگر_ختم
	حالت PV_UL:	واپس curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	حالت PV_LW:	واپس *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	حالت PV_MENC:	واپس *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#اگر_تعریف_شدہ FEAT_ARABIC
	حالت PV_ARAB:	واپس (char_u *)&(curwin->w_p_arab);
#اگر_ختم
	حالت PV_LIST:	واپس (char_u *)&(curwin->w_p_list);
	حالت PV_LCS:	واپس *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	حالت PV_FCS:	واپس *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	حالت PV_VE:	واپس *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#اگر_تعریف_شدہ FEAT_SPELL
	حالت PV_SPELL:	واپس (char_u *)&(curwin->w_p_spell);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
	حالت PV_CUC:	واپس (char_u *)&(curwin->w_p_cuc);
	حالت PV_CUL:	واپس (char_u *)&(curwin->w_p_cul);
	حالت PV_CULOPT:	واپس (char_u *)&(curwin->w_p_culopt);
	حالت PV_CC:	واپس (char_u *)&(curwin->w_p_cc);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
	حالت PV_DIFF:	واپس (char_u *)&(curwin->w_p_diff);
#اگر_ختم
	حالت PV_EIW:	واپس (char_u *)&(curwin->w_p_eiw);
#اگر_تعریف_شدہ FEAT_FOLDING
	حالت PV_FDC:	واپس (char_u *)&(curwin->w_p_fdc);
	حالت PV_FEN:	واپس (char_u *)&(curwin->w_p_fen);
	حالت PV_FDI:	واپس (char_u *)&(curwin->w_p_fdi);
	حالت PV_FDL:	واپس (char_u *)&(curwin->w_p_fdl);
	حالت PV_FDM:	واپس (char_u *)&(curwin->w_p_fdm);
	حالت PV_FML:	واپس (char_u *)&(curwin->w_p_fml);
	حالت PV_FDN:	واپس (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	حالت PV_FDE:	واپس (char_u *)&(curwin->w_p_fde);
	حالت PV_FDT:	واپس (char_u *)&(curwin->w_p_fdt);
# endif
	حالت PV_FMR:	واپس (char_u *)&(curwin->w_p_fmr);
#اگر_ختم
	حالت PV_NU:	واپس (char_u *)&(curwin->w_p_nu);
	حالت PV_RNU:	واپس (char_u *)&(curwin->w_p_rnu);
#اگر_تعریف_شدہ FEAT_LINEBREAK
	حالت PV_NUW:	واپس (char_u *)&(curwin->w_p_nuw);
#اگر_ختم
	حالت PV_WFB:	واپس (char_u *)&(curwin->w_p_wfb);
	حالت PV_WFH:	واپس (char_u *)&(curwin->w_p_wfh);
	حالت PV_WFW:	واپس (char_u *)&(curwin->w_p_wfw);
#اگر defined(FEAT_QUICKFIX)
	حالت PV_PVW:	واپس (char_u *)&(curwin->w_p_pvw);
	حالت PV_LHI:	واپس (char_u *)&(curwin->w_p_lhi);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	حالت PV_RL:	واپس (char_u *)&(curwin->w_p_rl);
	حالت PV_RLC:	واپس (char_u *)&(curwin->w_p_rlc);
#اگر_ختم
	حالت PV_SCROLL:	واپس (char_u *)&(curwin->w_p_scr);
	حالت PV_SMS:	واپس (char_u *)&(curwin->w_p_sms);
	حالت PV_WRAP:	واپس (char_u *)&(curwin->w_p_wrap);
#اگر_تعریف_شدہ FEAT_LINEBREAK
	حالت PV_LBR:	واپس (char_u *)&(curwin->w_p_lbr);
	حالت PV_BRI:	واپس (char_u *)&(curwin->w_p_bri);
	حالت PV_BRIOPT: واپس (char_u *)&(curwin->w_p_briopt);
#اگر_ختم
	حالت PV_WCR:	واپس (char_u *)&(curwin->w_p_wcr);
	حالت PV_SCBIND: واپس (char_u *)&(curwin->w_p_scb);
	حالت PV_CRBIND: واپس (char_u *)&(curwin->w_p_crb);
#اگر_تعریف_شدہ FEAT_CONCEAL
	حالت PV_COCU:   واپس (char_u *)&(curwin->w_p_cocu);
	حالت PV_COLE:   واپس (char_u *)&(curwin->w_p_cole);
#اگر_ختم
	حالت PV_WHL:	واپس (char_u *)&(curwin->w_p_whl);

#اگر_تعریف_شدہ FEAT_TERMINAL
	حالت PV_TWK:    واپس (char_u *)&(curwin->w_p_twk);
	حالت PV_TWS:    واپس (char_u *)&(curwin->w_p_tws);
	حالت PV_TWSL:	واپس (char_u *)&(curbuf->b_p_twsl);
#اگر_ختم

	حالت PV_AI:	واپس (char_u *)&(curbuf->b_p_ai);
	حالت PV_BIN:	واپس (char_u *)&(curbuf->b_p_bin);
	حالت PV_BOMB:	واپس (char_u *)&(curbuf->b_p_bomb);
	حالت PV_BH:	واپس (char_u *)&(curbuf->b_p_bh);
	حالت PV_BT:	واپس (char_u *)&(curbuf->b_p_bt);
	حالت PV_BL:	واپس (char_u *)&(curbuf->b_p_bl);
	حالت PV_CI:	واپس (char_u *)&(curbuf->b_p_ci);
	حالت PV_CIN:	واپس (char_u *)&(curbuf->b_p_cin);
	حالت PV_CINK:	واپس (char_u *)&(curbuf->b_p_cink);
	حالت PV_CINO:	واپس (char_u *)&(curbuf->b_p_cino);
	حالت PV_CINSD:	واپس (char_u *)&(curbuf->b_p_cinsd);
	حالت PV_CINW:	واپس (char_u *)&(curbuf->b_p_cinw);
	حالت PV_COM:	واپس (char_u *)&(curbuf->b_p_com);
	حالت PV_CMS:	واپس (char_u *)&(curbuf->b_p_cms);
	حالت PV_CPT:	واپس (char_u *)&(curbuf->b_p_cpt);
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	حالت PV_CSL:	واپس (char_u *)&(curbuf->b_p_csl);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
	حالت PV_CFU:	واپس (char_u *)&(curbuf->b_p_cfu);
	حالت PV_OFU:	واپس (char_u *)&(curbuf->b_p_ofu);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
	حالت PV_TFU:	واپس (char_u *)&(curbuf->b_p_tfu);
#اگر_ختم
	حالت PV_EOF:	واپس (char_u *)&(curbuf->b_p_eof);
	حالت PV_EOL:	واپس (char_u *)&(curbuf->b_p_eol);
	حالت PV_FIXEOL:	واپس (char_u *)&(curbuf->b_p_fixeol);
	حالت PV_ET:	واپس (char_u *)&(curbuf->b_p_et);
	حالت PV_FENC:	واپس (char_u *)&(curbuf->b_p_fenc);
	حالت PV_FF:	واپس (char_u *)&(curbuf->b_p_ff);
	حالت PV_FT:	واپس (char_u *)&(curbuf->b_p_ft);
	حالت PV_FO:	واپس (char_u *)&(curbuf->b_p_fo);
	حالت PV_FLP:	واپس (char_u *)&(curbuf->b_p_flp);
	حالت PV_IMI:	واپس (char_u *)&(curbuf->b_p_iminsert);
	حالت PV_IMS:	واپس (char_u *)&(curbuf->b_p_imsearch);
	حالت PV_INF:	واپس (char_u *)&(curbuf->b_p_inf);
	حالت PV_ISK:	واپس (char_u *)&(curbuf->b_p_isk);
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
# ifdef FEAT_EVAL
	حالت PV_INEX:	واپس (char_u *)&(curbuf->b_p_inex);
# endif
#اگر_ختم
#اگر defined(FEAT_EVAL)
	حالت PV_INDE:	واپس (char_u *)&(curbuf->b_p_inde);
	حالت PV_INDK:	واپس (char_u *)&(curbuf->b_p_indk);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
	حالت PV_FEX:	واپس (char_u *)&(curbuf->b_p_fex);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
	حالت PV_کلید:	واپس (char_u *)&(curbuf->b_p_کلید);
#اگر_ختم
	حالت PV_LISP:	واپس (char_u *)&(curbuf->b_p_lisp);
	حالت PV_LOP:	واپس (char_u *)&(curbuf->b_p_lop);
	حالت PV_ML:	واپس (char_u *)&(curbuf->b_p_ml);
	حالت PV_MPS:	واپس (char_u *)&(curbuf->b_p_mps);
	حالت PV_MA:	واپس (char_u *)&(curbuf->b_p_ma);
	حالت PV_MOD:	واپس (char_u *)&(curbuf->b_بدلا);
	حالت PV_NF:	واپس (char_u *)&(curbuf->b_p_nf);
	حالت PV_PI:	واپس (char_u *)&(curbuf->b_p_pi);
	حالت PV_QE:	واپس (char_u *)&(curbuf->b_p_qe);
	حالت PV_RO:	واپس (char_u *)&(curbuf->b_p_ro);
	حالت PV_SI:	واپس (char_u *)&(curbuf->b_p_si);
	حالت PV_SN:	واپس (char_u *)&(curbuf->b_p_sn);
	حالت PV_STS:	واپس (char_u *)&(curbuf->b_p_sts);
	حالت PV_SUA:	واپس (char_u *)&(curbuf->b_p_sua);
	حالت PV_SWF:	واپس (char_u *)&(curbuf->b_p_swf);
#اگر_تعریف_شدہ FEAT_SYN_HL
	حالت PV_SMC:	واپس (char_u *)&(curbuf->b_p_smc);
	حالت PV_SYN:	واپس (char_u *)&(curbuf->b_p_syn);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SPELL
	حالت PV_SPC:	واپس (char_u *)&(curwin->w_s->b_p_spc);
	حالت PV_SPF:	واپس (char_u *)&(curwin->w_s->b_p_spf);
	حالت PV_SPL:	واپس (char_u *)&(curwin->w_s->b_p_spl);
	حالت PV_SPO:	واپس (char_u *)&(curwin->w_s->b_p_spo);
#اگر_ختم
	حالت PV_SW:	واپس (char_u *)&(curbuf->b_p_sw);
	حالت PV_TS:	واپس (char_u *)&(curbuf->b_p_ts);
	حالت PV_TW:	واپس (char_u *)&(curbuf->b_p_tw);
	حالت PV_TX:	واپس (char_u *)&(curbuf->b_p_tx);
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
	حالت PV_UDF:	واپس (char_u *)&(curbuf->b_p_udf);
#اگر_ختم
	حالت PV_WM:	واپس (char_u *)&(curbuf->b_p_wm);
#اگر_تعریف_شدہ FEAT_KEYMAP
	حالت PV_KMAP:	واپس (char_u *)&(curbuf->b_p_keymap);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
	حالت PV_SCL:	واپس (char_u *)&(curwin->w_p_scl);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_VARTABS
	حالت PV_VSTS:	واپس (char_u *)&(curbuf->b_p_vsts);
	حالت PV_VTS:	واپس (char_u *)&(curbuf->b_p_vts);
#اگر_ختم
	طےشدہ:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    واپس (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(صحیح opt_idx)
{
    واپس options[opt_idx].var;
}

#اگر defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(صحیح opt_idx)
{
    واپس (char_u *)options[opt_idx].fullname;
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
    ساخت vimoption	*p;
    صحیح			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    اگر ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    اگر (opt_idx < 0)
	واپس FAIL;

    p = &options[opt_idx];
    dict_جوڑو_string(d, "name", (char_u *)p->fullname);
    dict_جوڑو_string(d, "shortname",
		    (char_u *)(p->shortname == تہی ? "" : p->shortname));
    dict_جوڑو_bool(d, "available", p->var != تہی);
    dict_جوڑو_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_عدد) ? "number" : "string"));
    dict_جوڑو_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    اگر (p->flags & P_BOOL)
	dict_جوڑو_bool(d, "default", (صحیح)(long_i)p->def_val[dvi]);
    ورنہ اگر (p->flags & P_عدد)
	dict_جوڑو_نمبر(d, "default", (طویل)(long_i)p->def_val[dvi]);
    ورنہ
	dict_جوڑو_string(d, "default", p->def_val[dvi] == تہی
					? (char_u *)"" : p->def_val[dvi]);
    واپس OK;
}
#اگر_ختم

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_سیٹ_cb_T
get_option_did_سیٹ_cb(صحیح opt_idx)
{
    واپس options[opt_idx].opt_did_سیٹ_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(خالی)
{
    اگر (*curbuf->b_p_ep == NUL)
	واپس p_ep;
    واپس curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(خالی)
{
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (*curbuf->b_p_ffu == NUL)
	واپس p_ffu;
    واپس curbuf->b_p_ffu;
#ورنہ
    واپس (char_u *)"";
#اگر_ختم
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    خالی
win_copy_options(win_T *wp_from, win_T *wp_مقصد)
{
    copy_winopt(&wp_from->w_onebuf_opt, &wp_مقصد->w_onebuf_opt);
    copy_winopt(&wp_from->w_allbuf_opt, &wp_مقصد->w_allbuf_opt);
#اگر_تعریف_شدہ FEAT_STL_OPT
    // w_stl_rendered_height is in win_T directly, not in winvar_T, so it is
    // not copied by copy_winopt().  Copy it explicitly here.
    wp_مقصد->w_stl_rendered_اونچائی = wp_from->w_stl_rendered_اونچائی;
#اگر_ختم
    after_copy_winopt(wp_مقصد);
}

/*
 * After copying window options: update variables depending on options.
 */
    خالی
after_copy_winopt(win_T *wp)
{
    حرف *errmsg = update_winhighlight(wp, wp->w_p_whl);

    اگر (errmsg != تہی)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    اگر (wp->w_p_wrap)
	wp->w_leftcol = 0;
    ورنہ
	wp->w_skipcol = 0;
#اگر_تعریف_شدہ FEAT_LINEBREAK
    briopt_check(تہی, wp);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
    fill_culopt_flags(تہی, wp);
    check_colorcolumn(تہی, wp);
#اگر_ختم
    سیٹ_listchars_option(wp, wp->w_p_lcs, TRUE, تہی, 0);
    سیٹ_fillchars_option(wp, wp->w_p_fcs, TRUE, تہی, 0);
}

    ساکن char_u *
copy_option_val(char_u *val)
{
    اگر (val == خالی_خانے_option)
	واپس خالی_خانے_option;  // no need to allocate memory
    واپس vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    خالی
copy_winopt(winopt_T *from, winopt_T *مقصد)
{
#اگر_تعریف_شدہ FEAT_ARABIC
    مقصد->wo_arab = from->wo_arab;
#اگر_ختم
    مقصد->wo_list = from->wo_list;
    مقصد->wo_lcs = copy_option_val(from->wo_lcs);
    مقصد->wo_fcs = copy_option_val(from->wo_fcs);
    مقصد->wo_nu = from->wo_nu;
    مقصد->wo_rnu = from->wo_rnu;
    مقصد->wo_ve = copy_option_val(from->wo_ve);
    مقصد->wo_ve_flags = from->wo_ve_flags;
#اگر_تعریف_شدہ FEAT_LINEBREAK
    مقصد->wo_nuw = from->wo_nuw;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    مقصد->wo_rl  = from->wo_rl;
    مقصد->wo_rlc = copy_option_val(from->wo_rlc);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
    مقصد->wo_sbr = copy_option_val(from->wo_sbr);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_STL_OPT
    مقصد->wo_stl = copy_option_val(from->wo_stl);
    مقصد->wo_stlo = copy_option_val(from->wo_stlo);
    مقصد->wo_stlo_fh = from->wo_stlo_fh;
    مقصد->wo_stlo_mh = from->wo_stlo_mh;
#اگر_ختم
    مقصد->wo_wrap = from->wo_wrap;
#اگر_تعریف_شدہ FEAT_DIFF
    مقصد->wo_wrap_save = from->wo_wrap_save;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
    مقصد->wo_lbr = from->wo_lbr;
    مقصد->wo_bri = from->wo_bri;
    مقصد->wo_briopt = copy_option_val(from->wo_briopt);
#اگر_ختم
    مقصد->wo_wcr = copy_option_val(from->wo_wcr);
    مقصد->wo_scb = from->wo_scb;
    مقصد->wo_scb_save = from->wo_scb_save;
    مقصد->wo_sms = from->wo_sms;
    مقصد->wo_crb = from->wo_crb;
    مقصد->wo_crb_save = from->wo_crb_save;
    مقصد->wo_siso = from->wo_siso;
    مقصد->wo_so = from->wo_so;
    مقصد->wo_sop = from->wo_sop;
#اگر_تعریف_شدہ FEAT_SPELL
    مقصد->wo_spell = from->wo_spell;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
    مقصد->wo_cuc = from->wo_cuc;
    مقصد->wo_cul = from->wo_cul;
    مقصد->wo_culopt = copy_option_val(from->wo_culopt);
    مقصد->wo_cc = copy_option_val(from->wo_cc);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
    مقصد->wo_diff = from->wo_diff;
    مقصد->wo_diff_saved = from->wo_diff_saved;
#اگر_ختم
    مقصد->wo_eiw = copy_option_val(from->wo_eiw);
#اگر_تعریف_شدہ FEAT_CONCEAL
    مقصد->wo_cocu = copy_option_val(from->wo_cocu);
    مقصد->wo_cole = from->wo_cole;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
    مقصد->wo_twk = copy_option_val(from->wo_twk);
    مقصد->wo_tws = copy_option_val(from->wo_tws);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
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
			       ? vim_strsave(from->wo_fdm_save) : خالی_خانے_option;
    مقصد->wo_fdn = from->wo_fdn;
# ifdef FEAT_EVAL
    مقصد->wo_fde = copy_option_val(from->wo_fde);
    مقصد->wo_fdt = copy_option_val(from->wo_fdt);
# endif
    مقصد->wo_fmr = copy_option_val(from->wo_fmr);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
    مقصد->wo_scl = copy_option_val(from->wo_scl);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_QUICKFIX
    مقصد->wo_lhi = from->wo_lhi;
#اگر_ختم

    مقصد->wo_wrap_flags = from->wo_wrap_flags;
#اگر_تعریف_شدہ FEAT_STL_OPT
    مقصد->wo_stl_flags = from->wo_stl_flags;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
    مقصد->wo_fde_flags = from->wo_fde_flags;
    مقصد->wo_fdt_flags = from->wo_fdt_flags;
#اگر_ختم
    مقصد->wo_whl = copy_option_val(from->wo_whl);

#اگر_تعریف_شدہ FEAT_EVAL
    // Copy the script context so that we know where the value was last set.
    mch_memmove(مقصد->wo_script_ctx, from->wo_script_ctx,
						    حجم(مقصد->wo_script_ctx));
#اگر_ختم
    check_winopt(مقصد);		// don't want NULL pointers
}

/*
 * Check string options in a window for a NULL value.
 */
    ساکن خالی
check_win_options(win_T *win)
{
    check_winopt(&win->w_onebuf_opt);
    check_winopt(&win->w_allbuf_opt);
}

/*
 * Check for NULL pointers in a winopt_T and replace them with empty_option.
 */
    ساکن خالی
check_winopt(winopt_T *wop غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_FOLDING
    check_string_option(&wop->wo_fdi);
    check_string_option(&wop->wo_fdm);
    check_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    check_string_option(&wop->wo_fde);
    check_string_option(&wop->wo_fdt);
# endif
    check_string_option(&wop->wo_fmr);
#اگر_ختم
    check_string_option(&wop->wo_eiw);
#اگر_تعریف_شدہ FEAT_SIGNS
    check_string_option(&wop->wo_scl);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    check_string_option(&wop->wo_rlc);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
    check_string_option(&wop->wo_sbr);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_STL_OPT
    check_string_option(&wop->wo_stl);
    check_string_option(&wop->wo_stlo);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
    check_string_option(&wop->wo_culopt);
    check_string_option(&wop->wo_cc);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
    check_string_option(&wop->wo_cocu);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
    check_string_option(&wop->wo_twk);
    check_string_option(&wop->wo_tws);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
    check_string_option(&wop->wo_briopt);
#اگر_ختم
    check_string_option(&wop->wo_wcr);
    check_string_option(&wop->wo_lcs);
    check_string_option(&wop->wo_fcs);
    check_string_option(&wop->wo_ve);
    check_string_option(&wop->wo_whl);
}

/*
 * Free the allocated memory inside a winopt_T.
 */
    خالی
clear_winopt(winopt_T *wop غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_FOLDING
    clear_string_option(&wop->wo_fdi);
    clear_string_option(&wop->wo_fdm);
    clear_string_option(&wop->wo_fdm_save);
# ifdef FEAT_EVAL
    clear_string_option(&wop->wo_fde);
    clear_string_option(&wop->wo_fdt);
# endif
    clear_string_option(&wop->wo_fmr);
#اگر_ختم
    clear_string_option(&wop->wo_eiw);
#اگر_تعریف_شدہ FEAT_SIGNS
    clear_string_option(&wop->wo_scl);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
    clear_string_option(&wop->wo_briopt);
#اگر_ختم
    clear_string_option(&wop->wo_wcr);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    clear_string_option(&wop->wo_rlc);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
    clear_string_option(&wop->wo_sbr);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_STL_OPT
    clear_string_option(&wop->wo_stl);
    clear_string_option(&wop->wo_stlo);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
    clear_string_option(&wop->wo_culopt);
    clear_string_option(&wop->wo_cc);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
    clear_string_option(&wop->wo_cocu);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
    clear_string_option(&wop->wo_twk);
    clear_string_option(&wop->wo_tws);
#اگر_ختم
    clear_string_option(&wop->wo_lcs);
    clear_string_option(&wop->wo_fcs);
    clear_string_option(&wop->wo_ve);
    clear_string_option(&wop->wo_whl);
}

#اگر_تعریف_شدہ FEAT_EVAL
// Index into the options table for a buffer-local option enum.
ساکن صحیح buf_opt_idx[BV_گنتی];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    ساکن خالی
init_buf_opt_idx(خالی)
{
    ساکن صحیح did_init_buf_opt_idx = FALSE;
    صحیح i;

    اگر (did_init_buf_opt_idx)
	واپس;
    did_init_buf_opt_idx = TRUE;
    برائے (i = 0; !istermoption_idx(i); i++)
	اگر (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_نقاب] = i;
}
#ورنہ
# define COPY_OPT_SCTX(buf, bv)
# define COPY_OPT_INSECURE(flagsfield, bv)
#اگر_ختم

/*
 * Copy global option values to local options for one buffer.
 * Used when creating a new buffer and sometimes when entering a buffer.
 * flags:
 * BCO_ENTER	We will enter the buffer "buf".
 * BCO_ALWAYS	Always copy the options, but only set b_p_initialized when
 *		appropriate.
 * BCO_NOHELP	Don't copy the values to a help buffer.
 */
    خالی
buf_copy_options(buf_T *buf, صحیح flags)
{
    صحیح		should_copy = TRUE;
    char_u	*save_p_isk = تہی;	    // init for GCC
    صحیح		dont_do_help;
    صحیح		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    اگر (p_cpo != تہی)
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
	اگر ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == تہی || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != تہی)))
	    should_copy = FALSE;

	اگر (should_copy || (flags & BCO_ALWAYS))
	{
#اگر_تعریف_شدہ FEAT_EVAL
	    CLEAR_میدان(buf->b_p_script_ctx);
	    init_buf_opt_idx();
#اگر_ختم
	    // Don't copy the options specific to a help buffer when
	    // BCO_NOHELP is given or the options were initialized already
	    // (jumping back to a help file with CTRL-T or CTRL-O)
	    dont_do_help = ((flags & BCO_NOHELP) && buf->b_help)
						       || buf->b_p_initialized;
	    اگر (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = تہی;
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
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); توڑو;
		    حالت 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); توڑو;
		    حالت 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); توڑو;
		    طےشدہ:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		اگر (buf->b_p_ff != تہی)
		    buf->b_آغاز_ffc = *buf->b_p_ff;
		buf->b_p_bh = خالی_خانے_option;
		buf->b_p_bt = خالی_خانے_option;
	    }
	    ورنہ
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
	    ورنہ
	    {
		buf->b_p_swf = p_swf;
		COPY_OPT_SCTX(buf, BV_SWF);
	    }
	    buf->b_p_cpt = vim_strsave(p_cpt);
	    COPY_OPT_SCTX(buf, BV_CPT);
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
	    COPY_OPT_INSECURE(buf->b_p_cpt_flags, BV_CPT);
	    سیٹ_buflocal_cpt_callbacks(buf);
#اگر_ختم
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	    buf->b_p_csl = vim_strsave(p_csl);
	    COPY_OPT_SCTX(buf, BV_CSL);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
	    buf->b_p_cfu = vim_strsave(p_cfu);
	    COPY_OPT_SCTX(buf, BV_CFU);
	    سیٹ_buflocal_cfu_callback(buf);
	    buf->b_p_ofu = vim_strsave(p_ofu);
	    COPY_OPT_SCTX(buf, BV_OFU);
	    سیٹ_buflocal_ofu_callback(buf);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
	    buf->b_p_tfu = vim_strsave(p_tfu);
	    COPY_OPT_SCTX(buf, BV_TFU);
	    سیٹ_buflocal_tfu_callback(buf);
#اگر_ختم
	    buf->b_p_sts = p_sts;
	    COPY_OPT_SCTX(buf, BV_STS);
	    buf->b_p_sts_nopaste = p_sts_nopaste;
#اگر_تعریف_شدہ FEAT_VARTABS
	    buf->b_p_vsts = vim_strsave(p_vsts);
	    COPY_OPT_SCTX(buf, BV_VSTS);
	    اگر (p_vsts && p_vsts != خالی_خانے_option)
		(خالی)tabstop_سیٹ(p_vsts, &buf->b_p_vsts_array);
	    ورنہ
		buf->b_p_vsts_array = تہی;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : تہی;
#اگر_ختم
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
	    buf->b_p_ft = خالی_خانے_option;
	    buf->b_p_pi = p_pi;
	    COPY_OPT_SCTX(buf, BV_PI);
	    buf->b_p_cinw = vim_strsave(p_cinw);
	    COPY_OPT_SCTX(buf, BV_CINW);
	    buf->b_p_lisp = p_lisp;
	    COPY_OPT_SCTX(buf, BV_LISP);
#اگر_تعریف_شدہ FEAT_SYN_HL
	    // Don't copy 'syntax', it must be set
	    buf->b_p_syn = خالی_خانے_option;
	    buf->b_p_smc = p_smc;
	    COPY_OPT_SCTX(buf, BV_SMC);
	    buf->b_s.b_syn_isk = خالی_خانے_option;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SPELL
	    buf->b_s.b_p_spc = vim_strsave(p_spc);
	    COPY_OPT_SCTX(buf, BV_SPC);
	    (خالی)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#اگر_ختم
#اگر defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#اگر_ختم
	    buf->b_p_fp = خالی_خانے_option;
#اگر defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
	    buf->b_p_کلید = vim_strsave(p_کلید);
	    COPY_OPT_SCTX(buf, BV_کلید);
#اگر_ختم
	    buf->b_p_sua = vim_strsave(p_sua);
	    COPY_OPT_SCTX(buf, BV_SUA);
#اگر_تعریف_شدہ FEAT_KEYMAP
	    buf->b_p_keymap = vim_strsave(p_keymap);
	    COPY_OPT_SCTX(buf, BV_KMAP);
	    buf->b_kmap_state |= KEYMAP_INIT;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
	    buf->b_p_twsl = p_twsl;
	    COPY_OPT_SCTX(buf, BV_TWSL);
#اگر_ختم
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
#اگر_تعریف_شدہ HAVE_FSYNC
	    buf->b_p_fs = -1;
#اگر_ختم
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    buf->b_p_bkc = خالی_خانے_option;
	    buf->b_bkc_flags = 0;
#اگر_تعریف_شدہ FEAT_QUICKFIX
	    buf->b_p_gefm = خالی_خانے_option;
	    buf->b_p_gp = خالی_خانے_option;
	    buf->b_p_mp = خالی_خانے_option;
	    buf->b_p_efm = خالی_خانے_option;
#اگر_ختم
	    buf->b_p_ep = خالی_خانے_option;
#اگر defined(FEAT_EVAL)
	    buf->b_p_ffu = خالی_خانے_option;
#اگر_ختم
	    buf->b_p_kp = خالی_خانے_option;
	    buf->b_p_راستہ = خالی_خانے_option;
	    buf->b_p_لیبل_فہرست = خالی_خانے_option;
	    buf->b_p_tc = خالی_خانے_option;
	    buf->b_tc_flags = 0;
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
	    buf->b_p_def = خالی_خانے_option;
	    buf->b_p_inc = خالی_خانے_option;
# ifdef FEAT_EVAL
	    buf->b_p_inex = vim_strsave(p_inex);
	    COPY_OPT_SCTX(buf, BV_INEX);
	    COPY_OPT_INSECURE(buf->b_p_inex_flags, BV_INEX);
# endif
#اگر_ختم
	    buf->b_p_cot = خالی_خانے_option;
	    buf->b_cot_flags = 0;
	    buf->b_p_dict = خالی_خانے_option;
#اگر_تعریف_شدہ FEAT_DIFF
	    buf->b_p_dia = خالی_خانے_option;
#اگر_ختم
	    buf->b_p_tsr = خالی_خانے_option;
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
	    buf->b_p_tsrfu = خالی_خانے_option;
#اگر_ختم
	    buf->b_p_qe = vim_strsave(p_qe);
	    COPY_OPT_SCTX(buf, BV_QE);
#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = خالی_خانے_option;
#اگر_ختم
#اگر defined(FEAT_CRYPT)
	    buf->b_p_cm = خالی_خانے_option;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
	    buf->b_p_udf = p_udf;
	    COPY_OPT_SCTX(buf, BV_UDF);
#اگر_ختم
	    buf->b_p_lw = خالی_خانے_option;
	    buf->b_p_menc = خالی_خانے_option;

	    // Don't copy the options set by ex_help(), use the saved values,
	    // when going from a help buffer to a non-help buffer.
	    // Don't touch these at all when BCO_NOHELP is used and going from
	    // or to a help buffer.
	    اگر (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#اگر_تعریف_شدہ FEAT_VARTABS
		اگر (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (خالی)tabstop_سیٹ(p_vts, &buf->b_p_vts_array);
		ورنہ
		    buf->b_p_vts_array = تہی;
#اگر_ختم
	    }
	    ورنہ
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#اگر_تعریف_شدہ FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		اگر (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (خالی)tabstop_سیٹ(p_vts, &buf->b_p_vts_array);
		ورنہ
		    buf->b_p_vts_array = تہی;
#اگر_ختم
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
	(خالی)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    خالی
reset_modifiable(خالی)
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
    خالی
سیٹ_iminsert_global(خالی)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    خالی
سیٹ_imsearch_global(خالی)
{
    p_imsearch = curbuf->b_p_imsearch;
}

ساکن صحیح expand_option_idx = -1;
ساکن صحیح expand_option_آغاز_ستون = 0;
ساکن char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
ساکن صحیح expand_option_flags = 0;
ساکن صحیح expand_option_append = FALSE;

    خالی
سیٹ_context_اندر_سیٹ_cmd(
    expand_T	*xp,
    char_u	*arg,
    صحیح		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    صحیح		nextchar;
    long_u	flags = 0;	// init for GCC
    صحیح		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    صحیح		is_رکن_option = FALSE;
    صحیح		کلید;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    اگر (*arg == NUL)
    {
	xp->xp_نمونہ = arg;
	واپس;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    اگر (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_نمونہ = p + 1;
	واپس;
    }
    جب (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	اگر (*p == ' ' || *p == ',')
	{
	    جب (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	اگر (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    توڑو;
	}
	--p;
    }
    اگر (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_سابقہ = XP_سابقہ_NO;
	p += 2;
    }
    ورنہ اگر (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_سابقہ = XP_سابقہ_INV;
	p += 3;
    }
    xp->xp_نمونہ = arg = p;
    اگر (*arg == '<')
    {
	جب (*p != '>')
	    اگر (*p++ == NUL)	    // expand terminal option name
		واپس;
	کلید = get_special_کلید_رمز(arg + 1);
	اگر (کلید == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    واپس;
	}
	nextchar = *++p;
	is_رکن_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(کلید);
	expand_option_name[3] = KEY2TERMCAP1(کلید);
    }
    ورنہ
    {
	اگر (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    اگر (*p != NUL)
		++p;
	    اگر (*p == NUL)
		واپس;		// expand option name
	    nextchar = *++p;
	    is_رکن_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	ورنہ
	{
	    // Allow * wildcard
	    جب (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    اگر (*p == NUL)
		واپس;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    اگر (opt_idx == -1 || options[opt_idx].var == تہی)
	    {
		xp->xp_context = EXPAND_NOTHING;
		واپس;
	    }
	    flags = options[opt_idx].flags;
	    اگر (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		واپس;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    صحیح expand_option_تفریق = FALSE;
    اگر ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	اگر (nextchar == '-')
	    expand_option_تفریق = TRUE;
	اگر (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    اگر ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	واپس;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    اگر (is_رکن_option)
	expand_option_idx = -1;
    ورنہ
	expand_option_idx = opt_idx;

    اگر (!is_رکن_option)
    {
	اگر (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    واپس;
	}
    }

    xp->xp_نمونہ = p + 1;
    expand_option_آغاز_ستون = (صحیح)(p + 1 - xp->xp_سطر);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#اگر_تعریف_شدہ FEAT_SYN_HL
    اگر (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	واپس;
    }
#اگر_ختم
    اگر (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	واپس;
    }
#اگر_تعریف_شدہ FEAT_KEYMAP
    اگر (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	واپس;
    }
#اگر_ختم

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    اگر (expand_option_تفریق)
    {
	xp->xp_context = EXPAND_SETTING_تفریق;
	واپس;
    }
    ورنہ اگر (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != تہی)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    ورنہ اگر (*xp->xp_نمونہ == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	واپس;
    }
    ورنہ
	xp->xp_context = EXPAND_NOTHING;

    اگر (is_رکن_option || (flags & P_عدد))
	واپس;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    اگر (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	اگر (p == (char_u *)&p_bdir
		|| p == (char_u *)&p_dir
		|| p == (char_u *)&p_راستہ
		|| p == (char_u *)&p_pp
		|| p == (char_u *)&p_rtp
		|| p == (char_u *)&p_cdpath
#اگر_تعریف_شدہ FEAT_SESSION
		|| p == (char_u *)&p_vdir
#اگر_ختم
		)
	{
	    xp->xp_context = EXPAND_DIRECTORIES;
	    اگر (p == (char_u *)&p_راستہ || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    ورنہ
		xp->xp_backslash = XP_BS_ONE;
	}
	ورنہ
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    اگر (p == (char_u *)&p_لیبل_فہرست)
		xp->xp_backslash = XP_BS_THREE;
	    ورنہ
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
	برائے (p = argend - 1; p >= xp->xp_نمونہ; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    اگر (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		جب (s > xp->xp_نمونہ && *(s - 1) == '\\')
		    --s;
		اگر ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#اگر defined(BACKSLASH_اندر_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#ورنہ
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#اگر_ختم
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_نمونہ = p + 1;
		    توڑو;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    اگر (flags & P_FLAGLIST)
	xp->xp_نمونہ = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#اگر_تعریف_شدہ FEAT_SPELL
    اگر (options[opt_idx].var == (char_u *)&p_sps)
    {
	اگر (STRNCMP(xp->xp_نمونہ, "file:", 5) == 0)
	{
	    xp->xp_نمونہ += 5;
	    واپس;
	}
	ورنہ اگر (options[expand_option_idx].opt_expand_cb != تہی)
	{
	    xp->xp_context = EXPAND_STRING_SETTING;
	}
    }
#اگر_ختم
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
    ساکن صحیح
مطابقت_str(
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
	    واپس TRUE;
	}
    }
    ورنہ
    {
	صحیح score;

	score = fuzzy_مطابقت_str(str, fuzzystr);
	اگر (score != FUZZY_SCORE_NONE)
	{
	    اگر (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    واپس TRUE;
	}
    }

    واپس FALSE;
}

    صحیح
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    صحیح		*عدد_Matches,
    char_u	***matches,
    صحیح		can_fuzzy)
{
    صحیح		عدد_معمول = 0;	    // Nr of matching non-term-code settings
    صحیح		عدد_رکن = 0;	    // Nr of matching terminal code settings
    صحیح		opt_idx;
    صحیح		مطابقت;
    صحیح		گنتی = 0;
    char_u	*str;
    صحیح		loop;
    صحیح		is_رکن_opt;
    char_u	name_buf[MAX_کلید_NAME_LEN];
    ساکن حرف *(نام[]) = {"all", "termcap"};
    صحیح		ic = regmatch->rm_ic;	// remember the ignore-case flag
    صحیح		fuzzy;
    fuzmatch_str_T  *fuzmatch = تہی;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    برائے (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	اگر (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    برائے (مطابقت = 0; مطابقت < (صحیح)ARRAY_لمبائی(نام); ++مطابقت)
	    {
		اگر (مطابقت_str((char_u *)نام[مطابقت], regmatch, *matches,
			    گنتی, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    اگر (loop == 0)
			عدد_معمول++;
		    ورنہ
			گنتی++;
		}
	    }
	}
	برائے (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != تہی;
								    opt_idx++)
	{
	    اگر (options[opt_idx].var == تہی)
		جاری;
	    اگر (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		جاری;
	    is_رکن_opt = istermoption_idx(opt_idx);
	    اگر (is_رکن_opt && عدد_معمول > 0)
		جاری;

	    اگر (مطابقت_str(str, regmatch, *matches, گنتی, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		اگر (loop == 0)
		{
		    اگر (is_رکن_opt)
			عدد_رکن++;
		    ورنہ
			عدد_معمول++;
		}
		ورنہ
		    گنتی++;
	    }
	    ورنہ اگر (!fuzzy && options[opt_idx].shortname != تہی
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		اگر (loop == 0)
		{
		    اگر (is_رکن_opt)
			عدد_رکن++;
		    ورنہ
			عدد_معمول++;
		}
		ورنہ
		    (*matches)[گنتی++] = vim_strsave(str);
	    }
	    ورنہ اگر (is_رکن_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		اگر (مطابقت_str(name_buf, regmatch, *matches, گنتی, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    اگر (loop == 0)
			عدد_رکن++;
		    ورنہ
			گنتی++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	اگر (xp->xp_context != EXPAND_BOOL_SETTINGS  && عدد_معمول == 0)
	{
	    برائے (opt_idx = 0; (str = get_termcode(opt_idx)) != تہی; opt_idx++)
	    {
		اگر (!محفوظ_isprint(str[0]) || !محفوظ_isprint(str[1]))
		    جاری;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		اگر (مطابقت_str(name_buf, regmatch, *matches, گنتی,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    اگر (loop == 0)
			عدد_رکن++;
		    ورنہ
			گنتی++;
		}
		ورنہ
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    اگر (مطابقت_str(name_buf, regmatch, *matches, گنتی,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			اگر (loop == 0)
			    عدد_رکن++;
			ورنہ
			    گنتی++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    برائے (opt_idx = 0; (str = get_کلید_name(opt_idx)) != تہی; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		اگر (مطابقت_str(name_buf, regmatch, *matches, گنتی, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    اگر (loop == 0)
			عدد_رکن++;
		    ورنہ
			گنتی++;
		}
	    }
	}
	اگر (loop == 0)
	{
	    اگر (عدد_معمول > 0)
		*عدد_Matches = عدد_معمول;
	    ورنہ اگر (عدد_رکن > 0)
		*عدد_Matches = عدد_رکن;
	    ورنہ
		واپس OK;
	    اگر (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *عدد_Matches);
		اگر (*matches == تہی)
		{
		    *matches = (char_u **)"";
		    واپس FAIL;
		}
	    }
	    ورنہ
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *عدد_Matches);
		اگر (fuzmatch == تہی)
		{
		    *matches = (char_u **)"";
		    واپس FAIL;
		}
	    }
	}
    }

    اگر (fuzzy &&
	fuzzymatches_مقصد_strmatches(fuzmatch, matches, گنتی, FALSE) == FAIL)
	واپس FAIL;

    واپس OK;
}

// Escape an option value that can be used on the command-line with :set.
// Caller needs to free the returned string, unless NULL is returned.
    ساکن char_u*
escape_option_str_cmdline(char_u *var)
{
    char_u *buf;

    // A backslash is required before some characters.  This is the reverse of
    // what happens in do_set().
    buf = vim_strsave_escaped(var, escape_chars);
    اگر (buf == تہی)
	واپس تہی;

#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    برائے (var = buf; *var != NUL; MB_PTR_ADV(var))
	اگر (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#اگر_ختم
    واپس buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    صحیح
ExpandOldSetting(صحیح *عدد_Matches, char_u ***matches)
{
    char_u  *var = تہی;	// init for GCC
    char_u  *buf;

    *عدد_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    اگر (*matches == تہی)
	واپس FAIL;

    // For a terminal key code expand_option_idx is < 0.
    اگر (expand_option_idx < 0)
    {
	var = ڈھونڈو_termcode(expand_option_name + 2);
	اگر (var == تہی)
	    expand_option_idx = findoption(expand_option_name);
    }

    اگر (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    ورنہ اگر (var == تہی)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    اگر (buf == تہی)
    {
	VIM_CLEAR(*matches);
	واپس FAIL;
    }

    (*matches)[0] = buf;
    *عدد_Matches = 1;
    واپس OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    صحیح
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    صحیح		*عدد_Matches,
    char_u	***matches)
{
    char_u  *var = تہی;	// init for GCC
    char_u  *buf;

    اگر (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == تہی)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	واپس FAIL;
    }

    optexpand_T دلائل;
    دلائل.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    دلائل.oe_append = expand_option_append;
    دلائل.oe_regmatch = regmatch;
    دلائل.oe_xp = xp;
    دلائل.oe_سیٹ_arg = xp->xp_سطر + expand_option_آغاز_ستون;
    دلائل.oe_include_orig_val =
	!expand_option_append &&
	(*دلائل.oe_سیٹ_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    اگر (buf == تہی)
	واپس FAIL;

    دلائل.oe_opt_قدر = buf;

    صحیح عدد_ret = options[expand_option_idx].opt_expand_cb(&دلائل, عدد_Matches, matches);

    vim_free(buf);
    واپس عدد_ret;
}

/*
 * Expansion handler for :set-=
 */
    صحیح
Expand_Setting_تفریق(
    expand_T	*xp,
    regmatch_T	*regmatch,
    صحیح		*عدد_Matches,
    char_u	***matches)
{
    اگر (expand_option_idx < 0)
	// term option
	واپس ExpandOldSetting(عدد_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    اگر (option_flags & P_عدد)
	واپس ExpandOldSetting(عدد_Matches, matches);
    ورنہ اگر (option_flags & P_COMMA)
    {
	// Split the option by comma, then present each option to the user if
	// it matches the pattern.
	// This condition needs to go first, because 'whichwrap' has both
	// P_COMMA and P_FLAGLIST.
	garray_T    ga;

	char_u	    *شے;
	char_u	    *option_copy;
	char_u	    *اگلا_val;
	char_u	    *comma;

	اگر (*option_val == NUL)
	    واپس FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	اگر (option_copy == تہی)
	    واپس FAIL;
	اگلا_val = option_copy;

	ga_init2(&ga, حجم(char_u *), 10);

	کرو
	{
	    شے = اگلا_val;
	    comma = vim_strchr(اگلا_val, ',');
	    جب (comma != تہی && comma != اگلا_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    اگر (comma != تہی)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		اگلا_val = comma + 1;
	    }
	    ورنہ
		اگلا_val = تہی;

	    اگر (*شے == NUL)
		// empty value, don't add to list
		جاری;

	    اگر (!vim_regexec(regmatch, شے, (colnr_T)0))
		جاری;

	    char_u *buf = escape_option_str_cmdline(شے);
	    اگر (buf == تہی)
	    {
		vim_free(option_copy);
		ga_clear_لڑیاں(&ga);
		واپس FAIL;
	    }
	    اگر (ga_جوڑو_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		توڑو;
	    }
	} جب (اگلا_val != تہی);

	vim_free(option_copy);

	*matches = ga.ga_ڈیٹا;
	*عدد_Matches = ga.ga_len;
	واپس OK;
    }
    ورنہ اگر (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	اگر (*xp->xp_نمونہ != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    واپس FAIL;
	}

	size_t عدد_flags = STRLEN(option_val);
	اگر (عدد_flags == 0)
	    واپس FAIL;

	*matches = ALLOC_MULT(char_u *, عدد_flags + 1);
	اگر (*matches == تہی)
	    واپس FAIL;

	صحیح گنتی = 0;
	char_u *p;

	p = vim_strnsave(option_val, عدد_flags);
	اگر (p == تہی)
	{
	    VIM_CLEAR(*matches);
	    واپس FAIL;
	}
	(*matches)[گنتی++] = p;

	اگر (عدد_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    برائے (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		اگر (p == تہی)
		    توڑو;
		(*matches)[گنتی++] = p;
	    }
	}

	*عدد_Matches = گنتی;
	واپس OK;
    }

    واپس ExpandOldSetting(عدد_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    ساکن خالی
option_value2string(
    ساخت vimoption	*opp,
    صحیح			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    اگر (opp->flags & P_عدد)
    {
	طویل wc = 0;

	اگر (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_کلید_name((صحیح)wc, 0));
	ورنہ اگر (wc != 0)
	    STRCPY(NameBuff, transchar((صحیح)wc));
	ورنہ
	    sprintf((حرف *)NameBuff, "%ld", *(طویل *)varp);
    }
    ورنہ    // P_STRING
    {
	varp = *(char_u **)(varp);
	اگر (varp == تہی)		    // just in case
	    NameBuff[0] = NUL;
#اگر_تعریف_شدہ FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	ورنہ اگر (opp->var == (char_u *)&p_کلید && *varp)
	    STRCPY(NameBuff, "*****");
#اگر_ختم
	ورنہ اگر (opp->flags & P_EXPAND)
	    home_replace(تہی, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	ورنہ اگر ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	ورنہ
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    ساکن صحیح
wc_use_keyname(char_u *varp, طویل *wcp)
{
    اگر (((طویل *)varp == &p_wc) || ((طویل *)varp == &p_wcm))
    {
	*wcp = *(طویل *)varp;
	اگر (IS_SPECIAL(*wcp) || ڈھونڈو_special_کلید_اندر_جدول((صحیح)*wcp) >= 0)
	    واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    صحیح
shortmess(صحیح x)
{
    واپس p_shm != تہی &&
	    (   vim_strchr(p_shm, x) != تہی
	    || (vim_strchr(p_shm, 'a') != تہی
		&& vim_strchr((char_u *)SHM_A, x) != تہی));
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 * When "fname" is not NULL, use it to set $"envname" when it wasn't set yet.
 */
    خالی
vimrc_ملا(char_u *fname, char_u *envname)
{
    صحیح		opt_idx;
    صحیح		dofree = FALSE;
    char_u	*p;

    اگر (!option_was_سیٹ((char_u *)"cp"))
    {
	p_cp = FALSE;
	برائے (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    اگر (!(options[opt_idx].flags & (P_WAS_سیٹ|P_VI_DEF)))
		سیٹ_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    اگر (fname != تہی)
    {
	p = vim_getenv(envname, &dofree);
	اگر (p == تہی)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = بھرا_Name_save(fname, FALSE);
	    اگر (p != تہی)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	ورنہ اگر (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    خالی
change_compatible(صحیح on)
{
    صحیح	    opt_idx;

    اگر (p_cp != on)
    {
	p_cp = on;
	compatible_سیٹ();
    }
    opt_idx = findoption((char_u *)"cp");
    اگر (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_سیٹ;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    صحیح
option_was_سیٹ(char_u *name)
{
    صحیح idx;

    idx = findoption(name);
    اگر (idx < 0)	// unknown option
	واپس FALSE;
    اگر (options[idx].flags & P_WAS_سیٹ)
	واپس TRUE;
    واپس FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    صحیح
reset_option_was_سیٹ(char_u *name)
{
    صحیح idx = findoption(name);

    اگر (idx < 0)
	واپس FAIL;

    options[idx].flags &= ~P_WAS_سیٹ;
    واپس OK;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    ساکن خالی
compatible_سیٹ(خالی)
{
    صحیح	    opt_idx;

    برائے (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	اگر (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    سیٹ_option_default(opt_idx, OPT_FREE, p_cp);
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
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    اگر (what == BS_آغاز && bt_prompt(curbuf))
	واپس FALSE;
#اگر_ختم
    انتخاب (*p_bs)
    {
	حالت '3':       واپس TRUE;
	حالت '2':	واپس (what != BS_NOSTOP);
	حالت '1':	واپس (what != BS_آغاز);
	حالت '0':	واپس FALSE;
    }
    واپس vim_strchr(p_bs, what) != تہی;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    طویل
get_scrolloff_قدر(خالی)
{
    واپس curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    طویل
get_scrolloffpad_قدر(خالی)
{
    واپس curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    طویل
get_sidescrolloff_قدر(خالی)
{
    واپس curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    غیرعلامتی صحیح
get_bkc_flags(buf_T *buf)
{
    واپس buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#اگر defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_قدر(buf_T *buf)
{
    اگر (buf->b_p_flp == تہی || *buf->b_p_flp == NUL)
	واپس p_flp;
    واپس buf->b_p_flp;
}
#اگر_ختم

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    غیرعلامتی صحیح
get_ve_flags(خالی)
{
    واپس (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#اگر defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_قدر(win_T *win)
{
    اگر (win->w_p_sbr == تہی || *win->w_p_sbr == NUL)
	واپس p_sbr;
    اگر (STRCMP(win->w_p_sbr, "NONE") == 0)
	واپس خالی_خانے_option;
    واپس win->w_p_sbr;
}
#اگر_ختم

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
    اگر (d == تہی)
	واپس تہی;

    برائے (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	ساخت vimoption *opt = &options[opt_idx];

	اگر ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    اگر (varp != تہی)
	    {
		اگر (opt->flags & P_STRING)
		    dict_جوڑو_string(d, opt->fullname, *(char_u **)varp);
		ورنہ اگر (opt->flags & P_عدد)
		    dict_جوڑو_نمبر(d, opt->fullname, *(طویل *)varp);
		ورنہ
		    dict_جوڑو_نمبر(d, opt->fullname, *(صحیح *)varp);
	    }
	}
    }

    واپس d;
}
#اگر_ختم

#اگر defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    صحیح
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    اگر (val == تہی)
	p = wp->w_p_culopt;
    ورنہ
	p = val;
    جب (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	اگر (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_سطر;
	}
	ورنہ اگر (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_سطر | CULOPT_NBR;
	}
	ورنہ اگر (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	ورنہ اگر (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	اگر (*p != ',' && *p != NUL)
	    واپس FAIL;
	اگر (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    اگر ((culopt_flags_new & CULOPT_سطر) && (culopt_flags_new & CULOPT_SCRLINE))
	واپس FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    واپس OK;
}
#اگر_ختم

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    صحیح
magic_isset(خالی)
{
    انتخاب (magic_overruled)
    {
	حالت OPTION_MAGIC_ON:      واپس TRUE;
	حالت OPTION_MAGIC_OFF:     واپس FALSE;
	حالت OPTION_MAGIC_NOT_سیٹ: توڑو;
    }
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (اندر_vim9script())
	واپس TRUE;
#اگر_ختم
    واپس p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    صحیح
option_سیٹ_callback_func(char_u *optval غیر_مستعمل, callback_T *optcb غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    صحیح		funcname = FALSE;

    اگر (optval == تہی || *optval == NUL)
    {
	free_callback(optcb);
	واپس OK;
    }

    اگر (*optval == '{' || (اندر_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_اظہاریہ(optval, تہی);
    ورنہ
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    اگر (tv == تہی)
	واپس FAIL;

    cb = get_callback(tv);
    اگر (cb.cb_name == تہی || *cb.cb_name == NUL)
    {
	free_tv(tv);
	واپس FAIL;
    }

    free_callback(optcb);
    سیٹ_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = تہی;
    اگر (اندر_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != تہی))
    {
	اگر (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = ڈھونڈو_imported(optval, dot - optval, FALSE);
	اگر (import != تہی && SCRIPT_ID_VALID(import->imp_sid)
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
	ورنہ
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

    واپس OK;
#ورنہ
    واپس FAIL;
#اگر_ختم
}

#اگر defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    حرف *
did_سیٹ_showtabpanel(optset_T *دلائل غیر_مستعمل)
{
    shell_new_columns();
    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
    ساکن خالی
didset_options_sctx(صحیح opt_flags, حرف **buf)
{
	برائے (صحیح i = 0; ; ++i)
	{
	    اگر (buf[i] == تہی)
		توڑو;

	    صحیح idx = findoption((char_u *)buf[i]);
	    اگر (idx >= 0)
		سیٹ_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#اگر_ختم
