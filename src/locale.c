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

#அந்தர்பாவ "vim.h"

#எனில் (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    நிலையான char_u *
get_locale_val(முழு what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, வெற்று);

# ifdef MSWIN
    எனில் (loc != வெற்று)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	எனில் (p != வெற்று)
	{
	    loc = ++p;
	    போது (*p != NUL)	// remove trailing newline
	    {
		எனில் (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    நிறுத்து;
		}
		++p;
	    }
	}
    }
# endif

    திரும்பு loc;
}
#endif


#ifdef MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    நிலையான char_u *
gettext_lang(char_u *name)
{
    முழு		i;
    நிலையான எழுத்து *(mtable[]) = {
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
			வெற்று};

    க்கு (i = 0; mtable[i] != வெற்று; i += 2)
	எனில் (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    திரும்பு (char_u *)mtable[i + 1];
    திரும்பு name;
}
#endif

#எனில் defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    நிலையான முழு
is_valid_mess_lang(char_u *lang)
{
    திரும்பு lang != வெற்று && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(வெறுமை)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  எனில் defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  இல்லையெனில்
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    எனில் (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# இல்லையெனில்
    p = mch_getenv((char_u *)"LC_ALL");
    எனில் (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	எனில் (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    திரும்பு is_valid_mess_lang(p) ? p : வெற்று;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#எனில் (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    நிலையான char_u *
get_mess_env(வெறுமை)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    எனில் (p != வெற்று && *p != NUL)
	திரும்பு p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    எனில் (p != வெற்று && *p != NUL)
	திரும்பு p;

    p = mch_getenv((char_u *)"LANG");
    எனில் (p != வெற்று && VIM_ISDIGIT(*p))
	p = வெற்று;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    எனில் (p == வெற்று || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    திரும்பு p;
}
#endif

#எனில் defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    வெறுமை
set_lang_var(வெறுமை)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# இல்லையெனில்
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# எனில் defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# இல்லையெனில்
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# இல்லையெனில்
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#எனில் defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    வெறுமை
init_locale(வெறுமை)
{
    setlocale(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# எனில் defined(LC_NUMERIC)
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
	முழு	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	எனில் (p != வெற்று && *p != NUL)
	{
	    vim_snprintf((எழுத்து *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (எழுத்து *)NameBuff);
	}
	எனில் (mustfree)
	    vim_free(p);
	textdomain(VIMPACKAGE);
    }
# endif
}

/*
 * ":language":  Set the language (locale).
 */
    வெறுமை
ex_language(exarg_T *eap)
{
    எழுத்து	*loc;
    char_u	*p;
    char_u	*name;
    முழு		what = LC_ALL;
    எழுத்து	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# இல்லையெனில்
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    எனில் ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	எனில் (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	இல்லையெனில் எனில் (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	இல்லையெனில் எனில் (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	இல்லையெனில் எனில் (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    எனில் (*name == NUL)
    {
# ifndef LC_MESSAGES
	எனில் (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	இல்லையெனில்
# endif
	    p = (char_u *)setlocale(what, வெற்று);
	எனில் (p == வெற்று || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    இல்லையெனில்
    {
# ifndef LC_MESSAGES
	எனில் (what == VIM_LC_MESSAGES)
	    loc = "";
	இல்லையெனில்
# endif
	{
	    loc = setlocale(what, (எழுத்து *)name);
# எனில் defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	எனில் (loc == வெற்று)
	    semsg(_(e_cannot_set_language_to_str), name);
	இல்லையெனில்
	{
# ifdef HAVE_NL_செய்தி_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    வெளி முழு nl_செய்தி_cat_cntr;

	    ++nl_செய்தி_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    எனில் (dyn_libintl_nl_செய்தி_cat_cntr != வெற்று)
		++*dyn_libintl_nl_செய்தி_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    எனில் (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		எனில் (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		எனில் (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# இல்லையெனில்
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

நிலையான char_u	**locales = வெற்று;	// Array of all available locales

நிலையான முழு	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    நிலையான char_u **
find_locales(வெறுமை)
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
						    வெற்று, SHELL_SILENT, வெற்று);
# இல்லையெனில்
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	முழு		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      வெற்று, options, WILD_ALL);
	ExpandCleanup(&xpc);
	எனில் (locale_list == வெற்று)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	போது (p != வெற்று && *p != NUL)
	{
	    எனில் (*p == '\n')
		நிறுத்து;
	    எனில் (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    எனில் (locale_list == வெற்று)
	திரும்பு வெற்று;
    ga_init2(&locales_ga, அளவு(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((எழுத்து *)locale_list, "\n");

    போது (loc != வெற்று)
    {
	முழு ignore = FALSE;

# ifdef MSWIN
	எனில் (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	எனில் (vim_strchr(loc, '.') != வெற்று)
	    ignore = TRUE;
# endif
	எனில் (!ignore)
	{
	    எனில் (ga_grow(&locales_ga, 1) == FAIL)
		நிறுத்து;

	    loc = vim_strsave(loc);
	    எனில் (loc == வெற்று)
		நிறுத்து;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(வெற்று, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    எனில் (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    எனில் (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	திரும்பு வெற்று;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = வெற்று;
    திரும்பு (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    நிலையான வெறுமை
init_locales(வெறுமை)
{
    எனில் (did_init_locales)
	திரும்பு;

    did_init_locales = TRUE;
    locales = find_locales();
}

# எனில் defined(EXITFREE)
    வெறுமை
free_locales(வெறுமை)
{
    முழு			i;

    எனில் (locales == வெற்று)
	திரும்பு;

    க்கு (i = 0; locales[i] != வெற்று; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx == 0)
	திரும்பு (char_u *)"messages";
    எனில் (idx == 1)
	திரும்பு (char_u *)"ctype";
    எனில் (idx == 2)
	திரும்பு (char_u *)"time";
    எனில் (idx == 3)
	திரும்பு (char_u *)"collate";

    init_locales();
    எனில் (locales == வெற்று)
	திரும்பு வெற்று;
    திரும்பு locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, முழு idx)
{
    init_locales();
    எனில் (locales == வெற்று)
	திரும்பு வெற்று;
    திரும்பு locales[idx];
}

#endif
