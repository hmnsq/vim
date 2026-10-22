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

#അന്തര്ഭാവ "vim.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ	cmd_showtail;	// Only show path tail in lists ?
സ്ഥിതം പൂർണ്ണസംഖ്യ	may_expand_pattern = FALSE;
സ്ഥിതം pos_T	pre_incsearch_pos; // Cursor position when incsearch started

സ്ഥിതം ശൂന്യം	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, പൂർണ്ണസംഖ്യ usefilter, expand_T *xp, പൂർണ്ണസംഖ്യ *complp);
സ്ഥിതം പൂർണ്ണസംഖ്യ	ExpandFromContext(expand_T *xp, char_u *, char_u ***, പൂർണ്ണസംഖ്യ *, പൂർണ്ണസംഖ്യ);
സ്ഥിതം char_u	*showmatches_gettail(char_u *s);
സ്ഥിതം പൂർണ്ണസംഖ്യ	expand_showtail(expand_T *xp);
സ്ഥിതം പൂർണ്ണസംഖ്യ	expand_shellcmd(char_u *filepat, char_u ***matches, പൂർണ്ണസംഖ്യ *അക്കം_Matches, പൂർണ്ണസംഖ്യ flagsarg);
#എങ്കിൽ defined(FEAT_EVAL)
സ്ഥിതം പൂർണ്ണസംഖ്യ	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, പൂർണ്ണസംഖ്യ *അക്കം_Matches);
സ്ഥിതം പൂർണ്ണസംഖ്യ	ExpandUserList(expand_T *xp, char_u ***matches, പൂർണ്ണസംഖ്യ *അക്കം_Matches);
സ്ഥിതം char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
സ്ഥിതം char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, പൂർണ്ണസംഖ്യ srclen, പൂർണ്ണസംഖ്യ *new_lenp);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ	expand_pattern_in_buf(char_u *pat, പൂർണ്ണസംഖ്യ dir, char_u ***matches, പൂർണ്ണസംഖ്യ *അക്കം_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
സ്ഥിതം pumitem_T *compl_match_array = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ compl_match_arraysize;
// First column in cmdline of the matched item for completion.
സ്ഥിതം പൂർണ്ണസംഖ്യ compl_startcol;
സ്ഥിതം പൂർണ്ണസംഖ്യ compl_selected;
// cmdline before expansion
സ്ഥിതം string_T cmdline_orig = {ശൂന്യ, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    തിരഞ്ഞെടുപ്പ് (xp->xp_context)
    {
    അവസ്ഥ EXPAND_BOOL_SETTINGS:
    അവസ്ഥ EXPAND_COLORS:
    അവസ്ഥ EXPAND_COMPILER:
    അവസ്ഥ EXPAND_DIRECTORIES:
    അവസ്ഥ EXPAND_DIRS_IN_CDPATH:
    അവസ്ഥ EXPAND_FILES:
    അവസ്ഥ EXPAND_FILES_IN_PATH:
    അവസ്ഥ EXPAND_FILETYPE:
    അവസ്ഥ EXPAND_FILETYPECMD:
    അവസ്ഥ EXPAND_HELP:
    അവസ്ഥ EXPAND_KEYMAP:
    അവസ്ഥ EXPAND_OLD_SETTING:
    അവസ്ഥ EXPAND_STRING_SETTING:
    അവസ്ഥ EXPAND_SETTING_SUBTRACT:
    അവസ്ഥ EXPAND_OWNSYNTAX:
    അവസ്ഥ EXPAND_PACKADD:
    അവസ്ഥ EXPAND_RUNTIME:
    അവസ്ഥ EXPAND_SHELLCMD:
    അവസ്ഥ EXPAND_SHELLCMDLINE:
    അവസ്ഥ EXPAND_TAGS:
    അവസ്ഥ EXPAND_TAGS_LISTFILES:
	തിരികെ FALSE;

    സ്ഥിരസ്ഥിതി:
	നിർത്തുക;
    }

    തിരികെ vim_strchr(p_wop, WOP_FUZZY) != ശൂന്യ;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    പൂർണ്ണസംഖ്യ
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    തിരികെ vim_strchr(p_wop, WOP_FUZZY) != ശൂന്യ && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
sort_func_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    എങ്കിൽ (*p1 != '<' && *p2 == '<') തിരികെ -1;
    എങ്കിൽ (*p1 == '<' && *p2 != '<') തിരികെ 1;
    തിരികെ STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    സ്ഥിതം ശൂന്യം
wildescape(
    expand_T	*xp,
    char_u	*str,
    പൂർണ്ണസംഖ്യ		numfiles,
    char_u	**files)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    എങ്കിൽ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    എങ്കിൽ (xp->xp_backslash & XP_BS_THREE)
	    {
		അക്ഷരം *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		എങ്കിൽ (p != ശൂന്യ)
		{
		    vim_free(files[i]);
		    files[i] = p;
#എങ്കിൽ defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    എങ്കിൽ (p != ശൂന്യ)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_backslash & XP_BS_COMMA)
	    {
		എങ്കിൽ (vim_strchr(files[i], ',') != ശൂന്യ)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    എങ്കിൽ (p != ശൂന്യ)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ifdef BACKSLASH_IN_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#അല്ലെങ്കിൽ
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#endif
	    എങ്കിൽ (p != ശൂന്യ)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    എങ്കിൽ (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	എങ്കിൽ (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    എങ്കിൽ (p != ശൂന്യ)
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
    സ്ഥിതം ശൂന്യം
ExpandEscape(
    expand_T	*xp,
    char_u	*str,
    പൂർണ്ണസംഖ്യ		numfiles,
    char_u	**files,
    പൂർണ്ണസംഖ്യ		options)
{
    // May change home directory back to "~"
    എങ്കിൽ (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    എങ്കിൽ (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    പൂർണ്ണസംഖ്യ
nextwild(
    expand_T	*xp,
    പൂർണ്ണസംഖ്യ		type,
    പൂർണ്ണസംഖ്യ		options,	// extra options for ExpandOne()
    പൂർണ്ണസംഖ്യ		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    പൂർണ്ണസംഖ്യ		i;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    പൂർണ്ണസംഖ്യ		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    എങ്കിൽ (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	എങ്കിൽ (ccline->നിവേശം_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	അല്ലെങ്കിൽ
#endif
	{
	    may_expand_pattern = options & WILD_MAY_EXPAND_PATTERN;
	    set_expand_context(xp);
	    may_expand_pattern = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    എങ്കിൽ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	തിരികെ OK;  // Something illegal on command line
    }
    എങ്കിൽ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	തിരികെ FAIL;
    }

    i = (പൂർണ്ണസംഖ്യ)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    എങ്കിൽ (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	തിരികെ FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    എങ്കിൽ (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	സന്ദേശം_puts("...");	    // show that we are busy
	out_flush();
    }

    എങ്കിൽ (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, ശൂന്യ, ശൂന്യ, 0, type);
    }
    അല്ലെങ്കിൽ
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	പൂർണ്ണസംഖ്യ	pat_len = xp->xp_pattern_len;
#എങ്കിൽ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	എങ്കിൽ (unesc != ശൂന്യ)
	    pat_src = unesc;
#endif

	എങ്കിൽ (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	അല്ലെങ്കിൽ
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#എങ്കിൽ defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	എങ്കിൽ (tmp == ശൂന്യ)
	    p = ശൂന്യ;
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    എങ്കിൽ (escape)
		use_options |= WILD_ESCAPE;
	    എങ്കിൽ (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    എങ്കിൽ (p != ശൂന്യ && type == WILD_LONGEST)
	    {
		പൂർണ്ണസംഖ്യ	j;

		വേണ്ടി (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    എങ്കിൽ (c == '*' || c == '?')
			നിർത്തുക;
		}
		എങ്കിൽ ((പൂർണ്ണസംഖ്യ)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    എങ്കിൽ (!wild_navigate && ccline->cmdbuff != ശൂന്യ)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	എങ്കിൽ (cmdline_orig.string == ശൂന്യ)
	    cmdline_orig.length = 0;
	അല്ലെങ്കിൽ
	    cmdline_orig.length = ccline->cmdlen;
    }

    എങ്കിൽ (p != ശൂന്യ && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	പൂർണ്ണസംഖ്യ	difflen;
	പൂർണ്ണസംഖ്യ	v = OK;

	difflen = (പൂർണ്ണസംഖ്യ)plen - xp->xp_pattern_len;
	എങ്കിൽ (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	എങ്കിൽ (v == OK)
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
    എങ്കിൽ (xp->xp_context == EXPAND_MAPPINGS && p == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (xp->xp_numfiles <= 0 && p == ശൂന്യ)
	beep_flush();
    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(ശൂന്യം)ExpandOne(xp, ശൂന്യ, ശൂന്യ, 0, WILD_FREE);

    vim_free(p);

    തിരികെ OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	പൂർണ്ണസംഖ്യ		അക്കം_Matches,
	പൂർണ്ണസംഖ്യ		showtail)
{
    പൂർണ്ണസംഖ്യ	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, അക്കം_Matches);
    എങ്കിൽ (compl_match_array == ശൂന്യ)
	തിരികെ EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = അക്കം_Matches;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_Matches; i++)
    {
	compl_match_array[i].pum_text = (xp->xp_files_abbr != ശൂന്യ
				    && xp->xp_files_abbr[i] != ശൂന്യ)
					? xp->xp_files_abbr[i]
					: SHOW_MATCH(i);
	compl_match_array[i].pum_info = xp->xp_files_info != ശൂന്യ
					    ? xp->xp_files_info[i] : ശൂന്യ;
	compl_match_array[i].pum_extra = xp->xp_files_menu != ശൂന്യ
					    ? xp->xp_files_menu[i] : ശൂന്യ;
	compl_match_array[i].pum_kind = xp->xp_files_kind != ശൂന്യ
					    ? xp->xp_files_kind[i] : ശൂന്യ;
	compl_match_array[i].pum_user_abbr_hlattr = -1;
	compl_match_array[i].pum_user_kind_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == ശൂന്യ ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    prefix_len = vim_strsize(xp->xp_pattern);
    എങ്കിൽ (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    തിരികെ EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    ശൂന്യം
cmdline_pum_display(ശൂന്യം)
{
    എങ്കിൽ (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    പൂർണ്ണസംഖ്യ
cmdline_pum_active(ശൂന്യം)
{
    തിരികെ pum_visible() && compl_match_array != ശൂന്യ;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    ശൂന്യം
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, പൂർണ്ണസംഖ്യ defer_redraw)
{
    പൂർണ്ണസംഖ്യ	save_താക്കോൽ_Typed = താക്കോൽ_Typed;
#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ	save_RedrawingDisabled = RedrawingDisabled;
    എങ്കിൽ (cclp->നിവേശം_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    എങ്കിൽ (!defer_redraw)
    {
	പൂർണ്ണസംഖ്യ save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    അല്ലെങ്കിൽ
	pum_call_update_screen();
    redrawcmd();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    താക്കോൽ_Typed = save_താക്കോൽ_Typed;
#ifdef FEAT_EVAL
    എങ്കിൽ (cclp->നിവേശം_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

    ശൂന്യം
cmdline_pum_cleanup(cmdline_info_T *cclp)
{
    cmdline_pum_remove(cclp, FALSE);
    wildmenu_cleanup(cclp);
}

/*
 * Returns the starting column number to use for the cmdline completion popup
 * menu.
 */
    പൂർണ്ണസംഖ്യ
cmdline_compl_startcol(ശൂന്യം)
{
    തിരികെ compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(ശൂന്യം)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    തിരികെ xp == ശൂന്യ ? ശൂന്യ : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    പൂർണ്ണസംഖ്യ
cmdline_compl_is_fuzzy(ശൂന്യം)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    തിരികെ xp != ശൂന്യ && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
skip_status_match_char(expand_T *xp, char_u *s)
{
    എങ്കിൽ ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	എങ്കിൽ (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    തിരികെ 2;
#endif
	തിരികെ 1;
    }
    തിരികെ 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
status_match_len(expand_T *xp, char_u *s)
{
    പൂർണ്ണസംഖ്യ	len = 0;

#ifdef FEAT_MENU
    പൂർണ്ണസംഖ്യ emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    എങ്കിൽ (emenu && menu_is_separator(s))
	തിരികെ 1;
#endif

    ആകുമ്പോൾ (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    തിരികെ len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    സ്ഥിതം ശൂന്യം
win_redr_status_matches(
    expand_T	*xp,
    പൂർണ്ണസംഖ്യ		അക്കം_matches,
    char_u	**matches,	// list of matches
    പൂർണ്ണസംഖ്യ		match,
    പൂർണ്ണസംഖ്യ		showtail)
{
    പൂർണ്ണസംഖ്യ		row;
    char_u	*buf;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		clen;		// length in screen cells
    പൂർണ്ണസംഖ്യ		fillchar;
    പൂർണ്ണസംഖ്യ		attr;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		highlight = TRUE;
    char_u	*selstart = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		selstart_col = 0;
    char_u	*selend = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	first_match = 0;
    പൂർണ്ണസംഖ്യ		add_ഇടത് = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    പൂർണ്ണസംഖ്യ		emenu;
#endif
    പൂർണ്ണസംഖ്യ		l;

    എങ്കിൽ (matches == ശൂന്യ)	// interrupted completion?
	തിരികെ;

    എങ്കിൽ (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    അല്ലെങ്കിൽ
	buf = alloc(topframe->fr_width + 1);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    എങ്കിൽ (match == 0)
	first_match = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_ഇടത് = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	// check if match fits on the screen
	വേണ്ടി (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	എങ്കിൽ (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	എങ്കിൽ (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    വേണ്ടി (i = match; i < അക്കം_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		എങ്കിൽ (clen >= topframe->fr_width)
		    നിർത്തുക;
	    }
	    എങ്കിൽ (i == അക്കം_matches)
		add_ഇടത് = TRUE;
	}
    }
    എങ്കിൽ (add_ഇടത്)
	ആകുമ്പോൾ (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    എങ്കിൽ (clen >= topframe->fr_width)
		നിർത്തുക;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    എങ്കിൽ (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    അല്ലെങ്കിൽ
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    ആകുമ്പോൾ (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	എങ്കിൽ (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	എങ്കിൽ (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (പൂർണ്ണസംഖ്യ)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	അല്ലെങ്കിൽ
#endif
	വേണ്ടി ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    എങ്കിൽ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    അല്ലെങ്കിൽ
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (പൂർണ്ണസംഖ്യ)STRLEN(buf + len);
	    }
	}
	എങ്കിൽ (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	എങ്കിൽ (++i == അക്കം_matches)
		നിർത്തുക;
    }

    എങ്കിൽ (i != അക്കം_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    എങ്കിൽ (row >= 0)
    {
	എങ്കിൽ (wild_menu_showing == 0)
	{
	    എങ്കിൽ (സന്ദേശം_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		എങ്കിൽ (cmdline_row == Rows - 1)
		{
		    screen_del_വരികൾ(0, 0, 1, (പൂർണ്ണസംഖ്യ)Rows, TRUE, 0, ശൂന്യ);
		    ++സന്ദേശം_scrolled;
		}
		അല്ലെങ്കിൽ
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		എങ്കിൽ (lastwin->w_status_height == 0)
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
	എങ്കിൽ (selstart != ശൂന്യ && highlight)
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

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Apply -completeopt=escape to a string about to be inserted into the command
 * line as a completion result.  If "str" is non-NULL and the active expansion
 * context is a customlist/custom user command with UCC_ESCAPE set, free "str"
 * and return a newly-allocated copy with spaces, tabs and backslashes prefixed
 * by a backslash.  Otherwise return "str" unchanged.
 */
    സ്ഥിതം char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	തിരികെ str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ str;
    vim_free(str);
    തിരികെ p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    സ്ഥിതം char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    പൂർണ്ണസംഖ്യ		srclen,
    പൂർണ്ണസംഖ്യ		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    എങ്കിൽ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	തിരികെ ശൂന്യ;

    buf = alloc(srclen + 1);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ ശൂന്യ;

    d = buf;
    end = src + srclen;
    വേണ്ടി (p = src; p < end; ++p)
    {
	എങ്കിൽ (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (പൂർണ്ണസംഖ്യ)(d - buf);
    തിരികെ buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    സ്ഥിതം char_u *
get_next_or_prev_match(പൂർണ്ണസംഖ്യ mode, expand_T *xp)
{
    പൂർണ്ണസംഖ്യ	    findex = xp->xp_selected;
    പൂർണ്ണസംഖ്യ	    ht;

    // When no matches found, return NULL
    എങ്കിൽ (xp->xp_numfiles <= 0)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	എങ്കിൽ (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (mode == WILD_NEXT)
    {
	// Select the next entry
	++findex;
    }
    അല്ലെങ്കിൽ   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	എങ്കിൽ (ht > 3)
	    ht -= 2;

	എങ്കിൽ (mode == WILD_PAGEUP)
	{
	    എങ്കിൽ (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    അല്ലെങ്കിൽ എങ്കിൽ (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    അല്ലെങ്കിൽ
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	അല്ലെങ്കിൽ    // mode == WILD_PAGEDOWN
	{
	    എങ്കിൽ (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    അല്ലെങ്കിൽ എങ്കിൽ (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    അല്ലെങ്കിൽ
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    എങ്കിൽ (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	എങ്കിൽ (xp->xp_orig != ശൂന്യ)
	    findex = -1;
	അല്ലെങ്കിൽ
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    എങ്കിൽ (p_wmnu)
    {
	എങ്കിൽ (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr(p_wop, WOP_PUM) != ശൂന്യ)
	{
	    എങ്കിൽ (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	അല്ലെങ്കിൽ
	    win_redr_status_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    എങ്കിൽ (findex == -1)
	തിരികെ vim_strsave(xp->xp_orig);
#എങ്കിൽ defined(FEAT_EVAL)
    തിരികെ apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#അല്ലെങ്കിൽ
    തിരികെ vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    സ്ഥിതം char_u *
ExpandOne_start(പൂർണ്ണസംഖ്യ mode, expand_T *xp, char_u *str, പൂർണ്ണസംഖ്യ options)
{
    പൂർണ്ണസംഖ്യ		non_suf_match;		// number without matching suffix
    പൂർണ്ണസംഖ്യ		i;
    char_u	*ss = ശൂന്യ;

    // Do the expansion.
    എങ്കിൽ (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	എങ്കിൽ (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_numfiles == 0)
    {
	എങ്കിൽ (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    അല്ലെങ്കിൽ
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	എങ്കിൽ (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    എങ്കിൽ (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    അല്ലെങ്കിൽ
		non_suf_match = 1;
	    എങ്കിൽ ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		വേണ്ടി (i = 0; i < 2; ++i)
		    എങ്കിൽ (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    എങ്കിൽ (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		എങ്കിൽ (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		അല്ലെങ്കിൽ എങ്കിൽ (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    എങ്കിൽ (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    തിരികെ ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    സ്ഥിതം char_u *
find_longest_match(expand_T *xp, പൂർണ്ണസംഖ്യ options)
{
    long_u	len;
    പൂർണ്ണസംഖ്യ		mb_len = 1;
    പൂർണ്ണസംഖ്യ		c0, ci;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*ss;

    വേണ്ടി (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	എങ്കിൽ (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	അല്ലെങ്കിൽ
	    c0 = xp->xp_files[0][len];
	വേണ്ടി (i = 1; i < xp->xp_numfiles; ++i)
	{
	    എങ്കിൽ (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    അല്ലെങ്കിൽ
		ci = xp->xp_files[i][len];
	    എങ്കിൽ (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		എങ്കിൽ (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c0 != ci)
		നിർത്തുക;
	}
	എങ്കിൽ (i < xp->xp_numfiles)
	{
	    എങ്കിൽ (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    നിർത്തുക;
	}
    }

    ss = alloc(len + 1);
    എങ്കിൽ (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    തിരികെ ss;
}

    ശൂന്യം
free_xp_files_extra(expand_T *xp, പൂർണ്ണസംഖ്യ numfiles)
{
    എങ്കിൽ (xp->xp_files_abbr != ശൂന്യ)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = ശൂന്യ;
    }
    എങ്കിൽ (xp->xp_files_kind != ശൂന്യ)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = ശൂന്യ;
    }
    എങ്കിൽ (xp->xp_files_menu != ശൂന്യ)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = ശൂന്യ;
    }
    എങ്കിൽ (xp->xp_files_info != ശൂന്യ)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = ശൂന്യ;
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
    പൂർണ്ണസംഖ്യ		options,
    പൂർണ്ണസംഖ്യ		mode)
{
    char_u	*ss = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		orig_saved = FALSE;
#എങ്കിൽ defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    പൂർണ്ണസംഖ്യ		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    എങ്കിൽ (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	തിരികെ get_next_or_prev_match(mode, xp);

    എങ്കിൽ (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    അല്ലെങ്കിൽ എങ്കിൽ (mode == WILD_APPLY)
    {
	എങ്കിൽ (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	അല്ലെങ്കിൽ
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#എങ്കിൽ defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    എങ്കിൽ (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	എങ്കിൽ (compl_match_array != ശൂന്യ)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    എങ്കിൽ (mode == WILD_FREE)	// only release file name
	തിരികെ ശൂന്യ;

    എങ്കിൽ (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#എങ്കിൽ defined(FEAT_EVAL)
	എങ്കിൽ (ss != ശൂന്യ)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    എങ്കിൽ (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#എങ്കിൽ defined(FEAT_EVAL)
	എങ്കിൽ (ss != ശൂന്യ)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    എങ്കിൽ (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	അക്ഷരം	*prefix = "";
	അക്ഷരം	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	പൂർണ്ണസംഖ്യ	n = xp->xp_numfiles - 1;
	പൂർണ്ണസംഖ്യ	i;
#എങ്കിൽ defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = ശൂന്യ;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	എങ്കിൽ ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    എങ്കിൽ (escaped != ശൂന്യ)
	    {
		വേണ്ടി (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    എങ്കിൽ (escaped[i] == ശൂന്യ)
		    {
			ആകുമ്പോൾ (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			നിർത്തുക;
		    }
		}
		എങ്കിൽ (escaped != ശൂന്യ)
		    files = escaped;
	    }
	}
#അല്ലെങ്കിൽ
	char_u	**files = xp->xp_files;
#endif

	എങ്കിൽ (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	വേണ്ടി (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	എങ്കിൽ (ss != ശൂന്യ)
	{
	    size_t  ss_len = 0;

	    വേണ്ടി (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (അക്ഷരം *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (അക്ഷരം *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#എങ്കിൽ defined(FEAT_EVAL)
	എങ്കിൽ (escaped != ശൂന്യ)
	{
	    വേണ്ടി (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    എങ്കിൽ (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    എങ്കിൽ (!orig_saved)
	vim_free(orig);

#എങ്കിൽ defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    എങ്കിൽ (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    തിരികെ ss;
}

/*
 * Prepare an expand structure for use.
 */
    ശൂന്യം
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
    ശൂന്യം
ExpandCleanup(expand_T *xp)
{
    എങ്കിൽ (xp->xp_numfiles >= 0)
    {
	free_xp_files_extra(xp, xp->xp_numfiles);
	FreeWild(xp->xp_numfiles, xp->xp_files);
	xp->xp_numfiles = -1;
    }
    VIM_CLEAR(xp->xp_orig);
}

    ശൂന്യം
clear_cmdline_orig(ശൂന്യം)
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
    സ്ഥിതം ശൂന്യം
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	പൂർണ്ണസംഖ്യ		അക്കം_Matches,
	പൂർണ്ണസംഖ്യ		വരികൾ,
	പൂർണ്ണസംഖ്യ		linenr,
	പൂർണ്ണസംഖ്യ		maxlen,
	പൂർണ്ണസംഖ്യ		showtail,
	പൂർണ്ണസംഖ്യ		dir_attr)
{
    പൂർണ്ണസംഖ്യ		i, j;
    പൂർണ്ണസംഖ്യ		isdir;
    പൂർണ്ണസംഖ്യ		lastlen;
    char_u	*p;

    lastlen = 999;
    വേണ്ടി (j = linenr; j < അക്കം_Matches; j += വരികൾ)
    {
	എങ്കിൽ (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    സന്ദേശം_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    സന്ദേശം_advance(maxlen + 1);
	    സന്ദേശം_puts((അക്ഷരം *)p);
	    സന്ദേശം_advance(maxlen + 3);
	    സന്ദേശം_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    നിർത്തുക;
	}
	വേണ്ടി (i = maxlen - lastlen; --i >= 0; )
	    സന്ദേശം_putchar(' ');
	എങ്കിൽ (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    എങ്കിൽ (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_path;
		char_u	*path;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_path = expand_env_save_opt(matches[j], TRUE, ശൂന്യ);
		path = exp_path != ശൂന്യ ? exp_path : matches[j];
		halved_slash = backslash_halve_save(path);
		isdir = mch_isdir(halved_slash != ശൂന്യ ? halved_slash
			: matches[j]);
		vim_free(exp_path);
		എങ്കിൽ (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    അല്ലെങ്കിൽ
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    എങ്കിൽ (showtail)
		p = SHOW_MATCH(j);
	    അല്ലെങ്കിൽ
	    {
		home_replace(ശൂന്യ, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = സന്ദേശം_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    എങ്കിൽ (സന്ദേശം_col > 0)	// when not wrapped around
    {
	സന്ദേശം_clr_eos();
	സന്ദേശം_putchar('\n');
    }
    out_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	പൂർണ്ണസംഖ്യ		അക്കം_Matches,
	പൂർണ്ണസംഖ്യ		showtail,
	പൂർണ്ണസംഖ്യ		noselect)
{
    പൂർണ്ണസംഖ്യ	retval = cmdline_pum_create(ccline, xp, matches, അക്കം_Matches, showtail);

    എങ്കിൽ (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    തിരികെ retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    പൂർണ്ണസംഖ്യ
showmatches(
    expand_T	*xp,
    പൂർണ്ണസംഖ്യ		display_wildmenu,
    പൂർണ്ണസംഖ്യ		display_list,
    പൂർണ്ണസംഖ്യ		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    പൂർണ്ണസംഖ്യ		അക്കം_Matches;
    char_u	**matches;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		maxlen;
    പൂർണ്ണസംഖ്യ		വരികൾ;
    പൂർണ്ണസംഖ്യ		columns;
    പൂർണ്ണസംഖ്യ		attr;
    പൂർണ്ണസംഖ്യ		showtail;
    പൂർണ്ണസംഖ്യ		noselect = (wim_flags_arg & WIM_NOSELECT);
    പൂർണ്ണസംഖ്യ		noinsert = (wim_flags_arg & WIM_NOINSERT);
    പൂർണ്ണസംഖ്യ		cmdline_unchanged = noselect || noinsert;

    എങ്കിൽ (xp->xp_numfiles == -1)
    {
	പൂർണ്ണസംഖ്യ retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&അക്കം_Matches, &matches);
	എങ്കിൽ (retval != EXPAND_OK)
	    തിരികെ retval;
	showtail = expand_showtail(xp);
    }
    അല്ലെങ്കിൽ
    {
	അക്കം_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    എങ്കിൽ (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != ശൂന്യ)
	തിരികെ show_pum_matches(ccline, xp, matches, അക്കം_Matches,
		showtail && !cmdline_unchanged, noselect);

    എങ്കിൽ (display_list)
    {
	സന്ദേശം_didany = FALSE;		// lines_left will be set
	സന്ദേശം_start();			// prepare for paging
	സന്ദേശം_putchar('\n');
	out_flush();
	cmdline_row = സന്ദേശം_row;
	സന്ദേശം_didany = FALSE;		// lines_left will be set again
	സന്ദേശം_start();			// prepare for paging
    }

    എങ്കിൽ (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    അല്ലെങ്കിൽ എങ്കിൽ (display_wildmenu && !display_list)
	win_redr_status_matches(xp, അക്കം_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    അല്ലെങ്കിൽ എങ്കിൽ (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	വേണ്ടി (i = 0; i < അക്കം_Matches; ++i)
	{
	    പൂർണ്ണസംഖ്യ	len;
	    എങ്കിൽ (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(ശൂന്യ, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    അല്ലെങ്കിൽ
		len = vim_strsize(SHOW_MATCH(i));
	    എങ്കിൽ (len > maxlen)
		maxlen = len;
	}

	എങ്കിൽ (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    വരികൾ = അക്കം_Matches;
	അല്ലെങ്കിൽ
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    എങ്കിൽ (columns < 1)
		columns = 1;
	    വരികൾ = (അക്കം_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	എങ്കിൽ (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    സന്ദേശം_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    സന്ദേശം_clr_eos();
	    സന്ദേശം_advance(maxlen - 3);
	    സന്ദേശം_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	വേണ്ടി (i = 0; i < വരികൾ; ++i)
	{
	    showmatches_oneline(xp, matches, അക്കം_Matches, വരികൾ, i,
						maxlen, showtail, attr);
	    എങ്കിൽ (got_int)
	    {
		got_int = FALSE;
		നിർത്തുക;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = സന്ദേശം_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    എങ്കിൽ (display_wildmenu && display_list)
    {
	എങ്കിൽ (vim_strchr(p_wop, WOP_PUM) != ശൂന്യ)
	    (ശൂന്യം)show_pum_matches(ccline, xp, matches, അക്കം_Matches,
		    showtail && !cmdline_unchanged, noselect);
	അല്ലെങ്കിൽ
	    win_redr_status_matches(xp, അക്കം_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    എങ്കിൽ (xp->xp_numfiles == -1)
    {
	FreeWild(അക്കം_Matches, matches);
	free_xp_files_extra(xp, അക്കം_Matches);
    }

    തിരികെ EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    സ്ഥിതം char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    പൂർണ്ണസംഖ്യ		had_sep = FALSE;

    വേണ്ടി (p = s; *p != NUL; )
    {
	എങ്കിൽ (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    തിരികെ t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    എങ്കിൽ (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	തിരികെ FALSE;

    end = gettail(xp->xp_pattern);
    എങ്കിൽ (end == xp->xp_pattern)		// there is no path separator
	തിരികെ FALSE;

    വേണ്ടി (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	എങ്കിൽ (rem_backslash(s))
	    ++s;
	അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr((char_u *)"*?[", *s) != ശൂന്യ)
	    തിരികെ FALSE;
    }
    തിരികെ TRUE;
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
    പൂർണ്ണസംഖ്യ		len,
    പൂർണ്ണസംഖ്യ		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    പൂർണ്ണസംഖ്യ		i, j;
    പൂർണ്ണസംഖ്യ		new_len;
    char_u	*tail;
    പൂർണ്ണസംഖ്യ		ends_in_star;

    എങ്കിൽ (context != EXPAND_FILES
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
	എങ്കിൽ (context == EXPAND_FINDFUNC
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
	അല്ലെങ്കിൽ
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    വേണ്ടി (i = 0; i < len; i++)
	    {
		എങ്കിൽ (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		എങ്കിൽ (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		എങ്കിൽ ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    എങ്കിൽ (retval != ശൂന്യ)
	    {
		retval[0] = '^';
		j = 1;
		വേണ്ടി (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    എങ്കിൽ (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			നിർത്തുക;

		    തിരഞ്ഞെടുപ്പ് (fname[i])
		    {
			അവസ്ഥ '*':   retval[j++] = '.';
				    നിർത്തുക;
			അവസ്ഥ '~':   retval[j++] = '\\';
				    നിർത്തുക;
			അവസ്ഥ '?':   retval[j] = '.';
				    തുടരുക;
			അവസ്ഥ '.':   എങ്കിൽ (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    നിർത്തുക;
			അവസ്ഥ '\\':  എങ്കിൽ (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    നിർത്തുക;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	retval = alloc(len + 4);
	എങ്കിൽ (retval != ശൂന്യ)
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
	    വേണ്ടി (i = len - 2; i >= 0; --i)
	    {
		എങ്കിൽ (retval[i] != '\\')
		    നിർത്തുക;
		ends_in_star = !ends_in_star;
	    }
#endif
	    എങ്കിൽ ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == ശൂന്യ
		    && vim_strchr(retval, '`') == ശൂന്യ)
		retval[len++] = '*';
	    അല്ലെങ്കിൽ എങ്കിൽ (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    തിരികെ retval;
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
    ശൂന്യം
set_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    എങ്കിൽ ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_തിരയൽ_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_തിരയൽ_EXTRA
	തിരയൽ_first_വരി = 0; // Search entire buffer
#endif
	തിരികെ;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    എങ്കിൽ (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->നിവേശം_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	തിരികെ;
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
    സ്ഥിതം char_u *
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, പൂർണ്ണസംഖ്യ *complp)
{
    char_u	*p = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len = 0;
    പൂർണ്ണസംഖ്യ		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    എങ്കിൽ (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    അല്ലെങ്കിൽ
    {
	p = cmd;
	ആകുമ്പോൾ (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	എങ്കിൽ (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    ആകുമ്പോൾ (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	എങ്കിൽ (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    ആകുമ്പോൾ (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	എങ്കിൽ (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != ശൂന്യ)
	    ++p;
	len = (പൂർണ്ണസംഖ്യ)(p - cmd);

	എങ്കിൽ (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    തിരികെ ശൂന്യ;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	എങ്കിൽ ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    ആകുമ്പോൾ (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    എങ്കിൽ (*p == NUL && ASCII_ISALNUM(p[-1]))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (eap->cmdidx == CMD_SIZE)
    {
	എങ്കിൽ (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != ശൂന്യ)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, ശൂന്യ, xp, complp);
	    എങ്കിൽ (p == ശൂന്യ)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    എങ്കിൽ (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	തിരികെ ശൂന്യ;
    }

    തിരികെ p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    സ്ഥിതം ശൂന്യം
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	പൂർണ്ണസംഖ്യ		usefilter,
	expand_T	*xp,
	പൂർണ്ണസംഖ്യ		*complp)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		in_quote = FALSE;
    char_u	*bow = ശൂന്യ;	// Beginning of word
    പൂർണ്ണസംഖ്യ		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    ആകുമ്പോൾ (*p != NUL)
    {
	എങ്കിൽ (has_mbyte)
	    c = mb_ptr2char(p);
	അല്ലെങ്കിൽ
	    c = *p;
	എങ്കിൽ (c == '\\' && p[1] != NUL)
	    ++p;
	അല്ലെങ്കിൽ എങ്കിൽ (c == '`')
	{
	    എങ്കിൽ (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	അല്ലെങ്കിൽ എങ്കിൽ (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != ശൂന്യ && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    ആകുമ്പോൾ (*p != NUL)
	    {
		എങ്കിൽ (has_mbyte)
		    c = mb_ptr2char(p);
		അല്ലെങ്കിൽ
		    c = *p;
		എങ്കിൽ (c == '`' || vim_isfilec_or_wc(c))
		    നിർത്തുക;
		എങ്കിൽ (has_mbyte)
		    len = (*mb_ptr2len)(p);
		അല്ലെങ്കിൽ
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    എങ്കിൽ (in_quote)
		bow = p;
	    അല്ലെങ്കിൽ
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    എങ്കിൽ (bow != ശൂന്യ && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    എങ്കിൽ (usefilter
	    || (eap != ശൂന്യ
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	എങ്കിൽ (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    എങ്കിൽ (*xp->xp_pattern == '$')
    {
	വേണ്ടി (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    എങ്കിൽ (!vim_isIDc(*p))
		നിർത്തുക;
	എങ്കിൽ (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    എങ്കിൽ (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    എങ്കിൽ (*xp->xp_pattern == '~')
    {
	വേണ്ടി (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	എങ്കിൽ (*p == NUL && p > xp->xp_pattern + 1
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
    സ്ഥിതം char_u *
set_context_in_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    എങ്കിൽ (p == ശൂന്യ)
	xp->xp_pattern = arg;
    അല്ലെങ്കിൽ
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    തിരികെ ശൂന്യ;
}

#ifdef FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    സ്ഥിതം char_u *
set_context_in_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    എങ്കിൽ (p == ശൂന്യ)
	xp->xp_pattern = arg;
    അല്ലെങ്കിൽ
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    സ്ഥിതം char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    എങ്കിൽ (*arg != NUL)
	arg = skip_vimgrep_pat(arg, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (arg == ശൂന്യ || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	തിരികെ ശൂന്യ;
    }
    തിരികെ skipwhite(arg);
}

#ifdef FEAT_തിരയൽ_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    സ്ഥിതം char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    എങ്കിൽ (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	എങ്കിൽ (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    തിരികെ find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    സ്ഥിതം char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    പൂർണ്ണസംഖ്യ		delim;

    delim = *arg;	    // get the delimiter
    എങ്കിൽ (delim)
	++arg;		    // skip delimiter if there is one

    ആകുമ്പോൾ (arg[0] != NUL && arg[0] != delim)
    {
	എങ്കിൽ (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    എങ്കിൽ (arg[0] != NUL)
	തിരികെ arg + 1;

    തിരികെ ശൂന്യ;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    സ്ഥിതം char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    പൂർണ്ണസംഖ്യ		delim;

    delim = *arg;
    എങ്കിൽ (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	എങ്കിൽ (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    ആകുമ്പോൾ (arg[0] != NUL && arg[0] != delim)
	    {
		എങ്കിൽ (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    എങ്കിൽ (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    ആകുമ്പോൾ (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == ശൂന്യ)
	++arg;
    എങ്കിൽ (arg[0] != NUL)
	തിരികെ arg;

    തിരികെ ശൂന്യ;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    സ്ഥിതം char_u *
find_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    എങ്കിൽ (*arg != '/')
	തിരികെ ശൂന്യ;

    // Match regexp, not just whole words
    വേണ്ടി (++arg; *arg && *arg != '/'; arg++)
	എങ്കിൽ (*arg == '\\' && arg[1] != NUL)
	    arg++;
    എങ്കിൽ (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	എങ്കിൽ (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == ശൂന്യ)
	    xp->xp_context = EXPAND_NOTHING;
	അല്ലെങ്കിൽ
	    തിരികെ arg;
    }

    തിരികെ ശൂന്യ;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    സ്ഥിതം char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    ആകുമ്പോൾ ((xp->xp_pattern = vim_strchr(arg, ' ')) != ശൂന്യ)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    എങ്കിൽ (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    സ്ഥിതം char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    എങ്കിൽ (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_pattern = skipwhite(p);
	}
	അല്ലെങ്കിൽ
	    xp->xp_context = EXPAND_NOTHING;
    }

    തിരികെ ശൂന്യ;
}
#endif

സ്ഥിതം എണ്ണൽ
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
സ്ഥിതം എണ്ണൽ
{
    EXP_BREAKPT_ADD,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    സ്ഥിതം char_u *
set_context_in_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_start;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_pattern = arg;

    എങ്കിൽ (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    അല്ലെങ്കിൽ എങ്കിൽ (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    അല്ലെങ്കിൽ
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    എങ്കിൽ (*p == NUL)
	തിരികെ ശൂന്യ;
    subcmd_start = p;

    എങ്കിൽ (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	എങ്കിൽ (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    എങ്കിൽ (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		തിരികെ ശൂന്യ;
	    }
	    p = skipwhite(p);
	}
	എങ്കിൽ (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	അല്ലെങ്കിൽ
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    തിരികെ ശൂന്യ;
}

    സ്ഥിതം char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = ശൂന്യ;

    p = skipwhite(arg);
    എങ്കിൽ (VIM_ISDIGIT(*p))
	തിരികെ ശൂന്യ;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    തിരികെ ശൂന്യ;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    സ്ഥിതം char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    പൂർണ്ണസംഖ്യ    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    എങ്കിൽ (*p == NUL)
	തിരികെ ശൂന്യ;

    വേണ്ടി (;;)
    {
	എങ്കിൽ (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    തുടരുക;
	}
	എങ്കിൽ (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    തുടരുക;
	}
	നിർത്തുക;
    }

    എങ്കിൽ ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    അല്ലെങ്കിൽ എങ്കിൽ ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    അല്ലെങ്കിൽ എങ്കിൽ ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    തിരികെ ശൂന്യ;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    സ്ഥിതം ശൂന്യം
set_context_with_pattern(expand_T *xp)
{
    പൂർണ്ണസംഖ്യ		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_തിരയൽ_EXTRA
    പൂർണ്ണസംഖ്യ		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    എങ്കിൽ (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	തിരികെ;
#endif

    xp->xp_pattern = ccline->cmdbuff + skiplen;
    xp->xp_pattern_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_PATTERN_IN_BUF;
    xp->xp_തിരയൽ_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    സ്ഥിതം char_u *
set_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	long		argt,
	പൂർണ്ണസംഖ്യ		compl,
	പൂർണ്ണസംഖ്യ		forceit)
{
    char_u  *nextcmd;

    തിരഞ്ഞെടുപ്പ് (cmdidx)
    {
	അവസ്ഥ CMD_find:
	അവസ്ഥ CMD_sfind:
	അവസ്ഥ CMD_tabfind:
	    എങ്കിൽ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    നിർത്തുക;
	അവസ്ഥ CMD_cd:
	അവസ്ഥ CMD_chdir:
	അവസ്ഥ CMD_tcd:
	അവസ്ഥ CMD_tchdir:
	അവസ്ഥ CMD_lcd:
	അവസ്ഥ CMD_lchdir:
	    എങ്കിൽ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    നിർത്തുക;
	അവസ്ഥ CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    നിർത്തുക;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	അവസ്ഥ CMD_aboveleft:
	അവസ്ഥ CMD_argdo:
	അവസ്ഥ CMD_belowright:
	അവസ്ഥ CMD_botright:
	അവസ്ഥ CMD_browse:
	അവസ്ഥ CMD_bufdo:
	അവസ്ഥ CMD_cdo:
	അവസ്ഥ CMD_cfdo:
	അവസ്ഥ CMD_confirm:
	അവസ്ഥ CMD_debug:
	അവസ്ഥ CMD_folddoclosed:
	അവസ്ഥ CMD_folddoopen:
	അവസ്ഥ CMD_hide:
	അവസ്ഥ CMD_horizontal:
	അവസ്ഥ CMD_keepalt:
	അവസ്ഥ CMD_keepjumps:
	അവസ്ഥ CMD_keepmarks:
	അവസ്ഥ CMD_keeppatterns:
	അവസ്ഥ CMD_ldo:
	അവസ്ഥ CMD_leftabove:
	അവസ്ഥ CMD_lfdo:
	അവസ്ഥ CMD_lockmarks:
	അവസ്ഥ CMD_noautocmd:
	അവസ്ഥ CMD_noswapfile:
	അവസ്ഥ CMD_rightbelow:
	അവസ്ഥ CMD_sandbox:
	അവസ്ഥ CMD_silent:
	അവസ്ഥ CMD_tab:
	അവസ്ഥ CMD_tabdo:
	അവസ്ഥ CMD_topleft:
	അവസ്ഥ CMD_unsilent:
	അവസ്ഥ CMD_verbose:
	അവസ്ഥ CMD_vertical:
	അവസ്ഥ CMD_windo:
	അവസ്ഥ CMD_vim9cmd:
	അവസ്ഥ CMD_legacy:
	    തിരികെ arg;

	അവസ്ഥ CMD_filter:
	    തിരികെ set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_തിരയൽ_EXTRA
	അവസ്ഥ CMD_match:
	    തിരികെ set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	അവസ്ഥ CMD_command:
	    തിരികെ set_context_in_user_cmd(xp, arg);

	അവസ്ഥ CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    നിർത്തുക;

	അവസ്ഥ CMD_global:
	അവസ്ഥ CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    എങ്കിൽ (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    തിരികെ nextcmd;

	അവസ്ഥ CMD_and:
	അവസ്ഥ CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    എങ്കിൽ (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    തിരികെ nextcmd;

	അവസ്ഥ CMD_isearch:
	അവസ്ഥ CMD_dsearch:
	അവസ്ഥ CMD_ilist:
	അവസ്ഥ CMD_dlist:
	അവസ്ഥ CMD_ijump:
	അവസ്ഥ CMD_psearch:
	അവസ്ഥ CMD_djump:
	അവസ്ഥ CMD_isplit:
	അവസ്ഥ CMD_dsplit:
	    തിരികെ find_cmd_after_isearch_cmd(xp, arg);
	അവസ്ഥ CMD_autocmd:
	    തിരികെ set_context_in_autocmd(xp, arg, FALSE);
	അവസ്ഥ CMD_doautocmd:
	അവസ്ഥ CMD_doautoall:
	    തിരികെ set_context_in_autocmd(xp, arg, TRUE);
	അവസ്ഥ CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    നിർത്തുക;
	അവസ്ഥ CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    നിർത്തുക;
	അവസ്ഥ CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    നിർത്തുക;
	അവസ്ഥ CMD_tag:
	അവസ്ഥ CMD_stag:
	അവസ്ഥ CMD_ptag:
	അവസ്ഥ CMD_ltag:
	അവസ്ഥ CMD_tselect:
	അവസ്ഥ CMD_stselect:
	അവസ്ഥ CMD_ptselect:
	അവസ്ഥ CMD_tjump:
	അവസ്ഥ CMD_stjump:
	അവസ്ഥ CMD_ptjump:
	    എങ്കിൽ (vim_strchr(p_wop, WOP_TAGFILE) != ശൂന്യ)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    അല്ലെങ്കിൽ
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    നിർത്തുക;
	അവസ്ഥ CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    നിർത്തുക;
#ifdef FEAT_SYN_HL
	അവസ്ഥ CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    നിർത്തുക;
#endif
#ifdef FEAT_EVAL
	അവസ്ഥ CMD_final:
	അവസ്ഥ CMD_const:
	അവസ്ഥ CMD_let:
	അവസ്ഥ CMD_var:
	അവസ്ഥ CMD_if:
	അവസ്ഥ CMD_elseif:
	അവസ്ഥ CMD_while:
	അവസ്ഥ CMD_for:
	അവസ്ഥ CMD_echo:
	അവസ്ഥ CMD_echon:
	അവസ്ഥ CMD_execute:
	അവസ്ഥ CMD_echomsg:
	അവസ്ഥ CMD_echoerr:
	അവസ്ഥ CMD_echoconsole:
	അവസ്ഥ CMD_echowindow:
	അവസ്ഥ CMD_call:
	അവസ്ഥ CMD_return:
	അവസ്ഥ CMD_cexpr:
	അവസ്ഥ CMD_caddexpr:
	അവസ്ഥ CMD_cgetexpr:
	അവസ്ഥ CMD_lexpr:
	അവസ്ഥ CMD_laddexpr:
	അവസ്ഥ CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    നിർത്തുക;

	അവസ്ഥ CMD_unlet:
	    തിരികെ set_context_in_unlet_cmd(xp, arg);
	അവസ്ഥ CMD_function:
	അവസ്ഥ CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    നിർത്തുക;
	അവസ്ഥ CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    നിർത്തുക;

	അവസ്ഥ CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    നിർത്തുക;
#endif
	അവസ്ഥ CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    നിർത്തുക;
#ifdef FEAT_CSCOPE
	അവസ്ഥ CMD_cscope:
	അവസ്ഥ CMD_lcscope:
	അവസ്ഥ CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    നിർത്തുക;
#endif
#ifdef FEAT_SIGNS
	അവസ്ഥ CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    നിർത്തുക;
#endif
	അവസ്ഥ CMD_bdelete:
	അവസ്ഥ CMD_bwipeout:
	അവസ്ഥ CMD_bunload:
	    ആകുമ്പോൾ ((xp->xp_pattern = vim_strchr(arg, ' ')) != ശൂന്യ)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	അവസ്ഥ CMD_buffer:
	അവസ്ഥ CMD_sbuffer:
	അവസ്ഥ CMD_pbuffer:
	അവസ്ഥ CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    നിർത്തുക;
#ifdef FEAT_DIFF
	അവസ്ഥ CMD_diffget:
	അവസ്ഥ CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    നിർത്തുക;
#endif
	അവസ്ഥ CMD_USER:
	അവസ്ഥ CMD_USER_BUF:
	    തിരികെ set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	അവസ്ഥ CMD_map:	    അവസ്ഥ CMD_noremap:
	അവസ്ഥ CMD_nmap:	    അവസ്ഥ CMD_nnoremap:
	അവസ്ഥ CMD_vmap:	    അവസ്ഥ CMD_vnoremap:
	അവസ്ഥ CMD_omap:	    അവസ്ഥ CMD_onoremap:
	അവസ്ഥ CMD_imap:	    അവസ്ഥ CMD_inoremap:
	അവസ്ഥ CMD_cmap:	    അവസ്ഥ CMD_cnoremap:
	അവസ്ഥ CMD_lmap:	    അവസ്ഥ CMD_lnoremap:
	അവസ്ഥ CMD_smap:	    അവസ്ഥ CMD_snoremap:
	അവസ്ഥ CMD_tmap:	    അവസ്ഥ CMD_tnoremap:
	അവസ്ഥ CMD_xmap:	    അവസ്ഥ CMD_xnoremap:
	    തിരികെ set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	അവസ്ഥ CMD_unmap:
	അവസ്ഥ CMD_nunmap:
	അവസ്ഥ CMD_vunmap:
	അവസ്ഥ CMD_ounmap:
	അവസ്ഥ CMD_iunmap:
	അവസ്ഥ CMD_cunmap:
	അവസ്ഥ CMD_lunmap:
	അവസ്ഥ CMD_sunmap:
	അവസ്ഥ CMD_tunmap:
	അവസ്ഥ CMD_xunmap:
	    തിരികെ set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	അവസ്ഥ CMD_mapclear:
	അവസ്ഥ CMD_nmapclear:
	അവസ്ഥ CMD_vmapclear:
	അവസ്ഥ CMD_omapclear:
	അവസ്ഥ CMD_imapclear:
	അവസ്ഥ CMD_cmapclear:
	അവസ്ഥ CMD_lmapclear:
	അവസ്ഥ CMD_smapclear:
	അവസ്ഥ CMD_tmapclear:
	അവസ്ഥ CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    നിർത്തുക;

	അവസ്ഥ CMD_abbreviate:	അവസ്ഥ CMD_noreabbrev:
	അവസ്ഥ CMD_cabbrev:	അവസ്ഥ CMD_cnoreabbrev:
	അവസ്ഥ CMD_iabbrev:	അവസ്ഥ CMD_inoreabbrev:
	    തിരികെ set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	അവസ്ഥ CMD_unabbreviate:
	അവസ്ഥ CMD_cunabbrev:
	അവസ്ഥ CMD_iunabbrev:
	    തിരികെ set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	അവസ്ഥ CMD_menu:	    അവസ്ഥ CMD_noremenu:	    അവസ്ഥ CMD_unmenu:
	അവസ്ഥ CMD_amenu:	    അവസ്ഥ CMD_anoremenu:	    അവസ്ഥ CMD_aunmenu:
	അവസ്ഥ CMD_nmenu:	    അവസ്ഥ CMD_nnoremenu:	    അവസ്ഥ CMD_nunmenu:
	അവസ്ഥ CMD_vmenu:	    അവസ്ഥ CMD_vnoremenu:	    അവസ്ഥ CMD_vunmenu:
	അവസ്ഥ CMD_omenu:	    അവസ്ഥ CMD_onoremenu:	    അവസ്ഥ CMD_ounmenu:
	അവസ്ഥ CMD_imenu:	    അവസ്ഥ CMD_inoremenu:	    അവസ്ഥ CMD_iunmenu:
	അവസ്ഥ CMD_cmenu:	    അവസ്ഥ CMD_cnoremenu:	    അവസ്ഥ CMD_cunmenu:
	അവസ്ഥ CMD_tlmenu:    അവസ്ഥ CMD_tlnoremenu:    അവസ്ഥ CMD_tlunmenu:
	അവസ്ഥ CMD_tmenu:				    അവസ്ഥ CMD_tunmenu:
	അവസ്ഥ CMD_popup:	    അവസ്ഥ CMD_tearoff:	    അവസ്ഥ CMD_emenu:
	    തിരികെ set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	അവസ്ഥ CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    നിർത്തുക;

	അവസ്ഥ CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    നിർത്തുക;

	അവസ്ഥ CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    നിർത്തുക;

	അവസ്ഥ CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    നിർത്തുക;

	അവസ്ഥ CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    നിർത്തുക;

	അവസ്ഥ CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    നിർത്തുക;

#എങ്കിൽ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	അവസ്ഥ CMD_language:
	    തിരികെ set_context_in_lang_cmd(xp, arg);
#endif
#എങ്കിൽ defined(FEAT_PROFILE)
	അവസ്ഥ CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    നിർത്തുക;
#endif
	അവസ്ഥ CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    നിർത്തുക;

	അവസ്ഥ CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    നിർത്തുക;

	അവസ്ഥ CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    നിർത്തുക;

	അവസ്ഥ CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    നിർത്തുക;
#എങ്കിൽ defined(FEAT_PROFILE)
	അവസ്ഥ CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    നിർത്തുക;
#endif

	അവസ്ഥ CMD_argdelete:
	    ആകുമ്പോൾ ((xp->xp_pattern = vim_strchr(arg, ' ')) != ശൂന്യ)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    നിർത്തുക;

#ifdef FEAT_EVAL
	അവസ്ഥ CMD_breakadd:
	അവസ്ഥ CMD_profdel:
	അവസ്ഥ CMD_breakdel:
	    തിരികെ set_context_in_breakadd_cmd(xp, arg, cmdidx);

	അവസ്ഥ CMD_scriptnames:
	    തിരികെ set_context_in_scriptnames_cmd(xp, arg);
#endif
	അവസ്ഥ CMD_filetype:
	    തിരികെ set_context_in_filetype_cmd(xp, arg);

	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
    തിരികെ ശൂന്യ;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    സ്ഥിതം char_u *
set_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    പൂർണ്ണസംഖ്യ			len = 0;
    exarg_T		ea;
    പൂർണ്ണസംഖ്യ			compl = EXPAND_NOTHING;
    പൂർണ്ണസംഖ്യ			forceit = FALSE;
    പൂർണ്ണസംഖ്യ			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_വരി = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    വേണ്ടി (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != ശൂന്യ; cmd++)
	;
    xp->xp_pattern = cmd;

    എങ്കിൽ (*cmd == NUL)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	തിരികെ ശൂന്യ;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    എങ്കിൽ (*cmd == NUL)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (*cmd == '|' || *cmd == '\n')
	തിരികെ cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ ശൂന്യ;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    എങ്കിൽ (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    എങ്കിൽ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    എങ്കിൽ ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	ആകുമ്പോൾ (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    ആകുമ്പോൾ (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    എങ്കിൽ (*p == NUL)
	    {
		എങ്കിൽ (ea.argt & EX_ARGOPT)
		    തിരികെ set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		എങ്കിൽ (ea.cmdidx == CMD_terminal)
		    തിരികെ set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    എങ്കിൽ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	എങ്കിൽ (*arg == '>')			// append
	{
	    എങ്കിൽ (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    എങ്കിൽ (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	എങ്കിൽ (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    എങ്കിൽ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ആകുമ്പോൾ (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    എങ്കിൽ ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	എങ്കിൽ (*arg == NUL)
	    തിരികെ p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    എങ്കിൽ ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	എങ്കിൽ (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	ആകുമ്പോൾ (*p)
	{
	    എങ്കിൽ (*p == Ctrl_V)
	    {
		എങ്കിൽ (p[1] != NUL)
		    ++p;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		എങ്കിൽ (*(p - 1) != '\\')
		{
		    എങ്കിൽ (*p == '|' || *p == '\n')
			തിരികെ p + 1;
		    തിരികെ ശൂന്യ;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    എങ്കിൽ (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == ശൂന്യ)
	// no arguments allowed but there is something
	തിരികെ ശൂന്യ;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (പൂർണ്ണസംഖ്യ)STRLEN(buff);
    ആകുമ്പോൾ (*p && p < buff + len)
    {
	എങ്കിൽ (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    എങ്കിൽ (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    തിരികെ set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    ശൂന്യം
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    പൂർണ്ണസംഖ്യ		len,	    // length of command line (excl. NUL)
    പൂർണ്ണസംഖ്യ		col,	    // position of cursor
    പൂർണ്ണസംഖ്യ		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    പൂർണ്ണസംഖ്യ			context;
#endif
    പൂർണ്ണസംഖ്യ		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    എങ്കിൽ (col < len)
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
    എങ്കിൽ (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (use_ccline && ccline->നിവേശം_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	എങ്കിൽ (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(ശൂന്യ, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    അല്ലെങ്കിൽ
#endif
	ആകുമ്പോൾ (nextcomm != ശൂന്യ)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_വരി = str;
    xp->xp_col = col;

    എങ്കിൽ (col < len)
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
    പൂർണ്ണസംഖ്യ
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    പൂർണ്ണസംഖ്യ		col,		// position of cursor
    പൂർണ്ണസംഖ്യ		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		options = WILD_ADD_SLASH|WILD_SILENT;

    എങ്കിൽ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	തിരികെ EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    എങ്കിൽ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	തിരികെ EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (പൂർണ്ണസംഖ്യ)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	പൂർണ്ണസംഖ്യ	pat_len = xp->xp_pattern_len;
#എങ്കിൽ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	എങ്കിൽ (unesc != ശൂന്യ)
	    pat_src = unesc;
#endif

	എങ്കിൽ (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	അല്ലെങ്കിൽ
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#എങ്കിൽ defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    എങ്കിൽ (file_str == ശൂന്യ)
	തിരികെ EXPAND_UNSUCCESSFUL;

    എങ്കിൽ (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    എങ്കിൽ (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = ശൂന്യ;
    }
    vim_free(file_str);

    തിരികെ EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	പൂർണ്ണസംഖ്യ		*അക്കം_Matches,
	പൂർണ്ണസംഖ്യ		flags,
	പൂർണ്ണസംഖ്യ		options)
{
    പൂർണ്ണസംഖ്യ		free_pat = FALSE;
    പൂർണ്ണസംഖ്യ		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    എങ്കിൽ (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	എങ്കിൽ (pat == ശൂന്യ)
	    തിരികെ ret;

	pat_end = pat + pat_len;
	വേണ്ടി (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    എങ്കിൽ (*p != '\\')
		തുടരുക;

	    എങ്കിൽ (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_backslash & XP_BS_COMMA)
	    {
		എങ്കിൽ (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		അല്ലെങ്കിൽ എങ്കിൽ (*(p + 1) == ',')
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

    എങ്കിൽ (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, അക്കം_Matches);
#endif
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	അല്ലെങ്കിൽ
	    flags = (flags | EW_DIR) & ~EW_FILE;
	എങ്കിൽ (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, അക്കം_Matches, matches, flags);
    }
    എങ്കിൽ (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    എങ്കിൽ (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	പൂർണ്ണസംഖ്യ j;

	വേണ്ടി (j = 0; j < *അക്കം_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    ആകുമ്പോൾ (*ptr != NUL)
	    {
		എങ്കിൽ (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		അല്ലെങ്കിൽ എങ്കിൽ (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    തിരികെ ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    സ്ഥിതം char_u *
get_behave_arg(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx == 0)
	തിരികെ (char_u *)"mswin";
    എങ്കിൽ (idx == 1)
	തിരികെ (char_u *)"xterm";
    തിരികെ ശൂന്യ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    സ്ഥിതം char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx < 0)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	അക്ഷരം	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	തിരികെ (char_u *)opts_all[idx];
    }
    എങ്കിൽ (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	അക്ഷരം	*opts_plugin[] = {"plugin", "on", "off"};
	തിരികെ (char_u *)opts_plugin[idx];
    }
    എങ്കിൽ (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	അക്ഷരം	*opts_indent[] = {"indent", "on", "off"};
	തിരികെ (char_u *)opts_indent[idx];
    }
    എങ്കിൽ (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	അക്ഷരം	*opts_onoff[] = {"on", "off"};
	തിരികെ (char_u *)opts_onoff[idx];
    }
    തിരികെ ശൂന്യ;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    സ്ഥിതം char_u *
get_breakadd_arg(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx >= 0 && idx <= 3)
    {
	അക്ഷരം	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	എങ്കിൽ (breakpt_expand_what == EXP_BREAKPT_ADD)
	    തിരികെ (char_u *)opts[idx];
	അല്ലെങ്കിൽ എങ്കിൽ (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    എങ്കിൽ (idx <= 2)
		തിരികെ (char_u *)opts[idx + 1];
	}
	അല്ലെങ്കിൽ
	{
	    // profdel {func, file}
	    എങ്കിൽ (idx <= 1)
		തിരികെ (char_u *)opts[idx + 1];
	}
    }
    തിരികെ ശൂന്യ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    സ്ഥിതം char_u *
get_scriptnames_arg(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    scriptitem_T *si;

    എങ്കിൽ (!SCRIPT_ID_VALID(idx + 1))
	തിരികെ ശൂന്യ;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(ശൂന്യ, si->sn_name, NameBuff, MAXPATHL, TRUE);
    തിരികെ NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    സ്ഥിതം char_u *
get_retab_arg(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx == 0)
	തിരികെ (char_u *)"-indentonly";
    തിരികെ ശൂന്യ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    സ്ഥിതം char_u *
get_messages_arg(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx == 0)
	തിരികെ (char_u *)"clear";
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം char_u *
get_mapclear_arg(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx == 0)
	തിരികെ (char_u *)"<buffer>";
    തിരികെ ശൂന്യ;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	പൂർണ്ണസംഖ്യ		*അക്കം_Matches)
{
    സ്ഥിതം ഘടന expgen
    {
	പൂർണ്ണസംഖ്യ		context;
	char_u	*((*func)(expand_T *, പൂർണ്ണസംഖ്യ));
	പൂർണ്ണസംഖ്യ		ic;
	പൂർണ്ണസംഖ്യ		escaped;
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
	{EXPAND_EXPRESSION, get_വ്യഞ്ജകം_name, FALSE, TRUE},
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
#എങ്കിൽ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
    പൂർണ്ണസംഖ്യ	i;
    പൂർണ്ണസംഖ്യ ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(tab); ++i)
    {
	എങ്കിൽ (xp->xp_context == tab[i].context)
	{
	    എങ്കിൽ (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, അക്കം_Matches,
						tab[i].func, tab[i].escaped);
	    നിർത്തുക;
	}
    }

    തിരികെ ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
map_wildopts_to_ewflags(പൂർണ്ണസംഖ്യ options)
{
    പൂർണ്ണസംഖ്യ		flags;

    flags = EW_DIR;	// include directories
    എങ്കിൽ (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    എങ്കിൽ (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    എങ്കിൽ (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    എങ്കിൽ (options & WILD_SILENT)
	flags |= EW_SILENT;
    എങ്കിൽ (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    എങ്കിൽ (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    തിരികെ flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches,
    പൂർണ്ണസംഖ്യ		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		ret;
    പൂർണ്ണസംഖ്യ		flags;
    char_u	*tofree = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    എങ്കിൽ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	തിരികെ expand_files_and_dirs(xp, pat, matches, അക്കം_Matches, flags,
								options);

    *matches = (char_u **)"";
    *അക്കം_Matches = 0;
    എങ്കിൽ (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	എങ്കിൽ (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					അക്കം_Matches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*അക്കം_Matches, *matches);
#endif
	    തിരികെ OK;
	}
	തിരികെ FAIL;
    }

    എങ്കിൽ (xp->xp_context == EXPAND_SHELLCMD)
	തിരികെ expand_shellcmd(pat, matches, അക്കം_Matches, flags);
    എങ്കിൽ (xp->xp_context == EXPAND_OLD_SETTING)
	തിരികെ ExpandOldSetting(അക്കം_Matches, matches);
    എങ്കിൽ (xp->xp_context == EXPAND_BUFFERS)
	തിരികെ ExpandBufnames(pat, അക്കം_Matches, matches, options);
#ifdef FEAT_DIFF
    എങ്കിൽ (xp->xp_context == EXPAND_DIFF_BUFFERS)
	തിരികെ ExpandBufnames(pat, അക്കം_Matches, matches,
						options | BUF_DIFF_FILTER);
#endif
    എങ്കിൽ (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	തിരികെ expand_tags(xp->xp_context == EXPAND_TAGS, pat, അക്കം_Matches,
								matches);
    എങ്കിൽ (xp->xp_context == EXPAND_COLORS)
    {
	അക്ഷരം *directories[] = {"colors", ശൂന്യ};
	തിരികെ ExpandRTDir(pat, DIP_START + DIP_OPT, അക്കം_Matches, matches,
								directories);
    }
    എങ്കിൽ (xp->xp_context == EXPAND_COMPILER)
    {
	അക്ഷരം *directories[] = {"compiler", ശൂന്യ};
	തിരികെ ExpandRTDir(pat, 0, അക്കം_Matches, matches, directories);
    }
    എങ്കിൽ (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	അക്ഷരം *directories[] = {"syntax", ശൂന്യ};
	തിരികെ ExpandRTDir(pat, 0, അക്കം_Matches, matches, directories);
    }
    എങ്കിൽ (xp->xp_context == EXPAND_FILETYPE)
    {
	അക്ഷരം *directories[] = {"syntax", "indent", "ftplugin", ശൂന്യ};
	തിരികെ ExpandRTDir(pat, 0, അക്കം_Matches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    എങ്കിൽ (xp->xp_context == EXPAND_KEYMAP)
    {
	അക്ഷരം *directories[] = {"keymap", ശൂന്യ};
	തിരികെ ExpandRTDir(pat, 0, അക്കം_Matches, matches, directories);
    }
#endif
#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ (xp->xp_context == EXPAND_USER_LIST)
	തിരികെ ExpandUserList(xp, matches, അക്കം_Matches);
#endif
    എങ്കിൽ (xp->xp_context == EXPAND_PACKADD)
	തിരികെ ExpandPackAddDir(pat, അക്കം_Matches, matches);
    എങ്കിൽ (xp->xp_context == EXPAND_RUNTIME)
	തിരികെ expand_runtime_cmd(pat, അക്കം_Matches, matches);
    എങ്കിൽ (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	തിരികെ expand_pattern_in_buf(pat, xp->xp_തിരയൽ_dir,
		matches, അക്കം_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    എങ്കിൽ ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	പൂർണ്ണസംഖ്യ len = (പൂർണ്ണസംഖ്യ)STRLEN(pat) + 20;

	tofree = alloc(len);
	എങ്കിൽ (tofree == ശൂന്യ)
	    തിരികെ FAIL;
	vim_snprintf((അക്ഷരം *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    എങ്കിൽ (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	എങ്കിൽ (regmatch.regprog == ശൂന്യ)
	{
	    vim_free(tofree);
	    തിരികെ FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    എങ്കിൽ (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, അക്കം_Matches, matches, fuzzy);
    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, അക്കം_Matches, matches);
    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, അക്കം_Matches, matches);
    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, അക്കം_Matches, matches);
    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, അക്കം_Matches);
    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, അക്കം_Matches);
#എങ്കിൽ defined(FEAT_TERMINAL)
    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, അക്കം_Matches);
#endif
#എങ്കിൽ defined(FEAT_EVAL)
    അല്ലെങ്കിൽ എങ്കിൽ (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, അക്കം_Matches);
#endif
    അല്ലെങ്കിൽ
	ret = ExpandOther(pat, xp, &regmatch, matches, അക്കം_Matches);

    എങ്കിൽ (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    തിരികെ ret;
}

    പൂർണ്ണസംഖ്യ
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches,
    char_u	*(*func)(expand_T *, പൂർണ്ണസംഖ്യ),
					  // returns a string from the list
    പൂർണ്ണസംഖ്യ		escaped)
{
    തിരികെ ExpandGenericExt(
	pat, xp, regmatch, matches, അക്കം_Matches, func, escaped, 0);
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
    പൂർണ്ണസംഖ്യ
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches,
    char_u	*(*func)(expand_T *, പൂർണ്ണസംഖ്യ),
					  // returns a string from the list
    പൂർണ്ണസംഖ്യ		escaped,
    പൂർണ്ണസംഖ്യ		sortStartIdx)
{
    പൂർണ്ണസംഖ്യ		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		score = 0;
    പൂർണ്ണസംഖ്യ		fuzzy;
    പൂർണ്ണസംഖ്യ		match;
    പൂർണ്ണസംഖ്യ		sort_matches = FALSE;
    പൂർണ്ണസംഖ്യ		funcsort = FALSE;
    പൂർണ്ണസംഖ്യ		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ശൂന്യ;
    *അക്കം_Matches = 0;

    എങ്കിൽ (!fuzzy)
	ga_init2(&ga, വലുപ്പം(അക്ഷരം *), 30);
    അല്ലെങ്കിൽ
	ga_init2(&ga, വലുപ്പം(fuzmatch_str_T), 30);

    വേണ്ടി (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	എങ്കിൽ (str == ശൂന്യ)	    // end of list
	    നിർത്തുക;
	എങ്കിൽ (*str == NUL)	    // skip empty strings
	    തുടരുക;

	എങ്കിൽ (xp->xp_pattern[0] != NUL)
	{
	    എങ്കിൽ (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    അല്ലെങ്കിൽ
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	അല്ലെങ്കിൽ
	    match = TRUE;

	എങ്കിൽ (!match)
	    തുടരുക;

	എങ്കിൽ (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	അല്ലെങ്കിൽ
	    str = vim_strsave(str);
	എങ്കിൽ (str == ശൂന്യ)
	{
	    എങ്കിൽ (!fuzzy)
	    {
		ga_clear_strings(&ga);
		തിരികെ FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    നിർത്തുക;
	}

	എങ്കിൽ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	അല്ലെങ്കിൽ
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

#ifdef FEAT_MENU
	എങ്കിൽ (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    എങ്കിൽ (*str == '\001')
		*str = '.';
	}
#endif

	എങ്കിൽ (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    എങ്കിൽ (ga.ga_len == 0)
	തിരികെ OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    എങ്കിൽ (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    എങ്കിൽ (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    എങ്കിൽ (sort_matches && sortStartMatchIdx != -1)
    {
	എങ്കിൽ (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((ശൂന്യം *)ga.ga_data, (size_t)ga.ga_len, വലുപ്പം(char_u *),
							   sort_func_compare);
	അല്ലെങ്കിൽ
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    എങ്കിൽ (!fuzzy)
    {
	*matches = ga.ga_data;
	*അക്കം_Matches = ga.ga_len;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    തിരികെ FAIL;
	*അക്കം_Matches = ga.ga_len;
    }

#എങ്കിൽ defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    തിരികെ OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    സ്ഥിതം ശൂന്യം
expand_shellcmd_onedir(
	char_u		*pathed_pattern,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	പൂർണ്ണസംഖ്യ		*അക്കം_Matches,
	പൂർണ്ണസംഖ്യ		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    എങ്കിൽ (expand_wildcards(1, &pathed_pattern, അക്കം_Matches, matches, flags) != OK)
	തിരികെ;

    എങ്കിൽ (ga_grow(gap, *അക്കം_Matches) == FAIL)
    {
	FreeWild(*അക്കം_Matches, *matches);
	തിരികെ;
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < *അക്കം_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	എങ്കിൽ (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    എങ്കിൽ (HASHITEM_EMPTY(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_data)[gap->ga_len++] = name;
		hash_add_item(ht, hi, name, hash);
		name = ശൂന്യ;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches,	// return: number of matches
    പൂർണ്ണസംഖ്യ		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    പൂർണ്ണസംഖ്യ		flags = flagsarg;
    പൂർണ്ണസംഖ്യ		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    എങ്കിൽ (pat == ശൂന്യ)
    {
	vim_free(buf);
	തിരികെ FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    വേണ്ടി (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	എങ്കിൽ (*s != '\\')
	    തുടരുക;

	p = s + 1;
	എങ്കിൽ (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    എങ്കിൽ (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    അല്ലെങ്കിൽ
    {
	// For an absolute name we don't use $PATH.
	എങ്കിൽ (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	എങ്കിൽ (path == ശൂന്യ)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, വലുപ്പം(അക്ഷരം *), 10);
    hash_init(&found_ht);
    വേണ്ടി (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	എങ്കിൽ (*s == NUL)
	{
	    എങ്കിൽ (did_curdir)
		നിർത്തുക;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	അല്ലെങ്കിൽ
	{
#എങ്കിൽ defined(MSWIN)
	    e = vim_strchr(s, ';');
#അല്ലെങ്കിൽ
	    e = vim_strchr(s, ':');
#endif
	    എങ്കിൽ (e == ശൂന്യ)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    എങ്കിൽ (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    അല്ലെങ്കിൽ
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? വലുപ്പം(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	എങ്കിൽ (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    എങ്കിൽ (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		എങ്കിൽ (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, അക്കം_Matches, flags,
				    &found_ht, &ga);
	}

	എങ്കിൽ (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_data;
    *അക്കം_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    എങ്കിൽ (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    തിരികെ OK;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    സ്ഥിതം ശൂന്യം *
call_user_expand_func(
    ശൂന്യം	*(*user_expand_func)(char_u *, പൂർണ്ണസംഖ്യ, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    പൂർണ്ണസംഖ്യ		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = ശൂന്യ;
    ശൂന്യം	*ret;

    എങ്കിൽ (xp->xp_arg == ശൂന്യ || xp->xp_arg[0] == '\0' || xp->xp_വരി == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (ccline->cmdbuff != ശൂന്യ)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	പൂർണ്ണസംഖ്യ unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	എങ്കിൽ (pat == ശൂന്യ)
	    pat = vim_strnsave(xp->xp_pattern, xp->xp_pattern_len);
    }

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = xp->xp_വരി;
    args[2].v_type = VAR_സംഖ്യ;
    args[2].vval.v_സംഖ്യ = xp->xp_col;
    args[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, args);

    current_sctx = save_current_sctx;
    എങ്കിൽ (ccline->cmdbuff != ശൂന്യ)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    തിരികെ ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    പൂർണ്ണസംഖ്യ		keep;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		fuzzy;
    പൂർണ്ണസംഖ്യ		match;
    പൂർണ്ണസംഖ്യ		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ശൂന്യ;
    *അക്കം_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    എങ്കിൽ (retstr == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (!fuzzy)
	ga_init2(&ga, വലുപ്പം(അക്ഷരം *), 3);
    അല്ലെങ്കിൽ
	ga_init2(&ga, വലുപ്പം(fuzmatch_str_T), 3);

    വേണ്ടി (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	എങ്കിൽ (e == ശൂന്യ)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	എങ്കിൽ (xp->xp_pattern[0] != NUL)
	{
	    എങ്കിൽ (!fuzzy)
		match = vim_regexec(regmatch, s, (colnr_T)0);
	    അല്ലെങ്കിൽ
	    {
		score = fuzzy_match_str(s, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	അല്ലെങ്കിൽ
	    match = TRUE;		// match everything

	*e = keep;

	എങ്കിൽ (match)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    എങ്കിൽ (p == ശൂന്യ)
		നിർത്തുക;

	    എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		നിർത്തുക;
	    }

	    എങ്കിൽ (!fuzzy)
		((char_u **)ga.ga_data)[ga.ga_len] = p;
	    അല്ലെങ്കിൽ
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	എങ്കിൽ (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    എങ്കിൽ (ga.ga_len == 0)
	തിരികെ OK;

    എങ്കിൽ (!fuzzy)
    {
	*matches = ga.ga_data;
	*അക്കം_Matches = ga.ga_len;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    തിരികെ FAIL;
	*അക്കം_Matches = ga.ga_len;
    }
    തിരികെ OK;
}

    ശൂന്യം
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    പൂർണ്ണസംഖ്യ		have_extra = FALSE;
    പൂർണ്ണസംഖ്യ		i;

    ga_init2(&ga, വലുപ്പം(അക്ഷരം *), 3);
    ga_init2(&ga_abbr, വലുപ്പം(അക്ഷരം *), 3);
    ga_init2(&ga_kind, വലുപ്പം(അക്ഷരം *), 3);
    ga_init2(&ga_menu, വലുപ്പം(അക്ഷരം *), 3);
    ga_init2(&ga_info, വലുപ്പം(അക്ഷരം *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_ഇനങ്ങൾ(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = ശൂന്യ;
	char_u	*abbr = ശൂന്യ;
	char_u	*kind = ശൂന്യ;
	char_u	*menu = ശൂന്യ;
	char_u	*info = ശൂന്യ;

	എങ്കിൽ (tv->v_type == VAR_STRING)
	{
	    എങ്കിൽ (tv->vval.v_string == ശൂന്യ)
		തുടരുക;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_DICT && tv->vval.v_dict != ശൂന്യ)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    എങ്കിൽ (word == ശൂന്യ)
		തുടരുക;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    എങ്കിൽ (abbr != ശൂന്യ || kind != ശൂന്യ || menu != ശൂന്യ || info != ശൂന്യ)
		have_extra = TRUE;
	}
	അല്ലെങ്കിൽ
	    തുടരുക;  // Skip other types

	എങ്കിൽ (p == ശൂന്യ
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
	    നിർത്തുക;
	}

	((char_u **)ga.ga_data)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_data)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_data)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_data)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_data)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_data;
    *അക്കം_Matches = ga.ga_len;
    എങ്കിൽ (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    അല്ലെങ്കിൽ
    {
	// No extra info collected; free the placeholder NULL entries.
	വേണ്ടി (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	വേണ്ടി (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	വേണ്ടി (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	വേണ്ടി (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches)
{
    list_T      *retlist;

    *matches = ശൂന്യ;
    *അക്കം_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    എങ്കിൽ (retlist == ശൂന്യ)
	തിരികെ FAIL;

    expand_process_user_list(retlist, matches, അക്കം_Matches, xp);
    list_unref(retlist);
    തിരികെ OK;
}
#endif

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    ശൂന്യം
globpath(
    char_u	*path,
    char_u	*file,
    garray_T	*ga,
    പൂർണ്ണസംഖ്യ		expand_options,
    പൂർണ്ണസംഖ്യ		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#എങ്കിൽ defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#അല്ലെങ്കിൽ
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    ആകുമ്പോൾ (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	എങ്കിൽ (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    പൂർണ്ണസംഖ്യ	    അക്കം_p;
	    char_u  **p;

	    എങ്കിൽ (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    എങ്കിൽ (ExpandFromContext(&xpc, buf, &p, &അക്കം_p,
			     WILD_SILENT|expand_options) != FAIL && അക്കം_p > 0)
	    {
		ExpandEscape(&xpc, buf, അക്കം_p, p, WILD_SILENT|expand_options);

		എങ്കിൽ (ga_grow(ga, അക്കം_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_p; ++i)
		    {
			((char_u **)ga->ga_data)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    FreeWild(അക്കം_p, p);
		    p = ശൂന്യ;
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
    പൂർണ്ണസംഖ്യ
wildmenu_translate_താക്കോൽ(
	cmdline_info_T	*cclp,
	പൂർണ്ണസംഖ്യ		താക്കോൽ,
	expand_T	*xp,
	പൂർണ്ണസംഖ്യ		did_wild_list)
{
    പൂർണ്ണസംഖ്യ c = താക്കോൽ;

    എങ്കിൽ (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	തിരഞ്ഞെടുപ്പ് (c)
	{
	    അവസ്ഥ K_UP:	  c = K_ഇടത്; നിർത്തുക;
	    അവസ്ഥ K_DOWN:  c = K_വലത്; നിർത്തുക;
	    അവസ്ഥ K_ഇടത്:  c = K_UP; നിർത്തുക;
	    അവസ്ഥ K_വലത്: c = K_DOWN; നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:	  നിർത്തുക;
	}
    }

    എങ്കിൽ (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	എങ്കിൽ (c == K_ഇടത്)
	    c = Ctrl_P;
	അല്ലെങ്കിൽ എങ്കിൽ (c == K_വലത്)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    എങ്കിൽ (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    തിരികെ c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    സ്ഥിതം ശൂന്യം
cmdline_del(cmdline_info_T *cclp, പൂർണ്ണസംഖ്യ from)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
wildmenu_process_താക്കോൽ_menunames(cmdline_info_T *cclp, പൂർണ്ണസംഖ്യ താക്കോൽ, expand_T *xp)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    എങ്കിൽ (താക്കോൽ == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	താക്കോൽ = p_wc;
	താക്കോൽ_Typed = TRUE;  // in case the key was mapped
    }
    അല്ലെങ്കിൽ എങ്കിൽ (താക്കോൽ == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	പൂർണ്ണസംഖ്യ found = FALSE;

	j = (പൂർണ്ണസംഖ്യ)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	ആകുമ്പോൾ (--j > 0)
	{
	    // check for start of menu name
	    എങ്കിൽ (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		നിർത്തുക;
	    }
	    // check for start of submenu name
	    എങ്കിൽ (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		എങ്കിൽ (found)
		{
		    i = j + 1;
		    നിർത്തുക;
		}
		അല്ലെങ്കിൽ
		    found = TRUE;
	    }
	}
	എങ്കിൽ (i > 0)
	    cmdline_del(cclp, i);
	താക്കോൽ = p_wc;
	താക്കോൽ_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    തിരികെ താക്കോൽ;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
wildmenu_process_താക്കോൽ_filenames(cmdline_info_T *cclp, പൂർണ്ണസംഖ്യ താക്കോൽ, expand_T *xp)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    എങ്കിൽ (താക്കോൽ == K_DOWN
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	താക്കോൽ = p_wc;
	താക്കോൽ_Typed = TRUE;  // in case the key was mapped
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && താക്കോൽ == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	പൂർണ്ണസംഖ്യ found = FALSE;

	j = cclp->cmdpos;
	i = (പൂർണ്ണസംഖ്യ)(xp->xp_pattern - cclp->cmdbuff);
	ആകുമ്പോൾ (--j > i)
	{
	    എങ്കിൽ (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    എങ്കിൽ (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    താക്കോൽ = p_wc;
	    താക്കോൽ_Typed = TRUE;  // in case the key was mapped
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (താക്കോൽ == K_UP)
    {
	// go up a directory
	പൂർണ്ണസംഖ്യ found = FALSE;

	j = cclp->cmdpos - 1;
	i = (പൂർണ്ണസംഖ്യ)(xp->xp_pattern - cclp->cmdbuff);
	ആകുമ്പോൾ (--j > i)
	{
	    എങ്കിൽ (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    എങ്കിൽ (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == ശൂന്യ
#endif
	       )
	    {
		എങ്കിൽ (found)
		{
		    i = j + 1;
		    നിർത്തുക;
		}
		അല്ലെങ്കിൽ
		    found = TRUE;
	    }
	}

	എങ്കിൽ (!found)
	    j = i;
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	അല്ലെങ്കിൽ
	    j = 0;
	എങ്കിൽ (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	താക്കോൽ = p_wc;
	താക്കോൽ_Typed = TRUE;
    }

    തിരികെ താക്കോൽ;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    പൂർണ്ണസംഖ്യ
wildmenu_process_താക്കോൽ(cmdline_info_T *cclp, പൂർണ്ണസംഖ്യ താക്കോൽ, expand_T *xp)
{
    എങ്കിൽ (xp->xp_context == EXPAND_MENUNAMES)
	തിരികെ wildmenu_process_താക്കോൽ_menunames(cclp, താക്കോൽ, xp);
    അല്ലെങ്കിൽ എങ്കിൽ ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	തിരികെ wildmenu_process_താക്കോൽ_filenames(cclp, താക്കോൽ, xp);

    തിരികെ താക്കോൽ;
}

/*
 * Free expanded names when finished walking through the matches
 */
    ശൂന്യം
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    പൂർണ്ണസംഖ്യ skt = താക്കോൽ_Typed;

    എങ്കിൽ (!p_wmnu || wild_menu_showing == 0)
	തിരികെ;

#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ save_RedrawingDisabled = RedrawingDisabled;
    എങ്കിൽ (cclp->നിവേശം_fn)
	RedrawingDisabled = 0;
#endif

#എങ്കിൽ defined(FEAT_തിരയൽ_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    എങ്കിൽ (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	last_status(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    അല്ലെങ്കിൽ
    {
	win_redraw_last_status(topframe);
	redraw_statuslines();
    }
    താക്കോൽ_Typed = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    എങ്കിൽ (cclp->നിവേശം_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    ശൂന്യം
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    പൂർണ്ണസംഖ്യ		filtered = FALSE;
    പൂർണ്ണസംഖ്യ		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	തിരികെ;

    pat = tv_get_string(&argvars[0]);
    എങ്കിൽ (check_for_string_arg(argvars, 1) == FAIL)
	തിരികെ;
    type = tv_get_string(&argvars[1]);

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], ശൂന്യ);

    എങ്കിൽ (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    എങ്കിൽ (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    എങ്കിൽ (STRCMP(type, "cmdline") == 0)
    {
	പൂർണ്ണസംഖ്യ cmdline_len = (പൂർണ്ണസംഖ്യ)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (പൂർണ്ണസംഖ്യ)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    അല്ലെങ്കിൽ
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (പൂർണ്ണസംഖ്യ)STRLEN(xpc.xp_pattern);
	xpc.xp_വരി = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	തിരഞ്ഞെടുപ്പ് (xpc.xp_context)
	{
	അവസ്ഥ EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    തിരികെ;

	അവസ്ഥ EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    എങ്കിൽ (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		തിരികെ;
	    }

	    xpc.xp_arg = type + 7;
	    നിർത്തുക;

	അവസ്ഥ EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    എങ്കിൽ (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		തിരികെ;
	    }

	    xpc.xp_arg = type + 11;
	    നിർത്തുക;

# എങ്കിൽ defined(FEAT_MENU)
	അവസ്ഥ EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (പൂർണ്ണസംഖ്യ)(xpc.xp_pattern - pattern_start);
	    നിർത്തുക;
# endif

# ifdef FEAT_CSCOPE
	അവസ്ഥ EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (പൂർണ്ണസംഖ്യ)(xpc.xp_pattern - pattern_start);
	    നിർത്തുക;
# endif

# ifdef FEAT_SIGNS
	അവസ്ഥ EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (പൂർണ്ണസംഖ്യ)(xpc.xp_pattern - pattern_start);
	    നിർത്തുക;
# endif

	അവസ്ഥ EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (പൂർണ്ണസംഖ്യ)(xpc.xp_pattern - pattern_start);
	    നിർത്തുക;

	അവസ്ഥ EXPAND_SHELLCMDLINE:
	{
	    പൂർണ്ണസംഖ്യ context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(ശൂന്യ, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (പൂർണ്ണസംഖ്യ)(xpc.xp_pattern - pattern_start);
	    നിർത്തുക;
	}

	അവസ്ഥ EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    നിർത്തുക;

	അവസ്ഥ EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	പൂർണ്ണസംഖ്യ	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	എങ്കിൽ (unesc != ശൂന്യ)
	    pat_src = unesc;

	എങ്കിൽ (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	അല്ലെങ്കിൽ
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    എങ്കിൽ (rettv_list_alloc(rettv) == OK && pat != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	i;

	ExpandOne(&xpc, pat, ശൂന്യ, options, WILD_ALL_KEEP);

	വേണ്ടി (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    ശൂന്യം
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    പൂർണ്ണസംഖ്യ		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (പൂർണ്ണസംഖ്യ)STRLEN(pat);
    set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_to_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    ശൂന്യം
f_cmdcomplete_info(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    പൂർണ്ണസംഖ്യ		    idx;
    പൂർണ്ണസംഖ്യ		    ret = OK;

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL || ccline == ശൂന്യ
	    || ccline->xpc == ശൂന്യ || ccline->xpc->xp_files == ശൂന്യ)
	തിരികെ;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (പൂർണ്ണസംഖ്യ)cmdline_orig.length);
    എങ്കിൽ (ret == OK)
	ret = dict_add_സംഖ്യ(retdict, "pum_visible", pum_visible());
    എങ്കിൽ (ret == OK)
	ret = dict_add_സംഖ്യ(retdict, "selected", ccline->xpc->xp_selected);
    എങ്കിൽ (ret == OK)
    {
	li = list_alloc();
	എങ്കിൽ (li == ശൂന്യ)
	    തിരികെ;
	ret = dict_add_list(retdict, "matches", li);
	എങ്കിൽ (ret == FAIL)
	    list_unref(li);
	വേണ്ടി (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*വരി, *start_വരി, *end_വരി;
    പൂർണ്ണസംഖ്യ		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ശൂന്യ;

    എങ്കിൽ (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	തിരികെ FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_വരി = ml_get(start->lnum);
    char_u  *start_ptr = start_വരി + start->col;
    പൂർണ്ണസംഖ്യ	    is_single_വരി = start->lnum == end->lnum;

    segment_len = is_single_വരി ? (end->col - start->col)
			: (പൂർണ്ണസംഖ്യ)(ml_get_len(start->lnum) - start->col);
    എങ്കിൽ (ga_grow(&ga, segment_len + 2) != OK)
	പോകുക fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    എങ്കിൽ (!is_single_വരി)
    {
	എങ്കിൽ (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	അല്ലെങ്കിൽ
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    എങ്കിൽ (!is_single_വരി)
    {
	വേണ്ടി (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    പൂർണ്ണസംഖ്യ  linelen;

	    വരി = ml_get(lnum);
	    linelen = (പൂർണ്ണസംഖ്യ)ml_get_len(lnum);
	    എങ്കിൽ (ga_grow(&ga, linelen + 2) != OK)
		പോകുക fail;
	    ga_concat_len(&ga, വരി, linelen);
	    എങ്കിൽ (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    അല്ലെങ്കിൽ
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_വരി = ml_get(end->lnum);
    word_end = find_word_end(end_വരി + end->col);
    segment_len = (പൂർണ്ണസംഖ്യ)(word_end - end_വരി);
    എങ്കിൽ (ga_grow(&ga, segment_len) != OK)
	പോകുക fail;
    ga_concat_len(&ga, end_വരി + (is_single_വരി ? end->col : 0),
	    segment_len - (is_single_വരി ? end->col : 0));

    // Null-terminate
    എങ്കിൽ (ga_grow(&ga, 1) != OK)
	പോകുക fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    തിരികെ OK;

fail:
    ga_clear(&ga);
    തിരികെ FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_regex_match(char_u *pat, char_u *str)
{
    എങ്കിൽ (STRCMP(pat, str) == 0)
	തിരികെ TRUE;

    regmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		result;

    ++emsg_off;
    ++സന്ദേശം_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --സന്ദേശം_silent;

    എങ്കിൽ (regmatch.regprog == ശൂന്യ)
	തിരികെ FALSE;
    regmatch.rm_ic = p_ic;
    എങ്കിൽ (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++സന്ദേശം_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --സന്ദേശം_silent;

    vim_regfree(regmatch.regprog);
    തിരികെ result;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    സ്ഥിതം char_u *
concat_pattern_with_buffer_match(
	char_u *pat,
	പൂർണ്ണസംഖ്യ pat_len,
	pos_T *end_match_pos,
	പൂർണ്ണസംഖ്യ lowercase UNUSED)
{
    char_u  *വരി = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(വരി + end_match_pos->col);
    പൂർണ്ണസംഖ്യ	    match_len = (പൂർണ്ണസംഖ്യ)(word_end - (വരി + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    എങ്കിൽ (match == ശൂന്യ)
	തിരികെ ശൂന്യ;
    mch_memmove(match, pat, pat_len);
    എങ്കിൽ (match_len > 0)
    {
#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_SPELL)
	എങ്കിൽ (lowercase)
	{
	    char_u  *mword = vim_strnsave(വരി + end_match_pos->col,
		    match_len);
	    എങ്കിൽ (mword == ശൂന്യ)
		പോകുക cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    എങ്കിൽ (lower == ശൂന്യ)
		പോകുക cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	അല്ലെങ്കിൽ
#endif
	    mch_memmove(match + pat_len, വരി + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    തിരികെ match;

#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    തിരികെ ശൂന്യ;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    പൂർണ്ണസംഖ്യ		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		found_new_match;
    പൂർണ്ണസംഖ്യ		looped_around = FALSE;
    പൂർണ്ണസംഖ്യ		pat_len;
    പൂർണ്ണസംഖ്യ		has_range = FALSE;
    പൂർണ്ണസംഖ്യ		compl_started = FALSE;
    പൂർണ്ണസംഖ്യ		തിരയൽ_flags;
    char_u	*match, *full_match;
    പൂർണ്ണസംഖ്യ		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ശൂന്യ;

#ifdef FEAT_തിരയൽ_EXTRA
    has_range = തിരയൽ_first_വരി != 0;
#endif

    *matches = ശൂന്യ;
    *അക്കം_Matches = 0;

    എങ്കിൽ (pat == ശൂന്യ || *pat == NUL)
	തിരികെ FAIL;

    pat_len = (പൂർണ്ണസംഖ്യ)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_തിരയൽ_EXTRA
    എങ്കിൽ (has_range)
	cur_match_pos.lnum = തിരയൽ_first_വരി;
    അല്ലെങ്കിൽ
#endif
	cur_match_pos = pre_incsearch_pos;

    തിരയൽ_flags = തിരയൽ_OPT | തിരയൽ_NOOF | തിരയൽ_PEEK | തിരയൽ_NFMSG
	| (has_range ? തിരയൽ_START : 0);

    ga_init2(&ga, വലുപ്പം(char_u *), 10); // Use growable array of char_u*

    വേണ്ടി (;;)
    {
	++emsg_off;
	++സന്ദേശം_silent;
	found_new_match = searchit(ശൂന്യ, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		തിരയൽ_flags, RE_LAST, ശൂന്യ);
	--സന്ദേശം_silent;
	--emsg_off;

	എങ്കിൽ (found_new_match == FAIL)
	    നിർത്തുക;

#ifdef FEAT_തിരയൽ_EXTRA
	// If in range mode, check if match is within the range
	എങ്കിൽ (has_range && (cur_match_pos.lnum < തിരയൽ_first_വരി
		    || cur_match_pos.lnum > തിരയൽ_last_വരി))
		നിർത്തുക;
#endif

	എങ്കിൽ (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    എങ്കിൽ ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		എങ്കിൽ (looped_around)
		    നിർത്തുക;
		അല്ലെങ്കിൽ
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	എങ്കിൽ (char_avail() || got_int)
	{
	    എങ്കിൽ (got_int)
	    {
		(ശൂന്യം)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    പോകുക cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	എങ്കിൽ (end_match_pos.lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    തുടരുക;
	}

	// Extract the matching text prepended to completed word
	എങ്കിൽ (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    നിർത്തുക;

	എങ്കിൽ (exacttext)
	    match = full_match;
	അല്ലെങ്കിൽ
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    എങ്കിൽ (match == ശൂന്യ || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		എങ്കിൽ (match == ശൂന്യ || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    തുടരുക;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < ga.ga_len; ++i)
	{
	    എങ്കിൽ (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (match != ശൂന്യ)
	{
	    എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		പോകുക cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    എങ്കിൽ (ga.ga_len > TAG_MANY)
		നിർത്തുക;
	}
	എങ്കിൽ (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *അക്കം_Matches = ga.ga_len;
    തിരികെ OK;

cleanup:
    ga_clear_strings(&ga);
    തിരികെ FAIL;
}
