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

#include "vim.h"

/*
 * ":help": open a read-only window on a help file
 */
    void
ex_help(exarg_T *eap)
{
    char_u	*arg;
    char_u	*tag;
    FILE	*helpfd;	// file descriptor of help file
    ཧྲིལ		n;
    ཧྲིལ		i;
    win_T	*wp;
    ཧྲིལ		num_matches;
    char_u	**matches;
    char_u	*p;
    ཧྲིལ		empty_fnum = 0;
    ཧྲིལ		alt_fnum = 0;
    buf_T	*buf;
#ifdef FEAT_MULTI_LANG
    ཧྲིལ		len;
    char_u	*lang;
#endif
#ifdef FEAT_FOLDING
    ཧྲིལ		old_KeyTyped = KeyTyped;
#endif

    གལ་ཏེ (ERROR_IF_ANY_POPUP_WINDOW)
	ཕྱིར;

    གལ་ཏེ (eap != NULL)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	ཆེད་དུ (arg = eap->arg; *arg; ++arg)
	{
	    གལ་ཏེ (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		bha_nga;
	    }
	}
	arg = eap->arg;

	གལ་ཏེ (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_(e_dont_panic));
	    ཕྱིར;
	}

	གལ་ཏེ (eap->skip)	    // not executing commands
	    ཕྱིར;
    }
    དེ་མིན
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    སྐབས (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#endif

    // When no argument given go to the index.
    གལ་ཏེ (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = find_help_tags(arg, &num_matches, &matches,
						 eap != NULL && eap->forceit);

    i = 0;
#ifdef FEAT_MULTI_LANG
    གལ་ཏེ (n != FAIL && lang != NULL)
	// Find first item with the requested language.
	ཆེད་དུ (i = 0; i < num_matches; ++i)
	{
	    len = (ཧྲིལ)STRLEN(matches[i]);
	    གལ་ཏེ (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		bha_nga;
	}
#endif
    གལ་ཏེ (i >= num_matches || n == FAIL)
    {
#ifdef FEAT_MULTI_LANG
	གལ་ཏེ (lang != NULL)
	    semsg(_(e_sorry_no_str_help_for_str), lang, arg);
	དེ་མིན
#endif
	    semsg(_(e_sorry_no_help_for_str), arg);
	གལ་ཏེ (n != FAIL)
	    FreeWild(num_matches, matches);
	ཕྱིར;
    }

    // The first match (in the requested language) is the best match.
    tag = vim_strsave(matches[i]);
    FreeWild(num_matches, matches);

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    གལ་ཏེ (!bt_help(curwin->w_buffer) || cmdmod.cmod_tab != 0)
    {
	གལ་ཏེ (cmdmod.cmod_tab != 0)
	    wp = NULL;
	དེ་མིན
	    FOR_ALL_WINDOWS(wp)
		གལ་ཏེ (bt_help(wp->w_buffer))
		    bha_nga;
	གལ་ཏེ (wp != NULL && wp->w_buffer->b_nwindows > 0)
	    win_enter(wp, TRUE);
	དེ་མིན
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    གལ་ཏེ ((helpfd = mch_fopen((var_na *)p_hf, READBIN)) == NULL)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		goto erret;
	    }
	    fclose(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    གལ་ཏེ (cmdmod.cmod_split == 0 && curwin->w_width != Columns
						  && curwin->w_width < 80)
		n |= p_sb ? WSP_BOT : WSP_TOP;
	    གལ་ཏེ (win_split(0, n) == FAIL)
		goto erret;

	    གལ་ཏེ (curwin->w_height < p_hh)
		win_setheight((ཧྲིལ)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (void)do_ecmd(0, NULL, NULL, NULL, ECMD_LASTL,
			  ECMD_HIDE + ECMD_SET_HELP,
			  NULL);  // buffer is still open, don't store info
	    གལ་ཏེ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    empty_fnum = curbuf->b_fnum;
	}
    }

    གལ་ཏེ (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#ifdef FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    KeyTyped = old_KeyTyped;
#endif

    གལ་ཏེ (tag != NULL)
	do_tag(tag, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    གལ་ཏེ (empty_fnum != 0 && curbuf->b_fnum != empty_fnum)
    {
	buf = buflist_findnr(empty_fnum);
	གལ་ཏེ (buf != NULL && buf->b_nwindows == 0)
	    wipe_buffer(buf, TRUE);
    }

    // keep the previous alternate file
    གལ་ཏེ (alt_fnum != 0 && curwin->w_alt_fnum == empty_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(tag);
}

/*
 * ":helpclose": Close one help window
 */
    void
ex_helpclose(exarg_T *eap UNUSED)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	གལ་ཏེ (bt_help(win->w_buffer))
	{
	    win_close(win, FALSE);
	    ཕྱིར;
	}
    }
}

#གལ་ཏེ defined(FEAT_MULTI_LANG)
/*
 * In an argument search for a language specifiers in the form "@xx".
 * Changes the "@" to NUL if found, and returns a pointer to "xx".
 * Returns NULL if not found.
 */
    char_u *
check_help_lang(char_u *arg)
{
    ཧྲིལ len = (ཧྲིལ)STRLEN(arg);

    གལ་ཏེ (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	ཕྱིར arg + len - 2;
    }
    ཕྱིར NULL;
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
    ཧྲིལ
help_heuristic(
    char_u	*matched_string,
    ཧྲིལ		offset,			// offset for match
    ཧྲིལ		wrong_case)		// no matching case
{
    ཧྲིལ		num_letters;
    char_u	*p;

    num_letters = 0;
    ཆེད་དུ (p = matched_string; *p; p++)
	གལ་ཏེ (ASCII_ISALNUM(*p))
	    num_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    གལ་ཏེ (ASCII_ISALNUM(matched_string[offset]) && offset > 0
				 && ASCII_ISALNUM(matched_string[offset - 1]))
	offset += 10000;
    དེ་མིན གལ་ཏེ (offset > 2)
	offset *= 200;
    གལ་ཏེ (wrong_case)
	offset += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    གལ་ཏེ (matched_string[0] == '+' && matched_string[1] != NUL)
	offset += 100;
    ཕྱིར (ཧྲིལ)(100 * num_letters + STRLEN(matched_string) + offset);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    static ཧྲིལ
help_compare(const void *s1, const void *s2)
{
    var_na    *p1;
    var_na    *p2;
    ཧྲིལ	    cmp;

    p1 = *(var_na **)s1 + strlen(*(var_na **)s1) + 1;
    p2 = *(var_na **)s2 + strlen(*(var_na **)s2) + 1;

    // Compare by help heuristic number first.
    cmp = strcmp(p1, p2);
    གལ་ཏེ (cmp != 0)
	ཕྱིར cmp;

    // Compare by strings as tie-breaker when same heuristic number.
    ཕྱིར strcmp(*(var_na **)s1, *(var_na **)s2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * The matches will be sorted with a "best" match algorithm.
 * When "keep_lang" is TRUE try keeping the language of the current buffer.
 */
    ཧྲིལ
find_help_tags(
    char_u	*arg,
    ཧྲིལ		*num_matches,
    char_u	***matches,
    ཧྲིལ		keep_lang)
{
    char_u	*s, *d;
    ཧྲིལ		i;
    // Specific tags that either have a specific replacement or won't go
    // through the generic rules.
    static var_na *(except_tbl[][2]) = {
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
	{NULL, NULL}
    };
    static var_na *(expr_table[]) = {"!=?", "!~?", "<=?", "<?", "==?", "=~?",
				   ">=?", ">?", "is?", "isnot?"};
    ཧྲིལ flags;

    d = IObuff;		    // assume IObuff is long enough!
    d[0] = NUL;

    གལ་ཏེ (STRNICMP(arg, "expr-", 5) == 0)
    {
	// When the string starting with "expr-" and containing '?' and matches
	// the table, it is taken literally (but ~ is escaped).  Otherwise '?'
	// is recognized as a wildcard.
	ཆེད་དུ (i = (ཧྲིལ)ARRAY_LENGTH(expr_table); --i >= 0; )
	    གལ་ཏེ (STRCMP(arg + 5, expr_table[i]) == 0)
	    {
		ཧྲིལ si = 0, di = 0;

		ཆེད་དུ (;;)
		{
		    གལ་ཏེ (arg[si] == '~')
			d[di++] = '\\';
		    d[di++] = arg[si];
		    གལ་ཏེ (arg[si] == NUL)
			bha_nga;
		    ++si;
		}
		bha_nga;
	    }
    }
    དེ་མིན
    {
	// Recognize a few exceptions to the rule.  Some strings that contain
	// '*'are changed to "star", otherwise '*' is recognized as a wildcard.
	ཆེད་དུ (i = 0; except_tbl[i][0] != NULL; ++i)
	    གལ་ཏེ (STRCMP(arg, except_tbl[i][0]) == 0)
	    {
		STRCPY(d, except_tbl[i][1]);
		bha_nga;
	    }
    }

    གལ་ཏེ (d[0] == NUL)	// no match in table
    {
	// Replace "\S" with "/\\S", etc.  Otherwise every tag is matched.
	// Also replace "\%^" and "\%(", they match every tag too.
	// Also "\zs", "\z1", etc.
	// Also "\@<", "\@=", "\@<=", etc.
	// And also "\_$" and "\_^".
	གལ་ཏེ (arg[0] == '\\'
		&& ((arg[1] != NUL && arg[2] == NUL)
		    || (vim_strchr((char_u *)"%_z@", arg[1]) != NULL
							   && arg[2] != NUL)))
	{
	    vim_snprintf((var_na *)d, IOSIZE, "/\\\\%s", arg + 1);
	    // Check for "/\\_$", should be "/\\_\$"
	    གལ་ཏེ (d[3] == '_' && d[4] == '$')
		STRCPY(d + 4, "\\$");
	}
	དེ་མིན
	{
	  // Replace:
	  // "[:...:]" with "\[:...:]"
	  // "[++...]" with "\[++...]"
	  // "\{" with "\\{"		   -- matching "} \}"
	    གལ་ཏེ ((arg[0] == '[' && (arg[1] == ':'
			 || (arg[1] == '+' && arg[2] == '+')))
		    || (arg[0] == '\\' && arg[1] == '{'))
	      *d++ = '\\';

	  // If tag starts with "('", skip the "(". Fixes CTRL-] on ('option'.
	  གལ་ཏེ (*arg == '(' && arg[1] == '\'')
	      arg++;
	  ཆེད་དུ (s = arg; *s; ++s)
	  {
	    // Replace "|" with "bar" and '"' with "quote" to match the name of
	    // the tags for these commands.
	    // Replace "*" with ".*" and "?" with "." to match command line
	    // completion.
	    // Insert a backslash before '~', '$' and '.' to avoid their
	    // special meaning.
	    གལ་ཏེ (d - IObuff > IOSIZE - 10)	// getting too long!?
		bha_nga;
	    switch (*s)
	    {
		pak_sa '|':   STRCPY(d, "bar");
			    d += 3;
			    continue;
		pak_sa '"':   STRCPY(d, "quote");
			    d += 5;
			    continue;
		pak_sa '*':   *d++ = '.';
			    bha_nga;
		pak_sa '?':   *d++ = '.';
			    continue;
		pak_sa '$':
		pak_sa '.':
		pak_sa '~':   *d++ = '\\';
			    bha_nga;
	    }

	    // Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	    // ":help i_^_CTRL-D" work.
	    // Insert '-' before and after "CTRL-X" when applicable.
	    གལ་ཏེ (*s < ' ' || (*s == '^' && s[1] && (ASCII_ISALPHA(s[1])
			   || vim_strchr((char_u *)"?@[\\]^", s[1]) != NULL)))
	    {
		གལ་ཏེ (d > IObuff && d[-1] != '_' && d[-1] != '\\')
		    *d++ = '_';		// prepend a '_' to make x_CTRL-x
		STRCPY(d, "CTRL-");
		d += 5;
		གལ་ཏེ (*s < ' ')
		{
		    *d++ = *s + '@';
		    གལ་ཏེ (d[-1] == '\\')
			*d++ = '\\';	// double a backslash
		}
		དེ་མིན
		    *d++ = *++s;
		གལ་ཏེ (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		// append a '_'
		continue;
	    }
	    དེ་མིན གལ་ཏེ (*s == '^')		// "^" or "CTRL-^" or "^_"
		*d++ = '\\';

	    // Insert a backslash before a backslash after a slash, for search
	    // pattern tags: "/\|" --> "/\\|".
	    དེ་མིན གལ་ཏེ (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    // "CTRL-\_" -> "CTRL-\\_" to avoid the special meaning of "\_" in
	    // "CTRL-\_CTRL-N"
	    གལ་ཏེ (STRNICMP(s, "CTRL-\\_", 7) == 0)
	    {
		STRCPY(d, "CTRL-\\\\");
		d += 7;
		s += 6;
	    }

	    *d++ = *s;

	    // If tag contains "({" or "([", tag terminates at the "(".
	    // This is for help on functions, e.g.: abs({expr}).
	    གལ་ཏེ (*s == '(' && (s[1] == '{' || s[1] =='['))
		bha_nga;

	    // If tag starts with ', toss everything after a second '. Fixes
	    // CTRL-] on 'option'. (would include the trailing '.').
	    གལ་ཏེ (*s == '\'' && s > arg && *arg == '\'')
		bha_nga;
	    // Also '{' and '}'.
	    གལ་ཏེ (*s == '}' && s > arg && *arg == '{')
		bha_nga;
	  }
	  *d = NUL;

	  གལ་ཏེ (*IObuff == '`')
	  {
	      གལ་ཏེ (d > IObuff + 2 && d[-1] == '`')
	      {
		  // remove the backticks from `command`
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-2] = NUL;
	      }
	      དེ་མིན གལ་ཏེ (d > IObuff + 3 && d[-2] == '`' && d[-1] == ',')
	      {
		  // remove the backticks and comma from `command`,
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-3] = NUL;
	      }
	      དེ་མིན གལ་ཏེ (d > IObuff + 4 && d[-3] == '`'
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
    *num_matches = 0;
    flags = TAG_HELP | TAG_REGEXP | TAG_NAMES | TAG_VERBOSE | TAG_NO_TAGFUNC;
    གལ་ཏེ (keep_lang)
	flags |= TAG_KEEP_LANG;
    གལ་ཏེ (find_tags(IObuff, num_matches, matches, flags, (ཧྲིལ)MAXCOL, NULL) == OK
	    && *num_matches > 0)
    {
	// Sort the matches found on the heuristic number that is after the
	// tag name.
	qsort((void *)*matches, (size_t)*num_matches,
					      parimaa_na(char_u *), help_compare);
	// Delete more than TAG_MANY to reduce the size of the listing.
	སྐབས (*num_matches > TAG_MANY)
	    vim_free((*matches)[--*num_matches]);
    }
    ཕྱིར OK;
}

#ifdef FEAT_MULTI_LANG
/*
 * Cleanup matches for help tags:
 * Remove "@ab" if the top of 'helplang' is "ab" and the language of the first
 * tag matches it.  Otherwise remove "@en" if "en" is the only language.
 */
    void
cleanup_help_tags(ཧྲིལ num_file, char_u **file)
{
    ཧྲིལ		i, j;
    ཧྲིལ		len;
    char_u	buf[4];
    char_u	*p = buf;

    གལ་ཏེ (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    ཆེད་དུ (i = 0; i < num_file; ++i)
    {
	len = (ཧྲིལ)STRLEN(file[i]) - 3;
	གལ་ཏེ (len <= 0)
	    continue;
	གལ་ཏེ (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    ཆེད་དུ (j = 0; j < num_file; ++j)
		གལ་ཏེ (j != i && (ཧྲིལ)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    bha_nga;
	    གལ་ཏེ (j == num_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    གལ་ཏེ (*buf != NUL)
	ཆེད་དུ (i = 0; i < num_file; ++i)
	{
	    len = (ཧྲིལ)STRLEN(file[i]) - 3;
	    གལ་ཏེ (len <= 0)
		continue;
	    གལ་ཏེ (STRCMP(file[i] + len, buf) == 0)
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
    void
prepare_help_buffer(void)
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
    གལ་ཏེ (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	set_string_option_direct((char_u *)"isk", -1, p, OPT_FREE|OPT_LOCAL, 0);
	check_buf_options(curbuf);
	(void)buf_init_chartab(curbuf, FALSE);
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
    void
fix_help_buffer(void)
{
    linenr_T	lnum;
    char_u	*line;
    ཧྲིལ		in_example = FALSE;
    ཧྲིལ		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    ཧྲིལ		mustfree;

    // Set filetype to "help" if still needed.
    གལ་ཏེ (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_lock;
	set_option_value_give_err((char_u *)"ft",
					      0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_lock;
    }

#ifdef FEAT_SYN_HL
    གལ་ཏེ (!syntax_present(curwin))
#endif
    {
	ཆེད་དུ (lnum = 1; lnum <= curbuf->b_ml.ml_line_count; ++lnum)
	{
	    line = ml_get_buf(curbuf, lnum, FALSE);
	    len = ml_get_buf_len(curbuf, lnum);
	    གལ་ཏེ (in_example && len > 0 && !VIM_ISWHITE(line[0]))
	    {
		// End of example: non-white or '<' in first column.
		གལ་ཏེ (line[0] == '<')
		{
		    // blank-out a '<' in the first column
		    line = ml_get_buf(curbuf, lnum, TRUE);
		    line[0] = ' ';
		}
		in_example = FALSE;
	    }
	    གལ་ཏེ (!in_example && len > 0)
	    {
		གལ་ཏེ (line[len - 1] == '>' && (len == 1 || line[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    line = ml_get_buf(curbuf, lnum, TRUE);
		    line[len - 1] = ' ';
		    in_example = TRUE;
		}
		དེ་མིན གལ་ཏེ (line[len - 1] == '~')
		{
		    // blank-out a '~' at the end of line (header marker)
		    line = ml_get_buf(curbuf, lnum, TRUE);
		    line[len - 1] = ' ';
		}
	    }
	}
    }

    // In the "help.txt" and "help.abx" file, add the locally added help
    // files.  This uses the very first line in the help file.
    fname = gettail(curbuf->b_fname);
    གལ་ཏེ (fnamecmp(fname, "help.txt") == 0
#ifdef FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#endif
	)
    {
	ཆེད་དུ (lnum = 1; lnum < curbuf->b_ml.ml_line_count; ++lnum)
	{
	    line = ml_get_buf(curbuf, lnum, FALSE);
	    གལ་ཏེ (strstr((var_na *)line, "*local-additions*") == NULL)
		continue;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    སྐབས (*p != NUL)
	    {
		ཧྲིལ NameBufflen;

		NameBufflen = copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		གལ་ཏེ (rt != NULL &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    ཧྲིལ		fcount;
		    char_u	**fnames;
		    FILE	*fd;
		    char_u	*s;
		    ཧྲིལ		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    གལ་ཏེ (*NameBuff != NUL && !after_pathsep(NameBuff, NameBuff + NameBufflen))
		    {
			STRCPY(NameBuff + NameBufflen, PATHSEPSTR);
			NameBufflen += parimaa_na(PATHSEP);
		    }
#ifdef FEAT_MULTI_LANG
		    STRCPY(NameBuff + NameBufflen, "doc/*.??[tx]");
#དེ་མིན
		    STRCPY(NameBuff + NameBufflen, "doc/*.txt");
#endif
		    གལ་ཏེ (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#ifdef FEAT_MULTI_LANG
			ཧྲིལ	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			ཆེད་དུ (i1 = 0; i1 < fcount; ++i1)
			{
			    f1 = fnames[i1];
			    t1 = gettail(f1);
			    e1 = vim_strrchr(t1, '.');
			    གལ་ཏེ (e1 == NULL)
				continue;
			    གལ་ཏེ (fnamecmp(e1, ".txt") != 0
					       && fnamecmp(e1, fname + 4) != 0)
			    {
				// Not .txt and not .abx, remove it.
				VIM_CLEAR(fnames[i1]);
				continue;
			    }

			    ཆེད་དུ (i2 = i1 + 1; i2 < fcount; ++i2)
			    {
				f2 = fnames[i2];
				གལ་ཏེ (f2 == NULL)
				    continue;
				t2 = gettail(f2);
				e2 = vim_strrchr(t2, '.');
				གལ་ཏེ (e2 == NULL)
				    continue;
				གལ་ཏེ (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    continue;
				གལ་ཏེ (fnamecmp(e1, ".txt") == 0
					       && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#endif
			ཆེད་དུ (fi = 0; fi < fcount; ++fi)
			{
			    གལ་ཏེ (fnames[fi] == NULL)
				continue;
			    fd = mch_fopen((var_na *)fnames[fi], "r");
			    གལ་ཏེ (fd != NULL)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				གལ་ཏེ (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != NULL)
				{
				    ཧྲིལ	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    སྐབས (*s != NUL)
				    {
					གལ་ཏེ (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					གལ་ཏེ (*s >= 0x80 && this_utf != FALSE)
					{
					    ཧྲིལ	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    གལ་ཏེ (l == 1)
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
				    གལ་ཏེ (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    དེ་མིན
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    NULL);
					གལ་ཏེ (cp == NULL)
					    cp = IObuff;
				    }
				    convert_setup(&vc, NULL, NULL);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    གལ་ཏེ (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				fclose(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		གལ་ཏེ (mustfree)
		    vim_free(rt);
	    }
	    bha_nga;
	}
    }
}

/*
 * ":exusage"
 */
    void
ex_exusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help ex-cmd-index");
}

/*
 * ":viusage"
 */
    void
ex_viusage(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"help normal-index");
}

/*
 * Generate tags in one help directory.
 */
    static void
helptags_one(
    char_u	*dir,		// doc directory
    char_u	*ext,		// suffix, ".txt", ".itx", ".frx", etc.
    char_u	*tagfname,	// "tags" for English, "tags-fr" for French.
    ཧྲིལ		add_help_tags,	// add "help-tags" tag
    ཧྲིལ		ignore_writeerr)    // ignore write error
{
    FILE	*fd_tags;
    FILE	*fd;
    garray_T	ga;
    ཧྲིལ		res;
    ཧྲིལ		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    ཧྲིལ		fi;
    char_u	*s;
    ཧྲིལ		i;
    char_u	*fname;
    ཧྲིལ		dirlen;
    ཧྲིལ		utf8 = MAYBE;
    ཧྲིལ		this_utf8;
    ཧྲིལ		firstline;
    ཧྲིལ		in_example;
    ཧྲིལ		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (ཧྲིལ)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    res = gen_expand_wildcards(1, &NameBuff, &filecount, &files,
							    EW_FILE|EW_SILENT);
    གལ་ཏེ (res == FAIL || filecount == 0)
    {
	གལ་ཏེ (!got_int)
	    semsg(_(e_no_match_str_1), NameBuff);
	གལ་ཏེ (res != FAIL)
	    FreeWild(filecount, files);
	ཕྱིར;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_tags = mch_fopen((var_na *)NameBuff, "w");
    གལ་ཏེ (fd_tags == NULL)
    {
	གལ་ཏེ (!ignore_writeerr)
	    semsg(_(e_cannot_open_str_for_writing_1), NameBuff);
	FreeWild(filecount, files);
	ཕྱིར;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, parimaa_na(char_u *), 100);
    གལ་ཏེ (add_help_tags || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	གལ་ཏེ (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	དེ་མིན
	{
	    s = alloc(18 + (unsigned)STRLEN(tagfname));
	    གལ་ཏེ (s == NULL)
		got_int = TRUE;
	    དེ་མིན
	    {
		sprintf((var_na *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_data)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    ཆེད་དུ (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((var_na *)files[fi], "r");
	གལ་ཏེ (fd == NULL)
	{
	    semsg(_(e_unable_to_open_str_for_reading), files[fi]);
	    continue;
	}
	fname = files[fi] + dirlen + 1;

	in_example = FALSE;
	firstline = TRUE;
	སྐབས (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    གལ་ཏེ (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		ཆེད་དུ (s = IObuff; *s != NUL; ++s)
		    གལ་ཏེ (*s >= 0x80)
		    {
			ཧྲིལ l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			གལ་ཏེ (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    bha_nga;
			}
			s += l - 1;
		    }
		གལ་ཏེ (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		གལ་ཏེ (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		དེ་མིན གལ་ཏེ (utf8 != this_utf8)
		{
		    semsg(_(e_mix_of_help_file_encodings_within_language_str), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    གལ་ཏེ (in_example)
	    {
		// skip over example; a non-white in the first column ends it
		གལ་ཏེ (vim_strchr((char_u *)" \t\n\r", IObuff[0]))
		    continue;
		in_example = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    སྐབས (p1 != NULL)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		གལ་ཏེ (p2 != NULL && p2 > p1 + 1)	// skip "*" and "**"
		{
		    ཆེད་དུ (s = p1 + 1; s < p2; ++s)
			གལ་ཏེ (*s == ' ' || *s == '\t' || *s == '|')
			    bha_nga;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    གལ་ཏེ (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != NULL
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			གལ་ཏེ (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    bha_nga;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			གལ་ཏེ (s == NULL)
			{
			    got_int = TRUE;
			    bha_nga;
			}
			((char_u **)ga.ga_data)[ga.ga_len] = s;
			++ga.ga_len;
			sprintf((var_na *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    size_t off = STRLEN(IObuff);
	    གལ་ཏེ (off >= 2 && IObuff[off - 1] == '\n')
	    {
		off -= 2;
		སྐབས (off > 0 && (ASCII_ISLOWER(IObuff[off])
						  || VIM_ISDIGIT(IObuff[off])))
		    off--;
		གལ་ཏེ (IObuff[off] == '>' && (off == 0 || IObuff[off - 1] == ' '))
		    in_example = TRUE;
	    }
	    line_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    གལ་ཏེ (!got_int)
    {
	// Sort the tags.
	གལ་ཏེ (ga.ga_data != NULL)
	    sort_strings((char_u **)ga.ga_data, ga.ga_len);

	// Check for duplicates.
	ཆེད་དུ (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_data)[i - 1];
	    p2 = ((char_u **)ga.ga_data)[i];
	    སྐབས (*p1 == *p2)
	    {
		གལ་ཏེ (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((var_na *)NameBuff, MAXPATHL,
			    _(e_duplicate_tag_str_in_file_str_str),
				     ((char_u **)ga.ga_data)[i], dir, p2 + 1);
		    emsg((var_na *)NameBuff);
		    *p2 = '\t';
		    bha_nga;
		}
		++p1;
		++p2;
	    }
	}

	གལ་ཏེ (utf8 == TRUE)
	    fprintf(fd_tags, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	ཆེད་དུ (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_data)[i];
	    གལ་ཏེ (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		fputs((var_na *)s, fd_tags);
	    དེ་མིན
	    {
		fprintf(fd_tags, "%s\t/*", s);
		ཆེད་དུ (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    གལ་ཏེ (*p1 == '\\' || *p1 == '/')
			putc('\\', fd_tags);
		    putc(*p1, fd_tags);
		}
		fprintf(fd_tags, "*\n");
	    }
	}
    }
    གལ་ཏེ (mix)
	got_int = FALSE;    // continue with other languages

    ཆེད་དུ (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_data)[i]);
    ga_clear(&ga);
    fclose(fd_tags);	    // there is no check for an error...
}

/*
 * Generate tags in one help directory, taking care of translations.
 */
    static void
do_helptags(char_u *dirname, ཧྲིལ add_help_tags, ཧྲིལ ignore_writeerr)
{
#ifdef FEAT_MULTI_LANG
    ཧྲིལ		len;
    ཧྲིལ		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    ཧྲིལ		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, dirname);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    གལ་ཏེ (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_(e_no_match_str_1), NameBuff);
	ཕྱིར;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    ཆེད་དུ (i = 0; i < filecount; ++i)
    {
	len = (ཧྲིལ)STRLEN(files[i]);
	གལ་ཏེ (len <= 4)
	    continue;

	གལ་ཏེ (STRICMP(files[i] + len - 4, ".txt") == 0)
	{
	    // ".txt" -> language "en"
	    lang[0] = 'e';
	    lang[1] = 'n';
	}
	དེ་མིན གལ་ཏེ (files[i][len - 4] == '.'
		&& ASCII_ISALPHA(files[i][len - 3])
		&& ASCII_ISALPHA(files[i][len - 2])
		&& TOLOWER_ASC(files[i][len - 1]) == 'x')
	{
	    // ".abx" -> language "ab"
	    lang[0] = TOLOWER_ASC(files[i][len - 3]);
	    lang[1] = TOLOWER_ASC(files[i][len - 2]);
	}
	དེ་མིན
	    continue;

	// Did we find this language already?
	ཆེད་དུ (j = 0; j < ga.ga_len; j += 2)
	    གལ་ཏེ (STRNCMP(lang, ((char_u *)ga.ga_data) + j, 2) == 0)
		bha_nga;
	གལ་ཏེ (j == ga.ga_len)
	{
	    // New language, add it.
	    གལ་ཏེ (ga_grow(&ga, 2) == FAIL)
		bha_nga;
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[0];
	    ((char_u *)ga.ga_data)[ga.ga_len++] = lang[1];
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    ཆེད་དུ (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_data)[j];
	fname[6] = ((char_u *)ga.ga_data)[j + 1];
	གལ་ཏེ (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	དེ་མིན
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

#དེ་མིན
    // No language support, just use "*.txt" and "tags".
    helptags_one(dirname, (char_u *)".txt", (char_u *)"tags", add_help_tags,
							    ignore_writeerr);
#endif
}

    static void
helptags_cb(char_u *fname, void *cookie)
{
    do_helptags(fname, *(ཧྲིལ *)cookie, TRUE);
}

/*
 * ":helptags"
 */
    void
ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*dirname;
    ཧྲིལ		add_help_tags = FALSE;

    // Check for ":helptags ++t {dir}".
    གལ་ཏེ (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	add_help_tags = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    གལ་ཏེ (STRCMP(eap->arg, "ALL") == 0)
    {
	do_in_path(p_rtp, "", (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &add_help_tags);
    }
    དེ་མིན
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	dirname = ExpandOne(&xpc, eap->arg, NULL,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	གལ་ཏེ (dirname == NULL || !mch_isdir(dirname))
	    semsg(_(e_not_a_directory_str), eap->arg);
	དེ་མིན
	    do_helptags(dirname, add_help_tags, FALSE);
	vim_free(dirname);
    }
}
