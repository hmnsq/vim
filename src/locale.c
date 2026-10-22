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

#सचे (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    स्थिर char_u *
get_locale_val(पूर्णाङ्क what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, रिक्त);

# ifdef MSWIN
    सचे (loc != रिक्त)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	सचे (p != रिक्त)
	{
	    loc = ++p;
	    याव (*p != NUL)	// remove trailing newline
	    {
		सचे (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    भङ्ग;
		}
		++p;
	    }
	}
    }
# endif

    पटिनिवत्त loc;
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

    अत्थाय (i = 0; mtable[i] != रिक्त; i += 2)
	सचे (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    पटिनिवत्त (char_u *)mtable[i + 1];
    पटिनिवत्त name;
}
#endif

#सचे defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    स्थिर पूर्णाङ्क
is_valid_mess_lang(char_u *lang)
{
    पटिनिवत्त lang != रिक्त && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
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
#  सचे defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  अञ्ञथा
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    सचे (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# अञ्ञथा
    p = mch_getenv((char_u *)"LC_ALL");
    सचे (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	सचे (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    पटिनिवत्त is_valid_mess_lang(p) ? p : रिक्त;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#सचे (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
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
    सचे (p != रिक्त && *p != NUL)
	पटिनिवत्त p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    सचे (p != रिक्त && *p != NUL)
	पटिनिवत्त p;

    p = mch_getenv((char_u *)"LANG");
    सचे (p != रिक्त && VIM_ISDIGIT(*p))
	p = रिक्त;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    सचे (p == रिक्त || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    पटिनिवत्त p;
}
#endif

#सचे defined(FEAT_EVAL)

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
# अञ्ञथा
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# सचे defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# अञ्ञथा
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# अञ्ञथा
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#सचे defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    शून्य
init_locale(शून्य)
{
    setlocale(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# सचे defined(LC_NUMERIC)
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
	पूर्णाङ्क	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	सचे (p != रिक्त && *p != NUL)
	{
	    vim_snprintf((वर्ण *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (वर्ण *)NameBuff);
	}
	सचे (mustfree)
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
# अञ्ञथा
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    सचे ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	सचे (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	अञ्ञथा सचे (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	अञ्ञथा सचे (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	अञ्ञथा सचे (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    सचे (*name == NUL)
    {
# ifndef LC_MESSAGES
	सचे (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	अञ्ञथा
# endif
	    p = (char_u *)setlocale(what, रिक्त);
	सचे (p == रिक्त || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    अञ्ञथा
    {
# ifndef LC_MESSAGES
	सचे (what == VIM_LC_MESSAGES)
	    loc = "";
	अञ्ञथा
# endif
	{
	    loc = setlocale(what, (वर्ण *)name);
# सचे defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	सचे (loc == रिक्त)
	    semsg(_(e_cannot_set_language_to_str), name);
	अञ्ञथा
	{
# ifdef HAVE_NL_सन्देशः_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    बाह्य पूर्णाङ्क nl_सन्देशः_cat_cntr;

	    ++nl_सन्देशः_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    सचे (dyn_libintl_nl_सन्देशः_cat_cntr != रिक्त)
		++*dyn_libintl_nl_सन्देशः_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    सचे (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		सचे (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		सचे (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# अञ्ञथा
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
    size_t	len = 0;
# endif

    // Find all available locales by running command "locale -a".  If this
    // doesn't work we won't have completion.
# ifndef MSWIN
    locale_list = get_cmd_output((char_u *)"locale -a",
						    रिक्त, SHELL_SILENT, रिक्त);
# अञ्ञथा
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
	सचे (locale_list == रिक्त)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	याव (p != रिक्त && *p != NUL)
	{
	    सचे (*p == '\n')
		भङ्ग;
	    सचे (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    सचे (locale_list == रिक्त)
	पटिनिवत्त रिक्त;
    ga_init2(&locales_ga, परिमाण(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((वर्ण *)locale_list, "\n");

    याव (loc != रिक्त)
    {
	पूर्णाङ्क ignore = FALSE;

# ifdef MSWIN
	सचे (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	सचे (vim_strchr(loc, '.') != रिक्त)
	    ignore = TRUE;
# endif
	सचे (!ignore)
	{
	    सचे (ga_grow(&locales_ga, 1) == FAIL)
		भङ्ग;

	    loc = vim_strsave(loc);
	    सचे (loc == रिक्त)
		भङ्ग;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(रिक्त, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    सचे (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    सचे (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	पटिनिवत्त रिक्त;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = रिक्त;
    पटिनिवत्त (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    स्थिर शून्य
init_locales(शून्य)
{
    सचे (did_init_locales)
	पटिनिवत्त;

    did_init_locales = TRUE;
    locales = find_locales();
}

# सचे defined(EXITFREE)
    शून्य
free_locales(शून्य)
{
    पूर्णाङ्क			i;

    सचे (locales == रिक्त)
	पटिनिवत्त;

    अत्थाय (i = 0; locales[i] != रिक्त; i++)
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
    सचे (idx == 0)
	पटिनिवत्त (char_u *)"messages";
    सचे (idx == 1)
	पटिनिवत्त (char_u *)"ctype";
    सचे (idx == 2)
	पटिनिवत्त (char_u *)"time";
    सचे (idx == 3)
	पटिनिवत्त (char_u *)"collate";

    init_locales();
    सचे (locales == रिक्त)
	पटिनिवत्त रिक्त;
    पटिनिवत्त locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    init_locales();
    सचे (locales == रिक्त)
	पटिनिवत्त रिक्त;
    पटिनिवत्त locales[idx];
}

#endif
