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

#అన్తర్భావ "vim.h"

స్థిర పూర్ణాంకం	cmd_showtail;	// Only show path tail in lists ?
స్థిర పూర్ణాంకం	may_expand_నమూనా = FALSE;
స్థిర pos_T	pre_incsearch_pos; // Cursor position when incsearch started

స్థిర శూన్య	సమితి_context_for_wildcard_arg(exarg_T *eap, char_u *arg, పూర్ణాంకం usefilter, expand_T *xp, పూర్ణాంకం *complp);
స్థిర పూర్ణాంకం	ExpandFromContext(expand_T *xp, char_u *, char_u ***, పూర్ణాంకం *, పూర్ణాంకం);
స్థిర char_u	*showmatches_gettail(char_u *s);
స్థిర పూర్ణాంకం	expand_showtail(expand_T *xp);
స్థిర పూర్ణాంకం	expand_shellcmd(char_u *filepat, char_u ***matches, పూర్ణాంకం *అంకె_Matches, పూర్ణాంకం flagsarg);
#ఒకవేళ defined(FEAT_EVAL)
స్థిర పూర్ణాంకం	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, పూర్ణాంకం *అంకె_Matches);
స్థిర పూర్ణాంకం	ExpandUserList(expand_T *xp, char_u ***matches, పూర్ణాంకం *అంకె_Matches);
స్థిర char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
స్థిర char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, పూర్ణాంకం srclen, పూర్ణాంకం *new_lenp);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం	expand_నమూనా_లోపల_buf(char_u *pat, పూర్ణాంకం dir, char_u ***matches, పూర్ణాంకం *అంకె_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
స్థిర pumitem_T *compl_సరిపోలిక_array = ఖాళీ;
స్థిర పూర్ణాంకం compl_సరిపోలిక_arraysize;
// First column in cmdline of the matched item for completion.
స్థిర పూర్ణాంకం compl_startcol;
స్థిర పూర్ణాంకం compl_selected;
// cmdline before expansion
స్థిర string_T cmdline_orig = {ఖాళీ, 0};

#నిర్వచించు SHOW_సరిపోలిక(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    స్థిర పూర్ణాంకం
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    ఎంపిక (xp->xp_context)
    {
    స్థితి EXPAND_BOOL_SETTINGS:
    స్థితి EXPAND_COLORS:
    స్థితి EXPAND_COMPILER:
    స్థితి EXPAND_DIRECTORIES:
    స్థితి EXPAND_DIRS_లోపల_CDPATH:
    స్థితి EXPAND_FILES:
    స్థితి EXPAND_FILES_లోపల_మార్గం:
    స్థితి EXPAND_FILETYPE:
    స్థితి EXPAND_FILETYPECMD:
    స్థితి EXPAND_HELP:
    స్థితి EXPAND_KEYMAP:
    స్థితి EXPAND_OLD_SETTING:
    స్థితి EXPAND_STRING_SETTING:
    స్థితి EXPAND_SETTING_వ్యవకలనం:
    స్థితి EXPAND_OWNSYNTAX:
    స్థితి EXPAND_PACKADD:
    స్థితి EXPAND_RUNTIME:
    స్థితి EXPAND_SHELLCMD:
    స్థితి EXPAND_SHELLCMDLINE:
    స్థితి EXPAND_ట్యాగులు:
    స్థితి EXPAND_ట్యాగులు_LISTFILES:
	తిరిగి FALSE;

    అప్రమేయ:
	ఆపు;
    }

    తిరిగి vim_strchr(p_wop, WOP_FUZZY) != ఖాళీ;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    పూర్ణాంకం
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    తిరిగి vim_strchr(p_wop, WOP_FUZZY) != ఖాళీ && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    స్థిర పూర్ణాంకం
క్రమీకరించు_func_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    ఒకవేళ (*p1 != '<' && *p2 == '<') తిరిగి -1;
    ఒకవేళ (*p1 == '<' && *p2 != '<') తిరిగి 1;
    తిరిగి STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    స్థిర శూన్య
wildescape(
    expand_T	*xp,
    char_u	*str,
    పూర్ణాంకం		numfiles,
    char_u	**files)
{
    char_u	*p;
    పూర్ణాంకం		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_బఫరు : VSE_NONE;

    ఒకవేళ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_లోపల_మార్గం
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_లోపల_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	కొరకు (పూర్ణాంకం i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    ఒకవేళ (xp->xp_backslash & XP_BS_THREE)
	    {
		అక్షరం *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		ఒకవేళ (p != ఖాళీ)
		{
		    vim_free(files[i]);
		    files[i] = p;
#ఒకవేళ defined(BACKSLASH_లోపల_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    ఒకవేళ (p != ఖాళీ)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#అయితే_ముగింపు
		}
	    }
	    లేకపోతే ఒకవేళ (xp->xp_backslash & XP_BS_COMMA)
	    {
		ఒకవేళ (vim_strchr(files[i], ',') != ఖాళీ)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    ఒకవేళ (p != ఖాళీ)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#లేకపోతే
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#అయితే_ముగింపు
	    ఒకవేళ (p != ఖాళీ)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    ఒకవేళ (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	ఒకవేళ (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    లేకపోతే ఒకవేళ (xp->xp_context == EXPAND_ట్యాగులు)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	కొరకు (పూర్ణాంకం i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    ఒకవేళ (p != ఖాళీ)
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
    స్థిర శూన్య
ExpandEscape(
    expand_T	*xp,
    char_u	*str,
    పూర్ణాంకం		numfiles,
    char_u	**files,
    పూర్ణాంకం		options)
{
    // May change home directory back to "~"
    ఒకవేళ (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    ఒకవేళ (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    పూర్ణాంకం
nextwild(
    expand_T	*xp,
    పూర్ణాంకం		type,
    పూర్ణాంకం		options,	// extra options for ExpandOne()
    పూర్ణాంకం		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    పూర్ణాంకం		i;
    char_u	*p;
    పూర్ణాంకం		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    పూర్ణాంకం		wild_navigate = (type == WILD_తదుపరి || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    ఒకవేళ (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (ccline->ఆదానం_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    సమితి_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    may_expand_నమూనా = options & WILD_MAY_EXPAND_నమూనా;
	    సమితి_expand_context(xp);
	    may_expand_నమూనా = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    ఒకవేళ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	తిరిగి OK;  // Something illegal on command line
    }
    ఒకవేళ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	తిరిగి FAIL;
    }

    i = (పూర్ణాంకం)(xp->xp_నమూనా - ccline->cmdbuff);
    xp->xp_నమూనా_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    ఒకవేళ (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_నమూనా_len == 0)
	తిరిగి FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    ఒకవేళ (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	సందేశం_puts("...");	    // show that we are busy
	బహిర్గతం_flush();
    }

    ఒకవేళ (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, ఖాళీ, ఖాళీ, 0, type);
    }
    లేకపోతే
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_నమూనా;
	పూర్ణాంకం	pat_len = xp->xp_నమూనా_len;
#ఒకవేళ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	ఒకవేళ (unesc != ఖాళీ)
	    pat_src = unesc;
#అయితే_ముగింపు

	ఒకవేళ (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_నమూనా_లోపల_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	లేకపోతే
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#ఒకవేళ defined(FEAT_EVAL)
	vim_free(unesc);
#అయితే_ముగింపు

	// Translate string into pattern and expand it.
	ఒకవేళ (tmp == ఖాళీ)
	    p = ఖాళీ;
	లేకపోతే
	{
	    పూర్ణాంకం use_options = options |
		    WILD_HOME_REPLACE|WILD_కూడు_SLASH|WILD_SILENT;
	    ఒకవేళ (escape)
		use_options |= WILD_ESCAPE;
	    ఒకవేళ (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_నమూనా_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    ఒకవేళ (p != ఖాళీ && type == WILD_LONGEST)
	    {
		పూర్ణాంకం	j;

		కొరకు (j = 0; j < xp->xp_నమూనా_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    ఒకవేళ (c == '*' || c == '?')
			ఆపు;
		}
		ఒకవేళ ((పూర్ణాంకం)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    ఒకవేళ (!wild_navigate && ccline->cmdbuff != ఖాళీ)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	ఒకవేళ (cmdline_orig.string == ఖాళీ)
	    cmdline_orig.నిడివి = 0;
	లేకపోతే
	    cmdline_orig.నిడివి = ccline->cmdlen;
    }

    ఒకవేళ (p != ఖాళీ && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	పూర్ణాంకం	difflen;
	పూర్ణాంకం	v = OK;

	difflen = (పూర్ణాంకం)plen - xp->xp_నమూనా_len;
	ఒకవేళ (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_నమూనా = ccline->cmdbuff + i;
	}

	ఒకవేళ (v == OK)
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
    ఒకవేళ (xp->xp_context == EXPAND_MAPPINGS && p == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (xp->xp_numfiles <= 0 && p == ఖాళీ)
	beep_flush();
    లేకపోతే ఒకవేళ (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(శూన్య)ExpandOne(xp, ఖాళీ, ఖాళీ, 0, WILD_FREE);

    vim_free(p);

    తిరిగి OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    స్థిర పూర్ణాంకం
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	పూర్ణాంకం		అంకె_Matches,
	పూర్ణాంకం		showtail)
{
    పూర్ణాంకం	ఉపసర్గ_len;

    // Add all the completion matches
    compl_సరిపోలిక_array = ALLOC_MULT(pumitem_T, అంకె_Matches);
    ఒకవేళ (compl_సరిపోలిక_array == ఖాళీ)
	తిరిగి EXPAND_UNSUCCESSFUL;

    compl_సరిపోలిక_arraysize = అంకె_Matches;
    కొరకు (పూర్ణాంకం i = 0; i < అంకె_Matches; i++)
    {
	compl_సరిపోలిక_array[i].pum_పాఠ్యం = (xp->xp_files_abbr != ఖాళీ
				    && xp->xp_files_abbr[i] != ఖాళీ)
					? xp->xp_files_abbr[i]
					: SHOW_సరిపోలిక(i);
	compl_సరిపోలిక_array[i].pum_info = xp->xp_files_info != ఖాళీ
					    ? xp->xp_files_info[i] : ఖాళీ;
	compl_సరిపోలిక_array[i].pum_extra = xp->xp_files_menu != ఖాళీ
					    ? xp->xp_files_menu[i] : ఖాళీ;
	compl_సరిపోలిక_array[i].pum_రకం = xp->xp_files_రకం != ఖాళీ
					    ? xp->xp_files_రకం[i] : ఖాళీ;
	compl_సరిపోలిక_array[i].pum_user_abbr_hlattr = -1;
	compl_సరిపోలిక_array[i].pum_user_రకం_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == ఖాళీ ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    ఉపసర్గ_len = vim_strsize(xp->xp_నమూనా);
    ఒకవేళ (showtail)
	ఉపసర్గ_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_నిలువు_off + MAX(0, compl_startcol - ఉపసర్గ_len);

    తిరిగి EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    శూన్య
cmdline_pum_display(శూన్య)
{
    ఒకవేళ (p_po > 0 && p_po < 100 && !pum_redraw_లోపల_same_position())
	pum_call_update_screen();
    pum_display(compl_సరిపోలిక_array, compl_సరిపోలిక_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    పూర్ణాంకం
cmdline_pum_చురుకైన(శూన్య)
{
    తిరిగి pum_visible() && compl_సరిపోలిక_array != ఖాళీ;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    శూన్య
cmdline_pum_remove(cmdline_info_T *cclp వాడని, పూర్ణాంకం defer_redraw)
{
    పూర్ణాంకం	save_కీలకం_Typed = కీలకం_Typed;
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం	save_RedrawingDisabled = RedrawingDisabled;
    ఒకవేళ (cclp->ఆదానం_fn)
	RedrawingDisabled = 0;
#అయితే_ముగింపు

    పదం_సమితి_sync_output(పదం_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_సరిపోలిక_array);
    compl_సరిపోలిక_arraysize = 0;
    ఒకవేళ (!defer_redraw)
    {
	పూర్ణాంకం save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    లేకపోతే
	pum_call_update_screen();
    redrawcmd();
    పదం_సమితి_sync_output(పదం_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    కీలకం_Typed = save_కీలకం_Typed;
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (cclp->ఆదానం_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#అయితే_ముగింపు
}

    శూన్య
cmdline_pum_cleanup(cmdline_info_T *cclp)
{
    cmdline_pum_remove(cclp, FALSE);
    wildmenu_cleanup(cclp);
}

/*
 * Returns the starting column number to use for the cmdline completion popup
 * menu.
 */
    పూర్ణాంకం
cmdline_compl_startcol(శూన్య)
{
    తిరిగి compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_నమూనా(శూన్య)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    తిరిగి xp == ఖాళీ ? ఖాళీ : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    పూర్ణాంకం
cmdline_compl_is_fuzzy(శూన్య)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    తిరిగి xp != ఖాళీ && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    స్థిర పూర్ణాంకం
skip_స్థితి_సరిపోలిక_char(expand_T *xp, char_u *s)
{
    ఒకవేళ ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_నమూనా_లోపల_BUF)
#నిర్వచితమైతే FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#అయితే_ముగింపు
	   )
    {
#నిర్వచితం_కాకపోతే BACKSLASH_లోపల_FILENAME
	ఒకవేళ (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    తిరిగి 2;
#అయితే_ముగింపు
	తిరిగి 1;
    }
    తిరిగి 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    స్థిర పూర్ణాంకం
స్థితి_సరిపోలిక_len(expand_T *xp, char_u *s)
{
    పూర్ణాంకం	len = 0;

#నిర్వచితమైతే FEAT_MENU
    పూర్ణాంకం emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    ఒకవేళ (emenu && menu_is_separator(s))
	తిరిగి 1;
#అయితే_ముగింపు

    అయితే (*s != NUL)
    {
	s += skip_స్థితి_సరిపోలిక_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    తిరిగి len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    స్థిర శూన్య
win_redr_స్థితి_matches(
    expand_T	*xp,
    పూర్ణాంకం		అంకె_matches,
    char_u	**matches,	// list of matches
    పూర్ణాంకం		సరిపోలిక,
    పూర్ణాంకం		showtail)
{
    పూర్ణాంకం		అడ్డువరుస;
    char_u	*buf;
    పూర్ణాంకం		len;
    పూర్ణాంకం		clen;		// length in screen cells
    పూర్ణాంకం		fillchar;
    పూర్ణాంకం		attr;
    పూర్ణాంకం		i;
    పూర్ణాంకం		highlight = TRUE;
    char_u	*selstart = ఖాళీ;
    పూర్ణాంకం		selstart_నిలువు = 0;
    char_u	*selend = ఖాళీ;
    స్థిర పూర్ణాంకం	మొదటి_సరిపోలిక = 0;
    పూర్ణాంకం		కూడు_ఎడమ = FALSE;
    char_u	*s;
#నిర్వచితమైతే FEAT_MENU
    పూర్ణాంకం		emenu;
#అయితే_ముగింపు
    పూర్ణాంకం		l;

    ఒకవేళ (matches == ఖాళీ)	// interrupted completion?
	తిరిగి;

    ఒకవేళ (has_mbyte)
	buf = alloc(topframe->fr_వెడల్పు * MB_MAXBYTES + 1);
    లేకపోతే
	buf = alloc(topframe->fr_వెడల్పు + 1);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    ఒకవేళ (సరిపోలిక == -1)	// don't show match but original text
    {
	సరిపోలిక = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = స్థితి_సరిపోలిక_len(xp, SHOW_సరిపోలిక(సరిపోలిక)) + 3;
    ఒకవేళ (సరిపోలిక == 0)
	మొదటి_సరిపోలిక = 0;
    లేకపోతే ఒకవేళ (సరిపోలిక < మొదటి_సరిపోలిక)
    {
	// jumping left, as far as we can go
	మొదటి_సరిపోలిక = సరిపోలిక;
	కూడు_ఎడమ = TRUE;
    }
    లేకపోతే
    {
	// check if match fits on the screen
	కొరకు (i = మొదటి_సరిపోలిక; i < సరిపోలిక; ++i)
	    clen += స్థితి_సరిపోలిక_len(xp, SHOW_సరిపోలిక(i)) + 2;
	ఒకవేళ (మొదటి_సరిపోలిక > 0)
	    clen += 2;
	// jumping right, put match at the left
	ఒకవేళ (clen > topframe->fr_వెడల్పు)
	{
	    మొదటి_సరిపోలిక = సరిపోలిక;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    కొరకు (i = సరిపోలిక; i < అంకె_matches; ++i)
	    {
		clen += స్థితి_సరిపోలిక_len(xp, SHOW_సరిపోలిక(i)) + 2;
		ఒకవేళ (clen >= topframe->fr_వెడల్పు)
		    ఆపు;
	    }
	    ఒకవేళ (i == అంకె_matches)
		కూడు_ఎడమ = TRUE;
	}
    }
    ఒకవేళ (కూడు_ఎడమ)
	అయితే (మొదటి_సరిపోలిక > 0)
	{
	    clen += స్థితి_సరిపోలిక_len(xp, SHOW_సరిపోలిక(మొదటి_సరిపోలిక - 1)) + 2;
	    ఒకవేళ (clen >= topframe->fr_వెడల్పు)
		ఆపు;
	    --మొదటి_సరిపోలిక;
	}

    fillchar = fillchar_స్థితి(&attr, curwin);

    ఒకవేళ (మొదటి_సరిపోలిక == 0)
    {
	*buf = NUL;
	len = 0;
    }
    లేకపోతే
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = మొదటి_సరిపోలిక;
    అయితే (clen + స్థితి_సరిపోలిక_len(xp, SHOW_సరిపోలిక(i)) + 2 < topframe->fr_వెడల్పు)
    {
	ఒకవేళ (i == సరిపోలిక)
	{
	    selstart = buf + len;
	    selstart_నిలువు = clen;
	}

	s = SHOW_సరిపోలిక(i);
	// Check for menu separators - replace with '|'
#నిర్వచితమైతే FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	ఒకవేళ (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (పూర్ణాంకం)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	లేకపోతే
#అయితే_ముగింపు
	కొరకు ( ; *s != NUL; ++s)
	{
	    s += skip_స్థితి_సరిపోలిక_char(xp, s);
	    clen += ptr2cells(s);
	    ఒకవేళ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    లేకపోతే
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (పూర్ణాంకం)STRLEN(buf + len);
	    }
	}
	ఒకవేళ (i == సరిపోలిక)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	ఒకవేళ (++i == అంకె_matches)
		ఆపు;
    }

    ఒకవేళ (i != అంకె_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    అడ్డువరుస = cmdline_అడ్డువరుస - 1;
    ఒకవేళ (అడ్డువరుస >= 0)
    {
	ఒకవేళ (wild_menu_showing == 0)
	{
	    ఒకవేళ (సందేశం_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		ఒకవేళ (cmdline_అడ్డువరుస == అడ్డువరుసలు - 1)
		{
		    screen_del_పంక్తులు(0, 0, 1, (పూర్ణాంకం)అడ్డువరుసలు, TRUE, 0, ఖాళీ);
		    ++సందేశం_scrolled;
		}
		లేకపోతే
		{
		    ++cmdline_అడ్డువరుస;
		    ++అడ్డువరుస;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    లేకపోతే
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		ఒకవేళ (lastwin->w_స్థితి_ఎత్తు == 0)
		{
		    save_p_ls = p_ls;
		    save_p_wmh = p_wmh;
		    p_ls = 2;
		    p_wmh = 0;
		    చివరి_స్థితి(FALSE);
		}
		wild_menu_showing = WM_SHOWN;
	    }
	}

	screen_puts(buf, అడ్డువరుస, firstwin->w_wincol, attr);
	ఒకవేళ (selstart != ఖాళీ && highlight)
	{
	    *selend = NUL;
	    screen_puts(selstart, అడ్డువరుస, firstwin->w_wincol + selstart_నిలువు,
		    HL_ATTR(HLF_WM));
	}

	screen_fill(అడ్డువరుస, అడ్డువరుస + 1, firstwin->w_wincol + clen,
		firstwin->w_wincol + topframe->fr_వెడల్పు,
		fillchar, fillchar, attr);
    }

    win_redraw_చివరి_స్థితి(topframe);
    vim_free(buf);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Apply -completeopt=escape to a string about to be inserted into the command
 * line as a completion result.  If "str" is non-NULL and the active expansion
 * context is a customlist/custom user command with UCC_ESCAPE set, free "str"
 * and return a newly-allocated copy with spaces, tabs and backslashes prefixed
 * by a backslash.  Otherwise return "str" unchanged.
 */
    స్థిర char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    ఒకవేళ (str == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	తిరిగి str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    ఒకవేళ (p == ఖాళీ)
	తిరిగి str;
    vim_free(str);
    తిరిగి p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    స్థిర char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    పూర్ణాంకం		srclen,
    పూర్ణాంకం		*new_lenp)
{
    char_u  *buf, *p, *d, *చివర;

    ఒకవేళ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	తిరిగి ఖాళీ;

    buf = alloc(srclen + 1);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి ఖాళీ;

    d = buf;
    చివర = src + srclen;
    కొరకు (p = src; p < చివర; ++p)
    {
	ఒకవేళ (*p == '\\' && p + 1 < చివర
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (పూర్ణాంకం)(d - buf);
    తిరిగి buf;
}
#అయితే_ముగింపు

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    స్థిర char_u *
get_తదుపరి_or_prev_సరిపోలిక(పూర్ణాంకం బహుళకం, expand_T *xp)
{
    పూర్ణాంకం	    findex = xp->xp_selected;
    పూర్ణాంకం	    ht;

    // When no matches found, return NULL
    ఒకవేళ (xp->xp_numfiles <= 0)
	తిరిగి ఖాళీ;

    ఒకవేళ (బహుళకం == WILD_PREV)
    {
	// Select the last entry if at original text
	ఒకవేళ (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    లేకపోతే ఒకవేళ (బహుళకం == WILD_తదుపరి)
    {
	// Select the next entry
	++findex;
    }
    లేకపోతే   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_ఎత్తు();
	ఒకవేళ (ht > 3)
	    ht -= 2;

	ఒకవేళ (బహుళకం == WILD_PAGEUP)
	{
	    ఒకవేళ (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    లేకపోతే ఒకవేళ (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    లేకపోతే
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	లేకపోతే    // mode == WILD_PAGEDOWN
	{
	    ఒకవేళ (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    లేకపోతే ఒకవేళ (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    లేకపోతే
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    ఒకవేళ (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	ఒకవేళ (xp->xp_orig != ఖాళీ)
	    findex = -1;
	లేకపోతే
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    ఒకవేళ (p_wmnu)
    {
	ఒకవేళ (compl_సరిపోలిక_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	లేకపోతే ఒకవేళ (vim_strchr(p_wop, WOP_PUM) != ఖాళీ)
	{
	    ఒకవేళ (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	లేకపోతే
	    win_redr_స్థితి_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    ఒకవేళ (findex == -1)
	తిరిగి vim_strsave(xp->xp_orig);
#ఒకవేళ defined(FEAT_EVAL)
    తిరిగి apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#లేకపోతే
    తిరిగి vim_strsave(xp->xp_files[findex]);
#అయితే_ముగింపు
}

/*
 * Start the command-line expansion and get the matches.
 */
    స్థిర char_u *
Expand_One_ప్రారంభం(పూర్ణాంకం బహుళకం, expand_T *xp, char_u *str, పూర్ణాంకం options)
{
    పూర్ణాంకం		non_suf_సరిపోలిక;		// number without matching suffix
    పూర్ణాంకం		i;
    char_u	*ss = ఖాళీ;

    // Do the expansion.
    ఒకవేళ (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#నిర్వచితమైతే FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	ఒకవేళ (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_సరిపోలిక_str_2), str);
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (xp->xp_numfiles == 0)
    {
	ఒకవేళ (!(options & WILD_SILENT))
	    semsg(_(e_no_సరిపోలిక_str_2), str);
    }
    లేకపోతే
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	ఒకవేళ (బహుళకం != WILD_ALL && బహుళకం != WILD_ALL_KEEP && బహుళకం != WILD_LONGEST)
	{
	    ఒకవేళ (xp->xp_numfiles)
		non_suf_సరిపోలిక = xp->xp_numfiles;
	    లేకపోతే
		non_suf_సరిపోలిక = 1;
	    ఒకవేళ ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_సరిపోలిక = 0;
		కొరకు (i = 0; i < 2; ++i)
		    ఒకవేళ (సరిపోలిక_suffix(xp->xp_files[i]))
			++non_suf_సరిపోలిక;
	    }
	    ఒకవేళ (non_suf_సరిపోలిక != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		ఒకవేళ (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_పేర్లు));
		లేకపోతే ఒకవేళ (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    ఒకవేళ (!(non_suf_సరిపోలిక != 1 && బహుళకం == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    తిరిగి ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    స్థిర char_u *
కనుగొను_longest_సరిపోలిక(expand_T *xp, పూర్ణాంకం options)
{
    long_u	len;
    పూర్ణాంకం		mb_len = 1;
    పూర్ణాంకం		c0, ci;
    పూర్ణాంకం		i;
    char_u	*ss;

    కొరకు (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	ఒకవేళ (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	లేకపోతే
	    c0 = xp->xp_files[0][len];
	కొరకు (i = 1; i < xp->xp_numfiles; ++i)
	{
	    ఒకవేళ (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    లేకపోతే
		ci = xp->xp_files[i][len];
	    ఒకవేళ (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		ఒకవేళ (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ (c0 != ci)
		ఆపు;
	}
	ఒకవేళ (i < xp->xp_numfiles)
	{
	    ఒకవేళ (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    ఆపు;
	}
    }

    ss = alloc(len + 1);
    ఒకవేళ (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    తిరిగి ss;
}

    శూన్య
free_xp_files_extra(expand_T *xp, పూర్ణాంకం numfiles)
{
    ఒకవేళ (xp->xp_files_abbr != ఖాళీ)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = ఖాళీ;
    }
    ఒకవేళ (xp->xp_files_రకం != ఖాళీ)
    {
	FreeWild(numfiles, xp->xp_files_రకం);
	xp->xp_files_రకం = ఖాళీ;
    }
    ఒకవేళ (xp->xp_files_menu != ఖాళీ)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = ఖాళీ;
    }
    ఒకవేళ (xp->xp_files_info != ఖాళీ)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = ఖాళీ;
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
    పూర్ణాంకం		options,
    పూర్ణాంకం		బహుళకం)
{
    char_u	*ss = ఖాళీ;
    పూర్ణాంకం		orig_saved = FALSE;
#ఒకవేళ defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    పూర్ణాంకం		ss_is_సరిపోలిక = FALSE;
#అయితే_ముగింపు

    // first handle the case of using an old match
    ఒకవేళ (బహుళకం == WILD_తదుపరి || బహుళకం == WILD_PREV
	    || బహుళకం == WILD_PAGEUP || బహుళకం == WILD_PAGEDOWN)
	తిరిగి get_తదుపరి_or_prev_సరిపోలిక(బహుళకం, xp);

    ఒకవేళ (బహుళకం == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    లేకపోతే ఒకవేళ (బహుళకం == WILD_APPLY)
    {
	ఒకవేళ (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	లేకపోతే
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#ఒకవేళ defined(FEAT_EVAL)
	    ss_is_సరిపోలిక = TRUE;
#అయితే_ముగింపు
	}
    }

    // free old names
    ఒకవేళ (xp->xp_numfiles != -1 && బహుళకం != WILD_ALL && బహుళకం != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	ఒకవేళ (compl_సరిపోలిక_array != ఖాళీ)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    ఒకవేళ (బహుళకం == WILD_FREE)	// only release file name
	తిరిగి ఖాళీ;

    ఒకవేళ (xp->xp_numfiles == -1 && బహుళకం != WILD_APPLY && బహుళకం != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = Expand_One_ప్రారంభం(బహుళకం, xp, str, options);
#ఒకవేళ defined(FEAT_EVAL)
	ఒకవేళ (ss != ఖాళీ)
	    ss_is_సరిపోలిక = TRUE;
#అయితే_ముగింపు
    }

    // Find longest common part
    ఒకవేళ (బహుళకం == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = కనుగొను_longest_సరిపోలిక(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#ఒకవేళ defined(FEAT_EVAL)
	ఒకవేళ (ss != ఖాళీ)
	    ss_is_సరిపోలిక = TRUE;
#అయితే_ముగింపు
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    ఒకవేళ (బహుళకం == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	అక్షరం	*ఉపసర్గ = "";
	అక్షరం	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	పూర్ణాంకం	n = xp->xp_numfiles - 1;
	పూర్ణాంకం	i;
#ఒకవేళ defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = ఖాళీ;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	ఒకవేళ ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    ఒకవేళ (escaped != ఖాళీ)
	    {
		కొరకు (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    ఒకవేళ (escaped[i] == ఖాళీ)
		    {
			అయితే (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			ఆపు;
		    }
		}
		ఒకవేళ (escaped != ఖాళీ)
		    files = escaped;
	    }
	}
#లేకపోతే
	char_u	**files = xp->xp_files;
#అయితే_ముగింపు

	ఒకవేళ (xp->xp_ఉపసర్గ == XP_ఉపసర్గ_NO)
	{
	    ఉపసర్గ = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	లేకపోతే ఒకవేళ (xp->xp_ఉపసర్గ == XP_ఉపసర్గ_INV)
	{
	    ఉపసర్గ = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	కొరకు (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	ఒకవేళ (ss != ఖాళీ)
	{
	    size_t  ss_len = 0;

	    కొరకు (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (అక్షరం *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? ఉపసర్గ : "",
		    (అక్షరం *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#ఒకవేళ defined(FEAT_EVAL)
	ఒకవేళ (escaped != ఖాళీ)
	{
	    కొరకు (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#అయితే_ముగింపు
    }

    ఒకవేళ (బహుళకం == WILD_EXPAND_FREE || బహుళకం == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    ఒకవేళ (!orig_saved)
	vim_free(orig);

#ఒకవేళ defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    ఒకవేళ (ss_is_సరిపోలిక && బహుళకం != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#అయితే_ముగింపు
    తిరిగి ss;
}

/*
 * Prepare an expand structure for use.
 */
    శూన్య
ExpandInit(expand_T *xp)
{
    CLEAR_సూచిక(xp);
    xp->xp_backslash = XP_BS_NONE;
    xp->xp_ఉపసర్గ = XP_ఉపసర్గ_NONE;
    xp->xp_numfiles = -1;
}

/*
 * Cleanup an expand structure after use.
 */
    శూన్య
ExpandCleanup(expand_T *xp)
{
    ఒకవేళ (xp->xp_numfiles >= 0)
    {
	free_xp_files_extra(xp, xp->xp_numfiles);
	FreeWild(xp->xp_numfiles, xp->xp_files);
	xp->xp_numfiles = -1;
    }
    VIM_CLEAR(xp->xp_orig);
}

    శూన్య
clear_cmdline_orig(శూన్య)
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
    స్థిర శూన్య
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	పూర్ణాంకం		అంకె_Matches,
	పూర్ణాంకం		పంక్తులు,
	పూర్ణాంకం		linenr,
	పూర్ణాంకం		maxlen,
	పూర్ణాంకం		showtail,
	పూర్ణాంకం		dir_attr)
{
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		isdir;
    పూర్ణాంకం		lastlen;
    char_u	*p;

    lastlen = 999;
    కొరకు (j = linenr; j < అంకె_Matches; j += పంక్తులు)
    {
	ఒకవేళ (xp->xp_context == EXPAND_ట్యాగులు_LISTFILES)
	{
	    సందేశం_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    సందేశం_advance(maxlen + 1);
	    సందేశం_puts((అక్షరం *)p);
	    సందేశం_advance(maxlen + 3);
	    సందేశం_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    ఆపు;
	}
	కొరకు (i = maxlen - lastlen; --i >= 0; )
	    సందేశం_putchar(' ');
	ఒకవేళ (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    ఒకవేళ (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_మార్గం;
		char_u	*మార్గం;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_మార్గం = expand_env_save_opt(matches[j], TRUE, ఖాళీ);
		మార్గం = exp_మార్గం != ఖాళీ ? exp_మార్గం : matches[j];
		halved_slash = backslash_halve_save(మార్గం);
		isdir = mch_isdir(halved_slash != ఖాళీ ? halved_slash
			: matches[j]);
		vim_free(exp_మార్గం);
		ఒకవేళ (halved_slash != మార్గం)
		    vim_free(halved_slash);
	    }
	    లేకపోతే
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    ఒకవేళ (showtail)
		p = SHOW_సరిపోలిక(j);
	    లేకపోతే
	    {
		home_replace(ఖాళీ, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	లేకపోతే
	{
	    isdir = FALSE;
	    p = SHOW_సరిపోలిక(j);
	}
	lastlen = సందేశం_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    ఒకవేళ (సందేశం_నిలువు > 0)	// when not wrapped around
    {
	సందేశం_clr_eos();
	సందేశం_putchar('\n');
    }
    బహిర్గతం_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    స్థిర పూర్ణాంకం
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	పూర్ణాంకం		అంకె_Matches,
	పూర్ణాంకం		showtail,
	పూర్ణాంకం		noselect)
{
    పూర్ణాంకం	retval = cmdline_pum_create(ccline, xp, matches, అంకె_Matches, showtail);

    ఒకవేళ (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    తిరిగి retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    పూర్ణాంకం
showmatches(
    expand_T	*xp,
    పూర్ణాంకం		display_wildmenu,
    పూర్ణాంకం		display_list,
    పూర్ణాంకం		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    పూర్ణాంకం		అంకె_Matches;
    char_u	**matches;
    పూర్ణాంకం		i;
    పూర్ణాంకం		maxlen;
    పూర్ణాంకం		పంక్తులు;
    పూర్ణాంకం		columns;
    పూర్ణాంకం		attr;
    పూర్ణాంకం		showtail;
    పూర్ణాంకం		noselect = (wim_flags_arg & WIM_NOSELECT);
    పూర్ణాంకం		noinsert = (wim_flags_arg & WIM_NOINSERT);
    పూర్ణాంకం		cmdline_unchanged = noselect || noinsert;

    ఒకవేళ (xp->xp_numfiles == -1)
    {
	పూర్ణాంకం retval;
	సమితి_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&అంకె_Matches, &matches);
	ఒకవేళ (retval != EXPAND_OK)
	    తిరిగి retval;
	showtail = expand_showtail(xp);
    }
    లేకపోతే
    {
	అంకె_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    ఒకవేళ (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != ఖాళీ)
	తిరిగి show_pum_matches(ccline, xp, matches, అంకె_Matches,
		showtail && !cmdline_unchanged, noselect);

    ఒకవేళ (display_list)
    {
	సందేశం_didany = FALSE;		// lines_left will be set
	సందేశం_ప్రారంభం();			// prepare for paging
	సందేశం_putchar('\n');
	బహిర్గతం_flush();
	cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;
	సందేశం_didany = FALSE;		// lines_left will be set again
	సందేశం_ప్రారంభం();			// prepare for paging
    }

    ఒకవేళ (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    లేకపోతే ఒకవేళ (display_wildmenu && !display_list)
	win_redr_స్థితి_matches(xp, అంకె_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    లేకపోతే ఒకవేళ (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	కొరకు (i = 0; i < అంకె_Matches; ++i)
	{
	    పూర్ణాంకం	len;
	    ఒకవేళ (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(ఖాళీ, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    లేకపోతే
		len = vim_strsize(SHOW_సరిపోలిక(i));
	    ఒకవేళ (len > maxlen)
		maxlen = len;
	}

	ఒకవేళ (xp->xp_context == EXPAND_ట్యాగులు_LISTFILES)
	    పంక్తులు = అంకె_Matches;
	లేకపోతే
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_వెడల్పు + 2) / maxlen;
	    ఒకవేళ (columns < 1)
		columns = 1;
	    పంక్తులు = (అంకె_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	ఒకవేళ (xp->xp_context == EXPAND_ట్యాగులు_LISTFILES)
	{
	    సందేశం_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    సందేశం_clr_eos();
	    సందేశం_advance(maxlen - 3);
	    సందేశం_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	కొరకు (i = 0; i < పంక్తులు; ++i)
	{
	    showmatches_oneline(xp, matches, అంకె_Matches, పంక్తులు, i,
						maxlen, showtail, attr);
	    ఒకవేళ (got_int)
	    {
		got_int = FALSE;
		ఆపు;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    ఒకవేళ (display_wildmenu && display_list)
    {
	ఒకవేళ (vim_strchr(p_wop, WOP_PUM) != ఖాళీ)
	    (శూన్య)show_pum_matches(ccline, xp, matches, అంకె_Matches,
		    showtail && !cmdline_unchanged, noselect);
	లేకపోతే
	    win_redr_స్థితి_matches(xp, అంకె_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    ఒకవేళ (xp->xp_numfiles == -1)
    {
	FreeWild(అంకె_Matches, matches);
	free_xp_files_extra(xp, అంకె_Matches);
    }

    తిరిగి EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    స్థిర char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    పూర్ణాంకం		had_sep = FALSE;

    కొరకు (p = s; *p != NUL; )
    {
	ఒకవేళ (vim_ispathsep(*p)
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
		&& !rem_backslash(p)
#అయితే_ముగింపు
	   )
	    had_sep = TRUE;
	లేకపోతే ఒకవేళ (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    తిరిగి t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    స్థిర పూర్ణాంకం
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*చివర;

    // When not completing file names a "/" may mean something different.
    ఒకవేళ (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	తిరిగి FALSE;

    చివర = gettail(xp->xp_నమూనా);
    ఒకవేళ (చివర == xp->xp_నమూనా)		// there is no path separator
	తిరిగి FALSE;

    కొరకు (s = xp->xp_నమూనా; s < చివర; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	ఒకవేళ (rem_backslash(s))
	    ++s;
	లేకపోతే ఒకవేళ (vim_strchr((char_u *)"*?[", *s) != ఖాళీ)
	    తిరిగి FALSE;
    }
    తిరిగి TRUE;
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
    పూర్ణాంకం		len,
    పూర్ణాంకం		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		new_len;
    char_u	*తోక;
    పూర్ణాంకం		చివర్లు_లోపల_నక్షత్రం;

    ఒకవేళ (context != EXPAND_FILES
	    && context != EXPAND_FILES_లోపల_మార్గం
	    && context != EXPAND_SHELLCMD
	    && context != EXPAND_DIRECTORIES
	    && context != EXPAND_DIRS_లోపల_CDPATH)
    {
	// Matching will be done internally (on something other than files).
	// So we convert the file-matching-type wildcards into our kind for
	// use with vim_regcomp().  First work out how long it will be:

	// For help tags the translation is done in find_help_tags().
	// For a tag pattern starting with "/" no translation is needed.
	ఒకవేళ (context == EXPAND_FINDFUNC
		|| context == EXPAND_HELP
		|| context == EXPAND_COLORS
		|| context == EXPAND_COMPILER
		|| context == EXPAND_OWNSYNTAX
		|| context == EXPAND_FILETYPE
		|| context == EXPAND_KEYMAP
		|| context == EXPAND_PACKADD
		|| context == EXPAND_RUNTIME
		|| ((context == EXPAND_ట్యాగులు_LISTFILES
			|| context == EXPAND_ట్యాగులు)
		    && fname[0] == '/'))
	    retval = vim_strnsave(fname, len);
	లేకపోతే
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    కొరకు (i = 0; i < len; i++)
	    {
		ఒకవేళ (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		ఒకవేళ (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		ఒకవేళ ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    ఒకవేళ (retval != ఖాళీ)
	    {
		retval[0] = '^';
		j = 1;
		కొరకు (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    ఒకవేళ (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			ఆపు;

		    ఎంపిక (fname[i])
		    {
			స్థితి '*':   retval[j++] = '.';
				    ఆపు;
			స్థితి '~':   retval[j++] = '\\';
				    ఆపు;
			స్థితి '?':   retval[j] = '.';
				    కొనసాగు;
			స్థితి '.':   ఒకవేళ (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    ఆపు;
			స్థితి '\\':  ఒకవేళ (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    ఆపు;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    లేకపోతే
    {
	retval = alloc(len + 4);
	ఒకవేళ (retval != ఖాళీ)
	{
	    vim_strncpy(retval, fname, len);

	    // Don't add a star to *, ~, ~user, $var or `cmd`.
	    // * would become **, which walks the whole tree.
	    // ~ would be at the start of the file name, but not the tail.
	    // $ could be anywhere in the tail.
	    // ` could be anywhere in the file name.
	    // When the name ends in '$' don't add a star, remove the '$'.
	    తోక = gettail(retval);
	    చివర్లు_లోపల_నక్షత్రం = (len > 0 && retval[len - 1] == '*');
#నిర్వచితం_కాకపోతే BACKSLASH_లోపల_FILENAME
	    కొరకు (i = len - 2; i >= 0; --i)
	    {
		ఒకవేళ (retval[i] != '\\')
		    ఆపు;
		చివర్లు_లోపల_నక్షత్రం = !చివర్లు_లోపల_నక్షత్రం;
	    }
#అయితే_ముగింపు
	    ఒకవేళ ((*retval != '~' || తోక != retval)
		    && !చివర్లు_లోపల_నక్షత్రం
		    && vim_strchr(తోక, '$') == ఖాళీ
		    && vim_strchr(retval, '`') == ఖాళీ)
		retval[len++] = '*';
	    లేకపోతే ఒకవేళ (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    తిరిగి retval;
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
    శూన్య
సమితి_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    ఒకవేళ ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_నమూనా)
    {
	xp->xp_context = EXPAND_నమూనా_లోపల_BUF;
	xp->xp_శోధన_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_నమూనా = ccline->cmdbuff;
	xp->xp_నమూనా_len = ccline->cmdpos;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	శోధన_మొదటి_పంక్తి = 0; // Search entire buffer
#అయితే_ముగింపు
	తిరిగి;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    ఒకవేళ (ccline->cmdfirstc != ':'
#నిర్వచితమైతే FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->ఆదానం_fn
#అయితే_ముగింపు
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	తిరిగి;
    }

    // Fallback to command-line expansion
    సమితి_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, TRUE);
}

/*
 * Sets the index of a built-in or user defined command 'cmd' in eap->cmdidx.
 * For user defined commands, the completion context is set in 'xp' and the
 * completion flags in 'complp'.
 *
 * Returns a pointer to the text after the command or NULL for failure.
 */
    స్థిర char_u *
సమితి_cmd_సూచీ(char_u *cmd, exarg_T *eap, expand_T *xp, పూర్ణాంకం *complp)
{
    char_u	*p = ఖాళీ;
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    ఒకవేళ (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    లేకపోతే
    {
	p = cmd;
	అయితే (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	ఒకవేళ (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    అయితే (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	ఒకవేళ (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    అయితే (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	ఒకవేళ (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != ఖాళీ)
	    ++p;
	len = (పూర్ణాంకం)(p - cmd);

	ఒకవేళ (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    తిరిగి ఖాళీ;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	ఒకవేళ ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    అయితే (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    ఒకవేళ (*p == NUL && ASCII_ISALNUM(p[-1]))
	తిరిగి ఖాళీ;

    ఒకవేళ (eap->cmdidx == CMD_కొలత)
    {
	ఒకవేళ (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != ఖాళీ)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	లేకపోతే ఒకవేళ (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = కనుగొను_ucmd(eap, p, ఖాళీ, xp, complp);
	    ఒకవేళ (p == ఖాళీ)
		eap->cmdidx = CMD_కొలత;	// ambiguous user command
	}
    }
    ఒకవేళ (eap->cmdidx == CMD_కొలత)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	తిరిగి ఖాళీ;
    }

    తిరిగి p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    స్థిర శూన్య
సమితి_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	పూర్ణాంకం		usefilter,
	expand_T	*xp,
	పూర్ణాంకం		*complp)
{
    char_u	*p;
    పూర్ణాంకం		c;
    పూర్ణాంకం		లోపల_quote = FALSE;
    char_u	*bow = ఖాళీ;	// Beginning of word
    పూర్ణాంకం		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_నమూనా = skipwhite(arg);
    p = xp->xp_నమూనా;
    అయితే (*p != NUL)
    {
	ఒకవేళ (has_mbyte)
	    c = mb_ptr2char(p);
	లేకపోతే
	    c = *p;
	ఒకవేళ (c == '\\' && p[1] != NUL)
	    ++p;
	లేకపోతే ఒకవేళ (c == '`')
	{
	    ఒకవేళ (!లోపల_quote)
	    {
		xp->xp_నమూనా = p;
		bow = p + 1;
	    }
	    లోపల_quote = !లోపల_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	లేకపోతే ఒకవేళ (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != ఖాళీ && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    అయితే (*p != NUL)
	    {
		ఒకవేళ (has_mbyte)
		    c = mb_ptr2char(p);
		లేకపోతే
		    c = *p;
		ఒకవేళ (c == '`' || vim_isfilec_or_wc(c))
		    ఆపు;
		ఒకవేళ (has_mbyte)
		    len = (*mb_ptr2len)(p);
		లేకపోతే
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    ఒకవేళ (లోపల_quote)
		bow = p;
	    లేకపోతే
		xp->xp_నమూనా = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    ఒకవేళ (bow != ఖాళీ && లోపల_quote)
	xp->xp_నమూనా = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    ఒకవేళ (usefilter
	    || (eap != ఖాళీ
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#నిర్వచితం_కాకపోతే BACKSLASH_లోపల_FILENAME
	xp->xp_shell = TRUE;
#అయితే_ముగింపు
	// When still after the command name expand executables.
	ఒకవేళ (xp->xp_నమూనా == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    ఒకవేళ (*xp->xp_నమూనా == '$')
    {
	కొరకు (p = xp->xp_నమూనా + 1; *p != NUL; ++p)
	    ఒకవేళ (!vim_isIDc(*p))
		ఆపు;
	ఒకవేళ (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_నమూనా;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    ఒకవేళ (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    ఒకవేళ (*xp->xp_నమూనా == '~')
    {
	కొరకు (p = xp->xp_నమూనా + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	ఒకవేళ (*p == NUL && p > xp->xp_నమూనా + 1
		&& సరిపోలిక_user(xp->xp_నమూనా + 1) >= 1)
	{
	    xp->xp_context = EXPAND_USER;
	    ++xp->xp_నమూనా;
	}
    }
}

/*
 * Set the completion context for the "++opt=arg" argument.  Always returns
 * NULL.
 */
    స్థిర char_u *
సమితి_context_లోపల_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    ఒకవేళ (p == ఖాళీ)
	xp->xp_నమూనా = arg;
    లేకపోతే
	xp->xp_నమూనా = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    తిరిగి ఖాళీ;
}

#నిర్వచితమైతే FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    స్థిర char_u *
సమితి_context_లోపల_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    ఒకవేళ (p == ఖాళీ)
	xp->xp_నమూనా = arg;
    లేకపోతే
	xp->xp_నమూనా = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    స్థిర char_u *
సమితి_context_లోపల_filter_cmd(expand_T *xp, char_u *arg)
{
    ఒకవేళ (*arg != NUL)
	arg = skip_vimgrep_pat(arg, ఖాళీ, ఖాళీ);
    ఒకవేళ (arg == ఖాళీ || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	తిరిగి ఖాళీ;
    }
    తిరిగి skipwhite(arg);
}

#నిర్వచితమైతే FEAT_శోధన_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    స్థిర char_u *
సమితి_context_లోపల_సరిపోలిక_cmd(expand_T *xp, char_u *arg)
{
    ఒకవేళ (*arg == NUL || !చివర్లు_excmd(*arg))
    {
	// also complete "None"
	సమితి_context_లోపల_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	ఒకవేళ (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    తిరిగి కనుగొను_nextcmd(arg);
}
#అయితే_ముగింపు

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    స్థిర char_u *
కనుగొను_cmd_after_global_cmd(char_u *arg)
{
    పూర్ణాంకం		delim;

    delim = *arg;	    // get the delimiter
    ఒకవేళ (delim)
	++arg;		    // skip delimiter if there is one

    అయితే (arg[0] != NUL && arg[0] != delim)
    {
	ఒకవేళ (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    ఒకవేళ (arg[0] != NUL)
	తిరిగి arg + 1;

    తిరిగి ఖాళీ;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    స్థిర char_u *
కనుగొను_cmd_after_substitute_cmd(char_u *arg)
{
    పూర్ణాంకం		delim;

    delim = *arg;
    ఒకవేళ (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	ఒకవేళ (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    అయితే (arg[0] != NUL && arg[0] != delim)
	    {
		ఒకవేళ (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    ఒకవేళ (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    అయితే (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == ఖాళీ)
	++arg;
    ఒకవేళ (arg[0] != NUL)
	తిరిగి arg;

    తిరిగి ఖాళీ;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    స్థిర char_u *
కనుగొను_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    ఒకవేళ (*arg != '/')
	తిరిగి ఖాళీ;

    // Match regexp, not just whole words
    కొరకు (++arg; *arg && *arg != '/'; arg++)
	ఒకవేళ (*arg == '\\' && arg[1] != NUL)
	    arg++;
    ఒకవేళ (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	ఒకవేళ (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == ఖాళీ)
	    xp->xp_context = EXPAND_NOTHING;
	లేకపోతే
	    తిరిగి arg;
    }

    తిరిగి ఖాళీ;
}

#నిర్వచితమైతే FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    స్థిర char_u *
సమితి_context_లోపల_unlet_cmd(expand_T *xp, char_u *arg)
{
    అయితే ((xp->xp_నమూనా = vim_strchr(arg, ' ')) != ఖాళీ)
	arg = xp->xp_నమూనా + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_నమూనా = arg;

    ఒకవేళ (*xp->xp_నమూనా == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_నమూనా;
    }

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    స్థిర char_u *
సమితి_context_లోపల_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    ఒకవేళ (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_నమూనా = arg;
    }
    లేకపోతే
    {
	ఒకవేళ ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_నమూనా = skipwhite(p);
	}
	లేకపోతే
	    xp->xp_context = EXPAND_NOTHING;
    }

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

స్థిర గణన
{
    EXP_FILETYPECMD_ALL,	// expand all :filetype values
    EXP_FILETYPECMD_PLUGIN,	// expand plugin on off
    EXP_FILETYPECMD_INDENT,	// expand indent on off
    EXP_FILETYPECMD_ONOFF,	// expand on off
} filetype_expand_what;

#నిర్వచించు EXPAND_FILETYPECMD_PLUGIN 0x01
#నిర్వచించు EXPAND_FILETYPECMD_INDENT 0x02
#నిర్వచించు EXPAND_FILETYPECMD_ONOFF  0x04

#నిర్వచితమైతే FEAT_EVAL
స్థిర గణన
{
    EXP_BREAKPT_కూడు,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    స్థిర char_u *
సమితి_context_లోపల_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_ప్రారంభం;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_నమూనా = arg;

    ఒకవేళ (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_కూడు;
    లేకపోతే ఒకవేళ (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    లేకపోతే
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    ఒకవేళ (*p == NUL)
	తిరిగి ఖాళీ;
    subcmd_ప్రారంభం = p;

    ఒకవేళ (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	ఒకవేళ (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    ఒకవేళ (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		తిరిగి ఖాళీ;
	    }
	    p = skipwhite(p);
	}
	ఒకవేళ (STRNCMP("file", subcmd_ప్రారంభం, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	లేకపోతే
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_నమూనా = p;
    }
    లేకపోతే ఒకవేళ (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_వ్యక్తీకరణ;
	xp->xp_నమూనా = skipwhite(p + 5);
    }

    తిరిగి ఖాళీ;
}

    స్థిర char_u *
సమితి_context_లోపల_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_నమూనా = ఖాళీ;

    p = skipwhite(arg);
    ఒకవేళ (VIM_ISDIGIT(*p))
	తిరిగి ఖాళీ;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_నమూనా = p;

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    స్థిర char_u *
సమితి_context_లోపల_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    పూర్ణాంకం    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_నమూనా = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    ఒకవేళ (*p == NUL)
	తిరిగి ఖాళీ;

    కొరకు (;;)
    {
	ఒకవేళ (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    కొనసాగు;
	}
	ఒకవేళ (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    కొనసాగు;
	}
	ఆపు;
    }

    ఒకవేళ ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    లేకపోతే ఒకవేళ ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    లేకపోతే ఒకవేళ ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_నమూనా = p;

    తిరిగి ఖాళీ;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    స్థిర శూన్య
సమితి_context_with_నమూనా(expand_T *xp)
{
    పూర్ణాంకం		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    పూర్ణాంకం		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_నమూనా_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    ఒకవేళ (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	తిరిగి;
#అయితే_ముగింపు

    xp->xp_నమూనా = ccline->cmdbuff + skiplen;
    xp->xp_నమూనా_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_నమూనా_లోపల_BUF;
    xp->xp_శోధన_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    స్థిర char_u *
సమితి_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	దీర్ఘ		argt,
	పూర్ణాంకం		compl,
	పూర్ణాంకం		forceit)
{
    char_u  *nextcmd;

    ఎంపిక (cmdidx)
    {
	స్థితి CMD_కనుగొను:
	స్థితి CMD_sfind:
	స్థితి CMD_tabfind:
	    ఒకవేళ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_లోపల_మార్గం;
	    ఆపు;
	స్థితి CMD_cd:
	స్థితి CMD_chdir:
	స్థితి CMD_tcd:
	స్థితి CMD_tchdir:
	స్థితి CMD_lcd:
	స్థితి CMD_lchdir:
	    ఒకవేళ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_లోపల_CDPATH;
	    ఆపు;
	స్థితి CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_నమూనా = arg;
	    ఆపు;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	స్థితి CMD_aboveleft:
	స్థితి CMD_argdo:
	స్థితి CMD_belowright:
	స్థితి CMD_botright:
	స్థితి CMD_browse:
	స్థితి CMD_bufdo:
	స్థితి CMD_cdo:
	స్థితి CMD_cfdo:
	స్థితి CMD_confirm:
	స్థితి CMD_debug:
	స్థితి CMD_folddoclosed:
	స్థితి CMD_folddoopen:
	స్థితి CMD_hide:
	స్థితి CMD_horizontal:
	స్థితి CMD_keepalt:
	స్థితి CMD_keepjumps:
	స్థితి CMD_keepmarks:
	స్థితి CMD_keeppatterns:
	స్థితి CMD_ldo:
	స్థితి CMD_leftabove:
	స్థితి CMD_lfdo:
	స్థితి CMD_lockmarks:
	స్థితి CMD_noautocmd:
	స్థితి CMD_noswapfile:
	స్థితి CMD_rightbelow:
	స్థితి CMD_sandbox:
	స్థితి CMD_silent:
	స్థితి CMD_tab:
	స్థితి CMD_tabdo:
	స్థితి CMD_topleft:
	స్థితి CMD_unsilent:
	స్థితి CMD_verbose:
	స్థితి CMD_vertical:
	స్థితి CMD_windo:
	స్థితి CMD_vim9cmd:
	స్థితి CMD_legacy:
	    తిరిగి arg;

	స్థితి CMD_filter:
	    తిరిగి సమితి_context_లోపల_filter_cmd(xp, arg);

#నిర్వచితమైతే FEAT_శోధన_EXTRA
	స్థితి CMD_సరిపోలిక:
	    తిరిగి సమితి_context_లోపల_సరిపోలిక_cmd(xp, arg);
#అయితే_ముగింపు

	// All completion for the +cmdline_compl feature goes here.

	స్థితి CMD_command:
	    తిరిగి సమితి_context_లోపల_user_cmd(xp, arg);

	స్థితి CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_నమూనా = arg;
	    ఆపు;

	స్థితి CMD_global:
	స్థితి CMD_vglobal:
	    nextcmd = కనుగొను_cmd_after_global_cmd(arg);
	    ఒకవేళ (!nextcmd && may_expand_నమూనా)
		సమితి_context_with_నమూనా(xp);
	    తిరిగి nextcmd;

	స్థితి CMD_and:
	స్థితి CMD_substitute:
	    nextcmd = కనుగొను_cmd_after_substitute_cmd(arg);
	    ఒకవేళ (!nextcmd && may_expand_నమూనా)
		సమితి_context_with_నమూనా(xp);
	    తిరిగి nextcmd;

	స్థితి CMD_isearch:
	స్థితి CMD_dsearch:
	స్థితి CMD_ilist:
	స్థితి CMD_dlist:
	స్థితి CMD_ijump:
	స్థితి CMD_psearch:
	స్థితి CMD_djump:
	స్థితి CMD_isplit:
	స్థితి CMD_dsplit:
	    తిరిగి కనుగొను_cmd_after_isearch_cmd(xp, arg);
	స్థితి CMD_autocmd:
	    తిరిగి సమితి_context_లోపల_autocmd(xp, arg, FALSE);
	స్థితి CMD_doautocmd:
	స్థితి CMD_doautoall:
	    తిరిగి సమితి_context_లోపల_autocmd(xp, arg, TRUE);
	స్థితి CMD_సమితి:
	    సమితి_context_లోపల_సమితి_cmd(xp, arg, 0);
	    ఆపు;
	స్థితి CMD_setglobal:
	    సమితి_context_లోపల_సమితి_cmd(xp, arg, OPT_GLOBAL);
	    ఆపు;
	స్థితి CMD_setlocal:
	    సమితి_context_లోపల_సమితి_cmd(xp, arg, OPT_LOCAL);
	    ఆపు;
	స్థితి CMD_ట్యాగ్:
	స్థితి CMD_stag:
	స్థితి CMD_ptag:
	స్థితి CMD_ltag:
	స్థితి CMD_tselect:
	స్థితి CMD_stselect:
	స్థితి CMD_ptselect:
	స్థితి CMD_tjump:
	స్థితి CMD_stjump:
	స్థితి CMD_ptjump:
	    ఒకవేళ (vim_strchr(p_wop, WOP_TAGFILE) != ఖాళీ)
		xp->xp_context = EXPAND_ట్యాగులు_LISTFILES;
	    లేకపోతే
		xp->xp_context = EXPAND_ట్యాగులు;
	    xp->xp_నమూనా = arg;
	    ఆపు;
	స్థితి CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_నమూనా = arg;
	    ఆపు;
#నిర్వచితమైతే FEAT_SYN_HL
	స్థితి CMD_syntax:
	    సమితి_context_లోపల_syntax_cmd(xp, arg);
	    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	స్థితి CMD_final:
	స్థితి CMD_const:
	స్థితి CMD_let:
	స్థితి CMD_var:
	స్థితి CMD_if:
	స్థితి CMD_elseif:
	స్థితి CMD_while:
	స్థితి CMD_for:
	స్థితి CMD_echo:
	స్థితి CMD_echon:
	స్థితి CMD_execute:
	స్థితి CMD_echomsg:
	స్థితి CMD_echoerr:
	స్థితి CMD_echoconsole:
	స్థితి CMD_echowindow:
	స్థితి CMD_call:
	స్థితి CMD_return:
	స్థితి CMD_cexpr:
	స్థితి CMD_caddexpr:
	స్థితి CMD_cgetexpr:
	స్థితి CMD_lexpr:
	స్థితి CMD_laddexpr:
	స్థితి CMD_lgetexpr:
	    సమితి_context_for_వ్యక్తీకరణ(xp, arg, cmdidx);
	    ఆపు;

	స్థితి CMD_unlet:
	    తిరిగి సమితి_context_లోపల_unlet_cmd(xp, arg);
	స్థితి CMD_function:
	స్థితి CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_నమూనా = arg;
	    ఆపు;
	స్థితి CMD_disassemble:
	    సమితి_context_లోపల_disassemble_cmd(xp, arg);
	    ఆపు;

	స్థితి CMD_echohl:
	    సమితి_context_లోపల_echohl_cmd(xp, arg);
	    ఆపు;
#అయితే_ముగింపు
	స్థితి CMD_highlight:
	    సమితి_context_లోపల_highlight_cmd(xp, arg);
	    ఆపు;
#నిర్వచితమైతే FEAT_CSCOPE
	స్థితి CMD_cscope:
	స్థితి CMD_lcscope:
	స్థితి CMD_scscope:
	    సమితి_context_లోపల_cscope_cmd(xp, arg, cmdidx);
	    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
	స్థితి CMD_sign:
	    సమితి_context_లోపల_sign_cmd(xp, arg);
	    ఆపు;
#అయితే_ముగింపు
	స్థితి CMD_bdelete:
	స్థితి CMD_bwipeout:
	స్థితి CMD_bunload:
	    అయితే ((xp->xp_నమూనా = vim_strchr(arg, ' ')) != ఖాళీ)
		arg = xp->xp_నమూనా + 1;
	    // FALLTHROUGH
	స్థితి CMD_బఫరు:
	స్థితి CMD_sbuffer:
	స్థితి CMD_pbuffer:
	స్థితి CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_నమూనా = arg;
	    ఆపు;
#నిర్వచితమైతే FEAT_DIFF
	స్థితి CMD_diffget:
	స్థితి CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_నమూనా = arg;
	    ఆపు;
#అయితే_ముగింపు
	స్థితి CMD_USER:
	స్థితి CMD_USER_BUF:
	    తిరిగి సమితి_context_లోపల_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	స్థితి CMD_map:	    స్థితి CMD_noremap:
	స్థితి CMD_nmap:	    స్థితి CMD_nnoremap:
	స్థితి CMD_vmap:	    స్థితి CMD_vnoremap:
	స్థితి CMD_omap:	    స్థితి CMD_onoremap:
	స్థితి CMD_imap:	    స్థితి CMD_inoremap:
	స్థితి CMD_cmap:	    స్థితి CMD_cnoremap:
	స్థితి CMD_lmap:	    స్థితి CMD_lnoremap:
	స్థితి CMD_smap:	    స్థితి CMD_snoremap:
	స్థితి CMD_tmap:	    స్థితి CMD_tnoremap:
	స్థితి CMD_xmap:	    స్థితి CMD_xnoremap:
	    తిరిగి సమితి_context_లోపల_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	స్థితి CMD_unmap:
	స్థితి CMD_nunmap:
	స్థితి CMD_vunmap:
	స్థితి CMD_ounmap:
	స్థితి CMD_iunmap:
	స్థితి CMD_cunmap:
	స్థితి CMD_lunmap:
	స్థితి CMD_sunmap:
	స్థితి CMD_tunmap:
	స్థితి CMD_xunmap:
	    తిరిగి సమితి_context_లోపల_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	స్థితి CMD_mapclear:
	స్థితి CMD_nmapclear:
	స్థితి CMD_vmapclear:
	స్థితి CMD_omapclear:
	స్థితి CMD_imapclear:
	స్థితి CMD_cmapclear:
	స్థితి CMD_lmapclear:
	స్థితి CMD_smapclear:
	స్థితి CMD_tmapclear:
	స్థితి CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_నమూనా = arg;
	    ఆపు;

	స్థితి CMD_abbreviate:	స్థితి CMD_noreabbrev:
	స్థితి CMD_cabbrev:	స్థితి CMD_cnoreabbrev:
	స్థితి CMD_iabbrev:	స్థితి CMD_inoreabbrev:
	    తిరిగి సమితి_context_లోపల_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	స్థితి CMD_unabbreviate:
	స్థితి CMD_cunabbrev:
	స్థితి CMD_iunabbrev:
	    తిరిగి సమితి_context_లోపల_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#నిర్వచితమైతే FEAT_MENU
	స్థితి CMD_menu:	    స్థితి CMD_noremenu:	    స్థితి CMD_unmenu:
	స్థితి CMD_amenu:	    స్థితి CMD_anoremenu:	    స్థితి CMD_aunmenu:
	స్థితి CMD_nmenu:	    స్థితి CMD_nnoremenu:	    స్థితి CMD_nunmenu:
	స్థితి CMD_vmenu:	    స్థితి CMD_vnoremenu:	    స్థితి CMD_vunmenu:
	స్థితి CMD_omenu:	    స్థితి CMD_onoremenu:	    స్థితి CMD_ounmenu:
	స్థితి CMD_imenu:	    స్థితి CMD_inoremenu:	    స్థితి CMD_iunmenu:
	స్థితి CMD_cmenu:	    స్థితి CMD_cnoremenu:	    స్థితి CMD_cunmenu:
	స్థితి CMD_tlmenu:    స్థితి CMD_tlnoremenu:    స్థితి CMD_tlunmenu:
	స్థితి CMD_tmenu:				    స్థితి CMD_tunmenu:
	స్థితి CMD_popup:	    స్థితి CMD_tearoff:	    స్థితి CMD_emenu:
	    తిరిగి సమితి_context_లోపల_menu_cmd(xp, cmd, arg, forceit);
#అయితే_ముగింపు

	స్థితి CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_నమూనా = arg;
	    ఆపు;

	స్థితి CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_నమూనా = arg;
	    ఆపు;

	స్థితి CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_నమూనా = arg;
	    ఆపు;

	స్థితి CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_నమూనా = arg;
	    ఆపు;

	స్థితి CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_నమూనా = arg;
	    ఆపు;

	స్థితి CMD_runtime:
	    సమితి_context_లోపల_runtime_cmd(xp, arg);
	    ఆపు;

#ఒకవేళ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	స్థితి CMD_language:
	    తిరిగి సమితి_context_లోపల_lang_cmd(xp, arg);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PROFILE)
	స్థితి CMD_profile:
	    సమితి_context_లోపల_profile_cmd(xp, arg);
	    ఆపు;
#అయితే_ముగింపు
	స్థితి CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_నమూనా = arg;
	    ఆపు;

	స్థితి CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_నమూనా = arg;
	    ఆపు;

	స్థితి CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_నమూనా = arg;
	    ఆపు;

	స్థితి CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_నమూనా = arg;
	    ఆపు;
#ఒకవేళ defined(FEAT_PROFILE)
	స్థితి CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_నమూనా = arg;
	    ఆపు;
#అయితే_ముగింపు

	స్థితి CMD_argdelete:
	    అయితే ((xp->xp_నమూనా = vim_strchr(arg, ' ')) != ఖాళీ)
		arg = xp->xp_నమూనా + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_నమూనా = arg;
	    ఆపు;

#నిర్వచితమైతే FEAT_EVAL
	స్థితి CMD_breakadd:
	స్థితి CMD_profdel:
	స్థితి CMD_breakdel:
	    తిరిగి సమితి_context_లోపల_breakadd_cmd(xp, arg, cmdidx);

	స్థితి CMD_scriptnames:
	    తిరిగి సమితి_context_లోపల_scriptnames_cmd(xp, arg);
#అయితే_ముగింపు
	స్థితి CMD_filetype:
	    తిరిగి సమితి_context_లోపల_filetype_cmd(xp, arg);

	అప్రమేయ:
	    ఆపు;
    }
    తిరిగి ఖాళీ;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    స్థిర char_u *
సమితి_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    పూర్ణాంకం			len = 0;
    exarg_T		ea;
    పూర్ణాంకం			compl = EXPAND_NOTHING;
    పూర్ణాంకం			forceit = FALSE;
    పూర్ణాంకం			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_నమూనా = buff;
    xp->xp_పంక్తి = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    కొరకు (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != ఖాళీ; cmd++)
	;
    xp->xp_నమూనా = cmd;

    ఒకవేళ (*cmd == NUL)
	తిరిగి ఖాళీ;
    ఒకవేళ (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	తిరిగి ఖాళీ;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_నమూనా = cmd;
    ఒకవేళ (*cmd == NUL)
	తిరిగి ఖాళీ;
    ఒకవేళ (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (*cmd == '|' || *cmd == '\n')
	తిరిగి cmd + 1;			// There's another command

    // Get the command index.
    p = సమితి_cmd_సూచీ(cmd, &ea, xp, &compl);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    ఒకవేళ (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    ఒకవేళ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    ఒకవేళ ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	అయితే (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    అయితే (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    ఒకవేళ (*p == NUL)
	    {
		ఒకవేళ (ea.argt & EX_ARGOPT)
		    తిరిగి సమితి_context_లోపల_argopt(xp, arg + 2);
#నిర్వచితమైతే FEAT_TERMINAL
		ఒకవేళ (ea.cmdidx == CMD_terminal)
		    తిరిగి సమితి_context_లోపల_terminalopt(xp, arg + 2);
#అయితే_ముగింపు
	    }

	    arg = skipwhite(p);
	}
    }

    ఒకవేళ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	ఒకవేళ (*arg == '>')			// append
	{
	    ఒకవేళ (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	లేకపోతే ఒకవేళ (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    ఒకవేళ (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	ఒకవేళ (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    ఒకవేళ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	అయితే (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    ఒకవేళ ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	ఒకవేళ (*arg == NUL)
	    తిరిగి p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    ఒకవేళ ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	ఒకవేళ (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	అయితే (*p)
	{
	    ఒకవేళ (*p == Ctrl_V)
	    {
		ఒకవేళ (p[1] != NUL)
		    ++p;
	    }
	    లేకపోతే ఒకవేళ ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		ఒకవేళ (*(p - 1) != '\\')
		{
		    ఒకవేళ (*p == '|' || *p == '\n')
			తిరిగి p + 1;
		    తిరిగి ఖాళీ;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    ఒకవేళ (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == ఖాళీ)
	// no arguments allowed but there is something
	తిరిగి ఖాళీ;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_నమూనా = p;
    len = (పూర్ణాంకం)STRLEN(buff);
    అయితే (*p && p < buff + len)
    {
	ఒకవేళ (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_నమూనా = ++p;
	}
	లేకపోతే
	{
	    ఒకవేళ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    ఒకవేళ (ea.argt & EX_XFILE)
	సమితి_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    తిరిగి సమితి_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    శూన్య
సమితి_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    పూర్ణాంకం		len,	    // length of command line (excl. NUL)
    పూర్ణాంకం		నిలువు,	    // position of cursor
    పూర్ణాంకం		use_ccline వాడని) // use ccline for info
{
#నిర్వచితమైతే FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    పూర్ణాంకం			context;
#అయితే_ముగింపు
    పూర్ణాంకం		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    ఒకవేళ (నిలువు < len)
    {
	old_char = str[నిలువు];
	str[నిలువు] = NUL;
    }
    nextcomm = str;

    filetype_expand_what = EXP_FILETYPECMD_ALL;
#నిర్వచితమైతే FEAT_EVAL
    breakpt_expand_what = EXP_BREAKPT_కూడు;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	సమితి_context_for_వ్యక్తీకరణ(xp, str, CMD_కొలత);
    }
    లేకపోతే ఒకవేళ (use_ccline && ccline->ఆదానం_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_నమూనా = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	ఒకవేళ (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    సమితి_context_for_wildcard_arg(ఖాళీ, xp->xp_నమూనా, FALSE, xp,
								     &context);
	}
    }
    లేకపోతే
#అయితే_ముగింపు
	అయితే (nextcomm != ఖాళీ)
	    nextcomm = సమితి_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_పంక్తి = str;
    xp->xp_నిలువు = నిలువు;

    ఒకవేళ (నిలువు < len)
	str[నిలువు] = old_char;
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
    పూర్ణాంకం
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    పూర్ణాంకం		నిలువు,		// position of cursor
    పూర్ణాంకం		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = ఖాళీ;
    పూర్ణాంకం		options = WILD_కూడు_SLASH|WILD_SILENT;

    ఒకవేళ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	తిరిగి EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    ఒకవేళ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	తిరిగి EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_నమూనా_len = (పూర్ణాంకం)(str + నిలువు - xp->xp_నమూనా);
    {
	char_u	*pat_src = xp->xp_నమూనా;
	పూర్ణాంకం	pat_len = xp->xp_నమూనా_len;
#ఒకవేళ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	ఒకవేళ (unesc != ఖాళీ)
	    pat_src = unesc;
#అయితే_ముగింపు

	ఒకవేళ (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	లేకపోతే
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#ఒకవేళ defined(FEAT_EVAL)
	vim_free(unesc);
#అయితే_ముగింపు
    }
    ఒకవేళ (file_str == ఖాళీ)
	తిరిగి EXPAND_UNSUCCESSFUL;

    ఒకవేళ (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    ఒకవేళ (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = ఖాళీ;
    }
    vim_free(file_str);

    తిరిగి EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	పూర్ణాంకం		*అంకె_Matches,
	పూర్ణాంకం		flags,
	పూర్ణాంకం		options)
{
    పూర్ణాంకం		free_pat = FALSE;
    పూర్ణాంకం		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    ఒకవేళ (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_చివర;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	ఒకవేళ (pat == ఖాళీ)
	    తిరిగి ret;

	pat_చివర = pat + pat_len;
	కొరకు (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    ఒకవేళ (*p != '\\')
		కొనసాగు;

	    ఒకవేళ (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_చివర - from) + 1);   // +1 for NUL
		pat_చివర -= 3;
	    }
	    లేకపోతే ఒకవేళ (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_చివర - from) + 1);   // +1 for NUL
		--pat_చివర;
	    }
	    లేకపోతే ఒకవేళ (xp->xp_backslash & XP_BS_COMMA)
	    {
		ఒకవేళ (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_చివర - from) + 1);   // +1 for NUL
		    pat_చివర -= 2;
		}
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
		లేకపోతే ఒకవేళ (*(p + 1) == ',')
		{
		    from = p + 1;
		    mch_memmove(p, from,
			(size_t)(pat_చివర - from) + 1);   // +1 for NUL
		    --pat_చివర;
		}
#అయితే_ముగింపు
	    }
	}
    }

    ఒకవేళ (xp->xp_context == EXPAND_FINDFUNC)
    {
#నిర్వచితమైతే FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, అంకె_Matches);
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	ఒకవేళ (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	లేకపోతే ఒకవేళ (xp->xp_context == EXPAND_FILES_లోపల_మార్గం)
	    flags |= (EW_FILE | EW_మార్గం);
	లేకపోతే ఒకవేళ (xp->xp_context == EXPAND_DIRS_లోపల_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	లేకపోతే
	    flags = (flags | EW_DIR) & ~EW_FILE;
	ఒకవేళ (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, అంకె_Matches, matches, flags);
    }
    ఒకవేళ (free_pat)
	vim_free(pat);
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    ఒకవేళ (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	పూర్ణాంకం j;

	కొరకు (j = 0; j < *అంకె_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    అయితే (*ptr != NUL)
	    {
		ఒకవేళ (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		లేకపోతే ఒకవేళ (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#అయితే_ముగింపు
    తిరిగి ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    స్థిర char_u *
get_behave_arg(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx == 0)
	తిరిగి (char_u *)"mswin";
    ఒకవేళ (idx == 1)
	తిరిగి (char_u *)"xterm";
    తిరిగి ఖాళీ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    స్థిర char_u *
get_filetypecmd_arg(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx < 0)
	తిరిగి ఖాళీ;

    ఒకవేళ (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	అక్షరం	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	తిరిగి (char_u *)opts_all[idx];
    }
    ఒకవేళ (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	అక్షరం	*opts_plugin[] = {"plugin", "on", "off"};
	తిరిగి (char_u *)opts_plugin[idx];
    }
    ఒకవేళ (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	అక్షరం	*opts_indent[] = {"indent", "on", "off"};
	తిరిగి (char_u *)opts_indent[idx];
    }
    ఒకవేళ (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	అక్షరం	*opts_onoff[] = {"on", "off"};
	తిరిగి (char_u *)opts_onoff[idx];
    }
    తిరిగి ఖాళీ;
}

#నిర్వచితమైతే FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    స్థిర char_u *
get_breakadd_arg(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx >= 0 && idx <= 3)
    {
	అక్షరం	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	ఒకవేళ (breakpt_expand_what == EXP_BREAKPT_కూడు)
	    తిరిగి (char_u *)opts[idx];
	లేకపోతే ఒకవేళ (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    ఒకవేళ (idx <= 2)
		తిరిగి (char_u *)opts[idx + 1];
	}
	లేకపోతే
	{
	    // profdel {func, file}
	    ఒకవేళ (idx <= 1)
		తిరిగి (char_u *)opts[idx + 1];
	}
    }
    తిరిగి ఖాళీ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    స్థిర char_u *
get_scriptnames_arg(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    scriptitem_T *si;

    ఒకవేళ (!SCRIPT_ID_VALID(idx + 1))
	తిరిగి ఖాళీ;

    si = SCRIPT_అంశం(idx + 1);
    home_replace(ఖాళీ, si->sn_name, NameBuff, MAXPATHL, TRUE);
    తిరిగి NameBuff;
}
#అయితే_ముగింపు

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    స్థిర char_u *
get_retab_arg(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx == 0)
	తిరిగి (char_u *)"-indentonly";
    తిరిగి ఖాళీ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    స్థిర char_u *
get_messages_arg(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx == 0)
	తిరిగి (char_u *)"clear";
    తిరిగి ఖాళీ;
}

    స్థిర char_u *
get_mapclear_arg(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx == 0)
	తిరిగి (char_u *)"<buffer>";
    తిరిగి ఖాళీ;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    స్థిర పూర్ణాంకం
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	పూర్ణాంకం		*అంకె_Matches)
{
    స్థిర నిర్మాణం expgen
    {
	పూర్ణాంకం		context;
	char_u	*((*func)(expand_T *, పూర్ణాంకం));
	పూర్ణాంకం		ic;
	పూర్ణాంకం		escaped;
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
#నిర్వచితమైతే FEAT_EVAL
	{EXPAND_USER_VARS, get_user_var_name, FALSE, TRUE},
	{EXPAND_FUNCTIONS, get_function_name, FALSE, TRUE},
	{EXPAND_USER_FUNC, get_user_func_name, FALSE, TRUE},
	{EXPAND_DISASSEMBLE, get_disassemble_వాదం, FALSE, TRUE},
	{EXPAND_వ్యక్తీకరణ, get_సమాసం_name, FALSE, TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MENU
	{EXPAND_MENUS, get_menu_name, FALSE, TRUE},
	{EXPAND_MENUNAMES, get_menu_పేర్లు, FALSE, TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
	{EXPAND_SYNTAX, get_syntax_name, TRUE, TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROFILE
	{EXPAND_SYNTIME, get_syntime_arg, TRUE, TRUE},
#అయితే_ముగింపు
	{EXPAND_HIGHLIGHT, get_highlight_name, TRUE, TRUE},
	{EXPAND_EVENTS, get_event_name, TRUE, FALSE},
	{EXPAND_AUGROUP, get_augroup_name, TRUE, FALSE},
#నిర్వచితమైతే FEAT_CSCOPE
	{EXPAND_CSCOPE, get_cscope_name, TRUE, TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
	{EXPAND_SIGN, get_sign_name, TRUE, TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROFILE
	{EXPAND_PROFILE, get_profile_name, TRUE, TRUE},
#అయితే_ముగింపు
#ఒకవేళ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	{EXPAND_LANGUAGE, get_lang_arg, TRUE, FALSE},
	{EXPAND_LOCALES, get_locales, TRUE, FALSE},
#అయితే_ముగింపు
	{EXPAND_ENV_VARS, get_env_name, TRUE, TRUE},
	{EXPAND_USER, get_users, TRUE, FALSE},
	{EXPAND_ARGLIST, get_arglist_name, TRUE, FALSE},
#నిర్వచితమైతే FEAT_EVAL
	{EXPAND_BREAKPOINT, get_breakadd_arg, TRUE, TRUE},
	{EXPAND_SCRIPTNAMES, get_scriptnames_arg, TRUE, FALSE},
#అయితే_ముగింపు
	{EXPAND_RETAB, get_retab_arg, TRUE, TRUE},
    };
    పూర్ణాంకం	i;
    పూర్ణాంకం ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(tab); ++i)
    {
	ఒకవేళ (xp->xp_context == tab[i].context)
	{
	    ఒకవేళ (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, అంకె_Matches,
						tab[i].func, tab[i].escaped);
	    ఆపు;
	}
    }

    తిరిగి ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    స్థిర పూర్ణాంకం
map_wildopts_గమ్యం_ewflags(పూర్ణాంకం options)
{
    పూర్ణాంకం		flags;

    flags = EW_DIR;	// include directories
    ఒకవేళ (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    ఒకవేళ (options & WILD_కూడు_SLASH)
	flags |= EW_ADDSLASH;
    ఒకవేళ (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    ఒకవేళ (options & WILD_SILENT)
	flags |= EW_SILENT;
    ఒకవేళ (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    ఒకవేళ (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    తిరిగి flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    స్థిర పూర్ణాంకం
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    పూర్ణాంకం		*అంకె_Matches,
    పూర్ణాంకం		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    పూర్ణాంకం		ret;
    పూర్ణాంకం		flags;
    char_u	*tofree = ఖాళీ;
    పూర్ణాంకం		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_గమ్యం_ewflags(options);

    ఒకవేళ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_లోపల_మార్గం
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_లోపల_CDPATH)
	తిరిగి expand_files_and_dirs(xp, pat, matches, అంకె_Matches, flags,
								options);

    *matches = (char_u **)"";
    *అంకె_Matches = 0;
    ఒకవేళ (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	ఒకవేళ (కనుగొను_help_ట్యాగులు(*pat == NUL ? (char_u *)"help" : pat,
					అంకె_Matches, matches, FALSE) == OK)
	{
#నిర్వచితమైతే FEAT_MULTI_LANG
	    cleanup_help_ట్యాగులు(*అంకె_Matches, *matches);
#అయితే_ముగింపు
	    తిరిగి OK;
	}
	తిరిగి FAIL;
    }

    ఒకవేళ (xp->xp_context == EXPAND_SHELLCMD)
	తిరిగి expand_shellcmd(pat, matches, అంకె_Matches, flags);
    ఒకవేళ (xp->xp_context == EXPAND_OLD_SETTING)
	తిరిగి ExpandOldSetting(అంకె_Matches, matches);
    ఒకవేళ (xp->xp_context == EXPAND_BUFFERS)
	తిరిగి ExpandBufnames(pat, అంకె_Matches, matches, options);
#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (xp->xp_context == EXPAND_DIFF_BUFFERS)
	తిరిగి ExpandBufnames(pat, అంకె_Matches, matches,
						options | BUF_DIFF_FILTER);
#అయితే_ముగింపు
    ఒకవేళ (xp->xp_context == EXPAND_ట్యాగులు
	    || xp->xp_context == EXPAND_ట్యాగులు_LISTFILES)
	తిరిగి expand_ట్యాగులు(xp->xp_context == EXPAND_ట్యాగులు, pat, అంకె_Matches,
								matches);
    ఒకవేళ (xp->xp_context == EXPAND_COLORS)
    {
	అక్షరం *directories[] = {"colors", ఖాళీ};
	తిరిగి ExpandRTDir(pat, DIP_ప్రారంభం + DIP_OPT, అంకె_Matches, matches,
								directories);
    }
    ఒకవేళ (xp->xp_context == EXPAND_COMPILER)
    {
	అక్షరం *directories[] = {"compiler", ఖాళీ};
	తిరిగి ExpandRTDir(pat, 0, అంకె_Matches, matches, directories);
    }
    ఒకవేళ (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	అక్షరం *directories[] = {"syntax", ఖాళీ};
	తిరిగి ExpandRTDir(pat, 0, అంకె_Matches, matches, directories);
    }
    ఒకవేళ (xp->xp_context == EXPAND_FILETYPE)
    {
	అక్షరం *directories[] = {"syntax", "indent", "ftplugin", ఖాళీ};
	తిరిగి ExpandRTDir(pat, 0, అంకె_Matches, matches, directories);
    }
#నిర్వచితమైతే FEAT_KEYMAP
    ఒకవేళ (xp->xp_context == EXPAND_KEYMAP)
    {
	అక్షరం *directories[] = {"keymap", ఖాళీ};
	తిరిగి ExpandRTDir(pat, 0, అంకె_Matches, matches, directories);
    }
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ (xp->xp_context == EXPAND_USER_LIST)
	తిరిగి ExpandUserList(xp, matches, అంకె_Matches);
#అయితే_ముగింపు
    ఒకవేళ (xp->xp_context == EXPAND_PACKADD)
	తిరిగి Expand_Pack_కూడు_Dir(pat, అంకె_Matches, matches);
    ఒకవేళ (xp->xp_context == EXPAND_RUNTIME)
	తిరిగి expand_runtime_cmd(pat, అంకె_Matches, matches);
    ఒకవేళ (xp->xp_context == EXPAND_నమూనా_లోపల_BUF)
	తిరిగి expand_నమూనా_లోపల_buf(pat, xp->xp_శోధన_dir,
		matches, అంకె_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    ఒకవేళ ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	పూర్ణాంకం len = (పూర్ణాంకం)STRLEN(pat) + 20;

	tofree = alloc(len);
	ఒకవేళ (tofree == ఖాళీ)
	    తిరిగి FAIL;
	vim_snprintf((అక్షరం *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    ఒకవేళ (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	ఒకవేళ (regmatch.regprog == ఖాళీ)
	{
	    vim_free(tofree);
	    తిరిగి FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    ఒకవేళ (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, అంకె_Matches, matches, fuzzy);
    లేకపోతే ఒకవేళ (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, అంకె_Matches, matches);
    లేకపోతే ఒకవేళ (xp->xp_context == EXPAND_SETTING_వ్యవకలనం)
	ret = Expand_Setting_వ్యవకలనం(xp, &regmatch, అంకె_Matches, matches);
    లేకపోతే ఒకవేళ (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, అంకె_Matches, matches);
    లేకపోతే ఒకవేళ (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, అంకె_Matches);
    లేకపోతే ఒకవేళ (xp->xp_context == EXPAND_HIGHLIGHT_సమూహం)
	ret = expand_highlight_సమూహం(pat, xp, &regmatch, matches, అంకె_Matches);
#ఒకవేళ defined(FEAT_TERMINAL)
    లేకపోతే ఒకవేళ (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, అంకె_Matches);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_EVAL)
    లేకపోతే ఒకవేళ (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, అంకె_Matches);
#అయితే_ముగింపు
    లేకపోతే
	ret = ExpandOther(pat, xp, &regmatch, matches, అంకె_Matches);

    ఒకవేళ (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    తిరిగి ret;
}

    పూర్ణాంకం
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    పూర్ణాంకం		*అంకె_Matches,
    char_u	*(*func)(expand_T *, పూర్ణాంకం),
					  // returns a string from the list
    పూర్ణాంకం		escaped)
{
    తిరిగి ExpandGenericExt(
	pat, xp, regmatch, matches, అంకె_Matches, func, escaped, 0);
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
    పూర్ణాంకం
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    పూర్ణాంకం		*అంకె_Matches,
    char_u	*(*func)(expand_T *, పూర్ణాంకం),
					  // returns a string from the list
    పూర్ణాంకం		escaped,
    పూర్ణాంకం		క్రమీకరించు_ప్రారంభం_Idx)
{
    పూర్ణాంకం		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = ఖాళీ;
    పూర్ణాంకం		score = 0;
    పూర్ణాంకం		fuzzy;
    పూర్ణాంకం		సరిపోలిక;
    పూర్ణాంకం		క్రమీకరించు_matches = FALSE;
    పూర్ణాంకం		funcsort = FALSE;
    పూర్ణాంకం		క్రమీకరించు_ప్రారంభం_సరిపోలిక_Idx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ఖాళీ;
    *అంకె_Matches = 0;

    ఒకవేళ (!fuzzy)
	ga_init2(&ga, పరిమాణం(అక్షరం *), 30);
    లేకపోతే
	ga_init2(&ga, పరిమాణం(fuzmatch_str_T), 30);

    కొరకు (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	ఒకవేళ (str == ఖాళీ)	    // end of list
	    ఆపు;
	ఒకవేళ (*str == NUL)	    // skip empty strings
	    కొనసాగు;

	ఒకవేళ (xp->xp_నమూనా[0] != NUL)
	{
	    ఒకవేళ (!fuzzy)
		సరిపోలిక = vim_regexec(regmatch, str, (colnr_T)0);
	    లేకపోతే
	    {
		score = fuzzy_సరిపోలిక_str(str, pat);
		సరిపోలిక = (score != FUZZY_SCORE_NONE);
	    }
	}
	లేకపోతే
	    సరిపోలిక = TRUE;

	ఒకవేళ (!సరిపోలిక)
	    కొనసాగు;

	ఒకవేళ (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	లేకపోతే
	    str = vim_strsave(str);
	ఒకవేళ (str == ఖాళీ)
	{
	    ఒకవేళ (!fuzzy)
	    {
		ga_clear_అక్షరశ్రేణులు(&ga);
		తిరిగి FAIL;
	    }
	    fuzmatch_str_free(ga.ga_దత్తాంశం, ga.ga_len);
	    తిరిగి FAIL;
	}

	ఒకవేళ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    ఆపు;
	}

	ఒకవేళ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_దత్తాంశం)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	లేకపోతే
	    ((char_u **)ga.ga_దత్తాంశం)[ga.ga_len] = str;

#నిర్వచితమైతే FEAT_MENU
	ఒకవేళ (func == get_menu_పేర్లు)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    ఒకవేళ (*str == '\001')
		*str = '.';
	}
#అయితే_ముగింపు

	ఒకవేళ (క్రమీకరించు_ప్రారంభం_Idx >= 0 && i >= క్రమీకరించు_ప్రారంభం_Idx && క్రమీకరించు_ప్రారంభం_సరిపోలిక_Idx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    క్రమీకరించు_ప్రారంభం_సరిపోలిక_Idx = ga.ga_len;
	}

	++ga.ga_len;
    }

    ఒకవేళ (ga.ga_len == 0)
	తిరిగి OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    ఒకవేళ (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	క్రమీకరించు_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    ఒకవేళ (xp->xp_context == EXPAND_వ్యక్తీకరణ
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    ఒకవేళ (క్రమీకరించు_matches && క్రమీకరించు_ప్రారంభం_సరిపోలిక_Idx != -1)
    {
	ఒకవేళ (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((శూన్య *)ga.ga_దత్తాంశం, (size_t)ga.ga_len, పరిమాణం(char_u *),
							   క్రమీకరించు_func_compare);
	లేకపోతే
	    క్రమీకరించు_అక్షరశ్రేణులు((char_u **)ga.ga_దత్తాంశం + క్రమీకరించు_ప్రారంభం_సరిపోలిక_Idx, ga.ga_len - క్రమీకరించు_ప్రారంభం_సరిపోలిక_Idx);
    }

    ఒకవేళ (!fuzzy)
    {
	*matches = ga.ga_దత్తాంశం;
	*అంకె_Matches = ga.ga_len;
    }
    లేకపోతే
    {
	ఒకవేళ (fuzzymatches_గమ్యం_strmatches(ga.ga_దత్తాంశం, matches, ga.ga_len,
							funcsort) == FAIL)
	    తిరిగి FAIL;
	*అంకె_Matches = ga.ga_len;
    }

#ఒకవేళ defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#అయితే_ముగింపు

    తిరిగి OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    స్థిర శూన్య
expand_shellcmd_onedir(
	char_u		*pathed_నమూనా,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	పూర్ణాంకం		*అంకె_Matches,
	పూర్ణాంకం		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    ఒకవేళ (expand_wildcards(1, &pathed_నమూనా, అంకె_Matches, matches, flags) != OK)
	తిరిగి;

    ఒకవేళ (ga_grow(gap, *అంకె_Matches) == FAIL)
    {
	FreeWild(*అంకె_Matches, *matches);
	తిరిగి;
    }

    కొరకు (పూర్ణాంకం i = 0; i < *అంకె_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	ఒకవేళ (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_దత్తాంశం)[gap->ga_len++] = name;
		hash_కూడు_అంశం(ht, hi, name, hash);
		name = ఖాళీ;
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
    స్థిర పూర్ణాంకం
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    పూర్ణాంకం		*అంకె_Matches,	// return: number of matches
    పూర్ణాంకం		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*మార్గం = ఖాళీ;
    పూర్ణాంకం		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    పూర్ణాంకం		flags = flagsarg;
    పూర్ణాంకం		did_curdir = FALSE;
    hashtab_T	దొరికింది_ht;

    buf = alloc(MAXPATHL);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    ఒకవేళ (pat == ఖాళీ)
    {
	vim_free(buf);
	తిరిగి FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    కొరకు (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	ఒకవేళ (*s != '\\')
	    కొనసాగు;

	p = s + 1;
	ఒకవేళ (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    ఒకవేళ (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	మార్గం = (char_u *)".";
    లేకపోతే
    {
	// For an absolute name we don't use $PATH.
	ఒకవేళ (!mch_is_నిండిన_Name(pat))
	    మార్గం = vim_getenv((char_u *)"PATH", &mustfree);
	ఒకవేళ (మార్గం == ఖాళీ)
	    మార్గం = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, పరిమాణం(అక్షరం *), 10);
    hash_init(&దొరికింది_ht);
    కొరకు (s = మార్గం; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	ఒకవేళ (*s == NUL)
	{
	    ఒకవేళ (did_curdir)
		ఆపు;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	లేకపోతే
	{
#ఒకవేళ defined(MSWIN)
	    e = vim_strchr(s, ';');
#లేకపోతే
	    e = vim_strchr(s, ':');
#అయితే_ముగింపు
	    ఒకవేళ (e == ఖాళీ)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    ఒకవేళ (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    లేకపోతే
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? పరిమాణం(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	ఒకవేళ (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    ఒకవేళ (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		ఒకవేళ (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, అంకె_Matches, flags,
				    &దొరికింది_ht, &ga);
	}

	ఒకవేళ (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_దత్తాంశం;
    *అంకె_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    ఒకవేళ (mustfree)
	vim_free(మార్గం);
    hash_clear(&దొరికింది_ht);
    తిరిగి OK;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    స్థిర శూన్య *
call_user_expand_func(
    శూన్య	*(*user_expand_func)(char_u *, పూర్ణాంకం, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    పూర్ణాంకం		keep = 0;
    typval_T	వాదనలు[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = ఖాళీ;
    శూన్య	*ret;

    ఒకవేళ (xp->xp_arg == ఖాళీ || xp->xp_arg[0] == '\0' || xp->xp_పంక్తి == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (ccline->cmdbuff != ఖాళీ)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	పూర్ణాంకం unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_నమూనా,
						  xp->xp_నమూనా_len, &unesc_len);
	ఒకవేళ (pat == ఖాళీ)
	    pat = vim_strnsave(xp->xp_నమూనా, xp->xp_నమూనా_len);
    }

    వాదనలు[0].v_type = VAR_STRING;
    వాదనలు[0].vval.v_string = pat;
    వాదనలు[1].v_type = VAR_STRING;
    వాదనలు[1].vval.v_string = xp->xp_పంక్తి;
    వాదనలు[2].v_type = VAR_సంఖ్య;
    వాదనలు[2].vval.v_సంఖ్య = xp->xp_నిలువు;
    వాదనలు[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, వాదనలు);

    current_sctx = save_current_sctx;
    ఒకవేళ (ccline->cmdbuff != ఖాళీ)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    తిరిగి ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    స్థిర పూర్ణాంకం
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    పూర్ణాంకం		*అంకె_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    పూర్ణాంకం		keep;
    garray_T	ga;
    పూర్ణాంకం		fuzzy;
    పూర్ణాంకం		సరిపోలిక;
    పూర్ణాంకం		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ఖాళీ;
    *అంకె_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    ఒకవేళ (retstr == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (!fuzzy)
	ga_init2(&ga, పరిమాణం(అక్షరం *), 3);
    లేకపోతే
	ga_init2(&ga, పరిమాణం(fuzmatch_str_T), 3);

    కొరకు (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	ఒకవేళ (e == ఖాళీ)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	ఒకవేళ (xp->xp_నమూనా[0] != NUL)
	{
	    ఒకవేళ (!fuzzy)
		సరిపోలిక = vim_regexec(regmatch, s, (colnr_T)0);
	    లేకపోతే
	    {
		score = fuzzy_సరిపోలిక_str(s, pat);
		సరిపోలిక = (score != FUZZY_SCORE_NONE);
	    }
	}
	లేకపోతే
	    సరిపోలిక = TRUE;		// match everything

	*e = keep;

	ఒకవేళ (సరిపోలిక)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    ఒకవేళ (p == ఖాళీ)
		ఆపు;

	    ఒకవేళ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		ఆపు;
	    }

	    ఒకవేళ (!fuzzy)
		((char_u **)ga.ga_దత్తాంశం)[ga.ga_len] = p;
	    లేకపోతే
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_దత్తాంశం)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	ఒకవేళ (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    ఒకవేళ (ga.ga_len == 0)
	తిరిగి OK;

    ఒకవేళ (!fuzzy)
    {
	*matches = ga.ga_దత్తాంశం;
	*అంకె_Matches = ga.ga_len;
    }
    లేకపోతే
    {
	ఒకవేళ (fuzzymatches_గమ్యం_strmatches(ga.ga_దత్తాంశం, matches, ga.ga_len,
								FALSE) == FAIL)
	    తిరిగి FAIL;
	*అంకె_Matches = ga.ga_len;
    }
    తిరిగి OK;
}

    శూన్య
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    పూర్ణాంకం		*అంకె_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_రకం;
    garray_T	ga_menu;
    garray_T	ga_info;
    పూర్ణాంకం		have_extra = FALSE;
    పూర్ణాంకం		i;

    ga_init2(&ga, పరిమాణం(అక్షరం *), 3);
    ga_init2(&ga_abbr, పరిమాణం(అక్షరం *), 3);
    ga_init2(&ga_రకం, పరిమాణం(అక్షరం *), 3);
    ga_init2(&ga_menu, పరిమాణం(అక్షరం *), 3);
    ga_init2(&ga_info, పరిమాణం(అక్షరం *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_వస్తువులు(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = ఖాళీ;
	char_u	*abbr = ఖాళీ;
	char_u	*రకం = ఖాళీ;
	char_u	*menu = ఖాళీ;
	char_u	*info = ఖాళీ;

	ఒకవేళ (tv->v_type == VAR_STRING)
	{
	    ఒకవేళ (tv->vval.v_string == ఖాళీ)
		కొనసాగు;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	లేకపోతే ఒకవేళ (tv->v_type == VAR_DICT && tv->vval.v_dict != ఖాళీ)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*మాట = dict_get_string(d, "word", FALSE);

	    ఒకవేళ (మాట == ఖాళీ)
		కొనసాగు;  // "word" is required
	    p = vim_strsave(మాట);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    రకం = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    ఒకవేళ (abbr != ఖాళీ || రకం != ఖాళీ || menu != ఖాళీ || info != ఖాళీ)
		have_extra = TRUE;
	}
	లేకపోతే
	    కొనసాగు;  // Skip other types

	ఒకవేళ (p == ఖాళీ
		|| ga_grow(&ga, 1) == FAIL
		|| ga_grow(&ga_abbr, 1) == FAIL
		|| ga_grow(&ga_రకం, 1) == FAIL
		|| ga_grow(&ga_menu, 1) == FAIL
		|| ga_grow(&ga_info, 1) == FAIL)
	{
	    vim_free(p);
	    vim_free(abbr);
	    vim_free(రకం);
	    vim_free(menu);
	    vim_free(info);
	    ఆపు;
	}

	((char_u **)ga.ga_దత్తాంశం)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_దత్తాంశం)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_రకం.ga_దత్తాంశం)[ga_రకం.ga_len++] = రకం;
	((char_u **)ga_menu.ga_దత్తాంశం)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_దత్తాంశం)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_దత్తాంశం;
    *అంకె_Matches = ga.ga_len;
    ఒకవేళ (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_దత్తాంశం;
	xp->xp_files_రకం = (char_u **)ga_రకం.ga_దత్తాంశం;
	xp->xp_files_menu = (char_u **)ga_menu.ga_దత్తాంశం;
	xp->xp_files_info = (char_u **)ga_info.ga_దత్తాంశం;
    }
    లేకపోతే
    {
	// No extra info collected; free the placeholder NULL entries.
	కొరకు (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_దత్తాంశం)[i]);
	vim_free(ga_abbr.ga_దత్తాంశం);
	కొరకు (i = 0; i < ga_రకం.ga_len; i++)
	    vim_free(((char_u **)ga_రకం.ga_దత్తాంశం)[i]);
	vim_free(ga_రకం.ga_దత్తాంశం);
	కొరకు (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_దత్తాంశం)[i]);
	vim_free(ga_menu.ga_దత్తాంశం);
	కొరకు (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_దత్తాంశం)[i]);
	vim_free(ga_info.ga_దత్తాంశం);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    స్థిర పూర్ణాంకం
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    పూర్ణాంకం		*అంకె_Matches)
{
    list_T      *retlist;

    *matches = ఖాళీ;
    *అంకె_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    ఒకవేళ (retlist == ఖాళీ)
	తిరిగి FAIL;

    expand_process_user_list(retlist, matches, అంకె_Matches, xp);
    list_unref(retlist);
    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    శూన్య
globpath(
    char_u	*మార్గం,
    char_u	*file,
    garray_T	*ga,
    పూర్ణాంకం		expand_options,
    పూర్ణాంకం		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#ఒకవేళ defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#లేకపోతే
# define TMP_PATHSEPSTR PATHSEPSTR
#అయితే_ముగింపు

    // Loop over all entries in {path}.
    అయితే (*మార్గం != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&మార్గం, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	ఒకవేళ (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    పూర్ణాంకం	    అంకె_p;
	    char_u  **p;

	    ఒకవేళ (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    ఒకవేళ (ExpandFromContext(&xpc, buf, &p, &అంకె_p,
			     WILD_SILENT|expand_options) != FAIL && అంకె_p > 0)
	    {
		ExpandEscape(&xpc, buf, అంకె_p, p, WILD_SILENT|expand_options);

		ఒకవేళ (ga_grow(ga, అంకె_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    కొరకు (పూర్ణాంకం i = 0; i < అంకె_p; ++i)
		    {
			((char_u **)ga->ga_దత్తాంశం)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		లేకపోతే
		{
		    FreeWild(అంకె_p, p);
		    p = ఖాళీ;
		}
		vim_free(p);
	    }
	}
    }

    vim_free(buf);
}
#నిర్వచనం_రద్దు TMP_PATHSEPSTR

/*
 * Translate some keys pressed when 'wildmenu' is used.
 */
    పూర్ణాంకం
wildmenu_translate_కీలకం(
	cmdline_info_T	*cclp,
	పూర్ణాంకం		కీలకం,
	expand_T	*xp,
	పూర్ణాంకం		did_wild_list)
{
    పూర్ణాంకం c = కీలకం;

    ఒకవేళ (cmdline_pum_చురుకైన())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	ఎంపిక (c)
	{
	    స్థితి K_పైకి:	  c = K_ఎడమ; ఆపు;
	    స్థితి K_కిందికి:  c = K_కుడి; ఆపు;
	    స్థితి K_ఎడమ:  c = K_పైకి; ఆపు;
	    స్థితి K_కుడి: c = K_కిందికి; ఆపు;
	    అప్రమేయ:	  ఆపు;
	}
    }

    ఒకవేళ (cmdline_pum_చురుకైన() || did_wild_list || wild_menu_showing)
    {
	ఒకవేళ (c == K_ఎడమ)
	    c = Ctrl_P;
	లేకపోతే ఒకవేళ (c == K_కుడి)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    ఒకవేళ (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_కిందికి;

    తిరిగి c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    స్థిర శూన్య
cmdline_del(cmdline_info_T *cclp, పూర్ణాంకం from)
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
    స్థిర పూర్ణాంకం
wildmenu_process_కీలకం_menunames(cmdline_info_T *cclp, పూర్ణాంకం కీలకం, expand_T *xp)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    ఒకవేళ (కీలకం == K_కిందికి && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	కీలకం = p_wc;
	కీలకం_Typed = TRUE;  // in case the key was mapped
    }
    లేకపోతే ఒకవేళ (కీలకం == K_పైకి)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	పూర్ణాంకం దొరికింది = FALSE;

	j = (పూర్ణాంకం)(xp->xp_నమూనా - cclp->cmdbuff);
	i = 0;
	అయితే (--j > 0)
	{
	    // check for start of menu name
	    ఒకవేళ (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		ఆపు;
	    }
	    // check for start of submenu name
	    ఒకవేళ (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		ఒకవేళ (దొరికింది)
		{
		    i = j + 1;
		    ఆపు;
		}
		లేకపోతే
		    దొరికింది = TRUE;
	    }
	}
	ఒకవేళ (i > 0)
	    cmdline_del(cclp, i);
	కీలకం = p_wc;
	కీలకం_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    తిరిగి కీలకం;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    స్థిర పూర్ణాంకం
wildmenu_process_కీలకం_filenames(cmdline_info_T *cclp, పూర్ణాంకం కీలకం, expand_T *xp)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    ఒకవేళ (కీలకం == K_కిందికి
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	కీలకం = p_wc;
	కీలకం_Typed = TRUE;  // in case the key was mapped
    }
    లేకపోతే ఒకవేళ (STRNCMP(xp->xp_నమూనా, upseg + 1, 3) == 0 && కీలకం == K_కిందికి)
    {
	// If in a direct ancestor, strip off one ../ to go down
	పూర్ణాంకం దొరికింది = FALSE;

	j = cclp->cmdpos;
	i = (పూర్ణాంకం)(xp->xp_నమూనా - cclp->cmdbuff);
	అయితే (--j > i)
	{
	    ఒకవేళ (has_mbyte)
		j -= (*mb_తల_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    ఒకవేళ (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		దొరికింది = TRUE;
		ఆపు;
	    }
	}
	ఒకవేళ (దొరికింది
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    కీలకం = p_wc;
	    కీలకం_Typed = TRUE;  // in case the key was mapped
	}
    }
    లేకపోతే ఒకవేళ (కీలకం == K_పైకి)
    {
	// go up a directory
	పూర్ణాంకం దొరికింది = FALSE;

	j = cclp->cmdpos - 1;
	i = (పూర్ణాంకం)(xp->xp_నమూనా - cclp->cmdbuff);
	అయితే (--j > i)
	{
	    ఒకవేళ (has_mbyte)
		j -= (*mb_తల_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    ఒకవేళ (vim_ispathsep(cclp->cmdbuff[j])
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == ఖాళీ
#అయితే_ముగింపు
	       )
	    {
		ఒకవేళ (దొరికింది)
		{
		    i = j + 1;
		    ఆపు;
		}
		లేకపోతే
		    దొరికింది = TRUE;
	    }
	}

	ఒకవేళ (!దొరికింది)
	    j = i;
	లేకపోతే ఒకవేళ (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	లేకపోతే ఒకవేళ (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	లేకపోతే
	    j = 0;
	ఒకవేళ (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	లేకపోతే ఒకవేళ (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	కీలకం = p_wc;
	కీలకం_Typed = TRUE;
    }

    తిరిగి కీలకం;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    పూర్ణాంకం
wildmenu_process_కీలకం(cmdline_info_T *cclp, పూర్ణాంకం కీలకం, expand_T *xp)
{
    ఒకవేళ (xp->xp_context == EXPAND_MENUNAMES)
	తిరిగి wildmenu_process_కీలకం_menunames(cclp, కీలకం, xp);
    లేకపోతే ఒకవేళ ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	తిరిగి wildmenu_process_కీలకం_filenames(cclp, కీలకం, xp);

    తిరిగి కీలకం;
}

/*
 * Free expanded names when finished walking through the matches
 */
    శూన్య
wildmenu_cleanup(cmdline_info_T *cclp వాడని)
{
    పూర్ణాంకం skt = కీలకం_Typed;

    ఒకవేళ (!p_wmnu || wild_menu_showing == 0)
	తిరిగి;

#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం save_RedrawingDisabled = RedrawingDisabled;
    ఒకవేళ (cclp->ఆదానం_fn)
	RedrawingDisabled = 0;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_శోధన_EXTRA)
    // Clear highlighting applied during wildmenu activity
    సమితి_no_hlsearch(TRUE);
#అయితే_ముగింపు

    ఒకవేళ (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_అడ్డువరుస--;
	redrawcmd();
    }
    లేకపోతే ఒకవేళ (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	చివరి_స్థితి(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    లేకపోతే
    {
	win_redraw_చివరి_స్థితి(topframe);
	redraw_statuslines();
    }
    కీలకం_Typed = skt;
    wild_menu_showing = 0;
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (cclp->ఆదానం_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    శూన్య
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    పూర్ణాంకం		filtered = FALSE;
    పూర్ణాంకం		options = WILD_SILENT | WILD_USE_NL | WILD_కూడు_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	తిరిగి;

    pat = tv_get_string(&argvars[0]);
    ఒకవేళ (check_for_string_arg(argvars, 1) == FAIL)
	తిరిగి;
    type = tv_get_string(&argvars[1]);

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], ఖాళీ);

    ఒకవేళ (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    ఒకవేళ (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    ఒకవేళ (STRCMP(type, "cmdline") == 0)
    {
	పూర్ణాంకం cmdline_len = (పూర్ణాంకం)STRLEN(pat);
	సమితి_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_నమూనా_len = (పూర్ణాంకం)STRLEN(xpc.xp_నమూనా);
	xpc.xp_నిలువు = cmdline_len;
    }
    లేకపోతే
    {
	char_u	*నమూనా_ప్రారంభం;

	xpc.xp_నమూనా = నమూనా_ప్రారంభం = pat;
	xpc.xp_నమూనా_len = (పూర్ణాంకం)STRLEN(xpc.xp_నమూనా);
	xpc.xp_పంక్తి = pat;

	xpc.xp_context = cmdcomplete_str_గమ్యం_type(type);
	ఎంపిక (xpc.xp_context)
	{
	స్థితి EXPAND_NOTHING:
	    semsg(_(e_invalid_వాదం_str), type);
	    తిరిగి;

	స్థితి EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    ఒకవేళ (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_వాదం_str), type);
		తిరిగి;
	    }

	    xpc.xp_arg = type + 7;
	    ఆపు;

	స్థితి EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    ఒకవేళ (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_వాదం_str), type);
		తిరిగి;
	    }

	    xpc.xp_arg = type + 11;
	    ఆపు;

# ఒకవేళ defined(FEAT_MENU)
	స్థితి EXPAND_MENUS:
	    సమితి_context_లోపల_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_నమూనా, FALSE);
	    xpc.xp_నమూనా_len -= (పూర్ణాంకం)(xpc.xp_నమూనా - నమూనా_ప్రారంభం);
	    ఆపు;
# endif

# ifdef FEAT_CSCOPE
	స్థితి EXPAND_CSCOPE:
	    సమితి_context_లోపల_cscope_cmd(&xpc, xpc.xp_నమూనా, CMD_cscope);
	    xpc.xp_నమూనా_len -= (పూర్ణాంకం)(xpc.xp_నమూనా - నమూనా_ప్రారంభం);
	    ఆపు;
# endif

# ifdef FEAT_SIGNS
	స్థితి EXPAND_SIGN:
	    సమితి_context_లోపల_sign_cmd(&xpc, xpc.xp_నమూనా);
	    xpc.xp_నమూనా_len -= (పూర్ణాంకం)(xpc.xp_నమూనా - నమూనా_ప్రారంభం);
	    ఆపు;
# endif

	స్థితి EXPAND_RUNTIME:
	    సమితి_context_లోపల_runtime_cmd(&xpc, xpc.xp_నమూనా);
	    xpc.xp_నమూనా_len -= (పూర్ణాంకం)(xpc.xp_నమూనా - నమూనా_ప్రారంభం);
	    ఆపు;

	స్థితి EXPAND_SHELLCMDLINE:
	{
	    పూర్ణాంకం context = EXPAND_SHELLCMDLINE;
	    సమితి_context_for_wildcard_arg(ఖాళీ, xpc.xp_నమూనా, FALSE, &xpc,
					 &context);
	    xpc.xp_నమూనా_len -= (పూర్ణాంకం)(xpc.xp_నమూనా - నమూనా_ప్రారంభం);
	    ఆపు;
	}

	స్థితి EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    ఆపు;

	స్థితి EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_కూడు;
	    ఆపు;

	అప్రమేయ:
	    ఆపు;
	}
    }

    {
	char_u	*pat_src = xpc.xp_నమూనా;
	పూర్ణాంకం	pat_len = xpc.xp_నమూనా_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	ఒకవేళ (unesc != ఖాళీ)
	    pat_src = unesc;

	ఒకవేళ (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	లేకపోతే
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    ఒకవేళ (rettv_list_alloc(rettv) == OK && pat != ఖాళీ)
    {
	పూర్ణాంకం	i;

	ExpandOne(&xpc, pat, ఖాళీ, options, WILD_ALL_KEEP);

	కొరకు (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    శూన్య
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    పూర్ణాంకం		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (పూర్ణాంకం)STRLEN(pat);
    సమితి_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_గమ్యం_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    శూన్య
f_cmdcomplete_info(typval_T *argvars వాడని, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    పూర్ణాంకం		    idx;
    పూర్ణాంకం		    ret = OK;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL || ccline == ఖాళీ
	    || ccline->xpc == ఖాళీ || ccline->xpc->xp_files == ఖాళీ)
	తిరిగి;
    retdict = rettv->vval.v_dict;
    ret = dict_కూడు_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (పూర్ణాంకం)cmdline_orig.నిడివి);
    ఒకవేళ (ret == OK)
	ret = dict_కూడు_సంఖ్య(retdict, "pum_visible", pum_visible());
    ఒకవేళ (ret == OK)
	ret = dict_కూడు_సంఖ్య(retdict, "selected", ccline->xpc->xp_selected);
    ఒకవేళ (ret == OK)
    {
	li = list_alloc();
	ఒకవేళ (li == ఖాళీ)
	    తిరిగి;
	ret = dict_కూడు_list(retdict, "matches", li);
	ఒకవేళ (ret == FAIL)
	    list_unref(li);
	కొరకు (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
	    list_append_string(li, ccline->xpc->xp_files[idx], -1);
    }
}
#అయితే_ముగింపు // FEAT_EVAL

/*
 * Copy a substring from the current buffer (curbuf), spanning from the given
 * 'start' position to the word boundary after 'end' position.
 * The copied string is stored in '*match', and the actual end position of the
 * matched text is returned in '*match_end'.
 */
    స్థిర పూర్ణాంకం
copy_substring_from_pos(pos_T *ప్రారంభం, pos_T *చివర, char_u **సరిపోలిక,
	pos_T *సరిపోలిక_చివర)
{
    char_u	*మాట_చివర;
    char_u	*పంక్తి, *ప్రారంభం_పంక్తి, *చివర_పంక్తి;
    పూర్ణాంకం		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    పూర్ణాంకం		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ఖాళీ;

    ఒకవేళ (ప్రారంభం->lnum > చివర->lnum
	    || (ప్రారంభం->lnum == చివర->lnum && ప్రారంభం->నిలువు >= చివర->నిలువు))
	తిరిగి FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    ప్రారంభం_పంక్తి = ml_get(ప్రారంభం->lnum);
    char_u  *ప్రారంభం_ptr = ప్రారంభం_పంక్తి + ప్రారంభం->నిలువు;
    పూర్ణాంకం	    is_single_పంక్తి = ప్రారంభం->lnum == చివర->lnum;

    segment_len = is_single_పంక్తి ? (చివర->నిలువు - ప్రారంభం->నిలువు)
			: (పూర్ణాంకం)(ml_get_len(ప్రారంభం->lnum) - ప్రారంభం->నిలువు);
    ఒకవేళ (ga_grow(&ga, segment_len + 2) != OK)
	వెళ్ళు fail;

    ga_concat_len(&ga, ప్రారంభం_ptr, segment_len);
    ఒకవేళ (!is_single_పంక్తి)
    {
	ఒకవేళ (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	లేకపోతే
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    ఒకవేళ (!is_single_పంక్తి)
    {
	కొరకు (lnum = ప్రారంభం->lnum + 1; lnum < చివర->lnum; lnum++)
	{
	    పూర్ణాంకం  linelen;

	    పంక్తి = ml_get(lnum);
	    linelen = (పూర్ణాంకం)ml_get_len(lnum);
	    ఒకవేళ (ga_grow(&ga, linelen + 2) != OK)
		వెళ్ళు fail;
	    ga_concat_len(&ga, పంక్తి, linelen);
	    ఒకవేళ (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    లేకపోతే
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    చివర_పంక్తి = ml_get(చివర->lnum);
    మాట_చివర = కనుగొను_మాట_చివర(చివర_పంక్తి + చివర->నిలువు);
    segment_len = (పూర్ణాంకం)(మాట_చివర - చివర_పంక్తి);
    ఒకవేళ (ga_grow(&ga, segment_len) != OK)
	వెళ్ళు fail;
    ga_concat_len(&ga, చివర_పంక్తి + (is_single_పంక్తి ? చివర->నిలువు : 0),
	    segment_len - (is_single_పంక్తి ? చివర->నిలువు : 0));

    // Null-terminate
    ఒకవేళ (ga_grow(&ga, 1) != OK)
	వెళ్ళు fail;
    ga_append(&ga, NUL);

    *సరిపోలిక = (char_u *)ga.ga_దత్తాంశం;
    సరిపోలిక_చివర->lnum = చివర->lnum;
    సరిపోలిక_చివర->నిలువు = segment_len;

    తిరిగి OK;

fail:
    ga_clear(&ga);
    తిరిగి FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    స్థిర పూర్ణాంకం
is_regex_సరిపోలిక(char_u *pat, char_u *str)
{
    ఒకవేళ (STRCMP(pat, str) == 0)
	తిరిగి TRUE;

    regmatch_T	regmatch;
    పూర్ణాంకం		ఫలితం;

    ++emsg_off;
    ++సందేశం_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --సందేశం_silent;

    ఒకవేళ (regmatch.regprog == ఖాళీ)
	తిరిగి FALSE;
    regmatch.rm_ic = p_ic;
    ఒకవేళ (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++సందేశం_silent;
    ఫలితం = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --సందేశం_silent;

    vim_regfree(regmatch.regprog);
    తిరిగి ఫలితం;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    స్థిర char_u *
concat_నమూనా_with_బఫరు_సరిపోలిక(
	char_u *pat,
	పూర్ణాంకం pat_len,
	pos_T *చివర_సరిపోలిక_pos,
	పూర్ణాంకం lowercase వాడని)
{
    char_u  *పంక్తి = ml_get(చివర_సరిపోలిక_pos->lnum);
    char_u  *మాట_చివర = కనుగొను_మాట_చివర(పంక్తి + చివర_సరిపోలిక_pos->నిలువు);
    పూర్ణాంకం	    సరిపోలిక_len = (పూర్ణాంకం)(మాట_చివర - (పంక్తి + చివర_సరిపోలిక_pos->నిలువు));
    char_u  *సరిపోలిక = alloc(సరిపోలిక_len + pat_len + 1);  // +1 for NUL

    ఒకవేళ (సరిపోలిక == ఖాళీ)
	తిరిగి ఖాళీ;
    mch_memmove(సరిపోలిక, pat, pat_len);
    ఒకవేళ (సరిపోలిక_len > 0)
    {
#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_SPELL)
	ఒకవేళ (lowercase)
	{
	    char_u  *mword = vim_strnsave(పంక్తి + చివర_సరిపోలిక_pos->నిలువు,
		    సరిపోలిక_len);
	    ఒకవేళ (mword == ఖాళీ)
		వెళ్ళు cleanup;
	    char_u  *క్రింది = strlow_save(mword);
	    vim_free(mword);
	    ఒకవేళ (క్రింది == ఖాళీ)
		వెళ్ళు cleanup;
	    mch_memmove(సరిపోలిక + pat_len, క్రింది, సరిపోలిక_len);
	    vim_free(క్రింది);
	}
	లేకపోతే
#అయితే_ముగింపు
	    mch_memmove(సరిపోలిక + pat_len, పంక్తి + చివర_సరిపోలిక_pos->నిలువు, సరిపోలిక_len);
    }
    సరిపోలిక[pat_len + సరిపోలిక_len] = NUL;
    తిరిగి సరిపోలిక;

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(సరిపోలిక);
    తిరిగి ఖాళీ;
#అయితే_ముగింపు
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    స్థిర పూర్ణాంకం
expand_నమూనా_లోపల_buf(
    char_u	*pat,		    // pattern to match
    పూర్ణాంకం		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    పూర్ణాంకం		*అంకె_Matches)	    // return: number of matches
{
    pos_T	cur_సరిపోలిక_pos, prev_సరిపోలిక_pos, చివర_సరిపోలిక_pos, మాట_చివర_pos;
    garray_T	ga;
    పూర్ణాంకం		దొరికింది_new_సరిపోలిక;
    పూర్ణాంకం		looped_around = FALSE;
    పూర్ణాంకం		pat_len;
    పూర్ణాంకం		has_range = FALSE;
    పూర్ణాంకం		compl_started = FALSE;
    పూర్ణాంకం		శోధన_flags;
    char_u	*సరిపోలిక, *నిండిన_సరిపోలిక;
    పూర్ణాంకం		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ఖాళీ;

#నిర్వచితమైతే FEAT_శోధన_EXTRA
    has_range = శోధన_మొదటి_పంక్తి != 0;
#అయితే_ముగింపు

    *matches = ఖాళీ;
    *అంకె_Matches = 0;

    ఒకవేళ (pat == ఖాళీ || *pat == NUL)
	తిరిగి FAIL;

    pat_len = (పూర్ణాంకం)STRLEN(pat);
    CLEAR_క్షేత్రం(cur_సరిపోలిక_pos);
    CLEAR_క్షేత్రం(prev_సరిపోలిక_pos);
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    ఒకవేళ (has_range)
	cur_సరిపోలిక_pos.lnum = శోధన_మొదటి_పంక్తి;
    లేకపోతే
#అయితే_ముగింపు
	cur_సరిపోలిక_pos = pre_incsearch_pos;

    శోధన_flags = శోధన_OPT | శోధన_NOOF | శోధన_PEEK | శోధన_NFMSG
	| (has_range ? శోధన_ప్రారంభం : 0);

    ga_init2(&ga, పరిమాణం(char_u *), 10); // Use growable array of char_u*

    కొరకు (;;)
    {
	++emsg_off;
	++సందేశం_silent;
	దొరికింది_new_సరిపోలిక = searchit(ఖాళీ, curbuf, &cur_సరిపోలిక_pos,
		&చివర_సరిపోలిక_pos, dir, pat, pat_len, 1L,
		శోధన_flags, RE_చివరి, ఖాళీ);
	--సందేశం_silent;
	--emsg_off;

	ఒకవేళ (దొరికింది_new_సరిపోలిక == FAIL)
	    ఆపు;

#నిర్వచితమైతే FEAT_శోధన_EXTRA
	// If in range mode, check if match is within the range
	ఒకవేళ (has_range && (cur_సరిపోలిక_pos.lnum < శోధన_మొదటి_పంక్తి
		    || cur_సరిపోలిక_pos.lnum > శోధన_చివరి_పంక్తి))
		ఆపు;
#అయితే_ముగింపు

	ఒకవేళ (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    ఒకవేళ ((dir == FORWARD && LTOREQ_POS(cur_సరిపోలిక_pos, prev_సరిపోలిక_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_సరిపోలిక_pos, cur_సరిపోలిక_pos)))
	    {
		ఒకవేళ (looped_around)
		    ఆపు;
		లేకపోతే
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_సరిపోలిక_pos = cur_సరిపోలిక_pos;

	// Abort if user typed a character or interrupted
	ఒకవేళ (char_avail() || got_int)
	{
	    ఒకవేళ (got_int)
	    {
		(శూన్య)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    వెళ్ళు cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	ఒకవేళ (చివర_సరిపోలిక_pos.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    cur_సరిపోలిక_pos.lnum = 1;
	    cur_సరిపోలిక_pos.నిలువు = 0;
	    cur_సరిపోలిక_pos.coladd = 0;
	    కొనసాగు;
	}

	// Extract the matching text prepended to completed word
	ఒకవేళ (copy_substring_from_pos(&cur_సరిపోలిక_pos, &చివర_సరిపోలిక_pos, &నిండిన_సరిపోలిక,
		    &మాట_చివర_pos) == FAIL)
	    ఆపు;

	ఒకవేళ (exacttext)
	    సరిపోలిక = నిండిన_సరిపోలిక;
	లేకపోతే
	{
	    // Construct a new match from completed word appended to pattern itself
	    సరిపోలిక = concat_నమూనా_with_బఫరు_సరిపోలిక(pat, pat_len, &చివర_సరిపోలిక_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    ఒకవేళ (సరిపోలిక == ఖాళీ || !is_regex_సరిపోలిక(సరిపోలిక, నిండిన_సరిపోలిక))
	    {
		vim_free(సరిపోలిక);
		సరిపోలిక = concat_నమూనా_with_బఫరు_సరిపోలిక(pat, pat_len,
			&చివర_సరిపోలిక_pos, TRUE);
		ఒకవేళ (సరిపోలిక == ఖాళీ || !is_regex_సరిపోలిక(సరిపోలిక, నిండిన_సరిపోలిక))
		{
		    vim_free(సరిపోలిక);
		    vim_free(నిండిన_సరిపోలిక);
		    కొనసాగు;
		}
	    }
	    vim_free(నిండిన_సరిపోలిక);
	}

	// Include this match if it is not a duplicate
	కొరకు (పూర్ణాంకం i = 0; i < ga.ga_len; ++i)
	{
	    ఒకవేళ (STRCMP(సరిపోలిక, ((char_u **)ga.ga_దత్తాంశం)[i]) == 0)
	    {
		VIM_CLEAR(సరిపోలిక);
		ఆపు;
	    }
	}
	ఒకవేళ (సరిపోలిక != ఖాళీ)
	{
	    ఒకవేళ (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(సరిపోలిక);
		వెళ్ళు cleanup;
	    }
	    ((char_u **)ga.ga_దత్తాంశం)[ga.ga_len++] = సరిపోలిక;
	    ఒకవేళ (ga.ga_len > ట్యాగ్_MANY)
		ఆపు;
	}
	ఒకవేళ (has_range)
	    cur_సరిపోలిక_pos = మాట_చివర_pos;
    }

    *matches = (char_u **)ga.ga_దత్తాంశం;
    *అంకె_Matches = ga.ga_len;
    తిరిగి OK;

cleanup:
    ga_clear_అక్షరశ్రేణులు(&ga);
    తిరిగి FAIL;
}
