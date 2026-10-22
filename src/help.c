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

#ಅನ್ತರ್ಭಾವ "vim.h"

/*
 * ":help": open a read-only window on a help file
 */
    ಶೂನ್ಯ
ex_help(exarg_T *eap)
{
    char_u	*arg;
    char_u	*tag;
    FILE	*helpfd;	// file descriptor of help file
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		i;
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_matches;
    char_u	**matches;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		empty_fnum = 0;
    ಪೂರ್ಣಾಙ್ಕ		alt_fnum = 0;
    buf_T	*buf;
#ifdef FEAT_MULTI_LANG
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*lang;
#endif
#ifdef FEAT_FOLDING
    ಪೂರ್ಣಾಙ್ಕ		old_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
#endif

    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (eap != ರಿಕ್ತ)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	ಪ್ರತಿ (arg = eap->arg; *arg; ++arg)
	{
	    ಯದಿ (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		ಭಙ್ಗ;
	    }
	}
	arg = eap->arg;

	ಯದಿ (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_(e_dont_panic));
	    ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (eap->skip)	    // not executing commands
	    ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    ಯಾವತ್ (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#endif

    // When no argument given go to the index.
    ಯದಿ (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = find_help_tags(arg, &ಅಂಕೆ_matches, &matches,
						 eap != ರಿಕ್ತ && eap->forceit);

    i = 0;
#ifdef FEAT_MULTI_LANG
    ಯದಿ (n != FAIL && lang != ರಿಕ್ತ)
	// Find first item with the requested language.
	ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_matches; ++i)
	{
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(matches[i]);
	    ಯದಿ (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		ಭಙ್ಗ;
	}
#endif
    ಯದಿ (i >= ಅಂಕೆ_matches || n == FAIL)
    {
#ifdef FEAT_MULTI_LANG
	ಯದಿ (lang != ರಿಕ್ತ)
	    semsg(_(e_sorry_no_str_help_for_str), lang, arg);
	ಅನ್ಯಥಾ
#endif
	    semsg(_(e_sorry_no_help_for_str), arg);
	ಯದಿ (n != FAIL)
	    FreeWild(ಅಂಕೆ_matches, matches);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // The first match (in the requested language) is the best match.
    tag = vim_strsave(matches[i]);
    FreeWild(ಅಂಕೆ_matches, matches);

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    ಯದಿ (!bt_help(curwin->w_buffer) || cmdmod.cmod_tab != 0)
    {
	ಯದಿ (cmdmod.cmod_tab != 0)
	    wp = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    FOR_ALL_WINDOWS(wp)
		ಯದಿ (bt_help(wp->w_buffer))
		    ಭಙ್ಗ;
	ಯದಿ (wp != ರಿಕ್ತ && wp->w_buffer->b_nwindows > 0)
	    win_enter(wp, TRUE);
	ಅನ್ಯಥಾ
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    ಯದಿ ((helpfd = mch_fopen((ವರ್ಣ *)p_hf, READBIN)) == ರಿಕ್ತ)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		ಗಚ್ಛ erret;
	    }
	    fclose(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    ಯದಿ (cmdmod.cmod_split == 0 && curwin->w_width != Columns
						  && curwin->w_width < 80)
		n |= p_sb ? WSP_BOT : WSP_ತುದಿ;
	    ಯದಿ (win_split(0, n) == FAIL)
		ಗಚ್ಛ erret;

	    ಯದಿ (curwin->w_height < p_hh)
		win_setheight((ಪೂರ್ಣಾಙ್ಕ)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (ಶೂನ್ಯ)do_ecmd(0, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ECMD_LASTL,
			  ECMD_HIDE + ECMD_SET_HELP,
			  ರಿಕ್ತ);  // buffer is still open, don't store info
	    ಯದಿ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    empty_fnum = curbuf->b_fnum;
	}
    }

    ಯದಿ (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#ifdef FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    ಕೀಲಿ_Typed = old_ಕೀಲಿ_Typed;
#endif

    ಯದಿ (tag != ರಿಕ್ತ)
	do_tag(tag, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    ಯದಿ (empty_fnum != 0 && curbuf->b_fnum != empty_fnum)
    {
	buf = buflist_findnr(empty_fnum);
	ಯದಿ (buf != ರಿಕ್ತ && buf->b_nwindows == 0)
	    wipe_buffer(buf, TRUE);
    }

    // keep the previous alternate file
    ಯದಿ (alt_fnum != 0 && curwin->w_alt_fnum == empty_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(tag);
}

/*
 * ":helpclose": Close one help window
 */
    ಶೂನ್ಯ
ex_helpclose(exarg_T *eap UNUSED)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	ಯದಿ (bt_help(win->w_buffer))
	{
	    win_close(win, FALSE);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
}

#ಯದಿ defined(FEAT_MULTI_LANG)
/*
 * In an argument search for a language specifiers in the form "@xx".
 * Changes the "@" to NUL if found, and returns a pointer to "xx".
 * Returns NULL if not found.
 */
    char_u *
check_help_lang(char_u *arg)
{
    ಪೂರ್ಣಾಙ್ಕ len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(arg);

    ಯದಿ (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	ಪ್ರತ್ಯಾಗಮ arg + len - 2;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
    ಪೂರ್ಣಾಙ್ಕ
help_heuristic(
    char_u	*matched_string,
    ಪೂರ್ಣಾಙ್ಕ		offset,			// offset for match
    ಪೂರ್ಣಾಙ್ಕ		wrong_case)		// no matching case
{
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_letters;
    char_u	*p;

    ಅಂಕೆ_letters = 0;
    ಪ್ರತಿ (p = matched_string; *p; p++)
	ಯದಿ (ASCII_ISALNUM(*p))
	    ಅಂಕೆ_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    ಯದಿ (ASCII_ISALNUM(matched_string[offset]) && offset > 0
				 && ASCII_ISALNUM(matched_string[offset - 1]))
	offset += 10000;
    ಅನ್ಯಥಾ ಯದಿ (offset > 2)
	offset *= 200;
    ಯದಿ (wrong_case)
	offset += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    ಯದಿ (matched_string[0] == '+' && matched_string[1] != NUL)
	offset += 100;
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(100 * ಅಂಕೆ_letters + STRLEN(matched_string) + offset);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
help_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    ವರ್ಣ    *p1;
    ವರ್ಣ    *p2;
    ಪೂರ್ಣಾಙ್ಕ	    cmp;

    p1 = *(ವರ್ಣ **)s1 + strlen(*(ವರ್ಣ **)s1) + 1;
    p2 = *(ವರ್ಣ **)s2 + strlen(*(ವರ್ಣ **)s2) + 1;

    // Compare by help heuristic number first.
    cmp = strcmp(p1, p2);
    ಯದಿ (cmp != 0)
	ಪ್ರತ್ಯಾಗಮ cmp;

    // Compare by strings as tie-breaker when same heuristic number.
    ಪ್ರತ್ಯಾಗಮ strcmp(*(ವರ್ಣ **)s1, *(ವರ್ಣ **)s2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * The matches will be sorted with a "best" match algorithm.
 * When "keep_lang" is TRUE try keeping the language of the current buffer.
 */
    ಪೂರ್ಣಾಙ್ಕ
find_help_tags(
    char_u	*arg,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_matches,
    char_u	***matches,
    ಪೂರ್ಣಾಙ್ಕ		keep_lang)
{
    char_u	*s, *d;
    ಪೂರ್ಣಾಙ್ಕ		i;
    // Specific tags that either have a specific replacement or won't go
    // through the generic rules.
    ಸ್ಥಿರ ವರ್ಣ *(except_tbl[][2]) = {
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
	{ರಿಕ್ತ, ರಿಕ್ತ}
    };
    ಸ್ಥಿರ ವರ್ಣ *(ಅಭಿವ್ಯಕ್ತಿ_table[]) = {"!=?", "!~?", "<=?", "<?", "==?", "=~?",
				   ">=?", ">?", "is?", "isnot?"};
    ಪೂರ್ಣಾಙ್ಕ flags;

    d = IObuff;		    // assume IObuff is long enough!
    d[0] = NUL;

    ಯದಿ (STRNICMP(arg, "expr-", 5) == 0)
    {
	// When the string starting with "expr-" and containing '?' and matches
	// the table, it is taken literally (but ~ is escaped).  Otherwise '?'
	// is recognized as a wildcard.
	ಪ್ರತಿ (i = (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(ಅಭಿವ್ಯಕ್ತಿ_table); --i >= 0; )
	    ಯದಿ (STRCMP(arg + 5, ಅಭಿವ್ಯಕ್ತಿ_table[i]) == 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ si = 0, di = 0;

		ಪ್ರತಿ (;;)
		{
		    ಯದಿ (arg[si] == '~')
			d[di++] = '\\';
		    d[di++] = arg[si];
		    ಯದಿ (arg[si] == NUL)
			ಭಙ್ಗ;
		    ++si;
		}
		ಭಙ್ಗ;
	    }
    }
    ಅನ್ಯಥಾ
    {
	// Recognize a few exceptions to the rule.  Some strings that contain
	// '*'are changed to "star", otherwise '*' is recognized as a wildcard.
	ಪ್ರತಿ (i = 0; except_tbl[i][0] != ರಿಕ್ತ; ++i)
	    ಯದಿ (STRCMP(arg, except_tbl[i][0]) == 0)
	    {
		STRCPY(d, except_tbl[i][1]);
		ಭಙ್ಗ;
	    }
    }

    ಯದಿ (d[0] == NUL)	// no match in table
    {
	// Replace "\S" with "/\\S", etc.  Otherwise every tag is matched.
	// Also replace "\%^" and "\%(", they match every tag too.
	// Also "\zs", "\z1", etc.
	// Also "\@<", "\@=", "\@<=", etc.
	// And also "\_$" and "\_^".
	ಯದಿ (arg[0] == '\\'
		&& ((arg[1] != NUL && arg[2] == NUL)
		    || (vim_strchr((char_u *)"%_z@", arg[1]) != ರಿಕ್ತ
							   && arg[2] != NUL)))
	{
	    vim_snprintf((ವರ್ಣ *)d, IOSIZE, "/\\\\%s", arg + 1);
	    // Check for "/\\_$", should be "/\\_\$"
	    ಯದಿ (d[3] == '_' && d[4] == '$')
		STRCPY(d + 4, "\\$");
	}
	ಅನ್ಯಥಾ
	{
	  // Replace:
	  // "[:...:]" with "\[:...:]"
	  // "[++...]" with "\[++...]"
	  // "\{" with "\\{"		   -- matching "} \}"
	    ಯದಿ ((arg[0] == '[' && (arg[1] == ':'
			 || (arg[1] == '+' && arg[2] == '+')))
		    || (arg[0] == '\\' && arg[1] == '{'))
	      *d++ = '\\';

	  // If tag starts with "('", skip the "(". Fixes CTRL-] on ('option'.
	  ಯದಿ (*arg == '(' && arg[1] == '\'')
	      arg++;
	  ಪ್ರತಿ (s = arg; *s; ++s)
	  {
	    // Replace "|" with "bar" and '"' with "quote" to match the name of
	    // the tags for these commands.
	    // Replace "*" with ".*" and "?" with "." to match command line
	    // completion.
	    // Insert a backslash before '~', '$' and '.' to avoid their
	    // special meaning.
	    ಯದಿ (d - IObuff > IOSIZE - 10)	// getting too long!?
		ಭಙ್ಗ;
	    ವಿಕಲ್ಪ (*s)
	    {
		ಪಕ್ಷ '|':   STRCPY(d, "bar");
			    d += 3;
			    ಅನುವರ್ತ;
		ಪಕ್ಷ '"':   STRCPY(d, "quote");
			    d += 5;
			    ಅನುವರ್ತ;
		ಪಕ್ಷ '*':   *d++ = '.';
			    ಭಙ್ಗ;
		ಪಕ್ಷ '?':   *d++ = '.';
			    ಅನುವರ್ತ;
		ಪಕ್ಷ '$':
		ಪಕ್ಷ '.':
		ಪಕ್ಷ '~':   *d++ = '\\';
			    ಭಙ್ಗ;
	    }

	    // Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	    // ":help i_^_CTRL-D" work.
	    // Insert '-' before and after "CTRL-X" when applicable.
	    ಯದಿ (*s < ' ' || (*s == '^' && s[1] && (ASCII_ISALPHA(s[1])
			   || vim_strchr((char_u *)"?@[\\]^", s[1]) != ರಿಕ್ತ)))
	    {
		ಯದಿ (d > IObuff && d[-1] != '_' && d[-1] != '\\')
		    *d++ = '_';		// prepend a '_' to make x_CTRL-x
		STRCPY(d, "CTRL-");
		d += 5;
		ಯದಿ (*s < ' ')
		{
		    *d++ = *s + '@';
		    ಯದಿ (d[-1] == '\\')
			*d++ = '\\';	// double a backslash
		}
		ಅನ್ಯಥಾ
		    *d++ = *++s;
		ಯದಿ (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		// append a '_'
		ಅನುವರ್ತ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*s == '^')		// "^" or "CTRL-^" or "^_"
		*d++ = '\\';

	    // Insert a backslash before a backslash after a slash, for search
	    // pattern tags: "/\|" --> "/\\|".
	    ಅನ್ಯಥಾ ಯದಿ (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    // "CTRL-\_" -> "CTRL-\\_" to avoid the special meaning of "\_" in
	    // "CTRL-\_CTRL-N"
	    ಯದಿ (STRNICMP(s, "CTRL-\\_", 7) == 0)
	    {
		STRCPY(d, "CTRL-\\\\");
		d += 7;
		s += 6;
	    }

	    *d++ = *s;

	    // If tag contains "({" or "([", tag terminates at the "(".
	    // This is for help on functions, e.g.: abs({expr}).
	    ಯದಿ (*s == '(' && (s[1] == '{' || s[1] =='['))
		ಭಙ್ಗ;

	    // If tag starts with ', toss everything after a second '. Fixes
	    // CTRL-] on 'option'. (would include the trailing '.').
	    ಯದಿ (*s == '\'' && s > arg && *arg == '\'')
		ಭಙ್ಗ;
	    // Also '{' and '}'.
	    ಯದಿ (*s == '}' && s > arg && *arg == '{')
		ಭಙ್ಗ;
	  }
	  *d = NUL;

	  ಯದಿ (*IObuff == '`')
	  {
	      ಯದಿ (d > IObuff + 2 && d[-1] == '`')
	      {
		  // remove the backticks from `command`
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-2] = NUL;
	      }
	      ಅನ್ಯಥಾ ಯದಿ (d > IObuff + 3 && d[-2] == '`' && d[-1] == ',')
	      {
		  // remove the backticks and comma from `command`,
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-3] = NUL;
	      }
	      ಅನ್ಯಥಾ ಯದಿ (d > IObuff + 4 && d[-3] == '`'
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
    *ಅಂಕೆ_matches = 0;
    flags = TAG_HELP | TAG_REGEXP | TAG_NAMES | TAG_VERBOSE | TAG_NO_TAGFUNC;
    ಯದಿ (keep_lang)
	flags |= TAG_KEEP_LANG;
    ಯದಿ (find_tags(IObuff, ಅಂಕೆ_matches, matches, flags, (ಪೂರ್ಣಾಙ್ಕ)MAXCOL, ರಿಕ್ತ) == OK
	    && *ಅಂಕೆ_matches > 0)
    {
	// Sort the matches found on the heuristic number that is after the
	// tag name.
	qsort((ಶೂನ್ಯ *)*matches, (size_t)*ಅಂಕೆ_matches,
					      ಪರಿಮಾಣ(char_u *), help_compare);
	// Delete more than TAG_MANY to reduce the size of the listing.
	ಯಾವತ್ (*ಅಂಕೆ_matches > TAG_MANY)
	    vim_free((*matches)[--*ಅಂಕೆ_matches]);
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

#ifdef FEAT_MULTI_LANG
/*
 * Cleanup matches for help tags:
 * Remove "@ab" if the top of 'helplang' is "ab" and the language of the first
 * tag matches it.  Otherwise remove "@en" if "en" is the only language.
 */
    ಶೂನ್ಯ
cleanup_help_tags(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_file, char_u **file)
{
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	buf[4];
    char_u	*p = buf;

    ಯದಿ (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_file; ++i)
    {
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(file[i]) - 3;
	ಯದಿ (len <= 0)
	    ಅನುವರ್ತ;
	ಯದಿ (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    ಪ್ರತಿ (j = 0; j < ಅಂಕೆ_file; ++j)
		ಯದಿ (j != i && (ಪೂರ್ಣಾಙ್ಕ)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    ಭಙ್ಗ;
	    ಯದಿ (j == ಅಂಕೆ_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    ಯದಿ (*buf != NUL)
	ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_file; ++i)
	{
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(file[i]) - 3;
	    ಯದಿ (len <= 0)
		ಅನುವರ್ತ;
	    ಯದಿ (STRCMP(file[i] + len, buf) == 0)
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
    ಶೂನ್ಯ
prepare_help_buffer(ಶೂನ್ಯ)
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
    ಯದಿ (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	set_string_option_direct((char_u *)"isk", -1, p, OPT_FREE|OPT_LOCAL, 0);
	check_buf_options(curbuf);
	(ಶೂನ್ಯ)buf_init_chartab(curbuf, FALSE);
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
    ಶೂನ್ಯ
fix_help_buffer(ಶೂನ್ಯ)
{
    linenr_T	lnum;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		in_example = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    ಪೂರ್ಣಾಙ್ಕ		mustfree;

    // Set filetype to "help" if still needed.
    ಯದಿ (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_lock;
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"ft",
					      0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_lock;
    }

#ifdef FEAT_SYN_HL
    ಯದಿ (!syntax_present(curwin))
#endif
    {
	ಪ್ರತಿ (lnum = 1; lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
	{
	    ಸಾಲು = ml_get_buf(curbuf, lnum, FALSE);
	    len = ml_get_buf_len(curbuf, lnum);
	    ಯದಿ (in_example && len > 0 && !VIM_ISWHITE(ಸಾಲು[0]))
	    {
		// End of example: non-white or '<' in first column.
		ಯದಿ (ಸಾಲು[0] == '<')
		{
		    // blank-out a '<' in the first column
		    ಸಾಲು = ml_get_buf(curbuf, lnum, TRUE);
		    ಸಾಲು[0] = ' ';
		}
		in_example = FALSE;
	    }
	    ಯದಿ (!in_example && len > 0)
	    {
		ಯದಿ (ಸಾಲು[len - 1] == '>' && (len == 1 || ಸಾಲು[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    ಸಾಲು = ml_get_buf(curbuf, lnum, TRUE);
		    ಸಾಲು[len - 1] = ' ';
		    in_example = TRUE;
		}
		ಅನ್ಯಥಾ ಯದಿ (ಸಾಲು[len - 1] == '~')
		{
		    // blank-out a '~' at the end of line (header marker)
		    ಸಾಲು = ml_get_buf(curbuf, lnum, TRUE);
		    ಸಾಲು[len - 1] = ' ';
		}
	    }
	}
    }

    // In the "help.txt" and "help.abx" file, add the locally added help
    // files.  This uses the very first line in the help file.
    fname = gettail(curbuf->b_fname);
    ಯದಿ (fnamecmp(fname, "help.txt") == 0
#ifdef FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#endif
	)
    {
	ಪ್ರತಿ (lnum = 1; lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
	{
	    ಸಾಲು = ml_get_buf(curbuf, lnum, FALSE);
	    ಯದಿ (strstr((ವರ್ಣ *)ಸಾಲು, "*local-additions*") == ರಿಕ್ತ)
		ಅನುವರ್ತ;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    ಯಾವತ್ (*p != NUL)
	    {
		ಪೂರ್ಣಾಙ್ಕ NameBufflen;

		NameBufflen = copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		ಯದಿ (rt != ರಿಕ್ತ &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    ಪೂರ್ಣಾಙ್ಕ		fcount;
		    char_u	**fnames;
		    FILE	*fd;
		    char_u	*s;
		    ಪೂರ್ಣಾಙ್ಕ		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    ಯದಿ (*NameBuff != NUL && !after_pathsep(NameBuff, NameBuff + NameBufflen))
		    {
			STRCPY(NameBuff + NameBufflen, PATHSEPSTR);
			NameBufflen += ಪರಿಮಾಣ(PATHSEP);
		    }
#ifdef FEAT_MULTI_LANG
		    STRCPY(NameBuff + NameBufflen, "doc/*.??[tx]");
#ಅನ್ಯಥಾ
		    STRCPY(NameBuff + NameBufflen, "doc/*.txt");
#endif
		    ಯದಿ (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#ifdef FEAT_MULTI_LANG
			ಪೂರ್ಣಾಙ್ಕ	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			ಪ್ರತಿ (i1 = 0; i1 < fcount; ++i1)
			{
			    f1 = fnames[i1];
			    t1 = gettail(f1);
			    e1 = vim_strrchr(t1, '.');
			    ಯದಿ (e1 == ರಿಕ್ತ)
				ಅನುವರ್ತ;
			    ಯದಿ (fnamecmp(e1, ".txt") != 0
					       && fnamecmp(e1, fname + 4) != 0)
			    {
				// Not .txt and not .abx, remove it.
				VIM_CLEAR(fnames[i1]);
				ಅನುವರ್ತ;
			    }

			    ಪ್ರತಿ (i2 = i1 + 1; i2 < fcount; ++i2)
			    {
				f2 = fnames[i2];
				ಯದಿ (f2 == ರಿಕ್ತ)
				    ಅನುವರ್ತ;
				t2 = gettail(f2);
				e2 = vim_strrchr(t2, '.');
				ಯದಿ (e2 == ರಿಕ್ತ)
				    ಅನುವರ್ತ;
				ಯದಿ (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    ಅನುವರ್ತ;
				ಯದಿ (fnamecmp(e1, ".txt") == 0
					       && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#endif
			ಪ್ರತಿ (fi = 0; fi < fcount; ++fi)
			{
			    ಯದಿ (fnames[fi] == ರಿಕ್ತ)
				ಅನುವರ್ತ;
			    fd = mch_fopen((ವರ್ಣ *)fnames[fi], "r");
			    ಯದಿ (fd != ರಿಕ್ತ)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				ಯದಿ (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != ರಿಕ್ತ)
				{
				    ಪೂರ್ಣಾಙ್ಕ	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    ಯಾವತ್ (*s != NUL)
				    {
					ಯದಿ (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					ಯದಿ (*s >= 0x80 && this_utf != FALSE)
					{
					    ಪೂರ್ಣಾಙ್ಕ	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    ಯದಿ (l == 1)
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
				    ಯದಿ (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    ಅನ್ಯಥಾ
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    ರಿಕ್ತ);
					ಯದಿ (cp == ರಿಕ್ತ)
					    cp = IObuff;
				    }
				    convert_setup(&vc, ರಿಕ್ತ, ರಿಕ್ತ);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    ಯದಿ (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				fclose(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		ಯದಿ (mustfree)
		    vim_free(rt);
	    }
	    ಭಙ್ಗ;
	}
    }
}

/*
 * ":exusage"
 */
    ಶೂನ್ಯ
ex_exusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help ex-cmd-index");
}

/*
 * ":viusage"
 */
    ಶೂನ್ಯ
ex_viusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help normal-index");
}

/*
 * Generate tags in one help directory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
helptags_one(
    char_u	*dir,		// doc directory
    char_u	*ext,		// suffix, ".txt", ".itx", ".frx", etc.
    char_u	*tagfname,	// "tags" for English, "tags-fr" for French.
    ಪೂರ್ಣಾಙ್ಕ		add_help_tags,	// add "help-tags" tag
    ಪೂರ್ಣಾಙ್ಕ		ignore_writeerr)    // ignore write error
{
    FILE	*fd_tags;
    FILE	*fd;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		res;
    ಪೂರ್ಣಾಙ್ಕ		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    ಪೂರ್ಣಾಙ್ಕ		fi;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*fname;
    ಪೂರ್ಣಾಙ್ಕ		dirlen;
    ಪೂರ್ಣಾಙ್ಕ		utf8 = MAYBE;
    ಪೂರ್ಣಾಙ್ಕ		this_utf8;
    ಪೂರ್ಣಾಙ್ಕ		firstline;
    ಪೂರ್ಣಾಙ್ಕ		in_example;
    ಪೂರ್ಣಾಙ್ಕ		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    res = gen_expand_wildcards(1, &NameBuff, &filecount, &files,
							    EW_FILE|EW_SILENT);
    ಯದಿ (res == FAIL || filecount == 0)
    {
	ಯದಿ (!got_int)
	    semsg(_(e_no_match_str_1), NameBuff);
	ಯದಿ (res != FAIL)
	    FreeWild(filecount, files);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_tags = mch_fopen((ವರ್ಣ *)NameBuff, "w");
    ಯದಿ (fd_tags == ರಿಕ್ತ)
    {
	ಯದಿ (!ignore_writeerr)
	    semsg(_(e_cannot_open_str_for_writing_1), NameBuff);
	FreeWild(filecount, files);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, ಪರಿಮಾಣ(char_u *), 100);
    ಯದಿ (add_help_tags || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	ಯದಿ (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	ಅನ್ಯಥಾ
	{
	    s = alloc(18 + (unsigned)STRLEN(tagfname));
	    ಯದಿ (s == ರಿಕ್ತ)
		got_int = TRUE;
	    ಅನ್ಯಥಾ
	    {
		sprintf((ವರ್ಣ *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_data)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    ಪ್ರತಿ (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((ವರ್ಣ *)files[fi], "r");
	ಯದಿ (fd == ರಿಕ್ತ)
	{
	    semsg(_(e_unable_to_open_str_for_reading), files[fi]);
	    ಅನುವರ್ತ;
	}
	fname = files[fi] + dirlen + 1;

	in_example = FALSE;
	firstline = TRUE;
	ಯಾವತ್ (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    ಯದಿ (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		ಪ್ರತಿ (s = IObuff; *s != NUL; ++s)
		    ಯದಿ (*s >= 0x80)
		    {
			ಪೂರ್ಣಾಙ್ಕ l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			ಯದಿ (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    ಭಙ್ಗ;
			}
			s += l - 1;
		    }
		ಯದಿ (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		ಯದಿ (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		ಅನ್ಯಥಾ ಯದಿ (utf8 != this_utf8)
		{
		    semsg(_(e_mix_of_help_file_encodings_within_language_str), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    ಯದಿ (in_example)
	    {
		// skip over example; a non-white in the first column ends it
		ಯದಿ (vim_strchr((char_u *)" \t\n\r", IObuff[0]))
		    ಅನುವರ್ತ;
		in_example = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    ಯಾವತ್ (p1 != ರಿಕ್ತ)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		ಯದಿ (p2 != ರಿಕ್ತ && p2 > p1 + 1)	// skip "*" and "**"
		{
		    ಪ್ರತಿ (s = p1 + 1; s < p2; ++s)
			ಯದಿ (*s == ' ' || *s == '\t' || *s == '|')
			    ಭಙ್ಗ;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    ಯದಿ (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != ರಿಕ್ತ
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			ಯದಿ (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    ಭಙ್ಗ;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			ಯದಿ (s == ರಿಕ್ತ)
			{
			    got_int = TRUE;
			    ಭಙ್ಗ;
			}
			((char_u **)ga.ga_data)[ga.ga_len] = s;
			++ga.ga_len;
			sprintf((ವರ್ಣ *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    size_t off = STRLEN(IObuff);
	    ಯದಿ (off >= 2 && IObuff[off - 1] == '\n')
	    {
		off -= 2;
		ಯಾವತ್ (off > 0 && (ASCII_ISLOWER(IObuff[off])
						  || VIM_ISDIGIT(IObuff[off])))
		    off--;
		ಯದಿ (IObuff[off] == '>' && (off == 0 || IObuff[off - 1] == ' '))
		    in_example = TRUE;
	    }
	    ಸಾಲು_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    ಯದಿ (!got_int)
    {
	// Sort the tags.
	ಯದಿ (ga.ga_data != ರಿಕ್ತ)
	    sort_strings((char_u **)ga.ga_data, ga.ga_len);

	// Check for duplicates.
	ಪ್ರತಿ (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_data)[i - 1];
	    p2 = ((char_u **)ga.ga_data)[i];
	    ಯಾವತ್ (*p1 == *p2)
	    {
		ಯದಿ (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((ವರ್ಣ *)NameBuff, MAXPATHL,
			    _(e_duplicate_tag_str_in_file_str_str),
				     ((char_u **)ga.ga_data)[i], dir, p2 + 1);
		    emsg((ವರ್ಣ *)NameBuff);
		    *p2 = '\t';
		    ಭಙ್ಗ;
		}
		++p1;
		++p2;
	    }
	}

	ಯದಿ (utf8 == TRUE)
	    fprintf(fd_tags, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	ಪ್ರತಿ (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_data)[i];
	    ಯದಿ (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		fputs((ವರ್ಣ *)s, fd_tags);
	    ಅನ್ಯಥಾ
	    {
		fprintf(fd_tags, "%s\t/*", s);
		ಪ್ರತಿ (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    ಯದಿ (*p1 == '\\' || *p1 == '/')
			putc('\\', fd_tags);
		    putc(*p1, fd_tags);
		}
		fprintf(fd_tags, "*\n");
	    }
	}
    }
    ಯದಿ (mix)
	got_int = FALSE;    // continue with other languages

    ಪ್ರತಿ (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_data)[i]);
    ga_clear(&ga);
    fclose(fd_tags);	    // there is no check for an error...
}

/*
 * Generate tags in one help directory, taking care of translations.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_helptags(char_u *dirname, ಪೂರ್ಣಾಙ್ಕ add_help_tags, ಪೂರ್ಣಾಙ್ಕ ignore_writeerr)
{
#ifdef FEAT_MULTI_LANG
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    ಪೂರ್ಣಾಙ್ಕ		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, dirname);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    ಯದಿ (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_(e_no_match_str_1), NameBuff);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    ಪ್ರತಿ (i = 0; i < filecount; ++i)
    {
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(files[i]);
	ಯದಿ (len <= 4)
	    ಅನುವರ್ತ;

	ಯದಿ (STRICMP(files[i] + len - 4, ".txt") == 0)
	{
	    // ".txt" -> language "en"
	    lang[0] = 'e';
	    lang[1] = 'n';
	}
	ಅನ್ಯಥಾ ಯದಿ (files[i][len - 4] == '.'
		&& ASCII_ISALPHA(files[i][len - 3])
		&& ASCII_ISALPHA(files[i][len - 2])
		&& TOLOWER_ASC(files[i][len - 1]) == 'x')
	{
	    // ".abx" -> language "ab"
	    lang[0] = TOLOWER_ASC(files[i][len - 3]);
	    lang[1] = TOLOWER_ASC(files[i][len - 2]);
	}
	ಅನ್ಯಥಾ
	    ಅನುವರ್ತ;

	// Did we find this language already?
	ಪ್ರತಿ (j = 0; j < ga.ga_len; j += 2)
	    ಯದಿ (STRNCMP(lang, ((char_u *)ga.ga_data) + j, 2) == 0)
		ಭಙ್ಗ;
	ಯದಿ (j == ga.ga_len)
	{
	    // New language, add it.
	    ಯದಿ (ga_grow(&ga, 2) == FAIL)
		ಭಙ್ಗ;
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[0];
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[1];
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    ಪ್ರತಿ (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_data)[j];
	fname[6] = ((char_u *)ga.ga_data)[j + 1];
	ಯದಿ (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	ಅನ್ಯಥಾ
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

#ಅನ್ಯಥಾ
    // No language support, just use "*.txt" and "tags".
    helptags_one(dirname, (char_u *)".txt", (char_u *)"tags", add_help_tags,
							    ignore_writeerr);
#endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
helptags_cb(char_u *fname, ಶೂನ್ಯ *cookie)
{
    do_helptags(fname, *(ಪೂರ್ಣಾಙ್ಕ *)cookie, TRUE);
}

/*
 * ":helptags"
 */
    ಶೂನ್ಯ
ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*dirname;
    ಪೂರ್ಣಾಙ್ಕ		add_help_tags = FALSE;

    // Check for ":helptags ++t {dir}".
    ಯದಿ (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	add_help_tags = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    ಯದಿ (STRCMP(eap->arg, "ALL") == 0)
    {
	do_in_path(p_rtp, "", (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &add_help_tags);
    }
    ಅನ್ಯಥಾ
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	dirname = ExpandOne(&xpc, eap->arg, ರಿಕ್ತ,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	ಯದಿ (dirname == ರಿಕ್ತ || !mch_isdir(dirname))
	    semsg(_(e_not_a_directory_str), eap->arg);
	ಅನ್ಯಥಾ
	    do_helptags(dirname, add_help_tags, FALSE);
	vim_free(dirname);
    }
}
