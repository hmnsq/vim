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

#அந்தர்பாவ "vim.h"

/*
 * ":help": open a read-only window on a help file
 */
    வெறுமை
ex_help(exarg_T *eap)
{
    char_u	*arg;
    char_u	*tag;
    FILE	*helpfd;	// file descriptor of help file
    முழு		n;
    முழு		i;
    win_T	*wp;
    முழு		இலக்கம்_matches;
    char_u	**matches;
    char_u	*p;
    முழு		empty_fnum = 0;
    முழு		alt_fnum = 0;
    buf_T	*buf;
#ifdef FEAT_MULTI_LANG
    முழு		len;
    char_u	*lang;
#endif
#ifdef FEAT_FOLDING
    முழு		old_சாவி_Typed = சாவி_Typed;
#endif

    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;

    எனில் (eap != வெற்று)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	க்கு (arg = eap->arg; *arg; ++arg)
	{
	    எனில் (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		நிறுத்து;
	    }
	}
	arg = eap->arg;

	எனில் (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_(e_dont_panic));
	    திரும்பு;
	}

	எனில் (eap->skip)	    // not executing commands
	    திரும்பு;
    }
    இல்லையெனில்
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    போது (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#endif

    // When no argument given go to the index.
    எனில் (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = find_help_tags(arg, &இலக்கம்_matches, &matches,
						 eap != வெற்று && eap->forceit);

    i = 0;
#ifdef FEAT_MULTI_LANG
    எனில் (n != FAIL && lang != வெற்று)
	// Find first item with the requested language.
	க்கு (i = 0; i < இலக்கம்_matches; ++i)
	{
	    len = (முழு)STRLEN(matches[i]);
	    எனில் (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		நிறுத்து;
	}
#endif
    எனில் (i >= இலக்கம்_matches || n == FAIL)
    {
#ifdef FEAT_MULTI_LANG
	எனில் (lang != வெற்று)
	    semsg(_(e_sorry_no_str_help_for_str), lang, arg);
	இல்லையெனில்
#endif
	    semsg(_(e_sorry_no_help_for_str), arg);
	எனில் (n != FAIL)
	    FreeWild(இலக்கம்_matches, matches);
	திரும்பு;
    }

    // The first match (in the requested language) is the best match.
    tag = vim_strsave(matches[i]);
    FreeWild(இலக்கம்_matches, matches);

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    எனில் (!bt_help(curwin->w_buffer) || cmdmod.cmod_tab != 0)
    {
	எனில் (cmdmod.cmod_tab != 0)
	    wp = வெற்று;
	இல்லையெனில்
	    FOR_ALL_WINDOWS(wp)
		எனில் (bt_help(wp->w_buffer))
		    நிறுத்து;
	எனில் (wp != வெற்று && wp->w_buffer->b_nwindows > 0)
	    win_enter(wp, TRUE);
	இல்லையெனில்
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    எனில் ((helpfd = mch_fopen((எழுத்து *)p_hf, READBIN)) == வெற்று)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		செல் erret;
	    }
	    fclose(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    எனில் (cmdmod.cmod_split == 0 && curwin->w_width != Columns
						  && curwin->w_width < 80)
		n |= p_sb ? WSP_BOT : WSP_உச்சி;
	    எனில் (win_split(0, n) == FAIL)
		செல் erret;

	    எனில் (curwin->w_height < p_hh)
		win_setheight((முழு)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (வெறுமை)do_ecmd(0, வெற்று, வெற்று, வெற்று, ECMD_LASTL,
			  ECMD_HIDE + ECMD_SET_HELP,
			  வெற்று);  // buffer is still open, don't store info
	    எனில் ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    empty_fnum = curbuf->b_fnum;
	}
    }

    எனில் (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#ifdef FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    சாவி_Typed = old_சாவி_Typed;
#endif

    எனில் (tag != வெற்று)
	do_tag(tag, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    எனில் (empty_fnum != 0 && curbuf->b_fnum != empty_fnum)
    {
	buf = buflist_findnr(empty_fnum);
	எனில் (buf != வெற்று && buf->b_nwindows == 0)
	    wipe_buffer(buf, TRUE);
    }

    // keep the previous alternate file
    எனில் (alt_fnum != 0 && curwin->w_alt_fnum == empty_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(tag);
}

/*
 * ":helpclose": Close one help window
 */
    வெறுமை
ex_helpclose(exarg_T *eap UNUSED)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	எனில் (bt_help(win->w_buffer))
	{
	    win_close(win, FALSE);
	    திரும்பு;
	}
    }
}

#எனில் defined(FEAT_MULTI_LANG)
/*
 * In an argument search for a language specifiers in the form "@xx".
 * Changes the "@" to NUL if found, and returns a pointer to "xx".
 * Returns NULL if not found.
 */
    char_u *
check_help_lang(char_u *arg)
{
    முழு len = (முழு)STRLEN(arg);

    எனில் (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	திரும்பு arg + len - 2;
    }
    திரும்பு வெற்று;
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
    முழு
help_heuristic(
    char_u	*matched_string,
    முழு		offset,			// offset for match
    முழு		wrong_case)		// no matching case
{
    முழு		இலக்கம்_letters;
    char_u	*p;

    இலக்கம்_letters = 0;
    க்கு (p = matched_string; *p; p++)
	எனில் (ASCII_ISALNUM(*p))
	    இலக்கம்_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    எனில் (ASCII_ISALNUM(matched_string[offset]) && offset > 0
				 && ASCII_ISALNUM(matched_string[offset - 1]))
	offset += 10000;
    இல்லையெனில் எனில் (offset > 2)
	offset *= 200;
    எனில் (wrong_case)
	offset += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    எனில் (matched_string[0] == '+' && matched_string[1] != NUL)
	offset += 100;
    திரும்பு (முழு)(100 * இலக்கம்_letters + STRLEN(matched_string) + offset);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    நிலையான முழு
help_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    எழுத்து    *p1;
    எழுத்து    *p2;
    முழு	    cmp;

    p1 = *(எழுத்து **)s1 + strlen(*(எழுத்து **)s1) + 1;
    p2 = *(எழுத்து **)s2 + strlen(*(எழுத்து **)s2) + 1;

    // Compare by help heuristic number first.
    cmp = strcmp(p1, p2);
    எனில் (cmp != 0)
	திரும்பு cmp;

    // Compare by strings as tie-breaker when same heuristic number.
    திரும்பு strcmp(*(எழுத்து **)s1, *(எழுத்து **)s2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * The matches will be sorted with a "best" match algorithm.
 * When "keep_lang" is TRUE try keeping the language of the current buffer.
 */
    முழு
find_help_tags(
    char_u	*arg,
    முழு		*இலக்கம்_matches,
    char_u	***matches,
    முழு		keep_lang)
{
    char_u	*s, *d;
    முழு		i;
    // Specific tags that either have a specific replacement or won't go
    // through the generic rules.
    நிலையான எழுத்து *(except_tbl[][2]) = {
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
	{வெற்று, வெற்று}
    };
    நிலையான எழுத்து *(கோவை_table[]) = {"!=?", "!~?", "<=?", "<?", "==?", "=~?",
				   ">=?", ">?", "is?", "isnot?"};
    முழு flags;

    d = IObuff;		    // assume IObuff is long enough!
    d[0] = NUL;

    எனில் (STRNICMP(arg, "expr-", 5) == 0)
    {
	// When the string starting with "expr-" and containing '?' and matches
	// the table, it is taken literally (but ~ is escaped).  Otherwise '?'
	// is recognized as a wildcard.
	க்கு (i = (முழு)ARRAY_LENGTH(கோவை_table); --i >= 0; )
	    எனில் (STRCMP(arg + 5, கோவை_table[i]) == 0)
	    {
		முழு si = 0, di = 0;

		க்கு (;;)
		{
		    எனில் (arg[si] == '~')
			d[di++] = '\\';
		    d[di++] = arg[si];
		    எனில் (arg[si] == NUL)
			நிறுத்து;
		    ++si;
		}
		நிறுத்து;
	    }
    }
    இல்லையெனில்
    {
	// Recognize a few exceptions to the rule.  Some strings that contain
	// '*'are changed to "star", otherwise '*' is recognized as a wildcard.
	க்கு (i = 0; except_tbl[i][0] != வெற்று; ++i)
	    எனில் (STRCMP(arg, except_tbl[i][0]) == 0)
	    {
		STRCPY(d, except_tbl[i][1]);
		நிறுத்து;
	    }
    }

    எனில் (d[0] == NUL)	// no match in table
    {
	// Replace "\S" with "/\\S", etc.  Otherwise every tag is matched.
	// Also replace "\%^" and "\%(", they match every tag too.
	// Also "\zs", "\z1", etc.
	// Also "\@<", "\@=", "\@<=", etc.
	// And also "\_$" and "\_^".
	எனில் (arg[0] == '\\'
		&& ((arg[1] != NUL && arg[2] == NUL)
		    || (vim_strchr((char_u *)"%_z@", arg[1]) != வெற்று
							   && arg[2] != NUL)))
	{
	    vim_snprintf((எழுத்து *)d, IOSIZE, "/\\\\%s", arg + 1);
	    // Check for "/\\_$", should be "/\\_\$"
	    எனில் (d[3] == '_' && d[4] == '$')
		STRCPY(d + 4, "\\$");
	}
	இல்லையெனில்
	{
	  // Replace:
	  // "[:...:]" with "\[:...:]"
	  // "[++...]" with "\[++...]"
	  // "\{" with "\\{"		   -- matching "} \}"
	    எனில் ((arg[0] == '[' && (arg[1] == ':'
			 || (arg[1] == '+' && arg[2] == '+')))
		    || (arg[0] == '\\' && arg[1] == '{'))
	      *d++ = '\\';

	  // If tag starts with "('", skip the "(". Fixes CTRL-] on ('option'.
	  எனில் (*arg == '(' && arg[1] == '\'')
	      arg++;
	  க்கு (s = arg; *s; ++s)
	  {
	    // Replace "|" with "bar" and '"' with "quote" to match the name of
	    // the tags for these commands.
	    // Replace "*" with ".*" and "?" with "." to match command line
	    // completion.
	    // Insert a backslash before '~', '$' and '.' to avoid their
	    // special meaning.
	    எனில் (d - IObuff > IOSIZE - 10)	// getting too long!?
		நிறுத்து;
	    தேர்வு (*s)
	    {
		நிலை '|':   STRCPY(d, "bar");
			    d += 3;
			    தொடர்;
		நிலை '"':   STRCPY(d, "quote");
			    d += 5;
			    தொடர்;
		நிலை '*':   *d++ = '.';
			    நிறுத்து;
		நிலை '?':   *d++ = '.';
			    தொடர்;
		நிலை '$':
		நிலை '.':
		நிலை '~':   *d++ = '\\';
			    நிறுத்து;
	    }

	    // Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	    // ":help i_^_CTRL-D" work.
	    // Insert '-' before and after "CTRL-X" when applicable.
	    எனில் (*s < ' ' || (*s == '^' && s[1] && (ASCII_ISALPHA(s[1])
			   || vim_strchr((char_u *)"?@[\\]^", s[1]) != வெற்று)))
	    {
		எனில் (d > IObuff && d[-1] != '_' && d[-1] != '\\')
		    *d++ = '_';		// prepend a '_' to make x_CTRL-x
		STRCPY(d, "CTRL-");
		d += 5;
		எனில் (*s < ' ')
		{
		    *d++ = *s + '@';
		    எனில் (d[-1] == '\\')
			*d++ = '\\';	// double a backslash
		}
		இல்லையெனில்
		    *d++ = *++s;
		எனில் (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		// append a '_'
		தொடர்;
	    }
	    இல்லையெனில் எனில் (*s == '^')		// "^" or "CTRL-^" or "^_"
		*d++ = '\\';

	    // Insert a backslash before a backslash after a slash, for search
	    // pattern tags: "/\|" --> "/\\|".
	    இல்லையெனில் எனில் (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    // "CTRL-\_" -> "CTRL-\\_" to avoid the special meaning of "\_" in
	    // "CTRL-\_CTRL-N"
	    எனில் (STRNICMP(s, "CTRL-\\_", 7) == 0)
	    {
		STRCPY(d, "CTRL-\\\\");
		d += 7;
		s += 6;
	    }

	    *d++ = *s;

	    // If tag contains "({" or "([", tag terminates at the "(".
	    // This is for help on functions, e.g.: abs({expr}).
	    எனில் (*s == '(' && (s[1] == '{' || s[1] =='['))
		நிறுத்து;

	    // If tag starts with ', toss everything after a second '. Fixes
	    // CTRL-] on 'option'. (would include the trailing '.').
	    எனில் (*s == '\'' && s > arg && *arg == '\'')
		நிறுத்து;
	    // Also '{' and '}'.
	    எனில் (*s == '}' && s > arg && *arg == '{')
		நிறுத்து;
	  }
	  *d = NUL;

	  எனில் (*IObuff == '`')
	  {
	      எனில் (d > IObuff + 2 && d[-1] == '`')
	      {
		  // remove the backticks from `command`
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-2] = NUL;
	      }
	      இல்லையெனில் எனில் (d > IObuff + 3 && d[-2] == '`' && d[-1] == ',')
	      {
		  // remove the backticks and comma from `command`,
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-3] = NUL;
	      }
	      இல்லையெனில் எனில் (d > IObuff + 4 && d[-3] == '`'
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
    *இலக்கம்_matches = 0;
    flags = TAG_HELP | TAG_REGEXP | TAG_NAMES | TAG_VERBOSE | TAG_NO_TAGFUNC;
    எனில் (keep_lang)
	flags |= TAG_KEEP_LANG;
    எனில் (find_tags(IObuff, இலக்கம்_matches, matches, flags, (முழு)MAXCOL, வெற்று) == OK
	    && *இலக்கம்_matches > 0)
    {
	// Sort the matches found on the heuristic number that is after the
	// tag name.
	qsort((வெறுமை *)*matches, (size_t)*இலக்கம்_matches,
					      அளவு(char_u *), help_compare);
	// Delete more than TAG_MANY to reduce the size of the listing.
	போது (*இலக்கம்_matches > TAG_MANY)
	    vim_free((*matches)[--*இலக்கம்_matches]);
    }
    திரும்பு OK;
}

#ifdef FEAT_MULTI_LANG
/*
 * Cleanup matches for help tags:
 * Remove "@ab" if the top of 'helplang' is "ab" and the language of the first
 * tag matches it.  Otherwise remove "@en" if "en" is the only language.
 */
    வெறுமை
cleanup_help_tags(முழு இலக்கம்_file, char_u **file)
{
    முழு		i, j;
    முழு		len;
    char_u	buf[4];
    char_u	*p = buf;

    எனில் (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    க்கு (i = 0; i < இலக்கம்_file; ++i)
    {
	len = (முழு)STRLEN(file[i]) - 3;
	எனில் (len <= 0)
	    தொடர்;
	எனில் (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    க்கு (j = 0; j < இலக்கம்_file; ++j)
		எனில் (j != i && (முழு)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    நிறுத்து;
	    எனில் (j == இலக்கம்_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    எனில் (*buf != NUL)
	க்கு (i = 0; i < இலக்கம்_file; ++i)
	{
	    len = (முழு)STRLEN(file[i]) - 3;
	    எனில் (len <= 0)
		தொடர்;
	    எனில் (STRCMP(file[i] + len, buf) == 0)
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
    வெறுமை
prepare_help_buffer(வெறுமை)
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
    எனில் (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	set_string_option_direct((char_u *)"isk", -1, p, OPT_FREE|OPT_LOCAL, 0);
	check_buf_options(curbuf);
	(வெறுமை)buf_init_chartab(curbuf, FALSE);
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
    வெறுமை
fix_help_buffer(வெறுமை)
{
    linenr_T	lnum;
    char_u	*வரி;
    முழு		in_example = FALSE;
    முழு		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    முழு		mustfree;

    // Set filetype to "help" if still needed.
    எனில் (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_lock;
	set_option_மதிப்பு_give_err((char_u *)"ft",
					      0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_lock;
    }

#ifdef FEAT_SYN_HL
    எனில் (!syntax_present(curwin))
#endif
    {
	க்கு (lnum = 1; lnum <= curbuf->b_ml.ml_வரி_மொத்தம்; ++lnum)
	{
	    வரி = ml_get_buf(curbuf, lnum, FALSE);
	    len = ml_get_buf_len(curbuf, lnum);
	    எனில் (in_example && len > 0 && !VIM_ISWHITE(வரி[0]))
	    {
		// End of example: non-white or '<' in first column.
		எனில் (வரி[0] == '<')
		{
		    // blank-out a '<' in the first column
		    வரி = ml_get_buf(curbuf, lnum, TRUE);
		    வரி[0] = ' ';
		}
		in_example = FALSE;
	    }
	    எனில் (!in_example && len > 0)
	    {
		எனில் (வரி[len - 1] == '>' && (len == 1 || வரி[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    வரி = ml_get_buf(curbuf, lnum, TRUE);
		    வரி[len - 1] = ' ';
		    in_example = TRUE;
		}
		இல்லையெனில் எனில் (வரி[len - 1] == '~')
		{
		    // blank-out a '~' at the end of line (header marker)
		    வரி = ml_get_buf(curbuf, lnum, TRUE);
		    வரி[len - 1] = ' ';
		}
	    }
	}
    }

    // In the "help.txt" and "help.abx" file, add the locally added help
    // files.  This uses the very first line in the help file.
    fname = gettail(curbuf->b_fname);
    எனில் (fnamecmp(fname, "help.txt") == 0
#ifdef FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#endif
	)
    {
	க்கு (lnum = 1; lnum < curbuf->b_ml.ml_வரி_மொத்தம்; ++lnum)
	{
	    வரி = ml_get_buf(curbuf, lnum, FALSE);
	    எனில் (strstr((எழுத்து *)வரி, "*local-additions*") == வெற்று)
		தொடர்;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    போது (*p != NUL)
	    {
		முழு NameBufflen;

		NameBufflen = copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		எனில் (rt != வெற்று &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    முழு		fcount;
		    char_u	**fnames;
		    FILE	*fd;
		    char_u	*s;
		    முழு		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    எனில் (*NameBuff != NUL && !after_pathsep(NameBuff, NameBuff + NameBufflen))
		    {
			STRCPY(NameBuff + NameBufflen, PATHSEPSTR);
			NameBufflen += அளவு(PATHSEP);
		    }
#ifdef FEAT_MULTI_LANG
		    STRCPY(NameBuff + NameBufflen, "doc/*.??[tx]");
#இல்லையெனில்
		    STRCPY(NameBuff + NameBufflen, "doc/*.txt");
#endif
		    எனில் (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#ifdef FEAT_MULTI_LANG
			முழு	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			க்கு (i1 = 0; i1 < fcount; ++i1)
			{
			    f1 = fnames[i1];
			    t1 = gettail(f1);
			    e1 = vim_strrchr(t1, '.');
			    எனில் (e1 == வெற்று)
				தொடர்;
			    எனில் (fnamecmp(e1, ".txt") != 0
					       && fnamecmp(e1, fname + 4) != 0)
			    {
				// Not .txt and not .abx, remove it.
				VIM_CLEAR(fnames[i1]);
				தொடர்;
			    }

			    க்கு (i2 = i1 + 1; i2 < fcount; ++i2)
			    {
				f2 = fnames[i2];
				எனில் (f2 == வெற்று)
				    தொடர்;
				t2 = gettail(f2);
				e2 = vim_strrchr(t2, '.');
				எனில் (e2 == வெற்று)
				    தொடர்;
				எனில் (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    தொடர்;
				எனில் (fnamecmp(e1, ".txt") == 0
					       && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#endif
			க்கு (fi = 0; fi < fcount; ++fi)
			{
			    எனில் (fnames[fi] == வெற்று)
				தொடர்;
			    fd = mch_fopen((எழுத்து *)fnames[fi], "r");
			    எனில் (fd != வெற்று)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				எனில் (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != வெற்று)
				{
				    முழு	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    போது (*s != NUL)
				    {
					எனில் (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					எனில் (*s >= 0x80 && this_utf != FALSE)
					{
					    முழு	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    எனில் (l == 1)
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
				    எனில் (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    இல்லையெனில்
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    வெற்று);
					எனில் (cp == வெற்று)
					    cp = IObuff;
				    }
				    convert_setup(&vc, வெற்று, வெற்று);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    எனில் (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				fclose(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		எனில் (mustfree)
		    vim_free(rt);
	    }
	    நிறுத்து;
	}
    }
}

/*
 * ":exusage"
 */
    வெறுமை
ex_exusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help ex-cmd-index");
}

/*
 * ":viusage"
 */
    வெறுமை
ex_viusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help normal-index");
}

/*
 * Generate tags in one help directory.
 */
    நிலையான வெறுமை
helptags_one(
    char_u	*dir,		// doc directory
    char_u	*ext,		// suffix, ".txt", ".itx", ".frx", etc.
    char_u	*tagfname,	// "tags" for English, "tags-fr" for French.
    முழு		add_help_tags,	// add "help-tags" tag
    முழு		ignore_writeerr)    // ignore write error
{
    FILE	*fd_tags;
    FILE	*fd;
    garray_T	ga;
    முழு		res;
    முழு		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    முழு		fi;
    char_u	*s;
    முழு		i;
    char_u	*fname;
    முழு		dirlen;
    முழு		utf8 = MAYBE;
    முழு		this_utf8;
    முழு		firstline;
    முழு		in_example;
    முழு		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (முழு)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    res = gen_expand_wildcards(1, &NameBuff, &filecount, &files,
							    EW_FILE|EW_SILENT);
    எனில் (res == FAIL || filecount == 0)
    {
	எனில் (!got_int)
	    semsg(_(e_no_match_str_1), NameBuff);
	எனில் (res != FAIL)
	    FreeWild(filecount, files);
	திரும்பு;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_tags = mch_fopen((எழுத்து *)NameBuff, "w");
    எனில் (fd_tags == வெற்று)
    {
	எனில் (!ignore_writeerr)
	    semsg(_(e_cannot_open_str_for_writing_1), NameBuff);
	FreeWild(filecount, files);
	திரும்பு;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, அளவு(char_u *), 100);
    எனில் (add_help_tags || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	எனில் (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	இல்லையெனில்
	{
	    s = alloc(18 + (unsigned)STRLEN(tagfname));
	    எனில் (s == வெற்று)
		got_int = TRUE;
	    இல்லையெனில்
	    {
		sprintf((எழுத்து *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_data)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    க்கு (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((எழுத்து *)files[fi], "r");
	எனில் (fd == வெற்று)
	{
	    semsg(_(e_unable_to_open_str_for_reading), files[fi]);
	    தொடர்;
	}
	fname = files[fi] + dirlen + 1;

	in_example = FALSE;
	firstline = TRUE;
	போது (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    எனில் (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		க்கு (s = IObuff; *s != NUL; ++s)
		    எனில் (*s >= 0x80)
		    {
			முழு l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			எனில் (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    நிறுத்து;
			}
			s += l - 1;
		    }
		எனில் (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		எனில் (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		இல்லையெனில் எனில் (utf8 != this_utf8)
		{
		    semsg(_(e_mix_of_help_file_encodings_within_language_str), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    எனில் (in_example)
	    {
		// skip over example; a non-white in the first column ends it
		எனில் (vim_strchr((char_u *)" \t\n\r", IObuff[0]))
		    தொடர்;
		in_example = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    போது (p1 != வெற்று)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		எனில் (p2 != வெற்று && p2 > p1 + 1)	// skip "*" and "**"
		{
		    க்கு (s = p1 + 1; s < p2; ++s)
			எனில் (*s == ' ' || *s == '\t' || *s == '|')
			    நிறுத்து;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    எனில் (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != வெற்று
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			எனில் (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    நிறுத்து;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			எனில் (s == வெற்று)
			{
			    got_int = TRUE;
			    நிறுத்து;
			}
			((char_u **)ga.ga_data)[ga.ga_len] = s;
			++ga.ga_len;
			sprintf((எழுத்து *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    size_t off = STRLEN(IObuff);
	    எனில் (off >= 2 && IObuff[off - 1] == '\n')
	    {
		off -= 2;
		போது (off > 0 && (ASCII_ISLOWER(IObuff[off])
						  || VIM_ISDIGIT(IObuff[off])))
		    off--;
		எனில் (IObuff[off] == '>' && (off == 0 || IObuff[off - 1] == ' '))
		    in_example = TRUE;
	    }
	    வரி_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    எனில் (!got_int)
    {
	// Sort the tags.
	எனில் (ga.ga_data != வெற்று)
	    sort_strings((char_u **)ga.ga_data, ga.ga_len);

	// Check for duplicates.
	க்கு (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_data)[i - 1];
	    p2 = ((char_u **)ga.ga_data)[i];
	    போது (*p1 == *p2)
	    {
		எனில் (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((எழுத்து *)NameBuff, MAXPATHL,
			    _(e_duplicate_tag_str_in_file_str_str),
				     ((char_u **)ga.ga_data)[i], dir, p2 + 1);
		    emsg((எழுத்து *)NameBuff);
		    *p2 = '\t';
		    நிறுத்து;
		}
		++p1;
		++p2;
	    }
	}

	எனில் (utf8 == TRUE)
	    fprintf(fd_tags, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	க்கு (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_data)[i];
	    எனில் (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		fputs((எழுத்து *)s, fd_tags);
	    இல்லையெனில்
	    {
		fprintf(fd_tags, "%s\t/*", s);
		க்கு (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    எனில் (*p1 == '\\' || *p1 == '/')
			putc('\\', fd_tags);
		    putc(*p1, fd_tags);
		}
		fprintf(fd_tags, "*\n");
	    }
	}
    }
    எனில் (mix)
	got_int = FALSE;    // continue with other languages

    க்கு (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_data)[i]);
    ga_clear(&ga);
    fclose(fd_tags);	    // there is no check for an error...
}

/*
 * Generate tags in one help directory, taking care of translations.
 */
    நிலையான வெறுமை
do_helptags(char_u *dirname, முழு add_help_tags, முழு ignore_writeerr)
{
#ifdef FEAT_MULTI_LANG
    முழு		len;
    முழு		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    முழு		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, dirname);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    எனில் (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_(e_no_match_str_1), NameBuff);
	திரும்பு;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    க்கு (i = 0; i < filecount; ++i)
    {
	len = (முழு)STRLEN(files[i]);
	எனில் (len <= 4)
	    தொடர்;

	எனில் (STRICMP(files[i] + len - 4, ".txt") == 0)
	{
	    // ".txt" -> language "en"
	    lang[0] = 'e';
	    lang[1] = 'n';
	}
	இல்லையெனில் எனில் (files[i][len - 4] == '.'
		&& ASCII_ISALPHA(files[i][len - 3])
		&& ASCII_ISALPHA(files[i][len - 2])
		&& TOLOWER_ASC(files[i][len - 1]) == 'x')
	{
	    // ".abx" -> language "ab"
	    lang[0] = TOLOWER_ASC(files[i][len - 3]);
	    lang[1] = TOLOWER_ASC(files[i][len - 2]);
	}
	இல்லையெனில்
	    தொடர்;

	// Did we find this language already?
	க்கு (j = 0; j < ga.ga_len; j += 2)
	    எனில் (STRNCMP(lang, ((char_u *)ga.ga_data) + j, 2) == 0)
		நிறுத்து;
	எனில் (j == ga.ga_len)
	{
	    // New language, add it.
	    எனில் (ga_grow(&ga, 2) == FAIL)
		நிறுத்து;
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[0];
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[1];
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    க்கு (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_data)[j];
	fname[6] = ((char_u *)ga.ga_data)[j + 1];
	எனில் (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	இல்லையெனில்
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

#இல்லையெனில்
    // No language support, just use "*.txt" and "tags".
    helptags_one(dirname, (char_u *)".txt", (char_u *)"tags", add_help_tags,
							    ignore_writeerr);
#endif
}

    நிலையான வெறுமை
helptags_cb(char_u *fname, வெறுமை *cookie)
{
    do_helptags(fname, *(முழு *)cookie, TRUE);
}

/*
 * ":helptags"
 */
    வெறுமை
ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*dirname;
    முழு		add_help_tags = FALSE;

    // Check for ":helptags ++t {dir}".
    எனில் (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	add_help_tags = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    எனில் (STRCMP(eap->arg, "ALL") == 0)
    {
	do_in_path(p_rtp, "", (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &add_help_tags);
    }
    இல்லையெனில்
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	dirname = ExpandOne(&xpc, eap->arg, வெற்று,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	எனில் (dirname == வெற்று || !mch_isdir(dirname))
	    semsg(_(e_not_a_directory_str), eap->arg);
	இல்லையெனில்
	    do_helptags(dirname, add_help_tags, FALSE);
	vim_free(dirname);
    }
}
