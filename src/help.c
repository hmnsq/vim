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
    char_u	*ಟ್ಯಾಗ್;
    FILE	*helpfd;	// file descriptor of help file
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		i;
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_matches;
    char_u	**matches;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_fnum = 0;
    ಪೂರ್ಣಾಂಕ		alt_fnum = 0;
    buf_T	*buf;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*lang;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಪೂರ್ಣಾಂಕ		old_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (eap != ಖಾಲಿ)
    {
	// A ":help" command ends at the first LF, or at a '|' that is
	// followed by some text.  Set nextcmd to the following command.
	ಗಾಗಿ (arg = eap->arg; *arg; ++arg)
	{
	    ಆದರೆ (*arg == '\n' || *arg == '\r'
		    || (*arg == '|' && arg[1] != NUL && arg[1] != '|'))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		ನಿಲ್ಲಿಸು;
	    }
	}
	arg = eap->arg;

	ಆದರೆ (eap->forceit && *arg == NUL && !curbuf->b_help)
	{
	    emsg(_(e_dont_panic));
	    ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (eap->skip)	    // not executing commands
	    ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	arg = (char_u *)"";

    // remove trailing blanks
    p = arg + STRLEN(arg) - 1;
    ಆಗ (p > arg && VIM_ISWHITE(*p) && p[-1] != '\\')
	*p-- = NUL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(arg);
#ಆದರೆ_ಅಂತ್ಯ

    // When no argument given go to the index.
    ಆದರೆ (*arg == NUL)
	arg = (char_u *)"help.txt";

    // Check if there is a match for the argument.
    n = ಕಂಡುಹಿಡಿ_help_ಟ್ಯಾಗುಗಳು(arg, &ಅಂಕೆ_matches, &matches,
						 eap != ಖಾಲಿ && eap->forceit);

    i = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    ಆದರೆ (n != FAIL && lang != ಖಾಲಿ)
	// Find first item with the requested language.
	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_matches; ++i)
	{
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(matches[i]);
	    ಆದರೆ (len > 3 && matches[i][len - 3] == '@'
				  && STRICMP(matches[i] + len - 2, lang) == 0)
		ನಿಲ್ಲಿಸು;
	}
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (i >= ಅಂಕೆ_matches || n == FAIL)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	ಆದರೆ (lang != ಖಾಲಿ)
	    semsg(_(e_sorry_no_str_help_for_str), lang, arg);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    semsg(_(e_sorry_no_help_for_str), arg);
	ಆದರೆ (n != FAIL)
	    FreeWild(ಅಂಕೆ_matches, matches);
	ಹಿಂತಿರುಗಿ;
    }

    // The first match (in the requested language) is the best match.
    ಟ್ಯಾಗ್ = vim_strsave(matches[i]);
    FreeWild(ಅಂಕೆ_matches, matches);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    // Re-use an existing help window or open a new one.
    // Always open a new one for ":tab help".
    ಆದರೆ (!bt_help(curwin->w_ಬಫರ್) || cmdmod.cmod_tab != 0)
    {
	ಆದರೆ (cmdmod.cmod_tab != 0)
	    wp = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    FOR_ALL_WINDOWS(wp)
		ಆದರೆ (bt_help(wp->w_ಬಫರ್))
		    ನಿಲ್ಲಿಸು;
	ಆದರೆ (wp != ಖಾಲಿ && wp->w_ಬಫರ್->b_nwindows > 0)
	    win_enter(wp, TRUE);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // There is no help window yet.
	    // Try to open the file specified by the "helpfile" option.
	    ಆದರೆ ((helpfd = mch_fopen((ಅಕ್ಷರ *)p_hf, READBIN)) == ಖಾಲಿ)
	    {
		smsg(_("Sorry, help file \"%s\" not found"), p_hf);
		ಹೋಗು erret;
	    }
	    fclose(helpfd);

	    // Split off help window; put it at far top if no position
	    // specified, the current window is vertically split and
	    // narrow.
	    n = WSP_HELP;
	    ಆದರೆ (cmdmod.cmod_ವಿಭಜಿಸು == 0 && curwin->w_ಅಗಲ != Columns
						  && curwin->w_ಅಗಲ < 80)
		n |= p_sb ? WSP_BOT : WSP_ತುದಿ;
	    ಆದರೆ (win_ವಿಭಜಿಸು(0, n) == FAIL)
		ಹೋಗು erret;

	    ಆದರೆ (curwin->w_ಎತ್ತರ < p_hh)
		win_setheight((ಪೂರ್ಣಾಂಕ)p_hh);

	    // Open help file (do_ecmd() will set b_help flag, readfile() will
	    // set b_p_ro flag).
	    // Set the alternate file to the previously edited file.
	    alt_fnum = curbuf->b_fnum;
	    (ಶೂನ್ಯ)do_ecmd(0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ECMD_LASTL,
			  ECMD_HIDE + ECMD_ಗಣ_HELP,
			  ಖಾಲಿ);  // buffer is still open, don't store info
	    ಆದರೆ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = alt_fnum;
	    ಖಾಲಿಗಳು_fnum = curbuf->b_fnum;
	}
    }

    ಆದರೆ (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // Restore KeyTyped, setting 'filetype=help' may reset it.
    // It is needed for do_tag top open folds under the cursor.
    ಕೀಲಿ_Typed = old_ಕೀಲಿ_Typed;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಟ್ಯಾಗ್ != ಖಾಲಿ)
	do_ಟ್ಯಾಗ್(ಟ್ಯಾಗ್, DT_HELP, 1, FALSE, TRUE);

    // Delete the empty buffer if we're not using it.  Careful: autocommands
    // may have jumped to another window, check that the buffer is not in a
    // window.
    ಆದರೆ (ಖಾಲಿಗಳು_fnum != 0 && curbuf->b_fnum != ಖಾಲಿಗಳು_fnum)
    {
	buf = buflist_findnr(ಖಾಲಿಗಳು_fnum);
	ಆದರೆ (buf != ಖಾಲಿ && buf->b_nwindows == 0)
	    wipe_ಬಫರ್(buf, TRUE);
    }

    // keep the previous alternate file
    ಆದರೆ (alt_fnum != 0 && curwin->w_alt_fnum == ಖಾಲಿಗಳು_fnum
				    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = alt_fnum;

erret:
    vim_free(ಟ್ಯಾಗ್);
}

/*
 * ":helpclose": Close one help window
 */
    ಶೂನ್ಯ
ex_helpclose(exarg_T *eap ಬಳಸದ)
{
    win_T *win;

    FOR_ALL_WINDOWS(win)
    {
	ಆದರೆ (bt_help(win->w_ಬಫರ್))
	{
	    win_close(win, FALSE);
	    ಹಿಂತಿರುಗಿ;
	}
    }
}

#ಆದರೆ defined(FEAT_MULTI_LANG)
/*
 * In an argument search for a language specifiers in the form "@xx".
 * Changes the "@" to NUL if found, and returns a pointer to "xx".
 * Returns NULL if not found.
 */
    char_u *
check_help_lang(char_u *arg)
{
    ಪೂರ್ಣಾಂಕ len = (ಪೂರ್ಣಾಂಕ)STRLEN(arg);

    ಆದರೆ (len >= 3 && arg[len - 3] == '@' && ASCII_ISALPHA(arg[len - 2])
					       && ASCII_ISALPHA(arg[len - 1]))
    {
	arg[len - 3] = NUL;		// remove the '@'
	ಹಿಂತಿರುಗಿ arg + len - 2;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
help_heuristic(
    char_u	*matched_string,
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ,			// offset for match
    ಪೂರ್ಣಾಂಕ		wrong_case)		// no matching case
{
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_letters;
    char_u	*p;

    ಅಂಕೆ_letters = 0;
    ಗಾಗಿ (p = matched_string; *p; p++)
	ಆದರೆ (ASCII_ISALNUM(*p))
	    ಅಂಕೆ_letters++;

    // Multiply the number of letters by 100 to give it a much bigger
    // weighting than the number of characters.
    // If there only is a match while ignoring case, add 5000.
    // If the match starts in the middle of a word, add 10000 to put it
    // somewhere in the last half.
    // If the match is more than 2 chars from the start, multiply by 200 to
    // put it after matches at the start.
    ಆದರೆ (ASCII_ISALNUM(matched_string[ಸ್ಥಾನಾಂತರ]) && ಸ್ಥಾನಾಂತರ > 0
				 && ASCII_ISALNUM(matched_string[ಸ್ಥಾನಾಂತರ - 1]))
	ಸ್ಥಾನಾಂತರ += 10000;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸ್ಥಾನಾಂತರ > 2)
	ಸ್ಥಾನಾಂತರ *= 200;
    ಆದರೆ (wrong_case)
	ಸ್ಥಾನಾಂತರ += 5000;
    // Features are less interesting than the subjects themselves, but "+"
    // alone is not a feature.
    ಆದರೆ (matched_string[0] == '+' && matched_string[1] != NUL)
	ಸ್ಥಾನಾಂತರ += 100;
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(100 * ಅಂಕೆ_letters + STRLEN(matched_string) + ಸ್ಥಾನಾಂತರ);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
help_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ಅಕ್ಷರ    *p1;
    ಅಕ್ಷರ    *p2;
    ಪೂರ್ಣಾಂಕ	    cmp;

    p1 = *(ಅಕ್ಷರ **)s1 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*(ಅಕ್ಷರ **)s1) + 1;
    p2 = *(ಅಕ್ಷರ **)s2 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*(ಅಕ್ಷರ **)s2) + 1;

    // Compare by help heuristic number first.
    cmp = strcmp(p1, p2);
    ಆದರೆ (cmp != 0)
	ಹಿಂತಿರುಗಿ cmp;

    // Compare by strings as tie-breaker when same heuristic number.
    ಹಿಂತಿರುಗಿ strcmp(*(ಅಕ್ಷರ **)s1, *(ಅಕ್ಷರ **)s2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * The matches will be sorted with a "best" match algorithm.
 * When "keep_lang" is TRUE try keeping the language of the current buffer.
 */
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_help_ಟ್ಯಾಗುಗಳು(
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_matches,
    char_u	***matches,
    ಪೂರ್ಣಾಂಕ		keep_lang)
{
    char_u	*s, *d;
    ಪೂರ್ಣಾಂಕ		i;
    // Specific tags that either have a specific replacement or won't go
    // through the generic rules.
    ಸ್ಥಿರ ಅಕ್ಷರ *(except_tbl[][2]) = {
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
	{ಖಾಲಿ, ಖಾಲಿ}
    };
    ಸ್ಥಿರ ಅಕ್ಷರ *(ಅಭಿವ್ಯಕ್ತಿ_ಕೋಷ್ಟಕ[]) = {"!=?", "!~?", "<=?", "<?", "==?", "=~?",
				   ">=?", ">?", "is?", "isnot?"};
    ಪೂರ್ಣಾಂಕ flags;

    d = IObuff;		    // assume IObuff is long enough!
    d[0] = NUL;

    ಆದರೆ (STRNICMP(arg, "expr-", 5) == 0)
    {
	// When the string starting with "expr-" and containing '?' and matches
	// the table, it is taken literally (but ~ is escaped).  Otherwise '?'
	// is recognized as a wildcard.
	ಗಾಗಿ (i = (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(ಅಭಿವ್ಯಕ್ತಿ_ಕೋಷ್ಟಕ); --i >= 0; )
	    ಆದರೆ (STRCMP(arg + 5, ಅಭಿವ್ಯಕ್ತಿ_ಕೋಷ್ಟಕ[i]) == 0)
	    {
		ಪೂರ್ಣಾಂಕ si = 0, di = 0;

		ಗಾಗಿ (;;)
		{
		    ಆದರೆ (arg[si] == '~')
			d[di++] = '\\';
		    d[di++] = arg[si];
		    ಆದರೆ (arg[si] == NUL)
			ನಿಲ್ಲಿಸು;
		    ++si;
		}
		ನಿಲ್ಲಿಸು;
	    }
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Recognize a few exceptions to the rule.  Some strings that contain
	// '*'are changed to "star", otherwise '*' is recognized as a wildcard.
	ಗಾಗಿ (i = 0; except_tbl[i][0] != ಖಾಲಿ; ++i)
	    ಆದರೆ (STRCMP(arg, except_tbl[i][0]) == 0)
	    {
		STRCPY(d, except_tbl[i][1]);
		ನಿಲ್ಲಿಸು;
	    }
    }

    ಆದರೆ (d[0] == NUL)	// no match in table
    {
	// Replace "\S" with "/\\S", etc.  Otherwise every tag is matched.
	// Also replace "\%^" and "\%(", they match every tag too.
	// Also "\zs", "\z1", etc.
	// Also "\@<", "\@=", "\@<=", etc.
	// And also "\_$" and "\_^".
	ಆದರೆ (arg[0] == '\\'
		&& ((arg[1] != NUL && arg[2] == NUL)
		    || (vim_strchr((char_u *)"%_z@", arg[1]) != ಖಾಲಿ
							   && arg[2] != NUL)))
	{
	    vim_snprintf((ಅಕ್ಷರ *)d, IOSIZE, "/\\\\%s", arg + 1);
	    // Check for "/\\_$", should be "/\\_\$"
	    ಆದರೆ (d[3] == '_' && d[4] == '$')
		STRCPY(d + 4, "\\$");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	  // Replace:
	  // "[:...:]" with "\[:...:]"
	  // "[++...]" with "\[++...]"
	  // "\{" with "\\{"		   -- matching "} \}"
	    ಆದರೆ ((arg[0] == '[' && (arg[1] == ':'
			 || (arg[1] == '+' && arg[2] == '+')))
		    || (arg[0] == '\\' && arg[1] == '{'))
	      *d++ = '\\';

	  // If tag starts with "('", skip the "(". Fixes CTRL-] on ('option'.
	  ಆದರೆ (*arg == '(' && arg[1] == '\'')
	      arg++;
	  ಗಾಗಿ (s = arg; *s; ++s)
	  {
	    // Replace "|" with "bar" and '"' with "quote" to match the name of
	    // the tags for these commands.
	    // Replace "*" with ".*" and "?" with "." to match command line
	    // completion.
	    // Insert a backslash before '~', '$' and '.' to avoid their
	    // special meaning.
	    ಆದರೆ (d - IObuff > IOSIZE - 10)	// getting too long!?
		ನಿಲ್ಲಿಸು;
	    ಆಯ್ಕೆ (*s)
	    {
		ಸ್ಥಿತಿ '|':   STRCPY(d, "bar");
			    d += 3;
			    ಮುಂದುವರಿ;
		ಸ್ಥಿತಿ '"':   STRCPY(d, "quote");
			    d += 5;
			    ಮುಂದುವರಿ;
		ಸ್ಥಿತಿ '*':   *d++ = '.';
			    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '?':   *d++ = '.';
			    ಮುಂದುವರಿ;
		ಸ್ಥಿತಿ '$':
		ಸ್ಥಿತಿ '.':
		ಸ್ಥಿತಿ '~':   *d++ = '\\';
			    ನಿಲ್ಲಿಸು;
	    }

	    // Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	    // ":help i_^_CTRL-D" work.
	    // Insert '-' before and after "CTRL-X" when applicable.
	    ಆದರೆ (*s < ' ' || (*s == '^' && s[1] && (ASCII_ISALPHA(s[1])
			   || vim_strchr((char_u *)"?@[\\]^", s[1]) != ಖಾಲಿ)))
	    {
		ಆದರೆ (d > IObuff && d[-1] != '_' && d[-1] != '\\')
		    *d++ = '_';		// prepend a '_' to make x_CTRL-x
		STRCPY(d, "CTRL-");
		d += 5;
		ಆದರೆ (*s < ' ')
		{
		    *d++ = *s + '@';
		    ಆದರೆ (d[-1] == '\\')
			*d++ = '\\';	// double a backslash
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    *d++ = *++s;
		ಆದರೆ (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		// append a '_'
		ಮುಂದುವರಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '^')		// "^" or "CTRL-^" or "^_"
		*d++ = '\\';

	    // Insert a backslash before a backslash after a slash, for search
	    // pattern tags: "/\|" --> "/\\|".
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    // "CTRL-\_" -> "CTRL-\\_" to avoid the special meaning of "\_" in
	    // "CTRL-\_CTRL-N"
	    ಆದರೆ (STRNICMP(s, "CTRL-\\_", 7) == 0)
	    {
		STRCPY(d, "CTRL-\\\\");
		d += 7;
		s += 6;
	    }

	    *d++ = *s;

	    // If tag contains "({" or "([", tag terminates at the "(".
	    // This is for help on functions, e.g.: abs({expr}).
	    ಆದರೆ (*s == '(' && (s[1] == '{' || s[1] =='['))
		ನಿಲ್ಲಿಸು;

	    // If tag starts with ', toss everything after a second '. Fixes
	    // CTRL-] on 'option'. (would include the trailing '.').
	    ಆದರೆ (*s == '\'' && s > arg && *arg == '\'')
		ನಿಲ್ಲಿಸು;
	    // Also '{' and '}'.
	    ಆದರೆ (*s == '}' && s > arg && *arg == '{')
		ನಿಲ್ಲಿಸು;
	  }
	  *d = NUL;

	  ಆದರೆ (*IObuff == '`')
	  {
	      ಆದರೆ (d > IObuff + 2 && d[-1] == '`')
	      {
		  // remove the backticks from `command`
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-2] = NUL;
	      }
	      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d > IObuff + 3 && d[-2] == '`' && d[-1] == ',')
	      {
		  // remove the backticks and comma from `command`,
		  mch_memmove(IObuff, IObuff + 1, STRLEN(IObuff));
		  d[-3] = NUL;
	      }
	      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d > IObuff + 4 && d[-3] == '`'
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
    flags = ಟ್ಯಾಗ್_HELP | ಟ್ಯಾಗ್_REGEXP | ಟ್ಯಾಗ್_ಹೆಸರುಗಳು | ಟ್ಯಾಗ್_VERBOSE | ಟ್ಯಾಗ್_NO_TAGFUNC;
    ಆದರೆ (keep_lang)
	flags |= ಟ್ಯಾಗ್_KEEP_LANG;
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗುಗಳು(IObuff, ಅಂಕೆ_matches, matches, flags, (ಪೂರ್ಣಾಂಕ)MAXCOL, ಖಾಲಿ) == OK
	    && *ಅಂಕೆ_matches > 0)
    {
	// Sort the matches found on the heuristic number that is after the
	// tag name.
	qsort((ಶೂನ್ಯ *)*matches, (size_t)*ಅಂಕೆ_matches,
					      ಗಾತ್ರ(char_u *), help_compare);
	// Delete more than TAG_MANY to reduce the size of the listing.
	ಆಗ (*ಅಂಕೆ_matches > ಟ್ಯಾಗ್_MANY)
	    vim_free((*matches)[--*ಅಂಕೆ_matches]);
    }
    ಹಿಂತಿರುಗಿ OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
/*
 * Cleanup matches for help tags:
 * Remove "@ab" if the top of 'helplang' is "ab" and the language of the first
 * tag matches it.  Otherwise remove "@en" if "en" is the only language.
 */
    ಶೂನ್ಯ
cleanup_help_ಟ್ಯಾಗುಗಳು(ಪೂರ್ಣಾಂಕ ಅಂಕೆ_file, char_u **file)
{
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	buf[4];
    char_u	*p = buf;

    ಆದರೆ (p_hlg[0] != NUL && (p_hlg[0] != 'e' || p_hlg[1] != 'n'))
    {
	*p++ = '@';
	*p++ = p_hlg[0];
	*p++ = p_hlg[1];
    }
    *p = NUL;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_file; ++i)
    {
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(file[i]) - 3;
	ಆದರೆ (len <= 0)
	    ಮುಂದುವರಿ;
	ಆದರೆ (STRCMP(file[i] + len, "@en") == 0)
	{
	    // Sorting on priority means the same item in another language may
	    // be anywhere.  Search all items for a match up to the "@en".
	    ಗಾಗಿ (j = 0; j < ಅಂಕೆ_file; ++j)
		ಆದರೆ (j != i && (ಪೂರ್ಣಾಂಕ)STRLEN(file[j]) == len + 3
			   && STRNCMP(file[i], file[j], len + 1) == 0)
		    ನಿಲ್ಲಿಸು;
	    ಆದರೆ (j == ಅಂಕೆ_file)
		// item only exists with @en, remove it
		file[i][len] = NUL;
	}
    }

    ಆದರೆ (*buf != NUL)
	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_file; ++i)
	{
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(file[i]) - 3;
	    ಆದರೆ (len <= 0)
		ಮುಂದುವರಿ;
	    ಆದರೆ (STRCMP(file[i] + len, buf) == 0)
	    {
		// remove the default language
		file[i][len] = NUL;
	    }
	}
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Called when starting to edit a buffer for a help file.
 */
    ಶೂನ್ಯ
prepare_help_ಬಫರ್(ಶೂನ್ಯ)
{
    char_u	*p;

    curbuf->b_help = true;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    ಗಣ_string_option_ನೇರ((char_u *)"buftype", -1,
				     (char_u *)"help", OPT_FREE|OPT_LOCAL, 0);
#ಆದರೆ_ಅಂತ್ಯ

    // Always set these options after jumping to a help tag, because the
    // user may have an autocommand that gets in the way.
    // When adding an option here, also update the help file helphelp.txt.

    // Accept all ASCII chars for keywords, except ' ', '*', '"', '|', and
    // latin1 word characters (for translated help files).
    // Only set it when needed, buf_init_chartab() is some work.
    p = (char_u *)"!-~,^*,^|,^\",192-255";
    ಆದರೆ (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	ಗಣ_string_option_ನೇರ((char_u *)"isk", -1, p, OPT_FREE|OPT_LOCAL, 0);
	check_buf_options(curbuf);
	(ಶೂನ್ಯ)buf_init_chartab(curbuf, FALSE);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // Don't use the global foldmethod.
    ಗಣ_string_option_ನೇರ((char_u *)"fdm", -1, (char_u *)"manual",
						       OPT_FREE|OPT_LOCAL, 0);
#ಆದರೆ_ಅಂತ್ಯ

    curbuf->b_p_ts = 8;		// 'tabstop' is 8
    curwin->w_p_list = FALSE;	// no list mode

    curbuf->b_p_ma = FALSE;	// not modifiable
    curbuf->b_p_bin = FALSE;	// reset 'bin' before reading file
    curwin->w_p_nu = 0;		// no line numbers
    curwin->w_p_rnu = 0;	// no relative line numbers
    RESET_BINDING(curwin);	// no scroll or cursor binding
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
    curwin->w_p_arab = FALSE;	// no arabic mode
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    curwin->w_p_rl  = FALSE;	// help window is left-to-right
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    curwin->w_p_fen = FALSE;	// No folding in the help window
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    curwin->w_p_diff = FALSE;	// No 'diff'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    curwin->w_p_spell = FALSE;	// No spell checking
#ಆದರೆ_ಅಂತ್ಯ

    ಗಣ_buflisted(FALSE);
}

/*
 * After reading a help file: May cleanup a help buffer when syntax
 * highlighting is not used.
 */
    ಶೂನ್ಯ
fix_help_ಬಫರ್(ಶೂನ್ಯ)
{
    linenr_T	lnum;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಒಳ_example = FALSE;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*fname;
    char_u	*p;
    char_u	*rt;
    ಪೂರ್ಣಾಂಕ		mustfree;

    // Set filetype to "help" if still needed.
    ಆದರೆ (STRCMP(curbuf->b_p_ft, "help") != 0)
    {
	++curbuf_ಬೀಗ;
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"ft",
					      0L, (char_u *)"help", OPT_LOCAL);
	--curbuf_ಬೀಗ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಆದರೆ (!syntax_present(curwin))
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಗಾಗಿ (lnum = 1; lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
	{
	    ಸಾಲು = ml_get_buf(curbuf, lnum, FALSE);
	    len = ml_get_buf_len(curbuf, lnum);
	    ಆದರೆ (ಒಳ_example && len > 0 && !VIM_ISWHITE(ಸಾಲು[0]))
	    {
		// End of example: non-white or '<' in first column.
		ಆದರೆ (ಸಾಲು[0] == '<')
		{
		    // blank-out a '<' in the first column
		    ಸಾಲು = ml_get_buf(curbuf, lnum, TRUE);
		    ಸಾಲು[0] = ' ';
		}
		ಒಳ_example = FALSE;
	    }
	    ಆದರೆ (!ಒಳ_example && len > 0)
	    {
		ಆದರೆ (ಸಾಲು[len - 1] == '>' && (len == 1 || ಸಾಲು[len - 2] == ' '))
		{
		    // blank-out a '>' in the last column (start of example)
		    ಸಾಲು = ml_get_buf(curbuf, lnum, TRUE);
		    ಸಾಲು[len - 1] = ' ';
		    ಒಳ_example = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಾಲು[len - 1] == '~')
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
    ಆದರೆ (fnamecmp(fname, "help.txt") == 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	|| (fnamencmp(fname, "help.", 5) == 0
	    && ASCII_ISALPHA(fname[5])
	    && ASCII_ISALPHA(fname[6])
	    && TOLOWER_ASC(fname[7]) == 'x'
	    && fname[8] == NUL)
#ಆದರೆ_ಅಂತ್ಯ
	)
    {
	ಗಾಗಿ (lnum = 1; lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
	{
	    ಸಾಲು = ml_get_buf(curbuf, lnum, FALSE);
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)ಸಾಲು, "*local-additions*") == ಖಾಲಿ)
		ಮುಂದುವರಿ;

	    // Go through all directories in 'runtimepath', skipping
	    // $VIMRUNTIME.
	    p = p_rtp;
	    ಆಗ (*p != NUL)
	    {
		ಪೂರ್ಣಾಂಕ NameBufflen;

		NameBufflen = copy_option_part(&p, NameBuff, MAXPATHL, ",");
		mustfree = FALSE;
		rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		ಆದರೆ (rt != ಖಾಲಿ &&
			    fullpathcmp(rt, NameBuff, FALSE, TRUE) != FPC_SAME)
		{
		    ಪೂರ್ಣಾಂಕ		fcount;
		    char_u	**fnames;
		    FILE	*fd;
		    char_u	*s;
		    ಪೂರ್ಣಾಂಕ		fi;
		    vimconv_T	vc;
		    char_u	*cp;

		    // Find all "doc/ *.txt" files in this directory.
		    ಆದರೆ (*NameBuff != NUL && !after_pathsep(NameBuff, NameBuff + NameBufflen))
		    {
			STRCPY(NameBuff + NameBufflen, PATHSEPSTR);
			NameBufflen += ಗಾತ್ರ(PATHSEP);
		    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
		    STRCPY(NameBuff + NameBufflen, "doc/*.??[tx]");
#ಇಲ್ಲದಿದ್ದರೆ
		    STRCPY(NameBuff + NameBufflen, "doc/*.txt");
#ಆದರೆ_ಅಂತ್ಯ
		    ಆದರೆ (gen_expand_wildcards(1, &NameBuff, &fcount,
					 &fnames, EW_FILE|EW_SILENT) == OK
			    && fcount > 0)
		    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
			ಪೂರ್ಣಾಂಕ	i1, i2;
			char_u	*f1, *f2;
			char_u	*t1, *t2;
			char_u	*e1, *e2;

			// If foo.abx is found use it instead of foo.txt in
			// the same directory.
			ಗಾಗಿ (i1 = 0; i1 < fcount; ++i1)
			{
			    f1 = fnames[i1];
			    t1 = gettail(f1);
			    e1 = vim_strrchr(t1, '.');
			    ಆದರೆ (e1 == ಖಾಲಿ)
				ಮುಂದುವರಿ;
			    ಆದರೆ (fnamecmp(e1, ".txt") != 0
					       && fnamecmp(e1, fname + 4) != 0)
			    {
				// Not .txt and not .abx, remove it.
				VIM_CLEAR(fnames[i1]);
				ಮುಂದುವರಿ;
			    }

			    ಗಾಗಿ (i2 = i1 + 1; i2 < fcount; ++i2)
			    {
				f2 = fnames[i2];
				ಆದರೆ (f2 == ಖಾಲಿ)
				    ಮುಂದುವರಿ;
				t2 = gettail(f2);
				e2 = vim_strrchr(t2, '.');
				ಆದರೆ (e2 == ಖಾಲಿ)
				    ಮುಂದುವರಿ;
				ಆದರೆ (e1 - f1 != e2 - f2
					    || fnamencmp(f1, f2, e1 - f1) != 0)
				    ಮುಂದುವರಿ;
				ಆದರೆ (fnamecmp(e1, ".txt") == 0
					       && fnamecmp(e2, fname + 4) == 0)
				    // use .abx instead of .txt
				    VIM_CLEAR(fnames[i1]);
			    }
			}
#ಆದರೆ_ಅಂತ್ಯ
			ಗಾಗಿ (fi = 0; fi < fcount; ++fi)
			{
			    ಆದರೆ (fnames[fi] == ಖಾಲಿ)
				ಮುಂದುವರಿ;
			    fd = mch_fopen((ಅಕ್ಷರ *)fnames[fi], "r");
			    ಆದರೆ (fd != ಖಾಲಿ)
			    {
				vim_fgets(IObuff, IOSIZE, fd);
				ಆದರೆ (IObuff[0] == '*'
					&& (s = vim_strchr(IObuff + 1, '*'))
								  != ಖಾಲಿ)
				{
				    ಪೂರ್ಣಾಂಕ	this_utf = MAYBE;

				    // Change tag definition to a
				    // reference and remove <CR>/<NL>.
				    IObuff[0] = '|';
				    *s = '|';
				    ಆಗ (*s != NUL)
				    {
					ಆದರೆ (*s == '\r' || *s == '\n')
					    *s = NUL;
					// The text is utf-8 when a byte
					// above 127 is found and no
					// illegal byte sequence is found.
					ಆದರೆ (*s >= 0x80 && this_utf != FALSE)
					{
					    ಪೂರ್ಣಾಂಕ	l;

					    this_utf = TRUE;
					    l = utf_ptr2len(s);
					    ಆದರೆ (l == 1)
						this_utf = FALSE;
					    s += l - 1;
					}
					++s;
				    }

				    // The help file is latin1 or utf-8;
				    // conversion to the current
				    // 'encoding' may be required.
				    vc.vc_type = CONV_NONE;
				    convert_ಸಿದ್ಧತೆ(&vc, (char_u *)(
						this_utf == TRUE ? "utf-8"
						      : "latin1"), p_enc);
				    ಆದರೆ (vc.vc_type == CONV_NONE)
					// No conversion needed.
					cp = IObuff;
				    ಇಲ್ಲದಿದ್ದರೆ
				    {
					// Do the conversion.  If it fails
					// use the unconverted text.
					cp = string_convert(&vc, IObuff,
								    ಖಾಲಿ);
					ಆದರೆ (cp == ಖಾಲಿ)
					    cp = IObuff;
				    }
				    convert_ಸಿದ್ಧತೆ(&vc, ಖಾಲಿ, ಖಾಲಿ);

				    ml_append(lnum, cp, (colnr_T)0, FALSE);
				    ಆದರೆ (cp != IObuff)
					vim_free(cp);
				    ++lnum;
				}
				fclose(fd);
			    }
			}
			FreeWild(fcount, fnames);
		    }
		}
		ಆದರೆ (mustfree)
		    vim_free(rt);
	    }
	    ನಿಲ್ಲಿಸು;
	}
    }
}

/*
 * ":exusage"
 */
    ಶೂನ್ಯ
ex_exusage(exarg_T *eap ಬಳಸದ)
{
    do_cmdline_cmd((char_u *)"help ex-cmd-index");
}

/*
 * ":viusage"
 */
    ಶೂನ್ಯ
ex_viusage(exarg_T *eap ಬಳಸದ)
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
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_help_ಟ್ಯಾಗುಗಳು,	// add "help-tags" tag
    ಪೂರ್ಣಾಂಕ		ignore_writeerr)    // ignore write error
{
    FILE	*fd_ಟ್ಯಾಗುಗಳು;
    FILE	*fd;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		res;
    ಪೂರ್ಣಾಂಕ		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    ಪೂರ್ಣಾಂಕ		fi;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*fname;
    ಪೂರ್ಣಾಂಕ		dirlen;
    ಪೂರ್ಣಾಂಕ		utf8 = MAYBE;
    ಪೂರ್ಣಾಂಕ		this_utf8;
    ಪೂರ್ಣಾಂಕ		firstline;
    ಪೂರ್ಣಾಂಕ		ಒಳ_example;
    ಪೂರ್ಣಾಂಕ		mix = FALSE;	// detected mixed encodings

    // Find all *.txt files.
    dirlen = (ಪೂರ್ಣಾಂಕ)STRLEN(dir);
    STRCPY(NameBuff, dir);
    STRCAT(NameBuff, "/**/*");
    STRCAT(NameBuff, ext);
    res = gen_expand_wildcards(1, &NameBuff, &filecount, &files,
							    EW_FILE|EW_SILENT);
    ಆದರೆ (res == FAIL || filecount == 0)
    {
	ಆದರೆ (!got_int)
	    semsg(_(e_no_ಹೊಂದಿಕೆ_str_1), NameBuff);
	ಆದರೆ (res != FAIL)
	    FreeWild(filecount, files);
	ಹಿಂತಿರುಗಿ;
    }

    // Open the tags file for writing.
    // Do this before scanning through all the files.
    STRCPY(NameBuff, dir);
    ಕೂಡಿಸು_pathsep(NameBuff);
    STRCAT(NameBuff, tagfname);
    fd_ಟ್ಯಾಗುಗಳು = mch_fopen((ಅಕ್ಷರ *)NameBuff, "w");
    ಆದರೆ (fd_ಟ್ಯಾಗುಗಳು == ಖಾಲಿ)
    {
	ಆದರೆ (!ignore_writeerr)
	    semsg(_(e_cannot_open_str_for_writing_1), NameBuff);
	FreeWild(filecount, files);
	ಹಿಂತಿರುಗಿ;
    }

    // If using the "++t" argument or generating tags for "$VIMRUNTIME/doc"
    // add the "help-tags" tag.
    ga_init2(&ga, ಗಾತ್ರ(char_u *), 100);
    ಆದರೆ (ಕೂಡಿಸು_help_ಟ್ಯಾಗುಗಳು || fullpathcmp((char_u *)"$VIMRUNTIME/doc",
						dir, FALSE, TRUE) == FPC_SAME)
    {
	ಆದರೆ (ga_grow(&ga, 1) == FAIL)
	    got_int = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    s = alloc(18 + (ಚಿಹ್ನೆರಹಿತ)STRLEN(tagfname));
	    ಆದರೆ (s == ಖಾಲಿ)
		got_int = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		sprintf((ಅಕ್ಷರ *)s, "help-tags\t%s\t1\n", tagfname);
		((char_u **)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len] = s;
		++ga.ga_len;
	    }
	}
    }

    // Go over all the files and extract the tags.
    ಗಾಗಿ (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = mch_fopen((ಅಕ್ಷರ *)files[fi], "r");
	ಆದರೆ (fd == ಖಾಲಿ)
	{
	    semsg(_(e_unable_ಗಮ್ಯ_open_str_for_reading), files[fi]);
	    ಮುಂದುವರಿ;
	}
	fname = files[fi] + dirlen + 1;

	ಒಳ_example = FALSE;
	firstline = TRUE;
	ಆಗ (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    ಆದರೆ (firstline)
	    {
		// Detect utf-8 file by a non-ASCII char in the first line.
		this_utf8 = MAYBE;
		ಗಾಗಿ (s = IObuff; *s != NUL; ++s)
		    ಆದರೆ (*s >= 0x80)
		    {
			ಪೂರ್ಣಾಂಕ l;

			this_utf8 = TRUE;
			l = utf_ptr2len(s);
			ಆದರೆ (l == 1)
			{
			    // Illegal UTF-8 byte sequence.
			    this_utf8 = FALSE;
			    ನಿಲ್ಲಿಸು;
			}
			s += l - 1;
		    }
		ಆದರೆ (this_utf8 == MAYBE)	    // only ASCII characters found
		    this_utf8 = FALSE;
		ಆದರೆ (utf8 == MAYBE)	    // first file
		    utf8 = this_utf8;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (utf8 != this_utf8)
		{
		    semsg(_(e_mix_ದ_help_file_encodings_within_language_str), files[fi]);
		    mix = !got_int;
		    got_int = TRUE;
		}
		firstline = FALSE;
	    }
	    ಆದರೆ (ಒಳ_example)
	    {
		// skip over example; a non-white in the first column ends it
		ಆದರೆ (vim_strchr((char_u *)" \t\n\r", IObuff[0]))
		    ಮುಂದುವರಿ;
		ಒಳ_example = FALSE;
	    }
	    p1 = vim_strchr(IObuff, '*');	// find first '*'
	    ಆಗ (p1 != ಖಾಲಿ)
	    {
		// Use vim_strbyte() instead of vim_strchr() so that when
		// 'encoding' is dbcs it still works, don't find '*' in the
		// second byte.
		p2 = vim_strbyte(p1 + 1, '*');	// find second '*'
		ಆದರೆ (p2 != ಖಾಲಿ && p2 > p1 + 1)	// skip "*" and "**"
		{
		    ಗಾಗಿ (s = p1 + 1; s < p2; ++s)
			ಆದರೆ (*s == ' ' || *s == '\t' || *s == '|')
			    ನಿಲ್ಲಿಸು;

		    // Only accept a *tag* when it consists of valid
		    // characters, there is white space before it and is
		    // followed by a white character or end-of-line.
		    ಆದರೆ (s == p2
			    && (p1 == IObuff || p1[-1] == ' ' || p1[-1] == '\t')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != ಖಾಲಿ
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			ಆದರೆ (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    ನಿಲ್ಲಿಸು;
			}
			s = alloc(p2 - p1 + STRLEN(fname) + 2);
			ಆದರೆ (s == ಖಾಲಿ)
			{
			    got_int = TRUE;
			    ನಿಲ್ಲಿಸು;
			}
			((char_u **)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len] = s;
			++ga.ga_len;
			sprintf((ಅಕ್ಷರ *)s, "%s\t%s", p1, fname);

			// find next '*'
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    size_t off = STRLEN(IObuff);
	    ಆದರೆ (off >= 2 && IObuff[off - 1] == '\n')
	    {
		off -= 2;
		ಆಗ (off > 0 && (ASCII_ISLOWER(IObuff[off])
						  || VIM_ISDIGIT(IObuff[off])))
		    off--;
		ಆದರೆ (IObuff[off] == '>' && (off == 0 || IObuff[off - 1] == ' '))
		    ಒಳ_example = TRUE;
	    }
	    ಸಾಲು_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    ಆದರೆ (!got_int)
    {
	// Sort the tags.
	ಆದರೆ (ga.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
	    ವಿಂಗಡಿಸು_ಅಕ್ಷರಸರಣಿಗಳು((char_u **)ga.ga_ದತ್ತಾಂಶ, ga.ga_len);

	// Check for duplicates.
	ಗಾಗಿ (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_ದತ್ತಾಂಶ)[i - 1];
	    p2 = ((char_u **)ga.ga_ದತ್ತಾಂಶ)[i];
	    ಆಗ (*p1 == *p2)
	    {
		ಆದರೆ (*p2 == '\t')
		{
		    *p2 = NUL;
		    vim_snprintf((ಅಕ್ಷರ *)NameBuff, MAXPATHL,
			    _(e_duplicate_ಟ್ಯಾಗ್_str_ಒಳ_file_str_str),
				     ((char_u **)ga.ga_ದತ್ತಾಂಶ)[i], dir, p2 + 1);
		    emsg((ಅಕ್ಷರ *)NameBuff);
		    *p2 = '\t';
		    ನಿಲ್ಲಿಸು;
		}
		++p1;
		++p2;
	    }
	}

	ಆದರೆ (utf8 == TRUE)
	    fprintf(fd_ಟ್ಯಾಗುಗಳು, "!_TAG_FILE_ENCODING\tutf-8\t//\n");

	// Write the tags into the file.
	ಗಾಗಿ (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_ದತ್ತಾಂಶ)[i];
	    ಆದರೆ (STRNCMP(s, "help-tags\t", 10) == 0)
		// help-tags entry was added in formatted form
		fputs((ಅಕ್ಷರ *)s, fd_ಟ್ಯಾಗುಗಳು);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		fprintf(fd_ಟ್ಯಾಗುಗಳು, "%s\t/*", s);
		ಗಾಗಿ (p1 = s; *p1 != '\t'; ++p1)
		{
		    // insert backslash before '\\' and '/'
		    ಆದರೆ (*p1 == '\\' || *p1 == '/')
			putc('\\', fd_ಟ್ಯಾಗುಗಳು);
		    putc(*p1, fd_ಟ್ಯಾಗುಗಳು);
		}
		fprintf(fd_ಟ್ಯಾಗುಗಳು, "*\n");
	    }
	}
    }
    ಆದರೆ (mix)
	got_int = FALSE;    // continue with other languages

    ಗಾಗಿ (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_ದತ್ತಾಂಶ)[i]);
    ga_clear(&ga);
    fclose(fd_ಟ್ಯಾಗುಗಳು);	    // there is no check for an error...
}

/*
 * Generate tags in one help directory, taking care of translations.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_helptags(char_u *dirname, ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_help_ಟ್ಯಾಗುಗಳು, ಪೂರ್ಣಾಂಕ ignore_writeerr)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i, j;
    garray_T	ga;
    char_u	lang[2];
    char_u	ext[5];
    char_u	fname[8];
    ಪೂರ್ಣಾಂಕ		filecount;
    char_u	**files;

    // Get a list of all files in the help directory and in subdirectories.
    STRCPY(NameBuff, dirname);
    ಕೂಡಿಸು_pathsep(NameBuff);
    STRCAT(NameBuff, "**");
    ಆದರೆ (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	semsg(_(e_no_ಹೊಂದಿಕೆ_str_1), NameBuff);
	ಹಿಂತಿರುಗಿ;
    }

    // Go over all files in the directory to find out what languages are
    // present.
    ga_init2(&ga, 1, 10);
    ಗಾಗಿ (i = 0; i < filecount; ++i)
    {
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(files[i]);
	ಆದರೆ (len <= 4)
	    ಮುಂದುವರಿ;

	ಆದರೆ (STRICMP(files[i] + len - 4, ".txt") == 0)
	{
	    // ".txt" -> language "en"
	    lang[0] = 'e';
	    lang[1] = 'n';
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (files[i][len - 4] == '.'
		&& ASCII_ISALPHA(files[i][len - 3])
		&& ASCII_ISALPHA(files[i][len - 2])
		&& TOLOWER_ASC(files[i][len - 1]) == 'x')
	{
	    // ".abx" -> language "ab"
	    lang[0] = TOLOWER_ASC(files[i][len - 3]);
	    lang[1] = TOLOWER_ASC(files[i][len - 2]);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಮುಂದುವರಿ;

	// Did we find this language already?
	ಗಾಗಿ (j = 0; j < ga.ga_len; j += 2)
	    ಆದರೆ (STRNCMP(lang, ((char_u *)ga.ga_ದತ್ತಾಂಶ) + j, 2) == 0)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (j == ga.ga_len)
	{
	    // New language, add it.
	    ಆದರೆ (ga_grow(&ga, 2) == FAIL)
		ನಿಲ್ಲಿಸು;
	    ((char_u *)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len++] = lang[0];
	    ((char_u *)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len++] = lang[1];
	}
    }

    // Loop over the found languages to generate a tags file for each one.
    ಗಾಗಿ (j = 0; j < ga.ga_len; j += 2)
    {
	STRCPY(fname, "tags-xx");
	fname[5] = ((char_u *)ga.ga_ದತ್ತಾಂಶ)[j];
	fname[6] = ((char_u *)ga.ga_ದತ್ತಾಂಶ)[j + 1];
	ಆದರೆ (fname[5] == 'e' && fname[6] == 'n')
	{
	    // English is an exception: use ".txt" and "tags".
	    fname[4] = NUL;
	    STRCPY(ext, ".txt");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Language "ab" uses ".abx" and "tags-ab".
	    STRCPY(ext, ".xxx");
	    ext[1] = fname[5];
	    ext[2] = fname[6];
	}
	helptags_one(dirname, ext, fname, ಕೂಡಿಸು_help_ಟ್ಯಾಗುಗಳು, ignore_writeerr);
    }

    ga_clear(&ga);
    FreeWild(filecount, files);

#ಇಲ್ಲದಿದ್ದರೆ
    // No language support, just use "*.txt" and "tags".
    helptags_one(dirname, (char_u *)".txt", (char_u *)"tags", ಕೂಡಿಸು_help_ಟ್ಯಾಗುಗಳು,
							    ignore_writeerr);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಶೂನ್ಯ
helptags_cb(char_u *fname, ಶೂನ್ಯ *cookie)
{
    do_helptags(fname, *(ಪೂರ್ಣಾಂಕ *)cookie, TRUE);
}

/*
 * ":helptags"
 */
    ಶೂನ್ಯ
ex_helptags(exarg_T *eap)
{
    expand_T	xpc;
    char_u	*dirname;
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_help_ಟ್ಯಾಗುಗಳು = FALSE;

    // Check for ":helptags ++t {dir}".
    ಆದರೆ (STRNCMP(eap->arg, "++t", 3) == 0 && VIM_ISWHITE(eap->arg[3]))
    {
	ಕೂಡಿಸು_help_ಟ್ಯಾಗುಗಳು = TRUE;
	eap->arg = skipwhite(eap->arg + 3);
    }

    ಆದರೆ (STRCMP(eap->arg, "ALL") == 0)
    {
	do_ಒಳ_ಮಾರ್ಗ(p_rtp, "", (char_u *)"doc", DIP_ALL + DIP_DIR,
						 helptags_cb, &ಕೂಡಿಸು_help_ಟ್ಯಾಗುಗಳು);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_DIRECTORIES;
	dirname = ExpandOne(&xpc, eap->arg, ಖಾಲಿ,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	ಆದರೆ (dirname == ಖಾಲಿ || !mch_isdir(dirname))
	    semsg(_(e_not_a_directory_str), eap->arg);
	ಇಲ್ಲದಿದ್ದರೆ
	    do_helptags(dirname, ಕೂಡಿಸು_help_ಟ್ಯಾಗುಗಳು, FALSE);
	vim_free(dirname);
    }
}
