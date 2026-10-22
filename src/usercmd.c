/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * usercmd.c: User defined command support
 */

#অন্তর্ভাৱ "vim.h"

প্রকারনির্দেশ সংরচনা ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    long	uc_def;		// The default value for a range/count
    পূর্ণাঙ্ক		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    পূর্ণাঙ্ক		uc_flags;	// some UC_ flags
    পূর্ণাঙ্ক		uc_compl_opt;	// completion options (UCC_ flags)
#ifdef FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#endif
} ucmd_T;

// List of all user commands.
স্থির garray_T ucmds = {0, 0, পরিমাণ(ucmd_T), 4, রিক্ত};

// When non-zero it is not allowed to add or remove user commands
স্থির পূর্ণাঙ্ক ucmd_locked = 0;

#define USER_CMD(i) (&((ucmd_T *)(ucmds.ga_data))[i])
#define USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_data))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
স্থির keyvalue_T command_complete_tab[] =
{
    KEYVALUE_ENTRY(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_ENTRY(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_ENTRY(EXPAND_BEHAVE, "behave"),
#যদি defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_BREAKPOINT, "breakpoint"),
#endif
    KEYVALUE_ENTRY(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ENTRY(EXPAND_COLORS, "color"),
    KEYVALUE_ENTRY(EXPAND_COMMANDS, "command"),
    KEYVALUE_ENTRY(EXPAND_COMPILER, "compiler"),
#যদি defined(FEAT_CSCOPE)
    KEYVALUE_ENTRY(EXPAND_CSCOPE, "cscope"),
#endif
#যদি defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_USER_DEFINED, "custom"),
    KEYVALUE_ENTRY(EXPAND_USER_LIST, "customlist"),
#endif
    KEYVALUE_ENTRY(EXPAND_DIFF_BUFFERS, "diff_buffer"),
    KEYVALUE_ENTRY(EXPAND_DIRECTORIES, "dir"),
    KEYVALUE_ENTRY(EXPAND_DIRS_IN_CDPATH, "dir_in_path"),
    KEYVALUE_ENTRY(EXPAND_ENV_VARS, "environment"),
    KEYVALUE_ENTRY(EXPAND_EVENTS, "event"),
    KEYVALUE_ENTRY(EXPAND_EXPRESSION, "expression"),
    KEYVALUE_ENTRY(EXPAND_FILES, "file"),
    KEYVALUE_ENTRY(EXPAND_FILES_IN_PATH, "file_in_path"),
    KEYVALUE_ENTRY(EXPAND_FILETYPE, "filetype"),
    KEYVALUE_ENTRY(EXPAND_FILETYPECMD, "filetypecmd"),
    KEYVALUE_ENTRY(EXPAND_FUNCTIONS, "function"),
    KEYVALUE_ENTRY(EXPAND_HELP, "help"),
    KEYVALUE_ENTRY(EXPAND_HIGHLIGHT, "highlight"),
    KEYVALUE_ENTRY(EXPAND_HISTORY, "history"),
#যদি defined(FEAT_KEYMAP)
    KEYVALUE_ENTRY(EXPAND_KEYMAP, "keymap"),
#endif
#যদি defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    KEYVALUE_ENTRY(EXPAND_LOCALES, "locale"),
#endif
    KEYVALUE_ENTRY(EXPAND_MAPCLEAR, "mapclear"),
    KEYVALUE_ENTRY(EXPAND_MAPPINGS, "mapping"),
    KEYVALUE_ENTRY(EXPAND_MENUS, "menu"),
    KEYVALUE_ENTRY(EXPAND_MESSAGES, "messages"),
    KEYVALUE_ENTRY(EXPAND_SETTINGS, "option"),
    KEYVALUE_ENTRY(EXPAND_PACKADD, "packadd"),
    KEYVALUE_ENTRY(EXPAND_RETAB, "retab"),
    KEYVALUE_ENTRY(EXPAND_RUNTIME, "runtime"),
#যদি defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_SCRIPTNAMES, "scriptnames"),
#endif
    KEYVALUE_ENTRY(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ENTRY(EXPAND_SHELLCMDLINE, "shellcmdline"),
#যদি defined(FEAT_SIGNS)
    KEYVALUE_ENTRY(EXPAND_SIGN, "sign"),
#endif
    KEYVALUE_ENTRY(EXPAND_OWNSYNTAX, "syntax"),
#যদি defined(FEAT_PROFILE)
    KEYVALUE_ENTRY(EXPAND_SYNTIME, "syntime"),
#endif
    KEYVALUE_ENTRY(EXPAND_TAGS, "tag"),
    KEYVALUE_ENTRY(EXPAND_TAGS_LISTFILES, "tag_listfiles"),
    KEYVALUE_ENTRY(EXPAND_USER, "user"),
    KEYVALUE_ENTRY(EXPAND_USER_VARS, "var")
};

প্রকারনির্দেশ সংরচনা
{
    cmd_addr_T চাবি;
    ৱর্ণ *fullname;
    size_t fullnamelen;
    ৱর্ণ *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#define ADDRTYPE_ENTRY(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
স্থির addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_ENTRY(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_ENTRY(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_ENTRY(ADDR_সাৰিগুলি, "lines", "line"),
    ADDRTYPE_ENTRY(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ENTRY(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ENTRY(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ENTRY(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ENTRY(ADDR_WINDOWS, "windows", "win")
};

স্থির পূর্ণাঙ্ক cmp_addr_type(ধ্রুৱ শূন্য *a, ধ্রুৱ শূন্য *b);

/*
 * Search for a user command that matches "eap->cmd".
 * Return cmdidx in "eap->cmdidx", flags in "eap->argt", idx in "eap->useridx".
 * Return a pointer to just after the command.
 * Return NULL if there is no matching command.
 */
    char_u *
find_ucmd(
    exarg_T	*eap,
    char_u	*p,	 // end of the command (possibly including count)
    পূর্ণাঙ্ক		*full,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    পূর্ণাঙ্ক		*complp) // completion flags or NULL
{
    পূর্ণাঙ্ক		len = (পূর্ণাঙ্ক)(p - eap->cmd);
    পূর্ণাঙ্ক		j, k, matchlen = 0;
    ucmd_T	*uc;
    পূর্ণাঙ্ক		found = FALSE;
    পূর্ণাঙ্ক		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    পূর্ণাঙ্ক		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    বাবে (;;)
    {
	বাবে (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    যেতিয়ালৈ (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    যদি (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		যদি (k == len && found && *np != NUL)
		{
		    যদি (gap == &ucmds)
			ঘূৰাই রিক্ত;
		    amb_local = TRUE;
		}

		যদি (!found || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    যদি (k == len)
			found = TRUE;
		    অন্যথা
			possible = TRUE;

		    যদি (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    অন্যথা
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (long)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    যদি (complp != রিক্ত)
			*complp = uc->uc_compl;
#ifdef FEAT_EVAL
		    যদি (xp != রিক্ত)
		    {
			xp->xp_arg = uc->uc_compl_arg;
			xp->xp_complete_opt = uc->uc_compl_opt;
			xp->xp_script_ctx = uc->uc_script_ctx;
			xp->xp_script_ctx.sc_lnum += SOURCING_LNUM;
		    }
#endif
		    // Do not search for further abbreviations
		    // if this is an exact match.
		    matchlen = k;
		    যদি (k == len && *np == NUL)
		    {
			যদি (full != রিক্ত)
			    *full = TRUE;
			amb_local = FALSE;
			ভঙ্গ;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	যদি (j < gap->ga_len || gap == &ucmds)
	    ভঙ্গ;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    যদি (amb_local)
    {
	যদি (xp != রিক্ত)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	ঘূৰাই রিক্ত;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    যদি (found || possible)
	ঘূৰাই p + (matchlen - len);
    ঘূৰাই p;
}

/*
 * Set completion context for :command
 */
    char_u *
set_context_in_user_cmd(expand_T *xp, char_u *arg_in)
{
    char_u	*arg = arg_in;
    char_u	*p;

    // Check for attributes
    যেতিয়ালৈ (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	যদি (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    যদি (p == রিক্ত)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_pattern = arg;
		ঘূৰাই রিক্ত;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    যদি (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_pattern = p + 1;
	    }
	    অন্যথা যদি (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_pattern = p + 1;
		বাবে (char_u *c = p + 1; *c != NUL; ++c)
		    যদি (*c == ',')
			xp->xp_pattern = c + 1;
	    }
	    অন্যথা যদি (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_pattern = p + 1;
	    }
	    অন্যথা যদি (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_pattern = p + 1;
	    }
	    ঘূৰাই রিক্ত;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    যদি (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_pattern = arg;
	ঘূৰাই রিক্ত;
    }

    // And finally comes a normal command
    ঘূৰাই skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
set_context_in_user_cmdarg(
	char_u		*cmd UNUSED,
	char_u		*arg,
	long		argt,
	পূর্ণাঙ্ক		context,
	expand_T	*xp,
	পূর্ণাঙ্ক		forceit)
{
    char_u	*p;

    যদি (context == EXPAND_NOTHING)
	ঘূৰাই রিক্ত;

    যদি (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	ঘূৰাই রিক্ত;
    }

#ifdef FEAT_MENU
    যদি (context == EXPAND_MENUS)
	ঘূৰাই set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif
    যদি (context == EXPAND_COMMANDS)
	ঘূৰাই arg;
    যদি (context == EXPAND_MAPPINGS)
	ঘূৰাই set_context_in_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    যদি (!(argt & EX_ARGSPACE))
    {
	p = arg;
	যেতিয়ালৈ (*p)
	{
	    যদি (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    অন্যথা যদি (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_pattern = arg;
    xp->xp_context = context;

    ঘূৰাই রিক্ত;
}

    char_u *
expand_user_command_name(পূর্ণাঙ্ক idx)
{
    ঘূৰাই get_user_commands(রিক্ত, idx - (পূর্ণাঙ্ক)CMD_SIZE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_buffer;

    যদি (idx < buf->b_ucmds.ga_len)
	ঘূৰাই USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    যদি (idx < ucmds.ga_len)
    {
	পূর্ণাঙ্ক	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	বাবে (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    যদি (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		ঘূৰাই (char_u *)"";
	ঘূৰাই name;
    }
    ঘূৰাই রিক্ত;
}

#ifdef FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক cmdidx)
{
    যদি (cmdidx == CMD_USER && idx < ucmds.ga_len)
	ঘূৰাই USER_CMD(idx)->uc_name;
    যদি (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_buffer;

	যদি (idx < buf->b_ucmds.ga_len)
	    ঘূৰাই USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    ঘূৰাই রিক্ত;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    পূর্ণাঙ্ক
user_command_info(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_buffer->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    যদি (idx >= gap->ga_len)
	ঘূৰাই FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_add_string(d, "name", uc->uc_name);
    dict_add_string(d, "kind", (char_u *)"user");
    dict_add_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_add_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_to_str(uc->uc_compl, uc->uc_compl_arg);
    dict_add_string(d, "complete", compl == রিক্ত ? (char_u *)"" : compl);
    vim_free(compl);
    dict_add_string(d, "definition", uc->uc_rep);
    dict_add_সংখ্যা(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_add_সংখ্যা(d, "lnum", uc->uc_script_ctx.sc_lnum);
    ঘূৰাই OK;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx < 0 || idx >= (পূর্ণাঙ্ক)ARRAY_LENGTH(addr_type_complete_tab))
	ঘূৰাই রিক্ত;
    ঘূৰাই (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    স্থির ৱর্ণ *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    যদি (idx < 0 || idx >= (পূর্ণাঙ্ক)ARRAY_LENGTH(user_cmd_flags))
	ঘূৰাই রিক্ত;
    ঘূৰাই (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    স্থির ৱর্ণ *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    যদি (idx < 0 || idx >= (পূর্ণাঙ্ক)ARRAY_LENGTH(user_cmd_nargs))
	ঘূৰাই রিক্ত;
    ঘূৰাই (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx < 0 || idx >= (পূর্ণাঙ্ক)ARRAY_LENGTH(command_complete_tab))
	ঘূৰাই রিক্ত;
    ঘূৰাই command_complete_tab[idx].মান.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
স্থির ৱর্ণ *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx < 0 || idx >= (পূর্ণাঙ্ক)ARRAY_LENGTH(user_cmd_completeopt_tab))
	ঘূৰাই রিক্ত;
    ঘূৰাই (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    স্থির keyvalue_T *
get_commandtype(পূর্ণাঙ্ক expand)
{
    পূর্ণাঙ্ক i;

    বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(command_complete_tab); ++i)
	যদি (command_complete_tab[i].চাবি == expand)
	    ঘূৰাই &command_complete_tab[i];

    ঘূৰাই রিক্ত;
}

#ifdef FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_to_str(পূর্ণাঙ্ক expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    যদি (kv == রিক্ত || kv->মান.string == রিক্ত)
	ঘূৰাই রিক্ত;

    cmd_compl = kv->মান.string;
    যদি (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*buffer;

	buffer = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	যদি (buffer == রিক্ত)
	    ঘূৰাই রিক্ত;
	sprintf((ৱর্ণ *)buffer, "%s,%s", cmd_compl, compl_arg);
	ঘূৰাই buffer;
    }

    ঘূৰাই vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    পূর্ণাঙ্ক
cmdcomplete_str_to_type(char_u *complete_str)
{
    keyvalue_T target;
    keyvalue_T *entry;
    স্থির keyvalue_T *last_entry = রিক্ত;	// cached result

    যদি (STRNCMP(complete_str, "custom,", 7) == 0)
	ঘূৰাই EXPAND_USER_DEFINED;
    যদি (STRNCMP(complete_str, "customlist,", 11) == 0)
	ঘূৰাই EXPAND_USER_LIST;

    target.চাবি = 0;
    target.মান.string = complete_str;
    target.মান.length = 0;			// not used, see cmp_keyvalue_value()

    যদি (last_entry != রিক্ত && cmp_keyvalue_মান(&target, last_entry) == 0)
	entry = last_entry;
    অন্যথা
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    পরিমাণ(command_complete_tab[0]),
	    cmp_keyvalue_মান);
	যদি (entry == রিক্ত)
	    ঘূৰাই EXPAND_NOTHING;

	last_entry = entry;
    }

    ঘূৰাই entry->চাবি;
}
#endif

/*
 * List user commands starting with "name[name_len]".
 */
    স্থির শূন্য
uc_list(char_u *name, size_t name_len)
{
    পূর্ণাঙ্ক		i, j;
    পূর্ণাঙ্ক		found = FALSE;
    ucmd_T	*cmd;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		over;
    long	a;
    garray_T	*gap;
    keyvalue_T	*entry;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    বাবে (;;)
    {
	বাবে (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (long)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    যদি (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || message_filtered(cmd->uc_name))
		অনুৱর্ত;

	    // Put out the title first time
	    যদি (!found)
		বাৰ্তা_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    found = TRUE;
	    বাৰ্তা_putchar('\n');
	    যদি (got_int)
		ভঙ্গ;

	    // Special cases
	    len = 4;
	    যদি (a & EX_BANG)
	    {
		বাৰ্তা_putchar('!');
		--len;
	    }
	    যদি (a & EX_REGSTR)
	    {
		বাৰ্তা_putchar('"');
		--len;
	    }
	    যদি (gap != &ucmds)
	    {
		বাৰ্তা_putchar('b');
		--len;
	    }
	    যদি (a & EX_TRLBAR)
	    {
		বাৰ্তা_putchar('|');
		--len;
	    }
	    যদি (len != 0)
		বাৰ্তা_puts(&"    "[4 - len]);

	    বাৰ্তা_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (পূর্ণাঙ্ক)cmd->uc_namelen + 4;

	    যদি (len < 21)
	    {
		// Field padding spaces   12345678901234567
		স্থির ৱর্ণ spaces[18] = "                 ";
		বাৰ্তা_puts(&spaces[len - 4]);
		len = 21;
	    }
	    বাৰ্তা_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    ৱিকল্প ((পূর্ণাঙ্ক)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		পক্ষ 0:				IObuff[len++] = '0'; ভঙ্গ;
		পক্ষ (EX_EXTRA):		IObuff[len++] = '*'; ভঙ্গ;
		পক্ষ (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; ভঙ্গ;
		পক্ষ (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; ভঙ্গ;
		পক্ষ (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; ভঙ্গ;
		পক্ষ (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; ভঙ্গ;
	    }

	    কৰা
	    {
		IObuff[len++] = ' ';
	    } যেতিয়ালৈ (len < 5 - over);

	    // Address / Range
	    যদি (a & (EX_RANGE|EX_গুনতি))
	    {
		যদি (a & EX_গুনতি)
		{
		    // -count=N
		    len += vim_snprintf((ৱর্ণ *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		অন্যথা যদি (a & EX_DFLALL)
		    IObuff[len++] = '%';
		অন্যথা যদি (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((ৱর্ণ *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		অন্যথা
		    IObuff[len++] = '.';
	    }

	    কৰা
	    {
		IObuff[len++] = ' ';
	    } যেতিয়ালৈ (len < 8 - over);

	    // Address Type
	    বাবে (j = 0; j < (পূর্ণাঙ্ক)ARRAY_LENGTH(addr_type_complete_tab); ++j)
		যদি (addr_type_complete_tab[j].চাবি != ADDR_সাৰিগুলি
			&& addr_type_complete_tab[j].চাবি == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (পূর্ণাঙ্ক)addr_type_complete_tab[j].shortnamelen;
		    ভঙ্গ;
		}

	    কৰা
	    {
		IObuff[len++] = ' ';
	    } যেতিয়ালৈ (len < 13 - over);

	    // Completion
	    entry = get_commandtype(cmd->uc_compl);
	    যদি (entry != রিক্ত)
	    {
		STRCPY(IObuff + len, entry->মান.string);
		len += (পূর্ণাঙ্ক)entry->মান.length;
#ifdef FEAT_EVAL
		যদি (p_verbose > 0 && cmd->uc_compl_arg != রিক্ত)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    যদি (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (পূর্ণাঙ্ক)uc_compl_arglen;
		    }
		}
		যদি (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((ৱর্ণ *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#endif
	    }

	    কৰা
	    {
		IObuff[len++] = ' ';
	    } যেতিয়ালৈ (len < 25 - over);

	    IObuff[len] = NUL;
	    বাৰ্তা_outtrans(IObuff);

	    বাৰ্তা_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ifdef FEAT_EVAL
	    যদি (p_verbose > 0)
		last_set_বাৰ্তা(cmd->uc_script_ctx);
#endif
	    out_flush();
	    ui_breakcheck();
	    যদি (got_int)
		ভঙ্গ;
	}
	যদি (gap == &ucmds || i < gap->ga_len)
	    ভঙ্গ;
	gap = &ucmds;
    }

    যদি (!found)
	বাৰ্তা(_("No user-defined commands found"));

    --ucmd_locked;
}

    ৱর্ণ *
uc_fun_cmd(শূন্য)
{
    স্থির char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    ঘূৰাই (ৱর্ণ *)IObuff;
}

/*
 * Parse address type argument
 */
    স্থির পূর্ণাঙ্ক
parse_addr_type_arg(
    char_u	*মান,
    পূর্ণাঙ্ক		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T target;
    addrtype_T *entry;
    স্থির addrtype_T *last_entry;	// cached result

    target.চাবি = 0;
    target.fullname = (ৱর্ণ *)মান;
    target.fullnamelen = vallen;

    যদি (last_entry != রিক্ত && cmp_addr_type(&target, last_entry) == 0)
	entry = last_entry;
    অন্যথা
    {
	entry = (addrtype_T *)bsearch(&target,
	    &addr_type_complete_tab,
	    ARRAY_LENGTH(addr_type_complete_tab),
	    পরিমাণ(addr_type_complete_tab[0]),
	    cmp_addr_type);
	যদি (entry == রিক্ত)
	{
	    পূর্ণাঙ্ক i;
	    char_u	*err = মান;

	    বাবে (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_address_type_মান_str), err);
	    ঘূৰাই FAIL;
	}

	last_entry = entry;
    }

    *addr_type_arg = entry->চাবি;

    ঘূৰাই OK;
}

    স্থির পূর্ণাঙ্ক
cmp_addr_type(ধ্রুৱ শূন্য *a, ধ্রুৱ শূন্য *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    ঘূৰাই STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    পূর্ণাঙ্ক
parse_compl_arg(
    char_u	*মান,
    পূর্ণাঙ্ক		vallen,
    পূর্ণাঙ্ক		*complp,
    long	*argt,
    char_u	**compl_arg UNUSED)
{
    char_u	*arg = রিক্ত;
#যদি defined(FEAT_EVAL)
    size_t	arglen = 0;
#endif
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		valend = vallen;
    keyvalue_T	target;
    keyvalue_T	*entry;
    স্থির keyvalue_T	*last_entry = রিক্ত;	    // cached result

    // Look for any argument part - which is the part after any ','
    বাবে (i = 0; i < vallen; ++i)
    {
	যদি (মান[i] == ',')
	{
	    arg = &মান[i + 1];
#যদি defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#endif
	    valend = i;
	    ভঙ্গ;
	}
    }

    target.চাবি = 0;
    target.মান.string = মান;
    target.মান.length = valend;

    যদি (last_entry != রিক্ত && cmp_keyvalue_মান_n(&target, last_entry) == 0)
	entry = last_entry;
    অন্যথা
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    পরিমাণ(command_complete_tab[0]),
	    cmp_keyvalue_মান_n);
	যদি (entry == রিক্ত)
	{
	    semsg(_(e_invalid_complete_মান_str), মান);
	    ঘূৰাই FAIL;
	}

	last_entry = entry;
    }

    *complp = entry->চাবি;
    যদি (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    অন্যথা যদি (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    যদি (
#যদি defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#endif
								arg != রিক্ত)
    {
	emsg(_(e_completion_argument_only_allowed_for_custom_completion));
	ঘূৰাই FAIL;
    }

#যদি defined(FEAT_EVAL)
    যদি ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == রিক্ত)
    {
	emsg(_(e_custom_completion_requires_function_argument));
	ঘূৰাই FAIL;
    }

    যদি (arg != রিক্ত)
	*compl_arg = vim_strnsave(arg, arglen);
#endif

    ঘূৰাই OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    স্থির পূর্ণাঙ্ক
parse_completeopt_arg(char_u *মান, পূর্ণাঙ্ক vallen, পূর্ণাঙ্ক *compl_opt)
{
    char_u  *p = মান;
    char_u  *end = মান + vallen;
    পূর্ণাঙ্ক	    flags = 0;

    যদি (vallen == 0)
    {
	semsg(_(e_argument_required_for_str), "-completeopt");
	ঘূৰাই FAIL;
    }

    যেতিয়ালৈ (p < end)
    {
	char_u	*comma;
	size_t	itemlen;
	পূর্ণাঙ্ক	matched = FALSE;
	পূর্ণাঙ্ক	i;

	comma = vim_strchr(p, ',');
	যদি (comma == রিক্ত || comma > end)
	    itemlen = (size_t)(end - p);
	অন্যথা
	    itemlen = (size_t)(comma - p);

	যদি (itemlen == 0)
	{
	    semsg(_(e_invalid_মান_for_argument_str), "completeopt");
	    ঘূৰাই FAIL;
	}

	বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(user_cmd_completeopt_tab); ++i)
	{
	    ৱর্ণ *name = user_cmd_completeopt_tab[i];

	    যদি (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		যদি (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		ভঙ্গ;
	    }
	}
	যদি (!matched)
	{
	    semsg(_(e_invalid_মান_for_argument_str), "completeopt");
	    ঘূৰাই FAIL;
	}

	p += itemlen;
	যদি (p < end && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    ঘূৰাই OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    স্থির পূর্ণাঙ্ক
uc_scan_attr(
    char_u	*attr,
    size_t	len,
    long	*argt,
    long	*def,
    পূর্ণাঙ্ক		*flags,
    পূর্ণাঙ্ক		*complp,
    char_u	**compl_arg,
    পূর্ণাঙ্ক		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    যদি (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	ঘূৰাই FAIL;
    }

    // First, try the simple attributes (no arguments)
    যদি (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    অন্যথা যদি (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_BUFFER;
    অন্যথা যদি (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    অন্যথা যদি (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    অন্যথা যদি (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    অন্যথা
    {
	পূর্ণাঙ্ক	i;
	char_u	*val = রিক্ত;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	বাবে (i = 0; i < (পূর্ণাঙ্ক)len; ++i)
	{
	    যদি (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		ভঙ্গ;
	    }
	}

	যদি (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    যদি (vallen == 1)
	    {
		যদি (*val == '0')
		    // Do nothing - this is the default
		    ;
		অন্যথা যদি (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		অন্যথা যদি (*val == '*')
		    *argt |= EX_EXTRA;
		অন্যথা যদি (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		অন্যথা যদি (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		অন্যথা যদি (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		অন্যথা
		    গচ্ছ wrong_nargs;
	    }
	    অন্যথা
	    {
wrong_nargs:
		emsg(_(e_invalid_সংখ্যা_of_arguments));
		ঘূৰাই FAIL;
	    }
	}
	অন্যথা যদি (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    যদি (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    অন্যথা যদি (val != রিক্ত)
	    {
		p = val;
		যদি (*def >= 0)
		{
two_গুনতি:
		    emsg(_(e_গুনতি_cannot_be_specified_twice));
		    ঘূৰাই FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		যদি (p != val + vallen || vallen == 0)
		{
invalid_গুনতি:
		    emsg(_(e_invalid_default_মান_for_গুনতি));
		    ঘূৰাই FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    যদি (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_সাৰিগুলি;
	}
	অন্যথা যদি (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_গুনতি | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    যদি (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    যদি (val != রিক্ত)
	    {
		p = val;
		যদি (*def >= 0)
		    গচ্ছ two_গুনতি;

		*def = getdigits(&p);

		যদি (p != val + vallen)
		    গচ্ছ invalid_গুনতি;
	    }

	    যদি (*def < 0)
		*def = 0;
	}
	অন্যথা যদি (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    যদি (val == রিক্ত)
	    {
		semsg(_(e_argument_required_for_str), "-complete");
		ঘূৰাই FAIL;
	    }

	    যদি (parse_compl_arg(val, (পূর্ণাঙ্ক)vallen, complp, argt, compl_arg)
								      == FAIL)
		ঘূৰাই FAIL;
	}
	অন্যথা যদি (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    যদি (val == রিক্ত)
	    {
		semsg(_(e_argument_required_for_str), "-completeopt");
		ঘূৰাই FAIL;
	    }

	    যদি (parse_completeopt_arg(val, (পূর্ণাঙ্ক)vallen, compl_opt) == FAIL)
		ঘূৰাই FAIL;
	}
	অন্যথা যদি (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    যদি (val == রিক্ত)
	    {
		semsg(_(e_argument_required_for_str), "-addr");
		ঘূৰাই FAIL;
	    }
	    যদি (parse_addr_type_arg(val, (পূর্ণাঙ্ক)vallen, addr_type_arg) == FAIL)
		ঘূৰাই FAIL;
	    যদি (*addr_type_arg != ADDR_সাৰিগুলি)
		*argt |= EX_ZEROR;
	}
	অন্যথা
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    ঘূৰাই FAIL;
	}
    }

    ঘূৰাই OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    স্থির পূর্ণাঙ্ক
uc_add_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    long	argt,
    long	def,
    পূর্ণাঙ্ক		flags,
    পূর্ণাঙ্ক		compl,
    char_u	*compl_arg UNUSED,
    পূর্ণাঙ্ক		compl_opt,
    cmd_addr_T	addr_type,
    পূর্ণাঙ্ক		force)
{
    ucmd_T	*cmd = রিক্ত;
    char_u	*p;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		cmp = 1;
    char_u	*rep_buf = রিক্ত;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, রিক্ত);
    যদি (rep_buf == রিক্ত)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	যদি (rep_buf == রিক্ত)
	    ঘূৰাই FAIL;
    }

    // get address of growarray: global or in curbuf
    যদি (flags & UC_BUFFER)
    {
	gap = &curbuf->b_ucmds;
	যদি (gap->ga_itemsize == 0)
	    ga_init2(gap, পরিমাণ(ucmd_T), 4);
    }
    অন্যথা
	gap = &ucmds;

    // Search for the command in the already defined commands.
    বাবে (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	যদি (cmp == 0)
	{
	    যদি (name_len < cmd->uc_namelen)
		cmp = -1;
	    অন্যথা যদি (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	যদি (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    যদি (!force
#ifdef FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#endif
		    )
	    {
		semsg(_(e_command_already_exists_add_bang_to_replace_it_str),
									 name);
		গচ্ছ fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#যদি defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#endif
	    ভঙ্গ;
	}

	// Stop as soon as we pass the name to add
	যদি (cmp < 0)
	    ভঙ্গ;
    }

    // Extend the array unless we're replacing an existing command
    যদি (cmp != 0)
    {
	যদি (ga_grow(gap, 1) == FAIL)
	    গচ্ছ fail;
	যদি ((p = vim_strnsave(name, name_len)) == রিক্ত)
	    গচ্ছ fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * পরিমাণ(ucmd_T));

	++gap->ga_len;

	cmd->uc_name = p;
	cmd->uc_namelen = name_len;
    }

    cmd->uc_rep = rep_buf;
    cmd->uc_argt = argt;
    cmd->uc_def = def;
    cmd->uc_compl = compl;
    cmd->uc_compl_opt = compl_opt;
    cmd->uc_script_ctx = current_sctx;
    যদি (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ifdef FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#endif
    cmd->uc_addr_type = addr_type;

    ঘূৰাই OK;

fail:
    vim_free(rep_buf);
#যদি defined(FEAT_EVAL)
    vim_free(compl_arg);
#endif
    ঘূৰাই FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_block(exarg_T *eap, char_u *p, char_u **tofree, পূর্ণাঙ্ক *flags)
{
    char_u *retp = p;
    char_u *block = find_cmd_block_start(p);

    যদি (block != রিক্ত && eap->ea_getline != রিক্ত)
    {
	garray_T    ga;
	char_u	    *সাৰি = রিক্ত;

	ga_init2(&ga, পরিমাণ(char_u *), 10);
	যদি (ga_copy_string(&ga, p) == FAIL)
	    ঘূৰাই retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	যদি (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    বাবে (;;)
	    {
		vim_free(সাৰি);
		যদি ((সাৰি = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == রিক্ত)
		{
		    emsg(_(e_missing_rcurly));
		    ভঙ্গ;
		}
		যদি (ga_copy_string(&ga, সাৰি) == FAIL)
		    ভঙ্গ;
		যদি (*skipwhite(সাৰি) == '}')
		    ভঙ্গ;
	    }
	vim_free(সাৰি);
	retp = *tofree = ga_concat_strings(&ga, "\n");
	যদি (retp == রিক্ত)
	    retp = p;
	ga_clear_strings(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	যদি (block == p)
	    *flags |= UC_VIM9;
    }
    ঘূৰাই retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    শূন্য
skip_cmd_block(exarg_T *eap)
{
    char_u	*tofree = রিক্ত;
    পূর্ণাঙ্ক		flags = 0;

    (শূন্য)may_get_cmd_block(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    শূন্য
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*end;
    char_u	*p;
    long	argt = 0;
    long	def = -1;
    পূর্ণাঙ্ক		flags = 0;
    পূর্ণাঙ্ক		compl = EXPAND_NOTHING;
    char_u	*compl_arg = রিক্ত;
    পূর্ণাঙ্ক		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    পূর্ণাঙ্ক		has_attr = (eap->arg[0] == '-');
    পূর্ণাঙ্ক		name_len;

    যদি (eap->skip)
    {
	skip_cmd_block(eap);
	ঘূৰাই;
    }

    p = eap->arg;

    // Check for attributes
    যেতিয়ালৈ (*p == '-')
    {
	++p;
	end = skiptowhite(p);
	যদি (uc_scan_attr(p, end - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    গচ্ছ theend;
	p = skipwhite(end);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    যদি (ASCII_ISALPHA(*p))
	যেতিয়ালৈ (ASCII_ISALNUM(*p))
	    ++p;
    যদি (!ends_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	গচ্ছ theend;
    }
    end = p;
    name_len = (পূর্ণাঙ্ক)(end - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(end);
    যদি (!has_attr && ends_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    অন্যথা যদি (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_start_with_an_uppercase_letter));
    }
    অন্যথা যদি ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_used_for_user_defined_command));
    }
    অন্যথা যদি (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	যদি (in_vim9script())
	    emsg(_(e_complete_used_without_allowing_arguments));
	অন্যথা
	    give_warning_with_source(
		       (char_u *)_(e_complete_used_without_allowing_arguments),
								   TRUE, TRUE);
    }
    অন্যথা যদি ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_used_with_nargs_underscore));
    }
    অন্যথা
    {
	char_u *tofree = রিক্ত;

	p = may_get_cmd_block(eap, p, &tofree, &flags);

	uc_add_command(name, end - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	ঘূৰাই;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    শূন্য
ex_comclear(exarg_T *eap UNUSED)
{
    uc_clear(&ucmds);
    যদি (curbuf != রিক্ত)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    স্থির পূর্ণাঙ্ক
is_ucmd_locked(শূন্য)
{
    যদি (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    শূন্য
uc_clear(garray_T *gap)
{
    পূর্ণাঙ্ক		i;
    ucmd_T	*cmd;

    যদি (is_ucmd_locked())
	ঘূৰাই;

    বাবে (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#যদি defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#endif
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    শূন্য
ex_delcommand(exarg_T *eap)
{
    পূর্ণাঙ্ক		i = 0;
    ucmd_T	*cmd = রিক্ত;
    পূর্ণাঙ্ক		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    পূর্ণাঙ্ক		buffer_only = FALSE;

    যদি (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	buffer_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    বাবে (;;)
    {
	বাবে (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    যদি (res <= 0)
		ভঙ্গ;
	}
	যদি (gap == &ucmds || res == 0 || buffer_only)
	    ভঙ্গ;
	gap = &ucmds;
    }

    যদি (res != 0)
    {
	semsg(_(buffer_only
		    ? e_no_such_user_defined_command_in_current_buffer_str
		    : e_no_such_user_defined_command_str), arg);
	ঘূৰাই;
    }

    যদি (is_ucmd_locked())
	ঘূৰাই;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#যদি defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#endif

    --gap->ga_len;

    যদি (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * পরিমাণ(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    স্থির char_u *
uc_split_args(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    পূর্ণাঙ্ক len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    যেতিয়ালৈ (*p)
    {
	যদি (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	অন্যথা যদি (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	অন্যথা যদি (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	অন্যথা যদি (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    যদি (*p == NUL)
		ভঙ্গ;
	    len += 4; // ", "
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    যদি (buf == রিক্ত)
    {
	*lenp = 0;
	ঘূৰাই buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    যেতিয়ালৈ (*p)
    {
	যদি (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	অন্যথা যদি (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	অন্যথা যদি (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	অন্যথা যদি (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    যদি (*p == NUL)
		ভঙ্গ;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	অন্যথা
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    ঘূৰাই buf;
}

    স্থির size_t
add_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    ৱর্ণ	*mod_str,
    size_t	mod_strlen,
    পূর্ণাঙ্ক		*multi_mods)
{
    যদি (buf != রিক্ত)
    {
	যদি (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    যদি (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    অন্যথা
	*multi_mods = 1;

    ঘূৰাই mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
add_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, পূর্ণাঙ্ক *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    যদি (cmod->cmod_split & WSP_ABOVE)
	buflen += add_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    যদি (cmod->cmod_split & WSP_BELOW)
	buflen += add_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    যদি (cmod->cmod_split & WSP_BOT)
	buflen += add_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    যদি (cmod->cmod_tab > 0)
    {
	পূর্ণাঙ্ক tabnr = cmod->cmod_tab - 1;

	যদি (tabnr == tabpage_index(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += add_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	অন্যথা
	{
	    ৱর্ণ tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, পরিমাণ(tab_buf), "%dtab", tabnr);
	    buflen += add_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    যদি (cmod->cmod_split & WSP_শীৰ্ষ)
	buflen += add_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    যদি (cmod->cmod_split & WSP_VERT)
	buflen += add_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    যদি (cmod->cmod_split & WSP_HOR)
	buflen += add_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    ঘূৰাই buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, পূর্ণাঙ্ক quote)
{
    size_t  buflen = 0;
    পূর্ণাঙ্ক	    multi_mods = 0;
    পূর্ণাঙ্ক	    i;
    স্থির keyvalue_T mod_entry_tab[] =
    {
#ifdef FEAT_BROWSE_CMD
	KEYVALUE_ENTRY(CMOD_BROWSE, "browse"),
#endif
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	KEYVALUE_ENTRY(CMOD_CONFIRM, "confirm"),
#endif
	KEYVALUE_ENTRY(CMOD_HIDE, "hide"),
	KEYVALUE_ENTRY(CMOD_KEEPALT, "keepalt"),
	KEYVALUE_ENTRY(CMOD_KEEPJUMPS, "keepjumps"),
	KEYVALUE_ENTRY(CMOD_KEEPMARKS, "keepmarks"),
	KEYVALUE_ENTRY(CMOD_KEEPPATTERNS, "keeppatterns"),
	KEYVALUE_ENTRY(CMOD_LOCKMARKS, "lockmarks"),
	KEYVALUE_ENTRY(CMOD_NOSWAPFILE, "noswapfile"),
	KEYVALUE_ENTRY(CMOD_UNSILENT, "unsilent"),
	KEYVALUE_ENTRY(CMOD_NOAUTOCMD, "noautocmd"),
#ifdef HAVE_SANDBOX
	KEYVALUE_ENTRY(CMOD_SANDBOX, "sandbox"),
#endif
	KEYVALUE_ENTRY(CMOD_LEGACY, "legacy")
    };

    যদি (quote)
    {
	++buflen;
	যদি (buf != রিক্ত)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    অন্যথা
    যদি (buf != রিক্ত)
	*buf = NUL;

    // the modifiers that are simple flags
    বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(mod_entry_tab); ++i)
	যদি (cmod->cmod_flags & mod_entry_tab[i].চাবি)
	    buflen += add_cmd_modifier(buf, buflen,
		    (ৱর্ণ *)mod_entry_tab[i].মান.string,
		    mod_entry_tab[i].মান.length, &multi_mods);

    // :silent
    যদি (cmod->cmod_flags & CMOD_SILENT)
    {
	যদি (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += add_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	অন্যথা
	    buflen += add_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    যদি (cmod->cmod_verbose > 0)
    {
	পূর্ণাঙ্ক verbose_মান = cmod->cmod_verbose - 1;

	যদি (verbose_মান == 1)
	    buflen += add_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	অন্যথা
	{
	    ৱর্ণ verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, পরিমাণ(verbose_buf), "%dverbose", verbose_মান);
	    buflen += add_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += add_win_cmd_modifiers((buf == রিক্ত) ? রিক্ত : buf + buflen, cmod, &multi_mods);

    যদি (quote)
    {
	যদি (buf == রিক্ত)
	    ++buflen;
	অন্যথা
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    ঘূৰাই buflen;
}

/*
 * Check for a <> code in a user command.
 * "code" points to the '<'.  "len" the length of the <> (inclusive).
 * "buf" is where the result is to be added.
 * "split_buf" points to a buffer used for splitting, caller should free it.
 * "split_len" is the length of what "split_buf" contains.
 * Returns the length of the replacement, which has been added to "buf".
 * Returns -1 if there was no match, and only the "<" has been copied.
 */
    স্থির size_t
uc_check_code(
    char_u	*code,
    size_t	len,
    char_u	*buf,
    ucmd_T	*cmd,		// the user command we're expanding
    exarg_T	*eap,		// ex arguments
    char_u	**split_buf,
    size_t	*split_len)
{
    size_t	result = 0;
    char_u	*p = code + 1;
    size_t	l = len - 2;
    পূর্ণাঙ্ক		quote = 0;
    গণনা {
	ct_ARGS,
	ct_BANG,
	ct_গুনতি,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_REGISTER,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    যদি ((vim_strchr((char_u *)"qQfF", *p) != রিক্ত) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    যদি (l <= 1)
	type = ct_NONE;
    অন্যথা যদি (STRNICMP(p, "args>", l) == 0)
	type = ct_ARGS;
    অন্যথা যদি (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    অন্যথা যদি (STRNICMP(p, "count>", l) == 0)
	type = ct_গুনতি;
    অন্যথা যদি (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    অন্যথা যদি (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    অন্যথা যদি (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    অন্যথা যদি (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    অন্যথা যদি (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_REGISTER;
    অন্যথা যদি (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    ৱিকল্প (type)
    {
    পক্ষ ct_ARGS:
	// Simple case first
	যদি (*eap->arg == NUL)
	{
	    যদি (quote == 1)
	    {
		result = 2;
		যদি (buf != রিক্ত)
		    STRCPY(buf, "''");
	    }
	    অন্যথা
		result = 0;
	    ভঙ্গ;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	যদি ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	ৱিকল্প (quote)
	{
	পক্ষ 0: // No quoting, no splitting
	    result = STRLEN(eap->arg);
	    যদি (buf != রিক্ত)
		STRCPY(buf, eap->arg);
	    ভঙ্গ;
	পক্ষ 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    পূর্ণাঙ্ক	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    result = STRLEN(eap->arg) + 2;
	    বাবে (p = eap->arg; *p; ++p)
	    {
		যদি (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		অন্যথা যদি (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    যদি (VIM_ISWHITE(p[1]))
			--result;
		    ++p;
		}
		অন্যথা
		     যদি (*p == '\\' || *p == '"')
		    ++result;
	    }

	    যদি (buf != রিক্ত)
	    {
		*buf++ = '"';
		বাবে (p = eap->arg; *p; ++p)
		{
		    যদি (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    অন্যথা যদি (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			যদি (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    অন্যথা
			 যদি (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    ভঙ্গ;
	}
	পক্ষ 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    যদি (*split_buf == রিক্ত)
		*split_buf = uc_split_args(eap->arg, split_len);

	    result = *split_len;
	    যদি (buf != রিক্ত && result != 0)
		STRCPY(buf, *split_buf);

	    ভঙ্গ;
	}
	ভঙ্গ;

    পক্ষ ct_BANG:
	result = eap->forceit ? 1 : 0;
	যদি (quote)
	    result += 2;
	যদি (buf != রিক্ত)
	{
	    যদি (quote)
		*buf++ = '"';
	    যদি (eap->forceit)
		*buf++ = '!';
	    যদি (quote)
		*buf = '"';
	}
	ভঙ্গ;

    পক্ষ ct_LINE1:
    পক্ষ ct_LINE2:
    পক্ষ ct_RANGE:
    পক্ষ ct_গুনতি:
    {
	ৱর্ণ অঙ্ক_buf[NUMBUFLEN];
	long অঙ্ক = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_গুনতি :
		   (eap->addr_গুনতি > 0) ? eap->line2 : cmd->uc_def;
	size_t অঙ্ক_len;

	অঙ্ক_len = vim_snprintf(অঙ্ক_buf, পরিমাণ(অঙ্ক_buf), "%ld", অঙ্ক);
	result = অঙ্ক_len;

	যদি (quote)
	    result += 2;

	যদি (buf != রিক্ত)
	{
	    যদি (quote)
		*buf++ = '"';
	    STRCPY(buf, অঙ্ক_buf);
	    buf += অঙ্ক_len;
	    যদি (quote)
		*buf = '"';
	}

	ভঙ্গ;
    }

    পক্ষ ct_MODS:
    {
	result = produce_cmdmods(buf, &cmdmod, quote);
	ভঙ্গ;
    }

    পক্ষ ct_REGISTER:
	result = eap->regname ? 1 : 0;
	যদি (quote)
	    result += 2;
	যদি (buf != রিক্ত)
	{
	    যদি (quote)
		*buf++ = '\'';
	    যদি (eap->regname)
		*buf++ = eap->regname;
	    যদি (quote)
		*buf = '\'';
	}
	ভঙ্গ;

    পক্ষ ct_LT:
	result = 1;
	যদি (buf != রিক্ত)
	    *buf = '<';
	ভঙ্গ;

    স্ৱভাৱ:
	// Not recognized: just copy the '<' and return -1.
	result = (size_t)-1;
	যদি (buf != রিক্ত)
	    *buf = '<';
	ভঙ্গ;
    }

    ঘূৰাই result;
}

/*
 * Execute a user defined command.
 */
    শূন্য
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*start;
    char_u	*end = রিক্ত;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	split_len = 0;
    char_u	*split_buf = রিক্ত;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    পূর্ণাঙ্ক		restore_current_sctx = FALSE;
#ifdef FEAT_EVAL
    পূর্ণাঙ্ক		restore_script_version = 0;
#endif

    যদি (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    অন্যথা
	cmd = USER_CMD_GA(&prevwin_curwin()->w_buffer->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = রিক্ত;
    বাবে (;;)
    {
	p = cmd->uc_rep;    // source
	q = buf;	    // destination
	totlen = 0;

	বাবে (;;)
	{
	    start = vim_strchr(p, '<');
	    যদি (start != রিক্ত)
		end = vim_strchr(start + 1, '>');
	    যদি (buf != রিক্ত)
	    {
		বাবে (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		যদি (*ksp == K_SPECIAL
			&& (start == রিক্ত || ksp < start || end == রিক্ত)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ifdef FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (পূর্ণাঙ্ক)KE_CSI)
#endif
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    যদি (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    অনুৱর্ত;
		}
	    }

	    // break if no <item> is found
	    যদি (start == রিক্ত || end == রিক্ত)
		ভঙ্গ;

	    // Include the '>'
	    ++end;

	    // Take everything up to the '<'
	    len = start - p;
	    যদি (buf == রিক্ত)
		totlen += len;
	    অন্যথা
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_code(start, end - start, q, cmd, eap,
			     &split_buf, &split_len);
	    যদি (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = start + 1;
		len = 1;
	    }
	    অন্যথা
		p = end;
	    যদি (buf == রিক্ত)
		totlen += len;
	    অন্যথা
		q += len;
	}
	যদি (buf != রিক্ত)	    // second time here, finished
	{
	    STRCPY(q, p);
	    ভঙ্গ;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	যদি (buf == রিক্ত)
	{
	    vim_free(split_buf);
	    ঘূৰাই;
	}
    }

    যদি ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_version = cmd->uc_script_ctx.sc_version;
#ifdef FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	যদি (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_version =
				  SCRIPT_ITEM(current_sctx.sc_sid)->sn_version;
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = SCRIPT_VERSION_VIM9;
	}
#endif
    }

    (শূন্য)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    যদি (restore_current_sctx)
    {
#ifdef FEAT_EVAL
	যদি (restore_script_version != 0)
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version =
							restore_script_version;
#endif
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(split_buf);
}
