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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

/*
 * ":help": open a read-only window on a help file
 */
    ਖਾਲੀ
ex_help(exarg_T *eap)
{
    char_u	*arg;
    char_u	*ਟੈਗ;
    FILE	*helpfd;	// file descriptor of help file
    ਪੂਰਨ		n;
    ਪੂਰਨ		i;
    win_T	*wp;
    ਪੂਰਨ		ਅੰਕ_matches;
    char_u	**matches;
    char_u	*p;
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_fnum = 0;
    ਪੂਰਨ		alt_fnum = 0;
    buf_T	*buf;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    ਪੂਰਨ		len;
    char_u	*lang;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਪੂਰਨ		old_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;
#ਜੇ_ਅੰਤ

    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;

    ਜੇ (eap != ਨਿਰਾ)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	ਲਈ (arg = eap->arg; *arg; ++arg)
	{
	    ਜੇ (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		ਰੋਕੋ;
	    }
	}
	arg = eap->arg;

	ਜੇ (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_(e_dont_panic));
	    ਵਾਪਸ;
	}

	ਜੇ (eap->skip)	    // not executing commands
	    ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    ਜਦੋਂ (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#ਜੇ_ਅੰਤ

    // When no argument given go to the index.
    ਜੇ (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = ਲੱਭੋ_help_ਟੈਗ_ਸੂਚੀ(arg, &ਅੰਕ_matches, &matches,
						 eap != ਨਿਰਾ && eap->forceit);

    i = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    ਜੇ (n != FAIL && lang != ਨਿਰਾ)
	// Find first item with the requested language.
	ਲਈ (i = 0; i < ਅੰਕ_matches; ++i)
	{
	    len = (ਪੂਰਨ)STRLEN(matches[i]);
	    ਜੇ (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		ਰੋਕੋ;
	}
#ਜੇ_ਅੰਤ
    ਜੇ (i >= ਅੰਕ_matches || n == FAIL)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	ਜੇ (lang != ਨਿਰਾ)
	    semsg(_(e_sorry_no_str_help_for_str), lang, arg);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    semsg(_(e_sorry_no_help_for_str), arg);
	ਜੇ (n != FAIL)
	    FreeWild(ਅੰਕ_matches, matches);
	ਵਾਪਸ;
    }

    // The first match (in the requested language) is the best match.
    ਟੈਗ = vim_strsave(matches[i]);
    FreeWild(ਅੰਕ_matches, matches);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    ਜੇ (!bt_help(curwin->w_ਬਫਰ) || cmdmod.cmod_tab != 0)
    {
	ਜੇ (cmdmod.cmod_tab != 0)
	    wp = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    FOR_ALL_WINDOWS(wp)
		ਜੇ (bt_help(wp->w_ਬਫਰ))
		    ਰੋਕੋ;
	ਜੇ (wp != ਨਿਰਾ && wp->w_ਬਫਰ->b_nwindows > 0)
	    win_enter(wp, TRUE);
	ਨਹੀਂਤਾਂ
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    ਜੇ ((helpfd = mch_fopen((ਅੱਖਰ *)p_hf, READBIN)) == ਨਿਰਾ)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		ਜਾਓ erret;
	    }
	    fclose(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    ਜੇ (cmdmod.cmod_ਵੰਡੋ == 0 && curwin->w_ਚੌੜਾਈ != Columns
						  && curwin->w_ਚੌੜਾਈ < 80)
		n |= p_sb ? WSP_BOT : WSP_ਸਿਖਰ;
	    ਜੇ (win_ਵੰਡੋ(0, n) == FAIL)
		ਜਾਓ erret;

	    ਜੇ (curwin->w_ਉਚਾਈ < p_hh)
		win_setheight((ਪੂਰਨ)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (ਖਾਲੀ)do_ecmd(0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ECMD_LASTL,
			  ECMD_HIDE + ECMD_ਸੈੱਟ_HELP,
			  ਨਿਰਾ);  // buffer is still open, don't store info
	    ਜੇ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    ਖਾਲੀ_ਥਾਵਾਂ_fnum = curbuf->b_fnum;
	}
    }

    ਜੇ (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    ਕੁੰਜੀ_Typed = old_ਕੁੰਜੀ_Typed;
#ਜੇ_ਅੰਤ

    ਜੇ (ਟੈਗ != ਨਿਰਾ)
	do_ਟੈਗ(ਟੈਗ, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_fnum != 0 && curbuf->b_fnum != ਖਾਲੀ_ਥਾਵਾਂ_fnum)
    {
	buf = buflist_findnr(ਖਾਲੀ_ਥਾਵਾਂ_fnum);
	ਜੇ (buf != ਨਿਰਾ && buf->b_nwindows == 0)
	    wipe_ਬਫਰ(buf, TRUE);
    }

    // keep the previous alternate file
    ਜੇ (alt_fnum != 0 && curwin->w_alt_fnum == ਖਾਲੀ_ਥਾਵਾਂ_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(ਟੈਗ);
}

/*
 * ":helpclose": Close one help window
 */
    ਖਾਲੀ
ex_helpclose(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	ਜੇ (bt_help(win->w_ਬਫਰ))
	{
	    win_close(win, FALSE);
	    ਵਾਪਸ;
	}
    }
}

#ਜੇ defined(FEAT_MULTI_LANG)
/*
 * In an argument search for a language specifiers in the form "@xx".
 * Changes the "@" to NUL if found, and returns a pointer to "xx".
 * Returns NULL if not found.
 */
    char_u *
check_help_lang(char_u *arg)
{
    ਪੂਰਨ len = (ਪੂਰਨ)STRLEN(arg);

    ਜੇ (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	ਵਾਪਸ arg + len - 2;
    }
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
help_heuristic(
    char_u	*matched_string,
    ਪੂਰਨ		ਵਿੱਥ,			// offset for match
    ਪੂਰਨ		wrong_case)		// no matching case
{
    ਪੂਰਨ		ਅੰਕ_letters;
    char_u	*p;

    ਅੰਕ_letters = 0;
    ਲਈ (p = matched_string; *p; p++)
	ਜੇ (ASCII_ISALNUM(*p))
	    ਅੰਕ_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    ਜੇ (ASCII_ISALNUM(matched_string[ਵਿੱਥ]) && ਵਿੱਥ > 0
				 && ASCII_ISALNUM(matched_string[ਵਿੱਥ - 1]))
	ਵਿੱਥ += 10000;
    ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਥ > 2)
	ਵਿੱਥ *= 200;
    ਜੇ (wrong_case)
	ਵਿੱਥ += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    ਜੇ (matched_string[0] == '+' && matched_string[1] != NUL)
	ਵਿੱਥ += 100;
    ਵਾਪਸ (ਪੂਰਨ)(100 * ਅੰਕ_letters + STRLEN(matched_string) + ਵਿੱਥ);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    ਸਥਿਰ ਪੂਰਨ
help_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    ਅੱਖਰ    *p1;
    ਅੱਖਰ    *p2;
    ਪੂਰਨ	    cmp;

    p1 = *(ਅੱਖਰ **)s1 + ਲੜੀ_ਲੰਬਾਈ(*(ਅੱਖਰ **)s1) + 1;
    p2 = *(ਅੱਖਰ **)s2 + ਲੜੀ_ਲੰਬਾਈ(*(ਅੱਖਰ **)s2) + 1;

    // Compare by help heuristic number first.
    cmp = strcmp(p1, p2);
    ਜੇ (cmp != 0)
	ਵਾਪਸ cmp;

    // Compare by strings as tie-breaker when same heuristic number.
    ਵਾਪਸ strcmp(*(ਅੱਖਰ **)s1, *(ਅੱਖਰ **)s2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * The matches will be sorted with a "best" match algorithm.
 * When "keep_lang" is TRUE try keeping the language of the current buffer.
 */
    ਪੂਰਨ
ਲੱਭੋ_help_ਟੈਗ_ਸੂਚੀ(
    char_u	*arg,
    ਪੂਰਨ		*ਅੰਕ_matches,
    char_u	***matches,
    ਪੂਰਨ		keep_lang)
{
    char_u	*s, *d;
    ਪੂਰਨ		i;
    // Specific tags that either have a specific replacement or won't go
    // through the generic rules.
    ਸਥਿਰ ਅੱਖਰ *(except_tbl[][2]) = {
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
	{ਨਿਰਾ, ਨਿਰਾ}
    };
    ਸਥਿਰ ਅੱਖਰ *(ਵਿਅੰਜਕ_ਸਾਰਣੀ[]) = {"!=?", "!~?", "<=?", "<?", "==?", "=~?",
				   ">=?", ">?", "is?", "isnot?"};
    ਪੂਰਨ flags;

    d = IObuff;		    // assume IObuff is long enough!
    d[0] = NUL;

    ਜੇ (STRNICMP(arg, "expr-", 5) == 0)
    {
	// When the string starting with "expr-" and containing '?' and matches
	// the table, it is taken literally (but ~ is escaped).  Otherwise '?'
	// is recognized as a wildcard.
	ਲਈ (i = (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(ਵਿਅੰਜਕ_ਸਾਰਣੀ); --i >= 0; )
	    ਜੇ (STRCMP(arg + 5, ਵਿਅੰਜਕ_ਸਾਰਣੀ[i]) == 0)
	    {
		ਪੂਰਨ si = 0, di = 0;

		ਲਈ (;;)
		{
		    ਜੇ (arg[si] == '~')
			d[di++] = '\\';
		    d[di++] = arg[si];
		    ਜੇ (arg[si] == NUL)
			ਰੋਕੋ;
		    ++si;
		}
		ਰੋਕੋ;
	    }
    }
    ਨਹੀਂਤਾਂ
    {
	// Recognize a few exceptions to the rule.  Some strings that contain
	// '*'are changed to "star", otherwise '*' is recognized as a wildcard.
	ਲਈ (i = 0; except_tbl[i][0] != ਨਿਰਾ; ++i)
	    ਜੇ (STRCMP(arg, except_tbl[i][0]) == 0)
	    {
		STRCPY(d, except_tbl[i][1]);
		ਰੋਕੋ;
	    }
    }

    ਜੇ (d[0] == NUL)	// no match in table
    {
	// Replace "\S" with "/\\S", etc.  Otherwise every tag is matched.
	// Also replace "\%^" and "\%(", they match every tag too.
	// Also "\zs", "\z1", etc.
	// Also "\@<", "\@=", "\@<=", etc.
	// And also "\_$" and "\_^".
	ਜੇ (arg[0] == '\\'
		&& ((arg[1] != NUL && arg[2] == NUL)
		    || (vim_strchr((char_u *)"%_z@", arg[1]) != ਨਿਰਾ
							   && arg[2] != NUL)))
	{
	    vim_snprintf((ਅੱਖਰ *)d, IOSIZE, "/\\\\%s", arg + 1);
	    // Check for "/\\_$", should be "/\\_\$"
	    ਜੇ (d[3] == '_' && d[4] == '$')
		STRCPY(d + 4, "\\$");
	}
	ਨਹੀਂਤਾਂ
	{
	  // Replace:
	  // "[:...:]" with "\[:...:]"
	  // "[++...]" with "\[++...]"
	  // "\{" with "\\{"		   -- matching "} \}"
	    ਜੇ ((arg[0] == '[' && (arg[1] == ':'
			 || (arg[1] == '+' && arg[2] == '+')))
		    || (arg[0] == '\\' && arg[1] == '{'))
	      *d++ = '\\';

	  // If tag starts with "('", skip the "(". Fixes CTRL-] on ('option'.
	  ਜੇ (*arg == '(' && arg[1] == '\'')
	      arg++;
	  ਲਈ (s = arg; *s; ++s)
	  {
	    // Replace "|" with "bar" and '"' with "quote" to match the name of
	    // the tags for these commands.
	    // Replace "*" with ".*" and "?" with "." to match command line
	    // completion.
	    // Insert a backslash before '~', '$' and '.' to avoid their
	    // special meaning.
	    ਜੇ (d - IObuff > IOSIZE - 10)	// getting too long!?
		ਰੋਕੋ;
	    ਚੋਣ (*s)
	    {
		ਹਾਲਤ '|':   STRCPY(d, "bar");
			    d += 3;
			    ਜਾਰੀ;
		ਹਾਲਤ '"':   STRCPY(d, "quote");
			    d += 5;
			    ਜਾਰੀ;
		ਹਾਲਤ '*':   *d++ = '.';
			    ਰੋਕੋ;
		ਹਾਲਤ '?':   *d++ = '.';
			    ਜਾਰੀ;
		ਹਾਲਤ '$':
		ਹਾਲਤ '.':
		ਹਾਲਤ '~':   *d++ = '\\';
			    ਰੋਕੋ;
	    }

	    // Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	    // ":help i_^_CTRL-D" work.
	    // Insert '-' before and after "CTRL-X" when applicable.
	    ਜੇ (*s < ' ' || (*s == '^' && s[1] && (ASCII_ISALPHA(s[1])
			   || vim_strchr((char_u *)"?@[\\]^", s[1]) != ਨਿਰਾ)))
	    {
		ਜੇ (d > IObuff && d[-1] != '_' && d[-1] != '\\')
		    *d++ = '_';		// prepend a '_' to make x_CTRL-x
		STRCPY(d, "CTRL-");
		d += 5;
		ਜੇ (*s < ' ')
		{
		    *d++ = *s + '@';
		    ਜੇ (d[-1] == '\\')
			*d++ = '\\';	// double a backslash
		}
		ਨਹੀਂਤਾਂ
		    *d++ = *++s;
		ਜੇ (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		// append a '_'
		ਜਾਰੀ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*s == '^')		// "^" or "CTRL-^" or "^_"
		*d++ = '\\';

	    // Insert a backslash before a backslash after a slash, for search
	    // pattern tags: "/\|" --> "/\\|".
	    ਨਹੀਂਤਾਂ ਜੇ (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    // "CTRL-\_" -> "CTRL-\\_" to avoid the special meaning of "\_" in
	    // "CTRL-\_CTRL-N"
	    ਜੇ (STRNICMP(s, "CTRL-\\_", 7) == 0)
	    {
		STRCPY(d, "CTRL-\\\\");
		d += 7;
		s += 6;
	    }

	    *d++ = *s;

	    // If tag contains "({" or "([", tag terminates at the "(".
	    // This is for help on functions, e.g.: abs({expr}).
	    ਜੇ (*s == '(' && (s[1] == '{' || s[1] =='['))
		ਰੋਕੋ;

	    // If tag starts with ', toss everything after a second '. Fixes
	    // CTRL-] on 'option'. (would include the trailing '.').
	    ਜੇ (*s == '\'' && s > arg && *arg == '\'')
		ਰੋਕੋ;
	    // Also '{' and '}'.
	    ਜੇ (*s == '}' && s > arg && *arg == '{')
		ਰੋਕੋ;
	  }
	  *d = NUL;

	  ਜੇ (*IObuff == '`')
	  {
	      ਜੇ (d > IObuff + 2 && d[-1] == '`')
	      {
		  // remove the backticks from `command`
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-2] = NUL;
	      }
	      ਨਹੀਂਤਾਂ ਜੇ (d > IObuff + 3 && d[-2] == '`' && d[-1] == ',')
	      {
		  // remove the backticks and comma from `command`,
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-3] = NUL;
	      }
	      ਨਹੀਂਤਾਂ ਜੇ (d > IObuff + 4 && d[-3] == '`'
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
    *ਅੰਕ_matches = 0;
    flags = ਟੈਗ_HELP | ਟੈਗ_REGEXP | ਟੈਗ_ਨਾਂ | ਟੈਗ_VERBOSE | ਟੈਗ_NO_TAGFUNC;
    ਜੇ (keep_lang)
	flags |= ਟੈਗ_KEEP_LANG;
    ਜੇ (ਲੱਭੋ_ਟੈਗ_ਸੂਚੀ(IObuff, ਅੰਕ_matches, matches, flags, (ਪੂਰਨ)MAXCOL, ਨਿਰਾ) == OK
	    && *ਅੰਕ_matches > 0)
    {
	// Sort the matches found on the heuristic number that is after the
	// tag name.
	qsort((ਖਾਲੀ *)*matches, (size_t)*ਅੰਕ_matches,
					      ਆਕਾਰ(char_u *), help_compare);
	// Delete more than TAG_MANY to reduce the size of the listing.
	ਜਦੋਂ (*ਅੰਕ_matches > ਟੈਗ_MANY)
	    vim_free((*matches)[--*ਅੰਕ_matches]);
    }
    ਵਾਪਸ OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
/*
 * Cleanup matches for help tags:
 * Remove "@ab" if the top of 'helplang' is "ab" and the language of the first
 * tag matches it.  Otherwise remove "@en" if "en" is the only language.
 */
    ਖਾਲੀ
cleanup_help_ਟੈਗ_ਸੂਚੀ(ਪੂਰਨ ਅੰਕ_file, char_u **file)
{
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		len;
    char_u	buf[4];
    char_u	*p = buf;

    ਜੇ (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    ਲਈ (i = 0; i < ਅੰਕ_file; ++i)
    {
	len = (ਪੂਰਨ)STRLEN(file[i]) - 3;
	ਜੇ (len <= 0)
	    ਜਾਰੀ;
	ਜੇ (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    ਲਈ (j = 0; j < ਅੰਕ_file; ++j)
		ਜੇ (j != i && (ਪੂਰਨ)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    ਰੋਕੋ;
	    ਜੇ (j == ਅੰਕ_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    ਜੇ (*buf != NUL)
	ਲਈ (i = 0; i < ਅੰਕ_file; ++i)
	{
	    len = (ਪੂਰਨ)STRLEN(file[i]) - 3;
	    ਜੇ (len <= 0)
		ਜਾਰੀ;
	    ਜੇ (STRCMP(file[i] + len, buf) == 0)
	    {
		// remove the default language
		file[i][len] = NUL;
	    }
	}
}
#ਜੇ_ਅੰਤ

/*
 * Called when starting to edit a buffer for a help file.
 */
    ਖਾਲੀ
prepare_help_ਬਫਰ(ਖਾਲੀ)
{
    char_u	*p;

    curbuf->b_help = true;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"buftype", -1,
				     (char_u *)"help", OPT_FREE|OPT_LOCAL, 0);
#ਜੇ_ਅੰਤ

    // Always set these options after jumping to a help tag, because the
    // user may have an autocommand that gets in the way.
    // When adding an option here, also update the help file helphelp.txt.

    // Accept all ASCII chars for keywords, except ' ', '*', '"', '|', and
    // latin1 word characters (for translated help files).
    // Only set it when needed, buf_init_chartab() is some work.
    p = (char_u *)"!-~,^*,^|,^\",192-255";
    ਜੇ (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"isk", -1, p, OPT_FREE|OPT_LOCAL, 0);
	check_buf_options(curbuf);
	(ਖਾਲੀ)buf_init_chartab(curbuf, FALSE);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // Don't use the global foldmethod.
    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"fdm", -1, (char_u *)"manual",
						       OPT_FREE|OPT_LOCAL, 0);
#ਜੇ_ਅੰਤ

    curbuf->b_p_ts = 8;		// 'tabstop' is 8
    curwin->w_p_list = FALSE;	// no list mode

    curbuf->b_p_ma = FALSE;	// not modifiable
    curbuf->b_p_bin = FALSE;	// reset 'bin' before reading file
    curwin->w_p_nu = 0;		// no line numbers
    curwin->w_p_rnu = 0;	// no relative line numbers
    RESET_BINDING(curwin);	// no scroll or cursor binding
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
    curwin->w_p_arab = FALSE;	// no arabic mode
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    curwin->w_p_rl  = FALSE;	// help window is left-to-right
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    curwin->w_p_fen = FALSE;	// No folding in the help window
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    curwin->w_p_diff = FALSE;	// No 'diff'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    curwin->w_p_spell = FALSE;	// No spell checking
#ਜੇ_ਅੰਤ

    ਸੈੱਟ_buflisted(FALSE);
}

/*
 * After reading a help file: May cleanup a help buffer when syntax
 * highlighting is not used.
 */
    ਖਾਲੀ
fix_help_ਬਫਰ(ਖਾਲੀ)
{
    linenr_T	lnum;
    char_u	*ਸਤਰ;
    ਪੂਰਨ		ਵਿੱਚ_example = FALSE;
    ਪੂਰਨ		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    ਪੂਰਨ		mustfree;

    // Set filetype to "help" if still needed.
    ਜੇ (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_ਤਾਲਾ;
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"ft",
					      0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_ਤਾਲਾ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਜੇ (!syntax_present(curwin))
#ਜੇ_ਅੰਤ
    {
	ਲਈ (lnum = 1; lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; ++lnum)
	{
	    ਸਤਰ = ml_get_buf(curbuf, lnum, FALSE);
	    len = ml_get_buf_len(curbuf, lnum);
	    ਜੇ (ਵਿੱਚ_example && len > 0 && !VIM_ISWHITE(ਸਤਰ[0]))
	    {
		// End of example: non-white or '<' in first column.
		ਜੇ (ਸਤਰ[0] == '<')
		{
		    // blank-out a '<' in the first column
		    ਸਤਰ = ml_get_buf(curbuf, lnum, TRUE);
		    ਸਤਰ[0] = ' ';
		}
		ਵਿੱਚ_example = FALSE;
	    }
	    ਜੇ (!ਵਿੱਚ_example && len > 0)
	    {
		ਜੇ (ਸਤਰ[len - 1] == '>' && (len == 1 || ਸਤਰ[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    ਸਤਰ = ml_get_buf(curbuf, lnum, TRUE);
		    ਸਤਰ[len - 1] = ' ';
		    ਵਿੱਚ_example = TRUE;
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਸਤਰ[len - 1] == '~')
		{
		    // blank-out a '~' at the end of line (header marker)
		    ਸਤਰ = ml_get_buf(curbuf, lnum, TRUE);
		    ਸਤਰ[len - 1] = ' ';
		}
	    }
	}
    }

    // In the "help.txt" and "help.abx" file, add the locally added help
    // files.  This uses the very first line in the help file.
    fname = gettail(curbuf->b_fname);
    ਜੇ (fnamecmp(fname, "help.txt") == 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#ਜੇ_ਅੰਤ
	)
    {
	ਲਈ (lnum = 1; lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; ++lnum)
	{
	    ਸਤਰ = ml_get_buf(curbuf, lnum, FALSE);
	    ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)ਸਤਰ, "*local-additions*") == ਨਿਰਾ)
		ਜਾਰੀ;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    ਜਦੋਂ (*p != NUL)
	    {
		ਪੂਰਨ NameBufflen;

		NameBufflen = copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		ਜੇ (rt != ਨਿਰਾ &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    ਪੂਰਨ		fcount;
		    char_u	**fnames;
		    FILE	*fd;
		    char_u	*s;
		    ਪੂਰਨ		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    ਜੇ (*NameBuff != NUL && !after_pathsep(NameBuff, NameBuff + NameBufflen))
		    {
			STRCPY(NameBuff + NameBufflen, PATHSEPSTR);
			NameBufflen += ਆਕਾਰ(PATHSEP);
		    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
		    STRCPY(NameBuff + NameBufflen, "doc/*.??[tx]");
#ਨਹੀਂਤਾਂ
		    STRCPY(NameBuff + NameBufflen, "doc/*.txt");
#ਜੇ_ਅੰਤ
		    ਜੇ (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
			ਪੂਰਨ	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			ਲਈ (i1 = 0; i1 < fcount; ++i1)
			{
			    f1 = fnames[i1];
			    t1 = gettail(f1);
			    e1 = vim_strrchr(t1, '.');
			    ਜੇ (e1 == ਨਿਰਾ)
				ਜਾਰੀ;
			    ਜੇ (fnamecmp(e1, ".txt") != 0
					       && fnamecmp(e1, fname + 4) != 0)
			    {
				// Not .txt and not .abx, remove it.
				VIM_CLEAR(fnames[i1]);
				ਜਾਰੀ;
			    }

			    ਲਈ (i2 = i1 + 1; i2 < fcount; ++i2)
			    {
				f2 = fnames[i2];
				ਜੇ (f2 == ਨਿਰਾ)
				    ਜਾਰੀ;
				t2 = gettail(f2);
				e2 = vim_strrchr(t2, '.');
				ਜੇ (e2 == ਨਿਰਾ)
				    ਜਾਰੀ;
				ਜੇ (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    ਜਾਰੀ;
				ਜੇ (fnamecmp(e1, ".txt") == 0
					       && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#ਜੇ_ਅੰਤ
			ਲਈ (fi = 0; fi < fcount; ++fi)
			{
			    ਜੇ (fnames[fi] == ਨਿਰਾ)
				ਜਾਰੀ;
			    fd = mch_fopen((ਅੱਖਰ *)fnames[fi], "r");
			    ਜੇ (fd != ਨਿਰਾ)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				ਜੇ (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != ਨਿਰਾ)
				{
				    ਪੂਰਨ	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    ਜਦੋਂ (*s != NUL)
				    {
					ਜੇ (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					ਜੇ (*s >= 0x80 && this_utf != FALSE)
					{
					    ਪੂਰਨ	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    ਜੇ (l == 1)
						this_utf = FALSE;
					    s += l - 1;
					}
					++s;
				    }

				    // The help file is latin1 or utf-8;
				    // conversion to the current
				    // 'encoding' may be required.
				    vc.vc_type = CONV_NONE;
				    convert_ਤਿਆਰੀ(&vc, (char_u *)(
						this_utf == TRUE ? "utf-8"
						      : "latin1"), p_enc);
				    ਜੇ (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    ਨਹੀਂਤਾਂ
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    ਨਿਰਾ);
					ਜੇ (cp == ਨਿਰਾ)
					    cp = IObuff;
				    }
				    convert_ਤਿਆਰੀ(&vc, ਨਿਰਾ, ਨਿਰਾ);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    ਜੇ (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				fclose(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		ਜੇ (mustfree)
		    vim_free(rt);
	    }
	    ਰੋਕੋ;
	}
    }
}

/*
 * ":exusage"
 */
    ਖਾਲੀ
ex_exusage(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    do_cmdline_cmd((char_u *)"help ex-cmd-index");
}

/*
 * ":viusage"
 */
    ਖਾਲੀ
ex_viusage(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    do_cmdline_cmd((char_u *)"help normal-index");
}

/*
 * Generate tags in one help directory.
 */
    ਸਥਿਰ ਖਾਲੀ
helptags_one(
    char_u	*dir,		// doc directory
    char_u	*ext,		// suffix, ".txt", ".itx", ".frx", etc.
    char_u	*tagfname,	// "tags" for English, "tags-fr" for French.
    ਪੂਰਨ		ਜੋੜੋ_help_ਟੈਗ_ਸੂਚੀ,	// add "help-tags" tag
    ਪੂਰਨ		ignore_writeerr)    // ignore write error
{
    FILE	*fd_ਟੈਗ_ਸੂਚੀ;
    FILE	*fd;
    garray_T	ga;
    ਪੂਰਨ		res;
    ਪੂਰਨ		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    ਪੂਰਨ		fi;
    char_u	*s;
    ਪੂਰਨ		i;
    char_u	*fname;
    ਪੂਰਨ		dirlen;
    ਪੂਰਨ		utf8 = MAYBE;
    ਪੂਰਨ		this_utf8;
    ਪੂਰਨ		firstline;
    ਪੂਰਨ		ਵਿੱਚ_example;
    ਪੂਰਨ		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (ਪੂਰਨ)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    res = gen_expand_wildcards(1, &NameBuff, &filecount, &files,
							    EW_FILE|EW_SILENT);
    ਜੇ (res == FAIL || filecount == 0)
    {
	ਜੇ (!got_int)
	    semsg(_(e_no_ਮੇਲ_str_1), NameBuff);
	ਜੇ (res != FAIL)
	    FreeWild(filecount, files);
	ਵਾਪਸ;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    ਜੋੜੋ_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_ਟੈਗ_ਸੂਚੀ = mch_fopen((ਅੱਖਰ *)NameBuff, "w");
    ਜੇ (fd_ਟੈਗ_ਸੂਚੀ == ਨਿਰਾ)
    {
	ਜੇ (!ignore_writeerr)
	    semsg(_(e_cannot_open_str_for_writing_1), NameBuff);
	FreeWild(filecount, files);
	ਵਾਪਸ;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, ਆਕਾਰ(char_u *), 100);
    ਜੇ (ਜੋੜੋ_help_ਟੈਗ_ਸੂਚੀ || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	ਜੇ (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	ਨਹੀਂਤਾਂ
	{
	    s = alloc(18 + (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(tagfname));
	    ਜੇ (s == ਨਿਰਾ)
		got_int = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		sprintf((ਅੱਖਰ *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_ਅੰਕੜੇ)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    ਲਈ (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((ਅੱਖਰ *)files[fi], "r");
	ਜੇ (fd == ਨਿਰਾ)
	{
	    semsg(_(e_unable_ਮੰਜ਼ਿਲ_open_str_for_reading), files[fi]);
	    ਜਾਰੀ;
	}
	fname = files[fi] + dirlen + 1;

	ਵਿੱਚ_example = FALSE;
	firstline = TRUE;
	ਜਦੋਂ (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    ਜੇ (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		ਲਈ (s = IObuff; *s != NUL; ++s)
		    ਜੇ (*s >= 0x80)
		    {
			ਪੂਰਨ l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			ਜੇ (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    ਰੋਕੋ;
			}
			s += l - 1;
		    }
		ਜੇ (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		ਜੇ (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		ਨਹੀਂਤਾਂ ਜੇ (utf8 != this_utf8)
		{
		    semsg(_(e_mix_ਦਾ_help_file_encodings_within_language_str), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    ਜੇ (ਵਿੱਚ_example)
	    {
		// skip over example; a non-white in the first column ends it
		ਜੇ (vim_strchr((char_u *)" \t\n\r", IObuff[0]))
		    ਜਾਰੀ;
		ਵਿੱਚ_example = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    ਜਦੋਂ (p1 != ਨਿਰਾ)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		ਜੇ (p2 != ਨਿਰਾ && p2 > p1 + 1)	// skip "*" and "**"
		{
		    ਲਈ (s = p1 + 1; s < p2; ++s)
			ਜੇ (*s == ' ' || *s == '\t' || *s == '|')
			    ਰੋਕੋ;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    ਜੇ (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != ਨਿਰਾ
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			ਜੇ (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    ਰੋਕੋ;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			ਜੇ (s == ਨਿਰਾ)
			{
			    got_int = TRUE;
			    ਰੋਕੋ;
			}
			((char_u **)ga.ga_ਅੰਕੜੇ)[ga.ga_len] = s;
			++ga.ga_len;
			sprintf((ਅੱਖਰ *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    size_t off = STRLEN(IObuff);
	    ਜੇ (off >= 2 && IObuff[off - 1] == '\n')
	    {
		off -= 2;
		ਜਦੋਂ (off > 0 && (ASCII_ISLOWER(IObuff[off])
						  || VIM_ISDIGIT(IObuff[off])))
		    off--;
		ਜੇ (IObuff[off] == '>' && (off == 0 || IObuff[off - 1] == ' '))
		    ਵਿੱਚ_example = TRUE;
	    }
	    ਸਤਰ_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    ਜੇ (!got_int)
    {
	// Sort the tags.
	ਜੇ (ga.ga_ਅੰਕੜੇ != ਨਿਰਾ)
	    ਛਾਂਟੋ_ਲੜੀਆਂ((char_u **)ga.ga_ਅੰਕੜੇ, ga.ga_len);

	// Check for duplicates.
	ਲਈ (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_ਅੰਕੜੇ)[i - 1];
	    p2 = ((char_u **)ga.ga_ਅੰਕੜੇ)[i];
	    ਜਦੋਂ (*p1 == *p2)
	    {
		ਜੇ (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((ਅੱਖਰ *)NameBuff, MAXPATHL,
			    _(e_duplicate_ਟੈਗ_str_ਵਿੱਚ_file_str_str),
				     ((char_u **)ga.ga_ਅੰਕੜੇ)[i], dir, p2 + 1);
		    emsg((ਅੱਖਰ *)NameBuff);
		    *p2 = '\t';
		    ਰੋਕੋ;
		}
		++p1;
		++p2;
	    }
	}

	ਜੇ (utf8 == TRUE)
	    fprintf(fd_ਟੈਗ_ਸੂਚੀ, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	ਲਈ (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_ਅੰਕੜੇ)[i];
	    ਜੇ (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		fputs((ਅੱਖਰ *)s, fd_ਟੈਗ_ਸੂਚੀ);
	    ਨਹੀਂਤਾਂ
	    {
		fprintf(fd_ਟੈਗ_ਸੂਚੀ, "%s\t/*", s);
		ਲਈ (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    ਜੇ (*p1 == '\\' || *p1 == '/')
			putc('\\', fd_ਟੈਗ_ਸੂਚੀ);
		    putc(*p1, fd_ਟੈਗ_ਸੂਚੀ);
		}
		fprintf(fd_ਟੈਗ_ਸੂਚੀ, "*\n");
	    }
	}
    }
    ਜੇ (mix)
	got_int = FALSE;    // continue with other languages

    ਲਈ (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_ਅੰਕੜੇ)[i]);
    ga_clear(&ga);
    fclose(fd_ਟੈਗ_ਸੂਚੀ);	    // there is no check for an error...
}

/*
 * Generate tags in one help directory, taking care of translations.
 */
    ਸਥਿਰ ਖਾਲੀ
do_helptags(char_u *dirname, ਪੂਰਨ ਜੋੜੋ_help_ਟੈਗ_ਸੂਚੀ, ਪੂਰਨ ignore_writeerr)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    ਪੂਰਨ		len;
    ਪੂਰਨ		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    ਪੂਰਨ		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, dirname);
    ਜੋੜੋ_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    ਜੇ (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_(e_no_ਮੇਲ_str_1), NameBuff);
	ਵਾਪਸ;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    ਲਈ (i = 0; i < filecount; ++i)
    {
	len = (ਪੂਰਨ)STRLEN(files[i]);
	ਜੇ (len <= 4)
	    ਜਾਰੀ;

	ਜੇ (STRICMP(files[i] + len - 4, ".txt") == 0)
	{
	    // ".txt" -> language "en"
	    lang[0] = 'e';
	    lang[1] = 'n';
	}
	ਨਹੀਂਤਾਂ ਜੇ (files[i][len - 4] == '.'
		&& ASCII_ISALPHA(files[i][len - 3])
		&& ASCII_ISALPHA(files[i][len - 2])
		&& TOLOWER_ASC(files[i][len - 1]) == 'x')
	{
	    // ".abx" -> language "ab"
	    lang[0] = TOLOWER_ASC(files[i][len - 3]);
	    lang[1] = TOLOWER_ASC(files[i][len - 2]);
	}
	ਨਹੀਂਤਾਂ
	    ਜਾਰੀ;

	// Did we find this language already?
	ਲਈ (j = 0; j < ga.ga_len; j += 2)
	    ਜੇ (STRNCMP(lang, ((char_u *)ga.ga_ਅੰਕੜੇ) + j, 2) == 0)
		ਰੋਕੋ;
	ਜੇ (j == ga.ga_len)
	{
	    // New language, add it.
	    ਜੇ (ga_grow(&ga, 2) == FAIL)
		ਰੋਕੋ;
	    ((char_u *)ga.ga_ਅੰਕੜੇ)[ga.ga_len++] = lang[0];
	    ((char_u *)ga.ga_ਅੰਕੜੇ)[ga.ga_len++] = lang[1];
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    ਲਈ (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_ਅੰਕੜੇ)[j];
	fname[6] = ((char_u *)ga.ga_ਅੰਕੜੇ)[j + 1];
	ਜੇ (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	ਨਹੀਂਤਾਂ
	{
	    // Language "ab" uses ".abx" and "tags-ab".
	    STRCPY(ext, ".xxx");
	    ext[1] = fname[5];
	    ext[2] = fname[6];
	}
	helptags_one(dirname, ext, fname, ਜੋੜੋ_help_ਟੈਗ_ਸੂਚੀ, ignore_writeerr);
    }

    ga_clear(&ga);
    FreeWild(filecount, files);

#ਨਹੀਂਤਾਂ
    // No language support, just use "*.txt" and "tags".
    helptags_one(dirname, (char_u *)".txt", (char_u *)"tags", ਜੋੜੋ_help_ਟੈਗ_ਸੂਚੀ,
							    ignore_writeerr);
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਖਾਲੀ
helptags_cb(char_u *fname, ਖਾਲੀ *cookie)
{
    do_helptags(fname, *(ਪੂਰਨ *)cookie, TRUE);
}

/*
 * ":helptags"
 */
    ਖਾਲੀ
ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*dirname;
    ਪੂਰਨ		ਜੋੜੋ_help_ਟੈਗ_ਸੂਚੀ = FALSE;

    // Check for ":helptags ++t {dir}".
    ਜੇ (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	ਜੋੜੋ_help_ਟੈਗ_ਸੂਚੀ = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    ਜੇ (STRCMP(eap->arg, "ALL") == 0)
    {
	do_ਵਿੱਚ_ਰਸਤਾ(p_rtp, "", (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &ਜੋੜੋ_help_ਟੈਗ_ਸੂਚੀ);
    }
    ਨਹੀਂਤਾਂ
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	dirname = ExpandOne(&xpc, eap->arg, ਨਿਰਾ,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	ਜੇ (dirname == ਨਿਰਾ || !mch_isdir(dirname))
	    semsg(_(e_not_a_directory_str), eap->arg);
	ਨਹੀਂਤਾਂ
	    do_helptags(dirname, ਜੋੜੋ_help_ਟੈਗ_ਸੂਚੀ, FALSE);
	vim_free(dirname);
    }
}
