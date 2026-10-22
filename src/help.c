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

#അന്തര്ഭാവ "vim.h"

/*
 * ":help": open a read-only window on a help file
 */
    ശൂന്യം
ex_help(exarg_T *eap)
{
    char_u	*arg;
    char_u	*tag;
    FILE	*helpfd;	// file descriptor of help file
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		i;
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		അക്കം_matches;
    char_u	**matches;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		empty_fnum = 0;
    പൂർണ്ണസംഖ്യ		alt_fnum = 0;
    buf_T	*buf;
#ifdef FEAT_MULTI_LANG
    പൂർണ്ണസംഖ്യ		len;
    char_u	*lang;
#endif
#ifdef FEAT_FOLDING
    പൂർണ്ണസംഖ്യ		old_താക്കോൽ_Typed = താക്കോൽ_Typed;
#endif

    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;

    എങ്കിൽ (eap != ശൂന്യ)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	വേണ്ടി (arg = eap->arg; *arg; ++arg)
	{
	    എങ്കിൽ (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		നിർത്തുക;
	    }
	}
	arg = eap->arg;

	എങ്കിൽ (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_(e_dont_panic));
	    തിരികെ;
	}

	എങ്കിൽ (eap->skip)	    // not executing commands
	    തിരികെ;
    }
    അല്ലെങ്കിൽ
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    ആകുമ്പോൾ (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#endif

    // When no argument given go to the index.
    എങ്കിൽ (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = find_help_tags(arg, &അക്കം_matches, &matches,
						 eap != ശൂന്യ && eap->forceit);

    i = 0;
#ifdef FEAT_MULTI_LANG
    എങ്കിൽ (n != FAIL && lang != ശൂന്യ)
	// Find first item with the requested language.
	വേണ്ടി (i = 0; i < അക്കം_matches; ++i)
	{
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(matches[i]);
	    എങ്കിൽ (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		നിർത്തുക;
	}
#endif
    എങ്കിൽ (i >= അക്കം_matches || n == FAIL)
    {
#ifdef FEAT_MULTI_LANG
	എങ്കിൽ (lang != ശൂന്യ)
	    semsg(_(e_sorry_no_str_help_for_str), lang, arg);
	അല്ലെങ്കിൽ
#endif
	    semsg(_(e_sorry_no_help_for_str), arg);
	എങ്കിൽ (n != FAIL)
	    FreeWild(അക്കം_matches, matches);
	തിരികെ;
    }

    // The first match (in the requested language) is the best match.
    tag = vim_strsave(matches[i]);
    FreeWild(അക്കം_matches, matches);

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    എങ്കിൽ (!bt_help(curwin->w_buffer) || cmdmod.cmod_tab != 0)
    {
	എങ്കിൽ (cmdmod.cmod_tab != 0)
	    wp = ശൂന്യ;
	അല്ലെങ്കിൽ
	    FOR_ALL_WINDOWS(wp)
		എങ്കിൽ (bt_help(wp->w_buffer))
		    നിർത്തുക;
	എങ്കിൽ (wp != ശൂന്യ && wp->w_buffer->b_nwindows > 0)
	    win_enter(wp, TRUE);
	അല്ലെങ്കിൽ
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    എങ്കിൽ ((helpfd = mch_fopen((അക്ഷരം *)p_hf, READBIN)) == ശൂന്യ)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		പോകുക erret;
	    }
	    fclose(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    എങ്കിൽ (cmdmod.cmod_split == 0 && curwin->w_width != Columns
						  && curwin->w_width < 80)
		n |= p_sb ? WSP_BOT : WSP_മുകൾ;
	    എങ്കിൽ (win_split(0, n) == FAIL)
		പോകുക erret;

	    എങ്കിൽ (curwin->w_height < p_hh)
		win_setheight((പൂർണ്ണസംഖ്യ)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (ശൂന്യം)do_ecmd(0, ശൂന്യ, ശൂന്യ, ശൂന്യ, ECMD_LASTL,
			  ECMD_HIDE + ECMD_SET_HELP,
			  ശൂന്യ);  // buffer is still open, don't store info
	    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    empty_fnum = curbuf->b_fnum;
	}
    }

    എങ്കിൽ (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#ifdef FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    താക്കോൽ_Typed = old_താക്കോൽ_Typed;
#endif

    എങ്കിൽ (tag != ശൂന്യ)
	do_tag(tag, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    എങ്കിൽ (empty_fnum != 0 && curbuf->b_fnum != empty_fnum)
    {
	buf = buflist_findnr(empty_fnum);
	എങ്കിൽ (buf != ശൂന്യ && buf->b_nwindows == 0)
	    wipe_buffer(buf, TRUE);
    }

    // keep the previous alternate file
    എങ്കിൽ (alt_fnum != 0 && curwin->w_alt_fnum == empty_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(tag);
}

/*
 * ":helpclose": Close one help window
 */
    ശൂന്യം
ex_helpclose(exarg_T *eap UNUSED)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	എങ്കിൽ (bt_help(win->w_buffer))
	{
	    win_close(win, FALSE);
	    തിരികെ;
	}
    }
}

#എങ്കിൽ defined(FEAT_MULTI_LANG)
/*
 * In an argument search for a language specifiers in the form "@xx".
 * Changes the "@" to NUL if found, and returns a pointer to "xx".
 * Returns NULL if not found.
 */
    char_u *
check_help_lang(char_u *arg)
{
    പൂർണ്ണസംഖ്യ len = (പൂർണ്ണസംഖ്യ)STRLEN(arg);

    എങ്കിൽ (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	തിരികെ arg + len - 2;
    }
    തിരികെ ശൂന്യ;
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
    പൂർണ്ണസംഖ്യ
help_heuristic(
    char_u	*matched_string,
    പൂർണ്ണസംഖ്യ		offset,			// offset for match
    പൂർണ്ണസംഖ്യ		wrong_case)		// no matching case
{
    പൂർണ്ണസംഖ്യ		അക്കം_letters;
    char_u	*p;

    അക്കം_letters = 0;
    വേണ്ടി (p = matched_string; *p; p++)
	എങ്കിൽ (ASCII_ISALNUM(*p))
	    അക്കം_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    എങ്കിൽ (ASCII_ISALNUM(matched_string[offset]) && offset > 0
				 && ASCII_ISALNUM(matched_string[offset - 1]))
	offset += 10000;
    അല്ലെങ്കിൽ എങ്കിൽ (offset > 2)
	offset *= 200;
    എങ്കിൽ (wrong_case)
	offset += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    എങ്കിൽ (matched_string[0] == '+' && matched_string[1] != NUL)
	offset += 100;
    തിരികെ (പൂർണ്ണസംഖ്യ)(100 * അക്കം_letters + STRLEN(matched_string) + offset);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
help_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    അക്ഷരം    *p1;
    അക്ഷരം    *p2;
    പൂർണ്ണസംഖ്യ	    cmp;

    p1 = *(അക്ഷരം **)s1 + strlen(*(അക്ഷരം **)s1) + 1;
    p2 = *(അക്ഷരം **)s2 + strlen(*(അക്ഷരം **)s2) + 1;

    // Compare by help heuristic number first.
    cmp = strcmp(p1, p2);
    എങ്കിൽ (cmp != 0)
	തിരികെ cmp;

    // Compare by strings as tie-breaker when same heuristic number.
    തിരികെ strcmp(*(അക്ഷരം **)s1, *(അക്ഷരം **)s2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * The matches will be sorted with a "best" match algorithm.
 * When "keep_lang" is TRUE try keeping the language of the current buffer.
 */
    പൂർണ്ണസംഖ്യ
find_help_tags(
    char_u	*arg,
    പൂർണ്ണസംഖ്യ		*അക്കം_matches,
    char_u	***matches,
    പൂർണ്ണസംഖ്യ		keep_lang)
{
    char_u	*s, *d;
    പൂർണ്ണസംഖ്യ		i;
    // Specific tags that either have a specific replacement or won't go
    // through the generic rules.
    സ്ഥിതം അക്ഷരം *(except_tbl[][2]) = {
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
	{ശൂന്യ, ശൂന്യ}
    };
    സ്ഥിതം അക്ഷരം *(വ്യഞ്ജകം_table[]) = {"!=?", "!~?", "<=?", "<?", "==?", "=~?",
				   ">=?", ">?", "is?", "isnot?"};
    പൂർണ്ണസംഖ്യ flags;

    d = IObuff;		    // assume IObuff is long enough!
    d[0] = NUL;

    എങ്കിൽ (STRNICMP(arg, "expr-", 5) == 0)
    {
	// When the string starting with "expr-" and containing '?' and matches
	// the table, it is taken literally (but ~ is escaped).  Otherwise '?'
	// is recognized as a wildcard.
	വേണ്ടി (i = (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(വ്യഞ്ജകം_table); --i >= 0; )
	    എങ്കിൽ (STRCMP(arg + 5, വ്യഞ്ജകം_table[i]) == 0)
	    {
		പൂർണ്ണസംഖ്യ si = 0, di = 0;

		വേണ്ടി (;;)
		{
		    എങ്കിൽ (arg[si] == '~')
			d[di++] = '\\';
		    d[di++] = arg[si];
		    എങ്കിൽ (arg[si] == NUL)
			നിർത്തുക;
		    ++si;
		}
		നിർത്തുക;
	    }
    }
    അല്ലെങ്കിൽ
    {
	// Recognize a few exceptions to the rule.  Some strings that contain
	// '*'are changed to "star", otherwise '*' is recognized as a wildcard.
	വേണ്ടി (i = 0; except_tbl[i][0] != ശൂന്യ; ++i)
	    എങ്കിൽ (STRCMP(arg, except_tbl[i][0]) == 0)
	    {
		STRCPY(d, except_tbl[i][1]);
		നിർത്തുക;
	    }
    }

    എങ്കിൽ (d[0] == NUL)	// no match in table
    {
	// Replace "\S" with "/\\S", etc.  Otherwise every tag is matched.
	// Also replace "\%^" and "\%(", they match every tag too.
	// Also "\zs", "\z1", etc.
	// Also "\@<", "\@=", "\@<=", etc.
	// And also "\_$" and "\_^".
	എങ്കിൽ (arg[0] == '\\'
		&& ((arg[1] != NUL && arg[2] == NUL)
		    || (vim_strchr((char_u *)"%_z@", arg[1]) != ശൂന്യ
							   && arg[2] != NUL)))
	{
	    vim_snprintf((അക്ഷരം *)d, IOSIZE, "/\\\\%s", arg + 1);
	    // Check for "/\\_$", should be "/\\_\$"
	    എങ്കിൽ (d[3] == '_' && d[4] == '$')
		STRCPY(d + 4, "\\$");
	}
	അല്ലെങ്കിൽ
	{
	  // Replace:
	  // "[:...:]" with "\[:...:]"
	  // "[++...]" with "\[++...]"
	  // "\{" with "\\{"		   -- matching "} \}"
	    എങ്കിൽ ((arg[0] == '[' && (arg[1] == ':'
			 || (arg[1] == '+' && arg[2] == '+')))
		    || (arg[0] == '\\' && arg[1] == '{'))
	      *d++ = '\\';

	  // If tag starts with "('", skip the "(". Fixes CTRL-] on ('option'.
	  എങ്കിൽ (*arg == '(' && arg[1] == '\'')
	      arg++;
	  വേണ്ടി (s = arg; *s; ++s)
	  {
	    // Replace "|" with "bar" and '"' with "quote" to match the name of
	    // the tags for these commands.
	    // Replace "*" with ".*" and "?" with "." to match command line
	    // completion.
	    // Insert a backslash before '~', '$' and '.' to avoid their
	    // special meaning.
	    എങ്കിൽ (d - IObuff > IOSIZE - 10)	// getting too long!?
		നിർത്തുക;
	    തിരഞ്ഞെടുപ്പ് (*s)
	    {
		അവസ്ഥ '|':   STRCPY(d, "bar");
			    d += 3;
			    തുടരുക;
		അവസ്ഥ '"':   STRCPY(d, "quote");
			    d += 5;
			    തുടരുക;
		അവസ്ഥ '*':   *d++ = '.';
			    നിർത്തുക;
		അവസ്ഥ '?':   *d++ = '.';
			    തുടരുക;
		അവസ്ഥ '$':
		അവസ്ഥ '.':
		അവസ്ഥ '~':   *d++ = '\\';
			    നിർത്തുക;
	    }

	    // Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	    // ":help i_^_CTRL-D" work.
	    // Insert '-' before and after "CTRL-X" when applicable.
	    എങ്കിൽ (*s < ' ' || (*s == '^' && s[1] && (ASCII_ISALPHA(s[1])
			   || vim_strchr((char_u *)"?@[\\]^", s[1]) != ശൂന്യ)))
	    {
		എങ്കിൽ (d > IObuff && d[-1] != '_' && d[-1] != '\\')
		    *d++ = '_';		// prepend a '_' to make x_CTRL-x
		STRCPY(d, "CTRL-");
		d += 5;
		എങ്കിൽ (*s < ' ')
		{
		    *d++ = *s + '@';
		    എങ്കിൽ (d[-1] == '\\')
			*d++ = '\\';	// double a backslash
		}
		അല്ലെങ്കിൽ
		    *d++ = *++s;
		എങ്കിൽ (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		// append a '_'
		തുടരുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*s == '^')		// "^" or "CTRL-^" or "^_"
		*d++ = '\\';

	    // Insert a backslash before a backslash after a slash, for search
	    // pattern tags: "/\|" --> "/\\|".
	    അല്ലെങ്കിൽ എങ്കിൽ (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    // "CTRL-\_" -> "CTRL-\\_" to avoid the special meaning of "\_" in
	    // "CTRL-\_CTRL-N"
	    എങ്കിൽ (STRNICMP(s, "CTRL-\\_", 7) == 0)
	    {
		STRCPY(d, "CTRL-\\\\");
		d += 7;
		s += 6;
	    }

	    *d++ = *s;

	    // If tag contains "({" or "([", tag terminates at the "(".
	    // This is for help on functions, e.g.: abs({expr}).
	    എങ്കിൽ (*s == '(' && (s[1] == '{' || s[1] =='['))
		നിർത്തുക;

	    // If tag starts with ', toss everything after a second '. Fixes
	    // CTRL-] on 'option'. (would include the trailing '.').
	    എങ്കിൽ (*s == '\'' && s > arg && *arg == '\'')
		നിർത്തുക;
	    // Also '{' and '}'.
	    എങ്കിൽ (*s == '}' && s > arg && *arg == '{')
		നിർത്തുക;
	  }
	  *d = NUL;

	  എങ്കിൽ (*IObuff == '`')
	  {
	      എങ്കിൽ (d > IObuff + 2 && d[-1] == '`')
	      {
		  // remove the backticks from `command`
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-2] = NUL;
	      }
	      അല്ലെങ്കിൽ എങ്കിൽ (d > IObuff + 3 && d[-2] == '`' && d[-1] == ',')
	      {
		  // remove the backticks and comma from `command`,
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-3] = NUL;
	      }
	      അല്ലെങ്കിൽ എങ്കിൽ (d > IObuff + 4 && d[-3] == '`'
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
    *അക്കം_matches = 0;
    flags = TAG_HELP | TAG_REGEXP | TAG_NAMES | TAG_VERBOSE | TAG_NO_TAGFUNC;
    എങ്കിൽ (keep_lang)
	flags |= TAG_KEEP_LANG;
    എങ്കിൽ (find_tags(IObuff, അക്കം_matches, matches, flags, (പൂർണ്ണസംഖ്യ)MAXCOL, ശൂന്യ) == OK
	    && *അക്കം_matches > 0)
    {
	// Sort the matches found on the heuristic number that is after the
	// tag name.
	qsort((ശൂന്യം *)*matches, (size_t)*അക്കം_matches,
					      വലുപ്പം(char_u *), help_compare);
	// Delete more than TAG_MANY to reduce the size of the listing.
	ആകുമ്പോൾ (*അക്കം_matches > TAG_MANY)
	    vim_free((*matches)[--*അക്കം_matches]);
    }
    തിരികെ OK;
}

#ifdef FEAT_MULTI_LANG
/*
 * Cleanup matches for help tags:
 * Remove "@ab" if the top of 'helplang' is "ab" and the language of the first
 * tag matches it.  Otherwise remove "@en" if "en" is the only language.
 */
    ശൂന്യം
cleanup_help_tags(പൂർണ്ണസംഖ്യ അക്കം_file, char_u **file)
{
    പൂർണ്ണസംഖ്യ		i, j;
    പൂർണ്ണസംഖ്യ		len;
    char_u	buf[4];
    char_u	*p = buf;

    എങ്കിൽ (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    വേണ്ടി (i = 0; i < അക്കം_file; ++i)
    {
	len = (പൂർണ്ണസംഖ്യ)STRLEN(file[i]) - 3;
	എങ്കിൽ (len <= 0)
	    തുടരുക;
	എങ്കിൽ (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    വേണ്ടി (j = 0; j < അക്കം_file; ++j)
		എങ്കിൽ (j != i && (പൂർണ്ണസംഖ്യ)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    നിർത്തുക;
	    എങ്കിൽ (j == അക്കം_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    എങ്കിൽ (*buf != NUL)
	വേണ്ടി (i = 0; i < അക്കം_file; ++i)
	{
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(file[i]) - 3;
	    എങ്കിൽ (len <= 0)
		തുടരുക;
	    എങ്കിൽ (STRCMP(file[i] + len, buf) == 0)
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
    ശൂന്യം
prepare_help_buffer(ശൂന്യം)
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
    എങ്കിൽ (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	set_string_option_direct((char_u *)"isk", -1, p, OPT_FREE|OPT_LOCAL, 0);
	check_buf_options(curbuf);
	(ശൂന്യം)buf_init_chartab(curbuf, FALSE);
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
    ശൂന്യം
fix_help_buffer(ശൂന്യം)
{
    linenr_T	lnum;
    char_u	*വരി;
    പൂർണ്ണസംഖ്യ		in_example = FALSE;
    പൂർണ്ണസംഖ്യ		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    പൂർണ്ണസംഖ്യ		mustfree;

    // Set filetype to "help" if still needed.
    എങ്കിൽ (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_lock;
	set_option_മൂല്യം_give_err((char_u *)"ft",
					      0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_lock;
    }

#ifdef FEAT_SYN_HL
    എങ്കിൽ (!syntax_present(curwin))
#endif
    {
	വേണ്ടി (lnum = 1; lnum <= curbuf->b_ml.ml_വരി_എണ്ണം; ++lnum)
	{
	    വരി = ml_get_buf(curbuf, lnum, FALSE);
	    len = ml_get_buf_len(curbuf, lnum);
	    എങ്കിൽ (in_example && len > 0 && !VIM_ISWHITE(വരി[0]))
	    {
		// End of example: non-white or '<' in first column.
		എങ്കിൽ (വരി[0] == '<')
		{
		    // blank-out a '<' in the first column
		    വരി = ml_get_buf(curbuf, lnum, TRUE);
		    വരി[0] = ' ';
		}
		in_example = FALSE;
	    }
	    എങ്കിൽ (!in_example && len > 0)
	    {
		എങ്കിൽ (വരി[len - 1] == '>' && (len == 1 || വരി[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    വരി = ml_get_buf(curbuf, lnum, TRUE);
		    വരി[len - 1] = ' ';
		    in_example = TRUE;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (വരി[len - 1] == '~')
		{
		    // blank-out a '~' at the end of line (header marker)
		    വരി = ml_get_buf(curbuf, lnum, TRUE);
		    വരി[len - 1] = ' ';
		}
	    }
	}
    }

    // In the "help.txt" and "help.abx" file, add the locally added help
    // files.  This uses the very first line in the help file.
    fname = gettail(curbuf->b_fname);
    എങ്കിൽ (fnamecmp(fname, "help.txt") == 0
#ifdef FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#endif
	)
    {
	വേണ്ടി (lnum = 1; lnum < curbuf->b_ml.ml_വരി_എണ്ണം; ++lnum)
	{
	    വരി = ml_get_buf(curbuf, lnum, FALSE);
	    എങ്കിൽ (strstr((അക്ഷരം *)വരി, "*local-additions*") == ശൂന്യ)
		തുടരുക;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    ആകുമ്പോൾ (*p != NUL)
	    {
		പൂർണ്ണസംഖ്യ NameBufflen;

		NameBufflen = copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		എങ്കിൽ (rt != ശൂന്യ &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    പൂർണ്ണസംഖ്യ		fcount;
		    char_u	**fnames;
		    FILE	*fd;
		    char_u	*s;
		    പൂർണ്ണസംഖ്യ		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    എങ്കിൽ (*NameBuff != NUL && !after_pathsep(NameBuff, NameBuff + NameBufflen))
		    {
			STRCPY(NameBuff + NameBufflen, PATHSEPSTR);
			NameBufflen += വലുപ്പം(PATHSEP);
		    }
#ifdef FEAT_MULTI_LANG
		    STRCPY(NameBuff + NameBufflen, "doc/*.??[tx]");
#അല്ലെങ്കിൽ
		    STRCPY(NameBuff + NameBufflen, "doc/*.txt");
#endif
		    എങ്കിൽ (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#ifdef FEAT_MULTI_LANG
			പൂർണ്ണസംഖ്യ	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			വേണ്ടി (i1 = 0; i1 < fcount; ++i1)
			{
			    f1 = fnames[i1];
			    t1 = gettail(f1);
			    e1 = vim_strrchr(t1, '.');
			    എങ്കിൽ (e1 == ശൂന്യ)
				തുടരുക;
			    എങ്കിൽ (fnamecmp(e1, ".txt") != 0
					       && fnamecmp(e1, fname + 4) != 0)
			    {
				// Not .txt and not .abx, remove it.
				VIM_CLEAR(fnames[i1]);
				തുടരുക;
			    }

			    വേണ്ടി (i2 = i1 + 1; i2 < fcount; ++i2)
			    {
				f2 = fnames[i2];
				എങ്കിൽ (f2 == ശൂന്യ)
				    തുടരുക;
				t2 = gettail(f2);
				e2 = vim_strrchr(t2, '.');
				എങ്കിൽ (e2 == ശൂന്യ)
				    തുടരുക;
				എങ്കിൽ (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    തുടരുക;
				എങ്കിൽ (fnamecmp(e1, ".txt") == 0
					       && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#endif
			വേണ്ടി (fi = 0; fi < fcount; ++fi)
			{
			    എങ്കിൽ (fnames[fi] == ശൂന്യ)
				തുടരുക;
			    fd = mch_fopen((അക്ഷരം *)fnames[fi], "r");
			    എങ്കിൽ (fd != ശൂന്യ)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				എങ്കിൽ (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != ശൂന്യ)
				{
				    പൂർണ്ണസംഖ്യ	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    ആകുമ്പോൾ (*s != NUL)
				    {
					എങ്കിൽ (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					എങ്കിൽ (*s >= 0x80 && this_utf != FALSE)
					{
					    പൂർണ്ണസംഖ്യ	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    എങ്കിൽ (l == 1)
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
				    എങ്കിൽ (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    അല്ലെങ്കിൽ
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    ശൂന്യ);
					എങ്കിൽ (cp == ശൂന്യ)
					    cp = IObuff;
				    }
				    convert_setup(&vc, ശൂന്യ, ശൂന്യ);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    എങ്കിൽ (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				fclose(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		എങ്കിൽ (mustfree)
		    vim_free(rt);
	    }
	    നിർത്തുക;
	}
    }
}

/*
 * ":exusage"
 */
    ശൂന്യം
ex_exusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help ex-cmd-index");
}

/*
 * ":viusage"
 */
    ശൂന്യം
ex_viusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help normal-index");
}

/*
 * Generate tags in one help directory.
 */
    സ്ഥിതം ശൂന്യം
helptags_one(
    char_u	*dir,		// doc directory
    char_u	*ext,		// suffix, ".txt", ".itx", ".frx", etc.
    char_u	*tagfname,	// "tags" for English, "tags-fr" for French.
    പൂർണ്ണസംഖ്യ		add_help_tags,	// add "help-tags" tag
    പൂർണ്ണസംഖ്യ		ignore_writeerr)    // ignore write error
{
    FILE	*fd_tags;
    FILE	*fd;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		res;
    പൂർണ്ണസംഖ്യ		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    പൂർണ്ണസംഖ്യ		fi;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*fname;
    പൂർണ്ണസംഖ്യ		dirlen;
    പൂർണ്ണസംഖ്യ		utf8 = MAYBE;
    പൂർണ്ണസംഖ്യ		this_utf8;
    പൂർണ്ണസംഖ്യ		firstline;
    പൂർണ്ണസംഖ്യ		in_example;
    പൂർണ്ണസംഖ്യ		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (പൂർണ്ണസംഖ്യ)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    res = gen_expand_wildcards(1, &NameBuff, &filecount, &files,
							    EW_FILE|EW_SILENT);
    എങ്കിൽ (res == FAIL || filecount == 0)
    {
	എങ്കിൽ (!got_int)
	    semsg(_(e_no_match_str_1), NameBuff);
	എങ്കിൽ (res != FAIL)
	    FreeWild(filecount, files);
	തിരികെ;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_tags = mch_fopen((അക്ഷരം *)NameBuff, "w");
    എങ്കിൽ (fd_tags == ശൂന്യ)
    {
	എങ്കിൽ (!ignore_writeerr)
	    semsg(_(e_cannot_open_str_for_writing_1), NameBuff);
	FreeWild(filecount, files);
	തിരികെ;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, വലുപ്പം(char_u *), 100);
    എങ്കിൽ (add_help_tags || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	അല്ലെങ്കിൽ
	{
	    s = alloc(18 + (unsigned)STRLEN(tagfname));
	    എങ്കിൽ (s == ശൂന്യ)
		got_int = TRUE;
	    അല്ലെങ്കിൽ
	    {
		sprintf((അക്ഷരം *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_data)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    വേണ്ടി (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((അക്ഷരം *)files[fi], "r");
	എങ്കിൽ (fd == ശൂന്യ)
	{
	    semsg(_(e_unable_to_open_str_for_reading), files[fi]);
	    തുടരുക;
	}
	fname = files[fi] + dirlen + 1;

	in_example = FALSE;
	firstline = TRUE;
	ആകുമ്പോൾ (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    എങ്കിൽ (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		വേണ്ടി (s = IObuff; *s != NUL; ++s)
		    എങ്കിൽ (*s >= 0x80)
		    {
			പൂർണ്ണസംഖ്യ l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			എങ്കിൽ (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    നിർത്തുക;
			}
			s += l - 1;
		    }
		എങ്കിൽ (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		എങ്കിൽ (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		അല്ലെങ്കിൽ എങ്കിൽ (utf8 != this_utf8)
		{
		    semsg(_(e_mix_of_help_file_encodings_within_language_str), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    എങ്കിൽ (in_example)
	    {
		// skip over example; a non-white in the first column ends it
		എങ്കിൽ (vim_strchr((char_u *)" \t\n\r", IObuff[0]))
		    തുടരുക;
		in_example = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    ആകുമ്പോൾ (p1 != ശൂന്യ)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		എങ്കിൽ (p2 != ശൂന്യ && p2 > p1 + 1)	// skip "*" and "**"
		{
		    വേണ്ടി (s = p1 + 1; s < p2; ++s)
			എങ്കിൽ (*s == ' ' || *s == '\t' || *s == '|')
			    നിർത്തുക;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    എങ്കിൽ (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != ശൂന്യ
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    നിർത്തുക;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			എങ്കിൽ (s == ശൂന്യ)
			{
			    got_int = TRUE;
			    നിർത്തുക;
			}
			((char_u **)ga.ga_data)[ga.ga_len] = s;
			++ga.ga_len;
			sprintf((അക്ഷരം *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    size_t off = STRLEN(IObuff);
	    എങ്കിൽ (off >= 2 && IObuff[off - 1] == '\n')
	    {
		off -= 2;
		ആകുമ്പോൾ (off > 0 && (ASCII_ISLOWER(IObuff[off])
						  || VIM_ISDIGIT(IObuff[off])))
		    off--;
		എങ്കിൽ (IObuff[off] == '>' && (off == 0 || IObuff[off - 1] == ' '))
		    in_example = TRUE;
	    }
	    വരി_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    എങ്കിൽ (!got_int)
    {
	// Sort the tags.
	എങ്കിൽ (ga.ga_data != ശൂന്യ)
	    sort_strings((char_u **)ga.ga_data, ga.ga_len);

	// Check for duplicates.
	വേണ്ടി (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_data)[i - 1];
	    p2 = ((char_u **)ga.ga_data)[i];
	    ആകുമ്പോൾ (*p1 == *p2)
	    {
		എങ്കിൽ (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((അക്ഷരം *)NameBuff, MAXPATHL,
			    _(e_duplicate_tag_str_in_file_str_str),
				     ((char_u **)ga.ga_data)[i], dir, p2 + 1);
		    emsg((അക്ഷരം *)NameBuff);
		    *p2 = '\t';
		    നിർത്തുക;
		}
		++p1;
		++p2;
	    }
	}

	എങ്കിൽ (utf8 == TRUE)
	    fprintf(fd_tags, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	വേണ്ടി (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_data)[i];
	    എങ്കിൽ (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		fputs((അക്ഷരം *)s, fd_tags);
	    അല്ലെങ്കിൽ
	    {
		fprintf(fd_tags, "%s\t/*", s);
		വേണ്ടി (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    എങ്കിൽ (*p1 == '\\' || *p1 == '/')
			putc('\\', fd_tags);
		    putc(*p1, fd_tags);
		}
		fprintf(fd_tags, "*\n");
	    }
	}
    }
    എങ്കിൽ (mix)
	got_int = FALSE;    // continue with other languages

    വേണ്ടി (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_data)[i]);
    ga_clear(&ga);
    fclose(fd_tags);	    // there is no check for an error...
}

/*
 * Generate tags in one help directory, taking care of translations.
 */
    സ്ഥിതം ശൂന്യം
do_helptags(char_u *dirname, പൂർണ്ണസംഖ്യ add_help_tags, പൂർണ്ണസംഖ്യ ignore_writeerr)
{
#ifdef FEAT_MULTI_LANG
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    പൂർണ്ണസംഖ്യ		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, dirname);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    എങ്കിൽ (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_(e_no_match_str_1), NameBuff);
	തിരികെ;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    വേണ്ടി (i = 0; i < filecount; ++i)
    {
	len = (പൂർണ്ണസംഖ്യ)STRLEN(files[i]);
	എങ്കിൽ (len <= 4)
	    തുടരുക;

	എങ്കിൽ (STRICMP(files[i] + len - 4, ".txt") == 0)
	{
	    // ".txt" -> language "en"
	    lang[0] = 'e';
	    lang[1] = 'n';
	}
	അല്ലെങ്കിൽ എങ്കിൽ (files[i][len - 4] == '.'
		&& ASCII_ISALPHA(files[i][len - 3])
		&& ASCII_ISALPHA(files[i][len - 2])
		&& TOLOWER_ASC(files[i][len - 1]) == 'x')
	{
	    // ".abx" -> language "ab"
	    lang[0] = TOLOWER_ASC(files[i][len - 3]);
	    lang[1] = TOLOWER_ASC(files[i][len - 2]);
	}
	അല്ലെങ്കിൽ
	    തുടരുക;

	// Did we find this language already?
	വേണ്ടി (j = 0; j < ga.ga_len; j += 2)
	    എങ്കിൽ (STRNCMP(lang, ((char_u *)ga.ga_data) + j, 2) == 0)
		നിർത്തുക;
	എങ്കിൽ (j == ga.ga_len)
	{
	    // New language, add it.
	    എങ്കിൽ (ga_grow(&ga, 2) == FAIL)
		നിർത്തുക;
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[0];
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[1];
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    വേണ്ടി (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_data)[j];
	fname[6] = ((char_u *)ga.ga_data)[j + 1];
	എങ്കിൽ (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	അല്ലെങ്കിൽ
	{
	    // Language "ab" uses ".abx" and "tags-ab".
	    STRCPY(ext, ".xxx");
	    ext[1] = fname[5];
	    ext[2] = fname[6];
	}
	helptags_one(dirname, ext, fname, add_help_tags, ignore_writeerr);
    }

    ga_clear(&ga);
    FreeWild(filecount, files);

#അല്ലെങ്കിൽ
    // No language support, just use "*.txt" and "tags".
    helptags_one(dirname, (char_u *)".txt", (char_u *)"tags", add_help_tags,
							    ignore_writeerr);
#endif
}

    സ്ഥിതം ശൂന്യം
helptags_cb(char_u *fname, ശൂന്യം *cookie)
{
    do_helptags(fname, *(പൂർണ്ണസംഖ്യ *)cookie, TRUE);
}

/*
 * ":helptags"
 */
    ശൂന്യം
ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*dirname;
    പൂർണ്ണസംഖ്യ		add_help_tags = FALSE;

    // Check for ":helptags ++t {dir}".
    എങ്കിൽ (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	add_help_tags = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    എങ്കിൽ (STRCMP(eap->arg, "ALL") == 0)
    {
	do_in_path(p_rtp, "", (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &add_help_tags);
    }
    അല്ലെങ്കിൽ
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	dirname = ExpandOne(&xpc, eap->arg, ശൂന്യ,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	എങ്കിൽ (dirname == ശൂന്യ || !mch_isdir(dirname))
	    semsg(_(e_not_a_directory_str), eap->arg);
	അല്ലെങ്കിൽ
	    do_helptags(dirname, add_help_tags, FALSE);
	vim_free(dirname);
    }
}
