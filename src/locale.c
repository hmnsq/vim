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
    ساکن char_u *
get_locale_val(صحیح what)
{
    char_u	*loc;

    // Obtain the locale value from the libraries.
    loc = (char_u *)setlocale(what, تہی);

# ifdef MSWIN
    اگر (loc != تہی)
    {
	char_u	*p;

	// setocale() returns something like "LC_COLLATE=<name>;LC_..." when
	// one of the values (e.g., LC_CTYPE) differs.
	p = vim_strchr(loc, '=');
	اگر (p != تہی)
	{
	    loc = ++p;
	    جب (*p != NUL)	// remove trailing newline
	    {
		اگر (*p < ' ' || *p == ';')
		{
		    *p = NUL;
		    توڑو;
		}
		++p;
	    }
	}
    }
# endif

    واپس loc;
}
#اگر_ختم


#اگر_تعریف_شدہ MSWIN
/*
 * On MS-Windows locale names are strings like "German_Germany.1252", but
 * gettext expects "de".  Try to translate one into another here for a few
 * supported languages.
 */
    ساکن char_u *
gettext_lang(char_u *name)
{
    صحیح		i;
    ساکن حرف *(mtable[]) = {
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
			تہی};

    برائے (i = 0; mtable[i] != تہی; i += 2)
	اگر (STRNICMP(mtable[i], name, STRLEN(mtable[i])) == 0)
	    واپس (char_u *)mtable[i + 1];
    واپس name;
}
#اگر_ختم

#اگر defined(FEAT_MULTI_LANG)
/*
 * Return TRUE when "lang" starts with a valid language name.
 * Rejects NULL, empty string, "C", "C.UTF-8" and others.
 */
    ساکن صحیح
is_valid_mess_lang(char_u *lang)
{
    واپس lang != تہی && ASCII_ISALPHA(lang[0]) && ASCII_ISALPHA(lang[1]);
}

/*
 * Obtain the current messages language.  Used to set the default for
 * 'helplang'.  May return NULL or an empty string.
 */
    char_u *
get_mess_lang(خالی)
{
    char_u *p;

# ifdef HAVE_GET_LOCALE_VAL
#  اگر defined(LC_MESSAGES)
    p = get_locale_val(LC_MESSAGES);
#  ورنہ
    // This is necessary for Win32, where LC_MESSAGES is not defined and $LANG
    // may be set to the LCID number.  $LANG holds the display language, see
    // set_init_lang_env().  Otherwise LC_COLLATE is the best guess, LC_TIME
    // and LC_MONETARY may be set differently for a Japanese working in the US.
    p = mch_getenv((char_u *)"LANG");
    اگر (!is_valid_mess_lang(p))
	p = get_locale_val(LC_COLLATE);
#  endif
# ورنہ
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
    واپس is_valid_mess_lang(p) ? p : تہی;
}
#اگر_ختم

// Complicated #if; matches with where get_mess_env() is used below.
#اگر (defined(FEAT_EVAL) && !((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	    && defined(LC_MESSAGES))) \
	|| ((defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
		&& !defined(LC_MESSAGES))
/*
 * Get the language used for messages from the environment.
 */
    ساکن char_u *
get_mess_env(خالی)
{
    char_u	*p;

    p = mch_getenv((char_u *)"LC_ALL");
    اگر (p != تہی && *p != NUL)
	واپس p;

    p = mch_getenv((char_u *)"LC_MESSAGES");
    اگر (p != تہی && *p != NUL)
	واپس p;

    p = mch_getenv((char_u *)"LANG");
    اگر (p != تہی && VIM_ISDIGIT(*p))
	p = تہی;		// ignore something like "1043"
# ifdef HAVE_GET_LOCALE_VAL
    اگر (p == تہی || *p == NUL)
	p = get_locale_val(LC_CTYPE);
# endif
    واپس p;
}
#اگر_ختم

#اگر defined(FEAT_EVAL)

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    خالی
سیٹ_lang_var(خالی)
{
    char_u	*loc;

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_CTYPE);
# ورنہ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    سیٹ_vim_var_string(VV_CTYPE, loc, -1);

    // When LC_MESSAGES isn't defined use the value from $LC_MESSAGES, fall
    // back to LC_CTYPE if it's empty.
# اگر defined(HAVE_GET_LOCALE_VAL) && defined(LC_MESSAGES)
    loc = get_locale_val(LC_MESSAGES);
# ورنہ
    loc = get_mess_env();
# endif
    سیٹ_vim_var_string(VV_LANG, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_وقت);
# endif
    سیٹ_vim_var_string(VV_LC_وقت, loc, -1);

# ifdef HAVE_GET_LOCALE_VAL
    loc = get_locale_val(LC_COLLATE);
# ورنہ
    // setlocale() not supported: use the default value
    loc = (char_u *)"C";
# endif
    سیٹ_vim_var_string(VV_COLLATE, loc, -1);
}
#اگر_ختم

#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Setup to use the current locale (for ctype() and many other things).
 */
    خالی
init_locale(خالی)
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
	اگر (p != تہی && *p != NUL)
	{
	    vim_snprintf((حرف *)NameBuff, MAXPATHL, "%s/lang", p);
	    bindtextdomain(VIMPACKAGE, (حرف *)NameBuff);
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
    خالی
ex_language(exarg_T *eap)
{
    حرف	*loc;
    char_u	*p;
    char_u	*name;
    صحیح		what = LC_ALL;
    حرف	*whatstr = "";
# ifdef LC_MESSAGES
#  define VIM_LC_MESSAGES LC_MESSAGES
# ورنہ
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
	ورنہ اگر (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	ورنہ اگر (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_وقت;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
	ورنہ اگر (STRNICMP(eap->arg, "collate", p - eap->arg) == 0)
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
	ورنہ
# endif
	    p = (char_u *)setlocale(what, تہی);
	اگر (p == تہی || *p == NUL)
	    p = (char_u *)"Unknown";
	smsg(_("Current %slanguage: \"%s\""), whatstr, p);
    }
    ورنہ
    {
# ifndef LC_MESSAGES
	اگر (what == VIM_LC_MESSAGES)
	    loc = "";
	ورنہ
# endif
	{
	    loc = setlocale(what, (حرف *)name);
# اگر defined(LC_NUMERIC)
	    // Make sure strtod() uses a decimal point, not a comma.
	    setlocale(LC_NUMERIC, "C");
# endif
	}
	اگر (loc == تہی)
	    semsg(_(e_cannot_سیٹ_language_مقصد_str), name);
	ورنہ
	{
# ifdef HAVE_NL_پیغام_CAT_CNTR
	    // Need to do this for GNU gettext, otherwise cached translations
	    // will be used again.
	    خارجی صحیح nl_پیغام_cat_cntr;

	    ++nl_پیغام_cat_cntr;
# elif defined(DYNAMIC_GETTEXT)
	    // Same thing when gettext is loaded at runtime.
	    اگر (dyn_libintl_nl_پیغام_cat_cntr != تہی)
		++*dyn_libintl_nl_پیغام_cat_cntr;
# endif
	    // Reset $LC_ALL, otherwise it would overrule everything.
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    اگر (what != LC_وقت && what != LC_COLLATE)
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
# ورنہ
		    mname = name;
# endif
		    // Clear $LANGUAGE because GNU gettext uses it.
		    vim_setenv((char_u *)"LANGUAGE", (char_u *)"");
		    vim_setenv((char_u *)"LC_MESSAGES", mname);
# ifdef FEAT_MULTI_LANG
		    سیٹ_helplang_default(mname);
# endif
		}
	    }

# ifdef FEAT_EVAL
	    // Set v:lang, v:lc_time, v:collate and v:ctype to the final result.
	    سیٹ_lang_var();
# endif
	    maketitle();
	}
    }
}

ساکن char_u	**locales = تہی;	// Array of all available locales

ساکن صحیح	did_init_locales = FALSE;

/*
 * Return an array of strings for all available locales + NULL for the
 * last element.  Return NULL in case of error.
 */
    ساکن char_u **
ڈھونڈو_locales(خالی)
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
						    تہی, SHELL_SILENT, تہی);
# ورنہ
    // Find all available locales by examining the directories in
    // $VIMRUNTIME/lang/
    {
	صحیح		options = WILD_SILENT|WILD_USE_NL|WILD_KEEP_ALL;
	expand_T	xpc;
	char_u		*p;

	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	locale_list = ExpandOne(&xpc, (char_u *)"$VIMRUNTIME/lang/*",
						      تہی, options, WILD_ALL);
	ExpandCleanup(&xpc);
	اگر (locale_list == تہی)
	    // Add a dummy input, that will be skipped lated but we need to
	    // have something in locale_list so that the C locale is added at
	    // the end.
	    locale_list = vim_strsave((char_u *)".\n");
	p = locale_list;
	// find the last directory delimiter
	جب (p != تہی && *p != NUL)
	{
	    اگر (*p == '\n')
		توڑو;
	    اگر (*p == '\\')
		len = p - locale_list;
	    p++;
	}
    }
# endif
    اگر (locale_list == تہی)
	واپس تہی;
    ga_init2(&locales_ga, حجم(char_u *), 20);

    // Transform locale_list string where each locale is separated by "\n"
    // into an array of locale strings.
    loc = (char_u *)strtok((حرف *)locale_list, "\n");

    جب (loc != تہی)
    {
	صحیح ignore = FALSE;

# ifdef MSWIN
	اگر (len > 0)
	    loc += len + 1;
	// skip locales with a dot (which indicates the charset)
	اگر (vim_strchr(loc, '.') != تہی)
	    ignore = TRUE;
# endif
	اگر (!ignore)
	{
	    اگر (ga_grow(&locales_ga, 1) == FAIL)
		توڑو;

	    loc = vim_strsave(loc);
	    اگر (loc == تہی)
		توڑو;

	    ((char_u **)locales_ga.ga_ڈیٹا)[locales_ga.ga_len++] = loc;
	}
	loc = (char_u *)strtok(تہی, "\n");
    }

# ifdef MSWIN
    // Add the C locale
    اگر (ga_grow(&locales_ga, 1) == OK)
	((char_u **)locales_ga.ga_ڈیٹا)[locales_ga.ga_len++] =
						    vim_strsave((char_u *)"C");
# endif

    vim_free(locale_list);
    اگر (ga_grow(&locales_ga, 1) == FAIL)
    {
	ga_clear(&locales_ga);
	واپس تہی;
    }
    ((char_u **)locales_ga.ga_ڈیٹا)[locales_ga.ga_len] = تہی;
    واپس (char_u **)locales_ga.ga_ڈیٹا;
}

/*
 * Lazy initialization of all available locales.
 */
    ساکن خالی
init_locales(خالی)
{
    اگر (did_init_locales)
	واپس;

    did_init_locales = TRUE;
    locales = ڈھونڈو_locales();
}

# اگر defined(EXITFREE)
    خالی
free_locales(خالی)
{
    صحیح			i;

    اگر (locales == تہی)
	واپس;

    برائے (i = 0; locales[i] != تہی; i++)
	vim_free(locales[i]);
    VIM_CLEAR(locales);
}
# endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
    char_u *
get_lang_arg(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx == 0)
	واپس (char_u *)"messages";
    اگر (idx == 1)
	واپس (char_u *)"ctype";
    اگر (idx == 2)
	واپس (char_u *)"time";
    اگر (idx == 3)
	واپس (char_u *)"collate";

    init_locales();
    اگر (locales == تہی)
	واپس تہی;
    واپس locales[idx - 4];
}

/*
 * Function given to ExpandGeneric() to obtain the available locales.
 */
    char_u *
get_locales(expand_T *xp غیر_مستعمل, صحیح idx)
{
    init_locales();
    اگر (locales == تہی)
	واپس تہی;
    واپس locales[idx];
}

#اگر_ختم
