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

#ଅନ୍ତର୍ଭାଵ "vim.h"

/*
 * ":help": open a read-only window on a help file
 */
    ଶୂନ୍ୟ
ex_help(exarg_T *eap)
{
    char_u	*arg;
    char_u	*tag;
    FILE	*helpfd;	// file descriptor of help file
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_matches;
    char_u	**matches;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty_fnum = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		alt_fnum = 0;
    buf_T	*buf;
#ifdef FEAT_MULTI_LANG
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*lang;
#endif
#ifdef FEAT_FOLDING
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_ଚାବି_Typed = ଚାବି_Typed;
#endif

    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;

    ଯଦି (eap != ଖାଲି)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	ପାଇଁ (arg = eap->arg; *arg; ++arg)
	{
	    ଯଦି (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		ଭାଙ୍ଗ;
	    }
	}
	arg = eap->arg;

	ଯଦି (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_(e_dont_panic));
	    ଫେରନ୍ତୁ;
	}

	ଯଦି (eap->skip)	    // not executing commands
	    ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    ଯେତେବେଳେ (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#endif

    // When no argument given go to the index.
    ଯଦି (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = find_help_tags(arg, &ଅଙ୍କ_matches, &matches,
						 eap != ଖାଲି && eap->forceit);

    i = 0;
#ifdef FEAT_MULTI_LANG
    ଯଦି (n != FAIL && lang != ଖାଲି)
	// Find first item with the requested language.
	ପାଇଁ (i = 0; i < ଅଙ୍କ_matches; ++i)
	{
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(matches[i]);
	    ଯଦି (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		ଭାଙ୍ଗ;
	}
#endif
    ଯଦି (i >= ଅଙ୍କ_matches || n == FAIL)
    {
#ifdef FEAT_MULTI_LANG
	ଯଦି (lang != ଖାଲି)
	    semsg(_(e_sorry_no_str_help_for_str), lang, arg);
	ଅନ୍ୟଥା
#endif
	    semsg(_(e_sorry_no_help_for_str), arg);
	ଯଦି (n != FAIL)
	    FreeWild(ଅଙ୍କ_matches, matches);
	ଫେରନ୍ତୁ;
    }

    // The first match (in the requested language) is the best match.
    tag = vim_strsave(matches[i]);
    FreeWild(ଅଙ୍କ_matches, matches);

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    ଯଦି (!bt_help(curwin->w_buffer) || cmdmod.cmod_tab != 0)
    {
	ଯଦି (cmdmod.cmod_tab != 0)
	    wp = ଖାଲି;
	ଅନ୍ୟଥା
	    FOR_ALL_WINDOWS(wp)
		ଯଦି (bt_help(wp->w_buffer))
		    ଭାଙ୍ଗ;
	ଯଦି (wp != ଖାଲି && wp->w_buffer->b_nwindows > 0)
	    win_enter(wp, TRUE);
	ଅନ୍ୟଥା
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    ଯଦି ((helpfd = mch_fopen((ଅକ୍ଷର *)p_hf, READBIN)) == ଖାଲି)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		ଯାଅ erret;
	    }
	    fclose(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    ଯଦି (cmdmod.cmod_split == 0 && curwin->w_width != Columns
						  && curwin->w_width < 80)
		n |= p_sb ? WSP_BOT : WSP_ଶୀର୍ଷ;
	    ଯଦି (win_split(0, n) == FAIL)
		ଯାଅ erret;

	    ଯଦି (curwin->w_height < p_hh)
		win_setheight((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (ଶୂନ୍ୟ)do_ecmd(0, ଖାଲି, ଖାଲି, ଖାଲି, ECMD_LASTL,
			  ECMD_HIDE + ECMD_SET_HELP,
			  ଖାଲି);  // buffer is still open, don't store info
	    ଯଦି ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    empty_fnum = curbuf->b_fnum;
	}
    }

    ଯଦି (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#ifdef FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    ଚାବି_Typed = old_ଚାବି_Typed;
#endif

    ଯଦି (tag != ଖାଲି)
	do_tag(tag, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    ଯଦି (empty_fnum != 0 && curbuf->b_fnum != empty_fnum)
    {
	buf = buflist_findnr(empty_fnum);
	ଯଦି (buf != ଖାଲି && buf->b_nwindows == 0)
	    wipe_buffer(buf, TRUE);
    }

    // keep the previous alternate file
    ଯଦି (alt_fnum != 0 && curwin->w_alt_fnum == empty_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(tag);
}

/*
 * ":helpclose": Close one help window
 */
    ଶୂନ୍ୟ
ex_helpclose(exarg_T *eap UNUSED)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	ଯଦି (bt_help(win->w_buffer))
	{
	    win_close(win, FALSE);
	    ଫେରନ୍ତୁ;
	}
    }
}

#ଯଦି defined(FEAT_MULTI_LANG)
/*
 * In an argument search for a language specifiers in the form "@xx".
 * Changes the "@" to NUL if found, and returns a pointer to "xx".
 * Returns NULL if not found.
 */
    char_u *
check_help_lang(char_u *arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(arg);

    ଯଦି (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	ଫେରନ୍ତୁ arg + len - 2;
    }
    ଫେରନ୍ତୁ ଖାଲି;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
help_heuristic(
    char_u	*matched_string,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		offset,			// offset for match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wrong_case)		// no matching case
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_letters;
    char_u	*p;

    ଅଙ୍କ_letters = 0;
    ପାଇଁ (p = matched_string; *p; p++)
	ଯଦି (ASCII_ISALNUM(*p))
	    ଅଙ୍କ_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    ଯଦି (ASCII_ISALNUM(matched_string[offset]) && offset > 0
				 && ASCII_ISALNUM(matched_string[offset - 1]))
	offset += 10000;
    ଅନ୍ୟଥା ଯଦି (offset > 2)
	offset *= 200;
    ଯଦି (wrong_case)
	offset += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    ଯଦି (matched_string[0] == '+' && matched_string[1] != NUL)
	offset += 100;
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(100 * ଅଙ୍କ_letters + STRLEN(matched_string) + offset);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
help_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    ଅକ୍ଷର    *p1;
    ଅକ୍ଷର    *p2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cmp;

    p1 = *(ଅକ୍ଷର **)s1 + strlen(*(ଅକ୍ଷର **)s1) + 1;
    p2 = *(ଅକ୍ଷର **)s2 + strlen(*(ଅକ୍ଷର **)s2) + 1;

    // Compare by help heuristic number first.
    cmp = strcmp(p1, p2);
    ଯଦି (cmp != 0)
	ଫେରନ୍ତୁ cmp;

    // Compare by strings as tie-breaker when same heuristic number.
    ଫେରନ୍ତୁ strcmp(*(ଅକ୍ଷର **)s1, *(ଅକ୍ଷର **)s2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * The matches will be sorted with a "best" match algorithm.
 * When "keep_lang" is TRUE try keeping the language of the current buffer.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_help_tags(
    char_u	*arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_matches,
    char_u	***matches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_lang)
{
    char_u	*s, *d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    // Specific tags that either have a specific replacement or won't go
    // through the generic rules.
    ସ୍ଥିର ଅକ୍ଷର *(except_tbl[][2]) = {
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
	{ଖାଲି, ଖାଲି}
    };
    ସ୍ଥିର ଅକ୍ଷର *(ପଦସମଷ୍ଟି_table[]) = {"!=?", "!~?", "<=?", "<?", "==?", "=~?",
				   ">=?", ">?", "is?", "isnot?"};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;

    d = IObuff;		    // assume IObuff is long enough!
    d[0] = NUL;

    ଯଦି (STRNICMP(arg, "expr-", 5) == 0)
    {
	// When the string starting with "expr-" and containing '?' and matches
	// the table, it is taken literally (but ~ is escaped).  Otherwise '?'
	// is recognized as a wildcard.
	ପାଇଁ (i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(ପଦସମଷ୍ଟି_table); --i >= 0; )
	    ଯଦି (STRCMP(arg + 5, ପଦସମଷ୍ଟି_table[i]) == 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା si = 0, di = 0;

		ପାଇଁ (;;)
		{
		    ଯଦି (arg[si] == '~')
			d[di++] = '\\';
		    d[di++] = arg[si];
		    ଯଦି (arg[si] == NUL)
			ଭାଙ୍ଗ;
		    ++si;
		}
		ଭାଙ୍ଗ;
	    }
    }
    ଅନ୍ୟଥା
    {
	// Recognize a few exceptions to the rule.  Some strings that contain
	// '*'are changed to "star", otherwise '*' is recognized as a wildcard.
	ପାଇଁ (i = 0; except_tbl[i][0] != ଖାଲି; ++i)
	    ଯଦି (STRCMP(arg, except_tbl[i][0]) == 0)
	    {
		STRCPY(d, except_tbl[i][1]);
		ଭାଙ୍ଗ;
	    }
    }

    ଯଦି (d[0] == NUL)	// no match in table
    {
	// Replace "\S" with "/\\S", etc.  Otherwise every tag is matched.
	// Also replace "\%^" and "\%(", they match every tag too.
	// Also "\zs", "\z1", etc.
	// Also "\@<", "\@=", "\@<=", etc.
	// And also "\_$" and "\_^".
	ଯଦି (arg[0] == '\\'
		&& ((arg[1] != NUL && arg[2] == NUL)
		    || (vim_strchr((char_u *)"%_z@", arg[1]) != ଖାଲି
							   && arg[2] != NUL)))
	{
	    vim_snprintf((ଅକ୍ଷର *)d, IOSIZE, "/\\\\%s", arg + 1);
	    // Check for "/\\_$", should be "/\\_\$"
	    ଯଦି (d[3] == '_' && d[4] == '$')
		STRCPY(d + 4, "\\$");
	}
	ଅନ୍ୟଥା
	{
	  // Replace:
	  // "[:...:]" with "\[:...:]"
	  // "[++...]" with "\[++...]"
	  // "\{" with "\\{"		   -- matching "} \}"
	    ଯଦି ((arg[0] == '[' && (arg[1] == ':'
			 || (arg[1] == '+' && arg[2] == '+')))
		    || (arg[0] == '\\' && arg[1] == '{'))
	      *d++ = '\\';

	  // If tag starts with "('", skip the "(". Fixes CTRL-] on ('option'.
	  ଯଦି (*arg == '(' && arg[1] == '\'')
	      arg++;
	  ପାଇଁ (s = arg; *s; ++s)
	  {
	    // Replace "|" with "bar" and '"' with "quote" to match the name of
	    // the tags for these commands.
	    // Replace "*" with ".*" and "?" with "." to match command line
	    // completion.
	    // Insert a backslash before '~', '$' and '.' to avoid their
	    // special meaning.
	    ଯଦି (d - IObuff > IOSIZE - 10)	// getting too long!?
		ଭାଙ୍ଗ;
	    ବାଛ (*s)
	    {
		ସ୍ଥିତି '|':   STRCPY(d, "bar");
			    d += 3;
			    ଜାରି;
		ସ୍ଥିତି '"':   STRCPY(d, "quote");
			    d += 5;
			    ଜାରି;
		ସ୍ଥିତି '*':   *d++ = '.';
			    ଭାଙ୍ଗ;
		ସ୍ଥିତି '?':   *d++ = '.';
			    ଜାରି;
		ସ୍ଥିତି '$':
		ସ୍ଥିତି '.':
		ସ୍ଥିତି '~':   *d++ = '\\';
			    ଭାଙ୍ଗ;
	    }

	    // Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	    // ":help i_^_CTRL-D" work.
	    // Insert '-' before and after "CTRL-X" when applicable.
	    ଯଦି (*s < ' ' || (*s == '^' && s[1] && (ASCII_ISALPHA(s[1])
			   || vim_strchr((char_u *)"?@[\\]^", s[1]) != ଖାଲି)))
	    {
		ଯଦି (d > IObuff && d[-1] != '_' && d[-1] != '\\')
		    *d++ = '_';		// prepend a '_' to make x_CTRL-x
		STRCPY(d, "CTRL-");
		d += 5;
		ଯଦି (*s < ' ')
		{
		    *d++ = *s + '@';
		    ଯଦି (d[-1] == '\\')
			*d++ = '\\';	// double a backslash
		}
		ଅନ୍ୟଥା
		    *d++ = *++s;
		ଯଦି (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		// append a '_'
		ଜାରି;
	    }
	    ଅନ୍ୟଥା ଯଦି (*s == '^')		// "^" or "CTRL-^" or "^_"
		*d++ = '\\';

	    // Insert a backslash before a backslash after a slash, for search
	    // pattern tags: "/\|" --> "/\\|".
	    ଅନ୍ୟଥା ଯଦି (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    // "CTRL-\_" -> "CTRL-\\_" to avoid the special meaning of "\_" in
	    // "CTRL-\_CTRL-N"
	    ଯଦି (STRNICMP(s, "CTRL-\\_", 7) == 0)
	    {
		STRCPY(d, "CTRL-\\\\");
		d += 7;
		s += 6;
	    }

	    *d++ = *s;

	    // If tag contains "({" or "([", tag terminates at the "(".
	    // This is for help on functions, e.g.: abs({expr}).
	    ଯଦି (*s == '(' && (s[1] == '{' || s[1] =='['))
		ଭାଙ୍ଗ;

	    // If tag starts with ', toss everything after a second '. Fixes
	    // CTRL-] on 'option'. (would include the trailing '.').
	    ଯଦି (*s == '\'' && s > arg && *arg == '\'')
		ଭାଙ୍ଗ;
	    // Also '{' and '}'.
	    ଯଦି (*s == '}' && s > arg && *arg == '{')
		ଭାଙ୍ଗ;
	  }
	  *d = NUL;

	  ଯଦି (*IObuff == '`')
	  {
	      ଯଦି (d > IObuff + 2 && d[-1] == '`')
	      {
		  // remove the backticks from `command`
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-2] = NUL;
	      }
	      ଅନ୍ୟଥା ଯଦି (d > IObuff + 3 && d[-2] == '`' && d[-1] == ',')
	      {
		  // remove the backticks and comma from `command`,
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-3] = NUL;
	      }
	      ଅନ୍ୟଥା ଯଦି (d > IObuff + 4 && d[-3] == '`'
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
    *ଅଙ୍କ_matches = 0;
    flags = TAG_HELP | TAG_REGEXP | TAG_NAMES | TAG_VERBOSE | TAG_NO_TAGFUNC;
    ଯଦି (keep_lang)
	flags |= TAG_KEEP_LANG;
    ଯଦି (find_tags(IObuff, ଅଙ୍କ_matches, matches, flags, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)MAXCOL, ଖାଲି) == OK
	    && *ଅଙ୍କ_matches > 0)
    {
	// Sort the matches found on the heuristic number that is after the
	// tag name.
	qsort((ଶୂନ୍ୟ *)*matches, (size_t)*ଅଙ୍କ_matches,
					      ଆକାର(char_u *), help_compare);
	// Delete more than TAG_MANY to reduce the size of the listing.
	ଯେତେବେଳେ (*ଅଙ୍କ_matches > TAG_MANY)
	    vim_free((*matches)[--*ଅଙ୍କ_matches]);
    }
    ଫେରନ୍ତୁ OK;
}

#ifdef FEAT_MULTI_LANG
/*
 * Cleanup matches for help tags:
 * Remove "@ab" if the top of 'helplang' is "ab" and the language of the first
 * tag matches it.  Otherwise remove "@en" if "en" is the only language.
 */
    ଶୂନ୍ୟ
cleanup_help_tags(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_file, char_u **file)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	buf[4];
    char_u	*p = buf;

    ଯଦି (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    ପାଇଁ (i = 0; i < ଅଙ୍କ_file; ++i)
    {
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(file[i]) - 3;
	ଯଦି (len <= 0)
	    ଜାରି;
	ଯଦି (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    ପାଇଁ (j = 0; j < ଅଙ୍କ_file; ++j)
		ଯଦି (j != i && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    ଭାଙ୍ଗ;
	    ଯଦି (j == ଅଙ୍କ_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    ଯଦି (*buf != NUL)
	ପାଇଁ (i = 0; i < ଅଙ୍କ_file; ++i)
	{
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(file[i]) - 3;
	    ଯଦି (len <= 0)
		ଜାରି;
	    ଯଦି (STRCMP(file[i] + len, buf) == 0)
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
    ଶୂନ୍ୟ
prepare_help_buffer(ଶୂନ୍ୟ)
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
    ଯଦି (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	set_string_option_direct((char_u *)"isk", -1, p, OPT_FREE|OPT_LOCAL, 0);
	check_buf_options(curbuf);
	(ଶୂନ୍ୟ)buf_init_chartab(curbuf, FALSE);
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
    ଶୂନ୍ୟ
fix_help_buffer(ଶୂନ୍ୟ)
{
    linenr_T	lnum;
    char_u	*ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_example = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mustfree;

    // Set filetype to "help" if still needed.
    ଯଦି (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_lock;
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"ft",
					      0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_lock;
    }

#ifdef FEAT_SYN_HL
    ଯଦି (!syntax_present(curwin))
#endif
    {
	ପାଇଁ (lnum = 1; lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା; ++lnum)
	{
	    ଧାଡ଼ି = ml_get_buf(curbuf, lnum, FALSE);
	    len = ml_get_buf_len(curbuf, lnum);
	    ଯଦି (in_example && len > 0 && !VIM_ISWHITE(ଧାଡ଼ି[0]))
	    {
		// End of example: non-white or '<' in first column.
		ଯଦି (ଧାଡ଼ି[0] == '<')
		{
		    // blank-out a '<' in the first column
		    ଧାଡ଼ି = ml_get_buf(curbuf, lnum, TRUE);
		    ଧାଡ଼ି[0] = ' ';
		}
		in_example = FALSE;
	    }
	    ଯଦି (!in_example && len > 0)
	    {
		ଯଦି (ଧାଡ଼ି[len - 1] == '>' && (len == 1 || ଧାଡ଼ି[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    ଧାଡ଼ି = ml_get_buf(curbuf, lnum, TRUE);
		    ଧାଡ଼ି[len - 1] = ' ';
		    in_example = TRUE;
		}
		ଅନ୍ୟଥା ଯଦି (ଧାଡ଼ି[len - 1] == '~')
		{
		    // blank-out a '~' at the end of line (header marker)
		    ଧାଡ଼ି = ml_get_buf(curbuf, lnum, TRUE);
		    ଧାଡ଼ି[len - 1] = ' ';
		}
	    }
	}
    }

    // In the "help.txt" and "help.abx" file, add the locally added help
    // files.  This uses the very first line in the help file.
    fname = gettail(curbuf->b_fname);
    ଯଦି (fnamecmp(fname, "help.txt") == 0
#ifdef FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#endif
	)
    {
	ପାଇଁ (lnum = 1; lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା; ++lnum)
	{
	    ଧାଡ଼ି = ml_get_buf(curbuf, lnum, FALSE);
	    ଯଦି (strstr((ଅକ୍ଷର *)ଧାଡ଼ି, "*local-additions*") == ଖାଲି)
		ଜାରି;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    ଯେତେବେଳେ (*p != NUL)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା NameBufflen;

		NameBufflen = copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		ଯଦି (rt != ଖାଲି &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fcount;
		    char_u	**fnames;
		    FILE	*fd;
		    char_u	*s;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    ଯଦି (*NameBuff != NUL && !after_pathsep(NameBuff, NameBuff + NameBufflen))
		    {
			STRCPY(NameBuff + NameBufflen, PATHSEPSTR);
			NameBufflen += ଆକାର(PATHSEP);
		    }
#ifdef FEAT_MULTI_LANG
		    STRCPY(NameBuff + NameBufflen, "doc/*.??[tx]");
#ଅନ୍ୟଥା
		    STRCPY(NameBuff + NameBufflen, "doc/*.txt");
#endif
		    ଯଦି (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#ifdef FEAT_MULTI_LANG
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			ପାଇଁ (i1 = 0; i1 < fcount; ++i1)
			{
			    f1 = fnames[i1];
			    t1 = gettail(f1);
			    e1 = vim_strrchr(t1, '.');
			    ଯଦି (e1 == ଖାଲି)
				ଜାରି;
			    ଯଦି (fnamecmp(e1, ".txt") != 0
					       && fnamecmp(e1, fname + 4) != 0)
			    {
				// Not .txt and not .abx, remove it.
				VIM_CLEAR(fnames[i1]);
				ଜାରି;
			    }

			    ପାଇଁ (i2 = i1 + 1; i2 < fcount; ++i2)
			    {
				f2 = fnames[i2];
				ଯଦି (f2 == ଖାଲି)
				    ଜାରି;
				t2 = gettail(f2);
				e2 = vim_strrchr(t2, '.');
				ଯଦି (e2 == ଖାଲି)
				    ଜାରି;
				ଯଦି (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    ଜାରି;
				ଯଦି (fnamecmp(e1, ".txt") == 0
					       && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#endif
			ପାଇଁ (fi = 0; fi < fcount; ++fi)
			{
			    ଯଦି (fnames[fi] == ଖାଲି)
				ଜାରି;
			    fd = mch_fopen((ଅକ୍ଷର *)fnames[fi], "r");
			    ଯଦି (fd != ଖାଲି)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				ଯଦି (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != ଖାଲି)
				{
				    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    ଯେତେବେଳେ (*s != NUL)
				    {
					ଯଦି (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					ଯଦି (*s >= 0x80 && this_utf != FALSE)
					{
					    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    ଯଦି (l == 1)
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
				    ଯଦି (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    ଅନ୍ୟଥା
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    ଖାଲି);
					ଯଦି (cp == ଖାଲି)
					    cp = IObuff;
				    }
				    convert_setup(&vc, ଖାଲି, ଖାଲି);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    ଯଦି (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				fclose(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		ଯଦି (mustfree)
		    vim_free(rt);
	    }
	    ଭାଙ୍ଗ;
	}
    }
}

/*
 * ":exusage"
 */
    ଶୂନ୍ୟ
ex_exusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help ex-cmd-index");
}

/*
 * ":viusage"
 */
    ଶୂନ୍ୟ
ex_viusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help normal-index");
}

/*
 * Generate tags in one help directory.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
helptags_one(
    char_u	*dir,		// doc directory
    char_u	*ext,		// suffix, ".txt", ".itx", ".frx", etc.
    char_u	*tagfname,	// "tags" for English, "tags-fr" for French.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_help_tags,	// add "help-tags" tag
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ignore_writeerr)    // ignore write error
{
    FILE	*fd_tags;
    FILE	*fd;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fi;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*fname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dirlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		utf8 = MAYBE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		this_utf8;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		firstline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_example;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    res = gen_expand_wildcards(1, &NameBuff, &filecount, &files,
							    EW_FILE|EW_SILENT);
    ଯଦି (res == FAIL || filecount == 0)
    {
	ଯଦି (!got_int)
	    semsg(_(e_no_match_str_1), NameBuff);
	ଯଦି (res != FAIL)
	    FreeWild(filecount, files);
	ଫେରନ୍ତୁ;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_tags = mch_fopen((ଅକ୍ଷର *)NameBuff, "w");
    ଯଦି (fd_tags == ଖାଲି)
    {
	ଯଦି (!ignore_writeerr)
	    semsg(_(e_cannot_open_str_for_writing_1), NameBuff);
	FreeWild(filecount, files);
	ଫେରନ୍ତୁ;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, ଆକାର(char_u *), 100);
    ଯଦି (add_help_tags || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	ଯଦି (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	ଅନ୍ୟଥା
	{
	    s = alloc(18 + (unsigned)STRLEN(tagfname));
	    ଯଦି (s == ଖାଲି)
		got_int = TRUE;
	    ଅନ୍ୟଥା
	    {
		sprintf((ଅକ୍ଷର *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_data)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    ପାଇଁ (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((ଅକ୍ଷର *)files[fi], "r");
	ଯଦି (fd == ଖାଲି)
	{
	    semsg(_(e_unable_to_open_str_for_reading), files[fi]);
	    ଜାରି;
	}
	fname = files[fi] + dirlen + 1;

	in_example = FALSE;
	firstline = TRUE;
	ଯେତେବେଳେ (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    ଯଦି (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		ପାଇଁ (s = IObuff; *s != NUL; ++s)
		    ଯଦି (*s >= 0x80)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			ଯଦି (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    ଭାଙ୍ଗ;
			}
			s += l - 1;
		    }
		ଯଦି (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		ଯଦି (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		ଅନ୍ୟଥା ଯଦି (utf8 != this_utf8)
		{
		    semsg(_(e_mix_of_help_file_encodings_within_language_str), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    ଯଦି (in_example)
	    {
		// skip over example; a non-white in the first column ends it
		ଯଦି (vim_strchr((char_u *)" \t\n\r", IObuff[0]))
		    ଜାରି;
		in_example = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    ଯେତେବେଳେ (p1 != ଖାଲି)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		ଯଦି (p2 != ଖାଲି && p2 > p1 + 1)	// skip "*" and "**"
		{
		    ପାଇଁ (s = p1 + 1; s < p2; ++s)
			ଯଦି (*s == ' ' || *s == '\t' || *s == '|')
			    ଭାଙ୍ଗ;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    ଯଦି (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != ଖାଲି
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			ଯଦି (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    ଭାଙ୍ଗ;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			ଯଦି (s == ଖାଲି)
			{
			    got_int = TRUE;
			    ଭାଙ୍ଗ;
			}
			((char_u **)ga.ga_data)[ga.ga_len] = s;
			++ga.ga_len;
			sprintf((ଅକ୍ଷର *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    size_t off = STRLEN(IObuff);
	    ଯଦି (off >= 2 && IObuff[off - 1] == '\n')
	    {
		off -= 2;
		ଯେତେବେଳେ (off > 0 && (ASCII_ISLOWER(IObuff[off])
						  || VIM_ISDIGIT(IObuff[off])))
		    off--;
		ଯଦି (IObuff[off] == '>' && (off == 0 || IObuff[off - 1] == ' '))
		    in_example = TRUE;
	    }
	    ଧାଡ଼ି_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    ଯଦି (!got_int)
    {
	// Sort the tags.
	ଯଦି (ga.ga_data != ଖାଲି)
	    sort_strings((char_u **)ga.ga_data, ga.ga_len);

	// Check for duplicates.
	ପାଇଁ (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_data)[i - 1];
	    p2 = ((char_u **)ga.ga_data)[i];
	    ଯେତେବେଳେ (*p1 == *p2)
	    {
		ଯଦି (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((ଅକ୍ଷର *)NameBuff, MAXPATHL,
			    _(e_duplicate_tag_str_in_file_str_str),
				     ((char_u **)ga.ga_data)[i], dir, p2 + 1);
		    emsg((ଅକ୍ଷର *)NameBuff);
		    *p2 = '\t';
		    ଭାଙ୍ଗ;
		}
		++p1;
		++p2;
	    }
	}

	ଯଦି (utf8 == TRUE)
	    fprintf(fd_tags, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	ପାଇଁ (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_data)[i];
	    ଯଦି (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		fputs((ଅକ୍ଷର *)s, fd_tags);
	    ଅନ୍ୟଥା
	    {
		fprintf(fd_tags, "%s\t/*", s);
		ପାଇଁ (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    ଯଦି (*p1 == '\\' || *p1 == '/')
			putc('\\', fd_tags);
		    putc(*p1, fd_tags);
		}
		fprintf(fd_tags, "*\n");
	    }
	}
    }
    ଯଦି (mix)
	got_int = FALSE;    // continue with other languages

    ପାଇଁ (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_data)[i]);
    ga_clear(&ga);
    fclose(fd_tags);	    // there is no check for an error...
}

/*
 * Generate tags in one help directory, taking care of translations.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_helptags(char_u *dirname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_help_tags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_writeerr)
{
#ifdef FEAT_MULTI_LANG
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, dirname);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    ଯଦି (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_(e_no_match_str_1), NameBuff);
	ଫେରନ୍ତୁ;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    ପାଇଁ (i = 0; i < filecount; ++i)
    {
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(files[i]);
	ଯଦି (len <= 4)
	    ଜାରି;

	ଯଦି (STRICMP(files[i] + len - 4, ".txt") == 0)
	{
	    // ".txt" -> language "en"
	    lang[0] = 'e';
	    lang[1] = 'n';
	}
	ଅନ୍ୟଥା ଯଦି (files[i][len - 4] == '.'
		&& ASCII_ISALPHA(files[i][len - 3])
		&& ASCII_ISALPHA(files[i][len - 2])
		&& TOLOWER_ASC(files[i][len - 1]) == 'x')
	{
	    // ".abx" -> language "ab"
	    lang[0] = TOLOWER_ASC(files[i][len - 3]);
	    lang[1] = TOLOWER_ASC(files[i][len - 2]);
	}
	ଅନ୍ୟଥା
	    ଜାରି;

	// Did we find this language already?
	ପାଇଁ (j = 0; j < ga.ga_len; j += 2)
	    ଯଦି (STRNCMP(lang, ((char_u *)ga.ga_data) + j, 2) == 0)
		ଭାଙ୍ଗ;
	ଯଦି (j == ga.ga_len)
	{
	    // New language, add it.
	    ଯଦି (ga_grow(&ga, 2) == FAIL)
		ଭାଙ୍ଗ;
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[0];
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[1];
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    ପାଇଁ (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_data)[j];
	fname[6] = ((char_u *)ga.ga_data)[j + 1];
	ଯଦି (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	ଅନ୍ୟଥା
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

#ଅନ୍ୟଥା
    // No language support, just use "*.txt" and "tags".
    helptags_one(dirname, (char_u *)".txt", (char_u *)"tags", add_help_tags,
							    ignore_writeerr);
#endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
helptags_cb(char_u *fname, ଶୂନ୍ୟ *cookie)
{
    do_helptags(fname, *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)cookie, TRUE);
}

/*
 * ":helptags"
 */
    ଶୂନ୍ୟ
ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*dirname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_help_tags = FALSE;

    // Check for ":helptags ++t {dir}".
    ଯଦି (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	add_help_tags = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    ଯଦି (STRCMP(eap->arg, "ALL") == 0)
    {
	do_in_path(p_rtp, "", (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &add_help_tags);
    }
    ଅନ୍ୟଥା
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	dirname = ExpandOne(&xpc, eap->arg, ଖାଲି,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	ଯଦି (dirname == ଖାଲି || !mch_isdir(dirname))
	    semsg(_(e_not_a_directory_str), eap->arg);
	ଅନ୍ୟଥା
	    do_helptags(dirname, add_help_tags, FALSE);
	vim_free(dirname);
    }
}
