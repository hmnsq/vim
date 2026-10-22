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

#অন্তর্ভাব "vim.h"

#যদি (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    স্থির char_u *
get_locale_val(পূর্ণাঙ্ক what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, রিক্ত);

# ifdef MSWIN
    যদি (loc != রিক্ত)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	যদি (p != রিক্ত)
	{
	    loc = ++p;
	    যাবত্ (*p != NUL)	// remove trailing newline
	    {
		যদি (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    ভঙ্গ;
		}
		++p;
	    }
	}
    }
# endif

    প্রত্যাগম loc;
}
#endif


#ifdef MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    স্থির char_u *
gettext_lang(char_u *name)
{
    পূর্ণাঙ্ক		i;
    স্থির বর্ণ *(mtable[]) = {
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
			রিক্ত};

    প্রতি (i = 0; mtable[i] != রিক্ত; i += 2)
	যদি (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    প্রত্যাগম (char_u *)mtable[i + 1];
    প্রত্যাগম name;
}
#endif

#যদি defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    স্থির পূর্ণাঙ্ক
is_valid_mess_lang(char_u *lang)
{
    প্রত্যাগম lang != রিক্ত && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(শূন্য)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  যদি defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  অন্যথা
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    যদি (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# অন্যথা
    p = mch_getenv((char_u *)"LC_ALL");
    যদি (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	যদি (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    প্রত্যাগম is_valid_mess_lang(p) ? p : রিক্ত;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#যদি (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    স্থির char_u *
get_mess_env(শূন্য)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    যদি (p != রিক্ত && *p != NUL)
	প্রত্যাগম p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    যদি (p != রিক্ত && *p != NUL)
	প্রত্যাগম p;

    p = mch_getenv((char_u *)"LANG");
    যদি (p != রিক্ত && VIM_ISDIGIT(*p))
	p = রিক্ত;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    যদি (p == রিক্ত || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    প্রত্যাগম p;
}
#endif

#যদি defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    শূন্য
set_lang_var(শূন্য)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# অন্যথা
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# যদি defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# অন্যথা
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# অন্যথা
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    শূন্য
init_locale(শূন্য)
{
    setlocale(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# যদি defined(LC_NUMERIC)
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
	পূর্ণাঙ্ক	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	যদি (p != রিক্ত && *p != NUL)
	{
	    vim_snprintf((বর্ণ *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (বর্ণ *)NameBuff);
	}
	যদি (mustfree)
	    vim_free(p);
	textdomain(VIMPACKAGE);
    }
# endif
}

/*
 * ":language":  Set the language (locale).
 */
    শূন্য
ex_language(exarg_T *eap)
{
    বর্ণ	*loc;
    char_u	*p;
    char_u	*name;
    পূর্ণাঙ্ক		what = LC_ALL;
    বর্ণ	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# অন্যথা
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    যদি ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	যদি (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	অন্যথা যদি (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	অন্যথা যদি (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	অন্যথা যদি (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    যদি (*name == NUL)
    {
# ifndef LC_MESSAGES
	যদি (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	অন্যথা
# endif
	    p = (char_u *)setlocale(what, রিক্ত);
	যদি (p == রিক্ত || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    অন্যথা
    {
# ifndef LC_MESSAGES
	যদি (what == VIM_LC_MESSAGES)
	    loc = "";
	অন্যথা
# endif
	{
	    loc = setlocale(what, (বর্ণ *)name);
# যদি defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	যদি (loc == রিক্ত)
	    semsg(_(e_cannot_set_language_to_str), name);
	অন্যথা
	{
# ifdef HAVE_NL_বার্তা_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    বাহ্য পূর্ণাঙ্ক nl_বার্তা_cat_cntr;

	    ++nl_বার্তা_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    যদি (dyn_libintl_nl_বার্তা_cat_cntr != রিক্ত)
		++*dyn_libintl_nl_বার্তা_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    যদি (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		যদি (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		যদি (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# অন্যথা
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

স্থির char_u	**locales = রিক্ত;	// Array of all available locales

স্থির পূর্ণাঙ্ক	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    স্থির char_u **
find_locales(শূন্য)
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
						    রিক্ত, SHELL_SILENT, রিক্ত);
# অন্যথা
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	পূর্ণাঙ্ক		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      রিক্ত, options, WILD_ALL);
	ExpandCleanup(&xpc);
	যদি (locale_list == রিক্ত)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	যাবত্ (p != রিক্ত && *p != NUL)
	{
	    যদি (*p == '\n')
		ভঙ্গ;
	    যদি (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    যদি (locale_list == রিক্ত)
	প্রত্যাগম রিক্ত;
    ga_init2(&locales_ga, পরিমাণ(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((বর্ণ *)locale_list, "\n");

    যাবত্ (loc != রিক্ত)
    {
	পূর্ণাঙ্ক ignore = FALSE;

# ifdef MSWIN
	যদি (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	যদি (vim_strchr(loc, '.') != রিক্ত)
	    ignore = TRUE;
# endif
	যদি (!ignore)
	{
	    যদি (ga_grow(&locales_ga, 1) == FAIL)
		ভঙ্গ;

	    loc = vim_strsave(loc);
	    যদি (loc == রিক্ত)
		ভঙ্গ;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(রিক্ত, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    যদি (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    যদি (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	প্রত্যাগম রিক্ত;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = রিক্ত;
    প্রত্যাগম (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    স্থির শূন্য
init_locales(শূন্য)
{
    যদি (did_init_locales)
	প্রত্যাগম;

    did_init_locales = TRUE;
    locales = find_locales();
}

# যদি defined(EXITFREE)
    শূন্য
free_locales(শূন্য)
{
    পূর্ণাঙ্ক			i;

    যদি (locales == রিক্ত)
	প্রত্যাগম;

    প্রতি (i = 0; locales[i] != রিক্ত; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx == 0)
	প্রত্যাগম (char_u *)"messages";
    যদি (idx == 1)
	প্রত্যাগম (char_u *)"ctype";
    যদি (idx == 2)
	প্রত্যাগম (char_u *)"time";
    যদি (idx == 3)
	প্রত্যাগম (char_u *)"collate";

    init_locales();
    যদি (locales == রিক্ত)
	প্রত্যাগম রিক্ত;
    প্রত্যাগম locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    init_locales();
    যদি (locales == রিক্ত)
	প্রত্যাগম রিক্ত;
    প্রত্যাগম locales[idx];
}

#endif
