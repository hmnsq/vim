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

#अन्तर्भाव "vim.h"

#जर (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    स्थिर char_u *
get_locale_val(पूर्णांक what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)रखो_क्षेत्र(what, रिक्त);

# ifdef MSWIN
    जर (loc != रिक्त)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	जर (p != रिक्त)
	{
	    loc = ++p;
	    पर्यंत (*p != NUL)	// remove trailing newline
	    {
		जर (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    थांबा;
		}
		++p;
	    }
	}
    }
# endif

    परत loc;
}
#endif


#ifdef MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    स्थिर char_u *
gettext_lang(char_u *name)
{
    पूर्णांक		i;
    स्थिर अक्षर *(mtable[]) = {
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
			रिक्त};

    साठी (i = 0; mtable[i] != रिक्त; i += 2)
	जर (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    परत (char_u *)mtable[i + 1];
    परत name;
}
#endif

#जर defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    स्थिर पूर्णांक
is_valid_mess_lang(char_u *lang)
{
    परत lang != रिक्त && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(शून्य)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  जर defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  अन्यथा
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    जर (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# अन्यथा
    p = mch_getenv((char_u *)"LC_ALL");
    जर (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	जर (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    परत is_valid_mess_lang(p) ? p : रिक्त;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#जर (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    स्थिर char_u *
get_mess_env(शून्य)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    जर (p != रिक्त && *p != NUL)
	परत p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    जर (p != रिक्त && *p != NUL)
	परत p;

    p = mch_getenv((char_u *)"LANG");
    जर (p != रिक्त && VIM_ISDIGIT(*p))
	p = रिक्त;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    जर (p == रिक्त || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    परत p;
}
#endif

#जर defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    शून्य
set_lang_var(शून्य)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# अन्यथा
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# जर defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# अन्यथा
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# अन्यथा
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#जर defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    शून्य
init_locale(शून्य)
{
    रखो_क्षेत्र(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# जर defined(LC_NUMERIC)
    // Make sure strtod() uses a decimal point, not a comma.
    रखो_क्षेत्र(LC_NUMERIC, "C");
# endif

# ifdef MSWIN
    // Apparently MS-Windows printf() may cause a crash when we give it 8-bit
    // text while it's expecting text in the current locale.  This call avoids
    // that.
    रखो_क्षेत्र(LC_CTYPE, "C");
# endif

# ifdef FEAT_GETTEXT
    {
	पूर्णांक	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	जर (p != रिक्त && *p != NUL)
	{
	    vim_snprintf((अक्षर *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (अक्षर *)NameBuff);
	}
	जर (mustfree)
	    vim_free(p);
	textdomain(VIMPACKAGE);
    }
# endif
}

/*
 * ":language":  Set the language (locale).
 */
    शून्य
ex_language(exarg_T *eap)
{
    अक्षर	*loc;
    char_u	*p;
    char_u	*name;
    पूर्णांक		what = LC_ALL;
    अक्षर	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# अन्यथा
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    जर ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	जर (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	अन्यथा जर (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	अन्यथा जर (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	अन्यथा जर (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    जर (*name == NUL)
    {
# ifndef LC_MESSAGES
	जर (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	अन्यथा
# endif
	    p = (char_u *)रखो_क्षेत्र(what, रिक्त);
	जर (p == रिक्त || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    अन्यथा
    {
# ifndef LC_MESSAGES
	जर (what == VIM_LC_MESSAGES)
	    loc = "";
	अन्यथा
# endif
	{
	    loc = रखो_क्षेत्र(what, (अक्षर *)name);
# जर defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    रखो_क्षेत्र(LC_NUMERIC, "C");
# endif
	}
	जर (loc == रिक्त)
	    semsg(_(e_cannot_set_language_to_str), name);
	अन्यथा
	{
# ifdef HAVE_NL_संदेश_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    बाह्य पूर्णांक nl_संदेश_cat_cntr;

	    ++nl_संदेश_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    जर (dyn_libintl_nl_संदेश_cat_cntr != रिक्त)
		++*dyn_libintl_nl_संदेश_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    जर (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		जर (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    रखो_क्षेत्र(LC_CTYPE, "C");
# endif
		}
		जर (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# अन्यथा
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

स्थिर char_u	**locales = रिक्त;	// Array of all available locales

स्थिर पूर्णांक	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    स्थिर char_u **
find_locales(शून्य)
{
    garray_T	locales_ga;
    char_u	*loc;
    char_u	*locale_list;
# ifdef MSWIN
    माप_प्रकार	len = 0;
# endif

    // Find all available locales by running command "locale -a".  If this
    // doesn't work we won't have completion.
# ifndef MSWIN
    locale_list = get_cmd_output((char_u *)"locale -a",
						    रिक्त, SHELL_SILENT, रिक्त);
# अन्यथा
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	पूर्णांक		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      रिक्त, options, WILD_ALL);
	ExpandCleanup(&xpc);
	जर (locale_list == रिक्त)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	पर्यंत (p != रिक्त && *p != NUL)
	{
	    जर (*p == '\n')
		थांबा;
	    जर (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    जर (locale_list == रिक्त)
	परत रिक्त;
    ga_init2(&locales_ga, आकार(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)म_मोहर((अक्षर *)locale_list, "\n");

    पर्यंत (loc != रिक्त)
    {
	पूर्णांक ignore = FALSE;

# ifdef MSWIN
	जर (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	जर (vim_strchr(loc, '.') != रिक्त)
	    ignore = TRUE;
# endif
	जर (!ignore)
	{
	    जर (ga_grow(&locales_ga, 1) == FAIL)
		थांबा;

	    loc = vim_strsave(loc);
	    जर (loc == रिक्त)
		थांबा;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)म_मोहर(रिक्त, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    जर (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    जर (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	परत रिक्त;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = रिक्त;
    परत (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    स्थिर शून्य
init_locales(शून्य)
{
    जर (did_init_locales)
	परत;

    did_init_locales = TRUE;
    locales = find_locales();
}

# जर defined(EXITFREE)
    शून्य
free_locales(शून्य)
{
    पूर्णांक			i;

    जर (locales == रिक्त)
	परत;

    साठी (i = 0; locales[i] != रिक्त; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx == 0)
	परत (char_u *)"messages";
    जर (idx == 1)
	परत (char_u *)"ctype";
    जर (idx == 2)
	परत (char_u *)"time";
    जर (idx == 3)
	परत (char_u *)"collate";

    init_locales();
    जर (locales == रिक्त)
	परत रिक्त;
    परत locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, पूर्णांक idx)
{
    init_locales();
    जर (locales == रिक्त)
	परत रिक्त;
    परत locales[idx];
}

#endif
