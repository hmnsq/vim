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

#شامل "vim.h"

#اگر (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    ایستا char_u *
get_locale_val(صحیح what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, پوچ);

# ifdef MSWIN
    اگر (loc != پوچ)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	اگر (p != پوچ)
	{
	    loc = ++p;
	    وقتی (*p != NUL)	// remove trailing newline
	    {
		اگر (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    بشکن;
		}
		++p;
	    }
	}
    }
# endif

    بازگشت loc;
}
#endif


#ifdef MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    ایستا char_u *
gettext_lang(char_u *name)
{
    صحیح		i;
    ایستا نویسه *(mtable[]) = {
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
			پوچ};

    برای (i = 0; mtable[i] != پوچ; i += 2)
	اگر (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    بازگشت (char_u *)mtable[i + 1];
    بازگشت name;
}
#endif

#اگر defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    ایستا صحیح
is_valid_mess_lang(char_u *lang)
{
    بازگشت lang != پوچ && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(تهی)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  اگر defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  وگرنه
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    اگر (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# وگرنه
    p = mch_getenv((char_u *)"LC_ALL");
    اگر (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	اگر (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    بازگشت is_valid_mess_lang(p) ? p : پوچ;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#اگر (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    ایستا char_u *
get_mess_env(تهی)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    اگر (p != پوچ && *p != NUL)
	بازگشت p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    اگر (p != پوچ && *p != NUL)
	بازگشت p;

    p = mch_getenv((char_u *)"LANG");
    اگر (p != پوچ && VIM_ISDIGIT(*p))
	p = پوچ;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    اگر (p == پوچ || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    بازگشت p;
}
#endif

#اگر defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    تهی
set_lang_var(تهی)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# وگرنه
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# اگر defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# وگرنه
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# وگرنه
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    تهی
init_locale(تهی)
{
    setlocale(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# اگر defined(LC_NUMERIC)
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
	صحیح	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	اگر (p != پوچ && *p != NUL)
	{
	    vim_snprintf((نویسه *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (نویسه *)NameBuff);
	}
	اگر (mustfree)
	    vim_free(p);
	textdomain(VIMPACKAGE);
    }
# endif
}

/*
 * ":language":  Set the language (locale).
 */
    تهی
ex_language(exarg_T *eap)
{
    نویسه	*loc;
    char_u	*p;
    char_u	*name;
    صحیح		what = LC_ALL;
    نویسه	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# وگرنه
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    اگر ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	اگر (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	وگرنه اگر (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	وگرنه اگر (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	وگرنه اگر (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    اگر (*name == NUL)
    {
# ifndef LC_MESSAGES
	اگر (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	وگرنه
# endif
	    p = (char_u *)setlocale(what, پوچ);
	اگر (p == پوچ || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    وگرنه
    {
# ifndef LC_MESSAGES
	اگر (what == VIM_LC_MESSAGES)
	    loc = "";
	وگرنه
# endif
	{
	    loc = setlocale(what, (نویسه *)name);
# اگر defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	اگر (loc == پوچ)
	    semsg(_(e_cannot_set_language_to_str), name);
	وگرنه
	{
# ifdef HAVE_NL_پیام_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    بیرونی صحیح nl_پیام_cat_cntr;

	    ++nl_پیام_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    اگر (dyn_libintl_nl_پیام_cat_cntr != پوچ)
		++*dyn_libintl_nl_پیام_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    اگر (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		اگر (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		اگر (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# وگرنه
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

ایستا char_u	**locales = پوچ;	// Array of all available locales

ایستا صحیح	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    ایستا char_u **
find_locales(تهی)
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
						    پوچ, SHELL_SILENT, پوچ);
# وگرنه
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	صحیح		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      پوچ, options, WILD_ALL);
	ExpandCleanup(&xpc);
	اگر (locale_list == پوچ)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	وقتی (p != پوچ && *p != NUL)
	{
	    اگر (*p == '\n')
		بشکن;
	    اگر (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    اگر (locale_list == پوچ)
	بازگشت پوچ;
    ga_init2(&locales_ga, اندازه(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((نویسه *)locale_list, "\n");

    وقتی (loc != پوچ)
    {
	صحیح ignore = FALSE;

# ifdef MSWIN
	اگر (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	اگر (vim_strchr(loc, '.') != پوچ)
	    ignore = TRUE;
# endif
	اگر (!ignore)
	{
	    اگر (ga_grow(&locales_ga, 1) == FAIL)
		بشکن;

	    loc = vim_strsave(loc);
	    اگر (loc == پوچ)
		بشکن;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(پوچ, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    اگر (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    اگر (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	بازگشت پوچ;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = پوچ;
    بازگشت (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    ایستا تهی
init_locales(تهی)
{
    اگر (did_init_locales)
	بازگشت;

    did_init_locales = TRUE;
    locales = find_locales();
}

# اگر defined(EXITFREE)
    تهی
free_locales(تهی)
{
    صحیح			i;

    اگر (locales == پوچ)
	بازگشت;

    برای (i = 0; locales[i] != پوچ; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx == 0)
	بازگشت (char_u *)"messages";
    اگر (idx == 1)
	بازگشت (char_u *)"ctype";
    اگر (idx == 2)
	بازگشت (char_u *)"time";
    اگر (idx == 3)
	بازگشت (char_u *)"collate";

    init_locales();
    اگر (locales == پوچ)
	بازگشت پوچ;
    بازگشت locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, صحیح idx)
{
    init_locales();
    اگر (locales == پوچ)
	بازگشت پوچ;
    بازگشت locales[idx];
}

#endif
