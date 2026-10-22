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

#అన్తర్భావ "vim.h"

#ఒకవేళ (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    స్థిర char_u *
get_locale_val(పూర్ణాంకం what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, ఖాళీ);

# ifdef MSWIN
    ఒకవేళ (loc != ఖాళీ)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	ఒకవేళ (p != ఖాళీ)
	{
	    loc = ++p;
	    అయితే (*p != NUL)	// remove trailing newline
	    {
		ఒకవేళ (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    ఆపు;
		}
		++p;
	    }
	}
    }
# endif

    తిరిగి loc;
}
#అయితే_ముగింపు


#నిర్వచితమైతే MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    స్థిర char_u *
gettext_lang(char_u *name)
{
    పూర్ణాంకం		i;
    స్థిర అక్షరం *(mtable[]) = {
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
			ఖాళీ};

    కొరకు (i = 0; mtable[i] != ఖాళీ; i += 2)
	ఒకవేళ (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    తిరిగి (char_u *)mtable[i + 1];
    తిరిగి name;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    స్థిర పూర్ణాంకం
is_valid_mess_lang(char_u *lang)
{
    తిరిగి lang != ఖాళీ && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(శూన్య)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  ఒకవేళ defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  లేకపోతే
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    ఒకవేళ (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# లేకపోతే
    p = mch_getenv((char_u *)"LC_ALL");
    ఒకవేళ (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	ఒకవేళ (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    తిరిగి is_valid_mess_lang(p) ? p : ఖాళీ;
}
#అయితే_ముగింపు

// Complicated #if; matches with where get_mess_env() is used below.
#ఒకవేళ (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    స్థిర char_u *
get_mess_env(శూన్య)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    ఒకవేళ (p != ఖాళీ && *p != NUL)
	తిరిగి p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    ఒకవేళ (p != ఖాళీ && *p != NUL)
	తిరిగి p;

    p = mch_getenv((char_u *)"LANG");
    ఒకవేళ (p != ఖాళీ && VIM_ISDIGIT(*p))
	p = ఖాళీ;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    ఒకవేళ (p == ఖాళీ || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    తిరిగి p;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    శూన్య
సమితి_lang_var(శూన్య)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# లేకపోతే
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    సమితి_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# ఒకవేళ defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# లేకపోతే
    loc = get_mess_env();
# endif
    సమితి_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_సమయం);
# endif
    సమితి_vim_var_string(VV_LC_సమయం, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# లేకపోతే
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    సమితి_vim_var_string(VV_COLLATE, loc, -1);
}
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    శూన్య
init_locale(శూన్య)
{
    setlocale(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# ఒకవేళ defined(LC_NUMERIC)
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
	పూర్ణాంకం	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	ఒకవేళ (p != ఖాళీ && *p != NUL)
	{
	    vim_snprintf((అక్షరం *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (అక్షరం *)NameBuff);
	}
	ఒకవేళ (mustfree)
	    vim_free(p);
	textdomain(VIMPACKAGE);
    }
# endif
}

/*
 * ":language":  Set the language (locale).
 */
    శూన్య
ex_language(exarg_T *eap)
{
    అక్షరం	*loc;
    char_u	*p;
    char_u	*name;
    పూర్ణాంకం		what = LC_ALL;
    అక్షరం	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# లేకపోతే
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    ఒకవేళ ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	ఒకవేళ (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	లేకపోతే ఒకవేళ (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	లేకపోతే ఒకవేళ (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_సమయం;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	లేకపోతే ఒకవేళ (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    ఒకవేళ (*name == NUL)
    {
# ifndef LC_MESSAGES
	ఒకవేళ (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	లేకపోతే
# endif
	    p = (char_u *)setlocale(what, ఖాళీ);
	ఒకవేళ (p == ఖాళీ || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    లేకపోతే
    {
# ifndef LC_MESSAGES
	ఒకవేళ (what == VIM_LC_MESSAGES)
	    loc = "";
	లేకపోతే
# endif
	{
	    loc = setlocale(what, (అక్షరం *)name);
# ఒకవేళ defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	ఒకవేళ (loc == ఖాళీ)
	    semsg(_(e_cannot_సమితి_language_గమ్యం_str), name);
	లేకపోతే
	{
# ifdef HAVE_NL_సందేశం_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    బాహ్య పూర్ణాంకం nl_సందేశం_cat_cntr;

	    ++nl_సందేశం_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    ఒకవేళ (dyn_libintl_nl_సందేశం_cat_cntr != ఖాళీ)
		++*dyn_libintl_nl_సందేశం_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    ఒకవేళ (what != LC_సమయం && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		ఒకవేళ (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		ఒకవేళ (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# లేకపోతే
		    mname = name;
# endif
		    // Clear $LANGUAGE because GNU gettext uses it.
		    vim_setenv((char_u *)"LANGUAGE", (char_u *)"");
		    vim_setenv((char_u *)"LC_MESSAGES", mname);
# ifdef FEAT_MULTI_LANG
		    సమితి_helplang_default(mname);
# endif
		}
	    }

# ifdef FEAT_EVAL
	    // Set v:lang, v:lc_time, v:collate and v:ctype to the final result.
	    సమితి_lang_var();
# endif
	    maketitle();
	}
    }
}

స్థిర char_u	**locales = ఖాళీ;	// Array of all available locales

స్థిర పూర్ణాంకం	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    స్థిర char_u **
కనుగొను_locales(శూన్య)
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
						    ఖాళీ, SHELL_SILENT, ఖాళీ);
# లేకపోతే
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	పూర్ణాంకం		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      ఖాళీ, options, WILD_ALL);
	ExpandCleanup(&xpc);
	ఒకవేళ (locale_list == ఖాళీ)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	అయితే (p != ఖాళీ && *p != NUL)
	{
	    ఒకవేళ (*p == '\n')
		ఆపు;
	    ఒకవేళ (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    ఒకవేళ (locale_list == ఖాళీ)
	తిరిగి ఖాళీ;
    ga_init2(&locales_ga, పరిమాణం(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((అక్షరం *)locale_list, "\n");

    అయితే (loc != ఖాళీ)
    {
	పూర్ణాంకం ignore = FALSE;

# ifdef MSWIN
	ఒకవేళ (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	ఒకవేళ (vim_strchr(loc, '.') != ఖాళీ)
	    ignore = TRUE;
# endif
	ఒకవేళ (!ignore)
	{
	    ఒకవేళ (ga_grow(&locales_ga, 1) == FAIL)
		ఆపు;

	    loc = vim_strsave(loc);
	    ఒకవేళ (loc == ఖాళీ)
		ఆపు;

	    ((char_u **)locales_ga.ga_దత్తాంశం)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(ఖాళీ, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    ఒకవేళ (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_దత్తాంశం)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    ఒకవేళ (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	తిరిగి ఖాళీ;
    }
    ((char_u **)locales_ga.ga_దత్తాంశం)[locales_ga.ga_len] = ఖాళీ;
    తిరిగి (char_u **)locales_ga.ga_దత్తాంశం;
}

/*
 * Lazy initialization of all available locales.
 */
    స్థిర శూన్య
init_locales(శూన్య)
{
    ఒకవేళ (did_init_locales)
	తిరిగి;

    did_init_locales = TRUE;
    locales = కనుగొను_locales();
}

# ఒకవేళ defined(EXITFREE)
    శూన్య
free_locales(శూన్య)
{
    పూర్ణాంకం			i;

    ఒకవేళ (locales == ఖాళీ)
	తిరిగి;

    కొరకు (i = 0; locales[i] != ఖాళీ; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx == 0)
	తిరిగి (char_u *)"messages";
    ఒకవేళ (idx == 1)
	తిరిగి (char_u *)"ctype";
    ఒకవేళ (idx == 2)
	తిరిగి (char_u *)"time";
    ఒకవేళ (idx == 3)
	తిరిగి (char_u *)"collate";

    init_locales();
    ఒకవేళ (locales == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    init_locales();
    ఒకవేళ (locales == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి locales[idx];
}

#అయితే_ముగింపు
