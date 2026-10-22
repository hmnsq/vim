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

#অন্তর্ভাৱ "vim.h"

স্থির পূর্ণাঙ্ক	cmd_showtail;	// Only show path tail in lists ?
স্থির পূর্ণাঙ্ক	may_expand_pattern = FALSE;
স্থির pos_T	pre_incsearch_pos; // Cursor position when incsearch started

স্থির শূন্য	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, পূর্ণাঙ্ক usefilter, expand_T *xp, পূর্ণাঙ্ক *complp);
স্থির পূর্ণাঙ্ক	ExpandFromContext(expand_T *xp, char_u *, char_u ***, পূর্ণাঙ্ক *, পূর্ণাঙ্ক);
স্থির char_u	*showmatches_gettail(char_u *s);
স্থির পূর্ণাঙ্ক	expand_showtail(expand_T *xp);
স্থির পূর্ণাঙ্ক	expand_shellcmd(char_u *filepat, char_u ***matches, পূর্ণাঙ্ক *অঙ্ক_Matches, পূর্ণাঙ্ক flagsarg);
#যদি defined(FEAT_EVAL)
স্থির পূর্ণাঙ্ক	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, পূর্ণাঙ্ক *অঙ্ক_Matches);
স্থির পূর্ণাঙ্ক	ExpandUserList(expand_T *xp, char_u ***matches, পূর্ণাঙ্ক *অঙ্ক_Matches);
স্থির char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
স্থির char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, পূর্ণাঙ্ক srclen, পূর্ণাঙ্ক *new_lenp);
#endif
স্থির পূর্ণাঙ্ক	expand_pattern_in_buf(char_u *pat, পূর্ণাঙ্ক dir, char_u ***matches, পূর্ণাঙ্ক *অঙ্ক_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
স্থির pumitem_T *compl_match_array = রিক্ত;
স্থির পূর্ণাঙ্ক compl_match_arraysize;
// First column in cmdline of the matched item for completion.
স্থির পূর্ণাঙ্ক compl_startcol;
স্থির পূর্ণাঙ্ক compl_selected;
// cmdline before expansion
স্থির string_T cmdline_orig = {রিক্ত, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    স্থির পূর্ণাঙ্ক
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    ৱিকল্প (xp->xp_context)
    {
    পক্ষ EXPAND_BOOL_SETTINGS:
    পক্ষ EXPAND_COLORS:
    পক্ষ EXPAND_COMPILER:
    পক্ষ EXPAND_DIRECTORIES:
    পক্ষ EXPAND_DIRS_IN_CDPATH:
    পক্ষ EXPAND_FILES:
    পক্ষ EXPAND_FILES_IN_PATH:
    পক্ষ EXPAND_FILETYPE:
    পক্ষ EXPAND_FILETYPECMD:
    পক্ষ EXPAND_HELP:
    পক্ষ EXPAND_KEYMAP:
    পক্ষ EXPAND_OLD_SETTING:
    পক্ষ EXPAND_STRING_SETTING:
    পক্ষ EXPAND_SETTING_SUBTRACT:
    পক্ষ EXPAND_OWNSYNTAX:
    পক্ষ EXPAND_PACKADD:
    পক্ষ EXPAND_RUNTIME:
    পক্ষ EXPAND_SHELLCMD:
    পক্ষ EXPAND_SHELLCMDLINE:
    পক্ষ EXPAND_TAGS:
    পক্ষ EXPAND_TAGS_LISTFILES:
	ঘূৰাই FALSE;

    স্ৱভাৱ:
	ভঙ্গ;
    }

    ঘূৰাই vim_strchr(p_wop, WOP_FUZZY) != রিক্ত;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    পূর্ণাঙ্ক
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    ঘূৰাই vim_strchr(p_wop, WOP_FUZZY) != রিক্ত && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    স্থির পূর্ণাঙ্ক
sort_func_compare(ধ্রুৱ শূন্য *s1, ধ্রুৱ শূন্য *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    যদি (*p1 != '<' && *p2 == '<') ঘূৰাই -1;
    যদি (*p1 == '<' && *p2 != '<') ঘূৰাই 1;
    ঘূৰাই STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    স্থির শূন্য
wildescape(
    expand_T	*xp,
    char_u	*str,
    পূর্ণাঙ্ক		numfiles,
    char_u	**files)
{
    char_u	*p;
    পূর্ণাঙ্ক		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    যদি (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	বাবে (পূর্ণাঙ্ক i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    যদি (xp->xp_backslash & XP_BS_THREE)
	    {
		ৱর্ণ *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		যদি (p != রিক্ত)
		{
		    vim_free(files[i]);
		    files[i] = p;
#যদি defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    যদি (p != রিক্ত)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    অন্যথা যদি (xp->xp_backslash & XP_BS_COMMA)
	    {
		যদি (vim_strchr(files[i], ',') != রিক্ত)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    যদি (p != রিক্ত)
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
	    যদি (p != রিক্ত)
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
	বাবে (পূর্ণাঙ্ক i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    যদি (p != রিক্ত)
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
    পূর্ণাঙ্ক		numfiles,
    char_u	**files,
    পূর্ণাঙ্ক		options)
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
    পূর্ণাঙ্ক
nextwild(
    expand_T	*xp,
    পূর্ণাঙ্ক		type,
    পূর্ণাঙ্ক		options,	// extra options for ExpandOne()
    পূর্ণাঙ্ক		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    পূর্ণাঙ্ক		i;
    char_u	*p;
    পূর্ণাঙ্ক		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    পূর্ণাঙ্ক		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    যদি (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	যদি (ccline->প্ৰবেশ_fn && ccline->xp_context == EXPAND_COMMANDS)
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
	ঘূৰাই OK;  // Something illegal on command line
    }
    যদি (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ঘূৰাই FAIL;
    }

    i = (পূর্ণাঙ্ক)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    যদি (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	ঘূৰাই FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    যদি (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	বাৰ্তা_puts("...");	    // show that we are busy
	out_flush();
    }

    যদি (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, রিক্ত, রিক্ত, 0, type);
    }
    অন্যথা
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	পূর্ণাঙ্ক	pat_len = xp->xp_pattern_len;
#যদি defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	যদি (unesc != রিক্ত)
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
	যদি (tmp == রিক্ত)
	    p = রিক্ত;
	অন্যথা
	{
	    পূর্ণাঙ্ক use_options = options |
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
	    যদি (p != রিক্ত && type == WILD_LONGEST)
	    {
		পূর্ণাঙ্ক	j;

		বাবে (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    যদি (c == '*' || c == '?')
			ভঙ্গ;
		}
		যদি ((পূর্ণাঙ্ক)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    যদি (!wild_navigate && ccline->cmdbuff != রিক্ত)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	যদি (cmdline_orig.string == রিক্ত)
	    cmdline_orig.length = 0;
	অন্যথা
	    cmdline_orig.length = ccline->cmdlen;
    }

    যদি (p != রিক্ত && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	পূর্ণাঙ্ক	difflen;
	পূর্ণাঙ্ক	v = OK;

	difflen = (পূর্ণাঙ্ক)plen - xp->xp_pattern_len;
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
    যদি (xp->xp_context == EXPAND_MAPPINGS && p == রিক্ত)
	ঘূৰাই FAIL;

    যদি (xp->xp_numfiles <= 0 && p == রিক্ত)
	beep_flush();
    অন্যথা যদি (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(শূন্য)ExpandOne(xp, রিক্ত, রিক্ত, 0, WILD_FREE);

    vim_free(p);

    ঘূৰাই OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    স্থির পূর্ণাঙ্ক
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	পূর্ণাঙ্ক		অঙ্ক_Matches,
	পূর্ণাঙ্ক		showtail)
{
    পূর্ণাঙ্ক	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, অঙ্ক_Matches);
    যদি (compl_match_array == রিক্ত)
	ঘূৰাই EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = অঙ্ক_Matches;
    বাবে (পূর্ণাঙ্ক i = 0; i < অঙ্ক_Matches; i++)
    {
	compl_match_array[i].pum_text = (xp->xp_files_abbr != রিক্ত
				    && xp->xp_files_abbr[i] != রিক্ত)
					? xp->xp_files_abbr[i]
					: SHOW_MATCH(i);
	compl_match_array[i].pum_info = xp->xp_files_info != রিক্ত
					    ? xp->xp_files_info[i] : রিক্ত;
	compl_match_array[i].pum_extra = xp->xp_files_menu != রিক্ত
					    ? xp->xp_files_menu[i] : রিক্ত;
	compl_match_array[i].pum_kind = xp->xp_files_kind != রিক্ত
					    ? xp->xp_files_kind[i] : রিক্ত;
	compl_match_array[i].pum_user_abbr_hlattr = -1;
	compl_match_array[i].pum_user_kind_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == রিক্ত ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    prefix_len = vim_strsize(xp->xp_pattern);
    যদি (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    ঘূৰাই EXPAND_OK;
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
    পূর্ণাঙ্ক
cmdline_pum_active(শূন্য)
{
    ঘূৰাই pum_visible() && compl_match_array != রিক্ত;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    শূন্য
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, পূর্ণাঙ্ক defer_redraw)
{
    পূর্ণাঙ্ক	save_চাবি_Typed = চাবি_Typed;
#ifdef FEAT_EVAL
    পূর্ণাঙ্ক	save_RedrawingDisabled = RedrawingDisabled;
    যদি (cclp->প্ৰবেশ_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    যদি (!defer_redraw)
    {
	পূর্ণাঙ্ক save_p_lz = p_lz;
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
    যদি (cclp->প্ৰবেশ_fn)
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
    পূর্ণাঙ্ক
cmdline_compl_startcol(শূন্য)
{
    ঘূৰাই compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(শূন্য)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ঘূৰাই xp == রিক্ত ? রিক্ত : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    পূর্ণাঙ্ক
cmdline_compl_is_fuzzy(শূন্য)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ঘূৰাই xp != রিক্ত && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    স্থির পূর্ণাঙ্ক
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
	    ঘূৰাই 2;
#endif
	ঘূৰাই 1;
    }
    ঘূৰাই 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    স্থির পূর্ণাঙ্ক
status_match_len(expand_T *xp, char_u *s)
{
    পূর্ণাঙ্ক	len = 0;

#ifdef FEAT_MENU
    পূর্ণাঙ্ক emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    যদি (emenu && menu_is_separator(s))
	ঘূৰাই 1;
#endif

    যেতিয়ালৈ (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    ঘূৰাই len;
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
    পূর্ণাঙ্ক		অঙ্ক_matches,
    char_u	**matches,	// list of matches
    পূর্ণাঙ্ক		match,
    পূর্ণাঙ্ক		showtail)
{
    পূর্ণাঙ্ক		row;
    char_u	*buf;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		clen;		// length in screen cells
    পূর্ণাঙ্ক		fillchar;
    পূর্ণাঙ্ক		attr;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		highlight = TRUE;
    char_u	*selstart = রিক্ত;
    পূর্ণাঙ্ক		selstart_col = 0;
    char_u	*selend = রিক্ত;
    স্থির পূর্ণাঙ্ক	first_match = 0;
    পূর্ণাঙ্ক		add_বাম = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    পূর্ণাঙ্ক		emenu;
#endif
    পূর্ণাঙ্ক		l;

    যদি (matches == রিক্ত)	// interrupted completion?
	ঘূৰাই;

    যদি (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    অন্যথা
	buf = alloc(topframe->fr_width + 1);
    যদি (buf == রিক্ত)
	ঘূৰাই;

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
	বাবে (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	যদি (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	যদি (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    বাবে (i = match; i < অঙ্ক_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		যদি (clen >= topframe->fr_width)
		    ভঙ্গ;
	    }
	    যদি (i == অঙ্ক_matches)
		add_বাম = TRUE;
	}
    }
    যদি (add_বাম)
	যেতিয়ালৈ (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    যদি (clen >= topframe->fr_width)
		ভঙ্গ;
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
    যেতিয়ালৈ (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
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
	    l = (পূর্ণাঙ্ক)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	অন্যথা
#endif
	বাবে ( ; *s != NUL; ++s)
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
		len += (পূর্ণাঙ্ক)STRLEN(buf + len);
	    }
	}
	যদি (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	যদি (++i == অঙ্ক_matches)
		ভঙ্গ;
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
	    যদি (বাৰ্তা_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		যদি (cmdline_row == Rows - 1)
		{
		    screen_del_সাৰিগুলি(0, 0, 1, (পূর্ণাঙ্ক)Rows, TRUE, 0, রিক্ত);
		    ++বাৰ্তা_scrolled;
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
	যদি (selstart != রিক্ত && highlight)
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

    যদি (str == রিক্ত)
	ঘূৰাই রিক্ত;
    যদি ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ঘূৰাই str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    যদি (p == রিক্ত)
	ঘূৰাই str;
    vim_free(str);
    ঘূৰাই p;
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
    পূর্ণাঙ্ক		srclen,
    পূর্ণাঙ্ক		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    যদি ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ঘূৰাই রিক্ত;

    buf = alloc(srclen + 1);
    যদি (buf == রিক্ত)
	ঘূৰাই রিক্ত;

    d = buf;
    end = src + srclen;
    বাবে (p = src; p < end; ++p)
    {
	যদি (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (পূর্ণাঙ্ক)(d - buf);
    ঘূৰাই buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    স্থির char_u *
get_next_or_prev_match(পূর্ণাঙ্ক mode, expand_T *xp)
{
    পূর্ণাঙ্ক	    findex = xp->xp_selected;
    পূর্ণাঙ্ক	    ht;

    // When no matches found, return NULL
    যদি (xp->xp_numfiles <= 0)
	ঘূৰাই রিক্ত;

    যদি (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	যদি (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    অন্যথা যদি (mode == WILD_NEXT)
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
	যদি (xp->xp_orig != রিক্ত)
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
	অন্যথা যদি (vim_strchr(p_wop, WOP_PUM) != রিক্ত)
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
	ঘূৰাই vim_strsave(xp->xp_orig);
#যদি defined(FEAT_EVAL)
    ঘূৰাই apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#অন্যথা
    ঘূৰাই vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    স্থির char_u *
ExpandOne_start(পূর্ণাঙ্ক mode, expand_T *xp, char_u *str, পূর্ণাঙ্ক options)
{
    পূর্ণাঙ্ক		non_suf_match;		// number without matching suffix
    পূর্ণাঙ্ক		i;
    char_u	*ss = রিক্ত;

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
		বাবে (i = 0; i < 2; ++i)
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

    ঘূৰাই ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    স্থির char_u *
find_longest_match(expand_T *xp, পূর্ণাঙ্ক options)
{
    long_u	len;
    পূর্ণাঙ্ক		mb_len = 1;
    পূর্ণাঙ্ক		c0, ci;
    পূর্ণাঙ্ক		i;
    char_u	*ss;

    বাবে (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	যদি (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	অন্যথা
	    c0 = xp->xp_files[0][len];
	বাবে (i = 1; i < xp->xp_numfiles; ++i)
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
		    ভঙ্গ;
	    }
	    অন্যথা যদি (c0 != ci)
		ভঙ্গ;
	}
	যদি (i < xp->xp_numfiles)
	{
	    যদি (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    ভঙ্গ;
	}
    }

    ss = alloc(len + 1);
    যদি (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    ঘূৰাই ss;
}

    শূন্য
free_xp_files_extra(expand_T *xp, পূর্ণাঙ্ক numfiles)
{
    যদি (xp->xp_files_abbr != রিক্ত)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = রিক্ত;
    }
    যদি (xp->xp_files_kind != রিক্ত)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = রিক্ত;
    }
    যদি (xp->xp_files_menu != রিক্ত)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = রিক্ত;
    }
    যদি (xp->xp_files_info != রিক্ত)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = রিক্ত;
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
    পূর্ণাঙ্ক		options,
    পূর্ণাঙ্ক		mode)
{
    char_u	*ss = রিক্ত;
    পূর্ণাঙ্ক		orig_saved = FALSE;
#যদি defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    পূর্ণাঙ্ক		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    যদি (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	ঘূৰাই get_next_or_prev_match(mode, xp);

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
	যদি (compl_match_array != রিক্ত)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    যদি (mode == WILD_FREE)	// only release file name
	ঘূৰাই রিক্ত;

    যদি (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#যদি defined(FEAT_EVAL)
	যদি (ss != রিক্ত)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    যদি (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#যদি defined(FEAT_EVAL)
	যদি (ss != রিক্ত)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    যদি (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	ৱর্ণ	*prefix = "";
	ৱর্ণ	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	পূর্ণাঙ্ক	n = xp->xp_numfiles - 1;
	পূর্ণাঙ্ক	i;
#যদি defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = রিক্ত;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	যদি ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    যদি (escaped != রিক্ত)
	    {
		বাবে (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    যদি (escaped[i] == রিক্ত)
		    {
			যেতিয়ালৈ (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			ভঙ্গ;
		    }
		}
		যদি (escaped != রিক্ত)
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

	বাবে (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	যদি (ss != রিক্ত)
	{
	    size_t  ss_len = 0;

	    বাবে (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (ৱর্ণ *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (ৱর্ণ *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#যদি defined(FEAT_EVAL)
	যদি (escaped != রিক্ত)
	{
	    বাবে (i = 0; i < xp->xp_numfiles; ++i)
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
    ঘূৰাই ss;
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
	পূর্ণাঙ্ক		অঙ্ক_Matches,
	পূর্ণাঙ্ক		সাৰিগুলি,
	পূর্ণাঙ্ক		linenr,
	পূর্ণাঙ্ক		maxlen,
	পূর্ণাঙ্ক		showtail,
	পূর্ণাঙ্ক		dir_attr)
{
    পূর্ণাঙ্ক		i, j;
    পূর্ণাঙ্ক		isdir;
    পূর্ণাঙ্ক		lastlen;
    char_u	*p;

    lastlen = 999;
    বাবে (j = linenr; j < অঙ্ক_Matches; j += সাৰিগুলি)
    {
	যদি (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    বাৰ্তা_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    বাৰ্তা_advance(maxlen + 1);
	    বাৰ্তা_puts((ৱর্ণ *)p);
	    বাৰ্তা_advance(maxlen + 3);
	    বাৰ্তা_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    ভঙ্গ;
	}
	বাবে (i = maxlen - lastlen; --i >= 0; )
	    বাৰ্তা_putchar(' ');
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
		exp_path = expand_env_save_opt(matches[j], TRUE, রিক্ত);
		path = exp_path != রিক্ত ? exp_path : matches[j];
		halved_slash = backslash_halve_save(path);
		isdir = mch_isdir(halved_slash != রিক্ত ? halved_slash
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
		home_replace(রিক্ত, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	অন্যথা
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = বাৰ্তা_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    যদি (বাৰ্তা_col > 0)	// when not wrapped around
    {
	বাৰ্তা_clr_eos();
	বাৰ্তা_putchar('\n');
    }
    out_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    স্থির পূর্ণাঙ্ক
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	পূর্ণাঙ্ক		অঙ্ক_Matches,
	পূর্ণাঙ্ক		showtail,
	পূর্ণাঙ্ক		noselect)
{
    পূর্ণাঙ্ক	retval = cmdline_pum_create(ccline, xp, matches, অঙ্ক_Matches, showtail);

    যদি (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    ঘূৰাই retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    পূর্ণাঙ্ক
showmatches(
    expand_T	*xp,
    পূর্ণাঙ্ক		display_wildmenu,
    পূর্ণাঙ্ক		display_list,
    পূর্ণাঙ্ক		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    পূর্ণাঙ্ক		অঙ্ক_Matches;
    char_u	**matches;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		maxlen;
    পূর্ণাঙ্ক		সাৰিগুলি;
    পূর্ণাঙ্ক		columns;
    পূর্ণাঙ্ক		attr;
    পূর্ণাঙ্ক		showtail;
    পূর্ণাঙ্ক		noselect = (wim_flags_arg & WIM_NOSELECT);
    পূর্ণাঙ্ক		noinsert = (wim_flags_arg & WIM_NOINSERT);
    পূর্ণাঙ্ক		cmdline_unchanged = noselect || noinsert;

    যদি (xp->xp_numfiles == -1)
    {
	পূর্ণাঙ্ক retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&অঙ্ক_Matches, &matches);
	যদি (retval != EXPAND_OK)
	    ঘূৰাই retval;
	showtail = expand_showtail(xp);
    }
    অন্যথা
    {
	অঙ্ক_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    যদি (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != রিক্ত)
	ঘূৰাই show_pum_matches(ccline, xp, matches, অঙ্ক_Matches,
		showtail && !cmdline_unchanged, noselect);

    যদি (display_list)
    {
	বাৰ্তা_didany = FALSE;		// lines_left will be set
	বাৰ্তা_start();			// prepare for paging
	বাৰ্তা_putchar('\n');
	out_flush();
	cmdline_row = বাৰ্তা_row;
	বাৰ্তা_didany = FALSE;		// lines_left will be set again
	বাৰ্তা_start();			// prepare for paging
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
	বাবে (i = 0; i < অঙ্ক_Matches; ++i)
	{
	    পূর্ণাঙ্ক	len;
	    যদি (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(রিক্ত, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    অন্যথা
		len = vim_strsize(SHOW_MATCH(i));
	    যদি (len > maxlen)
		maxlen = len;
	}

	যদি (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    সাৰিগুলি = অঙ্ক_Matches;
	অন্যথা
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    যদি (columns < 1)
		columns = 1;
	    সাৰিগুলি = (অঙ্ক_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	যদি (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    বাৰ্তা_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    বাৰ্তা_clr_eos();
	    বাৰ্তা_advance(maxlen - 3);
	    বাৰ্তা_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	বাবে (i = 0; i < সাৰিগুলি; ++i)
	{
	    showmatches_oneline(xp, matches, অঙ্ক_Matches, সাৰিগুলি, i,
						maxlen, showtail, attr);
	    যদি (got_int)
	    {
		got_int = FALSE;
		ভঙ্গ;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = বাৰ্তা_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    যদি (display_wildmenu && display_list)
    {
	যদি (vim_strchr(p_wop, WOP_PUM) != রিক্ত)
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

    ঘূৰাই EXPAND_OK;
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
    পূর্ণাঙ্ক		had_sep = FALSE;

    বাবে (p = s; *p != NUL; )
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
    ঘূৰাই t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    স্থির পূর্ণাঙ্ক
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    যদি (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	ঘূৰাই FALSE;

    end = gettail(xp->xp_pattern);
    যদি (end == xp->xp_pattern)		// there is no path separator
	ঘূৰাই FALSE;

    বাবে (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	যদি (rem_backslash(s))
	    ++s;
	অন্যথা যদি (vim_strchr((char_u *)"*?[", *s) != রিক্ত)
	    ঘূৰাই FALSE;
    }
    ঘূৰাই TRUE;
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
    পূর্ণাঙ্ক		len,
    পূর্ণাঙ্ক		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    পূর্ণাঙ্ক		i, j;
    পূর্ণাঙ্ক		new_len;
    char_u	*tail;
    পূর্ণাঙ্ক		ends_in_star;

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
	    বাবে (i = 0; i < len; i++)
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
	    যদি (retval != রিক্ত)
	    {
		retval[0] = '^';
		j = 1;
		বাবে (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    যদি (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			ভঙ্গ;

		    ৱিকল্প (fname[i])
		    {
			পক্ষ '*':   retval[j++] = '.';
				    ভঙ্গ;
			পক্ষ '~':   retval[j++] = '\\';
				    ভঙ্গ;
			পক্ষ '?':   retval[j] = '.';
				    অনুৱর্ত;
			পক্ষ '.':   যদি (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    ভঙ্গ;
			পক্ষ '\\':  যদি (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    ভঙ্গ;
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
	যদি (retval != রিক্ত)
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
	    বাবে (i = len - 2; i >= 0; --i)
	    {
		যদি (retval[i] != '\\')
		    ভঙ্গ;
		ends_in_star = !ends_in_star;
	    }
#endif
	    যদি ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == রিক্ত
		    && vim_strchr(retval, '`') == রিক্ত)
		retval[len++] = '*';
	    অন্যথা যদি (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    ঘূৰাই retval;
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
	অনুসন্ধান_first_সাৰি = 0; // Search entire buffer
#endif
	ঘূৰাই;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    যদি (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->প্ৰবেশ_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	ঘূৰাই;
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
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, পূর্ণাঙ্ক *complp)
{
    char_u	*p = রিক্ত;
    পূর্ণাঙ্ক		len = 0;
    পূর্ণাঙ্ক		fuzzy = cmdline_fuzzy_complete(cmd);

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
	যেতিয়ালৈ (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	যদি (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    যেতিয়ালৈ (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	যদি (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    যেতিয়ালৈ (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	যদি (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != রিক্ত)
	    ++p;
	len = (পূর্ণাঙ্ক)(p - cmd);

	যদি (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    ঘূৰাই রিক্ত;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	যদি ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    যেতিয়ালৈ (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    যদি (*p == NUL && ASCII_ISALNUM(p[-1]))
	ঘূৰাই রিক্ত;

    যদি (eap->cmdidx == CMD_SIZE)
    {
	যদি (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != রিক্ত)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	অন্যথা যদি (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, রিক্ত, xp, complp);
	    যদি (p == রিক্ত)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    যদি (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ঘূৰাই রিক্ত;
    }

    ঘূৰাই p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    স্থির শূন্য
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	পূর্ণাঙ্ক		usefilter,
	expand_T	*xp,
	পূর্ণাঙ্ক		*complp)
{
    char_u	*p;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		in_quote = FALSE;
    char_u	*bow = রিক্ত;	// Beginning of word
    পূর্ণাঙ্ক		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    যেতিয়ালৈ (*p != NUL)
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
		    && (!(eap != রিক্ত && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    যেতিয়ালৈ (*p != NUL)
	    {
		যদি (has_mbyte)
		    c = mb_ptr2char(p);
		অন্যথা
		    c = *p;
		যদি (c == '`' || vim_isfilec_or_wc(c))
		    ভঙ্গ;
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
    যদি (bow != রিক্ত && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    যদি (usefilter
	    || (eap != রিক্ত
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
	বাবে (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    যদি (!vim_isIDc(*p))
		ভঙ্গ;
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
	বাবে (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
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
    যদি (p == রিক্ত)
	xp->xp_pattern = arg;
    অন্যথা
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    ঘূৰাই রিক্ত;
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
    যদি (p == রিক্ত)
	xp->xp_pattern = arg;
    অন্যথা
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    ঘূৰাই রিক্ত;
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
	arg = skip_vimgrep_pat(arg, রিক্ত, রিক্ত);
    যদি (arg == রিক্ত || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	ঘূৰাই রিক্ত;
    }
    ঘূৰাই skipwhite(arg);
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
    ঘূৰাই find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    স্থির char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    পূর্ণাঙ্ক		delim;

    delim = *arg;	    // get the delimiter
    যদি (delim)
	++arg;		    // skip delimiter if there is one

    যেতিয়ালৈ (arg[0] != NUL && arg[0] != delim)
    {
	যদি (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    যদি (arg[0] != NUL)
	ঘূৰাই arg + 1;

    ঘূৰাই রিক্ত;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    স্থির char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    পূর্ণাঙ্ক		delim;

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
	    যেতিয়ালৈ (arg[0] != NUL && arg[0] != delim)
	    {
		যদি (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    যদি (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    যেতিয়ালৈ (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == রিক্ত)
	++arg;
    যদি (arg[0] != NUL)
	ঘূৰাই arg;

    ঘূৰাই রিক্ত;
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
	ঘূৰাই রিক্ত;

    // Match regexp, not just whole words
    বাবে (++arg; *arg && *arg != '/'; arg++)
	যদি (*arg == '\\' && arg[1] != NUL)
	    arg++;
    যদি (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	যদি (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == রিক্ত)
	    xp->xp_context = EXPAND_NOTHING;
	অন্যথা
	    ঘূৰাই arg;
    }

    ঘূৰাই রিক্ত;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    স্থির char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    যেতিয়ালৈ ((xp->xp_pattern = vim_strchr(arg, ' ')) != রিক্ত)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    যদি (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    ঘূৰাই রিক্ত;
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

    ঘূৰাই রিক্ত;
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
	ঘূৰাই রিক্ত;
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
		ঘূৰাই রিক্ত;
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

    ঘূৰাই রিক্ত;
}

    স্থির char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = রিক্ত;

    p = skipwhite(arg);
    যদি (VIM_ISDIGIT(*p))
	ঘূৰাই রিক্ত;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    ঘূৰাই রিক্ত;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    স্থির char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    পূর্ণাঙ্ক    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    যদি (*p == NUL)
	ঘূৰাই রিক্ত;

    বাবে (;;)
    {
	যদি (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    অনুৱর্ত;
	}
	যদি (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    অনুৱর্ত;
	}
	ভঙ্গ;
    }

    যদি ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    অন্যথা যদি ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    অন্যথা যদি ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    ঘূৰাই রিক্ত;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    স্থির শূন্য
set_context_with_pattern(expand_T *xp)
{
    পূর্ণাঙ্ক		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_অনুসন্ধান_EXTRA
    পূর্ণাঙ্ক		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    যদি (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	ঘূৰাই;
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
	পূর্ণাঙ্ক		compl,
	পূর্ণাঙ্ক		forceit)
{
    char_u  *nextcmd;

    ৱিকল্প (cmdidx)
    {
	পক্ষ CMD_find:
	পক্ষ CMD_sfind:
	পক্ষ CMD_tabfind:
	    যদি (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    ভঙ্গ;
	পক্ষ CMD_cd:
	পক্ষ CMD_chdir:
	পক্ষ CMD_tcd:
	পক্ষ CMD_tchdir:
	পক্ষ CMD_lcd:
	পক্ষ CMD_lchdir:
	    যদি (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    ভঙ্গ;
	পক্ষ CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    ভঙ্গ;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	পক্ষ CMD_aboveleft:
	পক্ষ CMD_argdo:
	পক্ষ CMD_belowright:
	পক্ষ CMD_botright:
	পক্ষ CMD_browse:
	পক্ষ CMD_bufdo:
	পক্ষ CMD_cdo:
	পক্ষ CMD_cfdo:
	পক্ষ CMD_confirm:
	পক্ষ CMD_debug:
	পক্ষ CMD_folddoclosed:
	পক্ষ CMD_folddoopen:
	পক্ষ CMD_hide:
	পক্ষ CMD_horizontal:
	পক্ষ CMD_keepalt:
	পক্ষ CMD_keepjumps:
	পক্ষ CMD_keepmarks:
	পক্ষ CMD_keeppatterns:
	পক্ষ CMD_ldo:
	পক্ষ CMD_leftabove:
	পক্ষ CMD_lfdo:
	পক্ষ CMD_lockmarks:
	পক্ষ CMD_noautocmd:
	পক্ষ CMD_noswapfile:
	পক্ষ CMD_rightbelow:
	পক্ষ CMD_sandbox:
	পক্ষ CMD_silent:
	পক্ষ CMD_tab:
	পক্ষ CMD_tabdo:
	পক্ষ CMD_topleft:
	পক্ষ CMD_unsilent:
	পক্ষ CMD_verbose:
	পক্ষ CMD_vertical:
	পক্ষ CMD_windo:
	পক্ষ CMD_vim9cmd:
	পক্ষ CMD_legacy:
	    ঘূৰাই arg;

	পক্ষ CMD_filter:
	    ঘূৰাই set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_অনুসন্ধান_EXTRA
	পক্ষ CMD_match:
	    ঘূৰাই set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	পক্ষ CMD_command:
	    ঘূৰাই set_context_in_user_cmd(xp, arg);

	পক্ষ CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    ভঙ্গ;

	পক্ষ CMD_global:
	পক্ষ CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    যদি (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    ঘূৰাই nextcmd;

	পক্ষ CMD_and:
	পক্ষ CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    যদি (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    ঘূৰাই nextcmd;

	পক্ষ CMD_isearch:
	পক্ষ CMD_dsearch:
	পক্ষ CMD_ilist:
	পক্ষ CMD_dlist:
	পক্ষ CMD_ijump:
	পক্ষ CMD_psearch:
	পক্ষ CMD_djump:
	পক্ষ CMD_isplit:
	পক্ষ CMD_dsplit:
	    ঘূৰাই find_cmd_after_isearch_cmd(xp, arg);
	পক্ষ CMD_autocmd:
	    ঘূৰাই set_context_in_autocmd(xp, arg, FALSE);
	পক্ষ CMD_doautocmd:
	পক্ষ CMD_doautoall:
	    ঘূৰাই set_context_in_autocmd(xp, arg, TRUE);
	পক্ষ CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    ভঙ্গ;
	পক্ষ CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    ভঙ্গ;
	পক্ষ CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    ভঙ্গ;
	পক্ষ CMD_tag:
	পক্ষ CMD_stag:
	পক্ষ CMD_ptag:
	পক্ষ CMD_ltag:
	পক্ষ CMD_tselect:
	পক্ষ CMD_stselect:
	পক্ষ CMD_ptselect:
	পক্ষ CMD_tjump:
	পক্ষ CMD_stjump:
	পক্ষ CMD_ptjump:
	    যদি (vim_strchr(p_wop, WOP_TAGFILE) != রিক্ত)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    অন্যথা
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    ভঙ্গ;
	পক্ষ CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    ভঙ্গ;
#ifdef FEAT_SYN_HL
	পক্ষ CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    ভঙ্গ;
#endif
#ifdef FEAT_EVAL
	পক্ষ CMD_final:
	পক্ষ CMD_const:
	পক্ষ CMD_let:
	পক্ষ CMD_var:
	পক্ষ CMD_if:
	পক্ষ CMD_elseif:
	পক্ষ CMD_while:
	পক্ষ CMD_for:
	পক্ষ CMD_echo:
	পক্ষ CMD_echon:
	পক্ষ CMD_execute:
	পক্ষ CMD_echomsg:
	পক্ষ CMD_echoerr:
	পক্ষ CMD_echoconsole:
	পক্ষ CMD_echowindow:
	পক্ষ CMD_call:
	পক্ষ CMD_return:
	পক্ষ CMD_cexpr:
	পক্ষ CMD_caddexpr:
	পক্ষ CMD_cgetexpr:
	পক্ষ CMD_lexpr:
	পক্ষ CMD_laddexpr:
	পক্ষ CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    ভঙ্গ;

	পক্ষ CMD_unlet:
	    ঘূৰাই set_context_in_unlet_cmd(xp, arg);
	পক্ষ CMD_function:
	পক্ষ CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    ভঙ্গ;
	পক্ষ CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    ভঙ্গ;

	পক্ষ CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    ভঙ্গ;
#endif
	পক্ষ CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    ভঙ্গ;
#ifdef FEAT_CSCOPE
	পক্ষ CMD_cscope:
	পক্ষ CMD_lcscope:
	পক্ষ CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    ভঙ্গ;
#endif
#ifdef FEAT_SIGNS
	পক্ষ CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    ভঙ্গ;
#endif
	পক্ষ CMD_bdelete:
	পক্ষ CMD_bwipeout:
	পক্ষ CMD_bunload:
	    যেতিয়ালৈ ((xp->xp_pattern = vim_strchr(arg, ' ')) != রিক্ত)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	পক্ষ CMD_buffer:
	পক্ষ CMD_sbuffer:
	পক্ষ CMD_pbuffer:
	পক্ষ CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    ভঙ্গ;
#ifdef FEAT_DIFF
	পক্ষ CMD_diffget:
	পক্ষ CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    ভঙ্গ;
#endif
	পক্ষ CMD_USER:
	পক্ষ CMD_USER_BUF:
	    ঘূৰাই set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	পক্ষ CMD_map:	    পক্ষ CMD_noremap:
	পক্ষ CMD_nmap:	    পক্ষ CMD_nnoremap:
	পক্ষ CMD_vmap:	    পক্ষ CMD_vnoremap:
	পক্ষ CMD_omap:	    পক্ষ CMD_onoremap:
	পক্ষ CMD_imap:	    পক্ষ CMD_inoremap:
	পক্ষ CMD_cmap:	    পক্ষ CMD_cnoremap:
	পক্ষ CMD_lmap:	    পক্ষ CMD_lnoremap:
	পক্ষ CMD_smap:	    পক্ষ CMD_snoremap:
	পক্ষ CMD_tmap:	    পক্ষ CMD_tnoremap:
	পক্ষ CMD_xmap:	    পক্ষ CMD_xnoremap:
	    ঘূৰাই set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	পক্ষ CMD_unmap:
	পক্ষ CMD_nunmap:
	পক্ষ CMD_vunmap:
	পক্ষ CMD_ounmap:
	পক্ষ CMD_iunmap:
	পক্ষ CMD_cunmap:
	পক্ষ CMD_lunmap:
	পক্ষ CMD_sunmap:
	পক্ষ CMD_tunmap:
	পক্ষ CMD_xunmap:
	    ঘূৰাই set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	পক্ষ CMD_mapclear:
	পক্ষ CMD_nmapclear:
	পক্ষ CMD_vmapclear:
	পক্ষ CMD_omapclear:
	পক্ষ CMD_imapclear:
	পক্ষ CMD_cmapclear:
	পক্ষ CMD_lmapclear:
	পক্ষ CMD_smapclear:
	পক্ষ CMD_tmapclear:
	পক্ষ CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    ভঙ্গ;

	পক্ষ CMD_abbreviate:	পক্ষ CMD_noreabbrev:
	পক্ষ CMD_cabbrev:	পক্ষ CMD_cnoreabbrev:
	পক্ষ CMD_iabbrev:	পক্ষ CMD_inoreabbrev:
	    ঘূৰাই set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	পক্ষ CMD_unabbreviate:
	পক্ষ CMD_cunabbrev:
	পক্ষ CMD_iunabbrev:
	    ঘূৰাই set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	পক্ষ CMD_menu:	    পক্ষ CMD_noremenu:	    পক্ষ CMD_unmenu:
	পক্ষ CMD_amenu:	    পক্ষ CMD_anoremenu:	    পক্ষ CMD_aunmenu:
	পক্ষ CMD_nmenu:	    পক্ষ CMD_nnoremenu:	    পক্ষ CMD_nunmenu:
	পক্ষ CMD_vmenu:	    পক্ষ CMD_vnoremenu:	    পক্ষ CMD_vunmenu:
	পক্ষ CMD_omenu:	    পক্ষ CMD_onoremenu:	    পক্ষ CMD_ounmenu:
	পক্ষ CMD_imenu:	    পক্ষ CMD_inoremenu:	    পক্ষ CMD_iunmenu:
	পক্ষ CMD_cmenu:	    পক্ষ CMD_cnoremenu:	    পক্ষ CMD_cunmenu:
	পক্ষ CMD_tlmenu:    পক্ষ CMD_tlnoremenu:    পক্ষ CMD_tlunmenu:
	পক্ষ CMD_tmenu:				    পক্ষ CMD_tunmenu:
	পক্ষ CMD_popup:	    পক্ষ CMD_tearoff:	    পক্ষ CMD_emenu:
	    ঘূৰাই set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	পক্ষ CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    ভঙ্গ;

	পক্ষ CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    ভঙ্গ;

	পক্ষ CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    ভঙ্গ;

	পক্ষ CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    ভঙ্গ;

	পক্ষ CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    ভঙ্গ;

	পক্ষ CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    ভঙ্গ;

#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	পক্ষ CMD_language:
	    ঘূৰাই set_context_in_lang_cmd(xp, arg);
#endif
#যদি defined(FEAT_PROFILE)
	পক্ষ CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    ভঙ্গ;
#endif
	পক্ষ CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    ভঙ্গ;

	পক্ষ CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    ভঙ্গ;

	পক্ষ CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    ভঙ্গ;

	পক্ষ CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    ভঙ্গ;
#যদি defined(FEAT_PROFILE)
	পক্ষ CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    ভঙ্গ;
#endif

	পক্ষ CMD_argdelete:
	    যেতিয়ালৈ ((xp->xp_pattern = vim_strchr(arg, ' ')) != রিক্ত)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    ভঙ্গ;

#ifdef FEAT_EVAL
	পক্ষ CMD_breakadd:
	পক্ষ CMD_profdel:
	পক্ষ CMD_breakdel:
	    ঘূৰাই set_context_in_breakadd_cmd(xp, arg, cmdidx);

	পক্ষ CMD_scriptnames:
	    ঘূৰাই set_context_in_scriptnames_cmd(xp, arg);
#endif
	পক্ষ CMD_filetype:
	    ঘূৰাই set_context_in_filetype_cmd(xp, arg);

	স্ৱভাৱ:
	    ভঙ্গ;
    }
    ঘূৰাই রিক্ত;
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
    পূর্ণাঙ্ক			len = 0;
    exarg_T		ea;
    পূর্ণাঙ্ক			compl = EXPAND_NOTHING;
    পূর্ণাঙ্ক			forceit = FALSE;
    পূর্ণাঙ্ক			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_সাৰি = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    বাবে (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != রিক্ত; cmd++)
	;
    xp->xp_pattern = cmd;

    যদি (*cmd == NUL)
	ঘূৰাই রিক্ত;
    যদি (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	ঘূৰাই রিক্ত;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    যদি (*cmd == NUL)
	ঘূৰাই রিক্ত;
    যদি (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	ঘূৰাই রিক্ত;
    }

    যদি (*cmd == '|' || *cmd == '\n')
	ঘূৰাই cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    যদি (p == রিক্ত)
	ঘূৰাই রিক্ত;

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
	যেতিয়ালৈ (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    যেতিয়ালৈ (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    যদি (*p == NUL)
	    {
		যদি (ea.argt & EX_ARGOPT)
		    ঘূৰাই set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		যদি (ea.cmdidx == CMD_terminal)
		    ঘূৰাই set_context_in_terminalopt(xp, arg + 2);
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
	যেতিয়ালৈ (*arg == *cmd)	    // allow any number of '>' or '<'
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
	    ঘূৰাই p;

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
	যেতিয়ালৈ (*p)
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
			ঘূৰাই p + 1;
		    ঘূৰাই রিক্ত;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    যদি (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == রিক্ত)
	// no arguments allowed but there is something
	ঘূৰাই রিক্ত;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (পূর্ণাঙ্ক)STRLEN(buff);
    যেতিয়ালৈ (*p && p < buff + len)
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
    ঘূৰাই set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    শূন্য
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    পূর্ণাঙ্ক		len,	    // length of command line (excl. NUL)
    পূর্ণাঙ্ক		col,	    // position of cursor
    পূর্ণাঙ্ক		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    পূর্ণাঙ্ক			context;
#endif
    পূর্ণাঙ্ক		old_char = NUL;
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
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    অন্যথা যদি (use_ccline && ccline->প্ৰবেশ_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	যদি (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(রিক্ত, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    অন্যথা
#endif
	যেতিয়ালৈ (nextcomm != রিক্ত)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_সাৰি = str;
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
    পূর্ণাঙ্ক
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    পূর্ণাঙ্ক		col,		// position of cursor
    পূর্ণাঙ্ক		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = রিক্ত;
    পূর্ণাঙ্ক		options = WILD_ADD_SLASH|WILD_SILENT;

    যদি (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ঘূৰাই EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    যদি (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ঘূৰাই EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (পূর্ণাঙ্ক)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	পূর্ণাঙ্ক	pat_len = xp->xp_pattern_len;
#যদি defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	যদি (unesc != রিক্ত)
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
    যদি (file_str == রিক্ত)
	ঘূৰাই EXPAND_UNSUCCESSFUL;

    যদি (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    যদি (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = রিক্ত;
    }
    vim_free(file_str);

    ঘূৰাই EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	পূর্ণাঙ্ক		*অঙ্ক_Matches,
	পূর্ণাঙ্ক		flags,
	পূর্ণাঙ্ক		options)
{
    পূর্ণাঙ্ক		free_pat = FALSE;
    পূর্ণাঙ্ক		ret = FAIL;

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
	যদি (pat == রিক্ত)
	    ঘূৰাই ret;

	pat_end = pat + pat_len;
	বাবে (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    যদি (*p != '\\')
		অনুৱর্ত;

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
	পূর্ণাঙ্ক j;

	বাবে (j = 0; j < *অঙ্ক_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    যেতিয়ালৈ (*ptr != NUL)
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
    ঘূৰাই ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    স্থির char_u *
get_behave_arg(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx == 0)
	ঘূৰাই (char_u *)"mswin";
    যদি (idx == 1)
	ঘূৰাই (char_u *)"xterm";
    ঘূৰাই রিক্ত;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    স্থির char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx < 0)
	ঘূৰাই রিক্ত;

    যদি (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	ৱর্ণ	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	ঘূৰাই (char_u *)opts_all[idx];
    }
    যদি (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	ৱর্ণ	*opts_plugin[] = {"plugin", "on", "off"};
	ঘূৰাই (char_u *)opts_plugin[idx];
    }
    যদি (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	ৱর্ণ	*opts_indent[] = {"indent", "on", "off"};
	ঘূৰাই (char_u *)opts_indent[idx];
    }
    যদি (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	ৱর্ণ	*opts_onoff[] = {"on", "off"};
	ঘূৰাই (char_u *)opts_onoff[idx];
    }
    ঘূৰাই রিক্ত;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    স্থির char_u *
get_breakadd_arg(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx >= 0 && idx <= 3)
    {
	ৱর্ণ	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	যদি (breakpt_expand_what == EXP_BREAKPT_ADD)
	    ঘূৰাই (char_u *)opts[idx];
	অন্যথা যদি (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    যদি (idx <= 2)
		ঘূৰাই (char_u *)opts[idx + 1];
	}
	অন্যথা
	{
	    // profdel {func, file}
	    যদি (idx <= 1)
		ঘূৰাই (char_u *)opts[idx + 1];
	}
    }
    ঘূৰাই রিক্ত;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    স্থির char_u *
get_scriptnames_arg(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    scriptitem_T *si;

    যদি (!SCRIPT_ID_VALID(idx + 1))
	ঘূৰাই রিক্ত;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(রিক্ত, si->sn_name, NameBuff, MAXPATHL, TRUE);
    ঘূৰাই NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    স্থির char_u *
get_retab_arg(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx == 0)
	ঘূৰাই (char_u *)"-indentonly";
    ঘূৰাই রিক্ত;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    স্থির char_u *
get_messages_arg(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx == 0)
	ঘূৰাই (char_u *)"clear";
    ঘূৰাই রিক্ত;
}

    স্থির char_u *
get_mapclear_arg(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx == 0)
	ঘূৰাই (char_u *)"<buffer>";
    ঘূৰাই রিক্ত;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    স্থির পূর্ণাঙ্ক
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	পূর্ণাঙ্ক		*অঙ্ক_Matches)
{
    স্থির সংরচনা expgen
    {
	পূর্ণাঙ্ক		context;
	char_u	*((*func)(expand_T *, পূর্ণাঙ্ক));
	পূর্ণাঙ্ক		ic;
	পূর্ণাঙ্ক		escaped;
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
	{EXPAND_EXPRESSION, get_ৰাশিমালা_name, FALSE, TRUE},
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
    পূর্ণাঙ্ক	i;
    পূর্ণাঙ্ক ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(tab); ++i)
    {
	যদি (xp->xp_context == tab[i].context)
	{
	    যদি (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, অঙ্ক_Matches,
						tab[i].func, tab[i].escaped);
	    ভঙ্গ;
	}
    }

    ঘূৰাই ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    স্থির পূর্ণাঙ্ক
map_wildopts_to_ewflags(পূর্ণাঙ্ক options)
{
    পূর্ণাঙ্ক		flags;

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

    ঘূৰাই flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    স্থির পূর্ণাঙ্ক
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    পূর্ণাঙ্ক		*অঙ্ক_Matches,
    পূর্ণাঙ্ক		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    পূর্ণাঙ্ক		ret;
    পূর্ণাঙ্ক		flags;
    char_u	*tofree = রিক্ত;
    পূর্ণাঙ্ক		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    যদি (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	ঘূৰাই expand_files_and_dirs(xp, pat, matches, অঙ্ক_Matches, flags,
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
	    ঘূৰাই OK;
	}
	ঘূৰাই FAIL;
    }

    যদি (xp->xp_context == EXPAND_SHELLCMD)
	ঘূৰাই expand_shellcmd(pat, matches, অঙ্ক_Matches, flags);
    যদি (xp->xp_context == EXPAND_OLD_SETTING)
	ঘূৰাই ExpandOldSetting(অঙ্ক_Matches, matches);
    যদি (xp->xp_context == EXPAND_BUFFERS)
	ঘূৰাই ExpandBufnames(pat, অঙ্ক_Matches, matches, options);
#ifdef FEAT_DIFF
    যদি (xp->xp_context == EXPAND_DIFF_BUFFERS)
	ঘূৰাই ExpandBufnames(pat, অঙ্ক_Matches, matches,
						options | BUF_DIFF_FILTER);
#endif
    যদি (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	ঘূৰাই expand_tags(xp->xp_context == EXPAND_TAGS, pat, অঙ্ক_Matches,
								matches);
    যদি (xp->xp_context == EXPAND_COLORS)
    {
	ৱর্ণ *directories[] = {"colors", রিক্ত};
	ঘূৰাই ExpandRTDir(pat, DIP_START + DIP_OPT, অঙ্ক_Matches, matches,
								directories);
    }
    যদি (xp->xp_context == EXPAND_COMPILER)
    {
	ৱর্ণ *directories[] = {"compiler", রিক্ত};
	ঘূৰাই ExpandRTDir(pat, 0, অঙ্ক_Matches, matches, directories);
    }
    যদি (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	ৱর্ণ *directories[] = {"syntax", রিক্ত};
	ঘূৰাই ExpandRTDir(pat, 0, অঙ্ক_Matches, matches, directories);
    }
    যদি (xp->xp_context == EXPAND_FILETYPE)
    {
	ৱর্ণ *directories[] = {"syntax", "indent", "ftplugin", রিক্ত};
	ঘূৰাই ExpandRTDir(pat, 0, অঙ্ক_Matches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    যদি (xp->xp_context == EXPAND_KEYMAP)
    {
	ৱর্ণ *directories[] = {"keymap", রিক্ত};
	ঘূৰাই ExpandRTDir(pat, 0, অঙ্ক_Matches, matches, directories);
    }
#endif
#যদি defined(FEAT_EVAL)
    যদি (xp->xp_context == EXPAND_USER_LIST)
	ঘূৰাই ExpandUserList(xp, matches, অঙ্ক_Matches);
#endif
    যদি (xp->xp_context == EXPAND_PACKADD)
	ঘূৰাই ExpandPackAddDir(pat, অঙ্ক_Matches, matches);
    যদি (xp->xp_context == EXPAND_RUNTIME)
	ঘূৰাই expand_runtime_cmd(pat, অঙ্ক_Matches, matches);
    যদি (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	ঘূৰাই expand_pattern_in_buf(pat, xp->xp_অনুসন্ধান_dir,
		matches, অঙ্ক_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    যদি ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	পূর্ণাঙ্ক len = (পূর্ণাঙ্ক)STRLEN(pat) + 20;

	tofree = alloc(len);
	যদি (tofree == রিক্ত)
	    ঘূৰাই FAIL;
	vim_snprintf((ৱর্ণ *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    যদি (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	যদি (regmatch.regprog == রিক্ত)
	{
	    vim_free(tofree);
	    ঘূৰাই FAIL;
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

    ঘূৰাই ret;
}

    পূর্ণাঙ্ক
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    পূর্ণাঙ্ক		*অঙ্ক_Matches,
    char_u	*(*func)(expand_T *, পূর্ণাঙ্ক),
					  // returns a string from the list
    পূর্ণাঙ্ক		escaped)
{
    ঘূৰাই ExpandGenericExt(
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
    পূর্ণাঙ্ক
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    পূর্ণাঙ্ক		*অঙ্ক_Matches,
    char_u	*(*func)(expand_T *, পূর্ণাঙ্ক),
					  // returns a string from the list
    পূর্ণাঙ্ক		escaped,
    পূর্ণাঙ্ক		sortStartIdx)
{
    পূর্ণাঙ্ক		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = রিক্ত;
    পূর্ণাঙ্ক		score = 0;
    পূর্ণাঙ্ক		fuzzy;
    পূর্ণাঙ্ক		match;
    পূর্ণাঙ্ক		sort_matches = FALSE;
    পূর্ণাঙ্ক		funcsort = FALSE;
    পূর্ণাঙ্ক		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = রিক্ত;
    *অঙ্ক_Matches = 0;

    যদি (!fuzzy)
	ga_init2(&ga, পরিমাণ(ৱর্ণ *), 30);
    অন্যথা
	ga_init2(&ga, পরিমাণ(fuzmatch_str_T), 30);

    বাবে (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	যদি (str == রিক্ত)	    // end of list
	    ভঙ্গ;
	যদি (*str == NUL)	    // skip empty strings
	    অনুৱর্ত;

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
	    অনুৱর্ত;

	যদি (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	অন্যথা
	    str = vim_strsave(str);
	যদি (str == রিক্ত)
	{
	    যদি (!fuzzy)
	    {
		ga_clear_strings(&ga);
		ঘূৰাই FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    ঘূৰাই FAIL;
	}

	যদি (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    ভঙ্গ;
	}

	যদি (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	অন্যথা
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

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
	ঘূৰাই OK;

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
	    qsort((শূন্য *)ga.ga_data, (size_t)ga.ga_len, পরিমাণ(char_u *),
							   sort_func_compare);
	অন্যথা
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    যদি (!fuzzy)
    {
	*matches = ga.ga_data;
	*অঙ্ক_Matches = ga.ga_len;
    }
    অন্যথা
    {
	যদি (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    ঘূৰাই FAIL;
	*অঙ্ক_Matches = ga.ga_len;
    }

#যদি defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    ঘূৰাই OK;
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
	পূর্ণাঙ্ক		*অঙ্ক_Matches,
	পূর্ণাঙ্ক		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    যদি (expand_wildcards(1, &pathed_pattern, অঙ্ক_Matches, matches, flags) != OK)
	ঘূৰাই;

    যদি (ga_grow(gap, *অঙ্ক_Matches) == FAIL)
    {
	FreeWild(*অঙ্ক_Matches, *matches);
	ঘূৰাই;
    }

    বাবে (পূর্ণাঙ্ক i = 0; i < *অঙ্ক_Matches; ++i)
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
		((char_u **)gap->ga_data)[gap->ga_len++] = name;
		hash_add_item(ht, hi, name, hash);
		name = রিক্ত;
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
    স্থির পূর্ণাঙ্ক
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    পূর্ণাঙ্ক		*অঙ্ক_Matches,	// return: number of matches
    পূর্ণাঙ্ক		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = রিক্ত;
    পূর্ণাঙ্ক		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    পূর্ণাঙ্ক		flags = flagsarg;
    পূর্ণাঙ্ক		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    যদি (buf == রিক্ত)
	ঘূৰাই FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    যদি (pat == রিক্ত)
    {
	vim_free(buf);
	ঘূৰাই FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    বাবে (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	যদি (*s != '\\')
	    অনুৱর্ত;

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
	যদি (path == রিক্ত)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, পরিমাণ(ৱর্ণ *), 10);
    hash_init(&found_ht);
    বাবে (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	যদি (*s == NUL)
	{
	    যদি (did_curdir)
		ভঙ্গ;

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
	    যদি (e == রিক্ত)
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

	    seplen = !after_pathsep(s, e) ? পরিমাণ(PATHSEP) : 0;
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
    *matches = ga.ga_data;
    *অঙ্ক_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    যদি (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    ঘূৰাই OK;
}

#যদি defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    স্থির শূন্য *
call_user_expand_func(
    শূন্য	*(*user_expand_func)(char_u *, পূর্ণাঙ্ক, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    পূর্ণাঙ্ক		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = রিক্ত;
    শূন্য	*ret;

    যদি (xp->xp_arg == রিক্ত || xp->xp_arg[0] == '\0' || xp->xp_সাৰি == রিক্ত)
	ঘূৰাই রিক্ত;

    যদি (ccline->cmdbuff != রিক্ত)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	পূর্ণাঙ্ক unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	যদি (pat == রিক্ত)
	    pat = vim_strnsave(xp->xp_pattern, xp->xp_pattern_len);
    }

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = xp->xp_সাৰি;
    args[2].v_type = VAR_সংখ্যা;
    args[2].vval.v_সংখ্যা = xp->xp_col;
    args[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, args);

    current_sctx = save_current_sctx;
    যদি (ccline->cmdbuff != রিক্ত)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    ঘূৰাই ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    স্থির পূর্ণাঙ্ক
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    পূর্ণাঙ্ক		*অঙ্ক_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    পূর্ণাঙ্ক		keep;
    garray_T	ga;
    পূর্ণাঙ্ক		fuzzy;
    পূর্ণাঙ্ক		match;
    পূর্ণাঙ্ক		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = রিক্ত;
    *অঙ্ক_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    যদি (retstr == রিক্ত)
	ঘূৰাই FAIL;

    যদি (!fuzzy)
	ga_init2(&ga, পরিমাণ(ৱর্ণ *), 3);
    অন্যথা
	ga_init2(&ga, পরিমাণ(fuzmatch_str_T), 3);

    বাবে (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	যদি (e == রিক্ত)
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
	    যদি (p == রিক্ত)
		ভঙ্গ;

	    যদি (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		ভঙ্গ;
	    }

	    যদি (!fuzzy)
		((char_u **)ga.ga_data)[ga.ga_len] = p;
	    অন্যথা
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_data)[ga.ga_len];
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
	ঘূৰাই OK;

    যদি (!fuzzy)
    {
	*matches = ga.ga_data;
	*অঙ্ক_Matches = ga.ga_len;
    }
    অন্যথা
    {
	যদি (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    ঘূৰাই FAIL;
	*অঙ্ক_Matches = ga.ga_len;
    }
    ঘূৰাই OK;
}

    শূন্য
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    পূর্ণাঙ্ক		*অঙ্ক_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    পূর্ণাঙ্ক		have_extra = FALSE;
    পূর্ণাঙ্ক		i;

    ga_init2(&ga, পরিমাণ(ৱর্ণ *), 3);
    ga_init2(&ga_abbr, পরিমাণ(ৱর্ণ *), 3);
    ga_init2(&ga_kind, পরিমাণ(ৱর্ণ *), 3);
    ga_init2(&ga_menu, পরিমাণ(ৱর্ণ *), 3);
    ga_init2(&ga_info, পরিমাণ(ৱর্ণ *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_বস্তুসমূহ(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = রিক্ত;
	char_u	*abbr = রিক্ত;
	char_u	*kind = রিক্ত;
	char_u	*menu = রিক্ত;
	char_u	*info = রিক্ত;

	যদি (tv->v_type == VAR_STRING)
	{
	    যদি (tv->vval.v_string == রিক্ত)
		অনুৱর্ত;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	অন্যথা যদি (tv->v_type == VAR_DICT && tv->vval.v_dict != রিক্ত)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    যদি (word == রিক্ত)
		অনুৱর্ত;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    যদি (abbr != রিক্ত || kind != রিক্ত || menu != রিক্ত || info != রিক্ত)
		have_extra = TRUE;
	}
	অন্যথা
	    অনুৱর্ত;  // Skip other types

	যদি (p == রিক্ত
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
	    ভঙ্গ;
	}

	((char_u **)ga.ga_data)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_data)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_data)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_data)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_data)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_data;
    *অঙ্ক_Matches = ga.ga_len;
    যদি (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    অন্যথা
    {
	// No extra info collected; free the placeholder NULL entries.
	বাবে (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	বাবে (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	বাবে (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	বাবে (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    স্থির পূর্ণাঙ্ক
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    পূর্ণাঙ্ক		*অঙ্ক_Matches)
{
    list_T      *retlist;

    *matches = রিক্ত;
    *অঙ্ক_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    যদি (retlist == রিক্ত)
	ঘূৰাই FAIL;

    expand_process_user_list(retlist, matches, অঙ্ক_Matches, xp);
    list_unref(retlist);
    ঘূৰাই OK;
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
    পূর্ণাঙ্ক		expand_options,
    পূর্ণাঙ্ক		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    যদি (buf == রিক্ত)
	ঘূৰাই;

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
    যেতিয়ালৈ (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	যদি (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    পূর্ণাঙ্ক	    অঙ্ক_p;
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
		    বাবে (পূর্ণাঙ্ক i = 0; i < অঙ্ক_p; ++i)
		    {
			((char_u **)ga->ga_data)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		অন্যথা
		{
		    FreeWild(অঙ্ক_p, p);
		    p = রিক্ত;
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
    পূর্ণাঙ্ক
wildmenu_translate_চাবি(
	cmdline_info_T	*cclp,
	পূর্ণাঙ্ক		চাবি,
	expand_T	*xp,
	পূর্ণাঙ্ক		did_wild_list)
{
    পূর্ণাঙ্ক c = চাবি;

    যদি (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	ৱিকল্প (c)
	{
	    পক্ষ K_UP:	  c = K_বাম; ভঙ্গ;
	    পক্ষ K_DOWN:  c = K_ডান; ভঙ্গ;
	    পক্ষ K_বাম:  c = K_UP; ভঙ্গ;
	    পক্ষ K_ডান: c = K_DOWN; ভঙ্গ;
	    স্ৱভাৱ:	  ভঙ্গ;
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

    ঘূৰাই c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    স্থির শূন্য
cmdline_del(cmdline_info_T *cclp, পূর্ণাঙ্ক from)
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
    স্থির পূর্ণাঙ্ক
wildmenu_process_চাবি_menunames(cmdline_info_T *cclp, পূর্ণাঙ্ক চাবি, expand_T *xp)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		j;

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
	পূর্ণাঙ্ক found = FALSE;

	j = (পূর্ণাঙ্ক)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	যেতিয়ালৈ (--j > 0)
	{
	    // check for start of menu name
	    যদি (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		ভঙ্গ;
	    }
	    // check for start of submenu name
	    যদি (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		যদি (found)
		{
		    i = j + 1;
		    ভঙ্গ;
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

    ঘূৰাই চাবি;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    স্থির পূর্ণাঙ্ক
wildmenu_process_চাবি_filenames(cmdline_info_T *cclp, পূর্ণাঙ্ক চাবি, expand_T *xp)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		j;
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
	পূর্ণাঙ্ক found = FALSE;

	j = cclp->cmdpos;
	i = (পূর্ণাঙ্ক)(xp->xp_pattern - cclp->cmdbuff);
	যেতিয়ালৈ (--j > i)
	{
	    যদি (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    যদি (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		ভঙ্গ;
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
	পূর্ণাঙ্ক found = FALSE;

	j = cclp->cmdpos - 1;
	i = (পূর্ণাঙ্ক)(xp->xp_pattern - cclp->cmdbuff);
	যেতিয়ালৈ (--j > i)
	{
	    যদি (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    যদি (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == রিক্ত
#endif
	       )
	    {
		যদি (found)
		{
		    i = j + 1;
		    ভঙ্গ;
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

    ঘূৰাই চাবি;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    পূর্ণাঙ্ক
wildmenu_process_চাবি(cmdline_info_T *cclp, পূর্ণাঙ্ক চাবি, expand_T *xp)
{
    যদি (xp->xp_context == EXPAND_MENUNAMES)
	ঘূৰাই wildmenu_process_চাবি_menunames(cclp, চাবি, xp);
    অন্যথা যদি ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	ঘূৰাই wildmenu_process_চাবি_filenames(cclp, চাবি, xp);

    ঘূৰাই চাবি;
}

/*
 * Free expanded names when finished walking through the matches
 */
    শূন্য
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    পূর্ণাঙ্ক skt = চাবি_Typed;

    যদি (!p_wmnu || wild_menu_showing == 0)
	ঘূৰাই;

#ifdef FEAT_EVAL
    পূর্ণাঙ্ক save_RedrawingDisabled = RedrawingDisabled;
    যদি (cclp->প্ৰবেশ_fn)
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
    যদি (cclp->প্ৰবেশ_fn)
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
    পূর্ণাঙ্ক		filtered = FALSE;
    পূর্ণাঙ্ক		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    pat = tv_get_string(&argvars[0]);
    যদি (check_for_string_arg(argvars, 1) == FAIL)
	ঘূৰাই;
    type = tv_get_string(&argvars[1]);

    যদি (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], রিক্ত);

    যদি (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    যদি (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    যদি (STRCMP(type, "cmdline") == 0)
    {
	পূর্ণাঙ্ক cmdline_len = (পূর্ণাঙ্ক)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (পূর্ণাঙ্ক)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    অন্যথা
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (পূর্ণাঙ্ক)STRLEN(xpc.xp_pattern);
	xpc.xp_সাৰি = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	ৱিকল্প (xpc.xp_context)
	{
	পক্ষ EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    ঘূৰাই;

	পক্ষ EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    যদি (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		ঘূৰাই;
	    }

	    xpc.xp_arg = type + 7;
	    ভঙ্গ;

	পক্ষ EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    যদি (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		ঘূৰাই;
	    }

	    xpc.xp_arg = type + 11;
	    ভঙ্গ;

# যদি defined(FEAT_MENU)
	পক্ষ EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (পূর্ণাঙ্ক)(xpc.xp_pattern - pattern_start);
	    ভঙ্গ;
# endif

# ifdef FEAT_CSCOPE
	পক্ষ EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (পূর্ণাঙ্ক)(xpc.xp_pattern - pattern_start);
	    ভঙ্গ;
# endif

# ifdef FEAT_SIGNS
	পক্ষ EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (পূর্ণাঙ্ক)(xpc.xp_pattern - pattern_start);
	    ভঙ্গ;
# endif

	পক্ষ EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (পূর্ণাঙ্ক)(xpc.xp_pattern - pattern_start);
	    ভঙ্গ;

	পক্ষ EXPAND_SHELLCMDLINE:
	{
	    পূর্ণাঙ্ক context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(রিক্ত, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (পূর্ণাঙ্ক)(xpc.xp_pattern - pattern_start);
	    ভঙ্গ;
	}

	পক্ষ EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    ভঙ্গ;

	পক্ষ EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    ভঙ্গ;

	স্ৱভাৱ:
	    ভঙ্গ;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	পূর্ণাঙ্ক	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	যদি (unesc != রিক্ত)
	    pat_src = unesc;

	যদি (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	অন্যথা
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    যদি (rettv_list_alloc(rettv) == OK && pat != রিক্ত)
    {
	পূর্ণাঙ্ক	i;

	ExpandOne(&xpc, pat, রিক্ত, options, WILD_ALL_KEEP);

	বাবে (i = 0; i < xpc.xp_numfiles; i++)
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
    পূর্ণাঙ্ক		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;

    যদি (check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (পূর্ণাঙ্ক)STRLEN(pat);
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
    পূর্ণাঙ্ক		    idx;
    পূর্ণাঙ্ক		    ret = OK;

    যদি (rettv_dict_alloc(rettv) == FAIL || ccline == রিক্ত
	    || ccline->xpc == রিক্ত || ccline->xpc->xp_files == রিক্ত)
	ঘূৰাই;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (পূর্ণাঙ্ক)cmdline_orig.length);
    যদি (ret == OK)
	ret = dict_add_সংখ্যা(retdict, "pum_visible", pum_visible());
    যদি (ret == OK)
	ret = dict_add_সংখ্যা(retdict, "selected", ccline->xpc->xp_selected);
    যদি (ret == OK)
    {
	li = list_alloc();
	যদি (li == রিক্ত)
	    ঘূৰাই;
	ret = dict_add_list(retdict, "matches", li);
	যদি (ret == FAIL)
	    list_unref(li);
	বাবে (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    স্থির পূর্ণাঙ্ক
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*সাৰি, *start_সাৰি, *end_সাৰি;
    পূর্ণাঙ্ক		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    পূর্ণাঙ্ক		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != রিক্ত;

    যদি (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	ঘূৰাই FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_সাৰি = ml_get(start->lnum);
    char_u  *start_ptr = start_সাৰি + start->col;
    পূর্ণাঙ্ক	    is_single_সাৰি = start->lnum == end->lnum;

    segment_len = is_single_সাৰি ? (end->col - start->col)
			: (পূর্ণাঙ্ক)(ml_get_len(start->lnum) - start->col);
    যদি (ga_grow(&ga, segment_len + 2) != OK)
	গচ্ছ fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    যদি (!is_single_সাৰি)
    {
	যদি (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	অন্যথা
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    যদি (!is_single_সাৰি)
    {
	বাবে (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    পূর্ণাঙ্ক  linelen;

	    সাৰি = ml_get(lnum);
	    linelen = (পূর্ণাঙ্ক)ml_get_len(lnum);
	    যদি (ga_grow(&ga, linelen + 2) != OK)
		গচ্ছ fail;
	    ga_concat_len(&ga, সাৰি, linelen);
	    যদি (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    অন্যথা
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_সাৰি = ml_get(end->lnum);
    word_end = find_word_end(end_সাৰি + end->col);
    segment_len = (পূর্ণাঙ্ক)(word_end - end_সাৰি);
    যদি (ga_grow(&ga, segment_len) != OK)
	গচ্ছ fail;
    ga_concat_len(&ga, end_সাৰি + (is_single_সাৰি ? end->col : 0),
	    segment_len - (is_single_সাৰি ? end->col : 0));

    // Null-terminate
    যদি (ga_grow(&ga, 1) != OK)
	গচ্ছ fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    ঘূৰাই OK;

fail:
    ga_clear(&ga);
    ঘূৰাই FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    স্থির পূর্ণাঙ্ক
is_regex_match(char_u *pat, char_u *str)
{
    যদি (STRCMP(pat, str) == 0)
	ঘূৰাই TRUE;

    regmatch_T	regmatch;
    পূর্ণাঙ্ক		result;

    ++emsg_off;
    ++বাৰ্তা_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --বাৰ্তা_silent;

    যদি (regmatch.regprog == রিক্ত)
	ঘূৰাই FALSE;
    regmatch.rm_ic = p_ic;
    যদি (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++বাৰ্তা_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --বাৰ্তা_silent;

    vim_regfree(regmatch.regprog);
    ঘূৰাই result;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    স্থির char_u *
concat_pattern_with_buffer_match(
	char_u *pat,
	পূর্ণাঙ্ক pat_len,
	pos_T *end_match_pos,
	পূর্ণাঙ্ক lowercase UNUSED)
{
    char_u  *সাৰি = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(সাৰি + end_match_pos->col);
    পূর্ণাঙ্ক	    match_len = (পূর্ণাঙ্ক)(word_end - (সাৰি + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    যদি (match == রিক্ত)
	ঘূৰাই রিক্ত;
    mch_memmove(match, pat, pat_len);
    যদি (match_len > 0)
    {
#যদি defined(FEAT_EVAL) || defined(FEAT_SPELL)
	যদি (lowercase)
	{
	    char_u  *mword = vim_strnsave(সাৰি + end_match_pos->col,
		    match_len);
	    যদি (mword == রিক্ত)
		গচ্ছ cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    যদি (lower == রিক্ত)
		গচ্ছ cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	অন্যথা
#endif
	    mch_memmove(match + pat_len, সাৰি + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    ঘূৰাই match;

#যদি defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    ঘূৰাই রিক্ত;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    স্থির পূর্ণাঙ্ক
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    পূর্ণাঙ্ক		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    পূর্ণাঙ্ক		*অঙ্ক_Matches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    পূর্ণাঙ্ক		found_new_match;
    পূর্ণাঙ্ক		looped_around = FALSE;
    পূর্ণাঙ্ক		pat_len;
    পূর্ণাঙ্ক		has_range = FALSE;
    পূর্ণাঙ্ক		compl_started = FALSE;
    পূর্ণাঙ্ক		অনুসন্ধান_flags;
    char_u	*match, *full_match;
    পূর্ণাঙ্ক		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != রিক্ত;

#ifdef FEAT_অনুসন্ধান_EXTRA
    has_range = অনুসন্ধান_first_সাৰি != 0;
#endif

    *matches = রিক্ত;
    *অঙ্ক_Matches = 0;

    যদি (pat == রিক্ত || *pat == NUL)
	ঘূৰাই FAIL;

    pat_len = (পূর্ণাঙ্ক)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_অনুসন্ধান_EXTRA
    যদি (has_range)
	cur_match_pos.lnum = অনুসন্ধান_first_সাৰি;
    অন্যথা
#endif
	cur_match_pos = pre_incsearch_pos;

    অনুসন্ধান_flags = অনুসন্ধান_OPT | অনুসন্ধান_NOOF | অনুসন্ধান_PEEK | অনুসন্ধান_NFMSG
	| (has_range ? অনুসন্ধান_START : 0);

    ga_init2(&ga, পরিমাণ(char_u *), 10); // Use growable array of char_u*

    বাবে (;;)
    {
	++emsg_off;
	++বাৰ্তা_silent;
	found_new_match = searchit(রিক্ত, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		অনুসন্ধান_flags, RE_LAST, রিক্ত);
	--বাৰ্তা_silent;
	--emsg_off;

	যদি (found_new_match == FAIL)
	    ভঙ্গ;

#ifdef FEAT_অনুসন্ধান_EXTRA
	// If in range mode, check if match is within the range
	যদি (has_range && (cur_match_pos.lnum < অনুসন্ধান_first_সাৰি
		    || cur_match_pos.lnum > অনুসন্ধান_last_সাৰি))
		ভঙ্গ;
#endif

	যদি (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    যদি ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		যদি (looped_around)
		    ভঙ্গ;
		অন্যথা
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	যদি (char_avail() || got_int)
	{
	    যদি (got_int)
	    {
		(শূন্য)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    গচ্ছ cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	যদি (end_match_pos.lnum > curbuf->b_ml.ml_সাৰি_গুনতি)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    অনুৱর্ত;
	}

	// Extract the matching text prepended to completed word
	যদি (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    ভঙ্গ;

	যদি (exacttext)
	    match = full_match;
	অন্যথা
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    যদি (match == রিক্ত || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		যদি (match == রিক্ত || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    অনুৱর্ত;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	বাবে (পূর্ণাঙ্ক i = 0; i < ga.ga_len; ++i)
	{
	    যদি (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		ভঙ্গ;
	    }
	}
	যদি (match != রিক্ত)
	{
	    যদি (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		গচ্ছ cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    যদি (ga.ga_len > TAG_MANY)
		ভঙ্গ;
	}
	যদি (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *অঙ্ক_Matches = ga.ga_len;
    ঘূৰাই OK;

cleanup:
    ga_clear_strings(&ga);
    ঘূৰাই FAIL;
}
