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

#ଅନ୍ତର୍ଭାଵ "vim.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cmd_showtail;	// Only show path tail in lists ?
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	may_expand_pattern = FALSE;
ସ୍ଥିର pos_T	pre_incsearch_pos; // Cursor position when incsearch started

ସ୍ଥିର ଶୂନ୍ୟ	set_context_for_wildcard_arg(exarg_T *eap, char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା usefilter, expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *complp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ExpandFromContext(expand_T *xp, char_u *, char_u ***, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର char_u	*showmatches_gettail(char_u *s);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	expand_showtail(expand_T *xp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	expand_shellcmd(char_u *filepat, char_u ***matches, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flagsarg);
#ଯଦି defined(FEAT_EVAL)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ExpandUserList(expand_T *xp, char_u ***matches, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches);
ସ୍ଥିର char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
ସ୍ଥିର char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, ପୂର୍ଣ୍ଣସଂଖ୍ୟା srclen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *new_lenp);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	expand_pattern_in_buf(char_u *pat, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, char_u ***matches, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ସ୍ଥିର pumitem_T *compl_match_array = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compl_match_arraysize;
// First column in cmdline of the matched item for completion.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compl_startcol;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compl_selected;
// cmdline before expansion
ସ୍ଥିର string_T cmdline_orig = {ଖାଲି, 0};

#define SHOW_MATCH(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    ବାଛ (xp->xp_context)
    {
    ସ୍ଥିତି EXPAND_BOOL_SETTINGS:
    ସ୍ଥିତି EXPAND_COLORS:
    ସ୍ଥିତି EXPAND_COMPILER:
    ସ୍ଥିତି EXPAND_DIRECTORIES:
    ସ୍ଥିତି EXPAND_DIRS_IN_CDPATH:
    ସ୍ଥିତି EXPAND_FILES:
    ସ୍ଥିତି EXPAND_FILES_IN_PATH:
    ସ୍ଥିତି EXPAND_FILETYPE:
    ସ୍ଥିତି EXPAND_FILETYPECMD:
    ସ୍ଥିତି EXPAND_HELP:
    ସ୍ଥିତି EXPAND_KEYMAP:
    ସ୍ଥିତି EXPAND_OLD_SETTING:
    ସ୍ଥିତି EXPAND_STRING_SETTING:
    ସ୍ଥିତି EXPAND_SETTING_SUBTRACT:
    ସ୍ଥିତି EXPAND_OWNSYNTAX:
    ସ୍ଥିତି EXPAND_PACKADD:
    ସ୍ଥିତି EXPAND_RUNTIME:
    ସ୍ଥିତି EXPAND_SHELLCMD:
    ସ୍ଥିତି EXPAND_SHELLCMDLINE:
    ସ୍ଥିତି EXPAND_TAGS:
    ସ୍ଥିତି EXPAND_TAGS_LISTFILES:
	ଫେରନ୍ତୁ FALSE;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ vim_strchr(p_wop, WOP_FUZZY) != ଖାଲି;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    ଫେରନ୍ତୁ vim_strchr(p_wop, WOP_FUZZY) != ଖାଲି && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sort_func_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    ଯଦି (*p1 != '<' && *p2 == '<') ଫେରନ୍ତୁ -1;
    ଯଦି (*p1 == '<' && *p2 != '<') ଫେରନ୍ତୁ 1;
    ଫେରନ୍ତୁ STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
wildescape(
    expand_T	*xp,
    char_u	*str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		numfiles,
    char_u	**files)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_BUFFER : VSE_NONE;

    ଯଦି (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    ଯଦି (xp->xp_backslash & XP_BS_THREE)
	    {
		ଅକ୍ଷର *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		ଯଦି (p != ଖାଲି)
		{
		    vim_free(files[i]);
		    files[i] = p;
#ଯଦି defined(BACKSLASH_IN_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    ଯଦି (p != ଖାଲି)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#endif
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (xp->xp_backslash & XP_BS_COMMA)
	    {
		ଯଦି (vim_strchr(files[i], ',') != ଖାଲି)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    ଯଦି (p != ଖାଲି)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#ifdef BACKSLASH_IN_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#ଅନ୍ୟଥା
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#endif
	    ଯଦି (p != ଖାଲି)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }

	    // If 'str' starts with "\~", replace "~" at start of
	    // files[i] with "\~".
	    ଯଦି (str[0] == '\\' && str[1] == '~' && files[i][0] == '~')
		escape_fname(&files[i]);
	}
	xp->xp_backslash = XP_BS_NONE;

	// If the first file starts with a '+' escape it.  Otherwise it
	// could be seen as "+cmd".
	ଯଦି (*files[0] == '+')
	    escape_fname(&files[0]);
    }
    ଅନ୍ୟଥା ଯଦି (xp->xp_context == EXPAND_TAGS)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    ଯଦି (p != ଖାଲି)
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
    ସ୍ଥିର ଶୂନ୍ୟ
ExpandEscape(
    expand_T	*xp,
    char_u	*str,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		numfiles,
    char_u	**files,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options)
{
    // May change home directory back to "~"
    ଯଦି (options & WILD_HOME_REPLACE)
	tilde_replace(str, numfiles, files);

    ଯଦି (options & WILD_ESCAPE)
	wildescape(xp, str, numfiles, files);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nextwild(
    expand_T	*xp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options,	// extra options for ExpandOne()
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		escape)		// if TRUE, escape the returned matches
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		from_wildtrigger_func = options & WILD_FUNC_TRIGGER;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wild_navigate = (type == WILD_NEXT || type == WILD_PREV
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    ଯଦି (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#ifdef FEAT_EVAL
	ଯଦି (ccline->ନିବେଶ_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    set_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	ଅନ୍ୟଥା
#endif
	{
	    may_expand_pattern = options & WILD_MAY_EXPAND_PATTERN;
	    set_expand_context(xp);
	    may_expand_pattern = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    ଯଦି (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ଫେରନ୍ତୁ OK;  // Something illegal on command line
    }
    ଯଦି (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ଫେରନ୍ତୁ FAIL;
    }

    i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(xp->xp_pattern - ccline->cmdbuff);
    xp->xp_pattern_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    ଯଦି (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_pattern_len == 0)
	ଫେରନ୍ତୁ FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    ଯଦି (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	ସନ୍ଦେଶ_puts("...");	    // show that we are busy
	out_flush();
    }

    ଯଦି (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, ଖାଲି, ଖାଲି, 0, type);
    }
    ଅନ୍ୟଥା
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_pattern;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	pat_len = xp->xp_pattern_len;
#ଯଦି defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	ଯଦି (unesc != ଖାଲି)
	    pat_src = unesc;
#endif

	ଯଦି (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_PATTERN_IN_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	ଅନ୍ୟଥା
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#ଯଦି defined(FEAT_EVAL)
	vim_free(unesc);
#endif

	// Translate string into pattern and expand it.
	ଯଦି (tmp == ଖାଲି)
	    p = ଖାଲି;
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_options = options |
		    WILD_HOME_REPLACE|WILD_ADD_SLASH|WILD_SILENT;
	    ଯଦି (escape)
		use_options |= WILD_ESCAPE;
	    ଯଦି (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_pattern_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    ଯଦି (p != ଖାଲି && type == WILD_LONGEST)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	j;

		ପାଇଁ (j = 0; j < xp->xp_pattern_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    ଯଦି (c == '*' || c == '?')
			ଭାଙ୍ଗ;
		}
		ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    ଯଦି (!wild_navigate && ccline->cmdbuff != ଖାଲି)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	ଯଦି (cmdline_orig.string == ଖାଲି)
	    cmdline_orig.length = 0;
	ଅନ୍ୟଥା
	    cmdline_orig.length = ccline->cmdlen;
    }

    ଯଦି (p != ଖାଲି && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	difflen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	v = OK;

	difflen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)plen - xp->xp_pattern_len;
	ଯଦି (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_pattern = ccline->cmdbuff + i;
	}

	ଯଦି (v == OK)
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
    ଯଦି (xp->xp_context == EXPAND_MAPPINGS && p == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (xp->xp_numfiles <= 0 && p == ଖାଲି)
	beep_flush();
    ଅନ୍ୟଥା ଯଦି (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(ଶୂନ୍ୟ)ExpandOne(xp, ଖାଲି, ଖାଲି, 0, WILD_FREE);

    vim_free(p);

    ଫେରନ୍ତୁ OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_Matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		showtail)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prefix_len;

    // Add all the completion matches
    compl_match_array = ALLOC_MULT(pumitem_T, ଅଙ୍କ_Matches);
    ଯଦି (compl_match_array == ଖାଲି)
	ଫେରନ୍ତୁ EXPAND_UNSUCCESSFUL;

    compl_match_arraysize = ଅଙ୍କ_Matches;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_Matches; i++)
    {
	compl_match_array[i].pum_text = (xp->xp_files_abbr != ଖାଲି
				    && xp->xp_files_abbr[i] != ଖାଲି)
					? xp->xp_files_abbr[i]
					: SHOW_MATCH(i);
	compl_match_array[i].pum_info = xp->xp_files_info != ଖାଲି
					    ? xp->xp_files_info[i] : ଖାଲି;
	compl_match_array[i].pum_extra = xp->xp_files_menu != ଖାଲି
					    ? xp->xp_files_menu[i] : ଖାଲି;
	compl_match_array[i].pum_kind = xp->xp_files_kind != ଖାଲି
					    ? xp->xp_files_kind[i] : ଖାଲି;
	compl_match_array[i].pum_user_abbr_hlattr = -1;
	compl_match_array[i].pum_user_kind_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == ଖାଲି ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    prefix_len = vim_strsize(xp->xp_pattern);
    ଯଦି (showtail)
	prefix_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_col_off + MAX(0, compl_startcol - prefix_len);

    ଫେରନ୍ତୁ EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    ଶୂନ୍ୟ
cmdline_pum_display(ଶୂନ୍ୟ)
{
    ଯଦି (p_po > 0 && p_po < 100 && !pum_redraw_in_same_position())
	pum_call_update_screen();
    pum_display(compl_match_array, compl_match_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_pum_active(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ pum_visible() && compl_match_array != ଖାଲି;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    ଶୂନ୍ୟ
cmdline_pum_remove(cmdline_info_T *cclp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା defer_redraw)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_ଚାବି_Typed = ଚାବି_Typed;
#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_RedrawingDisabled = RedrawingDisabled;
    ଯଦି (cclp->ନିବେଶ_fn)
	RedrawingDisabled = 0;
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_match_array);
    compl_match_arraysize = 0;
    ଯଦି (!defer_redraw)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    ଅନ୍ୟଥା
	pum_call_update_screen();
    redrawcmd();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    ଚାବି_Typed = save_ଚାବି_Typed;
#ifdef FEAT_EVAL
    ଯଦି (cclp->ନିବେଶ_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

    ଶୂନ୍ୟ
cmdline_pum_cleanup(cmdline_info_T *cclp)
{
    cmdline_pum_remove(cclp, FALSE);
    wildmenu_cleanup(cclp);
}

/*
 * Returns the starting column number to use for the cmdline completion popup
 * menu.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_compl_startcol(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_pattern(ଶୂନ୍ୟ)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ଫେରନ୍ତୁ xp == ଖାଲି ? ଖାଲି : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_compl_is_fuzzy(ଶୂନ୍ୟ)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    ଫେରନ୍ତୁ xp != ଖାଲି && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
skip_status_match_char(expand_T *xp, char_u *s)
{
    ଯଦି ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_PATTERN_IN_BUF)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	ଯଦି (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    ଫେରନ୍ତୁ 2;
#endif
	ଫେରନ୍ତୁ 1;
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
status_match_len(expand_T *xp, char_u *s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len = 0;

#ifdef FEAT_MENU
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    ଯଦି (emenu && menu_is_separator(s))
	ଫେରନ୍ତୁ 1;
#endif

    ଯେତେବେଳେ (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    ଫେରନ୍ତୁ len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
win_redr_status_matches(
    expand_T	*xp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_matches,
    char_u	**matches,	// list of matches
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		showtail)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row;
    char_u	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		clen;		// length in screen cells
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fillchar;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		highlight = TRUE;
    char_u	*selstart = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		selstart_col = 0;
    char_u	*selend = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	first_match = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_ବାମ = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		emenu;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

    ଯଦି (matches == ଖାଲି)	// interrupted completion?
	ଫେରନ୍ତୁ;

    ଯଦି (has_mbyte)
	buf = alloc(topframe->fr_width * MB_MAXBYTES + 1);
    ଅନ୍ୟଥା
	buf = alloc(topframe->fr_width + 1);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (match == -1)	// don't show match but original text
    {
	match = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = status_match_len(xp, SHOW_MATCH(match)) + 3;
    ଯଦି (match == 0)
	first_match = 0;
    ଅନ୍ୟଥା ଯଦି (match < first_match)
    {
	// jumping left, as far as we can go
	first_match = match;
	add_ବାମ = TRUE;
    }
    ଅନ୍ୟଥା
    {
	// check if match fits on the screen
	ପାଇଁ (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
	ଯଦି (first_match > 0)
	    clen += 2;
	// jumping right, put match at the left
	ଯଦି (clen > topframe->fr_width)
	{
	    first_match = match;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    ପାଇଁ (i = match; i < ଅଙ୍କ_matches; ++i)
	    {
		clen += status_match_len(xp, SHOW_MATCH(i)) + 2;
		ଯଦି (clen >= topframe->fr_width)
		    ଭାଙ୍ଗ;
	    }
	    ଯଦି (i == ଅଙ୍କ_matches)
		add_ବାମ = TRUE;
	}
    }
    ଯଦି (add_ବାମ)
	ଯେତେବେଳେ (first_match > 0)
	{
	    clen += status_match_len(xp, SHOW_MATCH(first_match - 1)) + 2;
	    ଯଦି (clen >= topframe->fr_width)
		ଭାଙ୍ଗ;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, curwin);

    ଯଦି (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    ଅନ୍ୟଥା
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    ଯେତେବେଳେ (clen + status_match_len(xp, SHOW_MATCH(i)) + 2 < topframe->fr_width)
    {
	ଯଦି (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = SHOW_MATCH(i);
	// Check for menu separators - replace with '|'
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	ଯଦି (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	ଅନ୍ୟଥା
#endif
	ପାଇଁ ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
	    ଯଦି (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    ଅନ୍ୟଥା
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(buf + len);
	    }
	}
	ଯଦି (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	ଯଦି (++i == ଅଙ୍କ_matches)
		ଭାଙ୍ଗ;
    }

    ଯଦି (i != ଅଙ୍କ_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    ଯଦି (row >= 0)
    {
	ଯଦି (wild_menu_showing == 0)
	{
	    ଯଦି (ସନ୍ଦେଶ_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		ଯଦି (cmdline_row == Rows - 1)
		{
		    screen_del_ଧାଡ଼ିଗୁଡ଼ିକ(0, 0, 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows, TRUE, 0, ଖାଲି);
		    ++ସନ୍ଦେଶ_scrolled;
		}
		ଅନ୍ୟଥା
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    ଅନ୍ୟଥା
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		ଯଦି (lastwin->w_status_height == 0)
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
	ଯଦି (selstart != ଖାଲି && highlight)
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

#ଯଦି defined(FEAT_EVAL)
/*
 * Apply -completeopt=escape to a string about to be inserted into the command
 * line as a completion result.  If "str" is non-NULL and the active expansion
 * context is a customlist/custom user command with UCC_ESCAPE set, free "str"
 * and return a newly-allocated copy with spaces, tabs and backslashes prefixed
 * by a backslash.  Otherwise return "str" unchanged.
 */
    ସ୍ଥିର char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ଫେରନ୍ତୁ str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ str;
    vim_free(str);
    ଫେରନ୍ତୁ p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    ସ୍ଥିର char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		srclen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*new_lenp)
{
    char_u  *buf, *p, *d, *end;

    ଯଦି ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	ଫେରନ୍ତୁ ଖାଲି;

    buf = alloc(srclen + 1);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    d = buf;
    end = src + srclen;
    ପାଇଁ (p = src; p < end; ++p)
    {
	ଯଦି (*p == '\\' && p + 1 < end
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(d - buf);
    ଫେରନ୍ତୁ buf;
}
#endif

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    ସ୍ଥିର char_u *
get_next_or_prev_match(ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode, expand_T *xp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    findex = xp->xp_selected;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ht;

    // When no matches found, return NULL
    ଯଦି (xp->xp_numfiles <= 0)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (mode == WILD_PREV)
    {
	// Select the last entry if at original text
	ଯଦି (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    ଅନ୍ୟଥା ଯଦି (mode == WILD_NEXT)
    {
	// Select the next entry
	++findex;
    }
    ଅନ୍ୟଥା   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_height();
	ଯଦି (ht > 3)
	    ht -= 2;

	ଯଦି (mode == WILD_PAGEUP)
	{
	    ଯଦି (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    ଅନ୍ୟଥା ଯଦି (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    ଅନ୍ୟଥା
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	ଅନ୍ୟଥା    // mode == WILD_PAGEDOWN
	{
	    ଯଦି (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    ଅନ୍ୟଥା ଯଦି (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    ଅନ୍ୟଥା
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    ଯଦି (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	ଯଦି (xp->xp_orig != ଖାଲି)
	    findex = -1;
	ଅନ୍ୟଥା
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    ଯଦି (p_wmnu)
    {
	ଯଦି (compl_match_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	ଅନ୍ୟଥା ଯଦି (vim_strchr(p_wop, WOP_PUM) != ଖାଲି)
	{
	    ଯଦି (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	ଅନ୍ୟଥା
	    win_redr_status_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    ଯଦି (findex == -1)
	ଫେରନ୍ତୁ vim_strsave(xp->xp_orig);
#ଯଦି defined(FEAT_EVAL)
    ଫେରନ୍ତୁ apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ vim_strsave(xp->xp_files[findex]);
#endif
}

/*
 * Start the command-line expansion and get the matches.
 */
    ସ୍ଥିର char_u *
ExpandOne_start(ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode, expand_T *xp, char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		non_suf_match;		// number without matching suffix
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*ss = ଖାଲି;

    // Do the expansion.
    ଯଦି (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#ifdef FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	ଯଦି (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_match_str_2), str);
#endif
    }
    ଅନ୍ୟଥା ଯଦି (xp->xp_numfiles == 0)
    {
	ଯଦି (!(options & WILD_SILENT))
	    semsg(_(e_no_match_str_2), str);
    }
    ଅନ୍ୟଥା
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	ଯଦି (mode != WILD_ALL && mode != WILD_ALL_KEEP && mode != WILD_LONGEST)
	{
	    ଯଦି (xp->xp_numfiles)
		non_suf_match = xp->xp_numfiles;
	    ଅନ୍ୟଥା
		non_suf_match = 1;
	    ଯଦି ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_match = 0;
		ପାଇଁ (i = 0; i < 2; ++i)
		    ଯଦି (match_suffix(xp->xp_files[i]))
			++non_suf_match;
	    }
	    ଯଦି (non_suf_match != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		ଯଦି (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_names));
		ଅନ୍ୟଥା ଯଦି (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    ଯଦି (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    ଫେରନ୍ତୁ ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    ସ୍ଥିର char_u *
find_longest_match(expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    long_u	len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mb_len = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c0, ci;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*ss;

    ପାଇଁ (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	ଯଦି (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	ଅନ୍ୟଥା
	    c0 = xp->xp_files[0][len];
	ପାଇଁ (i = 1; i < xp->xp_numfiles; ++i)
	{
	    ଯଦି (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    ଅନ୍ୟଥା
		ci = xp->xp_files[i][len];
	    ଯଦି (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		ଯଦି (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (c0 != ci)
		ଭାଙ୍ଗ;
	}
	ଯଦି (i < xp->xp_numfiles)
	{
	    ଯଦି (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    ଭାଙ୍ଗ;
	}
    }

    ss = alloc(len + 1);
    ଯଦି (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    ଫେରନ୍ତୁ ss;
}

    ଶୂନ୍ୟ
free_xp_files_extra(expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା numfiles)
{
    ଯଦି (xp->xp_files_abbr != ଖାଲି)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = ଖାଲି;
    }
    ଯଦି (xp->xp_files_kind != ଖାଲି)
    {
	FreeWild(numfiles, xp->xp_files_kind);
	xp->xp_files_kind = ଖାଲି;
    }
    ଯଦି (xp->xp_files_menu != ଖାଲି)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = ଖାଲି;
    }
    ଯଦି (xp->xp_files_info != ଖାଲି)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = ଖାଲି;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode)
{
    char_u	*ss = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orig_saved = FALSE;
#ଯଦି defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ss_is_match = FALSE;
#endif

    // first handle the case of using an old match
    ଯଦି (mode == WILD_NEXT || mode == WILD_PREV
	    || mode == WILD_PAGEUP || mode == WILD_PAGEDOWN)
	ଫେରନ୍ତୁ get_next_or_prev_match(mode, xp);

    ଯଦି (mode == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    ଅନ୍ୟଥା ଯଦି (mode == WILD_APPLY)
    {
	ଯଦି (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	ଅନ୍ୟଥା
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#ଯଦି defined(FEAT_EVAL)
	    ss_is_match = TRUE;
#endif
	}
    }

    // free old names
    ଯଦି (xp->xp_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	ଯଦି (compl_match_array != ଖାଲି)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    ଯଦି (mode == WILD_FREE)	// only release file name
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (xp->xp_numfiles == -1 && mode != WILD_APPLY && mode != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = ExpandOne_start(mode, xp, str, options);
#ଯଦି defined(FEAT_EVAL)
	ଯଦି (ss != ଖାଲି)
	    ss_is_match = TRUE;
#endif
    }

    // Find longest common part
    ଯଦି (mode == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = find_longest_match(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#ଯଦି defined(FEAT_EVAL)
	ଯଦି (ss != ଖାଲି)
	    ss_is_match = TRUE;
#endif
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    ଯଦି (mode == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	ଅକ୍ଷର	*prefix = "";
	ଅକ୍ଷର	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	n = xp->xp_numfiles - 1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
#ଯଦି defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = ଖାଲି;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	ଯଦି ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    ଯଦି (escaped != ଖାଲି)
	    {
		ପାଇଁ (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    ଯଦି (escaped[i] == ଖାଲି)
		    {
			ଯେତେବେଳେ (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			ଭାଙ୍ଗ;
		    }
		}
		ଯଦି (escaped != ଖାଲି)
		    files = escaped;
	    }
	}
#ଅନ୍ୟଥା
	char_u	**files = xp->xp_files;
#endif

	ଯଦି (xp->xp_prefix == XP_PREFIX_NO)
	{
	    prefix = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	ଅନ୍ୟଥା ଯଦି (xp->xp_prefix == XP_PREFIX_INV)
	{
	    prefix = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	ପାଇଁ (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	ଯଦି (ss != ଖାଲି)
	{
	    size_t  ss_len = 0;

	    ପାଇଁ (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (ଅକ୍ଷର *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? prefix : "",
		    (ଅକ୍ଷର *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#ଯଦି defined(FEAT_EVAL)
	ଯଦି (escaped != ଖାଲି)
	{
	    ପାଇଁ (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#endif
    }

    ଯଦି (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    ଯଦି (!orig_saved)
	vim_free(orig);

#ଯଦି defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    ଯଦି (ss_is_match && mode != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#endif
    ଫେରନ୍ତୁ ss;
}

/*
 * Prepare an expand structure for use.
 */
    ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
ExpandCleanup(expand_T *xp)
{
    ଯଦି (xp->xp_numfiles >= 0)
    {
	free_xp_files_extra(xp, xp->xp_numfiles);
	FreeWild(xp->xp_numfiles, xp->xp_files);
	xp->xp_numfiles = -1;
    }
    VIM_CLEAR(xp->xp_orig);
}

    ଶୂନ୍ୟ
clear_cmdline_orig(ଶୂନ୍ୟ)
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
    ସ୍ଥିର ଶୂନ୍ୟ
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_Matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ିଗୁଡ଼ିକ,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		linenr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		showtail,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir_attr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		isdir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lastlen;
    char_u	*p;

    lastlen = 999;
    ପାଇଁ (j = linenr; j < ଅଙ୍କ_Matches; j += ଧାଡ଼ିଗୁଡ଼ିକ)
    {
	ଯଦି (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    ସନ୍ଦେଶ_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    ସନ୍ଦେଶ_advance(maxlen + 1);
	    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)p);
	    ସନ୍ଦେଶ_advance(maxlen + 3);
	    ସନ୍ଦେଶ_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    ଭାଙ୍ଗ;
	}
	ପାଇଁ (i = maxlen - lastlen; --i >= 0; )
	    ସନ୍ଦେଶ_putchar(' ');
	ଯଦି (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    ଯଦି (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_path;
		char_u	*path;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_path = expand_env_save_opt(matches[j], TRUE, ଖାଲି);
		path = exp_path != ଖାଲି ? exp_path : matches[j];
		halved_slash = backslash_halve_save(path);
		isdir = mch_isdir(halved_slash != ଖାଲି ? halved_slash
			: matches[j]);
		vim_free(exp_path);
		ଯଦି (halved_slash != path)
		    vim_free(halved_slash);
	    }
	    ଅନ୍ୟଥା
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    ଯଦି (showtail)
		p = SHOW_MATCH(j);
	    ଅନ୍ୟଥା
	    {
		home_replace(ଖାଲି, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	ଅନ୍ୟଥା
	{
	    isdir = FALSE;
	    p = SHOW_MATCH(j);
	}
	lastlen = ସନ୍ଦେଶ_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    ଯଦି (ସନ୍ଦେଶ_col > 0)	// when not wrapped around
    {
	ସନ୍ଦେଶ_clr_eos();
	ସନ୍ଦେଶ_putchar('\n');
    }
    out_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_Matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		showtail,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noselect)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	retval = cmdline_pum_create(ccline, xp, matches, ଅଙ୍କ_Matches, showtail);

    ଯଦି (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    ଫେରନ୍ତୁ retval;
}

/*
 * Display completion matches.
 * Returns EXPAND_NOTHING when the character that triggered expansion should be
 *   inserted as a normal character.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
showmatches(
    expand_T	*xp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		display_wildmenu,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		display_list,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wim_flags_arg)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_Matches;
    char_u	**matches;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ିଗୁଡ଼ିକ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		columns;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		showtail;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noselect = (wim_flags_arg & WIM_NOSELECT);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noinsert = (wim_flags_arg & WIM_NOINSERT);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdline_unchanged = noselect || noinsert;

    ଯଦି (xp->xp_numfiles == -1)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval;
	set_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&ଅଙ୍କ_Matches, &matches);
	ଯଦି (retval != EXPAND_OK)
	    ଫେରନ୍ତୁ retval;
	showtail = expand_showtail(xp);
    }
    ଅନ୍ୟଥା
    {
	ଅଙ୍କ_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    ଯଦି (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != ଖାଲି)
	ଫେରନ୍ତୁ show_pum_matches(ccline, xp, matches, ଅଙ୍କ_Matches,
		showtail && !cmdline_unchanged, noselect);

    ଯଦି (display_list)
    {
	ସନ୍ଦେଶ_didany = FALSE;		// lines_left will be set
	ସନ୍ଦେଶ_start();			// prepare for paging
	ସନ୍ଦେଶ_putchar('\n');
	out_flush();
	cmdline_row = ସନ୍ଦେଶ_row;
	ସନ୍ଦେଶ_didany = FALSE;		// lines_left will be set again
	ସନ୍ଦେଶ_start();			// prepare for paging
    }

    ଯଦି (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    ଅନ୍ୟଥା ଯଦି (display_wildmenu && !display_list)
	win_redr_status_matches(xp, ଅଙ୍କ_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    ଅନ୍ୟଥା ଯଦି (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	ପାଇଁ (i = 0; i < ଅଙ୍କ_Matches; ++i)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;
	    ଯଦି (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(ଖାଲି, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    ଅନ୍ୟଥା
		len = vim_strsize(SHOW_MATCH(i));
	    ଯଦି (len > maxlen)
		maxlen = len;
	}

	ଯଦି (xp->xp_context == EXPAND_TAGS_LISTFILES)
	    ଧାଡ଼ିଗୁଡ଼ିକ = ଅଙ୍କ_Matches;
	ଅନ୍ୟଥା
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_width + 2) / maxlen;
	    ଯଦି (columns < 1)
		columns = 1;
	    ଧାଡ଼ିଗୁଡ଼ିକ = (ଅଙ୍କ_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	ଯଦି (xp->xp_context == EXPAND_TAGS_LISTFILES)
	{
	    ସନ୍ଦେଶ_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    ସନ୍ଦେଶ_clr_eos();
	    ସନ୍ଦେଶ_advance(maxlen - 3);
	    ସନ୍ଦେଶ_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	ପାଇଁ (i = 0; i < ଧାଡ଼ିଗୁଡ଼ିକ; ++i)
	{
	    showmatches_oneline(xp, matches, ଅଙ୍କ_Matches, ଧାଡ଼ିଗୁଡ଼ିକ, i,
						maxlen, showtail, attr);
	    ଯଦି (got_int)
	    {
		got_int = FALSE;
		ଭାଙ୍ଗ;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_row = ସନ୍ଦେଶ_row;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    ଯଦି (display_wildmenu && display_list)
    {
	ଯଦି (vim_strchr(p_wop, WOP_PUM) != ଖାଲି)
	    (ଶୂନ୍ୟ)show_pum_matches(ccline, xp, matches, ଅଙ୍କ_Matches,
		    showtail && !cmdline_unchanged, noselect);
	ଅନ୍ୟଥା
	    win_redr_status_matches(xp, ଅଙ୍କ_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    ଯଦି (xp->xp_numfiles == -1)
    {
	FreeWild(ଅଙ୍କ_Matches, matches);
	free_xp_files_extra(xp, ଅଙ୍କ_Matches);
    }

    ଫେରନ୍ତୁ EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    ସ୍ଥିର char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		had_sep = FALSE;

    ପାଇଁ (p = s; *p != NUL; )
    {
	ଯଦି (vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
		&& !rem_backslash(p)
#endif
	   )
	    had_sep = TRUE;
	ଅନ୍ୟଥା ଯଦି (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    ଫେରନ୍ତୁ t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*end;

    // When not completing file names a "/" may mean something different.
    ଯଦି (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	ଫେରନ୍ତୁ FALSE;

    end = gettail(xp->xp_pattern);
    ଯଦି (end == xp->xp_pattern)		// there is no path separator
	ଫେରନ୍ତୁ FALSE;

    ପାଇଁ (s = xp->xp_pattern; s < end; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	ଯଦି (rem_backslash(s))
	    ++s;
	ଅନ୍ୟଥା ଯଦି (vim_strchr((char_u *)"*?[", *s) != ଖାଲି)
	    ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		context)	// EXPAND_FILES etc.
{
    char_u	*retval;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_len;
    char_u	*tail;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ends_in_star;

    ଯଦି (context != EXPAND_FILES
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
	ଯଦି (context == EXPAND_FINDFUNC
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
	ଅନ୍ୟଥା
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    ପାଇଁ (i = 0; i < len; i++)
	    {
		ଯଦି (fname[i] == '*' || fname[i] == '~')
		    new_len++;		// '*' needs to be replaced by ".*"
					// '~' needs to be replaced by "\~"

		// Buffer names are like file names.  "." should be literal
		ଯଦି (context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		// "." becomes "\."

		// Custom expansion takes care of special things, match
		// backslashes literally (perhaps also for other types?)
		ଯଦି ((context == EXPAND_USER_DEFINED
			  || context == EXPAND_USER_LIST) && fname[i] == '\\')
		    new_len++;		// '\' becomes "\\"
	    }
	    retval = alloc(new_len);
	    ଯଦି (retval != ଖାଲି)
	    {
		retval[0] = '^';
		j = 1;
		ପାଇଁ (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    ଯଦି (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			ଭାଙ୍ଗ;

		    ବାଛ (fname[i])
		    {
			ସ୍ଥିତି '*':   retval[j++] = '.';
				    ଭାଙ୍ଗ;
			ସ୍ଥିତି '~':   retval[j++] = '\\';
				    ଭାଙ୍ଗ;
			ସ୍ଥିତି '?':   retval[j] = '.';
				    ଜାରି;
			ସ୍ଥିତି '.':   ଯଦି (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    ଭାଙ୍ଗ;
			ସ୍ଥିତି '\\':  ଯଦି (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    ଭାଙ୍ଗ;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	retval = alloc(len + 4);
	ଯଦି (retval != ଖାଲି)
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
	    ପାଇଁ (i = len - 2; i >= 0; --i)
	    {
		ଯଦି (retval[i] != '\\')
		    ଭାଙ୍ଗ;
		ends_in_star = !ends_in_star;
	    }
#endif
	    ଯଦି ((*retval != '~' || tail != retval)
		    && !ends_in_star
		    && vim_strchr(tail, '$') == ଖାଲି
		    && vim_strchr(retval, '`') == ଖାଲି)
		retval[len++] = '*';
	    ଅନ୍ୟଥା ଯଦି (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    ଫେରନ୍ତୁ retval;
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
    ଶୂନ୍ୟ
set_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    ଯଦି ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_pattern)
    {
	xp->xp_context = EXPAND_PATTERN_IN_BUF;
	xp->xp_ସନ୍ଧାନ_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_pattern_len = ccline->cmdpos;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	ସନ୍ଧାନ_first_ଧାଡ଼ି = 0; // Search entire buffer
#endif
	ଫେରନ୍ତୁ;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    ଯଦି (ccline->cmdfirstc != ':'
#ifdef FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->ନିବେଶ_fn
#endif
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	ଫେରନ୍ତୁ;
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
    ସ୍ଥିର char_u *
set_cmd_index(char_u *cmd, exarg_T *eap, expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *complp)
{
    char_u	*p = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fuzzy = cmdline_fuzzy_complete(cmd);

    // Isolate the command and search for it in the command table.
    // Exceptions:
    // - the 'k' command can directly be followed by any character, but do
    // accept "keepmarks", "keepalt" and "keepjumps". Bypass also when
    // 'ignorecase' is set so a lowercase ":kz" still completes a user
    // command like :Kz (#20241), and for fuzzy matching as that can find
    // matches anywhere in the command name.
    // - the 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
    ଯଦି (!fuzzy && !p_ic && (*cmd == 'k' && cmd[1] != 'e'))
    {
	eap->cmdidx = CMD_k;
	p = cmd + 1;
    }
    ଅନ୍ୟଥା
    {
	p = cmd;
	ଯେତେବେଳେ (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	ଯଦି (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    ଯେତେବେଳେ (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	ଯଦି (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    ଯେତେବେଳେ (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	ଯଦି (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != ଖାଲି)
	    ++p;
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - cmd);

	ଯଦି (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	ଯଦି ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    ଯେତେବେଳେ (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    ଯଦି (*p == NUL && ASCII_ISALNUM(p[-1]))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (eap->cmdidx == CMD_SIZE)
    {
	ଯଦି (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != ଖାଲି)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	ଅନ୍ୟଥା ଯଦି (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = find_ucmd(eap, p, ଖାଲି, xp, complp);
	    ଯଦି (p == ଖାଲି)
		eap->cmdidx = CMD_SIZE;	// ambiguous user command
	}
    }
    ଯଦି (eap->cmdidx == CMD_SIZE)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		usefilter,
	expand_T	*xp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*complp)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_quote = FALSE;
    char_u	*bow = ଖାଲି;	// Beginning of word
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_pattern = skipwhite(arg);
    p = xp->xp_pattern;
    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (has_mbyte)
	    c = mb_ptr2char(p);
	ଅନ୍ୟଥା
	    c = *p;
	ଯଦି (c == '\\' && p[1] != NUL)
	    ++p;
	ଅନ୍ୟଥା ଯଦି (c == '`')
	{
	    ଯଦି (!in_quote)
	    {
		xp->xp_pattern = p;
		bow = p + 1;
	    }
	    in_quote = !in_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	ଅନ୍ୟଥା ଯଦି (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != ଖାଲି && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    ଯେତେବେଳେ (*p != NUL)
	    {
		ଯଦି (has_mbyte)
		    c = mb_ptr2char(p);
		ଅନ୍ୟଥା
		    c = *p;
		ଯଦି (c == '`' || vim_isfilec_or_wc(c))
		    ଭାଙ୍ଗ;
		ଯଦି (has_mbyte)
		    len = (*mb_ptr2len)(p);
		ଅନ୍ୟଥା
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    ଯଦି (in_quote)
		bow = p;
	    ଅନ୍ୟଥା
		xp->xp_pattern = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    ଯଦି (bow != ଖାଲି && in_quote)
	xp->xp_pattern = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    ଯଦି (usefilter
	    || (eap != ଖାଲି
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#ifndef BACKSLASH_IN_FILENAME
	xp->xp_shell = TRUE;
#endif
	// When still after the command name expand executables.
	ଯଦି (xp->xp_pattern == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    ଯଦି (*xp->xp_pattern == '$')
    {
	ପାଇଁ (p = xp->xp_pattern + 1; *p != NUL; ++p)
	    ଯଦି (!vim_isIDc(*p))
		ଭାଙ୍ଗ;
	ଯଦି (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_pattern;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    ଯଦି (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    ଯଦି (*xp->xp_pattern == '~')
    {
	ପାଇଁ (p = xp->xp_pattern + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	ଯଦି (*p == NUL && p > xp->xp_pattern + 1
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
    ସ୍ଥିର char_u *
set_context_in_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    ଯଦି (p == ଖାଲି)
	xp->xp_pattern = arg;
    ଅନ୍ୟଥା
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    ଫେରନ୍ତୁ ଖାଲି;
}

#ifdef FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    ସ୍ଥିର char_u *
set_context_in_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    ଯଦି (p == ଖାଲି)
	xp->xp_pattern = arg;
    ଅନ୍ୟଥା
	xp->xp_pattern = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    ସ୍ଥିର char_u *
set_context_in_filter_cmd(expand_T *xp, char_u *arg)
{
    ଯଦି (*arg != NUL)
	arg = skip_vimgrep_pat(arg, ଖାଲି, ଖାଲି);
    ଯଦି (arg == ଖାଲି || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ skipwhite(arg);
}

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    ସ୍ଥିର char_u *
set_context_in_match_cmd(expand_T *xp, char_u *arg)
{
    ଯଦି (*arg == NUL || !ends_excmd(*arg))
    {
	// also complete "None"
	set_context_in_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	ଯଦି (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    ଫେରନ୍ତୁ find_nextcmd(arg);
}
#endif

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    ସ୍ଥିର char_u *
find_cmd_after_global_cmd(char_u *arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		delim;

    delim = *arg;	    // get the delimiter
    ଯଦି (delim)
	++arg;		    // skip delimiter if there is one

    ଯେତେବେଳେ (arg[0] != NUL && arg[0] != delim)
    {
	ଯଦି (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    ଯଦି (arg[0] != NUL)
	ଫେରନ୍ତୁ arg + 1;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    ସ୍ଥିର char_u *
find_cmd_after_substitute_cmd(char_u *arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		delim;

    delim = *arg;
    ଯଦି (delim)
    {
	// skip "from" part
	++arg;
	arg = skip_regexp(arg, delim, magic_isset());

	ଯଦି (arg[0] != NUL && arg[0] == delim)
	{
	    // skip "to" part
	    ++arg;
	    ଯେତେବେଳେ (arg[0] != NUL && arg[0] != delim)
	    {
		ଯଦି (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    ଯଦି (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    ଯେତେବେଳେ (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == ଖାଲି)
	++arg;
    ଯଦି (arg[0] != NUL)
	ଫେରନ୍ତୁ arg;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    ସ୍ଥିର char_u *
find_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    ଯଦି (*arg != '/')
	ଫେରନ୍ତୁ ଖାଲି;

    // Match regexp, not just whole words
    ପାଇଁ (++arg; *arg && *arg != '/'; arg++)
	ଯଦି (*arg == '\\' && arg[1] != NUL)
	    arg++;
    ଯଦି (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	ଯଦି (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == ଖାଲି)
	    xp->xp_context = EXPAND_NOTHING;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ arg;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

#ifdef FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    ସ୍ଥିର char_u *
set_context_in_unlet_cmd(expand_T *xp, char_u *arg)
{
    ଯେତେବେଳେ ((xp->xp_pattern = vim_strchr(arg, ' ')) != ଖାଲି)
	arg = xp->xp_pattern + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_pattern = arg;

    ଯଦି (*xp->xp_pattern == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_pattern;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    ସ୍ଥିର char_u *
set_context_in_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    ଯଦି (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_pattern = arg;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_pattern = skipwhite(p);
	}
	ଅନ୍ୟଥା
	    xp->xp_context = EXPAND_NOTHING;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

ସ୍ଥିର ଗଣନା
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
ସ୍ଥିର ଗଣନା
{
    EXP_BREAKPT_ADD,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    ସ୍ଥିର char_u *
set_context_in_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_start;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_pattern = arg;

    ଯଦି (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_ADD;
    ଅନ୍ୟଥା ଯଦି (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    ଅନ୍ୟଥା
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ ଖାଲି;
    subcmd_start = p;

    ଯଦି (STRNCMP("file ", p, 5) == 0 || STRNCMP("func ", p, 5) == 0)
    {
	// :breakadd file [lnum] <filename>
	// :breakadd func [lnum] <funcname>
	p += 4;
	p = skipwhite(p);

	// skip line number (if specified)
	ଯଦି (VIM_ISDIGIT(*p))
	{
	    p = skipdigits(p);
	    ଯଦି (*p != ' ')
	    {
		xp->xp_context = EXPAND_NOTHING;
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    p = skipwhite(p);
	}
	ଯଦି (STRNCMP("file", subcmd_start, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	ଅନ୍ୟଥା
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = p;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_EXPRESSION;
	xp->xp_pattern = skipwhite(p + 5);
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର char_u *
set_context_in_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_pattern = ଖାଲି;

    p = skipwhite(arg);
    ଯଦି (VIM_ISDIGIT(*p))
	ଫେରନ୍ତୁ ଖାଲି;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_pattern = p;

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    ସ୍ଥିର char_u *
set_context_in_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_pattern = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (;;)
    {
	ଯଦି (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    ଜାରି;
	}
	ଯଦି (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    ଜାରି;
	}
	ଭାଙ୍ଗ;
    }

    ଯଦି ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    ଅନ୍ୟଥା ଯଦି ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    ଅନ୍ୟଥା ଯଦି ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_pattern = p;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_context_with_pattern(expand_T *xp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_pattern_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    ଯଦି (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	ଫେରନ୍ତୁ;
#endif

    xp->xp_pattern = ccline->cmdbuff + skiplen;
    xp->xp_pattern_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_PATTERN_IN_BUF;
    xp->xp_ସନ୍ଧାନ_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    ସ୍ଥିର char_u *
set_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	long		argt,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compl,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit)
{
    char_u  *nextcmd;

    ବାଛ (cmdidx)
    {
	ସ୍ଥିତି CMD_find:
	ସ୍ଥିତି CMD_sfind:
	ସ୍ଥିତି CMD_tabfind:
	    ଯଦି (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_IN_PATH;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_cd:
	ସ୍ଥିତି CMD_chdir:
	ସ୍ଥିତି CMD_tcd:
	ସ୍ଥିତି CMD_tchdir:
	ସ୍ଥିତି CMD_lcd:
	ସ୍ଥିତି CMD_lchdir:
	    ଯଦି (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_IN_CDPATH;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;

	// Command modifiers: return the argument.
	// Also for commands with an argument that is a command.
	ସ୍ଥିତି CMD_aboveleft:
	ସ୍ଥିତି CMD_argdo:
	ସ୍ଥିତି CMD_belowright:
	ସ୍ଥିତି CMD_botright:
	ସ୍ଥିତି CMD_browse:
	ସ୍ଥିତି CMD_bufdo:
	ସ୍ଥିତି CMD_cdo:
	ସ୍ଥିତି CMD_cfdo:
	ସ୍ଥିତି CMD_confirm:
	ସ୍ଥିତି CMD_debug:
	ସ୍ଥିତି CMD_folddoclosed:
	ସ୍ଥିତି CMD_folddoopen:
	ସ୍ଥିତି CMD_hide:
	ସ୍ଥିତି CMD_horizontal:
	ସ୍ଥିତି CMD_keepalt:
	ସ୍ଥିତି CMD_keepjumps:
	ସ୍ଥିତି CMD_keepmarks:
	ସ୍ଥିତି CMD_keeppatterns:
	ସ୍ଥିତି CMD_ldo:
	ସ୍ଥିତି CMD_leftabove:
	ସ୍ଥିତି CMD_lfdo:
	ସ୍ଥିତି CMD_lockmarks:
	ସ୍ଥିତି CMD_noautocmd:
	ସ୍ଥିତି CMD_noswapfile:
	ସ୍ଥିତି CMD_rightbelow:
	ସ୍ଥିତି CMD_sandbox:
	ସ୍ଥିତି CMD_silent:
	ସ୍ଥିତି CMD_tab:
	ସ୍ଥିତି CMD_tabdo:
	ସ୍ଥିତି CMD_topleft:
	ସ୍ଥିତି CMD_unsilent:
	ସ୍ଥିତି CMD_verbose:
	ସ୍ଥିତି CMD_vertical:
	ସ୍ଥିତି CMD_windo:
	ସ୍ଥିତି CMD_vim9cmd:
	ସ୍ଥିତି CMD_legacy:
	    ଫେରନ୍ତୁ arg;

	ସ୍ଥିତି CMD_filter:
	    ଫେରନ୍ତୁ set_context_in_filter_cmd(xp, arg);

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	ସ୍ଥିତି CMD_match:
	    ଫେରନ୍ତୁ set_context_in_match_cmd(xp, arg);
#endif

	// All completion for the +cmdline_compl feature goes here.

	ସ୍ଥିତି CMD_command:
	    ଫେରନ୍ତୁ set_context_in_user_cmd(xp, arg);

	ସ୍ଥିତି CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_global:
	ସ୍ଥିତି CMD_vglobal:
	    nextcmd = find_cmd_after_global_cmd(arg);
	    ଯଦି (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    ଫେରନ୍ତୁ nextcmd;

	ସ୍ଥିତି CMD_and:
	ସ୍ଥିତି CMD_substitute:
	    nextcmd = find_cmd_after_substitute_cmd(arg);
	    ଯଦି (!nextcmd && may_expand_pattern)
		set_context_with_pattern(xp);
	    ଫେରନ୍ତୁ nextcmd;

	ସ୍ଥିତି CMD_isearch:
	ସ୍ଥିତି CMD_dsearch:
	ସ୍ଥିତି CMD_ilist:
	ସ୍ଥିତି CMD_dlist:
	ସ୍ଥିତି CMD_ijump:
	ସ୍ଥିତି CMD_psearch:
	ସ୍ଥିତି CMD_djump:
	ସ୍ଥିତି CMD_isplit:
	ସ୍ଥିତି CMD_dsplit:
	    ଫେରନ୍ତୁ find_cmd_after_isearch_cmd(xp, arg);
	ସ୍ଥିତି CMD_autocmd:
	    ଫେରନ୍ତୁ set_context_in_autocmd(xp, arg, FALSE);
	ସ୍ଥିତି CMD_doautocmd:
	ସ୍ଥିତି CMD_doautoall:
	    ଫେରନ୍ତୁ set_context_in_autocmd(xp, arg, TRUE);
	ସ୍ଥିତି CMD_set:
	    set_context_in_set_cmd(xp, arg, 0);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_setglobal:
	    set_context_in_set_cmd(xp, arg, OPT_GLOBAL);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_setlocal:
	    set_context_in_set_cmd(xp, arg, OPT_LOCAL);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_tag:
	ସ୍ଥିତି CMD_stag:
	ସ୍ଥିତି CMD_ptag:
	ସ୍ଥିତି CMD_ltag:
	ସ୍ଥିତି CMD_tselect:
	ସ୍ଥିତି CMD_stselect:
	ସ୍ଥିତି CMD_ptselect:
	ସ୍ଥିତି CMD_tjump:
	ସ୍ଥିତି CMD_stjump:
	ସ୍ଥିତି CMD_ptjump:
	    ଯଦି (vim_strchr(p_wop, WOP_TAGFILE) != ଖାଲି)
		xp->xp_context = EXPAND_TAGS_LISTFILES;
	    ଅନ୍ୟଥା
		xp->xp_context = EXPAND_TAGS;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;
#ifdef FEAT_SYN_HL
	ସ୍ଥିତି CMD_syntax:
	    set_context_in_syntax_cmd(xp, arg);
	    ଭାଙ୍ଗ;
#endif
#ifdef FEAT_EVAL
	ସ୍ଥିତି CMD_final:
	ସ୍ଥିତି CMD_const:
	ସ୍ଥିତି CMD_let:
	ସ୍ଥିତି CMD_var:
	ସ୍ଥିତି CMD_if:
	ସ୍ଥିତି CMD_elseif:
	ସ୍ଥିତି CMD_while:
	ସ୍ଥିତି CMD_for:
	ସ୍ଥିତି CMD_echo:
	ସ୍ଥିତି CMD_echon:
	ସ୍ଥିତି CMD_execute:
	ସ୍ଥିତି CMD_echomsg:
	ସ୍ଥିତି CMD_echoerr:
	ସ୍ଥିତି CMD_echoconsole:
	ସ୍ଥିତି CMD_echowindow:
	ସ୍ଥିତି CMD_call:
	ସ୍ଥିତି CMD_return:
	ସ୍ଥିତି CMD_cexpr:
	ସ୍ଥିତି CMD_caddexpr:
	ସ୍ଥିତି CMD_cgetexpr:
	ସ୍ଥିତି CMD_lexpr:
	ସ୍ଥିତି CMD_laddexpr:
	ସ୍ଥିତି CMD_lgetexpr:
	    set_context_for_expression(xp, arg, cmdidx);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_unlet:
	    ଫେରନ୍ତୁ set_context_in_unlet_cmd(xp, arg);
	ସ୍ଥିତି CMD_function:
	ସ୍ଥିତି CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_disassemble:
	    set_context_in_disassemble_cmd(xp, arg);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_echohl:
	    set_context_in_echohl_cmd(xp, arg);
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି CMD_highlight:
	    set_context_in_highlight_cmd(xp, arg);
	    ଭାଙ୍ଗ;
#ifdef FEAT_CSCOPE
	ସ୍ଥିତି CMD_cscope:
	ସ୍ଥିତି CMD_lcscope:
	ସ୍ଥିତି CMD_scscope:
	    set_context_in_cscope_cmd(xp, arg, cmdidx);
	    ଭାଙ୍ଗ;
#endif
#ifdef FEAT_SIGNS
	ସ୍ଥିତି CMD_sign:
	    set_context_in_sign_cmd(xp, arg);
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି CMD_bdelete:
	ସ୍ଥିତି CMD_bwipeout:
	ସ୍ଥିତି CMD_bunload:
	    ଯେତେବେଳେ ((xp->xp_pattern = vim_strchr(arg, ' ')) != ଖାଲି)
		arg = xp->xp_pattern + 1;
	    // FALLTHROUGH
	ସ୍ଥିତି CMD_buffer:
	ସ୍ଥିତି CMD_sbuffer:
	ସ୍ଥିତି CMD_pbuffer:
	ସ୍ଥିତି CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;
#ifdef FEAT_DIFF
	ସ୍ଥିତି CMD_diffget:
	ସ୍ଥିତି CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି CMD_USER:
	ସ୍ଥିତି CMD_USER_BUF:
	    ଫେରନ୍ତୁ set_context_in_user_cmdarg(cmd, arg, argt, compl, xp,
								forceit);

	ସ୍ଥିତି CMD_map:	    ସ୍ଥିତି CMD_noremap:
	ସ୍ଥିତି CMD_nmap:	    ସ୍ଥିତି CMD_nnoremap:
	ସ୍ଥିତି CMD_vmap:	    ସ୍ଥିତି CMD_vnoremap:
	ସ୍ଥିତି CMD_omap:	    ସ୍ଥିତି CMD_onoremap:
	ସ୍ଥିତି CMD_imap:	    ସ୍ଥିତି CMD_inoremap:
	ସ୍ଥିତି CMD_cmap:	    ସ୍ଥିତି CMD_cnoremap:
	ସ୍ଥିତି CMD_lmap:	    ସ୍ଥିତି CMD_lnoremap:
	ସ୍ଥିତି CMD_smap:	    ସ୍ଥିତି CMD_snoremap:
	ସ୍ଥିତି CMD_tmap:	    ସ୍ଥିତି CMD_tnoremap:
	ସ୍ଥିତି CMD_xmap:	    ସ୍ଥିତି CMD_xnoremap:
	    ଫେରନ୍ତୁ set_context_in_map_cmd(xp, cmd, arg, forceit,
						     FALSE, FALSE, cmdidx);
	ସ୍ଥିତି CMD_unmap:
	ସ୍ଥିତି CMD_nunmap:
	ସ୍ଥିତି CMD_vunmap:
	ସ୍ଥିତି CMD_ounmap:
	ସ୍ଥିତି CMD_iunmap:
	ସ୍ଥିତି CMD_cunmap:
	ସ୍ଥିତି CMD_lunmap:
	ସ୍ଥିତି CMD_sunmap:
	ସ୍ଥିତି CMD_tunmap:
	ସ୍ଥିତି CMD_xunmap:
	    ଫେରନ୍ତୁ set_context_in_map_cmd(xp, cmd, arg, forceit,
						      FALSE, TRUE, cmdidx);
	ସ୍ଥିତି CMD_mapclear:
	ସ୍ଥିତି CMD_nmapclear:
	ସ୍ଥିତି CMD_vmapclear:
	ସ୍ଥିତି CMD_omapclear:
	ସ୍ଥିତି CMD_imapclear:
	ସ୍ଥିତି CMD_cmapclear:
	ସ୍ଥିତି CMD_lmapclear:
	ସ୍ଥିତି CMD_smapclear:
	ସ୍ଥିତି CMD_tmapclear:
	ସ୍ଥିତି CMD_xmapclear:
	    xp->xp_context = EXPAND_MAPCLEAR;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_abbreviate:	ସ୍ଥିତି CMD_noreabbrev:
	ସ୍ଥିତି CMD_cabbrev:	ସ୍ଥିତି CMD_cnoreabbrev:
	ସ୍ଥିତି CMD_iabbrev:	ସ୍ଥିତି CMD_inoreabbrev:
	    ଫେରନ୍ତୁ set_context_in_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	ସ୍ଥିତି CMD_unabbreviate:
	ସ୍ଥିତି CMD_cunabbrev:
	ସ୍ଥିତି CMD_iunabbrev:
	    ଫେରନ୍ତୁ set_context_in_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#ifdef FEAT_MENU
	ସ୍ଥିତି CMD_menu:	    ସ୍ଥିତି CMD_noremenu:	    ସ୍ଥିତି CMD_unmenu:
	ସ୍ଥିତି CMD_amenu:	    ସ୍ଥିତି CMD_anoremenu:	    ସ୍ଥିତି CMD_aunmenu:
	ସ୍ଥିତି CMD_nmenu:	    ସ୍ଥିତି CMD_nnoremenu:	    ସ୍ଥିତି CMD_nunmenu:
	ସ୍ଥିତି CMD_vmenu:	    ସ୍ଥିତି CMD_vnoremenu:	    ସ୍ଥିତି CMD_vunmenu:
	ସ୍ଥିତି CMD_omenu:	    ସ୍ଥିତି CMD_onoremenu:	    ସ୍ଥିତି CMD_ounmenu:
	ସ୍ଥିତି CMD_imenu:	    ସ୍ଥିତି CMD_inoremenu:	    ସ୍ଥିତି CMD_iunmenu:
	ସ୍ଥିତି CMD_cmenu:	    ସ୍ଥିତି CMD_cnoremenu:	    ସ୍ଥିତି CMD_cunmenu:
	ସ୍ଥିତି CMD_tlmenu:    ସ୍ଥିତି CMD_tlnoremenu:    ସ୍ଥିତି CMD_tlunmenu:
	ସ୍ଥିତି CMD_tmenu:				    ସ୍ଥିତି CMD_tunmenu:
	ସ୍ଥିତି CMD_popup:	    ସ୍ଥିତି CMD_tearoff:	    ସ୍ଥିତି CMD_emenu:
	    ଫେରନ୍ତୁ set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif

	ସ୍ଥିତି CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_runtime:
	    set_context_in_runtime_cmd(xp, arg);
	    ଭାଙ୍ଗ;

#ଯଦି defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	ସ୍ଥିତି CMD_language:
	    ଫେରନ୍ତୁ set_context_in_lang_cmd(xp, arg);
#endif
#ଯଦି defined(FEAT_PROFILE)
	ସ୍ଥିତି CMD_profile:
	    set_context_in_profile_cmd(xp, arg);
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;
#ଯଦି defined(FEAT_PROFILE)
	ସ୍ଥିତି CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;
#endif

	ସ୍ଥିତି CMD_argdelete:
	    ଯେତେବେଳେ ((xp->xp_pattern = vim_strchr(arg, ' ')) != ଖାଲି)
		arg = xp->xp_pattern + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_pattern = arg;
	    ଭାଙ୍ଗ;

#ifdef FEAT_EVAL
	ସ୍ଥିତି CMD_breakadd:
	ସ୍ଥିତି CMD_profdel:
	ସ୍ଥିତି CMD_breakdel:
	    ଫେରନ୍ତୁ set_context_in_breakadd_cmd(xp, arg, cmdidx);

	ସ୍ଥିତି CMD_scriptnames:
	    ଫେରନ୍ତୁ set_context_in_scriptnames_cmd(xp, arg);
#endif
	ସ୍ଥିତି CMD_filetype:
	    ଫେରନ୍ତୁ set_context_in_filetype_cmd(xp, arg);

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    ସ୍ଥିର char_u *
set_one_cmd_context(
    expand_T	*xp,
    char_u	*buff)	    // buffer for command string
{
    char_u		*p;
    char_u		*cmd, *arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			len = 0;
    exarg_T		ea;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			compl = EXPAND_NOTHING;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			forceit = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			usefilter = FALSE;  // filter instead of file name

    ExpandInit(xp);
    xp->xp_pattern = buff;
    xp->xp_ଧାଡ଼ି = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    ପାଇଁ (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != ଖାଲି; cmd++)
	;
    xp->xp_pattern = cmd;

    ଯଦି (*cmd == NUL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_pattern = cmd;
    ଯଦି (*cmd == NUL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (*cmd == '|' || *cmd == '\n')
	ଫେରନ୍ତୁ cmd + 1;			// There's another command

    // Get the command index.
    p = set_cmd_index(cmd, &ea, xp, &compl);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    xp->xp_context = EXPAND_NOTHING; // Default now that we're past command

    ଯଦି (*p == '!')		    // forced commands
    {
	forceit = TRUE;
	++p;
    }

    // 6. parse arguments
    ଯଦି (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = excmd_get_argt(ea.cmdidx);

    arg = skipwhite(p);

    // Does command allow "++argopt" argument?
    ଯଦି ((ea.argt & EX_ARGOPT) || ea.cmdidx == CMD_terminal)
    {
	ଯେତେବେଳେ (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    ଯେତେବେଳେ (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    ଯଦି (*p == NUL)
	    {
		ଯଦି (ea.argt & EX_ARGOPT)
		    ଫେରନ୍ତୁ set_context_in_argopt(xp, arg + 2);
#ifdef FEAT_TERMINAL
		ଯଦି (ea.cmdidx == CMD_terminal)
		    ଫେରନ୍ତୁ set_context_in_terminalopt(xp, arg + 2);
#endif
	    }

	    arg = skipwhite(p);
	}
    }

    ଯଦି (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	ଯଦି (*arg == '>')			// append
	{
	    ଯଦି (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	ଅନ୍ୟଥା ଯଦି (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    ଯଦି (ea.cmdidx == CMD_read)
    {
	usefilter = forceit;			// :r! filter if forced
	ଯଦି (*arg == '!')			// :r !filter
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    ଯଦି (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ଯେତେବେଳେ (*arg == *cmd)	    // allow any number of '>' or '<'
	    ++arg;
	arg = skipwhite(arg);
    }

    // Does command allow "+command"?
    ଯଦି ((ea.argt & EX_CMDARG) && !usefilter && *arg == '+')
    {
	// Check if we're in the +command
	p = arg + 1;
	arg = skip_cmd_arg(arg, FALSE);

	// Still touching the command after '+'?
	ଯଦି (*arg == NUL)
	    ଫେରନ୍ତୁ p;

	// Skip space(s) after +command to get to the real argument
	arg = skipwhite(arg);
    }


    // Check for '|' to separate commands and '"' to start comments.
    // Don't do this for ":read !cmd" and ":write !cmd".
    ଯଦି ((ea.argt & EX_TRLBAR) && !usefilter)
    {
	p = arg;
	// ":redir @" is not the start of a comment
	ଯଦି (ea.cmdidx == CMD_redir && p[0] == '@' && p[1] == '"')
	    p += 2;
	ଯେତେବେଳେ (*p)
	{
	    ଯଦି (*p == Ctrl_V)
	    {
		ଯଦି (p[1] != NUL)
		    ++p;
	    }
	    ଅନ୍ୟଥା ଯଦି ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		ଯଦି (*(p - 1) != '\\')
		{
		    ଯଦି (*p == '|' || *p == '\n')
			ଫେରନ୍ତୁ p + 1;
		    ଫେରନ୍ତୁ ଖାଲି;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    ଯଦି (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == ଖାଲି)
	// no arguments allowed but there is something
	ଫେରନ୍ତୁ ଖାଲି;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_pattern = p;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(buff);
    ଯେତେବେଳେ (*p && p < buff + len)
    {
	ଯଦି (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_pattern = ++p;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    ଯଦି (ea.argt & EX_XFILE)
	set_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    ଫେରନ୍ତୁ set_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    ଶୂନ୍ୟ
set_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,	    // length of command line (excl. NUL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,	    // position of cursor
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_ccline UNUSED) // use ccline for info
{
#ifdef FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			context;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    ଯଦି (col < len)
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
    ଯଦି (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	set_context_for_expression(xp, str, CMD_SIZE);
    }
    ଅନ୍ୟଥା ଯଦି (use_ccline && ccline->ନିବେଶ_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_pattern = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	ଯଦି (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    set_context_for_wildcard_arg(ଖାଲି, xp->xp_pattern, FALSE, xp,
								     &context);
	}
    }
    ଅନ୍ୟଥା
#endif
	ଯେତେବେଳେ (nextcomm != ଖାଲି)
	    nextcomm = set_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_ଧାଡ଼ି = str;
    xp->xp_col = col;

    ଯଦି (col < len)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_cmdline(
    expand_T	*xp,
    char_u	*str,		// start of command line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,		// position of cursor
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options = WILD_ADD_SLASH|WILD_SILENT;

    ଯଦି (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	ଫେରନ୍ତୁ EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    ଯଦି (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	ଫେରନ୍ତୁ EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_pattern_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(str + col - xp->xp_pattern);
    {
	char_u	*pat_src = xp->xp_pattern;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	pat_len = xp->xp_pattern_len;
#ଯଦି defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	ଯଦି (unesc != ଖାଲି)
	    pat_src = unesc;
#endif

	ଯଦି (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	ଅନ୍ୟଥା
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#ଯଦି defined(FEAT_EVAL)
	vim_free(unesc);
#endif
    }
    ଯଦି (file_str == ଖାଲି)
	ଫେରନ୍ତୁ EXPAND_UNSUCCESSFUL;

    ଯଦି (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    ଯଦି (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = ଖାଲି;
    }
    vim_free(file_str);

    ଫେରନ୍ତୁ EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		free_pat = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped
    // space
    ଯଦି (xp->xp_backslash != XP_BS_NONE)
    {
	size_t	pat_len;
	char_u	*pat_end;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	ଯଦି (pat == ଖାଲି)
	    ଫେରନ୍ତୁ ret;

	pat_end = pat + pat_len;
	ପାଇଁ (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    ଯଦି (*p != '\\')
		ଜାରି;

	    ଯଦି (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		pat_end -= 3;
	    }
	    ଅନ୍ୟଥା ଯଦି (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_end - from) + 1);   // +1 for NUL
		--pat_end;
	    }
	    ଅନ୍ୟଥା ଯଦି (xp->xp_backslash & XP_BS_COMMA)
	    {
		ଯଦି (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_end - from) + 1);   // +1 for NUL
		    pat_end -= 2;
		}
#ifdef BACKSLASH_IN_FILENAME
		ଅନ୍ୟଥା ଯଦି (*(p + 1) == ',')
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

    ଯଦି (xp->xp_context == EXPAND_FINDFUNC)
    {
#ifdef FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, ଅଙ୍କ_Matches);
#endif
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	ଅନ୍ୟଥା ଯଦି (xp->xp_context == EXPAND_FILES_IN_PATH)
	    flags |= (EW_FILE | EW_PATH);
	ଅନ୍ୟଥା ଯଦି (xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	ଅନ୍ୟଥା
	    flags = (flags | EW_DIR) & ~EW_FILE;
	ଯଦି (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, ଅଙ୍କ_Matches, matches, flags);
    }
    ଯଦି (free_pat)
	vim_free(pat);
#ifdef BACKSLASH_IN_FILENAME
    ଯଦି (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;

	ପାଇଁ (j = 0; j < *ଅଙ୍କ_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    ଯେତେବେଳେ (*ptr != NUL)
	    {
		ଯଦି (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		ଅନ୍ୟଥା ଯଦି (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    ଫେରନ୍ତୁ ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    ସ୍ଥିର char_u *
get_behave_arg(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx == 0)
	ଫେରନ୍ତୁ (char_u *)"mswin";
    ଯଦି (idx == 1)
	ଫେରନ୍ତୁ (char_u *)"xterm";
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    ସ୍ଥିର char_u *
get_filetypecmd_arg(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx < 0)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	ଅକ୍ଷର	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	ଫେରନ୍ତୁ (char_u *)opts_all[idx];
    }
    ଯଦି (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	ଅକ୍ଷର	*opts_plugin[] = {"plugin", "on", "off"};
	ଫେରନ୍ତୁ (char_u *)opts_plugin[idx];
    }
    ଯଦି (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	ଅକ୍ଷର	*opts_indent[] = {"indent", "on", "off"};
	ଫେରନ୍ତୁ (char_u *)opts_indent[idx];
    }
    ଯଦି (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	ଅକ୍ଷର	*opts_onoff[] = {"on", "off"};
	ଫେରନ୍ତୁ (char_u *)opts_onoff[idx];
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

#ifdef FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    ସ୍ଥିର char_u *
get_breakadd_arg(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx >= 0 && idx <= 3)
    {
	ଅକ୍ଷର	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	ଯଦି (breakpt_expand_what == EXP_BREAKPT_ADD)
	    ଫେରନ୍ତୁ (char_u *)opts[idx];
	ଅନ୍ୟଥା ଯଦି (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    ଯଦି (idx <= 2)
		ଫେରନ୍ତୁ (char_u *)opts[idx + 1];
	}
	ଅନ୍ୟଥା
	{
	    // profdel {func, file}
	    ଯଦି (idx <= 1)
		ଫେରନ୍ତୁ (char_u *)opts[idx + 1];
	}
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    ସ୍ଥିର char_u *
get_scriptnames_arg(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    scriptitem_T *si;

    ଯଦି (!SCRIPT_ID_VALID(idx + 1))
	ଫେରନ୍ତୁ ଖାଲି;

    si = SCRIPT_ITEM(idx + 1);
    home_replace(ଖାଲି, si->sn_name, NameBuff, MAXPATHL, TRUE);
    ଫେରନ୍ତୁ NameBuff;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    ସ୍ଥିର char_u *
get_retab_arg(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx == 0)
	ଫେରନ୍ତୁ (char_u *)"-indentonly";
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    ସ୍ଥିର char_u *
get_messages_arg(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx == 0)
	ଫେରନ୍ତୁ (char_u *)"clear";
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର char_u *
get_mapclear_arg(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx == 0)
	ଫେରନ୍ତୁ (char_u *)"<buffer>";
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches)
{
    ସ୍ଥିର ସଂରଚନା expgen
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		context;
	char_u	*((*func)(expand_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		escaped;
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
	{EXPAND_EXPRESSION, get_ପଦସମଷ୍ଟି_name, FALSE, TRUE},
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
#ଯଦି defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(tab); ++i)
    {
	ଯଦି (xp->xp_context == tab[i].context)
	{
	    ଯଦି (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, ଅଙ୍କ_Matches,
						tab[i].func, tab[i].escaped);
	    ଭାଙ୍ଗ;
	}
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
map_wildopts_to_ewflags(ପୂର୍ଣ୍ଣସଂଖ୍ୟା options)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;

    flags = EW_DIR;	// include directories
    ଯଦି (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    ଯଦି (options & WILD_ADD_SLASH)
	flags |= EW_ADDSLASH;
    ଯଦି (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    ଯଦି (options & WILD_SILENT)
	flags |= EW_SILENT;
    ଯଦି (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    ଯଦି (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    ଫେରନ୍ତୁ flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;
    char_u	*tofree = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_to_ewflags(options);

    ଯଦି (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_IN_PATH
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_IN_CDPATH)
	ଫେରନ୍ତୁ expand_files_and_dirs(xp, pat, matches, ଅଙ୍କ_Matches, flags,
								options);

    *matches = (char_u **)"";
    *ଅଙ୍କ_Matches = 0;
    ଯଦି (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	ଯଦି (find_help_tags(*pat == NUL ? (char_u *)"help" : pat,
					ଅଙ୍କ_Matches, matches, FALSE) == OK)
	{
#ifdef FEAT_MULTI_LANG
	    cleanup_help_tags(*ଅଙ୍କ_Matches, *matches);
#endif
	    ଫେରନ୍ତୁ OK;
	}
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (xp->xp_context == EXPAND_SHELLCMD)
	ଫେରନ୍ତୁ expand_shellcmd(pat, matches, ଅଙ୍କ_Matches, flags);
    ଯଦି (xp->xp_context == EXPAND_OLD_SETTING)
	ଫେରନ୍ତୁ ExpandOldSetting(ଅଙ୍କ_Matches, matches);
    ଯଦି (xp->xp_context == EXPAND_BUFFERS)
	ଫେରନ୍ତୁ ExpandBufnames(pat, ଅଙ୍କ_Matches, matches, options);
#ifdef FEAT_DIFF
    ଯଦି (xp->xp_context == EXPAND_DIFF_BUFFERS)
	ଫେରନ୍ତୁ ExpandBufnames(pat, ଅଙ୍କ_Matches, matches,
						options | BUF_DIFF_FILTER);
#endif
    ଯଦି (xp->xp_context == EXPAND_TAGS
	    || xp->xp_context == EXPAND_TAGS_LISTFILES)
	ଫେରନ୍ତୁ expand_tags(xp->xp_context == EXPAND_TAGS, pat, ଅଙ୍କ_Matches,
								matches);
    ଯଦି (xp->xp_context == EXPAND_COLORS)
    {
	ଅକ୍ଷର *directories[] = {"colors", ଖାଲି};
	ଫେରନ୍ତୁ ExpandRTDir(pat, DIP_START + DIP_OPT, ଅଙ୍କ_Matches, matches,
								directories);
    }
    ଯଦି (xp->xp_context == EXPAND_COMPILER)
    {
	ଅକ୍ଷର *directories[] = {"compiler", ଖାଲି};
	ଫେରନ୍ତୁ ExpandRTDir(pat, 0, ଅଙ୍କ_Matches, matches, directories);
    }
    ଯଦି (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	ଅକ୍ଷର *directories[] = {"syntax", ଖାଲି};
	ଫେରନ୍ତୁ ExpandRTDir(pat, 0, ଅଙ୍କ_Matches, matches, directories);
    }
    ଯଦି (xp->xp_context == EXPAND_FILETYPE)
    {
	ଅକ୍ଷର *directories[] = {"syntax", "indent", "ftplugin", ଖାଲି};
	ଫେରନ୍ତୁ ExpandRTDir(pat, 0, ଅଙ୍କ_Matches, matches, directories);
    }
#ifdef FEAT_KEYMAP
    ଯଦି (xp->xp_context == EXPAND_KEYMAP)
    {
	ଅକ୍ଷର *directories[] = {"keymap", ଖାଲି};
	ଫେରନ୍ତୁ ExpandRTDir(pat, 0, ଅଙ୍କ_Matches, matches, directories);
    }
#endif
#ଯଦି defined(FEAT_EVAL)
    ଯଦି (xp->xp_context == EXPAND_USER_LIST)
	ଫେରନ୍ତୁ ExpandUserList(xp, matches, ଅଙ୍କ_Matches);
#endif
    ଯଦି (xp->xp_context == EXPAND_PACKADD)
	ଫେରନ୍ତୁ ExpandPackAddDir(pat, ଅଙ୍କ_Matches, matches);
    ଯଦି (xp->xp_context == EXPAND_RUNTIME)
	ଫେରନ୍ତୁ expand_runtime_cmd(pat, ଅଙ୍କ_Matches, matches);
    ଯଦି (xp->xp_context == EXPAND_PATTERN_IN_BUF)
	ଫେରନ୍ତୁ expand_pattern_in_buf(pat, xp->xp_ସନ୍ଧାନ_dir,
		matches, ଅଙ୍କ_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    ଯଦି ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(pat) + 20;

	tofree = alloc(len);
	ଯଦି (tofree == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	vim_snprintf((ଅକ୍ଷର *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    ଯଦି (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	ଯଦି (regmatch.regprog == ଖାଲି)
	{
	    vim_free(tofree);
	    ଫେରନ୍ତୁ FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    ଯଦି (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, ଅଙ୍କ_Matches, matches, fuzzy);
    ଅନ୍ୟଥା ଯଦି (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, ଅଙ୍କ_Matches, matches);
    ଅନ୍ୟଥା ଯଦି (xp->xp_context == EXPAND_SETTING_SUBTRACT)
	ret = ExpandSettingSubtract(xp, &regmatch, ଅଙ୍କ_Matches, matches);
    ଅନ୍ୟଥା ଯଦି (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, ଅଙ୍କ_Matches, matches);
    ଅନ୍ୟଥା ଯଦି (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, ଅଙ୍କ_Matches);
    ଅନ୍ୟଥା ଯଦି (xp->xp_context == EXPAND_HIGHLIGHT_GROUP)
	ret = expand_highlight_group(pat, xp, &regmatch, matches, ଅଙ୍କ_Matches);
#ଯଦି defined(FEAT_TERMINAL)
    ଅନ୍ୟଥା ଯଦି (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, ଅଙ୍କ_Matches);
#endif
#ଯଦି defined(FEAT_EVAL)
    ଅନ୍ୟଥା ଯଦି (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, ଅଙ୍କ_Matches);
#endif
    ଅନ୍ୟଥା
	ret = ExpandOther(pat, xp, &regmatch, matches, ଅଙ୍କ_Matches);

    ଯଦି (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    ଫେରନ୍ତୁ ret;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches,
    char_u	*(*func)(expand_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା),
					  // returns a string from the list
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		escaped)
{
    ଫେରନ୍ତୁ ExpandGenericExt(
	pat, xp, regmatch, matches, ଅଙ୍କ_Matches, func, escaped, 0);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandGenericExt(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches,
    char_u	*(*func)(expand_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା),
					  // returns a string from the list
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		escaped,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sortStartIdx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fuzzy;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sort_matches = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		funcsort = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sortStartMatchIdx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ଖାଲି;
    *ଅଙ୍କ_Matches = 0;

    ଯଦି (!fuzzy)
	ga_init2(&ga, ଆକାର(ଅକ୍ଷର *), 30);
    ଅନ୍ୟଥା
	ga_init2(&ga, ଆକାର(fuzmatch_str_T), 30);

    ପାଇଁ (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	ଯଦି (str == ଖାଲି)	    // end of list
	    ଭାଙ୍ଗ;
	ଯଦି (*str == NUL)	    // skip empty strings
	    ଜାରି;

	ଯଦି (xp->xp_pattern[0] != NUL)
	{
	    ଯଦି (!fuzzy)
		match = vim_regexec(regmatch, str, (colnr_T)0);
	    ଅନ୍ୟଥା
	    {
		score = fuzzy_match_str(str, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	ଅନ୍ୟଥା
	    match = TRUE;

	ଯଦି (!match)
	    ଜାରି;

	ଯଦି (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	ଅନ୍ୟଥା
	    str = vim_strsave(str);
	ଯଦି (str == ଖାଲି)
	{
	    ଯଦି (!fuzzy)
	    {
		ga_clear_strings(&ga);
		ଫେରନ୍ତୁ FAIL;
	    }
	    fuzmatch_str_free(ga.ga_data, ga.ga_len);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    ଭାଙ୍ଗ;
	}

	ଯଦି (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	ଅନ୍ୟଥା
	    ((char_u **)ga.ga_data)[ga.ga_len] = str;

#ifdef FEAT_MENU
	ଯଦି (func == get_menu_names)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    ଯଦି (*str == '\001')
		*str = '.';
	}
#endif

	ଯଦି (sortStartIdx >= 0 && i >= sortStartIdx && sortStartMatchIdx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    sortStartMatchIdx = ga.ga_len;
	}

	++ga.ga_len;
    }

    ଯଦି (ga.ga_len == 0)
	ଫେରନ୍ତୁ OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    ଯଦି (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	sort_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    ଯଦି (xp->xp_context == EXPAND_EXPRESSION
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    ଯଦି (sort_matches && sortStartMatchIdx != -1)
    {
	ଯଦି (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((ଶୂନ୍ୟ *)ga.ga_data, (size_t)ga.ga_len, ଆକାର(char_u *),
							   sort_func_compare);
	ଅନ୍ୟଥା
	    sort_strings((char_u **)ga.ga_data + sortStartMatchIdx, ga.ga_len - sortStartMatchIdx);
    }

    ଯଦି (!fuzzy)
    {
	*matches = ga.ga_data;
	*ଅଙ୍କ_Matches = ga.ga_len;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							funcsort) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	*ଅଙ୍କ_Matches = ga.ga_len;
    }

#ଯଦି defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#endif

    ଫେରନ୍ତୁ OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
expand_shellcmd_onedir(
	char_u		*pathed_pattern,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    ଯଦି (expand_wildcards(1, &pathed_pattern, ଅଙ୍କ_Matches, matches, flags) != OK)
	ଫେରନ୍ତୁ;

    ଯଦି (ga_grow(gap, *ଅଙ୍କ_Matches) == FAIL)
    {
	FreeWild(*ଅଙ୍କ_Matches, *matches);
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < *ଅଙ୍କ_Matches; ++i)
    {
	char_u	*name = (*matches)[i];
	size_t	namelen = STRLEN(name);

	ଯଦି (namelen > pathlen)
	{
	    hash_T	hash;
	    hashitem_T	*hi;

	    // Check if this name was already found.
	    hash = hash_hash(name + pathlen);
	    hi = hash_lookup(ht, name + pathlen, hash);
	    ଯଦି (HASHITEM_EMPTY(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_data)[gap->ga_len++] = name;
		hash_add_item(ht, hi, name, hash);
		name = ଖାଲି;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches,	// return: number of matches
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*path = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = flagsarg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_curdir = FALSE;
    hashtab_T	found_ht;

    buf = alloc(MAXPATHL);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    ଯଦି (pat == ଖାଲି)
    {
	vim_free(buf);
	ଫେରନ୍ତୁ FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    ପାଇଁ (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	ଯଦି (*s != '\\')
	    ଜାରି;

	p = s + 1;
	ଯଦି (*p == ' ')
	{
	    mch_memmove(s, p, (size_t)(e - p) + 1);     // +1 for NUL
	    --e;
	}
    }
    patlen = (size_t)(e - pat);

    flags |= EW_FILE | EW_EXEC | EW_SHELLCMD;

    ଯଦି (pat[0] == '.' && (vim_ispathsep(pat[1])
			       || (pat[1] == '.' && vim_ispathsep(pat[2]))))
	path = (char_u *)".";
    ଅନ୍ୟଥା
    {
	// For an absolute name we don't use $PATH.
	ଯଦି (!mch_isFullName(pat))
	    path = vim_getenv((char_u *)"PATH", &mustfree);
	ଯଦି (path == ଖାଲି)
	    path = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, ଆକାର(ଅକ୍ଷର *), 10);
    hash_init(&found_ht);
    ପାଇଁ (s = path; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	ଯଦି (*s == NUL)
	{
	    ଯଦି (did_curdir)
		ଭାଙ୍ଗ;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	ଅନ୍ୟଥା
	{
#ଯଦି defined(MSWIN)
	    e = vim_strchr(s, ';');
#ଅନ୍ୟଥା
	    e = vim_strchr(s, ':');
#endif
	    ଯଦି (e == ଖାଲି)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    ଯଦି (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    ଅନ୍ୟଥା
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? ଆକାର(PATHSEP) : 0;
	}

	// Make sure that the pathed pattern (ie the path and pattern concatenated
	// together) will fit inside the buffer. If not skip it and move on to the
	// next path.
	ଯଦି (pathlen + seplen + patlen + 1 <= MAXPATHL)
	{
	    ଯଦି (pathlen > 0)
	    {
		vim_strncpy(buf, s, pathlen);
		ଯଦି (seplen > 0)
		{
		    STRCPY(buf + pathlen, PATHSEPSTR);
		    pathlen += seplen;
		}
	    }
	    STRCPY(buf + pathlen, pat);

	    expand_shellcmd_onedir(buf, pathlen, matches, ଅଙ୍କ_Matches, flags,
				    &found_ht, &ga);
	}

	ଯଦି (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_data;
    *ଅଙ୍କ_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    ଯଦି (mustfree)
	vim_free(path);
    hash_clear(&found_ht);
    ଫେରନ୍ତୁ OK;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    ସ୍ଥିର ଶୂନ୍ୟ *
call_user_expand_func(
    ଶୂନ୍ୟ	*(*user_expand_func)(char_u *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep = 0;
    typval_T	args[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = ଖାଲି;
    ଶୂନ୍ୟ	*ret;

    ଯଦି (xp->xp_arg == ଖାଲି || xp->xp_arg[0] == '\0' || xp->xp_ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (ccline->cmdbuff != ଖାଲି)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_pattern,
						  xp->xp_pattern_len, &unesc_len);
	ଯଦି (pat == ଖାଲି)
	    pat = vim_strnsave(xp->xp_pattern, xp->xp_pattern_len);
    }

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = xp->xp_ଧାଡ଼ି;
    args[2].v_type = VAR_ସଂଖ୍ୟା;
    args[2].vval.v_ସଂଖ୍ୟା = xp->xp_col;
    args[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, args);

    current_sctx = save_current_sctx;
    ଯଦି (ccline->cmdbuff != ଖାଲି)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    ଫେରନ୍ତୁ ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fuzzy;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = ଖାଲି;
    *ଅଙ୍କ_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    ଯଦି (retstr == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (!fuzzy)
	ga_init2(&ga, ଆକାର(ଅକ୍ଷର *), 3);
    ଅନ୍ୟଥା
	ga_init2(&ga, ଆକାର(fuzmatch_str_T), 3);

    ପାଇଁ (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	ଯଦି (e == ଖାଲି)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	ଯଦି (xp->xp_pattern[0] != NUL)
	{
	    ଯଦି (!fuzzy)
		match = vim_regexec(regmatch, s, (colnr_T)0);
	    ଅନ୍ୟଥା
	    {
		score = fuzzy_match_str(s, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }
	}
	ଅନ୍ୟଥା
	    match = TRUE;		// match everything

	*e = keep;

	ଯଦି (match)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    ଯଦି (p == ଖାଲି)
		ଭାଙ୍ଗ;

	    ଯଦି (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (!fuzzy)
		((char_u **)ga.ga_data)[ga.ga_len] = p;
	    ଅନ୍ୟଥା
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ++ga.ga_len;
	}

	ଯଦି (*e != NUL)
	    ++e;
    }
    vim_free(retstr);

    ଯଦି (ga.ga_len == 0)
	ଫେରନ୍ତୁ OK;

    ଯଦି (!fuzzy)
    {
	*matches = ga.ga_data;
	*ଅଙ୍କ_Matches = ga.ga_len;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
								FALSE) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	*ଅଙ୍କ_Matches = ga.ga_len;
    }
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_kind;
    garray_T	ga_menu;
    garray_T	ga_info;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		have_extra = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ga_init2(&ga, ଆକାର(ଅକ୍ଷର *), 3);
    ga_init2(&ga_abbr, ଆକାର(ଅକ୍ଷର *), 3);
    ga_init2(&ga_kind, ଆକାର(ଅକ୍ଷର *), 3);
    ga_init2(&ga_menu, ଆକାର(ଅକ୍ଷର *), 3);
    ga_init2(&ga_info, ଆକାର(ଅକ୍ଷର *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = ଖାଲି;
	char_u	*abbr = ଖାଲି;
	char_u	*kind = ଖାଲି;
	char_u	*menu = ଖାଲି;
	char_u	*info = ଖାଲି;

	ଯଦି (tv->v_type == VAR_STRING)
	{
	    ଯଦି (tv->vval.v_string == ଖାଲି)
		ଜାରି;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_DICT && tv->vval.v_dict != ଖାଲି)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*word = dict_get_string(d, "word", FALSE);

	    ଯଦି (word == ଖାଲି)
		ଜାରି;  // "word" is required
	    p = vim_strsave(word);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    kind = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    ଯଦି (abbr != ଖାଲି || kind != ଖାଲି || menu != ଖାଲି || info != ଖାଲି)
		have_extra = TRUE;
	}
	ଅନ୍ୟଥା
	    ଜାରି;  // Skip other types

	ଯଦି (p == ଖାଲି
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
	    ଭାଙ୍ଗ;
	}

	((char_u **)ga.ga_data)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_data)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_kind.ga_data)[ga_kind.ga_len++] = kind;
	((char_u **)ga_menu.ga_data)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_data)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_data;
    *ଅଙ୍କ_Matches = ga.ga_len;
    ଯଦି (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_data;
	xp->xp_files_kind = (char_u **)ga_kind.ga_data;
	xp->xp_files_menu = (char_u **)ga_menu.ga_data;
	xp->xp_files_info = (char_u **)ga_info.ga_data;
    }
    ଅନ୍ୟଥା
    {
	// No extra info collected; free the placeholder NULL entries.
	ପାଇଁ (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_data)[i]);
	vim_free(ga_abbr.ga_data);
	ପାଇଁ (i = 0; i < ga_kind.ga_len; i++)
	    vim_free(((char_u **)ga_kind.ga_data)[i]);
	vim_free(ga_kind.ga_data);
	ପାଇଁ (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_data)[i]);
	vim_free(ga_menu.ga_data);
	ପାଇଁ (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_data)[i]);
	vim_free(ga_info.ga_data);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches)
{
    list_T      *retlist;

    *matches = ଖାଲି;
    *ଅଙ୍କ_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    ଯଦି (retlist == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    expand_process_user_list(retlist, matches, ଅଙ୍କ_Matches, xp);
    list_unref(retlist);
    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    ଶୂନ୍ୟ
globpath(
    char_u	*path,
    char_u	*file,
    garray_T	*ga,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		expand_options,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dirs)
{
    expand_T	xpc;
    char_u	*buf;
    size_t	pathlen;	    // length of the path portion of buf
				    // (including trailing slash)
    size_t	seplen;
    size_t	filelen;

    buf = alloc(MAXPATHL);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#ଯଦି defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#ଅନ୍ୟଥା
# define TMP_PATHSEPSTR PATHSEPSTR
#endif

    // Loop over all entries in {path}.
    ଯେତେବେଳେ (*path != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&path, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	ଯଦି (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଅଙ୍କ_p;
	    char_u  **p;

	    ଯଦି (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    ଯଦି (ExpandFromContext(&xpc, buf, &p, &ଅଙ୍କ_p,
			     WILD_SILENT|expand_options) != FAIL && ଅଙ୍କ_p > 0)
	    {
		ExpandEscape(&xpc, buf, ଅଙ୍କ_p, p, WILD_SILENT|expand_options);

		ଯଦି (ga_grow(ga, ଅଙ୍କ_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_p; ++i)
		    {
			((char_u **)ga->ga_data)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		ଅନ୍ୟଥା
		{
		    FreeWild(ଅଙ୍କ_p, p);
		    p = ଖାଲି;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
wildmenu_translate_ଚାବି(
	cmdline_info_T	*cclp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଚାବି,
	expand_T	*xp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_wild_list)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = ଚାବି;

    ଯଦି (cmdline_pum_active())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	ବାଛ (c)
	{
	    ସ୍ଥିତି K_UP:	  c = K_ବାମ; ଭାଙ୍ଗ;
	    ସ୍ଥିତି K_DOWN:  c = K_ଡାହାଣ; ଭାଙ୍ଗ;
	    ସ୍ଥିତି K_ବାମ:  c = K_UP; ଭାଙ୍ଗ;
	    ସ୍ଥିତି K_ଡାହାଣ: c = K_DOWN; ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	  ଭାଙ୍ଗ;
	}
    }

    ଯଦି (cmdline_pum_active() || did_wild_list || wild_menu_showing)
    {
	ଯଦି (c == K_ବାମ)
	    c = Ctrl_P;
	ଅନ୍ୟଥା ଯଦି (c == K_ଡାହାଣ)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    ଯଦି (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_DOWN;

    ଫେରନ୍ତୁ c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cmdline_del(cmdline_info_T *cclp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା from)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
wildmenu_process_ଚାବି_menunames(cmdline_info_T *cclp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି, expand_T *xp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    ଯଦି (ଚାବି == K_DOWN && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	ଚାବି = p_wc;
	ଚାବି_Typed = TRUE;  // in case the key was mapped
    }
    ଅନ୍ୟଥା ଯଦି (ଚାବି == K_UP)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା found = FALSE;

	j = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(xp->xp_pattern - cclp->cmdbuff);
	i = 0;
	ଯେତେବେଳେ (--j > 0)
	{
	    // check for start of menu name
	    ଯଦି (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		ଭାଙ୍ଗ;
	    }
	    // check for start of submenu name
	    ଯଦି (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		ଯଦି (found)
		{
		    i = j + 1;
		    ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା
		    found = TRUE;
	    }
	}
	ଯଦି (i > 0)
	    cmdline_del(cclp, i);
	ଚାବି = p_wc;
	ଚାବି_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    ଫେରନ୍ତୁ ଚାବି;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
wildmenu_process_ଚାବି_filenames(cmdline_info_T *cclp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି, expand_T *xp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;
    char_u	upseg[5];

    upseg[0] = PATHSEP;
    upseg[1] = '.';
    upseg[2] = '.';
    upseg[3] = PATHSEP;
    upseg[4] = NUL;

    ଯଦି (ଚାବି == K_DOWN
	    && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == PATHSEP
	    && (cclp->cmdpos < 3
		|| cclp->cmdbuff[cclp->cmdpos - 2] != '.'
		|| cclp->cmdbuff[cclp->cmdpos - 3] != '.'))
    {
	// go down a directory
	ଚାବି = p_wc;
	ଚାବି_Typed = TRUE;  // in case the key was mapped
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(xp->xp_pattern, upseg + 1, 3) == 0 && ଚାବି == K_DOWN)
    {
	// If in a direct ancestor, strip off one ../ to go down
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା found = FALSE;

	j = cclp->cmdpos;
	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(xp->xp_pattern - cclp->cmdbuff);
	ଯେତେବେଳେ (--j > i)
	{
	    ଯଦି (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    ଯଦି (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		found = TRUE;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (found
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    ଚାବି = p_wc;
	    ଚାବି_Typed = TRUE;  // in case the key was mapped
	}
    }
    ଅନ୍ୟଥା ଯଦି (ଚାବି == K_UP)
    {
	// go up a directory
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା found = FALSE;

	j = cclp->cmdpos - 1;
	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(xp->xp_pattern - cclp->cmdbuff);
	ଯେତେବେଳେ (--j > i)
	{
	    ଯଦି (has_mbyte)
		j -= (*mb_head_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    ଯଦି (vim_ispathsep(cclp->cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == ଖାଲି
#endif
	       )
	    {
		ଯଦି (found)
		{
		    i = j + 1;
		    ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା
		    found = TRUE;
	    }
	}

	ଯଦି (!found)
	    j = i;
	ଅନ୍ୟଥା ଯଦି (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	ଅନ୍ୟଥା ଯଦି (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	ଅନ୍ୟଥା
	    j = 0;
	ଯଦି (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	ଅନ୍ୟଥା ଯଦି (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	ଚାବି = p_wc;
	ଚାବି_Typed = TRUE;
    }

    ଫେରନ୍ତୁ ଚାବି;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
wildmenu_process_ଚାବି(cmdline_info_T *cclp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି, expand_T *xp)
{
    ଯଦି (xp->xp_context == EXPAND_MENUNAMES)
	ଫେରନ୍ତୁ wildmenu_process_ଚାବି_menunames(cclp, ଚାବି, xp);
    ଅନ୍ୟଥା ଯଦି ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	ଫେରନ୍ତୁ wildmenu_process_ଚାବି_filenames(cclp, ଚାବି, xp);

    ଫେରନ୍ତୁ ଚାବି;
}

/*
 * Free expanded names when finished walking through the matches
 */
    ଶୂନ୍ୟ
wildmenu_cleanup(cmdline_info_T *cclp UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା skt = ଚାବି_Typed;

    ଯଦି (!p_wmnu || wild_menu_showing == 0)
	ଫେରନ୍ତୁ;

#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_RedrawingDisabled = RedrawingDisabled;
    ଯଦି (cclp->ନିବେଶ_fn)
	RedrawingDisabled = 0;
#endif

#ଯଦି defined(FEAT_ସନ୍ଧାନ_EXTRA)
    // Clear highlighting applied during wildmenu activity
    set_no_hlsearch(TRUE);
#endif

    ଯଦି (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_row--;
	redrawcmd();
    }
    ଅନ୍ୟଥା ଯଦି (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	last_status(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    ଅନ୍ୟଥା
    {
	win_redraw_last_status(topframe);
	redraw_statuslines();
    }
    ଚାବି_Typed = skt;
    wild_menu_showing = 0;
#ifdef FEAT_EVAL
    ଯଦି (cclp->ନିବେଶ_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#endif
}

#ଯଦି defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    ଶୂନ୍ୟ
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		filtered = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options = WILD_SILENT | WILD_USE_NL | WILD_ADD_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    pat = tv_get_string(&argvars[0]);
    ଯଦି (check_for_string_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;
    type = tv_get_string(&argvars[1]);

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], ଖାଲି);

    ଯଦି (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    ଯଦି (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    ଯଦି (STRCMP(type, "cmdline") == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdline_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(pat);
	set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_pattern_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(xpc.xp_pattern);
	xpc.xp_col = cmdline_len;
    }
    ଅନ୍ୟଥା
    {
	char_u	*pattern_start;

	xpc.xp_pattern = pattern_start = pat;
	xpc.xp_pattern_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(xpc.xp_pattern);
	xpc.xp_ଧାଡ଼ି = pat;

	xpc.xp_context = cmdcomplete_str_to_type(type);
	ବାଛ (xpc.xp_context)
	{
	ସ୍ଥିତି EXPAND_NOTHING:
	    semsg(_(e_invalid_argument_str), type);
	    ଫେରନ୍ତୁ;

	ସ୍ଥିତି EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    ଯଦି (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		ଫେରନ୍ତୁ;
	    }

	    xpc.xp_arg = type + 7;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    ଯଦି (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_argument_str), type);
		ଫେରନ୍ତୁ;
	    }

	    xpc.xp_arg = type + 11;
	    ଭାଙ୍ଗ;

# ଯଦି defined(FEAT_MENU)
	ସ୍ଥିତି EXPAND_MENUS:
	    set_context_in_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_pattern, FALSE);
	    xpc.xp_pattern_len -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(xpc.xp_pattern - pattern_start);
	    ଭାଙ୍ଗ;
# endif

# ifdef FEAT_CSCOPE
	ସ୍ଥିତି EXPAND_CSCOPE:
	    set_context_in_cscope_cmd(&xpc, xpc.xp_pattern, CMD_cscope);
	    xpc.xp_pattern_len -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(xpc.xp_pattern - pattern_start);
	    ଭାଙ୍ଗ;
# endif

# ifdef FEAT_SIGNS
	ସ୍ଥିତି EXPAND_SIGN:
	    set_context_in_sign_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(xpc.xp_pattern - pattern_start);
	    ଭାଙ୍ଗ;
# endif

	ସ୍ଥିତି EXPAND_RUNTIME:
	    set_context_in_runtime_cmd(&xpc, xpc.xp_pattern);
	    xpc.xp_pattern_len -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(xpc.xp_pattern - pattern_start);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି EXPAND_SHELLCMDLINE:
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା context = EXPAND_SHELLCMDLINE;
	    set_context_for_wildcard_arg(ଖାଲି, xpc.xp_pattern, FALSE, &xpc,
					 &context);
	    xpc.xp_pattern_len -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(xpc.xp_pattern - pattern_start);
	    ଭାଙ୍ଗ;
	}

	ସ୍ଥିତି EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_ADD;
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
	}
    }

    {
	char_u	*pat_src = xpc.xp_pattern;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	pat_len = xpc.xp_pattern_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	ଯଦି (unesc != ଖାଲି)
	    pat_src = unesc;

	ଯଦି (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	ଅନ୍ୟଥା
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    ଯଦି (rettv_list_alloc(rettv) == OK && pat != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	ExpandOne(&xpc, pat, ଖାଲି, options, WILD_ALL_KEEP);

	ପାଇଁ (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    ଶୂନ୍ୟ
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(pat);
    set_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_to_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    ଶୂନ୍ୟ
f_cmdcomplete_info(typval_T *argvars UNUSED, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ret = OK;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL || ccline == ଖାଲି
	    || ccline->xpc == ଖାଲି || ccline->xpc->xp_files == ଖାଲି)
	ଫେରନ୍ତୁ;
    retdict = rettv->vval.v_dict;
    ret = dict_add_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cmdline_orig.length);
    ଯଦି (ret == OK)
	ret = dict_add_ସଂଖ୍ୟା(retdict, "pum_visible", pum_visible());
    ଯଦି (ret == OK)
	ret = dict_add_ସଂଖ୍ୟା(retdict, "selected", ccline->xpc->xp_selected);
    ଯଦି (ret == OK)
    {
	li = list_alloc();
	ଯଦି (li == ଖାଲି)
	    ଫେରନ୍ତୁ;
	ret = dict_add_list(retdict, "matches", li);
	ଯଦି (ret == FAIL)
	    list_unref(li);
	ପାଇଁ (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copy_substring_from_pos(pos_T *start, pos_T *end, char_u **match,
	pos_T *match_end)
{
    char_u	*word_end;
    char_u	*ଧାଡ଼ି, *start_ଧାଡ଼ି, *end_ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ଖାଲି;

    ଯଦି (start->lnum > end->lnum
	    || (start->lnum == end->lnum && start->col >= end->col))
	ଫେରନ୍ତୁ FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    start_ଧାଡ଼ି = ml_get(start->lnum);
    char_u  *start_ptr = start_ଧାଡ଼ି + start->col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_single_ଧାଡ଼ି = start->lnum == end->lnum;

    segment_len = is_single_ଧାଡ଼ି ? (end->col - start->col)
			: (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ml_get_len(start->lnum) - start->col);
    ଯଦି (ga_grow(&ga, segment_len + 2) != OK)
	ଯାଅ fail;

    ga_concat_len(&ga, start_ptr, segment_len);
    ଯଦି (!is_single_ଧାଡ଼ି)
    {
	ଯଦି (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	ଅନ୍ୟଥା
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    ଯଦି (!is_single_ଧାଡ଼ି)
    {
	ପାଇଁ (lnum = start->lnum + 1; lnum < end->lnum; lnum++)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା  linelen;

	    ଧାଡ଼ି = ml_get(lnum);
	    linelen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ml_get_len(lnum);
	    ଯଦି (ga_grow(&ga, linelen + 2) != OK)
		ଯାଅ fail;
	    ga_concat_len(&ga, ଧାଡ଼ି, linelen);
	    ଯଦି (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    ଅନ୍ୟଥା
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    end_ଧାଡ଼ି = ml_get(end->lnum);
    word_end = find_word_end(end_ଧାଡ଼ି + end->col);
    segment_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(word_end - end_ଧାଡ଼ି);
    ଯଦି (ga_grow(&ga, segment_len) != OK)
	ଯାଅ fail;
    ga_concat_len(&ga, end_ଧାଡ଼ି + (is_single_ଧାଡ଼ି ? end->col : 0),
	    segment_len - (is_single_ଧାଡ଼ି ? end->col : 0));

    // Null-terminate
    ଯଦି (ga_grow(&ga, 1) != OK)
	ଯାଅ fail;
    ga_append(&ga, NUL);

    *match = (char_u *)ga.ga_data;
    match_end->lnum = end->lnum;
    match_end->col = segment_len;

    ଫେରନ୍ତୁ OK;

fail:
    ga_clear(&ga);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_regex_match(char_u *pat, char_u *str)
{
    ଯଦି (STRCMP(pat, str) == 0)
	ଫେରନ୍ତୁ TRUE;

    regmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;

    ++emsg_off;
    ++ସନ୍ଦେଶ_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --ସନ୍ଦେଶ_silent;

    ଯଦି (regmatch.regprog == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    regmatch.rm_ic = p_ic;
    ଯଦି (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++ସନ୍ଦେଶ_silent;
    result = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --ସନ୍ଦେଶ_silent;

    vim_regfree(regmatch.regprog);
    ଫେରନ୍ତୁ result;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    ସ୍ଥିର char_u *
concat_pattern_with_buffer_match(
	char_u *pat,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pat_len,
	pos_T *end_match_pos,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା lowercase UNUSED)
{
    char_u  *ଧାଡ଼ି = ml_get(end_match_pos->lnum);
    char_u  *word_end = find_word_end(ଧାଡ଼ି + end_match_pos->col);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    match_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(word_end - (ଧାଡ଼ି + end_match_pos->col));
    char_u  *match = alloc(match_len + pat_len + 1);  // +1 for NUL

    ଯଦି (match == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    mch_memmove(match, pat, pat_len);
    ଯଦି (match_len > 0)
    {
#ଯଦି defined(FEAT_EVAL) || defined(FEAT_SPELL)
	ଯଦି (lowercase)
	{
	    char_u  *mword = vim_strnsave(ଧାଡ଼ି + end_match_pos->col,
		    match_len);
	    ଯଦି (mword == ଖାଲି)
		ଯାଅ cleanup;
	    char_u  *lower = strlow_save(mword);
	    vim_free(mword);
	    ଯଦି (lower == ଖାଲି)
		ଯାଅ cleanup;
	    mch_memmove(match + pat_len, lower, match_len);
	    vim_free(lower);
	}
	ଅନ୍ୟଥା
#endif
	    mch_memmove(match + pat_len, ଧାଡ଼ି + end_match_pos->col, match_len);
    }
    match[pat_len + match_len] = NUL;
    ଫେରନ୍ତୁ match;

#ଯଦି defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(match);
    ଫେରନ୍ତୁ ଖାଲି;
#endif
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_pattern_in_buf(
    char_u	*pat,		    // pattern to match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches)	    // return: number of matches
{
    pos_T	cur_match_pos, prev_match_pos, end_match_pos, word_end_pos;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_new_match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		looped_around = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pat_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_range = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compl_started = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସନ୍ଧାନ_flags;
    char_u	*match, *full_match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != ଖାଲି;

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    has_range = ସନ୍ଧାନ_first_ଧାଡ଼ି != 0;
#endif

    *matches = ଖାଲି;
    *ଅଙ୍କ_Matches = 0;

    ଯଦି (pat == ଖାଲି || *pat == NUL)
	ଫେରନ୍ତୁ FAIL;

    pat_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(pat);
    CLEAR_FIELD(cur_match_pos);
    CLEAR_FIELD(prev_match_pos);
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    ଯଦି (has_range)
	cur_match_pos.lnum = ସନ୍ଧାନ_first_ଧାଡ଼ି;
    ଅନ୍ୟଥା
#endif
	cur_match_pos = pre_incsearch_pos;

    ସନ୍ଧାନ_flags = ସନ୍ଧାନ_OPT | ସନ୍ଧାନ_NOOF | ସନ୍ଧାନ_PEEK | ସନ୍ଧାନ_NFMSG
	| (has_range ? ସନ୍ଧାନ_START : 0);

    ga_init2(&ga, ଆକାର(char_u *), 10); // Use growable array of char_u*

    ପାଇଁ (;;)
    {
	++emsg_off;
	++ସନ୍ଦେଶ_silent;
	found_new_match = searchit(ଖାଲି, curbuf, &cur_match_pos,
		&end_match_pos, dir, pat, pat_len, 1L,
		ସନ୍ଧାନ_flags, RE_LAST, ଖାଲି);
	--ସନ୍ଦେଶ_silent;
	--emsg_off;

	ଯଦି (found_new_match == FAIL)
	    ଭାଙ୍ଗ;

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	// If in range mode, check if match is within the range
	ଯଦି (has_range && (cur_match_pos.lnum < ସନ୍ଧାନ_first_ଧାଡ଼ି
		    || cur_match_pos.lnum > ସନ୍ଧାନ_last_ଧାଡ଼ି))
		ଭାଙ୍ଗ;
#endif

	ଯଦି (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    ଯଦି ((dir == FORWARD && LTOREQ_POS(cur_match_pos, prev_match_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(prev_match_pos, cur_match_pos)))
	    {
		ଯଦି (looped_around)
		    ଭାଙ୍ଗ;
		ଅନ୍ୟଥା
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	prev_match_pos = cur_match_pos;

	// Abort if user typed a character or interrupted
	ଯଦି (char_avail() || got_int)
	{
	    ଯଦି (got_int)
	    {
		(ଶୂନ୍ୟ)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    ଯାଅ cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	ଯଦି (end_match_pos.lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    cur_match_pos.lnum = 1;
	    cur_match_pos.col = 0;
	    cur_match_pos.coladd = 0;
	    ଜାରି;
	}

	// Extract the matching text prepended to completed word
	ଯଦି (copy_substring_from_pos(&cur_match_pos, &end_match_pos, &full_match,
		    &word_end_pos) == FAIL)
	    ଭାଙ୍ଗ;

	ଯଦି (exacttext)
	    match = full_match;
	ଅନ୍ୟଥା
	{
	    // Construct a new match from completed word appended to pattern itself
	    match = concat_pattern_with_buffer_match(pat, pat_len, &end_match_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    ଯଦି (match == ଖାଲି || !is_regex_match(match, full_match))
	    {
		vim_free(match);
		match = concat_pattern_with_buffer_match(pat, pat_len,
			&end_match_pos, TRUE);
		ଯଦି (match == ଖାଲି || !is_regex_match(match, full_match))
		{
		    vim_free(match);
		    vim_free(full_match);
		    ଜାରି;
		}
	    }
	    vim_free(full_match);
	}

	// Include this match if it is not a duplicate
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ga.ga_len; ++i)
	{
	    ଯଦି (STRCMP(match, ((char_u **)ga.ga_data)[i]) == 0)
	    {
		VIM_CLEAR(match);
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (match != ଖାଲି)
	{
	    ଯଦି (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(match);
		ଯାଅ cleanup;
	    }
	    ((char_u **)ga.ga_data)[ga.ga_len++] = match;
	    ଯଦି (ga.ga_len > TAG_MANY)
		ଭାଙ୍ଗ;
	}
	ଯଦି (has_range)
	    cur_match_pos = word_end_pos;
    }

    *matches = (char_u **)ga.ga_data;
    *ଅଙ୍କ_Matches = ga.ga_len;
    ଫେରନ୍ତୁ OK;

cleanup:
    ga_clear_strings(&ga);
    ଫେରନ୍ତୁ FAIL;
}
