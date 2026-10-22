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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ ਪੂਰਨ	cmd_showtail;	// Only show path tail in lists ?
ਸਥਿਰ ਪੂਰਨ	may_expand_ਨਮੂਨਾ = FALSE;
ਸਥਿਰ pos_T	pre_incsearch_pos; // Cursor position when incsearch started

ਸਥਿਰ ਖਾਲੀ	ਸੈੱਟ_context_for_wildcard_arg(exarg_T *eap, char_u *arg, ਪੂਰਨ usefilter, expand_T *xp, ਪੂਰਨ *complp);
ਸਥਿਰ ਪੂਰਨ	ExpandFromContext(expand_T *xp, char_u *, char_u ***, ਪੂਰਨ *, ਪੂਰਨ);
ਸਥਿਰ char_u	*showmatches_gettail(char_u *s);
ਸਥਿਰ ਪੂਰਨ	expand_showtail(expand_T *xp);
ਸਥਿਰ ਪੂਰਨ	expand_shellcmd(char_u *filepat, char_u ***matches, ਪੂਰਨ *ਅੰਕ_Matches, ਪੂਰਨ flagsarg);
#ਜੇ defined(FEAT_EVAL)
ਸਥਿਰ ਪੂਰਨ	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, ਪੂਰਨ *ਅੰਕ_Matches);
ਸਥਿਰ ਪੂਰਨ	ExpandUserList(expand_T *xp, char_u ***matches, ਪੂਰਨ *ਅੰਕ_Matches);
ਸਥਿਰ char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
ਸਥਿਰ char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, ਪੂਰਨ srclen, ਪੂਰਨ *new_lenp);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ	expand_ਨਮੂਨਾ_ਵਿੱਚ_buf(char_u *pat, ਪੂਰਨ dir, char_u ***matches, ਪੂਰਨ *ਅੰਕ_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ਸਥਿਰ pumitem_T *compl_ਮੇਲ_array = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ compl_ਮੇਲ_arraysize;
// First column in cmdline of the matched item for completion.
ਸਥਿਰ ਪੂਰਨ compl_startcol;
ਸਥਿਰ ਪੂਰਨ compl_selected;
// cmdline before expansion
ਸਥਿਰ string_T cmdline_orig = {ਨਿਰਾ, 0};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHOW_ਮੇਲ(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    ਸਥਿਰ ਪੂਰਨ
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    ਚੋਣ (xp->xp_context)
    {
    ਹਾਲਤ EXPAND_BOOL_SETTINGS:
    ਹਾਲਤ EXPAND_COLORS:
    ਹਾਲਤ EXPAND_COMPILER:
    ਹਾਲਤ EXPAND_DIRECTORIES:
    ਹਾਲਤ EXPAND_DIRS_ਵਿੱਚ_CDPATH:
    ਹਾਲਤ EXPAND_FILES:
    ਹਾਲਤ EXPAND_FILES_ਵਿੱਚ_ਰਸਤਾ:
    ਹਾਲਤ EXPAND_FILETYPE:
    ਹਾਲਤ EXPAND_FILETYPECMD:
    ਹਾਲਤ EXPAND_HELP:
    ਹਾਲਤ EXPAND_KEYMAP:
    ਹਾਲਤ EXPAND_OLD_SETTING:
    ਹਾਲਤ EXPAND_STRING_SETTING:
    ਹਾਲਤ EXPAND_SETTING_ਘਟਾਓ:
    ਹਾਲਤ EXPAND_OWNSYNTAX:
    ਹਾਲਤ EXPAND_PACKADD:
    ਹਾਲਤ EXPAND_RUNTIME:
    ਹਾਲਤ EXPAND_SHELLCMD:
    ਹਾਲਤ EXPAND_SHELLCMDLINE:
    ਹਾਲਤ EXPAND_ਟੈਗ_ਸੂਚੀ:
    ਹਾਲਤ EXPAND_ਟੈਗ_ਸੂਚੀ_LISTFILES:
	ਵਾਪਸ FALSE;

    ਮੂਲ:
	ਰੋਕੋ;
    }

    ਵਾਪਸ vim_strchr(p_wop, WOP_FUZZY) != ਨਿਰਾ;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    ਪੂਰਨ
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    ਵਾਪਸ vim_strchr(p_wop, WOP_FUZZY) != ਨਿਰਾ && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    ਸਥਿਰ ਪੂਰਨ
ਛਾਂਟੋ_func_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    ਜੇ (*p1 != '<' && *p2 == '<') ਵਾਪਸ -1;
    ਜੇ (*p1 == '<' && *p2 != '<') ਵਾਪਸ 1;
    ਵਾਪਸ STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    ਸਥਿਰ ਖਾਲੀ
wildescape(
    expand_T	*xp,
    char_u	*str,
    ਪੂਰਨ		numfiles,
    char_u	**files)
{
    char_u	*p;
    ਪੂਰਨ		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_ਬਫਰ : VSE_NONE;

    ਜੇ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_ਵਿੱਚ_ਰਸਤਾ
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_ਵਿੱਚ_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	ਲਈ (ਪੂਰਨ i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    ਜੇ (xp->xp_backslash & XP_BS_THREE)
	    {
		ਅੱਖਰ *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		ਜੇ (p != ਨਿਰਾ)
		{
		    vim_free(files[i]);
		    files[i] = p;
#ਜੇ defined(BACKSLASH_ਵਿੱਚ_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    ਜੇ (p != ਨਿਰਾ)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#ਜੇ_ਅੰਤ
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_backslash & XP_BS_COMMA)
	    {
		ਜੇ (vim_strchr(files[i], ',') != ਨਿਰਾ)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    ਜੇ (p != ਨਿਰਾ)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#ਨਹੀਂਤਾਂ
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#ਜੇ_ਅੰਤ
	    ਜੇ (p != ਨਿਰਾ)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    ਜੇ (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	ਜੇ (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_context == EXPAND_ਟੈਗ_ਸੂਚੀ)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	ਲਈ (ਪੂਰਨ i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    ਜੇ (p != ਨਿਰਾ)
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
    ਸਥਿਰ ਖਾਲੀ
ExpandEscape(
    expand_T	*xp,
    char_u	*str,
    ਪੂਰਨ		numfiles,
    char_u	**files,
    ਪੂਰਨ		options)
{
    // May change home directory back to "~"
    ਜੇ (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    ਜੇ (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    ਪੂਰਨ
nextwild(
    expand_T	*xp,
    ਪੂਰਨ		type,
    ਪੂਰਨ		options,	// extra options for ExpandOne()
    ਪੂਰਨ		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ਪੂਰਨ		i;
    char_u	*p;
    ਪੂਰਨ		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    ਪੂਰਨ		wild_navigate = (type == WILD_ਅਗਲਾ || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    ਜੇ (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (ccline->ਨਿਵੇਸ਼_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    ਸੈੱਟ_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    may_expand_ਨਮੂਨਾ = options & WILD_MAY_EXPAND_ਨਮੂਨਾ;
	    ਸੈੱਟ_expand_context(xp);
	    may_expand_ਨਮੂਨਾ = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    ਜੇ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ਵਾਪਸ OK;  // Something illegal on command line
    }
    ਜੇ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ਵਾਪਸ FAIL;
    }

    i = (ਪੂਰਨ)(xp->xp_ਨਮੂਨਾ - ccline->cmdbuff);
    xp->xp_ਨਮੂਨਾ_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    ਜੇ (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_ਨਮੂਨਾ_len == 0)
	ਵਾਪਸ FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    ਜੇ (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	ਸੁਨੇਹਾ_puts("...");	    // show that we are busy
	ਬਾਹਰਲਾ_flush();
    }

    ਜੇ (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, ਨਿਰਾ, ਨਿਰਾ, 0, type);
    }
    ਨਹੀਂਤਾਂ
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_ਨਮੂਨਾ;
	ਪੂਰਨ	pat_len = xp->xp_ਨਮੂਨਾ_len;
#ਜੇ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	ਜੇ (unesc != ਨਿਰਾ)
	    pat_src = unesc;
#ਜੇ_ਅੰਤ

	ਜੇ (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_ਨਮੂਨਾ_ਵਿੱਚ_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	ਨਹੀਂਤਾਂ
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#ਜੇ defined(FEAT_EVAL)
	vim_free(unesc);
#ਜੇ_ਅੰਤ

	// Translate string into pattern and expand it.
	ਜੇ (tmp == ਨਿਰਾ)
	    p = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ use_options = options |
		    WILD_HOME_REPLACE|WILD_ਜੋੜੋ_SLASH|WILD_SILENT;
	    ਜੇ (escape)
		use_options |= WILD_ESCAPE;
	    ਜੇ (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_ਨਮੂਨਾ_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    ਜੇ (p != ਨਿਰਾ && type == WILD_LONGEST)
	    {
		ਪੂਰਨ	j;

		ਲਈ (j = 0; j < xp->xp_ਨਮੂਨਾ_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    ਜੇ (c == '*' || c == '?')
			ਰੋਕੋ;
		}
		ਜੇ ((ਪੂਰਨ)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    ਜੇ (!wild_navigate && ccline->cmdbuff != ਨਿਰਾ)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	ਜੇ (cmdline_orig.string == ਨਿਰਾ)
	    cmdline_orig.ਲੰਮਾਈ = 0;
	ਨਹੀਂਤਾਂ
	    cmdline_orig.ਲੰਮਾਈ = ccline->cmdlen;
    }

    ਜੇ (p != ਨਿਰਾ && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	ਪੂਰਨ	difflen;
	ਪੂਰਨ	v = OK;

	difflen = (ਪੂਰਨ)plen - xp->xp_ਨਮੂਨਾ_len;
	ਜੇ (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_ਨਮੂਨਾ = ccline->cmdbuff + i;
	}

	ਜੇ (v == OK)
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
    ਜੇ (xp->xp_context == EXPAND_MAPPINGS && p == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (xp->xp_numfiles <= 0 && p == ਨਿਰਾ)
	beep_flush();
    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(ਖਾਲੀ)ExpandOne(xp, ਨਿਰਾ, ਨਿਰਾ, 0, WILD_FREE);

    vim_free(p);

    ਵਾਪਸ OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    ਸਥਿਰ ਪੂਰਨ
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	ਪੂਰਨ		ਅੰਕ_Matches,
	ਪੂਰਨ		showtail)
{
    ਪੂਰਨ	ਅਗੇਤਰ_len;

    // Add all the completion matches
    compl_ਮੇਲ_array = ALLOC_MULT(pumitem_T, ਅੰਕ_Matches);
    ਜੇ (compl_ਮੇਲ_array == ਨਿਰਾ)
	ਵਾਪਸ EXPAND_UNSUCCESSFUL;

    compl_ਮੇਲ_arraysize = ਅੰਕ_Matches;
    ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_Matches; i++)
    {
	compl_ਮੇਲ_array[i].pum_ਪਾਠ = (xp->xp_files_abbr != ਨਿਰਾ
				    && xp->xp_files_abbr[i] != ਨਿਰਾ)
					? xp->xp_files_abbr[i]
					: SHOW_ਮੇਲ(i);
	compl_ਮੇਲ_array[i].pum_info = xp->xp_files_info != ਨਿਰਾ
					    ? xp->xp_files_info[i] : ਨਿਰਾ;
	compl_ਮੇਲ_array[i].pum_extra = xp->xp_files_menu != ਨਿਰਾ
					    ? xp->xp_files_menu[i] : ਨਿਰਾ;
	compl_ਮੇਲ_array[i].pum_ਕਿਸਮ = xp->xp_files_ਕਿਸਮ != ਨਿਰਾ
					    ? xp->xp_files_ਕਿਸਮ[i] : ਨਿਰਾ;
	compl_ਮੇਲ_array[i].pum_user_abbr_hlattr = -1;
	compl_ਮੇਲ_array[i].pum_user_ਕਿਸਮ_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == ਨਿਰਾ ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    ਅਗੇਤਰ_len = vim_strsize(xp->xp_ਨਮੂਨਾ);
    ਜੇ (showtail)
	ਅਗੇਤਰ_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_ਖਾਨਾ_off + MAX(0, compl_startcol - ਅਗੇਤਰ_len);

    ਵਾਪਸ EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    ਖਾਲੀ
cmdline_pum_display(ਖਾਲੀ)
{
    ਜੇ (p_po > 0 && p_po < 100 && !pum_redraw_ਵਿੱਚ_same_position())
	pum_call_update_screen();
    pum_display(compl_ਮੇਲ_array, compl_ਮੇਲ_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    ਪੂਰਨ
cmdline_pum_ਸਰਗਰਮ(ਖਾਲੀ)
{
    ਵਾਪਸ pum_visible() && compl_ਮੇਲ_array != ਨਿਰਾ;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    ਖਾਲੀ
cmdline_pum_remove(cmdline_info_T *cclp ਅਣਵਰਤਿਆ, ਪੂਰਨ defer_redraw)
{
    ਪੂਰਨ	save_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ	save_RedrawingDisabled = RedrawingDisabled;
    ਜੇ (cclp->ਨਿਵੇਸ਼_fn)
	RedrawingDisabled = 0;
#ਜੇ_ਅੰਤ

    ਪਦ_ਸੈੱਟ_sync_output(ਪਦ_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_ਮੇਲ_array);
    compl_ਮੇਲ_arraysize = 0;
    ਜੇ (!defer_redraw)
    {
	ਪੂਰਨ save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    ਨਹੀਂਤਾਂ
	pum_call_update_screen();
    redrawcmd();
    ਪਦ_ਸੈੱਟ_sync_output(ਪਦ_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    ਕੁੰਜੀ_Typed = save_ਕੁੰਜੀ_Typed;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (cclp->ਨਿਵੇਸ਼_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
cmdline_pum_cleanup(cmdline_info_T *cclp)
{
    cmdline_pum_remove(cclp, FALSE);
    wildmenu_cleanup(cclp);
}

/*
 * Returns the starting column number to use for the cmdline completion popup
 * menu.
 */
    ਪੂਰਨ
cmdline_compl_startcol(ਖਾਲੀ)
{
    ਵਾਪਸ compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_ਨਮੂਨਾ(ਖਾਲੀ)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ਵਾਪਸ xp == ਨਿਰਾ ? ਨਿਰਾ : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    ਪੂਰਨ
cmdline_compl_is_fuzzy(ਖਾਲੀ)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ਵਾਪਸ xp != ਨਿਰਾ && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    ਸਥਿਰ ਪੂਰਨ
skip_ਸਥਿਤੀ_ਮੇਲ_char(expand_T *xp, char_u *s)
{
    ਜੇ ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_ਨਮੂਨਾ_ਵਿੱਚ_BUF)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#ਜੇ_ਅੰਤ
	   )
    {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	ਜੇ (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    ਵਾਪਸ 2;
#ਜੇ_ਅੰਤ
	ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    ਸਥਿਰ ਪੂਰਨ
ਸਥਿਤੀ_ਮੇਲ_len(expand_T *xp, char_u *s)
{
    ਪੂਰਨ	len = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਪੂਰਨ emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    ਜੇ (emenu && menu_is_separator(s))
	ਵਾਪਸ 1;
#ਜੇ_ਅੰਤ

    ਜਦੋਂ (*s != NUL)
    {
	s += skip_ਸਥਿਤੀ_ਮੇਲ_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    ਵਾਪਸ len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    ਸਥਿਰ ਖਾਲੀ
win_redr_ਸਥਿਤੀ_matches(
    expand_T	*xp,
    ਪੂਰਨ		ਅੰਕ_matches,
    char_u	**matches,	// list of matches
    ਪੂਰਨ		ਮੇਲ,
    ਪੂਰਨ		showtail)
{
    ਪੂਰਨ		ਪੰਕਤੀ;
    char_u	*buf;
    ਪੂਰਨ		len;
    ਪੂਰਨ		clen;		// length in screen cells
    ਪੂਰਨ		fillchar;
    ਪੂਰਨ		attr;
    ਪੂਰਨ		i;
    ਪੂਰਨ		highlight = TRUE;
    char_u	*selstart = ਨਿਰਾ;
    ਪੂਰਨ		selstart_ਖਾਨਾ = 0;
    char_u	*selend = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ	ਪਹਿਲਾ_ਮੇਲ = 0;
    ਪੂਰਨ		ਜੋੜੋ_ਖੱਬਾ = FALSE;
    char_u	*s;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਪੂਰਨ		emenu;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		l;

    ਜੇ (matches == ਨਿਰਾ)	// interrupted completion?
	ਵਾਪਸ;

    ਜੇ (has_mbyte)
	buf = alloc(topframe->fr_ਚੌੜਾਈ * MB_MAXBYTES + 1);
    ਨਹੀਂਤਾਂ
	buf = alloc(topframe->fr_ਚੌੜਾਈ + 1);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (ਮੇਲ == -1)	// don't show match but original text
    {
	ਮੇਲ = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = ਸਥਿਤੀ_ਮੇਲ_len(xp, SHOW_ਮੇਲ(ਮੇਲ)) + 3;
    ਜੇ (ਮੇਲ == 0)
	ਪਹਿਲਾ_ਮੇਲ = 0;
    ਨਹੀਂਤਾਂ ਜੇ (ਮੇਲ < ਪਹਿਲਾ_ਮੇਲ)
    {
	// jumping left, as far as we can go
	ਪਹਿਲਾ_ਮੇਲ = ਮੇਲ;
	ਜੋੜੋ_ਖੱਬਾ = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	// check if match fits on the screen
	ਲਈ (i = ਪਹਿਲਾ_ਮੇਲ; i < ਮੇਲ; ++i)
	    clen += ਸਥਿਤੀ_ਮੇਲ_len(xp, SHOW_ਮੇਲ(i)) + 2;
	ਜੇ (ਪਹਿਲਾ_ਮੇਲ > 0)
	    clen += 2;
	// jumping right, put match at the left
	ਜੇ (clen > topframe->fr_ਚੌੜਾਈ)
	{
	    ਪਹਿਲਾ_ਮੇਲ = ਮੇਲ;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    ਲਈ (i = ਮੇਲ; i < ਅੰਕ_matches; ++i)
	    {
		clen += ਸਥਿਤੀ_ਮੇਲ_len(xp, SHOW_ਮੇਲ(i)) + 2;
		ਜੇ (clen >= topframe->fr_ਚੌੜਾਈ)
		    ਰੋਕੋ;
	    }
	    ਜੇ (i == ਅੰਕ_matches)
		ਜੋੜੋ_ਖੱਬਾ = TRUE;
	}
    }
    ਜੇ (ਜੋੜੋ_ਖੱਬਾ)
	ਜਦੋਂ (ਪਹਿਲਾ_ਮੇਲ > 0)
	{
	    clen += ਸਥਿਤੀ_ਮੇਲ_len(xp, SHOW_ਮੇਲ(ਪਹਿਲਾ_ਮੇਲ - 1)) + 2;
	    ਜੇ (clen >= topframe->fr_ਚੌੜਾਈ)
		ਰੋਕੋ;
	    --ਪਹਿਲਾ_ਮੇਲ;
	}

    fillchar = fillchar_ਸਥਿਤੀ(&attr, curwin);

    ਜੇ (ਪਹਿਲਾ_ਮੇਲ == 0)
    {
	*buf = NUL;
	len = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = ਪਹਿਲਾ_ਮੇਲ;
    ਜਦੋਂ (clen + ਸਥਿਤੀ_ਮੇਲ_len(xp, SHOW_ਮੇਲ(i)) + 2 < topframe->fr_ਚੌੜਾਈ)
    {
	ਜੇ (i == ਮੇਲ)
	{
	    selstart = buf + len;
	    selstart_ਖਾਨਾ = clen;
	}

	s = SHOW_ਮੇਲ(i);
	// Check for menu separators - replace with '|'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	ਜੇ (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (ਪੂਰਨ)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਲਈ ( ; *s != NUL; ++s)
	{
	    s += skip_ਸਥਿਤੀ_ਮੇਲ_char(xp, s);
	    clen += ptr2cells(s);
	    ਜੇ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (ਪੂਰਨ)STRLEN(buf + len);
	    }
	}
	ਜੇ (i == ਮੇਲ)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	ਜੇ (++i == ਅੰਕ_matches)
		ਰੋਕੋ;
    }

    ਜੇ (i != ਅੰਕ_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ - 1;
    ਜੇ (ਪੰਕਤੀ >= 0)
    {
	ਜੇ (wild_menu_showing == 0)
	{
	    ਜੇ (ਸੁਨੇਹਾ_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		ਜੇ (cmdline_ਪੰਕਤੀ == ਪੰਕਤੀਆਂ - 1)
		{
		    screen_del_ਸਤਰਾਂ(0, 0, 1, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, TRUE, 0, ਨਿਰਾ);
		    ++ਸੁਨੇਹਾ_scrolled;
		}
		ਨਹੀਂਤਾਂ
		{
		    ++cmdline_ਪੰਕਤੀ;
		    ++ਪੰਕਤੀ;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		ਜੇ (lastwin->w_ਸਥਿਤੀ_ਉਚਾਈ == 0)
		{
		    save_p_ls = p_ls;
		    save_p_wmh = p_wmh;
		    p_ls = 2;
		    p_wmh = 0;
		    ਆਖਰੀ_ਸਥਿਤੀ(FALSE);
		}
		wild_menu_showing = WM_SHOWN;
	    }
	}

	screen_puts(buf, ਪੰਕਤੀ, firstwin->w_wincol, attr);
	ਜੇ (selstart != ਨਿਰਾ && highlight)
	{
	    *selend = NUL;
	    screen_puts(selstart, ਪੰਕਤੀ, firstwin->w_wincol + selstart_ਖਾਨਾ,
		    HL_ATTR(HLF_WM));
	}

	screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, firstwin->w_wincol + clen,
		firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ,
		fillchar, fillchar, attr);
    }

    win_redraw_ਆਖਰੀ_ਸਥਿਤੀ(topframe);
    vim_free(buf);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Apply -completeopt=escape to a string about to be inserted into the command
 * line as a completion result.  If "str" is non-NULL and the active expansion
 * context is a customlist/custom user command with UCC_ESCAPE set, free "str"
 * and return a newly-allocated copy with spaces, tabs and backslashes prefixed
 * by a backslash.  Otherwise return "str" unchanged.
 */
    ਸਥਿਰ char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ਵਾਪਸ str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ str;
    vim_free(str);
    ਵਾਪਸ p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    ਸਥਿਰ char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    ਪੂਰਨ		srclen,
    ਪੂਰਨ		*new_lenp)
{
    char_u  *buf, *p, *d, *ਸਿਰਾ;

    ਜੇ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ਵਾਪਸ ਨਿਰਾ;

    buf = alloc(srclen + 1);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    d = buf;
    ਸਿਰਾ = src + srclen;
    ਲਈ (p = src; p < ਸਿਰਾ; ++p)
    {
	ਜੇ (*p == '\\' && p + 1 < ਸਿਰਾ
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (ਪੂਰਨ)(d - buf);
    ਵਾਪਸ buf;
}
#ਜੇ_ਅੰਤ

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    ਸਥਿਰ char_u *
get_ਅਗਲਾ_or_prev_ਮੇਲ(ਪੂਰਨ ਬਹੁਲਕ, expand_T *xp)
{
    ਪੂਰਨ	    findex = xp->xp_selected;
    ਪੂਰਨ	    ht;

    // When no matches found, return NULL
    ਜੇ (xp->xp_numfiles <= 0)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (ਬਹੁਲਕ == WILD_PREV)
    {
	// Select the last entry if at original text
	ਜੇ (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਬਹੁਲਕ == WILD_ਅਗਲਾ)
    {
	// Select the next entry
	++findex;
    }
    ਨਹੀਂਤਾਂ   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_ਉਚਾਈ();
	ਜੇ (ht > 3)
	    ht -= 2;

	ਜੇ (ਬਹੁਲਕ == WILD_PAGEUP)
	{
	    ਜੇ (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    ਨਹੀਂਤਾਂ ਜੇ (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    ਨਹੀਂਤਾਂ
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	ਨਹੀਂਤਾਂ    // mode == WILD_PAGEDOWN
	{
	    ਜੇ (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    ਨਹੀਂਤਾਂ ਜੇ (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    ਨਹੀਂਤਾਂ
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    ਜੇ (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	ਜੇ (xp->xp_orig != ਨਿਰਾ)
	    findex = -1;
	ਨਹੀਂਤਾਂ
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    ਜੇ (p_wmnu)
    {
	ਜੇ (compl_ਮੇਲ_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	ਨਹੀਂਤਾਂ ਜੇ (vim_strchr(p_wop, WOP_PUM) != ਨਿਰਾ)
	{
	    ਜੇ (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	ਨਹੀਂਤਾਂ
	    win_redr_ਸਥਿਤੀ_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    ਜੇ (findex == -1)
	ਵਾਪਸ vim_strsave(xp->xp_orig);
#ਜੇ defined(FEAT_EVAL)
    ਵਾਪਸ apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ vim_strsave(xp->xp_files[findex]);
#ਜੇ_ਅੰਤ
}

/*
 * Start the command-line expansion and get the matches.
 */
    ਸਥਿਰ char_u *
Expand_One_ਸ਼ੁਰੂ(ਪੂਰਨ ਬਹੁਲਕ, expand_T *xp, char_u *str, ਪੂਰਨ options)
{
    ਪੂਰਨ		non_suf_ਮੇਲ;		// number without matching suffix
    ਪੂਰਨ		i;
    char_u	*ss = ਨਿਰਾ;

    // Do the expansion.
    ਜੇ (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	ਜੇ (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_ਮੇਲ_str_2), str);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_numfiles == 0)
    {
	ਜੇ (!(options & WILD_SILENT))
	    semsg(_(e_no_ਮੇਲ_str_2), str);
    }
    ਨਹੀਂਤਾਂ
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	ਜੇ (ਬਹੁਲਕ != WILD_ALL && ਬਹੁਲਕ != WILD_ALL_KEEP && ਬਹੁਲਕ != WILD_LONGEST)
	{
	    ਜੇ (xp->xp_numfiles)
		non_suf_ਮੇਲ = xp->xp_numfiles;
	    ਨਹੀਂਤਾਂ
		non_suf_ਮੇਲ = 1;
	    ਜੇ ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_ਮੇਲ = 0;
		ਲਈ (i = 0; i < 2; ++i)
		    ਜੇ (ਮੇਲ_suffix(xp->xp_files[i]))
			++non_suf_ਮੇਲ;
	    }
	    ਜੇ (non_suf_ਮੇਲ != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		ਜੇ (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_ਨਾਂ));
		ਨਹੀਂਤਾਂ ਜੇ (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    ਜੇ (!(non_suf_ਮੇਲ != 1 && ਬਹੁਲਕ == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    ਵਾਪਸ ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    ਸਥਿਰ char_u *
ਲੱਭੋ_longest_ਮੇਲ(expand_T *xp, ਪੂਰਨ options)
{
    long_u	len;
    ਪੂਰਨ		mb_len = 1;
    ਪੂਰਨ		c0, ci;
    ਪੂਰਨ		i;
    char_u	*ss;

    ਲਈ (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	ਜੇ (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	ਨਹੀਂਤਾਂ
	    c0 = xp->xp_files[0][len];
	ਲਈ (i = 1; i < xp->xp_numfiles; ++i)
	{
	    ਜੇ (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    ਨਹੀਂਤਾਂ
		ci = xp->xp_files[i][len];
	    ਜੇ (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		ਜੇ (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c0 != ci)
		ਰੋਕੋ;
	}
	ਜੇ (i < xp->xp_numfiles)
	{
	    ਜੇ (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    ਰੋਕੋ;
	}
    }

    ss = alloc(len + 1);
    ਜੇ (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    ਵਾਪਸ ss;
}

    ਖਾਲੀ
free_xp_files_extra(expand_T *xp, ਪੂਰਨ numfiles)
{
    ਜੇ (xp->xp_files_abbr != ਨਿਰਾ)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = ਨਿਰਾ;
    }
    ਜੇ (xp->xp_files_ਕਿਸਮ != ਨਿਰਾ)
    {
	FreeWild(numfiles, xp->xp_files_ਕਿਸਮ);
	xp->xp_files_ਕਿਸਮ = ਨਿਰਾ;
    }
    ਜੇ (xp->xp_files_menu != ਨਿਰਾ)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = ਨਿਰਾ;
    }
    ਜੇ (xp->xp_files_info != ਨਿਰਾ)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = ਨਿਰਾ;
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
    ਪੂਰਨ		options,
    ਪੂਰਨ		ਬਹੁਲਕ)
{
    char_u	*ss = ਨਿਰਾ;
    ਪੂਰਨ		orig_saved = FALSE;
#ਜੇ defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    ਪੂਰਨ		ss_is_ਮੇਲ = FALSE;
#ਜੇ_ਅੰਤ

    // first handle the case of using an old match
    ਜੇ (ਬਹੁਲਕ == WILD_ਅਗਲਾ || ਬਹੁਲਕ == WILD_PREV
	    || ਬਹੁਲਕ == WILD_PAGEUP || ਬਹੁਲਕ == WILD_PAGEDOWN)
	ਵਾਪਸ get_ਅਗਲਾ_or_prev_ਮੇਲ(ਬਹੁਲਕ, xp);

    ਜੇ (ਬਹੁਲਕ == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    ਨਹੀਂਤਾਂ ਜੇ (ਬਹੁਲਕ == WILD_APPLY)
    {
	ਜੇ (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	ਨਹੀਂਤਾਂ
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#ਜੇ defined(FEAT_EVAL)
	    ss_is_ਮੇਲ = TRUE;
#ਜੇ_ਅੰਤ
	}
    }

    // free old names
    ਜੇ (xp->xp_numfiles != -1 && ਬਹੁਲਕ != WILD_ALL && ਬਹੁਲਕ != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	ਜੇ (compl_ਮੇਲ_array != ਨਿਰਾ)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    ਜੇ (ਬਹੁਲਕ == WILD_FREE)	// only release file name
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (xp->xp_numfiles == -1 && ਬਹੁਲਕ != WILD_APPLY && ਬਹੁਲਕ != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = Expand_One_ਸ਼ੁਰੂ(ਬਹੁਲਕ, xp, str, options);
#ਜੇ defined(FEAT_EVAL)
	ਜੇ (ss != ਨਿਰਾ)
	    ss_is_ਮੇਲ = TRUE;
#ਜੇ_ਅੰਤ
    }

    // Find longest common part
    ਜੇ (ਬਹੁਲਕ == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = ਲੱਭੋ_longest_ਮੇਲ(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#ਜੇ defined(FEAT_EVAL)
	ਜੇ (ss != ਨਿਰਾ)
	    ss_is_ਮੇਲ = TRUE;
#ਜੇ_ਅੰਤ
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    ਜੇ (ਬਹੁਲਕ == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	ਅੱਖਰ	*ਅਗੇਤਰ = "";
	ਅੱਖਰ	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	ਪੂਰਨ	n = xp->xp_numfiles - 1;
	ਪੂਰਨ	i;
#ਜੇ defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = ਨਿਰਾ;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	ਜੇ ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    ਜੇ (escaped != ਨਿਰਾ)
	    {
		ਲਈ (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    ਜੇ (escaped[i] == ਨਿਰਾ)
		    {
			ਜਦੋਂ (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			ਰੋਕੋ;
		    }
		}
		ਜੇ (escaped != ਨਿਰਾ)
		    files = escaped;
	    }
	}
#ਨਹੀਂਤਾਂ
	char_u	**files = xp->xp_files;
#ਜੇ_ਅੰਤ

	ਜੇ (xp->xp_ਅਗੇਤਰ == XP_ਅਗੇਤਰ_NO)
	{
	    ਅਗੇਤਰ = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	ਨਹੀਂਤਾਂ ਜੇ (xp->xp_ਅਗੇਤਰ == XP_ਅਗੇਤਰ_INV)
	{
	    ਅਗੇਤਰ = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	ਲਈ (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	ਜੇ (ss != ਨਿਰਾ)
	{
	    size_t  ss_len = 0;

	    ਲਈ (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (ਅੱਖਰ *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? ਅਗੇਤਰ : "",
		    (ਅੱਖਰ *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#ਜੇ defined(FEAT_EVAL)
	ਜੇ (escaped != ਨਿਰਾ)
	{
	    ਲਈ (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#ਜੇ_ਅੰਤ
    }

    ਜੇ (ਬਹੁਲਕ == WILD_EXPAND_FREE || ਬਹੁਲਕ == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    ਜੇ (!orig_saved)
	vim_free(orig);

#ਜੇ defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    ਜੇ (ss_is_ਮੇਲ && ਬਹੁਲਕ != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ss;
}

/*
 * Prepare an expand structure for use.
 */
    ਖਾਲੀ
ExpandInit(expand_T *xp)
{
    CLEAR_ਸੂਚਕ(xp);
    xp->xp_backslash = XP_BS_NONE;
    xp->xp_ਅਗੇਤਰ = XP_ਅਗੇਤਰ_NONE;
    xp->xp_numfiles = -1;
}

/*
 * Cleanup an expand structure after use.
 */
    ਖਾਲੀ
ExpandCleanup(expand_T *xp)
{
    ਜੇ (xp->xp_numfiles >= 0)
    {
	free_xp_files_extra(xp, xp->xp_numfiles);
	FreeWild(xp->xp_numfiles, xp->xp_files);
	xp->xp_numfiles = -1;
    }
    VIM_CLEAR(xp->xp_orig);
}

    ਖਾਲੀ
clear_cmdline_orig(ਖਾਲੀ)
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
    ਸਥਿਰ ਖਾਲੀ
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	ਪੂਰਨ		ਅੰਕ_Matches,
	ਪੂਰਨ		ਸਤਰਾਂ,
	ਪੂਰਨ		linenr,
	ਪੂਰਨ		maxlen,
	ਪੂਰਨ		showtail,
	ਪੂਰਨ		dir_attr)
{
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		isdir;
    ਪੂਰਨ		lastlen;
    char_u	*p;

    lastlen = 999;
    ਲਈ (j = linenr; j < ਅੰਕ_Matches; j += ਸਤਰਾਂ)
    {
	ਜੇ (xp->xp_context == EXPAND_ਟੈਗ_ਸੂਚੀ_LISTFILES)
	{
	    ਸੁਨੇਹਾ_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    ਸੁਨੇਹਾ_advance(maxlen + 1);
	    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)p);
	    ਸੁਨੇਹਾ_advance(maxlen + 3);
	    ਸੁਨੇਹਾ_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    ਰੋਕੋ;
	}
	ਲਈ (i = maxlen - lastlen; --i >= 0; )
	    ਸੁਨੇਹਾ_putchar(' ');
	ਜੇ (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    ਜੇ (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_ਰਸਤਾ;
		char_u	*ਰਸਤਾ;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_ਰਸਤਾ = expand_env_save_opt(matches[j], TRUE, ਨਿਰਾ);
		ਰਸਤਾ = exp_ਰਸਤਾ != ਨਿਰਾ ? exp_ਰਸਤਾ : matches[j];
		halved_slash = backslash_halve_save(ਰਸਤਾ);
		isdir = mch_isdir(halved_slash != ਨਿਰਾ ? halved_slash
			: matches[j]);
		vim_free(exp_ਰਸਤਾ);
		ਜੇ (halved_slash != ਰਸਤਾ)
		    vim_free(halved_slash);
	    }
	    ਨਹੀਂਤਾਂ
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    ਜੇ (showtail)
		p = SHOW_ਮੇਲ(j);
	    ਨਹੀਂਤਾਂ
	    {
		home_replace(ਨਿਰਾ, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    isdir = FALSE;
	    p = SHOW_ਮੇਲ(j);
	}
	lastlen = ਸੁਨੇਹਾ_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ > 0)	// when not wrapped around
    {
	ਸੁਨੇਹਾ_clr_eos();
	ਸੁਨੇਹਾ_putchar('\n');
    }
    ਬਾਹਰਲਾ_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    ਸਥਿਰ ਪੂਰਨ
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	ਪੂਰਨ		ਅੰਕ_Matches,
	ਪੂਰਨ		showtail,
	ਪੂਰਨ		noselect)
{
    ਪੂਰਨ	retval = cmdline_pum_create(ccline, xp, matches, ਅੰਕ_Matches, showtail);

    ਜੇ (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    ਵਾਪਸ retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    ਪੂਰਨ
showmatches(
    expand_T	*xp,
    ਪੂਰਨ		display_wildmenu,
    ਪੂਰਨ		display_list,
    ਪੂਰਨ		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ਪੂਰਨ		ਅੰਕ_Matches;
    char_u	**matches;
    ਪੂਰਨ		i;
    ਪੂਰਨ		maxlen;
    ਪੂਰਨ		ਸਤਰਾਂ;
    ਪੂਰਨ		columns;
    ਪੂਰਨ		attr;
    ਪੂਰਨ		showtail;
    ਪੂਰਨ		noselect = (wim_flags_arg & WIM_NOSELECT);
    ਪੂਰਨ		noinsert = (wim_flags_arg & WIM_NOINSERT);
    ਪੂਰਨ		cmdline_unchanged = noselect || noinsert;

    ਜੇ (xp->xp_numfiles == -1)
    {
	ਪੂਰਨ retval;
	ਸੈੱਟ_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&ਅੰਕ_Matches, &matches);
	ਜੇ (retval != EXPAND_OK)
	    ਵਾਪਸ retval;
	showtail = expand_showtail(xp);
    }
    ਨਹੀਂਤਾਂ
    {
	ਅੰਕ_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    ਜੇ (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != ਨਿਰਾ)
	ਵਾਪਸ show_pum_matches(ccline, xp, matches, ਅੰਕ_Matches,
		showtail && !cmdline_unchanged, noselect);

    ਜੇ (display_list)
    {
	ਸੁਨੇਹਾ_didany = FALSE;		// lines_left will be set
	ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();			// prepare for paging
	ਸੁਨੇਹਾ_putchar('\n');
	ਬਾਹਰਲਾ_flush();
	cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	ਸੁਨੇਹਾ_didany = FALSE;		// lines_left will be set again
	ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();			// prepare for paging
    }

    ਜੇ (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    ਨਹੀਂਤਾਂ ਜੇ (display_wildmenu && !display_list)
	win_redr_ਸਥਿਤੀ_matches(xp, ਅੰਕ_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    ਨਹੀਂਤਾਂ ਜੇ (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	ਲਈ (i = 0; i < ਅੰਕ_Matches; ++i)
	{
	    ਪੂਰਨ	len;
	    ਜੇ (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(ਨਿਰਾ, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    ਨਹੀਂਤਾਂ
		len = vim_strsize(SHOW_ਮੇਲ(i));
	    ਜੇ (len > maxlen)
		maxlen = len;
	}

	ਜੇ (xp->xp_context == EXPAND_ਟੈਗ_ਸੂਚੀ_LISTFILES)
	    ਸਤਰਾਂ = ਅੰਕ_Matches;
	ਨਹੀਂਤਾਂ
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_ਚੌੜਾਈ + 2) / maxlen;
	    ਜੇ (columns < 1)
		columns = 1;
	    ਸਤਰਾਂ = (ਅੰਕ_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	ਜੇ (xp->xp_context == EXPAND_ਟੈਗ_ਸੂਚੀ_LISTFILES)
	{
	    ਸੁਨੇਹਾ_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    ਸੁਨੇਹਾ_clr_eos();
	    ਸੁਨੇਹਾ_advance(maxlen - 3);
	    ਸੁਨੇਹਾ_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	ਲਈ (i = 0; i < ਸਤਰਾਂ; ++i)
	{
	    showmatches_oneline(xp, matches, ਅੰਕ_Matches, ਸਤਰਾਂ, i,
						maxlen, showtail, attr);
	    ਜੇ (got_int)
	    {
		got_int = FALSE;
		ਰੋਕੋ;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    ਜੇ (display_wildmenu && display_list)
    {
	ਜੇ (vim_strchr(p_wop, WOP_PUM) != ਨਿਰਾ)
	    (ਖਾਲੀ)show_pum_matches(ccline, xp, matches, ਅੰਕ_Matches,
		    showtail && !cmdline_unchanged, noselect);
	ਨਹੀਂਤਾਂ
	    win_redr_ਸਥਿਤੀ_matches(xp, ਅੰਕ_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    ਜੇ (xp->xp_numfiles == -1)
    {
	FreeWild(ਅੰਕ_Matches, matches);
	free_xp_files_extra(xp, ਅੰਕ_Matches);
    }

    ਵਾਪਸ EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    ਸਥਿਰ char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    ਪੂਰਨ		had_sep = FALSE;

    ਲਈ (p = s; *p != NUL; )
    {
	ਜੇ (vim_ispathsep(*p)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
		&& !rem_backslash(p)
#ਜੇ_ਅੰਤ
	   )
	    had_sep = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    ਵਾਪਸ t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    ਸਥਿਰ ਪੂਰਨ
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*ਸਿਰਾ;

    // When not completing file names a "/" may mean something different.
    ਜੇ (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	ਵਾਪਸ FALSE;

    ਸਿਰਾ = gettail(xp->xp_ਨਮੂਨਾ);
    ਜੇ (ਸਿਰਾ == xp->xp_ਨਮੂਨਾ)		// there is no path separator
	ਵਾਪਸ FALSE;

    ਲਈ (s = xp->xp_ਨਮੂਨਾ; s < ਸਿਰਾ; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	ਜੇ (rem_backslash(s))
	    ++s;
	ਨਹੀਂਤਾਂ ਜੇ (vim_strchr((char_u *)"*?[", *s) != ਨਿਰਾ)
	    ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
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
    ਪੂਰਨ		len,
    ਪੂਰਨ		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		new_len;
    char_u	*ਪੂਛ;
    ਪੂਰਨ		ਸਿਰੇ_ਵਿੱਚ_ਤਾਰਾ;

    ਜੇ (context != EXPAND_FILES
	    && context != EXPAND_FILES_ਵਿੱਚ_ਰਸਤਾ
	    && context != EXPAND_SHELLCMD
	    && context != EXPAND_DIRECTORIES
	    && context != EXPAND_DIRS_ਵਿੱਚ_CDPATH)
    {
	// Matching will be done internally (on something other than files).
	// So we convert the file-matching-type wildcards into our kind for
	// use with vim_regcomp().  First work out how long it will be:

	// For help tags the translation is done in find_help_tags().
	// For a tag pattern starting with "/" no translation is needed.
	ਜੇ (context == EXPAND_FINDFUNC
		|| context == EXPAND_HELP
		|| context == EXPAND_COLORS
		|| context == EXPAND_COMPILER
		|| context == EXPAND_OWNSYNTAX
		|| context == EXPAND_FILETYPE
		|| context == EXPAND_KEYMAP
		|| context == EXPAND_PACKADD
		|| context == EXPAND_RUNTIME
		|| ((context == EXPAND_ਟੈਗ_ਸੂਚੀ_LISTFILES
			|| context == EXPAND_ਟੈਗ_ਸੂਚੀ)
		    && fname[0] == '/'))
	    retval = vim_strnsave(fname, len);
	ਨਹੀਂਤਾਂ
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    ਲਈ (i = 0; i < len; i++)
	    {
		ਜੇ (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		ਜੇ (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		ਜੇ ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    ਜੇ (retval != ਨਿਰਾ)
	    {
		retval[0] = '^';
		j = 1;
		ਲਈ (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    ਜੇ (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			ਰੋਕੋ;

		    ਚੋਣ (fname[i])
		    {
			ਹਾਲਤ '*':   retval[j++] = '.';
				    ਰੋਕੋ;
			ਹਾਲਤ '~':   retval[j++] = '\\';
				    ਰੋਕੋ;
			ਹਾਲਤ '?':   retval[j] = '.';
				    ਜਾਰੀ;
			ਹਾਲਤ '.':   ਜੇ (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    ਰੋਕੋ;
			ਹਾਲਤ '\\':  ਜੇ (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    ਰੋਕੋ;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	retval = alloc(len + 4);
	ਜੇ (retval != ਨਿਰਾ)
	{
	    vim_strncpy(retval, fname, len);

	    // Don't add a star to *, ~, ~user, $var or `cmd`.
	    // * would become **, which walks the whole tree.
	    // ~ would be at the start of the file name, but not the tail.
	    // $ could be anywhere in the tail.
	    // ` could be anywhere in the file name.
	    // When the name ends in '$' don't add a star, remove the '$'.
	    ਪੂਛ = gettail(retval);
	    ਸਿਰੇ_ਵਿੱਚ_ਤਾਰਾ = (len > 0 && retval[len - 1] == '*');
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	    ਲਈ (i = len - 2; i >= 0; --i)
	    {
		ਜੇ (retval[i] != '\\')
		    ਰੋਕੋ;
		ਸਿਰੇ_ਵਿੱਚ_ਤਾਰਾ = !ਸਿਰੇ_ਵਿੱਚ_ਤਾਰਾ;
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ ((*retval != '~' || ਪੂਛ != retval)
		    && !ਸਿਰੇ_ਵਿੱਚ_ਤਾਰਾ
		    && vim_strchr(ਪੂਛ, '$') == ਨਿਰਾ
		    && vim_strchr(retval, '`') == ਨਿਰਾ)
		retval[len++] = '*';
	    ਨਹੀਂਤਾਂ ਜੇ (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    ਵਾਪਸ retval;
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
    ਖਾਲੀ
ਸੈੱਟ_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    ਜੇ ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_ਨਮੂਨਾ)
    {
	xp->xp_context = EXPAND_ਨਮੂਨਾ_ਵਿੱਚ_BUF;
	xp->xp_ਖੋਜ_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_ਨਮੂਨਾ = ccline->cmdbuff;
	xp->xp_ਨਮੂਨਾ_len = ccline->cmdpos;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ = 0; // Search entire buffer
#ਜੇ_ਅੰਤ
	ਵਾਪਸ;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    ਜੇ (ccline->cmdfirstc != ':'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->ਨਿਵੇਸ਼_fn
#ਜੇ_ਅੰਤ
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	ਵਾਪਸ;
    }

    // Fallback to command-line expansion
    ਸੈੱਟ_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, TRUE);
}

/*
 * Sets the index of a built-in or user defined command 'cmd' in eap->cmdidx.
 * For user defined commands, the completion context is set in 'xp' and the
 * completion flags in 'complp'.
 *
 * Returns a pointer to the text after the command or NULL for failure.
 */
    ਸਥਿਰ char_u *
ਸੈੱਟ_cmd_ਸੂਚਕਾਂਕ(char_u *cmd, exarg_T *eap, expand_T *xp, ਪੂਰਨ *complp)
{
    char_u	*p = ਨਿਰਾ;
    ਪੂਰਨ		len = 0;
    ਪੂਰਨ		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    ਜੇ (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    ਨਹੀਂਤਾਂ
    {
	p = cmd;
	ਜਦੋਂ (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	ਜੇ (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    ਜਦੋਂ (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	ਜੇ (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    ਜਦੋਂ (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	ਜੇ (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != ਨਿਰਾ)
	    ++p;
	len = (ਪੂਰਨ)(p - cmd);

	ਜੇ (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    ਵਾਪਸ ਨਿਰਾ;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	ਜੇ ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    ਜਦੋਂ (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    ਜੇ (*p == NUL && ASCII_ISALNUM(p[-1]))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (eap->cmdidx == CMD_ਮਾਪ)
    {
	ਜੇ (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != ਨਿਰਾ)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = ਲੱਭੋ_ucmd(eap, p, ਨਿਰਾ, xp, complp);
	    ਜੇ (p == ਨਿਰਾ)
		eap->cmdidx = CMD_ਮਾਪ;	// ambiguous user command
	}
    }
    ਜੇ (eap->cmdidx == CMD_ਮਾਪ)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	ਪੂਰਨ		usefilter,
	expand_T	*xp,
	ਪੂਰਨ		*complp)
{
    char_u	*p;
    ਪੂਰਨ		c;
    ਪੂਰਨ		ਵਿੱਚ_quote = FALSE;
    char_u	*bow = ਨਿਰਾ;	// Beginning of word
    ਪੂਰਨ		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_ਨਮੂਨਾ = skipwhite(arg);
    p = xp->xp_ਨਮੂਨਾ;
    ਜਦੋਂ (*p != NUL)
    {
	ਜੇ (has_mbyte)
	    c = mb_ptr2char(p);
	ਨਹੀਂਤਾਂ
	    c = *p;
	ਜੇ (c == '\\' && p[1] != NUL)
	    ++p;
	ਨਹੀਂਤਾਂ ਜੇ (c == '`')
	{
	    ਜੇ (!ਵਿੱਚ_quote)
	    {
		xp->xp_ਨਮੂਨਾ = p;
		bow = p + 1;
	    }
	    ਵਿੱਚ_quote = !ਵਿੱਚ_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	ਨਹੀਂਤਾਂ ਜੇ (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != ਨਿਰਾ && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    ਜਦੋਂ (*p != NUL)
	    {
		ਜੇ (has_mbyte)
		    c = mb_ptr2char(p);
		ਨਹੀਂਤਾਂ
		    c = *p;
		ਜੇ (c == '`' || vim_isfilec_or_wc(c))
		    ਰੋਕੋ;
		ਜੇ (has_mbyte)
		    len = (*mb_ptr2len)(p);
		ਨਹੀਂਤਾਂ
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    ਜੇ (ਵਿੱਚ_quote)
		bow = p;
	    ਨਹੀਂਤਾਂ
		xp->xp_ਨਮੂਨਾ = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    ਜੇ (bow != ਨਿਰਾ && ਵਿੱਚ_quote)
	xp->xp_ਨਮੂਨਾ = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    ਜੇ (usefilter
	    || (eap != ਨਿਰਾ
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	xp->xp_shell = TRUE;
#ਜੇ_ਅੰਤ
	// When still after the command name expand executables.
	ਜੇ (xp->xp_ਨਮੂਨਾ == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    ਜੇ (*xp->xp_ਨਮੂਨਾ == '$')
    {
	ਲਈ (p = xp->xp_ਨਮੂਨਾ + 1; *p != NUL; ++p)
	    ਜੇ (!vim_isIDc(*p))
		ਰੋਕੋ;
	ਜੇ (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_ਨਮੂਨਾ;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    ਜੇ (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    ਜੇ (*xp->xp_ਨਮੂਨਾ == '~')
    {
	ਲਈ (p = xp->xp_ਨਮੂਨਾ + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	ਜੇ (*p == NUL && p > xp->xp_ਨਮੂਨਾ + 1
		&& ਮੇਲ_user(xp->xp_ਨਮੂਨਾ + 1) >= 1)
	{
	    xp->xp_context = EXPAND_USER;
	    ++xp->xp_ਨਮੂਨਾ;
	}
    }
}

/*
 * Set the completion context for the "++opt=arg" argument.  Always returns
 * NULL.
 */
    ਸਥਿਰ char_u *
ਸੈੱਟ_context_ਵਿੱਚ_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    ਜੇ (p == ਨਿਰਾ)
	xp->xp_ਨਮੂਨਾ = arg;
    ਨਹੀਂਤਾਂ
	xp->xp_ਨਮੂਨਾ = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    ਸਥਿਰ char_u *
ਸੈੱਟ_context_ਵਿੱਚ_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    ਜੇ (p == ਨਿਰਾ)
	xp->xp_ਨਮੂਨਾ = arg;
    ਨਹੀਂਤਾਂ
	xp->xp_ਨਮੂਨਾ = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    ਸਥਿਰ char_u *
ਸੈੱਟ_context_ਵਿੱਚ_filter_cmd(expand_T *xp, char_u *arg)
{
    ਜੇ (*arg != NUL)
	arg = skip_vimgrep_pat(arg, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (arg == ਨਿਰਾ || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ skipwhite(arg);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    ਸਥਿਰ char_u *
ਸੈੱਟ_context_ਵਿੱਚ_ਮੇਲ_cmd(expand_T *xp, char_u *arg)
{
    ਜੇ (*arg == NUL || !ਸਿਰੇ_excmd(*arg))
    {
	// also complete "None"
	ਸੈੱਟ_context_ਵਿੱਚ_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	ਜੇ (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    ਵਾਪਸ ਲੱਭੋ_nextcmd(arg);
}
#ਜੇ_ਅੰਤ

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    ਸਥਿਰ char_u *
ਲੱਭੋ_cmd_after_global_cmd(char_u *arg)
{
    ਪੂਰਨ		delim;

    delim = *arg;	    // get the delimiter
    ਜੇ (delim)
	++arg;		    // skip delimiter if there is one

    ਜਦੋਂ (arg[0] != NUL && arg[0] != delim)
    {
	ਜੇ (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    ਜੇ (arg[0] != NUL)
	ਵਾਪਸ arg + 1;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    ਸਥਿਰ char_u *
ਲੱਭੋ_cmd_after_substitute_cmd(char_u *arg)
{
    ਪੂਰਨ		delim;

    delim = *arg;
    ਜੇ (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	ਜੇ (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    ਜਦੋਂ (arg[0] != NUL && arg[0] != delim)
	    {
		ਜੇ (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    ਜੇ (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    ਜਦੋਂ (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == ਨਿਰਾ)
	++arg;
    ਜੇ (arg[0] != NUL)
	ਵਾਪਸ arg;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    ਸਥਿਰ char_u *
ਲੱਭੋ_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    ਜੇ (*arg != '/')
	ਵਾਪਸ ਨਿਰਾ;

    // Match regexp, not just whole words
    ਲਈ (++arg; *arg && *arg != '/'; arg++)
	ਜੇ (*arg == '\\' && arg[1] != NUL)
	    arg++;
    ਜੇ (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	ਜੇ (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == ਨਿਰਾ)
	    xp->xp_context = EXPAND_NOTHING;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ arg;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    ਸਥਿਰ char_u *
ਸੈੱਟ_context_ਵਿੱਚ_unlet_cmd(expand_T *xp, char_u *arg)
{
    ਜਦੋਂ ((xp->xp_ਨਮੂਨਾ = vim_strchr(arg, ' ')) != ਨਿਰਾ)
	arg = xp->xp_ਨਮੂਨਾ + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_ਨਮੂਨਾ = arg;

    ਜੇ (*xp->xp_ਨਮੂਨਾ == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_ਨਮੂਨਾ;
    }

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    ਸਥਿਰ char_u *
ਸੈੱਟ_context_ਵਿੱਚ_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    ਜੇ (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_ਨਮੂਨਾ = arg;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_ਨਮੂਨਾ = skipwhite(p);
	}
	ਨਹੀਂਤਾਂ
	    xp->xp_context = EXPAND_NOTHING;
    }

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਗਿਣਤੀ
{
    EXP_FILETYPECMD_ALL,	// expand all :filetype values
    EXP_FILETYPECMD_PLUGIN,	// expand plugin on off
    EXP_FILETYPECMD_INDENT,	// expand indent on off
    EXP_FILETYPECMD_ONOFF,	// expand on off
} filetype_expand_what;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_FILETYPECMD_PLUGIN 0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_FILETYPECMD_INDENT 0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_FILETYPECMD_ONOFF  0x04

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਗਿਣਤੀ
{
    EXP_BREAKPT_ਜੋੜੋ,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    ਸਥਿਰ char_u *
ਸੈੱਟ_context_ਵਿੱਚ_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_ਸ਼ੁਰੂ;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_ਨਮੂਨਾ = arg;

    ਜੇ (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ਜੋੜੋ;
    ਨਹੀਂਤਾਂ ਜੇ (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    ਨਹੀਂਤਾਂ
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    ਜੇ (*p == NUL)
	ਵਾਪਸ ਨਿਰਾ;
    subcmd_ਸ਼ੁਰੂ = p;

    ਜੇ (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	ਜੇ (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    ਜੇ (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    p = skipwhite(p);
	}
	ਜੇ (STRNCMP("file", subcmd_ਸ਼ੁਰੂ, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	ਨਹੀਂਤਾਂ
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_ਨਮੂਨਾ = p;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_ਵਿਅੰਜਕ;
	xp->xp_ਨਮੂਨਾ = skipwhite(p + 5);
    }

    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ char_u *
ਸੈੱਟ_context_ਵਿੱਚ_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_ਨਮੂਨਾ = ਨਿਰਾ;

    p = skipwhite(arg);
    ਜੇ (VIM_ISDIGIT(*p))
	ਵਾਪਸ ਨਿਰਾ;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_ਨਮੂਨਾ = p;

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    ਸਥਿਰ char_u *
ਸੈੱਟ_context_ਵਿੱਚ_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    ਪੂਰਨ    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_ਨਮੂਨਾ = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    ਜੇ (*p == NUL)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (;;)
    {
	ਜੇ (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    ਜਾਰੀ;
	}
	ਜੇ (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    ਜਾਰੀ;
	}
	ਰੋਕੋ;
    }

    ਜੇ ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    ਨਹੀਂਤਾਂ ਜੇ ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    ਨਹੀਂਤਾਂ ਜੇ ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_ਨਮੂਨਾ = p;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_context_with_ਨਮੂਨਾ(expand_T *xp)
{
    ਪੂਰਨ		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    ਪੂਰਨ		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_ਨਮੂਨਾ_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    ਜੇ (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

    xp->xp_ਨਮੂਨਾ = ccline->cmdbuff + skiplen;
    xp->xp_ਨਮੂਨਾ_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_ਨਮੂਨਾ_ਵਿੱਚ_BUF;
    xp->xp_ਖੋਜ_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    ਸਥਿਰ char_u *
ਸੈੱਟ_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	ਲੰਮਾ		argt,
	ਪੂਰਨ		compl,
	ਪੂਰਨ		forceit)
{
    char_u  *nextcmd;

    ਚੋਣ (cmdidx)
    {
	ਹਾਲਤ CMD_ਲੱਭੋ:
	ਹਾਲਤ CMD_sfind:
	ਹਾਲਤ CMD_tabfind:
	    ਜੇ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_ਵਿੱਚ_ਰਸਤਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_cd:
	ਹਾਲਤ CMD_chdir:
	ਹਾਲਤ CMD_tcd:
	ਹਾਲਤ CMD_tchdir:
	ਹਾਲਤ CMD_lcd:
	ਹਾਲਤ CMD_lchdir:
	    ਜੇ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_ਵਿੱਚ_CDPATH;
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	ਹਾਲਤ CMD_aboveleft:
	ਹਾਲਤ CMD_argdo:
	ਹਾਲਤ CMD_belowright:
	ਹਾਲਤ CMD_botright:
	ਹਾਲਤ CMD_browse:
	ਹਾਲਤ CMD_bufdo:
	ਹਾਲਤ CMD_cdo:
	ਹਾਲਤ CMD_cfdo:
	ਹਾਲਤ CMD_confirm:
	ਹਾਲਤ CMD_debug:
	ਹਾਲਤ CMD_folddoclosed:
	ਹਾਲਤ CMD_folddoopen:
	ਹਾਲਤ CMD_hide:
	ਹਾਲਤ CMD_horizontal:
	ਹਾਲਤ CMD_keepalt:
	ਹਾਲਤ CMD_keepjumps:
	ਹਾਲਤ CMD_keepmarks:
	ਹਾਲਤ CMD_keeppatterns:
	ਹਾਲਤ CMD_ldo:
	ਹਾਲਤ CMD_leftabove:
	ਹਾਲਤ CMD_lfdo:
	ਹਾਲਤ CMD_lockmarks:
	ਹਾਲਤ CMD_noautocmd:
	ਹਾਲਤ CMD_noswapfile:
	ਹਾਲਤ CMD_rightbelow:
	ਹਾਲਤ CMD_sandbox:
	ਹਾਲਤ CMD_silent:
	ਹਾਲਤ CMD_tab:
	ਹਾਲਤ CMD_tabdo:
	ਹਾਲਤ CMD_topleft:
	ਹਾਲਤ CMD_unsilent:
	ਹਾਲਤ CMD_verbose:
	ਹਾਲਤ CMD_vertical:
	ਹਾਲਤ CMD_windo:
	ਹਾਲਤ CMD_vim9cmd:
	ਹਾਲਤ CMD_legacy:
	    ਵਾਪਸ arg;

	ਹਾਲਤ CMD_filter:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_filter_cmd(xp, arg);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	ਹਾਲਤ CMD_ਮੇਲ:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_ਮੇਲ_cmd(xp, arg);
#ਜੇ_ਅੰਤ

	// All completion for the +cmdline_compl feature goes here.

	ਹਾਲਤ CMD_command:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_user_cmd(xp, arg);

	ਹਾਲਤ CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_global:
	ਹਾਲਤ CMD_vglobal:
	    nextcmd = ਲੱਭੋ_cmd_after_global_cmd(arg);
	    ਜੇ (!nextcmd && may_expand_ਨਮੂਨਾ)
		ਸੈੱਟ_context_with_ਨਮੂਨਾ(xp);
	    ਵਾਪਸ nextcmd;

	ਹਾਲਤ CMD_and:
	ਹਾਲਤ CMD_substitute:
	    nextcmd = ਲੱਭੋ_cmd_after_substitute_cmd(arg);
	    ਜੇ (!nextcmd && may_expand_ਨਮੂਨਾ)
		ਸੈੱਟ_context_with_ਨਮੂਨਾ(xp);
	    ਵਾਪਸ nextcmd;

	ਹਾਲਤ CMD_isearch:
	ਹਾਲਤ CMD_dsearch:
	ਹਾਲਤ CMD_ilist:
	ਹਾਲਤ CMD_dlist:
	ਹਾਲਤ CMD_ijump:
	ਹਾਲਤ CMD_psearch:
	ਹਾਲਤ CMD_djump:
	ਹਾਲਤ CMD_isplit:
	ਹਾਲਤ CMD_dsplit:
	    ਵਾਪਸ ਲੱਭੋ_cmd_after_isearch_cmd(xp, arg);
	ਹਾਲਤ CMD_autocmd:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_autocmd(xp, arg, FALSE);
	ਹਾਲਤ CMD_doautocmd:
	ਹਾਲਤ CMD_doautoall:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_autocmd(xp, arg, TRUE);
	ਹਾਲਤ CMD_ਸੈੱਟ:
	    ਸੈੱਟ_context_ਵਿੱਚ_ਸੈੱਟ_cmd(xp, arg, 0);
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_setglobal:
	    ਸੈੱਟ_context_ਵਿੱਚ_ਸੈੱਟ_cmd(xp, arg, OPT_GLOBAL);
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_setlocal:
	    ਸੈੱਟ_context_ਵਿੱਚ_ਸੈੱਟ_cmd(xp, arg, OPT_LOCAL);
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_ਟੈਗ:
	ਹਾਲਤ CMD_stag:
	ਹਾਲਤ CMD_ptag:
	ਹਾਲਤ CMD_ltag:
	ਹਾਲਤ CMD_tselect:
	ਹਾਲਤ CMD_stselect:
	ਹਾਲਤ CMD_ptselect:
	ਹਾਲਤ CMD_tjump:
	ਹਾਲਤ CMD_stjump:
	ਹਾਲਤ CMD_ptjump:
	    ਜੇ (vim_strchr(p_wop, WOP_TAGFILE) != ਨਿਰਾ)
		xp->xp_context = EXPAND_ਟੈਗ_ਸੂਚੀ_LISTFILES;
	    ਨਹੀਂਤਾਂ
		xp->xp_context = EXPAND_ਟੈਗ_ਸੂਚੀ;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਹਾਲਤ CMD_syntax:
	    ਸੈੱਟ_context_ਵਿੱਚ_syntax_cmd(xp, arg);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਹਾਲਤ CMD_final:
	ਹਾਲਤ CMD_const:
	ਹਾਲਤ CMD_let:
	ਹਾਲਤ CMD_var:
	ਹਾਲਤ CMD_if:
	ਹਾਲਤ CMD_elseif:
	ਹਾਲਤ CMD_while:
	ਹਾਲਤ CMD_for:
	ਹਾਲਤ CMD_echo:
	ਹਾਲਤ CMD_echon:
	ਹਾਲਤ CMD_execute:
	ਹਾਲਤ CMD_echomsg:
	ਹਾਲਤ CMD_echoerr:
	ਹਾਲਤ CMD_echoconsole:
	ਹਾਲਤ CMD_echowindow:
	ਹਾਲਤ CMD_call:
	ਹਾਲਤ CMD_return:
	ਹਾਲਤ CMD_cexpr:
	ਹਾਲਤ CMD_caddexpr:
	ਹਾਲਤ CMD_cgetexpr:
	ਹਾਲਤ CMD_lexpr:
	ਹਾਲਤ CMD_laddexpr:
	ਹਾਲਤ CMD_lgetexpr:
	    ਸੈੱਟ_context_for_ਵਿਅੰਜਕ(xp, arg, cmdidx);
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_unlet:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_unlet_cmd(xp, arg);
	ਹਾਲਤ CMD_function:
	ਹਾਲਤ CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_disassemble:
	    ਸੈੱਟ_context_ਵਿੱਚ_disassemble_cmd(xp, arg);
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_echohl:
	    ਸੈੱਟ_context_ਵਿੱਚ_echohl_cmd(xp, arg);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ CMD_highlight:
	    ਸੈੱਟ_context_ਵਿੱਚ_highlight_cmd(xp, arg);
	    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
	ਹਾਲਤ CMD_cscope:
	ਹਾਲਤ CMD_lcscope:
	ਹਾਲਤ CMD_scscope:
	    ਸੈੱਟ_context_ਵਿੱਚ_cscope_cmd(xp, arg, cmdidx);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	ਹਾਲਤ CMD_sign:
	    ਸੈੱਟ_context_ਵਿੱਚ_sign_cmd(xp, arg);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ CMD_bdelete:
	ਹਾਲਤ CMD_bwipeout:
	ਹਾਲਤ CMD_bunload:
	    ਜਦੋਂ ((xp->xp_ਨਮੂਨਾ = vim_strchr(arg, ' ')) != ਨਿਰਾ)
		arg = xp->xp_ਨਮੂਨਾ + 1;
	    // FALLTHROUGH
	ਹਾਲਤ CMD_ਬਫਰ:
	ਹਾਲਤ CMD_sbuffer:
	ਹਾਲਤ CMD_pbuffer:
	ਹਾਲਤ CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	ਹਾਲਤ CMD_diffget:
	ਹਾਲਤ CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ CMD_USER:
	ਹਾਲਤ CMD_USER_BUF:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	ਹਾਲਤ CMD_map:	    ਹਾਲਤ CMD_noremap:
	ਹਾਲਤ CMD_nmap:	    ਹਾਲਤ CMD_nnoremap:
	ਹਾਲਤ CMD_vmap:	    ਹਾਲਤ CMD_vnoremap:
	ਹਾਲਤ CMD_omap:	    ਹਾਲਤ CMD_onoremap:
	ਹਾਲਤ CMD_imap:	    ਹਾਲਤ CMD_inoremap:
	ਹਾਲਤ CMD_cmap:	    ਹਾਲਤ CMD_cnoremap:
	ਹਾਲਤ CMD_lmap:	    ਹਾਲਤ CMD_lnoremap:
	ਹਾਲਤ CMD_smap:	    ਹਾਲਤ CMD_snoremap:
	ਹਾਲਤ CMD_tmap:	    ਹਾਲਤ CMD_tnoremap:
	ਹਾਲਤ CMD_xmap:	    ਹਾਲਤ CMD_xnoremap:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	ਹਾਲਤ CMD_unmap:
	ਹਾਲਤ CMD_nunmap:
	ਹਾਲਤ CMD_vunmap:
	ਹਾਲਤ CMD_ounmap:
	ਹਾਲਤ CMD_iunmap:
	ਹਾਲਤ CMD_cunmap:
	ਹਾਲਤ CMD_lunmap:
	ਹਾਲਤ CMD_sunmap:
	ਹਾਲਤ CMD_tunmap:
	ਹਾਲਤ CMD_xunmap:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	ਹਾਲਤ CMD_mapclear:
	ਹਾਲਤ CMD_nmapclear:
	ਹਾਲਤ CMD_vmapclear:
	ਹਾਲਤ CMD_omapclear:
	ਹਾਲਤ CMD_imapclear:
	ਹਾਲਤ CMD_cmapclear:
	ਹਾਲਤ CMD_lmapclear:
	ਹਾਲਤ CMD_smapclear:
	ਹਾਲਤ CMD_tmapclear:
	ਹਾਲਤ CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_abbreviate:	ਹਾਲਤ CMD_noreabbrev:
	ਹਾਲਤ CMD_cabbrev:	ਹਾਲਤ CMD_cnoreabbrev:
	ਹਾਲਤ CMD_iabbrev:	ਹਾਲਤ CMD_inoreabbrev:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	ਹਾਲਤ CMD_unabbreviate:
	ਹਾਲਤ CMD_cunabbrev:
	ਹਾਲਤ CMD_iunabbrev:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	ਹਾਲਤ CMD_menu:	    ਹਾਲਤ CMD_noremenu:	    ਹਾਲਤ CMD_unmenu:
	ਹਾਲਤ CMD_amenu:	    ਹਾਲਤ CMD_anoremenu:	    ਹਾਲਤ CMD_aunmenu:
	ਹਾਲਤ CMD_nmenu:	    ਹਾਲਤ CMD_nnoremenu:	    ਹਾਲਤ CMD_nunmenu:
	ਹਾਲਤ CMD_vmenu:	    ਹਾਲਤ CMD_vnoremenu:	    ਹਾਲਤ CMD_vunmenu:
	ਹਾਲਤ CMD_omenu:	    ਹਾਲਤ CMD_onoremenu:	    ਹਾਲਤ CMD_ounmenu:
	ਹਾਲਤ CMD_imenu:	    ਹਾਲਤ CMD_inoremenu:	    ਹਾਲਤ CMD_iunmenu:
	ਹਾਲਤ CMD_cmenu:	    ਹਾਲਤ CMD_cnoremenu:	    ਹਾਲਤ CMD_cunmenu:
	ਹਾਲਤ CMD_tlmenu:    ਹਾਲਤ CMD_tlnoremenu:    ਹਾਲਤ CMD_tlunmenu:
	ਹਾਲਤ CMD_tmenu:				    ਹਾਲਤ CMD_tunmenu:
	ਹਾਲਤ CMD_popup:	    ਹਾਲਤ CMD_tearoff:	    ਹਾਲਤ CMD_emenu:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_menu_cmd(xp, cmd, arg, forceit);
#ਜੇ_ਅੰਤ

	ਹਾਲਤ CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_runtime:
	    ਸੈੱਟ_context_ਵਿੱਚ_runtime_cmd(xp, arg);
	    ਰੋਕੋ;

#ਜੇ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	ਹਾਲਤ CMD_language:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_lang_cmd(xp, arg);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_PROFILE)
	ਹਾਲਤ CMD_profile:
	    ਸੈੱਟ_context_ਵਿੱਚ_profile_cmd(xp, arg);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;
#ਜੇ defined(FEAT_PROFILE)
	ਹਾਲਤ CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ CMD_argdelete:
	    ਜਦੋਂ ((xp->xp_ਨਮੂਨਾ = vim_strchr(arg, ' ')) != ਨਿਰਾ)
		arg = xp->xp_ਨਮੂਨਾ + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_ਨਮੂਨਾ = arg;
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਹਾਲਤ CMD_breakadd:
	ਹਾਲਤ CMD_profdel:
	ਹਾਲਤ CMD_breakdel:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_breakadd_cmd(xp, arg, cmdidx);

	ਹਾਲਤ CMD_scriptnames:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_scriptnames_cmd(xp, arg);
#ਜੇ_ਅੰਤ
	ਹਾਲਤ CMD_filetype:
	    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_filetype_cmd(xp, arg);

	ਮੂਲ:
	    ਰੋਕੋ;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    ਸਥਿਰ char_u *
ਸੈੱਟ_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    ਪੂਰਨ			len = 0;
    exarg_T		ea;
    ਪੂਰਨ			compl = EXPAND_NOTHING;
    ਪੂਰਨ			forceit = FALSE;
    ਪੂਰਨ			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_ਨਮੂਨਾ = buff;
    xp->xp_ਸਤਰ = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    ਲਈ (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != ਨਿਰਾ; cmd++)
	;
    xp->xp_ਨਮੂਨਾ = cmd;

    ਜੇ (*cmd == NUL)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	ਵਾਪਸ ਨਿਰਾ;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_ਨਮੂਨਾ = cmd;
    ਜੇ (*cmd == NUL)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (*cmd == '|' || *cmd == '\n')
	ਵਾਪਸ cmd + 1;			// There's another command

    // Get the command index.
    p = ਸੈੱਟ_cmd_ਸੂਚਕਾਂਕ(cmd, &ea, xp, &compl);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    ਜੇ (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    ਜੇ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    ਜੇ ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	ਜਦੋਂ (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    ਜਦੋਂ (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    ਜੇ (*p == NUL)
	    {
		ਜੇ (ea.argt & EX_ARGOPT)
		    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_argopt(xp, arg + 2);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
		ਜੇ (ea.cmdidx == CMD_terminal)
		    ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_terminalopt(xp, arg + 2);
#ਜੇ_ਅੰਤ
	    }

	    arg = skipwhite(p);
	}
    }

    ਜੇ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	ਜੇ (*arg == '>')			// append
	{
	    ਜੇ (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	ਨਹੀਂਤਾਂ ਜੇ (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    ਜੇ (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	ਜੇ (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    ਜੇ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ਜਦੋਂ (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    ਜੇ ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	ਜੇ (*arg == NUL)
	    ਵਾਪਸ p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    ਜੇ ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	ਜੇ (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	ਜਦੋਂ (*p)
	{
	    ਜੇ (*p == Ctrl_V)
	    {
		ਜੇ (p[1] != NUL)
		    ++p;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		ਜੇ (*(p - 1) != '\\')
		{
		    ਜੇ (*p == '|' || *p == '\n')
			ਵਾਪਸ p + 1;
		    ਵਾਪਸ ਨਿਰਾ;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    ਜੇ (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == ਨਿਰਾ)
	// no arguments allowed but there is something
	ਵਾਪਸ ਨਿਰਾ;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_ਨਮੂਨਾ = p;
    len = (ਪੂਰਨ)STRLEN(buff);
    ਜਦੋਂ (*p && p < buff + len)
    {
	ਜੇ (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_ਨਮੂਨਾ = ++p;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    ਜੇ (ea.argt & EX_XFILE)
	ਸੈੱਟ_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    ਵਾਪਸ ਸੈੱਟ_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    ਖਾਲੀ
ਸੈੱਟ_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    ਪੂਰਨ		len,	    // length of command line (excl. NUL)
    ਪੂਰਨ		ਖਾਨਾ,	    // position of cursor
    ਪੂਰਨ		use_ccline ਅਣਵਰਤਿਆ) // use ccline for info
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    ਪੂਰਨ			context;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    ਜੇ (ਖਾਨਾ < len)
    {
	old_char = str[ਖਾਨਾ];
	str[ਖਾਨਾ] = NUL;
    }
    nextcomm = str;

    filetype_expand_what = EXP_FILETYPECMD_ALL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    breakpt_expand_what = EXP_BREAKPT_ਜੋੜੋ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	ਸੈੱਟ_context_for_ਵਿਅੰਜਕ(xp, str, CMD_ਮਾਪ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (use_ccline && ccline->ਨਿਵੇਸ਼_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_ਨਮੂਨਾ = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	ਜੇ (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    ਸੈੱਟ_context_for_wildcard_arg(ਨਿਰਾ, xp->xp_ਨਮੂਨਾ, FALSE, xp,
								     &context);
	}
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜਦੋਂ (nextcomm != ਨਿਰਾ)
	    nextcomm = ਸੈੱਟ_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_ਸਤਰ = str;
    xp->xp_ਖਾਨਾ = ਖਾਨਾ;

    ਜੇ (ਖਾਨਾ < len)
	str[ਖਾਨਾ] = old_char;
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
    ਪੂਰਨ
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    ਪੂਰਨ		ਖਾਨਾ,		// position of cursor
    ਪੂਰਨ		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = ਨਿਰਾ;
    ਪੂਰਨ		options = WILD_ਜੋੜੋ_SLASH|WILD_SILENT;

    ਜੇ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ਵਾਪਸ EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    ਜੇ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ਵਾਪਸ EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_ਨਮੂਨਾ_len = (ਪੂਰਨ)(str + ਖਾਨਾ - xp->xp_ਨਮੂਨਾ);
    {
	char_u	*pat_src = xp->xp_ਨਮੂਨਾ;
	ਪੂਰਨ	pat_len = xp->xp_ਨਮੂਨਾ_len;
#ਜੇ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	ਜੇ (unesc != ਨਿਰਾ)
	    pat_src = unesc;
#ਜੇ_ਅੰਤ

	ਜੇ (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	ਨਹੀਂਤਾਂ
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#ਜੇ defined(FEAT_EVAL)
	vim_free(unesc);
#ਜੇ_ਅੰਤ
    }
    ਜੇ (file_str == ਨਿਰਾ)
	ਵਾਪਸ EXPAND_UNSUCCESSFUL;

    ਜੇ (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    ਜੇ (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = ਨਿਰਾ;
    }
    vim_free(file_str);

    ਵਾਪਸ EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	ਪੂਰਨ		*ਅੰਕ_Matches,
	ਪੂਰਨ		flags,
	ਪੂਰਨ		options)
{
    ਪੂਰਨ		free_pat = FALSE;
    ਪੂਰਨ		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    ਜੇ (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_ਸਿਰਾ;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	ਜੇ (pat == ਨਿਰਾ)
	    ਵਾਪਸ ret;

	pat_ਸਿਰਾ = pat + pat_len;
	ਲਈ (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    ਜੇ (*p != '\\')
		ਜਾਰੀ;

	    ਜੇ (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_ਸਿਰਾ - from) + 1);   // +1 for NUL
		pat_ਸਿਰਾ -= 3;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_ਸਿਰਾ - from) + 1);   // +1 for NUL
		--pat_ਸਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_backslash & XP_BS_COMMA)
	    {
		ਜੇ (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_ਸਿਰਾ - from) + 1);   // +1 for NUL
		    pat_ਸਿਰਾ -= 2;
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
		ਨਹੀਂਤਾਂ ਜੇ (*(p + 1) == ',')
		{
		    from = p + 1;
		    mch_memmove(p, from,
			(size_t)(pat_ਸਿਰਾ - from) + 1);   // +1 for NUL
		    --pat_ਸਿਰਾ;
		}
#ਜੇ_ਅੰਤ
	    }
	}
    }

    ਜੇ (xp->xp_context == EXPAND_FINDFUNC)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, ਅੰਕ_Matches);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	ਨਹੀਂਤਾਂ ਜੇ (xp->xp_context == EXPAND_FILES_ਵਿੱਚ_ਰਸਤਾ)
	    flags |= (EW_FILE | EW_ਰਸਤਾ);
	ਨਹੀਂਤਾਂ ਜੇ (xp->xp_context == EXPAND_DIRS_ਵਿੱਚ_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	ਨਹੀਂਤਾਂ
	    flags = (flags | EW_DIR) & ~EW_FILE;
	ਜੇ (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, ਅੰਕ_Matches, matches, flags);
    }
    ਜੇ (free_pat)
	vim_free(pat);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    ਜੇ (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	ਪੂਰਨ j;

	ਲਈ (j = 0; j < *ਅੰਕ_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    ਜਦੋਂ (*ptr != NUL)
	    {
		ਜੇ (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		ਨਹੀਂਤਾਂ ਜੇ (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    ਸਥਿਰ char_u *
get_behave_arg(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx == 0)
	ਵਾਪਸ (char_u *)"mswin";
    ਜੇ (idx == 1)
	ਵਾਪਸ (char_u *)"xterm";
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    ਸਥਿਰ char_u *
get_filetypecmd_arg(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx < 0)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	ਅੱਖਰ	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	ਵਾਪਸ (char_u *)opts_all[idx];
    }
    ਜੇ (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	ਅੱਖਰ	*opts_plugin[] = {"plugin", "on", "off"};
	ਵਾਪਸ (char_u *)opts_plugin[idx];
    }
    ਜੇ (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	ਅੱਖਰ	*opts_indent[] = {"indent", "on", "off"};
	ਵਾਪਸ (char_u *)opts_indent[idx];
    }
    ਜੇ (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	ਅੱਖਰ	*opts_onoff[] = {"on", "off"};
	ਵਾਪਸ (char_u *)opts_onoff[idx];
    }
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    ਸਥਿਰ char_u *
get_breakadd_arg(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx >= 0 && idx <= 3)
    {
	ਅੱਖਰ	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	ਜੇ (breakpt_expand_what == EXP_BREAKPT_ਜੋੜੋ)
	    ਵਾਪਸ (char_u *)opts[idx];
	ਨਹੀਂਤਾਂ ਜੇ (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    ਜੇ (idx <= 2)
		ਵਾਪਸ (char_u *)opts[idx + 1];
	}
	ਨਹੀਂਤਾਂ
	{
	    // profdel {func, file}
	    ਜੇ (idx <= 1)
		ਵਾਪਸ (char_u *)opts[idx + 1];
	}
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    ਸਥਿਰ char_u *
get_scriptnames_arg(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    scriptitem_T *si;

    ਜੇ (!SCRIPT_ID_VALID(idx + 1))
	ਵਾਪਸ ਨਿਰਾ;

    si = SCRIPT_ਵਸਤੂ(idx + 1);
    home_replace(ਨਿਰਾ, si->sn_name, NameBuff, MAXPATHL, TRUE);
    ਵਾਪਸ NameBuff;
}
#ਜੇ_ਅੰਤ

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    ਸਥਿਰ char_u *
get_retab_arg(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx == 0)
	ਵਾਪਸ (char_u *)"-indentonly";
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    ਸਥਿਰ char_u *
get_messages_arg(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx == 0)
	ਵਾਪਸ (char_u *)"clear";
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ char_u *
get_mapclear_arg(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx == 0)
	ਵਾਪਸ (char_u *)"<buffer>";
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    ਸਥਿਰ ਪੂਰਨ
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	ਪੂਰਨ		*ਅੰਕ_Matches)
{
    ਸਥਿਰ ਬਣਤਰ expgen
    {
	ਪੂਰਨ		context;
	char_u	*((*func)(expand_T *, ਪੂਰਨ));
	ਪੂਰਨ		ic;
	ਪੂਰਨ		escaped;
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	{EXPAND_USER_VARS, get_user_var_name, FALSE, TRUE},
	{EXPAND_FUNCTIONS, get_function_name, FALSE, TRUE},
	{EXPAND_USER_FUNC, get_user_func_name, FALSE, TRUE},
	{EXPAND_DISASSEMBLE, get_disassemble_ਦਲੀਲ, FALSE, TRUE},
	{EXPAND_ਵਿਅੰਜਕ, get_ਵਿਅੰਜਕ_name, FALSE, TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	{EXPAND_MENUS, get_menu_name, FALSE, TRUE},
	{EXPAND_MENUNAMES, get_menu_ਨਾਂ, FALSE, TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	{EXPAND_SYNTAX, get_syntax_name, TRUE, TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	{EXPAND_SYNTIME, get_syntime_arg, TRUE, TRUE},
#ਜੇ_ਅੰਤ
	{EXPAND_HIGHLIGHT, get_highlight_name, TRUE, TRUE},
	{EXPAND_EVENTS, get_event_name, TRUE, FALSE},
	{EXPAND_AUGROUP, get_augroup_name, TRUE, FALSE},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
	{EXPAND_CSCOPE, get_cscope_name, TRUE, TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	{EXPAND_SIGN, get_sign_name, TRUE, TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	{EXPAND_PROFILE, get_profile_name, TRUE, TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	{EXPAND_LANGUAGE, get_lang_arg, TRUE, FALSE},
	{EXPAND_LOCALES, get_locales, TRUE, FALSE},
#ਜੇ_ਅੰਤ
	{EXPAND_ENV_VARS, get_env_name, TRUE, TRUE},
	{EXPAND_USER, get_users, TRUE, FALSE},
	{EXPAND_ARGLIST, get_arglist_name, TRUE, FALSE},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	{EXPAND_BREAKPOINT, get_breakadd_arg, TRUE, TRUE},
	{EXPAND_SCRIPTNAMES, get_scriptnames_arg, TRUE, FALSE},
#ਜੇ_ਅੰਤ
	{EXPAND_RETAB, get_retab_arg, TRUE, TRUE},
    };
    ਪੂਰਨ	i;
    ਪੂਰਨ ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(tab); ++i)
    {
	ਜੇ (xp->xp_context == tab[i].context)
	{
	    ਜੇ (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, ਅੰਕ_Matches,
						tab[i].func, tab[i].escaped);
	    ਰੋਕੋ;
	}
    }

    ਵਾਪਸ ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    ਸਥਿਰ ਪੂਰਨ
map_wildopts_ਮੰਜ਼ਿਲ_ewflags(ਪੂਰਨ options)
{
    ਪੂਰਨ		flags;

    flags = EW_DIR;	// include directories
    ਜੇ (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    ਜੇ (options & WILD_ਜੋੜੋ_SLASH)
	flags |= EW_ADDSLASH;
    ਜੇ (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    ਜੇ (options & WILD_SILENT)
	flags |= EW_SILENT;
    ਜੇ (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    ਜੇ (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    ਵਾਪਸ flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    ਸਥਿਰ ਪੂਰਨ
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    ਪੂਰਨ		*ਅੰਕ_Matches,
    ਪੂਰਨ		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    ਪੂਰਨ		ret;
    ਪੂਰਨ		flags;
    char_u	*tofree = ਨਿਰਾ;
    ਪੂਰਨ		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_ਮੰਜ਼ਿਲ_ewflags(options);

    ਜੇ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_ਵਿੱਚ_ਰਸਤਾ
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_ਵਿੱਚ_CDPATH)
	ਵਾਪਸ expand_files_and_dirs(xp, pat, matches, ਅੰਕ_Matches, flags,
								options);

    *matches = (char_u **)"";
    *ਅੰਕ_Matches = 0;
    ਜੇ (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	ਜੇ (ਲੱਭੋ_help_ਟੈਗ_ਸੂਚੀ(*pat == NUL ? (char_u *)"help" : pat,
					ਅੰਕ_Matches, matches, FALSE) == OK)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	    cleanup_help_ਟੈਗ_ਸੂਚੀ(*ਅੰਕ_Matches, *matches);
#ਜੇ_ਅੰਤ
	    ਵਾਪਸ OK;
	}
	ਵਾਪਸ FAIL;
    }

    ਜੇ (xp->xp_context == EXPAND_SHELLCMD)
	ਵਾਪਸ expand_shellcmd(pat, matches, ਅੰਕ_Matches, flags);
    ਜੇ (xp->xp_context == EXPAND_OLD_SETTING)
	ਵਾਪਸ ExpandOldSetting(ਅੰਕ_Matches, matches);
    ਜੇ (xp->xp_context == EXPAND_BUFFERS)
	ਵਾਪਸ ExpandBufnames(pat, ਅੰਕ_Matches, matches, options);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (xp->xp_context == EXPAND_DIFF_BUFFERS)
	ਵਾਪਸ ExpandBufnames(pat, ਅੰਕ_Matches, matches,
						options | BUF_DIFF_FILTER);
#ਜੇ_ਅੰਤ
    ਜੇ (xp->xp_context == EXPAND_ਟੈਗ_ਸੂਚੀ
	    || xp->xp_context == EXPAND_ਟੈਗ_ਸੂਚੀ_LISTFILES)
	ਵਾਪਸ expand_ਟੈਗ_ਸੂਚੀ(xp->xp_context == EXPAND_ਟੈਗ_ਸੂਚੀ, pat, ਅੰਕ_Matches,
								matches);
    ਜੇ (xp->xp_context == EXPAND_COLORS)
    {
	ਅੱਖਰ *directories[] = {"colors", ਨਿਰਾ};
	ਵਾਪਸ ExpandRTDir(pat, DIP_ਸ਼ੁਰੂ + DIP_OPT, ਅੰਕ_Matches, matches,
								directories);
    }
    ਜੇ (xp->xp_context == EXPAND_COMPILER)
    {
	ਅੱਖਰ *directories[] = {"compiler", ਨਿਰਾ};
	ਵਾਪਸ ExpandRTDir(pat, 0, ਅੰਕ_Matches, matches, directories);
    }
    ਜੇ (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	ਅੱਖਰ *directories[] = {"syntax", ਨਿਰਾ};
	ਵਾਪਸ ExpandRTDir(pat, 0, ਅੰਕ_Matches, matches, directories);
    }
    ਜੇ (xp->xp_context == EXPAND_FILETYPE)
    {
	ਅੱਖਰ *directories[] = {"syntax", "indent", "ftplugin", ਨਿਰਾ};
	ਵਾਪਸ ExpandRTDir(pat, 0, ਅੰਕ_Matches, matches, directories);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
    ਜੇ (xp->xp_context == EXPAND_KEYMAP)
    {
	ਅੱਖਰ *directories[] = {"keymap", ਨਿਰਾ};
	ਵਾਪਸ ExpandRTDir(pat, 0, ਅੰਕ_Matches, matches, directories);
    }
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_EVAL)
    ਜੇ (xp->xp_context == EXPAND_USER_LIST)
	ਵਾਪਸ ExpandUserList(xp, matches, ਅੰਕ_Matches);
#ਜੇ_ਅੰਤ
    ਜੇ (xp->xp_context == EXPAND_PACKADD)
	ਵਾਪਸ Expand_Pack_ਜੋੜੋ_Dir(pat, ਅੰਕ_Matches, matches);
    ਜੇ (xp->xp_context == EXPAND_RUNTIME)
	ਵਾਪਸ expand_runtime_cmd(pat, ਅੰਕ_Matches, matches);
    ਜੇ (xp->xp_context == EXPAND_ਨਮੂਨਾ_ਵਿੱਚ_BUF)
	ਵਾਪਸ expand_ਨਮੂਨਾ_ਵਿੱਚ_buf(pat, xp->xp_ਖੋਜ_dir,
		matches, ਅੰਕ_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    ਜੇ ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	ਪੂਰਨ len = (ਪੂਰਨ)STRLEN(pat) + 20;

	tofree = alloc(len);
	ਜੇ (tofree == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	vim_snprintf((ਅੱਖਰ *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    ਜੇ (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	ਜੇ (regmatch.regprog == ਨਿਰਾ)
	{
	    vim_free(tofree);
	    ਵਾਪਸ FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    ਜੇ (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, ਅੰਕ_Matches, matches, fuzzy);
    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, ਅੰਕ_Matches, matches);
    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_context == EXPAND_SETTING_ਘਟਾਓ)
	ret = Expand_Setting_ਘਟਾਓ(xp, &regmatch, ਅੰਕ_Matches, matches);
    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, ਅੰਕ_Matches, matches);
    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, ਅੰਕ_Matches);
    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_context == EXPAND_HIGHLIGHT_ਸਮੂਹ)
	ret = expand_highlight_ਸਮੂਹ(pat, xp, &regmatch, matches, ਅੰਕ_Matches);
#ਜੇ defined(FEAT_TERMINAL)
    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, ਅੰਕ_Matches);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_EVAL)
    ਨਹੀਂਤਾਂ ਜੇ (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, ਅੰਕ_Matches);
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
	ret = ExpandOther(pat, xp, &regmatch, matches, ਅੰਕ_Matches);

    ਜੇ (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    ਵਾਪਸ ret;
}

    ਪੂਰਨ
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ਪੂਰਨ		*ਅੰਕ_Matches,
    char_u	*(*func)(expand_T *, ਪੂਰਨ),
					  // returns a string from the list
    ਪੂਰਨ		escaped)
{
    ਵਾਪਸ ExpandGenericExt(
	pat, xp, regmatch, matches, ਅੰਕ_Matches, func, escaped, 0);
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
    ਪੂਰਨ
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ਪੂਰਨ		*ਅੰਕ_Matches,
    char_u	*(*func)(expand_T *, ਪੂਰਨ),
					  // returns a string from the list
    ਪੂਰਨ		escaped,
    ਪੂਰਨ		ਛਾਂਟੋ_ਸ਼ੁਰੂ_Idx)
{
    ਪੂਰਨ		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = ਨਿਰਾ;
    ਪੂਰਨ		score = 0;
    ਪੂਰਨ		fuzzy;
    ਪੂਰਨ		ਮੇਲ;
    ਪੂਰਨ		ਛਾਂਟੋ_matches = FALSE;
    ਪੂਰਨ		funcsort = FALSE;
    ਪੂਰਨ		ਛਾਂਟੋ_ਸ਼ੁਰੂ_ਮੇਲ_Idx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ਨਿਰਾ;
    *ਅੰਕ_Matches = 0;

    ਜੇ (!fuzzy)
	ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ *), 30);
    ਨਹੀਂਤਾਂ
	ga_init2(&ga, ਆਕਾਰ(fuzmatch_str_T), 30);

    ਲਈ (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	ਜੇ (str == ਨਿਰਾ)	    // end of list
	    ਰੋਕੋ;
	ਜੇ (*str == NUL)	    // skip empty strings
	    ਜਾਰੀ;

	ਜੇ (xp->xp_ਨਮੂਨਾ[0] != NUL)
	{
	    ਜੇ (!fuzzy)
		ਮੇਲ = vim_regexec(regmatch, str, (colnr_T)0);
	    ਨਹੀਂਤਾਂ
	    {
		score = fuzzy_ਮੇਲ_str(str, pat);
		ਮੇਲ = (score != FUZZY_SCORE_NONE);
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਮੇਲ = TRUE;

	ਜੇ (!ਮੇਲ)
	    ਜਾਰੀ;

	ਜੇ (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	ਨਹੀਂਤਾਂ
	    str = vim_strsave(str);
	ਜੇ (str == ਨਿਰਾ)
	{
	    ਜੇ (!fuzzy)
	    {
		ga_clear_ਲੜੀਆਂ(&ga);
		ਵਾਪਸ FAIL;
	    }
	    fuzmatch_str_free(ga.ga_ਅੰਕੜੇ, ga.ga_len);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    ਰੋਕੋ;
	}

	ਜੇ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_ਅੰਕੜੇ)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	ਨਹੀਂਤਾਂ
	    ((char_u **)ga.ga_ਅੰਕੜੇ)[ga.ga_len] = str;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	ਜੇ (func == get_menu_ਨਾਂ)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    ਜੇ (*str == '\001')
		*str = '.';
	}
#ਜੇ_ਅੰਤ

	ਜੇ (ਛਾਂਟੋ_ਸ਼ੁਰੂ_Idx >= 0 && i >= ਛਾਂਟੋ_ਸ਼ੁਰੂ_Idx && ਛਾਂਟੋ_ਸ਼ੁਰੂ_ਮੇਲ_Idx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    ਛਾਂਟੋ_ਸ਼ੁਰੂ_ਮੇਲ_Idx = ga.ga_len;
	}

	++ga.ga_len;
    }

    ਜੇ (ga.ga_len == 0)
	ਵਾਪਸ OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    ਜੇ (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	ਛਾਂਟੋ_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    ਜੇ (xp->xp_context == EXPAND_ਵਿਅੰਜਕ
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    ਜੇ (ਛਾਂਟੋ_matches && ਛਾਂਟੋ_ਸ਼ੁਰੂ_ਮੇਲ_Idx != -1)
    {
	ਜੇ (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((ਖਾਲੀ *)ga.ga_ਅੰਕੜੇ, (size_t)ga.ga_len, ਆਕਾਰ(char_u *),
							   ਛਾਂਟੋ_func_compare);
	ਨਹੀਂਤਾਂ
	    ਛਾਂਟੋ_ਲੜੀਆਂ((char_u **)ga.ga_ਅੰਕੜੇ + ਛਾਂਟੋ_ਸ਼ੁਰੂ_ਮੇਲ_Idx, ga.ga_len - ਛਾਂਟੋ_ਸ਼ੁਰੂ_ਮੇਲ_Idx);
    }

    ਜੇ (!fuzzy)
    {
	*matches = ga.ga_ਅੰਕੜੇ;
	*ਅੰਕ_Matches = ga.ga_len;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (fuzzymatches_ਮੰਜ਼ਿਲ_strmatches(ga.ga_ਅੰਕੜੇ, matches, ga.ga_len,
							funcsort) == FAIL)
	    ਵਾਪਸ FAIL;
	*ਅੰਕ_Matches = ga.ga_len;
    }

#ਜੇ defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    ਸਥਿਰ ਖਾਲੀ
expand_shellcmd_onedir(
	char_u		*pathed_ਨਮੂਨਾ,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	ਪੂਰਨ		*ਅੰਕ_Matches,
	ਪੂਰਨ		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    ਜੇ (expand_wildcards(1, &pathed_ਨਮੂਨਾ, ਅੰਕ_Matches, matches, flags) != OK)
	ਵਾਪਸ;

    ਜੇ (ga_grow(gap, *ਅੰਕ_Matches) == FAIL)
    {
	FreeWild(*ਅੰਕ_Matches, *matches);
	ਵਾਪਸ;
    }

    ਲਈ (ਪੂਰਨ i = 0; i < *ਅੰਕ_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	ਜੇ (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] = name;
		hash_ਜੋੜੋ_ਵਸਤੂ(ht, hi, name, hash);
		name = ਨਿਰਾ;
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
    ਸਥਿਰ ਪੂਰਨ
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    ਪੂਰਨ		*ਅੰਕ_Matches,	// return: number of matches
    ਪੂਰਨ		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*ਰਸਤਾ = ਨਿਰਾ;
    ਪੂਰਨ		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    ਪੂਰਨ		flags = flagsarg;
    ਪੂਰਨ		did_curdir = FALSE;
    hashtab_T	ਮਿਲਿਆ_ht;

    buf = alloc(MAXPATHL);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    ਜੇ (pat == ਨਿਰਾ)
    {
	vim_free(buf);
	ਵਾਪਸ FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    ਲਈ (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	ਜੇ (*s != '\\')
	    ਜਾਰੀ;

	p = s + 1;
	ਜੇ (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    ਜੇ (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	ਰਸਤਾ = (char_u *)".";
    ਨਹੀਂਤਾਂ
    {
	// For an absolute name we don't use $PATH.
	ਜੇ (!mch_is_ਭਰਿਆ_Name(pat))
	    ਰਸਤਾ = vim_getenv((char_u *)"PATH", &mustfree);
	ਜੇ (ਰਸਤਾ == ਨਿਰਾ)
	    ਰਸਤਾ = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ *), 10);
    hash_init(&ਮਿਲਿਆ_ht);
    ਲਈ (s = ਰਸਤਾ; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	ਜੇ (*s == NUL)
	{
	    ਜੇ (did_curdir)
		ਰੋਕੋ;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	ਨਹੀਂਤਾਂ
	{
#ਜੇ defined(MSWIN)
	    e = vim_strchr(s, ';');
#ਨਹੀਂਤਾਂ
	    e = vim_strchr(s, ':');
#ਜੇ_ਅੰਤ
	    ਜੇ (e == ਨਿਰਾ)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    ਜੇ (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    ਨਹੀਂਤਾਂ
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? ਆਕਾਰ(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	ਜੇ (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    ਜੇ (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		ਜੇ (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, ਅੰਕ_Matches, flags,
				    &ਮਿਲਿਆ_ht, &ga);
	}

	ਜੇ (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_ਅੰਕੜੇ;
    *ਅੰਕ_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    ਜੇ (mustfree)
	vim_free(ਰਸਤਾ);
    hash_clear(&ਮਿਲਿਆ_ht);
    ਵਾਪਸ OK;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    ਸਥਿਰ ਖਾਲੀ *
call_user_expand_func(
    ਖਾਲੀ	*(*user_expand_func)(char_u *, ਪੂਰਨ, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ਪੂਰਨ		keep = 0;
    typval_T	ਦਲੀਲਾਂ[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = ਨਿਰਾ;
    ਖਾਲੀ	*ret;

    ਜੇ (xp->xp_arg == ਨਿਰਾ || xp->xp_arg[0] == '\0' || xp->xp_ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (ccline->cmdbuff != ਨਿਰਾ)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	ਪੂਰਨ unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_ਨਮੂਨਾ,
						  xp->xp_ਨਮੂਨਾ_len, &unesc_len);
	ਜੇ (pat == ਨਿਰਾ)
	    pat = vim_strnsave(xp->xp_ਨਮੂਨਾ, xp->xp_ਨਮੂਨਾ_len);
    }

    ਦਲੀਲਾਂ[0].v_type = VAR_STRING;
    ਦਲੀਲਾਂ[0].vval.v_string = pat;
    ਦਲੀਲਾਂ[1].v_type = VAR_STRING;
    ਦਲੀਲਾਂ[1].vval.v_string = xp->xp_ਸਤਰ;
    ਦਲੀਲਾਂ[2].v_type = VAR_ਸੰਖਿਆ;
    ਦਲੀਲਾਂ[2].vval.v_ਸੰਖਿਆ = xp->xp_ਖਾਨਾ;
    ਦਲੀਲਾਂ[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, ਦਲੀਲਾਂ);

    current_sctx = save_current_sctx;
    ਜੇ (ccline->cmdbuff != ਨਿਰਾ)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    ਵਾਪਸ ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    ਸਥਿਰ ਪੂਰਨ
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ਪੂਰਨ		*ਅੰਕ_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    ਪੂਰਨ		keep;
    garray_T	ga;
    ਪੂਰਨ		fuzzy;
    ਪੂਰਨ		ਮੇਲ;
    ਪੂਰਨ		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ਨਿਰਾ;
    *ਅੰਕ_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    ਜੇ (retstr == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (!fuzzy)
	ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ *), 3);
    ਨਹੀਂਤਾਂ
	ga_init2(&ga, ਆਕਾਰ(fuzmatch_str_T), 3);

    ਲਈ (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	ਜੇ (e == ਨਿਰਾ)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	ਜੇ (xp->xp_ਨਮੂਨਾ[0] != NUL)
	{
	    ਜੇ (!fuzzy)
		ਮੇਲ = vim_regexec(regmatch, s, (colnr_T)0);
	    ਨਹੀਂਤਾਂ
	    {
		score = fuzzy_ਮੇਲ_str(s, pat);
		ਮੇਲ = (score != FUZZY_SCORE_NONE);
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਮੇਲ = TRUE;		// match everything

	*e = keep;

	ਜੇ (ਮੇਲ)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    ਜੇ (p == ਨਿਰਾ)
		ਰੋਕੋ;

	    ਜੇ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		ਰੋਕੋ;
	    }

	    ਜੇ (!fuzzy)
		((char_u **)ga.ga_ਅੰਕੜੇ)[ga.ga_len] = p;
	    ਨਹੀਂਤਾਂ
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_ਅੰਕੜੇ)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	ਜੇ (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    ਜੇ (ga.ga_len == 0)
	ਵਾਪਸ OK;

    ਜੇ (!fuzzy)
    {
	*matches = ga.ga_ਅੰਕੜੇ;
	*ਅੰਕ_Matches = ga.ga_len;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (fuzzymatches_ਮੰਜ਼ਿਲ_strmatches(ga.ga_ਅੰਕੜੇ, matches, ga.ga_len,
								FALSE) == FAIL)
	    ਵਾਪਸ FAIL;
	*ਅੰਕ_Matches = ga.ga_len;
    }
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    ਪੂਰਨ		*ਅੰਕ_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_ਕਿਸਮ;
    garray_T	ga_menu;
    garray_T	ga_info;
    ਪੂਰਨ		have_extra = FALSE;
    ਪੂਰਨ		i;

    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ *), 3);
    ga_init2(&ga_abbr, ਆਕਾਰ(ਅੱਖਰ *), 3);
    ga_init2(&ga_ਕਿਸਮ, ਆਕਾਰ(ਅੱਖਰ *), 3);
    ga_init2(&ga_menu, ਆਕਾਰ(ਅੱਖਰ *), 3);
    ga_init2(&ga_info, ਆਕਾਰ(ਅੱਖਰ *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = ਨਿਰਾ;
	char_u	*abbr = ਨਿਰਾ;
	char_u	*ਕਿਸਮ = ਨਿਰਾ;
	char_u	*menu = ਨਿਰਾ;
	char_u	*info = ਨਿਰਾ;

	ਜੇ (tv->v_type == VAR_STRING)
	{
	    ਜੇ (tv->vval.v_string == ਨਿਰਾ)
		ਜਾਰੀ;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_DICT && tv->vval.v_dict != ਨਿਰਾ)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*ਸ਼ਬਦ = dict_get_string(d, "word", FALSE);

	    ਜੇ (ਸ਼ਬਦ == ਨਿਰਾ)
		ਜਾਰੀ;  // "word" is required
	    p = vim_strsave(ਸ਼ਬਦ);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    ਕਿਸਮ = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    ਜੇ (abbr != ਨਿਰਾ || ਕਿਸਮ != ਨਿਰਾ || menu != ਨਿਰਾ || info != ਨਿਰਾ)
		have_extra = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    ਜਾਰੀ;  // Skip other types

	ਜੇ (p == ਨਿਰਾ
		|| ga_grow(&ga, 1) == FAIL
		|| ga_grow(&ga_abbr, 1) == FAIL
		|| ga_grow(&ga_ਕਿਸਮ, 1) == FAIL
		|| ga_grow(&ga_menu, 1) == FAIL
		|| ga_grow(&ga_info, 1) == FAIL)
	{
	    vim_free(p);
	    vim_free(abbr);
	    vim_free(ਕਿਸਮ);
	    vim_free(menu);
	    vim_free(info);
	    ਰੋਕੋ;
	}

	((char_u **)ga.ga_ਅੰਕੜੇ)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_ਅੰਕੜੇ)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_ਕਿਸਮ.ga_ਅੰਕੜੇ)[ga_ਕਿਸਮ.ga_len++] = ਕਿਸਮ;
	((char_u **)ga_menu.ga_ਅੰਕੜੇ)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_ਅੰਕੜੇ)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_ਅੰਕੜੇ;
    *ਅੰਕ_Matches = ga.ga_len;
    ਜੇ (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_ਅੰਕੜੇ;
	xp->xp_files_ਕਿਸਮ = (char_u **)ga_ਕਿਸਮ.ga_ਅੰਕੜੇ;
	xp->xp_files_menu = (char_u **)ga_menu.ga_ਅੰਕੜੇ;
	xp->xp_files_info = (char_u **)ga_info.ga_ਅੰਕੜੇ;
    }
    ਨਹੀਂਤਾਂ
    {
	// No extra info collected; free the placeholder NULL entries.
	ਲਈ (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_ਅੰਕੜੇ)[i]);
	vim_free(ga_abbr.ga_ਅੰਕੜੇ);
	ਲਈ (i = 0; i < ga_ਕਿਸਮ.ga_len; i++)
	    vim_free(((char_u **)ga_ਕਿਸਮ.ga_ਅੰਕੜੇ)[i]);
	vim_free(ga_ਕਿਸਮ.ga_ਅੰਕੜੇ);
	ਲਈ (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_ਅੰਕੜੇ)[i]);
	vim_free(ga_menu.ga_ਅੰਕੜੇ);
	ਲਈ (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_ਅੰਕੜੇ)[i]);
	vim_free(ga_info.ga_ਅੰਕੜੇ);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    ਸਥਿਰ ਪੂਰਨ
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    ਪੂਰਨ		*ਅੰਕ_Matches)
{
    list_T      *retlist;

    *matches = ਨਿਰਾ;
    *ਅੰਕ_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    ਜੇ (retlist == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    expand_process_user_list(retlist, matches, ਅੰਕ_Matches, xp);
    list_unref(retlist);
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    ਖਾਲੀ
globpath(
    char_u	*ਰਸਤਾ,
    char_u	*file,
    garray_T	*ga,
    ਪੂਰਨ		expand_options,
    ਪੂਰਨ		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#ਜੇ defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#ਨਹੀਂਤਾਂ
# define TMP_PATHSEPSTR PATHSEPSTR
#ਜੇ_ਅੰਤ

    // Loop over all entries in {path}.
    ਜਦੋਂ (*ਰਸਤਾ != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&ਰਸਤਾ, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	ਜੇ (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    ਪੂਰਨ	    ਅੰਕ_p;
	    char_u  **p;

	    ਜੇ (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    ਜੇ (ExpandFromContext(&xpc, buf, &p, &ਅੰਕ_p,
			     WILD_SILENT|expand_options) != FAIL && ਅੰਕ_p > 0)
	    {
		ExpandEscape(&xpc, buf, ਅੰਕ_p, p, WILD_SILENT|expand_options);

		ਜੇ (ga_grow(ga, ਅੰਕ_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_p; ++i)
		    {
			((char_u **)ga->ga_ਅੰਕੜੇ)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    FreeWild(ਅੰਕ_p, p);
		    p = ਨਿਰਾ;
		}
		vim_free(p);
	    }
	}
    }

    vim_free(buf);
}
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ TMP_PATHSEPSTR

/*
 * Translate some keys pressed when 'wildmenu' is used.
 */
    ਪੂਰਨ
wildmenu_translate_ਕੁੰਜੀ(
	cmdline_info_T	*cclp,
	ਪੂਰਨ		ਕੁੰਜੀ,
	expand_T	*xp,
	ਪੂਰਨ		did_wild_list)
{
    ਪੂਰਨ c = ਕੁੰਜੀ;

    ਜੇ (cmdline_pum_ਸਰਗਰਮ())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	ਚੋਣ (c)
	{
	    ਹਾਲਤ K_ਉੱਪਰ:	  c = K_ਖੱਬਾ; ਰੋਕੋ;
	    ਹਾਲਤ K_ਥੱਲੇ:  c = K_ਸੱਜਾ; ਰੋਕੋ;
	    ਹਾਲਤ K_ਖੱਬਾ:  c = K_ਉੱਪਰ; ਰੋਕੋ;
	    ਹਾਲਤ K_ਸੱਜਾ: c = K_ਥੱਲੇ; ਰੋਕੋ;
	    ਮੂਲ:	  ਰੋਕੋ;
	}
    }

    ਜੇ (cmdline_pum_ਸਰਗਰਮ() || did_wild_list || wild_menu_showing)
    {
	ਜੇ (c == K_ਖੱਬਾ)
	    c = Ctrl_P;
	ਨਹੀਂਤਾਂ ਜੇ (c == K_ਸੱਜਾ)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    ਜੇ (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_ਥੱਲੇ;

    ਵਾਪਸ c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    ਸਥਿਰ ਖਾਲੀ
cmdline_del(cmdline_info_T *cclp, ਪੂਰਨ from)
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
    ਸਥਿਰ ਪੂਰਨ
wildmenu_process_ਕੁੰਜੀ_menunames(cmdline_info_T *cclp, ਪੂਰਨ ਕੁੰਜੀ, expand_T *xp)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    ਜੇ (ਕੁੰਜੀ == K_ਥੱਲੇ && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	ਕੁੰਜੀ = p_wc;
	ਕੁੰਜੀ_Typed = TRUE;  // in case the key was mapped
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ == K_ਉੱਪਰ)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	ਪੂਰਨ ਮਿਲਿਆ = FALSE;

	j = (ਪੂਰਨ)(xp->xp_ਨਮੂਨਾ - cclp->cmdbuff);
	i = 0;
	ਜਦੋਂ (--j > 0)
	{
	    // check for start of menu name
	    ਜੇ (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		ਰੋਕੋ;
	    }
	    // check for start of submenu name
	    ਜੇ (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		ਜੇ (ਮਿਲਿਆ)
		{
		    i = j + 1;
		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ
		    ਮਿਲਿਆ = TRUE;
	    }
	}
	ਜੇ (i > 0)
	    cmdline_del(cclp, i);
	ਕੁੰਜੀ = p_wc;
	ਕੁੰਜੀ_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    ਵਾਪਸ ਕੁੰਜੀ;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    ਸਥਿਰ ਪੂਰਨ
wildmenu_process_ਕੁੰਜੀ_filenames(cmdline_info_T *cclp, ਪੂਰਨ ਕੁੰਜੀ, expand_T *xp)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    ਜੇ (ਕੁੰਜੀ == K_ਥੱਲੇ
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	ਕੁੰਜੀ = p_wc;
	ਕੁੰਜੀ_Typed = TRUE;  // in case the key was mapped
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(xp->xp_ਨਮੂਨਾ, upseg + 1, 3) == 0 && ਕੁੰਜੀ == K_ਥੱਲੇ)
    {
	// If in a direct ancestor, strip off one ../ to go down
	ਪੂਰਨ ਮਿਲਿਆ = FALSE;

	j = cclp->cmdpos;
	i = (ਪੂਰਨ)(xp->xp_ਨਮੂਨਾ - cclp->cmdbuff);
	ਜਦੋਂ (--j > i)
	{
	    ਜੇ (has_mbyte)
		j -= (*mb_ਸਿਰ_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    ਜੇ (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		ਮਿਲਿਆ = TRUE;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (ਮਿਲਿਆ
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    ਕੁੰਜੀ = p_wc;
	    ਕੁੰਜੀ_Typed = TRUE;  // in case the key was mapped
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ == K_ਉੱਪਰ)
    {
	// go up a directory
	ਪੂਰਨ ਮਿਲਿਆ = FALSE;

	j = cclp->cmdpos - 1;
	i = (ਪੂਰਨ)(xp->xp_ਨਮੂਨਾ - cclp->cmdbuff);
	ਜਦੋਂ (--j > i)
	{
	    ਜੇ (has_mbyte)
		j -= (*mb_ਸਿਰ_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    ਜੇ (vim_ispathsep(cclp->cmdbuff[j])
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == ਨਿਰਾ
#ਜੇ_ਅੰਤ
	       )
	    {
		ਜੇ (ਮਿਲਿਆ)
		{
		    i = j + 1;
		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ
		    ਮਿਲਿਆ = TRUE;
	    }
	}

	ਜੇ (!ਮਿਲਿਆ)
	    j = i;
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	ਨਹੀਂਤਾਂ
	    j = 0;
	ਜੇ (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	ਨਹੀਂਤਾਂ ਜੇ (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	ਕੁੰਜੀ = p_wc;
	ਕੁੰਜੀ_Typed = TRUE;
    }

    ਵਾਪਸ ਕੁੰਜੀ;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    ਪੂਰਨ
wildmenu_process_ਕੁੰਜੀ(cmdline_info_T *cclp, ਪੂਰਨ ਕੁੰਜੀ, expand_T *xp)
{
    ਜੇ (xp->xp_context == EXPAND_MENUNAMES)
	ਵਾਪਸ wildmenu_process_ਕੁੰਜੀ_menunames(cclp, ਕੁੰਜੀ, xp);
    ਨਹੀਂਤਾਂ ਜੇ ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	ਵਾਪਸ wildmenu_process_ਕੁੰਜੀ_filenames(cclp, ਕੁੰਜੀ, xp);

    ਵਾਪਸ ਕੁੰਜੀ;
}

/*
 * Free expanded names when finished walking through the matches
 */
    ਖਾਲੀ
wildmenu_cleanup(cmdline_info_T *cclp ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ skt = ਕੁੰਜੀ_Typed;

    ਜੇ (!p_wmnu || wild_menu_showing == 0)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ save_RedrawingDisabled = RedrawingDisabled;
    ਜੇ (cclp->ਨਿਵੇਸ਼_fn)
	RedrawingDisabled = 0;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_ਖੋਜ_EXTRA)
    // Clear highlighting applied during wildmenu activity
    ਸੈੱਟ_no_hlsearch(TRUE);
#ਜੇ_ਅੰਤ

    ਜੇ (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_ਪੰਕਤੀ--;
	redrawcmd();
    }
    ਨਹੀਂਤਾਂ ਜੇ (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	ਆਖਰੀ_ਸਥਿਤੀ(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    ਨਹੀਂਤਾਂ
    {
	win_redraw_ਆਖਰੀ_ਸਥਿਤੀ(topframe);
	redraw_statuslines();
    }
    ਕੁੰਜੀ_Typed = skt;
    wild_menu_showing = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (cclp->ਨਿਵੇਸ਼_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    ਖਾਲੀ
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    ਪੂਰਨ		filtered = FALSE;
    ਪੂਰਨ		options = WILD_SILENT | WILD_USE_NL | WILD_ਜੋੜੋ_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    pat = tv_get_string(&argvars[0]);
    ਜੇ (check_for_string_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;
    type = tv_get_string(&argvars[1]);

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], ਨਿਰਾ);

    ਜੇ (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    ਜੇ (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    ਜੇ (STRCMP(type, "cmdline") == 0)
    {
	ਪੂਰਨ cmdline_len = (ਪੂਰਨ)STRLEN(pat);
	ਸੈੱਟ_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_ਨਮੂਨਾ_len = (ਪੂਰਨ)STRLEN(xpc.xp_ਨਮੂਨਾ);
	xpc.xp_ਖਾਨਾ = cmdline_len;
    }
    ਨਹੀਂਤਾਂ
    {
	char_u	*ਨਮੂਨਾ_ਸ਼ੁਰੂ;

	xpc.xp_ਨਮੂਨਾ = ਨਮੂਨਾ_ਸ਼ੁਰੂ = pat;
	xpc.xp_ਨਮੂਨਾ_len = (ਪੂਰਨ)STRLEN(xpc.xp_ਨਮੂਨਾ);
	xpc.xp_ਸਤਰ = pat;

	xpc.xp_context = cmdcomplete_str_ਮੰਜ਼ਿਲ_type(type);
	ਚੋਣ (xpc.xp_context)
	{
	ਹਾਲਤ EXPAND_NOTHING:
	    semsg(_(e_invalid_ਦਲੀਲ_str), type);
	    ਵਾਪਸ;

	ਹਾਲਤ EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    ਜੇ (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_ਦਲੀਲ_str), type);
		ਵਾਪਸ;
	    }

	    xpc.xp_arg = type + 7;
	    ਰੋਕੋ;

	ਹਾਲਤ EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    ਜੇ (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_ਦਲੀਲ_str), type);
		ਵਾਪਸ;
	    }

	    xpc.xp_arg = type + 11;
	    ਰੋਕੋ;

# ਜੇ defined(FEAT_MENU)
	ਹਾਲਤ EXPAND_MENUS:
	    ਸੈੱਟ_context_ਵਿੱਚ_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_ਨਮੂਨਾ, FALSE);
	    xpc.xp_ਨਮੂਨਾ_len -= (ਪੂਰਨ)(xpc.xp_ਨਮੂਨਾ - ਨਮੂਨਾ_ਸ਼ੁਰੂ);
	    ਰੋਕੋ;
# endif

# ifdef FEAT_CSCOPE
	ਹਾਲਤ EXPAND_CSCOPE:
	    ਸੈੱਟ_context_ਵਿੱਚ_cscope_cmd(&xpc, xpc.xp_ਨਮੂਨਾ, CMD_cscope);
	    xpc.xp_ਨਮੂਨਾ_len -= (ਪੂਰਨ)(xpc.xp_ਨਮੂਨਾ - ਨਮੂਨਾ_ਸ਼ੁਰੂ);
	    ਰੋਕੋ;
# endif

# ifdef FEAT_SIGNS
	ਹਾਲਤ EXPAND_SIGN:
	    ਸੈੱਟ_context_ਵਿੱਚ_sign_cmd(&xpc, xpc.xp_ਨਮੂਨਾ);
	    xpc.xp_ਨਮੂਨਾ_len -= (ਪੂਰਨ)(xpc.xp_ਨਮੂਨਾ - ਨਮੂਨਾ_ਸ਼ੁਰੂ);
	    ਰੋਕੋ;
# endif

	ਹਾਲਤ EXPAND_RUNTIME:
	    ਸੈੱਟ_context_ਵਿੱਚ_runtime_cmd(&xpc, xpc.xp_ਨਮੂਨਾ);
	    xpc.xp_ਨਮੂਨਾ_len -= (ਪੂਰਨ)(xpc.xp_ਨਮੂਨਾ - ਨਮੂਨਾ_ਸ਼ੁਰੂ);
	    ਰੋਕੋ;

	ਹਾਲਤ EXPAND_SHELLCMDLINE:
	{
	    ਪੂਰਨ context = EXPAND_SHELLCMDLINE;
	    ਸੈੱਟ_context_for_wildcard_arg(ਨਿਰਾ, xpc.xp_ਨਮੂਨਾ, FALSE, &xpc,
					 &context);
	    xpc.xp_ਨਮੂਨਾ_len -= (ਪੂਰਨ)(xpc.xp_ਨਮੂਨਾ - ਨਮੂਨਾ_ਸ਼ੁਰੂ);
	    ਰੋਕੋ;
	}

	ਹਾਲਤ EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    ਰੋਕੋ;

	ਹਾਲਤ EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ਜੋੜੋ;
	    ਰੋਕੋ;

	ਮੂਲ:
	    ਰੋਕੋ;
	}
    }

    {
	char_u	*pat_src = xpc.xp_ਨਮੂਨਾ;
	ਪੂਰਨ	pat_len = xpc.xp_ਨਮੂਨਾ_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	ਜੇ (unesc != ਨਿਰਾ)
	    pat_src = unesc;

	ਜੇ (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	ਨਹੀਂਤਾਂ
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    ਜੇ (rettv_list_alloc(rettv) == OK && pat != ਨਿਰਾ)
    {
	ਪੂਰਨ	i;

	ExpandOne(&xpc, pat, ਨਿਰਾ, options, WILD_ALL_KEEP);

	ਲਈ (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    ਖਾਲੀ
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    ਪੂਰਨ		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (ਪੂਰਨ)STRLEN(pat);
    ਸੈੱਟ_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_ਮੰਜ਼ਿਲ_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    ਖਾਲੀ
f_cmdcomplete_info(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    ਪੂਰਨ		    idx;
    ਪੂਰਨ		    ret = OK;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL || ccline == ਨਿਰਾ
	    || ccline->xpc == ਨਿਰਾ || ccline->xpc->xp_files == ਨਿਰਾ)
	ਵਾਪਸ;
    retdict = rettv->vval.v_dict;
    ret = dict_ਜੋੜੋ_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (ਪੂਰਨ)cmdline_orig.ਲੰਮਾਈ);
    ਜੇ (ret == OK)
	ret = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "pum_visible", pum_visible());
    ਜੇ (ret == OK)
	ret = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "selected", ccline->xpc->xp_selected);
    ਜੇ (ret == OK)
    {
	li = list_alloc();
	ਜੇ (li == ਨਿਰਾ)
	    ਵਾਪਸ;
	ret = dict_ਜੋੜੋ_list(retdict, "matches", li);
	ਜੇ (ret == FAIL)
	    list_unref(li);
	ਲਈ (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
	    list_append_string(li, ccline->xpc->xp_files[idx], -1);
    }
}
#ਜੇ_ਅੰਤ // FEAT_EVAL

/*
 * Copy a substring from the current buffer (curbuf), spanning from the given
 * 'start' position to the word boundary after 'end' position.
 * The copied string is stored in '*match', and the actual end position of the
 * matched text is returned in '*match_end'.
 */
    ਸਥਿਰ ਪੂਰਨ
copy_substring_from_pos(pos_T *ਸ਼ੁਰੂ, pos_T *ਸਿਰਾ, char_u **ਮੇਲ,
	pos_T *ਮੇਲ_ਸਿਰਾ)
{
    char_u	*ਸ਼ਬਦ_ਸਿਰਾ;
    char_u	*ਸਤਰ, *ਸ਼ੁਰੂ_ਸਤਰ, *ਸਿਰਾ_ਸਤਰ;
    ਪੂਰਨ		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    ਪੂਰਨ		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ਨਿਰਾ;

    ਜੇ (ਸ਼ੁਰੂ->lnum > ਸਿਰਾ->lnum
	    || (ਸ਼ੁਰੂ->lnum == ਸਿਰਾ->lnum && ਸ਼ੁਰੂ->ਖਾਨਾ >= ਸਿਰਾ->ਖਾਨਾ))
	ਵਾਪਸ FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    ਸ਼ੁਰੂ_ਸਤਰ = ml_get(ਸ਼ੁਰੂ->lnum);
    char_u  *ਸ਼ੁਰੂ_ptr = ਸ਼ੁਰੂ_ਸਤਰ + ਸ਼ੁਰੂ->ਖਾਨਾ;
    ਪੂਰਨ	    is_single_ਸਤਰ = ਸ਼ੁਰੂ->lnum == ਸਿਰਾ->lnum;

    segment_len = is_single_ਸਤਰ ? (ਸਿਰਾ->ਖਾਨਾ - ਸ਼ੁਰੂ->ਖਾਨਾ)
			: (ਪੂਰਨ)(ml_get_len(ਸ਼ੁਰੂ->lnum) - ਸ਼ੁਰੂ->ਖਾਨਾ);
    ਜੇ (ga_grow(&ga, segment_len + 2) != OK)
	ਜਾਓ fail;

    ga_concat_len(&ga, ਸ਼ੁਰੂ_ptr, segment_len);
    ਜੇ (!is_single_ਸਤਰ)
    {
	ਜੇ (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	ਨਹੀਂਤਾਂ
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    ਜੇ (!is_single_ਸਤਰ)
    {
	ਲਈ (lnum = ਸ਼ੁਰੂ->lnum + 1; lnum < ਸਿਰਾ->lnum; lnum++)
	{
	    ਪੂਰਨ  linelen;

	    ਸਤਰ = ml_get(lnum);
	    linelen = (ਪੂਰਨ)ml_get_len(lnum);
	    ਜੇ (ga_grow(&ga, linelen + 2) != OK)
		ਜਾਓ fail;
	    ga_concat_len(&ga, ਸਤਰ, linelen);
	    ਜੇ (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    ਨਹੀਂਤਾਂ
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    ਸਿਰਾ_ਸਤਰ = ml_get(ਸਿਰਾ->lnum);
    ਸ਼ਬਦ_ਸਿਰਾ = ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(ਸਿਰਾ_ਸਤਰ + ਸਿਰਾ->ਖਾਨਾ);
    segment_len = (ਪੂਰਨ)(ਸ਼ਬਦ_ਸਿਰਾ - ਸਿਰਾ_ਸਤਰ);
    ਜੇ (ga_grow(&ga, segment_len) != OK)
	ਜਾਓ fail;
    ga_concat_len(&ga, ਸਿਰਾ_ਸਤਰ + (is_single_ਸਤਰ ? ਸਿਰਾ->ਖਾਨਾ : 0),
	    segment_len - (is_single_ਸਤਰ ? ਸਿਰਾ->ਖਾਨਾ : 0));

    // Null-terminate
    ਜੇ (ga_grow(&ga, 1) != OK)
	ਜਾਓ fail;
    ga_append(&ga, NUL);

    *ਮੇਲ = (char_u *)ga.ga_ਅੰਕੜੇ;
    ਮੇਲ_ਸਿਰਾ->lnum = ਸਿਰਾ->lnum;
    ਮੇਲ_ਸਿਰਾ->ਖਾਨਾ = segment_len;

    ਵਾਪਸ OK;

fail:
    ga_clear(&ga);
    ਵਾਪਸ FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    ਸਥਿਰ ਪੂਰਨ
is_regex_ਮੇਲ(char_u *pat, char_u *str)
{
    ਜੇ (STRCMP(pat, str) == 0)
	ਵਾਪਸ TRUE;

    regmatch_T	regmatch;
    ਪੂਰਨ		ਨਤੀਜਾ;

    ++emsg_off;
    ++ਸੁਨੇਹਾ_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --ਸੁਨੇਹਾ_silent;

    ਜੇ (regmatch.regprog == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    regmatch.rm_ic = p_ic;
    ਜੇ (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++ਸੁਨੇਹਾ_silent;
    ਨਤੀਜਾ = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --ਸੁਨੇਹਾ_silent;

    vim_regfree(regmatch.regprog);
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    ਸਥਿਰ char_u *
concat_ਨਮੂਨਾ_with_ਬਫਰ_ਮੇਲ(
	char_u *pat,
	ਪੂਰਨ pat_len,
	pos_T *ਸਿਰਾ_ਮੇਲ_pos,
	ਪੂਰਨ lowercase ਅਣਵਰਤਿਆ)
{
    char_u  *ਸਤਰ = ml_get(ਸਿਰਾ_ਮੇਲ_pos->lnum);
    char_u  *ਸ਼ਬਦ_ਸਿਰਾ = ਲੱਭੋ_ਸ਼ਬਦ_ਸਿਰਾ(ਸਤਰ + ਸਿਰਾ_ਮੇਲ_pos->ਖਾਨਾ);
    ਪੂਰਨ	    ਮੇਲ_len = (ਪੂਰਨ)(ਸ਼ਬਦ_ਸਿਰਾ - (ਸਤਰ + ਸਿਰਾ_ਮੇਲ_pos->ਖਾਨਾ));
    char_u  *ਮੇਲ = alloc(ਮੇਲ_len + pat_len + 1);  // +1 for NUL

    ਜੇ (ਮੇਲ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    mch_memmove(ਮੇਲ, pat, pat_len);
    ਜੇ (ਮੇਲ_len > 0)
    {
#ਜੇ defined(FEAT_EVAL) || defined(FEAT_SPELL)
	ਜੇ (lowercase)
	{
	    char_u  *mword = vim_strnsave(ਸਤਰ + ਸਿਰਾ_ਮੇਲ_pos->ਖਾਨਾ,
		    ਮੇਲ_len);
	    ਜੇ (mword == ਨਿਰਾ)
		ਜਾਓ cleanup;
	    char_u  *ਹੇਠਲਾ = strlow_save(mword);
	    vim_free(mword);
	    ਜੇ (ਹੇਠਲਾ == ਨਿਰਾ)
		ਜਾਓ cleanup;
	    mch_memmove(ਮੇਲ + pat_len, ਹੇਠਲਾ, ਮੇਲ_len);
	    vim_free(ਹੇਠਲਾ);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    mch_memmove(ਮੇਲ + pat_len, ਸਤਰ + ਸਿਰਾ_ਮੇਲ_pos->ਖਾਨਾ, ਮੇਲ_len);
    }
    ਮੇਲ[pat_len + ਮੇਲ_len] = NUL;
    ਵਾਪਸ ਮੇਲ;

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(ਮੇਲ);
    ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
expand_ਨਮੂਨਾ_ਵਿੱਚ_buf(
    char_u	*pat,		    // pattern to match
    ਪੂਰਨ		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    ਪੂਰਨ		*ਅੰਕ_Matches)	    // return: number of matches
{
    pos_T	cur_ਮੇਲ_pos, prev_ਮੇਲ_pos, ਸਿਰਾ_ਮੇਲ_pos, ਸ਼ਬਦ_ਸਿਰਾ_pos;
    garray_T	ga;
    ਪੂਰਨ		ਮਿਲਿਆ_new_ਮੇਲ;
    ਪੂਰਨ		looped_around = FALSE;
    ਪੂਰਨ		pat_len;
    ਪੂਰਨ		has_range = FALSE;
    ਪੂਰਨ		compl_started = FALSE;
    ਪੂਰਨ		ਖੋਜ_flags;
    char_u	*ਮੇਲ, *ਭਰਿਆ_ਮੇਲ;
    ਪੂਰਨ		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    has_range = ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ != 0;
#ਜੇ_ਅੰਤ

    *matches = ਨਿਰਾ;
    *ਅੰਕ_Matches = 0;

    ਜੇ (pat == ਨਿਰਾ || *pat == NUL)
	ਵਾਪਸ FAIL;

    pat_len = (ਪੂਰਨ)STRLEN(pat);
    CLEAR_ਖੇਤਰ(cur_ਮੇਲ_pos);
    CLEAR_ਖੇਤਰ(prev_ਮੇਲ_pos);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    ਜੇ (has_range)
	cur_ਮੇਲ_pos.lnum = ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	cur_ਮੇਲ_pos = pre_incsearch_pos;

    ਖੋਜ_flags = ਖੋਜ_OPT | ਖੋਜ_NOOF | ਖੋਜ_PEEK | ਖੋਜ_NFMSG
	| (has_range ? ਖੋਜ_ਸ਼ੁਰੂ : 0);

    ga_init2(&ga, ਆਕਾਰ(char_u *), 10); // Use growable array of char_u*

    ਲਈ (;;)
    {
	++emsg_off;
	++ਸੁਨੇਹਾ_silent;
	ਮਿਲਿਆ_new_ਮੇਲ = searchit(ਨਿਰਾ, curbuf, &cur_ਮੇਲ_pos,
		&ਸਿਰਾ_ਮੇਲ_pos, dir, pat, pat_len, 1L,
		ਖੋਜ_flags, RE_ਆਖਰੀ, ਨਿਰਾ);
	--ਸੁਨੇਹਾ_silent;
	--emsg_off;

	ਜੇ (ਮਿਲਿਆ_new_ਮੇਲ == FAIL)
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	// If in range mode, check if match is within the range
	ਜੇ (has_range && (cur_ਮੇਲ_pos.lnum < ਖੋਜ_ਪਹਿਲਾ_ਸਤਰ
		    || cur_ਮੇਲ_pos.lnum > ਖੋਜ_ਆਖਰੀ_ਸਤਰ))
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਜੇ (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    ਜੇ ((dir == FORWARD && LTOREQ_POS(cur_ਮੇਲ_pos, prev_ਮੇਲ_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_ਮੇਲ_pos, cur_ਮੇਲ_pos)))
	    {
		ਜੇ (looped_around)
		    ਰੋਕੋ;
		ਨਹੀਂਤਾਂ
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_ਮੇਲ_pos = cur_ਮੇਲ_pos;

	// Abort if user typed a character or interrupted
	ਜੇ (char_avail() || got_int)
	{
	    ਜੇ (got_int)
	    {
		(ਖਾਲੀ)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    ਜਾਓ cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	ਜੇ (ਸਿਰਾ_ਮੇਲ_pos.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    cur_ਮੇਲ_pos.lnum = 1;
	    cur_ਮੇਲ_pos.ਖਾਨਾ = 0;
	    cur_ਮੇਲ_pos.coladd = 0;
	    ਜਾਰੀ;
	}

	// Extract the matching text prepended to completed word
	ਜੇ (copy_substring_from_pos(&cur_ਮੇਲ_pos, &ਸਿਰਾ_ਮੇਲ_pos, &ਭਰਿਆ_ਮੇਲ,
		    &ਸ਼ਬਦ_ਸਿਰਾ_pos) == FAIL)
	    ਰੋਕੋ;

	ਜੇ (exacttext)
	    ਮੇਲ = ਭਰਿਆ_ਮੇਲ;
	ਨਹੀਂਤਾਂ
	{
	    // Construct a new match from completed word appended to pattern itself
	    ਮੇਲ = concat_ਨਮੂਨਾ_with_ਬਫਰ_ਮੇਲ(pat, pat_len, &ਸਿਰਾ_ਮੇਲ_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    ਜੇ (ਮੇਲ == ਨਿਰਾ || !is_regex_ਮੇਲ(ਮੇਲ, ਭਰਿਆ_ਮੇਲ))
	    {
		vim_free(ਮੇਲ);
		ਮੇਲ = concat_ਨਮੂਨਾ_with_ਬਫਰ_ਮੇਲ(pat, pat_len,
			&ਸਿਰਾ_ਮੇਲ_pos, TRUE);
		ਜੇ (ਮੇਲ == ਨਿਰਾ || !is_regex_ਮੇਲ(ਮੇਲ, ਭਰਿਆ_ਮੇਲ))
		{
		    vim_free(ਮੇਲ);
		    vim_free(ਭਰਿਆ_ਮੇਲ);
		    ਜਾਰੀ;
		}
	    }
	    vim_free(ਭਰਿਆ_ਮੇਲ);
	}

	// Include this match if it is not a duplicate
	ਲਈ (ਪੂਰਨ i = 0; i < ga.ga_len; ++i)
	{
	    ਜੇ (STRCMP(ਮੇਲ, ((char_u **)ga.ga_ਅੰਕੜੇ)[i]) == 0)
	    {
		VIM_CLEAR(ਮੇਲ);
		ਰੋਕੋ;
	    }
	}
	ਜੇ (ਮੇਲ != ਨਿਰਾ)
	{
	    ਜੇ (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(ਮੇਲ);
		ਜਾਓ cleanup;
	    }
	    ((char_u **)ga.ga_ਅੰਕੜੇ)[ga.ga_len++] = ਮੇਲ;
	    ਜੇ (ga.ga_len > ਟੈਗ_MANY)
		ਰੋਕੋ;
	}
	ਜੇ (has_range)
	    cur_ਮੇਲ_pos = ਸ਼ਬਦ_ਸਿਰਾ_pos;
    }

    *matches = (char_u **)ga.ga_ਅੰਕੜੇ;
    *ਅੰਕ_Matches = ga.ga_len;
    ਵਾਪਸ OK;

cleanup:
    ga_clear_ਲੜੀਆਂ(&ga);
    ਵਾਪਸ FAIL;
}
