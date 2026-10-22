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

#অন্তর্ভাব "vim.h"

স্থির পূর্ণসংখ্যা	cmd_showtail;	// Only show path tail in lists ?
স্থির পূর্ণসংখ্যা	may_expand_pattern = FALSE;
স্থির pos_T	pre_incsearch_pos; // Cursor position when incsearch started

স্থির শূন্য	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, পূর্ণসংখ্যা usefilter, expand_T *xp, পূর্ণসংখ্যা *complp);
স্থির পূর্ণসংখ্যা	ExpandFromContext(expand_T *xp, char_u *, char_u ***, পূর্ণসংখ্যা *, পূর্ণসংখ্যা);
স্থির char_u	*showmatches_gettail(char_u *s);
স্থির পূর্ণসংখ্যা	expand_showtail(expand_T *xp);
স্থির পূর্ণসংখ্যা	expand_shellcmd(char_u *filepat, char_u ***matches, পূর্ণসংখ্যা *অঙ্ক_Matches, পূর্ণসংখ্যা flagsarg);
#যদি defined(FEAT_EVAL)
স্থির পূর্ণসংখ্যা	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, পূর্ণসংখ্যা *অঙ্ক_Matches);
স্থির পূর্ণসংখ্যা	ExpandUserList(expand_T *xp, char_u ***matches, পূর্ণসংখ্যা *অঙ্ক_Matches);
স্থির char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
স্থির char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, পূর্ণসংখ্যা srclen, পূর্ণসংখ্যা *new_lenp);
#endif
স্থির পূর্ণসংখ্যা	expand_pattern_in_buf(char_u *pat, পূর্ণসংখ্যা dir, char_u ***matches, পূর্ণসংখ্যা *অঙ্ক_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
স্থির pumitem_T *compl_match_array = ফাঁকা;
স্থির পূর্ণসংখ্যা compl_match_arraysize;
// First column in cmdline of the matched item for completion.
স্থির পূর্ণসংখ্যা compl_startcol;
স্থির পূর্ণসংখ্যা compl_selected;
// cmdline before expansion
স্থির string_T cmdline_orig = {ফাঁকা, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    স্থির পূর্ণসংখ্যা
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    নির্বাচন (xp->xp_context)
    {
    ক্ষেত্র EXPAND_BOOL_SETTINGS:
    ক্ষেত্র EXPAND_COLORS:
    ক্ষেত্র EXPAND_COMPILER:
    ক্ষেত্র EXPAND_DIRECTORIES:
    ক্ষেত্র EXPAND_DIRS_IN_CDPATH:
    ক্ষেত্র EXPAND_FILES:
    ক্ষেত্র EXPAND_FILES_IN_PATH:
    ক্ষেত্র EXPAND_FILETYPE:
    ক্ষেত্র EXPAND_FILETYPECMD:
    ক্ষেত্র EXPAND_HELP:
    ক্ষেত্র EXPAND_KEYMAP:
    ক্ষেত্র EXPAND_OLD_SETTING:
    ক্ষেত্র EXPAND_STRING_SETTING:
    ক্ষেত্র EXPAND_SETTING_SUBTRACT:
    ক্ষেত্র EXPAND_OWNSYNTAX:
    ক্ষেত্র EXPAND_PACKADD:
    ক্ষেত্র EXPAND_RUNTIME:
    ক্ষেত্র EXPAND_SHELLCMD:
    ক্ষেত্র EXPAND_SHELLCMDLINE:
    ক্ষেত্র EXPAND_TAGS:
    ক্ষেত্র EXPAND_TAGS_LISTFILES:
	ফেরত FALSE;

    পূর্বনির্ধারিত:
	বিরতি;
    }

    ফেরত vim_strchr(p_wop, WOP_FUZZY) != ফাঁকা;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    পূর্ণসংখ্যা
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    ফেরত vim_strchr(p_wop, WOP_FUZZY) != ফাঁকা && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    স্থির পূর্ণসংখ্যা
sort_func_compare(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    যদি (*p1 != '<' && *p2 == '<') ফেরত -1;
    যদি (*p1 == '<' && *p2 != '<') ফেরত 1;
    ফেরত STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    স্থির শূন্য
wildescape(
    expand_T	*xp,
    char_u	*str,
    পূর্ণসংখ্যা		numfiles,
    char_u	**files)
{
    char_u	*p;
    পূর্ণসংখ্যা		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_বাফার : VSE_NONE;

    যদি (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	জন্য (পূর্ণসংখ্যা i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    যদি (xp->xp_backslash & XP_BS_THREE)
	    {
		অক্ষর *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		যদি (p != ফাঁকা)
		{
		    vim_free(files[i]);
		    files[i] = p;
#যদি defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    যদি (p != ফাঁকা)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    অন্যথা যদি (xp->xp_backslash & XP_BS_COMMA)
	    {
		যদি (vim_strchr(files[i], ',') != ফাঁকা)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    যদি (p != ফাঁকা)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ifdef BACKSLASH_IN_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#অন্যথা
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#endif
	    যদি (p != ফাঁকা)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    যদি (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	যদি (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    অন্যথা যদি (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	জন্য (পূর্ণসংখ্যা i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    যদি (p != ফাঁকা)
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
    স্থির শূন্য
ExpandEscape(
    expand_T	*xp,
    char_u	*str,
    পূর্ণসংখ্যা		numfiles,
    char_u	**files,
    পূর্ণসংখ্যা		options)
{
    // May change home directory back to "~"
    যদি (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    যদি (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    পূর্ণসংখ্যা
nextwild(
    expand_T	*xp,
    পূর্ণসংখ্যা		type,
    পূর্ণসংখ্যা		options,	// extra options for ExpandOne()
    পূর্ণসংখ্যা		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    পূর্ণসংখ্যা		i;
    char_u	*p;
    পূর্ণসংখ্যা		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    পূর্ণসংখ্যা		wild_navigate = (type == WILD_পরবর্তী || type == WILD_পূর্ব
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    যদি (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	যদি (ccline->প্রবেশ_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	অন্যথা
#endif
	{
	    may_expand_pattern = options & WILD_MAY_EXPAND_PATTERN;
	    set_expand_context(xp);
	    may_expand_pattern = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    যদি (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ফেরত OK;  // Something illegal on command line
    }
    যদি (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ফেরত FAIL;
    }

    i = (পূর্ণসংখ্যা)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    যদি (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	ফেরত FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    যদি (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	বার্তা_puts("...");	    // show that we are busy
	out_flush();
    }

    যদি (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, ফাঁকা, ফাঁকা, 0, type);
    }
    অন্যথা
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	পূর্ণসংখ্যা	pat_len = xp->xp_pattern_len;
#যদি defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	যদি (unesc != ফাঁকা)
	    pat_src = unesc;
#endif

	যদি (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	অন্যথা
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#যদি defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	যদি (tmp == ফাঁকা)
	    p = ফাঁকা;
	অন্যথা
	{
	    পূর্ণসংখ্যা use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    যদি (escape)
		use_options |= WILD_ESCAPE;
	    যদি (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    যদি (p != ফাঁকা && type == WILD_LONGEST)
	    {
		পূর্ণসংখ্যা	j;

		জন্য (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    যদি (c == '*' || c == '?')
			বিরতি;
		}
		যদি ((পূর্ণসংখ্যা)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    যদি (!wild_navigate && ccline->cmdbuff != ফাঁকা)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	যদি (cmdline_orig.string == ফাঁকা)
	    cmdline_orig.দৈর্ঘ্য = 0;
	অন্যথা
	    cmdline_orig.দৈর্ঘ্য = ccline->cmdlen;
    }

    যদি (p != ফাঁকা && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	পূর্ণসংখ্যা	difflen;
	পূর্ণসংখ্যা	v = OK;

	difflen = (পূর্ণসংখ্যা)plen - xp->xp_pattern_len;
	যদি (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	যদি (v == OK)
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
    যদি (xp->xp_context == EXPAND_MAPPINGS && p == ফাঁকা)
	ফেরত FAIL;

    যদি (xp->xp_numfiles <= 0 && p == ফাঁকা)
	beep_flush();
    অন্যথা যদি (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(শূন্য)ExpandOne(xp, ফাঁকা, ফাঁকা, 0, WILD_FREE);

    vim_free(p);

    ফেরত OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    স্থির পূর্ণসংখ্যা
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	পূর্ণসংখ্যা		অঙ্ক_Matches,
	পূর্ণসংখ্যা		showtail)
{
    পূর্ণসংখ্যা	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, অঙ্ক_Matches);
    যদি (compl_match_array == ফাঁকা)
	ফেরত EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = অঙ্ক_Matches;
    জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_Matches; i++)
    {
	compl_match_array[i].pum_text = (xp->xp_files_abbr != ফাঁকা
				    && xp->xp_files_abbr[i] != ফাঁকা)
					? xp->xp_files_abbr[i]
					: SHOW_MATCH(i);
	compl_match_array[i].pum_info = xp->xp_files_info != ফাঁকা
					    ? xp->xp_files_info[i] : ফাঁকা;
	compl_match_array[i].pum_extra = xp->xp_files_menu != ফাঁকা
					    ? xp->xp_files_menu[i] : ফাঁকা;
	compl_match_array[i].pum_kind = xp->xp_files_kind != ফাঁকা
					    ? xp->xp_files_kind[i] : ফাঁকা;
	compl_match_array[i].pum_user_abbr_hlattr = -1;
	compl_match_array[i].pum_user_kind_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == ফাঁকা ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    prefix_len = vim_strsize(xp->xp_pattern);
    যদি (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    ফেরত EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    শূন্য
cmdline_pum_display(শূন্য)
{
    যদি (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    পূর্ণসংখ্যা
cmdline_pum_active(শূন্য)
{
    ফেরত pum_visible() && compl_match_array != ফাঁকা;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    শূন্য
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, পূর্ণসংখ্যা defer_redraw)
{
    পূর্ণসংখ্যা	save_চাবি_Typed = চাবি_Typed;
#ifdef FEAT_EVAL
    পূর্ণসংখ্যা	save_RedrawingDisabled = RedrawingDisabled;
    যদি (cclp->প্রবেশ_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    যদি (!defer_redraw)
    {
	পূর্ণসংখ্যা save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    অন্যথা
	pum_call_update_screen();
    redrawcmd();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    চাবি_Typed = save_চাবি_Typed;
#ifdef FEAT_EVAL
    যদি (cclp->প্রবেশ_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

    শূন্য
cmdline_pum_cleanup(cmdline_info_T *cclp)
{
    cmdline_pum_remove(cclp, FALSE);
    wildmenu_cleanup(cclp);
}

/*
 * Returns the starting column number to use for the cmdline completion popup
 * menu.
 */
    পূর্ণসংখ্যা
cmdline_compl_startcol(শূন্য)
{
    ফেরত compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(শূন্য)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ফেরত xp == ফাঁকা ? ফাঁকা : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    পূর্ণসংখ্যা
cmdline_compl_is_fuzzy(শূন্য)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ফেরত xp != ফাঁকা && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    স্থির পূর্ণসংখ্যা
skip_status_match_char(expand_T *xp, char_u *s)
{
    যদি ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	যদি (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    ফেরত 2;
#endif
	ফেরত 1;
    }
    ফেরত 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    স্থির পূর্ণসংখ্যা
status_match_len(expand_T *xp, char_u *s)
{
    পূর্ণসংখ্যা	len = 0;

#ifdef FEAT_MENU
    পূর্ণসংখ্যা emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    যদি (emenu && menu_is_separator(s))
	ফেরত 1;
#endif

    যখন (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    ফেরত len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    স্থির শূন্য
win_redr_status_matches(
    expand_T	*xp,
    পূর্ণসংখ্যা		অঙ্ক_matches,
    char_u	**matches,	// list of matches
    পূর্ণসংখ্যা		match,
    পূর্ণসংখ্যা		showtail)
{
    পূর্ণসংখ্যা		row;
    char_u	*buf;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		clen;		// length in screen cells
    পূর্ণসংখ্যা		fillchar;
    পূর্ণসংখ্যা		attr;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		highlight = TRUE;
    char_u	*selstart = ফাঁকা;
    পূর্ণসংখ্যা		selstart_col = 0;
    char_u	*selend = ফাঁকা;
    স্থির পূর্ণসংখ্যা	first_match = 0;
    পূর্ণসংখ্যা		add_বাম = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    পূর্ণসংখ্যা		emenu;
#endif
    পূর্ণসংখ্যা		l;

    যদি (matches == ফাঁকা)	// interrupted completion?
	ফেরত;

    যদি (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    অন্যথা
	buf = alloc(topframe->fr_width + 1);
    যদি (buf == ফাঁকা)
	ফেরত;

    যদি (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    যদি (match == 0)
	first_match = 0;
    অন্যথা যদি (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_বাম = TRUE;
    }
    অন্যথা
    {
	// check if match fits on the screen
	জন্য (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	যদি (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	যদি (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    জন্য (i = match; i < অঙ্ক_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		যদি (clen >= topframe->fr_width)
		    বিরতি;
	    }
	    যদি (i == অঙ্ক_matches)
		add_বাম = TRUE;
	}
    }
    যদি (add_বাম)
	যখন (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    যদি (clen >= topframe->fr_width)
		বিরতি;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    যদি (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    অন্যথা
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    যখন (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	যদি (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	যদি (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (পূর্ণসংখ্যা)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	অন্যথা
#endif
	জন্য ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    যদি (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    অন্যথা
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (পূর্ণসংখ্যা)STRLEN(buf + len);
	    }
	}
	যদি (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	যদি (++i == অঙ্ক_matches)
		বিরতি;
    }

    যদি (i != অঙ্ক_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    যদি (row >= 0)
    {
	যদি (wild_menu_showing == 0)
	{
	    যদি (বার্তা_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		যদি (cmdline_row == Rows - 1)
		{
		    screen_del_সারিগুলি(0, 0, 1, (পূর্ণসংখ্যা)Rows, TRUE, 0, ফাঁকা);
		    ++বার্তা_scrolled;
		}
		অন্যথা
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    অন্যথা
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		যদি (lastwin->w_status_height == 0)
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
	যদি (selstart != ফাঁকা && highlight)
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

#যদি defined(FEAT_EVAL)
/*
 * Apply -completeopt=escape to a string about to be inserted into the command
 * line as a completion result.  If "str" is non-NULL and the active expansion
 * context is a customlist/custom user command with UCC_ESCAPE set, free "str"
 * and return a newly-allocated copy with spaces, tabs and backslashes prefixed
 * by a backslash.  Otherwise return "str" unchanged.
 */
    স্থির char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    যদি (str == ফাঁকা)
	ফেরত ফাঁকা;
    যদি ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ফেরত str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    যদি (p == ফাঁকা)
	ফেরত str;
    vim_free(str);
    ফেরত p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    স্থির char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    পূর্ণসংখ্যা		srclen,
    পূর্ণসংখ্যা		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    যদি ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ফেরত ফাঁকা;

    buf = alloc(srclen + 1);
    যদি (buf == ফাঁকা)
	ফেরত ফাঁকা;

    d = buf;
    end = src + srclen;
    জন্য (p = src; p < end; ++p)
    {
	যদি (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (পূর্ণসংখ্যা)(d - buf);
    ফেরত buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    স্থির char_u *
get_পরবর্তী_or_পূর্ব_match(পূর্ণসংখ্যা mode, expand_T *xp)
{
    পূর্ণসংখ্যা	    findex = xp->xp_selected;
    পূর্ণসংখ্যা	    ht;

    // When no matches found, return NULL
    যদি (xp->xp_numfiles <= 0)
	ফেরত ফাঁকা;

    যদি (mode == WILD_পূর্ব)
    {
	// Select the last entry if at original text
	যদি (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    অন্যথা যদি (mode == WILD_পরবর্তী)
    {
	// Select the next entry
	++findex;
    }
    অন্যথা   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	যদি (ht > 3)
	    ht -= 2;

	যদি (mode == WILD_PAGEUP)
	{
	    যদি (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    অন্যথা যদি (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    অন্যথা
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	অন্যথা    // mode == WILD_PAGEDOWN
	{
	    যদি (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    অন্যথা যদি (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    অন্যথা
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    যদি (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	যদি (xp->xp_orig != ফাঁকা)
	    findex = -1;
	অন্যথা
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    যদি (p_wmnu)
    {
	যদি (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	অন্যথা যদি (vim_strchr(p_wop, WOP_PUM) != ফাঁকা)
	{
	    যদি (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	অন্যথা
	    win_redr_status_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    যদি (findex == -1)
	ফেরত vim_strsave(xp->xp_orig);
#যদি defined(FEAT_EVAL)
    ফেরত apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#অন্যথা
    ফেরত vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    স্থির char_u *
ExpandOne_start(পূর্ণসংখ্যা mode, expand_T *xp, char_u *str, পূর্ণসংখ্যা options)
{
    পূর্ণসংখ্যা		non_suf_match;		// number without matching suffix
    পূর্ণসংখ্যা		i;
    char_u	*ss = ফাঁকা;

    // Do the expansion.
    যদি (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	যদি (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    অন্যথা যদি (xp->xp_numfiles == 0)
    {
	যদি (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    অন্যথা
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	যদি (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    যদি (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    অন্যথা
		non_suf_match = 1;
	    যদি ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		জন্য (i = 0; i < 2; ++i)
		    যদি (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    যদি (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		যদি (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		অন্যথা যদি (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    যদি (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    ফেরত ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    স্থির char_u *
find_longest_match(expand_T *xp, পূর্ণসংখ্যা options)
{
    long_u	len;
    পূর্ণসংখ্যা		mb_len = 1;
    পূর্ণসংখ্যা		c0, ci;
    পূর্ণসংখ্যা		i;
    char_u	*ss;

    জন্য (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	যদি (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	অন্যথা
	    c0 = xp->xp_files[0][len];
	জন্য (i = 1; i < xp->xp_numfiles; ++i)
	{
	    যদি (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    অন্যথা
		ci = xp->xp_files[i][len];
	    যদি (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		যদি (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    বিরতি;
	    }
	    অন্যথা যদি (c0 != ci)
		বিরতি;
	}
	যদি (i < xp->xp_numfiles)
	{
	    যদি (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    বিরতি;
	}
    }

    ss = alloc(len + 1);
    যদি (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    ফেরত ss;
}

    শূন্য
free_xp_files_extra(expand_T *xp, পূর্ণসংখ্যা numfiles)
{
    যদি (xp->xp_files_abbr != ফাঁকা)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = ফাঁকা;
    }
    যদি (xp->xp_files_kind != ফাঁকা)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = ফাঁকা;
    }
    যদি (xp->xp_files_menu != ফাঁকা)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = ফাঁকা;
    }
    যদি (xp->xp_files_info != ফাঁকা)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = ফাঁকা;
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
    পূর্ণসংখ্যা		options,
    পূর্ণসংখ্যা		mode)
{
    char_u	*ss = ফাঁকা;
    পূর্ণসংখ্যা		orig_saved = FALSE;
#যদি defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    পূর্ণসংখ্যা		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    যদি (mode == WILD_পরবর্তী || mode == WILD_পূর্ব
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	ফেরত get_পরবর্তী_or_পূর্ব_match(mode, xp);

    যদি (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    অন্যথা যদি (mode == WILD_APPLY)
    {
	যদি (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	অন্যথা
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#যদি defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    যদি (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	যদি (compl_match_array != ফাঁকা)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    যদি (mode == WILD_FREE)	// only release file name
	ফেরত ফাঁকা;

    যদি (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#যদি defined(FEAT_EVAL)
	যদি (ss != ফাঁকা)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    যদি (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#যদি defined(FEAT_EVAL)
	যদি (ss != ফাঁকা)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    যদি (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	অক্ষর	*prefix = "";
	অক্ষর	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	পূর্ণসংখ্যা	n = xp->xp_numfiles - 1;
	পূর্ণসংখ্যা	i;
#যদি defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = ফাঁকা;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	যদি ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    যদি (escaped != ফাঁকা)
	    {
		জন্য (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    যদি (escaped[i] == ফাঁকা)
		    {
			যখন (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			বিরতি;
		    }
		}
		যদি (escaped != ফাঁকা)
		    files = escaped;
	    }
	}
#অন্যথা
	char_u	**files = xp->xp_files;
#endif

	যদি (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	অন্যথা যদি (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	জন্য (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	যদি (ss != ফাঁকা)
	{
	    size_t  ss_len = 0;

	    জন্য (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (অক্ষর *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (অক্ষর *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#যদি defined(FEAT_EVAL)
	যদি (escaped != ফাঁকা)
	{
	    জন্য (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    যদি (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    যদি (!orig_saved)
	vim_free(orig);

#যদি defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    যদি (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    ফেরত ss;
}

/*
 * Prepare an expand structure for use.
 */
    শূন্য
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
    শূন্য
ExpandCleanup(expand_T *xp)
{
    যদি (xp->xp_numfiles >= 0)
    {
	free_xp_files_extra(xp, xp->xp_numfiles);
	FreeWild(xp->xp_numfiles, xp->xp_files);
	xp->xp_numfiles = -1;
    }
    VIM_CLEAR(xp->xp_orig);
}

    শূন্য
clear_cmdline_orig(শূন্য)
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
    স্থির শূন্য
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	পূর্ণসংখ্যা		অঙ্ক_Matches,
	পূর্ণসংখ্যা		সারিগুলি,
	পূর্ণসংখ্যা		linenr,
	পূর্ণসংখ্যা		maxlen,
	পূর্ণসংখ্যা		showtail,
	পূর্ণসংখ্যা		dir_attr)
{
    পূর্ণসংখ্যা		i, j;
    পূর্ণসংখ্যা		isdir;
    পূর্ণসংখ্যা		lastlen;
    char_u	*p;

    lastlen = 999;
    জন্য (j = linenr; j < অঙ্ক_Matches; j += সারিগুলি)
    {
	যদি (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    বার্তা_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    বার্তা_advance(maxlen + 1);
	    বার্তা_puts((অক্ষর *)p);
	    বার্তা_advance(maxlen + 3);
	    বার্তা_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    বিরতি;
	}
	জন্য (i = maxlen - lastlen; --i >= 0; )
	    বার্তা_putchar(' ');
	যদি (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    যদি (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_path;
		char_u	*path;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_path = expand_env_save_opt(matches[j], TRUE, ফাঁকা);
		path = exp_path != ফাঁকা ? exp_path : matches[j];
		halved_slash = backslash_halve_save(path);
		isdir = mch_isdir(halved_slash != ফাঁকা ? halved_slash
			: matches[j]);
		vim_free(exp_path);
		যদি (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    অন্যথা
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    যদি (showtail)
		p = SHOW_MATCH(j);
	    অন্যথা
	    {
		home_replace(ফাঁকা, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	অন্যথা
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = বার্তা_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    যদি (বার্তা_col > 0)	// when not wrapped around
    {
	বার্তা_clr_eos();
	বার্তা_putchar('\n');
    }
    out_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    স্থির পূর্ণসংখ্যা
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	পূর্ণসংখ্যা		অঙ্ক_Matches,
	পূর্ণসংখ্যা		showtail,
	পূর্ণসংখ্যা		noselect)
{
    পূর্ণসংখ্যা	retval = cmdline_pum_create(ccline, xp, matches, অঙ্ক_Matches, showtail);

    যদি (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    ফেরত retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    পূর্ণসংখ্যা
showmatches(
    expand_T	*xp,
    পূর্ণসংখ্যা		display_wildmenu,
    পূর্ণসংখ্যা		display_list,
    পূর্ণসংখ্যা		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    পূর্ণসংখ্যা		অঙ্ক_Matches;
    char_u	**matches;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		maxlen;
    পূর্ণসংখ্যা		সারিগুলি;
    পূর্ণসংখ্যা		columns;
    পূর্ণসংখ্যা		attr;
    পূর্ণসংখ্যা		showtail;
    পূর্ণসংখ্যা		noselect = (wim_flags_arg & WIM_NOSELECT);
    পূর্ণসংখ্যা		noinsert = (wim_flags_arg & WIM_NOINSERT);
    পূর্ণসংখ্যা		cmdline_unchanged = noselect || noinsert;

    যদি (xp->xp_numfiles == -1)
    {
	পূর্ণসংখ্যা retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&অঙ্ক_Matches, &matches);
	যদি (retval != EXPAND_OK)
	    ফেরত retval;
	showtail = expand_showtail(xp);
    }
    অন্যথা
    {
	অঙ্ক_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    যদি (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != ফাঁকা)
	ফেরত show_pum_matches(ccline, xp, matches, অঙ্ক_Matches,
		showtail && !cmdline_unchanged, noselect);

    যদি (display_list)
    {
	বার্তা_didany = FALSE;		// lines_left will be set
	বার্তা_start();			// prepare for paging
	বার্তা_putchar('\n');
	out_flush();
	cmdline_row = বার্তা_row;
	বার্তা_didany = FALSE;		// lines_left will be set again
	বার্তা_start();			// prepare for paging
    }

    যদি (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    অন্যথা যদি (display_wildmenu && !display_list)
	win_redr_status_matches(xp, অঙ্ক_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    অন্যথা যদি (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	জন্য (i = 0; i < অঙ্ক_Matches; ++i)
	{
	    পূর্ণসংখ্যা	len;
	    যদি (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(ফাঁকা, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    অন্যথা
		len = vim_strsize(SHOW_MATCH(i));
	    যদি (len > maxlen)
		maxlen = len;
	}

	যদি (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    সারিগুলি = অঙ্ক_Matches;
	অন্যথা
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    যদি (columns < 1)
		columns = 1;
	    সারিগুলি = (অঙ্ক_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	যদি (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    বার্তা_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    বার্তা_clr_eos();
	    বার্তা_advance(maxlen - 3);
	    বার্তা_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	জন্য (i = 0; i < সারিগুলি; ++i)
	{
	    showmatches_oneline(xp, matches, অঙ্ক_Matches, সারিগুলি, i,
						maxlen, showtail, attr);
	    যদি (got_int)
	    {
		got_int = FALSE;
		বিরতি;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = বার্তা_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    যদি (display_wildmenu && display_list)
    {
	যদি (vim_strchr(p_wop, WOP_PUM) != ফাঁকা)
	    (শূন্য)show_pum_matches(ccline, xp, matches, অঙ্ক_Matches,
		    showtail && !cmdline_unchanged, noselect);
	অন্যথা
	    win_redr_status_matches(xp, অঙ্ক_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    যদি (xp->xp_numfiles == -1)
    {
	FreeWild(অঙ্ক_Matches, matches);
	free_xp_files_extra(xp, অঙ্ক_Matches);
    }

    ফেরত EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    স্থির char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    পূর্ণসংখ্যা		had_sep = FALSE;

    জন্য (p = s; *p != NUL; )
    {
	যদি (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	অন্যথা যদি (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    ফেরত t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    স্থির পূর্ণসংখ্যা
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    যদি (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	ফেরত FALSE;

    end = gettail(xp->xp_pattern);
    যদি (end == xp->xp_pattern)		// there is no path separator
	ফেরত FALSE;

    জন্য (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	যদি (rem_backslash(s))
	    ++s;
	অন্যথা যদি (vim_strchr((char_u *)"*?[", *s) != ফাঁকা)
	    ফেরত FALSE;
    }
    ফেরত TRUE;
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
    পূর্ণসংখ্যা		len,
    পূর্ণসংখ্যা		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    পূর্ণসংখ্যা		i, j;
    পূর্ণসংখ্যা		new_len;
    char_u	*লেজ;
    পূর্ণসংখ্যা		ends_in_star;

    যদি (context != EXPAND_FILES
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
	যদি (context == EXPAND_FINDFUNC
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
	অন্যথা
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    জন্য (i = 0; i < len; i++)
	    {
		যদি (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		যদি (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		যদি ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    যদি (retval != ফাঁকা)
	    {
		retval[0] = '^';
		j = 1;
		জন্য (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    যদি (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			বিরতি;

		    নির্বাচন (fname[i])
		    {
			ক্ষেত্র '*':   retval[j++] = '.';
				    বিরতি;
			ক্ষেত্র '~':   retval[j++] = '\\';
				    বিরতি;
			ক্ষেত্র '?':   retval[j] = '.';
				    চালু;
			ক্ষেত্র '.':   যদি (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    বিরতি;
			ক্ষেত্র '\\':  যদি (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    বিরতি;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    অন্যথা
    {
	retval = alloc(len + 4);
	যদি (retval != ফাঁকা)
	{
	    vim_strncpy(retval, fname, len);

	    // Don't add a star to *, ~, ~user, $var or `cmd`.
	    // * would become **, which walks the whole tree.
	    // ~ would be at the start of the file name, but not the tail.
	    // $ could be anywhere in the tail.
	    // ` could be anywhere in the file name.
	    // When the name ends in '$' don't add a star, remove the '$'.
	    লেজ = gettail(retval);
	    ends_in_star = (len > 0 && retval[len - 1] == '*');
#ifndef BACKSLASH_IN_FILENAME
	    জন্য (i = len - 2; i >= 0; --i)
	    {
		যদি (retval[i] != '\\')
		    বিরতি;
		ends_in_star = !ends_in_star;
	    }
#endif
	    যদি ((*retval != '~' || লেজ != retval)
		    && !ends_in_star
		    && vim_strchr(লেজ, '$') == ফাঁকা
		    && vim_strchr(retval, '`') == ফাঁকা)
		retval[len++] = '*';
	    অন্যথা যদি (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    ফেরত retval;
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
    শূন্য
set_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    যদি ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_অনুসন্ধান_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_অনুসন্ধান_EXTRA
	অনুসন্ধান_first_সারি = 0; // Search entire buffer
#endif
	ফেরত;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    যদি (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->প্রবেশ_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	ফেরত;
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
    স্থির char_u *
set_cmd_সূচক(char_u *cmd, exarg_T *eap, expand_T *xp, পূর্ণসংখ্যা *complp)
{
    char_u	*p = ফাঁকা;
    পূর্ণসংখ্যা		len = 0;
    পূর্ণসংখ্যা		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    যদি (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    অন্যথা
    {
	p = cmd;
	যখন (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	যদি (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    যখন (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	যদি (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    যখন (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	যদি (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != ফাঁকা)
	    ++p;
	len = (পূর্ণসংখ্যা)(p - cmd);

	যদি (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    ফেরত ফাঁকা;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	যদি ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    যখন (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    যদি (*p == NUL && ASCII_ISALNUM(p[-1]))
	ফেরত ফাঁকা;

    যদি (eap->cmdidx == CMD_আকার)
    {
	যদি (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != ফাঁকা)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	অন্যথা যদি (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, ফাঁকা, xp, complp);
	    যদি (p == ফাঁকা)
		eap->cmdidx = CMD_আকার;	// ambiguous user command
	}
    }
    যদি (eap->cmdidx == CMD_আকার)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ফেরত ফাঁকা;
    }

    ফেরত p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    স্থির শূন্য
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	পূর্ণসংখ্যা		usefilter,
	expand_T	*xp,
	পূর্ণসংখ্যা		*complp)
{
    char_u	*p;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		in_quote = FALSE;
    char_u	*bow = ফাঁকা;	// Beginning of word
    পূর্ণসংখ্যা		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    যখন (*p != NUL)
    {
	যদি (has_mbyte)
	    c = mb_ptr2char(p);
	অন্যথা
	    c = *p;
	যদি (c == '\\' && p[1] != NUL)
	    ++p;
	অন্যথা যদি (c == '`')
	{
	    যদি (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	অন্যথা যদি (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != ফাঁকা && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    যখন (*p != NUL)
	    {
		যদি (has_mbyte)
		    c = mb_ptr2char(p);
		অন্যথা
		    c = *p;
		যদি (c == '`' || vim_isfilec_or_wc(c))
		    বিরতি;
		যদি (has_mbyte)
		    len = (*mb_ptr2len)(p);
		অন্যথা
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    যদি (in_quote)
		bow = p;
	    অন্যথা
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    যদি (bow != ফাঁকা && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    যদি (usefilter
	    || (eap != ফাঁকা
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	যদি (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    যদি (*xp->xp_pattern == '$')
    {
	জন্য (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    যদি (!vim_isIDc(*p))
		বিরতি;
	যদি (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    যদি (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    যদি (*xp->xp_pattern == '~')
    {
	জন্য (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	যদি (*p == NUL && p > xp->xp_pattern + 1
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
    স্থির char_u *
set_context_in_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    যদি (p == ফাঁকা)
	xp->xp_pattern = arg;
    অন্যথা
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    ফেরত ফাঁকা;
}

#ifdef FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    স্থির char_u *
set_context_in_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    যদি (p == ফাঁকা)
	xp->xp_pattern = arg;
    অন্যথা
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    ফেরত ফাঁকা;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    স্থির char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    যদি (*arg != NUL)
	arg = skip_vimgrep_pat(arg, ফাঁকা, ফাঁকা);
    যদি (arg == ফাঁকা || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	ফেরত ফাঁকা;
    }
    ফেরত skipwhite(arg);
}

#ifdef FEAT_অনুসন্ধান_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    স্থির char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    যদি (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	যদি (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    ফেরত find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    স্থির char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    পূর্ণসংখ্যা		delim;

    delim = *arg;	    // get the delimiter
    যদি (delim)
	++arg;		    // skip delimiter if there is one

    যখন (arg[0] != NUL && arg[0] != delim)
    {
	যদি (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    যদি (arg[0] != NUL)
	ফেরত arg + 1;

    ফেরত ফাঁকা;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    স্থির char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    পূর্ণসংখ্যা		delim;

    delim = *arg;
    যদি (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	যদি (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    যখন (arg[0] != NUL && arg[0] != delim)
	    {
		যদি (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    যদি (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    যখন (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == ফাঁকা)
	++arg;
    যদি (arg[0] != NUL)
	ফেরত arg;

    ফেরত ফাঁকা;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    স্থির char_u *
find_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    যদি (*arg != '/')
	ফেরত ফাঁকা;

    // Match regexp, not just whole words
    জন্য (++arg; *arg && *arg != '/'; arg++)
	যদি (*arg == '\\' && arg[1] != NUL)
	    arg++;
    যদি (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	যদি (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == ফাঁকা)
	    xp->xp_context = EXPAND_NOTHING;
	অন্যথা
	    ফেরত arg;
    }

    ফেরত ফাঁকা;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    স্থির char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    যখন ((xp->xp_pattern = vim_strchr(arg, ' ')) != ফাঁকা)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    যদি (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    ফেরত ফাঁকা;
}
#endif

#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    স্থির char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    যদি (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    অন্যথা
    {
	যদি ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_pattern = skipwhite(p);
	}
	অন্যথা
	    xp->xp_context = EXPAND_NOTHING;
    }

    ফেরত ফাঁকা;
}
#endif

স্থির গণনা
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
স্থির গণনা
{
    EXP_BREAKPT_ADD,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    স্থির char_u *
set_context_in_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_start;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_pattern = arg;

    যদি (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    অন্যথা যদি (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    অন্যথা
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    যদি (*p == NUL)
	ফেরত ফাঁকা;
    subcmd_start = p;

    যদি (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	যদি (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    যদি (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		ফেরত ফাঁকা;
	    }
	    p = skipwhite(p);
	}
	যদি (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	অন্যথা
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    অন্যথা যদি (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    ফেরত ফাঁকা;
}

    স্থির char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = ফাঁকা;

    p = skipwhite(arg);
    যদি (VIM_ISDIGIT(*p))
	ফেরত ফাঁকা;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    ফেরত ফাঁকা;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    স্থির char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    পূর্ণসংখ্যা    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    যদি (*p == NUL)
	ফেরত ফাঁকা;

    জন্য (;;)
    {
	যদি (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    চালু;
	}
	যদি (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    চালু;
	}
	বিরতি;
    }

    যদি ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    অন্যথা যদি ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    অন্যথা যদি ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    ফেরত ফাঁকা;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    স্থির শূন্য
set_context_with_pattern(expand_T *xp)
{
    পূর্ণসংখ্যা		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_অনুসন্ধান_EXTRA
    পূর্ণসংখ্যা		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    যদি (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	ফেরত;
#endif

    xp->xp_pattern = ccline->cmdbuff + skiplen;
    xp->xp_pattern_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_PATTERN_IN_BUF;
    xp->xp_অনুসন্ধান_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    স্থির char_u *
set_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	long		argt,
	পূর্ণসংখ্যা		compl,
	পূর্ণসংখ্যা		forceit)
{
    char_u  *nextcmd;

    নির্বাচন (cmdidx)
    {
	ক্ষেত্র CMD_find:
	ক্ষেত্র CMD_sfind:
	ক্ষেত্র CMD_tabfind:
	    যদি (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    বিরতি;
	ক্ষেত্র CMD_cd:
	ক্ষেত্র CMD_chdir:
	ক্ষেত্র CMD_tcd:
	ক্ষেত্র CMD_tchdir:
	ক্ষেত্র CMD_lcd:
	ক্ষেত্র CMD_lchdir:
	    যদি (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    বিরতি;
	ক্ষেত্র CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    বিরতি;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	ক্ষেত্র CMD_aboveleft:
	ক্ষেত্র CMD_argdo:
	ক্ষেত্র CMD_belowright:
	ক্ষেত্র CMD_botright:
	ক্ষেত্র CMD_browse:
	ক্ষেত্র CMD_bufdo:
	ক্ষেত্র CMD_cdo:
	ক্ষেত্র CMD_cfdo:
	ক্ষেত্র CMD_confirm:
	ক্ষেত্র CMD_debug:
	ক্ষেত্র CMD_folddoclosed:
	ক্ষেত্র CMD_folddoopen:
	ক্ষেত্র CMD_hide:
	ক্ষেত্র CMD_horizontal:
	ক্ষেত্র CMD_keepalt:
	ক্ষেত্র CMD_keepjumps:
	ক্ষেত্র CMD_keepmarks:
	ক্ষেত্র CMD_keeppatterns:
	ক্ষেত্র CMD_ldo:
	ক্ষেত্র CMD_leftabove:
	ক্ষেত্র CMD_lfdo:
	ক্ষেত্র CMD_lockmarks:
	ক্ষেত্র CMD_noautocmd:
	ক্ষেত্র CMD_noswapfile:
	ক্ষেত্র CMD_rightbelow:
	ক্ষেত্র CMD_sandbox:
	ক্ষেত্র CMD_silent:
	ক্ষেত্র CMD_tab:
	ক্ষেত্র CMD_tabdo:
	ক্ষেত্র CMD_topleft:
	ক্ষেত্র CMD_unsilent:
	ক্ষেত্র CMD_verbose:
	ক্ষেত্র CMD_vertical:
	ক্ষেত্র CMD_windo:
	ক্ষেত্র CMD_vim9cmd:
	ক্ষেত্র CMD_legacy:
	    ফেরত arg;

	ক্ষেত্র CMD_filter:
	    ফেরত set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_অনুসন্ধান_EXTRA
	ক্ষেত্র CMD_match:
	    ফেরত set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	ক্ষেত্র CMD_command:
	    ফেরত set_context_in_user_cmd(xp, arg);

	ক্ষেত্র CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    বিরতি;

	ক্ষেত্র CMD_global:
	ক্ষেত্র CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    যদি (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    ফেরত nextcmd;

	ক্ষেত্র CMD_and:
	ক্ষেত্র CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    যদি (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    ফেরত nextcmd;

	ক্ষেত্র CMD_isearch:
	ক্ষেত্র CMD_dsearch:
	ক্ষেত্র CMD_ilist:
	ক্ষেত্র CMD_dlist:
	ক্ষেত্র CMD_ijump:
	ক্ষেত্র CMD_psearch:
	ক্ষেত্র CMD_djump:
	ক্ষেত্র CMD_isplit:
	ক্ষেত্র CMD_dsplit:
	    ফেরত find_cmd_after_isearch_cmd(xp, arg);
	ক্ষেত্র CMD_autocmd:
	    ফেরত set_context_in_autocmd(xp, arg, FALSE);
	ক্ষেত্র CMD_doautocmd:
	ক্ষেত্র CMD_doautoall:
	    ফেরত set_context_in_autocmd(xp, arg, TRUE);
	ক্ষেত্র CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    বিরতি;
	ক্ষেত্র CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    বিরতি;
	ক্ষেত্র CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    বিরতি;
	ক্ষেত্র CMD_tag:
	ক্ষেত্র CMD_stag:
	ক্ষেত্র CMD_ptag:
	ক্ষেত্র CMD_ltag:
	ক্ষেত্র CMD_tselect:
	ক্ষেত্র CMD_stselect:
	ক্ষেত্র CMD_ptselect:
	ক্ষেত্র CMD_tjump:
	ক্ষেত্র CMD_stjump:
	ক্ষেত্র CMD_ptjump:
	    যদি (vim_strchr(p_wop, WOP_TAGFILE) != ফাঁকা)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    অন্যথা
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    বিরতি;
	ক্ষেত্র CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    বিরতি;
#ifdef FEAT_SYN_HL
	ক্ষেত্র CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    বিরতি;
#endif
#ifdef FEAT_EVAL
	ক্ষেত্র CMD_final:
	ক্ষেত্র CMD_const:
	ক্ষেত্র CMD_let:
	ক্ষেত্র CMD_var:
	ক্ষেত্র CMD_if:
	ক্ষেত্র CMD_elseif:
	ক্ষেত্র CMD_while:
	ক্ষেত্র CMD_for:
	ক্ষেত্র CMD_echo:
	ক্ষেত্র CMD_echon:
	ক্ষেত্র CMD_execute:
	ক্ষেত্র CMD_echomsg:
	ক্ষেত্র CMD_echoerr:
	ক্ষেত্র CMD_echoconsole:
	ক্ষেত্র CMD_echowindow:
	ক্ষেত্র CMD_call:
	ক্ষেত্র CMD_return:
	ক্ষেত্র CMD_cexpr:
	ক্ষেত্র CMD_caddexpr:
	ক্ষেত্র CMD_cgetexpr:
	ক্ষেত্র CMD_lexpr:
	ক্ষেত্র CMD_laddexpr:
	ক্ষেত্র CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    বিরতি;

	ক্ষেত্র CMD_unlet:
	    ফেরত set_context_in_unlet_cmd(xp, arg);
	ক্ষেত্র CMD_function:
	ক্ষেত্র CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    বিরতি;
	ক্ষেত্র CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    বিরতি;

	ক্ষেত্র CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    বিরতি;
#endif
	ক্ষেত্র CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    বিরতি;
#ifdef FEAT_CSCOPE
	ক্ষেত্র CMD_cscope:
	ক্ষেত্র CMD_lcscope:
	ক্ষেত্র CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    বিরতি;
#endif
#ifdef FEAT_SIGNS
	ক্ষেত্র CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    বিরতি;
#endif
	ক্ষেত্র CMD_bdelete:
	ক্ষেত্র CMD_bwipeout:
	ক্ষেত্র CMD_bunload:
	    যখন ((xp->xp_pattern = vim_strchr(arg, ' ')) != ফাঁকা)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	ক্ষেত্র CMD_বাফার:
	ক্ষেত্র CMD_sbuffer:
	ক্ষেত্র CMD_pbuffer:
	ক্ষেত্র CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    বিরতি;
#ifdef FEAT_DIFF
	ক্ষেত্র CMD_diffget:
	ক্ষেত্র CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    বিরতি;
#endif
	ক্ষেত্র CMD_USER:
	ক্ষেত্র CMD_USER_BUF:
	    ফেরত set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	ক্ষেত্র CMD_map:	    ক্ষেত্র CMD_noremap:
	ক্ষেত্র CMD_nmap:	    ক্ষেত্র CMD_nnoremap:
	ক্ষেত্র CMD_vmap:	    ক্ষেত্র CMD_vnoremap:
	ক্ষেত্র CMD_omap:	    ক্ষেত্র CMD_onoremap:
	ক্ষেত্র CMD_imap:	    ক্ষেত্র CMD_inoremap:
	ক্ষেত্র CMD_cmap:	    ক্ষেত্র CMD_cnoremap:
	ক্ষেত্র CMD_lmap:	    ক্ষেত্র CMD_lnoremap:
	ক্ষেত্র CMD_smap:	    ক্ষেত্র CMD_snoremap:
	ক্ষেত্র CMD_tmap:	    ক্ষেত্র CMD_tnoremap:
	ক্ষেত্র CMD_xmap:	    ক্ষেত্র CMD_xnoremap:
	    ফেরত set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	ক্ষেত্র CMD_unmap:
	ক্ষেত্র CMD_nunmap:
	ক্ষেত্র CMD_vunmap:
	ক্ষেত্র CMD_ounmap:
	ক্ষেত্র CMD_iunmap:
	ক্ষেত্র CMD_cunmap:
	ক্ষেত্র CMD_lunmap:
	ক্ষেত্র CMD_sunmap:
	ক্ষেত্র CMD_tunmap:
	ক্ষেত্র CMD_xunmap:
	    ফেরত set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	ক্ষেত্র CMD_mapclear:
	ক্ষেত্র CMD_nmapclear:
	ক্ষেত্র CMD_vmapclear:
	ক্ষেত্র CMD_omapclear:
	ক্ষেত্র CMD_imapclear:
	ক্ষেত্র CMD_cmapclear:
	ক্ষেত্র CMD_lmapclear:
	ক্ষেত্র CMD_smapclear:
	ক্ষেত্র CMD_tmapclear:
	ক্ষেত্র CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    বিরতি;

	ক্ষেত্র CMD_abbreviate:	ক্ষেত্র CMD_noreabbrev:
	ক্ষেত্র CMD_cabbrev:	ক্ষেত্র CMD_cnoreabbrev:
	ক্ষেত্র CMD_iabbrev:	ক্ষেত্র CMD_inoreabbrev:
	    ফেরত set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	ক্ষেত্র CMD_unabbreviate:
	ক্ষেত্র CMD_cunabbrev:
	ক্ষেত্র CMD_iunabbrev:
	    ফেরত set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	ক্ষেত্র CMD_menu:	    ক্ষেত্র CMD_noremenu:	    ক্ষেত্র CMD_unmenu:
	ক্ষেত্র CMD_amenu:	    ক্ষেত্র CMD_anoremenu:	    ক্ষেত্র CMD_aunmenu:
	ক্ষেত্র CMD_nmenu:	    ক্ষেত্র CMD_nnoremenu:	    ক্ষেত্র CMD_nunmenu:
	ক্ষেত্র CMD_vmenu:	    ক্ষেত্র CMD_vnoremenu:	    ক্ষেত্র CMD_vunmenu:
	ক্ষেত্র CMD_omenu:	    ক্ষেত্র CMD_onoremenu:	    ক্ষেত্র CMD_ounmenu:
	ক্ষেত্র CMD_imenu:	    ক্ষেত্র CMD_inoremenu:	    ক্ষেত্র CMD_iunmenu:
	ক্ষেত্র CMD_cmenu:	    ক্ষেত্র CMD_cnoremenu:	    ক্ষেত্র CMD_cunmenu:
	ক্ষেত্র CMD_tlmenu:    ক্ষেত্র CMD_tlnoremenu:    ক্ষেত্র CMD_tlunmenu:
	ক্ষেত্র CMD_tmenu:				    ক্ষেত্র CMD_tunmenu:
	ক্ষেত্র CMD_popup:	    ক্ষেত্র CMD_tearoff:	    ক্ষেত্র CMD_emenu:
	    ফেরত set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	ক্ষেত্র CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    বিরতি;

	ক্ষেত্র CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    বিরতি;

	ক্ষেত্র CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    বিরতি;

	ক্ষেত্র CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    বিরতি;

	ক্ষেত্র CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    বিরতি;

	ক্ষেত্র CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    বিরতি;

#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	ক্ষেত্র CMD_language:
	    ফেরত set_context_in_lang_cmd(xp, arg);
#endif
#যদি defined(FEAT_PROFILE)
	ক্ষেত্র CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    বিরতি;
#endif
	ক্ষেত্র CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    বিরতি;

	ক্ষেত্র CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    বিরতি;

	ক্ষেত্র CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    বিরতি;

	ক্ষেত্র CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    বিরতি;
#যদি defined(FEAT_PROFILE)
	ক্ষেত্র CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    বিরতি;
#endif

	ক্ষেত্র CMD_argdelete:
	    যখন ((xp->xp_pattern = vim_strchr(arg, ' ')) != ফাঁকা)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    বিরতি;

#ifdef FEAT_EVAL
	ক্ষেত্র CMD_breakadd:
	ক্ষেত্র CMD_profdel:
	ক্ষেত্র CMD_breakdel:
	    ফেরত set_context_in_breakadd_cmd(xp, arg, cmdidx);

	ক্ষেত্র CMD_scriptnames:
	    ফেরত set_context_in_scriptnames_cmd(xp, arg);
#endif
	ক্ষেত্র CMD_filetype:
	    ফেরত set_context_in_filetype_cmd(xp, arg);

	পূর্বনির্ধারিত:
	    বিরতি;
    }
    ফেরত ফাঁকা;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    স্থির char_u *
set_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    পূর্ণসংখ্যা			len = 0;
    exarg_T		ea;
    পূর্ণসংখ্যা			compl = EXPAND_NOTHING;
    পূর্ণসংখ্যা			forceit = FALSE;
    পূর্ণসংখ্যা			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_সারি = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    জন্য (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != ফাঁকা; cmd++)
	;
    xp->xp_pattern = cmd;

    যদি (*cmd == NUL)
	ফেরত ফাঁকা;
    যদি (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	ফেরত ফাঁকা;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    যদি (*cmd == NUL)
	ফেরত ফাঁকা;
    যদি (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	ফেরত ফাঁকা;
    }

    যদি (*cmd == '|' || *cmd == '\n')
	ফেরত cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_সূচক(cmd, &ea, xp, &compl);
    যদি (p == ফাঁকা)
	ফেরত ফাঁকা;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    যদি (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    যদি (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    যদি ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	যখন (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    যখন (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    যদি (*p == NUL)
	    {
		যদি (ea.argt & EX_ARGOPT)
		    ফেরত set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		যদি (ea.cmdidx == CMD_terminal)
		    ফেরত set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    যদি (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	যদি (*arg == '>')			// append
	{
	    যদি (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	অন্যথা যদি (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    যদি (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	যদি (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    যদি (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	যখন (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    যদি ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	যদি (*arg == NUL)
	    ফেরত p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    যদি ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	যদি (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	যখন (*p)
	{
	    যদি (*p == Ctrl_V)
	    {
		যদি (p[1] != NUL)
		    ++p;
	    }
	    অন্যথা যদি ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		যদি (*(p - 1) != '\\')
		{
		    যদি (*p == '|' || *p == '\n')
			ফেরত p + 1;
		    ফেরত ফাঁকা;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    যদি (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == ফাঁকা)
	// no arguments allowed but there is something
	ফেরত ফাঁকা;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (পূর্ণসংখ্যা)STRLEN(buff);
    যখন (*p && p < buff + len)
    {
	যদি (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	অন্যথা
	{
	    যদি (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    যদি (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    ফেরত set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    শূন্য
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    পূর্ণসংখ্যা		len,	    // length of command line (excl. NUL)
    পূর্ণসংখ্যা		col,	    // position of cursor
    পূর্ণসংখ্যা		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    পূর্ণসংখ্যা			context;
#endif
    পূর্ণসংখ্যা		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    যদি (col < len)
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
    যদি (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_আকার);
    }
    অন্যথা যদি (use_ccline && ccline->প্রবেশ_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	যদি (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(ফাঁকা, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    অন্যথা
#endif
	যখন (nextcomm != ফাঁকা)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_সারি = str;
    xp->xp_col = col;

    যদি (col < len)
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
    পূর্ণসংখ্যা
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    পূর্ণসংখ্যা		col,		// position of cursor
    পূর্ণসংখ্যা		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = ফাঁকা;
    পূর্ণসংখ্যা		options = WILD_ADD_SLASH|WILD_SILENT;

    যদি (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ফেরত EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    যদি (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ফেরত EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (পূর্ণসংখ্যা)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	পূর্ণসংখ্যা	pat_len = xp->xp_pattern_len;
#যদি defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	যদি (unesc != ফাঁকা)
	    pat_src = unesc;
#endif

	যদি (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	অন্যথা
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#যদি defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    যদি (file_str == ফাঁকা)
	ফেরত EXPAND_UNSUCCESSFUL;

    যদি (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    যদি (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = ফাঁকা;
    }
    vim_free(file_str);

    ফেরত EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	পূর্ণসংখ্যা		*অঙ্ক_Matches,
	পূর্ণসংখ্যা		flags,
	পূর্ণসংখ্যা		options)
{
    পূর্ণসংখ্যা		free_pat = FALSE;
    পূর্ণসংখ্যা		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    যদি (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	যদি (pat == ফাঁকা)
	    ফেরত ret;

	pat_end = pat + pat_len;
	জন্য (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    যদি (*p != '\\')
		চালু;

	    যদি (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    অন্যথা যদি (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    অন্যথা যদি (xp->xp_backslash & XP_BS_COMMA)
	    {
		যদি (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		অন্যথা যদি (*(p + 1) == ',')
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

    যদি (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, অঙ্ক_Matches);
#endif
    }
    অন্যথা
    {
	যদি (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	অন্যথা যদি (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	অন্যথা যদি (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	অন্যথা
	    flags = (flags | EW_DIR) & ~EW_FILE;
	যদি (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, অঙ্ক_Matches, matches, flags);
    }
    যদি (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    যদি (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	পূর্ণসংখ্যা j;

	জন্য (j = 0; j < *অঙ্ক_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    যখন (*ptr != NUL)
	    {
		যদি (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		অন্যথা যদি (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    ফেরত ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    স্থির char_u *
get_behave_arg(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    যদি (idx == 0)
	ফেরত (char_u *)"mswin";
    যদি (idx == 1)
	ফেরত (char_u *)"xterm";
    ফেরত ফাঁকা;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    স্থির char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    যদি (idx < 0)
	ফেরত ফাঁকা;

    যদি (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	অক্ষর	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	ফেরত (char_u *)opts_all[idx];
    }
    যদি (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	অক্ষর	*opts_plugin[] = {"plugin", "on", "off"};
	ফেরত (char_u *)opts_plugin[idx];
    }
    যদি (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	অক্ষর	*opts_indent[] = {"indent", "on", "off"};
	ফেরত (char_u *)opts_indent[idx];
    }
    যদি (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	অক্ষর	*opts_onoff[] = {"on", "off"};
	ফেরত (char_u *)opts_onoff[idx];
    }
    ফেরত ফাঁকা;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    স্থির char_u *
get_breakadd_arg(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    যদি (idx >= 0 && idx <= 3)
    {
	অক্ষর	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	যদি (breakpt_expand_what == EXP_BREAKPT_ADD)
	    ফেরত (char_u *)opts[idx];
	অন্যথা যদি (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    যদি (idx <= 2)
		ফেরত (char_u *)opts[idx + 1];
	}
	অন্যথা
	{
	    // profdel {func, file}
	    যদি (idx <= 1)
		ফেরত (char_u *)opts[idx + 1];
	}
    }
    ফেরত ফাঁকা;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    স্থির char_u *
get_scriptnames_arg(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    scriptitem_T *si;

    যদি (!SCRIPT_ID_VALID(idx + 1))
	ফেরত ফাঁকা;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(ফাঁকা, si->sn_name, NameBuff, MAXPATHL, TRUE);
    ফেরত NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    স্থির char_u *
get_retab_arg(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    যদি (idx == 0)
	ফেরত (char_u *)"-indentonly";
    ফেরত ফাঁকা;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    স্থির char_u *
get_messages_arg(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    যদি (idx == 0)
	ফেরত (char_u *)"clear";
    ফেরত ফাঁকা;
}

    স্থির char_u *
get_mapclear_arg(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    যদি (idx == 0)
	ফেরত (char_u *)"<buffer>";
    ফেরত ফাঁকা;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    স্থির পূর্ণসংখ্যা
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	পূর্ণসংখ্যা		*অঙ্ক_Matches)
{
    স্থির কাঠামো expgen
    {
	পূর্ণসংখ্যা		context;
	char_u	*((*func)(expand_T *, পূর্ণসংখ্যা));
	পূর্ণসংখ্যা		ic;
	পূর্ণসংখ্যা		escaped;
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
	{EXPAND_EXPRESSION, get_রাশিমালা_name, FALSE, TRUE},
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
#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
    পূর্ণসংখ্যা	i;
    পূর্ণসংখ্যা ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(tab); ++i)
    {
	যদি (xp->xp_context == tab[i].context)
	{
	    যদি (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, অঙ্ক_Matches,
						tab[i].func, tab[i].escaped);
	    বিরতি;
	}
    }

    ফেরত ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    স্থির পূর্ণসংখ্যা
map_wildopts_to_ewflags(পূর্ণসংখ্যা options)
{
    পূর্ণসংখ্যা		flags;

    flags = EW_DIR;	// include directories
    যদি (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    যদি (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    যদি (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    যদি (options & WILD_SILENT)
	flags |= EW_SILENT;
    যদি (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    যদি (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    ফেরত flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    স্থির পূর্ণসংখ্যা
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    পূর্ণসংখ্যা		*অঙ্ক_Matches,
    পূর্ণসংখ্যা		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    পূর্ণসংখ্যা		ret;
    পূর্ণসংখ্যা		flags;
    char_u	*tofree = ফাঁকা;
    পূর্ণসংখ্যা		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    যদি (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	ফেরত expand_files_and_dirs(xp, pat, matches, অঙ্ক_Matches, flags,
								options);

    *matches = (char_u **)"";
    *অঙ্ক_Matches = 0;
    যদি (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	যদি (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					অঙ্ক_Matches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*অঙ্ক_Matches, *matches);
#endif
	    ফেরত OK;
	}
	ফেরত FAIL;
    }

    যদি (xp->xp_context == EXPAND_SHELLCMD)
	ফেরত expand_shellcmd(pat, matches, অঙ্ক_Matches, flags);
    যদি (xp->xp_context == EXPAND_OLD_SETTING)
	ফেরত ExpandOldSetting(অঙ্ক_Matches, matches);
    যদি (xp->xp_context == EXPAND_BUFFERS)
	ফেরত ExpandBufnames(pat, অঙ্ক_Matches, matches, options);
#ifdef FEAT_DIFF
    যদি (xp->xp_context == EXPAND_DIFF_BUFFERS)
	ফেরত ExpandBufnames(pat, অঙ্ক_Matches, matches,
						options | BUF_DIFF_FILTER);
#endif
    যদি (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	ফেরত expand_tags(xp->xp_context == EXPAND_TAGS, pat, অঙ্ক_Matches,
								matches);
    যদি (xp->xp_context == EXPAND_COLORS)
    {
	অক্ষর *directories[] = {"colors", ফাঁকা};
	ফেরত ExpandRTDir(pat, DIP_START + DIP_OPT, অঙ্ক_Matches, matches,
								directories);
    }
    যদি (xp->xp_context == EXPAND_COMPILER)
    {
	অক্ষর *directories[] = {"compiler", ফাঁকা};
	ফেরত ExpandRTDir(pat, 0, অঙ্ক_Matches, matches, directories);
    }
    যদি (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	অক্ষর *directories[] = {"syntax", ফাঁকা};
	ফেরত ExpandRTDir(pat, 0, অঙ্ক_Matches, matches, directories);
    }
    যদি (xp->xp_context == EXPAND_FILETYPE)
    {
	অক্ষর *directories[] = {"syntax", "indent", "ftplugin", ফাঁকা};
	ফেরত ExpandRTDir(pat, 0, অঙ্ক_Matches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    যদি (xp->xp_context == EXPAND_KEYMAP)
    {
	অক্ষর *directories[] = {"keymap", ফাঁকা};
	ফেরত ExpandRTDir(pat, 0, অঙ্ক_Matches, matches, directories);
    }
#endif
#যদি defined(FEAT_EVAL)
    যদি (xp->xp_context == EXPAND_USER_LIST)
	ফেরত ExpandUserList(xp, matches, অঙ্ক_Matches);
#endif
    যদি (xp->xp_context == EXPAND_PACKADD)
	ফেরত ExpandPackAddDir(pat, অঙ্ক_Matches, matches);
    যদি (xp->xp_context == EXPAND_RUNTIME)
	ফেরত expand_runtime_cmd(pat, অঙ্ক_Matches, matches);
    যদি (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	ফেরত expand_pattern_in_buf(pat, xp->xp_অনুসন্ধান_dir,
		matches, অঙ্ক_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    যদি ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	পূর্ণসংখ্যা len = (পূর্ণসংখ্যা)STRLEN(pat) + 20;

	tofree = alloc(len);
	যদি (tofree == ফাঁকা)
	    ফেরত FAIL;
	vim_snprintf((অক্ষর *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    যদি (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	যদি (regmatch.regprog == ফাঁকা)
	{
	    vim_free(tofree);
	    ফেরত FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    যদি (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, অঙ্ক_Matches, matches, fuzzy);
    অন্যথা যদি (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, অঙ্ক_Matches, matches);
    অন্যথা যদি (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, অঙ্ক_Matches, matches);
    অন্যথা যদি (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, অঙ্ক_Matches, matches);
    অন্যথা যদি (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, অঙ্ক_Matches);
    অন্যথা যদি (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, অঙ্ক_Matches);
#যদি defined(FEAT_TERMINAL)
    অন্যথা যদি (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, অঙ্ক_Matches);
#endif
#যদি defined(FEAT_EVAL)
    অন্যথা যদি (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, অঙ্ক_Matches);
#endif
    অন্যথা
	ret = ExpandOther(pat, xp, &regmatch, matches, অঙ্ক_Matches);

    যদি (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    ফেরত ret;
}

    পূর্ণসংখ্যা
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    পূর্ণসংখ্যা		*অঙ্ক_Matches,
    char_u	*(*func)(expand_T *, পূর্ণসংখ্যা),
					  // returns a string from the list
    পূর্ণসংখ্যা		escaped)
{
    ফেরত ExpandGenericExt(
	pat, xp, regmatch, matches, অঙ্ক_Matches, func, escaped, 0);
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
    পূর্ণসংখ্যা
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    পূর্ণসংখ্যা		*অঙ্ক_Matches,
    char_u	*(*func)(expand_T *, পূর্ণসংখ্যা),
					  // returns a string from the list
    পূর্ণসংখ্যা		escaped,
    পূর্ণসংখ্যা		sortStartIdx)
{
    পূর্ণসংখ্যা		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = ফাঁকা;
    পূর্ণসংখ্যা		score = 0;
    পূর্ণসংখ্যা		fuzzy;
    পূর্ণসংখ্যা		match;
    পূর্ণসংখ্যা		sort_matches = FALSE;
    পূর্ণসংখ্যা		funcsort = FALSE;
    পূর্ণসংখ্যা		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ফাঁকা;
    *অঙ্ক_Matches = 0;

    যদি (!fuzzy)
	ga_init2(&ga, আকার(অক্ষর *), 30);
    অন্যথা
	ga_init2(&ga, আকার(fuzmatch_str_T), 30);

    জন্য (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	যদি (str == ফাঁকা)	    // end of list
	    বিরতি;
	যদি (*str == NUL)	    // skip empty strings
	    চালু;

	যদি (xp->xp_pattern[0] != NUL)
	{
	    যদি (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    অন্যথা
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	অন্যথা
	    match = TRUE;

	যদি (!match)
	    চালু;

	যদি (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	অন্যথা
	    str = vim_strsave(str);
	যদি (str == ফাঁকা)
	{
	    যদি (!fuzzy)
	    {
		ga_clear_strings(&ga);
		ফেরত FAIL;
	    }
	    fuzmatch_str_free(ga.ga_তথ্য, ga.ga_len);
	    ফেরত FAIL;
	}

	যদি (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    বিরতি;
	}

	যদি (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_তথ্য)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	অন্যথা
	    ((char_u **)ga.ga_তথ্য)[ga.ga_len] = str;

#ifdef FEAT_MENU
	যদি (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    যদি (*str == '\001')
		*str = '.';
	}
#endif

	যদি (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    যদি (ga.ga_len == 0)
	ফেরত OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    যদি (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    যদি (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    যদি (sort_matches && sortStartMatchIdx != -1)
    {
	যদি (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((শূন্য *)ga.ga_তথ্য, (size_t)ga.ga_len, আকার(char_u *),
							   sort_func_compare);
	অন্যথা
	    sort_strings((char_u **)ga.ga_তথ্য + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    যদি (!fuzzy)
    {
	*matches = ga.ga_তথ্য;
	*অঙ্ক_Matches = ga.ga_len;
    }
    অন্যথা
    {
	যদি (fuzzymatches_to_strmatches(ga.ga_তথ্য, matches, ga.ga_len,
							funcsort) == FAIL)
	    ফেরত FAIL;
	*অঙ্ক_Matches = ga.ga_len;
    }

#যদি defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    ফেরত OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    স্থির শূন্য
expand_shellcmd_onedir(
	char_u		*pathed_pattern,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	পূর্ণসংখ্যা		*অঙ্ক_Matches,
	পূর্ণসংখ্যা		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    যদি (expand_wildcards(1, &pathed_pattern, অঙ্ক_Matches, matches, flags) != OK)
	ফেরত;

    যদি (ga_grow(gap, *অঙ্ক_Matches) == FAIL)
    {
	FreeWild(*অঙ্ক_Matches, *matches);
	ফেরত;
    }

    জন্য (পূর্ণসংখ্যা i = 0; i < *অঙ্ক_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	যদি (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    যদি (HASHITEM_EMPTY(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_তথ্য)[gap->ga_len++] = name;
		hash_add_item(ht, hi, name, hash);
		name = ফাঁকা;
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
    স্থির পূর্ণসংখ্যা
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    পূর্ণসংখ্যা		*অঙ্ক_Matches,	// return: number of matches
    পূর্ণসংখ্যা		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = ফাঁকা;
    পূর্ণসংখ্যা		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    পূর্ণসংখ্যা		flags = flagsarg;
    পূর্ণসংখ্যা		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    যদি (buf == ফাঁকা)
	ফেরত FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    যদি (pat == ফাঁকা)
    {
	vim_free(buf);
	ফেরত FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    জন্য (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	যদি (*s != '\\')
	    চালু;

	p = s + 1;
	যদি (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    যদি (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    অন্যথা
    {
	// For an absolute name we don't use $PATH.
	যদি (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	যদি (path == ফাঁকা)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, আকার(অক্ষর *), 10);
    hash_init(&found_ht);
    জন্য (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	যদি (*s == NUL)
	{
	    যদি (did_curdir)
		বিরতি;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	অন্যথা
	{
#যদি defined(MSWIN)
	    e = vim_strchr(s, ';');
#অন্যথা
	    e = vim_strchr(s, ':');
#endif
	    যদি (e == ফাঁকা)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    যদি (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    অন্যথা
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? আকার(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	যদি (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    যদি (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		যদি (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, অঙ্ক_Matches, flags,
				    &found_ht, &ga);
	}

	যদি (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_তথ্য;
    *অঙ্ক_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    যদি (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    ফেরত OK;
}

#যদি defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    স্থির শূন্য *
call_user_expand_func(
    শূন্য	*(*user_expand_func)(char_u *, পূর্ণসংখ্যা, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    পূর্ণসংখ্যা		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = ফাঁকা;
    শূন্য	*ret;

    যদি (xp->xp_arg == ফাঁকা || xp->xp_arg[0] == '\0' || xp->xp_সারি == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (ccline->cmdbuff != ফাঁকা)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	পূর্ণসংখ্যা unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	যদি (pat == ফাঁকা)
	    pat = vim_strnsave(xp->xp_pattern, xp->xp_pattern_len);
    }

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = xp->xp_সারি;
    args[2].v_type = VAR_সংখ্যা;
    args[2].vval.v_সংখ্যা = xp->xp_col;
    args[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, args);

    current_sctx = save_current_sctx;
    যদি (ccline->cmdbuff != ফাঁকা)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    ফেরত ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    স্থির পূর্ণসংখ্যা
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    পূর্ণসংখ্যা		*অঙ্ক_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    পূর্ণসংখ্যা		keep;
    garray_T	ga;
    পূর্ণসংখ্যা		fuzzy;
    পূর্ণসংখ্যা		match;
    পূর্ণসংখ্যা		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ফাঁকা;
    *অঙ্ক_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    যদি (retstr == ফাঁকা)
	ফেরত FAIL;

    যদি (!fuzzy)
	ga_init2(&ga, আকার(অক্ষর *), 3);
    অন্যথা
	ga_init2(&ga, আকার(fuzmatch_str_T), 3);

    জন্য (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	যদি (e == ফাঁকা)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	যদি (xp->xp_pattern[0] != NUL)
	{
	    যদি (!fuzzy)
		match = vim_regexec(regmatch, s, (colnr_T)0);
	    অন্যথা
	    {
		score = fuzzy_match_str(s, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	অন্যথা
	    match = TRUE;		// match everything

	*e = keep;

	যদি (match)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    যদি (p == ফাঁকা)
		বিরতি;

	    যদি (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		বিরতি;
	    }

	    যদি (!fuzzy)
		((char_u **)ga.ga_তথ্য)[ga.ga_len] = p;
	    অন্যথা
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_তথ্য)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	যদি (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    যদি (ga.ga_len == 0)
	ফেরত OK;

    যদি (!fuzzy)
    {
	*matches = ga.ga_তথ্য;
	*অঙ্ক_Matches = ga.ga_len;
    }
    অন্যথা
    {
	যদি (fuzzymatches_to_strmatches(ga.ga_তথ্য, matches, ga.ga_len,
								FALSE) == FAIL)
	    ফেরত FAIL;
	*অঙ্ক_Matches = ga.ga_len;
    }
    ফেরত OK;
}

    শূন্য
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    পূর্ণসংখ্যা		*অঙ্ক_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    পূর্ণসংখ্যা		have_extra = FALSE;
    পূর্ণসংখ্যা		i;

    ga_init2(&ga, আকার(অক্ষর *), 3);
    ga_init2(&ga_abbr, আকার(অক্ষর *), 3);
    ga_init2(&ga_kind, আকার(অক্ষর *), 3);
    ga_init2(&ga_menu, আকার(অক্ষর *), 3);
    ga_init2(&ga_info, আকার(অক্ষর *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_বস্তুসমূহ(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = ফাঁকা;
	char_u	*abbr = ফাঁকা;
	char_u	*kind = ফাঁকা;
	char_u	*menu = ফাঁকা;
	char_u	*info = ফাঁকা;

	যদি (tv->v_type == VAR_STRING)
	{
	    যদি (tv->vval.v_string == ফাঁকা)
		চালু;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	অন্যথা যদি (tv->v_type == VAR_DICT && tv->vval.v_dict != ফাঁকা)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    যদি (word == ফাঁকা)
		চালু;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    যদি (abbr != ফাঁকা || kind != ফাঁকা || menu != ফাঁকা || info != ফাঁকা)
		have_extra = TRUE;
	}
	অন্যথা
	    চালু;  // Skip other types

	যদি (p == ফাঁকা
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
	    বিরতি;
	}

	((char_u **)ga.ga_তথ্য)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_তথ্য)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_তথ্য)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_তথ্য)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_তথ্য)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_তথ্য;
    *অঙ্ক_Matches = ga.ga_len;
    যদি (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_তথ্য;
	xp->xp_files_kind = (char_u **)ga_kind.ga_তথ্য;
	xp->xp_files_menu = (char_u **)ga_menu.ga_তথ্য;
	xp->xp_files_info = (char_u **)ga_info.ga_তথ্য;
    }
    অন্যথা
    {
	// No extra info collected; free the placeholder NULL entries.
	জন্য (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_তথ্য)[i]);
	vim_free(ga_abbr.ga_তথ্য);
	জন্য (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_তথ্য)[i]);
	vim_free(ga_kind.ga_তথ্য);
	জন্য (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_তথ্য)[i]);
	vim_free(ga_menu.ga_তথ্য);
	জন্য (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_তথ্য)[i]);
	vim_free(ga_info.ga_তথ্য);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    স্থির পূর্ণসংখ্যা
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    পূর্ণসংখ্যা		*অঙ্ক_Matches)
{
    list_T      *retlist;

    *matches = ফাঁকা;
    *অঙ্ক_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    যদি (retlist == ফাঁকা)
	ফেরত FAIL;

    expand_process_user_list(retlist, matches, অঙ্ক_Matches, xp);
    list_unref(retlist);
    ফেরত OK;
}
#endif

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    শূন্য
globpath(
    char_u	*path,
    char_u	*file,
    garray_T	*ga,
    পূর্ণসংখ্যা		expand_options,
    পূর্ণসংখ্যা		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    যদি (buf == ফাঁকা)
	ফেরত;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#যদি defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#অন্যথা
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    যখন (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	যদি (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    পূর্ণসংখ্যা	    অঙ্ক_p;
	    char_u  **p;

	    যদি (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    যদি (ExpandFromContext(&xpc, buf, &p, &অঙ্ক_p,
			     WILD_SILENT|expand_options) != FAIL && অঙ্ক_p > 0)
	    {
		ExpandEscape(&xpc, buf, অঙ্ক_p, p, WILD_SILENT|expand_options);

		যদি (ga_grow(ga, অঙ্ক_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_p; ++i)
		    {
			((char_u **)ga->ga_তথ্য)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		অন্যথা
		{
		    FreeWild(অঙ্ক_p, p);
		    p = ফাঁকা;
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
    পূর্ণসংখ্যা
wildmenu_translate_চাবি(
	cmdline_info_T	*cclp,
	পূর্ণসংখ্যা		চাবি,
	expand_T	*xp,
	পূর্ণসংখ্যা		did_wild_list)
{
    পূর্ণসংখ্যা c = চাবি;

    যদি (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	নির্বাচন (c)
	{
	    ক্ষেত্র K_UP:	  c = K_বাম; বিরতি;
	    ক্ষেত্র K_DOWN:  c = K_ডান; বিরতি;
	    ক্ষেত্র K_বাম:  c = K_UP; বিরতি;
	    ক্ষেত্র K_ডান: c = K_DOWN; বিরতি;
	    পূর্বনির্ধারিত:	  বিরতি;
	}
    }

    যদি (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	যদি (c == K_বাম)
	    c = Ctrl_P;
	অন্যথা যদি (c == K_ডান)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    যদি (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    ফেরত c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    স্থির শূন্য
cmdline_del(cmdline_info_T *cclp, পূর্ণসংখ্যা from)
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
    স্থির পূর্ণসংখ্যা
wildmenu_process_চাবি_menunames(cmdline_info_T *cclp, পূর্ণসংখ্যা চাবি, expand_T *xp)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    যদি (চাবি == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	চাবি = p_wc;
	চাবি_Typed = TRUE;  // in case the key was mapped
    }
    অন্যথা যদি (চাবি == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	পূর্ণসংখ্যা found = FALSE;

	j = (পূর্ণসংখ্যা)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	যখন (--j > 0)
	{
	    // check for start of menu name
	    যদি (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		বিরতি;
	    }
	    // check for start of submenu name
	    যদি (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		যদি (found)
		{
		    i = j + 1;
		    বিরতি;
		}
		অন্যথা
		    found = TRUE;
	    }
	}
	যদি (i > 0)
	    cmdline_del(cclp, i);
	চাবি = p_wc;
	চাবি_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    ফেরত চাবি;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    স্থির পূর্ণসংখ্যা
wildmenu_process_চাবি_filenames(cmdline_info_T *cclp, পূর্ণসংখ্যা চাবি, expand_T *xp)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    যদি (চাবি == K_DOWN
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	চাবি = p_wc;
	চাবি_Typed = TRUE;  // in case the key was mapped
    }
    অন্যথা যদি (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && চাবি == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	পূর্ণসংখ্যা found = FALSE;

	j = cclp->cmdpos;
	i = (পূর্ণসংখ্যা)(xp->xp_pattern - cclp->cmdbuff);
	যখন (--j > i)
	{
	    যদি (has_mbyte)
		j -= (*mb_শীর্ষ_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    যদি (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		বিরতি;
	    }
	}
	যদি (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    চাবি = p_wc;
	    চাবি_Typed = TRUE;  // in case the key was mapped
	}
    }
    অন্যথা যদি (চাবি == K_UP)
    {
	// go up a directory
	পূর্ণসংখ্যা found = FALSE;

	j = cclp->cmdpos - 1;
	i = (পূর্ণসংখ্যা)(xp->xp_pattern - cclp->cmdbuff);
	যখন (--j > i)
	{
	    যদি (has_mbyte)
		j -= (*mb_শীর্ষ_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    যদি (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == ফাঁকা
#endif
	       )
	    {
		যদি (found)
		{
		    i = j + 1;
		    বিরতি;
		}
		অন্যথা
		    found = TRUE;
	    }
	}

	যদি (!found)
	    j = i;
	অন্যথা যদি (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	অন্যথা যদি (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	অন্যথা
	    j = 0;
	যদি (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	অন্যথা যদি (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	চাবি = p_wc;
	চাবি_Typed = TRUE;
    }

    ফেরত চাবি;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    পূর্ণসংখ্যা
wildmenu_process_চাবি(cmdline_info_T *cclp, পূর্ণসংখ্যা চাবি, expand_T *xp)
{
    যদি (xp->xp_context == EXPAND_MENUNAMES)
	ফেরত wildmenu_process_চাবি_menunames(cclp, চাবি, xp);
    অন্যথা যদি ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	ফেরত wildmenu_process_চাবি_filenames(cclp, চাবি, xp);

    ফেরত চাবি;
}

/*
 * Free expanded names when finished walking through the matches
 */
    শূন্য
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    পূর্ণসংখ্যা skt = চাবি_Typed;

    যদি (!p_wmnu || wild_menu_showing == 0)
	ফেরত;

#ifdef FEAT_EVAL
    পূর্ণসংখ্যা save_RedrawingDisabled = RedrawingDisabled;
    যদি (cclp->প্রবেশ_fn)
	RedrawingDisabled = 0;
#endif

#যদি defined(FEAT_অনুসন্ধান_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    যদি (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    অন্যথা যদি (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	last_status(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    অন্যথা
    {
	win_redraw_last_status(topframe);
	redraw_statuslines();
    }
    চাবি_Typed = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    যদি (cclp->প্রবেশ_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#যদি defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    শূন্য
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    পূর্ণসংখ্যা		filtered = FALSE;
    পূর্ণসংখ্যা		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ফেরত;

    pat = tv_get_string(&argvars[0]);
    যদি (check_for_string_arg(argvars, 1) == FAIL)
	ফেরত;
    type = tv_get_string(&argvars[1]);

    যদি (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], ফাঁকা);

    যদি (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    যদি (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    যদি (STRCMP(type, "cmdline") == 0)
    {
	পূর্ণসংখ্যা cmdline_len = (পূর্ণসংখ্যা)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (পূর্ণসংখ্যা)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    অন্যথা
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (পূর্ণসংখ্যা)STRLEN(xpc.xp_pattern);
	xpc.xp_সারি = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	নির্বাচন (xpc.xp_context)
	{
	ক্ষেত্র EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    ফেরত;

	ক্ষেত্র EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    যদি (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		ফেরত;
	    }

	    xpc.xp_arg = type + 7;
	    বিরতি;

	ক্ষেত্র EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    যদি (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		ফেরত;
	    }

	    xpc.xp_arg = type + 11;
	    বিরতি;

# যদি defined(FEAT_MENU)
	ক্ষেত্র EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (পূর্ণসংখ্যা)(xpc.xp_pattern - pattern_start);
	    বিরতি;
# endif

# ifdef FEAT_CSCOPE
	ক্ষেত্র EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (পূর্ণসংখ্যা)(xpc.xp_pattern - pattern_start);
	    বিরতি;
# endif

# ifdef FEAT_SIGNS
	ক্ষেত্র EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (পূর্ণসংখ্যা)(xpc.xp_pattern - pattern_start);
	    বিরতি;
# endif

	ক্ষেত্র EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (পূর্ণসংখ্যা)(xpc.xp_pattern - pattern_start);
	    বিরতি;

	ক্ষেত্র EXPAND_SHELLCMDLINE:
	{
	    পূর্ণসংখ্যা context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(ফাঁকা, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (পূর্ণসংখ্যা)(xpc.xp_pattern - pattern_start);
	    বিরতি;
	}

	ক্ষেত্র EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    বিরতি;

	ক্ষেত্র EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    বিরতি;

	পূর্বনির্ধারিত:
	    বিরতি;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	পূর্ণসংখ্যা	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	যদি (unesc != ফাঁকা)
	    pat_src = unesc;

	যদি (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	অন্যথা
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    যদি (rettv_list_alloc(rettv) == OK && pat != ফাঁকা)
    {
	পূর্ণসংখ্যা	i;

	ExpandOne(&xpc, pat, ফাঁকা, options, WILD_ALL_KEEP);

	জন্য (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    শূন্য
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    পূর্ণসংখ্যা		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (পূর্ণসংখ্যা)STRLEN(pat);
    set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_to_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    শূন্য
f_cmdcomplete_info(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    পূর্ণসংখ্যা		    idx;
    পূর্ণসংখ্যা		    ret = OK;

    যদি (rettv_dict_alloc(rettv) == FAIL || ccline == ফাঁকা
	    || ccline->xpc == ফাঁকা || ccline->xpc->xp_files == ফাঁকা)
	ফেরত;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (পূর্ণসংখ্যা)cmdline_orig.দৈর্ঘ্য);
    যদি (ret == OK)
	ret = dict_add_সংখ্যা(retdict, "pum_visible", pum_visible());
    যদি (ret == OK)
	ret = dict_add_সংখ্যা(retdict, "selected", ccline->xpc->xp_selected);
    যদি (ret == OK)
    {
	li = list_alloc();
	যদি (li == ফাঁকা)
	    ফেরত;
	ret = dict_add_list(retdict, "matches", li);
	যদি (ret == FAIL)
	    list_unref(li);
	জন্য (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    স্থির পূর্ণসংখ্যা
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*সারি, *start_সারি, *end_সারি;
    পূর্ণসংখ্যা		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    পূর্ণসংখ্যা		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ফাঁকা;

    যদি (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	ফেরত FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_সারি = ml_get(start->lnum);
    char_u  *start_ptr = start_সারি + start->col;
    পূর্ণসংখ্যা	    is_single_সারি = start->lnum == end->lnum;

    segment_len = is_single_সারি ? (end->col - start->col)
			: (পূর্ণসংখ্যা)(ml_get_len(start->lnum) - start->col);
    যদি (ga_grow(&ga, segment_len + 2) != OK)
	যাও fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    যদি (!is_single_সারি)
    {
	যদি (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	অন্যথা
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    যদি (!is_single_সারি)
    {
	জন্য (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    পূর্ণসংখ্যা  linelen;

	    সারি = ml_get(lnum);
	    linelen = (পূর্ণসংখ্যা)ml_get_len(lnum);
	    যদি (ga_grow(&ga, linelen + 2) != OK)
		যাও fail;
	    ga_concat_len(&ga, সারি, linelen);
	    যদি (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    অন্যথা
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_সারি = ml_get(end->lnum);
    word_end = find_word_end(end_সারি + end->col);
    segment_len = (পূর্ণসংখ্যা)(word_end - end_সারি);
    যদি (ga_grow(&ga, segment_len) != OK)
	যাও fail;
    ga_concat_len(&ga, end_সারি + (is_single_সারি ? end->col : 0),
	    segment_len - (is_single_সারি ? end->col : 0));

    // Null-terminate
    যদি (ga_grow(&ga, 1) != OK)
	যাও fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_তথ্য;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    ফেরত OK;

fail:
    ga_clear(&ga);
    ফেরত FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    স্থির পূর্ণসংখ্যা
is_regex_match(char_u *pat, char_u *str)
{
    যদি (STRCMP(pat, str) == 0)
	ফেরত TRUE;

    regmatch_T	regmatch;
    পূর্ণসংখ্যা		ফলাফল;

    ++emsg_off;
    ++বার্তা_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --বার্তা_silent;

    যদি (regmatch.regprog == ফাঁকা)
	ফেরত FALSE;
    regmatch.rm_ic = p_ic;
    যদি (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++বার্তা_silent;
    ফলাফল = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --বার্তা_silent;

    vim_regfree(regmatch.regprog);
    ফেরত ফলাফল;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    স্থির char_u *
concat_pattern_with_বাফার_match(
	char_u *pat,
	পূর্ণসংখ্যা pat_len,
	pos_T *end_match_pos,
	পূর্ণসংখ্যা lowercase UNUSED)
{
    char_u  *সারি = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(সারি + end_match_pos->col);
    পূর্ণসংখ্যা	    match_len = (পূর্ণসংখ্যা)(word_end - (সারি + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    যদি (match == ফাঁকা)
	ফেরত ফাঁকা;
    mch_memmove(match, pat, pat_len);
    যদি (match_len > 0)
    {
#যদি defined(FEAT_EVAL) || defined(FEAT_SPELL)
	যদি (lowercase)
	{
	    char_u  *mword = vim_strnsave(সারি + end_match_pos->col,
		    match_len);
	    যদি (mword == ফাঁকা)
		যাও cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    যদি (lower == ফাঁকা)
		যাও cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	অন্যথা
#endif
	    mch_memmove(match + pat_len, সারি + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    ফেরত match;

#যদি defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    ফেরত ফাঁকা;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    স্থির পূর্ণসংখ্যা
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    পূর্ণসংখ্যা		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    পূর্ণসংখ্যা		*অঙ্ক_Matches)	    // return: number of matches
{
    pos_T	cur_match_pos, পূর্ব_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    পূর্ণসংখ্যা		found_new_match;
    পূর্ণসংখ্যা		looped_around = FALSE;
    পূর্ণসংখ্যা		pat_len;
    পূর্ণসংখ্যা		has_range = FALSE;
    পূর্ণসংখ্যা		compl_started = FALSE;
    পূর্ণসংখ্যা		অনুসন্ধান_flags;
    char_u	*match, *full_match;
    পূর্ণসংখ্যা		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ফাঁকা;

#ifdef FEAT_অনুসন্ধান_EXTRA
    has_range = অনুসন্ধান_first_সারি != 0;
#endif

    *matches = ফাঁকা;
    *অঙ্ক_Matches = 0;

    যদি (pat == ফাঁকা || *pat == NUL)
	ফেরত FAIL;

    pat_len = (পূর্ণসংখ্যা)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(পূর্ব_match_pos);
#ifdef FEAT_অনুসন্ধান_EXTRA
    যদি (has_range)
	cur_match_pos.lnum = অনুসন্ধান_first_সারি;
    অন্যথা
#endif
	cur_match_pos = pre_incsearch_pos;

    অনুসন্ধান_flags = অনুসন্ধান_OPT | অনুসন্ধান_NOOF | অনুসন্ধান_PEEK | অনুসন্ধান_NFMSG
	| (has_range ? অনুসন্ধান_START : 0);

    ga_init2(&ga, আকার(char_u *), 10); // Use growable array of char_u*

    জন্য (;;)
    {
	++emsg_off;
	++বার্তা_silent;
	found_new_match = searchit(ফাঁকা, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		অনুসন্ধান_flags, RE_LAST, ফাঁকা);
	--বার্তা_silent;
	--emsg_off;

	যদি (found_new_match == FAIL)
	    বিরতি;

#ifdef FEAT_অনুসন্ধান_EXTRA
	// If in range mode, check if match is within the range
	যদি (has_range && (cur_match_pos.lnum < অনুসন্ধান_first_সারি
		    || cur_match_pos.lnum > অনুসন্ধান_last_সারি))
		বিরতি;
#endif

	যদি (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    যদি ((dir == FORWARD && LTOREQ_POS(cur_match_pos, পূর্ব_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(পূর্ব_match_pos, cur_match_pos)))
	    {
		যদি (looped_around)
		    বিরতি;
		অন্যথা
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	পূর্ব_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	যদি (char_avail() || got_int)
	{
	    যদি (got_int)
	    {
		(শূন্য)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    যাও cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	যদি (end_match_pos.lnum > curbuf->b_ml.ml_সারি_গুনতি)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    চালু;
	}

	// Extract the matching text prepended to completed word
	যদি (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    বিরতি;

	যদি (exacttext)
	    match = full_match;
	অন্যথা
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_বাফার_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    যদি (match == ফাঁকা || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_বাফার_match(pat, pat_len,
			&end_match_pos, TRUE);
		যদি (match == ফাঁকা || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    চালু;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	জন্য (পূর্ণসংখ্যা i = 0; i < ga.ga_len; ++i)
	{
	    যদি (STRCMP(match, ((char_u **)ga.ga_তথ্য)[i]) == 0)
	    {
		VIM_CLEAR(match);
		বিরতি;
	    }
	}
	যদি (match != ফাঁকা)
	{
	    যদি (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		যাও cleanup;
	    }
	    ((char_u **)ga.ga_তথ্য)[ga.ga_len++] = match;
	    যদি (ga.ga_len > TAG_MANY)
		বিরতি;
	}
	যদি (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_তথ্য;
    *অঙ্ক_Matches = ga.ga_len;
    ফেরত OK;

cleanup:
    ga_clear_strings(&ga);
    ফেরত FAIL;
}
