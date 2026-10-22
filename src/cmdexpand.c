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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	cmd_showtail;	// Only show path tail in lists ?
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	may_expand_pattern = FALSE;
ಸ್ಥಿರ pos_T	pre_incsearch_pos; // Cursor position when incsearch started

ಸ್ಥಿರ ಶೂನ್ಯ	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, ಪೂರ್ಣಾಙ್ಕ usefilter, expand_T *xp, ಪೂರ್ಣಾಙ್ಕ *complp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	ExpandFromContext(expand_T *xp, char_u *, char_u ***, ಪೂರ್ಣಾಙ್ಕ *, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ char_u	*showmatches_gettail(char_u *s);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	expand_showtail(expand_T *xp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	expand_shellcmd(char_u *filepat, char_u ***matches, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, ಪೂರ್ಣಾಙ್ಕ flagsarg);
#ಯದಿ defined(FEAT_EVAL)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	ExpandUserList(expand_T *xp, char_u ***matches, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches);
ಸ್ಥಿರ char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
ಸ್ಥಿರ char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, ಪೂರ್ಣಾಙ್ಕ srclen, ಪೂರ್ಣಾಙ್ಕ *new_lenp);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	expand_pattern_in_buf(char_u *pat, ಪೂರ್ಣಾಙ್ಕ dir, char_u ***matches, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ಸ್ಥಿರ pumitem_T *compl_match_array = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compl_match_arraysize;
// First column in cmdline of the matched item for completion.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compl_startcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compl_selected;
// cmdline before expansion
ಸ್ಥಿರ string_T cmdline_orig = {ರಿಕ್ತ, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    ವಿಕಲ್ಪ (xp->xp_context)
    {
    ಪಕ್ಷ EXPAND_BOOL_SETTINGS:
    ಪಕ್ಷ EXPAND_COLORS:
    ಪಕ್ಷ EXPAND_COMPILER:
    ಪಕ್ಷ EXPAND_DIRECTORIES:
    ಪಕ್ಷ EXPAND_DIRS_IN_CDPATH:
    ಪಕ್ಷ EXPAND_FILES:
    ಪಕ್ಷ EXPAND_FILES_IN_PATH:
    ಪಕ್ಷ EXPAND_FILETYPE:
    ಪಕ್ಷ EXPAND_FILETYPECMD:
    ಪಕ್ಷ EXPAND_HELP:
    ಪಕ್ಷ EXPAND_KEYMAP:
    ಪಕ್ಷ EXPAND_OLD_SETTING:
    ಪಕ್ಷ EXPAND_STRING_SETTING:
    ಪಕ್ಷ EXPAND_SETTING_SUBTRACT:
    ಪಕ್ಷ EXPAND_OWNSYNTAX:
    ಪಕ್ಷ EXPAND_PACKADD:
    ಪಕ್ಷ EXPAND_RUNTIME:
    ಪಕ್ಷ EXPAND_SHELLCMD:
    ಪಕ್ಷ EXPAND_SHELLCMDLINE:
    ಪಕ್ಷ EXPAND_TAGS:
    ಪಕ್ಷ EXPAND_TAGS_LISTFILES:
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಸ್ವಭಾವ:
	ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ vim_strchr(p_wop, WOP_FUZZY) != ರಿಕ್ತ;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    ಪೂರ್ಣಾಙ್ಕ
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    ಪ್ರತ್ಯಾಗಮ vim_strchr(p_wop, WOP_FUZZY) != ರಿಕ್ತ && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sort_func_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    ಯದಿ (*p1 != '<' && *p2 == '<') ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (*p1 == '<' && *p2 != '<') ಪ್ರತ್ಯಾಗಮ 1;
    ಪ್ರತ್ಯಾಗಮ STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wildescape(
    expand_T	*xp,
    char_u	*str,
    ಪೂರ್ಣಾಙ್ಕ		numfiles,
    char_u	**files)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    ಯದಿ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    ಯದಿ (xp->xp_backslash & XP_BS_THREE)
	    {
		ವರ್ಣ *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		ಯದಿ (p != ರಿಕ್ತ)
		{
		    vim_free(files[i]);
		    files[i] = p;
#ಯದಿ defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    ಯದಿ (p != ರಿಕ್ತ)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (xp->xp_backslash & XP_BS_COMMA)
	    {
		ಯದಿ (vim_strchr(files[i], ',') != ರಿಕ್ತ)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    ಯದಿ (p != ರಿಕ್ತ)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ifdef BACKSLASH_IN_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#ಅನ್ಯಥಾ
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#endif
	    ಯದಿ (p != ರಿಕ್ತ)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    ಯದಿ (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	ಯದಿ (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    ಅನ್ಯಥಾ ಯದಿ (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    ಯದಿ (p != ರಿಕ್ತ)
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
    ಪೂರ್ಣಾಙ್ಕ		numfiles,
    char_u	**files,
    ಪೂರ್ಣಾಙ್ಕ		options)
{
    // May change home directory back to "~"
    ಯದಿ (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    ಯದಿ (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    ಪೂರ್ಣಾಙ್ಕ
nextwild(
    expand_T	*xp,
    ಪೂರ್ಣಾಙ್ಕ		type,
    ಪೂರ್ಣಾಙ್ಕ		options,	// extra options for ExpandOne()
    ಪೂರ್ಣಾಙ್ಕ		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    ಪೂರ್ಣಾಙ್ಕ		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    ಯದಿ (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	ಯದಿ (ccline->ಒಳಹರಿವು_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	ಅನ್ಯಥಾ
#endif
	{
	    may_expand_pattern = options & WILD_MAY_EXPAND_PATTERN;
	    set_expand_context(xp);
	    may_expand_pattern = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    ಯದಿ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ಪ್ರತ್ಯಾಗಮ OK;  // Something illegal on command line
    }
    ಯದಿ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    i = (ಪೂರ್ಣಾಙ್ಕ)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    ಯದಿ (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    ಯದಿ (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	ಸಂದೇಶ_puts("...");	    // show that we are busy
	out_flush();
    }

    ಯದಿ (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, ರಿಕ್ತ, ರಿಕ್ತ, 0, type);
    }
    ಅನ್ಯಥಾ
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	ಪೂರ್ಣಾಙ್ಕ	pat_len = xp->xp_pattern_len;
#ಯದಿ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	ಯದಿ (unesc != ರಿಕ್ತ)
	    pat_src = unesc;
#endif

	ಯದಿ (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	ಅನ್ಯಥಾ
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#ಯದಿ defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	ಯದಿ (tmp == ರಿಕ್ತ)
	    p = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    ಯದಿ (escape)
		use_options |= WILD_ESCAPE;
	    ಯದಿ (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    ಯದಿ (p != ರಿಕ್ತ && type == WILD_LONGEST)
	    {
		ಪೂರ್ಣಾಙ್ಕ	j;

		ಪ್ರತಿ (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    ಯದಿ (c == '*' || c == '?')
			ಭಙ್ಗ;
		}
		ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    ಯದಿ (!wild_navigate && ccline->cmdbuff != ರಿಕ್ತ)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	ಯದಿ (cmdline_orig.string == ರಿಕ್ತ)
	    cmdline_orig.length = 0;
	ಅನ್ಯಥಾ
	    cmdline_orig.length = ccline->cmdlen;
    }

    ಯದಿ (p != ರಿಕ್ತ && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	ಪೂರ್ಣಾಙ್ಕ	difflen;
	ಪೂರ್ಣಾಙ್ಕ	v = OK;

	difflen = (ಪೂರ್ಣಾಙ್ಕ)plen - xp->xp_pattern_len;
	ಯದಿ (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	ಯದಿ (v == OK)
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
    ಯದಿ (xp->xp_context == EXPAND_MAPPINGS && p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (xp->xp_numfiles <= 0 && p == ರಿಕ್ತ)
	beep_flush();
    ಅನ್ಯಥಾ ಯದಿ (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(ಶೂನ್ಯ)ExpandOne(xp, ರಿಕ್ತ, ರಿಕ್ತ, 0, WILD_FREE);

    vim_free(p);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_Matches,
	ಪೂರ್ಣಾಙ್ಕ		showtail)
{
    ಪೂರ್ಣಾಙ್ಕ	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, ಅಂಕೆ_Matches);
    ಯದಿ (compl_match_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = ಅಂಕೆ_Matches;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_Matches; i++)
    {
	compl_match_array[i].pum_text = (xp->xp_files_abbr != ರಿಕ್ತ
				    && xp->xp_files_abbr[i] != ರಿಕ್ತ)
					? xp->xp_files_abbr[i]
					: SHOW_MATCH(i);
	compl_match_array[i].pum_info = xp->xp_files_info != ರಿಕ್ತ
					    ? xp->xp_files_info[i] : ರಿಕ್ತ;
	compl_match_array[i].pum_extra = xp->xp_files_menu != ರಿಕ್ತ
					    ? xp->xp_files_menu[i] : ರಿಕ್ತ;
	compl_match_array[i].pum_kind = xp->xp_files_kind != ರಿಕ್ತ
					    ? xp->xp_files_kind[i] : ರಿಕ್ತ;
	compl_match_array[i].pum_user_abbr_hlattr = -1;
	compl_match_array[i].pum_user_kind_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == ರಿಕ್ತ ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    prefix_len = vim_strsize(xp->xp_pattern);
    ಯದಿ (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    ಪ್ರತ್ಯಾಗಮ EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    ಶೂನ್ಯ
cmdline_pum_display(ಶೂನ್ಯ)
{
    ಯದಿ (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    ಪೂರ್ಣಾಙ್ಕ
cmdline_pum_active(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ pum_visible() && compl_match_array != ರಿಕ್ತ;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    ಶೂನ್ಯ
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, ಪೂರ್ಣಾಙ್ಕ defer_redraw)
{
    ಪೂರ್ಣಾಙ್ಕ	save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
#ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ	save_RedrawingDisabled = RedrawingDisabled;
    ಯದಿ (cclp->ಒಳಹರಿವು_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    ಯದಿ (!defer_redraw)
    {
	ಪೂರ್ಣಾಙ್ಕ save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    ಅನ್ಯಥಾ
	pum_call_update_screen();
    redrawcmd();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    ಕೀಲಿ_Typed = save_ಕೀಲಿ_Typed;
#ifdef FEAT_EVAL
    ಯದಿ (cclp->ಒಳಹರಿವು_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
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
    ಪೂರ್ಣಾಙ್ಕ
cmdline_compl_startcol(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(ಶೂನ್ಯ)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ಪ್ರತ್ಯಾಗಮ xp == ರಿಕ್ತ ? ರಿಕ್ತ : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
cmdline_compl_is_fuzzy(ಶೂನ್ಯ)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ಪ್ರತ್ಯಾಗಮ xp != ರಿಕ್ತ && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
skip_status_match_char(expand_T *xp, char_u *s)
{
    ಯದಿ ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	ಯದಿ (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    ಪ್ರತ್ಯಾಗಮ 2;
#endif
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
status_match_len(expand_T *xp, char_u *s)
{
    ಪೂರ್ಣಾಙ್ಕ	len = 0;

#ifdef FEAT_MENU
    ಪೂರ್ಣಾಙ್ಕ emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    ಯದಿ (emenu && menu_is_separator(s))
	ಪ್ರತ್ಯಾಗಮ 1;
#endif

    ಯಾವತ್ (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    ಪ್ರತ್ಯಾಗಮ len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_redr_status_matches(
    expand_T	*xp,
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_matches,
    char_u	**matches,	// list of matches
    ಪೂರ್ಣಾಙ್ಕ		match,
    ಪೂರ್ಣಾಙ್ಕ		showtail)
{
    ಪೂರ್ಣಾಙ್ಕ		row;
    char_u	*buf;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		clen;		// length in screen cells
    ಪೂರ್ಣಾಙ್ಕ		fillchar;
    ಪೂರ್ಣಾಙ್ಕ		attr;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		highlight = TRUE;
    char_u	*selstart = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		selstart_col = 0;
    char_u	*selend = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	first_match = 0;
    ಪೂರ್ಣಾಙ್ಕ		add_ಎಡ = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    ಪೂರ್ಣಾಙ್ಕ		emenu;
#endif
    ಪೂರ್ಣಾಙ್ಕ		l;

    ಯದಿ (matches == ರಿಕ್ತ)	// interrupted completion?
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    ಅನ್ಯಥಾ
	buf = alloc(topframe->fr_width + 1);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    ಯದಿ (match == 0)
	first_match = 0;
    ಅನ್ಯಥಾ ಯದಿ (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_ಎಡ = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	// check if match fits on the screen
	ಪ್ರತಿ (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	ಯದಿ (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	ಯದಿ (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    ಪ್ರತಿ (i = match; i < ಅಂಕೆ_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		ಯದಿ (clen >= topframe->fr_width)
		    ಭಙ್ಗ;
	    }
	    ಯದಿ (i == ಅಂಕೆ_matches)
		add_ಎಡ = TRUE;
	}
    }
    ಯದಿ (add_ಎಡ)
	ಯಾವತ್ (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    ಯದಿ (clen >= topframe->fr_width)
		ಭಙ್ಗ;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    ಯದಿ (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    ಅನ್ಯಥಾ
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    ಯಾವತ್ (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	ಯದಿ (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	ಯದಿ (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	ಅನ್ಯಥಾ
#endif
	ಪ್ರತಿ ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    ಯದಿ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    ಅನ್ಯಥಾ
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(buf + len);
	    }
	}
	ಯದಿ (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	ಯದಿ (++i == ಅಂಕೆ_matches)
		ಭಙ್ಗ;
    }

    ಯದಿ (i != ಅಂಕೆ_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    ಯದಿ (row >= 0)
    {
	ಯದಿ (wild_menu_showing == 0)
	{
	    ಯದಿ (ಸಂದೇಶ_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		ಯದಿ (cmdline_row == Rows - 1)
		{
		    screen_del_ಸಾಲುಗಳು(0, 0, 1, (ಪೂರ್ಣಾಙ್ಕ)Rows, TRUE, 0, ರಿಕ್ತ);
		    ++ಸಂದೇಶ_scrolled;
		}
		ಅನ್ಯಥಾ
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		ಯದಿ (lastwin->w_status_height == 0)
		{
		    save_p_ls = p_ls;
		    save_p_wmh = p_wmh;
		    p_ls = 2;
		    p_wmh = 0;
		    last_status(FALSE);
		}
		wild_menu_showing = WM_SHOWN;
	    }
	}

	screen_puts(buf, row, firstwin->w_wincol, attr);
	ಯದಿ (selstart != ರಿಕ್ತ && highlight)
	{
	    *selend = NUL;
	    screen_puts(selstart, row, firstwin->w_wincol + selstart_col,
		    HL_ATTR(HLF_WM));
	}

	screen_fill(row, row + 1, firstwin->w_wincol + clen,
		firstwin->w_wincol + topframe->fr_width,
		fillchar, fillchar, attr);
    }

    win_redraw_last_status(topframe);
    vim_free(buf);
}

#ಯದಿ defined(FEAT_EVAL)
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

    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ಪ್ರತ್ಯಾಗಮ str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ str;
    vim_free(str);
    ಪ್ರತ್ಯಾಗಮ p;
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
    ಪೂರ್ಣಾಙ್ಕ		srclen,
    ಪೂರ್ಣಾಙ್ಕ		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    ಯದಿ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    buf = alloc(srclen + 1);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    d = buf;
    end = src + srclen;
    ಪ್ರತಿ (p = src; p < end; ++p)
    {
	ಯದಿ (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (ಪೂರ್ಣಾಙ್ಕ)(d - buf);
    ಪ್ರತ್ಯಾಗಮ buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    ಸ್ಥಿರ char_u *
get_next_or_prev_match(ಪೂರ್ಣಾಙ್ಕ mode, expand_T *xp)
{
    ಪೂರ್ಣಾಙ್ಕ	    findex = xp->xp_selected;
    ಪೂರ್ಣಾಙ್ಕ	    ht;

    // When no matches found, return NULL
    ಯದಿ (xp->xp_numfiles <= 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	ಯದಿ (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    ಅನ್ಯಥಾ ಯದಿ (mode == WILD_NEXT)
    {
	// Select the next entry
	++findex;
    }
    ಅನ್ಯಥಾ   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	ಯದಿ (ht > 3)
	    ht -= 2;

	ಯದಿ (mode == WILD_PAGEUP)
	{
	    ಯದಿ (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    ಅನ್ಯಥಾ ಯದಿ (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    ಅನ್ಯಥಾ
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	ಅನ್ಯಥಾ    // mode == WILD_PAGEDOWN
	{
	    ಯದಿ (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    ಅನ್ಯಥಾ ಯದಿ (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    ಅನ್ಯಥಾ
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    ಯದಿ (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	ಯದಿ (xp->xp_orig != ರಿಕ್ತ)
	    findex = -1;
	ಅನ್ಯಥಾ
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    ಯದಿ (p_wmnu)
    {
	ಯದಿ (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	ಅನ್ಯಥಾ ಯದಿ (vim_strchr(p_wop, WOP_PUM) != ರಿಕ್ತ)
	{
	    ಯದಿ (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	ಅನ್ಯಥಾ
	    win_redr_status_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    ಯದಿ (findex == -1)
	ಪ್ರತ್ಯಾಗಮ vim_strsave(xp->xp_orig);
#ಯದಿ defined(FEAT_EVAL)
    ಪ್ರತ್ಯಾಗಮ apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    ಸ್ಥಿರ char_u *
ExpandOne_start(ಪೂರ್ಣಾಙ್ಕ mode, expand_T *xp, char_u *str, ಪೂರ್ಣಾಙ್ಕ options)
{
    ಪೂರ್ಣಾಙ್ಕ		non_suf_match;		// number without matching suffix
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*ss = ರಿಕ್ತ;

    // Do the expansion.
    ಯದಿ (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	ಯದಿ (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (xp->xp_numfiles == 0)
    {
	ಯದಿ (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    ಅನ್ಯಥಾ
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	ಯದಿ (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    ಯದಿ (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    ಅನ್ಯಥಾ
		non_suf_match = 1;
	    ಯದಿ ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		ಪ್ರತಿ (i = 0; i < 2; ++i)
		    ಯದಿ (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    ಯದಿ (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		ಯದಿ (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		ಅನ್ಯಥಾ ಯದಿ (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    ಯದಿ (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    ಪ್ರತ್ಯಾಗಮ ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    ಸ್ಥಿರ char_u *
find_longest_match(expand_T *xp, ಪೂರ್ಣಾಙ್ಕ options)
{
    long_u	len;
    ಪೂರ್ಣಾಙ್ಕ		mb_len = 1;
    ಪೂರ್ಣಾಙ್ಕ		c0, ci;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*ss;

    ಪ್ರತಿ (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	ಯದಿ (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	ಅನ್ಯಥಾ
	    c0 = xp->xp_files[0][len];
	ಪ್ರತಿ (i = 1; i < xp->xp_numfiles; ++i)
	{
	    ಯದಿ (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    ಅನ್ಯಥಾ
		ci = xp->xp_files[i][len];
	    ಯದಿ (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		ಯದಿ (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c0 != ci)
		ಭಙ್ಗ;
	}
	ಯದಿ (i < xp->xp_numfiles)
	{
	    ಯದಿ (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    ಭಙ್ಗ;
	}
    }

    ss = alloc(len + 1);
    ಯದಿ (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    ಪ್ರತ್ಯಾಗಮ ss;
}

    ಶೂನ್ಯ
free_xp_files_extra(expand_T *xp, ಪೂರ್ಣಾಙ್ಕ numfiles)
{
    ಯದಿ (xp->xp_files_abbr != ರಿಕ್ತ)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = ರಿಕ್ತ;
    }
    ಯದಿ (xp->xp_files_kind != ರಿಕ್ತ)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = ರಿಕ್ತ;
    }
    ಯದಿ (xp->xp_files_menu != ರಿಕ್ತ)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = ರಿಕ್ತ;
    }
    ಯದಿ (xp->xp_files_info != ರಿಕ್ತ)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = ರಿಕ್ತ;
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
    ಪೂರ್ಣಾಙ್ಕ		options,
    ಪೂರ್ಣಾಙ್ಕ		mode)
{
    char_u	*ss = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		orig_saved = FALSE;
#ಯದಿ defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    ಪೂರ್ಣಾಙ್ಕ		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    ಯದಿ (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	ಪ್ರತ್ಯಾಗಮ get_next_or_prev_match(mode, xp);

    ಯದಿ (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    ಅನ್ಯಥಾ ಯದಿ (mode == WILD_APPLY)
    {
	ಯದಿ (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	ಅನ್ಯಥಾ
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#ಯದಿ defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    ಯದಿ (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	ಯದಿ (compl_match_array != ರಿಕ್ತ)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    ಯದಿ (mode == WILD_FREE)	// only release file name
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#ಯದಿ defined(FEAT_EVAL)
	ಯದಿ (ss != ರಿಕ್ತ)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    ಯದಿ (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#ಯದಿ defined(FEAT_EVAL)
	ಯದಿ (ss != ರಿಕ್ತ)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    ಯದಿ (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	ವರ್ಣ	*prefix = "";
	ವರ್ಣ	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	ಪೂರ್ಣಾಙ್ಕ	n = xp->xp_numfiles - 1;
	ಪೂರ್ಣಾಙ್ಕ	i;
#ಯದಿ defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = ರಿಕ್ತ;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	ಯದಿ ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    ಯದಿ (escaped != ರಿಕ್ತ)
	    {
		ಪ್ರತಿ (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    ಯದಿ (escaped[i] == ರಿಕ್ತ)
		    {
			ಯಾವತ್ (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			ಭಙ್ಗ;
		    }
		}
		ಯದಿ (escaped != ರಿಕ್ತ)
		    files = escaped;
	    }
	}
#ಅನ್ಯಥಾ
	char_u	**files = xp->xp_files;
#endif

	ಯದಿ (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	ಅನ್ಯಥಾ ಯದಿ (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	ಪ್ರತಿ (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	ಯದಿ (ss != ರಿಕ್ತ)
	{
	    size_t  ss_len = 0;

	    ಪ್ರತಿ (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (ವರ್ಣ *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (ವರ್ಣ *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#ಯದಿ defined(FEAT_EVAL)
	ಯದಿ (escaped != ರಿಕ್ತ)
	{
	    ಪ್ರತಿ (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    ಯದಿ (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    ಯದಿ (!orig_saved)
	vim_free(orig);

#ಯದಿ defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    ಯದಿ (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    ಪ್ರತ್ಯಾಗಮ ss;
}

/*
 * Prepare an expand structure for use.
 */
    ಶೂನ್ಯ
ExpandInit(expand_T *xp)
{
    CLEAR_POINTER(xp);
    xp->xp_backslash = XP_BS_NONE;
    xp->xp_prefix = XP_PREFIX_NONE;
    xp->xp_numfiles = -1;
}

/*
 * Cleanup an expand structure after use.
 */
    ಶೂನ್ಯ
ExpandCleanup(expand_T *xp)
{
    ಯದಿ (xp->xp_numfiles >= 0)
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
	ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_Matches,
	ಪೂರ್ಣಾಙ್ಕ		ಸಾಲುಗಳು,
	ಪೂರ್ಣಾಙ್ಕ		linenr,
	ಪೂರ್ಣಾಙ್ಕ		maxlen,
	ಪೂರ್ಣಾಙ್ಕ		showtail,
	ಪೂರ್ಣಾಙ್ಕ		dir_attr)
{
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    ಪೂರ್ಣಾಙ್ಕ		isdir;
    ಪೂರ್ಣಾಙ್ಕ		lastlen;
    char_u	*p;

    lastlen = 999;
    ಪ್ರತಿ (j = linenr; j < ಅಂಕೆ_Matches; j += ಸಾಲುಗಳು)
    {
	ಯದಿ (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    ಸಂದೇಶ_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    ಸಂದೇಶ_advance(maxlen + 1);
	    ಸಂದೇಶ_puts((ವರ್ಣ *)p);
	    ಸಂದೇಶ_advance(maxlen + 3);
	    ಸಂದೇಶ_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    ಭಙ್ಗ;
	}
	ಪ್ರತಿ (i = maxlen - lastlen; --i >= 0; )
	    ಸಂದೇಶ_putchar(' ');
	ಯದಿ (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    ಯದಿ (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_path;
		char_u	*path;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_path = expand_env_save_opt(matches[j], TRUE, ರಿಕ್ತ);
		path = exp_path != ರಿಕ್ತ ? exp_path : matches[j];
		halved_slash = backslash_halve_save(path);
		isdir = mch_isdir(halved_slash != ರಿಕ್ತ ? halved_slash
			: matches[j]);
		vim_free(exp_path);
		ಯದಿ (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    ಅನ್ಯಥಾ
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    ಯದಿ (showtail)
		p = SHOW_MATCH(j);
	    ಅನ್ಯಥಾ
	    {
		home_replace(ರಿಕ್ತ, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = ಸಂದೇಶ_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    ಯದಿ (ಸಂದೇಶ_col > 0)	// when not wrapped around
    {
	ಸಂದೇಶ_clr_eos();
	ಸಂದೇಶ_putchar('\n');
    }
    out_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_Matches,
	ಪೂರ್ಣಾಙ್ಕ		showtail,
	ಪೂರ್ಣಾಙ್ಕ		noselect)
{
    ಪೂರ್ಣಾಙ್ಕ	retval = cmdline_pum_create(ccline, xp, matches, ಅಂಕೆ_Matches, showtail);

    ಯದಿ (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    ಪೂರ್ಣಾಙ್ಕ
showmatches(
    expand_T	*xp,
    ಪೂರ್ಣಾಙ್ಕ		display_wildmenu,
    ಪೂರ್ಣಾಙ್ಕ		display_list,
    ಪೂರ್ಣಾಙ್ಕ		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_Matches;
    char_u	**matches;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		maxlen;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಙ್ಕ		columns;
    ಪೂರ್ಣಾಙ್ಕ		attr;
    ಪೂರ್ಣಾಙ್ಕ		showtail;
    ಪೂರ್ಣಾಙ್ಕ		noselect = (wim_flags_arg & WIM_NOSELECT);
    ಪೂರ್ಣಾಙ್ಕ		noinsert = (wim_flags_arg & WIM_NOINSERT);
    ಪೂರ್ಣಾಙ್ಕ		cmdline_unchanged = noselect || noinsert;

    ಯದಿ (xp->xp_numfiles == -1)
    {
	ಪೂರ್ಣಾಙ್ಕ retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&ಅಂಕೆ_Matches, &matches);
	ಯದಿ (retval != EXPAND_OK)
	    ಪ್ರತ್ಯಾಗಮ retval;
	showtail = expand_showtail(xp);
    }
    ಅನ್ಯಥಾ
    {
	ಅಂಕೆ_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    ಯದಿ (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ show_pum_matches(ccline, xp, matches, ಅಂಕೆ_Matches,
		showtail && !cmdline_unchanged, noselect);

    ಯದಿ (display_list)
    {
	ಸಂದೇಶ_didany = FALSE;		// lines_left will be set
	ಸಂದೇಶ_start();			// prepare for paging
	ಸಂದೇಶ_putchar('\n');
	out_flush();
	cmdline_row = ಸಂದೇಶ_row;
	ಸಂದೇಶ_didany = FALSE;		// lines_left will be set again
	ಸಂದೇಶ_start();			// prepare for paging
    }

    ಯದಿ (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    ಅನ್ಯಥಾ ಯದಿ (display_wildmenu && !display_list)
	win_redr_status_matches(xp, ಅಂಕೆ_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    ಅನ್ಯಥಾ ಯದಿ (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_Matches; ++i)
	{
	    ಪೂರ್ಣಾಙ್ಕ	len;
	    ಯದಿ (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(ರಿಕ್ತ, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    ಅನ್ಯಥಾ
		len = vim_strsize(SHOW_MATCH(i));
	    ಯದಿ (len > maxlen)
		maxlen = len;
	}

	ಯದಿ (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    ಸಾಲುಗಳು = ಅಂಕೆ_Matches;
	ಅನ್ಯಥಾ
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    ಯದಿ (columns < 1)
		columns = 1;
	    ಸಾಲುಗಳು = (ಅಂಕೆ_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	ಯದಿ (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    ಸಂದೇಶ_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    ಸಂದೇಶ_clr_eos();
	    ಸಂದೇಶ_advance(maxlen - 3);
	    ಸಂದೇಶ_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	ಪ್ರತಿ (i = 0; i < ಸಾಲುಗಳು; ++i)
	{
	    showmatches_oneline(xp, matches, ಅಂಕೆ_Matches, ಸಾಲುಗಳು, i,
						maxlen, showtail, attr);
	    ಯದಿ (got_int)
	    {
		got_int = FALSE;
		ಭಙ್ಗ;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = ಸಂದೇಶ_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    ಯದಿ (display_wildmenu && display_list)
    {
	ಯದಿ (vim_strchr(p_wop, WOP_PUM) != ರಿಕ್ತ)
	    (ಶೂನ್ಯ)show_pum_matches(ccline, xp, matches, ಅಂಕೆ_Matches,
		    showtail && !cmdline_unchanged, noselect);
	ಅನ್ಯಥಾ
	    win_redr_status_matches(xp, ಅಂಕೆ_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    ಯದಿ (xp->xp_numfiles == -1)
    {
	FreeWild(ಅಂಕೆ_Matches, matches);
	free_xp_files_extra(xp, ಅಂಕೆ_Matches);
    }

    ಪ್ರತ್ಯಾಗಮ EXPAND_OK;
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
    ಪೂರ್ಣಾಙ್ಕ		had_sep = FALSE;

    ಪ್ರತಿ (p = s; *p != NUL; )
    {
	ಯದಿ (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    ಪ್ರತ್ಯಾಗಮ t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    ಯದಿ (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    end = gettail(xp->xp_pattern);
    ಯದಿ (end == xp->xp_pattern)		// there is no path separator
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತಿ (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	ಯದಿ (rem_backslash(s))
	    ++s;
	ಅನ್ಯಥಾ ಯದಿ (vim_strchr((char_u *)"*?[", *s) != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಪೂರ್ಣಾಙ್ಕ		len,
    ಪೂರ್ಣಾಙ್ಕ		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    ಪೂರ್ಣಾಙ್ಕ		new_len;
    char_u	*tail;
    ಪೂರ್ಣಾಙ್ಕ		ends_in_star;

    ಯದಿ (context != EXPAND_FILES
	    && context != EXPAND_FILES_IN_PATH
	    && context != EXPAND_SHELLCMD
	    && context != EXPAND_DIRECTORIES
	    && context != EXPAND_DIRS_IN_CDPATH)
    {
	// Matching will be done internally (on something other than files).
	// So we convert the file-matching-type wildcards into our kind for
	// use with vim_regcomp().  First work out how long it will be:

	// For help tags the translation is done in find_help_tags().
	// For a tag pattern starting with "/" no translation is needed.
	ಯದಿ (context == EXPAND_FINDFUNC
		|| context == EXPAND_HELP
		|| context == EXPAND_COLORS
		|| context == EXPAND_COMPILER
		|| context == EXPAND_OWNSYNTAX
		|| context == EXPAND_FILETYPE
		|| context == EXPAND_KEYMAP
		|| context == EXPAND_PACKADD
		|| context == EXPAND_RUNTIME
		|| ((context == EXPAND_TAGS_LISTFILES
			|| context == EXPAND_TAGS)
		    && fname[0] == '/'))
	    retval = vim_strnsave(fname, len);
	ಅನ್ಯಥಾ
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    ಪ್ರತಿ (i = 0; i < len; i++)
	    {
		ಯದಿ (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		ಯದಿ (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		ಯದಿ ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    ಯದಿ (retval != ರಿಕ್ತ)
	    {
		retval[0] = '^';
		j = 1;
		ಪ್ರತಿ (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    ಯದಿ (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			ಭಙ್ಗ;

		    ವಿಕಲ್ಪ (fname[i])
		    {
			ಪಕ್ಷ '*':   retval[j++] = '.';
				    ಭಙ್ಗ;
			ಪಕ್ಷ '~':   retval[j++] = '\\';
				    ಭಙ್ಗ;
			ಪಕ್ಷ '?':   retval[j] = '.';
				    ಅನುವರ್ತ;
			ಪಕ್ಷ '.':   ಯದಿ (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    ಭಙ್ಗ;
			ಪಕ್ಷ '\\':  ಯದಿ (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    ಭಙ್ಗ;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	retval = alloc(len + 4);
	ಯದಿ (retval != ರಿಕ್ತ)
	{
	    vim_strncpy(retval, fname, len);

	    // Don't add a star to *, ~, ~user, $var or `cmd`.
	    // * would become **, which walks the whole tree.
	    // ~ would be at the start of the file name, but not the tail.
	    // $ could be anywhere in the tail.
	    // ` could be anywhere in the file name.
	    // When the name ends in '$' don't add a star, remove the '$'.
	    tail = gettail(retval);
	    ends_in_star = (len > 0 && retval[len - 1] == '*');
#ifndef BACKSLASH_IN_FILENAME
	    ಪ್ರತಿ (i = len - 2; i >= 0; --i)
	    {
		ಯದಿ (retval[i] != '\\')
		    ಭಙ್ಗ;
		ends_in_star = !ends_in_star;
	    }
#endif
	    ಯದಿ ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == ರಿಕ್ತ
		    && vim_strchr(retval, '`') == ರಿಕ್ತ)
		retval[len++] = '*';
	    ಅನ್ಯಥಾ ಯದಿ (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    ಪ್ರತ್ಯಾಗಮ retval;
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
set_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    ಯದಿ ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_ಹುಡುಕು_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_ಹುಡುಕು_EXTRA
	ಹುಡುಕು_first_ಸಾಲು = 0; // Search entire buffer
#endif
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    ಯದಿ (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->ಒಳಹರಿವು_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Fallback to command-line expansion
    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, TRUE);
}

/*
 * Sets the index of a built-in or user defined command 'cmd' in eap->cmdidx.
 * For user defined commands, the completion context is set in 'xp' and the
 * completion flags in 'complp'.
 *
 * Returns a pointer to the text after the command or NULL for failure.
 */
    ಸ್ಥಿರ char_u *
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, ಪೂರ್ಣಾಙ್ಕ *complp)
{
    char_u	*p = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    ಯದಿ (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    ಅನ್ಯಥಾ
    {
	p = cmd;
	ಯಾವತ್ (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	ಯದಿ (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    ಯಾವತ್ (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	ಯದಿ (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    ಯಾವತ್ (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	ಯದಿ (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != ರಿಕ್ತ)
	    ++p;
	len = (ಪೂರ್ಣಾಙ್ಕ)(p - cmd);

	ಯದಿ (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	ಯದಿ ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    ಯಾವತ್ (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    ಯದಿ (*p == NUL && ASCII_ISALNUM(p[-1]))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (eap->cmdidx == CMD_SIZE)
    {
	ಯದಿ (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != ರಿಕ್ತ)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	ಅನ್ಯಥಾ ಯದಿ (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, ರಿಕ್ತ, xp, complp);
	    ಯದಿ (p == ರಿಕ್ತ)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    ಯದಿ (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	ಪೂರ್ಣಾಙ್ಕ		usefilter,
	expand_T	*xp,
	ಪೂರ್ಣಾಙ್ಕ		*complp)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		in_quote = FALSE;
    char_u	*bow = ರಿಕ್ತ;	// Beginning of word
    ಪೂರ್ಣಾಙ್ಕ		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    ಯಾವತ್ (*p != NUL)
    {
	ಯದಿ (has_mbyte)
	    c = mb_ptr2char(p);
	ಅನ್ಯಥಾ
	    c = *p;
	ಯದಿ (c == '\\' && p[1] != NUL)
	    ++p;
	ಅನ್ಯಥಾ ಯದಿ (c == '`')
	{
	    ಯದಿ (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	ಅನ್ಯಥಾ ಯದಿ (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != ರಿಕ್ತ && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    ಯಾವತ್ (*p != NUL)
	    {
		ಯದಿ (has_mbyte)
		    c = mb_ptr2char(p);
		ಅನ್ಯಥಾ
		    c = *p;
		ಯದಿ (c == '`' || vim_isfilec_or_wc(c))
		    ಭಙ್ಗ;
		ಯದಿ (has_mbyte)
		    len = (*mb_ptr2len)(p);
		ಅನ್ಯಥಾ
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    ಯದಿ (in_quote)
		bow = p;
	    ಅನ್ಯಥಾ
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    ಯದಿ (bow != ರಿಕ್ತ && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    ಯದಿ (usefilter
	    || (eap != ರಿಕ್ತ
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	ಯದಿ (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    ಯದಿ (*xp->xp_pattern == '$')
    {
	ಪ್ರತಿ (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    ಯದಿ (!vim_isIDc(*p))
		ಭಙ್ಗ;
	ಯದಿ (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    ಯದಿ (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    ಯದಿ (*xp->xp_pattern == '~')
    {
	ಪ್ರತಿ (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	ಯದಿ (*p == NUL && p > xp->xp_pattern + 1
		&& match_user(xp->xp_pattern + 1) >= 1)
	{
	    xp->xp_context = EXPAND_USER;
	    ++xp->xp_pattern;
	}
    }
}

/*
 * Set the completion context for the "++opt=arg" argument.  Always returns
 * NULL.
 */
    ಸ್ಥಿರ char_u *
set_context_in_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    ಯದಿ (p == ರಿಕ್ತ)
	xp->xp_pattern = arg;
    ಅನ್ಯಥಾ
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ifdef FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    ಸ್ಥಿರ char_u *
set_context_in_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    ಯದಿ (p == ರಿಕ್ತ)
	xp->xp_pattern = arg;
    ಅನ್ಯಥಾ
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    ಸ್ಥಿರ char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    ಯದಿ (*arg != NUL)
	arg = skip_vimgrep_pat(arg, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (arg == ರಿಕ್ತ || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ skipwhite(arg);
}

#ifdef FEAT_ಹುಡುಕು_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    ಸ್ಥಿರ char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    ಯದಿ (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	ಯದಿ (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    ಪ್ರತ್ಯಾಗಮ find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    ಸ್ಥಿರ char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    ಪೂರ್ಣಾಙ್ಕ		delim;

    delim = *arg;	    // get the delimiter
    ಯದಿ (delim)
	++arg;		    // skip delimiter if there is one

    ಯಾವತ್ (arg[0] != NUL && arg[0] != delim)
    {
	ಯದಿ (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    ಯದಿ (arg[0] != NUL)
	ಪ್ರತ್ಯಾಗಮ arg + 1;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    ಸ್ಥಿರ char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    ಪೂರ್ಣಾಙ್ಕ		delim;

    delim = *arg;
    ಯದಿ (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	ಯದಿ (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    ಯಾವತ್ (arg[0] != NUL && arg[0] != delim)
	    {
		ಯದಿ (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    ಯದಿ (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    ಯಾವತ್ (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == ರಿಕ್ತ)
	++arg;
    ಯದಿ (arg[0] != NUL)
	ಪ್ರತ್ಯಾಗಮ arg;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    ಸ್ಥಿರ char_u *
find_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    ಯದಿ (*arg != '/')
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Match regexp, not just whole words
    ಪ್ರತಿ (++arg; *arg && *arg != '/'; arg++)
	ಯದಿ (*arg == '\\' && arg[1] != NUL)
	    arg++;
    ಯದಿ (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	ಯದಿ (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == ರಿಕ್ತ)
	    xp->xp_context = EXPAND_NOTHING;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ arg;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    ಸ್ಥಿರ char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    ಯಾವತ್ ((xp->xp_pattern = vim_strchr(arg, ' ')) != ರಿಕ್ತ)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    ಯದಿ (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    ಸ್ಥಿರ char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    ಯದಿ (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_pattern = skipwhite(p);
	}
	ಅನ್ಯಥಾ
	    xp->xp_context = EXPAND_NOTHING;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

ಸ್ಥಿರ ಗಣನಾ
{
    EXP_FILETYPECMD_ALL,	// expand all :filetype values
    EXP_FILETYPECMD_PLUGIN,	// expand plugin on off
    EXP_FILETYPECMD_INDENT,	// expand indent on off
    EXP_FILETYPECMD_ONOFF,	// expand on off
} filetype_expand_what;

#define EXPAND_FILETYPECMD_PLUGIN 0x01
#define EXPAND_FILETYPECMD_INDENT 0x02
#define EXPAND_FILETYPECMD_ONOFF  0x04

#ifdef FEAT_EVAL
ಸ್ಥಿರ ಗಣನಾ
{
    EXP_BREAKPT_ADD,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    ಸ್ಥಿರ char_u *
set_context_in_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_start;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_pattern = arg;

    ಯದಿ (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    ಅನ್ಯಥಾ ಯದಿ (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    ಅನ್ಯಥಾ
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    subcmd_start = p;

    ಯದಿ (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	ಯದಿ (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    ಯದಿ (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    p = skipwhite(p);
	}
	ಯದಿ (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	ಅನ್ಯಥಾ
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = ರಿಕ್ತ;

    p = skipwhite(arg);
    ಯದಿ (VIM_ISDIGIT(*p))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    ಸ್ಥಿರ char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    ಪೂರ್ಣಾಙ್ಕ    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (;;)
    {
	ಯದಿ (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    ಅನುವರ್ತ;
	}
	ಯದಿ (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    ಅನುವರ್ತ;
	}
	ಭಙ್ಗ;
    }

    ಯದಿ ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    ಅನ್ಯಥಾ ಯದಿ ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    ಅನ್ಯಥಾ ಯದಿ ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_context_with_pattern(expand_T *xp)
{
    ಪೂರ್ಣಾಙ್ಕ		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_ಹುಡುಕು_EXTRA
    ಪೂರ್ಣಾಙ್ಕ		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    ಯದಿ (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	ಪ್ರತ್ಯಾಗಮ;
#endif

    xp->xp_pattern = ccline->cmdbuff + skiplen;
    xp->xp_pattern_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_PATTERN_IN_BUF;
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
set_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	long		argt,
	ಪೂರ್ಣಾಙ್ಕ		compl,
	ಪೂರ್ಣಾಙ್ಕ		forceit)
{
    char_u  *nextcmd;

    ವಿಕಲ್ಪ (cmdidx)
    {
	ಪಕ್ಷ CMD_find:
	ಪಕ್ಷ CMD_sfind:
	ಪಕ್ಷ CMD_tabfind:
	    ಯದಿ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_cd:
	ಪಕ್ಷ CMD_chdir:
	ಪಕ್ಷ CMD_tcd:
	ಪಕ್ಷ CMD_tchdir:
	ಪಕ್ಷ CMD_lcd:
	ಪಕ್ಷ CMD_lchdir:
	    ಯದಿ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	ಪಕ್ಷ CMD_aboveleft:
	ಪಕ್ಷ CMD_argdo:
	ಪಕ್ಷ CMD_belowright:
	ಪಕ್ಷ CMD_botright:
	ಪಕ್ಷ CMD_browse:
	ಪಕ್ಷ CMD_bufdo:
	ಪಕ್ಷ CMD_cdo:
	ಪಕ್ಷ CMD_cfdo:
	ಪಕ್ಷ CMD_confirm:
	ಪಕ್ಷ CMD_debug:
	ಪಕ್ಷ CMD_folddoclosed:
	ಪಕ್ಷ CMD_folddoopen:
	ಪಕ್ಷ CMD_hide:
	ಪಕ್ಷ CMD_horizontal:
	ಪಕ್ಷ CMD_keepalt:
	ಪಕ್ಷ CMD_keepjumps:
	ಪಕ್ಷ CMD_keepmarks:
	ಪಕ್ಷ CMD_keeppatterns:
	ಪಕ್ಷ CMD_ldo:
	ಪಕ್ಷ CMD_leftabove:
	ಪಕ್ಷ CMD_lfdo:
	ಪಕ್ಷ CMD_lockmarks:
	ಪಕ್ಷ CMD_noautocmd:
	ಪಕ್ಷ CMD_noswapfile:
	ಪಕ್ಷ CMD_rightbelow:
	ಪಕ್ಷ CMD_sandbox:
	ಪಕ್ಷ CMD_silent:
	ಪಕ್ಷ CMD_tab:
	ಪಕ್ಷ CMD_tabdo:
	ಪಕ್ಷ CMD_topleft:
	ಪಕ್ಷ CMD_unsilent:
	ಪಕ್ಷ CMD_verbose:
	ಪಕ್ಷ CMD_vertical:
	ಪಕ್ಷ CMD_windo:
	ಪಕ್ಷ CMD_vim9cmd:
	ಪಕ್ಷ CMD_legacy:
	    ಪ್ರತ್ಯಾಗಮ arg;

	ಪಕ್ಷ CMD_filter:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_ಹುಡುಕು_EXTRA
	ಪಕ್ಷ CMD_match:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	ಪಕ್ಷ CMD_command:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_user_cmd(xp, arg);

	ಪಕ್ಷ CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_global:
	ಪಕ್ಷ CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    ಯದಿ (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    ಪ್ರತ್ಯಾಗಮ nextcmd;

	ಪಕ್ಷ CMD_and:
	ಪಕ್ಷ CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    ಯದಿ (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    ಪ್ರತ್ಯಾಗಮ nextcmd;

	ಪಕ್ಷ CMD_isearch:
	ಪಕ್ಷ CMD_dsearch:
	ಪಕ್ಷ CMD_ilist:
	ಪಕ್ಷ CMD_dlist:
	ಪಕ್ಷ CMD_ijump:
	ಪಕ್ಷ CMD_psearch:
	ಪಕ್ಷ CMD_djump:
	ಪಕ್ಷ CMD_isplit:
	ಪಕ್ಷ CMD_dsplit:
	    ಪ್ರತ್ಯಾಗಮ find_cmd_after_isearch_cmd(xp, arg);
	ಪಕ್ಷ CMD_autocmd:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_autocmd(xp, arg, FALSE);
	ಪಕ್ಷ CMD_doautocmd:
	ಪಕ್ಷ CMD_doautoall:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_autocmd(xp, arg, TRUE);
	ಪಕ್ಷ CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_tag:
	ಪಕ್ಷ CMD_stag:
	ಪಕ್ಷ CMD_ptag:
	ಪಕ್ಷ CMD_ltag:
	ಪಕ್ಷ CMD_tselect:
	ಪಕ್ಷ CMD_stselect:
	ಪಕ್ಷ CMD_ptselect:
	ಪಕ್ಷ CMD_tjump:
	ಪಕ್ಷ CMD_stjump:
	ಪಕ್ಷ CMD_ptjump:
	    ಯದಿ (vim_strchr(p_wop, WOP_TAGFILE) != ರಿಕ್ತ)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    ಅನ್ಯಥಾ
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;
#ifdef FEAT_SYN_HL
	ಪಕ್ಷ CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    ಭಙ್ಗ;
#endif
#ifdef FEAT_EVAL
	ಪಕ್ಷ CMD_final:
	ಪಕ್ಷ CMD_const:
	ಪಕ್ಷ CMD_let:
	ಪಕ್ಷ CMD_var:
	ಪಕ್ಷ CMD_if:
	ಪಕ್ಷ CMD_elseif:
	ಪಕ್ಷ CMD_while:
	ಪಕ್ಷ CMD_for:
	ಪಕ್ಷ CMD_echo:
	ಪಕ್ಷ CMD_echon:
	ಪಕ್ಷ CMD_execute:
	ಪಕ್ಷ CMD_echomsg:
	ಪಕ್ಷ CMD_echoerr:
	ಪಕ್ಷ CMD_echoconsole:
	ಪಕ್ಷ CMD_echowindow:
	ಪಕ್ಷ CMD_call:
	ಪಕ್ಷ CMD_return:
	ಪಕ್ಷ CMD_cexpr:
	ಪಕ್ಷ CMD_caddexpr:
	ಪಕ್ಷ CMD_cgetexpr:
	ಪಕ್ಷ CMD_lexpr:
	ಪಕ್ಷ CMD_laddexpr:
	ಪಕ್ಷ CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_unlet:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_unlet_cmd(xp, arg);
	ಪಕ್ಷ CMD_function:
	ಪಕ್ಷ CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    ಭಙ್ಗ;
#ifdef FEAT_CSCOPE
	ಪಕ್ಷ CMD_cscope:
	ಪಕ್ಷ CMD_lcscope:
	ಪಕ್ಷ CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    ಭಙ್ಗ;
#endif
#ifdef FEAT_SIGNS
	ಪಕ್ಷ CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ CMD_bdelete:
	ಪಕ್ಷ CMD_bwipeout:
	ಪಕ್ಷ CMD_bunload:
	    ಯಾವತ್ ((xp->xp_pattern = vim_strchr(arg, ' ')) != ರಿಕ್ತ)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	ಪಕ್ಷ CMD_buffer:
	ಪಕ್ಷ CMD_sbuffer:
	ಪಕ್ಷ CMD_pbuffer:
	ಪಕ್ಷ CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;
#ifdef FEAT_DIFF
	ಪಕ್ಷ CMD_diffget:
	ಪಕ್ಷ CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ CMD_USER:
	ಪಕ್ಷ CMD_USER_BUF:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	ಪಕ್ಷ CMD_map:	    ಪಕ್ಷ CMD_noremap:
	ಪಕ್ಷ CMD_nmap:	    ಪಕ್ಷ CMD_nnoremap:
	ಪಕ್ಷ CMD_vmap:	    ಪಕ್ಷ CMD_vnoremap:
	ಪಕ್ಷ CMD_omap:	    ಪಕ್ಷ CMD_onoremap:
	ಪಕ್ಷ CMD_imap:	    ಪಕ್ಷ CMD_inoremap:
	ಪಕ್ಷ CMD_cmap:	    ಪಕ್ಷ CMD_cnoremap:
	ಪಕ್ಷ CMD_lmap:	    ಪಕ್ಷ CMD_lnoremap:
	ಪಕ್ಷ CMD_smap:	    ಪಕ್ಷ CMD_snoremap:
	ಪಕ್ಷ CMD_tmap:	    ಪಕ್ಷ CMD_tnoremap:
	ಪಕ್ಷ CMD_xmap:	    ಪಕ್ಷ CMD_xnoremap:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	ಪಕ್ಷ CMD_unmap:
	ಪಕ್ಷ CMD_nunmap:
	ಪಕ್ಷ CMD_vunmap:
	ಪಕ್ಷ CMD_ounmap:
	ಪಕ್ಷ CMD_iunmap:
	ಪಕ್ಷ CMD_cunmap:
	ಪಕ್ಷ CMD_lunmap:
	ಪಕ್ಷ CMD_sunmap:
	ಪಕ್ಷ CMD_tunmap:
	ಪಕ್ಷ CMD_xunmap:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	ಪಕ್ಷ CMD_mapclear:
	ಪಕ್ಷ CMD_nmapclear:
	ಪಕ್ಷ CMD_vmapclear:
	ಪಕ್ಷ CMD_omapclear:
	ಪಕ್ಷ CMD_imapclear:
	ಪಕ್ಷ CMD_cmapclear:
	ಪಕ್ಷ CMD_lmapclear:
	ಪಕ್ಷ CMD_smapclear:
	ಪಕ್ಷ CMD_tmapclear:
	ಪಕ್ಷ CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_abbreviate:	ಪಕ್ಷ CMD_noreabbrev:
	ಪಕ್ಷ CMD_cabbrev:	ಪಕ್ಷ CMD_cnoreabbrev:
	ಪಕ್ಷ CMD_iabbrev:	ಪಕ್ಷ CMD_inoreabbrev:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	ಪಕ್ಷ CMD_unabbreviate:
	ಪಕ್ಷ CMD_cunabbrev:
	ಪಕ್ಷ CMD_iunabbrev:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	ಪಕ್ಷ CMD_menu:	    ಪಕ್ಷ CMD_noremenu:	    ಪಕ್ಷ CMD_unmenu:
	ಪಕ್ಷ CMD_amenu:	    ಪಕ್ಷ CMD_anoremenu:	    ಪಕ್ಷ CMD_aunmenu:
	ಪಕ್ಷ CMD_nmenu:	    ಪಕ್ಷ CMD_nnoremenu:	    ಪಕ್ಷ CMD_nunmenu:
	ಪಕ್ಷ CMD_vmenu:	    ಪಕ್ಷ CMD_vnoremenu:	    ಪಕ್ಷ CMD_vunmenu:
	ಪಕ್ಷ CMD_omenu:	    ಪಕ್ಷ CMD_onoremenu:	    ಪಕ್ಷ CMD_ounmenu:
	ಪಕ್ಷ CMD_imenu:	    ಪಕ್ಷ CMD_inoremenu:	    ಪಕ್ಷ CMD_iunmenu:
	ಪಕ್ಷ CMD_cmenu:	    ಪಕ್ಷ CMD_cnoremenu:	    ಪಕ್ಷ CMD_cunmenu:
	ಪಕ್ಷ CMD_tlmenu:    ಪಕ್ಷ CMD_tlnoremenu:    ಪಕ್ಷ CMD_tlunmenu:
	ಪಕ್ಷ CMD_tmenu:				    ಪಕ್ಷ CMD_tunmenu:
	ಪಕ್ಷ CMD_popup:	    ಪಕ್ಷ CMD_tearoff:	    ಪಕ್ಷ CMD_emenu:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	ಪಕ್ಷ CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    ಭಙ್ಗ;

#ಯದಿ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	ಪಕ್ಷ CMD_language:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_lang_cmd(xp, arg);
#endif
#ಯದಿ defined(FEAT_PROFILE)
	ಪಕ್ಷ CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;
#ಯದಿ defined(FEAT_PROFILE)
	ಪಕ್ಷ CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;
#endif

	ಪಕ್ಷ CMD_argdelete:
	    ಯಾವತ್ ((xp->xp_pattern = vim_strchr(arg, ' ')) != ರಿಕ್ತ)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    ಭಙ್ಗ;

#ifdef FEAT_EVAL
	ಪಕ್ಷ CMD_breakadd:
	ಪಕ್ಷ CMD_profdel:
	ಪಕ್ಷ CMD_breakdel:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_breakadd_cmd(xp, arg, cmdidx);

	ಪಕ್ಷ CMD_scriptnames:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_scriptnames_cmd(xp, arg);
#endif
	ಪಕ್ಷ CMD_filetype:
	    ಪ್ರತ್ಯಾಗಮ set_context_in_filetype_cmd(xp, arg);

	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    ಸ್ಥಿರ char_u *
set_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    ಪೂರ್ಣಾಙ್ಕ			len = 0;
    exarg_T		ea;
    ಪೂರ್ಣಾಙ್ಕ			compl = EXPAND_NOTHING;
    ಪೂರ್ಣಾಙ್ಕ			forceit = FALSE;
    ಪೂರ್ಣಾಙ್ಕ			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_ಸಾಲು = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    ಪ್ರತಿ (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != ರಿಕ್ತ; cmd++)
	;
    xp->xp_pattern = cmd;

    ಯದಿ (*cmd == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    ಯದಿ (*cmd == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (*cmd == '|' || *cmd == '\n')
	ಪ್ರತ್ಯಾಗಮ cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    ಯದಿ (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    ಯದಿ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    ಯದಿ ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	ಯಾವತ್ (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    ಯಾವತ್ (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    ಯದಿ (*p == NUL)
	    {
		ಯದಿ (ea.argt & EX_ARGOPT)
		    ಪ್ರತ್ಯಾಗಮ set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		ಯದಿ (ea.cmdidx == CMD_terminal)
		    ಪ್ರತ್ಯಾಗಮ set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    ಯದಿ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	ಯದಿ (*arg == '>')			// append
	{
	    ಯದಿ (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	ಅನ್ಯಥಾ ಯದಿ (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    ಯದಿ (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	ಯದಿ (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    ಯದಿ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ಯಾವತ್ (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    ಯದಿ ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	ಯದಿ (*arg == NUL)
	    ಪ್ರತ್ಯಾಗಮ p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    ಯದಿ ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	ಯದಿ (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	ಯಾವತ್ (*p)
	{
	    ಯದಿ (*p == Ctrl_V)
	    {
		ಯದಿ (p[1] != NUL)
		    ++p;
	    }
	    ಅನ್ಯಥಾ ಯದಿ ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		ಯದಿ (*(p - 1) != '\\')
		{
		    ಯದಿ (*p == '|' || *p == '\n')
			ಪ್ರತ್ಯಾಗಮ p + 1;
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    ಯದಿ (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == ರಿಕ್ತ)
	// no arguments allowed but there is something
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(buff);
    ಯಾವತ್ (*p && p < buff + len)
    {
	ಯದಿ (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    ಯದಿ (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    ಪ್ರತ್ಯಾಗಮ set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    ಶೂನ್ಯ
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    ಪೂರ್ಣಾಙ್ಕ		len,	    // length of command line (excl. NUL)
    ಪೂರ್ಣಾಙ್ಕ		col,	    // position of cursor
    ಪೂರ್ಣಾಙ್ಕ		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    ಪೂರ್ಣಾಙ್ಕ			context;
#endif
    ಪೂರ್ಣಾಙ್ಕ		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    ಯದಿ (col < len)
    {
	old_char = str[col];
	str[col] = NUL;
    }
    nextcomm = str;

    filetype_expand_what = EXP_FILETYPECMD_ALL;
#ifdef FEAT_EVAL
    breakpt_expand_what = EXP_BREAKPT_ADD;
#endif

#ifdef FEAT_EVAL
    ಯದಿ (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    ಅನ್ಯಥಾ ಯದಿ (use_ccline && ccline->ಒಳಹರಿವು_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	ಯದಿ (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(ರಿಕ್ತ, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    ಅನ್ಯಥಾ
#endif
	ಯಾವತ್ (nextcomm != ರಿಕ್ತ)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_ಸಾಲು = str;
    xp->xp_col = col;

    ಯದಿ (col < len)
	str[col] = old_char;
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
    ಪೂರ್ಣಾಙ್ಕ
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    ಪೂರ್ಣಾಙ್ಕ		col,		// position of cursor
    ಪೂರ್ಣಾಙ್ಕ		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		options = WILD_ADD_SLASH|WILD_SILENT;

    ಯದಿ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ಪ್ರತ್ಯಾಗಮ EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    ಯದಿ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ಪ್ರತ್ಯಾಗಮ EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (ಪೂರ್ಣಾಙ್ಕ)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	ಪೂರ್ಣಾಙ್ಕ	pat_len = xp->xp_pattern_len;
#ಯದಿ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	ಯದಿ (unesc != ರಿಕ್ತ)
	    pat_src = unesc;
#endif

	ಯದಿ (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	ಅನ್ಯಥಾ
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#ಯದಿ defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    ಯದಿ (file_str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ EXPAND_UNSUCCESSFUL;

    ಯದಿ (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    ಯದಿ (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = ರಿಕ್ತ;
    }
    vim_free(file_str);

    ಪ್ರತ್ಯಾಗಮ EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches,
	ಪೂರ್ಣಾಙ್ಕ		flags,
	ಪೂರ್ಣಾಙ್ಕ		options)
{
    ಪೂರ್ಣಾಙ್ಕ		free_pat = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    ಯದಿ (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	ಯದಿ (pat == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ret;

	pat_end = pat + pat_len;
	ಪ್ರತಿ (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    ಯದಿ (*p != '\\')
		ಅನುವರ್ತ;

	    ಯದಿ (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (xp->xp_backslash & XP_BS_COMMA)
	    {
		ಯದಿ (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		ಅನ್ಯಥಾ ಯದಿ (*(p + 1) == ',')
		{
		    from = p + 1;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    --pat_end;
		}
#endif
	    }
	}
    }

    ಯದಿ (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, ಅಂಕೆ_Matches);
#endif
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	ಅನ್ಯಥಾ ಯದಿ (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	ಅನ್ಯಥಾ ಯದಿ (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	ಅನ್ಯಥಾ
	    flags = (flags | EW_DIR) & ~EW_FILE;
	ಯದಿ (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, ಅಂಕೆ_Matches, matches, flags);
    }
    ಯದಿ (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    ಯದಿ (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	ಪೂರ್ಣಾಙ್ಕ j;

	ಪ್ರತಿ (j = 0; j < *ಅಂಕೆ_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    ಯಾವತ್ (*ptr != NUL)
	    {
		ಯದಿ (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		ಅನ್ಯಥಾ ಯದಿ (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    ಸ್ಥಿರ char_u *
get_behave_arg(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx == 0)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"mswin";
    ಯದಿ (idx == 1)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"xterm";
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    ಸ್ಥಿರ char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx < 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	ವರ್ಣ	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	ಪ್ರತ್ಯಾಗಮ (char_u *)opts_all[idx];
    }
    ಯದಿ (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	ವರ್ಣ	*opts_plugin[] = {"plugin", "on", "off"};
	ಪ್ರತ್ಯಾಗಮ (char_u *)opts_plugin[idx];
    }
    ಯದಿ (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	ವರ್ಣ	*opts_indent[] = {"indent", "on", "off"};
	ಪ್ರತ್ಯಾಗಮ (char_u *)opts_indent[idx];
    }
    ಯದಿ (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	ವರ್ಣ	*opts_onoff[] = {"on", "off"};
	ಪ್ರತ್ಯಾಗಮ (char_u *)opts_onoff[idx];
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    ಸ್ಥಿರ char_u *
get_breakadd_arg(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx >= 0 && idx <= 3)
    {
	ವರ್ಣ	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	ಯದಿ (breakpt_expand_what == EXP_BREAKPT_ADD)
	    ಪ್ರತ್ಯಾಗಮ (char_u *)opts[idx];
	ಅನ್ಯಥಾ ಯದಿ (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    ಯದಿ (idx <= 2)
		ಪ್ರತ್ಯಾಗಮ (char_u *)opts[idx + 1];
	}
	ಅನ್ಯಥಾ
	{
	    // profdel {func, file}
	    ಯದಿ (idx <= 1)
		ಪ್ರತ್ಯಾಗಮ (char_u *)opts[idx + 1];
	}
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    ಸ್ಥಿರ char_u *
get_scriptnames_arg(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    scriptitem_T *si;

    ಯದಿ (!SCRIPT_ID_VALID(idx + 1))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(ರಿಕ್ತ, si->sn_name, NameBuff, MAXPATHL, TRUE);
    ಪ್ರತ್ಯಾಗಮ NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    ಸ್ಥಿರ char_u *
get_retab_arg(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx == 0)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"-indentonly";
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    ಸ್ಥಿರ char_u *
get_messages_arg(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx == 0)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"clear";
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ char_u *
get_mapclear_arg(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx == 0)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"<buffer>";
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches)
{
    ಸ್ಥಿರ ಸಂರಚನಾ expgen
    {
	ಪೂರ್ಣಾಙ್ಕ		context;
	char_u	*((*func)(expand_T *, ಪೂರ್ಣಾಙ್ಕ));
	ಪೂರ್ಣಾಙ್ಕ		ic;
	ಪೂರ್ಣಾಙ್ಕ		escaped;
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
#ifdef FEAT_EVAL
	{EXPAND_USER_VARS, get_user_var_name, FALSE, TRUE},
	{EXPAND_FUNCTIONS, get_function_name, FALSE, TRUE},
	{EXPAND_USER_FUNC, get_user_func_name, FALSE, TRUE},
	{EXPAND_DISASSEMBLE, get_disassemble_argument, FALSE, TRUE},
	{EXPAND_EXPRESSION, get_ಅಭಿವ್ಯಕ್ತಿ_name, FALSE, TRUE},
#endif
#ifdef FEAT_MENU
	{EXPAND_MENUS, get_menu_name, FALSE, TRUE},
	{EXPAND_MENUNAMES, get_menu_names, FALSE, TRUE},
#endif
#ifdef FEAT_SYN_HL
	{EXPAND_SYNTAX, get_syntax_name, TRUE, TRUE},
#endif
#ifdef FEAT_PROFILE
	{EXPAND_SYNTIME, get_syntime_arg, TRUE, TRUE},
#endif
	{EXPAND_HIGHLIGHT, get_highlight_name, TRUE, TRUE},
	{EXPAND_EVENTS, get_event_name, TRUE, FALSE},
	{EXPAND_AUGROUP, get_augroup_name, TRUE, FALSE},
#ifdef FEAT_CSCOPE
	{EXPAND_CSCOPE, get_cscope_name, TRUE, TRUE},
#endif
#ifdef FEAT_SIGNS
	{EXPAND_SIGN, get_sign_name, TRUE, TRUE},
#endif
#ifdef FEAT_PROFILE
	{EXPAND_PROFILE, get_profile_name, TRUE, TRUE},
#endif
#ಯದಿ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	{EXPAND_LANGUAGE, get_lang_arg, TRUE, FALSE},
	{EXPAND_LOCALES, get_locales, TRUE, FALSE},
#endif
	{EXPAND_ENV_VARS, get_env_name, TRUE, TRUE},
	{EXPAND_USER, get_users, TRUE, FALSE},
	{EXPAND_ARGLIST, get_arglist_name, TRUE, FALSE},
#ifdef FEAT_EVAL
	{EXPAND_BREAKPOINT, get_breakadd_arg, TRUE, TRUE},
	{EXPAND_SCRIPTNAMES, get_scriptnames_arg, TRUE, FALSE},
#endif
	{EXPAND_RETAB, get_retab_arg, TRUE, TRUE},
    };
    ಪೂರ್ಣಾಙ್ಕ	i;
    ಪೂರ್ಣಾಙ್ಕ ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(tab); ++i)
    {
	ಯದಿ (xp->xp_context == tab[i].context)
	{
	    ಯದಿ (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, ಅಂಕೆ_Matches,
						tab[i].func, tab[i].escaped);
	    ಭಙ್ಗ;
	}
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
map_wildopts_to_ewflags(ಪೂರ್ಣಾಙ್ಕ options)
{
    ಪೂರ್ಣಾಙ್ಕ		flags;

    flags = EW_DIR;	// include directories
    ಯದಿ (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    ಯದಿ (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    ಯದಿ (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    ಯದಿ (options & WILD_SILENT)
	flags |= EW_SILENT;
    ಯದಿ (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    ಯದಿ (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    ಪ್ರತ್ಯಾಗಮ flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches,
    ಪೂರ್ಣಾಙ್ಕ		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    ಪೂರ್ಣಾಙ್ಕ		flags;
    char_u	*tofree = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    ಯದಿ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	ಪ್ರತ್ಯಾಗಮ expand_files_and_dirs(xp, pat, matches, ಅಂಕೆ_Matches, flags,
								options);

    *matches = (char_u **)"";
    *ಅಂಕೆ_Matches = 0;
    ಯದಿ (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	ಯದಿ (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					ಅಂಕೆ_Matches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*ಅಂಕೆ_Matches, *matches);
#endif
	    ಪ್ರತ್ಯಾಗಮ OK;
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (xp->xp_context == EXPAND_SHELLCMD)
	ಪ್ರತ್ಯಾಗಮ expand_shellcmd(pat, matches, ಅಂಕೆ_Matches, flags);
    ಯದಿ (xp->xp_context == EXPAND_OLD_SETTING)
	ಪ್ರತ್ಯಾಗಮ ExpandOldSetting(ಅಂಕೆ_Matches, matches);
    ಯದಿ (xp->xp_context == EXPAND_BUFFERS)
	ಪ್ರತ್ಯಾಗಮ ExpandBufnames(pat, ಅಂಕೆ_Matches, matches, options);
#ifdef FEAT_DIFF
    ಯದಿ (xp->xp_context == EXPAND_DIFF_BUFFERS)
	ಪ್ರತ್ಯಾಗಮ ExpandBufnames(pat, ಅಂಕೆ_Matches, matches,
						options | BUF_DIFF_FILTER);
#endif
    ಯದಿ (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	ಪ್ರತ್ಯಾಗಮ expand_tags(xp->xp_context == EXPAND_TAGS, pat, ಅಂಕೆ_Matches,
								matches);
    ಯದಿ (xp->xp_context == EXPAND_COLORS)
    {
	ವರ್ಣ *directories[] = {"colors", ರಿಕ್ತ};
	ಪ್ರತ್ಯಾಗಮ ExpandRTDir(pat, DIP_START + DIP_OPT, ಅಂಕೆ_Matches, matches,
								directories);
    }
    ಯದಿ (xp->xp_context == EXPAND_COMPILER)
    {
	ವರ್ಣ *directories[] = {"compiler", ರಿಕ್ತ};
	ಪ್ರತ್ಯಾಗಮ ExpandRTDir(pat, 0, ಅಂಕೆ_Matches, matches, directories);
    }
    ಯದಿ (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	ವರ್ಣ *directories[] = {"syntax", ರಿಕ್ತ};
	ಪ್ರತ್ಯಾಗಮ ExpandRTDir(pat, 0, ಅಂಕೆ_Matches, matches, directories);
    }
    ಯದಿ (xp->xp_context == EXPAND_FILETYPE)
    {
	ವರ್ಣ *directories[] = {"syntax", "indent", "ftplugin", ರಿಕ್ತ};
	ಪ್ರತ್ಯಾಗಮ ExpandRTDir(pat, 0, ಅಂಕೆ_Matches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    ಯದಿ (xp->xp_context == EXPAND_KEYMAP)
    {
	ವರ್ಣ *directories[] = {"keymap", ರಿಕ್ತ};
	ಪ್ರತ್ಯಾಗಮ ExpandRTDir(pat, 0, ಅಂಕೆ_Matches, matches, directories);
    }
#endif
#ಯದಿ defined(FEAT_EVAL)
    ಯದಿ (xp->xp_context == EXPAND_USER_LIST)
	ಪ್ರತ್ಯಾಗಮ ExpandUserList(xp, matches, ಅಂಕೆ_Matches);
#endif
    ಯದಿ (xp->xp_context == EXPAND_PACKADD)
	ಪ್ರತ್ಯಾಗಮ ExpandPackAddDir(pat, ಅಂಕೆ_Matches, matches);
    ಯದಿ (xp->xp_context == EXPAND_RUNTIME)
	ಪ್ರತ್ಯಾಗಮ expand_runtime_cmd(pat, ಅಂಕೆ_Matches, matches);
    ಯದಿ (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	ಪ್ರತ್ಯಾಗಮ expand_pattern_in_buf(pat, xp->xp_ಹುಡುಕು_dir,
		matches, ಅಂಕೆ_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    ಯದಿ ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	ಪೂರ್ಣಾಙ್ಕ len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(pat) + 20;

	tofree = alloc(len);
	ಯದಿ (tofree == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	vim_snprintf((ವರ್ಣ *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    ಯದಿ (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
	{
	    vim_free(tofree);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    ಯದಿ (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, ಅಂಕೆ_Matches, matches, fuzzy);
    ಅನ್ಯಥಾ ಯದಿ (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, ಅಂಕೆ_Matches, matches);
    ಅನ್ಯಥಾ ಯದಿ (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, ಅಂಕೆ_Matches, matches);
    ಅನ್ಯಥಾ ಯದಿ (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, ಅಂಕೆ_Matches, matches);
    ಅನ್ಯಥಾ ಯದಿ (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, ಅಂಕೆ_Matches);
    ಅನ್ಯಥಾ ಯದಿ (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, ಅಂಕೆ_Matches);
#ಯದಿ defined(FEAT_TERMINAL)
    ಅನ್ಯಥಾ ಯದಿ (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, ಅಂಕೆ_Matches);
#endif
#ಯದಿ defined(FEAT_EVAL)
    ಅನ್ಯಥಾ ಯದಿ (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, ಅಂಕೆ_Matches);
#endif
    ಅನ್ಯಥಾ
	ret = ExpandOther(pat, xp, &regmatch, matches, ಅಂಕೆ_Matches);

    ಯದಿ (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಪೂರ್ಣಾಙ್ಕ
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches,
    char_u	*(*func)(expand_T *, ಪೂರ್ಣಾಙ್ಕ),
					  // returns a string from the list
    ಪೂರ್ಣಾಙ್ಕ		escaped)
{
    ಪ್ರತ್ಯಾಗಮ ExpandGenericExt(
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
    ಪೂರ್ಣಾಙ್ಕ
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches,
    char_u	*(*func)(expand_T *, ಪೂರ್ಣಾಙ್ಕ),
					  // returns a string from the list
    ಪೂರ್ಣಾಙ್ಕ		escaped,
    ಪೂರ್ಣಾಙ್ಕ		sortStartIdx)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		score = 0;
    ಪೂರ್ಣಾಙ್ಕ		fuzzy;
    ಪೂರ್ಣಾಙ್ಕ		match;
    ಪೂರ್ಣಾಙ್ಕ		sort_matches = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		funcsort = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ರಿಕ್ತ;
    *ಅಂಕೆ_Matches = 0;

    ಯದಿ (!fuzzy)
	ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ *), 30);
    ಅನ್ಯಥಾ
	ga_init2(&ga, ಪರಿಮಾಣ(fuzmatch_str_T), 30);

    ಪ್ರತಿ (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	ಯದಿ (str == ರಿಕ್ತ)	    // end of list
	    ಭಙ್ಗ;
	ಯದಿ (*str == NUL)	    // skip empty strings
	    ಅನುವರ್ತ;

	ಯದಿ (xp->xp_pattern[0] != NUL)
	{
	    ಯದಿ (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    ಅನ್ಯಥಾ
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	ಅನ್ಯಥಾ
	    match = TRUE;

	ಯದಿ (!match)
	    ಅನುವರ್ತ;

	ಯದಿ (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	ಅನ್ಯಥಾ
	    str = vim_strsave(str);
	ಯದಿ (str == ರಿಕ್ತ)
	{
	    ಯದಿ (!fuzzy)
	    {
		ga_clear_strings(&ga);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    ಭಙ್ಗ;
	}

	ಯದಿ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	ಅನ್ಯಥಾ
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

#ifdef FEAT_MENU
	ಯದಿ (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    ಯದಿ (*str == '\001')
		*str = '.';
	}
#endif

	ಯದಿ (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    ಯದಿ (ga.ga_len == 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    ಯದಿ (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    ಯದಿ (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    ಯದಿ (sort_matches && sortStartMatchIdx != -1)
    {
	ಯದಿ (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((ಶೂನ್ಯ *)ga.ga_data, (size_t)ga.ga_len, ಪರಿಮಾಣ(char_u *),
							   sort_func_compare);
	ಅನ್ಯಥಾ
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    ಯದಿ (!fuzzy)
    {
	*matches = ga.ga_data;
	*ಅಂಕೆ_Matches = ga.ga_len;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	*ಅಂಕೆ_Matches = ga.ga_len;
    }

#ಯದಿ defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
expand_shellcmd_onedir(
	char_u		*pathed_pattern,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches,
	ಪೂರ್ಣಾಙ್ಕ		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    ಯದಿ (expand_wildcards(1, &pathed_pattern, ಅಂಕೆ_Matches, matches, flags) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (ga_grow(gap, *ಅಂಕೆ_Matches) == FAIL)
    {
	FreeWild(*ಅಂಕೆ_Matches, *matches);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < *ಅಂಕೆ_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	ಯದಿ (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    ಯದಿ (HASHITEM_EMPTY(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_data)[gap->ga_len++] = name;
		hash_add_item(ht, hi, name, hash);
		name = ರಿಕ್ತ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches,	// return: number of matches
    ಪೂರ್ಣಾಙ್ಕ		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    ಪೂರ್ಣಾಙ್ಕ		flags = flagsarg;
    ಪೂರ್ಣಾಙ್ಕ		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    ಯದಿ (pat == ರಿಕ್ತ)
    {
	vim_free(buf);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    ಪ್ರತಿ (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	ಯದಿ (*s != '\\')
	    ಅನುವರ್ತ;

	p = s + 1;
	ಯದಿ (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    ಯದಿ (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    ಅನ್ಯಥಾ
    {
	// For an absolute name we don't use $PATH.
	ಯದಿ (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	ಯದಿ (path == ರಿಕ್ತ)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ *), 10);
    hash_init(&found_ht);
    ಪ್ರತಿ (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	ಯದಿ (*s == NUL)
	{
	    ಯದಿ (did_curdir)
		ಭಙ್ಗ;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	ಅನ್ಯಥಾ
	{
#ಯದಿ defined(MSWIN)
	    e = vim_strchr(s, ';');
#ಅನ್ಯಥಾ
	    e = vim_strchr(s, ':');
#endif
	    ಯದಿ (e == ರಿಕ್ತ)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    ಯದಿ (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    ಅನ್ಯಥಾ
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? ಪರಿಮಾಣ(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	ಯದಿ (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    ಯದಿ (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		ಯದಿ (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, ಅಂಕೆ_Matches, flags,
				    &found_ht, &ga);
	}

	ಯದಿ (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_data;
    *ಅಂಕೆ_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    ಯದಿ (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ *
call_user_expand_func(
    ಶೂನ್ಯ	*(*user_expand_func)(char_u *, ಪೂರ್ಣಾಙ್ಕ, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ಪೂರ್ಣಾಙ್ಕ		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = ರಿಕ್ತ;
    ಶೂನ್ಯ	*ret;

    ಯದಿ (xp->xp_arg == ರಿಕ್ತ || xp->xp_arg[0] == '\0' || xp->xp_ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (ccline->cmdbuff != ರಿಕ್ತ)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	ಪೂರ್ಣಾಙ್ಕ unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	ಯದಿ (pat == ರಿಕ್ತ)
	    pat = vim_strnsave(xp->xp_pattern, xp->xp_pattern_len);
    }

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = xp->xp_ಸಾಲು;
    args[2].v_type = VAR_ಸಂಖ್ಯೆ;
    args[2].vval.v_ಸಂಖ್ಯೆ = xp->xp_col;
    args[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, args);

    current_sctx = save_current_sctx;
    ಯದಿ (ccline->cmdbuff != ರಿಕ್ತ)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    ಪೂರ್ಣಾಙ್ಕ		keep;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		fuzzy;
    ಪೂರ್ಣಾಙ್ಕ		match;
    ಪೂರ್ಣಾಙ್ಕ		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ರಿಕ್ತ;
    *ಅಂಕೆ_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    ಯದಿ (retstr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (!fuzzy)
	ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ *), 3);
    ಅನ್ಯಥಾ
	ga_init2(&ga, ಪರಿಮಾಣ(fuzmatch_str_T), 3);

    ಪ್ರತಿ (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	ಯದಿ (e == ರಿಕ್ತ)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	ಯದಿ (xp->xp_pattern[0] != NUL)
	{
	    ಯದಿ (!fuzzy)
		match = vim_regexec(regmatch, s, (colnr_T)0);
	    ಅನ್ಯಥಾ
	    {
		score = fuzzy_match_str(s, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	ಅನ್ಯಥಾ
	    match = TRUE;		// match everything

	*e = keep;

	ಯದಿ (match)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    ಯದಿ (p == ರಿಕ್ತ)
		ಭಙ್ಗ;

	    ಯದಿ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		ಭಙ್ಗ;
	    }

	    ಯದಿ (!fuzzy)
		((char_u **)ga.ga_data)[ga.ga_len] = p;
	    ಅನ್ಯಥಾ
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	ಯದಿ (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    ಯದಿ (ga.ga_len == 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (!fuzzy)
    {
	*matches = ga.ga_data;
	*ಅಂಕೆ_Matches = ga.ga_len;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	*ಅಂಕೆ_Matches = ga.ga_len;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    ಪೂರ್ಣಾಙ್ಕ		have_extra = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ *), 3);
    ga_init2(&ga_abbr, ಪರಿಮಾಣ(ವರ್ಣ *), 3);
    ga_init2(&ga_kind, ಪರಿಮಾಣ(ವರ್ಣ *), 3);
    ga_init2(&ga_menu, ಪರಿಮಾಣ(ವರ್ಣ *), 3);
    ga_init2(&ga_info, ಪರಿಮಾಣ(ವರ್ಣ *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_ವಸ್ತುಗಳು(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = ರಿಕ್ತ;
	char_u	*abbr = ರಿಕ್ತ;
	char_u	*kind = ರಿಕ್ತ;
	char_u	*menu = ರಿಕ್ತ;
	char_u	*info = ರಿಕ್ತ;

	ಯದಿ (tv->v_type == VAR_STRING)
	{
	    ಯದಿ (tv->vval.v_string == ರಿಕ್ತ)
		ಅನುವರ್ತ;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_DICT && tv->vval.v_dict != ರಿಕ್ತ)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    ಯದಿ (word == ರಿಕ್ತ)
		ಅನುವರ್ತ;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    ಯದಿ (abbr != ರಿಕ್ತ || kind != ರಿಕ್ತ || menu != ರಿಕ್ತ || info != ರಿಕ್ತ)
		have_extra = TRUE;
	}
	ಅನ್ಯಥಾ
	    ಅನುವರ್ತ;  // Skip other types

	ಯದಿ (p == ರಿಕ್ತ
		|| ga_grow(&ga, 1) == FAIL
		|| ga_grow(&ga_abbr, 1) == FAIL
		|| ga_grow(&ga_kind, 1) == FAIL
		|| ga_grow(&ga_menu, 1) == FAIL
		|| ga_grow(&ga_info, 1) == FAIL)
	{
	    vim_free(p);
	    vim_free(abbr);
	    vim_free(kind);
	    vim_free(menu);
	    vim_free(info);
	    ಭಙ್ಗ;
	}

	((char_u **)ga.ga_data)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_data)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_data)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_data)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_data)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_data;
    *ಅಂಕೆ_Matches = ga.ga_len;
    ಯದಿ (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    ಅನ್ಯಥಾ
    {
	// No extra info collected; free the placeholder NULL entries.
	ಪ್ರತಿ (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	ಪ್ರತಿ (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	ಪ್ರತಿ (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	ಪ್ರತಿ (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches)
{
    list_T      *retlist;

    *matches = ರಿಕ್ತ;
    *ಅಂಕೆ_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    ಯದಿ (retlist == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    expand_process_user_list(retlist, matches, ಅಂಕೆ_Matches, xp);
    list_unref(retlist);
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    ಶೂನ್ಯ
globpath(
    char_u	*path,
    char_u	*file,
    garray_T	*ga,
    ಪೂರ್ಣಾಙ್ಕ		expand_options,
    ಪೂರ್ಣಾಙ್ಕ		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#ಯದಿ defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#ಅನ್ಯಥಾ
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    ಯಾವತ್ (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	ಯದಿ (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    ಅಂಕೆ_p;
	    char_u  **p;

	    ಯದಿ (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    ಯದಿ (ExpandFromContext(&xpc, buf, &p, &ಅಂಕೆ_p,
			     WILD_SILENT|expand_options) != FAIL && ಅಂಕೆ_p > 0)
	    {
		ExpandEscape(&xpc, buf, ಅಂಕೆ_p, p, WILD_SILENT|expand_options);

		ಯದಿ (ga_grow(ga, ಅಂಕೆ_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_p; ++i)
		    {
			((char_u **)ga->ga_data)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    FreeWild(ಅಂಕೆ_p, p);
		    p = ರಿಕ್ತ;
		}
		vim_free(p);
	    }
	}
    }

    vim_free(buf);
}
#undef TMP_PATHSEPSTR

/*
 * Translate some keys pressed when 'wildmenu' is used.
 */
    ಪೂರ್ಣಾಙ್ಕ
wildmenu_translate_ಕೀಲಿ(
	cmdline_info_T	*cclp,
	ಪೂರ್ಣಾಙ್ಕ		ಕೀಲಿ,
	expand_T	*xp,
	ಪೂರ್ಣಾಙ್ಕ		did_wild_list)
{
    ಪೂರ್ಣಾಙ್ಕ c = ಕೀಲಿ;

    ಯದಿ (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	ವಿಕಲ್ಪ (c)
	{
	    ಪಕ್ಷ K_UP:	  c = K_ಎಡ; ಭಙ್ಗ;
	    ಪಕ್ಷ K_DOWN:  c = K_ಬಲ; ಭಙ್ಗ;
	    ಪಕ್ಷ K_ಎಡ:  c = K_UP; ಭಙ್ಗ;
	    ಪಕ್ಷ K_ಬಲ: c = K_DOWN; ಭಙ್ಗ;
	    ಸ್ವಭಾವ:	  ಭಙ್ಗ;
	}
    }

    ಯದಿ (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	ಯದಿ (c == K_ಎಡ)
	    c = Ctrl_P;
	ಅನ್ಯಥಾ ಯದಿ (c == K_ಬಲ)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    ಯದಿ (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    ಪ್ರತ್ಯಾಗಮ c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cmdline_del(cmdline_info_T *cclp, ಪೂರ್ಣಾಙ್ಕ from)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
wildmenu_process_ಕೀಲಿ_menunames(cmdline_info_T *cclp, ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ, expand_T *xp)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    ಯದಿ (ಕೀಲಿ == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	ಕೀಲಿ = p_wc;
	ಕೀಲಿ_Typed = TRUE;  // in case the key was mapped
    }
    ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	ಪೂರ್ಣಾಙ್ಕ found = FALSE;

	j = (ಪೂರ್ಣಾಙ್ಕ)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	ಯಾವತ್ (--j > 0)
	{
	    // check for start of menu name
	    ಯದಿ (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		ಭಙ್ಗ;
	    }
	    // check for start of submenu name
	    ಯದಿ (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		ಯದಿ (found)
		{
		    i = j + 1;
		    ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ
		    found = TRUE;
	    }
	}
	ಯದಿ (i > 0)
	    cmdline_del(cclp, i);
	ಕೀಲಿ = p_wc;
	ಕೀಲಿ_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
wildmenu_process_ಕೀಲಿ_filenames(cmdline_info_T *cclp, ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ, expand_T *xp)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    ಯದಿ (ಕೀಲಿ == K_DOWN
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
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && ಕೀಲಿ == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	ಪೂರ್ಣಾಙ್ಕ found = FALSE;

	j = cclp->cmdpos;
	i = (ಪೂರ್ಣಾಙ್ಕ)(xp->xp_pattern - cclp->cmdbuff);
	ಯಾವತ್ (--j > i)
	{
	    ಯದಿ (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    ಯದಿ (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    ಕೀಲಿ = p_wc;
	    ಕೀಲಿ_Typed = TRUE;  // in case the key was mapped
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ == K_UP)
    {
	// go up a directory
	ಪೂರ್ಣಾಙ್ಕ found = FALSE;

	j = cclp->cmdpos - 1;
	i = (ಪೂರ್ಣಾಙ್ಕ)(xp->xp_pattern - cclp->cmdbuff);
	ಯಾವತ್ (--j > i)
	{
	    ಯದಿ (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    ಯದಿ (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == ರಿಕ್ತ
#endif
	       )
	    {
		ಯದಿ (found)
		{
		    i = j + 1;
		    ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ
		    found = TRUE;
	    }
	}

	ಯದಿ (!found)
	    j = i;
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	ಅನ್ಯಥಾ
	    j = 0;
	ಯದಿ (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	ಅನ್ಯಥಾ ಯದಿ (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	ಕೀಲಿ = p_wc;
	ಕೀಲಿ_Typed = TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    ಪೂರ್ಣಾಙ್ಕ
wildmenu_process_ಕೀಲಿ(cmdline_info_T *cclp, ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ, expand_T *xp)
{
    ಯದಿ (xp->xp_context == EXPAND_MENUNAMES)
	ಪ್ರತ್ಯಾಗಮ wildmenu_process_ಕೀಲಿ_menunames(cclp, ಕೀಲಿ, xp);
    ಅನ್ಯಥಾ ಯದಿ ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	ಪ್ರತ್ಯಾಗಮ wildmenu_process_ಕೀಲಿ_filenames(cclp, ಕೀಲಿ, xp);

    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
}

/*
 * Free expanded names when finished walking through the matches
 */
    ಶೂನ್ಯ
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ skt = ಕೀಲಿ_Typed;

    ಯದಿ (!p_wmnu || wild_menu_showing == 0)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ save_RedrawingDisabled = RedrawingDisabled;
    ಯದಿ (cclp->ಒಳಹರಿವು_fn)
	RedrawingDisabled = 0;
#endif

#ಯದಿ defined(FEAT_ಹುಡುಕು_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    ಯದಿ (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    ಅನ್ಯಥಾ ಯದಿ (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	last_status(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    ಅನ್ಯಥಾ
    {
	win_redraw_last_status(topframe);
	redraw_statuslines();
    }
    ಕೀಲಿ_Typed = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    ಯದಿ (cclp->ಒಳಹರಿವು_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    ಶೂನ್ಯ
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    ಪೂರ್ಣಾಙ್ಕ		filtered = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    pat = tv_get_string(&argvars[0]);
    ಯದಿ (check_for_string_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    type = tv_get_string(&argvars[1]);

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], ರಿಕ್ತ);

    ಯದಿ (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    ಯದಿ (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    ಯದಿ (STRCMP(type, "cmdline") == 0)
    {
	ಪೂರ್ಣಾಙ್ಕ cmdline_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    ಅನ್ಯಥಾ
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(xpc.xp_pattern);
	xpc.xp_ಸಾಲು = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	ವಿಕಲ್ಪ (xpc.xp_context)
	{
	ಪಕ್ಷ EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    ಪ್ರತ್ಯಾಗಮ;

	ಪಕ್ಷ EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    ಯದಿ (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		ಪ್ರತ್ಯಾಗಮ;
	    }

	    xpc.xp_arg = type + 7;
	    ಭಙ್ಗ;

	ಪಕ್ಷ EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    ಯದಿ (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		ಪ್ರತ್ಯಾಗಮ;
	    }

	    xpc.xp_arg = type + 11;
	    ಭಙ್ಗ;

# ಯದಿ defined(FEAT_MENU)
	ಪಕ್ಷ EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (ಪೂರ್ಣಾಙ್ಕ)(xpc.xp_pattern - pattern_start);
	    ಭಙ್ಗ;
# endif

# ifdef FEAT_CSCOPE
	ಪಕ್ಷ EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (ಪೂರ್ಣಾಙ್ಕ)(xpc.xp_pattern - pattern_start);
	    ಭಙ್ಗ;
# endif

# ifdef FEAT_SIGNS
	ಪಕ್ಷ EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (ಪೂರ್ಣಾಙ್ಕ)(xpc.xp_pattern - pattern_start);
	    ಭಙ್ಗ;
# endif

	ಪಕ್ಷ EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (ಪೂರ್ಣಾಙ್ಕ)(xpc.xp_pattern - pattern_start);
	    ಭಙ್ಗ;

	ಪಕ್ಷ EXPAND_SHELLCMDLINE:
	{
	    ಪೂರ್ಣಾಙ್ಕ context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(ರಿಕ್ತ, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (ಪೂರ್ಣಾಙ್ಕ)(xpc.xp_pattern - pattern_start);
	    ಭಙ್ಗ;
	}

	ಪಕ್ಷ EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    ಭಙ್ಗ;

	ಪಕ್ಷ EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	ಪೂರ್ಣಾಙ್ಕ	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	ಯದಿ (unesc != ರಿಕ್ತ)
	    pat_src = unesc;

	ಯದಿ (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	ಅನ್ಯಥಾ
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    ಯದಿ (rettv_list_alloc(rettv) == OK && pat != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	i;

	ExpandOne(&xpc, pat, ರಿಕ್ತ, options, WILD_ALL_KEEP);

	ಪ್ರತಿ (i = 0; i < xpc.xp_numfiles; i++)
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
    ಪೂರ್ಣಾಙ್ಕ		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(pat);
    set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_to_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    ಶೂನ್ಯ
f_cmdcomplete_info(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    ಪೂರ್ಣಾಙ್ಕ		    idx;
    ಪೂರ್ಣಾಙ್ಕ		    ret = OK;

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL || ccline == ರಿಕ್ತ
	    || ccline->xpc == ರಿಕ್ತ || ccline->xpc->xp_files == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (ಪೂರ್ಣಾಙ್ಕ)cmdline_orig.length);
    ಯದಿ (ret == OK)
	ret = dict_add_ಸಂಖ್ಯೆ(retdict, "pum_visible", pum_visible());
    ಯದಿ (ret == OK)
	ret = dict_add_ಸಂಖ್ಯೆ(retdict, "selected", ccline->xpc->xp_selected);
    ಯದಿ (ret == OK)
    {
	li = list_alloc();
	ಯದಿ (li == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	ret = dict_add_list(retdict, "matches", li);
	ಯದಿ (ret == FAIL)
	    list_unref(li);
	ಪ್ರತಿ (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
	    list_append_string(li, ccline->xpc->xp_files[idx], -1);
    }
}
#endif // FEAT_EVAL

/*
 * Copy a substring from the current buffer (curbuf), spanning from the given
 * 'start' position to the word boundary after 'end' position.
 * The copied string is stored in '*match', and the actual end position of the
 * matched text is returned in '*match_end'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*ಸಾಲು, *start_ಸಾಲು, *end_ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ರಿಕ್ತ;

    ಯದಿ (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	ಪ್ರತ್ಯಾಗಮ FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_ಸಾಲು = ml_get(start->lnum);
    char_u  *start_ptr = start_ಸಾಲು + start->col;
    ಪೂರ್ಣಾಙ್ಕ	    is_single_ಸಾಲು = start->lnum == end->lnum;

    segment_len = is_single_ಸಾಲು ? (end->col - start->col)
			: (ಪೂರ್ಣಾಙ್ಕ)(ml_get_len(start->lnum) - start->col);
    ಯದಿ (ga_grow(&ga, segment_len + 2) != OK)
	ಗಚ್ಛ fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    ಯದಿ (!is_single_ಸಾಲು)
    {
	ಯದಿ (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	ಅನ್ಯಥಾ
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    ಯದಿ (!is_single_ಸಾಲು)
    {
	ಪ್ರತಿ (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    ಪೂರ್ಣಾಙ್ಕ  linelen;

	    ಸಾಲು = ml_get(lnum);
	    linelen = (ಪೂರ್ಣಾಙ್ಕ)ml_get_len(lnum);
	    ಯದಿ (ga_grow(&ga, linelen + 2) != OK)
		ಗಚ್ಛ fail;
	    ga_concat_len(&ga, ಸಾಲು, linelen);
	    ಯದಿ (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    ಅನ್ಯಥಾ
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_ಸಾಲು = ml_get(end->lnum);
    word_end = find_word_end(end_ಸಾಲು + end->col);
    segment_len = (ಪೂರ್ಣಾಙ್ಕ)(word_end - end_ಸಾಲು);
    ಯದಿ (ga_grow(&ga, segment_len) != OK)
	ಗಚ್ಛ fail;
    ga_concat_len(&ga, end_ಸಾಲು + (is_single_ಸಾಲು ? end->col : 0),
	    segment_len - (is_single_ಸಾಲು ? end->col : 0));

    // Null-terminate
    ಯದಿ (ga_grow(&ga, 1) != OK)
	ಗಚ್ಛ fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    ಪ್ರತ್ಯಾಗಮ OK;

fail:
    ga_clear(&ga);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_regex_match(char_u *pat, char_u *str)
{
    ಯದಿ (STRCMP(pat, str) == 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    regmatch_T	regmatch;
    ಪೂರ್ಣಾಙ್ಕ		result;

    ++emsg_off;
    ++ಸಂದೇಶ_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --ಸಂದೇಶ_silent;

    ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    regmatch.rm_ic = p_ic;
    ಯದಿ (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++ಸಂದೇಶ_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --ಸಂದೇಶ_silent;

    vim_regfree(regmatch.regprog);
    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    ಸ್ಥಿರ char_u *
concat_pattern_with_buffer_match(
	char_u *pat,
	ಪೂರ್ಣಾಙ್ಕ pat_len,
	pos_T *end_match_pos,
	ಪೂರ್ಣಾಙ್ಕ lowercase UNUSED)
{
    char_u  *ಸಾಲು = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(ಸಾಲು + end_match_pos->col);
    ಪೂರ್ಣಾಙ್ಕ	    match_len = (ಪೂರ್ಣಾಙ್ಕ)(word_end - (ಸಾಲು + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    ಯದಿ (match == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    mch_memmove(match, pat, pat_len);
    ಯದಿ (match_len > 0)
    {
#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_SPELL)
	ಯದಿ (lowercase)
	{
	    char_u  *mword = vim_strnsave(ಸಾಲು + end_match_pos->col,
		    match_len);
	    ಯದಿ (mword == ರಿಕ್ತ)
		ಗಚ್ಛ cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    ಯದಿ (lower == ರಿಕ್ತ)
		ಗಚ್ಛ cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	ಅನ್ಯಥಾ
#endif
	    mch_memmove(match + pat_len, ಸಾಲು + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    ಪ್ರತ್ಯಾಗಮ match;

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    ಪೂರ್ಣಾಙ್ಕ		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		found_new_match;
    ಪೂರ್ಣಾಙ್ಕ		looped_around = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		pat_len;
    ಪೂರ್ಣಾಙ್ಕ		has_range = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		compl_started = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		ಹುಡುಕು_flags;
    char_u	*match, *full_match;
    ಪೂರ್ಣಾಙ್ಕ		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ರಿಕ್ತ;

#ifdef FEAT_ಹುಡುಕು_EXTRA
    has_range = ಹುಡುಕು_first_ಸಾಲು != 0;
#endif

    *matches = ರಿಕ್ತ;
    *ಅಂಕೆ_Matches = 0;

    ಯದಿ (pat == ರಿಕ್ತ || *pat == NUL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    pat_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_ಹುಡುಕು_EXTRA
    ಯದಿ (has_range)
	cur_match_pos.lnum = ಹುಡುಕು_first_ಸಾಲು;
    ಅನ್ಯಥಾ
#endif
	cur_match_pos = pre_incsearch_pos;

    ಹುಡುಕು_flags = ಹುಡುಕು_OPT | ಹುಡುಕು_NOOF | ಹುಡುಕು_PEEK | ಹುಡುಕು_NFMSG
	| (has_range ? ಹುಡುಕು_START : 0);

    ga_init2(&ga, ಪರಿಮಾಣ(char_u *), 10); // Use growable array of char_u*

    ಪ್ರತಿ (;;)
    {
	++emsg_off;
	++ಸಂದೇಶ_silent;
	found_new_match = searchit(ರಿಕ್ತ, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		ಹುಡುಕು_flags, RE_LAST, ರಿಕ್ತ);
	--ಸಂದೇಶ_silent;
	--emsg_off;

	ಯದಿ (found_new_match == FAIL)
	    ಭಙ್ಗ;

#ifdef FEAT_ಹುಡುಕು_EXTRA
	// If in range mode, check if match is within the range
	ಯದಿ (has_range && (cur_match_pos.lnum < ಹುಡುಕು_first_ಸಾಲು
		    || cur_match_pos.lnum > ಹುಡುಕು_last_ಸಾಲು))
		ಭಙ್ಗ;
#endif

	ಯದಿ (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    ಯದಿ ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		ಯದಿ (looped_around)
		    ಭಙ್ಗ;
		ಅನ್ಯಥಾ
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	ಯದಿ (char_avail() || got_int)
	{
	    ಯದಿ (got_int)
	    {
		(ಶೂನ್ಯ)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    ಗಚ್ಛ cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	ಯದಿ (end_match_pos.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    ಅನುವರ್ತ;
	}

	// Extract the matching text prepended to completed word
	ಯದಿ (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    ಭಙ್ಗ;

	ಯದಿ (exacttext)
	    match = full_match;
	ಅನ್ಯಥಾ
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    ಯದಿ (match == ರಿಕ್ತ || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		ಯದಿ (match == ರಿಕ್ತ || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    ಅನುವರ್ತ;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ga.ga_len; ++i)
	{
	    ಯದಿ (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (match != ರಿಕ್ತ)
	{
	    ಯದಿ (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		ಗಚ್ಛ cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    ಯದಿ (ga.ga_len > TAG_MANY)
		ಭಙ್ಗ;
	}
	ಯದಿ (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *ಅಂಕೆ_Matches = ga.ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;

cleanup:
    ga_clear_strings(&ga);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}
