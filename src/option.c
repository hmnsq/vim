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

static void set_options_default(số_nguyên opt_flags);
static void set_string_default_esc(char *name, char_u *val, số_nguyên escape);
static char_u *find_dup_item(char_u *origval, char_u *newval, size_t newvallen, long_u flags);
static char_u *option_expand(số_nguyên opt_idx, char_u *val);
static void didset_options(void);
static void didset_options2(void);
#nếu defined(FEAT_EVAL)
static long_u *insecure_flag(win_T *wp, số_nguyên opt_idx, số_nguyên opt_flags);
#không_thì
# define insecure_flag(wp, opt_idx, opt_flags) (&options[opt_idx].flags)
#endif
static char *set_bool_option(số_nguyên opt_idx, char_u *varp, số_nguyên value, số_nguyên opt_flags);
static char *set_num_option(số_nguyên opt_idx, char_u *varp, long value, char *errbuf, size_t errbuflen, số_nguyên opt_flags);
static số_nguyên find_key_option(char_u *arg_arg, số_nguyên has_lt);
static void showoptions(số_nguyên all, số_nguyên opt_flags);
static số_nguyên optval_default(struct vimoption *, char_u *varp, số_nguyên compatible);
static void showoneopt(struct vimoption *, số_nguyên opt_flags);
static số_nguyên put_setstring(FILE *fd, bool legacy, char *cmd, char *name, char_u **valuep, long_u flags);
static số_nguyên put_setnum(FILE *fd, char *cmd, char *name, long *valuep);
static số_nguyên put_setbool(FILE *fd, char *cmd, char *name, số_nguyên value);
static số_nguyên istermoption(struct vimoption *p);
static số_nguyên istermoption_idx(số_nguyên opt_idx);
static char_u *get_varp_scope(struct vimoption *p, số_nguyên scope);
static char_u *get_varp(struct vimoption *);
static void check_win_options(win_T *win);
static void option_value2string(struct vimoption *, số_nguyên scope);
static void check_winopt(winopt_T *wop);
static số_nguyên wc_use_keyname(char_u *varp, long *wcp);
static void compatible_set(void);

#nếu defined(FEAT_EVAL)
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
static void didset_options_sctx(số_nguyên opt_flags, char **buf);
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
    nếu (((p = mch_getenv((char_u *)"SHELL")) != NULL && *p != NUL)
#nếu defined(MSWIN)
	    || ((p = mch_getenv((char_u *)"COMSPEC")) != NULL && *p != NUL)
	    || ((p = (char_u *)default_shell()) != NULL && *p != NUL)
#endif
	    )
#nếu defined(MSWIN)
    {
	// For MS-Windows put the path in quotes instead of escaping spaces.
	char_u	    *cmd;
	size_t	    len;

	nếu (vim_strchr(p, ' ') != NULL)
	{
	    len = STRLEN(p) + 3;  // two quotes and a trailing NUL
	    cmd = alloc(len);
	    nếu (cmd != NULL)
	    {
		vim_snprintf((char *)cmd, len, "\"%s\"", p);
		set_string_default("sh", cmd);
		vim_free(cmd);
	    }
	}
	không_thì
	    set_string_default("sh", p);
    }
#không_thì
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
    số_nguyên		opt_idx;
    số_nguyên		i;
    char_u	*p;
    số_nguyên		plen;
#ifdef UNIX
    static char	*(names[4]) = {"", "TMPDIR", "TEMP", "TMP"};
#không_thì
    static char	*(names[3]) = {"TMPDIR", "TEMP", "TMP"};
#endif
    garray_T	ga;

    opt_idx = findoption((char_u *)"backupskip");

    ga_init2(&ga, 1, 100);
    cho (i = 0; i < (số_nguyên)ARRAY_LENGTH(names); ++i)
    {
	số_nguyên		mustfree = FALSE;
#ifdef UNIX
	nếu (*names[i] == NUL)
	{
# ifdef MACOS_X
	    p = (char_u *)"/private/tmp";
	    plen = (số_nguyên)STRLEN_LITERAL("/private/tmp");
# không_thì
	    p = (char_u *)"/tmp";
	    plen = (số_nguyên)STRLEN_LITERAL("/tmp");
# endif
	}
	không_thì
#endif
	{
	    p = vim_getenv((char_u *)names[i], &mustfree);
	    plen = 0;	    // will be calculated below
	}
	nếu (p != NULL && *p != NUL)
	{
	    char_u  *item;
	    size_t  itemsize;
	    số_nguyên	    has_trailing_path_sep = FALSE;

	    nếu (plen == 0)
	    {
		// the value was retrieved from the environment
		plen = (số_nguyên)STRLEN(p);
		// does the value include a trailing path separator?
		nếu (after_pathsep(p, p + plen))
		    has_trailing_path_sep = TRUE;
	    }

	    // item size needs to be large enough to include "/*" and a trailing NUL
	    // note: the value (and therefore plen) may already include a path separator
	    itemsize = plen + (has_trailing_path_sep ? 0 : 1) + 2;
	    item = alloc(itemsize);
	    nếu (item != NULL)
	    {
		// add a preceding comma as a separator after the first item
		size_t	itemseplen = (ga.ga_len == 0) ? 0 : 1;
		size_t	itemlen;

		itemlen = vim_snprintf((char *)item, itemsize, "%s%s*", p, (has_trailing_path_sep) ? "" : PATHSEPSTR);

		nếu (find_dup_item(ga.ga_data, item, itemlen, options[opt_idx].flags) == NULL
			&& ga_grow(&ga, (số_nguyên)(itemseplen + itemlen + 1)) == OK)
		{
		    ga.ga_len += vim_snprintf((char *)ga.ga_data + ga.ga_len,
				    itemseplen + itemlen + 1,
				    "%s%s", (itemseplen > 0) ? "," : "", item);
		}
		vim_free(item);
	    }
	}
	nếu (mustfree)
	    vim_free(p);
    }
    nếu (ga.ga_data != NULL)
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
    số_nguyên		opt_idx;
    long_u	n;

    opt_idx = findoption((char_u *)"maxmemtot");
    nếu (opt_idx < 0)
	trả_về;

#nếu !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
    nếu (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
#endif
    {
#nếu defined(HAVE_AVAIL_MEM)
	// Use amount of memory available at this moment.
	n = (mch_avail_mem(FALSE) >> 1);
#elif defined(HAVE_TOTAL_MEM)
	// Use amount of memory available to Vim.
	n = (mch_total_mem(FALSE) >> 1);
#không_thì
	n = (0x7fffffff >> 11);
#endif
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	nếu (opt_idx >= 0)
	{
#nếu !defined(HAVE_AVAIL_MEM) && !defined(HAVE_TOTAL_MEM)
	    nếu ((long)(long_i)options[opt_idx].def_val[VI_DEFAULT] > (long)n
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
    số_nguyên		opt_idx;
    char_u	*cdpath;
    char_u	*buf;
    số_nguyên	i;
    số_nguyên	j;
    số_nguyên	mustfree = FALSE;

    cdpath = vim_getenv((char_u *)"CDPATH", &mustfree);
    nếu (cdpath == NULL)
	trả_về;

    buf = alloc((STRLEN(cdpath) << 1) + 2);
    nếu (buf != NULL)
    {
	buf[0] = ',';	    // start with ",", current dir first
	j = 1;
	cho (i = 0; cdpath[i] != NUL; ++i)
	{
	    nếu (vim_ispathlistsep(cdpath[i]))
		buf[j++] = ',';
	    không_thì
	    {
		nếu (cdpath[i] == ' ' || cdpath[i] == ',')
		    buf[j++] = '\\';
		buf[j++] = cdpath[i];
	    }
	}
	buf[j] = NUL;
	opt_idx = findoption((char_u *)"cdpath");
	nếu (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = buf;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
	không_thì
	    vim_free(buf); // cannot happen
    }
    nếu (mustfree)
	vim_free(cdpath);
}

/*
 * Initialize the 'printencoding' option to a default value.
 */
    static void
set_init_default_printencoding(void)
{
#nếu defined(FEAT_POSTSCRIPT) && \
    (defined(MSWIN) || defined(VMS) || defined(MAC) || defined(hpux))
    // Set print encoding on platforms that don't default to latin1
    set_string_default("penc",
# nếu defined(MSWIN)
	    (char_u *)"cp1252"
# elif defined(VMS)
	    (char_u *)"dec-mcs"
# elif defined(MAC)
	    (char_u *)"mac-roman"
# không_thì // HPUX
	    (char_u *)"hp-roman8"
# endif
	    );
#endif
}

#nếu defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
/*
 * Initialize the 'printexpr' option to a default value.
 */
    static void
set_init_default_printexpr(void)
{
    // 'printexpr' must be allocated to be able to evaluate it.
    set_string_default("pexpr",
# nếu defined(MSWIN)
	    (char_u *)"system('copy' . ' ' . v:fname_in . (&printdevice == '' ? ' LPT1:' : (' \"' . &printdevice . '\"'))) . delete(v:fname_in)"
# elif defined(VMS)
	    (char_u *)"system('print/delete' . (&printdevice == '' ? '' : ' /queue=' . &printdevice) . ' ' . v:fname_in)"

# không_thì
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
    nếu (p == NULL)
	trả_về;
    nếu (fnamecmp(p, "nologin") == 0 || fnamecmp(p, "false") == 0)
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
    số_nguyên		opt_idx;

    opt_idx = findoption((char_u *)"runtimepath");
    nếu (opt_idx >= 0)
    {
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)CLEAN_RUNTIMEPATH;
	p_rtp = (char_u *)CLEAN_RUNTIMEPATH;
    }
    opt_idx = findoption((char_u *)"packpath");
    nếu (opt_idx < 0)
	trả_về;
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
    số_nguyên		opt_idx;
    số_nguyên		has_xdg_env = TRUE;
    số_nguyên		should_free_xdg_dir = FALSE;
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
    nếu (!xdg_dir)
    {
	xdg_dir = expand_env_save((char_u *)"~/.config");
	should_free_xdg_dir = TRUE;
	has_xdg_env = FALSE;
    }
    concat_fnames(xdg_dir, STRLEN(xdg_dir),
	(char_u *)"vim/vimrc", STRLEN_LITERAL("vim/vimrc"), TRUE, &vimrc_xdg);

    nếu (file_is_readable(vimrc1) || file_is_readable(vimrc2) ||
	    !file_is_readable(vimrc_xdg.string))
	goto theend;

    xdg_rtp = has_xdg_env ? (char_u *)XDG_RUNTIMEPATH
	: (char_u *)XDG_RUNTIMEPATH_FB;

    nếu ((opt_idx = findoption((char_u *)"runtimepath")) < 0)
	goto theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_rtp = xdg_rtp;

    nếu ((opt_idx = findoption((char_u *)"packpath")) < 0)
	goto theend;

    options[opt_idx].def_val[VI_DEFAULT] = xdg_rtp;
    p_pp = xdg_rtp;

# nếu defined(XDG_VDIR) && defined(FEAT_SESSION)
    nếu ((opt_idx = findoption((char_u *)"viewdir")) < 0)
	goto theend;

    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)XDG_VDIR;
    p_vdir = (char_u *)XDG_VDIR;
# endif

theend:
    vim_free(vimrc1);
    vim_free(vimrc2);
    vim_free(vimrc_xdg.string);
    nếu (should_free_xdg_dir)
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
    số_nguyên		opt_idx;
    char_u	*p;

    cho (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	nếu ((options[opt_idx].flags & P_GETTEXT)
		&& options[opt_idx].var != NULL)
	    p = (char_u *)_(*(char **)options[opt_idx].var);
	không_thì
	    p = option_expand(opt_idx, NULL);
	nếu (p != NULL && (p = vim_strsave(p)) != NULL)
	{
	    *(char_u **)options[opt_idx].var = p;
	    // VIMEXP
	    // Defaults for all expanded options are currently the same for Vi
	    // and Vim.  When this changes, add some code here!  Also need to
	    // split P_DEF_ALLOCED in two.
	    nếu (options[opt_idx].flags & P_DEF_ALLOCED)
		vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }
}

#nếu defined(MSWIN) && defined(FEAT_GETTEXT)
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

    nếu (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num_languages, NULL,
								    &bufsize)
	    || bufsize == 0)
	trả_về NULL;

    buffer = ALLOC_MULT(WCHAR, bufsize);
    nếu (buffer == NULL)
	trả_về NULL;

    nếu (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &num_languages, buffer,
								    &bufsize))
    {
	// The list is NUL separated, the result needs the same room.
	langs = alloc(bufsize);
	nếu (langs != NULL)
	{
	    char_u	*d = langs;
	    WCHAR	*s = buffer;

	    trong_khi (*s != L'\0')
	    {
		bool	english = s[0] == L'e' && s[1] == L'n'
					&& (s[2] == L'\0' || s[2] == L'-');

		nếu (d > langs)
		    *d++ = ':';
		// Locale names are ASCII, "en-US" becomes "en_US".
		cho ( ; *s != L'\0'; ++s)
		    *d++ = *s == L'-' ? '_' : (char_u)*s;
		++s;

		nếu (english)
		    break;
	    }
	    *d = NUL;
	}
    }
    vim_free(buffer);

    trả_về langs;
}
#endif

/*
 * Initialize the 'LANG' environment variable to a default value.
 */
    static void
set_init_lang_env(void)
{
#nếu defined(MSWIN) && defined(FEAT_GETTEXT)
    // If the language isn't set in the environment, use the display language
    // of Windows.  Not the regional format, which is what the CRT would use
    // for setlocale(LC_ALL, "").
    nếu (mch_getenv((char_u *)"LANG") == NULL
	    && mch_getenv((char_u *)"LANGUAGE") == NULL
	    && mch_getenv((char_u *)"LC_ALL") == NULL
	    && mch_getenv((char_u *)"LC_MESSAGES") == NULL)
    {
	char_u	*langs = get_ui_langs();

	nếu (langs != NULL && *langs != NUL)
	{
	    char_u	*colon = vim_strchr(langs, ':');

	    // $LANGUAGE is the list gettext picks from, $LANG the language.
	    vim_setenv((char_u *)"LANGUAGE", langs);
	    nếu (colon != NULL)
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
    số_nguyên		opt_idx;

#nếu defined(MSWIN) || defined(__MVS__)
    // MS-Windows has builtin support for conversion to and from Unicode, using
    // "utf-8" for 'encoding' should work best for most users.
    // z/OS built should default to UTF-8 mode as setlocale does not respect utf-8 environment variable locales
    p = vim_strnsave((char_u *)ENC_DFLT, STRLEN_LITERAL(ENC_DFLT));
#không_thì
    // enc_locale() will try to find the encoding of the current locale.
    // This works best for properly configured systems, old and new.
    p = enc_locale();
#endif
    nếu (p == NULL)
	trả_về;

    // Try setting 'encoding' and check if the value is valid.
    // If not, go back to the default encoding.
    char_u *save_enc = p_enc;
    p_enc = p;
    nếu (STRCMP(p_enc, "gb18030") == 0)
    {
	// We don't support "gb18030", but "cp936" is a good substitute
	// for practical purposes, thus use that.  It's not an alias to
	// still support conversion between gb18030 and utf-8.
	p_enc = vim_strnsave((char_u *)"cp936", STRLEN_LITERAL("cp936"));
	vim_free(p);
    }
    nếu (mb_init() == NULL)
    {
	opt_idx = findoption((char_u *)"encoding");
	nếu (opt_idx >= 0)
	{
	    options[opt_idx].def_val[VI_DEFAULT] = p_enc;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}

#nếu defined(MSWIN) || defined(MACOS_X) || defined(VMS)
	nếu (STRCMP(p_enc, "latin1") == 0 || enc_utf8)
	{
	    // Adjust the default for 'isprint' and 'iskeyword' to match
	    // latin1.  Also set the defaults for when 'nocompatible' is
	    // set.
	    set_string_option_direct((char_u *)"isp", -1,
		    ISP_LATIN1, OPT_FREE, SID_NONE);
	    set_string_option_direct((char_u *)"isk", -1,
		    ISK_LATIN1, OPT_FREE, SID_NONE);
	    opt_idx = findoption((char_u *)"isp");
	    nếu (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISP_LATIN1;
	    opt_idx = findoption((char_u *)"isk");
	    nếu (opt_idx >= 0)
		options[opt_idx].def_val[VIM_DEFAULT] = ISK_LATIN1;
	    (void)init_chartab();
	}
#endif

#nếu defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	// Win32 console: When GetACP() returns a different value from
	// GetConsoleCP() set 'termencoding'.
	nếu (
# ifdef VIMDLL
		(!gui.in_use && !gui.starting) &&
# endif
		GetACP() != GetConsoleCP())
	{
	    char	buf[50];
	    size_t	buflen;

	    // Win32 console: In ConPTY, GetConsoleCP() returns zero.
	    // Use an alternative value.
	    nếu (GetConsoleCP() == 0)
		buflen = vim_snprintf(buf, sizeof(buf), "cp%ld", (long)GetACP());
	    không_thì
		buflen = vim_snprintf(buf, sizeof(buf), "cp%ld", (long)GetConsoleCP());
	    p_tenc = vim_strnsave((char_u *)buf, buflen);
	    nếu (p_tenc != NULL)
	    {
		opt_idx = findoption((char_u *)"termencoding");
		nếu (opt_idx >= 0)
		{
		    options[opt_idx].def_val[VI_DEFAULT] = p_tenc;
		    options[opt_idx].flags |= P_DEF_ALLOCED;
		}
		convert_setup(&input_conv, p_tenc, p_enc);
		convert_setup(&output_conv, p_enc, p_tenc);
	    }
	    không_thì
		p_tenc = empty_option;
	}
#endif
#nếu defined(MSWIN)
	// $HOME may have characters in active code page.
	init_homedir();
#endif
    }
    không_thì
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
set_init_1(số_nguyên clean_arg)
{
#ifdef FEAT_LANGMAP
    langmap_init();
#endif

    // Be Vi compatible by default
    p_cp = TRUE;

    // Use POSIX compatibility when $VIM_POSIX is set.
    nếu (mch_getenv((char_u *)"VIM_POSIX") != NULL)
    {
	set_string_default("cpo", (char_u *)CPO_ALL);
	set_string_default("shm", (char_u *)SHM_POSIX);
    }

    set_init_default_shell();
    set_init_default_backupskip();
    set_init_default_maxmemtot();
    set_init_default_cdpath();
    set_init_default_printencoding();
#nếu defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
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
    nếu (clean_arg)
	set_init_clean_rtp();
#endif

#ifdef FEAT_GUI
    nếu (found_reverse_arg)
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

#nếu defined(FEAT_ARABIC)
    // Detect use of mlterm.
    // Mlterm is a terminal emulator akin to xterm that has some special
    // abilities (bidi namely).
    // NOTE: mlterm's author is being asked to 'set' a variable
    //       instead of an environment variable due to inheritance.
    nếu (mch_getenv((char_u *)"MLTERM") != NULL)
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
    số_nguyên		opt_idx,
    số_nguyên		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    số_nguyên		compatible)	// use Vi default value
{
    char_u	*varp;		// pointer to variable for current option
    số_nguyên		dvi;		// index in def_val[]
    long_u	flags;
    long_u	*flagsp;
    số_nguyên		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    varp = get_varp_scope(&(options[opt_idx]), both ? OPT_LOCAL : opt_flags);
    flags = options[opt_idx].flags;
    nếu (varp != NULL)	    // skip hidden option, nothing to do for it
    {
	dvi = ((flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
	nếu (flags & P_STRING)
	{
	    // 'fencs' default value depends on 'encoding'
	    nếu (options[opt_idx].var == (char_u *)&p_fencs && enc_utf8)
		set_fencs_unicode();
	    // Use set_string_option_direct() for local options to handle
	    // freeing and allocating the value.
	    không_thì nếu (options[opt_idx].indir != PV_NONE)
		set_string_option_direct(NULL, opt_idx,
				 options[opt_idx].def_val[dvi], opt_flags, 0);
	    không_thì
	    {
		nếu ((opt_flags & OPT_FREE) && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	không_thì nếu (flags & P_NUM)
	{
	    nếu (options[opt_idx].indir == PV_SCROLL)
		win_comp_scroll(curwin);
	    không_thì
	    {
		long def_val = (long)(long_i)options[opt_idx].def_val[dvi];

		nếu ((long *)varp == &curwin->w_p_so
			|| (long *)varp == &curwin->w_p_siso
			|| (long *)varp == &curwin->w_p_sop)
		    // 'scrolloff', 'sidescrolloff', and 'scrolloffpad' local values have a
		    // different default value than the global default.
		    *(long *)varp = -1;
		không_thì
		    *(long *)varp = def_val;
		// May also set global value for local option.
		nếu (both)
		    *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								def_val;
	    }
	}
	không_thì	// P_BOOL
	{
	    // the cast to long is required for Manx C, long_i is needed for
	    // MSVC
	    *(số_nguyên *)varp = (số_nguyên)(long)(long_i)options[opt_idx].def_val[dvi];
#ifdef UNIX
	    // 'modeline' defaults to off for root
	    nếu (options[opt_idx].indir == PV_ML && getuid() == ROOT_UID)
		*(số_nguyên *)varp = FALSE;
#endif
	    // May also set global value for local option.
	    nếu (both)
		*(số_nguyên *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) =
								*(số_nguyên *)varp;
	}

	// The default value is not insecure.
	flagsp = insecure_flag(curwin, opt_idx, opt_flags);
	*flagsp = *flagsp & ~P_INSECURE;
	nếu (both)
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
    số_nguyên		opt_flags)	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
{
    số_nguyên		i;
    win_T	*wp;
    tabpage_T	*tp;

    cho (i = 0; !istermoption_idx(i); i++)
	nếu (!(options[i].flags & P_NODEFAULT)
		&& (opt_flags == 0
		    || (options[i].var != (char_u *)&p_enc
#nếu defined(FEAT_CRYPT)
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
set_string_default_esc(char *name, char_u *val, số_nguyên escape)
{
    char_u	*p;
    số_nguyên		opt_idx;

    nếu (escape && vim_strchr(val, ' ') != NULL)
	p = vim_strsave_escaped(val, (char_u *)" ");
    không_thì
	p = vim_strsave(val);
    nếu (p == NULL)		// we don't want a NULL
	trả_về;

    opt_idx = findoption((char_u *)name);
    nếu (opt_idx < 0)
    {
	vim_free(p);
	trả_về;
    }

    nếu (options[opt_idx].flags & P_DEF_ALLOCED)
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
    số_nguyên	    bs = 0;
    char_u  *s;

    nếu (origval == NULL)
	trả_về NULL;

    cho (s = origval; *s != NUL; ++s)
    {
	nếu ((!(flags & P_COMMA)
		    || s == origval
		    || (s[-1] == ',' && !(bs & 1)))
		&& STRNCMP(s, newval, newvallen) == 0
		&& (!(flags & P_COMMA)
		    || s[newvallen] == ','
		    || s[newvallen] == NUL))
	    trả_về s;
	// Count backslashes.  Only a comma with an even number of backslashes
	// or a single backslash preceded by a comma before it is recognized as
	// a separator.
	nếu ((s > origval + 1
		    && s[-1] == '\\'
		    && s[-2] != ',')
		|| (s == origval + 1
		    && s[-1] == '\\'))
	    ++bs;
	không_thì
	    bs = 0;
    }
    trả_về NULL;
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'lines' and 'columns'.
 */
    void
set_number_default(char *name, long val)
{
    số_nguyên		opt_idx;

    opt_idx = findoption((char_u *)name);
    nếu (opt_idx >= 0)
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
}

#nếu defined(FEAT_PROP_POPUP)
/*
 * Set all window-local and buffer-local options to the Vim default.
 * local-global options will use the global value.
 * When "do_buffer" is FALSE don't set buffer-local options.
 */
    void
set_local_options_default(win_T *wp, số_nguyên do_buffer)
{
    win_T	*save_curwin = curwin;
    số_nguyên		i;

    curwin = wp;
    curbuf = curwin->w_buffer;
    block_autocmds();

    cho (i = 0; !istermoption_idx(i); i++)
    {
	struct vimoption    *p = &(options[i]);
	char_u		    *varp = get_varp_scope(p, OPT_LOCAL);

	nếu (p->indir != PV_NONE
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

#nếu defined(EXITFREE)
/*
 * Free all options.
 */
    void
free_all_options(void)
{
    số_nguyên		i;

    cho (i = 0; !istermoption_idx(i); i++)
    {
	nếu (options[i].indir == PV_NONE)
	{
	    // global option: free value and default value.
	    nếu ((options[i].flags & P_ALLOCED) && options[i].var != NULL)
		free_string_option(*(char_u **)options[i].var);
	    nếu (options[i].flags & P_DEF_ALLOCED)
		free_string_option(options[i].def_val[VI_DEFAULT]);
	}
	không_thì nếu (options[i].var != VAR_WIN
		&& (options[i].flags & P_STRING))
	    // buffer-local option: free global value
	    clear_string_option((char_u **)options[i].var);
    }
    free_operatorfunc_option();
    free_tagfunc_option();
# nếu defined(FEAT_EVAL)
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
    số_nguyên		idx;

    // 'scroll' defaults to half the window height. The stored default is zero,
    // which results in the actual value computed from the window height.
    idx = findoption((char_u *)"scroll");
    nếu (idx >= 0 && !(options[idx].flags & P_WAS_SET))
	set_option_default(idx, OPT_LOCAL, p_cp);
    comp_col();

    // 'window' is only for backwards compatibility with Vi.
    // Default is Rows - 1.
    nếu (!option_was_set((char_u *)"window"))
	p_window = Rows - 1;
    set_number_default("window", Rows - 1);

    // For DOS console the default is always black.
#nếu !((defined(MSWIN)) && !defined(FEAT_GUI))
    // If 'background' wasn't set by the user, try guessing the value,
    // depending on the terminal name.  Only need to check for terminals
    // with a dark background, that can handle color.
    idx = findoption((char_u *)"bg");
    nếu (idx >= 0 && !(options[idx].flags & P_WAS_SET)
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
#nếu defined(UNIX) || defined(MSWIN)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    số_nguyên	    idx_srr;
    số_nguyên	    do_srr;
# ifdef FEAT_QUICKFIX
    số_nguyên	    idx_sp;
    số_nguyên	    do_sp;
# endif

    idx_srr = findoption((char_u *)"srr");
    nếu (idx_srr < 0)
	do_srr = FALSE;
    không_thì
	do_srr = !(options[idx_srr].flags & P_WAS_SET);
# ifdef FEAT_QUICKFIX
    idx_sp = findoption((char_u *)"sp");
    nếu (idx_sp < 0)
	do_sp = FALSE;
    không_thì
	do_sp = !(options[idx_sp].flags & P_WAS_SET);
# endif
    p = get_isolated_shell_name();
    nếu (p != NULL)
    {
	// Default for p_sp is "| tee", for p_srr is ">".
	// For known shells it is changed here to include stderr.
	nếu (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# nếu defined(MSWIN)	// also check with .exe extension
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
# nếu defined(FEAT_QUICKFIX)
	    nếu (do_sp)
	    {
#  ifdef MSWIN
		p_sp = (char_u *)">&";
#  không_thì
		p_sp = (char_u *)"|& tee";
#  endif
		options[idx_sp].def_val[VI_DEFAULT] = p_sp;
	    }
# endif
	    nếu (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx_srr].def_val[VI_DEFAULT] = p_srr;
	    }
	}
# ifdef MSWIN
	// Windows PowerShell output is UTF-16 with BOM so re-encode to the
	// current codepage.
	không_thì nếu (   fnamecmp(p, "powershell") == 0
		    || fnamecmp(p, "powershell.exe") == 0
		)
	{
#  nếu defined(FEAT_QUICKFIX)
		nếu (do_sp)
		{
		    p_sp = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
#  endif
		nếu (do_srr)
		{
		    p_srr = (char_u *)"2>&1 | Out-File -Encoding default";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	}
# endif
	không_thì
	    // Always use POSIX shell style redirection if we reach this
	    nếu (       fnamecmp(p, "sh") == 0
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
# nếu defined(FEAT_QUICKFIX)
		nếu (do_sp)
		{
#  ifdef MSWIN
		    p_sp = (char_u *)">%s 2>&1";
#  không_thì
		    nếu (fnamecmp(p, "pwsh") == 0)
			p_sp = (char_u *)">%s 2>&1";
		    không_thì
			p_sp = (char_u *)"2>&1| tee";
#  endif
		    options[idx_sp].def_val[VI_DEFAULT] = p_sp;
		}
# endif
		nếu (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx_srr].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#endif

#nếu defined(MSWIN)
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
    nếu (strstr((char *)gettail(p_sh), "powershell") != NULL)
    {
	số_nguyên	idx_opt;

	idx_opt = findoption((char_u *)"shcf");
	nếu (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_shcf = (char_u*)"-Command";
	    options[idx_opt].def_val[VI_DEFAULT] = p_shcf;
	}

	idx_opt = findoption((char_u *)"sxq");
	nếu (idx_opt >= 0 && !(options[idx_opt].flags & P_WAS_SET))
	{
	    p_sxq = (char_u*)"\"";
	    options[idx_opt].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    không_thì nếu (strstr((char *)gettail(p_sh), "sh") != NULL)
    {
	số_nguyên	idx3;

	idx3 = findoption((char_u *)"shcf");
	nếu (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

	// Somehow Win32 requires the quotes around the redirection too
	idx3 = findoption((char_u *)"sxq");
	nếu (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
    }
    không_thì nếu (strstr((char *)gettail(p_sh), "cmd.exe") != NULL)
    {
	số_nguyên	idx3;

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
	nếu (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"(";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}

	idx3 = findoption((char_u *)"shcf");
	nếu (idx3 >= 0 && !(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"/c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}
    }
#endif

    nếu (BUFEMPTY())
    {
	số_nguyên idx_ffs = findoption((char_u *)"ffs");

	// Apply the first entry of 'fileformats' to the initial buffer.
	nếu (idx_ffs >= 0 && (options[idx_ffs].flags & P_WAS_SET))
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    set_title_defaults();
}

#nếu defined(FEAT_MULTI_LANG)
/*
 * When 'helplang' is still at its default value, set it to "lang".
 * Only the first two characters of "lang" are used.
 */
    void
set_helplang_default(char_u *lang)
{
    số_nguyên		idx;
    size_t	langlen;

    nếu (lang == NULL)	// safety check
	trả_về;

    langlen = STRLEN(lang);
    nếu (langlen < 2)	// safety check
	trả_về;

    idx = findoption((char_u *)"hlg");
    nếu (idx < 0 || (options[idx].flags & P_WAS_SET))
	trả_về;

    nếu (options[idx].flags & P_ALLOCED)
	free_string_option(p_hlg);
    p_hlg = vim_strnsave(lang, langlen);
    nếu (p_hlg == NULL)
	p_hlg = empty_option;
    không_thì
    {
	// zh_CN becomes "cn", zh_TW becomes "tw"
	nếu (STRNICMP(p_hlg, "zh_", 3) == 0 && langlen >= 5)
	{
	    p_hlg[0] = TOLOWER_ASC(p_hlg[3]);
	    p_hlg[1] = TOLOWER_ASC(p_hlg[4]);
	}
	// any C like setting, such as C.UTF-8, becomes "en"
	không_thì nếu (langlen >= 1 && *p_hlg == 'C')
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
    số_nguyên	    idx1;
    long    val;

    // If GUI is (going to be) used, we can always set the window title and
    // icon name.  Saves a bit of time, because the X11 display server does
    // not need to be contacted.
    idx1 = findoption((char_u *)"title");
    nếu (idx1 >= 0 && !(options[idx1].flags & P_WAS_SET))
    {
#ifdef FEAT_GUI
	nếu (gui.starting || gui.in_use)
	    val = TRUE;
	không_thì
#endif
	    val = mch_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    nếu (idx1 < 0 || (options[idx1].flags & P_WAS_SET))
    {
	trả_về;
    }

#ifdef FEAT_GUI
    nếu (gui.starting || gui.in_use)
	val = TRUE;
    không_thì
#endif
	val = mch_can_restore_icon();
    options[idx1].def_val[VI_DEFAULT] = (char_u *)(long_i)val;
    p_icon = val;
}

    void
ex_set(exarg_T *eap)
{
    số_nguyên		flags = 0;

    nếu (eap->cmdidx == CMD_setlocal)
	flags = OPT_LOCAL;
    không_thì nếu (eap->cmdidx == CMD_setglobal)
	flags = OPT_GLOBAL;
#nếu defined(FEAT_EVAL) && defined(FEAT_BROWSE)
    nếu ((cmdmod.cmod_flags & CMOD_BROWSE) && flags == 0)
	ex_options(eap);
    không_thì
#endif
    {
	nếu (eap->forceit)
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
    số_nguyên		prefix = PREFIX_NONE;
    char_u	*arg = *argp;

    nếu (STRNCMP(arg, "no", 2) == 0 && STRNCMP(arg, "novice", 6) != 0)
    {
	prefix = PREFIX_NO;
	arg += 2;
    }
    không_thì nếu (STRNCMP(arg, "inv", 3) == 0)
    {
	prefix = PREFIX_INV;
	arg += 3;
    }

    *argp = arg;
    trả_về prefix;
}

/*
 * Parse the option name in "arg" and return the option index in "*opt_idxp",
 * and the option name length in "*lenp".  For a <t_xx> option, return the key
 * number in "*keyp".
 *
 * Returns FAIL if an option starting with "<" doesn't end with a ">",
 * otherwise returns OK.
 */
    static số_nguyên
parse_option_name(char_u *arg, số_nguyên *opt_idxp, số_nguyên *lenp, số_nguyên *keyp)
{
    số_nguyên	key = 0;
    số_nguyên	len;
    số_nguyên	opt_idx;

    nếu (*arg == '<')
    {
	opt_idx = -1;
	// look out for <t_>;>
	nếu (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
	    len = 5;
	không_thì
	{
	    len = 1;
	    trong_khi (arg[len] != NUL && arg[len] != '>')
		++len;
	}
	nếu (arg[len] != '>')
	    trả_về FAIL;

	arg[len] = NUL;			    // put NUL after name
	nếu (arg[1] == 't' && arg[2] == '_') // could be term code
	    opt_idx = findoption(arg + 1);
	arg[len++] = '>';		    // restore '>'
	nếu (opt_idx == -1)
	    key = find_key_option(arg + 1, TRUE);
    }
    không_thì
    {
	số_nguyên	nextchar;   // next non-white char after option name

	len = 0;
	// The two characters after "t_" may not be alphanumeric.
	nếu (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	    len = 4;
	không_thì
	    trong_khi (ASCII_ISALNUM(arg[len]) || arg[len] == '_')
		++len;
	nextchar = arg[len];
	arg[len] = NUL;			    // put NUL after name
	opt_idx = findoption(arg);
	arg[len] = nextchar;		    // restore nextchar
	nếu (opt_idx == -1)
	    key = find_key_option(arg, FALSE);
    }

    *keyp = key;
    *lenp = len;
    *opt_idxp = opt_idx;

    trả_về OK;
}

/*
 * Get the option operator (+=, ^=, -=).
 */
    static set_op_T
get_opt_op(char_u *arg)
{
    set_op_T op = OP_NONE;

    nếu (*arg != NUL && *(arg + 1) == '=')
    {
	nếu (*arg == '+')
	    op = OP_ADDING;		// "+="
	không_thì nếu (*arg == '^')
	    op = OP_PREPENDING;		// "^="
	không_thì nếu (*arg == '-')
	    op = OP_REMOVING;		// "-="
    }

    trả_về op;
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
    cho (số_nguyên i = 0; modeline_whitelist[i] != NULL; i++)
	nếu (STRCMP(name, modeline_whitelist[i]) == 0)
	    trả_về true;
    trả_về false;
}

/*
 * Validate whether the value of the option in "opt_idx" can be changed.
 * Returns FAIL if the option can be skipped or cannot be changed. Returns OK
 * if it can be changed.
 */
    static số_nguyên
validate_opt_idx(
	số_nguyên opt_idx,
	số_nguyên opt_flags,
	long_u flags,
	char **errmsg,
	set_prefix_T prefix)
{
    // Skip all options that are not window-local (used when showing
    // an already loaded buffer in a window).
    nếu ((opt_flags & OPT_WINONLY)
	    && (opt_idx < 0 || options[opt_idx].var != VAR_WIN))
	trả_về FAIL;

    // Skip all options that are window-local (used for :vimgrep).
    nếu ((opt_flags & OPT_NOWIN) && opt_idx >= 0
	    && options[opt_idx].var == VAR_WIN)
	trả_về FAIL;

    // Disallow changing some options from modelines.
    nếu (opt_flags & OPT_MODELINE)
    {
	nếu (flags & (P_SECURE | P_NO_ML))
	{
	    *errmsg = e_not_allowed_in_modeline;
	    trả_về FAIL;
	}
	nếu ((flags & P_MLE) && !p_mle)
	{
	    *errmsg = e_not_allowed_in_modeline_when_modelineexpr_is_off;
	    trả_về FAIL;
	}
	// When 'modelinestrict' is on, only whitelisted options may be
	// set from a modeline.  Silently skip others.
	nếu (p_mlstr && opt_idx >= 0)
	{
	    // special case: allow disabling modeline
	    nếu (options[opt_idx].indir == PV_ML && prefix == PREFIX_NO)
		trả_về OK;
	    không_thì nếu (!is_modeline_whitelisted(options[opt_idx].fullname))
		trả_về FAIL;
	}
#ifdef FEAT_DIFF
	// In diff mode some options are overruled.  This avoids that
	// 'foldmethod' becomes "marker" instead of "diff" and that
	// "wrap" gets set.
	nếu (curwin->w_p_diff
		&& opt_idx >= 0  // shut up coverity warning
		&& (
# ifdef FEAT_FOLDING
		    options[opt_idx].indir == PV_FDM ||
# endif
		    options[opt_idx].indir == PV_WRAP))
	    trả_về FAIL;
#endif
    }

#ifdef HAVE_SANDBOX
    // Disallow changing some options in the sandbox
    nếu (sandbox != 0 && (flags & P_SECURE))
    {
	*errmsg = e_not_allowed_in_sandbox;
	trả_về FAIL;
    }
#endif

    trả_về OK;
}

/*
 * Get the Vim/Vi default value for a string option.
 */
    static char_u *
stropt_get_default_val(
    số_nguyên		opt_idx,
    char_u	*varp,
    số_nguyên		flags,
    số_nguyên		cp_val)
{
    char_u	*newval;

    newval = options[opt_idx].def_val[((flags & P_VI_DEF) || cp_val)
	? VI_DEFAULT : VIM_DEFAULT];
    nếu ((char_u **)varp == &p_bg)
    {
	// guess the value of 'background'
#ifdef FEAT_GUI
	nếu (gui.in_use)
	    newval = gui_bg_default();
	không_thì
#endif
	    newval = term_bg_default();
    }
    không_thì nếu ((char_u **)varp == &p_fencs && enc_utf8)
	newval = fencs_utf8_default;

    // expand environment variables and ~ since the default value was
    // already expanded, only required when an environment variable was set
    // later
    nếu (newval == NULL)
	newval = empty_option;
    không_thì
    {
	char_u *s = option_expand(opt_idx, newval);
	nếu (s == NULL)
	    s = newval;
	newval = vim_strsave(s);
    }

    trả_về newval;
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
    số_nguyên		i = getdigits((char_u **)varp);

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
    nếu (*origval_p == *oldval_p)
	*origval_p = *(char_u **)varp;
    nếu (*origval_l_p == *oldval_p)
	*origval_l_p = *(char_u **)varp;
    nếu (*origval_g_p == *oldval_p)
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
    số_nguyên i = getdigits(argp);
    nếu (i & 1)
    {
	STRCPY(whichwrap, "b,");
	len += 2;
    }
    nếu (i & 2)
    {
	STRCPY(whichwrap + len, "s,");
	len += 2;
    }
    nếu (i & 4)
    {
	STRCPY(whichwrap + len, "h,l,");
	len += 4;
    }
    nếu (i & 8)
    {
	STRCPY(whichwrap + len, "<,>,");
	len += 4;
    }
    nếu (i & 16)
    {
	STRCPY(whichwrap + len, "[,],");
	len += 4;
    }
    nếu (*whichwrap != NUL)	// remove trailing ,
	whichwrap[len - 1] = NUL;

    trả_về whichwrap;
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
    số_nguyên		flags UNUSED)
{
    char_u	*arg = *argp;
    unsigned	newlen;
    char_u	*newval;
    char_u	*s = NULL;

    // get a bit too much
    newlen = (unsigned)STRLEN(arg) + 1;
    nếu (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;
    newval = alloc(newlen);
    nếu (newval == NULL)  // out of mem, don't change
	trả_về NULL;
    s = newval;

    // Copy the string, skip over escaped chars.
    // For MS-DOS and WIN32 backslashes before normal file name characters
    // are not removed, and keep backslash at start, for "\\machine\path",
    // but do remove it for "\\\\machine\\path".
    // The reverse is found in escape_option_str_cmdline().
    trong_khi (*arg != NUL && !VIM_ISWHITE(*arg))
    {
	số_nguyên i;

	nếu (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
		&& !((flags & P_EXPAND)
		    && vim_isfilec(arg[1])
		    && !VIM_ISWHITE(arg[1])
		    && (arg[1] != '\\'
			|| (s == newval && arg[2] != '\\')))
#endif
	   )
	    ++arg;	// remove backslash
	nếu (has_mbyte && (i = (*mb_ptr2len)(arg)) > 1)
	{
	    // copy multibyte char
	    mch_memmove(s, arg, (size_t)i);
	    arg += i;
	    s += i;
	}
	không_thì
	    *s++ = *arg++;
    }
    *s = NUL;

    *argp = arg;
    trả_về newval;
}

/*
 * Expand environment variables and ~ in string option value 'newval'.
 */
    static char_u *
stropt_expand_envvar(
    số_nguyên		opt_idx,
    char_u	*origval,
    char_u	*newval,
    set_op_T	op)
{
    char_u *s = option_expand(opt_idx, newval);
    nếu (s == NULL)
	trả_về newval;

    vim_free(newval);
    unsigned newlen = (unsigned)STRLEN(s) + 1;
    nếu (op != OP_NONE)
	newlen += (unsigned)STRLEN(origval) + 1;

    newval = alloc(newlen);
    nếu (newval == NULL)
	trả_về NULL;

    STRCPY(newval, s);

    trả_về newval;
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
    số_nguyên		flags)
{
    số_nguyên		len = 0;

    số_nguyên comma = ((flags & P_COMMA) && *origval != NUL && *newval != NUL);
    nếu (op == OP_ADDING)
    {
	len = (số_nguyên)STRLEN(origval);
	// strip a trailing comma, would get 2
	nếu (comma && len > 1
		&& (flags & P_ONECOMMA) == P_ONECOMMA
		&& origval[len - 1] == ','
		&& origval[len - 2] != '\\')
	    len--;
	mch_memmove(newval + len + comma, newval, STRLEN(newval) + 1);
	mch_memmove(newval, origval, (size_t)len);
    }
    không_thì
    {
	len = (số_nguyên)STRLEN(newval);
	STRMOVE(newval + len + comma, origval);
    }
    nếu (comma)
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
    số_nguyên		flags,
    char_u	*strval,
    số_nguyên		len)
{
    // Remove newval[] from origval[]. (Note: "len" has been set above
    // and is used here).
    STRCPY(newval, origval);
    nếu (*strval)
    {
	// may need to remove a comma
	nếu (flags & P_COMMA)
	{
	    nếu (strval == origval)
	    {
		// include comma after string
		nếu (strval[len] == ',')
		    ++len;
	    }
	    không_thì
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
find_key_item(char_u *src, char_u *key, số_nguyên keylen, số_nguyên *itemlenp)
{
    char_u  *p = src;

    trong_khi (*p != NUL)
    {
	// Check if this item starts with the same key
	nếu ((p == src || *(p - 1) == ',')
		&& STRNCMP(p, key, keylen) == 0)
	{
	    // Find the end of this item
	    char_u *end = vim_strchr(p, ',');
	    nếu (end == NULL)
		end = p + STRLEN(p);
	    *itemlenp = (số_nguyên)(end - p);
	    trả_về p;
	}
	++p;
    }
    trả_về NULL;
}

/*
 * Remove one item of length "itemlen" at position "item" from comma-separated
 * string "str" in-place.  Handles the comma before or after the item.
 */
    static void
remove_comma_item(char_u *str, char_u *item, số_nguyên itemlen)
{
    nếu (item[itemlen] == ',')
	// Remove item and trailing comma
	STRMOVE(item, item + itemlen + 1);
    không_thì nếu (item > str && *(item - 1) == ',')
	// Last item: remove leading comma and item
	STRMOVE(item - 1, item + itemlen);
    không_thì
	// Only item
	*item = NUL;
}

/*
 * Remove all items matching "key" (with ':') from comma-separated string "str"
 * in-place.  If "skip" is not NULL, the item at that position is kept.
 */
    static void
remove_key_item(char_u *str, char_u *key, số_nguyên keylen, char_u *skip)
{
    số_nguyên	    itemlen;
    char_u  *found;

    trong_khi ((found = find_key_item(str, key, keylen, &itemlen)) != NULL)
    {
	nếu (found == skip)
	{
	    // Search for the next match after this one.
	    char_u *next = found + itemlen;
	    nếu (*next == ',')
		++next;
	    found = find_key_item(next, key, keylen, &itemlen);
	    nếu (found == NULL)
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
append_item(char_u *str, char_u *item, số_nguyên item_len)
{
    số_nguyên len = (số_nguyên)STRLEN(str);

    nếu (len > 0)
	str[len++] = ',';
    mch_memmove(str + len, item, (size_t)item_len);
    str[len + item_len] = NUL;
}

/*
 * Prepend a comma-separated item to the beginning of "str" in-place.
 * Adds a comma after the item if "str" is not empty.
 */
    static void
prepend_item(char_u *str, char_u *item, số_nguyên item_len)
{
    số_nguyên len = (số_nguyên)STRLEN(str);
    số_nguyên comma = (len > 0) ? 1 : 0;

    mch_memmove(str + item_len + comma, str, (size_t)len + 1);
    mch_memmove(str, item, (size_t)item_len);
    nếu (comma)
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
    số_nguyên		flags UNUSED)
{
    char_u  *p;
    char_u  *item_start;

    // Check if newval contains any "key:value" item or multiple
    // comma-separated items.  If neither, let the caller use the existing
    // code path.
    nếu (vim_strchr(newval, ':') == NULL && vim_strchr(newval, ',') == NULL)
	trả_về false;

    // Work on a copy of newval for iteration.
    char_u *newval_copy = vim_strsave(newval);
    nếu (newval_copy == NULL)
	trả_về false;

    // Build the result in newval.  Start with a copy of origval, then
    // modify it per-item.  newval buffer has room for origval + arg.
    STRCPY(newval, origval);

    // Process each item individually, modifying newval in-place.
    item_start = newval_copy;
    cho (;;)
    {
	p = vim_strchr(item_start, ',');
	số_nguyên item_len = (p == NULL)
			    ? (số_nguyên)STRLEN(item_start) : (số_nguyên)(p - item_start);

	nếu (item_len > 0)
	{
	    char_u *colon = vim_strchr(item_start, ':');
	    nếu (colon != NULL && colon < item_start + item_len)
	    {
		số_nguyên keylen = (số_nguyên)(colon - item_start) + 1;

		nếu (op == OP_ADDING || op == OP_PREPENDING)
		{
		    số_nguyên old_itemlen;
		    char_u *found = find_key_item(newval, item_start,
						       keylen, &old_itemlen);
		    nếu (found != NULL)
		    {
			nếu (old_itemlen == item_len
				&& STRNCMP(found, item_start,
							 item_len) == 0)
			{
			    // Exact duplicate: keep it in place, but
			    // remove other items with the same key.
			    remove_key_item(newval, item_start,
							   keylen, found);
			}
			không_thì
			{
			    // Key match with different value: remove all
			    // items with the same key, then add.
			    remove_key_item(newval, item_start,
							    keylen, NULL);
			    nếu (op == OP_PREPENDING)
				prepend_item(newval, item_start, item_len);
			    không_thì
				append_item(newval, item_start, item_len);
			}
		    }
		    không_thì
		    {
			// New item.
			nếu (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			không_thì
			    append_item(newval, item_start, item_len);
		    }
		}
		không_thì nếu (op == OP_REMOVING)
		    remove_key_item(newval, item_start, keylen, NULL);
	    }
	    không_thì
	    {
		nếu (op == OP_ADDING || op == OP_PREPENDING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    nếu (found == NULL)
		    {
			// New item.
			nếu (op == OP_PREPENDING)
			    prepend_item(newval, item_start, item_len);
			không_thì
			    append_item(newval, item_start, item_len);
		    }
		    // else: exact duplicate — do nothing
		}
		không_thì nếu (op == OP_REMOVING)
		{
		    char_u *found = find_dup_item(newval, item_start,
							   item_len, P_COMMA);
		    nếu (found != NULL)
			remove_comma_item(newval, found, item_len);
		}
	    }
	}

	nếu (p == NULL)
	    break;
	item_start = p + 1;
    }

    vim_free(newval_copy);

    trả_về true;
}

/*
 * Remove flags that appear twice in the string option value 'newval'.
 */
    static void
stropt_remove_dupflags(char_u *newval, số_nguyên flags)
{
    char_u	*s = newval;

    // Remove flags that appear twice.
    trong_khi (*s)
    {
	// if options have P_FLAGLIST and P_ONECOMMA such as 'whichwrap'
	nếu (flags & P_ONECOMMA)
	{
	    nếu (*s != ',' && *(s + 1) == ',' && vim_strchr(s + 2, *s) != NULL)
	    {
		// Remove the duplicated value and the next comma.
		STRMOVE(s, s + 2);
		continue;
	    }
	}
	không_thì
	{
	    nếu ((!(flags & P_COMMA) || *s != ',')
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
    số_nguyên		nextchar,
    số_nguyên		opt_idx,
    char_u	**argp,
    char_u	*varp,
    char_u	**origval_arg,
    char_u	**origval_l_arg,
    char_u	**origval_g_arg,
    char_u	**oldval_arg,
    set_op_T    *op_arg,
    số_nguyên		flags,
    số_nguyên		cp_val)
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

    nếu (nextchar == '&')	// set to default val
	newval = stropt_get_default_val(opt_idx, varp, flags, cp_val);
    không_thì nếu (nextchar == '<')	// set to global val
	newval = vim_strsave(*(char_u **)get_varp_scope(
					     &(options[opt_idx]), OPT_GLOBAL));
    không_thì
    {
	++arg;	// jump to after the '=' or ':'

	// Set 'keywordprg' to ":help" if an empty
	// value was passed to :set by the user.
	nếu (varp == (char_u *)&p_kp && (*arg == NUL || *arg == ' '))
	{
	    save_arg = arg;
	    arg = (char_u *)":help";
	}
	// Convert 'backspace' number to string
	không_thì nếu (varp == (char_u *)&p_bs && VIM_ISDIGIT(**(char_u **)varp))
	    opt_backspace_nr2str(varp, &origval, &origval_l, &origval_g,
								&oldval);
	không_thì nếu (varp == (char_u *)&p_ww && VIM_ISDIGIT(*arg))
	{
	    // Convert 'whichwrap' number to string, for backwards
	    // compatibility with Vim 3.0.
	    char_u *t = opt_whichwrap_nr2str(&arg, whichwrap);
	    save_arg = arg;
	    arg = t;
	}
	// Remove '>' before 'dir' and 'bdir', for backwards compatibility with
	// version 3.0
	không_thì nếu (*arg == '>' && (varp == (char_u *)&p_dir
		    || varp == (char_u *)&p_bdir))
	    ++arg;

	// Copy the new string into allocated memory.
	newval = stropt_copy_value(origval, &arg, op, flags);
	nếu (newval == NULL)
	    goto done;

	// Expand environment variables and ~.
	// Don't do it when adding without inserting a comma.
	nếu (op == OP_NONE || (flags & P_COMMA))
	{
	    newval = stropt_expand_envvar(opt_idx, origval, newval, op);
	    nếu (newval == NULL)
		goto done;
	}

	// For P_COMMA|P_COLON options with "key:value" items: process
	// each item individually by matching on the key part.  If
	// handled, skip the normal add/remove logic below.
	nếu ((flags & P_COMMA) && (flags & P_COLON) && op != OP_NONE
		&& stropt_handle_keymatch(origval, newval, op, flags))
	    ;  // fully handled
	không_thì
	{
	    // locate newval[] in origval[] when removing it and when
	    // adding to avoid duplicates
	    số_nguyên len = 0;
	    nếu (op == OP_REMOVING || (flags & P_NODUP))
	    {
		len = (số_nguyên)STRLEN(newval);
		s = find_dup_item(origval, newval, len, flags);

		// do not add if already there
		nếu ((op == OP_ADDING || op == OP_PREPENDING) && s != NULL)
		{
		    op = OP_NONE;
		    STRCPY(newval, origval);
		}

		// if no duplicate, move pointer to end of original value
		nếu (s == NULL)
		    s = origval + (số_nguyên)STRLEN(origval);
	    }

	    // concatenate the two strings; add a ',' if needed
	    nếu (op == OP_ADDING || op == OP_PREPENDING)
		stropt_concat_with_comma(origval, newval, op, flags);
	    không_thì nếu (op == OP_REMOVING)
		// Remove newval[] from origval[]. (Note: "len" has been
		// set above and is used here).
		stropt_remove_val(origval, newval, flags, s, len);
	}

	nếu (flags & P_FLAGLIST)
	    // Remove flags that appear twice.
	    stropt_remove_dupflags(newval, flags);
    }

done:
    nếu (save_arg != NULL)
	arg = save_arg;  // arg was temporarily changed, restore it
    *argp = arg;
    *origval_arg = origval;
    *origval_l_arg = origval_l;
    *origval_g_arg = origval_g;
    *oldval_arg = oldval;
    *op_arg = op;

    trả_về newval;
}

/*
 * Part of do_set() for string options.
 * Returns FAIL on failure, do not process further options.
 */
    static số_nguyên
do_set_option_string(
	số_nguyên	    opt_idx,
	số_nguyên	    opt_flags,
	char_u	    **argp,
	số_nguyên	    nextchar,
	set_op_T    op_arg,
	long_u	    flags,
	số_nguyên	    cp_val,
	char_u	    *varp_arg,
	char	    *errbuf,
	size_t	    errbuflen,
	số_nguyên	    *value_checked,
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
#nếu defined(FEAT_EVAL)
    char_u	*saved_origval = NULL;
    char_u	*saved_origval_l = NULL;
    char_u	*saved_origval_g = NULL;
    char_u	*saved_newval = NULL;
#endif

    // When using ":set opt=val" for a global option
    // with a local value the local value will be
    // reset, use the global value here.
    nếu ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    && ((số_nguyên)options[opt_idx].indir & PV_BOTH))
	varp = options[opt_idx].var;

    // The old value is kept until we are sure that the new value is valid.
    oldval = *(char_u **)varp;

    nếu ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	origval_l = *(char_u **)get_varp_scope(
			   &(options[opt_idx]), OPT_LOCAL);
	origval_g = *(char_u **)get_varp_scope(
			  &(options[opt_idx]), OPT_GLOBAL);

	// A global-local string option might have an empty option as value to
	// indicate that the global value should be used.
	nếu (((số_nguyên)options[opt_idx].indir & PV_BOTH)
						  && origval_l == empty_option)
	    origval_l = origval_g;
    }

    // When setting the local value of a global option, the old value may be
    // the global value.
    nếu (((số_nguyên)options[opt_idx].indir & PV_BOTH) && (opt_flags & OPT_LOCAL))
	origval = *(char_u **)get_varp(&options[opt_idx]);
    không_thì
	origval = oldval;

    // Get the new value for the option
    newval = stropt_get_newval(nextchar, opt_idx, &arg, varp, &origval,
				&origval_l, &origval_g, &oldval, &op, flags,
				cp_val);

    // Set the new value.
    *(char_u **)(varp) = newval;
    nếu (newval == NULL)
	*(char_u **)(varp) = empty_option;

#nếu defined(FEAT_EVAL)
    nếu (!starting
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
	nếu (origval_l != NULL)
	    saved_origval_l = vim_strsave(origval_l);
	nếu (origval_g != NULL)
	    saved_origval_g = vim_strsave(origval_g);
    }
#endif

    {
	long_u	*p = insecure_flag(curwin, opt_idx, opt_flags);
	số_nguyên	secure_saved = secure;

	// When an option is set in the sandbox, from a modeline or in secure
	// mode, then deal with side effects in secure mode.  Also when the
	// value was set with the P_INSECURE flag and is not completely
	// replaced.
	nếu ((opt_flags & OPT_MODELINE)
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

#nếu defined(FEAT_EVAL)
    nếu (*errmsg == NULL)
	trigger_optionset_string(opt_idx, opt_flags, saved_origval,
			       saved_origval_l, saved_origval_g, saved_newval);
    vim_free(saved_origval);
    vim_free(saved_origval_l);
    vim_free(saved_origval_g);
    vim_free(saved_newval);
#endif

    *argp = arg;
    trả_về *errmsg == NULL ? OK : FAIL;
}

/*
 * Set a boolean option.
 * Returns an untranslated error message or NULL.
 */
    static char *
do_set_option_bool(
    số_nguyên		opt_idx,
    số_nguyên		opt_flags,
    set_prefix_T prefix,
    long_u	flags,
    char_u	*varp,
    số_nguyên		nextchar,
    số_nguyên		afterchar,
    số_nguyên		cp_val)

{
    varnumber_T	value;

    nếu (nextchar == '=' || nextchar == ':')
	trả_về e_invalid_argument;
    nếu (opt_idx < 0 || varp == NULL)
	trả_về NULL;  // "cannot happen"

    // ":set opt!": invert
    // ":set opt&": reset to default value
    // ":set opt<": reset to global value
    nếu (nextchar == '!')
	value = *(số_nguyên *)(varp) ^ 1;
    không_thì nếu (nextchar == '&')
	value = (số_nguyên)(long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    không_thì nếu (nextchar == '<')
    {
	// For 'autoread' -1 means to use global value.
	nếu ((số_nguyên *)varp == &curbuf->b_p_ar && opt_flags == OPT_LOCAL)
	    value = -1;
	không_thì nếu ((số_nguyên *)varp == &curbuf->b_p_ac && opt_flags == OPT_LOCAL)
	    value = -1;
#ifdef HAVE_FSYNC
	không_thì nếu ((số_nguyên *)varp == &curbuf->b_p_fs && opt_flags == OPT_LOCAL)
	    value = -1;
#endif
	không_thì
	    value = *(số_nguyên *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    không_thì
    {
	// ":set invopt": invert
	// ":set opt" or ":set noopt": set or reset
	nếu (nextchar != NUL && !VIM_ISWHITE(afterchar))
	    trả_về e_trailing_characters;
	nếu (prefix == PREFIX_INV)
	    value = *(số_nguyên *)(varp) ^ 1;
	không_thì
	    value = prefix == PREFIX_NO ? 0 : 1;
    }

    trả_về set_bool_option(opt_idx, varp, (số_nguyên)value, opt_flags);
}

/*
 * Set a numeric option.
 * Returns an untranslated error message or NULL.
 */
    static char *
do_set_option_numeric(
    số_nguyên		opt_idx,
    số_nguyên		opt_flags,
    char_u	**argp,
    số_nguyên		nextchar,
    set_op_T	op,
    long_u	flags,
    số_nguyên		cp_val,
    char_u	*varp,
    char	*errbuf,
    size_t	errbuflen)
{
    char_u		*arg = *argp;
    varnumber_T		value;
    số_nguyên			i;
    char		*errmsg = NULL;

    nếu (opt_idx < 0 || varp == NULL)
	trả_về NULL;  // "cannot happen"
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
    nếu (nextchar == '&')
	value = (long)(long_i)options[opt_idx].def_val[
	    ((flags & P_VI_DEF) || cp_val) ? VI_DEFAULT : VIM_DEFAULT];
    không_thì nếu (nextchar == '<')
    {
	nếu ((long *)varp == &curbuf->b_p_ul && opt_flags == OPT_LOCAL)
	    // for 'undolevels' NO_LOCAL_UNDOLEVEL means using the global value
	    value = NO_LOCAL_UNDOLEVEL;
	không_thì nếu (opt_flags == OPT_LOCAL
		    && ((long *)varp == &curwin->w_p_siso
		     || (long *)varp == &curwin->w_p_so
		     || (long *)varp == &curwin->w_p_sop))
	    // for 'scrolloff'/'sidescrolloff'/'scrolloffpad' -1 means using the global value
	    value = -1;
	không_thì
	    value = *(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL);
    }
    không_thì nếu (((long *)varp == &p_wc || (long *)varp == &p_wcm)
	    && (*arg == '<'
		|| *arg == '^'
		|| (*arg != NUL
		    && (!arg[1] || VIM_ISWHITE(arg[1]))
		    && !VIM_ISDIGIT(*arg))))
    {
	value = string_to_key(arg, FALSE);
	nếu (value == 0)
	{
	    errmsg = e_invalid_argument;
	    goto skip;
	}
    }
    không_thì nếu (*arg == '-' || VIM_ISDIGIT(*arg))
    {
	// Allow negative (for 'undolevels'), octal and hex numbers.
	vim_str2nr(arg, NULL, &i, STR2NR_ALL, &value, NULL, 0, TRUE, NULL);
	nếu (i == 0 || (arg[i] != NUL && !VIM_ISWHITE(arg[i])))
	{
	    errmsg = e_number_required_after_equal;
	    goto skip;
	}
    }
    không_thì
    {
	errmsg = e_number_required_after_equal;
	goto skip;
    }

    nếu (op == OP_ADDING)
	value = *(long *)varp + value;
    không_thì nếu (op == OP_PREPENDING)
	value = *(long *)varp * value;
    không_thì nếu (op == OP_REMOVING)
	value = *(long *)varp - value;

    nếu ((long *)varp == &curwin->w_p_sop && value < -1)
    {
	errmsg = e_invalid_argument;
	goto skip;
    }

    errmsg = set_num_option(opt_idx, varp, value, errbuf, errbuflen,
								opt_flags);

skip:
    *argp = arg;
    trả_về errmsg;
}

/*
 * Set a key code (t_xx) option
 */
    static char *
do_set_option_keycode(char_u **argp, char_u *key_name, số_nguyên nextchar)
{
    char_u	*arg = *argp;
    char_u	*p;

    nếu (nextchar == '&')
    {
	nếu (add_termcap_entry(key_name, TRUE) == FAIL)
	    trả_về e_not_found_in_termcap;
    }
    không_thì
    {
	++arg; // jump to after the '=' or ':'
	cho (p = arg; *p && !VIM_ISWHITE(*p); ++p)
	    nếu (*p == '\\' && p[1] != NUL)
		++p;
	nextchar = *p;
	*p = NUL;
	add_termcode(key_name, arg, FALSE);
	*p = nextchar;
    }
    nếu (full_screen)
	ttest(FALSE);
    redraw_all_later(UPD_CLEAR);

    *argp = arg;
    trả_về NULL;
}

/*
 * Set an option to a new value.
 */
    static char *
do_set_option_value(
    số_nguyên		opt_idx,
    số_nguyên		opt_flags,
    char_u	**argp,
    set_prefix_T prefix,
    set_op_T	op,
    long_u	flags,
    char_u	*varp,
    char_u	*key_name,
    số_nguyên		nextchar,
    số_nguyên		afterchar,
    số_nguyên		cp_val,
    số_nguyên		*stopopteval,
    char	*errbuf,
    size_t	errbuflen)
{
    số_nguyên		value_checked = FALSE;
    char	*errmsg = NULL;
    char_u	*arg = *argp;

    nếu (flags & P_BOOL)
    {
	// boolean option
	errmsg = do_set_option_bool(opt_idx, opt_flags, prefix, flags, varp,
						nextchar, afterchar, cp_val);
	nếu (errmsg != NULL)
	    goto skip;
    }
    không_thì
    {
	// numeric or string option
	nếu (vim_strchr((char_u *)"=:&<", nextchar) == NULL
		|| prefix != PREFIX_NONE)
	{
	    errmsg = e_invalid_argument;
	    goto skip;
	}

	nếu (flags & P_NUM)
	{
	    // numeric option
	    errmsg = do_set_option_numeric(opt_idx, opt_flags, &arg, nextchar,
						op, flags, cp_val, varp,
						errbuf, errbuflen);
	    nếu (errmsg != NULL)
		goto skip;
	}
	không_thì nếu (opt_idx >= 0)
	{
	    // string option
	    nếu (do_set_option_string(opt_idx, opt_flags, &arg, nextchar, op,
					flags, cp_val, varp, errbuf, errbuflen,
					&value_checked, &errmsg) == FAIL)
	    {
		nếu (errmsg != NULL)
		    goto skip;
		*stopopteval = TRUE;
		goto skip;
	    }
	}
	không_thì
	{
	    // key code option
	    errmsg = do_set_option_keycode(&arg, key_name, nextchar);
	    nếu (errmsg != NULL)
		goto skip;
	}
    }

    nếu (opt_idx >= 0)
	did_set_option(opt_idx, opt_flags, op == OP_NONE, value_checked);

skip:
    *argp = arg;
    trả_về errmsg;
}

/*
 * Set an option to a new value.
 * Return NULL if OK, return an untranslated error message when something is
 * wrong.  "errbuf[errbuflen]" can be used to create the error message.
 */
    static char *
do_set_option(
    số_nguyên		opt_flags,
    char_u	**argp,
    char_u	*arg_start,
    char_u	**startarg,
    số_nguyên		*did_show,
    số_nguyên		*stopopteval,
    char	*errbuf,
    size_t	errbuflen)
{
    số_nguyên		opt_idx;
    char_u	*arg;
    set_prefix_T prefix;	// no prefix, "no" prefix or "inv" prefix
    set_op_T	op;
    long_u	flags;		// flags for current option
    char_u	*varp;		// pointer to variable for current option
    char_u	key_name[2];
    số_nguyên		nextchar;	// next non-white char after option name
    số_nguyên		afterchar;	// character just after option name
    char	*errmsg = NULL;
    số_nguyên		key;
    số_nguyên		len;

    prefix = get_option_prefix(argp);
    arg = *argp;

    // find end of name
    key = 0;
    nếu (parse_option_name(arg, &opt_idx, &len, &key) == FAIL)
	trả_về e_invalid_argument;

    // remember character after option name
    afterchar = arg[len];

    nếu (in_vim9script())
    {
	char_u *p = skipwhite(arg + len);

	// disallow white space before =val, +=val, -=val, ^=val
	nếu (p > arg + len && (p[0] == '='
		    || (vim_strchr((char_u *)"+-^", p[0]) != NULL
			&& p[1] == '=')))
	{
	    errmsg = e_no_white_space_allowed_between_option_and;
	    arg = p;
	    *startarg = p;
	    goto skip;
	}
    }
    không_thì
	// skip white space, allow ":set ai  ?", ":set hlsearch  !"
	trong_khi (VIM_ISWHITE(arg[len]))
	    ++len;

    op = get_opt_op(arg + len);
    nếu (op != OP_NONE)
	len++;

    nextchar = arg[len];

    nếu (opt_idx == -1 && key == 0)	// found a mismatch: skip
    {
	nếu (in_vim9script() && arg > arg_start
		&& vim_strchr((char_u *)"!&<", *arg) != NULL)
	    errmsg = e_no_white_space_allowed_between_option_and;
	không_thì
	    errmsg = e_unknown_option;
	goto skip;
    }

    nếu (opt_idx >= 0)
    {
	nếu (options[opt_idx].var == NULL)   // hidden option: skip
	{
	    // Only give an error message when requesting the value of
	    // a hidden option, ignore setting it.
	    nếu (vim_strchr((char_u *)"=:!&<", nextchar) == NULL
		    && (!(options[opt_idx].flags & P_BOOL)
			|| nextchar == '?'))
		errmsg = e_option_not_supported;
	    goto skip;
	}

	flags = options[opt_idx].flags;
	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
    }
    không_thì
    {
	flags = P_STRING;
	varp = NULL;
	nếu (key < 0)
	{
	    key_name[0] = KEY2TERMCAP0(key);
	    key_name[1] = KEY2TERMCAP1(key);
	}
	không_thì
	{
	    key_name[0] = KS_KEY;
	    key_name[1] = (key & 0xff);
	}
    }

    // Make sure the option value can be changed.
    nếu (validate_opt_idx(opt_idx, opt_flags, flags, &errmsg, prefix) == FAIL)
	goto skip;

    số_nguyên cp_val = p_cp;
    nếu (vim_strchr((char_u *)"?=:!&<", nextchar) != NULL)
    {
	arg += len;
	nếu (nextchar == '&' && arg[1] == 'v' && arg[2] == 'i')
	{
	    nếu (arg[3] == 'm')	// "opt&vim": set to Vim default
	    {
		cp_val = FALSE;
		arg += 3;
	    }
	    không_thì		// "opt&vi": set to Vi default
	    {
		cp_val = TRUE;
		arg += 2;
	    }
	}
	nếu (vim_strchr((char_u *)"?!&<", nextchar) != NULL
		&& arg[1] != NUL && !VIM_ISWHITE(arg[1]))
	{
	    errmsg = e_trailing_characters;
	    goto skip;
	}
    }

    // Allow '=' and ':' for historical reasons (MSDOS command.com).
    // Allows only one '=' character per "set" command line. grrr. (jw)
    nếu (nextchar == '?'
	    || (prefix == PREFIX_NONE
		&& vim_strchr((char_u *)"=:&<", nextchar) == NULL
		&& !(flags & P_BOOL)))
    {
	// print value
	nếu (*did_show)
	    msg_putchar('\n');	    // cursor below last one
	không_thì
	{
	    gotocmdline(TRUE);	    // cursor at status line
	    *did_show = TRUE;	    // remember that we did a line
	}
	nếu (opt_idx >= 0)
	{
	    showoneopt(&options[opt_idx], opt_flags);
#ifdef FEAT_EVAL
	    nếu (p_verbose > 0)
	    {
		// Mention where the option was last set.
		nếu (varp == options[opt_idx].var)
		    last_set_msg(options[opt_idx].script_ctx);
		không_thì nếu ((số_nguyên)options[opt_idx].indir & PV_WIN)
		    last_set_msg(curwin->w_p_script_ctx[
			    (số_nguyên)options[opt_idx].indir & PV_MASK]);
		không_thì nếu ((số_nguyên)options[opt_idx].indir & PV_BUF)
		    last_set_msg(curbuf->b_p_script_ctx[
			    (số_nguyên)options[opt_idx].indir & PV_MASK]);
	    }
#endif
	}
	không_thì
	{
	    char_u	    *p;

	    p = find_termcode(key_name);
	    nếu (p == NULL)
	    {
		errmsg = e_key_code_not_set;
		goto skip;
	    }
	    không_thì
		(void)show_one_termcode(key_name, p, TRUE);
	}
	nếu (nextchar != '?'
		&& nextchar != NUL && !VIM_ISWHITE(afterchar))
	    errmsg = e_trailing_characters;
    }
    không_thì
    {
	errmsg = do_set_option_value(opt_idx, opt_flags, &arg, prefix, op,
					flags, varp, key_name, nextchar,
					afterchar, cp_val, stopopteval, errbuf,
					errbuflen);
    }

skip:
    *argp = arg;
    trả_về errmsg;
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
    số_nguyên
do_set(
    char_u	*arg_start,	// option string (may be written to!)
    số_nguyên		opt_flags)
{
    char_u	*arg = arg_start;
    số_nguyên		i;
    số_nguyên		did_show = FALSE;   // already showed one value

    nếu (*arg == NUL)
    {
	showoptions(0, opt_flags);
	did_show = TRUE;
	goto theend;
    }

    trong_khi (*arg != NUL)		// loop to process all options
    {
	nếu (STRNCMP(arg, "all", 3) == 0 && !ASCII_ISALPHA(arg[3])
						&& !(opt_flags & OPT_MODELINE))
	{
	    // ":set all"  show all options.
	    // ":set all&" set all options to their default value.
	    arg += 3;
	    nếu (*arg == '&')
	    {
		++arg;
		// Only for :set command set global value of local options.
		set_options_default(OPT_FREE | opt_flags);
		didset_options();
		didset_options2();
		redraw_all_later(UPD_CLEAR);
	    }
	    không_thì
	    {
		showoptions(1, opt_flags);
		did_show = TRUE;
	    }
	}
	không_thì nếu (STRNCMP(arg, "termcap", 7) == 0 && !(opt_flags & OPT_MODELINE))
	{
	    showoptions(2, opt_flags);
	    show_termcodes(opt_flags);
	    did_show = TRUE;
	    arg += 7;
	}
	không_thì
	{
	    số_nguyên		stopopteval = FALSE;
	    char	*errmsg = NULL;
	    char	errbuf[ERR_BUFLEN];
	    char_u	*startarg = arg;

	    errmsg = do_set_option(opt_flags, &arg, arg_start, &startarg,
					&did_show, &stopopteval, errbuf,
					ERR_BUFLEN);
	    nếu (stopopteval)
		break;

	    // Advance to next argument.
	    // - skip until a blank found, taking care of backslashes
	    // - skip blanks
	    // - skip one "=val" argument (for hidden options ":set gfn =xx")
	    cho (i = 0; i < 2 ; ++i)
	    {
		trong_khi (*arg != NUL && !VIM_ISWHITE(*arg))
		    nếu (*arg++ == '\\' && *arg != NUL)
			++arg;
		arg = skipwhite(arg);
		nếu (*arg != '=')
		    break;
	    }

	    nếu (errmsg != NULL)
	    {
		i = vim_snprintf((char *)IObuff, IOSIZE, "%s", (char_u *)_(errmsg)) + 2;
		nếu (i + (arg - startarg) < IOSIZE)
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

		trả_về FAIL;
	    }
	}

	arg = skipwhite(arg);
    }

theend:
    nếu (silent_mode && did_show)
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

    trả_về OK;
}

/*
 * Call this when an option has been given a new value through a user command.
 * Sets the P_WAS_SET flag and takes care of the P_INSECURE flag.
 */
    void
did_set_option(
    số_nguyên	    opt_idx,
    số_nguyên	    opt_flags,	    // possibly with OPT_MODELINE
    số_nguyên	    new_value,	    // value was replaced completely
    số_nguyên	    value_checked)  // value was checked to be safe, no need to set the
			    // P_INSECURE flag.
{
    long_u	*flagsp;
    long_u	*flagsp_local = NULL;
    số_nguyên		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;

    options[opt_idx].flags |= P_WAS_SET;

    // When an option is set in the sandbox, from a modeline or in secure mode
    // set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
    // flag.
    flagsp = insecure_flag(curwin, opt_idx, opt_flags);
    nếu (both)
	flagsp_local = insecure_flag(curwin, opt_idx, OPT_LOCAL);
    nếu (!value_checked && (secure
#ifdef HAVE_SANDBOX
	    || sandbox != 0
#endif
	    || (opt_flags & OPT_MODELINE)))
    {
	*flagsp = *flagsp | P_INSECURE;
	nếu (flagsp_local != NULL)
	    *flagsp_local = *flagsp_local | P_INSECURE;
    }
    không_thì nếu (new_value)
    {
	*flagsp = *flagsp & ~P_INSECURE;
	nếu (flagsp_local != NULL)
	    *flagsp_local = *flagsp_local & ~P_INSECURE;
    }
}

/*
 * Convert a key name or string into a key value.
 * Used for 'cedit', 'termwinkey', 'wildchar' and 'wildcharm' options.
 * When "multi_byte" is TRUE allow for multi-byte characters.
 */
    số_nguyên
string_to_key(char_u *arg, số_nguyên multi_byte)
{
    nếu (*arg == '<' && arg[1])
	trả_về find_key_option(arg + 1, TRUE);
    nếu (*arg == '^' && arg[1])
    {
	số_nguyên key = Ctrl_chr(arg[1]);
	nếu (key == 0)		// ^@ is <Nul>
	    key = K_ZERO;
	trả_về key;
    }
    nếu (multi_byte)
	trả_về PTR2CHAR(arg);
    trả_về *arg;
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
    nếu (starting != NO_SCREEN
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
    số_nguyên		oldval,
    số_nguyên		newval,
    số_nguyên		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    // The option values that are changed when 'bin' changes are
    // copied when 'bin is set and restored when 'bin' is reset.
    nếu (newval)
    {
	nếu (!oldval)		// switched on
	{
	    nếu (!(opt_flags & OPT_GLOBAL))
	    {
		curbuf->b_p_tw_nobin = curbuf->b_p_tw;
		curbuf->b_p_wm_nobin = curbuf->b_p_wm;
		curbuf->b_p_ml_nobin = curbuf->b_p_ml;
		curbuf->b_p_et_nobin = curbuf->b_p_et;
	    }
	    nếu (!(opt_flags & OPT_LOCAL))
	    {
		p_tw_nobin = p_tw;
		p_wm_nobin = p_wm;
		p_ml_nobin = p_ml;
		p_et_nobin = p_et;
	    }
	}

	nếu (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = 0;	// no automatic line wrap
	    curbuf->b_p_wm = 0;	// no automatic line wrap
	    curbuf->b_p_ml = 0;	// no modelines
	    curbuf->b_p_et = 0;	// no expandtab
	}
	nếu (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = 0;
	    p_wm = 0;
	    p_ml = FALSE;
	    p_et = FALSE;
	    p_bin = TRUE;	// needed when called for the "-b" argument
	}
    }
    không_thì nếu (oldval)		// switched off
    {
	nếu (!(opt_flags & OPT_GLOBAL))
	{
	    curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	    curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	    curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	    curbuf->b_p_et = curbuf->b_p_et_nobin;
	}
	nếu (!(opt_flags & OPT_LOCAL))
	{
	    p_tw = p_tw_nobin;
	    p_wm = p_wm_nobin;
	    p_ml = p_ml_nobin;
	    p_et = p_et_nobin;
	}
    }
#nếu defined(FEAT_EVAL)
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
option_expand(số_nguyên opt_idx, char_u *val)
{
    // if option doesn't need expansion nothing to do
    nếu (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == NULL)
	trả_về NULL;

    // If val is longer than MAXPATHL no meaningful expansion can be done,
    // expand_env() would truncate the string.
    nếu (val != NULL && STRLEN(val) > MAXPATHL)
	trả_về NULL;

    nếu (val == NULL)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags' or 'path', they are used to separate
     * file names.
     * For 'spellsuggest' expand after "file:".
     */
    char_u ** var = (char_u **)options[opt_idx].var;
    số_nguyên esc = var == &p_tags || var == &p_path;

    expand_env_esc(val, NameBuff, MAXPATHL,
	    esc ? (char_u *)" \t" : NULL, FALSE,
#ifdef FEAT_SPELL
	    var == &p_sps ? (char_u *)"file:" :
#endif
				  NULL);
    nếu (STRCMP(NameBuff, val) == 0)   // they are the same
	trả_về NULL;

    trả_về NameBuff;
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
    số_nguyên		opt_idx;

    cho (opt_idx = 0; options[opt_idx].fullname != NULL; opt_idx++)
	nếu ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != NULL)
	    check_string_option((char_u **)get_varp(&(options[opt_idx])));
}

/*
 * Return the option index found by a pointer into term_strings[].
 * Return -1 if not found.
 */
    số_nguyên
get_term_opt_idx(char_u **p)
{
    số_nguyên opt_idx;

    cho (opt_idx = 1; options[opt_idx].fullname != NULL; opt_idx++)
	nếu (options[opt_idx].var == (char_u *)p)
	    trả_về opt_idx;
    trả_về -1; // cannot happen: didn't find it!
}

/*
 * Mark a terminal option as allocated, found by a pointer into term_strings[].
 * Return the option index or -1 if not found.
 */
    số_nguyên
set_term_option_alloced(char_u **p)
{
    số_nguyên		opt_idx = get_term_opt_idx(p);

    nếu (opt_idx >= 0)
	options[opt_idx].flags |= P_ALLOCED;
    trả_về opt_idx;
}

#nếu defined(FEAT_EVAL)
/*
 * Return TRUE when option "opt" was set from a modeline or in secure mode.
 * Return FALSE when it wasn't.
 * Return -1 for an unknown option.
 */
    số_nguyên
was_set_insecurely(win_T *wp, char_u *opt, số_nguyên opt_flags)
{
    số_nguyên	    idx = findoption(opt);
    long_u  *flagp;

    nếu (idx >= 0)
    {
	flagp = insecure_flag(wp, idx, opt_flags);
	trả_về (*flagp & P_INSECURE) != 0;
    }
    internal_error("was_set_insecurely()");
    trả_về -1;
}

/*
 * Get a pointer to the flags used for the P_INSECURE flag of option
 * "opt_idx".  For some local options a local flags field is used.
 * NOTE: Caller must make sure that "curwin" is set to the window from which
 * the option is used.
 */
    static long_u *
insecure_flag(win_T *wp, số_nguyên opt_idx, số_nguyên opt_flags)
{
    nếu (opt_flags & OPT_LOCAL)
	switch ((số_nguyên)options[opt_idx].indir)
	{
	    case PV_WRAP:	trả_về &wp->w_p_wrap_flags;
# ifdef FEAT_STL_OPT
	    case PV_STL:	trả_về &wp->w_p_stl_flags;
# endif
# ifdef FEAT_EVAL
#  ifdef FEAT_FOLDING
	    case PV_FDE:	trả_về &wp->w_p_fde_flags;
	    case PV_FDT:	trả_về &wp->w_p_fdt_flags;
#  endif
#  ifdef FEAT_BEVAL
	    case PV_BEXPR:	trả_về &wp->w_buffer->b_p_bexpr_flags;
#  endif
	    case PV_INDE:	trả_về &wp->w_buffer->b_p_inde_flags;
	    case PV_FEX:	trả_về &wp->w_buffer->b_p_fex_flags;
#  ifdef FEAT_FIND_ID
	    case PV_INEX:	trả_về &wp->w_buffer->b_p_inex_flags;
#  endif
#  ifdef FEAT_COMPL_FUNC
	    case PV_CPT:	trả_về &wp->w_buffer->b_p_cpt_flags;
#  endif
# endif
	}
    không_thì
	// For global value of window-local options, use flags in w_allbuf_opt.
	switch ((số_nguyên)options[opt_idx].indir)
	{
	    case PV_WRAP:	trả_về &wp->w_allbuf_opt.wo_wrap_flags;
# nếu defined(FEAT_EVAL) && defined(FEAT_FOLDING)
	    case PV_FDE:	trả_về &wp->w_allbuf_opt.wo_fde_flags;
	    case PV_FDT:	trả_về &wp->w_allbuf_opt.wo_fdt_flags;
# endif
	}

    // Nothing special, return global flags field.
    trả_về &options[opt_idx].flags;
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
#nếu defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
}

/*
 * Return TRUE if "val" is a valid name: only consists of alphanumeric ASCII
 * characters or characters in "allowed".
 */
    số_nguyên
valid_name(char_u *val, char *allowed)
{
    char_u *s;

    cho (s = val; *s != NUL; ++s)
	nếu (!ASCII_ISALNUM(*s) && vim_strchr((char_u *)allowed, *s) == NULL)
	    trả_về FALSE;
    trả_về TRUE;
}

#nếu defined(FEAT_EVAL)
/*
 * Set the script_ctx for an option, taking care of setting the buffer- or
 * window-local value.
 */
    void
set_option_sctx_idx(số_nguyên opt_idx, số_nguyên opt_flags, sctx_T script_ctx)
{
    số_nguyên		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    số_nguyên		indir = (số_nguyên)options[opt_idx].indir;
    sctx_T	new_script_ctx = script_ctx;

    // Modeline already has the line number set.
    nếu (!(opt_flags & OPT_MODELINE))
	new_script_ctx.sc_lnum += SOURCING_LNUM;

    // ":legacy" and ":vim9cmd" change the execution context of an option.
    nếu (cmdmod.cmod_flags & CMOD_VIM9CMD)
	new_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    nếu (cmdmod.cmod_flags & CMOD_LEGACY)
	// It is a bit confusing, but "MAX" is actually the legacy Vim script
	// version before Vim9.
	new_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // Remember where the option was set.  For local options need to do that
    // in the buffer or window structure.
    nếu (both || (opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	options[opt_idx].script_ctx = new_script_ctx;
    nếu (both || (opt_flags & OPT_LOCAL))
    {
	nếu (indir & PV_BUF)
	    curbuf->b_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	không_thì nếu (indir & PV_WIN)
	{
	    curwin->w_p_script_ctx[indir & PV_MASK] = new_script_ctx;
	    nếu (both)
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
is_option_value_vim9(số_nguyên opt_idx, số_nguyên opt_flags)
{
    số_nguyên		indir = (số_nguyên)options[opt_idx].indir;
    sctx_T	*sctx = NULL;

    nếu ((opt_flags & OPT_GLOBAL) || (indir & (PV_BUF|PV_WIN)) == 0)
	sctx = &options[opt_idx].script_ctx;

    nếu ((opt_flags & OPT_LOCAL) || (indir & (PV_BUF|PV_WIN)))
    {
	nếu (indir & PV_BUF)
	    sctx = &curbuf->b_p_script_ctx[indir & PV_MASK];
	không_thì nếu (indir & PV_WIN)
	    sctx = &curwin->w_p_script_ctx[indir & PV_MASK];
    }

    // If "sc_sid" is not set, it means the option value was not modified from
    // the default yet.  As we move towards Vim9 all the default values should
    // be valid Vim9.
    trả_về !SCRIPT_ID_VALID(sctx->sc_sid) ||
	sctx->sc_version >= SCRIPT_VERSION_VIM9;
}

/*
 * Get the script context of global option "name".
 *
 */
    sctx_T *
get_option_sctx(char *name)
{
    số_nguyên idx = findoption((char_u *)name);

    nếu (idx >= 0)
	trả_về &options[idx].script_ctx;
    siemsg("no such option: %s", name);
    trả_về NULL;
}

/*
 * Set the script_ctx for a termcap option.
 * "name" must be the two character code, e.g. "RV".
 * When "name" is NULL use "opt_idx".
 */
    void
set_term_option_sctx_idx(char *name, số_nguyên opt_idx)
{
    char_u  buf[5];
    số_nguyên	    idx;

    nếu (name == NULL)
	idx = opt_idx;
    không_thì
    {
	buf[0] = 't';
	buf[1] = '_';
	buf[2] = name[0];
	buf[3] = name[1];
	buf[4] = 0;
	idx = findoption(buf);
    }
    nếu (idx >= 0)
	set_option_sctx_idx(idx, OPT_GLOBAL, current_sctx);
}
#endif

#nếu defined(FEAT_EVAL)
/*
 * Apply the OptionSet autocommand.
 */
    static void
apply_optionset_autocmd(
	số_nguyên	opt_idx,
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
    nếu (starting || errmsg != NULL || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	trả_về;

    len = vim_snprintf_safelen((char *)buf_new, sizeof(buf_new), "%ld", newval);
    set_vim_var_string(VV_OPTION_NEW, buf_new, (số_nguyên)len);
    len = vim_snprintf_safelen((char *)buf_type, sizeof(buf_type), "%s",
				(opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (số_nguyên)len);

    buf_oldlen = vim_snprintf_safelen((char *)buf_old, sizeof(buf_old), "%ld", oldval);
    set_vim_var_string(VV_OPTION_OLD, buf_old, (số_nguyên)buf_oldlen);

    nếu (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (số_nguyên)buf_oldlen);
    }
    nếu (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old, (số_nguyên)buf_oldlen);
    }
    nếu ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	char_u	buf_old_global[12];
	size_t	buf_old_globallen;

	buf_old_globallen = vim_snprintf_safelen((char *)buf_old_global,
	    sizeof(buf_old_global), "%ld", oldval_g);

	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (số_nguyên)buf_oldlen);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, buf_old_global, (số_nguyên)buf_old_globallen);
    }
    nếu (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, buf_old, (số_nguyên)buf_oldlen);
    }
    apply_autocmds(EVENT_OPTIONSET, (char_u *)options[opt_idx].fullname,
	    NULL, FALSE, NULL);
    reset_v_option_vars();
}
#endif

#nếu defined(FEAT_ARABIC)
/*
 * Process the updated 'arabic' option value.
 */
    char *
did_set_arabic(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    nếu (curwin->w_p_arab)
    {
	// 'arabic' is set, handle various sub-settings.
	nếu (!p_tbidi)
	{
	    // set rightleft mode
	    nếu (!curwin->w_p_rl)
	    {
		curwin->w_p_rl = TRUE;
		changed_window_setting();
	    }

	    // Enable Arabic shaping (major part of what Arabic requires)
	    nếu (!p_arshape)
	    {
		p_arshape = TRUE;
		redraw_later_clear();
	    }
	}

	// Arabic requires a utf-8 encoding, inform the user if it's not
	// set.
	nếu (STRCMP(p_enc, "utf-8") != 0)
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
    không_thì
    {
	// 'arabic' is reset, handle various sub-settings.
	nếu (!p_tbidi)
	{
	    // reset rightleft mode
	    nếu (curwin->w_p_rl)
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

    trả_về errmsg;
}
#endif

#nếu defined(FEAT_AUTOCHDIR)
/*
 * Process the updated 'autochdir' option value.
 */
    char *
did_set_autochdir(optset_T *args UNUSED)
{
    // Change directories when the 'acd' option is set now.
    DO_AUTOCHDIR;
    trả_về NULL;
}
#endif

#nếu defined(FEAT_BEVAL_GUI)
/*
 * Process the updated 'ballooneval' option value.
 */
    char *
did_set_ballooneval(optset_T *args)
{
    nếu (balloonEvalForTerm)
	trả_về NULL;

    nếu (p_beval && !args->os_oldval.boolean)
	gui_mch_enable_beval_area(balloonEval);
    không_thì nếu (!p_beval && args->os_oldval.boolean)
	gui_mch_disable_beval_area(balloonEval);

    trả_về NULL;
}
#endif

#nếu defined(FEAT_BEVAL_TERM)
/*
 * Process the updated 'balloonevalterm' option value.
 */
    char *
did_set_balloonevalterm(optset_T *args UNUSED)
{
    mch_bevalterm_changed();
    trả_về NULL;
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

    trả_về NULL;
}

/*
 * Process the updated 'buflisted' option value.
 */
    char *
did_set_buflisted(optset_T *args)
{
    // when 'buflisted' changes, trigger autocommands
    nếu (args->os_oldval.boolean != curbuf->b_p_bl)
	apply_autocmds(curbuf->b_p_bl ? EVENT_BUFADD : EVENT_BUFDELETE,
						NULL, NULL, TRUE, curbuf);
    trả_về NULL;
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
    nếu (p_ch < MIN_CMDHEIGHT)
    {
	errmsg = e_argument_must_be_positive;
	p_ch = MIN_CMDHEIGHT;
    }
    nếu (p_ch > Rows - min_rows() + MIN_CMDHEIGHT)
	p_ch = Rows - min_rows() + MIN_CMDHEIGHT;

    // Only compute the new window layout when startup has been
    // completed. Otherwise the frame sizes may be wrong.
    nếu ((p_ch != old_value
		|| tabline_height() + topframe->fr_height != Rows - p_ch)
	    && full_screen
#ifdef FEAT_GUI
	    && !gui.starting
#endif
       )
	command_height();

    trả_về errmsg;
}

/*
 * Process the updated 'compatible' option value.
 */
    char *
did_set_compatible(optset_T *args UNUSED)
{
    compatible_set();
    trả_về NULL;
}

#nếu defined(FEAT_CONCEAL)
/*
 * Process the new 'conceallevel' option value.
 */
    char *
did_set_conceallevel(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    nếu (curwin->w_p_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_cole = 0;
    }
    không_thì nếu (curwin->w_p_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_cole = 3;
    }
    nếu (curwin->w_allbuf_opt.wo_cole < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_cole = 0;
    }
    không_thì nếu (curwin->w_allbuf_opt.wo_cole > 3)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_cole = 3;
    }

    trả_về errmsg;
}
#endif

#nếu defined(FEAT_DIFF)
/*
 * Process the updated 'diff' option value.
 */
    char *
did_set_diff(optset_T *args UNUSED)
{
    // May add or remove the buffer from the list of diff buffers.
    diff_buf_adjust(curwin);
# ifdef FEAT_FOLDING
    nếu (foldmethodIsDiff(curwin))
	foldUpdateAll(curwin);
# endif
    trả_về NULL;
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
    trả_về NULL;
}

/*
 * Process the updated 'equalalways' option value.
 */
    char *
did_set_equalalways(optset_T *args)
{
    nếu (p_ea && !args->os_oldval.boolean)
	win_equal(curwin, FALSE, 0);

    trả_về NULL;
}

#nếu defined(FEAT_FOLDING)
/*
 * Process the new 'foldcolumn' option value.
 */
    char *
did_set_foldcolumn(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    nếu (curwin->w_p_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_fdc = 0;
    }
    không_thì nếu (curwin->w_p_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_fdc = 12;
    }
    nếu (curwin->w_allbuf_opt.wo_fdc < 0)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_fdc = 0;
    }
    không_thì nếu (curwin->w_allbuf_opt.wo_fdc > 12)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_fdc = 12;
    }

    trả_về errmsg;
}

/*
 * Process the new 'foldlevel' option value.
 */
    char *
did_set_foldlevel(optset_T *args UNUSED)
{
    nếu (curwin->w_p_fdl < 0)
	curwin->w_p_fdl = 0;
    newFoldLevel();
    trả_về NULL;
}

/*
 * Process the new 'foldminlines' option value.
 */
    char *
did_set_foldminlines(optset_T *args UNUSED)
{
    foldUpdateAll(curwin);
    trả_về NULL;
}

/*
 * Process the new 'foldnestmax' option value.
 */
    char *
did_set_foldnestmax(optset_T *args UNUSED)
{
    nếu (foldmethodIsSyntax(curwin) || foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    trả_về NULL;
}
#endif

#nếu defined(FEAT_SEARCH_EXTRA)
/*
 * Process the updated 'hlsearch' option value.
 */
    char *
did_set_hlsearch(optset_T *args UNUSED)
{
    // when 'hlsearch' is set or reset: reset no_hlsearch
    set_no_hlsearch(FALSE);
    trả_về NULL;
}
#endif

/*
 * Process the updated 'ignorecase' option value.
 */
    char *
did_set_ignorecase(optset_T *args UNUSED)
{
    // when 'ignorecase' is set or reset and 'hlsearch' is set, redraw
    nếu (p_hls)
	redraw_all_later(UPD_SOME_VALID);
    trả_về NULL;
}

#nếu defined(HAVE_INPUT_METHOD)
/*
 * Process the updated 'imdisable' option value.
 */
    char *
did_set_imdisable(optset_T *args UNUSED)
{
    // Only de-activate it here, it will be enabled when changing mode.
    nếu (p_imdisable)
	im_set_active(FALSE);
    không_thì nếu (State & MODE_INSERT)
	// When the option is set from an autocommand, it may need to take
	// effect right away.
	im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
    trả_về NULL;
}
#endif

/*
 * Process the new 'iminsert' option value.
 */
    char *
did_set_iminsert(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    nếu (curbuf->b_p_iminsert < 0 || curbuf->b_p_iminsert > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_iminsert = B_IMODE_NONE;
    }
    p_iminsert = curbuf->b_p_iminsert;
    nếu (termcap_active)	// don't do this in the alternate screen
	showmode();
#nếu defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif

    trả_về errmsg;
}

/*
 * Process the new 'imsearch' option value.
 */
    char *
did_set_imsearch(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    nếu (curbuf->b_p_imsearch < -1 || curbuf->b_p_imsearch > B_IMODE_LAST)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_imsearch = B_IMODE_NONE;
    }
    p_imsearch = curbuf->b_p_imsearch;

    trả_về errmsg;
}

#nếu defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * Process the new 'imstyle' option value.
 */
    char *
did_set_imstyle(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    nếu (p_imst != IM_ON_THE_SPOT && p_imst != IM_OVER_THE_SPOT)
	errmsg = e_invalid_argument;

    trả_về errmsg;
}
#endif

/*
 * Process the updated 'insertmode' option value.
 */
    char *
did_set_insertmode(optset_T *args)
{
    // when 'insertmode' is set from an autocommand need to do work here
    nếu (p_im)
    {
	nếu ((State & MODE_INSERT) == 0)
	    need_start_insertmode = TRUE;
	stop_insert_mode = FALSE;
    }
    // only reset if it was set previously
    không_thì nếu (args->os_oldval.boolean)
    {
	need_start_insertmode = FALSE;
	stop_insert_mode = TRUE;
	nếu (restart_edit != 0 && mode_displayed)
	    clear_cmdline = TRUE;	// remove "(insert)"
	restart_edit = 0;
    }

    trả_về NULL;
}

#nếu defined(FEAT_LANGMAP)
/*
 * Process the updated 'langnoremap' option value.
 */
    char *
did_set_langnoremap(optset_T *args UNUSED)
{
    // 'langnoremap' -> !'langremap'
    p_lrm = !p_lnr;
    trả_về NULL;
}

/*
 * Process the updated 'langremap' option value.
 */
    char *
did_set_langremap(optset_T *args UNUSED)
{
    // 'langremap' -> !'langnoremap'
    p_lnr = !p_lrm;
    trả_về NULL;
}
#endif

/*
 * Process the new 'laststatus' option value.
 */
    char *
did_set_laststatus(optset_T *args UNUSED)
{
    last_status(FALSE);	// (re)set last window status line
    trả_về NULL;
}

#nếu defined(FEAT_GUI)
/*
 * Process the new 'linespace' option value.
 */
    char *
did_set_linespace(optset_T *args UNUSED)
{
    // Recompute gui.char_height and resize the Vim window to keep the
    // same number of lines.
    nếu (gui.in_use && gui_mch_adjust_charheight() == OK)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    trả_về NULL;
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
    trả_về NULL;
}

/*
 * Process the new 'maxcombine' option value.
 */
    char *
did_set_maxcombine(optset_T *args UNUSED)
{
    nếu (p_mco > MAX_MCO)
	p_mco = MAX_MCO;
    không_thì nếu (p_mco < 0)
	p_mco = 0;
    screenclear();	    // will re-allocate the screen
    trả_về NULL;
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
    nếu (curbuf->b_p_ma && (term_in_normal_mode(curbuf) || (bt_terminal(curbuf)
		    && curbuf->b_term != NULL && !term_is_finished(curbuf))))
    {
	curbuf->b_p_ma = FALSE;
	trả_về e_cannot_make_terminal_with_running_job_modifiable;
    }
#endif
    redraw_titles();

    trả_về NULL;
}

/*
 * Process the updated 'modified' option value.
 */
    char *
did_set_modified(optset_T *args)
{
    nếu (!args->os_newval.boolean)
	save_file_ff(curbuf);	// Buffer is unchanged
    redraw_titles();
    curbuf->b_modified_was_set = !!args->os_newval.boolean;
    trả_về NULL;
}

#nếu defined(FEAT_GUI)
/*
 * Process the updated 'mousehide' option value.
 */
    char *
did_set_mousehide(optset_T *args UNUSED)
{
    nếu (!p_mh)
	gui_mch_mousehide(FALSE);
    trả_về NULL;
}
#endif

/*
 * Process the updated 'number' or 'relativenumber' option value.
 */
    char *
did_set_number_relativenumber(optset_T *args UNUSED)
{
#nếu defined(FEAT_SIGNS) && defined(FEAT_GUI)
    nếu (gui.in_use
	    && (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u')
	    && curbuf->b_signlist != NULL)
    {
	// If the 'number' or 'relativenumber' options are modified and
	// 'signcolumn' is set to 'number', then clear the screen for a full
	// refresh. Otherwise the sign icons are not displayed properly in the
	// number column.  If the 'number' option is set and only the
	// 'relativenumber' option is toggled, then don't refresh the screen
	// (optimization).
	nếu (!(curwin->w_p_nu && ((số_nguyên *)args->os_varp == &curwin->w_p_rnu)))
	    redraw_all_later(UPD_CLEAR);
    }
#endif
    trả_về NULL;
}

#nếu defined(FEAT_LINEBREAK)
/*
 * Process the new 'numberwidth' option value.
 */
    char *
did_set_numberwidth(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    // 'numberwidth' must be positive
    nếu (curwin->w_p_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_p_nuw = 1;
    }
    không_thì nếu (curwin->w_p_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_p_nuw = 20;
    }
    nếu (curwin->w_allbuf_opt.wo_nuw < 1)
    {
	errmsg = e_argument_must_be_positive;
	curwin->w_allbuf_opt.wo_nuw = 1;
    }
    không_thì nếu (curwin->w_allbuf_opt.wo_nuw > 20)
    {
	errmsg = e_invalid_argument;
	curwin->w_allbuf_opt.wo_nuw = 20;
    }
    curwin->w_nrwidth_line_count = 0; // trigger a redraw

    trả_về errmsg;
}
#endif

/*
 * Process the updated 'osctimeoutlen' option value.
 */
    char *
did_set_osctimeoutlen(optset_T *args)
{
    nếu (p_ost < 0)
    {
	p_ost = args->os_oldval.number;
	trả_về e_argument_must_be_positive;
    }

    trả_về NULL;
}

/*
 * Process the updated 'paste' option value.  Called after p_paste was set or
 * reset.  When 'paste' is set or reset also change other options.
 */
    char *
did_set_paste(optset_T *args UNUSED)
{
    static số_nguyên	old_p_paste = FALSE;
    static số_nguyên	save_sm = 0;
    static số_nguyên	save_sta = 0;
    static số_nguyên	save_ru = 0;
#ifdef FEAT_RIGHTLEFT
    static số_nguyên	save_ri = 0;
    static số_nguyên	save_hkmap = 0;
#endif
    buf_T	*buf;

    nếu (p_paste)
    {
	// Paste switched from off to on.
	// Save the current values, so they can be restored later.
	nếu (!old_p_paste)
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
		nếu (buf->b_p_vsts_nopaste)
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
	    nếu (p_vsts_nopaste)
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
	    nếu (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = empty_option;
	    VIM_CLEAR(buf->b_p_vsts_array);
#endif
	}

	// set global options
	p_sm = 0;		    // no showmatch
	p_sta = 0;		    // no smarttab
	nếu (p_ru)
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
	nếu (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = empty_option;
#endif
    }

    // Paste switched from on to off: Restore saved values.
    không_thì nếu (old_p_paste)
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
	    nếu (buf->b_p_vsts)
		free_string_option(buf->b_p_vsts);
	    buf->b_p_vsts = buf->b_p_vsts_nopaste
			 ? vim_strsave(buf->b_p_vsts_nopaste) : empty_option;
	    vim_free(buf->b_p_vsts_array);
	    nếu (buf->b_p_vsts && buf->b_p_vsts != empty_option)
		(void)tabstop_set(buf->b_p_vsts, &buf->b_p_vsts_array);
	    không_thì
		buf->b_p_vsts_array = NULL;
#endif
	}

	// restore global options
	p_sm = save_sm;
	p_sta = save_sta;
	nếu (p_ru != save_ru)
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
	nếu (p_vsts)
	    free_string_option(p_vsts);
	p_vsts = p_vsts_nopaste ? vim_strsave(p_vsts_nopaste) : empty_option;
#endif
    }

    old_p_paste = p_paste;

#nếu defined(FEAT_EVAL)
    // Remember where the dependent options were reset
    didset_options_sctx((OPT_LOCAL | OPT_GLOBAL), p_paste_dep_opts);
#endif

    trả_về NULL;
}


#ifdef FEAT_QUICKFIX
/*
 * Process the updated 'previewwindow' option value.
 */
    char *
did_set_previewwindow(optset_T *args UNUSED)
{
    nếu (!curwin->w_p_pvw)
	trả_về NULL;

    // There can be only one window with 'previewwindow' set.
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	nếu (win->w_p_pvw && win != curwin)
	{
	    curwin->w_p_pvw = FALSE;
	    trả_về e_preview_window_already_exists;
	}

    trả_về NULL;
}
#endif

#nếu defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Process the new 'pyxversion' option value.
 */
    char *
did_set_pyxversion(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    nếu (p_pyx != 0 && p_pyx != 2 && p_pyx != 3)
	errmsg = e_invalid_argument;

    trả_về errmsg;
}
#endif

/*
 * Process the updated 'readonly' option value.
 */
    char *
did_set_readonly(optset_T *args)
{
    // when 'readonly' is reset globally, also reset readonlymode
    nếu (!curbuf->b_p_ro && (args->os_flags & OPT_LOCAL) == 0)
	readonlymode = FALSE;

    // when 'readonly' is set may give W10 again
    nếu (curbuf->b_p_ro)
	curbuf->b_did_warn = false;

    redraw_titles();

    trả_về NULL;
}

/*
 * Process the updated 'scrollbind' option value.
 */
    char *
did_set_scrollbind(optset_T *args UNUSED)
{
    // when 'scrollbind' is set: snapshot the current position to avoid a jump
    // at the end of normal_cmd()
    nếu (!curwin->w_p_scb)
	trả_về NULL;

    do_check_scrollbind(FALSE);
    curwin->w_scbind_pos = curwin->w_topline;
    trả_về NULL;
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

    nếu (p_msc <= 0)
	errmsg = e_argument_must_be_positive;
    không_thì nếu (p_msc > MAX_SEARCH_COUNT)
	errmsg = e_invalid_argument;

    nếu (errmsg != NULL)
	p_msc = 99;

    trả_về errmsg;
#undef MAX_SEARCH_COUNT
}

#nếu defined(BACKSLASH_IN_FILENAME)
/*
 * Process the updated 'shellslash' option value.
 */
    char *
did_set_shellslash(optset_T *args UNUSED)
{
    nếu (p_ssl)
    {
	psepc = '/';
	psepcN = '\\';
	pseps[0] = '/';
    }
    không_thì
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
    trả_về NULL;
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

    nếu (curbuf->b_p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_ts = 8;
    }
    không_thì nếu (curbuf->b_p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	curbuf->b_p_ts = 8;
    }
    nếu (p_ts <= 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ts = 8;
    }
    không_thì nếu (p_ts > TABSTOP_MAX)
    {
	errmsg = e_invalid_argument;
	p_ts = 8;
    }

    nếu (curbuf->b_p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	curbuf->b_p_sw = tabstop_count(curbuf->b_p_vts_array) > 0
		       ? tabstop_first(curbuf->b_p_vts_array)
		       : curbuf->b_p_ts;
#không_thì
	curbuf->b_p_sw = curbuf->b_p_ts;
#endif
    }
    nếu (p_sw < 0)
    {
	errmsg = e_argument_must_be_positive;
#ifdef FEAT_VARTABS
	// Use the first 'vartabstop' value, or 'tabstop' if vts isn't in use.
	p_sw = tabstop_count(curbuf->b_p_vts_array) > 0
	     ? tabstop_first(curbuf->b_p_vts_array)
	     : curbuf->b_p_ts;
#không_thì
	p_sw = curbuf->b_p_ts;
#endif
    }

#ifdef FEAT_FOLDING
    nếu (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
#endif
    // When 'shiftwidth' changes, or it's zero and 'tabstop' changes:
    // parse 'cinoptions'.
    nếu (pp == &curbuf->b_p_sw || curbuf->b_p_sw == 0)
	parse_cino(curbuf);

    trả_về errmsg;
}

/*
 * Process the new 'showtabline' option value.
 */
    char *
did_set_showtabline(optset_T *args UNUSED)
{
    shell_new_rows();	// recompute window positions and heights
    trả_về NULL;
}

/*
 * Process the updated 'smoothscroll' option value.
 */
    char *
did_set_smoothscroll(optset_T *args UNUSED)
{
    nếu (!curwin->w_p_sms)
	curwin->w_skipcol = 0;

    trả_về NULL;
}

#nếu defined(FEAT_SPELL)
/*
 * Process the updated 'spell' option value.
 */
    char *
did_set_spell(optset_T *args UNUSED)
{
    nếu (curwin->w_p_spell)
	trả_về parse_spelllang(curwin);

    trả_về NULL;
}
#endif

/*
 * Process the updated 'swapfile' option value.
 */
    char *
did_set_swapfile(optset_T *args UNUSED)
{
    // when 'swf' is set, create swapfile, when reset remove swapfile
    nếu (curbuf->b_p_swf && p_uc)
	ml_open_file(curbuf);		// create the swap file
    không_thì
	// no need to reset curbuf->b_may_swap, ml_open_file() will check
	// buf->b_p_swf
	mf_close_file(curbuf, TRUE);	// remove the swap file
    trả_về NULL;
}

#nếu defined(FEAT_TERMGUICOLORS)
    char *
did_set_termguicolors(optset_T *args UNUSED)
{
# ifdef FEAT_VTP
    // Do not turn on 'tgc' when 24-bit colors are not supported.
    nếu (
#  ifdef VIMDLL
	    !gui.in_use && !gui.starting &&
#  endif
	    !has_vtp_working())
    {
	p_tgc_set = TRUE;
	p_tgc = 0;
	trả_về e_24_bit_colors_are_not_supported_on_this_environment;
    }
    nếu (is_term_win32())
	swap_tcap();
# endif
# ifdef FEAT_GUI
    nếu (!gui.in_use && !gui.starting)
# endif
	highlight_gui_started();
# ifdef FEAT_VTP
    // reset t_Co
    nếu (is_term_win32())
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

    trả_về NULL;
}
#endif

    char *
did_set_termsync(optset_T *args UNUSED)
{
    nếu (!p_tsy)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);
    trả_về NULL;
}

#nếu defined(FEAT_TERMINAL)
/*
 * Process the updated 'termwinscroll' option value.
 */
    char *
did_set_termwinscroll(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    char	*errmsg = NULL;

    nếu (*pp < 1)
    {
	errmsg = e_argument_must_be_positive;
	*pp = 1;
    }

    trả_về errmsg;
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
    nếu (p_terse && p == NULL)
    {
	STRCPY(IObuff, p_shm);
	STRCAT(IObuff, "s");
	set_string_option_direct((char_u *)"shm", -1, IObuff, OPT_FREE, 0);
    }
    // remove 's' from p_shm
    không_thì nếu (!p_terse && p != NULL)
	STRMOVE(p, p + 1);
    trả_về NULL;
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

    trả_về NULL;
}

/*
 * Process the updated 'textmode' option value.
 */
    char *
did_set_textmode(optset_T *args)
{
    // when 'textmode' is set or reset also change 'fileformat'
    set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX, args->os_flags);

    trả_về NULL;
}

/*
 * Process the new 'textwidth' option value.
 */
    char *
did_set_textwidth(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    nếu (curbuf->b_p_tw < 0)
    {
	errmsg = e_argument_must_be_positive;
	curbuf->b_p_tw = 0;
    }
    nếu (p_tw < 0)
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

    trả_về errmsg;
}

/*
 * Process the updated 'title' or the 'icon' option value.
 */
    char *
did_set_title_icon(optset_T *args UNUSED)
{
    // when 'title' changed, may need to change the title; same for 'icon'
    did_set_title();
    trả_về NULL;
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
    nếu (p_titlelen < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_titlelen = 85;
    }
    nếu (starting != NO_SCREEN && old_value != p_titlelen)
	need_maketitle = TRUE;

    trả_về errmsg;
}

#nếu defined(FEAT_PERSISTENT_UNDO)
/*
 * Process the updated 'undofile' option value.
 */
    char *
did_set_undofile(optset_T *args)
{
    // Only take action when the option was set.
    nếu (!curbuf->b_p_udf && !p_udf)
	trả_về NULL;

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
	nếu ((curbuf == save_curbuf
		    || (args->os_flags & OPT_GLOBAL)
		    || args->os_flags == 0)
		&& !curbufIsChanged() && curbuf->b_ml.ml_mfp != NULL)
	{
# ifdef FEAT_CRYPT
	    nếu (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
		continue;
# endif
	    u_compute_hash(hash);
	    u_read_undo(NULL, hash, curbuf->b_fname);
	}
    }
    curbuf = save_curbuf;

    trả_về NULL;
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

    nếu (pp == &p_ul)			// global 'undolevels'
	update_global_undolevels(args->os_newval.number,
						       args->os_oldval.number);
    không_thì nếu (pp == &curbuf->b_p_ul)	// buffer local 'undolevels'
	update_buflocal_undolevels(args->os_newval.number,
						       args->os_oldval.number);

    trả_về NULL;
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
    nếu (p_uc < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_uc = 100;
    }
    nếu (p_uc && !old_value)
	ml_open_files();

    trả_về errmsg;
}

/*
 * Process the updated 'weirdinvert' option value.
 */
    char *
did_set_weirdinvert(optset_T *args)
{
    // When 'weirdinvert' changed, set/reset 't_xs'.
    // Then set 'weirdinvert' according to value of 't_xs'.
    nếu (p_wiv && !args->os_oldval.boolean)
	T_XS = (char_u *)"y";
    không_thì nếu (!p_wiv && args->os_oldval.boolean)
	T_XS = empty_option;
    p_wiv = (*T_XS != NUL);

    trả_về NULL;
}

/*
 * Process the new 'wildchar' / 'wildcharm' option value.
 */
    char *
did_set_wildchar(optset_T *args)
{
    long c = *(long *)args->os_varp;

    // Don't allow key values that wouldn't work as wildchar.
    nếu (c == Ctrl_C || c == '\n' || c == '\r' || c == K_KENTER)
	trả_về e_invalid_argument;

    trả_về NULL;
}

/*
 * Process the new 'window' option value.
 */
    char *
did_set_window(optset_T *args UNUSED)
{
    nếu (p_window < 1)
	p_window = 1;
    không_thì nếu (p_window >= Rows)
	p_window = Rows - 1;
    trả_về NULL;
}

/*
 * Process the new 'winheight' or the 'helpheight' option value.
 */
    char *
did_set_winheight_helpheight(optset_T *args)
{
    long	*pp = (long *)args->os_varp;
    char	*errmsg = NULL;

    nếu (p_wh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wh = 1;
    }
    nếu (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wh = p_wmh;
    }
    nếu (p_hh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hh = 0;
    }

    // Change window height NOW
    nếu (!ONE_WINDOW)
    {
	nếu (pp == &p_wh && curwin->w_height < p_wh)
	    win_setheight((số_nguyên)p_wh);
	nếu (pp == &p_hh && curbuf->b_help && curwin->w_height < p_hh)
	    win_setheight((số_nguyên)p_hh);
    }

    trả_về errmsg;
}

/*
 * Process the new 'winminheight' option value.
 */
    char *
did_set_winminheight(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    nếu (p_wmh < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmh = 0;
    }
    nếu (p_wmh > p_wh)
    {
	errmsg = e_winheight_cannot_be_smaller_than_winminheight;
	p_wmh = p_wh;
    }
    win_setminheight();

    trả_về errmsg;
}

/*
 * Process the new 'winminwidth' option value.
 */
    char *
did_set_winminwidth(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    nếu (p_wmw < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_wmw = 0;
    }
    nếu (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wmw = p_wiw;
    }
    win_setminwidth();

    trả_về errmsg;
}

/*
 * Process the new 'winwidth' option value.
 */
    char *
did_set_winwidth(optset_T *args UNUSED)
{
    char *errmsg = NULL;

    nếu (p_wiw < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_wiw = 1;
    }
    nếu (p_wmw > p_wiw)
    {
	errmsg = e_winwidth_cannot_be_smaller_than_winminwidth;
	p_wiw = p_wmw;
    }

    // Change window width NOW
    nếu (!ONE_WINDOW && curwin->w_width < p_wiw)
	win_setwidth((số_nguyên)p_wiw);

    trả_về errmsg;
}

#nếu defined(FEAT_WAYLAND_CLIPBOARD)
/*
 * Process the new 'wlsteal' option value.
 */
    char *
did_set_wlsteal(optset_T *args UNUSED)
{
    clip_reset_wayland();

    trả_về NULL;
}
#endif

#nếu defined(FEAT_WAYLAND)
/*
 * Process the new 'wltimeoutlen' option value.
 */
    char *
did_set_wltimeoutlen(optset_T *args)
{
    nếu (p_wtm < 0)
    {
	p_wtm = args->os_oldval.number;
	trả_về e_argument_must_be_positive;
    }

    trả_về NULL;
}
#endif

/*
 * Process the updated 'wrap' option value.
 */
    char *
did_set_wrap(optset_T *args UNUSED)
{
    // Set w_leftcol or w_skipcol to zero.
    nếu (curwin->w_p_wrap)
	curwin->w_leftcol = 0;
    không_thì
	curwin->w_skipcol = 0;

    trả_về NULL;
}

#ifdef FEAT_QUICKFIX
/*
 * Process the new 'chistory' or 'lhistory' option value. 'chistory' will
 * be used if args->os_varp is the same as p_chi, else 'lhistory'.
 */
    char *
did_set_xhistory(optset_T *args)
{
    số_nguyên is_p_chi = (long*)args->os_varp == &p_chi;
    số_nguyên err;
    long *arg = (is_p_chi) ? &p_chi :(long*)args->os_varp;

    // cannot have zero or negative number of quickfix lists in a stack
    nếu (*arg < 1)
    {
	*arg = args->os_oldval.number;
	trả_về e_cannot_have_negative_or_zero_number_of_quickfix;
    }

    // cannot have more than 100 quickfix lists in a stack
    nếu (*arg > 100)
    {
	*arg = args->os_oldval.number;
	trả_về e_cannot_have_more_than_hundred_quickfix;
    }

    nếu (is_p_chi)
	err = qf_resize_stack(*arg);
    không_thì
	err = ll_resize_stack(curwin, *arg);

    nếu (err == FAIL)
	trả_về e_failed_resizing_quickfix_stack;

    trả_về NULL;
}
#endif

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    static char *
set_bool_option(
    số_nguyên		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    số_nguyên		value,			// new value
    số_nguyên		opt_flags)		// OPT_LOCAL and/or OPT_GLOBAL
{
    số_nguyên		old_value = *(số_nguyên *)varp;
#nếu defined(FEAT_EVAL)
    số_nguyên		old_global_value = 0;
#endif
    char	*errmsg = NULL;

    // Disallow changing some options from secure mode
    nếu ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	trả_về e_not_allowed_here;

#nếu defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    nếu ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	old_global_value = *(số_nguyên *)get_varp_scope(&(options[opt_idx]),
								   OPT_GLOBAL);
#endif

    *(số_nguyên *)varp = value;	    // set the new value
#ifdef FEAT_EVAL
    // Remember where the option was set.
    set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // May set global value for local option.
    nếu ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(số_nguyên *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = value;

    // Handle side effects of changing a bool option.
    nếu (options[opt_idx].opt_did_set_cb != NULL)
    {
	optset_T args;

	CLEAR_FIELD(args);
	args.os_varp = varp;
	args.os_flags = opt_flags;
	args.os_oldval.boolean = old_value;
	args.os_newval.boolean = value;
	args.os_errbuf = NULL;
	errmsg = options[opt_idx].opt_did_set_cb(&args);
	nếu (errmsg != NULL)
	    trả_về errmsg;
    }

    // after handling side effects, call autocommand

    options[opt_idx].flags |= P_WAS_SET;

#nếu defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags,
				(long)(old_value ? TRUE : FALSE),
				(long)(old_global_value ? TRUE : FALSE),
				(long)(value ? TRUE : FALSE), NULL);
#endif

    comp_col();			    // in case 'ruler' or 'showcmd' changed

    nếu (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    nếu ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    trả_về errmsg;
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
    nếu (Rows < min_rows_for_all_tabpages() && full_screen)
    {
	nếu (errbuf != NULL)
	{
	    vim_snprintf(errbuf, errbuflen,
		    _(e_need_at_least_nr_lines), min_rows_for_all_tabpages());
	    errmsg = errbuf;
	}
	Rows = min_rows_for_all_tabpages();
    }
    nếu (Columns < MIN_COLUMNS && full_screen)
    {
	nếu (errbuf != NULL)
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
    nếu (old_Rows != Rows || old_Columns != Columns)
    {
	// Changing the screen size is not allowed while updating the screen.
	nếu (updating_screen)
	    *pp = old_value;
	không_thì nếu (full_screen
#ifdef FEAT_GUI
		&& !gui.starting
#endif
		)
	    set_shellsize((số_nguyên)Columns, (số_nguyên)Rows, TRUE);
	không_thì
	{
	    // Postpone the resizing; check the size and cmdline position for
	    // messages.
	    check_shellsize();
	    nếu (cmdline_row > Rows - p_ch && Rows > p_ch)
		cmdline_row = Rows - p_ch;
	}
	nếu (p_window >= Rows || !option_was_set((char_u *)"window"))
	    p_window = Rows - 1;
    }

    nếu (p_tm < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_tm = 0;
    }
    nếu ((curwin->w_p_scr <= 0
		|| (curwin->w_p_scr > curwin->w_height
		    && curwin->w_height > 0))
	    && full_screen)
    {
	nếu (pp == &(curwin->w_p_scr))
	{
	    nếu (curwin->w_p_scr != 0)
		errmsg = e_invalid_scroll_size;
	    win_comp_scroll(curwin);
	}
	// If 'scroll' became invalid because of a side effect silently adjust
	// it.
	không_thì nếu (curwin->w_p_scr <= 0)
	    curwin->w_p_scr = 1;
	không_thì // curwin->w_p_scr > curwin->w_height
	    curwin->w_p_scr = curwin->w_height;
    }
    nếu (p_hi < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_hi = 0;
    }
    không_thì nếu (p_hi > 10000)
    {
	errmsg = e_invalid_argument;
	p_hi = 10000;
    }
    nếu (p_re < 0 || p_re > 2)
    {
	errmsg = e_invalid_argument;
	p_re = 0;
    }
    nếu (p_report < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_report = 1;
    }
    nếu ((p_sj < -100 || p_sj >= Rows) && full_screen)
    {
	nếu (Rows != old_Rows)	// Rows changed, just adjust p_sj
	    p_sj = Rows / 2;
	không_thì
	{
	    errmsg = e_invalid_scroll_size;
	    p_sj = 1;
	}
    }
    nếu (p_so < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_so = 0;
    }
    nếu (p_sop < 0 && full_screen)
    {
	errmsg = e_invalid_argument;
	p_sop = 0;
    }
    nếu (p_siso < 0 && full_screen)
    {
	errmsg = e_argument_must_be_positive;
	p_siso = 0;
    }
    nếu (p_cwh < 1)
    {
	errmsg = e_argument_must_be_positive;
	p_cwh = 1;
    }
    nếu (p_ut < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ut = 2000;
    }
    nếu (p_ss < 0)
    {
	errmsg = e_argument_must_be_positive;
	p_ss = 0;
    }

    trả_về errmsg;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    static char *
set_num_option(
    số_nguyên		opt_idx,		// index in options[] table
    char_u	*varp,			// pointer to the option variable
    long	value,			// new value
    char	*errbuf,		// buffer for error messages
    size_t	errbuflen,		// length of "errbuf"
    số_nguyên		opt_flags)		// OPT_LOCAL, OPT_GLOBAL,
					// OPT_MODELINE, etc.
{
    char	*errmsg = NULL;
    long	old_value = *(long *)varp;
#nếu defined(FEAT_EVAL)
    long	old_global_value = 0;	// only used when setting a local and
					// global option
#endif
    long	old_Rows = Rows;	// remember old Rows
    long	old_Columns = Columns;	// remember old Columns
    long	*pp = (long *)varp;

    // Disallow changing some options from secure mode.
    nếu ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (options[opt_idx].flags & P_SECURE))
	trả_về e_not_allowed_here;

#nếu defined(FEAT_EVAL)
    // Save the global value before changing anything. This is needed as for
    // a global-only option setting the "local value" in fact sets the global
    // value (since there is only one value).
    nếu ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
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
    nếu ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
	*(long *)get_varp_scope(&(options[opt_idx]), OPT_GLOBAL) = value;

    // Invoke the option specific callback function to validate and apply the
    // new value.
    nếu (options[opt_idx].opt_did_set_cb != NULL)
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

#nếu defined(FEAT_EVAL)
    apply_optionset_autocmd(opt_idx, opt_flags, old_value, old_global_value,
								value, errmsg);
#endif

    comp_col();			    // in case 'columns' or 'ls' changed

    nếu (curwin->w_curswant != MAXCOL
		     && (options[opt_idx].flags & (P_CURSWANT | P_RALL)) != 0
				   && (options[opt_idx].flags & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    nếu ((opt_flags & OPT_NO_REDRAW) == 0)
	check_redraw(options[opt_idx].flags);

    trả_về errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    void
check_redraw(long_u flags)
{
    // Careful: P_RCLR and P_RALL are a combination of other P_ flags
    số_nguyên		doclear = (flags & P_RCLR) == P_RCLR;
    số_nguyên		all = ((flags & P_RALL) == P_RALL || doclear);

    nếu ((flags & P_RSTAT) || all)	// mark all status lines dirty
	status_redraw_all();

    nếu ((flags & P_RBUF) || (flags & P_RWIN) || all)
    {
	nếu (flags & P_HLONLY)
	    redraw_later(UPD_NOT_VALID);
	không_thì
	    changed_window_setting();
    }
    nếu (flags & P_RBUF)
	redraw_curbuf_later(UPD_NOT_VALID);
    nếu (doclear)
	redraw_all_later(UPD_CLEAR);
    không_thì nếu (all)
	redraw_all_later(UPD_NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    số_nguyên
findoption(char_u *arg)
{
    số_nguyên		    opt_idx;
    char	    *s, *p;
    static short    quick_tab[27] = {0, 0};	// quick access table
    số_nguyên		    is_term_opt;

    // For first call: Initialize the quick-access table.
    // It contains the index for the first option that starts with a certain
    // letter.  There are 26 letters, plus the first "t_" option.
    nếu (quick_tab[1] == 0)
    {
	// Make sure we do not leave any entries uninitialized, even if we have
	// no options that start with a particular letter.
	số_nguyên last_opt_idx;
	cho (last_opt_idx = 0; options[last_opt_idx].fullname != NULL;
		last_opt_idx++)
	    ;
	cho (số_nguyên tab_idx = 1; tab_idx < 27; tab_idx++)
	    quick_tab[tab_idx] = last_opt_idx;

	p = options[0].fullname;
	cho (opt_idx = 1; opt_idx < last_opt_idx; opt_idx++)
	{
	    s = options[opt_idx].fullname;
	    nếu (s[0] != p[0])
	    {
		nếu (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		không_thì
		    quick_tab[CharOrdLow(s[0])] = opt_idx;
	    }
	    p = s;
	}
    }

    // Check for name starting with an illegal character.
    nếu (arg[0] < 'a' || arg[0] > 'z')
	trả_về -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    nếu (is_term_opt)
	opt_idx = quick_tab[26];
    không_thì
	opt_idx = quick_tab[CharOrdLow(arg[0])];
    cho (; (s = options[opt_idx].fullname) != NULL && s[0] == arg[0]; opt_idx++)
    {
	nếu (STRCMP(arg, s) == 0)		    // match full name
	    break;
    }
    nếu (s != NULL && s[0] != arg[0])
	s = NULL;
    nếu (s == NULL && !is_term_opt)
    {
	opt_idx = quick_tab[CharOrdLow(arg[0])];
	cho ( ; options[opt_idx].fullname != NULL; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    nếu (s != NULL && STRCMP(arg, s) == 0)   // match short name
		break;
	    s = NULL;
	}
    }
    nếu (s == NULL)
	opt_idx = -1;
    trả_về opt_idx;
}

#nếu defined(FEAT_EVAL) || defined(FEAT_TCL) || defined(FEAT_MZSCHEME) \
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
    số_nguyên		*flagsp,
    số_nguyên		scope)
{
    số_nguyên		opt_idx;
    char_u	*varp;

    opt_idx = findoption(name);
    nếu (opt_idx < 0)		    // option not in the table
    {
	số_nguyên key;

	nếu (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
				  && (key = find_key_option(name, FALSE)) != 0)
	{
	    char_u key_name[2];
	    char_u *p;

	    nếu (flagsp != NULL)
		*flagsp = 0;  // terminal option has no flags

	    // check for a terminal option
	    nếu (key < 0)
	    {
		key_name[0] = KEY2TERMCAP0(key);
		key_name[1] = KEY2TERMCAP1(key);
	    }
	    không_thì
	    {
		key_name[0] = KS_KEY;
		key_name[1] = (key & 0xff);
	    }
	    p = find_termcode(key_name);
	    nếu (p != NULL)
	    {
		nếu (stringval != NULL)
		    *stringval = vim_strsave(p);
		trả_về gov_string;
	    }
	}
	trả_về gov_unknown;
    }

    varp = get_varp_scope(&(options[opt_idx]), scope);

    nếu (flagsp != NULL)
	// Return the P_xxxx option flags.
	*flagsp = options[opt_idx].flags;

    nếu (options[opt_idx].flags & P_STRING)
    {
	nếu (varp == NULL)		    // hidden option
	    trả_về gov_hidden_string;
	nếu (stringval != NULL)
	{
	    nếu ((char_u **)varp == &p_pt)	// 'pastetoggle'
		*stringval = str2special_save(*(char_u **)(varp), FALSE,
									FALSE);
# ifdef FEAT_CRYPT
	    // never return the value of the crypt key
	    không_thì nếu ((char_u **)varp == &curbuf->b_p_key
						&& **(char_u **)(varp) != NUL)
		*stringval = vim_strnsave((char_u *)"*****", STRLEN_LITERAL("*****"));
# endif
	    không_thì
		*stringval = vim_strsave(*(char_u **)(varp));
	}
	trả_về gov_string;
    }

    nếu (varp == NULL)		    // hidden option
	trả_về (options[opt_idx].flags & P_NUM)
					 ? gov_hidden_number : gov_hidden_bool;
    nếu (options[opt_idx].flags & P_NUM)
	*numval = *(long *)varp;
    không_thì
    {
	// Special case: 'modified' is b_changed, but we also want to consider
	// it set when 'ff' or 'fenc' changed.
	nếu ((số_nguyên *)varp == &curbuf->b_changed)
	    *numval = curbufIsChanged();
	không_thì
	    *numval = (long) *(số_nguyên *)varp;
    }
    trả_về (options[opt_idx].flags & P_NUM) ? gov_number : gov_bool;
}
#endif

#nếu defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    số_nguyên
get_option_value_strict(
    char_u	*name,
    long	*numval,
    char_u	**stringval,	    // NULL when only obtaining attributes
    số_nguyên		opt_type,
    void	*from)
{
    số_nguyên		opt_idx;
    char_u	*varp = NULL;
    struct vimoption *p;
    số_nguyên		r = 0;

    opt_idx = findoption(name);
    nếu (opt_idx < 0)
	trả_về 0;

    p = &(options[opt_idx]);

    // Hidden option
    nếu (p->var == NULL)
	trả_về 0;

    nếu (p->flags & P_BOOL)
	r |= SOPT_BOOL;
    không_thì nếu (p->flags & P_NUM)
	r |= SOPT_NUM;
    không_thì nếu (p->flags & P_STRING)
	r |= SOPT_STRING;

    nếu (p->indir == PV_NONE)
    {
	nếu (opt_type == SREQ_GLOBAL)
	    r |= SOPT_GLOBAL;
	không_thì
	    trả_về 0; // Did not request global-only option
    }
    không_thì
    {
	nếu (p->indir & PV_BOTH)
	    r |= SOPT_GLOBAL;
	không_thì nếu (opt_type == SREQ_GLOBAL)
	    trả_về 0; // Requested global option

	nếu (p->indir & PV_WIN)
	{
	    nếu (opt_type == SREQ_BUF)
		trả_về 0; // Did not request window-local option
	    không_thì
		r |= SOPT_WIN;
	}
	không_thì nếu (p->indir & PV_BUF)
	{
	    nếu (opt_type == SREQ_WIN)
		trả_về 0; // Did not request buffer-local option
	    không_thì
		r |= SOPT_BUF;
	}
    }

    nếu (stringval == NULL)
	trả_về r;

    nếu (opt_type == SREQ_GLOBAL)
	varp = p->var;
    không_thì
    {
	nếu (opt_type == SREQ_BUF)
	{
	    // Special case: 'modified' is b_changed, but we also want to
	    // consider it set when 'ff' or 'fenc' changed.
	    nếu (p->indir == PV_MOD)
	    {
		*numval = bufIsChanged((buf_T *)from);
		varp = NULL;
	    }
# ifdef FEAT_CRYPT
	    không_thì nếu (p->indir == PV_KEY)
	    {
		// never return the value of the crypt key
		*stringval = NULL;
		varp = NULL;
	    }
# endif
	    không_thì
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
	không_thì nếu (opt_type == SREQ_WIN)
	{
	    win_T	*save_curwin = curwin;

	    curwin = (win_T *)from;
	    curbuf = curwin->w_buffer;
	    varp = get_varp(p);
	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	}
	nếu (varp == p->var)
	    trả_về (r | SOPT_UNSET);
    }

    nếu (varp != NULL)
    {
	nếu (p->flags & P_STRING)
	    *stringval = vim_strsave(*(char_u **)(varp));
	không_thì nếu (p->flags & P_NUM)
	    *numval = *(long *) varp;
	không_thì
	    *numval = *(số_nguyên *)varp;
    }

    trả_về r;
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
option_iter_next(void **option, số_nguyên opt_type)
{
    struct vimoption	*ret = NULL;
    do
    {
	nếu (*option == NULL)
	    *option = (void *) options;
	không_thì nếu (((struct vimoption *) (*option))->fullname == NULL)
	{
	    *option = NULL;
	    trả_về NULL;
	}
	không_thì
	    *option = (void *) (((struct vimoption *) (*option)) + 1);

	ret = ((struct vimoption *) (*option));

	// Hidden option
	nếu (ret->var == NULL)
	{
	    ret = NULL;
	    continue;
	}

	switch (opt_type)
	{
	    case SREQ_GLOBAL:
		nếu (!(ret->indir == PV_NONE || ret->indir & PV_BOTH))
		    ret = NULL;
		break;
	    case SREQ_BUF:
		nếu (!(ret->indir & PV_BUF))
		    ret = NULL;
		break;
	    case SREQ_WIN:
		nếu (!(ret->indir & PV_WIN))
		    ret = NULL;
		break;
	    default:
		internal_error("option_iter_next()");
		trả_về NULL;
	}
    }
    trong_khi (ret == NULL);

    trả_về (char_u *)ret->fullname;
}
#endif

/*
 * Return the flags for the option at 'opt_idx'.
 */
    long_u
get_option_flags(số_nguyên opt_idx)
{
    trả_về options[opt_idx].flags;
}

/*
 * Set a flag for the option at 'opt_idx'.
 */
    void
set_option_flag(số_nguyên opt_idx, long_u flag)
{
    options[opt_idx].flags |= flag;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option
 */
    số_nguyên
is_global_option(số_nguyên opt_idx)
{
    trả_về options[opt_idx].indir == PV_NONE;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a global option which also has a
 * local value.
 */
    số_nguyên
is_global_local_option(số_nguyên opt_idx)
{
    trả_về options[opt_idx].indir & PV_BOTH;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a window-local option
 */
    số_nguyên
is_window_local_option(số_nguyên opt_idx)
{
    trả_về options[opt_idx].var == VAR_WIN;
}

/*
 * Returns TRUE if the option at 'opt_idx' is a hidden option
 */
    số_nguyên
is_hidden_option(số_nguyên opt_idx)
{
    trả_về options[opt_idx].var == NULL;
}

#nếu defined(FEAT_CRYPT)
/*
 * Returns TRUE if the option at 'opt_idx' is a crypt key option
 */
    số_nguyên
is_crypt_key_option(số_nguyên opt_idx)
{
    trả_về options[opt_idx].indir == PV_KEY;
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
    số_nguyên		opt_flags)	// OPT_LOCAL or 0 (both)
{
    số_nguyên		opt_idx;
    char_u	*varp;
    long_u	flags;
    static char	errbuf[ERR_BUFLEN];
    số_nguyên		errbuflen = ERR_BUFLEN;

    opt_idx = findoption(name);
    nếu (opt_idx < 0)
    {
	số_nguyên key;

	nếu (STRLEN(name) == 4 && name[0] == 't' && name[1] == '_'
		&& (key = find_key_option(name, FALSE)) != 0)
	{
	    char_u key_name[2];

	    nếu (key < 0)
	    {
		key_name[0] = KEY2TERMCAP0(key);
		key_name[1] = KEY2TERMCAP1(key);
	    }
	    không_thì
	    {
		key_name[0] = KS_KEY;
		key_name[1] = (key & 0xff);
	    }
	    add_termcode(key_name, string, FALSE);
	    nếu (full_screen)
		ttest(FALSE);
	    redraw_all_later(UPD_CLEAR);
	    trả_về NULL;
	}

	semsg(_(e_unknown_option_str_2), name);
    }
    không_thì
    {
	flags = options[opt_idx].flags;
#ifdef HAVE_SANDBOX
	// Disallow changing some options in the sandbox
	nếu (sandbox > 0 && (flags & P_SECURE))
	{
	    emsg(_(e_not_allowed_in_sandbox));
	    trả_về NULL;
	}
#endif
	nếu (flags & P_STRING)
	    trả_về set_string_option(opt_idx, string, opt_flags, errbuf, errbuflen);

	varp = get_varp_scope(&(options[opt_idx]), opt_flags);
	nếu (varp != NULL)	// hidden option is not changed
	{
	    nếu (number == 0 && string != NULL)
	    {
		số_nguyên idx;

		// Either we are given a string or we are setting option
		// to zero.
		cho (idx = 0; string[idx] == '0'; ++idx)
		    ;
		nếu (string[idx] != NUL || idx == 0)
		{
		    // There's another character after zeros or the string
		    // is empty.  In both cases, we are trying to set a
		    // num option using a string.
		    semsg(_(e_number_required_after_str_equal_str),
								 name, string);
		    trả_về NULL;     // do nothing as we hit an error

		}
	    }
	    nếu (flags & P_NUM)
	    {
		trả_về set_num_option(opt_idx, varp, number,
					   errbuf, sizeof(errbuf), opt_flags);
	    }
	    không_thì
		trả_về set_bool_option(opt_idx, varp, (số_nguyên)number, opt_flags);
	}

    }
    trả_về NULL;
}

/*
 * Call set_option_value() and when an error is returned report it.
 */
    void
set_option_value_give_err(
    char_u	*name,
    long	number,
    char_u	*string,
    số_nguyên		opt_flags)	// OPT_LOCAL or 0 (both)
{
    char *errmsg = set_option_value(name, number, string, opt_flags);

    nếu (errmsg != NULL)
	emsg(_(errmsg));
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_term_code(char_u *tname)
{
    số_nguyên	    opt_idx;
    char_u  *varp;

    nếu (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	trả_về NULL;
    nếu ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	nếu (varp != NULL)
	    varp = *(char_u **)(varp);
	trả_về varp;
    }
    trả_về find_termcode(tname + 2);
}

    char_u *
get_highlight_default(void)
{
    số_nguyên i;

    i = findoption((char_u *)"hl");
    nếu (i >= 0)
	trả_về options[i].def_val[VI_DEFAULT];
    trả_về (char_u *)NULL;
}

    char_u *
get_encoding_default(void)
{
    số_nguyên i;

    i = findoption((char_u *)"enc");
    nếu (i >= 0)
	trả_về options[i].def_val[VI_DEFAULT];
    trả_về (char_u *)NULL;
}

#nếu defined(FEAT_QUICKFIX)
    số_nguyên
is_option_allocated(char *name)
{
    số_nguyên idx = findoption((char_u *)name);

    trả_về idx >= 0 && (options[idx].flags & P_ALLOCED);
}
#endif

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 * When "has_lt" is true there is a '<' before "*arg_arg".
 * Returns 0 when the key is not recognized.
 */
    static số_nguyên
find_key_option(char_u *arg_arg, số_nguyên has_lt)
{
    số_nguyên		key = 0;
    số_nguyên		modifiers;
    char_u	*arg = arg_arg;

    // Don't use get_special_key_code() for t_xx, we don't want it to call
    // add_termcap_entry().
    nếu (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
    {
	nếu (!has_lt || arg[4] == '>')
	    key = TERMCAP2KEY(arg[2], arg[3]);
    }
    không_thì nếu (has_lt)
    {
	--arg;			    // put arg at the '<'
	modifiers = 0;
	key = find_special_key(&arg, &modifiers,
			    FSK_KEYCODE | FSK_KEEP_X_KEY | FSK_SIMPLIFY, NULL);
	nếu (modifiers)		    // can't handle modifiers here
	    key = 0;
    }
    trả_về key;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    static void
showoptions(
    số_nguyên		all,
    số_nguyên		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    struct vimoption	*p;
    số_nguyên			col;
    số_nguyên			isterm;
    char_u		*varp;
    struct vimoption	**items;
    số_nguyên			item_count;
    số_nguyên			run;
    số_nguyên			row, rows;
    số_nguyên			cols;
    số_nguyên			i;
    số_nguyên			len;

#define INC 20
#define GAP 3

    items = ALLOC_MULT(struct vimoption *, OPTION_COUNT);
    nếu (items == NULL)
	trả_về;

    // Highlight title
    nếu (all == 2)
	msg_puts_title(_("\n--- Terminal codes ---"));
    không_thì nếu (opt_flags & OPT_GLOBAL)
	msg_puts_title(_("\n--- Global option values ---"));
    không_thì nếu (opt_flags & OPT_LOCAL)
	msg_puts_title(_("\n--- Local option values ---"));
    không_thì
	msg_puts_title(_("\n--- Options ---"));

    // Do the loop two times:
    // 1. display the short items
    // 2. display the long items (only strings and numbers)
    // When "opt_flags" has OPT_ONECOLUMN do everything in run 2.
    cho (run = 1; run <= 2 && !got_int; ++run)
    {
	// collect the items in items[]
	item_count = 0;
	cho (p = &options[0]; p->fullname != NULL; p++)
	{
	    // apply :filter /pat/
	    nếu (message_filtered((char_u *)p->fullname))
		continue;

	    varp = NULL;
	    isterm = istermoption(p);
	    nếu ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) != 0)
	    {
		nếu (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    không_thì
		varp = get_varp(p);
	    nếu (varp != NULL
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp, p_cp))))
	    {
		nếu (opt_flags & OPT_ONECOLUMN)
		    len = Columns;
		không_thì nếu (p->flags & P_BOOL)
		    len = 1;		// a toggle option fits always
		không_thì
		{
		    option_value2string(p, opt_flags);
		    len = (số_nguyên)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		nếu ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    items[item_count++] = p;
	    }
	}

	// display the items
	nếu (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    nếu (cols == 0)
		cols = 1;
	    rows = (item_count + cols - 1) / cols;
	}
	không_thì	// run == 2
	    rows = item_count;
	cho (row = 0; row < rows && !got_int; ++row)
	{
	    msg_putchar('\n');			// go to next line
	    nếu (got_int)			// 'q' typed in more
		break;
	    col = 0;
	    cho (i = row; i < item_count; i += rows)
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
    static số_nguyên
optval_default(struct vimoption *p, char_u *varp, số_nguyên compatible)
{
    số_nguyên		dvi;

    nếu (varp == NULL)
	trả_về TRUE;	    // hidden option is always at default
    dvi = ((p->flags & P_VI_DEF) || compatible) ? VI_DEFAULT : VIM_DEFAULT;
    nếu (p->flags & P_NUM)
	trả_về (*(long *)varp == (long)(long_i)p->def_val[dvi]);
    nếu (p->flags & P_BOOL)
			// the cast to long is required for Manx C, long_i is
			// needed for MSVC
	trả_về (*(số_nguyên *)varp == (số_nguyên)(long)(long_i)p->def_val[dvi]);
    // P_STRING
    trả_về (STRCMP(*(char_u **)varp, p->def_val[dvi]) == 0);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    static void
showoneopt(
    struct vimoption	*p,
    số_nguyên			opt_flags)	// OPT_LOCAL or OPT_GLOBAL
{
    char_u	*varp;
    số_nguyên		save_silent = silent_mode;

    silent_mode = FALSE;
    info_message = TRUE;	// use mch_msg(), not mch_errmsg()

    varp = get_varp_scope(p, opt_flags);

    // for 'modified' we also need to check if 'ff' or 'fenc' changed.
    nếu ((p->flags & P_BOOL) && ((số_nguyên *)varp == &curbuf->b_changed
					? !curbufIsChanged() : !*(số_nguyên *)varp))
	msg_puts("no");
    không_thì nếu ((p->flags & P_BOOL) && *(số_nguyên *)varp < 0)
	msg_puts("--");
    không_thì
	msg_puts("  ");
    msg_puts(p->fullname);
    nếu (!(p->flags & P_BOOL))
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
    số_nguyên
makeset(FILE *fd, số_nguyên opt_flags, số_nguyên local_only)
{
    struct vimoption	*p;
    char_u		*varp;			// currently used value
    char_u		*varp_fresh;		// local value
    char_u		*varp_local = NULL;	// fresh value
    char		*cmd;
    số_nguyên			round;
    số_nguyên			pri;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written.  Terminal options are also not written.
     * Do the loop over "options[]" twice: once for options with the
     * P_PRI_MKRC flag and once without.
     */
    cho (pri = 1; pri >= 0; --pri)
    {
      cho (p = &options[0]; !istermoption(p); p++)
	nếu (!(p->flags & P_NO_MKRC)
		&& !istermoption(p)
		&& ((pri == 1) == ((p->flags & P_PRI_MKRC) != 0)))
	{
	    // skip global option when only doing locals
	    nếu (p->indir == PV_NONE && !(opt_flags & OPT_GLOBAL))
		continue;

	    // Do not store options like 'bufhidden' and 'syntax' in a vimrc
	    // file, they are always buffer-specific.
	    nếu ((opt_flags & OPT_GLOBAL) && (p->flags & P_NOGLOB))
		continue;

	    // Global values are only written when not at the default value.
	    varp = get_varp_scope(p, opt_flags);
	    nếu ((opt_flags & OPT_GLOBAL) && optval_default(p, varp, p_cp))
		continue;

	    nếu ((opt_flags & OPT_SKIPRTP) && (p->var == (char_u *)&p_rtp
						 || p->var == (char_u *)&p_pp))
		continue;

	    round = 2;
	    nếu (p->indir != PV_NONE)
	    {
		nếu (p->var == VAR_WIN)
		{
		    // skip window-local option when only doing globals
		    nếu (!(opt_flags & OPT_LOCAL))
			continue;
		    // When fresh value of window-local option is not at the
		    // default, need to write it too.
		    nếu (!(opt_flags & OPT_GLOBAL) && !local_only)
		    {
			varp_fresh = get_varp_scope(p, OPT_GLOBAL);
			nếu (!optval_default(p, varp_fresh, p_cp))
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
	    cho ( ; round <= 2; varp = varp_local, ++round)
	    {
		nếu (round == 1 || (opt_flags & OPT_GLOBAL))
		    cmd = "set";
		không_thì
		    cmd = "setlocal";

		nếu (p->flags & P_BOOL)
		{
		    nếu (put_setbool(fd, cmd, p->fullname, *(số_nguyên *)varp) == FAIL)
			trả_về FAIL;
		}
		không_thì nếu (p->flags & P_NUM)
		{
		    nếu (put_setnum(fd, cmd, p->fullname, (long *)varp) == FAIL)
			trả_về FAIL;
		}
		không_thì    // P_STRING
		{
		    số_nguyên		do_endif = FALSE;
		    bool	legacy;

		    // Ignore those options associated to lambda expressions because
		    // persistence makes no sense for them.
		    nếu ((p->flags & P_FUNC) && *(char_u **)varp != NULL
			    && strstr(*(char **)varp, "<lambda>") != NULL)
			continue;

#ifdef FEAT_EVAL
		    legacy = !is_option_value_vim9(p - &options[0],
			    round == 1 ? opt_flags | OPT_GLOBAL : OPT_LOCAL);
#không_thì
		    // I think none of the options that require expression
		    // evaluation will be present if expression evaluation is
		    // disabled.  For example, 'includeexpr' is set to NULL if
		    // FEAT_EVAL is not present.  So it should be safe to use
		    // normal "set" and "setlocal" in the session file.
		    legacy = false;
#endif

		    // Don't set 'syntax' and 'filetype' again if the value is
		    // already right, avoids reloading the syntax file.
		    nếu (
#nếu defined(FEAT_SYN_HL)
			    p->indir == PV_SYN ||
#endif
			    p->indir == PV_FT)
		    {
			nếu (fprintf(fd, "if &%s != '%s'", p->fullname,
						       *(char_u **)(varp)) < 0
				|| put_eol(fd) < 0)
			    trả_về FAIL;
			do_endif = TRUE;
		    }
		    nếu (put_setstring(fd, legacy, cmd, p->fullname,
					    (char_u **)varp, p->flags) == FAIL)
			trả_về FAIL;
		    nếu (do_endif)
		    {
			nếu (put_line(fd, "endif") == FAIL)
			    trả_về FAIL;
		    }
		}
	    }
	}
    }
    trả_về OK;
}

#nếu defined(FEAT_FOLDING)
/*
 * Generate set commands for the local fold options only.  Used when
 * 'sessionoptions' or 'viewoptions' contains "folds" but not "options".
 */
    số_nguyên
makefoldset(FILE *fd)
{
# ifdef FEAT_EVAL
    sctx_T	*fde_script_ctx = &curwin->w_p_script_ctx[WV_FDE];
    // Similarly to the check in is_option_value_vim9(), if "sc_id" is not set
    // we have a default value, so it is safe to avoid the ":legacy" prefix.
    bool	fde_is_legacy = SCRIPT_ID_VALID(fde_script_ctx->sc_sid)
			&& fde_script_ctx->sc_version < SCRIPT_VERSION_VIM9;
# endif

    nếu (put_setstring(fd, false, "setlocal", "fdm", &curwin->w_p_fdm, 0)
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
	trả_về FAIL;

    trả_về OK;
}
#endif

    static số_nguyên
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

    nếu (legacy && fprintf(fd, "legacy ") < 0)
	trả_về FAIL;
    nếu (fprintf(fd, "%s %s=", cmd, name) < 0)
	trả_về FAIL;
    nếu (*valuep != NULL)
    {
	// Output 'pastetoggle' as key names.  For other
	// options some characters have to be escaped with
	// CTRL-V or backslash
	nếu (valuep == &p_pt)
	{
	    s = *valuep;
	    trong_khi (*s != NUL)
		nếu (put_escstr(fd, str2special(&s, FALSE, FALSE), 2) == FAIL)
		    trả_về FAIL;
	}
	// expand the option value, replace $HOME by ~
	không_thì nếu ((flags & P_EXPAND) != 0)
	{
	    số_nguyên  size = (số_nguyên)STRLEN(*valuep) + 1;

	    // replace home directory in the whole option value into "buf"
	    buf = alloc(size);
	    nếu (buf == NULL)
		goto fail;
	    home_replace(NULL, *valuep, buf, size, FALSE);

	    // If the option value is longer than MAXPATHL, we need to append
	    // each comma separated part of the option separately, so that it
	    // can be expanded when read back.
	    nếu (size >= MAXPATHL && (flags & P_COMMA) != 0
					   && vim_strchr(*valuep, ',') != NULL)
	    {
		part = alloc(size);
		nếu (part == NULL)
		    goto fail;

		// write line break to clear the option, e.g. ':set rtp='
		nếu (put_eol(fd) == FAIL)
		    goto fail;

		p = buf;
		trong_khi (*p != NUL)
		{
		    nếu (legacy && fprintf(fd, "legacy ") < 0)
			trả_về FAIL;
		    // for each comma separated option part, append value to
		    // the option, :set rtp+=value
		    nếu (fprintf(fd, "%s %s+=", cmd, name) < 0)
			goto fail;
		    (void)copy_option_part(&p, part, size,  ",");
		    nếu (put_escstr(fd, part, 2) == FAIL || put_eol(fd) == FAIL)
			goto fail;
		}
		vim_free(buf);
		vim_free(part);
		trả_về OK;
	    }
	    nếu (put_escstr(fd, buf, 2) == FAIL)
	    {
		vim_free(buf);
		trả_về FAIL;
	    }
	    vim_free(buf);
	}
	không_thì nếu (put_escstr(fd, *valuep, 2) == FAIL)
	    trả_về FAIL;
    }
    nếu (put_eol(fd) < 0)
	trả_về FAIL;
    trả_về OK;
fail:
    vim_free(buf);
    vim_free(part);
    trả_về FAIL;
}

    static số_nguyên
put_setnum(
    FILE	*fd,
    char	*cmd,
    char	*name,
    long	*valuep)
{
    long	wc;

    nếu (fprintf(fd, "%s %s=", cmd, name) < 0)
	trả_về FAIL;
    nếu (wc_use_keyname((char_u *)valuep, &wc))
    {
	// print 'wildchar' and 'wildcharm' as a key name
	nếu (fputs((char *)get_special_key_name((số_nguyên)wc, 0), fd) < 0)
	    trả_về FAIL;
    }
    không_thì nếu (fprintf(fd, "%ld", *valuep) < 0)
	trả_về FAIL;
    nếu (put_eol(fd) < 0)
	trả_về FAIL;
    trả_về OK;
}

    static số_nguyên
put_setbool(
    FILE	*fd,
    char	*cmd,
    char	*name,
    số_nguyên		value)
{
    nếu (value < 0)	// global/local option using global value
	trả_về OK;
    nếu (fprintf(fd, "%s %s%s", cmd, value ? "" : "no", name) < 0
	    || put_eol(fd) < 0)
	trả_về FAIL;
    trả_về OK;
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
#nếu defined(FEAT_XCLIPBOARD) && defined(FEAT_GUI)
    // When starting the GUI close the display opened for the clipboard.
    // After restoring the title, because that will need the display.
    nếu (gui.starting)
	clear_xterm_clip();
#endif
    stoptermcap();			// stop termcap mode

    free_termoptions();
}

    void
free_termoptions(void)
{
    struct vimoption   *p;

    cho (p = options; p->fullname != NULL; p++)
	nếu (istermoption(p))
	{
	    nếu (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    nếu (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = empty_option;
	    p->def_val[VI_DEFAULT] = empty_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
#ifdef FEAT_EVAL
	    // remember where the option was cleared
	    set_option_sctx_idx((số_nguyên)(p - options), OPT_GLOBAL, current_sctx);
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

    cho (p = &options[0]; p->fullname != NULL; p++)
	nếu (p->var == var)
	{
	    nếu (p->flags & P_ALLOCED)
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

    cho (p = &options[0]; p->fullname != NULL; p++)
    {
	nếu (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    nếu (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    nếu (p->flags & P_ALLOCED)
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
    static số_nguyên
istermoption(struct vimoption *p)
{
    trả_về (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Returns TRUE if the option at 'opt_idx' starts with 't_'
 */
    static số_nguyên
istermoption_idx(số_nguyên opt_idx)
{
    trả_về istermoption(&options[opt_idx]);
}

#nếu defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Unset local option value, similar to ":set opt<".
 */
    void
unset_global_local_option(char_u *name, void *from)
{
    struct vimoption *p;
    số_nguyên		opt_idx;
    buf_T	*buf = (buf_T *)from;

    opt_idx = findoption(name);
    nếu (opt_idx < 0)
	trả_về;
    p = &(options[opt_idx]);

    switch ((số_nguyên)p->indir)
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
# nếu defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	case PV_BEXPR:
	    clear_string_option(&buf->b_p_bexpr);
	    break;
# endif
# nếu defined(FEAT_CRYPT)
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
get_varp_scope(struct vimoption *p, số_nguyên scope)
{
    nếu ((scope & OPT_GLOBAL) && p->indir != PV_NONE)
    {
	nếu (p->var == VAR_WIN)
	    trả_về (char_u *)GLOBAL_WO(get_varp(p));
	trả_về p->var;
    }
    nếu ((scope & OPT_LOCAL) && ((số_nguyên)p->indir & PV_BOTH))
    {
	switch ((số_nguyên)p->indir)
	{
	    case PV_FP:   trả_về (char_u *)&(curbuf->b_p_fp);
#ifdef HAVE_FSYNC
	    case PV_FS:	trả_về (char_u *)&(curbuf->b_p_fs);
#endif
#ifdef FEAT_EVAL
	    case PV_FFU: trả_về (char_u *)&(curbuf->b_p_ffu);
#endif
#ifdef FEAT_QUICKFIX
	    case PV_EFM:  trả_về (char_u *)&(curbuf->b_p_efm);
	    case PV_GEFM: trả_về (char_u *)&(curbuf->b_p_gefm);
	    case PV_GP:   trả_về (char_u *)&(curbuf->b_p_gp);
	    case PV_MP:   trả_về (char_u *)&(curbuf->b_p_mp);
#endif
	    case PV_EP:   trả_về (char_u *)&(curbuf->b_p_ep);
	    case PV_KP:   trả_về (char_u *)&(curbuf->b_p_kp);
	    case PV_PATH: trả_về (char_u *)&(curbuf->b_p_path);
	    case PV_AC:   trả_về (char_u *)&(curbuf->b_p_ac);
	    case PV_AR:   trả_về (char_u *)&(curbuf->b_p_ar);
	    case PV_TAGS: trả_về (char_u *)&(curbuf->b_p_tags);
	    case PV_TC:   trả_về (char_u *)&(curbuf->b_p_tc);
	    case PV_SISO: trả_về (char_u *)&(curwin->w_p_siso);
	    case PV_SO:   trả_về (char_u *)&(curwin->w_p_so);
	    case PV_SOP:  trả_về (char_u *)&(curwin->w_p_sop);
#ifdef FEAT_FIND_ID
	    case PV_DEF:  trả_về (char_u *)&(curbuf->b_p_def);
	    case PV_INC:  trả_về (char_u *)&(curbuf->b_p_inc);
#endif
	    case PV_COT:  trả_về (char_u *)&(curbuf->b_p_cot);
	    case PV_DICT: trả_về (char_u *)&(curbuf->b_p_dict);
#ifdef FEAT_DIFF
	    case PV_DIA:  trả_về (char_u *)&(curbuf->b_p_dia);
#endif
	    case PV_TSR:  trả_về (char_u *)&(curbuf->b_p_tsr);
#ifdef FEAT_COMPL_FUNC
	    case PV_TSRFU: trả_về (char_u *)&(curbuf->b_p_tsrfu);
#endif
#nếu defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    case PV_BEXPR: trả_về (char_u *)&(curbuf->b_p_bexpr);
#endif
#nếu defined(FEAT_CRYPT)
	    case PV_CM:	  trả_về (char_u *)&(curbuf->b_p_cm);
#endif
#ifdef FEAT_LINEBREAK
	    case PV_SBR:  trả_về (char_u *)&(curwin->w_p_sbr);
#endif
#ifdef FEAT_STL_OPT
	    case PV_STL:  trả_về (char_u *)&(curwin->w_p_stl);
	    case PV_STLO: trả_về (char_u *)&(curwin->w_p_stlo);
#endif
	    case PV_UL:   trả_về (char_u *)&(curbuf->b_p_ul);
	    case PV_LW:   trả_về (char_u *)&(curbuf->b_p_lw);
	    case PV_BKC:  trả_về (char_u *)&(curbuf->b_p_bkc);
	    case PV_MENC: trả_về (char_u *)&(curbuf->b_p_menc);
	    case PV_LCS:  trả_về (char_u *)&(curwin->w_p_lcs);
	    case PV_FCS:  trả_về (char_u *)&(curwin->w_p_fcs);
	    case PV_VE:	  trả_về (char_u *)&(curwin->w_p_ve);

	}
	trả_về NULL; // "cannot happen"
    }
    trả_về get_varp(p);
}

/*
 * Get pointer to option variable at 'opt_idx', depending on local or global
 * scope.
 */
    char_u *
get_option_varp_scope(số_nguyên opt_idx, số_nguyên scope)
{
    trả_về get_varp_scope(&(options[opt_idx]), scope);
}

/*
 * Get pointer to option variable.
 */
    static char_u *
get_varp(struct vimoption *p)
{
    // hidden option, always return NULL
    nếu (p->var == NULL)
	trả_về NULL;

    switch ((số_nguyên)p->indir)
    {
	case PV_NONE:	trả_về p->var;

	// global option with local value: use local value if it's been set
	case PV_EP:	trả_về *curbuf->b_p_ep != NUL
				    ? (char_u *)&curbuf->b_p_ep : p->var;
	case PV_KP:	trả_về *curbuf->b_p_kp != NUL
				    ? (char_u *)&curbuf->b_p_kp : p->var;
	case PV_PATH:	trả_về *curbuf->b_p_path != NUL
				    ? (char_u *)&(curbuf->b_p_path) : p->var;
	case PV_AC:	trả_về curbuf->b_p_ac >= 0
				    ? (char_u *)&(curbuf->b_p_ac) : p->var;
	case PV_AR:	trả_về curbuf->b_p_ar >= 0
				    ? (char_u *)&(curbuf->b_p_ar) : p->var;
	case PV_TAGS:	trả_về *curbuf->b_p_tags != NUL
				    ? (char_u *)&(curbuf->b_p_tags) : p->var;
	case PV_TC:	trả_về *curbuf->b_p_tc != NUL
				    ? (char_u *)&(curbuf->b_p_tc) : p->var;
	case PV_BKC:	trả_về *curbuf->b_p_bkc != NUL
				    ? (char_u *)&(curbuf->b_p_bkc) : p->var;
	case PV_SISO:	trả_về curwin->w_p_siso >= 0
				    ? (char_u *)&(curwin->w_p_siso) : p->var;
	case PV_SO:	trả_về curwin->w_p_so >= 0
				    ? (char_u *)&(curwin->w_p_so) : p->var;
	case PV_SOP:	trả_về curwin->w_p_sop != -1
				    ? (char_u *)&(curwin->w_p_sop) : p->var;
#ifdef FEAT_FIND_ID
	case PV_DEF:	trả_về *curbuf->b_p_def != NUL
				    ? (char_u *)&(curbuf->b_p_def) : p->var;
	case PV_INC:	trả_về *curbuf->b_p_inc != NUL
				    ? (char_u *)&(curbuf->b_p_inc) : p->var;
#endif
	case PV_COT:	trả_về *curbuf->b_p_cot != NUL
				    ? (char_u *)&(curbuf->b_p_cot) : p->var;
	case PV_DICT:	trả_về *curbuf->b_p_dict != NUL
				    ? (char_u *)&(curbuf->b_p_dict) : p->var;
#ifdef FEAT_DIFF
	case PV_DIA:	trả_về *curbuf->b_p_dia != NUL
				    ? (char_u *)&(curbuf->b_p_dia) : p->var;
#endif
	case PV_TSR:	trả_về *curbuf->b_p_tsr != NUL
				    ? (char_u *)&(curbuf->b_p_tsr) : p->var;
#ifdef FEAT_COMPL_FUNC
	case PV_TSRFU:	trả_về *curbuf->b_p_tsrfu != NUL
				    ? (char_u *)&(curbuf->b_p_tsrfu) : p->var;
#endif
	case PV_FP:	trả_về *curbuf->b_p_fp != NUL
				    ? (char_u *)&(curbuf->b_p_fp) : p->var;
#ifdef HAVE_FSYNC
	case PV_FS:	trả_về curbuf->b_p_fs >= 0
				    ? (char_u *)&(curbuf->b_p_fs) : p->var;
#endif
#ifdef FEAT_EVAL
	case PV_FFU:	trả_về *curbuf->b_p_ffu != NUL
				    ? (char_u *)&(curbuf->b_p_ffu) : p->var;
#endif
#ifdef FEAT_QUICKFIX
	case PV_EFM:	trả_về *curbuf->b_p_efm != NUL
				    ? (char_u *)&(curbuf->b_p_efm) : p->var;
	case PV_GEFM:	trả_về *curbuf->b_p_gefm != NUL
				    ? (char_u *)&(curbuf->b_p_gefm) : p->var;
	case PV_GP:	trả_về *curbuf->b_p_gp != NUL
				    ? (char_u *)&(curbuf->b_p_gp) : p->var;
	case PV_MP:	trả_về *curbuf->b_p_mp != NUL
				    ? (char_u *)&(curbuf->b_p_mp) : p->var;
#endif
#nếu defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	case PV_BEXPR:	trả_về *curbuf->b_p_bexpr != NUL
				    ? (char_u *)&(curbuf->b_p_bexpr) : p->var;
#endif
#nếu defined(FEAT_CRYPT)
	case PV_CM:	trả_về *curbuf->b_p_cm != NUL
				    ? (char_u *)&(curbuf->b_p_cm) : p->var;
#endif
#ifdef FEAT_LINEBREAK
	case PV_SBR:	trả_về *curwin->w_p_sbr != NUL
				    ? (char_u *)&(curwin->w_p_sbr) : p->var;
#endif
#ifdef FEAT_STL_OPT
	case PV_STL:	trả_về *curwin->w_p_stl != NUL
				    ? (char_u *)&(curwin->w_p_stl) : p->var;
	case PV_STLO:	trả_về *curwin->w_p_stlo != NUL
				    ? (char_u *)&(curwin->w_p_stlo) : p->var;
#endif
	case PV_UL:	trả_về curbuf->b_p_ul != NO_LOCAL_UNDOLEVEL
				    ? (char_u *)&(curbuf->b_p_ul) : p->var;
	case PV_LW:	trả_về *curbuf->b_p_lw != NUL
				    ? (char_u *)&(curbuf->b_p_lw) : p->var;
	case PV_MENC:	trả_về *curbuf->b_p_menc != NUL
				    ? (char_u *)&(curbuf->b_p_menc) : p->var;
#ifdef FEAT_ARABIC
	case PV_ARAB:	trả_về (char_u *)&(curwin->w_p_arab);
#endif
	case PV_LIST:	trả_về (char_u *)&(curwin->w_p_list);
	case PV_LCS:	trả_về *curwin->w_p_lcs != NUL
				    ? (char_u *)&(curwin->w_p_lcs) : p->var;
	case PV_FCS:	trả_về *curwin->w_p_fcs != NUL
				    ? (char_u *)&(curwin->w_p_fcs) : p->var;
	case PV_VE:	trả_về *curwin->w_p_ve != NUL
				    ? (char_u *)&(curwin->w_p_ve) : p->var;
#ifdef FEAT_SPELL
	case PV_SPELL:	trả_về (char_u *)&(curwin->w_p_spell);
#endif
#ifdef FEAT_SYN_HL
	case PV_CUC:	trả_về (char_u *)&(curwin->w_p_cuc);
	case PV_CUL:	trả_về (char_u *)&(curwin->w_p_cul);
	case PV_CULOPT:	trả_về (char_u *)&(curwin->w_p_culopt);
	case PV_CC:	trả_về (char_u *)&(curwin->w_p_cc);
#endif
#ifdef FEAT_DIFF
	case PV_DIFF:	trả_về (char_u *)&(curwin->w_p_diff);
#endif
	case PV_EIW:	trả_về (char_u *)&(curwin->w_p_eiw);
#ifdef FEAT_FOLDING
	case PV_FDC:	trả_về (char_u *)&(curwin->w_p_fdc);
	case PV_FEN:	trả_về (char_u *)&(curwin->w_p_fen);
	case PV_FDI:	trả_về (char_u *)&(curwin->w_p_fdi);
	case PV_FDL:	trả_về (char_u *)&(curwin->w_p_fdl);
	case PV_FDM:	trả_về (char_u *)&(curwin->w_p_fdm);
	case PV_FML:	trả_về (char_u *)&(curwin->w_p_fml);
	case PV_FDN:	trả_về (char_u *)&(curwin->w_p_fdn);
# ifdef FEAT_EVAL
	case PV_FDE:	trả_về (char_u *)&(curwin->w_p_fde);
	case PV_FDT:	trả_về (char_u *)&(curwin->w_p_fdt);
# endif
	case PV_FMR:	trả_về (char_u *)&(curwin->w_p_fmr);
#endif
	case PV_NU:	trả_về (char_u *)&(curwin->w_p_nu);
	case PV_RNU:	trả_về (char_u *)&(curwin->w_p_rnu);
#ifdef FEAT_LINEBREAK
	case PV_NUW:	trả_về (char_u *)&(curwin->w_p_nuw);
#endif
	case PV_WFB:	trả_về (char_u *)&(curwin->w_p_wfb);
	case PV_WFH:	trả_về (char_u *)&(curwin->w_p_wfh);
	case PV_WFW:	trả_về (char_u *)&(curwin->w_p_wfw);
#nếu defined(FEAT_QUICKFIX)
	case PV_PVW:	trả_về (char_u *)&(curwin->w_p_pvw);
	case PV_LHI:	trả_về (char_u *)&(curwin->w_p_lhi);
#endif
#ifdef FEAT_RIGHTLEFT
	case PV_RL:	trả_về (char_u *)&(curwin->w_p_rl);
	case PV_RLC:	trả_về (char_u *)&(curwin->w_p_rlc);
#endif
	case PV_SCROLL:	trả_về (char_u *)&(curwin->w_p_scr);
	case PV_SMS:	trả_về (char_u *)&(curwin->w_p_sms);
	case PV_WRAP:	trả_về (char_u *)&(curwin->w_p_wrap);
#ifdef FEAT_LINEBREAK
	case PV_LBR:	trả_về (char_u *)&(curwin->w_p_lbr);
	case PV_BRI:	trả_về (char_u *)&(curwin->w_p_bri);
	case PV_BRIOPT: trả_về (char_u *)&(curwin->w_p_briopt);
#endif
	case PV_WCR:	trả_về (char_u *)&(curwin->w_p_wcr);
	case PV_SCBIND: trả_về (char_u *)&(curwin->w_p_scb);
	case PV_CRBIND: trả_về (char_u *)&(curwin->w_p_crb);
#ifdef FEAT_CONCEAL
	case PV_COCU:   trả_về (char_u *)&(curwin->w_p_cocu);
	case PV_COLE:   trả_về (char_u *)&(curwin->w_p_cole);
#endif
	case PV_WHL:	trả_về (char_u *)&(curwin->w_p_whl);

#ifdef FEAT_TERMINAL
	case PV_TWK:    trả_về (char_u *)&(curwin->w_p_twk);
	case PV_TWS:    trả_về (char_u *)&(curwin->w_p_tws);
	case PV_TWSL:	trả_về (char_u *)&(curbuf->b_p_twsl);
#endif

	case PV_AI:	trả_về (char_u *)&(curbuf->b_p_ai);
	case PV_BIN:	trả_về (char_u *)&(curbuf->b_p_bin);
	case PV_BOMB:	trả_về (char_u *)&(curbuf->b_p_bomb);
	case PV_BH:	trả_về (char_u *)&(curbuf->b_p_bh);
	case PV_BT:	trả_về (char_u *)&(curbuf->b_p_bt);
	case PV_BL:	trả_về (char_u *)&(curbuf->b_p_bl);
	case PV_CI:	trả_về (char_u *)&(curbuf->b_p_ci);
	case PV_CIN:	trả_về (char_u *)&(curbuf->b_p_cin);
	case PV_CINK:	trả_về (char_u *)&(curbuf->b_p_cink);
	case PV_CINO:	trả_về (char_u *)&(curbuf->b_p_cino);
	case PV_CINSD:	trả_về (char_u *)&(curbuf->b_p_cinsd);
	case PV_CINW:	trả_về (char_u *)&(curbuf->b_p_cinw);
	case PV_COM:	trả_về (char_u *)&(curbuf->b_p_com);
	case PV_CMS:	trả_về (char_u *)&(curbuf->b_p_cms);
	case PV_CPT:	trả_về (char_u *)&(curbuf->b_p_cpt);
#ifdef BACKSLASH_IN_FILENAME
	case PV_CSL:	trả_về (char_u *)&(curbuf->b_p_csl);
#endif
#ifdef FEAT_COMPL_FUNC
	case PV_CFU:	trả_về (char_u *)&(curbuf->b_p_cfu);
	case PV_OFU:	trả_về (char_u *)&(curbuf->b_p_ofu);
#endif
#ifdef FEAT_EVAL
	case PV_TFU:	trả_về (char_u *)&(curbuf->b_p_tfu);
#endif
	case PV_EOF:	trả_về (char_u *)&(curbuf->b_p_eof);
	case PV_EOL:	trả_về (char_u *)&(curbuf->b_p_eol);
	case PV_FIXEOL:	trả_về (char_u *)&(curbuf->b_p_fixeol);
	case PV_ET:	trả_về (char_u *)&(curbuf->b_p_et);
	case PV_FENC:	trả_về (char_u *)&(curbuf->b_p_fenc);
	case PV_FF:	trả_về (char_u *)&(curbuf->b_p_ff);
	case PV_FT:	trả_về (char_u *)&(curbuf->b_p_ft);
	case PV_FO:	trả_về (char_u *)&(curbuf->b_p_fo);
	case PV_FLP:	trả_về (char_u *)&(curbuf->b_p_flp);
	case PV_IMI:	trả_về (char_u *)&(curbuf->b_p_iminsert);
	case PV_IMS:	trả_về (char_u *)&(curbuf->b_p_imsearch);
	case PV_INF:	trả_về (char_u *)&(curbuf->b_p_inf);
	case PV_ISK:	trả_về (char_u *)&(curbuf->b_p_isk);
#ifdef FEAT_FIND_ID
# ifdef FEAT_EVAL
	case PV_INEX:	trả_về (char_u *)&(curbuf->b_p_inex);
# endif
#endif
#nếu defined(FEAT_EVAL)
	case PV_INDE:	trả_về (char_u *)&(curbuf->b_p_inde);
	case PV_INDK:	trả_về (char_u *)&(curbuf->b_p_indk);
#endif
#ifdef FEAT_EVAL
	case PV_FEX:	trả_về (char_u *)&(curbuf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
	case PV_KEY:	trả_về (char_u *)&(curbuf->b_p_key);
#endif
	case PV_LISP:	trả_về (char_u *)&(curbuf->b_p_lisp);
	case PV_LOP:	trả_về (char_u *)&(curbuf->b_p_lop);
	case PV_ML:	trả_về (char_u *)&(curbuf->b_p_ml);
	case PV_MPS:	trả_về (char_u *)&(curbuf->b_p_mps);
	case PV_MA:	trả_về (char_u *)&(curbuf->b_p_ma);
	case PV_MOD:	trả_về (char_u *)&(curbuf->b_changed);
	case PV_NF:	trả_về (char_u *)&(curbuf->b_p_nf);
	case PV_PI:	trả_về (char_u *)&(curbuf->b_p_pi);
	case PV_QE:	trả_về (char_u *)&(curbuf->b_p_qe);
	case PV_RO:	trả_về (char_u *)&(curbuf->b_p_ro);
	case PV_SI:	trả_về (char_u *)&(curbuf->b_p_si);
	case PV_SN:	trả_về (char_u *)&(curbuf->b_p_sn);
	case PV_STS:	trả_về (char_u *)&(curbuf->b_p_sts);
	case PV_SUA:	trả_về (char_u *)&(curbuf->b_p_sua);
	case PV_SWF:	trả_về (char_u *)&(curbuf->b_p_swf);
#ifdef FEAT_SYN_HL
	case PV_SMC:	trả_về (char_u *)&(curbuf->b_p_smc);
	case PV_SYN:	trả_về (char_u *)&(curbuf->b_p_syn);
#endif
#ifdef FEAT_SPELL
	case PV_SPC:	trả_về (char_u *)&(curwin->w_s->b_p_spc);
	case PV_SPF:	trả_về (char_u *)&(curwin->w_s->b_p_spf);
	case PV_SPL:	trả_về (char_u *)&(curwin->w_s->b_p_spl);
	case PV_SPO:	trả_về (char_u *)&(curwin->w_s->b_p_spo);
#endif
	case PV_SW:	trả_về (char_u *)&(curbuf->b_p_sw);
	case PV_TS:	trả_về (char_u *)&(curbuf->b_p_ts);
	case PV_TW:	trả_về (char_u *)&(curbuf->b_p_tw);
	case PV_TX:	trả_về (char_u *)&(curbuf->b_p_tx);
#ifdef FEAT_PERSISTENT_UNDO
	case PV_UDF:	trả_về (char_u *)&(curbuf->b_p_udf);
#endif
	case PV_WM:	trả_về (char_u *)&(curbuf->b_p_wm);
#ifdef FEAT_KEYMAP
	case PV_KMAP:	trả_về (char_u *)&(curbuf->b_p_keymap);
#endif
#ifdef FEAT_SIGNS
	case PV_SCL:	trả_về (char_u *)&(curwin->w_p_scl);
#endif
#ifdef FEAT_VARTABS
	case PV_VSTS:	trả_về (char_u *)&(curbuf->b_p_vsts);
	case PV_VTS:	trả_về (char_u *)&(curbuf->b_p_vts);
#endif
	default:	iemsg(e_get_varp_error);
    }
    // always return a valid pointer to avoid a crash!
    trả_về (char_u *)&(curbuf->b_p_wm);
}

/*
 * Return a pointer to the variable for option at 'opt_idx'
 */
    char_u *
get_option_var(số_nguyên opt_idx)
{
    trả_về options[opt_idx].var;
}

#nếu defined(FEAT_EVAL)
/*
 * Return the full name of the option at 'opt_idx'
 */
    char_u *
get_option_fullname(số_nguyên opt_idx)
{
    trả_về (char_u *)options[opt_idx].fullname;
}

/*
 * Add what getinfo() reports for the option "name" to "d".  "available" tells
 * whether the option is supported in this Vim.
 * Returns FAIL when there is no such option.
 */
    số_nguyên
option_info(char_u *name, dict_T *d)
{
    số_nguyên			opt_idx;
    struct vimoption	*p;
    số_nguyên			dvi;

    // The "g:" or "l:" scope does not matter for the definition.
    nếu ((name[0] == 'g' || name[0] == 'l') && name[1] == ':')
	name += 2;
    opt_idx = findoption(name);
    nếu (opt_idx < 0)
	trả_về FAIL;

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
    nếu (p->flags & P_BOOL)
	dict_add_bool(d, "default", (số_nguyên)(long_i)p->def_val[dvi]);
    không_thì nếu (p->flags & P_NUM)
	dict_add_number(d, "default", (long)(long_i)p->def_val[dvi]);
    không_thì
	dict_add_string(d, "default", p->def_val[dvi] == NULL
					? (char_u *)"" : p->def_val[dvi]);
    trả_về OK;
}
#endif

/*
 * Return the did_set callback function for the option at 'opt_idx'
 */
    opt_did_set_cb_T
get_option_did_set_cb(số_nguyên opt_idx)
{
    trả_về options[opt_idx].opt_did_set_cb;
}

/*
 * Get the value of 'equalprg', either the buffer-local one or the global one.
 */
    char_u *
get_equalprg(void)
{
    nếu (*curbuf->b_p_ep == NUL)
	trả_về p_ep;
    trả_về curbuf->b_p_ep;
}

/*
 * Get the value of 'findfunc', either the buffer-local one or the global one.
 */
    char_u *
get_findfunc(void)
{
#ifdef FEAT_EVAL
    nếu (*curbuf->b_p_ffu == NUL)
	trả_về p_ffu;
    trả_về curbuf->b_p_ffu;
#không_thì
    trả_về (char_u *)"";
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

    nếu (errmsg != NULL)
	emsg(_(errmsg));

    // Set w_leftcol or w_skipcol to zero.
    nếu (wp->w_p_wrap)
	wp->w_leftcol = 0;
    không_thì
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
    nếu (val == empty_option)
	trả_về empty_option;  // no need to allocate memory
    trả_về vim_strsave(val);
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
static số_nguyên buf_opt_idx[BV_COUNT];
# define COPY_OPT_SCTX(buf, bv) buf->b_p_script_ctx[bv] = options[buf_opt_idx[bv]].script_ctx
# define COPY_OPT_INSECURE(flagsfield, bv) \
	(flagsfield) = (options[buf_opt_idx[bv]].flags & P_INSECURE)

/*
 * Initialize buf_opt_idx[] if not done already.
 */
    static void
init_buf_opt_idx(void)
{
    static số_nguyên did_init_buf_opt_idx = FALSE;
    số_nguyên i;

    nếu (did_init_buf_opt_idx)
	trả_về;
    did_init_buf_opt_idx = TRUE;
    cho (i = 0; !istermoption_idx(i); i++)
	nếu (options[i].indir & PV_BUF)
	    buf_opt_idx[options[i].indir & PV_MASK] = i;
}
#không_thì
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
buf_copy_options(buf_T *buf, số_nguyên flags)
{
    số_nguyên		should_copy = TRUE;
    char_u	*save_p_isk = NULL;	    // init for GCC
    số_nguyên		dont_do_help;
    số_nguyên		did_isk = FALSE;

    // Skip this when the option defaults have not been set yet.  Happens when
    // main() allocates the first buffer.
    nếu (p_cpo != NULL)
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
	nếu ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == NULL || !(flags & BCO_ENTER))
		&& (buf->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != NULL)))
	    should_copy = FALSE;

	nếu (should_copy || (flags & BCO_ALWAYS))
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
	    nếu (dont_do_help)		// don't free b_p_isk
	    {
		save_p_isk = buf->b_p_isk;
		buf->b_p_isk = NULL;
	    }
	    // Always free the allocated strings.  If not already initialized,
	    // reset 'readonly' and copy 'fileformat'.
	    nếu (!buf->b_p_initialized)
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
		nếu (buf->b_p_ff != NULL)
		    buf->b_start_ffc = *buf->b_p_ff;
		buf->b_p_bh = empty_option;
		buf->b_p_bt = empty_option;
	    }
	    không_thì
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
	    nếu (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
		buf->b_p_swf = FALSE;
	    không_thì
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
	    nếu (p_vsts && p_vsts != empty_option)
		(void)tabstop_set(p_vsts, &buf->b_p_vsts_array);
	    không_thì
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
#nếu defined(FEAT_EVAL)
	    buf->b_p_inde = vim_strsave(p_inde);
	    COPY_OPT_SCTX(buf, BV_INDE);
	    COPY_OPT_INSECURE(buf->b_p_inde_flags, BV_INDE);
	    buf->b_p_indk = vim_strsave(p_indk);
	    COPY_OPT_SCTX(buf, BV_INDK);
#endif
	    buf->b_p_fp = empty_option;
#nếu defined(FEAT_EVAL)
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
#nếu defined(FEAT_EVAL)
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
#nếu defined(FEAT_BEVAL) && defined(FEAT_EVAL)
	    buf->b_p_bexpr = empty_option;
#endif
#nếu defined(FEAT_CRYPT)
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
	    nếu (dont_do_help)
	    {
		buf->b_p_isk = save_p_isk;
#ifdef FEAT_VARTABS
		nếu (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (void)tabstop_set(p_vts, &buf->b_p_vts_array);
		không_thì
		    buf->b_p_vts_array = NULL;
#endif
	    }
	    không_thì
	    {
		buf->b_p_isk = vim_strsave(p_isk);
		COPY_OPT_SCTX(buf, BV_ISK);
		did_isk = TRUE;
		buf->b_p_ts = p_ts;
		COPY_OPT_SCTX(buf, BV_TS);
#ifdef FEAT_VARTABS
		buf->b_p_vts = vim_strsave(p_vts);
		COPY_OPT_SCTX(buf, BV_VTS);
		nếu (p_vts && *p_vts != NUL && !buf->b_p_vts_array)
		    (void)tabstop_set(p_vts, &buf->b_p_vts_array);
		không_thì
		    buf->b_p_vts_array = NULL;
#endif
		buf->b_help = false;
		nếu (buf->b_p_bt[0] == 'h')
		    clear_string_option(&buf->b_p_bt);
		buf->b_p_ma = p_ma;
		COPY_OPT_SCTX(buf, BV_MA);
	    }
	}

	// When the options should be copied (ignoring BCO_ALWAYS), set the
	// flag that indicates that the options have been initialized.
	nếu (should_copy)
	    buf->b_p_initialized = true;
    }

    check_buf_options(buf);	    // make sure we don't have NULLs
    nếu (did_isk)
	(void)buf_init_chartab(buf, FALSE);
}

/*
 * Reset the 'modifiable' option and its default value.
 */
    void
reset_modifiable(void)
{
    số_nguyên		opt_idx;

    curbuf->b_p_ma = FALSE;
    p_ma = FALSE;
    opt_idx = findoption((char_u *)"ma");
    nếu (opt_idx >= 0)
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

static số_nguyên expand_option_idx = -1;
static số_nguyên expand_option_start_col = 0;
static char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};
static số_nguyên expand_option_flags = 0;
static số_nguyên expand_option_append = FALSE;

    void
set_context_in_set_cmd(
    expand_T	*xp,
    char_u	*arg,
    số_nguyên		opt_flags)	// OPT_GLOBAL and/or OPT_LOCAL
{
    số_nguyên		nextchar;
    long_u	flags = 0;	// init for GCC
    số_nguyên		opt_idx = 0;	// init for GCC
    char_u	*p;
    char_u	*s;
    số_nguyên		is_term_option = FALSE;
    số_nguyên		key;
    char_u	*argend;

    expand_option_flags = opt_flags;

    xp->xp_context = EXPAND_SETTINGS;
    nếu (*arg == NUL)
    {
	xp->xp_pattern = arg;
	trả_về;
    }
    argend = arg + STRLEN(arg);
    p = argend - 1;
    nếu (*p == ' ' && *(p - 1) != '\\')
    {
	xp->xp_pattern = p + 1;
	trả_về;
    }
    trong_khi (p > arg)
    {
	s = p;
	// count number of backslashes before ' ' or ','
	nếu (*p == ' ' || *p == ',')
	{
	    trong_khi (s > arg && *(s - 1) == '\\')
		--s;
	}
	// break at a space with an even number of backslashes
	nếu (*p == ' ' && ((p - s) & 1) == 0)
	{
	    ++p;
	    break;
	}
	--p;
    }
    nếu (STRNCMP(p, "no", 2) == 0 && STRNCMP(p, "novice", 6) != 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_NO;
	p += 2;
    }
    không_thì nếu (STRNCMP(p, "inv", 3) == 0)
    {
	xp->xp_context = EXPAND_BOOL_SETTINGS;
	xp->xp_prefix = XP_PREFIX_INV;
	p += 3;
    }
    xp->xp_pattern = arg = p;
    nếu (*arg == '<')
    {
	trong_khi (*p != '>')
	    nếu (*p++ == NUL)	    // expand terminal option name
		trả_về;
	key = get_special_key_code(arg + 1);
	nếu (key == 0)		    // unknown name
	{
	    xp->xp_context = EXPAND_NOTHING;
	    trả_về;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(key);
	expand_option_name[3] = KEY2TERMCAP1(key);
    }
    không_thì
    {
	nếu (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    nếu (*p != NUL)
		++p;
	    nếu (*p == NUL)
		trả_về;		// expand option name
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	không_thì
	{
	    // Allow * wildcard
	    trong_khi (ASCII_ISALNUM(*p) || *p == '_' || *p == '*')
		p++;
	    nếu (*p == NUL)
		trả_về;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    nếu (opt_idx == -1 || options[opt_idx].var == NULL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		trả_về;
	    }
	    flags = options[opt_idx].flags;
	    nếu (flags & P_BOOL)
	    {
		xp->xp_context = EXPAND_NOTHING;
		trả_về;
	    }
	}
    }
    // handle "-=" and "+="
    expand_option_append = FALSE;
    số_nguyên expand_option_subtract = FALSE;
    nếu ((nextchar == '-' || nextchar == '+' || nextchar == '^') && p[1] == '=')
    {
	nếu (nextchar == '-')
	    expand_option_subtract = TRUE;
	nếu (nextchar == '+' || nextchar == '^')
	    expand_option_append = TRUE;
	++p;
	nextchar = '=';
    }
    nếu ((nextchar != '=' && nextchar != ':')
				    || xp->xp_context == EXPAND_BOOL_SETTINGS)
    {
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	trả_về;
    }

    // Below are for handling expanding a specific option's value after the '='
    // or ':'

    nếu (is_term_option)
	expand_option_idx = -1;
    không_thì
	expand_option_idx = opt_idx;

    nếu (!is_term_option)
    {
	nếu (options[opt_idx].flags & P_NO_CMD_EXPAND)
	{
	    xp->xp_context=EXPAND_UNSUCCESSFUL;
	    trả_về;
	}
    }

    xp->xp_pattern = p + 1;
    expand_option_start_col = (số_nguyên)(p + 1 - xp->xp_line);

    // Certain options currently have special case handling to reuse the
    // expansion logic with other commands.
#ifdef FEAT_SYN_HL
    nếu (options[opt_idx].var == (char_u *)&p_syn)
    {
	xp->xp_context = EXPAND_OWNSYNTAX;
	trả_về;
    }
#endif
    nếu (options[opt_idx].var == (char_u *)&p_ft)
    {
	xp->xp_context = EXPAND_FILETYPE;
	trả_về;
    }
#ifdef FEAT_KEYMAP
    nếu (options[opt_idx].var == (char_u *)&p_keymap)
    {
	xp->xp_context = EXPAND_KEYMAP;
	trả_về;
    }
#endif

    // Now pick. If the option has a custom expander, use that. Otherwise, just
    // fill with the existing option value.
    nếu (expand_option_subtract)
    {
	xp->xp_context = EXPAND_SETTING_SUBTRACT;
	trả_về;
    }
    không_thì nếu (expand_option_idx >= 0 &&
	    options[expand_option_idx].opt_expand_cb != NULL)
    {
	xp->xp_context = EXPAND_STRING_SETTING;
    }
    không_thì nếu (*xp->xp_pattern == NUL)
    {
	xp->xp_context = EXPAND_OLD_SETTING;
	trả_về;
    }
    không_thì
	xp->xp_context = EXPAND_NOTHING;

    nếu (is_term_option || (flags & P_NUM))
	trả_về;

    // Only string options below

    // Options that have P_EXPAND are considered to all use file/dir expansion.
    nếu (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	nếu (p == (char_u *)&p_bdir
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
	    nếu (p == (char_u *)&p_path || p == (char_u *)&p_cdpath)
		xp->xp_backslash = XP_BS_THREE;
	    không_thì
		xp->xp_backslash = XP_BS_ONE;
	}
	không_thì
	{
	    xp->xp_context = EXPAND_FILES;
	    // for 'tags' need three backslashes for a space
	    nếu (p == (char_u *)&p_tags)
		xp->xp_backslash = XP_BS_THREE;
	    không_thì
		xp->xp_backslash = XP_BS_ONE;
	}
	nếu (flags & P_COMMA)
	    xp->xp_backslash |= XP_BS_COMMA;
    }

    // For an option that is a list of file names, or comma/colon-separated
    // values, split it by the delimiter and find the start of the current
    // pattern, while accounting for backslash-escaped space/commas/colons.
    // Triple-backslashed escaped file names (e.g. 'path') can also be
    // delimited by space.
    nếu ((flags & P_EXPAND) || (flags & P_COMMA) || (flags & P_COLON))
    {
	cho (p = argend - 1; p >= xp->xp_pattern; --p)
	{
	    // count number of backslashes before ' ' or ',' or ':'
	    nếu (*p == ' ' || *p == ',' ||
		    (*p == ':' && (flags & P_COLON)))
	    {
		s = p;
		trong_khi (s > xp->xp_pattern && *(s - 1) == '\\')
		    --s;
		nếu ((*p == ' ' && ((xp->xp_backslash & XP_BS_THREE) && (p - s) < 3))
#nếu defined(BACKSLASH_IN_FILENAME)
			|| (*p == ',' && (flags & P_COMMA) && (p - s) < 1)
#không_thì
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
    nếu (flags & P_FLAGLIST)
	xp->xp_pattern = argend;

    // Some options can either be using file/dir expansions, or custom value
    // expansion depending on what the user typed. Unfortunately we have to
    // manually handle it here to make sure we have the correct xp_context set.
#ifdef FEAT_SPELL
    nếu (options[opt_idx].var == (char_u *)&p_sps)
    {
	nếu (STRNCMP(xp->xp_pattern, "file:", 5) == 0)
	{
	    xp->xp_pattern += 5;
	    trả_về;
	}
	không_thì nếu (options[expand_option_idx].opt_expand_cb != NULL)
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
    static số_nguyên
match_str(
	char_u		*str,
	regmatch_T	*regmatch,
	char_u		**matches,
	số_nguyên		idx,
	số_nguyên		test_only,
	số_nguyên		fuzzy,
	char_u		*fuzzystr,
	fuzmatch_str_T  *fuzmatch)
{
    nếu (!fuzzy)
    {
	nếu (vim_regexec(regmatch, str, (colnr_T)0))
	{
	    nếu (!test_only)
		matches[idx] = vim_strsave(str);
	    trả_về TRUE;
	}
    }
    không_thì
    {
	số_nguyên score;

	score = fuzzy_match_str(str, fuzzystr);
	nếu (score != FUZZY_SCORE_NONE)
	{
	    nếu (!test_only)
	    {
		fuzmatch[idx].idx = idx;
		fuzmatch[idx].str = vim_strsave(str);
		fuzmatch[idx].score = score;
	    }

	    trả_về TRUE;
	}
    }

    trả_về FALSE;
}

    số_nguyên
ExpandSettings(
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	*fuzzystr,
    số_nguyên		*numMatches,
    char_u	***matches,
    số_nguyên		can_fuzzy)
{
    số_nguyên		num_normal = 0;	    // Nr of matching non-term-code settings
    số_nguyên		num_term = 0;	    // Nr of matching terminal code settings
    số_nguyên		opt_idx;
    số_nguyên		match;
    số_nguyên		count = 0;
    char_u	*str;
    số_nguyên		loop;
    số_nguyên		is_term_opt;
    char_u	name_buf[MAX_KEY_NAME_LEN];
    static char *(names[]) = {"all", "termcap"};
    số_nguyên		ic = regmatch->rm_ic;	// remember the ignore-case flag
    số_nguyên		fuzzy;
    fuzmatch_str_T  *fuzmatch = NULL;

    fuzzy = can_fuzzy && cmdline_fuzzy_complete(fuzzystr);

    // do this loop twice:
    // loop == 0: count the number of matching options
    // loop == 1: copy the matching options into allocated memory
    cho (loop = 0; loop <= 1; ++loop)
    {
	regmatch->rm_ic = ic;
	nếu (xp->xp_context != EXPAND_BOOL_SETTINGS)
	{
	    cho (match = 0; match < (số_nguyên)ARRAY_LENGTH(names); ++match)
	    {
		nếu (match_str((char_u *)names[match], regmatch, *matches,
			    count, (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    nếu (loop == 0)
			num_normal++;
		    không_thì
			count++;
		}
	    }
	}
	cho (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != NULL;
								    opt_idx++)
	{
	    nếu (options[opt_idx].var == NULL)
		continue;
	    nếu (xp->xp_context == EXPAND_BOOL_SETTINGS
		    && !(options[opt_idx].flags & P_BOOL))
		continue;
	    is_term_opt = istermoption_idx(opt_idx);
	    nếu (is_term_opt && num_normal > 0)
		continue;

	    nếu (match_str(str, regmatch, *matches, count, (loop == 0),
						fuzzy, fuzzystr, fuzmatch))
	    {
		nếu (loop == 0)
		{
		    nếu (is_term_opt)
			num_term++;
		    không_thì
			num_normal++;
		}
		không_thì
		    count++;
	    }
	    không_thì nếu (!fuzzy && options[opt_idx].shortname != NULL
			&& vim_regexec(regmatch,
			     (char_u *)options[opt_idx].shortname, (colnr_T)0))
	    {
		// Compare against the abbreviated option name (for regular
		// expression match). Fuzzy matching (previous if) already
		// matches against both the expanded and abbreviated names.
		nếu (loop == 0)
		{
		    nếu (is_term_opt)
			num_term++;
		    không_thì
			num_normal++;
		}
		không_thì
		    (*matches)[count++] = vim_strsave(str);
	    }
	    không_thì nếu (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;

		nếu (match_str(name_buf, regmatch, *matches, count, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    nếu (loop == 0)
			num_term++;
		    không_thì
			count++;
		}
	    }
	}

	// Check terminal key codes, these are not in the option table
	nếu (xp->xp_context != EXPAND_BOOL_SETTINGS  && num_normal == 0)
	{
	    cho (opt_idx = 0; (str = get_termcode(opt_idx)) != NULL; opt_idx++)
	    {
		nếu (!SAFE_isprint(str[0]) || !SAFE_isprint(str[1]))
		    continue;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		nếu (match_str(name_buf, regmatch, *matches, count,
				       (loop == 0), fuzzy, fuzzystr, fuzmatch))
		{
		    nếu (loop == 0)
			num_term++;
		    không_thì
			count++;
		}
		không_thì
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    nếu (match_str(name_buf, regmatch, *matches, count,
					    (loop == 0), fuzzy, fuzzystr,
					    fuzmatch))
		    {
			nếu (loop == 0)
			    num_term++;
			không_thì
			    count++;
		    }
		}
	    }

	    // Check special key names.
	    regmatch->rm_ic = TRUE;		// ignore case here
	    cho (opt_idx = 0; (str = get_key_name(opt_idx)) != NULL; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		nếu (match_str(name_buf, regmatch, *matches, count, (loop == 0),
			    fuzzy, fuzzystr, fuzmatch))
		{
		    nếu (loop == 0)
			num_term++;
		    không_thì
			count++;
		}
	    }
	}
	nếu (loop == 0)
	{
	    nếu (num_normal > 0)
		*numMatches = num_normal;
	    không_thì nếu (num_term > 0)
		*numMatches = num_term;
	    không_thì
		trả_về OK;
	    nếu (!fuzzy)
	    {
		*matches = ALLOC_MULT(char_u *, *numMatches);
		nếu (*matches == NULL)
		{
		    *matches = (char_u **)"";
		    trả_về FAIL;
		}
	    }
	    không_thì
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, *numMatches);
		nếu (fuzmatch == NULL)
		{
		    *matches = (char_u **)"";
		    trả_về FAIL;
		}
	    }
	}
    }

    nếu (fuzzy &&
	fuzzymatches_to_strmatches(fuzmatch, matches, count, FALSE) == FAIL)
	trả_về FAIL;

    trả_về OK;
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
    nếu (buf == NULL)
	trả_về NULL;

#ifdef BACKSLASH_IN_FILENAME
    // For MS-Windows et al. we don't double backslashes at the start and
    // before a file name character.
    // The reverse is found at stropt_copy_value().
    cho (var = buf; *var != NUL; MB_PTR_ADV(var))
	nếu (var[0] == '\\' && var[1] == '\\'
		&& expand_option_idx >= 0
		&& (options[expand_option_idx].flags & P_EXPAND)
		&& vim_isfilec(var[2])
		&& (var[2] != '\\' || (var == buf && var[4] != '\\')))
	    STRMOVE(var, var + 1);
#endif
    trả_về buf;
}

/*
 * Expansion handler for :set= when we just want to fill in with the existing value.
 */
    số_nguyên
ExpandOldSetting(số_nguyên *numMatches, char_u ***matches)
{
    char_u  *var = NULL;	// init for GCC
    char_u  *buf;

    *numMatches = 0;
    *matches = ALLOC_MULT(char_u *, 1);
    nếu (*matches == NULL)
	trả_về FAIL;

    // For a terminal key code expand_option_idx is < 0.
    nếu (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	nếu (var == NULL)
	    expand_option_idx = findoption(expand_option_name);
    }

    nếu (expand_option_idx >= 0)
    {
	// put string of option value in NameBuff
	option_value2string(&options[expand_option_idx], expand_option_flags);
	var = NameBuff;
    }
    không_thì nếu (var == NULL)
	var = (char_u *)"";

    buf = escape_option_str_cmdline(var);
    nếu (buf == NULL)
    {
	VIM_CLEAR(*matches);
	trả_về FAIL;
    }

    (*matches)[0] = buf;
    *numMatches = 1;
    trả_về OK;
}

/*
 * Expansion handler for :set=/:set+= when the option has a custom expansion handler.
 */
    số_nguyên
ExpandStringSetting(
    expand_T	*xp,
    regmatch_T	*regmatch,
    số_nguyên		*numMatches,
    char_u	***matches)
{
    char_u  *var = NULL;	// init for GCC
    char_u  *buf;

    nếu (expand_option_idx < 0 ||
	    options[expand_option_idx].opt_expand_cb == NULL)
    {
	// Not supposed to reach this. This function is only for options with
	// custom expansion callbacks.
	trả_về FAIL;
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
    nếu (buf == NULL)
	trả_về FAIL;

    args.oe_opt_value = buf;

    số_nguyên num_ret = options[expand_option_idx].opt_expand_cb(&args, numMatches, matches);

    vim_free(buf);
    trả_về num_ret;
}

/*
 * Expansion handler for :set-=
 */
    số_nguyên
ExpandSettingSubtract(
    expand_T	*xp,
    regmatch_T	*regmatch,
    số_nguyên		*numMatches,
    char_u	***matches)
{
    nếu (expand_option_idx < 0)
	// term option
	trả_về ExpandOldSetting(numMatches, matches);

    char_u *option_val = *(char_u**)get_option_varp_scope(
	expand_option_idx, expand_option_flags);

    long_u option_flags =  options[expand_option_idx].flags;

    nếu (option_flags & P_NUM)
	trả_về ExpandOldSetting(numMatches, matches);
    không_thì nếu (option_flags & P_COMMA)
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

	nếu (*option_val == NUL)
	    trả_về FAIL;

	// Make a copy as we need to inject null characters destructively.
	option_copy = vim_strsave(option_val);
	nếu (option_copy == NULL)
	    trả_về FAIL;
	next_val = option_copy;

	ga_init2(&ga, sizeof(char_u *), 10);

	do
	{
	    item = next_val;
	    comma = vim_strchr(next_val, ',');
	    trong_khi (comma != NULL && comma != next_val && *(comma - 1) == '\\')
	    {
		// "\," is interpreted as a literal comma rather than option
		// separator when reading options in copy_option_part(). Skip
		// it.
		comma = vim_strchr(comma + 1, ',');
	    }
	    nếu (comma != NULL)
	    {
		*comma = NUL; // null-terminate this value, required by later functions
		next_val = comma + 1;
	    }
	    không_thì
		next_val = NULL;

	    nếu (*item == NUL)
		// empty value, don't add to list
		continue;

	    nếu (!vim_regexec(regmatch, item, (colnr_T)0))
		continue;

	    char_u *buf = escape_option_str_cmdline(item);
	    nếu (buf == NULL)
	    {
		vim_free(option_copy);
		ga_clear_strings(&ga);
		trả_về FAIL;
	    }
	    nếu (ga_add_string(&ga, buf) != OK)
	    {
		vim_free(buf);
		break;
	    }
	} trong_khi (next_val != NULL);

	vim_free(option_copy);

	*matches = ga.ga_data;
	*numMatches = ga.ga_len;
	trả_về OK;
    }
    không_thì nếu (option_flags & P_FLAGLIST)
    {
	// Only present the flags that are set on the option as the other flags
	// are not meaningful to do set-= on.

	nếu (*xp->xp_pattern != NUL)
	{
	    // Don't suggest anything if cmdline is non-empty. Vim's set-=
	    // behavior requires consecutive strings and it's usually
	    // unintuitive to users if they try to subtract multiple flags at
	    // once.
	    trả_về FAIL;
	}

	size_t num_flags = STRLEN(option_val);
	nếu (num_flags == 0)
	    trả_về FAIL;

	*matches = ALLOC_MULT(char_u *, num_flags + 1);
	nếu (*matches == NULL)
	    trả_về FAIL;

	số_nguyên count = 0;
	char_u *p;

	p = vim_strnsave(option_val, num_flags);
	nếu (p == NULL)
	{
	    VIM_CLEAR(*matches);
	    trả_về FAIL;
	}
	(*matches)[count++] = p;

	nếu (num_flags > 1)
	{
	    // If more than one flags, split the flags up and expose each
	    // character as individual choice.
	    cho (char_u *flag = option_val; *flag != NUL; flag++)
	    {
		p = vim_strnsave(flag, 1);
		nếu (p == NULL)
		    break;
		(*matches)[count++] = p;
	    }
	}

	*numMatches = count;
	trả_về OK;
    }

    trả_về ExpandOldSetting(numMatches, matches);
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    static void
option_value2string(
    struct vimoption	*opp,
    số_nguyên			scope)	// OPT_GLOBAL and/or OPT_LOCAL
{
    char_u	*varp;

    varp = get_varp_scope(opp, scope);

    nếu (opp->flags & P_NUM)
    {
	long wc = 0;

	nếu (wc_use_keyname(varp, &wc))
	    STRCPY(NameBuff, get_special_key_name((số_nguyên)wc, 0));
	không_thì nếu (wc != 0)
	    STRCPY(NameBuff, transchar((số_nguyên)wc));
	không_thì
	    sprintf((char *)NameBuff, "%ld", *(long *)varp);
    }
    không_thì    // P_STRING
    {
	varp = *(char_u **)(varp);
	nếu (varp == NULL)		    // just in case
	    NameBuff[0] = NUL;
#ifdef FEAT_CRYPT
	// don't show the actual value of 'key', only that it's set
	không_thì nếu (opp->var == (char_u *)&p_key && *varp)
	    STRCPY(NameBuff, "*****");
#endif
	không_thì nếu (opp->flags & P_EXPAND)
	    home_replace(NULL, varp, NameBuff, MAXPATHL, FALSE);
	// Translate 'pastetoggle' into special key names
	không_thì nếu ((char_u **)opp->var == &p_pt)
	    str2specialbuf(p_pt, NameBuff, MAXPATHL);
	không_thì
	    vim_strncpy(NameBuff, varp, MAXPATHL - 1);
    }
}

/*
 * Return TRUE if "varp" points to 'wildchar' or 'wildcharm' and it can be
 * printed as a keyname.
 * "*wcp" is set to the value of the option if it's 'wildchar' or 'wildcharm'.
 */
    static số_nguyên
wc_use_keyname(char_u *varp, long *wcp)
{
    nếu (((long *)varp == &p_wc) || ((long *)varp == &p_wcm))
    {
	*wcp = *(long *)varp;
	nếu (IS_SPECIAL(*wcp) || find_special_key_in_table((số_nguyên)*wcp) >= 0)
	    trả_về TRUE;
    }
    trả_về FALSE;
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    số_nguyên
shortmess(số_nguyên x)
{
    trả_về p_shm != NULL &&
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
    số_nguyên		opt_idx;
    số_nguyên		dofree = FALSE;
    char_u	*p;

    nếu (!option_was_set((char_u *)"cp"))
    {
	p_cp = FALSE;
	cho (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	    nếu (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, OPT_FREE, FALSE);
	didset_options();
	didset_options2();
    }

    nếu (fname != NULL)
    {
	p = vim_getenv(envname, &dofree);
	nếu (p == NULL)
	{
	    // Set $MYVIMRC to the first vimrc file found.
	    p = FullName_save(fname, FALSE);
	    nếu (p != NULL)
	    {
		vim_setenv(envname, p);
		vim_free(p);
	    }
	    // Set $MYVIMDIR
	    export_myvimdir();
	}
	không_thì nếu (dofree)
	    vim_free(p);
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    void
change_compatible(số_nguyên on)
{
    số_nguyên	    opt_idx;

    nếu (p_cp != on)
    {
	p_cp = on;
	compatible_set();
    }
    opt_idx = findoption((char_u *)"cp");
    nếu (opt_idx >= 0)
	options[opt_idx].flags |= P_WAS_SET;
}

/*
 * Return TRUE when option "name" has been set.
 * Only works correctly for global options.
 */
    số_nguyên
option_was_set(char_u *name)
{
    số_nguyên idx;

    idx = findoption(name);
    nếu (idx < 0)	// unknown option
	trả_về FALSE;
    nếu (options[idx].flags & P_WAS_SET)
	trả_về TRUE;
    trả_về FALSE;
}

/*
 * Reset the flag indicating option "name" was set.
 */
    số_nguyên
reset_option_was_set(char_u *name)
{
    số_nguyên idx = findoption(name);

    nếu (idx < 0)
	trả_về FAIL;

    options[idx].flags &= ~P_WAS_SET;
    trả_về OK;
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
    số_nguyên	    opt_idx;

    cho (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
	nếu (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    set_option_default(opt_idx, OPT_FREE, p_cp);
    didset_options();
    didset_options2();
}

/*
 * Check if backspacing over something is allowed.
 */
    số_nguyên
can_bs(
    số_nguyên		what)	    // BS_INDENT, BS_EOL, BS_START or BS_NOSTOP
{
#ifdef FEAT_JOB_CHANNEL
    nếu (what == BS_START && bt_prompt(curbuf))
	trả_về FALSE;
#endif
    switch (*p_bs)
    {
	case '3':       trả_về TRUE;
	case '2':	trả_về (what != BS_NOSTOP);
	case '1':	trả_về (what != BS_START);
	case '0':	trả_về FALSE;
    }
    trả_về vim_strchr(p_bs, what) != NULL;
}

/*
 * Return the effective 'scrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloff_value(void)
{
    trả_về curwin->w_p_so < 0 ? p_so : curwin->w_p_so;
}

/*
 * Return the effective 'scrolloffpad' value for the current window, using the
 * global value when appropriate.
 */
    long
get_scrolloffpad_value(void)
{
    trả_về curwin->w_p_sop == -1 ? p_sop : curwin->w_p_sop;
}

/*
 * Return the effective 'sidescrolloff' value for the current window, using the
 * global value when appropriate.
 */
    long
get_sidescrolloff_value(void)
{
    trả_về curwin->w_p_siso < 0 ? p_siso : curwin->w_p_siso;
}

/*
 * Get the local or global value of 'backupcopy' flags.
 */
    unsigned số_nguyên
get_bkc_flags(buf_T *buf)
{
    trả_về buf->b_bkc_flags ? buf->b_bkc_flags : bkc_flags;
}

#nếu defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'formatlistpat'.
 */
    char_u *
get_flp_value(buf_T *buf)
{
    nếu (buf->b_p_flp == NULL || *buf->b_p_flp == NUL)
	trả_về p_flp;
    trả_về buf->b_p_flp;
}
#endif

/*
 * Get the local or global value of 'virtualedit' flags.
 */
    unsigned số_nguyên
get_ve_flags(void)
{
    trả_về (curwin->w_ve_flags ? curwin->w_ve_flags : ve_flags)
	    & ~(VE_NONE | VE_NONEU);
}

#nếu defined(FEAT_LINEBREAK)
/*
 * Get the local or global value of 'showbreak'.
 */
    char_u *
get_showbreak_value(win_T *win)
{
    nếu (win->w_p_sbr == NULL || *win->w_p_sbr == NUL)
	trả_về p_sbr;
    nếu (STRCMP(win->w_p_sbr, "NONE") == 0)
	trả_về empty_option;
    trả_về win->w_p_sbr;
}
#endif

#nếu defined(FEAT_EVAL)
/*
 * Get window or buffer local options.
 */
    dict_T *
get_winbuf_options(số_nguyên bufopt)
{
    dict_T	*d;
    số_nguyên		opt_idx;

    d = dict_alloc();
    nếu (d == NULL)
	trả_về NULL;

    cho (opt_idx = 0; !istermoption_idx(opt_idx); opt_idx++)
    {
	struct vimoption *opt = &options[opt_idx];

	nếu ((bufopt && (opt->indir & PV_BUF))
					 || (!bufopt && (opt->indir & PV_WIN)))
	{
	    char_u *varp = get_varp(opt);

	    nếu (varp != NULL)
	    {
		nếu (opt->flags & P_STRING)
		    dict_add_string(d, opt->fullname, *(char_u **)varp);
		không_thì nếu (opt->flags & P_NUM)
		    dict_add_number(d, opt->fullname, *(long *)varp);
		không_thì
		    dict_add_number(d, opt->fullname, *(số_nguyên *)varp);
	    }
	}
    }

    trả_về d;
}
#endif

#nếu defined(FEAT_SYN_HL)
/*
 * This is called when 'culopt' is changed
 */
    số_nguyên
fill_culopt_flags(char_u *val, win_T *wp)
{
    char_u	*p;
    char_u	culopt_flags_new = 0;

    nếu (val == NULL)
	p = wp->w_p_culopt;
    không_thì
	p = val;
    trong_khi (*p != NUL)
    {
	// Note: Keep this in sync with p_culopt_values.
	nếu (STRNCMP(p, "line", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_LINE;
	}
	không_thì nếu (STRNCMP(p, "both", 4) == 0)
	{
	    p += 4;
	    culopt_flags_new |= CULOPT_LINE | CULOPT_NBR;
	}
	không_thì nếu (STRNCMP(p, "number", 6) == 0)
	{
	    p += 6;
	    culopt_flags_new |= CULOPT_NBR;
	}
	không_thì nếu (STRNCMP(p, "screenline", 10) == 0)
	{
	    p += 10;
	    culopt_flags_new |= CULOPT_SCRLINE;
	}

	nếu (*p != ',' && *p != NUL)
	    trả_về FAIL;
	nếu (*p == ',')
	    ++p;
    }

    // Can't have both "line" and "screenline".
    nếu ((culopt_flags_new & CULOPT_LINE) && (culopt_flags_new & CULOPT_SCRLINE))
	trả_về FAIL;
    wp->w_p_culopt_flags = culopt_flags_new;

    trả_về OK;
}
#endif

/*
 * Get the value of 'magic' adjusted for Vim9 script.
 */
    số_nguyên
magic_isset(void)
{
    switch (magic_overruled)
    {
	case OPTION_MAGIC_ON:      trả_về TRUE;
	case OPTION_MAGIC_OFF:     trả_về FALSE;
	case OPTION_MAGIC_NOT_SET: break;
    }
#ifdef FEAT_EVAL
    nếu (in_vim9script())
	trả_về TRUE;
#endif
    trả_về p_magic;
}

/*
 * Set the callback function value for an option that accepts a function name,
 * lambda, et al. (e.g. 'operatorfunc', 'tagfunc', etc.)
 * Returns OK if the option is successfully set to a function, otherwise
 * returns FAIL.
 */
    số_nguyên
option_set_callback_func(char_u *optval UNUSED, callback_T *optcb UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	*tv;
    callback_T	cb;
    số_nguyên		funcname = FALSE;

    nếu (optval == NULL || *optval == NUL)
    {
	free_callback(optcb);
	trả_về OK;
    }

    nếu (*optval == '{' || (in_vim9script() && *optval == '(')
	    || (STRNCMP(optval, "function(", 9) == 0)
	    || (STRNCMP(optval, "funcref(", 8) == 0))
	// Lambda expression or a funcref
	tv = eval_expr(optval, NULL);
    không_thì
    {
	// treat everything else as a function name string
	tv = alloc_string_tv(vim_strsave(optval));
	funcname = TRUE;
    }
    nếu (tv == NULL)
	trả_về FAIL;

    cb = get_callback(tv);
    nếu (cb.cb_name == NULL || *cb.cb_name == NUL)
    {
	free_tv(tv);
	trả_về FAIL;
    }

    free_callback(optcb);
    set_callback(optcb, &cb);
    free_tv(tv);

    char_u  *dot = NULL;
    nếu (in_vim9script() && funcname
				&& ((dot = vim_strchr(optval, '.')) != NULL))
    {
	nếu (optcb->cb_free_name)
	    vim_free(optcb->cb_name);

	imported_T *import = find_imported(optval, dot - optval, FALSE);
	nếu (import != NULL && SCRIPT_ID_VALID(import->imp_sid)
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
	không_thì
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

    trả_về OK;
#không_thì
    trả_về FAIL;
#endif
}

#nếu defined(FEAT_TABPANEL)
/*
 * Process the new 'showtabpanel' option value.
 */
    char *
did_set_showtabpanel(optset_T *args UNUSED)
{
    shell_new_columns();
    trả_về NULL;
}
#endif

#nếu defined(FEAT_EVAL)
    static void
didset_options_sctx(số_nguyên opt_flags, char **buf)
{
	cho (số_nguyên i = 0; ; ++i)
	{
	    nếu (buf[i] == NULL)
		break;

	    số_nguyên idx = findoption((char_u *)buf[i]);
	    nếu (idx >= 0)
		set_option_sctx_idx(idx, opt_flags, current_sctx);
	}
}
#endif
