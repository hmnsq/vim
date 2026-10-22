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

ساکن صحیح	cmd_showtail;	// Only show path tail in lists ?
ساکن صحیح	may_expand_نمونہ = FALSE;
ساکن pos_T	pre_incsearch_pos; // Cursor position when incsearch started

ساکن خالی	سیٹ_context_for_wildcard_arg(exarg_T *eap, char_u *arg, صحیح usefilter, expand_T *xp, صحیح *complp);
ساکن صحیح	ExpandFromContext(expand_T *xp, char_u *, char_u ***, صحیح *, صحیح);
ساکن char_u	*showmatches_gettail(char_u *s);
ساکن صحیح	expand_showtail(expand_T *xp);
ساکن صحیح	expand_shellcmd(char_u *filepat, char_u ***matches, صحیح *عدد_Matches, صحیح flagsarg);
#اگر defined(FEAT_EVAL)
ساکن صحیح	ExpandUserDefined(char_u *pat, expand_T *xp, regmatch_T *regmatch, char_u ***matches, صحیح *عدد_Matches);
ساکن صحیح	ExpandUserList(expand_T *xp, char_u ***matches, صحیح *عدد_Matches);
ساکن char_u	*apply_user_completeopt_escape(expand_T *xp, char_u *str);
ساکن char_u	*unescape_user_completeopt_pat(expand_T *xp, char_u *src, صحیح srclen, صحیح *new_lenp);
#اگر_ختم
ساکن صحیح	expand_نمونہ_اندر_buf(char_u *pat, صحیح dir, char_u ***matches, صحیح *عدد_Matches);

// "compl_match_array" points the currently displayed list of entries in the
// popup menu.  It is NULL when there is no popup menu.
ساکن pumitem_T *compl_مطابقت_array = تہی;
ساکن صحیح compl_مطابقت_arraysize;
// First column in cmdline of the matched item for completion.
ساکن صحیح compl_startcol;
ساکن صحیح compl_selected;
// cmdline before expansion
ساکن string_T cmdline_orig = {تہی, 0};

#تعریف_کرو SHOW_مطابقت(m) (showtail ? showmatches_gettail(matches[m]) : matches[m])

/*
 * Returns TRUE if fuzzy completion is supported for a given cmdline completion
 * context.
 */
    ساکن صحیح
cmdline_fuzzy_completion_supported(expand_T *xp)
{
    انتخاب (xp->xp_context)
    {
    حالت EXPAND_BOOL_SETTINGS:
    حالت EXPAND_COLORS:
    حالت EXPAND_COMPILER:
    حالت EXPAND_DIRECTORIES:
    حالت EXPAND_DIRS_اندر_CDPATH:
    حالت EXPAND_FILES:
    حالت EXPAND_FILES_اندر_راستہ:
    حالت EXPAND_FILETYPE:
    حالت EXPAND_FILETYPECMD:
    حالت EXPAND_HELP:
    حالت EXPAND_KEYMAP:
    حالت EXPAND_OLD_SETTING:
    حالت EXPAND_STRING_SETTING:
    حالت EXPAND_SETTING_تفریق:
    حالت EXPAND_OWNSYNTAX:
    حالت EXPAND_PACKADD:
    حالت EXPAND_RUNTIME:
    حالت EXPAND_SHELLCMD:
    حالت EXPAND_SHELLCMDLINE:
    حالت EXPAND_لیبل_فہرست:
    حالت EXPAND_لیبل_فہرست_LISTFILES:
	واپس FALSE;

    طےشدہ:
	توڑو;
    }

    واپس vim_strchr(p_wop, WOP_FUZZY) != تہی;
}

/*
 * Returns TRUE if fuzzy completion for cmdline completion is enabled and
 * 'fuzzystr' is not empty.  If search pattern is empty, then don't use fuzzy
 * matching.
 */
    صحیح
cmdline_fuzzy_complete(char_u *fuzzystr)
{
    واپس vim_strchr(p_wop, WOP_FUZZY) != تہی && *fuzzystr != NUL;
}

/*
 * sort function for the completion matches.
 * <SNR> functions should be sorted to the end.
 */
    ساکن صحیح
ترتیب_دو_func_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    char_u *p1 = *(char_u **)s1;
    char_u *p2 = *(char_u **)s2;

    اگر (*p1 != '<' && *p2 == '<') واپس -1;
    اگر (*p1 == '<' && *p2 != '<') واپس 1;
    واپس STRCMP(p1, p2);
}

/*
 * Escape special characters in the cmdline completion matches.
 */
    ساکن خالی
wildescape(
    expand_T	*xp,
    char_u	*str,
    صحیح		numfiles,
    char_u	**files)
{
    char_u	*p;
    صحیح		vse_what = xp->xp_context == EXPAND_BUFFERS
						       ? VSE_بفر : VSE_NONE;

    اگر (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_FILES_اندر_راستہ
	    || xp->xp_context == EXPAND_SHELLCMD
	    || xp->xp_context == EXPAND_BUFFERS
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_DIRS_اندر_CDPATH)
    {
	// Insert a backslash into a file name before a space, \, %, #
	// and wildmatch characters, except '~'.
	برائے (صحیح i = 0; i < numfiles; ++i)
	{
	    // for ":set path=" we need to escape spaces twice
	    اگر (xp->xp_backslash & XP_BS_THREE)
	    {
		حرف *pat = (xp->xp_backslash & XP_BS_COMMA) ?  " ," : " ";
		p = vim_strsave_escaped(files[i], (char_u *)pat);
		اگر (p != تہی)
		{
		    vim_free(files[i]);
		    files[i] = p;
#اگر defined(BACKSLASH_اندر_FILENAME)
		    p = vim_strsave_escaped(files[i], (char_u *)" ");
		    اگر (p != تہی)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
#اگر_ختم
		}
	    }
	    ورنہ اگر (xp->xp_backslash & XP_BS_COMMA)
	    {
		اگر (vim_strchr(files[i], ',') != تہی)
		{
		    p = vim_strsave_escaped(files[i], (char_u *)",");
		    اگر (p != تہی)
		    {
			vim_free(files[i]);
			files[i] = p;
		    }
		}
	    }
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	    p = vim_strsave_fnameescape(files[i], vse_what);
#ورنہ
	    p = vim_strsave_fnameescape(files[i],
		    xp->xp_shell ? VSE_SHELL : vse_what);
#اگر_ختم
	    اگر (p != تہی)
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
    ورنہ اگر (xp->xp_context == EXPAND_لیبل_فہرست)
    {
	// Insert a backslash before characters in a tag name that
	// would terminate the ":tag" command.
	برائے (صحیح i = 0; i < numfiles; ++i)
	{
	    p = vim_strsave_escaped(files[i], (char_u *)"\\|\"");
	    اگر (p != تہی)
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
    ساکن خالی
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
    صحیح		wild_navigate = (type == WILD_اگلا || type == WILD_پچھلا
	    || type == WILD_PAGEUP || type == WILD_PAGEDOWN);

    اگر (xp->xp_numfiles == -1)
    {
	pre_incsearch_pos = xp->xp_pre_incsearch_pos;
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (ccline->داخلہ_fn && ccline->xp_context == EXPAND_COMMANDS)
	{
	    // Expand commands typed in input() function
	    سیٹ_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, FALSE);
	}
	ورنہ
#اگر_ختم
	{
	    may_expand_نمونہ = options & WILD_MAY_EXPAND_نمونہ;
	    سیٹ_expand_context(xp);
	    may_expand_نمونہ = FALSE;
	}
	cmd_showtail = expand_showtail(xp);
    }

    اگر (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	واپس OK;  // Something illegal on command line
    }
    اگر (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	واپس FAIL;
    }

    i = (صحیح)(xp->xp_نمونہ - ccline->cmdbuff);
    xp->xp_نمونہ_len = ccline->cmdpos - i;

    // Skip showing matches if prefix is invalid during wildtrigger()
    اگر (from_wildtrigger_func && xp->xp_context == EXPAND_COMMANDS
	    && xp->xp_نمونہ_len == 0)
	واپس FAIL;

    // If cmd_silent is set then don't show the dots, because redrawcmd() below
    // won't remove them.
    اگر (!cmd_silent && !from_wildtrigger_func && !wild_navigate)
    {
	پیغام_puts("...");	    // show that we are busy
	برآمد_flush();
    }

    اگر (wild_navigate)
    {
	// Get next/previous match for a previous expanded pattern.
	p = ExpandOne(xp, تہی, تہی, 0, type);
    }
    ورنہ
    {
	char_u	*tmp;
	char_u	*pat_src = xp->xp_نمونہ;
	صحیح	pat_len = xp->xp_نمونہ_len;
#اگر defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	اگر (unesc != تہی)
	    pat_src = unesc;
#اگر_ختم

	اگر (cmdline_fuzzy_completion_supported(xp)
		|| xp->xp_context == EXPAND_نمونہ_اندر_BUF)
	    // Don't modify the search string
	    tmp = vim_strnsave(pat_src, pat_len);
	ورنہ
	    tmp = addstar(pat_src, pat_len, xp->xp_context);
#اگر defined(FEAT_EVAL)
	vim_free(unesc);
#اگر_ختم

	// Translate string into pattern and expand it.
	اگر (tmp == تہی)
	    p = تہی;
	ورنہ
	{
	    صحیح use_options = options |
		    WILD_HOME_REPLACE|WILD_جوڑو_SLASH|WILD_SILENT;
	    اگر (escape)
		use_options |= WILD_ESCAPE;
	    اگر (p_wic)
		use_options += WILD_ICASE;

	    p = ExpandOne(xp, tmp,
			 vim_strnsave(&ccline->cmdbuff[i], xp->xp_نمونہ_len),
							   use_options, type);
	    vim_free(tmp);
	    // longest match: make sure it is not shorter, happens with :help
	    اگر (p != تہی && type == WILD_LONGEST)
	    {
		صحیح	j;

		برائے (j = 0; j < xp->xp_نمونہ_len; ++j)
		{
		    char_u  c = ccline->cmdbuff[i + j];
		    اگر (c == '*' || c == '?')
			توڑو;
		}
		اگر ((صحیح)STRLEN(p) < j)
		    VIM_CLEAR(p);
	    }
	}
    }

    // Save cmdline before inserting selected item
    اگر (!wild_navigate && ccline->cmdbuff != تہی)
    {
	vim_free(cmdline_orig.string);
	cmdline_orig.string = vim_strnsave(ccline->cmdbuff, ccline->cmdlen);
	اگر (cmdline_orig.string == تہی)
	    cmdline_orig.لمبائی = 0;
	ورنہ
	    cmdline_orig.لمبائی = ccline->cmdlen;
    }

    اگر (p != تہی && !got_int && !(options & (WILD_NOSELECT | WILD_NOINSERT)))
    {
	size_t	plen = STRLEN(p);
	صحیح	difflen;
	صحیح	v = OK;

	difflen = (صحیح)plen - xp->xp_نمونہ_len;
	اگر (ccline->cmdlen + difflen + 4 > ccline->cmdbufflen)
	{
	    v = realloc_cmdbuff(ccline->cmdlen + difflen + 4);
	    xp->xp_نمونہ = ccline->cmdbuff + i;
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
    اگر (xp->xp_context == EXPAND_MAPPINGS && p == تہی)
	واپس FAIL;

    اگر (xp->xp_numfiles <= 0 && p == تہی)
	beep_flush();
    ورنہ اگر (xp->xp_numfiles == 1
	    && !(options & (WILD_NOSELECT | WILD_NOINSERT))
	    && !wild_navigate)
	// free expanded pattern
	(خالی)ExpandOne(xp, تہی, تہی, 0, WILD_FREE);

    vim_free(p);

    واپس OK;
}

/*
 * Create completion popup menu with items from 'matches'.
 */
    ساکن صحیح
cmdline_pum_create(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	صحیح		عدد_Matches,
	صحیح		showtail)
{
    صحیح	سابقہ_len;

    // Add all the completion matches
    compl_مطابقت_array = ALLOC_MULT(pumitem_T, عدد_Matches);
    اگر (compl_مطابقت_array == تہی)
	واپس EXPAND_UNSUCCESSFUL;

    compl_مطابقت_arraysize = عدد_Matches;
    برائے (صحیح i = 0; i < عدد_Matches; i++)
    {
	compl_مطابقت_array[i].pum_متن = (xp->xp_files_abbr != تہی
				    && xp->xp_files_abbr[i] != تہی)
					? xp->xp_files_abbr[i]
					: SHOW_مطابقت(i);
	compl_مطابقت_array[i].pum_info = xp->xp_files_info != تہی
					    ? xp->xp_files_info[i] : تہی;
	compl_مطابقت_array[i].pum_extra = xp->xp_files_menu != تہی
					    ? xp->xp_files_menu[i] : تہی;
	compl_مطابقت_array[i].pum_قسم = xp->xp_files_قسم != تہی
					    ? xp->xp_files_قسم[i] : تہی;
	compl_مطابقت_array[i].pum_user_abbr_hlattr = -1;
	compl_مطابقت_array[i].pum_user_قسم_hlattr = -1;
    }

    // Compute the popup menu starting column
    compl_startcol = ccline == تہی ? 0 : vim_strsize(ccline->cmdbuff) + 1;
    سابقہ_len = vim_strsize(xp->xp_نمونہ);
    اگر (showtail)
	سابقہ_len += vim_strsize(showmatches_gettail(matches[0]))
	    - vim_strsize(matches[0]);
    compl_startcol = cmdline_ستون_off + MAX(0, compl_startcol - سابقہ_len);

    واپس EXPAND_OK;
}

/*
 * Display the cmdline completion matches in a popup menu
 */
    خالی
cmdline_pum_display(خالی)
{
    اگر (p_po > 0 && p_po < 100 && !pum_redraw_اندر_same_position())
	pum_call_update_screen();
    pum_display(compl_مطابقت_array, compl_مطابقت_arraysize, compl_selected, -1);
}

/*
 * Returns TRUE if the cmdline completion popup menu is being displayed.
 */
    صحیح
cmdline_pum_فعال(خالی)
{
    واپس pum_visible() && compl_مطابقت_array != تہی;
}

/*
 * Remove the cmdline completion popup menu (if present), free the list of
 * items and refresh the screen.
 */
    خالی
cmdline_pum_remove(cmdline_info_T *cclp غیر_مستعمل, صحیح defer_redraw)
{
    صحیح	save_کلید_Typed = کلید_Typed;
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح	save_RedrawingDisabled = RedrawingDisabled;
    اگر (cclp->داخلہ_fn)
	RedrawingDisabled = 0;
#اگر_ختم

    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_ENABLE);
    pum_undisplay();
    VIM_CLEAR(compl_مطابقت_array);
    compl_مطابقت_arraysize = 0;
    اگر (!defer_redraw)
    {
	صحیح save_p_lz = p_lz;
	p_lz = FALSE;  // avoid the popup menu hanging around
	update_screen(0);
	p_lz = save_p_lz;
    }
    ورنہ
	pum_call_update_screen();
    redrawcmd();
    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_DISABLE);

    // When a function is called (e.g. for 'foldtext') KeyTyped might be reset
    // as a side effect.
    کلید_Typed = save_کلید_Typed;
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (cclp->داخلہ_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#اگر_ختم
}

    خالی
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
cmdline_compl_startcol(خالی)
{
    واپس compl_startcol;
}

/*
 * Returns the current cmdline completion pattern.
 */
    char_u *
cmdline_compl_نمونہ(خالی)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    واپس xp == تہی ? تہی : xp->xp_orig;
}

/*
 * Returns TRUE if fuzzy cmdline completion is active, FALSE otherwise.
 */
    صحیح
cmdline_compl_is_fuzzy(خالی)
{
    expand_T	*xp = get_cmdline_info()->xpc;

    واپس xp != تہی && cmdline_fuzzy_completion_supported(xp);
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags
 * and in search pattern completion matches.
 */
    ساکن صحیح
skip_حالت_مطابقت_char(expand_T *xp, char_u *s)
{
    اگر ((rem_backslash(s) && xp->xp_context != EXPAND_HELP
		&& xp->xp_context != EXPAND_نمونہ_اندر_BUF)
#اگر_تعریف_شدہ FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#اگر_ختم
	   )
    {
#اگر_غیر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	اگر (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    واپس 2;
#اگر_ختم
	واپس 1;
    }
    واپس 0;
}

/*
 * Get the length of an item as it will be shown in the status line.
 */
    ساکن صحیح
حالت_مطابقت_len(expand_T *xp, char_u *s)
{
    صحیح	len = 0;

#اگر_تعریف_شدہ FEAT_MENU
    صحیح emenu = xp->xp_context == EXPAND_MENUS
	     || xp->xp_context == EXPAND_MENUNAMES;

    // Check for menu separators - replace with '|'.
    اگر (emenu && menu_is_separator(s))
	واپس 1;
#اگر_ختم

    جب (*s != NUL)
    {
	s += skip_حالت_مطابقت_char(xp, s);
	len += ptr2cells(s);
	MB_PTR_ADV(s);
    }

    واپس len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    ساکن خالی
win_redr_حالت_matches(
    expand_T	*xp,
    صحیح		عدد_matches,
    char_u	**matches,	// list of matches
    صحیح		مطابقت,
    صحیح		showtail)
{
    صحیح		صف;
    char_u	*buf;
    صحیح		len;
    صحیح		clen;		// length in screen cells
    صحیح		fillchar;
    صحیح		attr;
    صحیح		i;
    صحیح		highlight = TRUE;
    char_u	*selstart = تہی;
    صحیح		selstart_ستون = 0;
    char_u	*selend = تہی;
    ساکن صحیح	پہلا_مطابقت = 0;
    صحیح		جوڑو_بایاں = FALSE;
    char_u	*s;
#اگر_تعریف_شدہ FEAT_MENU
    صحیح		emenu;
#اگر_ختم
    صحیح		l;

    اگر (matches == تہی)	// interrupted completion?
	واپس;

    اگر (has_mbyte)
	buf = alloc(topframe->fr_چوڑائی * MB_MAXBYTES + 1);
    ورنہ
	buf = alloc(topframe->fr_چوڑائی + 1);
    اگر (buf == تہی)
	واپس;

    اگر (مطابقت == -1)	// don't show match but original text
    {
	مطابقت = 0;
	highlight = FALSE;
    }
    // count 1 for the ending ">"
    clen = حالت_مطابقت_len(xp, SHOW_مطابقت(مطابقت)) + 3;
    اگر (مطابقت == 0)
	پہلا_مطابقت = 0;
    ورنہ اگر (مطابقت < پہلا_مطابقت)
    {
	// jumping left, as far as we can go
	پہلا_مطابقت = مطابقت;
	جوڑو_بایاں = TRUE;
    }
    ورنہ
    {
	// check if match fits on the screen
	برائے (i = پہلا_مطابقت; i < مطابقت; ++i)
	    clen += حالت_مطابقت_len(xp, SHOW_مطابقت(i)) + 2;
	اگر (پہلا_مطابقت > 0)
	    clen += 2;
	// jumping right, put match at the left
	اگر (clen > topframe->fr_چوڑائی)
	{
	    پہلا_مطابقت = مطابقت;
	    // if showing the last match, we can add some on the left
	    clen = 2;
	    برائے (i = مطابقت; i < عدد_matches; ++i)
	    {
		clen += حالت_مطابقت_len(xp, SHOW_مطابقت(i)) + 2;
		اگر (clen >= topframe->fr_چوڑائی)
		    توڑو;
	    }
	    اگر (i == عدد_matches)
		جوڑو_بایاں = TRUE;
	}
    }
    اگر (جوڑو_بایاں)
	جب (پہلا_مطابقت > 0)
	{
	    clen += حالت_مطابقت_len(xp, SHOW_مطابقت(پہلا_مطابقت - 1)) + 2;
	    اگر (clen >= topframe->fr_چوڑائی)
		توڑو;
	    --پہلا_مطابقت;
	}

    fillchar = fillchar_حالت(&attr, curwin);

    اگر (پہلا_مطابقت == 0)
    {
	*buf = NUL;
	len = 0;
    }
    ورنہ
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = پہلا_مطابقت;
    جب (clen + حالت_مطابقت_len(xp, SHOW_مطابقت(i)) + 2 < topframe->fr_چوڑائی)
    {
	اگر (i == مطابقت)
	{
	    selstart = buf + len;
	    selstart_ستون = clen;
	}

	s = SHOW_مطابقت(i);
	// Check for menu separators - replace with '|'
#اگر_تعریف_شدہ FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	اگر (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (صحیح)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	ورنہ
#اگر_ختم
	برائے ( ; *s != NUL; ++s)
	{
	    s += skip_حالت_مطابقت_char(xp, s);
	    clen += ptr2cells(s);
	    اگر (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    ورنہ
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (صحیح)STRLEN(buf + len);
	    }
	}
	اگر (i == مطابقت)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	اگر (++i == عدد_matches)
		توڑو;
    }

    اگر (i != عدد_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    صف = cmdline_صف - 1;
    اگر (صف >= 0)
    {
	اگر (wild_menu_showing == 0)
	{
	    اگر (پیغام_scrolled > 0)
	    {
		// Put the wildmenu just above the command line.  If there is
		// no room, scroll the screen one line up.
		اگر (cmdline_صف == صفیں - 1)
		{
		    screen_del_سطریں(0, 0, 1, (صحیح)صفیں, TRUE, 0, تہی);
		    ++پیغام_scrolled;
		}
		ورنہ
		{
		    ++cmdline_صف;
		    ++صف;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    ورنہ
	    {
		// Create status line if needed by setting 'laststatus' to 2.
		// Set 'winminheight' to zero to avoid that the window is
		// resized.
		اگر (lastwin->w_حالت_اونچائی == 0)
		{
		    save_p_ls = p_ls;
		    save_p_wmh = p_wmh;
		    p_ls = 2;
		    p_wmh = 0;
		    آخری_حالت(FALSE);
		}
		wild_menu_showing = WM_SHOWN;
	    }
	}

	screen_puts(buf, صف, firstwin->w_wincol, attr);
	اگر (selstart != تہی && highlight)
	{
	    *selend = NUL;
	    screen_puts(selstart, صف, firstwin->w_wincol + selstart_ستون,
		    HL_ATTR(HLF_WM));
	}

	screen_fill(صف, صف + 1, firstwin->w_wincol + clen,
		firstwin->w_wincol + topframe->fr_چوڑائی,
		fillchar, fillchar, attr);
    }

    win_redraw_آخری_حالت(topframe);
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
    ساکن char_u *
apply_user_completeopt_escape(expand_T *xp, char_u *str)
{
    char_u  *p;

    اگر (str == تہی)
	واپس تہی;
    اگر ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	واپس str;
    p = vim_strsave_escaped(str, (char_u *)" \t\\");
    اگر (p == تہی)
	واپس str;
    vim_free(str);
    واپس p;
}

/*
 * For -completeopt=escape on a user command, build the "logical" ArgLead by
 * collapsing a backslash before a space, tab or backslash in the typed text.
 * The completion function then sees "foo bar" instead of "foo\ bar".
 * Returns a newly-allocated string and stores its length in "*new_lenp", or
 * NULL when no unescape is applicable (caller should keep the original).
 */
    ساکن char_u *
unescape_user_completeopt_pat(
    expand_T	*xp,
    char_u	*src,
    صحیح		srclen,
    صحیح		*new_lenp)
{
    char_u  *buf, *p, *d, *انتہا;

    اگر ((xp->xp_context != EXPAND_USER_DEFINED
		&& xp->xp_context != EXPAND_USER_LIST)
	    || !(xp->xp_complete_opt & UCC_ESCAPE))
	واپس تہی;

    buf = alloc(srclen + 1);
    اگر (buf == تہی)
	واپس تہی;

    d = buf;
    انتہا = src + srclen;
    برائے (p = src; p < انتہا; ++p)
    {
	اگر (*p == '\\' && p + 1 < انتہا
			   && (p[1] == ' ' || p[1] == TAB || p[1] == '\\'))
	    ++p;
	*d++ = *p;
    }
    *d = NUL;
    *new_lenp = (صحیح)(d - buf);
    واپس buf;
}
#اگر_ختم

/*
 * Get the next or prev cmdline completion match. The index of the match is set
 * in "xp->xp_selected"
 */
    ساکن char_u *
get_اگلا_or_پچھلا_مطابقت(صحیح منوال, expand_T *xp)
{
    صحیح	    findex = xp->xp_selected;
    صحیح	    ht;

    // When no matches found, return NULL
    اگر (xp->xp_numfiles <= 0)
	واپس تہی;

    اگر (منوال == WILD_پچھلا)
    {
	// Select the last entry if at original text
	اگر (findex == -1)
	    findex = xp->xp_numfiles;
	// Otherwise select the previous entry
	--findex;
    }
    ورنہ اگر (منوال == WILD_اگلا)
    {
	// Select the next entry
	++findex;
    }
    ورنہ   // WILD_PAGEDOWN or WILD_PAGEUP
    {
	// Get the height of popup menu (used for both PAGEUP and PAGEDOWN)
	ht = pum_get_اونچائی();
	اگر (ht > 3)
	    ht -= 2;

	اگر (منوال == WILD_PAGEUP)
	{
	    اگر (findex == 0)
		// at the first entry, don't select any entries
		findex = -1;
	    ورنہ اگر (findex < 0)
		// no entry is selected. select the last entry
		findex = xp->xp_numfiles - 1;
	    ورنہ
		// go up by the pum height
		findex = MAX(findex - ht, 0);
	}
	ورنہ    // mode == WILD_PAGEDOWN
	{
	    اگر (findex >= xp->xp_numfiles - 1)
		// at the last entry, don't select any entries
		findex = -1;
	    ورنہ اگر (findex < 0)
		// no entry is selected, select the first entry
		findex = 0;
	    ورنہ
		// go down by the pum height
		findex = MIN(findex + ht, xp->xp_numfiles - 1);
	}
    }

    // Handle wrapping around
    اگر (findex < 0 || findex >= xp->xp_numfiles)
    {
	// If original text exists, return to it when wrapping around
	اگر (xp->xp_orig != تہی)
	    findex = -1;
	ورنہ
	    // Wrap around to opposite end
	    findex = (findex < 0) ? xp->xp_numfiles - 1 : 0;
    }

    // Display matches on screen
    اگر (p_wmnu)
    {
	اگر (compl_مطابقت_array)
	{
	    compl_selected = findex;
	    cmdline_pum_display();
	}
	ورنہ اگر (vim_strchr(p_wop, WOP_PUM) != تہی)
	{
	    اگر (cmdline_pum_create(get_cmdline_info(), xp, xp->xp_files,
			xp->xp_numfiles, cmd_showtail) == EXPAND_OK)
	    {
		compl_selected = findex;
		pum_clear();
		cmdline_pum_display();
	    }
	}
	ورنہ
	    win_redr_حالت_matches(xp, xp->xp_numfiles, xp->xp_files, findex,
		    cmd_showtail);
    }

    xp->xp_selected = findex;
    // Return the original text or the selected match
    اگر (findex == -1)
	واپس vim_strsave(xp->xp_orig);
#اگر defined(FEAT_EVAL)
    واپس apply_user_completeopt_escape(xp, vim_strsave(xp->xp_files[findex]));
#ورنہ
    واپس vim_strsave(xp->xp_files[findex]);
#اگر_ختم
}

/*
 * Start the command-line expansion and get the matches.
 */
    ساکن char_u *
Expand_One_آغاز(صحیح منوال, expand_T *xp, char_u *str, صحیح options)
{
    صحیح		non_suf_مطابقت;		// number without matching suffix
    صحیح		i;
    char_u	*ss = تہی;

    // Do the expansion.
    اگر (ExpandFromContext(xp, str, &xp->xp_files, &xp->xp_numfiles,
		options) == FAIL)
    {
#اگر_تعریف_شدہ FNAME_ILLEGAL
	// Illegal file name has been silently skipped.  But when there
	// are wildcards, the real problem is that there was no match,
	// causing the pattern to be added, which has illegal characters.
	اگر (!(options & WILD_SILENT) && (options & WILD_LIST_NOTFOUND))
	    semsg(_(e_no_مطابقت_str_2), str);
#اگر_ختم
    }
    ورنہ اگر (xp->xp_numfiles == 0)
    {
	اگر (!(options & WILD_SILENT))
	    semsg(_(e_no_مطابقت_str_2), str);
    }
    ورنہ
    {
	// Escape the matches for use on the command line.
	ExpandEscape(xp, str, xp->xp_numfiles, xp->xp_files, options);

	// Check for matching suffixes in file names.
	اگر (منوال != WILD_ALL && منوال != WILD_ALL_KEEP && منوال != WILD_LONGEST)
	{
	    اگر (xp->xp_numfiles)
		non_suf_مطابقت = xp->xp_numfiles;
	    ورنہ
		non_suf_مطابقت = 1;
	    اگر ((xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_DIRECTORIES)
		    && xp->xp_numfiles > 1)
	    {
		// More than one match; check suffix.
		// The files will have been sorted on matching suffix in
		// expand_wildcards, only need to check the first two.
		non_suf_مطابقت = 0;
		برائے (i = 0; i < 2; ++i)
		    اگر (مطابقت_suffix(xp->xp_files[i]))
			++non_suf_مطابقت;
	    }
	    اگر (non_suf_مطابقت != 1)
	    {
		// Can we ever get here unless it's while expanding
		// interactively?  If not, we can get rid of this all
		// together. Don't really want to wait for this message
		// (and possibly have to hit return to continue!).
		اگر (!(options & WILD_SILENT))
		    emsg(_(e_too_many_file_نام));
		ورنہ اگر (!(options & WILD_NO_BEEP))
		    beep_flush();
	    }
	    اگر (!(non_suf_مطابقت != 1 && منوال == WILD_EXPAND_FREE))
		ss = vim_strsave(xp->xp_files[0]);
	}
    }

    واپس ss;
}

/*
 * Return the longest common part in the list of cmdline completion matches.
 */
    ساکن char_u *
ڈھونڈو_longest_مطابقت(expand_T *xp, صحیح options)
{
    long_u	len;
    صحیح		mb_len = 1;
    صحیح		c0, ci;
    صحیح		i;
    char_u	*ss;

    برائے (len = 0; xp->xp_files[0][len]; len += mb_len)
    {
	اگر (has_mbyte)
	{
	    mb_len = (*mb_ptr2len)(&xp->xp_files[0][len]);
	    c0 = (*mb_ptr2char)(&xp->xp_files[0][len]);
	}
	ورنہ
	    c0 = xp->xp_files[0][len];
	برائے (i = 1; i < xp->xp_numfiles; ++i)
	{
	    اگر (has_mbyte)
		ci = (*mb_ptr2char)(&xp->xp_files[i][len]);
	    ورنہ
		ci = xp->xp_files[i][len];
	    اگر (p_fic && (xp->xp_context == EXPAND_DIRECTORIES
			|| xp->xp_context == EXPAND_FILES
			|| xp->xp_context == EXPAND_SHELLCMD
			|| xp->xp_context == EXPAND_BUFFERS))
	    {
		اگر (MB_TOLOWER(c0) != MB_TOLOWER(ci))
		    توڑو;
	    }
	    ورنہ اگر (c0 != ci)
		توڑو;
	}
	اگر (i < xp->xp_numfiles)
	{
	    اگر (!(options & WILD_NO_BEEP))
		vim_beep(BO_WILD);
	    توڑو;
	}
    }

    ss = alloc(len + 1);
    اگر (ss)
	vim_strncpy(ss, xp->xp_files[0], (size_t)len);

    واپس ss;
}

    خالی
free_xp_files_extra(expand_T *xp, صحیح numfiles)
{
    اگر (xp->xp_files_abbr != تہی)
    {
	FreeWild(numfiles, xp->xp_files_abbr);
	xp->xp_files_abbr = تہی;
    }
    اگر (xp->xp_files_قسم != تہی)
    {
	FreeWild(numfiles, xp->xp_files_قسم);
	xp->xp_files_قسم = تہی;
    }
    اگر (xp->xp_files_menu != تہی)
    {
	FreeWild(numfiles, xp->xp_files_menu);
	xp->xp_files_menu = تہی;
    }
    اگر (xp->xp_files_info != تہی)
    {
	FreeWild(numfiles, xp->xp_files_info);
	xp->xp_files_info = تہی;
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
    صحیح		منوال)
{
    char_u	*ss = تہی;
    صحیح		orig_saved = FALSE;
#اگر defined(FEAT_EVAL)
    // ss_is_match is TRUE when ss is derived from xp_files and should be
    // escaped per -completeopt=escape before being inserted.  WILD_CANCEL
    // and WILD_APPLY-without-selection return xp_orig unchanged.
    صحیح		ss_is_مطابقت = FALSE;
#اگر_ختم

    // first handle the case of using an old match
    اگر (منوال == WILD_اگلا || منوال == WILD_پچھلا
	    || منوال == WILD_PAGEUP || منوال == WILD_PAGEDOWN)
	واپس get_اگلا_or_پچھلا_مطابقت(منوال, xp);

    اگر (منوال == WILD_CANCEL)
	ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
    ورنہ اگر (منوال == WILD_APPLY)
    {
	اگر (xp->xp_selected == -1)
	    ss = vim_strsave(xp->xp_orig ? xp->xp_orig : (char_u *)"");
	ورنہ
	{
	    ss = vim_strsave(xp->xp_files[xp->xp_selected]);
#اگر defined(FEAT_EVAL)
	    ss_is_مطابقت = TRUE;
#اگر_ختم
	}
    }

    // free old names
    اگر (xp->xp_numfiles != -1 && منوال != WILD_ALL && منوال != WILD_LONGEST)
    {
	FreeWild(xp->xp_numfiles, xp->xp_files);
	free_xp_files_extra(xp, xp->xp_numfiles);
	xp->xp_numfiles = -1;
	VIM_CLEAR(xp->xp_orig);

	// The entries from xp_files may be used in the PUM, remove it.
	اگر (compl_مطابقت_array != تہی)
	    cmdline_pum_remove(get_cmdline_info(), FALSE);
    }
    xp->xp_selected = (options & WILD_NOSELECT) ? -1 : 0;

    اگر (منوال == WILD_FREE)	// only release file name
	واپس تہی;

    اگر (xp->xp_numfiles == -1 && منوال != WILD_APPLY && منوال != WILD_CANCEL)
    {
	vim_free(xp->xp_orig);
	xp->xp_orig = orig;
	orig_saved = TRUE;

	ss = Expand_One_آغاز(منوال, xp, str, options);
#اگر defined(FEAT_EVAL)
	اگر (ss != تہی)
	    ss_is_مطابقت = TRUE;
#اگر_ختم
    }

    // Find longest common part
    اگر (منوال == WILD_LONGEST && xp->xp_numfiles > 0)
    {
	ss = ڈھونڈو_longest_مطابقت(xp, options);
	xp->xp_selected = -1;			// next p_wc gets first one
#اگر defined(FEAT_EVAL)
	اگر (ss != تہی)
	    ss_is_مطابقت = TRUE;
#اگر_ختم
    }

    // Concatenate all matching names.  Unless interrupted, this can be slow
    // and the result probably won't be used.
    اگر (منوال == WILD_ALL && xp->xp_numfiles > 0 && !got_int)
    {
	size_t	ss_size = 0;
	حرف	*سابقہ = "";
	حرف	*suffix = (options & WILD_USE_NL) ? "\n" : " ";
	صحیح	n = xp->xp_numfiles - 1;
	صحیح	i;
#اگر defined(FEAT_EVAL)
	char_u	**files = xp->xp_files;
	char_u	**escaped = تہی;

	// When -completeopt=escape is set for a user command, escape each
	// match before joining so the separator spaces stay unescaped.
	اگر ((xp->xp_context == EXPAND_USER_DEFINED
		    || xp->xp_context == EXPAND_USER_LIST)
		&& (xp->xp_complete_opt & UCC_ESCAPE))
	{
	    escaped = ALLOC_MULT(char_u *, xp->xp_numfiles);
	    اگر (escaped != تہی)
	    {
		برائے (i = 0; i < xp->xp_numfiles; ++i)
		{
		    escaped[i] = vim_strsave_escaped(xp->xp_files[i],
						       (char_u *)" \t\\");
		    اگر (escaped[i] == تہی)
		    {
			جب (--i >= 0)
			    vim_free(escaped[i]);
			VIM_CLEAR(escaped);
			توڑو;
		    }
		}
		اگر (escaped != تہی)
		    files = escaped;
	    }
	}
#ورنہ
	char_u	**files = xp->xp_files;
#اگر_ختم

	اگر (xp->xp_سابقہ == XP_سابقہ_NO)
	{
	    سابقہ = "no";
	    ss_size = STRLEN_LITERAL("no") * n;
	}
	ورنہ اگر (xp->xp_سابقہ == XP_سابقہ_INV)
	{
	    سابقہ = "inv";
	    ss_size = STRLEN_LITERAL("inv") * n;
	}

	برائے (i = 0; i < xp->xp_numfiles; ++i)
	    ss_size += STRLEN(files[i]) + 1;		// +1 for the suffix
	++ss_size;					// +1 for the NUL

	ss = alloc(ss_size);
	اگر (ss != تہی)
	{
	    size_t  ss_len = 0;

	    برائے (i = 0; i < xp->xp_numfiles; ++i)
	    {
		ss_len += vim_snprintf_safelen(
		    (حرف *)ss + ss_len,
		    ss_size - ss_len,
		    "%s%s%s",
		    (i > 0) ? سابقہ : "",
		    (حرف *)files[i],
		    (i < n) ? suffix : "");
	    }
	}
#اگر defined(FEAT_EVAL)
	اگر (escaped != تہی)
	{
	    برائے (i = 0; i < xp->xp_numfiles; ++i)
		vim_free(escaped[i]);
	    vim_free(escaped);
	}
#اگر_ختم
    }

    اگر (منوال == WILD_EXPAND_FREE || منوال == WILD_ALL)
	ExpandCleanup(xp);

    // Free "orig" if it wasn't stored in "xp->xp_orig".
    اگر (!orig_saved)
	vim_free(orig);

#اگر defined(FEAT_EVAL)
    // WILD_ALL already escaped its component matches in place, so don't
    // re-escape the joined string (its separator spaces would break).
    اگر (ss_is_مطابقت && منوال != WILD_ALL)
	ss = apply_user_completeopt_escape(xp, ss);
#اگر_ختم
    واپس ss;
}

/*
 * Prepare an expand structure for use.
 */
    خالی
ExpandInit(expand_T *xp)
{
    CLEAR_اشارہ_گر(xp);
    xp->xp_backslash = XP_BS_NONE;
    xp->xp_سابقہ = XP_سابقہ_NONE;
    xp->xp_numfiles = -1;
}

/*
 * Cleanup an expand structure after use.
 */
    خالی
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

    خالی
clear_cmdline_orig(خالی)
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
    ساکن خالی
showmatches_oneline(
	expand_T	*xp,
	char_u		**matches,
	صحیح		عدد_Matches,
	صحیح		سطریں,
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
    برائے (j = linenr; j < عدد_Matches; j += سطریں)
    {
	اگر (xp->xp_context == EXPAND_لیبل_فہرست_LISTFILES)
	{
	    پیغام_outtrans_attr(matches[j], HL_ATTR(HLF_D));
	    p = matches[j] + STRLEN(matches[j]) + 1;
	    پیغام_advance(maxlen + 1);
	    پیغام_puts((حرف *)p);
	    پیغام_advance(maxlen + 3);
	    پیغام_outtrans_long_attr(p + 2, HL_ATTR(HLF_D));
	    توڑو;
	}
	برائے (i = maxlen - lastlen; --i >= 0; )
	    پیغام_putchar(' ');
	اگر (xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_SHELLCMD
		|| xp->xp_context == EXPAND_BUFFERS)
	{
	    // highlight directories
	    اگر (xp->xp_numfiles != -1)
	    {
		char_u	*halved_slash;
		char_u	*exp_راستہ;
		char_u	*راستہ;

		// Expansion was done before and special characters
		// were escaped, need to halve backslashes.  Also
		// $HOME has been replaced with ~/.
		exp_راستہ = expand_env_save_opt(matches[j], TRUE, تہی);
		راستہ = exp_راستہ != تہی ? exp_راستہ : matches[j];
		halved_slash = backslash_halve_save(راستہ);
		isdir = mch_isdir(halved_slash != تہی ? halved_slash
			: matches[j]);
		vim_free(exp_راستہ);
		اگر (halved_slash != راستہ)
		    vim_free(halved_slash);
	    }
	    ورنہ
		// Expansion was done here, file names are literal.
		isdir = mch_isdir(matches[j]);
	    اگر (showtail)
		p = SHOW_مطابقت(j);
	    ورنہ
	    {
		home_replace(تہی, matches[j], NameBuff, MAXPATHL,
			TRUE);
		p = NameBuff;
	    }
	}
	ورنہ
	{
	    isdir = FALSE;
	    p = SHOW_مطابقت(j);
	}
	lastlen = پیغام_outtrans_attr(p, isdir ? dir_attr : 0);
    }
    اگر (پیغام_ستون > 0)	// when not wrapped around
    {
	پیغام_clr_eos();
	پیغام_putchar('\n');
    }
    برآمد_flush();		    // show one line at a time
}

/*
 * Show the matches in a popup menu, with the first one selected unless
 * "noselect" is set.
 */
    ساکن صحیح
show_pum_matches(
	cmdline_info_T	*ccline,
	expand_T	*xp,
	char_u		**matches,
	صحیح		عدد_Matches,
	صحیح		showtail,
	صحیح		noselect)
{
    صحیح	retval = cmdline_pum_create(ccline, xp, matches, عدد_Matches, showtail);

    اگر (retval == EXPAND_OK)
    {
	compl_selected = noselect ? -1 : 0;
	pum_clear();
	cmdline_pum_display();
    }
    واپس retval;
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
    صحیح		عدد_Matches;
    char_u	**matches;
    صحیح		i;
    صحیح		maxlen;
    صحیح		سطریں;
    صحیح		columns;
    صحیح		attr;
    صحیح		showtail;
    صحیح		noselect = (wim_flags_arg & WIM_NOSELECT);
    صحیح		noinsert = (wim_flags_arg & WIM_NOINSERT);
    صحیح		cmdline_unchanged = noselect || noinsert;

    اگر (xp->xp_numfiles == -1)
    {
	صحیح retval;
	سیٹ_expand_context(xp);
	retval = expand_cmdline(xp, ccline->cmdbuff, ccline->cmdpos,
		&عدد_Matches, &matches);
	اگر (retval != EXPAND_OK)
	    واپس retval;
	showtail = expand_showtail(xp);
    }
    ورنہ
    {
	عدد_Matches = xp->xp_numfiles;
	matches = xp->xp_files;
	showtail = cmd_showtail;
    }

    اگر (display_wildmenu && !display_list
	    && vim_strchr(p_wop, WOP_PUM) != تہی)
	واپس show_pum_matches(ccline, xp, matches, عدد_Matches,
		showtail && !cmdline_unchanged, noselect);

    اگر (display_list)
    {
	پیغام_didany = FALSE;		// lines_left will be set
	پیغام_آغاز();			// prepare for paging
	پیغام_putchar('\n');
	برآمد_flush();
	cmdline_صف = پیغام_صف;
	پیغام_didany = FALSE;		// lines_left will be set again
	پیغام_آغاز();			// prepare for paging
    }

    اگر (got_int)
	got_int = FALSE;  // only interrupt the completion, not the cmd line
    ورنہ اگر (display_wildmenu && !display_list)
	win_redr_حالت_matches(xp, عدد_Matches, matches, noselect ? -1 : 0,
		showtail);  // display statusbar menu
    ورنہ اگر (display_list)
    {
	// find the length of the longest file name
	maxlen = 0;
	برائے (i = 0; i < عدد_Matches; ++i)
	{
	    صحیح	len;
	    اگر (!showtail && (xp->xp_context == EXPAND_FILES
			  || xp->xp_context == EXPAND_SHELLCMD
			  || xp->xp_context == EXPAND_BUFFERS))
	    {
		home_replace(تہی, matches[i], NameBuff, MAXPATHL, TRUE);
		len = vim_strsize(NameBuff);
	    }
	    ورنہ
		len = vim_strsize(SHOW_مطابقت(i));
	    اگر (len > maxlen)
		maxlen = len;
	}

	اگر (xp->xp_context == EXPAND_لیبل_فہرست_LISTFILES)
	    سطریں = عدد_Matches;
	ورنہ
	{
	    // compute the number of columns and lines for the listing
	    maxlen += 2;    // two spaces between file names
	    columns = (cmdline_چوڑائی + 2) / maxlen;
	    اگر (columns < 1)
		columns = 1;
	    سطریں = (عدد_Matches + columns - 1) / columns;
	}

	attr = HL_ATTR(HLF_D);	// find out highlighting for directories

	اگر (xp->xp_context == EXPAND_لیبل_فہرست_LISTFILES)
	{
	    پیغام_puts_attr(_("tagname"), HL_ATTR(HLF_T));
	    پیغام_clr_eos();
	    پیغام_advance(maxlen - 3);
	    پیغام_puts_attr(_(" kind file\n"), HL_ATTR(HLF_T));
	}

	// list the files line by line
	برائے (i = 0; i < سطریں; ++i)
	{
	    showmatches_oneline(xp, matches, عدد_Matches, سطریں, i,
						maxlen, showtail, attr);
	    اگر (got_int)
	    {
		got_int = FALSE;
		توڑو;
	    }
	}

	// we redraw the command below the lines that we have just listed
	// This is a bit tricky, but it saves a lot of screen updating.
	cmdline_صف = پیغام_صف;	// will put it back later
    }

    // "list" and "full" in the same 'wildmode' phase: the matches are listed
    // above and shown in the menu as well.
    اگر (display_wildmenu && display_list)
    {
	اگر (vim_strchr(p_wop, WOP_PUM) != تہی)
	    (خالی)show_pum_matches(ccline, xp, matches, عدد_Matches,
		    showtail && !cmdline_unchanged, noselect);
	ورنہ
	    win_redr_حالت_matches(xp, عدد_Matches, matches,
		    noselect ? -1 : 0, showtail);
    }

    اگر (xp->xp_numfiles == -1)
    {
	FreeWild(عدد_Matches, matches);
	free_xp_files_extra(xp, عدد_Matches);
    }

    واپس EXPAND_OK;
}

/*
 * gettail() version for showmatches() and win_redr_status_matches():
 * Return the tail of file name path "s", ignoring a trailing "/".
 */
    ساکن char_u *
showmatches_gettail(char_u *s)
{
    char_u	*p;
    char_u	*t = s;
    صحیح		had_sep = FALSE;

    برائے (p = s; *p != NUL; )
    {
	اگر (vim_ispathsep(*p)
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
		&& !rem_backslash(p)
#اگر_ختم
	   )
	    had_sep = TRUE;
	ورنہ اگر (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
	MB_PTR_ADV(p);
    }
    واپس t;
}

/*
 * Return TRUE if we only need to show the tail of completion matches.
 * When not completing file names or there is a wildcard in the path FALSE is
 * returned.
 */
    ساکن صحیح
expand_showtail(expand_T *xp)
{
    char_u	*s;
    char_u	*انتہا;

    // When not completing file names a "/" may mean something different.
    اگر (xp->xp_context != EXPAND_FILES
	    && xp->xp_context != EXPAND_SHELLCMD
	    && xp->xp_context != EXPAND_DIRECTORIES)
	واپس FALSE;

    انتہا = gettail(xp->xp_نمونہ);
    اگر (انتہا == xp->xp_نمونہ)		// there is no path separator
	واپس FALSE;

    برائے (s = xp->xp_نمونہ; s < انتہا; s++)
    {
	// Skip escaped wildcards.  Only when the backslash is not a path
	// separator, on DOS the '*' "path\*\file" must not be skipped.
	اگر (rem_backslash(s))
	    ++s;
	ورنہ اگر (vim_strchr((char_u *)"*?[", *s) != تہی)
	    واپس FALSE;
    }
    واپس TRUE;
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
    char_u	*دم;
    صحیح		سرے_اندر_ستارہ;

    اگر (context != EXPAND_FILES
	    && context != EXPAND_FILES_اندر_راستہ
	    && context != EXPAND_SHELLCMD
	    && context != EXPAND_DIRECTORIES
	    && context != EXPAND_DIRS_اندر_CDPATH)
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
		|| ((context == EXPAND_لیبل_فہرست_LISTFILES
			|| context == EXPAND_لیبل_فہرست)
		    && fname[0] == '/'))
	    retval = vim_strnsave(fname, len);
	ورنہ
	{
	    new_len = len + 2;		// +2 for '^' at start, NUL at end
	    برائے (i = 0; i < len; i++)
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
	    اگر (retval != تہی)
	    {
		retval[0] = '^';
		j = 1;
		برائے (i = 0; i < len; i++, j++)
		{
		    // Skip backslash.  But why?  At least keep it for custom
		    // expansion.
		    اگر (context != EXPAND_USER_DEFINED
			    && context != EXPAND_USER_LIST
			    && fname[i] == '\\'
			    && ++i == len)
			توڑو;

		    انتخاب (fname[i])
		    {
			حالت '*':   retval[j++] = '.';
				    توڑو;
			حالت '~':   retval[j++] = '\\';
				    توڑو;
			حالت '?':   retval[j] = '.';
				    جاری;
			حالت '.':   اگر (context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    توڑو;
			حالت '\\':  اگر (context == EXPAND_USER_DEFINED
					    || context == EXPAND_USER_LIST)
					retval[j++] = '\\';
				    توڑو;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    ورنہ
    {
	retval = alloc(len + 4);
	اگر (retval != تہی)
	{
	    vim_strncpy(retval, fname, len);

	    // Don't add a star to *, ~, ~user, $var or `cmd`.
	    // * would become **, which walks the whole tree.
	    // ~ would be at the start of the file name, but not the tail.
	    // $ could be anywhere in the tail.
	    // ` could be anywhere in the file name.
	    // When the name ends in '$' don't add a star, remove the '$'.
	    دم = gettail(retval);
	    سرے_اندر_ستارہ = (len > 0 && retval[len - 1] == '*');
#اگر_غیر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	    برائے (i = len - 2; i >= 0; --i)
	    {
		اگر (retval[i] != '\\')
		    توڑو;
		سرے_اندر_ستارہ = !سرے_اندر_ستارہ;
	    }
#اگر_ختم
	    اگر ((*retval != '~' || دم != retval)
		    && !سرے_اندر_ستارہ
		    && vim_strchr(دم, '$') == تہی
		    && vim_strchr(retval, '`') == تہی)
		retval[len++] = '*';
	    ورنہ اگر (len > 0 && retval[len - 1] == '$')
		--len;
	    retval[len] = NUL;
	}
    }
    واپس retval;
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
    خالی
سیٹ_expand_context(expand_T *xp)
{
    cmdline_info_T  *ccline = get_cmdline_info();

    // Handle search commands: '/' or '?'
    اگر ((ccline->cmdfirstc == '/' || ccline->cmdfirstc == '?')
	    && may_expand_نمونہ)
    {
	xp->xp_context = EXPAND_نمونہ_اندر_BUF;
	xp->xp_تلاش_dir = (ccline->cmdfirstc == '/') ? FORWARD : BACKWARD;
	xp->xp_نمونہ = ccline->cmdbuff;
	xp->xp_نمونہ_len = ccline->cmdpos;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	تلاش_پہلا_سطر = 0; // Search entire buffer
#اگر_ختم
	واپس;
    }

    // Only handle ':', '>', or '=' command-lines, or expression input
    اگر (ccline->cmdfirstc != ':'
#اگر_تعریف_شدہ FEAT_EVAL
	    && ccline->cmdfirstc != '>' && ccline->cmdfirstc != '='
	    && !ccline->داخلہ_fn
#اگر_ختم
       )
    {
	xp->xp_context = EXPAND_NOTHING;
	واپس;
    }

    // Fallback to command-line expansion
    سیٹ_cmd_context(xp, ccline->cmdbuff, ccline->cmdlen, ccline->cmdpos, TRUE);
}

/*
 * Sets the index of a built-in or user defined command 'cmd' in eap->cmdidx.
 * For user defined commands, the completion context is set in 'xp' and the
 * completion flags in 'complp'.
 *
 * Returns a pointer to the text after the command or NULL for failure.
 */
    ساکن char_u *
سیٹ_cmd_اشاریہ(char_u *cmd, exarg_T *eap, expand_T *xp, صحیح *complp)
{
    char_u	*p = تہی;
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
    ورنہ
    {
	p = cmd;
	جب (ASCII_ISALPHA(*p) || *p == '*')    // Allow * wild card
	    ++p;
	// A user command may contain digits.
	// Include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	اگر (ASCII_ISUPPER(cmd[0]) || STRNCMP("vim9", cmd, 4) == 0)
	    جب (ASCII_ISALNUM(*p) || *p == '*')
		++p;
	// for python 3.x: ":py3*" commands completion
	اگر (cmd[0] == 'p' && cmd[1] == 'y' && p == cmd + 2 && *p == '3')
	{
	    ++p;
	    جب (ASCII_ISALPHA(*p) || *p == '*')
		++p;
	}
	// check for non-alpha command
	اگر (p == cmd && vim_strchr((char_u *)"@*!=><&~#", *p) != تہی)
	    ++p;
	len = (صحیح)(p - cmd);

	اگر (len == 0)
	{
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	    واپس تہی;
	}

	eap->cmdidx = excmd_get_cmdidx(cmd, len);

	// User defined commands support alphanumeric characters.
	// Also when doing fuzzy expansion for non-shell commands, support
	// alphanumeric characters.
	اگر ((cmd[0] >= 'A' && cmd[0] <= 'Z')
		|| (fuzzy && eap->cmdidx != CMD_bang && *p != NUL))
	    جب (ASCII_ISALNUM(*p) || *p == '*')	// Allow * wild card
		++p;
    }

    // If the cursor is touching the command, and it ends in an alphanumeric
    // character, complete the command name.
    اگر (*p == NUL && ASCII_ISALNUM(p[-1]))
	واپس تہی;

    اگر (eap->cmdidx == CMD_سائز)
    {
	اگر (*cmd == 's' && vim_strchr((char_u *)"cgriI", cmd[1]) != تہی)
	{
	    eap->cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	ورنہ اگر (cmd[0] >= 'A' && cmd[0] <= 'Z')
	{
	    eap->cmd = cmd;
	    p = ڈھونڈو_ucmd(eap, p, تہی, xp, complp);
	    اگر (p == تہی)
		eap->cmdidx = CMD_سائز;	// ambiguous user command
	}
    }
    اگر (eap->cmdidx == CMD_سائز)
    {
	// Not still touching the command and it was an illegal one
	xp->xp_context = EXPAND_UNSUCCESSFUL;
	واپس تہی;
    }

    واپس p;
}

/*
 * Set the completion context for a command argument with wild card characters.
 */
    ساکن خالی
سیٹ_context_for_wildcard_arg(
	exarg_T		*eap,
	char_u		*arg,
	صحیح		usefilter,
	expand_T	*xp,
	صحیح		*complp)
{
    char_u	*p;
    صحیح		c;
    صحیح		اندر_quote = FALSE;
    char_u	*bow = تہی;	// Beginning of word
    صحیح		len = 0;

    // Allow spaces within back-quotes to count as part of the argument
    // being expanded.
    xp->xp_نمونہ = skipwhite(arg);
    p = xp->xp_نمونہ;
    جب (*p != NUL)
    {
	اگر (has_mbyte)
	    c = mb_ptr2char(p);
	ورنہ
	    c = *p;
	اگر (c == '\\' && p[1] != NUL)
	    ++p;
	ورنہ اگر (c == '`')
	{
	    اگر (!اندر_quote)
	    {
		xp->xp_نمونہ = p;
		bow = p + 1;
	    }
	    اندر_quote = !اندر_quote;
	}
	// An argument can contain just about everything, except
	// characters that end the command and white space.
	ورنہ اگر (c == '|' || c == '\n' || c == '"' || (VIM_ISWHITE(c)
		    && (!(eap != تہی && (eap->argt & EX_NOSPC)) || usefilter)
		    ))
	{
	    len = 0;  // avoid getting stuck when space is in 'isfname'
	    جب (*p != NUL)
	    {
		اگر (has_mbyte)
		    c = mb_ptr2char(p);
		ورنہ
		    c = *p;
		اگر (c == '`' || vim_isfilec_or_wc(c))
		    توڑو;
		اگر (has_mbyte)
		    len = (*mb_ptr2len)(p);
		ورنہ
		    len = 1;
		MB_PTR_ADV(p);
	    }
	    اگر (اندر_quote)
		bow = p;
	    ورنہ
		xp->xp_نمونہ = p;
	    p -= len;
	}
	MB_PTR_ADV(p);
    }

    // If we are still inside the quotes, and we passed a space, just
    // expand from there.
    اگر (bow != تہی && اندر_quote)
	xp->xp_نمونہ = bow;
    xp->xp_context = EXPAND_FILES;

    // For a shell command more chars need to be escaped.
    اگر (usefilter
	    || (eap != تہی
		&& (eap->cmdidx == CMD_bang || eap->cmdidx == CMD_terminal))
	    || *complp == EXPAND_SHELLCMDLINE)
    {
#اگر_غیر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	xp->xp_shell = TRUE;
#اگر_ختم
	// When still after the command name expand executables.
	اگر (xp->xp_نمونہ == skipwhite(arg))
	    xp->xp_context = EXPAND_SHELLCMD;
    }

    // Check for environment variable.
    اگر (*xp->xp_نمونہ == '$')
    {
	برائے (p = xp->xp_نمونہ + 1; *p != NUL; ++p)
	    اگر (!vim_isIDc(*p))
		توڑو;
	اگر (*p == NUL)
	{
	    xp->xp_context = EXPAND_ENV_VARS;
	    ++xp->xp_نمونہ;
	    // Avoid that the assignment uses EXPAND_FILES again.
	    اگر (*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST)
		*complp = EXPAND_ENV_VARS;
	}
    }
    // Check for user names.
    اگر (*xp->xp_نمونہ == '~')
    {
	برائے (p = xp->xp_نمونہ + 1; *p != NUL && *p != '/'; ++p)
	    ;
	// Complete ~user only if it partially matches a user name.
	// A full match ~user<Tab> will be replaced by user's home
	// directory i.e. something like ~user<Tab> -> /home/user/
	اگر (*p == NUL && p > xp->xp_نمونہ + 1
		&& مطابقت_user(xp->xp_نمونہ + 1) >= 1)
	{
	    xp->xp_context = EXPAND_USER;
	    ++xp->xp_نمونہ;
	}
    }
}

/*
 * Set the completion context for the "++opt=arg" argument.  Always returns
 * NULL.
 */
    ساکن char_u *
سیٹ_context_اندر_argopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    اگر (p == تہی)
	xp->xp_نمونہ = arg;
    ورنہ
	xp->xp_نمونہ = p + 1;

    xp->xp_context = EXPAND_ARGOPT;
    واپس تہی;
}

#اگر_تعریف_شدہ FEAT_TERMINAL
/*
 * Set the completion context for :terminal's [options].  Always returns NULL.
 */
    ساکن char_u *
سیٹ_context_اندر_terminalopt(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = vim_strchr(arg, '=');
    اگر (p == تہی)
	xp->xp_نمونہ = arg;
    ورنہ
	xp->xp_نمونہ = p + 1;

    xp->xp_context = EXPAND_TERMINALOPT;
    واپس تہی;
}
#اگر_ختم

/*
 * Set the completion context for the :filter command. Returns a pointer to the
 * next command after the :filter command.
 */
    ساکن char_u *
سیٹ_context_اندر_filter_cmd(expand_T *xp, char_u *arg)
{
    اگر (*arg != NUL)
	arg = skip_vimgrep_pat(arg, تہی, تہی);
    اگر (arg == تہی || *arg == NUL)
    {
	xp->xp_context = EXPAND_NOTHING;
	واپس تہی;
    }
    واپس skipwhite(arg);
}

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
/*
 * Set the completion context for the :match command. Returns a pointer to the
 * next command after the :match command.
 */
    ساکن char_u *
سیٹ_context_اندر_مطابقت_cmd(expand_T *xp, char_u *arg)
{
    اگر (*arg == NUL || !سرے_excmd(*arg))
    {
	// also complete "None"
	سیٹ_context_اندر_echohl_cmd(xp, arg);
	arg = skipwhite(skiptowhite(arg));
	اگر (*arg != NUL)
	{
	    xp->xp_context = EXPAND_NOTHING;
	    arg = skip_regexp(arg + 1, *arg, magic_isset());
	}
    }
    واپس ڈھونڈو_nextcmd(arg);
}
#اگر_ختم

/*
 * Returns a pointer to the next command after a :global or a :v command.
 * Returns NULL if there is no next command.
 */
    ساکن char_u *
ڈھونڈو_cmd_after_global_cmd(char_u *arg)
{
    صحیح		delim;

    delim = *arg;	    // get the delimiter
    اگر (delim)
	++arg;		    // skip delimiter if there is one

    جب (arg[0] != NUL && arg[0] != delim)
    {
	اگر (arg[0] == '\\' && arg[1] != NUL)
	    ++arg;
	++arg;
    }
    اگر (arg[0] != NUL)
	واپس arg + 1;

    واپس تہی;
}

/*
 * Returns a pointer to the next command after a :substitute or a :& command.
 * Returns NULL if there is no next command.
 */
    ساکن char_u *
ڈھونڈو_cmd_after_substitute_cmd(char_u *arg)
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
	    جب (arg[0] != NUL && arg[0] != delim)
	    {
		اگر (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    اگر (arg[0] != NUL)	// skip delimiter
		++arg;
	}
    }
    جب (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == تہی)
	++arg;
    اگر (arg[0] != NUL)
	واپس arg;

    واپس تہی;
}

/*
 * Returns a pointer to the next command after a :isearch/:dsearch/:ilist
 * :dlist/:ijump/:psearch/:djump/:isplit/:dsplit command.
 * Returns NULL if there is no next command.
 */
    ساکن char_u *
ڈھونڈو_cmd_after_isearch_cmd(expand_T *xp, char_u *arg)
{
    arg = skipwhite(skipdigits(arg));	    // skip count
    اگر (*arg != '/')
	واپس تہی;

    // Match regexp, not just whole words
    برائے (++arg; *arg && *arg != '/'; arg++)
	اگر (*arg == '\\' && arg[1] != NUL)
	    arg++;
    اگر (*arg)
    {
	arg = skipwhite(arg + 1);

	// Check for trailing illegal characters
	اگر (*arg == NUL || vim_strchr((char_u *)"|\"\n", *arg) == تہی)
	    xp->xp_context = EXPAND_NOTHING;
	ورنہ
	    واپس arg;
    }

    واپس تہی;
}

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Set the completion context for the :unlet command. Always returns NULL.
 */
    ساکن char_u *
سیٹ_context_اندر_unlet_cmd(expand_T *xp, char_u *arg)
{
    جب ((xp->xp_نمونہ = vim_strchr(arg, ' ')) != تہی)
	arg = xp->xp_نمونہ + 1;

    xp->xp_context = EXPAND_USER_VARS;
    xp->xp_نمونہ = arg;

    اگر (*xp->xp_نمونہ == '$')
    {
	xp->xp_context = EXPAND_ENV_VARS;
	++xp->xp_نمونہ;
    }

    واپس تہی;
}
#اگر_ختم

#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
/*
 * Set the completion context for the :language command. Always returns NULL.
 */
    ساکن char_u *
سیٹ_context_اندر_lang_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    p = skiptowhite(arg);
    اگر (*p == NUL)
    {
	xp->xp_context = EXPAND_LANGUAGE;
	xp->xp_نمونہ = arg;
    }
    ورنہ
    {
	اگر ( STRNCMP(arg, "messages", p - arg) == 0
		|| STRNCMP(arg, "ctype", p - arg) == 0
		|| STRNCMP(arg, "time", p - arg) == 0
		|| STRNCMP(arg, "collate", p - arg) == 0)
	{
	    xp->xp_context = EXPAND_LOCALES;
	    xp->xp_نمونہ = skipwhite(p);
	}
	ورنہ
	    xp->xp_context = EXPAND_NOTHING;
    }

    واپس تہی;
}
#اگر_ختم

ساکن شمار
{
    EXP_FILETYPECMD_ALL,	// expand all :filetype values
    EXP_FILETYPECMD_PLUGIN,	// expand plugin on off
    EXP_FILETYPECMD_INDENT,	// expand indent on off
    EXP_FILETYPECMD_ONOFF,	// expand on off
} filetype_expand_what;

#تعریف_کرو EXPAND_FILETYPECMD_PLUGIN 0x01
#تعریف_کرو EXPAND_FILETYPECMD_INDENT 0x02
#تعریف_کرو EXPAND_FILETYPECMD_ONOFF  0x04

#اگر_تعریف_شدہ FEAT_EVAL
ساکن شمار
{
    EXP_BREAKPT_جوڑو,	// expand ":breakadd" sub-commands
    EXP_BREAKPT_DEL,	// expand ":breakdel" sub-commands
    EXP_PROFDEL		// expand ":profdel" sub-commands
} breakpt_expand_what;

/*
 * Set the completion context for the :breakadd command. Always returns NULL.
 */
    ساکن char_u *
سیٹ_context_اندر_breakadd_cmd(expand_T *xp, char_u *arg, cmdidx_T cmdidx)
{
    char_u *p;
    char_u *subcmd_آغاز;

    xp->xp_context = EXPAND_BREAKPOINT;
    xp->xp_نمونہ = arg;

    اگر (cmdidx == CMD_breakadd)
	breakpt_expand_what = EXP_BREAKPT_جوڑو;
    ورنہ اگر (cmdidx == CMD_breakdel)
	breakpt_expand_what = EXP_BREAKPT_DEL;
    ورنہ
	breakpt_expand_what = EXP_PROFDEL;

    p = skipwhite(arg);
    اگر (*p == NUL)
	واپس تہی;
    subcmd_آغاز = p;

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
		واپس تہی;
	    }
	    p = skipwhite(p);
	}
	اگر (STRNCMP("file", subcmd_آغاز, 4) == 0)
	    xp->xp_context = EXPAND_FILES;
	ورنہ
	    xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_نمونہ = p;
    }
    ورنہ اگر (STRNCMP("expr ", p, 5) == 0)
    {
	// :breakadd expr <expression>
	xp->xp_context = EXPAND_عبارت;
	xp->xp_نمونہ = skipwhite(p + 5);
    }

    واپس تہی;
}

    ساکن char_u *
سیٹ_context_اندر_scriptnames_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;

    xp->xp_context = EXPAND_NOTHING;
    xp->xp_نمونہ = تہی;

    p = skipwhite(arg);
    اگر (VIM_ISDIGIT(*p))
	واپس تہی;

    xp->xp_context = EXPAND_SCRIPTNAMES;
    xp->xp_نمونہ = p;

    واپس تہی;
}
#اگر_ختم

/*
 * Set the completion context for the :filetype command. Always returns NULL.
 */
    ساکن char_u *
سیٹ_context_اندر_filetype_cmd(expand_T *xp, char_u *arg)
{
    char_u *p;
    صحیح    val = 0;

    xp->xp_context = EXPAND_FILETYPECMD;
    xp->xp_نمونہ = arg;
    filetype_expand_what = EXP_FILETYPECMD_ALL;

    p = skipwhite(arg);
    اگر (*p == NUL)
	واپس تہی;

    برائے (;;)
    {
	اگر (STRNCMP(p, "plugin", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_PLUGIN;
	    p = skipwhite(p + 6);
	    جاری;
	}
	اگر (STRNCMP(p, "indent", 6) == 0 && VIM_ISWHITE(p[6]))
	{
	    val |= EXPAND_FILETYPECMD_INDENT;
	    p = skipwhite(p + 6);
	    جاری;
	}
	توڑو;
    }

    اگر ((val & EXPAND_FILETYPECMD_PLUGIN) && (val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_ONOFF;
    ورنہ اگر ((val & EXPAND_FILETYPECMD_PLUGIN))
	filetype_expand_what = EXP_FILETYPECMD_INDENT;
    ورنہ اگر ((val & EXPAND_FILETYPECMD_INDENT))
	filetype_expand_what = EXP_FILETYPECMD_PLUGIN;

    xp->xp_نمونہ = p;

    واپس تہی;
}

/*
 * Sets the completion context for commands that involve a search pattern
 * and a line range (e.g., :s, :g, :v).
 */
    ساکن خالی
سیٹ_context_with_نمونہ(expand_T *xp)
{
    صحیح		    skiplen = 0;
    cmdline_info_T  *ccline = get_cmdline_info();
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    صحیح		    dummy, patlen, retval;

    ++emsg_off;
    retval = parse_نمونہ_and_range(&pre_incsearch_pos, &dummy, &skiplen,
	    &patlen);
    --emsg_off;

    // Check if cursor is within search pattern
    اگر (!retval || ccline->cmdpos <= skiplen
	    || ccline->cmdpos > skiplen + patlen)
	واپس;
#اگر_ختم

    xp->xp_نمونہ = ccline->cmdbuff + skiplen;
    xp->xp_نمونہ_len = ccline->cmdpos - skiplen;
    xp->xp_context = EXPAND_نمونہ_اندر_BUF;
    xp->xp_تلاش_dir = FORWARD;
}

/*
 * Set the completion context in 'xp' for command 'cmd' with index 'cmdidx'.
 * The argument to the command is 'arg' and the argument flags is 'argt'.
 * For user-defined commands and for environment variables, 'compl' has the
 * completion type.
 * Returns a pointer to the next command. Returns NULL if there is no next
 * command.
 */
    ساکن char_u *
سیٹ_context_by_cmdname(
	char_u		*cmd,
	cmdidx_T	cmdidx,
	expand_T	*xp,
	char_u		*arg,
	طویل		argt,
	صحیح		compl,
	صحیح		forceit)
{
    char_u  *nextcmd;

    انتخاب (cmdidx)
    {
	حالت CMD_ڈھونڈو:
	حالت CMD_sfind:
	حالت CMD_tabfind:
	    اگر (xp->xp_context == EXPAND_FILES)
		xp->xp_context = *get_findfunc() != NUL ? EXPAND_FINDFUNC
							: EXPAND_FILES_اندر_راستہ;
	    توڑو;
	حالت CMD_cd:
	حالت CMD_chdir:
	حالت CMD_tcd:
	حالت CMD_tchdir:
	حالت CMD_lcd:
	حالت CMD_lchdir:
	    اگر (xp->xp_context == EXPAND_FILES)
		xp->xp_context = EXPAND_DIRS_اندر_CDPATH;
	    توڑو;
	حالت CMD_help:
	    xp->xp_context = EXPAND_HELP;
	    xp->xp_نمونہ = arg;
	    توڑو;

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
	    واپس arg;

	حالت CMD_filter:
	    واپس سیٹ_context_اندر_filter_cmd(xp, arg);

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	حالت CMD_مطابقت:
	    واپس سیٹ_context_اندر_مطابقت_cmd(xp, arg);
#اگر_ختم

	// All completion for the +cmdline_compl feature goes here.

	حالت CMD_command:
	    واپس سیٹ_context_اندر_user_cmd(xp, arg);

	حالت CMD_delcommand:
	    xp->xp_context = EXPAND_USER_COMMANDS;
	    xp->xp_نمونہ = arg;
	    توڑو;

	حالت CMD_global:
	حالت CMD_vglobal:
	    nextcmd = ڈھونڈو_cmd_after_global_cmd(arg);
	    اگر (!nextcmd && may_expand_نمونہ)
		سیٹ_context_with_نمونہ(xp);
	    واپس nextcmd;

	حالت CMD_and:
	حالت CMD_substitute:
	    nextcmd = ڈھونڈو_cmd_after_substitute_cmd(arg);
	    اگر (!nextcmd && may_expand_نمونہ)
		سیٹ_context_with_نمونہ(xp);
	    واپس nextcmd;

	حالت CMD_isearch:
	حالت CMD_dsearch:
	حالت CMD_ilist:
	حالت CMD_dlist:
	حالت CMD_ijump:
	حالت CMD_psearch:
	حالت CMD_djump:
	حالت CMD_isplit:
	حالت CMD_dsplit:
	    واپس ڈھونڈو_cmd_after_isearch_cmd(xp, arg);
	حالت CMD_autocmd:
	    واپس سیٹ_context_اندر_autocmd(xp, arg, FALSE);
	حالت CMD_doautocmd:
	حالت CMD_doautoall:
	    واپس سیٹ_context_اندر_autocmd(xp, arg, TRUE);
	حالت CMD_سیٹ:
	    سیٹ_context_اندر_سیٹ_cmd(xp, arg, 0);
	    توڑو;
	حالت CMD_setglobal:
	    سیٹ_context_اندر_سیٹ_cmd(xp, arg, OPT_GLOBAL);
	    توڑو;
	حالت CMD_setlocal:
	    سیٹ_context_اندر_سیٹ_cmd(xp, arg, OPT_LOCAL);
	    توڑو;
	حالت CMD_لیبل:
	حالت CMD_stag:
	حالت CMD_ptag:
	حالت CMD_ltag:
	حالت CMD_tselect:
	حالت CMD_stselect:
	حالت CMD_ptselect:
	حالت CMD_tjump:
	حالت CMD_stjump:
	حالت CMD_ptjump:
	    اگر (vim_strchr(p_wop, WOP_TAGFILE) != تہی)
		xp->xp_context = EXPAND_لیبل_فہرست_LISTFILES;
	    ورنہ
		xp->xp_context = EXPAND_لیبل_فہرست;
	    xp->xp_نمونہ = arg;
	    توڑو;
	حالت CMD_augroup:
	    xp->xp_context = EXPAND_AUGROUP;
	    xp->xp_نمونہ = arg;
	    توڑو;
#اگر_تعریف_شدہ FEAT_SYN_HL
	حالت CMD_syntax:
	    سیٹ_context_اندر_syntax_cmd(xp, arg);
	    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
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
	    سیٹ_context_for_عبارت(xp, arg, cmdidx);
	    توڑو;

	حالت CMD_unlet:
	    واپس سیٹ_context_اندر_unlet_cmd(xp, arg);
	حالت CMD_function:
	حالت CMD_delfunction:
	    xp->xp_context = EXPAND_USER_FUNC;
	    xp->xp_نمونہ = arg;
	    توڑو;
	حالت CMD_disassemble:
	    سیٹ_context_اندر_disassemble_cmd(xp, arg);
	    توڑو;

	حالت CMD_echohl:
	    سیٹ_context_اندر_echohl_cmd(xp, arg);
	    توڑو;
#اگر_ختم
	حالت CMD_highlight:
	    سیٹ_context_اندر_highlight_cmd(xp, arg);
	    توڑو;
#اگر_تعریف_شدہ FEAT_CSCOPE
	حالت CMD_cscope:
	حالت CMD_lcscope:
	حالت CMD_scscope:
	    سیٹ_context_اندر_cscope_cmd(xp, arg, cmdidx);
	    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
	حالت CMD_sign:
	    سیٹ_context_اندر_sign_cmd(xp, arg);
	    توڑو;
#اگر_ختم
	حالت CMD_bdelete:
	حالت CMD_bwipeout:
	حالت CMD_bunload:
	    جب ((xp->xp_نمونہ = vim_strchr(arg, ' ')) != تہی)
		arg = xp->xp_نمونہ + 1;
	    // FALLTHROUGH
	حالت CMD_بفر:
	حالت CMD_sbuffer:
	حالت CMD_pbuffer:
	حالت CMD_checktime:
	    xp->xp_context = EXPAND_BUFFERS;
	    xp->xp_نمونہ = arg;
	    توڑو;
#اگر_تعریف_شدہ FEAT_DIFF
	حالت CMD_diffget:
	حالت CMD_diffput:
	    // If current buffer is in diff mode, complete buffer names
	    // which are in diff mode, and different than current buffer.
	    xp->xp_context = EXPAND_DIFF_BUFFERS;
	    xp->xp_نمونہ = arg;
	    توڑو;
#اگر_ختم
	حالت CMD_USER:
	حالت CMD_USER_BUF:
	    واپس سیٹ_context_اندر_user_cmdarg(cmd, arg, argt, compl, xp,
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
	    واپس سیٹ_context_اندر_map_cmd(xp, cmd, arg, forceit,
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
	    واپس سیٹ_context_اندر_map_cmd(xp, cmd, arg, forceit,
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
	    xp->xp_نمونہ = arg;
	    توڑو;

	حالت CMD_abbreviate:	حالت CMD_noreabbrev:
	حالت CMD_cabbrev:	حالت CMD_cnoreabbrev:
	حالت CMD_iabbrev:	حالت CMD_inoreabbrev:
	    واپس سیٹ_context_اندر_map_cmd(xp, cmd, arg, forceit,
						      TRUE, FALSE, cmdidx);
	حالت CMD_unabbreviate:
	حالت CMD_cunabbrev:
	حالت CMD_iunabbrev:
	    واپس سیٹ_context_اندر_map_cmd(xp, cmd, arg, forceit,
						       TRUE, TRUE, cmdidx);
#اگر_تعریف_شدہ FEAT_MENU
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
	    واپس سیٹ_context_اندر_menu_cmd(xp, cmd, arg, forceit);
#اگر_ختم

	حالت CMD_colorscheme:
	    xp->xp_context = EXPAND_COLORS;
	    xp->xp_نمونہ = arg;
	    توڑو;

	حالت CMD_compiler:
	    xp->xp_context = EXPAND_COMPILER;
	    xp->xp_نمونہ = arg;
	    توڑو;

	حالت CMD_ownsyntax:
	    xp->xp_context = EXPAND_OWNSYNTAX;
	    xp->xp_نمونہ = arg;
	    توڑو;

	حالت CMD_setfiletype:
	    xp->xp_context = EXPAND_FILETYPE;
	    xp->xp_نمونہ = arg;
	    توڑو;

	حالت CMD_packadd:
	    xp->xp_context = EXPAND_PACKADD;
	    xp->xp_نمونہ = arg;
	    توڑو;

	حالت CMD_runtime:
	    سیٹ_context_اندر_runtime_cmd(xp, arg);
	    توڑو;

#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	حالت CMD_language:
	    واپس سیٹ_context_اندر_lang_cmd(xp, arg);
#اگر_ختم
#اگر defined(FEAT_PROFILE)
	حالت CMD_profile:
	    سیٹ_context_اندر_profile_cmd(xp, arg);
	    توڑو;
#اگر_ختم
	حالت CMD_behave:
	    xp->xp_context = EXPAND_BEHAVE;
	    xp->xp_نمونہ = arg;
	    توڑو;

	حالت CMD_retab:
	    xp->xp_context = EXPAND_RETAB;
	    xp->xp_نمونہ = arg;
	    توڑو;

	حالت CMD_messages:
	    xp->xp_context = EXPAND_MESSAGES;
	    xp->xp_نمونہ = arg;
	    توڑو;

	حالت CMD_history:
	    xp->xp_context = EXPAND_HISTORY;
	    xp->xp_نمونہ = arg;
	    توڑو;
#اگر defined(FEAT_PROFILE)
	حالت CMD_syntime:
	    xp->xp_context = EXPAND_SYNTIME;
	    xp->xp_نمونہ = arg;
	    توڑو;
#اگر_ختم

	حالت CMD_argdelete:
	    جب ((xp->xp_نمونہ = vim_strchr(arg, ' ')) != تہی)
		arg = xp->xp_نمونہ + 1;
	    xp->xp_context = EXPAND_ARGLIST;
	    xp->xp_نمونہ = arg;
	    توڑو;

#اگر_تعریف_شدہ FEAT_EVAL
	حالت CMD_breakadd:
	حالت CMD_profdel:
	حالت CMD_breakdel:
	    واپس سیٹ_context_اندر_breakadd_cmd(xp, arg, cmdidx);

	حالت CMD_scriptnames:
	    واپس سیٹ_context_اندر_scriptnames_cmd(xp, arg);
#اگر_ختم
	حالت CMD_filetype:
	    واپس سیٹ_context_اندر_filetype_cmd(xp, arg);

	طےشدہ:
	    توڑو;
    }
    واپس تہی;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay
 * perfectly compatible with each other, but then the command line syntax
 * probably won't change that much -- webb.
 */
    ساکن char_u *
سیٹ_one_cmd_context(
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
    xp->xp_نمونہ = buff;
    xp->xp_سطر = buff;
    xp->xp_context = EXPAND_COMMANDS;	// Default until we get past command
    ea.argt = 0;

    // 1. skip comment lines and leading space, colons or bars
    برائے (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != تہی; cmd++)
	;
    xp->xp_نمونہ = cmd;

    اگر (*cmd == NUL)
	واپس تہی;
    اگر (*cmd == '"')	    // ignore comment lines
    {
	xp->xp_context = EXPAND_NOTHING;
	واپس تہی;
    }

    // 3. Skip over the range to find the command.
    cmd = skip_range(cmd, TRUE, &xp->xp_context);
    xp->xp_نمونہ = cmd;
    اگر (*cmd == NUL)
	واپس تہی;
    اگر (*cmd == '"')
    {
	xp->xp_context = EXPAND_NOTHING;
	واپس تہی;
    }

    اگر (*cmd == '|' || *cmd == '\n')
	واپس cmd + 1;			// There's another command

    // Get the command index.
    p = سیٹ_cmd_اشاریہ(cmd, &ea, xp, &compl);
    اگر (p == تہی)
	واپس تہی;

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
	جب (*arg != NUL && STRNCMP(arg, "++", 2) == 0)
	{
	    p = arg + 2;
	    جب (*p && !vim_isspace(*p))
		MB_PTR_ADV(p);

	    // Still touching the command after "++"?
	    اگر (*p == NUL)
	    {
		اگر (ea.argt & EX_ARGOPT)
		    واپس سیٹ_context_اندر_argopt(xp, arg + 2);
#اگر_تعریف_شدہ FEAT_TERMINAL
		اگر (ea.cmdidx == CMD_terminal)
		    واپس سیٹ_context_اندر_terminalopt(xp, arg + 2);
#اگر_ختم
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
	ورنہ اگر (*arg == '!' && ea.cmdidx == CMD_write)	// :w !filter
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
	جب (*arg == *cmd)	    // allow any number of '>' or '<'
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
	    واپس p;

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
	جب (*p)
	{
	    اگر (*p == Ctrl_V)
	    {
		اگر (p[1] != NUL)
		    ++p;
	    }
	    ورنہ اگر ( (*p == '"' && !(ea.argt & EX_NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		اگر (*(p - 1) != '\\')
		{
		    اگر (*p == '|' || *p == '\n')
			واپس p + 1;
		    واپس تہی;    // It's a comment
		}
	    }
	    MB_PTR_ADV(p);
	}
    }

    اگر (!(ea.argt & EX_EXTRA) && *arg != NUL
				  && vim_strchr((char_u *)"|\"", *arg) == تہی)
	// no arguments allowed but there is something
	واپس تہی;

    // Find start of last argument (argument just before cursor):
    p = buff;
    xp->xp_نمونہ = p;
    len = (صحیح)STRLEN(buff);
    جب (*p && p < buff + len)
    {
	اگر (*p == ' ' || *p == TAB)
	{
	    // argument starts after a space
	    xp->xp_نمونہ = ++p;
	}
	ورنہ
	{
	    اگر (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }

    اگر (ea.argt & EX_XFILE)
	سیٹ_context_for_wildcard_arg(&ea, arg, usefilter, xp, &compl);

    // 6. Switch on command name.
    واپس سیٹ_context_by_cmdname(cmd, ea.cmdidx, xp, arg, ea.argt, compl,
								forceit);
}

/*
 * Set the completion context in 'xp' for command 'str'
 */
    خالی
سیٹ_cmd_context(
    expand_T	*xp,
    char_u	*str,	    // start of command line
    صحیح		len,	    // length of command line (excl. NUL)
    صحیح		ستون,	    // position of cursor
    صحیح		use_ccline غیر_مستعمل) // use ccline for info
{
#اگر_تعریف_شدہ FEAT_EVAL
    cmdline_info_T	*ccline = get_cmdline_info();
    صحیح			context;
#اگر_ختم
    صحیح		old_char = NUL;
    char_u	*nextcomm;

    // Only save and overwrite the character when it is not the NUL terminator
    // already.  "str" may be a read-only empty string: tv_get_string() falls
    // back to a "" literal for a NULL string or on a type error, and writing
    // at "col" would then crash.
    // This also avoids a UMR warning from Purify.
    اگر (ستون < len)
    {
	old_char = str[ستون];
	str[ستون] = NUL;
    }
    nextcomm = str;

    filetype_expand_what = EXP_FILETYPECMD_ALL;
#اگر_تعریف_شدہ FEAT_EVAL
    breakpt_expand_what = EXP_BREAKPT_جوڑو;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (use_ccline && ccline->cmdfirstc == '=')
    {
	// pass CMD_SIZE because there is no real command
	سیٹ_context_for_عبارت(xp, str, CMD_سائز);
    }
    ورنہ اگر (use_ccline && ccline->داخلہ_fn)
    {
	xp->xp_context = ccline->xp_context;
	xp->xp_نمونہ = ccline->cmdbuff;
	xp->xp_arg = ccline->xp_arg;
	اگر (xp->xp_context == EXPAND_SHELLCMDLINE)
	{
	    context = xp->xp_context;
	    سیٹ_context_for_wildcard_arg(تہی, xp->xp_نمونہ, FALSE, xp,
								     &context);
	}
    }
    ورنہ
#اگر_ختم
	جب (nextcomm != تہی)
	    nextcomm = سیٹ_one_cmd_context(xp, nextcomm);

    // Store the string here so that call_user_expand_func() can get to them
    // easily.
    xp->xp_سطر = str;
    xp->xp_ستون = ستون;

    اگر (ستون < len)
	str[ستون] = old_char;
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
    صحیح		ستون,		// position of cursor
    صحیح		*matchcount,	// return: nr of matches
    char_u	***matches)	// return: array of pointers to matches
{
    char_u	*file_str = تہی;
    صحیح		options = WILD_جوڑو_SLASH|WILD_SILENT;

    اگر (xp->xp_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	واپس EXPAND_UNSUCCESSFUL;  // Something illegal on command line
    }
    اگر (xp->xp_context == EXPAND_NOTHING)
    {
	// Caller can use the character as a normal char instead
	واپس EXPAND_NOTHING;
    }

    // add star to file name, or convert to regexp if not exp. files.
    xp->xp_نمونہ_len = (صحیح)(str + ستون - xp->xp_نمونہ);
    {
	char_u	*pat_src = xp->xp_نمونہ;
	صحیح	pat_len = xp->xp_نمونہ_len;
#اگر defined(FEAT_EVAL)
	char_u	*unesc = unescape_user_completeopt_pat(xp, pat_src, pat_len,
								  &pat_len);
	اگر (unesc != تہی)
	    pat_src = unesc;
#اگر_ختم

	اگر (cmdline_fuzzy_completion_supported(xp))
	    // If fuzzy matching, don't modify the search string
	    file_str = vim_strnsave(pat_src, pat_len);
	ورنہ
	    file_str = addstar(pat_src, pat_len, xp->xp_context);
#اگر defined(FEAT_EVAL)
	vim_free(unesc);
#اگر_ختم
    }
    اگر (file_str == تہی)
	واپس EXPAND_UNSUCCESSFUL;

    اگر (p_wic)
	options += WILD_ICASE;

    // find all files that match the description
    اگر (ExpandFromContext(xp, file_str, matches, matchcount, options) == FAIL)
    {
	*matchcount = 0;
	*matches = تہی;
    }
    vim_free(file_str);

    واپس EXPAND_OK;
}

/*
 * Expand file or directory names.
 * Returns OK or FAIL.
 */
    ساکن صحیح
expand_files_and_dirs(
	expand_T	*xp,
	char_u		*pat,
	char_u		***matches,
	صحیح		*عدد_Matches,
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
	char_u	*pat_انتہا;
	char_u	*p;

	free_pat = TRUE;

	pat_len = STRLEN(pat);
	pat = vim_strnsave(pat, pat_len);
	اگر (pat == تہی)
	    واپس ret;

	pat_انتہا = pat + pat_len;
	برائے (p = pat; *p != NUL; ++p)
	{
	    char_u  *from;

	    اگر (*p != '\\')
		جاری;

	    اگر (xp->xp_backslash & XP_BS_THREE
		&& *(p + 1) == '\\'
		&& *(p + 2) == '\\'
		&& *(p + 3) == ' ')
	    {
		from = p + 3;
		mch_memmove(p, from,
		    (size_t)(pat_انتہا - from) + 1);   // +1 for NUL
		pat_انتہا -= 3;
	    }
	    ورنہ اگر (xp->xp_backslash & XP_BS_ONE
		&& *(p + 1) == ' ')
	    {
		from = p + 1;
		mch_memmove(p, from,
		    (size_t)(pat_انتہا - from) + 1);   // +1 for NUL
		--pat_انتہا;
	    }
	    ورنہ اگر (xp->xp_backslash & XP_BS_COMMA)
	    {
		اگر (*(p + 1) == '\\' && *(p + 2) == ',')
		{
		    from = p + 2;
		    mch_memmove(p, from,
			(size_t)(pat_انتہا - from) + 1);   // +1 for NUL
		    pat_انتہا -= 2;
		}
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
		ورنہ اگر (*(p + 1) == ',')
		{
		    from = p + 1;
		    mch_memmove(p, from,
			(size_t)(pat_انتہا - from) + 1);   // +1 for NUL
		    --pat_انتہا;
		}
#اگر_ختم
	    }
	}
    }

    اگر (xp->xp_context == EXPAND_FINDFUNC)
    {
#اگر_تعریف_شدہ FEAT_EVAL
	ret = expand_findfunc(xp, pat, matches, عدد_Matches);
#اگر_ختم
    }
    ورنہ
    {
	اگر (xp->xp_context == EXPAND_FILES)
	    flags |= EW_FILE;
	ورنہ اگر (xp->xp_context == EXPAND_FILES_اندر_راستہ)
	    flags |= (EW_FILE | EW_راستہ);
	ورنہ اگر (xp->xp_context == EXPAND_DIRS_اندر_CDPATH)
	    flags = (flags | EW_DIR | EW_CDPATH) & ~EW_FILE;
	ورنہ
	    flags = (flags | EW_DIR) & ~EW_FILE;
	اگر (options & WILD_ICASE)
	    flags |= EW_ICASE;

	// Expand wildcards, supporting %:h and the like.
	ret = expand_wildcards_eval(&pat, عدد_Matches, matches, flags);
    }
    اگر (free_pat)
	vim_free(pat);
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    اگر (p_csl[0] != NUL
	    && (options & WILD_IGNORE_COMPLETESLASH) == 0
	    && xp->xp_context != EXPAND_FINDFUNC)
    {
	صحیح j;

	برائے (j = 0; j < *عدد_Matches; ++j)
	{
	    char_u	*ptr = (*matches)[j];

	    جب (*ptr != NUL)
	    {
		اگر (p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		ورنہ اگر (p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#اگر_ختم
    واپس ret;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":behave {mswin,xterm}" command.
 */
    ساکن char_u *
get_behave_arg(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx == 0)
	واپس (char_u *)"mswin";
    اگر (idx == 1)
	واپس (char_u *)"xterm";
    واپس تہی;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":filetype {plugin,indent}" command.
 */
    ساکن char_u *
get_filetypecmd_arg(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx < 0)
	واپس تہی;

    اگر (filetype_expand_what == EXP_FILETYPECMD_ALL && idx < 5)
    {
	حرف	*opts_all[] = {"detect", "indent", "plugin", "on", "off"};
	واپس (char_u *)opts_all[idx];
    }
    اگر (filetype_expand_what == EXP_FILETYPECMD_PLUGIN && idx < 3)
    {
	حرف	*opts_plugin[] = {"plugin", "on", "off"};
	واپس (char_u *)opts_plugin[idx];
    }
    اگر (filetype_expand_what == EXP_FILETYPECMD_INDENT && idx < 3)
    {
	حرف	*opts_indent[] = {"indent", "on", "off"};
	واپس (char_u *)opts_indent[idx];
    }
    اگر (filetype_expand_what == EXP_FILETYPECMD_ONOFF && idx < 2)
    {
	حرف	*opts_onoff[] = {"on", "off"};
	واپس (char_u *)opts_onoff[idx];
    }
    واپس تہی;
}

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":breakadd {expr, file, func, here}" command.
 * ":breakdel {func, file, here}" command.
 */
    ساکن char_u *
get_breakadd_arg(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx >= 0 && idx <= 3)
    {
	حرف	*opts[] = {"expr", "file", "func", "here"};

	// breakadd {expr, file, func, here}
	اگر (breakpt_expand_what == EXP_BREAKPT_جوڑو)
	    واپس (char_u *)opts[idx];
	ورنہ اگر (breakpt_expand_what == EXP_BREAKPT_DEL)
	{
	    // breakdel {func, file, here}
	    اگر (idx <= 2)
		واپس (char_u *)opts[idx + 1];
	}
	ورنہ
	{
	    // profdel {func, file}
	    اگر (idx <= 1)
		واپس (char_u *)opts[idx + 1];
	}
    }
    واپس تہی;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments for the
 * ":scriptnames" command.
 */
    ساکن char_u *
get_scriptnames_arg(expand_T *xp غیر_مستعمل, صحیح idx)
{
    scriptitem_T *si;

    اگر (!SCRIPT_ID_VALID(idx + 1))
	واپس تہی;

    si = SCRIPT_شے(idx + 1);
    home_replace(تہی, si->sn_name, NameBuff, MAXPATHL, TRUE);
    واپس NameBuff;
}
#اگر_ختم

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":retab {-indentonly}" option.
 */
    ساکن char_u *
get_retab_arg(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx == 0)
	واپس (char_u *)"-indentonly";
    واپس تہی;
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":messages {clear}" command.
 */
    ساکن char_u *
get_messages_arg(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx == 0)
	واپس (char_u *)"clear";
    واپس تہی;
}

    ساکن char_u *
get_mapclear_arg(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx == 0)
	واپس (char_u *)"<buffer>";
    واپس تہی;
}

/*
 * Do the expansion based on xp->xp_context and 'rmp'.
 */
    ساکن صحیح
ExpandOther(
	char_u		*pat,
	expand_T	*xp,
	regmatch_T	*rmp,
	char_u		***matches,
	صحیح		*عدد_Matches)
{
    ساکن ساخت expgen
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
#اگر_تعریف_شدہ FEAT_EVAL
	{EXPAND_USER_VARS, get_user_var_name, FALSE, TRUE},
	{EXPAND_FUNCTIONS, get_function_name, FALSE, TRUE},
	{EXPAND_USER_FUNC, get_user_func_name, FALSE, TRUE},
	{EXPAND_DISASSEMBLE, get_disassemble_دلیل, FALSE, TRUE},
	{EXPAND_عبارت, get_اظہاریہ_name, FALSE, TRUE},
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MENU
	{EXPAND_MENUS, get_menu_name, FALSE, TRUE},
	{EXPAND_MENUNAMES, get_menu_نام, FALSE, TRUE},
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
	{EXPAND_SYNTAX, get_syntax_name, TRUE, TRUE},
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROFILE
	{EXPAND_SYNTIME, get_syntime_arg, TRUE, TRUE},
#اگر_ختم
	{EXPAND_HIGHLIGHT, get_highlight_name, TRUE, TRUE},
	{EXPAND_EVENTS, get_event_name, TRUE, FALSE},
	{EXPAND_AUGROUP, get_augroup_name, TRUE, FALSE},
#اگر_تعریف_شدہ FEAT_CSCOPE
	{EXPAND_CSCOPE, get_cscope_name, TRUE, TRUE},
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
	{EXPAND_SIGN, get_sign_name, TRUE, TRUE},
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROFILE
	{EXPAND_PROFILE, get_profile_name, TRUE, TRUE},
#اگر_ختم
#اگر defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	{EXPAND_LANGUAGE, get_lang_arg, TRUE, FALSE},
	{EXPAND_LOCALES, get_locales, TRUE, FALSE},
#اگر_ختم
	{EXPAND_ENV_VARS, get_env_name, TRUE, TRUE},
	{EXPAND_USER, get_users, TRUE, FALSE},
	{EXPAND_ARGLIST, get_arglist_name, TRUE, FALSE},
#اگر_تعریف_شدہ FEAT_EVAL
	{EXPAND_BREAKPOINT, get_breakadd_arg, TRUE, TRUE},
	{EXPAND_SCRIPTNAMES, get_scriptnames_arg, TRUE, FALSE},
#اگر_ختم
	{EXPAND_RETAB, get_retab_arg, TRUE, TRUE},
    };
    صحیح	i;
    صحیح ret = FAIL;

    // Find a context in the table and call the ExpandGeneric() with the
    // right function to do the expansion.
    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(tab); ++i)
    {
	اگر (xp->xp_context == tab[i].context)
	{
	    اگر (tab[i].ic)
		rmp->rm_ic = TRUE;
	    ret = ExpandGeneric(pat, xp, rmp, matches, عدد_Matches,
						tab[i].func, tab[i].escaped);
	    توڑو;
	}
    }

    واپس ret;
}

/*
 * Map wild expand options to flags for expand_wildcards()
 */
    ساکن صحیح
map_wildopts_مقصد_ewflags(صحیح options)
{
    صحیح		flags;

    flags = EW_DIR;	// include directories
    اگر (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;
    اگر (options & WILD_جوڑو_SLASH)
	flags |= EW_ADDSLASH;
    اگر (options & WILD_KEEP_ALL)
	flags |= EW_KEEPALL;
    اگر (options & WILD_SILENT)
	flags |= EW_SILENT;
    اگر (options & WILD_NOERROR)
	flags |= EW_NOERROR;
    اگر (options & WILD_ALLLINKS)
	flags |= EW_ALLLINKS;

    واپس flags;
}

/*
 * Do the expansion based on xp->xp_context and "pat".
 */
    ساکن صحیح
ExpandFromContext(
    expand_T	*xp,
    char_u	*pat,
    char_u	***matches,
    صحیح		*عدد_Matches,
    صحیح		options)  // WILD_ flags
{
    regmatch_T	regmatch;
    صحیح		ret;
    صحیح		flags;
    char_u	*tofree = تہی;
    صحیح		fuzzy = cmdline_fuzzy_complete(pat)
				     && cmdline_fuzzy_completion_supported(xp);

    flags = map_wildopts_مقصد_ewflags(options);

    اگر (xp->xp_context == EXPAND_FILES
	    || xp->xp_context == EXPAND_DIRECTORIES
	    || xp->xp_context == EXPAND_FILES_اندر_راستہ
	    || xp->xp_context == EXPAND_FINDFUNC
	    || xp->xp_context == EXPAND_DIRS_اندر_CDPATH)
	واپس expand_files_and_dirs(xp, pat, matches, عدد_Matches, flags,
								options);

    *matches = (char_u **)"";
    *عدد_Matches = 0;
    اگر (xp->xp_context == EXPAND_HELP)
    {
	// With an empty argument we would get all the help tags, which is
	// very slow.  Get matches for "help" instead.
	اگر (ڈھونڈو_help_لیبل_فہرست(*pat == NUL ? (char_u *)"help" : pat,
					عدد_Matches, matches, FALSE) == OK)
	{
#اگر_تعریف_شدہ FEAT_MULTI_LANG
	    cleanup_help_لیبل_فہرست(*عدد_Matches, *matches);
#اگر_ختم
	    واپس OK;
	}
	واپس FAIL;
    }

    اگر (xp->xp_context == EXPAND_SHELLCMD)
	واپس expand_shellcmd(pat, matches, عدد_Matches, flags);
    اگر (xp->xp_context == EXPAND_OLD_SETTING)
	واپس ExpandOldSetting(عدد_Matches, matches);
    اگر (xp->xp_context == EXPAND_BUFFERS)
	واپس ExpandBufnames(pat, عدد_Matches, matches, options);
#اگر_تعریف_شدہ FEAT_DIFF
    اگر (xp->xp_context == EXPAND_DIFF_BUFFERS)
	واپس ExpandBufnames(pat, عدد_Matches, matches,
						options | BUF_DIFF_FILTER);
#اگر_ختم
    اگر (xp->xp_context == EXPAND_لیبل_فہرست
	    || xp->xp_context == EXPAND_لیبل_فہرست_LISTFILES)
	واپس expand_لیبل_فہرست(xp->xp_context == EXPAND_لیبل_فہرست, pat, عدد_Matches,
								matches);
    اگر (xp->xp_context == EXPAND_COLORS)
    {
	حرف *directories[] = {"colors", تہی};
	واپس ExpandRTDir(pat, DIP_آغاز + DIP_OPT, عدد_Matches, matches,
								directories);
    }
    اگر (xp->xp_context == EXPAND_COMPILER)
    {
	حرف *directories[] = {"compiler", تہی};
	واپس ExpandRTDir(pat, 0, عدد_Matches, matches, directories);
    }
    اگر (xp->xp_context == EXPAND_OWNSYNTAX)
    {
	حرف *directories[] = {"syntax", تہی};
	واپس ExpandRTDir(pat, 0, عدد_Matches, matches, directories);
    }
    اگر (xp->xp_context == EXPAND_FILETYPE)
    {
	حرف *directories[] = {"syntax", "indent", "ftplugin", تہی};
	واپس ExpandRTDir(pat, 0, عدد_Matches, matches, directories);
    }
#اگر_تعریف_شدہ FEAT_KEYMAP
    اگر (xp->xp_context == EXPAND_KEYMAP)
    {
	حرف *directories[] = {"keymap", تہی};
	واپس ExpandRTDir(pat, 0, عدد_Matches, matches, directories);
    }
#اگر_ختم
#اگر defined(FEAT_EVAL)
    اگر (xp->xp_context == EXPAND_USER_LIST)
	واپس ExpandUserList(xp, matches, عدد_Matches);
#اگر_ختم
    اگر (xp->xp_context == EXPAND_PACKADD)
	واپس Expand_Pack_جوڑو_Dir(pat, عدد_Matches, matches);
    اگر (xp->xp_context == EXPAND_RUNTIME)
	واپس expand_runtime_cmd(pat, عدد_Matches, matches);
    اگر (xp->xp_context == EXPAND_نمونہ_اندر_BUF)
	واپس expand_نمونہ_اندر_buf(pat, xp->xp_تلاش_dir,
		matches, عدد_Matches);

    // When expanding a function name starting with s:, match the <SNR>nr_
    // prefix.
    اگر ((xp->xp_context == EXPAND_USER_FUNC
				       || xp->xp_context == EXPAND_DISASSEMBLE)
	    && STRNCMP(pat, "^s:", 3) == 0)
    {
	صحیح len = (صحیح)STRLEN(pat) + 20;

	tofree = alloc(len);
	اگر (tofree == تہی)
	    واپس FAIL;
	vim_snprintf((حرف *)tofree, len, "^<SNR>\\d\\+_%s", pat + 3);
	pat = tofree;
    }

    اگر (!fuzzy)
    {
	regmatch.regprog = vim_regcomp(pat, magic_isset() ? RE_MAGIC : 0);
	اگر (regmatch.regprog == تہی)
	{
	    vim_free(tofree);
	    واپس FAIL;
	}
	// set ignore-case according to p_ic, p_scs and pat
	regmatch.rm_ic = ignorecase(pat);
    }

    اگر (xp->xp_context == EXPAND_SETTINGS
	    || xp->xp_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(xp, &regmatch, pat, عدد_Matches, matches, fuzzy);
    ورنہ اگر (xp->xp_context == EXPAND_STRING_SETTING)
	ret = ExpandStringSetting(xp, &regmatch, عدد_Matches, matches);
    ورنہ اگر (xp->xp_context == EXPAND_SETTING_تفریق)
	ret = Expand_Setting_تفریق(xp, &regmatch, عدد_Matches, matches);
    ورنہ اگر (xp->xp_context == EXPAND_MAPPINGS)
	ret = ExpandMappings(pat, &regmatch, عدد_Matches, matches);
    ورنہ اگر (xp->xp_context == EXPAND_ARGOPT)
	ret = expand_argopt(pat, xp, &regmatch, matches, عدد_Matches);
    ورنہ اگر (xp->xp_context == EXPAND_HIGHLIGHT_گروہ)
	ret = expand_highlight_گروہ(pat, xp, &regmatch, matches, عدد_Matches);
#اگر defined(FEAT_TERMINAL)
    ورنہ اگر (xp->xp_context == EXPAND_TERMINALOPT)
	ret = expand_terminal_opt(pat, xp, &regmatch, matches, عدد_Matches);
#اگر_ختم
#اگر defined(FEAT_EVAL)
    ورنہ اگر (xp->xp_context == EXPAND_USER_DEFINED)
	ret = ExpandUserDefined(pat, xp, &regmatch, matches, عدد_Matches);
#اگر_ختم
    ورنہ
	ret = ExpandOther(pat, xp, &regmatch, matches, عدد_Matches);

    اگر (!fuzzy)
	vim_regfree(regmatch.regprog);
    vim_free(tofree);

    واپس ret;
}

    صحیح
ExpandGeneric(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    صحیح		*عدد_Matches,
    char_u	*(*func)(expand_T *, صحیح),
					  // returns a string from the list
    صحیح		escaped)
{
    واپس ExpandGenericExt(
	pat, xp, regmatch, matches, عدد_Matches, func, escaped, 0);
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
    صحیح		*عدد_Matches,
    char_u	*(*func)(expand_T *, صحیح),
					  // returns a string from the list
    صحیح		escaped,
    صحیح		ترتیب_دو_آغاز_Idx)
{
    صحیح		i;
    garray_T	ga;
    char_u	*str;
    fuzmatch_str_T	*fuzmatch = تہی;
    صحیح		score = 0;
    صحیح		fuzzy;
    صحیح		مطابقت;
    صحیح		ترتیب_دو_matches = FALSE;
    صحیح		funcsort = FALSE;
    صحیح		ترتیب_دو_آغاز_مطابقت_Idx = -1;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = تہی;
    *عدد_Matches = 0;

    اگر (!fuzzy)
	ga_init2(&ga, حجم(حرف *), 30);
    ورنہ
	ga_init2(&ga, حجم(fuzmatch_str_T), 30);

    برائے (i = 0; ; ++i)
    {
	str = (*func)(xp, i);
	اگر (str == تہی)	    // end of list
	    توڑو;
	اگر (*str == NUL)	    // skip empty strings
	    جاری;

	اگر (xp->xp_نمونہ[0] != NUL)
	{
	    اگر (!fuzzy)
		مطابقت = vim_regexec(regmatch, str, (colnr_T)0);
	    ورنہ
	    {
		score = fuzzy_مطابقت_str(str, pat);
		مطابقت = (score != FUZZY_SCORE_NONE);
	    }
	}
	ورنہ
	    مطابقت = TRUE;

	اگر (!مطابقت)
	    جاری;

	اگر (escaped)
	    str = vim_strsave_escaped(str, (char_u *)" \t\\.");
	ورنہ
	    str = vim_strsave(str);
	اگر (str == تہی)
	{
	    اگر (!fuzzy)
	    {
		ga_clear_لڑیاں(&ga);
		واپس FAIL;
	    }
	    fuzmatch_str_free(ga.ga_ڈیٹا, ga.ga_len);
	    واپس FAIL;
	}

	اگر (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(str);
	    توڑو;
	}

	اگر (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_ڈیٹا)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = str;
	    fuzmatch->score = score;
	}
	ورنہ
	    ((char_u **)ga.ga_ڈیٹا)[ga.ga_len] = str;

#اگر_تعریف_شدہ FEAT_MENU
	اگر (func == get_menu_نام)
	{
	    // test for separator added by get_menu_names()
	    str += STRLEN(str) - 1;
	    اگر (*str == '\001')
		*str = '.';
	}
#اگر_ختم

	اگر (ترتیب_دو_آغاز_Idx >= 0 && i >= ترتیب_دو_آغاز_Idx && ترتیب_دو_آغاز_مطابقت_Idx == -1)
	{
	    // Found first item to start sorting from. This is usually 0.
	    ترتیب_دو_آغاز_مطابقت_Idx = ga.ga_len;
	}

	++ga.ga_len;
    }

    اگر (ga.ga_len == 0)
	واپس OK;

    // sort the matches when using regular expression matching and sorting
    // applies to the completion context. Menus and scriptnames should be kept
    // in the specified order.
    اگر (!fuzzy && xp->xp_context != EXPAND_MENUNAMES
					&& xp->xp_context != EXPAND_STRING_SETTING
					&& xp->xp_context != EXPAND_MENUS
					&& xp->xp_context != EXPAND_SCRIPTNAMES
					&& xp->xp_context != EXPAND_ARGOPT
					&& xp->xp_context != EXPAND_TERMINALOPT)
	ترتیب_دو_matches = TRUE;

    // <SNR> functions should be sorted to the end.
    اگر (xp->xp_context == EXPAND_عبارت
	    || xp->xp_context == EXPAND_FUNCTIONS
	    || xp->xp_context == EXPAND_USER_FUNC
	    || xp->xp_context == EXPAND_DISASSEMBLE)
	funcsort = TRUE;

    // Sort the matches.
    اگر (ترتیب_دو_matches && ترتیب_دو_آغاز_مطابقت_Idx != -1)
    {
	اگر (funcsort)
	    // <SNR> functions should be sorted to the end.
	    qsort((خالی *)ga.ga_ڈیٹا, (size_t)ga.ga_len, حجم(char_u *),
							   ترتیب_دو_func_compare);
	ورنہ
	    ترتیب_دو_لڑیاں((char_u **)ga.ga_ڈیٹا + ترتیب_دو_آغاز_مطابقت_Idx, ga.ga_len - ترتیب_دو_آغاز_مطابقت_Idx);
    }

    اگر (!fuzzy)
    {
	*matches = ga.ga_ڈیٹا;
	*عدد_Matches = ga.ga_len;
    }
    ورنہ
    {
	اگر (fuzzymatches_مقصد_strmatches(ga.ga_ڈیٹا, matches, ga.ga_len,
							funcsort) == FAIL)
	    واپس FAIL;
	*عدد_Matches = ga.ga_len;
    }

#اگر defined(FEAT_SYN_HL)
    // Reset the variables used for special highlight names expansion, so that
    // they don't show up when getting normal highlight names by ID.
    reset_expand_highlight();
#اگر_ختم

    واپس OK;
}

/*
 * Expand shell command matches in one directory of $PATH.
 */
    ساکن خالی
expand_shellcmd_onedir(
	char_u		*pathed_نمونہ,    // fully pathed pattern
	size_t		pathlen,	    // length of the path portion of pathed_pattern
					    // (0 if no path).
	char_u		***matches,
	صحیح		*عدد_Matches,
	صحیح		flags,
	hashtab_T	*ht,
	garray_T	*gap)
{
    // Expand matches in one directory of $PATH.
    اگر (expand_wildcards(1, &pathed_نمونہ, عدد_Matches, matches, flags) != OK)
	واپس;

    اگر (ga_grow(gap, *عدد_Matches) == FAIL)
    {
	FreeWild(*عدد_Matches, *matches);
	واپس;
    }

    برائے (صحیح i = 0; i < *عدد_Matches; ++i)
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
	    اگر (HASHITEM_خالی_خانے(hi))
	    {
		// Remove the path that was prepended.
		mch_memmove(name, name + pathlen,
		    (size_t)(namelen - pathlen) + 1);   // +1 for NUL
		((char_u **)gap->ga_ڈیٹا)[gap->ga_len++] = name;
		hash_جوڑو_شے(ht, hi, name, hash);
		name = تہی;
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
    ساکن صحیح
expand_shellcmd(
    char_u	*filepat,	// pattern to match with command names
    char_u	***matches,	// return: array with matches
    صحیح		*عدد_Matches,	// return: number of matches
    صحیح		flagsarg)	// EW_ flags
{
    char_u	*pat;
    size_t	patlen;
    char_u	*راستہ = تہی;
    صحیح		mustfree = FALSE;
    garray_T    ga;
    char_u	*buf;
    char_u	*s, *e;
    صحیح		flags = flagsarg;
    صحیح		did_curdir = FALSE;
    hashtab_T	ملا_ht;

    buf = alloc(MAXPATHL);
    اگر (buf == تہی)
	واپس FAIL;

    // for ":set path=" and ":set tags=" halve backslashes for escaped space
    patlen = STRLEN(filepat);
    pat = vim_strnsave(filepat, patlen);
    اگر (pat == تہی)
    {
	vim_free(buf);
	واپس FAIL;
    }

    // Replace "\ " with " ".
    e = pat + patlen;
    برائے (s = pat; *s != NUL; ++s)
    {
	char_u  *p;

	اگر (*s != '\\')
	    جاری;

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
	راستہ = (char_u *)".";
    ورنہ
    {
	// For an absolute name we don't use $PATH.
	اگر (!mch_is_بھرا_Name(pat))
	    راستہ = vim_getenv((char_u *)"PATH", &mustfree);
	اگر (راستہ == تہی)
	    راستہ = (char_u *)"";
    }

    // Go over all directories in $PATH.  Expand matches in that directory and
    // collect them in "ga".  When "." is not in $PATH also expand for the
    // current directory, to find "subdir/cmd".
    ga_init2(&ga, حجم(حرف *), 10);
    hash_init(&ملا_ht);
    برائے (s = راستہ; ; s = e)
    {
	size_t	pathlen;	// length of the path portion of buf
				// (including trailing slash).
	size_t	seplen;

	اگر (*s == NUL)
	{
	    اگر (did_curdir)
		توڑو;

	    // Find directories in the current directory, path is empty.
	    did_curdir = TRUE;
	    flags |= EW_DIR;

	    e = s;
	    pathlen = 0;
	    seplen = 0;
	}
	ورنہ
	{
#اگر defined(MSWIN)
	    e = vim_strchr(s, ';');
#ورنہ
	    e = vim_strchr(s, ':');
#اگر_ختم
	    اگر (e == تہی)
		e = s + STRLEN(s);

	    pathlen = (size_t)(e - s);
	    اگر (STRNCMP(s, ".", pathlen) == 0)
	    {
		did_curdir = TRUE;
		flags |= EW_DIR;
	    }
	    ورنہ
		// Do not match directories inside a $PATH item.
		flags &= ~EW_DIR;

	    seplen = !after_pathsep(s, e) ? حجم(PATHSEP) : 0;
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

	    expand_shellcmd_onedir(buf, pathlen, matches, عدد_Matches, flags,
				    &ملا_ht, &ga);
	}

	اگر (*e != NUL)
	    ++e;
    }
    *matches = ga.ga_ڈیٹا;
    *عدد_Matches = ga.ga_len;

    vim_free(buf);
    vim_free(pat);
    اگر (mustfree)
	vim_free(راستہ);
    hash_clear(&ملا_ht);
    واپس OK;
}

#اگر defined(FEAT_EVAL)
/*
 * Call "user_expand_func()" to invoke a user defined Vim script function and
 * return the result (either a string, a List or NULL).
 */
    ساکن خالی *
call_user_expand_func(
    خالی	*(*user_expand_func)(char_u *, صحیح, typval_T *),
    expand_T	*xp)
{
    cmdline_info_T	*ccline = get_cmdline_info();
    صحیح		keep = 0;
    typval_T	دلائل[4];
    sctx_T	save_current_sctx = current_sctx;
    char_u	*pat = تہی;
    خالی	*ret;

    اگر (xp->xp_arg == تہی || xp->xp_arg[0] == '\0' || xp->xp_سطر == تہی)
	واپس تہی;

    اگر (ccline->cmdbuff != تہی)
    {
	keep = ccline->cmdbuff[ccline->cmdlen];
	ccline->cmdbuff[ccline->cmdlen] = 0;
    }

    {
	صحیح unesc_len;
	pat = unescape_user_completeopt_pat(xp, xp->xp_نمونہ,
						  xp->xp_نمونہ_len, &unesc_len);
	اگر (pat == تہی)
	    pat = vim_strnsave(xp->xp_نمونہ, xp->xp_نمونہ_len);
    }

    دلائل[0].v_type = VAR_STRING;
    دلائل[0].vval.v_string = pat;
    دلائل[1].v_type = VAR_STRING;
    دلائل[1].vval.v_string = xp->xp_سطر;
    دلائل[2].v_type = VAR_نمبر;
    دلائل[2].vval.v_نمبر = xp->xp_ستون;
    دلائل[3].v_type = VAR_UNKNOWN;

    current_sctx = xp->xp_script_ctx;

    ret = user_expand_func(xp->xp_arg, 3, دلائل);

    current_sctx = save_current_sctx;
    اگر (ccline->cmdbuff != تہی)
	ccline->cmdbuff[ccline->cmdlen] = keep;

    vim_free(pat);
    واپس ret;
}

/*
 * Expand names with a function defined by the user (EXPAND_USER_DEFINED and
 * EXPAND_USER_LIST).
 */
    ساکن صحیح
ExpandUserDefined(
    char_u	*pat,
    expand_T	*xp,
    regmatch_T	*regmatch,
    char_u	***matches,
    صحیح		*عدد_Matches)
{
    char_u	*retstr;
    char_u	*s;
    char_u	*e;
    صحیح		keep;
    garray_T	ga;
    صحیح		fuzzy;
    صحیح		مطابقت;
    صحیح		score = 0;

    fuzzy = cmdline_fuzzy_complete(pat);
    *matches = تہی;
    *عدد_Matches = 0;

    retstr = call_user_expand_func(call_func_retstr, xp);
    اگر (retstr == تہی)
	واپس FAIL;

    اگر (!fuzzy)
	ga_init2(&ga, حجم(حرف *), 3);
    ورنہ
	ga_init2(&ga, حجم(fuzmatch_str_T), 3);

    برائے (s = retstr; *s != NUL; s = e)
    {
	e = vim_strchr(s, '\n');
	اگر (e == تہی)
	    e = s + STRLEN(s);
	keep = *e;
	*e = NUL;

	اگر (xp->xp_نمونہ[0] != NUL)
	{
	    اگر (!fuzzy)
		مطابقت = vim_regexec(regmatch, s, (colnr_T)0);
	    ورنہ
	    {
		score = fuzzy_مطابقت_str(s, pat);
		مطابقت = (score != FUZZY_SCORE_NONE);
	    }
	}
	ورنہ
	    مطابقت = TRUE;		// match everything

	*e = keep;

	اگر (مطابقت)
	{
	    char_u  *p = vim_strnsave(s, (size_t)(e - s));
	    اگر (p == تہی)
		توڑو;

	    اگر (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		توڑو;
	    }

	    اگر (!fuzzy)
		((char_u **)ga.ga_ڈیٹا)[ga.ga_len] = p;
	    ورنہ
	    {
		fuzmatch_str_T  *fuzmatch =
				&((fuzmatch_str_T  *)ga.ga_ڈیٹا)[ga.ga_len];
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
	واپس OK;

    اگر (!fuzzy)
    {
	*matches = ga.ga_ڈیٹا;
	*عدد_Matches = ga.ga_len;
    }
    ورنہ
    {
	اگر (fuzzymatches_مقصد_strmatches(ga.ga_ڈیٹا, matches, ga.ga_len,
								FALSE) == FAIL)
	    واپس FAIL;
	*عدد_Matches = ga.ga_len;
    }
    واپس OK;
}

    خالی
expand_process_user_list(
    list_T	*retlist,
    char_u	***matches,
    صحیح		*عدد_Matches,
    expand_T	*xp)
{
    listitem_T	*li;
    garray_T	ga;
    garray_T	ga_abbr;
    garray_T	ga_قسم;
    garray_T	ga_menu;
    garray_T	ga_info;
    صحیح		have_extra = FALSE;
    صحیح		i;

    ga_init2(&ga, حجم(حرف *), 3);
    ga_init2(&ga_abbr, حجم(حرف *), 3);
    ga_init2(&ga_قسم, حجم(حرف *), 3);
    ga_init2(&ga_menu, حجم(حرف *), 3);
    ga_init2(&ga_info, حجم(حرف *), 3);
    // Loop over the items in the list.
    FOR_ALL_LIST_اشیاء(retlist, li)
    {
	typval_T *tv = &li->li_tv;
	char_u	*p = تہی;
	char_u	*abbr = تہی;
	char_u	*قسم = تہی;
	char_u	*menu = تہی;
	char_u	*info = تہی;

	اگر (tv->v_type == VAR_STRING)
	{
	    اگر (tv->vval.v_string == تہی)
		جاری;  // Skip NULL strings
	    p = vim_strsave(tv->vval.v_string);
	}
	ورنہ اگر (tv->v_type == VAR_DICT && tv->vval.v_dict != تہی)
	{
	    dict_T	*d = tv->vval.v_dict;
	    char_u	*لفظ = dict_get_string(d, "word", FALSE);

	    اگر (لفظ == تہی)
		جاری;  // "word" is required
	    p = vim_strsave(لفظ);
	    abbr = dict_get_string(d, "abbr", TRUE);
	    قسم = dict_get_string(d, "kind", TRUE);
	    menu = dict_get_string(d, "menu", TRUE);
	    info = dict_get_string(d, "info", TRUE);
	    اگر (abbr != تہی || قسم != تہی || menu != تہی || info != تہی)
		have_extra = TRUE;
	}
	ورنہ
	    جاری;  // Skip other types

	اگر (p == تہی
		|| ga_grow(&ga, 1) == FAIL
		|| ga_grow(&ga_abbr, 1) == FAIL
		|| ga_grow(&ga_قسم, 1) == FAIL
		|| ga_grow(&ga_menu, 1) == FAIL
		|| ga_grow(&ga_info, 1) == FAIL)
	{
	    vim_free(p);
	    vim_free(abbr);
	    vim_free(قسم);
	    vim_free(menu);
	    vim_free(info);
	    توڑو;
	}

	((char_u **)ga.ga_ڈیٹا)[ga.ga_len++] = p;
	((char_u **)ga_abbr.ga_ڈیٹا)[ga_abbr.ga_len++] = abbr;
	((char_u **)ga_قسم.ga_ڈیٹا)[ga_قسم.ga_len++] = قسم;
	((char_u **)ga_menu.ga_ڈیٹا)[ga_menu.ga_len++] = menu;
	((char_u **)ga_info.ga_ڈیٹا)[ga_info.ga_len++] = info;
    }

    *matches = ga.ga_ڈیٹا;
    *عدد_Matches = ga.ga_len;
    اگر (have_extra && ga.ga_len > 0)
    {
	xp->xp_files_abbr = (char_u **)ga_abbr.ga_ڈیٹا;
	xp->xp_files_قسم = (char_u **)ga_قسم.ga_ڈیٹا;
	xp->xp_files_menu = (char_u **)ga_menu.ga_ڈیٹا;
	xp->xp_files_info = (char_u **)ga_info.ga_ڈیٹا;
    }
    ورنہ
    {
	// No extra info collected; free the placeholder NULL entries.
	برائے (i = 0; i < ga_abbr.ga_len; i++)
	    vim_free(((char_u **)ga_abbr.ga_ڈیٹا)[i]);
	vim_free(ga_abbr.ga_ڈیٹا);
	برائے (i = 0; i < ga_قسم.ga_len; i++)
	    vim_free(((char_u **)ga_قسم.ga_ڈیٹا)[i]);
	vim_free(ga_قسم.ga_ڈیٹا);
	برائے (i = 0; i < ga_menu.ga_len; i++)
	    vim_free(((char_u **)ga_menu.ga_ڈیٹا)[i]);
	vim_free(ga_menu.ga_ڈیٹا);
	برائے (i = 0; i < ga_info.ga_len; i++)
	    vim_free(((char_u **)ga_info.ga_ڈیٹا)[i]);
	vim_free(ga_info.ga_ڈیٹا);
    }
}

/*
 * Expand names with a list returned by a function defined by the user.
 */
    ساکن صحیح
ExpandUserList(
    expand_T	*xp,
    char_u	***matches,
    صحیح		*عدد_Matches)
{
    list_T      *retlist;

    *matches = تہی;
    *عدد_Matches = 0;
    retlist = call_user_expand_func(call_func_retlist, xp);
    اگر (retlist == تہی)
	واپس FAIL;

    expand_process_user_list(retlist, matches, عدد_Matches, xp);
    list_unref(retlist);
    واپس OK;
}
#اگر_ختم

/*
 * Expand "file" for all comma-separated directories in "path".
 * Adds the matches to "ga".  Caller must init "ga".
 * If "dirs" is TRUE only expand directory names.
 */
    خالی
globpath(
    char_u	*راستہ,
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
    اگر (buf == تہی)
	واپس;

    ExpandInit(&xpc);
    xpc.xp_context = dirs ? EXPAND_DIRECTORIES : EXPAND_FILES;

    filelen = STRLEN(file);

#اگر defined(MSWIN)
    // Using the platform's path separator (\) makes vim incorrectly
    // treat it as an escape character, use '/' instead.
# define TMP_PATHSEPSTR "/"
#ورنہ
# define TMP_PATHSEPSTR PATHSEPSTR
#اگر_ختم

    // Loop over all entries in {path}.
    جب (*راستہ != NUL)
    {
	// Copy one item of the path to buf[] and concatenate the file name.
	pathlen = (size_t)copy_option_part(&راستہ, buf, MAXPATHL, ",");
	seplen = (*buf != NUL && !after_pathsep(buf, buf + pathlen))
	    ? STRLEN_LITERAL(TMP_PATHSEPSTR)
	    : 0;

	اگر (pathlen + seplen + filelen + 1 <= MAXPATHL)
	{
	    صحیح	    عدد_p;
	    char_u  **p;

	    اگر (seplen > 0)
	    {
		STRCPY(buf + pathlen, TMP_PATHSEPSTR);
		pathlen += seplen;
	    }
	    STRCPY(buf + pathlen, file);

	    اگر (ExpandFromContext(&xpc, buf, &p, &عدد_p,
			     WILD_SILENT|expand_options) != FAIL && عدد_p > 0)
	    {
		ExpandEscape(&xpc, buf, عدد_p, p, WILD_SILENT|expand_options);

		اگر (ga_grow(ga, عدد_p) == OK)
		{
		    // take over the pointers and put them in "ga"
		    برائے (صحیح i = 0; i < عدد_p; ++i)
		    {
			((char_u **)ga->ga_ڈیٹا)[ga->ga_len] = p[i];
			++ga->ga_len;
		    }
		}
		ورنہ
		{
		    FreeWild(عدد_p, p);
		    p = تہی;
		}
		vim_free(p);
	    }
	}
    }

    vim_free(buf);
}
#تعریف_منسوخ TMP_PATHSEPSTR

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

    اگر (cmdline_pum_فعال())
    {
	// When the popup menu is used for cmdline completion:
	//   Up	  : go to the previous item in the menu
	//   Down : go to the next item in the menu
	//   Left : go to the parent directory
	//   Right: list the files in the selected directory
	انتخاب (c)
	{
	    حالت K_اوپر:	  c = K_بایاں; توڑو;
	    حالت K_نیچے_کو:  c = K_دایاں; توڑو;
	    حالت K_بایاں:  c = K_اوپر; توڑو;
	    حالت K_دایاں: c = K_نیچے_کو; توڑو;
	    طےشدہ:	  توڑو;
	}
    }

    اگر (cmdline_pum_فعال() || did_wild_list || wild_menu_showing)
    {
	اگر (c == K_بایاں)
	    c = Ctrl_P;
	ورنہ اگر (c == K_دایاں)
	    c = Ctrl_N;
    }

    // Hitting CR after "emenu Name.": complete submenu
    اگر (xp->xp_context == EXPAND_MENUNAMES
	    && cclp->cmdpos > 1
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.'
	    && cclp->cmdbuff[cclp->cmdpos - 2] != '\\'
	    && (c == '\n' || c == '\r' || c == K_KENTER))
	c = K_نیچے_کو;

    واپس c;
}

/*
 * Delete characters on the command line, from "from" to the current
 * position.
 */
    ساکن خالی
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
    ساکن صحیح
wildmenu_process_کلید_menunames(cmdline_info_T *cclp, صحیح کلید, expand_T *xp)
{
    صحیح		i;
    صحیح		j;

    // Hitting <Down> after "emenu Name.": complete submenu
    اگر (کلید == K_نیچے_کو && cclp->cmdpos > 0
	    && cclp->cmdbuff[cclp->cmdpos - 1] == '.')
    {
	کلید = p_wc;
	کلید_Typed = TRUE;  // in case the key was mapped
    }
    ورنہ اگر (کلید == K_اوپر)
    {
	// Hitting <Up>: Remove one submenu name in front of the
	// cursor
	صحیح ملا = FALSE;

	j = (صحیح)(xp->xp_نمونہ - cclp->cmdbuff);
	i = 0;
	جب (--j > 0)
	{
	    // check for start of menu name
	    اگر (cclp->cmdbuff[j] == ' '
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		i = j + 1;
		توڑو;
	    }
	    // check for start of submenu name
	    اگر (cclp->cmdbuff[j] == '.'
		    && cclp->cmdbuff[j - 1] != '\\')
	    {
		اگر (ملا)
		{
		    i = j + 1;
		    توڑو;
		}
		ورنہ
		    ملا = TRUE;
	    }
	}
	اگر (i > 0)
	    cmdline_del(cclp, i);
	کلید = p_wc;
	کلید_Typed = TRUE;  // in case the key was mapped
	xp->xp_context = EXPAND_NOTHING;
    }

    واپس کلید;
}

/*
 * Handle a key pressed when the wild menu for file names (EXPAND_FILES) or
 * directory names (EXPAND_DIRECTORIES) or shell command names
 * (EXPAND_SHELLCMD) is displayed.
 */
    ساکن صحیح
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

    اگر (کلید == K_نیچے_کو
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
    ورنہ اگر (STRNCMP(xp->xp_نمونہ, upseg + 1, 3) == 0 && کلید == K_نیچے_کو)
    {
	// If in a direct ancestor, strip off one ../ to go down
	صحیح ملا = FALSE;

	j = cclp->cmdpos;
	i = (صحیح)(xp->xp_نمونہ - cclp->cmdbuff);
	جب (--j > i)
	{
	    اگر (has_mbyte)
		j -= (*mb_سر_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    اگر (vim_ispathsep(cclp->cmdbuff[j]))
	    {
		ملا = TRUE;
		توڑو;
	    }
	}
	اگر (ملا
		&& cclp->cmdbuff[j - 1] == '.'
		&& cclp->cmdbuff[j - 2] == '.'
		&& (vim_ispathsep(cclp->cmdbuff[j - 3]) || j == i + 2))
	{
	    cmdline_del(cclp, j - 2);
	    کلید = p_wc;
	    کلید_Typed = TRUE;  // in case the key was mapped
	}
    }
    ورنہ اگر (کلید == K_اوپر)
    {
	// go up a directory
	صحیح ملا = FALSE;

	j = cclp->cmdpos - 1;
	i = (صحیح)(xp->xp_نمونہ - cclp->cmdbuff);
	جب (--j > i)
	{
	    اگر (has_mbyte)
		j -= (*mb_سر_off)(cclp->cmdbuff, cclp->cmdbuff + j);
	    اگر (vim_ispathsep(cclp->cmdbuff[j])
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
		    && vim_strchr((char_u *)" *?[{`$%#",
			cclp->cmdbuff[j + 1]) == تہی
#اگر_ختم
	       )
	    {
		اگر (ملا)
		{
		    i = j + 1;
		    توڑو;
		}
		ورنہ
		    ملا = TRUE;
	    }
	}

	اگر (!ملا)
	    j = i;
	ورنہ اگر (STRNCMP(cclp->cmdbuff + j, upseg, 4) == 0)
	    j += 4;
	ورنہ اگر (STRNCMP(cclp->cmdbuff + j, upseg + 1, 3) == 0
		&& j == i)
	    j += 3;
	ورنہ
	    j = 0;
	اگر (j > 0)
	{
	    // TODO this is only for DOS/UNIX systems - need to put in
	    // machine-specific stuff here and in upseg init
	    cmdline_del(cclp, j);
	    put_on_cmdline(upseg + 1, 3, FALSE);
	}
	ورنہ اگر (cclp->cmdpos > i)
	    cmdline_del(cclp, i);

	// Now complete in the new directory. Set KeyTyped in case the
	// Up key came from a mapping.
	کلید = p_wc;
	کلید_Typed = TRUE;
    }

    واپس کلید;
}

/*
 * Handle a key pressed when the wild menu is displayed
 */
    صحیح
wildmenu_process_کلید(cmdline_info_T *cclp, صحیح کلید, expand_T *xp)
{
    اگر (xp->xp_context == EXPAND_MENUNAMES)
	واپس wildmenu_process_کلید_menunames(cclp, کلید, xp);
    ورنہ اگر ((xp->xp_context == EXPAND_FILES
		|| xp->xp_context == EXPAND_DIRECTORIES
		|| xp->xp_context == EXPAND_SHELLCMD))
	واپس wildmenu_process_کلید_filenames(cclp, کلید, xp);

    واپس کلید;
}

/*
 * Free expanded names when finished walking through the matches
 */
    خالی
wildmenu_cleanup(cmdline_info_T *cclp غیر_مستعمل)
{
    صحیح skt = کلید_Typed;

    اگر (!p_wmnu || wild_menu_showing == 0)
	واپس;

#اگر_تعریف_شدہ FEAT_EVAL
    صحیح save_RedrawingDisabled = RedrawingDisabled;
    اگر (cclp->داخلہ_fn)
	RedrawingDisabled = 0;
#اگر_ختم

#اگر defined(FEAT_تلاش_EXTRA)
    // Clear highlighting applied during wildmenu activity
    سیٹ_no_hlsearch(TRUE);
#اگر_ختم

    اگر (wild_menu_showing == WM_SCROLLED)
    {
	// Entered command line, move it up
	cmdline_صف--;
	redrawcmd();
    }
    ورنہ اگر (save_p_ls != -1)
    {
	// restore 'laststatus' and 'winminheight'
	p_ls = save_p_ls;
	p_wmh = save_p_wmh;
	آخری_حالت(FALSE);
	update_screen(UPD_VALID);	// redraw the screen NOW
	redrawcmd();
	save_p_ls = -1;
    }
    ورنہ
    {
	win_redraw_آخری_حالت(topframe);
	redraw_statuslines();
    }
    کلید_Typed = skt;
    wild_menu_showing = 0;
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (cclp->داخلہ_fn)
	RedrawingDisabled = save_RedrawingDisabled;
#اگر_ختم
}

#اگر defined(FEAT_EVAL)
/*
 * "getcompletion()" function
 */
    خالی
f_getcompletion(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    char_u	*type;
    expand_T	xpc;
    صحیح		filtered = FALSE;
    صحیح		options = WILD_SILENT | WILD_USE_NL | WILD_جوڑو_SLASH
					| WILD_NO_BEEP | WILD_HOME_REPLACE;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL))
	واپس;

    pat = tv_get_string(&argvars[0]);
    اگر (check_for_string_arg(argvars, 1) == FAIL)
	واپس;
    type = tv_get_string(&argvars[1]);

    اگر (argvars[2].v_type != VAR_UNKNOWN)
	filtered = tv_get_bool_chk(&argvars[2], تہی);

    اگر (p_wic)
	options |= WILD_ICASE;

    // For filtered results, 'wildignore' is used
    اگر (!filtered)
	options |= WILD_KEEP_ALL;

    ExpandInit(&xpc);
    اگر (STRCMP(type, "cmdline") == 0)
    {
	صحیح cmdline_len = (صحیح)STRLEN(pat);
	سیٹ_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
	xpc.xp_نمونہ_len = (صحیح)STRLEN(xpc.xp_نمونہ);
	xpc.xp_ستون = cmdline_len;
    }
    ورنہ
    {
	char_u	*نمونہ_آغاز;

	xpc.xp_نمونہ = نمونہ_آغاز = pat;
	xpc.xp_نمونہ_len = (صحیح)STRLEN(xpc.xp_نمونہ);
	xpc.xp_سطر = pat;

	xpc.xp_context = cmdcomplete_str_مقصد_type(type);
	انتخاب (xpc.xp_context)
	{
	حالت EXPAND_NOTHING:
	    semsg(_(e_invalid_دلیل_str), type);
	    واپس;

	حالت EXPAND_USER_DEFINED:
	    // Must be "custom,funcname" pattern
	    اگر (STRNCMP(type, "custom,", 7) != 0)
	    {
		semsg(_(e_invalid_دلیل_str), type);
		واپس;
	    }

	    xpc.xp_arg = type + 7;
	    توڑو;

	حالت EXPAND_USER_LIST:
	    // Must be "customlist,funcname" pattern
	    اگر (STRNCMP(type, "customlist,", 11) != 0)
	    {
		semsg(_(e_invalid_دلیل_str), type);
		واپس;
	    }

	    xpc.xp_arg = type + 11;
	    توڑو;

# اگر defined(FEAT_MENU)
	حالت EXPAND_MENUS:
	    سیٹ_context_اندر_menu_cmd(&xpc, (char_u *)"menu", xpc.xp_نمونہ, FALSE);
	    xpc.xp_نمونہ_len -= (صحیح)(xpc.xp_نمونہ - نمونہ_آغاز);
	    توڑو;
# endif

# ifdef FEAT_CSCOPE
	حالت EXPAND_CSCOPE:
	    سیٹ_context_اندر_cscope_cmd(&xpc, xpc.xp_نمونہ, CMD_cscope);
	    xpc.xp_نمونہ_len -= (صحیح)(xpc.xp_نمونہ - نمونہ_آغاز);
	    توڑو;
# endif

# ifdef FEAT_SIGNS
	حالت EXPAND_SIGN:
	    سیٹ_context_اندر_sign_cmd(&xpc, xpc.xp_نمونہ);
	    xpc.xp_نمونہ_len -= (صحیح)(xpc.xp_نمونہ - نمونہ_آغاز);
	    توڑو;
# endif

	حالت EXPAND_RUNTIME:
	    سیٹ_context_اندر_runtime_cmd(&xpc, xpc.xp_نمونہ);
	    xpc.xp_نمونہ_len -= (صحیح)(xpc.xp_نمونہ - نمونہ_آغاز);
	    توڑو;

	حالت EXPAND_SHELLCMDLINE:
	{
	    صحیح context = EXPAND_SHELLCMDLINE;
	    سیٹ_context_for_wildcard_arg(تہی, xpc.xp_نمونہ, FALSE, &xpc,
					 &context);
	    xpc.xp_نمونہ_len -= (صحیح)(xpc.xp_نمونہ - نمونہ_آغاز);
	    توڑو;
	}

	حالت EXPAND_FILETYPECMD:
	    filetype_expand_what = EXP_FILETYPECMD_ALL;
	    توڑو;

	حالت EXPAND_BREAKPOINT:
	    breakpt_expand_what = EXP_BREAKPT_جوڑو;
	    توڑو;

	طےشدہ:
	    توڑو;
	}
    }

    {
	char_u	*pat_src = xpc.xp_نمونہ;
	صحیح	pat_len = xpc.xp_نمونہ_len;
	char_u	*unesc = unescape_user_completeopt_pat(&xpc, pat_src, pat_len,
								  &pat_len);
	اگر (unesc != تہی)
	    pat_src = unesc;

	اگر (cmdline_fuzzy_completion_supported(&xpc))
	    // when fuzzy matching, don't modify the search string
	    pat = vim_strnsave(pat_src, pat_len);
	ورنہ
	    pat = addstar(pat_src, pat_len, xpc.xp_context);
	vim_free(unesc);
    }

    اگر (rettv_list_alloc(rettv) == OK && pat != تہی)
    {
	صحیح	i;

	ExpandOne(&xpc, pat, تہی, options, WILD_ALL_KEEP);

	برائے (i = 0; i < xpc.xp_numfiles; i++)
	    list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);
    }
    vim_free(pat);
    ExpandCleanup(&xpc);
}

/*
 * "getcompletiontype()" function
 */
    خالی
f_getcompletiontype(typval_T *argvars, typval_T *rettv)
{
    char_u	*pat;
    expand_T	xpc;
    صحیح		cmdline_len;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    pat = tv_get_string(&argvars[0]);
    ExpandInit(&xpc);

    cmdline_len = (صحیح)STRLEN(pat);
    سیٹ_cmd_context(&xpc, pat, cmdline_len, cmdline_len, FALSE);
    rettv->vval.v_string = cmdcomplete_type_مقصد_str(xpc.xp_context, xpc.xp_arg);

    ExpandCleanup(&xpc);
}

/*
 * "cmdcomplete_info()" function
 */
    خالی
f_cmdcomplete_info(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    cmdline_info_T  *ccline = get_cmdline_info();
    dict_T	    *retdict;
    list_T	    *li;
    صحیح		    idx;
    صحیح		    ret = OK;

    اگر (rettv_dict_alloc(rettv) == FAIL || ccline == تہی
	    || ccline->xpc == تہی || ccline->xpc->xp_files == تہی)
	واپس;
    retdict = rettv->vval.v_dict;
    ret = dict_جوڑو_string_len(retdict, "cmdline_orig",
	cmdline_orig.string, (صحیح)cmdline_orig.لمبائی);
    اگر (ret == OK)
	ret = dict_جوڑو_نمبر(retdict, "pum_visible", pum_visible());
    اگر (ret == OK)
	ret = dict_جوڑو_نمبر(retdict, "selected", ccline->xpc->xp_selected);
    اگر (ret == OK)
    {
	li = list_alloc();
	اگر (li == تہی)
	    واپس;
	ret = dict_جوڑو_list(retdict, "matches", li);
	اگر (ret == FAIL)
	    list_unref(li);
	برائے (idx = 0; ret == OK && idx < ccline->xpc->xp_numfiles; idx++)
	    list_append_string(li, ccline->xpc->xp_files[idx], -1);
    }
}
#اگر_ختم // FEAT_EVAL

/*
 * Copy a substring from the current buffer (curbuf), spanning from the given
 * 'start' position to the word boundary after 'end' position.
 * The copied string is stored in '*match', and the actual end position of the
 * matched text is returned in '*match_end'.
 */
    ساکن صحیح
copy_substring_from_pos(pos_T *آغاز, pos_T *انتہا, char_u **مطابقت,
	pos_T *مطابقت_انتہا)
{
    char_u	*لفظ_انتہا;
    char_u	*سطر, *آغاز_سطر, *انتہا_سطر;
    صحیح		segment_len;
    linenr_T	lnum;
    garray_T	ga;
    صحیح		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != تہی;

    اگر (آغاز->lnum > انتہا->lnum
	    || (آغاز->lnum == انتہا->lnum && آغاز->ستون >= انتہا->ستون))
	واپس FAIL; // invalid range

    // Use a growable string (ga)
    ga_init2(&ga, 1, 128);

    // Append start line from start->col to end
    آغاز_سطر = ml_get(آغاز->lnum);
    char_u  *آغاز_ptr = آغاز_سطر + آغاز->ستون;
    صحیح	    is_single_سطر = آغاز->lnum == انتہا->lnum;

    segment_len = is_single_سطر ? (انتہا->ستون - آغاز->ستون)
			: (صحیح)(ml_get_len(آغاز->lnum) - آغاز->ستون);
    اگر (ga_grow(&ga, segment_len + 2) != OK)
	جاؤ fail;

    ga_concat_len(&ga, آغاز_ptr, segment_len);
    اگر (!is_single_سطر)
    {
	اگر (exacttext)
	    GA_CONCAT_LITERAL(&ga, "\\n");
	ورنہ
	    ga_append(&ga, '\n');
    }

    // Append full lines between start and end
    اگر (!is_single_سطر)
    {
	برائے (lnum = آغاز->lnum + 1; lnum < انتہا->lnum; lnum++)
	{
	    صحیح  linelen;

	    سطر = ml_get(lnum);
	    linelen = (صحیح)ml_get_len(lnum);
	    اگر (ga_grow(&ga, linelen + 2) != OK)
		جاؤ fail;
	    ga_concat_len(&ga, سطر, linelen);
	    اگر (exacttext)
		GA_CONCAT_LITERAL(&ga, "\\n");
	    ورنہ
		ga_append(&ga, '\n');
	}
    }

    // Append partial end line (up to word end)
    انتہا_سطر = ml_get(انتہا->lnum);
    لفظ_انتہا = ڈھونڈو_لفظ_انتہا(انتہا_سطر + انتہا->ستون);
    segment_len = (صحیح)(لفظ_انتہا - انتہا_سطر);
    اگر (ga_grow(&ga, segment_len) != OK)
	جاؤ fail;
    ga_concat_len(&ga, انتہا_سطر + (is_single_سطر ? انتہا->ستون : 0),
	    segment_len - (is_single_سطر ? انتہا->ستون : 0));

    // Null-terminate
    اگر (ga_grow(&ga, 1) != OK)
	جاؤ fail;
    ga_append(&ga, NUL);

    *مطابقت = (char_u *)ga.ga_ڈیٹا;
    مطابقت_انتہا->lnum = انتہا->lnum;
    مطابقت_انتہا->ستون = segment_len;

    واپس OK;

fail:
    ga_clear(&ga);
    واپس FAIL;
}

/*
 * Returns TRUE if the given string `str` matches the regex pattern `pat`.
 * Honors the 'ignorecase' (p_ic) and 'smartcase' (p_scs) settings to determine
 * case sensitivity.
 */
    ساکن صحیح
is_regex_مطابقت(char_u *pat, char_u *str)
{
    اگر (STRCMP(pat, str) == 0)
	واپس TRUE;

    regmatch_T	regmatch;
    صحیح		نتیجہ;

    ++emsg_off;
    ++پیغام_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    --emsg_off;
    --پیغام_silent;

    اگر (regmatch.regprog == تہی)
	واپس FALSE;
    regmatch.rm_ic = p_ic;
    اگر (p_ic && p_scs)
	regmatch.rm_ic = !pat_has_uppercase(pat);

    ++emsg_off;
    ++پیغام_silent;
    نتیجہ = vim_regexec_nl(&regmatch, str, (colnr_T)0);
    --emsg_off;
    --پیغام_silent;

    vim_regfree(regmatch.regprog);
    واپس نتیجہ;
}

/*
 * Constructs a new match string by appending text from the buffer (starting at
 * end_match_pos) to the given pattern `pat`. The result is a concatenation of
 * `pat` and the word following end_match_pos.
 * If 'lowercase' is TRUE, the appended text is converted to lowercase before
 * being combined. Returns the newly allocated match string, or NULL on failure.
 */
    ساکن char_u *
concat_نمونہ_with_بفر_مطابقت(
	char_u *pat,
	صحیح pat_len,
	pos_T *انتہا_مطابقت_pos,
	صحیح lowercase غیر_مستعمل)
{
    char_u  *سطر = ml_get(انتہا_مطابقت_pos->lnum);
    char_u  *لفظ_انتہا = ڈھونڈو_لفظ_انتہا(سطر + انتہا_مطابقت_pos->ستون);
    صحیح	    مطابقت_len = (صحیح)(لفظ_انتہا - (سطر + انتہا_مطابقت_pos->ستون));
    char_u  *مطابقت = alloc(مطابقت_len + pat_len + 1);  // +1 for NUL

    اگر (مطابقت == تہی)
	واپس تہی;
    mch_memmove(مطابقت, pat, pat_len);
    اگر (مطابقت_len > 0)
    {
#اگر defined(FEAT_EVAL) || defined(FEAT_SPELL)
	اگر (lowercase)
	{
	    char_u  *mword = vim_strnsave(سطر + انتہا_مطابقت_pos->ستون,
		    مطابقت_len);
	    اگر (mword == تہی)
		جاؤ cleanup;
	    char_u  *زیریں = strlow_save(mword);
	    vim_free(mword);
	    اگر (زیریں == تہی)
		جاؤ cleanup;
	    mch_memmove(مطابقت + pat_len, زیریں, مطابقت_len);
	    vim_free(زیریں);
	}
	ورنہ
#اگر_ختم
	    mch_memmove(مطابقت + pat_len, سطر + انتہا_مطابقت_pos->ستون, مطابقت_len);
    }
    مطابقت[pat_len + مطابقت_len] = NUL;
    واپس مطابقت;

#اگر defined(FEAT_EVAL) || defined(FEAT_SPELL)
cleanup:
    vim_free(مطابقت);
    واپس تہی;
#اگر_ختم
}

/*
 * Search for strings matching "pat" in the specified range and return them.
 * Returns OK on success, FAIL otherwise.
 */
    ساکن صحیح
expand_نمونہ_اندر_buf(
    char_u	*pat,		    // pattern to match
    صحیح		dir,		    // direction: FORWARD or BACKWARD
    char_u	***matches,	    // return: array with matched strings
    صحیح		*عدد_Matches)	    // return: number of matches
{
    pos_T	cur_مطابقت_pos, پچھلا_مطابقت_pos, انتہا_مطابقت_pos, لفظ_انتہا_pos;
    garray_T	ga;
    صحیح		ملا_new_مطابقت;
    صحیح		looped_around = FALSE;
    صحیح		pat_len;
    صحیح		has_range = FALSE;
    صحیح		compl_started = FALSE;
    صحیح		تلاش_flags;
    char_u	*مطابقت, *بھرا_مطابقت;
    صحیح		exacttext = vim_strchr(p_wop, WOP_EXACTTEXT) != تہی;

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    has_range = تلاش_پہلا_سطر != 0;
#اگر_ختم

    *matches = تہی;
    *عدد_Matches = 0;

    اگر (pat == تہی || *pat == NUL)
	واپس FAIL;

    pat_len = (صحیح)STRLEN(pat);
    CLEAR_میدان(cur_مطابقت_pos);
    CLEAR_میدان(پچھلا_مطابقت_pos);
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    اگر (has_range)
	cur_مطابقت_pos.lnum = تلاش_پہلا_سطر;
    ورنہ
#اگر_ختم
	cur_مطابقت_pos = pre_incsearch_pos;

    تلاش_flags = تلاش_OPT | تلاش_NOOF | تلاش_PEEK | تلاش_NFMSG
	| (has_range ? تلاش_آغاز : 0);

    ga_init2(&ga, حجم(char_u *), 10); // Use growable array of char_u*

    برائے (;;)
    {
	++emsg_off;
	++پیغام_silent;
	ملا_new_مطابقت = searchit(تہی, curbuf, &cur_مطابقت_pos,
		&انتہا_مطابقت_pos, dir, pat, pat_len, 1L,
		تلاش_flags, RE_آخری, تہی);
	--پیغام_silent;
	--emsg_off;

	اگر (ملا_new_مطابقت == FAIL)
	    توڑو;

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	// If in range mode, check if match is within the range
	اگر (has_range && (cur_مطابقت_pos.lnum < تلاش_پہلا_سطر
		    || cur_مطابقت_pos.lnum > تلاش_آخری_سطر))
		توڑو;
#اگر_ختم

	اگر (compl_started)
	{
	    // If we've looped back to an earlier match, stop
	    اگر ((dir == FORWARD && LTOREQ_POS(cur_مطابقت_pos, پچھلا_مطابقت_pos)) ||
		(dir == BACKWARD && LTOREQ_POS(پچھلا_مطابقت_pos, cur_مطابقت_pos)))
	    {
		اگر (looped_around)
		    توڑو;
		ورنہ
		    looped_around = TRUE;
	    }
	}

	compl_started = TRUE;
	پچھلا_مطابقت_pos = cur_مطابقت_pos;

	// Abort if user typed a character or interrupted
	اگر (char_avail() || got_int)
	{
	    اگر (got_int)
	    {
		(خالی)vpeekc();  // Remove <C-C> from input stream
		got_int = FALSE; // Don't abandon the command line
	    }
	    جاؤ cleanup;
	}

	// searchit() can return line number +1 past the last line when
	// searching for "foo\n" if "foo" is at end of buffer.
	اگر (انتہا_مطابقت_pos.lnum > curbuf->b_ml.ml_سطر_گنتی)
	{
	    cur_مطابقت_pos.lnum = 1;
	    cur_مطابقت_pos.ستون = 0;
	    cur_مطابقت_pos.coladd = 0;
	    جاری;
	}

	// Extract the matching text prepended to completed word
	اگر (copy_substring_from_pos(&cur_مطابقت_pos, &انتہا_مطابقت_pos, &بھرا_مطابقت,
		    &لفظ_انتہا_pos) == FAIL)
	    توڑو;

	اگر (exacttext)
	    مطابقت = بھرا_مطابقت;
	ورنہ
	{
	    // Construct a new match from completed word appended to pattern itself
	    مطابقت = concat_نمونہ_with_بفر_مطابقت(pat, pat_len, &انتہا_مطابقت_pos,
		    FALSE);

	    // The regex pattern may include '\C' or '\c'. First, try matching the
	    // buffer word as-is. If it doesn't match, try again with the lowercase
	    // version of the word to handle smartcase behavior.
	    اگر (مطابقت == تہی || !is_regex_مطابقت(مطابقت, بھرا_مطابقت))
	    {
		vim_free(مطابقت);
		مطابقت = concat_نمونہ_with_بفر_مطابقت(pat, pat_len,
			&انتہا_مطابقت_pos, TRUE);
		اگر (مطابقت == تہی || !is_regex_مطابقت(مطابقت, بھرا_مطابقت))
		{
		    vim_free(مطابقت);
		    vim_free(بھرا_مطابقت);
		    جاری;
		}
	    }
	    vim_free(بھرا_مطابقت);
	}

	// Include this match if it is not a duplicate
	برائے (صحیح i = 0; i < ga.ga_len; ++i)
	{
	    اگر (STRCMP(مطابقت, ((char_u **)ga.ga_ڈیٹا)[i]) == 0)
	    {
		VIM_CLEAR(مطابقت);
		توڑو;
	    }
	}
	اگر (مطابقت != تہی)
	{
	    اگر (ga_grow(&ga, 1) == FAIL)
	    {
		VIM_CLEAR(مطابقت);
		جاؤ cleanup;
	    }
	    ((char_u **)ga.ga_ڈیٹا)[ga.ga_len++] = مطابقت;
	    اگر (ga.ga_len > لیبل_MANY)
		توڑو;
	}
	اگر (has_range)
	    cur_مطابقت_pos = لفظ_انتہا_pos;
    }

    *matches = (char_u **)ga.ga_ڈیٹا;
    *عدد_Matches = ga.ga_len;
    واپس OK;

cleanup:
    ga_clear_لڑیاں(&ga);
    واپس FAIL;
}
