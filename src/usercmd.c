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

#അന്തര്ഭാവ "vim.h"

തരനിർവചനം ഘടന ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    long	uc_def;		// The default value for a range/count
    പൂർണ്ണസംഖ്യ		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    പൂർണ്ണസംഖ്യ		uc_flags;	// some UC_ flags
    പൂർണ്ണസംഖ്യ		uc_compl_opt;	// completion options (UCC_ flags)
#ifdef FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#endif
} ucmd_T;

// List of all user commands.
സ്ഥിതം garray_T ucmds = {0, 0, വലുപ്പം(ucmd_T), 4, ശൂന്യ};

// When non-zero it is not allowed to add or remove user commands
സ്ഥിതം പൂർണ്ണസംഖ്യ ucmd_locked = 0;

#define USER_CMD(i) (&((ucmd_T *)(ucmds.ga_data))[i])
#define USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_data))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
സ്ഥിതം keyvalue_T command_complete_tab[] =
{
    KEYVALUE_ENTRY(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_ENTRY(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_ENTRY(EXPAND_BEHAVE, "behave"),
#എങ്കിൽ defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_BREAKPOINT, "breakpoint"),
#endif
    KEYVALUE_ENTRY(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ENTRY(EXPAND_COLORS, "color"),
    KEYVALUE_ENTRY(EXPAND_COMMANDS, "command"),
    KEYVALUE_ENTRY(EXPAND_COMPILER, "compiler"),
#എങ്കിൽ defined(FEAT_CSCOPE)
    KEYVALUE_ENTRY(EXPAND_CSCOPE, "cscope"),
#endif
#എങ്കിൽ defined(FEAT_EVAL)
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
#എങ്കിൽ defined(FEAT_KEYMAP)
    KEYVALUE_ENTRY(EXPAND_KEYMAP, "keymap"),
#endif
#എങ്കിൽ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
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
#എങ്കിൽ defined(FEAT_EVAL)
    KEYVALUE_ENTRY(EXPAND_SCRIPTNAMES, "scriptnames"),
#endif
    KEYVALUE_ENTRY(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ENTRY(EXPAND_SHELLCMDLINE, "shellcmdline"),
#എങ്കിൽ defined(FEAT_SIGNS)
    KEYVALUE_ENTRY(EXPAND_SIGN, "sign"),
#endif
    KEYVALUE_ENTRY(EXPAND_OWNSYNTAX, "syntax"),
#എങ്കിൽ defined(FEAT_PROFILE)
    KEYVALUE_ENTRY(EXPAND_SYNTIME, "syntime"),
#endif
    KEYVALUE_ENTRY(EXPAND_TAGS, "tag"),
    KEYVALUE_ENTRY(EXPAND_TAGS_LISTFILES, "tag_listfiles"),
    KEYVALUE_ENTRY(EXPAND_USER, "user"),
    KEYVALUE_ENTRY(EXPAND_USER_VARS, "var")
};

തരനിർവചനം ഘടന
{
    cmd_addr_T താക്കോൽ;
    അക്ഷരം *fullname;
    size_t fullnamelen;
    അക്ഷരം *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#define ADDRTYPE_ENTRY(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
സ്ഥിതം addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_ENTRY(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_ENTRY(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_ENTRY(ADDR_വരികൾ, "lines", "line"),
    ADDRTYPE_ENTRY(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ENTRY(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ENTRY(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ENTRY(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ENTRY(ADDR_WINDOWS, "windows", "win")
};

സ്ഥിതം പൂർണ്ണസംഖ്യ cmp_addr_type(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b);

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
    പൂർണ്ണസംഖ്യ		*full,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    പൂർണ്ണസംഖ്യ		*complp) // completion flags or NULL
{
    പൂർണ്ണസംഖ്യ		len = (പൂർണ്ണസംഖ്യ)(p - eap->cmd);
    പൂർണ്ണസംഖ്യ		j, k, matchlen = 0;
    ucmd_T	*uc;
    പൂർണ്ണസംഖ്യ		found = FALSE;
    പൂർണ്ണസംഖ്യ		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    പൂർണ്ണസംഖ്യ		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    വേണ്ടി (;;)
    {
	വേണ്ടി (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    ആകുമ്പോൾ (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    എങ്കിൽ (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		എങ്കിൽ (k == len && found && *np != NUL)
		{
		    എങ്കിൽ (gap == &ucmds)
			തിരികെ ശൂന്യ;
		    amb_local = TRUE;
		}

		എങ്കിൽ (!found || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    എങ്കിൽ (k == len)
			found = TRUE;
		    അല്ലെങ്കിൽ
			possible = TRUE;

		    എങ്കിൽ (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    അല്ലെങ്കിൽ
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (long)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    എങ്കിൽ (complp != ശൂന്യ)
			*complp = uc->uc_compl;
#ifdef FEAT_EVAL
		    എങ്കിൽ (xp != ശൂന്യ)
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
		    എങ്കിൽ (k == len && *np == NUL)
		    {
			എങ്കിൽ (full != ശൂന്യ)
			    *full = TRUE;
			amb_local = FALSE;
			നിർത്തുക;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	എങ്കിൽ (j < gap->ga_len || gap == &ucmds)
	    നിർത്തുക;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    എങ്കിൽ (amb_local)
    {
	എങ്കിൽ (xp != ശൂന്യ)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	തിരികെ ശൂന്യ;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    എങ്കിൽ (found || possible)
	തിരികെ p + (matchlen - len);
    തിരികെ p;
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
    ആകുമ്പോൾ (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	എങ്കിൽ (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    എങ്കിൽ (p == ശൂന്യ)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_pattern = arg;
		തിരികെ ശൂന്യ;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    എങ്കിൽ (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_pattern = p + 1;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_pattern = p + 1;
		വേണ്ടി (char_u *c = p + 1; *c != NUL; ++c)
		    എങ്കിൽ (*c == ',')
			xp->xp_pattern = c + 1;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_pattern = p + 1;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_pattern = p + 1;
	    }
	    തിരികെ ശൂന്യ;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    എങ്കിൽ (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_pattern = arg;
	തിരികെ ശൂന്യ;
    }

    // And finally comes a normal command
    തിരികെ skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
set_context_in_user_cmdarg(
	char_u		*cmd UNUSED,
	char_u		*arg,
	long		argt,
	പൂർണ്ണസംഖ്യ		context,
	expand_T	*xp,
	പൂർണ്ണസംഖ്യ		forceit)
{
    char_u	*p;

    എങ്കിൽ (context == EXPAND_NOTHING)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	തിരികെ ശൂന്യ;
    }

#ifdef FEAT_MENU
    എങ്കിൽ (context == EXPAND_MENUS)
	തിരികെ set_context_in_menu_cmd(xp, cmd, arg, forceit);
#endif
    എങ്കിൽ (context == EXPAND_COMMANDS)
	തിരികെ arg;
    എങ്കിൽ (context == EXPAND_MAPPINGS)
	തിരികെ set_context_in_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    എങ്കിൽ (!(argt & EX_ARGSPACE))
    {
	p = arg;
	ആകുമ്പോൾ (*p)
	{
	    എങ്കിൽ (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_pattern = arg;
    xp->xp_context = context;

    തിരികെ ശൂന്യ;
}

    char_u *
expand_user_command_name(പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ get_user_commands(ശൂന്യ, idx - (പൂർണ്ണസംഖ്യ)CMD_SIZE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_buffer;

    എങ്കിൽ (idx < buf->b_ucmds.ga_len)
	തിരികെ USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    എങ്കിൽ (idx < ucmds.ga_len)
    {
	പൂർണ്ണസംഖ്യ	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	വേണ്ടി (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    എങ്കിൽ (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		തിരികെ (char_u *)"";
	തിരികെ name;
    }
    തിരികെ ശൂന്യ;
}

#ifdef FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ cmdidx)
{
    എങ്കിൽ (cmdidx == CMD_USER && idx < ucmds.ga_len)
	തിരികെ USER_CMD(idx)->uc_name;
    എങ്കിൽ (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_buffer;

	എങ്കിൽ (idx < buf->b_ucmds.ga_len)
	    തിരികെ USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    പൂർണ്ണസംഖ്യ
user_command_info(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_buffer->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    എങ്കിൽ (idx >= gap->ga_len)
	തിരികെ FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_add_string(d, "name", uc->uc_name);
    dict_add_string(d, "kind", (char_u *)"user");
    dict_add_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_add_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_to_str(uc->uc_compl, uc->uc_compl_arg);
    dict_add_string(d, "complete", compl == ശൂന്യ ? (char_u *)"" : compl);
    vim_free(compl);
    dict_add_string(d, "definition", uc->uc_rep);
    dict_add_സംഖ്യ(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_add_സംഖ്യ(d, "lnum", uc->uc_script_ctx.sc_lnum);
    തിരികെ OK;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx < 0 || idx >= (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(addr_type_complete_tab))
	തിരികെ ശൂന്യ;
    തിരികെ (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    സ്ഥിതം അക്ഷരം *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    എങ്കിൽ (idx < 0 || idx >= (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(user_cmd_flags))
	തിരികെ ശൂന്യ;
    തിരികെ (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    സ്ഥിതം അക്ഷരം *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    എങ്കിൽ (idx < 0 || idx >= (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(user_cmd_nargs))
	തിരികെ ശൂന്യ;
    തിരികെ (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx < 0 || idx >= (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(command_complete_tab))
	തിരികെ ശൂന്യ;
    തിരികെ command_complete_tab[idx].മൂല്യം.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
സ്ഥിതം അക്ഷരം *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx < 0 || idx >= (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(user_cmd_completeopt_tab))
	തിരികെ ശൂന്യ;
    തിരികെ (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    സ്ഥിതം keyvalue_T *
get_commandtype(പൂർണ്ണസംഖ്യ expand)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(command_complete_tab); ++i)
	എങ്കിൽ (command_complete_tab[i].താക്കോൽ == expand)
	    തിരികെ &command_complete_tab[i];

    തിരികെ ശൂന്യ;
}

#ifdef FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_to_str(പൂർണ്ണസംഖ്യ expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    എങ്കിൽ (kv == ശൂന്യ || kv->മൂല്യം.string == ശൂന്യ)
	തിരികെ ശൂന്യ;

    cmd_compl = kv->മൂല്യം.string;
    എങ്കിൽ (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*buffer;

	buffer = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	എങ്കിൽ (buffer == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	sprintf((അക്ഷരം *)buffer, "%s,%s", cmd_compl, compl_arg);
	തിരികെ buffer;
    }

    തിരികെ vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    പൂർണ്ണസംഖ്യ
cmdcomplete_str_to_type(char_u *complete_str)
{
    keyvalue_T target;
    keyvalue_T *entry;
    സ്ഥിതം keyvalue_T *last_entry = ശൂന്യ;	// cached result

    എങ്കിൽ (STRNCMP(complete_str, "custom,", 7) == 0)
	തിരികെ EXPAND_USER_DEFINED;
    എങ്കിൽ (STRNCMP(complete_str, "customlist,", 11) == 0)
	തിരികെ EXPAND_USER_LIST;

    target.താക്കോൽ = 0;
    target.മൂല്യം.string = complete_str;
    target.മൂല്യം.length = 0;			// not used, see cmp_keyvalue_value()

    എങ്കിൽ (last_entry != ശൂന്യ && cmp_keyvalue_മൂല്യം(&target, last_entry) == 0)
	entry = last_entry;
    അല്ലെങ്കിൽ
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    വലുപ്പം(command_complete_tab[0]),
	    cmp_keyvalue_മൂല്യം);
	എങ്കിൽ (entry == ശൂന്യ)
	    തിരികെ EXPAND_NOTHING;

	last_entry = entry;
    }

    തിരികെ entry->താക്കോൽ;
}
#endif

/*
 * List user commands starting with "name[name_len]".
 */
    സ്ഥിതം ശൂന്യം
uc_list(char_u *name, size_t name_len)
{
    പൂർണ്ണസംഖ്യ		i, j;
    പൂർണ്ണസംഖ്യ		found = FALSE;
    ucmd_T	*cmd;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		over;
    long	a;
    garray_T	*gap;
    keyvalue_T	*entry;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_buffer->b_ucmds;
    വേണ്ടി (;;)
    {
	വേണ്ടി (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (long)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    എങ്കിൽ (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || message_filtered(cmd->uc_name))
		തുടരുക;

	    // Put out the title first time
	    എങ്കിൽ (!found)
		സന്ദേശം_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    found = TRUE;
	    സന്ദേശം_putchar('\n');
	    എങ്കിൽ (got_int)
		നിർത്തുക;

	    // Special cases
	    len = 4;
	    എങ്കിൽ (a & EX_BANG)
	    {
		സന്ദേശം_putchar('!');
		--len;
	    }
	    എങ്കിൽ (a & EX_REGSTR)
	    {
		സന്ദേശം_putchar('"');
		--len;
	    }
	    എങ്കിൽ (gap != &ucmds)
	    {
		സന്ദേശം_putchar('b');
		--len;
	    }
	    എങ്കിൽ (a & EX_TRLBAR)
	    {
		സന്ദേശം_putchar('|');
		--len;
	    }
	    എങ്കിൽ (len != 0)
		സന്ദേശം_puts(&"    "[4 - len]);

	    സന്ദേശം_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (പൂർണ്ണസംഖ്യ)cmd->uc_namelen + 4;

	    എങ്കിൽ (len < 21)
	    {
		// Field padding spaces   12345678901234567
		സ്ഥിതം അക്ഷരം spaces[18] = "                 ";
		സന്ദേശം_puts(&spaces[len - 4]);
		len = 21;
	    }
	    സന്ദേശം_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    തിരഞ്ഞെടുപ്പ് ((പൂർണ്ണസംഖ്യ)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		അവസ്ഥ 0:				IObuff[len++] = '0'; നിർത്തുക;
		അവസ്ഥ (EX_EXTRA):		IObuff[len++] = '*'; നിർത്തുക;
		അവസ്ഥ (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; നിർത്തുക;
		അവസ്ഥ (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; നിർത്തുക;
		അവസ്ഥ (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; നിർത്തുക;
		അവസ്ഥ (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; നിർത്തുക;
	    }

	    ചെയ്യുക
	    {
		IObuff[len++] = ' ';
	    } ആകുമ്പോൾ (len < 5 - over);

	    // Address / Range
	    എങ്കിൽ (a & (EX_RANGE|EX_എണ്ണം))
	    {
		എങ്കിൽ (a & EX_എണ്ണം)
		{
		    // -count=N
		    len += vim_snprintf((അക്ഷരം *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (a & EX_DFLALL)
		    IObuff[len++] = '%';
		അല്ലെങ്കിൽ എങ്കിൽ (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((അക്ഷരം *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		അല്ലെങ്കിൽ
		    IObuff[len++] = '.';
	    }

	    ചെയ്യുക
	    {
		IObuff[len++] = ' ';
	    } ആകുമ്പോൾ (len < 8 - over);

	    // Address Type
	    വേണ്ടി (j = 0; j < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(addr_type_complete_tab); ++j)
		എങ്കിൽ (addr_type_complete_tab[j].താക്കോൽ != ADDR_വരികൾ
			&& addr_type_complete_tab[j].താക്കോൽ == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (പൂർണ്ണസംഖ്യ)addr_type_complete_tab[j].shortnamelen;
		    നിർത്തുക;
		}

	    ചെയ്യുക
	    {
		IObuff[len++] = ' ';
	    } ആകുമ്പോൾ (len < 13 - over);

	    // Completion
	    entry = get_commandtype(cmd->uc_compl);
	    എങ്കിൽ (entry != ശൂന്യ)
	    {
		STRCPY(IObuff + len, entry->മൂല്യം.string);
		len += (പൂർണ്ണസംഖ്യ)entry->മൂല്യം.length;
#ifdef FEAT_EVAL
		എങ്കിൽ (p_verbose > 0 && cmd->uc_compl_arg != ശൂന്യ)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    എങ്കിൽ (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (പൂർണ്ണസംഖ്യ)uc_compl_arglen;
		    }
		}
		എങ്കിൽ (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((അക്ഷരം *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#endif
	    }

	    ചെയ്യുക
	    {
		IObuff[len++] = ' ';
	    } ആകുമ്പോൾ (len < 25 - over);

	    IObuff[len] = NUL;
	    സന്ദേശം_outtrans(IObuff);

	    സന്ദേശം_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ifdef FEAT_EVAL
	    എങ്കിൽ (p_verbose > 0)
		last_set_സന്ദേശം(cmd->uc_script_ctx);
#endif
	    out_flush();
	    ui_breakcheck();
	    എങ്കിൽ (got_int)
		നിർത്തുക;
	}
	എങ്കിൽ (gap == &ucmds || i < gap->ga_len)
	    നിർത്തുക;
	gap = &ucmds;
    }

    എങ്കിൽ (!found)
	സന്ദേശം(_("No user-defined commands found"));

    --ucmd_locked;
}

    അക്ഷരം *
uc_fun_cmd(ശൂന്യം)
{
    സ്ഥിതം char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    തിരികെ (അക്ഷരം *)IObuff;
}

/*
 * Parse address type argument
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_addr_type_arg(
    char_u	*മൂല്യം,
    പൂർണ്ണസംഖ്യ		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T target;
    addrtype_T *entry;
    സ്ഥിതം addrtype_T *last_entry;	// cached result

    target.താക്കോൽ = 0;
    target.fullname = (അക്ഷരം *)മൂല്യം;
    target.fullnamelen = vallen;

    എങ്കിൽ (last_entry != ശൂന്യ && cmp_addr_type(&target, last_entry) == 0)
	entry = last_entry;
    അല്ലെങ്കിൽ
    {
	entry = (addrtype_T *)bsearch(&target,
	    &addr_type_complete_tab,
	    ARRAY_LENGTH(addr_type_complete_tab),
	    വലുപ്പം(addr_type_complete_tab[0]),
	    cmp_addr_type);
	എങ്കിൽ (entry == ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ i;
	    char_u	*err = മൂല്യം;

	    വേണ്ടി (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_address_type_മൂല്യം_str), err);
	    തിരികെ FAIL;
	}

	last_entry = entry;
    }

    *addr_type_arg = entry->താക്കോൽ;

    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
cmp_addr_type(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    തിരികെ STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    പൂർണ്ണസംഖ്യ
parse_compl_arg(
    char_u	*മൂല്യം,
    പൂർണ്ണസംഖ്യ		vallen,
    പൂർണ്ണസംഖ്യ		*complp,
    long	*argt,
    char_u	**compl_arg UNUSED)
{
    char_u	*arg = ശൂന്യ;
#എങ്കിൽ defined(FEAT_EVAL)
    size_t	arglen = 0;
#endif
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		valend = vallen;
    keyvalue_T	target;
    keyvalue_T	*entry;
    സ്ഥിതം keyvalue_T	*last_entry = ശൂന്യ;	    // cached result

    // Look for any argument part - which is the part after any ','
    വേണ്ടി (i = 0; i < vallen; ++i)
    {
	എങ്കിൽ (മൂല്യം[i] == ',')
	{
	    arg = &മൂല്യം[i + 1];
#എങ്കിൽ defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#endif
	    valend = i;
	    നിർത്തുക;
	}
    }

    target.താക്കോൽ = 0;
    target.മൂല്യം.string = മൂല്യം;
    target.മൂല്യം.length = valend;

    എങ്കിൽ (last_entry != ശൂന്യ && cmp_keyvalue_മൂല്യം_n(&target, last_entry) == 0)
	entry = last_entry;
    അല്ലെങ്കിൽ
    {
	entry = (keyvalue_T *)bsearch(&target,
	    &command_complete_tab,
	    ARRAY_LENGTH(command_complete_tab),
	    വലുപ്പം(command_complete_tab[0]),
	    cmp_keyvalue_മൂല്യം_n);
	എങ്കിൽ (entry == ശൂന്യ)
	{
	    semsg(_(e_invalid_complete_മൂല്യം_str), മൂല്യം);
	    തിരികെ FAIL;
	}

	last_entry = entry;
    }

    *complp = entry->താക്കോൽ;
    എങ്കിൽ (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    അല്ലെങ്കിൽ എങ്കിൽ (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    എങ്കിൽ (
#എങ്കിൽ defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#endif
								arg != ശൂന്യ)
    {
	emsg(_(e_completion_argument_only_allowed_for_custom_completion));
	തിരികെ FAIL;
    }

#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == ശൂന്യ)
    {
	emsg(_(e_custom_completion_requires_function_argument));
	തിരികെ FAIL;
    }

    എങ്കിൽ (arg != ശൂന്യ)
	*compl_arg = vim_strnsave(arg, arglen);
#endif

    തിരികെ OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_completeopt_arg(char_u *മൂല്യം, പൂർണ്ണസംഖ്യ vallen, പൂർണ്ണസംഖ്യ *compl_opt)
{
    char_u  *p = മൂല്യം;
    char_u  *end = മൂല്യം + vallen;
    പൂർണ്ണസംഖ്യ	    flags = 0;

    എങ്കിൽ (vallen == 0)
    {
	semsg(_(e_argument_required_for_str), "-completeopt");
	തിരികെ FAIL;
    }

    ആകുമ്പോൾ (p < end)
    {
	char_u	*comma;
	size_t	itemlen;
	പൂർണ്ണസംഖ്യ	matched = FALSE;
	പൂർണ്ണസംഖ്യ	i;

	comma = vim_strchr(p, ',');
	എങ്കിൽ (comma == ശൂന്യ || comma > end)
	    itemlen = (size_t)(end - p);
	അല്ലെങ്കിൽ
	    itemlen = (size_t)(comma - p);

	എങ്കിൽ (itemlen == 0)
	{
	    semsg(_(e_invalid_മൂല്യം_for_argument_str), "completeopt");
	    തിരികെ FAIL;
	}

	വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(user_cmd_completeopt_tab); ++i)
	{
	    അക്ഷരം *name = user_cmd_completeopt_tab[i];

	    എങ്കിൽ (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		എങ്കിൽ (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (!matched)
	{
	    semsg(_(e_invalid_മൂല്യം_for_argument_str), "completeopt");
	    തിരികെ FAIL;
	}

	p += itemlen;
	എങ്കിൽ (p < end && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    തിരികെ OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
uc_scan_attr(
    char_u	*attr,
    size_t	len,
    long	*argt,
    long	*def,
    പൂർണ്ണസംഖ്യ		*flags,
    പൂർണ്ണസംഖ്യ		*complp,
    char_u	**compl_arg,
    പൂർണ്ണസംഖ്യ		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    എങ്കിൽ (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	തിരികെ FAIL;
    }

    // First, try the simple attributes (no arguments)
    എങ്കിൽ (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_BUFFER;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	i;
	char_u	*val = ശൂന്യ;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)len; ++i)
	{
	    എങ്കിൽ (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		നിർത്തുക;
	    }
	}

	എങ്കിൽ (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    എങ്കിൽ (vallen == 1)
	    {
		എങ്കിൽ (*val == '0')
		    // Do nothing - this is the default
		    ;
		അല്ലെങ്കിൽ എങ്കിൽ (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		അല്ലെങ്കിൽ എങ്കിൽ (*val == '*')
		    *argt |= EX_EXTRA;
		അല്ലെങ്കിൽ എങ്കിൽ (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		അല്ലെങ്കിൽ എങ്കിൽ (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		അല്ലെങ്കിൽ എങ്കിൽ (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		അല്ലെങ്കിൽ
		    പോകുക wrong_nargs;
	    }
	    അല്ലെങ്കിൽ
	    {
wrong_nargs:
		emsg(_(e_invalid_സംഖ്യ_of_arguments));
		തിരികെ FAIL;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    എങ്കിൽ (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    അല്ലെങ്കിൽ എങ്കിൽ (val != ശൂന്യ)
	    {
		p = val;
		എങ്കിൽ (*def >= 0)
		{
two_എണ്ണം:
		    emsg(_(e_എണ്ണം_cannot_be_specified_twice));
		    തിരികെ FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		എങ്കിൽ (p != val + vallen || vallen == 0)
		{
invalid_എണ്ണം:
		    emsg(_(e_invalid_default_മൂല്യം_for_എണ്ണം));
		    തിരികെ FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    എങ്കിൽ (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_വരികൾ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_എണ്ണം | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    എങ്കിൽ (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    എങ്കിൽ (val != ശൂന്യ)
	    {
		p = val;
		എങ്കിൽ (*def >= 0)
		    പോകുക two_എണ്ണം;

		*def = getdigits(&p);

		എങ്കിൽ (p != val + vallen)
		    പോകുക invalid_എണ്ണം;
	    }

	    എങ്കിൽ (*def < 0)
		*def = 0;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    എങ്കിൽ (val == ശൂന്യ)
	    {
		semsg(_(e_argument_required_for_str), "-complete");
		തിരികെ FAIL;
	    }

	    എങ്കിൽ (parse_compl_arg(val, (പൂർണ്ണസംഖ്യ)vallen, complp, argt, compl_arg)
								      == FAIL)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    എങ്കിൽ (val == ശൂന്യ)
	    {
		semsg(_(e_argument_required_for_str), "-completeopt");
		തിരികെ FAIL;
	    }

	    എങ്കിൽ (parse_completeopt_arg(val, (പൂർണ്ണസംഖ്യ)vallen, compl_opt) == FAIL)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    എങ്കിൽ (val == ശൂന്യ)
	    {
		semsg(_(e_argument_required_for_str), "-addr");
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (parse_addr_type_arg(val, (പൂർണ്ണസംഖ്യ)vallen, addr_type_arg) == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (*addr_type_arg != ADDR_വരികൾ)
		*argt |= EX_ZEROR;
	}
	അല്ലെങ്കിൽ
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    തിരികെ FAIL;
	}
    }

    തിരികെ OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
uc_add_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    long	argt,
    long	def,
    പൂർണ്ണസംഖ്യ		flags,
    പൂർണ്ണസംഖ്യ		compl,
    char_u	*compl_arg UNUSED,
    പൂർണ്ണസംഖ്യ		compl_opt,
    cmd_addr_T	addr_type,
    പൂർണ്ണസംഖ്യ		force)
{
    ucmd_T	*cmd = ശൂന്യ;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		cmp = 1;
    char_u	*rep_buf = ശൂന്യ;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, ശൂന്യ);
    എങ്കിൽ (rep_buf == ശൂന്യ)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	എങ്കിൽ (rep_buf == ശൂന്യ)
	    തിരികെ FAIL;
    }

    // get address of growarray: global or in curbuf
    എങ്കിൽ (flags & UC_BUFFER)
    {
	gap = &curbuf->b_ucmds;
	എങ്കിൽ (gap->ga_itemsize == 0)
	    ga_init2(gap, വലുപ്പം(ucmd_T), 4);
    }
    അല്ലെങ്കിൽ
	gap = &ucmds;

    // Search for the command in the already defined commands.
    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	എങ്കിൽ (cmp == 0)
	{
	    എങ്കിൽ (name_len < cmd->uc_namelen)
		cmp = -1;
	    അല്ലെങ്കിൽ എങ്കിൽ (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	എങ്കിൽ (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    എങ്കിൽ (!force
#ifdef FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#endif
		    )
	    {
		semsg(_(e_command_already_exists_add_bang_to_replace_it_str),
									 name);
		പോകുക fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#എങ്കിൽ defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#endif
	    നിർത്തുക;
	}

	// Stop as soon as we pass the name to add
	എങ്കിൽ (cmp < 0)
	    നിർത്തുക;
    }

    // Extend the array unless we're replacing an existing command
    എങ്കിൽ (cmp != 0)
    {
	എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	    പോകുക fail;
	എങ്കിൽ ((p = vim_strnsave(name, name_len)) == ശൂന്യ)
	    പോകുക fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * വലുപ്പം(ucmd_T));

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
    എങ്കിൽ (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_version = SCRIPT_VERSION_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ifdef FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#endif
    cmd->uc_addr_type = addr_type;

    തിരികെ OK;

fail:
    vim_free(rep_buf);
#എങ്കിൽ defined(FEAT_EVAL)
    vim_free(compl_arg);
#endif
    തിരികെ FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_block(exarg_T *eap, char_u *p, char_u **tofree, പൂർണ്ണസംഖ്യ *flags)
{
    char_u *retp = p;
    char_u *block = find_cmd_block_start(p);

    എങ്കിൽ (block != ശൂന്യ && eap->ea_getline != ശൂന്യ)
    {
	garray_T    ga;
	char_u	    *വരി = ശൂന്യ;

	ga_init2(&ga, വലുപ്പം(char_u *), 10);
	എങ്കിൽ (ga_copy_string(&ga, p) == FAIL)
	    തിരികെ retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	എങ്കിൽ (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    വേണ്ടി (;;)
	    {
		vim_free(വരി);
		എങ്കിൽ ((വരി = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == ശൂന്യ)
		{
		    emsg(_(e_missing_rcurly));
		    നിർത്തുക;
		}
		എങ്കിൽ (ga_copy_string(&ga, വരി) == FAIL)
		    നിർത്തുക;
		എങ്കിൽ (*skipwhite(വരി) == '}')
		    നിർത്തുക;
	    }
	vim_free(വരി);
	retp = *tofree = ga_concat_strings(&ga, "\n");
	എങ്കിൽ (retp == ശൂന്യ)
	    retp = p;
	ga_clear_strings(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	എങ്കിൽ (block == p)
	    *flags |= UC_VIM9;
    }
    തിരികെ retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    ശൂന്യം
skip_cmd_block(exarg_T *eap)
{
    char_u	*tofree = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		flags = 0;

    (ശൂന്യം)may_get_cmd_block(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    ശൂന്യം
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*end;
    char_u	*p;
    long	argt = 0;
    long	def = -1;
    പൂർണ്ണസംഖ്യ		flags = 0;
    പൂർണ്ണസംഖ്യ		compl = EXPAND_NOTHING;
    char_u	*compl_arg = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    പൂർണ്ണസംഖ്യ		has_attr = (eap->arg[0] == '-');
    പൂർണ്ണസംഖ്യ		name_len;

    എങ്കിൽ (eap->skip)
    {
	skip_cmd_block(eap);
	തിരികെ;
    }

    p = eap->arg;

    // Check for attributes
    ആകുമ്പോൾ (*p == '-')
    {
	++p;
	end = skiptowhite(p);
	എങ്കിൽ (uc_scan_attr(p, end - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    പോകുക theend;
	p = skipwhite(end);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    എങ്കിൽ (ASCII_ISALPHA(*p))
	ആകുമ്പോൾ (ASCII_ISALNUM(*p))
	    ++p;
    എങ്കിൽ (!ends_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	പോകുക theend;
    }
    end = p;
    name_len = (പൂർണ്ണസംഖ്യ)(end - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(end);
    എങ്കിൽ (!has_attr && ends_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_start_with_an_uppercase_letter));
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_used_for_user_defined_command));
    }
    അല്ലെങ്കിൽ എങ്കിൽ (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	എങ്കിൽ (in_vim9script())
	    emsg(_(e_complete_used_without_allowing_arguments));
	അല്ലെങ്കിൽ
	    give_warning_with_source(
		       (char_u *)_(e_complete_used_without_allowing_arguments),
								   TRUE, TRUE);
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_used_with_nargs_underscore));
    }
    അല്ലെങ്കിൽ
    {
	char_u *tofree = ശൂന്യ;

	p = may_get_cmd_block(eap, p, &tofree, &flags);

	uc_add_command(name, end - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	തിരികെ;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    ശൂന്യം
ex_comclear(exarg_T *eap UNUSED)
{
    uc_clear(&ucmds);
    എങ്കിൽ (curbuf != ശൂന്യ)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_ucmd_locked(ശൂന്യം)
{
    എങ്കിൽ (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    ശൂന്യം
uc_clear(garray_T *gap)
{
    പൂർണ്ണസംഖ്യ		i;
    ucmd_T	*cmd;

    എങ്കിൽ (is_ucmd_locked())
	തിരികെ;

    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#എങ്കിൽ defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#endif
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    ശൂന്യം
ex_delcommand(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		i = 0;
    ucmd_T	*cmd = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    പൂർണ്ണസംഖ്യ		buffer_only = FALSE;

    എങ്കിൽ (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	buffer_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    വേണ്ടി (;;)
    {
	വേണ്ടി (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    എങ്കിൽ (res <= 0)
		നിർത്തുക;
	}
	എങ്കിൽ (gap == &ucmds || res == 0 || buffer_only)
	    നിർത്തുക;
	gap = &ucmds;
    }

    എങ്കിൽ (res != 0)
    {
	semsg(_(buffer_only
		    ? e_no_such_user_defined_command_in_current_buffer_str
		    : e_no_such_user_defined_command_str), arg);
	തിരികെ;
    }

    എങ്കിൽ (is_ucmd_locked())
	തിരികെ;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#എങ്കിൽ defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#endif

    --gap->ga_len;

    എങ്കിൽ (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * വലുപ്പം(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    സ്ഥിതം char_u *
uc_split_args(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    പൂർണ്ണസംഖ്യ len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    ആകുമ്പോൾ (*p)
    {
	എങ്കിൽ (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    എങ്കിൽ (*p == NUL)
		നിർത്തുക;
	    len += 4; // ", "
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    എങ്കിൽ (buf == ശൂന്യ)
    {
	*lenp = 0;
	തിരികെ buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    ആകുമ്പോൾ (*p)
    {
	എങ്കിൽ (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    എങ്കിൽ (*p == NUL)
		നിർത്തുക;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	അല്ലെങ്കിൽ
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    തിരികെ buf;
}

    സ്ഥിതം size_t
add_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    അക്ഷരം	*mod_str,
    size_t	mod_strlen,
    പൂർണ്ണസംഖ്യ		*multi_mods)
{
    എങ്കിൽ (buf != ശൂന്യ)
    {
	എങ്കിൽ (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    എങ്കിൽ (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    അല്ലെങ്കിൽ
	*multi_mods = 1;

    തിരികെ mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
add_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, പൂർണ്ണസംഖ്യ *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    എങ്കിൽ (cmod->cmod_split & WSP_ABOVE)
	buflen += add_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    എങ്കിൽ (cmod->cmod_split & WSP_BELOW)
	buflen += add_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    എങ്കിൽ (cmod->cmod_split & WSP_BOT)
	buflen += add_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    എങ്കിൽ (cmod->cmod_tab > 0)
    {
	പൂർണ്ണസംഖ്യ tabnr = cmod->cmod_tab - 1;

	എങ്കിൽ (tabnr == tabpage_index(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += add_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	അല്ലെങ്കിൽ
	{
	    അക്ഷരം tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, വലുപ്പം(tab_buf), "%dtab", tabnr);
	    buflen += add_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    എങ്കിൽ (cmod->cmod_split & WSP_മുകൾ)
	buflen += add_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    എങ്കിൽ (cmod->cmod_split & WSP_VERT)
	buflen += add_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    എങ്കിൽ (cmod->cmod_split & WSP_HOR)
	buflen += add_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    തിരികെ buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, പൂർണ്ണസംഖ്യ quote)
{
    size_t  buflen = 0;
    പൂർണ്ണസംഖ്യ	    multi_mods = 0;
    പൂർണ്ണസംഖ്യ	    i;
    സ്ഥിതം keyvalue_T mod_entry_tab[] =
    {
#ifdef FEAT_BROWSE_CMD
	KEYVALUE_ENTRY(CMOD_BROWSE, "browse"),
#endif
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
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

    എങ്കിൽ (quote)
    {
	++buflen;
	എങ്കിൽ (buf != ശൂന്യ)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    അല്ലെങ്കിൽ
    എങ്കിൽ (buf != ശൂന്യ)
	*buf = NUL;

    // the modifiers that are simple flags
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(mod_entry_tab); ++i)
	എങ്കിൽ (cmod->cmod_flags & mod_entry_tab[i].താക്കോൽ)
	    buflen += add_cmd_modifier(buf, buflen,
		    (അക്ഷരം *)mod_entry_tab[i].മൂല്യം.string,
		    mod_entry_tab[i].മൂല്യം.length, &multi_mods);

    // :silent
    എങ്കിൽ (cmod->cmod_flags & CMOD_SILENT)
    {
	എങ്കിൽ (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += add_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	അല്ലെങ്കിൽ
	    buflen += add_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    എങ്കിൽ (cmod->cmod_verbose > 0)
    {
	പൂർണ്ണസംഖ്യ verbose_മൂല്യം = cmod->cmod_verbose - 1;

	എങ്കിൽ (verbose_മൂല്യം == 1)
	    buflen += add_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	അല്ലെങ്കിൽ
	{
	    അക്ഷരം verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, വലുപ്പം(verbose_buf), "%dverbose", verbose_മൂല്യം);
	    buflen += add_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += add_win_cmd_modifiers((buf == ശൂന്യ) ? ശൂന്യ : buf + buflen, cmod, &multi_mods);

    എങ്കിൽ (quote)
    {
	എങ്കിൽ (buf == ശൂന്യ)
	    ++buflen;
	അല്ലെങ്കിൽ
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    തിരികെ buflen;
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
    സ്ഥിതം size_t
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
    പൂർണ്ണസംഖ്യ		quote = 0;
    എണ്ണൽ {
	ct_ARGS,
	ct_BANG,
	ct_എണ്ണം,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_REGISTER,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    എങ്കിൽ ((vim_strchr((char_u *)"qQfF", *p) != ശൂന്യ) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    എങ്കിൽ (l <= 1)
	type = ct_NONE;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(p, "args>", l) == 0)
	type = ct_ARGS;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(p, "count>", l) == 0)
	type = ct_എണ്ണം;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_REGISTER;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    തിരഞ്ഞെടുപ്പ് (type)
    {
    അവസ്ഥ ct_ARGS:
	// Simple case first
	എങ്കിൽ (*eap->arg == NUL)
	{
	    എങ്കിൽ (quote == 1)
	    {
		result = 2;
		എങ്കിൽ (buf != ശൂന്യ)
		    STRCPY(buf, "''");
	    }
	    അല്ലെങ്കിൽ
		result = 0;
	    നിർത്തുക;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	എങ്കിൽ ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	തിരഞ്ഞെടുപ്പ് (quote)
	{
	അവസ്ഥ 0: // No quoting, no splitting
	    result = STRLEN(eap->arg);
	    എങ്കിൽ (buf != ശൂന്യ)
		STRCPY(buf, eap->arg);
	    നിർത്തുക;
	അവസ്ഥ 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    പൂർണ്ണസംഖ്യ	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    result = STRLEN(eap->arg) + 2;
	    വേണ്ടി (p = eap->arg; *p; ++p)
	    {
		എങ്കിൽ (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		അല്ലെങ്കിൽ എങ്കിൽ (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    എങ്കിൽ (VIM_ISWHITE(p[1]))
			--result;
		    ++p;
		}
		അല്ലെങ്കിൽ
		     എങ്കിൽ (*p == '\\' || *p == '"')
		    ++result;
	    }

	    എങ്കിൽ (buf != ശൂന്യ)
	    {
		*buf++ = '"';
		വേണ്ടി (p = eap->arg; *p; ++p)
		{
		    എങ്കിൽ (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    അല്ലെങ്കിൽ എങ്കിൽ (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			എങ്കിൽ (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    അല്ലെങ്കിൽ
			 എങ്കിൽ (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    നിർത്തുക;
	}
	അവസ്ഥ 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    എങ്കിൽ (*split_buf == ശൂന്യ)
		*split_buf = uc_split_args(eap->arg, split_len);

	    result = *split_len;
	    എങ്കിൽ (buf != ശൂന്യ && result != 0)
		STRCPY(buf, *split_buf);

	    നിർത്തുക;
	}
	നിർത്തുക;

    അവസ്ഥ ct_BANG:
	result = eap->forceit ? 1 : 0;
	എങ്കിൽ (quote)
	    result += 2;
	എങ്കിൽ (buf != ശൂന്യ)
	{
	    എങ്കിൽ (quote)
		*buf++ = '"';
	    എങ്കിൽ (eap->forceit)
		*buf++ = '!';
	    എങ്കിൽ (quote)
		*buf = '"';
	}
	നിർത്തുക;

    അവസ്ഥ ct_LINE1:
    അവസ്ഥ ct_LINE2:
    അവസ്ഥ ct_RANGE:
    അവസ്ഥ ct_എണ്ണം:
    {
	അക്ഷരം അക്കം_buf[NUMBUFLEN];
	long അക്കം = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_എണ്ണം :
		   (eap->addr_എണ്ണം > 0) ? eap->line2 : cmd->uc_def;
	size_t അക്കം_len;

	അക്കം_len = vim_snprintf(അക്കം_buf, വലുപ്പം(അക്കം_buf), "%ld", അക്കം);
	result = അക്കം_len;

	എങ്കിൽ (quote)
	    result += 2;

	എങ്കിൽ (buf != ശൂന്യ)
	{
	    എങ്കിൽ (quote)
		*buf++ = '"';
	    STRCPY(buf, അക്കം_buf);
	    buf += അക്കം_len;
	    എങ്കിൽ (quote)
		*buf = '"';
	}

	നിർത്തുക;
    }

    അവസ്ഥ ct_MODS:
    {
	result = produce_cmdmods(buf, &cmdmod, quote);
	നിർത്തുക;
    }

    അവസ്ഥ ct_REGISTER:
	result = eap->regname ? 1 : 0;
	എങ്കിൽ (quote)
	    result += 2;
	എങ്കിൽ (buf != ശൂന്യ)
	{
	    എങ്കിൽ (quote)
		*buf++ = '\'';
	    എങ്കിൽ (eap->regname)
		*buf++ = eap->regname;
	    എങ്കിൽ (quote)
		*buf = '\'';
	}
	നിർത്തുക;

    അവസ്ഥ ct_LT:
	result = 1;
	എങ്കിൽ (buf != ശൂന്യ)
	    *buf = '<';
	നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
	// Not recognized: just copy the '<' and return -1.
	result = (size_t)-1;
	എങ്കിൽ (buf != ശൂന്യ)
	    *buf = '<';
	നിർത്തുക;
    }

    തിരികെ result;
}

/*
 * Execute a user defined command.
 */
    ശൂന്യം
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*start;
    char_u	*end = ശൂന്യ;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	split_len = 0;
    char_u	*split_buf = ശൂന്യ;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    പൂർണ്ണസംഖ്യ		restore_current_sctx = FALSE;
#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		restore_script_version = 0;
#endif

    എങ്കിൽ (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    അല്ലെങ്കിൽ
	cmd = USER_CMD_GA(&prevwin_curwin()->w_buffer->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = ശൂന്യ;
    വേണ്ടി (;;)
    {
	p = cmd->uc_rep;    // source
	q = buf;	    // destination
	totlen = 0;

	വേണ്ടി (;;)
	{
	    start = vim_strchr(p, '<');
	    എങ്കിൽ (start != ശൂന്യ)
		end = vim_strchr(start + 1, '>');
	    എങ്കിൽ (buf != ശൂന്യ)
	    {
		വേണ്ടി (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		എങ്കിൽ (*ksp == K_SPECIAL
			&& (start == ശൂന്യ || ksp < start || end == ശൂന്യ)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ifdef FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (പൂർണ്ണസംഖ്യ)KE_CSI)
#endif
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    എങ്കിൽ (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    തുടരുക;
		}
	    }

	    // break if no <item> is found
	    എങ്കിൽ (start == ശൂന്യ || end == ശൂന്യ)
		നിർത്തുക;

	    // Include the '>'
	    ++end;

	    // Take everything up to the '<'
	    len = start - p;
	    എങ്കിൽ (buf == ശൂന്യ)
		totlen += len;
	    അല്ലെങ്കിൽ
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_code(start, end - start, q, cmd, eap,
			     &split_buf, &split_len);
	    എങ്കിൽ (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = start + 1;
		len = 1;
	    }
	    അല്ലെങ്കിൽ
		p = end;
	    എങ്കിൽ (buf == ശൂന്യ)
		totlen += len;
	    അല്ലെങ്കിൽ
		q += len;
	}
	എങ്കിൽ (buf != ശൂന്യ)	    // second time here, finished
	{
	    STRCPY(q, p);
	    നിർത്തുക;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	എങ്കിൽ (buf == ശൂന്യ)
	{
	    vim_free(split_buf);
	    തിരികെ;
	}
    }

    എങ്കിൽ ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_version = cmd->uc_script_ctx.sc_version;
#ifdef FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	എങ്കിൽ (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_version =
				  SCRIPT_ITEM(current_sctx.sc_sid)->sn_version;
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = SCRIPT_VERSION_VIM9;
	}
#endif
    }

    (ശൂന്യം)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    എങ്കിൽ (restore_current_sctx)
    {
#ifdef FEAT_EVAL
	എങ്കിൽ (restore_script_version != 0)
	    SCRIPT_ITEM(current_sctx.sc_sid)->sn_version =
							restore_script_version;
#endif
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(split_buf);
}
