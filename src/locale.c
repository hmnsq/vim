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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    സ്ഥിതം char_u *
get_locale_val(പൂർണ്ണസംഖ്യ what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, ശൂന്യ);

# ifdef MSWIN
    എങ്കിൽ (loc != ശൂന്യ)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	എങ്കിൽ (p != ശൂന്യ)
	{
	    loc = ++p;
	    ആകുമ്പോൾ (*p != NUL)	// remove trailing newline
	    {
		എങ്കിൽ (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    നിർത്തുക;
		}
		++p;
	    }
	}
    }
# endif

    തിരികെ loc;
}
#endif


#ifdef MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    സ്ഥിതം char_u *
gettext_lang(char_u *name)
{
    പൂർണ്ണസംഖ്യ		i;
    സ്ഥിതം അക്ഷരം *(mtable[]) = {
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
			ശൂന്യ};

    വേണ്ടി (i = 0; mtable[i] != ശൂന്യ; i += 2)
	എങ്കിൽ (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    തിരികെ (char_u *)mtable[i + 1];
    തിരികെ name;
}
#endif

#എങ്കിൽ defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_valid_mess_lang(char_u *lang)
{
    തിരികെ lang != ശൂന്യ && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(ശൂന്യം)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  എങ്കിൽ defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  അല്ലെങ്കിൽ
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    എങ്കിൽ (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# അല്ലെങ്കിൽ
    p = mch_getenv((char_u *)"LC_ALL");
    എങ്കിൽ (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	എങ്കിൽ (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    തിരികെ is_valid_mess_lang(p) ? p : ശൂന്യ;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#എങ്കിൽ (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    സ്ഥിതം char_u *
get_mess_env(ശൂന്യം)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    എങ്കിൽ (p != ശൂന്യ && *p != NUL)
	തിരികെ p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    എങ്കിൽ (p != ശൂന്യ && *p != NUL)
	തിരികെ p;

    p = mch_getenv((char_u *)"LANG");
    എങ്കിൽ (p != ശൂന്യ && VIM_ISDIGIT(*p))
	p = ശൂന്യ;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    എങ്കിൽ (p == ശൂന്യ || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    തിരികെ p;
}
#endif

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    ശൂന്യം
set_lang_var(ശൂന്യം)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# അല്ലെങ്കിൽ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# എങ്കിൽ defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# അല്ലെങ്കിൽ
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# അല്ലെങ്കിൽ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#എങ്കിൽ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    ശൂന്യം
init_locale(ശൂന്യം)
{
    setlocale(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# എങ്കിൽ defined(LC_NUMERIC)
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
	പൂർണ്ണസംഖ്യ	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	എങ്കിൽ (p != ശൂന്യ && *p != NUL)
	{
	    vim_snprintf((അക്ഷരം *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (അക്ഷരം *)NameBuff);
	}
	എങ്കിൽ (mustfree)
	    vim_free(p);
	textdomain(VIMPACKAGE);
    }
# endif
}

/*
 * ":language":  Set the language (locale).
 */
    ശൂന്യം
ex_language(exarg_T *eap)
{
    അക്ഷരം	*loc;
    char_u	*p;
    char_u	*name;
    പൂർണ്ണസംഖ്യ		what = LC_ALL;
    അക്ഷരം	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# അല്ലെങ്കിൽ
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    എങ്കിൽ ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	എങ്കിൽ (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    എങ്കിൽ (*name == NUL)
    {
# ifndef LC_MESSAGES
	എങ്കിൽ (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	അല്ലെങ്കിൽ
# endif
	    p = (char_u *)setlocale(what, ശൂന്യ);
	എങ്കിൽ (p == ശൂന്യ || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    അല്ലെങ്കിൽ
    {
# ifndef LC_MESSAGES
	എങ്കിൽ (what == VIM_LC_MESSAGES)
	    loc = "";
	അല്ലെങ്കിൽ
# endif
	{
	    loc = setlocale(what, (അക്ഷരം *)name);
# എങ്കിൽ defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	എങ്കിൽ (loc == ശൂന്യ)
	    semsg(_(e_cannot_set_language_to_str), name);
	അല്ലെങ്കിൽ
	{
# ifdef HAVE_NL_സന്ദേശം_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    ബാഹ്യ പൂർണ്ണസംഖ്യ nl_സന്ദേശം_cat_cntr;

	    ++nl_സന്ദേശം_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    എങ്കിൽ (dyn_libintl_nl_സന്ദേശം_cat_cntr != ശൂന്യ)
		++*dyn_libintl_nl_സന്ദേശം_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    എങ്കിൽ (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		എങ്കിൽ (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		എങ്കിൽ (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# അല്ലെങ്കിൽ
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

സ്ഥിതം char_u	**locales = ശൂന്യ;	// Array of all available locales

സ്ഥിതം പൂർണ്ണസംഖ്യ	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    സ്ഥിതം char_u **
find_locales(ശൂന്യം)
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
						    ശൂന്യ, SHELL_SILENT, ശൂന്യ);
# അല്ലെങ്കിൽ
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	പൂർണ്ണസംഖ്യ		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      ശൂന്യ, options, WILD_ALL);
	ExpandCleanup(&xpc);
	എങ്കിൽ (locale_list == ശൂന്യ)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	ആകുമ്പോൾ (p != ശൂന്യ && *p != NUL)
	{
	    എങ്കിൽ (*p == '\n')
		നിർത്തുക;
	    എങ്കിൽ (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    എങ്കിൽ (locale_list == ശൂന്യ)
	തിരികെ ശൂന്യ;
    ga_init2(&locales_ga, വലുപ്പം(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((അക്ഷരം *)locale_list, "\n");

    ആകുമ്പോൾ (loc != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ ignore = FALSE;

# ifdef MSWIN
	എങ്കിൽ (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	എങ്കിൽ (vim_strchr(loc, '.') != ശൂന്യ)
	    ignore = TRUE;
# endif
	എങ്കിൽ (!ignore)
	{
	    എങ്കിൽ (ga_grow(&locales_ga, 1) == FAIL)
		നിർത്തുക;

	    loc = vim_strsave(loc);
	    എങ്കിൽ (loc == ശൂന്യ)
		നിർത്തുക;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(ശൂന്യ, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    എങ്കിൽ (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    എങ്കിൽ (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	തിരികെ ശൂന്യ;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = ശൂന്യ;
    തിരികെ (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    സ്ഥിതം ശൂന്യം
init_locales(ശൂന്യം)
{
    എങ്കിൽ (did_init_locales)
	തിരികെ;

    did_init_locales = TRUE;
    locales = find_locales();
}

# എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_locales(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ			i;

    എങ്കിൽ (locales == ശൂന്യ)
	തിരികെ;

    വേണ്ടി (i = 0; locales[i] != ശൂന്യ; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx == 0)
	തിരികെ (char_u *)"messages";
    എങ്കിൽ (idx == 1)
	തിരികെ (char_u *)"ctype";
    എങ്കിൽ (idx == 2)
	തിരികെ (char_u *)"time";
    എങ്കിൽ (idx == 3)
	തിരികെ (char_u *)"collate";

    init_locales();
    എങ്കിൽ (locales == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    init_locales();
    എങ്കിൽ (locales == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ locales[idx];
}

#endif
