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

#אם (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    ساكن char_u *
get_locale_val(صحيح what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, عدم);

# ifdef MSWIN
    אם (loc != عدم)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	אם (p != عدم)
	{
	    loc = ++p;
	    כלעוד (*p != NUL)	// remove trailing newline
	    {
		אם (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    اكسر;
		}
		++p;
	    }
	}
    }
# endif

    החזר loc;
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

    עבור (i = 0; mtable[i] != عدم; i += 2)
	אם (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    החזר (char_u *)mtable[i + 1];
    החזר name;
}
#endif

#אם defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    ساكن صحيح
is_valid_mess_lang(char_u *lang)
{
    החזר lang != عدم && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
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
#  אם defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  אחרת
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    אם (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# אחרת
    p = mch_getenv((char_u *)"LC_ALL");
    אם (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	אם (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    החזר is_valid_mess_lang(p) ? p : عدم;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#אם (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
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
    אם (p != عدم && *p != NUL)
	החזר p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    אם (p != عدم && *p != NUL)
	החזר p;

    p = mch_getenv((char_u *)"LANG");
    אם (p != عدم && VIM_ISDIGIT(*p))
	p = عدم;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    אם (p == عدم || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    החזר p;
}
#endif

#אם defined(FEAT_EVAL)

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
# אחרת
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# אם defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# אחרת
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# אחרת
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#אם defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
# אם defined(LC_NUMERIC)
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
	אם (p != عدم && *p != NUL)
	{
	    vim_snprintf((حرف *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (حرف *)NameBuff);
	}
	אם (mustfree)
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
# אחרת
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    אם ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	אם (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	אחרת אם (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	אחרת אם (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	אחרת אם (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    אם (*name == NUL)
    {
# ifndef LC_MESSAGES
	אם (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	אחרת
# endif
	    p = (char_u *)setlocale(what, عدم);
	אם (p == عدم || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    אחרת
    {
# ifndef LC_MESSAGES
	אם (what == VIM_LC_MESSAGES)
	    loc = "";
	אחרת
# endif
	{
	    loc = setlocale(what, (حرف *)name);
# אם defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	אם (loc == عدم)
	    semsg(_(e_cannot_set_language_to_str), name);
	אחרת
	{
# ifdef HAVE_NL_MSG_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    خارجي صحيح _nl_msg_cat_cntr;

	    ++_nl_msg_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    אם (dyn_libintl_nl_msg_cat_cntr != عدم)
		++*dyn_libintl_nl_msg_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    אם (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		אם (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		אם (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# אחרת
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
# אחרת
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
	אם (locale_list == عدم)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	כלעוד (p != عدم && *p != NUL)
	{
	    אם (*p == '\n')
		اكسر;
	    אם (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    אם (locale_list == عدم)
	החזר عدم;
    ga_init2(&locales_ga, حجم(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((حرف *)locale_list, "\n");

    כלעוד (loc != عدم)
    {
	صحيح ignore = FALSE;

# ifdef MSWIN
	אם (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	אם (vim_strchr(loc, '.') != عدم)
	    ignore = TRUE;
# endif
	אם (!ignore)
	{
	    אם (ga_grow(&locales_ga, 1) == FAIL)
		اكسر;

	    loc = vim_strsave(loc);
	    אם (loc == عدم)
		اكسر;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(عدم, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    אם (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    אם (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	החזר عدم;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = عدم;
    החזר (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    ساكن فراغ
init_locales(فراغ)
{
    אם (did_init_locales)
	החזר;

    did_init_locales = TRUE;
    locales = find_locales();
}

# אם defined(EXITFREE)
    فراغ
free_locales(فراغ)
{
    صحيح			i;

    אם (locales == عدم)
	החזר;

    עבור (i = 0; locales[i] != عدم; i++)
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
    אם (idx == 0)
	החזר (char_u *)"messages";
    אם (idx == 1)
	החזר (char_u *)"ctype";
    אם (idx == 2)
	החזר (char_u *)"time";
    אם (idx == 3)
	החזר (char_u *)"collate";

    init_locales();
    אם (locales == عدم)
	החזר عدم;
    החזר locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, صحيح idx)
{
    init_locales();
    אם (locales == عدم)
	החזר عدم;
    החזר locales[idx];
}

#endif
