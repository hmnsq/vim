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

स्थिर पूर्णाङ्क	cmd_showtail;	// Only show path tail in lists ?
स्थिर पूर्णाङ्क	may_expand_pattern = FALSE;
स्थिर pos_T	pre_incsearch_pos; // Cursor position when incsearch started

स्थिर शून्य	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, पूर्णाङ्क usefilter, expand_T *xp, पूर्णाङ्क *complp);
स्थिर पूर्णाङ्क	ExpandFromContext(expand_T *xp, char_u *, char_u ***, पूर्णाङ्क *, पूर्णाङ्क);
स्थिर char_u	*showmatches_gettail(char_u *s);
स्थिर पूर्णाङ्क	expand_showtail(expand_T *xp);
स्थिर पूर्णाङ्क	expand_shellcmd(char_u *filepat, char_u ***matches, पूर्णाङ्क *अंक_Matches, पूर्णाङ्क flagsarg);
#यदि defined(FEAT_EVAL)
स्थिर पूर्णाङ्क	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, पूर्णाङ्क *अंक_Matches);
स्थिर पूर्णाङ्क	ExpandUserList(expand_T *xp, char_u ***matches, पूर्णाङ्क *अंक_Matches);
स्थिर char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
स्थिर char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, पूर्णाङ्क srclen, पूर्णाङ्क *new_lenp);
#endif
स्थिर पूर्णाङ्क	expand_pattern_in_buf(char_u *pat, पूर्णाङ्क dir, char_u ***matches, पूर्णाङ्क *अंक_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
स्थिर pumitem_T *compl_match_array = रिक्त;
स्थिर पूर्णाङ्क compl_match_arraysize;
// First column in cmdline of the matched item for completion.
स्थिर पूर्णाङ्क compl_startcol;
स्थिर पूर्णाङ्क compl_selected;
// cmdline before expansion
स्थिर string_T cmdline_orig = {रिक्त, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    स्थिर पूर्णाङ्क
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    विकल्प (xp->xp_context)
    {
    पक्ष EXPAND_BOOL_SETTINGS:
    पक्ष EXPAND_COLORS:
    पक्ष EXPAND_COMPILER:
    पक्ष EXPAND_DIRECTORIES:
    पक्ष EXPAND_DIRS_IN_CDPATH:
    पक्ष EXPAND_FILES:
    पक्ष EXPAND_FILES_IN_PATH:
    पक्ष EXPAND_FILETYPE:
    पक्ष EXPAND_FILETYPECMD:
    पक्ष EXPAND_HELP:
    पक्ष EXPAND_KEYMAP:
    पक्ष EXPAND_OLD_SETTING:
    पक्ष EXPAND_STRING_SETTING:
    पक्ष EXPAND_SETTING_SUBTRACT:
    पक्ष EXPAND_OWNSYNTAX:
    पक्ष EXPAND_PACKADD:
    पक्ष EXPAND_RUNTIME:
    पक्ष EXPAND_SHELLCMD:
    पक्ष EXPAND_SHELLCMDLINE:
    पक्ष EXPAND_TAGS:
    पक्ष EXPAND_TAGS_LISTFILES:
	प्रत्यागम FALSE;

    स्वभाव:
	भङ्ग;
    }

    प्रत्यागम vim_strchr(p_wop, WOP_FUZZY) != रिक्त;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    पूर्णाङ्क
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    प्रत्यागम vim_strchr(p_wop, WOP_FUZZY) != रिक्त && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    स्थिर पूर्णाङ्क
sort_func_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    यदि (*p1 != '<' && *p2 == '<') प्रत्यागम -1;
    यदि (*p1 == '<' && *p2 != '<') प्रत्यागम 1;
    प्रत्यागम STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    स्थिर शून्य
wildescape(
    expand_T	*xp,
    char_u	*str,
    पूर्णाङ्क		numfiles,
    char_u	**files)
{
    char_u	*p;
    पूर्णाङ्क		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    यदि (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	प्रति (पूर्णाङ्क i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    यदि (xp->xp_backslash & XP_BS_THREE)
	    {
		वर्ण *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		यदि (p != रिक्त)
		{
		    vim_free(files[i]);
		    files[i] = p;
#यदि defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    यदि (p != रिक्त)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    अन्यथा यदि (xp->xp_backslash & XP_BS_COMMA)
	    {
		यदि (vim_strchr(files[i], ',') != रिक्त)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    यदि (p != रिक्त)
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
	    यदि (p != रिक्त)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    यदि (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	यदि (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    अन्यथा यदि (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	प्रति (पूर्णाङ्क i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    यदि (p != रिक्त)
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
    पूर्णाङ्क		numfiles,
    char_u	**files,
    पूर्णाङ्क		options)
{
    // May change home directory back to "~"
    यदि (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    यदि (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    पूर्णाङ्क
nextwild(
    expand_T	*xp,
    पूर्णाङ्क		type,
    पूर्णाङ्क		options,	// extra options for ExpandOne()
    पूर्णाङ्क		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    पूर्णाङ्क		i;
    char_u	*p;
    पूर्णाङ्क		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    पूर्णाङ्क		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    यदि (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	यदि (ccline->निवेश_fn && ccline->xp_context == EXPAND_COMMANDS)
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

    यदि (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	प्रत्यागम OK;  // Something illegal on command line
    }
    यदि (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	प्रत्यागम FAIL;
    }

    i = (पूर्णाङ्क)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    यदि (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	प्रत्यागम FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    यदि (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	संदेश_puts("...");	    // show that we are busy
	out_flush();
    }

    यदि (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, रिक्त, रिक्त, 0, type);
    }
    अन्यथा
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	पूर्णाङ्क	pat_len = xp->xp_pattern_len;
#यदि defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	यदि (unesc != रिक्त)
	    pat_src = unesc;
#endif

	यदि (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	अन्यथा
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#यदि defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	यदि (tmp == रिक्त)
	    p = रिक्त;
	अन्यथा
	{
	    पूर्णाङ्क use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    यदि (escape)
		use_options |= WILD_ESCAPE;
	    यदि (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    यदि (p != रिक्त && type == WILD_LONGEST)
	    {
		पूर्णाङ्क	j;

		प्रति (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    यदि (c == '*' || c == '?')
			भङ्ग;
		}
		यदि ((पूर्णाङ्क)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    यदि (!wild_navigate && ccline->cmdbuff != रिक्त)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	यदि (cmdline_orig.string == रिक्त)
	    cmdline_orig.length = 0;
	अन्यथा
	    cmdline_orig.length = ccline->cmdlen;
    }

    यदि (p != रिक्त && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	पूर्णाङ्क	difflen;
	पूर्णाङ्क	v = OK;

	difflen = (पूर्णाङ्क)plen - xp->xp_pattern_len;
	यदि (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	यदि (v == OK)
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
    यदि (xp->xp_context == EXPAND_MAPPINGS && p == रिक्त)
	प्रत्यागम FAIL;

    यदि (xp->xp_numfiles <= 0 && p == रिक्त)
	beep_flush();
    अन्यथा यदि (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(शून्य)ExpandOne(xp, रिक्त, रिक्त, 0, WILD_FREE);

    vim_free(p);

    प्रत्यागम OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    स्थिर पूर्णाङ्क
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	पूर्णाङ्क		अंक_Matches,
	पूर्णाङ्क		showtail)
{
    पूर्णाङ्क	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, अंक_Matches);
    यदि (compl_match_array == रिक्त)
	प्रत्यागम EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = अंक_Matches;
    प्रति (पूर्णाङ्क i = 0; i < अंक_Matches; i++)
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
    यदि (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    प्रत्यागम EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    शून्य
cmdline_pum_display(शून्य)
{
    यदि (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    पूर्णाङ्क
cmdline_pum_active(शून्य)
{
    प्रत्यागम pum_visible() && compl_match_array != रिक्त;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    शून्य
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, पूर्णाङ्क defer_redraw)
{
    पूर्णाङ्क	save_कुंजी_Typed = कुंजी_Typed;
#ifdef FEAT_EVAL
    पूर्णाङ्क	save_RedrawingDisabled = RedrawingDisabled;
    यदि (cclp->निवेश_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    यदि (!defer_redraw)
    {
	पूर्णाङ्क save_p_lz = p_lz;
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
    कुंजी_Typed = save_कुंजी_Typed;
#ifdef FEAT_EVAL
    यदि (cclp->निवेश_fn)
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
    पूर्णाङ्क
cmdline_compl_startcol(शून्य)
{
    प्रत्यागम compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(शून्य)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    प्रत्यागम xp == रिक्त ? रिक्त : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    पूर्णाङ्क
cmdline_compl_is_fuzzy(शून्य)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    प्रत्यागम xp != रिक्त && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    स्थिर पूर्णाङ्क
skip_status_match_char(expand_T *xp, char_u *s)
{
    यदि ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	यदि (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    प्रत्यागम 2;
#endif
	प्रत्यागम 1;
    }
    प्रत्यागम 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    स्थिर पूर्णाङ्क
status_match_len(expand_T *xp, char_u *s)
{
    पूर्णाङ्क	len = 0;

#ifdef FEAT_MENU
    पूर्णाङ्क emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    यदि (emenu && menu_is_separator(s))
	प्रत्यागम 1;
#endif

    यावत् (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    प्रत्यागम len;
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
    पूर्णाङ्क		अंक_matches,
    char_u	**matches,	// list of matches
    पूर्णाङ्क		match,
    पूर्णाङ्क		showtail)
{
    पूर्णाङ्क		row;
    char_u	*buf;
    पूर्णाङ्क		len;
    पूर्णाङ्क		clen;		// length in screen cells
    पूर्णाङ्क		fillchar;
    पूर्णाङ्क		attr;
    पूर्णाङ्क		i;
    पूर्णाङ्क		highlight = TRUE;
    char_u	*selstart = रिक्त;
    पूर्णाङ्क		selstart_col = 0;
    char_u	*selend = रिक्त;
    स्थिर पूर्णाङ्क	first_match = 0;
    पूर्णाङ्क		add_बायाँ = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    पूर्णाङ्क		emenu;
#endif
    पूर्णाङ्क		l;

    यदि (matches == रिक्त)	// interrupted completion?
	प्रत्यागम;

    यदि (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    अन्यथा
	buf = alloc(topframe->fr_width + 1);
    यदि (buf == रिक्त)
	प्रत्यागम;

    यदि (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    यदि (match == 0)
	first_match = 0;
    अन्यथा यदि (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_बायाँ = TRUE;
    }
    अन्यथा
    {
	// check if match fits on the screen
	प्रति (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	यदि (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	यदि (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    प्रति (i = match; i < अंक_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		यदि (clen >= topframe->fr_width)
		    भङ्ग;
	    }
	    यदि (i == अंक_matches)
		add_बायाँ = TRUE;
	}
    }
    यदि (add_बायाँ)
	यावत् (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    यदि (clen >= topframe->fr_width)
		भङ्ग;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    यदि (first_match == 0)
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
    यावत् (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	यदि (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	यदि (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (पूर्णाङ्क)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	अन्यथा
#endif
	प्रति ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    यदि (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    अन्यथा
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (पूर्णाङ्क)STRLEN(buf + len);
	    }
	}
	यदि (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	यदि (++i == अंक_matches)
		भङ्ग;
    }

    यदि (i != अंक_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    यदि (row >= 0)
    {
	यदि (wild_menu_showing == 0)
	{
	    यदि (संदेश_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		यदि (cmdline_row == Rows - 1)
		{
		    screen_del_पंक्तियाँ(0, 0, 1, (पूर्णाङ्क)Rows, TRUE, 0, रिक्त);
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
		यदि (lastwin->w_status_height == 0)
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
	यदि (selstart != रिक्त && highlight)
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

#यदि defined(FEAT_EVAL)
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

    यदि (str == रिक्त)
	प्रत्यागम रिक्त;
    यदि ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	प्रत्यागम str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    यदि (p == रिक्त)
	प्रत्यागम str;
    vim_free(str);
    प्रत्यागम p;
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
    पूर्णाङ्क		srclen,
    पूर्णाङ्क		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    यदि ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	प्रत्यागम रिक्त;

    buf = alloc(srclen + 1);
    यदि (buf == रिक्त)
	प्रत्यागम रिक्त;

    d = buf;
    end = src + srclen;
    प्रति (p = src; p < end; ++p)
    {
	यदि (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (पूर्णाङ्क)(d - buf);
    प्रत्यागम buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    स्थिर char_u *
get_next_or_prev_match(पूर्णाङ्क mode, expand_T *xp)
{
    पूर्णाङ्क	    findex = xp->xp_selected;
    पूर्णाङ्क	    ht;

    // When no matches found, return NULL
    यदि (xp->xp_numfiles <= 0)
	प्रत्यागम रिक्त;

    यदि (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	यदि (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    अन्यथा यदि (mode == WILD_NEXT)
    {
	// Select the next entry
	++findex;
    }
    अन्यथा   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	यदि (ht > 3)
	    ht -= 2;

	यदि (mode == WILD_PAGEUP)
	{
	    यदि (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    अन्यथा यदि (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    अन्यथा
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	अन्यथा    // mode == WILD_PAGEDOWN
	{
	    यदि (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    अन्यथा यदि (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    अन्यथा
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    यदि (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	यदि (xp->xp_orig != रिक्त)
	    findex = -1;
	अन्यथा
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    यदि (p_wmnu)
    {
	यदि (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	अन्यथा यदि (vim_strchr(p_wop, WOP_PUM) != रिक्त)
	{
	    यदि (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
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
    यदि (findex == -1)
	प्रत्यागम vim_strsave(xp->xp_orig);
#यदि defined(FEAT_EVAL)
    प्रत्यागम apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#अन्यथा
    प्रत्यागम vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    स्थिर char_u *
ExpandOne_start(पूर्णाङ्क mode, expand_T *xp, char_u *str, पूर्णाङ्क options)
{
    पूर्णाङ्क		non_suf_match;		// number without matching suffix
    पूर्णाङ्क		i;
    char_u	*ss = रिक्त;

    // Do the expansion.
    यदि (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	यदि (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    अन्यथा यदि (xp->xp_numfiles == 0)
    {
	यदि (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    अन्यथा
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	यदि (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    यदि (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    अन्यथा
		non_suf_match = 1;
	    यदि ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		प्रति (i = 0; i < 2; ++i)
		    यदि (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    यदि (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		यदि (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		अन्यथा यदि (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    यदि (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    प्रत्यागम ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    स्थिर char_u *
find_longest_match(expand_T *xp, पूर्णाङ्क options)
{
    long_u	len;
    पूर्णाङ्क		mb_len = 1;
    पूर्णाङ्क		c0, ci;
    पूर्णाङ्क		i;
    char_u	*ss;

    प्रति (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	यदि (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	अन्यथा
	    c0 = xp->xp_files[0][len];
	प्रति (i = 1; i < xp->xp_numfiles; ++i)
	{
	    यदि (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    अन्यथा
		ci = xp->xp_files[i][len];
	    यदि (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		यदि (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    भङ्ग;
	    }
	    अन्यथा यदि (c0 != ci)
		भङ्ग;
	}
	यदि (i < xp->xp_numfiles)
	{
	    यदि (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    भङ्ग;
	}
    }

    ss = alloc(len + 1);
    यदि (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    प्रत्यागम ss;
}

    शून्य
free_xp_files_extra(expand_T *xp, पूर्णाङ्क numfiles)
{
    यदि (xp->xp_files_abbr != रिक्त)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = रिक्त;
    }
    यदि (xp->xp_files_kind != रिक्त)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = रिक्त;
    }
    यदि (xp->xp_files_menu != रिक्त)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = रिक्त;
    }
    यदि (xp->xp_files_info != रिक्त)
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
    पूर्णाङ्क		options,
    पूर्णाङ्क		mode)
{
    char_u	*ss = रिक्त;
    पूर्णाङ्क		orig_saved = FALSE;
#यदि defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    पूर्णाङ्क		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    यदि (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	प्रत्यागम get_next_or_prev_match(mode, xp);

    यदि (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    अन्यथा यदि (mode == WILD_APPLY)
    {
	यदि (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	अन्यथा
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#यदि defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    यदि (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	यदि (compl_match_array != रिक्त)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    यदि (mode == WILD_FREE)	// only release file name
	प्रत्यागम रिक्त;

    यदि (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#यदि defined(FEAT_EVAL)
	यदि (ss != रिक्त)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    यदि (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#यदि defined(FEAT_EVAL)
	यदि (ss != रिक्त)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    यदि (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	वर्ण	*prefix = "";
	वर्ण	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	पूर्णाङ्क	n = xp->xp_numfiles - 1;
	पूर्णाङ्क	i;
#यदि defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = रिक्त;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	यदि ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    यदि (escaped != रिक्त)
	    {
		प्रति (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    यदि (escaped[i] == रिक्त)
		    {
			यावत् (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			भङ्ग;
		    }
		}
		यदि (escaped != रिक्त)
		    files = escaped;
	    }
	}
#अन्यथा
	char_u	**files = xp->xp_files;
#endif

	यदि (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	अन्यथा यदि (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	प्रति (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	यदि (ss != रिक्त)
	{
	    size_t  ss_len = 0;

	    प्रति (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (वर्ण *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (वर्ण *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#यदि defined(FEAT_EVAL)
	यदि (escaped != रिक्त)
	{
	    प्रति (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    यदि (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    यदि (!orig_saved)
	vim_free(orig);

#यदि defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    यदि (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    प्रत्यागम ss;
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
    यदि (xp->xp_numfiles >= 0)
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
	पूर्णाङ्क		अंक_Matches,
	पूर्णाङ्क		पंक्तियाँ,
	पूर्णाङ्क		linenr,
	पूर्णाङ्क		maxlen,
	पूर्णाङ्क		showtail,
	पूर्णाङ्क		dir_attr)
{
    पूर्णाङ्क		i, j;
    पूर्णाङ्क		isdir;
    पूर्णाङ्क		lastlen;
    char_u	*p;

    lastlen = 999;
    प्रति (j = linenr; j < अंक_Matches; j += पंक्तियाँ)
    {
	यदि (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    संदेश_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    संदेश_advance(maxlen + 1);
	    संदेश_puts((वर्ण *)p);
	    संदेश_advance(maxlen + 3);
	    संदेश_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    भङ्ग;
	}
	प्रति (i = maxlen - lastlen; --i >= 0; )
	    संदेश_putchar(' ');
	यदि (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    यदि (xp->xp_numfiles != -1)
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
		यदि (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    अन्यथा
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    यदि (showtail)
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
    यदि (संदेश_col > 0)	// when not wrapped around
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
    स्थिर पूर्णाङ्क
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	पूर्णाङ्क		अंक_Matches,
	पूर्णाङ्क		showtail,
	पूर्णाङ्क		noselect)
{
    पूर्णाङ्क	retval = cmdline_pum_create(ccline, xp, matches, अंक_Matches, showtail);

    यदि (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    प्रत्यागम retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    पूर्णाङ्क
showmatches(
    expand_T	*xp,
    पूर्णाङ्क		display_wildmenu,
    पूर्णाङ्क		display_list,
    पूर्णाङ्क		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    पूर्णाङ्क		अंक_Matches;
    char_u	**matches;
    पूर्णाङ्क		i;
    पूर्णाङ्क		maxlen;
    पूर्णाङ्क		पंक्तियाँ;
    पूर्णाङ्क		columns;
    पूर्णाङ्क		attr;
    पूर्णाङ्क		showtail;
    पूर्णाङ्क		noselect = (wim_flags_arg & WIM_NOSELECT);
    पूर्णाङ्क		noinsert = (wim_flags_arg & WIM_NOINSERT);
    पूर्णाङ्क		cmdline_unchanged = noselect || noinsert;

    यदि (xp->xp_numfiles == -1)
    {
	पूर्णाङ्क retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&अंक_Matches, &matches);
	यदि (retval != EXPAND_OK)
	    प्रत्यागम retval;
	showtail = expand_showtail(xp);
    }
    अन्यथा
    {
	अंक_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    यदि (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != रिक्त)
	प्रत्यागम show_pum_matches(ccline, xp, matches, अंक_Matches,
		showtail && !cmdline_unchanged, noselect);

    यदि (display_list)
    {
	संदेश_didany = FALSE;		// lines_left will be set
	संदेश_start();			// prepare for paging
	संदेश_putchar('\n');
	out_flush();
	cmdline_row = संदेश_row;
	संदेश_didany = FALSE;		// lines_left will be set again
	संदेश_start();			// prepare for paging
    }

    यदि (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    अन्यथा यदि (display_wildmenu && !display_list)
	win_redr_status_matches(xp, अंक_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    अन्यथा यदि (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	प्रति (i = 0; i < अंक_Matches; ++i)
	{
	    पूर्णाङ्क	len;
	    यदि (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(रिक्त, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    अन्यथा
		len = vim_strsize(SHOW_MATCH(i));
	    यदि (len > maxlen)
		maxlen = len;
	}

	यदि (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    पंक्तियाँ = अंक_Matches;
	अन्यथा
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    यदि (columns < 1)
		columns = 1;
	    पंक्तियाँ = (अंक_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	यदि (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    संदेश_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    संदेश_clr_eos();
	    संदेश_advance(maxlen - 3);
	    संदेश_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	प्रति (i = 0; i < पंक्तियाँ; ++i)
	{
	    showmatches_oneline(xp, matches, अंक_Matches, पंक्तियाँ, i,
						maxlen, showtail, attr);
	    यदि (got_int)
	    {
		got_int = FALSE;
		भङ्ग;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = संदेश_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    यदि (display_wildmenu && display_list)
    {
	यदि (vim_strchr(p_wop, WOP_PUM) != रिक्त)
	    (शून्य)show_pum_matches(ccline, xp, matches, अंक_Matches,
		    showtail && !cmdline_unchanged, noselect);
	अन्यथा
	    win_redr_status_matches(xp, अंक_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    यदि (xp->xp_numfiles == -1)
    {
	FreeWild(अंक_Matches, matches);
	free_xp_files_extra(xp, अंक_Matches);
    }

    प्रत्यागम EXPAND_OK;
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
    पूर्णाङ्क		had_sep = FALSE;

    प्रति (p = s; *p != NUL; )
    {
	यदि (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	अन्यथा यदि (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    प्रत्यागम t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    स्थिर पूर्णाङ्क
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    यदि (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	प्रत्यागम FALSE;

    end = gettail(xp->xp_pattern);
    यदि (end == xp->xp_pattern)		// there is no path separator
	प्रत्यागम FALSE;

    प्रति (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	यदि (rem_backslash(s))
	    ++s;
	अन्यथा यदि (vim_strchr((char_u *)"*?[", *s) != रिक्त)
	    प्रत्यागम FALSE;
    }
    प्रत्यागम TRUE;
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
    पूर्णाङ्क		len,
    पूर्णाङ्क		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    पूर्णाङ्क		i, j;
    पूर्णाङ्क		new_len;
    char_u	*tail;
    पूर्णाङ्क		ends_in_star;

    यदि (context != EXPAND_FILES
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
	यदि (context == EXPAND_FINDFUNC
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
	    प्रति (i = 0; i < len; i++)
	    {
		यदि (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		यदि (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		यदि ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    यदि (retval != रिक्त)
	    {
		retval[0] = '^';
		j = 1;
		प्रति (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    यदि (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			भङ्ग;

		    विकल्प (fname[i])
		    {
			पक्ष '*':   retval[j++] = '.';
				    भङ्ग;
			पक्ष '~':   retval[j++] = '\\';
				    भङ्ग;
			पक्ष '?':   retval[j] = '.';
				    अनुवर्त;
			पक्ष '.':   यदि (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    भङ्ग;
			पक्ष '\\':  यदि (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    भङ्ग;
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
	यदि (retval != रिक्त)
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
	    प्रति (i = len - 2; i >= 0; --i)
	    {
		यदि (retval[i] != '\\')
		    भङ्ग;
		ends_in_star = !ends_in_star;
	    }
#endif
	    यदि ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == रिक्त
		    && vim_strchr(retval, '`') == रिक्त)
		retval[len++] = '*';
	    अन्यथा यदि (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    प्रत्यागम retval;
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
    यदि ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_खोज_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_खोज_EXTRA
	खोज_first_पंक्ति = 0; // Search entire buffer
#endif
	प्रत्यागम;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    यदि (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->निवेश_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	प्रत्यागम;
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
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, पूर्णाङ्क *complp)
{
    char_u	*p = रिक्त;
    पूर्णाङ्क		len = 0;
    पूर्णाङ्क		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    यदि (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    अन्यथा
    {
	p = cmd;
	यावत् (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	यदि (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    यावत् (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	यदि (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    यावत् (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	यदि (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != रिक्त)
	    ++p;
	len = (पूर्णाङ्क)(p - cmd);

	यदि (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    प्रत्यागम रिक्त;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	यदि ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    यावत् (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    यदि (*p == NUL && ASCII_ISALNUM(p[-1]))
	प्रत्यागम रिक्त;

    यदि (eap->cmdidx == CMD_SIZE)
    {
	यदि (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != रिक्त)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	अन्यथा यदि (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, रिक्त, xp, complp);
	    यदि (p == रिक्त)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    यदि (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	प्रत्यागम रिक्त;
    }

    प्रत्यागम p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    स्थिर शून्य
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	पूर्णाङ्क		usefilter,
	expand_T	*xp,
	पूर्णाङ्क		*complp)
{
    char_u	*p;
    पूर्णाङ्क		c;
    पूर्णाङ्क		in_quote = FALSE;
    char_u	*bow = रिक्त;	// Beginning of word
    पूर्णाङ्क		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    यावत् (*p != NUL)
    {
	यदि (has_mbyte)
	    c = mb_ptr2char(p);
	अन्यथा
	    c = *p;
	यदि (c == '\\' && p[1] != NUL)
	    ++p;
	अन्यथा यदि (c == '`')
	{
	    यदि (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	अन्यथा यदि (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != रिक्त && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    यावत् (*p != NUL)
	    {
		यदि (has_mbyte)
		    c = mb_ptr2char(p);
		अन्यथा
		    c = *p;
		यदि (c == '`' || vim_isfilec_or_wc(c))
		    भङ्ग;
		यदि (has_mbyte)
		    len = (*mb_ptr2len)(p);
		अन्यथा
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    यदि (in_quote)
		bow = p;
	    अन्यथा
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    यदि (bow != रिक्त && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    यदि (usefilter
	    || (eap != रिक्त
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	यदि (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    यदि (*xp->xp_pattern == '$')
    {
	प्रति (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    यदि (!vim_isIDc(*p))
		भङ्ग;
	यदि (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    यदि (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    यदि (*xp->xp_pattern == '~')
    {
	प्रति (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	यदि (*p == NUL && p > xp->xp_pattern + 1
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
    यदि (p == रिक्त)
	xp->xp_pattern = arg;
    अन्यथा
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    प्रत्यागम रिक्त;
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
    यदि (p == रिक्त)
	xp->xp_pattern = arg;
    अन्यथा
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    प्रत्यागम रिक्त;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    स्थिर char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    यदि (*arg != NUL)
	arg = skip_vimgrep_pat(arg, रिक्त, रिक्त);
    यदि (arg == रिक्त || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	प्रत्यागम रिक्त;
    }
    प्रत्यागम skipwhite(arg);
}

#ifdef FEAT_खोज_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    स्थिर char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    यदि (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	यदि (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    प्रत्यागम find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    स्थिर char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    पूर्णाङ्क		delim;

    delim = *arg;	    // get the delimiter
    यदि (delim)
	++arg;		    // skip delimiter if there is one

    यावत् (arg[0] != NUL && arg[0] != delim)
    {
	यदि (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    यदि (arg[0] != NUL)
	प्रत्यागम arg + 1;

    प्रत्यागम रिक्त;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    स्थिर char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    पूर्णाङ्क		delim;

    delim = *arg;
    यदि (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	यदि (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    यावत् (arg[0] != NUL && arg[0] != delim)
	    {
		यदि (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    यदि (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    यावत् (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == रिक्त)
	++arg;
    यदि (arg[0] != NUL)
	प्रत्यागम arg;

    प्रत्यागम रिक्त;
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
    यदि (*arg != '/')
	प्रत्यागम रिक्त;

    // Match regexp, not just whole words
    प्रति (++arg; *arg && *arg != '/'; arg++)
	यदि (*arg == '\\' && arg[1] != NUL)
	    arg++;
    यदि (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	यदि (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == रिक्त)
	    xp->xp_context = EXPAND_NOTHING;
	अन्यथा
	    प्रत्यागम arg;
    }

    प्रत्यागम रिक्त;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    स्थिर char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    यावत् ((xp->xp_pattern = vim_strchr(arg, ' ')) != रिक्त)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    यदि (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    प्रत्यागम रिक्त;
}
#endif

#यदि defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    स्थिर char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    यदि (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    अन्यथा
    {
	यदि ( STRNCMP(arg, "messages", p - arg) == 0
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

    प्रत्यागम रिक्त;
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

    यदि (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    अन्यथा यदि (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    अन्यथा
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    यदि (*p == NUL)
	प्रत्यागम रिक्त;
    subcmd_start = p;

    यदि (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	यदि (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    यदि (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		प्रत्यागम रिक्त;
	    }
	    p = skipwhite(p);
	}
	यदि (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	अन्यथा
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    अन्यथा यदि (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    प्रत्यागम रिक्त;
}

    स्थिर char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = रिक्त;

    p = skipwhite(arg);
    यदि (VIM_ISDIGIT(*p))
	प्रत्यागम रिक्त;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    प्रत्यागम रिक्त;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    स्थिर char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    पूर्णाङ्क    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    यदि (*p == NUL)
	प्रत्यागम रिक्त;

    प्रति (;;)
    {
	यदि (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    अनुवर्त;
	}
	यदि (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    अनुवर्त;
	}
	भङ्ग;
    }

    यदि ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    अन्यथा यदि ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    अन्यथा यदि ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    प्रत्यागम रिक्त;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    स्थिर शून्य
set_context_with_pattern(expand_T *xp)
{
    पूर्णाङ्क		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_खोज_EXTRA
    पूर्णाङ्क		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    यदि (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	प्रत्यागम;
#endif

    xp->xp_pattern = ccline->cmdbuff + skiplen;
    xp->xp_pattern_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_PATTERN_IN_BUF;
    xp->xp_खोज_dir = FORWARD;
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
	long		argt,
	पूर्णाङ्क		compl,
	पूर्णाङ्क		forceit)
{
    char_u  *nextcmd;

    विकल्प (cmdidx)
    {
	पक्ष CMD_find:
	पक्ष CMD_sfind:
	पक्ष CMD_tabfind:
	    यदि (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    भङ्ग;
	पक्ष CMD_cd:
	पक्ष CMD_chdir:
	पक्ष CMD_tcd:
	पक्ष CMD_tchdir:
	पक्ष CMD_lcd:
	पक्ष CMD_lchdir:
	    यदि (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    भङ्ग;
	पक्ष CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    भङ्ग;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	पक्ष CMD_aboveleft:
	पक्ष CMD_argdo:
	पक्ष CMD_belowright:
	पक्ष CMD_botright:
	पक्ष CMD_browse:
	पक्ष CMD_bufdo:
	पक्ष CMD_cdo:
	पक्ष CMD_cfdo:
	पक्ष CMD_confirm:
	पक्ष CMD_debug:
	पक्ष CMD_folddoclosed:
	पक्ष CMD_folddoopen:
	पक्ष CMD_hide:
	पक्ष CMD_horizontal:
	पक्ष CMD_keepalt:
	पक्ष CMD_keepjumps:
	पक्ष CMD_keepmarks:
	पक्ष CMD_keeppatterns:
	पक्ष CMD_ldo:
	पक्ष CMD_leftabove:
	पक्ष CMD_lfdo:
	पक्ष CMD_lockmarks:
	पक्ष CMD_noautocmd:
	पक्ष CMD_noswapfile:
	पक्ष CMD_rightbelow:
	पक्ष CMD_sandbox:
	पक्ष CMD_silent:
	पक्ष CMD_tab:
	पक्ष CMD_tabdo:
	पक्ष CMD_topleft:
	पक्ष CMD_unsilent:
	पक्ष CMD_verbose:
	पक्ष CMD_vertical:
	पक्ष CMD_windo:
	पक्ष CMD_vim9cmd:
	पक्ष CMD_legacy:
	    प्रत्यागम arg;

	पक्ष CMD_filter:
	    प्रत्यागम set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_खोज_EXTRA
	पक्ष CMD_match:
	    प्रत्यागम set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	पक्ष CMD_command:
	    प्रत्यागम set_context_in_user_cmd(xp, arg);

	पक्ष CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    भङ्ग;

	पक्ष CMD_global:
	पक्ष CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    यदि (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    प्रत्यागम nextcmd;

	पक्ष CMD_and:
	पक्ष CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    यदि (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    प्रत्यागम nextcmd;

	पक्ष CMD_isearch:
	पक्ष CMD_dsearch:
	पक्ष CMD_ilist:
	पक्ष CMD_dlist:
	पक्ष CMD_ijump:
	पक्ष CMD_psearch:
	पक्ष CMD_djump:
	पक्ष CMD_isplit:
	पक्ष CMD_dsplit:
	    प्रत्यागम find_cmd_after_isearch_cmd(xp, arg);
	पक्ष CMD_autocmd:
	    प्रत्यागम set_context_in_autocmd(xp, arg, FALSE);
	पक्ष CMD_doautocmd:
	पक्ष CMD_doautoall:
	    प्रत्यागम set_context_in_autocmd(xp, arg, TRUE);
	पक्ष CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    भङ्ग;
	पक्ष CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    भङ्ग;
	पक्ष CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    भङ्ग;
	पक्ष CMD_tag:
	पक्ष CMD_stag:
	पक्ष CMD_ptag:
	पक्ष CMD_ltag:
	पक्ष CMD_tselect:
	पक्ष CMD_stselect:
	पक्ष CMD_ptselect:
	पक्ष CMD_tjump:
	पक्ष CMD_stjump:
	पक्ष CMD_ptjump:
	    यदि (vim_strchr(p_wop, WOP_TAGFILE) != रिक्त)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    अन्यथा
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    भङ्ग;
	पक्ष CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    भङ्ग;
#ifdef FEAT_SYN_HL
	पक्ष CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    भङ्ग;
#endif
#ifdef FEAT_EVAL
	पक्ष CMD_final:
	पक्ष CMD_const:
	पक्ष CMD_let:
	पक्ष CMD_var:
	पक्ष CMD_if:
	पक्ष CMD_elseif:
	पक्ष CMD_while:
	पक्ष CMD_for:
	पक्ष CMD_echo:
	पक्ष CMD_echon:
	पक्ष CMD_execute:
	पक्ष CMD_echomsg:
	पक्ष CMD_echoerr:
	पक्ष CMD_echoconsole:
	पक्ष CMD_echowindow:
	पक्ष CMD_call:
	पक्ष CMD_return:
	पक्ष CMD_cexpr:
	पक्ष CMD_caddexpr:
	पक्ष CMD_cgetexpr:
	पक्ष CMD_lexpr:
	पक्ष CMD_laddexpr:
	पक्ष CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    भङ्ग;

	पक्ष CMD_unlet:
	    प्रत्यागम set_context_in_unlet_cmd(xp, arg);
	पक्ष CMD_function:
	पक्ष CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    भङ्ग;
	पक्ष CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    भङ्ग;

	पक्ष CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    भङ्ग;
#endif
	पक्ष CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    भङ्ग;
#ifdef FEAT_CSCOPE
	पक्ष CMD_cscope:
	पक्ष CMD_lcscope:
	पक्ष CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    भङ्ग;
#endif
#ifdef FEAT_SIGNS
	पक्ष CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    भङ्ग;
#endif
	पक्ष CMD_bdelete:
	पक्ष CMD_bwipeout:
	पक्ष CMD_bunload:
	    यावत् ((xp->xp_pattern = vim_strchr(arg, ' ')) != रिक्त)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	पक्ष CMD_buffer:
	पक्ष CMD_sbuffer:
	पक्ष CMD_pbuffer:
	पक्ष CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    भङ्ग;
#ifdef FEAT_DIFF
	पक्ष CMD_diffget:
	पक्ष CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    भङ्ग;
#endif
	पक्ष CMD_USER:
	पक्ष CMD_USER_BUF:
	    प्रत्यागम set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	पक्ष CMD_map:	    पक्ष CMD_noremap:
	पक्ष CMD_nmap:	    पक्ष CMD_nnoremap:
	पक्ष CMD_vmap:	    पक्ष CMD_vnoremap:
	पक्ष CMD_omap:	    पक्ष CMD_onoremap:
	पक्ष CMD_imap:	    पक्ष CMD_inoremap:
	पक्ष CMD_cmap:	    पक्ष CMD_cnoremap:
	पक्ष CMD_lmap:	    पक्ष CMD_lnoremap:
	पक्ष CMD_smap:	    पक्ष CMD_snoremap:
	पक्ष CMD_tmap:	    पक्ष CMD_tnoremap:
	पक्ष CMD_xmap:	    पक्ष CMD_xnoremap:
	    प्रत्यागम set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	पक्ष CMD_unmap:
	पक्ष CMD_nunmap:
	पक्ष CMD_vunmap:
	पक्ष CMD_ounmap:
	पक्ष CMD_iunmap:
	पक्ष CMD_cunmap:
	पक्ष CMD_lunmap:
	पक्ष CMD_sunmap:
	पक्ष CMD_tunmap:
	पक्ष CMD_xunmap:
	    प्रत्यागम set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	पक्ष CMD_mapclear:
	पक्ष CMD_nmapclear:
	पक्ष CMD_vmapclear:
	पक्ष CMD_omapclear:
	पक्ष CMD_imapclear:
	पक्ष CMD_cmapclear:
	पक्ष CMD_lmapclear:
	पक्ष CMD_smapclear:
	पक्ष CMD_tmapclear:
	पक्ष CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    भङ्ग;

	पक्ष CMD_abbreviate:	पक्ष CMD_noreabbrev:
	पक्ष CMD_cabbrev:	पक्ष CMD_cnoreabbrev:
	पक्ष CMD_iabbrev:	पक्ष CMD_inoreabbrev:
	    प्रत्यागम set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	पक्ष CMD_unabbreviate:
	पक्ष CMD_cunabbrev:
	पक्ष CMD_iunabbrev:
	    प्रत्यागम set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	पक्ष CMD_menu:	    पक्ष CMD_noremenu:	    पक्ष CMD_unmenu:
	पक्ष CMD_amenu:	    पक्ष CMD_anoremenu:	    पक्ष CMD_aunmenu:
	पक्ष CMD_nmenu:	    पक्ष CMD_nnoremenu:	    पक्ष CMD_nunmenu:
	पक्ष CMD_vmenu:	    पक्ष CMD_vnoremenu:	    पक्ष CMD_vunmenu:
	पक्ष CMD_omenu:	    पक्ष CMD_onoremenu:	    पक्ष CMD_ounmenu:
	पक्ष CMD_imenu:	    पक्ष CMD_inoremenu:	    पक्ष CMD_iunmenu:
	पक्ष CMD_cmenu:	    पक्ष CMD_cnoremenu:	    पक्ष CMD_cunmenu:
	पक्ष CMD_tlmenu:    पक्ष CMD_tlnoremenu:    पक्ष CMD_tlunmenu:
	पक्ष CMD_tmenu:				    पक्ष CMD_tunmenu:
	पक्ष CMD_popup:	    पक्ष CMD_tearoff:	    पक्ष CMD_emenu:
	    प्रत्यागम set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	पक्ष CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    भङ्ग;

	पक्ष CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    भङ्ग;

	पक्ष CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    भङ्ग;

	पक्ष CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    भङ्ग;

	पक्ष CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    भङ्ग;

	पक्ष CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    भङ्ग;

#यदि defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	पक्ष CMD_language:
	    प्रत्यागम set_context_in_lang_cmd(xp, arg);
#endif
#यदि defined(FEAT_PROFILE)
	पक्ष CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    भङ्ग;
#endif
	पक्ष CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    भङ्ग;

	पक्ष CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    भङ्ग;

	पक्ष CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    भङ्ग;

	पक्ष CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    भङ्ग;
#यदि defined(FEAT_PROFILE)
	पक्ष CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    भङ्ग;
#endif

	पक्ष CMD_argdelete:
	    यावत् ((xp->xp_pattern = vim_strchr(arg, ' ')) != रिक्त)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    भङ्ग;

#ifdef FEAT_EVAL
	पक्ष CMD_breakadd:
	पक्ष CMD_profdel:
	पक्ष CMD_breakdel:
	    प्रत्यागम set_context_in_breakadd_cmd(xp, arg, cmdidx);

	पक्ष CMD_scriptnames:
	    प्रत्यागम set_context_in_scriptnames_cmd(xp, arg);
#endif
	पक्ष CMD_filetype:
	    प्रत्यागम set_context_in_filetype_cmd(xp, arg);

	स्वभाव:
	    भङ्ग;
    }
    प्रत्यागम रिक्त;
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
    पूर्णाङ्क			len = 0;
    exarg_T		ea;
    पूर्णाङ्क			compl = EXPAND_NOTHING;
    पूर्णाङ्क			forceit = FALSE;
    पूर्णाङ्क			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_पंक्ति = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    प्रति (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != रिक्त; cmd++)
	;
    xp->xp_pattern = cmd;

    यदि (*cmd == NUL)
	प्रत्यागम रिक्त;
    यदि (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	प्रत्यागम रिक्त;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    यदि (*cmd == NUL)
	प्रत्यागम रिक्त;
    यदि (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	प्रत्यागम रिक्त;
    }

    यदि (*cmd == '|' || *cmd == '\n')
	प्रत्यागम cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    यदि (p == रिक्त)
	प्रत्यागम रिक्त;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    यदि (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    यदि (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    यदि ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	यावत् (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    यावत् (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    यदि (*p == NUL)
	    {
		यदि (ea.argt & EX_ARGOPT)
		    प्रत्यागम set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		यदि (ea.cmdidx == CMD_terminal)
		    प्रत्यागम set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    यदि (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	यदि (*arg == '>')			// append
	{
	    यदि (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	अन्यथा यदि (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    यदि (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	यदि (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    यदि (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	यावत् (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    यदि ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	यदि (*arg == NUL)
	    प्रत्यागम p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    यदि ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	यदि (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	यावत् (*p)
	{
	    यदि (*p == Ctrl_V)
	    {
		यदि (p[1] != NUL)
		    ++p;
	    }
	    अन्यथा यदि ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		यदि (*(p - 1) != '\\')
		{
		    यदि (*p == '|' || *p == '\n')
			प्रत्यागम p + 1;
		    प्रत्यागम रिक्त;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    यदि (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == रिक्त)
	// no arguments allowed but there is something
	प्रत्यागम रिक्त;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (पूर्णाङ्क)STRLEN(buff);
    यावत् (*p && p < buff + len)
    {
	यदि (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	अन्यथा
	{
	    यदि (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    यदि (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    प्रत्यागम set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    शून्य
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    पूर्णाङ्क		len,	    // length of command line (excl. NUL)
    पूर्णाङ्क		col,	    // position of cursor
    पूर्णाङ्क		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    पूर्णाङ्क			context;
#endif
    पूर्णाङ्क		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    यदि (col < len)
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
    यदि (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    अन्यथा यदि (use_ccline && ccline->निवेश_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	यदि (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(रिक्त, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    अन्यथा
#endif
	यावत् (nextcomm != रिक्त)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_पंक्ति = str;
    xp->xp_col = col;

    यदि (col < len)
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
    पूर्णाङ्क
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    पूर्णाङ्क		col,		// position of cursor
    पूर्णाङ्क		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = रिक्त;
    पूर्णाङ्क		options = WILD_ADD_SLASH|WILD_SILENT;

    यदि (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	प्रत्यागम EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    यदि (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	प्रत्यागम EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (पूर्णाङ्क)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	पूर्णाङ्क	pat_len = xp->xp_pattern_len;
#यदि defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	यदि (unesc != रिक्त)
	    pat_src = unesc;
#endif

	यदि (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	अन्यथा
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#यदि defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    यदि (file_str == रिक्त)
	प्रत्यागम EXPAND_UNSUCCESSFUL;

    यदि (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    यदि (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = रिक्त;
    }
    vim_free(file_str);

    प्रत्यागम EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	पूर्णाङ्क		*अंक_Matches,
	पूर्णाङ्क		flags,
	पूर्णाङ्क		options)
{
    पूर्णाङ्क		free_pat = FALSE;
    पूर्णाङ्क		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    यदि (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	यदि (pat == रिक्त)
	    प्रत्यागम ret;

	pat_end = pat + pat_len;
	प्रति (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    यदि (*p != '\\')
		अनुवर्त;

	    यदि (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    अन्यथा यदि (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    अन्यथा यदि (xp->xp_backslash & XP_BS_COMMA)
	    {
		यदि (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		अन्यथा यदि (*(p + 1) == ',')
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

    यदि (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, अंक_Matches);
#endif
    }
    अन्यथा
    {
	यदि (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	अन्यथा यदि (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	अन्यथा यदि (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	अन्यथा
	    flags = (flags | EW_DIR) & ~EW_FILE;
	यदि (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, अंक_Matches, matches, flags);
    }
    यदि (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    यदि (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	पूर्णाङ्क j;

	प्रति (j = 0; j < *अंक_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    यावत् (*ptr != NUL)
	    {
		यदि (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		अन्यथा यदि (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    प्रत्यागम ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    स्थिर char_u *
get_behave_arg(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    यदि (idx == 0)
	प्रत्यागम (char_u *)"mswin";
    यदि (idx == 1)
	प्रत्यागम (char_u *)"xterm";
    प्रत्यागम रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    स्थिर char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    यदि (idx < 0)
	प्रत्यागम रिक्त;

    यदि (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	वर्ण	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	प्रत्यागम (char_u *)opts_all[idx];
    }
    यदि (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	वर्ण	*opts_plugin[] = {"plugin", "on", "off"};
	प्रत्यागम (char_u *)opts_plugin[idx];
    }
    यदि (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	वर्ण	*opts_indent[] = {"indent", "on", "off"};
	प्रत्यागम (char_u *)opts_indent[idx];
    }
    यदि (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	वर्ण	*opts_onoff[] = {"on", "off"};
	प्रत्यागम (char_u *)opts_onoff[idx];
    }
    प्रत्यागम रिक्त;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    स्थिर char_u *
get_breakadd_arg(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    यदि (idx >= 0 && idx <= 3)
    {
	वर्ण	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	यदि (breakpt_expand_what == EXP_BREAKPT_ADD)
	    प्रत्यागम (char_u *)opts[idx];
	अन्यथा यदि (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    यदि (idx <= 2)
		प्रत्यागम (char_u *)opts[idx + 1];
	}
	अन्यथा
	{
	    // profdel {func, file}
	    यदि (idx <= 1)
		प्रत्यागम (char_u *)opts[idx + 1];
	}
    }
    प्रत्यागम रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    स्थिर char_u *
get_scriptnames_arg(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    scriptitem_T *si;

    यदि (!SCRIPT_ID_VALID(idx + 1))
	प्रत्यागम रिक्त;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(रिक्त, si->sn_name, NameBuff, MAXPATHL, TRUE);
    प्रत्यागम NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    स्थिर char_u *
get_retab_arg(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    यदि (idx == 0)
	प्रत्यागम (char_u *)"-indentonly";
    प्रत्यागम रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    स्थिर char_u *
get_messages_arg(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    यदि (idx == 0)
	प्रत्यागम (char_u *)"clear";
    प्रत्यागम रिक्त;
}

    स्थिर char_u *
get_mapclear_arg(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    यदि (idx == 0)
	प्रत्यागम (char_u *)"<buffer>";
    प्रत्यागम रिक्त;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    स्थिर पूर्णाङ्क
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	पूर्णाङ्क		*अंक_Matches)
{
    स्थिर संरचना expgen
    {
	पूर्णाङ्क		context;
	char_u	*((*func)(expand_T *, पूर्णाङ्क));
	पूर्णाङ्क		ic;
	पूर्णाङ्क		escaped;
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
	{EXPAND_EXPRESSION, get_व्यंजक_name, FALSE, TRUE},
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
#यदि defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
    पूर्णाङ्क	i;
    पूर्णाङ्क ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(tab); ++i)
    {
	यदि (xp->xp_context == tab[i].context)
	{
	    यदि (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, अंक_Matches,
						tab[i].func, tab[i].escaped);
	    भङ्ग;
	}
    }

    प्रत्यागम ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    स्थिर पूर्णाङ्क
map_wildopts_to_ewflags(पूर्णाङ्क options)
{
    पूर्णाङ्क		flags;

    flags = EW_DIR;	// include directories
    यदि (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    यदि (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    यदि (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    यदि (options & WILD_SILENT)
	flags |= EW_SILENT;
    यदि (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    यदि (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    प्रत्यागम flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    स्थिर पूर्णाङ्क
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    पूर्णाङ्क		*अंक_Matches,
    पूर्णाङ्क		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    पूर्णाङ्क		ret;
    पूर्णाङ्क		flags;
    char_u	*tofree = रिक्त;
    पूर्णाङ्क		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    यदि (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	प्रत्यागम expand_files_and_dirs(xp, pat, matches, अंक_Matches, flags,
								options);

    *matches = (char_u **)"";
    *अंक_Matches = 0;
    यदि (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	यदि (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					अंक_Matches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*अंक_Matches, *matches);
#endif
	    प्रत्यागम OK;
	}
	प्रत्यागम FAIL;
    }

    यदि (xp->xp_context == EXPAND_SHELLCMD)
	प्रत्यागम expand_shellcmd(pat, matches, अंक_Matches, flags);
    यदि (xp->xp_context == EXPAND_OLD_SETTING)
	प्रत्यागम ExpandOldSetting(अंक_Matches, matches);
    यदि (xp->xp_context == EXPAND_BUFFERS)
	प्रत्यागम ExpandBufnames(pat, अंक_Matches, matches, options);
#ifdef FEAT_DIFF
    यदि (xp->xp_context == EXPAND_DIFF_BUFFERS)
	प्रत्यागम ExpandBufnames(pat, अंक_Matches, matches,
						options | BUF_DIFF_FILTER);
#endif
    यदि (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	प्रत्यागम expand_tags(xp->xp_context == EXPAND_TAGS, pat, अंक_Matches,
								matches);
    यदि (xp->xp_context == EXPAND_COLORS)
    {
	वर्ण *directories[] = {"colors", रिक्त};
	प्रत्यागम ExpandRTDir(pat, DIP_START + DIP_OPT, अंक_Matches, matches,
								directories);
    }
    यदि (xp->xp_context == EXPAND_COMPILER)
    {
	वर्ण *directories[] = {"compiler", रिक्त};
	प्रत्यागम ExpandRTDir(pat, 0, अंक_Matches, matches, directories);
    }
    यदि (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	वर्ण *directories[] = {"syntax", रिक्त};
	प्रत्यागम ExpandRTDir(pat, 0, अंक_Matches, matches, directories);
    }
    यदि (xp->xp_context == EXPAND_FILETYPE)
    {
	वर्ण *directories[] = {"syntax", "indent", "ftplugin", रिक्त};
	प्रत्यागम ExpandRTDir(pat, 0, अंक_Matches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    यदि (xp->xp_context == EXPAND_KEYMAP)
    {
	वर्ण *directories[] = {"keymap", रिक्त};
	प्रत्यागम ExpandRTDir(pat, 0, अंक_Matches, matches, directories);
    }
#endif
#यदि defined(FEAT_EVAL)
    यदि (xp->xp_context == EXPAND_USER_LIST)
	प्रत्यागम ExpandUserList(xp, matches, अंक_Matches);
#endif
    यदि (xp->xp_context == EXPAND_PACKADD)
	प्रत्यागम ExpandPackAddDir(pat, अंक_Matches, matches);
    यदि (xp->xp_context == EXPAND_RUNTIME)
	प्रत्यागम expand_runtime_cmd(pat, अंक_Matches, matches);
    यदि (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	प्रत्यागम expand_pattern_in_buf(pat, xp->xp_खोज_dir,
		matches, अंक_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    यदि ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	पूर्णाङ्क len = (पूर्णाङ्क)STRLEN(pat) + 20;

	tofree = alloc(len);
	यदि (tofree == रिक्त)
	    प्रत्यागम FAIL;
	vim_snprintf((वर्ण *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    यदि (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	यदि (regmatch.regprog == रिक्त)
	{
	    vim_free(tofree);
	    प्रत्यागम FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    यदि (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, अंक_Matches, matches, fuzzy);
    अन्यथा यदि (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, अंक_Matches, matches);
    अन्यथा यदि (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, अंक_Matches, matches);
    अन्यथा यदि (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, अंक_Matches, matches);
    अन्यथा यदि (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, अंक_Matches);
    अन्यथा यदि (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, अंक_Matches);
#यदि defined(FEAT_TERMINAL)
    अन्यथा यदि (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, अंक_Matches);
#endif
#यदि defined(FEAT_EVAL)
    अन्यथा यदि (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, अंक_Matches);
#endif
    अन्यथा
	ret = ExpandOther(pat, xp, &regmatch, matches, अंक_Matches);

    यदि (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    प्रत्यागम ret;
}

    पूर्णाङ्क
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    पूर्णाङ्क		*अंक_Matches,
    char_u	*(*func)(expand_T *, पूर्णाङ्क),
					  // returns a string from the list
    पूर्णाङ्क		escaped)
{
    प्रत्यागम ExpandGenericExt(
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
    पूर्णाङ्क
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    पूर्णाङ्क		*अंक_Matches,
    char_u	*(*func)(expand_T *, पूर्णाङ्क),
					  // returns a string from the list
    पूर्णाङ्क		escaped,
    पूर्णाङ्क		sortStartIdx)
{
    पूर्णाङ्क		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = रिक्त;
    पूर्णाङ्क		score = 0;
    पूर्णाङ्क		fuzzy;
    पूर्णाङ्क		match;
    पूर्णाङ्क		sort_matches = FALSE;
    पूर्णाङ्क		funcsort = FALSE;
    पूर्णाङ्क		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = रिक्त;
    *अंक_Matches = 0;

    यदि (!fuzzy)
	ga_init2(&ga, परिमाण(वर्ण *), 30);
    अन्यथा
	ga_init2(&ga, परिमाण(fuzmatch_str_T), 30);

    प्रति (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	यदि (str == रिक्त)	    // end of list
	    भङ्ग;
	यदि (*str == NUL)	    // skip empty strings
	    अनुवर्त;

	यदि (xp->xp_pattern[0] != NUL)
	{
	    यदि (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    अन्यथा
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	अन्यथा
	    match = TRUE;

	यदि (!match)
	    अनुवर्त;

	यदि (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	अन्यथा
	    str = vim_strsave(str);
	यदि (str == रिक्त)
	{
	    यदि (!fuzzy)
	    {
		ga_clear_strings(&ga);
		प्रत्यागम FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    प्रत्यागम FAIL;
	}

	यदि (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    भङ्ग;
	}

	यदि (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	अन्यथा
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

#ifdef FEAT_MENU
	यदि (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    यदि (*str == '\001')
		*str = '.';
	}
#endif

	यदि (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    यदि (ga.ga_len == 0)
	प्रत्यागम OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    यदि (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    यदि (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    यदि (sort_matches && sortStartMatchIdx != -1)
    {
	यदि (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((शून्य *)ga.ga_data, (size_t)ga.ga_len, परिमाण(char_u *),
							   sort_func_compare);
	अन्यथा
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    यदि (!fuzzy)
    {
	*matches = ga.ga_data;
	*अंक_Matches = ga.ga_len;
    }
    अन्यथा
    {
	यदि (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    प्रत्यागम FAIL;
	*अंक_Matches = ga.ga_len;
    }

#यदि defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    प्रत्यागम OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    स्थिर शून्य
expand_shellcmd_onedir(
	char_u		*pathed_pattern,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	पूर्णाङ्क		*अंक_Matches,
	पूर्णाङ्क		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    यदि (expand_wildcards(1, &pathed_pattern, अंक_Matches, matches, flags) != OK)
	प्रत्यागम;

    यदि (ga_grow(gap, *अंक_Matches) == FAIL)
    {
	FreeWild(*अंक_Matches, *matches);
	प्रत्यागम;
    }

    प्रति (पूर्णाङ्क i = 0; i < *अंक_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	यदि (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    यदि (HASHITEM_EMPTY(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
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
    स्थिर पूर्णाङ्क
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    पूर्णाङ्क		*अंक_Matches,	// return: number of matches
    पूर्णाङ्क		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = रिक्त;
    पूर्णाङ्क		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    पूर्णाङ्क		flags = flagsarg;
    पूर्णाङ्क		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    यदि (buf == रिक्त)
	प्रत्यागम FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    यदि (pat == रिक्त)
    {
	vim_free(buf);
	प्रत्यागम FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    प्रति (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	यदि (*s != '\\')
	    अनुवर्त;

	p = s + 1;
	यदि (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    यदि (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    अन्यथा
    {
	// For an absolute name we don't use $PATH.
	यदि (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	यदि (path == रिक्त)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, परिमाण(वर्ण *), 10);
    hash_init(&found_ht);
    प्रति (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	यदि (*s == NUL)
	{
	    यदि (did_curdir)
		भङ्ग;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	अन्यथा
	{
#यदि defined(MSWIN)
	    e = vim_strchr(s, ';');
#अन्यथा
	    e = vim_strchr(s, ':');
#endif
	    यदि (e == रिक्त)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    यदि (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    अन्यथा
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? परिमाण(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	यदि (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    यदि (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		यदि (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, अंक_Matches, flags,
				    &found_ht, &ga);
	}

	यदि (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_data;
    *अंक_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    यदि (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    प्रत्यागम OK;
}

#यदि defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    स्थिर शून्य *
call_user_expand_func(
    शून्य	*(*user_expand_func)(char_u *, पूर्णाङ्क, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    पूर्णाङ्क		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = रिक्त;
    शून्य	*ret;

    यदि (xp->xp_arg == रिक्त || xp->xp_arg[0] == '\0' || xp->xp_पंक्ति == रिक्त)
	प्रत्यागम रिक्त;

    यदि (ccline->cmdbuff != रिक्त)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	पूर्णाङ्क unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	यदि (pat == रिक्त)
	    pat = vim_strnsave(xp->xp_pattern, xp->xp_pattern_len);
    }

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = xp->xp_पंक्ति;
    args[2].v_type = VAR_संख्या;
    args[2].vval.v_संख्या = xp->xp_col;
    args[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, args);

    current_sctx = save_current_sctx;
    यदि (ccline->cmdbuff != रिक्त)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    प्रत्यागम ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    स्थिर पूर्णाङ्क
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    पूर्णाङ्क		*अंक_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    पूर्णाङ्क		keep;
    garray_T	ga;
    पूर्णाङ्क		fuzzy;
    पूर्णाङ्क		match;
    पूर्णाङ्क		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = रिक्त;
    *अंक_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    यदि (retstr == रिक्त)
	प्रत्यागम FAIL;

    यदि (!fuzzy)
	ga_init2(&ga, परिमाण(वर्ण *), 3);
    अन्यथा
	ga_init2(&ga, परिमाण(fuzmatch_str_T), 3);

    प्रति (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	यदि (e == रिक्त)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	यदि (xp->xp_pattern[0] != NUL)
	{
	    यदि (!fuzzy)
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

	यदि (match)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    यदि (p == रिक्त)
		भङ्ग;

	    यदि (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		भङ्ग;
	    }

	    यदि (!fuzzy)
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

	यदि (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    यदि (ga.ga_len == 0)
	प्रत्यागम OK;

    यदि (!fuzzy)
    {
	*matches = ga.ga_data;
	*अंक_Matches = ga.ga_len;
    }
    अन्यथा
    {
	यदि (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    प्रत्यागम FAIL;
	*अंक_Matches = ga.ga_len;
    }
    प्रत्यागम OK;
}

    शून्य
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    पूर्णाङ्क		*अंक_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    पूर्णाङ्क		have_extra = FALSE;
    पूर्णाङ्क		i;

    ga_init2(&ga, परिमाण(वर्ण *), 3);
    ga_init2(&ga_abbr, परिमाण(वर्ण *), 3);
    ga_init2(&ga_kind, परिमाण(वर्ण *), 3);
    ga_init2(&ga_menu, परिमाण(वर्ण *), 3);
    ga_init2(&ga_info, परिमाण(वर्ण *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_वस्तुएँ(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = रिक्त;
	char_u	*abbr = रिक्त;
	char_u	*kind = रिक्त;
	char_u	*menu = रिक्त;
	char_u	*info = रिक्त;

	यदि (tv->v_type == VAR_STRING)
	{
	    यदि (tv->vval.v_string == रिक्त)
		अनुवर्त;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	अन्यथा यदि (tv->v_type == VAR_DICT && tv->vval.v_dict != रिक्त)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    यदि (word == रिक्त)
		अनुवर्त;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    यदि (abbr != रिक्त || kind != रिक्त || menu != रिक्त || info != रिक्त)
		have_extra = TRUE;
	}
	अन्यथा
	    अनुवर्त;  // Skip other types

	यदि (p == रिक्त
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
	    भङ्ग;
	}

	((char_u **)ga.ga_data)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_data)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_data)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_data)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_data)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_data;
    *अंक_Matches = ga.ga_len;
    यदि (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    अन्यथा
    {
	// No extra info collected; free the placeholder NULL entries.
	प्रति (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	प्रति (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	प्रति (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	प्रति (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    स्थिर पूर्णाङ्क
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    पूर्णाङ्क		*अंक_Matches)
{
    list_T      *retlist;

    *matches = रिक्त;
    *अंक_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    यदि (retlist == रिक्त)
	प्रत्यागम FAIL;

    expand_process_user_list(retlist, matches, अंक_Matches, xp);
    list_unref(retlist);
    प्रत्यागम OK;
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
    पूर्णाङ्क		expand_options,
    पूर्णाङ्क		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    यदि (buf == रिक्त)
	प्रत्यागम;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#यदि defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#अन्यथा
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    यावत् (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	यदि (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    पूर्णाङ्क	    अंक_p;
	    char_u  **p;

	    यदि (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    यदि (ExpandFromContext(&xpc, buf, &p, &अंक_p,
			     WILD_SILENT|expand_options) != FAIL && अंक_p > 0)
	    {
		ExpandEscape(&xpc, buf, अंक_p, p, WILD_SILENT|expand_options);

		यदि (ga_grow(ga, अंक_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    प्रति (पूर्णाङ्क i = 0; i < अंक_p; ++i)
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
    पूर्णाङ्क
wildmenu_translate_कुंजी(
	cmdline_info_T	*cclp,
	पूर्णाङ्क		कुंजी,
	expand_T	*xp,
	पूर्णाङ्क		did_wild_list)
{
    पूर्णाङ्क c = कुंजी;

    यदि (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	विकल्प (c)
	{
	    पक्ष K_UP:	  c = K_बायाँ; भङ्ग;
	    पक्ष K_DOWN:  c = K_दायाँ; भङ्ग;
	    पक्ष K_बायाँ:  c = K_UP; भङ्ग;
	    पक्ष K_दायाँ: c = K_DOWN; भङ्ग;
	    स्वभाव:	  भङ्ग;
	}
    }

    यदि (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	यदि (c == K_बायाँ)
	    c = Ctrl_P;
	अन्यथा यदि (c == K_दायाँ)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    यदि (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    प्रत्यागम c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    स्थिर शून्य
cmdline_del(cmdline_info_T *cclp, पूर्णाङ्क from)
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
    स्थिर पूर्णाङ्क
wildmenu_process_कुंजी_menunames(cmdline_info_T *cclp, पूर्णाङ्क कुंजी, expand_T *xp)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    यदि (कुंजी == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	कुंजी = p_wc;
	कुंजी_Typed = TRUE;  // in case the key was mapped
    }
    अन्यथा यदि (कुंजी == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	पूर्णाङ्क found = FALSE;

	j = (पूर्णाङ्क)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	यावत् (--j > 0)
	{
	    // check for start of menu name
	    यदि (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		भङ्ग;
	    }
	    // check for start of submenu name
	    यदि (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		यदि (found)
		{
		    i = j + 1;
		    भङ्ग;
		}
		अन्यथा
		    found = TRUE;
	    }
	}
	यदि (i > 0)
	    cmdline_del(cclp, i);
	कुंजी = p_wc;
	कुंजी_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    प्रत्यागम कुंजी;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    स्थिर पूर्णाङ्क
wildmenu_process_कुंजी_filenames(cmdline_info_T *cclp, पूर्णाङ्क कुंजी, expand_T *xp)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    यदि (कुंजी == K_DOWN
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	कुंजी = p_wc;
	कुंजी_Typed = TRUE;  // in case the key was mapped
    }
    अन्यथा यदि (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && कुंजी == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	पूर्णाङ्क found = FALSE;

	j = cclp->cmdpos;
	i = (पूर्णाङ्क)(xp->xp_pattern - cclp->cmdbuff);
	यावत् (--j > i)
	{
	    यदि (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    यदि (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		भङ्ग;
	    }
	}
	यदि (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    कुंजी = p_wc;
	    कुंजी_Typed = TRUE;  // in case the key was mapped
	}
    }
    अन्यथा यदि (कुंजी == K_UP)
    {
	// go up a directory
	पूर्णाङ्क found = FALSE;

	j = cclp->cmdpos - 1;
	i = (पूर्णाङ्क)(xp->xp_pattern - cclp->cmdbuff);
	यावत् (--j > i)
	{
	    यदि (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    यदि (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == रिक्त
#endif
	       )
	    {
		यदि (found)
		{
		    i = j + 1;
		    भङ्ग;
		}
		अन्यथा
		    found = TRUE;
	    }
	}

	यदि (!found)
	    j = i;
	अन्यथा यदि (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	अन्यथा यदि (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	अन्यथा
	    j = 0;
	यदि (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	अन्यथा यदि (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	कुंजी = p_wc;
	कुंजी_Typed = TRUE;
    }

    प्रत्यागम कुंजी;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    पूर्णाङ्क
wildmenu_process_कुंजी(cmdline_info_T *cclp, पूर्णाङ्क कुंजी, expand_T *xp)
{
    यदि (xp->xp_context == EXPAND_MENUNAMES)
	प्रत्यागम wildmenu_process_कुंजी_menunames(cclp, कुंजी, xp);
    अन्यथा यदि ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	प्रत्यागम wildmenu_process_कुंजी_filenames(cclp, कुंजी, xp);

    प्रत्यागम कुंजी;
}

/*
 * Free expanded names when finished walking through the matches
 */
    शून्य
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    पूर्णाङ्क skt = कुंजी_Typed;

    यदि (!p_wmnu || wild_menu_showing == 0)
	प्रत्यागम;

#ifdef FEAT_EVAL
    पूर्णाङ्क save_RedrawingDisabled = RedrawingDisabled;
    यदि (cclp->निवेश_fn)
	RedrawingDisabled = 0;
#endif

#यदि defined(FEAT_खोज_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    यदि (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    अन्यथा यदि (save_p_ls != -1)
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
    कुंजी_Typed = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    यदि (cclp->निवेश_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#यदि defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    शून्य
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    पूर्णाङ्क		filtered = FALSE;
    पूर्णाङ्क		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    pat = tv_get_string(&argvars[0]);
    यदि (check_for_string_arg(argvars, 1) == FAIL)
	प्रत्यागम;
    type = tv_get_string(&argvars[1]);

    यदि (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], रिक्त);

    यदि (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    यदि (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    यदि (STRCMP(type, "cmdline") == 0)
    {
	पूर्णाङ्क cmdline_len = (पूर्णाङ्क)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (पूर्णाङ्क)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    अन्यथा
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (पूर्णाङ्क)STRLEN(xpc.xp_pattern);
	xpc.xp_पंक्ति = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	विकल्प (xpc.xp_context)
	{
	पक्ष EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    प्रत्यागम;

	पक्ष EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    यदि (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		प्रत्यागम;
	    }

	    xpc.xp_arg = type + 7;
	    भङ्ग;

	पक्ष EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    यदि (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		प्रत्यागम;
	    }

	    xpc.xp_arg = type + 11;
	    भङ्ग;

# यदि defined(FEAT_MENU)
	पक्ष EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (पूर्णाङ्क)(xpc.xp_pattern - pattern_start);
	    भङ्ग;
# endif

# ifdef FEAT_CSCOPE
	पक्ष EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (पूर्णाङ्क)(xpc.xp_pattern - pattern_start);
	    भङ्ग;
# endif

# ifdef FEAT_SIGNS
	पक्ष EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (पूर्णाङ्क)(xpc.xp_pattern - pattern_start);
	    भङ्ग;
# endif

	पक्ष EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (पूर्णाङ्क)(xpc.xp_pattern - pattern_start);
	    भङ्ग;

	पक्ष EXPAND_SHELLCMDLINE:
	{
	    पूर्णाङ्क context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(रिक्त, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (पूर्णाङ्क)(xpc.xp_pattern - pattern_start);
	    भङ्ग;
	}

	पक्ष EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    भङ्ग;

	पक्ष EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    भङ्ग;

	स्वभाव:
	    भङ्ग;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	पूर्णाङ्क	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	यदि (unesc != रिक्त)
	    pat_src = unesc;

	यदि (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	अन्यथा
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    यदि (rettv_list_alloc(rettv) == OK && pat != रिक्त)
    {
	पूर्णाङ्क	i;

	ExpandOne(&xpc, pat, रिक्त, options, WILD_ALL_KEEP);

	प्रति (i = 0; i < xpc.xp_numfiles; i++)
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
    पूर्णाङ्क		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    यदि (check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (पूर्णाङ्क)STRLEN(pat);
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
    पूर्णाङ्क		    idx;
    पूर्णाङ्क		    ret = OK;

    यदि (rettv_dict_alloc(rettv) == FAIL || ccline == रिक्त
	    || ccline->xpc == रिक्त || ccline->xpc->xp_files == रिक्त)
	प्रत्यागम;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (पूर्णाङ्क)cmdline_orig.length);
    यदि (ret == OK)
	ret = dict_add_संख्या(retdict, "pum_visible", pum_visible());
    यदि (ret == OK)
	ret = dict_add_संख्या(retdict, "selected", ccline->xpc->xp_selected);
    यदि (ret == OK)
    {
	li = list_alloc();
	यदि (li == रिक्त)
	    प्रत्यागम;
	ret = dict_add_list(retdict, "matches", li);
	यदि (ret == FAIL)
	    list_unref(li);
	प्रति (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    स्थिर पूर्णाङ्क
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*पंक्ति, *start_पंक्ति, *end_पंक्ति;
    पूर्णाङ्क		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    पूर्णाङ्क		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != रिक्त;

    यदि (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	प्रत्यागम FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_पंक्ति = ml_get(start->lnum);
    char_u  *start_ptr = start_पंक्ति + start->col;
    पूर्णाङ्क	    is_single_पंक्ति = start->lnum == end->lnum;

    segment_len = is_single_पंक्ति ? (end->col - start->col)
			: (पूर्णाङ्क)(ml_get_len(start->lnum) - start->col);
    यदि (ga_grow(&ga, segment_len + 2) != OK)
	गच्छ fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    यदि (!is_single_पंक्ति)
    {
	यदि (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	अन्यथा
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    यदि (!is_single_पंक्ति)
    {
	प्रति (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    पूर्णाङ्क  linelen;

	    पंक्ति = ml_get(lnum);
	    linelen = (पूर्णाङ्क)ml_get_len(lnum);
	    यदि (ga_grow(&ga, linelen + 2) != OK)
		गच्छ fail;
	    ga_concat_len(&ga, पंक्ति, linelen);
	    यदि (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    अन्यथा
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_पंक्ति = ml_get(end->lnum);
    word_end = find_word_end(end_पंक्ति + end->col);
    segment_len = (पूर्णाङ्क)(word_end - end_पंक्ति);
    यदि (ga_grow(&ga, segment_len) != OK)
	गच्छ fail;
    ga_concat_len(&ga, end_पंक्ति + (is_single_पंक्ति ? end->col : 0),
	    segment_len - (is_single_पंक्ति ? end->col : 0));

    // Null-terminate
    यदि (ga_grow(&ga, 1) != OK)
	गच्छ fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    प्रत्यागम OK;

fail:
    ga_clear(&ga);
    प्रत्यागम FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    स्थिर पूर्णाङ्क
is_regex_match(char_u *pat, char_u *str)
{
    यदि (STRCMP(pat, str) == 0)
	प्रत्यागम TRUE;

    regmatch_T	regmatch;
    पूर्णाङ्क		result;

    ++emsg_off;
    ++संदेश_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --संदेश_silent;

    यदि (regmatch.regprog == रिक्त)
	प्रत्यागम FALSE;
    regmatch.rm_ic = p_ic;
    यदि (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++संदेश_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --संदेश_silent;

    vim_regfree(regmatch.regprog);
    प्रत्यागम result;
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
	पूर्णाङ्क pat_len,
	pos_T *end_match_pos,
	पूर्णाङ्क lowercase UNUSED)
{
    char_u  *पंक्ति = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(पंक्ति + end_match_pos->col);
    पूर्णाङ्क	    match_len = (पूर्णाङ्क)(word_end - (पंक्ति + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    यदि (match == रिक्त)
	प्रत्यागम रिक्त;
    mch_memmove(match, pat, pat_len);
    यदि (match_len > 0)
    {
#यदि defined(FEAT_EVAL) || defined(FEAT_SPELL)
	यदि (lowercase)
	{
	    char_u  *mword = vim_strnsave(पंक्ति + end_match_pos->col,
		    match_len);
	    यदि (mword == रिक्त)
		गच्छ cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    यदि (lower == रिक्त)
		गच्छ cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	अन्यथा
#endif
	    mch_memmove(match + pat_len, पंक्ति + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    प्रत्यागम match;

#यदि defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    प्रत्यागम रिक्त;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    स्थिर पूर्णाङ्क
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    पूर्णाङ्क		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    पूर्णाङ्क		*अंक_Matches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    पूर्णाङ्क		found_new_match;
    पूर्णाङ्क		looped_around = FALSE;
    पूर्णाङ्क		pat_len;
    पूर्णाङ्क		has_range = FALSE;
    पूर्णाङ्क		compl_started = FALSE;
    पूर्णाङ्क		खोज_flags;
    char_u	*match, *full_match;
    पूर्णाङ्क		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != रिक्त;

#ifdef FEAT_खोज_EXTRA
    has_range = खोज_first_पंक्ति != 0;
#endif

    *matches = रिक्त;
    *अंक_Matches = 0;

    यदि (pat == रिक्त || *pat == NUL)
	प्रत्यागम FAIL;

    pat_len = (पूर्णाङ्क)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_खोज_EXTRA
    यदि (has_range)
	cur_match_pos.lnum = खोज_first_पंक्ति;
    अन्यथा
#endif
	cur_match_pos = pre_incsearch_pos;

    खोज_flags = खोज_OPT | खोज_NOOF | खोज_PEEK | खोज_NFMSG
	| (has_range ? खोज_START : 0);

    ga_init2(&ga, परिमाण(char_u *), 10); // Use growable array of char_u*

    प्रति (;;)
    {
	++emsg_off;
	++संदेश_silent;
	found_new_match = searchit(रिक्त, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		खोज_flags, RE_LAST, रिक्त);
	--संदेश_silent;
	--emsg_off;

	यदि (found_new_match == FAIL)
	    भङ्ग;

#ifdef FEAT_खोज_EXTRA
	// If in range mode, check if match is within the range
	यदि (has_range && (cur_match_pos.lnum < खोज_first_पंक्ति
		    || cur_match_pos.lnum > खोज_last_पंक्ति))
		भङ्ग;
#endif

	यदि (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    यदि ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		यदि (looped_around)
		    भङ्ग;
		अन्यथा
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	यदि (char_avail() || got_int)
	{
	    यदि (got_int)
	    {
		(शून्य)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    गच्छ cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	यदि (end_match_pos.lnum > curbuf->b_ml.ml_पंक्ति_गिनती)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    अनुवर्त;
	}

	// Extract the matching text prepended to completed word
	यदि (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    भङ्ग;

	यदि (exacttext)
	    match = full_match;
	अन्यथा
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    यदि (match == रिक्त || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		यदि (match == रिक्त || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    अनुवर्त;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	प्रति (पूर्णाङ्क i = 0; i < ga.ga_len; ++i)
	{
	    यदि (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		भङ्ग;
	    }
	}
	यदि (match != रिक्त)
	{
	    यदि (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		गच्छ cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    यदि (ga.ga_len > TAG_MANY)
		भङ्ग;
	}
	यदि (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *अंक_Matches = ga.ga_len;
    प्रत्यागम OK;

cleanup:
    ga_clear_strings(&ga);
    प्रत्यागम FAIL;
}
