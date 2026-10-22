/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * cmdexpand.c: functions for command-line completion
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	cmd_showtail;	// Only show path tail in lists ?
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	may_expand_ನಮೂನೆ = FALSE;
ಸ್ಥಿರ pos_T	pre_incsearch_pos; // Cursor position when incsearch started

ಸ್ಥಿರ ಶೂನ್ಯ	ಗಣ_context_for_wildcard_arg(exarg_T *eap, char_u *arg, ಪೂರ್ಣಾಂಕ usefilter, expand_T *xp, ಪೂರ್ಣಾಂಕ *complp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ExpandFromContext(expand_T *xp, char_u *, char_u ***, ಪೂರ್ಣಾಂಕ *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ char_u	*showmatches_gettail(char_u *s);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	expand_showtail(expand_T *xp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	expand_shellcmd(char_u *filepat, char_u ***matches, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, ಪೂರ್ಣಾಂಕ flagsarg);
#ಆದರೆ defined(FEAT_EVAL)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ExpandUserList(expand_T *xp, char_u ***matches, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches);
ಸ್ಥಿರ char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
ಸ್ಥಿರ char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, ಪೂರ್ಣಾಂಕ srclen, ಪೂರ್ಣಾಂಕ *new_lenp);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	expand_ನಮೂನೆ_ಒಳ_buf(char_u *pat, ಪೂರ್ಣಾಂಕ dir, char_u ***matches, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ಸ್ಥಿರ pumitem_T *compl_ಹೊಂದಿಕೆ_array = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compl_ಹೊಂದಿಕೆ_arraysize;
// First column in cmdline of the matched item for completion.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compl_startcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compl_selected;
// cmdline before expansion
ಸ್ಥಿರ string_T cmdline_orig = {ಖಾಲಿ, 0};

#ವ್ಯಾಖ್ಯಾನಿಸು SHOW_ಹೊಂದಿಕೆ(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    ಆಯ್ಕೆ (xp->xp_context)
    {
    ಸ್ಥಿತಿ EXPAND_BOOL_SETTINGS:
    ಸ್ಥಿತಿ EXPAND_COLORS:
    ಸ್ಥಿತಿ EXPAND_COMPILER:
    ಸ್ಥಿತಿ EXPAND_DIRECTORIES:
    ಸ್ಥಿತಿ EXPAND_DIRS_ಒಳ_CDPATH:
    ಸ್ಥಿತಿ EXPAND_FILES:
    ಸ್ಥಿತಿ EXPAND_FILES_ಒಳ_ಮಾರ್ಗ:
    ಸ್ಥಿತಿ EXPAND_FILETYPE:
    ಸ್ಥಿತಿ EXPAND_FILETYPECMD:
    ಸ್ಥಿತಿ EXPAND_HELP:
    ಸ್ಥಿತಿ EXPAND_KEYMAP:
    ಸ್ಥಿತಿ EXPAND_OLD_SETTING:
    ಸ್ಥಿತಿ EXPAND_STRING_SETTING:
    ಸ್ಥಿತಿ EXPAND_SETTING_ಕಳೆ:
    ಸ್ಥಿತಿ EXPAND_OWNSYNTAX:
    ಸ್ಥಿತಿ EXPAND_PACKADD:
    ಸ್ಥಿತಿ EXPAND_RUNTIME:
    ಸ್ಥಿತಿ EXPAND_SHELLCMD:
    ಸ್ಥಿತಿ EXPAND_SHELLCMDLINE:
    ಸ್ಥಿತಿ EXPAND_ಟ್ಯಾಗುಗಳು:
    ಸ್ಥಿತಿ EXPAND_ಟ್ಯಾಗುಗಳು_LISTFILES:
	ಹಿಂತಿರುಗಿ FALSE;

    ಪೂರ್ವನಿಯೋಜಿತ:
	ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ vim_strchr(p_wop, WOP_FUZZY) != ಖಾಲಿ;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    ಪೂರ್ಣಾಂಕ
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    ಹಿಂತಿರುಗಿ vim_strchr(p_wop, WOP_FUZZY) != ಖಾಲಿ && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ವಿಂಗಡಿಸು_func_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    ಆದರೆ (*p1 != '<' && *p2 == '<') ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (*p1 == '<' && *p2 != '<') ಹಿಂತಿರುಗಿ 1;
    ಹಿಂತಿರುಗಿ STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wildescape(
    expand_T	*xp,
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		numfiles,
    char_u	**files)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_ಬಫರ್ : VSE_NONE;

    ಆದರೆ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_ಒಳ_ಮಾರ್ಗ
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_ಒಳ_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    ಆದರೆ (xp->xp_backslash & XP_BS_THREE)
	    {
		ಅಕ್ಷರ *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		ಆದರೆ (p != ಖಾಲಿ)
		{
		    vim_free(files[i]);
		    files[i] = p;
#ಆದರೆ defined(BACKSLASH_ಒಳ_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    ಆದರೆ (p != ಖಾಲಿ)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#ಆದರೆ_ಅಂತ್ಯ
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_backslash & XP_BS_COMMA)
	    {
		ಆದರೆ (vim_strchr(files[i], ',') != ಖಾಲಿ)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    ಆದರೆ (p != ಖಾಲಿ)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#ಇಲ್ಲದಿದ್ದರೆ
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (p != ಖಾಲಿ)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    ಆದರೆ (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	ಆದರೆ (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_context == EXPAND_ಟ್ಯಾಗುಗಳು)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    ಆದರೆ (p != ಖಾಲಿ)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }
	}
    }
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ExpandEscape(
    expand_T	*xp,
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		numfiles,
    char_u	**files,
    ಪೂರ್ಣಾಂಕ		options)
{
    // May change home directory back to "~"
    ಆದರೆ (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    ಆದರೆ (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    ಪೂರ್ಣಾಂಕ
nextwild(
    expand_T	*xp,
    ಪೂರ್ಣಾಂಕ		type,
    ಪೂರ್ಣಾಂಕ		options,	// extra options for ExpandOne()
    ಪೂರ್ಣಾಂಕ		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    ಪೂರ್ಣಾಂಕ		wild_navigate = (type == WILD_ಮುಂದಿನ || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    ಆದರೆ (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (ccline->ಒಳಹರಿವು_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    ಗಣ_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    may_expand_ನಮೂನೆ = options & WILD_MAY_EXPAND_ನಮೂನೆ;
	    ಗಣ_expand_context(xp);
	    may_expand_ನಮೂನೆ = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    ಆದರೆ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ಹಿಂತಿರುಗಿ OK;  // Something illegal on command line
    }
    ಆದರೆ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ಹಿಂತಿರುಗಿ FAIL;
    }

    i = (ಪೂರ್ಣಾಂಕ)(xp->xp_ನಮೂನೆ - ccline->cmdbuff);
    xp->xp_ನಮೂನೆ_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    ಆದರೆ (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_ನಮೂನೆ_len == 0)
	ಹಿಂತಿರುಗಿ FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    ಆದರೆ (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	ಸಂದೇಶ_puts("...");	    // show that we are busy
	ಹೊರಹೋಗು_flush();
    }

    ಆದರೆ (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, ಖಾಲಿ, ಖಾಲಿ, 0, type);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_ನಮೂನೆ;
	ಪೂರ್ಣಾಂಕ	pat_len = xp->xp_ನಮೂನೆ_len;
#ಆದರೆ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	ಆದರೆ (unesc != ಖಾಲಿ)
	    pat_src = unesc;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_ನಮೂನೆ_ಒಳ_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	ಇಲ್ಲದಿದ್ದರೆ
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#ಆದರೆ defined(FEAT_EVAL)
	vim_free(unesc);
#ಆದರೆ_ಅಂತ್ಯ

	// Translate string into pattern and expand it.
	ಆದರೆ (tmp == ಖಾಲಿ)
	    p = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ use_options = options |
		    WILD_HOME_REPLACE|WILD_ಕೂಡಿಸು_SLASH|WILD_SILENT;
	    ಆದರೆ (escape)
		use_options |= WILD_ESCAPE;
	    ಆದರೆ (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_ನಮೂನೆ_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    ಆದರೆ (p != ಖಾಲಿ && type == WILD_LONGEST)
	    {
		ಪೂರ್ಣಾಂಕ	j;

		ಗಾಗಿ (j = 0; j < xp->xp_ನಮೂನೆ_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    ಆದರೆ (c == '*' || c == '?')
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ ((ಪೂರ್ಣಾಂಕ)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    ಆದರೆ (!wild_navigate && ccline->cmdbuff != ಖಾಲಿ)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	ಆದರೆ (cmdline_orig.string == ಖಾಲಿ)
	    cmdline_orig.ಉದ್ದಳತೆ = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    cmdline_orig.ಉದ್ದಳತೆ = ccline->cmdlen;
    }

    ಆದರೆ (p != ಖಾಲಿ && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	ಪೂರ್ಣಾಂಕ	difflen;
	ಪೂರ್ಣಾಂಕ	v = OK;

	difflen = (ಪೂರ್ಣಾಂಕ)plen - xp->xp_ನಮೂನೆ_len;
	ಆದರೆ (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_ನಮೂನೆ = ccline->cmdbuff + i;
	}

	ಆದರೆ (v == OK)
	{
	    mch_memmove(&ccline->cmdbuff[ccline->cmdpos + difflen],
		    &ccline->cmdbuff[ccline->cmdpos],
		    (size_t)(ccline->cmdlen - ccline->cmdpos + 1));
	    mch_memmove(&ccline->cmdbuff[i], p, plen);
	    ccline->cmdlen += difflen;
	    ccline->cmdpos += difflen;
	}
    }

    redrawcmd();
    cursorcmd();

    // When expanding a ":map" command and no matches are found, assume that
    // the key is supposed to be inserted literally
    ಆದರೆ (xp->xp_context == EXPAND_MAPPINGS && p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (xp->xp_numfiles <= 0 && p == ಖಾಲಿ)
	beep_flush();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(ಶೂನ್ಯ)ExpandOne(xp, ಖಾಲಿ, ಖಾಲಿ, 0, WILD_FREE);

    vim_free(p);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_Matches,
	ಪೂರ್ಣಾಂಕ		showtail)
{
    ಪೂರ್ಣಾಂಕ	ಪೂರ್ವಪ್ರತ್ಯಯ_len;

    // Add all the completion matches
    compl_ಹೊಂದಿಕೆ_array = ALLOC_MULT(pumitem_T, ಅಂಕೆ_Matches);
    ಆದರೆ (compl_ಹೊಂದಿಕೆ_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ EXPAND_UNSUCCESSFUL;

    compl_ಹೊಂದಿಕೆ_arraysize = ಅಂಕೆ_Matches;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_Matches; i++)
    {
	compl_ಹೊಂದಿಕೆ_array[i].pum_ಪಠ್ಯ = (xp->xp_files_abbr != ಖಾಲಿ
				    && xp->xp_files_abbr[i] != ಖಾಲಿ)
					? xp->xp_files_abbr[i]
					: SHOW_ಹೊಂದಿಕೆ(i);
	compl_ಹೊಂದಿಕೆ_array[i].pum_info = xp->xp_files_info != ಖಾಲಿ
					    ? xp->xp_files_info[i] : ಖಾಲಿ;
	compl_ಹೊಂದಿಕೆ_array[i].pum_extra = xp->xp_files_menu != ಖಾಲಿ
					    ? xp->xp_files_menu[i] : ಖಾಲಿ;
	compl_ಹೊಂದಿಕೆ_array[i].pum_ಬಗೆ = xp->xp_files_ಬಗೆ != ಖಾಲಿ
					    ? xp->xp_files_ಬಗೆ[i] : ಖಾಲಿ;
	compl_ಹೊಂದಿಕೆ_array[i].pum_user_abbr_hlattr = -1;
	compl_ಹೊಂದಿಕೆ_array[i].pum_user_ಬಗೆ_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == ಖಾಲಿ ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    ಪೂರ್ವಪ್ರತ್ಯಯ_len = vim_strsize(xp->xp_ನಮೂನೆ);
    ಆದರೆ (showtail)
	ಪೂರ್ವಪ್ರತ್ಯಯ_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_ಲಂಬಸಾಲು_off + MAX(0, compl_startcol - ಪೂರ್ವಪ್ರತ್ಯಯ_len);

    ಹಿಂತಿರುಗಿ EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    ಶೂನ್ಯ
cmdline_pum_display(ಶೂನ್ಯ)
{
    ಆದರೆ (p_po > 0 && p_po < 100 && !pum_redraw_ಒಳ_same_position())
	pum_call_update_screen();
    pum_display(compl_ಹೊಂದಿಕೆ_array, compl_ಹೊಂದಿಕೆ_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    ಪೂರ್ಣಾಂಕ
cmdline_pum_ಸಕ್ರಿಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ pum_visible() && compl_ಹೊಂದಿಕೆ_array != ಖಾಲಿ;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    ಶೂನ್ಯ
cmdline_pum_remove(cmdline_info_T *cclp ಬಳಸದ, ಪೂರ್ಣಾಂಕ defer_redraw)
{
    ಪೂರ್ಣಾಂಕ	save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ	save_RedrawingDisabled = RedrawingDisabled;
    ಆದರೆ (cclp->ಒಳಹರಿವು_fn)
	RedrawingDisabled = 0;
#ಆದರೆ_ಅಂತ್ಯ

    ಪದ_ಗಣ_sync_output(ಪದ_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_ಹೊಂದಿಕೆ_array);
    compl_ಹೊಂದಿಕೆ_arraysize = 0;
    ಆದರೆ (!defer_redraw)
    {
	ಪೂರ್ಣಾಂಕ save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	pum_call_update_screen();
    redrawcmd();
    ಪದ_ಗಣ_sync_output(ಪದ_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    ಕೀಲಿ_Typed = save_ಕೀಲಿ_Typed;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (cclp->ಒಳಹರಿವು_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
cmdline_pum_cleanup(cmdline_info_T *cclp)
{
    cmdline_pum_remove(cclp, FALSE);
    wildmenu_cleanup(cclp);
}

/*
 * Returns the starting column number to use for the cmdline completion popup
 * menu.
 */
    ಪೂರ್ಣಾಂಕ
cmdline_compl_startcol(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_ನಮೂನೆ(ಶೂನ್ಯ)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ಹಿಂತಿರುಗಿ xp == ಖಾಲಿ ? ಖಾಲಿ : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    ಪೂರ್ಣಾಂಕ
cmdline_compl_is_fuzzy(ಶೂನ್ಯ)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ಹಿಂತಿರುಗಿ xp != ಖಾಲಿ && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
skip_ಸ್ಥಿತಿ_ಹೊಂದಿಕೆ_char(expand_T *xp, char_u *s)
{
    ಆದರೆ ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_ನಮೂನೆ_ಒಳ_BUF)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#ಆದರೆ_ಅಂತ್ಯ
	   )
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ BACKSLASH_ಒಳ_FILENAME
	ಆದರೆ (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    ಹಿಂತಿರುಗಿ 2;
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಸ್ಥಿತಿ_ಹೊಂದಿಕೆ_len(expand_T *xp, char_u *s)
{
    ಪೂರ್ಣಾಂಕ	len = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಪೂರ್ಣಾಂಕ emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    ಆದರೆ (emenu && menu_is_separator(s))
	ಹಿಂತಿರುಗಿ 1;
#ಆದರೆ_ಅಂತ್ಯ

    ಆಗ (*s != NUL)
    {
	s += skip_ಸ್ಥಿತಿ_ಹೊಂದಿಕೆ_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    ಹಿಂತಿರುಗಿ len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_redr_ಸ್ಥಿತಿ_matches(
    expand_T	*xp,
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_matches,
    char_u	**matches,	// list of matches
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ,
    ಪೂರ್ಣಾಂಕ		showtail)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    char_u	*buf;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		clen;		// length in screen cells
    ಪೂರ್ಣಾಂಕ		fillchar;
    ಪೂರ್ಣಾಂಕ		attr;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		highlight = TRUE;
    char_u	*selstart = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		selstart_ಲಂಬಸಾಲು = 0;
    char_u	*selend = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಮೊದಲ_ಹೊಂದಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_ಎಡ = FALSE;
    char_u	*s;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಪೂರ್ಣಾಂಕ		emenu;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		l;

    ಆದರೆ (matches == ಖಾಲಿ)	// interrupted completion?
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (has_mbyte)
	buf = alloc(topframe->fr_ಅಗಲ * MB_MAXBYTES + 1);
    ಇಲ್ಲದಿದ್ದರೆ
	buf = alloc(topframe->fr_ಅಗಲ + 1);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಹೊಂದಿಕೆ == -1)	// don't show match but original text
    {
	ಹೊಂದಿಕೆ = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = ಸ್ಥಿತಿ_ಹೊಂದಿಕೆ_len(xp, SHOW_ಹೊಂದಿಕೆ(ಹೊಂದಿಕೆ)) + 3;
    ಆದರೆ (ಹೊಂದಿಕೆ == 0)
	ಮೊದಲ_ಹೊಂದಿಕೆ = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಹೊಂದಿಕೆ < ಮೊದಲ_ಹೊಂದಿಕೆ)
    {
	// jumping left, as far as we can go
	ಮೊದಲ_ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ;
	ಕೂಡಿಸು_ಎಡ = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// check if match fits on the screen
	ಗಾಗಿ (i = ಮೊದಲ_ಹೊಂದಿಕೆ; i < ಹೊಂದಿಕೆ; ++i)
	    clen += ಸ್ಥಿತಿ_ಹೊಂದಿಕೆ_len(xp, SHOW_ಹೊಂದಿಕೆ(i)) + 2;
	ಆದರೆ (ಮೊದಲ_ಹೊಂದಿಕೆ > 0)
	    clen += 2;
	// jumping right, put match at the left
	ಆದರೆ (clen > topframe->fr_ಅಗಲ)
	{
	    ಮೊದಲ_ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    ಗಾಗಿ (i = ಹೊಂದಿಕೆ; i < ಅಂಕೆ_matches; ++i)
	    {
		clen += ಸ್ಥಿತಿ_ಹೊಂದಿಕೆ_len(xp, SHOW_ಹೊಂದಿಕೆ(i)) + 2;
		ಆದರೆ (clen >= topframe->fr_ಅಗಲ)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (i == ಅಂಕೆ_matches)
		ಕೂಡಿಸು_ಎಡ = TRUE;
	}
    }
    ಆದರೆ (ಕೂಡಿಸು_ಎಡ)
	ಆಗ (ಮೊದಲ_ಹೊಂದಿಕೆ > 0)
	{
	    clen += ಸ್ಥಿತಿ_ಹೊಂದಿಕೆ_len(xp, SHOW_ಹೊಂದಿಕೆ(ಮೊದಲ_ಹೊಂದಿಕೆ - 1)) + 2;
	    ಆದರೆ (clen >= topframe->fr_ಅಗಲ)
		ನಿಲ್ಲಿಸು;
	    --ಮೊದಲ_ಹೊಂದಿಕೆ;
	}

    fillchar = fillchar_ಸ್ಥಿತಿ(&attr, curwin);

    ಆದರೆ (ಮೊದಲ_ಹೊಂದಿಕೆ == 0)
    {
	*buf = NUL;
	len = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = ಮೊದಲ_ಹೊಂದಿಕೆ;
    ಆಗ (clen + ಸ್ಥಿತಿ_ಹೊಂದಿಕೆ_len(xp, SHOW_ಹೊಂದಿಕೆ(i)) + 2 < topframe->fr_ಅಗಲ)
    {
	ಆದರೆ (i == ಹೊಂದಿಕೆ)
	{
	    selstart = buf + len;
	    selstart_ಲಂಬಸಾಲು = clen;
	}

	s = SHOW_ಹೊಂದಿಕೆ(i);
	// Check for menu separators - replace with '|'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	ಆದರೆ (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (ಪೂರ್ಣಾಂಕ)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಗಾಗಿ ( ; *s != NUL; ++s)
	{
	    s += skip_ಸ್ಥಿತಿ_ಹೊಂದಿಕೆ_char(xp, s);
	    clen += ptr2cells(s);
	    ಆದರೆ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (ಪೂರ್ಣಾಂಕ)STRLEN(buf + len);
	    }
	}
	ಆದರೆ (i == ಹೊಂದಿಕೆ)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	ಆದರೆ (++i == ಅಂಕೆ_matches)
		ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (i != ಅಂಕೆ_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು - 1;
    ಆದರೆ (ಅಡ್ಡಸಾಲು >= 0)
    {
	ಆದರೆ (wild_menu_showing == 0)
	{
	    ಆದರೆ (ಸಂದೇಶ_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		ಆದರೆ (cmdline_ಅಡ್ಡಸಾಲು == ಅಡ್ಡಸಾಲುಗಳು - 1)
		{
		    screen_del_ಸಾಲುಗಳು(0, 0, 1, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, TRUE, 0, ಖಾಲಿ);
		    ++ಸಂದೇಶ_scrolled;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ++cmdline_ಅಡ್ಡಸಾಲು;
		    ++ಅಡ್ಡಸಾಲು;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		ಆದರೆ (lastwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0)
		{
		    save_p_ls = p_ls;
		    save_p_wmh = p_wmh;
		    p_ls = 2;
		    p_wmh = 0;
		    ಕೊನೆಯ_ಸ್ಥಿತಿ(FALSE);
		}
		wild_menu_showing = WM_SHOWN;
	    }
	}

	screen_puts(buf, ಅಡ್ಡಸಾಲು, firstwin->w_wincol, attr);
	ಆದರೆ (selstart != ಖಾಲಿ && highlight)
	{
	    *selend = NUL;
	    screen_puts(selstart, ಅಡ್ಡಸಾಲು, firstwin->w_wincol + selstart_ಲಂಬಸಾಲು,
		    HL_ATTR(HLF_WM));
	}

	screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, firstwin->w_wincol + clen,
		firstwin->w_wincol + topframe->fr_ಅಗಲ,
		fillchar, fillchar, attr);
    }

    win_redraw_ಕೊನೆಯ_ಸ್ಥಿತಿ(topframe);
    vim_free(buf);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Apply -completeopt=escape to a string about to be inserted into the command
 * line as a completion result.  If "str" is non-NULL and the active expansion
 * context is a customlist/custom user command with UCC_ESCAPE set, free "str"
 * and return a newly-allocated copy with spaces, tabs and backslashes prefixed
 * by a backslash.  Otherwise return "str" unchanged.
 */
    ಸ್ಥಿರ char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ಹಿಂತಿರುಗಿ str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ str;
    vim_free(str);
    ಹಿಂತಿರುಗಿ p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    ಸ್ಥಿರ char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    ಪೂರ್ಣಾಂಕ		srclen,
    ಪೂರ್ಣಾಂಕ		*new_lenp)
{
    char_u  *buf, *p, *d, *ಕೊನೆ;

    ಆದರೆ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    buf = alloc(srclen + 1);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    d = buf;
    ಕೊನೆ = src + srclen;
    ಗಾಗಿ (p = src; p < ಕೊನೆ; ++p)
    {
	ಆದರೆ (*p == '\\' && p + 1 < ಕೊನೆ
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (ಪೂರ್ಣಾಂಕ)(d - buf);
    ಹಿಂತಿರುಗಿ buf;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    ಸ್ಥಿರ char_u *
get_ಮುಂದಿನ_or_prev_ಹೊಂದಿಕೆ(ಪೂರ್ಣಾಂಕ ಬಹುಲಕ, expand_T *xp)
{
    ಪೂರ್ಣಾಂಕ	    findex = xp->xp_selected;
    ಪೂರ್ಣಾಂಕ	    ht;

    // When no matches found, return NULL
    ಆದರೆ (xp->xp_numfiles <= 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (ಬಹುಲಕ == WILD_PREV)
    {
	// Select the last entry if at original text
	ಆದರೆ (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಹುಲಕ == WILD_ಮುಂದಿನ)
    {
	// Select the next entry
	++findex;
    }
    ಇಲ್ಲದಿದ್ದರೆ   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_ಎತ್ತರ();
	ಆದರೆ (ht > 3)
	    ht -= 2;

	ಆದರೆ (ಬಹುಲಕ == WILD_PAGEUP)
	{
	    ಆದರೆ (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	ಇಲ್ಲದಿದ್ದರೆ    // mode == WILD_PAGEDOWN
	{
	    ಆದರೆ (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    ಆದರೆ (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	ಆದರೆ (xp->xp_orig != ಖಾಲಿ)
	    findex = -1;
	ಇಲ್ಲದಿದ್ದರೆ
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    ಆದರೆ (p_wmnu)
    {
	ಆದರೆ (compl_ಹೊಂದಿಕೆ_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr(p_wop, WOP_PUM) != ಖಾಲಿ)
	{
	    ಆದರೆ (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    win_redr_ಸ್ಥಿತಿ_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    ಆದರೆ (findex == -1)
	ಹಿಂತಿರುಗಿ vim_strsave(xp->xp_orig);
#ಆದರೆ defined(FEAT_EVAL)
    ಹಿಂತಿರುಗಿ apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ vim_strsave(xp->xp_files[findex]);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Start the command-line expansion and get the matches.
 */
    ಸ್ಥಿರ char_u *
Expand_One_ಆರಂಭ(ಪೂರ್ಣಾಂಕ ಬಹುಲಕ, expand_T *xp, char_u *str, ಪೂರ್ಣಾಂಕ options)
{
    ಪೂರ್ಣಾಂಕ		non_suf_ಹೊಂದಿಕೆ;		// number without matching suffix
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*ss = ಖಾಲಿ;

    // Do the expansion.
    ಆದರೆ (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	ಆದರೆ (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_ಹೊಂದಿಕೆ_str_2), str);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_numfiles == 0)
    {
	ಆದರೆ (!(options & WILD_SILENT))
	    semsg(_(e_no_ಹೊಂದಿಕೆ_str_2), str);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	ಆದರೆ (ಬಹುಲಕ != WILD_ALL && ಬಹುಲಕ != WILD_ALL_KEEP && ಬಹುಲಕ != WILD_LONGEST)
	{
	    ಆದರೆ (xp->xp_numfiles)
		non_suf_ಹೊಂದಿಕೆ = xp->xp_numfiles;
	    ಇಲ್ಲದಿದ್ದರೆ
		non_suf_ಹೊಂದಿಕೆ = 1;
	    ಆದರೆ ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_ಹೊಂದಿಕೆ = 0;
		ಗಾಗಿ (i = 0; i < 2; ++i)
		    ಆದರೆ (ಹೊಂದಿಕೆ_suffix(xp->xp_files[i]))
			++non_suf_ಹೊಂದಿಕೆ;
	    }
	    ಆದರೆ (non_suf_ಹೊಂದಿಕೆ != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		ಆದರೆ (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_ಹೆಸರುಗಳು));
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    ಆದರೆ (!(non_suf_ಹೊಂದಿಕೆ != 1 && ಬಹುಲಕ == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    ಹಿಂತಿರುಗಿ ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    ಸ್ಥಿರ char_u *
ಕಂಡುಹಿಡಿ_longest_ಹೊಂದಿಕೆ(expand_T *xp, ಪೂರ್ಣಾಂಕ options)
{
    long_u	len;
    ಪೂರ್ಣಾಂಕ		mb_len = 1;
    ಪೂರ್ಣಾಂಕ		c0, ci;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*ss;

    ಗಾಗಿ (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	ಆದರೆ (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    c0 = xp->xp_files[0][len];
	ಗಾಗಿ (i = 1; i < xp->xp_numfiles; ++i)
	{
	    ಆದರೆ (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    ಇಲ್ಲದಿದ್ದರೆ
		ci = xp->xp_files[i][len];
	    ಆದರೆ (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		ಆದರೆ (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 != ci)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (i < xp->xp_numfiles)
	{
	    ಆದರೆ (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    ನಿಲ್ಲಿಸು;
	}
    }

    ss = alloc(len + 1);
    ಆದರೆ (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    ಹಿಂತಿರುಗಿ ss;
}

    ಶೂನ್ಯ
free_xp_files_extra(expand_T *xp, ಪೂರ್ಣಾಂಕ numfiles)
{
    ಆದರೆ (xp->xp_files_abbr != ಖಾಲಿ)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = ಖಾಲಿ;
    }
    ಆದರೆ (xp->xp_files_ಬಗೆ != ಖಾಲಿ)
    {
	FreeWild(numfiles, xp->xp_files_ಬಗೆ);
	xp->xp_files_ಬಗೆ = ಖಾಲಿ;
    }
    ಆದರೆ (xp->xp_files_menu != ಖಾಲಿ)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = ಖಾಲಿ;
    }
    ಆದರೆ (xp->xp_files_info != ಖಾಲಿ)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = ಖಾಲಿ;
    }
}

/*
 * Do wildcard expansion on the string "str".
 * Chars that should not be expanded must be preceded with a backslash.
 * Return a pointer to allocated memory containing the new string.
 * Return NULL for failure.
 *
 * "orig" is the originally expanded string, copied to allocated memory.  It
 * should either be kept in "xp->xp_orig" or freed.  When "mode" is WILD_NEXT
 * or WILD_PREV "orig" should be NULL.
 *
 * Results are cached in xp->xp_files and xp->xp_numfiles, except when "mode"
 * is WILD_EXPAND_FREE or WILD_ALL.
 *
 * mode = WILD_FREE:	    just free previously expanded matches
 * mode = WILD_EXPAND_FREE: normal expansion, do not keep matches
 * mode = WILD_EXPAND_KEEP: normal expansion, keep matches
 * mode = WILD_NEXT:	    use next match in multiple match, wrap to first
 * mode = WILD_PREV:	    use previous match in multiple match, wrap to first
 * mode = WILD_ALL:	    return all matches concatenated
 * mode = WILD_LONGEST:	    return longest matched part
 * mode = WILD_ALL_KEEP:    get all matches, keep matches
 * mode = WILD_APPLY:	    apply the item selected in the cmdline completion
 *			    popup menu and close the menu.
 * mode = WILD_CANCEL:	    cancel and close the cmdline completion popup and
 *			    use the original text.
 *
 * options = WILD_LIST_NOTFOUND:    list entries without a match
 * options = WILD_HOME_REPLACE:	    do home_replace() for buffer names
 * options = WILD_USE_NL:	    Use '\n' for WILD_ALL
 * options = WILD_NO_BEEP:	    Don't beep for multiple matches
 * options = WILD_ADD_SLASH:	    add a slash after directory names
 * options = WILD_KEEP_ALL:	    don't remove 'wildignore' entries
 * options = WILD_SILENT:	    don't print warning messages
 * options = WILD_ESCAPE:	    put backslash before special chars
 * options = WILD_ICASE:	    ignore case for files
 * options = WILD_ALLLINKS;	    keep broken links
 *
 * The variables xp->xp_context and xp->xp_backslash must have been set!
 */
    char_u *
ExpandOne(
    expand_T	*xp,
    char_u	*str,
    char_u	*orig,	    // allocated copy of original of expanded string
    ಪೂರ್ಣಾಂಕ		options,
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ)
{
    char_u	*ss = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		orig_saved = FALSE;
#ಆದರೆ defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    ಪೂರ್ಣಾಂಕ		ss_is_ಹೊಂದಿಕೆ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // first handle the case of using an old match
    ಆದರೆ (ಬಹುಲಕ == WILD_ಮುಂದಿನ || ಬಹುಲಕ == WILD_PREV
	    || ಬಹುಲಕ == WILD_PAGEUP || ಬಹುಲಕ == WILD_PAGEDOWN)
	ಹಿಂತಿರುಗಿ get_ಮುಂದಿನ_or_prev_ಹೊಂದಿಕೆ(ಬಹುಲಕ, xp);

    ಆದರೆ (ಬಹುಲಕ == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಹುಲಕ == WILD_APPLY)
    {
	ಆದರೆ (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#ಆದರೆ defined(FEAT_EVAL)
	    ss_is_ಹೊಂದಿಕೆ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

    // free old names
    ಆದರೆ (xp->xp_numfiles != -1 && ಬಹುಲಕ != WILD_ALL && ಬಹುಲಕ != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	ಆದರೆ (compl_ಹೊಂದಿಕೆ_array != ಖಾಲಿ)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    ಆದರೆ (ಬಹುಲಕ == WILD_FREE)	// only release file name
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (xp->xp_numfiles == -1 && ಬಹುಲಕ != WILD_APPLY && ಬಹುಲಕ != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = Expand_One_ಆರಂಭ(ಬಹುಲಕ, xp, str, options);
#ಆದರೆ defined(FEAT_EVAL)
	ಆದರೆ (ss != ಖಾಲಿ)
	    ss_is_ಹೊಂದಿಕೆ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Find longest common part
    ಆದರೆ (ಬಹುಲಕ == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = ಕಂಡುಹಿಡಿ_longest_ಹೊಂದಿಕೆ(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#ಆದರೆ defined(FEAT_EVAL)
	ಆದರೆ (ss != ಖಾಲಿ)
	    ss_is_ಹೊಂದಿಕೆ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    ಆದರೆ (ಬಹುಲಕ == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	ಅಕ್ಷರ	*ಪೂರ್ವಪ್ರತ್ಯಯ = "";
	ಅಕ್ಷರ	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	ಪೂರ್ಣಾಂಕ	n = xp->xp_numfiles - 1;
	ಪೂರ್ಣಾಂಕ	i;
#ಆದರೆ defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = ಖಾಲಿ;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	ಆದರೆ ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    ಆದರೆ (escaped != ಖಾಲಿ)
	    {
		ಗಾಗಿ (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    ಆದರೆ (escaped[i] == ಖಾಲಿ)
		    {
			ಆಗ (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			ನಿಲ್ಲಿಸು;
		    }
		}
		ಆದರೆ (escaped != ಖಾಲಿ)
		    files = escaped;
	    }
	}
#ಇಲ್ಲದಿದ್ದರೆ
	char_u	**files = xp->xp_files;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (xp->xp_ಪೂರ್ವಪ್ರತ್ಯಯ == XP_ಪೂರ್ವಪ್ರತ್ಯಯ_NO)
	{
	    ಪೂರ್ವಪ್ರತ್ಯಯ = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_ಪೂರ್ವಪ್ರತ್ಯಯ == XP_ಪೂರ್ವಪ್ರತ್ಯಯ_INV)
	{
	    ಪೂರ್ವಪ್ರತ್ಯಯ = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	ಗಾಗಿ (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	ಆದರೆ (ss != ಖಾಲಿ)
	{
	    size_t  ss_len = 0;

	    ಗಾಗಿ (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (ಅಕ್ಷರ *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? ಪೂರ್ವಪ್ರತ್ಯಯ : "",
		    (ಅಕ್ಷರ *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#ಆದರೆ defined(FEAT_EVAL)
	ಆದರೆ (escaped != ಖಾಲಿ)
	{
	    ಗಾಗಿ (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (ಬಹುಲಕ == WILD_EXPAND_FREE || ಬಹುಲಕ == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    ಆದರೆ (!orig_saved)
	vim_free(orig);

#ಆದರೆ defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    ಆದರೆ (ss_is_ಹೊಂದಿಕೆ && ಬಹುಲಕ != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ss;
}

/*
 * Prepare an expand structure for use.
 */
    ಶೂನ್ಯ
ExpandInit(expand_T *xp)
{
    CLEAR_ಸೂಚಕ(xp);
    xp->xp_backslash = XP_BS_NONE;
    xp->xp_ಪೂರ್ವಪ್ರತ್ಯಯ = XP_ಪೂರ್ವಪ್ರತ್ಯಯ_NONE;
    xp->xp_numfiles = -1;
}

/*
 * Cleanup an expand structure after use.
 */
    ಶೂನ್ಯ
ExpandCleanup(expand_T *xp)
{
    ಆದರೆ (xp->xp_numfiles >= 0)
    {
	free_xp_files_extra(xp, xp->xp_numfiles);
	FreeWild(xp->xp_numfiles, xp->xp_files);
	xp->xp_numfiles = -1;
    }
    VIM_CLEAR(xp->xp_orig);
}

    ಶೂನ್ಯ
clear_cmdline_orig(ಶೂನ್ಯ)
{
    VIM_CLEAR_STRING(cmdline_orig);
}

/*
 * Display one line of completion matches. Multiple matches are displayed in
 * each line (used by wildmode=list and CTRL-D)
 *   matches - list of completion match names
 *   numMatches - number of completion matches in "matches"
 *   lines - number of output lines
 *   linenr - line number of matches to display
 *   maxlen - maximum number of characters in each line
 *   showtail - display only the tail of the full path of a file name
 *   dir_attr - highlight attribute to use for directory names
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_Matches,
	ಪೂರ್ಣಾಂಕ		ಸಾಲುಗಳು,
	ಪೂರ್ಣಾಂಕ		linenr,
	ಪೂರ್ಣಾಂಕ		maxlen,
	ಪೂರ್ಣಾಂಕ		showtail,
	ಪೂರ್ಣಾಂಕ		dir_attr)
{
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		isdir;
    ಪೂರ್ಣಾಂಕ		lastlen;
    char_u	*p;

    lastlen = 999;
    ಗಾಗಿ (j = linenr; j < ಅಂಕೆ_Matches; j += ಸಾಲುಗಳು)
    {
	ಆದರೆ (xp->xp_context == EXPAND_ಟ್ಯಾಗುಗಳು_LISTFILES)
	{
	    ಸಂದೇಶ_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    ಸಂದೇಶ_advance(maxlen + 1);
	    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)p);
	    ಸಂದೇಶ_advance(maxlen + 3);
	    ಸಂದೇಶ_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    ನಿಲ್ಲಿಸು;
	}
	ಗಾಗಿ (i = maxlen - lastlen; --i >= 0; )
	    ಸಂದೇಶ_putchar(' ');
	ಆದರೆ (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    ಆದರೆ (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_ಮಾರ್ಗ;
		char_u	*ಮಾರ್ಗ;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_ಮಾರ್ಗ = expand_env_save_opt(matches[j], TRUE, ಖಾಲಿ);
		ಮಾರ್ಗ = exp_ಮಾರ್ಗ != ಖಾಲಿ ? exp_ಮಾರ್ಗ : matches[j];
		halved_slash = backslash_halve_save(ಮಾರ್ಗ);
		isdir = mch_isdir(halved_slash != ಖಾಲಿ ? halved_slash
			: matches[j]);
		vim_free(exp_ಮಾರ್ಗ);
		ಆದರೆ (halved_slash != ಮಾರ್ಗ)
		    vim_free(halved_slash);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    ಆದರೆ (showtail)
		p = SHOW_ಹೊಂದಿಕೆ(j);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		home_replace(ಖಾಲಿ, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    isdir = FALSE;
	    p = SHOW_ಹೊಂದಿಕೆ(j);
	}
	lastlen = ಸಂದೇಶ_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು > 0)	// when not wrapped around
    {
	ಸಂದೇಶ_clr_eos();
	ಸಂದೇಶ_putchar('\n');
    }
    ಹೊರಹೋಗು_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_Matches,
	ಪೂರ್ಣಾಂಕ		showtail,
	ಪೂರ್ಣಾಂಕ		noselect)
{
    ಪೂರ್ಣಾಂಕ	retval = cmdline_pum_create(ccline, xp, matches, ಅಂಕೆ_Matches, showtail);

    ಆದರೆ (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    ಪೂರ್ಣಾಂಕ
showmatches(
    expand_T	*xp,
    ಪೂರ್ಣಾಂಕ		display_wildmenu,
    ಪೂರ್ಣಾಂಕ		display_list,
    ಪೂರ್ಣಾಂಕ		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_Matches;
    char_u	**matches;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		maxlen;
    ಪೂರ್ಣಾಂಕ		ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಂಕ		columns;
    ಪೂರ್ಣಾಂಕ		attr;
    ಪೂರ್ಣಾಂಕ		showtail;
    ಪೂರ್ಣಾಂಕ		noselect = (wim_flags_arg & WIM_NOSELECT);
    ಪೂರ್ಣಾಂಕ		noinsert = (wim_flags_arg & WIM_NOINSERT);
    ಪೂರ್ಣಾಂಕ		cmdline_unchanged = noselect || noinsert;

    ಆದರೆ (xp->xp_numfiles == -1)
    {
	ಪೂರ್ಣಾಂಕ retval;
	ಗಣ_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&ಅಂಕೆ_Matches, &matches);
	ಆದರೆ (retval != EXPAND_OK)
	    ಹಿಂತಿರುಗಿ retval;
	showtail = expand_showtail(xp);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಅಂಕೆ_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    ಆದರೆ (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ show_pum_matches(ccline, xp, matches, ಅಂಕೆ_Matches,
		showtail && !cmdline_unchanged, noselect);

    ಆದರೆ (display_list)
    {
	ಸಂದೇಶ_didany = FALSE;		// lines_left will be set
	ಸಂದೇಶ_ಆರಂಭ();			// prepare for paging
	ಸಂದೇಶ_putchar('\n');
	ಹೊರಹೋಗು_flush();
	cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	ಸಂದೇಶ_didany = FALSE;		// lines_left will be set again
	ಸಂದೇಶ_ಆರಂಭ();			// prepare for paging
    }

    ಆದರೆ (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (display_wildmenu && !display_list)
	win_redr_ಸ್ಥಿತಿ_matches(xp, ಅಂಕೆ_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_Matches; ++i)
	{
	    ಪೂರ್ಣಾಂಕ	len;
	    ಆದರೆ (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(ಖಾಲಿ, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		len = vim_strsize(SHOW_ಹೊಂದಿಕೆ(i));
	    ಆದರೆ (len > maxlen)
		maxlen = len;
	}

	ಆದರೆ (xp->xp_context == EXPAND_ಟ್ಯಾಗುಗಳು_LISTFILES)
	    ಸಾಲುಗಳು = ಅಂಕೆ_Matches;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_ಅಗಲ + 2) / maxlen;
	    ಆದರೆ (columns < 1)
		columns = 1;
	    ಸಾಲುಗಳು = (ಅಂಕೆ_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	ಆದರೆ (xp->xp_context == EXPAND_ಟ್ಯಾಗುಗಳು_LISTFILES)
	{
	    ಸಂದೇಶ_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    ಸಂದೇಶ_clr_eos();
	    ಸಂದೇಶ_advance(maxlen - 3);
	    ಸಂದೇಶ_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	ಗಾಗಿ (i = 0; i < ಸಾಲುಗಳು; ++i)
	{
	    showmatches_oneline(xp, matches, ಅಂಕೆ_Matches, ಸಾಲುಗಳು, i,
						maxlen, showtail, attr);
	    ಆದರೆ (got_int)
	    {
		got_int = FALSE;
		ನಿಲ್ಲಿಸು;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    ಆದರೆ (display_wildmenu && display_list)
    {
	ಆದರೆ (vim_strchr(p_wop, WOP_PUM) != ಖಾಲಿ)
	    (ಶೂನ್ಯ)show_pum_matches(ccline, xp, matches, ಅಂಕೆ_Matches,
		    showtail && !cmdline_unchanged, noselect);
	ಇಲ್ಲದಿದ್ದರೆ
	    win_redr_ಸ್ಥಿತಿ_matches(xp, ಅಂಕೆ_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    ಆದರೆ (xp->xp_numfiles == -1)
    {
	FreeWild(ಅಂಕೆ_Matches, matches);
	free_xp_files_extra(xp, ಅಂಕೆ_Matches);
    }

    ಹಿಂತಿರುಗಿ EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    ಸ್ಥಿರ char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    ಪೂರ್ಣಾಂಕ		had_sep = FALSE;

    ಗಾಗಿ (p = s; *p != NUL; )
    {
	ಆದರೆ (vim_ispathsep(*p)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
		&& !rem_backslash(p)
#ಆದರೆ_ಅಂತ್ಯ
	   )
	    had_sep = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    ಹಿಂತಿರುಗಿ t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*ಕೊನೆ;

    // When not completing file names a "/" may mean something different.
    ಆದರೆ (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	ಹಿಂತಿರುಗಿ FALSE;

    ಕೊನೆ = gettail(xp->xp_ನಮೂನೆ);
    ಆದರೆ (ಕೊನೆ == xp->xp_ನಮೂನೆ)		// there is no path separator
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (s = xp->xp_ನಮೂನೆ; s < ಕೊನೆ; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	ಆದರೆ (rem_backslash(s))
	    ++s;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr((char_u *)"*?[", *s) != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Prepare a string for expansion.
 * When expanding file names: The string will be used with expand_wildcards().
 * Copy "fname[len]" into allocated memory and add a '*' at the end.
 * When expanding other names: The string will be used with regcomp().  Copy
 * the name into allocated memory and prepend "^".
 */
    char_u *
addstar(
    char_u	*fname,
    ಪೂರ್ಣಾಂಕ		len,
    ಪೂರ್ಣಾಂಕ		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		new_len;
    char_u	*ಬಾಲ;
    ಪೂರ್ಣಾಂಕ		ತುದಿಗಳು_ಒಳ_ನಕ್ಷತ್ರ;

    ಆದರೆ (context != EXPAND_FILES
	    && context != EXPAND_FILES_ಒಳ_ಮಾರ್ಗ
	    && context != EXPAND_SHELLCMD
	    && context != EXPAND_DIRECTORIES
	    && context != EXPAND_DIRS_ಒಳ_CDPATH)
    {
	// Matching will be done internally (on something other than files).
	// So we convert the file-matching-type wildcards into our kind for
	// use with vim_regcomp().  First work out how long it will be:

	// For help tags the translation is done in find_help_tags().
	// For a tag pattern starting with "/" no translation is needed.
	ಆದರೆ (context == EXPAND_FINDFUNC
		|| context == EXPAND_HELP
		|| context == EXPAND_COLORS
		|| context == EXPAND_COMPILER
		|| context == EXPAND_OWNSYNTAX
		|| context == EXPAND_FILETYPE
		|| context == EXPAND_KEYMAP
		|| context == EXPAND_PACKADD
		|| context == EXPAND_RUNTIME
		|| ((context == EXPAND_ಟ್ಯಾಗುಗಳು_LISTFILES
			|| context == EXPAND_ಟ್ಯಾಗುಗಳು)
		    && fname[0] == '/'))
	    retval = vim_strnsave(fname, len);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    ಗಾಗಿ (i = 0; i < len; i++)
	    {
		ಆದರೆ (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		ಆದರೆ (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		ಆದರೆ ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    ಆದರೆ (retval != ಖಾಲಿ)
	    {
		retval[0] = '^';
		j = 1;
		ಗಾಗಿ (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    ಆದರೆ (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			ನಿಲ್ಲಿಸು;

		    ಆಯ್ಕೆ (fname[i])
		    {
			ಸ್ಥಿತಿ '*':   retval[j++] = '.';
				    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ '~':   retval[j++] = '\\';
				    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ '?':   retval[j] = '.';
				    ಮುಂದುವರಿ;
			ಸ್ಥಿತಿ '.':   ಆದರೆ (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ '\\':  ಆದರೆ (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    ನಿಲ್ಲಿಸು;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	retval = alloc(len + 4);
	ಆದರೆ (retval != ಖಾಲಿ)
	{
	    vim_strncpy(retval, fname, len);

	    // Don't add a star to *, ~, ~user, $var or `cmd`.
	    // * would become **, which walks the whole tree.
	    // ~ would be at the start of the file name, but not the tail.
	    // $ could be anywhere in the tail.
	    // ` could be anywhere in the file name.
	    // When the name ends in '$' don't add a star, remove the '$'.
	    ಬಾಲ = gettail(retval);
	    ತುದಿಗಳು_ಒಳ_ನಕ್ಷತ್ರ = (len > 0 && retval[len - 1] == '*');
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ BACKSLASH_ಒಳ_FILENAME
	    ಗಾಗಿ (i = len - 2; i >= 0; --i)
	    {
		ಆದರೆ (retval[i] != '\\')
		    ನಿಲ್ಲಿಸು;
		ತುದಿಗಳು_ಒಳ_ನಕ್ಷತ್ರ = !ತುದಿಗಳು_ಒಳ_ನಕ್ಷತ್ರ;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ ((*retval != '~' || ಬಾಲ != retval)
		    && !ತುದಿಗಳು_ಒಳ_ನಕ್ಷತ್ರ
		    && vim_strchr(ಬಾಲ, '$') == ಖಾಲಿ
		    && vim_strchr(retval, '`') == ಖಾಲಿ)
		retval[len++] = '*';
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Must parse the command line so far to work out what context we are in.
 * Completion can then be done based on that context.
 * This routine sets the variables:
 *  xp->xp_pattern	    The start of the pattern to be expanded within
 *				the command line (ends at the cursor).
 *  xp->xp_context	    The type of thing to expand.  Will be one of:
 *
 *  EXPAND_UNSUCCESSFUL	    Used sometimes when there is something illegal on
 *			    the command line, like an unknown command.	Caller
 *			    should beep.
 *  EXPAND_NOTHING	    Unrecognised context for completion, use char like
 *			    a normal char, rather than for completion.	eg
 *			    :s/^I/
 *  EXPAND_COMMANDS	    Cursor is still touching the command, so complete
 *			    it.
 *  EXPAND_BUFFERS	    Complete file names for :buf and :sbuf commands.
 *  EXPAND_FILES	    After command with EX_XFILE set, or after setting
 *			    with P_EXPAND set.	eg :e ^I, :w>>^I
 *  EXPAND_DIRECTORIES	    In some cases this is used instead of the latter
 *			    when we know only directories are of interest.
 *			    E.g.  :set dir=^I  and  :cd ^I
 *  EXPAND_SHELLCMD	    After ":!cmd", ":r !cmd"  or ":w !cmd".
 *  EXPAND_SETTINGS	    Complete variable names.  eg :set d^I
 *  EXPAND_BOOL_SETTINGS    Complete boolean variables only,  eg :set no^I
 *  EXPAND_TAGS		    Complete tags from the files in p_tags.  eg :ta a^I
 *  EXPAND_TAGS_LISTFILES   As above, but list filenames on ^D, after :tselect
 *  EXPAND_HELP		    Complete tags from the file 'helpfile'/tags
 *  EXPAND_EVENTS	    Complete event names
 *  EXPAND_SYNTAX	    Complete :syntax command arguments
 *  EXPAND_HIGHLIGHT	    Complete highlight (syntax) group names
 *  EXPAND_AUGROUP	    Complete autocommand group names
 *  EXPAND_USER_VARS	    Complete user defined variable names, eg :unlet a^I
 *  EXPAND_MAPPINGS	    Complete mapping and abbreviation names,
 *			      eg :unmap a^I , :cunab x^I
 *  EXPAND_FUNCTIONS	    Complete internal or user defined function names,
 *			      eg :call sub^I
 *  EXPAND_USER_FUNC	    Complete user defined function names, eg :delf F^I
 *  EXPAND_EXPRESSION	    Complete internal or user defined function/variable
 *			    names in expressions, eg :while s^I
 *  EXPAND_ENV_VARS	    Complete environment variable names
 *  EXPAND_USER		    Complete user names
 *  EXPAND_PATTERN_IN_BUF   Complete pattern in '/', '?', ':s', ':g', etc.
 */
    ಶೂನ್ಯ
ಗಣ_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    ಆದರೆ ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_ನಮೂನೆ)
    {
	xp->xp_context = EXPAND_ನಮೂನೆ_ಒಳ_BUF;
	xp->xp_ಹುಡುಕು_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_ನಮೂನೆ = ccline->cmdbuff;
	xp->xp_ನಮೂನೆ_len = ccline->cmdpos;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	ಹುಡುಕು_ಮೊದಲ_ಸಾಲು = 0; // Search entire buffer
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    ಆದರೆ (ccline->cmdfirstc != ':'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->ಒಳಹರಿವು_fn
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	ಹಿಂತಿರುಗಿ;
    }

    // Fallback to command-line expansion
    ಗಣ_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, TRUE);
}

/*
 * Sets the index of a built-in or user defined command 'cmd' in eap->cmdidx.
 * For user defined commands, the completion context is set in 'xp' and the
 * completion flags in 'complp'.
 *
 * Returns a pointer to the text after the command or NULL for failure.
 */
    ಸ್ಥಿರ char_u *
ಗಣ_cmd_ಸೂಚ್ಯಂಕ(char_u *cmd, exarg_T *eap, expand_T *xp, ಪೂರ್ಣಾಂಕ *complp)
{
    char_u	*p = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    ಆದರೆ (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = cmd;
	ಆಗ (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	ಆದರೆ (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    ಆಗ (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	ಆದರೆ (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    ಆಗ (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	ಆದರೆ (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != ಖಾಲಿ)
	    ++p;
	len = (ಪೂರ್ಣಾಂಕ)(p - cmd);

	ಆದರೆ (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	ಆದರೆ ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    ಆಗ (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    ಆದರೆ (*p == NUL && ASCII_ISALNUM(p[-1]))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (eap->cmdidx == CMD_ಗಾತ್ರ)
    {
	ಆದರೆ (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != ಖಾಲಿ)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = ಕಂಡುಹಿಡಿ_ucmd(eap, p, ಖಾಲಿ, xp, complp);
	    ಆದರೆ (p == ಖಾಲಿ)
		eap->cmdidx = CMD_ಗಾತ್ರ;	// ambiguous user command
	}
    }
    ಆದರೆ (eap->cmdidx == CMD_ಗಾತ್ರ)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	ಪೂರ್ಣಾಂಕ		usefilter,
	expand_T	*xp,
	ಪೂರ್ಣಾಂಕ		*complp)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		ಒಳ_quote = FALSE;
    char_u	*bow = ಖಾಲಿ;	// Beginning of word
    ಪೂರ್ಣಾಂಕ		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_ನಮೂನೆ = skipwhite(arg);
    p = xp->xp_ನಮೂನೆ;
    ಆಗ (*p != NUL)
    {
	ಆದರೆ (has_mbyte)
	    c = mb_ptr2char(p);
	ಇಲ್ಲದಿದ್ದರೆ
	    c = *p;
	ಆದರೆ (c == '\\' && p[1] != NUL)
	    ++p;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '`')
	{
	    ಆದರೆ (!ಒಳ_quote)
	    {
		xp->xp_ನಮೂನೆ = p;
		bow = p + 1;
	    }
	    ಒಳ_quote = !ಒಳ_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != ಖಾಲಿ && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    ಆಗ (*p != NUL)
	    {
		ಆದರೆ (has_mbyte)
		    c = mb_ptr2char(p);
		ಇಲ್ಲದಿದ್ದರೆ
		    c = *p;
		ಆದರೆ (c == '`' || vim_isfilec_or_wc(c))
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (has_mbyte)
		    len = (*mb_ptr2len)(p);
		ಇಲ್ಲದಿದ್ದರೆ
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    ಆದರೆ (ಒಳ_quote)
		bow = p;
	    ಇಲ್ಲದಿದ್ದರೆ
		xp->xp_ನಮೂನೆ = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    ಆದರೆ (bow != ಖಾಲಿ && ಒಳ_quote)
	xp->xp_ನಮೂನೆ = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    ಆದರೆ (usefilter
	    || (eap != ಖಾಲಿ
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ BACKSLASH_ಒಳ_FILENAME
	xp->xp_shell = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	// When still after the command name expand executables.
	ಆದರೆ (xp->xp_ನಮೂನೆ == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    ಆದರೆ (*xp->xp_ನಮೂನೆ == '$')
    {
	ಗಾಗಿ (p = xp->xp_ನಮೂನೆ + 1; *p != NUL; ++p)
	    ಆದರೆ (!vim_isIDc(*p))
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_ನಮೂನೆ;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    ಆದರೆ (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    ಆದರೆ (*xp->xp_ನಮೂನೆ == '~')
    {
	ಗಾಗಿ (p = xp->xp_ನಮೂನೆ + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	ಆದರೆ (*p == NUL && p > xp->xp_ನಮೂನೆ + 1
		&& ಹೊಂದಿಕೆ_user(xp->xp_ನಮೂನೆ + 1) >= 1)
	{
	    xp->xp_context = EXPAND_USER;
	    ++xp->xp_ನಮೂನೆ;
	}
    }
}

/*
 * Set the completion context for the "++opt=arg" argument.  Always returns
 * NULL.
 */
    ಸ್ಥಿರ char_u *
ಗಣ_context_ಒಳ_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    ಆದರೆ (p == ಖಾಲಿ)
	xp->xp_ನಮೂನೆ = arg;
    ಇಲ್ಲದಿದ್ದರೆ
	xp->xp_ನಮೂನೆ = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    ಸ್ಥಿರ char_u *
ಗಣ_context_ಒಳ_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    ಆದರೆ (p == ಖಾಲಿ)
	xp->xp_ನಮೂನೆ = arg;
    ಇಲ್ಲದಿದ್ದರೆ
	xp->xp_ನಮೂನೆ = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    ಸ್ಥಿರ char_u *
ಗಣ_context_ಒಳ_filter_cmd(expand_T *xp, char_u *arg)
{
    ಆದರೆ (*arg != NUL)
	arg = skip_vimgrep_pat(arg, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (arg == ಖಾಲಿ || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ skipwhite(arg);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    ಸ್ಥಿರ char_u *
ಗಣ_context_ಒಳ_ಹೊಂದಿಕೆ_cmd(expand_T *xp, char_u *arg)
{
    ಆದರೆ (*arg == NUL || !ತುದಿಗಳು_excmd(*arg))
    {
	// also complete "None"
	ಗಣ_context_ಒಳ_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	ಆದರೆ (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_nextcmd(arg);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    ಸ್ಥಿರ char_u *
ಕಂಡುಹಿಡಿ_cmd_after_global_cmd(char_u *arg)
{
    ಪೂರ್ಣಾಂಕ		delim;

    delim = *arg;	    // get the delimiter
    ಆದರೆ (delim)
	++arg;		    // skip delimiter if there is one

    ಆಗ (arg[0] != NUL && arg[0] != delim)
    {
	ಆದರೆ (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    ಆದರೆ (arg[0] != NUL)
	ಹಿಂತಿರುಗಿ arg + 1;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    ಸ್ಥಿರ char_u *
ಕಂಡುಹಿಡಿ_cmd_after_substitute_cmd(char_u *arg)
{
    ಪೂರ್ಣಾಂಕ		delim;

    delim = *arg;
    ಆದರೆ (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	ಆದರೆ (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    ಆಗ (arg[0] != NUL && arg[0] != delim)
	    {
		ಆದರೆ (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    ಆದರೆ (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    ಆಗ (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == ಖಾಲಿ)
	++arg;
    ಆದರೆ (arg[0] != NUL)
	ಹಿಂತಿರುಗಿ arg;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    ಸ್ಥಿರ char_u *
ಕಂಡುಹಿಡಿ_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    ಆದರೆ (*arg != '/')
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Match regexp, not just whole words
    ಗಾಗಿ (++arg; *arg && *arg != '/'; arg++)
	ಆದರೆ (*arg == '\\' && arg[1] != NUL)
	    arg++;
    ಆದರೆ (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	ಆದರೆ (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == ಖಾಲಿ)
	    xp->xp_context = EXPAND_NOTHING;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ arg;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    ಸ್ಥಿರ char_u *
ಗಣ_context_ಒಳ_unlet_cmd(expand_T *xp, char_u *arg)
{
    ಆಗ ((xp->xp_ನಮೂನೆ = vim_strchr(arg, ' ')) != ಖಾಲಿ)
	arg = xp->xp_ನಮೂನೆ + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_ನಮೂನೆ = arg;

    ಆದರೆ (*xp->xp_ನಮೂನೆ == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_ನಮೂನೆ;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    ಸ್ಥಿರ char_u *
ಗಣ_context_ಒಳ_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    ಆದರೆ (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_ನಮೂನೆ = arg;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_ನಮೂನೆ = skipwhite(p);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    xp->xp_context = EXPAND_NOTHING;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಎಣಿಕೆ
{
    EXP_FILETYPECMD_ALL,	// expand all :filetype values
    EXP_FILETYPECMD_PLUGIN,	// expand plugin on off
    EXP_FILETYPECMD_INDENT,	// expand indent on off
    EXP_FILETYPECMD_ONOFF,	// expand on off
} filetype_expand_what;

#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_FILETYPECMD_PLUGIN 0x01
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_FILETYPECMD_INDENT 0x02
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_FILETYPECMD_ONOFF  0x04

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಎಣಿಕೆ
{
    EXP_BREAKPT_ಕೂಡಿಸು,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    ಸ್ಥಿರ char_u *
ಗಣ_context_ಒಳ_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_ಆರಂಭ;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_ನಮೂನೆ = arg;

    ಆದರೆ (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ಕೂಡಿಸು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    ಇಲ್ಲದಿದ್ದರೆ
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    subcmd_ಆರಂಭ = p;

    ಆದರೆ (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	ಆದರೆ (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    ಆದರೆ (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    p = skipwhite(p);
	}
	ಆದರೆ (STRNCMP("file", subcmd_ಆರಂಭ, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	ಇಲ್ಲದಿದ್ದರೆ
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_ನಮೂನೆ = p;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_ಅಭಿವ್ಯಕ್ತಿ;
	xp->xp_ನಮೂನೆ = skipwhite(p + 5);
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ char_u *
ಗಣ_context_ಒಳ_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_ನಮೂನೆ = ಖಾಲಿ;

    p = skipwhite(arg);
    ಆದರೆ (VIM_ISDIGIT(*p))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_ನಮೂನೆ = p;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    ಸ್ಥಿರ char_u *
ಗಣ_context_ಒಳ_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    ಪೂರ್ಣಾಂಕ    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_ನಮೂನೆ = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    ಮುಂದುವರಿ;
	}
	ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_ನಮೂನೆ = p;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_context_with_ನಮೂನೆ(expand_T *xp)
{
    ಪೂರ್ಣಾಂಕ		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    ಪೂರ್ಣಾಂಕ		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_ನಮೂನೆ_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    ಆದರೆ (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

    xp->xp_ನಮೂನೆ = ccline->cmdbuff + skiplen;
    xp->xp_ನಮೂನೆ_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_ನಮೂನೆ_ಒಳ_BUF;
    xp->xp_ಹುಡುಕು_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    ಸ್ಥಿರ char_u *
ಗಣ_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	ದೀರ್ಘ		argt,
	ಪೂರ್ಣಾಂಕ		compl,
	ಪೂರ್ಣಾಂಕ		forceit)
{
    char_u  *nextcmd;

    ಆಯ್ಕೆ (cmdidx)
    {
	ಸ್ಥಿತಿ CMD_ಕಂಡುಹಿಡಿ:
	ಸ್ಥಿತಿ CMD_sfind:
	ಸ್ಥಿತಿ CMD_tabfind:
	    ಆದರೆ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_ಒಳ_ಮಾರ್ಗ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_cd:
	ಸ್ಥಿತಿ CMD_chdir:
	ಸ್ಥಿತಿ CMD_tcd:
	ಸ್ಥಿತಿ CMD_tchdir:
	ಸ್ಥಿತಿ CMD_lcd:
	ಸ್ಥಿತಿ CMD_lchdir:
	    ಆದರೆ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_ಒಳ_CDPATH;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	ಸ್ಥಿತಿ CMD_aboveleft:
	ಸ್ಥಿತಿ CMD_argdo:
	ಸ್ಥಿತಿ CMD_belowright:
	ಸ್ಥಿತಿ CMD_botright:
	ಸ್ಥಿತಿ CMD_browse:
	ಸ್ಥಿತಿ CMD_bufdo:
	ಸ್ಥಿತಿ CMD_cdo:
	ಸ್ಥಿತಿ CMD_cfdo:
	ಸ್ಥಿತಿ CMD_confirm:
	ಸ್ಥಿತಿ CMD_debug:
	ಸ್ಥಿತಿ CMD_folddoclosed:
	ಸ್ಥಿತಿ CMD_folddoopen:
	ಸ್ಥಿತಿ CMD_hide:
	ಸ್ಥಿತಿ CMD_horizontal:
	ಸ್ಥಿತಿ CMD_keepalt:
	ಸ್ಥಿತಿ CMD_keepjumps:
	ಸ್ಥಿತಿ CMD_keepmarks:
	ಸ್ಥಿತಿ CMD_keeppatterns:
	ಸ್ಥಿತಿ CMD_ldo:
	ಸ್ಥಿತಿ CMD_leftabove:
	ಸ್ಥಿತಿ CMD_lfdo:
	ಸ್ಥಿತಿ CMD_lockmarks:
	ಸ್ಥಿತಿ CMD_noautocmd:
	ಸ್ಥಿತಿ CMD_noswapfile:
	ಸ್ಥಿತಿ CMD_rightbelow:
	ಸ್ಥಿತಿ CMD_sandbox:
	ಸ್ಥಿತಿ CMD_silent:
	ಸ್ಥಿತಿ CMD_tab:
	ಸ್ಥಿತಿ CMD_tabdo:
	ಸ್ಥಿತಿ CMD_topleft:
	ಸ್ಥಿತಿ CMD_unsilent:
	ಸ್ಥಿತಿ CMD_verbose:
	ಸ್ಥಿತಿ CMD_vertical:
	ಸ್ಥಿತಿ CMD_windo:
	ಸ್ಥಿತಿ CMD_vim9cmd:
	ಸ್ಥಿತಿ CMD_legacy:
	    ಹಿಂತಿರುಗಿ arg;

	ಸ್ಥಿತಿ CMD_filter:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_filter_cmd(xp, arg);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	ಸ್ಥಿತಿ CMD_ಹೊಂದಿಕೆ:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_ಹೊಂದಿಕೆ_cmd(xp, arg);
#ಆದರೆ_ಅಂತ್ಯ

	// All completion for the +cmdline_compl feature goes here.

	ಸ್ಥಿತಿ CMD_command:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_user_cmd(xp, arg);

	ಸ್ಥಿತಿ CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_global:
	ಸ್ಥಿತಿ CMD_vglobal:
	    nextcmd = ಕಂಡುಹಿಡಿ_cmd_after_global_cmd(arg);
	    ಆದರೆ (!nextcmd && may_expand_ನಮೂನೆ)
		ಗಣ_context_with_ನಮೂನೆ(xp);
	    ಹಿಂತಿರುಗಿ nextcmd;

	ಸ್ಥಿತಿ CMD_and:
	ಸ್ಥಿತಿ CMD_substitute:
	    nextcmd = ಕಂಡುಹಿಡಿ_cmd_after_substitute_cmd(arg);
	    ಆದರೆ (!nextcmd && may_expand_ನಮೂನೆ)
		ಗಣ_context_with_ನಮೂನೆ(xp);
	    ಹಿಂತಿರುಗಿ nextcmd;

	ಸ್ಥಿತಿ CMD_isearch:
	ಸ್ಥಿತಿ CMD_dsearch:
	ಸ್ಥಿತಿ CMD_ilist:
	ಸ್ಥಿತಿ CMD_dlist:
	ಸ್ಥಿತಿ CMD_ijump:
	ಸ್ಥಿತಿ CMD_psearch:
	ಸ್ಥಿತಿ CMD_djump:
	ಸ್ಥಿತಿ CMD_isplit:
	ಸ್ಥಿತಿ CMD_dsplit:
	    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_cmd_after_isearch_cmd(xp, arg);
	ಸ್ಥಿತಿ CMD_autocmd:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_autocmd(xp, arg, FALSE);
	ಸ್ಥಿತಿ CMD_doautocmd:
	ಸ್ಥಿತಿ CMD_doautoall:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_autocmd(xp, arg, TRUE);
	ಸ್ಥಿತಿ CMD_ಗಣ:
	    ಗಣ_context_ಒಳ_ಗಣ_cmd(xp, arg, 0);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_setglobal:
	    ಗಣ_context_ಒಳ_ಗಣ_cmd(xp, arg, OPT_GLOBAL);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_setlocal:
	    ಗಣ_context_ಒಳ_ಗಣ_cmd(xp, arg, OPT_LOCAL);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_ಟ್ಯಾಗ್:
	ಸ್ಥಿತಿ CMD_stag:
	ಸ್ಥಿತಿ CMD_ptag:
	ಸ್ಥಿತಿ CMD_ltag:
	ಸ್ಥಿತಿ CMD_tselect:
	ಸ್ಥಿತಿ CMD_stselect:
	ಸ್ಥಿತಿ CMD_ptselect:
	ಸ್ಥಿತಿ CMD_tjump:
	ಸ್ಥಿತಿ CMD_stjump:
	ಸ್ಥಿತಿ CMD_ptjump:
	    ಆದರೆ (vim_strchr(p_wop, WOP_TAGFILE) != ಖಾಲಿ)
		xp->xp_context = EXPAND_ಟ್ಯಾಗುಗಳು_LISTFILES;
	    ಇಲ್ಲದಿದ್ದರೆ
		xp->xp_context = EXPAND_ಟ್ಯಾಗುಗಳು;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಸ್ಥಿತಿ CMD_syntax:
	    ಗಣ_context_ಒಳ_syntax_cmd(xp, arg);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಸ್ಥಿತಿ CMD_final:
	ಸ್ಥಿತಿ CMD_const:
	ಸ್ಥಿತಿ CMD_let:
	ಸ್ಥಿತಿ CMD_var:
	ಸ್ಥಿತಿ CMD_if:
	ಸ್ಥಿತಿ CMD_elseif:
	ಸ್ಥಿತಿ CMD_while:
	ಸ್ಥಿತಿ CMD_for:
	ಸ್ಥಿತಿ CMD_echo:
	ಸ್ಥಿತಿ CMD_echon:
	ಸ್ಥಿತಿ CMD_execute:
	ಸ್ಥಿತಿ CMD_echomsg:
	ಸ್ಥಿತಿ CMD_echoerr:
	ಸ್ಥಿತಿ CMD_echoconsole:
	ಸ್ಥಿತಿ CMD_echowindow:
	ಸ್ಥಿತಿ CMD_call:
	ಸ್ಥಿತಿ CMD_return:
	ಸ್ಥಿತಿ CMD_cexpr:
	ಸ್ಥಿತಿ CMD_caddexpr:
	ಸ್ಥಿತಿ CMD_cgetexpr:
	ಸ್ಥಿತಿ CMD_lexpr:
	ಸ್ಥಿತಿ CMD_laddexpr:
	ಸ್ಥಿತಿ CMD_lgetexpr:
	    ಗಣ_context_for_ಅಭಿವ್ಯಕ್ತಿ(xp, arg, cmdidx);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_unlet:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_unlet_cmd(xp, arg);
	ಸ್ಥಿತಿ CMD_function:
	ಸ್ಥಿತಿ CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_disassemble:
	    ಗಣ_context_ಒಳ_disassemble_cmd(xp, arg);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_echohl:
	    ಗಣ_context_ಒಳ_echohl_cmd(xp, arg);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ CMD_highlight:
	    ಗಣ_context_ಒಳ_highlight_cmd(xp, arg);
	    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
	ಸ್ಥಿತಿ CMD_cscope:
	ಸ್ಥಿತಿ CMD_lcscope:
	ಸ್ಥಿತಿ CMD_scscope:
	    ಗಣ_context_ಒಳ_cscope_cmd(xp, arg, cmdidx);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	ಸ್ಥಿತಿ CMD_sign:
	    ಗಣ_context_ಒಳ_sign_cmd(xp, arg);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ CMD_bdelete:
	ಸ್ಥಿತಿ CMD_bwipeout:
	ಸ್ಥಿತಿ CMD_bunload:
	    ಆಗ ((xp->xp_ನಮೂನೆ = vim_strchr(arg, ' ')) != ಖಾಲಿ)
		arg = xp->xp_ನಮೂನೆ + 1;
	    // FALLTHROUGH
	ಸ್ಥಿತಿ CMD_ಬಫರ್:
	ಸ್ಥಿತಿ CMD_sbuffer:
	ಸ್ಥಿತಿ CMD_pbuffer:
	ಸ್ಥಿತಿ CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	ಸ್ಥಿತಿ CMD_diffget:
	ಸ್ಥಿತಿ CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ CMD_USER:
	ಸ್ಥಿತಿ CMD_USER_BUF:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	ಸ್ಥಿತಿ CMD_map:	    ಸ್ಥಿತಿ CMD_noremap:
	ಸ್ಥಿತಿ CMD_nmap:	    ಸ್ಥಿತಿ CMD_nnoremap:
	ಸ್ಥಿತಿ CMD_vmap:	    ಸ್ಥಿತಿ CMD_vnoremap:
	ಸ್ಥಿತಿ CMD_omap:	    ಸ್ಥಿತಿ CMD_onoremap:
	ಸ್ಥಿತಿ CMD_imap:	    ಸ್ಥಿತಿ CMD_inoremap:
	ಸ್ಥಿತಿ CMD_cmap:	    ಸ್ಥಿತಿ CMD_cnoremap:
	ಸ್ಥಿತಿ CMD_lmap:	    ಸ್ಥಿತಿ CMD_lnoremap:
	ಸ್ಥಿತಿ CMD_smap:	    ಸ್ಥಿತಿ CMD_snoremap:
	ಸ್ಥಿತಿ CMD_tmap:	    ಸ್ಥಿತಿ CMD_tnoremap:
	ಸ್ಥಿತಿ CMD_xmap:	    ಸ್ಥಿತಿ CMD_xnoremap:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	ಸ್ಥಿತಿ CMD_unmap:
	ಸ್ಥಿತಿ CMD_nunmap:
	ಸ್ಥಿತಿ CMD_vunmap:
	ಸ್ಥಿತಿ CMD_ounmap:
	ಸ್ಥಿತಿ CMD_iunmap:
	ಸ್ಥಿತಿ CMD_cunmap:
	ಸ್ಥಿತಿ CMD_lunmap:
	ಸ್ಥಿತಿ CMD_sunmap:
	ಸ್ಥಿತಿ CMD_tunmap:
	ಸ್ಥಿತಿ CMD_xunmap:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	ಸ್ಥಿತಿ CMD_mapclear:
	ಸ್ಥಿತಿ CMD_nmapclear:
	ಸ್ಥಿತಿ CMD_vmapclear:
	ಸ್ಥಿತಿ CMD_omapclear:
	ಸ್ಥಿತಿ CMD_imapclear:
	ಸ್ಥಿತಿ CMD_cmapclear:
	ಸ್ಥಿತಿ CMD_lmapclear:
	ಸ್ಥಿತಿ CMD_smapclear:
	ಸ್ಥಿತಿ CMD_tmapclear:
	ಸ್ಥಿತಿ CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_abbreviate:	ಸ್ಥಿತಿ CMD_noreabbrev:
	ಸ್ಥಿತಿ CMD_cabbrev:	ಸ್ಥಿತಿ CMD_cnoreabbrev:
	ಸ್ಥಿತಿ CMD_iabbrev:	ಸ್ಥಿತಿ CMD_inoreabbrev:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	ಸ್ಥಿತಿ CMD_unabbreviate:
	ಸ್ಥಿತಿ CMD_cunabbrev:
	ಸ್ಥಿತಿ CMD_iunabbrev:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	ಸ್ಥಿತಿ CMD_menu:	    ಸ್ಥಿತಿ CMD_noremenu:	    ಸ್ಥಿತಿ CMD_unmenu:
	ಸ್ಥಿತಿ CMD_amenu:	    ಸ್ಥಿತಿ CMD_anoremenu:	    ಸ್ಥಿತಿ CMD_aunmenu:
	ಸ್ಥಿತಿ CMD_nmenu:	    ಸ್ಥಿತಿ CMD_nnoremenu:	    ಸ್ಥಿತಿ CMD_nunmenu:
	ಸ್ಥಿತಿ CMD_vmenu:	    ಸ್ಥಿತಿ CMD_vnoremenu:	    ಸ್ಥಿತಿ CMD_vunmenu:
	ಸ್ಥಿತಿ CMD_omenu:	    ಸ್ಥಿತಿ CMD_onoremenu:	    ಸ್ಥಿತಿ CMD_ounmenu:
	ಸ್ಥಿತಿ CMD_imenu:	    ಸ್ಥಿತಿ CMD_inoremenu:	    ಸ್ಥಿತಿ CMD_iunmenu:
	ಸ್ಥಿತಿ CMD_cmenu:	    ಸ್ಥಿತಿ CMD_cnoremenu:	    ಸ್ಥಿತಿ CMD_cunmenu:
	ಸ್ಥಿತಿ CMD_tlmenu:    ಸ್ಥಿತಿ CMD_tlnoremenu:    ಸ್ಥಿತಿ CMD_tlunmenu:
	ಸ್ಥಿತಿ CMD_tmenu:				    ಸ್ಥಿತಿ CMD_tunmenu:
	ಸ್ಥಿತಿ CMD_popup:	    ಸ್ಥಿತಿ CMD_tearoff:	    ಸ್ಥಿತಿ CMD_emenu:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_menu_cmd(xp, cmd, arg, forceit);
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_runtime:
	    ಗಣ_context_ಒಳ_runtime_cmd(xp, arg);
	    ನಿಲ್ಲಿಸು;

#ಆದರೆ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	ಸ್ಥಿತಿ CMD_language:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_lang_cmd(xp, arg);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PROFILE)
	ಸ್ಥಿತಿ CMD_profile:
	    ಗಣ_context_ಒಳ_profile_cmd(xp, arg);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ defined(FEAT_PROFILE)
	ಸ್ಥಿತಿ CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ CMD_argdelete:
	    ಆಗ ((xp->xp_ನಮೂನೆ = vim_strchr(arg, ' ')) != ಖಾಲಿ)
		arg = xp->xp_ನಮೂನೆ + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_ನಮೂನೆ = arg;
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಸ್ಥಿತಿ CMD_breakadd:
	ಸ್ಥಿತಿ CMD_profdel:
	ಸ್ಥಿತಿ CMD_breakdel:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_breakadd_cmd(xp, arg, cmdidx);

	ಸ್ಥಿತಿ CMD_scriptnames:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_scriptnames_cmd(xp, arg);
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ CMD_filetype:
	    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_filetype_cmd(xp, arg);

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    ಸ್ಥಿರ char_u *
ಗಣ_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    ಪೂರ್ಣಾಂಕ			len = 0;
    exarg_T		ea;
    ಪೂರ್ಣಾಂಕ			compl = EXPAND_NOTHING;
    ಪೂರ್ಣಾಂಕ			forceit = FALSE;
    ಪೂರ್ಣಾಂಕ			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_ನಮೂನೆ = buff;
    xp->xp_ಸಾಲು = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    ಗಾಗಿ (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != ಖಾಲಿ; cmd++)
	;
    xp->xp_ನಮೂನೆ = cmd;

    ಆದರೆ (*cmd == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_ನಮೂನೆ = cmd;
    ಆದರೆ (*cmd == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (*cmd == '|' || *cmd == '\n')
	ಹಿಂತಿರುಗಿ cmd + 1;			// There's another command

    // Get the command index.
    p = ಗಣ_cmd_ಸೂಚ್ಯಂಕ(cmd, &ea, xp, &compl);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    ಆದರೆ (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    ಆದರೆ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    ಆದರೆ ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	ಆಗ (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    ಆಗ (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    ಆದರೆ (*p == NUL)
	    {
		ಆದರೆ (ea.argt & EX_ARGOPT)
		    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_argopt(xp, arg + 2);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
		ಆದರೆ (ea.cmdidx == CMD_terminal)
		    ಹಿಂತಿರುಗಿ ಗಣ_context_ಒಳ_terminalopt(xp, arg + 2);
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    arg = skipwhite(p);
	}
    }

    ಆದರೆ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	ಆದರೆ (*arg == '>')			// append
	{
	    ಆದರೆ (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    ಆದರೆ (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	ಆದರೆ (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    ಆದರೆ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ಆಗ (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    ಆದರೆ ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	ಆದರೆ (*arg == NUL)
	    ಹಿಂತಿರುಗಿ p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    ಆದರೆ ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	ಆದರೆ (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	ಆಗ (*p)
	{
	    ಆದರೆ (*p == Ctrl_V)
	    {
		ಆದರೆ (p[1] != NUL)
		    ++p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		ಆದರೆ (*(p - 1) != '\\')
		{
		    ಆದರೆ (*p == '|' || *p == '\n')
			ಹಿಂತಿರುಗಿ p + 1;
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    ಆದರೆ (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == ಖಾಲಿ)
	// no arguments allowed but there is something
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_ನಮೂನೆ = p;
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(buff);
    ಆಗ (*p && p < buff + len)
    {
	ಆದರೆ (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_ನಮೂನೆ = ++p;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    ಆದರೆ (ea.argt & EX_XFILE)
	ಗಣ_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    ಹಿಂತಿರುಗಿ ಗಣ_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    ಶೂನ್ಯ
ಗಣ_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    ಪೂರ್ಣಾಂಕ		len,	    // length of command line (excl. NUL)
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,	    // position of cursor
    ಪೂರ್ಣಾಂಕ		use_ccline ಬಳಸದ) // use ccline for info
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    ಪೂರ್ಣಾಂಕ			context;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    ಆದರೆ (ಲಂಬಸಾಲು < len)
    {
	old_char = str[ಲಂಬಸಾಲು];
	str[ಲಂಬಸಾಲು] = NUL;
    }
    nextcomm = str;

    filetype_expand_what = EXP_FILETYPECMD_ALL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    breakpt_expand_what = EXP_BREAKPT_ಕೂಡಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	ಗಣ_context_for_ಅಭಿವ್ಯಕ್ತಿ(xp, str, CMD_ಗಾತ್ರ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (use_ccline && ccline->ಒಳಹರಿವು_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_ನಮೂನೆ = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	ಆದರೆ (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    ಗಣ_context_for_wildcard_arg(ಖಾಲಿ, xp->xp_ನಮೂನೆ, FALSE, xp,
								     &context);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆಗ (nextcomm != ಖಾಲಿ)
	    nextcomm = ಗಣ_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_ಸಾಲು = str;
    xp->xp_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;

    ಆದರೆ (ಲಂಬಸಾಲು < len)
	str[ಲಂಬಸಾಲು] = old_char;
}

/*
 * Expand the command line "str" from context "xp".
 * "xp" must have been set by set_cmd_context().
 * xp->xp_pattern points into "str", to where the text that is to be expanded
 * starts.
 * Returns EXPAND_UNSUCCESSFUL when there is something illegal before the
 * cursor.
 * Returns EXPAND_NOTHING when there is nothing to expand, might insert the
 * key that triggered expansion literally.
 * Returns EXPAND_OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,		// position of cursor
    ಪೂರ್ಣಾಂಕ		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		options = WILD_ಕೂಡಿಸು_SLASH|WILD_SILENT;

    ಆದರೆ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ಹಿಂತಿರುಗಿ EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    ಆದರೆ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ಹಿಂತಿರುಗಿ EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_ನಮೂನೆ_len = (ಪೂರ್ಣಾಂಕ)(str + ಲಂಬಸಾಲು - xp->xp_ನಮೂನೆ);
    {
	char_u	*pat_src = xp->xp_ನಮೂನೆ;
	ಪೂರ್ಣಾಂಕ	pat_len = xp->xp_ನಮೂನೆ_len;
#ಆದರೆ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	ಆದರೆ (unesc != ಖಾಲಿ)
	    pat_src = unesc;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	ಇಲ್ಲದಿದ್ದರೆ
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#ಆದರೆ defined(FEAT_EVAL)
	vim_free(unesc);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (file_str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ EXPAND_UNSUCCESSFUL;

    ಆದರೆ (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    ಆದರೆ (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = ಖಾಲಿ;
    }
    vim_free(file_str);

    ಹಿಂತಿರುಗಿ EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches,
	ಪೂರ್ಣಾಂಕ		flags,
	ಪೂರ್ಣಾಂಕ		options)
{
    ಪೂರ್ಣಾಂಕ		free_pat = FALSE;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    ಆದರೆ (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_ಕೊನೆ;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	ಆದರೆ (pat == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ret;

	pat_ಕೊನೆ = pat + pat_len;
	ಗಾಗಿ (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    ಆದರೆ (*p != '\\')
		ಮುಂದುವರಿ;

	    ಆದರೆ (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_ಕೊನೆ - from) + 1);   // +1 for NUL
		pat_ಕೊನೆ -= 3;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_ಕೊನೆ - from) + 1);   // +1 for NUL
		--pat_ಕೊನೆ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_backslash & XP_BS_COMMA)
	    {
		ಆದರೆ (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_ಕೊನೆ - from) + 1);   // +1 for NUL
		    pat_ಕೊನೆ -= 2;
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*(p + 1) == ',')
		{
		    from = p + 1;
		    mch_memmove(p, from,
			(size_t)(pat_ಕೊನೆ - from) + 1);   // +1 for NUL
		    --pat_ಕೊನೆ;
		}
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}
    }

    ಆದರೆ (xp->xp_context == EXPAND_FINDFUNC)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, ಅಂಕೆ_Matches);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_context == EXPAND_FILES_ಒಳ_ಮಾರ್ಗ)
	    flags |= (EW_FILE | EW_ಮಾರ್ಗ);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_context == EXPAND_DIRS_ಒಳ_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	ಇಲ್ಲದಿದ್ದರೆ
	    flags = (flags | EW_DIR) & ~EW_FILE;
	ಆದರೆ (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, ಅಂಕೆ_Matches, matches, flags);
    }
    ಆದರೆ (free_pat)
	vim_free(pat);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    ಆದರೆ (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	ಪೂರ್ಣಾಂಕ j;

	ಗಾಗಿ (j = 0; j < *ಅಂಕೆ_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    ಆಗ (*ptr != NUL)
	    {
		ಆದರೆ (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    ಸ್ಥಿರ char_u *
get_behave_arg(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx == 0)
	ಹಿಂತಿರುಗಿ (char_u *)"mswin";
    ಆದರೆ (idx == 1)
	ಹಿಂತಿರುಗಿ (char_u *)"xterm";
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    ಸ್ಥಿರ char_u *
get_filetypecmd_arg(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx < 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	ಅಕ್ಷರ	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	ಹಿಂತಿರುಗಿ (char_u *)opts_all[idx];
    }
    ಆದರೆ (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	ಅಕ್ಷರ	*opts_plugin[] = {"plugin", "on", "off"};
	ಹಿಂತಿರುಗಿ (char_u *)opts_plugin[idx];
    }
    ಆದರೆ (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	ಅಕ್ಷರ	*opts_indent[] = {"indent", "on", "off"};
	ಹಿಂತಿರುಗಿ (char_u *)opts_indent[idx];
    }
    ಆದರೆ (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	ಅಕ್ಷರ	*opts_onoff[] = {"on", "off"};
	ಹಿಂತಿರುಗಿ (char_u *)opts_onoff[idx];
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    ಸ್ಥಿರ char_u *
get_breakadd_arg(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx >= 0 && idx <= 3)
    {
	ಅಕ್ಷರ	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	ಆದರೆ (breakpt_expand_what == EXP_BREAKPT_ಕೂಡಿಸು)
	    ಹಿಂತಿರುಗಿ (char_u *)opts[idx];
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    ಆದರೆ (idx <= 2)
		ಹಿಂತಿರುಗಿ (char_u *)opts[idx + 1];
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // profdel {func, file}
	    ಆದರೆ (idx <= 1)
		ಹಿಂತಿರುಗಿ (char_u *)opts[idx + 1];
	}
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    ಸ್ಥಿರ char_u *
get_scriptnames_arg(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    scriptitem_T *si;

    ಆದರೆ (!SCRIPT_ID_VALID(idx + 1))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    si = SCRIPT_ಅಂಶ(idx + 1);
    home_replace(ಖಾಲಿ, si->sn_name, NameBuff, MAXPATHL, TRUE);
    ಹಿಂತಿರುಗಿ NameBuff;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    ಸ್ಥಿರ char_u *
get_retab_arg(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx == 0)
	ಹಿಂತಿರುಗಿ (char_u *)"-indentonly";
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    ಸ್ಥಿರ char_u *
get_messages_arg(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx == 0)
	ಹಿಂತಿರುಗಿ (char_u *)"clear";
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ char_u *
get_mapclear_arg(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx == 0)
	ಹಿಂತಿರುಗಿ (char_u *)"<buffer>";
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches)
{
    ಸ್ಥಿರ ರಚನೆ expgen
    {
	ಪೂರ್ಣಾಂಕ		context;
	char_u	*((*func)(expand_T *, ಪೂರ್ಣಾಂಕ));
	ಪೂರ್ಣಾಂಕ		ic;
	ಪೂರ್ಣಾಂಕ		escaped;
    } tab[] =
    {
	{EXPAND_COMMANDS, get_command_name, FALSE, TRUE},
	{EXPAND_BEHAVE, get_behave_arg, TRUE, TRUE},
	{EXPAND_FILETYPECMD, get_filetypecmd_arg, TRUE, TRUE},
	{EXPAND_MAPCLEAR, get_mapclear_arg, TRUE, TRUE},
	{EXPAND_MESSAGES, get_messages_arg, TRUE, TRUE},
	{EXPAND_HISTORY, get_history_arg, TRUE, TRUE},
	{EXPAND_USER_COMMANDS, get_user_commands, FALSE, TRUE},
	{EXPAND_USER_ADDR_TYPE, get_user_cmd_addr_type, FALSE, TRUE},
	{EXPAND_USER_CMD_FLAGS, get_user_cmd_flags, FALSE, TRUE},
	{EXPAND_USER_NARGS, get_user_cmd_nargs, FALSE, TRUE},
	{EXPAND_USER_COMPLETE, get_user_cmd_complete, FALSE, TRUE},
	{EXPAND_USER_COMPLETEOPT, get_user_cmd_completeopt, FALSE, TRUE},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	{EXPAND_USER_VARS, get_user_var_name, FALSE, TRUE},
	{EXPAND_FUNCTIONS, get_function_name, FALSE, TRUE},
	{EXPAND_USER_FUNC, get_user_func_name, FALSE, TRUE},
	{EXPAND_DISASSEMBLE, get_disassemble_ವಾದ, FALSE, TRUE},
	{EXPAND_ಅಭಿವ್ಯಕ್ತಿ, get_ಅಭಿವ್ಯಕ್ತಿ_name, FALSE, TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	{EXPAND_MENUS, get_menu_name, FALSE, TRUE},
	{EXPAND_MENUNAMES, get_menu_ಹೆಸರುಗಳು, FALSE, TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	{EXPAND_SYNTAX, get_syntax_name, TRUE, TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	{EXPAND_SYNTIME, get_syntime_arg, TRUE, TRUE},
#ಆದರೆ_ಅಂತ್ಯ
	{EXPAND_HIGHLIGHT, get_highlight_name, TRUE, TRUE},
	{EXPAND_EVENTS, get_event_name, TRUE, FALSE},
	{EXPAND_AUGROUP, get_augroup_name, TRUE, FALSE},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
	{EXPAND_CSCOPE, get_cscope_name, TRUE, TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	{EXPAND_SIGN, get_sign_name, TRUE, TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	{EXPAND_PROFILE, get_profile_name, TRUE, TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	{EXPAND_LANGUAGE, get_lang_arg, TRUE, FALSE},
	{EXPAND_LOCALES, get_locales, TRUE, FALSE},
#ಆದರೆ_ಅಂತ್ಯ
	{EXPAND_ENV_VARS, get_env_name, TRUE, TRUE},
	{EXPAND_USER, get_users, TRUE, FALSE},
	{EXPAND_ARGLIST, get_arglist_name, TRUE, FALSE},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	{EXPAND_BREAKPOINT, get_breakadd_arg, TRUE, TRUE},
	{EXPAND_SCRIPTNAMES, get_scriptnames_arg, TRUE, FALSE},
#ಆದರೆ_ಅಂತ್ಯ
	{EXPAND_RETAB, get_retab_arg, TRUE, TRUE},
    };
    ಪೂರ್ಣಾಂಕ	i;
    ಪೂರ್ಣಾಂಕ ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(tab); ++i)
    {
	ಆದರೆ (xp->xp_context == tab[i].context)
	{
	    ಆದರೆ (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, ಅಂಕೆ_Matches,
						tab[i].func, tab[i].escaped);
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
map_wildopts_ಗಮ್ಯ_ewflags(ಪೂರ್ಣಾಂಕ options)
{
    ಪೂರ್ಣಾಂಕ		flags;

    flags = EW_DIR;	// include directories
    ಆದರೆ (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    ಆದರೆ (options & WILD_ಕೂಡಿಸು_SLASH)
	flags |= EW_ADDSLASH;
    ಆದರೆ (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    ಆದರೆ (options & WILD_SILENT)
	flags |= EW_SILENT;
    ಆದರೆ (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    ಆದರೆ (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    ಹಿಂತಿರುಗಿ flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches,
    ಪೂರ್ಣಾಂಕ		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		ret;
    ಪೂರ್ಣಾಂಕ		flags;
    char_u	*tofree = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_ಗಮ್ಯ_ewflags(options);

    ಆದರೆ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_ಒಳ_ಮಾರ್ಗ
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_ಒಳ_CDPATH)
	ಹಿಂತಿರುಗಿ expand_files_and_dirs(xp, pat, matches, ಅಂಕೆ_Matches, flags,
								options);

    *matches = (char_u **)"";
    *ಅಂಕೆ_Matches = 0;
    ಆದರೆ (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	ಆದರೆ (ಕಂಡುಹಿಡಿ_help_ಟ್ಯಾಗುಗಳು(*pat == NUL ? (char_u *)"help" : pat,
					ಅಂಕೆ_Matches, matches, FALSE) == OK)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	    cleanup_help_ಟ್ಯಾಗುಗಳು(*ಅಂಕೆ_Matches, *matches);
#ಆದರೆ_ಅಂತ್ಯ
	    ಹಿಂತಿರುಗಿ OK;
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (xp->xp_context == EXPAND_SHELLCMD)
	ಹಿಂತಿರುಗಿ expand_shellcmd(pat, matches, ಅಂಕೆ_Matches, flags);
    ಆದರೆ (xp->xp_context == EXPAND_OLD_SETTING)
	ಹಿಂತಿರುಗಿ ExpandOldSetting(ಅಂಕೆ_Matches, matches);
    ಆದರೆ (xp->xp_context == EXPAND_BUFFERS)
	ಹಿಂತಿರುಗಿ ExpandBufnames(pat, ಅಂಕೆ_Matches, matches, options);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (xp->xp_context == EXPAND_DIFF_BUFFERS)
	ಹಿಂತಿರುಗಿ ExpandBufnames(pat, ಅಂಕೆ_Matches, matches,
						options | BUF_DIFF_FILTER);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (xp->xp_context == EXPAND_ಟ್ಯಾಗುಗಳು
	    || xp->xp_context == EXPAND_ಟ್ಯಾಗುಗಳು_LISTFILES)
	ಹಿಂತಿರುಗಿ expand_ಟ್ಯಾಗುಗಳು(xp->xp_context == EXPAND_ಟ್ಯಾಗುಗಳು, pat, ಅಂಕೆ_Matches,
								matches);
    ಆದರೆ (xp->xp_context == EXPAND_COLORS)
    {
	ಅಕ್ಷರ *directories[] = {"colors", ಖಾಲಿ};
	ಹಿಂತಿರುಗಿ ExpandRTDir(pat, DIP_ಆರಂಭ + DIP_OPT, ಅಂಕೆ_Matches, matches,
								directories);
    }
    ಆದರೆ (xp->xp_context == EXPAND_COMPILER)
    {
	ಅಕ್ಷರ *directories[] = {"compiler", ಖಾಲಿ};
	ಹಿಂತಿರುಗಿ ExpandRTDir(pat, 0, ಅಂಕೆ_Matches, matches, directories);
    }
    ಆದರೆ (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	ಅಕ್ಷರ *directories[] = {"syntax", ಖಾಲಿ};
	ಹಿಂತಿರುಗಿ ExpandRTDir(pat, 0, ಅಂಕೆ_Matches, matches, directories);
    }
    ಆದರೆ (xp->xp_context == EXPAND_FILETYPE)
    {
	ಅಕ್ಷರ *directories[] = {"syntax", "indent", "ftplugin", ಖಾಲಿ};
	ಹಿಂತಿರುಗಿ ExpandRTDir(pat, 0, ಅಂಕೆ_Matches, matches, directories);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
    ಆದರೆ (xp->xp_context == EXPAND_KEYMAP)
    {
	ಅಕ್ಷರ *directories[] = {"keymap", ಖಾಲಿ};
	ಹಿಂತಿರುಗಿ ExpandRTDir(pat, 0, ಅಂಕೆ_Matches, matches, directories);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ (xp->xp_context == EXPAND_USER_LIST)
	ಹಿಂತಿರುಗಿ ExpandUserList(xp, matches, ಅಂಕೆ_Matches);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (xp->xp_context == EXPAND_PACKADD)
	ಹಿಂತಿರುಗಿ Expand_Pack_ಕೂಡಿಸು_Dir(pat, ಅಂಕೆ_Matches, matches);
    ಆದರೆ (xp->xp_context == EXPAND_RUNTIME)
	ಹಿಂತಿರುಗಿ expand_runtime_cmd(pat, ಅಂಕೆ_Matches, matches);
    ಆದರೆ (xp->xp_context == EXPAND_ನಮೂನೆ_ಒಳ_BUF)
	ಹಿಂತಿರುಗಿ expand_ನಮೂನೆ_ಒಳ_buf(pat, xp->xp_ಹುಡುಕು_dir,
		matches, ಅಂಕೆ_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    ಆದರೆ ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	ಪೂರ್ಣಾಂಕ len = (ಪೂರ್ಣಾಂಕ)STRLEN(pat) + 20;

	tofree = alloc(len);
	ಆದರೆ (tofree == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	vim_snprintf((ಅಕ್ಷರ *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    ಆದರೆ (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
	{
	    vim_free(tofree);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    ಆದರೆ (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, ಅಂಕೆ_Matches, matches, fuzzy);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, ಅಂಕೆ_Matches, matches);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_context == EXPAND_SETTING_ಕಳೆ)
	ret = Expand_Setting_ಕಳೆ(xp, &regmatch, ಅಂಕೆ_Matches, matches);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, ಅಂಕೆ_Matches, matches);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, ಅಂಕೆ_Matches);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_context == EXPAND_HIGHLIGHT_ಗುಂಪು)
	ret = expand_highlight_ಗುಂಪು(pat, xp, &regmatch, matches, ಅಂಕೆ_Matches);
#ಆದರೆ defined(FEAT_TERMINAL)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, ಅಂಕೆ_Matches);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, ಅಂಕೆ_Matches);
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
	ret = ExpandOther(pat, xp, &regmatch, matches, ಅಂಕೆ_Matches);

    ಆದರೆ (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    ಹಿಂತಿರುಗಿ ret;
}

    ಪೂರ್ಣಾಂಕ
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches,
    char_u	*(*func)(expand_T *, ಪೂರ್ಣಾಂಕ),
					  // returns a string from the list
    ಪೂರ್ಣಾಂಕ		escaped)
{
    ಹಿಂತಿರುಗಿ ExpandGenericExt(
	pat, xp, regmatch, matches, ಅಂಕೆ_Matches, func, escaped, 0);
}

/*
 * Expand a list of names.
 *
 * Generic function for command line completion.  It calls a function to
 * obtain strings, one by one.	The strings are matched against a regexp
 * program.  Matching strings are copied into an array, which is returned.
 *
 * If 'fuzzy' is TRUE, then fuzzy matching is used. Otherwise, regex matching
 * is used.
 *
 * 'sortStartIdx' allows the caller to control sorting behavior. Items before
 * the index will not be sorted. Pass 0 to sort all, and -1 to prevent any
 * sorting.
 *
 * Returns OK when no problems encountered, FAIL for error (out of memory).
 */
    ಪೂರ್ಣಾಂಕ
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches,
    char_u	*(*func)(expand_T *, ಪೂರ್ಣಾಂಕ),
					  // returns a string from the list
    ಪೂರ್ಣಾಂಕ		escaped,
    ಪೂರ್ಣಾಂಕ		ವಿಂಗಡಿಸು_ಆರಂಭ_Idx)
{
    ಪೂರ್ಣಾಂಕ		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		score = 0;
    ಪೂರ್ಣಾಂಕ		fuzzy;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		ವಿಂಗಡಿಸು_matches = FALSE;
    ಪೂರ್ಣಾಂಕ		funcsort = FALSE;
    ಪೂರ್ಣಾಂಕ		ವಿಂಗಡಿಸು_ಆರಂಭ_ಹೊಂದಿಕೆ_Idx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ಖಾಲಿ;
    *ಅಂಕೆ_Matches = 0;

    ಆದರೆ (!fuzzy)
	ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 30);
    ಇಲ್ಲದಿದ್ದರೆ
	ga_init2(&ga, ಗಾತ್ರ(fuzmatch_str_T), 30);

    ಗಾಗಿ (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	ಆದರೆ (str == ಖಾಲಿ)	    // end of list
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (*str == NUL)	    // skip empty strings
	    ಮುಂದುವರಿ;

	ಆದರೆ (xp->xp_ನಮೂನೆ[0] != NUL)
	{
	    ಆದರೆ (!fuzzy)
		ಹೊಂದಿಕೆ = vim_regexec(regmatch, str, (colnr_T)0);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		score = fuzzy_ಹೊಂದಿಕೆ_str(str, pat);
		ಹೊಂದಿಕೆ = (score != FUZZY_SCORE_NONE);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹೊಂದಿಕೆ = TRUE;

	ಆದರೆ (!ಹೊಂದಿಕೆ)
	    ಮುಂದುವರಿ;

	ಆದರೆ (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	ಇಲ್ಲದಿದ್ದರೆ
	    str = vim_strsave(str);
	ಆದರೆ (str == ಖಾಲಿ)
	{
	    ಆದರೆ (!fuzzy)
	    {
		ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    fuzmatch_str_free(ga.ga_ದತ್ತಾಂಶ, ga.ga_len);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ((char_u **)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len] = str;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	ಆದರೆ (func == get_menu_ಹೆಸರುಗಳು)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    ಆದರೆ (*str == '\001')
		*str = '.';
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (ವಿಂಗಡಿಸು_ಆರಂಭ_Idx >= 0 && i >= ವಿಂಗಡಿಸು_ಆರಂಭ_Idx && ವಿಂಗಡಿಸು_ಆರಂಭ_ಹೊಂದಿಕೆ_Idx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    ವಿಂಗಡಿಸು_ಆರಂಭ_ಹೊಂದಿಕೆ_Idx = ga.ga_len;
	}

	++ga.ga_len;
    }

    ಆದರೆ (ga.ga_len == 0)
	ಹಿಂತಿರುಗಿ OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    ಆದರೆ (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	ವಿಂಗಡಿಸು_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    ಆದರೆ (xp->xp_context == EXPAND_ಅಭಿವ್ಯಕ್ತಿ
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    ಆದರೆ (ವಿಂಗಡಿಸು_matches && ವಿಂಗಡಿಸು_ಆರಂಭ_ಹೊಂದಿಕೆ_Idx != -1)
    {
	ಆದರೆ (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((ಶೂನ್ಯ *)ga.ga_ದತ್ತಾಂಶ, (size_t)ga.ga_len, ಗಾತ್ರ(char_u *),
							   ವಿಂಗಡಿಸು_func_compare);
	ಇಲ್ಲದಿದ್ದರೆ
	    ವಿಂಗಡಿಸು_ಅಕ್ಷರಸರಣಿಗಳು((char_u **)ga.ga_ದತ್ತಾಂಶ + ವಿಂಗಡಿಸು_ಆರಂಭ_ಹೊಂದಿಕೆ_Idx, ga.ga_len - ವಿಂಗಡಿಸು_ಆರಂಭ_ಹೊಂದಿಕೆ_Idx);
    }

    ಆದರೆ (!fuzzy)
    {
	*matches = ga.ga_ದತ್ತಾಂಶ;
	*ಅಂಕೆ_Matches = ga.ga_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (fuzzymatches_ಗಮ್ಯ_strmatches(ga.ga_ದತ್ತಾಂಶ, matches, ga.ga_len,
							funcsort) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	*ಅಂಕೆ_Matches = ga.ga_len;
    }

#ಆದರೆ defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
expand_shellcmd_onedir(
	char_u		*pathed_ನಮೂನೆ,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches,
	ಪೂರ್ಣಾಂಕ		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    ಆದರೆ (expand_wildcards(1, &pathed_ನಮೂನೆ, ಅಂಕೆ_Matches, matches, flags) != OK)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ga_grow(gap, *ಅಂಕೆ_Matches) == FAIL)
    {
	FreeWild(*ಅಂಕೆ_Matches, *matches);
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < *ಅಂಕೆ_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	ಆದರೆ (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len++] = name;
		hash_ಕೂಡಿಸು_ಅಂಶ(ht, hi, name, hash);
		name = ಖಾಲಿ;
	    }
	}
	vim_free(name);
    }
    vim_free(*matches);
}

/*
 * Complete a shell command.
 * Returns FAIL or OK;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches,	// return: number of matches
    ಪೂರ್ಣಾಂಕ		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*ಮಾರ್ಗ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    ಪೂರ್ಣಾಂಕ		flags = flagsarg;
    ಪೂರ್ಣಾಂಕ		did_curdir = FALSE;
    hashtab_T	ಸಿಕ್ಕಿತು_ht;

    buf = alloc(MAXPATHL);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    ಆದರೆ (pat == ಖಾಲಿ)
    {
	vim_free(buf);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    ಗಾಗಿ (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	ಆದರೆ (*s != '\\')
	    ಮುಂದುವರಿ;

	p = s + 1;
	ಆದರೆ (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    ಆದರೆ (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	ಮಾರ್ಗ = (char_u *)".";
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// For an absolute name we don't use $PATH.
	ಆದರೆ (!mch_is_ತುಂಬಿದ_Name(pat))
	    ಮಾರ್ಗ = vim_getenv((char_u *)"PATH", &mustfree);
	ಆದರೆ (ಮಾರ್ಗ == ಖಾಲಿ)
	    ಮಾರ್ಗ = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 10);
    hash_init(&ಸಿಕ್ಕಿತು_ht);
    ಗಾಗಿ (s = ಮಾರ್ಗ; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	ಆದರೆ (*s == NUL)
	{
	    ಆದರೆ (did_curdir)
		ನಿಲ್ಲಿಸು;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ಆದರೆ defined(MSWIN)
	    e = vim_strchr(s, ';');
#ಇಲ್ಲದಿದ್ದರೆ
	    e = vim_strchr(s, ':');
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (e == ಖಾಲಿ)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    ಆದರೆ (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? ಗಾತ್ರ(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	ಆದರೆ (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    ಆದರೆ (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		ಆದರೆ (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, ಅಂಕೆ_Matches, flags,
				    &ಸಿಕ್ಕಿತು_ht, &ga);
	}

	ಆದರೆ (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_ದತ್ತಾಂಶ;
    *ಅಂಕೆ_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    ಆದರೆ (mustfree)
	vim_free(ಮಾರ್ಗ);
    hash_clear(&ಸಿಕ್ಕಿತು_ht);
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ *
call_user_expand_func(
    ಶೂನ್ಯ	*(*user_expand_func)(char_u *, ಪೂರ್ಣಾಂಕ, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ಪೂರ್ಣಾಂಕ		keep = 0;
    typval_T	ವಾದಗಳು[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = ಖಾಲಿ;
    ಶೂನ್ಯ	*ret;

    ಆದರೆ (xp->xp_arg == ಖಾಲಿ || xp->xp_arg[0] == '\0' || xp->xp_ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (ccline->cmdbuff != ಖಾಲಿ)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	ಪೂರ್ಣಾಂಕ unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_ನಮೂನೆ,
						  xp->xp_ನಮೂನೆ_len, &unesc_len);
	ಆದರೆ (pat == ಖಾಲಿ)
	    pat = vim_strnsave(xp->xp_ನಮೂನೆ, xp->xp_ನಮೂನೆ_len);
    }

    ವಾದಗಳು[0].v_type = VAR_STRING;
    ವಾದಗಳು[0].vval.v_string = pat;
    ವಾದಗಳು[1].v_type = VAR_STRING;
    ವಾದಗಳು[1].vval.v_string = xp->xp_ಸಾಲು;
    ವಾದಗಳು[2].v_type = VAR_ಸಂಖ್ಯೆ;
    ವಾದಗಳು[2].vval.v_ಸಂಖ್ಯೆ = xp->xp_ಲಂಬಸಾಲು;
    ವಾದಗಳು[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, ವಾದಗಳು);

    current_sctx = save_current_sctx;
    ಆದರೆ (ccline->cmdbuff != ಖಾಲಿ)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    ಪೂರ್ಣಾಂಕ		keep;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		fuzzy;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ಖಾಲಿ;
    *ಅಂಕೆ_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    ಆದರೆ (retstr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (!fuzzy)
	ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 3);
    ಇಲ್ಲದಿದ್ದರೆ
	ga_init2(&ga, ಗಾತ್ರ(fuzmatch_str_T), 3);

    ಗಾಗಿ (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	ಆದರೆ (e == ಖಾಲಿ)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	ಆದರೆ (xp->xp_ನಮೂನೆ[0] != NUL)
	{
	    ಆದರೆ (!fuzzy)
		ಹೊಂದಿಕೆ = vim_regexec(regmatch, s, (colnr_T)0);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		score = fuzzy_ಹೊಂದಿಕೆ_str(s, pat);
		ಹೊಂದಿಕೆ = (score != FUZZY_SCORE_NONE);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹೊಂದಿಕೆ = TRUE;		// match everything

	*e = keep;

	ಆದರೆ (ಹೊಂದಿಕೆ)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    ಆದರೆ (p == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (!fuzzy)
		((char_u **)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len] = p;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	ಆದರೆ (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    ಆದರೆ (ga.ga_len == 0)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (!fuzzy)
    {
	*matches = ga.ga_ದತ್ತಾಂಶ;
	*ಅಂಕೆ_Matches = ga.ga_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (fuzzymatches_ಗಮ್ಯ_strmatches(ga.ga_ದತ್ತಾಂಶ, matches, ga.ga_len,
								FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	*ಅಂಕೆ_Matches = ga.ga_len;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_ಬಗೆ;
    garray_T	ga_menu;
    garray_T	ga_info;
    ಪೂರ್ಣಾಂಕ		have_extra = FALSE;
    ಪೂರ್ಣಾಂಕ		i;

    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 3);
    ga_init2(&ga_abbr, ಗಾತ್ರ(ಅಕ್ಷರ *), 3);
    ga_init2(&ga_ಬಗೆ, ಗಾತ್ರ(ಅಕ್ಷರ *), 3);
    ga_init2(&ga_menu, ಗಾತ್ರ(ಅಕ್ಷರ *), 3);
    ga_init2(&ga_info, ಗಾತ್ರ(ಅಕ್ಷರ *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_ವಸ್ತುಗಳು(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = ಖಾಲಿ;
	char_u	*abbr = ಖಾಲಿ;
	char_u	*ಬಗೆ = ಖಾಲಿ;
	char_u	*menu = ಖಾಲಿ;
	char_u	*info = ಖಾಲಿ;

	ಆದರೆ (tv->v_type == VAR_STRING)
	{
	    ಆದರೆ (tv->vval.v_string == ಖಾಲಿ)
		ಮುಂದುವರಿ;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_DICT && tv->vval.v_dict != ಖಾಲಿ)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*ಶಬ್ದ = dict_get_string(d, "word", FALSE);

	    ಆದರೆ (ಶಬ್ದ == ಖಾಲಿ)
		ಮುಂದುವರಿ;  // "word" is required
	    p = vim_strsave(ಶಬ್ದ);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    ಬಗೆ = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    ಆದರೆ (abbr != ಖಾಲಿ || ಬಗೆ != ಖಾಲಿ || menu != ಖಾಲಿ || info != ಖಾಲಿ)
		have_extra = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಮುಂದುವರಿ;  // Skip other types

	ಆದರೆ (p == ಖಾಲಿ
		|| ga_grow(&ga, 1) == FAIL
		|| ga_grow(&ga_abbr, 1) == FAIL
		|| ga_grow(&ga_ಬಗೆ, 1) == FAIL
		|| ga_grow(&ga_menu, 1) == FAIL
		|| ga_grow(&ga_info, 1) == FAIL)
	{
	    vim_free(p);
	    vim_free(abbr);
	    vim_free(ಬಗೆ);
	    vim_free(menu);
	    vim_free(info);
	    ನಿಲ್ಲಿಸು;
	}

	((char_u **)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_ದತ್ತಾಂಶ)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_ಬಗೆ.ga_ದತ್ತಾಂಶ)[ga_ಬಗೆ.ga_len++] = ಬಗೆ;
	((char_u **)ga_menu.ga_ದತ್ತಾಂಶ)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_ದತ್ತಾಂಶ)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_ದತ್ತಾಂಶ;
    *ಅಂಕೆ_Matches = ga.ga_len;
    ಆದರೆ (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_ದತ್ತಾಂಶ;
	xp->xp_files_ಬಗೆ = (char_u **)ga_ಬಗೆ.ga_ದತ್ತಾಂಶ;
	xp->xp_files_menu = (char_u **)ga_menu.ga_ದತ್ತಾಂಶ;
	xp->xp_files_info = (char_u **)ga_info.ga_ದತ್ತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// No extra info collected; free the placeholder NULL entries.
	ಗಾಗಿ (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_ದತ್ತಾಂಶ)[i]);
	vim_free(ga_abbr.ga_ದತ್ತಾಂಶ);
	ಗಾಗಿ (i = 0; i < ga_ಬಗೆ.ga_len; i++)
	    vim_free(((char_u **)ga_ಬಗೆ.ga_ದತ್ತಾಂಶ)[i]);
	vim_free(ga_ಬಗೆ.ga_ದತ್ತಾಂಶ);
	ಗಾಗಿ (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_ದತ್ತಾಂಶ)[i]);
	vim_free(ga_menu.ga_ದತ್ತಾಂಶ);
	ಗಾಗಿ (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_ದತ್ತಾಂಶ)[i]);
	vim_free(ga_info.ga_ದತ್ತಾಂಶ);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches)
{
    list_T      *retlist;

    *matches = ಖಾಲಿ;
    *ಅಂಕೆ_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    ಆದರೆ (retlist == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    expand_process_user_list(retlist, matches, ಅಂಕೆ_Matches, xp);
    list_unref(retlist);
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    ಶೂನ್ಯ
globpath(
    char_u	*ಮಾರ್ಗ,
    char_u	*file,
    garray_T	*ga,
    ಪೂರ್ಣಾಂಕ		expand_options,
    ಪೂರ್ಣಾಂಕ		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#ಆದರೆ defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#ಇಲ್ಲದಿದ್ದರೆ
# define TMP_PATHSEPSTR PATHSEPSTR
#ಆದರೆ_ಅಂತ್ಯ

    // Loop over all entries in {path}.
    ಆಗ (*ಮಾರ್ಗ != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&ಮಾರ್ಗ, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	ಆದರೆ (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    ಪೂರ್ಣಾಂಕ	    ಅಂಕೆ_p;
	    char_u  **p;

	    ಆದರೆ (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    ಆದರೆ (ExpandFromContext(&xpc, buf, &p, &ಅಂಕೆ_p,
			     WILD_SILENT|expand_options) != FAIL && ಅಂಕೆ_p > 0)
	    {
		ExpandEscape(&xpc, buf, ಅಂಕೆ_p, p, WILD_SILENT|expand_options);

		ಆದರೆ (ga_grow(ga, ಅಂಕೆ_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_p; ++i)
		    {
			((char_u **)ga->ga_ದತ್ತಾಂಶ)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    FreeWild(ಅಂಕೆ_p, p);
		    p = ಖಾಲಿ;
		}
		vim_free(p);
	    }
	}
    }

    vim_free(buf);
}
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು TMP_PATHSEPSTR

/*
 * Translate some keys pressed when 'wildmenu' is used.
 */
    ಪೂರ್ಣಾಂಕ
wildmenu_translate_ಕೀಲಿ(
	cmdline_info_T	*cclp,
	ಪೂರ್ಣಾಂಕ		ಕೀಲಿ,
	expand_T	*xp,
	ಪೂರ್ಣಾಂಕ		did_wild_list)
{
    ಪೂರ್ಣಾಂಕ c = ಕೀಲಿ;

    ಆದರೆ (cmdline_pum_ಸಕ್ರಿಯ())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	ಆಯ್ಕೆ (c)
	{
	    ಸ್ಥಿತಿ K_ಮೇಲೆ:	  c = K_ಎಡ; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ K_ಕೆಳಕ್ಕೆ:  c = K_ಬಲ; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ K_ಎಡ:  c = K_ಮೇಲೆ; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ K_ಬಲ: c = K_ಕೆಳಕ್ಕೆ; ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:	  ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (cmdline_pum_ಸಕ್ರಿಯ() || did_wild_list || wild_menu_showing)
    {
	ಆದರೆ (c == K_ಎಡ)
	    c = Ctrl_P;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == K_ಬಲ)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    ಆದರೆ (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_ಕೆಳಕ್ಕೆ;

    ಹಿಂತಿರುಗಿ c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cmdline_del(cmdline_info_T *cclp, ಪೂರ್ಣಾಂಕ from)
{
    mch_memmove(cclp->cmdbuff + from, cclp->cmdbuff + cclp->cmdpos,
	    (size_t)(cclp->cmdlen - cclp->cmdpos + 1));
    cclp->cmdlen -= cclp->cmdpos - from;
    cclp->cmdpos = from;
}

/*
 * Handle a key pressed when the wild menu for the menu names
 * (EXPAND_MENUNAMES) is displayed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
wildmenu_process_ಕೀಲಿ_menunames(cmdline_info_T *cclp, ಪೂರ್ಣಾಂಕ ಕೀಲಿ, expand_T *xp)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    ಆದರೆ (ಕೀಲಿ == K_ಕೆಳಕ್ಕೆ && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	ಕೀಲಿ = p_wc;
	ಕೀಲಿ_Typed = TRUE;  // in case the key was mapped
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ == K_ಮೇಲೆ)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು = FALSE;

	j = (ಪೂರ್ಣಾಂಕ)(xp->xp_ನಮೂನೆ - cclp->cmdbuff);
	i = 0;
	ಆಗ (--j > 0)
	{
	    // check for start of menu name
	    ಆದರೆ (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		ನಿಲ್ಲಿಸು;
	    }
	    // check for start of submenu name
	    ಆದರೆ (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		ಆದರೆ (ಸಿಕ್ಕಿತು)
		{
		    i = j + 1;
		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಿಕ್ಕಿತು = TRUE;
	    }
	}
	ಆದರೆ (i > 0)
	    cmdline_del(cclp, i);
	ಕೀಲಿ = p_wc;
	ಕೀಲಿ_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    ಹಿಂತಿರುಗಿ ಕೀಲಿ;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
wildmenu_process_ಕೀಲಿ_filenames(cmdline_info_T *cclp, ಪೂರ್ಣಾಂಕ ಕೀಲಿ, expand_T *xp)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    ಆದರೆ (ಕೀಲಿ == K_ಕೆಳಕ್ಕೆ
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	ಕೀಲಿ = p_wc;
	ಕೀಲಿ_Typed = TRUE;  // in case the key was mapped
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(xp->xp_ನಮೂನೆ, upseg + 1, 3) == 0 && ಕೀಲಿ == K_ಕೆಳಕ್ಕೆ)
    {
	// If in a direct ancestor, strip off one ../ to go down
	ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು = FALSE;

	j = cclp->cmdpos;
	i = (ಪೂರ್ಣಾಂಕ)(xp->xp_ನಮೂನೆ - cclp->cmdbuff);
	ಆಗ (--j > i)
	{
	    ಆದರೆ (has_mbyte)
		j -= (*mb_ತಲೆ_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    ಆದರೆ (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		ಸಿಕ್ಕಿತು = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (ಸಿಕ್ಕಿತು
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    ಕೀಲಿ = p_wc;
	    ಕೀಲಿ_Typed = TRUE;  // in case the key was mapped
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ == K_ಮೇಲೆ)
    {
	// go up a directory
	ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು = FALSE;

	j = cclp->cmdpos - 1;
	i = (ಪೂರ್ಣಾಂಕ)(xp->xp_ನಮೂನೆ - cclp->cmdbuff);
	ಆಗ (--j > i)
	{
	    ಆದರೆ (has_mbyte)
		j -= (*mb_ತಲೆ_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    ಆದರೆ (vim_ispathsep(cclp->cmdbuff[j])
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
	       )
	    {
		ಆದರೆ (ಸಿಕ್ಕಿತು)
		{
		    i = j + 1;
		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಿಕ್ಕಿತು = TRUE;
	    }
	}

	ಆದರೆ (!ಸಿಕ್ಕಿತು)
	    j = i;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	ಇಲ್ಲದಿದ್ದರೆ
	    j = 0;
	ಆದರೆ (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	ಕೀಲಿ = p_wc;
	ಕೀಲಿ_Typed = TRUE;
    }

    ಹಿಂತಿರುಗಿ ಕೀಲಿ;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    ಪೂರ್ಣಾಂಕ
wildmenu_process_ಕೀಲಿ(cmdline_info_T *cclp, ಪೂರ್ಣಾಂಕ ಕೀಲಿ, expand_T *xp)
{
    ಆದರೆ (xp->xp_context == EXPAND_MENUNAMES)
	ಹಿಂತಿರುಗಿ wildmenu_process_ಕೀಲಿ_menunames(cclp, ಕೀಲಿ, xp);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	ಹಿಂತಿರುಗಿ wildmenu_process_ಕೀಲಿ_filenames(cclp, ಕೀಲಿ, xp);

    ಹಿಂತಿರುಗಿ ಕೀಲಿ;
}

/*
 * Free expanded names when finished walking through the matches
 */
    ಶೂನ್ಯ
wildmenu_cleanup(cmdline_info_T *cclp ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ skt = ಕೀಲಿ_Typed;

    ಆದರೆ (!p_wmnu || wild_menu_showing == 0)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ save_RedrawingDisabled = RedrawingDisabled;
    ಆದರೆ (cclp->ಒಳಹರಿವು_fn)
	RedrawingDisabled = 0;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA)
    // Clear highlighting applied during wildmenu activity
    ಗಣ_no_hlsearch(TRUE);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_ಅಡ್ಡಸಾಲು--;
	redrawcmd();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	ಕೊನೆಯ_ಸ್ಥಿತಿ(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	win_redraw_ಕೊನೆಯ_ಸ್ಥಿತಿ(topframe);
	redraw_statuslines();
    }
    ಕೀಲಿ_Typed = skt;
    wild_menu_showing = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (cclp->ಒಳಹರಿವು_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    ಶೂನ್ಯ
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    ಪೂರ್ಣಾಂಕ		filtered = FALSE;
    ಪೂರ್ಣಾಂಕ		options = WILD_SILENT | WILD_USE_NL | WILD_ಕೂಡಿಸು_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    pat = tv_get_string(&argvars[0]);
    ಆದರೆ (check_for_string_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;
    type = tv_get_string(&argvars[1]);

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], ಖಾಲಿ);

    ಆದರೆ (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    ಆದರೆ (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    ಆದರೆ (STRCMP(type, "cmdline") == 0)
    {
	ಪೂರ್ಣಾಂಕ cmdline_len = (ಪೂರ್ಣಾಂಕ)STRLEN(pat);
	ಗಣ_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_ನಮೂನೆ_len = (ಪೂರ್ಣಾಂಕ)STRLEN(xpc.xp_ನಮೂನೆ);
	xpc.xp_ಲಂಬಸಾಲು = cmdline_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	*ನಮೂನೆ_ಆರಂಭ;

	xpc.xp_ನಮೂನೆ = ನಮೂನೆ_ಆರಂಭ = pat;
	xpc.xp_ನಮೂನೆ_len = (ಪೂರ್ಣಾಂಕ)STRLEN(xpc.xp_ನಮೂನೆ);
	xpc.xp_ಸಾಲು = pat;

	xpc.xp_context = cmdcomplete_str_ಗಮ್ಯ_type(type);
	ಆಯ್ಕೆ (xpc.xp_context)
	{
	ಸ್ಥಿತಿ EXPAND_NOTHING:
	    semsg(_(e_invalid_ವಾದ_str), type);
	    ಹಿಂತಿರುಗಿ;

	ಸ್ಥಿತಿ EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    ಆದರೆ (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_ವಾದ_str), type);
		ಹಿಂತಿರುಗಿ;
	    }

	    xpc.xp_arg = type + 7;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    ಆದರೆ (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_ವಾದ_str), type);
		ಹಿಂತಿರುಗಿ;
	    }

	    xpc.xp_arg = type + 11;
	    ನಿಲ್ಲಿಸು;

# ಆದರೆ defined(FEAT_MENU)
	ಸ್ಥಿತಿ EXPAND_MENUS:
	    ಗಣ_context_ಒಳ_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_ನಮೂನೆ, FALSE);
	    xpc.xp_ನಮೂನೆ_len -= (ಪೂರ್ಣಾಂಕ)(xpc.xp_ನಮೂನೆ - ನಮೂನೆ_ಆರಂಭ);
	    ನಿಲ್ಲಿಸು;
# endif

# ifdef FEAT_CSCOPE
	ಸ್ಥಿತಿ EXPAND_CSCOPE:
	    ಗಣ_context_ಒಳ_cscope_cmd(&xpc, xpc.xp_ನಮೂನೆ, CMD_cscope);
	    xpc.xp_ನಮೂನೆ_len -= (ಪೂರ್ಣಾಂಕ)(xpc.xp_ನಮೂನೆ - ನಮೂನೆ_ಆರಂಭ);
	    ನಿಲ್ಲಿಸು;
# endif

# ifdef FEAT_SIGNS
	ಸ್ಥಿತಿ EXPAND_SIGN:
	    ಗಣ_context_ಒಳ_sign_cmd(&xpc, xpc.xp_ನಮೂನೆ);
	    xpc.xp_ನಮೂನೆ_len -= (ಪೂರ್ಣಾಂಕ)(xpc.xp_ನಮೂನೆ - ನಮೂನೆ_ಆರಂಭ);
	    ನಿಲ್ಲಿಸು;
# endif

	ಸ್ಥಿತಿ EXPAND_RUNTIME:
	    ಗಣ_context_ಒಳ_runtime_cmd(&xpc, xpc.xp_ನಮೂನೆ);
	    xpc.xp_ನಮೂನೆ_len -= (ಪೂರ್ಣಾಂಕ)(xpc.xp_ನಮೂನೆ - ನಮೂನೆ_ಆರಂಭ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ EXPAND_SHELLCMDLINE:
	{
	    ಪೂರ್ಣಾಂಕ context = EXPAND_SHELLCMDLINE;
	    ಗಣ_context_for_wildcard_arg(ಖಾಲಿ, xpc.xp_ನಮೂನೆ, FALSE, &xpc,
					 &context);
	    xpc.xp_ನಮೂನೆ_len -= (ಪೂರ್ಣಾಂಕ)(xpc.xp_ನಮೂನೆ - ನಮೂನೆ_ಆರಂಭ);
	    ನಿಲ್ಲಿಸು;
	}

	ಸ್ಥಿತಿ EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ಕೂಡಿಸು;
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
	}
    }

    {
	char_u	*pat_src = xpc.xp_ನಮೂನೆ;
	ಪೂರ್ಣಾಂಕ	pat_len = xpc.xp_ನಮೂನೆ_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	ಆದರೆ (unesc != ಖಾಲಿ)
	    pat_src = unesc;

	ಆದರೆ (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	ಇಲ್ಲದಿದ್ದರೆ
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    ಆದರೆ (rettv_list_alloc(rettv) == OK && pat != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	i;

	ExpandOne(&xpc, pat, ಖಾಲಿ, options, WILD_ALL_KEEP);

	ಗಾಗಿ (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    ಶೂನ್ಯ
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    ಪೂರ್ಣಾಂಕ		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (ಪೂರ್ಣಾಂಕ)STRLEN(pat);
    ಗಣ_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_ಗಮ್ಯ_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    ಶೂನ್ಯ
f_cmdcomplete_info(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    ಪೂರ್ಣಾಂಕ		    idx;
    ಪೂರ್ಣಾಂಕ		    ret = OK;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL || ccline == ಖಾಲಿ
	    || ccline->xpc == ಖಾಲಿ || ccline->xpc->xp_files == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    retdict = rettv->vval.v_dict;
    ret = dict_ಕೂಡಿಸು_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (ಪೂರ್ಣಾಂಕ)cmdline_orig.ಉದ್ದಳತೆ);
    ಆದರೆ (ret == OK)
	ret = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "pum_visible", pum_visible());
    ಆದರೆ (ret == OK)
	ret = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "selected", ccline->xpc->xp_selected);
    ಆದರೆ (ret == OK)
    {
	li = list_alloc();
	ಆದರೆ (li == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ret = dict_ಕೂಡಿಸು_list(retdict, "matches", li);
	ಆದರೆ (ret == FAIL)
	    list_unref(li);
	ಗಾಗಿ (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
	    list_append_string(li, ccline->xpc->xp_files[idx], -1);
    }
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

/*
 * Copy a substring from the current buffer (curbuf), spanning from the given
 * 'start' position to the word boundary after 'end' position.
 * The copied string is stored in '*match', and the actual end position of the
 * matched text is returned in '*match_end'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
copy_substring_from_pos(pos_T *ಆರಂಭ, pos_T *ಕೊನೆ, char_u **ಹೊಂದಿಕೆ,
	pos_T *ಹೊಂದಿಕೆ_ಕೊನೆ)
{
    char_u	*ಶಬ್ದ_ಕೊನೆ;
    char_u	*ಸಾಲು, *ಆರಂಭ_ಸಾಲು, *ಕೊನೆ_ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ಖಾಲಿ;

    ಆದರೆ (ಆರಂಭ->lnum > ಕೊನೆ->lnum
	    || (ಆರಂಭ->lnum == ಕೊನೆ->lnum && ಆರಂಭ->ಲಂಬಸಾಲು >= ಕೊನೆ->ಲಂಬಸಾಲು))
	ಹಿಂತಿರುಗಿ FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    ಆರಂಭ_ಸಾಲು = ml_get(ಆರಂಭ->lnum);
    char_u  *ಆರಂಭ_ptr = ಆರಂಭ_ಸಾಲು + ಆರಂಭ->ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ	    is_single_ಸಾಲು = ಆರಂಭ->lnum == ಕೊನೆ->lnum;

    segment_len = is_single_ಸಾಲು ? (ಕೊನೆ->ಲಂಬಸಾಲು - ಆರಂಭ->ಲಂಬಸಾಲು)
			: (ಪೂರ್ಣಾಂಕ)(ml_get_len(ಆರಂಭ->lnum) - ಆರಂಭ->ಲಂಬಸಾಲು);
    ಆದರೆ (ga_grow(&ga, segment_len + 2) != OK)
	ಹೋಗು fail;

    ga_concat_len(&ga, ಆರಂಭ_ptr, segment_len);
    ಆದರೆ (!is_single_ಸಾಲು)
    {
	ಆದರೆ (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	ಇಲ್ಲದಿದ್ದರೆ
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    ಆದರೆ (!is_single_ಸಾಲು)
    {
	ಗಾಗಿ (lnum = ಆರಂಭ->lnum + 1; lnum < ಕೊನೆ->lnum; lnum++)
	{
	    ಪೂರ್ಣಾಂಕ  linelen;

	    ಸಾಲು = ml_get(lnum);
	    linelen = (ಪೂರ್ಣಾಂಕ)ml_get_len(lnum);
	    ಆದರೆ (ga_grow(&ga, linelen + 2) != OK)
		ಹೋಗು fail;
	    ga_concat_len(&ga, ಸಾಲು, linelen);
	    ಆದರೆ (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    ಇಲ್ಲದಿದ್ದರೆ
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    ಕೊನೆ_ಸಾಲು = ml_get(ಕೊನೆ->lnum);
    ಶಬ್ದ_ಕೊನೆ = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(ಕೊನೆ_ಸಾಲು + ಕೊನೆ->ಲಂಬಸಾಲು);
    segment_len = (ಪೂರ್ಣಾಂಕ)(ಶಬ್ದ_ಕೊನೆ - ಕೊನೆ_ಸಾಲು);
    ಆದರೆ (ga_grow(&ga, segment_len) != OK)
	ಹೋಗು fail;
    ga_concat_len(&ga, ಕೊನೆ_ಸಾಲು + (is_single_ಸಾಲು ? ಕೊನೆ->ಲಂಬಸಾಲು : 0),
	    segment_len - (is_single_ಸಾಲು ? ಕೊನೆ->ಲಂಬಸಾಲು : 0));

    // Null-terminate
    ಆದರೆ (ga_grow(&ga, 1) != OK)
	ಹೋಗು fail;
    ga_append(&ga, NUL);

    *ಹೊಂದಿಕೆ = (char_u *)ga.ga_ದತ್ತಾಂಶ;
    ಹೊಂದಿಕೆ_ಕೊನೆ->lnum = ಕೊನೆ->lnum;
    ಹೊಂದಿಕೆ_ಕೊನೆ->ಲಂಬಸಾಲು = segment_len;

    ಹಿಂತಿರುಗಿ OK;

fail:
    ga_clear(&ga);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_regex_ಹೊಂದಿಕೆ(char_u *pat, char_u *str)
{
    ಆದರೆ (STRCMP(pat, str) == 0)
	ಹಿಂತಿರುಗಿ TRUE;

    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;

    ++emsg_off;
    ++ಸಂದೇಶ_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --ಸಂದೇಶ_silent;

    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    regmatch.rm_ic = p_ic;
    ಆದರೆ (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++ಸಂದೇಶ_silent;
    ಫಲಿತಾಂಶ = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --ಸಂದೇಶ_silent;

    vim_regfree(regmatch.regprog);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    ಸ್ಥಿರ char_u *
concat_ನಮೂನೆ_with_ಬಫರ್_ಹೊಂದಿಕೆ(
	char_u *pat,
	ಪೂರ್ಣಾಂಕ pat_len,
	pos_T *ಕೊನೆ_ಹೊಂದಿಕೆ_pos,
	ಪೂರ್ಣಾಂಕ lowercase ಬಳಸದ)
{
    char_u  *ಸಾಲು = ml_get(ಕೊನೆ_ಹೊಂದಿಕೆ_pos->lnum);
    char_u  *ಶಬ್ದ_ಕೊನೆ = ಕಂಡುಹಿಡಿ_ಶಬ್ದ_ಕೊನೆ(ಸಾಲು + ಕೊನೆ_ಹೊಂದಿಕೆ_pos->ಲಂಬಸಾಲು);
    ಪೂರ್ಣಾಂಕ	    ಹೊಂದಿಕೆ_len = (ಪೂರ್ಣಾಂಕ)(ಶಬ್ದ_ಕೊನೆ - (ಸಾಲು + ಕೊನೆ_ಹೊಂದಿಕೆ_pos->ಲಂಬಸಾಲು));
    char_u  *ಹೊಂದಿಕೆ = alloc(ಹೊಂದಿಕೆ_len + pat_len + 1);  // +1 for NUL

    ಆದರೆ (ಹೊಂದಿಕೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    mch_memmove(ಹೊಂದಿಕೆ, pat, pat_len);
    ಆದರೆ (ಹೊಂದಿಕೆ_len > 0)
    {
#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_SPELL)
	ಆದರೆ (lowercase)
	{
	    char_u  *mword = vim_strnsave(ಸಾಲು + ಕೊನೆ_ಹೊಂದಿಕೆ_pos->ಲಂಬಸಾಲು,
		    ಹೊಂದಿಕೆ_len);
	    ಆದರೆ (mword == ಖಾಲಿ)
		ಹೋಗು cleanup;
	    char_u  *ಅಧೋ = strlow_save(mword);
	    vim_free(mword);
	    ಆದರೆ (ಅಧೋ == ಖಾಲಿ)
		ಹೋಗು cleanup;
	    mch_memmove(ಹೊಂದಿಕೆ + pat_len, ಅಧೋ, ಹೊಂದಿಕೆ_len);
	    vim_free(ಅಧೋ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    mch_memmove(ಹೊಂದಿಕೆ + pat_len, ಸಾಲು + ಕೊನೆ_ಹೊಂದಿಕೆ_pos->ಲಂಬಸಾಲು, ಹೊಂದಿಕೆ_len);
    }
    ಹೊಂದಿಕೆ[pat_len + ಹೊಂದಿಕೆ_len] = NUL;
    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ;

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(ಹೊಂದಿಕೆ);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
expand_ನಮೂನೆ_ಒಳ_buf(
    char_u	*pat,		    // pattern to match
    ಪೂರ್ಣಾಂಕ		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches)	    // return: number of matches
{
    pos_T	cur_ಹೊಂದಿಕೆ_pos, prev_ಹೊಂದಿಕೆ_pos, ಕೊನೆ_ಹೊಂದಿಕೆ_pos, ಶಬ್ದ_ಕೊನೆ_pos;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		looped_around = FALSE;
    ಪೂರ್ಣಾಂಕ		pat_len;
    ಪೂರ್ಣಾಂಕ		has_range = FALSE;
    ಪೂರ್ಣಾಂಕ		compl_started = FALSE;
    ಪೂರ್ಣಾಂಕ		ಹುಡುಕು_flags;
    char_u	*ಹೊಂದಿಕೆ, *ತುಂಬಿದ_ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    has_range = ಹುಡುಕು_ಮೊದಲ_ಸಾಲು != 0;
#ಆದರೆ_ಅಂತ್ಯ

    *matches = ಖಾಲಿ;
    *ಅಂಕೆ_Matches = 0;

    ಆದರೆ (pat == ಖಾಲಿ || *pat == NUL)
	ಹಿಂತಿರುಗಿ FAIL;

    pat_len = (ಪೂರ್ಣಾಂಕ)STRLEN(pat);
    CLEAR_ಕ್ಷೇತ್ರ(cur_ಹೊಂದಿಕೆ_pos);
    CLEAR_ಕ್ಷೇತ್ರ(prev_ಹೊಂದಿಕೆ_pos);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    ಆದರೆ (has_range)
	cur_ಹೊಂದಿಕೆ_pos.lnum = ಹುಡುಕು_ಮೊದಲ_ಸಾಲು;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	cur_ಹೊಂದಿಕೆ_pos = pre_incsearch_pos;

    ಹುಡುಕು_flags = ಹುಡುಕು_OPT | ಹುಡುಕು_NOOF | ಹುಡುಕು_PEEK | ಹುಡುಕು_NFMSG
	| (has_range ? ಹುಡುಕು_ಆರಂಭ : 0);

    ga_init2(&ga, ಗಾತ್ರ(char_u *), 10); // Use growable array of char_u*

    ಗಾಗಿ (;;)
    {
	++emsg_off;
	++ಸಂದೇಶ_silent;
	ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ = searchit(ಖಾಲಿ, curbuf, &cur_ಹೊಂದಿಕೆ_pos,
		&ಕೊನೆ_ಹೊಂದಿಕೆ_pos, dir, pat, pat_len, 1L,
		ಹುಡುಕು_flags, RE_ಕೊನೆಯ, ಖಾಲಿ);
	--ಸಂದೇಶ_silent;
	--emsg_off;

	ಆದರೆ (ಸಿಕ್ಕಿತು_new_ಹೊಂದಿಕೆ == FAIL)
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	// If in range mode, check if match is within the range
	ಆದರೆ (has_range && (cur_ಹೊಂದಿಕೆ_pos.lnum < ಹುಡುಕು_ಮೊದಲ_ಸಾಲು
		    || cur_ಹೊಂದಿಕೆ_pos.lnum > ಹುಡುಕು_ಕೊನೆಯ_ಸಾಲು))
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    ಆದರೆ ((dir == FORWARD && LTOREQ_POS(cur_ಹೊಂದಿಕೆ_pos, prev_ಹೊಂದಿಕೆ_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_ಹೊಂದಿಕೆ_pos, cur_ಹೊಂದಿಕೆ_pos)))
	    {
		ಆದರೆ (looped_around)
		    ನಿಲ್ಲಿಸು;
		ಇಲ್ಲದಿದ್ದರೆ
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_ಹೊಂದಿಕೆ_pos = cur_ಹೊಂದಿಕೆ_pos;

	// Abort if user typed a character or interrupted
	ಆದರೆ (char_avail() || got_int)
	{
	    ಆದರೆ (got_int)
	    {
		(ಶೂನ್ಯ)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    ಹೋಗು cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	ಆದರೆ (ಕೊನೆ_ಹೊಂದಿಕೆ_pos.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    cur_ಹೊಂದಿಕೆ_pos.lnum = 1;
	    cur_ಹೊಂದಿಕೆ_pos.ಲಂಬಸಾಲು = 0;
	    cur_ಹೊಂದಿಕೆ_pos.coladd = 0;
	    ಮುಂದುವರಿ;
	}

	// Extract the matching text prepended to completed word
	ಆದರೆ (copy_substring_from_pos(&cur_ಹೊಂದಿಕೆ_pos, &ಕೊನೆ_ಹೊಂದಿಕೆ_pos, &ತುಂಬಿದ_ಹೊಂದಿಕೆ,
		    &ಶಬ್ದ_ಕೊನೆ_pos) == FAIL)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (exacttext)
	    ಹೊಂದಿಕೆ = ತುಂಬಿದ_ಹೊಂದಿಕೆ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Construct a new match from completed word appended to pattern itself
	    ಹೊಂದಿಕೆ = concat_ನಮೂನೆ_with_ಬಫರ್_ಹೊಂದಿಕೆ(pat, pat_len, &ಕೊನೆ_ಹೊಂದಿಕೆ_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    ಆದರೆ (ಹೊಂದಿಕೆ == ಖಾಲಿ || !is_regex_ಹೊಂದಿಕೆ(ಹೊಂದಿಕೆ, ತುಂಬಿದ_ಹೊಂದಿಕೆ))
	    {
		vim_free(ಹೊಂದಿಕೆ);
		ಹೊಂದಿಕೆ = concat_ನಮೂನೆ_with_ಬಫರ್_ಹೊಂದಿಕೆ(pat, pat_len,
			&ಕೊನೆ_ಹೊಂದಿಕೆ_pos, TRUE);
		ಆದರೆ (ಹೊಂದಿಕೆ == ಖಾಲಿ || !is_regex_ಹೊಂದಿಕೆ(ಹೊಂದಿಕೆ, ತುಂಬಿದ_ಹೊಂದಿಕೆ))
		{
		    vim_free(ಹೊಂದಿಕೆ);
		    vim_free(ತುಂಬಿದ_ಹೊಂದಿಕೆ);
		    ಮುಂದುವರಿ;
		}
	    }
	    vim_free(ತುಂಬಿದ_ಹೊಂದಿಕೆ);
	}

	// Include this match if it is not a duplicate
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ga.ga_len; ++i)
	{
	    ಆದರೆ (STRCMP(ಹೊಂದಿಕೆ, ((char_u **)ga.ga_ದತ್ತಾಂಶ)[i]) == 0)
	    {
		VIM_CLEAR(ಹೊಂದಿಕೆ);
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (ಹೊಂದಿಕೆ != ಖಾಲಿ)
	{
	    ಆದರೆ (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(ಹೊಂದಿಕೆ);
		ಹೋಗು cleanup;
	    }
	    ((char_u **)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len++] = ಹೊಂದಿಕೆ;
	    ಆದರೆ (ga.ga_len > ಟ್ಯಾಗ್_MANY)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (has_range)
	    cur_ಹೊಂದಿಕೆ_pos = ಶಬ್ದ_ಕೊನೆ_pos;
    }

    *matches = (char_u **)ga.ga_ದತ್ತಾಂಶ;
    *ಅಂಕೆ_Matches = ga.ga_len;
    ಹಿಂತಿರುಗಿ OK;

cleanup:
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga);
    ಹಿಂತಿರುಗಿ FAIL;
}
