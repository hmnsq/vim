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

#అన్తర్భావ "vim.h"

/*
 * ":help": open a read-only window on a help file
 */
    శూన్య
ex_help(exarg_T *eap)
{
    char_u	*arg;
    char_u	*ట్యాగ్;
    FILE	*helpfd;	// file descriptor of help file
    పూర్ణాంకం		n;
    పూర్ణాంకం		i;
    win_T	*wp;
    పూర్ణాంకం		అంకె_matches;
    char_u	**matches;
    char_u	*p;
    పూర్ణాంకం		ఖాళీలు_fnum = 0;
    పూర్ణాంకం		alt_fnum = 0;
    buf_T	*buf;
#నిర్వచితమైతే FEAT_MULTI_LANG
    పూర్ణాంకం		len;
    char_u	*lang;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
    పూర్ణాంకం		old_కీలకం_Typed = కీలకం_Typed;
#అయితే_ముగింపు

    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;

    ఒకవేళ (eap != ఖాళీ)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	కొరకు (arg = eap->arg; *arg; ++arg)
	{
	    ఒకవేళ (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		ఆపు;
	    }
	}
	arg = eap->arg;

	ఒకవేళ (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_(e_dont_panic));
	    తిరిగి;
	}

	ఒకవేళ (eap->skip)	    // not executing commands
	    తిరిగి;
    }
    లేకపోతే
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    అయితే (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#నిర్వచితమైతే FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#అయితే_ముగింపు

    // When no argument given go to the index.
    ఒకవేళ (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = కనుగొను_help_ట్యాగులు(arg, &అంకె_matches, &matches,
						 eap != ఖాళీ && eap->forceit);

    i = 0;
#నిర్వచితమైతే FEAT_MULTI_LANG
    ఒకవేళ (n != FAIL && lang != ఖాళీ)
	// Find first item with the requested language.
	కొరకు (i = 0; i < అంకె_matches; ++i)
	{
	    len = (పూర్ణాంకం)STRLEN(matches[i]);
	    ఒకవేళ (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		ఆపు;
	}
#అయితే_ముగింపు
    ఒకవేళ (i >= అంకె_matches || n == FAIL)
    {
#నిర్వచితమైతే FEAT_MULTI_LANG
	ఒకవేళ (lang != ఖాళీ)
	    semsg(_(e_sorry_no_str_help_for_str), lang, arg);
	లేకపోతే
#అయితే_ముగింపు
	    semsg(_(e_sorry_no_help_for_str), arg);
	ఒకవేళ (n != FAIL)
	    FreeWild(అంకె_matches, matches);
	తిరిగి;
    }

    // The first match (in the requested language) is the best match.
    ట్యాగ్ = vim_strsave(matches[i]);
    FreeWild(అంకె_matches, matches);

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    ఒకవేళ (!bt_help(curwin->w_బఫరు) || cmdmod.cmod_tab != 0)
    {
	ఒకవేళ (cmdmod.cmod_tab != 0)
	    wp = ఖాళీ;
	లేకపోతే
	    FOR_ALL_WINDOWS(wp)
		ఒకవేళ (bt_help(wp->w_బఫరు))
		    ఆపు;
	ఒకవేళ (wp != ఖాళీ && wp->w_బఫరు->b_nwindows > 0)
	    win_enter(wp, TRUE);
	లేకపోతే
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    ఒకవేళ ((helpfd = mch_fopen((అక్షరం *)p_hf, READBIN)) == ఖాళీ)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		వెళ్ళు erret;
	    }
	    fclose(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    ఒకవేళ (cmdmod.cmod_విభజించు == 0 && curwin->w_వెడల్పు != Columns
						  && curwin->w_వెడల్పు < 80)
		n |= p_sb ? WSP_BOT : WSP_శిఖరం;
	    ఒకవేళ (win_విభజించు(0, n) == FAIL)
		వెళ్ళు erret;

	    ఒకవేళ (curwin->w_ఎత్తు < p_hh)
		win_setheight((పూర్ణాంకం)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (శూన్య)do_ecmd(0, ఖాళీ, ఖాళీ, ఖాళీ, ECMD_LASTL,
			  ECMD_HIDE + ECMD_సమితి_HELP,
			  ఖాళీ);  // buffer is still open, don't store info
	    ఒకవేళ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    ఖాళీలు_fnum = curbuf->b_fnum;
	}
    }

    ఒకవేళ (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#నిర్వచితమైతే FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    కీలకం_Typed = old_కీలకం_Typed;
#అయితే_ముగింపు

    ఒకవేళ (ట్యాగ్ != ఖాళీ)
	do_ట్యాగ్(ట్యాగ్, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    ఒకవేళ (ఖాళీలు_fnum != 0 && curbuf->b_fnum != ఖాళీలు_fnum)
    {
	buf = buflist_findnr(ఖాళీలు_fnum);
	ఒకవేళ (buf != ఖాళీ && buf->b_nwindows == 0)
	    wipe_బఫరు(buf, TRUE);
    }

    // keep the previous alternate file
    ఒకవేళ (alt_fnum != 0 && curwin->w_alt_fnum == ఖాళీలు_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(ట్యాగ్);
}

/*
 * ":helpclose": Close one help window
 */
    శూన్య
ex_helpclose(exarg_T *eap వాడని)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	ఒకవేళ (bt_help(win->w_బఫరు))
	{
	    win_close(win, FALSE);
	    తిరిగి;
	}
    }
}

#ఒకవేళ defined(FEAT_MULTI_LANG)
/*
 * In an argument search for a language specifiers in the form "@xx".
 * Changes the "@" to NUL if found, and returns a pointer to "xx".
 * Returns NULL if not found.
 */
    char_u *
check_help_lang(char_u *arg)
{
    పూర్ణాంకం len = (పూర్ణాంకం)STRLEN(arg);

    ఒకవేళ (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	తిరిగి arg + len - 2;
    }
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

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
    పూర్ణాంకం
help_heuristic(
    char_u	*matched_string,
    పూర్ణాంకం		స్థానభ్రంశం,			// offset for match
    పూర్ణాంకం		wrong_case)		// no matching case
{
    పూర్ణాంకం		అంకె_letters;
    char_u	*p;

    అంకె_letters = 0;
    కొరకు (p = matched_string; *p; p++)
	ఒకవేళ (ASCII_ISALNUM(*p))
	    అంకె_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    ఒకవేళ (ASCII_ISALNUM(matched_string[స్థానభ్రంశం]) && స్థానభ్రంశం > 0
				 && ASCII_ISALNUM(matched_string[స్థానభ్రంశం - 1]))
	స్థానభ్రంశం += 10000;
    లేకపోతే ఒకవేళ (స్థానభ్రంశం > 2)
	స్థానభ్రంశం *= 200;
    ఒకవేళ (wrong_case)
	స్థానభ్రంశం += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    ఒకవేళ (matched_string[0] == '+' && matched_string[1] != NUL)
	స్థానభ్రంశం += 100;
    తిరిగి (పూర్ణాంకం)(100 * అంకె_letters + STRLEN(matched_string) + స్థానభ్రంశం);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    స్థిర పూర్ణాంకం
help_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    అక్షరం    *p1;
    అక్షరం    *p2;
    పూర్ణాంకం	    cmp;

    p1 = *(అక్షరం **)s1 + అక్షరశ్రేణి_పొడవు(*(అక్షరం **)s1) + 1;
    p2 = *(అక్షరం **)s2 + అక్షరశ్రేణి_పొడవు(*(అక్షరం **)s2) + 1;

    // Compare by help heuristic number first.
    cmp = strcmp(p1, p2);
    ఒకవేళ (cmp != 0)
	తిరిగి cmp;

    // Compare by strings as tie-breaker when same heuristic number.
    తిరిగి strcmp(*(అక్షరం **)s1, *(అక్షరం **)s2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * The matches will be sorted with a "best" match algorithm.
 * When "keep_lang" is TRUE try keeping the language of the current buffer.
 */
    పూర్ణాంకం
కనుగొను_help_ట్యాగులు(
    char_u	*arg,
    పూర్ణాంకం		*అంకె_matches,
    char_u	***matches,
    పూర్ణాంకం		keep_lang)
{
    char_u	*s, *d;
    పూర్ణాంకం		i;
    // Specific tags that either have a specific replacement or won't go
    // through the generic rules.
    స్థిర అక్షరం *(except_tbl[][2]) = {
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
	{ఖాళీ, ఖాళీ}
    };
    స్థిర అక్షరం *(సమాసం_పట్టిక[]) = {"!=?", "!~?", "<=?", "<?", "==?", "=~?",
				   ">=?", ">?", "is?", "isnot?"};
    పూర్ణాంకం flags;

    d = IObuff;		    // assume IObuff is long enough!
    d[0] = NUL;

    ఒకవేళ (STRNICMP(arg, "expr-", 5) == 0)
    {
	// When the string starting with "expr-" and containing '?' and matches
	// the table, it is taken literally (but ~ is escaped).  Otherwise '?'
	// is recognized as a wildcard.
	కొరకు (i = (పూర్ణాంకం)ARRAY_నిడివి(సమాసం_పట్టిక); --i >= 0; )
	    ఒకవేళ (STRCMP(arg + 5, సమాసం_పట్టిక[i]) == 0)
	    {
		పూర్ణాంకం si = 0, di = 0;

		కొరకు (;;)
		{
		    ఒకవేళ (arg[si] == '~')
			d[di++] = '\\';
		    d[di++] = arg[si];
		    ఒకవేళ (arg[si] == NUL)
			ఆపు;
		    ++si;
		}
		ఆపు;
	    }
    }
    లేకపోతే
    {
	// Recognize a few exceptions to the rule.  Some strings that contain
	// '*'are changed to "star", otherwise '*' is recognized as a wildcard.
	కొరకు (i = 0; except_tbl[i][0] != ఖాళీ; ++i)
	    ఒకవేళ (STRCMP(arg, except_tbl[i][0]) == 0)
	    {
		STRCPY(d, except_tbl[i][1]);
		ఆపు;
	    }
    }

    ఒకవేళ (d[0] == NUL)	// no match in table
    {
	// Replace "\S" with "/\\S", etc.  Otherwise every tag is matched.
	// Also replace "\%^" and "\%(", they match every tag too.
	// Also "\zs", "\z1", etc.
	// Also "\@<", "\@=", "\@<=", etc.
	// And also "\_$" and "\_^".
	ఒకవేళ (arg[0] == '\\'
		&& ((arg[1] != NUL && arg[2] == NUL)
		    || (vim_strchr((char_u *)"%_z@", arg[1]) != ఖాళీ
							   && arg[2] != NUL)))
	{
	    vim_snprintf((అక్షరం *)d, IOSIZE, "/\\\\%s", arg + 1);
	    // Check for "/\\_$", should be "/\\_\$"
	    ఒకవేళ (d[3] == '_' && d[4] == '$')
		STRCPY(d + 4, "\\$");
	}
	లేకపోతే
	{
	  // Replace:
	  // "[:...:]" with "\[:...:]"
	  // "[++...]" with "\[++...]"
	  // "\{" with "\\{"		   -- matching "} \}"
	    ఒకవేళ ((arg[0] == '[' && (arg[1] == ':'
			 || (arg[1] == '+' && arg[2] == '+')))
		    || (arg[0] == '\\' && arg[1] == '{'))
	      *d++ = '\\';

	  // If tag starts with "('", skip the "(". Fixes CTRL-] on ('option'.
	  ఒకవేళ (*arg == '(' && arg[1] == '\'')
	      arg++;
	  కొరకు (s = arg; *s; ++s)
	  {
	    // Replace "|" with "bar" and '"' with "quote" to match the name of
	    // the tags for these commands.
	    // Replace "*" with ".*" and "?" with "." to match command line
	    // completion.
	    // Insert a backslash before '~', '$' and '.' to avoid their
	    // special meaning.
	    ఒకవేళ (d - IObuff > IOSIZE - 10)	// getting too long!?
		ఆపు;
	    ఎంపిక (*s)
	    {
		స్థితి '|':   STRCPY(d, "bar");
			    d += 3;
			    కొనసాగు;
		స్థితి '"':   STRCPY(d, "quote");
			    d += 5;
			    కొనసాగు;
		స్థితి '*':   *d++ = '.';
			    ఆపు;
		స్థితి '?':   *d++ = '.';
			    కొనసాగు;
		స్థితి '$':
		స్థితి '.':
		స్థితి '~':   *d++ = '\\';
			    ఆపు;
	    }

	    // Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	    // ":help i_^_CTRL-D" work.
	    // Insert '-' before and after "CTRL-X" when applicable.
	    ఒకవేళ (*s < ' ' || (*s == '^' && s[1] && (ASCII_ISALPHA(s[1])
			   || vim_strchr((char_u *)"?@[\\]^", s[1]) != ఖాళీ)))
	    {
		ఒకవేళ (d > IObuff && d[-1] != '_' && d[-1] != '\\')
		    *d++ = '_';		// prepend a '_' to make x_CTRL-x
		STRCPY(d, "CTRL-");
		d += 5;
		ఒకవేళ (*s < ' ')
		{
		    *d++ = *s + '@';
		    ఒకవేళ (d[-1] == '\\')
			*d++ = '\\';	// double a backslash
		}
		లేకపోతే
		    *d++ = *++s;
		ఒకవేళ (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		// append a '_'
		కొనసాగు;
	    }
	    లేకపోతే ఒకవేళ (*s == '^')		// "^" or "CTRL-^" or "^_"
		*d++ = '\\';

	    // Insert a backslash before a backslash after a slash, for search
	    // pattern tags: "/\|" --> "/\\|".
	    లేకపోతే ఒకవేళ (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    // "CTRL-\_" -> "CTRL-\\_" to avoid the special meaning of "\_" in
	    // "CTRL-\_CTRL-N"
	    ఒకవేళ (STRNICMP(s, "CTRL-\\_", 7) == 0)
	    {
		STRCPY(d, "CTRL-\\\\");
		d += 7;
		s += 6;
	    }

	    *d++ = *s;

	    // If tag contains "({" or "([", tag terminates at the "(".
	    // This is for help on functions, e.g.: abs({expr}).
	    ఒకవేళ (*s == '(' && (s[1] == '{' || s[1] =='['))
		ఆపు;

	    // If tag starts with ', toss everything after a second '. Fixes
	    // CTRL-] on 'option'. (would include the trailing '.').
	    ఒకవేళ (*s == '\'' && s > arg && *arg == '\'')
		ఆపు;
	    // Also '{' and '}'.
	    ఒకవేళ (*s == '}' && s > arg && *arg == '{')
		ఆపు;
	  }
	  *d = NUL;

	  ఒకవేళ (*IObuff == '`')
	  {
	      ఒకవేళ (d > IObuff + 2 && d[-1] == '`')
	      {
		  // remove the backticks from `command`
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-2] = NUL;
	      }
	      లేకపోతే ఒకవేళ (d > IObuff + 3 && d[-2] == '`' && d[-1] == ',')
	      {
		  // remove the backticks and comma from `command`,
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-3] = NUL;
	      }
	      లేకపోతే ఒకవేళ (d > IObuff + 4 && d[-3] == '`'
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
    *అంకె_matches = 0;
    flags = ట్యాగ్_HELP | ట్యాగ్_REGEXP | ట్యాగ్_పేర్లు | ట్యాగ్_VERBOSE | ట్యాగ్_NO_TAGFUNC;
    ఒకవేళ (keep_lang)
	flags |= ట్యాగ్_KEEP_LANG;
    ఒకవేళ (కనుగొను_ట్యాగులు(IObuff, అంకె_matches, matches, flags, (పూర్ణాంకం)MAXCOL, ఖాళీ) == OK
	    && *అంకె_matches > 0)
    {
	// Sort the matches found on the heuristic number that is after the
	// tag name.
	qsort((శూన్య *)*matches, (size_t)*అంకె_matches,
					      పరిమాణం(char_u *), help_compare);
	// Delete more than TAG_MANY to reduce the size of the listing.
	అయితే (*అంకె_matches > ట్యాగ్_MANY)
	    vim_free((*matches)[--*అంకె_matches]);
    }
    తిరిగి OK;
}

#నిర్వచితమైతే FEAT_MULTI_LANG
/*
 * Cleanup matches for help tags:
 * Remove "@ab" if the top of 'helplang' is "ab" and the language of the first
 * tag matches it.  Otherwise remove "@en" if "en" is the only language.
 */
    శూన్య
cleanup_help_ట్యాగులు(పూర్ణాంకం అంకె_file, char_u **file)
{
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		len;
    char_u	buf[4];
    char_u	*p = buf;

    ఒకవేళ (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    కొరకు (i = 0; i < అంకె_file; ++i)
    {
	len = (పూర్ణాంకం)STRLEN(file[i]) - 3;
	ఒకవేళ (len <= 0)
	    కొనసాగు;
	ఒకవేళ (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    కొరకు (j = 0; j < అంకె_file; ++j)
		ఒకవేళ (j != i && (పూర్ణాంకం)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    ఆపు;
	    ఒకవేళ (j == అంకె_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    ఒకవేళ (*buf != NUL)
	కొరకు (i = 0; i < అంకె_file; ++i)
	{
	    len = (పూర్ణాంకం)STRLEN(file[i]) - 3;
	    ఒకవేళ (len <= 0)
		కొనసాగు;
	    ఒకవేళ (STRCMP(file[i] + len, buf) == 0)
	    {
		// remove the default language
		file[i][len] = NUL;
	    }
	}
}
#అయితే_ముగింపు

/*
 * Called when starting to edit a buffer for a help file.
 */
    శూన్య
prepare_help_బఫరు(శూన్య)
{
    char_u	*p;

    curbuf->b_help = true;
#నిర్వచితమైతే FEAT_QUICKFIX
    సమితి_string_option_ప్రత్యక్ష((char_u *)"buftype", -1,
				     (char_u *)"help", OPT_FREE|OPT_LOCAL, 0);
#అయితే_ముగింపు

    // Always set these options after jumping to a help tag, because the
    // user may have an autocommand that gets in the way.
    // When adding an option here, also update the help file helphelp.txt.

    // Accept all ASCII chars for keywords, except ' ', '*', '"', '|', and
    // latin1 word characters (for translated help files).
    // Only set it when needed, buf_init_chartab() is some work.
    p = (char_u *)"!-~,^*,^|,^\",192-255";
    ఒకవేళ (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	సమితి_string_option_ప్రత్యక్ష((char_u *)"isk", -1, p, OPT_FREE|OPT_LOCAL, 0);
	check_buf_options(curbuf);
	(శూన్య)buf_init_chartab(curbuf, FALSE);
    }

#నిర్వచితమైతే FEAT_FOLDING
    // Don't use the global foldmethod.
    సమితి_string_option_ప్రత్యక్ష((char_u *)"fdm", -1, (char_u *)"manual",
						       OPT_FREE|OPT_LOCAL, 0);
#అయితే_ముగింపు

    curbuf->b_p_ts = 8;		// 'tabstop' is 8
    curwin->w_p_list = FALSE;	// no list mode

    curbuf->b_p_ma = FALSE;	// not modifiable
    curbuf->b_p_bin = FALSE;	// reset 'bin' before reading file
    curwin->w_p_nu = 0;		// no line numbers
    curwin->w_p_rnu = 0;	// no relative line numbers
    RESET_BINDING(curwin);	// no scroll or cursor binding
#నిర్వచితమైతే FEAT_ARABIC
    curwin->w_p_arab = FALSE;	// no arabic mode
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RIGHTLEFT
    curwin->w_p_rl  = FALSE;	// help window is left-to-right
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
    curwin->w_p_fen = FALSE;	// No folding in the help window
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
    curwin->w_p_diff = FALSE;	// No 'diff'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
    curwin->w_p_spell = FALSE;	// No spell checking
#అయితే_ముగింపు

    సమితి_buflisted(FALSE);
}

/*
 * After reading a help file: May cleanup a help buffer when syntax
 * highlighting is not used.
 */
    శూన్య
fix_help_బఫరు(శూన్య)
{
    linenr_T	lnum;
    char_u	*పంక్తి;
    పూర్ణాంకం		లోపల_example = FALSE;
    పూర్ణాంకం		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    పూర్ణాంకం		mustfree;

    // Set filetype to "help" if still needed.
    ఒకవేళ (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_తాళం;
	సమితి_option_విలువ_give_err((char_u *)"ft",
					      0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_తాళం;
    }

#నిర్వచితమైతే FEAT_SYN_HL
    ఒకవేళ (!syntax_present(curwin))
#అయితే_ముగింపు
    {
	కొరకు (lnum = 1; lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క; ++lnum)
	{
	    పంక్తి = ml_get_buf(curbuf, lnum, FALSE);
	    len = ml_get_buf_len(curbuf, lnum);
	    ఒకవేళ (లోపల_example && len > 0 && !VIM_ISWHITE(పంక్తి[0]))
	    {
		// End of example: non-white or '<' in first column.
		ఒకవేళ (పంక్తి[0] == '<')
		{
		    // blank-out a '<' in the first column
		    పంక్తి = ml_get_buf(curbuf, lnum, TRUE);
		    పంక్తి[0] = ' ';
		}
		లోపల_example = FALSE;
	    }
	    ఒకవేళ (!లోపల_example && len > 0)
	    {
		ఒకవేళ (పంక్తి[len - 1] == '>' && (len == 1 || పంక్తి[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    పంక్తి = ml_get_buf(curbuf, lnum, TRUE);
		    పంక్తి[len - 1] = ' ';
		    లోపల_example = TRUE;
		}
		లేకపోతే ఒకవేళ (పంక్తి[len - 1] == '~')
		{
		    // blank-out a '~' at the end of line (header marker)
		    పంక్తి = ml_get_buf(curbuf, lnum, TRUE);
		    పంక్తి[len - 1] = ' ';
		}
	    }
	}
    }

    // In the "help.txt" and "help.abx" file, add the locally added help
    // files.  This uses the very first line in the help file.
    fname = gettail(curbuf->b_fname);
    ఒకవేళ (fnamecmp(fname, "help.txt") == 0
#నిర్వచితమైతే FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#అయితే_ముగింపు
	)
    {
	కొరకు (lnum = 1; lnum < curbuf->b_ml.ml_పంక్తి_లెక్క; ++lnum)
	{
	    పంక్తి = ml_get_buf(curbuf, lnum, FALSE);
	    ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)పంక్తి, "*local-additions*") == ఖాళీ)
		కొనసాగు;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    అయితే (*p != NUL)
	    {
		పూర్ణాంకం NameBufflen;

		NameBufflen = copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		ఒకవేళ (rt != ఖాళీ &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    పూర్ణాంకం		fcount;
		    char_u	**fnames;
		    FILE	*fd;
		    char_u	*s;
		    పూర్ణాంకం		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    ఒకవేళ (*NameBuff != NUL && !after_pathsep(NameBuff, NameBuff + NameBufflen))
		    {
			STRCPY(NameBuff + NameBufflen, PATHSEPSTR);
			NameBufflen += పరిమాణం(PATHSEP);
		    }
#నిర్వచితమైతే FEAT_MULTI_LANG
		    STRCPY(NameBuff + NameBufflen, "doc/*.??[tx]");
#లేకపోతే
		    STRCPY(NameBuff + NameBufflen, "doc/*.txt");
#అయితే_ముగింపు
		    ఒకవేళ (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#నిర్వచితమైతే FEAT_MULTI_LANG
			పూర్ణాంకం	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			కొరకు (i1 = 0; i1 < fcount; ++i1)
			{
			    f1 = fnames[i1];
			    t1 = gettail(f1);
			    e1 = vim_strrchr(t1, '.');
			    ఒకవేళ (e1 == ఖాళీ)
				కొనసాగు;
			    ఒకవేళ (fnamecmp(e1, ".txt") != 0
					       && fnamecmp(e1, fname + 4) != 0)
			    {
				// Not .txt and not .abx, remove it.
				VIM_CLEAR(fnames[i1]);
				కొనసాగు;
			    }

			    కొరకు (i2 = i1 + 1; i2 < fcount; ++i2)
			    {
				f2 = fnames[i2];
				ఒకవేళ (f2 == ఖాళీ)
				    కొనసాగు;
				t2 = gettail(f2);
				e2 = vim_strrchr(t2, '.');
				ఒకవేళ (e2 == ఖాళీ)
				    కొనసాగు;
				ఒకవేళ (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    కొనసాగు;
				ఒకవేళ (fnamecmp(e1, ".txt") == 0
					       && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#అయితే_ముగింపు
			కొరకు (fi = 0; fi < fcount; ++fi)
			{
			    ఒకవేళ (fnames[fi] == ఖాళీ)
				కొనసాగు;
			    fd = mch_fopen((అక్షరం *)fnames[fi], "r");
			    ఒకవేళ (fd != ఖాళీ)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				ఒకవేళ (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != ఖాళీ)
				{
				    పూర్ణాంకం	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    అయితే (*s != NUL)
				    {
					ఒకవేళ (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					ఒకవేళ (*s >= 0x80 && this_utf != FALSE)
					{
					    పూర్ణాంకం	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    ఒకవేళ (l == 1)
						this_utf = FALSE;
					    s += l - 1;
					}
					++s;
				    }

				    // The help file is latin1 or utf-8;
				    // conversion to the current
				    // 'encoding' may be required.
				    vc.vc_type = CONV_NONE;
				    convert_అమరిక(&vc, (char_u *)(
						this_utf == TRUE ? "utf-8"
						      : "latin1"), p_enc);
				    ఒకవేళ (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    లేకపోతే
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    ఖాళీ);
					ఒకవేళ (cp == ఖాళీ)
					    cp = IObuff;
				    }
				    convert_అమరిక(&vc, ఖాళీ, ఖాళీ);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    ఒకవేళ (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				fclose(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		ఒకవేళ (mustfree)
		    vim_free(rt);
	    }
	    ఆపు;
	}
    }
}

/*
 * ":exusage"
 */
    శూన్య
ex_exusage(exarg_T *eap వాడని)
{
    do_cmdline_cmd((char_u *)"help ex-cmd-index");
}

/*
 * ":viusage"
 */
    శూన్య
ex_viusage(exarg_T *eap వాడని)
{
    do_cmdline_cmd((char_u *)"help normal-index");
}

/*
 * Generate tags in one help directory.
 */
    స్థిర శూన్య
helptags_one(
    char_u	*dir,		// doc directory
    char_u	*ext,		// suffix, ".txt", ".itx", ".frx", etc.
    char_u	*tagfname,	// "tags" for English, "tags-fr" for French.
    పూర్ణాంకం		కూడు_help_ట్యాగులు,	// add "help-tags" tag
    పూర్ణాంకం		ignore_writeerr)    // ignore write error
{
    FILE	*fd_ట్యాగులు;
    FILE	*fd;
    garray_T	ga;
    పూర్ణాంకం		res;
    పూర్ణాంకం		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    పూర్ణాంకం		fi;
    char_u	*s;
    పూర్ణాంకం		i;
    char_u	*fname;
    పూర్ణాంకం		dirlen;
    పూర్ణాంకం		utf8 = MAYBE;
    పూర్ణాంకం		this_utf8;
    పూర్ణాంకం		firstline;
    పూర్ణాంకం		లోపల_example;
    పూర్ణాంకం		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (పూర్ణాంకం)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    res = gen_expand_wildcards(1, &NameBuff, &filecount, &files,
							    EW_FILE|EW_SILENT);
    ఒకవేళ (res == FAIL || filecount == 0)
    {
	ఒకవేళ (!got_int)
	    semsg(_(e_no_సరిపోలిక_str_1), NameBuff);
	ఒకవేళ (res != FAIL)
	    FreeWild(filecount, files);
	తిరిగి;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    కూడు_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_ట్యాగులు = mch_fopen((అక్షరం *)NameBuff, "w");
    ఒకవేళ (fd_ట్యాగులు == ఖాళీ)
    {
	ఒకవేళ (!ignore_writeerr)
	    semsg(_(e_cannot_open_str_for_writing_1), NameBuff);
	FreeWild(filecount, files);
	తిరిగి;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, పరిమాణం(char_u *), 100);
    ఒకవేళ (కూడు_help_ట్యాగులు || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	ఒకవేళ (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	లేకపోతే
	{
	    s = alloc(18 + (గుర్తులేని)STRLEN(tagfname));
	    ఒకవేళ (s == ఖాళీ)
		got_int = TRUE;
	    లేకపోతే
	    {
		sprintf((అక్షరం *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_దత్తాంశం)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    కొరకు (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((అక్షరం *)files[fi], "r");
	ఒకవేళ (fd == ఖాళీ)
	{
	    semsg(_(e_unable_గమ్యం_open_str_for_reading), files[fi]);
	    కొనసాగు;
	}
	fname = files[fi] + dirlen + 1;

	లోపల_example = FALSE;
	firstline = TRUE;
	అయితే (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    ఒకవేళ (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		కొరకు (s = IObuff; *s != NUL; ++s)
		    ఒకవేళ (*s >= 0x80)
		    {
			పూర్ణాంకం l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			ఒకవేళ (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    ఆపు;
			}
			s += l - 1;
		    }
		ఒకవేళ (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		ఒకవేళ (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		లేకపోతే ఒకవేళ (utf8 != this_utf8)
		{
		    semsg(_(e_mix_యొక్క_help_file_encodings_within_language_str), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    ఒకవేళ (లోపల_example)
	    {
		// skip over example; a non-white in the first column ends it
		ఒకవేళ (vim_strchr((char_u *)" \t\n\r", IObuff[0]))
		    కొనసాగు;
		లోపల_example = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    అయితే (p1 != ఖాళీ)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		ఒకవేళ (p2 != ఖాళీ && p2 > p1 + 1)	// skip "*" and "**"
		{
		    కొరకు (s = p1 + 1; s < p2; ++s)
			ఒకవేళ (*s == ' ' || *s == '\t' || *s == '|')
			    ఆపు;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    ఒకవేళ (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != ఖాళీ
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			ఒకవేళ (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    ఆపు;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			ఒకవేళ (s == ఖాళీ)
			{
			    got_int = TRUE;
			    ఆపు;
			}
			((char_u **)ga.ga_దత్తాంశం)[ga.ga_len] = s;
			++ga.ga_len;
			sprintf((అక్షరం *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    size_t off = STRLEN(IObuff);
	    ఒకవేళ (off >= 2 && IObuff[off - 1] == '\n')
	    {
		off -= 2;
		అయితే (off > 0 && (ASCII_ISLOWER(IObuff[off])
						  || VIM_ISDIGIT(IObuff[off])))
		    off--;
		ఒకవేళ (IObuff[off] == '>' && (off == 0 || IObuff[off - 1] == ' '))
		    లోపల_example = TRUE;
	    }
	    పంక్తి_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    ఒకవేళ (!got_int)
    {
	// Sort the tags.
	ఒకవేళ (ga.ga_దత్తాంశం != ఖాళీ)
	    క్రమీకరించు_అక్షరశ్రేణులు((char_u **)ga.ga_దత్తాంశం, ga.ga_len);

	// Check for duplicates.
	కొరకు (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_దత్తాంశం)[i - 1];
	    p2 = ((char_u **)ga.ga_దత్తాంశం)[i];
	    అయితే (*p1 == *p2)
	    {
		ఒకవేళ (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((అక్షరం *)NameBuff, MAXPATHL,
			    _(e_duplicate_ట్యాగ్_str_లోపల_file_str_str),
				     ((char_u **)ga.ga_దత్తాంశం)[i], dir, p2 + 1);
		    emsg((అక్షరం *)NameBuff);
		    *p2 = '\t';
		    ఆపు;
		}
		++p1;
		++p2;
	    }
	}

	ఒకవేళ (utf8 == TRUE)
	    fprintf(fd_ట్యాగులు, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	కొరకు (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_దత్తాంశం)[i];
	    ఒకవేళ (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		fputs((అక్షరం *)s, fd_ట్యాగులు);
	    లేకపోతే
	    {
		fprintf(fd_ట్యాగులు, "%s\t/*", s);
		కొరకు (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    ఒకవేళ (*p1 == '\\' || *p1 == '/')
			putc('\\', fd_ట్యాగులు);
		    putc(*p1, fd_ట్యాగులు);
		}
		fprintf(fd_ట్యాగులు, "*\n");
	    }
	}
    }
    ఒకవేళ (mix)
	got_int = FALSE;    // continue with other languages

    కొరకు (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_దత్తాంశం)[i]);
    ga_clear(&ga);
    fclose(fd_ట్యాగులు);	    // there is no check for an error...
}

/*
 * Generate tags in one help directory, taking care of translations.
 */
    స్థిర శూన్య
do_helptags(char_u *dirname, పూర్ణాంకం కూడు_help_ట్యాగులు, పూర్ణాంకం ignore_writeerr)
{
#నిర్వచితమైతే FEAT_MULTI_LANG
    పూర్ణాంకం		len;
    పూర్ణాంకం		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    పూర్ణాంకం		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, dirname);
    కూడు_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    ఒకవేళ (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_(e_no_సరిపోలిక_str_1), NameBuff);
	తిరిగి;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    కొరకు (i = 0; i < filecount; ++i)
    {
	len = (పూర్ణాంకం)STRLEN(files[i]);
	ఒకవేళ (len <= 4)
	    కొనసాగు;

	ఒకవేళ (STRICMP(files[i] + len - 4, ".txt") == 0)
	{
	    // ".txt" -> language "en"
	    lang[0] = 'e';
	    lang[1] = 'n';
	}
	లేకపోతే ఒకవేళ (files[i][len - 4] == '.'
		&& ASCII_ISALPHA(files[i][len - 3])
		&& ASCII_ISALPHA(files[i][len - 2])
		&& TOLOWER_ASC(files[i][len - 1]) == 'x')
	{
	    // ".abx" -> language "ab"
	    lang[0] = TOLOWER_ASC(files[i][len - 3]);
	    lang[1] = TOLOWER_ASC(files[i][len - 2]);
	}
	లేకపోతే
	    కొనసాగు;

	// Did we find this language already?
	కొరకు (j = 0; j < ga.ga_len; j += 2)
	    ఒకవేళ (STRNCMP(lang, ((char_u *)ga.ga_దత్తాంశం) + j, 2) == 0)
		ఆపు;
	ఒకవేళ (j == ga.ga_len)
	{
	    // New language, add it.
	    ఒకవేళ (ga_grow(&ga, 2) == FAIL)
		ఆపు;
	    ((char_u *)ga.ga_దత్తాంశం)[ga.ga_len++] = lang[0];
	    ((char_u *)ga.ga_దత్తాంశం)[ga.ga_len++] = lang[1];
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    కొరకు (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_దత్తాంశం)[j];
	fname[6] = ((char_u *)ga.ga_దత్తాంశం)[j + 1];
	ఒకవేళ (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	లేకపోతే
	{
	    // Language "ab" uses ".abx" and "tags-ab".
	    STRCPY(ext, ".xxx");
	    ext[1] = fname[5];
	    ext[2] = fname[6];
	}
	helptags_one(dirname, ext, fname, కూడు_help_ట్యాగులు, ignore_writeerr);
    }

    ga_clear(&ga);
    FreeWild(filecount, files);

#లేకపోతే
    // No language support, just use "*.txt" and "tags".
    helptags_one(dirname, (char_u *)".txt", (char_u *)"tags", కూడు_help_ట్యాగులు,
							    ignore_writeerr);
#అయితే_ముగింపు
}

    స్థిర శూన్య
helptags_cb(char_u *fname, శూన్య *cookie)
{
    do_helptags(fname, *(పూర్ణాంకం *)cookie, TRUE);
}

/*
 * ":helptags"
 */
    శూన్య
ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*dirname;
    పూర్ణాంకం		కూడు_help_ట్యాగులు = FALSE;

    // Check for ":helptags ++t {dir}".
    ఒకవేళ (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	కూడు_help_ట్యాగులు = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    ఒకవేళ (STRCMP(eap->arg, "ALL") == 0)
    {
	do_లోపల_మార్గం(p_rtp, "", (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &కూడు_help_ట్యాగులు);
    }
    లేకపోతే
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	dirname = ExpandOne(&xpc, eap->arg, ఖాళీ,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	ఒకవేళ (dirname == ఖాళీ || !mch_isdir(dirname))
	    semsg(_(e_not_a_directory_str), eap->arg);
	లేకపోతే
	    do_helptags(dirname, కూడు_help_ట్యాగులు, FALSE);
	vim_free(dirname);
    }
}
