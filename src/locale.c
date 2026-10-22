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

#અન્તર્ભાવ "vim.h"

#જો (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    સ્થિર char_u *
get_locale_val(પૂર્ણાંક what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, ખાલી);

# ifdef MSWIN
    જો (loc != ખાલી)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	જો (p != ખાલી)
	{
	    loc = ++p;
	    જ્યારે (*p != NUL)	// remove trailing newline
	    {
		જો (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    તોડો;
		}
		++p;
	    }
	}
    }
# endif

    પાછા loc;
}
#endif


#ifdef MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    સ્થિર char_u *
gettext_lang(char_u *name)
{
    પૂર્ણાંક		i;
    સ્થિર અક્ષર *(mtable[]) = {
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
			ખાલી};

    માટે (i = 0; mtable[i] != ખાલી; i += 2)
	જો (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    પાછા (char_u *)mtable[i + 1];
    પાછા name;
}
#endif

#જો defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    સ્થિર પૂર્ણાંક
is_valid_mess_lang(char_u *lang)
{
    પાછા lang != ખાલી && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(શૂન્ય)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  જો defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  નહીંતર
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    જો (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# નહીંતર
    p = mch_getenv((char_u *)"LC_ALL");
    જો (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	જો (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    પાછા is_valid_mess_lang(p) ? p : ખાલી;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#જો (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    સ્થિર char_u *
get_mess_env(શૂન્ય)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    જો (p != ખાલી && *p != NUL)
	પાછા p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    જો (p != ખાલી && *p != NUL)
	પાછા p;

    p = mch_getenv((char_u *)"LANG");
    જો (p != ખાલી && VIM_ISDIGIT(*p))
	p = ખાલી;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    જો (p == ખાલી || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    પાછા p;
}
#endif

#જો defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    શૂન્ય
set_lang_var(શૂન્ય)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# નહીંતર
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# જો defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# નહીંતર
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# નહીંતર
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#જો defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    શૂન્ય
init_locale(શૂન્ય)
{
    setlocale(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# જો defined(LC_NUMERIC)
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
	પૂર્ણાંક	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	જો (p != ખાલી && *p != NUL)
	{
	    vim_snprintf((અક્ષર *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (અક્ષર *)NameBuff);
	}
	જો (mustfree)
	    vim_free(p);
	textdomain(VIMPACKAGE);
    }
# endif
}

/*
 * ":language":  Set the language (locale).
 */
    શૂન્ય
ex_language(exarg_T *eap)
{
    અક્ષર	*loc;
    char_u	*p;
    char_u	*name;
    પૂર્ણાંક		what = LC_ALL;
    અક્ષર	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# નહીંતર
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    જો ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	જો (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	નહીંતર જો (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	નહીંતર જો (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	નહીંતર જો (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    જો (*name == NUL)
    {
# ifndef LC_MESSAGES
	જો (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	નહીંતર
# endif
	    p = (char_u *)setlocale(what, ખાલી);
	જો (p == ખાલી || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    નહીંતર
    {
# ifndef LC_MESSAGES
	જો (what == VIM_LC_MESSAGES)
	    loc = "";
	નહીંતર
# endif
	{
	    loc = setlocale(what, (અક્ષર *)name);
# જો defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	જો (loc == ખાલી)
	    semsg(_(e_cannot_set_language_to_str), name);
	નહીંતર
	{
# ifdef HAVE_NL_સંદેશ_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    બાહ્ય પૂર્ણાંક nl_સંદેશ_cat_cntr;

	    ++nl_સંદેશ_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    જો (dyn_libintl_nl_સંદેશ_cat_cntr != ખાલી)
		++*dyn_libintl_nl_સંદેશ_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    જો (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		જો (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		જો (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# નહીંતર
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

સ્થિર char_u	**locales = ખાલી;	// Array of all available locales

સ્થિર પૂર્ણાંક	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    સ્થિર char_u **
find_locales(શૂન્ય)
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
						    ખાલી, SHELL_SILENT, ખાલી);
# નહીંતર
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	પૂર્ણાંક		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      ખાલી, options, WILD_ALL);
	ExpandCleanup(&xpc);
	જો (locale_list == ખાલી)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	જ્યારે (p != ખાલી && *p != NUL)
	{
	    જો (*p == '\n')
		તોડો;
	    જો (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    જો (locale_list == ખાલી)
	પાછા ખાલી;
    ga_init2(&locales_ga, કદ(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((અક્ષર *)locale_list, "\n");

    જ્યારે (loc != ખાલી)
    {
	પૂર્ણાંક ignore = FALSE;

# ifdef MSWIN
	જો (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	જો (vim_strchr(loc, '.') != ખાલી)
	    ignore = TRUE;
# endif
	જો (!ignore)
	{
	    જો (ga_grow(&locales_ga, 1) == FAIL)
		તોડો;

	    loc = vim_strsave(loc);
	    જો (loc == ખાલી)
		તોડો;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(ખાલી, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    જો (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    જો (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	પાછા ખાલી;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = ખાલી;
    પાછા (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    સ્થિર શૂન્ય
init_locales(શૂન્ય)
{
    જો (did_init_locales)
	પાછા;

    did_init_locales = TRUE;
    locales = find_locales();
}

# જો defined(EXITFREE)
    શૂન્ય
free_locales(શૂન્ય)
{
    પૂર્ણાંક			i;

    જો (locales == ખાલી)
	પાછા;

    માટે (i = 0; locales[i] != ખાલી; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx == 0)
	પાછા (char_u *)"messages";
    જો (idx == 1)
	પાછા (char_u *)"ctype";
    જો (idx == 2)
	પાછા (char_u *)"time";
    જો (idx == 3)
	પાછા (char_u *)"collate";

    init_locales();
    જો (locales == ખાલી)
	પાછા ખાલી;
    પાછા locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    init_locales();
    જો (locales == ખાલી)
	પાછા ખાલી;
    પાછા locales[idx];
}

#endif
