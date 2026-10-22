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

#यदि (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    स्थिर char_u *
get_locale_val(पूर्णाङ्क what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)रखो_क्षेत्र(what, रिक्त);

# ifdef MSWIN
    यदि (loc != रिक्त)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	यदि (p != रिक्त)
	{
	    loc = ++p;
	    यावत् (*p != NUL)	// remove trailing newline
	    {
		यदि (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    भङ्ग;
		}
		++p;
	    }
	}
    }
# endif

    प्रत्यागम loc;
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
    पूर्णाङ्क		i;
    स्थिर वर्ण *(mtable[]) = {
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

    प्रति (i = 0; mtable[i] != रिक्त; i += 2)
	यदि (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    प्रत्यागम (char_u *)mtable[i + 1];
    प्रत्यागम name;
}
#endif

#यदि defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    स्थिर पूर्णाङ्क
is_valid_mess_lang(char_u *lang)
{
    प्रत्यागम lang != रिक्त && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
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
#  यदि defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  अन्यथा
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    यदि (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# अन्यथा
    p = mch_getenv((char_u *)"LC_ALL");
    यदि (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	यदि (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    प्रत्यागम is_valid_mess_lang(p) ? p : रिक्त;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#यदि (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
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
    यदि (p != रिक्त && *p != NUL)
	प्रत्यागम p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    यदि (p != रिक्त && *p != NUL)
	प्रत्यागम p;

    p = mch_getenv((char_u *)"LANG");
    यदि (p != रिक्त && VIM_ISDIGIT(*p))
	p = रिक्त;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    यदि (p == रिक्त || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    प्रत्यागम p;
}
#endif

#यदि defined(FEAT_EVAL)

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
# यदि defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
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

#यदि defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
# यदि defined(LC_NUMERIC)
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
	पूर्णाङ्क	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	यदि (p != रिक्त && *p != NUL)
	{
	    vim_snprintf((वर्ण *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (वर्ण *)NameBuff);
	}
	यदि (mustfree)
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
    वर्ण	*loc;
    char_u	*p;
    char_u	*name;
    पूर्णाङ्क		what = LC_ALL;
    वर्ण	*whatstr = "";
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
    यदि ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	यदि (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	अन्यथा यदि (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	अन्यथा यदि (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	अन्यथा यदि (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    यदि (*name == NUL)
    {
# ifndef LC_MESSAGES
	यदि (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	अन्यथा
# endif
	    p = (char_u *)रखो_क्षेत्र(what, रिक्त);
	यदि (p == रिक्त || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    अन्यथा
    {
# ifndef LC_MESSAGES
	यदि (what == VIM_LC_MESSAGES)
	    loc = "";
	अन्यथा
# endif
	{
	    loc = रखो_क्षेत्र(what, (वर्ण *)name);
# यदि defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    रखो_क्षेत्र(LC_NUMERIC, "C");
# endif
	}
	यदि (loc == रिक्त)
	    semsg(_(e_cannot_set_language_to_str), name);
	अन्यथा
	{
# ifdef HAVE_NL_संदेश_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    बाह्य पूर्णाङ्क nl_संदेश_cat_cntr;

	    ++nl_संदेश_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    यदि (dyn_libintl_nl_संदेश_cat_cntr != रिक्त)
		++*dyn_libintl_nl_संदेश_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    यदि (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		यदि (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    रखो_क्षेत्र(LC_CTYPE, "C");
# endif
		}
		यदि (what != LC_CTYPE)
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

स्थिर पूर्णाङ्क	did_init_locales = FALSE;

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
	पूर्णाङ्क		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      रिक्त, options, WILD_ALL);
	ExpandCleanup(&xpc);
	यदि (locale_list == रिक्त)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	यावत् (p != रिक्त && *p != NUL)
	{
	    यदि (*p == '\n')
		भङ्ग;
	    यदि (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    यदि (locale_list == रिक्त)
	प्रत्यागम रिक्त;
    ga_init2(&locales_ga, परिमाण(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)म_मोहर((वर्ण *)locale_list, "\n");

    यावत् (loc != रिक्त)
    {
	पूर्णाङ्क ignore = FALSE;

# ifdef MSWIN
	यदि (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	यदि (vim_strchr(loc, '.') != रिक्त)
	    ignore = TRUE;
# endif
	यदि (!ignore)
	{
	    यदि (ga_grow(&locales_ga, 1) == FAIL)
		भङ्ग;

	    loc = vim_strsave(loc);
	    यदि (loc == रिक्त)
		भङ्ग;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)म_मोहर(रिक्त, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    यदि (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    यदि (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	प्रत्यागम रिक्त;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = रिक्त;
    प्रत्यागम (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    स्थिर शून्य
init_locales(शून्य)
{
    यदि (did_init_locales)
	प्रत्यागम;

    did_init_locales = TRUE;
    locales = find_locales();
}

# यदि defined(EXITFREE)
    शून्य
free_locales(शून्य)
{
    पूर्णाङ्क			i;

    यदि (locales == रिक्त)
	प्रत्यागम;

    प्रति (i = 0; locales[i] != रिक्त; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    यदि (idx == 0)
	प्रत्यागम (char_u *)"messages";
    यदि (idx == 1)
	प्रत्यागम (char_u *)"ctype";
    यदि (idx == 2)
	प्रत्यागम (char_u *)"time";
    यदि (idx == 3)
	प्रत्यागम (char_u *)"collate";

    init_locales();
    यदि (locales == रिक्त)
	प्रत्यागम रिक्त;
    प्रत्यागम locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    init_locales();
    यदि (locales == रिक्त)
	प्रत्यागम रिक्त;
    प्रत्यागम locales[idx];
}

#endif
