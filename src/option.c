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

static void set_options_default(ціле opt_flags);
static void set_string_default_esc(char *name, char_u *val, ціле escape);
static char_u *find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
static char_u *option_expand(ціле opt_idx, char_u *val);
static void didset_options(void);
static void didset_options2(void);
#якщо defined(FEAT_EVAL)
static long_u *insecure_flag(win_T *wp, ціле opt_idx, ціле opt_flags);
#інакше
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#endif
static char *set_bool_option(ціле opt_idx, char_u *varp, ціле value, ціле opt_flags);
static char *set_num_option(ціле opt_idx, char_u *varp, long value, char *errbuf, size_t errbuflen, ціле opt_flags);
static ціле find_key_option(char_u *arg_arg, ціле has_lt);
static void showoptions(ціле all, ціле opt_flags);
static ціле optval_default(struct vimoption *, char_u *varp, ціле compatible);
static void showoneopt(struct vimoption *, ціле opt_flags);
static ціле put_setstring(FILE *fd, bool legacy, char *cmd, char *name, char_u **valuep, long_u flags);
static ціле put_setnum(FILE *fd, char *cmd, char *name, long *valuep);
static ціле put_setbool(FILE *fd, char *cmd, char *name, ціле value);
static ціле istermoption(struct vimoption *p);
static ціле istermoption_idx(ціле opt_idx);
static char_u *get_varp_scope(struct vimoption *p, ціле scope);
static char_u *get_varp(struct vimoption *);
static void check_win_options(win_T *win);
static void option_value2string(struct vimoption *, ціле scope);
static void check_winopt(winopt_T *wop);
static ціле wc_use_keyname(char_u *varp, long *wcp);
static void compatible_set(void);

#якщо defined(FEAT_EVAL)
static char *(p_bin_dep_opts[])    = {"textwidth", "wrapmargin", "modeline", "expandtab", NULL};
static char *(p_paste_dep_opts[])  = {"autoindent", "expandtab", "ruler", "showmatch", "smarttab",
    "softtabstop", "textwidth", "wrapmargin",
# ifdef FEAT_RIGHTLEFT
    "hkmap", "revins",
# endif
# ifdef FEAT_VARTABS
    "varsofttabstop",
# endif
    NULL};
static void didset_options_sctx(ціле opt_flags, char **buf);
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
    якщо (((p = mch_getenv((char_u *)"SHELL")) != NULL && *p != NUL)
#якщо defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != NULL && *p != NUL)
	    || ((p = (char_u *)default_shell()) != NULL && *p != NUL)
#endif
	    )
#якщо defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	якщо (vim_strchr(p, ' ') != NULL)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    якщо (cmd != NULL)
	    {
		vim_snprintf((char *)cmd, len, "\"%s\"", p);
		set_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	інакше
	    set_string_default("sh", p);
    }
#інакше
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
    ціле		opt_idx;
    ціле		i;
    char_u	*p;
    ціле		plen;
#ifdef UNIX
    static char	*(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#інакше
    static char	*(names[3]) = {"TMPDIR", "TEMP", "TMP"};
#endif
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    для (i = 0; i < (ціле)ARRAY_LENGTH(names); ++i)
    {
	ціле		mustfree = FALSE;
#ifdef UNIX
	якщо (*names[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (ціле)STRLEN_LITERAL("/private/tmp");
# інакше
	    p = (char_u *)"/tmp";
	    plen = (ціле)STRLEN_LITERAL("/tmp");
# endif
	}
	інакше
#endif
	{
	    p = vim_getenv((char_u *)names[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	якщо (p != NULL && *p != NUL)
	{
	    char_u  *item;
	    size_t  itemsize;
	    ціле	    has_trailing_path_sep = FALSE;

	    якщо (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (ціле)STRLEN(p);
		// does the value include a trailing path separator?
		якщо (after_pathsep(p, p + plen))
		    has_trailing_path_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_path_sep ? 0 : 1) + 2;
	    item = alloc(itemsize);
	    якщо (item != NULL)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((char *)item, itemsize, "%s%s*", p, (has_trailing_path_sep) ? "" : PATHSEPSTR);

		якщо (find_dup_item(ga.ga_data, item, itemlen, options[opt_idx].flags) == NULL
			&& ga_grow(&ga, (ціле)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((char *)ga.ga_data + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", item);
		}
		vim_free(item);
	    }
	}
	якщо (mustfree)
	    vim_free(p);
    }
    якщо (ga.ga_data != NULL)
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
    ціле		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    якщо (opt_idx < 0)
	повернути;

#якщо !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
    якщо (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#endif
    {
#якщо defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#elif defined(HAVE_TOTAL_MEM)
	// Use amount of memory available to Vim.
	n = (mch_total_mem(FALSE) >> 1);
#інакше
	n = (0x7fffffff >> 11);
#endif
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	якщо (opt_idx >= 0)
	{
#якщо !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
	    якщо ((long)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (long)n
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
    ціле		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    ціле	i;
    ціле	j;
    ціле	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    якщо (cdpath == NULL)
	повернути;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    якщо (buf != NULL)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	для (i = 0; cdpath[i] != NUL; ++i)
	{
	    якщо (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    інакше
	    {
		якщо (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	якщо (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	інакше
	    vim_free(buf); // cannot happen
    }
    якщо (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    static void
set_init_default_printencoding(void)
{
#якщо defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    set_string_default("penc",
# якщо defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# інакше // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#endif
}

#якщо defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    static void
set_init_default_printexpr(void)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    set_string_default("pexpr",
# якщо defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# інакше
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
    якщо (p == NULL)
	повернути;
    якщо (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
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
    ціле		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    якщо (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    якщо (opt_idx < 0)
	повернути;
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
    ціле		opt_idx;
    ціле		has_xdg_env = TRUE;
    ціле		should_free_xdg_dir = FALSE;
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
    якщо (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    якщо (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	goto theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    якщо ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	goto theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    якщо ((opt_idx = findoption((char_u *)"packpath")) < 0)
	goto theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# якщо defined(XDG_VDIR) && defined(FEAT_SESSION)
    якщо ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	goto theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    якщо (should_free_xdg_dir)
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
    ціле		opt_idx;
    char_u	*p;

    для (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	якщо ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != NULL)
	    p = (char_u *)_(*(char **)options[opt_idx].var);
	інакше
	    p = option_expand(opt_idx, NULL);
	якщо (p != NULL && (p = vim_strsave(p)) != NULL)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    якщо (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#якщо defined(MSWIN) && defined(FEAT_GETTEXT)
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

    якщо (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num_languages, NULL,
								    &bufsize)
	    || bufsize == 0)
	повернути NULL;

    buffer = ALLOC_MULT(WCHAR, bufsize);
    якщо (buffer == NULL)
	повернути NULL;

    якщо (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num_languages, buffer,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	якщо (langs != NULL)
	{
	    char_u	*d = langs;
	    WCHAR	*s = buffer;

	    поки (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		якщо (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		для ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		якщо (english)
		    break;
	    }
	    *d = NUL;
	}
    }
    vim_free(buffer);

    повернути langs;
}
#endif

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    static void
set_init_lang_env(void)
{
#якщо defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    якщо (mch_getenv((char_u *)"LANG") == NULL
	    && mch_getenv((char_u *)"LANGUAGE") == NULL
	    && mch_getenv((char_u *)"LC_ALL") == NULL
	    && mch_getenv((char_u *)"LC_MESSAGES") == NULL)
    {
	char_u	*langs = get_ui_langs();

	якщо (langs != NULL && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    якщо (colon != NULL)
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
    ціле		opt_idx;

#якщо defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#інакше
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#endif
    якщо (p == NULL)
	повернути;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    якщо (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    якщо (mb_init() == NULL)
    {
	opt_idx = findoption((char_u *)"encoding");
	якщо (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#якщо defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	якщо (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    set_string_option_direct((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    set_string_option_direct((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    якщо (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    якщо (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (void)init_chartab();
	}
#endif

#якщо defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	якщо (
# ifdef VIMDLL
		(!gui.in_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    char	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    якщо (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, sizeof(buf), "cp%ld", (long)GetACP());
	    інакше
		buflen = vim_snprintf(buf, sizeof(buf), "cp%ld", (long)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    якщо (p_tenc != NULL)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		якщо (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_setup(&input_conv, p_tenc, p_enc);
		convert_setup(&output_conv, p_enc, p_tenc);
	    }
	    інакше
		p_tenc = empty_option;
	}
#endif
#якщо defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#endif
    }
    інакше
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
set_init_1(ціле clean_arg)
{
#ifdef FEAT_LANGMAP
    langmap_init();
#endif

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    якщо (mch_getenv((char_u *)"VIM_POSIX") != NULL)
    {
	set_string_default("cpo", (char_u *)CPO_ALL);
	set_string_default("shm", (char_u *)SHM_POSIX);
    }

    set_init_default_shell();
    set_init_default_backupskip();
    set_init_default_maxmemtot();
    set_init_default_cdpath();
    set_init_default_printencoding();
#якщо defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
    якщо (clean_arg)
	set_init_clean_rtp();
#endif

#ifdef FEAT_GUI
    якщо (found_reverse_arg)
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

#якщо defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    якщо (mch_getenv((char_u *)"MLTERM") != NULL)
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
    ціле		opt_idx,
    ціле		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    ціле		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    ціле		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    ціле		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    якщо (varp != NULL)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	якщо (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    якщо (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		set_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    інакше якщо (options[opt_idx].indir != PV_NONE)
		set_string_option_direct(NULL, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    інакше
	    {
		якщо ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	інакше якщо (flags & P_NUM)
	{
	    якщо (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    інакше
	    {
		long def_val = (long)(long_i)options[opt_idx].def_val[dvi];

		якщо ((long *)varp == &curwin->w_p_so
			|| (long *)varp == &curwin->w_p_siso
			|| (long *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(long *)varp = -1;
		інакше
		    *(long *)varp = def_val;
		// May also set global value for local option.
		якщо (both)
		    *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	інакше	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(ціле *)varp = (ціле)(long)(long_i)options[opt_idx].def_val[dvi];
#ifdef UNIX
	    // 'modeline' defaults to off for root
	    якщо (options[opt_idx].indir == PV_ML && getuid() == ROOT_UID)
		*(ціле *)varp = FALSE;
#endif
	    // May also set global value for local option.
	    якщо (both)
		*(ціле *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(ціле *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	якщо (both)
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
    ціле		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    ціле		i;
    win_T	*wp;
    tabpage_T	*tp;

    для (i = 0; !istermoption_idx(i); i++)
	якщо (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#якщо defined(FEAT_CRYPT)
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
set_string_default_esc(char *name, char_u *val, ціле escape)
{
    char_u	*p;
    ціле		opt_idx;

    якщо (escape && vim_strchr(val, ' ') != NULL)
	p = vim_strsave_escaped(val, (char_u *)" ");
    інакше
	p = vim_strsave(val);
    якщо (p == NULL)		// we don't want a NULL
	повернути;

    opt_idx = findoption((char_u *)name);
    якщо (opt_idx < 0)
    {
	vim_free(p);
	повернути;
    }

    якщо (options[opt_idx].flags & P_DEF_ALLOCED)
	vim_free(options[opt_idx].def_val[VI_DEFAULT]);
    options[opt_idx].def_val[VI_DEFAULT] = p;
    options[opt_idx].flags |= P_DEF_ALLOCED;
}

    void
set_string_default(char *name, char_u *val)
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
    ціле	    bs = 0;
    char_u  *s;

    якщо (origval == NULL)
	повернути NULL;

    для (s = origval; *s != NUL; ++s)
    {
	якщо ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    повернути s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	якщо ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	інакше
	    bs = 0;
    }
    повернути NULL;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    void
set_number_default(char *name, long val)
{
    ціле		opt_idx;

    opt_idx = findoption((char_u *)name);
    якщо (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#якщо defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    void
set_local_options_default(win_T *wp, ціле do_buffer)
{
    win_T	*save_curwin = curwin;
    ціле		i;

    curwin = wp;
    curbuf = curwin->w_buffer;
    block_autocmds();

    для (i = 0; !istermoption_idx(i); i++)
    {
	struct vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	якщо (p->indir != PV_NONE
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

#якщо defined(EXITFREE)
/*
 * Free all options.
 */
    void
free_all_options(void)
{
    ціле		i;

    для (i = 0; !istermoption_idx(i); i++)
    {
	якщо (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    якщо ((options[i].flags & P_ALLOCED) && options[i].var != NULL)
		free_string_option(*(char_u **)options[i].var);
	    якщо (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	інакше якщо (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# якщо defined(FEAT_EVAL)
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
    ціле		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    якщо (idx >= 0 && !(options[idx].flags & P_WAS_SET))
	set_option_default(idx, OPT_LOCAL, p_cp);
    comp_col();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    якщо (!option_was_set((char_u *)"window"))
	p_window = Rows - 1;
    set_number_default("window", Rows - 1);

    // For DOS console the default is always black.
#якщо !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    якщо (idx >= 0 && !(options[idx].flags & P_WAS_SET)
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
#якщо defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    ціле	    idx_srr;
    ціле	    do_srr;
# ifdef FEAT_QUICKFIX
    ціле	    idx_sp;
    ціле	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    якщо (idx_srr < 0)
	do_srr = FALSE;
    інакше
	do_srr = !(options[idx_srr].flags & P_WAS_SET);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    якщо (idx_sp < 0)
	do_sp = FALSE;
    інакше
	do_sp = !(options[idx_sp].flags & P_WAS_SET);
# endif
    p = get_isolated_shell_name();
    якщо (p != NULL)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	якщо (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# якщо defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# якщо defined(FEAT_QUICKFIX)
	    якщо (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  інакше
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    якщо (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	інакше якщо (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  якщо defined(FEAT_QUICKFIX)
		якщо (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		якщо (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	інакше
	    // Always use POSIX shell style redirection if we reach this
	    якщо (       fnamecmp(p, "sh") == 0
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
# якщо defined(FEAT_QUICKFIX)
		якщо (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  інакше
		    якщо (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    інакше
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		якщо (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#endif

#якщо defined(MSWIN)
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
    якщо (strstr((char *)gettail(p_sh), "powershell") != NULL)
    {
	ціле	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	якщо (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	якщо (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    інакше якщо (strstr((char *)gettail(p_sh), "sh") != NULL)
    {
	ціле	idx3;

	idx3 = findoption((char_u *)"shcf");
	якщо (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	якщо (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    інакше якщо (strstr((char *)gettail(p_sh), "cmd.exe") != NULL)
    {
	ціле	idx3;

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
	якщо (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	якщо (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#endif

    якщо (BUFEMPTY())
    {
	ціле idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	якщо (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_SET))
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    set_title_defaults();
}

#якщо defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    void
set_helplang_default(char_u *lang)
{
    ціле		idx;
    size_t	langlen;

    якщо (lang == NULL)	// safety check
	повернути;

    langlen = STRLEN(lang);
    якщо (langlen < 2)	// safety check
	повернути;

    idx = findoption((char_u *)"hlg");
    якщо (idx < 0 || (options[idx].flags & P_WAS_SET))
	повернути;

    якщо (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    якщо (p_hlg == NULL)
	p_hlg = empty_option;
    інакше
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	якщо (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	інакше якщо (langlen >= 1 && *p_hlg == 'C')
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
    ціле	    idx1;
    long    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    якщо (idx1 >= 0 && !(options[idx1].flags & P_WAS_SET))
    {
#ifdef FEAT_GUI
	якщо (gui.starting || gui.in_use)
	    val = TRUE;
	інакше
#endif
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    якщо (idx1 < 0 || (options[idx1].flags & P_WAS_SET))
    {
	повернути;
    }

#ifdef FEAT_GUI
    якщо (gui.starting || gui.in_use)
	val = TRUE;
    інакше
#endif
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    void
ex_set(exarg_T *eap)
{
    ціле		flags = 0;

    якщо (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    інакше якщо (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#якщо defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    якщо ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    інакше
#endif
    {
	якщо (eap->forceit)
	    flags |= OPT_ONECOLUMN;
	(void)do_set(eap->arg, flags);
    }
}

/*
 * :set boolean option prefix
 */
typedef enum {
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
    ціле		prefix = PREFIX_NONE;
    char_u	*arg = *argp;

    якщо (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	prefix = PREFIX_NO;
	arg += 2;
    }
    інакше якщо (STRNCMP(arg, "inv", 3) == 0)
    {
	prefix = PREFIX_INV;
	arg += 3;
    }

    *argp = arg;
    повернути prefix;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    static ціле
parse_option_name(char_u *arg, ціле *opt_idxp, ціле *lenp, ціле *keyp)
{
    ціле	key = 0;
    ціле	len;
    ціле	opt_idx;

    якщо (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	якщо (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	інакше
	{
	    len = 1;
	    поки (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	якщо (arg[len] != '>')
	    повернути FAIL;

	arg[len] = NUL;			    // put NUL after name
	якщо (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	якщо (opt_idx == -1)
	    key = find_key_option(arg + 1, TRUE);
    }
    інакше
    {
	ціле	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	якщо (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	інакше
	    поки (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	якщо (opt_idx == -1)
	    key = find_key_option(arg, FALSE);
    }

    *keyp = key;
    *lenp = len;
    *opt_idxp = opt_idx;

    повернути OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    static set_op_T
get_opt_op(char_u *arg)
{
    set_op_T op = OP_NONE;

    якщо (*arg != NUL && *(arg + 1) == '=')
    {
	якщо (*arg == '+')
	    op = OP_ADDING;		// "+="
	інакше якщо (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	інакше якщо (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    повернути op;
}

// Options that are allowed in a modeline when 'modelinestrict' is on.
static char *modeline_whitelist[] =
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
is_modeline_whitelisted(char *name)
{
    для (ціле i = 0; modeline_whitelist[i] != NULL; i++)
	якщо (STRCMP(name, modeline_whitelist[i]) == 0)
	    повернути true;
    повернути false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    static ціле
validate_opt_idx(
	ціле opt_idx,
	ціле opt_flags,
	long_u flags,
	char **errmsg,
	set_prefix_T prefix)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    якщо ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	повернути FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    якщо ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	повернути FAIL;

    // Disallow changing some options from modelines.
    якщо (opt_flags & OPT_MODELINE)
    {
	якщо (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_in_modeline;
	    повернути FAIL;
	}
	якщо ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_in_modeline_when_modelineexpr_is_off;
	    повернути FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	якщо (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    якщо (options[opt_idx].indir == PV_ML && prefix == PREFIX_NO)
		повернути OK;
	    інакше якщо (!is_modeline_whitelisted(options[opt_idx].fullname))
		повернути FAIL;
	}
#ifdef FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	якщо (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    повернути FAIL;
#endif
    }

#ifdef HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    якщо (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_in_sandbox;
	повернути FAIL;
    }
#endif

    повернути OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    static char_u *
stropt_get_default_val(
    ціле		opt_idx,
    char_u	*varp,
    ціле		flags,
    ціле		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    якщо ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ifdef FEAT_GUI
	якщо (gui.in_use)
	    newval = gui_bg_default();
	інакше
#endif
	    newval = term_bg_default();
    }
    інакше якщо ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    якщо (newval == NULL)
	newval = empty_option;
    інакше
    {
	char_u *s = option_expand(opt_idx, newval);
	якщо (s == NULL)
	    s = newval;
	newval = vim_strsave(s);
    }

    повернути newval;
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
    ціле		i = getdigits((char_u **)varp);

    switch (i)
    {
	case 0:
	    *(char_u **)varp = empty_option;
	    break;
	case 1:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol", STRLEN_LITERAL("indent,eol"));
	    break;
	case 2:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,start", STRLEN_LITERAL("indent,eol,start"));
	    break;
	case 3:
	    *(char_u **)varp = vim_strnsave((char_u *)"indent,eol,nostop", STRLEN_LITERAL("indent,eol,nostop"));
	    break;
    }
    vim_free(*oldval_p);
    якщо (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    якщо (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    якщо (*origval_g_p == *oldval_p)
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
    ціле i = getdigits(argp);
    якщо (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    якщо (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    якщо (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    якщо (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    якщо (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    якщо (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    повернути whichwrap;
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
    ціле		flags UNUSED)
{
    char_u	*arg = *argp;
    unsigned	newlen;
    char_u	*newval;
    char_u	*s = NULL;

    // get a bit too much
    newlen = (unsigned)STRLEN(arg) + 1;
    якщо (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;
    newval = alloc(newlen);
    якщо (newval == NULL)  // out of mem, don't change
	повернути NULL;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    поки (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	ціле i;

	якщо (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#endif
	   )
	    ++arg;	// remove backslash
	якщо (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	інакше
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    повернути newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    static char_u *
stropt_expand_envvar(
    ціле		opt_idx,
    char_u	*origval,
    char_u	*newval,
    set_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    якщо (s == NULL)
	повернути newval;

    vim_free(newval);
    unsigned newlen = (unsigned)STRLEN(s) + 1;
    якщо (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;

    newval = alloc(newlen);
    якщо (newval == NULL)
	повернути NULL;

    STRCPY(newval, s);

    повернути newval;
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
    ціле		flags)
{
    ціле		len = 0;

    ціле comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    якщо (op == OP_ADDING)
    {
	len = (ціле)STRLEN(origval);
	// strip a trailing comma, would get 2
	якщо (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    інакше
    {
	len = (ціле)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    якщо (comma)
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
    ціле		flags,
    char_u	*strval,
    ціле		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    якщо (*strval)
    {
	// may need to remove a comma
	якщо (flags & P_COMMA)
	{
	    якщо (strval == origval)
	    {
		// include comma after string
		якщо (strval[len] == ',')
		    ++len;
	    }
	    інакше
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
find_key_item(char_u *src, char_u *key, ціле keylen, ціле *itemlenp)
{
    char_u  *p = src;

    поки (*p != NUL)
    {
	// Check if this item starts with the same key
	якщо ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, key, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *end = vim_strchr(p, ',');
	    якщо (end == NULL)
		end = p + STRLEN(p);
	    *itemlenp = (ціле)(end - p);
	    повернути p;
	}
	++p;
    }
    повернути NULL;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    static void
remove_comma_item(char_u *str, char_u *item, ціле itemlen)
{
    якщо (item[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(item, item + itemlen + 1);
    інакше якщо (item > str && *(item - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(item - 1, item + itemlen);
    інакше
	// Only item
	*item = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    static void
remove_key_item(char_u *str, char_u *key, ціле keylen, char_u *skip)
{
    ціле	    itemlen;
    char_u  *found;

    поки ((found = find_key_item(str, key, keylen, &itemlen)) != NULL)
    {
	якщо (found == skip)
	{
	    // Search for the next match after this one.
	    char_u *next = found + itemlen;
	    якщо (*next == ',')
		++next;
	    found = find_key_item(next, key, keylen, &itemlen);
	    якщо (found == NULL)
		break;
	}

	remove_comma_item(str, found, itemlen);
    }
}

/*
 * Append a comma-separated item to the end of "str" in-place.
 * Adds a comma before the item if "str" is not empty.
 */
    static void
append_item(char_u *str, char_u *item, ціле item_len)
{
    ціле len = (ціле)STRLEN(str);

    якщо (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, item, (size_t)item_len);
    str[len + item_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    static void
prepend_item(char_u *str, char_u *item, ціле item_len)
{
    ціле len = (ціле)STRLEN(str);
    ціле comma = (len > 0) ? 1 : 0;

    mch_memmove(str + item_len + comma, str, (size_t)len + 1);
    mch_memmove(str, item, (size_t)item_len);
    якщо (comma)
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
    ціле		flags UNUSED)
{
    char_u  *p;
    char_u  *item_start;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    якщо (vim_strchr(newval, ':') == NULL && vim_strchr(newval, ',') == NULL)
	повернути false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    якщо (newval_copy == NULL)
	повернути false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    item_start = newval_copy;
    для (;;)
    {
	p = vim_strchr(item_start, ',');
	ціле item_len = (p == NULL)
			    ? (ціле)STRLEN(item_start) : (ціле)(p - item_start);

	якщо (item_len > 0)
	{
	    char_u *colon = vim_strchr(item_start, ':');
	    якщо (colon != NULL && colon < item_start + item_len)
	    {
		ціле keylen = (ціле)(colon - item_start) + 1;

		якщо (op == OP_ADDING || op == OP_PREPENDING)
		{
		    ціле old_itemlen;
		    char_u *found = find_key_item(newval, item_start,
						       keylen, &old_itemlen);
		    якщо (found != NULL)
		    {
			якщо (old_itemlen == item_len
				&& STRNCMP(found, item_start,
							 item_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_key_item(newval, item_start,
							   keylen, found);
			}
			інакше
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_key_item(newval, item_start,
							    keylen, NULL);
			    якщо (op == OP_PREPENDING)
				prepend_item(newval, item_start, item_len);
			    інакше
				append_item(newval, item_start, item_len);
			}
		    }
		    інакше
		    {
			// New item.
			якщо (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			інакше
			    append_item(newval, item_start, item_len);
		    }
		}
		інакше якщо (op == OP_REMOVING)
		    remove_key_item(newval, item_start, keylen, NULL);
	    }
	    інакше
	    {
		якщо (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    якщо (found == NULL)
		    {
			// New item.
			якщо (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			інакше
			    append_item(newval, item_start, item_len);
		    }
		    // else: exact duplicate — do nothing
		}
		інакше якщо (op == OP_REMOVING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    якщо (found != NULL)
			remove_comma_item(newval, found, item_len);
		}
	    }
	}

	якщо (p == NULL)
	    break;
	item_start = p + 1;
    }

    vim_free(newval_copy);

    повернути true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    static void
stropt_remove_dupflags(char_u *newval, ціле flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    поки (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	якщо (flags & P_ONECOMMA)
	{
	    якщо (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != NULL)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		continue;
	    }
	}
	інакше
	{
	    якщо ((!(flags & P_COMMA) || *s != ',')
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
    ціле		nextchar,
    ціле		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    set_op_T    *op_arg,
    ціле		flags,
    ціле		cp_val)
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

    якщо (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    інакше якщо (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    інакше
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	якщо (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	інакше якщо (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	інакше якщо (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	інакше якщо (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_value(origval, &arg, op, flags);
	якщо (newval == NULL)
	    goto done;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	якщо (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    якщо (newval == NULL)
		goto done;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	якщо ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	інакше
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    ціле len = 0;
	    якщо (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (ціле)STRLEN(newval);
		s = find_dup_item(origval, newval, len, flags);

		// do not add if already there
		якщо ((op == OP_ADDING || op == OP_PREPENDING) && s != NULL)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		якщо (s == NULL)
		    s = origval + (ціле)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    якщо (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    інакше якщо (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	якщо (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

done:
    якщо (save_arg != NULL)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    повернути newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    static ціле
do_set_option_string(
	ціле	    opt_idx,
	ціле	    opt_flags,
	char_u	    **argp,
	ціле	    nextchar,
	set_op_T    op_arg,
	long_u	    flags,
	ціле	    cp_val,
	char_u	    *varp_arg,
	char	    *errbuf,
	size_t	    errbuflen,
	ціле	    *value_checked,
	char	    **errmsg)
{
    char_u	*arg = *argp;
    set_op_T    op = op_arg;
    char_u	*varp = varp_arg;
    char_u	*oldval = NULL; // previous value if *varp
    char_u	*newval;
    char_u	*origval = NULL;
    char_u	*origval_l = NULL;
    char_u	*origval_g = NULL;
#якщо defined(FEAT_EVAL)
    char_u	*saved_origval = NULL;
    char_u	*saved_origval_l = NULL;
    char_u	*saved_origval_g = NULL;
    char_u	*saved_newval = NULL;
#endif

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    якщо ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((ціле)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    якщо ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	якщо (((ціле)options[opt_idx].indir & PV_BOTH)
						  && origval_l == empty_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    якщо (((ціле)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    інакше
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    якщо (newval == NULL)
	*(char_u **)(varp) = empty_option;

#якщо defined(FEAT_EVAL)
    якщо (!starting
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
	якщо (origval_l != NULL)
	    saved_origval_l = vim_strsave(origval_l);
	якщо (origval_g != NULL)
	    saved_origval_g = vim_strsave(origval_g);
    }
#endif

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	ціле	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	якщо ((opt_flags & OPT_MODELINE)
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

#якщо defined(FEAT_EVAL)
    якщо (*errmsg == NULL)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#endif

    *argp = arg;
    повернути *errmsg == NULL ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    static char *
do_set_option_bool(
    ціле		opt_idx,
    ціле		opt_flags,
    set_prefix_T prefix,
    long_u	flags,
    char_u	*varp,
    ціле		nextchar,
    ціле		afterchar,
    ціле		cp_val)

{
    varnumber_T	value;

    якщо (nextchar == '=' || nextchar == ':')
	повернути e_invalid_argument;
    якщо (opt_idx < 0 || varp == NULL)
	повернути NULL;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    якщо (nextchar == '!')
	value = *(ціле *)(varp) ^ 1;
    інакше якщо (nextchar == '&')
	value = (ціле)(long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    інакше якщо (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	якщо ((ціле *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    value = -1;
	інакше якщо ((ціле *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    value = -1;
#ifdef HAVE_FSYNC
	інакше якщо ((ціле *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    value = -1;
#endif
	інакше
	    value = *(ціле *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    інакше
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	якщо (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    повернути e_trailing_characters;
	якщо (prefix == PREFIX_INV)
	    value = *(ціле *)(varp) ^ 1;
	інакше
	    value = prefix == PREFIX_NO ? 0 : 1;
    }

    повернути set_bool_option(opt_idx, varp, (ціле)value, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    static char *
do_set_option_numeric(
    ціле		opt_idx,
    ціле		opt_flags,
    char_u	**argp,
    ціле		nextchar,
    set_op_T	op,
    long_u	flags,
    ціле		cp_val,
    char_u	*varp,
    char	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		value;
    ціле			i;
    char		*errmsg = NULL;

    якщо (opt_idx < 0 || varp == NULL)
	повернути NULL;  // "cannot happen"
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
    якщо (nextchar == '&')
	value = (long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    інакше якщо (nextchar == '<')
    {
	якщо ((long *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    value = NO_LOCAL_UNDOLEVEL;
	інакше якщо (opt_flags == OPT_LOCAL
		    && ((long *)varp == &curwin->w_p_siso
		     || (long *)varp == &curwin->w_p_so
		     || (long *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    value = -1;
	інакше
	    value = *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    інакше якщо (((long *)varp == &p_wc || (long *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	value = string_to_key(arg, FALSE);
	якщо (value == 0)
	{
	    errmsg = e_invalid_argument;
	    goto skip;
	}
    }
    інакше якщо (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, NULL, &i, STR2NR_ALL, &value, NULL, 0, TRUE, NULL);
	якщо (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_number_required_after_equal;
	    goto skip;
	}
    }
    інакше
    {
	errmsg = e_number_required_after_equal;
	goto skip;
    }

    якщо (op == OP_ADDING)
	value = *(long *)varp + value;
    інакше якщо (op == OP_PREPENDING)
	value = *(long *)varp * value;
    інакше якщо (op == OP_REMOVING)
	value = *(long *)varp - value;

    якщо ((long *)varp == &curwin->w_p_sop && value < -1)
    {
	errmsg = e_invalid_argument;
	goto skip;
    }

    errmsg = set_num_option(opt_idx, varp, value, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    повернути errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    static char *
do_set_option_keycode(char_u **argp, char_u *key_name, ціле nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    якщо (nextchar == '&')
    {
	якщо (add_termcap_entry(key_name, TRUE) == FAIL)
	    повернути e_not_found_in_termcap;
    }
    інакше
    {
	++arg; // jump to after the '=' or ':'
	для (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    якщо (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	add_termcode(key_name, arg, FALSE);
	*p = nextchar;
    }
    якщо (full_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    повернути NULL;
}

/*
 * Set an option to a new value.
 */
    static char *
do_set_option_value(
    ціле		opt_idx,
    ціле		opt_flags,
    char_u	**argp,
    set_prefix_T prefix,
    set_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*key_name,
    ціле		nextchar,
    ціле		afterchar,
    ціле		cp_val,
    ціле		*stopopteval,
    char	*errbuf,
    size_t	errbuflen)
{
    ціле		value_checked = FALSE;
    char	*errmsg = NULL;
    char_u	*arg = *argp;

    якщо (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_set_option_bool(opt_idx, opt_flags, prefix, flags, varp,
						nextchar, afterchar, cp_val);
	якщо (errmsg != NULL)
	    goto skip;
    }
    інакше
    {
	// numeric or string option
	якщо (vim_strchr((char_u *)"=:&<", nextchar) == NULL
		|| prefix != PREFIX_NONE)
	{
	    errmsg = e_invalid_argument;
	    goto skip;
	}

	якщо (flags & P_NUM)
	{
	    // numeric option
	    errmsg = do_set_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    якщо (errmsg != NULL)
		goto skip;
	}
	інакше якщо (opt_idx >= 0)
	{
	    // string option
	    якщо (do_set_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&value_checked, &errmsg) == FAIL)
	    {
		якщо (errmsg != NULL)
		    goto skip;
		*stopopteval = TRUE;
		goto skip;
	    }
	}
	інакше
	{
	    // key code option
	    errmsg = do_set_option_keycode(&arg, key_name, nextchar);
	    якщо (errmsg != NULL)
		goto skip;
	}
    }

    якщо (opt_idx >= 0)
	did_set_option(opt_idx, opt_flags, op == OP_NONE, value_checked);

skip:
    *argp = arg;
    повернути errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    static char *
do_set_option(
    ціле		opt_flags,
    char_u	**argp,
    char_u	*arg_start,
    char_u	**startarg,
    ціле		*did_show,
    ціле		*stopopteval,
    char	*errbuf,
    size_t	errbuflen)
{
    ціле		opt_idx;
    char_u	*arg;
    set_prefix_T prefix;	// no prefix, "no" prefix or "inv" prefix
    set_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	key_name[2];
    ціле		nextchar;	// next non-white char after option name
    ціле		afterchar;	// character just after option name
    char	*errmsg = NULL;
    ціле		key;
    ціле		len;

    prefix = get_option_prefix(argp);
    arg = *argp;

    // find end of name
    key = 0;
    якщо (parse_option_name(arg, &opt_idx, &len, &key) == FAIL)
	повернути e_invalid_argument;

    // remember character after option name
    afterchar = arg[len];

    якщо (in_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	якщо (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != NULL
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    goto skip;
	}
    }
    інакше
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	поки (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    якщо (op != OP_NONE)
	len++;

    nextchar = arg[len];

    якщо (opt_idx == -1 && key == 0)	// found a mismatch: skip
    {
	якщо (in_vim9script() && arg > arg_start
		&& vim_strchr((char_u *)"!&<", *arg) != NULL)
	    errmsg = e_no_white_space_allowed_between_option_and;
	інакше
	    errmsg = e_unknown_option;
	goto skip;
    }

    якщо (opt_idx >= 0)
    {
	якщо (options[opt_idx].var == NULL)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    якщо (vim_strchr((char_u *)"=:!&<", nextchar) == NULL
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    goto skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    інакше
    {
	flags = P_STRING;
	varp = NULL;
	якщо (key < 0)
	{
	    key_name[0] = KEY2TERMCAP0(key);
	    key_name[1] = KEY2TERMCAP1(key);
	}
	інакше
	{
	    key_name[0] = KS_KEY;
	    key_name[1] = (key & 0xff);
	}
    }

    // Make sure the option value can be changed.
    якщо (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, prefix) == FAIL)
	goto skip;

    ціле cp_val = p_cp;
    якщо (vim_strchr((char_u *)"?=:!&<", nextchar) != NULL)
    {
	arg += len;
	якщо (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    якщо (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    інакше		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	якщо (vim_strchr((char_u *)"?!&<", nextchar) != NULL
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    goto skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    якщо (nextchar == '?'
	    || (prefix == PREFIX_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == NULL
		&& !(flags & P_BOOL)))
    {
	// print value
	якщо (*did_show)
	    msg_putchar('\n');	    // cursor below last one
	інакше
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	якщо (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ifdef FEAT_EVAL
	    якщо (p_verbose > 0)
	    {
		// Mention where the option was last set.
		якщо (varp == options[opt_idx].var)
		    last_set_msg(options[opt_idx].script_ctx);
		інакше якщо ((ціле)options[opt_idx].indir & PV_WIN)
		    last_set_msg(curwin->w_p_script_ctx[
			    (ціле)options[opt_idx].indir & PV_MASK]);
		інакше якщо ((ціле)options[opt_idx].indir & PV_BUF)
		    last_set_msg(curbuf->b_p_script_ctx[
			    (ціле)options[opt_idx].indir & PV_MASK]);
	    }
#endif
	}
	інакше
	{
	    char_u	    *p;

	    p = find_termcode(key_name);
	    якщо (p == NULL)
	    {
		errmsg = e_key_code_not_set;
		goto skip;
	    }
	    інакше
		(void)show_one_termcode(key_name, p, TRUE);
	}
	якщо (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    інакше
    {
	errmsg = do_set_option_value(opt_idx, opt_flags, &arg, prefix, op,
					flags, varp, key_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    повернути errmsg;
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
    ціле
do_set(
    char_u	*arg_start,	// option string (may be written to!)
    ціле		opt_flags)
{
    char_u	*arg = arg_start;
    ціле		i;
    ціле		did_show = FALSE;   // already showed one value

    якщо (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	goto theend;
    }

    поки (*arg != NUL)		// loop to process all options
    {
	якщо (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    якщо (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		set_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    інакше
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	інакше якщо (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	інакше
	{
	    ціле		stopopteval = FALSE;
	    char	*errmsg = NULL;
	    char	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_set_option(opt_flags, &arg, arg_start, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    якщо (stopopteval)
		break;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    для (i = 0; i < 2 ; ++i)
	    {
		поки (*arg != NUL && !VIM_ISWHITE(*arg))
		    якщо (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		якщо (*arg != '=')
		    break;
	    }

	    якщо (errmsg != NULL)
	    {
		i = vim_snprintf((char *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		якщо (i + (arg - startarg) < IOSIZE)
		{
		    // append the argument with the error
		    STRCPY(IObuff + i - 2, ": ");
		    mch_memmove(IObuff + i, startarg, (arg - startarg));
		    IObuff[i + (arg - startarg)] = NUL;
		}
		// make sure all characters are printable
		trans_characters(IObuff, IOSIZE);

		++no_wait_return;		// wait_return() done later
		emsg((char *)IObuff);	// show error highlighted
		--no_wait_return;

		повернути FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    якщо (silent_mode && did_show)
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

    повернути OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    void
did_set_option(
    ціле	    opt_idx,
    ціле	    opt_flags,	    // possibly with OPT_MODELINE
    ціле	    new_value,	    // value was replaced completely
    ціле	    value_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = NULL;
    ціле		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_SET;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    якщо (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    якщо (!value_checked && (secure
#ifdef HAVE_SANDBOX
	    || sandbox != 0
#endif
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	якщо (flagsp_local != NULL)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    інакше якщо (new_value)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	якщо (flagsp_local != NULL)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    ціле
string_to_key(char_u *arg, ціле multi_byte)
{
    якщо (*arg == '<' && arg[1])
	повернути find_key_option(arg + 1, TRUE);
    якщо (*arg == '^' && arg[1])
    {
	ціле key = Ctrl_chr(arg[1]);
	якщо (key == 0)		// ^@ is <Nul>
	    key = K_ZERO;
	повернути key;
    }
    якщо (multi_byte)
	повернути PTR2CHAR(arg);
    повернути *arg;
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
    якщо (starting != NO_SCREEN
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
    ціле		oldval,
    ціле		newval,
    ціле		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    якщо (newval)
    {
	якщо (!oldval)		// switched on
	{
	    якщо (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    якщо (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	якщо (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	якщо (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    інакше якщо (oldval)		// switched off
    {
	якщо (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	якщо (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#якщо defined(FEAT_EVAL)
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
option_expand(ціле opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    якщо (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == NULL)
	повернути NULL;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    якщо (val != NULL && STRLEN(val) > MAXPATHL)
	повернути NULL;

    якщо (val == NULL)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    ціле esc = var == &p_tags || var == &p_path;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : NULL, FALSE,
#ifdef FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#endif
				  NULL);
    якщо (STRCMP(NameBuff, val) == 0)   // they are the same
	повернути NULL;

    повернути NameBuff;
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
    ціле		opt_idx;

    для (opt_idx = 0; options[opt_idx].fullname != NULL; opt_idx++)
	якщо ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != NULL)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    ціле
get_term_opt_idx(char_u **p)
{
    ціле opt_idx;

    для (opt_idx = 1; options[opt_idx].fullname != NULL; opt_idx++)
	якщо (options[opt_idx].var == (char_u *)p)
	    повернути opt_idx;
    повернути -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    ціле
set_term_option_alloced(char_u **p)
{
    ціле		opt_idx = get_term_opt_idx(p);

    якщо (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    повернути opt_idx;
}

#якщо defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    ціле
was_set_insecurely(win_T *wp, char_u *opt, ціле opt_flags)
{
    ціле	    idx = findoption(opt);
    long_u  *flagp;

    якщо (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	повернути (*flagp & P_INSECURE) != 0;
    }
    internal_error("was_set_insecurely()");
    повернути -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    static long_u *
insecure_flag(win_T *wp, ціле opt_idx, ціле opt_flags)
{
    якщо (opt_flags & OPT_LOCAL)
	switch ((ціле)options[opt_idx].indir)
	{
	    case PV_WRAP:	повернути &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    case PV_STL:	повернути &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    case PV_FDE:	повернути &wp->w_p_fde_flags;
	    case PV_FDT:	повернути &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    case PV_BEXPR:	повернути &wp->w_buffer->b_p_bexpr_flags;
#  endif
	    case PV_INDE:	повернути &wp->w_buffer->b_p_inde_flags;
	    case PV_FEX:	повернути &wp->w_buffer->b_p_fex_flags;
#  ifdef FEAT_FIND_ID
	    case PV_INEX:	повернути &wp->w_buffer->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    case PV_CPT:	повернути &wp->w_buffer->b_p_cpt_flags;
#  endif
# endif
	}
    інакше
	// For global value of window-local options, use flags in w_allbuf_opt.
	switch ((ціле)options[opt_idx].indir)
	{
	    case PV_WRAP:	повернути &wp->w_allbuf_opt.wo_wrap_flags;
# якщо defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    case PV_FDE:	повернути &wp->w_allbuf_opt.wo_fde_flags;
	    case PV_FDT:	повернути &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    повернути &options[opt_idx].flags;
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
#якщо defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    ціле
valid_name(char_u *val, char *allowed)
{
    char_u *s;

    для (s = val; *s != NUL; ++s)
	якщо (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == NULL)
	    повернути FALSE;
    повернути TRUE;
}

#якщо defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    void
set_option_sctx_idx(ціле opt_idx, ціле opt_flags, sctx_T script_ctx)
{
    ціле		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    ціле		indir = (ціле)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    якщо (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    якщо (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    якщо (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    якщо (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    якщо (both || (opt_flags & OPT_LOCAL))
    {
	якщо (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	інакше якщо (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	    якщо (both)
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
is_option_value_vim9(ціле opt_idx, ціле opt_flags)
{
    ціле		indir = (ціле)options[opt_idx].indir;
    sctx_T	*sctx = NULL;

    якщо ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    якщо ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	якщо (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_MASK];
	інакше якщо (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_MASK];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    повернути !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_version >= SCRIPT_VERSION_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(char *name)
{
    ціле idx = findoption((char_u *)name);

    якщо (idx >= 0)
	повернути &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    повернути NULL;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    void
set_term_option_sctx_idx(char *name, ціле opt_idx)
{
    char_u  buf[5];
    ціле	    idx;

    якщо (name == NULL)
	idx = opt_idx;
    інакше
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    якщо (idx >= 0)
	set_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#endif

#якщо defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    static void
apply_optionset_autocmd(
	ціле	opt_idx,
	long	opt_flags,
	long	oldval,
	long	oldval_g,
	long	newval,
	char	*errmsg)
{
    char_u buf_old[12], buf_new[12], buf_type[12];
    size_t  buf_oldlen;
    size_t  len;

    // Don't do this while starting up, failure or recursively.
    якщо (starting || errmsg != NULL || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	повернути;

    len = vim_snprintf_safelen((char *)buf_new, sizeof(buf_new), "%ld", newval);
    set_vim_var_string(VV_OPTION_NEW, buf_new, (ціле)len);
    len = vim_snprintf_safelen((char *)buf_type, sizeof(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (ціле)len);

    buf_oldlen = vim_snprintf_safelen((char *)buf_old, sizeof(buf_old), "%ld", oldval);
    set_vim_var_string(VV_OPTION_OLD, buf_old, (ціле)buf_oldlen);

    якщо (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ціле)buf_oldlen);
    }
    якщо (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (ціле)buf_oldlen);
    }
    якщо ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((char *)buf_old_global,
	    sizeof(buf_old_global), "%ld", oldval_g);

	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ціле)buf_oldlen);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (ціле)buf_old_globallen);
    }
    якщо (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (ціле)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    NULL, FALSE, NULL);
    reset_v_option_vars();
}
#endif

#якщо defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    char *
did_set_arabic(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    якщо (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	якщо (!p_tbidi)
	{
	    // set rightleft mode
	    якщо (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		changed_window_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    якщо (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	якщо (STRCMP(p_enc, "utf-8") != 0)
	{
	    static char *w_arabic = N_("W17: Arabic requires UTF-8, do ':set encoding=utf-8'");

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
    інакше
    {
	// 'arabic' is reset, handle various sub-settings.
	якщо (!p_tbidi)
	{
	    // reset rightleft mode
	    якщо (curwin->w_p_rl)
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

    повернути errmsg;
}
#endif

#якщо defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    char *
did_set_autochdir(optset_T *args UNUSED)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    повернути NULL;
}
#endif

#якщо defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    char *
did_set_ballooneval(optset_T *args)
{
    якщо (balloonEvalForTerm)
	повернути NULL;

    якщо (p_beval && !args->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    інакше якщо (!p_beval && args->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    повернути NULL;
}
#endif

#якщо defined(FEAT_BEVAL_TERM)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    char *
did_set_balloonevalterm(optset_T *args UNUSED)
{
    mch_bevalterm_changed();
    повернути NULL;
}
#endif

/*
 * Process the updated 'binary' option value.
 */
    char *
did_set_binary(optset_T *args)
{
    // when 'bin' is set also set some other options
    set_options_bin(args->os_oldval.boolean, curbuf->b_p_bin, args->os_flags);
    redraw_titles();

    повернути NULL;
}

/*
 * Process the updated 'buflisted' option value.
 */
    char *
did_set_buflisted(optset_T *args)
{
    // when 'buflisted' changes, trigger autocommands
    якщо (args->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						NULL, NULL, TRUE, curbuf);
    повернути NULL;
}

/*
 * Process the new 'cmdheight' option value.
 */
    char *
did_set_cmdheight(optset_T *args)
{
    long old_value = args->os_oldval.number;
    char *errmsg = NULL;

    // if p_ch changed value, change the command line height
    якщо (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_argument_must_be_positive;
	p_ch = MIN_CMDHEIGHT;
    }
    якщо (p_ch > Rows - min_rows() + MIN_CMDHEIGHT)
	p_ch = Rows - min_rows() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    якщо ((p_ch != old_value
		|| tabline_height() + topframe->fr_height != Rows - p_ch)
	    && full_screen
#ifdef FEAT_GUI
	    && !gui.starting
#endif
       )
	command_height();

    повернути errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    char *
did_set_compatible(optset_T *args UNUSED)
{
    compatible_set();
    повернути NULL;
}

#якщо defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    char *
did_set_conceallevel(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    якщо (curwin->w_p_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_cole = 0;
    }
    інакше якщо (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_cole = 3;
    }
    якщо (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    інакше якщо (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    повернути errmsg;
}
#endif

#якщо defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    char *
did_set_diff(optset_T *args UNUSED)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    якщо (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    повернути NULL;
}
#endif

/*
 * Process the updated 'endoffile' or 'endofline' or 'fixendofline' or 'bomb'
 * option value.
 */
    char *
did_set_eof_eol_fixeol_bomb(optset_T *args UNUSED)
{
    // redraw the window title and tab page text
    redraw_titles();
    повернути NULL;
}

/*
 * Process the updated 'equalalways' option value.
 */
    char *
did_set_equalalways(optset_T *args)
{
    якщо (p_ea && !args->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    повернути NULL;
}

#якщо defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    char *
did_set_foldcolumn(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    якщо (curwin->w_p_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_fdc = 0;
    }
    інакше якщо (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_fdc = 12;
    }
    якщо (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    інакше якщо (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    повернути errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    char *
did_set_foldlevel(optset_T *args UNUSED)
{
    якщо (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    повернути NULL;
}

/*
 * Process the new 'foldminlines' option value.
 */
    char *
did_set_foldminlines(optset_T *args UNUSED)
{
    foldUpdateAll(curwin);
    повернути NULL;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    char *
did_set_foldnestmax(optset_T *args UNUSED)
{
    якщо (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    повернути NULL;
}
#endif

#якщо defined(FEAT_SEARCH_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    char *
did_set_hlsearch(optset_T *args UNUSED)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    set_no_hlsearch(FALSE);
    повернути NULL;
}
#endif

/*
 * Process the updated 'ignorecase' option value.
 */
    char *
did_set_ignorecase(optset_T *args UNUSED)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    якщо (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    повернути NULL;
}

#якщо defined(HAVE_INPUT_METHOD)
/*
 * Process the updated 'imdisable' option value.
 */
    char *
did_set_imdisable(optset_T *args UNUSED)
{
    // Only de-activate it here, it will be enabled when changing mode.
    якщо (p_imdisable)
	im_set_active(FALSE);
    інакше якщо (State & MODE_INSERT)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
    повернути NULL;
}
#endif

/*
 * Process the new 'iminsert' option value.
 */
    char *
did_set_iminsert(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    якщо (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    якщо (termcap_active)	// don't do this in the alternate screen
	showmode();
#якщо defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif

    повернути errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    char *
did_set_imsearch(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    якщо (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    повернути errmsg;
}

#якщо defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    char *
did_set_imstyle(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    якщо (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_argument;

    повернути errmsg;
}
#endif

/*
 * Process the updated 'insertmode' option value.
 */
    char *
did_set_insertmode(optset_T *args)
{
    // when 'insertmode' is set from an autocommand need to do work here
    якщо (p_im)
    {
	якщо ((State & MODE_INSERT) == 0)
	    need_start_insertmode = TRUE;
	stop_insert_mode = FALSE;
    }
    // only reset if it was set previously
    інакше якщо (args->os_oldval.boolean)
    {
	need_start_insertmode = FALSE;
	stop_insert_mode = TRUE;
	якщо (restart_edit != 0 && mode_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    повернути NULL;
}

#якщо defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    char *
did_set_langnoremap(optset_T *args UNUSED)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    повернути NULL;
}

/*
 * Process the updated 'langremap' option value.
 */
    char *
did_set_langremap(optset_T *args UNUSED)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    повернути NULL;
}
#endif

/*
 * Process the new 'laststatus' option value.
 */
    char *
did_set_laststatus(optset_T *args UNUSED)
{
    last_status(FALSE);	// (re)set last window status line
    повернути NULL;
}

#якщо defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    char *
did_set_linespace(optset_T *args UNUSED)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    якщо (gui.in_use && gui_mch_adjust_charheight() == OK)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    повернути NULL;
}
#endif

/*
 * Process the updated 'lisp' option value.
 */
    char *
did_set_lisp(optset_T *args UNUSED)
{
    // When 'lisp' option changes include/exclude '-' in keyword characters.
    (void)buf_init_chartab(curbuf, FALSE);	    // ignore errors
    повернути NULL;
}

/*
 * Process the new 'maxcombine' option value.
 */
    char *
did_set_maxcombine(optset_T *args UNUSED)
{
    якщо (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    інакше якщо (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    повернути NULL;
}

/*
 * Process the updated 'modifiable' option value.
 */
    char *
did_set_modifiable(optset_T *args UNUSED)
{
    // when 'modifiable' is changed, redraw the window title

#ifdef FEAT_TERMINAL
    // Cannot set 'modifiable' when in Terminal mode.
    якщо (curbuf->b_p_ma && (term_in_normal_mode(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_term != NULL && !term_is_finished(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	повернути e_cannot_make_terminal_with_running_job_modifiable;
    }
#endif
    redraw_titles();

    повернути NULL;
}

/*
 * Process the updated 'modified' option value.
 */
    char *
did_set_modified(optset_T *args)
{
    якщо (!args->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_set = !!args->os_newval.boolean;
    повернути NULL;
}

#якщо defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    char *
did_set_mousehide(optset_T *args UNUSED)
{
    якщо (!p_mh)
	gui_mch_mousehide(FALSE);
    повернути NULL;
}
#endif

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    char *
did_set_number_relativenumber(optset_T *args UNUSED)
{
#якщо defined(FEAT_SIGNS) && defined(FEAT_GUI)
    якщо (gui.in_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != NULL)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	якщо (!(curwin->w_p_nu && ((ціле *)args->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#endif
    повернути NULL;
}

#якщо defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    char *
did_set_numberwidth(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    // 'numberwidth' must be positive
    якщо (curwin->w_p_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_nuw = 1;
    }
    інакше якщо (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_nuw = 20;
    }
    якщо (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    інакше якщо (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_line_count = 0; // trigger a redraw

    повернути errmsg;
}
#endif

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    char *
did_set_osctimeoutlen(optset_T *args)
{
    якщо (p_ost < 0)
    {
	p_ost = args->os_oldval.number;
	повернути e_argument_must_be_positive;
    }

    повернути NULL;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    char *
did_set_paste(optset_T *args UNUSED)
{
    static ціле	old_p_paste = FALSE;
    static ціле	save_sm = 0;
    static ціле	save_sta = 0;
    static ціле	save_ru = 0;
#ifdef FEAT_RIGHTLEFT
    static ціле	save_ri = 0;
    static ціле	save_hkmap = 0;
#endif
    buf_T	*buf;

    якщо (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	якщо (!old_p_paste)
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
		якщо (buf->b_p_vsts_nopaste)
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
	    якщо (p_vsts_nopaste)
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
	    якщо (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = empty_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#endif
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	якщо (p_ru)
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
	якщо (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = empty_option;
#endif
    }

    // Paste switched from on to off: Restore saved values.
    інакше якщо (old_p_paste)
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
	    якщо (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : empty_option;
	    vim_free(buf->b_p_vsts_array);
	    якщо (buf->b_p_vsts && buf->b_p_vsts != empty_option)
		(void)tabstop_set(buf->b_p_vsts, &buf->b_p_vsts_array);
	    інакше
		buf->b_p_vsts_array = NULL;
#endif
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	якщо (p_ru != save_ru)
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
	якщо (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : empty_option;
#endif
    }

    old_p_paste = p_paste;

#якщо defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#endif

    повернути NULL;
}


#ifdef FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    char *
did_set_previewwindow(optset_T *args UNUSED)
{
    якщо (!curwin->w_p_pvw)
	повернути NULL;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	якщо (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    повернути e_preview_window_already_exists;
	}

    повернути NULL;
}
#endif

#якщо defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    char *
did_set_pyxversion(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    якщо (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_argument;

    повернути errmsg;
}
#endif

/*
 * Process the updated 'readonly' option value.
 */
    char *
did_set_readonly(optset_T *args)
{
    // when 'readonly' is reset globally, also reset readonlymode
    якщо (!curbuf->b_p_ro && (args->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    якщо (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    повернути NULL;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    char *
did_set_scrollbind(optset_T *args UNUSED)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    якщо (!curwin->w_p_scb)
	повернути NULL;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    повернути NULL;
}

/*
 * Process the new 'maxsearchcount' option value.
 */
    char *
did_set_maxsearchcount(optset_T *args UNUSED)
{
    char	*errmsg = NULL;
// if you increase this, also increase SEARCH_STAT_BUF_LEN in search.c
#define MAX_SEARCH_COUNT 9999

    якщо (p_msc <= 0)
	errmsg = e_argument_must_be_positive;
    інакше якщо (p_msc > MAX_SEARCH_COUNT)
	errmsg = e_invalid_argument;

    якщо (errmsg != NULL)
	p_msc = 99;

    повернути errmsg;
#undef MAX_SEARCH_COUNT
}

#якщо defined(BACKSLASH_IN_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    char *
did_set_shellslash(optset_T *args UNUSED)
{
    якщо (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    інакше
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
    повернути NULL;
}
#endif

/*
 * Process the new 'shiftwidth' or the 'tabstop' option value.
 */
    char *
did_set_shiftwidth_tabstop(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    char	*errmsg = NULL;

    якщо (curbuf->b_p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_ts = 8;
    }
    інакше якщо (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_ts = 8;
    }
    якщо (p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ts = 8;
    }
    інакше якщо (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	p_ts = 8;
    }

    якщо (curbuf->b_p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_count(curbuf->b_p_vts_array) > 0
		       ? tabstop_first(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#інакше
	curbuf->b_p_sw = curbuf->b_p_ts;
#endif
    }
    якщо (p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_count(curbuf->b_p_vts_array) > 0
	     ? tabstop_first(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#інакше
	p_sw = curbuf->b_p_ts;
#endif
    }

#ifdef FEAT_FOLDING
    якщо (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#endif
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    якщо (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    повернути errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    char *
did_set_showtabline(optset_T *args UNUSED)
{
    shell_new_rows();	// recompute window positions and heights
    повернути NULL;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    char *
did_set_smoothscroll(optset_T *args UNUSED)
{
    якщо (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    повернути NULL;
}

#якщо defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    char *
did_set_spell(optset_T *args UNUSED)
{
    якщо (curwin->w_p_spell)
	повернути parse_spelllang(curwin);

    повернути NULL;
}
#endif

/*
 * Process the updated 'swapfile' option value.
 */
    char *
did_set_swapfile(optset_T *args UNUSED)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    якщо (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    інакше
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    повернути NULL;
}

#якщо defined(FEAT_TERMGUICOLORS)
    char *
did_set_termguicolors(optset_T *args UNUSED)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    якщо (
#  ifdef VIMDLL
	    !gui.in_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_set = TRUE;
	p_tgc = 0;
	повернути e_24_bit_colors_are_not_supported_on_this_environment;
    }
    якщо (is_term_win32())
	swap_tcap();
# endif
# ifdef FEAT_GUI
    якщо (!gui.in_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    якщо (is_term_win32())
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

    повернути NULL;
}
#endif

    char *
did_set_termsync(optset_T *args UNUSED)
{
    якщо (!p_tsy)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);
    повернути NULL;
}

#якщо defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    char *
did_set_termwinscroll(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    char	*errmsg = NULL;

    якщо (*pp < 1)
    {
	errmsg = e_argument_must_be_positive;
	*pp = 1;
    }

    повернути errmsg;
}
#endif

/*
 * Process the updated 'terse' option value.
 */
    char *
did_set_terse(optset_T *args UNUSED)
{
    char_u	*p;

    // when 'terse' is set change 'shortmess'
    p = vim_strchr(p_shm, SHM_SEARCH);

    // insert 's' in p_shm
    якщо (p_terse && p == NULL)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    інакше якщо (!p_terse && p != NULL)
	STRMOVE(p, p + 1);
    повернути NULL;
}

/*
 * Process the updated 'textauto' option value.
 */
    char *
did_set_textauto(optset_T *args)
{
    // when 'textauto' is set or reset also change 'fileformats'
    set_string_option_direct((char_u *)"ffs", -1,
				p_ta ? (char_u *)DFLT_FFS_VIM : (char_u *)"",
				OPT_FREE | args->os_flags, 0);

    повернути NULL;
}

/*
 * Process the updated 'textmode' option value.
 */
    char *
did_set_textmode(optset_T *args)
{
    // when 'textmode' is set or reset also change 'fileformat'
    set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, args->os_flags);

    повернути NULL;
}

/*
 * Process the new 'textwidth' option value.
 */
    char *
did_set_textwidth(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    якщо (curbuf->b_p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_tw = 0;
    }
    якщо (p_tw < 0)
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

    повернути errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    char *
did_set_title_icon(optset_T *args UNUSED)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_set_title();
    повернути NULL;
}

/*
 * Process the new 'titlelen' option value.
 */
    char *
did_set_titlelen(optset_T *args)
{
    long old_value = args->os_oldval.number;
    char *errmsg = NULL;

    // if 'titlelen' has changed, redraw the title
    якщо (p_titlelen < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_titlelen = 85;
    }
    якщо (starting != NO_SCREEN && old_value != p_titlelen)
	need_maketitle = TRUE;

    повернути errmsg;
}

#якщо defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    char *
did_set_undofile(optset_T *args)
{
    // Only take action when the option was set.
    якщо (!curbuf->b_p_udf && !p_udf)
	повернути NULL;

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
	якщо ((curbuf == save_curbuf
		    || (args->os_flags & OPT_GLOBAL)
		    || args->os_flags == 0)
		&& !curbufIsChanged() && curbuf->b_ml.ml_mfp != NULL)
	{
# ifdef FEAT_CRYPT
	    якщо (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
		continue;
# endif
	    u_compute_hash(hash);
	    u_read_undo(NULL, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    повернути NULL;
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
    char *
did_set_undolevels(optset_T *args)
{
    long *pp = (long *)args->os_varp;

    якщо (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(args->os_newval.number,
						       args->os_oldval.number);
    інакше якщо (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(args->os_newval.number,
						       args->os_oldval.number);

    повернути NULL;
}

/*
 * Process the new 'updatecount' option value.
 */
    char *
did_set_updatecount(optset_T *args)
{
    long old_value = args->os_oldval.number;
    char *errmsg = NULL;

    // when 'updatecount' changes from zero to non-zero, open swap files
    якщо (p_uc < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_uc = 100;
    }
    якщо (p_uc && !old_value)
	ml_open_files();

    повернути errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    char *
did_set_weirdinvert(optset_T *args)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    якщо (p_wiv && !args->os_oldval.boolean)
	T_XS = (char_u *)"y";
    інакше якщо (!p_wiv && args->os_oldval.boolean)
	T_XS = empty_option;
    p_wiv = (*T_XS != NUL);

    повернути NULL;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    char *
did_set_wildchar(optset_T *args)
{
    long c = *(long *)args->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    якщо (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	повернути e_invalid_argument;

    повернути NULL;
}

/*
 * Process the new 'window' option value.
 */
    char *
did_set_window(optset_T *args UNUSED)
{
    якщо (p_window < 1)
	p_window = 1;
    інакше якщо (p_window >= Rows)
	p_window = Rows - 1;
    повернути NULL;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    char *
did_set_winheight_helpheight(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    char	*errmsg = NULL;

    якщо (p_wh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wh = 1;
    }
    якщо (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    якщо (p_hh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hh = 0;
    }

    // Change window height NOW
    якщо (!ONE_WINDOW)
    {
	якщо (pp == &p_wh && curwin->w_height < p_wh)
	    win_setheight((ціле)p_wh);
	якщо (pp == &p_hh && curbuf->b_help && curwin->w_height < p_hh)
	    win_setheight((ціле)p_hh);
    }

    повернути errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    char *
did_set_winminheight(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    якщо (p_wmh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmh = 0;
    }
    якщо (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    повернути errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    char *
did_set_winminwidth(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    якщо (p_wmw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmw = 0;
    }
    якщо (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    повернути errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    char *
did_set_winwidth(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    якщо (p_wiw < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wiw = 1;
    }
    якщо (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    якщо (!ONE_WINDOW && curwin->w_width < p_wiw)
	win_setwidth((ціле)p_wiw);

    повернути errmsg;
}

#якщо defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    char *
did_set_wlsteal(optset_T *args UNUSED)
{
    clip_reset_wayland();

    повернути NULL;
}
#endif

#якщо defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    char *
did_set_wltimeoutlen(optset_T *args)
{
    якщо (p_wtm < 0)
    {
	p_wtm = args->os_oldval.number;
	повернути e_argument_must_be_positive;
    }

    повернути NULL;
}
#endif

/*
 * Process the updated 'wrap' option value.
 */
    char *
did_set_wrap(optset_T *args UNUSED)
{
    // Set w_leftcol or w_skipcol to zero.
    якщо (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    інакше
	curwin->w_skipcol = 0;

    повернути NULL;
}

#ifdef FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    char *
did_set_xhistory(optset_T *args)
{
    ціле is_p_chi = (long*)args->os_varp == &p_chi;
    ціле err;
    long *arg = (is_p_chi) ? &p_chi :(long*)args->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    якщо (*arg < 1)
    {
	*arg = args->os_oldval.number;
	повернути e_cannot_have_negative_or_zero_number_of_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    якщо (*arg > 100)
    {
	*arg = args->os_oldval.number;
	повернути e_cannot_have_more_than_hundred_quickfix;
    }

    якщо (is_p_chi)
	err = qf_resize_stack(*arg);
    інакше
	err = ll_resize_stack(curwin, *arg);

    якщо (err == FAIL)
	повернути e_failed_resizing_quickfix_stack;

    повернути NULL;
}
#endif

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    static char *
set_bool_option(
    ціле		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    ціле		value,			// new value
    ціле		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    ціле		old_value = *(ціле *)varp;
#якщо defined(FEAT_EVAL)
    ціле		old_global_value = 0;
#endif
    char	*errmsg = NULL;

    // Disallow changing some options from secure mode
    якщо ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	повернути e_not_allowed_here;

#якщо defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    якщо ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_value = *(ціле *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *(ціле *)varp = value;	    // set the new value
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    якщо ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(ціле *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = value;

    // Handle side effects of changing a bool option.
    якщо (options[opt_idx].opt_did_set_cb != NULL)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.boolean = old_value;
	args.os_newval.boolean = value;
	args.os_errbuf = NULL;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
	якщо (errmsg != NULL)
	    повернути errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_SET;

#якщо defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(long)(old_value ? TRUE : FALSE),
				(long)(old_global_value ? TRUE : FALSE),
				(long)(value ? TRUE : FALSE), NULL);
#endif

    comp_col();			    // in case 'ruler' or 'showcmd' changed

    якщо (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    якщо ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    повернути errmsg;
}

/*
 * Check the bounds of numeric options.
 */
    static char *
check_num_option_bounds(
    long	*pp,
    long	old_value,
    long	old_Rows,
    long	old_Columns,
    char	*errbuf,
    size_t	errbuflen,
    char	*errmsg)
{
    якщо (Rows < min_rows_for_all_tabpages() && full_screen)
    {
	якщо (errbuf != NULL)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_lines), min_rows_for_all_tabpages());
	    errmsg = errbuf;
	}
	Rows = min_rows_for_all_tabpages();
    }
    якщо (Columns < MIN_COLUMNS && full_screen)
    {
	якщо (errbuf != NULL)
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
    якщо (old_Rows != Rows || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	якщо (updating_screen)
	    *pp = old_value;
	інакше якщо (full_screen
#ifdef FEAT_GUI
		&& !gui.starting
#endif
		)
	    set_shellsize((ціле)Columns, (ціле)Rows, TRUE);
	інакше
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    якщо (cmdline_row > Rows - p_ch && Rows > p_ch)
		cmdline_row = Rows - p_ch;
	}
	якщо (p_window >= Rows || !option_was_set((char_u *)"window"))
	    p_window = Rows - 1;
    }

    якщо (p_tm < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tm = 0;
    }
    якщо ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_height
		    && curwin->w_height > 0))
	    && full_screen)
    {
	якщо (pp == &(curwin->w_p_scr))
	{
	    якщо (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_size;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	інакше якщо (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	інакше // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_height;
    }
    якщо (p_hi < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hi = 0;
    }
    інакше якщо (p_hi > 10000)
    {
	errmsg = e_invalid_argument;
	p_hi = 10000;
    }
    якщо (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_argument;
	p_re = 0;
    }
    якщо (p_report < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_report = 1;
    }
    якщо ((p_sj < -100 || p_sj >= Rows) && full_screen)
    {
	якщо (Rows != old_Rows)	// Rows changed, just adjust p_sj
	    p_sj = Rows / 2;
	інакше
	{
	    errmsg = e_invalid_scroll_size;
	    p_sj = 1;
	}
    }
    якщо (p_so < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_so = 0;
    }
    якщо (p_sop < 0 && full_screen)
    {
	errmsg = e_invalid_argument;
	p_sop = 0;
    }
    якщо (p_siso < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_siso = 0;
    }
    якщо (p_cwh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_cwh = 1;
    }
    якщо (p_ut < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ut = 2000;
    }
    якщо (p_ss < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ss = 0;
    }

    повернути errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    static char *
set_num_option(
    ціле		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    long	value,			// new value
    char	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    ціле		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    char	*errmsg = NULL;
    long	old_value = *(long *)varp;
#якщо defined(FEAT_EVAL)
    long	old_global_value = 0;	// only used when setting a local and
					// global option
#endif
    long	old_Rows = Rows;	// remember old Rows
    long	old_Columns = Columns;	// remember old Columns
    long	*pp = (long *)varp;

    // Disallow changing some options from secure mode.
    якщо ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	повернути e_not_allowed_here;

#якщо defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    якщо ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
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
    якщо ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = value;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    якщо (options[opt_idx].opt_did_set_cb != NULL)
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

#якщо defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_value, old_global_value,
								value, errmsg);
#endif

    comp_col();			    // in case 'columns' or 'ls' changed

    якщо (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    якщо ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    повернути errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    void
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    ціле		doclear = (flags & P_RCLR) == P_RCLR;
    ціле		all = ((flags & P_RALL) == P_RALL || doclear);

    якщо ((flags & P_RSTAT) || all)	// mark all status lines dirty
	status_redraw_all();

    якщо ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	якщо (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	інакше
	    changed_window_setting();
    }
    якщо (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    якщо (doclear)
	redraw_all_later(UPD_CLEAR);
    інакше якщо (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    ціле
findoption(char_u *arg)
{
    ціле		    opt_idx;
    char	    *s, *p;
    static short    quick_tab[27] = {0, 0};	// quick access table
    ціле		    is_term_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    якщо (quick_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	ціле last_opt_idx;
	для (last_opt_idx = 0; options[last_opt_idx].fullname != NULL;
		last_opt_idx++)
	    ;
	для (ціле tab_idx = 1; tab_idx < 27; tab_idx++)
	    quick_tab[tab_idx] = last_opt_idx;

	p = options[0].fullname;
	для (opt_idx = 1; opt_idx < last_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    якщо (s[0] != p[0])
	    {
		якщо (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		інакше
		    quick_tab[CharOrdLow(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    якщо (arg[0] < 'a' || arg[0] > 'z')
	повернути -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    якщо (is_term_opt)
	opt_idx = quick_tab[26];
    інакше
	opt_idx = quick_tab[CharOrdLow(arg[0])];
    для (; (s = options[opt_idx].fullname) != NULL && s[0] == arg[0]; opt_idx++)
    {
	якщо (STRCMP(arg, s) == 0)		    // match full name
	    break;
    }
    якщо (s != NULL && s[0] != arg[0])
	s = NULL;
    якщо (s == NULL && !is_term_opt)
    {
	opt_idx = quick_tab[CharOrdLow(arg[0])];
	для ( ; options[opt_idx].fullname != NULL; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    якщо (s != NULL && STRCMP(arg, s) == 0)   // match short name
		break;
	    s = NULL;
	}
    }
    якщо (s == NULL)
	opt_idx = -1;
    повернути opt_idx;
}

#якщо defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
    ціле		*flagsp,
    ціле		scope)
{
    ціле		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    якщо (opt_idx < 0)		    // option not in the table
    {
	ціле key;

	якщо (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (key = find_key_option(name, FALSE)) != 0)
	{
	    char_u key_name[2];
	    char_u *p;

	    якщо (flagsp != NULL)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    якщо (key < 0)
	    {
		key_name[0] = KEY2TERMCAP0(key);
		key_name[1] = KEY2TERMCAP1(key);
	    }
	    інакше
	    {
		key_name[0] = KS_KEY;
		key_name[1] = (key & 0xff);
	    }
	    p = find_termcode(key_name);
	    якщо (p != NULL)
	    {
		якщо (stringval != NULL)
		    *stringval = vim_strsave(p);
		повернути gov_string;
	    }
	}
	повернути gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    якщо (flagsp != NULL)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    якщо (options[opt_idx].flags & P_STRING)
    {
	якщо (varp == NULL)		    // hidden option
	    повернути gov_hidden_string;
	якщо (stringval != NULL)
	{
	    якщо ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    інакше якщо ((char_u **)varp == &curbuf->b_p_key
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    інакше
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	повернути gov_string;
    }

    якщо (varp == NULL)		    // hidden option
	повернути (options[opt_idx].flags & P_NUM)
					 ? gov_hidden_number : gov_hidden_bool;
    якщо (options[opt_idx].flags & P_NUM)
	*numval = *(long *)varp;
    інакше
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	якщо ((ціле *)varp == &curbuf->b_changed)
	    *numval = curbufIsChanged();
	інакше
	    *numval = (long) *(ціле *)varp;
    }
    повернути (options[opt_idx].flags & P_NUM) ? gov_number : gov_bool;
}
#endif

#якщо defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    ціле
get_option_value_strict(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    ціле		opt_type,
    void	*from)
{
    ціле		opt_idx;
    char_u	*varp = NULL;
    struct vimoption *p;
    ціле		r = 0;

    opt_idx = findoption(name);
    якщо (opt_idx < 0)
	повернути 0;

    p = &(options[opt_idx]);

    // Hidden option
    якщо (p->var == NULL)
	повернути 0;

    якщо (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    інакше якщо (p->flags & P_NUM)
	r |= SOPT_NUM;
    інакше якщо (p->flags & P_STRING)
	r |= SOPT_STRING;

    якщо (p->indir == PV_NONE)
    {
	якщо (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	інакше
	    повернути 0; // Did not request global-only option
    }
    інакше
    {
	якщо (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	інакше якщо (opt_type == SREQ_GLOBAL)
	    повернути 0; // Requested global option

	якщо (p->indir & PV_WIN)
	{
	    якщо (opt_type == SREQ_BUF)
		повернути 0; // Did not request window-local option
	    інакше
		r |= SOPT_WIN;
	}
	інакше якщо (p->indir & PV_BUF)
	{
	    якщо (opt_type == SREQ_WIN)
		повернути 0; // Did not request buffer-local option
	    інакше
		r |= SOPT_BUF;
	}
    }

    якщо (stringval == NULL)
	повернути r;

    якщо (opt_type == SREQ_GLOBAL)
	varp = p->var;
    інакше
    {
	якщо (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    якщо (p->indir == PV_MOD)
	    {
		*numval = bufIsChanged((buf_T *)from);
		varp = NULL;
	    }
# ifdef FEAT_CRYPT
	    інакше якщо (p->indir == PV_KEY)
	    {
		// never return the value of the crypt key
		*stringval = NULL;
		varp = NULL;
	    }
# endif
	    інакше
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
	інакше якщо (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_buffer;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	}
	якщо (varp == p->var)
	    повернути (r | SOPT_UNSET);
    }

    якщо (varp != NULL)
    {
	якщо (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	інакше якщо (p->flags & P_NUM)
	    *numval = *(long *) varp;
	інакше
	    *numval = *(ціле *)varp;
    }

    повернути r;
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
option_iter_next(void **option, ціле opt_type)
{
    struct vimoption	*ret = NULL;
    do
    {
	якщо (*option == NULL)
	    *option = (void *) options;
	інакше якщо (((struct vimoption *) (*option))->fullname == NULL)
	{
	    *option = NULL;
	    повернути NULL;
	}
	інакше
	    *option = (void *) (((struct vimoption *) (*option)) + 1);

	ret = ((struct vimoption *) (*option));

	// Hidden option
	якщо (ret->var == NULL)
	{
	    ret = NULL;
	    continue;
	}

	switch (opt_type)
	{
	    case SREQ_GLOBAL:
		якщо (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = NULL;
		break;
	    case SREQ_BUF:
		якщо (!(ret->indir & PV_BUF))
		    ret = NULL;
		break;
	    case SREQ_WIN:
		якщо (!(ret->indir & PV_WIN))
		    ret = NULL;
		break;
	    default:
		internal_error("option_iter_next()");
		повернути NULL;
	}
    }
    поки (ret == NULL);

    повернути (char_u *)ret->fullname;
}
#endif

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(ціле opt_idx)
{
    повернути options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    void
set_option_flag(ціле opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    ціле
is_global_option(ціле opt_idx)
{
    повернути options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    ціле
is_global_local_option(ціле opt_idx)
{
    повернути options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    ціле
is_window_local_option(ціле opt_idx)
{
    повернути options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    ціле
is_hidden_option(ціле opt_idx)
{
    повернути options[opt_idx].var == NULL;
}

#якщо defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    ціле
is_crypt_key_option(ціле opt_idx)
{
    повернути options[opt_idx].indir == PV_KEY;
}
#endif

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    char *
set_option_value(
    char_u	*name,
    long	number,
    char_u	*string,
    ціле		opt_flags)	// OPT_LOCAL or 0 (both)
{
    ціле		opt_idx;
    char_u	*varp;
    long_u	flags;
    static char	errbuf[ERR_BUFLEN];
    ціле		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    якщо (opt_idx < 0)
    {
	ціле key;

	якщо (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (key = find_key_option(name, FALSE)) != 0)
	{
	    char_u key_name[2];

	    якщо (key < 0)
	    {
		key_name[0] = KEY2TERMCAP0(key);
		key_name[1] = KEY2TERMCAP1(key);
	    }
	    інакше
	    {
		key_name[0] = KS_KEY;
		key_name[1] = (key & 0xff);
	    }
	    add_termcode(key_name, string, FALSE);
	    якщо (full_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    повернути NULL;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    інакше
    {
	flags = options[opt_idx].flags;
#ifdef HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	якщо (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_in_sandbox));
	    повернути NULL;
	}
#endif
	якщо (flags & P_STRING)
	    повернути set_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	якщо (varp != NULL)	// hidden option is not changed
	{
	    якщо (number == 0 && string != NULL)
	    {
		ціле idx;

		// Either we are given a string or we are setting option
		// to zero.
		для (idx = 0; string[idx] == '0'; ++idx)
		    ;
		якщо (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_number_required_after_str_equal_str),
								 name, string);
		    повернути NULL;     // do nothing as we hit an error

		}
	    }
	    якщо (flags & P_NUM)
	    {
		повернути set_num_option(opt_idx, varp, number,
					   errbuf, sizeof(errbuf), opt_flags);
	    }
	    інакше
		повернути set_bool_option(opt_idx, varp, (ціле)number, opt_flags);
	}

    }
    повернути NULL;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    void
set_option_value_give_err(
    char_u	*name,
    long	number,
    char_u	*string,
    ціле		opt_flags)	// OPT_LOCAL or 0 (both)
{
    char *errmsg = set_option_value(name, number, string, opt_flags);

    якщо (errmsg != NULL)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_term_code(char_u *tname)
{
    ціле	    opt_idx;
    char_u  *varp;

    якщо (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	повернути NULL;
    якщо ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	якщо (varp != NULL)
	    varp = *(char_u **)(varp);
	повернути varp;
    }
    повернути find_termcode(tname + 2);
}

    char_u *
get_highlight_default(void)
{
    ціле i;

    i = findoption((char_u *)"hl");
    якщо (i >= 0)
	повернути options[i].def_val[VI_DEFAULT];
    повернути (char_u *)NULL;
}

    char_u *
get_encoding_default(void)
{
    ціле i;

    i = findoption((char_u *)"enc");
    якщо (i >= 0)
	повернути options[i].def_val[VI_DEFAULT];
    повернути (char_u *)NULL;
}

#якщо defined(FEAT_QUICKFIX)
    ціле
is_option_allocated(char *name)
{
    ціле idx = findoption((char_u *)name);

    повернути idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#endif

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    static ціле
find_key_option(char_u *arg_arg, ціле has_lt)
{
    ціле		key = 0;
    ціле		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    якщо (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	якщо (!has_lt || arg[4] == '>')
	    key = TERMCAP2KEY(arg[2], arg[3]);
    }
    інакше якщо (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	key = find_special_key(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_KEY | FSK_SIMPLIFY, NULL);
	якщо (modifiers)		    // can't handle modifiers here
	    key = 0;
    }
    повернути key;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    static void
showoptions(
    ціле		all,
    ціле		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    struct vimoption	*p;
    ціле			col;
    ціле			isterm;
    char_u		*varp;
    struct vimoption	**items;
    ціле			item_count;
    ціле			run;
    ціле			row, rows;
    ціле			cols;
    ціле			i;
    ціле			len;

#define INC 20
#define GAP 3

    items = ALLOC_MULT(struct vimoption *, OPTION_COUNT);
    якщо (items == NULL)
	повернути;

    // Highlight title
    якщо (all == 2)
	msg_puts_title(_("\n--- Terminal codes ---"));
    інакше якщо (opt_flags & OPT_GLOBAL)
	msg_puts_title(_("\n--- Global option values ---"));
    інакше якщо (opt_flags & OPT_LOCAL)
	msg_puts_title(_("\n--- Local option values ---"));
    інакше
	msg_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    для (run = 1; run <= 2 && !got_int; ++run)
    {
	// collect the items in items[]
	item_count = 0;
	для (p = &options[0]; p->fullname != NULL; p++)
	{
	    // apply :filter /pat/
	    якщо (message_filtered((char_u *)p->fullname))
		continue;

	    varp = NULL;
	    isterm = istermoption(p);
	    якщо ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		якщо (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    інакше
		varp = get_varp(p);
	    якщо (varp != NULL
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		якщо (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		інакше якщо (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		інакше
		{
		    option_value2string(p, opt_flags);
		    len = (ціле)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		якщо ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    items[item_count++] = p;
	    }
	}

	// display the items
	якщо (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    якщо (cols == 0)
		cols = 1;
	    rows = (item_count + cols - 1) / cols;
	}
	інакше	// run == 2
	    rows = item_count;
	для (row = 0; row < rows && !got_int; ++row)
	{
	    msg_putchar('\n');			// go to next line
	    якщо (got_int)			// 'q' typed in more
		break;
	    col = 0;
	    для (i = row; i < item_count; i += rows)
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
    static ціле
optval_default(struct vimoption *p, char_u *varp, ціле compatible)
{
    ціле		dvi;

    якщо (varp == NULL)
	повернути TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    якщо (p->flags & P_NUM)
	повернути (*(long *)varp == (long)(long_i)p->def_val[dvi]);
    якщо (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	повернути (*(ціле *)varp == (ціле)(long)(long_i)p->def_val[dvi]);
    // P_STRING
    повернути (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    static void
showoneopt(
    struct vimoption	*p,
    ціле			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    ціле		save_silent = silent_mode;

    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    якщо ((p->flags & P_BOOL) && ((ціле *)varp == &curbuf->b_changed
					? !curbufIsChanged() : !*(ціле *)varp))
	msg_puts("no");
    інакше якщо ((p->flags & P_BOOL) && *(ціле *)varp < 0)
	msg_puts("--");
    інакше
	msg_puts("  ");
    msg_puts(p->fullname);
    якщо (!(p->flags & P_BOOL))
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
    ціле
makeset(FILE *fd, ціле opt_flags, ціле local_only)
{
    struct vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = NULL;	// fresh value
    char		*cmd;
    ціле			round;
    ціле			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    для (pri = 1; pri >= 0; --pri)
    {
      для (p = &options[0]; !istermoption(p); p++)
	якщо (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    якщо (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		continue;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    якщо ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		continue;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    якщо ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		continue;

	    якщо ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		continue;

	    round = 2;
	    якщо (p->indir != PV_NONE)
	    {
		якщо (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    якщо (!(opt_flags & OPT_LOCAL))
			continue;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    якщо (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			якщо (!optval_default(p, varp_fresh, p_cp))
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
	    для ( ; round <= 2; varp = varp_local, ++round)
	    {
		якщо (round == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		інакше
		    cmd = "setlocal";

		якщо (p->flags & P_BOOL)
		{
		    якщо (put_setbool(fd, cmd, p->fullname, *(ціле *)varp) == FAIL)
			повернути FAIL;
		}
		інакше якщо (p->flags & P_NUM)
		{
		    якщо (put_setnum(fd, cmd, p->fullname, (long *)varp) == FAIL)
			повернути FAIL;
		}
		інакше    // P_STRING
		{
		    ціле		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    якщо ((p->flags & P_FUNC) && *(char_u **)varp != NULL
			    && strstr(*(char **)varp, "<lambda>") != NULL)
			continue;

#ifdef FEAT_EVAL
		    legacy = !is_option_value_vim9(p - &options[0],
			    round == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#інакше
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#endif

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    якщо (
#якщо defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#endif
			    p->indir == PV_FT)
		    {
			якщо (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    повернути FAIL;
			do_endif = TRUE;
		    }
		    якщо (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			повернути FAIL;
		    якщо (do_endif)
		    {
			якщо (put_line(fd, "endif") == FAIL)
			    повернути FAIL;
		    }
		}
	    }
	}
    }
    повернути OK;
}

#якщо defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    ціле
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_version < SCRIPT_VERSION_VIM9;
# endif

    якщо (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	повернути FAIL;

    повернути OK;
}
#endif

    static ціле
put_setstring(
    FILE	*fd,
    bool	legacy,
    char	*cmd,
    char	*name,
    char_u	**valuep,
    long_u	flags)
{
    char_u	*s;
    char_u	*buf = NULL;
    char_u	*part = NULL;
    char_u	*p;

    якщо (legacy && fprintf(fd, "legacy ") < 0)
	повернути FAIL;
    якщо (fprintf(fd, "%s %s=", cmd, name) < 0)
	повернути FAIL;
    якщо (*valuep != NULL)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	якщо (valuep == &p_pt)
	{
	    s = *valuep;
	    поки (*s != NUL)
		якщо (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    повернути FAIL;
	}
	// expand the option value, replace $HOME by ~
	інакше якщо ((flags & P_EXPAND) != 0)
	{
	    ціле  size = (ціле)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(size);
	    якщо (buf == NULL)
		goto fail;
	    home_replace(NULL, *valuep, buf, size, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    якщо (size >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != NULL)
	    {
		part = alloc(size);
		якщо (part == NULL)
		    goto fail;

		// write line break to clear the option, e.g. ':set rtp='
		якщо (put_eol(fd) == FAIL)
		    goto fail;

		p = buf;
		поки (*p != NUL)
		{
		    якщо (legacy && fprintf(fd, "legacy ") < 0)
			повернути FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    якщо (fprintf(fd, "%s %s+=", cmd, name) < 0)
			goto fail;
		    (void)copy_option_part(&p, part, size,  ",");
		    якщо (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			goto fail;
		}
		vim_free(buf);
		vim_free(part);
		повернути OK;
	    }
	    якщо (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		повернути FAIL;
	    }
	    vim_free(buf);
	}
	інакше якщо (put_escstr(fd, *valuep, 2) == FAIL)
	    повернути FAIL;
    }
    якщо (put_eol(fd) < 0)
	повернути FAIL;
    повернути OK;
fail:
    vim_free(buf);
    vim_free(part);
    повернути FAIL;
}

    static ціле
put_setnum(
    FILE	*fd,
    char	*cmd,
    char	*name,
    long	*valuep)
{
    long	wc;

    якщо (fprintf(fd, "%s %s=", cmd, name) < 0)
	повернути FAIL;
    якщо (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	якщо (fputs((char *)get_special_key_name((ціле)wc, 0), fd) < 0)
	    повернути FAIL;
    }
    інакше якщо (fprintf(fd, "%ld", *valuep) < 0)
	повернути FAIL;
    якщо (put_eol(fd) < 0)
	повернути FAIL;
    повернути OK;
}

    static ціле
put_setbool(
    FILE	*fd,
    char	*cmd,
    char	*name,
    ціле		value)
{
    якщо (value < 0)	// global/local option using global value
	повернути OK;
    якщо (fprintf(fd, "%s %s%s", cmd, value ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	повернути FAIL;
    повернути OK;
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
#якщо defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    якщо (gui.starting)
	clear_xterm_clip();
#endif
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    void
free_termoptions(void)
{
    struct vimoption   *p;

    для (p = options; p->fullname != NULL; p++)
	якщо (istermoption(p))
	{
	    якщо (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    якщо (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = empty_option;
	    p->def_val[VI_DEFAULT] = empty_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ifdef FEAT_EVAL
	    // remember where the option was cleared
	    set_option_sctx_idx((ціле)(p - options), OPT_GLOBAL, current_sctx);
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
    struct vimoption   *p;

    для (p = &options[0]; p->fullname != NULL; p++)
	якщо (p->var == var)
	{
	    якщо (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    *(char_u **)(p->var) = empty_option;
	    p->flags &= ~P_ALLOCED;
	    break;
	}
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    void
set_term_defaults(void)
{
    struct vimoption   *p;

    для (p = &options[0]; p->fullname != NULL; p++)
    {
	якщо (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    якщо (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    якщо (p->flags & P_ALLOCED)
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
    static ціле
istermoption(struct vimoption *p)
{
    повернути (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    static ціле
istermoption_idx(ціле opt_idx)
{
    повернути istermoption(&options[opt_idx]);
}

#якщо defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    void
unset_global_local_option(char_u *name, void *from)
{
    struct vimoption *p;
    ціле		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    якщо (opt_idx < 0)
	повернути;
    p = &(options[opt_idx]);

    switch ((ціле)p->indir)
    {
	// global option with local value: use local value if it's been set
	case PV_EP:
	    clear_string_option(&buf->b_p_ep);
	    break;
	case PV_KP:
	    clear_string_option(&buf->b_p_kp);
	    break;
	case PV_PATH:
	    clear_string_option(&buf->b_p_path);
	    break;
	case PV_AC:
	    buf->b_p_ac = -1;
	    break;
	case PV_AR:
	    buf->b_p_ar = -1;
	    break;
# ifdef HAVE_FSYNC
	case PV_FS:
	    buf->b_p_fs = -1;
	    break;
# endif
	case PV_BKC:
	    clear_string_option(&buf->b_p_bkc);
	    buf->b_bkc_flags = 0;
	    break;
	case PV_TAGS:
	    clear_string_option(&buf->b_p_tags);
	    break;
	case PV_TC:
	    clear_string_option(&buf->b_p_tc);
	    buf->b_tc_flags = 0;
	    break;
	case PV_SISO:
	    curwin->w_p_siso = -1;
	    break;
	case PV_SO:
	    curwin->w_p_so = -1;
	    break;
	case PV_SOP:
	    curwin->w_p_sop = -1;
	    break;
# ifdef FEAT_FIND_ID
	case PV_DEF:
	    clear_string_option(&buf->b_p_def);
	    break;
	case PV_INC:
	    clear_string_option(&buf->b_p_inc);
	    break;
# endif
	case PV_COT:
	    clear_string_option(&buf->b_p_cot);
	    buf->b_cot_flags = 0;
	    break;
	case PV_DICT:
	    clear_string_option(&buf->b_p_dict);
	    break;
# ifdef FEAT_DIFF
	case PV_DIA:
	    clear_string_option(&buf->b_p_dia);
	    break;
# endif
	case PV_TSR:
	    clear_string_option(&buf->b_p_tsr);
	    break;
# ifdef FEAT_COMPL_FUNC
	case PV_TSRFU:
	    clear_string_option(&buf->b_p_tsrfu);
	    break;
# endif
	case PV_FP:
	    clear_string_option(&buf->b_p_fp);
	    break;
# ifdef FEAT_EVAL
	case PV_FFU:
	    clear_string_option(&buf->b_p_ffu);
	    break;
# endif
# ifdef FEAT_QUICKFIX
	case PV_EFM:
	    clear_string_option(&buf->b_p_efm);
	    break;
	case PV_GEFM:
	    clear_string_option(&buf->b_p_gefm);
	    break;
	case PV_GP:
	    clear_string_option(&buf->b_p_gp);
	    break;
	case PV_MP:
	    clear_string_option(&buf->b_p_mp);
	    break;
# endif
# якщо defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	case PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    break;
# endif
# якщо defined(FEAT_CRYPT)
	case PV_CM:
	    clear_string_option(&buf->b_p_cm);
	    break;
# endif
# ifdef FEAT_LINEBREAK
	case PV_SBR:
	    clear_string_option(&((win_T *)from)->w_p_sbr);
	    break;
# endif
# ifdef FEAT_STL_OPT
	case PV_STL:
	    clear_string_option(&((win_T *)from)->w_p_stl);
	    break;
	case PV_STLO:
	    clear_string_option(&((win_T *)from)->w_p_stlo);
	    break;
# endif
	case PV_UL:
	    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
	    break;
	case PV_LW:
	    clear_string_option(&buf->b_p_lw);
	    break;
	case PV_MENC:
	    clear_string_option(&buf->b_p_menc);
	    break;
	case PV_LCS:
	    clear_string_option(&((win_T *)from)->w_p_lcs);
	    set_listchars_option((win_T *)from, ((win_T *)from)->w_p_lcs, TRUE,
								      NULL, 0);
	    redraw_later(UPD_NOT_VALID);
	    break;
	case PV_FCS:
	    clear_string_option(&((win_T *)from)->w_p_fcs);
	    set_fillchars_option((win_T *)from, ((win_T *)from)->w_p_fcs, TRUE,
								      NULL, 0);
	    redraw_later(UPD_NOT_VALID);
	    break;
	case PV_VE:
	    clear_string_option(&((win_T *)from)->w_p_ve);
	    ((win_T *)from)->w_ve_flags = 0;
	    break;
    }
}
#endif

/*
 * Get pointer to option variable, depending on local or global scope.
 * "scope" can be OPT_LOCAL, OPT_GLOBAL or a combination.
 */
    static char_u *
get_varp_scope(struct vimoption *p, ціле scope)
{
    якщо ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	якщо (p->var == VAR_WIN)
	    повернути (char_u *)GLOBAL_WO(get_varp(p));
	повернути p->var;
    }
    якщо ((scope & OPT_LOCAL) && ((ціле)p->indir & PV_BOTH))
    {
	switch ((ціле)p->indir)
	{
	    case PV_FP:   повернути (char_u *)&(curbuf->b_p_fp);
#ifdef HAVE_FSYNC
	    case PV_FS:	повернути (char_u *)&(curbuf->b_p_fs);
#endif
#ifdef FEAT_EVAL
	    case PV_FFU: повернути (char_u *)&(curbuf->b_p_ffu);
#endif
#ifdef FEAT_QUICKFIX
	    case PV_EFM:  повернути (char_u *)&(curbuf->b_p_efm);
	    case PV_GEFM: повернути (char_u *)&(curbuf->b_p_gefm);
	    case PV_GP:   повернути (char_u *)&(curbuf->b_p_gp);
	    case PV_MP:   повернути (char_u *)&(curbuf->b_p_mp);
#endif
	    case PV_EP:   повернути (char_u *)&(curbuf->b_p_ep);
	    case PV_KP:   повернути (char_u *)&(curbuf->b_p_kp);
	    case PV_PATH: повернути (char_u *)&(curbuf->b_p_path);
	    case PV_AC:   повернути (char_u *)&(curbuf->b_p_ac);
	    case PV_AR:   повернути (char_u *)&(curbuf->b_p_ar);
	    case PV_TAGS: повернути (char_u *)&(curbuf->b_p_tags);
	    case PV_TC:   повернути (char_u *)&(curbuf->b_p_tc);
	    case PV_SISO: повернути (char_u *)&(curwin->w_p_siso);
	    case PV_SO:   повернути (char_u *)&(curwin->w_p_so);
	    case PV_SOP:  повернути (char_u *)&(curwin->w_p_sop);
#ifdef FEAT_FIND_ID
	    case PV_DEF:  повернути (char_u *)&(curbuf->b_p_def);
	    case PV_INC:  повернути (char_u *)&(curbuf->b_p_inc);
#endif
	    case PV_COT:  повернути (char_u *)&(curbuf->b_p_cot);
	    case PV_DICT: повернути (char_u *)&(curbuf->b_p_dict);
#ifdef FEAT_DIFF
	    case PV_DIA:  повернути (char_u *)&(curbuf->b_p_dia);
#endif
	    case PV_TSR:  повернути (char_u *)&(curbuf->b_p_tsr);
#ifdef FEAT_COMPL_FUNC
	    case PV_TSRFU: повернути (char_u *)&(curbuf->b_p_tsrfu);
#endif
#якщо defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    case PV_BEXPR: повернути (char_u *)&(curbuf->b_p_bexpr);
#endif
#якщо defined(FEAT_CRYPT)
	    case PV_CM:	  повернути (char_u *)&(curbuf->b_p_cm);
#endif
#ifdef FEAT_LINEBREAK
	    case PV_SBR:  повернути (char_u *)&(curwin->w_p_sbr);
#endif
#ifdef FEAT_STL_OPT
	    case PV_STL:  повернути (char_u *)&(curwin->w_p_stl);
	    case PV_STLO: повернути (char_u *)&(curwin->w_p_stlo);
#endif
	    case PV_UL:   повернути (char_u *)&(curbuf->b_p_ul);
	    case PV_LW:   повернути (char_u *)&(curbuf->b_p_lw);
	    case PV_BKC:  повернути (char_u *)&(curbuf->b_p_bkc);
	    case PV_MENC: повернути (char_u *)&(curbuf->b_p_menc);
	    case PV_LCS:  повернути (char_u *)&(curwin->w_p_lcs);
	    case PV_FCS:  повернути (char_u *)&(curwin->w_p_fcs);
	    case PV_VE:	  повернути (char_u *)&(curwin->w_p_ve);

	}
	повернути NULL; // "cannot happen"
    }
    повернути get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(ціле opt_idx, ціле scope)
{
    повернути get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    static char_u *
get_varp(struct vimoption *p)
{
    // hidden option, always return NULL
    якщо (p->var == NULL)
	повернути NULL;

    switch ((ціле)p->indir)
    {
	case PV_NONE:	повернути p->var;

	// global option with local value: use local value if it's been set
	case PV_EP:	повернути *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	case PV_KP:	повернути *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	case PV_PATH:	повернути *curbuf->b_p_path != NUL
				    ? (char_u *)&(curbuf->b_p_path) : p->var;
	case PV_AC:	повернути curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	case PV_AR:	повернути curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	case PV_TAGS:	повернути *curbuf->b_p_tags != NUL
				    ? (char_u *)&(curbuf->b_p_tags) : p->var;
	case PV_TC:	повернути *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	case PV_BKC:	повернути *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	case PV_SISO:	повернути curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	case PV_SO:	повернути curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	case PV_SOP:	повернути curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ifdef FEAT_FIND_ID
	case PV_DEF:	повернути *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	case PV_INC:	повернути *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#endif
	case PV_COT:	повернути *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	case PV_DICT:	повернути *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ifdef FEAT_DIFF
	case PV_DIA:	повернути *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#endif
	case PV_TSR:	повернути *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ifdef FEAT_COMPL_FUNC
	case PV_TSRFU:	повернути *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#endif
	case PV_FP:	повернути *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ifdef HAVE_FSYNC
	case PV_FS:	повернути curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#endif
#ifdef FEAT_EVAL
	case PV_FFU:	повернути *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#endif
#ifdef FEAT_QUICKFIX
	case PV_EFM:	повернути *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	case PV_GEFM:	повернути *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	case PV_GP:	повернути *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	case PV_MP:	повернути *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#endif
#якщо defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	case PV_BEXPR:	повернути *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#endif
#якщо defined(FEAT_CRYPT)
	case PV_CM:	повернути *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#endif
#ifdef FEAT_LINEBREAK
	case PV_SBR:	повернути *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#endif
#ifdef FEAT_STL_OPT
	case PV_STL:	повернути *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	case PV_STLO:	повернути *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#endif
	case PV_UL:	повернути curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	case PV_LW:	повернути *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	case PV_MENC:	повернути *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ifdef FEAT_ARABIC
	case PV_ARAB:	повернути (char_u *)&(curwin->w_p_arab);
#endif
	case PV_LIST:	повернути (char_u *)&(curwin->w_p_list);
	case PV_LCS:	повернути *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	case PV_FCS:	повернути *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	case PV_VE:	повернути *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ifdef FEAT_SPELL
	case PV_SPELL:	повернути (char_u *)&(curwin->w_p_spell);
#endif
#ifdef FEAT_SYN_HL
	case PV_CUC:	повернути (char_u *)&(curwin->w_p_cuc);
	case PV_CUL:	повернути (char_u *)&(curwin->w_p_cul);
	case PV_CULOPT:	повернути (char_u *)&(curwin->w_p_culopt);
	case PV_CC:	повернути (char_u *)&(curwin->w_p_cc);
#endif
#ifdef FEAT_DIFF
	case PV_DIFF:	повернути (char_u *)&(curwin->w_p_diff);
#endif
	case PV_EIW:	повернути (char_u *)&(curwin->w_p_eiw);
#ifdef FEAT_FOLDING
	case PV_FDC:	повернути (char_u *)&(curwin->w_p_fdc);
	case PV_FEN:	повернути (char_u *)&(curwin->w_p_fen);
	case PV_FDI:	повернути (char_u *)&(curwin->w_p_fdi);
	case PV_FDL:	повернути (char_u *)&(curwin->w_p_fdl);
	case PV_FDM:	повернути (char_u *)&(curwin->w_p_fdm);
	case PV_FML:	повернути (char_u *)&(curwin->w_p_fml);
	case PV_FDN:	повернути (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	case PV_FDE:	повернути (char_u *)&(curwin->w_p_fde);
	case PV_FDT:	повернути (char_u *)&(curwin->w_p_fdt);
# endif
	case PV_FMR:	повернути (char_u *)&(curwin->w_p_fmr);
#endif
	case PV_NU:	повернути (char_u *)&(curwin->w_p_nu);
	case PV_RNU:	повернути (char_u *)&(curwin->w_p_rnu);
#ifdef FEAT_LINEBREAK
	case PV_NUW:	повернути (char_u *)&(curwin->w_p_nuw);
#endif
	case PV_WFB:	повернути (char_u *)&(curwin->w_p_wfb);
	case PV_WFH:	повернути (char_u *)&(curwin->w_p_wfh);
	case PV_WFW:	повернути (char_u *)&(curwin->w_p_wfw);
#якщо defined(FEAT_QUICKFIX)
	case PV_PVW:	повернути (char_u *)&(curwin->w_p_pvw);
	case PV_LHI:	повернути (char_u *)&(curwin->w_p_lhi);
#endif
#ifdef FEAT_RIGHTLEFT
	case PV_RL:	повернути (char_u *)&(curwin->w_p_rl);
	case PV_RLC:	повернути (char_u *)&(curwin->w_p_rlc);
#endif
	case PV_SCROLL:	повернути (char_u *)&(curwin->w_p_scr);
	case PV_SMS:	повернути (char_u *)&(curwin->w_p_sms);
	case PV_WRAP:	повернути (char_u *)&(curwin->w_p_wrap);
#ifdef FEAT_LINEBREAK
	case PV_LBR:	повернути (char_u *)&(curwin->w_p_lbr);
	case PV_BRI:	повернути (char_u *)&(curwin->w_p_bri);
	case PV_BRIOPT: повернути (char_u *)&(curwin->w_p_briopt);
#endif
	case PV_WCR:	повернути (char_u *)&(curwin->w_p_wcr);
	case PV_SCBIND: повернути (char_u *)&(curwin->w_p_scb);
	case PV_CRBIND: повернути (char_u *)&(curwin->w_p_crb);
#ifdef FEAT_CONCEAL
	case PV_COCU:   повернути (char_u *)&(curwin->w_p_cocu);
	case PV_COLE:   повернути (char_u *)&(curwin->w_p_cole);
#endif
	case PV_WHL:	повернути (char_u *)&(curwin->w_p_whl);

#ifdef FEAT_TERMINAL
	case PV_TWK:    повернути (char_u *)&(curwin->w_p_twk);
	case PV_TWS:    повернути (char_u *)&(curwin->w_p_tws);
	case PV_TWSL:	повернути (char_u *)&(curbuf->b_p_twsl);
#endif

	case PV_AI:	повернути (char_u *)&(curbuf->b_p_ai);
	case PV_BIN:	повернути (char_u *)&(curbuf->b_p_bin);
	case PV_BOMB:	повернути (char_u *)&(curbuf->b_p_bomb);
	case PV_BH:	повернути (char_u *)&(curbuf->b_p_bh);
	case PV_BT:	повернути (char_u *)&(curbuf->b_p_bt);
	case PV_BL:	повернути (char_u *)&(curbuf->b_p_bl);
	case PV_CI:	повернути (char_u *)&(curbuf->b_p_ci);
	case PV_CIN:	повернути (char_u *)&(curbuf->b_p_cin);
	case PV_CINK:	повернути (char_u *)&(curbuf->b_p_cink);
	case PV_CINO:	повернути (char_u *)&(curbuf->b_p_cino);
	case PV_CINSD:	повернути (char_u *)&(curbuf->b_p_cinsd);
	case PV_CINW:	повернути (char_u *)&(curbuf->b_p_cinw);
	case PV_COM:	повернути (char_u *)&(curbuf->b_p_com);
	case PV_CMS:	повернути (char_u *)&(curbuf->b_p_cms);
	case PV_CPT:	повернути (char_u *)&(curbuf->b_p_cpt);
#ifdef BACKSLASH_IN_FILENAME
	case PV_CSL:	повернути (char_u *)&(curbuf->b_p_csl);
#endif
#ifdef FEAT_COMPL_FUNC
	case PV_CFU:	повернути (char_u *)&(curbuf->b_p_cfu);
	case PV_OFU:	повернути (char_u *)&(curbuf->b_p_ofu);
#endif
#ifdef FEAT_EVAL
	case PV_TFU:	повернути (char_u *)&(curbuf->b_p_tfu);
#endif
	case PV_EOF:	повернути (char_u *)&(curbuf->b_p_eof);
	case PV_EOL:	повернути (char_u *)&(curbuf->b_p_eol);
	case PV_FIXEOL:	повернути (char_u *)&(curbuf->b_p_fixeol);
	case PV_ET:	повернути (char_u *)&(curbuf->b_p_et);
	case PV_FENC:	повернути (char_u *)&(curbuf->b_p_fenc);
	case PV_FF:	повернути (char_u *)&(curbuf->b_p_ff);
	case PV_FT:	повернути (char_u *)&(curbuf->b_p_ft);
	case PV_FO:	повернути (char_u *)&(curbuf->b_p_fo);
	case PV_FLP:	повернути (char_u *)&(curbuf->b_p_flp);
	case PV_IMI:	повернути (char_u *)&(curbuf->b_p_iminsert);
	case PV_IMS:	повернути (char_u *)&(curbuf->b_p_imsearch);
	case PV_INF:	повернути (char_u *)&(curbuf->b_p_inf);
	case PV_ISK:	повернути (char_u *)&(curbuf->b_p_isk);
#ifdef FEAT_FIND_ID
# ifdef FEAT_EVAL
	case PV_INEX:	повернути (char_u *)&(curbuf->b_p_inex);
# endif
#endif
#якщо defined(FEAT_EVAL)
	case PV_INDE:	повернути (char_u *)&(curbuf->b_p_inde);
	case PV_INDK:	повернути (char_u *)&(curbuf->b_p_indk);
#endif
#ifdef FEAT_EVAL
	case PV_FEX:	повернути (char_u *)&(curbuf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
	case PV_KEY:	повернути (char_u *)&(curbuf->b_p_key);
#endif
	case PV_LISP:	повернути (char_u *)&(curbuf->b_p_lisp);
	case PV_LOP:	повернути (char_u *)&(curbuf->b_p_lop);
	case PV_ML:	повернути (char_u *)&(curbuf->b_p_ml);
	case PV_MPS:	повернути (char_u *)&(curbuf->b_p_mps);
	case PV_MA:	повернути (char_u *)&(curbuf->b_p_ma);
	case PV_MOD:	повернути (char_u *)&(curbuf->b_changed);
	case PV_NF:	повернути (char_u *)&(curbuf->b_p_nf);
	case PV_PI:	повернути (char_u *)&(curbuf->b_p_pi);
	case PV_QE:	повернути (char_u *)&(curbuf->b_p_qe);
	case PV_RO:	повернути (char_u *)&(curbuf->b_p_ro);
	case PV_SI:	повернути (char_u *)&(curbuf->b_p_si);
	case PV_SN:	повернути (char_u *)&(curbuf->b_p_sn);
	case PV_STS:	повернути (char_u *)&(curbuf->b_p_sts);
	case PV_SUA:	повернути (char_u *)&(curbuf->b_p_sua);
	case PV_SWF:	повернути (char_u *)&(curbuf->b_p_swf);
#ifdef FEAT_SYN_HL
	case PV_SMC:	повернути (char_u *)&(curbuf->b_p_smc);
	case PV_SYN:	повернути (char_u *)&(curbuf->b_p_syn);
#endif
#ifdef FEAT_SPELL
	case PV_SPC:	повернути (char_u *)&(curwin->w_s->b_p_spc);
	case PV_SPF:	повернути (char_u *)&(curwin->w_s->b_p_spf);
	case PV_SPL:	повернути (char_u *)&(curwin->w_s->b_p_spl);
	case PV_SPO:	повернути (char_u *)&(curwin->w_s->b_p_spo);
#endif
	case PV_SW:	повернути (char_u *)&(curbuf->b_p_sw);
	case PV_TS:	повернути (char_u *)&(curbuf->b_p_ts);
	case PV_TW:	повернути (char_u *)&(curbuf->b_p_tw);
	case PV_TX:	повернути (char_u *)&(curbuf->b_p_tx);
#ifdef FEAT_PERSISTENT_UNDO
	case PV_UDF:	повернути (char_u *)&(curbuf->b_p_udf);
#endif
	case PV_WM:	повернути (char_u *)&(curbuf->b_p_wm);
#ifdef FEAT_KEYMAP
	case PV_KMAP:	повернути (char_u *)&(curbuf->b_p_keymap);
#endif
#ifdef FEAT_SIGNS
	case PV_SCL:	повернути (char_u *)&(curwin->w_p_scl);
#endif
#ifdef FEAT_VARTABS
	case PV_VSTS:	повернути (char_u *)&(curbuf->b_p_vsts);
	case PV_VTS:	повернути (char_u *)&(curbuf->b_p_vts);
#endif
	default:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    повернути (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(ціле opt_idx)
{
    повернути options[opt_idx].var;
}

#якщо defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(ціле opt_idx)
{
    повернути (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    ціле
option_info(char_u *name, dict_T *d)
{
    ціле			opt_idx;
    struct vimoption	*p;
    ціле			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    якщо ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    якщо (opt_idx < 0)
	повернути FAIL;

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
    якщо (p->flags & P_BOOL)
	dict_add_bool(d, "default", (ціле)(long_i)p->def_val[dvi]);
    інакше якщо (p->flags & P_NUM)
	dict_add_number(d, "default", (long)(long_i)p->def_val[dvi]);
    інакше
	dict_add_string(d, "default", p->def_val[dvi] == NULL
					? (char_u *)"" : p->def_val[dvi]);
    повернути OK;
}
#endif

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_set_cb_T
get_option_did_set_cb(ціле opt_idx)
{
    повернути options[opt_idx].opt_did_set_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(void)
{
    якщо (*curbuf->b_p_ep == NUL)
	повернути p_ep;
    повернути curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(void)
{
#ifdef FEAT_EVAL
    якщо (*curbuf->b_p_ffu == NUL)
	повернути p_ffu;
    повернути curbuf->b_p_ffu;
#інакше
    повернути (char_u *)"";
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
    char *errmsg = update_winhighlight(wp, wp->w_p_whl);

    якщо (errmsg != NULL)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    якщо (wp->w_p_wrap)
	wp->w_leftcol = 0;
    інакше
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
    якщо (val == empty_option)
	повернути empty_option;  // no need to allocate memory
    повернути vim_strsave(val);
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
						    sizeof(to->wo_script_ctx));
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
static ціле buf_opt_idx[BV_COUNT];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    static void
init_buf_opt_idx(void)
{
    static ціле did_init_buf_opt_idx = FALSE;
    ціле i;

    якщо (did_init_buf_opt_idx)
	повернути;
    did_init_buf_opt_idx = TRUE;
    для (i = 0; !istermoption_idx(i); i++)
	якщо (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_MASK] = i;
}
#інакше
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
buf_copy_options(buf_T *buf, ціле flags)
{
    ціле		should_copy = TRUE;
    char_u	*save_p_isk = NULL;	    // init for GCC
    ціле		dont_do_help;
    ціле		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    якщо (p_cpo != NULL)
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
	якщо ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == NULL || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != NULL)))
	    should_copy = FALSE;

	якщо (should_copy || (flags & BCO_ALWAYS))
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
	    якщо (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = NULL;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    якщо (!buf->b_p_initialized)
	    {
		free_buf_options(buf, TRUE);
		buf->b_p_ro = FALSE;		// don't copy readonly
		buf->b_p_tx = p_tx;
		buf->b_p_fenc = vim_strsave(p_fenc);
		switch (*p_ffs)
		{
		    case 'm':
			buf->b_p_ff = vim_strsave((char_u *)FF_MAC); break;
		    case 'd':
			buf->b_p_ff = vim_strsave((char_u *)FF_DOS); break;
		    case 'u':
			buf->b_p_ff = vim_strsave((char_u *)FF_UNIX); break;
		    default:
			buf->b_p_ff = vim_strsave(p_ff);
		}
		якщо (buf->b_p_ff != NULL)
		    buf->b_start_ffc = *buf->b_p_ff;
		buf->b_p_bh = empty_option;
		buf->b_p_bt = empty_option;
	    }
	    інакше
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
	    якщо (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    інакше
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
	    якщо (p_vsts && p_vsts != empty_option)
		(void)tabstop_set(p_vsts, &buf->b_p_vsts_array);
	    інакше
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
#якщо defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#endif
	    buf->b_p_fp = empty_option;
#якщо defined(FEAT_EVAL)
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
#якщо defined(FEAT_EVAL)
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
#якщо defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = empty_option;
#endif
#якщо defined(FEAT_CRYPT)
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
	    якщо (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ifdef FEAT_VARTABS
		якщо (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (void)tabstop_set(p_vts, &buf->b_p_vts_array);
		інакше
		    buf->b_p_vts_array = NULL;
#endif
	    }
	    інакше
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#ifdef FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		якщо (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (void)tabstop_set(p_vts, &buf->b_p_vts_array);
		інакше
		    buf->b_p_vts_array = NULL;
#endif
		buf->b_help = false;
		якщо (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	якщо (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    якщо (did_isk)
	(void)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    void
reset_modifiable(void)
{
    ціле		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    якщо (opt_idx >= 0)
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

static ціле expand_option_idx = -1;
static ціле expand_option_start_col = 0;
static char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
static ціле expand_option_flags = 0;
static ціле expand_option_append = FALSE;

    void
set_context_in_set_cmd(
    expand_T	*xp,
    char_u	*arg,
    ціле		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    ціле		nextchar;
    long_u	flags = 0;	// init for GCC
    ціле		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    ціле		is_term_option = FALSE;
    ціле		key;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    якщо (*arg == NUL)
    {
	xp->xp_pattern = arg;
	повернути;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    якщо (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_pattern = p + 1;
	повернути;
    }
    поки (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	якщо (*p == ' ' || *p == ',')
	{
	    поки (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	якщо (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    break;
	}
	--p;
    }
    якщо (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_NO;
	p += 2;
    }
    інакше якщо (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_INV;
	p += 3;
    }
    xp->xp_pattern = arg = p;
    якщо (*arg == '<')
    {
	поки (*p != '>')
	    якщо (*p++ == NUL)	    // expand terminal option name
		повернути;
	key = get_special_key_code(arg + 1);
	якщо (key == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    повернути;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(key);
	expand_option_name[3] = KEY2TERMCAP1(key);
    }
    інакше
    {
	якщо (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    якщо (*p != NUL)
		++p;
	    якщо (*p == NUL)
		повернути;		// expand option name
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	інакше
	{
	    // Allow * wildcard
	    поки (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    якщо (*p == NUL)
		повернути;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    якщо (opt_idx == -1 || options[opt_idx].var == NULL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		повернути;
	    }
	    flags = options[opt_idx].flags;
	    якщо (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		повернути;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    ціле expand_option_subtract = FALSE;
    якщо ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	якщо (nextchar == '-')
	    expand_option_subtract = TRUE;
	якщо (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    якщо ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	повернути;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    якщо (is_term_option)
	expand_option_idx = -1;
    інакше
	expand_option_idx = opt_idx;

    якщо (!is_term_option)
    {
	якщо (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    повернути;
	}
    }

    xp->xp_pattern = p + 1;
    expand_option_start_col = (ціле)(p + 1 - xp->xp_line);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ifdef FEAT_SYN_HL
    якщо (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	повернути;
    }
#endif
    якщо (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	повернути;
    }
#ifdef FEAT_KEYMAP
    якщо (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	повернути;
    }
#endif

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    якщо (expand_option_subtract)
    {
	xp->xp_context = EXPAND_SETTING_SUBTRACT;
	повернути;
    }
    інакше якщо (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != NULL)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    інакше якщо (*xp->xp_pattern == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	повернути;
    }
    інакше
	xp->xp_context = EXPAND_NOTHING;

    якщо (is_term_option || (flags & P_NUM))
	повернути;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    якщо (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	якщо (p == (char_u *)&p_bdir
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
	    якщо (p == (char_u *)&p_path || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    інакше
		xp->xp_backslash = XP_BS_ONE;
	}
	інакше
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    якщо (p == (char_u *)&p_tags)
		xp->xp_backslash = XP_BS_THREE;
	    інакше
		xp->xp_backslash = XP_BS_ONE;
	}
	якщо (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    якщо ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	для (p = argend - 1; p >= xp->xp_pattern; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    якщо (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		поки (s > xp->xp_pattern && *(s - 1) == '\\')
		    --s;
		якщо ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#якщо defined(BACKSLASH_IN_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#інакше
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 2)
#endif
			|| (*p == ':' && (flags & P_COLON)))
		{
		    xp->xp_pattern = p + 1;
		    break;
		}
	    }
	}
    }

    // An option that is a list of single-character flags should always start
    // at the end as we don't complete words.
    якщо (flags & P_FLAGLIST)
	xp->xp_pattern = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ifdef FEAT_SPELL
    якщо (options[opt_idx].var == (char_u *)&p_sps)
    {
	якщо (STRNCMP(xp->xp_pattern, "file:", 5) == 0)
	{
	    xp->xp_pattern += 5;
	    повернути;
	}
	інакше якщо (options[expand_option_idx].opt_expand_cb != NULL)
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
    static ціле
match_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	ціле		idx,
	ціле		test_only,
	ціле		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    якщо (!fuzzy)
    {
	якщо (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    якщо (!test_only)
		matches[idx] = vim_strsave(str);
	    повернути TRUE;
	}
    }
    інакше
    {
	ціле score;

	score = fuzzy_match_str(str, fuzzystr);
	якщо (score != FUZZY_SCORE_NONE)
	{
	    якщо (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    повернути TRUE;
	}
    }

    повернути FALSE;
}

    ціле
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    ціле		*numMatches,
    char_u	***matches,
    ціле		can_fuzzy)
{
    ціле		num_normal = 0;	    // Nr of matching non-term-code settings
    ціле		num_term = 0;	    // Nr of matching terminal code settings
    ціле		opt_idx;
    ціле		match;
    ціле		count = 0;
    char_u	*str;
    ціле		loop;
    ціле		is_term_opt;
    char_u	name_buf[MAX_KEY_NAME_LEN];
    static char *(names[]) = {"all", "termcap"};
    ціле		ic = regmatch->rm_ic;	// remember the ignore-case flag
    ціле		fuzzy;
    fuzmatch_str_T  *fuzmatch = NULL;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    для (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	якщо (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    для (match = 0; match < (ціле)ARRAY_LENGTH(names); ++match)
	    {
		якщо (match_str((char_u *)names[match], regmatch, *matches,
			    count, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    якщо (loop == 0)
			num_normal++;
		    інакше
			count++;
		}
	    }
	}
	для (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != NULL;
								    opt_idx++)
	{
	    якщо (options[opt_idx].var == NULL)
		continue;
	    якщо (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		continue;
	    is_term_opt = istermoption_idx(opt_idx);
	    якщо (is_term_opt && num_normal > 0)
		continue;

	    якщо (match_str(str, regmatch, *matches, count, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		якщо (loop == 0)
		{
		    якщо (is_term_opt)
			num_term++;
		    інакше
			num_normal++;
		}
		інакше
		    count++;
	    }
	    інакше якщо (!fuzzy && options[opt_idx].shortname != NULL
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		якщо (loop == 0)
		{
		    якщо (is_term_opt)
			num_term++;
		    інакше
			num_normal++;
		}
		інакше
		    (*matches)[count++] = vim_strsave(str);
	    }
	    інакше якщо (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		якщо (match_str(name_buf, regmatch, *matches, count, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    якщо (loop == 0)
			num_term++;
		    інакше
			count++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	якщо (xp->xp_context != EXPAND_BOOL_SETTINGS  && num_normal == 0)
	{
	    для (opt_idx = 0; (str = get_termcode(opt_idx)) != NULL; opt_idx++)
	    {
		якщо (!SAFE_isprint(str[0]) || !SAFE_isprint(str[1]))
		    continue;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		якщо (match_str(name_buf, regmatch, *matches, count,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    якщо (loop == 0)
			num_term++;
		    інакше
			count++;
		}
		інакше
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    якщо (match_str(name_buf, regmatch, *matches, count,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			якщо (loop == 0)
			    num_term++;
			інакше
			    count++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    для (opt_idx = 0; (str = get_key_name(opt_idx)) != NULL; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		якщо (match_str(name_buf, regmatch, *matches, count, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    якщо (loop == 0)
			num_term++;
		    інакше
			count++;
		}
	    }
	}
	якщо (loop == 0)
	{
	    якщо (num_normal > 0)
		*numMatches = num_normal;
	    інакше якщо (num_term > 0)
		*numMatches = num_term;
	    інакше
		повернути OK;
	    якщо (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *numMatches);
		якщо (*matches == NULL)
		{
		    *matches = (char_u **)"";
		    повернути FAIL;
		}
	    }
	    інакше
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *numMatches);
		якщо (fuzmatch == NULL)
		{
		    *matches = (char_u **)"";
		    повернути FAIL;
		}
	    }
	}
    }

    якщо (fuzzy &&
	fuzzymatches_to_strmatches(fuzmatch, matches, count, FALSE) == FAIL)
	повернути FAIL;

    повернути OK;
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
    якщо (buf == NULL)
	повернути NULL;

#ifdef BACKSLASH_IN_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    для (var = buf; *var != NUL; MB_PTR_ADV(var))
	якщо (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#endif
    повернути buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    ціле
ExpandOldSetting(ціле *numMatches, char_u ***matches)
{
    char_u  *var = NULL;	// init for GCC
    char_u  *buf;

    *numMatches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    якщо (*matches == NULL)
	повернути FAIL;

    // For a terminal key code expand_option_idx is < 0.
    якщо (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	якщо (var == NULL)
	    expand_option_idx = findoption(expand_option_name);
    }

    якщо (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    інакше якщо (var == NULL)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    якщо (buf == NULL)
    {
	VIM_CLEAR(*matches);
	повернути FAIL;
    }

    (*matches)[0] = buf;
    *numMatches = 1;
    повернути OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    ціле
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    ціле		*numMatches,
    char_u	***matches)
{
    char_u  *var = NULL;	// init for GCC
    char_u  *buf;

    якщо (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == NULL)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	повернути FAIL;
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
    якщо (buf == NULL)
	повернути FAIL;

    args.oe_opt_value = buf;

    ціле num_ret = options[expand_option_idx].opt_expand_cb(&args, numMatches, matches);

    vim_free(buf);
    повернути num_ret;
}

/*
 * Expansion handler for :set-=
 */
    ціле
ExpandSettingSubtract(
    expand_T	*xp,
    regmatch_T	*regmatch,
    ціле		*numMatches,
    char_u	***matches)
{
    якщо (expand_option_idx < 0)
	// term option
	повернути ExpandOldSetting(numMatches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    якщо (option_flags & P_NUM)
	повернути ExpandOldSetting(numMatches, matches);
    інакше якщо (option_flags & P_COMMA)
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

	якщо (*option_val == NUL)
	    повернути FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	якщо (option_copy == NULL)
	    повернути FAIL;
	next_val = option_copy;

	ga_init2(&ga, sizeof(char_u *), 10);

	do
	{
	    item = next_val;
	    comma = vim_strchr(next_val, ',');
	    поки (comma != NULL && comma != next_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    якщо (comma != NULL)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		next_val = comma + 1;
	    }
	    інакше
		next_val = NULL;

	    якщо (*item == NUL)
		// empty value, don't add to list
		continue;

	    якщо (!vim_regexec(regmatch, item, (colnr_T)0))
		continue;

	    char_u *buf = escape_option_str_cmdline(item);
	    якщо (buf == NULL)
	    {
		vim_free(option_copy);
		ga_clear_strings(&ga);
		повернути FAIL;
	    }
	    якщо (ga_add_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		break;
	    }
	} поки (next_val != NULL);

	vim_free(option_copy);

	*matches = ga.ga_data;
	*numMatches = ga.ga_len;
	повернути OK;
    }
    інакше якщо (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	якщо (*xp->xp_pattern != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    повернути FAIL;
	}

	size_t num_flags = STRLEN(option_val);
	якщо (num_flags == 0)
	    повернути FAIL;

	*matches = ALLOC_MULT(char_u *, num_flags + 1);
	якщо (*matches == NULL)
	    повернути FAIL;

	ціле count = 0;
	char_u *p;

	p = vim_strnsave(option_val, num_flags);
	якщо (p == NULL)
	{
	    VIM_CLEAR(*matches);
	    повернути FAIL;
	}
	(*matches)[count++] = p;

	якщо (num_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    для (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		якщо (p == NULL)
		    break;
		(*matches)[count++] = p;
	    }
	}

	*numMatches = count;
	повернути OK;
    }

    повернути ExpandOldSetting(numMatches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    static void
option_value2string(
    struct vimoption	*opp,
    ціле			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    якщо (opp->flags & P_NUM)
    {
	long wc = 0;

	якщо (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_key_name((ціле)wc, 0));
	інакше якщо (wc != 0)
	    STRCPY(NameBuff, transchar((ціле)wc));
	інакше
	    sprintf((char *)NameBuff, "%ld", *(long *)varp);
    }
    інакше    // P_STRING
    {
	varp = *(char_u **)(varp);
	якщо (varp == NULL)		    // just in case
	    NameBuff[0] = NUL;
#ifdef FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	інакше якщо (opp->var == (char_u *)&p_key && *varp)
	    STRCPY(NameBuff, "*****");
#endif
	інакше якщо (opp->flags & P_EXPAND)
	    home_replace(NULL, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	інакше якщо ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	інакше
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    static ціле
wc_use_keyname(char_u *varp, long *wcp)
{
    якщо (((long *)varp == &p_wc) || ((long *)varp == &p_wcm))
    {
	*wcp = *(long *)varp;
	якщо (IS_SPECIAL(*wcp) || find_special_key_in_table((ціле)*wcp) >= 0)
	    повернути TRUE;
    }
    повернути FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    ціле
shortmess(ціле x)
{
    повернути p_shm != NULL &&
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
    ціле		opt_idx;
    ціле		dofree = FALSE;
    char_u	*p;

    якщо (!option_was_set((char_u *)"cp"))
    {
	p_cp = FALSE;
	для (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    якщо (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    якщо (fname != NULL)
    {
	p = vim_getenv(envname, &dofree);
	якщо (p == NULL)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = FullName_save(fname, FALSE);
	    якщо (p != NULL)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	інакше якщо (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    void
change_compatible(ціле on)
{
    ціле	    opt_idx;

    якщо (p_cp != on)
    {
	p_cp = on;
	compatible_set();
    }
    opt_idx = findoption((char_u *)"cp");
    якщо (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_SET;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    ціле
option_was_set(char_u *name)
{
    ціле idx;

    idx = findoption(name);
    якщо (idx < 0)	// unknown option
	повернути FALSE;
    якщо (options[idx].flags & P_WAS_SET)
	повернути TRUE;
    повернути FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    ціле
reset_option_was_set(char_u *name)
{
    ціле idx = findoption(name);

    якщо (idx < 0)
	повернути FAIL;

    options[idx].flags &= ~P_WAS_SET;
    повернути OK;
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
    ціле	    opt_idx;

    для (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	якщо (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    set_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    ціле
can_bs(
    ціле		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ifdef FEAT_JOB_CHANNEL
    якщо (what == BS_START && bt_prompt(curbuf))
	повернути FALSE;
#endif
    switch (*p_bs)
    {
	case '3':       повернути TRUE;
	case '2':	повернути (what != BS_NOSTOP);
	case '1':	повернути (what != BS_START);
	case '0':	повернути FALSE;
    }
    повернути vim_strchr(p_bs, what) != NULL;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloff_value(void)
{
    повернути curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloffpad_value(void)
{
    повернути curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_sidescrolloff_value(void)
{
    повернути curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    unsigned ціле
get_bkc_flags(buf_T *buf)
{
    повернути buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#якщо defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_value(buf_T *buf)
{
    якщо (buf->b_p_flp == NULL || *buf->b_p_flp == NUL)
	повернути p_flp;
    повернути buf->b_p_flp;
}
#endif

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    unsigned ціле
get_ve_flags(void)
{
    повернути (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#якщо defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_value(win_T *win)
{
    якщо (win->w_p_sbr == NULL || *win->w_p_sbr == NUL)
	повернути p_sbr;
    якщо (STRCMP(win->w_p_sbr, "NONE") == 0)
	повернути empty_option;
    повернути win->w_p_sbr;
}
#endif

#якщо defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(ціле bufopt)
{
    dict_T	*d;
    ціле		opt_idx;

    d = dict_alloc();
    якщо (d == NULL)
	повернути NULL;

    для (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	struct vimoption *opt = &options[opt_idx];

	якщо ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    якщо (varp != NULL)
	    {
		якщо (opt->flags & P_STRING)
		    dict_add_string(d, opt->fullname, *(char_u **)varp);
		інакше якщо (opt->flags & P_NUM)
		    dict_add_number(d, opt->fullname, *(long *)varp);
		інакше
		    dict_add_number(d, opt->fullname, *(ціле *)varp);
	    }
	}
    }

    повернути d;
}
#endif

#якщо defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    ціле
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    якщо (val == NULL)
	p = wp->w_p_culopt;
    інакше
	p = val;
    поки (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	якщо (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_LINE;
	}
	інакше якщо (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_LINE | CULOPT_NBR;
	}
	інакше якщо (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	інакше якщо (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	якщо (*p != ',' && *p != NUL)
	    повернути FAIL;
	якщо (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    якщо ((culopt_flags_new & CULOPT_LINE) && (culopt_flags_new & CULOPT_SCRLINE))
	повернути FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    повернути OK;
}
#endif

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    ціле
magic_isset(void)
{
    switch (magic_overruled)
    {
	case OPTION_MAGIC_ON:      повернути TRUE;
	case OPTION_MAGIC_OFF:     повернути FALSE;
	case OPTION_MAGIC_NOT_SET: break;
    }
#ifdef FEAT_EVAL
    якщо (in_vim9script())
	повернути TRUE;
#endif
    повернути p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    ціле
option_set_callback_func(char_u *optval UNUSED, callback_T *optcb UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    ціле		funcname = FALSE;

    якщо (optval == NULL || *optval == NUL)
    {
	free_callback(optcb);
	повернути OK;
    }

    якщо (*optval == '{' || (in_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_expr(optval, NULL);
    інакше
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    якщо (tv == NULL)
	повернути FAIL;

    cb = get_callback(tv);
    якщо (cb.cb_name == NULL || *cb.cb_name == NUL)
    {
	free_tv(tv);
	повернути FAIL;
    }

    free_callback(optcb);
    set_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = NULL;
    якщо (in_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != NULL))
    {
	якщо (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = find_imported(optval, dot - optval, FALSE);
	якщо (import != NULL && SCRIPT_ID_VALID(import->imp_sid)
				&& !(import->imp_flags & IMP_FLAGS_AUTOLOAD))
	{
	    char_u	fnamebuf[MAX_FUNC_NAME_LEN];

	    // Imported non-autoloaded function.  Replace the import script
	    // name (import.funcname) with the script ID (<SNR>123_funcname)
	    vim_snprintf((char *)fnamebuf, sizeof(fnamebuf), "<SNR>%d_%s",
						import->imp_sid, dot + 1);
	    optcb->cb_name = vim_strsave(fnamebuf);
	    optcb->cb_free_name = TRUE;
	}
	інакше
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

    повернути OK;
#інакше
    повернути FAIL;
#endif
}

#якщо defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    char *
did_set_showtabpanel(optset_T *args UNUSED)
{
    shell_new_columns();
    повернути NULL;
}
#endif

#якщо defined(FEAT_EVAL)
    static void
didset_options_sctx(ціле opt_flags, char **buf)
{
	для (ціле i = 0; ; ++i)
	{
	    якщо (buf[i] == NULL)
		break;

	    ціле idx = findoption((char_u *)buf[i]);
	    якщо (idx >= 0)
		set_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#endif
