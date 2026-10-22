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

#અન્તર્ભાવ "vim.h"

સ્થિર પૂર્ણાંક	cmd_showtail;	// Only show path tail in lists ?
સ્થિર પૂર્ણાંક	may_expand_pattern = FALSE;
સ્થિર pos_T	pre_incsearch_pos; // Cursor position when incsearch started

સ્થિર શૂન્ય	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, પૂર્ણાંક usefilter, expand_T *xp, પૂર્ણાંક *complp);
સ્થિર પૂર્ણાંક	ExpandFromContext(expand_T *xp, char_u *, char_u ***, પૂર્ણાંક *, પૂર્ણાંક);
સ્થિર char_u	*showmatches_gettail(char_u *s);
સ્થિર પૂર્ણાંક	expand_showtail(expand_T *xp);
સ્થિર પૂર્ણાંક	expand_shellcmd(char_u *filepat, char_u ***matches, પૂર્ણાંક *અંક_Matches, પૂર્ણાંક flagsarg);
#જો defined(FEAT_EVAL)
સ્થિર પૂર્ણાંક	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, પૂર્ણાંક *અંક_Matches);
સ્થિર પૂર્ણાંક	ExpandUserList(expand_T *xp, char_u ***matches, પૂર્ણાંક *અંક_Matches);
સ્થિર char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
સ્થિર char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, પૂર્ણાંક srclen, પૂર્ણાંક *new_lenp);
#endif
સ્થિર પૂર્ણાંક	expand_pattern_in_buf(char_u *pat, પૂર્ણાંક dir, char_u ***matches, પૂર્ણાંક *અંક_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
સ્થિર pumitem_T *compl_match_array = ખાલી;
સ્થિર પૂર્ણાંક compl_match_arraysize;
// First column in cmdline of the matched item for completion.
સ્થિર પૂર્ણાંક compl_startcol;
સ્થિર પૂર્ણાંક compl_selected;
// cmdline before expansion
સ્થિર string_T cmdline_orig = {ખાલી, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    સ્થિર પૂર્ણાંક
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    પસંદગી (xp->xp_context)
    {
    સ્થિતિ EXPAND_BOOL_SETTINGS:
    સ્થિતિ EXPAND_COLORS:
    સ્થિતિ EXPAND_COMPILER:
    સ્થિતિ EXPAND_DIRECTORIES:
    સ્થિતિ EXPAND_DIRS_IN_CDPATH:
    સ્થિતિ EXPAND_FILES:
    સ્થિતિ EXPAND_FILES_IN_PATH:
    સ્થિતિ EXPAND_FILETYPE:
    સ્થિતિ EXPAND_FILETYPECMD:
    સ્થિતિ EXPAND_HELP:
    સ્થિતિ EXPAND_KEYMAP:
    સ્થિતિ EXPAND_OLD_SETTING:
    સ્થિતિ EXPAND_STRING_SETTING:
    સ્થિતિ EXPAND_SETTING_SUBTRACT:
    સ્થિતિ EXPAND_OWNSYNTAX:
    સ્થિતિ EXPAND_PACKADD:
    સ્થિતિ EXPAND_RUNTIME:
    સ્થિતિ EXPAND_SHELLCMD:
    સ્થિતિ EXPAND_SHELLCMDLINE:
    સ્થિતિ EXPAND_TAGS:
    સ્થિતિ EXPAND_TAGS_LISTFILES:
	પાછા FALSE;

    મૂળભૂત:
	તોડો;
    }

    પાછા vim_strchr(p_wop, WOP_FUZZY) != ખાલી;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    પૂર્ણાંક
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    પાછા vim_strchr(p_wop, WOP_FUZZY) != ખાલી && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    સ્થિર પૂર્ણાંક
sort_func_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    જો (*p1 != '<' && *p2 == '<') પાછા -1;
    જો (*p1 == '<' && *p2 != '<') પાછા 1;
    પાછા STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    સ્થિર શૂન્ય
wildescape(
    expand_T	*xp,
    char_u	*str,
    પૂર્ણાંક		numfiles,
    char_u	**files)
{
    char_u	*p;
    પૂર્ણાંક		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    જો (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	માટે (પૂર્ણાંક i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    જો (xp->xp_backslash & XP_BS_THREE)
	    {
		અક્ષર *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		જો (p != ખાલી)
		{
		    vim_free(files[i]);
		    files[i] = p;
#જો defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    જો (p != ખાલી)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    નહીંતર જો (xp->xp_backslash & XP_BS_COMMA)
	    {
		જો (vim_strchr(files[i], ',') != ખાલી)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    જો (p != ખાલી)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ifdef BACKSLASH_IN_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#નહીંતર
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#endif
	    જો (p != ખાલી)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    જો (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	જો (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    નહીંતર જો (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	માટે (પૂર્ણાંક i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    જો (p != ખાલી)
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
    સ્થિર શૂન્ય
ExpandEscape(
    expand_T	*xp,
    char_u	*str,
    પૂર્ણાંક		numfiles,
    char_u	**files,
    પૂર્ણાંક		options)
{
    // May change home directory back to "~"
    જો (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    જો (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    પૂર્ણાંક
nextwild(
    expand_T	*xp,
    પૂર્ણાંક		type,
    પૂર્ણાંક		options,	// extra options for ExpandOne()
    પૂર્ણાંક		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    પૂર્ણાંક		i;
    char_u	*p;
    પૂર્ણાંક		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    પૂર્ણાંક		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    જો (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	જો (ccline->આગત_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	નહીંતર
#endif
	{
	    may_expand_pattern = options & WILD_MAY_EXPAND_PATTERN;
	    set_expand_context(xp);
	    may_expand_pattern = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    જો (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	પાછા OK;  // Something illegal on command line
    }
    જો (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	પાછા FAIL;
    }

    i = (પૂર્ણાંક)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    જો (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	પાછા FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    જો (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	સંદેશ_puts("...");	    // show that we are busy
	out_flush();
    }

    જો (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, ખાલી, ખાલી, 0, type);
    }
    નહીંતર
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	પૂર્ણાંક	pat_len = xp->xp_pattern_len;
#જો defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	જો (unesc != ખાલી)
	    pat_src = unesc;
#endif

	જો (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	નહીંતર
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#જો defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	જો (tmp == ખાલી)
	    p = ખાલી;
	નહીંતર
	{
	    પૂર્ણાંક use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    જો (escape)
		use_options |= WILD_ESCAPE;
	    જો (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    જો (p != ખાલી && type == WILD_LONGEST)
	    {
		પૂર્ણાંક	j;

		માટે (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    જો (c == '*' || c == '?')
			તોડો;
		}
		જો ((પૂર્ણાંક)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    જો (!wild_navigate && ccline->cmdbuff != ખાલી)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	જો (cmdline_orig.string == ખાલી)
	    cmdline_orig.length = 0;
	નહીંતર
	    cmdline_orig.length = ccline->cmdlen;
    }

    જો (p != ખાલી && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	પૂર્ણાંક	difflen;
	પૂર્ણાંક	v = OK;

	difflen = (પૂર્ણાંક)plen - xp->xp_pattern_len;
	જો (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	જો (v == OK)
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
    જો (xp->xp_context == EXPAND_MAPPINGS && p == ખાલી)
	પાછા FAIL;

    જો (xp->xp_numfiles <= 0 && p == ખાલી)
	beep_flush();
    નહીંતર જો (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(શૂન્ય)ExpandOne(xp, ખાલી, ખાલી, 0, WILD_FREE);

    vim_free(p);

    પાછા OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    સ્થિર પૂર્ણાંક
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	પૂર્ણાંક		અંક_Matches,
	પૂર્ણાંક		showtail)
{
    પૂર્ણાંક	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, અંક_Matches);
    જો (compl_match_array == ખાલી)
	પાછા EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = અંક_Matches;
    માટે (પૂર્ણાંક i = 0; i < અંક_Matches; i++)
    {
	compl_match_array[i].pum_text = (xp->xp_files_abbr != ખાલી
				    && xp->xp_files_abbr[i] != ખાલી)
					? xp->xp_files_abbr[i]
					: SHOW_MATCH(i);
	compl_match_array[i].pum_info = xp->xp_files_info != ખાલી
					    ? xp->xp_files_info[i] : ખાલી;
	compl_match_array[i].pum_extra = xp->xp_files_menu != ખાલી
					    ? xp->xp_files_menu[i] : ખાલી;
	compl_match_array[i].pum_kind = xp->xp_files_kind != ખાલી
					    ? xp->xp_files_kind[i] : ખાલી;
	compl_match_array[i].pum_user_abbr_hlattr = -1;
	compl_match_array[i].pum_user_kind_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == ખાલી ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    prefix_len = vim_strsize(xp->xp_pattern);
    જો (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    પાછા EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    શૂન્ય
cmdline_pum_display(શૂન્ય)
{
    જો (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    પૂર્ણાંક
cmdline_pum_active(શૂન્ય)
{
    પાછા pum_visible() && compl_match_array != ખાલી;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    શૂન્ય
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, પૂર્ણાંક defer_redraw)
{
    પૂર્ણાંક	save_ચાવી_Typed = ચાવી_Typed;
#ifdef FEAT_EVAL
    પૂર્ણાંક	save_RedrawingDisabled = RedrawingDisabled;
    જો (cclp->આગત_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    જો (!defer_redraw)
    {
	પૂર્ણાંક save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    નહીંતર
	pum_call_update_screen();
    redrawcmd();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    ચાવી_Typed = save_ચાવી_Typed;
#ifdef FEAT_EVAL
    જો (cclp->આગત_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

    શૂન્ય
cmdline_pum_cleanup(cmdline_info_T *cclp)
{
    cmdline_pum_remove(cclp, FALSE);
    wildmenu_cleanup(cclp);
}

/*
 * Returns the starting column number to use for the cmdline completion popup
 * menu.
 */
    પૂર્ણાંક
cmdline_compl_startcol(શૂન્ય)
{
    પાછા compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(શૂન્ય)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    પાછા xp == ખાલી ? ખાલી : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    પૂર્ણાંક
cmdline_compl_is_fuzzy(શૂન્ય)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    પાછા xp != ખાલી && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    સ્થિર પૂર્ણાંક
skip_status_match_char(expand_T *xp, char_u *s)
{
    જો ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	જો (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    પાછા 2;
#endif
	પાછા 1;
    }
    પાછા 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    સ્થિર પૂર્ણાંક
status_match_len(expand_T *xp, char_u *s)
{
    પૂર્ણાંક	len = 0;

#ifdef FEAT_MENU
    પૂર્ણાંક emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    જો (emenu && menu_is_separator(s))
	પાછા 1;
#endif

    જ્યારે (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    પાછા len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    સ્થિર શૂન્ય
win_redr_status_matches(
    expand_T	*xp,
    પૂર્ણાંક		અંક_matches,
    char_u	**matches,	// list of matches
    પૂર્ણાંક		match,
    પૂર્ણાંક		showtail)
{
    પૂર્ણાંક		row;
    char_u	*buf;
    પૂર્ણાંક		len;
    પૂર્ણાંક		clen;		// length in screen cells
    પૂર્ણાંક		fillchar;
    પૂર્ણાંક		attr;
    પૂર્ણાંક		i;
    પૂર્ણાંક		highlight = TRUE;
    char_u	*selstart = ખાલી;
    પૂર્ણાંક		selstart_col = 0;
    char_u	*selend = ખાલી;
    સ્થિર પૂર્ણાંક	first_match = 0;
    પૂર્ણાંક		add_ડાબું = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    પૂર્ણાંક		emenu;
#endif
    પૂર્ણાંક		l;

    જો (matches == ખાલી)	// interrupted completion?
	પાછા;

    જો (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    નહીંતર
	buf = alloc(topframe->fr_width + 1);
    જો (buf == ખાલી)
	પાછા;

    જો (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    જો (match == 0)
	first_match = 0;
    નહીંતર જો (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_ડાબું = TRUE;
    }
    નહીંતર
    {
	// check if match fits on the screen
	માટે (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	જો (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	જો (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    માટે (i = match; i < અંક_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		જો (clen >= topframe->fr_width)
		    તોડો;
	    }
	    જો (i == અંક_matches)
		add_ડાબું = TRUE;
	}
    }
    જો (add_ડાબું)
	જ્યારે (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    જો (clen >= topframe->fr_width)
		તોડો;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    જો (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    નહીંતર
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    જ્યારે (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	જો (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	જો (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (પૂર્ણાંક)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	નહીંતર
#endif
	માટે ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    જો (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    નહીંતર
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (પૂર્ણાંક)STRLEN(buf + len);
	    }
	}
	જો (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	જો (++i == અંક_matches)
		તોડો;
    }

    જો (i != અંક_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    જો (row >= 0)
    {
	જો (wild_menu_showing == 0)
	{
	    જો (સંદેશ_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		જો (cmdline_row == Rows - 1)
		{
		    screen_del_લીટીઓ(0, 0, 1, (પૂર્ણાંક)Rows, TRUE, 0, ખાલી);
		    ++સંદેશ_scrolled;
		}
		નહીંતર
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    નહીંતર
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		જો (lastwin->w_status_height == 0)
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
	જો (selstart != ખાલી && highlight)
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

#જો defined(FEAT_EVAL)
/*
 * Apply -completeopt=escape to a string about to be inserted into the command
 * line as a completion result.  If "str" is non-NULL and the active expansion
 * context is a customlist/custom user command with UCC_ESCAPE set, free "str"
 * and return a newly-allocated copy with spaces, tabs and backslashes prefixed
 * by a backslash.  Otherwise return "str" unchanged.
 */
    સ્થિર char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    જો (str == ખાલી)
	પાછા ખાલી;
    જો ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	પાછા str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    જો (p == ખાલી)
	પાછા str;
    vim_free(str);
    પાછા p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    સ્થિર char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    પૂર્ણાંક		srclen,
    પૂર્ણાંક		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    જો ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	પાછા ખાલી;

    buf = alloc(srclen + 1);
    જો (buf == ખાલી)
	પાછા ખાલી;

    d = buf;
    end = src + srclen;
    માટે (p = src; p < end; ++p)
    {
	જો (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (પૂર્ણાંક)(d - buf);
    પાછા buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    સ્થિર char_u *
get_next_or_prev_match(પૂર્ણાંક mode, expand_T *xp)
{
    પૂર્ણાંક	    findex = xp->xp_selected;
    પૂર્ણાંક	    ht;

    // When no matches found, return NULL
    જો (xp->xp_numfiles <= 0)
	પાછા ખાલી;

    જો (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	જો (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    નહીંતર જો (mode == WILD_NEXT)
    {
	// Select the next entry
	++findex;
    }
    નહીંતર   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	જો (ht > 3)
	    ht -= 2;

	જો (mode == WILD_PAGEUP)
	{
	    જો (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    નહીંતર જો (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    નહીંતર
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	નહીંતર    // mode == WILD_PAGEDOWN
	{
	    જો (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    નહીંતર જો (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    નહીંતર
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    જો (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	જો (xp->xp_orig != ખાલી)
	    findex = -1;
	નહીંતર
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    જો (p_wmnu)
    {
	જો (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	નહીંતર જો (vim_strchr(p_wop, WOP_PUM) != ખાલી)
	{
	    જો (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	નહીંતર
	    win_redr_status_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    જો (findex == -1)
	પાછા vim_strsave(xp->xp_orig);
#જો defined(FEAT_EVAL)
    પાછા apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#નહીંતર
    પાછા vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    સ્થિર char_u *
ExpandOne_start(પૂર્ણાંક mode, expand_T *xp, char_u *str, પૂર્ણાંક options)
{
    પૂર્ણાંક		non_suf_match;		// number without matching suffix
    પૂર્ણાંક		i;
    char_u	*ss = ખાલી;

    // Do the expansion.
    જો (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	જો (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    નહીંતર જો (xp->xp_numfiles == 0)
    {
	જો (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    નહીંતર
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	જો (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    જો (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    નહીંતર
		non_suf_match = 1;
	    જો ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		માટે (i = 0; i < 2; ++i)
		    જો (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    જો (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		જો (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		નહીંતર જો (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    જો (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    પાછા ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    સ્થિર char_u *
find_longest_match(expand_T *xp, પૂર્ણાંક options)
{
    long_u	len;
    પૂર્ણાંક		mb_len = 1;
    પૂર્ણાંક		c0, ci;
    પૂર્ણાંક		i;
    char_u	*ss;

    માટે (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	જો (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	નહીંતર
	    c0 = xp->xp_files[0][len];
	માટે (i = 1; i < xp->xp_numfiles; ++i)
	{
	    જો (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    નહીંતર
		ci = xp->xp_files[i][len];
	    જો (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		જો (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    તોડો;
	    }
	    નહીંતર જો (c0 != ci)
		તોડો;
	}
	જો (i < xp->xp_numfiles)
	{
	    જો (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    તોડો;
	}
    }

    ss = alloc(len + 1);
    જો (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    પાછા ss;
}

    શૂન્ય
free_xp_files_extra(expand_T *xp, પૂર્ણાંક numfiles)
{
    જો (xp->xp_files_abbr != ખાલી)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = ખાલી;
    }
    જો (xp->xp_files_kind != ખાલી)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = ખાલી;
    }
    જો (xp->xp_files_menu != ખાલી)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = ખાલી;
    }
    જો (xp->xp_files_info != ખાલી)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = ખાલી;
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
    પૂર્ણાંક		options,
    પૂર્ણાંક		mode)
{
    char_u	*ss = ખાલી;
    પૂર્ણાંક		orig_saved = FALSE;
#જો defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    પૂર્ણાંક		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    જો (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	પાછા get_next_or_prev_match(mode, xp);

    જો (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    નહીંતર જો (mode == WILD_APPLY)
    {
	જો (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	નહીંતર
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#જો defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    જો (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	જો (compl_match_array != ખાલી)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    જો (mode == WILD_FREE)	// only release file name
	પાછા ખાલી;

    જો (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#જો defined(FEAT_EVAL)
	જો (ss != ખાલી)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    જો (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#જો defined(FEAT_EVAL)
	જો (ss != ખાલી)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    જો (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	અક્ષર	*prefix = "";
	અક્ષર	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	પૂર્ણાંક	n = xp->xp_numfiles - 1;
	પૂર્ણાંક	i;
#જો defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = ખાલી;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	જો ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    જો (escaped != ખાલી)
	    {
		માટે (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    જો (escaped[i] == ખાલી)
		    {
			જ્યારે (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			તોડો;
		    }
		}
		જો (escaped != ખાલી)
		    files = escaped;
	    }
	}
#નહીંતર
	char_u	**files = xp->xp_files;
#endif

	જો (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	નહીંતર જો (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	માટે (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	જો (ss != ખાલી)
	{
	    size_t  ss_len = 0;

	    માટે (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (અક્ષર *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (અક્ષર *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#જો defined(FEAT_EVAL)
	જો (escaped != ખાલી)
	{
	    માટે (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    જો (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    જો (!orig_saved)
	vim_free(orig);

#જો defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    જો (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    પાછા ss;
}

/*
 * Prepare an expand structure for use.
 */
    શૂન્ય
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
    શૂન્ય
ExpandCleanup(expand_T *xp)
{
    જો (xp->xp_numfiles >= 0)
    {
	free_xp_files_extra(xp, xp->xp_numfiles);
	FreeWild(xp->xp_numfiles, xp->xp_files);
	xp->xp_numfiles = -1;
    }
    VIM_CLEAR(xp->xp_orig);
}

    શૂન્ય
clear_cmdline_orig(શૂન્ય)
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
    સ્થિર શૂન્ય
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	પૂર્ણાંક		અંક_Matches,
	પૂર્ણાંક		લીટીઓ,
	પૂર્ણાંક		linenr,
	પૂર્ણાંક		maxlen,
	પૂર્ણાંક		showtail,
	પૂર્ણાંક		dir_attr)
{
    પૂર્ણાંક		i, j;
    પૂર્ણાંક		isdir;
    પૂર્ણાંક		lastlen;
    char_u	*p;

    lastlen = 999;
    માટે (j = linenr; j < અંક_Matches; j += લીટીઓ)
    {
	જો (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    સંદેશ_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    સંદેશ_advance(maxlen + 1);
	    સંદેશ_puts((અક્ષર *)p);
	    સંદેશ_advance(maxlen + 3);
	    સંદેશ_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    તોડો;
	}
	માટે (i = maxlen - lastlen; --i >= 0; )
	    સંદેશ_putchar(' ');
	જો (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    જો (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_path;
		char_u	*path;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_path = expand_env_save_opt(matches[j], TRUE, ખાલી);
		path = exp_path != ખાલી ? exp_path : matches[j];
		halved_slash = backslash_halve_save(path);
		isdir = mch_isdir(halved_slash != ખાલી ? halved_slash
			: matches[j]);
		vim_free(exp_path);
		જો (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    નહીંતર
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    જો (showtail)
		p = SHOW_MATCH(j);
	    નહીંતર
	    {
		home_replace(ખાલી, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	નહીંતર
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = સંદેશ_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    જો (સંદેશ_col > 0)	// when not wrapped around
    {
	સંદેશ_clr_eos();
	સંદેશ_putchar('\n');
    }
    out_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    સ્થિર પૂર્ણાંક
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	પૂર્ણાંક		અંક_Matches,
	પૂર્ણાંક		showtail,
	પૂર્ણાંક		noselect)
{
    પૂર્ણાંક	retval = cmdline_pum_create(ccline, xp, matches, અંક_Matches, showtail);

    જો (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    પાછા retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    પૂર્ણાંક
showmatches(
    expand_T	*xp,
    પૂર્ણાંક		display_wildmenu,
    પૂર્ણાંક		display_list,
    પૂર્ણાંક		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    પૂર્ણાંક		અંક_Matches;
    char_u	**matches;
    પૂર્ણાંક		i;
    પૂર્ણાંક		maxlen;
    પૂર્ણાંક		લીટીઓ;
    પૂર્ણાંક		columns;
    પૂર્ણાંક		attr;
    પૂર્ણાંક		showtail;
    પૂર્ણાંક		noselect = (wim_flags_arg & WIM_NOSELECT);
    પૂર્ણાંક		noinsert = (wim_flags_arg & WIM_NOINSERT);
    પૂર્ણાંક		cmdline_unchanged = noselect || noinsert;

    જો (xp->xp_numfiles == -1)
    {
	પૂર્ણાંક retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&અંક_Matches, &matches);
	જો (retval != EXPAND_OK)
	    પાછા retval;
	showtail = expand_showtail(xp);
    }
    નહીંતર
    {
	અંક_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    જો (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != ખાલી)
	પાછા show_pum_matches(ccline, xp, matches, અંક_Matches,
		showtail && !cmdline_unchanged, noselect);

    જો (display_list)
    {
	સંદેશ_didany = FALSE;		// lines_left will be set
	સંદેશ_start();			// prepare for paging
	સંદેશ_putchar('\n');
	out_flush();
	cmdline_row = સંદેશ_row;
	સંદેશ_didany = FALSE;		// lines_left will be set again
	સંદેશ_start();			// prepare for paging
    }

    જો (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    નહીંતર જો (display_wildmenu && !display_list)
	win_redr_status_matches(xp, અંક_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    નહીંતર જો (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	માટે (i = 0; i < અંક_Matches; ++i)
	{
	    પૂર્ણાંક	len;
	    જો (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(ખાલી, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    નહીંતર
		len = vim_strsize(SHOW_MATCH(i));
	    જો (len > maxlen)
		maxlen = len;
	}

	જો (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    લીટીઓ = અંક_Matches;
	નહીંતર
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    જો (columns < 1)
		columns = 1;
	    લીટીઓ = (અંક_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	જો (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    સંદેશ_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    સંદેશ_clr_eos();
	    સંદેશ_advance(maxlen - 3);
	    સંદેશ_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	માટે (i = 0; i < લીટીઓ; ++i)
	{
	    showmatches_oneline(xp, matches, અંક_Matches, લીટીઓ, i,
						maxlen, showtail, attr);
	    જો (got_int)
	    {
		got_int = FALSE;
		તોડો;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = સંદેશ_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    જો (display_wildmenu && display_list)
    {
	જો (vim_strchr(p_wop, WOP_PUM) != ખાલી)
	    (શૂન્ય)show_pum_matches(ccline, xp, matches, અંક_Matches,
		    showtail && !cmdline_unchanged, noselect);
	નહીંતર
	    win_redr_status_matches(xp, અંક_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    જો (xp->xp_numfiles == -1)
    {
	FreeWild(અંક_Matches, matches);
	free_xp_files_extra(xp, અંક_Matches);
    }

    પાછા EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    સ્થિર char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    પૂર્ણાંક		had_sep = FALSE;

    માટે (p = s; *p != NUL; )
    {
	જો (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	નહીંતર જો (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    પાછા t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    સ્થિર પૂર્ણાંક
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    જો (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	પાછા FALSE;

    end = gettail(xp->xp_pattern);
    જો (end == xp->xp_pattern)		// there is no path separator
	પાછા FALSE;

    માટે (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	જો (rem_backslash(s))
	    ++s;
	નહીંતર જો (vim_strchr((char_u *)"*?[", *s) != ખાલી)
	    પાછા FALSE;
    }
    પાછા TRUE;
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
    પૂર્ણાંક		len,
    પૂર્ણાંક		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    પૂર્ણાંક		i, j;
    પૂર્ણાંક		new_len;
    char_u	*tail;
    પૂર્ણાંક		ends_in_star;

    જો (context != EXPAND_FILES
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
	જો (context == EXPAND_FINDFUNC
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
	નહીંતર
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    માટે (i = 0; i < len; i++)
	    {
		જો (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		જો (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		જો ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    જો (retval != ખાલી)
	    {
		retval[0] = '^';
		j = 1;
		માટે (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    જો (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			તોડો;

		    પસંદગી (fname[i])
		    {
			સ્થિતિ '*':   retval[j++] = '.';
				    તોડો;
			સ્થિતિ '~':   retval[j++] = '\\';
				    તોડો;
			સ્થિતિ '?':   retval[j] = '.';
				    ચાલુ;
			સ્થિતિ '.':   જો (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    તોડો;
			સ્થિતિ '\\':  જો (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    તોડો;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    નહીંતર
    {
	retval = alloc(len + 4);
	જો (retval != ખાલી)
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
	    માટે (i = len - 2; i >= 0; --i)
	    {
		જો (retval[i] != '\\')
		    તોડો;
		ends_in_star = !ends_in_star;
	    }
#endif
	    જો ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == ખાલી
		    && vim_strchr(retval, '`') == ખાલી)
		retval[len++] = '*';
	    નહીંતર જો (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    પાછા retval;
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
    શૂન્ય
set_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    જો ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_શોધ_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_શોધ_EXTRA
	શોધ_first_લીટી = 0; // Search entire buffer
#endif
	પાછા;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    જો (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->આગત_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	પાછા;
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
    સ્થિર char_u *
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, પૂર્ણાંક *complp)
{
    char_u	*p = ખાલી;
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    જો (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    નહીંતર
    {
	p = cmd;
	જ્યારે (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	જો (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    જ્યારે (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	જો (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    જ્યારે (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	જો (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != ખાલી)
	    ++p;
	len = (પૂર્ણાંક)(p - cmd);

	જો (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    પાછા ખાલી;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	જો ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    જ્યારે (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    જો (*p == NUL && ASCII_ISALNUM(p[-1]))
	પાછા ખાલી;

    જો (eap->cmdidx == CMD_SIZE)
    {
	જો (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != ખાલી)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	નહીંતર જો (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, ખાલી, xp, complp);
	    જો (p == ખાલી)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    જો (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	પાછા ખાલી;
    }

    પાછા p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    સ્થિર શૂન્ય
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	પૂર્ણાંક		usefilter,
	expand_T	*xp,
	પૂર્ણાંક		*complp)
{
    char_u	*p;
    પૂર્ણાંક		c;
    પૂર્ણાંક		in_quote = FALSE;
    char_u	*bow = ખાલી;	// Beginning of word
    પૂર્ણાંક		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    જ્યારે (*p != NUL)
    {
	જો (has_mbyte)
	    c = mb_ptr2char(p);
	નહીંતર
	    c = *p;
	જો (c == '\\' && p[1] != NUL)
	    ++p;
	નહીંતર જો (c == '`')
	{
	    જો (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	નહીંતર જો (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != ખાલી && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    જ્યારે (*p != NUL)
	    {
		જો (has_mbyte)
		    c = mb_ptr2char(p);
		નહીંતર
		    c = *p;
		જો (c == '`' || vim_isfilec_or_wc(c))
		    તોડો;
		જો (has_mbyte)
		    len = (*mb_ptr2len)(p);
		નહીંતર
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    જો (in_quote)
		bow = p;
	    નહીંતર
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    જો (bow != ખાલી && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    જો (usefilter
	    || (eap != ખાલી
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	જો (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    જો (*xp->xp_pattern == '$')
    {
	માટે (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    જો (!vim_isIDc(*p))
		તોડો;
	જો (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    જો (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    જો (*xp->xp_pattern == '~')
    {
	માટે (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	જો (*p == NUL && p > xp->xp_pattern + 1
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
    સ્થિર char_u *
set_context_in_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    જો (p == ખાલી)
	xp->xp_pattern = arg;
    નહીંતર
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    પાછા ખાલી;
}

#ifdef FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    સ્થિર char_u *
set_context_in_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    જો (p == ખાલી)
	xp->xp_pattern = arg;
    નહીંતર
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    પાછા ખાલી;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    સ્થિર char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    જો (*arg != NUL)
	arg = skip_vimgrep_pat(arg, ખાલી, ખાલી);
    જો (arg == ખાલી || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	પાછા ખાલી;
    }
    પાછા skipwhite(arg);
}

#ifdef FEAT_શોધ_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    સ્થિર char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    જો (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	જો (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    પાછા find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    સ્થિર char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    પૂર્ણાંક		delim;

    delim = *arg;	    // get the delimiter
    જો (delim)
	++arg;		    // skip delimiter if there is one

    જ્યારે (arg[0] != NUL && arg[0] != delim)
    {
	જો (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    જો (arg[0] != NUL)
	પાછા arg + 1;

    પાછા ખાલી;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    સ્થિર char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    પૂર્ણાંક		delim;

    delim = *arg;
    જો (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	જો (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    જ્યારે (arg[0] != NUL && arg[0] != delim)
	    {
		જો (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    જો (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    જ્યારે (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == ખાલી)
	++arg;
    જો (arg[0] != NUL)
	પાછા arg;

    પાછા ખાલી;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    સ્થિર char_u *
find_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    જો (*arg != '/')
	પાછા ખાલી;

    // Match regexp, not just whole words
    માટે (++arg; *arg && *arg != '/'; arg++)
	જો (*arg == '\\' && arg[1] != NUL)
	    arg++;
    જો (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	જો (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == ખાલી)
	    xp->xp_context = EXPAND_NOTHING;
	નહીંતર
	    પાછા arg;
    }

    પાછા ખાલી;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    સ્થિર char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    જ્યારે ((xp->xp_pattern = vim_strchr(arg, ' ')) != ખાલી)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    જો (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    પાછા ખાલી;
}
#endif

#જો defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    સ્થિર char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    જો (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    નહીંતર
    {
	જો ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_pattern = skipwhite(p);
	}
	નહીંતર
	    xp->xp_context = EXPAND_NOTHING;
    }

    પાછા ખાલી;
}
#endif

સ્થિર ગણના
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
સ્થિર ગણના
{
    EXP_BREAKPT_ADD,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    સ્થિર char_u *
set_context_in_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_start;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_pattern = arg;

    જો (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    નહીંતર જો (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    નહીંતર
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    જો (*p == NUL)
	પાછા ખાલી;
    subcmd_start = p;

    જો (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	જો (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    જો (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		પાછા ખાલી;
	    }
	    p = skipwhite(p);
	}
	જો (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	નહીંતર
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    નહીંતર જો (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    પાછા ખાલી;
}

    સ્થિર char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = ખાલી;

    p = skipwhite(arg);
    જો (VIM_ISDIGIT(*p))
	પાછા ખાલી;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    પાછા ખાલી;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    સ્થિર char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    પૂર્ણાંક    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    જો (*p == NUL)
	પાછા ખાલી;

    માટે (;;)
    {
	જો (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    ચાલુ;
	}
	જો (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    ચાલુ;
	}
	તોડો;
    }

    જો ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    નહીંતર જો ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    નહીંતર જો ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    પાછા ખાલી;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    સ્થિર શૂન્ય
set_context_with_pattern(expand_T *xp)
{
    પૂર્ણાંક		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_શોધ_EXTRA
    પૂર્ણાંક		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    જો (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	પાછા;
#endif

    xp->xp_pattern = ccline->cmdbuff + skiplen;
    xp->xp_pattern_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_PATTERN_IN_BUF;
    xp->xp_શોધ_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    સ્થિર char_u *
set_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	long		argt,
	પૂર્ણાંક		compl,
	પૂર્ણાંક		forceit)
{
    char_u  *nextcmd;

    પસંદગી (cmdidx)
    {
	સ્થિતિ CMD_find:
	સ્થિતિ CMD_sfind:
	સ્થિતિ CMD_tabfind:
	    જો (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    તોડો;
	સ્થિતિ CMD_cd:
	સ્થિતિ CMD_chdir:
	સ્થિતિ CMD_tcd:
	સ્થિતિ CMD_tchdir:
	સ્થિતિ CMD_lcd:
	સ્થિતિ CMD_lchdir:
	    જો (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    તોડો;
	સ્થિતિ CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    તોડો;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	સ્થિતિ CMD_aboveleft:
	સ્થિતિ CMD_argdo:
	સ્થિતિ CMD_belowright:
	સ્થિતિ CMD_botright:
	સ્થિતિ CMD_browse:
	સ્થિતિ CMD_bufdo:
	સ્થિતિ CMD_cdo:
	સ્થિતિ CMD_cfdo:
	સ્થિતિ CMD_confirm:
	સ્થિતિ CMD_debug:
	સ્થિતિ CMD_folddoclosed:
	સ્થિતિ CMD_folddoopen:
	સ્થિતિ CMD_hide:
	સ્થિતિ CMD_horizontal:
	સ્થિતિ CMD_keepalt:
	સ્થિતિ CMD_keepjumps:
	સ્થિતિ CMD_keepmarks:
	સ્થિતિ CMD_keeppatterns:
	સ્થિતિ CMD_ldo:
	સ્થિતિ CMD_leftabove:
	સ્થિતિ CMD_lfdo:
	સ્થિતિ CMD_lockmarks:
	સ્થિતિ CMD_noautocmd:
	સ્થિતિ CMD_noswapfile:
	સ્થિતિ CMD_rightbelow:
	સ્થિતિ CMD_sandbox:
	સ્થિતિ CMD_silent:
	સ્થિતિ CMD_tab:
	સ્થિતિ CMD_tabdo:
	સ્થિતિ CMD_topleft:
	સ્થિતિ CMD_unsilent:
	સ્થિતિ CMD_verbose:
	સ્થિતિ CMD_vertical:
	સ્થિતિ CMD_windo:
	સ્થિતિ CMD_vim9cmd:
	સ્થિતિ CMD_legacy:
	    પાછા arg;

	સ્થિતિ CMD_filter:
	    પાછા set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_શોધ_EXTRA
	સ્થિતિ CMD_match:
	    પાછા set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	સ્થિતિ CMD_command:
	    પાછા set_context_in_user_cmd(xp, arg);

	સ્થિતિ CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    તોડો;

	સ્થિતિ CMD_global:
	સ્થિતિ CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    જો (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    પાછા nextcmd;

	સ્થિતિ CMD_and:
	સ્થિતિ CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    જો (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    પાછા nextcmd;

	સ્થિતિ CMD_isearch:
	સ્થિતિ CMD_dsearch:
	સ્થિતિ CMD_ilist:
	સ્થિતિ CMD_dlist:
	સ્થિતિ CMD_ijump:
	સ્થિતિ CMD_psearch:
	સ્થિતિ CMD_djump:
	સ્થિતિ CMD_isplit:
	સ્થિતિ CMD_dsplit:
	    પાછા find_cmd_after_isearch_cmd(xp, arg);
	સ્થિતિ CMD_autocmd:
	    પાછા set_context_in_autocmd(xp, arg, FALSE);
	સ્થિતિ CMD_doautocmd:
	સ્થિતિ CMD_doautoall:
	    પાછા set_context_in_autocmd(xp, arg, TRUE);
	સ્થિતિ CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    તોડો;
	સ્થિતિ CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    તોડો;
	સ્થિતિ CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    તોડો;
	સ્થિતિ CMD_tag:
	સ્થિતિ CMD_stag:
	સ્થિતિ CMD_ptag:
	સ્થિતિ CMD_ltag:
	સ્થિતિ CMD_tselect:
	સ્થિતિ CMD_stselect:
	સ્થિતિ CMD_ptselect:
	સ્થિતિ CMD_tjump:
	સ્થિતિ CMD_stjump:
	સ્થિતિ CMD_ptjump:
	    જો (vim_strchr(p_wop, WOP_TAGFILE) != ખાલી)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    નહીંતર
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    તોડો;
	સ્થિતિ CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    તોડો;
#ifdef FEAT_SYN_HL
	સ્થિતિ CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    તોડો;
#endif
#ifdef FEAT_EVAL
	સ્થિતિ CMD_final:
	સ્થિતિ CMD_const:
	સ્થિતિ CMD_let:
	સ્થિતિ CMD_var:
	સ્થિતિ CMD_if:
	સ્થિતિ CMD_elseif:
	સ્થિતિ CMD_while:
	સ્થિતિ CMD_for:
	સ્થિતિ CMD_echo:
	સ્થિતિ CMD_echon:
	સ્થિતિ CMD_execute:
	સ્થિતિ CMD_echomsg:
	સ્થિતિ CMD_echoerr:
	સ્થિતિ CMD_echoconsole:
	સ્થિતિ CMD_echowindow:
	સ્થિતિ CMD_call:
	સ્થિતિ CMD_return:
	સ્થિતિ CMD_cexpr:
	સ્થિતિ CMD_caddexpr:
	સ્થિતિ CMD_cgetexpr:
	સ્થિતિ CMD_lexpr:
	સ્થિતિ CMD_laddexpr:
	સ્થિતિ CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    તોડો;

	સ્થિતિ CMD_unlet:
	    પાછા set_context_in_unlet_cmd(xp, arg);
	સ્થિતિ CMD_function:
	સ્થિતિ CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    તોડો;
	સ્થિતિ CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    તોડો;

	સ્થિતિ CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    તોડો;
#endif
	સ્થિતિ CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    તોડો;
#ifdef FEAT_CSCOPE
	સ્થિતિ CMD_cscope:
	સ્થિતિ CMD_lcscope:
	સ્થિતિ CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    તોડો;
#endif
#ifdef FEAT_SIGNS
	સ્થિતિ CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    તોડો;
#endif
	સ્થિતિ CMD_bdelete:
	સ્થિતિ CMD_bwipeout:
	સ્થિતિ CMD_bunload:
	    જ્યારે ((xp->xp_pattern = vim_strchr(arg, ' ')) != ખાલી)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	સ્થિતિ CMD_buffer:
	સ્થિતિ CMD_sbuffer:
	સ્થિતિ CMD_pbuffer:
	સ્થિતિ CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    તોડો;
#ifdef FEAT_DIFF
	સ્થિતિ CMD_diffget:
	સ્થિતિ CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    તોડો;
#endif
	સ્થિતિ CMD_USER:
	સ્થિતિ CMD_USER_BUF:
	    પાછા set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	સ્થિતિ CMD_map:	    સ્થિતિ CMD_noremap:
	સ્થિતિ CMD_nmap:	    સ્થિતિ CMD_nnoremap:
	સ્થિતિ CMD_vmap:	    સ્થિતિ CMD_vnoremap:
	સ્થિતિ CMD_omap:	    સ્થિતિ CMD_onoremap:
	સ્થિતિ CMD_imap:	    સ્થિતિ CMD_inoremap:
	સ્થિતિ CMD_cmap:	    સ્થિતિ CMD_cnoremap:
	સ્થિતિ CMD_lmap:	    સ્થિતિ CMD_lnoremap:
	સ્થિતિ CMD_smap:	    સ્થિતિ CMD_snoremap:
	સ્થિતિ CMD_tmap:	    સ્થિતિ CMD_tnoremap:
	સ્થિતિ CMD_xmap:	    સ્થિતિ CMD_xnoremap:
	    પાછા set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	સ્થિતિ CMD_unmap:
	સ્થિતિ CMD_nunmap:
	સ્થિતિ CMD_vunmap:
	સ્થિતિ CMD_ounmap:
	સ્થિતિ CMD_iunmap:
	સ્થિતિ CMD_cunmap:
	સ્થિતિ CMD_lunmap:
	સ્થિતિ CMD_sunmap:
	સ્થિતિ CMD_tunmap:
	સ્થિતિ CMD_xunmap:
	    પાછા set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	સ્થિતિ CMD_mapclear:
	સ્થિતિ CMD_nmapclear:
	સ્થિતિ CMD_vmapclear:
	સ્થિતિ CMD_omapclear:
	સ્થિતિ CMD_imapclear:
	સ્થિતિ CMD_cmapclear:
	સ્થિતિ CMD_lmapclear:
	સ્થિતિ CMD_smapclear:
	સ્થિતિ CMD_tmapclear:
	સ્થિતિ CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    તોડો;

	સ્થિતિ CMD_abbreviate:	સ્થિતિ CMD_noreabbrev:
	સ્થિતિ CMD_cabbrev:	સ્થિતિ CMD_cnoreabbrev:
	સ્થિતિ CMD_iabbrev:	સ્થિતિ CMD_inoreabbrev:
	    પાછા set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	સ્થિતિ CMD_unabbreviate:
	સ્થિતિ CMD_cunabbrev:
	સ્થિતિ CMD_iunabbrev:
	    પાછા set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	સ્થિતિ CMD_menu:	    સ્થિતિ CMD_noremenu:	    સ્થિતિ CMD_unmenu:
	સ્થિતિ CMD_amenu:	    સ્થિતિ CMD_anoremenu:	    સ્થિતિ CMD_aunmenu:
	સ્થિતિ CMD_nmenu:	    સ્થિતિ CMD_nnoremenu:	    સ્થિતિ CMD_nunmenu:
	સ્થિતિ CMD_vmenu:	    સ્થિતિ CMD_vnoremenu:	    સ્થિતિ CMD_vunmenu:
	સ્થિતિ CMD_omenu:	    સ્થિતિ CMD_onoremenu:	    સ્થિતિ CMD_ounmenu:
	સ્થિતિ CMD_imenu:	    સ્થિતિ CMD_inoremenu:	    સ્થિતિ CMD_iunmenu:
	સ્થિતિ CMD_cmenu:	    સ્થિતિ CMD_cnoremenu:	    સ્થિતિ CMD_cunmenu:
	સ્થિતિ CMD_tlmenu:    સ્થિતિ CMD_tlnoremenu:    સ્થિતિ CMD_tlunmenu:
	સ્થિતિ CMD_tmenu:				    સ્થિતિ CMD_tunmenu:
	સ્થિતિ CMD_popup:	    સ્થિતિ CMD_tearoff:	    સ્થિતિ CMD_emenu:
	    પાછા set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	સ્થિતિ CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    તોડો;

	સ્થિતિ CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    તોડો;

	સ્થિતિ CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    તોડો;

	સ્થિતિ CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    તોડો;

	સ્થિતિ CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    તોડો;

	સ્થિતિ CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    તોડો;

#જો defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	સ્થિતિ CMD_language:
	    પાછા set_context_in_lang_cmd(xp, arg);
#endif
#જો defined(FEAT_PROFILE)
	સ્થિતિ CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    તોડો;
#endif
	સ્થિતિ CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    તોડો;

	સ્થિતિ CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    તોડો;

	સ્થિતિ CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    તોડો;

	સ્થિતિ CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    તોડો;
#જો defined(FEAT_PROFILE)
	સ્થિતિ CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    તોડો;
#endif

	સ્થિતિ CMD_argdelete:
	    જ્યારે ((xp->xp_pattern = vim_strchr(arg, ' ')) != ખાલી)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    તોડો;

#ifdef FEAT_EVAL
	સ્થિતિ CMD_breakadd:
	સ્થિતિ CMD_profdel:
	સ્થિતિ CMD_breakdel:
	    પાછા set_context_in_breakadd_cmd(xp, arg, cmdidx);

	સ્થિતિ CMD_scriptnames:
	    પાછા set_context_in_scriptnames_cmd(xp, arg);
#endif
	સ્થિતિ CMD_filetype:
	    પાછા set_context_in_filetype_cmd(xp, arg);

	મૂળભૂત:
	    તોડો;
    }
    પાછા ખાલી;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    સ્થિર char_u *
set_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    પૂર્ણાંક			len = 0;
    exarg_T		ea;
    પૂર્ણાંક			compl = EXPAND_NOTHING;
    પૂર્ણાંક			forceit = FALSE;
    પૂર્ણાંક			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_લીટી = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    માટે (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != ખાલી; cmd++)
	;
    xp->xp_pattern = cmd;

    જો (*cmd == NUL)
	પાછા ખાલી;
    જો (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	પાછા ખાલી;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    જો (*cmd == NUL)
	પાછા ખાલી;
    જો (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	પાછા ખાલી;
    }

    જો (*cmd == '|' || *cmd == '\n')
	પાછા cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    જો (p == ખાલી)
	પાછા ખાલી;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    જો (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    જો (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    જો ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	જ્યારે (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    જ્યારે (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    જો (*p == NUL)
	    {
		જો (ea.argt & EX_ARGOPT)
		    પાછા set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		જો (ea.cmdidx == CMD_terminal)
		    પાછા set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    જો (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	જો (*arg == '>')			// append
	{
	    જો (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	નહીંતર જો (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    જો (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	જો (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    જો (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	જ્યારે (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    જો ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	જો (*arg == NUL)
	    પાછા p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    જો ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	જો (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	જ્યારે (*p)
	{
	    જો (*p == Ctrl_V)
	    {
		જો (p[1] != NUL)
		    ++p;
	    }
	    નહીંતર જો ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		જો (*(p - 1) != '\\')
		{
		    જો (*p == '|' || *p == '\n')
			પાછા p + 1;
		    પાછા ખાલી;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    જો (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == ખાલી)
	// no arguments allowed but there is something
	પાછા ખાલી;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (પૂર્ણાંક)STRLEN(buff);
    જ્યારે (*p && p < buff + len)
    {
	જો (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	નહીંતર
	{
	    જો (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    જો (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    પાછા set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    શૂન્ય
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    પૂર્ણાંક		len,	    // length of command line (excl. NUL)
    પૂર્ણાંક		col,	    // position of cursor
    પૂર્ણાંક		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    પૂર્ણાંક			context;
#endif
    પૂર્ણાંક		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    જો (col < len)
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
    જો (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    નહીંતર જો (use_ccline && ccline->આગત_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	જો (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(ખાલી, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    નહીંતર
#endif
	જ્યારે (nextcomm != ખાલી)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_લીટી = str;
    xp->xp_col = col;

    જો (col < len)
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
    પૂર્ણાંક
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    પૂર્ણાંક		col,		// position of cursor
    પૂર્ણાંક		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = ખાલી;
    પૂર્ણાંક		options = WILD_ADD_SLASH|WILD_SILENT;

    જો (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	પાછા EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    જો (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	પાછા EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (પૂર્ણાંક)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	પૂર્ણાંક	pat_len = xp->xp_pattern_len;
#જો defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	જો (unesc != ખાલી)
	    pat_src = unesc;
#endif

	જો (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	નહીંતર
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#જો defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    જો (file_str == ખાલી)
	પાછા EXPAND_UNSUCCESSFUL;

    જો (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    જો (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = ખાલી;
    }
    vim_free(file_str);

    પાછા EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	પૂર્ણાંક		*અંક_Matches,
	પૂર્ણાંક		flags,
	પૂર્ણાંક		options)
{
    પૂર્ણાંક		free_pat = FALSE;
    પૂર્ણાંક		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    જો (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	જો (pat == ખાલી)
	    પાછા ret;

	pat_end = pat + pat_len;
	માટે (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    જો (*p != '\\')
		ચાલુ;

	    જો (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    નહીંતર જો (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    નહીંતર જો (xp->xp_backslash & XP_BS_COMMA)
	    {
		જો (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		નહીંતર જો (*(p + 1) == ',')
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

    જો (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, અંક_Matches);
#endif
    }
    નહીંતર
    {
	જો (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	નહીંતર જો (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	નહીંતર જો (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	નહીંતર
	    flags = (flags | EW_DIR) & ~EW_FILE;
	જો (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, અંક_Matches, matches, flags);
    }
    જો (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    જો (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	પૂર્ણાંક j;

	માટે (j = 0; j < *અંક_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    જ્યારે (*ptr != NUL)
	    {
		જો (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		નહીંતર જો (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    પાછા ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    સ્થિર char_u *
get_behave_arg(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx == 0)
	પાછા (char_u *)"mswin";
    જો (idx == 1)
	પાછા (char_u *)"xterm";
    પાછા ખાલી;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    સ્થિર char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx < 0)
	પાછા ખાલી;

    જો (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	અક્ષર	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	પાછા (char_u *)opts_all[idx];
    }
    જો (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	અક્ષર	*opts_plugin[] = {"plugin", "on", "off"};
	પાછા (char_u *)opts_plugin[idx];
    }
    જો (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	અક્ષર	*opts_indent[] = {"indent", "on", "off"};
	પાછા (char_u *)opts_indent[idx];
    }
    જો (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	અક્ષર	*opts_onoff[] = {"on", "off"};
	પાછા (char_u *)opts_onoff[idx];
    }
    પાછા ખાલી;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    સ્થિર char_u *
get_breakadd_arg(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx >= 0 && idx <= 3)
    {
	અક્ષર	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	જો (breakpt_expand_what == EXP_BREAKPT_ADD)
	    પાછા (char_u *)opts[idx];
	નહીંતર જો (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    જો (idx <= 2)
		પાછા (char_u *)opts[idx + 1];
	}
	નહીંતર
	{
	    // profdel {func, file}
	    જો (idx <= 1)
		પાછા (char_u *)opts[idx + 1];
	}
    }
    પાછા ખાલી;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    સ્થિર char_u *
get_scriptnames_arg(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    scriptitem_T *si;

    જો (!SCRIPT_ID_VALID(idx + 1))
	પાછા ખાલી;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(ખાલી, si->sn_name, NameBuff, MAXPATHL, TRUE);
    પાછા NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    સ્થિર char_u *
get_retab_arg(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx == 0)
	પાછા (char_u *)"-indentonly";
    પાછા ખાલી;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    સ્થિર char_u *
get_messages_arg(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx == 0)
	પાછા (char_u *)"clear";
    પાછા ખાલી;
}

    સ્થિર char_u *
get_mapclear_arg(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx == 0)
	પાછા (char_u *)"<buffer>";
    પાછા ખાલી;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    સ્થિર પૂર્ણાંક
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	પૂર્ણાંક		*અંક_Matches)
{
    સ્થિર સંરચના expgen
    {
	પૂર્ણાંક		context;
	char_u	*((*func)(expand_T *, પૂર્ણાંક));
	પૂર્ણાંક		ic;
	પૂર્ણાંક		escaped;
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
	{EXPAND_EXPRESSION, get_પદાવલિ_name, FALSE, TRUE},
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
#જો defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
    પૂર્ણાંક	i;
    પૂર્ણાંક ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(tab); ++i)
    {
	જો (xp->xp_context == tab[i].context)
	{
	    જો (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, અંક_Matches,
						tab[i].func, tab[i].escaped);
	    તોડો;
	}
    }

    પાછા ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    સ્થિર પૂર્ણાંક
map_wildopts_to_ewflags(પૂર્ણાંક options)
{
    પૂર્ણાંક		flags;

    flags = EW_DIR;	// include directories
    જો (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    જો (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    જો (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    જો (options & WILD_SILENT)
	flags |= EW_SILENT;
    જો (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    જો (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    પાછા flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    સ્થિર પૂર્ણાંક
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    પૂર્ણાંક		*અંક_Matches,
    પૂર્ણાંક		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    પૂર્ણાંક		ret;
    પૂર્ણાંક		flags;
    char_u	*tofree = ખાલી;
    પૂર્ણાંક		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    જો (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	પાછા expand_files_and_dirs(xp, pat, matches, અંક_Matches, flags,
								options);

    *matches = (char_u **)"";
    *અંક_Matches = 0;
    જો (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	જો (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					અંક_Matches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*અંક_Matches, *matches);
#endif
	    પાછા OK;
	}
	પાછા FAIL;
    }

    જો (xp->xp_context == EXPAND_SHELLCMD)
	પાછા expand_shellcmd(pat, matches, અંક_Matches, flags);
    જો (xp->xp_context == EXPAND_OLD_SETTING)
	પાછા ExpandOldSetting(અંક_Matches, matches);
    જો (xp->xp_context == EXPAND_BUFFERS)
	પાછા ExpandBufnames(pat, અંક_Matches, matches, options);
#ifdef FEAT_DIFF
    જો (xp->xp_context == EXPAND_DIFF_BUFFERS)
	પાછા ExpandBufnames(pat, અંક_Matches, matches,
						options | BUF_DIFF_FILTER);
#endif
    જો (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	પાછા expand_tags(xp->xp_context == EXPAND_TAGS, pat, અંક_Matches,
								matches);
    જો (xp->xp_context == EXPAND_COLORS)
    {
	અક્ષર *directories[] = {"colors", ખાલી};
	પાછા ExpandRTDir(pat, DIP_START + DIP_OPT, અંક_Matches, matches,
								directories);
    }
    જો (xp->xp_context == EXPAND_COMPILER)
    {
	અક્ષર *directories[] = {"compiler", ખાલી};
	પાછા ExpandRTDir(pat, 0, અંક_Matches, matches, directories);
    }
    જો (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	અક્ષર *directories[] = {"syntax", ખાલી};
	પાછા ExpandRTDir(pat, 0, અંક_Matches, matches, directories);
    }
    જો (xp->xp_context == EXPAND_FILETYPE)
    {
	અક્ષર *directories[] = {"syntax", "indent", "ftplugin", ખાલી};
	પાછા ExpandRTDir(pat, 0, અંક_Matches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    જો (xp->xp_context == EXPAND_KEYMAP)
    {
	અક્ષર *directories[] = {"keymap", ખાલી};
	પાછા ExpandRTDir(pat, 0, અંક_Matches, matches, directories);
    }
#endif
#જો defined(FEAT_EVAL)
    જો (xp->xp_context == EXPAND_USER_LIST)
	પાછા ExpandUserList(xp, matches, અંક_Matches);
#endif
    જો (xp->xp_context == EXPAND_PACKADD)
	પાછા ExpandPackAddDir(pat, અંક_Matches, matches);
    જો (xp->xp_context == EXPAND_RUNTIME)
	પાછા expand_runtime_cmd(pat, અંક_Matches, matches);
    જો (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	પાછા expand_pattern_in_buf(pat, xp->xp_શોધ_dir,
		matches, અંક_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    જો ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	પૂર્ણાંક len = (પૂર્ણાંક)STRLEN(pat) + 20;

	tofree = alloc(len);
	જો (tofree == ખાલી)
	    પાછા FAIL;
	vim_snprintf((અક્ષર *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    જો (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	જો (regmatch.regprog == ખાલી)
	{
	    vim_free(tofree);
	    પાછા FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    જો (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, અંક_Matches, matches, fuzzy);
    નહીંતર જો (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, અંક_Matches, matches);
    નહીંતર જો (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, અંક_Matches, matches);
    નહીંતર જો (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, અંક_Matches, matches);
    નહીંતર જો (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, અંક_Matches);
    નહીંતર જો (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, અંક_Matches);
#જો defined(FEAT_TERMINAL)
    નહીંતર જો (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, અંક_Matches);
#endif
#જો defined(FEAT_EVAL)
    નહીંતર જો (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, અંક_Matches);
#endif
    નહીંતર
	ret = ExpandOther(pat, xp, &regmatch, matches, અંક_Matches);

    જો (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    પાછા ret;
}

    પૂર્ણાંક
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    પૂર્ણાંક		*અંક_Matches,
    char_u	*(*func)(expand_T *, પૂર્ણાંક),
					  // returns a string from the list
    પૂર્ણાંક		escaped)
{
    પાછા ExpandGenericExt(
	pat, xp, regmatch, matches, અંક_Matches, func, escaped, 0);
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
    પૂર્ણાંક
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    પૂર્ણાંક		*અંક_Matches,
    char_u	*(*func)(expand_T *, પૂર્ણાંક),
					  // returns a string from the list
    પૂર્ણાંક		escaped,
    પૂર્ણાંક		sortStartIdx)
{
    પૂર્ણાંક		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = ખાલી;
    પૂર્ણાંક		score = 0;
    પૂર્ણાંક		fuzzy;
    પૂર્ણાંક		match;
    પૂર્ણાંક		sort_matches = FALSE;
    પૂર્ણાંક		funcsort = FALSE;
    પૂર્ણાંક		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ખાલી;
    *અંક_Matches = 0;

    જો (!fuzzy)
	ga_init2(&ga, કદ(અક્ષર *), 30);
    નહીંતર
	ga_init2(&ga, કદ(fuzmatch_str_T), 30);

    માટે (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	જો (str == ખાલી)	    // end of list
	    તોડો;
	જો (*str == NUL)	    // skip empty strings
	    ચાલુ;

	જો (xp->xp_pattern[0] != NUL)
	{
	    જો (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    નહીંતર
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	નહીંતર
	    match = TRUE;

	જો (!match)
	    ચાલુ;

	જો (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	નહીંતર
	    str = vim_strsave(str);
	જો (str == ખાલી)
	{
	    જો (!fuzzy)
	    {
		ga_clear_strings(&ga);
		પાછા FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    પાછા FAIL;
	}

	જો (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    તોડો;
	}

	જો (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	નહીંતર
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

#ifdef FEAT_MENU
	જો (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    જો (*str == '\001')
		*str = '.';
	}
#endif

	જો (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    જો (ga.ga_len == 0)
	પાછા OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    જો (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    જો (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    જો (sort_matches && sortStartMatchIdx != -1)
    {
	જો (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((શૂન્ય *)ga.ga_data, (size_t)ga.ga_len, કદ(char_u *),
							   sort_func_compare);
	નહીંતર
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    જો (!fuzzy)
    {
	*matches = ga.ga_data;
	*અંક_Matches = ga.ga_len;
    }
    નહીંતર
    {
	જો (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    પાછા FAIL;
	*અંક_Matches = ga.ga_len;
    }

#જો defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    પાછા OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    સ્થિર શૂન્ય
expand_shellcmd_onedir(
	char_u		*pathed_pattern,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	પૂર્ણાંક		*અંક_Matches,
	પૂર્ણાંક		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    જો (expand_wildcards(1, &pathed_pattern, અંક_Matches, matches, flags) != OK)
	પાછા;

    જો (ga_grow(gap, *અંક_Matches) == FAIL)
    {
	FreeWild(*અંક_Matches, *matches);
	પાછા;
    }

    માટે (પૂર્ણાંક i = 0; i < *અંક_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	જો (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    જો (HASHITEM_EMPTY(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_data)[gap->ga_len++] = name;
		hash_add_item(ht, hi, name, hash);
		name = ખાલી;
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
    સ્થિર પૂર્ણાંક
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    પૂર્ણાંક		*અંક_Matches,	// return: number of matches
    પૂર્ણાંક		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = ખાલી;
    પૂર્ણાંક		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    પૂર્ણાંક		flags = flagsarg;
    પૂર્ણાંક		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    જો (buf == ખાલી)
	પાછા FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    જો (pat == ખાલી)
    {
	vim_free(buf);
	પાછા FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    માટે (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	જો (*s != '\\')
	    ચાલુ;

	p = s + 1;
	જો (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    જો (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    નહીંતર
    {
	// For an absolute name we don't use $PATH.
	જો (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	જો (path == ખાલી)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, કદ(અક્ષર *), 10);
    hash_init(&found_ht);
    માટે (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	જો (*s == NUL)
	{
	    જો (did_curdir)
		તોડો;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	નહીંતર
	{
#જો defined(MSWIN)
	    e = vim_strchr(s, ';');
#નહીંતર
	    e = vim_strchr(s, ':');
#endif
	    જો (e == ખાલી)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    જો (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    નહીંતર
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? કદ(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	જો (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    જો (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		જો (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, અંક_Matches, flags,
				    &found_ht, &ga);
	}

	જો (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_data;
    *અંક_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    જો (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    પાછા OK;
}

#જો defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    સ્થિર શૂન્ય *
call_user_expand_func(
    શૂન્ય	*(*user_expand_func)(char_u *, પૂર્ણાંક, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    પૂર્ણાંક		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = ખાલી;
    શૂન્ય	*ret;

    જો (xp->xp_arg == ખાલી || xp->xp_arg[0] == '\0' || xp->xp_લીટી == ખાલી)
	પાછા ખાલી;

    જો (ccline->cmdbuff != ખાલી)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	પૂર્ણાંક unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	જો (pat == ખાલી)
	    pat = vim_strnsave(xp->xp_pattern, xp->xp_pattern_len);
    }

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = xp->xp_લીટી;
    args[2].v_type = VAR_સંખ્યા;
    args[2].vval.v_સંખ્યા = xp->xp_col;
    args[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, args);

    current_sctx = save_current_sctx;
    જો (ccline->cmdbuff != ખાલી)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    પાછા ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    સ્થિર પૂર્ણાંક
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    પૂર્ણાંક		*અંક_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    પૂર્ણાંક		keep;
    garray_T	ga;
    પૂર્ણાંક		fuzzy;
    પૂર્ણાંક		match;
    પૂર્ણાંક		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ખાલી;
    *અંક_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    જો (retstr == ખાલી)
	પાછા FAIL;

    જો (!fuzzy)
	ga_init2(&ga, કદ(અક્ષર *), 3);
    નહીંતર
	ga_init2(&ga, કદ(fuzmatch_str_T), 3);

    માટે (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	જો (e == ખાલી)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	જો (xp->xp_pattern[0] != NUL)
	{
	    જો (!fuzzy)
		match = vim_regexec(regmatch, s, (colnr_T)0);
	    નહીંતર
	    {
		score = fuzzy_match_str(s, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	નહીંતર
	    match = TRUE;		// match everything

	*e = keep;

	જો (match)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    જો (p == ખાલી)
		તોડો;

	    જો (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		તોડો;
	    }

	    જો (!fuzzy)
		((char_u **)ga.ga_data)[ga.ga_len] = p;
	    નહીંતર
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	જો (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    જો (ga.ga_len == 0)
	પાછા OK;

    જો (!fuzzy)
    {
	*matches = ga.ga_data;
	*અંક_Matches = ga.ga_len;
    }
    નહીંતર
    {
	જો (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    પાછા FAIL;
	*અંક_Matches = ga.ga_len;
    }
    પાછા OK;
}

    શૂન્ય
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    પૂર્ણાંક		*અંક_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    પૂર્ણાંક		have_extra = FALSE;
    પૂર્ણાંક		i;

    ga_init2(&ga, કદ(અક્ષર *), 3);
    ga_init2(&ga_abbr, કદ(અક્ષર *), 3);
    ga_init2(&ga_kind, કદ(અક્ષર *), 3);
    ga_init2(&ga_menu, કદ(અક્ષર *), 3);
    ga_init2(&ga_info, કદ(અક્ષર *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_વસ્તુઓ(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = ખાલી;
	char_u	*abbr = ખાલી;
	char_u	*kind = ખાલી;
	char_u	*menu = ખાલી;
	char_u	*info = ખાલી;

	જો (tv->v_type == VAR_STRING)
	{
	    જો (tv->vval.v_string == ખાલી)
		ચાલુ;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	નહીંતર જો (tv->v_type == VAR_DICT && tv->vval.v_dict != ખાલી)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    જો (word == ખાલી)
		ચાલુ;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    જો (abbr != ખાલી || kind != ખાલી || menu != ખાલી || info != ખાલી)
		have_extra = TRUE;
	}
	નહીંતર
	    ચાલુ;  // Skip other types

	જો (p == ખાલી
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
	    તોડો;
	}

	((char_u **)ga.ga_data)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_data)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_data)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_data)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_data)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_data;
    *અંક_Matches = ga.ga_len;
    જો (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    નહીંતર
    {
	// No extra info collected; free the placeholder NULL entries.
	માટે (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	માટે (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	માટે (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	માટે (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    સ્થિર પૂર્ણાંક
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    પૂર્ણાંક		*અંક_Matches)
{
    list_T      *retlist;

    *matches = ખાલી;
    *અંક_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    જો (retlist == ખાલી)
	પાછા FAIL;

    expand_process_user_list(retlist, matches, અંક_Matches, xp);
    list_unref(retlist);
    પાછા OK;
}
#endif

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    શૂન્ય
globpath(
    char_u	*path,
    char_u	*file,
    garray_T	*ga,
    પૂર્ણાંક		expand_options,
    પૂર્ણાંક		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    જો (buf == ખાલી)
	પાછા;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#જો defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#નહીંતર
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    જ્યારે (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	જો (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    પૂર્ણાંક	    અંક_p;
	    char_u  **p;

	    જો (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    જો (ExpandFromContext(&xpc, buf, &p, &અંક_p,
			     WILD_SILENT|expand_options) != FAIL && અંક_p > 0)
	    {
		ExpandEscape(&xpc, buf, અંક_p, p, WILD_SILENT|expand_options);

		જો (ga_grow(ga, અંક_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    માટે (પૂર્ણાંક i = 0; i < અંક_p; ++i)
		    {
			((char_u **)ga->ga_data)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		નહીંતર
		{
		    FreeWild(અંક_p, p);
		    p = ખાલી;
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
    પૂર્ણાંક
wildmenu_translate_ચાવી(
	cmdline_info_T	*cclp,
	પૂર્ણાંક		ચાવી,
	expand_T	*xp,
	પૂર્ણાંક		did_wild_list)
{
    પૂર્ણાંક c = ચાવી;

    જો (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	પસંદગી (c)
	{
	    સ્થિતિ K_UP:	  c = K_ડાબું; તોડો;
	    સ્થિતિ K_DOWN:  c = K_જમણું; તોડો;
	    સ્થિતિ K_ડાબું:  c = K_UP; તોડો;
	    સ્થિતિ K_જમણું: c = K_DOWN; તોડો;
	    મૂળભૂત:	  તોડો;
	}
    }

    જો (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	જો (c == K_ડાબું)
	    c = Ctrl_P;
	નહીંતર જો (c == K_જમણું)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    જો (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    પાછા c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    સ્થિર શૂન્ય
cmdline_del(cmdline_info_T *cclp, પૂર્ણાંક from)
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
    સ્થિર પૂર્ણાંક
wildmenu_process_ચાવી_menunames(cmdline_info_T *cclp, પૂર્ણાંક ચાવી, expand_T *xp)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    જો (ચાવી == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	ચાવી = p_wc;
	ચાવી_Typed = TRUE;  // in case the key was mapped
    }
    નહીંતર જો (ચાવી == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	પૂર્ણાંક found = FALSE;

	j = (પૂર્ણાંક)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	જ્યારે (--j > 0)
	{
	    // check for start of menu name
	    જો (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		તોડો;
	    }
	    // check for start of submenu name
	    જો (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		જો (found)
		{
		    i = j + 1;
		    તોડો;
		}
		નહીંતર
		    found = TRUE;
	    }
	}
	જો (i > 0)
	    cmdline_del(cclp, i);
	ચાવી = p_wc;
	ચાવી_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    પાછા ચાવી;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    સ્થિર પૂર્ણાંક
wildmenu_process_ચાવી_filenames(cmdline_info_T *cclp, પૂર્ણાંક ચાવી, expand_T *xp)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    જો (ચાવી == K_DOWN
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	ચાવી = p_wc;
	ચાવી_Typed = TRUE;  // in case the key was mapped
    }
    નહીંતર જો (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && ચાવી == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	પૂર્ણાંક found = FALSE;

	j = cclp->cmdpos;
	i = (પૂર્ણાંક)(xp->xp_pattern - cclp->cmdbuff);
	જ્યારે (--j > i)
	{
	    જો (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    જો (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		તોડો;
	    }
	}
	જો (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    ચાવી = p_wc;
	    ચાવી_Typed = TRUE;  // in case the key was mapped
	}
    }
    નહીંતર જો (ચાવી == K_UP)
    {
	// go up a directory
	પૂર્ણાંક found = FALSE;

	j = cclp->cmdpos - 1;
	i = (પૂર્ણાંક)(xp->xp_pattern - cclp->cmdbuff);
	જ્યારે (--j > i)
	{
	    જો (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    જો (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == ખાલી
#endif
	       )
	    {
		જો (found)
		{
		    i = j + 1;
		    તોડો;
		}
		નહીંતર
		    found = TRUE;
	    }
	}

	જો (!found)
	    j = i;
	નહીંતર જો (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	નહીંતર જો (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	નહીંતર
	    j = 0;
	જો (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	નહીંતર જો (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	ચાવી = p_wc;
	ચાવી_Typed = TRUE;
    }

    પાછા ચાવી;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    પૂર્ણાંક
wildmenu_process_ચાવી(cmdline_info_T *cclp, પૂર્ણાંક ચાવી, expand_T *xp)
{
    જો (xp->xp_context == EXPAND_MENUNAMES)
	પાછા wildmenu_process_ચાવી_menunames(cclp, ચાવી, xp);
    નહીંતર જો ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	પાછા wildmenu_process_ચાવી_filenames(cclp, ચાવી, xp);

    પાછા ચાવી;
}

/*
 * Free expanded names when finished walking through the matches
 */
    શૂન્ય
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    પૂર્ણાંક skt = ચાવી_Typed;

    જો (!p_wmnu || wild_menu_showing == 0)
	પાછા;

#ifdef FEAT_EVAL
    પૂર્ણાંક save_RedrawingDisabled = RedrawingDisabled;
    જો (cclp->આગત_fn)
	RedrawingDisabled = 0;
#endif

#જો defined(FEAT_શોધ_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    જો (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    નહીંતર જો (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	last_status(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    નહીંતર
    {
	win_redraw_last_status(topframe);
	redraw_statuslines();
    }
    ચાવી_Typed = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    જો (cclp->આગત_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#જો defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    શૂન્ય
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    પૂર્ણાંક		filtered = FALSE;
    પૂર્ણાંક		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	પાછા;

    pat = tv_get_string(&argvars[0]);
    જો (check_for_string_arg(argvars, 1) == FAIL)
	પાછા;
    type = tv_get_string(&argvars[1]);

    જો (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], ખાલી);

    જો (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    જો (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    જો (STRCMP(type, "cmdline") == 0)
    {
	પૂર્ણાંક cmdline_len = (પૂર્ણાંક)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (પૂર્ણાંક)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    નહીંતર
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (પૂર્ણાંક)STRLEN(xpc.xp_pattern);
	xpc.xp_લીટી = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	પસંદગી (xpc.xp_context)
	{
	સ્થિતિ EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    પાછા;

	સ્થિતિ EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    જો (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		પાછા;
	    }

	    xpc.xp_arg = type + 7;
	    તોડો;

	સ્થિતિ EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    જો (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		પાછા;
	    }

	    xpc.xp_arg = type + 11;
	    તોડો;

# જો defined(FEAT_MENU)
	સ્થિતિ EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (પૂર્ણાંક)(xpc.xp_pattern - pattern_start);
	    તોડો;
# endif

# ifdef FEAT_CSCOPE
	સ્થિતિ EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (પૂર્ણાંક)(xpc.xp_pattern - pattern_start);
	    તોડો;
# endif

# ifdef FEAT_SIGNS
	સ્થિતિ EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (પૂર્ણાંક)(xpc.xp_pattern - pattern_start);
	    તોડો;
# endif

	સ્થિતિ EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (પૂર્ણાંક)(xpc.xp_pattern - pattern_start);
	    તોડો;

	સ્થિતિ EXPAND_SHELLCMDLINE:
	{
	    પૂર્ણાંક context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(ખાલી, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (પૂર્ણાંક)(xpc.xp_pattern - pattern_start);
	    તોડો;
	}

	સ્થિતિ EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    તોડો;

	સ્થિતિ EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    તોડો;

	મૂળભૂત:
	    તોડો;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	પૂર્ણાંક	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	જો (unesc != ખાલી)
	    pat_src = unesc;

	જો (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	નહીંતર
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    જો (rettv_list_alloc(rettv) == OK && pat != ખાલી)
    {
	પૂર્ણાંક	i;

	ExpandOne(&xpc, pat, ખાલી, options, WILD_ALL_KEEP);

	માટે (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    શૂન્ય
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    પૂર્ણાંક		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (પૂર્ણાંક)STRLEN(pat);
    set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_to_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    શૂન્ય
f_cmdcomplete_info(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    પૂર્ણાંક		    idx;
    પૂર્ણાંક		    ret = OK;

    જો (rettv_dict_alloc(rettv) == FAIL || ccline == ખાલી
	    || ccline->xpc == ખાલી || ccline->xpc->xp_files == ખાલી)
	પાછા;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (પૂર્ણાંક)cmdline_orig.length);
    જો (ret == OK)
	ret = dict_add_સંખ્યા(retdict, "pum_visible", pum_visible());
    જો (ret == OK)
	ret = dict_add_સંખ્યા(retdict, "selected", ccline->xpc->xp_selected);
    જો (ret == OK)
    {
	li = list_alloc();
	જો (li == ખાલી)
	    પાછા;
	ret = dict_add_list(retdict, "matches", li);
	જો (ret == FAIL)
	    list_unref(li);
	માટે (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    સ્થિર પૂર્ણાંક
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*લીટી, *start_લીટી, *end_લીટી;
    પૂર્ણાંક		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    પૂર્ણાંક		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ખાલી;

    જો (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	પાછા FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_લીટી = ml_get(start->lnum);
    char_u  *start_ptr = start_લીટી + start->col;
    પૂર્ણાંક	    is_single_લીટી = start->lnum == end->lnum;

    segment_len = is_single_લીટી ? (end->col - start->col)
			: (પૂર્ણાંક)(ml_get_len(start->lnum) - start->col);
    જો (ga_grow(&ga, segment_len + 2) != OK)
	જાઓ fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    જો (!is_single_લીટી)
    {
	જો (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	નહીંતર
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    જો (!is_single_લીટી)
    {
	માટે (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    પૂર્ણાંક  linelen;

	    લીટી = ml_get(lnum);
	    linelen = (પૂર્ણાંક)ml_get_len(lnum);
	    જો (ga_grow(&ga, linelen + 2) != OK)
		જાઓ fail;
	    ga_concat_len(&ga, લીટી, linelen);
	    જો (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    નહીંતર
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_લીટી = ml_get(end->lnum);
    word_end = find_word_end(end_લીટી + end->col);
    segment_len = (પૂર્ણાંક)(word_end - end_લીટી);
    જો (ga_grow(&ga, segment_len) != OK)
	જાઓ fail;
    ga_concat_len(&ga, end_લીટી + (is_single_લીટી ? end->col : 0),
	    segment_len - (is_single_લીટી ? end->col : 0));

    // Null-terminate
    જો (ga_grow(&ga, 1) != OK)
	જાઓ fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    પાછા OK;

fail:
    ga_clear(&ga);
    પાછા FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    સ્થિર પૂર્ણાંક
is_regex_match(char_u *pat, char_u *str)
{
    જો (STRCMP(pat, str) == 0)
	પાછા TRUE;

    regmatch_T	regmatch;
    પૂર્ણાંક		result;

    ++emsg_off;
    ++સંદેશ_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --સંદેશ_silent;

    જો (regmatch.regprog == ખાલી)
	પાછા FALSE;
    regmatch.rm_ic = p_ic;
    જો (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++સંદેશ_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --સંદેશ_silent;

    vim_regfree(regmatch.regprog);
    પાછા result;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    સ્થિર char_u *
concat_pattern_with_buffer_match(
	char_u *pat,
	પૂર્ણાંક pat_len,
	pos_T *end_match_pos,
	પૂર્ણાંક lowercase UNUSED)
{
    char_u  *લીટી = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(લીટી + end_match_pos->col);
    પૂર્ણાંક	    match_len = (પૂર્ણાંક)(word_end - (લીટી + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    જો (match == ખાલી)
	પાછા ખાલી;
    mch_memmove(match, pat, pat_len);
    જો (match_len > 0)
    {
#જો defined(FEAT_EVAL) || defined(FEAT_SPELL)
	જો (lowercase)
	{
	    char_u  *mword = vim_strnsave(લીટી + end_match_pos->col,
		    match_len);
	    જો (mword == ખાલી)
		જાઓ cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    જો (lower == ખાલી)
		જાઓ cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	નહીંતર
#endif
	    mch_memmove(match + pat_len, લીટી + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    પાછા match;

#જો defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    પાછા ખાલી;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    સ્થિર પૂર્ણાંક
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    પૂર્ણાંક		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    પૂર્ણાંક		*અંક_Matches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    પૂર્ણાંક		found_new_match;
    પૂર્ણાંક		looped_around = FALSE;
    પૂર્ણાંક		pat_len;
    પૂર્ણાંક		has_range = FALSE;
    પૂર્ણાંક		compl_started = FALSE;
    પૂર્ણાંક		શોધ_flags;
    char_u	*match, *full_match;
    પૂર્ણાંક		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ખાલી;

#ifdef FEAT_શોધ_EXTRA
    has_range = શોધ_first_લીટી != 0;
#endif

    *matches = ખાલી;
    *અંક_Matches = 0;

    જો (pat == ખાલી || *pat == NUL)
	પાછા FAIL;

    pat_len = (પૂર્ણાંક)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_શોધ_EXTRA
    જો (has_range)
	cur_match_pos.lnum = શોધ_first_લીટી;
    નહીંતર
#endif
	cur_match_pos = pre_incsearch_pos;

    શોધ_flags = શોધ_OPT | શોધ_NOOF | શોધ_PEEK | શોધ_NFMSG
	| (has_range ? શોધ_START : 0);

    ga_init2(&ga, કદ(char_u *), 10); // Use growable array of char_u*

    માટે (;;)
    {
	++emsg_off;
	++સંદેશ_silent;
	found_new_match = searchit(ખાલી, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		શોધ_flags, RE_LAST, ખાલી);
	--સંદેશ_silent;
	--emsg_off;

	જો (found_new_match == FAIL)
	    તોડો;

#ifdef FEAT_શોધ_EXTRA
	// If in range mode, check if match is within the range
	જો (has_range && (cur_match_pos.lnum < શોધ_first_લીટી
		    || cur_match_pos.lnum > શોધ_last_લીટી))
		તોડો;
#endif

	જો (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    જો ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		જો (looped_around)
		    તોડો;
		નહીંતર
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	જો (char_avail() || got_int)
	{
	    જો (got_int)
	    {
		(શૂન્ય)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    જાઓ cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	જો (end_match_pos.lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    ચાલુ;
	}

	// Extract the matching text prepended to completed word
	જો (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    તોડો;

	જો (exacttext)
	    match = full_match;
	નહીંતર
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    જો (match == ખાલી || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		જો (match == ખાલી || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    ચાલુ;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	માટે (પૂર્ણાંક i = 0; i < ga.ga_len; ++i)
	{
	    જો (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		તોડો;
	    }
	}
	જો (match != ખાલી)
	{
	    જો (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		જાઓ cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    જો (ga.ga_len > TAG_MANY)
		તોડો;
	}
	જો (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *અંક_Matches = ga.ga_len;
    પાછા OK;

cleanup:
    ga_clear_strings(&ga);
    પાછા FAIL;
}
