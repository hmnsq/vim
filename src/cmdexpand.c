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

#அந்தர்பாவ "vim.h"

நிலையான முழு	cmd_showtail;	// Only show path tail in lists ?
நிலையான முழு	may_expand_pattern = FALSE;
நிலையான pos_T	pre_incsearch_pos; // Cursor position when incsearch started

நிலையான வெறுமை	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, முழு usefilter, expand_T *xp, முழு *complp);
நிலையான முழு	ExpandFromContext(expand_T *xp, char_u *, char_u ***, முழு *, முழு);
நிலையான char_u	*showmatches_gettail(char_u *s);
நிலையான முழு	expand_showtail(expand_T *xp);
நிலையான முழு	expand_shellcmd(char_u *filepat, char_u ***matches, முழு *இலக்கம்_Matches, முழு flagsarg);
#எனில் defined(FEAT_EVAL)
நிலையான முழு	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, முழு *இலக்கம்_Matches);
நிலையான முழு	ExpandUserList(expand_T *xp, char_u ***matches, முழு *இலக்கம்_Matches);
நிலையான char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
நிலையான char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, முழு srclen, முழு *new_lenp);
#endif
நிலையான முழு	expand_pattern_in_buf(char_u *pat, முழு dir, char_u ***matches, முழு *இலக்கம்_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
நிலையான pumitem_T *compl_match_array = வெற்று;
நிலையான முழு compl_match_arraysize;
// First column in cmdline of the matched item for completion.
நிலையான முழு compl_startcol;
நிலையான முழு compl_selected;
// cmdline before expansion
நிலையான string_T cmdline_orig = {வெற்று, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    நிலையான முழு
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    தேர்வு (xp->xp_context)
    {
    நிலை EXPAND_BOOL_SETTINGS:
    நிலை EXPAND_COLORS:
    நிலை EXPAND_COMPILER:
    நிலை EXPAND_DIRECTORIES:
    நிலை EXPAND_DIRS_IN_CDPATH:
    நிலை EXPAND_FILES:
    நிலை EXPAND_FILES_IN_PATH:
    நிலை EXPAND_FILETYPE:
    நிலை EXPAND_FILETYPECMD:
    நிலை EXPAND_HELP:
    நிலை EXPAND_KEYMAP:
    நிலை EXPAND_OLD_SETTING:
    நிலை EXPAND_STRING_SETTING:
    நிலை EXPAND_SETTING_SUBTRACT:
    நிலை EXPAND_OWNSYNTAX:
    நிலை EXPAND_PACKADD:
    நிலை EXPAND_RUNTIME:
    நிலை EXPAND_SHELLCMD:
    நிலை EXPAND_SHELLCMDLINE:
    நிலை EXPAND_TAGS:
    நிலை EXPAND_TAGS_LISTFILES:
	திரும்பு FALSE;

    இயல்பு:
	நிறுத்து;
    }

    திரும்பு vim_strchr(p_wop, WOP_FUZZY) != வெற்று;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    முழு
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    திரும்பு vim_strchr(p_wop, WOP_FUZZY) != வெற்று && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    நிலையான முழு
sort_func_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    எனில் (*p1 != '<' && *p2 == '<') திரும்பு -1;
    எனில் (*p1 == '<' && *p2 != '<') திரும்பு 1;
    திரும்பு STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    நிலையான வெறுமை
wildescape(
    expand_T	*xp,
    char_u	*str,
    முழு		numfiles,
    char_u	**files)
{
    char_u	*p;
    முழு		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    எனில் (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	க்கு (முழு i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    எனில் (xp->xp_backslash & XP_BS_THREE)
	    {
		எழுத்து *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		எனில் (p != வெற்று)
		{
		    vim_free(files[i]);
		    files[i] = p;
#எனில் defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    எனில் (p != வெற்று)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    இல்லையெனில் எனில் (xp->xp_backslash & XP_BS_COMMA)
	    {
		எனில் (vim_strchr(files[i], ',') != வெற்று)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    எனில் (p != வெற்று)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ifdef BACKSLASH_IN_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#இல்லையெனில்
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#endif
	    எனில் (p != வெற்று)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    எனில் (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	எனில் (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    இல்லையெனில் எனில் (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	க்கு (முழு i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    எனில் (p != வெற்று)
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
    நிலையான வெறுமை
ExpandEscape(
    expand_T	*xp,
    char_u	*str,
    முழு		numfiles,
    char_u	**files,
    முழு		options)
{
    // May change home directory back to "~"
    எனில் (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    எனில் (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    முழு
nextwild(
    expand_T	*xp,
    முழு		type,
    முழு		options,	// extra options for ExpandOne()
    முழு		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    முழு		i;
    char_u	*p;
    முழு		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    முழு		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    எனில் (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	எனில் (ccline->உள்ளீடு_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	இல்லையெனில்
#endif
	{
	    may_expand_pattern = options & WILD_MAY_EXPAND_PATTERN;
	    set_expand_context(xp);
	    may_expand_pattern = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    எனில் (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	திரும்பு OK;  // Something illegal on command line
    }
    எனில் (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	திரும்பு FAIL;
    }

    i = (முழு)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    எனில் (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	திரும்பு FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    எனில் (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	செய்தி_puts("...");	    // show that we are busy
	out_flush();
    }

    எனில் (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, வெற்று, வெற்று, 0, type);
    }
    இல்லையெனில்
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	முழு	pat_len = xp->xp_pattern_len;
#எனில் defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	எனில் (unesc != வெற்று)
	    pat_src = unesc;
#endif

	எனில் (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	இல்லையெனில்
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#எனில் defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	எனில் (tmp == வெற்று)
	    p = வெற்று;
	இல்லையெனில்
	{
	    முழு use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    எனில் (escape)
		use_options |= WILD_ESCAPE;
	    எனில் (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    எனில் (p != வெற்று && type == WILD_LONGEST)
	    {
		முழு	j;

		க்கு (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    எனில் (c == '*' || c == '?')
			நிறுத்து;
		}
		எனில் ((முழு)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    எனில் (!wild_navigate && ccline->cmdbuff != வெற்று)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	எனில் (cmdline_orig.string == வெற்று)
	    cmdline_orig.length = 0;
	இல்லையெனில்
	    cmdline_orig.length = ccline->cmdlen;
    }

    எனில் (p != வெற்று && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	முழு	difflen;
	முழு	v = OK;

	difflen = (முழு)plen - xp->xp_pattern_len;
	எனில் (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	எனில் (v == OK)
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
    எனில் (xp->xp_context == EXPAND_MAPPINGS && p == வெற்று)
	திரும்பு FAIL;

    எனில் (xp->xp_numfiles <= 0 && p == வெற்று)
	beep_flush();
    இல்லையெனில் எனில் (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(வெறுமை)ExpandOne(xp, வெற்று, வெற்று, 0, WILD_FREE);

    vim_free(p);

    திரும்பு OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    நிலையான முழு
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	முழு		இலக்கம்_Matches,
	முழு		showtail)
{
    முழு	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, இலக்கம்_Matches);
    எனில் (compl_match_array == வெற்று)
	திரும்பு EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = இலக்கம்_Matches;
    க்கு (முழு i = 0; i < இலக்கம்_Matches; i++)
    {
	compl_match_array[i].pum_text = (xp->xp_files_abbr != வெற்று
				    && xp->xp_files_abbr[i] != வெற்று)
					? xp->xp_files_abbr[i]
					: SHOW_MATCH(i);
	compl_match_array[i].pum_info = xp->xp_files_info != வெற்று
					    ? xp->xp_files_info[i] : வெற்று;
	compl_match_array[i].pum_extra = xp->xp_files_menu != வெற்று
					    ? xp->xp_files_menu[i] : வெற்று;
	compl_match_array[i].pum_kind = xp->xp_files_kind != வெற்று
					    ? xp->xp_files_kind[i] : வெற்று;
	compl_match_array[i].pum_user_abbr_hlattr = -1;
	compl_match_array[i].pum_user_kind_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == வெற்று ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    prefix_len = vim_strsize(xp->xp_pattern);
    எனில் (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    திரும்பு EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    வெறுமை
cmdline_pum_display(வெறுமை)
{
    எனில் (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    முழு
cmdline_pum_active(வெறுமை)
{
    திரும்பு pum_visible() && compl_match_array != வெற்று;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    வெறுமை
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, முழு defer_redraw)
{
    முழு	save_சாவி_Typed = சாவி_Typed;
#ifdef FEAT_EVAL
    முழு	save_RedrawingDisabled = RedrawingDisabled;
    எனில் (cclp->உள்ளீடு_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    எனில் (!defer_redraw)
    {
	முழு save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    இல்லையெனில்
	pum_call_update_screen();
    redrawcmd();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    சாவி_Typed = save_சாவி_Typed;
#ifdef FEAT_EVAL
    எனில் (cclp->உள்ளீடு_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

    வெறுமை
cmdline_pum_cleanup(cmdline_info_T *cclp)
{
    cmdline_pum_remove(cclp, FALSE);
    wildmenu_cleanup(cclp);
}

/*
 * Returns the starting column number to use for the cmdline completion popup
 * menu.
 */
    முழு
cmdline_compl_startcol(வெறுமை)
{
    திரும்பு compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(வெறுமை)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    திரும்பு xp == வெற்று ? வெற்று : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    முழு
cmdline_compl_is_fuzzy(வெறுமை)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    திரும்பு xp != வெற்று && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    நிலையான முழு
skip_status_match_char(expand_T *xp, char_u *s)
{
    எனில் ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	எனில் (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    திரும்பு 2;
#endif
	திரும்பு 1;
    }
    திரும்பு 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    நிலையான முழு
status_match_len(expand_T *xp, char_u *s)
{
    முழு	len = 0;

#ifdef FEAT_MENU
    முழு emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    எனில் (emenu && menu_is_separator(s))
	திரும்பு 1;
#endif

    போது (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    திரும்பு len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    நிலையான வெறுமை
win_redr_status_matches(
    expand_T	*xp,
    முழு		இலக்கம்_matches,
    char_u	**matches,	// list of matches
    முழு		match,
    முழு		showtail)
{
    முழு		row;
    char_u	*buf;
    முழு		len;
    முழு		clen;		// length in screen cells
    முழு		fillchar;
    முழு		attr;
    முழு		i;
    முழு		highlight = TRUE;
    char_u	*selstart = வெற்று;
    முழு		selstart_col = 0;
    char_u	*selend = வெற்று;
    நிலையான முழு	first_match = 0;
    முழு		add_இடது = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    முழு		emenu;
#endif
    முழு		l;

    எனில் (matches == வெற்று)	// interrupted completion?
	திரும்பு;

    எனில் (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    இல்லையெனில்
	buf = alloc(topframe->fr_width + 1);
    எனில் (buf == வெற்று)
	திரும்பு;

    எனில் (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    எனில் (match == 0)
	first_match = 0;
    இல்லையெனில் எனில் (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_இடது = TRUE;
    }
    இல்லையெனில்
    {
	// check if match fits on the screen
	க்கு (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	எனில் (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	எனில் (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    க்கு (i = match; i < இலக்கம்_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		எனில் (clen >= topframe->fr_width)
		    நிறுத்து;
	    }
	    எனில் (i == இலக்கம்_matches)
		add_இடது = TRUE;
	}
    }
    எனில் (add_இடது)
	போது (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    எனில் (clen >= topframe->fr_width)
		நிறுத்து;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    எனில் (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    இல்லையெனில்
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    போது (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	எனில் (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	எனில் (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (முழு)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	இல்லையெனில்
#endif
	க்கு ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    எனில் (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    இல்லையெனில்
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (முழு)STRLEN(buf + len);
	    }
	}
	எனில் (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	எனில் (++i == இலக்கம்_matches)
		நிறுத்து;
    }

    எனில் (i != இலக்கம்_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    எனில் (row >= 0)
    {
	எனில் (wild_menu_showing == 0)
	{
	    எனில் (செய்தி_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		எனில் (cmdline_row == Rows - 1)
		{
		    screen_del_வரிகள்(0, 0, 1, (முழு)Rows, TRUE, 0, வெற்று);
		    ++செய்தி_scrolled;
		}
		இல்லையெனில்
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    இல்லையெனில்
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		எனில் (lastwin->w_status_height == 0)
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
	எனில் (selstart != வெற்று && highlight)
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

#எனில் defined(FEAT_EVAL)
/*
 * Apply -completeopt=escape to a string about to be inserted into the command
 * line as a completion result.  If "str" is non-NULL and the active expansion
 * context is a customlist/custom user command with UCC_ESCAPE set, free "str"
 * and return a newly-allocated copy with spaces, tabs and backslashes prefixed
 * by a backslash.  Otherwise return "str" unchanged.
 */
    நிலையான char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    எனில் (str == வெற்று)
	திரும்பு வெற்று;
    எனில் ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	திரும்பு str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    எனில் (p == வெற்று)
	திரும்பு str;
    vim_free(str);
    திரும்பு p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    நிலையான char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    முழு		srclen,
    முழு		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    எனில் ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	திரும்பு வெற்று;

    buf = alloc(srclen + 1);
    எனில் (buf == வெற்று)
	திரும்பு வெற்று;

    d = buf;
    end = src + srclen;
    க்கு (p = src; p < end; ++p)
    {
	எனில் (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (முழு)(d - buf);
    திரும்பு buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    நிலையான char_u *
get_next_or_prev_match(முழு mode, expand_T *xp)
{
    முழு	    findex = xp->xp_selected;
    முழு	    ht;

    // When no matches found, return NULL
    எனில் (xp->xp_numfiles <= 0)
	திரும்பு வெற்று;

    எனில் (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	எனில் (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    இல்லையெனில் எனில் (mode == WILD_NEXT)
    {
	// Select the next entry
	++findex;
    }
    இல்லையெனில்   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	எனில் (ht > 3)
	    ht -= 2;

	எனில் (mode == WILD_PAGEUP)
	{
	    எனில் (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    இல்லையெனில் எனில் (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    இல்லையெனில்
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	இல்லையெனில்    // mode == WILD_PAGEDOWN
	{
	    எனில் (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    இல்லையெனில் எனில் (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    இல்லையெனில்
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    எனில் (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	எனில் (xp->xp_orig != வெற்று)
	    findex = -1;
	இல்லையெனில்
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    எனில் (p_wmnu)
    {
	எனில் (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	இல்லையெனில் எனில் (vim_strchr(p_wop, WOP_PUM) != வெற்று)
	{
	    எனில் (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	இல்லையெனில்
	    win_redr_status_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    எனில் (findex == -1)
	திரும்பு vim_strsave(xp->xp_orig);
#எனில் defined(FEAT_EVAL)
    திரும்பு apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#இல்லையெனில்
    திரும்பு vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    நிலையான char_u *
ExpandOne_start(முழு mode, expand_T *xp, char_u *str, முழு options)
{
    முழு		non_suf_match;		// number without matching suffix
    முழு		i;
    char_u	*ss = வெற்று;

    // Do the expansion.
    எனில் (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	எனில் (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    இல்லையெனில் எனில் (xp->xp_numfiles == 0)
    {
	எனில் (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    இல்லையெனில்
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	எனில் (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    எனில் (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    இல்லையெனில்
		non_suf_match = 1;
	    எனில் ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		க்கு (i = 0; i < 2; ++i)
		    எனில் (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    எனில் (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		எனில் (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		இல்லையெனில் எனில் (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    எனில் (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    திரும்பு ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    நிலையான char_u *
find_longest_match(expand_T *xp, முழு options)
{
    long_u	len;
    முழு		mb_len = 1;
    முழு		c0, ci;
    முழு		i;
    char_u	*ss;

    க்கு (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	எனில் (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	இல்லையெனில்
	    c0 = xp->xp_files[0][len];
	க்கு (i = 1; i < xp->xp_numfiles; ++i)
	{
	    எனில் (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    இல்லையெனில்
		ci = xp->xp_files[i][len];
	    எனில் (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		எனில் (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் (c0 != ci)
		நிறுத்து;
	}
	எனில் (i < xp->xp_numfiles)
	{
	    எனில் (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    நிறுத்து;
	}
    }

    ss = alloc(len + 1);
    எனில் (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    திரும்பு ss;
}

    வெறுமை
free_xp_files_extra(expand_T *xp, முழு numfiles)
{
    எனில் (xp->xp_files_abbr != வெற்று)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = வெற்று;
    }
    எனில் (xp->xp_files_kind != வெற்று)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = வெற்று;
    }
    எனில் (xp->xp_files_menu != வெற்று)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = வெற்று;
    }
    எனில் (xp->xp_files_info != வெற்று)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = வெற்று;
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
    முழு		options,
    முழு		mode)
{
    char_u	*ss = வெற்று;
    முழு		orig_saved = FALSE;
#எனில் defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    முழு		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    எனில் (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	திரும்பு get_next_or_prev_match(mode, xp);

    எனில் (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    இல்லையெனில் எனில் (mode == WILD_APPLY)
    {
	எனில் (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	இல்லையெனில்
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#எனில் defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    எனில் (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	எனில் (compl_match_array != வெற்று)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    எனில் (mode == WILD_FREE)	// only release file name
	திரும்பு வெற்று;

    எனில் (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#எனில் defined(FEAT_EVAL)
	எனில் (ss != வெற்று)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    எனில் (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#எனில் defined(FEAT_EVAL)
	எனில் (ss != வெற்று)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    எனில் (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	எழுத்து	*prefix = "";
	எழுத்து	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	முழு	n = xp->xp_numfiles - 1;
	முழு	i;
#எனில் defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = வெற்று;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	எனில் ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    எனில் (escaped != வெற்று)
	    {
		க்கு (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    எனில் (escaped[i] == வெற்று)
		    {
			போது (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			நிறுத்து;
		    }
		}
		எனில் (escaped != வெற்று)
		    files = escaped;
	    }
	}
#இல்லையெனில்
	char_u	**files = xp->xp_files;
#endif

	எனில் (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	இல்லையெனில் எனில் (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	க்கு (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	எனில் (ss != வெற்று)
	{
	    size_t  ss_len = 0;

	    க்கு (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (எழுத்து *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (எழுத்து *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#எனில் defined(FEAT_EVAL)
	எனில் (escaped != வெற்று)
	{
	    க்கு (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    எனில் (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    எனில் (!orig_saved)
	vim_free(orig);

#எனில் defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    எனில் (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    திரும்பு ss;
}

/*
 * Prepare an expand structure for use.
 */
    வெறுமை
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
    வெறுமை
ExpandCleanup(expand_T *xp)
{
    எனில் (xp->xp_numfiles >= 0)
    {
	free_xp_files_extra(xp, xp->xp_numfiles);
	FreeWild(xp->xp_numfiles, xp->xp_files);
	xp->xp_numfiles = -1;
    }
    VIM_CLEAR(xp->xp_orig);
}

    வெறுமை
clear_cmdline_orig(வெறுமை)
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
    நிலையான வெறுமை
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	முழு		இலக்கம்_Matches,
	முழு		வரிகள்,
	முழு		linenr,
	முழு		maxlen,
	முழு		showtail,
	முழு		dir_attr)
{
    முழு		i, j;
    முழு		isdir;
    முழு		lastlen;
    char_u	*p;

    lastlen = 999;
    க்கு (j = linenr; j < இலக்கம்_Matches; j += வரிகள்)
    {
	எனில் (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    செய்தி_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    செய்தி_advance(maxlen + 1);
	    செய்தி_puts((எழுத்து *)p);
	    செய்தி_advance(maxlen + 3);
	    செய்தி_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    நிறுத்து;
	}
	க்கு (i = maxlen - lastlen; --i >= 0; )
	    செய்தி_putchar(' ');
	எனில் (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    எனில் (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_path;
		char_u	*path;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_path = expand_env_save_opt(matches[j], TRUE, வெற்று);
		path = exp_path != வெற்று ? exp_path : matches[j];
		halved_slash = backslash_halve_save(path);
		isdir = mch_isdir(halved_slash != வெற்று ? halved_slash
			: matches[j]);
		vim_free(exp_path);
		எனில் (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    இல்லையெனில்
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    எனில் (showtail)
		p = SHOW_MATCH(j);
	    இல்லையெனில்
	    {
		home_replace(வெற்று, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	இல்லையெனில்
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = செய்தி_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    எனில் (செய்தி_col > 0)	// when not wrapped around
    {
	செய்தி_clr_eos();
	செய்தி_putchar('\n');
    }
    out_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    நிலையான முழு
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	முழு		இலக்கம்_Matches,
	முழு		showtail,
	முழு		noselect)
{
    முழு	retval = cmdline_pum_create(ccline, xp, matches, இலக்கம்_Matches, showtail);

    எனில் (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    திரும்பு retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    முழு
showmatches(
    expand_T	*xp,
    முழு		display_wildmenu,
    முழு		display_list,
    முழு		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    முழு		இலக்கம்_Matches;
    char_u	**matches;
    முழு		i;
    முழு		maxlen;
    முழு		வரிகள்;
    முழு		columns;
    முழு		attr;
    முழு		showtail;
    முழு		noselect = (wim_flags_arg & WIM_NOSELECT);
    முழு		noinsert = (wim_flags_arg & WIM_NOINSERT);
    முழு		cmdline_unchanged = noselect || noinsert;

    எனில் (xp->xp_numfiles == -1)
    {
	முழு retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&இலக்கம்_Matches, &matches);
	எனில் (retval != EXPAND_OK)
	    திரும்பு retval;
	showtail = expand_showtail(xp);
    }
    இல்லையெனில்
    {
	இலக்கம்_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    எனில் (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != வெற்று)
	திரும்பு show_pum_matches(ccline, xp, matches, இலக்கம்_Matches,
		showtail && !cmdline_unchanged, noselect);

    எனில் (display_list)
    {
	செய்தி_didany = FALSE;		// lines_left will be set
	செய்தி_start();			// prepare for paging
	செய்தி_putchar('\n');
	out_flush();
	cmdline_row = செய்தி_row;
	செய்தி_didany = FALSE;		// lines_left will be set again
	செய்தி_start();			// prepare for paging
    }

    எனில் (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    இல்லையெனில் எனில் (display_wildmenu && !display_list)
	win_redr_status_matches(xp, இலக்கம்_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    இல்லையெனில் எனில் (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	க்கு (i = 0; i < இலக்கம்_Matches; ++i)
	{
	    முழு	len;
	    எனில் (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(வெற்று, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    இல்லையெனில்
		len = vim_strsize(SHOW_MATCH(i));
	    எனில் (len > maxlen)
		maxlen = len;
	}

	எனில் (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    வரிகள் = இலக்கம்_Matches;
	இல்லையெனில்
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    எனில் (columns < 1)
		columns = 1;
	    வரிகள் = (இலக்கம்_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	எனில் (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    செய்தி_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    செய்தி_clr_eos();
	    செய்தி_advance(maxlen - 3);
	    செய்தி_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	க்கு (i = 0; i < வரிகள்; ++i)
	{
	    showmatches_oneline(xp, matches, இலக்கம்_Matches, வரிகள், i,
						maxlen, showtail, attr);
	    எனில் (got_int)
	    {
		got_int = FALSE;
		நிறுத்து;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = செய்தி_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    எனில் (display_wildmenu && display_list)
    {
	எனில் (vim_strchr(p_wop, WOP_PUM) != வெற்று)
	    (வெறுமை)show_pum_matches(ccline, xp, matches, இலக்கம்_Matches,
		    showtail && !cmdline_unchanged, noselect);
	இல்லையெனில்
	    win_redr_status_matches(xp, இலக்கம்_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    எனில் (xp->xp_numfiles == -1)
    {
	FreeWild(இலக்கம்_Matches, matches);
	free_xp_files_extra(xp, இலக்கம்_Matches);
    }

    திரும்பு EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    நிலையான char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    முழு		had_sep = FALSE;

    க்கு (p = s; *p != NUL; )
    {
	எனில் (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	இல்லையெனில் எனில் (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    திரும்பு t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    நிலையான முழு
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    எனில் (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	திரும்பு FALSE;

    end = gettail(xp->xp_pattern);
    எனில் (end == xp->xp_pattern)		// there is no path separator
	திரும்பு FALSE;

    க்கு (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	எனில் (rem_backslash(s))
	    ++s;
	இல்லையெனில் எனில் (vim_strchr((char_u *)"*?[", *s) != வெற்று)
	    திரும்பு FALSE;
    }
    திரும்பு TRUE;
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
    முழு		len,
    முழு		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    முழு		i, j;
    முழு		new_len;
    char_u	*tail;
    முழு		ends_in_star;

    எனில் (context != EXPAND_FILES
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
	எனில் (context == EXPAND_FINDFUNC
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
	இல்லையெனில்
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    க்கு (i = 0; i < len; i++)
	    {
		எனில் (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		எனில் (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		எனில் ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    எனில் (retval != வெற்று)
	    {
		retval[0] = '^';
		j = 1;
		க்கு (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    எனில் (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			நிறுத்து;

		    தேர்வு (fname[i])
		    {
			நிலை '*':   retval[j++] = '.';
				    நிறுத்து;
			நிலை '~':   retval[j++] = '\\';
				    நிறுத்து;
			நிலை '?':   retval[j] = '.';
				    தொடர்;
			நிலை '.':   எனில் (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    நிறுத்து;
			நிலை '\\':  எனில் (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    நிறுத்து;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    இல்லையெனில்
    {
	retval = alloc(len + 4);
	எனில் (retval != வெற்று)
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
	    க்கு (i = len - 2; i >= 0; --i)
	    {
		எனில் (retval[i] != '\\')
		    நிறுத்து;
		ends_in_star = !ends_in_star;
	    }
#endif
	    எனில் ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == வெற்று
		    && vim_strchr(retval, '`') == வெற்று)
		retval[len++] = '*';
	    இல்லையெனில் எனில் (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    திரும்பு retval;
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
    வெறுமை
set_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    எனில் ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_தேடல்_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_தேடல்_EXTRA
	தேடல்_first_வரி = 0; // Search entire buffer
#endif
	திரும்பு;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    எனில் (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->உள்ளீடு_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	திரும்பு;
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
    நிலையான char_u *
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, முழு *complp)
{
    char_u	*p = வெற்று;
    முழு		len = 0;
    முழு		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    எனில் (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    இல்லையெனில்
    {
	p = cmd;
	போது (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	எனில் (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    போது (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	எனில் (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    போது (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	எனில் (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != வெற்று)
	    ++p;
	len = (முழு)(p - cmd);

	எனில் (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    திரும்பு வெற்று;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	எனில் ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    போது (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    எனில் (*p == NUL && ASCII_ISALNUM(p[-1]))
	திரும்பு வெற்று;

    எனில் (eap->cmdidx == CMD_SIZE)
    {
	எனில் (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != வெற்று)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	இல்லையெனில் எனில் (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, வெற்று, xp, complp);
	    எனில் (p == வெற்று)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    எனில் (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	திரும்பு வெற்று;
    }

    திரும்பு p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    நிலையான வெறுமை
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	முழு		usefilter,
	expand_T	*xp,
	முழு		*complp)
{
    char_u	*p;
    முழு		c;
    முழு		in_quote = FALSE;
    char_u	*bow = வெற்று;	// Beginning of word
    முழு		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    போது (*p != NUL)
    {
	எனில் (has_mbyte)
	    c = mb_ptr2char(p);
	இல்லையெனில்
	    c = *p;
	எனில் (c == '\\' && p[1] != NUL)
	    ++p;
	இல்லையெனில் எனில் (c == '`')
	{
	    எனில் (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	இல்லையெனில் எனில் (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != வெற்று && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    போது (*p != NUL)
	    {
		எனில் (has_mbyte)
		    c = mb_ptr2char(p);
		இல்லையெனில்
		    c = *p;
		எனில் (c == '`' || vim_isfilec_or_wc(c))
		    நிறுத்து;
		எனில் (has_mbyte)
		    len = (*mb_ptr2len)(p);
		இல்லையெனில்
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    எனில் (in_quote)
		bow = p;
	    இல்லையெனில்
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    எனில் (bow != வெற்று && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    எனில் (usefilter
	    || (eap != வெற்று
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	எனில் (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    எனில் (*xp->xp_pattern == '$')
    {
	க்கு (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    எனில் (!vim_isIDc(*p))
		நிறுத்து;
	எனில் (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    எனில் (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    எனில் (*xp->xp_pattern == '~')
    {
	க்கு (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	எனில் (*p == NUL && p > xp->xp_pattern + 1
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
    நிலையான char_u *
set_context_in_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    எனில் (p == வெற்று)
	xp->xp_pattern = arg;
    இல்லையெனில்
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    திரும்பு வெற்று;
}

#ifdef FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    நிலையான char_u *
set_context_in_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    எனில் (p == வெற்று)
	xp->xp_pattern = arg;
    இல்லையெனில்
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    திரும்பு வெற்று;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    நிலையான char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    எனில் (*arg != NUL)
	arg = skip_vimgrep_pat(arg, வெற்று, வெற்று);
    எனில் (arg == வெற்று || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	திரும்பு வெற்று;
    }
    திரும்பு skipwhite(arg);
}

#ifdef FEAT_தேடல்_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    நிலையான char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    எனில் (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	எனில் (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    திரும்பு find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    நிலையான char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    முழு		delim;

    delim = *arg;	    // get the delimiter
    எனில் (delim)
	++arg;		    // skip delimiter if there is one

    போது (arg[0] != NUL && arg[0] != delim)
    {
	எனில் (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    எனில் (arg[0] != NUL)
	திரும்பு arg + 1;

    திரும்பு வெற்று;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    நிலையான char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    முழு		delim;

    delim = *arg;
    எனில் (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	எனில் (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    போது (arg[0] != NUL && arg[0] != delim)
	    {
		எனில் (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    எனில் (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    போது (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == வெற்று)
	++arg;
    எனில் (arg[0] != NUL)
	திரும்பு arg;

    திரும்பு வெற்று;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    நிலையான char_u *
find_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    எனில் (*arg != '/')
	திரும்பு வெற்று;

    // Match regexp, not just whole words
    க்கு (++arg; *arg && *arg != '/'; arg++)
	எனில் (*arg == '\\' && arg[1] != NUL)
	    arg++;
    எனில் (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	எனில் (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == வெற்று)
	    xp->xp_context = EXPAND_NOTHING;
	இல்லையெனில்
	    திரும்பு arg;
    }

    திரும்பு வெற்று;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    நிலையான char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    போது ((xp->xp_pattern = vim_strchr(arg, ' ')) != வெற்று)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    எனில் (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    திரும்பு வெற்று;
}
#endif

#எனில் defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    நிலையான char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    எனில் (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    இல்லையெனில்
    {
	எனில் ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_pattern = skipwhite(p);
	}
	இல்லையெனில்
	    xp->xp_context = EXPAND_NOTHING;
    }

    திரும்பு வெற்று;
}
#endif

நிலையான எண்ணிக்கை
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
நிலையான எண்ணிக்கை
{
    EXP_BREAKPT_ADD,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    நிலையான char_u *
set_context_in_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_start;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_pattern = arg;

    எனில் (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    இல்லையெனில் எனில் (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    இல்லையெனில்
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    எனில் (*p == NUL)
	திரும்பு வெற்று;
    subcmd_start = p;

    எனில் (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	எனில் (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    எனில் (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		திரும்பு வெற்று;
	    }
	    p = skipwhite(p);
	}
	எனில் (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	இல்லையெனில்
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    இல்லையெனில் எனில் (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    திரும்பு வெற்று;
}

    நிலையான char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = வெற்று;

    p = skipwhite(arg);
    எனில் (VIM_ISDIGIT(*p))
	திரும்பு வெற்று;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    திரும்பு வெற்று;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    நிலையான char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    முழு    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    எனில் (*p == NUL)
	திரும்பு வெற்று;

    க்கு (;;)
    {
	எனில் (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    தொடர்;
	}
	எனில் (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    தொடர்;
	}
	நிறுத்து;
    }

    எனில் ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    இல்லையெனில் எனில் ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    இல்லையெனில் எனில் ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    திரும்பு வெற்று;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    நிலையான வெறுமை
set_context_with_pattern(expand_T *xp)
{
    முழு		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_தேடல்_EXTRA
    முழு		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    எனில் (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	திரும்பு;
#endif

    xp->xp_pattern = ccline->cmdbuff + skiplen;
    xp->xp_pattern_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_PATTERN_IN_BUF;
    xp->xp_தேடல்_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    நிலையான char_u *
set_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	long		argt,
	முழு		compl,
	முழு		forceit)
{
    char_u  *nextcmd;

    தேர்வு (cmdidx)
    {
	நிலை CMD_find:
	நிலை CMD_sfind:
	நிலை CMD_tabfind:
	    எனில் (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    நிறுத்து;
	நிலை CMD_cd:
	நிலை CMD_chdir:
	நிலை CMD_tcd:
	நிலை CMD_tchdir:
	நிலை CMD_lcd:
	நிலை CMD_lchdir:
	    எனில் (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    நிறுத்து;
	நிலை CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    நிறுத்து;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	நிலை CMD_aboveleft:
	நிலை CMD_argdo:
	நிலை CMD_belowright:
	நிலை CMD_botright:
	நிலை CMD_browse:
	நிலை CMD_bufdo:
	நிலை CMD_cdo:
	நிலை CMD_cfdo:
	நிலை CMD_confirm:
	நிலை CMD_debug:
	நிலை CMD_folddoclosed:
	நிலை CMD_folddoopen:
	நிலை CMD_hide:
	நிலை CMD_horizontal:
	நிலை CMD_keepalt:
	நிலை CMD_keepjumps:
	நிலை CMD_keepmarks:
	நிலை CMD_keeppatterns:
	நிலை CMD_ldo:
	நிலை CMD_leftabove:
	நிலை CMD_lfdo:
	நிலை CMD_lockmarks:
	நிலை CMD_noautocmd:
	நிலை CMD_noswapfile:
	நிலை CMD_rightbelow:
	நிலை CMD_sandbox:
	நிலை CMD_silent:
	நிலை CMD_tab:
	நிலை CMD_tabdo:
	நிலை CMD_topleft:
	நிலை CMD_unsilent:
	நிலை CMD_verbose:
	நிலை CMD_vertical:
	நிலை CMD_windo:
	நிலை CMD_vim9cmd:
	நிலை CMD_legacy:
	    திரும்பு arg;

	நிலை CMD_filter:
	    திரும்பு set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_தேடல்_EXTRA
	நிலை CMD_match:
	    திரும்பு set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	நிலை CMD_command:
	    திரும்பு set_context_in_user_cmd(xp, arg);

	நிலை CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    நிறுத்து;

	நிலை CMD_global:
	நிலை CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    எனில் (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    திரும்பு nextcmd;

	நிலை CMD_and:
	நிலை CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    எனில் (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    திரும்பு nextcmd;

	நிலை CMD_isearch:
	நிலை CMD_dsearch:
	நிலை CMD_ilist:
	நிலை CMD_dlist:
	நிலை CMD_ijump:
	நிலை CMD_psearch:
	நிலை CMD_djump:
	நிலை CMD_isplit:
	நிலை CMD_dsplit:
	    திரும்பு find_cmd_after_isearch_cmd(xp, arg);
	நிலை CMD_autocmd:
	    திரும்பு set_context_in_autocmd(xp, arg, FALSE);
	நிலை CMD_doautocmd:
	நிலை CMD_doautoall:
	    திரும்பு set_context_in_autocmd(xp, arg, TRUE);
	நிலை CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    நிறுத்து;
	நிலை CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    நிறுத்து;
	நிலை CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    நிறுத்து;
	நிலை CMD_tag:
	நிலை CMD_stag:
	நிலை CMD_ptag:
	நிலை CMD_ltag:
	நிலை CMD_tselect:
	நிலை CMD_stselect:
	நிலை CMD_ptselect:
	நிலை CMD_tjump:
	நிலை CMD_stjump:
	நிலை CMD_ptjump:
	    எனில் (vim_strchr(p_wop, WOP_TAGFILE) != வெற்று)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    இல்லையெனில்
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    நிறுத்து;
	நிலை CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    நிறுத்து;
#ifdef FEAT_SYN_HL
	நிலை CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    நிறுத்து;
#endif
#ifdef FEAT_EVAL
	நிலை CMD_final:
	நிலை CMD_const:
	நிலை CMD_let:
	நிலை CMD_var:
	நிலை CMD_if:
	நிலை CMD_elseif:
	நிலை CMD_while:
	நிலை CMD_for:
	நிலை CMD_echo:
	நிலை CMD_echon:
	நிலை CMD_execute:
	நிலை CMD_echomsg:
	நிலை CMD_echoerr:
	நிலை CMD_echoconsole:
	நிலை CMD_echowindow:
	நிலை CMD_call:
	நிலை CMD_return:
	நிலை CMD_cexpr:
	நிலை CMD_caddexpr:
	நிலை CMD_cgetexpr:
	நிலை CMD_lexpr:
	நிலை CMD_laddexpr:
	நிலை CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    நிறுத்து;

	நிலை CMD_unlet:
	    திரும்பு set_context_in_unlet_cmd(xp, arg);
	நிலை CMD_function:
	நிலை CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    நிறுத்து;
	நிலை CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    நிறுத்து;

	நிலை CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    நிறுத்து;
#endif
	நிலை CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    நிறுத்து;
#ifdef FEAT_CSCOPE
	நிலை CMD_cscope:
	நிலை CMD_lcscope:
	நிலை CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    நிறுத்து;
#endif
#ifdef FEAT_SIGNS
	நிலை CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    நிறுத்து;
#endif
	நிலை CMD_bdelete:
	நிலை CMD_bwipeout:
	நிலை CMD_bunload:
	    போது ((xp->xp_pattern = vim_strchr(arg, ' ')) != வெற்று)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	நிலை CMD_buffer:
	நிலை CMD_sbuffer:
	நிலை CMD_pbuffer:
	நிலை CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    நிறுத்து;
#ifdef FEAT_DIFF
	நிலை CMD_diffget:
	நிலை CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    நிறுத்து;
#endif
	நிலை CMD_USER:
	நிலை CMD_USER_BUF:
	    திரும்பு set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	நிலை CMD_map:	    நிலை CMD_noremap:
	நிலை CMD_nmap:	    நிலை CMD_nnoremap:
	நிலை CMD_vmap:	    நிலை CMD_vnoremap:
	நிலை CMD_omap:	    நிலை CMD_onoremap:
	நிலை CMD_imap:	    நிலை CMD_inoremap:
	நிலை CMD_cmap:	    நிலை CMD_cnoremap:
	நிலை CMD_lmap:	    நிலை CMD_lnoremap:
	நிலை CMD_smap:	    நிலை CMD_snoremap:
	நிலை CMD_tmap:	    நிலை CMD_tnoremap:
	நிலை CMD_xmap:	    நிலை CMD_xnoremap:
	    திரும்பு set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	நிலை CMD_unmap:
	நிலை CMD_nunmap:
	நிலை CMD_vunmap:
	நிலை CMD_ounmap:
	நிலை CMD_iunmap:
	நிலை CMD_cunmap:
	நிலை CMD_lunmap:
	நிலை CMD_sunmap:
	நிலை CMD_tunmap:
	நிலை CMD_xunmap:
	    திரும்பு set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	நிலை CMD_mapclear:
	நிலை CMD_nmapclear:
	நிலை CMD_vmapclear:
	நிலை CMD_omapclear:
	நிலை CMD_imapclear:
	நிலை CMD_cmapclear:
	நிலை CMD_lmapclear:
	நிலை CMD_smapclear:
	நிலை CMD_tmapclear:
	நிலை CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    நிறுத்து;

	நிலை CMD_abbreviate:	நிலை CMD_noreabbrev:
	நிலை CMD_cabbrev:	நிலை CMD_cnoreabbrev:
	நிலை CMD_iabbrev:	நிலை CMD_inoreabbrev:
	    திரும்பு set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	நிலை CMD_unabbreviate:
	நிலை CMD_cunabbrev:
	நிலை CMD_iunabbrev:
	    திரும்பு set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	நிலை CMD_menu:	    நிலை CMD_noremenu:	    நிலை CMD_unmenu:
	நிலை CMD_amenu:	    நிலை CMD_anoremenu:	    நிலை CMD_aunmenu:
	நிலை CMD_nmenu:	    நிலை CMD_nnoremenu:	    நிலை CMD_nunmenu:
	நிலை CMD_vmenu:	    நிலை CMD_vnoremenu:	    நிலை CMD_vunmenu:
	நிலை CMD_omenu:	    நிலை CMD_onoremenu:	    நிலை CMD_ounmenu:
	நிலை CMD_imenu:	    நிலை CMD_inoremenu:	    நிலை CMD_iunmenu:
	நிலை CMD_cmenu:	    நிலை CMD_cnoremenu:	    நிலை CMD_cunmenu:
	நிலை CMD_tlmenu:    நிலை CMD_tlnoremenu:    நிலை CMD_tlunmenu:
	நிலை CMD_tmenu:				    நிலை CMD_tunmenu:
	நிலை CMD_popup:	    நிலை CMD_tearoff:	    நிலை CMD_emenu:
	    திரும்பு set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	நிலை CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    நிறுத்து;

	நிலை CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    நிறுத்து;

	நிலை CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    நிறுத்து;

	நிலை CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    நிறுத்து;

	நிலை CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    நிறுத்து;

	நிலை CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    நிறுத்து;

#எனில் defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	நிலை CMD_language:
	    திரும்பு set_context_in_lang_cmd(xp, arg);
#endif
#எனில் defined(FEAT_PROFILE)
	நிலை CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    நிறுத்து;
#endif
	நிலை CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    நிறுத்து;

	நிலை CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    நிறுத்து;

	நிலை CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    நிறுத்து;

	நிலை CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    நிறுத்து;
#எனில் defined(FEAT_PROFILE)
	நிலை CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    நிறுத்து;
#endif

	நிலை CMD_argdelete:
	    போது ((xp->xp_pattern = vim_strchr(arg, ' ')) != வெற்று)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    நிறுத்து;

#ifdef FEAT_EVAL
	நிலை CMD_breakadd:
	நிலை CMD_profdel:
	நிலை CMD_breakdel:
	    திரும்பு set_context_in_breakadd_cmd(xp, arg, cmdidx);

	நிலை CMD_scriptnames:
	    திரும்பு set_context_in_scriptnames_cmd(xp, arg);
#endif
	நிலை CMD_filetype:
	    திரும்பு set_context_in_filetype_cmd(xp, arg);

	இயல்பு:
	    நிறுத்து;
    }
    திரும்பு வெற்று;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    நிலையான char_u *
set_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    முழு			len = 0;
    exarg_T		ea;
    முழு			compl = EXPAND_NOTHING;
    முழு			forceit = FALSE;
    முழு			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_வரி = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    க்கு (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != வெற்று; cmd++)
	;
    xp->xp_pattern = cmd;

    எனில் (*cmd == NUL)
	திரும்பு வெற்று;
    எனில் (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	திரும்பு வெற்று;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    எனில் (*cmd == NUL)
	திரும்பு வெற்று;
    எனில் (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	திரும்பு வெற்று;
    }

    எனில் (*cmd == '|' || *cmd == '\n')
	திரும்பு cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    எனில் (p == வெற்று)
	திரும்பு வெற்று;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    எனில் (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    எனில் (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    எனில் ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	போது (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    போது (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    எனில் (*p == NUL)
	    {
		எனில் (ea.argt & EX_ARGOPT)
		    திரும்பு set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		எனில் (ea.cmdidx == CMD_terminal)
		    திரும்பு set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    எனில் (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	எனில் (*arg == '>')			// append
	{
	    எனில் (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	இல்லையெனில் எனில் (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    எனில் (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	எனில் (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    எனில் (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	போது (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    எனில் ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	எனில் (*arg == NUL)
	    திரும்பு p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    எனில் ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	எனில் (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	போது (*p)
	{
	    எனில் (*p == Ctrl_V)
	    {
		எனில் (p[1] != NUL)
		    ++p;
	    }
	    இல்லையெனில் எனில் ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		எனில் (*(p - 1) != '\\')
		{
		    எனில் (*p == '|' || *p == '\n')
			திரும்பு p + 1;
		    திரும்பு வெற்று;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    எனில் (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == வெற்று)
	// no arguments allowed but there is something
	திரும்பு வெற்று;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (முழு)STRLEN(buff);
    போது (*p && p < buff + len)
    {
	எனில் (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	இல்லையெனில்
	{
	    எனில் (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    எனில் (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    திரும்பு set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    வெறுமை
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    முழு		len,	    // length of command line (excl. NUL)
    முழு		col,	    // position of cursor
    முழு		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    முழு			context;
#endif
    முழு		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    எனில் (col < len)
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
    எனில் (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    இல்லையெனில் எனில் (use_ccline && ccline->உள்ளீடு_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	எனில் (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(வெற்று, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    இல்லையெனில்
#endif
	போது (nextcomm != வெற்று)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_வரி = str;
    xp->xp_col = col;

    எனில் (col < len)
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
    முழு
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    முழு		col,		// position of cursor
    முழு		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = வெற்று;
    முழு		options = WILD_ADD_SLASH|WILD_SILENT;

    எனில் (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	திரும்பு EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    எனில் (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	திரும்பு EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (முழு)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	முழு	pat_len = xp->xp_pattern_len;
#எனில் defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	எனில் (unesc != வெற்று)
	    pat_src = unesc;
#endif

	எனில் (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	இல்லையெனில்
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#எனில் defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    எனில் (file_str == வெற்று)
	திரும்பு EXPAND_UNSUCCESSFUL;

    எனில் (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    எனில் (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = வெற்று;
    }
    vim_free(file_str);

    திரும்பு EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    நிலையான முழு
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	முழு		*இலக்கம்_Matches,
	முழு		flags,
	முழு		options)
{
    முழு		free_pat = FALSE;
    முழு		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    எனில் (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	எனில் (pat == வெற்று)
	    திரும்பு ret;

	pat_end = pat + pat_len;
	க்கு (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    எனில் (*p != '\\')
		தொடர்;

	    எனில் (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    இல்லையெனில் எனில் (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    இல்லையெனில் எனில் (xp->xp_backslash & XP_BS_COMMA)
	    {
		எனில் (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		இல்லையெனில் எனில் (*(p + 1) == ',')
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

    எனில் (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, இலக்கம்_Matches);
#endif
    }
    இல்லையெனில்
    {
	எனில் (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	இல்லையெனில் எனில் (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	இல்லையெனில் எனில் (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	இல்லையெனில்
	    flags = (flags | EW_DIR) & ~EW_FILE;
	எனில் (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, இலக்கம்_Matches, matches, flags);
    }
    எனில் (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    எனில் (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	முழு j;

	க்கு (j = 0; j < *இலக்கம்_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    போது (*ptr != NUL)
	    {
		எனில் (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		இல்லையெனில் எனில் (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    திரும்பு ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    நிலையான char_u *
get_behave_arg(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx == 0)
	திரும்பு (char_u *)"mswin";
    எனில் (idx == 1)
	திரும்பு (char_u *)"xterm";
    திரும்பு வெற்று;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    நிலையான char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx < 0)
	திரும்பு வெற்று;

    எனில் (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	எழுத்து	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	திரும்பு (char_u *)opts_all[idx];
    }
    எனில் (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	எழுத்து	*opts_plugin[] = {"plugin", "on", "off"};
	திரும்பு (char_u *)opts_plugin[idx];
    }
    எனில் (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	எழுத்து	*opts_indent[] = {"indent", "on", "off"};
	திரும்பு (char_u *)opts_indent[idx];
    }
    எனில் (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	எழுத்து	*opts_onoff[] = {"on", "off"};
	திரும்பு (char_u *)opts_onoff[idx];
    }
    திரும்பு வெற்று;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    நிலையான char_u *
get_breakadd_arg(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx >= 0 && idx <= 3)
    {
	எழுத்து	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	எனில் (breakpt_expand_what == EXP_BREAKPT_ADD)
	    திரும்பு (char_u *)opts[idx];
	இல்லையெனில் எனில் (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    எனில் (idx <= 2)
		திரும்பு (char_u *)opts[idx + 1];
	}
	இல்லையெனில்
	{
	    // profdel {func, file}
	    எனில் (idx <= 1)
		திரும்பு (char_u *)opts[idx + 1];
	}
    }
    திரும்பு வெற்று;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    நிலையான char_u *
get_scriptnames_arg(expand_T *xp UNUSED, முழு idx)
{
    scriptitem_T *si;

    எனில் (!SCRIPT_ID_VALID(idx + 1))
	திரும்பு வெற்று;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(வெற்று, si->sn_name, NameBuff, MAXPATHL, TRUE);
    திரும்பு NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    நிலையான char_u *
get_retab_arg(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx == 0)
	திரும்பு (char_u *)"-indentonly";
    திரும்பு வெற்று;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    நிலையான char_u *
get_messages_arg(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx == 0)
	திரும்பு (char_u *)"clear";
    திரும்பு வெற்று;
}

    நிலையான char_u *
get_mapclear_arg(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx == 0)
	திரும்பு (char_u *)"<buffer>";
    திரும்பு வெற்று;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    நிலையான முழு
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	முழு		*இலக்கம்_Matches)
{
    நிலையான கட்டமைப்பு expgen
    {
	முழு		context;
	char_u	*((*func)(expand_T *, முழு));
	முழு		ic;
	முழு		escaped;
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
	{EXPAND_EXPRESSION, get_கோவை_name, FALSE, TRUE},
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
#எனில் defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
    முழு	i;
    முழு ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(tab); ++i)
    {
	எனில் (xp->xp_context == tab[i].context)
	{
	    எனில் (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, இலக்கம்_Matches,
						tab[i].func, tab[i].escaped);
	    நிறுத்து;
	}
    }

    திரும்பு ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    நிலையான முழு
map_wildopts_to_ewflags(முழு options)
{
    முழு		flags;

    flags = EW_DIR;	// include directories
    எனில் (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    எனில் (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    எனில் (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    எனில் (options & WILD_SILENT)
	flags |= EW_SILENT;
    எனில் (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    எனில் (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    திரும்பு flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    நிலையான முழு
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    முழு		*இலக்கம்_Matches,
    முழு		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    முழு		ret;
    முழு		flags;
    char_u	*tofree = வெற்று;
    முழு		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    எனில் (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	திரும்பு expand_files_and_dirs(xp, pat, matches, இலக்கம்_Matches, flags,
								options);

    *matches = (char_u **)"";
    *இலக்கம்_Matches = 0;
    எனில் (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	எனில் (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					இலக்கம்_Matches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*இலக்கம்_Matches, *matches);
#endif
	    திரும்பு OK;
	}
	திரும்பு FAIL;
    }

    எனில் (xp->xp_context == EXPAND_SHELLCMD)
	திரும்பு expand_shellcmd(pat, matches, இலக்கம்_Matches, flags);
    எனில் (xp->xp_context == EXPAND_OLD_SETTING)
	திரும்பு ExpandOldSetting(இலக்கம்_Matches, matches);
    எனில் (xp->xp_context == EXPAND_BUFFERS)
	திரும்பு ExpandBufnames(pat, இலக்கம்_Matches, matches, options);
#ifdef FEAT_DIFF
    எனில் (xp->xp_context == EXPAND_DIFF_BUFFERS)
	திரும்பு ExpandBufnames(pat, இலக்கம்_Matches, matches,
						options | BUF_DIFF_FILTER);
#endif
    எனில் (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	திரும்பு expand_tags(xp->xp_context == EXPAND_TAGS, pat, இலக்கம்_Matches,
								matches);
    எனில் (xp->xp_context == EXPAND_COLORS)
    {
	எழுத்து *directories[] = {"colors", வெற்று};
	திரும்பு ExpandRTDir(pat, DIP_START + DIP_OPT, இலக்கம்_Matches, matches,
								directories);
    }
    எனில் (xp->xp_context == EXPAND_COMPILER)
    {
	எழுத்து *directories[] = {"compiler", வெற்று};
	திரும்பு ExpandRTDir(pat, 0, இலக்கம்_Matches, matches, directories);
    }
    எனில் (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	எழுத்து *directories[] = {"syntax", வெற்று};
	திரும்பு ExpandRTDir(pat, 0, இலக்கம்_Matches, matches, directories);
    }
    எனில் (xp->xp_context == EXPAND_FILETYPE)
    {
	எழுத்து *directories[] = {"syntax", "indent", "ftplugin", வெற்று};
	திரும்பு ExpandRTDir(pat, 0, இலக்கம்_Matches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    எனில் (xp->xp_context == EXPAND_KEYMAP)
    {
	எழுத்து *directories[] = {"keymap", வெற்று};
	திரும்பு ExpandRTDir(pat, 0, இலக்கம்_Matches, matches, directories);
    }
#endif
#எனில் defined(FEAT_EVAL)
    எனில் (xp->xp_context == EXPAND_USER_LIST)
	திரும்பு ExpandUserList(xp, matches, இலக்கம்_Matches);
#endif
    எனில் (xp->xp_context == EXPAND_PACKADD)
	திரும்பு ExpandPackAddDir(pat, இலக்கம்_Matches, matches);
    எனில் (xp->xp_context == EXPAND_RUNTIME)
	திரும்பு expand_runtime_cmd(pat, இலக்கம்_Matches, matches);
    எனில் (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	திரும்பு expand_pattern_in_buf(pat, xp->xp_தேடல்_dir,
		matches, இலக்கம்_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    எனில் ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	முழு len = (முழு)STRLEN(pat) + 20;

	tofree = alloc(len);
	எனில் (tofree == வெற்று)
	    திரும்பு FAIL;
	vim_snprintf((எழுத்து *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    எனில் (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	எனில் (regmatch.regprog == வெற்று)
	{
	    vim_free(tofree);
	    திரும்பு FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    எனில் (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, இலக்கம்_Matches, matches, fuzzy);
    இல்லையெனில் எனில் (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, இலக்கம்_Matches, matches);
    இல்லையெனில் எனில் (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, இலக்கம்_Matches, matches);
    இல்லையெனில் எனில் (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, இலக்கம்_Matches, matches);
    இல்லையெனில் எனில் (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, இலக்கம்_Matches);
    இல்லையெனில் எனில் (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, இலக்கம்_Matches);
#எனில் defined(FEAT_TERMINAL)
    இல்லையெனில் எனில் (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, இலக்கம்_Matches);
#endif
#எனில் defined(FEAT_EVAL)
    இல்லையெனில் எனில் (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, இலக்கம்_Matches);
#endif
    இல்லையெனில்
	ret = ExpandOther(pat, xp, &regmatch, matches, இலக்கம்_Matches);

    எனில் (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    திரும்பு ret;
}

    முழு
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    முழு		*இலக்கம்_Matches,
    char_u	*(*func)(expand_T *, முழு),
					  // returns a string from the list
    முழு		escaped)
{
    திரும்பு ExpandGenericExt(
	pat, xp, regmatch, matches, இலக்கம்_Matches, func, escaped, 0);
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
    முழு
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    முழு		*இலக்கம்_Matches,
    char_u	*(*func)(expand_T *, முழு),
					  // returns a string from the list
    முழு		escaped,
    முழு		sortStartIdx)
{
    முழு		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = வெற்று;
    முழு		score = 0;
    முழு		fuzzy;
    முழு		match;
    முழு		sort_matches = FALSE;
    முழு		funcsort = FALSE;
    முழு		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = வெற்று;
    *இலக்கம்_Matches = 0;

    எனில் (!fuzzy)
	ga_init2(&ga, அளவு(எழுத்து *), 30);
    இல்லையெனில்
	ga_init2(&ga, அளவு(fuzmatch_str_T), 30);

    க்கு (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	எனில் (str == வெற்று)	    // end of list
	    நிறுத்து;
	எனில் (*str == NUL)	    // skip empty strings
	    தொடர்;

	எனில் (xp->xp_pattern[0] != NUL)
	{
	    எனில் (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    இல்லையெனில்
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	இல்லையெனில்
	    match = TRUE;

	எனில் (!match)
	    தொடர்;

	எனில் (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	இல்லையெனில்
	    str = vim_strsave(str);
	எனில் (str == வெற்று)
	{
	    எனில் (!fuzzy)
	    {
		ga_clear_strings(&ga);
		திரும்பு FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    திரும்பு FAIL;
	}

	எனில் (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    நிறுத்து;
	}

	எனில் (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	இல்லையெனில்
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

#ifdef FEAT_MENU
	எனில் (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    எனில் (*str == '\001')
		*str = '.';
	}
#endif

	எனில் (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    எனில் (ga.ga_len == 0)
	திரும்பு OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    எனில் (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    எனில் (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    எனில் (sort_matches && sortStartMatchIdx != -1)
    {
	எனில் (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((வெறுமை *)ga.ga_data, (size_t)ga.ga_len, அளவு(char_u *),
							   sort_func_compare);
	இல்லையெனில்
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    எனில் (!fuzzy)
    {
	*matches = ga.ga_data;
	*இலக்கம்_Matches = ga.ga_len;
    }
    இல்லையெனில்
    {
	எனில் (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    திரும்பு FAIL;
	*இலக்கம்_Matches = ga.ga_len;
    }

#எனில் defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    திரும்பு OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    நிலையான வெறுமை
expand_shellcmd_onedir(
	char_u		*pathed_pattern,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	முழு		*இலக்கம்_Matches,
	முழு		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    எனில் (expand_wildcards(1, &pathed_pattern, இலக்கம்_Matches, matches, flags) != OK)
	திரும்பு;

    எனில் (ga_grow(gap, *இலக்கம்_Matches) == FAIL)
    {
	FreeWild(*இலக்கம்_Matches, *matches);
	திரும்பு;
    }

    க்கு (முழு i = 0; i < *இலக்கம்_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	எனில் (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    எனில் (HASHITEM_EMPTY(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_data)[gap->ga_len++] = name;
		hash_add_item(ht, hi, name, hash);
		name = வெற்று;
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
    நிலையான முழு
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    முழு		*இலக்கம்_Matches,	// return: number of matches
    முழு		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = வெற்று;
    முழு		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    முழு		flags = flagsarg;
    முழு		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    எனில் (buf == வெற்று)
	திரும்பு FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    எனில் (pat == வெற்று)
    {
	vim_free(buf);
	திரும்பு FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    க்கு (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	எனில் (*s != '\\')
	    தொடர்;

	p = s + 1;
	எனில் (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    எனில் (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    இல்லையெனில்
    {
	// For an absolute name we don't use $PATH.
	எனில் (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	எனில் (path == வெற்று)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, அளவு(எழுத்து *), 10);
    hash_init(&found_ht);
    க்கு (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	எனில் (*s == NUL)
	{
	    எனில் (did_curdir)
		நிறுத்து;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	இல்லையெனில்
	{
#எனில் defined(MSWIN)
	    e = vim_strchr(s, ';');
#இல்லையெனில்
	    e = vim_strchr(s, ':');
#endif
	    எனில் (e == வெற்று)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    எனில் (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    இல்லையெனில்
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? அளவு(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	எனில் (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    எனில் (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		எனில் (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, இலக்கம்_Matches, flags,
				    &found_ht, &ga);
	}

	எனில் (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_data;
    *இலக்கம்_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    எனில் (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    திரும்பு OK;
}

#எனில் defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    நிலையான வெறுமை *
call_user_expand_func(
    வெறுமை	*(*user_expand_func)(char_u *, முழு, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    முழு		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = வெற்று;
    வெறுமை	*ret;

    எனில் (xp->xp_arg == வெற்று || xp->xp_arg[0] == '\0' || xp->xp_வரி == வெற்று)
	திரும்பு வெற்று;

    எனில் (ccline->cmdbuff != வெற்று)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	முழு unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	எனில் (pat == வெற்று)
	    pat = vim_strnsave(xp->xp_pattern, xp->xp_pattern_len);
    }

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = xp->xp_வரி;
    args[2].v_type = VAR_எண்;
    args[2].vval.v_எண் = xp->xp_col;
    args[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, args);

    current_sctx = save_current_sctx;
    எனில் (ccline->cmdbuff != வெற்று)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    திரும்பு ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    நிலையான முழு
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    முழு		*இலக்கம்_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    முழு		keep;
    garray_T	ga;
    முழு		fuzzy;
    முழு		match;
    முழு		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = வெற்று;
    *இலக்கம்_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    எனில் (retstr == வெற்று)
	திரும்பு FAIL;

    எனில் (!fuzzy)
	ga_init2(&ga, அளவு(எழுத்து *), 3);
    இல்லையெனில்
	ga_init2(&ga, அளவு(fuzmatch_str_T), 3);

    க்கு (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	எனில் (e == வெற்று)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	எனில் (xp->xp_pattern[0] != NUL)
	{
	    எனில் (!fuzzy)
		match = vim_regexec(regmatch, s, (colnr_T)0);
	    இல்லையெனில்
	    {
		score = fuzzy_match_str(s, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	இல்லையெனில்
	    match = TRUE;		// match everything

	*e = keep;

	எனில் (match)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    எனில் (p == வெற்று)
		நிறுத்து;

	    எனில் (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		நிறுத்து;
	    }

	    எனில் (!fuzzy)
		((char_u **)ga.ga_data)[ga.ga_len] = p;
	    இல்லையெனில்
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	எனில் (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    எனில் (ga.ga_len == 0)
	திரும்பு OK;

    எனில் (!fuzzy)
    {
	*matches = ga.ga_data;
	*இலக்கம்_Matches = ga.ga_len;
    }
    இல்லையெனில்
    {
	எனில் (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    திரும்பு FAIL;
	*இலக்கம்_Matches = ga.ga_len;
    }
    திரும்பு OK;
}

    வெறுமை
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    முழு		*இலக்கம்_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    முழு		have_extra = FALSE;
    முழு		i;

    ga_init2(&ga, அளவு(எழுத்து *), 3);
    ga_init2(&ga_abbr, அளவு(எழுத்து *), 3);
    ga_init2(&ga_kind, அளவு(எழுத்து *), 3);
    ga_init2(&ga_menu, அளவு(எழுத்து *), 3);
    ga_init2(&ga_info, அளவு(எழுத்து *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_பொருட்கள்(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = வெற்று;
	char_u	*abbr = வெற்று;
	char_u	*kind = வெற்று;
	char_u	*menu = வெற்று;
	char_u	*info = வெற்று;

	எனில் (tv->v_type == VAR_STRING)
	{
	    எனில் (tv->vval.v_string == வெற்று)
		தொடர்;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	இல்லையெனில் எனில் (tv->v_type == VAR_DICT && tv->vval.v_dict != வெற்று)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    எனில் (word == வெற்று)
		தொடர்;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    எனில் (abbr != வெற்று || kind != வெற்று || menu != வெற்று || info != வெற்று)
		have_extra = TRUE;
	}
	இல்லையெனில்
	    தொடர்;  // Skip other types

	எனில் (p == வெற்று
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
	    நிறுத்து;
	}

	((char_u **)ga.ga_data)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_data)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_data)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_data)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_data)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_data;
    *இலக்கம்_Matches = ga.ga_len;
    எனில் (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    இல்லையெனில்
    {
	// No extra info collected; free the placeholder NULL entries.
	க்கு (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	க்கு (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	க்கு (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	க்கு (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    நிலையான முழு
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    முழு		*இலக்கம்_Matches)
{
    list_T      *retlist;

    *matches = வெற்று;
    *இலக்கம்_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    எனில் (retlist == வெற்று)
	திரும்பு FAIL;

    expand_process_user_list(retlist, matches, இலக்கம்_Matches, xp);
    list_unref(retlist);
    திரும்பு OK;
}
#endif

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    வெறுமை
globpath(
    char_u	*path,
    char_u	*file,
    garray_T	*ga,
    முழு		expand_options,
    முழு		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    எனில் (buf == வெற்று)
	திரும்பு;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#எனில் defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#இல்லையெனில்
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    போது (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	எனில் (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    முழு	    இலக்கம்_p;
	    char_u  **p;

	    எனில் (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    எனில் (ExpandFromContext(&xpc, buf, &p, &இலக்கம்_p,
			     WILD_SILENT|expand_options) != FAIL && இலக்கம்_p > 0)
	    {
		ExpandEscape(&xpc, buf, இலக்கம்_p, p, WILD_SILENT|expand_options);

		எனில் (ga_grow(ga, இலக்கம்_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    க்கு (முழு i = 0; i < இலக்கம்_p; ++i)
		    {
			((char_u **)ga->ga_data)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		இல்லையெனில்
		{
		    FreeWild(இலக்கம்_p, p);
		    p = வெற்று;
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
    முழு
wildmenu_translate_சாவி(
	cmdline_info_T	*cclp,
	முழு		சாவி,
	expand_T	*xp,
	முழு		did_wild_list)
{
    முழு c = சாவி;

    எனில் (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	தேர்வு (c)
	{
	    நிலை K_UP:	  c = K_இடது; நிறுத்து;
	    நிலை K_DOWN:  c = K_வலது; நிறுத்து;
	    நிலை K_இடது:  c = K_UP; நிறுத்து;
	    நிலை K_வலது: c = K_DOWN; நிறுத்து;
	    இயல்பு:	  நிறுத்து;
	}
    }

    எனில் (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	எனில் (c == K_இடது)
	    c = Ctrl_P;
	இல்லையெனில் எனில் (c == K_வலது)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    எனில் (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    திரும்பு c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    நிலையான வெறுமை
cmdline_del(cmdline_info_T *cclp, முழு from)
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
    நிலையான முழு
wildmenu_process_சாவி_menunames(cmdline_info_T *cclp, முழு சாவி, expand_T *xp)
{
    முழு		i;
    முழு		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    எனில் (சாவி == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	சாவி = p_wc;
	சாவி_Typed = TRUE;  // in case the key was mapped
    }
    இல்லையெனில் எனில் (சாவி == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	முழு found = FALSE;

	j = (முழு)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	போது (--j > 0)
	{
	    // check for start of menu name
	    எனில் (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		நிறுத்து;
	    }
	    // check for start of submenu name
	    எனில் (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		எனில் (found)
		{
		    i = j + 1;
		    நிறுத்து;
		}
		இல்லையெனில்
		    found = TRUE;
	    }
	}
	எனில் (i > 0)
	    cmdline_del(cclp, i);
	சாவி = p_wc;
	சாவி_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    திரும்பு சாவி;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    நிலையான முழு
wildmenu_process_சாவி_filenames(cmdline_info_T *cclp, முழு சாவி, expand_T *xp)
{
    முழு		i;
    முழு		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    எனில் (சாவி == K_DOWN
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	சாவி = p_wc;
	சாவி_Typed = TRUE;  // in case the key was mapped
    }
    இல்லையெனில் எனில் (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && சாவி == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	முழு found = FALSE;

	j = cclp->cmdpos;
	i = (முழு)(xp->xp_pattern - cclp->cmdbuff);
	போது (--j > i)
	{
	    எனில் (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    எனில் (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		நிறுத்து;
	    }
	}
	எனில் (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    சாவி = p_wc;
	    சாவி_Typed = TRUE;  // in case the key was mapped
	}
    }
    இல்லையெனில் எனில் (சாவி == K_UP)
    {
	// go up a directory
	முழு found = FALSE;

	j = cclp->cmdpos - 1;
	i = (முழு)(xp->xp_pattern - cclp->cmdbuff);
	போது (--j > i)
	{
	    எனில் (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    எனில் (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == வெற்று
#endif
	       )
	    {
		எனில் (found)
		{
		    i = j + 1;
		    நிறுத்து;
		}
		இல்லையெனில்
		    found = TRUE;
	    }
	}

	எனில் (!found)
	    j = i;
	இல்லையெனில் எனில் (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	இல்லையெனில் எனில் (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	இல்லையெனில்
	    j = 0;
	எனில் (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	இல்லையெனில் எனில் (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	சாவி = p_wc;
	சாவி_Typed = TRUE;
    }

    திரும்பு சாவி;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    முழு
wildmenu_process_சாவி(cmdline_info_T *cclp, முழு சாவி, expand_T *xp)
{
    எனில் (xp->xp_context == EXPAND_MENUNAMES)
	திரும்பு wildmenu_process_சாவி_menunames(cclp, சாவி, xp);
    இல்லையெனில் எனில் ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	திரும்பு wildmenu_process_சாவி_filenames(cclp, சாவி, xp);

    திரும்பு சாவி;
}

/*
 * Free expanded names when finished walking through the matches
 */
    வெறுமை
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    முழு skt = சாவி_Typed;

    எனில் (!p_wmnu || wild_menu_showing == 0)
	திரும்பு;

#ifdef FEAT_EVAL
    முழு save_RedrawingDisabled = RedrawingDisabled;
    எனில் (cclp->உள்ளீடு_fn)
	RedrawingDisabled = 0;
#endif

#எனில் defined(FEAT_தேடல்_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    எனில் (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    இல்லையெனில் எனில் (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	last_status(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    இல்லையெனில்
    {
	win_redraw_last_status(topframe);
	redraw_statuslines();
    }
    சாவி_Typed = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    எனில் (cclp->உள்ளீடு_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#எனில் defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    வெறுமை
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    முழு		filtered = FALSE;
    முழு		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	திரும்பு;

    pat = tv_get_string(&argvars[0]);
    எனில் (check_for_string_arg(argvars, 1) == FAIL)
	திரும்பு;
    type = tv_get_string(&argvars[1]);

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], வெற்று);

    எனில் (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    எனில் (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    எனில் (STRCMP(type, "cmdline") == 0)
    {
	முழு cmdline_len = (முழு)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (முழு)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    இல்லையெனில்
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (முழு)STRLEN(xpc.xp_pattern);
	xpc.xp_வரி = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	தேர்வு (xpc.xp_context)
	{
	நிலை EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    திரும்பு;

	நிலை EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    எனில் (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		திரும்பு;
	    }

	    xpc.xp_arg = type + 7;
	    நிறுத்து;

	நிலை EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    எனில் (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		திரும்பு;
	    }

	    xpc.xp_arg = type + 11;
	    நிறுத்து;

# எனில் defined(FEAT_MENU)
	நிலை EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (முழு)(xpc.xp_pattern - pattern_start);
	    நிறுத்து;
# endif

# ifdef FEAT_CSCOPE
	நிலை EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (முழு)(xpc.xp_pattern - pattern_start);
	    நிறுத்து;
# endif

# ifdef FEAT_SIGNS
	நிலை EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (முழு)(xpc.xp_pattern - pattern_start);
	    நிறுத்து;
# endif

	நிலை EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (முழு)(xpc.xp_pattern - pattern_start);
	    நிறுத்து;

	நிலை EXPAND_SHELLCMDLINE:
	{
	    முழு context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(வெற்று, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (முழு)(xpc.xp_pattern - pattern_start);
	    நிறுத்து;
	}

	நிலை EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    நிறுத்து;

	நிலை EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    நிறுத்து;

	இயல்பு:
	    நிறுத்து;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	முழு	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	எனில் (unesc != வெற்று)
	    pat_src = unesc;

	எனில் (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	இல்லையெனில்
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    எனில் (rettv_list_alloc(rettv) == OK && pat != வெற்று)
    {
	முழு	i;

	ExpandOne(&xpc, pat, வெற்று, options, WILD_ALL_KEEP);

	க்கு (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    வெறுமை
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    முழு		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (முழு)STRLEN(pat);
    set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_to_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    வெறுமை
f_cmdcomplete_info(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    முழு		    idx;
    முழு		    ret = OK;

    எனில் (rettv_dict_alloc(rettv) == FAIL || ccline == வெற்று
	    || ccline->xpc == வெற்று || ccline->xpc->xp_files == வெற்று)
	திரும்பு;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (முழு)cmdline_orig.length);
    எனில் (ret == OK)
	ret = dict_add_எண்(retdict, "pum_visible", pum_visible());
    எனில் (ret == OK)
	ret = dict_add_எண்(retdict, "selected", ccline->xpc->xp_selected);
    எனில் (ret == OK)
    {
	li = list_alloc();
	எனில் (li == வெற்று)
	    திரும்பு;
	ret = dict_add_list(retdict, "matches", li);
	எனில் (ret == FAIL)
	    list_unref(li);
	க்கு (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    நிலையான முழு
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*வரி, *start_வரி, *end_வரி;
    முழு		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    முழு		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != வெற்று;

    எனில் (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	திரும்பு FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_வரி = ml_get(start->lnum);
    char_u  *start_ptr = start_வரி + start->col;
    முழு	    is_single_வரி = start->lnum == end->lnum;

    segment_len = is_single_வரி ? (end->col - start->col)
			: (முழு)(ml_get_len(start->lnum) - start->col);
    எனில் (ga_grow(&ga, segment_len + 2) != OK)
	செல் fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    எனில் (!is_single_வரி)
    {
	எனில் (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	இல்லையெனில்
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    எனில் (!is_single_வரி)
    {
	க்கு (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    முழு  linelen;

	    வரி = ml_get(lnum);
	    linelen = (முழு)ml_get_len(lnum);
	    எனில் (ga_grow(&ga, linelen + 2) != OK)
		செல் fail;
	    ga_concat_len(&ga, வரி, linelen);
	    எனில் (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    இல்லையெனில்
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_வரி = ml_get(end->lnum);
    word_end = find_word_end(end_வரி + end->col);
    segment_len = (முழு)(word_end - end_வரி);
    எனில் (ga_grow(&ga, segment_len) != OK)
	செல் fail;
    ga_concat_len(&ga, end_வரி + (is_single_வரி ? end->col : 0),
	    segment_len - (is_single_வரி ? end->col : 0));

    // Null-terminate
    எனில் (ga_grow(&ga, 1) != OK)
	செல் fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    திரும்பு OK;

fail:
    ga_clear(&ga);
    திரும்பு FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    நிலையான முழு
is_regex_match(char_u *pat, char_u *str)
{
    எனில் (STRCMP(pat, str) == 0)
	திரும்பு TRUE;

    regmatch_T	regmatch;
    முழு		result;

    ++emsg_off;
    ++செய்தி_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --செய்தி_silent;

    எனில் (regmatch.regprog == வெற்று)
	திரும்பு FALSE;
    regmatch.rm_ic = p_ic;
    எனில் (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++செய்தி_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --செய்தி_silent;

    vim_regfree(regmatch.regprog);
    திரும்பு result;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    நிலையான char_u *
concat_pattern_with_buffer_match(
	char_u *pat,
	முழு pat_len,
	pos_T *end_match_pos,
	முழு lowercase UNUSED)
{
    char_u  *வரி = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(வரி + end_match_pos->col);
    முழு	    match_len = (முழு)(word_end - (வரி + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    எனில் (match == வெற்று)
	திரும்பு வெற்று;
    mch_memmove(match, pat, pat_len);
    எனில் (match_len > 0)
    {
#எனில் defined(FEAT_EVAL) || defined(FEAT_SPELL)
	எனில் (lowercase)
	{
	    char_u  *mword = vim_strnsave(வரி + end_match_pos->col,
		    match_len);
	    எனில் (mword == வெற்று)
		செல் cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    எனில் (lower == வெற்று)
		செல் cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	இல்லையெனில்
#endif
	    mch_memmove(match + pat_len, வரி + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    திரும்பு match;

#எனில் defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    திரும்பு வெற்று;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    நிலையான முழு
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    முழு		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    முழு		*இலக்கம்_Matches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    முழு		found_new_match;
    முழு		looped_around = FALSE;
    முழு		pat_len;
    முழு		has_range = FALSE;
    முழு		compl_started = FALSE;
    முழு		தேடல்_flags;
    char_u	*match, *full_match;
    முழு		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != வெற்று;

#ifdef FEAT_தேடல்_EXTRA
    has_range = தேடல்_first_வரி != 0;
#endif

    *matches = வெற்று;
    *இலக்கம்_Matches = 0;

    எனில் (pat == வெற்று || *pat == NUL)
	திரும்பு FAIL;

    pat_len = (முழு)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_தேடல்_EXTRA
    எனில் (has_range)
	cur_match_pos.lnum = தேடல்_first_வரி;
    இல்லையெனில்
#endif
	cur_match_pos = pre_incsearch_pos;

    தேடல்_flags = தேடல்_OPT | தேடல்_NOOF | தேடல்_PEEK | தேடல்_NFMSG
	| (has_range ? தேடல்_START : 0);

    ga_init2(&ga, அளவு(char_u *), 10); // Use growable array of char_u*

    க்கு (;;)
    {
	++emsg_off;
	++செய்தி_silent;
	found_new_match = searchit(வெற்று, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		தேடல்_flags, RE_LAST, வெற்று);
	--செய்தி_silent;
	--emsg_off;

	எனில் (found_new_match == FAIL)
	    நிறுத்து;

#ifdef FEAT_தேடல்_EXTRA
	// If in range mode, check if match is within the range
	எனில் (has_range && (cur_match_pos.lnum < தேடல்_first_வரி
		    || cur_match_pos.lnum > தேடல்_last_வரி))
		நிறுத்து;
#endif

	எனில் (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    எனில் ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		எனில் (looped_around)
		    நிறுத்து;
		இல்லையெனில்
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	எனில் (char_avail() || got_int)
	{
	    எனில் (got_int)
	    {
		(வெறுமை)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    செல் cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	எனில் (end_match_pos.lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    தொடர்;
	}

	// Extract the matching text prepended to completed word
	எனில் (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    நிறுத்து;

	எனில் (exacttext)
	    match = full_match;
	இல்லையெனில்
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    எனில் (match == வெற்று || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		எனில் (match == வெற்று || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    தொடர்;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	க்கு (முழு i = 0; i < ga.ga_len; ++i)
	{
	    எனில் (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		நிறுத்து;
	    }
	}
	எனில் (match != வெற்று)
	{
	    எனில் (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		செல் cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    எனில் (ga.ga_len > TAG_MANY)
		நிறுத்து;
	}
	எனில் (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *இலக்கம்_Matches = ga.ga_len;
    திரும்பு OK;

cleanup:
    ga_clear_strings(&ga);
    திரும்பு FAIL;
}
