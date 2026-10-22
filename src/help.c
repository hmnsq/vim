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

#અન્તર્ભાવ "vim.h"

/*
 * ":help": open a read-only window on a help file
 */
    શૂન્ય
ex_help(exarg_T *eap)
{
    char_u	*arg;
    char_u	*tag;
    FILE	*helpfd;	// file descriptor of help file
    પૂર્ણાંક		n;
    પૂર્ણાંક		i;
    win_T	*wp;
    પૂર્ણાંક		અંક_matches;
    char_u	**matches;
    char_u	*p;
    પૂર્ણાંક		empty_fnum = 0;
    પૂર્ણાંક		alt_fnum = 0;
    buf_T	*buf;
#ifdef FEAT_MULTI_LANG
    પૂર્ણાંક		len;
    char_u	*lang;
#endif
#ifdef FEAT_FOLDING
    પૂર્ણાંક		old_ચાવી_Typed = ચાવી_Typed;
#endif

    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;

    જો (eap != ખાલી)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	માટે (arg = eap->arg; *arg; ++arg)
	{
	    જો (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		તોડો;
	    }
	}
	arg = eap->arg;

	જો (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_(e_dont_panic));
	    પાછા;
	}

	જો (eap->skip)	    // not executing commands
	    પાછા;
    }
    નહીંતર
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    જ્યારે (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#endif

    // When no argument given go to the index.
    જો (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = find_help_tags(arg, &અંક_matches, &matches,
						 eap != ખાલી && eap->forceit);

    i = 0;
#ifdef FEAT_MULTI_LANG
    જો (n != FAIL && lang != ખાલી)
	// Find first item with the requested language.
	માટે (i = 0; i < અંક_matches; ++i)
	{
	    len = (પૂર્ણાંક)STRLEN(matches[i]);
	    જો (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		તોડો;
	}
#endif
    જો (i >= અંક_matches || n == FAIL)
    {
#ifdef FEAT_MULTI_LANG
	જો (lang != ખાલી)
	    semsg(_(e_sorry_no_str_help_for_str), lang, arg);
	નહીંતર
#endif
	    semsg(_(e_sorry_no_help_for_str), arg);
	જો (n != FAIL)
	    FreeWild(અંક_matches, matches);
	પાછા;
    }

    // The first match (in the requested language) is the best match.
    tag = vim_strsave(matches[i]);
    FreeWild(અંક_matches, matches);

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    જો (!bt_help(curwin->w_buffer) || cmdmod.cmod_tab != 0)
    {
	જો (cmdmod.cmod_tab != 0)
	    wp = ખાલી;
	નહીંતર
	    FOR_ALL_WINDOWS(wp)
		જો (bt_help(wp->w_buffer))
		    તોડો;
	જો (wp != ખાલી && wp->w_buffer->b_nwindows > 0)
	    win_enter(wp, TRUE);
	નહીંતર
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    જો ((helpfd = mch_fopen((અક્ષર *)p_hf, READBIN)) == ખાલી)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		જાઓ erret;
	    }
	    fclose(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    જો (cmdmod.cmod_split == 0 && curwin->w_width != Columns
						  && curwin->w_width < 80)
		n |= p_sb ? WSP_BOT : WSP_ટોચ;
	    જો (win_split(0, n) == FAIL)
		જાઓ erret;

	    જો (curwin->w_height < p_hh)
		win_setheight((પૂર્ણાંક)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (શૂન્ય)do_ecmd(0, ખાલી, ખાલી, ખાલી, ECMD_LASTL,
			  ECMD_HIDE + ECMD_SET_HELP,
			  ખાલી);  // buffer is still open, don't store info
	    જો ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    empty_fnum = curbuf->b_fnum;
	}
    }

    જો (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#ifdef FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    ચાવી_Typed = old_ચાવી_Typed;
#endif

    જો (tag != ખાલી)
	do_tag(tag, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    જો (empty_fnum != 0 && curbuf->b_fnum != empty_fnum)
    {
	buf = buflist_findnr(empty_fnum);
	જો (buf != ખાલી && buf->b_nwindows == 0)
	    wipe_buffer(buf, TRUE);
    }

    // keep the previous alternate file
    જો (alt_fnum != 0 && curwin->w_alt_fnum == empty_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(tag);
}

/*
 * ":helpclose": Close one help window
 */
    શૂન્ય
ex_helpclose(exarg_T *eap UNUSED)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	જો (bt_help(win->w_buffer))
	{
	    win_close(win, FALSE);
	    પાછા;
	}
    }
}

#જો defined(FEAT_MULTI_LANG)
/*
 * In an argument search for a language specifiers in the form "@xx".
 * Changes the "@" to NUL if found, and returns a pointer to "xx".
 * Returns NULL if not found.
 */
    char_u *
check_help_lang(char_u *arg)
{
    પૂર્ણાંક len = (પૂર્ણાંક)STRLEN(arg);

    જો (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	પાછા arg + len - 2;
    }
    પાછા ખાલી;
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
    પૂર્ણાંક
help_heuristic(
    char_u	*matched_string,
    પૂર્ણાંક		offset,			// offset for match
    પૂર્ણાંક		wrong_case)		// no matching case
{
    પૂર્ણાંક		અંક_letters;
    char_u	*p;

    અંક_letters = 0;
    માટે (p = matched_string; *p; p++)
	જો (ASCII_ISALNUM(*p))
	    અંક_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    જો (ASCII_ISALNUM(matched_string[offset]) && offset > 0
				 && ASCII_ISALNUM(matched_string[offset - 1]))
	offset += 10000;
    નહીંતર જો (offset > 2)
	offset *= 200;
    જો (wrong_case)
	offset += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    જો (matched_string[0] == '+' && matched_string[1] != NUL)
	offset += 100;
    પાછા (પૂર્ણાંક)(100 * અંક_letters + STRLEN(matched_string) + offset);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    સ્થિર પૂર્ણાંક
help_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    અક્ષર    *p1;
    અક્ષર    *p2;
    પૂર્ણાંક	    cmp;

    p1 = *(અક્ષર **)s1 + strlen(*(અક્ષર **)s1) + 1;
    p2 = *(અક્ષર **)s2 + strlen(*(અક્ષર **)s2) + 1;

    // Compare by help heuristic number first.
    cmp = strcmp(p1, p2);
    જો (cmp != 0)
	પાછા cmp;

    // Compare by strings as tie-breaker when same heuristic number.
    પાછા strcmp(*(અક્ષર **)s1, *(અક્ષર **)s2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * The matches will be sorted with a "best" match algorithm.
 * When "keep_lang" is TRUE try keeping the language of the current buffer.
 */
    પૂર્ણાંક
find_help_tags(
    char_u	*arg,
    પૂર્ણાંક		*અંક_matches,
    char_u	***matches,
    પૂર્ણાંક		keep_lang)
{
    char_u	*s, *d;
    પૂર્ણાંક		i;
    // Specific tags that either have a specific replacement or won't go
    // through the generic rules.
    સ્થિર અક્ષર *(except_tbl[][2]) = {
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
	{ખાલી, ખાલી}
    };
    સ્થિર અક્ષર *(પદાવલિ_table[]) = {"!=?", "!~?", "<=?", "<?", "==?", "=~?",
				   ">=?", ">?", "is?", "isnot?"};
    પૂર્ણાંક flags;

    d = IObuff;		    // assume IObuff is long enough!
    d[0] = NUL;

    જો (STRNICMP(arg, "expr-", 5) == 0)
    {
	// When the string starting with "expr-" and containing '?' and matches
	// the table, it is taken literally (but ~ is escaped).  Otherwise '?'
	// is recognized as a wildcard.
	માટે (i = (પૂર્ણાંક)ARRAY_LENGTH(પદાવલિ_table); --i >= 0; )
	    જો (STRCMP(arg + 5, પદાવલિ_table[i]) == 0)
	    {
		પૂર્ણાંક si = 0, di = 0;

		માટે (;;)
		{
		    જો (arg[si] == '~')
			d[di++] = '\\';
		    d[di++] = arg[si];
		    જો (arg[si] == NUL)
			તોડો;
		    ++si;
		}
		તોડો;
	    }
    }
    નહીંતર
    {
	// Recognize a few exceptions to the rule.  Some strings that contain
	// '*'are changed to "star", otherwise '*' is recognized as a wildcard.
	માટે (i = 0; except_tbl[i][0] != ખાલી; ++i)
	    જો (STRCMP(arg, except_tbl[i][0]) == 0)
	    {
		STRCPY(d, except_tbl[i][1]);
		તોડો;
	    }
    }

    જો (d[0] == NUL)	// no match in table
    {
	// Replace "\S" with "/\\S", etc.  Otherwise every tag is matched.
	// Also replace "\%^" and "\%(", they match every tag too.
	// Also "\zs", "\z1", etc.
	// Also "\@<", "\@=", "\@<=", etc.
	// And also "\_$" and "\_^".
	જો (arg[0] == '\\'
		&& ((arg[1] != NUL && arg[2] == NUL)
		    || (vim_strchr((char_u *)"%_z@", arg[1]) != ખાલી
							   && arg[2] != NUL)))
	{
	    vim_snprintf((અક્ષર *)d, IOSIZE, "/\\\\%s", arg + 1);
	    // Check for "/\\_$", should be "/\\_\$"
	    જો (d[3] == '_' && d[4] == '$')
		STRCPY(d + 4, "\\$");
	}
	નહીંતર
	{
	  // Replace:
	  // "[:...:]" with "\[:...:]"
	  // "[++...]" with "\[++...]"
	  // "\{" with "\\{"		   -- matching "} \}"
	    જો ((arg[0] == '[' && (arg[1] == ':'
			 || (arg[1] == '+' && arg[2] == '+')))
		    || (arg[0] == '\\' && arg[1] == '{'))
	      *d++ = '\\';

	  // If tag starts with "('", skip the "(". Fixes CTRL-] on ('option'.
	  જો (*arg == '(' && arg[1] == '\'')
	      arg++;
	  માટે (s = arg; *s; ++s)
	  {
	    // Replace "|" with "bar" and '"' with "quote" to match the name of
	    // the tags for these commands.
	    // Replace "*" with ".*" and "?" with "." to match command line
	    // completion.
	    // Insert a backslash before '~', '$' and '.' to avoid their
	    // special meaning.
	    જો (d - IObuff > IOSIZE - 10)	// getting too long!?
		તોડો;
	    પસંદગી (*s)
	    {
		સ્થિતિ '|':   STRCPY(d, "bar");
			    d += 3;
			    ચાલુ;
		સ્થિતિ '"':   STRCPY(d, "quote");
			    d += 5;
			    ચાલુ;
		સ્થિતિ '*':   *d++ = '.';
			    તોડો;
		સ્થિતિ '?':   *d++ = '.';
			    ચાલુ;
		સ્થિતિ '$':
		સ્થિતિ '.':
		સ્થિતિ '~':   *d++ = '\\';
			    તોડો;
	    }

	    // Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	    // ":help i_^_CTRL-D" work.
	    // Insert '-' before and after "CTRL-X" when applicable.
	    જો (*s < ' ' || (*s == '^' && s[1] && (ASCII_ISALPHA(s[1])
			   || vim_strchr((char_u *)"?@[\\]^", s[1]) != ખાલી)))
	    {
		જો (d > IObuff && d[-1] != '_' && d[-1] != '\\')
		    *d++ = '_';		// prepend a '_' to make x_CTRL-x
		STRCPY(d, "CTRL-");
		d += 5;
		જો (*s < ' ')
		{
		    *d++ = *s + '@';
		    જો (d[-1] == '\\')
			*d++ = '\\';	// double a backslash
		}
		નહીંતર
		    *d++ = *++s;
		જો (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		// append a '_'
		ચાલુ;
	    }
	    નહીંતર જો (*s == '^')		// "^" or "CTRL-^" or "^_"
		*d++ = '\\';

	    // Insert a backslash before a backslash after a slash, for search
	    // pattern tags: "/\|" --> "/\\|".
	    નહીંતર જો (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    // "CTRL-\_" -> "CTRL-\\_" to avoid the special meaning of "\_" in
	    // "CTRL-\_CTRL-N"
	    જો (STRNICMP(s, "CTRL-\\_", 7) == 0)
	    {
		STRCPY(d, "CTRL-\\\\");
		d += 7;
		s += 6;
	    }

	    *d++ = *s;

	    // If tag contains "({" or "([", tag terminates at the "(".
	    // This is for help on functions, e.g.: abs({expr}).
	    જો (*s == '(' && (s[1] == '{' || s[1] =='['))
		તોડો;

	    // If tag starts with ', toss everything after a second '. Fixes
	    // CTRL-] on 'option'. (would include the trailing '.').
	    જો (*s == '\'' && s > arg && *arg == '\'')
		તોડો;
	    // Also '{' and '}'.
	    જો (*s == '}' && s > arg && *arg == '{')
		તોડો;
	  }
	  *d = NUL;

	  જો (*IObuff == '`')
	  {
	      જો (d > IObuff + 2 && d[-1] == '`')
	      {
		  // remove the backticks from `command`
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-2] = NUL;
	      }
	      નહીંતર જો (d > IObuff + 3 && d[-2] == '`' && d[-1] == ',')
	      {
		  // remove the backticks and comma from `command`,
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-3] = NUL;
	      }
	      નહીંતર જો (d > IObuff + 4 && d[-3] == '`'
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
    *અંક_matches = 0;
    flags = TAG_HELP | TAG_REGEXP | TAG_NAMES | TAG_VERBOSE | TAG_NO_TAGFUNC;
    જો (keep_lang)
	flags |= TAG_KEEP_LANG;
    જો (find_tags(IObuff, અંક_matches, matches, flags, (પૂર્ણાંક)MAXCOL, ખાલી) == OK
	    && *અંક_matches > 0)
    {
	// Sort the matches found on the heuristic number that is after the
	// tag name.
	qsort((શૂન્ય *)*matches, (size_t)*અંક_matches,
					      કદ(char_u *), help_compare);
	// Delete more than TAG_MANY to reduce the size of the listing.
	જ્યારે (*અંક_matches > TAG_MANY)
	    vim_free((*matches)[--*અંક_matches]);
    }
    પાછા OK;
}

#ifdef FEAT_MULTI_LANG
/*
 * Cleanup matches for help tags:
 * Remove "@ab" if the top of 'helplang' is "ab" and the language of the first
 * tag matches it.  Otherwise remove "@en" if "en" is the only language.
 */
    શૂન્ય
cleanup_help_tags(પૂર્ણાંક અંક_file, char_u **file)
{
    પૂર્ણાંક		i, j;
    પૂર્ણાંક		len;
    char_u	buf[4];
    char_u	*p = buf;

    જો (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    માટે (i = 0; i < અંક_file; ++i)
    {
	len = (પૂર્ણાંક)STRLEN(file[i]) - 3;
	જો (len <= 0)
	    ચાલુ;
	જો (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    માટે (j = 0; j < અંક_file; ++j)
		જો (j != i && (પૂર્ણાંક)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    તોડો;
	    જો (j == અંક_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    જો (*buf != NUL)
	માટે (i = 0; i < અંક_file; ++i)
	{
	    len = (પૂર્ણાંક)STRLEN(file[i]) - 3;
	    જો (len <= 0)
		ચાલુ;
	    જો (STRCMP(file[i] + len, buf) == 0)
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
    શૂન્ય
prepare_help_buffer(શૂન્ય)
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
    જો (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	set_string_option_direct((char_u *)"isk", -1, p, OPT_FREE|OPT_LOCAL, 0);
	check_buf_options(curbuf);
	(શૂન્ય)buf_init_chartab(curbuf, FALSE);
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
    શૂન્ય
fix_help_buffer(શૂન્ય)
{
    linenr_T	lnum;
    char_u	*લીટી;
    પૂર્ણાંક		in_example = FALSE;
    પૂર્ણાંક		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    પૂર્ણાંક		mustfree;

    // Set filetype to "help" if still needed.
    જો (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_lock;
	set_option_કિંમત_give_err((char_u *)"ft",
					      0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_lock;
    }

#ifdef FEAT_SYN_HL
    જો (!syntax_present(curwin))
#endif
    {
	માટે (lnum = 1; lnum <= curbuf->b_ml.ml_લીટી_ગણતરી; ++lnum)
	{
	    લીટી = ml_get_buf(curbuf, lnum, FALSE);
	    len = ml_get_buf_len(curbuf, lnum);
	    જો (in_example && len > 0 && !VIM_ISWHITE(લીટી[0]))
	    {
		// End of example: non-white or '<' in first column.
		જો (લીટી[0] == '<')
		{
		    // blank-out a '<' in the first column
		    લીટી = ml_get_buf(curbuf, lnum, TRUE);
		    લીટી[0] = ' ';
		}
		in_example = FALSE;
	    }
	    જો (!in_example && len > 0)
	    {
		જો (લીટી[len - 1] == '>' && (len == 1 || લીટી[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    લીટી = ml_get_buf(curbuf, lnum, TRUE);
		    લીટી[len - 1] = ' ';
		    in_example = TRUE;
		}
		નહીંતર જો (લીટી[len - 1] == '~')
		{
		    // blank-out a '~' at the end of line (header marker)
		    લીટી = ml_get_buf(curbuf, lnum, TRUE);
		    લીટી[len - 1] = ' ';
		}
	    }
	}
    }

    // In the "help.txt" and "help.abx" file, add the locally added help
    // files.  This uses the very first line in the help file.
    fname = gettail(curbuf->b_fname);
    જો (fnamecmp(fname, "help.txt") == 0
#ifdef FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#endif
	)
    {
	માટે (lnum = 1; lnum < curbuf->b_ml.ml_લીટી_ગણતરી; ++lnum)
	{
	    લીટી = ml_get_buf(curbuf, lnum, FALSE);
	    જો (strstr((અક્ષર *)લીટી, "*local-additions*") == ખાલી)
		ચાલુ;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    જ્યારે (*p != NUL)
	    {
		પૂર્ણાંક NameBufflen;

		NameBufflen = copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		જો (rt != ખાલી &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    પૂર્ણાંક		fcount;
		    char_u	**fnames;
		    FILE	*fd;
		    char_u	*s;
		    પૂર્ણાંક		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    જો (*NameBuff != NUL && !after_pathsep(NameBuff, NameBuff + NameBufflen))
		    {
			STRCPY(NameBuff + NameBufflen, PATHSEPSTR);
			NameBufflen += કદ(PATHSEP);
		    }
#ifdef FEAT_MULTI_LANG
		    STRCPY(NameBuff + NameBufflen, "doc/*.??[tx]");
#નહીંતર
		    STRCPY(NameBuff + NameBufflen, "doc/*.txt");
#endif
		    જો (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#ifdef FEAT_MULTI_LANG
			પૂર્ણાંક	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			માટે (i1 = 0; i1 < fcount; ++i1)
			{
			    f1 = fnames[i1];
			    t1 = gettail(f1);
			    e1 = vim_strrchr(t1, '.');
			    જો (e1 == ખાલી)
				ચાલુ;
			    જો (fnamecmp(e1, ".txt") != 0
					       && fnamecmp(e1, fname + 4) != 0)
			    {
				// Not .txt and not .abx, remove it.
				VIM_CLEAR(fnames[i1]);
				ચાલુ;
			    }

			    માટે (i2 = i1 + 1; i2 < fcount; ++i2)
			    {
				f2 = fnames[i2];
				જો (f2 == ખાલી)
				    ચાલુ;
				t2 = gettail(f2);
				e2 = vim_strrchr(t2, '.');
				જો (e2 == ખાલી)
				    ચાલુ;
				જો (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    ચાલુ;
				જો (fnamecmp(e1, ".txt") == 0
					       && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#endif
			માટે (fi = 0; fi < fcount; ++fi)
			{
			    જો (fnames[fi] == ખાલી)
				ચાલુ;
			    fd = mch_fopen((અક્ષર *)fnames[fi], "r");
			    જો (fd != ખાલી)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				જો (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != ખાલી)
				{
				    પૂર્ણાંક	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    જ્યારે (*s != NUL)
				    {
					જો (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					જો (*s >= 0x80 && this_utf != FALSE)
					{
					    પૂર્ણાંક	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    જો (l == 1)
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
				    જો (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    નહીંતર
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    ખાલી);
					જો (cp == ખાલી)
					    cp = IObuff;
				    }
				    convert_setup(&vc, ખાલી, ખાલી);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    જો (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				fclose(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		જો (mustfree)
		    vim_free(rt);
	    }
	    તોડો;
	}
    }
}

/*
 * ":exusage"
 */
    શૂન્ય
ex_exusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help ex-cmd-index");
}

/*
 * ":viusage"
 */
    શૂન્ય
ex_viusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help normal-index");
}

/*
 * Generate tags in one help directory.
 */
    સ્થિર શૂન્ય
helptags_one(
    char_u	*dir,		// doc directory
    char_u	*ext,		// suffix, ".txt", ".itx", ".frx", etc.
    char_u	*tagfname,	// "tags" for English, "tags-fr" for French.
    પૂર્ણાંક		add_help_tags,	// add "help-tags" tag
    પૂર્ણાંક		ignore_writeerr)    // ignore write error
{
    FILE	*fd_tags;
    FILE	*fd;
    garray_T	ga;
    પૂર્ણાંક		res;
    પૂર્ણાંક		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    પૂર્ણાંક		fi;
    char_u	*s;
    પૂર્ણાંક		i;
    char_u	*fname;
    પૂર્ણાંક		dirlen;
    પૂર્ણાંક		utf8 = MAYBE;
    પૂર્ણાંક		this_utf8;
    પૂર્ણાંક		firstline;
    પૂર્ણાંક		in_example;
    પૂર્ણાંક		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (પૂર્ણાંક)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    res = gen_expand_wildcards(1, &NameBuff, &filecount, &files,
							    EW_FILE|EW_SILENT);
    જો (res == FAIL || filecount == 0)
    {
	જો (!got_int)
	    semsg(_(e_no_match_str_1), NameBuff);
	જો (res != FAIL)
	    FreeWild(filecount, files);
	પાછા;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_tags = mch_fopen((અક્ષર *)NameBuff, "w");
    જો (fd_tags == ખાલી)
    {
	જો (!ignore_writeerr)
	    semsg(_(e_cannot_open_str_for_writing_1), NameBuff);
	FreeWild(filecount, files);
	પાછા;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, કદ(char_u *), 100);
    જો (add_help_tags || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	જો (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	નહીંતર
	{
	    s = alloc(18 + (unsigned)STRLEN(tagfname));
	    જો (s == ખાલી)
		got_int = TRUE;
	    નહીંતર
	    {
		sprintf((અક્ષર *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_data)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    માટે (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((અક્ષર *)files[fi], "r");
	જો (fd == ખાલી)
	{
	    semsg(_(e_unable_to_open_str_for_reading), files[fi]);
	    ચાલુ;
	}
	fname = files[fi] + dirlen + 1;

	in_example = FALSE;
	firstline = TRUE;
	જ્યારે (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    જો (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		માટે (s = IObuff; *s != NUL; ++s)
		    જો (*s >= 0x80)
		    {
			પૂર્ણાંક l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			જો (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    તોડો;
			}
			s += l - 1;
		    }
		જો (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		જો (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		નહીંતર જો (utf8 != this_utf8)
		{
		    semsg(_(e_mix_of_help_file_encodings_within_language_str), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    જો (in_example)
	    {
		// skip over example; a non-white in the first column ends it
		જો (vim_strchr((char_u *)" \t\n\r", IObuff[0]))
		    ચાલુ;
		in_example = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    જ્યારે (p1 != ખાલી)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		જો (p2 != ખાલી && p2 > p1 + 1)	// skip "*" and "**"
		{
		    માટે (s = p1 + 1; s < p2; ++s)
			જો (*s == ' ' || *s == '\t' || *s == '|')
			    તોડો;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    જો (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != ખાલી
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			જો (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    તોડો;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			જો (s == ખાલી)
			{
			    got_int = TRUE;
			    તોડો;
			}
			((char_u **)ga.ga_data)[ga.ga_len] = s;
			++ga.ga_len;
			sprintf((અક્ષર *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    size_t off = STRLEN(IObuff);
	    જો (off >= 2 && IObuff[off - 1] == '\n')
	    {
		off -= 2;
		જ્યારે (off > 0 && (ASCII_ISLOWER(IObuff[off])
						  || VIM_ISDIGIT(IObuff[off])))
		    off--;
		જો (IObuff[off] == '>' && (off == 0 || IObuff[off - 1] == ' '))
		    in_example = TRUE;
	    }
	    લીટી_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    જો (!got_int)
    {
	// Sort the tags.
	જો (ga.ga_data != ખાલી)
	    sort_strings((char_u **)ga.ga_data, ga.ga_len);

	// Check for duplicates.
	માટે (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_data)[i - 1];
	    p2 = ((char_u **)ga.ga_data)[i];
	    જ્યારે (*p1 == *p2)
	    {
		જો (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((અક્ષર *)NameBuff, MAXPATHL,
			    _(e_duplicate_tag_str_in_file_str_str),
				     ((char_u **)ga.ga_data)[i], dir, p2 + 1);
		    emsg((અક્ષર *)NameBuff);
		    *p2 = '\t';
		    તોડો;
		}
		++p1;
		++p2;
	    }
	}

	જો (utf8 == TRUE)
	    fprintf(fd_tags, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	માટે (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_data)[i];
	    જો (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		fputs((અક્ષર *)s, fd_tags);
	    નહીંતર
	    {
		fprintf(fd_tags, "%s\t/*", s);
		માટે (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    જો (*p1 == '\\' || *p1 == '/')
			putc('\\', fd_tags);
		    putc(*p1, fd_tags);
		}
		fprintf(fd_tags, "*\n");
	    }
	}
    }
    જો (mix)
	got_int = FALSE;    // continue with other languages

    માટે (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_data)[i]);
    ga_clear(&ga);
    fclose(fd_tags);	    // there is no check for an error...
}

/*
 * Generate tags in one help directory, taking care of translations.
 */
    સ્થિર શૂન્ય
do_helptags(char_u *dirname, પૂર્ણાંક add_help_tags, પૂર્ણાંક ignore_writeerr)
{
#ifdef FEAT_MULTI_LANG
    પૂર્ણાંક		len;
    પૂર્ણાંક		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    પૂર્ણાંક		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, dirname);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    જો (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_(e_no_match_str_1), NameBuff);
	પાછા;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    માટે (i = 0; i < filecount; ++i)
    {
	len = (પૂર્ણાંક)STRLEN(files[i]);
	જો (len <= 4)
	    ચાલુ;

	જો (STRICMP(files[i] + len - 4, ".txt") == 0)
	{
	    // ".txt" -> language "en"
	    lang[0] = 'e';
	    lang[1] = 'n';
	}
	નહીંતર જો (files[i][len - 4] == '.'
		&& ASCII_ISALPHA(files[i][len - 3])
		&& ASCII_ISALPHA(files[i][len - 2])
		&& TOLOWER_ASC(files[i][len - 1]) == 'x')
	{
	    // ".abx" -> language "ab"
	    lang[0] = TOLOWER_ASC(files[i][len - 3]);
	    lang[1] = TOLOWER_ASC(files[i][len - 2]);
	}
	નહીંતર
	    ચાલુ;

	// Did we find this language already?
	માટે (j = 0; j < ga.ga_len; j += 2)
	    જો (STRNCMP(lang, ((char_u *)ga.ga_data) + j, 2) == 0)
		તોડો;
	જો (j == ga.ga_len)
	{
	    // New language, add it.
	    જો (ga_grow(&ga, 2) == FAIL)
		તોડો;
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[0];
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[1];
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    માટે (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_data)[j];
	fname[6] = ((char_u *)ga.ga_data)[j + 1];
	જો (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	નહીંતર
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

#નહીંતર
    // No language support, just use "*.txt" and "tags".
    helptags_one(dirname, (char_u *)".txt", (char_u *)"tags", add_help_tags,
							    ignore_writeerr);
#endif
}

    સ્થિર શૂન્ય
helptags_cb(char_u *fname, શૂન્ય *cookie)
{
    do_helptags(fname, *(પૂર્ણાંક *)cookie, TRUE);
}

/*
 * ":helptags"
 */
    શૂન્ય
ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*dirname;
    પૂર્ણાંક		add_help_tags = FALSE;

    // Check for ":helptags ++t {dir}".
    જો (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	add_help_tags = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    જો (STRCMP(eap->arg, "ALL") == 0)
    {
	do_in_path(p_rtp, "", (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &add_help_tags);
    }
    નહીંતર
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	dirname = ExpandOne(&xpc, eap->arg, ખાલી,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	જો (dirname == ખાલી || !mch_isdir(dirname))
	    semsg(_(e_not_a_directory_str), eap->arg);
	નહીંતર
	    do_helptags(dirname, add_help_tags, FALSE);
	vim_free(dirname);
    }
}
