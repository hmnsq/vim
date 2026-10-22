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

#만약 (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    static char_u *
get_locale_val(정수 what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, NULL);

# ifdef MSWIN
    만약 (loc != NULL)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	만약 (p != NULL)
	{
	    loc = ++p;
	    동안 (*p != NUL)	// remove trailing newline
	    {
		만약 (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    break;
		}
		++p;
	    }
	}
    }
# endif

    반환 loc;
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
    정수		i;
    static char *(mtable[]) = {
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

    위해 (i = 0; mtable[i] != NULL; i += 2)
	만약 (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    반환 (char_u *)mtable[i + 1];
    반환 name;
}
#endif

#만약 defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    static 정수
is_valid_mess_lang(char_u *lang)
{
    반환 lang != NULL && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
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
#  만약 defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  아니면
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    만약 (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# 아니면
    p = mch_getenv((char_u *)"LC_ALL");
    만약 (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	만약 (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    반환 is_valid_mess_lang(p) ? p : NULL;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#만약 (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
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
    만약 (p != NULL && *p != NUL)
	반환 p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    만약 (p != NULL && *p != NUL)
	반환 p;

    p = mch_getenv((char_u *)"LANG");
    만약 (p != NULL && VIM_ISDIGIT(*p))
	p = NULL;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    만약 (p == NULL || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    반환 p;
}
#endif

#만약 defined(FEAT_EVAL)

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
# 아니면
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# 만약 defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# 아니면
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# 아니면
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#만약 defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
# 만약 defined(LC_NUMERIC)
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
	정수	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	만약 (p != NULL && *p != NUL)
	{
	    vim_snprintf((char *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (char *)NameBuff);
	}
	만약 (mustfree)
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
    char	*loc;
    char_u	*p;
    char_u	*name;
    정수		what = LC_ALL;
    char	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# 아니면
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    만약 ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	만약 (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	아니면 만약 (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	아니면 만약 (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	아니면 만약 (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    만약 (*name == NUL)
    {
# ifndef LC_MESSAGES
	만약 (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	아니면
# endif
	    p = (char_u *)setlocale(what, NULL);
	만약 (p == NULL || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    아니면
    {
# ifndef LC_MESSAGES
	만약 (what == VIM_LC_MESSAGES)
	    loc = "";
	아니면
# endif
	{
	    loc = setlocale(what, (char *)name);
# 만약 defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	만약 (loc == NULL)
	    semsg(_(e_cannot_set_language_to_str), name);
	아니면
	{
# ifdef HAVE_NL_MSG_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    extern 정수 _nl_msg_cat_cntr;

	    ++_nl_msg_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    만약 (dyn_libintl_nl_msg_cat_cntr != NULL)
		++*dyn_libintl_nl_msg_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    만약 (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		만약 (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		만약 (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# 아니면
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

static 정수	did_init_locales = FALSE;

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
# 아니면
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	정수		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      NULL, options, WILD_ALL);
	ExpandCleanup(&xpc);
	만약 (locale_list == NULL)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	동안 (p != NULL && *p != NUL)
	{
	    만약 (*p == '\n')
		break;
	    만약 (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    만약 (locale_list == NULL)
	반환 NULL;
    ga_init2(&locales_ga, sizeof(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((char *)locale_list, "\n");

    동안 (loc != NULL)
    {
	정수 ignore = FALSE;

# ifdef MSWIN
	만약 (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	만약 (vim_strchr(loc, '.') != NULL)
	    ignore = TRUE;
# endif
	만약 (!ignore)
	{
	    만약 (ga_grow(&locales_ga, 1) == FAIL)
		break;

	    loc = vim_strsave(loc);
	    만약 (loc == NULL)
		break;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(NULL, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    만약 (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    만약 (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	반환 NULL;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = NULL;
    반환 (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    static void
init_locales(void)
{
    만약 (did_init_locales)
	반환;

    did_init_locales = TRUE;
    locales = find_locales();
}

# 만약 defined(EXITFREE)
    void
free_locales(void)
{
    정수			i;

    만약 (locales == NULL)
	반환;

    위해 (i = 0; locales[i] != NULL; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp UNUSED, 정수 idx)
{
    만약 (idx == 0)
	반환 (char_u *)"messages";
    만약 (idx == 1)
	반환 (char_u *)"ctype";
    만약 (idx == 2)
	반환 (char_u *)"time";
    만약 (idx == 3)
	반환 (char_u *)"collate";

    init_locales();
    만약 (locales == NULL)
	반환 NULL;
    반환 locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, 정수 idx)
{
    init_locales();
    만약 (locales == NULL)
	반환 NULL;
    반환 locales[idx];
}

#endif
