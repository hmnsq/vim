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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    ಸ್ಥಿರ char_u *
get_locale_val(ಪೂರ್ಣಾಙ್ಕ what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, ರಿಕ್ತ);

# ifdef MSWIN
    ಯದಿ (loc != ರಿಕ್ತ)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    loc = ++p;
	    ಯಾವತ್ (*p != NUL)	// remove trailing newline
	    {
		ಯದಿ (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    ಭಙ್ಗ;
		}
		++p;
	    }
	}
    }
# endif

    ಪ್ರತ್ಯಾಗಮ loc;
}
#endif


#ifdef MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    ಸ್ಥಿರ char_u *
gettext_lang(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಸ್ಥಿರ ವರ್ಣ *(mtable[]) = {
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
			ರಿಕ್ತ};

    ಪ್ರತಿ (i = 0; mtable[i] != ರಿಕ್ತ; i += 2)
	ಯದಿ (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    ಪ್ರತ್ಯಾಗಮ (char_u *)mtable[i + 1];
    ಪ್ರತ್ಯಾಗಮ name;
}
#endif

#ಯದಿ defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_valid_mess_lang(char_u *lang)
{
    ಪ್ರತ್ಯಾಗಮ lang != ರಿಕ್ತ && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(ಶೂನ್ಯ)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  ಯದಿ defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  ಅನ್ಯಥಾ
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    ಯದಿ (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# ಅನ್ಯಥಾ
    p = mch_getenv((char_u *)"LC_ALL");
    ಯದಿ (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	ಯದಿ (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    ಪ್ರತ್ಯಾಗಮ is_valid_mess_lang(p) ? p : ರಿಕ್ತ;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#ಯದಿ (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    ಸ್ಥಿರ char_u *
get_mess_env(ಶೂನ್ಯ)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    ಯದಿ (p != ರಿಕ್ತ && *p != NUL)
	ಪ್ರತ್ಯಾಗಮ p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    ಯದಿ (p != ರಿಕ್ತ && *p != NUL)
	ಪ್ರತ್ಯಾಗಮ p;

    p = mch_getenv((char_u *)"LANG");
    ಯದಿ (p != ರಿಕ್ತ && VIM_ISDIGIT(*p))
	p = ರಿಕ್ತ;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    ಯದಿ (p == ರಿಕ್ತ || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    ಪ್ರತ್ಯಾಗಮ p;
}
#endif

#ಯದಿ defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    ಶೂನ್ಯ
set_lang_var(ಶೂನ್ಯ)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# ಅನ್ಯಥಾ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# ಯದಿ defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# ಅನ್ಯಥಾ
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# ಅನ್ಯಥಾ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#ಯದಿ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    ಶೂನ್ಯ
init_locale(ಶೂನ್ಯ)
{
    setlocale(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# ಯದಿ defined(LC_NUMERIC)
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
	ಪೂರ್ಣಾಙ್ಕ	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	ಯದಿ (p != ರಿಕ್ತ && *p != NUL)
	{
	    vim_snprintf((ವರ್ಣ *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (ವರ್ಣ *)NameBuff);
	}
	ಯದಿ (mustfree)
	    vim_free(p);
	textdomain(VIMPACKAGE);
    }
# endif
}

/*
 * ":language":  Set the language (locale).
 */
    ಶೂನ್ಯ
ex_language(exarg_T *eap)
{
    ವರ್ಣ	*loc;
    char_u	*p;
    char_u	*name;
    ಪೂರ್ಣಾಙ್ಕ		what = LC_ALL;
    ವರ್ಣ	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# ಅನ್ಯಥಾ
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    ಯದಿ ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	ಯದಿ (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    ಯದಿ (*name == NUL)
    {
# ifndef LC_MESSAGES
	ಯದಿ (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	ಅನ್ಯಥಾ
# endif
	    p = (char_u *)setlocale(what, ರಿಕ್ತ);
	ಯದಿ (p == ರಿಕ್ತ || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    ಅನ್ಯಥಾ
    {
# ifndef LC_MESSAGES
	ಯದಿ (what == VIM_LC_MESSAGES)
	    loc = "";
	ಅನ್ಯಥಾ
# endif
	{
	    loc = setlocale(what, (ವರ್ಣ *)name);
# ಯದಿ defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	ಯದಿ (loc == ರಿಕ್ತ)
	    semsg(_(e_cannot_set_language_to_str), name);
	ಅನ್ಯಥಾ
	{
# ifdef HAVE_NL_ಸಂದೇಶ_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ nl_ಸಂದೇಶ_cat_cntr;

	    ++nl_ಸಂದೇಶ_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    ಯದಿ (dyn_libintl_nl_ಸಂದೇಶ_cat_cntr != ರಿಕ್ತ)
		++*dyn_libintl_nl_ಸಂದೇಶ_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    ಯದಿ (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		ಯದಿ (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		ಯದಿ (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# ಅನ್ಯಥಾ
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

ಸ್ಥಿರ char_u	**locales = ರಿಕ್ತ;	// Array of all available locales

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    ಸ್ಥಿರ char_u **
find_locales(ಶೂನ್ಯ)
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
						    ರಿಕ್ತ, SHELL_SILENT, ರಿಕ್ತ);
# ಅನ್ಯಥಾ
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	ಪೂರ್ಣಾಙ್ಕ		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      ರಿಕ್ತ, options, WILD_ALL);
	ExpandCleanup(&xpc);
	ಯದಿ (locale_list == ರಿಕ್ತ)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	ಯಾವತ್ (p != ರಿಕ್ತ && *p != NUL)
	{
	    ಯದಿ (*p == '\n')
		ಭಙ್ಗ;
	    ಯದಿ (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    ಯದಿ (locale_list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ga_init2(&locales_ga, ಪರಿಮಾಣ(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((ವರ್ಣ *)locale_list, "\n");

    ಯಾವತ್ (loc != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ ignore = FALSE;

# ifdef MSWIN
	ಯದಿ (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	ಯದಿ (vim_strchr(loc, '.') != ರಿಕ್ತ)
	    ignore = TRUE;
# endif
	ಯದಿ (!ignore)
	{
	    ಯದಿ (ga_grow(&locales_ga, 1) == FAIL)
		ಭಙ್ಗ;

	    loc = vim_strsave(loc);
	    ಯದಿ (loc == ರಿಕ್ತ)
		ಭಙ್ಗ;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(ರಿಕ್ತ, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    ಯದಿ (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    ಯದಿ (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_locales(ಶೂನ್ಯ)
{
    ಯದಿ (did_init_locales)
	ಪ್ರತ್ಯಾಗಮ;

    did_init_locales = TRUE;
    locales = find_locales();
}

# ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_locales(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ			i;

    ಯದಿ (locales == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (i = 0; locales[i] != ರಿಕ್ತ; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx == 0)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"messages";
    ಯದಿ (idx == 1)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"ctype";
    ಯದಿ (idx == 2)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"time";
    ಯದಿ (idx == 3)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"collate";

    init_locales();
    ಯದಿ (locales == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    init_locales();
    ಯದಿ (locales == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ locales[idx];
}

#endif
