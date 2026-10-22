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

#ಆದರೆ (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    ಸ್ಥಿರ char_u *
get_locale_val(ಪೂರ್ಣಾಂಕ what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, ಖಾಲಿ);

# ifdef MSWIN
    ಆದರೆ (loc != ಖಾಲಿ)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    loc = ++p;
	    ಆಗ (*p != NUL)	// remove trailing newline
	    {
		ಆದರೆ (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    ನಿಲ್ಲಿಸು;
		}
		++p;
	    }
	}
    }
# endif

    ಹಿಂತಿರುಗಿ loc;
}
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    ಸ್ಥಿರ char_u *
gettext_lang(char_u *name)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಸ್ಥಿರ ಅಕ್ಷರ *(mtable[]) = {
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
			ಖಾಲಿ};

    ಗಾಗಿ (i = 0; mtable[i] != ಖಾಲಿ; i += 2)
	ಆದರೆ (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    ಹಿಂತಿರುಗಿ (char_u *)mtable[i + 1];
    ಹಿಂತಿರುಗಿ name;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_valid_mess_lang(char_u *lang)
{
    ಹಿಂತಿರುಗಿ lang != ಖಾಲಿ && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
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
#  ಆದರೆ defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  ಇಲ್ಲದಿದ್ದರೆ
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    ಆದರೆ (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
    p = mch_getenv((char_u *)"LC_ALL");
    ಆದರೆ (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	ಆದರೆ (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    ಹಿಂತಿರುಗಿ is_valid_mess_lang(p) ? p : ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

// Complicated #if; matches with where get_mess_env() is used below.
#ಆದರೆ (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
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
    ಆದರೆ (p != ಖಾಲಿ && *p != NUL)
	ಹಿಂತಿರುಗಿ p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    ಆದರೆ (p != ಖಾಲಿ && *p != NUL)
	ಹಿಂತಿರುಗಿ p;

    p = mch_getenv((char_u *)"LANG");
    ಆದರೆ (p != ಖಾಲಿ && VIM_ISDIGIT(*p))
	p = ಖಾಲಿ;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    ಆದರೆ (p == ಖಾಲಿ || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    ಹಿಂತಿರುಗಿ p;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    ಶೂನ್ಯ
ಗಣ_lang_var(ಶೂನ್ಯ)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# ಇಲ್ಲದಿದ್ದರೆ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    ಗಣ_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# ಆದರೆ defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# ಇಲ್ಲದಿದ್ದರೆ
    loc = get_mess_env();
# endif
    ಗಣ_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_ಸಮಯ);
# endif
    ಗಣ_vim_var_string(VV_LC_ಸಮಯ, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# ಇಲ್ಲದಿದ್ದರೆ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    ಗಣ_vim_var_string(VV_COLLATE, loc, -1);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
# ಆದರೆ defined(LC_NUMERIC)
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
	ಪೂರ್ಣಾಂಕ	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	ಆದರೆ (p != ಖಾಲಿ && *p != NUL)
	{
	    vim_snprintf((ಅಕ್ಷರ *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (ಅಕ್ಷರ *)NameBuff);
	}
	ಆದರೆ (mustfree)
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
    ಅಕ್ಷರ	*loc;
    char_u	*p;
    char_u	*name;
    ಪೂರ್ಣಾಂಕ		what = LC_ALL;
    ಅಕ್ಷರ	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# ಇಲ್ಲದಿದ್ದರೆ
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    ಆದರೆ ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	ಆದರೆ (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_ಸಮಯ;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    ಆದರೆ (*name == NUL)
    {
# ifndef LC_MESSAGES
	ಆದರೆ (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    p = (char_u *)setlocale(what, ಖಾಲಿ);
	ಆದರೆ (p == ಖಾಲಿ || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
# ifndef LC_MESSAGES
	ಆದರೆ (what == VIM_LC_MESSAGES)
	    loc = "";
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	{
	    loc = setlocale(what, (ಅಕ್ಷರ *)name);
# ಆದರೆ defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	ಆದರೆ (loc == ಖಾಲಿ)
	    semsg(_(e_cannot_ಗಣ_language_ಗಮ್ಯ_str), name);
	ಇಲ್ಲದಿದ್ದರೆ
	{
# ifdef HAVE_NL_ಸಂದೇಶ_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ nl_ಸಂದೇಶ_cat_cntr;

	    ++nl_ಸಂದೇಶ_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    ಆದರೆ (dyn_libintl_nl_ಸಂದೇಶ_cat_cntr != ಖಾಲಿ)
		++*dyn_libintl_nl_ಸಂದೇಶ_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    ಆದರೆ (what != LC_ಸಮಯ && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		ಆದರೆ (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		ಆದರೆ (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# ಇಲ್ಲದಿದ್ದರೆ
		    mname = name;
# endif
		    // Clear $LANGUAGE because GNU gettext uses it.
		    vim_setenv((char_u *)"LANGUAGE", (char_u *)"");
		    vim_setenv((char_u *)"LC_MESSAGES", mname);
# ifdef FEAT_MULTI_LANG
		    ಗಣ_helplang_default(mname);
# endif
		}
	    }

# ifdef FEAT_EVAL
	    // Set v:lang, v:lc_time, v:collate and v:ctype to the final result.
	    ಗಣ_lang_var();
# endif
	    maketitle();
	}
    }
}

ಸ್ಥಿರ char_u	**locales = ಖಾಲಿ;	// Array of all available locales

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    ಸ್ಥಿರ char_u **
ಕಂಡುಹಿಡಿ_locales(ಶೂನ್ಯ)
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
						    ಖಾಲಿ, SHELL_SILENT, ಖಾಲಿ);
# ಇಲ್ಲದಿದ್ದರೆ
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	ಪೂರ್ಣಾಂಕ		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      ಖಾಲಿ, options, WILD_ALL);
	ExpandCleanup(&xpc);
	ಆದರೆ (locale_list == ಖಾಲಿ)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	ಆಗ (p != ಖಾಲಿ && *p != NUL)
	{
	    ಆದರೆ (*p == '\n')
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    ಆದರೆ (locale_list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ga_init2(&locales_ga, ಗಾತ್ರ(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((ಅಕ್ಷರ *)locale_list, "\n");

    ಆಗ (loc != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ ignore = FALSE;

# ifdef MSWIN
	ಆದರೆ (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	ಆದರೆ (vim_strchr(loc, '.') != ಖಾಲಿ)
	    ignore = TRUE;
# endif
	ಆದರೆ (!ignore)
	{
	    ಆದರೆ (ga_grow(&locales_ga, 1) == FAIL)
		ನಿಲ್ಲಿಸು;

	    loc = vim_strsave(loc);
	    ಆದರೆ (loc == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

	    ((char_u **)locales_ga.ga_ದತ್ತಾಂಶ)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(ಖಾಲಿ, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    ಆದರೆ (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_ದತ್ತಾಂಶ)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    ಆದರೆ (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ((char_u **)locales_ga.ga_ದತ್ತಾಂಶ)[locales_ga.ga_len] = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ (char_u **)locales_ga.ga_ದತ್ತಾಂಶ;
}

/*
 * Lazy initialization of all available locales.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_locales(ಶೂನ್ಯ)
{
    ಆದರೆ (did_init_locales)
	ಹಿಂತಿರುಗಿ;

    did_init_locales = TRUE;
    locales = ಕಂಡುಹಿಡಿ_locales();
}

# ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_locales(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ			i;

    ಆದರೆ (locales == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; locales[i] != ಖಾಲಿ; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx == 0)
	ಹಿಂತಿರುಗಿ (char_u *)"messages";
    ಆದರೆ (idx == 1)
	ಹಿಂತಿರುಗಿ (char_u *)"ctype";
    ಆದರೆ (idx == 2)
	ಹಿಂತಿರುಗಿ (char_u *)"time";
    ಆದರೆ (idx == 3)
	ಹಿಂತಿರುಗಿ (char_u *)"collate";

    init_locales();
    ಆದರೆ (locales == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    init_locales();
    ಆದರೆ (locales == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ locales[idx];
}

#ಆದರೆ_ಅಂತ್ಯ
