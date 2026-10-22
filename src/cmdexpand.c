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

static puur_naa_nka	cmd_showtail;	// Only show path tail in lists ?
static puur_naa_nka	may_expand_pattern = FALSE;
static pos_T	pre_incsearch_pos; // Cursor position when incsearch started

static void	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, puur_naa_nka usefilter, expand_T *xp, puur_naa_nka *complp);
static puur_naa_nka	ExpandFromContext(expand_T *xp, char_u *, char_u ***, puur_naa_nka *, puur_naa_nka);
static char_u	*showmatches_gettail(char_u *s);
static puur_naa_nka	expand_showtail(expand_T *xp);
static puur_naa_nka	expand_shellcmd(char_u *filepat, char_u ***matches, puur_naa_nka *numMatches, puur_naa_nka flagsarg);
#ᯀᯮᯝ defined(FEAT_EVAL)
static puur_naa_nka	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, puur_naa_nka *numMatches);
static puur_naa_nka	ExpandUserList(expand_T *xp, char_u ***matches, puur_naa_nka *numMatches);
static char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
static char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, puur_naa_nka srclen, puur_naa_nka *new_lenp);
#endif
static puur_naa_nka	expand_pattern_in_buf(char_u *pat, puur_naa_nka dir, char_u ***matches, puur_naa_nka *numMatches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
static pumitem_T *compl_match_array = NULL;
static puur_naa_nka compl_match_arraysize;
// First column in cmdline of the matched item for completion.
static puur_naa_nka compl_startcol;
static puur_naa_nka compl_selected;
// cmdline before expansion
static string_T cmdline_orig = {NULL, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    static puur_naa_nka
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
	ᯀᯮᯞ FALSE;

    default:
	bha_nga;
    }

    ᯀᯮᯞ vim_strchr(p_wop, WOP_FUZZY) != NULL;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    puur_naa_nka
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    ᯀᯮᯞ vim_strchr(p_wop, WOP_FUZZY) != NULL && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    static puur_naa_nka
sort_func_compare(const void *s1, const void *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    ᯀᯮᯝ (*p1 != '<' && *p2 == '<') ᯀᯮᯞ -1;
    ᯀᯮᯝ (*p1 == '<' && *p2 != '<') ᯀᯮᯞ 1;
    ᯀᯮᯞ STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    static void
wildescape(
    expand_T	*xp,
    char_u	*str,
    puur_naa_nka		numfiles,
    char_u	**files)
{
    char_u	*p;
    puur_naa_nka		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    ᯀᯮᯝ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	for (puur_naa_nka i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    ᯀᯮᯝ (xp->xp_backslash & XP_BS_THREE)
	    {
		var_na *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		ᯀᯮᯝ (p != NULL)
		{
		    vim_free(files[i]);
		    files[i] = p;
#ᯀᯮᯝ defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    ᯀᯮᯝ (p != NULL)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_backslash & XP_BS_COMMA)
	    {
		ᯀᯮᯝ (vim_strchr(files[i], ',') != NULL)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    ᯀᯮᯝ (p != NULL)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ifdef BACKSLASH_IN_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#ᯀᯮᯝᯂᯪ
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#endif
	    ᯀᯮᯝ (p != NULL)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    ᯀᯮᯝ (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	ᯀᯮᯝ (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	for (puur_naa_nka i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    ᯀᯮᯝ (p != NULL)
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
    puur_naa_nka		numfiles,
    char_u	**files,
    puur_naa_nka		options)
{
    // May change home directory back to "~"
    ᯀᯮᯝ (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    ᯀᯮᯝ (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    puur_naa_nka
nextwild(
    expand_T	*xp,
    puur_naa_nka		type,
    puur_naa_nka		options,	// extra options for ExpandOne()
    puur_naa_nka		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    puur_naa_nka		i;
    char_u	*p;
    puur_naa_nka		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    puur_naa_nka		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    ᯀᯮᯝ (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	ᯀᯮᯝ (ccline->input_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	ᯀᯮᯝᯂᯪ
#endif
	{
	    may_expand_pattern = options & WILD_MAY_EXPAND_PATTERN;
	    set_expand_context(xp);
	    may_expand_pattern = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    ᯀᯮᯝ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ᯀᯮᯞ OK;  // Something illegal on command line
    }
    ᯀᯮᯝ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ᯀᯮᯞ FAIL;
    }

    i = (puur_naa_nka)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    ᯀᯮᯝ (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	ᯀᯮᯞ FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    ᯀᯮᯝ (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	msg_puts("...");	    // show that we are busy
	out_flush();
    }

    ᯀᯮᯝ (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, NULL, NULL, 0, type);
    }
    ᯀᯮᯝᯂᯪ
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	puur_naa_nka	pat_len = xp->xp_pattern_len;
#ᯀᯮᯝ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	ᯀᯮᯝ (unesc != NULL)
	    pat_src = unesc;
#endif

	ᯀᯮᯝ (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	ᯀᯮᯝᯂᯪ
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#ᯀᯮᯝ defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	ᯀᯮᯝ (tmp == NULL)
	    p = NULL;
	ᯀᯮᯝᯂᯪ
	{
	    puur_naa_nka use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    ᯀᯮᯝ (escape)
		use_options |= WILD_ESCAPE;
	    ᯀᯮᯝ (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    ᯀᯮᯝ (p != NULL && type == WILD_LONGEST)
	    {
		puur_naa_nka	j;

		for (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    ᯀᯮᯝ (c == '*' || c == '?')
			bha_nga;
		}
		ᯀᯮᯝ ((puur_naa_nka)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    ᯀᯮᯝ (!wild_navigate && ccline->cmdbuff != NULL)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	ᯀᯮᯝ (cmdline_orig.string == NULL)
	    cmdline_orig.length = 0;
	ᯀᯮᯝᯂᯪ
	    cmdline_orig.length = ccline->cmdlen;
    }

    ᯀᯮᯝ (p != NULL && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	puur_naa_nka	difflen;
	puur_naa_nka	v = OK;

	difflen = (puur_naa_nka)plen - xp->xp_pattern_len;
	ᯀᯮᯝ (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	ᯀᯮᯝ (v == OK)
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
    ᯀᯮᯝ (xp->xp_context == EXPAND_MAPPINGS && p == NULL)
	ᯀᯮᯞ FAIL;

    ᯀᯮᯝ (xp->xp_numfiles <= 0 && p == NULL)
	beep_flush();
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(void)ExpandOne(xp, NULL, NULL, 0, WILD_FREE);

    vim_free(p);

    ᯀᯮᯞ OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    static puur_naa_nka
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	puur_naa_nka		numMatches,
	puur_naa_nka		showtail)
{
    puur_naa_nka	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, numMatches);
    ᯀᯮᯝ (compl_match_array == NULL)
	ᯀᯮᯞ EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = numMatches;
    for (puur_naa_nka i = 0; i < numMatches; i++)
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
    ᯀᯮᯝ (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    ᯀᯮᯞ EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    void
cmdline_pum_display(void)
{
    ᯀᯮᯝ (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    puur_naa_nka
cmdline_pum_active(void)
{
    ᯀᯮᯞ pum_visible() && compl_match_array != NULL;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    void
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, puur_naa_nka defer_redraw)
{
    puur_naa_nka	save_KeyTyped = KeyTyped;
#ifdef FEAT_EVAL
    puur_naa_nka	save_RedrawingDisabled = RedrawingDisabled;
    ᯀᯮᯝ (cclp->input_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    ᯀᯮᯝ (!defer_redraw)
    {
	puur_naa_nka save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    ᯀᯮᯝᯂᯪ
	pum_call_update_screen();
    redrawcmd();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    KeyTyped = save_KeyTyped;
#ifdef FEAT_EVAL
    ᯀᯮᯝ (cclp->input_fn)
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
    puur_naa_nka
cmdline_compl_startcol(void)
{
    ᯀᯮᯞ compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(void)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ᯀᯮᯞ xp == NULL ? NULL : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    puur_naa_nka
cmdline_compl_is_fuzzy(void)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ᯀᯮᯞ xp != NULL && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    static puur_naa_nka
skip_status_match_char(expand_T *xp, char_u *s)
{
    ᯀᯮᯝ ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	ᯀᯮᯝ (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    ᯀᯮᯞ 2;
#endif
	ᯀᯮᯞ 1;
    }
    ᯀᯮᯞ 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    static puur_naa_nka
status_match_len(expand_T *xp, char_u *s)
{
    puur_naa_nka	len = 0;

#ifdef FEAT_MENU
    puur_naa_nka emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    ᯀᯮᯝ (emenu && menu_is_separator(s))
	ᯀᯮᯞ 1;
#endif

    ᯀᯬᯝ (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    ᯀᯮᯞ len;
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
    puur_naa_nka		num_matches,
    char_u	**matches,	// list of matches
    puur_naa_nka		match,
    puur_naa_nka		showtail)
{
    puur_naa_nka		row;
    char_u	*buf;
    puur_naa_nka		len;
    puur_naa_nka		clen;		// length in screen cells
    puur_naa_nka		fillchar;
    puur_naa_nka		attr;
    puur_naa_nka		i;
    puur_naa_nka		highlight = TRUE;
    char_u	*selstart = NULL;
    puur_naa_nka		selstart_col = 0;
    char_u	*selend = NULL;
    static puur_naa_nka	first_match = 0;
    puur_naa_nka		add_left = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    puur_naa_nka		emenu;
#endif
    puur_naa_nka		l;

    ᯀᯮᯝ (matches == NULL)	// interrupted completion?
	ᯀᯮᯞ;

    ᯀᯮᯝ (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    ᯀᯮᯝᯂᯪ
	buf = alloc(topframe->fr_width + 1);
    ᯀᯮᯝ (buf == NULL)
	ᯀᯮᯞ;

    ᯀᯮᯝ (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    ᯀᯮᯝ (match == 0)
	first_match = 0;
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_left = TRUE;
    }
    ᯀᯮᯝᯂᯪ
    {
	// check if match fits on the screen
	for (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	ᯀᯮᯝ (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	ᯀᯮᯝ (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    for (i = match; i < num_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		ᯀᯮᯝ (clen >= topframe->fr_width)
		    bha_nga;
	    }
	    ᯀᯮᯝ (i == num_matches)
		add_left = TRUE;
	}
    }
    ᯀᯮᯝ (add_left)
	ᯀᯬᯝ (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    ᯀᯮᯝ (clen >= topframe->fr_width)
		bha_nga;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    ᯀᯮᯝ (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    ᯀᯮᯝᯂᯪ
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    ᯀᯬᯝ (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	ᯀᯮᯝ (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	ᯀᯮᯝ (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (puur_naa_nka)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	ᯀᯮᯝᯂᯪ
#endif
	for ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    ᯀᯮᯝ (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    ᯀᯮᯝᯂᯪ
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (puur_naa_nka)STRLEN(buf + len);
	    }
	}
	ᯀᯮᯝ (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	ᯀᯮᯝ (++i == num_matches)
		bha_nga;
    }

    ᯀᯮᯝ (i != num_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    ᯀᯮᯝ (row >= 0)
    {
	ᯀᯮᯝ (wild_menu_showing == 0)
	{
	    ᯀᯮᯝ (msg_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		ᯀᯮᯝ (cmdline_row == Rows - 1)
		{
		    screen_del_lines(0, 0, 1, (puur_naa_nka)Rows, TRUE, 0, NULL);
		    ++msg_scrolled;
		}
		ᯀᯮᯝᯂᯪ
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    ᯀᯮᯝᯂᯪ
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		ᯀᯮᯝ (lastwin->w_status_height == 0)
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
	ᯀᯮᯝ (selstart != NULL && highlight)
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

#ᯀᯮᯝ defined(FEAT_EVAL)
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

    ᯀᯮᯝ (str == NULL)
	ᯀᯮᯞ NULL;
    ᯀᯮᯝ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ᯀᯮᯞ str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    ᯀᯮᯝ (p == NULL)
	ᯀᯮᯞ str;
    vim_free(str);
    ᯀᯮᯞ p;
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
    puur_naa_nka		srclen,
    puur_naa_nka		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    ᯀᯮᯝ ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ᯀᯮᯞ NULL;

    buf = alloc(srclen + 1);
    ᯀᯮᯝ (buf == NULL)
	ᯀᯮᯞ NULL;

    d = buf;
    end = src + srclen;
    for (p = src; p < end; ++p)
    {
	ᯀᯮᯝ (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (puur_naa_nka)(d - buf);
    ᯀᯮᯞ buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    static char_u *
get_next_or_prev_match(puur_naa_nka mode, expand_T *xp)
{
    puur_naa_nka	    findex = xp->xp_selected;
    puur_naa_nka	    ht;

    // When no matches found, return NULL
    ᯀᯮᯝ (xp->xp_numfiles <= 0)
	ᯀᯮᯞ NULL;

    ᯀᯮᯝ (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	ᯀᯮᯝ (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (mode == WILD_NEXT)
    {
	// Select the next entry
	++findex;
    }
    ᯀᯮᯝᯂᯪ   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	ᯀᯮᯝ (ht > 3)
	    ht -= 2;

	ᯀᯮᯝ (mode == WILD_PAGEUP)
	{
	    ᯀᯮᯝ (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    ᯀᯮᯝᯂᯪ
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	ᯀᯮᯝᯂᯪ    // mode == WILD_PAGEDOWN
	{
	    ᯀᯮᯝ (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    ᯀᯮᯝᯂᯪ
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    ᯀᯮᯝ (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	ᯀᯮᯝ (xp->xp_orig != NULL)
	    findex = -1;
	ᯀᯮᯝᯂᯪ
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    ᯀᯮᯝ (p_wmnu)
    {
	ᯀᯮᯝ (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (vim_strchr(p_wop, WOP_PUM) != NULL)
	{
	    ᯀᯮᯝ (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	ᯀᯮᯝᯂᯪ
	    win_redr_status_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    ᯀᯮᯝ (findex == -1)
	ᯀᯮᯞ vim_strsave(xp->xp_orig);
#ᯀᯮᯝ defined(FEAT_EVAL)
    ᯀᯮᯞ apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#ᯀᯮᯝᯂᯪ
    ᯀᯮᯞ vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    static char_u *
ExpandOne_start(puur_naa_nka mode, expand_T *xp, char_u *str, puur_naa_nka options)
{
    puur_naa_nka		non_suf_match;		// number without matching suffix
    puur_naa_nka		i;
    char_u	*ss = NULL;

    // Do the expansion.
    ᯀᯮᯝ (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	ᯀᯮᯝ (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_numfiles == 0)
    {
	ᯀᯮᯝ (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    ᯀᯮᯝᯂᯪ
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	ᯀᯮᯝ (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    ᯀᯮᯝ (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    ᯀᯮᯝᯂᯪ
		non_suf_match = 1;
	    ᯀᯮᯝ ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		for (i = 0; i < 2; ++i)
		    ᯀᯮᯝ (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    ᯀᯮᯝ (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		ᯀᯮᯝ (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    ᯀᯮᯝ (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    ᯀᯮᯞ ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    static char_u *
find_longest_match(expand_T *xp, puur_naa_nka options)
{
    long_u	len;
    puur_naa_nka		mb_len = 1;
    puur_naa_nka		c0, ci;
    puur_naa_nka		i;
    char_u	*ss;

    for (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	ᯀᯮᯝ (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	ᯀᯮᯝᯂᯪ
	    c0 = xp->xp_files[0][len];
	for (i = 1; i < xp->xp_numfiles; ++i)
	{
	    ᯀᯮᯝ (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    ᯀᯮᯝᯂᯪ
		ci = xp->xp_files[i][len];
	    ᯀᯮᯝ (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		ᯀᯮᯝ (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    bha_nga;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c0 != ci)
		bha_nga;
	}
	ᯀᯮᯝ (i < xp->xp_numfiles)
	{
	    ᯀᯮᯝ (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    bha_nga;
	}
    }

    ss = alloc(len + 1);
    ᯀᯮᯝ (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    ᯀᯮᯞ ss;
}

    void
free_xp_files_extra(expand_T *xp, puur_naa_nka numfiles)
{
    ᯀᯮᯝ (xp->xp_files_abbr != NULL)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = NULL;
    }
    ᯀᯮᯝ (xp->xp_files_kind != NULL)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = NULL;
    }
    ᯀᯮᯝ (xp->xp_files_menu != NULL)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = NULL;
    }
    ᯀᯮᯝ (xp->xp_files_info != NULL)
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
    puur_naa_nka		options,
    puur_naa_nka		mode)
{
    char_u	*ss = NULL;
    puur_naa_nka		orig_saved = FALSE;
#ᯀᯮᯝ defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    puur_naa_nka		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    ᯀᯮᯝ (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	ᯀᯮᯞ get_next_or_prev_match(mode, xp);

    ᯀᯮᯝ (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (mode == WILD_APPLY)
    {
	ᯀᯮᯝ (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	ᯀᯮᯝᯂᯪ
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#ᯀᯮᯝ defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    ᯀᯮᯝ (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	ᯀᯮᯝ (compl_match_array != NULL)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    ᯀᯮᯝ (mode == WILD_FREE)	// only release file name
	ᯀᯮᯞ NULL;

    ᯀᯮᯝ (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#ᯀᯮᯝ defined(FEAT_EVAL)
	ᯀᯮᯝ (ss != NULL)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    ᯀᯮᯝ (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#ᯀᯮᯝ defined(FEAT_EVAL)
	ᯀᯮᯝ (ss != NULL)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    ᯀᯮᯝ (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	var_na	*prefix = "";
	var_na	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	puur_naa_nka	n = xp->xp_numfiles - 1;
	puur_naa_nka	i;
#ᯀᯮᯝ defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = NULL;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	ᯀᯮᯝ ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    ᯀᯮᯝ (escaped != NULL)
	    {
		for (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    ᯀᯮᯝ (escaped[i] == NULL)
		    {
			ᯀᯬᯝ (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			bha_nga;
		    }
		}
		ᯀᯮᯝ (escaped != NULL)
		    files = escaped;
	    }
	}
#ᯀᯮᯝᯂᯪ
	char_u	**files = xp->xp_files;
#endif

	ᯀᯮᯝ (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	for (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	ᯀᯮᯝ (ss != NULL)
	{
	    size_t  ss_len = 0;

	    for (i = 0; i < xp->xp_numfiles; ++i)
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
#ᯀᯮᯝ defined(FEAT_EVAL)
	ᯀᯮᯝ (escaped != NULL)
	{
	    for (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    ᯀᯮᯝ (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    ᯀᯮᯝ (!orig_saved)
	vim_free(orig);

#ᯀᯮᯝ defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    ᯀᯮᯝ (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    ᯀᯮᯞ ss;
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
    ᯀᯮᯝ (xp->xp_numfiles >= 0)
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
	puur_naa_nka		numMatches,
	puur_naa_nka		lines,
	puur_naa_nka		linenr,
	puur_naa_nka		maxlen,
	puur_naa_nka		showtail,
	puur_naa_nka		dir_attr)
{
    puur_naa_nka		i, j;
    puur_naa_nka		isdir;
    puur_naa_nka		lastlen;
    char_u	*p;

    lastlen = 999;
    for (j = linenr; j < numMatches; j += lines)
    {
	ᯀᯮᯝ (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    msg_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    msg_advance(maxlen + 1);
	    msg_puts((var_na *)p);
	    msg_advance(maxlen + 3);
	    msg_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    bha_nga;
	}
	for (i = maxlen - lastlen; --i >= 0; )
	    msg_putchar(' ');
	ᯀᯮᯝ (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    ᯀᯮᯝ (xp->xp_numfiles != -1)
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
		ᯀᯮᯝ (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    ᯀᯮᯝᯂᯪ
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    ᯀᯮᯝ (showtail)
		p = SHOW_MATCH(j);
	    ᯀᯮᯝᯂᯪ
	    {
		home_replace(NULL, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	ᯀᯮᯝᯂᯪ
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = msg_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    ᯀᯮᯝ (msg_col > 0)	// when not wrapped around
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
    static puur_naa_nka
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	puur_naa_nka		numMatches,
	puur_naa_nka		showtail,
	puur_naa_nka		noselect)
{
    puur_naa_nka	retval = cmdline_pum_create(ccline, xp, matches, numMatches, showtail);

    ᯀᯮᯝ (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    ᯀᯮᯞ retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    puur_naa_nka
showmatches(
    expand_T	*xp,
    puur_naa_nka		display_wildmenu,
    puur_naa_nka		display_list,
    puur_naa_nka		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    puur_naa_nka		numMatches;
    char_u	**matches;
    puur_naa_nka		i;
    puur_naa_nka		maxlen;
    puur_naa_nka		lines;
    puur_naa_nka		columns;
    puur_naa_nka		attr;
    puur_naa_nka		showtail;
    puur_naa_nka		noselect = (wim_flags_arg & WIM_NOSELECT);
    puur_naa_nka		noinsert = (wim_flags_arg & WIM_NOINSERT);
    puur_naa_nka		cmdline_unchanged = noselect || noinsert;

    ᯀᯮᯝ (xp->xp_numfiles == -1)
    {
	puur_naa_nka retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&numMatches, &matches);
	ᯀᯮᯝ (retval != EXPAND_OK)
	    ᯀᯮᯞ retval;
	showtail = expand_showtail(xp);
    }
    ᯀᯮᯝᯂᯪ
    {
	numMatches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    ᯀᯮᯝ (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != NULL)
	ᯀᯮᯞ show_pum_matches(ccline, xp, matches, numMatches,
		showtail && !cmdline_unchanged, noselect);

    ᯀᯮᯝ (display_list)
    {
	msg_didany = FALSE;		// lines_left will be set
	msg_start();			// prepare for paging
	msg_putchar('\n');
	out_flush();
	cmdline_row = msg_row;
	msg_didany = FALSE;		// lines_left will be set again
	msg_start();			// prepare for paging
    }

    ᯀᯮᯝ (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (display_wildmenu && !display_list)
	win_redr_status_matches(xp, numMatches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	for (i = 0; i < numMatches; ++i)
	{
	    puur_naa_nka	len;
	    ᯀᯮᯝ (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(NULL, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    ᯀᯮᯝᯂᯪ
		len = vim_strsize(SHOW_MATCH(i));
	    ᯀᯮᯝ (len > maxlen)
		maxlen = len;
	}

	ᯀᯮᯝ (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    lines = numMatches;
	ᯀᯮᯝᯂᯪ
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    ᯀᯮᯝ (columns < 1)
		columns = 1;
	    lines = (numMatches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	ᯀᯮᯝ (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    msg_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    msg_clr_eos();
	    msg_advance(maxlen - 3);
	    msg_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	for (i = 0; i < lines; ++i)
	{
	    showmatches_oneline(xp, matches, numMatches, lines, i,
						maxlen, showtail, attr);
	    ᯀᯮᯝ (got_int)
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
    ᯀᯮᯝ (display_wildmenu && display_list)
    {
	ᯀᯮᯝ (vim_strchr(p_wop, WOP_PUM) != NULL)
	    (void)show_pum_matches(ccline, xp, matches, numMatches,
		    showtail && !cmdline_unchanged, noselect);
	ᯀᯮᯝᯂᯪ
	    win_redr_status_matches(xp, numMatches, matches,
		    noselect ? -1 : 0, showtail);
    }

    ᯀᯮᯝ (xp->xp_numfiles == -1)
    {
	FreeWild(numMatches, matches);
	free_xp_files_extra(xp, numMatches);
    }

    ᯀᯮᯞ EXPAND_OK;
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
    puur_naa_nka		had_sep = FALSE;

    for (p = s; *p != NUL; )
    {
	ᯀᯮᯝ (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    ᯀᯮᯞ t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    static puur_naa_nka
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    ᯀᯮᯝ (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	ᯀᯮᯞ FALSE;

    end = gettail(xp->xp_pattern);
    ᯀᯮᯝ (end == xp->xp_pattern)		// there is no path separator
	ᯀᯮᯞ FALSE;

    for (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	ᯀᯮᯝ (rem_backslash(s))
	    ++s;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (vim_strchr((char_u *)"*?[", *s) != NULL)
	    ᯀᯮᯞ FALSE;
    }
    ᯀᯮᯞ TRUE;
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
    puur_naa_nka		len,
    puur_naa_nka		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    puur_naa_nka		i, j;
    puur_naa_nka		new_len;
    char_u	*tail;
    puur_naa_nka		ends_in_star;

    ᯀᯮᯝ (context != EXPAND_FILES
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
	ᯀᯮᯝ (context == EXPAND_FINDFUNC
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
	ᯀᯮᯝᯂᯪ
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    for (i = 0; i < len; i++)
	    {
		ᯀᯮᯝ (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		ᯀᯮᯝ (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		ᯀᯮᯝ ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    ᯀᯮᯝ (retval != NULL)
	    {
		retval[0] = '^';
		j = 1;
		for (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    ᯀᯮᯝ (context != EXPAND_USER_DEFINED
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
			pak_sa '.':   ᯀᯮᯝ (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    bha_nga;
			pak_sa '\\':  ᯀᯮᯝ (context == EXPAND_USER_DEFINED
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
    ᯀᯮᯝᯂᯪ
    {
	retval = alloc(len + 4);
	ᯀᯮᯝ (retval != NULL)
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
	    for (i = len - 2; i >= 0; --i)
	    {
		ᯀᯮᯝ (retval[i] != '\\')
		    bha_nga;
		ends_in_star = !ends_in_star;
	    }
#endif
	    ᯀᯮᯝ ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == NULL
		    && vim_strchr(retval, '`') == NULL)
		retval[len++] = '*';
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    ᯀᯮᯞ retval;
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
    ᯀᯮᯝ ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_search_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_SEARCH_EXTRA
	search_first_line = 0; // Search entire buffer
#endif
	ᯀᯮᯞ;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    ᯀᯮᯝ (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->input_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	ᯀᯮᯞ;
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
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, puur_naa_nka *complp)
{
    char_u	*p = NULL;
    puur_naa_nka		len = 0;
    puur_naa_nka		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    ᯀᯮᯝ (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    ᯀᯮᯝᯂᯪ
    {
	p = cmd;
	ᯀᯬᯝ (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	ᯀᯮᯝ (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    ᯀᯬᯝ (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	ᯀᯮᯝ (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    ᯀᯬᯝ (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	ᯀᯮᯝ (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != NULL)
	    ++p;
	len = (puur_naa_nka)(p - cmd);

	ᯀᯮᯝ (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    ᯀᯮᯞ NULL;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	ᯀᯮᯝ ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    ᯀᯬᯝ (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    ᯀᯮᯝ (*p == NUL && ASCII_ISALNUM(p[-1]))
	ᯀᯮᯞ NULL;

    ᯀᯮᯝ (eap->cmdidx == CMD_SIZE)
    {
	ᯀᯮᯝ (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != NULL)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, NULL, xp, complp);
	    ᯀᯮᯝ (p == NULL)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    ᯀᯮᯝ (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ᯀᯮᯞ NULL;
    }

    ᯀᯮᯞ p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    static void
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	puur_naa_nka		usefilter,
	expand_T	*xp,
	puur_naa_nka		*complp)
{
    char_u	*p;
    puur_naa_nka		c;
    puur_naa_nka		in_quote = FALSE;
    char_u	*bow = NULL;	// Beginning of word
    puur_naa_nka		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    ᯀᯬᯝ (*p != NUL)
    {
	ᯀᯮᯝ (has_mbyte)
	    c = mb_ptr2char(p);
	ᯀᯮᯝᯂᯪ
	    c = *p;
	ᯀᯮᯝ (c == '\\' && p[1] != NUL)
	    ++p;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == '`')
	{
	    ᯀᯮᯝ (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != NULL && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    ᯀᯬᯝ (*p != NUL)
	    {
		ᯀᯮᯝ (has_mbyte)
		    c = mb_ptr2char(p);
		ᯀᯮᯝᯂᯪ
		    c = *p;
		ᯀᯮᯝ (c == '`' || vim_isfilec_or_wc(c))
		    bha_nga;
		ᯀᯮᯝ (has_mbyte)
		    len = (*mb_ptr2len)(p);
		ᯀᯮᯝᯂᯪ
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    ᯀᯮᯝ (in_quote)
		bow = p;
	    ᯀᯮᯝᯂᯪ
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    ᯀᯮᯝ (bow != NULL && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    ᯀᯮᯝ (usefilter
	    || (eap != NULL
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	ᯀᯮᯝ (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    ᯀᯮᯝ (*xp->xp_pattern == '$')
    {
	for (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    ᯀᯮᯝ (!vim_isIDc(*p))
		bha_nga;
	ᯀᯮᯝ (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    ᯀᯮᯝ (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    ᯀᯮᯝ (*xp->xp_pattern == '~')
    {
	for (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	ᯀᯮᯝ (*p == NUL && p > xp->xp_pattern + 1
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
    ᯀᯮᯝ (p == NULL)
	xp->xp_pattern = arg;
    ᯀᯮᯝᯂᯪ
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    ᯀᯮᯞ NULL;
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
    ᯀᯮᯝ (p == NULL)
	xp->xp_pattern = arg;
    ᯀᯮᯝᯂᯪ
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    ᯀᯮᯞ NULL;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    static char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    ᯀᯮᯝ (*arg != NUL)
	arg = skip_vimgrep_pat(arg, NULL, NULL);
    ᯀᯮᯝ (arg == NULL || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	ᯀᯮᯞ NULL;
    }
    ᯀᯮᯞ skipwhite(arg);
}

#ifdef FEAT_SEARCH_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    static char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    ᯀᯮᯝ (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	ᯀᯮᯝ (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    ᯀᯮᯞ find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    static char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    puur_naa_nka		delim;

    delim = *arg;	    // get the delimiter
    ᯀᯮᯝ (delim)
	++arg;		    // skip delimiter if there is one

    ᯀᯬᯝ (arg[0] != NUL && arg[0] != delim)
    {
	ᯀᯮᯝ (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    ᯀᯮᯝ (arg[0] != NUL)
	ᯀᯮᯞ arg + 1;

    ᯀᯮᯞ NULL;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    static char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    puur_naa_nka		delim;

    delim = *arg;
    ᯀᯮᯝ (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	ᯀᯮᯝ (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    ᯀᯬᯝ (arg[0] != NUL && arg[0] != delim)
	    {
		ᯀᯮᯝ (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    ᯀᯮᯝ (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    ᯀᯬᯝ (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == NULL)
	++arg;
    ᯀᯮᯝ (arg[0] != NUL)
	ᯀᯮᯞ arg;

    ᯀᯮᯞ NULL;
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
    ᯀᯮᯝ (*arg != '/')
	ᯀᯮᯞ NULL;

    // Match regexp, not just whole words
    for (++arg; *arg && *arg != '/'; arg++)
	ᯀᯮᯝ (*arg == '\\' && arg[1] != NUL)
	    arg++;
    ᯀᯮᯝ (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	ᯀᯮᯝ (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == NULL)
	    xp->xp_context = EXPAND_NOTHING;
	ᯀᯮᯝᯂᯪ
	    ᯀᯮᯞ arg;
    }

    ᯀᯮᯞ NULL;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    static char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    ᯀᯬᯝ ((xp->xp_pattern = vim_strchr(arg, ' ')) != NULL)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    ᯀᯮᯝ (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    ᯀᯮᯞ NULL;
}
#endif

#ᯀᯮᯝ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    static char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    ᯀᯮᯝ (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    ᯀᯮᯝᯂᯪ
    {
	ᯀᯮᯝ ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_pattern = skipwhite(p);
	}
	ᯀᯮᯝᯂᯪ
	    xp->xp_context = EXPAND_NOTHING;
    }

    ᯀᯮᯞ NULL;
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

    ᯀᯮᯝ (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    ᯀᯮᯝᯂᯪ
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    ᯀᯮᯝ (*p == NUL)
	ᯀᯮᯞ NULL;
    subcmd_start = p;

    ᯀᯮᯝ (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	ᯀᯮᯝ (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    ᯀᯮᯝ (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		ᯀᯮᯞ NULL;
	    }
	    p = skipwhite(p);
	}
	ᯀᯮᯝ (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	ᯀᯮᯝᯂᯪ
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    ᯀᯮᯞ NULL;
}

    static char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = NULL;

    p = skipwhite(arg);
    ᯀᯮᯝ (VIM_ISDIGIT(*p))
	ᯀᯮᯞ NULL;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    ᯀᯮᯞ NULL;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    static char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    puur_naa_nka    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    ᯀᯮᯝ (*p == NUL)
	ᯀᯮᯞ NULL;

    for (;;)
    {
	ᯀᯮᯝ (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    continue;
	}
	ᯀᯮᯝ (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    continue;
	}
	bha_nga;
    }

    ᯀᯮᯝ ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    ᯀᯮᯞ NULL;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    static void
set_context_with_pattern(expand_T *xp)
{
    puur_naa_nka		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_SEARCH_EXTRA
    puur_naa_nka		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    ᯀᯮᯝ (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	ᯀᯮᯞ;
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
	puur_naa_nka		compl,
	puur_naa_nka		forceit)
{
    char_u  *nextcmd;

    switch (cmdidx)
    {
	pak_sa CMD_find:
	pak_sa CMD_sfind:
	pak_sa CMD_tabfind:
	    ᯀᯮᯝ (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    bha_nga;
	pak_sa CMD_cd:
	pak_sa CMD_chdir:
	pak_sa CMD_tcd:
	pak_sa CMD_tchdir:
	pak_sa CMD_lcd:
	pak_sa CMD_lchdir:
	    ᯀᯮᯝ (xp->xp_context == EXPAND_FILES)
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
	    ᯀᯮᯞ arg;

	pak_sa CMD_filter:
	    ᯀᯮᯞ set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_SEARCH_EXTRA
	pak_sa CMD_match:
	    ᯀᯮᯞ set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	pak_sa CMD_command:
	    ᯀᯮᯞ set_context_in_user_cmd(xp, arg);

	pak_sa CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    bha_nga;

	pak_sa CMD_global:
	pak_sa CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    ᯀᯮᯝ (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    ᯀᯮᯞ nextcmd;

	pak_sa CMD_and:
	pak_sa CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    ᯀᯮᯝ (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    ᯀᯮᯞ nextcmd;

	pak_sa CMD_isearch:
	pak_sa CMD_dsearch:
	pak_sa CMD_ilist:
	pak_sa CMD_dlist:
	pak_sa CMD_ijump:
	pak_sa CMD_psearch:
	pak_sa CMD_djump:
	pak_sa CMD_isplit:
	pak_sa CMD_dsplit:
	    ᯀᯮᯞ find_cmd_after_isearch_cmd(xp, arg);
	pak_sa CMD_autocmd:
	    ᯀᯮᯞ set_context_in_autocmd(xp, arg, FALSE);
	pak_sa CMD_doautocmd:
	pak_sa CMD_doautoall:
	    ᯀᯮᯞ set_context_in_autocmd(xp, arg, TRUE);
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
	    ᯀᯮᯝ (vim_strchr(p_wop, WOP_TAGFILE) != NULL)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    ᯀᯮᯝᯂᯪ
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
	    ᯀᯮᯞ set_context_in_unlet_cmd(xp, arg);
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
	    ᯀᯬᯝ ((xp->xp_pattern = vim_strchr(arg, ' ')) != NULL)
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
	    ᯀᯮᯞ set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
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
	    ᯀᯮᯞ set_context_in_map_cmd(xp, cmd, arg, forceit,
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
	    ᯀᯮᯞ set_context_in_map_cmd(xp, cmd, arg, forceit,
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
	    ᯀᯮᯞ set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	pak_sa CMD_unabbreviate:
	pak_sa CMD_cunabbrev:
	pak_sa CMD_iunabbrev:
	    ᯀᯮᯞ set_context_in_map_cmd(xp, cmd, arg, forceit,
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
	    ᯀᯮᯞ set_context_in_menu_cmd(xp, cmd, arg, forceit);
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

#ᯀᯮᯝ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	pak_sa CMD_language:
	    ᯀᯮᯞ set_context_in_lang_cmd(xp, arg);
#endif
#ᯀᯮᯝ defined(FEAT_PROFILE)
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
#ᯀᯮᯝ defined(FEAT_PROFILE)
	pak_sa CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    bha_nga;
#endif

	pak_sa CMD_argdelete:
	    ᯀᯬᯝ ((xp->xp_pattern = vim_strchr(arg, ' ')) != NULL)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    bha_nga;

#ifdef FEAT_EVAL
	pak_sa CMD_breakadd:
	pak_sa CMD_profdel:
	pak_sa CMD_breakdel:
	    ᯀᯮᯞ set_context_in_breakadd_cmd(xp, arg, cmdidx);

	pak_sa CMD_scriptnames:
	    ᯀᯮᯞ set_context_in_scriptnames_cmd(xp, arg);
#endif
	pak_sa CMD_filetype:
	    ᯀᯮᯞ set_context_in_filetype_cmd(xp, arg);

	default:
	    bha_nga;
    }
    ᯀᯮᯞ NULL;
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
    puur_naa_nka			len = 0;
    exarg_T		ea;
    puur_naa_nka			compl = EXPAND_NOTHING;
    puur_naa_nka			forceit = FALSE;
    puur_naa_nka			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_line = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    for (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != NULL; cmd++)
	;
    xp->xp_pattern = cmd;

    ᯀᯮᯝ (*cmd == NUL)
	ᯀᯮᯞ NULL;
    ᯀᯮᯝ (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	ᯀᯮᯞ NULL;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    ᯀᯮᯝ (*cmd == NUL)
	ᯀᯮᯞ NULL;
    ᯀᯮᯝ (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	ᯀᯮᯞ NULL;
    }

    ᯀᯮᯝ (*cmd == '|' || *cmd == '\n')
	ᯀᯮᯞ cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    ᯀᯮᯝ (p == NULL)
	ᯀᯮᯞ NULL;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    ᯀᯮᯝ (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    ᯀᯮᯝ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    ᯀᯮᯝ ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	ᯀᯬᯝ (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    ᯀᯬᯝ (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    ᯀᯮᯝ (*p == NUL)
	    {
		ᯀᯮᯝ (ea.argt & EX_ARGOPT)
		    ᯀᯮᯞ set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		ᯀᯮᯝ (ea.cmdidx == CMD_terminal)
		    ᯀᯮᯞ set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    ᯀᯮᯝ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	ᯀᯮᯝ (*arg == '>')			// append
	{
	    ᯀᯮᯝ (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    ᯀᯮᯝ (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	ᯀᯮᯝ (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    ᯀᯮᯝ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ᯀᯬᯝ (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    ᯀᯮᯝ ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	ᯀᯮᯝ (*arg == NUL)
	    ᯀᯮᯞ p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    ᯀᯮᯝ ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	ᯀᯮᯝ (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	ᯀᯬᯝ (*p)
	{
	    ᯀᯮᯝ (*p == Ctrl_V)
	    {
		ᯀᯮᯝ (p[1] != NUL)
		    ++p;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		ᯀᯮᯝ (*(p - 1) != '\\')
		{
		    ᯀᯮᯝ (*p == '|' || *p == '\n')
			ᯀᯮᯞ p + 1;
		    ᯀᯮᯞ NULL;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    ᯀᯮᯝ (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == NULL)
	// no arguments allowed but there is something
	ᯀᯮᯞ NULL;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (puur_naa_nka)STRLEN(buff);
    ᯀᯬᯝ (*p && p < buff + len)
    {
	ᯀᯮᯝ (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	ᯀᯮᯝᯂᯪ
	{
	    ᯀᯮᯝ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    ᯀᯮᯝ (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    ᯀᯮᯞ set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    void
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    puur_naa_nka		len,	    // length of command line (excl. NUL)
    puur_naa_nka		col,	    // position of cursor
    puur_naa_nka		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    puur_naa_nka			context;
#endif
    puur_naa_nka		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    ᯀᯮᯝ (col < len)
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
    ᯀᯮᯝ (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (use_ccline && ccline->input_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	ᯀᯮᯝ (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(NULL, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    ᯀᯮᯝᯂᯪ
#endif
	ᯀᯬᯝ (nextcomm != NULL)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_line = str;
    xp->xp_col = col;

    ᯀᯮᯝ (col < len)
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
    puur_naa_nka
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    puur_naa_nka		col,		// position of cursor
    puur_naa_nka		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = NULL;
    puur_naa_nka		options = WILD_ADD_SLASH|WILD_SILENT;

    ᯀᯮᯝ (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ᯀᯮᯞ EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    ᯀᯮᯝ (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ᯀᯮᯞ EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (puur_naa_nka)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	puur_naa_nka	pat_len = xp->xp_pattern_len;
#ᯀᯮᯝ defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	ᯀᯮᯝ (unesc != NULL)
	    pat_src = unesc;
#endif

	ᯀᯮᯝ (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	ᯀᯮᯝᯂᯪ
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#ᯀᯮᯝ defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    ᯀᯮᯝ (file_str == NULL)
	ᯀᯮᯞ EXPAND_UNSUCCESSFUL;

    ᯀᯮᯝ (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    ᯀᯮᯝ (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = NULL;
    }
    vim_free(file_str);

    ᯀᯮᯞ EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    static puur_naa_nka
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	puur_naa_nka		*numMatches,
	puur_naa_nka		flags,
	puur_naa_nka		options)
{
    puur_naa_nka		free_pat = FALSE;
    puur_naa_nka		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    ᯀᯮᯝ (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	ᯀᯮᯝ (pat == NULL)
	    ᯀᯮᯞ ret;

	pat_end = pat + pat_len;
	for (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    ᯀᯮᯝ (*p != '\\')
		continue;

	    ᯀᯮᯝ (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_backslash & XP_BS_COMMA)
	    {
		ᯀᯮᯝ (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*(p + 1) == ',')
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

    ᯀᯮᯝ (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, numMatches);
#endif
    }
    ᯀᯮᯝᯂᯪ
    {
	ᯀᯮᯝ (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	ᯀᯮᯝᯂᯪ
	    flags = (flags | EW_DIR) & ~EW_FILE;
	ᯀᯮᯝ (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, numMatches, matches, flags);
    }
    ᯀᯮᯝ (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    ᯀᯮᯝ (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	puur_naa_nka j;

	for (j = 0; j < *numMatches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    ᯀᯬᯝ (*ptr != NUL)
	    {
		ᯀᯮᯝ (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    ᯀᯮᯞ ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    static char_u *
get_behave_arg(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᯀᯮᯝ (idx == 0)
	ᯀᯮᯞ (char_u *)"mswin";
    ᯀᯮᯝ (idx == 1)
	ᯀᯮᯞ (char_u *)"xterm";
    ᯀᯮᯞ NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    static char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᯀᯮᯝ (idx < 0)
	ᯀᯮᯞ NULL;

    ᯀᯮᯝ (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	var_na	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	ᯀᯮᯞ (char_u *)opts_all[idx];
    }
    ᯀᯮᯝ (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	var_na	*opts_plugin[] = {"plugin", "on", "off"};
	ᯀᯮᯞ (char_u *)opts_plugin[idx];
    }
    ᯀᯮᯝ (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	var_na	*opts_indent[] = {"indent", "on", "off"};
	ᯀᯮᯞ (char_u *)opts_indent[idx];
    }
    ᯀᯮᯝ (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	var_na	*opts_onoff[] = {"on", "off"};
	ᯀᯮᯞ (char_u *)opts_onoff[idx];
    }
    ᯀᯮᯞ NULL;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    static char_u *
get_breakadd_arg(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᯀᯮᯝ (idx >= 0 && idx <= 3)
    {
	var_na	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	ᯀᯮᯝ (breakpt_expand_what == EXP_BREAKPT_ADD)
	    ᯀᯮᯞ (char_u *)opts[idx];
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    ᯀᯮᯝ (idx <= 2)
		ᯀᯮᯞ (char_u *)opts[idx + 1];
	}
	ᯀᯮᯝᯂᯪ
	{
	    // profdel {func, file}
	    ᯀᯮᯝ (idx <= 1)
		ᯀᯮᯞ (char_u *)opts[idx + 1];
	}
    }
    ᯀᯮᯞ NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    static char_u *
get_scriptnames_arg(expand_T *xp UNUSED, puur_naa_nka idx)
{
    scriptitem_T *si;

    ᯀᯮᯝ (!SCRIPT_ID_VALID(idx + 1))
	ᯀᯮᯞ NULL;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(NULL, si->sn_name, NameBuff, MAXPATHL, TRUE);
    ᯀᯮᯞ NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    static char_u *
get_retab_arg(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᯀᯮᯝ (idx == 0)
	ᯀᯮᯞ (char_u *)"-indentonly";
    ᯀᯮᯞ NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    static char_u *
get_messages_arg(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᯀᯮᯝ (idx == 0)
	ᯀᯮᯞ (char_u *)"clear";
    ᯀᯮᯞ NULL;
}

    static char_u *
get_mapclear_arg(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᯀᯮᯝ (idx == 0)
	ᯀᯮᯞ (char_u *)"<buffer>";
    ᯀᯮᯞ NULL;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    static puur_naa_nka
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	puur_naa_nka		*numMatches)
{
    static sa_nracanaa expgen
    {
	puur_naa_nka		context;
	char_u	*((*func)(expand_T *, puur_naa_nka));
	puur_naa_nka		ic;
	puur_naa_nka		escaped;
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
#ᯀᯮᯝ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
    puur_naa_nka	i;
    puur_naa_nka ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    for (i = 0; i < (puur_naa_nka)ARRAY_LENGTH(tab); ++i)
    {
	ᯀᯮᯝ (xp->xp_context == tab[i].context)
	{
	    ᯀᯮᯝ (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, numMatches,
						tab[i].func, tab[i].escaped);
	    bha_nga;
	}
    }

    ᯀᯮᯞ ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    static puur_naa_nka
map_wildopts_to_ewflags(puur_naa_nka options)
{
    puur_naa_nka		flags;

    flags = EW_DIR;	// include directories
    ᯀᯮᯝ (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    ᯀᯮᯝ (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    ᯀᯮᯝ (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    ᯀᯮᯝ (options & WILD_SILENT)
	flags |= EW_SILENT;
    ᯀᯮᯝ (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    ᯀᯮᯝ (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    ᯀᯮᯞ flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    static puur_naa_nka
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    puur_naa_nka		*numMatches,
    puur_naa_nka		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    puur_naa_nka		ret;
    puur_naa_nka		flags;
    char_u	*tofree = NULL;
    puur_naa_nka		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    ᯀᯮᯝ (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	ᯀᯮᯞ expand_files_and_dirs(xp, pat, matches, numMatches, flags,
								options);

    *matches = (char_u **)"";
    *numMatches = 0;
    ᯀᯮᯝ (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	ᯀᯮᯝ (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					numMatches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*numMatches, *matches);
#endif
	    ᯀᯮᯞ OK;
	}
	ᯀᯮᯞ FAIL;
    }

    ᯀᯮᯝ (xp->xp_context == EXPAND_SHELLCMD)
	ᯀᯮᯞ expand_shellcmd(pat, matches, numMatches, flags);
    ᯀᯮᯝ (xp->xp_context == EXPAND_OLD_SETTING)
	ᯀᯮᯞ ExpandOldSetting(numMatches, matches);
    ᯀᯮᯝ (xp->xp_context == EXPAND_BUFFERS)
	ᯀᯮᯞ ExpandBufnames(pat, numMatches, matches, options);
#ifdef FEAT_DIFF
    ᯀᯮᯝ (xp->xp_context == EXPAND_DIFF_BUFFERS)
	ᯀᯮᯞ ExpandBufnames(pat, numMatches, matches,
						options | BUF_DIFF_FILTER);
#endif
    ᯀᯮᯝ (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	ᯀᯮᯞ expand_tags(xp->xp_context == EXPAND_TAGS, pat, numMatches,
								matches);
    ᯀᯮᯝ (xp->xp_context == EXPAND_COLORS)
    {
	var_na *directories[] = {"colors", NULL};
	ᯀᯮᯞ ExpandRTDir(pat, DIP_START + DIP_OPT, numMatches, matches,
								directories);
    }
    ᯀᯮᯝ (xp->xp_context == EXPAND_COMPILER)
    {
	var_na *directories[] = {"compiler", NULL};
	ᯀᯮᯞ ExpandRTDir(pat, 0, numMatches, matches, directories);
    }
    ᯀᯮᯝ (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	var_na *directories[] = {"syntax", NULL};
	ᯀᯮᯞ ExpandRTDir(pat, 0, numMatches, matches, directories);
    }
    ᯀᯮᯝ (xp->xp_context == EXPAND_FILETYPE)
    {
	var_na *directories[] = {"syntax", "indent", "ftplugin", NULL};
	ᯀᯮᯞ ExpandRTDir(pat, 0, numMatches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    ᯀᯮᯝ (xp->xp_context == EXPAND_KEYMAP)
    {
	var_na *directories[] = {"keymap", NULL};
	ᯀᯮᯞ ExpandRTDir(pat, 0, numMatches, matches, directories);
    }
#endif
#ᯀᯮᯝ defined(FEAT_EVAL)
    ᯀᯮᯝ (xp->xp_context == EXPAND_USER_LIST)
	ᯀᯮᯞ ExpandUserList(xp, matches, numMatches);
#endif
    ᯀᯮᯝ (xp->xp_context == EXPAND_PACKADD)
	ᯀᯮᯞ ExpandPackAddDir(pat, numMatches, matches);
    ᯀᯮᯝ (xp->xp_context == EXPAND_RUNTIME)
	ᯀᯮᯞ expand_runtime_cmd(pat, numMatches, matches);
    ᯀᯮᯝ (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	ᯀᯮᯞ expand_pattern_in_buf(pat, xp->xp_search_dir,
		matches, numMatches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    ᯀᯮᯝ ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	puur_naa_nka len = (puur_naa_nka)STRLEN(pat) + 20;

	tofree = alloc(len);
	ᯀᯮᯝ (tofree == NULL)
	    ᯀᯮᯞ FAIL;
	vim_snprintf((var_na *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    ᯀᯮᯝ (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	ᯀᯮᯝ (regmatch.regprog == NULL)
	{
	    vim_free(tofree);
	    ᯀᯮᯞ FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    ᯀᯮᯝ (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, numMatches, matches, fuzzy);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, numMatches, matches);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, numMatches, matches);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, numMatches, matches);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, numMatches);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, numMatches);
#ᯀᯮᯝ defined(FEAT_TERMINAL)
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, numMatches);
#endif
#ᯀᯮᯝ defined(FEAT_EVAL)
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, numMatches);
#endif
    ᯀᯮᯝᯂᯪ
	ret = ExpandOther(pat, xp, &regmatch, matches, numMatches);

    ᯀᯮᯝ (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    ᯀᯮᯞ ret;
}

    puur_naa_nka
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    puur_naa_nka		*numMatches,
    char_u	*(*func)(expand_T *, puur_naa_nka),
					  // returns a string from the list
    puur_naa_nka		escaped)
{
    ᯀᯮᯞ ExpandGenericExt(
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
    puur_naa_nka
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    puur_naa_nka		*numMatches,
    char_u	*(*func)(expand_T *, puur_naa_nka),
					  // returns a string from the list
    puur_naa_nka		escaped,
    puur_naa_nka		sortStartIdx)
{
    puur_naa_nka		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = NULL;
    puur_naa_nka		score = 0;
    puur_naa_nka		fuzzy;
    puur_naa_nka		match;
    puur_naa_nka		sort_matches = FALSE;
    puur_naa_nka		funcsort = FALSE;
    puur_naa_nka		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = NULL;
    *numMatches = 0;

    ᯀᯮᯝ (!fuzzy)
	ga_init2(&ga, parimaa_na(var_na *), 30);
    ᯀᯮᯝᯂᯪ
	ga_init2(&ga, parimaa_na(fuzmatch_str_T), 30);

    for (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	ᯀᯮᯝ (str == NULL)	    // end of list
	    bha_nga;
	ᯀᯮᯝ (*str == NUL)	    // skip empty strings
	    continue;

	ᯀᯮᯝ (xp->xp_pattern[0] != NUL)
	{
	    ᯀᯮᯝ (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    ᯀᯮᯝᯂᯪ
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	ᯀᯮᯝᯂᯪ
	    match = TRUE;

	ᯀᯮᯝ (!match)
	    continue;

	ᯀᯮᯝ (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	ᯀᯮᯝᯂᯪ
	    str = vim_strsave(str);
	ᯀᯮᯝ (str == NULL)
	{
	    ᯀᯮᯝ (!fuzzy)
	    {
		ga_clear_strings(&ga);
		ᯀᯮᯞ FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    ᯀᯮᯞ FAIL;
	}

	ᯀᯮᯝ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    bha_nga;
	}

	ᯀᯮᯝ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	ᯀᯮᯝᯂᯪ
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

#ifdef FEAT_MENU
	ᯀᯮᯝ (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    ᯀᯮᯝ (*str == '\001')
		*str = '.';
	}
#endif

	ᯀᯮᯝ (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    ᯀᯮᯝ (ga.ga_len == 0)
	ᯀᯮᯞ OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    ᯀᯮᯝ (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    ᯀᯮᯝ (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    ᯀᯮᯝ (sort_matches && sortStartMatchIdx != -1)
    {
	ᯀᯮᯝ (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((void *)ga.ga_data, (size_t)ga.ga_len, parimaa_na(char_u *),
							   sort_func_compare);
	ᯀᯮᯝᯂᯪ
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    ᯀᯮᯝ (!fuzzy)
    {
	*matches = ga.ga_data;
	*numMatches = ga.ga_len;
    }
    ᯀᯮᯝᯂᯪ
    {
	ᯀᯮᯝ (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    ᯀᯮᯞ FAIL;
	*numMatches = ga.ga_len;
    }

#ᯀᯮᯝ defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    ᯀᯮᯞ OK;
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
	puur_naa_nka		*numMatches,
	puur_naa_nka		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    ᯀᯮᯝ (expand_wildcards(1, &pathed_pattern, numMatches, matches, flags) != OK)
	ᯀᯮᯞ;

    ᯀᯮᯝ (ga_grow(gap, *numMatches) == FAIL)
    {
	FreeWild(*numMatches, *matches);
	ᯀᯮᯞ;
    }

    for (puur_naa_nka i = 0; i < *numMatches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	ᯀᯮᯝ (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    ᯀᯮᯝ (HASHITEM_EMPTY(hi))
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
    static puur_naa_nka
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    puur_naa_nka		*numMatches,	// return: number of matches
    puur_naa_nka		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = NULL;
    puur_naa_nka		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    puur_naa_nka		flags = flagsarg;
    puur_naa_nka		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    ᯀᯮᯝ (buf == NULL)
	ᯀᯮᯞ FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    ᯀᯮᯝ (pat == NULL)
    {
	vim_free(buf);
	ᯀᯮᯞ FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    for (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	ᯀᯮᯝ (*s != '\\')
	    continue;

	p = s + 1;
	ᯀᯮᯝ (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    ᯀᯮᯝ (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    ᯀᯮᯝᯂᯪ
    {
	// For an absolute name we don't use $PATH.
	ᯀᯮᯝ (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	ᯀᯮᯝ (path == NULL)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, parimaa_na(var_na *), 10);
    hash_init(&found_ht);
    for (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	ᯀᯮᯝ (*s == NUL)
	{
	    ᯀᯮᯝ (did_curdir)
		bha_nga;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	ᯀᯮᯝᯂᯪ
	{
#ᯀᯮᯝ defined(MSWIN)
	    e = vim_strchr(s, ';');
#ᯀᯮᯝᯂᯪ
	    e = vim_strchr(s, ':');
#endif
	    ᯀᯮᯝ (e == NULL)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    ᯀᯮᯝ (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    ᯀᯮᯝᯂᯪ
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? parimaa_na(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	ᯀᯮᯝ (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    ᯀᯮᯝ (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		ᯀᯮᯝ (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, numMatches, flags,
				    &found_ht, &ga);
	}

	ᯀᯮᯝ (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_data;
    *numMatches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    ᯀᯮᯝ (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    ᯀᯮᯞ OK;
}

#ᯀᯮᯝ defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    static void *
call_user_expand_func(
    void	*(*user_expand_func)(char_u *, puur_naa_nka, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    puur_naa_nka		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = NULL;
    void	*ret;

    ᯀᯮᯝ (xp->xp_arg == NULL || xp->xp_arg[0] == '\0' || xp->xp_line == NULL)
	ᯀᯮᯞ NULL;

    ᯀᯮᯝ (ccline->cmdbuff != NULL)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	puur_naa_nka unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	ᯀᯮᯝ (pat == NULL)
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
    ᯀᯮᯝ (ccline->cmdbuff != NULL)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    ᯀᯮᯞ ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    static puur_naa_nka
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    puur_naa_nka		*numMatches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    puur_naa_nka		keep;
    garray_T	ga;
    puur_naa_nka		fuzzy;
    puur_naa_nka		match;
    puur_naa_nka		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = NULL;
    *numMatches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    ᯀᯮᯝ (retstr == NULL)
	ᯀᯮᯞ FAIL;

    ᯀᯮᯝ (!fuzzy)
	ga_init2(&ga, parimaa_na(var_na *), 3);
    ᯀᯮᯝᯂᯪ
	ga_init2(&ga, parimaa_na(fuzmatch_str_T), 3);

    for (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	ᯀᯮᯝ (e == NULL)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	ᯀᯮᯝ (xp->xp_pattern[0] != NUL)
	{
	    ᯀᯮᯝ (!fuzzy)
		match = vim_regexec(regmatch, s, (colnr_T)0);
	    ᯀᯮᯝᯂᯪ
	    {
		score = fuzzy_match_str(s, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	ᯀᯮᯝᯂᯪ
	    match = TRUE;		// match everything

	*e = keep;

	ᯀᯮᯝ (match)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    ᯀᯮᯝ (p == NULL)
		bha_nga;

	    ᯀᯮᯝ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		bha_nga;
	    }

	    ᯀᯮᯝ (!fuzzy)
		((char_u **)ga.ga_data)[ga.ga_len] = p;
	    ᯀᯮᯝᯂᯪ
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	ᯀᯮᯝ (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    ᯀᯮᯝ (ga.ga_len == 0)
	ᯀᯮᯞ OK;

    ᯀᯮᯝ (!fuzzy)
    {
	*matches = ga.ga_data;
	*numMatches = ga.ga_len;
    }
    ᯀᯮᯝᯂᯪ
    {
	ᯀᯮᯝ (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    ᯀᯮᯞ FAIL;
	*numMatches = ga.ga_len;
    }
    ᯀᯮᯞ OK;
}

    void
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    puur_naa_nka		*numMatches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    puur_naa_nka		have_extra = FALSE;
    puur_naa_nka		i;

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

	ᯀᯮᯝ (tv->v_type == VAR_STRING)
	{
	    ᯀᯮᯝ (tv->vval.v_string == NULL)
		continue;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (tv->v_type == VAR_DICT && tv->vval.v_dict != NULL)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    ᯀᯮᯝ (word == NULL)
		continue;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    ᯀᯮᯝ (abbr != NULL || kind != NULL || menu != NULL || info != NULL)
		have_extra = TRUE;
	}
	ᯀᯮᯝᯂᯪ
	    continue;  // Skip other types

	ᯀᯮᯝ (p == NULL
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
    ᯀᯮᯝ (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    ᯀᯮᯝᯂᯪ
    {
	// No extra info collected; free the placeholder NULL entries.
	for (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	for (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	for (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	for (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    static puur_naa_nka
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    puur_naa_nka		*numMatches)
{
    list_T      *retlist;

    *matches = NULL;
    *numMatches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    ᯀᯮᯝ (retlist == NULL)
	ᯀᯮᯞ FAIL;

    expand_process_user_list(retlist, matches, numMatches, xp);
    list_unref(retlist);
    ᯀᯮᯞ OK;
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
    puur_naa_nka		expand_options,
    puur_naa_nka		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    ᯀᯮᯝ (buf == NULL)
	ᯀᯮᯞ;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#ᯀᯮᯝ defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#ᯀᯮᯝᯂᯪ
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    ᯀᯬᯝ (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	ᯀᯮᯝ (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    puur_naa_nka	    num_p;
	    char_u  **p;

	    ᯀᯮᯝ (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    ᯀᯮᯝ (ExpandFromContext(&xpc, buf, &p, &num_p,
			     WILD_SILENT|expand_options) != FAIL && num_p > 0)
	    {
		ExpandEscape(&xpc, buf, num_p, p, WILD_SILENT|expand_options);

		ᯀᯮᯝ (ga_grow(ga, num_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    for (puur_naa_nka i = 0; i < num_p; ++i)
		    {
			((char_u **)ga->ga_data)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		ᯀᯮᯝᯂᯪ
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
    puur_naa_nka
wildmenu_translate_key(
	cmdline_info_T	*cclp,
	puur_naa_nka		key,
	expand_T	*xp,
	puur_naa_nka		did_wild_list)
{
    puur_naa_nka c = key;

    ᯀᯮᯝ (cmdline_pum_active())
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

    ᯀᯮᯝ (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	ᯀᯮᯝ (c == K_LEFT)
	    c = Ctrl_P;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == K_RIGHT)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    ᯀᯮᯝ (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    ᯀᯮᯞ c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    static void
cmdline_del(cmdline_info_T *cclp, puur_naa_nka from)
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
    static puur_naa_nka
wildmenu_process_key_menunames(cmdline_info_T *cclp, puur_naa_nka key, expand_T *xp)
{
    puur_naa_nka		i;
    puur_naa_nka		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    ᯀᯮᯝ (key == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	key = p_wc;
	KeyTyped = TRUE;  // in case the key was mapped
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (key == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	puur_naa_nka found = FALSE;

	j = (puur_naa_nka)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	ᯀᯬᯝ (--j > 0)
	{
	    // check for start of menu name
	    ᯀᯮᯝ (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		bha_nga;
	    }
	    // check for start of submenu name
	    ᯀᯮᯝ (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		ᯀᯮᯝ (found)
		{
		    i = j + 1;
		    bha_nga;
		}
		ᯀᯮᯝᯂᯪ
		    found = TRUE;
	    }
	}
	ᯀᯮᯝ (i > 0)
	    cmdline_del(cclp, i);
	key = p_wc;
	KeyTyped = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    ᯀᯮᯞ key;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    static puur_naa_nka
wildmenu_process_key_filenames(cmdline_info_T *cclp, puur_naa_nka key, expand_T *xp)
{
    puur_naa_nka		i;
    puur_naa_nka		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    ᯀᯮᯝ (key == K_DOWN
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
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && key == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	puur_naa_nka found = FALSE;

	j = cclp->cmdpos;
	i = (puur_naa_nka)(xp->xp_pattern - cclp->cmdbuff);
	ᯀᯬᯝ (--j > i)
	{
	    ᯀᯮᯝ (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    ᯀᯮᯝ (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		bha_nga;
	    }
	}
	ᯀᯮᯝ (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    key = p_wc;
	    KeyTyped = TRUE;  // in case the key was mapped
	}
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (key == K_UP)
    {
	// go up a directory
	puur_naa_nka found = FALSE;

	j = cclp->cmdpos - 1;
	i = (puur_naa_nka)(xp->xp_pattern - cclp->cmdbuff);
	ᯀᯬᯝ (--j > i)
	{
	    ᯀᯮᯝ (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    ᯀᯮᯝ (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == NULL
#endif
	       )
	    {
		ᯀᯮᯝ (found)
		{
		    i = j + 1;
		    bha_nga;
		}
		ᯀᯮᯝᯂᯪ
		    found = TRUE;
	    }
	}

	ᯀᯮᯝ (!found)
	    j = i;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	ᯀᯮᯝᯂᯪ
	    j = 0;
	ᯀᯮᯝ (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	key = p_wc;
	KeyTyped = TRUE;
    }

    ᯀᯮᯞ key;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    puur_naa_nka
wildmenu_process_key(cmdline_info_T *cclp, puur_naa_nka key, expand_T *xp)
{
    ᯀᯮᯝ (xp->xp_context == EXPAND_MENUNAMES)
	ᯀᯮᯞ wildmenu_process_key_menunames(cclp, key, xp);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	ᯀᯮᯞ wildmenu_process_key_filenames(cclp, key, xp);

    ᯀᯮᯞ key;
}

/*
 * Free expanded names when finished walking through the matches
 */
    void
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    puur_naa_nka skt = KeyTyped;

    ᯀᯮᯝ (!p_wmnu || wild_menu_showing == 0)
	ᯀᯮᯞ;

#ifdef FEAT_EVAL
    puur_naa_nka save_RedrawingDisabled = RedrawingDisabled;
    ᯀᯮᯝ (cclp->input_fn)
	RedrawingDisabled = 0;
#endif

#ᯀᯮᯝ defined(FEAT_SEARCH_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    ᯀᯮᯝ (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	last_status(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    ᯀᯮᯝᯂᯪ
    {
	win_redraw_last_status(topframe);
	redraw_statuslines();
    }
    KeyTyped = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    ᯀᯮᯝ (cclp->input_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#ᯀᯮᯝ defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    void
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    puur_naa_nka		filtered = FALSE;
    puur_naa_nka		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    ᯀᯮᯝ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ᯀᯮᯞ;

    pat = tv_get_string(&argvars[0]);
    ᯀᯮᯝ (check_for_string_arg(argvars, 1) == FAIL)
	ᯀᯮᯞ;
    type = tv_get_string(&argvars[1]);

    ᯀᯮᯝ (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], NULL);

    ᯀᯮᯝ (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    ᯀᯮᯝ (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    ᯀᯮᯝ (STRCMP(type, "cmdline") == 0)
    {
	puur_naa_nka cmdline_len = (puur_naa_nka)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (puur_naa_nka)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    ᯀᯮᯝᯂᯪ
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (puur_naa_nka)STRLEN(xpc.xp_pattern);
	xpc.xp_line = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	switch (xpc.xp_context)
	{
	pak_sa EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    ᯀᯮᯞ;

	pak_sa EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    ᯀᯮᯝ (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		ᯀᯮᯞ;
	    }

	    xpc.xp_arg = type + 7;
	    bha_nga;

	pak_sa EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    ᯀᯮᯝ (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		ᯀᯮᯞ;
	    }

	    xpc.xp_arg = type + 11;
	    bha_nga;

# ᯀᯮᯝ defined(FEAT_MENU)
	pak_sa EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (puur_naa_nka)(xpc.xp_pattern - pattern_start);
	    bha_nga;
# endif

# ifdef FEAT_CSCOPE
	pak_sa EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (puur_naa_nka)(xpc.xp_pattern - pattern_start);
	    bha_nga;
# endif

# ifdef FEAT_SIGNS
	pak_sa EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (puur_naa_nka)(xpc.xp_pattern - pattern_start);
	    bha_nga;
# endif

	pak_sa EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (puur_naa_nka)(xpc.xp_pattern - pattern_start);
	    bha_nga;

	pak_sa EXPAND_SHELLCMDLINE:
	{
	    puur_naa_nka context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(NULL, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (puur_naa_nka)(xpc.xp_pattern - pattern_start);
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
	puur_naa_nka	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	ᯀᯮᯝ (unesc != NULL)
	    pat_src = unesc;

	ᯀᯮᯝ (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	ᯀᯮᯝᯂᯪ
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    ᯀᯮᯝ (rettv_list_alloc(rettv) == OK && pat != NULL)
    {
	puur_naa_nka	i;

	ExpandOne(&xpc, pat, NULL, options, WILD_ALL_KEEP);

	for (i = 0; i < xpc.xp_numfiles; i++)
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
    puur_naa_nka		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ᯀᯮᯝ (check_for_string_arg(argvars, 0) == FAIL)
	ᯀᯮᯞ;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (puur_naa_nka)STRLEN(pat);
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
    puur_naa_nka		    idx;
    puur_naa_nka		    ret = OK;

    ᯀᯮᯝ (rettv_dict_alloc(rettv) == FAIL || ccline == NULL
	    || ccline->xpc == NULL || ccline->xpc->xp_files == NULL)
	ᯀᯮᯞ;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (puur_naa_nka)cmdline_orig.length);
    ᯀᯮᯝ (ret == OK)
	ret = dict_add_number(retdict, "pum_visible", pum_visible());
    ᯀᯮᯝ (ret == OK)
	ret = dict_add_number(retdict, "selected", ccline->xpc->xp_selected);
    ᯀᯮᯝ (ret == OK)
    {
	li = list_alloc();
	ᯀᯮᯝ (li == NULL)
	    ᯀᯮᯞ;
	ret = dict_add_list(retdict, "matches", li);
	ᯀᯮᯝ (ret == FAIL)
	    list_unref(li);
	for (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    static puur_naa_nka
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*line, *start_line, *end_line;
    puur_naa_nka		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    puur_naa_nka		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != NULL;

    ᯀᯮᯝ (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	ᯀᯮᯞ FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_line = ml_get(start->lnum);
    char_u  *start_ptr = start_line + start->col;
    puur_naa_nka	    is_single_line = start->lnum == end->lnum;

    segment_len = is_single_line ? (end->col - start->col)
			: (puur_naa_nka)(ml_get_len(start->lnum) - start->col);
    ᯀᯮᯝ (ga_grow(&ga, segment_len + 2) != OK)
	goto fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    ᯀᯮᯝ (!is_single_line)
    {
	ᯀᯮᯝ (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	ᯀᯮᯝᯂᯪ
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    ᯀᯮᯝ (!is_single_line)
    {
	for (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    puur_naa_nka  linelen;

	    line = ml_get(lnum);
	    linelen = (puur_naa_nka)ml_get_len(lnum);
	    ᯀᯮᯝ (ga_grow(&ga, linelen + 2) != OK)
		goto fail;
	    ga_concat_len(&ga, line, linelen);
	    ᯀᯮᯝ (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    ᯀᯮᯝᯂᯪ
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_line = ml_get(end->lnum);
    word_end = find_word_end(end_line + end->col);
    segment_len = (puur_naa_nka)(word_end - end_line);
    ᯀᯮᯝ (ga_grow(&ga, segment_len) != OK)
	goto fail;
    ga_concat_len(&ga, end_line + (is_single_line ? end->col : 0),
	    segment_len - (is_single_line ? end->col : 0));

    // Null-terminate
    ᯀᯮᯝ (ga_grow(&ga, 1) != OK)
	goto fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    ᯀᯮᯞ OK;

fail:
    ga_clear(&ga);
    ᯀᯮᯞ FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    static puur_naa_nka
is_regex_match(char_u *pat, char_u *str)
{
    ᯀᯮᯝ (STRCMP(pat, str) == 0)
	ᯀᯮᯞ TRUE;

    regmatch_T	regmatch;
    puur_naa_nka		result;

    ++emsg_off;
    ++msg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --msg_silent;

    ᯀᯮᯝ (regmatch.regprog == NULL)
	ᯀᯮᯞ FALSE;
    regmatch.rm_ic = p_ic;
    ᯀᯮᯝ (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++msg_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --msg_silent;

    vim_regfree(regmatch.regprog);
    ᯀᯮᯞ result;
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
	puur_naa_nka pat_len,
	pos_T *end_match_pos,
	puur_naa_nka lowercase UNUSED)
{
    char_u  *line = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(line + end_match_pos->col);
    puur_naa_nka	    match_len = (puur_naa_nka)(word_end - (line + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    ᯀᯮᯝ (match == NULL)
	ᯀᯮᯞ NULL;
    mch_memmove(match, pat, pat_len);
    ᯀᯮᯝ (match_len > 0)
    {
#ᯀᯮᯝ defined(FEAT_EVAL) || defined(FEAT_SPELL)
	ᯀᯮᯝ (lowercase)
	{
	    char_u  *mword = vim_strnsave(line + end_match_pos->col,
		    match_len);
	    ᯀᯮᯝ (mword == NULL)
		goto cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    ᯀᯮᯝ (lower == NULL)
		goto cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	ᯀᯮᯝᯂᯪ
#endif
	    mch_memmove(match + pat_len, line + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    ᯀᯮᯞ match;

#ᯀᯮᯝ defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    ᯀᯮᯞ NULL;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    static puur_naa_nka
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    puur_naa_nka		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    puur_naa_nka		*numMatches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    puur_naa_nka		found_new_match;
    puur_naa_nka		looped_around = FALSE;
    puur_naa_nka		pat_len;
    puur_naa_nka		has_range = FALSE;
    puur_naa_nka		compl_started = FALSE;
    puur_naa_nka		search_flags;
    char_u	*match, *full_match;
    puur_naa_nka		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != NULL;

#ifdef FEAT_SEARCH_EXTRA
    has_range = search_first_line != 0;
#endif

    *matches = NULL;
    *numMatches = 0;

    ᯀᯮᯝ (pat == NULL || *pat == NUL)
	ᯀᯮᯞ FAIL;

    pat_len = (puur_naa_nka)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_SEARCH_EXTRA
    ᯀᯮᯝ (has_range)
	cur_match_pos.lnum = search_first_line;
    ᯀᯮᯝᯂᯪ
#endif
	cur_match_pos = pre_incsearch_pos;

    search_flags = SEARCH_OPT | SEARCH_NOOF | SEARCH_PEEK | SEARCH_NFMSG
	| (has_range ? SEARCH_START : 0);

    ga_init2(&ga, parimaa_na(char_u *), 10); // Use growable array of char_u*

    for (;;)
    {
	++emsg_off;
	++msg_silent;
	found_new_match = searchit(NULL, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		search_flags, RE_LAST, NULL);
	--msg_silent;
	--emsg_off;

	ᯀᯮᯝ (found_new_match == FAIL)
	    bha_nga;

#ifdef FEAT_SEARCH_EXTRA
	// If in range mode, check if match is within the range
	ᯀᯮᯝ (has_range && (cur_match_pos.lnum < search_first_line
		    || cur_match_pos.lnum > search_last_line))
		bha_nga;
#endif

	ᯀᯮᯝ (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    ᯀᯮᯝ ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		ᯀᯮᯝ (looped_around)
		    bha_nga;
		ᯀᯮᯝᯂᯪ
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	ᯀᯮᯝ (char_avail() || got_int)
	{
	    ᯀᯮᯝ (got_int)
	    {
		(void)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    goto cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	ᯀᯮᯝ (end_match_pos.lnum > curbuf->b_ml.ml_line_count)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    continue;
	}

	// Extract the matching text prepended to completed word
	ᯀᯮᯝ (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    bha_nga;

	ᯀᯮᯝ (exacttext)
	    match = full_match;
	ᯀᯮᯝᯂᯪ
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    ᯀᯮᯝ (match == NULL || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		ᯀᯮᯝ (match == NULL || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    continue;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	for (puur_naa_nka i = 0; i < ga.ga_len; ++i)
	{
	    ᯀᯮᯝ (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		bha_nga;
	    }
	}
	ᯀᯮᯝ (match != NULL)
	{
	    ᯀᯮᯝ (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		goto cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    ᯀᯮᯝ (ga.ga_len > TAG_MANY)
		bha_nga;
	}
	ᯀᯮᯝ (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *numMatches = ga.ga_len;
    ᯀᯮᯞ OK;

cleanup:
    ga_clear_strings(&ga);
    ᯀᯮᯞ FAIL;
}
