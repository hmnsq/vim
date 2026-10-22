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

static ཧྲིལ	cmd_showtail;	// Only show path tail in lists ?
static ཧྲིལ	may_expand_pattern = FALSE;
static pos_T	pre_incsearch_pos; // Cursor position when incsearch started

static void	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, ཧྲིལ usefilter, expand_T *xp, ཧྲིལ *complp);
static ཧྲིལ	ExpandFromContext(expand_T *xp, char_u *, char_u ***, ཧྲིལ *, ཧྲིལ);
static char_u	*showmatches_gettail(char_u *s);
static ཧྲིལ	expand_showtail(expand_T *xp);
static ཧྲིལ	expand_shellcmd(char_u *filepat, char_u ***matches, ཧྲིལ *numMatches, ཧྲིལ flagsarg);
#གལ་ཏེ defined(FEAT_EVAL)
static ཧྲིལ	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, ཧྲིལ *numMatches);
static ཧྲིལ	ExpandUserList(expand_T *xp, char_u ***matches, ཧྲིལ *numMatches);
static char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
static char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, ཧྲིལ srclen, ཧྲིལ *new_lenp);
#endif
static ཧྲིལ	expand_pattern_in_buf(char_u *pat, ཧྲིལ dir, char_u ***matches, ཧྲིལ *numMatches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
static pumitem_T *compl_match_array = NULL;
static ཧྲིལ compl_match_arraysize;
// First column in cmdline of the matched item for completion.
static ཧྲིལ compl_startcol;
static ཧྲིལ compl_selected;
// cmdline before expansion
static string_T cmdline_orig = {NULL, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    static ཧྲིལ
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    switch (xp->xp_context)
    {
    pak_sa EXPAND_BOOL_SETTINGS:
    pak_sa EXPAND_COLORS:
    pak_sa EXPAND_COMPILER:
    pak_sa EXPAND_DIRECTORIES:
    pak_sa EXPAND_DIRS_IN_CDPATH:
    pak_sa EXPAND_FILES:
    pak_sa EXPAND_FILES_IN_PATH:
    pak_sa EXPAND_FILETYPE:
    pak_sa EXPAND_FILETYPECMD:
    pak_sa EXPAND_HELP:
    pak_sa EXPAND_KEYMAP:
    pak_sa EXPAND_OLD_SETTING:
    pak_sa EXPAND_STRING_SETTING:
    pak_sa EXPAND_SETTING_SUBTRACT:
    pak_sa EXPAND_OWNSYNTAX:
    pak_sa EXPAND_PACKADD:
    pak_sa EXPAND_RUNTIME:
    pak_sa EXPAND_SHELLCMD:
    pak_sa EXPAND_SHELLCMDLINE:
    pak_sa EXPAND_TAGS:
    pak_sa EXPAND_TAGS_LISTFILES:
	ཕྱིར FALSE;

    default:
	bha_nga;
    }

    ཕྱིར vim_strchr(p_wop, WOP_FUZZY) != NULL;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    ཧྲིལ
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    ཕྱིར vim_strchr(p_wop, WOP_FUZZY) != NULL && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    static ཧྲིལ
sort_func_compare(const void *s1, const void *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    གལ་ཏེ (*p1 != '<' && *p2 == '<') ཕྱིར -1;
    གལ་ཏེ (*p1 == '<' && *p2 != '<') ཕྱིར 1;
    ཕྱིར STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    static void
wildescape(
    expand_T	*xp,
    char_u	*str,
    ཧྲིལ		numfiles,
    char_u	**files)
{
    char_u	*p;
    ཧྲིལ		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    གལ་ཏེ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	ཆེད་དུ (ཧྲིལ i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    གལ་ཏེ (xp->xp_backslash & XP_BS_THREE)
	    {
		var_na *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		གལ་ཏེ (p != NULL)
		{
		    vim_free(files[i]);
		    files[i] = p;
#གལ་ཏེ defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    གལ་ཏེ (p != NULL)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    དེ་མིན གལ་ཏེ (xp->xp_backslash & XP_BS_COMMA)
	    {
		གལ་ཏེ (vim_strchr(files[i], ',') != NULL)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    གལ་ཏེ (p != NULL)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ifdef BACKSLASH_IN_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#དེ་མིན
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#endif
	    གལ་ཏེ (p != NULL)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    གལ་ཏེ (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	གལ་ཏེ (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    དེ་མིན གལ་ཏེ (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	ཆེད་དུ (ཧྲིལ i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    གལ་ཏེ (p != NULL)
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
    ཧྲིལ		numfiles,
    char_u	**files,
    ཧྲིལ		options)
{
    // May change home directory back to "~"
    གལ་ཏེ (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    གལ་ཏེ (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    ཧྲིལ
nextwild(
    expand_T	*xp,
    ཧྲིལ		type,
    ཧྲིལ		options,	// extra options for ExpandOne()
    ཧྲིལ		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ཧྲིལ		i;
    char_u	*p;
    ཧྲིལ		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    ཧྲིལ		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    གལ་ཏེ (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	གལ་ཏེ (ccline->input_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	དེ་མིན
#endif
	{
	    may_expand_pattern = options & WILD_MAY_EXPAND_PATTERN;
	    set_expand_context(xp);
	    may_expand_pattern = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    གལ་ཏེ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ཕྱིར OK;  // Something illegal on command line
    }
    གལ་ཏེ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ཕྱིར FAIL;
    }

    i = (ཧྲིལ)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    གལ་ཏེ (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	ཕྱིར FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    གལ་ཏེ (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	msg_puts("...");	    // show that we are busy
	out_flush();
    }

    གལ་ཏེ (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, NULL, NULL, 0, type);
    }
    དེ་མིན
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	ཧྲིལ	pat_len = xp->xp_pattern_len;
#གལ་ཏེ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	གལ་ཏེ (unesc != NULL)
	    pat_src = unesc;
#endif

	གལ་ཏེ (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	དེ་མིན
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#གལ་ཏེ defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	གལ་ཏེ (tmp == NULL)
	    p = NULL;
	དེ་མིན
	{
	    ཧྲིལ use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    གལ་ཏེ (escape)
		use_options |= WILD_ESCAPE;
	    གལ་ཏེ (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    གལ་ཏེ (p != NULL && type == WILD_LONGEST)
	    {
		ཧྲིལ	j;

		ཆེད་དུ (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    གལ་ཏེ (c == '*' || c == '?')
			bha_nga;
		}
		གལ་ཏེ ((ཧྲིལ)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    གལ་ཏེ (!wild_navigate && ccline->cmdbuff != NULL)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	གལ་ཏེ (cmdline_orig.string == NULL)
	    cmdline_orig.length = 0;
	དེ་མིན
	    cmdline_orig.length = ccline->cmdlen;
    }

    གལ་ཏེ (p != NULL && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	ཧྲིལ	difflen;
	ཧྲིལ	v = OK;

	difflen = (ཧྲིལ)plen - xp->xp_pattern_len;
	གལ་ཏེ (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	གལ་ཏེ (v == OK)
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
    གལ་ཏེ (xp->xp_context == EXPAND_MAPPINGS && p == NULL)
	ཕྱིར FAIL;

    གལ་ཏེ (xp->xp_numfiles <= 0 && p == NULL)
	beep_flush();
    དེ་མིན གལ་ཏེ (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(void)ExpandOne(xp, NULL, NULL, 0, WILD_FREE);

    vim_free(p);

    ཕྱིར OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    static ཧྲིལ
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	ཧྲིལ		numMatches,
	ཧྲིལ		showtail)
{
    ཧྲིལ	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, numMatches);
    གལ་ཏེ (compl_match_array == NULL)
	ཕྱིར EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = numMatches;
    ཆེད་དུ (ཧྲིལ i = 0; i < numMatches; i++)
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
    གལ་ཏེ (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    ཕྱིར EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    void
cmdline_pum_display(void)
{
    གལ་ཏེ (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    ཧྲིལ
cmdline_pum_active(void)
{
    ཕྱིར pum_visible() && compl_match_array != NULL;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    void
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, ཧྲིལ defer_redraw)
{
    ཧྲིལ	save_KeyTyped = KeyTyped;
#ifdef FEAT_EVAL
    ཧྲིལ	save_RedrawingDisabled = RedrawingDisabled;
    གལ་ཏེ (cclp->input_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    གལ་ཏེ (!defer_redraw)
    {
	ཧྲིལ save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    དེ་མིན
	pum_call_update_screen();
    redrawcmd();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    KeyTyped = save_KeyTyped;
#ifdef FEAT_EVAL
    གལ་ཏེ (cclp->input_fn)
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
    ཧྲིལ
cmdline_compl_startcol(void)
{
    ཕྱིར compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(void)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ཕྱིར xp == NULL ? NULL : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    ཧྲིལ
cmdline_compl_is_fuzzy(void)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ཕྱིར xp != NULL && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    static ཧྲིལ
skip_status_match_char(expand_T *xp, char_u *s)
{
    གལ་ཏེ ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	གལ་ཏེ (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    ཕྱིར 2;
#endif
	ཕྱིར 1;
    }
    ཕྱིར 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    static ཧྲིལ
status_match_len(expand_T *xp, char_u *s)
{
    ཧྲིལ	len = 0;

#ifdef FEAT_MENU
    ཧྲིལ emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    གལ་ཏེ (emenu && menu_is_separator(s))
	ཕྱིར 1;
#endif

    སྐབས (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    ཕྱིར len;
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
    ཧྲིལ		num_matches,
    char_u	**matches,	// list of matches
    ཧྲིལ		match,
    ཧྲིལ		showtail)
{
    ཧྲིལ		row;
    char_u	*buf;
    ཧྲིལ		len;
    ཧྲིལ		clen;		// length in screen cells
    ཧྲིལ		fillchar;
    ཧྲིལ		attr;
    ཧྲིལ		i;
    ཧྲིལ		highlight = TRUE;
    char_u	*selstart = NULL;
    ཧྲིལ		selstart_col = 0;
    char_u	*selend = NULL;
    static ཧྲིལ	first_match = 0;
    ཧྲིལ		add_left = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    ཧྲིལ		emenu;
#endif
    ཧྲིལ		l;

    གལ་ཏེ (matches == NULL)	// interrupted completion?
	ཕྱིར;

    གལ་ཏེ (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    དེ་མིན
	buf = alloc(topframe->fr_width + 1);
    གལ་ཏེ (buf == NULL)
	ཕྱིར;

    གལ་ཏེ (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    གལ་ཏེ (match == 0)
	first_match = 0;
    དེ་མིན གལ་ཏེ (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_left = TRUE;
    }
    དེ་མིན
    {
	// check if match fits on the screen
	ཆེད་དུ (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	གལ་ཏེ (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	གལ་ཏེ (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    ཆེད་དུ (i = match; i < num_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		གལ་ཏེ (clen >= topframe->fr_width)
		    bha_nga;
	    }
	    གལ་ཏེ (i == num_matches)
		add_left = TRUE;
	}
    }
    གལ་ཏེ (add_left)
	སྐབས (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    གལ་ཏེ (clen >= topframe->fr_width)
		bha_nga;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    གལ་ཏེ (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    དེ་མིན
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    སྐབས (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	གལ་ཏེ (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	གལ་ཏེ (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (ཧྲིལ)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	དེ་མིན
#endif
	ཆེད་དུ ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    གལ་ཏེ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    དེ་མིན
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (ཧྲིལ)STRLEN(buf + len);
	    }
	}
	གལ་ཏེ (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	གལ་ཏེ (++i == num_matches)
		bha_nga;
    }

    གལ་ཏེ (i != num_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    གལ་ཏེ (row >= 0)
    {
	གལ་ཏེ (wild_menu_showing == 0)
	{
	    གལ་ཏེ (msg_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		གལ་ཏེ (cmdline_row == Rows - 1)
		{
		    screen_del_lines(0, 0, 1, (ཧྲིལ)Rows, TRUE, 0, NULL);
		    ++msg_scrolled;
		}
		དེ་མིན
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    དེ་མིན
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		གལ་ཏེ (lastwin->w_status_height == 0)
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
	གལ་ཏེ (selstart != NULL && highlight)
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

#གལ་ཏེ defined(FEAT_EVAL)
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

    གལ་ཏེ (str == NULL)
	ཕྱིར NULL;
    གལ་ཏེ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ཕྱིར str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    གལ་ཏེ (p == NULL)
	ཕྱིར str;
    vim_free(str);
    ཕྱིར p;
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
    ཧྲིལ		srclen,
    ཧྲིལ		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    གལ་ཏེ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ཕྱིར NULL;

    buf = alloc(srclen + 1);
    གལ་ཏེ (buf == NULL)
	ཕྱིར NULL;

    d = buf;
    end = src + srclen;
    ཆེད་དུ (p = src; p < end; ++p)
    {
	གལ་ཏེ (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (ཧྲིལ)(d - buf);
    ཕྱིར buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    static char_u *
get_next_or_prev_match(ཧྲིལ mode, expand_T *xp)
{
    ཧྲིལ	    findex = xp->xp_selected;
    ཧྲིལ	    ht;

    // When no matches found, return NULL
    གལ་ཏེ (xp->xp_numfiles <= 0)
	ཕྱིར NULL;

    གལ་ཏེ (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	གལ་ཏེ (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    དེ་མིན གལ་ཏེ (mode == WILD_NEXT)
    {
	// Select the next entry
	++findex;
    }
    དེ་མིན   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	གལ་ཏེ (ht > 3)
	    ht -= 2;

	གལ་ཏེ (mode == WILD_PAGEUP)
	{
	    གལ་ཏེ (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    དེ་མིན གལ་ཏེ (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    དེ་མིན
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	དེ་མིན    // mode == WILD_PAGEDOWN
	{
	    གལ་ཏེ (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    དེ་མིན གལ་ཏེ (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    དེ་མིན
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    གལ་ཏེ (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	གལ་ཏེ (xp->xp_orig != NULL)
	    findex = -1;
	དེ་མིན
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    གལ་ཏེ (p_wmnu)
    {
	གལ་ཏེ (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	དེ་མིན གལ་ཏེ (vim_strchr(p_wop, WOP_PUM) != NULL)
	{
	    གལ་ཏེ (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	དེ་མིན
	    win_redr_status_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    གལ་ཏེ (findex == -1)
	ཕྱིར vim_strsave(xp->xp_orig);
#གལ་ཏེ defined(FEAT_EVAL)
    ཕྱིར apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#དེ་མིན
    ཕྱིར vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    static char_u *
ExpandOne_start(ཧྲིལ mode, expand_T *xp, char_u *str, ཧྲིལ options)
{
    ཧྲིལ		non_suf_match;		// number without matching suffix
    ཧྲིལ		i;
    char_u	*ss = NULL;

    // Do the expansion.
    གལ་ཏེ (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	གལ་ཏེ (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    དེ་མིན གལ་ཏེ (xp->xp_numfiles == 0)
    {
	གལ་ཏེ (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    དེ་མིན
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	གལ་ཏེ (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    གལ་ཏེ (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    དེ་མིན
		non_suf_match = 1;
	    གལ་ཏེ ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		ཆེད་དུ (i = 0; i < 2; ++i)
		    གལ་ཏེ (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    གལ་ཏེ (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		གལ་ཏེ (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		དེ་མིན གལ་ཏེ (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    གལ་ཏེ (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    ཕྱིར ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    static char_u *
find_longest_match(expand_T *xp, ཧྲིལ options)
{
    long_u	len;
    ཧྲིལ		mb_len = 1;
    ཧྲིལ		c0, ci;
    ཧྲིལ		i;
    char_u	*ss;

    ཆེད་དུ (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	གལ་ཏེ (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	དེ་མིན
	    c0 = xp->xp_files[0][len];
	ཆེད་དུ (i = 1; i < xp->xp_numfiles; ++i)
	{
	    གལ་ཏེ (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    དེ་མིན
		ci = xp->xp_files[i][len];
	    གལ་ཏེ (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		གལ་ཏེ (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    bha_nga;
	    }
	    དེ་མིན གལ་ཏེ (c0 != ci)
		bha_nga;
	}
	གལ་ཏེ (i < xp->xp_numfiles)
	{
	    གལ་ཏེ (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    bha_nga;
	}
    }

    ss = alloc(len + 1);
    གལ་ཏེ (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    ཕྱིར ss;
}

    void
free_xp_files_extra(expand_T *xp, ཧྲིལ numfiles)
{
    གལ་ཏེ (xp->xp_files_abbr != NULL)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = NULL;
    }
    གལ་ཏེ (xp->xp_files_kind != NULL)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = NULL;
    }
    གལ་ཏེ (xp->xp_files_menu != NULL)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = NULL;
    }
    གལ་ཏེ (xp->xp_files_info != NULL)
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
    ཧྲིལ		options,
    ཧྲིལ		mode)
{
    char_u	*ss = NULL;
    ཧྲིལ		orig_saved = FALSE;
#གལ་ཏེ defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    ཧྲིལ		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    གལ་ཏེ (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	ཕྱིར get_next_or_prev_match(mode, xp);

    གལ་ཏེ (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    དེ་མིན གལ་ཏེ (mode == WILD_APPLY)
    {
	གལ་ཏེ (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	དེ་མིན
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#གལ་ཏེ defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    གལ་ཏེ (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	གལ་ཏེ (compl_match_array != NULL)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    གལ་ཏེ (mode == WILD_FREE)	// only release file name
	ཕྱིར NULL;

    གལ་ཏེ (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#གལ་ཏེ defined(FEAT_EVAL)
	གལ་ཏེ (ss != NULL)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    གལ་ཏེ (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#གལ་ཏེ defined(FEAT_EVAL)
	གལ་ཏེ (ss != NULL)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    གལ་ཏེ (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	var_na	*prefix = "";
	var_na	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	ཧྲིལ	n = xp->xp_numfiles - 1;
	ཧྲིལ	i;
#གལ་ཏེ defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = NULL;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	གལ་ཏེ ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    གལ་ཏེ (escaped != NULL)
	    {
		ཆེད་དུ (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    གལ་ཏེ (escaped[i] == NULL)
		    {
			སྐབས (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			bha_nga;
		    }
		}
		གལ་ཏེ (escaped != NULL)
		    files = escaped;
	    }
	}
#དེ་མིན
	char_u	**files = xp->xp_files;
#endif

	གལ་ཏེ (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	དེ་མིན གལ་ཏེ (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	ཆེད་དུ (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	གལ་ཏེ (ss != NULL)
	{
	    size_t  ss_len = 0;

	    ཆེད་དུ (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (var_na *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (var_na *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#གལ་ཏེ defined(FEAT_EVAL)
	གལ་ཏེ (escaped != NULL)
	{
	    ཆེད་དུ (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    གལ་ཏེ (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    གལ་ཏེ (!orig_saved)
	vim_free(orig);

#གལ་ཏེ defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    གལ་ཏེ (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    ཕྱིར ss;
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
    གལ་ཏེ (xp->xp_numfiles >= 0)
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
	ཧྲིལ		numMatches,
	ཧྲིལ		lines,
	ཧྲིལ		linenr,
	ཧྲིལ		maxlen,
	ཧྲིལ		showtail,
	ཧྲིལ		dir_attr)
{
    ཧྲིལ		i, j;
    ཧྲིལ		isdir;
    ཧྲིལ		lastlen;
    char_u	*p;

    lastlen = 999;
    ཆེད་དུ (j = linenr; j < numMatches; j += lines)
    {
	གལ་ཏེ (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    msg_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    msg_advance(maxlen + 1);
	    msg_puts((var_na *)p);
	    msg_advance(maxlen + 3);
	    msg_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    bha_nga;
	}
	ཆེད་དུ (i = maxlen - lastlen; --i >= 0; )
	    msg_putchar(' ');
	གལ་ཏེ (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    གལ་ཏེ (xp->xp_numfiles != -1)
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
		གལ་ཏེ (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    དེ་མིན
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    གལ་ཏེ (showtail)
		p = SHOW_MATCH(j);
	    དེ་མིན
	    {
		home_replace(NULL, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	དེ་མིན
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = msg_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    གལ་ཏེ (msg_col > 0)	// when not wrapped around
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
    static ཧྲིལ
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	ཧྲིལ		numMatches,
	ཧྲིལ		showtail,
	ཧྲིལ		noselect)
{
    ཧྲིལ	retval = cmdline_pum_create(ccline, xp, matches, numMatches, showtail);

    གལ་ཏེ (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    ཕྱིར retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    ཧྲིལ
showmatches(
    expand_T	*xp,
    ཧྲིལ		display_wildmenu,
    ཧྲིལ		display_list,
    ཧྲིལ		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ཧྲིལ		numMatches;
    char_u	**matches;
    ཧྲིལ		i;
    ཧྲིལ		maxlen;
    ཧྲིལ		lines;
    ཧྲིལ		columns;
    ཧྲིལ		attr;
    ཧྲིལ		showtail;
    ཧྲིལ		noselect = (wim_flags_arg & WIM_NOSELECT);
    ཧྲིལ		noinsert = (wim_flags_arg & WIM_NOINSERT);
    ཧྲིལ		cmdline_unchanged = noselect || noinsert;

    གལ་ཏེ (xp->xp_numfiles == -1)
    {
	ཧྲིལ retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&numMatches, &matches);
	གལ་ཏེ (retval != EXPAND_OK)
	    ཕྱིར retval;
	showtail = expand_showtail(xp);
    }
    དེ་མིན
    {
	numMatches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    གལ་ཏེ (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != NULL)
	ཕྱིར show_pum_matches(ccline, xp, matches, numMatches,
		showtail && !cmdline_unchanged, noselect);

    གལ་ཏེ (display_list)
    {
	msg_didany = FALSE;		// lines_left will be set
	msg_start();			// prepare for paging
	msg_putchar('\n');
	out_flush();
	cmdline_row = msg_row;
	msg_didany = FALSE;		// lines_left will be set again
	msg_start();			// prepare for paging
    }

    གལ་ཏེ (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    དེ་མིན གལ་ཏེ (display_wildmenu && !display_list)
	win_redr_status_matches(xp, numMatches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    དེ་མིན གལ་ཏེ (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	ཆེད་དུ (i = 0; i < numMatches; ++i)
	{
	    ཧྲིལ	len;
	    གལ་ཏེ (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(NULL, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    དེ་མིན
		len = vim_strsize(SHOW_MATCH(i));
	    གལ་ཏེ (len > maxlen)
		maxlen = len;
	}

	གལ་ཏེ (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    lines = numMatches;
	དེ་མིན
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    གལ་ཏེ (columns < 1)
		columns = 1;
	    lines = (numMatches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	གལ་ཏེ (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    msg_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    msg_clr_eos();
	    msg_advance(maxlen - 3);
	    msg_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	ཆེད་དུ (i = 0; i < lines; ++i)
	{
	    showmatches_oneline(xp, matches, numMatches, lines, i,
						maxlen, showtail, attr);
	    གལ་ཏེ (got_int)
	    {
		got_int = FALSE;
		bha_nga;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = msg_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    གལ་ཏེ (display_wildmenu && display_list)
    {
	གལ་ཏེ (vim_strchr(p_wop, WOP_PUM) != NULL)
	    (void)show_pum_matches(ccline, xp, matches, numMatches,
		    showtail && !cmdline_unchanged, noselect);
	དེ་མིན
	    win_redr_status_matches(xp, numMatches, matches,
		    noselect ? -1 : 0, showtail);
    }

    གལ་ཏེ (xp->xp_numfiles == -1)
    {
	FreeWild(numMatches, matches);
	free_xp_files_extra(xp, numMatches);
    }

    ཕྱིར EXPAND_OK;
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
    ཧྲིལ		had_sep = FALSE;

    ཆེད་དུ (p = s; *p != NUL; )
    {
	གལ་ཏེ (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	དེ་མིན གལ་ཏེ (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    ཕྱིར t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    static ཧྲིལ
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    གལ་ཏེ (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	ཕྱིར FALSE;

    end = gettail(xp->xp_pattern);
    གལ་ཏེ (end == xp->xp_pattern)		// there is no path separator
	ཕྱིར FALSE;

    ཆེད་དུ (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	གལ་ཏེ (rem_backslash(s))
	    ++s;
	དེ་མིན གལ་ཏེ (vim_strchr((char_u *)"*?[", *s) != NULL)
	    ཕྱིར FALSE;
    }
    ཕྱིར TRUE;
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
    ཧྲིལ		len,
    ཧྲིལ		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    ཧྲིལ		i, j;
    ཧྲིལ		new_len;
    char_u	*tail;
    ཧྲིལ		ends_in_star;

    གལ་ཏེ (context != EXPAND_FILES
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
	གལ་ཏེ (context == EXPAND_FINDFUNC
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
	དེ་མིན
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    ཆེད་དུ (i = 0; i < len; i++)
	    {
		གལ་ཏེ (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		གལ་ཏེ (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		གལ་ཏེ ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    གལ་ཏེ (retval != NULL)
	    {
		retval[0] = '^';
		j = 1;
		ཆེད་དུ (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    གལ་ཏེ (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			bha_nga;

		    switch (fname[i])
		    {
			pak_sa '*':   retval[j++] = '.';
				    bha_nga;
			pak_sa '~':   retval[j++] = '\\';
				    bha_nga;
			pak_sa '?':   retval[j] = '.';
				    continue;
			pak_sa '.':   གལ་ཏེ (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    bha_nga;
			pak_sa '\\':  གལ་ཏེ (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    bha_nga;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    དེ་མིན
    {
	retval = alloc(len + 4);
	གལ་ཏེ (retval != NULL)
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
	    ཆེད་དུ (i = len - 2; i >= 0; --i)
	    {
		གལ་ཏེ (retval[i] != '\\')
		    bha_nga;
		ends_in_star = !ends_in_star;
	    }
#endif
	    གལ་ཏེ ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == NULL
		    && vim_strchr(retval, '`') == NULL)
		retval[len++] = '*';
	    དེ་མིན གལ་ཏེ (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    ཕྱིར retval;
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
    གལ་ཏེ ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_search_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_SEARCH_EXTRA
	search_first_line = 0; // Search entire buffer
#endif
	ཕྱིར;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    གལ་ཏེ (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->input_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	ཕྱིར;
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
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, ཧྲིལ *complp)
{
    char_u	*p = NULL;
    ཧྲིལ		len = 0;
    ཧྲིལ		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    གལ་ཏེ (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    དེ་མིན
    {
	p = cmd;
	སྐབས (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	གལ་ཏེ (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    སྐབས (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	གལ་ཏེ (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    སྐབས (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	གལ་ཏེ (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != NULL)
	    ++p;
	len = (ཧྲིལ)(p - cmd);

	གལ་ཏེ (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    ཕྱིར NULL;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	གལ་ཏེ ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    སྐབས (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    གལ་ཏེ (*p == NUL && ASCII_ISALNUM(p[-1]))
	ཕྱིར NULL;

    གལ་ཏེ (eap->cmdidx == CMD_SIZE)
    {
	གལ་ཏེ (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != NULL)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	དེ་མིན གལ་ཏེ (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, NULL, xp, complp);
	    གལ་ཏེ (p == NULL)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    གལ་ཏེ (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ཕྱིར NULL;
    }

    ཕྱིར p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    static void
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	ཧྲིལ		usefilter,
	expand_T	*xp,
	ཧྲིལ		*complp)
{
    char_u	*p;
    ཧྲིལ		c;
    ཧྲིལ		in_quote = FALSE;
    char_u	*bow = NULL;	// Beginning of word
    ཧྲིལ		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    སྐབས (*p != NUL)
    {
	གལ་ཏེ (has_mbyte)
	    c = mb_ptr2char(p);
	དེ་མིན
	    c = *p;
	གལ་ཏེ (c == '\\' && p[1] != NUL)
	    ++p;
	དེ་མིན གལ་ཏེ (c == '`')
	{
	    གལ་ཏེ (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	དེ་མིན གལ་ཏེ (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != NULL && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    སྐབས (*p != NUL)
	    {
		གལ་ཏེ (has_mbyte)
		    c = mb_ptr2char(p);
		དེ་མིན
		    c = *p;
		གལ་ཏེ (c == '`' || vim_isfilec_or_wc(c))
		    bha_nga;
		གལ་ཏེ (has_mbyte)
		    len = (*mb_ptr2len)(p);
		དེ་མིན
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    གལ་ཏེ (in_quote)
		bow = p;
	    དེ་མིན
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    གལ་ཏེ (bow != NULL && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    གལ་ཏེ (usefilter
	    || (eap != NULL
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	གལ་ཏེ (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    གལ་ཏེ (*xp->xp_pattern == '$')
    {
	ཆེད་དུ (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    གལ་ཏེ (!vim_isIDc(*p))
		bha_nga;
	གལ་ཏེ (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    གལ་ཏེ (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    གལ་ཏེ (*xp->xp_pattern == '~')
    {
	ཆེད་དུ (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	གལ་ཏེ (*p == NUL && p > xp->xp_pattern + 1
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
    གལ་ཏེ (p == NULL)
	xp->xp_pattern = arg;
    དེ་མིན
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    ཕྱིར NULL;
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
    གལ་ཏེ (p == NULL)
	xp->xp_pattern = arg;
    དེ་མིན
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    ཕྱིར NULL;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    static char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    གལ་ཏེ (*arg != NUL)
	arg = skip_vimgrep_pat(arg, NULL, NULL);
    གལ་ཏེ (arg == NULL || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	ཕྱིར NULL;
    }
    ཕྱིར skipwhite(arg);
}

#ifdef FEAT_SEARCH_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    static char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    གལ་ཏེ (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	གལ་ཏེ (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    ཕྱིར find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    static char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    ཧྲིལ		delim;

    delim = *arg;	    // get the delimiter
    གལ་ཏེ (delim)
	++arg;		    // skip delimiter if there is one

    སྐབས (arg[0] != NUL && arg[0] != delim)
    {
	གལ་ཏེ (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    གལ་ཏེ (arg[0] != NUL)
	ཕྱིར arg + 1;

    ཕྱིར NULL;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    static char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    ཧྲིལ		delim;

    delim = *arg;
    གལ་ཏེ (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	གལ་ཏེ (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    སྐབས (arg[0] != NUL && arg[0] != delim)
	    {
		གལ་ཏེ (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    གལ་ཏེ (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    སྐབས (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == NULL)
	++arg;
    གལ་ཏེ (arg[0] != NUL)
	ཕྱིར arg;

    ཕྱིར NULL;
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
    གལ་ཏེ (*arg != '/')
	ཕྱིར NULL;

    // Match regexp, not just whole words
    ཆེད་དུ (++arg; *arg && *arg != '/'; arg++)
	གལ་ཏེ (*arg == '\\' && arg[1] != NUL)
	    arg++;
    གལ་ཏེ (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	གལ་ཏེ (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == NULL)
	    xp->xp_context = EXPAND_NOTHING;
	དེ་མིན
	    ཕྱིར arg;
    }

    ཕྱིར NULL;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    static char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    སྐབས ((xp->xp_pattern = vim_strchr(arg, ' ')) != NULL)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    གལ་ཏེ (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    ཕྱིར NULL;
}
#endif

#གལ་ཏེ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    static char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    གལ་ཏེ (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    དེ་མིན
    {
	གལ་ཏེ ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_pattern = skipwhite(p);
	}
	དེ་མིན
	    xp->xp_context = EXPAND_NOTHING;
    }

    ཕྱིར NULL;
}
#endif

static ga_nanaa
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
static ga_nanaa
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

    གལ་ཏེ (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    དེ་མིན གལ་ཏེ (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    དེ་མིན
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    གལ་ཏེ (*p == NUL)
	ཕྱིར NULL;
    subcmd_start = p;

    གལ་ཏེ (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	གལ་ཏེ (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    གལ་ཏེ (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		ཕྱིར NULL;
	    }
	    p = skipwhite(p);
	}
	གལ་ཏེ (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	དེ་མིན
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    དེ་མིན གལ་ཏེ (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    ཕྱིར NULL;
}

    static char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = NULL;

    p = skipwhite(arg);
    གལ་ཏེ (VIM_ISDIGIT(*p))
	ཕྱིར NULL;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    ཕྱིར NULL;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    static char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    ཧྲིལ    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    གལ་ཏེ (*p == NUL)
	ཕྱིར NULL;

    ཆེད་དུ (;;)
    {
	གལ་ཏེ (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    continue;
	}
	གལ་ཏེ (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    continue;
	}
	bha_nga;
    }

    གལ་ཏེ ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    དེ་མིན གལ་ཏེ ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    དེ་མིན གལ་ཏེ ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    ཕྱིར NULL;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    static void
set_context_with_pattern(expand_T *xp)
{
    ཧྲིལ		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_SEARCH_EXTRA
    ཧྲིལ		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    གལ་ཏེ (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	ཕྱིར;
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
	ཧྲིལ		compl,
	ཧྲིལ		forceit)
{
    char_u  *nextcmd;

    switch (cmdidx)
    {
	pak_sa CMD_find:
	pak_sa CMD_sfind:
	pak_sa CMD_tabfind:
	    གལ་ཏེ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    bha_nga;
	pak_sa CMD_cd:
	pak_sa CMD_chdir:
	pak_sa CMD_tcd:
	pak_sa CMD_tchdir:
	pak_sa CMD_lcd:
	pak_sa CMD_lchdir:
	    གལ་ཏེ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    bha_nga;
	pak_sa CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    bha_nga;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	pak_sa CMD_aboveleft:
	pak_sa CMD_argdo:
	pak_sa CMD_belowright:
	pak_sa CMD_botright:
	pak_sa CMD_browse:
	pak_sa CMD_bufdo:
	pak_sa CMD_cdo:
	pak_sa CMD_cfdo:
	pak_sa CMD_confirm:
	pak_sa CMD_debug:
	pak_sa CMD_folddoclosed:
	pak_sa CMD_folddoopen:
	pak_sa CMD_hide:
	pak_sa CMD_horizontal:
	pak_sa CMD_keepalt:
	pak_sa CMD_keepjumps:
	pak_sa CMD_keepmarks:
	pak_sa CMD_keeppatterns:
	pak_sa CMD_ldo:
	pak_sa CMD_leftabove:
	pak_sa CMD_lfdo:
	pak_sa CMD_lockmarks:
	pak_sa CMD_noautocmd:
	pak_sa CMD_noswapfile:
	pak_sa CMD_rightbelow:
	pak_sa CMD_sandbox:
	pak_sa CMD_silent:
	pak_sa CMD_tab:
	pak_sa CMD_tabdo:
	pak_sa CMD_topleft:
	pak_sa CMD_unsilent:
	pak_sa CMD_verbose:
	pak_sa CMD_vertical:
	pak_sa CMD_windo:
	pak_sa CMD_vim9cmd:
	pak_sa CMD_legacy:
	    ཕྱིར arg;

	pak_sa CMD_filter:
	    ཕྱིར set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_SEARCH_EXTRA
	pak_sa CMD_match:
	    ཕྱིར set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	pak_sa CMD_command:
	    ཕྱིར set_context_in_user_cmd(xp, arg);

	pak_sa CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    bha_nga;

	pak_sa CMD_global:
	pak_sa CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    གལ་ཏེ (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    ཕྱིར nextcmd;

	pak_sa CMD_and:
	pak_sa CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    གལ་ཏེ (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    ཕྱིར nextcmd;

	pak_sa CMD_isearch:
	pak_sa CMD_dsearch:
	pak_sa CMD_ilist:
	pak_sa CMD_dlist:
	pak_sa CMD_ijump:
	pak_sa CMD_psearch:
	pak_sa CMD_djump:
	pak_sa CMD_isplit:
	pak_sa CMD_dsplit:
	    ཕྱིར find_cmd_after_isearch_cmd(xp, arg);
	pak_sa CMD_autocmd:
	    ཕྱིར set_context_in_autocmd(xp, arg, FALSE);
	pak_sa CMD_doautocmd:
	pak_sa CMD_doautoall:
	    ཕྱིར set_context_in_autocmd(xp, arg, TRUE);
	pak_sa CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    bha_nga;
	pak_sa CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    bha_nga;
	pak_sa CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    bha_nga;
	pak_sa CMD_tag:
	pak_sa CMD_stag:
	pak_sa CMD_ptag:
	pak_sa CMD_ltag:
	pak_sa CMD_tselect:
	pak_sa CMD_stselect:
	pak_sa CMD_ptselect:
	pak_sa CMD_tjump:
	pak_sa CMD_stjump:
	pak_sa CMD_ptjump:
	    གལ་ཏེ (vim_strchr(p_wop, WOP_TAGFILE) != NULL)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    དེ་མིན
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    bha_nga;
	pak_sa CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    bha_nga;
#ifdef FEAT_SYN_HL
	pak_sa CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    bha_nga;
#endif
#ifdef FEAT_EVAL
	pak_sa CMD_final:
	pak_sa CMD_const:
	pak_sa CMD_let:
	pak_sa CMD_var:
	pak_sa CMD_if:
	pak_sa CMD_elseif:
	pak_sa CMD_while:
	pak_sa CMD_for:
	pak_sa CMD_echo:
	pak_sa CMD_echon:
	pak_sa CMD_execute:
	pak_sa CMD_echomsg:
	pak_sa CMD_echoerr:
	pak_sa CMD_echoconsole:
	pak_sa CMD_echowindow:
	pak_sa CMD_call:
	pak_sa CMD_return:
	pak_sa CMD_cexpr:
	pak_sa CMD_caddexpr:
	pak_sa CMD_cgetexpr:
	pak_sa CMD_lexpr:
	pak_sa CMD_laddexpr:
	pak_sa CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    bha_nga;

	pak_sa CMD_unlet:
	    ཕྱིར set_context_in_unlet_cmd(xp, arg);
	pak_sa CMD_function:
	pak_sa CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    bha_nga;
	pak_sa CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    bha_nga;

	pak_sa CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    bha_nga;
#endif
	pak_sa CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    bha_nga;
#ifdef FEAT_CSCOPE
	pak_sa CMD_cscope:
	pak_sa CMD_lcscope:
	pak_sa CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    bha_nga;
#endif
#ifdef FEAT_SIGNS
	pak_sa CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    bha_nga;
#endif
	pak_sa CMD_bdelete:
	pak_sa CMD_bwipeout:
	pak_sa CMD_bunload:
	    སྐབས ((xp->xp_pattern = vim_strchr(arg, ' ')) != NULL)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	pak_sa CMD_buffer:
	pak_sa CMD_sbuffer:
	pak_sa CMD_pbuffer:
	pak_sa CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    bha_nga;
#ifdef FEAT_DIFF
	pak_sa CMD_diffget:
	pak_sa CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    bha_nga;
#endif
	pak_sa CMD_USER:
	pak_sa CMD_USER_BUF:
	    ཕྱིར set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	pak_sa CMD_map:	    pak_sa CMD_noremap:
	pak_sa CMD_nmap:	    pak_sa CMD_nnoremap:
	pak_sa CMD_vmap:	    pak_sa CMD_vnoremap:
	pak_sa CMD_omap:	    pak_sa CMD_onoremap:
	pak_sa CMD_imap:	    pak_sa CMD_inoremap:
	pak_sa CMD_cmap:	    pak_sa CMD_cnoremap:
	pak_sa CMD_lmap:	    pak_sa CMD_lnoremap:
	pak_sa CMD_smap:	    pak_sa CMD_snoremap:
	pak_sa CMD_tmap:	    pak_sa CMD_tnoremap:
	pak_sa CMD_xmap:	    pak_sa CMD_xnoremap:
	    ཕྱིར set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	pak_sa CMD_unmap:
	pak_sa CMD_nunmap:
	pak_sa CMD_vunmap:
	pak_sa CMD_ounmap:
	pak_sa CMD_iunmap:
	pak_sa CMD_cunmap:
	pak_sa CMD_lunmap:
	pak_sa CMD_sunmap:
	pak_sa CMD_tunmap:
	pak_sa CMD_xunmap:
	    ཕྱིར set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	pak_sa CMD_mapclear:
	pak_sa CMD_nmapclear:
	pak_sa CMD_vmapclear:
	pak_sa CMD_omapclear:
	pak_sa CMD_imapclear:
	pak_sa CMD_cmapclear:
	pak_sa CMD_lmapclear:
	pak_sa CMD_smapclear:
	pak_sa CMD_tmapclear:
	pak_sa CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    bha_nga;

	pak_sa CMD_abbreviate:	pak_sa CMD_noreabbrev:
	pak_sa CMD_cabbrev:	pak_sa CMD_cnoreabbrev:
	pak_sa CMD_iabbrev:	pak_sa CMD_inoreabbrev:
	    ཕྱིར set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	pak_sa CMD_unabbreviate:
	pak_sa CMD_cunabbrev:
	pak_sa CMD_iunabbrev:
	    ཕྱིར set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	pak_sa CMD_menu:	    pak_sa CMD_noremenu:	    pak_sa CMD_unmenu:
	pak_sa CMD_amenu:	    pak_sa CMD_anoremenu:	    pak_sa CMD_aunmenu:
	pak_sa CMD_nmenu:	    pak_sa CMD_nnoremenu:	    pak_sa CMD_nunmenu:
	pak_sa CMD_vmenu:	    pak_sa CMD_vnoremenu:	    pak_sa CMD_vunmenu:
	pak_sa CMD_omenu:	    pak_sa CMD_onoremenu:	    pak_sa CMD_ounmenu:
	pak_sa CMD_imenu:	    pak_sa CMD_inoremenu:	    pak_sa CMD_iunmenu:
	pak_sa CMD_cmenu:	    pak_sa CMD_cnoremenu:	    pak_sa CMD_cunmenu:
	pak_sa CMD_tlmenu:    pak_sa CMD_tlnoremenu:    pak_sa CMD_tlunmenu:
	pak_sa CMD_tmenu:				    pak_sa CMD_tunmenu:
	pak_sa CMD_popup:	    pak_sa CMD_tearoff:	    pak_sa CMD_emenu:
	    ཕྱིར set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	pak_sa CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    bha_nga;

	pak_sa CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    bha_nga;

	pak_sa CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    bha_nga;

	pak_sa CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    bha_nga;

	pak_sa CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    bha_nga;

	pak_sa CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    bha_nga;

#གལ་ཏེ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	pak_sa CMD_language:
	    ཕྱིར set_context_in_lang_cmd(xp, arg);
#endif
#གལ་ཏེ defined(FEAT_PROFILE)
	pak_sa CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    bha_nga;
#endif
	pak_sa CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    bha_nga;

	pak_sa CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    bha_nga;

	pak_sa CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    bha_nga;

	pak_sa CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    bha_nga;
#གལ་ཏེ defined(FEAT_PROFILE)
	pak_sa CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    bha_nga;
#endif

	pak_sa CMD_argdelete:
	    སྐབས ((xp->xp_pattern = vim_strchr(arg, ' ')) != NULL)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    bha_nga;

#ifdef FEAT_EVAL
	pak_sa CMD_breakadd:
	pak_sa CMD_profdel:
	pak_sa CMD_breakdel:
	    ཕྱིར set_context_in_breakadd_cmd(xp, arg, cmdidx);

	pak_sa CMD_scriptnames:
	    ཕྱིར set_context_in_scriptnames_cmd(xp, arg);
#endif
	pak_sa CMD_filetype:
	    ཕྱིར set_context_in_filetype_cmd(xp, arg);

	default:
	    bha_nga;
    }
    ཕྱིར NULL;
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
    ཧྲིལ			len = 0;
    exarg_T		ea;
    ཧྲིལ			compl = EXPAND_NOTHING;
    ཧྲིལ			forceit = FALSE;
    ཧྲིལ			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_line = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    ཆེད་དུ (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != NULL; cmd++)
	;
    xp->xp_pattern = cmd;

    གལ་ཏེ (*cmd == NUL)
	ཕྱིར NULL;
    གལ་ཏེ (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	ཕྱིར NULL;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    གལ་ཏེ (*cmd == NUL)
	ཕྱིར NULL;
    གལ་ཏེ (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	ཕྱིར NULL;
    }

    གལ་ཏེ (*cmd == '|' || *cmd == '\n')
	ཕྱིར cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    གལ་ཏེ (p == NULL)
	ཕྱིར NULL;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    གལ་ཏེ (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    གལ་ཏེ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    གལ་ཏེ ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	སྐབས (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    སྐབས (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    གལ་ཏེ (*p == NUL)
	    {
		གལ་ཏེ (ea.argt & EX_ARGOPT)
		    ཕྱིར set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		གལ་ཏེ (ea.cmdidx == CMD_terminal)
		    ཕྱིར set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    གལ་ཏེ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	གལ་ཏེ (*arg == '>')			// append
	{
	    གལ་ཏེ (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	དེ་མིན གལ་ཏེ (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    གལ་ཏེ (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	གལ་ཏེ (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    གལ་ཏེ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	སྐབས (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    གལ་ཏེ ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	གལ་ཏེ (*arg == NUL)
	    ཕྱིར p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    གལ་ཏེ ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	གལ་ཏེ (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	སྐབས (*p)
	{
	    གལ་ཏེ (*p == Ctrl_V)
	    {
		གལ་ཏེ (p[1] != NUL)
		    ++p;
	    }
	    དེ་མིན གལ་ཏེ ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		གལ་ཏེ (*(p - 1) != '\\')
		{
		    གལ་ཏེ (*p == '|' || *p == '\n')
			ཕྱིར p + 1;
		    ཕྱིར NULL;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    གལ་ཏེ (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == NULL)
	// no arguments allowed but there is something
	ཕྱིར NULL;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (ཧྲིལ)STRLEN(buff);
    སྐབས (*p && p < buff + len)
    {
	གལ་ཏེ (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	དེ་མིན
	{
	    གལ་ཏེ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    གལ་ཏེ (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    ཕྱིར set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    void
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    ཧྲིལ		len,	    // length of command line (excl. NUL)
    ཧྲིལ		col,	    // position of cursor
    ཧྲིལ		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    ཧྲིལ			context;
#endif
    ཧྲིལ		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    གལ་ཏེ (col < len)
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
    གལ་ཏེ (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    དེ་མིན གལ་ཏེ (use_ccline && ccline->input_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	གལ་ཏེ (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(NULL, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    དེ་མིན
#endif
	སྐབས (nextcomm != NULL)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_line = str;
    xp->xp_col = col;

    གལ་ཏེ (col < len)
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
    ཧྲིལ
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    ཧྲིལ		col,		// position of cursor
    ཧྲིལ		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = NULL;
    ཧྲིལ		options = WILD_ADD_SLASH|WILD_SILENT;

    གལ་ཏེ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ཕྱིར EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    གལ་ཏེ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ཕྱིར EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (ཧྲིལ)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	ཧྲིལ	pat_len = xp->xp_pattern_len;
#གལ་ཏེ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	གལ་ཏེ (unesc != NULL)
	    pat_src = unesc;
#endif

	གལ་ཏེ (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	དེ་མིན
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#གལ་ཏེ defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    གལ་ཏེ (file_str == NULL)
	ཕྱིར EXPAND_UNSUCCESSFUL;

    གལ་ཏེ (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    གལ་ཏེ (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = NULL;
    }
    vim_free(file_str);

    ཕྱིར EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    static ཧྲིལ
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	ཧྲིལ		*numMatches,
	ཧྲིལ		flags,
	ཧྲིལ		options)
{
    ཧྲིལ		free_pat = FALSE;
    ཧྲིལ		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    གལ་ཏེ (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	གལ་ཏེ (pat == NULL)
	    ཕྱིར ret;

	pat_end = pat + pat_len;
	ཆེད་དུ (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    གལ་ཏེ (*p != '\\')
		continue;

	    གལ་ཏེ (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    དེ་མིན གལ་ཏེ (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    དེ་མིན གལ་ཏེ (xp->xp_backslash & XP_BS_COMMA)
	    {
		གལ་ཏེ (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		དེ་མིན གལ་ཏེ (*(p + 1) == ',')
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

    གལ་ཏེ (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, numMatches);
#endif
    }
    དེ་མིན
    {
	གལ་ཏེ (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	དེ་མིན གལ་ཏེ (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	དེ་མིན གལ་ཏེ (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	དེ་མིན
	    flags = (flags | EW_DIR) & ~EW_FILE;
	གལ་ཏེ (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, numMatches, matches, flags);
    }
    གལ་ཏེ (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    གལ་ཏེ (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	ཧྲིལ j;

	ཆེད་དུ (j = 0; j < *numMatches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    སྐབས (*ptr != NUL)
	    {
		གལ་ཏེ (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		དེ་མིན གལ་ཏེ (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    ཕྱིར ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    static char_u *
get_behave_arg(expand_T *xp UNUSED, ཧྲིལ idx)
{
    གལ་ཏེ (idx == 0)
	ཕྱིར (char_u *)"mswin";
    གལ་ཏེ (idx == 1)
	ཕྱིར (char_u *)"xterm";
    ཕྱིར NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    static char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, ཧྲིལ idx)
{
    གལ་ཏེ (idx < 0)
	ཕྱིར NULL;

    གལ་ཏེ (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	var_na	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	ཕྱིར (char_u *)opts_all[idx];
    }
    གལ་ཏེ (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	var_na	*opts_plugin[] = {"plugin", "on", "off"};
	ཕྱིར (char_u *)opts_plugin[idx];
    }
    གལ་ཏེ (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	var_na	*opts_indent[] = {"indent", "on", "off"};
	ཕྱིར (char_u *)opts_indent[idx];
    }
    གལ་ཏེ (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	var_na	*opts_onoff[] = {"on", "off"};
	ཕྱིར (char_u *)opts_onoff[idx];
    }
    ཕྱིར NULL;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    static char_u *
get_breakadd_arg(expand_T *xp UNUSED, ཧྲིལ idx)
{
    གལ་ཏེ (idx >= 0 && idx <= 3)
    {
	var_na	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	གལ་ཏེ (breakpt_expand_what == EXP_BREAKPT_ADD)
	    ཕྱིར (char_u *)opts[idx];
	དེ་མིན གལ་ཏེ (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    གལ་ཏེ (idx <= 2)
		ཕྱིར (char_u *)opts[idx + 1];
	}
	དེ་མིན
	{
	    // profdel {func, file}
	    གལ་ཏེ (idx <= 1)
		ཕྱིར (char_u *)opts[idx + 1];
	}
    }
    ཕྱིར NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    static char_u *
get_scriptnames_arg(expand_T *xp UNUSED, ཧྲིལ idx)
{
    scriptitem_T *si;

    གལ་ཏེ (!SCRIPT_ID_VALID(idx + 1))
	ཕྱིར NULL;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(NULL, si->sn_name, NameBuff, MAXPATHL, TRUE);
    ཕྱིར NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    static char_u *
get_retab_arg(expand_T *xp UNUSED, ཧྲིལ idx)
{
    གལ་ཏེ (idx == 0)
	ཕྱིར (char_u *)"-indentonly";
    ཕྱིར NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    static char_u *
get_messages_arg(expand_T *xp UNUSED, ཧྲིལ idx)
{
    གལ་ཏེ (idx == 0)
	ཕྱིར (char_u *)"clear";
    ཕྱིར NULL;
}

    static char_u *
get_mapclear_arg(expand_T *xp UNUSED, ཧྲིལ idx)
{
    གལ་ཏེ (idx == 0)
	ཕྱིར (char_u *)"<buffer>";
    ཕྱིར NULL;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    static ཧྲིལ
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	ཧྲིལ		*numMatches)
{
    static sa_nracanaa expgen
    {
	ཧྲིལ		context;
	char_u	*((*func)(expand_T *, ཧྲིལ));
	ཧྲིལ		ic;
	ཧྲིལ		escaped;
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
#གལ་ཏེ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
    ཧྲིལ	i;
    ཧྲིལ ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    ཆེད་དུ (i = 0; i < (ཧྲིལ)ARRAY_LENGTH(tab); ++i)
    {
	གལ་ཏེ (xp->xp_context == tab[i].context)
	{
	    གལ་ཏེ (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, numMatches,
						tab[i].func, tab[i].escaped);
	    bha_nga;
	}
    }

    ཕྱིར ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    static ཧྲིལ
map_wildopts_to_ewflags(ཧྲིལ options)
{
    ཧྲིལ		flags;

    flags = EW_DIR;	// include directories
    གལ་ཏེ (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    གལ་ཏེ (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    གལ་ཏེ (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    གལ་ཏེ (options & WILD_SILENT)
	flags |= EW_SILENT;
    གལ་ཏེ (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    གལ་ཏེ (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    ཕྱིར flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    static ཧྲིལ
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    ཧྲིལ		*numMatches,
    ཧྲིལ		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    ཧྲིལ		ret;
    ཧྲིལ		flags;
    char_u	*tofree = NULL;
    ཧྲིལ		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    གལ་ཏེ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	ཕྱིར expand_files_and_dirs(xp, pat, matches, numMatches, flags,
								options);

    *matches = (char_u **)"";
    *numMatches = 0;
    གལ་ཏེ (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	གལ་ཏེ (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					numMatches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*numMatches, *matches);
#endif
	    ཕྱིར OK;
	}
	ཕྱིར FAIL;
    }

    གལ་ཏེ (xp->xp_context == EXPAND_SHELLCMD)
	ཕྱིར expand_shellcmd(pat, matches, numMatches, flags);
    གལ་ཏེ (xp->xp_context == EXPAND_OLD_SETTING)
	ཕྱིར ExpandOldSetting(numMatches, matches);
    གལ་ཏེ (xp->xp_context == EXPAND_BUFFERS)
	ཕྱིར ExpandBufnames(pat, numMatches, matches, options);
#ifdef FEAT_DIFF
    གལ་ཏེ (xp->xp_context == EXPAND_DIFF_BUFFERS)
	ཕྱིར ExpandBufnames(pat, numMatches, matches,
						options | BUF_DIFF_FILTER);
#endif
    གལ་ཏེ (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	ཕྱིར expand_tags(xp->xp_context == EXPAND_TAGS, pat, numMatches,
								matches);
    གལ་ཏེ (xp->xp_context == EXPAND_COLORS)
    {
	var_na *directories[] = {"colors", NULL};
	ཕྱིར ExpandRTDir(pat, DIP_START + DIP_OPT, numMatches, matches,
								directories);
    }
    གལ་ཏེ (xp->xp_context == EXPAND_COMPILER)
    {
	var_na *directories[] = {"compiler", NULL};
	ཕྱིར ExpandRTDir(pat, 0, numMatches, matches, directories);
    }
    གལ་ཏེ (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	var_na *directories[] = {"syntax", NULL};
	ཕྱིར ExpandRTDir(pat, 0, numMatches, matches, directories);
    }
    གལ་ཏེ (xp->xp_context == EXPAND_FILETYPE)
    {
	var_na *directories[] = {"syntax", "indent", "ftplugin", NULL};
	ཕྱིར ExpandRTDir(pat, 0, numMatches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    གལ་ཏེ (xp->xp_context == EXPAND_KEYMAP)
    {
	var_na *directories[] = {"keymap", NULL};
	ཕྱིར ExpandRTDir(pat, 0, numMatches, matches, directories);
    }
#endif
#གལ་ཏེ defined(FEAT_EVAL)
    གལ་ཏེ (xp->xp_context == EXPAND_USER_LIST)
	ཕྱིར ExpandUserList(xp, matches, numMatches);
#endif
    གལ་ཏེ (xp->xp_context == EXPAND_PACKADD)
	ཕྱིར ExpandPackAddDir(pat, numMatches, matches);
    གལ་ཏེ (xp->xp_context == EXPAND_RUNTIME)
	ཕྱིར expand_runtime_cmd(pat, numMatches, matches);
    གལ་ཏེ (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	ཕྱིར expand_pattern_in_buf(pat, xp->xp_search_dir,
		matches, numMatches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    གལ་ཏེ ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	ཧྲིལ len = (ཧྲིལ)STRLEN(pat) + 20;

	tofree = alloc(len);
	གལ་ཏེ (tofree == NULL)
	    ཕྱིར FAIL;
	vim_snprintf((var_na *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    གལ་ཏེ (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	གལ་ཏེ (regmatch.regprog == NULL)
	{
	    vim_free(tofree);
	    ཕྱིར FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    གལ་ཏེ (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, numMatches, matches, fuzzy);
    དེ་མིན གལ་ཏེ (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, numMatches, matches);
    དེ་མིན གལ་ཏེ (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, numMatches, matches);
    དེ་མིན གལ་ཏེ (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, numMatches, matches);
    དེ་མིན གལ་ཏེ (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, numMatches);
    དེ་མིན གལ་ཏེ (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, numMatches);
#གལ་ཏེ defined(FEAT_TERMINAL)
    དེ་མིན གལ་ཏེ (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, numMatches);
#endif
#གལ་ཏེ defined(FEAT_EVAL)
    དེ་མིན གལ་ཏེ (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, numMatches);
#endif
    དེ་མིན
	ret = ExpandOther(pat, xp, &regmatch, matches, numMatches);

    གལ་ཏེ (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    ཕྱིར ret;
}

    ཧྲིལ
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ཧྲིལ		*numMatches,
    char_u	*(*func)(expand_T *, ཧྲིལ),
					  // returns a string from the list
    ཧྲིལ		escaped)
{
    ཕྱིར ExpandGenericExt(
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
    ཧྲིལ
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ཧྲིལ		*numMatches,
    char_u	*(*func)(expand_T *, ཧྲིལ),
					  // returns a string from the list
    ཧྲིལ		escaped,
    ཧྲིལ		sortStartIdx)
{
    ཧྲིལ		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = NULL;
    ཧྲིལ		score = 0;
    ཧྲིལ		fuzzy;
    ཧྲིལ		match;
    ཧྲིལ		sort_matches = FALSE;
    ཧྲིལ		funcsort = FALSE;
    ཧྲིལ		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = NULL;
    *numMatches = 0;

    གལ་ཏེ (!fuzzy)
	ga_init2(&ga, parimaa_na(var_na *), 30);
    དེ་མིན
	ga_init2(&ga, parimaa_na(fuzmatch_str_T), 30);

    ཆེད་དུ (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	གལ་ཏེ (str == NULL)	    // end of list
	    bha_nga;
	གལ་ཏེ (*str == NUL)	    // skip empty strings
	    continue;

	གལ་ཏེ (xp->xp_pattern[0] != NUL)
	{
	    གལ་ཏེ (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    དེ་མིན
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	དེ་མིན
	    match = TRUE;

	གལ་ཏེ (!match)
	    continue;

	གལ་ཏེ (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	དེ་མིན
	    str = vim_strsave(str);
	གལ་ཏེ (str == NULL)
	{
	    གལ་ཏེ (!fuzzy)
	    {
		ga_clear_strings(&ga);
		ཕྱིར FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    ཕྱིར FAIL;
	}

	གལ་ཏེ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    bha_nga;
	}

	གལ་ཏེ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	དེ་མིན
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

#ifdef FEAT_MENU
	གལ་ཏེ (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    གལ་ཏེ (*str == '\001')
		*str = '.';
	}
#endif

	གལ་ཏེ (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    གལ་ཏེ (ga.ga_len == 0)
	ཕྱིར OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    གལ་ཏེ (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    གལ་ཏེ (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    གལ་ཏེ (sort_matches && sortStartMatchIdx != -1)
    {
	གལ་ཏེ (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((void *)ga.ga_data, (size_t)ga.ga_len, parimaa_na(char_u *),
							   sort_func_compare);
	དེ་མིན
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    གལ་ཏེ (!fuzzy)
    {
	*matches = ga.ga_data;
	*numMatches = ga.ga_len;
    }
    དེ་མིན
    {
	གལ་ཏེ (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    ཕྱིར FAIL;
	*numMatches = ga.ga_len;
    }

#གལ་ཏེ defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    ཕྱིར OK;
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
	ཧྲིལ		*numMatches,
	ཧྲིལ		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    གལ་ཏེ (expand_wildcards(1, &pathed_pattern, numMatches, matches, flags) != OK)
	ཕྱིར;

    གལ་ཏེ (ga_grow(gap, *numMatches) == FAIL)
    {
	FreeWild(*numMatches, *matches);
	ཕྱིར;
    }

    ཆེད་དུ (ཧྲིལ i = 0; i < *numMatches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	གལ་ཏེ (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    གལ་ཏེ (HASHITEM_EMPTY(hi))
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
    static ཧྲིལ
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    ཧྲིལ		*numMatches,	// return: number of matches
    ཧྲིལ		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = NULL;
    ཧྲིལ		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    ཧྲིལ		flags = flagsarg;
    ཧྲིལ		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    གལ་ཏེ (buf == NULL)
	ཕྱིར FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    གལ་ཏེ (pat == NULL)
    {
	vim_free(buf);
	ཕྱིར FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    ཆེད་དུ (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	གལ་ཏེ (*s != '\\')
	    continue;

	p = s + 1;
	གལ་ཏེ (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    གལ་ཏེ (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    དེ་མིན
    {
	// For an absolute name we don't use $PATH.
	གལ་ཏེ (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	གལ་ཏེ (path == NULL)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, parimaa_na(var_na *), 10);
    hash_init(&found_ht);
    ཆེད་དུ (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	གལ་ཏེ (*s == NUL)
	{
	    གལ་ཏེ (did_curdir)
		bha_nga;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	དེ་མིན
	{
#གལ་ཏེ defined(MSWIN)
	    e = vim_strchr(s, ';');
#དེ་མིན
	    e = vim_strchr(s, ':');
#endif
	    གལ་ཏེ (e == NULL)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    གལ་ཏེ (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    དེ་མིན
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? parimaa_na(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	གལ་ཏེ (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    གལ་ཏེ (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		གལ་ཏེ (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, numMatches, flags,
				    &found_ht, &ga);
	}

	གལ་ཏེ (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_data;
    *numMatches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    གལ་ཏེ (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    ཕྱིར OK;
}

#གལ་ཏེ defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    static void *
call_user_expand_func(
    void	*(*user_expand_func)(char_u *, ཧྲིལ, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ཧྲིལ		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = NULL;
    void	*ret;

    གལ་ཏེ (xp->xp_arg == NULL || xp->xp_arg[0] == '\0' || xp->xp_line == NULL)
	ཕྱིར NULL;

    གལ་ཏེ (ccline->cmdbuff != NULL)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	ཧྲིལ unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	གལ་ཏེ (pat == NULL)
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
    གལ་ཏེ (ccline->cmdbuff != NULL)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    ཕྱིར ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    static ཧྲིལ
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ཧྲིལ		*numMatches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    ཧྲིལ		keep;
    garray_T	ga;
    ཧྲིལ		fuzzy;
    ཧྲིལ		match;
    ཧྲིལ		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = NULL;
    *numMatches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    གལ་ཏེ (retstr == NULL)
	ཕྱིར FAIL;

    གལ་ཏེ (!fuzzy)
	ga_init2(&ga, parimaa_na(var_na *), 3);
    དེ་མིན
	ga_init2(&ga, parimaa_na(fuzmatch_str_T), 3);

    ཆེད་དུ (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	གལ་ཏེ (e == NULL)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	གལ་ཏེ (xp->xp_pattern[0] != NUL)
	{
	    གལ་ཏེ (!fuzzy)
		match = vim_regexec(regmatch, s, (colnr_T)0);
	    དེ་མིན
	    {
		score = fuzzy_match_str(s, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	དེ་མིན
	    match = TRUE;		// match everything

	*e = keep;

	གལ་ཏེ (match)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    གལ་ཏེ (p == NULL)
		bha_nga;

	    གལ་ཏེ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		bha_nga;
	    }

	    གལ་ཏེ (!fuzzy)
		((char_u **)ga.ga_data)[ga.ga_len] = p;
	    དེ་མིན
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	གལ་ཏེ (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    གལ་ཏེ (ga.ga_len == 0)
	ཕྱིར OK;

    གལ་ཏེ (!fuzzy)
    {
	*matches = ga.ga_data;
	*numMatches = ga.ga_len;
    }
    དེ་མིན
    {
	གལ་ཏེ (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    ཕྱིར FAIL;
	*numMatches = ga.ga_len;
    }
    ཕྱིར OK;
}

    void
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    ཧྲིལ		*numMatches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    ཧྲིལ		have_extra = FALSE;
    ཧྲིལ		i;

    ga_init2(&ga, parimaa_na(var_na *), 3);
    ga_init2(&ga_abbr, parimaa_na(var_na *), 3);
    ga_init2(&ga_kind, parimaa_na(var_na *), 3);
    ga_init2(&ga_menu, parimaa_na(var_na *), 3);
    ga_init2(&ga_info, parimaa_na(var_na *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_ITEMS(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = NULL;
	char_u	*abbr = NULL;
	char_u	*kind = NULL;
	char_u	*menu = NULL;
	char_u	*info = NULL;

	གལ་ཏེ (tv->v_type == VAR_STRING)
	{
	    གལ་ཏེ (tv->vval.v_string == NULL)
		continue;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	དེ་མིན གལ་ཏེ (tv->v_type == VAR_DICT && tv->vval.v_dict != NULL)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    གལ་ཏེ (word == NULL)
		continue;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    གལ་ཏེ (abbr != NULL || kind != NULL || menu != NULL || info != NULL)
		have_extra = TRUE;
	}
	དེ་མིན
	    continue;  // Skip other types

	གལ་ཏེ (p == NULL
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
	    bha_nga;
	}

	((char_u **)ga.ga_data)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_data)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_data)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_data)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_data)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_data;
    *numMatches = ga.ga_len;
    གལ་ཏེ (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    དེ་མིན
    {
	// No extra info collected; free the placeholder NULL entries.
	ཆེད་དུ (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	ཆེད་དུ (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	ཆེད་དུ (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	ཆེད་དུ (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    static ཧྲིལ
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    ཧྲིལ		*numMatches)
{
    list_T      *retlist;

    *matches = NULL;
    *numMatches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    གལ་ཏེ (retlist == NULL)
	ཕྱིར FAIL;

    expand_process_user_list(retlist, matches, numMatches, xp);
    list_unref(retlist);
    ཕྱིར OK;
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
    ཧྲིལ		expand_options,
    ཧྲིལ		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    གལ་ཏེ (buf == NULL)
	ཕྱིར;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#གལ་ཏེ defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#དེ་མིན
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    སྐབས (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	གལ་ཏེ (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    ཧྲིལ	    num_p;
	    char_u  **p;

	    གལ་ཏེ (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    གལ་ཏེ (ExpandFromContext(&xpc, buf, &p, &num_p,
			     WILD_SILENT|expand_options) != FAIL && num_p > 0)
	    {
		ExpandEscape(&xpc, buf, num_p, p, WILD_SILENT|expand_options);

		གལ་ཏེ (ga_grow(ga, num_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    ཆེད་དུ (ཧྲིལ i = 0; i < num_p; ++i)
		    {
			((char_u **)ga->ga_data)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		དེ་མིན
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
    ཧྲིལ
wildmenu_translate_key(
	cmdline_info_T	*cclp,
	ཧྲིལ		key,
	expand_T	*xp,
	ཧྲིལ		did_wild_list)
{
    ཧྲིལ c = key;

    གལ་ཏེ (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	switch (c)
	{
	    pak_sa K_UP:	  c = K_LEFT; bha_nga;
	    pak_sa K_DOWN:  c = K_RIGHT; bha_nga;
	    pak_sa K_LEFT:  c = K_UP; bha_nga;
	    pak_sa K_RIGHT: c = K_DOWN; bha_nga;
	    default:	  bha_nga;
	}
    }

    གལ་ཏེ (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	གལ་ཏེ (c == K_LEFT)
	    c = Ctrl_P;
	དེ་མིན གལ་ཏེ (c == K_RIGHT)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    གལ་ཏེ (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    ཕྱིར c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    static void
cmdline_del(cmdline_info_T *cclp, ཧྲིལ from)
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
    static ཧྲིལ
wildmenu_process_key_menunames(cmdline_info_T *cclp, ཧྲིལ key, expand_T *xp)
{
    ཧྲིལ		i;
    ཧྲིལ		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    གལ་ཏེ (key == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	key = p_wc;
	KeyTyped = TRUE;  // in case the key was mapped
    }
    དེ་མིན གལ་ཏེ (key == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	ཧྲིལ found = FALSE;

	j = (ཧྲིལ)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	སྐབས (--j > 0)
	{
	    // check for start of menu name
	    གལ་ཏེ (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		bha_nga;
	    }
	    // check for start of submenu name
	    གལ་ཏེ (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		གལ་ཏེ (found)
		{
		    i = j + 1;
		    bha_nga;
		}
		དེ་མིན
		    found = TRUE;
	    }
	}
	གལ་ཏེ (i > 0)
	    cmdline_del(cclp, i);
	key = p_wc;
	KeyTyped = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    ཕྱིར key;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    static ཧྲིལ
wildmenu_process_key_filenames(cmdline_info_T *cclp, ཧྲིལ key, expand_T *xp)
{
    ཧྲིལ		i;
    ཧྲིལ		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    གལ་ཏེ (key == K_DOWN
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
    དེ་མིན གལ་ཏེ (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && key == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	ཧྲིལ found = FALSE;

	j = cclp->cmdpos;
	i = (ཧྲིལ)(xp->xp_pattern - cclp->cmdbuff);
	སྐབས (--j > i)
	{
	    གལ་ཏེ (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    གལ་ཏེ (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		bha_nga;
	    }
	}
	གལ་ཏེ (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    key = p_wc;
	    KeyTyped = TRUE;  // in case the key was mapped
	}
    }
    དེ་མིན གལ་ཏེ (key == K_UP)
    {
	// go up a directory
	ཧྲིལ found = FALSE;

	j = cclp->cmdpos - 1;
	i = (ཧྲིལ)(xp->xp_pattern - cclp->cmdbuff);
	སྐབས (--j > i)
	{
	    གལ་ཏེ (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    གལ་ཏེ (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == NULL
#endif
	       )
	    {
		གལ་ཏེ (found)
		{
		    i = j + 1;
		    bha_nga;
		}
		དེ་མིན
		    found = TRUE;
	    }
	}

	གལ་ཏེ (!found)
	    j = i;
	དེ་མིན གལ་ཏེ (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	དེ་མིན གལ་ཏེ (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	དེ་མིན
	    j = 0;
	གལ་ཏེ (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	དེ་མིན གལ་ཏེ (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	key = p_wc;
	KeyTyped = TRUE;
    }

    ཕྱིར key;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    ཧྲིལ
wildmenu_process_key(cmdline_info_T *cclp, ཧྲིལ key, expand_T *xp)
{
    གལ་ཏེ (xp->xp_context == EXPAND_MENUNAMES)
	ཕྱིར wildmenu_process_key_menunames(cclp, key, xp);
    དེ་མིན གལ་ཏེ ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	ཕྱིར wildmenu_process_key_filenames(cclp, key, xp);

    ཕྱིར key;
}

/*
 * Free expanded names when finished walking through the matches
 */
    void
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    ཧྲིལ skt = KeyTyped;

    གལ་ཏེ (!p_wmnu || wild_menu_showing == 0)
	ཕྱིར;

#ifdef FEAT_EVAL
    ཧྲིལ save_RedrawingDisabled = RedrawingDisabled;
    གལ་ཏེ (cclp->input_fn)
	RedrawingDisabled = 0;
#endif

#གལ་ཏེ defined(FEAT_SEARCH_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    གལ་ཏེ (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    དེ་མིན གལ་ཏེ (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	last_status(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    དེ་མིན
    {
	win_redraw_last_status(topframe);
	redraw_statuslines();
    }
    KeyTyped = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    གལ་ཏེ (cclp->input_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#གལ་ཏེ defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    void
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    ཧྲིལ		filtered = FALSE;
    ཧྲིལ		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    གལ་ཏེ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ཕྱིར;

    pat = tv_get_string(&argvars[0]);
    གལ་ཏེ (check_for_string_arg(argvars, 1) == FAIL)
	ཕྱིར;
    type = tv_get_string(&argvars[1]);

    གལ་ཏེ (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], NULL);

    གལ་ཏེ (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    གལ་ཏེ (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    གལ་ཏེ (STRCMP(type, "cmdline") == 0)
    {
	ཧྲིལ cmdline_len = (ཧྲིལ)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (ཧྲིལ)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    དེ་མིན
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (ཧྲིལ)STRLEN(xpc.xp_pattern);
	xpc.xp_line = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	switch (xpc.xp_context)
	{
	pak_sa EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    ཕྱིར;

	pak_sa EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    གལ་ཏེ (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		ཕྱིར;
	    }

	    xpc.xp_arg = type + 7;
	    bha_nga;

	pak_sa EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    གལ་ཏེ (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		ཕྱིར;
	    }

	    xpc.xp_arg = type + 11;
	    bha_nga;

# གལ་ཏེ defined(FEAT_MENU)
	pak_sa EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (ཧྲིལ)(xpc.xp_pattern - pattern_start);
	    bha_nga;
# endif

# ifdef FEAT_CSCOPE
	pak_sa EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (ཧྲིལ)(xpc.xp_pattern - pattern_start);
	    bha_nga;
# endif

# ifdef FEAT_SIGNS
	pak_sa EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (ཧྲིལ)(xpc.xp_pattern - pattern_start);
	    bha_nga;
# endif

	pak_sa EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (ཧྲིལ)(xpc.xp_pattern - pattern_start);
	    bha_nga;

	pak_sa EXPAND_SHELLCMDLINE:
	{
	    ཧྲིལ context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(NULL, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (ཧྲིལ)(xpc.xp_pattern - pattern_start);
	    bha_nga;
	}

	pak_sa EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    bha_nga;

	pak_sa EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    bha_nga;

	default:
	    bha_nga;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	ཧྲིལ	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	གལ་ཏེ (unesc != NULL)
	    pat_src = unesc;

	གལ་ཏེ (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	དེ་མིན
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    གལ་ཏེ (rettv_list_alloc(rettv) == OK && pat != NULL)
    {
	ཧྲིལ	i;

	ExpandOne(&xpc, pat, NULL, options, WILD_ALL_KEEP);

	ཆེད་དུ (i = 0; i < xpc.xp_numfiles; i++)
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
    ཧྲིལ		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    གལ་ཏེ (check_for_string_arg(argvars, 0) == FAIL)
	ཕྱིར;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (ཧྲིལ)STRLEN(pat);
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
    ཧྲིལ		    idx;
    ཧྲིལ		    ret = OK;

    གལ་ཏེ (rettv_dict_alloc(rettv) == FAIL || ccline == NULL
	    || ccline->xpc == NULL || ccline->xpc->xp_files == NULL)
	ཕྱིར;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (ཧྲིལ)cmdline_orig.length);
    གལ་ཏེ (ret == OK)
	ret = dict_add_number(retdict, "pum_visible", pum_visible());
    གལ་ཏེ (ret == OK)
	ret = dict_add_number(retdict, "selected", ccline->xpc->xp_selected);
    གལ་ཏེ (ret == OK)
    {
	li = list_alloc();
	གལ་ཏེ (li == NULL)
	    ཕྱིར;
	ret = dict_add_list(retdict, "matches", li);
	གལ་ཏེ (ret == FAIL)
	    list_unref(li);
	ཆེད་དུ (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    static ཧྲིལ
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*line, *start_line, *end_line;
    ཧྲིལ		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    ཧྲིལ		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != NULL;

    གལ་ཏེ (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	ཕྱིར FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_line = ml_get(start->lnum);
    char_u  *start_ptr = start_line + start->col;
    ཧྲིལ	    is_single_line = start->lnum == end->lnum;

    segment_len = is_single_line ? (end->col - start->col)
			: (ཧྲིལ)(ml_get_len(start->lnum) - start->col);
    གལ་ཏེ (ga_grow(&ga, segment_len + 2) != OK)
	goto fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    གལ་ཏེ (!is_single_line)
    {
	གལ་ཏེ (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	དེ་མིན
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    གལ་ཏེ (!is_single_line)
    {
	ཆེད་དུ (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    ཧྲིལ  linelen;

	    line = ml_get(lnum);
	    linelen = (ཧྲིལ)ml_get_len(lnum);
	    གལ་ཏེ (ga_grow(&ga, linelen + 2) != OK)
		goto fail;
	    ga_concat_len(&ga, line, linelen);
	    གལ་ཏེ (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    དེ་མིན
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_line = ml_get(end->lnum);
    word_end = find_word_end(end_line + end->col);
    segment_len = (ཧྲིལ)(word_end - end_line);
    གལ་ཏེ (ga_grow(&ga, segment_len) != OK)
	goto fail;
    ga_concat_len(&ga, end_line + (is_single_line ? end->col : 0),
	    segment_len - (is_single_line ? end->col : 0));

    // Null-terminate
    གལ་ཏེ (ga_grow(&ga, 1) != OK)
	goto fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    ཕྱིར OK;

fail:
    ga_clear(&ga);
    ཕྱིར FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    static ཧྲིལ
is_regex_match(char_u *pat, char_u *str)
{
    གལ་ཏེ (STRCMP(pat, str) == 0)
	ཕྱིར TRUE;

    regmatch_T	regmatch;
    ཧྲིལ		result;

    ++emsg_off;
    ++msg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --msg_silent;

    གལ་ཏེ (regmatch.regprog == NULL)
	ཕྱིར FALSE;
    regmatch.rm_ic = p_ic;
    གལ་ཏེ (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++msg_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --msg_silent;

    vim_regfree(regmatch.regprog);
    ཕྱིར result;
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
	ཧྲིལ pat_len,
	pos_T *end_match_pos,
	ཧྲིལ lowercase UNUSED)
{
    char_u  *line = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(line + end_match_pos->col);
    ཧྲིལ	    match_len = (ཧྲིལ)(word_end - (line + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    གལ་ཏེ (match == NULL)
	ཕྱིར NULL;
    mch_memmove(match, pat, pat_len);
    གལ་ཏེ (match_len > 0)
    {
#གལ་ཏེ defined(FEAT_EVAL) || defined(FEAT_SPELL)
	གལ་ཏེ (lowercase)
	{
	    char_u  *mword = vim_strnsave(line + end_match_pos->col,
		    match_len);
	    གལ་ཏེ (mword == NULL)
		goto cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    གལ་ཏེ (lower == NULL)
		goto cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	དེ་མིན
#endif
	    mch_memmove(match + pat_len, line + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    ཕྱིར match;

#གལ་ཏེ defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    ཕྱིར NULL;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    static ཧྲིལ
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    ཧྲིལ		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    ཧྲིལ		*numMatches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    ཧྲིལ		found_new_match;
    ཧྲིལ		looped_around = FALSE;
    ཧྲིལ		pat_len;
    ཧྲིལ		has_range = FALSE;
    ཧྲིལ		compl_started = FALSE;
    ཧྲིལ		search_flags;
    char_u	*match, *full_match;
    ཧྲིལ		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != NULL;

#ifdef FEAT_SEARCH_EXTRA
    has_range = search_first_line != 0;
#endif

    *matches = NULL;
    *numMatches = 0;

    གལ་ཏེ (pat == NULL || *pat == NUL)
	ཕྱིར FAIL;

    pat_len = (ཧྲིལ)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_SEARCH_EXTRA
    གལ་ཏེ (has_range)
	cur_match_pos.lnum = search_first_line;
    དེ་མིན
#endif
	cur_match_pos = pre_incsearch_pos;

    search_flags = SEARCH_OPT | SEARCH_NOOF | SEARCH_PEEK | SEARCH_NFMSG
	| (has_range ? SEARCH_START : 0);

    ga_init2(&ga, parimaa_na(char_u *), 10); // Use growable array of char_u*

    ཆེད་དུ (;;)
    {
	++emsg_off;
	++msg_silent;
	found_new_match = searchit(NULL, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		search_flags, RE_LAST, NULL);
	--msg_silent;
	--emsg_off;

	གལ་ཏེ (found_new_match == FAIL)
	    bha_nga;

#ifdef FEAT_SEARCH_EXTRA
	// If in range mode, check if match is within the range
	གལ་ཏེ (has_range && (cur_match_pos.lnum < search_first_line
		    || cur_match_pos.lnum > search_last_line))
		bha_nga;
#endif

	གལ་ཏེ (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    གལ་ཏེ ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		གལ་ཏེ (looped_around)
		    bha_nga;
		དེ་མིན
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	གལ་ཏེ (char_avail() || got_int)
	{
	    གལ་ཏེ (got_int)
	    {
		(void)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    goto cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	གལ་ཏེ (end_match_pos.lnum > curbuf->b_ml.ml_line_count)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    continue;
	}

	// Extract the matching text prepended to completed word
	གལ་ཏེ (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    bha_nga;

	གལ་ཏེ (exacttext)
	    match = full_match;
	དེ་མིན
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    གལ་ཏེ (match == NULL || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		གལ་ཏེ (match == NULL || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    continue;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	ཆེད་དུ (ཧྲིལ i = 0; i < ga.ga_len; ++i)
	{
	    གལ་ཏེ (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		bha_nga;
	    }
	}
	གལ་ཏེ (match != NULL)
	{
	    གལ་ཏེ (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		goto cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    གལ་ཏེ (ga.ga_len > TAG_MANY)
		bha_nga;
	}
	གལ་ཏེ (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *numMatches = ga.ga_len;
    ཕྱིར OK;

cleanup:
    ga_clear_strings(&ga);
    ཕྱིར FAIL;
}
