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
#include "vim.h"
#include "optiondefs.h"

static void set_options_default(පූර්ණ_සංඛ්‍යා opt_flags);
static void set_string_default_esc(var_na *name, char_u *val, පූර්ණ_සංඛ්‍යා escape);
static char_u *find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
static char_u *option_expand(පූර්ණ_සංඛ්‍යා opt_idx, char_u *val);
static void didset_options(void);
static void didset_options2(void);
#නම් defined(FEAT_EVAL)
static long_u *insecure_flag(win_T *wp, පූර්ණ_සංඛ්‍යා opt_idx, පූර්ණ_සංඛ්‍යා opt_flags);
#නැතහොත්
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#endif
static var_na *set_bool_option(පූර්ණ_සංඛ්‍යා opt_idx, char_u *varp, පූර්ණ_සංඛ්‍යා value, පූර්ණ_සංඛ්‍යා opt_flags);
static var_na *set_num_option(පූර්ණ_සංඛ්‍යා opt_idx, char_u *varp, long value, var_na *errbuf, size_t errbuflen, පූර්ණ_සංඛ්‍යා opt_flags);
static පූර්ණ_සංඛ්‍යා find_key_option(char_u *arg_arg, පූර්ණ_සංඛ්‍යා has_lt);
static void showoptions(පූර්ණ_සංඛ්‍යා all, පූර්ණ_සංඛ්‍යා opt_flags);
static පූර්ණ_සංඛ්‍යා optval_default(sa_nracanaa vimoption *, char_u *varp, පූර්ණ_සංඛ්‍යා compatible);
static void showoneopt(sa_nracanaa vimoption *, පූර්ණ_සංඛ්‍යා opt_flags);
static පූර්ණ_සංඛ්‍යා put_setstring(FILE *fd, bool legacy, var_na *cmd, var_na *name, char_u **valuep, long_u flags);
static පූර්ණ_සංඛ්‍යා put_setnum(FILE *fd, var_na *cmd, var_na *name, long *valuep);
static පූර්ණ_සංඛ්‍යා put_setbool(FILE *fd, var_na *cmd, var_na *name, පූර්ණ_සංඛ්‍යා value);
static පූර්ණ_සංඛ්‍යා istermoption(sa_nracanaa vimoption *p);
static පූර්ණ_සංඛ්‍යා istermoption_idx(පූර්ණ_සංඛ්‍යා opt_idx);
static char_u *get_varp_scope(sa_nracanaa vimoption *p, පූර්ණ_සංඛ්‍යා scope);
static char_u *get_varp(sa_nracanaa vimoption *);
static void check_win_options(win_T *win);
static void option_value2string(sa_nracanaa vimoption *, පූර්ණ_සංඛ්‍යා scope);
static void check_winopt(winopt_T *wop);
static පූර්ණ_සංඛ්‍යා wc_use_keyname(char_u *varp, long *wcp);
static void compatible_set(void);

#නම් defined(FEAT_EVAL)
static var_na *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", NULL};
static var_na *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    NULL};
static void didset_options_sctx(පූර්ණ_සංඛ්‍යා opt_flags, var_na **buf);
#endif


/*
 * Initialize the 'shell' option to a default value.
 */
    static void
set_init_default_shell(void)
{
    char_u	*p;

    // Find default value for 'shell' option.
    // Don't use it if it is empty.
    නම් (((p = mch_getenv((char_u *)"SHELL")) != NULL && *p != NUL)
#නම් defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != NULL && *p != NUL)
	    || ((p = (char_u *)default_shell()) != NULL && *p != NUL)
#endif
	    )
#නම් defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	නම් (vim_strchr(p, ' ') != NULL)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    නම් (cmd != NULL)
	    {
		vim_snprintf((var_na *)cmd, len, "\"%s\"", p);
		set_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	නැතහොත්
	    set_string_default("sh", p);
    }
#නැතහොත්
	set_string_default_esc("sh", p, TRUE);
#endif
}

/*
 * Set the default for 'backupskip' to include environment variables for
 * temp files.
 */
    static void
set_init_default_backupskip(void)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;
    පූර්ණ_සංඛ්‍යා		i;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		plen;
#ifdef UNIX
    static var_na	*(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#නැතහොත්
    static var_na	*(names[3]) = {"TMPDIR", "TEMP", "TMP"};
#endif
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    සඳහා (i = 0; i < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(names); ++i)
    {
	පූර්ණ_සංඛ්‍යා		mustfree = FALSE;
#ifdef UNIX
	නම් (*names[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (පූර්ණ_සංඛ්‍යා)STRLEN_LITERAL("/private/tmp");
# නැතහොත්
	    p = (char_u *)"/tmp";
	    plen = (පූර්ණ_සංඛ්‍යා)STRLEN_LITERAL("/tmp");
# endif
	}
	නැතහොත්
#endif
	{
	    p = vim_getenv((char_u *)names[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	නම් (p != NULL && *p != NUL)
	{
	    char_u  *item;
	    size_t  itemsize;
	    පූර්ණ_සංඛ්‍යා	    has_trailing_path_sep = FALSE;

	    නම් (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (පූර්ණ_සංඛ්‍යා)STRLEN(p);
		// does the value include a trailing path separator?
		නම් (after_pathsep(p, p + plen))
		    has_trailing_path_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_path_sep ? 0 : 1) + 2;
	    item = alloc(itemsize);
	    නම් (item != NULL)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((var_na *)item, itemsize, "%s%s*", p, (has_trailing_path_sep) ? "" : PATHSEPSTR);

		නම් (find_dup_item(ga.ga_data, item, itemlen, options[opt_idx].flags) == NULL
			&& ga_grow(&ga, (පූර්ණ_සංඛ්‍යා)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((var_na *)ga.ga_data + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", item);
		}
		vim_free(item);
	    }
	}
	නම් (mustfree)
	    vim_free(p);
    }
    නම් (ga.ga_data != NULL)
    {
	set_string_default("bsk", ga.ga_data);
	vim_free(ga.ga_data);
    }
}

/*
 * Initialize the 'maxmemtot' and 'maxmem' options to a default value.
 * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory.
 */
    static void
set_init_default_maxmemtot(void)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    නම් (opt_idx < 0)
	ආපසු;

#නම් !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
    නම් (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#endif
    {
#නම් defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#elif defined(HAVE_TOTAL_MEM)
	// Use amount of memory available to Vim.
	n = (mch_total_mem(FALSE) >> 1);
#නැතහොත්
	n = (0x7fffffff >> 11);
#endif
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	නම් (opt_idx >= 0)
	{
#නම් !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
	    නම් ((long)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (long)n
		    || (long)(long_i)options[opt_idx].def_val[VI_DEFAULT] == 0L)
#endif
		options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	}
    }
}

/*
 * Initialize the 'cdpath' option to a default value.
 */
    static void
set_init_default_cdpath(void)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    පූර්ණ_සංඛ්‍යා	i;
    පූර්ණ_සංඛ්‍යා	j;
    පූර්ණ_සංඛ්‍යා	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    නම් (cdpath == NULL)
	ආපසු;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    නම් (buf != NULL)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	සඳහා (i = 0; cdpath[i] != NUL; ++i)
	{
	    නම් (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    නැතහොත්
	    {
		නම් (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	නම් (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	නැතහොත්
	    vim_free(buf); // cannot happen
    }
    නම් (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    static void
set_init_default_printencoding(void)
{
#නම් defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    set_string_default("penc",
# නම් defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# නැතහොත් // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#endif
}

#නම් defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    static void
set_init_default_printexpr(void)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    set_string_default("pexpr",
# නම් defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# නැතහොත්
	    (char_u *)"system('lpr' . (&printdevice == '' ? '' : ' -P' . &printdevice) . ' ' . v:fname_in) . delete(v:fname_in) + v:shell_error"
# endif
	    );
}
#endif

#ifdef UNIX
/*
 * Force restricted-mode on for "nologin" or "false" $SHELL
 */
    static void
set_init_restricted_mode(void)
{
    char_u	*p;

    p = get_isolated_shell_name();
    නම් (p == NULL)
	ආපසු;
    නම් (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
	restricted = TRUE;
    vim_free(p);
}
#endif

#ifdef CLEAN_RUNTIMEPATH
/*
 * When Vim is started with the "--clean" argument, set the default value
 * for the 'runtimepath' and 'packpath' options.
 */
    static void
set_init_clean_rtp(void)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    නම් (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    නම් (opt_idx < 0)
	ආපසු;
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
    static void
set_init_xdg_rtp(void)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;
    පූර්ණ_සංඛ්‍යා		has_xdg_env = TRUE;
    පූර්ණ_සංඛ්‍යා		should_free_xdg_dir = FALSE;
    char_u	*vimrc1 = NULL;
    char_u	*vimrc2 = NULL;
    char_u	*xdg_dir = NULL;
    char_u	*xdg_rtp = NULL;
    string_T	vimrc_xdg = {NULL, 0};

    // initialize chartab, so we can expand $HOME
    (void)init_chartab();
    vimrc1 = expand_env_save((char_u *)USR_VIMRC_FILE);
    vimrc2 = expand_env_save((char_u *)USR_VIMRC_FILE2);

    xdg_dir = mch_getenv("XDG_CONFIG_HOME");
    නම් (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    නම් (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	goto theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    නම් ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	goto theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    නම් ((opt_idx = findoption((char_u *)"packpath")) < 0)
	goto theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# නම් defined(XDG_VDIR) && defined(FEAT_SESSION)
    නම් ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	goto theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    නම් (should_free_xdg_dir)
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
    static void
set_init_expand_env(void)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;
    char_u	*p;

    සඳහා (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	නම් ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != NULL)
	    p = (char_u *)_(*(var_na **)options[opt_idx].var);
	නැතහොත්
	    p = option_expand(opt_idx, NULL);
	නම් (p != NULL && (p = vim_strsave(p)) != NULL)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    නම් (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#නම් defined(MSWIN) && defined(FEAT_GETTEXT)
/*
 * Get the display language of Windows and the languages to fall back on, as a
 * colon separated list for gettext, e.g. "ja_JP:en_US".  The list stops after
 * English, untranslated messages are English already.
 * Returns NULL when it cannot be obtained.  The result must be freed.
 */
    static char_u *
get_ui_langs(void)
{
    ULONG	num_languages = 0;
    ULONG	bufsize = 0;
    WCHAR	*buffer;
    char_u	*langs = NULL;

    නම් (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num_languages, NULL,
								    &bufsize)
	    || bufsize == 0)
	ආපසු NULL;

    buffer = ALLOC_MULT(WCHAR, bufsize);
    නම් (buffer == NULL)
	ආපසු NULL;

    නම් (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num_languages, buffer,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	නම් (langs != NULL)
	{
	    char_u	*d = langs;
	    WCHAR	*s = buffer;

	    අතරතුර (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		නම් (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		සඳහා ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		නම් (english)
		    bha_nga;
	    }
	    *d = NUL;
	}
    }
    vim_free(buffer);

    ආපසු langs;
}
#endif

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    static void
set_init_lang_env(void)
{
#නම් defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    නම් (mch_getenv((char_u *)"LANG") == NULL
	    && mch_getenv((char_u *)"LANGUAGE") == NULL
	    && mch_getenv((char_u *)"LC_ALL") == NULL
	    && mch_getenv((char_u *)"LC_MESSAGES") == NULL)
    {
	char_u	*langs = get_ui_langs();

	නම් (langs != NULL && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    නම් (colon != NULL)
		*colon = NUL;
	    vim_setenv((char_u *)"LANG", langs);
	}
	vim_free(langs);
    }
#elif defined(MACOS_CONVERT)
    // Moved to os_mac_conv.c to avoid dependency problems.
    mac_lang_init();
#endif
}

/*
 * Initialize the 'encoding' option to a default value.
 */
    static void
set_init_default_encoding(void)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		opt_idx;

#නම් defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#නැතහොත්
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#endif
    නම් (p == NULL)
	ආපසු;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    නම් (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    නම් (mb_init() == NULL)
    {
	opt_idx = findoption((char_u *)"encoding");
	නම් (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#නම් defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	නම් (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    set_string_option_direct((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    set_string_option_direct((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    නම් (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    නම් (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (void)init_chartab();
	}
#endif

#නම් defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	නම් (
# ifdef VIMDLL
		(!gui.in_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    var_na	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    නම් (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, parimaa_na(buf), "cp%ld", (long)GetACP());
	    නැතහොත්
		buflen = vim_snprintf(buf, parimaa_na(buf), "cp%ld", (long)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    නම් (p_tenc != NULL)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		නම් (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_setup(&input_conv, p_tenc, p_enc);
		convert_setup(&output_conv, p_enc, p_tenc);
	    }
	    නැතහොත්
		p_tenc = empty_option;
	}
#endif
#නම් defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#endif
    }
    නැතහොත්
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
    void
set_init_1(පූර්ණ_සංඛ්‍යා clean_arg)
{
#ifdef FEAT_LANGMAP
    langmap_init();
#endif

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    නම් (mch_getenv((char_u *)"VIM_POSIX") != NULL)
    {
	set_string_default("cpo", (char_u *)CPO_ALL);
	set_string_default("shm", (char_u *)SHM_POSIX);
    }

    set_init_default_shell();
    set_init_default_backupskip();
    set_init_default_maxmemtot();
    set_init_default_cdpath();
    set_init_default_printencoding();
#නම් defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
    නම් (clean_arg)
	set_init_clean_rtp();
#endif

#ifdef FEAT_GUI
    නම් (found_reverse_arg)
	set_option_value_give_err((char_u *)"bg", 0L, (char_u *)"dark", 0);
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

#නම් defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    නම් (mch_getenv((char_u *)"MLTERM") != NULL)
	set_option_value_give_err((char_u *)"tbidi", 1L, NULL, 0);
#endif

    didset_options2();

    set_init_lang_env();

#ifdef FEAT_MULTI_LANG
    // Set the default for 'helplang'.
    set_helplang_default(get_mess_lang());
#endif
}

static char_u *fencs_utf8_default = (char_u *)"ucs-bom,utf-8,default,latin1";

/*
 * Set the "fileencodings" option to the default value for when 'encoding' is
 * utf-8.
 */
    void
set_fencs_unicode(void)
{
    set_string_option_direct((char_u *)"fencs", -1, fencs_utf8_default,
								  OPT_FREE, 0);
}

/*
 * Set an option to its default value.
 * This does not take care of side effects!
 */
    static void
set_option_default(
    පූර්ණ_සංඛ්‍යා		opt_idx,
    පූර්ණ_සංඛ්‍යා		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    පූර්ණ_සංඛ්‍යා		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    පූර්ණ_සංඛ්‍යා		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    පූර්ණ_සංඛ්‍යා		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    නම් (varp != NULL)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	නම් (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    නම් (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		set_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    නැතහොත් නම් (options[opt_idx].indir != PV_NONE)
		set_string_option_direct(NULL, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    නැතහොත්
	    {
		නම් ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	නැතහොත් නම් (flags & P_NUM)
	{
	    නම් (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    නැතහොත්
	    {
		long def_val = (long)(long_i)options[opt_idx].def_val[dvi];

		නම් ((long *)varp == &curwin->w_p_so
			|| (long *)varp == &curwin->w_p_siso
			|| (long *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(long *)varp = -1;
		නැතහොත්
		    *(long *)varp = def_val;
		// May also set global value for local option.
		නම් (both)
		    *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	නැතහොත්	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(පූර්ණ_සංඛ්‍යා *)varp = (පූර්ණ_සංඛ්‍යා)(long)(long_i)options[opt_idx].def_val[dvi];
#ifdef UNIX
	    // 'modeline' defaults to off for root
	    නම් (options[opt_idx].indir == PV_ML && getuid() == ROOT_UID)
		*(පූර්ණ_සංඛ්‍යා *)varp = FALSE;
#endif
	    // May also set global value for local option.
	    නම් (both)
		*(පූර්ණ_සංඛ්‍යා *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(පූර්ණ_සංඛ්‍යා *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	නම් (both)
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
    static void
set_options_default(
    පූර්ණ_සංඛ්‍යා		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    පූර්ණ_සංඛ්‍යා		i;
    win_T	*wp;
    tabpage_T	*tp;

    සඳහා (i = 0; !istermoption_idx(i); i++)
	නම් (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#නම් defined(FEAT_CRYPT)
			&& options[i].var != (char_u *)&p_cm
			&& options[i].var != (char_u *)&p_key
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
    static void
set_string_default_esc(var_na *name, char_u *val, පූර්ණ_සංඛ්‍යා escape)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		opt_idx;

    නම් (escape && vim_strchr(val, ' ') != NULL)
	p = vim_strsave_escaped(val, (char_u *)" ");
    නැතහොත්
	p = vim_strsave(val);
    නම් (p == NULL)		// we don't want a NULL
	ආපසු;

    opt_idx = findoption((char_u *)name);
    නම් (opt_idx < 0)
    {
	vim_free(p);
	ආපසු;
    }

    නම් (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    void
set_string_default(var_na *name, char_u *val)
{
    set_string_default_esc(name, val, FALSE);
}

/*
 * For an option value that contains comma separated items, find "newval" in
 * "origval".  Return NULL if not found.
 */
    static char_u *
find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags)
{
    පූර්ණ_සංඛ්‍යා	    bs = 0;
    char_u  *s;

    නම් (origval == NULL)
	ආපසු NULL;

    සඳහා (s = origval; *s != NUL; ++s)
    {
	නම් ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    ආපසු s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	නම් ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	නැතහොත්
	    bs = 0;
    }
    ආපසු NULL;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    void
set_number_default(var_na *name, long val)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;

    opt_idx = findoption((char_u *)name);
    නම් (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#නම් defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    void
set_local_options_default(win_T *wp, පූර්ණ_සංඛ්‍යා do_buffer)
{
    win_T	*save_curwin = curwin;
    පූර්ණ_සංඛ්‍යා		i;

    curwin = wp;
    curbuf = curwin->w_buffer;
    block_autocmds();

    සඳහා (i = 0; !istermoption_idx(i); i++)
    {
	sa_nracanaa vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	නම් (p->indir != PV_NONE
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

#නම් defined(EXITFREE)
/*
 * Free all options.
 */
    void
free_all_options(void)
{
    පූර්ණ_සංඛ්‍යා		i;

    සඳහා (i = 0; !istermoption_idx(i); i++)
    {
	නම් (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    නම් ((options[i].flags & P_ALLOCED) && options[i].var != NULL)
		free_string_option(*(char_u **)options[i].var);
	    නම් (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	නැතහොත් නම් (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# නම් defined(FEAT_EVAL)
    free_findfunc_option();
# endif
}
#endif


/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    void
set_init_2(void)
{
    පූර්ණ_සංඛ්‍යා		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    නම් (idx >= 0 && !(options[idx].flags & P_WAS_SET))
	set_option_default(idx, OPT_LOCAL, p_cp);
    comp_col();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    නම් (!option_was_set((char_u *)"window"))
	p_window = Rows - 1;
    set_number_default("window", Rows - 1);

    // For DOS console the default is always black.
#නම් !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    නම් (idx >= 0 && !(options[idx].flags & P_WAS_SET)
						 && *term_bg_default() == 'd')
    {
	set_string_option_direct(NULL, idx, (char_u *)"dark", OPT_FREE, 0);
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
    (void)parse_printoptions(NULL);	// parse 'printoptions' default value
#endif
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    void
set_init_3(void)
{
#නම් defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    පූර්ණ_සංඛ්‍යා	    idx_srr;
    පූර්ණ_සංඛ්‍යා	    do_srr;
# ifdef FEAT_QUICKFIX
    පූර්ණ_සංඛ්‍යා	    idx_sp;
    පූර්ණ_සංඛ්‍යා	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    නම් (idx_srr < 0)
	do_srr = FALSE;
    නැතහොත්
	do_srr = !(options[idx_srr].flags & P_WAS_SET);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    නම් (idx_sp < 0)
	do_sp = FALSE;
    නැතහොත්
	do_sp = !(options[idx_sp].flags & P_WAS_SET);
# endif
    p = get_isolated_shell_name();
    නම් (p != NULL)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	නම් (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# නම් defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# නම් defined(FEAT_QUICKFIX)
	    නම් (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  නැතහොත්
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    නම් (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	නැතහොත් නම් (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  නම් defined(FEAT_QUICKFIX)
		නම් (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		නම් (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	නැතහොත්
	    // Always use POSIX shell style redirection if we reach this
	    නම් (       fnamecmp(p, "sh") == 0
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
# නම් defined(FEAT_QUICKFIX)
		නම් (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  නැතහොත්
		    නම් (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    නැතහොත්
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		නම් (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#endif

#නම් defined(MSWIN)
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
    නම් (strstr((var_na *)gettail(p_sh), "powershell") != NULL)
    {
	පූර්ණ_සංඛ්‍යා	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	නම් (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	නම් (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    නැතහොත් නම් (strstr((var_na *)gettail(p_sh), "sh") != NULL)
    {
	පූර්ණ_සංඛ්‍යා	idx3;

	idx3 = findoption((char_u *)"shcf");
	නම් (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	නම් (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    නැතහොත් නම් (strstr((var_na *)gettail(p_sh), "cmd.exe") != NULL)
    {
	පූර්ණ_සංඛ්‍යා	idx3;

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
	නම් (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	නම් (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#endif

    නම් (BUFEMPTY())
    {
	පූර්ණ_සංඛ්‍යා idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	නම් (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_SET))
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    set_title_defaults();
}

#නම් defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    void
set_helplang_default(char_u *lang)
{
    පූර්ණ_සංඛ්‍යා		idx;
    size_t	langlen;

    නම් (lang == NULL)	// safety check
	ආපසු;

    langlen = STRLEN(lang);
    නම් (langlen < 2)	// safety check
	ආපසු;

    idx = findoption((char_u *)"hlg");
    නම් (idx < 0 || (options[idx].flags & P_WAS_SET))
	ආපසු;

    නම් (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    නම් (p_hlg == NULL)
	p_hlg = empty_option;
    නැතහොත්
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	නම් (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	නැතහොත් නම් (langlen >= 1 && *p_hlg == 'C')
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
    void
set_title_defaults(void)
{
    පූර්ණ_සංඛ්‍යා	    idx1;
    long    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    නම් (idx1 >= 0 && !(options[idx1].flags & P_WAS_SET))
    {
#ifdef FEAT_GUI
	නම් (gui.starting || gui.in_use)
	    val = TRUE;
	නැතහොත්
#endif
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    නම් (idx1 < 0 || (options[idx1].flags & P_WAS_SET))
    {
	ආපසු;
    }

#ifdef FEAT_GUI
    නම් (gui.starting || gui.in_use)
	val = TRUE;
    නැතහොත්
#endif
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    void
ex_set(exarg_T *eap)
{
    පූර්ණ_සංඛ්‍යා		flags = 0;

    නම් (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    නැතහොත් නම් (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#නම් defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    නම් ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    නැතහොත්
#endif
    {
	නම් (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(void)do_set(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
typedef ga_nanaa {
    PREFIX_NO = 0,	// "no" prefix
    PREFIX_NONE,	// no prefix
    PREFIX_INV,		// "inv" prefix
} set_prefix_T;

/*
 * Return the prefix type for the option name in *argp.
 */
    static set_prefix_T
get_option_prefix(char_u **argp)
{
    පූර්ණ_සංඛ්‍යා		prefix = PREFIX_NONE;
    char_u	*arg = *argp;

    නම් (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	prefix = PREFIX_NO;
	arg += 2;
    }
    නැතහොත් නම් (STRNCMP(arg, "inv", 3) == 0)
    {
	prefix = PREFIX_INV;
	arg += 3;
    }

    *argp = arg;
    ආපසු prefix;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    static පූර්ණ_සංඛ්‍යා
parse_option_name(char_u *arg, පූර්ණ_සංඛ්‍යා *opt_idxp, පූර්ණ_සංඛ්‍යා *lenp, පූර්ණ_සංඛ්‍යා *keyp)
{
    පූර්ණ_සංඛ්‍යා	key = 0;
    පූර්ණ_සංඛ්‍යා	len;
    පූර්ණ_සංඛ්‍යා	opt_idx;

    නම් (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	නම් (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	නැතහොත්
	{
	    len = 1;
	    අතරතුර (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	නම් (arg[len] != '>')
	    ආපසු FAIL;

	arg[len] = NUL;			    // put NUL after name
	නම් (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	නම් (opt_idx == -1)
	    key = find_key_option(arg + 1, TRUE);
    }
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	නම් (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	නැතහොත්
	    අතරතුර (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	නම් (opt_idx == -1)
	    key = find_key_option(arg, FALSE);
    }

    *keyp = key;
    *lenp = len;
    *opt_idxp = opt_idx;

    ආපසු OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    static set_op_T
get_opt_op(char_u *arg)
{
    set_op_T op = OP_NONE;

    නම් (*arg != NUL && *(arg + 1) == '=')
    {
	නම් (*arg == '+')
	    op = OP_ADDING;		// "+="
	නැතහොත් නම් (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	නැතහොත් නම් (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    ආපසු op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
static var_na *modeline_whitelist[] =
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
    NULL
};

/*
 * Return TRUE if option "name" is in the modeline whitelist.
 */
    static bool
is_modeline_whitelisted(var_na *name)
{
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; modeline_whitelist[i] != NULL; i++)
	නම් (STRCMP(name, modeline_whitelist[i]) == 0)
	    ආපසු true;
    ආපසු false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    static පූර්ණ_සංඛ්‍යා
validate_opt_idx(
	පූර්ණ_සංඛ්‍යා opt_idx,
	පූර්ණ_සංඛ්‍යා opt_flags,
	long_u flags,
	var_na **errmsg,
	set_prefix_T prefix)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    නම් ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	ආපසු FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    නම් ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	ආපසු FAIL;

    // Disallow changing some options from modelines.
    නම් (opt_flags & OPT_MODELINE)
    {
	නම් (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_in_modeline;
	    ආපසු FAIL;
	}
	නම් ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_in_modeline_when_modelineexpr_is_off;
	    ආපසු FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	නම් (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    නම් (options[opt_idx].indir == PV_ML && prefix == PREFIX_NO)
		ආපසු OK;
	    නැතහොත් නම් (!is_modeline_whitelisted(options[opt_idx].fullname))
		ආපසු FAIL;
	}
#ifdef FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	නම් (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    ආපසු FAIL;
#endif
    }

#ifdef HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    නම් (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_in_sandbox;
	ආපසු FAIL;
    }
#endif

    ආපසු OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    static char_u *
stropt_get_default_val(
    පූර්ණ_සංඛ්‍යා		opt_idx,
    char_u	*varp,
    පූර්ණ_සංඛ්‍යා		flags,
    පූර්ණ_සංඛ්‍යා		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    නම් ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ifdef FEAT_GUI
	නම් (gui.in_use)
	    newval = gui_bg_default();
	නැතහොත්
#endif
	    newval = term_bg_default();
    }
    නැතහොත් නම් ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    නම් (newval == NULL)
	newval = empty_option;
    නැතහොත්
    {
	char_u *s = option_expand(opt_idx, newval);
	නම් (s == NULL)
	    s = newval;
	newval = vim_strsave(s);
    }

    ආපසු newval;
}

/*
 * Convert the 'backspace' option number value to a string: for adding,
 * prepending and removing string.
 */
    static void
opt_backspace_nr2str(
    char_u	*varp,
    char_u	**origval_p,
    char_u	**origval_l_p,
    char_u	**origval_g_p,
    char_u	**oldval_p)
{
    පූර්ණ_සංඛ්‍යා		i = getdigits((char_u **)varp);

    switch (i)
    {
	pak_sa 0:
	    *(char_u **)varp = empty_option;
	    bha_nga;
	pak_sa 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    bha_nga;
	pak_sa 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    bha_nga;
	pak_sa 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    bha_nga;
    }
    vim_free(*oldval_p);
    නම් (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    නම් (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    නම් (*origval_g_p == *oldval_p)
	*origval_g_p = *(char_u **)varp;
    *oldval_p = *(char_u **)varp;
}

/*
 * Convert the 'whichwrap' option number value to a string, for backwards
 * compatibility with Vim 3.0.
 * Note: 'argp' is a pointer to a char_u pointer and is updated.
 */
    static char_u *
opt_whichwrap_nr2str(char_u **argp, char_u *whichwrap)
{
    size_t  len = 0;

    *whichwrap = NUL;
    පූර්ණ_සංඛ්‍යා i = getdigits(argp);
    නම් (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    නම් (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    නම් (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    නම් (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    නම් (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    නම් (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    ආපසු whichwrap;
}

/*
 * Copy the new string value into allocated memory for the option.
 * Can't use set_string_option_direct(), because we need to remove the
 * backslashes.
 */
    static char_u *
stropt_copy_value(
    char_u	*origval,
    char_u	**argp,
    set_op_T	op,
    පූර්ණ_සංඛ්‍යා		flags UNUSED)
{
    char_u	*arg = *argp;
    unsigned	newlen;
    char_u	*newval;
    char_u	*s = NULL;

    // get a bit too much
    newlen = (unsigned)STRLEN(arg) + 1;
    නම් (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;
    newval = alloc(newlen);
    නම් (newval == NULL)  // out of mem, don't change
	ආපසු NULL;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    අතරතුර (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	පූර්ණ_සංඛ්‍යා i;

	නම් (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#endif
	   )
	    ++arg;	// remove backslash
	නම් (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	නැතහොත්
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    ආපසු newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    static char_u *
stropt_expand_envvar(
    පූර්ණ_සංඛ්‍යා		opt_idx,
    char_u	*origval,
    char_u	*newval,
    set_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    නම් (s == NULL)
	ආපසු newval;

    vim_free(newval);
    unsigned newlen = (unsigned)STRLEN(s) + 1;
    නම් (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;

    newval = alloc(newlen);
    නම් (newval == NULL)
	ආපසු NULL;

    STRCPY(newval, s);

    ආපසු newval;
}

/*
 * Concatenate the original and new values of a string option, adding a "," if
 * needed.
 */
    static void
stropt_concat_with_comma(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    පූර්ණ_සංඛ්‍යා		flags)
{
    පූර්ණ_සංඛ්‍යා		len = 0;

    පූර්ණ_සංඛ්‍යා comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    නම් (op == OP_ADDING)
    {
	len = (පූර්ණ_සංඛ්‍යා)STRLEN(origval);
	// strip a trailing comma, would get 2
	නම් (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    නැතහොත්
    {
	len = (පූර්ණ_සංඛ්‍යා)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    නම් (comma)
	newval[len] = ',';
}

/*
 * Remove a value from a string option.  Copy string option value in "origval"
 * to "newval" and then remove the string "strval" of length "len".
 */
    static void
stropt_remove_val(
    char_u	*origval,
    char_u	*newval,
    පූර්ණ_සංඛ්‍යා		flags,
    char_u	*strval,
    පූර්ණ_සංඛ්‍යා		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    නම් (*strval)
    {
	// may need to remove a comma
	නම් (flags & P_COMMA)
	{
	    නම් (strval == origval)
	    {
		// include comma after string
		නම් (strval[len] == ',')
		    ++len;
	    }
	    නැතහොත්
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
    static char_u *
find_key_item(char_u *src, char_u *key, පූර්ණ_සංඛ්‍යා keylen, පූර්ණ_සංඛ්‍යා *itemlenp)
{
    char_u  *p = src;

    අතරතුර (*p != NUL)
    {
	// Check if this item starts with the same key
	නම් ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, key, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *end = vim_strchr(p, ',');
	    නම් (end == NULL)
		end = p + STRLEN(p);
	    *itemlenp = (පූර්ණ_සංඛ්‍යා)(end - p);
	    ආපසු p;
	}
	++p;
    }
    ආපසු NULL;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    static void
remove_comma_item(char_u *str, char_u *item, පූර්ණ_සංඛ්‍යා itemlen)
{
    නම් (item[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(item, item + itemlen + 1);
    නැතහොත් නම් (item > str && *(item - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(item - 1, item + itemlen);
    නැතහොත්
	// Only item
	*item = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    static void
remove_key_item(char_u *str, char_u *key, පූර්ණ_සංඛ්‍යා keylen, char_u *skip)
{
    පූර්ණ_සංඛ්‍යා	    itemlen;
    char_u  *found;

    අතරතුර ((found = find_key_item(str, key, keylen, &itemlen)) != NULL)
    {
	නම් (found == skip)
	{
	    // Search for the next match after this one.
	    char_u *next = found + itemlen;
	    නම් (*next == ',')
		++next;
	    found = find_key_item(next, key, keylen, &itemlen);
	    නම් (found == NULL)
		bha_nga;
	}

	remove_comma_item(str, found, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    static void
append_item(char_u *str, char_u *item, පූර්ණ_සංඛ්‍යා item_len)
{
    පූර්ණ_සංඛ්‍යා len = (පූර්ණ_සංඛ්‍යා)STRLEN(str);

    නම් (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, item, (size_t)item_len);
    str[len + item_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    static void
prepend_item(char_u *str, char_u *item, පූර්ණ_සංඛ්‍යා item_len)
{
    පූර්ණ_සංඛ්‍යා len = (පූර්ණ_සංඛ්‍යා)STRLEN(str);
    පූර්ණ_සංඛ්‍යා comma = (len > 0) ? 1 : 0;

    mch_memmove(str + item_len + comma, str, (size_t)len + 1);
    mch_memmove(str, item, (size_t)item_len);
    නම් (comma)
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
    static bool
stropt_handle_keymatch(
    char_u	*origval,
    char_u	*newval,
    set_op_T	op,
    පූර්ණ_සංඛ්‍යා		flags UNUSED)
{
    char_u  *p;
    char_u  *item_start;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    නම් (vim_strchr(newval, ':') == NULL && vim_strchr(newval, ',') == NULL)
	ආපසු false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    නම් (newval_copy == NULL)
	ආපසු false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    item_start = newval_copy;
    සඳහා (;;)
    {
	p = vim_strchr(item_start, ',');
	පූර්ණ_සංඛ්‍යා item_len = (p == NULL)
			    ? (පූර්ණ_සංඛ්‍යා)STRLEN(item_start) : (පූර්ණ_සංඛ්‍යා)(p - item_start);

	නම් (item_len > 0)
	{
	    char_u *colon = vim_strchr(item_start, ':');
	    නම් (colon != NULL && colon < item_start + item_len)
	    {
		පූර්ණ_සංඛ්‍යා keylen = (පූර්ණ_සංඛ්‍යා)(colon - item_start) + 1;

		නම් (op == OP_ADDING || op == OP_PREPENDING)
		{
		    පූර්ණ_සංඛ්‍යා old_itemlen;
		    char_u *found = find_key_item(newval, item_start,
						       keylen, &old_itemlen);
		    නම් (found != NULL)
		    {
			නම් (old_itemlen == item_len
				&& STRNCMP(found, item_start,
							 item_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_key_item(newval, item_start,
							   keylen, found);
			}
			නැතහොත්
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_key_item(newval, item_start,
							    keylen, NULL);
			    නම් (op == OP_PREPENDING)
				prepend_item(newval, item_start, item_len);
			    නැතහොත්
				append_item(newval, item_start, item_len);
			}
		    }
		    නැතහොත්
		    {
			// New item.
			නම් (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			නැතහොත්
			    append_item(newval, item_start, item_len);
		    }
		}
		නැතහොත් නම් (op == OP_REMOVING)
		    remove_key_item(newval, item_start, keylen, NULL);
	    }
	    නැතහොත්
	    {
		නම් (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    නම් (found == NULL)
		    {
			// New item.
			නම් (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			නැතහොත්
			    append_item(newval, item_start, item_len);
		    }
		    // else: exact duplicate — do nothing
		}
		නැතහොත් නම් (op == OP_REMOVING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    නම් (found != NULL)
			remove_comma_item(newval, found, item_len);
		}
	    }
	}

	නම් (p == NULL)
	    bha_nga;
	item_start = p + 1;
    }

    vim_free(newval_copy);

    ආපසු true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    static void
stropt_remove_dupflags(char_u *newval, පූර්ණ_සංඛ්‍යා flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    අතරතුර (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	නම් (flags & P_ONECOMMA)
	{
	    නම් (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != NULL)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		continue;
	    }
	}
	නැතහොත්
	{
	    නම් ((!(flags & P_COMMA) || *s != ',')
		    && vim_strchr(s + 1, *s) != NULL)
	    {
		STRMOVE(s, s + 1);
		continue;
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
    static char_u *
stropt_get_newval(
    පූර්ණ_සංඛ්‍යා		nextchar,
    පූර්ණ_සංඛ්‍යා		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    set_op_T    *op_arg,
    පූර්ණ_සංඛ්‍යා		flags,
    පූර්ණ_සංඛ්‍යා		cp_val)
{
    char_u	*arg = *argp;
    char_u	*origval = *origval_arg;
    char_u	*origval_l = *origval_l_arg;
    char_u	*origval_g = *origval_g_arg;
    char_u	*oldval = *oldval_arg;
    set_op_T    op = *op_arg;
    char_u	*save_arg = NULL;
    char_u	*newval;
    char_u	*s = NULL;
    char_u	whichwrap[80];

    නම් (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    නැතහොත් නම් (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    නැතහොත්
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	නම් (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	නැතහොත් නම් (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	නැතහොත් නම් (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	නැතහොත් නම් (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_value(origval, &arg, op, flags);
	නම් (newval == NULL)
	    goto done;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	නම් (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    නම් (newval == NULL)
		goto done;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	නම් ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	නැතහොත්
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    පූර්ණ_සංඛ්‍යා len = 0;
	    නම් (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (පූර්ණ_සංඛ්‍යා)STRLEN(newval);
		s = find_dup_item(origval, newval, len, flags);

		// do not add if already there
		නම් ((op == OP_ADDING || op == OP_PREPENDING) && s != NULL)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		නම් (s == NULL)
		    s = origval + (පූර්ණ_සංඛ්‍යා)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    නම් (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    නැතහොත් නම් (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	නම් (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

done:
    නම් (save_arg != NULL)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    ආපසු newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    static පූර්ණ_සංඛ්‍යා
do_set_option_string(
	පූර්ණ_සංඛ්‍යා	    opt_idx,
	පූර්ණ_සංඛ්‍යා	    opt_flags,
	char_u	    **argp,
	පූර්ණ_සංඛ්‍යා	    nextchar,
	set_op_T    op_arg,
	long_u	    flags,
	පූර්ණ_සංඛ්‍යා	    cp_val,
	char_u	    *varp_arg,
	var_na	    *errbuf,
	size_t	    errbuflen,
	පූර්ණ_සංඛ්‍යා	    *value_checked,
	var_na	    **errmsg)
{
    char_u	*arg = *argp;
    set_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = NULL; // previous value if *varp
    char_u	*newval;
    char_u	*origval = NULL;
    char_u	*origval_l = NULL;
    char_u	*origval_g = NULL;
#නම් defined(FEAT_EVAL)
    char_u	*saved_origval = NULL;
    char_u	*saved_origval_l = NULL;
    char_u	*saved_origval_g = NULL;
    char_u	*saved_newval = NULL;
#endif

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    නම් ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((පූර්ණ_සංඛ්‍යා)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    නම් ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	නම් (((පූර්ණ_සංඛ්‍යා)options[opt_idx].indir & PV_BOTH)
						  && origval_l == empty_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    නම් (((පූර්ණ_සංඛ්‍යා)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    නැතහොත්
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    නම් (newval == NULL)
	*(char_u **)(varp) = empty_option;

#නම් defined(FEAT_EVAL)
    නම් (!starting
# ifdef FEAT_CRYPT
	    && options[opt_idx].indir != PV_KEY
# endif
		      && origval != NULL && newval != NULL)
    {
	// origval may be freed by did_set_string_option(), make a copy.
	saved_origval = vim_strsave(origval);
	// newval (and varp) may become invalid if the buffer is closed by
	// autocommands.
	saved_newval = vim_strsave(newval);
	නම් (origval_l != NULL)
	    saved_origval_l = vim_strsave(origval_l);
	නම් (origval_g != NULL)
	    saved_origval_g = vim_strsave(origval_g);
    }
#endif

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	පූර්ණ_සංඛ්‍යා	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	නම් ((opt_flags & OPT_MODELINE)
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
			errbuflen, opt_flags, op, value_checked);

	secure = secure_saved;
    }

#නම් defined(FEAT_EVAL)
    නම් (*errmsg == NULL)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#endif

    *argp = arg;
    ආපසු *errmsg == NULL ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    static var_na *
do_set_option_bool(
    පූර්ණ_සංඛ්‍යා		opt_idx,
    පූර්ණ_සංඛ්‍යා		opt_flags,
    set_prefix_T prefix,
    long_u	flags,
    char_u	*varp,
    පූර්ණ_සංඛ්‍යා		nextchar,
    පූර්ණ_සංඛ්‍යා		afterchar,
    පූර්ණ_සංඛ්‍යා		cp_val)

{
    varnumber_T	value;

    නම් (nextchar == '=' || nextchar == ':')
	ආපසු e_invalid_argument;
    නම් (opt_idx < 0 || varp == NULL)
	ආපසු NULL;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    නම් (nextchar == '!')
	value = *(පූර්ණ_සංඛ්‍යා *)(varp) ^ 1;
    නැතහොත් නම් (nextchar == '&')
	value = (පූර්ණ_සංඛ්‍යා)(long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    නැතහොත් නම් (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	නම් ((පූර්ණ_සංඛ්‍යා *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    value = -1;
	නැතහොත් නම් ((පූර්ණ_සංඛ්‍යා *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    value = -1;
#ifdef HAVE_FSYNC
	නැතහොත් නම් ((පූර්ණ_සංඛ්‍යා *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    value = -1;
#endif
	නැතහොත්
	    value = *(පූර්ණ_සංඛ්‍යා *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    නැතහොත්
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	නම් (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    ආපසු e_trailing_characters;
	නම් (prefix == PREFIX_INV)
	    value = *(පූර්ණ_සංඛ්‍යා *)(varp) ^ 1;
	නැතහොත්
	    value = prefix == PREFIX_NO ? 0 : 1;
    }

    ආපසු set_bool_option(opt_idx, varp, (පූර්ණ_සංඛ්‍යා)value, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    static var_na *
do_set_option_numeric(
    පූර්ණ_සංඛ්‍යා		opt_idx,
    පූර්ණ_සංඛ්‍යා		opt_flags,
    char_u	**argp,
    පූර්ණ_සංඛ්‍යා		nextchar,
    set_op_T	op,
    long_u	flags,
    පූර්ණ_සංඛ්‍යා		cp_val,
    char_u	*varp,
    var_na	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		value;
    පූර්ණ_සංඛ්‍යා			i;
    var_na		*errmsg = NULL;

    නම් (opt_idx < 0 || varp == NULL)
	ආපසු NULL;  // "cannot happen"
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
    නම් (nextchar == '&')
	value = (long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    නැතහොත් නම් (nextchar == '<')
    {
	නම් ((long *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    value = NO_LOCAL_UNDOLEVEL;
	නැතහොත් නම් (opt_flags == OPT_LOCAL
		    && ((long *)varp == &curwin->w_p_siso
		     || (long *)varp == &curwin->w_p_so
		     || (long *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    value = -1;
	නැතහොත්
	    value = *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    නැතහොත් නම් (((long *)varp == &p_wc || (long *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	value = string_to_key(arg, FALSE);
	නම් (value == 0)
	{
	    errmsg = e_invalid_argument;
	    goto skip;
	}
    }
    නැතහොත් නම් (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, NULL, &i, STR2NR_ALL, &value, NULL, 0, TRUE, NULL);
	නම් (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_number_required_after_equal;
	    goto skip;
	}
    }
    නැතහොත්
    {
	errmsg = e_number_required_after_equal;
	goto skip;
    }

    නම් (op == OP_ADDING)
	value = *(long *)varp + value;
    නැතහොත් නම් (op == OP_PREPENDING)
	value = *(long *)varp * value;
    නැතහොත් නම් (op == OP_REMOVING)
	value = *(long *)varp - value;

    නම් ((long *)varp == &curwin->w_p_sop && value < -1)
    {
	errmsg = e_invalid_argument;
	goto skip;
    }

    errmsg = set_num_option(opt_idx, varp, value, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    ආපසු errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    static var_na *
do_set_option_keycode(char_u **argp, char_u *key_name, පූර්ණ_සංඛ්‍යා nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    නම් (nextchar == '&')
    {
	නම් (add_termcap_entry(key_name, TRUE) == FAIL)
	    ආපසු e_not_found_in_termcap;
    }
    නැතහොත්
    {
	++arg; // jump to after the '=' or ':'
	සඳහා (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    නම් (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	add_termcode(key_name, arg, FALSE);
	*p = nextchar;
    }
    නම් (full_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    ආපසු NULL;
}

/*
 * Set an option to a new value.
 */
    static var_na *
do_set_option_value(
    පූර්ණ_සංඛ්‍යා		opt_idx,
    පූර්ණ_සංඛ්‍යා		opt_flags,
    char_u	**argp,
    set_prefix_T prefix,
    set_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*key_name,
    පූර්ණ_සංඛ්‍යා		nextchar,
    පූර්ණ_සංඛ්‍යා		afterchar,
    පූර්ණ_සංඛ්‍යා		cp_val,
    පූර්ණ_සංඛ්‍යා		*stopopteval,
    var_na	*errbuf,
    size_t	errbuflen)
{
    පූර්ණ_සංඛ්‍යා		value_checked = FALSE;
    var_na	*errmsg = NULL;
    char_u	*arg = *argp;

    නම් (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_set_option_bool(opt_idx, opt_flags, prefix, flags, varp,
						nextchar, afterchar, cp_val);
	නම් (errmsg != NULL)
	    goto skip;
    }
    නැතහොත්
    {
	// numeric or string option
	නම් (vim_strchr((char_u *)"=:&<", nextchar) == NULL
		|| prefix != PREFIX_NONE)
	{
	    errmsg = e_invalid_argument;
	    goto skip;
	}

	නම් (flags & P_NUM)
	{
	    // numeric option
	    errmsg = do_set_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    නම් (errmsg != NULL)
		goto skip;
	}
	නැතහොත් නම් (opt_idx >= 0)
	{
	    // string option
	    නම් (do_set_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&value_checked, &errmsg) == FAIL)
	    {
		නම් (errmsg != NULL)
		    goto skip;
		*stopopteval = TRUE;
		goto skip;
	    }
	}
	නැතහොත්
	{
	    // key code option
	    errmsg = do_set_option_keycode(&arg, key_name, nextchar);
	    නම් (errmsg != NULL)
		goto skip;
	}
    }

    නම් (opt_idx >= 0)
	did_set_option(opt_idx, opt_flags, op == OP_NONE, value_checked);

skip:
    *argp = arg;
    ආපසු errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    static var_na *
do_set_option(
    පූර්ණ_සංඛ්‍යා		opt_flags,
    char_u	**argp,
    char_u	*arg_start,
    char_u	**startarg,
    පූර්ණ_සංඛ්‍යා		*did_show,
    පූර්ණ_සංඛ්‍යා		*stopopteval,
    var_na	*errbuf,
    size_t	errbuflen)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;
    char_u	*arg;
    set_prefix_T prefix;	// no prefix, "no" prefix or "inv" prefix
    set_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	key_name[2];
    පූර්ණ_සංඛ්‍යා		nextchar;	// next non-white char after option name
    පූර්ණ_සංඛ්‍යා		afterchar;	// character just after option name
    var_na	*errmsg = NULL;
    පූර්ණ_සංඛ්‍යා		key;
    පූර්ණ_සංඛ්‍යා		len;

    prefix = get_option_prefix(argp);
    arg = *argp;

    // find end of name
    key = 0;
    නම් (parse_option_name(arg, &opt_idx, &len, &key) == FAIL)
	ආපසු e_invalid_argument;

    // remember character after option name
    afterchar = arg[len];

    නම් (in_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	නම් (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != NULL
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    goto skip;
	}
    }
    නැතහොත්
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	අතරතුර (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    නම් (op != OP_NONE)
	len++;

    nextchar = arg[len];

    නම් (opt_idx == -1 && key == 0)	// found a mismatch: skip
    {
	නම් (in_vim9script() && arg > arg_start
		&& vim_strchr((char_u *)"!&<", *arg) != NULL)
	    errmsg = e_no_white_space_allowed_between_option_and;
	නැතහොත්
	    errmsg = e_unknown_option;
	goto skip;
    }

    නම් (opt_idx >= 0)
    {
	නම් (options[opt_idx].var == NULL)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    නම් (vim_strchr((char_u *)"=:!&<", nextchar) == NULL
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    goto skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    නැතහොත්
    {
	flags = P_STRING;
	varp = NULL;
	නම් (key < 0)
	{
	    key_name[0] = KEY2TERMCAP0(key);
	    key_name[1] = KEY2TERMCAP1(key);
	}
	නැතහොත්
	{
	    key_name[0] = KS_KEY;
	    key_name[1] = (key & 0xff);
	}
    }

    // Make sure the option value can be changed.
    නම් (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, prefix) == FAIL)
	goto skip;

    පූර්ණ_සංඛ්‍යා cp_val = p_cp;
    නම් (vim_strchr((char_u *)"?=:!&<", nextchar) != NULL)
    {
	arg += len;
	නම් (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    නම් (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    නැතහොත්		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	නම් (vim_strchr((char_u *)"?!&<", nextchar) != NULL
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    goto skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    නම් (nextchar == '?'
	    || (prefix == PREFIX_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == NULL
		&& !(flags & P_BOOL)))
    {
	// print value
	නම් (*did_show)
	    msg_putchar('\n');	    // cursor below last one
	නැතහොත්
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	නම් (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ifdef FEAT_EVAL
	    නම් (p_verbose > 0)
	    {
		// Mention where the option was last set.
		නම් (varp == options[opt_idx].var)
		    last_set_msg(options[opt_idx].script_ctx);
		නැතහොත් නම් ((පූර්ණ_සංඛ්‍යා)options[opt_idx].indir & PV_WIN)
		    last_set_msg(curwin->w_p_script_ctx[
			    (පූර්ණ_සංඛ්‍යා)options[opt_idx].indir & PV_MASK]);
		නැතහොත් නම් ((පූර්ණ_සංඛ්‍යා)options[opt_idx].indir & PV_BUF)
		    last_set_msg(curbuf->b_p_script_ctx[
			    (පූර්ණ_සංඛ්‍යා)options[opt_idx].indir & PV_MASK]);
	    }
#endif
	}
	නැතහොත්
	{
	    char_u	    *p;

	    p = find_termcode(key_name);
	    නම් (p == NULL)
	    {
		errmsg = e_key_code_not_set;
		goto skip;
	    }
	    නැතහොත්
		(void)show_one_termcode(key_name, p, TRUE);
	}
	නම් (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    නැතහොත්
    {
	errmsg = do_set_option_value(opt_idx, opt_flags, &arg, prefix, op,
					flags, varp, key_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    ආපසු errmsg;
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
    පූර්ණ_සංඛ්‍යා
do_set(
    char_u	*arg_start,	// option string (may be written to!)
    පූර්ණ_සංඛ්‍යා		opt_flags)
{
    char_u	*arg = arg_start;
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		did_show = FALSE;   // already showed one value

    නම් (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	goto theend;
    }

    අතරතුර (*arg != NUL)		// loop to process all options
    {
	නම් (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    නම් (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		set_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    නැතහොත්
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	නැතහොත් නම් (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	නැතහොත්
	{
	    පූර්ණ_සංඛ්‍යා		stopopteval = FALSE;
	    var_na	*errmsg = NULL;
	    var_na	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_set_option(opt_flags, &arg, arg_start, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    නම් (stopopteval)
		bha_nga;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    සඳහා (i = 0; i < 2 ; ++i)
	    {
		අතරතුර (*arg != NUL && !VIM_ISWHITE(*arg))
		    නම් (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		නම් (*arg != '=')
		    bha_nga;
	    }

	    නම් (errmsg != NULL)
	    {
		i = vim_snprintf((var_na *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		නම් (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((var_na *)IObuff);	// show error highlighted
		--no_wait_return;

		ආපසු FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    නම් (silent_mode && did_show)
    {
	// After displaying option values in silent mode.
	silent_mode = FALSE;
	info_message = TRUE;	// use mch_msg(), not mch_errmsg()
	msg_putchar('\n');
	cursor_on();		// msg_start() switches it off
	out_flush();
	silent_mode = TRUE;
	info_message = FALSE;	// use mch_msg(), not mch_errmsg()
    }

    ආපසු OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    void
did_set_option(
    පූර්ණ_සංඛ්‍යා	    opt_idx,
    පූර්ණ_සංඛ්‍යා	    opt_flags,	    // possibly with OPT_MODELINE
    පූර්ණ_සංඛ්‍යා	    new_value,	    // value was replaced completely
    පූර්ණ_සංඛ්‍යා	    value_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = NULL;
    පූර්ණ_සංඛ්‍යා		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_SET;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    නම් (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    නම් (!value_checked && (secure
#ifdef HAVE_SANDBOX
	    || sandbox != 0
#endif
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	නම් (flagsp_local != NULL)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    නැතහොත් නම් (new_value)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	නම් (flagsp_local != NULL)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    පූර්ණ_සංඛ්‍යා
string_to_key(char_u *arg, පූර්ණ_සංඛ්‍යා multi_byte)
{
    නම් (*arg == '<' && arg[1])
	ආපසු find_key_option(arg + 1, TRUE);
    නම් (*arg == '^' && arg[1])
    {
	පූර්ණ_සංඛ්‍යා key = Ctrl_chr(arg[1]);
	නම් (key == 0)		// ^@ is <Nul>
	    key = K_ZERO;
	ආපසු key;
    }
    නම් (multi_byte)
	ආපසු PTR2CHAR(arg);
    ආපසු *arg;
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    void
did_set_title(void)
{
    නම් (starting != NO_SCREEN
#ifdef FEAT_GUI
	    && !gui.starting
#endif
				)
	maketitle();
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    void
set_options_bin(
    පූර්ණ_සංඛ්‍යා		oldval,
    පූර්ණ_සංඛ්‍යා		newval,
    පූර්ණ_සංඛ්‍යා		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    නම් (newval)
    {
	නම් (!oldval)		// switched on
	{
	    නම් (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    නම් (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	නම් (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	නම් (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    නැතහොත් නම් (oldval)		// switched off
    {
	නම් (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	නම් (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#නම් defined(FEAT_EVAL)
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
    static char_u *
option_expand(පූර්ණ_සංඛ්‍යා opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    නම් (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == NULL)
	ආපසු NULL;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    නම් (val != NULL && STRLEN(val) > MAXPATHL)
	ආපසු NULL;

    නම් (val == NULL)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    පූර්ණ_සංඛ්‍යා esc = var == &p_tags || var == &p_path;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : NULL, FALSE,
#ifdef FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#endif
				  NULL);
    නම් (STRCMP(NameBuff, val) == 0)   // they are the same
	ආපසු NULL;

    ආපසු NameBuff;
}

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    static void
didset_options(void)
{
    // initialize the table for 'iskeyword' et.al.
    (void)init_chartab();

    didset_string_options();

#ifdef FEAT_SPELL
    (void)spell_check_msm();
    (void)spell_check_sps();
    (void)compile_cap_prog(curwin->w_s);
    (void)did_set_spell_option();
#endif
    // set cedit_key
    (void)did_set_cedit(NULL);
#ifdef FEAT_LINEBREAK
    // initialize the table for 'breakat'.
    did_set_breakat(NULL);
#endif
    after_copy_winopt(curwin);
}

/*
 * More side effects of setting options.
 */
    static void
didset_options2(void)
{
    // Initialize the highlight_attr[] table.
    (void)highlight_changed();

    // Parse default for 'wildmode'
    check_opt_wim();

    // Parse default for 'listchars'.
    (void)set_listchars_option(curwin, curwin->w_p_lcs, TRUE, NULL, 0);

    // Parse default for 'fillchars'.
    (void)set_fillchars_option(curwin, curwin->w_p_fcs, TRUE, NULL, 0);

#ifdef FEAT_CLIPBOARD
    // Parse default for 'clipboard'
    (void)did_set_clipboard(NULL);
#endif
#ifdef FEAT_VARTABS
    vim_free(curbuf->b_p_vsts_array);
    (void)tabstop_set(curbuf->b_p_vsts, &curbuf->b_p_vsts_array);
    vim_free(curbuf->b_p_vts_array);
    (void)tabstop_set(curbuf->b_p_vts,  &curbuf->b_p_vts_array);
#endif
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    void
check_options(void)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;

    සඳහා (opt_idx = 0; options[opt_idx].fullname != NULL; opt_idx++)
	නම් ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != NULL)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    පූර්ණ_සංඛ්‍යා
get_term_opt_idx(char_u **p)
{
    පූර්ණ_සංඛ්‍යා opt_idx;

    සඳහා (opt_idx = 1; options[opt_idx].fullname != NULL; opt_idx++)
	නම් (options[opt_idx].var == (char_u *)p)
	    ආපසු opt_idx;
    ආපසු -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    පූර්ණ_සංඛ්‍යා
set_term_option_alloced(char_u **p)
{
    පූර්ණ_සංඛ්‍යා		opt_idx = get_term_opt_idx(p);

    නම් (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    ආපසු opt_idx;
}

#නම් defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    පූර්ණ_සංඛ්‍යා
was_set_insecurely(win_T *wp, char_u *opt, පූර්ණ_සංඛ්‍යා opt_flags)
{
    පූර්ණ_සංඛ්‍යා	    idx = findoption(opt);
    long_u  *flagp;

    නම් (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	ආපසු (*flagp & P_INSECURE) != 0;
    }
    internal_error("was_set_insecurely()");
    ආපසු -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    static long_u *
insecure_flag(win_T *wp, පූර්ණ_සංඛ්‍යා opt_idx, පූර්ණ_සංඛ්‍යා opt_flags)
{
    නම් (opt_flags & OPT_LOCAL)
	switch ((පූර්ණ_සංඛ්‍යා)options[opt_idx].indir)
	{
	    pak_sa PV_WRAP:	ආපසු &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    pak_sa PV_STL:	ආපසු &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    pak_sa PV_FDE:	ආපසු &wp->w_p_fde_flags;
	    pak_sa PV_FDT:	ආපසු &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    pak_sa PV_BEXPR:	ආපසු &wp->w_buffer->b_p_bexpr_flags;
#  endif
	    pak_sa PV_INDE:	ආපසු &wp->w_buffer->b_p_inde_flags;
	    pak_sa PV_FEX:	ආපසු &wp->w_buffer->b_p_fex_flags;
#  ifdef FEAT_FIND_ID
	    pak_sa PV_INEX:	ආපසු &wp->w_buffer->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    pak_sa PV_CPT:	ආපසු &wp->w_buffer->b_p_cpt_flags;
#  endif
# endif
	}
    නැතහොත්
	// For global value of window-local options, use flags in w_allbuf_opt.
	switch ((පූර්ණ_සංඛ්‍යා)options[opt_idx].indir)
	{
	    pak_sa PV_WRAP:	ආපසු &wp->w_allbuf_opt.wo_wrap_flags;
# නම් defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    pak_sa PV_FDE:	ආපසු &wp->w_allbuf_opt.wo_fde_flags;
	    pak_sa PV_FDT:	ආපසු &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    ආපසු &options[opt_idx].flags;
}
#endif

/*
 * Redraw the window title and/or tab page text later.
 */
    void
redraw_titles(void)
{
    need_maketitle = TRUE;
    redraw_tabline = TRUE;
#නම් defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    පූර්ණ_සංඛ්‍යා
valid_name(char_u *val, var_na *allowed)
{
    char_u *s;

    සඳහා (s = val; *s != NUL; ++s)
	නම් (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == NULL)
	    ආපසු FALSE;
    ආපසු TRUE;
}

#නම් defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    void
set_option_sctx_idx(පූර්ණ_සංඛ්‍යා opt_idx, පූර්ණ_සංඛ්‍යා opt_flags, sctx_T script_ctx)
{
    පූර්ණ_සංඛ්‍යා		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    පූර්ණ_සංඛ්‍යා		indir = (පූර්ණ_සංඛ්‍යා)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    නම් (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    නම් (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    නම් (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    නම් (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    නම් (both || (opt_flags & OPT_LOCAL))
    {
	නම් (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	නැතහොත් නම් (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	    නම් (both)
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
is_option_value_vim9(පූර්ණ_සංඛ්‍යා opt_idx, පූර්ණ_සංඛ්‍යා opt_flags)
{
    පූර්ණ_සංඛ්‍යා		indir = (පූර්ණ_සංඛ්‍යා)options[opt_idx].indir;
    sctx_T	*sctx = NULL;

    නම් ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    නම් ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	නම් (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_MASK];
	නැතහොත් නම් (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_MASK];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    ආපසු !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_version >= SCRIPT_VERSION_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(var_na *name)
{
    පූර්ණ_සංඛ්‍යා idx = findoption((char_u *)name);

    නම් (idx >= 0)
	ආපසු &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    ආපසු NULL;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    void
set_term_option_sctx_idx(var_na *name, පූර්ණ_සංඛ්‍යා opt_idx)
{
    char_u  buf[5];
    පූර්ණ_සංඛ්‍යා	    idx;

    නම් (name == NULL)
	idx = opt_idx;
    නැතහොත්
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    නම් (idx >= 0)
	set_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#endif

#නම් defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    static void
apply_optionset_autocmd(
	පූර්ණ_සංඛ්‍යා	opt_idx,
	long	opt_flags,
	long	oldval,
	long	oldval_g,
	long	newval,
	var_na	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    නම් (starting || errmsg != NULL || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	ආපසු;

    len = vim_snprintf_safelen((var_na *)buf_new, parimaa_na(buf_new), "%ld", newval);
    set_vim_var_string(VV_OPTION_NEW, buf_new, (පූර්ණ_සංඛ්‍යා)len);
    len = vim_snprintf_safelen((var_na *)buf_type, parimaa_na(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (පූර්ණ_සංඛ්‍යා)len);

    buf_oldlen = vim_snprintf_safelen((var_na *)buf_old, parimaa_na(buf_old), "%ld", oldval);
    set_vim_var_string(VV_OPTION_OLD, buf_old, (පූර්ණ_සංඛ්‍යා)buf_oldlen);

    නම් (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (පූර්ණ_සංඛ්‍යා)buf_oldlen);
    }
    නම් (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (පූර්ණ_සංඛ්‍යා)buf_oldlen);
    }
    නම් ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((var_na *)buf_old_global,
	    parimaa_na(buf_old_global), "%ld", oldval_g);

	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (පූර්ණ_සංඛ්‍යා)buf_oldlen);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (පූර්ණ_සංඛ්‍යා)buf_old_globallen);
    }
    නම් (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (පූර්ණ_සංඛ්‍යා)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    NULL, FALSE, NULL);
    reset_v_option_vars();
}
#endif

#නම් defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    var_na *
did_set_arabic(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    නම් (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	නම් (!p_tbidi)
	{
	    // set rightleft mode
	    නම් (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		changed_window_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    නම් (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	නම් (STRCMP(p_enc, "utf-8") != 0)
	{
	    static var_na *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

	    msg_source(HL_ATTR(HLF_W));
	    msg_attr(_(w_arabic), HL_ATTR(HLF_W));
# ifdef FEAT_EVAL
	    set_vim_var_string(VV_WARNINGMSG, (char_u *)_(w_arabic), -1);
# endif
	}

	// set 'delcombine'
	p_deco = TRUE;

# ifdef FEAT_KEYMAP
	// Force-set the necessary keymap for arabic
	errmsg = set_option_value((char_u *)"keymap", 0L, (char_u *)"arabic",
								OPT_LOCAL);
# endif
    }
    නැතහොත්
    {
	// 'arabic' is reset, handle various sub-settings.
	නම් (!p_tbidi)
	{
	    // reset rightleft mode
	    නම් (curwin->w_p_rl)
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
	curbuf->b_p_imsearch = B_IMODE_USE_INSERT;
# endif
    }

    ආපසු errmsg;
}
#endif

#නම් defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    var_na *
did_set_autochdir(optset_T *args UNUSED)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    ආපසු NULL;
}
#endif

#නම් defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    var_na *
did_set_ballooneval(optset_T *args)
{
    නම් (balloonEvalForTerm)
	ආපසු NULL;

    නම් (p_beval && !args->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    නැතහොත් නම් (!p_beval && args->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    ආපසු NULL;
}
#endif

#නම් defined(FEAT_BEVAL_TERM)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    var_na *
did_set_balloonevalterm(optset_T *args UNUSED)
{
    mch_bevalterm_changed();
    ආපසු NULL;
}
#endif

/*
 * Process the updated 'binary' option value.
 */
    var_na *
did_set_binary(optset_T *args)
{
    // when 'bin' is set also set some other options
    set_options_bin(args->os_oldval.boolean, curbuf->b_p_bin, args->os_flags);
    redraw_titles();

    ආපසු NULL;
}

/*
 * Process the updated 'buflisted' option value.
 */
    var_na *
did_set_buflisted(optset_T *args)
{
    // when 'buflisted' changes, trigger autocommands
    නම් (args->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						NULL, NULL, TRUE, curbuf);
    ආපසු NULL;
}

/*
 * Process the new 'cmdheight' option value.
 */
    var_na *
did_set_cmdheight(optset_T *args)
{
    long old_value = args->os_oldval.number;
    var_na *errmsg = NULL;

    // if p_ch changed value, change the command line height
    නම් (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_argument_must_be_positive;
	p_ch = MIN_CMDHEIGHT;
    }
    නම් (p_ch > Rows - min_rows() + MIN_CMDHEIGHT)
	p_ch = Rows - min_rows() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    නම් ((p_ch != old_value
		|| tabline_height() + topframe->fr_height != Rows - p_ch)
	    && full_screen
#ifdef FEAT_GUI
	    && !gui.starting
#endif
       )
	command_height();

    ආපසු errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    var_na *
did_set_compatible(optset_T *args UNUSED)
{
    compatible_set();
    ආපසු NULL;
}

#නම් defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    var_na *
did_set_conceallevel(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    නම් (curwin->w_p_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_cole = 0;
    }
    නැතහොත් නම් (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_cole = 3;
    }
    නම් (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    නැතහොත් නම් (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    ආපසු errmsg;
}
#endif

#නම් defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    var_na *
did_set_diff(optset_T *args UNUSED)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    නම් (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    ආපසු NULL;
}
#endif

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    var_na *
did_set_eof_eol_fixeol_bomb(optset_T *args UNUSED)
{
    // redraw the window title and tab page text
    redraw_titles();
    ආපසු NULL;
}

/*
 * Process the updated 'equalalways' option value.
 */
    var_na *
did_set_equalalways(optset_T *args)
{
    නම් (p_ea && !args->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    ආපසු NULL;
}

#නම් defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    var_na *
did_set_foldcolumn(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    නම් (curwin->w_p_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_fdc = 0;
    }
    නැතහොත් නම් (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_fdc = 12;
    }
    නම් (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    නැතහොත් නම් (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    ආපසු errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    var_na *
did_set_foldlevel(optset_T *args UNUSED)
{
    නම් (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    ආපසු NULL;
}

/*
 * Process the new 'foldminlines' option value.
 */
    var_na *
did_set_foldminlines(optset_T *args UNUSED)
{
    foldUpdateAll(curwin);
    ආපසු NULL;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    var_na *
did_set_foldnestmax(optset_T *args UNUSED)
{
    නම් (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    ආපසු NULL;
}
#endif

#නම් defined(FEAT_SEARCH_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    var_na *
did_set_hlsearch(optset_T *args UNUSED)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    set_no_hlsearch(FALSE);
    ආපසු NULL;
}
#endif

/*
 * Process the updated 'ignorecase' option value.
 */
    var_na *
did_set_ignorecase(optset_T *args UNUSED)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    නම් (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    ආපසු NULL;
}

#නම් defined(HAVE_INPUT_METHOD)
/*
 * Process the updated 'imdisable' option value.
 */
    var_na *
did_set_imdisable(optset_T *args UNUSED)
{
    // Only de-activate it here, it will be enabled when changing mode.
    නම් (p_imdisable)
	im_set_active(FALSE);
    නැතහොත් නම් (State & MODE_INSERT)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
    ආපසු NULL;
}
#endif

/*
 * Process the new 'iminsert' option value.
 */
    var_na *
did_set_iminsert(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    නම් (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    නම් (termcap_active)	// don't do this in the alternate screen
	showmode();
#නම් defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif

    ආපසු errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    var_na *
did_set_imsearch(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    නම් (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    ආපසු errmsg;
}

#නම් defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    var_na *
did_set_imstyle(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    නම් (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_argument;

    ආපසු errmsg;
}
#endif

/*
 * Process the updated 'insertmode' option value.
 */
    var_na *
did_set_insertmode(optset_T *args)
{
    // when 'insertmode' is set from an autocommand need to do work here
    නම් (p_im)
    {
	නම් ((State & MODE_INSERT) == 0)
	    need_start_insertmode = TRUE;
	stop_insert_mode = FALSE;
    }
    // only reset if it was set previously
    නැතහොත් නම් (args->os_oldval.boolean)
    {
	need_start_insertmode = FALSE;
	stop_insert_mode = TRUE;
	නම් (restart_edit != 0 && mode_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    ආපසු NULL;
}

#නම් defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    var_na *
did_set_langnoremap(optset_T *args UNUSED)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    ආපසු NULL;
}

/*
 * Process the updated 'langremap' option value.
 */
    var_na *
did_set_langremap(optset_T *args UNUSED)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    ආපසු NULL;
}
#endif

/*
 * Process the new 'laststatus' option value.
 */
    var_na *
did_set_laststatus(optset_T *args UNUSED)
{
    last_status(FALSE);	// (re)set last window status line
    ආපසු NULL;
}

#නම් defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    var_na *
did_set_linespace(optset_T *args UNUSED)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    නම් (gui.in_use && gui_mch_adjust_charheight() == OK)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    ආපසු NULL;
}
#endif

/*
 * Process the updated 'lisp' option value.
 */
    var_na *
did_set_lisp(optset_T *args UNUSED)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (void)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    ආපසු NULL;
}

/*
 * Process the new 'maxcombine' option value.
 */
    var_na *
did_set_maxcombine(optset_T *args UNUSED)
{
    නම් (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    නැතහොත් නම් (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    ආපසු NULL;
}

/*
 * Process the updated 'modifiable' option value.
 */
    var_na *
did_set_modifiable(optset_T *args UNUSED)
{
    // when 'modifiable' is changed, redraw the window title

#ifdef FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    නම් (curbuf->b_p_ma && (term_in_normal_mode(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_term != NULL && !term_is_finished(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	ආපසු e_cannot_make_terminal_with_running_job_modifiable;
    }
#endif
    redraw_titles();

    ආපසු NULL;
}

/*
 * Process the updated 'modified' option value.
 */
    var_na *
did_set_modified(optset_T *args)
{
    නම් (!args->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_set = !!args->os_newval.boolean;
    ආපසු NULL;
}

#නම් defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    var_na *
did_set_mousehide(optset_T *args UNUSED)
{
    නම් (!p_mh)
	gui_mch_mousehide(FALSE);
    ආපසු NULL;
}
#endif

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    var_na *
did_set_number_relativenumber(optset_T *args UNUSED)
{
#නම් defined(FEAT_SIGNS) && defined(FEAT_GUI)
    නම් (gui.in_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != NULL)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	නම් (!(curwin->w_p_nu && ((පූර්ණ_සංඛ්‍යා *)args->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#endif
    ආපසු NULL;
}

#නම් defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    var_na *
did_set_numberwidth(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    // 'numberwidth' must be positive
    නම් (curwin->w_p_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_nuw = 1;
    }
    නැතහොත් නම් (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_nuw = 20;
    }
    නම් (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    නැතහොත් නම් (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_line_count = 0; // trigger a redraw

    ආපසු errmsg;
}
#endif

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    var_na *
did_set_osctimeoutlen(optset_T *args)
{
    නම් (p_ost < 0)
    {
	p_ost = args->os_oldval.number;
	ආපසු e_argument_must_be_positive;
    }

    ආපසු NULL;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    var_na *
did_set_paste(optset_T *args UNUSED)
{
    static පූර්ණ_සංඛ්‍යා	old_p_paste = FALSE;
    static පූර්ණ_සංඛ්‍යා	save_sm = 0;
    static පූර්ණ_සංඛ්‍යා	save_sta = 0;
    static පූර්ණ_සංඛ්‍යා	save_ru = 0;
#ifdef FEAT_RIGHTLEFT
    static පූර්ණ_සංඛ්‍යා	save_ri = 0;
    static පූර්ණ_සංඛ්‍යා	save_hkmap = 0;
#endif
    buf_T	*buf;

    නම් (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	නම් (!old_p_paste)
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
		නම් (buf->b_p_vsts_nopaste)
		    vim_free(buf->b_p_vsts_nopaste);
		buf->b_p_vsts_nopaste =
			buf->b_p_vsts && buf->b_p_vsts != empty_option
					   ? vim_strsave(buf->b_p_vsts) : NULL;
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
	    නම් (p_vsts_nopaste)
		vim_free(p_vsts_nopaste);
	    p_vsts_nopaste = p_vsts && p_vsts != empty_option
						  ? vim_strsave(p_vsts) : NULL;
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
	    නම් (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = empty_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#endif
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	නම් (p_ru)
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
	නම් (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = empty_option;
#endif
    }

    // Paste switched from on to off: Restore saved values.
    නැතහොත් නම් (old_p_paste)
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
	    නම් (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : empty_option;
	    vim_free(buf->b_p_vsts_array);
	    නම් (buf->b_p_vsts && buf->b_p_vsts != empty_option)
		(void)tabstop_set(buf->b_p_vsts, &buf->b_p_vsts_array);
	    නැතහොත්
		buf->b_p_vsts_array = NULL;
#endif
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	නම් (p_ru != save_ru)
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
	නම් (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : empty_option;
#endif
    }

    old_p_paste = p_paste;

#නම් defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#endif

    ආපසු NULL;
}


#ifdef FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    var_na *
did_set_previewwindow(optset_T *args UNUSED)
{
    නම් (!curwin->w_p_pvw)
	ආපසු NULL;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	නම් (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    ආපසු e_preview_window_already_exists;
	}

    ආපසු NULL;
}
#endif

#නම් defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    var_na *
did_set_pyxversion(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    නම් (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_argument;

    ආපසු errmsg;
}
#endif

/*
 * Process the updated 'readonly' option value.
 */
    var_na *
did_set_readonly(optset_T *args)
{
    // when 'readonly' is reset globally, also reset readonlymode
    නම් (!curbuf->b_p_ro && (args->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    නම් (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    ආපසු NULL;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    var_na *
did_set_scrollbind(optset_T *args UNUSED)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    නම් (!curwin->w_p_scb)
	ආපසු NULL;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    ආපසු NULL;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    var_na *
did_set_maxsearchcount(optset_T *args UNUSED)
{
    var_na	*errmsg = NULL;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#define MAX_SEARCH_COUNT 9999

    නම් (p_msc <= 0)
	errmsg = e_argument_must_be_positive;
    නැතහොත් නම් (p_msc > MAX_SEARCH_COUNT)
	errmsg = e_invalid_argument;

    නම් (errmsg != NULL)
	p_msc = 99;

    ආපසු errmsg;
#undef MAX_SEARCH_COUNT
}

#නම් defined(BACKSLASH_IN_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    var_na *
did_set_shellslash(optset_T *args UNUSED)
{
    නම් (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    නැතහොත්
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
    ආපසු NULL;
}
#endif

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    var_na *
did_set_shiftwidth_tabstop(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    var_na	*errmsg = NULL;

    නම් (curbuf->b_p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_ts = 8;
    }
    නැතහොත් නම් (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_ts = 8;
    }
    නම් (p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ts = 8;
    }
    නැතහොත් නම් (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	p_ts = 8;
    }

    නම් (curbuf->b_p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_count(curbuf->b_p_vts_array) > 0
		       ? tabstop_first(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#නැතහොත්
	curbuf->b_p_sw = curbuf->b_p_ts;
#endif
    }
    නම් (p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_count(curbuf->b_p_vts_array) > 0
	     ? tabstop_first(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#නැතහොත්
	p_sw = curbuf->b_p_ts;
#endif
    }

#ifdef FEAT_FOLDING
    නම් (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#endif
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    නම් (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    ආපසු errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    var_na *
did_set_showtabline(optset_T *args UNUSED)
{
    shell_new_rows();	// recompute window positions and heights
    ආපසු NULL;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    var_na *
did_set_smoothscroll(optset_T *args UNUSED)
{
    නම් (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    ආපසු NULL;
}

#නම් defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    var_na *
did_set_spell(optset_T *args UNUSED)
{
    නම් (curwin->w_p_spell)
	ආපසු parse_spelllang(curwin);

    ආපසු NULL;
}
#endif

/*
 * Process the updated 'swapfile' option value.
 */
    var_na *
did_set_swapfile(optset_T *args UNUSED)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    නම් (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    නැතහොත්
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    ආපසු NULL;
}

#නම් defined(FEAT_TERMGUICOLORS)
    var_na *
did_set_termguicolors(optset_T *args UNUSED)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    නම් (
#  ifdef VIMDLL
	    !gui.in_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_set = TRUE;
	p_tgc = 0;
	ආපසු e_24_bit_colors_are_not_supported_on_this_environment;
    }
    නම් (is_term_win32())
	swap_tcap();
# endif
# ifdef FEAT_GUI
    නම් (!gui.in_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    නම් (is_term_win32())
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

    ආපසු NULL;
}
#endif

    var_na *
did_set_termsync(optset_T *args UNUSED)
{
    නම් (!p_tsy)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);
    ආපසු NULL;
}

#නම් defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    var_na *
did_set_termwinscroll(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    var_na	*errmsg = NULL;

    නම් (*pp < 1)
    {
	errmsg = e_argument_must_be_positive;
	*pp = 1;
    }

    ආපසු errmsg;
}
#endif

/*
 * Process the updated 'terse' option value.
 */
    var_na *
did_set_terse(optset_T *args UNUSED)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_SEARCH);

    // insert 's' in p_shm
    නම් (p_terse && p == NULL)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    නැතහොත් නම් (!p_terse && p != NULL)
	STRMOVE(p, p + 1);
    ආපසු NULL;
}

/*
 * Process the updated 'textauto' option value.
 */
    var_na *
did_set_textauto(optset_T *args)
{
    // when 'textauto' is set or reset also change 'fileformats'
    set_string_option_direct((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | args->os_flags, 0);

    ආපසු NULL;
}

/*
 * Process the updated 'textmode' option value.
 */
    var_na *
did_set_textmode(optset_T *args)
{
    // when 'textmode' is set or reset also change 'fileformat'
    set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, args->os_flags);

    ආපසු NULL;
}

/*
 * Process the new 'textwidth' option value.
 */
    var_na *
did_set_textwidth(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    නම් (curbuf->b_p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_tw = 0;
    }
    නම් (p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tw = 0;
    }
#ifdef FEAT_SYN_HL
    {
	win_T	*wp;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    check_colorcolumn(NULL, wp);
    }
#endif

    ආපසු errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    var_na *
did_set_title_icon(optset_T *args UNUSED)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_set_title();
    ආපසු NULL;
}

/*
 * Process the new 'titlelen' option value.
 */
    var_na *
did_set_titlelen(optset_T *args)
{
    long old_value = args->os_oldval.number;
    var_na *errmsg = NULL;

    // if 'titlelen' has changed, redraw the title
    නම් (p_titlelen < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_titlelen = 85;
    }
    නම් (starting != NO_SCREEN && old_value != p_titlelen)
	need_maketitle = TRUE;

    ආපසු errmsg;
}

#නම් defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    var_na *
did_set_undofile(optset_T *args)
{
    // Only take action when the option was set.
    නම් (!curbuf->b_p_udf && !p_udf)
	ආපසු NULL;

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
	නම් ((curbuf == save_curbuf
		    || (args->os_flags & OPT_GLOBAL)
		    || args->os_flags == 0)
		&& !curbufIsChanged() && curbuf->b_ml.ml_mfp != NULL)
	{
# ifdef FEAT_CRYPT
	    නම් (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
		continue;
# endif
	    u_compute_hash(hash);
	    u_read_undo(NULL, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    ආපසු NULL;
}
#endif

/*
 * Process the new global 'undolevels' option value.
 */
    static void
update_global_undolevels(long value, long old_value)
{
    // sync undo before 'undolevels' changes

    // use the old value, otherwise u_sync() may not work properly
    p_ul = old_value;
    u_sync(TRUE);
    p_ul = value;
}

/*
 * Process the new buffer local 'undolevels' option value.
 */
    static void
update_buflocal_undolevels(long value, long old_value)
{
    // use the old value, otherwise u_sync() may not work properly
    curbuf->b_p_ul = old_value;
    u_sync(TRUE);
    curbuf->b_p_ul = value;
}

/*
 * Process the new 'undolevels' option value.
 */
    var_na *
did_set_undolevels(optset_T *args)
{
    long *pp = (long *)args->os_varp;

    නම් (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(args->os_newval.number,
						       args->os_oldval.number);
    නැතහොත් නම් (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(args->os_newval.number,
						       args->os_oldval.number);

    ආපසු NULL;
}

/*
 * Process the new 'updatecount' option value.
 */
    var_na *
did_set_updatecount(optset_T *args)
{
    long old_value = args->os_oldval.number;
    var_na *errmsg = NULL;

    // when 'updatecount' changes from zero to non-zero, open swap files
    නම් (p_uc < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_uc = 100;
    }
    නම් (p_uc && !old_value)
	ml_open_files();

    ආපසු errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    var_na *
did_set_weirdinvert(optset_T *args)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    නම් (p_wiv && !args->os_oldval.boolean)
	T_XS = (char_u *)"y";
    නැතහොත් නම් (!p_wiv && args->os_oldval.boolean)
	T_XS = empty_option;
    p_wiv = (*T_XS != NUL);

    ආපසු NULL;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    var_na *
did_set_wildchar(optset_T *args)
{
    long c = *(long *)args->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    නම් (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	ආපසු e_invalid_argument;

    ආපසු NULL;
}

/*
 * Process the new 'window' option value.
 */
    var_na *
did_set_window(optset_T *args UNUSED)
{
    නම් (p_window < 1)
	p_window = 1;
    නැතහොත් නම් (p_window >= Rows)
	p_window = Rows - 1;
    ආපසු NULL;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    var_na *
did_set_winheight_helpheight(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    var_na	*errmsg = NULL;

    නම් (p_wh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wh = 1;
    }
    නම් (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    නම් (p_hh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hh = 0;
    }

    // Change window height NOW
    නම් (!ONE_WINDOW)
    {
	නම් (pp == &p_wh && curwin->w_height < p_wh)
	    win_setheight((පූර්ණ_සංඛ්‍යා)p_wh);
	නම් (pp == &p_hh && curbuf->b_help && curwin->w_height < p_hh)
	    win_setheight((පූර්ණ_සංඛ්‍යා)p_hh);
    }

    ආපසු errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    var_na *
did_set_winminheight(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    නම් (p_wmh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmh = 0;
    }
    නම් (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    ආපසු errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    var_na *
did_set_winminwidth(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    නම් (p_wmw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmw = 0;
    }
    නම් (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    ආපසු errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    var_na *
did_set_winwidth(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    නම් (p_wiw < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wiw = 1;
    }
    නම් (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    නම් (!ONE_WINDOW && curwin->w_width < p_wiw)
	win_setwidth((පූර්ණ_සංඛ්‍යා)p_wiw);

    ආපසු errmsg;
}

#නම් defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    var_na *
did_set_wlsteal(optset_T *args UNUSED)
{
    clip_reset_wayland();

    ආපසු NULL;
}
#endif

#නම් defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    var_na *
did_set_wltimeoutlen(optset_T *args)
{
    නම් (p_wtm < 0)
    {
	p_wtm = args->os_oldval.number;
	ආපසු e_argument_must_be_positive;
    }

    ආපසු NULL;
}
#endif

/*
 * Process the updated 'wrap' option value.
 */
    var_na *
did_set_wrap(optset_T *args UNUSED)
{
    // Set w_leftcol or w_skipcol to zero.
    නම් (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    නැතහොත්
	curwin->w_skipcol = 0;

    ආපසු NULL;
}

#ifdef FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    var_na *
did_set_xhistory(optset_T *args)
{
    පූර්ණ_සංඛ්‍යා is_p_chi = (long*)args->os_varp == &p_chi;
    පූර්ණ_සංඛ්‍යා err;
    long *arg = (is_p_chi) ? &p_chi :(long*)args->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    නම් (*arg < 1)
    {
	*arg = args->os_oldval.number;
	ආපසු e_cannot_have_negative_or_zero_number_of_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    නම් (*arg > 100)
    {
	*arg = args->os_oldval.number;
	ආපසු e_cannot_have_more_than_hundred_quickfix;
    }

    නම් (is_p_chi)
	err = qf_resize_stack(*arg);
    නැතහොත්
	err = ll_resize_stack(curwin, *arg);

    නම් (err == FAIL)
	ආපසු e_failed_resizing_quickfix_stack;

    ආපසු NULL;
}
#endif

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    static var_na *
set_bool_option(
    පූර්ණ_සංඛ්‍යා		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    පූර්ණ_සංඛ්‍යා		value,			// new value
    පූර්ණ_සංඛ්‍යා		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    පූර්ණ_සංඛ්‍යා		old_value = *(පූර්ණ_සංඛ්‍යා *)varp;
#නම් defined(FEAT_EVAL)
    පූර්ණ_සංඛ්‍යා		old_global_value = 0;
#endif
    var_na	*errmsg = NULL;

    // Disallow changing some options from secure mode
    නම් ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	ආපසු e_not_allowed_here;

#නම් defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    නම් ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_value = *(පූර්ණ_සංඛ්‍යා *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *(පූර්ණ_සංඛ්‍යා *)varp = value;	    // set the new value
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    නම් ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(පූර්ණ_සංඛ්‍යා *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = value;

    // Handle side effects of changing a bool option.
    නම් (options[opt_idx].opt_did_set_cb != NULL)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.boolean = old_value;
	args.os_newval.boolean = value;
	args.os_errbuf = NULL;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
	නම් (errmsg != NULL)
	    ආපසු errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_SET;

#නම් defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(long)(old_value ? TRUE : FALSE),
				(long)(old_global_value ? TRUE : FALSE),
				(long)(value ? TRUE : FALSE), NULL);
#endif

    comp_col();			    // in case 'ruler' or 'showcmd' changed

    නම් (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    නම් ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    ආපසු errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    static var_na *
check_num_option_bounds(
    long	*pp,
    long	old_value,
    long	old_Rows,
    long	old_Columns,
    var_na	*errbuf,
    size_t	errbuflen,
    var_na	*errmsg)
{
    නම් (Rows < min_rows_for_all_tabpages() && full_screen)
    {
	නම් (errbuf != NULL)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_lines), min_rows_for_all_tabpages());
	    errmsg = errbuf;
	}
	Rows = min_rows_for_all_tabpages();
    }
    නම් (Columns < MIN_COLUMNS && full_screen)
    {
	නම් (errbuf != NULL)
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
    නම් (old_Rows != Rows || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	නම් (updating_screen)
	    *pp = old_value;
	නැතහොත් නම් (full_screen
#ifdef FEAT_GUI
		&& !gui.starting
#endif
		)
	    set_shellsize((පූර්ණ_සංඛ්‍යා)Columns, (පූර්ණ_සංඛ්‍යා)Rows, TRUE);
	නැතහොත්
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    නම් (cmdline_row > Rows - p_ch && Rows > p_ch)
		cmdline_row = Rows - p_ch;
	}
	නම් (p_window >= Rows || !option_was_set((char_u *)"window"))
	    p_window = Rows - 1;
    }

    නම් (p_tm < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tm = 0;
    }
    නම් ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_height
		    && curwin->w_height > 0))
	    && full_screen)
    {
	නම් (pp == &(curwin->w_p_scr))
	{
	    නම් (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_size;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	නැතහොත් නම් (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	නැතහොත් // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_height;
    }
    නම් (p_hi < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hi = 0;
    }
    නැතහොත් නම් (p_hi > 10000)
    {
	errmsg = e_invalid_argument;
	p_hi = 10000;
    }
    නම් (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_argument;
	p_re = 0;
    }
    නම් (p_report < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_report = 1;
    }
    නම් ((p_sj < -100 || p_sj >= Rows) && full_screen)
    {
	නම් (Rows != old_Rows)	// Rows changed, just adjust p_sj
	    p_sj = Rows / 2;
	නැතහොත්
	{
	    errmsg = e_invalid_scroll_size;
	    p_sj = 1;
	}
    }
    නම් (p_so < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_so = 0;
    }
    නම් (p_sop < 0 && full_screen)
    {
	errmsg = e_invalid_argument;
	p_sop = 0;
    }
    නම් (p_siso < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_siso = 0;
    }
    නම් (p_cwh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_cwh = 1;
    }
    නම් (p_ut < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ut = 2000;
    }
    නම් (p_ss < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ss = 0;
    }

    ආපසු errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    static var_na *
set_num_option(
    පූර්ණ_සංඛ්‍යා		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    long	value,			// new value
    var_na	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    පූර්ණ_සංඛ්‍යා		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    var_na	*errmsg = NULL;
    long	old_value = *(long *)varp;
#නම් defined(FEAT_EVAL)
    long	old_global_value = 0;	// only used when setting a local and
					// global option
#endif
    long	old_Rows = Rows;	// remember old Rows
    long	old_Columns = Columns;	// remember old Columns
    long	*pp = (long *)varp;

    // Disallow changing some options from secure mode.
    නම් ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	ආපසු e_not_allowed_here;

#නම් defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    නම් ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_value = *(long *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *pp = value;
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    නම් ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = value;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    නම් (options[opt_idx].opt_did_set_cb != NULL)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.number = old_value;
	args.os_newval.number = value;
	args.os_errbuf = NULL;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
    }

    // Check the bounds for numeric options here
    errmsg = check_num_option_bounds(pp, old_value, old_Rows, old_Columns,
						errbuf, errbuflen, errmsg);

    options[opt_idx].flags |= P_WAS_SET;

#නම් defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_value, old_global_value,
								value, errmsg);
#endif

    comp_col();			    // in case 'columns' or 'ls' changed

    නම් (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    නම් ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    ආපසු errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    void
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    පූර්ණ_සංඛ්‍යා		doclear = (flags & P_RCLR) == P_RCLR;
    පූර්ණ_සංඛ්‍යා		all = ((flags & P_RALL) == P_RALL || doclear);

    නම් ((flags & P_RSTAT) || all)	// mark all status lines dirty
	status_redraw_all();

    නම් ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	නම් (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	නැතහොත්
	    changed_window_setting();
    }
    නම් (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    නම් (doclear)
	redraw_all_later(UPD_CLEAR);
    නැතහොත් නම් (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    පූර්ණ_සංඛ්‍යා
findoption(char_u *arg)
{
    පූර්ණ_සංඛ්‍යා		    opt_idx;
    var_na	    *s, *p;
    static short    quick_tab[27] = {0, 0};	// quick access table
    පූර්ණ_සංඛ්‍යා		    is_term_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    නම් (quick_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	පූර්ණ_සංඛ්‍යා last_opt_idx;
	සඳහා (last_opt_idx = 0; options[last_opt_idx].fullname != NULL;
		last_opt_idx++)
	    ;
	සඳහා (පූර්ණ_සංඛ්‍යා tab_idx = 1; tab_idx < 27; tab_idx++)
	    quick_tab[tab_idx] = last_opt_idx;

	p = options[0].fullname;
	සඳහා (opt_idx = 1; opt_idx < last_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    නම් (s[0] != p[0])
	    {
		නම් (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		නැතහොත්
		    quick_tab[CharOrdLow(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    නම් (arg[0] < 'a' || arg[0] > 'z')
	ආපසු -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    නම් (is_term_opt)
	opt_idx = quick_tab[26];
    නැතහොත්
	opt_idx = quick_tab[CharOrdLow(arg[0])];
    සඳහා (; (s = options[opt_idx].fullname) != NULL && s[0] == arg[0]; opt_idx++)
    {
	නම් (STRCMP(arg, s) == 0)		    // match full name
	    bha_nga;
    }
    නම් (s != NULL && s[0] != arg[0])
	s = NULL;
    නම් (s == NULL && !is_term_opt)
    {
	opt_idx = quick_tab[CharOrdLow(arg[0])];
	සඳහා ( ; options[opt_idx].fullname != NULL; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    නම් (s != NULL && STRCMP(arg, s) == 0)   // match short name
		bha_nga;
	    s = NULL;
	}
    }
    නම් (s == NULL)
	opt_idx = -1;
    ආපසු opt_idx;
}

#නම් defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
get_option_value(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only checking existence
    පූර්ණ_සංඛ්‍යා		*flagsp,
    පූර්ණ_සංඛ්‍යා		scope)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    නම් (opt_idx < 0)		    // option not in the table
    {
	පූර්ණ_සංඛ්‍යා key;

	නම් (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (key = find_key_option(name, FALSE)) != 0)
	{
	    char_u key_name[2];
	    char_u *p;

	    නම් (flagsp != NULL)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    නම් (key < 0)
	    {
		key_name[0] = KEY2TERMCAP0(key);
		key_name[1] = KEY2TERMCAP1(key);
	    }
	    නැතහොත්
	    {
		key_name[0] = KS_KEY;
		key_name[1] = (key & 0xff);
	    }
	    p = find_termcode(key_name);
	    නම් (p != NULL)
	    {
		නම් (stringval != NULL)
		    *stringval = vim_strsave(p);
		ආපසු gov_string;
	    }
	}
	ආපසු gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    නම් (flagsp != NULL)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    නම් (options[opt_idx].flags & P_STRING)
    {
	නම් (varp == NULL)		    // hidden option
	    ආපසු gov_hidden_string;
	නම් (stringval != NULL)
	{
	    නම් ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    නැතහොත් නම් ((char_u **)varp == &curbuf->b_p_key
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    නැතහොත්
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	ආපසු gov_string;
    }

    නම් (varp == NULL)		    // hidden option
	ආපසු (options[opt_idx].flags & P_NUM)
					 ? gov_hidden_number : gov_hidden_bool;
    නම් (options[opt_idx].flags & P_NUM)
	*numval = *(long *)varp;
    නැතහොත්
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	නම් ((පූර්ණ_සංඛ්‍යා *)varp == &curbuf->b_changed)
	    *numval = curbufIsChanged();
	නැතහොත්
	    *numval = (long) *(පූර්ණ_සංඛ්‍යා *)varp;
    }
    ආපසු (options[opt_idx].flags & P_NUM) ? gov_number : gov_bool;
}
#endif

#නම් defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    පූර්ණ_සංඛ්‍යා
get_option_value_strict(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    පූර්ණ_සංඛ්‍යා		opt_type,
    void	*from)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;
    char_u	*varp = NULL;
    sa_nracanaa vimoption *p;
    පූර්ණ_සංඛ්‍යා		r = 0;

    opt_idx = findoption(name);
    නම් (opt_idx < 0)
	ආපසු 0;

    p = &(options[opt_idx]);

    // Hidden option
    නම් (p->var == NULL)
	ආපසු 0;

    නම් (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    නැතහොත් නම් (p->flags & P_NUM)
	r |= SOPT_NUM;
    නැතහොත් නම් (p->flags & P_STRING)
	r |= SOPT_STRING;

    නම් (p->indir == PV_NONE)
    {
	නම් (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	නැතහොත්
	    ආපසු 0; // Did not request global-only option
    }
    නැතහොත්
    {
	නම් (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	නැතහොත් නම් (opt_type == SREQ_GLOBAL)
	    ආපසු 0; // Requested global option

	නම් (p->indir & PV_WIN)
	{
	    නම් (opt_type == SREQ_BUF)
		ආපසු 0; // Did not request window-local option
	    නැතහොත්
		r |= SOPT_WIN;
	}
	නැතහොත් නම් (p->indir & PV_BUF)
	{
	    නම් (opt_type == SREQ_WIN)
		ආපසු 0; // Did not request buffer-local option
	    නැතහොත්
		r |= SOPT_BUF;
	}
    }

    නම් (stringval == NULL)
	ආපසු r;

    නම් (opt_type == SREQ_GLOBAL)
	varp = p->var;
    නැතහොත්
    {
	නම් (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    නම් (p->indir == PV_MOD)
	    {
		*numval = bufIsChanged((buf_T *)from);
		varp = NULL;
	    }
# ifdef FEAT_CRYPT
	    නැතහොත් නම් (p->indir == PV_KEY)
	    {
		// never return the value of the crypt key
		*stringval = NULL;
		varp = NULL;
	    }
# endif
	    නැතහොත්
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
	නැතහොත් නම් (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_buffer;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	}
	නම් (varp == p->var)
	    ආපසු (r | SOPT_UNSET);
    }

    නම් (varp != NULL)
    {
	නම් (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	නැතහොත් නම් (p->flags & P_NUM)
	    *numval = *(long *) varp;
	නැතහොත්
	    *numval = *(පූර්ණ_සංඛ්‍යා *)varp;
    }

    ආපසු r;
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
option_iter_next(void **option, පූර්ණ_සංඛ්‍යා opt_type)
{
    sa_nracanaa vimoption	*ret = NULL;
    do
    {
	නම් (*option == NULL)
	    *option = (void *) options;
	නැතහොත් නම් (((sa_nracanaa vimoption *) (*option))->fullname == NULL)
	{
	    *option = NULL;
	    ආපසු NULL;
	}
	නැතහොත්
	    *option = (void *) (((sa_nracanaa vimoption *) (*option)) + 1);

	ret = ((sa_nracanaa vimoption *) (*option));

	// Hidden option
	නම් (ret->var == NULL)
	{
	    ret = NULL;
	    continue;
	}

	switch (opt_type)
	{
	    pak_sa SREQ_GLOBAL:
		නම් (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = NULL;
		bha_nga;
	    pak_sa SREQ_BUF:
		නම් (!(ret->indir & PV_BUF))
		    ret = NULL;
		bha_nga;
	    pak_sa SREQ_WIN:
		නම් (!(ret->indir & PV_WIN))
		    ret = NULL;
		bha_nga;
	    default:
		internal_error("option_iter_next()");
		ආපසු NULL;
	}
    }
    අතරතුර (ret == NULL);

    ආපසු (char_u *)ret->fullname;
}
#endif

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(පූර්ණ_සංඛ්‍යා opt_idx)
{
    ආපසු options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    void
set_option_flag(පූර්ණ_සංඛ්‍යා opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    පූර්ණ_සංඛ්‍යා
is_global_option(පූර්ණ_සංඛ්‍යා opt_idx)
{
    ආපසු options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    පූර්ණ_සංඛ්‍යා
is_global_local_option(පූර්ණ_සංඛ්‍යා opt_idx)
{
    ආපසු options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    පූර්ණ_සංඛ්‍යා
is_window_local_option(පූර්ණ_සංඛ්‍යා opt_idx)
{
    ආපසු options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    පූර්ණ_සංඛ්‍යා
is_hidden_option(පූර්ණ_සංඛ්‍යා opt_idx)
{
    ආපසු options[opt_idx].var == NULL;
}

#නම් defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    පූර්ණ_සංඛ්‍යා
is_crypt_key_option(පූර්ණ_සංඛ්‍යා opt_idx)
{
    ආපසු options[opt_idx].indir == PV_KEY;
}
#endif

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    var_na *
set_option_value(
    char_u	*name,
    long	number,
    char_u	*string,
    පූර්ණ_සංඛ්‍යා		opt_flags)	// OPT_LOCAL or 0 (both)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;
    char_u	*varp;
    long_u	flags;
    static var_na	errbuf[ERR_BUFLEN];
    පූර්ණ_සංඛ්‍යා		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    නම් (opt_idx < 0)
    {
	පූර්ණ_සංඛ්‍යා key;

	නම් (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (key = find_key_option(name, FALSE)) != 0)
	{
	    char_u key_name[2];

	    නම් (key < 0)
	    {
		key_name[0] = KEY2TERMCAP0(key);
		key_name[1] = KEY2TERMCAP1(key);
	    }
	    නැතහොත්
	    {
		key_name[0] = KS_KEY;
		key_name[1] = (key & 0xff);
	    }
	    add_termcode(key_name, string, FALSE);
	    නම් (full_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    ආපසු NULL;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    නැතහොත්
    {
	flags = options[opt_idx].flags;
#ifdef HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	නම් (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_in_sandbox));
	    ආපසු NULL;
	}
#endif
	නම් (flags & P_STRING)
	    ආපසු set_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	නම් (varp != NULL)	// hidden option is not changed
	{
	    නම් (number == 0 && string != NULL)
	    {
		පූර්ණ_සංඛ්‍යා idx;

		// Either we are given a string or we are setting option
		// to zero.
		සඳහා (idx = 0; string[idx] == '0'; ++idx)
		    ;
		නම් (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_number_required_after_str_equal_str),
								 name, string);
		    ආපසු NULL;     // do nothing as we hit an error

		}
	    }
	    නම් (flags & P_NUM)
	    {
		ආපසු set_num_option(opt_idx, varp, number,
					   errbuf, parimaa_na(errbuf), opt_flags);
	    }
	    නැතහොත්
		ආපසු set_bool_option(opt_idx, varp, (පූර්ණ_සංඛ්‍යා)number, opt_flags);
	}

    }
    ආපසු NULL;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    void
set_option_value_give_err(
    char_u	*name,
    long	number,
    char_u	*string,
    පූර්ණ_සංඛ්‍යා		opt_flags)	// OPT_LOCAL or 0 (both)
{
    var_na *errmsg = set_option_value(name, number, string, opt_flags);

    නම් (errmsg != NULL)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_term_code(char_u *tname)
{
    පූර්ණ_සංඛ්‍යා	    opt_idx;
    char_u  *varp;

    නම් (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	ආපසු NULL;
    නම් ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	නම් (varp != NULL)
	    varp = *(char_u **)(varp);
	ආපසු varp;
    }
    ආපසු find_termcode(tname + 2);
}

    char_u *
get_highlight_default(void)
{
    පූර්ණ_සංඛ්‍යා i;

    i = findoption((char_u *)"hl");
    නම් (i >= 0)
	ආපසු options[i].def_val[VI_DEFAULT];
    ආපසු (char_u *)NULL;
}

    char_u *
get_encoding_default(void)
{
    පූර්ණ_සංඛ්‍යා i;

    i = findoption((char_u *)"enc");
    නම් (i >= 0)
	ආපසු options[i].def_val[VI_DEFAULT];
    ආපසු (char_u *)NULL;
}

#නම් defined(FEAT_QUICKFIX)
    පූර්ණ_සංඛ්‍යා
is_option_allocated(var_na *name)
{
    පූර්ණ_සංඛ්‍යා idx = findoption((char_u *)name);

    ආපසු idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#endif

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    static පූර්ණ_සංඛ්‍යා
find_key_option(char_u *arg_arg, පූර්ණ_සංඛ්‍යා has_lt)
{
    පූර්ණ_සංඛ්‍යා		key = 0;
    පූර්ණ_සංඛ්‍යා		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    නම් (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	නම් (!has_lt || arg[4] == '>')
	    key = TERMCAP2KEY(arg[2], arg[3]);
    }
    නැතහොත් නම් (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	key = find_special_key(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_KEY | FSK_SIMPLIFY, NULL);
	නම් (modifiers)		    // can't handle modifiers here
	    key = 0;
    }
    ආපසු key;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    static void
showoptions(
    පූර්ණ_සංඛ්‍යා		all,
    පූර්ණ_සංඛ්‍යා		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    sa_nracanaa vimoption	*p;
    පූර්ණ_සංඛ්‍යා			col;
    පූර්ණ_සංඛ්‍යා			isterm;
    char_u		*varp;
    sa_nracanaa vimoption	**items;
    පූර්ණ_සංඛ්‍යා			item_count;
    පූර්ණ_සංඛ්‍යා			run;
    පූර්ණ_සංඛ්‍යා			row, rows;
    පූර්ණ_සංඛ්‍යා			cols;
    පූර්ණ_සංඛ්‍යා			i;
    පූර්ණ_සංඛ්‍යා			len;

#define INC 20
#define GAP 3

    items = ALLOC_MULT(sa_nracanaa vimoption *, OPTION_COUNT);
    නම් (items == NULL)
	ආපසු;

    // Highlight title
    නම් (all == 2)
	msg_puts_title(_("\n--- Terminal codes ---"));
    නැතහොත් නම් (opt_flags & OPT_GLOBAL)
	msg_puts_title(_("\n--- Global option values ---"));
    නැතහොත් නම් (opt_flags & OPT_LOCAL)
	msg_puts_title(_("\n--- Local option values ---"));
    නැතහොත්
	msg_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    සඳහා (run = 1; run <= 2 && !got_int; ++run)
    {
	// collect the items in items[]
	item_count = 0;
	සඳහා (p = &options[0]; p->fullname != NULL; p++)
	{
	    // apply :filter /pat/
	    නම් (message_filtered((char_u *)p->fullname))
		continue;

	    varp = NULL;
	    isterm = istermoption(p);
	    නම් ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		නම් (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    නැතහොත්
		varp = get_varp(p);
	    නම් (varp != NULL
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		නම් (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		නැතහොත් නම් (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		නැතහොත්
		{
		    option_value2string(p, opt_flags);
		    len = (පූර්ණ_සංඛ්‍යා)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		නම් ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    items[item_count++] = p;
	    }
	}

	// display the items
	නම් (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    නම් (cols == 0)
		cols = 1;
	    rows = (item_count + cols - 1) / cols;
	}
	නැතහොත්	// run == 2
	    rows = item_count;
	සඳහා (row = 0; row < rows && !got_int; ++row)
	{
	    msg_putchar('\n');			// go to next line
	    නම් (got_int)			// 'q' typed in more
		bha_nga;
	    col = 0;
	    සඳහා (i = row; i < item_count; i += rows)
	    {
		msg_col = col;			// make columns
		showoneopt(items[i], opt_flags);
		col += INC;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(items);
}

/*
 * Return TRUE if option "p" has its default value.
 */
    static පූර්ණ_සංඛ්‍යා
optval_default(sa_nracanaa vimoption *p, char_u *varp, පූර්ණ_සංඛ්‍යා compatible)
{
    පූර්ණ_සංඛ්‍යා		dvi;

    නම් (varp == NULL)
	ආපසු TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    නම් (p->flags & P_NUM)
	ආපසු (*(long *)varp == (long)(long_i)p->def_val[dvi]);
    නම් (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	ආපසු (*(පූර්ණ_සංඛ්‍යා *)varp == (පූර්ණ_සංඛ්‍යා)(long)(long_i)p->def_val[dvi]);
    // P_STRING
    ආපසු (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    static void
showoneopt(
    sa_nracanaa vimoption	*p,
    පූර්ණ_සංඛ්‍යා			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    පූර්ණ_සංඛ්‍යා		save_silent = silent_mode;

    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    නම් ((p->flags & P_BOOL) && ((පූර්ණ_සංඛ්‍යා *)varp == &curbuf->b_changed
					? !curbufIsChanged() : !*(පූර්ණ_සංඛ්‍යා *)varp))
	msg_puts("no");
    නැතහොත් නම් ((p->flags & P_BOOL) && *(පූර්ණ_සංඛ්‍යා *)varp < 0)
	msg_puts("--");
    නැතහොත්
	msg_puts("  ");
    msg_puts(p->fullname);
    නම් (!(p->flags & P_BOOL))
    {
	msg_putchar('=');
	// put value string in NameBuff
	option_value2string(p, opt_flags);
	msg_outtrans(NameBuff);
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
    පූර්ණ_සංඛ්‍යා
makeset(FILE *fd, පූර්ණ_සංඛ්‍යා opt_flags, පූර්ණ_සංඛ්‍යා local_only)
{
    sa_nracanaa vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = NULL;	// fresh value
    var_na		*cmd;
    පූර්ණ_සංඛ්‍යා			round;
    පූර්ණ_සංඛ්‍යා			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    සඳහා (pri = 1; pri >= 0; --pri)
    {
      සඳහා (p = &options[0]; !istermoption(p); p++)
	නම් (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    නම් (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		continue;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    නම් ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		continue;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    නම් ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		continue;

	    නම් ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		continue;

	    round = 2;
	    නම් (p->indir != PV_NONE)
	    {
		නම් (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    නම් (!(opt_flags & OPT_LOCAL))
			continue;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    නම් (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			නම් (!optval_default(p, varp_fresh, p_cp))
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
	    සඳහා ( ; round <= 2; varp = varp_local, ++round)
	    {
		නම් (round == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		නැතහොත්
		    cmd = "setlocal";

		නම් (p->flags & P_BOOL)
		{
		    නම් (put_setbool(fd, cmd, p->fullname, *(පූර්ණ_සංඛ්‍යා *)varp) == FAIL)
			ආපසු FAIL;
		}
		නැතහොත් නම් (p->flags & P_NUM)
		{
		    නම් (put_setnum(fd, cmd, p->fullname, (long *)varp) == FAIL)
			ආපසු FAIL;
		}
		නැතහොත්    // P_STRING
		{
		    පූර්ණ_සංඛ්‍යා		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    නම් ((p->flags & P_FUNC) && *(char_u **)varp != NULL
			    && strstr(*(var_na **)varp, "<lambda>") != NULL)
			continue;

#ifdef FEAT_EVAL
		    legacy = !is_option_value_vim9(p - &options[0],
			    round == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#නැතහොත්
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#endif

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    නම් (
#නම් defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#endif
			    p->indir == PV_FT)
		    {
			නම් (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    ආපසු FAIL;
			do_endif = TRUE;
		    }
		    නම් (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			ආපසු FAIL;
		    නම් (do_endif)
		    {
			නම් (put_line(fd, "endif") == FAIL)
			    ආපසු FAIL;
		    }
		}
	    }
	}
    }
    ආපසු OK;
}

#නම් defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    පූර්ණ_සංඛ්‍යා
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_version < SCRIPT_VERSION_VIM9;
# endif

    නම් (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	ආපසු FAIL;

    ආපසු OK;
}
#endif

    static පූර්ණ_සංඛ්‍යා
put_setstring(
    FILE	*fd,
    bool	legacy,
    var_na	*cmd,
    var_na	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = NULL;
    char_u	*part = NULL;
    char_u	*p;

    නම් (legacy && fprintf(fd, "legacy ") < 0)
	ආපසු FAIL;
    නම් (fprintf(fd, "%s %s=", cmd, name) < 0)
	ආපසු FAIL;
    නම් (*valuep != NULL)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	නම් (valuep == &p_pt)
	{
	    s = *valuep;
	    අතරතුර (*s != NUL)
		නම් (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    ආපසු FAIL;
	}
	// expand the option value, replace $HOME by ~
	නැතහොත් නම් ((flags & P_EXPAND) != 0)
	{
	    පූර්ණ_සංඛ්‍යා  size = (පූර්ණ_සංඛ්‍යා)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(size);
	    නම් (buf == NULL)
		goto fail;
	    home_replace(NULL, *valuep, buf, size, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    නම් (size >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != NULL)
	    {
		part = alloc(size);
		නම් (part == NULL)
		    goto fail;

		// write line break to clear the option, e.g. ':set rtp='
		නම් (put_eol(fd) == FAIL)
		    goto fail;

		p = buf;
		අතරතුර (*p != NUL)
		{
		    නම් (legacy && fprintf(fd, "legacy ") < 0)
			ආපසු FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    නම් (fprintf(fd, "%s %s+=", cmd, name) < 0)
			goto fail;
		    (void)copy_option_part(&p, part, size,  ",");
		    නම් (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			goto fail;
		}
		vim_free(buf);
		vim_free(part);
		ආපසු OK;
	    }
	    නම් (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		ආපසු FAIL;
	    }
	    vim_free(buf);
	}
	නැතහොත් නම් (put_escstr(fd, *valuep, 2) == FAIL)
	    ආපසු FAIL;
    }
    නම් (put_eol(fd) < 0)
	ආපසු FAIL;
    ආපසු OK;
fail:
    vim_free(buf);
    vim_free(part);
    ආපසු FAIL;
}

    static පූර්ණ_සංඛ්‍යා
put_setnum(
    FILE	*fd,
    var_na	*cmd,
    var_na	*name,
    long	*valuep)
{
    long	wc;

    නම් (fprintf(fd, "%s %s=", cmd, name) < 0)
	ආපසු FAIL;
    නම් (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	නම් (fputs((var_na *)get_special_key_name((පූර්ණ_සංඛ්‍යා)wc, 0), fd) < 0)
	    ආපසු FAIL;
    }
    නැතහොත් නම් (fprintf(fd, "%ld", *valuep) < 0)
	ආපසු FAIL;
    නම් (put_eol(fd) < 0)
	ආපසු FAIL;
    ආපසු OK;
}

    static පූර්ණ_සංඛ්‍යා
put_setbool(
    FILE	*fd,
    var_na	*cmd,
    var_na	*name,
    පූර්ණ_සංඛ්‍යා		value)
{
    නම් (value < 0)	// global/local option using global value
	ආපසු OK;
    නම් (fprintf(fd, "%s %s%s", cmd, value ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	ආපසු FAIL;
    ආපසු OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    void
clear_termoptions(void)
{
    // Reset a few things before clearing the old options. This may cause
    // outputting a few things that the terminal doesn't understand, but the
    // screen will be cleared later, so this is OK.
    mch_setmouse(FALSE);	    // switch mouse off
    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window titles
#නම් defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    නම් (gui.starting)
	clear_xterm_clip();
#endif
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    void
free_termoptions(void)
{
    sa_nracanaa vimoption   *p;

    සඳහා (p = options; p->fullname != NULL; p++)
	නම් (istermoption(p))
	{
	    නම් (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    නම් (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = empty_option;
	    p->def_val[VI_DEFAULT] = empty_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ifdef FEAT_EVAL
	    // remember where the option was cleared
	    set_option_sctx_idx((පූර්ණ_සංඛ්‍යා)(p - options), OPT_GLOBAL, current_sctx);
#endif
	}
    clear_termcodes();
}

/*
 * Free the string for one term option, if it was allocated.
 * Set the string to empty_option and clear allocated flag.
 * "var" points to the option value.
 */
    void
free_one_termoption(char_u *var)
{
    sa_nracanaa vimoption   *p;

    සඳහා (p = &options[0]; p->fullname != NULL; p++)
	නම් (p->var == var)
	{
	    නම් (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = empty_option;
	    p->flags &= ~P_ALLOCED;
	    bha_nga;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    void
set_term_defaults(void)
{
    sa_nracanaa vimoption   *p;

    සඳහා (p = &options[0]; p->fullname != NULL; p++)
    {
	නම් (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    නම් (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    නම් (p->flags & P_ALLOCED)
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
    static පූර්ණ_සංඛ්‍යා
istermoption(sa_nracanaa vimoption *p)
{
    ආපසු (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    static පූර්ණ_සංඛ්‍යා
istermoption_idx(පූර්ණ_සංඛ්‍යා opt_idx)
{
    ආපසු istermoption(&options[opt_idx]);
}

#නම් defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    void
unset_global_local_option(char_u *name, void *from)
{
    sa_nracanaa vimoption *p;
    පූර්ණ_සංඛ්‍යා		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    නම් (opt_idx < 0)
	ආපසු;
    p = &(options[opt_idx]);

    switch ((පූර්ණ_සංඛ්‍යා)p->indir)
    {
	// global option with local value: use local value if it's been set
	pak_sa PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    bha_nga;
	pak_sa PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    bha_nga;
	pak_sa PV_PATH:
	    clear_string_option(&buf->b_p_path);
	    bha_nga;
	pak_sa PV_AC:
	    buf->b_p_ac = -1;
	    bha_nga;
	pak_sa PV_AR:
	    buf->b_p_ar = -1;
	    bha_nga;
# ifdef HAVE_FSYNC
	pak_sa PV_FS:
	    buf->b_p_fs = -1;
	    bha_nga;
# endif
	pak_sa PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    bha_nga;
	pak_sa PV_TAGS:
	    clear_string_option(&buf->b_p_tags);
	    bha_nga;
	pak_sa PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    bha_nga;
	pak_sa PV_SISO:
	    curwin->w_p_siso = -1;
	    bha_nga;
	pak_sa PV_SO:
	    curwin->w_p_so = -1;
	    bha_nga;
	pak_sa PV_SOP:
	    curwin->w_p_sop = -1;
	    bha_nga;
# ifdef FEAT_FIND_ID
	pak_sa PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    bha_nga;
	pak_sa PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    bha_nga;
# endif
	pak_sa PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    bha_nga;
	pak_sa PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    bha_nga;
# ifdef FEAT_DIFF
	pak_sa PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    bha_nga;
# endif
	pak_sa PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    bha_nga;
# ifdef FEAT_COMPL_FUNC
	pak_sa PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    bha_nga;
# endif
	pak_sa PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    bha_nga;
# ifdef FEAT_EVAL
	pak_sa PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    bha_nga;
# endif
# ifdef FEAT_QUICKFIX
	pak_sa PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    bha_nga;
	pak_sa PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    bha_nga;
	pak_sa PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    bha_nga;
	pak_sa PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    bha_nga;
# endif
# නම් defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	pak_sa PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    bha_nga;
# endif
# නම් defined(FEAT_CRYPT)
	pak_sa PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    bha_nga;
# endif
# ifdef FEAT_LINEBREAK
	pak_sa PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    bha_nga;
# endif
# ifdef FEAT_STL_OPT
	pak_sa PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    bha_nga;
	pak_sa PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    bha_nga;
# endif
	pak_sa PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    bha_nga;
	pak_sa PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    bha_nga;
	pak_sa PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    bha_nga;
	pak_sa PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    set_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      NULL, 0);
	    redraw_later(UPD_NOT_VALID);
	    bha_nga;
	pak_sa PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    set_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      NULL, 0);
	    redraw_later(UPD_NOT_VALID);
	    bha_nga;
	pak_sa PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    bha_nga;
    }
}
#endif

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    static char_u *
get_varp_scope(sa_nracanaa vimoption *p, පූර්ණ_සංඛ්‍යා scope)
{
    නම් ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	නම් (p->var == VAR_WIN)
	    ආපසු (char_u *)GLOBAL_WO(get_varp(p));
	ආපසු p->var;
    }
    නම් ((scope & OPT_LOCAL) && ((පූර්ණ_සංඛ්‍යා)p->indir & PV_BOTH))
    {
	switch ((පූර්ණ_සංඛ්‍යා)p->indir)
	{
	    pak_sa PV_FP:   ආපසු (char_u *)&(curbuf->b_p_fp);
#ifdef HAVE_FSYNC
	    pak_sa PV_FS:	ආපසු (char_u *)&(curbuf->b_p_fs);
#endif
#ifdef FEAT_EVAL
	    pak_sa PV_FFU: ආපසු (char_u *)&(curbuf->b_p_ffu);
#endif
#ifdef FEAT_QUICKFIX
	    pak_sa PV_EFM:  ආපසු (char_u *)&(curbuf->b_p_efm);
	    pak_sa PV_GEFM: ආපසු (char_u *)&(curbuf->b_p_gefm);
	    pak_sa PV_GP:   ආපසු (char_u *)&(curbuf->b_p_gp);
	    pak_sa PV_MP:   ආපසු (char_u *)&(curbuf->b_p_mp);
#endif
	    pak_sa PV_EP:   ආපසු (char_u *)&(curbuf->b_p_ep);
	    pak_sa PV_KP:   ආපසු (char_u *)&(curbuf->b_p_kp);
	    pak_sa PV_PATH: ආපසු (char_u *)&(curbuf->b_p_path);
	    pak_sa PV_AC:   ආපසු (char_u *)&(curbuf->b_p_ac);
	    pak_sa PV_AR:   ආපසු (char_u *)&(curbuf->b_p_ar);
	    pak_sa PV_TAGS: ආපසු (char_u *)&(curbuf->b_p_tags);
	    pak_sa PV_TC:   ආපසු (char_u *)&(curbuf->b_p_tc);
	    pak_sa PV_SISO: ආපසු (char_u *)&(curwin->w_p_siso);
	    pak_sa PV_SO:   ආපසු (char_u *)&(curwin->w_p_so);
	    pak_sa PV_SOP:  ආපසු (char_u *)&(curwin->w_p_sop);
#ifdef FEAT_FIND_ID
	    pak_sa PV_DEF:  ආපසු (char_u *)&(curbuf->b_p_def);
	    pak_sa PV_INC:  ආපසු (char_u *)&(curbuf->b_p_inc);
#endif
	    pak_sa PV_COT:  ආපසු (char_u *)&(curbuf->b_p_cot);
	    pak_sa PV_DICT: ආපසු (char_u *)&(curbuf->b_p_dict);
#ifdef FEAT_DIFF
	    pak_sa PV_DIA:  ආපසු (char_u *)&(curbuf->b_p_dia);
#endif
	    pak_sa PV_TSR:  ආපසු (char_u *)&(curbuf->b_p_tsr);
#ifdef FEAT_COMPL_FUNC
	    pak_sa PV_TSRFU: ආපසු (char_u *)&(curbuf->b_p_tsrfu);
#endif
#නම් defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    pak_sa PV_BEXPR: ආපසු (char_u *)&(curbuf->b_p_bexpr);
#endif
#නම් defined(FEAT_CRYPT)
	    pak_sa PV_CM:	  ආපසු (char_u *)&(curbuf->b_p_cm);
#endif
#ifdef FEAT_LINEBREAK
	    pak_sa PV_SBR:  ආපසු (char_u *)&(curwin->w_p_sbr);
#endif
#ifdef FEAT_STL_OPT
	    pak_sa PV_STL:  ආපසු (char_u *)&(curwin->w_p_stl);
	    pak_sa PV_STLO: ආපසු (char_u *)&(curwin->w_p_stlo);
#endif
	    pak_sa PV_UL:   ආපසු (char_u *)&(curbuf->b_p_ul);
	    pak_sa PV_LW:   ආපසු (char_u *)&(curbuf->b_p_lw);
	    pak_sa PV_BKC:  ආපසු (char_u *)&(curbuf->b_p_bkc);
	    pak_sa PV_MENC: ආපසු (char_u *)&(curbuf->b_p_menc);
	    pak_sa PV_LCS:  ආපසු (char_u *)&(curwin->w_p_lcs);
	    pak_sa PV_FCS:  ආපසු (char_u *)&(curwin->w_p_fcs);
	    pak_sa PV_VE:	  ආපසු (char_u *)&(curwin->w_p_ve);

	}
	ආපසු NULL; // "cannot happen"
    }
    ආපසු get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(පූර්ණ_සංඛ්‍යා opt_idx, පූර්ණ_සංඛ්‍යා scope)
{
    ආපසු get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    static char_u *
get_varp(sa_nracanaa vimoption *p)
{
    // hidden option, always return NULL
    නම් (p->var == NULL)
	ආපසු NULL;

    switch ((පූර්ණ_සංඛ්‍යා)p->indir)
    {
	pak_sa PV_NONE:	ආපසු p->var;

	// global option with local value: use local value if it's been set
	pak_sa PV_EP:	ආපසු *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	pak_sa PV_KP:	ආපසු *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	pak_sa PV_PATH:	ආපසු *curbuf->b_p_path != NUL
				    ? (char_u *)&(curbuf->b_p_path) : p->var;
	pak_sa PV_AC:	ආපසු curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	pak_sa PV_AR:	ආපසු curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	pak_sa PV_TAGS:	ආපසු *curbuf->b_p_tags != NUL
				    ? (char_u *)&(curbuf->b_p_tags) : p->var;
	pak_sa PV_TC:	ආපසු *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	pak_sa PV_BKC:	ආපසු *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	pak_sa PV_SISO:	ආපසු curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	pak_sa PV_SO:	ආපසු curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	pak_sa PV_SOP:	ආපසු curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ifdef FEAT_FIND_ID
	pak_sa PV_DEF:	ආපසු *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	pak_sa PV_INC:	ආපසු *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#endif
	pak_sa PV_COT:	ආපසු *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	pak_sa PV_DICT:	ආපසු *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ifdef FEAT_DIFF
	pak_sa PV_DIA:	ආපසු *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#endif
	pak_sa PV_TSR:	ආපසු *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ifdef FEAT_COMPL_FUNC
	pak_sa PV_TSRFU:	ආපසු *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#endif
	pak_sa PV_FP:	ආපසු *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ifdef HAVE_FSYNC
	pak_sa PV_FS:	ආපසු curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#endif
#ifdef FEAT_EVAL
	pak_sa PV_FFU:	ආපසු *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#endif
#ifdef FEAT_QUICKFIX
	pak_sa PV_EFM:	ආපසු *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	pak_sa PV_GEFM:	ආපසු *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	pak_sa PV_GP:	ආපසු *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	pak_sa PV_MP:	ආපසු *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#endif
#නම් defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	pak_sa PV_BEXPR:	ආපසු *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#endif
#නම් defined(FEAT_CRYPT)
	pak_sa PV_CM:	ආපසු *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#endif
#ifdef FEAT_LINEBREAK
	pak_sa PV_SBR:	ආපසු *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#endif
#ifdef FEAT_STL_OPT
	pak_sa PV_STL:	ආපසු *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	pak_sa PV_STLO:	ආපසු *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#endif
	pak_sa PV_UL:	ආපසු curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	pak_sa PV_LW:	ආපසු *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	pak_sa PV_MENC:	ආපසු *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ifdef FEAT_ARABIC
	pak_sa PV_ARAB:	ආපසු (char_u *)&(curwin->w_p_arab);
#endif
	pak_sa PV_LIST:	ආපසු (char_u *)&(curwin->w_p_list);
	pak_sa PV_LCS:	ආපසු *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	pak_sa PV_FCS:	ආපසු *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	pak_sa PV_VE:	ආපසු *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ifdef FEAT_SPELL
	pak_sa PV_SPELL:	ආපසු (char_u *)&(curwin->w_p_spell);
#endif
#ifdef FEAT_SYN_HL
	pak_sa PV_CUC:	ආපසු (char_u *)&(curwin->w_p_cuc);
	pak_sa PV_CUL:	ආපසු (char_u *)&(curwin->w_p_cul);
	pak_sa PV_CULOPT:	ආපසු (char_u *)&(curwin->w_p_culopt);
	pak_sa PV_CC:	ආපසු (char_u *)&(curwin->w_p_cc);
#endif
#ifdef FEAT_DIFF
	pak_sa PV_DIFF:	ආපසු (char_u *)&(curwin->w_p_diff);
#endif
	pak_sa PV_EIW:	ආපසු (char_u *)&(curwin->w_p_eiw);
#ifdef FEAT_FOLDING
	pak_sa PV_FDC:	ආපසු (char_u *)&(curwin->w_p_fdc);
	pak_sa PV_FEN:	ආපසු (char_u *)&(curwin->w_p_fen);
	pak_sa PV_FDI:	ආපසු (char_u *)&(curwin->w_p_fdi);
	pak_sa PV_FDL:	ආපසු (char_u *)&(curwin->w_p_fdl);
	pak_sa PV_FDM:	ආපසු (char_u *)&(curwin->w_p_fdm);
	pak_sa PV_FML:	ආපසු (char_u *)&(curwin->w_p_fml);
	pak_sa PV_FDN:	ආපසු (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	pak_sa PV_FDE:	ආපසු (char_u *)&(curwin->w_p_fde);
	pak_sa PV_FDT:	ආපසු (char_u *)&(curwin->w_p_fdt);
# endif
	pak_sa PV_FMR:	ආපසු (char_u *)&(curwin->w_p_fmr);
#endif
	pak_sa PV_NU:	ආපසු (char_u *)&(curwin->w_p_nu);
	pak_sa PV_RNU:	ආපසු (char_u *)&(curwin->w_p_rnu);
#ifdef FEAT_LINEBREAK
	pak_sa PV_NUW:	ආපසු (char_u *)&(curwin->w_p_nuw);
#endif
	pak_sa PV_WFB:	ආපසු (char_u *)&(curwin->w_p_wfb);
	pak_sa PV_WFH:	ආපසු (char_u *)&(curwin->w_p_wfh);
	pak_sa PV_WFW:	ආපසු (char_u *)&(curwin->w_p_wfw);
#නම් defined(FEAT_QUICKFIX)
	pak_sa PV_PVW:	ආපසු (char_u *)&(curwin->w_p_pvw);
	pak_sa PV_LHI:	ආපසු (char_u *)&(curwin->w_p_lhi);
#endif
#ifdef FEAT_RIGHTLEFT
	pak_sa PV_RL:	ආපසු (char_u *)&(curwin->w_p_rl);
	pak_sa PV_RLC:	ආපසු (char_u *)&(curwin->w_p_rlc);
#endif
	pak_sa PV_SCROLL:	ආපසු (char_u *)&(curwin->w_p_scr);
	pak_sa PV_SMS:	ආපසු (char_u *)&(curwin->w_p_sms);
	pak_sa PV_WRAP:	ආපසු (char_u *)&(curwin->w_p_wrap);
#ifdef FEAT_LINEBREAK
	pak_sa PV_LBR:	ආපසු (char_u *)&(curwin->w_p_lbr);
	pak_sa PV_BRI:	ආපසු (char_u *)&(curwin->w_p_bri);
	pak_sa PV_BRIOPT: ආපසු (char_u *)&(curwin->w_p_briopt);
#endif
	pak_sa PV_WCR:	ආපසු (char_u *)&(curwin->w_p_wcr);
	pak_sa PV_SCBIND: ආපසු (char_u *)&(curwin->w_p_scb);
	pak_sa PV_CRBIND: ආපසු (char_u *)&(curwin->w_p_crb);
#ifdef FEAT_CONCEAL
	pak_sa PV_COCU:   ආපසු (char_u *)&(curwin->w_p_cocu);
	pak_sa PV_COLE:   ආපසු (char_u *)&(curwin->w_p_cole);
#endif
	pak_sa PV_WHL:	ආපසු (char_u *)&(curwin->w_p_whl);

#ifdef FEAT_TERMINAL
	pak_sa PV_TWK:    ආපසු (char_u *)&(curwin->w_p_twk);
	pak_sa PV_TWS:    ආපසු (char_u *)&(curwin->w_p_tws);
	pak_sa PV_TWSL:	ආපසු (char_u *)&(curbuf->b_p_twsl);
#endif

	pak_sa PV_AI:	ආපසු (char_u *)&(curbuf->b_p_ai);
	pak_sa PV_BIN:	ආපසු (char_u *)&(curbuf->b_p_bin);
	pak_sa PV_BOMB:	ආපසු (char_u *)&(curbuf->b_p_bomb);
	pak_sa PV_BH:	ආපසු (char_u *)&(curbuf->b_p_bh);
	pak_sa PV_BT:	ආපසු (char_u *)&(curbuf->b_p_bt);
	pak_sa PV_BL:	ආපසු (char_u *)&(curbuf->b_p_bl);
	pak_sa PV_CI:	ආපසු (char_u *)&(curbuf->b_p_ci);
	pak_sa PV_CIN:	ආපසු (char_u *)&(curbuf->b_p_cin);
	pak_sa PV_CINK:	ආපසු (char_u *)&(curbuf->b_p_cink);
	pak_sa PV_CINO:	ආපසු (char_u *)&(curbuf->b_p_cino);
	pak_sa PV_CINSD:	ආපසු (char_u *)&(curbuf->b_p_cinsd);
	pak_sa PV_CINW:	ආපසු (char_u *)&(curbuf->b_p_cinw);
	pak_sa PV_COM:	ආපසු (char_u *)&(curbuf->b_p_com);
	pak_sa PV_CMS:	ආපසු (char_u *)&(curbuf->b_p_cms);
	pak_sa PV_CPT:	ආපසු (char_u *)&(curbuf->b_p_cpt);
#ifdef BACKSLASH_IN_FILENAME
	pak_sa PV_CSL:	ආපසු (char_u *)&(curbuf->b_p_csl);
#endif
#ifdef FEAT_COMPL_FUNC
	pak_sa PV_CFU:	ආපසු (char_u *)&(curbuf->b_p_cfu);
	pak_sa PV_OFU:	ආපසු (char_u *)&(curbuf->b_p_ofu);
#endif
#ifdef FEAT_EVAL
	pak_sa PV_TFU:	ආපසු (char_u *)&(curbuf->b_p_tfu);
#endif
	pak_sa PV_EOF:	ආපසු (char_u *)&(curbuf->b_p_eof);
	pak_sa PV_EOL:	ආපසු (char_u *)&(curbuf->b_p_eol);
	pak_sa PV_FIXEOL:	ආපසු (char_u *)&(curbuf->b_p_fixeol);
	pak_sa PV_ET:	ආපසු (char_u *)&(curbuf->b_p_et);
	pak_sa PV_FENC:	ආපසු (char_u *)&(curbuf->b_p_fenc);
	pak_sa PV_FF:	ආපසු (char_u *)&(curbuf->b_p_ff);
	pak_sa PV_FT:	ආපසු (char_u *)&(curbuf->b_p_ft);
	pak_sa PV_FO:	ආපසු (char_u *)&(curbuf->b_p_fo);
	pak_sa PV_FLP:	ආපසු (char_u *)&(curbuf->b_p_flp);
	pak_sa PV_IMI:	ආපසු (char_u *)&(curbuf->b_p_iminsert);
	pak_sa PV_IMS:	ආපසු (char_u *)&(curbuf->b_p_imsearch);
	pak_sa PV_INF:	ආපසු (char_u *)&(curbuf->b_p_inf);
	pak_sa PV_ISK:	ආපසු (char_u *)&(curbuf->b_p_isk);
#ifdef FEAT_FIND_ID
# ifdef FEAT_EVAL
	pak_sa PV_INEX:	ආපසු (char_u *)&(curbuf->b_p_inex);
# endif
#endif
#නම් defined(FEAT_EVAL)
	pak_sa PV_INDE:	ආපසු (char_u *)&(curbuf->b_p_inde);
	pak_sa PV_INDK:	ආපසු (char_u *)&(curbuf->b_p_indk);
#endif
#ifdef FEAT_EVAL
	pak_sa PV_FEX:	ආපසු (char_u *)&(curbuf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
	pak_sa PV_KEY:	ආපසු (char_u *)&(curbuf->b_p_key);
#endif
	pak_sa PV_LISP:	ආපසු (char_u *)&(curbuf->b_p_lisp);
	pak_sa PV_LOP:	ආපසු (char_u *)&(curbuf->b_p_lop);
	pak_sa PV_ML:	ආපසු (char_u *)&(curbuf->b_p_ml);
	pak_sa PV_MPS:	ආපසු (char_u *)&(curbuf->b_p_mps);
	pak_sa PV_MA:	ආපසු (char_u *)&(curbuf->b_p_ma);
	pak_sa PV_MOD:	ආපසු (char_u *)&(curbuf->b_changed);
	pak_sa PV_NF:	ආපසු (char_u *)&(curbuf->b_p_nf);
	pak_sa PV_PI:	ආපසු (char_u *)&(curbuf->b_p_pi);
	pak_sa PV_QE:	ආපසු (char_u *)&(curbuf->b_p_qe);
	pak_sa PV_RO:	ආපසු (char_u *)&(curbuf->b_p_ro);
	pak_sa PV_SI:	ආපසු (char_u *)&(curbuf->b_p_si);
	pak_sa PV_SN:	ආපසු (char_u *)&(curbuf->b_p_sn);
	pak_sa PV_STS:	ආපසු (char_u *)&(curbuf->b_p_sts);
	pak_sa PV_SUA:	ආපසු (char_u *)&(curbuf->b_p_sua);
	pak_sa PV_SWF:	ආපසු (char_u *)&(curbuf->b_p_swf);
#ifdef FEAT_SYN_HL
	pak_sa PV_SMC:	ආපසු (char_u *)&(curbuf->b_p_smc);
	pak_sa PV_SYN:	ආපසු (char_u *)&(curbuf->b_p_syn);
#endif
#ifdef FEAT_SPELL
	pak_sa PV_SPC:	ආපසු (char_u *)&(curwin->w_s->b_p_spc);
	pak_sa PV_SPF:	ආපසු (char_u *)&(curwin->w_s->b_p_spf);
	pak_sa PV_SPL:	ආපසු (char_u *)&(curwin->w_s->b_p_spl);
	pak_sa PV_SPO:	ආපසු (char_u *)&(curwin->w_s->b_p_spo);
#endif
	pak_sa PV_SW:	ආපසු (char_u *)&(curbuf->b_p_sw);
	pak_sa PV_TS:	ආපසු (char_u *)&(curbuf->b_p_ts);
	pak_sa PV_TW:	ආපසු (char_u *)&(curbuf->b_p_tw);
	pak_sa PV_TX:	ආපසු (char_u *)&(curbuf->b_p_tx);
#ifdef FEAT_PERSISTENT_UNDO
	pak_sa PV_UDF:	ආපසු (char_u *)&(curbuf->b_p_udf);
#endif
	pak_sa PV_WM:	ආපසු (char_u *)&(curbuf->b_p_wm);
#ifdef FEAT_KEYMAP
	pak_sa PV_KMAP:	ආපසු (char_u *)&(curbuf->b_p_keymap);
#endif
#ifdef FEAT_SIGNS
	pak_sa PV_SCL:	ආපසු (char_u *)&(curwin->w_p_scl);
#endif
#ifdef FEAT_VARTABS
	pak_sa PV_VSTS:	ආපසු (char_u *)&(curbuf->b_p_vsts);
	pak_sa PV_VTS:	ආපසු (char_u *)&(curbuf->b_p_vts);
#endif
	default:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    ආපසු (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(පූර්ණ_සංඛ්‍යා opt_idx)
{
    ආපසු options[opt_idx].var;
}

#නම් defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(පූර්ණ_සංඛ්‍යා opt_idx)
{
    ආපසු (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    පූර්ණ_සංඛ්‍යා
option_info(char_u *name, dict_T *d)
{
    පූර්ණ_සංඛ්‍යා			opt_idx;
    sa_nracanaa vimoption	*p;
    පූර්ණ_සංඛ්‍යා			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    නම් ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    නම් (opt_idx < 0)
	ආපසු FAIL;

    p = &options[opt_idx];
    dict_add_string(d, "name", (char_u *)p->fullname);
    dict_add_string(d, "shortname",
		    (char_u *)(p->shortname == NULL ? "" : p->shortname));
    dict_add_bool(d, "available", p->var != NULL);
    dict_add_string(d, "type", (char_u *)((p->flags & P_BOOL) ? "bool"
				: (p->flags & P_NUM) ? "number" : "string"));
    dict_add_string(d, "scope", (char_u *)(p->indir == PV_NONE ? "global"
		: (p->indir & PV_BOTH)
		    ? ((p->indir & PV_WIN) ? "global-window" : "global-buffer")
		    : ((p->indir & PV_WIN) ? "window" : "buffer")));
    dvi = (p->flags & P_VI_DEF) ? VI_DEFAULT : VIM_DEFAULT;
    නම් (p->flags & P_BOOL)
	dict_add_bool(d, "default", (පූර්ණ_සංඛ්‍යා)(long_i)p->def_val[dvi]);
    නැතහොත් නම් (p->flags & P_NUM)
	dict_add_number(d, "default", (long)(long_i)p->def_val[dvi]);
    නැතහොත්
	dict_add_string(d, "default", p->def_val[dvi] == NULL
					? (char_u *)"" : p->def_val[dvi]);
    ආපසු OK;
}
#endif

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_set_cb_T
get_option_did_set_cb(පූර්ණ_සංඛ්‍යා opt_idx)
{
    ආපසු options[opt_idx].opt_did_set_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(void)
{
    නම් (*curbuf->b_p_ep == NUL)
	ආපසු p_ep;
    ආපසු curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(void)
{
#ifdef FEAT_EVAL
    නම් (*curbuf->b_p_ffu == NUL)
	ආපසු p_ffu;
    ආපසු curbuf->b_p_ffu;
#නැතහොත්
    ආපසු (char_u *)"";
#endif
}

/*
 * Copy options from one window to another.
 * Used when splitting a window.
 */
    void
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
    void
after_copy_winopt(win_T *wp)
{
    var_na *errmsg = update_winhighlight(wp, wp->w_p_whl);

    නම් (errmsg != NULL)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    නම් (wp->w_p_wrap)
	wp->w_leftcol = 0;
    නැතහොත්
	wp->w_skipcol = 0;
#ifdef FEAT_LINEBREAK
    briopt_check(NULL, wp);
#endif
#ifdef FEAT_SYN_HL
    fill_culopt_flags(NULL, wp);
    check_colorcolumn(NULL, wp);
#endif
    set_listchars_option(wp, wp->w_p_lcs, TRUE, NULL, 0);
    set_fillchars_option(wp, wp->w_p_fcs, TRUE, NULL, 0);
}

    static char_u *
copy_option_val(char_u *val)
{
    නම් (val == empty_option)
	ආපසු empty_option;  // no need to allocate memory
    ආපසු vim_strsave(val);
}

/*
 * Copy the options from one winopt_T to another.
 * Doesn't free the old option values in "to", use clear_winopt() for that.
 * The 'scroll' option is not copied, because it depends on the window height.
 * The 'previewwindow' option is reset, there can be only one preview window.
 */
    void
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
						    parimaa_na(to->wo_script_ctx));
#endif
    check_winopt(to);		// don't want NULL pointers
}

/*
 * Check string options in a window for a NULL value.
 */
    static void
check_win_options(win_T *win)
{
    check_winopt(&win->w_onebuf_opt);
    check_winopt(&win->w_allbuf_opt);
}

/*
 * Check for NULL pointers in a winopt_T and replace them with empty_option.
 */
    static void
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
    void
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
static පූර්ණ_සංඛ්‍යා buf_opt_idx[BV_COUNT];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    static void
init_buf_opt_idx(void)
{
    static පූර්ණ_සංඛ්‍යා did_init_buf_opt_idx = FALSE;
    පූර්ණ_සංඛ්‍යා i;

    නම් (did_init_buf_opt_idx)
	ආපසු;
    did_init_buf_opt_idx = TRUE;
    සඳහා (i = 0; !istermoption_idx(i); i++)
	නම් (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_MASK] = i;
}
#නැතහොත්
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
    void
buf_copy_options(buf_T *buf, පූර්ණ_සංඛ්‍යා flags)
{
    පූර්ණ_සංඛ්‍යා		should_copy = TRUE;
    char_u	*save_p_isk = NULL;	    // init for GCC
    පූර්ණ_සංඛ්‍යා		dont_do_help;
    පූර්ණ_සංඛ්‍යා		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    නම් (p_cpo != NULL)
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
	නම් ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == NULL || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != NULL)))
	    should_copy = FALSE;

	නම් (should_copy || (flags & BCO_ALWAYS))
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
	    නම් (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = NULL;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    නම් (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		switch (*p_ffs)
		{
		    pak_sa 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); bha_nga;
		    pak_sa 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); bha_nga;
		    pak_sa 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); bha_nga;
		    default:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		නම් (buf->b_p_ff != NULL)
		    buf->b_start_ffc = *buf->b_p_ff;
		buf->b_p_bh = empty_option;
		buf->b_p_bt = empty_option;
	    }
	    නැතහොත්
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
	    නම් (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    නැතහොත්
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
	    නම් (p_vsts && p_vsts != empty_option)
		(void)tabstop_set(p_vsts, &buf->b_p_vsts_array);
	    නැතහොත්
		buf->b_p_vsts_array = NULL;
	    buf->b_p_vsts_nopaste = p_vsts_nopaste
				 ? vim_strsave(p_vsts_nopaste) : NULL;
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
	    (void)compile_cap_prog(&buf->b_s);
	    buf->b_s.b_p_spf = vim_strsave(p_spf);
	    COPY_OPT_SCTX(buf, BV_SPF);
	    buf->b_s.b_p_spl = vim_strsave(p_spl);
	    COPY_OPT_SCTX(buf, BV_SPL);
	    buf->b_s.b_p_spo = vim_strsave(p_spo);
	    COPY_OPT_SCTX(buf, BV_SPO);
#endif
#නම් defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#endif
	    buf->b_p_fp = empty_option;
#නම් defined(FEAT_EVAL)
	    buf->b_p_fex = vim_strsave(p_fex);
	    COPY_OPT_SCTX(buf, BV_FEX);
	    COPY_OPT_INSECURE(buf->b_p_fex_flags, BV_FEX);
#endif
#ifdef FEAT_CRYPT
	    buf->b_p_key = vim_strsave(p_key);
	    COPY_OPT_SCTX(buf, BV_KEY);
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
#නම් defined(FEAT_EVAL)
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
#නම් defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = empty_option;
#endif
#නම් defined(FEAT_CRYPT)
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
	    නම් (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ifdef FEAT_VARTABS
		නම් (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (void)tabstop_set(p_vts, &buf->b_p_vts_array);
		නැතහොත්
		    buf->b_p_vts_array = NULL;
#endif
	    }
	    නැතහොත්
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#ifdef FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		නම් (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (void)tabstop_set(p_vts, &buf->b_p_vts_array);
		නැතහොත්
		    buf->b_p_vts_array = NULL;
#endif
		buf->b_help = false;
		නම් (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	නම් (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    නම් (did_isk)
	(void)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    void
reset_modifiable(void)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    නම් (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}

/*
 * Set the global value for 'iminsert' to the local value.
 */
    void
set_iminsert_global(void)
{
    p_iminsert = curbuf->b_p_iminsert;
}

/*
 * Set the global value for 'imsearch' to the local value.
 */
    void
set_imsearch_global(void)
{
    p_imsearch = curbuf->b_p_imsearch;
}

static පූර්ණ_සංඛ්‍යා expand_option_idx = -1;
static පූර්ණ_සංඛ්‍යා expand_option_start_col = 0;
static char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
static පූර්ණ_සංඛ්‍යා expand_option_flags = 0;
static පූර්ණ_සංඛ්‍යා expand_option_append = FALSE;

    void
set_context_in_set_cmd(
    expand_T	*xp,
    char_u	*arg,
    පූර්ණ_සංඛ්‍යා		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    පූර්ණ_සංඛ්‍යා		nextchar;
    long_u	flags = 0;	// init for GCC
    පූර්ණ_සංඛ්‍යා		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    පූර්ණ_සංඛ්‍යා		is_term_option = FALSE;
    පූර්ණ_සංඛ්‍යා		key;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    නම් (*arg == NUL)
    {
	xp->xp_pattern = arg;
	ආපසු;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    නම් (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_pattern = p + 1;
	ආපසු;
    }
    අතරතුර (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	නම් (*p == ' ' || *p == ',')
	{
	    අතරතුර (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	නම් (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    bha_nga;
	}
	--p;
    }
    නම් (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_NO;
	p += 2;
    }
    නැතහොත් නම් (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_INV;
	p += 3;
    }
    xp->xp_pattern = arg = p;
    නම් (*arg == '<')
    {
	අතරතුර (*p != '>')
	    නම් (*p++ == NUL)	    // expand terminal option name
		ආපසු;
	key = get_special_key_code(arg + 1);
	නම් (key == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    ආපසු;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(key);
	expand_option_name[3] = KEY2TERMCAP1(key);
    }
    නැතහොත්
    {
	නම් (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    නම් (*p != NUL)
		++p;
	    නම් (*p == NUL)
		ආපසු;		// expand option name
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	නැතහොත්
	{
	    // Allow * wildcard
	    අතරතුර (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    නම් (*p == NUL)
		ආපසු;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    නම් (opt_idx == -1 || options[opt_idx].var == NULL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		ආපසු;
	    }
	    flags = options[opt_idx].flags;
	    නම් (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		ආපසු;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    පූර්ණ_සංඛ්‍යා expand_option_subtract = FALSE;
    නම් ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	නම් (nextchar == '-')
	    expand_option_subtract = TRUE;
	නම් (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    නම් ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ආපසු;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    නම් (is_term_option)
	expand_option_idx = -1;
    නැතහොත්
	expand_option_idx = opt_idx;

    නම් (!is_term_option)
    {
	නම් (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    ආපසු;
	}
    }

    xp->xp_pattern = p + 1;
    expand_option_start_col = (පූර්ණ_සංඛ්‍යා)(p + 1 - xp->xp_line);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ifdef FEAT_SYN_HL
    නම් (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	ආපසු;
    }
#endif
    නම් (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	ආපසු;
    }
#ifdef FEAT_KEYMAP
    නම් (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	ආපසු;
    }
#endif

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    නම් (expand_option_subtract)
    {
	xp->xp_context = EXPAND_SETTING_SUBTRACT;
	ආපසු;
    }
    නැතහොත් නම් (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != NULL)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    නැතහොත් නම් (*xp->xp_pattern == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	ආපසු;
    }
    නැතහොත්
	xp->xp_context = EXPAND_NOTHING;

    නම් (is_term_option || (flags & P_NUM))
	ආපසු;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    නම් (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	නම් (p == (char_u *)&p_bdir
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
	    නම් (p == (char_u *)&p_path || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    නැතහොත්
		xp->xp_backslash = XP_BS_ONE;
	}
	නැතහොත්
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    නම් (p == (char_u *)&p_tags)
		xp->xp_backslash = XP_BS_THREE;
	    නැතහොත්
		xp->xp_backslash = XP_BS_ONE;
	}
	නම් (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    නම් ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	සඳහා (p = argend - 1; p >= xp->xp_pattern; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    නම් (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		අතරතුර (s > xp->xp_pattern && *(s - 1) == '\\')
		    --s;
		නම් ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#නම් defined(BACKSLASH_IN_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#නැතහොත්
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#endif
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_pattern = p + 1;
		    bha_nga;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    නම් (flags & P_FLAGLIST)
	xp->xp_pattern = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ifdef FEAT_SPELL
    නම් (options[opt_idx].var == (char_u *)&p_sps)
    {
	නම් (STRNCMP(xp->xp_pattern, "file:", 5) == 0)
	{
	    xp->xp_pattern += 5;
	    ආපසු;
	}
	නැතහොත් නම් (options[expand_option_idx].opt_expand_cb != NULL)
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
    static පූර්ණ_සංඛ්‍යා
match_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	පූර්ණ_සංඛ්‍යා		idx,
	පූර්ණ_සංඛ්‍යා		test_only,
	පූර්ණ_සංඛ්‍යා		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    නම් (!fuzzy)
    {
	නම් (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    නම් (!test_only)
		matches[idx] = vim_strsave(str);
	    ආපසු TRUE;
	}
    }
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා score;

	score = fuzzy_match_str(str, fuzzystr);
	නම් (score != FUZZY_SCORE_NONE)
	{
	    නම් (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    ආපසු TRUE;
	}
    }

    ආපසු FALSE;
}

    පූර්ණ_සංඛ්‍යා
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    පූර්ණ_සංඛ්‍යා		*numMatches,
    char_u	***matches,
    පූර්ණ_සංඛ්‍යා		can_fuzzy)
{
    පූර්ණ_සංඛ්‍යා		num_normal = 0;	    // Nr of matching non-term-code settings
    පූර්ණ_සංඛ්‍යා		num_term = 0;	    // Nr of matching terminal code settings
    පූර්ණ_සංඛ්‍යා		opt_idx;
    පූර්ණ_සංඛ්‍යා		match;
    පූර්ණ_සංඛ්‍යා		count = 0;
    char_u	*str;
    පූර්ණ_සංඛ්‍යා		loop;
    පූර්ණ_සංඛ්‍යා		is_term_opt;
    char_u	name_buf[MAX_KEY_NAME_LEN];
    static var_na *(names[]) = {"all", "termcap"};
    පූර්ණ_සංඛ්‍යා		ic = regmatch->rm_ic;	// remember the ignore-case flag
    පූර්ණ_සංඛ්‍යා		fuzzy;
    fuzmatch_str_T  *fuzmatch = NULL;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    සඳහා (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	නම් (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    සඳහා (match = 0; match < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(names); ++match)
	    {
		නම් (match_str((char_u *)names[match], regmatch, *matches,
			    count, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    නම් (loop == 0)
			num_normal++;
		    නැතහොත්
			count++;
		}
	    }
	}
	සඳහා (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != NULL;
								    opt_idx++)
	{
	    නම් (options[opt_idx].var == NULL)
		continue;
	    නම් (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		continue;
	    is_term_opt = istermoption_idx(opt_idx);
	    නම් (is_term_opt && num_normal > 0)
		continue;

	    නම් (match_str(str, regmatch, *matches, count, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		නම් (loop == 0)
		{
		    නම් (is_term_opt)
			num_term++;
		    නැතහොත්
			num_normal++;
		}
		නැතහොත්
		    count++;
	    }
	    නැතහොත් නම් (!fuzzy && options[opt_idx].shortname != NULL
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		නම් (loop == 0)
		{
		    නම් (is_term_opt)
			num_term++;
		    නැතහොත්
			num_normal++;
		}
		නැතහොත්
		    (*matches)[count++] = vim_strsave(str);
	    }
	    නැතහොත් නම් (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		නම් (match_str(name_buf, regmatch, *matches, count, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    නම් (loop == 0)
			num_term++;
		    නැතහොත්
			count++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	නම් (xp->xp_context != EXPAND_BOOL_SETTINGS  && num_normal == 0)
	{
	    සඳහා (opt_idx = 0; (str = get_termcode(opt_idx)) != NULL; opt_idx++)
	    {
		නම් (!SAFE_isprint(str[0]) || !SAFE_isprint(str[1]))
		    continue;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		නම් (match_str(name_buf, regmatch, *matches, count,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    නම් (loop == 0)
			num_term++;
		    නැතහොත්
			count++;
		}
		නැතහොත්
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    නම් (match_str(name_buf, regmatch, *matches, count,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			නම් (loop == 0)
			    num_term++;
			නැතහොත්
			    count++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    සඳහා (opt_idx = 0; (str = get_key_name(opt_idx)) != NULL; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		නම් (match_str(name_buf, regmatch, *matches, count, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    නම් (loop == 0)
			num_term++;
		    නැතහොත්
			count++;
		}
	    }
	}
	නම් (loop == 0)
	{
	    නම් (num_normal > 0)
		*numMatches = num_normal;
	    නැතහොත් නම් (num_term > 0)
		*numMatches = num_term;
	    නැතහොත්
		ආපසු OK;
	    නම් (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *numMatches);
		නම් (*matches == NULL)
		{
		    *matches = (char_u **)"";
		    ආපසු FAIL;
		}
	    }
	    නැතහොත්
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *numMatches);
		නම් (fuzmatch == NULL)
		{
		    *matches = (char_u **)"";
		    ආපසු FAIL;
		}
	    }
	}
    }

    නම් (fuzzy &&
	fuzzymatches_to_strmatches(fuzmatch, matches, count, FALSE) == FAIL)
	ආපසු FAIL;

    ආපසු OK;
}

// Escape an option value that can be used on the command-line with :set.
// Caller needs to free the returned string, unless NULL is returned.
    static char_u*
escape_option_str_cmdline(char_u *var)
{
    char_u *buf;

    // A backslash is required before some characters.  This is the reverse of
    // what happens in do_set().
    buf = vim_strsave_escaped(var, escape_chars);
    නම් (buf == NULL)
	ආපසු NULL;

#ifdef BACKSLASH_IN_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    සඳහා (var = buf; *var != NUL; MB_PTR_ADV(var))
	නම් (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#endif
    ආපසු buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    පූර්ණ_සංඛ්‍යා
ExpandOldSetting(පූර්ණ_සංඛ්‍යා *numMatches, char_u ***matches)
{
    char_u  *var = NULL;	// init for GCC
    char_u  *buf;

    *numMatches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    නම් (*matches == NULL)
	ආපසු FAIL;

    // For a terminal key code expand_option_idx is < 0.
    නම් (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	නම් (var == NULL)
	    expand_option_idx = findoption(expand_option_name);
    }

    නම් (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    නැතහොත් නම් (var == NULL)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    නම් (buf == NULL)
    {
	VIM_CLEAR(*matches);
	ආපසු FAIL;
    }

    (*matches)[0] = buf;
    *numMatches = 1;
    ආපසු OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    පූර්ණ_සංඛ්‍යා
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    පූර්ණ_සංඛ්‍යා		*numMatches,
    char_u	***matches)
{
    char_u  *var = NULL;	// init for GCC
    char_u  *buf;

    නම් (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == NULL)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	ආපසු FAIL;
    }

    optexpand_T args;
    args.oe_varp = get_varp_scope(&options[expand_option_idx], expand_option_flags);
    args.oe_append = expand_option_append;
    args.oe_regmatch = regmatch;
    args.oe_xp = xp;
    args.oe_set_arg = xp->xp_line + expand_option_start_col;
    args.oe_include_orig_val =
	!expand_option_append &&
	(*args.oe_set_arg == NUL);

    // Retrieve the existing value, but escape it as a reverse of setting it.
    // We technically only need to do this when oe_append or
    // oe_include_orig_val is true.
    option_value2string(&options[expand_option_idx], expand_option_flags);
    var = NameBuff;
    buf = escape_option_str_cmdline(var);
    නම් (buf == NULL)
	ආපසු FAIL;

    args.oe_opt_value = buf;

    පූර්ණ_සංඛ්‍යා num_ret = options[expand_option_idx].opt_expand_cb(&args, numMatches, matches);

    vim_free(buf);
    ආපසු num_ret;
}

/*
 * Expansion handler for :set-=
 */
    පූර්ණ_සංඛ්‍යා
ExpandSettingSubtract(
    expand_T	*xp,
    regmatch_T	*regmatch,
    පූර්ණ_සංඛ්‍යා		*numMatches,
    char_u	***matches)
{
    නම් (expand_option_idx < 0)
	// term option
	ආපසු ExpandOldSetting(numMatches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    නම් (option_flags & P_NUM)
	ආපසු ExpandOldSetting(numMatches, matches);
    නැතහොත් නම් (option_flags & P_COMMA)
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

	නම් (*option_val == NUL)
	    ආපසු FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	නම් (option_copy == NULL)
	    ආපසු FAIL;
	next_val = option_copy;

	ga_init2(&ga, parimaa_na(char_u *), 10);

	do
	{
	    item = next_val;
	    comma = vim_strchr(next_val, ',');
	    අතරතුර (comma != NULL && comma != next_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    නම් (comma != NULL)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		next_val = comma + 1;
	    }
	    නැතහොත්
		next_val = NULL;

	    නම් (*item == NUL)
		// empty value, don't add to list
		continue;

	    නම් (!vim_regexec(regmatch, item, (colnr_T)0))
		continue;

	    char_u *buf = escape_option_str_cmdline(item);
	    නම් (buf == NULL)
	    {
		vim_free(option_copy);
		ga_clear_strings(&ga);
		ආපසු FAIL;
	    }
	    නම් (ga_add_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		bha_nga;
	    }
	} අතරතුර (next_val != NULL);

	vim_free(option_copy);

	*matches = ga.ga_data;
	*numMatches = ga.ga_len;
	ආපසු OK;
    }
    නැතහොත් නම් (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	නම් (*xp->xp_pattern != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    ආපසු FAIL;
	}

	size_t num_flags = STRLEN(option_val);
	නම් (num_flags == 0)
	    ආපසු FAIL;

	*matches = ALLOC_MULT(char_u *, num_flags + 1);
	නම් (*matches == NULL)
	    ආපසු FAIL;

	පූර්ණ_සංඛ්‍යා count = 0;
	char_u *p;

	p = vim_strnsave(option_val, num_flags);
	නම් (p == NULL)
	{
	    VIM_CLEAR(*matches);
	    ආපසු FAIL;
	}
	(*matches)[count++] = p;

	නම් (num_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    සඳහා (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		නම් (p == NULL)
		    bha_nga;
		(*matches)[count++] = p;
	    }
	}

	*numMatches = count;
	ආපසු OK;
    }

    ආපසු ExpandOldSetting(numMatches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    static void
option_value2string(
    sa_nracanaa vimoption	*opp,
    පූර්ණ_සංඛ්‍යා			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    නම් (opp->flags & P_NUM)
    {
	long wc = 0;

	නම් (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_key_name((පූර්ණ_සංඛ්‍යා)wc, 0));
	නැතහොත් නම් (wc != 0)
	    STRCPY(NameBuff, transchar((පූර්ණ_සංඛ්‍යා)wc));
	නැතහොත්
	    sprintf((var_na *)NameBuff, "%ld", *(long *)varp);
    }
    නැතහොත්    // P_STRING
    {
	varp = *(char_u **)(varp);
	නම් (varp == NULL)		    // just in case
	    NameBuff[0] = NUL;
#ifdef FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	නැතහොත් නම් (opp->var == (char_u *)&p_key && *varp)
	    STRCPY(NameBuff, "*****");
#endif
	නැතහොත් නම් (opp->flags & P_EXPAND)
	    home_replace(NULL, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	නැතහොත් නම් ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	නැතහොත්
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    static පූර්ණ_සංඛ්‍යා
wc_use_keyname(char_u *varp, long *wcp)
{
    නම් (((long *)varp == &p_wc) || ((long *)varp == &p_wcm))
    {
	*wcp = *(long *)varp;
	නම් (IS_SPECIAL(*wcp) || find_special_key_in_table((පූර්ණ_සංඛ්‍යා)*wcp) >= 0)
	    ආපසු TRUE;
    }
    ආපසු FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    පූර්ණ_සංඛ්‍යා
shortmess(පූර්ණ_සංඛ්‍යා x)
{
    ආපසු p_shm != NULL &&
	    (   vim_strchr(p_shm, x) != NULL
	    || (vim_strchr(p_shm, 'a') != NULL
		&& vim_strchr((char_u *)SHM_A, x) != NULL));
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 * When "fname" is not NULL, use it to set $"envname" when it wasn't set yet.
 */
    void
vimrc_found(char_u *fname, char_u *envname)
{
    පූර්ණ_සංඛ්‍යා		opt_idx;
    පූර්ණ_සංඛ්‍යා		dofree = FALSE;
    char_u	*p;

    නම් (!option_was_set((char_u *)"cp"))
    {
	p_cp = FALSE;
	සඳහා (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    නම් (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    නම් (fname != NULL)
    {
	p = vim_getenv(envname, &dofree);
	නම් (p == NULL)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = FullName_save(fname, FALSE);
	    නම් (p != NULL)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	නැතහොත් නම් (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    void
change_compatible(පූර්ණ_සංඛ්‍යා on)
{
    පූර්ණ_සංඛ්‍යා	    opt_idx;

    නම් (p_cp != on)
    {
	p_cp = on;
	compatible_set();
    }
    opt_idx = findoption((char_u *)"cp");
    නම් (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_SET;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    පූර්ණ_සංඛ්‍යා
option_was_set(char_u *name)
{
    පූර්ණ_සංඛ්‍යා idx;

    idx = findoption(name);
    නම් (idx < 0)	// unknown option
	ආපසු FALSE;
    නම් (options[idx].flags & P_WAS_SET)
	ආපසු TRUE;
    ආපසු FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    පූර්ණ_සංඛ්‍යා
reset_option_was_set(char_u *name)
{
    පූර්ණ_සංඛ්‍යා idx = findoption(name);

    නම් (idx < 0)
	ආපසු FAIL;

    options[idx].flags &= ~P_WAS_SET;
    ආපසු OK;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    static void
compatible_set(void)
{
    පූර්ණ_සංඛ්‍යා	    opt_idx;

    සඳහා (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	නම් (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    set_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    පූර්ණ_සංඛ්‍යා
can_bs(
    පූර්ණ_සංඛ්‍යා		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ifdef FEAT_JOB_CHANNEL
    නම් (what == BS_START && bt_prompt(curbuf))
	ආපසු FALSE;
#endif
    switch (*p_bs)
    {
	pak_sa '3':       ආපසු TRUE;
	pak_sa '2':	ආපසු (what != BS_NOSTOP);
	pak_sa '1':	ආපසු (what != BS_START);
	pak_sa '0':	ආපසු FALSE;
    }
    ආපසු vim_strchr(p_bs, what) != NULL;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloff_value(void)
{
    ආපසු curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloffpad_value(void)
{
    ආපසු curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_sidescrolloff_value(void)
{
    ආපසු curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    unsigned පූර්ණ_සංඛ්‍යා
get_bkc_flags(buf_T *buf)
{
    ආපසු buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#නම් defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_value(buf_T *buf)
{
    නම් (buf->b_p_flp == NULL || *buf->b_p_flp == NUL)
	ආපසු p_flp;
    ආපසු buf->b_p_flp;
}
#endif

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    unsigned පූර්ණ_සංඛ්‍යා
get_ve_flags(void)
{
    ආපසු (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#නම් defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_value(win_T *win)
{
    නම් (win->w_p_sbr == NULL || *win->w_p_sbr == NUL)
	ආපසු p_sbr;
    නම් (STRCMP(win->w_p_sbr, "NONE") == 0)
	ආපසු empty_option;
    ආපසු win->w_p_sbr;
}
#endif

#නම් defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(පූර්ණ_සංඛ්‍යා bufopt)
{
    dict_T	*d;
    පූර්ණ_සංඛ්‍යා		opt_idx;

    d = dict_alloc();
    නම් (d == NULL)
	ආපසු NULL;

    සඳහා (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	sa_nracanaa vimoption *opt = &options[opt_idx];

	නම් ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    නම් (varp != NULL)
	    {
		නම් (opt->flags & P_STRING)
		    dict_add_string(d, opt->fullname, *(char_u **)varp);
		නැතහොත් නම් (opt->flags & P_NUM)
		    dict_add_number(d, opt->fullname, *(long *)varp);
		නැතහොත්
		    dict_add_number(d, opt->fullname, *(පූර්ණ_සංඛ්‍යා *)varp);
	    }
	}
    }

    ආපසු d;
}
#endif

#නම් defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    පූර්ණ_සංඛ්‍යා
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    නම් (val == NULL)
	p = wp->w_p_culopt;
    නැතහොත්
	p = val;
    අතරතුර (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	නම් (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_LINE;
	}
	නැතහොත් නම් (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_LINE | CULOPT_NBR;
	}
	නැතහොත් නම් (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	නැතහොත් නම් (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	නම් (*p != ',' && *p != NUL)
	    ආපසු FAIL;
	නම් (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    නම් ((culopt_flags_new & CULOPT_LINE) && (culopt_flags_new & CULOPT_SCRLINE))
	ආපසු FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    ආපසු OK;
}
#endif

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    පූර්ණ_සංඛ්‍යා
magic_isset(void)
{
    switch (magic_overruled)
    {
	pak_sa OPTION_MAGIC_ON:      ආපසු TRUE;
	pak_sa OPTION_MAGIC_OFF:     ආපසු FALSE;
	pak_sa OPTION_MAGIC_NOT_SET: bha_nga;
    }
#ifdef FEAT_EVAL
    නම් (in_vim9script())
	ආපසු TRUE;
#endif
    ආපසු p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    පූර්ණ_සංඛ්‍යා
option_set_callback_func(char_u *optval UNUSED, callback_T *optcb UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    පූර්ණ_සංඛ්‍යා		funcname = FALSE;

    නම් (optval == NULL || *optval == NUL)
    {
	free_callback(optcb);
	ආපසු OK;
    }

    නම් (*optval == '{' || (in_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_expr(optval, NULL);
    නැතහොත්
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    නම් (tv == NULL)
	ආපසු FAIL;

    cb = get_callback(tv);
    නම් (cb.cb_name == NULL || *cb.cb_name == NUL)
    {
	free_tv(tv);
	ආපසු FAIL;
    }

    free_callback(optcb);
    set_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = NULL;
    නම් (in_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != NULL))
    {
	නම් (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = find_imported(optval, dot - optval, FALSE);
	නම් (import != NULL && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((var_na *)fnamebuf, parimaa_na(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	නැතහොත්
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

    ආපසු OK;
#නැතහොත්
    ආපසු FAIL;
#endif
}

#නම් defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    var_na *
did_set_showtabpanel(optset_T *args UNUSED)
{
    shell_new_columns();
    ආපසු NULL;
}
#endif

#නම් defined(FEAT_EVAL)
    static void
didset_options_sctx(පූර්ණ_සංඛ්‍යා opt_flags, var_na **buf)
{
	සඳහා (පූර්ණ_සංඛ්‍යා i = 0; ; ++i)
	{
	    නම් (buf[i] == NULL)
		bha_nga;

	    පූර්ණ_සංඛ්‍යා idx = findoption((char_u *)buf[i]);
	    නම් (idx >= 0)
		set_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#endif
