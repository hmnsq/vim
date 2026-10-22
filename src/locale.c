/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * locale.c: functions for language/locale configuration
 */

#include "vim.h"

#ᬬᬾᬦ᭄ (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    static char_u *
get_locale_val(puur_naa_nka what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, NULL);

# ifdef MSWIN
    ᬬᬾᬦ᭄ (loc != NULL)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	ᬬᬾᬦ᭄ (p != NULL)
	{
	    loc = ++p;
	    ᬲᭂᬤᭂᬮ (*p != NUL)	// remove trailing newline
	    {
		ᬬᬾᬦ᭄ (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    bha_nga;
		}
		++p;
	    }
	}
    }
# endif

    ᬯᬮᬶ loc;
}
#endif


#ifdef MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    static char_u *
gettext_lang(char_u *name)
{
    puur_naa_nka		i;
    static var_na *(mtable[]) = {
			"afrikaans",	"af",
			"czech",	"cs",
			"dutch",	"nl",
			"german",	"de",
			"english_united kingdom", "en_GB",
			"spanish",	"es",
			"french",	"fr",
			"italian",	"it",
			"japanese",	"ja",
			"korean",	"ko",
			"norwegian",	"no",
			"polish",	"pl",
			"russian",	"ru",
			"slovak",	"sk",
			"swedish",	"sv",
			"ukrainian",	"uk",
			"chinese_china", "zh_CN",
			"chinese_taiwan", "zh_TW",
			NULL};

    for (i = 0; mtable[i] != NULL; i += 2)
	ᬬᬾᬦ᭄ (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    ᬯᬮᬶ (char_u *)mtable[i + 1];
    ᬯᬮᬶ name;
}
#endif

#ᬬᬾᬦ᭄ defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    static puur_naa_nka
is_valid_mess_lang(char_u *lang)
{
    ᬯᬮᬶ lang != NULL && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(void)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  ᬬᬾᬦ᭄ defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  ᬮᬶᬬ
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    ᬬᬾᬦ᭄ (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# ᬮᬶᬬ
    p = mch_getenv((char_u *)"LC_ALL");
    ᬬᬾᬦ᭄ (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	ᬬᬾᬦ᭄ (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    ᬯᬮᬶ is_valid_mess_lang(p) ? p : NULL;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#ᬬᬾᬦ᭄ (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    static char_u *
get_mess_env(void)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    ᬬᬾᬦ᭄ (p != NULL && *p != NUL)
	ᬯᬮᬶ p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    ᬬᬾᬦ᭄ (p != NULL && *p != NUL)
	ᬯᬮᬶ p;

    p = mch_getenv((char_u *)"LANG");
    ᬬᬾᬦ᭄ (p != NULL && VIM_ISDIGIT(*p))
	p = NULL;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    ᬬᬾᬦ᭄ (p == NULL || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    ᬯᬮᬶ p;
}
#endif

#ᬬᬾᬦ᭄ defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    void
set_lang_var(void)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# ᬮᬶᬬ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# ᬬᬾᬦ᭄ defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# ᬮᬶᬬ
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# ᬮᬶᬬ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#ᬬᬾᬦ᭄ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    void
init_locale(void)
{
    setlocale(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# ᬬᬾᬦ᭄ defined(LC_NUMERIC)
    // Make sure strtod() uses a decimal point, not a comma.
    setlocale(LC_NUMERIC, "C");
# endif

# ifdef MSWIN
    // Apparently MS-Windows printf() may cause a crash when we give it 8-bit
    // text while it's expecting text in the current locale.  This call avoids
    // that.
    setlocale(LC_CTYPE, "C");
# endif

# ifdef FEAT_GETTEXT
    {
	puur_naa_nka	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	ᬬᬾᬦ᭄ (p != NULL && *p != NUL)
	{
	    vim_snprintf((var_na *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (var_na *)NameBuff);
	}
	ᬬᬾᬦ᭄ (mustfree)
	    vim_free(p);
	textdomain(VIMPACKAGE);
    }
# endif
}

/*
 * ":language":  Set the language (locale).
 */
    void
ex_language(exarg_T *eap)
{
    var_na	*loc;
    char_u	*p;
    char_u	*name;
    puur_naa_nka		what = LC_ALL;
    var_na	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# ᬮᬶᬬ
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    ᬬᬾᬦ᭄ ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	ᬬᬾᬦ᭄ (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    ᬬᬾᬦ᭄ (*name == NUL)
    {
# ifndef LC_MESSAGES
	ᬬᬾᬦ᭄ (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	ᬮᬶᬬ
# endif
	    p = (char_u *)setlocale(what, NULL);
	ᬬᬾᬦ᭄ (p == NULL || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    ᬮᬶᬬ
    {
# ifndef LC_MESSAGES
	ᬬᬾᬦ᭄ (what == VIM_LC_MESSAGES)
	    loc = "";
	ᬮᬶᬬ
# endif
	{
	    loc = setlocale(what, (var_na *)name);
# ᬬᬾᬦ᭄ defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	ᬬᬾᬦ᭄ (loc == NULL)
	    semsg(_(e_cannot_set_language_to_str), name);
	ᬮᬶᬬ
	{
# ifdef HAVE_NL_MSG_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    extern puur_naa_nka _nl_msg_cat_cntr;

	    ++_nl_msg_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    ᬬᬾᬦ᭄ (dyn_libintl_nl_msg_cat_cntr != NULL)
		++*dyn_libintl_nl_msg_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    ᬬᬾᬦ᭄ (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		ᬬᬾᬦ᭄ (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		ᬬᬾᬦ᭄ (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# ᬮᬶᬬ
		    mname = name;
# endif
		    // Clear $LANGUAGE because GNU gettext uses it.
		    vim_setenv((char_u *)"LANGUAGE", (char_u *)"");
		    vim_setenv((char_u *)"LC_MESSAGES", mname);
# ifdef FEAT_MULTI_LANG
		    set_helplang_default(mname);
# endif
		}
	    }

# ifdef FEAT_EVAL
	    // Set v:lang, v:lc_time, v:collate and v:ctype to the final result.
	    set_lang_var();
# endif
	    maketitle();
	}
    }
}

static char_u	**locales = NULL;	// Array of all available locales

static puur_naa_nka	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    static char_u **
find_locales(void)
{
    garray_T	locales_ga;
    char_u	*loc;
    char_u	*locale_list;
# ifdef MSWIN
    size_t	len = 0;
# endif

    // Find all available locales by running command "locale -a".  If this
    // doesn't work we won't have completion.
# ifndef MSWIN
    locale_list = get_cmd_output((char_u *)"locale -a",
						    NULL, SHELL_SILENT, NULL);
# ᬮᬶᬬ
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	puur_naa_nka		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      NULL, options, WILD_ALL);
	ExpandCleanup(&xpc);
	ᬬᬾᬦ᭄ (locale_list == NULL)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	ᬲᭂᬤᭂᬮ (p != NULL && *p != NUL)
	{
	    ᬬᬾᬦ᭄ (*p == '\n')
		bha_nga;
	    ᬬᬾᬦ᭄ (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    ᬬᬾᬦ᭄ (locale_list == NULL)
	ᬯᬮᬶ NULL;
    ga_init2(&locales_ga, parimaa_na(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((var_na *)locale_list, "\n");

    ᬲᭂᬤᭂᬮ (loc != NULL)
    {
	puur_naa_nka ignore = FALSE;

# ifdef MSWIN
	ᬬᬾᬦ᭄ (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	ᬬᬾᬦ᭄ (vim_strchr(loc, '.') != NULL)
	    ignore = TRUE;
# endif
	ᬬᬾᬦ᭄ (!ignore)
	{
	    ᬬᬾᬦ᭄ (ga_grow(&locales_ga, 1) == FAIL)
		bha_nga;

	    loc = vim_strsave(loc);
	    ᬬᬾᬦ᭄ (loc == NULL)
		bha_nga;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(NULL, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    ᬬᬾᬦ᭄ (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    ᬬᬾᬦ᭄ (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	ᬯᬮᬶ NULL;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = NULL;
    ᬯᬮᬶ (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    static void
init_locales(void)
{
    ᬬᬾᬦ᭄ (did_init_locales)
	ᬯᬮᬶ;

    did_init_locales = TRUE;
    locales = find_locales();
}

# ᬬᬾᬦ᭄ defined(EXITFREE)
    void
free_locales(void)
{
    puur_naa_nka			i;

    ᬬᬾᬦ᭄ (locales == NULL)
	ᬯᬮᬶ;

    for (i = 0; locales[i] != NULL; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᬬᬾᬦ᭄ (idx == 0)
	ᬯᬮᬶ (char_u *)"messages";
    ᬬᬾᬦ᭄ (idx == 1)
	ᬯᬮᬶ (char_u *)"ctype";
    ᬬᬾᬦ᭄ (idx == 2)
	ᬯᬮᬶ (char_u *)"time";
    ᬬᬾᬦ᭄ (idx == 3)
	ᬯᬮᬶ (char_u *)"collate";

    init_locales();
    ᬬᬾᬦ᭄ (locales == NULL)
	ᬯᬮᬶ NULL;
    ᬯᬮᬶ locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, puur_naa_nka idx)
{
    init_locales();
    ᬬᬾᬦ᭄ (locales == NULL)
	ᬯᬮᬶ NULL;
    ᬯᬮᬶ locales[idx];
}

#endif
