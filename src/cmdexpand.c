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

#شامل "vim.h"

ایستا صحیح	cmd_showtail;	// Only show path tail in lists ?
ایستا صحیح	may_expand_pattern = FALSE;
ایستا pos_T	pre_incsearch_pos; // Cursor position when incsearch started

ایستا تهی	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, صحیح usefilter, expand_T *xp, صحیح *complp);
ایستا صحیح	ExpandFromContext(expand_T *xp, char_u *, char_u ***, صحیح *, صحیح);
ایستا char_u	*showmatches_gettail(char_u *s);
ایستا صحیح	expand_showtail(expand_T *xp);
ایستا صحیح	expand_shellcmd(char_u *filepat, char_u ***matches, صحیح *رقم_Matches, صحیح flagsarg);
#اگر defined(FEAT_EVAL)
ایستا صحیح	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, صحیح *رقم_Matches);
ایستا صحیح	ExpandUserList(expand_T *xp, char_u ***matches, صحیح *رقم_Matches);
ایستا char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
ایستا char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, صحیح srclen, صحیح *new_lenp);
#endif
ایستا صحیح	expand_pattern_in_buf(char_u *pat, صحیح dir, char_u ***matches, صحیح *رقم_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ایستا pumitem_T *compl_match_array = پوچ;
ایستا صحیح compl_match_arraysize;
// First column in cmdline of the matched item for completion.
ایستا صحیح compl_startcol;
ایستا صحیح compl_selected;
// cmdline before expansion
ایستا string_T cmdline_orig = {پوچ, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    ایستا صحیح
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    انتخاب (xp->xp_context)
    {
    حالت EXPAND_BOOL_SETTINGS:
    حالت EXPAND_COLORS:
    حالت EXPAND_COMPILER:
    حالت EXPAND_DIRECTORIES:
    حالت EXPAND_DIRS_IN_CDPATH:
    حالت EXPAND_FILES:
    حالت EXPAND_FILES_IN_PATH:
    حالت EXPAND_FILETYPE:
    حالت EXPAND_FILETYPECMD:
    حالت EXPAND_HELP:
    حالت EXPAND_KEYMAP:
    حالت EXPAND_OLD_SETTING:
    حالت EXPAND_STRING_SETTING:
    حالت EXPAND_SETTING_SUBTRACT:
    حالت EXPAND_OWNSYNTAX:
    حالت EXPAND_PACKADD:
    حالت EXPAND_RUNTIME:
    حالت EXPAND_SHELLCMD:
    حالت EXPAND_SHELLCMDLINE:
    حالت EXPAND_TAGS:
    حالت EXPAND_TAGS_LISTFILES:
	بازگشت FALSE;

    پیشفرض:
	بشکن;
    }

    بازگشت vim_strchr(p_wop, WOP_FUZZY) != پوچ;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    صحیح
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    بازگشت vim_strchr(p_wop, WOP_FUZZY) != پوچ && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    ایستا صحیح
sort_func_compare(ثابت تهی *s1, ثابت تهی *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    اگر (*p1 != '<' && *p2 == '<') بازگشت -1;
    اگر (*p1 == '<' && *p2 != '<') بازگشت 1;
    بازگشت STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    ایستا تهی
wildescape(
    expand_T	*xp,
    char_u	*str,
    صحیح		numfiles,
    char_u	**files)
{
    char_u	*p;
    صحیح		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    اگر (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	برای (صحیح i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    اگر (xp->xp_backslash & XP_BS_THREE)
	    {
		نویسه *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		اگر (p != پوچ)
		{
		    vim_free(files[i]);
		    files[i] = p;
#اگر defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    اگر (p != پوچ)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    وگرنه اگر (xp->xp_backslash & XP_BS_COMMA)
	    {
		اگر (vim_strchr(files[i], ',') != پوچ)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    اگر (p != پوچ)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ifdef BACKSLASH_IN_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#وگرنه
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#endif
	    اگر (p != پوچ)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    اگر (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	اگر (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    وگرنه اگر (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	برای (صحیح i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    اگر (p != پوچ)
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
    ایستا تهی
ExpandEscape(
    expand_T	*xp,
    char_u	*str,
    صحیح		numfiles,
    char_u	**files,
    صحیح		options)
{
    // May change home directory back to "~"
    اگر (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    اگر (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    صحیح
nextwild(
    expand_T	*xp,
    صحیح		type,
    صحیح		options,	// extra options for ExpandOne()
    صحیح		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    صحیح		i;
    char_u	*p;
    صحیح		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    صحیح		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    اگر (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	اگر (ccline->درون_داد_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	وگرنه
#endif
	{
	    may_expand_pattern = options & WILD_MAY_EXPAND_PATTERN;
	    set_expand_context(xp);
	    may_expand_pattern = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    اگر (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	بازگشت OK;  // Something illegal on command line
    }
    اگر (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	بازگشت FAIL;
    }

    i = (صحیح)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    اگر (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	بازگشت FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    اگر (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	پیام_puts("...");	    // show that we are busy
	out_flush();
    }

    اگر (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, پوچ, پوچ, 0, type);
    }
    وگرنه
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	صحیح	pat_len = xp->xp_pattern_len;
#اگر defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	اگر (unesc != پوچ)
	    pat_src = unesc;
#endif

	اگر (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	وگرنه
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#اگر defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	اگر (tmp == پوچ)
	    p = پوچ;
	وگرنه
	{
	    صحیح use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    اگر (escape)
		use_options |= WILD_ESCAPE;
	    اگر (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    اگر (p != پوچ && type == WILD_LONGEST)
	    {
		صحیح	j;

		برای (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    اگر (c == '*' || c == '?')
			بشکن;
		}
		اگر ((صحیح)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    اگر (!wild_navigate && ccline->cmdbuff != پوچ)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	اگر (cmdline_orig.string == پوچ)
	    cmdline_orig.length = 0;
	وگرنه
	    cmdline_orig.length = ccline->cmdlen;
    }

    اگر (p != پوچ && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	صحیح	difflen;
	صحیح	v = OK;

	difflen = (صحیح)plen - xp->xp_pattern_len;
	اگر (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	اگر (v == OK)
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
    اگر (xp->xp_context == EXPAND_MAPPINGS && p == پوچ)
	بازگشت FAIL;

    اگر (xp->xp_numfiles <= 0 && p == پوچ)
	beep_flush();
    وگرنه اگر (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(تهی)ExpandOne(xp, پوچ, پوچ, 0, WILD_FREE);

    vim_free(p);

    بازگشت OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    ایستا صحیح
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	صحیح		رقم_Matches,
	صحیح		showtail)
{
    صحیح	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, رقم_Matches);
    اگر (compl_match_array == پوچ)
	بازگشت EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = رقم_Matches;
    برای (صحیح i = 0; i < رقم_Matches; i++)
    {
	compl_match_array[i].pum_text = (xp->xp_files_abbr != پوچ
				    && xp->xp_files_abbr[i] != پوچ)
					? xp->xp_files_abbr[i]
					: SHOW_MATCH(i);
	compl_match_array[i].pum_info = xp->xp_files_info != پوچ
					    ? xp->xp_files_info[i] : پوچ;
	compl_match_array[i].pum_extra = xp->xp_files_menu != پوچ
					    ? xp->xp_files_menu[i] : پوچ;
	compl_match_array[i].pum_kind = xp->xp_files_kind != پوچ
					    ? xp->xp_files_kind[i] : پوچ;
	compl_match_array[i].pum_user_abbr_hlattr = -1;
	compl_match_array[i].pum_user_kind_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == پوچ ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    prefix_len = vim_strsize(xp->xp_pattern);
    اگر (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    بازگشت EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    تهی
cmdline_pum_display(تهی)
{
    اگر (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    صحیح
cmdline_pum_active(تهی)
{
    بازگشت pum_visible() && compl_match_array != پوچ;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    تهی
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, صحیح defer_redraw)
{
    صحیح	save_کلید_Typed = کلید_Typed;
#ifdef FEAT_EVAL
    صحیح	save_RedrawingDisabled = RedrawingDisabled;
    اگر (cclp->درون_داد_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    اگر (!defer_redraw)
    {
	صحیح save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    وگرنه
	pum_call_update_screen();
    redrawcmd();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    کلید_Typed = save_کلید_Typed;
#ifdef FEAT_EVAL
    اگر (cclp->درون_داد_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

    تهی
cmdline_pum_cleanup(cmdline_info_T *cclp)
{
    cmdline_pum_remove(cclp, FALSE);
    wildmenu_cleanup(cclp);
}

/*
 * Returns the starting column number to use for the cmdline completion popup
 * menu.
 */
    صحیح
cmdline_compl_startcol(تهی)
{
    بازگشت compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(تهی)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    بازگشت xp == پوچ ? پوچ : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    صحیح
cmdline_compl_is_fuzzy(تهی)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    بازگشت xp != پوچ && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    ایستا صحیح
skip_status_match_char(expand_T *xp, char_u *s)
{
    اگر ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	اگر (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    بازگشت 2;
#endif
	بازگشت 1;
    }
    بازگشت 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    ایستا صحیح
status_match_len(expand_T *xp, char_u *s)
{
    صحیح	len = 0;

#ifdef FEAT_MENU
    صحیح emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    اگر (emenu && menu_is_separator(s))
	بازگشت 1;
#endif

    وقتی (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    بازگشت len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    ایستا تهی
win_redr_status_matches(
    expand_T	*xp,
    صحیح		رقم_matches,
    char_u	**matches,	// list of matches
    صحیح		match,
    صحیح		showtail)
{
    صحیح		row;
    char_u	*buf;
    صحیح		len;
    صحیح		clen;		// length in screen cells
    صحیح		fillchar;
    صحیح		attr;
    صحیح		i;
    صحیح		highlight = TRUE;
    char_u	*selstart = پوچ;
    صحیح		selstart_col = 0;
    char_u	*selend = پوچ;
    ایستا صحیح	first_match = 0;
    صحیح		add_چپ = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    صحیح		emenu;
#endif
    صحیح		l;

    اگر (matches == پوچ)	// interrupted completion?
	بازگشت;

    اگر (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    وگرنه
	buf = alloc(topframe->fr_width + 1);
    اگر (buf == پوچ)
	بازگشت;

    اگر (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    اگر (match == 0)
	first_match = 0;
    وگرنه اگر (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_چپ = TRUE;
    }
    وگرنه
    {
	// check if match fits on the screen
	برای (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	اگر (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	اگر (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    برای (i = match; i < رقم_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		اگر (clen >= topframe->fr_width)
		    بشکن;
	    }
	    اگر (i == رقم_matches)
		add_چپ = TRUE;
	}
    }
    اگر (add_چپ)
	وقتی (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    اگر (clen >= topframe->fr_width)
		بشکن;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    اگر (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    وگرنه
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    وقتی (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	اگر (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	اگر (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (صحیح)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	وگرنه
#endif
	برای ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    اگر (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    وگرنه
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (صحیح)STRLEN(buf + len);
	    }
	}
	اگر (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	اگر (++i == رقم_matches)
		بشکن;
    }

    اگر (i != رقم_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    اگر (row >= 0)
    {
	اگر (wild_menu_showing == 0)
	{
	    اگر (پیام_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		اگر (cmdline_row == Rows - 1)
		{
		    screen_del_سطرها(0, 0, 1, (صحیح)Rows, TRUE, 0, پوچ);
		    ++پیام_scrolled;
		}
		وگرنه
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    وگرنه
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		اگر (lastwin->w_status_height == 0)
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
	اگر (selstart != پوچ && highlight)
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

#اگر defined(FEAT_EVAL)
/*
 * Apply -completeopt=escape to a string about to be inserted into the command
 * line as a completion result.  If "str" is non-NULL and the active expansion
 * context is a customlist/custom user command with UCC_ESCAPE set, free "str"
 * and return a newly-allocated copy with spaces, tabs and backslashes prefixed
 * by a backslash.  Otherwise return "str" unchanged.
 */
    ایستا char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    اگر (str == پوچ)
	بازگشت پوچ;
    اگر ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	بازگشت str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    اگر (p == پوچ)
	بازگشت str;
    vim_free(str);
    بازگشت p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    ایستا char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    صحیح		srclen,
    صحیح		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    اگر ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	بازگشت پوچ;

    buf = alloc(srclen + 1);
    اگر (buf == پوچ)
	بازگشت پوچ;

    d = buf;
    end = src + srclen;
    برای (p = src; p < end; ++p)
    {
	اگر (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (صحیح)(d - buf);
    بازگشت buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    ایستا char_u *
get_next_or_prev_match(صحیح mode, expand_T *xp)
{
    صحیح	    findex = xp->xp_selected;
    صحیح	    ht;

    // When no matches found, return NULL
    اگر (xp->xp_numfiles <= 0)
	بازگشت پوچ;

    اگر (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	اگر (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    وگرنه اگر (mode == WILD_NEXT)
    {
	// Select the next entry
	++findex;
    }
    وگرنه   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	اگر (ht > 3)
	    ht -= 2;

	اگر (mode == WILD_PAGEUP)
	{
	    اگر (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    وگرنه اگر (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    وگرنه
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	وگرنه    // mode == WILD_PAGEDOWN
	{
	    اگر (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    وگرنه اگر (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    وگرنه
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    اگر (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	اگر (xp->xp_orig != پوچ)
	    findex = -1;
	وگرنه
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    اگر (p_wmnu)
    {
	اگر (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	وگرنه اگر (vim_strchr(p_wop, WOP_PUM) != پوچ)
	{
	    اگر (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	وگرنه
	    win_redr_status_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    اگر (findex == -1)
	بازگشت vim_strsave(xp->xp_orig);
#اگر defined(FEAT_EVAL)
    بازگشت apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#وگرنه
    بازگشت vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    ایستا char_u *
ExpandOne_start(صحیح mode, expand_T *xp, char_u *str, صحیح options)
{
    صحیح		non_suf_match;		// number without matching suffix
    صحیح		i;
    char_u	*ss = پوچ;

    // Do the expansion.
    اگر (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	اگر (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    وگرنه اگر (xp->xp_numfiles == 0)
    {
	اگر (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    وگرنه
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	اگر (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    اگر (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    وگرنه
		non_suf_match = 1;
	    اگر ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		برای (i = 0; i < 2; ++i)
		    اگر (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    اگر (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		اگر (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		وگرنه اگر (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    اگر (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    بازگشت ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    ایستا char_u *
find_longest_match(expand_T *xp, صحیح options)
{
    long_u	len;
    صحیح		mb_len = 1;
    صحیح		c0, ci;
    صحیح		i;
    char_u	*ss;

    برای (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	اگر (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	وگرنه
	    c0 = xp->xp_files[0][len];
	برای (i = 1; i < xp->xp_numfiles; ++i)
	{
	    اگر (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    وگرنه
		ci = xp->xp_files[i][len];
	    اگر (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		اگر (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    بشکن;
	    }
	    وگرنه اگر (c0 != ci)
		بشکن;
	}
	اگر (i < xp->xp_numfiles)
	{
	    اگر (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    بشکن;
	}
    }

    ss = alloc(len + 1);
    اگر (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    بازگشت ss;
}

    تهی
free_xp_files_extra(expand_T *xp, صحیح numfiles)
{
    اگر (xp->xp_files_abbr != پوچ)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = پوچ;
    }
    اگر (xp->xp_files_kind != پوچ)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = پوچ;
    }
    اگر (xp->xp_files_menu != پوچ)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = پوچ;
    }
    اگر (xp->xp_files_info != پوچ)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = پوچ;
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
    صحیح		options,
    صحیح		mode)
{
    char_u	*ss = پوچ;
    صحیح		orig_saved = FALSE;
#اگر defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    صحیح		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    اگر (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	بازگشت get_next_or_prev_match(mode, xp);

    اگر (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    وگرنه اگر (mode == WILD_APPLY)
    {
	اگر (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	وگرنه
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#اگر defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    اگر (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	اگر (compl_match_array != پوچ)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    اگر (mode == WILD_FREE)	// only release file name
	بازگشت پوچ;

    اگر (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#اگر defined(FEAT_EVAL)
	اگر (ss != پوچ)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    اگر (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#اگر defined(FEAT_EVAL)
	اگر (ss != پوچ)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    اگر (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	نویسه	*prefix = "";
	نویسه	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	صحیح	n = xp->xp_numfiles - 1;
	صحیح	i;
#اگر defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = پوچ;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	اگر ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    اگر (escaped != پوچ)
	    {
		برای (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    اگر (escaped[i] == پوچ)
		    {
			وقتی (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			بشکن;
		    }
		}
		اگر (escaped != پوچ)
		    files = escaped;
	    }
	}
#وگرنه
	char_u	**files = xp->xp_files;
#endif

	اگر (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	وگرنه اگر (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	برای (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	اگر (ss != پوچ)
	{
	    size_t  ss_len = 0;

	    برای (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (نویسه *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (نویسه *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#اگر defined(FEAT_EVAL)
	اگر (escaped != پوچ)
	{
	    برای (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    اگر (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    اگر (!orig_saved)
	vim_free(orig);

#اگر defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    اگر (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    بازگشت ss;
}

/*
 * Prepare an expand structure for use.
 */
    تهی
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
    تهی
ExpandCleanup(expand_T *xp)
{
    اگر (xp->xp_numfiles >= 0)
    {
	free_xp_files_extra(xp, xp->xp_numfiles);
	FreeWild(xp->xp_numfiles, xp->xp_files);
	xp->xp_numfiles = -1;
    }
    VIM_CLEAR(xp->xp_orig);
}

    تهی
clear_cmdline_orig(تهی)
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
    ایستا تهی
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	صحیح		رقم_Matches,
	صحیح		سطرها,
	صحیح		linenr,
	صحیح		maxlen,
	صحیح		showtail,
	صحیح		dir_attr)
{
    صحیح		i, j;
    صحیح		isdir;
    صحیح		lastlen;
    char_u	*p;

    lastlen = 999;
    برای (j = linenr; j < رقم_Matches; j += سطرها)
    {
	اگر (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    پیام_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    پیام_advance(maxlen + 1);
	    پیام_puts((نویسه *)p);
	    پیام_advance(maxlen + 3);
	    پیام_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    بشکن;
	}
	برای (i = maxlen - lastlen; --i >= 0; )
	    پیام_putchar(' ');
	اگر (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    اگر (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_path;
		char_u	*path;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_path = expand_env_save_opt(matches[j], TRUE, پوچ);
		path = exp_path != پوچ ? exp_path : matches[j];
		halved_slash = backslash_halve_save(path);
		isdir = mch_isdir(halved_slash != پوچ ? halved_slash
			: matches[j]);
		vim_free(exp_path);
		اگر (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    وگرنه
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    اگر (showtail)
		p = SHOW_MATCH(j);
	    وگرنه
	    {
		home_replace(پوچ, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	وگرنه
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = پیام_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    اگر (پیام_col > 0)	// when not wrapped around
    {
	پیام_clr_eos();
	پیام_putchar('\n');
    }
    out_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    ایستا صحیح
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	صحیح		رقم_Matches,
	صحیح		showtail,
	صحیح		noselect)
{
    صحیح	retval = cmdline_pum_create(ccline, xp, matches, رقم_Matches, showtail);

    اگر (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    بازگشت retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    صحیح
showmatches(
    expand_T	*xp,
    صحیح		display_wildmenu,
    صحیح		display_list,
    صحیح		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    صحیح		رقم_Matches;
    char_u	**matches;
    صحیح		i;
    صحیح		maxlen;
    صحیح		سطرها;
    صحیح		columns;
    صحیح		attr;
    صحیح		showtail;
    صحیح		noselect = (wim_flags_arg & WIM_NOSELECT);
    صحیح		noinsert = (wim_flags_arg & WIM_NOINSERT);
    صحیح		cmdline_unchanged = noselect || noinsert;

    اگر (xp->xp_numfiles == -1)
    {
	صحیح retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&رقم_Matches, &matches);
	اگر (retval != EXPAND_OK)
	    بازگشت retval;
	showtail = expand_showtail(xp);
    }
    وگرنه
    {
	رقم_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    اگر (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != پوچ)
	بازگشت show_pum_matches(ccline, xp, matches, رقم_Matches,
		showtail && !cmdline_unchanged, noselect);

    اگر (display_list)
    {
	پیام_didany = FALSE;		// lines_left will be set
	پیام_start();			// prepare for paging
	پیام_putchar('\n');
	out_flush();
	cmdline_row = پیام_row;
	پیام_didany = FALSE;		// lines_left will be set again
	پیام_start();			// prepare for paging
    }

    اگر (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    وگرنه اگر (display_wildmenu && !display_list)
	win_redr_status_matches(xp, رقم_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    وگرنه اگر (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	برای (i = 0; i < رقم_Matches; ++i)
	{
	    صحیح	len;
	    اگر (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(پوچ, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    وگرنه
		len = vim_strsize(SHOW_MATCH(i));
	    اگر (len > maxlen)
		maxlen = len;
	}

	اگر (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    سطرها = رقم_Matches;
	وگرنه
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    اگر (columns < 1)
		columns = 1;
	    سطرها = (رقم_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	اگر (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    پیام_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    پیام_clr_eos();
	    پیام_advance(maxlen - 3);
	    پیام_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	برای (i = 0; i < سطرها; ++i)
	{
	    showmatches_oneline(xp, matches, رقم_Matches, سطرها, i,
						maxlen, showtail, attr);
	    اگر (got_int)
	    {
		got_int = FALSE;
		بشکن;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = پیام_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    اگر (display_wildmenu && display_list)
    {
	اگر (vim_strchr(p_wop, WOP_PUM) != پوچ)
	    (تهی)show_pum_matches(ccline, xp, matches, رقم_Matches,
		    showtail && !cmdline_unchanged, noselect);
	وگرنه
	    win_redr_status_matches(xp, رقم_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    اگر (xp->xp_numfiles == -1)
    {
	FreeWild(رقم_Matches, matches);
	free_xp_files_extra(xp, رقم_Matches);
    }

    بازگشت EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    ایستا char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    صحیح		had_sep = FALSE;

    برای (p = s; *p != NUL; )
    {
	اگر (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	وگرنه اگر (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    بازگشت t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    ایستا صحیح
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    اگر (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	بازگشت FALSE;

    end = gettail(xp->xp_pattern);
    اگر (end == xp->xp_pattern)		// there is no path separator
	بازگشت FALSE;

    برای (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	اگر (rem_backslash(s))
	    ++s;
	وگرنه اگر (vim_strchr((char_u *)"*?[", *s) != پوچ)
	    بازگشت FALSE;
    }
    بازگشت TRUE;
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
    صحیح		len,
    صحیح		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    صحیح		i, j;
    صحیح		new_len;
    char_u	*tail;
    صحیح		ends_in_star;

    اگر (context != EXPAND_FILES
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
	اگر (context == EXPAND_FINDFUNC
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
	وگرنه
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    برای (i = 0; i < len; i++)
	    {
		اگر (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		اگر (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		اگر ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    اگر (retval != پوچ)
	    {
		retval[0] = '^';
		j = 1;
		برای (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    اگر (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			بشکن;

		    انتخاب (fname[i])
		    {
			حالت '*':   retval[j++] = '.';
				    بشکن;
			حالت '~':   retval[j++] = '\\';
				    بشکن;
			حالت '?':   retval[j] = '.';
				    ادامه;
			حالت '.':   اگر (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    بشکن;
			حالت '\\':  اگر (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    بشکن;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    وگرنه
    {
	retval = alloc(len + 4);
	اگر (retval != پوچ)
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
	    برای (i = len - 2; i >= 0; --i)
	    {
		اگر (retval[i] != '\\')
		    بشکن;
		ends_in_star = !ends_in_star;
	    }
#endif
	    اگر ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == پوچ
		    && vim_strchr(retval, '`') == پوچ)
		retval[len++] = '*';
	    وگرنه اگر (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    بازگشت retval;
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
    تهی
set_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    اگر ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_جستجو_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_جستجو_EXTRA
	جستجو_first_سطر = 0; // Search entire buffer
#endif
	بازگشت;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    اگر (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->درون_داد_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	بازگشت;
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
    ایستا char_u *
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, صحیح *complp)
{
    char_u	*p = پوچ;
    صحیح		len = 0;
    صحیح		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    اگر (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    وگرنه
    {
	p = cmd;
	وقتی (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	اگر (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    وقتی (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	اگر (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    وقتی (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	اگر (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != پوچ)
	    ++p;
	len = (صحیح)(p - cmd);

	اگر (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    بازگشت پوچ;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	اگر ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    وقتی (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    اگر (*p == NUL && ASCII_ISALNUM(p[-1]))
	بازگشت پوچ;

    اگر (eap->cmdidx == CMD_SIZE)
    {
	اگر (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != پوچ)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	وگرنه اگر (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, پوچ, xp, complp);
	    اگر (p == پوچ)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    اگر (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	بازگشت پوچ;
    }

    بازگشت p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    ایستا تهی
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	صحیح		usefilter,
	expand_T	*xp,
	صحیح		*complp)
{
    char_u	*p;
    صحیح		c;
    صحیح		in_quote = FALSE;
    char_u	*bow = پوچ;	// Beginning of word
    صحیح		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    وقتی (*p != NUL)
    {
	اگر (has_mbyte)
	    c = mb_ptr2char(p);
	وگرنه
	    c = *p;
	اگر (c == '\\' && p[1] != NUL)
	    ++p;
	وگرنه اگر (c == '`')
	{
	    اگر (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	وگرنه اگر (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != پوچ && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    وقتی (*p != NUL)
	    {
		اگر (has_mbyte)
		    c = mb_ptr2char(p);
		وگرنه
		    c = *p;
		اگر (c == '`' || vim_isfilec_or_wc(c))
		    بشکن;
		اگر (has_mbyte)
		    len = (*mb_ptr2len)(p);
		وگرنه
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    اگر (in_quote)
		bow = p;
	    وگرنه
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    اگر (bow != پوچ && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    اگر (usefilter
	    || (eap != پوچ
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	اگر (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    اگر (*xp->xp_pattern == '$')
    {
	برای (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    اگر (!vim_isIDc(*p))
		بشکن;
	اگر (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    اگر (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    اگر (*xp->xp_pattern == '~')
    {
	برای (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	اگر (*p == NUL && p > xp->xp_pattern + 1
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
    ایستا char_u *
set_context_in_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    اگر (p == پوچ)
	xp->xp_pattern = arg;
    وگرنه
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    بازگشت پوچ;
}

#ifdef FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    ایستا char_u *
set_context_in_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    اگر (p == پوچ)
	xp->xp_pattern = arg;
    وگرنه
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    بازگشت پوچ;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    ایستا char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    اگر (*arg != NUL)
	arg = skip_vimgrep_pat(arg, پوچ, پوچ);
    اگر (arg == پوچ || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	بازگشت پوچ;
    }
    بازگشت skipwhite(arg);
}

#ifdef FEAT_جستجو_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    ایستا char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    اگر (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	اگر (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    بازگشت find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    ایستا char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    صحیح		delim;

    delim = *arg;	    // get the delimiter
    اگر (delim)
	++arg;		    // skip delimiter if there is one

    وقتی (arg[0] != NUL && arg[0] != delim)
    {
	اگر (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    اگر (arg[0] != NUL)
	بازگشت arg + 1;

    بازگشت پوچ;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    ایستا char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    صحیح		delim;

    delim = *arg;
    اگر (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	اگر (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    وقتی (arg[0] != NUL && arg[0] != delim)
	    {
		اگر (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    اگر (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    وقتی (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == پوچ)
	++arg;
    اگر (arg[0] != NUL)
	بازگشت arg;

    بازگشت پوچ;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    ایستا char_u *
find_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    اگر (*arg != '/')
	بازگشت پوچ;

    // Match regexp, not just whole words
    برای (++arg; *arg && *arg != '/'; arg++)
	اگر (*arg == '\\' && arg[1] != NUL)
	    arg++;
    اگر (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	اگر (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == پوچ)
	    xp->xp_context = EXPAND_NOTHING;
	وگرنه
	    بازگشت arg;
    }

    بازگشت پوچ;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    ایستا char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    وقتی ((xp->xp_pattern = vim_strchr(arg, ' ')) != پوچ)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    اگر (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    بازگشت پوچ;
}
#endif

#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    ایستا char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    اگر (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    وگرنه
    {
	اگر ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_pattern = skipwhite(p);
	}
	وگرنه
	    xp->xp_context = EXPAND_NOTHING;
    }

    بازگشت پوچ;
}
#endif

ایستا شمارش
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
ایستا شمارش
{
    EXP_BREAKPT_ADD,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    ایستا char_u *
set_context_in_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_start;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_pattern = arg;

    اگر (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    وگرنه اگر (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    وگرنه
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    اگر (*p == NUL)
	بازگشت پوچ;
    subcmd_start = p;

    اگر (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	اگر (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    اگر (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		بازگشت پوچ;
	    }
	    p = skipwhite(p);
	}
	اگر (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	وگرنه
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    وگرنه اگر (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    بازگشت پوچ;
}

    ایستا char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = پوچ;

    p = skipwhite(arg);
    اگر (VIM_ISDIGIT(*p))
	بازگشت پوچ;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    بازگشت پوچ;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    ایستا char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    صحیح    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    اگر (*p == NUL)
	بازگشت پوچ;

    برای (;;)
    {
	اگر (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    ادامه;
	}
	اگر (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    ادامه;
	}
	بشکن;
    }

    اگر ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    وگرنه اگر ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    وگرنه اگر ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    بازگشت پوچ;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    ایستا تهی
set_context_with_pattern(expand_T *xp)
{
    صحیح		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_جستجو_EXTRA
    صحیح		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    اگر (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	بازگشت;
#endif

    xp->xp_pattern = ccline->cmdbuff + skiplen;
    xp->xp_pattern_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_PATTERN_IN_BUF;
    xp->xp_جستجو_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    ایستا char_u *
set_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	long		argt,
	صحیح		compl,
	صحیح		forceit)
{
    char_u  *nextcmd;

    انتخاب (cmdidx)
    {
	حالت CMD_find:
	حالت CMD_sfind:
	حالت CMD_tabfind:
	    اگر (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    بشکن;
	حالت CMD_cd:
	حالت CMD_chdir:
	حالت CMD_tcd:
	حالت CMD_tchdir:
	حالت CMD_lcd:
	حالت CMD_lchdir:
	    اگر (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    بشکن;
	حالت CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    بشکن;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	حالت CMD_aboveleft:
	حالت CMD_argdo:
	حالت CMD_belowright:
	حالت CMD_botright:
	حالت CMD_browse:
	حالت CMD_bufdo:
	حالت CMD_cdo:
	حالت CMD_cfdo:
	حالت CMD_confirm:
	حالت CMD_debug:
	حالت CMD_folddoclosed:
	حالت CMD_folddoopen:
	حالت CMD_hide:
	حالت CMD_horizontal:
	حالت CMD_keepalt:
	حالت CMD_keepjumps:
	حالت CMD_keepmarks:
	حالت CMD_keeppatterns:
	حالت CMD_ldo:
	حالت CMD_leftabove:
	حالت CMD_lfdo:
	حالت CMD_lockmarks:
	حالت CMD_noautocmd:
	حالت CMD_noswapfile:
	حالت CMD_rightbelow:
	حالت CMD_sandbox:
	حالت CMD_silent:
	حالت CMD_tab:
	حالت CMD_tabdo:
	حالت CMD_topleft:
	حالت CMD_unsilent:
	حالت CMD_verbose:
	حالت CMD_vertical:
	حالت CMD_windo:
	حالت CMD_vim9cmd:
	حالت CMD_legacy:
	    بازگشت arg;

	حالت CMD_filter:
	    بازگشت set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_جستجو_EXTRA
	حالت CMD_match:
	    بازگشت set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	حالت CMD_command:
	    بازگشت set_context_in_user_cmd(xp, arg);

	حالت CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    بشکن;

	حالت CMD_global:
	حالت CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    اگر (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    بازگشت nextcmd;

	حالت CMD_and:
	حالت CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    اگر (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    بازگشت nextcmd;

	حالت CMD_isearch:
	حالت CMD_dsearch:
	حالت CMD_ilist:
	حالت CMD_dlist:
	حالت CMD_ijump:
	حالت CMD_psearch:
	حالت CMD_djump:
	حالت CMD_isplit:
	حالت CMD_dsplit:
	    بازگشت find_cmd_after_isearch_cmd(xp, arg);
	حالت CMD_autocmd:
	    بازگشت set_context_in_autocmd(xp, arg, FALSE);
	حالت CMD_doautocmd:
	حالت CMD_doautoall:
	    بازگشت set_context_in_autocmd(xp, arg, TRUE);
	حالت CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    بشکن;
	حالت CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    بشکن;
	حالت CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    بشکن;
	حالت CMD_tag:
	حالت CMD_stag:
	حالت CMD_ptag:
	حالت CMD_ltag:
	حالت CMD_tselect:
	حالت CMD_stselect:
	حالت CMD_ptselect:
	حالت CMD_tjump:
	حالت CMD_stjump:
	حالت CMD_ptjump:
	    اگر (vim_strchr(p_wop, WOP_TAGFILE) != پوچ)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    وگرنه
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    بشکن;
	حالت CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    بشکن;
#ifdef FEAT_SYN_HL
	حالت CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    بشکن;
#endif
#ifdef FEAT_EVAL
	حالت CMD_final:
	حالت CMD_const:
	حالت CMD_let:
	حالت CMD_var:
	حالت CMD_if:
	حالت CMD_elseif:
	حالت CMD_while:
	حالت CMD_for:
	حالت CMD_echo:
	حالت CMD_echon:
	حالت CMD_execute:
	حالت CMD_echomsg:
	حالت CMD_echoerr:
	حالت CMD_echoconsole:
	حالت CMD_echowindow:
	حالت CMD_call:
	حالت CMD_return:
	حالت CMD_cexpr:
	حالت CMD_caddexpr:
	حالت CMD_cgetexpr:
	حالت CMD_lexpr:
	حالت CMD_laddexpr:
	حالت CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    بشکن;

	حالت CMD_unlet:
	    بازگشت set_context_in_unlet_cmd(xp, arg);
	حالت CMD_function:
	حالت CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    بشکن;
	حالت CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    بشکن;

	حالت CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    بشکن;
#endif
	حالت CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    بشکن;
#ifdef FEAT_CSCOPE
	حالت CMD_cscope:
	حالت CMD_lcscope:
	حالت CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    بشکن;
#endif
#ifdef FEAT_SIGNS
	حالت CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    بشکن;
#endif
	حالت CMD_bdelete:
	حالت CMD_bwipeout:
	حالت CMD_bunload:
	    وقتی ((xp->xp_pattern = vim_strchr(arg, ' ')) != پوچ)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	حالت CMD_buffer:
	حالت CMD_sbuffer:
	حالت CMD_pbuffer:
	حالت CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    بشکن;
#ifdef FEAT_DIFF
	حالت CMD_diffget:
	حالت CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    بشکن;
#endif
	حالت CMD_USER:
	حالت CMD_USER_BUF:
	    بازگشت set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	حالت CMD_map:	    حالت CMD_noremap:
	حالت CMD_nmap:	    حالت CMD_nnoremap:
	حالت CMD_vmap:	    حالت CMD_vnoremap:
	حالت CMD_omap:	    حالت CMD_onoremap:
	حالت CMD_imap:	    حالت CMD_inoremap:
	حالت CMD_cmap:	    حالت CMD_cnoremap:
	حالت CMD_lmap:	    حالت CMD_lnoremap:
	حالت CMD_smap:	    حالت CMD_snoremap:
	حالت CMD_tmap:	    حالت CMD_tnoremap:
	حالت CMD_xmap:	    حالت CMD_xnoremap:
	    بازگشت set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	حالت CMD_unmap:
	حالت CMD_nunmap:
	حالت CMD_vunmap:
	حالت CMD_ounmap:
	حالت CMD_iunmap:
	حالت CMD_cunmap:
	حالت CMD_lunmap:
	حالت CMD_sunmap:
	حالت CMD_tunmap:
	حالت CMD_xunmap:
	    بازگشت set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	حالت CMD_mapclear:
	حالت CMD_nmapclear:
	حالت CMD_vmapclear:
	حالت CMD_omapclear:
	حالت CMD_imapclear:
	حالت CMD_cmapclear:
	حالت CMD_lmapclear:
	حالت CMD_smapclear:
	حالت CMD_tmapclear:
	حالت CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    بشکن;

	حالت CMD_abbreviate:	حالت CMD_noreabbrev:
	حالت CMD_cabbrev:	حالت CMD_cnoreabbrev:
	حالت CMD_iabbrev:	حالت CMD_inoreabbrev:
	    بازگشت set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	حالت CMD_unabbreviate:
	حالت CMD_cunabbrev:
	حالت CMD_iunabbrev:
	    بازگشت set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	حالت CMD_menu:	    حالت CMD_noremenu:	    حالت CMD_unmenu:
	حالت CMD_amenu:	    حالت CMD_anoremenu:	    حالت CMD_aunmenu:
	حالت CMD_nmenu:	    حالت CMD_nnoremenu:	    حالت CMD_nunmenu:
	حالت CMD_vmenu:	    حالت CMD_vnoremenu:	    حالت CMD_vunmenu:
	حالت CMD_omenu:	    حالت CMD_onoremenu:	    حالت CMD_ounmenu:
	حالت CMD_imenu:	    حالت CMD_inoremenu:	    حالت CMD_iunmenu:
	حالت CMD_cmenu:	    حالت CMD_cnoremenu:	    حالت CMD_cunmenu:
	حالت CMD_tlmenu:    حالت CMD_tlnoremenu:    حالت CMD_tlunmenu:
	حالت CMD_tmenu:				    حالت CMD_tunmenu:
	حالت CMD_popup:	    حالت CMD_tearoff:	    حالت CMD_emenu:
	    بازگشت set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	حالت CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    بشکن;

	حالت CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    بشکن;

	حالت CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    بشکن;

	حالت CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    بشکن;

	حالت CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    بشکن;

	حالت CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    بشکن;

#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	حالت CMD_language:
	    بازگشت set_context_in_lang_cmd(xp, arg);
#endif
#اگر defined(FEAT_PROFILE)
	حالت CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    بشکن;
#endif
	حالت CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    بشکن;

	حالت CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    بشکن;

	حالت CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    بشکن;

	حالت CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    بشکن;
#اگر defined(FEAT_PROFILE)
	حالت CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    بشکن;
#endif

	حالت CMD_argdelete:
	    وقتی ((xp->xp_pattern = vim_strchr(arg, ' ')) != پوچ)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    بشکن;

#ifdef FEAT_EVAL
	حالت CMD_breakadd:
	حالت CMD_profdel:
	حالت CMD_breakdel:
	    بازگشت set_context_in_breakadd_cmd(xp, arg, cmdidx);

	حالت CMD_scriptnames:
	    بازگشت set_context_in_scriptnames_cmd(xp, arg);
#endif
	حالت CMD_filetype:
	    بازگشت set_context_in_filetype_cmd(xp, arg);

	پیشفرض:
	    بشکن;
    }
    بازگشت پوچ;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    ایستا char_u *
set_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    صحیح			len = 0;
    exarg_T		ea;
    صحیح			compl = EXPAND_NOTHING;
    صحیح			forceit = FALSE;
    صحیح			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_سطر = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    برای (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != پوچ; cmd++)
	;
    xp->xp_pattern = cmd;

    اگر (*cmd == NUL)
	بازگشت پوچ;
    اگر (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	بازگشت پوچ;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    اگر (*cmd == NUL)
	بازگشت پوچ;
    اگر (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	بازگشت پوچ;
    }

    اگر (*cmd == '|' || *cmd == '\n')
	بازگشت cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    اگر (p == پوچ)
	بازگشت پوچ;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    اگر (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    اگر (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    اگر ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	وقتی (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    وقتی (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    اگر (*p == NUL)
	    {
		اگر (ea.argt & EX_ARGOPT)
		    بازگشت set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		اگر (ea.cmdidx == CMD_terminal)
		    بازگشت set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    اگر (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	اگر (*arg == '>')			// append
	{
	    اگر (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	وگرنه اگر (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    اگر (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	اگر (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    اگر (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	وقتی (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    اگر ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	اگر (*arg == NUL)
	    بازگشت p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    اگر ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	اگر (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	وقتی (*p)
	{
	    اگر (*p == Ctrl_V)
	    {
		اگر (p[1] != NUL)
		    ++p;
	    }
	    وگرنه اگر ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		اگر (*(p - 1) != '\\')
		{
		    اگر (*p == '|' || *p == '\n')
			بازگشت p + 1;
		    بازگشت پوچ;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    اگر (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == پوچ)
	// no arguments allowed but there is something
	بازگشت پوچ;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (صحیح)STRLEN(buff);
    وقتی (*p && p < buff + len)
    {
	اگر (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	وگرنه
	{
	    اگر (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    اگر (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    بازگشت set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    تهی
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    صحیح		len,	    // length of command line (excl. NUL)
    صحیح		col,	    // position of cursor
    صحیح		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    صحیح			context;
#endif
    صحیح		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    اگر (col < len)
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
    اگر (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    وگرنه اگر (use_ccline && ccline->درون_داد_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	اگر (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(پوچ, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    وگرنه
#endif
	وقتی (nextcomm != پوچ)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_سطر = str;
    xp->xp_col = col;

    اگر (col < len)
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
    صحیح
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    صحیح		col,		// position of cursor
    صحیح		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = پوچ;
    صحیح		options = WILD_ADD_SLASH|WILD_SILENT;

    اگر (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	بازگشت EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    اگر (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	بازگشت EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (صحیح)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	صحیح	pat_len = xp->xp_pattern_len;
#اگر defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	اگر (unesc != پوچ)
	    pat_src = unesc;
#endif

	اگر (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	وگرنه
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#اگر defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    اگر (file_str == پوچ)
	بازگشت EXPAND_UNSUCCESSFUL;

    اگر (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    اگر (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = پوچ;
    }
    vim_free(file_str);

    بازگشت EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    ایستا صحیح
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	صحیح		*رقم_Matches,
	صحیح		flags,
	صحیح		options)
{
    صحیح		free_pat = FALSE;
    صحیح		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    اگر (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	اگر (pat == پوچ)
	    بازگشت ret;

	pat_end = pat + pat_len;
	برای (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    اگر (*p != '\\')
		ادامه;

	    اگر (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    وگرنه اگر (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    وگرنه اگر (xp->xp_backslash & XP_BS_COMMA)
	    {
		اگر (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		وگرنه اگر (*(p + 1) == ',')
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

    اگر (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, رقم_Matches);
#endif
    }
    وگرنه
    {
	اگر (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	وگرنه اگر (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	وگرنه اگر (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	وگرنه
	    flags = (flags | EW_DIR) & ~EW_FILE;
	اگر (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, رقم_Matches, matches, flags);
    }
    اگر (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    اگر (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	صحیح j;

	برای (j = 0; j < *رقم_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    وقتی (*ptr != NUL)
	    {
		اگر (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		وگرنه اگر (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    بازگشت ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    ایستا char_u *
get_behave_arg(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx == 0)
	بازگشت (char_u *)"mswin";
    اگر (idx == 1)
	بازگشت (char_u *)"xterm";
    بازگشت پوچ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    ایستا char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx < 0)
	بازگشت پوچ;

    اگر (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	نویسه	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	بازگشت (char_u *)opts_all[idx];
    }
    اگر (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	نویسه	*opts_plugin[] = {"plugin", "on", "off"};
	بازگشت (char_u *)opts_plugin[idx];
    }
    اگر (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	نویسه	*opts_indent[] = {"indent", "on", "off"};
	بازگشت (char_u *)opts_indent[idx];
    }
    اگر (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	نویسه	*opts_onoff[] = {"on", "off"};
	بازگشت (char_u *)opts_onoff[idx];
    }
    بازگشت پوچ;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    ایستا char_u *
get_breakadd_arg(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx >= 0 && idx <= 3)
    {
	نویسه	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	اگر (breakpt_expand_what == EXP_BREAKPT_ADD)
	    بازگشت (char_u *)opts[idx];
	وگرنه اگر (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    اگر (idx <= 2)
		بازگشت (char_u *)opts[idx + 1];
	}
	وگرنه
	{
	    // profdel {func, file}
	    اگر (idx <= 1)
		بازگشت (char_u *)opts[idx + 1];
	}
    }
    بازگشت پوچ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    ایستا char_u *
get_scriptnames_arg(expand_T *xp UNUSED, صحیح idx)
{
    scriptitem_T *si;

    اگر (!SCRIPT_ID_VALID(idx + 1))
	بازگشت پوچ;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(پوچ, si->sn_name, NameBuff, MAXPATHL, TRUE);
    بازگشت NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    ایستا char_u *
get_retab_arg(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx == 0)
	بازگشت (char_u *)"-indentonly";
    بازگشت پوچ;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    ایستا char_u *
get_messages_arg(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx == 0)
	بازگشت (char_u *)"clear";
    بازگشت پوچ;
}

    ایستا char_u *
get_mapclear_arg(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx == 0)
	بازگشت (char_u *)"<buffer>";
    بازگشت پوچ;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    ایستا صحیح
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	صحیح		*رقم_Matches)
{
    ایستا ساختار expgen
    {
	صحیح		context;
	char_u	*((*func)(expand_T *, صحیح));
	صحیح		ic;
	صحیح		escaped;
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
	{EXPAND_EXPRESSION, get_عبارت_name, FALSE, TRUE},
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
#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
    صحیح	i;
    صحیح ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    برای (i = 0; i < (صحیح)ARRAY_LENGTH(tab); ++i)
    {
	اگر (xp->xp_context == tab[i].context)
	{
	    اگر (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, رقم_Matches,
						tab[i].func, tab[i].escaped);
	    بشکن;
	}
    }

    بازگشت ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    ایستا صحیح
map_wildopts_to_ewflags(صحیح options)
{
    صحیح		flags;

    flags = EW_DIR;	// include directories
    اگر (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    اگر (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    اگر (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    اگر (options & WILD_SILENT)
	flags |= EW_SILENT;
    اگر (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    اگر (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    بازگشت flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    ایستا صحیح
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    صحیح		*رقم_Matches,
    صحیح		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    صحیح		ret;
    صحیح		flags;
    char_u	*tofree = پوچ;
    صحیح		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    اگر (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	بازگشت expand_files_and_dirs(xp, pat, matches, رقم_Matches, flags,
								options);

    *matches = (char_u **)"";
    *رقم_Matches = 0;
    اگر (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	اگر (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					رقم_Matches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*رقم_Matches, *matches);
#endif
	    بازگشت OK;
	}
	بازگشت FAIL;
    }

    اگر (xp->xp_context == EXPAND_SHELLCMD)
	بازگشت expand_shellcmd(pat, matches, رقم_Matches, flags);
    اگر (xp->xp_context == EXPAND_OLD_SETTING)
	بازگشت ExpandOldSetting(رقم_Matches, matches);
    اگر (xp->xp_context == EXPAND_BUFFERS)
	بازگشت ExpandBufnames(pat, رقم_Matches, matches, options);
#ifdef FEAT_DIFF
    اگر (xp->xp_context == EXPAND_DIFF_BUFFERS)
	بازگشت ExpandBufnames(pat, رقم_Matches, matches,
						options | BUF_DIFF_FILTER);
#endif
    اگر (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	بازگشت expand_tags(xp->xp_context == EXPAND_TAGS, pat, رقم_Matches,
								matches);
    اگر (xp->xp_context == EXPAND_COLORS)
    {
	نویسه *directories[] = {"colors", پوچ};
	بازگشت ExpandRTDir(pat, DIP_START + DIP_OPT, رقم_Matches, matches,
								directories);
    }
    اگر (xp->xp_context == EXPAND_COMPILER)
    {
	نویسه *directories[] = {"compiler", پوچ};
	بازگشت ExpandRTDir(pat, 0, رقم_Matches, matches, directories);
    }
    اگر (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	نویسه *directories[] = {"syntax", پوچ};
	بازگشت ExpandRTDir(pat, 0, رقم_Matches, matches, directories);
    }
    اگر (xp->xp_context == EXPAND_FILETYPE)
    {
	نویسه *directories[] = {"syntax", "indent", "ftplugin", پوچ};
	بازگشت ExpandRTDir(pat, 0, رقم_Matches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    اگر (xp->xp_context == EXPAND_KEYMAP)
    {
	نویسه *directories[] = {"keymap", پوچ};
	بازگشت ExpandRTDir(pat, 0, رقم_Matches, matches, directories);
    }
#endif
#اگر defined(FEAT_EVAL)
    اگر (xp->xp_context == EXPAND_USER_LIST)
	بازگشت ExpandUserList(xp, matches, رقم_Matches);
#endif
    اگر (xp->xp_context == EXPAND_PACKADD)
	بازگشت ExpandPackAddDir(pat, رقم_Matches, matches);
    اگر (xp->xp_context == EXPAND_RUNTIME)
	بازگشت expand_runtime_cmd(pat, رقم_Matches, matches);
    اگر (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	بازگشت expand_pattern_in_buf(pat, xp->xp_جستجو_dir,
		matches, رقم_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    اگر ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	صحیح len = (صحیح)STRLEN(pat) + 20;

	tofree = alloc(len);
	اگر (tofree == پوچ)
	    بازگشت FAIL;
	vim_snprintf((نویسه *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    اگر (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	اگر (regmatch.regprog == پوچ)
	{
	    vim_free(tofree);
	    بازگشت FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    اگر (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, رقم_Matches, matches, fuzzy);
    وگرنه اگر (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, رقم_Matches, matches);
    وگرنه اگر (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, رقم_Matches, matches);
    وگرنه اگر (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, رقم_Matches, matches);
    وگرنه اگر (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, رقم_Matches);
    وگرنه اگر (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, رقم_Matches);
#اگر defined(FEAT_TERMINAL)
    وگرنه اگر (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, رقم_Matches);
#endif
#اگر defined(FEAT_EVAL)
    وگرنه اگر (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, رقم_Matches);
#endif
    وگرنه
	ret = ExpandOther(pat, xp, &regmatch, matches, رقم_Matches);

    اگر (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    بازگشت ret;
}

    صحیح
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    صحیح		*رقم_Matches,
    char_u	*(*func)(expand_T *, صحیح),
					  // returns a string from the list
    صحیح		escaped)
{
    بازگشت ExpandGenericExt(
	pat, xp, regmatch, matches, رقم_Matches, func, escaped, 0);
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
    صحیح
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    صحیح		*رقم_Matches,
    char_u	*(*func)(expand_T *, صحیح),
					  // returns a string from the list
    صحیح		escaped,
    صحیح		sortStartIdx)
{
    صحیح		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = پوچ;
    صحیح		score = 0;
    صحیح		fuzzy;
    صحیح		match;
    صحیح		sort_matches = FALSE;
    صحیح		funcsort = FALSE;
    صحیح		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = پوچ;
    *رقم_Matches = 0;

    اگر (!fuzzy)
	ga_init2(&ga, اندازه(نویسه *), 30);
    وگرنه
	ga_init2(&ga, اندازه(fuzmatch_str_T), 30);

    برای (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	اگر (str == پوچ)	    // end of list
	    بشکن;
	اگر (*str == NUL)	    // skip empty strings
	    ادامه;

	اگر (xp->xp_pattern[0] != NUL)
	{
	    اگر (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    وگرنه
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	وگرنه
	    match = TRUE;

	اگر (!match)
	    ادامه;

	اگر (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	وگرنه
	    str = vim_strsave(str);
	اگر (str == پوچ)
	{
	    اگر (!fuzzy)
	    {
		ga_clear_strings(&ga);
		بازگشت FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    بازگشت FAIL;
	}

	اگر (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    بشکن;
	}

	اگر (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	وگرنه
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

#ifdef FEAT_MENU
	اگر (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    اگر (*str == '\001')
		*str = '.';
	}
#endif

	اگر (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    اگر (ga.ga_len == 0)
	بازگشت OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    اگر (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    اگر (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    اگر (sort_matches && sortStartMatchIdx != -1)
    {
	اگر (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((تهی *)ga.ga_data, (size_t)ga.ga_len, اندازه(char_u *),
							   sort_func_compare);
	وگرنه
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    اگر (!fuzzy)
    {
	*matches = ga.ga_data;
	*رقم_Matches = ga.ga_len;
    }
    وگرنه
    {
	اگر (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    بازگشت FAIL;
	*رقم_Matches = ga.ga_len;
    }

#اگر defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    بازگشت OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    ایستا تهی
expand_shellcmd_onedir(
	char_u		*pathed_pattern,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	صحیح		*رقم_Matches,
	صحیح		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    اگر (expand_wildcards(1, &pathed_pattern, رقم_Matches, matches, flags) != OK)
	بازگشت;

    اگر (ga_grow(gap, *رقم_Matches) == FAIL)
    {
	FreeWild(*رقم_Matches, *matches);
	بازگشت;
    }

    برای (صحیح i = 0; i < *رقم_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	اگر (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    اگر (HASHITEM_EMPTY(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_data)[gap->ga_len++] = name;
		hash_add_item(ht, hi, name, hash);
		name = پوچ;
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
    ایستا صحیح
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    صحیح		*رقم_Matches,	// return: number of matches
    صحیح		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = پوچ;
    صحیح		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    صحیح		flags = flagsarg;
    صحیح		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    اگر (buf == پوچ)
	بازگشت FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    اگر (pat == پوچ)
    {
	vim_free(buf);
	بازگشت FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    برای (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	اگر (*s != '\\')
	    ادامه;

	p = s + 1;
	اگر (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    اگر (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    وگرنه
    {
	// For an absolute name we don't use $PATH.
	اگر (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	اگر (path == پوچ)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, اندازه(نویسه *), 10);
    hash_init(&found_ht);
    برای (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	اگر (*s == NUL)
	{
	    اگر (did_curdir)
		بشکن;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	وگرنه
	{
#اگر defined(MSWIN)
	    e = vim_strchr(s, ';');
#وگرنه
	    e = vim_strchr(s, ':');
#endif
	    اگر (e == پوچ)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    اگر (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    وگرنه
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? اندازه(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	اگر (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    اگر (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		اگر (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, رقم_Matches, flags,
				    &found_ht, &ga);
	}

	اگر (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_data;
    *رقم_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    اگر (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    بازگشت OK;
}

#اگر defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    ایستا تهی *
call_user_expand_func(
    تهی	*(*user_expand_func)(char_u *, صحیح, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    صحیح		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = پوچ;
    تهی	*ret;

    اگر (xp->xp_arg == پوچ || xp->xp_arg[0] == '\0' || xp->xp_سطر == پوچ)
	بازگشت پوچ;

    اگر (ccline->cmdbuff != پوچ)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	صحیح unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	اگر (pat == پوچ)
	    pat = vim_strnsave(xp->xp_pattern, xp->xp_pattern_len);
    }

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = xp->xp_سطر;
    args[2].v_type = VAR_عدد;
    args[2].vval.v_عدد = xp->xp_col;
    args[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, args);

    current_sctx = save_current_sctx;
    اگر (ccline->cmdbuff != پوچ)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    بازگشت ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    ایستا صحیح
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    صحیح		*رقم_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    صحیح		keep;
    garray_T	ga;
    صحیح		fuzzy;
    صحیح		match;
    صحیح		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = پوچ;
    *رقم_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    اگر (retstr == پوچ)
	بازگشت FAIL;

    اگر (!fuzzy)
	ga_init2(&ga, اندازه(نویسه *), 3);
    وگرنه
	ga_init2(&ga, اندازه(fuzmatch_str_T), 3);

    برای (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	اگر (e == پوچ)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	اگر (xp->xp_pattern[0] != NUL)
	{
	    اگر (!fuzzy)
		match = vim_regexec(regmatch, s, (colnr_T)0);
	    وگرنه
	    {
		score = fuzzy_match_str(s, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	وگرنه
	    match = TRUE;		// match everything

	*e = keep;

	اگر (match)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    اگر (p == پوچ)
		بشکن;

	    اگر (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		بشکن;
	    }

	    اگر (!fuzzy)
		((char_u **)ga.ga_data)[ga.ga_len] = p;
	    وگرنه
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	اگر (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    اگر (ga.ga_len == 0)
	بازگشت OK;

    اگر (!fuzzy)
    {
	*matches = ga.ga_data;
	*رقم_Matches = ga.ga_len;
    }
    وگرنه
    {
	اگر (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    بازگشت FAIL;
	*رقم_Matches = ga.ga_len;
    }
    بازگشت OK;
}

    تهی
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    صحیح		*رقم_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    صحیح		have_extra = FALSE;
    صحیح		i;

    ga_init2(&ga, اندازه(نویسه *), 3);
    ga_init2(&ga_abbr, اندازه(نویسه *), 3);
    ga_init2(&ga_kind, اندازه(نویسه *), 3);
    ga_init2(&ga_menu, اندازه(نویسه *), 3);
    ga_init2(&ga_info, اندازه(نویسه *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_اقلام(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = پوچ;
	char_u	*abbr = پوچ;
	char_u	*kind = پوچ;
	char_u	*menu = پوچ;
	char_u	*info = پوچ;

	اگر (tv->v_type == VAR_STRING)
	{
	    اگر (tv->vval.v_string == پوچ)
		ادامه;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	وگرنه اگر (tv->v_type == VAR_DICT && tv->vval.v_dict != پوچ)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    اگر (word == پوچ)
		ادامه;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    اگر (abbr != پوچ || kind != پوچ || menu != پوچ || info != پوچ)
		have_extra = TRUE;
	}
	وگرنه
	    ادامه;  // Skip other types

	اگر (p == پوچ
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
	    بشکن;
	}

	((char_u **)ga.ga_data)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_data)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_data)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_data)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_data)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_data;
    *رقم_Matches = ga.ga_len;
    اگر (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    وگرنه
    {
	// No extra info collected; free the placeholder NULL entries.
	برای (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	برای (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	برای (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	برای (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    ایستا صحیح
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    صحیح		*رقم_Matches)
{
    list_T      *retlist;

    *matches = پوچ;
    *رقم_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    اگر (retlist == پوچ)
	بازگشت FAIL;

    expand_process_user_list(retlist, matches, رقم_Matches, xp);
    list_unref(retlist);
    بازگشت OK;
}
#endif

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    تهی
globpath(
    char_u	*path,
    char_u	*file,
    garray_T	*ga,
    صحیح		expand_options,
    صحیح		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    اگر (buf == پوچ)
	بازگشت;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#اگر defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#وگرنه
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    وقتی (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	اگر (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    صحیح	    رقم_p;
	    char_u  **p;

	    اگر (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    اگر (ExpandFromContext(&xpc, buf, &p, &رقم_p,
			     WILD_SILENT|expand_options) != FAIL && رقم_p > 0)
	    {
		ExpandEscape(&xpc, buf, رقم_p, p, WILD_SILENT|expand_options);

		اگر (ga_grow(ga, رقم_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    برای (صحیح i = 0; i < رقم_p; ++i)
		    {
			((char_u **)ga->ga_data)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		وگرنه
		{
		    FreeWild(رقم_p, p);
		    p = پوچ;
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
    صحیح
wildmenu_translate_کلید(
	cmdline_info_T	*cclp,
	صحیح		کلید,
	expand_T	*xp,
	صحیح		did_wild_list)
{
    صحیح c = کلید;

    اگر (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	انتخاب (c)
	{
	    حالت K_UP:	  c = K_چپ; بشکن;
	    حالت K_DOWN:  c = K_راست; بشکن;
	    حالت K_چپ:  c = K_UP; بشکن;
	    حالت K_راست: c = K_DOWN; بشکن;
	    پیشفرض:	  بشکن;
	}
    }

    اگر (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	اگر (c == K_چپ)
	    c = Ctrl_P;
	وگرنه اگر (c == K_راست)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    اگر (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    بازگشت c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    ایستا تهی
cmdline_del(cmdline_info_T *cclp, صحیح from)
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
    ایستا صحیح
wildmenu_process_کلید_menunames(cmdline_info_T *cclp, صحیح کلید, expand_T *xp)
{
    صحیح		i;
    صحیح		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    اگر (کلید == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	کلید = p_wc;
	کلید_Typed = TRUE;  // in case the key was mapped
    }
    وگرنه اگر (کلید == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	صحیح found = FALSE;

	j = (صحیح)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	وقتی (--j > 0)
	{
	    // check for start of menu name
	    اگر (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		بشکن;
	    }
	    // check for start of submenu name
	    اگر (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		اگر (found)
		{
		    i = j + 1;
		    بشکن;
		}
		وگرنه
		    found = TRUE;
	    }
	}
	اگر (i > 0)
	    cmdline_del(cclp, i);
	کلید = p_wc;
	کلید_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    بازگشت کلید;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    ایستا صحیح
wildmenu_process_کلید_filenames(cmdline_info_T *cclp, صحیح کلید, expand_T *xp)
{
    صحیح		i;
    صحیح		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    اگر (کلید == K_DOWN
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	کلید = p_wc;
	کلید_Typed = TRUE;  // in case the key was mapped
    }
    وگرنه اگر (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && کلید == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	صحیح found = FALSE;

	j = cclp->cmdpos;
	i = (صحیح)(xp->xp_pattern - cclp->cmdbuff);
	وقتی (--j > i)
	{
	    اگر (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    اگر (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		بشکن;
	    }
	}
	اگر (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    کلید = p_wc;
	    کلید_Typed = TRUE;  // in case the key was mapped
	}
    }
    وگرنه اگر (کلید == K_UP)
    {
	// go up a directory
	صحیح found = FALSE;

	j = cclp->cmdpos - 1;
	i = (صحیح)(xp->xp_pattern - cclp->cmdbuff);
	وقتی (--j > i)
	{
	    اگر (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    اگر (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == پوچ
#endif
	       )
	    {
		اگر (found)
		{
		    i = j + 1;
		    بشکن;
		}
		وگرنه
		    found = TRUE;
	    }
	}

	اگر (!found)
	    j = i;
	وگرنه اگر (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	وگرنه اگر (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	وگرنه
	    j = 0;
	اگر (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	وگرنه اگر (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	کلید = p_wc;
	کلید_Typed = TRUE;
    }

    بازگشت کلید;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    صحیح
wildmenu_process_کلید(cmdline_info_T *cclp, صحیح کلید, expand_T *xp)
{
    اگر (xp->xp_context == EXPAND_MENUNAMES)
	بازگشت wildmenu_process_کلید_menunames(cclp, کلید, xp);
    وگرنه اگر ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	بازگشت wildmenu_process_کلید_filenames(cclp, کلید, xp);

    بازگشت کلید;
}

/*
 * Free expanded names when finished walking through the matches
 */
    تهی
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    صحیح skt = کلید_Typed;

    اگر (!p_wmnu || wild_menu_showing == 0)
	بازگشت;

#ifdef FEAT_EVAL
    صحیح save_RedrawingDisabled = RedrawingDisabled;
    اگر (cclp->درون_داد_fn)
	RedrawingDisabled = 0;
#endif

#اگر defined(FEAT_جستجو_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    اگر (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    وگرنه اگر (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	last_status(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    وگرنه
    {
	win_redraw_last_status(topframe);
	redraw_statuslines();
    }
    کلید_Typed = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    اگر (cclp->درون_داد_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#اگر defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    تهی
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    صحیح		filtered = FALSE;
    صحیح		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	بازگشت;

    pat = tv_get_string(&argvars[0]);
    اگر (check_for_string_arg(argvars, 1) == FAIL)
	بازگشت;
    type = tv_get_string(&argvars[1]);

    اگر (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], پوچ);

    اگر (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    اگر (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    اگر (STRCMP(type, "cmdline") == 0)
    {
	صحیح cmdline_len = (صحیح)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (صحیح)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    وگرنه
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (صحیح)STRLEN(xpc.xp_pattern);
	xpc.xp_سطر = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	انتخاب (xpc.xp_context)
	{
	حالت EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    بازگشت;

	حالت EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    اگر (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		بازگشت;
	    }

	    xpc.xp_arg = type + 7;
	    بشکن;

	حالت EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    اگر (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		بازگشت;
	    }

	    xpc.xp_arg = type + 11;
	    بشکن;

# اگر defined(FEAT_MENU)
	حالت EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (صحیح)(xpc.xp_pattern - pattern_start);
	    بشکن;
# endif

# ifdef FEAT_CSCOPE
	حالت EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (صحیح)(xpc.xp_pattern - pattern_start);
	    بشکن;
# endif

# ifdef FEAT_SIGNS
	حالت EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (صحیح)(xpc.xp_pattern - pattern_start);
	    بشکن;
# endif

	حالت EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (صحیح)(xpc.xp_pattern - pattern_start);
	    بشکن;

	حالت EXPAND_SHELLCMDLINE:
	{
	    صحیح context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(پوچ, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (صحیح)(xpc.xp_pattern - pattern_start);
	    بشکن;
	}

	حالت EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    بشکن;

	حالت EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    بشکن;

	پیشفرض:
	    بشکن;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	صحیح	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	اگر (unesc != پوچ)
	    pat_src = unesc;

	اگر (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	وگرنه
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    اگر (rettv_list_alloc(rettv) == OK && pat != پوچ)
    {
	صحیح	i;

	ExpandOne(&xpc, pat, پوچ, options, WILD_ALL_KEEP);

	برای (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    تهی
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    صحیح		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (صحیح)STRLEN(pat);
    set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_to_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    تهی
f_cmdcomplete_info(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    صحیح		    idx;
    صحیح		    ret = OK;

    اگر (rettv_dict_alloc(rettv) == FAIL || ccline == پوچ
	    || ccline->xpc == پوچ || ccline->xpc->xp_files == پوچ)
	بازگشت;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (صحیح)cmdline_orig.length);
    اگر (ret == OK)
	ret = dict_add_عدد(retdict, "pum_visible", pum_visible());
    اگر (ret == OK)
	ret = dict_add_عدد(retdict, "selected", ccline->xpc->xp_selected);
    اگر (ret == OK)
    {
	li = list_alloc();
	اگر (li == پوچ)
	    بازگشت;
	ret = dict_add_list(retdict, "matches", li);
	اگر (ret == FAIL)
	    list_unref(li);
	برای (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    ایستا صحیح
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*سطر, *start_سطر, *end_سطر;
    صحیح		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    صحیح		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != پوچ;

    اگر (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	بازگشت FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_سطر = ml_get(start->lnum);
    char_u  *start_ptr = start_سطر + start->col;
    صحیح	    is_single_سطر = start->lnum == end->lnum;

    segment_len = is_single_سطر ? (end->col - start->col)
			: (صحیح)(ml_get_len(start->lnum) - start->col);
    اگر (ga_grow(&ga, segment_len + 2) != OK)
	برو fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    اگر (!is_single_سطر)
    {
	اگر (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	وگرنه
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    اگر (!is_single_سطر)
    {
	برای (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    صحیح  linelen;

	    سطر = ml_get(lnum);
	    linelen = (صحیح)ml_get_len(lnum);
	    اگر (ga_grow(&ga, linelen + 2) != OK)
		برو fail;
	    ga_concat_len(&ga, سطر, linelen);
	    اگر (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    وگرنه
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_سطر = ml_get(end->lnum);
    word_end = find_word_end(end_سطر + end->col);
    segment_len = (صحیح)(word_end - end_سطر);
    اگر (ga_grow(&ga, segment_len) != OK)
	برو fail;
    ga_concat_len(&ga, end_سطر + (is_single_سطر ? end->col : 0),
	    segment_len - (is_single_سطر ? end->col : 0));

    // Null-terminate
    اگر (ga_grow(&ga, 1) != OK)
	برو fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    بازگشت OK;

fail:
    ga_clear(&ga);
    بازگشت FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    ایستا صحیح
is_regex_match(char_u *pat, char_u *str)
{
    اگر (STRCMP(pat, str) == 0)
	بازگشت TRUE;

    regmatch_T	regmatch;
    صحیح		result;

    ++emsg_off;
    ++پیام_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --پیام_silent;

    اگر (regmatch.regprog == پوچ)
	بازگشت FALSE;
    regmatch.rm_ic = p_ic;
    اگر (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++پیام_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --پیام_silent;

    vim_regfree(regmatch.regprog);
    بازگشت result;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    ایستا char_u *
concat_pattern_with_buffer_match(
	char_u *pat,
	صحیح pat_len,
	pos_T *end_match_pos,
	صحیح lowercase UNUSED)
{
    char_u  *سطر = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(سطر + end_match_pos->col);
    صحیح	    match_len = (صحیح)(word_end - (سطر + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    اگر (match == پوچ)
	بازگشت پوچ;
    mch_memmove(match, pat, pat_len);
    اگر (match_len > 0)
    {
#اگر defined(FEAT_EVAL) || defined(FEAT_SPELL)
	اگر (lowercase)
	{
	    char_u  *mword = vim_strnsave(سطر + end_match_pos->col,
		    match_len);
	    اگر (mword == پوچ)
		برو cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    اگر (lower == پوچ)
		برو cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	وگرنه
#endif
	    mch_memmove(match + pat_len, سطر + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    بازگشت match;

#اگر defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    بازگشت پوچ;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    ایستا صحیح
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    صحیح		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    صحیح		*رقم_Matches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    صحیح		found_new_match;
    صحیح		looped_around = FALSE;
    صحیح		pat_len;
    صحیح		has_range = FALSE;
    صحیح		compl_started = FALSE;
    صحیح		جستجو_flags;
    char_u	*match, *full_match;
    صحیح		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != پوچ;

#ifdef FEAT_جستجو_EXTRA
    has_range = جستجو_first_سطر != 0;
#endif

    *matches = پوچ;
    *رقم_Matches = 0;

    اگر (pat == پوچ || *pat == NUL)
	بازگشت FAIL;

    pat_len = (صحیح)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_جستجو_EXTRA
    اگر (has_range)
	cur_match_pos.lnum = جستجو_first_سطر;
    وگرنه
#endif
	cur_match_pos = pre_incsearch_pos;

    جستجو_flags = جستجو_OPT | جستجو_NOOF | جستجو_PEEK | جستجو_NFMSG
	| (has_range ? جستجو_START : 0);

    ga_init2(&ga, اندازه(char_u *), 10); // Use growable array of char_u*

    برای (;;)
    {
	++emsg_off;
	++پیام_silent;
	found_new_match = searchit(پوچ, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		جستجو_flags, RE_LAST, پوچ);
	--پیام_silent;
	--emsg_off;

	اگر (found_new_match == FAIL)
	    بشکن;

#ifdef FEAT_جستجو_EXTRA
	// If in range mode, check if match is within the range
	اگر (has_range && (cur_match_pos.lnum < جستجو_first_سطر
		    || cur_match_pos.lnum > جستجو_last_سطر))
		بشکن;
#endif

	اگر (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    اگر ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		اگر (looped_around)
		    بشکن;
		وگرنه
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	اگر (char_avail() || got_int)
	{
	    اگر (got_int)
	    {
		(تهی)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    برو cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	اگر (end_match_pos.lnum > curbuf->b_ml.ml_سطر_شمار)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    ادامه;
	}

	// Extract the matching text prepended to completed word
	اگر (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    بشکن;

	اگر (exacttext)
	    match = full_match;
	وگرنه
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    اگر (match == پوچ || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		اگر (match == پوچ || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    ادامه;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	برای (صحیح i = 0; i < ga.ga_len; ++i)
	{
	    اگر (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		بشکن;
	    }
	}
	اگر (match != پوچ)
	{
	    اگر (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		برو cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    اگر (ga.ga_len > TAG_MANY)
		بشکن;
	}
	اگر (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *رقم_Matches = ga.ga_len;
    بازگشت OK;

cleanup:
    ga_clear_strings(&ga);
    بازگشت FAIL;
}
