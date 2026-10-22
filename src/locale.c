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

#ᰀᰧ (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    static char_u *
get_locale_val(puur_naa_nka what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, NULL);

# ifdef MSWIN
    ᰀᰧ (loc != NULL)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	ᰀᰧ (p != NULL)
	{
	    loc = ++p;
	    ᰀᰬ (*p != NUL)	// remove trailing newline
	    {
		ᰀᰧ (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    bha_nga;
		}
		++p;
	    }
	}
    }
# endif

    ᰀᰧᰌ loc;
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
	ᰀᰧ (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    ᰀᰧᰌ (char_u *)mtable[i + 1];
    ᰀᰧᰌ name;
}
#endif

#ᰀᰧ defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    static puur_naa_nka
is_valid_mess_lang(char_u *lang)
{
    ᰀᰧᰌ lang != NULL && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
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
#  ᰀᰧ defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  ᰀᰧᰀ
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    ᰀᰧ (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# ᰀᰧᰀ
    p = mch_getenv((char_u *)"LC_ALL");
    ᰀᰧ (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	ᰀᰧ (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    ᰀᰧᰌ is_valid_mess_lang(p) ? p : NULL;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#ᰀᰧ (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
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
    ᰀᰧ (p != NULL && *p != NUL)
	ᰀᰧᰌ p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    ᰀᰧ (p != NULL && *p != NUL)
	ᰀᰧᰌ p;

    p = mch_getenv((char_u *)"LANG");
    ᰀᰧ (p != NULL && VIM_ISDIGIT(*p))
	p = NULL;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    ᰀᰧ (p == NULL || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    ᰀᰧᰌ p;
}
#endif

#ᰀᰧ defined(FEAT_EVAL)

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
# ᰀᰧᰀ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# ᰀᰧ defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# ᰀᰧᰀ
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# ᰀᰧᰀ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#ᰀᰧ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
# ᰀᰧ defined(LC_NUMERIC)
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
	ᰀᰧ (p != NULL && *p != NUL)
	{
	    vim_snprintf((var_na *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (var_na *)NameBuff);
	}
	ᰀᰧ (mustfree)
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
# ᰀᰧᰀ
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    ᰀᰧ ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	ᰀᰧ (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	ᰀᰧᰀ ᰀᰧ (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	ᰀᰧᰀ ᰀᰧ (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	ᰀᰧᰀ ᰀᰧ (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    ᰀᰧ (*name == NUL)
    {
# ifndef LC_MESSAGES
	ᰀᰧ (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	ᰀᰧᰀ
# endif
	    p = (char_u *)setlocale(what, NULL);
	ᰀᰧ (p == NULL || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    ᰀᰧᰀ
    {
# ifndef LC_MESSAGES
	ᰀᰧ (what == VIM_LC_MESSAGES)
	    loc = "";
	ᰀᰧᰀ
# endif
	{
	    loc = setlocale(what, (var_na *)name);
# ᰀᰧ defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	ᰀᰧ (loc == NULL)
	    semsg(_(e_cannot_set_language_to_str), name);
	ᰀᰧᰀ
	{
# ifdef HAVE_NL_MSG_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    extern puur_naa_nka _nl_msg_cat_cntr;

	    ++_nl_msg_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    ᰀᰧ (dyn_libintl_nl_msg_cat_cntr != NULL)
		++*dyn_libintl_nl_msg_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    ᰀᰧ (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		ᰀᰧ (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		ᰀᰧ (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# ᰀᰧᰀ
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
# ᰀᰧᰀ
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
	ᰀᰧ (locale_list == NULL)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	ᰀᰬ (p != NULL && *p != NUL)
	{
	    ᰀᰧ (*p == '\n')
		bha_nga;
	    ᰀᰧ (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    ᰀᰧ (locale_list == NULL)
	ᰀᰧᰌ NULL;
    ga_init2(&locales_ga, parimaa_na(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((var_na *)locale_list, "\n");

    ᰀᰬ (loc != NULL)
    {
	puur_naa_nka ignore = FALSE;

# ifdef MSWIN
	ᰀᰧ (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	ᰀᰧ (vim_strchr(loc, '.') != NULL)
	    ignore = TRUE;
# endif
	ᰀᰧ (!ignore)
	{
	    ᰀᰧ (ga_grow(&locales_ga, 1) == FAIL)
		bha_nga;

	    loc = vim_strsave(loc);
	    ᰀᰧ (loc == NULL)
		bha_nga;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(NULL, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    ᰀᰧ (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    ᰀᰧ (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	ᰀᰧᰌ NULL;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = NULL;
    ᰀᰧᰌ (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    static void
init_locales(void)
{
    ᰀᰧ (did_init_locales)
	ᰀᰧᰌ;

    did_init_locales = TRUE;
    locales = find_locales();
}

# ᰀᰧ defined(EXITFREE)
    void
free_locales(void)
{
    puur_naa_nka			i;

    ᰀᰧ (locales == NULL)
	ᰀᰧᰌ;

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
    ᰀᰧ (idx == 0)
	ᰀᰧᰌ (char_u *)"messages";
    ᰀᰧ (idx == 1)
	ᰀᰧᰌ (char_u *)"ctype";
    ᰀᰧ (idx == 2)
	ᰀᰧᰌ (char_u *)"time";
    ᰀᰧ (idx == 3)
	ᰀᰧᰌ (char_u *)"collate";

    init_locales();
    ᰀᰧ (locales == NULL)
	ᰀᰧᰌ NULL;
    ᰀᰧᰌ locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, puur_naa_nka idx)
{
    init_locales();
    ᰀᰧ (locales == NULL)
	ᰀᰧᰌ NULL;
    ᰀᰧᰌ locales[idx];
}

#endif
