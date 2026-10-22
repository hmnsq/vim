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

#include "vim.h"

static 整数	cmd_showtail;	// Only show path tail in lists ?
static 整数	may_expand_pattern = FALSE;
static pos_T	pre_incsearch_pos; // Cursor position when incsearch started

static void	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, 整数 usefilter, expand_T *xp, 整数 *complp);
static 整数	ExpandFromContext(expand_T *xp, char_u *, char_u ***, 整数 *, 整数);
static char_u	*showmatches_gettail(char_u *s);
static 整数	expand_showtail(expand_T *xp);
static 整数	expand_shellcmd(char_u *filepat, char_u ***matches, 整数 *numMatches, 整数 flagsarg);
#もし defined(FEAT_EVAL)
static 整数	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, 整数 *numMatches);
static 整数	ExpandUserList(expand_T *xp, char_u ***matches, 整数 *numMatches);
static char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
static char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, 整数 srclen, 整数 *new_lenp);
#endif
static 整数	expand_pattern_in_buf(char_u *pat, 整数 dir, char_u ***matches, 整数 *numMatches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
static pumitem_T *compl_match_array = NULL;
static 整数 compl_match_arraysize;
// First column in cmdline of the matched item for completion.
static 整数 compl_startcol;
static 整数 compl_selected;
// cmdline before expansion
static string_T cmdline_orig = {NULL, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    static 整数
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    switch (xp->xp_context)
    {
    case EXPAND_BOOL_SETTINGS:
    case EXPAND_COLORS:
    case EXPAND_COMPILER:
    case EXPAND_DIRECTORIES:
    case EXPAND_DIRS_IN_CDPATH:
    case EXPAND_FILES:
    case EXPAND_FILES_IN_PATH:
    case EXPAND_FILETYPE:
    case EXPAND_FILETYPECMD:
    case EXPAND_HELP:
    case EXPAND_KEYMAP:
    case EXPAND_OLD_SETTING:
    case EXPAND_STRING_SETTING:
    case EXPAND_SETTING_SUBTRACT:
    case EXPAND_OWNSYNTAX:
    case EXPAND_PACKADD:
    case EXPAND_RUNTIME:
    case EXPAND_SHELLCMD:
    case EXPAND_SHELLCMDLINE:
    case EXPAND_TAGS:
    case EXPAND_TAGS_LISTFILES:
	戻る FALSE;

    default:
	break;
    }

    戻る vim_strchr(p_wop, WOP_FUZZY) != NULL;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    整数
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    戻る vim_strchr(p_wop, WOP_FUZZY) != NULL && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    static 整数
sort_func_compare(const void *s1, const void *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    もし (*p1 != '<' && *p2 == '<') 戻る -1;
    もし (*p1 == '<' && *p2 != '<') 戻る 1;
    戻る STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    static void
wildescape(
    expand_T	*xp,
    char_u	*str,
    整数		numfiles,
    char_u	**files)
{
    char_u	*p;
    整数		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    もし (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	ために (整数 i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    もし (xp->xp_backslash & XP_BS_THREE)
	    {
		char *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		もし (p != NULL)
		{
		    vim_free(files[i]);
		    files[i] = p;
#もし defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    もし (p != NULL)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    否则 もし (xp->xp_backslash & XP_BS_COMMA)
	    {
		もし (vim_strchr(files[i], ',') != NULL)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    もし (p != NULL)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ifdef BACKSLASH_IN_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#否则
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#endif
	    もし (p != NULL)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    もし (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	もし (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    否则 もし (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	ために (整数 i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    もし (p != NULL)
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
    static void
ExpandEscape(
    expand_T	*xp,
    char_u	*str,
    整数		numfiles,
    char_u	**files,
    整数		options)
{
    // May change home directory back to "~"
    もし (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    もし (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    整数
nextwild(
    expand_T	*xp,
    整数		type,
    整数		options,	// extra options for ExpandOne()
    整数		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    整数		i;
    char_u	*p;
    整数		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    整数		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    もし (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	もし (ccline->input_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	否则
#endif
	{
	    may_expand_pattern = options & WILD_MAY_EXPAND_PATTERN;
	    set_expand_context(xp);
	    may_expand_pattern = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    もし (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	戻る OK;  // Something illegal on command line
    }
    もし (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	戻る FAIL;
    }

    i = (整数)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    もし (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	戻る FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    もし (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	msg_puts("...");	    // show that we are busy
	out_flush();
    }

    もし (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, NULL, NULL, 0, type);
    }
    否则
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	整数	pat_len = xp->xp_pattern_len;
#もし defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	もし (unesc != NULL)
	    pat_src = unesc;
#endif

	もし (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	否则
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#もし defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	もし (tmp == NULL)
	    p = NULL;
	否则
	{
	    整数 use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    もし (escape)
		use_options |= WILD_ESCAPE;
	    もし (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    もし (p != NULL && type == WILD_LONGEST)
	    {
		整数	j;

		ために (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    もし (c == '*' || c == '?')
			break;
		}
		もし ((整数)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    もし (!wild_navigate && ccline->cmdbuff != NULL)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	もし (cmdline_orig.string == NULL)
	    cmdline_orig.length = 0;
	否则
	    cmdline_orig.length = ccline->cmdlen;
    }

    もし (p != NULL && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	整数	difflen;
	整数	v = OK;

	difflen = (整数)plen - xp->xp_pattern_len;
	もし (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	もし (v == OK)
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
    もし (xp->xp_context == EXPAND_MAPPINGS && p == NULL)
	戻る FAIL;

    もし (xp->xp_numfiles <= 0 && p == NULL)
	beep_flush();
    否则 もし (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(void)ExpandOne(xp, NULL, NULL, 0, WILD_FREE);

    vim_free(p);

    戻る OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    static 整数
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	整数		numMatches,
	整数		showtail)
{
    整数	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, numMatches);
    もし (compl_match_array == NULL)
	戻る EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = numMatches;
    ために (整数 i = 0; i < numMatches; i++)
    {
	compl_match_array[i].pum_text = (xp->xp_files_abbr != NULL
				    && xp->xp_files_abbr[i] != NULL)
					? xp->xp_files_abbr[i]
					: SHOW_MATCH(i);
	compl_match_array[i].pum_info = xp->xp_files_info != NULL
					    ? xp->xp_files_info[i] : NULL;
	compl_match_array[i].pum_extra = xp->xp_files_menu != NULL
					    ? xp->xp_files_menu[i] : NULL;
	compl_match_array[i].pum_kind = xp->xp_files_kind != NULL
					    ? xp->xp_files_kind[i] : NULL;
	compl_match_array[i].pum_user_abbr_hlattr = -1;
	compl_match_array[i].pum_user_kind_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == NULL ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    prefix_len = vim_strsize(xp->xp_pattern);
    もし (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    戻る EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    void
cmdline_pum_display(void)
{
    もし (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    整数
cmdline_pum_active(void)
{
    戻る pum_visible() && compl_match_array != NULL;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    void
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, 整数 defer_redraw)
{
    整数	save_KeyTyped = KeyTyped;
#ifdef FEAT_EVAL
    整数	save_RedrawingDisabled = RedrawingDisabled;
    もし (cclp->input_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    もし (!defer_redraw)
    {
	整数 save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    否则
	pum_call_update_screen();
    redrawcmd();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    KeyTyped = save_KeyTyped;
#ifdef FEAT_EVAL
    もし (cclp->input_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

    void
cmdline_pum_cleanup(cmdline_info_T *cclp)
{
    cmdline_pum_remove(cclp, FALSE);
    wildmenu_cleanup(cclp);
}

/*
 * Returns the starting column number to use for the cmdline completion popup
 * menu.
 */
    整数
cmdline_compl_startcol(void)
{
    戻る compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(void)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    戻る xp == NULL ? NULL : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    整数
cmdline_compl_is_fuzzy(void)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    戻る xp != NULL && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    static 整数
skip_status_match_char(expand_T *xp, char_u *s)
{
    もし ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	もし (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    戻る 2;
#endif
	戻る 1;
    }
    戻る 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    static 整数
status_match_len(expand_T *xp, char_u *s)
{
    整数	len = 0;

#ifdef FEAT_MENU
    整数 emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    もし (emenu && menu_is_separator(s))
	戻る 1;
#endif

    ながら (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    戻る len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    static void
win_redr_status_matches(
    expand_T	*xp,
    整数		num_matches,
    char_u	**matches,	// list of matches
    整数		match,
    整数		showtail)
{
    整数		row;
    char_u	*buf;
    整数		len;
    整数		clen;		// length in screen cells
    整数		fillchar;
    整数		attr;
    整数		i;
    整数		highlight = TRUE;
    char_u	*selstart = NULL;
    整数		selstart_col = 0;
    char_u	*selend = NULL;
    static 整数	first_match = 0;
    整数		add_left = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    整数		emenu;
#endif
    整数		l;

    もし (matches == NULL)	// interrupted completion?
	戻る;

    もし (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    否则
	buf = alloc(topframe->fr_width + 1);
    もし (buf == NULL)
	戻る;

    もし (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    もし (match == 0)
	first_match = 0;
    否则 もし (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_left = TRUE;
    }
    否则
    {
	// check if match fits on the screen
	ために (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	もし (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	もし (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    ために (i = match; i < num_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		もし (clen >= topframe->fr_width)
		    break;
	    }
	    もし (i == num_matches)
		add_left = TRUE;
	}
    }
    もし (add_left)
	ながら (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    もし (clen >= topframe->fr_width)
		break;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    もし (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    否则
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    ながら (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	もし (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	もし (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (整数)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	否则
#endif
	ために ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    もし (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    否则
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (整数)STRLEN(buf + len);
	    }
	}
	もし (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	もし (++i == num_matches)
		break;
    }

    もし (i != num_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    もし (row >= 0)
    {
	もし (wild_menu_showing == 0)
	{
	    もし (msg_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		もし (cmdline_row == Rows - 1)
		{
		    screen_del_lines(0, 0, 1, (整数)Rows, TRUE, 0, NULL);
		    ++msg_scrolled;
		}
		否则
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    否则
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		もし (lastwin->w_status_height == 0)
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
	もし (selstart != NULL && highlight)
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

#もし defined(FEAT_EVAL)
/*
 * Apply -completeopt=escape to a string about to be inserted into the command
 * line as a completion result.  If "str" is non-NULL and the active expansion
 * context is a customlist/custom user command with UCC_ESCAPE set, free "str"
 * and return a newly-allocated copy with spaces, tabs and backslashes prefixed
 * by a backslash.  Otherwise return "str" unchanged.
 */
    static char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    もし (str == NULL)
	戻る NULL;
    もし ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	戻る str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    もし (p == NULL)
	戻る str;
    vim_free(str);
    戻る p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    static char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    整数		srclen,
    整数		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    もし ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	戻る NULL;

    buf = alloc(srclen + 1);
    もし (buf == NULL)
	戻る NULL;

    d = buf;
    end = src + srclen;
    ために (p = src; p < end; ++p)
    {
	もし (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (整数)(d - buf);
    戻る buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    static char_u *
get_next_or_prev_match(整数 mode, expand_T *xp)
{
    整数	    findex = xp->xp_selected;
    整数	    ht;

    // When no matches found, return NULL
    もし (xp->xp_numfiles <= 0)
	戻る NULL;

    もし (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	もし (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    否则 もし (mode == WILD_NEXT)
    {
	// Select the next entry
	++findex;
    }
    否则   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	もし (ht > 3)
	    ht -= 2;

	もし (mode == WILD_PAGEUP)
	{
	    もし (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    否则 もし (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    否则
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	否则    // mode == WILD_PAGEDOWN
	{
	    もし (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    否则 もし (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    否则
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    もし (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	もし (xp->xp_orig != NULL)
	    findex = -1;
	否则
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    もし (p_wmnu)
    {
	もし (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	否则 もし (vim_strchr(p_wop, WOP_PUM) != NULL)
	{
	    もし (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	否则
	    win_redr_status_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    もし (findex == -1)
	戻る vim_strsave(xp->xp_orig);
#もし defined(FEAT_EVAL)
    戻る apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#否则
    戻る vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    static char_u *
ExpandOne_start(整数 mode, expand_T *xp, char_u *str, 整数 options)
{
    整数		non_suf_match;		// number without matching suffix
    整数		i;
    char_u	*ss = NULL;

    // Do the expansion.
    もし (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	もし (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    否则 もし (xp->xp_numfiles == 0)
    {
	もし (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    否则
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	もし (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    もし (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    否则
		non_suf_match = 1;
	    もし ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		ために (i = 0; i < 2; ++i)
		    もし (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    もし (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		もし (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		否则 もし (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    もし (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    戻る ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    static char_u *
find_longest_match(expand_T *xp, 整数 options)
{
    long_u	len;
    整数		mb_len = 1;
    整数		c0, ci;
    整数		i;
    char_u	*ss;

    ために (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	もし (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	否则
	    c0 = xp->xp_files[0][len];
	ために (i = 1; i < xp->xp_numfiles; ++i)
	{
	    もし (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    否则
		ci = xp->xp_files[i][len];
	    もし (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		もし (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    break;
	    }
	    否则 もし (c0 != ci)
		break;
	}
	もし (i < xp->xp_numfiles)
	{
	    もし (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    break;
	}
    }

    ss = alloc(len + 1);
    もし (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    戻る ss;
}

    void
free_xp_files_extra(expand_T *xp, 整数 numfiles)
{
    もし (xp->xp_files_abbr != NULL)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = NULL;
    }
    もし (xp->xp_files_kind != NULL)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = NULL;
    }
    もし (xp->xp_files_menu != NULL)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = NULL;
    }
    もし (xp->xp_files_info != NULL)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = NULL;
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
    整数		options,
    整数		mode)
{
    char_u	*ss = NULL;
    整数		orig_saved = FALSE;
#もし defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    整数		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    もし (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	戻る get_next_or_prev_match(mode, xp);

    もし (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    否则 もし (mode == WILD_APPLY)
    {
	もし (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	否则
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#もし defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    もし (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	もし (compl_match_array != NULL)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    もし (mode == WILD_FREE)	// only release file name
	戻る NULL;

    もし (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#もし defined(FEAT_EVAL)
	もし (ss != NULL)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    もし (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#もし defined(FEAT_EVAL)
	もし (ss != NULL)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    もし (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	char	*prefix = "";
	char	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	整数	n = xp->xp_numfiles - 1;
	整数	i;
#もし defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = NULL;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	もし ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    もし (escaped != NULL)
	    {
		ために (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    もし (escaped[i] == NULL)
		    {
			ながら (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			break;
		    }
		}
		もし (escaped != NULL)
		    files = escaped;
	    }
	}
#否则
	char_u	**files = xp->xp_files;
#endif

	もし (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	否则 もし (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	ために (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	もし (ss != NULL)
	{
	    size_t  ss_len = 0;

	    ために (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (char *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (char *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#もし defined(FEAT_EVAL)
	もし (escaped != NULL)
	{
	    ために (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    もし (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    もし (!orig_saved)
	vim_free(orig);

#もし defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    もし (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    戻る ss;
}

/*
 * Prepare an expand structure for use.
 */
    void
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
    void
ExpandCleanup(expand_T *xp)
{
    もし (xp->xp_numfiles >= 0)
    {
	free_xp_files_extra(xp, xp->xp_numfiles);
	FreeWild(xp->xp_numfiles, xp->xp_files);
	xp->xp_numfiles = -1;
    }
    VIM_CLEAR(xp->xp_orig);
}

    void
clear_cmdline_orig(void)
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
    static void
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	整数		numMatches,
	整数		lines,
	整数		linenr,
	整数		maxlen,
	整数		showtail,
	整数		dir_attr)
{
    整数		i, j;
    整数		isdir;
    整数		lastlen;
    char_u	*p;

    lastlen = 999;
    ために (j = linenr; j < numMatches; j += lines)
    {
	もし (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    msg_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    msg_advance(maxlen + 1);
	    msg_puts((char *)p);
	    msg_advance(maxlen + 3);
	    msg_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    break;
	}
	ために (i = maxlen - lastlen; --i >= 0; )
	    msg_putchar(' ');
	もし (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    もし (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_path;
		char_u	*path;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_path = expand_env_save_opt(matches[j], TRUE, NULL);
		path = exp_path != NULL ? exp_path : matches[j];
		halved_slash = backslash_halve_save(path);
		isdir = mch_isdir(halved_slash != NULL ? halved_slash
			: matches[j]);
		vim_free(exp_path);
		もし (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    否则
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    もし (showtail)
		p = SHOW_MATCH(j);
	    否则
	    {
		home_replace(NULL, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	否则
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = msg_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    もし (msg_col > 0)	// when not wrapped around
    {
	msg_clr_eos();
	msg_putchar('\n');
    }
    out_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    static 整数
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	整数		numMatches,
	整数		showtail,
	整数		noselect)
{
    整数	retval = cmdline_pum_create(ccline, xp, matches, numMatches, showtail);

    もし (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    戻る retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    整数
showmatches(
    expand_T	*xp,
    整数		display_wildmenu,
    整数		display_list,
    整数		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    整数		numMatches;
    char_u	**matches;
    整数		i;
    整数		maxlen;
    整数		lines;
    整数		columns;
    整数		attr;
    整数		showtail;
    整数		noselect = (wim_flags_arg & WIM_NOSELECT);
    整数		noinsert = (wim_flags_arg & WIM_NOINSERT);
    整数		cmdline_unchanged = noselect || noinsert;

    もし (xp->xp_numfiles == -1)
    {
	整数 retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&numMatches, &matches);
	もし (retval != EXPAND_OK)
	    戻る retval;
	showtail = expand_showtail(xp);
    }
    否则
    {
	numMatches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    もし (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != NULL)
	戻る show_pum_matches(ccline, xp, matches, numMatches,
		showtail && !cmdline_unchanged, noselect);

    もし (display_list)
    {
	msg_didany = FALSE;		// lines_left will be set
	msg_start();			// prepare for paging
	msg_putchar('\n');
	out_flush();
	cmdline_row = msg_row;
	msg_didany = FALSE;		// lines_left will be set again
	msg_start();			// prepare for paging
    }

    もし (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    否则 もし (display_wildmenu && !display_list)
	win_redr_status_matches(xp, numMatches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    否则 もし (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	ために (i = 0; i < numMatches; ++i)
	{
	    整数	len;
	    もし (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(NULL, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    否则
		len = vim_strsize(SHOW_MATCH(i));
	    もし (len > maxlen)
		maxlen = len;
	}

	もし (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    lines = numMatches;
	否则
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    もし (columns < 1)
		columns = 1;
	    lines = (numMatches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	もし (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    msg_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    msg_clr_eos();
	    msg_advance(maxlen - 3);
	    msg_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	ために (i = 0; i < lines; ++i)
	{
	    showmatches_oneline(xp, matches, numMatches, lines, i,
						maxlen, showtail, attr);
	    もし (got_int)
	    {
		got_int = FALSE;
		break;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = msg_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    もし (display_wildmenu && display_list)
    {
	もし (vim_strchr(p_wop, WOP_PUM) != NULL)
	    (void)show_pum_matches(ccline, xp, matches, numMatches,
		    showtail && !cmdline_unchanged, noselect);
	否则
	    win_redr_status_matches(xp, numMatches, matches,
		    noselect ? -1 : 0, showtail);
    }

    もし (xp->xp_numfiles == -1)
    {
	FreeWild(numMatches, matches);
	free_xp_files_extra(xp, numMatches);
    }

    戻る EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    static char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    整数		had_sep = FALSE;

    ために (p = s; *p != NUL; )
    {
	もし (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	否则 もし (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    戻る t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    static 整数
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    もし (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	戻る FALSE;

    end = gettail(xp->xp_pattern);
    もし (end == xp->xp_pattern)		// there is no path separator
	戻る FALSE;

    ために (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	もし (rem_backslash(s))
	    ++s;
	否则 もし (vim_strchr((char_u *)"*?[", *s) != NULL)
	    戻る FALSE;
    }
    戻る TRUE;
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
    整数		len,
    整数		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    整数		i, j;
    整数		new_len;
    char_u	*tail;
    整数		ends_in_star;

    もし (context != EXPAND_FILES
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
	もし (context == EXPAND_FINDFUNC
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
	否则
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    ために (i = 0; i < len; i++)
	    {
		もし (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		もし (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		もし ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    もし (retval != NULL)
	    {
		retval[0] = '^';
		j = 1;
		ために (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    もし (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			break;

		    switch (fname[i])
		    {
			case '*':   retval[j++] = '.';
				    break;
			case '~':   retval[j++] = '\\';
				    break;
			case '?':   retval[j] = '.';
				    continue;
			case '.':   もし (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    break;
			case '\\':  もし (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    break;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    否则
    {
	retval = alloc(len + 4);
	もし (retval != NULL)
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
	    ために (i = len - 2; i >= 0; --i)
	    {
		もし (retval[i] != '\\')
		    break;
		ends_in_star = !ends_in_star;
	    }
#endif
	    もし ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == NULL
		    && vim_strchr(retval, '`') == NULL)
		retval[len++] = '*';
	    否则 もし (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    戻る retval;
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
    void
set_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    もし ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_search_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_SEARCH_EXTRA
	search_first_line = 0; // Search entire buffer
#endif
	戻る;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    もし (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->input_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	戻る;
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
    static char_u *
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, 整数 *complp)
{
    char_u	*p = NULL;
    整数		len = 0;
    整数		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    もし (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    否则
    {
	p = cmd;
	ながら (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	もし (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    ながら (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	もし (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    ながら (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	もし (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != NULL)
	    ++p;
	len = (整数)(p - cmd);

	もし (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    戻る NULL;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	もし ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    ながら (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    もし (*p == NUL && ASCII_ISALNUM(p[-1]))
	戻る NULL;

    もし (eap->cmdidx == CMD_SIZE)
    {
	もし (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != NULL)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	否则 もし (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, NULL, xp, complp);
	    もし (p == NULL)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    もし (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	戻る NULL;
    }

    戻る p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    static void
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	整数		usefilter,
	expand_T	*xp,
	整数		*complp)
{
    char_u	*p;
    整数		c;
    整数		in_quote = FALSE;
    char_u	*bow = NULL;	// Beginning of word
    整数		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    ながら (*p != NUL)
    {
	もし (has_mbyte)
	    c = mb_ptr2char(p);
	否则
	    c = *p;
	もし (c == '\\' && p[1] != NUL)
	    ++p;
	否则 もし (c == '`')
	{
	    もし (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	否则 もし (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != NULL && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    ながら (*p != NUL)
	    {
		もし (has_mbyte)
		    c = mb_ptr2char(p);
		否则
		    c = *p;
		もし (c == '`' || vim_isfilec_or_wc(c))
		    break;
		もし (has_mbyte)
		    len = (*mb_ptr2len)(p);
		否则
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    もし (in_quote)
		bow = p;
	    否则
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    もし (bow != NULL && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    もし (usefilter
	    || (eap != NULL
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	もし (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    もし (*xp->xp_pattern == '$')
    {
	ために (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    もし (!vim_isIDc(*p))
		break;
	もし (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    もし (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    もし (*xp->xp_pattern == '~')
    {
	ために (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	もし (*p == NUL && p > xp->xp_pattern + 1
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
    static char_u *
set_context_in_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    もし (p == NULL)
	xp->xp_pattern = arg;
    否则
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    戻る NULL;
}

#ifdef FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    static char_u *
set_context_in_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    もし (p == NULL)
	xp->xp_pattern = arg;
    否则
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    戻る NULL;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    static char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    もし (*arg != NUL)
	arg = skip_vimgrep_pat(arg, NULL, NULL);
    もし (arg == NULL || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	戻る NULL;
    }
    戻る skipwhite(arg);
}

#ifdef FEAT_SEARCH_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    static char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    もし (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	もし (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    戻る find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    static char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    整数		delim;

    delim = *arg;	    // get the delimiter
    もし (delim)
	++arg;		    // skip delimiter if there is one

    ながら (arg[0] != NUL && arg[0] != delim)
    {
	もし (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    もし (arg[0] != NUL)
	戻る arg + 1;

    戻る NULL;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    static char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    整数		delim;

    delim = *arg;
    もし (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	もし (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    ながら (arg[0] != NUL && arg[0] != delim)
	    {
		もし (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    もし (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    ながら (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == NULL)
	++arg;
    もし (arg[0] != NUL)
	戻る arg;

    戻る NULL;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    static char_u *
find_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    もし (*arg != '/')
	戻る NULL;

    // Match regexp, not just whole words
    ために (++arg; *arg && *arg != '/'; arg++)
	もし (*arg == '\\' && arg[1] != NUL)
	    arg++;
    もし (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	もし (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == NULL)
	    xp->xp_context = EXPAND_NOTHING;
	否则
	    戻る arg;
    }

    戻る NULL;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    static char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    ながら ((xp->xp_pattern = vim_strchr(arg, ' ')) != NULL)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    もし (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    戻る NULL;
}
#endif

#もし defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    static char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    もし (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    否则
    {
	もし ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_pattern = skipwhite(p);
	}
	否则
	    xp->xp_context = EXPAND_NOTHING;
    }

    戻る NULL;
}
#endif

static enum
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
static enum
{
    EXP_BREAKPT_ADD,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    static char_u *
set_context_in_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_start;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_pattern = arg;

    もし (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    否则 もし (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    否则
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    もし (*p == NUL)
	戻る NULL;
    subcmd_start = p;

    もし (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	もし (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    もし (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		戻る NULL;
	    }
	    p = skipwhite(p);
	}
	もし (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	否则
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    否则 もし (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    戻る NULL;
}

    static char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = NULL;

    p = skipwhite(arg);
    もし (VIM_ISDIGIT(*p))
	戻る NULL;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    戻る NULL;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    static char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    整数    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    もし (*p == NUL)
	戻る NULL;

    ために (;;)
    {
	もし (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    continue;
	}
	もし (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    continue;
	}
	break;
    }

    もし ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    否则 もし ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    否则 もし ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    戻る NULL;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    static void
set_context_with_pattern(expand_T *xp)
{
    整数		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_SEARCH_EXTRA
    整数		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    もし (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	戻る;
#endif

    xp->xp_pattern = ccline->cmdbuff + skiplen;
    xp->xp_pattern_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_PATTERN_IN_BUF;
    xp->xp_search_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    static char_u *
set_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	long		argt,
	整数		compl,
	整数		forceit)
{
    char_u  *nextcmd;

    switch (cmdidx)
    {
	case CMD_find:
	case CMD_sfind:
	case CMD_tabfind:
	    もし (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    break;
	case CMD_cd:
	case CMD_chdir:
	case CMD_tcd:
	case CMD_tchdir:
	case CMD_lcd:
	case CMD_lchdir:
	    もし (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    break;
	case CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    break;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	case CMD_aboveleft:
	case CMD_argdo:
	case CMD_belowright:
	case CMD_botright:
	case CMD_browse:
	case CMD_bufdo:
	case CMD_cdo:
	case CMD_cfdo:
	case CMD_confirm:
	case CMD_debug:
	case CMD_folddoclosed:
	case CMD_folddoopen:
	case CMD_hide:
	case CMD_horizontal:
	case CMD_keepalt:
	case CMD_keepjumps:
	case CMD_keepmarks:
	case CMD_keeppatterns:
	case CMD_ldo:
	case CMD_leftabove:
	case CMD_lfdo:
	case CMD_lockmarks:
	case CMD_noautocmd:
	case CMD_noswapfile:
	case CMD_rightbelow:
	case CMD_sandbox:
	case CMD_silent:
	case CMD_tab:
	case CMD_tabdo:
	case CMD_topleft:
	case CMD_unsilent:
	case CMD_verbose:
	case CMD_vertical:
	case CMD_windo:
	case CMD_vim9cmd:
	case CMD_legacy:
	    戻る arg;

	case CMD_filter:
	    戻る set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_SEARCH_EXTRA
	case CMD_match:
	    戻る set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	case CMD_command:
	    戻る set_context_in_user_cmd(xp, arg);

	case CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    break;

	case CMD_global:
	case CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    もし (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    戻る nextcmd;

	case CMD_and:
	case CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    もし (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    戻る nextcmd;

	case CMD_isearch:
	case CMD_dsearch:
	case CMD_ilist:
	case CMD_dlist:
	case CMD_ijump:
	case CMD_psearch:
	case CMD_djump:
	case CMD_isplit:
	case CMD_dsplit:
	    戻る find_cmd_after_isearch_cmd(xp, arg);
	case CMD_autocmd:
	    戻る set_context_in_autocmd(xp, arg, FALSE);
	case CMD_doautocmd:
	case CMD_doautoall:
	    戻る set_context_in_autocmd(xp, arg, TRUE);
	case CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    break;
	case CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    break;
	case CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    break;
	case CMD_tag:
	case CMD_stag:
	case CMD_ptag:
	case CMD_ltag:
	case CMD_tselect:
	case CMD_stselect:
	case CMD_ptselect:
	case CMD_tjump:
	case CMD_stjump:
	case CMD_ptjump:
	    もし (vim_strchr(p_wop, WOP_TAGFILE) != NULL)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    否则
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    break;
	case CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    break;
#ifdef FEAT_SYN_HL
	case CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    break;
#endif
#ifdef FEAT_EVAL
	case CMD_final:
	case CMD_const:
	case CMD_let:
	case CMD_var:
	case CMD_if:
	case CMD_elseif:
	case CMD_while:
	case CMD_for:
	case CMD_echo:
	case CMD_echon:
	case CMD_execute:
	case CMD_echomsg:
	case CMD_echoerr:
	case CMD_echoconsole:
	case CMD_echowindow:
	case CMD_call:
	case CMD_return:
	case CMD_cexpr:
	case CMD_caddexpr:
	case CMD_cgetexpr:
	case CMD_lexpr:
	case CMD_laddexpr:
	case CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    break;

	case CMD_unlet:
	    戻る set_context_in_unlet_cmd(xp, arg);
	case CMD_function:
	case CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    break;
	case CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    break;

	case CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    break;
#endif
	case CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    break;
#ifdef FEAT_CSCOPE
	case CMD_cscope:
	case CMD_lcscope:
	case CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    break;
#endif
#ifdef FEAT_SIGNS
	case CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    break;
#endif
	case CMD_bdelete:
	case CMD_bwipeout:
	case CMD_bunload:
	    ながら ((xp->xp_pattern = vim_strchr(arg, ' ')) != NULL)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	case CMD_buffer:
	case CMD_sbuffer:
	case CMD_pbuffer:
	case CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    break;
#ifdef FEAT_DIFF
	case CMD_diffget:
	case CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    break;
#endif
	case CMD_USER:
	case CMD_USER_BUF:
	    戻る set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	case CMD_map:	    case CMD_noremap:
	case CMD_nmap:	    case CMD_nnoremap:
	case CMD_vmap:	    case CMD_vnoremap:
	case CMD_omap:	    case CMD_onoremap:
	case CMD_imap:	    case CMD_inoremap:
	case CMD_cmap:	    case CMD_cnoremap:
	case CMD_lmap:	    case CMD_lnoremap:
	case CMD_smap:	    case CMD_snoremap:
	case CMD_tmap:	    case CMD_tnoremap:
	case CMD_xmap:	    case CMD_xnoremap:
	    戻る set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	case CMD_unmap:
	case CMD_nunmap:
	case CMD_vunmap:
	case CMD_ounmap:
	case CMD_iunmap:
	case CMD_cunmap:
	case CMD_lunmap:
	case CMD_sunmap:
	case CMD_tunmap:
	case CMD_xunmap:
	    戻る set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	case CMD_mapclear:
	case CMD_nmapclear:
	case CMD_vmapclear:
	case CMD_omapclear:
	case CMD_imapclear:
	case CMD_cmapclear:
	case CMD_lmapclear:
	case CMD_smapclear:
	case CMD_tmapclear:
	case CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    break;

	case CMD_abbreviate:	case CMD_noreabbrev:
	case CMD_cabbrev:	case CMD_cnoreabbrev:
	case CMD_iabbrev:	case CMD_inoreabbrev:
	    戻る set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	case CMD_unabbreviate:
	case CMD_cunabbrev:
	case CMD_iunabbrev:
	    戻る set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	case CMD_menu:	    case CMD_noremenu:	    case CMD_unmenu:
	case CMD_amenu:	    case CMD_anoremenu:	    case CMD_aunmenu:
	case CMD_nmenu:	    case CMD_nnoremenu:	    case CMD_nunmenu:
	case CMD_vmenu:	    case CMD_vnoremenu:	    case CMD_vunmenu:
	case CMD_omenu:	    case CMD_onoremenu:	    case CMD_ounmenu:
	case CMD_imenu:	    case CMD_inoremenu:	    case CMD_iunmenu:
	case CMD_cmenu:	    case CMD_cnoremenu:	    case CMD_cunmenu:
	case CMD_tlmenu:    case CMD_tlnoremenu:    case CMD_tlunmenu:
	case CMD_tmenu:				    case CMD_tunmenu:
	case CMD_popup:	    case CMD_tearoff:	    case CMD_emenu:
	    戻る set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	case CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    break;

	case CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    break;

	case CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    break;

	case CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    break;

	case CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    break;

	case CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    break;

#もし defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	case CMD_language:
	    戻る set_context_in_lang_cmd(xp, arg);
#endif
#もし defined(FEAT_PROFILE)
	case CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    break;
#endif
	case CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    break;

	case CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    break;

	case CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    break;

	case CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    break;
#もし defined(FEAT_PROFILE)
	case CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    break;
#endif

	case CMD_argdelete:
	    ながら ((xp->xp_pattern = vim_strchr(arg, ' ')) != NULL)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    break;

#ifdef FEAT_EVAL
	case CMD_breakadd:
	case CMD_profdel:
	case CMD_breakdel:
	    戻る set_context_in_breakadd_cmd(xp, arg, cmdidx);

	case CMD_scriptnames:
	    戻る set_context_in_scriptnames_cmd(xp, arg);
#endif
	case CMD_filetype:
	    戻る set_context_in_filetype_cmd(xp, arg);

	default:
	    break;
    }
    戻る NULL;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    static char_u *
set_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    整数			len = 0;
    exarg_T		ea;
    整数			compl = EXPAND_NOTHING;
    整数			forceit = FALSE;
    整数			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_line = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    ために (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != NULL; cmd++)
	;
    xp->xp_pattern = cmd;

    もし (*cmd == NUL)
	戻る NULL;
    もし (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	戻る NULL;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    もし (*cmd == NUL)
	戻る NULL;
    もし (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	戻る NULL;
    }

    もし (*cmd == '|' || *cmd == '\n')
	戻る cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    もし (p == NULL)
	戻る NULL;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    もし (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    もし (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    もし ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	ながら (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    ながら (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    もし (*p == NUL)
	    {
		もし (ea.argt & EX_ARGOPT)
		    戻る set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		もし (ea.cmdidx == CMD_terminal)
		    戻る set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    もし (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	もし (*arg == '>')			// append
	{
	    もし (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	否则 もし (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    もし (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	もし (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    もし (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ながら (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    もし ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	もし (*arg == NUL)
	    戻る p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    もし ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	もし (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	ながら (*p)
	{
	    もし (*p == Ctrl_V)
	    {
		もし (p[1] != NUL)
		    ++p;
	    }
	    否则 もし ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		もし (*(p - 1) != '\\')
		{
		    もし (*p == '|' || *p == '\n')
			戻る p + 1;
		    戻る NULL;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    もし (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == NULL)
	// no arguments allowed but there is something
	戻る NULL;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (整数)STRLEN(buff);
    ながら (*p && p < buff + len)
    {
	もし (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	否则
	{
	    もし (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    もし (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    戻る set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    void
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    整数		len,	    // length of command line (excl. NUL)
    整数		col,	    // position of cursor
    整数		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    整数			context;
#endif
    整数		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    もし (col < len)
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
    もし (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    否则 もし (use_ccline && ccline->input_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	もし (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(NULL, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    否则
#endif
	ながら (nextcomm != NULL)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_line = str;
    xp->xp_col = col;

    もし (col < len)
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
    整数
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    整数		col,		// position of cursor
    整数		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = NULL;
    整数		options = WILD_ADD_SLASH|WILD_SILENT;

    もし (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	戻る EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    もし (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	戻る EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (整数)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	整数	pat_len = xp->xp_pattern_len;
#もし defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	もし (unesc != NULL)
	    pat_src = unesc;
#endif

	もし (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	否则
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#もし defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    もし (file_str == NULL)
	戻る EXPAND_UNSUCCESSFUL;

    もし (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    もし (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = NULL;
    }
    vim_free(file_str);

    戻る EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    static 整数
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	整数		*numMatches,
	整数		flags,
	整数		options)
{
    整数		free_pat = FALSE;
    整数		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    もし (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	もし (pat == NULL)
	    戻る ret;

	pat_end = pat + pat_len;
	ために (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    もし (*p != '\\')
		continue;

	    もし (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    否则 もし (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    否则 もし (xp->xp_backslash & XP_BS_COMMA)
	    {
		もし (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		否则 もし (*(p + 1) == ',')
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

    もし (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, numMatches);
#endif
    }
    否则
    {
	もし (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	否则 もし (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	否则 もし (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	否则
	    flags = (flags | EW_DIR) & ~EW_FILE;
	もし (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, numMatches, matches, flags);
    }
    もし (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    もし (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	整数 j;

	ために (j = 0; j < *numMatches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    ながら (*ptr != NUL)
	    {
		もし (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		否则 もし (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    戻る ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    static char_u *
get_behave_arg(expand_T *xp UNUSED, 整数 idx)
{
    もし (idx == 0)
	戻る (char_u *)"mswin";
    もし (idx == 1)
	戻る (char_u *)"xterm";
    戻る NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    static char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, 整数 idx)
{
    もし (idx < 0)
	戻る NULL;

    もし (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	char	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	戻る (char_u *)opts_all[idx];
    }
    もし (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	char	*opts_plugin[] = {"plugin", "on", "off"};
	戻る (char_u *)opts_plugin[idx];
    }
    もし (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	char	*opts_indent[] = {"indent", "on", "off"};
	戻る (char_u *)opts_indent[idx];
    }
    もし (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	char	*opts_onoff[] = {"on", "off"};
	戻る (char_u *)opts_onoff[idx];
    }
    戻る NULL;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    static char_u *
get_breakadd_arg(expand_T *xp UNUSED, 整数 idx)
{
    もし (idx >= 0 && idx <= 3)
    {
	char	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	もし (breakpt_expand_what == EXP_BREAKPT_ADD)
	    戻る (char_u *)opts[idx];
	否则 もし (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    もし (idx <= 2)
		戻る (char_u *)opts[idx + 1];
	}
	否则
	{
	    // profdel {func, file}
	    もし (idx <= 1)
		戻る (char_u *)opts[idx + 1];
	}
    }
    戻る NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    static char_u *
get_scriptnames_arg(expand_T *xp UNUSED, 整数 idx)
{
    scriptitem_T *si;

    もし (!SCRIPT_ID_VALID(idx + 1))
	戻る NULL;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(NULL, si->sn_name, NameBuff, MAXPATHL, TRUE);
    戻る NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    static char_u *
get_retab_arg(expand_T *xp UNUSED, 整数 idx)
{
    もし (idx == 0)
	戻る (char_u *)"-indentonly";
    戻る NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    static char_u *
get_messages_arg(expand_T *xp UNUSED, 整数 idx)
{
    もし (idx == 0)
	戻る (char_u *)"clear";
    戻る NULL;
}

    static char_u *
get_mapclear_arg(expand_T *xp UNUSED, 整数 idx)
{
    もし (idx == 0)
	戻る (char_u *)"<buffer>";
    戻る NULL;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    static 整数
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	整数		*numMatches)
{
    static struct expgen
    {
	整数		context;
	char_u	*((*func)(expand_T *, 整数));
	整数		ic;
	整数		escaped;
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
	{EXPAND_EXPRESSION, get_expr_name, FALSE, TRUE},
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
#もし defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
    整数	i;
    整数 ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    ために (i = 0; i < (整数)ARRAY_LENGTH(tab); ++i)
    {
	もし (xp->xp_context == tab[i].context)
	{
	    もし (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, numMatches,
						tab[i].func, tab[i].escaped);
	    break;
	}
    }

    戻る ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    static 整数
map_wildopts_to_ewflags(整数 options)
{
    整数		flags;

    flags = EW_DIR;	// include directories
    もし (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    もし (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    もし (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    もし (options & WILD_SILENT)
	flags |= EW_SILENT;
    もし (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    もし (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    戻る flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    static 整数
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    整数		*numMatches,
    整数		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    整数		ret;
    整数		flags;
    char_u	*tofree = NULL;
    整数		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    もし (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	戻る expand_files_and_dirs(xp, pat, matches, numMatches, flags,
								options);

    *matches = (char_u **)"";
    *numMatches = 0;
    もし (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	もし (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					numMatches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*numMatches, *matches);
#endif
	    戻る OK;
	}
	戻る FAIL;
    }

    もし (xp->xp_context == EXPAND_SHELLCMD)
	戻る expand_shellcmd(pat, matches, numMatches, flags);
    もし (xp->xp_context == EXPAND_OLD_SETTING)
	戻る ExpandOldSetting(numMatches, matches);
    もし (xp->xp_context == EXPAND_BUFFERS)
	戻る ExpandBufnames(pat, numMatches, matches, options);
#ifdef FEAT_DIFF
    もし (xp->xp_context == EXPAND_DIFF_BUFFERS)
	戻る ExpandBufnames(pat, numMatches, matches,
						options | BUF_DIFF_FILTER);
#endif
    もし (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	戻る expand_tags(xp->xp_context == EXPAND_TAGS, pat, numMatches,
								matches);
    もし (xp->xp_context == EXPAND_COLORS)
    {
	char *directories[] = {"colors", NULL};
	戻る ExpandRTDir(pat, DIP_START + DIP_OPT, numMatches, matches,
								directories);
    }
    もし (xp->xp_context == EXPAND_COMPILER)
    {
	char *directories[] = {"compiler", NULL};
	戻る ExpandRTDir(pat, 0, numMatches, matches, directories);
    }
    もし (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	char *directories[] = {"syntax", NULL};
	戻る ExpandRTDir(pat, 0, numMatches, matches, directories);
    }
    もし (xp->xp_context == EXPAND_FILETYPE)
    {
	char *directories[] = {"syntax", "indent", "ftplugin", NULL};
	戻る ExpandRTDir(pat, 0, numMatches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    もし (xp->xp_context == EXPAND_KEYMAP)
    {
	char *directories[] = {"keymap", NULL};
	戻る ExpandRTDir(pat, 0, numMatches, matches, directories);
    }
#endif
#もし defined(FEAT_EVAL)
    もし (xp->xp_context == EXPAND_USER_LIST)
	戻る ExpandUserList(xp, matches, numMatches);
#endif
    もし (xp->xp_context == EXPAND_PACKADD)
	戻る ExpandPackAddDir(pat, numMatches, matches);
    もし (xp->xp_context == EXPAND_RUNTIME)
	戻る expand_runtime_cmd(pat, numMatches, matches);
    もし (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	戻る expand_pattern_in_buf(pat, xp->xp_search_dir,
		matches, numMatches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    もし ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	整数 len = (整数)STRLEN(pat) + 20;

	tofree = alloc(len);
	もし (tofree == NULL)
	    戻る FAIL;
	vim_snprintf((char *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    もし (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	もし (regmatch.regprog == NULL)
	{
	    vim_free(tofree);
	    戻る FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    もし (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, numMatches, matches, fuzzy);
    否则 もし (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, numMatches, matches);
    否则 もし (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, numMatches, matches);
    否则 もし (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, numMatches, matches);
    否则 もし (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, numMatches);
    否则 もし (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, numMatches);
#もし defined(FEAT_TERMINAL)
    否则 もし (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, numMatches);
#endif
#もし defined(FEAT_EVAL)
    否则 もし (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, numMatches);
#endif
    否则
	ret = ExpandOther(pat, xp, &regmatch, matches, numMatches);

    もし (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    戻る ret;
}

    整数
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    整数		*numMatches,
    char_u	*(*func)(expand_T *, 整数),
					  // returns a string from the list
    整数		escaped)
{
    戻る ExpandGenericExt(
	pat, xp, regmatch, matches, numMatches, func, escaped, 0);
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
    整数
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    整数		*numMatches,
    char_u	*(*func)(expand_T *, 整数),
					  // returns a string from the list
    整数		escaped,
    整数		sortStartIdx)
{
    整数		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = NULL;
    整数		score = 0;
    整数		fuzzy;
    整数		match;
    整数		sort_matches = FALSE;
    整数		funcsort = FALSE;
    整数		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = NULL;
    *numMatches = 0;

    もし (!fuzzy)
	ga_init2(&ga, sizeof(char *), 30);
    否则
	ga_init2(&ga, sizeof(fuzmatch_str_T), 30);

    ために (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	もし (str == NULL)	    // end of list
	    break;
	もし (*str == NUL)	    // skip empty strings
	    continue;

	もし (xp->xp_pattern[0] != NUL)
	{
	    もし (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    否则
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	否则
	    match = TRUE;

	もし (!match)
	    continue;

	もし (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	否则
	    str = vim_strsave(str);
	もし (str == NULL)
	{
	    もし (!fuzzy)
	    {
		ga_clear_strings(&ga);
		戻る FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    戻る FAIL;
	}

	もし (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    break;
	}

	もし (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	否则
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

#ifdef FEAT_MENU
	もし (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    もし (*str == '\001')
		*str = '.';
	}
#endif

	もし (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    もし (ga.ga_len == 0)
	戻る OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    もし (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    もし (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    もし (sort_matches && sortStartMatchIdx != -1)
    {
	もし (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((void *)ga.ga_data, (size_t)ga.ga_len, sizeof(char_u *),
							   sort_func_compare);
	否则
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    もし (!fuzzy)
    {
	*matches = ga.ga_data;
	*numMatches = ga.ga_len;
    }
    否则
    {
	もし (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    戻る FAIL;
	*numMatches = ga.ga_len;
    }

#もし defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    戻る OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    static void
expand_shellcmd_onedir(
	char_u		*pathed_pattern,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	整数		*numMatches,
	整数		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    もし (expand_wildcards(1, &pathed_pattern, numMatches, matches, flags) != OK)
	戻る;

    もし (ga_grow(gap, *numMatches) == FAIL)
    {
	FreeWild(*numMatches, *matches);
	戻る;
    }

    ために (整数 i = 0; i < *numMatches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	もし (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    もし (HASHITEM_EMPTY(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_data)[gap->ga_len++] = name;
		hash_add_item(ht, hi, name, hash);
		name = NULL;
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
    static 整数
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    整数		*numMatches,	// return: number of matches
    整数		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = NULL;
    整数		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    整数		flags = flagsarg;
    整数		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    もし (buf == NULL)
	戻る FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    もし (pat == NULL)
    {
	vim_free(buf);
	戻る FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    ために (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	もし (*s != '\\')
	    continue;

	p = s + 1;
	もし (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    もし (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    否则
    {
	// For an absolute name we don't use $PATH.
	もし (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	もし (path == NULL)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, sizeof(char *), 10);
    hash_init(&found_ht);
    ために (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	もし (*s == NUL)
	{
	    もし (did_curdir)
		break;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	否则
	{
#もし defined(MSWIN)
	    e = vim_strchr(s, ';');
#否则
	    e = vim_strchr(s, ':');
#endif
	    もし (e == NULL)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    もし (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    否则
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? sizeof(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	もし (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    もし (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		もし (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, numMatches, flags,
				    &found_ht, &ga);
	}

	もし (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_data;
    *numMatches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    もし (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    戻る OK;
}

#もし defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    static void *
call_user_expand_func(
    void	*(*user_expand_func)(char_u *, 整数, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    整数		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = NULL;
    void	*ret;

    もし (xp->xp_arg == NULL || xp->xp_arg[0] == '\0' || xp->xp_line == NULL)
	戻る NULL;

    もし (ccline->cmdbuff != NULL)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	整数 unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	もし (pat == NULL)
	    pat = vim_strnsave(xp->xp_pattern, xp->xp_pattern_len);
    }

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = xp->xp_line;
    args[2].v_type = VAR_NUMBER;
    args[2].vval.v_number = xp->xp_col;
    args[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, args);

    current_sctx = save_current_sctx;
    もし (ccline->cmdbuff != NULL)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    戻る ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    static 整数
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    整数		*numMatches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    整数		keep;
    garray_T	ga;
    整数		fuzzy;
    整数		match;
    整数		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = NULL;
    *numMatches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    もし (retstr == NULL)
	戻る FAIL;

    もし (!fuzzy)
	ga_init2(&ga, sizeof(char *), 3);
    否则
	ga_init2(&ga, sizeof(fuzmatch_str_T), 3);

    ために (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	もし (e == NULL)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	もし (xp->xp_pattern[0] != NUL)
	{
	    もし (!fuzzy)
		match = vim_regexec(regmatch, s, (colnr_T)0);
	    否则
	    {
		score = fuzzy_match_str(s, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	否则
	    match = TRUE;		// match everything

	*e = keep;

	もし (match)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    もし (p == NULL)
		break;

	    もし (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		break;
	    }

	    もし (!fuzzy)
		((char_u **)ga.ga_data)[ga.ga_len] = p;
	    否则
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	もし (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    もし (ga.ga_len == 0)
	戻る OK;

    もし (!fuzzy)
    {
	*matches = ga.ga_data;
	*numMatches = ga.ga_len;
    }
    否则
    {
	もし (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    戻る FAIL;
	*numMatches = ga.ga_len;
    }
    戻る OK;
}

    void
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    整数		*numMatches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    整数		have_extra = FALSE;
    整数		i;

    ga_init2(&ga, sizeof(char *), 3);
    ga_init2(&ga_abbr, sizeof(char *), 3);
    ga_init2(&ga_kind, sizeof(char *), 3);
    ga_init2(&ga_menu, sizeof(char *), 3);
    ga_init2(&ga_info, sizeof(char *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_ITEMS(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = NULL;
	char_u	*abbr = NULL;
	char_u	*kind = NULL;
	char_u	*menu = NULL;
	char_u	*info = NULL;

	もし (tv->v_type == VAR_STRING)
	{
	    もし (tv->vval.v_string == NULL)
		continue;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	否则 もし (tv->v_type == VAR_DICT && tv->vval.v_dict != NULL)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    もし (word == NULL)
		continue;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    もし (abbr != NULL || kind != NULL || menu != NULL || info != NULL)
		have_extra = TRUE;
	}
	否则
	    continue;  // Skip other types

	もし (p == NULL
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
	    break;
	}

	((char_u **)ga.ga_data)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_data)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_data)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_data)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_data)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_data;
    *numMatches = ga.ga_len;
    もし (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    否则
    {
	// No extra info collected; free the placeholder NULL entries.
	ために (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	ために (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	ために (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	ために (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    static 整数
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    整数		*numMatches)
{
    list_T      *retlist;

    *matches = NULL;
    *numMatches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    もし (retlist == NULL)
	戻る FAIL;

    expand_process_user_list(retlist, matches, numMatches, xp);
    list_unref(retlist);
    戻る OK;
}
#endif

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    void
globpath(
    char_u	*path,
    char_u	*file,
    garray_T	*ga,
    整数		expand_options,
    整数		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    もし (buf == NULL)
	戻る;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#もし defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#否则
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    ながら (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	もし (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    整数	    num_p;
	    char_u  **p;

	    もし (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    もし (ExpandFromContext(&xpc, buf, &p, &num_p,
			     WILD_SILENT|expand_options) != FAIL && num_p > 0)
	    {
		ExpandEscape(&xpc, buf, num_p, p, WILD_SILENT|expand_options);

		もし (ga_grow(ga, num_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    ために (整数 i = 0; i < num_p; ++i)
		    {
			((char_u **)ga->ga_data)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		否则
		{
		    FreeWild(num_p, p);
		    p = NULL;
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
    整数
wildmenu_translate_key(
	cmdline_info_T	*cclp,
	整数		key,
	expand_T	*xp,
	整数		did_wild_list)
{
    整数 c = key;

    もし (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	switch (c)
	{
	    case K_UP:	  c = K_LEFT; break;
	    case K_DOWN:  c = K_RIGHT; break;
	    case K_LEFT:  c = K_UP; break;
	    case K_RIGHT: c = K_DOWN; break;
	    default:	  break;
	}
    }

    もし (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	もし (c == K_LEFT)
	    c = Ctrl_P;
	否则 もし (c == K_RIGHT)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    もし (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    戻る c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    static void
cmdline_del(cmdline_info_T *cclp, 整数 from)
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
    static 整数
wildmenu_process_key_menunames(cmdline_info_T *cclp, 整数 key, expand_T *xp)
{
    整数		i;
    整数		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    もし (key == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	key = p_wc;
	KeyTyped = TRUE;  // in case the key was mapped
    }
    否则 もし (key == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	整数 found = FALSE;

	j = (整数)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	ながら (--j > 0)
	{
	    // check for start of menu name
	    もし (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		break;
	    }
	    // check for start of submenu name
	    もし (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		もし (found)
		{
		    i = j + 1;
		    break;
		}
		否则
		    found = TRUE;
	    }
	}
	もし (i > 0)
	    cmdline_del(cclp, i);
	key = p_wc;
	KeyTyped = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    戻る key;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    static 整数
wildmenu_process_key_filenames(cmdline_info_T *cclp, 整数 key, expand_T *xp)
{
    整数		i;
    整数		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    もし (key == K_DOWN
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	key = p_wc;
	KeyTyped = TRUE;  // in case the key was mapped
    }
    否则 もし (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && key == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	整数 found = FALSE;

	j = cclp->cmdpos;
	i = (整数)(xp->xp_pattern - cclp->cmdbuff);
	ながら (--j > i)
	{
	    もし (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    もし (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		break;
	    }
	}
	もし (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    key = p_wc;
	    KeyTyped = TRUE;  // in case the key was mapped
	}
    }
    否则 もし (key == K_UP)
    {
	// go up a directory
	整数 found = FALSE;

	j = cclp->cmdpos - 1;
	i = (整数)(xp->xp_pattern - cclp->cmdbuff);
	ながら (--j > i)
	{
	    もし (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    もし (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == NULL
#endif
	       )
	    {
		もし (found)
		{
		    i = j + 1;
		    break;
		}
		否则
		    found = TRUE;
	    }
	}

	もし (!found)
	    j = i;
	否则 もし (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	否则 もし (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	否则
	    j = 0;
	もし (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	否则 もし (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	key = p_wc;
	KeyTyped = TRUE;
    }

    戻る key;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    整数
wildmenu_process_key(cmdline_info_T *cclp, 整数 key, expand_T *xp)
{
    もし (xp->xp_context == EXPAND_MENUNAMES)
	戻る wildmenu_process_key_menunames(cclp, key, xp);
    否则 もし ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	戻る wildmenu_process_key_filenames(cclp, key, xp);

    戻る key;
}

/*
 * Free expanded names when finished walking through the matches
 */
    void
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    整数 skt = KeyTyped;

    もし (!p_wmnu || wild_menu_showing == 0)
	戻る;

#ifdef FEAT_EVAL
    整数 save_RedrawingDisabled = RedrawingDisabled;
    もし (cclp->input_fn)
	RedrawingDisabled = 0;
#endif

#もし defined(FEAT_SEARCH_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    もし (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    否则 もし (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	last_status(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    否则
    {
	win_redraw_last_status(topframe);
	redraw_statuslines();
    }
    KeyTyped = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    もし (cclp->input_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#もし defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    void
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    整数		filtered = FALSE;
    整数		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	戻る;

    pat = tv_get_string(&argvars[0]);
    もし (check_for_string_arg(argvars, 1) == FAIL)
	戻る;
    type = tv_get_string(&argvars[1]);

    もし (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], NULL);

    もし (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    もし (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    もし (STRCMP(type, "cmdline") == 0)
    {
	整数 cmdline_len = (整数)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (整数)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    否则
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (整数)STRLEN(xpc.xp_pattern);
	xpc.xp_line = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	switch (xpc.xp_context)
	{
	case EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    戻る;

	case EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    もし (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		戻る;
	    }

	    xpc.xp_arg = type + 7;
	    break;

	case EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    もし (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		戻る;
	    }

	    xpc.xp_arg = type + 11;
	    break;

# もし defined(FEAT_MENU)
	case EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (整数)(xpc.xp_pattern - pattern_start);
	    break;
# endif

# ifdef FEAT_CSCOPE
	case EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (整数)(xpc.xp_pattern - pattern_start);
	    break;
# endif

# ifdef FEAT_SIGNS
	case EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (整数)(xpc.xp_pattern - pattern_start);
	    break;
# endif

	case EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (整数)(xpc.xp_pattern - pattern_start);
	    break;

	case EXPAND_SHELLCMDLINE:
	{
	    整数 context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(NULL, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (整数)(xpc.xp_pattern - pattern_start);
	    break;
	}

	case EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    break;

	case EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    break;

	default:
	    break;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	整数	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	もし (unesc != NULL)
	    pat_src = unesc;

	もし (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	否则
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    もし (rettv_list_alloc(rettv) == OK && pat != NULL)
    {
	整数	i;

	ExpandOne(&xpc, pat, NULL, options, WILD_ALL_KEEP);

	ために (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    void
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    整数		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    もし (check_for_string_arg(argvars, 0) == FAIL)
	戻る;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (整数)STRLEN(pat);
    set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_to_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    void
f_cmdcomplete_info(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    整数		    idx;
    整数		    ret = OK;

    もし (rettv_dict_alloc(rettv) == FAIL || ccline == NULL
	    || ccline->xpc == NULL || ccline->xpc->xp_files == NULL)
	戻る;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (整数)cmdline_orig.length);
    もし (ret == OK)
	ret = dict_add_number(retdict, "pum_visible", pum_visible());
    もし (ret == OK)
	ret = dict_add_number(retdict, "selected", ccline->xpc->xp_selected);
    もし (ret == OK)
    {
	li = list_alloc();
	もし (li == NULL)
	    戻る;
	ret = dict_add_list(retdict, "matches", li);
	もし (ret == FAIL)
	    list_unref(li);
	ために (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    static 整数
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*line, *start_line, *end_line;
    整数		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    整数		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != NULL;

    もし (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	戻る FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_line = ml_get(start->lnum);
    char_u  *start_ptr = start_line + start->col;
    整数	    is_single_line = start->lnum == end->lnum;

    segment_len = is_single_line ? (end->col - start->col)
			: (整数)(ml_get_len(start->lnum) - start->col);
    もし (ga_grow(&ga, segment_len + 2) != OK)
	goto fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    もし (!is_single_line)
    {
	もし (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	否则
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    もし (!is_single_line)
    {
	ために (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    整数  linelen;

	    line = ml_get(lnum);
	    linelen = (整数)ml_get_len(lnum);
	    もし (ga_grow(&ga, linelen + 2) != OK)
		goto fail;
	    ga_concat_len(&ga, line, linelen);
	    もし (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    否则
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_line = ml_get(end->lnum);
    word_end = find_word_end(end_line + end->col);
    segment_len = (整数)(word_end - end_line);
    もし (ga_grow(&ga, segment_len) != OK)
	goto fail;
    ga_concat_len(&ga, end_line + (is_single_line ? end->col : 0),
	    segment_len - (is_single_line ? end->col : 0));

    // Null-terminate
    もし (ga_grow(&ga, 1) != OK)
	goto fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    戻る OK;

fail:
    ga_clear(&ga);
    戻る FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    static 整数
is_regex_match(char_u *pat, char_u *str)
{
    もし (STRCMP(pat, str) == 0)
	戻る TRUE;

    regmatch_T	regmatch;
    整数		result;

    ++emsg_off;
    ++msg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --msg_silent;

    もし (regmatch.regprog == NULL)
	戻る FALSE;
    regmatch.rm_ic = p_ic;
    もし (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++msg_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --msg_silent;

    vim_regfree(regmatch.regprog);
    戻る result;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    static char_u *
concat_pattern_with_buffer_match(
	char_u *pat,
	整数 pat_len,
	pos_T *end_match_pos,
	整数 lowercase UNUSED)
{
    char_u  *line = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(line + end_match_pos->col);
    整数	    match_len = (整数)(word_end - (line + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    もし (match == NULL)
	戻る NULL;
    mch_memmove(match, pat, pat_len);
    もし (match_len > 0)
    {
#もし defined(FEAT_EVAL) || defined(FEAT_SPELL)
	もし (lowercase)
	{
	    char_u  *mword = vim_strnsave(line + end_match_pos->col,
		    match_len);
	    もし (mword == NULL)
		goto cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    もし (lower == NULL)
		goto cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	否则
#endif
	    mch_memmove(match + pat_len, line + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    戻る match;

#もし defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    戻る NULL;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    static 整数
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    整数		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    整数		*numMatches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    整数		found_new_match;
    整数		looped_around = FALSE;
    整数		pat_len;
    整数		has_range = FALSE;
    整数		compl_started = FALSE;
    整数		search_flags;
    char_u	*match, *full_match;
    整数		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != NULL;

#ifdef FEAT_SEARCH_EXTRA
    has_range = search_first_line != 0;
#endif

    *matches = NULL;
    *numMatches = 0;

    もし (pat == NULL || *pat == NUL)
	戻る FAIL;

    pat_len = (整数)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_SEARCH_EXTRA
    もし (has_range)
	cur_match_pos.lnum = search_first_line;
    否则
#endif
	cur_match_pos = pre_incsearch_pos;

    search_flags = SEARCH_OPT | SEARCH_NOOF | SEARCH_PEEK | SEARCH_NFMSG
	| (has_range ? SEARCH_START : 0);

    ga_init2(&ga, sizeof(char_u *), 10); // Use growable array of char_u*

    ために (;;)
    {
	++emsg_off;
	++msg_silent;
	found_new_match = searchit(NULL, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		search_flags, RE_LAST, NULL);
	--msg_silent;
	--emsg_off;

	もし (found_new_match == FAIL)
	    break;

#ifdef FEAT_SEARCH_EXTRA
	// If in range mode, check if match is within the range
	もし (has_range && (cur_match_pos.lnum < search_first_line
		    || cur_match_pos.lnum > search_last_line))
		break;
#endif

	もし (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    もし ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		もし (looped_around)
		    break;
		否则
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	もし (char_avail() || got_int)
	{
	    もし (got_int)
	    {
		(void)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    goto cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	もし (end_match_pos.lnum > curbuf->b_ml.ml_line_count)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    continue;
	}

	// Extract the matching text prepended to completed word
	もし (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    break;

	もし (exacttext)
	    match = full_match;
	否则
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    もし (match == NULL || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		もし (match == NULL || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    continue;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	ために (整数 i = 0; i < ga.ga_len; ++i)
	{
	    もし (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		break;
	    }
	}
	もし (match != NULL)
	{
	    もし (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		goto cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    もし (ga.ga_len > TAG_MANY)
		break;
	}
	もし (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *numMatches = ga.ga_len;
    戻る OK;

cleanup:
    ga_clear_strings(&ga);
    戻る FAIL;
}
