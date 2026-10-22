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

#অন্তর্ভাব "vim.h"

/*
 * ":help": open a read-only window on a help file
 */
    শূন্য
ex_help(exarg_T *eap)
{
    char_u	*arg;
    char_u	*tag;
    FILE	*helpfd;	// file descriptor of help file
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		i;
    win_T	*wp;
    পূর্ণাঙ্ক		অঙ্ক_matches;
    char_u	**matches;
    char_u	*p;
    পূর্ণাঙ্ক		empty_fnum = 0;
    পূর্ণাঙ্ক		alt_fnum = 0;
    buf_T	*buf;
#ifdef FEAT_MULTI_LANG
    পূর্ণাঙ্ক		len;
    char_u	*lang;
#endif
#ifdef FEAT_FOLDING
    পূর্ণাঙ্ক		old_চাবি_Typed = চাবি_Typed;
#endif

    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	প্রত্যাগম;

    যদি (eap != রিক্ত)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	প্রতি (arg = eap->arg; *arg; ++arg)
	{
	    যদি (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		ভঙ্গ;
	    }
	}
	arg = eap->arg;

	যদি (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_(e_dont_panic));
	    প্রত্যাগম;
	}

	যদি (eap->skip)	    // not executing commands
	    প্রত্যাগম;
    }
    অন্যথা
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    যাবত্ (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#endif

    // When no argument given go to the index.
    যদি (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = find_help_tags(arg, &অঙ্ক_matches, &matches,
						 eap != রিক্ত && eap->forceit);

    i = 0;
#ifdef FEAT_MULTI_LANG
    যদি (n != FAIL && lang != রিক্ত)
	// Find first item with the requested language.
	প্রতি (i = 0; i < অঙ্ক_matches; ++i)
	{
	    len = (পূর্ণাঙ্ক)STRLEN(matches[i]);
	    যদি (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		ভঙ্গ;
	}
#endif
    যদি (i >= অঙ্ক_matches || n == FAIL)
    {
#ifdef FEAT_MULTI_LANG
	যদি (lang != রিক্ত)
	    semsg(_(e_sorry_no_str_help_for_str), lang, arg);
	অন্যথা
#endif
	    semsg(_(e_sorry_no_help_for_str), arg);
	যদি (n != FAIL)
	    FreeWild(অঙ্ক_matches, matches);
	প্রত্যাগম;
    }

    // The first match (in the requested language) is the best match.
    tag = vim_strsave(matches[i]);
    FreeWild(অঙ্ক_matches, matches);

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    যদি (!bt_help(curwin->w_buffer) || cmdmod.cmod_tab != 0)
    {
	যদি (cmdmod.cmod_tab != 0)
	    wp = রিক্ত;
	অন্যথা
	    FOR_ALL_WINDOWS(wp)
		যদি (bt_help(wp->w_buffer))
		    ভঙ্গ;
	যদি (wp != রিক্ত && wp->w_buffer->b_nwindows > 0)
	    win_enter(wp, TRUE);
	অন্যথা
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    যদি ((helpfd = mch_fopen((বর্ণ *)p_hf, READBIN)) == রিক্ত)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		গচ্ছ erret;
	    }
	    fclose(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    যদি (cmdmod.cmod_split == 0 && curwin->w_width != Columns
						  && curwin->w_width < 80)
		n |= p_sb ? WSP_BOT : WSP_শীর্ষ;
	    যদি (win_split(0, n) == FAIL)
		গচ্ছ erret;

	    যদি (curwin->w_height < p_hh)
		win_setheight((পূর্ণাঙ্ক)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (শূন্য)do_ecmd(0, রিক্ত, রিক্ত, রিক্ত, ECMD_LASTL,
			  ECMD_HIDE + ECMD_SET_HELP,
			  রিক্ত);  // buffer is still open, don't store info
	    যদি ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    empty_fnum = curbuf->b_fnum;
	}
    }

    যদি (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#ifdef FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    চাবি_Typed = old_চাবি_Typed;
#endif

    যদি (tag != রিক্ত)
	do_tag(tag, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    যদি (empty_fnum != 0 && curbuf->b_fnum != empty_fnum)
    {
	buf = buflist_findnr(empty_fnum);
	যদি (buf != রিক্ত && buf->b_nwindows == 0)
	    wipe_buffer(buf, TRUE);
    }

    // keep the previous alternate file
    যদি (alt_fnum != 0 && curwin->w_alt_fnum == empty_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(tag);
}

/*
 * ":helpclose": Close one help window
 */
    শূন্য
ex_helpclose(exarg_T *eap UNUSED)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	যদি (bt_help(win->w_buffer))
	{
	    win_close(win, FALSE);
	    প্রত্যাগম;
	}
    }
}

#যদি defined(FEAT_MULTI_LANG)
/*
 * In an argument search for a language specifiers in the form "@xx".
 * Changes the "@" to NUL if found, and returns a pointer to "xx".
 * Returns NULL if not found.
 */
    char_u *
check_help_lang(char_u *arg)
{
    পূর্ণাঙ্ক len = (পূর্ণাঙ্ক)STRLEN(arg);

    যদি (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	প্রত্যাগম arg + len - 2;
    }
    প্রত্যাগম রিক্ত;
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
    পূর্ণাঙ্ক
help_heuristic(
    char_u	*matched_string,
    পূর্ণাঙ্ক		offset,			// offset for match
    পূর্ণাঙ্ক		wrong_case)		// no matching case
{
    পূর্ণাঙ্ক		অঙ্ক_letters;
    char_u	*p;

    অঙ্ক_letters = 0;
    প্রতি (p = matched_string; *p; p++)
	যদি (ASCII_ISALNUM(*p))
	    অঙ্ক_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    যদি (ASCII_ISALNUM(matched_string[offset]) && offset > 0
				 && ASCII_ISALNUM(matched_string[offset - 1]))
	offset += 10000;
    অন্যথা যদি (offset > 2)
	offset *= 200;
    যদি (wrong_case)
	offset += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    যদি (matched_string[0] == '+' && matched_string[1] != NUL)
	offset += 100;
    প্রত্যাগম (পূর্ণাঙ্ক)(100 * অঙ্ক_letters + STRLEN(matched_string) + offset);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    স্থির পূর্ণাঙ্ক
help_compare(ধ্রুব শূন্য *s1, ধ্রুব শূন্য *s2)
{
    বর্ণ    *p1;
    বর্ণ    *p2;
    পূর্ণাঙ্ক	    cmp;

    p1 = *(বর্ণ **)s1 + strlen(*(বর্ণ **)s1) + 1;
    p2 = *(বর্ণ **)s2 + strlen(*(বর্ণ **)s2) + 1;

    // Compare by help heuristic number first.
    cmp = strcmp(p1, p2);
    যদি (cmp != 0)
	প্রত্যাগম cmp;

    // Compare by strings as tie-breaker when same heuristic number.
    প্রত্যাগম strcmp(*(বর্ণ **)s1, *(বর্ণ **)s2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * The matches will be sorted with a "best" match algorithm.
 * When "keep_lang" is TRUE try keeping the language of the current buffer.
 */
    পূর্ণাঙ্ক
find_help_tags(
    char_u	*arg,
    পূর্ণাঙ্ক		*অঙ্ক_matches,
    char_u	***matches,
    পূর্ণাঙ্ক		keep_lang)
{
    char_u	*s, *d;
    পূর্ণাঙ্ক		i;
    // Specific tags that either have a specific replacement or won't go
    // through the generic rules.
    স্থির বর্ণ *(except_tbl[][2]) = {
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
	{রিক্ত, রিক্ত}
    };
    স্থির বর্ণ *(রাশিমালা_table[]) = {"!=?", "!~?", "<=?", "<?", "==?", "=~?",
				   ">=?", ">?", "is?", "isnot?"};
    পূর্ণাঙ্ক flags;

    d = IObuff;		    // assume IObuff is long enough!
    d[0] = NUL;

    যদি (STRNICMP(arg, "expr-", 5) == 0)
    {
	// When the string starting with "expr-" and containing '?' and matches
	// the table, it is taken literally (but ~ is escaped).  Otherwise '?'
	// is recognized as a wildcard.
	প্রতি (i = (পূর্ণাঙ্ক)ARRAY_LENGTH(রাশিমালা_table); --i >= 0; )
	    যদি (STRCMP(arg + 5, রাশিমালা_table[i]) == 0)
	    {
		পূর্ণাঙ্ক si = 0, di = 0;

		প্রতি (;;)
		{
		    যদি (arg[si] == '~')
			d[di++] = '\\';
		    d[di++] = arg[si];
		    যদি (arg[si] == NUL)
			ভঙ্গ;
		    ++si;
		}
		ভঙ্গ;
	    }
    }
    অন্যথা
    {
	// Recognize a few exceptions to the rule.  Some strings that contain
	// '*'are changed to "star", otherwise '*' is recognized as a wildcard.
	প্রতি (i = 0; except_tbl[i][0] != রিক্ত; ++i)
	    যদি (STRCMP(arg, except_tbl[i][0]) == 0)
	    {
		STRCPY(d, except_tbl[i][1]);
		ভঙ্গ;
	    }
    }

    যদি (d[0] == NUL)	// no match in table
    {
	// Replace "\S" with "/\\S", etc.  Otherwise every tag is matched.
	// Also replace "\%^" and "\%(", they match every tag too.
	// Also "\zs", "\z1", etc.
	// Also "\@<", "\@=", "\@<=", etc.
	// And also "\_$" and "\_^".
	যদি (arg[0] == '\\'
		&& ((arg[1] != NUL && arg[2] == NUL)
		    || (vim_strchr((char_u *)"%_z@", arg[1]) != রিক্ত
							   && arg[2] != NUL)))
	{
	    vim_snprintf((বর্ণ *)d, IOSIZE, "/\\\\%s", arg + 1);
	    // Check for "/\\_$", should be "/\\_\$"
	    যদি (d[3] == '_' && d[4] == '$')
		STRCPY(d + 4, "\\$");
	}
	অন্যথা
	{
	  // Replace:
	  // "[:...:]" with "\[:...:]"
	  // "[++...]" with "\[++...]"
	  // "\{" with "\\{"		   -- matching "} \}"
	    যদি ((arg[0] == '[' && (arg[1] == ':'
			 || (arg[1] == '+' && arg[2] == '+')))
		    || (arg[0] == '\\' && arg[1] == '{'))
	      *d++ = '\\';

	  // If tag starts with "('", skip the "(". Fixes CTRL-] on ('option'.
	  যদি (*arg == '(' && arg[1] == '\'')
	      arg++;
	  প্রতি (s = arg; *s; ++s)
	  {
	    // Replace "|" with "bar" and '"' with "quote" to match the name of
	    // the tags for these commands.
	    // Replace "*" with ".*" and "?" with "." to match command line
	    // completion.
	    // Insert a backslash before '~', '$' and '.' to avoid their
	    // special meaning.
	    যদি (d - IObuff > IOSIZE - 10)	// getting too long!?
		ভঙ্গ;
	    বিকল্প (*s)
	    {
		পক্ষ '|':   STRCPY(d, "bar");
			    d += 3;
			    অনুবর্ত;
		পক্ষ '"':   STRCPY(d, "quote");
			    d += 5;
			    অনুবর্ত;
		পক্ষ '*':   *d++ = '.';
			    ভঙ্গ;
		পক্ষ '?':   *d++ = '.';
			    অনুবর্ত;
		পক্ষ '$':
		পক্ষ '.':
		পক্ষ '~':   *d++ = '\\';
			    ভঙ্গ;
	    }

	    // Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	    // ":help i_^_CTRL-D" work.
	    // Insert '-' before and after "CTRL-X" when applicable.
	    যদি (*s < ' ' || (*s == '^' && s[1] && (ASCII_ISALPHA(s[1])
			   || vim_strchr((char_u *)"?@[\\]^", s[1]) != রিক্ত)))
	    {
		যদি (d > IObuff && d[-1] != '_' && d[-1] != '\\')
		    *d++ = '_';		// prepend a '_' to make x_CTRL-x
		STRCPY(d, "CTRL-");
		d += 5;
		যদি (*s < ' ')
		{
		    *d++ = *s + '@';
		    যদি (d[-1] == '\\')
			*d++ = '\\';	// double a backslash
		}
		অন্যথা
		    *d++ = *++s;
		যদি (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		// append a '_'
		অনুবর্ত;
	    }
	    অন্যথা যদি (*s == '^')		// "^" or "CTRL-^" or "^_"
		*d++ = '\\';

	    // Insert a backslash before a backslash after a slash, for search
	    // pattern tags: "/\|" --> "/\\|".
	    অন্যথা যদি (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    // "CTRL-\_" -> "CTRL-\\_" to avoid the special meaning of "\_" in
	    // "CTRL-\_CTRL-N"
	    যদি (STRNICMP(s, "CTRL-\\_", 7) == 0)
	    {
		STRCPY(d, "CTRL-\\\\");
		d += 7;
		s += 6;
	    }

	    *d++ = *s;

	    // If tag contains "({" or "([", tag terminates at the "(".
	    // This is for help on functions, e.g.: abs({expr}).
	    যদি (*s == '(' && (s[1] == '{' || s[1] =='['))
		ভঙ্গ;

	    // If tag starts with ', toss everything after a second '. Fixes
	    // CTRL-] on 'option'. (would include the trailing '.').
	    যদি (*s == '\'' && s > arg && *arg == '\'')
		ভঙ্গ;
	    // Also '{' and '}'.
	    যদি (*s == '}' && s > arg && *arg == '{')
		ভঙ্গ;
	  }
	  *d = NUL;

	  যদি (*IObuff == '`')
	  {
	      যদি (d > IObuff + 2 && d[-1] == '`')
	      {
		  // remove the backticks from `command`
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-2] = NUL;
	      }
	      অন্যথা যদি (d > IObuff + 3 && d[-2] == '`' && d[-1] == ',')
	      {
		  // remove the backticks and comma from `command`,
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-3] = NUL;
	      }
	      অন্যথা যদি (d > IObuff + 4 && d[-3] == '`'
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
    *অঙ্ক_matches = 0;
    flags = TAG_HELP | TAG_REGEXP | TAG_NAMES | TAG_VERBOSE | TAG_NO_TAGFUNC;
    যদি (keep_lang)
	flags |= TAG_KEEP_LANG;
    যদি (find_tags(IObuff, অঙ্ক_matches, matches, flags, (পূর্ণাঙ্ক)MAXCOL, রিক্ত) == OK
	    && *অঙ্ক_matches > 0)
    {
	// Sort the matches found on the heuristic number that is after the
	// tag name.
	qsort((শূন্য *)*matches, (size_t)*অঙ্ক_matches,
					      পরিমাণ(char_u *), help_compare);
	// Delete more than TAG_MANY to reduce the size of the listing.
	যাবত্ (*অঙ্ক_matches > TAG_MANY)
	    vim_free((*matches)[--*অঙ্ক_matches]);
    }
    প্রত্যাগম OK;
}

#ifdef FEAT_MULTI_LANG
/*
 * Cleanup matches for help tags:
 * Remove "@ab" if the top of 'helplang' is "ab" and the language of the first
 * tag matches it.  Otherwise remove "@en" if "en" is the only language.
 */
    শূন্য
cleanup_help_tags(পূর্ণাঙ্ক অঙ্ক_file, char_u **file)
{
    পূর্ণাঙ্ক		i, j;
    পূর্ণাঙ্ক		len;
    char_u	buf[4];
    char_u	*p = buf;

    যদি (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    প্রতি (i = 0; i < অঙ্ক_file; ++i)
    {
	len = (পূর্ণাঙ্ক)STRLEN(file[i]) - 3;
	যদি (len <= 0)
	    অনুবর্ত;
	যদি (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    প্রতি (j = 0; j < অঙ্ক_file; ++j)
		যদি (j != i && (পূর্ণাঙ্ক)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    ভঙ্গ;
	    যদি (j == অঙ্ক_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    যদি (*buf != NUL)
	প্রতি (i = 0; i < অঙ্ক_file; ++i)
	{
	    len = (পূর্ণাঙ্ক)STRLEN(file[i]) - 3;
	    যদি (len <= 0)
		অনুবর্ত;
	    যদি (STRCMP(file[i] + len, buf) == 0)
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
    শূন্য
prepare_help_buffer(শূন্য)
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
    যদি (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	set_string_option_direct((char_u *)"isk", -1, p, OPT_FREE|OPT_LOCAL, 0);
	check_buf_options(curbuf);
	(শূন্য)buf_init_chartab(curbuf, FALSE);
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
    শূন্য
fix_help_buffer(শূন্য)
{
    linenr_T	lnum;
    char_u	*সারি;
    পূর্ণাঙ্ক		in_example = FALSE;
    পূর্ণাঙ্ক		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    পূর্ণাঙ্ক		mustfree;

    // Set filetype to "help" if still needed.
    যদি (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_lock;
	set_option_মান_give_err((char_u *)"ft",
					      0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_lock;
    }

#ifdef FEAT_SYN_HL
    যদি (!syntax_present(curwin))
#endif
    {
	প্রতি (lnum = 1; lnum <= curbuf->b_ml.ml_সারি_গুনতি; ++lnum)
	{
	    সারি = ml_get_buf(curbuf, lnum, FALSE);
	    len = ml_get_buf_len(curbuf, lnum);
	    যদি (in_example && len > 0 && !VIM_ISWHITE(সারি[0]))
	    {
		// End of example: non-white or '<' in first column.
		যদি (সারি[0] == '<')
		{
		    // blank-out a '<' in the first column
		    সারি = ml_get_buf(curbuf, lnum, TRUE);
		    সারি[0] = ' ';
		}
		in_example = FALSE;
	    }
	    যদি (!in_example && len > 0)
	    {
		যদি (সারি[len - 1] == '>' && (len == 1 || সারি[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    সারি = ml_get_buf(curbuf, lnum, TRUE);
		    সারি[len - 1] = ' ';
		    in_example = TRUE;
		}
		অন্যথা যদি (সারি[len - 1] == '~')
		{
		    // blank-out a '~' at the end of line (header marker)
		    সারি = ml_get_buf(curbuf, lnum, TRUE);
		    সারি[len - 1] = ' ';
		}
	    }
	}
    }

    // In the "help.txt" and "help.abx" file, add the locally added help
    // files.  This uses the very first line in the help file.
    fname = gettail(curbuf->b_fname);
    যদি (fnamecmp(fname, "help.txt") == 0
#ifdef FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#endif
	)
    {
	প্রতি (lnum = 1; lnum < curbuf->b_ml.ml_সারি_গুনতি; ++lnum)
	{
	    সারি = ml_get_buf(curbuf, lnum, FALSE);
	    যদি (strstr((বর্ণ *)সারি, "*local-additions*") == রিক্ত)
		অনুবর্ত;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    যাবত্ (*p != NUL)
	    {
		পূর্ণাঙ্ক NameBufflen;

		NameBufflen = copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		যদি (rt != রিক্ত &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    পূর্ণাঙ্ক		fcount;
		    char_u	**fnames;
		    FILE	*fd;
		    char_u	*s;
		    পূর্ণাঙ্ক		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    যদি (*NameBuff != NUL && !after_pathsep(NameBuff, NameBuff + NameBufflen))
		    {
			STRCPY(NameBuff + NameBufflen, PATHSEPSTR);
			NameBufflen += পরিমাণ(PATHSEP);
		    }
#ifdef FEAT_MULTI_LANG
		    STRCPY(NameBuff + NameBufflen, "doc/*.??[tx]");
#অন্যথা
		    STRCPY(NameBuff + NameBufflen, "doc/*.txt");
#endif
		    যদি (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#ifdef FEAT_MULTI_LANG
			পূর্ণাঙ্ক	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			প্রতি (i1 = 0; i1 < fcount; ++i1)
			{
			    f1 = fnames[i1];
			    t1 = gettail(f1);
			    e1 = vim_strrchr(t1, '.');
			    যদি (e1 == রিক্ত)
				অনুবর্ত;
			    যদি (fnamecmp(e1, ".txt") != 0
					       && fnamecmp(e1, fname + 4) != 0)
			    {
				// Not .txt and not .abx, remove it.
				VIM_CLEAR(fnames[i1]);
				অনুবর্ত;
			    }

			    প্রতি (i2 = i1 + 1; i2 < fcount; ++i2)
			    {
				f2 = fnames[i2];
				যদি (f2 == রিক্ত)
				    অনুবর্ত;
				t2 = gettail(f2);
				e2 = vim_strrchr(t2, '.');
				যদি (e2 == রিক্ত)
				    অনুবর্ত;
				যদি (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    অনুবর্ত;
				যদি (fnamecmp(e1, ".txt") == 0
					       && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#endif
			প্রতি (fi = 0; fi < fcount; ++fi)
			{
			    যদি (fnames[fi] == রিক্ত)
				অনুবর্ত;
			    fd = mch_fopen((বর্ণ *)fnames[fi], "r");
			    যদি (fd != রিক্ত)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				যদি (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != রিক্ত)
				{
				    পূর্ণাঙ্ক	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    যাবত্ (*s != NUL)
				    {
					যদি (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					যদি (*s >= 0x80 && this_utf != FALSE)
					{
					    পূর্ণাঙ্ক	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    যদি (l == 1)
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
				    যদি (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    অন্যথা
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    রিক্ত);
					যদি (cp == রিক্ত)
					    cp = IObuff;
				    }
				    convert_setup(&vc, রিক্ত, রিক্ত);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    যদি (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				fclose(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		যদি (mustfree)
		    vim_free(rt);
	    }
	    ভঙ্গ;
	}
    }
}

/*
 * ":exusage"
 */
    শূন্য
ex_exusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help ex-cmd-index");
}

/*
 * ":viusage"
 */
    শূন্য
ex_viusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help normal-index");
}

/*
 * Generate tags in one help directory.
 */
    স্থির শূন্য
helptags_one(
    char_u	*dir,		// doc directory
    char_u	*ext,		// suffix, ".txt", ".itx", ".frx", etc.
    char_u	*tagfname,	// "tags" for English, "tags-fr" for French.
    পূর্ণাঙ্ক		add_help_tags,	// add "help-tags" tag
    পূর্ণাঙ্ক		ignore_writeerr)    // ignore write error
{
    FILE	*fd_tags;
    FILE	*fd;
    garray_T	ga;
    পূর্ণাঙ্ক		res;
    পূর্ণাঙ্ক		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    পূর্ণাঙ্ক		fi;
    char_u	*s;
    পূর্ণাঙ্ক		i;
    char_u	*fname;
    পূর্ণাঙ্ক		dirlen;
    পূর্ণাঙ্ক		utf8 = MAYBE;
    পূর্ণাঙ্ক		this_utf8;
    পূর্ণাঙ্ক		firstline;
    পূর্ণাঙ্ক		in_example;
    পূর্ণাঙ্ক		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (পূর্ণাঙ্ক)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    res = gen_expand_wildcards(1, &NameBuff, &filecount, &files,
							    EW_FILE|EW_SILENT);
    যদি (res == FAIL || filecount == 0)
    {
	যদি (!got_int)
	    semsg(_(e_no_match_str_1), NameBuff);
	যদি (res != FAIL)
	    FreeWild(filecount, files);
	প্রত্যাগম;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_tags = mch_fopen((বর্ণ *)NameBuff, "w");
    যদি (fd_tags == রিক্ত)
    {
	যদি (!ignore_writeerr)
	    semsg(_(e_cannot_open_str_for_writing_1), NameBuff);
	FreeWild(filecount, files);
	প্রত্যাগম;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, পরিমাণ(char_u *), 100);
    যদি (add_help_tags || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	যদি (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	অন্যথা
	{
	    s = alloc(18 + (unsigned)STRLEN(tagfname));
	    যদি (s == রিক্ত)
		got_int = TRUE;
	    অন্যথা
	    {
		sprintf((বর্ণ *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_data)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    প্রতি (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((বর্ণ *)files[fi], "r");
	যদি (fd == রিক্ত)
	{
	    semsg(_(e_unable_to_open_str_for_reading), files[fi]);
	    অনুবর্ত;
	}
	fname = files[fi] + dirlen + 1;

	in_example = FALSE;
	firstline = TRUE;
	যাবত্ (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    যদি (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		প্রতি (s = IObuff; *s != NUL; ++s)
		    যদি (*s >= 0x80)
		    {
			পূর্ণাঙ্ক l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			যদি (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    ভঙ্গ;
			}
			s += l - 1;
		    }
		যদি (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		যদি (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		অন্যথা যদি (utf8 != this_utf8)
		{
		    semsg(_(e_mix_of_help_file_encodings_within_language_str), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    যদি (in_example)
	    {
		// skip over example; a non-white in the first column ends it
		যদি (vim_strchr((char_u *)" \t\n\r", IObuff[0]))
		    অনুবর্ত;
		in_example = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    যাবত্ (p1 != রিক্ত)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		যদি (p2 != রিক্ত && p2 > p1 + 1)	// skip "*" and "**"
		{
		    প্রতি (s = p1 + 1; s < p2; ++s)
			যদি (*s == ' ' || *s == '\t' || *s == '|')
			    ভঙ্গ;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    যদি (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != রিক্ত
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			যদি (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    ভঙ্গ;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			যদি (s == রিক্ত)
			{
			    got_int = TRUE;
			    ভঙ্গ;
			}
			((char_u **)ga.ga_data)[ga.ga_len] = s;
			++ga.ga_len;
			sprintf((বর্ণ *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    size_t off = STRLEN(IObuff);
	    যদি (off >= 2 && IObuff[off - 1] == '\n')
	    {
		off -= 2;
		যাবত্ (off > 0 && (ASCII_ISLOWER(IObuff[off])
						  || VIM_ISDIGIT(IObuff[off])))
		    off--;
		যদি (IObuff[off] == '>' && (off == 0 || IObuff[off - 1] == ' '))
		    in_example = TRUE;
	    }
	    সারি_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    যদি (!got_int)
    {
	// Sort the tags.
	যদি (ga.ga_data != রিক্ত)
	    sort_strings((char_u **)ga.ga_data, ga.ga_len);

	// Check for duplicates.
	প্রতি (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_data)[i - 1];
	    p2 = ((char_u **)ga.ga_data)[i];
	    যাবত্ (*p1 == *p2)
	    {
		যদি (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((বর্ণ *)NameBuff, MAXPATHL,
			    _(e_duplicate_tag_str_in_file_str_str),
				     ((char_u **)ga.ga_data)[i], dir, p2 + 1);
		    emsg((বর্ণ *)NameBuff);
		    *p2 = '\t';
		    ভঙ্গ;
		}
		++p1;
		++p2;
	    }
	}

	যদি (utf8 == TRUE)
	    fprintf(fd_tags, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	প্রতি (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_data)[i];
	    যদি (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		fputs((বর্ণ *)s, fd_tags);
	    অন্যথা
	    {
		fprintf(fd_tags, "%s\t/*", s);
		প্রতি (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    যদি (*p1 == '\\' || *p1 == '/')
			putc('\\', fd_tags);
		    putc(*p1, fd_tags);
		}
		fprintf(fd_tags, "*\n");
	    }
	}
    }
    যদি (mix)
	got_int = FALSE;    // continue with other languages

    প্রতি (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_data)[i]);
    ga_clear(&ga);
    fclose(fd_tags);	    // there is no check for an error...
}

/*
 * Generate tags in one help directory, taking care of translations.
 */
    স্থির শূন্য
do_helptags(char_u *dirname, পূর্ণাঙ্ক add_help_tags, পূর্ণাঙ্ক ignore_writeerr)
{
#ifdef FEAT_MULTI_LANG
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    পূর্ণাঙ্ক		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, dirname);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    যদি (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_(e_no_match_str_1), NameBuff);
	প্রত্যাগম;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    প্রতি (i = 0; i < filecount; ++i)
    {
	len = (পূর্ণাঙ্ক)STRLEN(files[i]);
	যদি (len <= 4)
	    অনুবর্ত;

	যদি (STRICMP(files[i] + len - 4, ".txt") == 0)
	{
	    // ".txt" -> language "en"
	    lang[0] = 'e';
	    lang[1] = 'n';
	}
	অন্যথা যদি (files[i][len - 4] == '.'
		&& ASCII_ISALPHA(files[i][len - 3])
		&& ASCII_ISALPHA(files[i][len - 2])
		&& TOLOWER_ASC(files[i][len - 1]) == 'x')
	{
	    // ".abx" -> language "ab"
	    lang[0] = TOLOWER_ASC(files[i][len - 3]);
	    lang[1] = TOLOWER_ASC(files[i][len - 2]);
	}
	অন্যথা
	    অনুবর্ত;

	// Did we find this language already?
	প্রতি (j = 0; j < ga.ga_len; j += 2)
	    যদি (STRNCMP(lang, ((char_u *)ga.ga_data) + j, 2) == 0)
		ভঙ্গ;
	যদি (j == ga.ga_len)
	{
	    // New language, add it.
	    যদি (ga_grow(&ga, 2) == FAIL)
		ভঙ্গ;
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[0];
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[1];
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    প্রতি (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_data)[j];
	fname[6] = ((char_u *)ga.ga_data)[j + 1];
	যদি (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	অন্যথা
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

#অন্যথা
    // No language support, just use "*.txt" and "tags".
    helptags_one(dirname, (char_u *)".txt", (char_u *)"tags", add_help_tags,
							    ignore_writeerr);
#endif
}

    স্থির শূন্য
helptags_cb(char_u *fname, শূন্য *cookie)
{
    do_helptags(fname, *(পূর্ণাঙ্ক *)cookie, TRUE);
}

/*
 * ":helptags"
 */
    শূন্য
ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*dirname;
    পূর্ণাঙ্ক		add_help_tags = FALSE;

    // Check for ":helptags ++t {dir}".
    যদি (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	add_help_tags = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    যদি (STRCMP(eap->arg, "ALL") == 0)
    {
	do_in_path(p_rtp, "", (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &add_help_tags);
    }
    অন্যথা
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	dirname = ExpandOne(&xpc, eap->arg, রিক্ত,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	যদি (dirname == রিক্ত || !mch_isdir(dirname))
	    semsg(_(e_not_a_directory_str), eap->arg);
	অন্যথা
	    do_helptags(dirname, add_help_tags, FALSE);
	vim_free(dirname);
    }
}
