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

#ضمن "vim.h"

ساكن صحيح	cmd_showtail;	// Only show path tail in lists ?
ساكن صحيح	may_expand_pattern = FALSE;
ساكن pos_T	pre_incsearch_pos; // Cursor position when incsearch started

ساكن فراغ	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, صحيح usefilter, expand_T *xp, صحيح *complp);
ساكن صحيح	ExpandFromContext(expand_T *xp, char_u *, char_u ***, صحيح *, صحيح);
ساكن char_u	*showmatches_gettail(char_u *s);
ساكن صحيح	expand_showtail(expand_T *xp);
ساكن صحيح	expand_shellcmd(char_u *filepat, char_u ***matches, صحيح *اڠک_Matches, صحيح flagsarg);
#إذا defined(FEAT_EVAL)
ساكن صحيح	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, صحيح *اڠک_Matches);
ساكن صحيح	ExpandUserList(expand_T *xp, char_u ***matches, صحيح *اڠک_Matches);
ساكن char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
ساكن char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, صحيح srclen, صحيح *new_lenp);
#endif
ساكن صحيح	expand_pattern_in_buf(char_u *pat, صحيح dir, char_u ***matches, صحيح *اڠک_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ساكن pumitem_T *compl_match_array = عدم;
ساكن صحيح compl_match_arraysize;
// First column in cmdline of the matched item for completion.
ساكن صحيح compl_startcol;
ساكن صحيح compl_selected;
// cmdline before expansion
ساكن string_T cmdline_orig = {عدم, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    ساكن صحيح
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    بدل (xp->xp_context)
    {
    حالة EXPAND_BOOL_SETTINGS:
    حالة EXPAND_COLORS:
    حالة EXPAND_COMPILER:
    حالة EXPAND_DIRECTORIES:
    حالة EXPAND_DIRS_IN_CDPATH:
    حالة EXPAND_FILES:
    حالة EXPAND_FILES_IN_PATH:
    حالة EXPAND_FILETYPE:
    حالة EXPAND_FILETYPECMD:
    حالة EXPAND_HELP:
    حالة EXPAND_KEYMAP:
    حالة EXPAND_OLD_SETTING:
    حالة EXPAND_STRING_SETTING:
    حالة EXPAND_SETTING_SUBTRACT:
    حالة EXPAND_OWNSYNTAX:
    حالة EXPAND_PACKADD:
    حالة EXPAND_RUNTIME:
    حالة EXPAND_SHELLCMD:
    حالة EXPAND_SHELLCMDLINE:
    حالة EXPAND_TAGS:
    حالة EXPAND_TAGS_LISTFILES:
	أرجع FALSE;

    افتراضي:
	اكسر;
    }

    أرجع vim_strchr(p_wop, WOP_FUZZY) != عدم;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    صحيح
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    أرجع vim_strchr(p_wop, WOP_FUZZY) != عدم && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    ساكن صحيح
sort_func_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    إذا (*p1 != '<' && *p2 == '<') أرجع -1;
    إذا (*p1 == '<' && *p2 != '<') أرجع 1;
    أرجع STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    ساكن فراغ
wildescape(
    expand_T	*xp,
    char_u	*str,
    صحيح		numfiles,
    char_u	**files)
{
    char_u	*p;
    صحيح		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    إذا (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	لكل (صحيح i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    إذا (xp->xp_backslash & XP_BS_THREE)
	    {
		حرف *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		إذا (p != عدم)
		{
		    vim_free(files[i]);
		    files[i] = p;
#إذا defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    إذا (p != عدم)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    وإلا إذا (xp->xp_backslash & XP_BS_COMMA)
	    {
		إذا (vim_strchr(files[i], ',') != عدم)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    إذا (p != عدم)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ifdef BACKSLASH_IN_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#وإلا
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#endif
	    إذا (p != عدم)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    إذا (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	إذا (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    وإلا إذا (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	لكل (صحيح i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    إذا (p != عدم)
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
    ساكن فراغ
ExpandEscape(
    expand_T	*xp,
    char_u	*str,
    صحيح		numfiles,
    char_u	**files,
    صحيح		options)
{
    // May change home directory back to "~"
    إذا (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    إذا (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    صحيح
nextwild(
    expand_T	*xp,
    صحيح		type,
    صحيح		options,	// extra options for ExpandOne()
    صحيح		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    صحيح		i;
    char_u	*p;
    صحيح		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    صحيح		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    إذا (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	إذا (ccline->ماسوقن_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	وإلا
#endif
	{
	    may_expand_pattern = options & WILD_MAY_EXPAND_PATTERN;
	    set_expand_context(xp);
	    may_expand_pattern = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    إذا (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	أرجع OK;  // Something illegal on command line
    }
    إذا (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	أرجع FAIL;
    }

    i = (صحيح)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    إذا (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	أرجع FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    إذا (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	ڤسنن_puts("...");	    // show that we are busy
	out_flush();
    }

    إذا (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, عدم, عدم, 0, type);
    }
    وإلا
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	صحيح	pat_len = xp->xp_pattern_len;
#إذا defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	إذا (unesc != عدم)
	    pat_src = unesc;
#endif

	إذا (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	وإلا
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#إذا defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	إذا (tmp == عدم)
	    p = عدم;
	وإلا
	{
	    صحيح use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    إذا (escape)
		use_options |= WILD_ESCAPE;
	    إذا (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    إذا (p != عدم && type == WILD_LONGEST)
	    {
		صحيح	j;

		لكل (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    إذا (c == '*' || c == '?')
			اكسر;
		}
		إذا ((صحيح)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    إذا (!wild_navigate && ccline->cmdbuff != عدم)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	إذا (cmdline_orig.string == عدم)
	    cmdline_orig.length = 0;
	وإلا
	    cmdline_orig.length = ccline->cmdlen;
    }

    إذا (p != عدم && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	صحيح	difflen;
	صحيح	v = OK;

	difflen = (صحيح)plen - xp->xp_pattern_len;
	إذا (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	إذا (v == OK)
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
    إذا (xp->xp_context == EXPAND_MAPPINGS && p == عدم)
	أرجع FAIL;

    إذا (xp->xp_numfiles <= 0 && p == عدم)
	beep_flush();
    وإلا إذا (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(فراغ)ExpandOne(xp, عدم, عدم, 0, WILD_FREE);

    vim_free(p);

    أرجع OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    ساكن صحيح
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	صحيح		اڠک_Matches,
	صحيح		showtail)
{
    صحيح	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, اڠک_Matches);
    إذا (compl_match_array == عدم)
	أرجع EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = اڠک_Matches;
    لكل (صحيح i = 0; i < اڠک_Matches; i++)
    {
	compl_match_array[i].pum_text = (xp->xp_files_abbr != عدم
				    && xp->xp_files_abbr[i] != عدم)
					? xp->xp_files_abbr[i]
					: SHOW_MATCH(i);
	compl_match_array[i].pum_info = xp->xp_files_info != عدم
					    ? xp->xp_files_info[i] : عدم;
	compl_match_array[i].pum_extra = xp->xp_files_menu != عدم
					    ? xp->xp_files_menu[i] : عدم;
	compl_match_array[i].pum_kind = xp->xp_files_kind != عدم
					    ? xp->xp_files_kind[i] : عدم;
	compl_match_array[i].pum_user_abbr_hlattr = -1;
	compl_match_array[i].pum_user_kind_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == عدم ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    prefix_len = vim_strsize(xp->xp_pattern);
    إذا (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    أرجع EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    فراغ
cmdline_pum_display(فراغ)
{
    إذا (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    صحيح
cmdline_pum_active(فراغ)
{
    أرجع pum_visible() && compl_match_array != عدم;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    فراغ
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, صحيح defer_redraw)
{
    صحيح	save_کونچي_Typed = کونچي_Typed;
#ifdef FEAT_EVAL
    صحيح	save_RedrawingDisabled = RedrawingDisabled;
    إذا (cclp->ماسوقن_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    إذا (!defer_redraw)
    {
	صحيح save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    وإلا
	pum_call_update_screen();
    redrawcmd();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    کونچي_Typed = save_کونچي_Typed;
#ifdef FEAT_EVAL
    إذا (cclp->ماسوقن_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

    فراغ
cmdline_pum_cleanup(cmdline_info_T *cclp)
{
    cmdline_pum_remove(cclp, FALSE);
    wildmenu_cleanup(cclp);
}

/*
 * Returns the starting column number to use for the cmdline completion popup
 * menu.
 */
    صحيح
cmdline_compl_startcol(فراغ)
{
    أرجع compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(فراغ)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    أرجع xp == عدم ? عدم : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    صحيح
cmdline_compl_is_fuzzy(فراغ)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    أرجع xp != عدم && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    ساكن صحيح
skip_status_match_char(expand_T *xp, char_u *s)
{
    إذا ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	إذا (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    أرجع 2;
#endif
	أرجع 1;
    }
    أرجع 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    ساكن صحيح
status_match_len(expand_T *xp, char_u *s)
{
    صحيح	len = 0;

#ifdef FEAT_MENU
    صحيح emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    إذا (emenu && menu_is_separator(s))
	أرجع 1;
#endif

    طالما (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    أرجع len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    ساكن فراغ
win_redr_status_matches(
    expand_T	*xp,
    صحيح		اڠک_matches,
    char_u	**matches,	// list of matches
    صحيح		match,
    صحيح		showtail)
{
    صحيح		row;
    char_u	*buf;
    صحيح		len;
    صحيح		clen;		// length in screen cells
    صحيح		fillchar;
    صحيح		attr;
    صحيح		i;
    صحيح		highlight = TRUE;
    char_u	*selstart = عدم;
    صحيح		selstart_col = 0;
    char_u	*selend = عدم;
    ساكن صحيح	first_match = 0;
    صحيح		add_کيري = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    صحيح		emenu;
#endif
    صحيح		l;

    إذا (matches == عدم)	// interrupted completion?
	أرجع;

    إذا (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    وإلا
	buf = alloc(topframe->fr_width + 1);
    إذا (buf == عدم)
	أرجع;

    إذا (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    إذا (match == 0)
	first_match = 0;
    وإلا إذا (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_کيري = TRUE;
    }
    وإلا
    {
	// check if match fits on the screen
	لكل (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	إذا (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	إذا (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    لكل (i = match; i < اڠک_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		إذا (clen >= topframe->fr_width)
		    اكسر;
	    }
	    إذا (i == اڠک_matches)
		add_کيري = TRUE;
	}
    }
    إذا (add_کيري)
	طالما (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    إذا (clen >= topframe->fr_width)
		اكسر;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    إذا (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    وإلا
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    طالما (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	إذا (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	إذا (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (صحيح)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	وإلا
#endif
	لكل ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    إذا (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    وإلا
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (صحيح)STRLEN(buf + len);
	    }
	}
	إذا (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	إذا (++i == اڠک_matches)
		اكسر;
    }

    إذا (i != اڠک_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    إذا (row >= 0)
    {
	إذا (wild_menu_showing == 0)
	{
	    إذا (ڤسنن_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		إذا (cmdline_row == Rows - 1)
		{
		    screen_del_باريس_سموا(0, 0, 1, (صحيح)Rows, TRUE, 0, عدم);
		    ++ڤسنن_scrolled;
		}
		وإلا
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    وإلا
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		إذا (lastwin->w_status_height == 0)
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
	إذا (selstart != عدم && highlight)
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

#إذا defined(FEAT_EVAL)
/*
 * Apply -completeopt=escape to a string about to be inserted into the command
 * line as a completion result.  If "str" is non-NULL and the active expansion
 * context is a customlist/custom user command with UCC_ESCAPE set, free "str"
 * and return a newly-allocated copy with spaces, tabs and backslashes prefixed
 * by a backslash.  Otherwise return "str" unchanged.
 */
    ساكن char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    إذا (str == عدم)
	أرجع عدم;
    إذا ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	أرجع str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    إذا (p == عدم)
	أرجع str;
    vim_free(str);
    أرجع p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    ساكن char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    صحيح		srclen,
    صحيح		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    إذا ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	أرجع عدم;

    buf = alloc(srclen + 1);
    إذا (buf == عدم)
	أرجع عدم;

    d = buf;
    end = src + srclen;
    لكل (p = src; p < end; ++p)
    {
	إذا (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (صحيح)(d - buf);
    أرجع buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    ساكن char_u *
get_next_or_prev_match(صحيح mode, expand_T *xp)
{
    صحيح	    findex = xp->xp_selected;
    صحيح	    ht;

    // When no matches found, return NULL
    إذا (xp->xp_numfiles <= 0)
	أرجع عدم;

    إذا (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	إذا (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    وإلا إذا (mode == WILD_NEXT)
    {
	// Select the next entry
	++findex;
    }
    وإلا   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	إذا (ht > 3)
	    ht -= 2;

	إذا (mode == WILD_PAGEUP)
	{
	    إذا (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    وإلا إذا (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    وإلا
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	وإلا    // mode == WILD_PAGEDOWN
	{
	    إذا (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    وإلا إذا (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    وإلا
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    إذا (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	إذا (xp->xp_orig != عدم)
	    findex = -1;
	وإلا
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    إذا (p_wmnu)
    {
	إذا (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	وإلا إذا (vim_strchr(p_wop, WOP_PUM) != عدم)
	{
	    إذا (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	وإلا
	    win_redr_status_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    إذا (findex == -1)
	أرجع vim_strsave(xp->xp_orig);
#إذا defined(FEAT_EVAL)
    أرجع apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#وإلا
    أرجع vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    ساكن char_u *
ExpandOne_start(صحيح mode, expand_T *xp, char_u *str, صحيح options)
{
    صحيح		non_suf_match;		// number without matching suffix
    صحيح		i;
    char_u	*ss = عدم;

    // Do the expansion.
    إذا (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	إذا (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    وإلا إذا (xp->xp_numfiles == 0)
    {
	إذا (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    وإلا
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	إذا (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    إذا (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    وإلا
		non_suf_match = 1;
	    إذا ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		لكل (i = 0; i < 2; ++i)
		    إذا (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    إذا (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		إذا (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		وإلا إذا (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    إذا (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    أرجع ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    ساكن char_u *
find_longest_match(expand_T *xp, صحيح options)
{
    long_u	len;
    صحيح		mb_len = 1;
    صحيح		c0, ci;
    صحيح		i;
    char_u	*ss;

    لكل (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	إذا (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	وإلا
	    c0 = xp->xp_files[0][len];
	لكل (i = 1; i < xp->xp_numfiles; ++i)
	{
	    إذا (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    وإلا
		ci = xp->xp_files[i][len];
	    إذا (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		إذا (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    اكسر;
	    }
	    وإلا إذا (c0 != ci)
		اكسر;
	}
	إذا (i < xp->xp_numfiles)
	{
	    إذا (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    اكسر;
	}
    }

    ss = alloc(len + 1);
    إذا (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    أرجع ss;
}

    فراغ
free_xp_files_extra(expand_T *xp, صحيح numfiles)
{
    إذا (xp->xp_files_abbr != عدم)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = عدم;
    }
    إذا (xp->xp_files_kind != عدم)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = عدم;
    }
    إذا (xp->xp_files_menu != عدم)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = عدم;
    }
    إذا (xp->xp_files_info != عدم)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = عدم;
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
    صحيح		options,
    صحيح		mode)
{
    char_u	*ss = عدم;
    صحيح		orig_saved = FALSE;
#إذا defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    صحيح		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    إذا (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	أرجع get_next_or_prev_match(mode, xp);

    إذا (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    وإلا إذا (mode == WILD_APPLY)
    {
	إذا (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	وإلا
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#إذا defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    إذا (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	إذا (compl_match_array != عدم)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    إذا (mode == WILD_FREE)	// only release file name
	أرجع عدم;

    إذا (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#إذا defined(FEAT_EVAL)
	إذا (ss != عدم)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    إذا (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#إذا defined(FEAT_EVAL)
	إذا (ss != عدم)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    إذا (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	حرف	*prefix = "";
	حرف	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	صحيح	n = xp->xp_numfiles - 1;
	صحيح	i;
#إذا defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = عدم;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	إذا ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    إذا (escaped != عدم)
	    {
		لكل (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    إذا (escaped[i] == عدم)
		    {
			طالما (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			اكسر;
		    }
		}
		إذا (escaped != عدم)
		    files = escaped;
	    }
	}
#وإلا
	char_u	**files = xp->xp_files;
#endif

	إذا (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	وإلا إذا (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	لكل (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	إذا (ss != عدم)
	{
	    size_t  ss_len = 0;

	    لكل (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (حرف *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (حرف *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#إذا defined(FEAT_EVAL)
	إذا (escaped != عدم)
	{
	    لكل (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    إذا (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    إذا (!orig_saved)
	vim_free(orig);

#إذا defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    إذا (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    أرجع ss;
}

/*
 * Prepare an expand structure for use.
 */
    فراغ
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
    فراغ
ExpandCleanup(expand_T *xp)
{
    إذا (xp->xp_numfiles >= 0)
    {
	free_xp_files_extra(xp, xp->xp_numfiles);
	FreeWild(xp->xp_numfiles, xp->xp_files);
	xp->xp_numfiles = -1;
    }
    VIM_CLEAR(xp->xp_orig);
}

    فراغ
clear_cmdline_orig(فراغ)
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
    ساكن فراغ
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	صحيح		اڠک_Matches,
	صحيح		باريس_سموا,
	صحيح		linenr,
	صحيح		maxlen,
	صحيح		showtail,
	صحيح		dir_attr)
{
    صحيح		i, j;
    صحيح		isdir;
    صحيح		lastlen;
    char_u	*p;

    lastlen = 999;
    لكل (j = linenr; j < اڠک_Matches; j += باريس_سموا)
    {
	إذا (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    ڤسنن_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    ڤسنن_advance(maxlen + 1);
	    ڤسنن_puts((حرف *)p);
	    ڤسنن_advance(maxlen + 3);
	    ڤسنن_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    اكسر;
	}
	لكل (i = maxlen - lastlen; --i >= 0; )
	    ڤسنن_putchar(' ');
	إذا (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    إذا (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_path;
		char_u	*path;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_path = expand_env_save_opt(matches[j], TRUE, عدم);
		path = exp_path != عدم ? exp_path : matches[j];
		halved_slash = backslash_halve_save(path);
		isdir = mch_isdir(halved_slash != عدم ? halved_slash
			: matches[j]);
		vim_free(exp_path);
		إذا (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    وإلا
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    إذا (showtail)
		p = SHOW_MATCH(j);
	    وإلا
	    {
		home_replace(عدم, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	وإلا
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = ڤسنن_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    إذا (ڤسنن_col > 0)	// when not wrapped around
    {
	ڤسنن_clr_eos();
	ڤسنن_putchar('\n');
    }
    out_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    ساكن صحيح
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	صحيح		اڠک_Matches,
	صحيح		showtail,
	صحيح		noselect)
{
    صحيح	retval = cmdline_pum_create(ccline, xp, matches, اڠک_Matches, showtail);

    إذا (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    أرجع retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    صحيح
showmatches(
    expand_T	*xp,
    صحيح		display_wildmenu,
    صحيح		display_list,
    صحيح		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    صحيح		اڠک_Matches;
    char_u	**matches;
    صحيح		i;
    صحيح		maxlen;
    صحيح		باريس_سموا;
    صحيح		columns;
    صحيح		attr;
    صحيح		showtail;
    صحيح		noselect = (wim_flags_arg & WIM_NOSELECT);
    صحيح		noinsert = (wim_flags_arg & WIM_NOINSERT);
    صحيح		cmdline_unchanged = noselect || noinsert;

    إذا (xp->xp_numfiles == -1)
    {
	صحيح retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&اڠک_Matches, &matches);
	إذا (retval != EXPAND_OK)
	    أرجع retval;
	showtail = expand_showtail(xp);
    }
    وإلا
    {
	اڠک_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    إذا (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != عدم)
	أرجع show_pum_matches(ccline, xp, matches, اڠک_Matches,
		showtail && !cmdline_unchanged, noselect);

    إذا (display_list)
    {
	ڤسنن_didany = FALSE;		// lines_left will be set
	ڤسنن_start();			// prepare for paging
	ڤسنن_putchar('\n');
	out_flush();
	cmdline_row = ڤسنن_row;
	ڤسنن_didany = FALSE;		// lines_left will be set again
	ڤسنن_start();			// prepare for paging
    }

    إذا (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    وإلا إذا (display_wildmenu && !display_list)
	win_redr_status_matches(xp, اڠک_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    وإلا إذا (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	لكل (i = 0; i < اڠک_Matches; ++i)
	{
	    صحيح	len;
	    إذا (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(عدم, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    وإلا
		len = vim_strsize(SHOW_MATCH(i));
	    إذا (len > maxlen)
		maxlen = len;
	}

	إذا (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    باريس_سموا = اڠک_Matches;
	وإلا
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    إذا (columns < 1)
		columns = 1;
	    باريس_سموا = (اڠک_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	إذا (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    ڤسنن_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    ڤسنن_clr_eos();
	    ڤسنن_advance(maxlen - 3);
	    ڤسنن_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	لكل (i = 0; i < باريس_سموا; ++i)
	{
	    showmatches_oneline(xp, matches, اڠک_Matches, باريس_سموا, i,
						maxlen, showtail, attr);
	    إذا (got_int)
	    {
		got_int = FALSE;
		اكسر;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = ڤسنن_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    إذا (display_wildmenu && display_list)
    {
	إذا (vim_strchr(p_wop, WOP_PUM) != عدم)
	    (فراغ)show_pum_matches(ccline, xp, matches, اڠک_Matches,
		    showtail && !cmdline_unchanged, noselect);
	وإلا
	    win_redr_status_matches(xp, اڠک_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    إذا (xp->xp_numfiles == -1)
    {
	FreeWild(اڠک_Matches, matches);
	free_xp_files_extra(xp, اڠک_Matches);
    }

    أرجع EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    ساكن char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    صحيح		had_sep = FALSE;

    لكل (p = s; *p != NUL; )
    {
	إذا (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	وإلا إذا (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    أرجع t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    ساكن صحيح
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    إذا (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	أرجع FALSE;

    end = gettail(xp->xp_pattern);
    إذا (end == xp->xp_pattern)		// there is no path separator
	أرجع FALSE;

    لكل (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	إذا (rem_backslash(s))
	    ++s;
	وإلا إذا (vim_strchr((char_u *)"*?[", *s) != عدم)
	    أرجع FALSE;
    }
    أرجع TRUE;
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
    صحيح		len,
    صحيح		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    صحيح		i, j;
    صحيح		new_len;
    char_u	*tail;
    صحيح		ends_in_star;

    إذا (context != EXPAND_FILES
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
	إذا (context == EXPAND_FINDFUNC
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
	وإلا
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    لكل (i = 0; i < len; i++)
	    {
		إذا (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		إذا (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		إذا ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    إذا (retval != عدم)
	    {
		retval[0] = '^';
		j = 1;
		لكل (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    إذا (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			اكسر;

		    بدل (fname[i])
		    {
			حالة '*':   retval[j++] = '.';
				    اكسر;
			حالة '~':   retval[j++] = '\\';
				    اكسر;
			حالة '?':   retval[j] = '.';
				    استمر;
			حالة '.':   إذا (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    اكسر;
			حالة '\\':  إذا (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    اكسر;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    وإلا
    {
	retval = alloc(len + 4);
	إذا (retval != عدم)
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
	    لكل (i = len - 2; i >= 0; --i)
	    {
		إذا (retval[i] != '\\')
		    اكسر;
		ends_in_star = !ends_in_star;
	    }
#endif
	    إذا ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == عدم
		    && vim_strchr(retval, '`') == عدم)
		retval[len++] = '*';
	    وإلا إذا (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    أرجع retval;
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
    فراغ
set_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    إذا ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_چارين_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_چارين_EXTRA
	چارين_first_باريس = 0; // Search entire buffer
#endif
	أرجع;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    إذا (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->ماسوقن_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	أرجع;
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
    ساكن char_u *
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, صحيح *complp)
{
    char_u	*p = عدم;
    صحيح		len = 0;
    صحيح		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    إذا (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    وإلا
    {
	p = cmd;
	طالما (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	إذا (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    طالما (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	إذا (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    طالما (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	إذا (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != عدم)
	    ++p;
	len = (صحيح)(p - cmd);

	إذا (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    أرجع عدم;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	إذا ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    طالما (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    إذا (*p == NUL && ASCII_ISALNUM(p[-1]))
	أرجع عدم;

    إذا (eap->cmdidx == CMD_SIZE)
    {
	إذا (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != عدم)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	وإلا إذا (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, عدم, xp, complp);
	    إذا (p == عدم)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    إذا (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	أرجع عدم;
    }

    أرجع p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    ساكن فراغ
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	صحيح		usefilter,
	expand_T	*xp,
	صحيح		*complp)
{
    char_u	*p;
    صحيح		c;
    صحيح		in_quote = FALSE;
    char_u	*bow = عدم;	// Beginning of word
    صحيح		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    طالما (*p != NUL)
    {
	إذا (has_mbyte)
	    c = mb_ptr2char(p);
	وإلا
	    c = *p;
	إذا (c == '\\' && p[1] != NUL)
	    ++p;
	وإلا إذا (c == '`')
	{
	    إذا (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	وإلا إذا (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != عدم && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    طالما (*p != NUL)
	    {
		إذا (has_mbyte)
		    c = mb_ptr2char(p);
		وإلا
		    c = *p;
		إذا (c == '`' || vim_isfilec_or_wc(c))
		    اكسر;
		إذا (has_mbyte)
		    len = (*mb_ptr2len)(p);
		وإلا
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    إذا (in_quote)
		bow = p;
	    وإلا
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    إذا (bow != عدم && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    إذا (usefilter
	    || (eap != عدم
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	إذا (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    إذا (*xp->xp_pattern == '$')
    {
	لكل (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    إذا (!vim_isIDc(*p))
		اكسر;
	إذا (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    إذا (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    إذا (*xp->xp_pattern == '~')
    {
	لكل (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	إذا (*p == NUL && p > xp->xp_pattern + 1
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
    ساكن char_u *
set_context_in_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    إذا (p == عدم)
	xp->xp_pattern = arg;
    وإلا
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    أرجع عدم;
}

#ifdef FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    ساكن char_u *
set_context_in_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    إذا (p == عدم)
	xp->xp_pattern = arg;
    وإلا
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    أرجع عدم;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    ساكن char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    إذا (*arg != NUL)
	arg = skip_vimgrep_pat(arg, عدم, عدم);
    إذا (arg == عدم || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	أرجع عدم;
    }
    أرجع skipwhite(arg);
}

#ifdef FEAT_چارين_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    ساكن char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    إذا (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	إذا (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    أرجع find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    ساكن char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    صحيح		delim;

    delim = *arg;	    // get the delimiter
    إذا (delim)
	++arg;		    // skip delimiter if there is one

    طالما (arg[0] != NUL && arg[0] != delim)
    {
	إذا (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    إذا (arg[0] != NUL)
	أرجع arg + 1;

    أرجع عدم;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    ساكن char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    صحيح		delim;

    delim = *arg;
    إذا (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	إذا (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    طالما (arg[0] != NUL && arg[0] != delim)
	    {
		إذا (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    إذا (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    طالما (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == عدم)
	++arg;
    إذا (arg[0] != NUL)
	أرجع arg;

    أرجع عدم;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    ساكن char_u *
find_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    إذا (*arg != '/')
	أرجع عدم;

    // Match regexp, not just whole words
    لكل (++arg; *arg && *arg != '/'; arg++)
	إذا (*arg == '\\' && arg[1] != NUL)
	    arg++;
    إذا (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	إذا (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == عدم)
	    xp->xp_context = EXPAND_NOTHING;
	وإلا
	    أرجع arg;
    }

    أرجع عدم;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    ساكن char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    طالما ((xp->xp_pattern = vim_strchr(arg, ' ')) != عدم)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    إذا (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    أرجع عدم;
}
#endif

#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    ساكن char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    إذا (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    وإلا
    {
	إذا ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_pattern = skipwhite(p);
	}
	وإلا
	    xp->xp_context = EXPAND_NOTHING;
    }

    أرجع عدم;
}
#endif

ساكن تعداد
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
ساكن تعداد
{
    EXP_BREAKPT_ADD,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    ساكن char_u *
set_context_in_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_start;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_pattern = arg;

    إذا (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    وإلا إذا (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    وإلا
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    إذا (*p == NUL)
	أرجع عدم;
    subcmd_start = p;

    إذا (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	إذا (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    إذا (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		أرجع عدم;
	    }
	    p = skipwhite(p);
	}
	إذا (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	وإلا
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    وإلا إذا (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    أرجع عدم;
}

    ساكن char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = عدم;

    p = skipwhite(arg);
    إذا (VIM_ISDIGIT(*p))
	أرجع عدم;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    أرجع عدم;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    ساكن char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    صحيح    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    إذا (*p == NUL)
	أرجع عدم;

    لكل (;;)
    {
	إذا (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    استمر;
	}
	إذا (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    استمر;
	}
	اكسر;
    }

    إذا ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    وإلا إذا ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    وإلا إذا ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    أرجع عدم;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    ساكن فراغ
set_context_with_pattern(expand_T *xp)
{
    صحيح		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_چارين_EXTRA
    صحيح		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    إذا (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	أرجع;
#endif

    xp->xp_pattern = ccline->cmdbuff + skiplen;
    xp->xp_pattern_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_PATTERN_IN_BUF;
    xp->xp_چارين_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    ساكن char_u *
set_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	long		argt,
	صحيح		compl,
	صحيح		forceit)
{
    char_u  *nextcmd;

    بدل (cmdidx)
    {
	حالة CMD_find:
	حالة CMD_sfind:
	حالة CMD_tabfind:
	    إذا (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    اكسر;
	حالة CMD_cd:
	حالة CMD_chdir:
	حالة CMD_tcd:
	حالة CMD_tchdir:
	حالة CMD_lcd:
	حالة CMD_lchdir:
	    إذا (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    اكسر;
	حالة CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    اكسر;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	حالة CMD_aboveleft:
	حالة CMD_argdo:
	حالة CMD_belowright:
	حالة CMD_botright:
	حالة CMD_browse:
	حالة CMD_bufdo:
	حالة CMD_cdo:
	حالة CMD_cfdo:
	حالة CMD_confirm:
	حالة CMD_debug:
	حالة CMD_folddoclosed:
	حالة CMD_folddoopen:
	حالة CMD_hide:
	حالة CMD_horizontal:
	حالة CMD_keepalt:
	حالة CMD_keepjumps:
	حالة CMD_keepmarks:
	حالة CMD_keeppatterns:
	حالة CMD_ldo:
	حالة CMD_leftabove:
	حالة CMD_lfdo:
	حالة CMD_lockmarks:
	حالة CMD_noautocmd:
	حالة CMD_noswapfile:
	حالة CMD_rightbelow:
	حالة CMD_sandbox:
	حالة CMD_silent:
	حالة CMD_tab:
	حالة CMD_tabdo:
	حالة CMD_topleft:
	حالة CMD_unsilent:
	حالة CMD_verbose:
	حالة CMD_vertical:
	حالة CMD_windo:
	حالة CMD_vim9cmd:
	حالة CMD_legacy:
	    أرجع arg;

	حالة CMD_filter:
	    أرجع set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_چارين_EXTRA
	حالة CMD_match:
	    أرجع set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	حالة CMD_command:
	    أرجع set_context_in_user_cmd(xp, arg);

	حالة CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    اكسر;

	حالة CMD_global:
	حالة CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    إذا (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    أرجع nextcmd;

	حالة CMD_and:
	حالة CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    إذا (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    أرجع nextcmd;

	حالة CMD_isearch:
	حالة CMD_dsearch:
	حالة CMD_ilist:
	حالة CMD_dlist:
	حالة CMD_ijump:
	حالة CMD_psearch:
	حالة CMD_djump:
	حالة CMD_isplit:
	حالة CMD_dsplit:
	    أرجع find_cmd_after_isearch_cmd(xp, arg);
	حالة CMD_autocmd:
	    أرجع set_context_in_autocmd(xp, arg, FALSE);
	حالة CMD_doautocmd:
	حالة CMD_doautoall:
	    أرجع set_context_in_autocmd(xp, arg, TRUE);
	حالة CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    اكسر;
	حالة CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    اكسر;
	حالة CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    اكسر;
	حالة CMD_tag:
	حالة CMD_stag:
	حالة CMD_ptag:
	حالة CMD_ltag:
	حالة CMD_tselect:
	حالة CMD_stselect:
	حالة CMD_ptselect:
	حالة CMD_tjump:
	حالة CMD_stjump:
	حالة CMD_ptjump:
	    إذا (vim_strchr(p_wop, WOP_TAGFILE) != عدم)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    وإلا
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    اكسر;
	حالة CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    اكسر;
#ifdef FEAT_SYN_HL
	حالة CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    اكسر;
#endif
#ifdef FEAT_EVAL
	حالة CMD_final:
	حالة CMD_const:
	حالة CMD_let:
	حالة CMD_var:
	حالة CMD_if:
	حالة CMD_elseif:
	حالة CMD_while:
	حالة CMD_for:
	حالة CMD_echo:
	حالة CMD_echon:
	حالة CMD_execute:
	حالة CMD_echomsg:
	حالة CMD_echoerr:
	حالة CMD_echoconsole:
	حالة CMD_echowindow:
	حالة CMD_call:
	حالة CMD_return:
	حالة CMD_cexpr:
	حالة CMD_caddexpr:
	حالة CMD_cgetexpr:
	حالة CMD_lexpr:
	حالة CMD_laddexpr:
	حالة CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    اكسر;

	حالة CMD_unlet:
	    أرجع set_context_in_unlet_cmd(xp, arg);
	حالة CMD_function:
	حالة CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    اكسر;
	حالة CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    اكسر;

	حالة CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    اكسر;
#endif
	حالة CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    اكسر;
#ifdef FEAT_CSCOPE
	حالة CMD_cscope:
	حالة CMD_lcscope:
	حالة CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    اكسر;
#endif
#ifdef FEAT_SIGNS
	حالة CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    اكسر;
#endif
	حالة CMD_bdelete:
	حالة CMD_bwipeout:
	حالة CMD_bunload:
	    طالما ((xp->xp_pattern = vim_strchr(arg, ' ')) != عدم)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	حالة CMD_buffer:
	حالة CMD_sbuffer:
	حالة CMD_pbuffer:
	حالة CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    اكسر;
#ifdef FEAT_DIFF
	حالة CMD_diffget:
	حالة CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    اكسر;
#endif
	حالة CMD_USER:
	حالة CMD_USER_BUF:
	    أرجع set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	حالة CMD_map:	    حالة CMD_noremap:
	حالة CMD_nmap:	    حالة CMD_nnoremap:
	حالة CMD_vmap:	    حالة CMD_vnoremap:
	حالة CMD_omap:	    حالة CMD_onoremap:
	حالة CMD_imap:	    حالة CMD_inoremap:
	حالة CMD_cmap:	    حالة CMD_cnoremap:
	حالة CMD_lmap:	    حالة CMD_lnoremap:
	حالة CMD_smap:	    حالة CMD_snoremap:
	حالة CMD_tmap:	    حالة CMD_tnoremap:
	حالة CMD_xmap:	    حالة CMD_xnoremap:
	    أرجع set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	حالة CMD_unmap:
	حالة CMD_nunmap:
	حالة CMD_vunmap:
	حالة CMD_ounmap:
	حالة CMD_iunmap:
	حالة CMD_cunmap:
	حالة CMD_lunmap:
	حالة CMD_sunmap:
	حالة CMD_tunmap:
	حالة CMD_xunmap:
	    أرجع set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	حالة CMD_mapclear:
	حالة CMD_nmapclear:
	حالة CMD_vmapclear:
	حالة CMD_omapclear:
	حالة CMD_imapclear:
	حالة CMD_cmapclear:
	حالة CMD_lmapclear:
	حالة CMD_smapclear:
	حالة CMD_tmapclear:
	حالة CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    اكسر;

	حالة CMD_abbreviate:	حالة CMD_noreabbrev:
	حالة CMD_cabbrev:	حالة CMD_cnoreabbrev:
	حالة CMD_iabbrev:	حالة CMD_inoreabbrev:
	    أرجع set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	حالة CMD_unabbreviate:
	حالة CMD_cunabbrev:
	حالة CMD_iunabbrev:
	    أرجع set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	حالة CMD_menu:	    حالة CMD_noremenu:	    حالة CMD_unmenu:
	حالة CMD_amenu:	    حالة CMD_anoremenu:	    حالة CMD_aunmenu:
	حالة CMD_nmenu:	    حالة CMD_nnoremenu:	    حالة CMD_nunmenu:
	حالة CMD_vmenu:	    حالة CMD_vnoremenu:	    حالة CMD_vunmenu:
	حالة CMD_omenu:	    حالة CMD_onoremenu:	    حالة CMD_ounmenu:
	حالة CMD_imenu:	    حالة CMD_inoremenu:	    حالة CMD_iunmenu:
	حالة CMD_cmenu:	    حالة CMD_cnoremenu:	    حالة CMD_cunmenu:
	حالة CMD_tlmenu:    حالة CMD_tlnoremenu:    حالة CMD_tlunmenu:
	حالة CMD_tmenu:				    حالة CMD_tunmenu:
	حالة CMD_popup:	    حالة CMD_tearoff:	    حالة CMD_emenu:
	    أرجع set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	حالة CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    اكسر;

	حالة CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    اكسر;

	حالة CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    اكسر;

	حالة CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    اكسر;

	حالة CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    اكسر;

	حالة CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    اكسر;

#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	حالة CMD_language:
	    أرجع set_context_in_lang_cmd(xp, arg);
#endif
#إذا defined(FEAT_PROFILE)
	حالة CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    اكسر;
#endif
	حالة CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    اكسر;

	حالة CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    اكسر;

	حالة CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    اكسر;

	حالة CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    اكسر;
#إذا defined(FEAT_PROFILE)
	حالة CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    اكسر;
#endif

	حالة CMD_argdelete:
	    طالما ((xp->xp_pattern = vim_strchr(arg, ' ')) != عدم)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    اكسر;

#ifdef FEAT_EVAL
	حالة CMD_breakadd:
	حالة CMD_profdel:
	حالة CMD_breakdel:
	    أرجع set_context_in_breakadd_cmd(xp, arg, cmdidx);

	حالة CMD_scriptnames:
	    أرجع set_context_in_scriptnames_cmd(xp, arg);
#endif
	حالة CMD_filetype:
	    أرجع set_context_in_filetype_cmd(xp, arg);

	افتراضي:
	    اكسر;
    }
    أرجع عدم;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    ساكن char_u *
set_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    صحيح			len = 0;
    exarg_T		ea;
    صحيح			compl = EXPAND_NOTHING;
    صحيح			forceit = FALSE;
    صحيح			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_باريس = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    لكل (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != عدم; cmd++)
	;
    xp->xp_pattern = cmd;

    إذا (*cmd == NUL)
	أرجع عدم;
    إذا (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	أرجع عدم;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    إذا (*cmd == NUL)
	أرجع عدم;
    إذا (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	أرجع عدم;
    }

    إذا (*cmd == '|' || *cmd == '\n')
	أرجع cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    إذا (p == عدم)
	أرجع عدم;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    إذا (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    إذا (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    إذا ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	طالما (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    طالما (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    إذا (*p == NUL)
	    {
		إذا (ea.argt & EX_ARGOPT)
		    أرجع set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		إذا (ea.cmdidx == CMD_terminal)
		    أرجع set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    إذا (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	إذا (*arg == '>')			// append
	{
	    إذا (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	وإلا إذا (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    إذا (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	إذا (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    إذا (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	طالما (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    إذا ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	إذا (*arg == NUL)
	    أرجع p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    إذا ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	إذا (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	طالما (*p)
	{
	    إذا (*p == Ctrl_V)
	    {
		إذا (p[1] != NUL)
		    ++p;
	    }
	    وإلا إذا ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		إذا (*(p - 1) != '\\')
		{
		    إذا (*p == '|' || *p == '\n')
			أرجع p + 1;
		    أرجع عدم;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    إذا (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == عدم)
	// no arguments allowed but there is something
	أرجع عدم;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (صحيح)STRLEN(buff);
    طالما (*p && p < buff + len)
    {
	إذا (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	وإلا
	{
	    إذا (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    إذا (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    أرجع set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    فراغ
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    صحيح		len,	    // length of command line (excl. NUL)
    صحيح		col,	    // position of cursor
    صحيح		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    صحيح			context;
#endif
    صحيح		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    إذا (col < len)
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
    إذا (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    وإلا إذا (use_ccline && ccline->ماسوقن_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	إذا (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(عدم, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    وإلا
#endif
	طالما (nextcomm != عدم)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_باريس = str;
    xp->xp_col = col;

    إذا (col < len)
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
    صحيح
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    صحيح		col,		// position of cursor
    صحيح		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = عدم;
    صحيح		options = WILD_ADD_SLASH|WILD_SILENT;

    إذا (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	أرجع EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    إذا (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	أرجع EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (صحيح)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	صحيح	pat_len = xp->xp_pattern_len;
#إذا defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	إذا (unesc != عدم)
	    pat_src = unesc;
#endif

	إذا (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	وإلا
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#إذا defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    إذا (file_str == عدم)
	أرجع EXPAND_UNSUCCESSFUL;

    إذا (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    إذا (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = عدم;
    }
    vim_free(file_str);

    أرجع EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    ساكن صحيح
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	صحيح		*اڠک_Matches,
	صحيح		flags,
	صحيح		options)
{
    صحيح		free_pat = FALSE;
    صحيح		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    إذا (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	إذا (pat == عدم)
	    أرجع ret;

	pat_end = pat + pat_len;
	لكل (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    إذا (*p != '\\')
		استمر;

	    إذا (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    وإلا إذا (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    وإلا إذا (xp->xp_backslash & XP_BS_COMMA)
	    {
		إذا (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		وإلا إذا (*(p + 1) == ',')
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

    إذا (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, اڠک_Matches);
#endif
    }
    وإلا
    {
	إذا (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	وإلا إذا (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	وإلا إذا (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	وإلا
	    flags = (flags | EW_DIR) & ~EW_FILE;
	إذا (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, اڠک_Matches, matches, flags);
    }
    إذا (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    إذا (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	صحيح j;

	لكل (j = 0; j < *اڠک_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    طالما (*ptr != NUL)
	    {
		إذا (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		وإلا إذا (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    أرجع ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    ساكن char_u *
get_behave_arg(expand_T *xp UNUSED, صحيح idx)
{
    إذا (idx == 0)
	أرجع (char_u *)"mswin";
    إذا (idx == 1)
	أرجع (char_u *)"xterm";
    أرجع عدم;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    ساكن char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, صحيح idx)
{
    إذا (idx < 0)
	أرجع عدم;

    إذا (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	حرف	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	أرجع (char_u *)opts_all[idx];
    }
    إذا (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	حرف	*opts_plugin[] = {"plugin", "on", "off"};
	أرجع (char_u *)opts_plugin[idx];
    }
    إذا (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	حرف	*opts_indent[] = {"indent", "on", "off"};
	أرجع (char_u *)opts_indent[idx];
    }
    إذا (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	حرف	*opts_onoff[] = {"on", "off"};
	أرجع (char_u *)opts_onoff[idx];
    }
    أرجع عدم;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    ساكن char_u *
get_breakadd_arg(expand_T *xp UNUSED, صحيح idx)
{
    إذا (idx >= 0 && idx <= 3)
    {
	حرف	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	إذا (breakpt_expand_what == EXP_BREAKPT_ADD)
	    أرجع (char_u *)opts[idx];
	وإلا إذا (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    إذا (idx <= 2)
		أرجع (char_u *)opts[idx + 1];
	}
	وإلا
	{
	    // profdel {func, file}
	    إذا (idx <= 1)
		أرجع (char_u *)opts[idx + 1];
	}
    }
    أرجع عدم;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    ساكن char_u *
get_scriptnames_arg(expand_T *xp UNUSED, صحيح idx)
{
    scriptitem_T *si;

    إذا (!SCRIPT_ID_VALID(idx + 1))
	أرجع عدم;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(عدم, si->sn_name, NameBuff, MAXPATHL, TRUE);
    أرجع NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    ساكن char_u *
get_retab_arg(expand_T *xp UNUSED, صحيح idx)
{
    إذا (idx == 0)
	أرجع (char_u *)"-indentonly";
    أرجع عدم;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    ساكن char_u *
get_messages_arg(expand_T *xp UNUSED, صحيح idx)
{
    إذا (idx == 0)
	أرجع (char_u *)"clear";
    أرجع عدم;
}

    ساكن char_u *
get_mapclear_arg(expand_T *xp UNUSED, صحيح idx)
{
    إذا (idx == 0)
	أرجع (char_u *)"<buffer>";
    أرجع عدم;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    ساكن صحيح
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	صحيح		*اڠک_Matches)
{
    ساكن بنية expgen
    {
	صحيح		context;
	char_u	*((*func)(expand_T *, صحيح));
	صحيح		ic;
	صحيح		escaped;
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
	{EXPAND_EXPRESSION, get_اوڠکاڤن_name, FALSE, TRUE},
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
#إذا defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
    صحيح	i;
    صحيح ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    لكل (i = 0; i < (صحيح)ARRAY_LENGTH(tab); ++i)
    {
	إذا (xp->xp_context == tab[i].context)
	{
	    إذا (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, اڠک_Matches,
						tab[i].func, tab[i].escaped);
	    اكسر;
	}
    }

    أرجع ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    ساكن صحيح
map_wildopts_to_ewflags(صحيح options)
{
    صحيح		flags;

    flags = EW_DIR;	// include directories
    إذا (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    إذا (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    إذا (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    إذا (options & WILD_SILENT)
	flags |= EW_SILENT;
    إذا (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    إذا (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    أرجع flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    ساكن صحيح
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    صحيح		*اڠک_Matches,
    صحيح		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    صحيح		ret;
    صحيح		flags;
    char_u	*tofree = عدم;
    صحيح		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    إذا (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	أرجع expand_files_and_dirs(xp, pat, matches, اڠک_Matches, flags,
								options);

    *matches = (char_u **)"";
    *اڠک_Matches = 0;
    إذا (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	إذا (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					اڠک_Matches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*اڠک_Matches, *matches);
#endif
	    أرجع OK;
	}
	أرجع FAIL;
    }

    إذا (xp->xp_context == EXPAND_SHELLCMD)
	أرجع expand_shellcmd(pat, matches, اڠک_Matches, flags);
    إذا (xp->xp_context == EXPAND_OLD_SETTING)
	أرجع ExpandOldSetting(اڠک_Matches, matches);
    إذا (xp->xp_context == EXPAND_BUFFERS)
	أرجع ExpandBufnames(pat, اڠک_Matches, matches, options);
#ifdef FEAT_DIFF
    إذا (xp->xp_context == EXPAND_DIFF_BUFFERS)
	أرجع ExpandBufnames(pat, اڠک_Matches, matches,
						options | BUF_DIFF_FILTER);
#endif
    إذا (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	أرجع expand_tags(xp->xp_context == EXPAND_TAGS, pat, اڠک_Matches,
								matches);
    إذا (xp->xp_context == EXPAND_COLORS)
    {
	حرف *directories[] = {"colors", عدم};
	أرجع ExpandRTDir(pat, DIP_START + DIP_OPT, اڠک_Matches, matches,
								directories);
    }
    إذا (xp->xp_context == EXPAND_COMPILER)
    {
	حرف *directories[] = {"compiler", عدم};
	أرجع ExpandRTDir(pat, 0, اڠک_Matches, matches, directories);
    }
    إذا (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	حرف *directories[] = {"syntax", عدم};
	أرجع ExpandRTDir(pat, 0, اڠک_Matches, matches, directories);
    }
    إذا (xp->xp_context == EXPAND_FILETYPE)
    {
	حرف *directories[] = {"syntax", "indent", "ftplugin", عدم};
	أرجع ExpandRTDir(pat, 0, اڠک_Matches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    إذا (xp->xp_context == EXPAND_KEYMAP)
    {
	حرف *directories[] = {"keymap", عدم};
	أرجع ExpandRTDir(pat, 0, اڠک_Matches, matches, directories);
    }
#endif
#إذا defined(FEAT_EVAL)
    إذا (xp->xp_context == EXPAND_USER_LIST)
	أرجع ExpandUserList(xp, matches, اڠک_Matches);
#endif
    إذا (xp->xp_context == EXPAND_PACKADD)
	أرجع ExpandPackAddDir(pat, اڠک_Matches, matches);
    إذا (xp->xp_context == EXPAND_RUNTIME)
	أرجع expand_runtime_cmd(pat, اڠک_Matches, matches);
    إذا (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	أرجع expand_pattern_in_buf(pat, xp->xp_چارين_dir,
		matches, اڠک_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    إذا ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	صحيح len = (صحيح)STRLEN(pat) + 20;

	tofree = alloc(len);
	إذا (tofree == عدم)
	    أرجع FAIL;
	vim_snprintf((حرف *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    إذا (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	إذا (regmatch.regprog == عدم)
	{
	    vim_free(tofree);
	    أرجع FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    إذا (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, اڠک_Matches, matches, fuzzy);
    وإلا إذا (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, اڠک_Matches, matches);
    وإلا إذا (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, اڠک_Matches, matches);
    وإلا إذا (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, اڠک_Matches, matches);
    وإلا إذا (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, اڠک_Matches);
    وإلا إذا (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, اڠک_Matches);
#إذا defined(FEAT_TERMINAL)
    وإلا إذا (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, اڠک_Matches);
#endif
#إذا defined(FEAT_EVAL)
    وإلا إذا (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, اڠک_Matches);
#endif
    وإلا
	ret = ExpandOther(pat, xp, &regmatch, matches, اڠک_Matches);

    إذا (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    أرجع ret;
}

    صحيح
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    صحيح		*اڠک_Matches,
    char_u	*(*func)(expand_T *, صحيح),
					  // returns a string from the list
    صحيح		escaped)
{
    أرجع ExpandGenericExt(
	pat, xp, regmatch, matches, اڠک_Matches, func, escaped, 0);
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
    صحيح
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    صحيح		*اڠک_Matches,
    char_u	*(*func)(expand_T *, صحيح),
					  // returns a string from the list
    صحيح		escaped,
    صحيح		sortStartIdx)
{
    صحيح		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = عدم;
    صحيح		score = 0;
    صحيح		fuzzy;
    صحيح		match;
    صحيح		sort_matches = FALSE;
    صحيح		funcsort = FALSE;
    صحيح		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = عدم;
    *اڠک_Matches = 0;

    إذا (!fuzzy)
	ga_init2(&ga, حجم(حرف *), 30);
    وإلا
	ga_init2(&ga, حجم(fuzmatch_str_T), 30);

    لكل (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	إذا (str == عدم)	    // end of list
	    اكسر;
	إذا (*str == NUL)	    // skip empty strings
	    استمر;

	إذا (xp->xp_pattern[0] != NUL)
	{
	    إذا (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    وإلا
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	وإلا
	    match = TRUE;

	إذا (!match)
	    استمر;

	إذا (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	وإلا
	    str = vim_strsave(str);
	إذا (str == عدم)
	{
	    إذا (!fuzzy)
	    {
		ga_clear_strings(&ga);
		أرجع FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    أرجع FAIL;
	}

	إذا (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    اكسر;
	}

	إذا (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	وإلا
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

#ifdef FEAT_MENU
	إذا (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    إذا (*str == '\001')
		*str = '.';
	}
#endif

	إذا (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    إذا (ga.ga_len == 0)
	أرجع OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    إذا (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    إذا (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    إذا (sort_matches && sortStartMatchIdx != -1)
    {
	إذا (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((فراغ *)ga.ga_data, (size_t)ga.ga_len, حجم(char_u *),
							   sort_func_compare);
	وإلا
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    إذا (!fuzzy)
    {
	*matches = ga.ga_data;
	*اڠک_Matches = ga.ga_len;
    }
    وإلا
    {
	إذا (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    أرجع FAIL;
	*اڠک_Matches = ga.ga_len;
    }

#إذا defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    أرجع OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    ساكن فراغ
expand_shellcmd_onedir(
	char_u		*pathed_pattern,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	صحيح		*اڠک_Matches,
	صحيح		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    إذا (expand_wildcards(1, &pathed_pattern, اڠک_Matches, matches, flags) != OK)
	أرجع;

    إذا (ga_grow(gap, *اڠک_Matches) == FAIL)
    {
	FreeWild(*اڠک_Matches, *matches);
	أرجع;
    }

    لكل (صحيح i = 0; i < *اڠک_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	إذا (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    إذا (HASHITEM_EMPTY(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_data)[gap->ga_len++] = name;
		hash_add_item(ht, hi, name, hash);
		name = عدم;
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
    ساكن صحيح
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    صحيح		*اڠک_Matches,	// return: number of matches
    صحيح		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = عدم;
    صحيح		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    صحيح		flags = flagsarg;
    صحيح		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    إذا (buf == عدم)
	أرجع FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    إذا (pat == عدم)
    {
	vim_free(buf);
	أرجع FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    لكل (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	إذا (*s != '\\')
	    استمر;

	p = s + 1;
	إذا (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    إذا (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    وإلا
    {
	// For an absolute name we don't use $PATH.
	إذا (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	إذا (path == عدم)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, حجم(حرف *), 10);
    hash_init(&found_ht);
    لكل (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	إذا (*s == NUL)
	{
	    إذا (did_curdir)
		اكسر;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	وإلا
	{
#إذا defined(MSWIN)
	    e = vim_strchr(s, ';');
#وإلا
	    e = vim_strchr(s, ':');
#endif
	    إذا (e == عدم)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    إذا (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    وإلا
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? حجم(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	إذا (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    إذا (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		إذا (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, اڠک_Matches, flags,
				    &found_ht, &ga);
	}

	إذا (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_data;
    *اڠک_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    إذا (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    أرجع OK;
}

#إذا defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    ساكن فراغ *
call_user_expand_func(
    فراغ	*(*user_expand_func)(char_u *, صحيح, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    صحيح		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = عدم;
    فراغ	*ret;

    إذا (xp->xp_arg == عدم || xp->xp_arg[0] == '\0' || xp->xp_باريس == عدم)
	أرجع عدم;

    إذا (ccline->cmdbuff != عدم)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	صحيح unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	إذا (pat == عدم)
	    pat = vim_strnsave(xp->xp_pattern, xp->xp_pattern_len);
    }

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = xp->xp_باريس;
    args[2].v_type = VAR_نومبور;
    args[2].vval.v_نومبور = xp->xp_col;
    args[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, args);

    current_sctx = save_current_sctx;
    إذا (ccline->cmdbuff != عدم)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    أرجع ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    ساكن صحيح
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    صحيح		*اڠک_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    صحيح		keep;
    garray_T	ga;
    صحيح		fuzzy;
    صحيح		match;
    صحيح		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = عدم;
    *اڠک_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    إذا (retstr == عدم)
	أرجع FAIL;

    إذا (!fuzzy)
	ga_init2(&ga, حجم(حرف *), 3);
    وإلا
	ga_init2(&ga, حجم(fuzmatch_str_T), 3);

    لكل (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	إذا (e == عدم)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	إذا (xp->xp_pattern[0] != NUL)
	{
	    إذا (!fuzzy)
		match = vim_regexec(regmatch, s, (colnr_T)0);
	    وإلا
	    {
		score = fuzzy_match_str(s, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	وإلا
	    match = TRUE;		// match everything

	*e = keep;

	إذا (match)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    إذا (p == عدم)
		اكسر;

	    إذا (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		اكسر;
	    }

	    إذا (!fuzzy)
		((char_u **)ga.ga_data)[ga.ga_len] = p;
	    وإلا
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	إذا (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    إذا (ga.ga_len == 0)
	أرجع OK;

    إذا (!fuzzy)
    {
	*matches = ga.ga_data;
	*اڠک_Matches = ga.ga_len;
    }
    وإلا
    {
	إذا (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    أرجع FAIL;
	*اڠک_Matches = ga.ga_len;
    }
    أرجع OK;
}

    فراغ
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    صحيح		*اڠک_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    صحيح		have_extra = FALSE;
    صحيح		i;

    ga_init2(&ga, حجم(حرف *), 3);
    ga_init2(&ga_abbr, حجم(حرف *), 3);
    ga_init2(&ga_kind, حجم(حرف *), 3);
    ga_init2(&ga_menu, حجم(حرف *), 3);
    ga_init2(&ga_info, حجم(حرف *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_بارڠن(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = عدم;
	char_u	*abbr = عدم;
	char_u	*kind = عدم;
	char_u	*menu = عدم;
	char_u	*info = عدم;

	إذا (tv->v_type == VAR_STRING)
	{
	    إذا (tv->vval.v_string == عدم)
		استمر;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	وإلا إذا (tv->v_type == VAR_DICT && tv->vval.v_dict != عدم)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    إذا (word == عدم)
		استمر;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    إذا (abbr != عدم || kind != عدم || menu != عدم || info != عدم)
		have_extra = TRUE;
	}
	وإلا
	    استمر;  // Skip other types

	إذا (p == عدم
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
	    اكسر;
	}

	((char_u **)ga.ga_data)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_data)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_data)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_data)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_data)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_data;
    *اڠک_Matches = ga.ga_len;
    إذا (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    وإلا
    {
	// No extra info collected; free the placeholder NULL entries.
	لكل (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	لكل (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	لكل (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	لكل (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    ساكن صحيح
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    صحيح		*اڠک_Matches)
{
    list_T      *retlist;

    *matches = عدم;
    *اڠک_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    إذا (retlist == عدم)
	أرجع FAIL;

    expand_process_user_list(retlist, matches, اڠک_Matches, xp);
    list_unref(retlist);
    أرجع OK;
}
#endif

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    فراغ
globpath(
    char_u	*path,
    char_u	*file,
    garray_T	*ga,
    صحيح		expand_options,
    صحيح		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    إذا (buf == عدم)
	أرجع;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#إذا defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#وإلا
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    طالما (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	إذا (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    صحيح	    اڠک_p;
	    char_u  **p;

	    إذا (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    إذا (ExpandFromContext(&xpc, buf, &p, &اڠک_p,
			     WILD_SILENT|expand_options) != FAIL && اڠک_p > 0)
	    {
		ExpandEscape(&xpc, buf, اڠک_p, p, WILD_SILENT|expand_options);

		إذا (ga_grow(ga, اڠک_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    لكل (صحيح i = 0; i < اڠک_p; ++i)
		    {
			((char_u **)ga->ga_data)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		وإلا
		{
		    FreeWild(اڠک_p, p);
		    p = عدم;
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
    صحيح
wildmenu_translate_کونچي(
	cmdline_info_T	*cclp,
	صحيح		کونچي,
	expand_T	*xp,
	صحيح		did_wild_list)
{
    صحيح c = کونچي;

    إذا (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	بدل (c)
	{
	    حالة K_UP:	  c = K_کيري; اكسر;
	    حالة K_DOWN:  c = K_کانن; اكسر;
	    حالة K_کيري:  c = K_UP; اكسر;
	    حالة K_کانن: c = K_DOWN; اكسر;
	    افتراضي:	  اكسر;
	}
    }

    إذا (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	إذا (c == K_کيري)
	    c = Ctrl_P;
	وإلا إذا (c == K_کانن)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    إذا (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    أرجع c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    ساكن فراغ
cmdline_del(cmdline_info_T *cclp, صحيح from)
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
    ساكن صحيح
wildmenu_process_کونچي_menunames(cmdline_info_T *cclp, صحيح کونچي, expand_T *xp)
{
    صحيح		i;
    صحيح		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    إذا (کونچي == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	کونچي = p_wc;
	کونچي_Typed = TRUE;  // in case the key was mapped
    }
    وإلا إذا (کونچي == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	صحيح found = FALSE;

	j = (صحيح)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	طالما (--j > 0)
	{
	    // check for start of menu name
	    إذا (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		اكسر;
	    }
	    // check for start of submenu name
	    إذا (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		إذا (found)
		{
		    i = j + 1;
		    اكسر;
		}
		وإلا
		    found = TRUE;
	    }
	}
	إذا (i > 0)
	    cmdline_del(cclp, i);
	کونچي = p_wc;
	کونچي_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    أرجع کونچي;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    ساكن صحيح
wildmenu_process_کونچي_filenames(cmdline_info_T *cclp, صحيح کونچي, expand_T *xp)
{
    صحيح		i;
    صحيح		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    إذا (کونچي == K_DOWN
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	کونچي = p_wc;
	کونچي_Typed = TRUE;  // in case the key was mapped
    }
    وإلا إذا (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && کونچي == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	صحيح found = FALSE;

	j = cclp->cmdpos;
	i = (صحيح)(xp->xp_pattern - cclp->cmdbuff);
	طالما (--j > i)
	{
	    إذا (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    إذا (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		اكسر;
	    }
	}
	إذا (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    کونچي = p_wc;
	    کونچي_Typed = TRUE;  // in case the key was mapped
	}
    }
    وإلا إذا (کونچي == K_UP)
    {
	// go up a directory
	صحيح found = FALSE;

	j = cclp->cmdpos - 1;
	i = (صحيح)(xp->xp_pattern - cclp->cmdbuff);
	طالما (--j > i)
	{
	    إذا (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    إذا (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == عدم
#endif
	       )
	    {
		إذا (found)
		{
		    i = j + 1;
		    اكسر;
		}
		وإلا
		    found = TRUE;
	    }
	}

	إذا (!found)
	    j = i;
	وإلا إذا (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	وإلا إذا (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	وإلا
	    j = 0;
	إذا (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	وإلا إذا (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	کونچي = p_wc;
	کونچي_Typed = TRUE;
    }

    أرجع کونچي;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    صحيح
wildmenu_process_کونچي(cmdline_info_T *cclp, صحيح کونچي, expand_T *xp)
{
    إذا (xp->xp_context == EXPAND_MENUNAMES)
	أرجع wildmenu_process_کونچي_menunames(cclp, کونچي, xp);
    وإلا إذا ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	أرجع wildmenu_process_کونچي_filenames(cclp, کونچي, xp);

    أرجع کونچي;
}

/*
 * Free expanded names when finished walking through the matches
 */
    فراغ
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    صحيح skt = کونچي_Typed;

    إذا (!p_wmnu || wild_menu_showing == 0)
	أرجع;

#ifdef FEAT_EVAL
    صحيح save_RedrawingDisabled = RedrawingDisabled;
    إذا (cclp->ماسوقن_fn)
	RedrawingDisabled = 0;
#endif

#إذا defined(FEAT_چارين_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    إذا (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    وإلا إذا (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	last_status(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    وإلا
    {
	win_redraw_last_status(topframe);
	redraw_statuslines();
    }
    کونچي_Typed = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    إذا (cclp->ماسوقن_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#إذا defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    فراغ
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    صحيح		filtered = FALSE;
    صحيح		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    إذا (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	أرجع;

    pat = tv_get_string(&argvars[0]);
    إذا (check_for_string_arg(argvars, 1) == FAIL)
	أرجع;
    type = tv_get_string(&argvars[1]);

    إذا (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], عدم);

    إذا (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    إذا (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    إذا (STRCMP(type, "cmdline") == 0)
    {
	صحيح cmdline_len = (صحيح)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (صحيح)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    وإلا
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (صحيح)STRLEN(xpc.xp_pattern);
	xpc.xp_باريس = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	بدل (xpc.xp_context)
	{
	حالة EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    أرجع;

	حالة EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    إذا (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		أرجع;
	    }

	    xpc.xp_arg = type + 7;
	    اكسر;

	حالة EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    إذا (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		أرجع;
	    }

	    xpc.xp_arg = type + 11;
	    اكسر;

# إذا defined(FEAT_MENU)
	حالة EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (صحيح)(xpc.xp_pattern - pattern_start);
	    اكسر;
# endif

# ifdef FEAT_CSCOPE
	حالة EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (صحيح)(xpc.xp_pattern - pattern_start);
	    اكسر;
# endif

# ifdef FEAT_SIGNS
	حالة EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (صحيح)(xpc.xp_pattern - pattern_start);
	    اكسر;
# endif

	حالة EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (صحيح)(xpc.xp_pattern - pattern_start);
	    اكسر;

	حالة EXPAND_SHELLCMDLINE:
	{
	    صحيح context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(عدم, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (صحيح)(xpc.xp_pattern - pattern_start);
	    اكسر;
	}

	حالة EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    اكسر;

	حالة EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    اكسر;

	افتراضي:
	    اكسر;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	صحيح	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	إذا (unesc != عدم)
	    pat_src = unesc;

	إذا (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	وإلا
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    إذا (rettv_list_alloc(rettv) == OK && pat != عدم)
    {
	صحيح	i;

	ExpandOne(&xpc, pat, عدم, options, WILD_ALL_KEEP);

	لكل (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    فراغ
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    صحيح		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (صحيح)STRLEN(pat);
    set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_to_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    فراغ
f_cmdcomplete_info(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    صحيح		    idx;
    صحيح		    ret = OK;

    إذا (rettv_dict_alloc(rettv) == FAIL || ccline == عدم
	    || ccline->xpc == عدم || ccline->xpc->xp_files == عدم)
	أرجع;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (صحيح)cmdline_orig.length);
    إذا (ret == OK)
	ret = dict_add_نومبور(retdict, "pum_visible", pum_visible());
    إذا (ret == OK)
	ret = dict_add_نومبور(retdict, "selected", ccline->xpc->xp_selected);
    إذا (ret == OK)
    {
	li = list_alloc();
	إذا (li == عدم)
	    أرجع;
	ret = dict_add_list(retdict, "matches", li);
	إذا (ret == FAIL)
	    list_unref(li);
	لكل (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    ساكن صحيح
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*باريس, *start_باريس, *end_باريس;
    صحيح		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    صحيح		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != عدم;

    إذا (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	أرجع FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_باريس = ml_get(start->lnum);
    char_u  *start_ptr = start_باريس + start->col;
    صحيح	    is_single_باريس = start->lnum == end->lnum;

    segment_len = is_single_باريس ? (end->col - start->col)
			: (صحيح)(ml_get_len(start->lnum) - start->col);
    إذا (ga_grow(&ga, segment_len + 2) != OK)
	اذهب fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    إذا (!is_single_باريس)
    {
	إذا (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	وإلا
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    إذا (!is_single_باريس)
    {
	لكل (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    صحيح  linelen;

	    باريس = ml_get(lnum);
	    linelen = (صحيح)ml_get_len(lnum);
	    إذا (ga_grow(&ga, linelen + 2) != OK)
		اذهب fail;
	    ga_concat_len(&ga, باريس, linelen);
	    إذا (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    وإلا
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_باريس = ml_get(end->lnum);
    word_end = find_word_end(end_باريس + end->col);
    segment_len = (صحيح)(word_end - end_باريس);
    إذا (ga_grow(&ga, segment_len) != OK)
	اذهب fail;
    ga_concat_len(&ga, end_باريس + (is_single_باريس ? end->col : 0),
	    segment_len - (is_single_باريس ? end->col : 0));

    // Null-terminate
    إذا (ga_grow(&ga, 1) != OK)
	اذهب fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    أرجع OK;

fail:
    ga_clear(&ga);
    أرجع FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    ساكن صحيح
is_regex_match(char_u *pat, char_u *str)
{
    إذا (STRCMP(pat, str) == 0)
	أرجع TRUE;

    regmatch_T	regmatch;
    صحيح		result;

    ++emsg_off;
    ++ڤسنن_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --ڤسنن_silent;

    إذا (regmatch.regprog == عدم)
	أرجع FALSE;
    regmatch.rm_ic = p_ic;
    إذا (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++ڤسنن_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --ڤسنن_silent;

    vim_regfree(regmatch.regprog);
    أرجع result;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    ساكن char_u *
concat_pattern_with_buffer_match(
	char_u *pat,
	صحيح pat_len,
	pos_T *end_match_pos,
	صحيح lowercase UNUSED)
{
    char_u  *باريس = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(باريس + end_match_pos->col);
    صحيح	    match_len = (صحيح)(word_end - (باريس + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    إذا (match == عدم)
	أرجع عدم;
    mch_memmove(match, pat, pat_len);
    إذا (match_len > 0)
    {
#إذا defined(FEAT_EVAL) || defined(FEAT_SPELL)
	إذا (lowercase)
	{
	    char_u  *mword = vim_strnsave(باريس + end_match_pos->col,
		    match_len);
	    إذا (mword == عدم)
		اذهب cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    إذا (lower == عدم)
		اذهب cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	وإلا
#endif
	    mch_memmove(match + pat_len, باريس + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    أرجع match;

#إذا defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    أرجع عدم;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    ساكن صحيح
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    صحيح		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    صحيح		*اڠک_Matches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    صحيح		found_new_match;
    صحيح		looped_around = FALSE;
    صحيح		pat_len;
    صحيح		has_range = FALSE;
    صحيح		compl_started = FALSE;
    صحيح		چارين_flags;
    char_u	*match, *full_match;
    صحيح		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != عدم;

#ifdef FEAT_چارين_EXTRA
    has_range = چارين_first_باريس != 0;
#endif

    *matches = عدم;
    *اڠک_Matches = 0;

    إذا (pat == عدم || *pat == NUL)
	أرجع FAIL;

    pat_len = (صحيح)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_چارين_EXTRA
    إذا (has_range)
	cur_match_pos.lnum = چارين_first_باريس;
    وإلا
#endif
	cur_match_pos = pre_incsearch_pos;

    چارين_flags = چارين_OPT | چارين_NOOF | چارين_PEEK | چارين_NFMSG
	| (has_range ? چارين_START : 0);

    ga_init2(&ga, حجم(char_u *), 10); // Use growable array of char_u*

    لكل (;;)
    {
	++emsg_off;
	++ڤسنن_silent;
	found_new_match = searchit(عدم, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		چارين_flags, RE_LAST, عدم);
	--ڤسنن_silent;
	--emsg_off;

	إذا (found_new_match == FAIL)
	    اكسر;

#ifdef FEAT_چارين_EXTRA
	// If in range mode, check if match is within the range
	إذا (has_range && (cur_match_pos.lnum < چارين_first_باريس
		    || cur_match_pos.lnum > چارين_last_باريس))
		اكسر;
#endif

	إذا (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    إذا ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		إذا (looped_around)
		    اكسر;
		وإلا
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	إذا (char_avail() || got_int)
	{
	    إذا (got_int)
	    {
		(فراغ)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    اذهب cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	إذا (end_match_pos.lnum > curbuf->b_ml.ml_باريس_کيراءن)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    استمر;
	}

	// Extract the matching text prepended to completed word
	إذا (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    اكسر;

	إذا (exacttext)
	    match = full_match;
	وإلا
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    إذا (match == عدم || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		إذا (match == عدم || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    استمر;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	لكل (صحيح i = 0; i < ga.ga_len; ++i)
	{
	    إذا (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		اكسر;
	    }
	}
	إذا (match != عدم)
	{
	    إذا (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		اذهب cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    إذا (ga.ga_len > TAG_MANY)
		اكسر;
	}
	إذا (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *اڠک_Matches = ga.ga_len;
    أرجع OK;

cleanup:
    ga_clear_strings(&ga);
    أرجع FAIL;
}
