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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    ਸਥਿਰ char_u *
get_locale_val(ਪੂਰਨ what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, ਨਿਰਾ);

# ifdef MSWIN
    ਜੇ (loc != ਨਿਰਾ)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	ਜੇ (p != ਨਿਰਾ)
	{
	    loc = ++p;
	    ਜਦੋਂ (*p != NUL)	// remove trailing newline
	    {
		ਜੇ (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    ਰੋਕੋ;
		}
		++p;
	    }
	}
    }
# endif

    ਵਾਪਸ loc;
}
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    ਸਥਿਰ char_u *
gettext_lang(char_u *name)
{
    ਪੂਰਨ		i;
    ਸਥਿਰ ਅੱਖਰ *(mtable[]) = {
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
			ਨਿਰਾ};

    ਲਈ (i = 0; mtable[i] != ਨਿਰਾ; i += 2)
	ਜੇ (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    ਵਾਪਸ (char_u *)mtable[i + 1];
    ਵਾਪਸ name;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    ਸਥਿਰ ਪੂਰਨ
is_valid_mess_lang(char_u *lang)
{
    ਵਾਪਸ lang != ਨਿਰਾ && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(ਖਾਲੀ)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  ਜੇ defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  ਨਹੀਂਤਾਂ
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    ਜੇ (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# ਨਹੀਂਤਾਂ
    p = mch_getenv((char_u *)"LC_ALL");
    ਜੇ (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	ਜੇ (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    ਵਾਪਸ is_valid_mess_lang(p) ? p : ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

// Complicated #if; matches with where get_mess_env() is used below.
#ਜੇ (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    ਸਥਿਰ char_u *
get_mess_env(ਖਾਲੀ)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    ਜੇ (p != ਨਿਰਾ && *p != NUL)
	ਵਾਪਸ p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    ਜੇ (p != ਨਿਰਾ && *p != NUL)
	ਵਾਪਸ p;

    p = mch_getenv((char_u *)"LANG");
    ਜੇ (p != ਨਿਰਾ && VIM_ISDIGIT(*p))
	p = ਨਿਰਾ;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    ਜੇ (p == ਨਿਰਾ || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    ਵਾਪਸ p;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    ਖਾਲੀ
ਸੈੱਟ_lang_var(ਖਾਲੀ)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# ਨਹੀਂਤਾਂ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    ਸੈੱਟ_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# ਜੇ defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# ਨਹੀਂਤਾਂ
    loc = get_mess_env();
# endif
    ਸੈੱਟ_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_ਸਮਾਂ);
# endif
    ਸੈੱਟ_vim_var_string(VV_LC_ਸਮਾਂ, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# ਨਹੀਂਤਾਂ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    ਸੈੱਟ_vim_var_string(VV_COLLATE, loc, -1);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    ਖਾਲੀ
init_locale(ਖਾਲੀ)
{
    setlocale(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# ਜੇ defined(LC_NUMERIC)
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
	ਪੂਰਨ	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	ਜੇ (p != ਨਿਰਾ && *p != NUL)
	{
	    vim_snprintf((ਅੱਖਰ *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (ਅੱਖਰ *)NameBuff);
	}
	ਜੇ (mustfree)
	    vim_free(p);
	textdomain(VIMPACKAGE);
    }
# endif
}

/*
 * ":language":  Set the language (locale).
 */
    ਖਾਲੀ
ex_language(exarg_T *eap)
{
    ਅੱਖਰ	*loc;
    char_u	*p;
    char_u	*name;
    ਪੂਰਨ		what = LC_ALL;
    ਅੱਖਰ	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# ਨਹੀਂਤਾਂ
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    ਜੇ ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	ਜੇ (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_ਸਮਾਂ;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    ਜੇ (*name == NUL)
    {
# ifndef LC_MESSAGES
	ਜੇ (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	ਨਹੀਂਤਾਂ
# endif
	    p = (char_u *)setlocale(what, ਨਿਰਾ);
	ਜੇ (p == ਨਿਰਾ || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    ਨਹੀਂਤਾਂ
    {
# ifndef LC_MESSAGES
	ਜੇ (what == VIM_LC_MESSAGES)
	    loc = "";
	ਨਹੀਂਤਾਂ
# endif
	{
	    loc = setlocale(what, (ਅੱਖਰ *)name);
# ਜੇ defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	ਜੇ (loc == ਨਿਰਾ)
	    semsg(_(e_cannot_ਸੈੱਟ_language_ਮੰਜ਼ਿਲ_str), name);
	ਨਹੀਂਤਾਂ
	{
# ifdef HAVE_NL_ਸੁਨੇਹਾ_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    ਬਾਹਰੀ ਪੂਰਨ nl_ਸੁਨੇਹਾ_cat_cntr;

	    ++nl_ਸੁਨੇਹਾ_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    ਜੇ (dyn_libintl_nl_ਸੁਨੇਹਾ_cat_cntr != ਨਿਰਾ)
		++*dyn_libintl_nl_ਸੁਨੇਹਾ_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    ਜੇ (what != LC_ਸਮਾਂ && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		ਜੇ (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		ਜੇ (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# ਨਹੀਂਤਾਂ
		    mname = name;
# endif
		    // Clear $LANGUAGE because GNU gettext uses it.
		    vim_setenv((char_u *)"LANGUAGE", (char_u *)"");
		    vim_setenv((char_u *)"LC_MESSAGES", mname);
# ifdef FEAT_MULTI_LANG
		    ਸੈੱਟ_helplang_default(mname);
# endif
		}
	    }

# ifdef FEAT_EVAL
	    // Set v:lang, v:lc_time, v:collate and v:ctype to the final result.
	    ਸੈੱਟ_lang_var();
# endif
	    maketitle();
	}
    }
}

ਸਥਿਰ char_u	**locales = ਨਿਰਾ;	// Array of all available locales

ਸਥਿਰ ਪੂਰਨ	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    ਸਥਿਰ char_u **
ਲੱਭੋ_locales(ਖਾਲੀ)
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
						    ਨਿਰਾ, SHELL_SILENT, ਨਿਰਾ);
# ਨਹੀਂਤਾਂ
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	ਪੂਰਨ		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      ਨਿਰਾ, options, WILD_ALL);
	ExpandCleanup(&xpc);
	ਜੇ (locale_list == ਨਿਰਾ)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	ਜਦੋਂ (p != ਨਿਰਾ && *p != NUL)
	{
	    ਜੇ (*p == '\n')
		ਰੋਕੋ;
	    ਜੇ (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    ਜੇ (locale_list == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ga_init2(&locales_ga, ਆਕਾਰ(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((ਅੱਖਰ *)locale_list, "\n");

    ਜਦੋਂ (loc != ਨਿਰਾ)
    {
	ਪੂਰਨ ignore = FALSE;

# ifdef MSWIN
	ਜੇ (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	ਜੇ (vim_strchr(loc, '.') != ਨਿਰਾ)
	    ignore = TRUE;
# endif
	ਜੇ (!ignore)
	{
	    ਜੇ (ga_grow(&locales_ga, 1) == FAIL)
		ਰੋਕੋ;

	    loc = vim_strsave(loc);
	    ਜੇ (loc == ਨਿਰਾ)
		ਰੋਕੋ;

	    ((char_u **)locales_ga.ga_ਅੰਕੜੇ)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(ਨਿਰਾ, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    ਜੇ (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_ਅੰਕੜੇ)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    ਜੇ (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ((char_u **)locales_ga.ga_ਅੰਕੜੇ)[locales_ga.ga_len] = ਨਿਰਾ;
    ਵਾਪਸ (char_u **)locales_ga.ga_ਅੰਕੜੇ;
}

/*
 * Lazy initialization of all available locales.
 */
    ਸਥਿਰ ਖਾਲੀ
init_locales(ਖਾਲੀ)
{
    ਜੇ (did_init_locales)
	ਵਾਪਸ;

    did_init_locales = TRUE;
    locales = ਲੱਭੋ_locales();
}

# ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_locales(ਖਾਲੀ)
{
    ਪੂਰਨ			i;

    ਜੇ (locales == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (i = 0; locales[i] != ਨਿਰਾ; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx == 0)
	ਵਾਪਸ (char_u *)"messages";
    ਜੇ (idx == 1)
	ਵਾਪਸ (char_u *)"ctype";
    ਜੇ (idx == 2)
	ਵਾਪਸ (char_u *)"time";
    ਜੇ (idx == 3)
	ਵਾਪਸ (char_u *)"collate";

    init_locales();
    ਜੇ (locales == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    init_locales();
    ਜੇ (locales == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ locales[idx];
}

#ਜੇ_ਅੰਤ
