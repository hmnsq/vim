/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * help.c: functions for Vim help
 */

#ضمن "vim.h"

/*
 * ":help": open a read-only window on a help file
 */
    فراغ
ex_help(exarg_T *eap)
{
    char_u	*arg;
    char_u	*برچسب;
    FILE	*helpfd;	// file descriptor of help file
    صحيح		n;
    صحيح		i;
    win_T	*wp;
    صحيح		رقم_matches;
    char_u	**matches;
    char_u	*p;
    صحيح		خالی_fnum = 0;
    صحيح		alt_fnum = 0;
    buf_T	*buf;
#اگر_تعریف_شده FEAT_MULTI_LANG
    صحيح		len;
    char_u	*lang;
#پایان_اگر
#اگر_تعریف_شده FEAT_FOLDING
    صحيح		old_کلید_Typed = کلید_Typed;
#پایان_اگر

    إذا (ERROR_IF_ANY_POPUP_پنجره)
	أرجع;

    إذا (eap != عدم)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	لكل (arg = eap->arg; *arg; ++arg)
	{
	    إذا (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		اكسر;
	    }
	}
	arg = eap->arg;

	إذا (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_(e_dont_panic));
	    أرجع;
	}

	إذا (eap->skip)	    // not executing commands
	    أرجع;
    }
    وإلا
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    طالما (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#اگر_تعریف_شده FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#پایان_اگر

    // When no argument given go to the index.
    إذا (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = پیدا_کن_help_برچسبها(arg, &رقم_matches, &matches,
						 eap != عدم && eap->forceit);

    i = 0;
#اگر_تعریف_شده FEAT_MULTI_LANG
    إذا (n != FAIL && lang != عدم)
	// Find first item with the requested language.
	لكل (i = 0; i < رقم_matches; ++i)
	{
	    len = (صحيح)STRLEN(matches[i]);
	    إذا (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		اكسر;
	}
#پایان_اگر
    إذا (i >= رقم_matches || n == FAIL)
    {
#اگر_تعریف_شده FEAT_MULTI_LANG
	إذا (lang != عدم)
	    semsg(_(e_sorry_no_str_help_for_str), lang, arg);
	وإلا
#پایان_اگر
	    semsg(_(e_sorry_no_help_for_str), arg);
	إذا (n != FAIL)
	    FreeWild(رقم_matches, matches);
	أرجع;
    }

    // The first match (in the requested language) is the best match.
    برچسب = vim_strsave(matches[i]);
    FreeWild(رقم_matches, matches);

#اگر_تعریف_شده FEAT_GUI
    need_mouse_correct = TRUE;
#پایان_اگر

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    إذا (!bt_help(curwin->w_میانگیر) || cmdmod.cmod_tab != 0)
    {
	إذا (cmdmod.cmod_tab != 0)
	    wp = عدم;
	وإلا
	    FOR_ALL_WINDOWS(wp)
		إذا (bt_help(wp->w_میانگیر))
		    اكسر;
	إذا (wp != عدم && wp->w_میانگیر->b_nwindows > 0)
	    win_enter(wp, TRUE);
	وإلا
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    إذا ((helpfd = mch_fopen((حرف *)p_hf, READBIN)) == عدم)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		اذهب erret;
	    }
	    fclose(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    إذا (cmdmod.cmod_تقسیم == 0 && curwin->w_عرض != Columns
						  && curwin->w_عرض < 80)
		n |= p_sb ? WSP_BOT : WSP_بالاترین;
	    إذا (win_تقسیم(0, n) == FAIL)
		اذهب erret;

	    إذا (curwin->w_ارتفاع < p_hh)
		win_setheight((صحيح)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (فراغ)do_ecmd(0, عدم, عدم, عدم, ECMD_LASTL,
			  ECMD_HIDE + ECMD_مجموعه_HELP,
			  عدم);  // buffer is still open, don't store info
	    إذا ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    خالی_fnum = curbuf->b_fnum;
	}
    }

    إذا (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#اگر_تعریف_شده FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    کلید_Typed = old_کلید_Typed;
#پایان_اگر

    إذا (برچسب != عدم)
	do_برچسب(برچسب, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    إذا (خالی_fnum != 0 && curbuf->b_fnum != خالی_fnum)
    {
	buf = buflist_findnr(خالی_fnum);
	إذا (buf != عدم && buf->b_nwindows == 0)
	    wipe_میانگیر(buf, TRUE);
    }

    // keep the previous alternate file
    إذا (alt_fnum != 0 && curwin->w_alt_fnum == خالی_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(برچسب);
}

/*
 * ":helpclose": Close one help window
 */
    فراغ
ex_helpclose(exarg_T *eap استفادهنشده)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	إذا (bt_help(win->w_میانگیر))
	{
	    win_close(win, FALSE);
	    أرجع;
	}
    }
}

#إذا defined(FEAT_MULTI_LANG)
/*
 * In an argument search for a language specifiers in the form "@xx".
 * Changes the "@" to NUL if found, and returns a pointer to "xx".
 * Returns NULL if not found.
 */
    char_u *
check_help_lang(char_u *arg)
{
    صحيح len = (صحيح)STRLEN(arg);

    إذا (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	أرجع arg + len - 2;
    }
    أرجع عدم;
}
#پایان_اگر

/*
 * Return a heuristic indicating how well the given string matches.  The
 * smaller the number, the better the match.  This is the order of priorities,
 * from best match to worst match:
 *	- Match with least alphanumeric characters is better.
 *	- Match with least total characters is better.
 *	- Match towards the start is better.
 *	- Match starting with "+" is worse (feature instead of command)
 * Assumption is made that the matched_string passed has already been found to
 * match some string for which help is requested.  webb.
 */
    صحيح
help_heuristic(
    char_u	*matched_string,
    صحيح		جابه_جایی,			// offset for match
    صحيح		wrong_case)		// no matching case
{
    صحيح		رقم_letters;
    char_u	*p;

    رقم_letters = 0;
    لكل (p = matched_string; *p; p++)
	إذا (ASCII_ISALNUM(*p))
	    رقم_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    إذا (ASCII_ISALNUM(matched_string[جابه_جایی]) && جابه_جایی > 0
				 && ASCII_ISALNUM(matched_string[جابه_جایی - 1]))
	جابه_جایی += 10000;
    وإلا إذا (جابه_جایی > 2)
	جابه_جایی *= 200;
    إذا (wrong_case)
	جابه_جایی += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    إذا (matched_string[0] == '+' && matched_string[1] != NUL)
	جابه_جایی += 100;
    أرجع (صحيح)(100 * رقم_letters + STRLEN(matched_string) + جابه_جایی);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    ساكن صحيح
help_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    حرف    *p1;
    حرف    *p2;
    صحيح	    cmp;

    p1 = *(حرف **)s1 + طول_رشته(*(حرف **)s1) + 1;
    p2 = *(حرف **)s2 + طول_رشته(*(حرف **)s2) + 1;

    // Compare by help heuristic number first.
    cmp = strcmp(p1, p2);
    إذا (cmp != 0)
	أرجع cmp;

    // Compare by strings as tie-breaker when same heuristic number.
    أرجع strcmp(*(حرف **)s1, *(حرف **)s2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * The matches will be sorted with a "best" match algorithm.
 * When "keep_lang" is TRUE try keeping the language of the current buffer.
 */
    صحيح
پیدا_کن_help_برچسبها(
    char_u	*arg,
    صحيح		*رقم_matches,
    char_u	***matches,
    صحيح		keep_lang)
{
    char_u	*s, *d;
    صحيح		i;
    // Specific tags that either have a specific replacement or won't go
    // through the generic rules.
    ساكن حرف *(except_tbl[][2]) = {
	{"*",		"star"},
	{"g*",		"gstar"},
	{"[*",		"[star"},
	{"]*",		"]star"},
	{":*",		":star"},
	{"/*",		"/star"},
	{"/\\*",	"/\\\\star"},
	{"\"*",		"quotestar"},
	{"**",		"starstar"},
	{"cpo-*",	"cpo-star"},
	{"/\\(\\)",	"/\\\\(\\\\)"},
	{"/\\%(\\)",	"/\\\\%(\\\\)"},
	{"?",		"?"},
	{"??",		"??"},
	{":?",		":?"},
	{"?<CR>",	"?<CR>"},
	{"g?",		"g?"},
	{"g?g?",	"g?g?"},
	{"g??",		"g??"},
	{"-?",		"-?"},
	{"q?",		"q?"},
	{"v_g?",	"v_g?"},
	{"/\\?",	"/\\\\?"},
	{"/\\z(\\)",	"/\\\\z(\\\\)"},
	{"\\=",		"\\\\="},
	{":s\\=",	":s\\\\="},
	{"[count]",	"\\[count]"},
	{"[quotex]",	"\\[quotex]"},
	{"[range]",	"\\[range]"},
	{":[range]",	":\\[range]"},
	{"[pattern]",	"\\[pattern]"},
	{"\\|",		"\\\\bar"},
	{"\\%$",	"/\\\\%\\$"},
	{"s/\\~",	"s/\\\\\\~"},
	{"s/\\U",	"s/\\\\U"},
	{"s/\\L",	"s/\\\\L"},
	{"s/\\1",	"s/\\\\1"},
	{"s/\\2",	"s/\\\\2"},
	{"s/\\3",	"s/\\\\3"},
	{"s/\\9",	"s/\\\\9"},
	{عدم, عدم}
    };
    ساكن حرف *(عبارت_جدول[]) = {"!=?", "!~?", "<=?", "<?", "==?", "=~?",
				   ">=?", ">?", "is?", "isnot?"};
    صحيح flags;

    d = IObuff;		    // assume IObuff is long enough!
    d[0] = NUL;

    إذا (STRNICMP(arg, "expr-", 5) == 0)
    {
	// When the string starting with "expr-" and containing '?' and matches
	// the table, it is taken literally (but ~ is escaped).  Otherwise '?'
	// is recognized as a wildcard.
	لكل (i = (صحيح)ARRAY_درازی(عبارت_جدول); --i >= 0; )
	    إذا (STRCMP(arg + 5, عبارت_جدول[i]) == 0)
	    {
		صحيح si = 0, di = 0;

		لكل (;;)
		{
		    إذا (arg[si] == '~')
			d[di++] = '\\';
		    d[di++] = arg[si];
		    إذا (arg[si] == NUL)
			اكسر;
		    ++si;
		}
		اكسر;
	    }
    }
    وإلا
    {
	// Recognize a few exceptions to the rule.  Some strings that contain
	// '*'are changed to "star", otherwise '*' is recognized as a wildcard.
	لكل (i = 0; except_tbl[i][0] != عدم; ++i)
	    إذا (STRCMP(arg, except_tbl[i][0]) == 0)
	    {
		STRCPY(d, except_tbl[i][1]);
		اكسر;
	    }
    }

    إذا (d[0] == NUL)	// no match in table
    {
	// Replace "\S" with "/\\S", etc.  Otherwise every tag is matched.
	// Also replace "\%^" and "\%(", they match every tag too.
	// Also "\zs", "\z1", etc.
	// Also "\@<", "\@=", "\@<=", etc.
	// And also "\_$" and "\_^".
	إذا (arg[0] == '\\'
		&& ((arg[1] != NUL && arg[2] == NUL)
		    || (vim_strchr((char_u *)"%_z@", arg[1]) != عدم
							   && arg[2] != NUL)))
	{
	    vim_snprintf((حرف *)d, IOSIZE, "/\\\\%s", arg + 1);
	    // Check for "/\\_$", should be "/\\_\$"
	    إذا (d[3] == '_' && d[4] == '$')
		STRCPY(d + 4, "\\$");
	}
	وإلا
	{
	  // Replace:
	  // "[:...:]" with "\[:...:]"
	  // "[++...]" with "\[++...]"
	  // "\{" with "\\{"		   -- matching "} \}"
	    إذا ((arg[0] == '[' && (arg[1] == ':'
			 || (arg[1] == '+' && arg[2] == '+')))
		    || (arg[0] == '\\' && arg[1] == '{'))
	      *d++ = '\\';

	  // If tag starts with "('", skip the "(". Fixes CTRL-] on ('option'.
	  إذا (*arg == '(' && arg[1] == '\'')
	      arg++;
	  لكل (s = arg; *s; ++s)
	  {
	    // Replace "|" with "bar" and '"' with "quote" to match the name of
	    // the tags for these commands.
	    // Replace "*" with ".*" and "?" with "." to match command line
	    // completion.
	    // Insert a backslash before '~', '$' and '.' to avoid their
	    // special meaning.
	    إذا (d - IObuff > IOSIZE - 10)	// getting too long!?
		اكسر;
	    بدل (*s)
	    {
		حالة '|':   STRCPY(d, "bar");
			    d += 3;
			    استمر;
		حالة '"':   STRCPY(d, "quote");
			    d += 5;
			    استمر;
		حالة '*':   *d++ = '.';
			    اكسر;
		حالة '?':   *d++ = '.';
			    استمر;
		حالة '$':
		حالة '.':
		حالة '~':   *d++ = '\\';
			    اكسر;
	    }

	    // Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	    // ":help i_^_CTRL-D" work.
	    // Insert '-' before and after "CTRL-X" when applicable.
	    إذا (*s < ' ' || (*s == '^' && s[1] && (ASCII_ISALPHA(s[1])
			   || vim_strchr((char_u *)"?@[\\]^", s[1]) != عدم)))
	    {
		إذا (d > IObuff && d[-1] != '_' && d[-1] != '\\')
		    *d++ = '_';		// prepend a '_' to make x_CTRL-x
		STRCPY(d, "CTRL-");
		d += 5;
		إذا (*s < ' ')
		{
		    *d++ = *s + '@';
		    إذا (d[-1] == '\\')
			*d++ = '\\';	// double a backslash
		}
		وإلا
		    *d++ = *++s;
		إذا (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		// append a '_'
		استمر;
	    }
	    وإلا إذا (*s == '^')		// "^" or "CTRL-^" or "^_"
		*d++ = '\\';

	    // Insert a backslash before a backslash after a slash, for search
	    // pattern tags: "/\|" --> "/\\|".
	    وإلا إذا (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    // "CTRL-\_" -> "CTRL-\\_" to avoid the special meaning of "\_" in
	    // "CTRL-\_CTRL-N"
	    إذا (STRNICMP(s, "CTRL-\\_", 7) == 0)
	    {
		STRCPY(d, "CTRL-\\\\");
		d += 7;
		s += 6;
	    }

	    *d++ = *s;

	    // If tag contains "({" or "([", tag terminates at the "(".
	    // This is for help on functions, e.g.: abs({expr}).
	    إذا (*s == '(' && (s[1] == '{' || s[1] =='['))
		اكسر;

	    // If tag starts with ', toss everything after a second '. Fixes
	    // CTRL-] on 'option'. (would include the trailing '.').
	    إذا (*s == '\'' && s > arg && *arg == '\'')
		اكسر;
	    // Also '{' and '}'.
	    إذا (*s == '}' && s > arg && *arg == '{')
		اكسر;
	  }
	  *d = NUL;

	  إذا (*IObuff == '`')
	  {
	      إذا (d > IObuff + 2 && d[-1] == '`')
	      {
		  // remove the backticks from `command`
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-2] = NUL;
	      }
	      وإلا إذا (d > IObuff + 3 && d[-2] == '`' && d[-1] == ',')
	      {
		  // remove the backticks and comma from `command`,
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-3] = NUL;
	      }
	      وإلا إذا (d > IObuff + 4 && d[-3] == '`'
					     && d[-2] == '\\' && d[-1] == '.')
	      {
		  // remove the backticks and dot from `command`\.
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-4] = NUL;
	      }
	  }
	}
    }

    *matches = (char_u **)"";
    *رقم_matches = 0;
    flags = برچسب_HELP | برچسب_REGEXP | برچسب_نامها | برچسب_VERBOSE | برچسب_NO_TAGFUNC;
    إذا (keep_lang)
	flags |= برچسب_KEEP_LANG;
    إذا (پیدا_کن_برچسبها(IObuff, رقم_matches, matches, flags, (صحيح)MAXCOL, عدم) == OK
	    && *رقم_matches > 0)
    {
	// Sort the matches found on the heuristic number that is after the
	// tag name.
	qsort((فراغ *)*matches, (size_t)*رقم_matches,
					      حجم(char_u *), help_compare);
	// Delete more than TAG_MANY to reduce the size of the listing.
	طالما (*رقم_matches > برچسب_MANY)
	    vim_free((*matches)[--*رقم_matches]);
    }
    أرجع OK;
}

#اگر_تعریف_شده FEAT_MULTI_LANG
/*
 * Cleanup matches for help tags:
 * Remove "@ab" if the top of 'helplang' is "ab" and the language of the first
 * tag matches it.  Otherwise remove "@en" if "en" is the only language.
 */
    فراغ
cleanup_help_برچسبها(صحيح رقم_file, char_u **file)
{
    صحيح		i, j;
    صحيح		len;
    char_u	buf[4];
    char_u	*p = buf;

    إذا (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    لكل (i = 0; i < رقم_file; ++i)
    {
	len = (صحيح)STRLEN(file[i]) - 3;
	إذا (len <= 0)
	    استمر;
	إذا (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    لكل (j = 0; j < رقم_file; ++j)
		إذا (j != i && (صحيح)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    اكسر;
	    إذا (j == رقم_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    إذا (*buf != NUL)
	لكل (i = 0; i < رقم_file; ++i)
	{
	    len = (صحيح)STRLEN(file[i]) - 3;
	    إذا (len <= 0)
		استمر;
	    إذا (STRCMP(file[i] + len, buf) == 0)
	    {
		// remove the default language
		file[i][len] = NUL;
	    }
	}
}
#پایان_اگر

/*
 * Called when starting to edit a buffer for a help file.
 */
    فراغ
prepare_help_میانگیر(فراغ)
{
    char_u	*p;

    curbuf->b_help = true;
#اگر_تعریف_شده FEAT_QUICKFIX
    مجموعه_string_option_مستقیم((char_u *)"buftype", -1,
				     (char_u *)"help", OPT_FREE|OPT_LOCAL, 0);
#پایان_اگر

    // Always set these options after jumping to a help tag, because the
    // user may have an autocommand that gets in the way.
    // When adding an option here, also update the help file helphelp.txt.

    // Accept all ASCII chars for keywords, except ' ', '*', '"', '|', and
    // latin1 word characters (for translated help files).
    // Only set it when needed, buf_init_chartab() is some work.
    p = (char_u *)"!-~,^*,^|,^\",192-255";
    إذا (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	مجموعه_string_option_مستقیم((char_u *)"isk", -1, p, OPT_FREE|OPT_LOCAL, 0);
	check_buf_options(curbuf);
	(فراغ)buf_init_chartab(curbuf, FALSE);
    }

#اگر_تعریف_شده FEAT_FOLDING
    // Don't use the global foldmethod.
    مجموعه_string_option_مستقیم((char_u *)"fdm", -1, (char_u *)"manual",
						       OPT_FREE|OPT_LOCAL, 0);
#پایان_اگر

    curbuf->b_p_ts = 8;		// 'tabstop' is 8
    curwin->w_p_list = FALSE;	// no list mode

    curbuf->b_p_ma = FALSE;	// not modifiable
    curbuf->b_p_bin = FALSE;	// reset 'bin' before reading file
    curwin->w_p_nu = 0;		// no line numbers
    curwin->w_p_rnu = 0;	// no relative line numbers
    RESET_BINDING(curwin);	// no scroll or cursor binding
#اگر_تعریف_شده FEAT_ARABIC
    curwin->w_p_arab = FALSE;	// no arabic mode
#پایان_اگر
#اگر_تعریف_شده FEAT_RIGHTLEFT
    curwin->w_p_rl  = FALSE;	// help window is left-to-right
#پایان_اگر
#اگر_تعریف_شده FEAT_FOLDING
    curwin->w_p_fen = FALSE;	// No folding in the help window
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
    curwin->w_p_diff = FALSE;	// No 'diff'
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
    curwin->w_p_spell = FALSE;	// No spell checking
#پایان_اگر

    مجموعه_buflisted(FALSE);
}

/*
 * After reading a help file: May cleanup a help buffer when syntax
 * highlighting is not used.
 */
    فراغ
fix_help_میانگیر(فراغ)
{
    linenr_T	lnum;
    char_u	*سطر;
    صحيح		درون_example = FALSE;
    صحيح		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    صحيح		mustfree;

    // Set filetype to "help" if still needed.
    إذا (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_قفل;
	مجموعه_option_مقدار_give_err((char_u *)"ft",
					      0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_قفل;
    }

#اگر_تعریف_شده FEAT_SYN_HL
    إذا (!syntax_present(curwin))
#پایان_اگر
    {
	لكل (lnum = 1; lnum <= curbuf->b_ml.ml_سطر_شمار; ++lnum)
	{
	    سطر = ml_get_buf(curbuf, lnum, FALSE);
	    len = ml_get_buf_len(curbuf, lnum);
	    إذا (درون_example && len > 0 && !VIM_ISWHITE(سطر[0]))
	    {
		// End of example: non-white or '<' in first column.
		إذا (سطر[0] == '<')
		{
		    // blank-out a '<' in the first column
		    سطر = ml_get_buf(curbuf, lnum, TRUE);
		    سطر[0] = ' ';
		}
		درون_example = FALSE;
	    }
	    إذا (!درون_example && len > 0)
	    {
		إذا (سطر[len - 1] == '>' && (len == 1 || سطر[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    سطر = ml_get_buf(curbuf, lnum, TRUE);
		    سطر[len - 1] = ' ';
		    درون_example = TRUE;
		}
		وإلا إذا (سطر[len - 1] == '~')
		{
		    // blank-out a '~' at the end of line (header marker)
		    سطر = ml_get_buf(curbuf, lnum, TRUE);
		    سطر[len - 1] = ' ';
		}
	    }
	}
    }

    // In the "help.txt" and "help.abx" file, add the locally added help
    // files.  This uses the very first line in the help file.
    fname = gettail(curbuf->b_fname);
    إذا (fnamecmp(fname, "help.txt") == 0
#اگر_تعریف_شده FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#پایان_اگر
	)
    {
	لكل (lnum = 1; lnum < curbuf->b_ml.ml_سطر_شمار; ++lnum)
	{
	    سطر = ml_get_buf(curbuf, lnum, FALSE);
	    إذا (جستجوی_رشته((حرف *)سطر, "*local-additions*") == عدم)
		استمر;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    طالما (*p != NUL)
	    {
		صحيح NameBufflen;

		NameBufflen = copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		إذا (rt != عدم &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    صحيح		fcount;
		    char_u	**fnames;
		    FILE	*fd;
		    char_u	*s;
		    صحيح		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    إذا (*NameBuff != NUL && !after_pathsep(NameBuff, NameBuff + NameBufflen))
		    {
			STRCPY(NameBuff + NameBufflen, PATHSEPSTR);
			NameBufflen += حجم(PATHSEP);
		    }
#اگر_تعریف_شده FEAT_MULTI_LANG
		    STRCPY(NameBuff + NameBufflen, "doc/*.??[tx]");
#وإلا
		    STRCPY(NameBuff + NameBufflen, "doc/*.txt");
#پایان_اگر
		    إذا (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#اگر_تعریف_شده FEAT_MULTI_LANG
			صحيح	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			لكل (i1 = 0; i1 < fcount; ++i1)
			{
			    f1 = fnames[i1];
			    t1 = gettail(f1);
			    e1 = vim_strrchr(t1, '.');
			    إذا (e1 == عدم)
				استمر;
			    إذا (fnamecmp(e1, ".txt") != 0
					       && fnamecmp(e1, fname + 4) != 0)
			    {
				// Not .txt and not .abx, remove it.
				VIM_CLEAR(fnames[i1]);
				استمر;
			    }

			    لكل (i2 = i1 + 1; i2 < fcount; ++i2)
			    {
				f2 = fnames[i2];
				إذا (f2 == عدم)
				    استمر;
				t2 = gettail(f2);
				e2 = vim_strrchr(t2, '.');
				إذا (e2 == عدم)
				    استمر;
				إذا (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    استمر;
				إذا (fnamecmp(e1, ".txt") == 0
					       && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#پایان_اگر
			لكل (fi = 0; fi < fcount; ++fi)
			{
			    إذا (fnames[fi] == عدم)
				استمر;
			    fd = mch_fopen((حرف *)fnames[fi], "r");
			    إذا (fd != عدم)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				إذا (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != عدم)
				{
				    صحيح	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    طالما (*s != NUL)
				    {
					إذا (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					إذا (*s >= 0x80 && this_utf != FALSE)
					{
					    صحيح	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    إذا (l == 1)
						this_utf = FALSE;
					    s += l - 1;
					}
					++s;
				    }

				    // The help file is latin1 or utf-8;
				    // conversion to the current
				    // 'encoding' may be required.
				    vc.vc_type = CONV_NONE;
				    convert_راهاندازی(&vc, (char_u *)(
						this_utf == TRUE ? "utf-8"
						      : "latin1"), p_enc);
				    إذا (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    وإلا
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    عدم);
					إذا (cp == عدم)
					    cp = IObuff;
				    }
				    convert_راهاندازی(&vc, عدم, عدم);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    إذا (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				fclose(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		إذا (mustfree)
		    vim_free(rt);
	    }
	    اكسر;
	}
    }
}

/*
 * ":exusage"
 */
    فراغ
ex_exusage(exarg_T *eap استفادهنشده)
{
    do_cmdline_cmd((char_u *)"help ex-cmd-index");
}

/*
 * ":viusage"
 */
    فراغ
ex_viusage(exarg_T *eap استفادهنشده)
{
    do_cmdline_cmd((char_u *)"help normal-index");
}

/*
 * Generate tags in one help directory.
 */
    ساكن فراغ
helptags_one(
    char_u	*dir,		// doc directory
    char_u	*ext,		// suffix, ".txt", ".itx", ".frx", etc.
    char_u	*tagfname,	// "tags" for English, "tags-fr" for French.
    صحيح		جمع_کن_help_برچسبها,	// add "help-tags" tag
    صحيح		ignore_writeerr)    // ignore write error
{
    FILE	*fd_برچسبها;
    FILE	*fd;
    garray_T	ga;
    صحيح		res;
    صحيح		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    صحيح		fi;
    char_u	*s;
    صحيح		i;
    char_u	*fname;
    صحيح		dirlen;
    صحيح		utf8 = MAYBE;
    صحيح		this_utf8;
    صحيح		firstline;
    صحيح		درون_example;
    صحيح		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (صحيح)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    res = gen_expand_wildcards(1, &NameBuff, &filecount, &files,
							    EW_FILE|EW_SILENT);
    إذا (res == FAIL || filecount == 0)
    {
	إذا (!got_int)
	    semsg(_(e_no_تطابق_str_1), NameBuff);
	إذا (res != FAIL)
	    FreeWild(filecount, files);
	أرجع;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    جمع_کن_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_برچسبها = mch_fopen((حرف *)NameBuff, "w");
    إذا (fd_برچسبها == عدم)
    {
	إذا (!ignore_writeerr)
	    semsg(_(e_cannot_open_str_for_writing_1), NameBuff);
	FreeWild(filecount, files);
	أرجع;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, حجم(char_u *), 100);
    إذا (جمع_کن_help_برچسبها || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	إذا (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	وإلا
	{
	    s = alloc(18 + (بیعلامت)STRLEN(tagfname));
	    إذا (s == عدم)
		got_int = TRUE;
	    وإلا
	    {
		sprintf((حرف *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_داده)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    لكل (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((حرف *)files[fi], "r");
	إذا (fd == عدم)
	{
	    semsg(_(e_unable_مقصد_open_str_for_reading), files[fi]);
	    استمر;
	}
	fname = files[fi] + dirlen + 1;

	درون_example = FALSE;
	firstline = TRUE;
	طالما (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    إذا (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		لكل (s = IObuff; *s != NUL; ++s)
		    إذا (*s >= 0x80)
		    {
			صحيح l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			إذا (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    اكسر;
			}
			s += l - 1;
		    }
		إذا (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		إذا (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		وإلا إذا (utf8 != this_utf8)
		{
		    semsg(_(e_mix_از_help_file_encodings_within_language_str), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    إذا (درون_example)
	    {
		// skip over example; a non-white in the first column ends it
		إذا (vim_strchr((char_u *)" \t\n\r", IObuff[0]))
		    استمر;
		درون_example = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    طالما (p1 != عدم)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		إذا (p2 != عدم && p2 > p1 + 1)	// skip "*" and "**"
		{
		    لكل (s = p1 + 1; s < p2; ++s)
			إذا (*s == ' ' || *s == '\t' || *s == '|')
			    اكسر;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    إذا (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != عدم
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			إذا (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    اكسر;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			إذا (s == عدم)
			{
			    got_int = TRUE;
			    اكسر;
			}
			((char_u **)ga.ga_داده)[ga.ga_len] = s;
			++ga.ga_len;
			sprintf((حرف *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    size_t off = STRLEN(IObuff);
	    إذا (off >= 2 && IObuff[off - 1] == '\n')
	    {
		off -= 2;
		طالما (off > 0 && (ASCII_ISLOWER(IObuff[off])
						  || VIM_ISDIGIT(IObuff[off])))
		    off--;
		إذا (IObuff[off] == '>' && (off == 0 || IObuff[off - 1] == ' '))
		    درون_example = TRUE;
	    }
	    سطر_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    إذا (!got_int)
    {
	// Sort the tags.
	إذا (ga.ga_داده != عدم)
	    مرتب_سازی_رشتهها((char_u **)ga.ga_داده, ga.ga_len);

	// Check for duplicates.
	لكل (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_داده)[i - 1];
	    p2 = ((char_u **)ga.ga_داده)[i];
	    طالما (*p1 == *p2)
	    {
		إذا (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((حرف *)NameBuff, MAXPATHL,
			    _(e_duplicate_برچسب_str_درون_file_str_str),
				     ((char_u **)ga.ga_داده)[i], dir, p2 + 1);
		    emsg((حرف *)NameBuff);
		    *p2 = '\t';
		    اكسر;
		}
		++p1;
		++p2;
	    }
	}

	إذا (utf8 == TRUE)
	    fprintf(fd_برچسبها, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	لكل (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_داده)[i];
	    إذا (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		fputs((حرف *)s, fd_برچسبها);
	    وإلا
	    {
		fprintf(fd_برچسبها, "%s\t/*", s);
		لكل (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    إذا (*p1 == '\\' || *p1 == '/')
			putc('\\', fd_برچسبها);
		    putc(*p1, fd_برچسبها);
		}
		fprintf(fd_برچسبها, "*\n");
	    }
	}
    }
    إذا (mix)
	got_int = FALSE;    // continue with other languages

    لكل (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_داده)[i]);
    ga_clear(&ga);
    fclose(fd_برچسبها);	    // there is no check for an error...
}

/*
 * Generate tags in one help directory, taking care of translations.
 */
    ساكن فراغ
do_helptags(char_u *dirname, صحيح جمع_کن_help_برچسبها, صحيح ignore_writeerr)
{
#اگر_تعریف_شده FEAT_MULTI_LANG
    صحيح		len;
    صحيح		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    صحيح		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, dirname);
    جمع_کن_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    إذا (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_(e_no_تطابق_str_1), NameBuff);
	أرجع;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    لكل (i = 0; i < filecount; ++i)
    {
	len = (صحيح)STRLEN(files[i]);
	إذا (len <= 4)
	    استمر;

	إذا (STRICMP(files[i] + len - 4, ".txt") == 0)
	{
	    // ".txt" -> language "en"
	    lang[0] = 'e';
	    lang[1] = 'n';
	}
	وإلا إذا (files[i][len - 4] == '.'
		&& ASCII_ISALPHA(files[i][len - 3])
		&& ASCII_ISALPHA(files[i][len - 2])
		&& TOLOWER_ASC(files[i][len - 1]) == 'x')
	{
	    // ".abx" -> language "ab"
	    lang[0] = TOLOWER_ASC(files[i][len - 3]);
	    lang[1] = TOLOWER_ASC(files[i][len - 2]);
	}
	وإلا
	    استمر;

	// Did we find this language already?
	لكل (j = 0; j < ga.ga_len; j += 2)
	    إذا (STRNCMP(lang, ((char_u *)ga.ga_داده) + j, 2) == 0)
		اكسر;
	إذا (j == ga.ga_len)
	{
	    // New language, add it.
	    إذا (ga_grow(&ga, 2) == FAIL)
		اكسر;
	    ((char_u *)ga.ga_داده)[ga.ga_len++] = lang[0];
	    ((char_u *)ga.ga_داده)[ga.ga_len++] = lang[1];
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    لكل (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_داده)[j];
	fname[6] = ((char_u *)ga.ga_داده)[j + 1];
	إذا (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	وإلا
	{
	    // Language "ab" uses ".abx" and "tags-ab".
	    STRCPY(ext, ".xxx");
	    ext[1] = fname[5];
	    ext[2] = fname[6];
	}
	helptags_one(dirname, ext, fname, جمع_کن_help_برچسبها, ignore_writeerr);
    }

    ga_clear(&ga);
    FreeWild(filecount, files);

#وإلا
    // No language support, just use "*.txt" and "tags".
    helptags_one(dirname, (char_u *)".txt", (char_u *)"tags", جمع_کن_help_برچسبها,
							    ignore_writeerr);
#پایان_اگر
}

    ساكن فراغ
helptags_cb(char_u *fname, فراغ *cookie)
{
    do_helptags(fname, *(صحيح *)cookie, TRUE);
}

/*
 * ":helptags"
 */
    فراغ
ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*dirname;
    صحيح		جمع_کن_help_برچسبها = FALSE;

    // Check for ":helptags ++t {dir}".
    إذا (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	جمع_کن_help_برچسبها = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    إذا (STRCMP(eap->arg, "ALL") == 0)
    {
	do_درون_مسیر(p_rtp, "", (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &جمع_کن_help_برچسبها);
    }
    وإلا
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	dirname = ExpandOne(&xpc, eap->arg, عدم,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	إذا (dirname == عدم || !mch_isdir(dirname))
	    semsg(_(e_not_a_directory_str), eap->arg);
	وإلا
	    do_helptags(dirname, جمع_کن_help_برچسبها, FALSE);
	vim_free(dirname);
    }
}
