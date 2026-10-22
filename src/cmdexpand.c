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

#अन्तर्भाव "vim.h"

स्थिर पूर्णांक	cmd_showtail;	// Only show path tail in lists ?
स्थिर पूर्णांक	may_expand_pattern = FALSE;
स्थिर pos_T	pre_incsearch_pos; // Cursor position when incsearch started

स्थिर शून्य	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, पूर्णांक usefilter, expand_T *xp, पूर्णांक *complp);
स्थिर पूर्णांक	ExpandFromContext(expand_T *xp, char_u *, char_u ***, पूर्णांक *, पूर्णांक);
स्थिर char_u	*showmatches_gettail(char_u *s);
स्थिर पूर्णांक	expand_showtail(expand_T *xp);
स्थिर पूर्णांक	expand_shellcmd(char_u *filepat, char_u ***matches, पूर्णांक *अंक_Matches, पूर्णांक flagsarg);
#जर defined(FEAT_EVAL)
स्थिर पूर्णांक	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, पूर्णांक *अंक_Matches);
स्थिर पूर्णांक	ExpandUserList(expand_T *xp, char_u ***matches, पूर्णांक *अंक_Matches);
स्थिर char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
स्थिर char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, पूर्णांक srclen, पूर्णांक *new_lenp);
#endif
स्थिर पूर्णांक	expand_pattern_in_buf(char_u *pat, पूर्णांक dir, char_u ***matches, पूर्णांक *अंक_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
स्थिर pumitem_T *compl_match_array = रिक्त;
स्थिर पूर्णांक compl_match_arraysize;
// First column in cmdline of the matched item for completion.
स्थिर पूर्णांक compl_startcol;
स्थिर पूर्णांक compl_selected;
// cmdline before expansion
स्थिर string_T cmdline_orig = {रिक्त, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    स्थिर पूर्णांक
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    निवड (xp->xp_context)
    {
    स्थिती EXPAND_BOOL_SETTINGS:
    स्थिती EXPAND_COLORS:
    स्थिती EXPAND_COMPILER:
    स्थिती EXPAND_DIRECTORIES:
    स्थिती EXPAND_DIRS_IN_CDPATH:
    स्थिती EXPAND_FILES:
    स्थिती EXPAND_FILES_IN_PATH:
    स्थिती EXPAND_FILETYPE:
    स्थिती EXPAND_FILETYPECMD:
    स्थिती EXPAND_HELP:
    स्थिती EXPAND_KEYMAP:
    स्थिती EXPAND_OLD_SETTING:
    स्थिती EXPAND_STRING_SETTING:
    स्थिती EXPAND_SETTING_SUBTRACT:
    स्थिती EXPAND_OWNSYNTAX:
    स्थिती EXPAND_PACKADD:
    स्थिती EXPAND_RUNTIME:
    स्थिती EXPAND_SHELLCMD:
    स्थिती EXPAND_SHELLCMDLINE:
    स्थिती EXPAND_TAGS:
    स्थिती EXPAND_TAGS_LISTFILES:
	परत FALSE;

    पूर्वनिर्धारित:
	थांबा;
    }

    परत vim_strchr(p_wop, WOP_FUZZY) != रिक्त;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    पूर्णांक
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    परत vim_strchr(p_wop, WOP_FUZZY) != रिक्त && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    स्थिर पूर्णांक
sort_func_compare(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    जर (*p1 != '<' && *p2 == '<') परत -1;
    जर (*p1 == '<' && *p2 != '<') परत 1;
    परत STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    स्थिर शून्य
wildescape(
    expand_T	*xp,
    char_u	*str,
    पूर्णांक		numfiles,
    char_u	**files)
{
    char_u	*p;
    पूर्णांक		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    जर (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	साठी (पूर्णांक i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    जर (xp->xp_backslash & XP_BS_THREE)
	    {
		अक्षर *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		जर (p != रिक्त)
		{
		    vim_free(files[i]);
		    files[i] = p;
#जर defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    जर (p != रिक्त)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    अन्यथा जर (xp->xp_backslash & XP_BS_COMMA)
	    {
		जर (vim_strchr(files[i], ',') != रिक्त)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    जर (p != रिक्त)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ifdef BACKSLASH_IN_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#अन्यथा
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#endif
	    जर (p != रिक्त)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    जर (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	जर (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    अन्यथा जर (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	साठी (पूर्णांक i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    जर (p != रिक्त)
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
    स्थिर शून्य
ExpandEscape(
    expand_T	*xp,
    char_u	*str,
    पूर्णांक		numfiles,
    char_u	**files,
    पूर्णांक		options)
{
    // May change home directory back to "~"
    जर (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    जर (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    पूर्णांक
nextwild(
    expand_T	*xp,
    पूर्णांक		type,
    पूर्णांक		options,	// extra options for ExpandOne()
    पूर्णांक		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    पूर्णांक		i;
    char_u	*p;
    पूर्णांक		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    पूर्णांक		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    जर (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	जर (ccline->आदान_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	अन्यथा
#endif
	{
	    may_expand_pattern = options & WILD_MAY_EXPAND_PATTERN;
	    set_expand_context(xp);
	    may_expand_pattern = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    जर (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	परत OK;  // Something illegal on command line
    }
    जर (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	परत FAIL;
    }

    i = (पूर्णांक)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    जर (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	परत FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    जर (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	संदेश_puts("...");	    // show that we are busy
	out_flush();
    }

    जर (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, रिक्त, रिक्त, 0, type);
    }
    अन्यथा
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	पूर्णांक	pat_len = xp->xp_pattern_len;
#जर defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	जर (unesc != रिक्त)
	    pat_src = unesc;
#endif

	जर (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	अन्यथा
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#जर defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	जर (tmp == रिक्त)
	    p = रिक्त;
	अन्यथा
	{
	    पूर्णांक use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    जर (escape)
		use_options |= WILD_ESCAPE;
	    जर (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    जर (p != रिक्त && type == WILD_LONGEST)
	    {
		पूर्णांक	j;

		साठी (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    जर (c == '*' || c == '?')
			थांबा;
		}
		जर ((पूर्णांक)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    जर (!wild_navigate && ccline->cmdbuff != रिक्त)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	जर (cmdline_orig.string == रिक्त)
	    cmdline_orig.length = 0;
	अन्यथा
	    cmdline_orig.length = ccline->cmdlen;
    }

    जर (p != रिक्त && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	माप_प्रकार	plen = STRLEN(p);
	पूर्णांक	difflen;
	पूर्णांक	v = OK;

	difflen = (पूर्णांक)plen - xp->xp_pattern_len;
	जर (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	जर (v == OK)
	{
	    mch_memmove(&ccline->cmdbuff[ccline->cmdpos + difflen],
		    &ccline->cmdbuff[ccline->cmdpos],
		    (माप_प्रकार)(ccline->cmdlen - ccline->cmdpos + 1));
	    mch_memmove(&ccline->cmdbuff[i], p, plen);
	    ccline->cmdlen += difflen;
	    ccline->cmdpos += difflen;
	}
    }

    redrawcmd();
    cursorcmd();

    // When expanding a ":map" command and no matches are found, assume that
    // the key is supposed to be inserted literally
    जर (xp->xp_context == EXPAND_MAPPINGS && p == रिक्त)
	परत FAIL;

    जर (xp->xp_numfiles <= 0 && p == रिक्त)
	beep_flush();
    अन्यथा जर (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(शून्य)ExpandOne(xp, रिक्त, रिक्त, 0, WILD_FREE);

    vim_free(p);

    परत OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    स्थिर पूर्णांक
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	पूर्णांक		अंक_Matches,
	पूर्णांक		showtail)
{
    पूर्णांक	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, अंक_Matches);
    जर (compl_match_array == रिक्त)
	परत EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = अंक_Matches;
    साठी (पूर्णांक i = 0; i < अंक_Matches; i++)
    {
	compl_match_array[i].pum_text = (xp->xp_files_abbr != रिक्त
				    && xp->xp_files_abbr[i] != रिक्त)
					? xp->xp_files_abbr[i]
					: SHOW_MATCH(i);
	compl_match_array[i].pum_info = xp->xp_files_info != रिक्त
					    ? xp->xp_files_info[i] : रिक्त;
	compl_match_array[i].pum_extra = xp->xp_files_menu != रिक्त
					    ? xp->xp_files_menu[i] : रिक्त;
	compl_match_array[i].pum_kind = xp->xp_files_kind != रिक्त
					    ? xp->xp_files_kind[i] : रिक्त;
	compl_match_array[i].pum_user_abbr_hlattr = -1;
	compl_match_array[i].pum_user_kind_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == रिक्त ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    prefix_len = vim_strsize(xp->xp_pattern);
    जर (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    परत EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    शून्य
cmdline_pum_display(शून्य)
{
    जर (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    पूर्णांक
cmdline_pum_active(शून्य)
{
    परत pum_visible() && compl_match_array != रिक्त;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    शून्य
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, पूर्णांक defer_redraw)
{
    पूर्णांक	save_किल्ली_Typed = किल्ली_Typed;
#ifdef FEAT_EVAL
    पूर्णांक	save_RedrawingDisabled = RedrawingDisabled;
    जर (cclp->आदान_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    जर (!defer_redraw)
    {
	पूर्णांक save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    अन्यथा
	pum_call_update_screen();
    redrawcmd();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    किल्ली_Typed = save_किल्ली_Typed;
#ifdef FEAT_EVAL
    जर (cclp->आदान_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

    शून्य
cmdline_pum_cleanup(cmdline_info_T *cclp)
{
    cmdline_pum_remove(cclp, FALSE);
    wildmenu_cleanup(cclp);
}

/*
 * Returns the starting column number to use for the cmdline completion popup
 * menu.
 */
    पूर्णांक
cmdline_compl_startcol(शून्य)
{
    परत compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(शून्य)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    परत xp == रिक्त ? रिक्त : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    पूर्णांक
cmdline_compl_is_fuzzy(शून्य)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    परत xp != रिक्त && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    स्थिर पूर्णांक
skip_status_match_char(expand_T *xp, char_u *s)
{
    जर ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	जर (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    परत 2;
#endif
	परत 1;
    }
    परत 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    स्थिर पूर्णांक
status_match_len(expand_T *xp, char_u *s)
{
    पूर्णांक	len = 0;

#ifdef FEAT_MENU
    पूर्णांक emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    जर (emenu && menu_is_separator(s))
	परत 1;
#endif

    पर्यंत (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    परत len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    स्थिर शून्य
win_redr_status_matches(
    expand_T	*xp,
    पूर्णांक		अंक_matches,
    char_u	**matches,	// list of matches
    पूर्णांक		match,
    पूर्णांक		showtail)
{
    पूर्णांक		row;
    char_u	*buf;
    पूर्णांक		len;
    पूर्णांक		clen;		// length in screen cells
    पूर्णांक		fillchar;
    पूर्णांक		attr;
    पूर्णांक		i;
    पूर्णांक		highlight = TRUE;
    char_u	*selstart = रिक्त;
    पूर्णांक		selstart_col = 0;
    char_u	*selend = रिक्त;
    स्थिर पूर्णांक	first_match = 0;
    पूर्णांक		add_डावा = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    पूर्णांक		emenu;
#endif
    पूर्णांक		l;

    जर (matches == रिक्त)	// interrupted completion?
	परत;

    जर (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    अन्यथा
	buf = alloc(topframe->fr_width + 1);
    जर (buf == रिक्त)
	परत;

    जर (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    जर (match == 0)
	first_match = 0;
    अन्यथा जर (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_डावा = TRUE;
    }
    अन्यथा
    {
	// check if match fits on the screen
	साठी (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	जर (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	जर (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    साठी (i = match; i < अंक_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		जर (clen >= topframe->fr_width)
		    थांबा;
	    }
	    जर (i == अंक_matches)
		add_डावा = TRUE;
	}
    }
    जर (add_डावा)
	पर्यंत (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    जर (clen >= topframe->fr_width)
		थांबा;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    जर (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    अन्यथा
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    पर्यंत (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	जर (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	जर (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (पूर्णांक)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	अन्यथा
#endif
	साठी ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    जर (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    अन्यथा
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (पूर्णांक)STRLEN(buf + len);
	    }
	}
	जर (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	जर (++i == अंक_matches)
		थांबा;
    }

    जर (i != अंक_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    जर (row >= 0)
    {
	जर (wild_menu_showing == 0)
	{
	    जर (संदेश_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		जर (cmdline_row == Rows - 1)
		{
		    screen_del_ओळी(0, 0, 1, (पूर्णांक)Rows, TRUE, 0, रिक्त);
		    ++संदेश_scrolled;
		}
		अन्यथा
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    अन्यथा
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		जर (lastwin->w_status_height == 0)
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
	जर (selstart != रिक्त && highlight)
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

#जर defined(FEAT_EVAL)
/*
 * Apply -completeopt=escape to a string about to be inserted into the command
 * line as a completion result.  If "str" is non-NULL and the active expansion
 * context is a customlist/custom user command with UCC_ESCAPE set, free "str"
 * and return a newly-allocated copy with spaces, tabs and backslashes prefixed
 * by a backslash.  Otherwise return "str" unchanged.
 */
    स्थिर char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    जर (str == रिक्त)
	परत रिक्त;
    जर ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	परत str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    जर (p == रिक्त)
	परत str;
    vim_free(str);
    परत p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    स्थिर char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    पूर्णांक		srclen,
    पूर्णांक		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    जर ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	परत रिक्त;

    buf = alloc(srclen + 1);
    जर (buf == रिक्त)
	परत रिक्त;

    d = buf;
    end = src + srclen;
    साठी (p = src; p < end; ++p)
    {
	जर (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (पूर्णांक)(d - buf);
    परत buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    स्थिर char_u *
get_next_or_prev_match(पूर्णांक mode, expand_T *xp)
{
    पूर्णांक	    findex = xp->xp_selected;
    पूर्णांक	    ht;

    // When no matches found, return NULL
    जर (xp->xp_numfiles <= 0)
	परत रिक्त;

    जर (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	जर (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    अन्यथा जर (mode == WILD_NEXT)
    {
	// Select the next entry
	++findex;
    }
    अन्यथा   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	जर (ht > 3)
	    ht -= 2;

	जर (mode == WILD_PAGEUP)
	{
	    जर (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    अन्यथा जर (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    अन्यथा
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	अन्यथा    // mode == WILD_PAGEDOWN
	{
	    जर (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    अन्यथा जर (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    अन्यथा
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    जर (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	जर (xp->xp_orig != रिक्त)
	    findex = -1;
	अन्यथा
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    जर (p_wmnu)
    {
	जर (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	अन्यथा जर (vim_strchr(p_wop, WOP_PUM) != रिक्त)
	{
	    जर (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	अन्यथा
	    win_redr_status_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    जर (findex == -1)
	परत vim_strsave(xp->xp_orig);
#जर defined(FEAT_EVAL)
    परत apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#अन्यथा
    परत vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    स्थिर char_u *
ExpandOne_start(पूर्णांक mode, expand_T *xp, char_u *str, पूर्णांक options)
{
    पूर्णांक		non_suf_match;		// number without matching suffix
    पूर्णांक		i;
    char_u	*ss = रिक्त;

    // Do the expansion.
    जर (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	जर (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    अन्यथा जर (xp->xp_numfiles == 0)
    {
	जर (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    अन्यथा
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	जर (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    जर (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    अन्यथा
		non_suf_match = 1;
	    जर ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		साठी (i = 0; i < 2; ++i)
		    जर (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    जर (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		जर (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		अन्यथा जर (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    जर (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    परत ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    स्थिर char_u *
find_longest_match(expand_T *xp, पूर्णांक options)
{
    long_u	len;
    पूर्णांक		mb_len = 1;
    पूर्णांक		c0, ci;
    पूर्णांक		i;
    char_u	*ss;

    साठी (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	जर (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	अन्यथा
	    c0 = xp->xp_files[0][len];
	साठी (i = 1; i < xp->xp_numfiles; ++i)
	{
	    जर (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    अन्यथा
		ci = xp->xp_files[i][len];
	    जर (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		जर (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    थांबा;
	    }
	    अन्यथा जर (c0 != ci)
		थांबा;
	}
	जर (i < xp->xp_numfiles)
	{
	    जर (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    थांबा;
	}
    }

    ss = alloc(len + 1);
    जर (ss)
	vim_strncpy(ss, xp->xp_files[0], (माप_प्रकार)len);

    परत ss;
}

    शून्य
free_xp_files_extra(expand_T *xp, पूर्णांक numfiles)
{
    जर (xp->xp_files_abbr != रिक्त)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = रिक्त;
    }
    जर (xp->xp_files_kind != रिक्त)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = रिक्त;
    }
    जर (xp->xp_files_menu != रिक्त)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = रिक्त;
    }
    जर (xp->xp_files_info != रिक्त)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = रिक्त;
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
    पूर्णांक		options,
    पूर्णांक		mode)
{
    char_u	*ss = रिक्त;
    पूर्णांक		orig_saved = FALSE;
#जर defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    पूर्णांक		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    जर (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	परत get_next_or_prev_match(mode, xp);

    जर (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    अन्यथा जर (mode == WILD_APPLY)
    {
	जर (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	अन्यथा
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#जर defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    जर (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	जर (compl_match_array != रिक्त)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    जर (mode == WILD_FREE)	// only release file name
	परत रिक्त;

    जर (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#जर defined(FEAT_EVAL)
	जर (ss != रिक्त)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    जर (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#जर defined(FEAT_EVAL)
	जर (ss != रिक्त)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    जर (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	माप_प्रकार	ss_size = 0;
	अक्षर	*prefix = "";
	अक्षर	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	पूर्णांक	n = xp->xp_numfiles - 1;
	पूर्णांक	i;
#जर defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = रिक्त;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	जर ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    जर (escaped != रिक्त)
	    {
		साठी (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    जर (escaped[i] == रिक्त)
		    {
			पर्यंत (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			थांबा;
		    }
		}
		जर (escaped != रिक्त)
		    files = escaped;
	    }
	}
#अन्यथा
	char_u	**files = xp->xp_files;
#endif

	जर (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	अन्यथा जर (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	साठी (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	जर (ss != रिक्त)
	{
	    माप_प्रकार  ss_len = 0;

	    साठी (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (अक्षर *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (अक्षर *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#जर defined(FEAT_EVAL)
	जर (escaped != रिक्त)
	{
	    साठी (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    जर (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    जर (!orig_saved)
	vim_free(orig);

#जर defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    जर (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    परत ss;
}

/*
 * Prepare an expand structure for use.
 */
    शून्य
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
    शून्य
ExpandCleanup(expand_T *xp)
{
    जर (xp->xp_numfiles >= 0)
    {
	free_xp_files_extra(xp, xp->xp_numfiles);
	FreeWild(xp->xp_numfiles, xp->xp_files);
	xp->xp_numfiles = -1;
    }
    VIM_CLEAR(xp->xp_orig);
}

    शून्य
clear_cmdline_orig(शून्य)
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
    स्थिर शून्य
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	पूर्णांक		अंक_Matches,
	पूर्णांक		ओळी,
	पूर्णांक		linenr,
	पूर्णांक		maxlen,
	पूर्णांक		showtail,
	पूर्णांक		dir_attr)
{
    पूर्णांक		i, j;
    पूर्णांक		isdir;
    पूर्णांक		lastlen;
    char_u	*p;

    lastlen = 999;
    साठी (j = linenr; j < अंक_Matches; j += ओळी)
    {
	जर (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    संदेश_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    संदेश_advance(maxlen + 1);
	    संदेश_puts((अक्षर *)p);
	    संदेश_advance(maxlen + 3);
	    संदेश_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    थांबा;
	}
	साठी (i = maxlen - lastlen; --i >= 0; )
	    संदेश_putchar(' ');
	जर (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    जर (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_path;
		char_u	*path;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_path = expand_env_save_opt(matches[j], TRUE, रिक्त);
		path = exp_path != रिक्त ? exp_path : matches[j];
		halved_slash = backslash_halve_save(path);
		isdir = mch_isdir(halved_slash != रिक्त ? halved_slash
			: matches[j]);
		vim_free(exp_path);
		जर (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    अन्यथा
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    जर (showtail)
		p = SHOW_MATCH(j);
	    अन्यथा
	    {
		home_replace(रिक्त, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	अन्यथा
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = संदेश_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    जर (संदेश_col > 0)	// when not wrapped around
    {
	संदेश_clr_eos();
	संदेश_putchar('\n');
    }
    out_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    स्थिर पूर्णांक
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	पूर्णांक		अंक_Matches,
	पूर्णांक		showtail,
	पूर्णांक		noselect)
{
    पूर्णांक	retval = cmdline_pum_create(ccline, xp, matches, अंक_Matches, showtail);

    जर (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    परत retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    पूर्णांक
showmatches(
    expand_T	*xp,
    पूर्णांक		display_wildmenu,
    पूर्णांक		display_list,
    पूर्णांक		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    पूर्णांक		अंक_Matches;
    char_u	**matches;
    पूर्णांक		i;
    पूर्णांक		maxlen;
    पूर्णांक		ओळी;
    पूर्णांक		columns;
    पूर्णांक		attr;
    पूर्णांक		showtail;
    पूर्णांक		noselect = (wim_flags_arg & WIM_NOSELECT);
    पूर्णांक		noinsert = (wim_flags_arg & WIM_NOINSERT);
    पूर्णांक		cmdline_unchanged = noselect || noinsert;

    जर (xp->xp_numfiles == -1)
    {
	पूर्णांक retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&अंक_Matches, &matches);
	जर (retval != EXPAND_OK)
	    परत retval;
	showtail = expand_showtail(xp);
    }
    अन्यथा
    {
	अंक_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    जर (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != रिक्त)
	परत show_pum_matches(ccline, xp, matches, अंक_Matches,
		showtail && !cmdline_unchanged, noselect);

    जर (display_list)
    {
	संदेश_didany = FALSE;		// lines_left will be set
	संदेश_start();			// prepare for paging
	संदेश_putchar('\n');
	out_flush();
	cmdline_row = संदेश_row;
	संदेश_didany = FALSE;		// lines_left will be set again
	संदेश_start();			// prepare for paging
    }

    जर (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    अन्यथा जर (display_wildmenu && !display_list)
	win_redr_status_matches(xp, अंक_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    अन्यथा जर (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	साठी (i = 0; i < अंक_Matches; ++i)
	{
	    पूर्णांक	len;
	    जर (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(रिक्त, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    अन्यथा
		len = vim_strsize(SHOW_MATCH(i));
	    जर (len > maxlen)
		maxlen = len;
	}

	जर (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    ओळी = अंक_Matches;
	अन्यथा
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    जर (columns < 1)
		columns = 1;
	    ओळी = (अंक_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	जर (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    संदेश_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    संदेश_clr_eos();
	    संदेश_advance(maxlen - 3);
	    संदेश_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	साठी (i = 0; i < ओळी; ++i)
	{
	    showmatches_oneline(xp, matches, अंक_Matches, ओळी, i,
						maxlen, showtail, attr);
	    जर (got_int)
	    {
		got_int = FALSE;
		थांबा;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = संदेश_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    जर (display_wildmenu && display_list)
    {
	जर (vim_strchr(p_wop, WOP_PUM) != रिक्त)
	    (शून्य)show_pum_matches(ccline, xp, matches, अंक_Matches,
		    showtail && !cmdline_unchanged, noselect);
	अन्यथा
	    win_redr_status_matches(xp, अंक_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    जर (xp->xp_numfiles == -1)
    {
	FreeWild(अंक_Matches, matches);
	free_xp_files_extra(xp, अंक_Matches);
    }

    परत EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    स्थिर char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    पूर्णांक		had_sep = FALSE;

    साठी (p = s; *p != NUL; )
    {
	जर (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	अन्यथा जर (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    परत t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    स्थिर पूर्णांक
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    जर (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	परत FALSE;

    end = gettail(xp->xp_pattern);
    जर (end == xp->xp_pattern)		// there is no path separator
	परत FALSE;

    साठी (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	जर (rem_backslash(s))
	    ++s;
	अन्यथा जर (vim_strchr((char_u *)"*?[", *s) != रिक्त)
	    परत FALSE;
    }
    परत TRUE;
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
    पूर्णांक		len,
    पूर्णांक		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    पूर्णांक		i, j;
    पूर्णांक		new_len;
    char_u	*tail;
    पूर्णांक		ends_in_star;

    जर (context != EXPAND_FILES
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
	जर (context == EXPAND_FINDFUNC
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
	अन्यथा
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    साठी (i = 0; i < len; i++)
	    {
		जर (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		जर (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		जर ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    जर (retval != रिक्त)
	    {
		retval[0] = '^';
		j = 1;
		साठी (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    जर (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			थांबा;

		    निवड (fname[i])
		    {
			स्थिती '*':   retval[j++] = '.';
				    थांबा;
			स्थिती '~':   retval[j++] = '\\';
				    थांबा;
			स्थिती '?':   retval[j] = '.';
				    सुरूठेवा;
			स्थिती '.':   जर (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    थांबा;
			स्थिती '\\':  जर (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    थांबा;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    अन्यथा
    {
	retval = alloc(len + 4);
	जर (retval != रिक्त)
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
	    साठी (i = len - 2; i >= 0; --i)
	    {
		जर (retval[i] != '\\')
		    थांबा;
		ends_in_star = !ends_in_star;
	    }
#endif
	    जर ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == रिक्त
		    && vim_strchr(retval, '`') == रिक्त)
		retval[len++] = '*';
	    अन्यथा जर (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    परत retval;
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
    शून्य
set_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    जर ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_शोध_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_शोध_EXTRA
	शोध_first_ओळ = 0; // Search entire buffer
#endif
	परत;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    जर (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->आदान_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	परत;
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
    स्थिर char_u *
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, पूर्णांक *complp)
{
    char_u	*p = रिक्त;
    पूर्णांक		len = 0;
    पूर्णांक		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    जर (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    अन्यथा
    {
	p = cmd;
	पर्यंत (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	जर (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    पर्यंत (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	जर (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    पर्यंत (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	जर (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != रिक्त)
	    ++p;
	len = (पूर्णांक)(p - cmd);

	जर (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    परत रिक्त;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	जर ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    पर्यंत (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    जर (*p == NUL && ASCII_ISALNUM(p[-1]))
	परत रिक्त;

    जर (eap->cmdidx == CMD_SIZE)
    {
	जर (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != रिक्त)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	अन्यथा जर (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, रिक्त, xp, complp);
	    जर (p == रिक्त)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    जर (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	परत रिक्त;
    }

    परत p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    स्थिर शून्य
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	पूर्णांक		usefilter,
	expand_T	*xp,
	पूर्णांक		*complp)
{
    char_u	*p;
    पूर्णांक		c;
    पूर्णांक		in_quote = FALSE;
    char_u	*bow = रिक्त;	// Beginning of word
    पूर्णांक		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    पर्यंत (*p != NUL)
    {
	जर (has_mbyte)
	    c = mb_ptr2char(p);
	अन्यथा
	    c = *p;
	जर (c == '\\' && p[1] != NUL)
	    ++p;
	अन्यथा जर (c == '`')
	{
	    जर (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	अन्यथा जर (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != रिक्त && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    पर्यंत (*p != NUL)
	    {
		जर (has_mbyte)
		    c = mb_ptr2char(p);
		अन्यथा
		    c = *p;
		जर (c == '`' || vim_isfilec_or_wc(c))
		    थांबा;
		जर (has_mbyte)
		    len = (*mb_ptr2len)(p);
		अन्यथा
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    जर (in_quote)
		bow = p;
	    अन्यथा
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    जर (bow != रिक्त && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    जर (usefilter
	    || (eap != रिक्त
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	जर (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    जर (*xp->xp_pattern == '$')
    {
	साठी (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    जर (!vim_isIDc(*p))
		थांबा;
	जर (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    जर (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    जर (*xp->xp_pattern == '~')
    {
	साठी (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	जर (*p == NUL && p > xp->xp_pattern + 1
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
    स्थिर char_u *
set_context_in_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    जर (p == रिक्त)
	xp->xp_pattern = arg;
    अन्यथा
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    परत रिक्त;
}

#ifdef FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    स्थिर char_u *
set_context_in_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    जर (p == रिक्त)
	xp->xp_pattern = arg;
    अन्यथा
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    परत रिक्त;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    स्थिर char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    जर (*arg != NUL)
	arg = skip_vimgrep_pat(arg, रिक्त, रिक्त);
    जर (arg == रिक्त || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	परत रिक्त;
    }
    परत skipwhite(arg);
}

#ifdef FEAT_शोध_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    स्थिर char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    जर (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	जर (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    परत find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    स्थिर char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    पूर्णांक		delim;

    delim = *arg;	    // get the delimiter
    जर (delim)
	++arg;		    // skip delimiter if there is one

    पर्यंत (arg[0] != NUL && arg[0] != delim)
    {
	जर (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    जर (arg[0] != NUL)
	परत arg + 1;

    परत रिक्त;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    स्थिर char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    पूर्णांक		delim;

    delim = *arg;
    जर (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	जर (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    पर्यंत (arg[0] != NUL && arg[0] != delim)
	    {
		जर (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    जर (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    पर्यंत (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == रिक्त)
	++arg;
    जर (arg[0] != NUL)
	परत arg;

    परत रिक्त;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    स्थिर char_u *
find_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    जर (*arg != '/')
	परत रिक्त;

    // Match regexp, not just whole words
    साठी (++arg; *arg && *arg != '/'; arg++)
	जर (*arg == '\\' && arg[1] != NUL)
	    arg++;
    जर (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	जर (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == रिक्त)
	    xp->xp_context = EXPAND_NOTHING;
	अन्यथा
	    परत arg;
    }

    परत रिक्त;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    स्थिर char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    पर्यंत ((xp->xp_pattern = vim_strchr(arg, ' ')) != रिक्त)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    जर (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    परत रिक्त;
}
#endif

#जर defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    स्थिर char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    जर (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    अन्यथा
    {
	जर ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_pattern = skipwhite(p);
	}
	अन्यथा
	    xp->xp_context = EXPAND_NOTHING;
    }

    परत रिक्त;
}
#endif

स्थिर गणना
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
स्थिर गणना
{
    EXP_BREAKPT_ADD,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    स्थिर char_u *
set_context_in_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_start;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_pattern = arg;

    जर (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    अन्यथा जर (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    अन्यथा
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    जर (*p == NUL)
	परत रिक्त;
    subcmd_start = p;

    जर (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	जर (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    जर (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		परत रिक्त;
	    }
	    p = skipwhite(p);
	}
	जर (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	अन्यथा
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    अन्यथा जर (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    परत रिक्त;
}

    स्थिर char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = रिक्त;

    p = skipwhite(arg);
    जर (VIM_ISDIGIT(*p))
	परत रिक्त;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    परत रिक्त;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    स्थिर char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    पूर्णांक    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    जर (*p == NUL)
	परत रिक्त;

    साठी (;;)
    {
	जर (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    सुरूठेवा;
	}
	जर (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    सुरूठेवा;
	}
	थांबा;
    }

    जर ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    अन्यथा जर ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    अन्यथा जर ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    परत रिक्त;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    स्थिर शून्य
set_context_with_pattern(expand_T *xp)
{
    पूर्णांक		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_शोध_EXTRA
    पूर्णांक		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    जर (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	परत;
#endif

    xp->xp_pattern = ccline->cmdbuff + skiplen;
    xp->xp_pattern_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_PATTERN_IN_BUF;
    xp->xp_शोध_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    स्थिर char_u *
set_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	दीर्घ		argt,
	पूर्णांक		compl,
	पूर्णांक		forceit)
{
    char_u  *nextcmd;

    निवड (cmdidx)
    {
	स्थिती CMD_find:
	स्थिती CMD_sfind:
	स्थिती CMD_tabfind:
	    जर (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    थांबा;
	स्थिती CMD_cd:
	स्थिती CMD_chdir:
	स्थिती CMD_tcd:
	स्थिती CMD_tchdir:
	स्थिती CMD_lcd:
	स्थिती CMD_lchdir:
	    जर (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    थांबा;
	स्थिती CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    थांबा;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	स्थिती CMD_aboveleft:
	स्थिती CMD_argdo:
	स्थिती CMD_belowright:
	स्थिती CMD_botright:
	स्थिती CMD_browse:
	स्थिती CMD_bufdo:
	स्थिती CMD_cdo:
	स्थिती CMD_cfdo:
	स्थिती CMD_confirm:
	स्थिती CMD_debug:
	स्थिती CMD_folddoclosed:
	स्थिती CMD_folddoopen:
	स्थिती CMD_hide:
	स्थिती CMD_horizontal:
	स्थिती CMD_keepalt:
	स्थिती CMD_keepjumps:
	स्थिती CMD_keepmarks:
	स्थिती CMD_keeppatterns:
	स्थिती CMD_ldo:
	स्थिती CMD_leftabove:
	स्थिती CMD_lfdo:
	स्थिती CMD_lockmarks:
	स्थिती CMD_noautocmd:
	स्थिती CMD_noswapfile:
	स्थिती CMD_rightbelow:
	स्थिती CMD_sandbox:
	स्थिती CMD_silent:
	स्थिती CMD_tab:
	स्थिती CMD_tabdo:
	स्थिती CMD_topleft:
	स्थिती CMD_unsilent:
	स्थिती CMD_verbose:
	स्थिती CMD_vertical:
	स्थिती CMD_windo:
	स्थिती CMD_vim9cmd:
	स्थिती CMD_legacy:
	    परत arg;

	स्थिती CMD_filter:
	    परत set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_शोध_EXTRA
	स्थिती CMD_match:
	    परत set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	स्थिती CMD_command:
	    परत set_context_in_user_cmd(xp, arg);

	स्थिती CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    थांबा;

	स्थिती CMD_global:
	स्थिती CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    जर (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    परत nextcmd;

	स्थिती CMD_and:
	स्थिती CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    जर (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    परत nextcmd;

	स्थिती CMD_isearch:
	स्थिती CMD_dsearch:
	स्थिती CMD_ilist:
	स्थिती CMD_dlist:
	स्थिती CMD_ijump:
	स्थिती CMD_psearch:
	स्थिती CMD_djump:
	स्थिती CMD_isplit:
	स्थिती CMD_dsplit:
	    परत find_cmd_after_isearch_cmd(xp, arg);
	स्थिती CMD_autocmd:
	    परत set_context_in_autocmd(xp, arg, FALSE);
	स्थिती CMD_doautocmd:
	स्थिती CMD_doautoall:
	    परत set_context_in_autocmd(xp, arg, TRUE);
	स्थिती CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    थांबा;
	स्थिती CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    थांबा;
	स्थिती CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    थांबा;
	स्थिती CMD_tag:
	स्थिती CMD_stag:
	स्थिती CMD_ptag:
	स्थिती CMD_ltag:
	स्थिती CMD_tselect:
	स्थिती CMD_stselect:
	स्थिती CMD_ptselect:
	स्थिती CMD_tjump:
	स्थिती CMD_stjump:
	स्थिती CMD_ptjump:
	    जर (vim_strchr(p_wop, WOP_TAGFILE) != रिक्त)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    अन्यथा
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    थांबा;
	स्थिती CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    थांबा;
#ifdef FEAT_SYN_HL
	स्थिती CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    थांबा;
#endif
#ifdef FEAT_EVAL
	स्थिती CMD_final:
	स्थिती CMD_const:
	स्थिती CMD_let:
	स्थिती CMD_var:
	स्थिती CMD_if:
	स्थिती CMD_elseif:
	स्थिती CMD_while:
	स्थिती CMD_for:
	स्थिती CMD_echo:
	स्थिती CMD_echon:
	स्थिती CMD_execute:
	स्थिती CMD_echomsg:
	स्थिती CMD_echoerr:
	स्थिती CMD_echoconsole:
	स्थिती CMD_echowindow:
	स्थिती CMD_call:
	स्थिती CMD_return:
	स्थिती CMD_cexpr:
	स्थिती CMD_caddexpr:
	स्थिती CMD_cgetexpr:
	स्थिती CMD_lexpr:
	स्थिती CMD_laddexpr:
	स्थिती CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    थांबा;

	स्थिती CMD_unlet:
	    परत set_context_in_unlet_cmd(xp, arg);
	स्थिती CMD_function:
	स्थिती CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    थांबा;
	स्थिती CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    थांबा;

	स्थिती CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    थांबा;
#endif
	स्थिती CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    थांबा;
#ifdef FEAT_CSCOPE
	स्थिती CMD_cscope:
	स्थिती CMD_lcscope:
	स्थिती CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    थांबा;
#endif
#ifdef FEAT_SIGNS
	स्थिती CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    थांबा;
#endif
	स्थिती CMD_bdelete:
	स्थिती CMD_bwipeout:
	स्थिती CMD_bunload:
	    पर्यंत ((xp->xp_pattern = vim_strchr(arg, ' ')) != रिक्त)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	स्थिती CMD_buffer:
	स्थिती CMD_sbuffer:
	स्थिती CMD_pbuffer:
	स्थिती CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    थांबा;
#ifdef FEAT_DIFF
	स्थिती CMD_diffget:
	स्थिती CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    थांबा;
#endif
	स्थिती CMD_USER:
	स्थिती CMD_USER_BUF:
	    परत set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	स्थिती CMD_map:	    स्थिती CMD_noremap:
	स्थिती CMD_nmap:	    स्थिती CMD_nnoremap:
	स्थिती CMD_vmap:	    स्थिती CMD_vnoremap:
	स्थिती CMD_omap:	    स्थिती CMD_onoremap:
	स्थिती CMD_imap:	    स्थिती CMD_inoremap:
	स्थिती CMD_cmap:	    स्थिती CMD_cnoremap:
	स्थिती CMD_lmap:	    स्थिती CMD_lnoremap:
	स्थिती CMD_smap:	    स्थिती CMD_snoremap:
	स्थिती CMD_tmap:	    स्थिती CMD_tnoremap:
	स्थिती CMD_xmap:	    स्थिती CMD_xnoremap:
	    परत set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	स्थिती CMD_unmap:
	स्थिती CMD_nunmap:
	स्थिती CMD_vunmap:
	स्थिती CMD_ounmap:
	स्थिती CMD_iunmap:
	स्थिती CMD_cunmap:
	स्थिती CMD_lunmap:
	स्थिती CMD_sunmap:
	स्थिती CMD_tunmap:
	स्थिती CMD_xunmap:
	    परत set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	स्थिती CMD_mapclear:
	स्थिती CMD_nmapclear:
	स्थिती CMD_vmapclear:
	स्थिती CMD_omapclear:
	स्थिती CMD_imapclear:
	स्थिती CMD_cmapclear:
	स्थिती CMD_lmapclear:
	स्थिती CMD_smapclear:
	स्थिती CMD_tmapclear:
	स्थिती CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    थांबा;

	स्थिती CMD_abbreviate:	स्थिती CMD_noreabbrev:
	स्थिती CMD_cabbrev:	स्थिती CMD_cnoreabbrev:
	स्थिती CMD_iabbrev:	स्थिती CMD_inoreabbrev:
	    परत set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	स्थिती CMD_unabbreviate:
	स्थिती CMD_cunabbrev:
	स्थिती CMD_iunabbrev:
	    परत set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	स्थिती CMD_menu:	    स्थिती CMD_noremenu:	    स्थिती CMD_unmenu:
	स्थिती CMD_amenu:	    स्थिती CMD_anoremenu:	    स्थिती CMD_aunmenu:
	स्थिती CMD_nmenu:	    स्थिती CMD_nnoremenu:	    स्थिती CMD_nunmenu:
	स्थिती CMD_vmenu:	    स्थिती CMD_vnoremenu:	    स्थिती CMD_vunmenu:
	स्थिती CMD_omenu:	    स्थिती CMD_onoremenu:	    स्थिती CMD_ounmenu:
	स्थिती CMD_imenu:	    स्थिती CMD_inoremenu:	    स्थिती CMD_iunmenu:
	स्थिती CMD_cmenu:	    स्थिती CMD_cnoremenu:	    स्थिती CMD_cunmenu:
	स्थिती CMD_tlmenu:    स्थिती CMD_tlnoremenu:    स्थिती CMD_tlunmenu:
	स्थिती CMD_tmenu:				    स्थिती CMD_tunmenu:
	स्थिती CMD_popup:	    स्थिती CMD_tearoff:	    स्थिती CMD_emenu:
	    परत set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	स्थिती CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    थांबा;

	स्थिती CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    थांबा;

	स्थिती CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    थांबा;

	स्थिती CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    थांबा;

	स्थिती CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    थांबा;

	स्थिती CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    थांबा;

#जर defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	स्थिती CMD_language:
	    परत set_context_in_lang_cmd(xp, arg);
#endif
#जर defined(FEAT_PROFILE)
	स्थिती CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    थांबा;
#endif
	स्थिती CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    थांबा;

	स्थिती CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    थांबा;

	स्थिती CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    थांबा;

	स्थिती CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    थांबा;
#जर defined(FEAT_PROFILE)
	स्थिती CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    थांबा;
#endif

	स्थिती CMD_argdelete:
	    पर्यंत ((xp->xp_pattern = vim_strchr(arg, ' ')) != रिक्त)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    थांबा;

#ifdef FEAT_EVAL
	स्थिती CMD_breakadd:
	स्थिती CMD_profdel:
	स्थिती CMD_breakdel:
	    परत set_context_in_breakadd_cmd(xp, arg, cmdidx);

	स्थिती CMD_scriptnames:
	    परत set_context_in_scriptnames_cmd(xp, arg);
#endif
	स्थिती CMD_filetype:
	    परत set_context_in_filetype_cmd(xp, arg);

	पूर्वनिर्धारित:
	    थांबा;
    }
    परत रिक्त;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    स्थिर char_u *
set_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    पूर्णांक			len = 0;
    exarg_T		ea;
    पूर्णांक			compl = EXPAND_NOTHING;
    पूर्णांक			forceit = FALSE;
    पूर्णांक			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_ओळ = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    साठी (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != रिक्त; cmd++)
	;
    xp->xp_pattern = cmd;

    जर (*cmd == NUL)
	परत रिक्त;
    जर (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	परत रिक्त;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    जर (*cmd == NUL)
	परत रिक्त;
    जर (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	परत रिक्त;
    }

    जर (*cmd == '|' || *cmd == '\n')
	परत cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    जर (p == रिक्त)
	परत रिक्त;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    जर (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    जर (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    जर ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	पर्यंत (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    पर्यंत (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    जर (*p == NUL)
	    {
		जर (ea.argt & EX_ARGOPT)
		    परत set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		जर (ea.cmdidx == CMD_terminal)
		    परत set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    जर (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	जर (*arg == '>')			// append
	{
	    जर (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	अन्यथा जर (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    जर (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	जर (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    जर (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	पर्यंत (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    जर ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	जर (*arg == NUL)
	    परत p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    जर ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	जर (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	पर्यंत (*p)
	{
	    जर (*p == Ctrl_V)
	    {
		जर (p[1] != NUL)
		    ++p;
	    }
	    अन्यथा जर ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		जर (*(p - 1) != '\\')
		{
		    जर (*p == '|' || *p == '\n')
			परत p + 1;
		    परत रिक्त;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    जर (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == रिक्त)
	// no arguments allowed but there is something
	परत रिक्त;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (पूर्णांक)STRLEN(buff);
    पर्यंत (*p && p < buff + len)
    {
	जर (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	अन्यथा
	{
	    जर (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    जर (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    परत set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    शून्य
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    पूर्णांक		len,	    // length of command line (excl. NUL)
    पूर्णांक		col,	    // position of cursor
    पूर्णांक		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    पूर्णांक			context;
#endif
    पूर्णांक		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    जर (col < len)
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
    जर (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    अन्यथा जर (use_ccline && ccline->आदान_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	जर (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(रिक्त, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    अन्यथा
#endif
	पर्यंत (nextcomm != रिक्त)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_ओळ = str;
    xp->xp_col = col;

    जर (col < len)
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
    पूर्णांक
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    पूर्णांक		col,		// position of cursor
    पूर्णांक		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = रिक्त;
    पूर्णांक		options = WILD_ADD_SLASH|WILD_SILENT;

    जर (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	परत EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    जर (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	परत EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (पूर्णांक)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	पूर्णांक	pat_len = xp->xp_pattern_len;
#जर defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	जर (unesc != रिक्त)
	    pat_src = unesc;
#endif

	जर (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	अन्यथा
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#जर defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    जर (file_str == रिक्त)
	परत EXPAND_UNSUCCESSFUL;

    जर (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    जर (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = रिक्त;
    }
    vim_free(file_str);

    परत EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    स्थिर पूर्णांक
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	पूर्णांक		*अंक_Matches,
	पूर्णांक		flags,
	पूर्णांक		options)
{
    पूर्णांक		free_pat = FALSE;
    पूर्णांक		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    जर (xp->xp_backslash != XP_BS_NONE)
    {
	माप_प्रकार	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	जर (pat == रिक्त)
	    परत ret;

	pat_end = pat + pat_len;
	साठी (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    जर (*p != '\\')
		सुरूठेवा;

	    जर (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (माप_प्रकार)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    अन्यथा जर (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (माप_प्रकार)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    अन्यथा जर (xp->xp_backslash & XP_BS_COMMA)
	    {
		जर (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(माप_प्रकार)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		अन्यथा जर (*(p + 1) == ',')
		{
		    from = p + 1;
		    mch_memmove(p, from,
			(माप_प्रकार)(pat_end - from) + 1);   // +1 for NUL
		    --pat_end;
		}
#endif
	    }
	}
    }

    जर (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, अंक_Matches);
#endif
    }
    अन्यथा
    {
	जर (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	अन्यथा जर (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	अन्यथा जर (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	अन्यथा
	    flags = (flags | EW_DIR) & ~EW_FILE;
	जर (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, अंक_Matches, matches, flags);
    }
    जर (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    जर (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	पूर्णांक j;

	साठी (j = 0; j < *अंक_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    पर्यंत (*ptr != NUL)
	    {
		जर (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		अन्यथा जर (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    परत ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    स्थिर char_u *
get_behave_arg(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx == 0)
	परत (char_u *)"mswin";
    जर (idx == 1)
	परत (char_u *)"xterm";
    परत रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    स्थिर char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx < 0)
	परत रिक्त;

    जर (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	अक्षर	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	परत (char_u *)opts_all[idx];
    }
    जर (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	अक्षर	*opts_plugin[] = {"plugin", "on", "off"};
	परत (char_u *)opts_plugin[idx];
    }
    जर (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	अक्षर	*opts_indent[] = {"indent", "on", "off"};
	परत (char_u *)opts_indent[idx];
    }
    जर (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	अक्षर	*opts_onoff[] = {"on", "off"};
	परत (char_u *)opts_onoff[idx];
    }
    परत रिक्त;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    स्थिर char_u *
get_breakadd_arg(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx >= 0 && idx <= 3)
    {
	अक्षर	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	जर (breakpt_expand_what == EXP_BREAKPT_ADD)
	    परत (char_u *)opts[idx];
	अन्यथा जर (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    जर (idx <= 2)
		परत (char_u *)opts[idx + 1];
	}
	अन्यथा
	{
	    // profdel {func, file}
	    जर (idx <= 1)
		परत (char_u *)opts[idx + 1];
	}
    }
    परत रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    स्थिर char_u *
get_scriptnames_arg(expand_T *xp UNUSED, पूर्णांक idx)
{
    scriptitem_T *si;

    जर (!SCRIPT_ID_VALID(idx + 1))
	परत रिक्त;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(रिक्त, si->sn_name, NameBuff, MAXPATHL, TRUE);
    परत NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    स्थिर char_u *
get_retab_arg(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx == 0)
	परत (char_u *)"-indentonly";
    परत रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    स्थिर char_u *
get_messages_arg(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx == 0)
	परत (char_u *)"clear";
    परत रिक्त;
}

    स्थिर char_u *
get_mapclear_arg(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx == 0)
	परत (char_u *)"<buffer>";
    परत रिक्त;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    स्थिर पूर्णांक
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	पूर्णांक		*अंक_Matches)
{
    स्थिर संरचना expgen
    {
	पूर्णांक		context;
	char_u	*((*func)(expand_T *, पूर्णांक));
	पूर्णांक		ic;
	पूर्णांक		escaped;
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
	{EXPAND_EXPRESSION, get_अभिव्यक्ती_name, FALSE, TRUE},
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
#जर defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
    पूर्णांक	i;
    पूर्णांक ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(tab); ++i)
    {
	जर (xp->xp_context == tab[i].context)
	{
	    जर (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, अंक_Matches,
						tab[i].func, tab[i].escaped);
	    थांबा;
	}
    }

    परत ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    स्थिर पूर्णांक
map_wildopts_to_ewflags(पूर्णांक options)
{
    पूर्णांक		flags;

    flags = EW_DIR;	// include directories
    जर (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    जर (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    जर (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    जर (options & WILD_SILENT)
	flags |= EW_SILENT;
    जर (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    जर (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    परत flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    स्थिर पूर्णांक
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    पूर्णांक		*अंक_Matches,
    पूर्णांक		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    पूर्णांक		ret;
    पूर्णांक		flags;
    char_u	*tofree = रिक्त;
    पूर्णांक		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    जर (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	परत expand_files_and_dirs(xp, pat, matches, अंक_Matches, flags,
								options);

    *matches = (char_u **)"";
    *अंक_Matches = 0;
    जर (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	जर (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					अंक_Matches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*अंक_Matches, *matches);
#endif
	    परत OK;
	}
	परत FAIL;
    }

    जर (xp->xp_context == EXPAND_SHELLCMD)
	परत expand_shellcmd(pat, matches, अंक_Matches, flags);
    जर (xp->xp_context == EXPAND_OLD_SETTING)
	परत ExpandOldSetting(अंक_Matches, matches);
    जर (xp->xp_context == EXPAND_BUFFERS)
	परत ExpandBufnames(pat, अंक_Matches, matches, options);
#ifdef FEAT_DIFF
    जर (xp->xp_context == EXPAND_DIFF_BUFFERS)
	परत ExpandBufnames(pat, अंक_Matches, matches,
						options | BUF_DIFF_FILTER);
#endif
    जर (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	परत expand_tags(xp->xp_context == EXPAND_TAGS, pat, अंक_Matches,
								matches);
    जर (xp->xp_context == EXPAND_COLORS)
    {
	अक्षर *directories[] = {"colors", रिक्त};
	परत ExpandRTDir(pat, DIP_START + DIP_OPT, अंक_Matches, matches,
								directories);
    }
    जर (xp->xp_context == EXPAND_COMPILER)
    {
	अक्षर *directories[] = {"compiler", रिक्त};
	परत ExpandRTDir(pat, 0, अंक_Matches, matches, directories);
    }
    जर (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	अक्षर *directories[] = {"syntax", रिक्त};
	परत ExpandRTDir(pat, 0, अंक_Matches, matches, directories);
    }
    जर (xp->xp_context == EXPAND_FILETYPE)
    {
	अक्षर *directories[] = {"syntax", "indent", "ftplugin", रिक्त};
	परत ExpandRTDir(pat, 0, अंक_Matches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    जर (xp->xp_context == EXPAND_KEYMAP)
    {
	अक्षर *directories[] = {"keymap", रिक्त};
	परत ExpandRTDir(pat, 0, अंक_Matches, matches, directories);
    }
#endif
#जर defined(FEAT_EVAL)
    जर (xp->xp_context == EXPAND_USER_LIST)
	परत ExpandUserList(xp, matches, अंक_Matches);
#endif
    जर (xp->xp_context == EXPAND_PACKADD)
	परत ExpandPackAddDir(pat, अंक_Matches, matches);
    जर (xp->xp_context == EXPAND_RUNTIME)
	परत expand_runtime_cmd(pat, अंक_Matches, matches);
    जर (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	परत expand_pattern_in_buf(pat, xp->xp_शोध_dir,
		matches, अंक_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    जर ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	पूर्णांक len = (पूर्णांक)STRLEN(pat) + 20;

	tofree = alloc(len);
	जर (tofree == रिक्त)
	    परत FAIL;
	vim_snprintf((अक्षर *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    जर (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	जर (regmatch.regprog == रिक्त)
	{
	    vim_free(tofree);
	    परत FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    जर (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, अंक_Matches, matches, fuzzy);
    अन्यथा जर (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, अंक_Matches, matches);
    अन्यथा जर (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, अंक_Matches, matches);
    अन्यथा जर (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, अंक_Matches, matches);
    अन्यथा जर (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, अंक_Matches);
    अन्यथा जर (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, अंक_Matches);
#जर defined(FEAT_TERMINAL)
    अन्यथा जर (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, अंक_Matches);
#endif
#जर defined(FEAT_EVAL)
    अन्यथा जर (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, अंक_Matches);
#endif
    अन्यथा
	ret = ExpandOther(pat, xp, &regmatch, matches, अंक_Matches);

    जर (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    परत ret;
}

    पूर्णांक
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    पूर्णांक		*अंक_Matches,
    char_u	*(*func)(expand_T *, पूर्णांक),
					  // returns a string from the list
    पूर्णांक		escaped)
{
    परत ExpandGenericExt(
	pat, xp, regmatch, matches, अंक_Matches, func, escaped, 0);
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
    पूर्णांक
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    पूर्णांक		*अंक_Matches,
    char_u	*(*func)(expand_T *, पूर्णांक),
					  // returns a string from the list
    पूर्णांक		escaped,
    पूर्णांक		sortStartIdx)
{
    पूर्णांक		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = रिक्त;
    पूर्णांक		score = 0;
    पूर्णांक		fuzzy;
    पूर्णांक		match;
    पूर्णांक		sort_matches = FALSE;
    पूर्णांक		funcsort = FALSE;
    पूर्णांक		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = रिक्त;
    *अंक_Matches = 0;

    जर (!fuzzy)
	ga_init2(&ga, आकार(अक्षर *), 30);
    अन्यथा
	ga_init2(&ga, आकार(fuzmatch_str_T), 30);

    साठी (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	जर (str == रिक्त)	    // end of list
	    थांबा;
	जर (*str == NUL)	    // skip empty strings
	    सुरूठेवा;

	जर (xp->xp_pattern[0] != NUL)
	{
	    जर (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    अन्यथा
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	अन्यथा
	    match = TRUE;

	जर (!match)
	    सुरूठेवा;

	जर (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	अन्यथा
	    str = vim_strsave(str);
	जर (str == रिक्त)
	{
	    जर (!fuzzy)
	    {
		ga_clear_strings(&ga);
		परत FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    परत FAIL;
	}

	जर (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    थांबा;
	}

	जर (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	अन्यथा
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

#ifdef FEAT_MENU
	जर (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    जर (*str == '\001')
		*str = '.';
	}
#endif

	जर (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    जर (ga.ga_len == 0)
	परत OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    जर (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    जर (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    जर (sort_matches && sortStartMatchIdx != -1)
    {
	जर (funcsort)
	    // <SNR> functions should be sorted to the end.
	    क्विक((शून्य *)ga.ga_data, (माप_प्रकार)ga.ga_len, आकार(char_u *),
							   sort_func_compare);
	अन्यथा
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    जर (!fuzzy)
    {
	*matches = ga.ga_data;
	*अंक_Matches = ga.ga_len;
    }
    अन्यथा
    {
	जर (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    परत FAIL;
	*अंक_Matches = ga.ga_len;
    }

#जर defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    परत OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    स्थिर शून्य
expand_shellcmd_onedir(
	char_u		*pathed_pattern,    // fully pathed pattern
	माप_प्रकार		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	पूर्णांक		*अंक_Matches,
	पूर्णांक		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    जर (expand_wildcards(1, &pathed_pattern, अंक_Matches, matches, flags) != OK)
	परत;

    जर (ga_grow(gap, *अंक_Matches) == FAIL)
    {
	FreeWild(*अंक_Matches, *matches);
	परत;
    }

    साठी (पूर्णांक i = 0; i < *अंक_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	माप_प्रकार	namelen = STRLEN(name);

	जर (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    जर (HASHITEM_EMPTY(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (माप_प्रकार)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_data)[gap->ga_len++] = name;
		hash_add_item(ht, hi, name, hash);
		name = रिक्त;
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
    स्थिर पूर्णांक
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    पूर्णांक		*अंक_Matches,	// return: number of matches
    पूर्णांक		flagsarg)	// EW_ flags
{
    char_u	*pat;
    माप_प्रकार	patlen;
    char_u	*path = रिक्त;
    पूर्णांक		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    पूर्णांक		flags = flagsarg;
    पूर्णांक		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    जर (buf == रिक्त)
	परत FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    जर (pat == रिक्त)
    {
	vim_free(buf);
	परत FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    साठी (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	जर (*s != '\\')
	    सुरूठेवा;

	p = s + 1;
	जर (*p == ' ')
	{
	    mch_memmove(s, p, (माप_प्रकार)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (माप_प्रकार)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    जर (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    अन्यथा
    {
	// For an absolute name we don't use $PATH.
	जर (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	जर (path == रिक्त)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, आकार(अक्षर *), 10);
    hash_init(&found_ht);
    साठी (s = path; ; s = e)
    {
	माप_प्रकार	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	माप_प्रकार	seplen;

	जर (*s == NUL)
	{
	    जर (did_curdir)
		थांबा;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	अन्यथा
	{
#जर defined(MSWIN)
	    e = vim_strchr(s, ';');
#अन्यथा
	    e = vim_strchr(s, ':');
#endif
	    जर (e == रिक्त)
		e = s + STRLEN(s);

	    pathlen = (माप_प्रकार)(e - s);
	    जर (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    अन्यथा
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? आकार(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	जर (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    जर (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		जर (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, अंक_Matches, flags,
				    &found_ht, &ga);
	}

	जर (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_data;
    *अंक_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    जर (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    परत OK;
}

#जर defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    स्थिर शून्य *
call_user_expand_func(
    शून्य	*(*user_expand_func)(char_u *, पूर्णांक, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    पूर्णांक		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = रिक्त;
    शून्य	*ret;

    जर (xp->xp_arg == रिक्त || xp->xp_arg[0] == '\0' || xp->xp_ओळ == रिक्त)
	परत रिक्त;

    जर (ccline->cmdbuff != रिक्त)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	पूर्णांक unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	जर (pat == रिक्त)
	    pat = vim_strnsave(xp->xp_pattern, xp->xp_pattern_len);
    }

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = xp->xp_ओळ;
    args[2].v_type = VAR_संख्या;
    args[2].vval.v_संख्या = xp->xp_col;
    args[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, args);

    current_sctx = save_current_sctx;
    जर (ccline->cmdbuff != रिक्त)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    परत ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    स्थिर पूर्णांक
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    पूर्णांक		*अंक_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    पूर्णांक		keep;
    garray_T	ga;
    पूर्णांक		fuzzy;
    पूर्णांक		match;
    पूर्णांक		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = रिक्त;
    *अंक_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    जर (retstr == रिक्त)
	परत FAIL;

    जर (!fuzzy)
	ga_init2(&ga, आकार(अक्षर *), 3);
    अन्यथा
	ga_init2(&ga, आकार(fuzmatch_str_T), 3);

    साठी (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	जर (e == रिक्त)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	जर (xp->xp_pattern[0] != NUL)
	{
	    जर (!fuzzy)
		match = vim_regexec(regmatch, s, (colnr_T)0);
	    अन्यथा
	    {
		score = fuzzy_match_str(s, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	अन्यथा
	    match = TRUE;		// match everything

	*e = keep;

	जर (match)
	{
	    char_u  *p = vim_strnsave(s, (माप_प्रकार)(e - s));
	    जर (p == रिक्त)
		थांबा;

	    जर (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		थांबा;
	    }

	    जर (!fuzzy)
		((char_u **)ga.ga_data)[ga.ga_len] = p;
	    अन्यथा
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	जर (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    जर (ga.ga_len == 0)
	परत OK;

    जर (!fuzzy)
    {
	*matches = ga.ga_data;
	*अंक_Matches = ga.ga_len;
    }
    अन्यथा
    {
	जर (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    परत FAIL;
	*अंक_Matches = ga.ga_len;
    }
    परत OK;
}

    शून्य
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    पूर्णांक		*अंक_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    पूर्णांक		have_extra = FALSE;
    पूर्णांक		i;

    ga_init2(&ga, आकार(अक्षर *), 3);
    ga_init2(&ga_abbr, आकार(अक्षर *), 3);
    ga_init2(&ga_kind, आकार(अक्षर *), 3);
    ga_init2(&ga_menu, आकार(अक्षर *), 3);
    ga_init2(&ga_info, आकार(अक्षर *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_वस्तुसंच(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = रिक्त;
	char_u	*abbr = रिक्त;
	char_u	*kind = रिक्त;
	char_u	*menu = रिक्त;
	char_u	*info = रिक्त;

	जर (tv->v_type == VAR_STRING)
	{
	    जर (tv->vval.v_string == रिक्त)
		सुरूठेवा;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	अन्यथा जर (tv->v_type == VAR_DICT && tv->vval.v_dict != रिक्त)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    जर (word == रिक्त)
		सुरूठेवा;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    जर (abbr != रिक्त || kind != रिक्त || menu != रिक्त || info != रिक्त)
		have_extra = TRUE;
	}
	अन्यथा
	    सुरूठेवा;  // Skip other types

	जर (p == रिक्त
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
	    थांबा;
	}

	((char_u **)ga.ga_data)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_data)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_data)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_data)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_data)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_data;
    *अंक_Matches = ga.ga_len;
    जर (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    अन्यथा
    {
	// No extra info collected; free the placeholder NULL entries.
	साठी (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	साठी (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	साठी (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	साठी (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    स्थिर पूर्णांक
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    पूर्णांक		*अंक_Matches)
{
    list_T      *retlist;

    *matches = रिक्त;
    *अंक_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    जर (retlist == रिक्त)
	परत FAIL;

    expand_process_user_list(retlist, matches, अंक_Matches, xp);
    list_unref(retlist);
    परत OK;
}
#endif

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    शून्य
globpath(
    char_u	*path,
    char_u	*file,
    garray_T	*ga,
    पूर्णांक		expand_options,
    पूर्णांक		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    माप_प्रकार	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    माप_प्रकार	seplen;
    माप_प्रकार	filelen;

    buf = alloc(MAXPATHL);
    जर (buf == रिक्त)
	परत;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#जर defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#अन्यथा
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    पर्यंत (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (माप_प्रकार)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	जर (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    पूर्णांक	    अंक_p;
	    char_u  **p;

	    जर (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    जर (ExpandFromContext(&xpc, buf, &p, &अंक_p,
			     WILD_SILENT|expand_options) != FAIL && अंक_p > 0)
	    {
		ExpandEscape(&xpc, buf, अंक_p, p, WILD_SILENT|expand_options);

		जर (ga_grow(ga, अंक_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    साठी (पूर्णांक i = 0; i < अंक_p; ++i)
		    {
			((char_u **)ga->ga_data)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		अन्यथा
		{
		    FreeWild(अंक_p, p);
		    p = रिक्त;
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
    पूर्णांक
wildmenu_translate_किल्ली(
	cmdline_info_T	*cclp,
	पूर्णांक		किल्ली,
	expand_T	*xp,
	पूर्णांक		did_wild_list)
{
    पूर्णांक c = किल्ली;

    जर (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	निवड (c)
	{
	    स्थिती K_UP:	  c = K_डावा; थांबा;
	    स्थिती K_DOWN:  c = K_उजवा; थांबा;
	    स्थिती K_डावा:  c = K_UP; थांबा;
	    स्थिती K_उजवा: c = K_DOWN; थांबा;
	    पूर्वनिर्धारित:	  थांबा;
	}
    }

    जर (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	जर (c == K_डावा)
	    c = Ctrl_P;
	अन्यथा जर (c == K_उजवा)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    जर (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    परत c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    स्थिर शून्य
cmdline_del(cmdline_info_T *cclp, पूर्णांक from)
{
    mch_memmove(cclp->cmdbuff + from, cclp->cmdbuff + cclp->cmdpos,
	    (माप_प्रकार)(cclp->cmdlen - cclp->cmdpos + 1));
    cclp->cmdlen -= cclp->cmdpos - from;
    cclp->cmdpos = from;
}

/*
 * Handle a key pressed when the wild menu for the menu names
 * (EXPAND_MENUNAMES) is displayed.
 */
    स्थिर पूर्णांक
wildmenu_process_किल्ली_menunames(cmdline_info_T *cclp, पूर्णांक किल्ली, expand_T *xp)
{
    पूर्णांक		i;
    पूर्णांक		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    जर (किल्ली == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	किल्ली = p_wc;
	किल्ली_Typed = TRUE;  // in case the key was mapped
    }
    अन्यथा जर (किल्ली == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	पूर्णांक found = FALSE;

	j = (पूर्णांक)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	पर्यंत (--j > 0)
	{
	    // check for start of menu name
	    जर (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		थांबा;
	    }
	    // check for start of submenu name
	    जर (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		जर (found)
		{
		    i = j + 1;
		    थांबा;
		}
		अन्यथा
		    found = TRUE;
	    }
	}
	जर (i > 0)
	    cmdline_del(cclp, i);
	किल्ली = p_wc;
	किल्ली_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    परत किल्ली;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    स्थिर पूर्णांक
wildmenu_process_किल्ली_filenames(cmdline_info_T *cclp, पूर्णांक किल्ली, expand_T *xp)
{
    पूर्णांक		i;
    पूर्णांक		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    जर (किल्ली == K_DOWN
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	किल्ली = p_wc;
	किल्ली_Typed = TRUE;  // in case the key was mapped
    }
    अन्यथा जर (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && किल्ली == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	पूर्णांक found = FALSE;

	j = cclp->cmdpos;
	i = (पूर्णांक)(xp->xp_pattern - cclp->cmdbuff);
	पर्यंत (--j > i)
	{
	    जर (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    जर (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		थांबा;
	    }
	}
	जर (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    किल्ली = p_wc;
	    किल्ली_Typed = TRUE;  // in case the key was mapped
	}
    }
    अन्यथा जर (किल्ली == K_UP)
    {
	// go up a directory
	पूर्णांक found = FALSE;

	j = cclp->cmdpos - 1;
	i = (पूर्णांक)(xp->xp_pattern - cclp->cmdbuff);
	पर्यंत (--j > i)
	{
	    जर (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    जर (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == रिक्त
#endif
	       )
	    {
		जर (found)
		{
		    i = j + 1;
		    थांबा;
		}
		अन्यथा
		    found = TRUE;
	    }
	}

	जर (!found)
	    j = i;
	अन्यथा जर (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	अन्यथा जर (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	अन्यथा
	    j = 0;
	जर (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	अन्यथा जर (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	किल्ली = p_wc;
	किल्ली_Typed = TRUE;
    }

    परत किल्ली;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    पूर्णांक
wildmenu_process_किल्ली(cmdline_info_T *cclp, पूर्णांक किल्ली, expand_T *xp)
{
    जर (xp->xp_context == EXPAND_MENUNAMES)
	परत wildmenu_process_किल्ली_menunames(cclp, किल्ली, xp);
    अन्यथा जर ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	परत wildmenu_process_किल्ली_filenames(cclp, किल्ली, xp);

    परत किल्ली;
}

/*
 * Free expanded names when finished walking through the matches
 */
    शून्य
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    पूर्णांक skt = किल्ली_Typed;

    जर (!p_wmnu || wild_menu_showing == 0)
	परत;

#ifdef FEAT_EVAL
    पूर्णांक save_RedrawingDisabled = RedrawingDisabled;
    जर (cclp->आदान_fn)
	RedrawingDisabled = 0;
#endif

#जर defined(FEAT_शोध_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    जर (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    अन्यथा जर (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	last_status(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    अन्यथा
    {
	win_redraw_last_status(topframe);
	redraw_statuslines();
    }
    किल्ली_Typed = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    जर (cclp->आदान_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#जर defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    शून्य
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    पूर्णांक		filtered = FALSE;
    पूर्णांक		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	परत;

    pat = tv_get_string(&argvars[0]);
    जर (check_for_string_arg(argvars, 1) == FAIL)
	परत;
    type = tv_get_string(&argvars[1]);

    जर (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], रिक्त);

    जर (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    जर (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    जर (STRCMP(type, "cmdline") == 0)
    {
	पूर्णांक cmdline_len = (पूर्णांक)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (पूर्णांक)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    अन्यथा
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (पूर्णांक)STRLEN(xpc.xp_pattern);
	xpc.xp_ओळ = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	निवड (xpc.xp_context)
	{
	स्थिती EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    परत;

	स्थिती EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    जर (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		परत;
	    }

	    xpc.xp_arg = type + 7;
	    थांबा;

	स्थिती EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    जर (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		परत;
	    }

	    xpc.xp_arg = type + 11;
	    थांबा;

# जर defined(FEAT_MENU)
	स्थिती EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (पूर्णांक)(xpc.xp_pattern - pattern_start);
	    थांबा;
# endif

# ifdef FEAT_CSCOPE
	स्थिती EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (पूर्णांक)(xpc.xp_pattern - pattern_start);
	    थांबा;
# endif

# ifdef FEAT_SIGNS
	स्थिती EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (पूर्णांक)(xpc.xp_pattern - pattern_start);
	    थांबा;
# endif

	स्थिती EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (पूर्णांक)(xpc.xp_pattern - pattern_start);
	    थांबा;

	स्थिती EXPAND_SHELLCMDLINE:
	{
	    पूर्णांक context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(रिक्त, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (पूर्णांक)(xpc.xp_pattern - pattern_start);
	    थांबा;
	}

	स्थिती EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    थांबा;

	स्थिती EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    थांबा;

	पूर्वनिर्धारित:
	    थांबा;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	पूर्णांक	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	जर (unesc != रिक्त)
	    pat_src = unesc;

	जर (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	अन्यथा
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    जर (rettv_list_alloc(rettv) == OK && pat != रिक्त)
    {
	पूर्णांक	i;

	ExpandOne(&xpc, pat, रिक्त, options, WILD_ALL_KEEP);

	साठी (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    शून्य
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    पूर्णांक		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जर (check_for_string_arg(argvars, 0) == FAIL)
	परत;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (पूर्णांक)STRLEN(pat);
    set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_to_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    शून्य
f_cmdcomplete_info(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    पूर्णांक		    idx;
    पूर्णांक		    ret = OK;

    जर (rettv_dict_alloc(rettv) == FAIL || ccline == रिक्त
	    || ccline->xpc == रिक्त || ccline->xpc->xp_files == रिक्त)
	परत;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (पूर्णांक)cmdline_orig.length);
    जर (ret == OK)
	ret = dict_add_संख्या(retdict, "pum_visible", pum_visible());
    जर (ret == OK)
	ret = dict_add_संख्या(retdict, "selected", ccline->xpc->xp_selected);
    जर (ret == OK)
    {
	li = list_alloc();
	जर (li == रिक्त)
	    परत;
	ret = dict_add_list(retdict, "matches", li);
	जर (ret == FAIL)
	    list_unref(li);
	साठी (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    स्थिर पूर्णांक
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*ओळ, *start_ओळ, *end_ओळ;
    पूर्णांक		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    पूर्णांक		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != रिक्त;

    जर (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	परत FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_ओळ = ml_get(start->lnum);
    char_u  *start_ptr = start_ओळ + start->col;
    पूर्णांक	    is_single_ओळ = start->lnum == end->lnum;

    segment_len = is_single_ओळ ? (end->col - start->col)
			: (पूर्णांक)(ml_get_len(start->lnum) - start->col);
    जर (ga_grow(&ga, segment_len + 2) != OK)
	जा fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    जर (!is_single_ओळ)
    {
	जर (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	अन्यथा
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    जर (!is_single_ओळ)
    {
	साठी (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    पूर्णांक  linelen;

	    ओळ = ml_get(lnum);
	    linelen = (पूर्णांक)ml_get_len(lnum);
	    जर (ga_grow(&ga, linelen + 2) != OK)
		जा fail;
	    ga_concat_len(&ga, ओळ, linelen);
	    जर (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    अन्यथा
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_ओळ = ml_get(end->lnum);
    word_end = find_word_end(end_ओळ + end->col);
    segment_len = (पूर्णांक)(word_end - end_ओळ);
    जर (ga_grow(&ga, segment_len) != OK)
	जा fail;
    ga_concat_len(&ga, end_ओळ + (is_single_ओळ ? end->col : 0),
	    segment_len - (is_single_ओळ ? end->col : 0));

    // Null-terminate
    जर (ga_grow(&ga, 1) != OK)
	जा fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    परत OK;

fail:
    ga_clear(&ga);
    परत FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    स्थिर पूर्णांक
is_regex_match(char_u *pat, char_u *str)
{
    जर (STRCMP(pat, str) == 0)
	परत TRUE;

    regmatch_T	regmatch;
    पूर्णांक		result;

    ++emsg_off;
    ++संदेश_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --संदेश_silent;

    जर (regmatch.regprog == रिक्त)
	परत FALSE;
    regmatch.rm_ic = p_ic;
    जर (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++संदेश_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --संदेश_silent;

    vim_regfree(regmatch.regprog);
    परत result;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    स्थिर char_u *
concat_pattern_with_buffer_match(
	char_u *pat,
	पूर्णांक pat_len,
	pos_T *end_match_pos,
	पूर्णांक lowercase UNUSED)
{
    char_u  *ओळ = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(ओळ + end_match_pos->col);
    पूर्णांक	    match_len = (पूर्णांक)(word_end - (ओळ + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    जर (match == रिक्त)
	परत रिक्त;
    mch_memmove(match, pat, pat_len);
    जर (match_len > 0)
    {
#जर defined(FEAT_EVAL) || defined(FEAT_SPELL)
	जर (lowercase)
	{
	    char_u  *mword = vim_strnsave(ओळ + end_match_pos->col,
		    match_len);
	    जर (mword == रिक्त)
		जा cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    जर (lower == रिक्त)
		जा cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	अन्यथा
#endif
	    mch_memmove(match + pat_len, ओळ + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    परत match;

#जर defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    परत रिक्त;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    स्थिर पूर्णांक
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    पूर्णांक		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    पूर्णांक		*अंक_Matches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    पूर्णांक		found_new_match;
    पूर्णांक		looped_around = FALSE;
    पूर्णांक		pat_len;
    पूर्णांक		has_range = FALSE;
    पूर्णांक		compl_started = FALSE;
    पूर्णांक		शोध_flags;
    char_u	*match, *full_match;
    पूर्णांक		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != रिक्त;

#ifdef FEAT_शोध_EXTRA
    has_range = शोध_first_ओळ != 0;
#endif

    *matches = रिक्त;
    *अंक_Matches = 0;

    जर (pat == रिक्त || *pat == NUL)
	परत FAIL;

    pat_len = (पूर्णांक)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_शोध_EXTRA
    जर (has_range)
	cur_match_pos.lnum = शोध_first_ओळ;
    अन्यथा
#endif
	cur_match_pos = pre_incsearch_pos;

    शोध_flags = शोध_OPT | शोध_NOOF | शोध_PEEK | शोध_NFMSG
	| (has_range ? शोध_START : 0);

    ga_init2(&ga, आकार(char_u *), 10); // Use growable array of char_u*

    साठी (;;)
    {
	++emsg_off;
	++संदेश_silent;
	found_new_match = searchit(रिक्त, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		शोध_flags, RE_LAST, रिक्त);
	--संदेश_silent;
	--emsg_off;

	जर (found_new_match == FAIL)
	    थांबा;

#ifdef FEAT_शोध_EXTRA
	// If in range mode, check if match is within the range
	जर (has_range && (cur_match_pos.lnum < शोध_first_ओळ
		    || cur_match_pos.lnum > शोध_last_ओळ))
		थांबा;
#endif

	जर (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    जर ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		जर (looped_around)
		    थांबा;
		अन्यथा
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	जर (char_avail() || got_int)
	{
	    जर (got_int)
	    {
		(शून्य)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    जा cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	जर (end_match_pos.lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    सुरूठेवा;
	}

	// Extract the matching text prepended to completed word
	जर (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    थांबा;

	जर (exacttext)
	    match = full_match;
	अन्यथा
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    जर (match == रिक्त || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		जर (match == रिक्त || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    सुरूठेवा;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	साठी (पूर्णांक i = 0; i < ga.ga_len; ++i)
	{
	    जर (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		थांबा;
	    }
	}
	जर (match != रिक्त)
	{
	    जर (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		जा cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    जर (ga.ga_len > TAG_MANY)
		थांबा;
	}
	जर (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *अंक_Matches = ga.ga_len;
    परत OK;

cleanup:
    ga_clear_strings(&ga);
    परत FAIL;
}
