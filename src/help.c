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

#अन्तर्भाव "vim.h"

/*
 * ":help": open a read-only window on a help file
 */
    शून्य
ex_help(exarg_T *eap)
{
    char_u	*arg;
    char_u	*tag;
    खाता	*helpfd;	// file descriptor of help file
    पूर्णांक		n;
    पूर्णांक		i;
    win_T	*wp;
    पूर्णांक		अंक_matches;
    char_u	**matches;
    char_u	*p;
    पूर्णांक		empty_fnum = 0;
    पूर्णांक		alt_fnum = 0;
    buf_T	*buf;
#ifdef FEAT_MULTI_LANG
    पूर्णांक		len;
    char_u	*lang;
#endif
#ifdef FEAT_FOLDING
    पूर्णांक		old_किल्ली_Typed = किल्ली_Typed;
#endif

    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत;

    जर (eap != रिक्त)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	साठी (arg = eap->arg; *arg; ++arg)
	{
	    जर (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		थांबा;
	    }
	}
	arg = eap->arg;

	जर (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_(e_dont_panic));
	    परत;
	}

	जर (eap->skip)	    // not executing commands
	    परत;
    }
    अन्यथा
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    पर्यंत (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#endif

    // When no argument given go to the index.
    जर (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = find_help_tags(arg, &अंक_matches, &matches,
						 eap != रिक्त && eap->forceit);

    i = 0;
#ifdef FEAT_MULTI_LANG
    जर (n != FAIL && lang != रिक्त)
	// Find first item with the requested language.
	साठी (i = 0; i < अंक_matches; ++i)
	{
	    len = (पूर्णांक)STRLEN(matches[i]);
	    जर (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		थांबा;
	}
#endif
    जर (i >= अंक_matches || n == FAIL)
    {
#ifdef FEAT_MULTI_LANG
	जर (lang != रिक्त)
	    semsg(_(e_sorry_no_str_help_for_str), lang, arg);
	अन्यथा
#endif
	    semsg(_(e_sorry_no_help_for_str), arg);
	जर (n != FAIL)
	    FreeWild(अंक_matches, matches);
	परत;
    }

    // The first match (in the requested language) is the best match.
    tag = vim_strsave(matches[i]);
    FreeWild(अंक_matches, matches);

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    जर (!bt_help(curwin->w_buffer) || cmdmod.cmod_tab != 0)
    {
	जर (cmdmod.cmod_tab != 0)
	    wp = रिक्त;
	अन्यथा
	    FOR_ALL_WINDOWS(wp)
		जर (bt_help(wp->w_buffer))
		    थांबा;
	जर (wp != रिक्त && wp->w_buffer->b_nwindows > 0)
	    win_enter(wp, TRUE);
	अन्यथा
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    जर ((helpfd = mch_fopen((अक्षर *)p_hf, READBIN)) == रिक्त)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		जा erret;
	    }
	    ख_बंद(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    जर (cmdmod.cmod_split == 0 && curwin->w_width != Columns
						  && curwin->w_width < 80)
		n |= p_sb ? WSP_BOT : WSP_शीर्ष;
	    जर (win_split(0, n) == FAIL)
		जा erret;

	    जर (curwin->w_height < p_hh)
		win_setheight((पूर्णांक)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (शून्य)do_ecmd(0, रिक्त, रिक्त, रिक्त, ECMD_LASTL,
			  ECMD_HIDE + ECMD_SET_HELP,
			  रिक्त);  // buffer is still open, don't store info
	    जर ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    empty_fnum = curbuf->b_fnum;
	}
    }

    जर (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#ifdef FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    किल्ली_Typed = old_किल्ली_Typed;
#endif

    जर (tag != रिक्त)
	do_tag(tag, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    जर (empty_fnum != 0 && curbuf->b_fnum != empty_fnum)
    {
	buf = buflist_findnr(empty_fnum);
	जर (buf != रिक्त && buf->b_nwindows == 0)
	    wipe_buffer(buf, TRUE);
    }

    // keep the previous alternate file
    जर (alt_fnum != 0 && curwin->w_alt_fnum == empty_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(tag);
}

/*
 * ":helpclose": Close one help window
 */
    शून्य
ex_helpclose(exarg_T *eap UNUSED)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	जर (bt_help(win->w_buffer))
	{
	    win_close(win, FALSE);
	    परत;
	}
    }
}

#जर defined(FEAT_MULTI_LANG)
/*
 * In an argument search for a language specifiers in the form "@xx".
 * Changes the "@" to NUL if found, and returns a pointer to "xx".
 * Returns NULL if not found.
 */
    char_u *
check_help_lang(char_u *arg)
{
    पूर्णांक len = (पूर्णांक)STRLEN(arg);

    जर (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	परत arg + len - 2;
    }
    परत रिक्त;
}
#endif

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
    पूर्णांक
help_heuristic(
    char_u	*matched_string,
    पूर्णांक		offset,			// offset for match
    पूर्णांक		wrong_case)		// no matching case
{
    पूर्णांक		अंक_letters;
    char_u	*p;

    अंक_letters = 0;
    साठी (p = matched_string; *p; p++)
	जर (ASCII_ISALNUM(*p))
	    अंक_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    जर (ASCII_ISALNUM(matched_string[offset]) && offset > 0
				 && ASCII_ISALNUM(matched_string[offset - 1]))
	offset += 10000;
    अन्यथा जर (offset > 2)
	offset *= 200;
    जर (wrong_case)
	offset += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    जर (matched_string[0] == '+' && matched_string[1] != NUL)
	offset += 100;
    परत (पूर्णांक)(100 * अंक_letters + STRLEN(matched_string) + offset);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    स्थिर पूर्णांक
help_compare(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2)
{
    अक्षर    *p1;
    अक्षर    *p2;
    पूर्णांक	    cmp;

    p1 = *(अक्षर **)s1 + म_माप(*(अक्षर **)s1) + 1;
    p2 = *(अक्षर **)s2 + म_माप(*(अक्षर **)s2) + 1;

    // Compare by help heuristic number first.
    cmp = म_भेद(p1, p2);
    जर (cmp != 0)
	परत cmp;

    // Compare by strings as tie-breaker when same heuristic number.
    परत म_भेद(*(अक्षर **)s1, *(अक्षर **)s2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * The matches will be sorted with a "best" match algorithm.
 * When "keep_lang" is TRUE try keeping the language of the current buffer.
 */
    पूर्णांक
find_help_tags(
    char_u	*arg,
    पूर्णांक		*अंक_matches,
    char_u	***matches,
    पूर्णांक		keep_lang)
{
    char_u	*s, *d;
    पूर्णांक		i;
    // Specific tags that either have a specific replacement or won't go
    // through the generic rules.
    स्थिर अक्षर *(except_tbl[][2]) = {
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
	{रिक्त, रिक्त}
    };
    स्थिर अक्षर *(अभिव्यक्ती_table[]) = {"!=?", "!~?", "<=?", "<?", "==?", "=~?",
				   ">=?", ">?", "is?", "isnot?"};
    पूर्णांक flags;

    d = IObuff;		    // assume IObuff is long enough!
    d[0] = NUL;

    जर (STRNICMP(arg, "expr-", 5) == 0)
    {
	// When the string starting with "expr-" and containing '?' and matches
	// the table, it is taken literally (but ~ is escaped).  Otherwise '?'
	// is recognized as a wildcard.
	साठी (i = (पूर्णांक)ARRAY_LENGTH(अभिव्यक्ती_table); --i >= 0; )
	    जर (STRCMP(arg + 5, अभिव्यक्ती_table[i]) == 0)
	    {
		पूर्णांक si = 0, di = 0;

		साठी (;;)
		{
		    जर (arg[si] == '~')
			d[di++] = '\\';
		    d[di++] = arg[si];
		    जर (arg[si] == NUL)
			थांबा;
		    ++si;
		}
		थांबा;
	    }
    }
    अन्यथा
    {
	// Recognize a few exceptions to the rule.  Some strings that contain
	// '*'are changed to "star", otherwise '*' is recognized as a wildcard.
	साठी (i = 0; except_tbl[i][0] != रिक्त; ++i)
	    जर (STRCMP(arg, except_tbl[i][0]) == 0)
	    {
		STRCPY(d, except_tbl[i][1]);
		थांबा;
	    }
    }

    जर (d[0] == NUL)	// no match in table
    {
	// Replace "\S" with "/\\S", etc.  Otherwise every tag is matched.
	// Also replace "\%^" and "\%(", they match every tag too.
	// Also "\zs", "\z1", etc.
	// Also "\@<", "\@=", "\@<=", etc.
	// And also "\_$" and "\_^".
	जर (arg[0] == '\\'
		&& ((arg[1] != NUL && arg[2] == NUL)
		    || (vim_strchr((char_u *)"%_z@", arg[1]) != रिक्त
							   && arg[2] != NUL)))
	{
	    vim_snprintf((अक्षर *)d, IOSIZE, "/\\\\%s", arg + 1);
	    // Check for "/\\_$", should be "/\\_\$"
	    जर (d[3] == '_' && d[4] == '$')
		STRCPY(d + 4, "\\$");
	}
	अन्यथा
	{
	  // Replace:
	  // "[:...:]" with "\[:...:]"
	  // "[++...]" with "\[++...]"
	  // "\{" with "\\{"		   -- matching "} \}"
	    जर ((arg[0] == '[' && (arg[1] == ':'
			 || (arg[1] == '+' && arg[2] == '+')))
		    || (arg[0] == '\\' && arg[1] == '{'))
	      *d++ = '\\';

	  // If tag starts with "('", skip the "(". Fixes CTRL-] on ('option'.
	  जर (*arg == '(' && arg[1] == '\'')
	      arg++;
	  साठी (s = arg; *s; ++s)
	  {
	    // Replace "|" with "bar" and '"' with "quote" to match the name of
	    // the tags for these commands.
	    // Replace "*" with ".*" and "?" with "." to match command line
	    // completion.
	    // Insert a backslash before '~', '$' and '.' to avoid their
	    // special meaning.
	    जर (d - IObuff > IOSIZE - 10)	// getting too long!?
		थांबा;
	    निवड (*s)
	    {
		स्थिती '|':   STRCPY(d, "bar");
			    d += 3;
			    सुरूठेवा;
		स्थिती '"':   STRCPY(d, "quote");
			    d += 5;
			    सुरूठेवा;
		स्थिती '*':   *d++ = '.';
			    थांबा;
		स्थिती '?':   *d++ = '.';
			    सुरूठेवा;
		स्थिती '$':
		स्थिती '.':
		स्थिती '~':   *d++ = '\\';
			    थांबा;
	    }

	    // Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	    // ":help i_^_CTRL-D" work.
	    // Insert '-' before and after "CTRL-X" when applicable.
	    जर (*s < ' ' || (*s == '^' && s[1] && (ASCII_ISALPHA(s[1])
			   || vim_strchr((char_u *)"?@[\\]^", s[1]) != रिक्त)))
	    {
		जर (d > IObuff && d[-1] != '_' && d[-1] != '\\')
		    *d++ = '_';		// prepend a '_' to make x_CTRL-x
		STRCPY(d, "CTRL-");
		d += 5;
		जर (*s < ' ')
		{
		    *d++ = *s + '@';
		    जर (d[-1] == '\\')
			*d++ = '\\';	// double a backslash
		}
		अन्यथा
		    *d++ = *++s;
		जर (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		// append a '_'
		सुरूठेवा;
	    }
	    अन्यथा जर (*s == '^')		// "^" or "CTRL-^" or "^_"
		*d++ = '\\';

	    // Insert a backslash before a backslash after a slash, for search
	    // pattern tags: "/\|" --> "/\\|".
	    अन्यथा जर (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    // "CTRL-\_" -> "CTRL-\\_" to avoid the special meaning of "\_" in
	    // "CTRL-\_CTRL-N"
	    जर (STRNICMP(s, "CTRL-\\_", 7) == 0)
	    {
		STRCPY(d, "CTRL-\\\\");
		d += 7;
		s += 6;
	    }

	    *d++ = *s;

	    // If tag contains "({" or "([", tag terminates at the "(".
	    // This is for help on functions, e.g.: abs({expr}).
	    जर (*s == '(' && (s[1] == '{' || s[1] =='['))
		थांबा;

	    // If tag starts with ', toss everything after a second '. Fixes
	    // CTRL-] on 'option'. (would include the trailing '.').
	    जर (*s == '\'' && s > arg && *arg == '\'')
		थांबा;
	    // Also '{' and '}'.
	    जर (*s == '}' && s > arg && *arg == '{')
		थांबा;
	  }
	  *d = NUL;

	  जर (*IObuff == '`')
	  {
	      जर (d > IObuff + 2 && d[-1] == '`')
	      {
		  // remove the backticks from `command`
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-2] = NUL;
	      }
	      अन्यथा जर (d > IObuff + 3 && d[-2] == '`' && d[-1] == ',')
	      {
		  // remove the backticks and comma from `command`,
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-3] = NUL;
	      }
	      अन्यथा जर (d > IObuff + 4 && d[-3] == '`'
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
    *अंक_matches = 0;
    flags = TAG_HELP | TAG_REGEXP | TAG_NAMES | TAG_VERBOSE | TAG_NO_TAGFUNC;
    जर (keep_lang)
	flags |= TAG_KEEP_LANG;
    जर (find_tags(IObuff, अंक_matches, matches, flags, (पूर्णांक)MAXCOL, रिक्त) == OK
	    && *अंक_matches > 0)
    {
	// Sort the matches found on the heuristic number that is after the
	// tag name.
	क्विक((शून्य *)*matches, (माप_प्रकार)*अंक_matches,
					      आकार(char_u *), help_compare);
	// Delete more than TAG_MANY to reduce the size of the listing.
	पर्यंत (*अंक_matches > TAG_MANY)
	    vim_free((*matches)[--*अंक_matches]);
    }
    परत OK;
}

#ifdef FEAT_MULTI_LANG
/*
 * Cleanup matches for help tags:
 * Remove "@ab" if the top of 'helplang' is "ab" and the language of the first
 * tag matches it.  Otherwise remove "@en" if "en" is the only language.
 */
    शून्य
cleanup_help_tags(पूर्णांक अंक_file, char_u **file)
{
    पूर्णांक		i, j;
    पूर्णांक		len;
    char_u	buf[4];
    char_u	*p = buf;

    जर (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    साठी (i = 0; i < अंक_file; ++i)
    {
	len = (पूर्णांक)STRLEN(file[i]) - 3;
	जर (len <= 0)
	    सुरूठेवा;
	जर (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    साठी (j = 0; j < अंक_file; ++j)
		जर (j != i && (पूर्णांक)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    थांबा;
	    जर (j == अंक_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    जर (*buf != NUL)
	साठी (i = 0; i < अंक_file; ++i)
	{
	    len = (पूर्णांक)STRLEN(file[i]) - 3;
	    जर (len <= 0)
		सुरूठेवा;
	    जर (STRCMP(file[i] + len, buf) == 0)
	    {
		// remove the default language
		file[i][len] = NUL;
	    }
	}
}
#endif

/*
 * Called when starting to edit a buffer for a help file.
 */
    शून्य
prepare_help_buffer(शून्य)
{
    char_u	*p;

    curbuf->b_help = true;
#ifdef FEAT_QUICKFIX
    set_string_option_direct((char_u *)"buftype", -1,
				     (char_u *)"help", OPT_FREE|OPT_LOCAL, 0);
#endif

    // Always set these options after jumping to a help tag, because the
    // user may have an autocommand that gets in the way.
    // When adding an option here, also update the help file helphelp.txt.

    // Accept all ASCII chars for keywords, except ' ', '*', '"', '|', and
    // latin1 word characters (for translated help files).
    // Only set it when needed, buf_init_chartab() is some work.
    p = (char_u *)"!-~,^*,^|,^\",192-255";
    जर (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	set_string_option_direct((char_u *)"isk", -1, p, OPT_FREE|OPT_LOCAL, 0);
	check_buf_options(curbuf);
	(शून्य)buf_init_chartab(curbuf, FALSE);
    }

#ifdef FEAT_FOLDING
    // Don't use the global foldmethod.
    set_string_option_direct((char_u *)"fdm", -1, (char_u *)"manual",
						       OPT_FREE|OPT_LOCAL, 0);
#endif

    curbuf->b_p_ts = 8;		// 'tabstop' is 8
    curwin->w_p_list = FALSE;	// no list mode

    curbuf->b_p_ma = FALSE;	// not modifiable
    curbuf->b_p_bin = FALSE;	// reset 'bin' before reading file
    curwin->w_p_nu = 0;		// no line numbers
    curwin->w_p_rnu = 0;	// no relative line numbers
    RESET_BINDING(curwin);	// no scroll or cursor binding
#ifdef FEAT_ARABIC
    curwin->w_p_arab = FALSE;	// no arabic mode
#endif
#ifdef FEAT_RIGHTLEFT
    curwin->w_p_rl  = FALSE;	// help window is left-to-right
#endif
#ifdef FEAT_FOLDING
    curwin->w_p_fen = FALSE;	// No folding in the help window
#endif
#ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;	// No 'diff'
#endif
#ifdef FEAT_SPELL
    curwin->w_p_spell = FALSE;	// No spell checking
#endif

    set_buflisted(FALSE);
}

/*
 * After reading a help file: May cleanup a help buffer when syntax
 * highlighting is not used.
 */
    शून्य
fix_help_buffer(शून्य)
{
    linenr_T	lnum;
    char_u	*ओळ;
    पूर्णांक		in_example = FALSE;
    पूर्णांक		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    पूर्णांक		mustfree;

    // Set filetype to "help" if still needed.
    जर (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_lock;
	set_option_मूल्य_give_err((char_u *)"ft",
					      0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_lock;
    }

#ifdef FEAT_SYN_HL
    जर (!syntax_present(curwin))
#endif
    {
	साठी (lnum = 1; lnum <= curbuf->b_ml.ml_ओळ_मोजणी; ++lnum)
	{
	    ओळ = ml_get_buf(curbuf, lnum, FALSE);
	    len = ml_get_buf_len(curbuf, lnum);
	    जर (in_example && len > 0 && !VIM_ISWHITE(ओळ[0]))
	    {
		// End of example: non-white or '<' in first column.
		जर (ओळ[0] == '<')
		{
		    // blank-out a '<' in the first column
		    ओळ = ml_get_buf(curbuf, lnum, TRUE);
		    ओळ[0] = ' ';
		}
		in_example = FALSE;
	    }
	    जर (!in_example && len > 0)
	    {
		जर (ओळ[len - 1] == '>' && (len == 1 || ओळ[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    ओळ = ml_get_buf(curbuf, lnum, TRUE);
		    ओळ[len - 1] = ' ';
		    in_example = TRUE;
		}
		अन्यथा जर (ओळ[len - 1] == '~')
		{
		    // blank-out a '~' at the end of line (header marker)
		    ओळ = ml_get_buf(curbuf, lnum, TRUE);
		    ओळ[len - 1] = ' ';
		}
	    }
	}
    }

    // In the "help.txt" and "help.abx" file, add the locally added help
    // files.  This uses the very first line in the help file.
    fname = gettail(curbuf->b_fname);
    जर (fnamecmp(fname, "help.txt") == 0
#ifdef FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#endif
	)
    {
	साठी (lnum = 1; lnum < curbuf->b_ml.ml_ओळ_मोजणी; ++lnum)
	{
	    ओळ = ml_get_buf(curbuf, lnum, FALSE);
	    जर (म_माला((अक्षर *)ओळ, "*local-additions*") == रिक्त)
		सुरूठेवा;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    पर्यंत (*p != NUL)
	    {
		पूर्णांक NameBufflen;

		NameBufflen = copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		जर (rt != रिक्त &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    पूर्णांक		fcount;
		    char_u	**fnames;
		    खाता	*fd;
		    char_u	*s;
		    पूर्णांक		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    जर (*NameBuff != NUL && !after_pathsep(NameBuff, NameBuff + NameBufflen))
		    {
			STRCPY(NameBuff + NameBufflen, PATHSEPSTR);
			NameBufflen += आकार(PATHSEP);
		    }
#ifdef FEAT_MULTI_LANG
		    STRCPY(NameBuff + NameBufflen, "doc/*.??[tx]");
#अन्यथा
		    STRCPY(NameBuff + NameBufflen, "doc/*.txt");
#endif
		    जर (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#ifdef FEAT_MULTI_LANG
			पूर्णांक	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			साठी (i1 = 0; i1 < fcount; ++i1)
			{
			    f1 = fnames[i1];
			    t1 = gettail(f1);
			    e1 = vim_strrchr(t1, '.');
			    जर (e1 == रिक्त)
				सुरूठेवा;
			    जर (fnamecmp(e1, ".txt") != 0
					       && fnamecmp(e1, fname + 4) != 0)
			    {
				// Not .txt and not .abx, remove it.
				VIM_CLEAR(fnames[i1]);
				सुरूठेवा;
			    }

			    साठी (i2 = i1 + 1; i2 < fcount; ++i2)
			    {
				f2 = fnames[i2];
				जर (f2 == रिक्त)
				    सुरूठेवा;
				t2 = gettail(f2);
				e2 = vim_strrchr(t2, '.');
				जर (e2 == रिक्त)
				    सुरूठेवा;
				जर (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    सुरूठेवा;
				जर (fnamecmp(e1, ".txt") == 0
					       && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#endif
			साठी (fi = 0; fi < fcount; ++fi)
			{
			    जर (fnames[fi] == रिक्त)
				सुरूठेवा;
			    fd = mch_fopen((अक्षर *)fnames[fi], "r");
			    जर (fd != रिक्त)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				जर (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != रिक्त)
				{
				    पूर्णांक	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    पर्यंत (*s != NUL)
				    {
					जर (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					जर (*s >= 0x80 && this_utf != FALSE)
					{
					    पूर्णांक	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    जर (l == 1)
						this_utf = FALSE;
					    s += l - 1;
					}
					++s;
				    }

				    // The help file is latin1 or utf-8;
				    // conversion to the current
				    // 'encoding' may be required.
				    vc.vc_type = CONV_NONE;
				    convert_setup(&vc, (char_u *)(
						this_utf == TRUE ? "utf-8"
						      : "latin1"), p_enc);
				    जर (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    अन्यथा
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    रिक्त);
					जर (cp == रिक्त)
					    cp = IObuff;
				    }
				    convert_setup(&vc, रिक्त, रिक्त);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    जर (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				ख_बंद(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		जर (mustfree)
		    vim_free(rt);
	    }
	    थांबा;
	}
    }
}

/*
 * ":exusage"
 */
    शून्य
ex_exusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help ex-cmd-index");
}

/*
 * ":viusage"
 */
    शून्य
ex_viusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help normal-index");
}

/*
 * Generate tags in one help directory.
 */
    स्थिर शून्य
helptags_one(
    char_u	*dir,		// doc directory
    char_u	*ext,		// suffix, ".txt", ".itx", ".frx", etc.
    char_u	*tagfname,	// "tags" for English, "tags-fr" for French.
    पूर्णांक		add_help_tags,	// add "help-tags" tag
    पूर्णांक		ignore_writeerr)    // ignore write error
{
    खाता	*fd_tags;
    खाता	*fd;
    garray_T	ga;
    पूर्णांक		res;
    पूर्णांक		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    पूर्णांक		fi;
    char_u	*s;
    पूर्णांक		i;
    char_u	*fname;
    पूर्णांक		dirlen;
    पूर्णांक		utf8 = MAYBE;
    पूर्णांक		this_utf8;
    पूर्णांक		firstline;
    पूर्णांक		in_example;
    पूर्णांक		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (पूर्णांक)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    res = gen_expand_wildcards(1, &NameBuff, &filecount, &files,
							    EW_FILE|EW_SILENT);
    जर (res == FAIL || filecount == 0)
    {
	जर (!got_int)
	    semsg(_(e_no_match_str_1), NameBuff);
	जर (res != FAIL)
	    FreeWild(filecount, files);
	परत;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_tags = mch_fopen((अक्षर *)NameBuff, "w");
    जर (fd_tags == रिक्त)
    {
	जर (!ignore_writeerr)
	    semsg(_(e_cannot_open_str_for_writing_1), NameBuff);
	FreeWild(filecount, files);
	परत;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, आकार(char_u *), 100);
    जर (add_help_tags || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	जर (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	अन्यथा
	{
	    s = alloc(18 + (अचिन्हित)STRLEN(tagfname));
	    जर (s == रिक्त)
		got_int = TRUE;
	    अन्यथा
	    {
		प्र_लिखो((अक्षर *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_data)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    साठी (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((अक्षर *)files[fi], "r");
	जर (fd == रिक्त)
	{
	    semsg(_(e_unable_to_open_str_for_reading), files[fi]);
	    सुरूठेवा;
	}
	fname = files[fi] + dirlen + 1;

	in_example = FALSE;
	firstline = TRUE;
	पर्यंत (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    जर (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		साठी (s = IObuff; *s != NUL; ++s)
		    जर (*s >= 0x80)
		    {
			पूर्णांक l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			जर (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    थांबा;
			}
			s += l - 1;
		    }
		जर (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		जर (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		अन्यथा जर (utf8 != this_utf8)
		{
		    semsg(_(e_mix_of_help_file_encodings_within_language_str), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    जर (in_example)
	    {
		// skip over example; a non-white in the first column ends it
		जर (vim_strchr((char_u *)" \t\n\r", IObuff[0]))
		    सुरूठेवा;
		in_example = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    पर्यंत (p1 != रिक्त)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		जर (p2 != रिक्त && p2 > p1 + 1)	// skip "*" and "**"
		{
		    साठी (s = p1 + 1; s < p2; ++s)
			जर (*s == ' ' || *s == '\t' || *s == '|')
			    थांबा;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    जर (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != रिक्त
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			जर (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    थांबा;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			जर (s == रिक्त)
			{
			    got_int = TRUE;
			    थांबा;
			}
			((char_u **)ga.ga_data)[ga.ga_len] = s;
			++ga.ga_len;
			प्र_लिखो((अक्षर *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    माप_प्रकार off = STRLEN(IObuff);
	    जर (off >= 2 && IObuff[off - 1] == '\n')
	    {
		off -= 2;
		पर्यंत (off > 0 && (ASCII_ISLOWER(IObuff[off])
						  || VIM_ISDIGIT(IObuff[off])))
		    off--;
		जर (IObuff[off] == '>' && (off == 0 || IObuff[off - 1] == ' '))
		    in_example = TRUE;
	    }
	    ओळ_breakcheck();
	}

	ख_बंद(fd);
    }

    FreeWild(filecount, files);

    जर (!got_int)
    {
	// Sort the tags.
	जर (ga.ga_data != रिक्त)
	    sort_strings((char_u **)ga.ga_data, ga.ga_len);

	// Check for duplicates.
	साठी (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_data)[i - 1];
	    p2 = ((char_u **)ga.ga_data)[i];
	    पर्यंत (*p1 == *p2)
	    {
		जर (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((अक्षर *)NameBuff, MAXPATHL,
			    _(e_duplicate_tag_str_in_file_str_str),
				     ((char_u **)ga.ga_data)[i], dir, p2 + 1);
		    emsg((अक्षर *)NameBuff);
		    *p2 = '\t';
		    थांबा;
		}
		++p1;
		++p2;
	    }
	}

	जर (utf8 == TRUE)
	    ख_लिखो(fd_tags, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	साठी (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_data)[i];
	    जर (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		ख_माला_दो((अक्षर *)s, fd_tags);
	    अन्यथा
	    {
		ख_लिखो(fd_tags, "%s\t/*", s);
		साठी (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    जर (*p1 == '\\' || *p1 == '/')
			अ_दो('\\', fd_tags);
		    अ_दो(*p1, fd_tags);
		}
		ख_लिखो(fd_tags, "*\n");
	    }
	}
    }
    जर (mix)
	got_int = FALSE;    // continue with other languages

    साठी (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_data)[i]);
    ga_clear(&ga);
    ख_बंद(fd_tags);	    // there is no check for an error...
}

/*
 * Generate tags in one help directory, taking care of translations.
 */
    स्थिर शून्य
do_helptags(char_u *स_नाम, पूर्णांक add_help_tags, पूर्णांक ignore_writeerr)
{
#ifdef FEAT_MULTI_LANG
    पूर्णांक		len;
    पूर्णांक		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    पूर्णांक		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, स_नाम);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    जर (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_(e_no_match_str_1), NameBuff);
	परत;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    साठी (i = 0; i < filecount; ++i)
    {
	len = (पूर्णांक)STRLEN(files[i]);
	जर (len <= 4)
	    सुरूठेवा;

	जर (STRICMP(files[i] + len - 4, ".txt") == 0)
	{
	    // ".txt" -> language "en"
	    lang[0] = 'e';
	    lang[1] = 'n';
	}
	अन्यथा जर (files[i][len - 4] == '.'
		&& ASCII_ISALPHA(files[i][len - 3])
		&& ASCII_ISALPHA(files[i][len - 2])
		&& TOLOWER_ASC(files[i][len - 1]) == 'x')
	{
	    // ".abx" -> language "ab"
	    lang[0] = TOLOWER_ASC(files[i][len - 3]);
	    lang[1] = TOLOWER_ASC(files[i][len - 2]);
	}
	अन्यथा
	    सुरूठेवा;

	// Did we find this language already?
	साठी (j = 0; j < ga.ga_len; j += 2)
	    जर (STRNCMP(lang, ((char_u *)ga.ga_data) + j, 2) == 0)
		थांबा;
	जर (j == ga.ga_len)
	{
	    // New language, add it.
	    जर (ga_grow(&ga, 2) == FAIL)
		थांबा;
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[0];
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[1];
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    साठी (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_data)[j];
	fname[6] = ((char_u *)ga.ga_data)[j + 1];
	जर (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	अन्यथा
	{
	    // Language "ab" uses ".abx" and "tags-ab".
	    STRCPY(ext, ".xxx");
	    ext[1] = fname[5];
	    ext[2] = fname[6];
	}
	helptags_one(स_नाम, ext, fname, add_help_tags, ignore_writeerr);
    }

    ga_clear(&ga);
    FreeWild(filecount, files);

#अन्यथा
    // No language support, just use "*.txt" and "tags".
    helptags_one(स_नाम, (char_u *)".txt", (char_u *)"tags", add_help_tags,
							    ignore_writeerr);
#endif
}

    स्थिर शून्य
helptags_cb(char_u *fname, शून्य *cookie)
{
    do_helptags(fname, *(पूर्णांक *)cookie, TRUE);
}

/*
 * ":helptags"
 */
    शून्य
ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*स_नाम;
    पूर्णांक		add_help_tags = FALSE;

    // Check for ":helptags ++t {dir}".
    जर (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	add_help_tags = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    जर (STRCMP(eap->arg, "ALL") == 0)
    {
	do_in_path(p_rtp, "", (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &add_help_tags);
    }
    अन्यथा
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	स_नाम = ExpandOne(&xpc, eap->arg, रिक्त,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	जर (स_नाम == रिक्त || !mch_isdir(स_नाम))
	    semsg(_(e_not_a_directory_str), eap->arg);
	अन्यथा
	    do_helptags(स_नाम, add_help_tags, FALSE);
	vim_free(स_नाम);
    }
}
