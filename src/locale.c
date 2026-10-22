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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_EVAL) || defined(FEAT_MULTI_LANG))
# define HAVE_GET_LOCALE_VAL
    ସ୍ଥିର char_u *
get_locale_val(ପୂର୍ଣ୍ଣସଂଖ୍ୟା what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, ଖାଲି);

# ifdef MSWIN
    ଯଦି (loc != ଖାଲି)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	ଯଦି (p != ଖାଲି)
	{
	    loc = ++p;
	    ଯେତେବେଳେ (*p != NUL)	// remove trailing newline
	    {
		ଯଦି (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    ଭାଙ୍ଗ;
		}
		++p;
	    }
	}
    }
# endif

    ଫେରନ୍ତୁ loc;
}
#endif


#ifdef MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    ସ୍ଥିର char_u *
gettext_lang(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ସ୍ଥିର ଅକ୍ଷର *(mtable[]) = {
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
			ଖାଲି};

    ପାଇଁ (i = 0; mtable[i] != ଖାଲି; i += 2)
	ଯଦି (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    ଫେରନ୍ତୁ (char_u *)mtable[i + 1];
    ଫେରନ୍ତୁ name;
}
#endif

#ଯଦି defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_valid_mess_lang(char_u *lang)
{
    ଫେରନ୍ତୁ lang != ଖାଲି && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(ଶୂନ୍ୟ)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  ଯଦି defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  ଅନ୍ୟଥା
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    ଯଦି (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# ଅନ୍ୟଥା
    p = mch_getenv((char_u *)"LC_ALL");
    ଯଦି (!is_valid_mess_lang(p))
    {
	p = mch_getenv((char_u *)"LC_MESSAGES");
	ଯଦି (!is_valid_mess_lang(p))
	    p = mch_getenv((char_u *)"LANG");
    }
# endif
# ifdef MSWIN
    p = gettext_lang(p);
# endif
    ଫେରନ୍ତୁ is_valid_mess_lang(p) ? p : ଖାଲି;
}
#endif

// Complicated #if; matches with where get_mess_env() is used below.
#ଯଦି (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    ସ୍ଥିର char_u *
get_mess_env(ଶୂନ୍ୟ)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    ଯଦି (p != ଖାଲି && *p != NUL)
	ଫେରନ୍ତୁ p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    ଯଦି (p != ଖାଲି && *p != NUL)
	ଫେରନ୍ତୁ p;

    p = mch_getenv((char_u *)"LANG");
    ଯଦି (p != ଖାଲି && VIM_ISDIGIT(*p))
	p = ଖାଲି;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    ଯଦି (p == ଖାଲି || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    ଫେରନ୍ତୁ p;
}
#endif

#ଯଦି defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    ଶୂନ୍ୟ
set_lang_var(ଶୂନ୍ୟ)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# ଅନ୍ୟଥା
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# ଯଦି defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# ଅନ୍ୟଥା
    loc = get_mess_env();
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# ଅନ୍ୟଥା
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_COLLATE, loc, -1);
}
#endif

#ଯଦି defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    ଶୂନ୍ୟ
init_locale(ଶୂନ୍ୟ)
{
    setlocale(LC_ALL, "");

# ifdef FEAT_GUI_GTK
    // Tell Gtk not to change our locale settings.
    gtk_disable_setlocale();
# endif
# ଯଦି defined(LC_NUMERIC)
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	mustfree = FALSE;
	char_u	*p;

#  ifdef DYNAMIC_GETTEXT
	// Initialize the gettext library
	dyn_libintl_init();
#  endif
	// expand_env() doesn't work yet, because g_chartab[] is not
	// initialized yet, call vim_getenv() directly
	p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
	ଯଦି (p != ଖାଲି && *p != NUL)
	{
	    vim_snprintf((ଅକ୍ଷର *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (ଅକ୍ଷର *)NameBuff);
	}
	ଯଦି (mustfree)
	    vim_free(p);
	textdomain(VIMPACKAGE);
    }
# endif
}

/*
 * ":language":  Set the language (locale).
 */
    ଶୂନ୍ୟ
ex_language(exarg_T *eap)
{
    ଅକ୍ଷର	*loc;
    char_u	*p;
    char_u	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		what = LC_ALL;
    ଅକ୍ଷର	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# ଅନ୍ୟଥା
#  define VIM_LC_MESSAGES 6789
# endif

    name = eap->arg;

    // Check for "messages {name}", "ctype {name}" or "time {name}" argument.
    // Allow abbreviation, but require at least 3 characters to avoid
    // confusion with a two letter language name "me" or "ct".
    p = skiptowhite(eap->arg);
    ଯଦି ((*p == NUL || VIM_ISWHITE(*p)) && p - eap->arg >= 3)
    {
	ଯଦି (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
	    what = VIM_LC_MESSAGES;
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	ଅନ୍ୟଥା ଯଦି (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	ଅନ୍ୟଥା ଯଦି (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	ଅନ୍ୟଥା ଯଦି (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
	{
	    what = LC_COLLATE;
	    name = skipwhite(p);
	    whatstr = "collate ";
	}
    }

    ଯଦି (*name == NUL)
    {
# ifndef LC_MESSAGES
	ଯଦି (what == VIM_LC_MESSAGES)
	    p = get_mess_env();
	ଅନ୍ୟଥା
# endif
	    p = (char_u *)setlocale(what, ଖାଲି);
	ଯଦି (p == ଖାଲି || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    ଅନ୍ୟଥା
    {
# ifndef LC_MESSAGES
	ଯଦି (what == VIM_LC_MESSAGES)
	    loc = "";
	ଅନ୍ୟଥା
# endif
	{
	    loc = setlocale(what, (ଅକ୍ଷର *)name);
# ଯଦି defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	ଯଦି (loc == ଖାଲି)
	    semsg(_(e_cannot_set_language_to_str), name);
	ଅନ୍ୟଥା
	{
# ifdef HAVE_NL_ସନ୍ଦେଶ_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା nl_ସନ୍ଦେଶ_cat_cntr;

	    ++nl_ସନ୍ଦେଶ_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    ଯଦି (dyn_libintl_nl_ସନ୍ଦେଶ_cat_cntr != ଖାଲି)
		++*dyn_libintl_nl_ସନ୍ଦେଶ_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    ଯଦି (what != LC_TIME && what != LC_COLLATE)
	    {
		// Tell gettext() what to translate to.  It apparently doesn't
		// use the currently effective locale.  Also do this when
		// FEAT_GETTEXT isn't defined, so that shell commands use this
		// value.
		ଯଦି (what == LC_ALL)
		{
		    vim_setenv((char_u *)"LANG", name);
# ifdef MSWIN
		    // Apparently MS-Windows printf() may cause a crash when
		    // we give it 8-bit text while it's expecting text in the
		    // current locale.  This call avoids that.
		    setlocale(LC_CTYPE, "C");
# endif
		}
		ଯଦି (what != LC_CTYPE)
		{
		    char_u	*mname;
# ifdef MSWIN
		    mname = gettext_lang(name);
# ଅନ୍ୟଥା
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

ସ୍ଥିର char_u	**locales = ଖାଲି;	// Array of all available locales

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    ସ୍ଥିର char_u **
find_locales(ଶୂନ୍ୟ)
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
						    ଖାଲି, SHELL_SILENT, ଖାଲି);
# ଅନ୍ୟଥା
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      ଖାଲି, options, WILD_ALL);
	ExpandCleanup(&xpc);
	ଯଦି (locale_list == ଖାଲି)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	ଯେତେବେଳେ (p != ଖାଲି && *p != NUL)
	{
	    ଯଦି (*p == '\n')
		ଭାଙ୍ଗ;
	    ଯଦି (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    ଯଦି (locale_list == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ga_init2(&locales_ga, ଆକାର(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((ଅକ୍ଷର *)locale_list, "\n");

    ଯେତେବେଳେ (loc != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore = FALSE;

# ifdef MSWIN
	ଯଦି (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	ଯଦି (vim_strchr(loc, '.') != ଖାଲି)
	    ignore = TRUE;
# endif
	ଯଦି (!ignore)
	{
	    ଯଦି (ga_grow(&locales_ga, 1) == FAIL)
		ଭାଙ୍ଗ;

	    loc = vim_strsave(loc);
	    ଯଦି (loc == ଖାଲି)
		ଭାଙ୍ଗ;

	    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(ଖାଲି, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    ଯଦି (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_data)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    ଯଦି (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ((char_u **)locales_ga.ga_data)[locales_ga.ga_len] = ଖାଲି;
    ଫେରନ୍ତୁ (char_u **)locales_ga.ga_data;
}

/*
 * Lazy initialization of all available locales.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_locales(ଶୂନ୍ୟ)
{
    ଯଦି (did_init_locales)
	ଫେରନ୍ତୁ;

    did_init_locales = TRUE;
    locales = find_locales();
}

# ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_locales(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;

    ଯଦି (locales == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; locales[i] != ଖାଲି; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx == 0)
	ଫେରନ୍ତୁ (char_u *)"messages";
    ଯଦି (idx == 1)
	ଫେରନ୍ତୁ (char_u *)"ctype";
    ଯଦି (idx == 2)
	ଫେରନ୍ତୁ (char_u *)"time";
    ଯଦି (idx == 3)
	ଫେରନ୍ତୁ (char_u *)"collate";

    init_locales();
    ଯଦି (locales == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    init_locales();
    ଯଦି (locales == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ locales[idx];
}

#endif
