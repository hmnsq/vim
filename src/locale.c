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

#ضمن "vim.h"

#ܐܢ (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    ساكن char_u *
get_locale_val(صحيح what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, عدم);

# ifdef MSWIN
    ܐܢ (loc != عدم)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	ܐܢ (p != عدم)
	{
	    loc = ++p;
	    ܟܡܐ (*p != NUL)	// remove trailing newline
	    {
		ܐܢ (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    اكسر;
		}
		++p;
	    }
	}
    }
# endif

    ܦܢܐ loc;
}
#endif


#ifdef MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    ساكن char_u *
gettext_lang(char_u *name)
{
    صحيح		i;
    ساكن حرف *(mtable[]) = {
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
			عدم};

    لكل (i = 0; mtable[i] != عدم; i += 2)
	ܐܢ (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    ܦܢܐ (char_u *)mtable[i + 1];
    ܦܢܐ name;
}
#endif

#ܐܢ defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    ساكن صحيح
is_valid_mess_lang(char_u *lang)
{
    ܦܢܐ lang != عدم && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(فراغ)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  ܐܢ defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  ܐܚܪܢܐ
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    ܐܢ (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# ܐܚܪܢܐ
    p = mch_getenv((char_u *)"LC_ALL");
    ܐܢ (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	ܐܢ (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    ܦܢܐ is_valid_mess_lang(p) ? p : عدم;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#ܐܢ (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    ساكن char_u *
get_mess_env(فراغ)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    ܐܢ (p != عدم && *p != NUL)
	ܦܢܐ p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    ܐܢ (p != عدم && *p != NUL)
	ܦܢܐ p;

    p = mch_getenv((char_u *)"LANG");
    ܐܢ (p != عدم && VIM_ISDIGIT(*p))
	p = عدم;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    ܐܢ (p == عدم || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    ܦܢܐ p;
}
#endif

#ܐܢ defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    فراغ
set_lang_var(فراغ)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# ܐܚܪܢܐ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# ܐܢ defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# ܐܚܪܢܐ
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# ܐܚܪܢܐ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#ܐܢ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    فراغ
init_locale(فراغ)
{
    setlocale(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# ܐܢ defined(LC_NUMERIC)
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
	صحيح	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	ܐܢ (p != عدم && *p != NUL)
	{
	    vim_snprintf((حرف *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (حرف *)NameBuff);
	}
	ܐܢ (mustfree)
	    vim_free(p);
	textdomain(VIMPACKAGE);
    }
# endif
}

/*
 * ":language":  Set the language (locale).
 */
    فراغ
ex_language(exarg_T *eap)
{
    حرف	*loc;
    char_u	*p;
    char_u	*name;
    صحيح		what = LC_ALL;
    حرف	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# ܐܚܪܢܐ
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    ܐܢ ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	ܐܢ (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	ܐܚܪܢܐ ܐܢ (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	ܐܚܪܢܐ ܐܢ (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	ܐܚܪܢܐ ܐܢ (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    ܐܢ (*name == NUL)
    {
# ifndef LC_MESSAGES
	ܐܢ (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	ܐܚܪܢܐ
# endif
	    p = (char_u *)setlocale(what, عدم);
	ܐܢ (p == عدم || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    ܐܚܪܢܐ
    {
# ifndef LC_MESSAGES
	ܐܢ (what == VIM_LC_MESSAGES)
	    loc = "";
	ܐܚܪܢܐ
# endif
	{
	    loc = setlocale(what, (حرف *)name);
# ܐܢ defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	ܐܢ (loc == عدم)
	    semsg(_(e_cannot_set_language_to_str), name);
	ܐܚܪܢܐ
	{
# ifdef HAVE_NL_MSG_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    خارجي صحيح _nl_msg_cat_cntr;

	    ++_nl_msg_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    ܐܢ (dyn_libintl_nl_msg_cat_cntr != عدم)
		++*dyn_libintl_nl_msg_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    ܐܢ (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		ܐܢ (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		ܐܢ (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# ܐܚܪܢܐ
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

ساكن char_u	**locales = عدم;	// Array of all available locales

ساكن صحيح	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    ساكن char_u **
find_locales(فراغ)
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
						    عدم, SHELL_SILENT, عدم);
# ܐܚܪܢܐ
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	صحيح		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      عدم, options, WILD_ALL);
	ExpandCleanup(&xpc);
	ܐܢ (locale_list == عدم)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	ܟܡܐ (p != عدم && *p != NUL)
	{
	    ܐܢ (*p == '\n')
		اكسر;
	    ܐܢ (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    ܐܢ (locale_list == عدم)
	ܦܢܐ عدم;
    ga_init2(&locales_ga, حجم(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((حرف *)locale_list, "\n");

    ܟܡܐ (loc != عدم)
    {
	صحيح ignore = FALSE;

# ifdef MSWIN
	ܐܢ (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	ܐܢ (vim_strchr(loc, '.') != عدم)
	    ignore = TRUE;
# endif
	ܐܢ (!ignore)
	{
	    ܐܢ (ga_grow(&locales_ga, 1) == FAIL)
		اكسر;

	    loc = vim_strsave(loc);
	    ܐܢ (loc == عدم)
		اكسر;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(عدم, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    ܐܢ (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    ܐܢ (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	ܦܢܐ عدم;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = عدم;
    ܦܢܐ (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    ساكن فراغ
init_locales(فراغ)
{
    ܐܢ (did_init_locales)
	ܦܢܐ;

    did_init_locales = TRUE;
    locales = find_locales();
}

# ܐܢ defined(EXITFREE)
    فراغ
free_locales(فراغ)
{
    صحيح			i;

    ܐܢ (locales == عدم)
	ܦܢܐ;

    لكل (i = 0; locales[i] != عدم; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp UNUSED, صحيح idx)
{
    ܐܢ (idx == 0)
	ܦܢܐ (char_u *)"messages";
    ܐܢ (idx == 1)
	ܦܢܐ (char_u *)"ctype";
    ܐܢ (idx == 2)
	ܦܢܐ (char_u *)"time";
    ܐܢ (idx == 3)
	ܦܢܐ (char_u *)"collate";

    init_locales();
    ܐܢ (locales == عدم)
	ܦܢܐ عدم;
    ܦܢܐ locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, صحيح idx)
{
    init_locales();
    ܐܢ (locales == عدم)
	ܦܢܐ عدم;
    ܦܢܐ locales[idx];
}

#endif
