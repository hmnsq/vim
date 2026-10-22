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
#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "optiondefs.h"

நிலையான வெறுமை set_options_default(முழு opt_flags);
நிலையான வெறுமை set_string_default_esc(எழுத்து *name, char_u *val, முழு escape);
நிலையான char_u *find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
நிலையான char_u *option_expand(முழு opt_idx, char_u *val);
நிலையான வெறுமை didset_options(வெறுமை);
நிலையான வெறுமை didset_options2(வெறுமை);
#எனில் defined(FEAT_EVAL)
நிலையான long_u *insecure_flag(win_T *wp, முழு opt_idx, முழு opt_flags);
#இல்லையெனில்
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#endif
நிலையான எழுத்து *set_bool_option(முழு opt_idx, char_u *varp, முழு மதிப்பு, முழு opt_flags);
நிலையான எழுத்து *set_இலக்கம்_option(முழு opt_idx, char_u *varp, long மதிப்பு, எழுத்து *errbuf, size_t errbuflen, முழு opt_flags);
நிலையான முழு find_சாவி_option(char_u *arg_arg, முழு has_lt);
நிலையான வெறுமை showoptions(முழு all, முழு opt_flags);
நிலையான முழு optval_default(கட்டமைப்பு vimoption *, char_u *varp, முழு compatible);
நிலையான வெறுமை showoneopt(கட்டமைப்பு vimoption *, முழு opt_flags);
நிலையான முழு put_setstring(FILE *fd, bool legacy, எழுத்து *cmd, எழுத்து *name, char_u **valuep, long_u flags);
நிலையான முழு put_setnum(FILE *fd, எழுத்து *cmd, எழுத்து *name, long *valuep);
நிலையான முழு put_setbool(FILE *fd, எழுத்து *cmd, எழுத்து *name, முழு மதிப்பு);
நிலையான முழு istermoption(கட்டமைப்பு vimoption *p);
நிலையான முழு istermoption_idx(முழு opt_idx);
நிலையான char_u *get_varp_scope(கட்டமைப்பு vimoption *p, முழு scope);
நிலையான char_u *get_varp(கட்டமைப்பு vimoption *);
நிலையான வெறுமை check_win_options(win_T *win);
நிலையான வெறுமை option_value2string(கட்டமைப்பு vimoption *, முழு scope);
நிலையான வெறுமை check_winopt(winopt_T *wop);
நிலையான முழு wc_use_keyname(char_u *varp, long *wcp);
நிலையான வெறுமை compatible_set(வெறுமை);

#எனில் defined(FEAT_EVAL)
நிலையான எழுத்து *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", வெற்று};
நிலையான எழுத்து *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    வெற்று};
நிலையான வெறுமை didset_options_sctx(முழு opt_flags, எழுத்து **buf);
#endif


/*
 * Initialize the 'shell' option to a default value.
 */
    நிலையான வெறுமை
set_init_default_shell(வெறுமை)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    எனில் (((p = mch_getenv((char_u *)"SHELL")) != வெற்று && *p != NUL)
#எனில் defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != வெற்று && *p != NUL)
	    || ((p = (char_u *)default_shell()) != வெற்று && *p != NUL)
#endif
	    )
#எனில் defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	எனில் (vim_strchr(p, ' ') != வெற்று)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    எனில் (cmd != வெற்று)
	    {
		vim_snprintf((எழுத்து *)cmd, len, "\"%s\"", p);
		set_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	இல்லையெனில்
	    set_string_default("sh", p);
    }
#இல்லையெனில்
	set_string_default_esc("sh", p, TRUE);
#endif
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    நிலையான வெறுமை
set_init_default_backupskip(வெறுமை)
{
    முழு		opt_idx;
    முழு		i;
    char_u	*p;
    முழு		plen;
#ifdef UNIX
    நிலையான எழுத்து	*(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#இல்லையெனில்
    நிலையான எழுத்து	*(names[3]) = {"TMPDIR", "TEMP", "TMP"};
#endif
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(names); ++i)
    {
	முழு		mustfree = FALSE;
#ifdef UNIX
	எனில் (*names[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (முழு)STRLEN_LITERAL("/private/tmp");
# இல்லையெனில்
	    p = (char_u *)"/tmp";
	    plen = (முழு)STRLEN_LITERAL("/tmp");
# endif
	}
	இல்லையெனில்
#endif
	{
	    p = vim_getenv((char_u *)names[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	எனில் (p != வெற்று && *p != NUL)
	{
	    char_u  *item;
	    size_t  itemsize;
	    முழு	    has_trailing_path_sep = FALSE;

	    எனில் (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (முழு)STRLEN(p);
		// does the value include a trailing path separator?
		எனில் (after_pathsep(p, p + plen))
		    has_trailing_path_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_path_sep ? 0 : 1) + 2;
	    item = alloc(itemsize);
	    எனில் (item != வெற்று)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((எழுத்து *)item, itemsize, "%s%s*", p, (has_trailing_path_sep) ? "" : PATHSEPSTR);

		எனில் (find_dup_item(ga.ga_data, item, itemlen, options[opt_idx].flags) == வெற்று
			&& ga_grow(&ga, (முழு)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((எழுத்து *)ga.ga_data + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", item);
		}
		vim_free(item);
	    }
	}
	எனில் (mustfree)
	    vim_free(p);
    }
    எனில் (ga.ga_data != வெற்று)
    {
	set_string_default("bsk", ga.ga_data);
	vim_free(ga.ga_data);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    நிலையான வெறுமை
set_init_default_maxmemtot(வெறுமை)
{
    முழு		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    எனில் (opt_idx < 0)
	திரும்பு;

#எனில் !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
    எனில் (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#endif
    {
#எனில் defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#அல்லதெனில் defined(HAVE_TOTAL_MEM)
	// Use amount of memory available to Vim.
	n = (mch_total_mem(FALSE) >> 1);
#இல்லையெனில்
	n = (0x7fffffff >> 11);
#endif
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	எனில் (opt_idx >= 0)
	{
#எனில் !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
	    எனில் ((long)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (long)n
		    || (long)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#endif
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    நிலையான வெறுமை
set_init_default_cdpath(வெறுமை)
{
    முழு		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    முழு	i;
    முழு	j;
    முழு	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    எனில் (cdpath == வெற்று)
	திரும்பு;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    எனில் (buf != வெற்று)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	க்கு (i = 0; cdpath[i] != NUL; ++i)
	{
	    எனில் (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    இல்லையெனில்
	    {
		எனில் (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	எனில் (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	இல்லையெனில்
	    vim_free(buf); // cannot happen
    }
    எனில் (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    நிலையான வெறுமை
set_init_default_printencoding(வெறுமை)
{
#எனில் defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    set_string_default("penc",
# எனில் defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# இல்லையெனில் // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#endif
}

#எனில் defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    நிலையான வெறுமை
set_init_default_printexpr(வெறுமை)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    set_string_default("pexpr",
# எனில் defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# இல்லையெனில்
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#endif

#ifdef UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    நிலையான வெறுமை
set_init_restricted_mode(வெறுமை)
{
    char_u	*p;

    p = get_isolated_shell_name();
    எனில் (p == வெற்று)
	திரும்பு;
    எனில் (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#endif

#ifdef CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    நிலையான வெறுமை
set_init_clean_rtp(வெறுமை)
{
    முழு		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    எனில் (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    எனில் (opt_idx < 0)
	திரும்பு;
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
    நிலையான வெறுமை
set_init_xdg_rtp(வெறுமை)
{
    முழு		opt_idx;
    முழு		has_xdg_env = TRUE;
    முழு		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = வெற்று;
    char_u	*vimrc2 = வெற்று;
    char_u	*xdg_dir = வெற்று;
    char_u	*xdg_rtp = வெற்று;
    string_T	vimrc_xdg = {வெற்று, 0};

    // initialize chartab, so we can expand $HOME
    (வெறுமை)init_chartab();
    vimrc1 = expand_env_save((char_u *)USR_VIMRC_FILE);
    vimrc2 = expand_env_save((char_u *)USR_VIMRC_FILE2);

    xdg_dir = mch_getenv("XDG_CONFIG_HOME");
    எனில் (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    எனில் (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	செல் theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    எனில் ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	செல் theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    எனில் ((opt_idx = findoption((char_u *)"packpath")) < 0)
	செல் theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# எனில் defined(XDG_VDIR) && defined(FEAT_SESSION)
    எனில் ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	செல் theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    எனில் (should_free_xdg_dir)
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
    நிலையான வெறுமை
set_init_expand_env(வெறுமை)
{
    முழு		opt_idx;
    char_u	*p;

    க்கு (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	எனில் ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != வெற்று)
	    p = (char_u *)_(*(எழுத்து **)options[opt_idx].var);
	இல்லையெனில்
	    p = option_expand(opt_idx, வெற்று);
	எனில் (p != வெற்று && (p = vim_strsave(p)) != வெற்று)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    எனில் (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#எனில் defined(MSWIN) && defined(FEAT_GETTEXT)
/*
 * Get the display language of Windows and the languages to fall back on, as a
 * colon separated list for gettext, e.g. "ja_JP:en_US".  The list stops after
 * English, untranslated messages are English already.
 * Returns NULL when it cannot be obtained.  The result must be freed.
 */
    நிலையான char_u *
get_ui_langs(வெறுமை)
{
    ULONG	இலக்கம்_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*buffer;
    char_u	*langs = வெற்று;

    எனில் (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &இலக்கம்_languages, வெற்று,
								    &bufsize)
	    || bufsize == 0)
	திரும்பு வெற்று;

    buffer = ALLOC_MULT(WCHAR, bufsize);
    எனில் (buffer == வெற்று)
	திரும்பு வெற்று;

    எனில் (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &இலக்கம்_languages, buffer,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	எனில் (langs != வெற்று)
	{
	    char_u	*d = langs;
	    WCHAR	*s = buffer;

	    போது (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		எனில் (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		க்கு ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		எனில் (english)
		    நிறுத்து;
	    }
	    *d = NUL;
	}
    }
    vim_free(buffer);

    திரும்பு langs;
}
#endif

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    நிலையான வெறுமை
set_init_lang_env(வெறுமை)
{
#எனில் defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    எனில் (mch_getenv((char_u *)"LANG") == வெற்று
	    && mch_getenv((char_u *)"LANGUAGE") == வெற்று
	    && mch_getenv((char_u *)"LC_ALL") == வெற்று
	    && mch_getenv((char_u *)"LC_MESSAGES") == வெற்று)
    {
	char_u	*langs = get_ui_langs();

	எனில் (langs != வெற்று && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    எனில் (colon != வெற்று)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#அல்லதெனில் defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#endif
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    நிலையான வெறுமை
set_init_default_encoding(வெறுமை)
{
    char_u	*p;
    முழு		opt_idx;

#எனில் defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#இல்லையெனில்
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#endif
    எனில் (p == வெற்று)
	திரும்பு;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    எனில் (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    எனில் (mb_init() == வெற்று)
    {
	opt_idx = findoption((char_u *)"encoding");
	எனில் (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#எனில் defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	எனில் (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    set_string_option_direct((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    set_string_option_direct((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    எனில் (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    எனில் (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (வெறுமை)init_chartab();
	}
#endif

#எனில் defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	எனில் (
# ifdef VIMDLL
		(!gui.in_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    எழுத்து	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    எனில் (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, அளவு(buf), "cp%ld", (long)GetACP());
	    இல்லையெனில்
		buflen = vim_snprintf(buf, அளவு(buf), "cp%ld", (long)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    எனில் (p_tenc != வெற்று)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		எனில் (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_setup(&உள்ளீடு_conv, p_tenc, p_enc);
		convert_setup(&output_conv, p_enc, p_tenc);
	    }
	    இல்லையெனில்
		p_tenc = empty_option;
	}
#endif
#எனில் defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#endif
    }
    இல்லையெனில்
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
    வெறுமை
set_init_1(முழு clean_arg)
{
#ifdef FEAT_LANGMAP
    langmap_init();
#endif

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    எனில் (mch_getenv((char_u *)"VIM_POSIX") != வெற்று)
    {
	set_string_default("cpo", (char_u *)CPO_ALL);
	set_string_default("shm", (char_u *)SHM_POSIX);
    }

    set_init_default_shell();
    set_init_default_backupskip();
    set_init_default_maxmemtot();
    set_init_default_cdpath();
    set_init_default_printencoding();
#எனில் defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
    எனில் (clean_arg)
	set_init_clean_rtp();
#endif

#ifdef FEAT_GUI
    எனில் (found_reverse_arg)
	set_option_மதிப்பு_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
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

#எனில் defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    எனில் (mch_getenv((char_u *)"MLTERM") != வெற்று)
	set_option_மதிப்பு_give_err((char_u *)"tbidi", 1L, வெற்று, 0);
#endif

    didset_options2();

    set_init_lang_env();

#ifdef FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    set_helplang_default(get_mess_lang());
#endif
}

நிலையான char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    வெறுமை
set_fencs_unicode(வெறுமை)
{
    set_string_option_direct((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    நிலையான வெறுமை
set_option_default(
    முழு		opt_idx,
    முழு		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    முழு		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    முழு		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    முழு		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    எனில் (varp != வெற்று)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	எனில் (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    எனில் (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		set_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    இல்லையெனில் எனில் (options[opt_idx].indir != PV_NONE)
		set_string_option_direct(வெற்று, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    இல்லையெனில்
	    {
		எனில் ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	இல்லையெனில் எனில் (flags & P_இலக்கம்)
	{
	    எனில் (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    இல்லையெனில்
	    {
		long def_val = (long)(long_i)options[opt_idx].def_val[dvi];

		எனில் ((long *)varp == &curwin->w_p_so
			|| (long *)varp == &curwin->w_p_siso
			|| (long *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(long *)varp = -1;
		இல்லையெனில்
		    *(long *)varp = def_val;
		// May also set global value for local option.
		எனில் (both)
		    *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	இல்லையெனில்	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(முழு *)varp = (முழு)(long)(long_i)options[opt_idx].def_val[dvi];
#ifdef UNIX
	    // 'modeline' defaults to off for root
	    எனில் (options[opt_idx].indir == PV_ML && getuid() == வேர்_UID)
		*(முழு *)varp = FALSE;
#endif
	    // May also set global value for local option.
	    எனில் (both)
		*(முழு *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(முழு *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	எனில் (both)
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
    நிலையான வெறுமை
set_options_default(
    முழு		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    முழு		i;
    win_T	*wp;
    tabpage_T	*tp;

    க்கு (i = 0; !istermoption_idx(i); i++)
	எனில் (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#எனில் defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_சாவி
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
    நிலையான வெறுமை
set_string_default_esc(எழுத்து *name, char_u *val, முழு escape)
{
    char_u	*p;
    முழு		opt_idx;

    எனில் (escape && vim_strchr(val, ' ') != வெற்று)
	p = vim_strsave_escaped(val, (char_u *)" ");
    இல்லையெனில்
	p = vim_strsave(val);
    எனில் (p == வெற்று)		// we don't want a NULL
	திரும்பு;

    opt_idx = findoption((char_u *)name);
    எனில் (opt_idx < 0)
    {
	vim_free(p);
	திரும்பு;
    }

    எனில் (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    வெறுமை
set_string_default(எழுத்து *name, char_u *val)
{
    set_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    நிலையான char_u *
find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags)
{
    முழு	    bs = 0;
    char_u  *s;

    எனில் (origval == வெற்று)
	திரும்பு வெற்று;

    க்கு (s = origval; *s != NUL; ++s)
    {
	எனில் ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    திரும்பு s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	எனில் ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	இல்லையெனில்
	    bs = 0;
    }
    திரும்பு வெற்று;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    வெறுமை
set_எண்_default(எழுத்து *name, long val)
{
    முழு		opt_idx;

    opt_idx = findoption((char_u *)name);
    எனில் (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#எனில் defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    வெறுமை
set_local_options_default(win_T *wp, முழு do_buffer)
{
    win_T	*save_curwin = curwin;
    முழு		i;

    curwin = wp;
    curbuf = curwin->w_buffer;
    block_autocmds();

    க்கு (i = 0; !istermoption_idx(i); i++)
    {
	கட்டமைப்பு vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	எனில் (p->indir != PV_NONE
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

#எனில் defined(EXITFREE)
/*
 * Free all options.
 */
    வெறுமை
free_all_options(வெறுமை)
{
    முழு		i;

    க்கு (i = 0; !istermoption_idx(i); i++)
    {
	எனில் (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    எனில் ((options[i].flags & P_ALLOCED) && options[i].var != வெற்று)
		free_string_option(*(char_u **)options[i].var);
	    எனில் (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	இல்லையெனில் எனில் (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# எனில் defined(FEAT_EVAL)
    free_findfunc_option();
# endif
}
#endif


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    வெறுமை
set_init_2(வெறுமை)
{
    முழு		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    எனில் (idx >= 0 && !(options[idx].flags & P_WAS_SET))
	set_option_default(idx, OPT_LOCAL, p_cp);
    comp_col();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    எனில் (!option_was_set((char_u *)"window"))
	p_window = Rows - 1;
    set_எண்_default("window", Rows - 1);

    // For DOS console the default is always black.
#எனில் !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    எனில் (idx >= 0 && !(options[idx].flags & P_WAS_SET)
						 && *term_bg_default() == 'd')
    {
	set_string_option_direct(வெற்று, idx, (char_u *)"dark", OPT_FREE, 0);
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
    (வெறுமை)parse_printoptions(வெற்று);	// parse 'printoptions' default value
#endif
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    வெறுமை
set_init_3(வெறுமை)
{
#எனில் defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    முழு	    idx_srr;
    முழு	    do_srr;
# ifdef FEAT_QUICKFIX
    முழு	    idx_sp;
    முழு	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    எனில் (idx_srr < 0)
	do_srr = FALSE;
    இல்லையெனில்
	do_srr = !(options[idx_srr].flags & P_WAS_SET);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    எனில் (idx_sp < 0)
	do_sp = FALSE;
    இல்லையெனில்
	do_sp = !(options[idx_sp].flags & P_WAS_SET);
# endif
    p = get_isolated_shell_name();
    எனில் (p != வெற்று)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	எனில் (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# எனில் defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# எனில் defined(FEAT_QUICKFIX)
	    எனில் (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  இல்லையெனில்
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    எனில் (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	இல்லையெனில் எனில் (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  எனில் defined(FEAT_QUICKFIX)
		எனில் (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		எனில் (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	இல்லையெனில்
	    // Always use POSIX shell style redirection if we reach this
	    எனில் (       fnamecmp(p, "sh") == 0
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
# எனில் defined(FEAT_QUICKFIX)
		எனில் (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  இல்லையெனில்
		    எனில் (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    இல்லையெனில்
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		எனில் (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#endif

#எனில் defined(MSWIN)
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
    எனில் (strstr((எழுத்து *)gettail(p_sh), "powershell") != வெற்று)
    {
	முழு	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	எனில் (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	எனில் (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    இல்லையெனில் எனில் (strstr((எழுத்து *)gettail(p_sh), "sh") != வெற்று)
    {
	முழு	idx3;

	idx3 = findoption((char_u *)"shcf");
	எனில் (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	எனில் (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    இல்லையெனில் எனில் (strstr((எழுத்து *)gettail(p_sh), "cmd.exe") != வெற்று)
    {
	முழு	idx3;

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
	எனில் (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	எனில் (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#endif

    எனில் (BUFEMPTY())
    {
	முழு idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	எனில் (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_SET))
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    set_title_defaults();
}

#எனில் defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    வெறுமை
set_helplang_default(char_u *lang)
{
    முழு		idx;
    size_t	langlen;

    எனில் (lang == வெற்று)	// safety check
	திரும்பு;

    langlen = STRLEN(lang);
    எனில் (langlen < 2)	// safety check
	திரும்பு;

    idx = findoption((char_u *)"hlg");
    எனில் (idx < 0 || (options[idx].flags & P_WAS_SET))
	திரும்பு;

    எனில் (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    எனில் (p_hlg == வெற்று)
	p_hlg = empty_option;
    இல்லையெனில்
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	எனில் (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	இல்லையெனில் எனில் (langlen >= 1 && *p_hlg == 'C')
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
    வெறுமை
set_title_defaults(வெறுமை)
{
    முழு	    idx1;
    long    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    எனில் (idx1 >= 0 && !(options[idx1].flags & P_WAS_SET))
    {
#ifdef FEAT_GUI
	எனில் (gui.starting || gui.in_use)
	    val = TRUE;
	இல்லையெனில்
#endif
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    எனில் (idx1 < 0 || (options[idx1].flags & P_WAS_SET))
    {
	திரும்பு;
    }

#ifdef FEAT_GUI
    எனில் (gui.starting || gui.in_use)
	val = TRUE;
    இல்லையெனில்
#endif
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    வெறுமை
ex_set(exarg_T *eap)
{
    முழு		flags = 0;

    எனில் (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    இல்லையெனில் எனில் (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#எனில் defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    எனில் ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    இல்லையெனில்
#endif
    {
	எனில் (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(வெறுமை)do_set(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
வகைவரை எண்ணிக்கை {
    PREFIX_NO = 0,	// "no" prefix
    PREFIX_NONE,	// no prefix
    PREFIX_INV,		// "inv" prefix
} set_prefix_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    நிலையான set_prefix_T
get_option_prefix(char_u **argp)
{
    முழு		prefix = PREFIX_NONE;
    char_u	*arg = *argp;

    எனில் (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	prefix = PREFIX_NO;
	arg += 2;
    }
    இல்லையெனில் எனில் (STRNCMP(arg, "inv", 3) == 0)
    {
	prefix = PREFIX_INV;
	arg += 3;
    }

    *argp = arg;
    திரும்பு prefix;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    நிலையான முழு
parse_option_name(char_u *arg, முழு *opt_idxp, முழு *lenp, முழு *keyp)
{
    முழு	சாவி = 0;
    முழு	len;
    முழு	opt_idx;

    எனில் (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	எனில் (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	இல்லையெனில்
	{
	    len = 1;
	    போது (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	எனில் (arg[len] != '>')
	    திரும்பு FAIL;

	arg[len] = NUL;			    // put NUL after name
	எனில் (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	எனில் (opt_idx == -1)
	    சாவி = find_சாவி_option(arg + 1, TRUE);
    }
    இல்லையெனில்
    {
	முழு	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	எனில் (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	இல்லையெனில்
	    போது (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	எனில் (opt_idx == -1)
	    சாவி = find_சாவி_option(arg, FALSE);
    }

    *keyp = சாவி;
    *lenp = len;
    *opt_idxp = opt_idx;

    திரும்பு OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    நிலையான set_op_T
get_opt_op(char_u *arg)
{
    set_op_T op = OP_NONE;

    எனில் (*arg != NUL && *(arg + 1) == '=')
    {
	எனில் (*arg == '+')
	    op = OP_ADDING;		// "+="
	இல்லையெனில் எனில் (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	இல்லையெனில் எனில் (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    திரும்பு op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
நிலையான எழுத்து *modeline_whitelist[] =
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
    வெற்று
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    நிலையான bool
is_modeline_whitelisted(எழுத்து *name)
{
    க்கு (முழு i = 0; modeline_whitelist[i] != வெற்று; i++)
	எனில் (STRCMP(name, modeline_whitelist[i]) == 0)
	    திரும்பு true;
    திரும்பு false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    நிலையான முழு
validate_opt_idx(
	முழு opt_idx,
	முழு opt_flags,
	long_u flags,
	எழுத்து **errmsg,
	set_prefix_T prefix)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    எனில் ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	திரும்பு FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    எனில் ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	திரும்பு FAIL;

    // Disallow changing some options from modelines.
    எனில் (opt_flags & OPT_MODELINE)
    {
	எனில் (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_in_modeline;
	    திரும்பு FAIL;
	}
	எனில் ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_in_modeline_when_modelineexpr_is_off;
	    திரும்பு FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	எனில் (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    எனில் (options[opt_idx].indir == PV_ML && prefix == PREFIX_NO)
		திரும்பு OK;
	    இல்லையெனில் எனில் (!is_modeline_whitelisted(options[opt_idx].fullname))
		திரும்பு FAIL;
	}
#ifdef FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	எனில் (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    திரும்பு FAIL;
#endif
    }

#ifdef HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    எனில் (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_in_sandbox;
	திரும்பு FAIL;
    }
#endif

    திரும்பு OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    நிலையான char_u *
stropt_get_default_val(
    முழு		opt_idx,
    char_u	*varp,
    முழு		flags,
    முழு		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    எனில் ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ifdef FEAT_GUI
	எனில் (gui.in_use)
	    newval = gui_bg_default();
	இல்லையெனில்
#endif
	    newval = term_bg_default();
    }
    இல்லையெனில் எனில் ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    எனில் (newval == வெற்று)
	newval = empty_option;
    இல்லையெனில்
    {
	char_u *s = option_expand(opt_idx, newval);
	எனில் (s == வெற்று)
	    s = newval;
	newval = vim_strsave(s);
    }

    திரும்பு newval;
}

/*
 * Convert the 'backspace' option number value to a string: for adding,
 * prepending and removing string.
 */
    நிலையான வெறுமை
opt_backspace_nr2str(
    char_u	*varp,
    char_u	**origval_p,
    char_u	**origval_l_p,
    char_u	**origval_g_p,
    char_u	**oldval_p)
{
    முழு		i = getdigits((char_u **)varp);

    தேர்வு (i)
    {
	நிலை 0:
	    *(char_u **)varp = empty_option;
	    நிறுத்து;
	நிலை 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    நிறுத்து;
	நிலை 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    நிறுத்து;
	நிலை 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    நிறுத்து;
    }
    vim_free(*oldval_p);
    எனில் (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    எனில் (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    எனில் (*origval_g_p == *oldval_p)
	*origval_g_p = *(char_u **)varp;
    *oldval_p = *(char_u **)varp;
}

/*
 * Convert the 'whichwrap' option number value to a string, for backwards
 * compatibility with Vim 3.0.
 * Note: 'argp' is a pointer to a char_u pointer and is updated.
 */
    நிலையான char_u *
opt_whichwrap_nr2str(char_u **argp, char_u *whichwrap)
{
    size_t  len = 0;

    *whichwrap = NUL;
    முழு i = getdigits(argp);
    எனில் (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    எனில் (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    எனில் (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    எனில் (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    எனில் (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    எனில் (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    திரும்பு whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    நிலையான char_u *
stropt_copy_மதிப்பு(
    char_u	*origval,
    char_u	**argp,
    set_op_T	op,
    முழு		flags UNUSED)
{
    char_u	*arg = *argp;
    unsigned	newlen;
    char_u	*newval;
    char_u	*s = வெற்று;

    // get a bit too much
    newlen = (unsigned)STRLEN(arg) + 1;
    எனில் (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;
    newval = alloc(newlen);
    எனில் (newval == வெற்று)  // out of mem, don't change
	திரும்பு வெற்று;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    போது (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	முழு i;

	எனில் (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#endif
	   )
	    ++arg;	// remove backslash
	எனில் (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	இல்லையெனில்
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    திரும்பு newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    நிலையான char_u *
stropt_expand_envvar(
    முழு		opt_idx,
    char_u	*origval,
    char_u	*newval,
    set_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    எனில் (s == வெற்று)
	திரும்பு newval;

    vim_free(newval);
    unsigned newlen = (unsigned)STRLEN(s) + 1;
    எனில் (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;

    newval = alloc(newlen);
    எனில் (newval == வெற்று)
	திரும்பு வெற்று;

    STRCPY(newval, s);

    திரும்பு newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    நிலையான வெறுமை
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    முழு		flags)
{
    முழு		len = 0;

    முழு comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    எனில் (op == OP_ADDING)
    {
	len = (முழு)STRLEN(origval);
	// strip a trailing comma, would get 2
	எனில் (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    இல்லையெனில்
    {
	len = (முழு)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    எனில் (comma)
	newval[len] = ',';
}

/*
 * Remove a value from a string option.  Copy string option value in "origval"
 * to "newval" and then remove the string "strval" of length "len".
 */
    நிலையான வெறுமை
stropt_remove_val(
    char_u	*origval,
    char_u	*newval,
    முழு		flags,
    char_u	*strval,
    முழு		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    எனில் (*strval)
    {
	// may need to remove a comma
	எனில் (flags & P_COMMA)
	{
	    எனில் (strval == origval)
	    {
		// include comma after string
		எனில் (strval[len] == ',')
		    ++len;
	    }
	    இல்லையெனில்
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
    நிலையான char_u *
find_சாவி_item(char_u *src, char_u *சாவி, முழு keylen, முழு *itemlenp)
{
    char_u  *p = src;

    போது (*p != NUL)
    {
	// Check if this item starts with the same key
	எனில் ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, சாவி, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *end = vim_strchr(p, ',');
	    எனில் (end == வெற்று)
		end = p + STRLEN(p);
	    *itemlenp = (முழு)(end - p);
	    திரும்பு p;
	}
	++p;
    }
    திரும்பு வெற்று;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    நிலையான வெறுமை
remove_comma_item(char_u *str, char_u *item, முழு itemlen)
{
    எனில் (item[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(item, item + itemlen + 1);
    இல்லையெனில் எனில் (item > str && *(item - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(item - 1, item + itemlen);
    இல்லையெனில்
	// Only item
	*item = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    நிலையான வெறுமை
remove_சாவி_item(char_u *str, char_u *சாவி, முழு keylen, char_u *skip)
{
    முழு	    itemlen;
    char_u  *found;

    போது ((found = find_சாவி_item(str, சாவி, keylen, &itemlen)) != வெற்று)
    {
	எனில் (found == skip)
	{
	    // Search for the next match after this one.
	    char_u *next = found + itemlen;
	    எனில் (*next == ',')
		++next;
	    found = find_சாவி_item(next, சாவி, keylen, &itemlen);
	    எனில் (found == வெற்று)
		நிறுத்து;
	}

	remove_comma_item(str, found, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    நிலையான வெறுமை
append_item(char_u *str, char_u *item, முழு item_len)
{
    முழு len = (முழு)STRLEN(str);

    எனில் (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, item, (size_t)item_len);
    str[len + item_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    நிலையான வெறுமை
prepend_item(char_u *str, char_u *item, முழு item_len)
{
    முழு len = (முழு)STRLEN(str);
    முழு comma = (len > 0) ? 1 : 0;

    mch_memmove(str + item_len + comma, str, (size_t)len + 1);
    mch_memmove(str, item, (size_t)item_len);
    எனில் (comma)
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
    நிலையான bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    முழு		flags UNUSED)
{
    char_u  *p;
    char_u  *item_start;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    எனில் (vim_strchr(newval, ':') == வெற்று && vim_strchr(newval, ',') == வெற்று)
	திரும்பு false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    எனில் (newval_copy == வெற்று)
	திரும்பு false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    item_start = newval_copy;
    க்கு (;;)
    {
	p = vim_strchr(item_start, ',');
	முழு item_len = (p == வெற்று)
			    ? (முழு)STRLEN(item_start) : (முழு)(p - item_start);

	எனில் (item_len > 0)
	{
	    char_u *colon = vim_strchr(item_start, ':');
	    எனில் (colon != வெற்று && colon < item_start + item_len)
	    {
		முழு keylen = (முழு)(colon - item_start) + 1;

		எனில் (op == OP_ADDING || op == OP_PREPENDING)
		{
		    முழு old_itemlen;
		    char_u *found = find_சாவி_item(newval, item_start,
						       keylen, &old_itemlen);
		    எனில் (found != வெற்று)
		    {
			எனில் (old_itemlen == item_len
				&& STRNCMP(found, item_start,
							 item_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_சாவி_item(newval, item_start,
							   keylen, found);
			}
			இல்லையெனில்
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_சாவி_item(newval, item_start,
							    keylen, வெற்று);
			    எனில் (op == OP_PREPENDING)
				prepend_item(newval, item_start, item_len);
			    இல்லையெனில்
				append_item(newval, item_start, item_len);
			}
		    }
		    இல்லையெனில்
		    {
			// New item.
			எனில் (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			இல்லையெனில்
			    append_item(newval, item_start, item_len);
		    }
		}
		இல்லையெனில் எனில் (op == OP_REMOVING)
		    remove_சாவி_item(newval, item_start, keylen, வெற்று);
	    }
	    இல்லையெனில்
	    {
		எனில் (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    எனில் (found == வெற்று)
		    {
			// New item.
			எனில் (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			இல்லையெனில்
			    append_item(newval, item_start, item_len);
		    }
		    // else: exact duplicate — do nothing
		}
		இல்லையெனில் எனில் (op == OP_REMOVING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    எனில் (found != வெற்று)
			remove_comma_item(newval, found, item_len);
		}
	    }
	}

	எனில் (p == வெற்று)
	    நிறுத்து;
	item_start = p + 1;
    }

    vim_free(newval_copy);

    திரும்பு true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    நிலையான வெறுமை
stropt_remove_dupflags(char_u *newval, முழு flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    போது (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	எனில் (flags & P_ONECOMMA)
	{
	    எனில் (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != வெற்று)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		தொடர்;
	    }
	}
	இல்லையெனில்
	{
	    எனில் ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != வெற்று)
	    {
		STRMOVE(s, s + 1);
		தொடர்;
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
    நிலையான char_u *
stropt_get_newval(
    முழு		nextchar,
    முழு		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    set_op_T    *op_arg,
    முழு		flags,
    முழு		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    set_op_T    op = *op_arg;
    char_u	*save_arg = வெற்று;
    char_u	*newval;
    char_u	*s = வெற்று;
    char_u	whichwrap[80];

    எனில் (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    இல்லையெனில் எனில் (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    இல்லையெனில்
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	எனில் (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	இல்லையெனில் எனில் (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	இல்லையெனில் எனில் (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	இல்லையெனில் எனில் (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_மதிப்பு(origval, &arg, op, flags);
	எனில் (newval == வெற்று)
	    செல் done;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	எனில் (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    எனில் (newval == வெற்று)
		செல் done;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	எனில் ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	இல்லையெனில்
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    முழு len = 0;
	    எனில் (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (முழு)STRLEN(newval);
		s = find_dup_item(origval, newval, len, flags);

		// do not add if already there
		எனில் ((op == OP_ADDING || op == OP_PREPENDING) && s != வெற்று)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		எனில் (s == வெற்று)
		    s = origval + (முழு)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    எனில் (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    இல்லையெனில் எனில் (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	எனில் (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

done:
    எனில் (save_arg != வெற்று)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    திரும்பு newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    நிலையான முழு
do_set_option_string(
	முழு	    opt_idx,
	முழு	    opt_flags,
	char_u	    **argp,
	முழு	    nextchar,
	set_op_T    op_arg,
	long_u	    flags,
	முழு	    cp_val,
	char_u	    *varp_arg,
	எழுத்து	    *errbuf,
	size_t	    errbuflen,
	முழு	    *மதிப்பு_checked,
	எழுத்து	    **errmsg)
{
    char_u	*arg = *argp;
    set_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = வெற்று; // previous value if *varp
    char_u	*newval;
    char_u	*origval = வெற்று;
    char_u	*origval_l = வெற்று;
    char_u	*origval_g = வெற்று;
#எனில் defined(FEAT_EVAL)
    char_u	*saved_origval = வெற்று;
    char_u	*saved_origval_l = வெற்று;
    char_u	*saved_origval_g = வெற்று;
    char_u	*saved_newval = வெற்று;
#endif

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    எனில் ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((முழு)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    எனில் ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	எனில் (((முழு)options[opt_idx].indir & PV_BOTH)
						  && origval_l == empty_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    எனில் (((முழு)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    இல்லையெனில்
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    எனில் (newval == வெற்று)
	*(char_u **)(varp) = empty_option;

#எனில் defined(FEAT_EVAL)
    எனில் (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_சாவி
# endif
		      && origval != வெற்று && newval != வெற்று)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	எனில் (origval_l != வெற்று)
	    saved_origval_l = vim_strsave(origval_l);
	எனில் (origval_g != வெற்று)
	    saved_origval_g = vim_strsave(origval_g);
    }
#endif

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	முழு	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	எனில் ((opt_flags & OPT_MODELINE)
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
			errbuflen, opt_flags, op, மதிப்பு_checked);

	secure = secure_saved;
    }

#எனில் defined(FEAT_EVAL)
    எனில் (*errmsg == வெற்று)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#endif

    *argp = arg;
    திரும்பு *errmsg == வெற்று ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    நிலையான எழுத்து *
do_set_option_bool(
    முழு		opt_idx,
    முழு		opt_flags,
    set_prefix_T prefix,
    long_u	flags,
    char_u	*varp,
    முழு		nextchar,
    முழு		afterchar,
    முழு		cp_val)

{
    varnumber_T	மதிப்பு;

    எனில் (nextchar == '=' || nextchar == ':')
	திரும்பு e_invalid_argument;
    எனில் (opt_idx < 0 || varp == வெற்று)
	திரும்பு வெற்று;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    எனில் (nextchar == '!')
	மதிப்பு = *(முழு *)(varp) ^ 1;
    இல்லையெனில் எனில் (nextchar == '&')
	மதிப்பு = (முழு)(long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    இல்லையெனில் எனில் (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	எனில் ((முழு *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    மதிப்பு = -1;
	இல்லையெனில் எனில் ((முழு *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    மதிப்பு = -1;
#ifdef HAVE_FSYNC
	இல்லையெனில் எனில் ((முழு *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    மதிப்பு = -1;
#endif
	இல்லையெனில்
	    மதிப்பு = *(முழு *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    இல்லையெனில்
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	எனில் (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    திரும்பு e_trailing_characters;
	எனில் (prefix == PREFIX_INV)
	    மதிப்பு = *(முழு *)(varp) ^ 1;
	இல்லையெனில்
	    மதிப்பு = prefix == PREFIX_NO ? 0 : 1;
    }

    திரும்பு set_bool_option(opt_idx, varp, (முழு)மதிப்பு, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    நிலையான எழுத்து *
do_set_option_numeric(
    முழு		opt_idx,
    முழு		opt_flags,
    char_u	**argp,
    முழு		nextchar,
    set_op_T	op,
    long_u	flags,
    முழு		cp_val,
    char_u	*varp,
    எழுத்து	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		மதிப்பு;
    முழு			i;
    எழுத்து		*errmsg = வெற்று;

    எனில் (opt_idx < 0 || varp == வெற்று)
	திரும்பு வெற்று;  // "cannot happen"
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
    எனில் (nextchar == '&')
	மதிப்பு = (long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    இல்லையெனில் எனில் (nextchar == '<')
    {
	எனில் ((long *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    மதிப்பு = NO_LOCAL_UNDOLEVEL;
	இல்லையெனில் எனில் (opt_flags == OPT_LOCAL
		    && ((long *)varp == &curwin->w_p_siso
		     || (long *)varp == &curwin->w_p_so
		     || (long *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    மதிப்பு = -1;
	இல்லையெனில்
	    மதிப்பு = *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    இல்லையெனில் எனில் (((long *)varp == &p_wc || (long *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	மதிப்பு = string_to_சாவி(arg, FALSE);
	எனில் (மதிப்பு == 0)
	{
	    errmsg = e_invalid_argument;
	    செல் skip;
	}
    }
    இல்லையெனில் எனில் (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, வெற்று, &i, STR2NR_ALL, &மதிப்பு, வெற்று, 0, TRUE, வெற்று);
	எனில் (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_எண்_required_after_equal;
	    செல் skip;
	}
    }
    இல்லையெனில்
    {
	errmsg = e_எண்_required_after_equal;
	செல் skip;
    }

    எனில் (op == OP_ADDING)
	மதிப்பு = *(long *)varp + மதிப்பு;
    இல்லையெனில் எனில் (op == OP_PREPENDING)
	மதிப்பு = *(long *)varp * மதிப்பு;
    இல்லையெனில் எனில் (op == OP_REMOVING)
	மதிப்பு = *(long *)varp - மதிப்பு;

    எனில் ((long *)varp == &curwin->w_p_sop && மதிப்பு < -1)
    {
	errmsg = e_invalid_argument;
	செல் skip;
    }

    errmsg = set_இலக்கம்_option(opt_idx, varp, மதிப்பு, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    திரும்பு errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    நிலையான எழுத்து *
do_set_option_keycode(char_u **argp, char_u *சாவி_name, முழு nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    எனில் (nextchar == '&')
    {
	எனில் (add_termcap_entry(சாவி_name, TRUE) == FAIL)
	    திரும்பு e_not_found_in_termcap;
    }
    இல்லையெனில்
    {
	++arg; // jump to after the '=' or ':'
	க்கு (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    எனில் (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	add_termcode(சாவி_name, arg, FALSE);
	*p = nextchar;
    }
    எனில் (full_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    திரும்பு வெற்று;
}

/*
 * Set an option to a new value.
 */
    நிலையான எழுத்து *
do_set_option_மதிப்பு(
    முழு		opt_idx,
    முழு		opt_flags,
    char_u	**argp,
    set_prefix_T prefix,
    set_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*சாவி_name,
    முழு		nextchar,
    முழு		afterchar,
    முழு		cp_val,
    முழு		*stopopteval,
    எழுத்து	*errbuf,
    size_t	errbuflen)
{
    முழு		மதிப்பு_checked = FALSE;
    எழுத்து	*errmsg = வெற்று;
    char_u	*arg = *argp;

    எனில் (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_set_option_bool(opt_idx, opt_flags, prefix, flags, varp,
						nextchar, afterchar, cp_val);
	எனில் (errmsg != வெற்று)
	    செல் skip;
    }
    இல்லையெனில்
    {
	// numeric or string option
	எனில் (vim_strchr((char_u *)"=:&<", nextchar) == வெற்று
		|| prefix != PREFIX_NONE)
	{
	    errmsg = e_invalid_argument;
	    செல் skip;
	}

	எனில் (flags & P_இலக்கம்)
	{
	    // numeric option
	    errmsg = do_set_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    எனில் (errmsg != வெற்று)
		செல் skip;
	}
	இல்லையெனில் எனில் (opt_idx >= 0)
	{
	    // string option
	    எனில் (do_set_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&மதிப்பு_checked, &errmsg) == FAIL)
	    {
		எனில் (errmsg != வெற்று)
		    செல் skip;
		*stopopteval = TRUE;
		செல் skip;
	    }
	}
	இல்லையெனில்
	{
	    // key code option
	    errmsg = do_set_option_keycode(&arg, சாவி_name, nextchar);
	    எனில் (errmsg != வெற்று)
		செல் skip;
	}
    }

    எனில் (opt_idx >= 0)
	did_set_option(opt_idx, opt_flags, op == OP_NONE, மதிப்பு_checked);

skip:
    *argp = arg;
    திரும்பு errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    நிலையான எழுத்து *
do_set_option(
    முழு		opt_flags,
    char_u	**argp,
    char_u	*arg_start,
    char_u	**startarg,
    முழு		*did_show,
    முழு		*stopopteval,
    எழுத்து	*errbuf,
    size_t	errbuflen)
{
    முழு		opt_idx;
    char_u	*arg;
    set_prefix_T prefix;	// no prefix, "no" prefix or "inv" prefix
    set_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	சாவி_name[2];
    முழு		nextchar;	// next non-white char after option name
    முழு		afterchar;	// character just after option name
    எழுத்து	*errmsg = வெற்று;
    முழு		சாவி;
    முழு		len;

    prefix = get_option_prefix(argp);
    arg = *argp;

    // find end of name
    சாவி = 0;
    எனில் (parse_option_name(arg, &opt_idx, &len, &சாவி) == FAIL)
	திரும்பு e_invalid_argument;

    // remember character after option name
    afterchar = arg[len];

    எனில் (in_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	எனில் (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != வெற்று
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    செல் skip;
	}
    }
    இல்லையெனில்
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	போது (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    எனில் (op != OP_NONE)
	len++;

    nextchar = arg[len];

    எனில் (opt_idx == -1 && சாவி == 0)	// found a mismatch: skip
    {
	எனில் (in_vim9script() && arg > arg_start
		&& vim_strchr((char_u *)"!&<", *arg) != வெற்று)
	    errmsg = e_no_white_space_allowed_between_option_and;
	இல்லையெனில்
	    errmsg = e_unknown_option;
	செல் skip;
    }

    எனில் (opt_idx >= 0)
    {
	எனில் (options[opt_idx].var == வெற்று)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    எனில் (vim_strchr((char_u *)"=:!&<", nextchar) == வெற்று
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    செல் skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    இல்லையெனில்
    {
	flags = P_STRING;
	varp = வெற்று;
	எனில் (சாவி < 0)
	{
	    சாவி_name[0] = KEY2TERMCAP0(சாவி);
	    சாவி_name[1] = KEY2TERMCAP1(சாவி);
	}
	இல்லையெனில்
	{
	    சாவி_name[0] = KS_சாவி;
	    சாவி_name[1] = (சாவி & 0xff);
	}
    }

    // Make sure the option value can be changed.
    எனில் (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, prefix) == FAIL)
	செல் skip;

    முழு cp_val = p_cp;
    எனில் (vim_strchr((char_u *)"?=:!&<", nextchar) != வெற்று)
    {
	arg += len;
	எனில் (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    எனில் (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    இல்லையெனில்		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	எனில் (vim_strchr((char_u *)"?!&<", nextchar) != வெற்று
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    செல் skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    எனில் (nextchar == '?'
	    || (prefix == PREFIX_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == வெற்று
		&& !(flags & P_BOOL)))
    {
	// print value
	எனில் (*did_show)
	    செய்தி_putchar('\n');	    // cursor below last one
	இல்லையெனில்
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	எனில் (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ifdef FEAT_EVAL
	    எனில் (p_verbose > 0)
	    {
		// Mention where the option was last set.
		எனில் (varp == options[opt_idx].var)
		    last_set_செய்தி(options[opt_idx].script_ctx);
		இல்லையெனில் எனில் ((முழு)options[opt_idx].indir & PV_WIN)
		    last_set_செய்தி(curwin->w_p_script_ctx[
			    (முழு)options[opt_idx].indir & PV_MASK]);
		இல்லையெனில் எனில் ((முழு)options[opt_idx].indir & PV_BUF)
		    last_set_செய்தி(curbuf->b_p_script_ctx[
			    (முழு)options[opt_idx].indir & PV_MASK]);
	    }
#endif
	}
	இல்லையெனில்
	{
	    char_u	    *p;

	    p = find_termcode(சாவி_name);
	    எனில் (p == வெற்று)
	    {
		errmsg = e_சாவி_code_not_set;
		செல் skip;
	    }
	    இல்லையெனில்
		(வெறுமை)show_one_termcode(சாவி_name, p, TRUE);
	}
	எனில் (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    இல்லையெனில்
    {
	errmsg = do_set_option_மதிப்பு(opt_idx, opt_flags, &arg, prefix, op,
					flags, varp, சாவி_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    திரும்பு errmsg;
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
    முழு
do_set(
    char_u	*arg_start,	// option string (may be written to!)
    முழு		opt_flags)
{
    char_u	*arg = arg_start;
    முழு		i;
    முழு		did_show = FALSE;   // already showed one value

    எனில் (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	செல் theend;
    }

    போது (*arg != NUL)		// loop to process all options
    {
	எனில் (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    எனில் (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		set_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    இல்லையெனில்
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	இல்லையெனில் எனில் (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	இல்லையெனில்
	{
	    முழு		stopopteval = FALSE;
	    எழுத்து	*errmsg = வெற்று;
	    எழுத்து	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_set_option(opt_flags, &arg, arg_start, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    எனில் (stopopteval)
		நிறுத்து;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    க்கு (i = 0; i < 2 ; ++i)
	    {
		போது (*arg != NUL && !VIM_ISWHITE(*arg))
		    எனில் (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		எனில் (*arg != '=')
		    நிறுத்து;
	    }

	    எனில் (errmsg != வெற்று)
	    {
		i = vim_snprintf((எழுத்து *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		எனில் (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((எழுத்து *)IObuff);	// show error highlighted
		--no_wait_return;

		திரும்பு FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    எனில் (silent_mode && did_show)
    {
	// After displaying option values in silent mode.
	silent_mode = FALSE;
	info_message = TRUE;	// use mch_msg(), not mch_errmsg()
	செய்தி_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = TRUE;
	info_message = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    திரும்பு OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    வெறுமை
did_set_option(
    முழு	    opt_idx,
    முழு	    opt_flags,	    // possibly with OPT_MODELINE
    முழு	    new_மதிப்பு_2,	    // value was replaced completely
    முழு	    மதிப்பு_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = வெற்று;
    முழு		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_SET;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    எனில் (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    எனில் (!மதிப்பு_checked && (secure
#ifdef HAVE_SANDBOX
	    || sandbox != 0
#endif
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	எனில் (flagsp_local != வெற்று)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    இல்லையெனில் எனில் (new_மதிப்பு_2)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	எனில் (flagsp_local != வெற்று)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    முழு
string_to_சாவி(char_u *arg, முழு multi_byte)
{
    எனில் (*arg == '<' && arg[1])
	திரும்பு find_சாவி_option(arg + 1, TRUE);
    எனில் (*arg == '^' && arg[1])
    {
	முழு சாவி = Ctrl_chr(arg[1]);
	எனில் (சாவி == 0)		// ^@ is <Nul>
	    சாவி = K_ZERO;
	திரும்பு சாவி;
    }
    எனில் (multi_byte)
	திரும்பு PTR2CHAR(arg);
    திரும்பு *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    வெறுமை
did_set_title(வெறுமை)
{
    எனில் (starting != NO_SCREEN
#ifdef FEAT_GUI
	    && !gui.starting
#endif
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    வெறுமை
set_options_bin(
    முழு		oldval,
    முழு		newval,
    முழு		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    எனில் (newval)
    {
	எனில் (!oldval)		// switched on
	{
	    எனில் (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    எனில் (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	எனில் (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	எனில் (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    இல்லையெனில் எனில் (oldval)		// switched off
    {
	எனில் (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	எனில் (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#எனில் defined(FEAT_EVAL)
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
    நிலையான char_u *
option_expand(முழு opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    எனில் (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == வெற்று)
	திரும்பு வெற்று;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    எனில் (val != வெற்று && STRLEN(val) > MAXPATHL)
	திரும்பு வெற்று;

    எனில் (val == வெற்று)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    முழு esc = var == &p_tags || var == &p_path;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : வெற்று, FALSE,
#ifdef FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#endif
				  வெற்று);
    எனில் (STRCMP(NameBuff, val) == 0)   // they are the same
	திரும்பு வெற்று;

    திரும்பு NameBuff;
}

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    நிலையான வெறுமை
didset_options(வெறுமை)
{
    // initialize the table for 'iskeyword' et.al.
    (வெறுமை)init_chartab();

    didset_string_options();

#ifdef FEAT_SPELL
    (வெறுமை)spell_check_msm();
    (வெறுமை)spell_check_sps();
    (வெறுமை)compile_cap_prog(curwin->w_s);
    (வெறுமை)did_set_spell_option();
#endif
    // set cedit_key
    (வெறுமை)did_set_cedit(வெற்று);
#ifdef FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_set_breakat(வெற்று);
#endif
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    நிலையான வெறுமை
didset_options2(வெறுமை)
{
    // Initialize the highlight_attr[] table.
    (வெறுமை)highlight_changed();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (வெறுமை)set_listchars_option(curwin, curwin->w_p_lcs, TRUE, வெற்று, 0);

    // Parse default for 'fillchars'.
    (வெறுமை)set_fillchars_option(curwin, curwin->w_p_fcs, TRUE, வெற்று, 0);

#ifdef FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (வெறுமை)did_set_clipboard(வெற்று);
#endif
#ifdef FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (வெறுமை)tabstop_set(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (வெறுமை)tabstop_set(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#endif
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    வெறுமை
check_options(வெறுமை)
{
    முழு		opt_idx;

    க்கு (opt_idx = 0; options[opt_idx].fullname != வெற்று; opt_idx++)
	எனில் ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != வெற்று)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    முழு
get_term_opt_idx(char_u **p)
{
    முழு opt_idx;

    க்கு (opt_idx = 1; options[opt_idx].fullname != வெற்று; opt_idx++)
	எனில் (options[opt_idx].var == (char_u *)p)
	    திரும்பு opt_idx;
    திரும்பு -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    முழு
set_term_option_alloced(char_u **p)
{
    முழு		opt_idx = get_term_opt_idx(p);

    எனில் (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    திரும்பு opt_idx;
}

#எனில் defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    முழு
was_set_insecurely(win_T *wp, char_u *opt, முழு opt_flags)
{
    முழு	    idx = findoption(opt);
    long_u  *flagp;

    எனில் (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	திரும்பு (*flagp & P_INSECURE) != 0;
    }
    internal_error("was_set_insecurely()");
    திரும்பு -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    நிலையான long_u *
insecure_flag(win_T *wp, முழு opt_idx, முழு opt_flags)
{
    எனில் (opt_flags & OPT_LOCAL)
	தேர்வு ((முழு)options[opt_idx].indir)
	{
	    நிலை PV_WRAP:	திரும்பு &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    நிலை PV_STL:	திரும்பு &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    நிலை PV_FDE:	திரும்பு &wp->w_p_fde_flags;
	    நிலை PV_FDT:	திரும்பு &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    நிலை PV_BEXPR:	திரும்பு &wp->w_buffer->b_p_bexpr_flags;
#  endif
	    நிலை PV_INDE:	திரும்பு &wp->w_buffer->b_p_inde_flags;
	    நிலை PV_FEX:	திரும்பு &wp->w_buffer->b_p_fex_flags;
#  ifdef FEAT_FIND_ID
	    நிலை PV_INEX:	திரும்பு &wp->w_buffer->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    நிலை PV_CPT:	திரும்பு &wp->w_buffer->b_p_cpt_flags;
#  endif
# endif
	}
    இல்லையெனில்
	// For global value of window-local options, use flags in w_allbuf_opt.
	தேர்வு ((முழு)options[opt_idx].indir)
	{
	    நிலை PV_WRAP:	திரும்பு &wp->w_allbuf_opt.wo_wrap_flags;
# எனில் defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    நிலை PV_FDE:	திரும்பு &wp->w_allbuf_opt.wo_fde_flags;
	    நிலை PV_FDT:	திரும்பு &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    திரும்பு &options[opt_idx].flags;
}
#endif

/*
 * Redraw the window title and/or tab page text later.
 */
    வெறுமை
redraw_titles(வெறுமை)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#எனில் defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    முழு
valid_name(char_u *val, எழுத்து *allowed)
{
    char_u *s;

    க்கு (s = val; *s != NUL; ++s)
	எனில் (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == வெற்று)
	    திரும்பு FALSE;
    திரும்பு TRUE;
}

#எனில் defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    வெறுமை
set_option_sctx_idx(முழு opt_idx, முழு opt_flags, sctx_T script_ctx)
{
    முழு		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    முழு		indir = (முழு)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    எனில் (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    எனில் (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    எனில் (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    எனில் (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    எனில் (both || (opt_flags & OPT_LOCAL))
    {
	எனில் (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	இல்லையெனில் எனில் (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	    எனில் (both)
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
is_option_மதிப்பு_vim9(முழு opt_idx, முழு opt_flags)
{
    முழு		indir = (முழு)options[opt_idx].indir;
    sctx_T	*sctx = வெற்று;

    எனில் ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    எனில் ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	எனில் (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_MASK];
	இல்லையெனில் எனில் (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_MASK];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    திரும்பு !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_version >= SCRIPT_VERSION_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(எழுத்து *name)
{
    முழு idx = findoption((char_u *)name);

    எனில் (idx >= 0)
	திரும்பு &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    திரும்பு வெற்று;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    வெறுமை
set_term_option_sctx_idx(எழுத்து *name, முழு opt_idx)
{
    char_u  buf[5];
    முழு	    idx;

    எனில் (name == வெற்று)
	idx = opt_idx;
    இல்லையெனில்
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    எனில் (idx >= 0)
	set_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#endif

#எனில் defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    நிலையான வெறுமை
apply_optionset_autocmd(
	முழு	opt_idx,
	long	opt_flags,
	long	oldval,
	long	oldval_g,
	long	newval,
	எழுத்து	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    எனில் (starting || errmsg != வெற்று || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	திரும்பு;

    len = vim_snprintf_safelen((எழுத்து *)buf_new, அளவு(buf_new), "%ld", newval);
    set_vim_var_string(VV_OPTION_NEW, buf_new, (முழு)len);
    len = vim_snprintf_safelen((எழுத்து *)buf_type, அளவு(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (முழு)len);

    buf_oldlen = vim_snprintf_safelen((எழுத்து *)buf_old, அளவு(buf_old), "%ld", oldval);
    set_vim_var_string(VV_OPTION_OLD, buf_old, (முழு)buf_oldlen);

    எனில் (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (முழு)buf_oldlen);
    }
    எனில் (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (முழு)buf_oldlen);
    }
    எனில் ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((எழுத்து *)buf_old_global,
	    அளவு(buf_old_global), "%ld", oldval_g);

	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (முழு)buf_oldlen);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (முழு)buf_old_globallen);
    }
    எனில் (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (முழு)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    வெற்று, FALSE, வெற்று);
    reset_v_option_vars();
}
#endif

#எனில் defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    எழுத்து *
did_set_arabic(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	எனில் (!p_tbidi)
	{
	    // set rightleft mode
	    எனில் (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		changed_window_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    எனில் (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	எனில் (STRCMP(p_enc, "utf-8") != 0)
	{
	    நிலையான எழுத்து *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    செய்தி_source(HL_ATTR(HLF_W));
	    செய்தி_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = set_option_மதிப்பு((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    இல்லையெனில்
    {
	// 'arabic' is reset, handle various sub-settings.
	எனில் (!p_tbidi)
	{
	    // reset rightleft mode
	    எனில் (curwin->w_p_rl)
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
	curbuf->b_p_imsearch = B_IMODE_USE_செருகு;
# endif
    }

    திரும்பு errmsg;
}
#endif

#எனில் defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    எழுத்து *
did_set_autochdir(optset_T *args UNUSED)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    எழுத்து *
did_set_ballooneval(optset_T *args)
{
    எனில் (balloonEvalForTerm)
	திரும்பு வெற்று;

    எனில் (p_beval && !args->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    இல்லையெனில் எனில் (!p_beval && args->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_BEVAL_TERM)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    எழுத்து *
did_set_balloonevalterm(optset_T *args UNUSED)
{
    mch_bevalterm_changed();
    திரும்பு வெற்று;
}
#endif

/*
 * Process the updated 'binary' option value.
 */
    எழுத்து *
did_set_இருமம்(optset_T *args)
{
    // when 'bin' is set also set some other options
    set_options_bin(args->os_oldval.boolean, curbuf->b_p_bin, args->os_flags);
    redraw_titles();

    திரும்பு வெற்று;
}

/*
 * Process the updated 'buflisted' option value.
 */
    எழுத்து *
did_set_buflisted(optset_T *args)
{
    // when 'buflisted' changes, trigger autocommands
    எனில் (args->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						வெற்று, வெற்று, TRUE, curbuf);
    திரும்பு வெற்று;
}

/*
 * Process the new 'cmdheight' option value.
 */
    எழுத்து *
did_set_cmdheight(optset_T *args)
{
    long old_மதிப்பு = args->os_oldval.எண்;
    எழுத்து *errmsg = வெற்று;

    // if p_ch changed value, change the command line height
    எனில் (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_argument_must_be_positive;
	p_ch = MIN_CMDHEIGHT;
    }
    எனில் (p_ch > Rows - min_rows() + MIN_CMDHEIGHT)
	p_ch = Rows - min_rows() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    எனில் ((p_ch != old_மதிப்பு
		|| tabline_height() + topframe->fr_height != Rows - p_ch)
	    && full_screen
#ifdef FEAT_GUI
	    && !gui.starting
#endif
       )
	command_height();

    திரும்பு errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    எழுத்து *
did_set_compatible(optset_T *args UNUSED)
{
    compatible_set();
    திரும்பு வெற்று;
}

#எனில் defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    எழுத்து *
did_set_conceallevel(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (curwin->w_p_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_cole = 0;
    }
    இல்லையெனில் எனில் (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_cole = 3;
    }
    எனில் (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    இல்லையெனில் எனில் (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    திரும்பு errmsg;
}
#endif

#எனில் defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    எழுத்து *
did_set_diff(optset_T *args UNUSED)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    எனில் (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    திரும்பு வெற்று;
}
#endif

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    எழுத்து *
did_set_eof_eol_fixeol_bomb(optset_T *args UNUSED)
{
    // redraw the window title and tab page text
    redraw_titles();
    திரும்பு வெற்று;
}

/*
 * Process the updated 'equalalways' option value.
 */
    எழுத்து *
did_set_equalalways(optset_T *args)
{
    எனில் (p_ea && !args->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    திரும்பு வெற்று;
}

#எனில் defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    எழுத்து *
did_set_foldcolumn(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (curwin->w_p_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_fdc = 0;
    }
    இல்லையெனில் எனில் (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_fdc = 12;
    }
    எனில் (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    இல்லையெனில் எனில் (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    திரும்பு errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    எழுத்து *
did_set_foldlevel(optset_T *args UNUSED)
{
    எனில் (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    திரும்பு வெற்று;
}

/*
 * Process the new 'foldminlines' option value.
 */
    எழுத்து *
did_set_foldminlines(optset_T *args UNUSED)
{
    foldUpdateAll(curwin);
    திரும்பு வெற்று;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    எழுத்து *
did_set_foldnestmax(optset_T *args UNUSED)
{
    எனில் (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_தேடல்_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    எழுத்து *
did_set_hlsearch(optset_T *args UNUSED)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    set_no_hlsearch(FALSE);
    திரும்பு வெற்று;
}
#endif

/*
 * Process the updated 'ignorecase' option value.
 */
    எழுத்து *
did_set_ignorecase(optset_T *args UNUSED)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    எனில் (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    திரும்பு வெற்று;
}

#எனில் defined(HAVE_உள்ளீடு_METHOD)
/*
 * Process the updated 'imdisable' option value.
 */
    எழுத்து *
did_set_imdisable(optset_T *args UNUSED)
{
    // Only de-activate it here, it will be enabled when changing mode.
    எனில் (p_imdisable)
	im_set_active(FALSE);
    இல்லையெனில் எனில் (State & MODE_செருகு)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
    திரும்பு வெற்று;
}
#endif

/*
 * Process the new 'iminsert' option value.
 */
    எழுத்து *
did_set_iminsert(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    எனில் (termcap_active)	// don't do this in the alternate screen
	showmode();
#எனில் defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif

    திரும்பு errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    எழுத்து *
did_set_imsearch(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    திரும்பு errmsg;
}

#எனில் defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    எழுத்து *
did_set_imstyle(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_argument;

    திரும்பு errmsg;
}
#endif

/*
 * Process the updated 'insertmode' option value.
 */
    எழுத்து *
did_set_insertmode(optset_T *args)
{
    // when 'insertmode' is set from an autocommand need to do work here
    எனில் (p_im)
    {
	எனில் ((State & MODE_செருகு) == 0)
	    need_start_insertmode = TRUE;
	stop_செருகு_mode = FALSE;
    }
    // only reset if it was set previously
    இல்லையெனில் எனில் (args->os_oldval.boolean)
    {
	need_start_insertmode = FALSE;
	stop_செருகு_mode = TRUE;
	எனில் (restart_edit != 0 && mode_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    திரும்பு வெற்று;
}

#எனில் defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    எழுத்து *
did_set_langnoremap(optset_T *args UNUSED)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    திரும்பு வெற்று;
}

/*
 * Process the updated 'langremap' option value.
 */
    எழுத்து *
did_set_langremap(optset_T *args UNUSED)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    திரும்பு வெற்று;
}
#endif

/*
 * Process the new 'laststatus' option value.
 */
    எழுத்து *
did_set_laststatus(optset_T *args UNUSED)
{
    last_status(FALSE);	// (re)set last window status line
    திரும்பு வெற்று;
}

#எனில் defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    எழுத்து *
did_set_linespace(optset_T *args UNUSED)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    எனில் (gui.in_use && gui_mch_adjust_charheight() == OK)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    திரும்பு வெற்று;
}
#endif

/*
 * Process the updated 'lisp' option value.
 */
    எழுத்து *
did_set_lisp(optset_T *args UNUSED)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (வெறுமை)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    திரும்பு வெற்று;
}

/*
 * Process the new 'maxcombine' option value.
 */
    எழுத்து *
did_set_maxcombine(optset_T *args UNUSED)
{
    எனில் (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    இல்லையெனில் எனில் (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    திரும்பு வெற்று;
}

/*
 * Process the updated 'modifiable' option value.
 */
    எழுத்து *
did_set_modifiable(optset_T *args UNUSED)
{
    // when 'modifiable' is changed, redraw the window title

#ifdef FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    எனில் (curbuf->b_p_ma && (term_in_normal_mode(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_term != வெற்று && !term_is_finished(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	திரும்பு e_cannot_make_terminal_with_running_job_modifiable;
    }
#endif
    redraw_titles();

    திரும்பு வெற்று;
}

/*
 * Process the updated 'modified' option value.
 */
    எழுத்து *
did_set_modified(optset_T *args)
{
    எனில் (!args->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_set = !!args->os_newval.boolean;
    திரும்பு வெற்று;
}

#எனில் defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    எழுத்து *
did_set_mousehide(optset_T *args UNUSED)
{
    எனில் (!p_mh)
	gui_mch_mousehide(FALSE);
    திரும்பு வெற்று;
}
#endif

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    எழுத்து *
did_set_எண்_relativenumber(optset_T *args UNUSED)
{
#எனில் defined(FEAT_SIGNS) && defined(FEAT_GUI)
    எனில் (gui.in_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != வெற்று)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	எனில் (!(curwin->w_p_nu && ((முழு *)args->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#endif
    திரும்பு வெற்று;
}

#எனில் defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    எழுத்து *
did_set_numberwidth(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    // 'numberwidth' must be positive
    எனில் (curwin->w_p_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_nuw = 1;
    }
    இல்லையெனில் எனில் (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_nuw = 20;
    }
    எனில் (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    இல்லையெனில் எனில் (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_வரி_மொத்தம் = 0; // trigger a redraw

    திரும்பு errmsg;
}
#endif

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    எழுத்து *
did_set_osctimeoutlen(optset_T *args)
{
    எனில் (p_ost < 0)
    {
	p_ost = args->os_oldval.எண்;
	திரும்பு e_argument_must_be_positive;
    }

    திரும்பு வெற்று;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    எழுத்து *
did_set_paste(optset_T *args UNUSED)
{
    நிலையான முழு	old_p_paste = FALSE;
    நிலையான முழு	save_sm = 0;
    நிலையான முழு	save_sta = 0;
    நிலையான முழு	save_ru = 0;
#ifdef FEAT_RIGHTLEFT
    நிலையான முழு	save_ri = 0;
    நிலையான முழு	save_hkmap = 0;
#endif
    buf_T	*buf;

    எனில் (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	எனில் (!old_p_paste)
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
		எனில் (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != empty_option
					   ? vim_strsave(buf->b_p_vsts) : வெற்று;
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
	    எனில் (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != empty_option
						  ? vim_strsave(p_vsts) : வெற்று;
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
	    எனில் (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = empty_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#endif
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	எனில் (p_ru)
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
	எனில் (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = empty_option;
#endif
    }

    // Paste switched from on to off: Restore saved values.
    இல்லையெனில் எனில் (old_p_paste)
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
	    எனில் (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : empty_option;
	    vim_free(buf->b_p_vsts_array);
	    எனில் (buf->b_p_vsts && buf->b_p_vsts != empty_option)
		(வெறுமை)tabstop_set(buf->b_p_vsts, &buf->b_p_vsts_array);
	    இல்லையெனில்
		buf->b_p_vsts_array = வெற்று;
#endif
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	எனில் (p_ru != save_ru)
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
	எனில் (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : empty_option;
#endif
    }

    old_p_paste = p_paste;

#எனில் defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#endif

    திரும்பு வெற்று;
}


#ifdef FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    எழுத்து *
did_set_previewwindow(optset_T *args UNUSED)
{
    எனில் (!curwin->w_p_pvw)
	திரும்பு வெற்று;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	எனில் (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    திரும்பு e_preview_window_already_exists;
	}

    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    எழுத்து *
did_set_pyxversion(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_argument;

    திரும்பு errmsg;
}
#endif

/*
 * Process the updated 'readonly' option value.
 */
    எழுத்து *
did_set_readonly(optset_T *args)
{
    // when 'readonly' is reset globally, also reset readonlymode
    எனில் (!curbuf->b_p_ro && (args->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    எனில் (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    திரும்பு வெற்று;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    எழுத்து *
did_set_scrollbind(optset_T *args UNUSED)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    எனில் (!curwin->w_p_scb)
	திரும்பு வெற்று;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    திரும்பு வெற்று;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    எழுத்து *
did_set_maxsearchcount(optset_T *args UNUSED)
{
    எழுத்து	*errmsg = வெற்று;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#define MAX_தேடல்_மொத்தம் 9999

    எனில் (p_msc <= 0)
	errmsg = e_argument_must_be_positive;
    இல்லையெனில் எனில் (p_msc > MAX_தேடல்_மொத்தம்)
	errmsg = e_invalid_argument;

    எனில் (errmsg != வெற்று)
	p_msc = 99;

    திரும்பு errmsg;
#undef MAX_தேடல்_மொத்தம்
}

#எனில் defined(BACKSLASH_IN_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    எழுத்து *
did_set_shellslash(optset_T *args UNUSED)
{
    எனில் (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    இல்லையெனில்
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
    திரும்பு வெற்று;
}
#endif

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    எழுத்து *
did_set_shiftwidth_tabstop(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    எழுத்து	*errmsg = வெற்று;

    எனில் (curbuf->b_p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_ts = 8;
    }
    இல்லையெனில் எனில் (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_ts = 8;
    }
    எனில் (p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ts = 8;
    }
    இல்லையெனில் எனில் (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	p_ts = 8;
    }

    எனில் (curbuf->b_p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_மொத்தம்(curbuf->b_p_vts_array) > 0
		       ? tabstop_first(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#இல்லையெனில்
	curbuf->b_p_sw = curbuf->b_p_ts;
#endif
    }
    எனில் (p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_மொத்தம்(curbuf->b_p_vts_array) > 0
	     ? tabstop_first(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#இல்லையெனில்
	p_sw = curbuf->b_p_ts;
#endif
    }

#ifdef FEAT_FOLDING
    எனில் (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#endif
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    எனில் (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    திரும்பு errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    எழுத்து *
did_set_showtabline(optset_T *args UNUSED)
{
    shell_new_rows();	// recompute window positions and heights
    திரும்பு வெற்று;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    எழுத்து *
did_set_smoothscroll(optset_T *args UNUSED)
{
    எனில் (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    திரும்பு வெற்று;
}

#எனில் defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    எழுத்து *
did_set_spell(optset_T *args UNUSED)
{
    எனில் (curwin->w_p_spell)
	திரும்பு parse_spelllang(curwin);

    திரும்பு வெற்று;
}
#endif

/*
 * Process the updated 'swapfile' option value.
 */
    எழுத்து *
did_set_swapfile(optset_T *args UNUSED)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    எனில் (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    இல்லையெனில்
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    திரும்பு வெற்று;
}

#எனில் defined(FEAT_TERMGUICOLORS)
    எழுத்து *
did_set_termguicolors(optset_T *args UNUSED)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    எனில் (
#  ifdef VIMDLL
	    !gui.in_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_set = TRUE;
	p_tgc = 0;
	திரும்பு e_24_bit_colors_are_not_supported_on_this_environment;
    }
    எனில் (is_term_win32())
	swap_tcap();
# endif
# ifdef FEAT_GUI
    எனில் (!gui.in_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    எனில் (is_term_win32())
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

    திரும்பு வெற்று;
}
#endif

    எழுத்து *
did_set_termsync(optset_T *args UNUSED)
{
    எனில் (!p_tsy)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);
    திரும்பு வெற்று;
}

#எனில் defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    எழுத்து *
did_set_termwinscroll(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    எழுத்து	*errmsg = வெற்று;

    எனில் (*pp < 1)
    {
	errmsg = e_argument_must_be_positive;
	*pp = 1;
    }

    திரும்பு errmsg;
}
#endif

/*
 * Process the updated 'terse' option value.
 */
    எழுத்து *
did_set_terse(optset_T *args UNUSED)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_தேடல்);

    // insert 's' in p_shm
    எனில் (p_terse && p == வெற்று)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    இல்லையெனில் எனில் (!p_terse && p != வெற்று)
	STRMOVE(p, p + 1);
    திரும்பு வெற்று;
}

/*
 * Process the updated 'textauto' option value.
 */
    எழுத்து *
did_set_textauto(optset_T *args)
{
    // when 'textauto' is set or reset also change 'fileformats'
    set_string_option_direct((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | args->os_flags, 0);

    திரும்பு வெற்று;
}

/*
 * Process the updated 'textmode' option value.
 */
    எழுத்து *
did_set_textmode(optset_T *args)
{
    // when 'textmode' is set or reset also change 'fileformat'
    set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, args->os_flags);

    திரும்பு வெற்று;
}

/*
 * Process the new 'textwidth' option value.
 */
    எழுத்து *
did_set_textwidth(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (curbuf->b_p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_tw = 0;
    }
    எனில் (p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tw = 0;
    }
#ifdef FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(வெற்று, wp);
    }
#endif

    திரும்பு errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    எழுத்து *
did_set_title_icon(optset_T *args UNUSED)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_set_title();
    திரும்பு வெற்று;
}

/*
 * Process the new 'titlelen' option value.
 */
    எழுத்து *
did_set_titlelen(optset_T *args)
{
    long old_மதிப்பு = args->os_oldval.எண்;
    எழுத்து *errmsg = வெற்று;

    // if 'titlelen' has changed, redraw the title
    எனில் (p_titlelen < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_titlelen = 85;
    }
    எனில் (starting != NO_SCREEN && old_மதிப்பு != p_titlelen)
	need_maketitle = TRUE;

    திரும்பு errmsg;
}

#எனில் defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    எழுத்து *
did_set_undofile(optset_T *args)
{
    // Only take action when the option was set.
    எனில் (!curbuf->b_p_udf && !p_udf)
	திரும்பு வெற்று;

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
	எனில் ((curbuf == save_curbuf
		    || (args->os_flags & OPT_GLOBAL)
		    || args->os_flags == 0)
		&& !curbufIsChanged() && curbuf->b_ml.ml_mfp != வெற்று)
	{
# ifdef FEAT_CRYPT
	    எனில் (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
		தொடர்;
# endif
	    u_compute_hash(hash);
	    u_read_undo(வெற்று, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    திரும்பு வெற்று;
}
#endif

/*
 * Process the new global 'undolevels' option value.
 */
    நிலையான வெறுமை
update_global_undolevels(long மதிப்பு, long old_மதிப்பு)
{
    // sync undo before 'undolevels' changes

    // use the old value, otherwise u_sync() may not work properly
    p_ul = old_மதிப்பு;
    u_sync(TRUE);
    p_ul = மதிப்பு;
}

/*
 * Process the new buffer local 'undolevels' option value.
 */
    நிலையான வெறுமை
update_buflocal_undolevels(long மதிப்பு, long old_மதிப்பு)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_மதிப்பு;
    u_sync(TRUE);
    curbuf->b_p_ul = மதிப்பு;
}

/*
 * Process the new 'undolevels' option value.
 */
    எழுத்து *
did_set_undolevels(optset_T *args)
{
    long *pp = (long *)args->os_varp;

    எனில் (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(args->os_newval.எண்,
						       args->os_oldval.எண்);
    இல்லையெனில் எனில் (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(args->os_newval.எண்,
						       args->os_oldval.எண்);

    திரும்பு வெற்று;
}

/*
 * Process the new 'updatecount' option value.
 */
    எழுத்து *
did_set_updatecount(optset_T *args)
{
    long old_மதிப்பு = args->os_oldval.எண்;
    எழுத்து *errmsg = வெற்று;

    // when 'updatecount' changes from zero to non-zero, open swap files
    எனில் (p_uc < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_uc = 100;
    }
    எனில் (p_uc && !old_மதிப்பு)
	ml_open_files();

    திரும்பு errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    எழுத்து *
did_set_weirdinvert(optset_T *args)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    எனில் (p_wiv && !args->os_oldval.boolean)
	T_XS = (char_u *)"y";
    இல்லையெனில் எனில் (!p_wiv && args->os_oldval.boolean)
	T_XS = empty_option;
    p_wiv = (*T_XS != NUL);

    திரும்பு வெற்று;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    எழுத்து *
did_set_wildchar(optset_T *args)
{
    long c = *(long *)args->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    எனில் (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

/*
 * Process the new 'window' option value.
 */
    எழுத்து *
did_set_window(optset_T *args UNUSED)
{
    எனில் (p_window < 1)
	p_window = 1;
    இல்லையெனில் எனில் (p_window >= Rows)
	p_window = Rows - 1;
    திரும்பு வெற்று;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    எழுத்து *
did_set_winheight_helpheight(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    எழுத்து	*errmsg = வெற்று;

    எனில் (p_wh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wh = 1;
    }
    எனில் (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    எனில் (p_hh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hh = 0;
    }

    // Change window height NOW
    எனில் (!ONE_WINDOW)
    {
	எனில் (pp == &p_wh && curwin->w_height < p_wh)
	    win_setheight((முழு)p_wh);
	எனில் (pp == &p_hh && curbuf->b_help && curwin->w_height < p_hh)
	    win_setheight((முழு)p_hh);
    }

    திரும்பு errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    எழுத்து *
did_set_winminheight(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (p_wmh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmh = 0;
    }
    எனில் (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    திரும்பு errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    எழுத்து *
did_set_winminwidth(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (p_wmw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmw = 0;
    }
    எனில் (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    திரும்பு errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    எழுத்து *
did_set_winwidth(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (p_wiw < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wiw = 1;
    }
    எனில் (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    எனில் (!ONE_WINDOW && curwin->w_width < p_wiw)
	win_setwidth((முழு)p_wiw);

    திரும்பு errmsg;
}

#எனில் defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    எழுத்து *
did_set_wlsteal(optset_T *args UNUSED)
{
    clip_reset_wayland();

    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    எழுத்து *
did_set_wltimeoutlen(optset_T *args)
{
    எனில் (p_wtm < 0)
    {
	p_wtm = args->os_oldval.எண்;
	திரும்பு e_argument_must_be_positive;
    }

    திரும்பு வெற்று;
}
#endif

/*
 * Process the updated 'wrap' option value.
 */
    எழுத்து *
did_set_wrap(optset_T *args UNUSED)
{
    // Set w_leftcol or w_skipcol to zero.
    எனில் (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    இல்லையெனில்
	curwin->w_skipcol = 0;

    திரும்பு வெற்று;
}

#ifdef FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    எழுத்து *
did_set_xhistory(optset_T *args)
{
    முழு is_p_chi = (long*)args->os_varp == &p_chi;
    முழு err;
    long *arg = (is_p_chi) ? &p_chi :(long*)args->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    எனில் (*arg < 1)
    {
	*arg = args->os_oldval.எண்;
	திரும்பு e_cannot_have_negative_or_zero_எண்_of_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    எனில் (*arg > 100)
    {
	*arg = args->os_oldval.எண்;
	திரும்பு e_cannot_have_more_than_hundred_quickfix;
    }

    எனில் (is_p_chi)
	err = qf_resize_அடுக்கு(*arg);
    இல்லையெனில்
	err = ll_resize_அடுக்கு(curwin, *arg);

    எனில் (err == FAIL)
	திரும்பு e_failed_resizing_quickfix_அடுக்கு;

    திரும்பு வெற்று;
}
#endif

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    நிலையான எழுத்து *
set_bool_option(
    முழு		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    முழு		மதிப்பு,			// new value
    முழு		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    முழு		old_மதிப்பு = *(முழு *)varp;
#எனில் defined(FEAT_EVAL)
    முழு		old_global_மதிப்பு = 0;
#endif
    எழுத்து	*errmsg = வெற்று;

    // Disallow changing some options from secure mode
    எனில் ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	திரும்பு e_not_allowed_here;

#எனில் defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    எனில் ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_மதிப்பு = *(முழு *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *(முழு *)varp = மதிப்பு;	    // set the new value
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    எனில் ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(முழு *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = மதிப்பு;

    // Handle side effects of changing a bool option.
    எனில் (options[opt_idx].opt_did_set_cb != வெற்று)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.boolean = old_மதிப்பு;
	args.os_newval.boolean = மதிப்பு;
	args.os_errbuf = வெற்று;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
	எனில் (errmsg != வெற்று)
	    திரும்பு errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_SET;

#எனில் defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(long)(old_மதிப்பு ? TRUE : FALSE),
				(long)(old_global_மதிப்பு ? TRUE : FALSE),
				(long)(மதிப்பு ? TRUE : FALSE), வெற்று);
#endif

    comp_col();			    // in case 'ruler' or 'showcmd' changed

    எனில் (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    எனில் ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    திரும்பு errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    நிலையான எழுத்து *
check_இலக்கம்_option_bounds(
    long	*pp,
    long	old_மதிப்பு,
    long	old_Rows,
    long	old_Columns,
    எழுத்து	*errbuf,
    size_t	errbuflen,
    எழுத்து	*errmsg)
{
    எனில் (Rows < min_rows_for_all_tabpages() && full_screen)
    {
	எனில் (errbuf != வெற்று)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_வரிகள்), min_rows_for_all_tabpages());
	    errmsg = errbuf;
	}
	Rows = min_rows_for_all_tabpages();
    }
    எனில் (Columns < MIN_COLUMNS && full_screen)
    {
	எனில் (errbuf != வெற்று)
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
    எனில் (old_Rows != Rows || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	எனில் (updating_screen)
	    *pp = old_மதிப்பு;
	இல்லையெனில் எனில் (full_screen
#ifdef FEAT_GUI
		&& !gui.starting
#endif
		)
	    set_shellsize((முழு)Columns, (முழு)Rows, TRUE);
	இல்லையெனில்
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    எனில் (cmdline_row > Rows - p_ch && Rows > p_ch)
		cmdline_row = Rows - p_ch;
	}
	எனில் (p_window >= Rows || !option_was_set((char_u *)"window"))
	    p_window = Rows - 1;
    }

    எனில் (p_tm < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tm = 0;
    }
    எனில் ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_height
		    && curwin->w_height > 0))
	    && full_screen)
    {
	எனில் (pp == &(curwin->w_p_scr))
	{
	    எனில் (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_size;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	இல்லையெனில் எனில் (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	இல்லையெனில் // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_height;
    }
    எனில் (p_hi < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hi = 0;
    }
    இல்லையெனில் எனில் (p_hi > 10000)
    {
	errmsg = e_invalid_argument;
	p_hi = 10000;
    }
    எனில் (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_argument;
	p_re = 0;
    }
    எனில் (p_report < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_report = 1;
    }
    எனில் ((p_sj < -100 || p_sj >= Rows) && full_screen)
    {
	எனில் (Rows != old_Rows)	// Rows changed, just adjust p_sj
	    p_sj = Rows / 2;
	இல்லையெனில்
	{
	    errmsg = e_invalid_scroll_size;
	    p_sj = 1;
	}
    }
    எனில் (p_so < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_so = 0;
    }
    எனில் (p_sop < 0 && full_screen)
    {
	errmsg = e_invalid_argument;
	p_sop = 0;
    }
    எனில் (p_siso < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_siso = 0;
    }
    எனில் (p_cwh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_cwh = 1;
    }
    எனில் (p_ut < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ut = 2000;
    }
    எனில் (p_ss < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ss = 0;
    }

    திரும்பு errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    நிலையான எழுத்து *
set_இலக்கம்_option(
    முழு		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    long	மதிப்பு,			// new value
    எழுத்து	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    முழு		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    எழுத்து	*errmsg = வெற்று;
    long	old_மதிப்பு = *(long *)varp;
#எனில் defined(FEAT_EVAL)
    long	old_global_மதிப்பு = 0;	// only used when setting a local and
					// global option
#endif
    long	old_Rows = Rows;	// remember old Rows
    long	old_Columns = Columns;	// remember old Columns
    long	*pp = (long *)varp;

    // Disallow changing some options from secure mode.
    எனில் ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	திரும்பு e_not_allowed_here;

#எனில் defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    எனில் ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_மதிப்பு = *(long *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *pp = மதிப்பு;
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    எனில் ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = மதிப்பு;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    எனில் (options[opt_idx].opt_did_set_cb != வெற்று)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.எண் = old_மதிப்பு;
	args.os_newval.எண் = மதிப்பு;
	args.os_errbuf = வெற்று;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
    }

    // Check the bounds for numeric options here
    errmsg = check_இலக்கம்_option_bounds(pp, old_மதிப்பு, old_Rows, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_SET;

#எனில் defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_மதிப்பு, old_global_மதிப்பு,
								மதிப்பு, errmsg);
#endif

    comp_col();			    // in case 'columns' or 'ls' changed

    எனில் (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    எனில் ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    திரும்பு errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    வெறுமை
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    முழு		doclear = (flags & P_RCLR) == P_RCLR;
    முழு		all = ((flags & P_RALL) == P_RALL || doclear);

    எனில் ((flags & P_RSTAT) || all)	// mark all status lines dirty
	status_redraw_all();

    எனில் ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	எனில் (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	இல்லையெனில்
	    changed_window_setting();
    }
    எனில் (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    எனில் (doclear)
	redraw_all_later(UPD_CLEAR);
    இல்லையெனில் எனில் (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    முழு
findoption(char_u *arg)
{
    முழு		    opt_idx;
    எழுத்து	    *s, *p;
    நிலையான short    quick_tab[27] = {0, 0};	// quick access table
    முழு		    is_term_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    எனில் (quick_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	முழு last_opt_idx;
	க்கு (last_opt_idx = 0; options[last_opt_idx].fullname != வெற்று;
		last_opt_idx++)
	    ;
	க்கு (முழு tab_idx = 1; tab_idx < 27; tab_idx++)
	    quick_tab[tab_idx] = last_opt_idx;

	p = options[0].fullname;
	க்கு (opt_idx = 1; opt_idx < last_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    எனில் (s[0] != p[0])
	    {
		எனில் (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		இல்லையெனில்
		    quick_tab[Char_Ord_கீழ்(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    எனில் (arg[0] < 'a' || arg[0] > 'z')
	திரும்பு -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    எனில் (is_term_opt)
	opt_idx = quick_tab[26];
    இல்லையெனில்
	opt_idx = quick_tab[Char_Ord_கீழ்(arg[0])];
    க்கு (; (s = options[opt_idx].fullname) != வெற்று && s[0] == arg[0]; opt_idx++)
    {
	எனில் (STRCMP(arg, s) == 0)		    // match full name
	    நிறுத்து;
    }
    எனில் (s != வெற்று && s[0] != arg[0])
	s = வெற்று;
    எனில் (s == வெற்று && !is_term_opt)
    {
	opt_idx = quick_tab[Char_Ord_கீழ்(arg[0])];
	க்கு ( ; options[opt_idx].fullname != வெற்று; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    எனில் (s != வெற்று && STRCMP(arg, s) == 0)   // match short name
		நிறுத்து;
	    s = வெற்று;
	}
    }
    எனில் (s == வெற்று)
	opt_idx = -1;
    திரும்பு opt_idx;
}

#எனில் defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
get_option_மதிப்பு(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    முழு		*flagsp,
    முழு		scope)
{
    முழு		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    எனில் (opt_idx < 0)		    // option not in the table
    {
	முழு சாவி;

	எனில் (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (சாவி = find_சாவி_option(name, FALSE)) != 0)
	{
	    char_u சாவி_name[2];
	    char_u *p;

	    எனில் (flagsp != வெற்று)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    எனில் (சாவி < 0)
	    {
		சாவி_name[0] = KEY2TERMCAP0(சாவி);
		சாவி_name[1] = KEY2TERMCAP1(சாவி);
	    }
	    இல்லையெனில்
	    {
		சாவி_name[0] = KS_சாவி;
		சாவி_name[1] = (சாவி & 0xff);
	    }
	    p = find_termcode(சாவி_name);
	    எனில் (p != வெற்று)
	    {
		எனில் (stringval != வெற்று)
		    *stringval = vim_strsave(p);
		திரும்பு gov_string;
	    }
	}
	திரும்பு gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    எனில் (flagsp != வெற்று)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    எனில் (options[opt_idx].flags & P_STRING)
    {
	எனில் (varp == வெற்று)		    // hidden option
	    திரும்பு gov_hidden_string;
	எனில் (stringval != வெற்று)
	{
	    எனில் ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    இல்லையெனில் எனில் ((char_u **)varp == &curbuf->b_p_சாவி
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    இல்லையெனில்
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	திரும்பு gov_string;
    }

    எனில் (varp == வெற்று)		    // hidden option
	திரும்பு (options[opt_idx].flags & P_இலக்கம்)
					 ? gov_hidden_எண் : gov_hidden_bool;
    எனில் (options[opt_idx].flags & P_இலக்கம்)
	*numval = *(long *)varp;
    இல்லையெனில்
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	எனில் ((முழு *)varp == &curbuf->b_changed)
	    *numval = curbufIsChanged();
	இல்லையெனில்
	    *numval = (long) *(முழு *)varp;
    }
    திரும்பு (options[opt_idx].flags & P_இலக்கம்) ? gov_எண் : gov_bool;
}
#endif

#எனில் defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    முழு
get_option_மதிப்பு_strict(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    முழு		opt_type,
    வெறுமை	*from)
{
    முழு		opt_idx;
    char_u	*varp = வெற்று;
    கட்டமைப்பு vimoption *p;
    முழு		r = 0;

    opt_idx = findoption(name);
    எனில் (opt_idx < 0)
	திரும்பு 0;

    p = &(options[opt_idx]);

    // Hidden option
    எனில் (p->var == வெற்று)
	திரும்பு 0;

    எனில் (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    இல்லையெனில் எனில் (p->flags & P_இலக்கம்)
	r |= SOPT_இலக்கம்;
    இல்லையெனில் எனில் (p->flags & P_STRING)
	r |= SOPT_STRING;

    எனில் (p->indir == PV_NONE)
    {
	எனில் (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	இல்லையெனில்
	    திரும்பு 0; // Did not request global-only option
    }
    இல்லையெனில்
    {
	எனில் (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	இல்லையெனில் எனில் (opt_type == SREQ_GLOBAL)
	    திரும்பு 0; // Requested global option

	எனில் (p->indir & PV_WIN)
	{
	    எனில் (opt_type == SREQ_BUF)
		திரும்பு 0; // Did not request window-local option
	    இல்லையெனில்
		r |= SOPT_WIN;
	}
	இல்லையெனில் எனில் (p->indir & PV_BUF)
	{
	    எனில் (opt_type == SREQ_WIN)
		திரும்பு 0; // Did not request buffer-local option
	    இல்லையெனில்
		r |= SOPT_BUF;
	}
    }

    எனில் (stringval == வெற்று)
	திரும்பு r;

    எனில் (opt_type == SREQ_GLOBAL)
	varp = p->var;
    இல்லையெனில்
    {
	எனில் (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    எனில் (p->indir == PV_MOD)
	    {
		*numval = bufIsChanged((buf_T *)from);
		varp = வெற்று;
	    }
# ifdef FEAT_CRYPT
	    இல்லையெனில் எனில் (p->indir == PV_சாவி)
	    {
		// never return the value of the crypt key
		*stringval = வெற்று;
		varp = வெற்று;
	    }
# endif
	    இல்லையெனில்
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
	இல்லையெனில் எனில் (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_buffer;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	}
	எனில் (varp == p->var)
	    திரும்பு (r | SOPT_UNSET);
    }

    எனில் (varp != வெற்று)
    {
	எனில் (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	இல்லையெனில் எனில் (p->flags & P_இலக்கம்)
	    *numval = *(long *) varp;
	இல்லையெனில்
	    *numval = *(முழு *)varp;
    }

    திரும்பு r;
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
option_iter_next(வெறுமை **option, முழு opt_type)
{
    கட்டமைப்பு vimoption	*ret = வெற்று;
    செய்
    {
	எனில் (*option == வெற்று)
	    *option = (வெறுமை *) options;
	இல்லையெனில் எனில் (((கட்டமைப்பு vimoption *) (*option))->fullname == வெற்று)
	{
	    *option = வெற்று;
	    திரும்பு வெற்று;
	}
	இல்லையெனில்
	    *option = (வெறுமை *) (((கட்டமைப்பு vimoption *) (*option)) + 1);

	ret = ((கட்டமைப்பு vimoption *) (*option));

	// Hidden option
	எனில் (ret->var == வெற்று)
	{
	    ret = வெற்று;
	    தொடர்;
	}

	தேர்வு (opt_type)
	{
	    நிலை SREQ_GLOBAL:
		எனில் (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = வெற்று;
		நிறுத்து;
	    நிலை SREQ_BUF:
		எனில் (!(ret->indir & PV_BUF))
		    ret = வெற்று;
		நிறுத்து;
	    நிலை SREQ_WIN:
		எனில் (!(ret->indir & PV_WIN))
		    ret = வெற்று;
		நிறுத்து;
	    இயல்பு:
		internal_error("option_iter_next()");
		திரும்பு வெற்று;
	}
    }
    போது (ret == வெற்று);

    திரும்பு (char_u *)ret->fullname;
}
#endif

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(முழு opt_idx)
{
    திரும்பு options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    வெறுமை
set_option_flag(முழு opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    முழு
is_global_option(முழு opt_idx)
{
    திரும்பு options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    முழு
is_global_local_option(முழு opt_idx)
{
    திரும்பு options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    முழு
is_window_local_option(முழு opt_idx)
{
    திரும்பு options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    முழு
is_hidden_option(முழு opt_idx)
{
    திரும்பு options[opt_idx].var == வெற்று;
}

#எனில் defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    முழு
is_crypt_சாவி_option(முழு opt_idx)
{
    திரும்பு options[opt_idx].indir == PV_சாவி;
}
#endif

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    எழுத்து *
set_option_மதிப்பு(
    char_u	*name,
    long	எண்,
    char_u	*string,
    முழு		opt_flags)	// OPT_LOCAL or 0 (both)
{
    முழு		opt_idx;
    char_u	*varp;
    long_u	flags;
    நிலையான எழுத்து	errbuf[ERR_BUFLEN];
    முழு		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    எனில் (opt_idx < 0)
    {
	முழு சாவி;

	எனில் (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (சாவி = find_சாவி_option(name, FALSE)) != 0)
	{
	    char_u சாவி_name[2];

	    எனில் (சாவி < 0)
	    {
		சாவி_name[0] = KEY2TERMCAP0(சாவி);
		சாவி_name[1] = KEY2TERMCAP1(சாவி);
	    }
	    இல்லையெனில்
	    {
		சாவி_name[0] = KS_சாவி;
		சாவி_name[1] = (சாவி & 0xff);
	    }
	    add_termcode(சாவி_name, string, FALSE);
	    எனில் (full_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    திரும்பு வெற்று;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    இல்லையெனில்
    {
	flags = options[opt_idx].flags;
#ifdef HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	எனில் (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_in_sandbox));
	    திரும்பு வெற்று;
	}
#endif
	எனில் (flags & P_STRING)
	    திரும்பு set_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	எனில் (varp != வெற்று)	// hidden option is not changed
	{
	    எனில் (எண் == 0 && string != வெற்று)
	    {
		முழு idx;

		// Either we are given a string or we are setting option
		// to zero.
		க்கு (idx = 0; string[idx] == '0'; ++idx)
		    ;
		எனில் (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_எண்_required_after_str_equal_str),
								 name, string);
		    திரும்பு வெற்று;     // do nothing as we hit an error

		}
	    }
	    எனில் (flags & P_இலக்கம்)
	    {
		திரும்பு set_இலக்கம்_option(opt_idx, varp, எண்,
					   errbuf, அளவு(errbuf), opt_flags);
	    }
	    இல்லையெனில்
		திரும்பு set_bool_option(opt_idx, varp, (முழு)எண், opt_flags);
	}

    }
    திரும்பு வெற்று;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    வெறுமை
set_option_மதிப்பு_give_err(
    char_u	*name,
    long	எண்,
    char_u	*string,
    முழு		opt_flags)	// OPT_LOCAL or 0 (both)
{
    எழுத்து *errmsg = set_option_மதிப்பு(name, எண், string, opt_flags);

    எனில் (errmsg != வெற்று)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_term_code(char_u *tname)
{
    முழு	    opt_idx;
    char_u  *varp;

    எனில் (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	திரும்பு வெற்று;
    எனில் ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	எனில் (varp != வெற்று)
	    varp = *(char_u **)(varp);
	திரும்பு varp;
    }
    திரும்பு find_termcode(tname + 2);
}

    char_u *
get_highlight_default(வெறுமை)
{
    முழு i;

    i = findoption((char_u *)"hl");
    எனில் (i >= 0)
	திரும்பு options[i].def_val[VI_DEFAULT];
    திரும்பு (char_u *)வெற்று;
}

    char_u *
get_encoding_default(வெறுமை)
{
    முழு i;

    i = findoption((char_u *)"enc");
    எனில் (i >= 0)
	திரும்பு options[i].def_val[VI_DEFAULT];
    திரும்பு (char_u *)வெற்று;
}

#எனில் defined(FEAT_QUICKFIX)
    முழு
is_option_allocated(எழுத்து *name)
{
    முழு idx = findoption((char_u *)name);

    திரும்பு idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#endif

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    நிலையான முழு
find_சாவி_option(char_u *arg_arg, முழு has_lt)
{
    முழு		சாவி = 0;
    முழு		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    எனில் (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	எனில் (!has_lt || arg[4] == '>')
	    சாவி = TERMCAP2_சாவி(arg[2], arg[3]);
    }
    இல்லையெனில் எனில் (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	சாவி = find_special_சாவி(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_சாவி | FSK_SIMPLIFY, வெற்று);
	எனில் (modifiers)		    // can't handle modifiers here
	    சாவி = 0;
    }
    திரும்பு சாவி;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    நிலையான வெறுமை
showoptions(
    முழு		all,
    முழு		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    கட்டமைப்பு vimoption	*p;
    முழு			col;
    முழு			isterm;
    char_u		*varp;
    கட்டமைப்பு vimoption	**பொருட்கள்;
    முழு			item_மொத்தம்;
    முழு			run;
    முழு			row, rows;
    முழு			cols;
    முழு			i;
    முழு			len;

#define INC 20
#define GAP 3

    பொருட்கள் = ALLOC_MULT(கட்டமைப்பு vimoption *, OPTION_மொத்தம்);
    எனில் (பொருட்கள் == வெற்று)
	திரும்பு;

    // Highlight title
    எனில் (all == 2)
	செய்தி_puts_title(_("\n--- Terminal codes ---"));
    இல்லையெனில் எனில் (opt_flags & OPT_GLOBAL)
	செய்தி_puts_title(_("\n--- Global option values ---"));
    இல்லையெனில் எனில் (opt_flags & OPT_LOCAL)
	செய்தி_puts_title(_("\n--- Local option values ---"));
    இல்லையெனில்
	செய்தி_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    க்கு (run = 1; run <= 2 && !got_int; ++run)
    {
	// collect the items in items[]
	item_மொத்தம் = 0;
	க்கு (p = &options[0]; p->fullname != வெற்று; p++)
	{
	    // apply :filter /pat/
	    எனில் (message_filtered((char_u *)p->fullname))
		தொடர்;

	    varp = வெற்று;
	    isterm = istermoption(p);
	    எனில் ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		எனில் (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    இல்லையெனில்
		varp = get_varp(p);
	    எனில் (varp != வெற்று
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		எனில் (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		இல்லையெனில் எனில் (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		இல்லையெனில்
		{
		    option_value2string(p, opt_flags);
		    len = (முழு)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		எனில் ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    பொருட்கள்[item_மொத்தம்++] = p;
	    }
	}

	// display the items
	எனில் (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    எனில் (cols == 0)
		cols = 1;
	    rows = (item_மொத்தம் + cols - 1) / cols;
	}
	இல்லையெனில்	// run == 2
	    rows = item_மொத்தம்;
	க்கு (row = 0; row < rows && !got_int; ++row)
	{
	    செய்தி_putchar('\n');			// go to next line
	    எனில் (got_int)			// 'q' typed in more
		நிறுத்து;
	    col = 0;
	    க்கு (i = row; i < item_மொத்தம்; i += rows)
	    {
		செய்தி_col = col;			// make columns
		showoneopt(பொருட்கள்[i], opt_flags);
		col += INC;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(பொருட்கள்);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    நிலையான முழு
optval_default(கட்டமைப்பு vimoption *p, char_u *varp, முழு compatible)
{
    முழு		dvi;

    எனில் (varp == வெற்று)
	திரும்பு TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    எனில் (p->flags & P_இலக்கம்)
	திரும்பு (*(long *)varp == (long)(long_i)p->def_val[dvi]);
    எனில் (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	திரும்பு (*(முழு *)varp == (முழு)(long)(long_i)p->def_val[dvi]);
    // P_STRING
    திரும்பு (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    நிலையான வெறுமை
showoneopt(
    கட்டமைப்பு vimoption	*p,
    முழு			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    முழு		save_silent = silent_mode;

    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    எனில் ((p->flags & P_BOOL) && ((முழு *)varp == &curbuf->b_changed
					? !curbufIsChanged() : !*(முழு *)varp))
	செய்தி_puts("no");
    இல்லையெனில் எனில் ((p->flags & P_BOOL) && *(முழு *)varp < 0)
	செய்தி_puts("--");
    இல்லையெனில்
	செய்தி_puts("  ");
    செய்தி_puts(p->fullname);
    எனில் (!(p->flags & P_BOOL))
    {
	செய்தி_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	செய்தி_outtrans(NameBuff);
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
    முழு
makeset(FILE *fd, முழு opt_flags, முழு local_only)
{
    கட்டமைப்பு vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = வெற்று;	// fresh value
    எழுத்து		*cmd;
    முழு			round;
    முழு			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    க்கு (pri = 1; pri >= 0; --pri)
    {
      க்கு (p = &options[0]; !istermoption(p); p++)
	எனில் (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    எனில் (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		தொடர்;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    எனில் ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		தொடர்;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    எனில் ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		தொடர்;

	    எனில் ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		தொடர்;

	    round = 2;
	    எனில் (p->indir != PV_NONE)
	    {
		எனில் (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    எனில் (!(opt_flags & OPT_LOCAL))
			தொடர்;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    எனில் (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			எனில் (!optval_default(p, varp_fresh, p_cp))
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
	    க்கு ( ; round <= 2; varp = varp_local, ++round)
	    {
		எனில் (round == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		இல்லையெனில்
		    cmd = "setlocal";

		எனில் (p->flags & P_BOOL)
		{
		    எனில் (put_setbool(fd, cmd, p->fullname, *(முழு *)varp) == FAIL)
			திரும்பு FAIL;
		}
		இல்லையெனில் எனில் (p->flags & P_இலக்கம்)
		{
		    எனில் (put_setnum(fd, cmd, p->fullname, (long *)varp) == FAIL)
			திரும்பு FAIL;
		}
		இல்லையெனில்    // P_STRING
		{
		    முழு		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    எனில் ((p->flags & P_FUNC) && *(char_u **)varp != வெற்று
			    && strstr(*(எழுத்து **)varp, "<lambda>") != வெற்று)
			தொடர்;

#ifdef FEAT_EVAL
		    legacy = !is_option_மதிப்பு_vim9(p - &options[0],
			    round == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#இல்லையெனில்
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#endif

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    எனில் (
#எனில் defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#endif
			    p->indir == PV_FT)
		    {
			எனில் (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    திரும்பு FAIL;
			do_endif = TRUE;
		    }
		    எனில் (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			திரும்பு FAIL;
		    எனில் (do_endif)
		    {
			எனில் (put_வரி(fd, "endif") == FAIL)
			    திரும்பு FAIL;
		    }
		}
	    }
	}
    }
    திரும்பு OK;
}

#எனில் defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    முழு
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_version < SCRIPT_VERSION_VIM9;
# endif

    எனில் (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	திரும்பு FAIL;

    திரும்பு OK;
}
#endif

    நிலையான முழு
put_setstring(
    FILE	*fd,
    bool	legacy,
    எழுத்து	*cmd,
    எழுத்து	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = வெற்று;
    char_u	*part = வெற்று;
    char_u	*p;

    எனில் (legacy && fprintf(fd, "legacy ") < 0)
	திரும்பு FAIL;
    எனில் (fprintf(fd, "%s %s=", cmd, name) < 0)
	திரும்பு FAIL;
    எனில் (*valuep != வெற்று)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	எனில் (valuep == &p_pt)
	{
	    s = *valuep;
	    போது (*s != NUL)
		எனில் (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    திரும்பு FAIL;
	}
	// expand the option value, replace $HOME by ~
	இல்லையெனில் எனில் ((flags & P_EXPAND) != 0)
	{
	    முழு  size = (முழு)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(size);
	    எனில் (buf == வெற்று)
		செல் fail;
	    home_replace(வெற்று, *valuep, buf, size, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    எனில் (size >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != வெற்று)
	    {
		part = alloc(size);
		எனில் (part == வெற்று)
		    செல் fail;

		// write line break to clear the option, e.g. ':set rtp='
		எனில் (put_eol(fd) == FAIL)
		    செல் fail;

		p = buf;
		போது (*p != NUL)
		{
		    எனில் (legacy && fprintf(fd, "legacy ") < 0)
			திரும்பு FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    எனில் (fprintf(fd, "%s %s+=", cmd, name) < 0)
			செல் fail;
		    (வெறுமை)copy_option_part(&p, part, size,  ",");
		    எனில் (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			செல் fail;
		}
		vim_free(buf);
		vim_free(part);
		திரும்பு OK;
	    }
	    எனில் (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		திரும்பு FAIL;
	    }
	    vim_free(buf);
	}
	இல்லையெனில் எனில் (put_escstr(fd, *valuep, 2) == FAIL)
	    திரும்பு FAIL;
    }
    எனில் (put_eol(fd) < 0)
	திரும்பு FAIL;
    திரும்பு OK;
fail:
    vim_free(buf);
    vim_free(part);
    திரும்பு FAIL;
}

    நிலையான முழு
put_setnum(
    FILE	*fd,
    எழுத்து	*cmd,
    எழுத்து	*name,
    long	*valuep)
{
    long	wc;

    எனில் (fprintf(fd, "%s %s=", cmd, name) < 0)
	திரும்பு FAIL;
    எனில் (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	எனில் (fputs((எழுத்து *)get_special_சாவி_name((முழு)wc, 0), fd) < 0)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (fprintf(fd, "%ld", *valuep) < 0)
	திரும்பு FAIL;
    எனில் (put_eol(fd) < 0)
	திரும்பு FAIL;
    திரும்பு OK;
}

    நிலையான முழு
put_setbool(
    FILE	*fd,
    எழுத்து	*cmd,
    எழுத்து	*name,
    முழு		மதிப்பு)
{
    எனில் (மதிப்பு < 0)	// global/local option using global value
	திரும்பு OK;
    எனில் (fprintf(fd, "%s %s%s", cmd, மதிப்பு ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    வெறுமை
clear_termoptions(வெறுமை)
{
    // Reset a few things before clearing the old options. This may cause
    // outputting a few things that the terminal doesn't understand, but the
    // screen will be cleared later, so this is OK.
    mch_setmouse(FALSE);	    // switch mouse off
    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window titles
#எனில் defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    எனில் (gui.starting)
	clear_xterm_clip();
#endif
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    வெறுமை
free_termoptions(வெறுமை)
{
    கட்டமைப்பு vimoption   *p;

    க்கு (p = options; p->fullname != வெற்று; p++)
	எனில் (istermoption(p))
	{
	    எனில் (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    எனில் (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = empty_option;
	    p->def_val[VI_DEFAULT] = empty_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ifdef FEAT_EVAL
	    // remember where the option was cleared
	    set_option_sctx_idx((முழு)(p - options), OPT_GLOBAL, current_sctx);
#endif
	}
    clear_termcodes();
}

/*
 * Free the string for one term option, if it was allocated.
 * Set the string to empty_option and clear allocated flag.
 * "var" points to the option value.
 */
    வெறுமை
free_one_termoption(char_u *var)
{
    கட்டமைப்பு vimoption   *p;

    க்கு (p = &options[0]; p->fullname != வெற்று; p++)
	எனில் (p->var == var)
	{
	    எனில் (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = empty_option;
	    p->flags &= ~P_ALLOCED;
	    நிறுத்து;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    வெறுமை
set_term_defaults(வெறுமை)
{
    கட்டமைப்பு vimoption   *p;

    க்கு (p = &options[0]; p->fullname != வெற்று; p++)
    {
	எனில் (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    எனில் (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    எனில் (p->flags & P_ALLOCED)
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
    நிலையான முழு
istermoption(கட்டமைப்பு vimoption *p)
{
    திரும்பு (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    நிலையான முழு
istermoption_idx(முழு opt_idx)
{
    திரும்பு istermoption(&options[opt_idx]);
}

#எனில் defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    வெறுமை
unset_global_local_option(char_u *name, வெறுமை *from)
{
    கட்டமைப்பு vimoption *p;
    முழு		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    எனில் (opt_idx < 0)
	திரும்பு;
    p = &(options[opt_idx]);

    தேர்வு ((முழு)p->indir)
    {
	// global option with local value: use local value if it's been set
	நிலை PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    நிறுத்து;
	நிலை PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    நிறுத்து;
	நிலை PV_PATH:
	    clear_string_option(&buf->b_p_path);
	    நிறுத்து;
	நிலை PV_AC:
	    buf->b_p_ac = -1;
	    நிறுத்து;
	நிலை PV_AR:
	    buf->b_p_ar = -1;
	    நிறுத்து;
# ifdef HAVE_FSYNC
	நிலை PV_FS:
	    buf->b_p_fs = -1;
	    நிறுத்து;
# endif
	நிலை PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    நிறுத்து;
	நிலை PV_TAGS:
	    clear_string_option(&buf->b_p_tags);
	    நிறுத்து;
	நிலை PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    நிறுத்து;
	நிலை PV_SISO:
	    curwin->w_p_siso = -1;
	    நிறுத்து;
	நிலை PV_SO:
	    curwin->w_p_so = -1;
	    நிறுத்து;
	நிலை PV_SOP:
	    curwin->w_p_sop = -1;
	    நிறுத்து;
# ifdef FEAT_FIND_ID
	நிலை PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    நிறுத்து;
	நிலை PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    நிறுத்து;
# endif
	நிலை PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    நிறுத்து;
	நிலை PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    நிறுத்து;
# ifdef FEAT_DIFF
	நிலை PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    நிறுத்து;
# endif
	நிலை PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    நிறுத்து;
# ifdef FEAT_COMPL_FUNC
	நிலை PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    நிறுத்து;
# endif
	நிலை PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    நிறுத்து;
# ifdef FEAT_EVAL
	நிலை PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    நிறுத்து;
# endif
# ifdef FEAT_QUICKFIX
	நிலை PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    நிறுத்து;
	நிலை PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    நிறுத்து;
	நிலை PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    நிறுத்து;
	நிலை PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    நிறுத்து;
# endif
# எனில் defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	நிலை PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    நிறுத்து;
# endif
# எனில் defined(FEAT_CRYPT)
	நிலை PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    நிறுத்து;
# endif
# ifdef FEAT_LINEBREAK
	நிலை PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    நிறுத்து;
# endif
# ifdef FEAT_STL_OPT
	நிலை PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    நிறுத்து;
	நிலை PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    நிறுத்து;
# endif
	நிலை PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    நிறுத்து;
	நிலை PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    நிறுத்து;
	நிலை PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    நிறுத்து;
	நிலை PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    set_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      வெற்று, 0);
	    redraw_later(UPD_NOT_VALID);
	    நிறுத்து;
	நிலை PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    set_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      வெற்று, 0);
	    redraw_later(UPD_NOT_VALID);
	    நிறுத்து;
	நிலை PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    நிறுத்து;
    }
}
#endif

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    நிலையான char_u *
get_varp_scope(கட்டமைப்பு vimoption *p, முழு scope)
{
    எனில் ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	எனில் (p->var == VAR_WIN)
	    திரும்பு (char_u *)GLOBAL_WO(get_varp(p));
	திரும்பு p->var;
    }
    எனில் ((scope & OPT_LOCAL) && ((முழு)p->indir & PV_BOTH))
    {
	தேர்வு ((முழு)p->indir)
	{
	    நிலை PV_FP:   திரும்பு (char_u *)&(curbuf->b_p_fp);
#ifdef HAVE_FSYNC
	    நிலை PV_FS:	திரும்பு (char_u *)&(curbuf->b_p_fs);
#endif
#ifdef FEAT_EVAL
	    நிலை PV_FFU: திரும்பு (char_u *)&(curbuf->b_p_ffu);
#endif
#ifdef FEAT_QUICKFIX
	    நிலை PV_EFM:  திரும்பு (char_u *)&(curbuf->b_p_efm);
	    நிலை PV_GEFM: திரும்பு (char_u *)&(curbuf->b_p_gefm);
	    நிலை PV_GP:   திரும்பு (char_u *)&(curbuf->b_p_gp);
	    நிலை PV_MP:   திரும்பு (char_u *)&(curbuf->b_p_mp);
#endif
	    நிலை PV_EP:   திரும்பு (char_u *)&(curbuf->b_p_ep);
	    நிலை PV_KP:   திரும்பு (char_u *)&(curbuf->b_p_kp);
	    நிலை PV_PATH: திரும்பு (char_u *)&(curbuf->b_p_path);
	    நிலை PV_AC:   திரும்பு (char_u *)&(curbuf->b_p_ac);
	    நிலை PV_AR:   திரும்பு (char_u *)&(curbuf->b_p_ar);
	    நிலை PV_TAGS: திரும்பு (char_u *)&(curbuf->b_p_tags);
	    நிலை PV_TC:   திரும்பு (char_u *)&(curbuf->b_p_tc);
	    நிலை PV_SISO: திரும்பு (char_u *)&(curwin->w_p_siso);
	    நிலை PV_SO:   திரும்பு (char_u *)&(curwin->w_p_so);
	    நிலை PV_SOP:  திரும்பு (char_u *)&(curwin->w_p_sop);
#ifdef FEAT_FIND_ID
	    நிலை PV_DEF:  திரும்பு (char_u *)&(curbuf->b_p_def);
	    நிலை PV_INC:  திரும்பு (char_u *)&(curbuf->b_p_inc);
#endif
	    நிலை PV_COT:  திரும்பு (char_u *)&(curbuf->b_p_cot);
	    நிலை PV_DICT: திரும்பு (char_u *)&(curbuf->b_p_dict);
#ifdef FEAT_DIFF
	    நிலை PV_DIA:  திரும்பு (char_u *)&(curbuf->b_p_dia);
#endif
	    நிலை PV_TSR:  திரும்பு (char_u *)&(curbuf->b_p_tsr);
#ifdef FEAT_COMPL_FUNC
	    நிலை PV_TSRFU: திரும்பு (char_u *)&(curbuf->b_p_tsrfu);
#endif
#எனில் defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    நிலை PV_BEXPR: திரும்பு (char_u *)&(curbuf->b_p_bexpr);
#endif
#எனில் defined(FEAT_CRYPT)
	    நிலை PV_CM:	  திரும்பு (char_u *)&(curbuf->b_p_cm);
#endif
#ifdef FEAT_LINEBREAK
	    நிலை PV_SBR:  திரும்பு (char_u *)&(curwin->w_p_sbr);
#endif
#ifdef FEAT_STL_OPT
	    நிலை PV_STL:  திரும்பு (char_u *)&(curwin->w_p_stl);
	    நிலை PV_STLO: திரும்பு (char_u *)&(curwin->w_p_stlo);
#endif
	    நிலை PV_UL:   திரும்பு (char_u *)&(curbuf->b_p_ul);
	    நிலை PV_LW:   திரும்பு (char_u *)&(curbuf->b_p_lw);
	    நிலை PV_BKC:  திரும்பு (char_u *)&(curbuf->b_p_bkc);
	    நிலை PV_MENC: திரும்பு (char_u *)&(curbuf->b_p_menc);
	    நிலை PV_LCS:  திரும்பு (char_u *)&(curwin->w_p_lcs);
	    நிலை PV_FCS:  திரும்பு (char_u *)&(curwin->w_p_fcs);
	    நிலை PV_VE:	  திரும்பு (char_u *)&(curwin->w_p_ve);

	}
	திரும்பு வெற்று; // "cannot happen"
    }
    திரும்பு get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(முழு opt_idx, முழு scope)
{
    திரும்பு get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    நிலையான char_u *
get_varp(கட்டமைப்பு vimoption *p)
{
    // hidden option, always return NULL
    எனில் (p->var == வெற்று)
	திரும்பு வெற்று;

    தேர்வு ((முழு)p->indir)
    {
	நிலை PV_NONE:	திரும்பு p->var;

	// global option with local value: use local value if it's been set
	நிலை PV_EP:	திரும்பு *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	நிலை PV_KP:	திரும்பு *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	நிலை PV_PATH:	திரும்பு *curbuf->b_p_path != NUL
				    ? (char_u *)&(curbuf->b_p_path) : p->var;
	நிலை PV_AC:	திரும்பு curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	நிலை PV_AR:	திரும்பு curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	நிலை PV_TAGS:	திரும்பு *curbuf->b_p_tags != NUL
				    ? (char_u *)&(curbuf->b_p_tags) : p->var;
	நிலை PV_TC:	திரும்பு *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	நிலை PV_BKC:	திரும்பு *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	நிலை PV_SISO:	திரும்பு curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	நிலை PV_SO:	திரும்பு curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	நிலை PV_SOP:	திரும்பு curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ifdef FEAT_FIND_ID
	நிலை PV_DEF:	திரும்பு *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	நிலை PV_INC:	திரும்பு *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#endif
	நிலை PV_COT:	திரும்பு *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	நிலை PV_DICT:	திரும்பு *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ifdef FEAT_DIFF
	நிலை PV_DIA:	திரும்பு *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#endif
	நிலை PV_TSR:	திரும்பு *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ifdef FEAT_COMPL_FUNC
	நிலை PV_TSRFU:	திரும்பு *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#endif
	நிலை PV_FP:	திரும்பு *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ifdef HAVE_FSYNC
	நிலை PV_FS:	திரும்பு curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#endif
#ifdef FEAT_EVAL
	நிலை PV_FFU:	திரும்பு *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#endif
#ifdef FEAT_QUICKFIX
	நிலை PV_EFM:	திரும்பு *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	நிலை PV_GEFM:	திரும்பு *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	நிலை PV_GP:	திரும்பு *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	நிலை PV_MP:	திரும்பு *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#endif
#எனில் defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	நிலை PV_BEXPR:	திரும்பு *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#endif
#எனில் defined(FEAT_CRYPT)
	நிலை PV_CM:	திரும்பு *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#endif
#ifdef FEAT_LINEBREAK
	நிலை PV_SBR:	திரும்பு *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#endif
#ifdef FEAT_STL_OPT
	நிலை PV_STL:	திரும்பு *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	நிலை PV_STLO:	திரும்பு *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#endif
	நிலை PV_UL:	திரும்பு curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	நிலை PV_LW:	திரும்பு *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	நிலை PV_MENC:	திரும்பு *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ifdef FEAT_ARABIC
	நிலை PV_ARAB:	திரும்பு (char_u *)&(curwin->w_p_arab);
#endif
	நிலை PV_LIST:	திரும்பு (char_u *)&(curwin->w_p_list);
	நிலை PV_LCS:	திரும்பு *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	நிலை PV_FCS:	திரும்பு *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	நிலை PV_VE:	திரும்பு *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ifdef FEAT_SPELL
	நிலை PV_SPELL:	திரும்பு (char_u *)&(curwin->w_p_spell);
#endif
#ifdef FEAT_SYN_HL
	நிலை PV_CUC:	திரும்பு (char_u *)&(curwin->w_p_cuc);
	நிலை PV_CUL:	திரும்பு (char_u *)&(curwin->w_p_cul);
	நிலை PV_CULOPT:	திரும்பு (char_u *)&(curwin->w_p_culopt);
	நிலை PV_CC:	திரும்பு (char_u *)&(curwin->w_p_cc);
#endif
#ifdef FEAT_DIFF
	நிலை PV_DIFF:	திரும்பு (char_u *)&(curwin->w_p_diff);
#endif
	நிலை PV_EIW:	திரும்பு (char_u *)&(curwin->w_p_eiw);
#ifdef FEAT_FOLDING
	நிலை PV_FDC:	திரும்பு (char_u *)&(curwin->w_p_fdc);
	நிலை PV_FEN:	திரும்பு (char_u *)&(curwin->w_p_fen);
	நிலை PV_FDI:	திரும்பு (char_u *)&(curwin->w_p_fdi);
	நிலை PV_FDL:	திரும்பு (char_u *)&(curwin->w_p_fdl);
	நிலை PV_FDM:	திரும்பு (char_u *)&(curwin->w_p_fdm);
	நிலை PV_FML:	திரும்பு (char_u *)&(curwin->w_p_fml);
	நிலை PV_FDN:	திரும்பு (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	நிலை PV_FDE:	திரும்பு (char_u *)&(curwin->w_p_fde);
	நிலை PV_FDT:	திரும்பு (char_u *)&(curwin->w_p_fdt);
# endif
	நிலை PV_FMR:	திரும்பு (char_u *)&(curwin->w_p_fmr);
#endif
	நிலை PV_NU:	திரும்பு (char_u *)&(curwin->w_p_nu);
	நிலை PV_RNU:	திரும்பு (char_u *)&(curwin->w_p_rnu);
#ifdef FEAT_LINEBREAK
	நிலை PV_NUW:	திரும்பு (char_u *)&(curwin->w_p_nuw);
#endif
	நிலை PV_WFB:	திரும்பு (char_u *)&(curwin->w_p_wfb);
	நிலை PV_WFH:	திரும்பு (char_u *)&(curwin->w_p_wfh);
	நிலை PV_WFW:	திரும்பு (char_u *)&(curwin->w_p_wfw);
#எனில் defined(FEAT_QUICKFIX)
	நிலை PV_PVW:	திரும்பு (char_u *)&(curwin->w_p_pvw);
	நிலை PV_LHI:	திரும்பு (char_u *)&(curwin->w_p_lhi);
#endif
#ifdef FEAT_RIGHTLEFT
	நிலை PV_RL:	திரும்பு (char_u *)&(curwin->w_p_rl);
	நிலை PV_RLC:	திரும்பு (char_u *)&(curwin->w_p_rlc);
#endif
	நிலை PV_SCROLL:	திரும்பு (char_u *)&(curwin->w_p_scr);
	நிலை PV_SMS:	திரும்பு (char_u *)&(curwin->w_p_sms);
	நிலை PV_WRAP:	திரும்பு (char_u *)&(curwin->w_p_wrap);
#ifdef FEAT_LINEBREAK
	நிலை PV_LBR:	திரும்பு (char_u *)&(curwin->w_p_lbr);
	நிலை PV_BRI:	திரும்பு (char_u *)&(curwin->w_p_bri);
	நிலை PV_BRIOPT: திரும்பு (char_u *)&(curwin->w_p_briopt);
#endif
	நிலை PV_WCR:	திரும்பு (char_u *)&(curwin->w_p_wcr);
	நிலை PV_SCBIND: திரும்பு (char_u *)&(curwin->w_p_scb);
	நிலை PV_CRBIND: திரும்பு (char_u *)&(curwin->w_p_crb);
#ifdef FEAT_CONCEAL
	நிலை PV_COCU:   திரும்பு (char_u *)&(curwin->w_p_cocu);
	நிலை PV_COLE:   திரும்பு (char_u *)&(curwin->w_p_cole);
#endif
	நிலை PV_WHL:	திரும்பு (char_u *)&(curwin->w_p_whl);

#ifdef FEAT_TERMINAL
	நிலை PV_TWK:    திரும்பு (char_u *)&(curwin->w_p_twk);
	நிலை PV_TWS:    திரும்பு (char_u *)&(curwin->w_p_tws);
	நிலை PV_TWSL:	திரும்பு (char_u *)&(curbuf->b_p_twsl);
#endif

	நிலை PV_AI:	திரும்பு (char_u *)&(curbuf->b_p_ai);
	நிலை PV_BIN:	திரும்பு (char_u *)&(curbuf->b_p_bin);
	நிலை PV_BOMB:	திரும்பு (char_u *)&(curbuf->b_p_bomb);
	நிலை PV_BH:	திரும்பு (char_u *)&(curbuf->b_p_bh);
	நிலை PV_BT:	திரும்பு (char_u *)&(curbuf->b_p_bt);
	நிலை PV_BL:	திரும்பு (char_u *)&(curbuf->b_p_bl);
	நிலை PV_CI:	திரும்பு (char_u *)&(curbuf->b_p_ci);
	நிலை PV_CIN:	திரும்பு (char_u *)&(curbuf->b_p_cin);
	நிலை PV_CINK:	திரும்பு (char_u *)&(curbuf->b_p_cink);
	நிலை PV_CINO:	திரும்பு (char_u *)&(curbuf->b_p_cino);
	நிலை PV_CINSD:	திரும்பு (char_u *)&(curbuf->b_p_cinsd);
	நிலை PV_CINW:	திரும்பு (char_u *)&(curbuf->b_p_cinw);
	நிலை PV_COM:	திரும்பு (char_u *)&(curbuf->b_p_com);
	நிலை PV_CMS:	திரும்பு (char_u *)&(curbuf->b_p_cms);
	நிலை PV_CPT:	திரும்பு (char_u *)&(curbuf->b_p_cpt);
#ifdef BACKSLASH_IN_FILENAME
	நிலை PV_CSL:	திரும்பு (char_u *)&(curbuf->b_p_csl);
#endif
#ifdef FEAT_COMPL_FUNC
	நிலை PV_CFU:	திரும்பு (char_u *)&(curbuf->b_p_cfu);
	நிலை PV_OFU:	திரும்பு (char_u *)&(curbuf->b_p_ofu);
#endif
#ifdef FEAT_EVAL
	நிலை PV_TFU:	திரும்பு (char_u *)&(curbuf->b_p_tfu);
#endif
	நிலை PV_EOF:	திரும்பு (char_u *)&(curbuf->b_p_eof);
	நிலை PV_EOL:	திரும்பு (char_u *)&(curbuf->b_p_eol);
	நிலை PV_FIXEOL:	திரும்பு (char_u *)&(curbuf->b_p_fixeol);
	நிலை PV_ET:	திரும்பு (char_u *)&(curbuf->b_p_et);
	நிலை PV_FENC:	திரும்பு (char_u *)&(curbuf->b_p_fenc);
	நிலை PV_FF:	திரும்பு (char_u *)&(curbuf->b_p_ff);
	நிலை PV_FT:	திரும்பு (char_u *)&(curbuf->b_p_ft);
	நிலை PV_FO:	திரும்பு (char_u *)&(curbuf->b_p_fo);
	நிலை PV_FLP:	திரும்பு (char_u *)&(curbuf->b_p_flp);
	நிலை PV_IMI:	திரும்பு (char_u *)&(curbuf->b_p_iminsert);
	நிலை PV_IMS:	திரும்பு (char_u *)&(curbuf->b_p_imsearch);
	நிலை PV_INF:	திரும்பு (char_u *)&(curbuf->b_p_inf);
	நிலை PV_ISK:	திரும்பு (char_u *)&(curbuf->b_p_isk);
#ifdef FEAT_FIND_ID
# ifdef FEAT_EVAL
	நிலை PV_INEX:	திரும்பு (char_u *)&(curbuf->b_p_inex);
# endif
#endif
#எனில் defined(FEAT_EVAL)
	நிலை PV_INDE:	திரும்பு (char_u *)&(curbuf->b_p_inde);
	நிலை PV_INDK:	திரும்பு (char_u *)&(curbuf->b_p_indk);
#endif
#ifdef FEAT_EVAL
	நிலை PV_FEX:	திரும்பு (char_u *)&(curbuf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
	நிலை PV_சாவி:	திரும்பு (char_u *)&(curbuf->b_p_சாவி);
#endif
	நிலை PV_LISP:	திரும்பு (char_u *)&(curbuf->b_p_lisp);
	நிலை PV_LOP:	திரும்பு (char_u *)&(curbuf->b_p_lop);
	நிலை PV_ML:	திரும்பு (char_u *)&(curbuf->b_p_ml);
	நிலை PV_MPS:	திரும்பு (char_u *)&(curbuf->b_p_mps);
	நிலை PV_MA:	திரும்பு (char_u *)&(curbuf->b_p_ma);
	நிலை PV_MOD:	திரும்பு (char_u *)&(curbuf->b_changed);
	நிலை PV_NF:	திரும்பு (char_u *)&(curbuf->b_p_nf);
	நிலை PV_PI:	திரும்பு (char_u *)&(curbuf->b_p_pi);
	நிலை PV_QE:	திரும்பு (char_u *)&(curbuf->b_p_qe);
	நிலை PV_RO:	திரும்பு (char_u *)&(curbuf->b_p_ro);
	நிலை PV_SI:	திரும்பு (char_u *)&(curbuf->b_p_si);
	நிலை PV_SN:	திரும்பு (char_u *)&(curbuf->b_p_sn);
	நிலை PV_STS:	திரும்பு (char_u *)&(curbuf->b_p_sts);
	நிலை PV_SUA:	திரும்பு (char_u *)&(curbuf->b_p_sua);
	நிலை PV_SWF:	திரும்பு (char_u *)&(curbuf->b_p_swf);
#ifdef FEAT_SYN_HL
	நிலை PV_SMC:	திரும்பு (char_u *)&(curbuf->b_p_smc);
	நிலை PV_SYN:	திரும்பு (char_u *)&(curbuf->b_p_syn);
#endif
#ifdef FEAT_SPELL
	நிலை PV_SPC:	திரும்பு (char_u *)&(curwin->w_s->b_p_spc);
	நிலை PV_SPF:	திரும்பு (char_u *)&(curwin->w_s->b_p_spf);
	நிலை PV_SPL:	திரும்பு (char_u *)&(curwin->w_s->b_p_spl);
	நிலை PV_SPO:	திரும்பு (char_u *)&(curwin->w_s->b_p_spo);
#endif
	நிலை PV_SW:	திரும்பு (char_u *)&(curbuf->b_p_sw);
	நிலை PV_TS:	திரும்பு (char_u *)&(curbuf->b_p_ts);
	நிலை PV_TW:	திரும்பு (char_u *)&(curbuf->b_p_tw);
	நிலை PV_TX:	திரும்பு (char_u *)&(curbuf->b_p_tx);
#ifdef FEAT_PERSISTENT_UNDO
	நிலை PV_UDF:	திரும்பு (char_u *)&(curbuf->b_p_udf);
#endif
	நிலை PV_WM:	திரும்பு (char_u *)&(curbuf->b_p_wm);
#ifdef FEAT_KEYMAP
	நிலை PV_KMAP:	திரும்பு (char_u *)&(curbuf->b_p_keymap);
#endif
#ifdef FEAT_SIGNS
	நிலை PV_SCL:	திரும்பு (char_u *)&(curwin->w_p_scl);
#endif
#ifdef FEAT_VARTABS
	நிலை PV_VSTS:	திரும்பு (char_u *)&(curbuf->b_p_vsts);
	நிலை PV_VTS:	திரும்பு (char_u *)&(curbuf->b_p_vts);
#endif
	இயல்பு:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    திரும்பு (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(முழு opt_idx)
{
    திரும்பு options[opt_idx].var;
}

#எனில் defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(முழு opt_idx)
{
    திரும்பு (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    முழு
option_info(char_u *name, dict_T *d)
{
    முழு			opt_idx;
    கட்டமைப்பு vimoption	*p;
    முழு			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    எனில் ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    எனில் (opt_idx < 0)
	திரும்பு FAIL;

    p = &options[opt_idx];
    dict_add_string(d, "name", (char_u *)p->fullname);
    dict_add_string(d, "shortname",
		    (char_u *)(p->shortname == வெற்று ? "" : p->shortname));
    dict_add_bool(d, "available", p->var != வெற்று);
    dict_add_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_இலக்கம்) ? "number" : "string"));
    dict_add_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    எனில் (p->flags & P_BOOL)
	dict_add_bool(d, "default", (முழு)(long_i)p->def_val[dvi]);
    இல்லையெனில் எனில் (p->flags & P_இலக்கம்)
	dict_add_எண்(d, "default", (long)(long_i)p->def_val[dvi]);
    இல்லையெனில்
	dict_add_string(d, "default", p->def_val[dvi] == வெற்று
					? (char_u *)"" : p->def_val[dvi]);
    திரும்பு OK;
}
#endif

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_set_cb_T
get_option_did_set_cb(முழு opt_idx)
{
    திரும்பு options[opt_idx].opt_did_set_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(வெறுமை)
{
    எனில் (*curbuf->b_p_ep == NUL)
	திரும்பு p_ep;
    திரும்பு curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(வெறுமை)
{
#ifdef FEAT_EVAL
    எனில் (*curbuf->b_p_ffu == NUL)
	திரும்பு p_ffu;
    திரும்பு curbuf->b_p_ffu;
#இல்லையெனில்
    திரும்பு (char_u *)"";
#endif
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    வெறுமை
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
    வெறுமை
after_copy_winopt(win_T *wp)
{
    எழுத்து *errmsg = update_winhighlight(wp, wp->w_p_whl);

    எனில் (errmsg != வெற்று)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    எனில் (wp->w_p_wrap)
	wp->w_leftcol = 0;
    இல்லையெனில்
	wp->w_skipcol = 0;
#ifdef FEAT_LINEBREAK
    briopt_check(வெற்று, wp);
#endif
#ifdef FEAT_SYN_HL
    fill_culopt_flags(வெற்று, wp);
    check_colorcolumn(வெற்று, wp);
#endif
    set_listchars_option(wp, wp->w_p_lcs, TRUE, வெற்று, 0);
    set_fillchars_option(wp, wp->w_p_fcs, TRUE, வெற்று, 0);
}

    நிலையான char_u *
copy_option_val(char_u *val)
{
    எனில் (val == empty_option)
	திரும்பு empty_option;  // no need to allocate memory
    திரும்பு vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    வெறுமை
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
						    அளவு(to->wo_script_ctx));
#endif
    check_winopt(to);		// don't want NULL pointers
}

/*
 * Check string options in a window for a NULL value.
 */
    நிலையான வெறுமை
check_win_options(win_T *win)
{
    check_winopt(&win->w_onebuf_opt);
    check_winopt(&win->w_allbuf_opt);
}

/*
 * Check for NULL pointers in a winopt_T and replace them with empty_option.
 */
    நிலையான வெறுமை
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
    வெறுமை
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
நிலையான முழு buf_opt_idx[BV_மொத்தம்];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    நிலையான வெறுமை
init_buf_opt_idx(வெறுமை)
{
    நிலையான முழு did_init_buf_opt_idx = FALSE;
    முழு i;

    எனில் (did_init_buf_opt_idx)
	திரும்பு;
    did_init_buf_opt_idx = TRUE;
    க்கு (i = 0; !istermoption_idx(i); i++)
	எனில் (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_MASK] = i;
}
#இல்லையெனில்
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
    வெறுமை
buf_copy_options(buf_T *buf, முழு flags)
{
    முழு		should_copy = TRUE;
    char_u	*save_p_isk = வெற்று;	    // init for GCC
    முழு		dont_do_help;
    முழு		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    எனில் (p_cpo != வெற்று)
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
	எனில் ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == வெற்று || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != வெற்று)))
	    should_copy = FALSE;

	எனில் (should_copy || (flags & BCO_ALWAYS))
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
	    எனில் (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = வெற்று;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    எனில் (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		தேர்வு (*p_ffs)
		{
		    நிலை 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); நிறுத்து;
		    நிலை 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); நிறுத்து;
		    நிலை 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); நிறுத்து;
		    இயல்பு:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		எனில் (buf->b_p_ff != வெற்று)
		    buf->b_start_ffc = *buf->b_p_ff;
		buf->b_p_bh = empty_option;
		buf->b_p_bt = empty_option;
	    }
	    இல்லையெனில்
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
	    எனில் (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    இல்லையெனில்
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
	    எனில் (p_vsts && p_vsts != empty_option)
		(வெறுமை)tabstop_set(p_vsts, &buf->b_p_vsts_array);
	    இல்லையெனில்
		buf->b_p_vsts_array = வெற்று;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : வெற்று;
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
	    (வெறுமை)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#endif
#எனில் defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#endif
	    buf->b_p_fp = empty_option;
#எனில் defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#endif
#ifdef FEAT_CRYPT
	    buf->b_p_சாவி = vim_strsave(p_சாவி);
	    COPY_OPT_SCTX(buf, BV_சாவி);
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
#எனில் defined(FEAT_EVAL)
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
#எனில் defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = empty_option;
#endif
#எனில் defined(FEAT_CRYPT)
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
	    எனில் (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ifdef FEAT_VARTABS
		எனில் (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (வெறுமை)tabstop_set(p_vts, &buf->b_p_vts_array);
		இல்லையெனில்
		    buf->b_p_vts_array = வெற்று;
#endif
	    }
	    இல்லையெனில்
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#ifdef FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		எனில் (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (வெறுமை)tabstop_set(p_vts, &buf->b_p_vts_array);
		இல்லையெனில்
		    buf->b_p_vts_array = வெற்று;
#endif
		buf->b_help = false;
		எனில் (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	எனில் (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    எனில் (did_isk)
	(வெறுமை)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    வெறுமை
reset_modifiable(வெறுமை)
{
    முழு		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    எனில் (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}

/*
 * Set the global value for 'iminsert' to the local value.
 */
    வெறுமை
set_iminsert_global(வெறுமை)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    வெறுமை
set_imsearch_global(வெறுமை)
{
    p_imsearch = curbuf->b_p_imsearch;
}

நிலையான முழு expand_option_idx = -1;
நிலையான முழு expand_option_start_col = 0;
நிலையான char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
நிலையான முழு expand_option_flags = 0;
நிலையான முழு expand_option_append = FALSE;

    வெறுமை
set_context_in_set_cmd(
    expand_T	*xp,
    char_u	*arg,
    முழு		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    முழு		nextchar;
    long_u	flags = 0;	// init for GCC
    முழு		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    முழு		is_term_option = FALSE;
    முழு		சாவி;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    எனில் (*arg == NUL)
    {
	xp->xp_pattern = arg;
	திரும்பு;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    எனில் (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_pattern = p + 1;
	திரும்பு;
    }
    போது (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	எனில் (*p == ' ' || *p == ',')
	{
	    போது (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	எனில் (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    நிறுத்து;
	}
	--p;
    }
    எனில் (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_NO;
	p += 2;
    }
    இல்லையெனில் எனில் (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_INV;
	p += 3;
    }
    xp->xp_pattern = arg = p;
    எனில் (*arg == '<')
    {
	போது (*p != '>')
	    எனில் (*p++ == NUL)	    // expand terminal option name
		திரும்பு;
	சாவி = get_special_சாவி_code(arg + 1);
	எனில் (சாவி == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    திரும்பு;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(சாவி);
	expand_option_name[3] = KEY2TERMCAP1(சாவி);
    }
    இல்லையெனில்
    {
	எனில் (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    எனில் (*p != NUL)
		++p;
	    எனில் (*p == NUL)
		திரும்பு;		// expand option name
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	இல்லையெனில்
	{
	    // Allow * wildcard
	    போது (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    எனில் (*p == NUL)
		திரும்பு;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    எனில் (opt_idx == -1 || options[opt_idx].var == வெற்று)
	    {
		xp->xp_context = EXPAND_NOTHING;
		திரும்பு;
	    }
	    flags = options[opt_idx].flags;
	    எனில் (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		திரும்பு;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    முழு expand_option_subtract = FALSE;
    எனில் ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	எனில் (nextchar == '-')
	    expand_option_subtract = TRUE;
	எனில் (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    எனில் ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	திரும்பு;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    எனில் (is_term_option)
	expand_option_idx = -1;
    இல்லையெனில்
	expand_option_idx = opt_idx;

    எனில் (!is_term_option)
    {
	எனில் (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    திரும்பு;
	}
    }

    xp->xp_pattern = p + 1;
    expand_option_start_col = (முழு)(p + 1 - xp->xp_வரி);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ifdef FEAT_SYN_HL
    எனில் (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	திரும்பு;
    }
#endif
    எனில் (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	திரும்பு;
    }
#ifdef FEAT_KEYMAP
    எனில் (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	திரும்பு;
    }
#endif

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    எனில் (expand_option_subtract)
    {
	xp->xp_context = EXPAND_SETTING_SUBTRACT;
	திரும்பு;
    }
    இல்லையெனில் எனில் (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != வெற்று)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    இல்லையெனில் எனில் (*xp->xp_pattern == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	திரும்பு;
    }
    இல்லையெனில்
	xp->xp_context = EXPAND_NOTHING;

    எனில் (is_term_option || (flags & P_இலக்கம்))
	திரும்பு;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    எனில் (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	எனில் (p == (char_u *)&p_bdir
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
	    எனில் (p == (char_u *)&p_path || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    இல்லையெனில்
		xp->xp_backslash = XP_BS_ONE;
	}
	இல்லையெனில்
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    எனில் (p == (char_u *)&p_tags)
		xp->xp_backslash = XP_BS_THREE;
	    இல்லையெனில்
		xp->xp_backslash = XP_BS_ONE;
	}
	எனில் (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    எனில் ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	க்கு (p = argend - 1; p >= xp->xp_pattern; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    எனில் (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		போது (s > xp->xp_pattern && *(s - 1) == '\\')
		    --s;
		எனில் ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#எனில் defined(BACKSLASH_IN_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#இல்லையெனில்
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#endif
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_pattern = p + 1;
		    நிறுத்து;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    எனில் (flags & P_FLAGLIST)
	xp->xp_pattern = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ifdef FEAT_SPELL
    எனில் (options[opt_idx].var == (char_u *)&p_sps)
    {
	எனில் (STRNCMP(xp->xp_pattern, "file:", 5) == 0)
	{
	    xp->xp_pattern += 5;
	    திரும்பு;
	}
	இல்லையெனில் எனில் (options[expand_option_idx].opt_expand_cb != வெற்று)
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
    நிலையான முழு
match_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	முழு		idx,
	முழு		test_only,
	முழு		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    எனில் (!fuzzy)
    {
	எனில் (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    எனில் (!test_only)
		matches[idx] = vim_strsave(str);
	    திரும்பு TRUE;
	}
    }
    இல்லையெனில்
    {
	முழு score;

	score = fuzzy_match_str(str, fuzzystr);
	எனில் (score != FUZZY_SCORE_NONE)
	{
	    எனில் (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    திரும்பு TRUE;
	}
    }

    திரும்பு FALSE;
}

    முழு
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    முழு		*இலக்கம்_Matches,
    char_u	***matches,
    முழு		can_fuzzy)
{
    முழு		இலக்கம்_normal = 0;	    // Nr of matching non-term-code settings
    முழு		இலக்கம்_term = 0;	    // Nr of matching terminal code settings
    முழு		opt_idx;
    முழு		match;
    முழு		மொத்தம் = 0;
    char_u	*str;
    முழு		loop;
    முழு		is_term_opt;
    char_u	name_buf[MAX_சாவி_NAME_LEN];
    நிலையான எழுத்து *(names[]) = {"all", "termcap"};
    முழு		ic = regmatch->rm_ic;	// remember the ignore-case flag
    முழு		fuzzy;
    fuzmatch_str_T  *fuzmatch = வெற்று;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    க்கு (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	எனில் (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    க்கு (match = 0; match < (முழு)ARRAY_LENGTH(names); ++match)
	    {
		எனில் (match_str((char_u *)names[match], regmatch, *matches,
			    மொத்தம், (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    எனில் (loop == 0)
			இலக்கம்_normal++;
		    இல்லையெனில்
			மொத்தம்++;
		}
	    }
	}
	க்கு (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != வெற்று;
								    opt_idx++)
	{
	    எனில் (options[opt_idx].var == வெற்று)
		தொடர்;
	    எனில் (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		தொடர்;
	    is_term_opt = istermoption_idx(opt_idx);
	    எனில் (is_term_opt && இலக்கம்_normal > 0)
		தொடர்;

	    எனில் (match_str(str, regmatch, *matches, மொத்தம், (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		எனில் (loop == 0)
		{
		    எனில் (is_term_opt)
			இலக்கம்_term++;
		    இல்லையெனில்
			இலக்கம்_normal++;
		}
		இல்லையெனில்
		    மொத்தம்++;
	    }
	    இல்லையெனில் எனில் (!fuzzy && options[opt_idx].shortname != வெற்று
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		எனில் (loop == 0)
		{
		    எனில் (is_term_opt)
			இலக்கம்_term++;
		    இல்லையெனில்
			இலக்கம்_normal++;
		}
		இல்லையெனில்
		    (*matches)[மொத்தம்++] = vim_strsave(str);
	    }
	    இல்லையெனில் எனில் (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		எனில் (match_str(name_buf, regmatch, *matches, மொத்தம், (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    எனில் (loop == 0)
			இலக்கம்_term++;
		    இல்லையெனில்
			மொத்தம்++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	எனில் (xp->xp_context != EXPAND_BOOL_SETTINGS  && இலக்கம்_normal == 0)
	{
	    க்கு (opt_idx = 0; (str = get_termcode(opt_idx)) != வெற்று; opt_idx++)
	    {
		எனில் (!SAFE_isprint(str[0]) || !SAFE_isprint(str[1]))
		    தொடர்;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		எனில் (match_str(name_buf, regmatch, *matches, மொத்தம்,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    எனில் (loop == 0)
			இலக்கம்_term++;
		    இல்லையெனில்
			மொத்தம்++;
		}
		இல்லையெனில்
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    எனில் (match_str(name_buf, regmatch, *matches, மொத்தம்,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			எனில் (loop == 0)
			    இலக்கம்_term++;
			இல்லையெனில்
			    மொத்தம்++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    க்கு (opt_idx = 0; (str = get_சாவி_name(opt_idx)) != வெற்று; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		எனில் (match_str(name_buf, regmatch, *matches, மொத்தம், (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    எனில் (loop == 0)
			இலக்கம்_term++;
		    இல்லையெனில்
			மொத்தம்++;
		}
	    }
	}
	எனில் (loop == 0)
	{
	    எனில் (இலக்கம்_normal > 0)
		*இலக்கம்_Matches = இலக்கம்_normal;
	    இல்லையெனில் எனில் (இலக்கம்_term > 0)
		*இலக்கம்_Matches = இலக்கம்_term;
	    இல்லையெனில்
		திரும்பு OK;
	    எனில் (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *இலக்கம்_Matches);
		எனில் (*matches == வெற்று)
		{
		    *matches = (char_u **)"";
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில்
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *இலக்கம்_Matches);
		எனில் (fuzmatch == வெற்று)
		{
		    *matches = (char_u **)"";
		    திரும்பு FAIL;
		}
	    }
	}
    }

    எனில் (fuzzy &&
	fuzzymatches_to_strmatches(fuzmatch, matches, மொத்தம், FALSE) == FAIL)
	திரும்பு FAIL;

    திரும்பு OK;
}

// Escape an option value that can be used on the command-line with :set.
// Caller needs to free the returned string, unless NULL is returned.
    நிலையான char_u*
escape_option_str_cmdline(char_u *var)
{
    char_u *buf;

    // A backslash is required before some characters.  This is the reverse of
    // what happens in do_set().
    buf = vim_strsave_escaped(var, escape_chars);
    எனில் (buf == வெற்று)
	திரும்பு வெற்று;

#ifdef BACKSLASH_IN_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    க்கு (var = buf; *var != NUL; MB_PTR_ADV(var))
	எனில் (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#endif
    திரும்பு buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    முழு
ExpandOldSetting(முழு *இலக்கம்_Matches, char_u ***matches)
{
    char_u  *var = வெற்று;	// init for GCC
    char_u  *buf;

    *இலக்கம்_Matches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    எனில் (*matches == வெற்று)
	திரும்பு FAIL;

    // For a terminal key code expand_option_idx is < 0.
    எனில் (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	எனில் (var == வெற்று)
	    expand_option_idx = findoption(expand_option_name);
    }

    எனில் (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    இல்லையெனில் எனில் (var == வெற்று)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    எனில் (buf == வெற்று)
    {
	VIM_CLEAR(*matches);
	திரும்பு FAIL;
    }

    (*matches)[0] = buf;
    *இலக்கம்_Matches = 1;
    திரும்பு OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    முழு
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    முழு		*இலக்கம்_Matches,
    char_u	***matches)
{
    char_u  *var = வெற்று;	// init for GCC
    char_u  *buf;

    எனில் (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == வெற்று)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	திரும்பு FAIL;
    }

    optexpand_T args;
    args.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    args.oe_append = expand_option_append;
    args.oe_regmatch = regmatch;
    args.oe_xp = xp;
    args.oe_set_arg = xp->xp_வரி + expand_option_start_col;
    args.oe_include_orig_val =
	!expand_option_append &&
	(*args.oe_set_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    எனில் (buf == வெற்று)
	திரும்பு FAIL;

    args.oe_opt_மதிப்பு = buf;

    முழு இலக்கம்_ret = options[expand_option_idx].opt_expand_cb(&args, இலக்கம்_Matches, matches);

    vim_free(buf);
    திரும்பு இலக்கம்_ret;
}

/*
 * Expansion handler for :set-=
 */
    முழு
ExpandSettingSubtract(
    expand_T	*xp,
    regmatch_T	*regmatch,
    முழு		*இலக்கம்_Matches,
    char_u	***matches)
{
    எனில் (expand_option_idx < 0)
	// term option
	திரும்பு ExpandOldSetting(இலக்கம்_Matches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    எனில் (option_flags & P_இலக்கம்)
	திரும்பு ExpandOldSetting(இலக்கம்_Matches, matches);
    இல்லையெனில் எனில் (option_flags & P_COMMA)
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

	எனில் (*option_val == NUL)
	    திரும்பு FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	எனில் (option_copy == வெற்று)
	    திரும்பு FAIL;
	next_val = option_copy;

	ga_init2(&ga, அளவு(char_u *), 10);

	செய்
	{
	    item = next_val;
	    comma = vim_strchr(next_val, ',');
	    போது (comma != வெற்று && comma != next_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    எனில் (comma != வெற்று)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		next_val = comma + 1;
	    }
	    இல்லையெனில்
		next_val = வெற்று;

	    எனில் (*item == NUL)
		// empty value, don't add to list
		தொடர்;

	    எனில் (!vim_regexec(regmatch, item, (colnr_T)0))
		தொடர்;

	    char_u *buf = escape_option_str_cmdline(item);
	    எனில் (buf == வெற்று)
	    {
		vim_free(option_copy);
		ga_clear_strings(&ga);
		திரும்பு FAIL;
	    }
	    எனில் (ga_add_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		நிறுத்து;
	    }
	} போது (next_val != வெற்று);

	vim_free(option_copy);

	*matches = ga.ga_data;
	*இலக்கம்_Matches = ga.ga_len;
	திரும்பு OK;
    }
    இல்லையெனில் எனில் (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	எனில் (*xp->xp_pattern != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    திரும்பு FAIL;
	}

	size_t இலக்கம்_flags = STRLEN(option_val);
	எனில் (இலக்கம்_flags == 0)
	    திரும்பு FAIL;

	*matches = ALLOC_MULT(char_u *, இலக்கம்_flags + 1);
	எனில் (*matches == வெற்று)
	    திரும்பு FAIL;

	முழு மொத்தம் = 0;
	char_u *p;

	p = vim_strnsave(option_val, இலக்கம்_flags);
	எனில் (p == வெற்று)
	{
	    VIM_CLEAR(*matches);
	    திரும்பு FAIL;
	}
	(*matches)[மொத்தம்++] = p;

	எனில் (இலக்கம்_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    க்கு (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		எனில் (p == வெற்று)
		    நிறுத்து;
		(*matches)[மொத்தம்++] = p;
	    }
	}

	*இலக்கம்_Matches = மொத்தம்;
	திரும்பு OK;
    }

    திரும்பு ExpandOldSetting(இலக்கம்_Matches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    நிலையான வெறுமை
option_value2string(
    கட்டமைப்பு vimoption	*opp,
    முழு			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    எனில் (opp->flags & P_இலக்கம்)
    {
	long wc = 0;

	எனில் (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_சாவி_name((முழு)wc, 0));
	இல்லையெனில் எனில் (wc != 0)
	    STRCPY(NameBuff, transchar((முழு)wc));
	இல்லையெனில்
	    sprintf((எழுத்து *)NameBuff, "%ld", *(long *)varp);
    }
    இல்லையெனில்    // P_STRING
    {
	varp = *(char_u **)(varp);
	எனில் (varp == வெற்று)		    // just in case
	    NameBuff[0] = NUL;
#ifdef FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	இல்லையெனில் எனில் (opp->var == (char_u *)&p_சாவி && *varp)
	    STRCPY(NameBuff, "*****");
#endif
	இல்லையெனில் எனில் (opp->flags & P_EXPAND)
	    home_replace(வெற்று, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	இல்லையெனில் எனில் ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	இல்லையெனில்
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    நிலையான முழு
wc_use_keyname(char_u *varp, long *wcp)
{
    எனில் (((long *)varp == &p_wc) || ((long *)varp == &p_wcm))
    {
	*wcp = *(long *)varp;
	எனில் (IS_SPECIAL(*wcp) || find_special_சாவி_in_table((முழு)*wcp) >= 0)
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    முழு
shortmess(முழு x)
{
    திரும்பு p_shm != வெற்று &&
	    (   vim_strchr(p_shm, x) != வெற்று
	    || (vim_strchr(p_shm, 'a') != வெற்று
		&& vim_strchr((char_u *)SHM_A, x) != வெற்று));
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 * When "fname" is not NULL, use it to set $"envname" when it wasn't set yet.
 */
    வெறுமை
vimrc_found(char_u *fname, char_u *envname)
{
    முழு		opt_idx;
    முழு		dofree = FALSE;
    char_u	*p;

    எனில் (!option_was_set((char_u *)"cp"))
    {
	p_cp = FALSE;
	க்கு (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    எனில் (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    எனில் (fname != வெற்று)
    {
	p = vim_getenv(envname, &dofree);
	எனில் (p == வெற்று)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = FullName_save(fname, FALSE);
	    எனில் (p != வெற்று)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	இல்லையெனில் எனில் (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    வெறுமை
change_compatible(முழு on)
{
    முழு	    opt_idx;

    எனில் (p_cp != on)
    {
	p_cp = on;
	compatible_set();
    }
    opt_idx = findoption((char_u *)"cp");
    எனில் (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_SET;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    முழு
option_was_set(char_u *name)
{
    முழு idx;

    idx = findoption(name);
    எனில் (idx < 0)	// unknown option
	திரும்பு FALSE;
    எனில் (options[idx].flags & P_WAS_SET)
	திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    முழு
reset_option_was_set(char_u *name)
{
    முழு idx = findoption(name);

    எனில் (idx < 0)
	திரும்பு FAIL;

    options[idx].flags &= ~P_WAS_SET;
    திரும்பு OK;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    நிலையான வெறுமை
compatible_set(வெறுமை)
{
    முழு	    opt_idx;

    க்கு (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	எனில் (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    set_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    முழு
can_bs(
    முழு		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ifdef FEAT_JOB_CHANNEL
    எனில் (what == BS_START && bt_prompt(curbuf))
	திரும்பு FALSE;
#endif
    தேர்வு (*p_bs)
    {
	நிலை '3':       திரும்பு TRUE;
	நிலை '2':	திரும்பு (what != BS_NOSTOP);
	நிலை '1':	திரும்பு (what != BS_START);
	நிலை '0':	திரும்பு FALSE;
    }
    திரும்பு vim_strchr(p_bs, what) != வெற்று;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloff_மதிப்பு(வெறுமை)
{
    திரும்பு curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloffpad_மதிப்பு(வெறுமை)
{
    திரும்பு curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_sidescrolloff_மதிப்பு(வெறுமை)
{
    திரும்பு curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    unsigned முழு
get_bkc_flags(buf_T *buf)
{
    திரும்பு buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#எனில் defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_மதிப்பு(buf_T *buf)
{
    எனில் (buf->b_p_flp == வெற்று || *buf->b_p_flp == NUL)
	திரும்பு p_flp;
    திரும்பு buf->b_p_flp;
}
#endif

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    unsigned முழு
get_ve_flags(வெறுமை)
{
    திரும்பு (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#எனில் defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_மதிப்பு(win_T *win)
{
    எனில் (win->w_p_sbr == வெற்று || *win->w_p_sbr == NUL)
	திரும்பு p_sbr;
    எனில் (STRCMP(win->w_p_sbr, "NONE") == 0)
	திரும்பு empty_option;
    திரும்பு win->w_p_sbr;
}
#endif

#எனில் defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(முழு bufopt)
{
    dict_T	*d;
    முழு		opt_idx;

    d = dict_alloc();
    எனில் (d == வெற்று)
	திரும்பு வெற்று;

    க்கு (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	கட்டமைப்பு vimoption *opt = &options[opt_idx];

	எனில் ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    எனில் (varp != வெற்று)
	    {
		எனில் (opt->flags & P_STRING)
		    dict_add_string(d, opt->fullname, *(char_u **)varp);
		இல்லையெனில் எனில் (opt->flags & P_இலக்கம்)
		    dict_add_எண்(d, opt->fullname, *(long *)varp);
		இல்லையெனில்
		    dict_add_எண்(d, opt->fullname, *(முழு *)varp);
	    }
	}
    }

    திரும்பு d;
}
#endif

#எனில் defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    முழு
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    எனில் (val == வெற்று)
	p = wp->w_p_culopt;
    இல்லையெனில்
	p = val;
    போது (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	எனில் (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_வரி;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_வரி | CULOPT_NBR;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	எனில் (*p != ',' && *p != NUL)
	    திரும்பு FAIL;
	எனில் (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    எனில் ((culopt_flags_new & CULOPT_வரி) && (culopt_flags_new & CULOPT_SCRLINE))
	திரும்பு FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    திரும்பு OK;
}
#endif

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    முழு
magic_isset(வெறுமை)
{
    தேர்வு (magic_overruled)
    {
	நிலை OPTION_MAGIC_ON:      திரும்பு TRUE;
	நிலை OPTION_MAGIC_OFF:     திரும்பு FALSE;
	நிலை OPTION_MAGIC_NOT_SET: நிறுத்து;
    }
#ifdef FEAT_EVAL
    எனில் (in_vim9script())
	திரும்பு TRUE;
#endif
    திரும்பு p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    முழு
option_set_callback_func(char_u *optval UNUSED, callback_T *optcb UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    முழு		funcname = FALSE;

    எனில் (optval == வெற்று || *optval == NUL)
    {
	free_callback(optcb);
	திரும்பு OK;
    }

    எனில் (*optval == '{' || (in_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_கோவை(optval, வெற்று);
    இல்லையெனில்
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    எனில் (tv == வெற்று)
	திரும்பு FAIL;

    cb = get_callback(tv);
    எனில் (cb.cb_name == வெற்று || *cb.cb_name == NUL)
    {
	free_tv(tv);
	திரும்பு FAIL;
    }

    free_callback(optcb);
    set_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = வெற்று;
    எனில் (in_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != வெற்று))
    {
	எனில் (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = find_imported(optval, dot - optval, FALSE);
	எனில் (import != வெற்று && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((எழுத்து *)fnamebuf, அளவு(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	இல்லையெனில்
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

    திரும்பு OK;
#இல்லையெனில்
    திரும்பு FAIL;
#endif
}

#எனில் defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    எழுத்து *
did_set_showtabpanel(optset_T *args UNUSED)
{
    shell_new_columns();
    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_EVAL)
    நிலையான வெறுமை
didset_options_sctx(முழு opt_flags, எழுத்து **buf)
{
	க்கு (முழு i = 0; ; ++i)
	{
	    எனில் (buf[i] == வெற்று)
		நிறுத்து;

	    முழு idx = findoption((char_u *)buf[i]);
	    எனில் (idx >= 0)
		set_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#endif
