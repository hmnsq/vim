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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ucmd
{
    char_u	*uc_name;	// The command name
    size_t	uc_namelen;	// The length of the command name (excluding the NUL)
    long_u	uc_argt;	// The argument type
    char_u	*uc_rep;	// The command's replacement string
    ਲੰਮਾ	uc_def;		// The default value for a range/count
    ਪੂਰਨ		uc_compl;	// completion type
    cmd_addr_T	uc_addr_type;	// The command's address type
    sctx_T	uc_script_ctx;	// SCTX where the command was defined
    ਪੂਰਨ		uc_flags;	// some UC_ flags
    ਪੂਰਨ		uc_compl_opt;	// completion options (UCC_ flags)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    char_u	*uc_compl_arg;	// completion argument if any
#ਜੇ_ਅੰਤ
} ucmd_T;

// List of all user commands.
ਸਥਿਰ garray_T ucmds = {0, 0, ਆਕਾਰ(ucmd_T), 4, ਨਿਰਾ};

// When non-zero it is not allowed to add or remove user commands
ਸਥਿਰ ਪੂਰਨ ucmd_locked = 0;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USER_CMD(i) (&((ucmd_T *)(ucmds.ga_ਅੰਕੜੇ))[i])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USER_CMD_GA(gap, i) (&((ucmd_T *)((gap)->ga_ਅੰਕੜੇ))[i])

/*
 * List of names for completion for ":command" with the EXPAND_ flag.
 * Must be alphabetical on the 'value' field for completion and because
 * it is used by bsearch()!
 */
ਸਥਿਰ keyvalue_T command_complete_tab[] =
{
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_ARGLIST, "arglist"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_AUGROUP, "augroup"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_BEHAVE, "behave"),
#ਜੇ defined(FEAT_EVAL)
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_BREAKPOINT, "breakpoint"),
#ਜੇ_ਅੰਤ
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_BUFFERS, "buffer"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_COLORS, "color"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_COMMANDS, "command"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_COMPILER, "compiler"),
#ਜੇ defined(FEAT_CSCOPE)
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_CSCOPE, "cscope"),
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_EVAL)
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_USER_DEFINED, "custom"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_USER_LIST, "customlist"),
#ਜੇ_ਅੰਤ
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_DIFF_BUFFERS, "diff_buffer"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_DIRECTORIES, "dir"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_DIRS_ਵਿੱਚ_CDPATH, "dir_in_path"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_ENV_VARS, "environment"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_EVENTS, "event"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_ਵਿਅੰਜਕ, "expression"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_FILES, "file"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_FILES_ਵਿੱਚ_ਰਸਤਾ, "file_in_path"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_FILETYPE, "filetype"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_FILETYPECMD, "filetypecmd"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_FUNCTIONS, "function"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_HELP, "help"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_HIGHLIGHT, "highlight"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_HISTORY, "history"),
#ਜੇ defined(FEAT_KEYMAP)
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_KEYMAP, "keymap"),
#ਜੇ_ਅੰਤ
#ਜੇ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_LOCALES, "locale"),
#ਜੇ_ਅੰਤ
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_MAPCLEAR, "mapclear"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_MAPPINGS, "mapping"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_MENUS, "menu"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_MESSAGES, "messages"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_SETTINGS, "option"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_PACKADD, "packadd"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_RETAB, "retab"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_RUNTIME, "runtime"),
#ਜੇ defined(FEAT_EVAL)
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_SCRIPTNAMES, "scriptnames"),
#ਜੇ_ਅੰਤ
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_SHELLCMD, "shellcmd"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_SHELLCMDLINE, "shellcmdline"),
#ਜੇ defined(FEAT_SIGNS)
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_SIGN, "sign"),
#ਜੇ_ਅੰਤ
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_OWNSYNTAX, "syntax"),
#ਜੇ defined(FEAT_PROFILE)
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_SYNTIME, "syntime"),
#ਜੇ_ਅੰਤ
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_ਟੈਗ_ਸੂਚੀ, "tag"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_ਟੈਗ_ਸੂਚੀ_LISTFILES, "tag_listfiles"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_USER, "user"),
    KEYVALUE_ਇੰਦਰਾਜ(EXPAND_USER_VARS, "var")
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    cmd_addr_T ਕੁੰਜੀ;
    ਅੱਖਰ *fullname;
    size_t fullnamelen;
    ਅੱਖਰ *shortname;
    size_t shortnamelen;
} addrtype_T;

/*
 * List of names of address types.  Must be alphabetical for completion.
 * Must be sorted by the 'fullname' field because it is used by bsearch()!
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ADDRTYPE_ਇੰਦਰਾਜ(k, fn, sn) \
	{(k), (fn), STRLEN_LITERAL(fn), (sn), STRLEN_LITERAL(sn)}
ਸਥਿਰ addrtype_T addr_type_complete_tab[] =
{
    ADDRTYPE_ਇੰਦਰਾਜ(ADDR_ARGUMENTS, "arguments", "arg"),
    ADDRTYPE_ਇੰਦਰਾਜ(ADDR_BUFFERS, "buffers", "buf"),
    ADDRTYPE_ਇੰਦਰਾਜ(ADDR_ਸਤਰਾਂ, "lines", "line"),
    ADDRTYPE_ਇੰਦਰਾਜ(ADDR_LOADED_BUFFERS, "loaded_buffers", "load"),
    ADDRTYPE_ਇੰਦਰਾਜ(ADDR_OTHER, "other", "?"),
    ADDRTYPE_ਇੰਦਰਾਜ(ADDR_QUICKFIX, "quickfix", "qf"),
    ADDRTYPE_ਇੰਦਰਾਜ(ADDR_TABS, "tabs", "tab"),
    ADDRTYPE_ਇੰਦਰਾਜ(ADDR_WINDOWS, "windows", "win")
};

ਸਥਿਰ ਪੂਰਨ cmp_addr_type(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b);

/*
 * Search for a user command that matches "eap->cmd".
 * Return cmdidx in "eap->cmdidx", flags in "eap->argt", idx in "eap->useridx".
 * Return a pointer to just after the command.
 * Return NULL if there is no matching command.
 */
    char_u *
ਲੱਭੋ_ucmd(
    exarg_T	*eap,
    char_u	*p,	 // end of the command (possibly including count)
    ਪੂਰਨ		*ਭਰਿਆ,	 // set to TRUE for a full match
    expand_T	*xp,	 // used for completion, NULL otherwise
    ਪੂਰਨ		*complp) // completion flags or NULL
{
    ਪੂਰਨ		len = (ਪੂਰਨ)(p - eap->cmd);
    ਪੂਰਨ		j, k, matchlen = 0;
    ucmd_T	*uc;
    ਪੂਰਨ		ਮਿਲਿਆ = FALSE;
    ਪੂਰਨ		possible = FALSE;
    char_u	*cp, *np;	    // Point into typed cmd and test name
    garray_T	*gap;
    ਪੂਰਨ		amb_local = FALSE;  // Found ambiguous buffer-local command,
				    // only full match global is accepted.

    /*
     * Look for buffer-local user commands first, then global ones.
     */
    gap = &prevwin_curwin()->w_ਬਫਰ->b_ucmds;
    ਲਈ (;;)
    {
	ਲਈ (j = 0; j < gap->ga_len; ++j)
	{
	    uc = USER_CMD_GA(gap, j);
	    cp = eap->cmd;
	    np = uc->uc_name;
	    k = 0;
	    ਜਦੋਂ (k < len && *np != NUL && *cp++ == *np++)
		k++;
	    ਜੇ (k == len || (*np == NUL && vim_isdigit(eap->cmd[k])))
	    {
		// If finding a second match, the command is ambiguous.  But
		// not if a buffer-local command wasn't a full match and a
		// global command is a full match.
		ਜੇ (k == len && ਮਿਲਿਆ && *np != NUL)
		{
		    ਜੇ (gap == &ucmds)
			ਵਾਪਸ ਨਿਰਾ;
		    amb_local = TRUE;
		}

		ਜੇ (!ਮਿਲਿਆ || (k == len && *np == NUL))
		{
		    // If we matched up to a digit, then there could
		    // be another command including the digit that we
		    // should use instead.
		    ਜੇ (k == len)
			ਮਿਲਿਆ = TRUE;
		    ਨਹੀਂਤਾਂ
			possible = TRUE;

		    ਜੇ (gap == &ucmds)
			eap->cmdidx = CMD_USER;
		    ਨਹੀਂਤਾਂ
			eap->cmdidx = CMD_USER_BUF;
		    eap->argt = (ਲੰਮਾ)uc->uc_argt;
		    eap->useridx = j;
		    eap->addr_type = uc->uc_addr_type;

		    ਜੇ (complp != ਨਿਰਾ)
			*complp = uc->uc_compl;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    ਜੇ (xp != ਨਿਰਾ)
		    {
			xp->xp_arg = uc->uc_compl_arg;
			xp->xp_complete_opt = uc->uc_compl_opt;
			xp->xp_script_ctx = uc->uc_script_ctx;
			xp->xp_script_ctx.sc_lnum += SOURCING_LNUM;
		    }
#ਜੇ_ਅੰਤ
		    // Do not search for further abbreviations
		    // if this is an exact match.
		    matchlen = k;
		    ਜੇ (k == len && *np == NUL)
		    {
			ਜੇ (ਭਰਿਆ != ਨਿਰਾ)
			    *ਭਰਿਆ = TRUE;
			amb_local = FALSE;
			ਰੋਕੋ;
		    }
		}
	    }
	}

	// Stop if we found a full match or searched all.
	ਜੇ (j < gap->ga_len || gap == &ucmds)
	    ਰੋਕੋ;
	gap = &ucmds;
    }

    // Only found ambiguous matches.
    ਜੇ (amb_local)
    {
	ਜੇ (xp != ਨਿਰਾ)
	    xp->xp_context = EXPAND_UNSUCCESSFUL;
	ਵਾਪਸ ਨਿਰਾ;
    }

    // The match we found may be followed immediately by a number.  Move "p"
    // back to point to it.
    ਜੇ (ਮਿਲਿਆ || possible)
	ਵਾਪਸ p + (matchlen - len);
    ਵਾਪਸ p;
}

/*
 * Set completion context for :command
 */
    char_u *
ਸੈੱਟ_context_ਵਿੱਚ_user_cmd(expand_T *xp, char_u *arg_ਵਿੱਚ)
{
    char_u	*arg = arg_ਵਿੱਚ;
    char_u	*p;

    // Check for attributes
    ਜਦੋਂ (*arg == '-')
    {
	arg++;	    // Skip "-"
	p = skiptowhite(arg);
	ਜੇ (*p == NUL)
	{
	    // Cursor is still in the attribute
	    p = vim_strchr(arg, '=');
	    ਜੇ (p == ਨਿਰਾ)
	    {
		// No "=", so complete attribute names
		xp->xp_context = EXPAND_USER_CMD_FLAGS;
		xp->xp_ਨਮੂਨਾ = arg;
		ਵਾਪਸ ਨਿਰਾ;
	    }

	    // For the -complete, -nargs and -addr attributes, we complete
	    // their arguments as well.
	    ਜੇ (STRNICMP(arg, "complete", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETE;
		xp->xp_ਨਮੂਨਾ = p + 1;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "completeopt", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_COMPLETEOPT;
		// xp_pattern points to the last comma-separated item being
		// typed so that completion replaces only that item.
		xp->xp_ਨਮੂਨਾ = p + 1;
		ਲਈ (char_u *c = p + 1; *c != NUL; ++c)
		    ਜੇ (*c == ',')
			xp->xp_ਨਮੂਨਾ = c + 1;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "nargs", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_NARGS;
		xp->xp_ਨਮੂਨਾ = p + 1;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "addr", p - arg) == 0)
	    {
		xp->xp_context = EXPAND_USER_ADDR_TYPE;
		xp->xp_ਨਮੂਨਾ = p + 1;
	    }
	    ਵਾਪਸ ਨਿਰਾ;
	}
	arg = skipwhite(p);
    }

    // After the attributes comes the new command name
    p = skiptowhite(arg);
    ਜੇ (*p == NUL)
    {
	xp->xp_context = EXPAND_USER_COMMANDS;
	xp->xp_ਨਮੂਨਾ = arg;
	ਵਾਪਸ ਨਿਰਾ;
    }

    // And finally comes a normal command
    ਵਾਪਸ skipwhite(p);
}

/*
 * Set the completion context for the argument of a user defined command.
 */
    char_u *
ਸੈੱਟ_context_ਵਿੱਚ_user_cmdarg(
	char_u		*cmd ਅਣਵਰਤਿਆ,
	char_u		*arg,
	ਲੰਮਾ		argt,
	ਪੂਰਨ		context,
	expand_T	*xp,
	ਪੂਰਨ		forceit)
{
    char_u	*p;

    ਜੇ (context == EXPAND_NOTHING)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (argt & EX_XFILE)
    {
	// EX_XFILE: file names are handled before this call
	ਵਾਪਸ ਨਿਰਾ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਜੇ (context == EXPAND_MENUS)
	ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_menu_cmd(xp, cmd, arg, forceit);
#ਜੇ_ਅੰਤ
    ਜੇ (context == EXPAND_COMMANDS)
	ਵਾਪਸ arg;
    ਜੇ (context == EXPAND_MAPPINGS)
	ਵਾਪਸ ਸੈੱਟ_context_ਵਿੱਚ_map_cmd(xp, (char_u *)"map", arg, forceit, FALSE,
							FALSE, CMD_map);
    // Find start of last argument.
    ਜੇ (!(argt & EX_ARGSPACE))
    {
	p = arg;
	ਜਦੋਂ (*p)
	{
	    ਜੇ (*p == ' ')
		// argument starts after a space
		arg = p + 1;
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '\\' && *(p + 1) != NUL)
		++p; // skip over escaped character
	    MB_PTR_ADV(p);
	}
    }
    xp->xp_ਨਮੂਨਾ = arg;
    xp->xp_context = context;

    ਵਾਪਸ ਨਿਰਾ;
}

    char_u *
expand_user_command_name(ਪੂਰਨ idx)
{
    ਵਾਪਸ get_user_commands(ਨਿਰਾ, idx - (ਪੂਰਨ)CMD_ਮਾਪ);
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command names.
 */
    char_u *
get_user_commands(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    // In cmdwin, the alternative buffer should be used.
    buf_T *buf = prevwin_curwin()->w_ਬਫਰ;

    ਜੇ (idx < buf->b_ucmds.ga_len)
	ਵਾਪਸ USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;

    idx -= buf->b_ucmds.ga_len;
    ਜੇ (idx < ucmds.ga_len)
    {
	ਪੂਰਨ	i;
	char_u  *name = USER_CMD(idx)->uc_name;

	ਲਈ (i = 0; i < buf->b_ucmds.ga_len; ++i)
	    ਜੇ (STRCMP(name, USER_CMD_GA(&buf->b_ucmds, i)->uc_name) == 0)
		// global command is overruled by buffer-local one
		ਵਾਪਸ (char_u *)"";
	ਵਾਪਸ name;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Get the name of user command "idx".  "cmdidx" can be CMD_USER or
 * CMD_USER_BUF.
 * Returns NULL if the command is not found.
 */
    char_u *
get_user_command_name(ਪੂਰਨ idx, ਪੂਰਨ cmdidx)
{
    ਜੇ (cmdidx == CMD_USER && idx < ucmds.ga_len)
	ਵਾਪਸ USER_CMD(idx)->uc_name;
    ਜੇ (cmdidx == CMD_USER_BUF)
    {
	// In cmdwin, the alternative buffer should be used.
	buf_T *buf = prevwin_curwin()->w_ਬਫਰ;

	ਜੇ (idx < buf->b_ucmds.ga_len)
	    ਵਾਪਸ USER_CMD_GA(&buf->b_ucmds, idx)->uc_name;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Add what getinfo() reports for the user command "idx" to "d".
 * "cmdidx" is CMD_USER or CMD_USER_BUF.
 * Returns FAIL when the command is not found.
 */
    ਪੂਰਨ
user_command_info(ਪੂਰਨ idx, ਪੂਰਨ cmdidx, dict_T *d)
{
    garray_T	*gap = cmdidx == CMD_USER_BUF
				? &prevwin_curwin()->w_ਬਫਰ->b_ucmds : &ucmds;
    ucmd_T	*uc;
    char_u	*compl;

    ਜੇ (idx >= gap->ga_len)
	ਵਾਪਸ FAIL;
    uc = USER_CMD_GA(gap, idx);
    dict_ਜੋੜੋ_string(d, "name", uc->uc_name);
    dict_ਜੋੜੋ_string(d, "kind", (char_u *)"user");
    dict_ਜੋੜੋ_bool(d, "available", TRUE);
    ex_command_attrs(d, uc->uc_argt, uc->uc_addr_type, uc->uc_def);
    dict_ਜੋੜੋ_bool(d, "buffer", cmdidx == CMD_USER_BUF);
    compl = cmdcomplete_type_ਮੰਜ਼ਿਲ_str(uc->uc_compl, uc->uc_compl_arg);
    dict_ਜੋੜੋ_string(d, "complete", compl == ਨਿਰਾ ? (char_u *)"" : compl);
    vim_free(compl);
    dict_ਜੋੜੋ_string(d, "definition", uc->uc_rep);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "sid", uc->uc_script_ctx.sc_sid);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "lnum", uc->uc_script_ctx.sc_lnum);
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Function given to ExpandGeneric() to obtain the list of user address type
 * names.
 */
    char_u *
get_user_cmd_addr_type(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx < 0 || idx >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(addr_type_complete_tab))
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ (char_u *)addr_type_complete_tab[idx].fullname;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user command
 * attributes.
 */
    char_u *
get_user_cmd_flags(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਸਥਿਰ ਅੱਖਰ *user_cmd_flags[] = {
	"addr", "bang", "bar", "buffer", "complete",
	"completeopt", "count", "nargs", "range", "register", "keepscript"
    };

    ਜੇ (idx < 0 || idx >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(user_cmd_flags))
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ (char_u *)user_cmd_flags[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for -nargs.
 */
    char_u *
get_user_cmd_nargs(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਸਥਿਰ ਅੱਖਰ *user_cmd_nargs[] = {"0", "1", "_", "*", "?", "+"};

    ਜੇ (idx < 0 || idx >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(user_cmd_nargs))
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ (char_u *)user_cmd_nargs[idx];
}

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -complete.
 */
    char_u *
get_user_cmd_complete(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx < 0 || idx >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(command_complete_tab))
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ command_complete_tab[idx].ਮੁੱਲ.string;
}

/*
 * Names of options accepted by -completeopt= for :command.  Keep in sync with
 * parse_completeopt_arg() below.
 */
ਸਥਿਰ ਅੱਖਰ *user_cmd_completeopt_tab[] = {
    "escape"
};

/*
 * Function given to ExpandGeneric() to obtain the list of values for
 * -completeopt.
 */
    char_u *
get_user_cmd_completeopt(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx < 0 || idx >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(user_cmd_completeopt_tab))
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ (char_u *)user_cmd_completeopt_tab[idx];
}

/*
 * Return the row in the command_complete_tab table that contains the given key.
 */
    ਸਥਿਰ keyvalue_T *
get_commandtype(ਪੂਰਨ expand)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(command_complete_tab); ++i)
	ਜੇ (command_complete_tab[i].ਕੁੰਜੀ == expand)
	    ਵਾਪਸ &command_complete_tab[i];

    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Get the name of completion type "expand" as an allocated string.
 * "compl_arg" is the function name for "custom" and "customlist" types.
 * Returns NULL if no completion is available or on allocation failure.
 */
    char_u *
cmdcomplete_type_ਮੰਜ਼ਿਲ_str(ਪੂਰਨ expand, char_u *compl_arg)
{
    keyvalue_T *kv;
    char_u     *cmd_compl;

    kv = get_commandtype(expand);
    ਜੇ (kv == ਨਿਰਾ || kv->ਮੁੱਲ.string == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    cmd_compl = kv->ਮੁੱਲ.string;
    ਜੇ (expand == EXPAND_USER_LIST || expand == EXPAND_USER_DEFINED)
    {
	char_u	*ਬਫਰ;

	ਬਫਰ = alloc(STRLEN(cmd_compl) + STRLEN(compl_arg) + 2);
	ਜੇ (ਬਫਰ == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	sprintf((ਅੱਖਰ *)ਬਫਰ, "%s,%s", cmd_compl, compl_arg);
	ਵਾਪਸ ਬਫਰ;
    }

    ਵਾਪਸ vim_strsave(cmd_compl);
}

/*
 * Get the index of completion type "complete_str".
 * Returns EXPAND_NOTHING if no match found.
 */
    ਪੂਰਨ
cmdcomplete_str_ਮੰਜ਼ਿਲ_type(char_u *complete_str)
{
    keyvalue_T ਨਿਸ਼ਾਨਾ;
    keyvalue_T *ਇੰਦਰਾਜ;
    ਸਥਿਰ keyvalue_T *ਆਖਰੀ_ਇੰਦਰਾਜ = ਨਿਰਾ;	// cached result

    ਜੇ (STRNCMP(complete_str, "custom,", 7) == 0)
	ਵਾਪਸ EXPAND_USER_DEFINED;
    ਜੇ (STRNCMP(complete_str, "customlist,", 11) == 0)
	ਵਾਪਸ EXPAND_USER_LIST;

    ਨਿਸ਼ਾਨਾ.ਕੁੰਜੀ = 0;
    ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.string = complete_str;
    ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.ਲੰਮਾਈ = 0;			// not used, see cmp_keyvalue_value()

    ਜੇ (ਆਖਰੀ_ਇੰਦਰਾਜ != ਨਿਰਾ && cmp_keyvalue_ਮੁੱਲ(&ਨਿਸ਼ਾਨਾ, ਆਖਰੀ_ਇੰਦਰਾਜ) == 0)
	ਇੰਦਰਾਜ = ਆਖਰੀ_ਇੰਦਰਾਜ;
    ਨਹੀਂਤਾਂ
    {
	ਇੰਦਰਾਜ = (keyvalue_T *)bsearch(&ਨਿਸ਼ਾਨਾ,
	    &command_complete_tab,
	    ARRAY_ਲੰਮਾਈ(command_complete_tab),
	    ਆਕਾਰ(command_complete_tab[0]),
	    cmp_keyvalue_ਮੁੱਲ);
	ਜੇ (ਇੰਦਰਾਜ == ਨਿਰਾ)
	    ਵਾਪਸ EXPAND_NOTHING;

	ਆਖਰੀ_ਇੰਦਰਾਜ = ਇੰਦਰਾਜ;
    }

    ਵਾਪਸ ਇੰਦਰਾਜ->ਕੁੰਜੀ;
}
#ਜੇ_ਅੰਤ

/*
 * List user commands starting with "name[name_len]".
 */
    ਸਥਿਰ ਖਾਲੀ
uc_list(char_u *name, size_t name_len)
{
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		ਮਿਲਿਆ = FALSE;
    ucmd_T	*cmd;
    ਪੂਰਨ		len;
    ਪੂਰਨ		over;
    ਲੰਮਾ	a;
    garray_T	*gap;
    keyvalue_T	*ਇੰਦਰਾਜ;

    // don't allow for adding or removing user commands here
    ++ucmd_locked;

    // In cmdwin, the alternative buffer should be used.
    gap = &prevwin_curwin()->w_ਬਫਰ->b_ucmds;
    ਲਈ (;;)
    {
	ਲਈ (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    a = (ਲੰਮਾ)cmd->uc_argt;

	    // Skip commands which don't match the requested prefix and
	    // commands filtered out.
	    ਜੇ (STRNCMP(name, cmd->uc_name, name_len) != 0
		    || ਸੁਨੇਹਾ_filtered(cmd->uc_name))
		ਜਾਰੀ;

	    // Put out the title first time
	    ਜੇ (!ਮਿਲਿਆ)
		ਸੁਨੇਹਾ_puts_title(_("\n    Name              Args Address Complete    Definition"));
	    ਮਿਲਿਆ = TRUE;
	    ਸੁਨੇਹਾ_putchar('\n');
	    ਜੇ (got_int)
		ਰੋਕੋ;

	    // Special cases
	    len = 4;
	    ਜੇ (a & EX_BANG)
	    {
		ਸੁਨੇਹਾ_putchar('!');
		--len;
	    }
	    ਜੇ (a & EX_REGSTR)
	    {
		ਸੁਨੇਹਾ_putchar('"');
		--len;
	    }
	    ਜੇ (gap != &ucmds)
	    {
		ਸੁਨੇਹਾ_putchar('b');
		--len;
	    }
	    ਜੇ (a & EX_TRLBAR)
	    {
		ਸੁਨੇਹਾ_putchar('|');
		--len;
	    }
	    ਜੇ (len != 0)
		ਸੁਨੇਹਾ_puts(&"    "[4 - len]);

	    ਸੁਨੇਹਾ_outtrans_attr(cmd->uc_name, HL_ATTR(HLF_D));
	    len = (ਪੂਰਨ)cmd->uc_namelen + 4;

	    ਜੇ (len < 21)
	    {
		// Field padding spaces   12345678901234567
		ਸਥਿਰ ਅੱਖਰ spaces[18] = "                 ";
		ਸੁਨੇਹਾ_puts(&spaces[len - 4]);
		len = 21;
	    }
	    ਸੁਨੇਹਾ_putchar(' ');
	    ++len;

	    // "over" is how much longer the name is than the column width for
	    // the name, we'll try to align what comes after.
	    over = len - 22;
	    len = 0;

	    // Arguments
	    ਚੋਣ ((ਪੂਰਨ)(a & (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE)))
	    {
		ਹਾਲਤ 0:				IObuff[len++] = '0'; ਰੋਕੋ;
		ਹਾਲਤ (EX_EXTRA):		IObuff[len++] = '*'; ਰੋਕੋ;
		ਹਾਲਤ (EX_EXTRA|EX_NOSPC):	IObuff[len++] = '?'; ਰੋਕੋ;
		ਹਾਲਤ (EX_EXTRA|EX_NEEDARG):	IObuff[len++] = '+'; ਰੋਕੋ;
		ਹਾਲਤ (EX_EXTRA|EX_NOSPC|EX_NEEDARG): IObuff[len++] = '1'; ਰੋਕੋ;
		ਹਾਲਤ (EX_EXTRA|EX_NOSPC|EX_NEEDARG|EX_ARGSPACE): IObuff[len++] = '_'; ਰੋਕੋ;
	    }

	    ਕਰੋ
	    {
		IObuff[len++] = ' ';
	    } ਜਦੋਂ (len < 5 - over);

	    // Address / Range
	    ਜੇ (a & (EX_RANGE|EX_ਗਿਣਤੀ))
	    {
		ਜੇ (a & EX_ਗਿਣਤੀ)
		{
		    // -count=N
		    len += vim_snprintf((ਅੱਖਰ *)IObuff + len, IOSIZE - len, "%ldc", cmd->uc_def);
		}
		ਨਹੀਂਤਾਂ ਜੇ (a & EX_DFLALL)
		    IObuff[len++] = '%';
		ਨਹੀਂਤਾਂ ਜੇ (cmd->uc_def >= 0)
		{
		    // -range=N
		    len += vim_snprintf((ਅੱਖਰ *)IObuff + len, IOSIZE - len, "%ld", cmd->uc_def);
		}
		ਨਹੀਂਤਾਂ
		    IObuff[len++] = '.';
	    }

	    ਕਰੋ
	    {
		IObuff[len++] = ' ';
	    } ਜਦੋਂ (len < 8 - over);

	    // Address Type
	    ਲਈ (j = 0; j < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(addr_type_complete_tab); ++j)
		ਜੇ (addr_type_complete_tab[j].ਕੁੰਜੀ != ADDR_ਸਤਰਾਂ
			&& addr_type_complete_tab[j].ਕੁੰਜੀ == cmd->uc_addr_type)
		{
		    STRCPY(IObuff + len, addr_type_complete_tab[j].shortname);
		    len += (ਪੂਰਨ)addr_type_complete_tab[j].shortnamelen;
		    ਰੋਕੋ;
		}

	    ਕਰੋ
	    {
		IObuff[len++] = ' ';
	    } ਜਦੋਂ (len < 13 - over);

	    // Completion
	    ਇੰਦਰਾਜ = get_commandtype(cmd->uc_compl);
	    ਜੇ (ਇੰਦਰਾਜ != ਨਿਰਾ)
	    {
		STRCPY(IObuff + len, ਇੰਦਰਾਜ->ਮੁੱਲ.string);
		len += (ਪੂਰਨ)ਇੰਦਰਾਜ->ਮੁੱਲ.ਲੰਮਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		ਜੇ (p_verbose > 0 && cmd->uc_compl_arg != ਨਿਰਾ)
		{
		    size_t uc_compl_arglen = STRLEN(cmd->uc_compl_arg);

		    ਜੇ (uc_compl_arglen < 200)
		    {
			IObuff[len++] = ',';
			STRCPY(IObuff + len, cmd->uc_compl_arg);
			len += (ਪੂਰਨ)uc_compl_arglen;
		    }
		}
		ਜੇ (p_verbose > 0 && (cmd->uc_compl_opt & UCC_ESCAPE))
		    len += vim_snprintf((ਅੱਖਰ *)IObuff + len, IOSIZE - len,
						       " -completeopt=escape");
#ਜੇ_ਅੰਤ
	    }

	    ਕਰੋ
	    {
		IObuff[len++] = ' ';
	    } ਜਦੋਂ (len < 25 - over);

	    IObuff[len] = NUL;
	    ਸੁਨੇਹਾ_outtrans(IObuff);

	    ਸੁਨੇਹਾ_outtrans_special(cmd->uc_rep, FALSE,
					     name_len == 0 ? Columns - 47 : 0);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਜੇ (p_verbose > 0)
		ਆਖਰੀ_ਸੈੱਟ_ਸੁਨੇਹਾ(cmd->uc_script_ctx);
#ਜੇ_ਅੰਤ
	    ਬਾਹਰਲਾ_flush();
	    ui_breakcheck();
	    ਜੇ (got_int)
		ਰੋਕੋ;
	}
	ਜੇ (gap == &ucmds || i < gap->ga_len)
	    ਰੋਕੋ;
	gap = &ucmds;
    }

    ਜੇ (!ਮਿਲਿਆ)
	ਸੁਨੇਹਾ(_("No user-defined commands found"));

    --ucmd_locked;
}

    ਅੱਖਰ *
uc_fun_cmd(ਖਾਲੀ)
{
    ਸਥਿਰ char_u fcmd[] = {0x84, 0xaf, 0x60, 0xb9, 0xaf, 0xb5, 0x60, 0xa4,
			    0xa5, 0xad, 0xa1, 0xae, 0xa4, 0x60, 0xa1, 0x60,
			    0xb3, 0xa8, 0xb2, 0xb5, 0xa2, 0xa2, 0xa5, 0xb2,
			    0xb9, 0x7f, 0};
    ਪੂਰਨ		i;

    ਲਈ (i = 0; fcmd[i]; ++i)
	IObuff[i] = fcmd[i] - 0x40;
    IObuff[i] = NUL;
    ਵਾਪਸ (ਅੱਖਰ *)IObuff;
}

/*
 * Parse address type argument
 */
    ਸਥਿਰ ਪੂਰਨ
parse_addr_type_arg(
    char_u	*ਮੁੱਲ,
    ਪੂਰਨ		vallen,
    cmd_addr_T	*addr_type_arg)
{
    addrtype_T ਨਿਸ਼ਾਨਾ;
    addrtype_T *ਇੰਦਰਾਜ;
    ਸਥਿਰ addrtype_T *ਆਖਰੀ_ਇੰਦਰਾਜ;	// cached result

    ਨਿਸ਼ਾਨਾ.ਕੁੰਜੀ = 0;
    ਨਿਸ਼ਾਨਾ.fullname = (ਅੱਖਰ *)ਮੁੱਲ;
    ਨਿਸ਼ਾਨਾ.fullnamelen = vallen;

    ਜੇ (ਆਖਰੀ_ਇੰਦਰਾਜ != ਨਿਰਾ && cmp_addr_type(&ਨਿਸ਼ਾਨਾ, ਆਖਰੀ_ਇੰਦਰਾਜ) == 0)
	ਇੰਦਰਾਜ = ਆਖਰੀ_ਇੰਦਰਾਜ;
    ਨਹੀਂਤਾਂ
    {
	ਇੰਦਰਾਜ = (addrtype_T *)bsearch(&ਨਿਸ਼ਾਨਾ,
	    &addr_type_complete_tab,
	    ARRAY_ਲੰਮਾਈ(addr_type_complete_tab),
	    ਆਕਾਰ(addr_type_complete_tab[0]),
	    cmp_addr_type);
	ਜੇ (ਇੰਦਰਾਜ == ਨਿਰਾ)
	{
	    ਪੂਰਨ i;
	    char_u	*err = ਮੁੱਲ;

	    ਲਈ (i = 0; err[i] != NUL && !VIM_ISWHITE(err[i]); i++)
		;
	    err[i] = NUL;
	    semsg(_(e_invalid_ਪਤਾ_type_ਮੁੱਲ_str), err);
	    ਵਾਪਸ FAIL;
	}

	ਆਖਰੀ_ਇੰਦਰਾਜ = ਇੰਦਰਾਜ;
    }

    *addr_type_arg = ਇੰਦਰਾਜ->ਕੁੰਜੀ;

    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
cmp_addr_type(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
{
    addrtype_T *at1 = (addrtype_T *)a;
    addrtype_T *at2 = (addrtype_T *)b;

    ਵਾਪਸ STRNCMP(at1->fullname, at2->fullname, MAX(at1->fullnamelen, at2->fullnamelen));
}

/*
 * Parse a completion argument "value[vallen]".
 * The detected completion goes in "*complp", argument type in "*argt".
 * When there is an argument, for function and user defined completion, it's
 * copied to allocated memory and stored in "*compl_arg".
 * Returns FAIL if something is wrong.
 */
    ਪੂਰਨ
parse_compl_arg(
    char_u	*ਮੁੱਲ,
    ਪੂਰਨ		vallen,
    ਪੂਰਨ		*complp,
    ਲੰਮਾ	*argt,
    char_u	**compl_arg ਅਣਵਰਤਿਆ)
{
    char_u	*arg = ਨਿਰਾ;
#ਜੇ defined(FEAT_EVAL)
    size_t	arglen = 0;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		i;
    ਪੂਰਨ		valend = vallen;
    keyvalue_T	ਨਿਸ਼ਾਨਾ;
    keyvalue_T	*ਇੰਦਰਾਜ;
    ਸਥਿਰ keyvalue_T	*ਆਖਰੀ_ਇੰਦਰਾਜ = ਨਿਰਾ;	    // cached result

    // Look for any argument part - which is the part after any ','
    ਲਈ (i = 0; i < vallen; ++i)
    {
	ਜੇ (ਮੁੱਲ[i] == ',')
	{
	    arg = &ਮੁੱਲ[i + 1];
#ਜੇ defined(FEAT_EVAL)
	    arglen = vallen - i - 1;
#ਜੇ_ਅੰਤ
	    valend = i;
	    ਰੋਕੋ;
	}
    }

    ਨਿਸ਼ਾਨਾ.ਕੁੰਜੀ = 0;
    ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.string = ਮੁੱਲ;
    ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.ਲੰਮਾਈ = valend;

    ਜੇ (ਆਖਰੀ_ਇੰਦਰਾਜ != ਨਿਰਾ && cmp_keyvalue_ਮੁੱਲ_n(&ਨਿਸ਼ਾਨਾ, ਆਖਰੀ_ਇੰਦਰਾਜ) == 0)
	ਇੰਦਰਾਜ = ਆਖਰੀ_ਇੰਦਰਾਜ;
    ਨਹੀਂਤਾਂ
    {
	ਇੰਦਰਾਜ = (keyvalue_T *)bsearch(&ਨਿਸ਼ਾਨਾ,
	    &command_complete_tab,
	    ARRAY_ਲੰਮਾਈ(command_complete_tab),
	    ਆਕਾਰ(command_complete_tab[0]),
	    cmp_keyvalue_ਮੁੱਲ_n);
	ਜੇ (ਇੰਦਰਾਜ == ਨਿਰਾ)
	{
	    semsg(_(e_invalid_complete_ਮੁੱਲ_str), ਮੁੱਲ);
	    ਵਾਪਸ FAIL;
	}

	ਆਖਰੀ_ਇੰਦਰਾਜ = ਇੰਦਰਾਜ;
    }

    *complp = ਇੰਦਰਾਜ->ਕੁੰਜੀ;
    ਜੇ (*complp == EXPAND_BUFFERS)
	*argt |= EX_BUFNAME;
    ਨਹੀਂਤਾਂ ਜੇ (*complp == EXPAND_DIRECTORIES || *complp == EXPAND_FILES || *complp == EXPAND_SHELLCMDLINE)
	*argt |= EX_XFILE;

    ਜੇ (
#ਜੇ defined(FEAT_EVAL)
	*complp != EXPAND_USER_DEFINED && *complp != EXPAND_USER_LIST
								&&
#ਜੇ_ਅੰਤ
								arg != ਨਿਰਾ)
    {
	emsg(_(e_completion_ਦਲੀਲ_only_allowed_for_custom_completion));
	ਵਾਪਸ FAIL;
    }

#ਜੇ defined(FEAT_EVAL)
    ਜੇ ((*complp == EXPAND_USER_DEFINED || *complp == EXPAND_USER_LIST)
							       && arg == ਨਿਰਾ)
    {
	emsg(_(e_custom_completion_requires_function_ਦਲੀਲ));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (arg != ਨਿਰਾ)
	*compl_arg = vim_strnsave(arg, arglen);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

/*
 * Parse a -completeopt= value.  "value" points to a comma-separated list of
 * option names; on success the corresponding UCC_ flags are OR'ed into
 * "*compl_opt".
 * Returns FAIL on an unknown name.
 */
    ਸਥਿਰ ਪੂਰਨ
parse_completeopt_arg(char_u *ਮੁੱਲ, ਪੂਰਨ vallen, ਪੂਰਨ *compl_opt)
{
    char_u  *p = ਮੁੱਲ;
    char_u  *ਸਿਰਾ = ਮੁੱਲ + vallen;
    ਪੂਰਨ	    flags = 0;

    ਜੇ (vallen == 0)
    {
	semsg(_(e_ਦਲੀਲ_required_for_str), "-completeopt");
	ਵਾਪਸ FAIL;
    }

    ਜਦੋਂ (p < ਸਿਰਾ)
    {
	char_u	*comma;
	size_t	itemlen;
	ਪੂਰਨ	matched = FALSE;
	ਪੂਰਨ	i;

	comma = vim_strchr(p, ',');
	ਜੇ (comma == ਨਿਰਾ || comma > ਸਿਰਾ)
	    itemlen = (size_t)(ਸਿਰਾ - p);
	ਨਹੀਂਤਾਂ
	    itemlen = (size_t)(comma - p);

	ਜੇ (itemlen == 0)
	{
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "completeopt");
	    ਵਾਪਸ FAIL;
	}

	ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(user_cmd_completeopt_tab); ++i)
	{
	    ਅੱਖਰ *name = user_cmd_completeopt_tab[i];

	    ਜੇ (STRLEN(name) == itemlen
				   && STRNCMP(p, name, itemlen) == 0)
	    {
		ਜੇ (STRCMP(name, "escape") == 0)
		    flags |= UCC_ESCAPE;
		matched = TRUE;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (!matched)
	{
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "completeopt");
	    ਵਾਪਸ FAIL;
	}

	p += itemlen;
	ਜੇ (p < ਸਿਰਾ && *p == ',')
	    ++p;
    }

    *compl_opt |= flags;
    ਵਾਪਸ OK;
}

/*
 * Scan attributes in the ":command" command.
 * Return FAIL when something is wrong.
 */
    ਸਥਿਰ ਪੂਰਨ
uc_ਜਾਂਚ_attr(
    char_u	*attr,
    size_t	len,
    ਲੰਮਾ	*argt,
    ਲੰਮਾ	*def,
    ਪੂਰਨ		*flags,
    ਪੂਰਨ		*complp,
    char_u	**compl_arg,
    ਪੂਰਨ		*compl_opt,
    cmd_addr_T	*addr_type_arg)
{
    char_u	*p;

    ਜੇ (len == 0)
    {
	emsg(_(e_no_attribute_specified));
	ਵਾਪਸ FAIL;
    }

    // First, try the simple attributes (no arguments)
    ਜੇ (STRNICMP(attr, "bang", len) == 0)
	*argt |= EX_BANG;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(attr, "buffer", len) == 0)
	*flags |= UC_ਬਫਰ;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(attr, "register", len) == 0)
	*argt |= EX_REGSTR;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(attr, "keepscript", len) == 0)
	*argt |= EX_KEEPSCRIPT;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(attr, "bar", len) == 0)
	*argt |= EX_TRLBAR;
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	i;
	char_u	*val = ਨਿਰਾ;
	size_t	vallen = 0;
	size_t	attrlen = len;

	// Look for the attribute name - which is the part before any '='
	ਲਈ (i = 0; i < (ਪੂਰਨ)len; ++i)
	{
	    ਜੇ (attr[i] == '=')
	    {
		val = &attr[i + 1];
		vallen = len - i - 1;
		attrlen = i;
		ਰੋਕੋ;
	    }
	}

	ਜੇ (STRNICMP(attr, "nargs", attrlen) == 0)
	{
	    ਜੇ (vallen == 1)
	    {
		ਜੇ (*val == '0')
		    // Do nothing - this is the default
		    ;
		ਨਹੀਂਤਾਂ ਜੇ (*val == '1')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG);
		ਨਹੀਂਤਾਂ ਜੇ (*val == '*')
		    *argt |= EX_EXTRA;
		ਨਹੀਂਤਾਂ ਜੇ (*val == '?')
		    *argt |= (EX_EXTRA | EX_NOSPC);
		ਨਹੀਂਤਾਂ ਜੇ (*val == '+')
		    *argt |= (EX_EXTRA | EX_NEEDARG);
		ਨਹੀਂਤਾਂ ਜੇ (*val == '_')
		    *argt |= (EX_EXTRA | EX_NOSPC | EX_NEEDARG | EX_ARGSPACE);
		ਨਹੀਂਤਾਂ
		    ਜਾਓ wrong_nargs;
	    }
	    ਨਹੀਂਤਾਂ
	    {
wrong_nargs:
		emsg(_(e_invalid_ਸੰਖਿਆ_ਦਾ_arguments));
		ਵਾਪਸ FAIL;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(attr, "range", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    ਜੇ (vallen == 1 && *val == '%')
		*argt |= EX_DFLALL;
	    ਨਹੀਂਤਾਂ ਜੇ (val != ਨਿਰਾ)
	    {
		p = val;
		ਜੇ (*def >= 0)
		{
two_ਗਿਣਤੀ:
		    emsg(_(e_ਗਿਣਤੀ_cannot_be_specified_twice));
		    ਵਾਪਸ FAIL;
		}

		*def = getdigits(&p);
		*argt |= EX_ZEROR;

		ਜੇ (p != val + vallen || vallen == 0)
		{
invalid_ਗਿਣਤੀ:
		    emsg(_(e_invalid_default_ਮੁੱਲ_for_ਗਿਣਤੀ));
		    ਵਾਪਸ FAIL;
		}
	    }
	    // default for -range is using buffer lines
	    ਜੇ (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_ਸਤਰਾਂ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(attr, "count", attrlen) == 0)
	{
	    *argt |= (EX_ਗਿਣਤੀ | EX_ZEROR | EX_RANGE);
	    // default for -count is using any number
	    ਜੇ (*addr_type_arg == ADDR_NONE)
		*addr_type_arg = ADDR_OTHER;

	    ਜੇ (val != ਨਿਰਾ)
	    {
		p = val;
		ਜੇ (*def >= 0)
		    ਜਾਓ two_ਗਿਣਤੀ;

		*def = getdigits(&p);

		ਜੇ (p != val + vallen)
		    ਜਾਓ invalid_ਗਿਣਤੀ;
	    }

	    ਜੇ (*def < 0)
		*def = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(attr, "complete", attrlen) == 0)
	{
	    ਜੇ (val == ਨਿਰਾ)
	    {
		semsg(_(e_ਦਲੀਲ_required_for_str), "-complete");
		ਵਾਪਸ FAIL;
	    }

	    ਜੇ (parse_compl_arg(val, (ਪੂਰਨ)vallen, complp, argt, compl_arg)
								      == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(attr, "completeopt", attrlen) == 0)
	{
	    ਜੇ (val == ਨਿਰਾ)
	    {
		semsg(_(e_ਦਲੀਲ_required_for_str), "-completeopt");
		ਵਾਪਸ FAIL;
	    }

	    ਜੇ (parse_completeopt_arg(val, (ਪੂਰਨ)vallen, compl_opt) == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(attr, "addr", attrlen) == 0)
	{
	    *argt |= EX_RANGE;
	    ਜੇ (val == ਨਿਰਾ)
	    {
		semsg(_(e_ਦਲੀਲ_required_for_str), "-addr");
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (parse_addr_type_arg(val, (ਪੂਰਨ)vallen, addr_type_arg) == FAIL)
		ਵਾਪਸ FAIL;
	    ਜੇ (*addr_type_arg != ADDR_ਸਤਰਾਂ)
		*argt |= EX_ZEROR;
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u ch = attr[len];
	    attr[len] = '\0';
	    semsg(_(e_invalid_attribute_str), attr);
	    attr[len] = ch;
	    ਵਾਪਸ FAIL;
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Add a user command to the list or replace an existing one.
 */
    ਸਥਿਰ ਪੂਰਨ
uc_ਜੋੜੋ_command(
    char_u	*name,
    size_t	name_len,
    char_u	*rep,
    ਲੰਮਾ	argt,
    ਲੰਮਾ	def,
    ਪੂਰਨ		flags,
    ਪੂਰਨ		compl,
    char_u	*compl_arg ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		compl_opt,
    cmd_addr_T	addr_type,
    ਪੂਰਨ		force)
{
    ucmd_T	*cmd = ਨਿਰਾ;
    char_u	*p;
    ਪੂਰਨ		i;
    ਪੂਰਨ		cmp = 1;
    char_u	*rep_buf = ਨਿਰਾ;
    garray_T	*gap;

    replace_termcodes(rep, &rep_buf, 0, 0, ਨਿਰਾ);
    ਜੇ (rep_buf == ਨਿਰਾ)
    {
	// can't replace termcodes - try using the string as is
	rep_buf = vim_strsave(rep);

	// give up if out of memory
	ਜੇ (rep_buf == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }

    // get address of growarray: global or in curbuf
    ਜੇ (flags & UC_ਬਫਰ)
    {
	gap = &curbuf->b_ucmds;
	ਜੇ (gap->ga_itemsize == 0)
	    ga_init2(gap, ਆਕਾਰ(ucmd_T), 4);
    }
    ਨਹੀਂਤਾਂ
	gap = &ucmds;

    // Search for the command in the already defined commands.
    ਲਈ (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	cmp = STRNCMP(name, cmd->uc_name, name_len);
	ਜੇ (cmp == 0)
	{
	    ਜੇ (name_len < cmd->uc_namelen)
		cmp = -1;
	    ਨਹੀਂਤਾਂ ਜੇ (name_len > cmd->uc_namelen)
		cmp = 1;
	}

	ਜੇ (cmp == 0)
	{
	    // Command can be replaced with "command!" and when sourcing the
	    // same script again, but only once.
	    ਜੇ (!force
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    && (cmd->uc_script_ctx.sc_sid != current_sctx.sc_sid
			  || cmd->uc_script_ctx.sc_seq == current_sctx.sc_seq)
#ਜੇ_ਅੰਤ
		    )
	    {
		semsg(_(e_command_already_exists_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_replace_it_str),
									 name);
		ਜਾਓ fail;
	    }

	    VIM_CLEAR(cmd->uc_rep);
#ਜੇ defined(FEAT_EVAL)
	    VIM_CLEAR(cmd->uc_compl_arg);
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	}

	// Stop as soon as we pass the name to add
	ਜੇ (cmp < 0)
	    ਰੋਕੋ;
    }

    // Extend the array unless we're replacing an existing command
    ਜੇ (cmp != 0)
    {
	ਜੇ (ga_grow(gap, 1) == FAIL)
	    ਜਾਓ fail;
	ਜੇ ((p = vim_strnsave(name, name_len)) == ਨਿਰਾ)
	    ਜਾਓ fail;

	cmd = USER_CMD_GA(gap, i);
	mch_memmove(cmd + 1, cmd, (gap->ga_len - i) * ਆਕਾਰ(ucmd_T));

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
    ਜੇ (flags & UC_VIM9)
	cmd->uc_script_ctx.sc_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_VIM9;
    cmd->uc_flags = flags & UC_VIM9;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    cmd->uc_script_ctx.sc_lnum += SOURCING_LNUM;
    cmd->uc_compl_arg = compl_arg;
#ਜੇ_ਅੰਤ
    cmd->uc_addr_type = addr_type;

    ਵਾਪਸ OK;

fail:
    vim_free(rep_buf);
#ਜੇ defined(FEAT_EVAL)
    vim_free(compl_arg);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ FAIL;
}

/*
 * If "p" starts a block of commands, read it until "}".
 * Used for ":command" and ":autocmd".
 */
    char_u *
may_get_cmd_ਖੰਡ(exarg_T *eap, char_u *p, char_u **tofree, ਪੂਰਨ *flags)
{
    char_u *retp = p;
    char_u *ਖੰਡ = ਲੱਭੋ_cmd_ਖੰਡ_ਸ਼ੁਰੂ(p);

    ਜੇ (ਖੰਡ != ਨਿਰਾ && eap->ea_getline != ਨਿਰਾ)
    {
	garray_T    ga;
	char_u	    *ਸਤਰ = ਨਿਰਾ;

	ga_init2(&ga, ਆਕਾਰ(char_u *), 10);
	ਜੇ (ga_copy_string(&ga, p) == FAIL)
	    ਵਾਪਸ retp;

	// If the argument ends in "}" it must have been concatenated already
	// for ISN_EXEC.
	ਜੇ (p[STRLEN(p) - 1] != '}')
	    // Read lines between '{' and '}'.  Does not support nesting or
	    // here-doc constructs.
	    ਲਈ (;;)
	    {
		vim_free(ਸਤਰ);
		ਜੇ ((ਸਤਰ = eap->ea_getline(':', eap->cookie,
					   0, GETLINE_CONCAT_CONTBAR)) == ਨਿਰਾ)
		{
		    emsg(_(e_missing_rcurly));
		    ਰੋਕੋ;
		}
		ਜੇ (ga_copy_string(&ga, ਸਤਰ) == FAIL)
		    ਰੋਕੋ;
		ਜੇ (*skipwhite(ਸਤਰ) == '}')
		    ਰੋਕੋ;
	    }
	vim_free(ਸਤਰ);
	retp = *tofree = ga_concat_ਲੜੀਆਂ(&ga, "\n");
	ਜੇ (retp == ਨਿਰਾ)
	    retp = p;
	ga_clear_ਲੜੀਆਂ(&ga);
	// Only the command owning the block uses Vim9 syntax.  A command with
	// the block nested in it keeps the syntax of its script.
	ਜੇ (ਖੰਡ == p)
	    *flags |= UC_VIM9;
    }
    ਵਾਪਸ retp;
}

/*
 * Read the block of commands that may follow and throw it away.
 * Used for ":command" and ":autocmd" while skipping.
 */
    ਖਾਲੀ
skip_cmd_ਖੰਡ(exarg_T *eap)
{
    char_u	*tofree = ਨਿਰਾ;
    ਪੂਰਨ		flags = 0;

    (ਖਾਲੀ)may_get_cmd_ਖੰਡ(eap, eap->cmd, &tofree, &flags);
    vim_free(tofree);
}

/*
 * ":command ..." implementation
 */
    ਖਾਲੀ
ex_command(exarg_T *eap)
{
    char_u	*name;
    char_u	*ਸਿਰਾ;
    char_u	*p;
    ਲੰਮਾ	argt = 0;
    ਲੰਮਾ	def = -1;
    ਪੂਰਨ		flags = 0;
    ਪੂਰਨ		compl = EXPAND_NOTHING;
    char_u	*compl_arg = ਨਿਰਾ;
    ਪੂਰਨ		compl_opt = 0;
    cmd_addr_T	addr_type_arg = ADDR_NONE;
    ਪੂਰਨ		has_attr = (eap->arg[0] == '-');
    ਪੂਰਨ		name_len;

    ਜੇ (eap->skip)
    {
	skip_cmd_ਖੰਡ(eap);
	ਵਾਪਸ;
    }

    p = eap->arg;

    // Check for attributes
    ਜਦੋਂ (*p == '-')
    {
	++p;
	ਸਿਰਾ = skiptowhite(p);
	ਜੇ (uc_ਜਾਂਚ_attr(p, ਸਿਰਾ - p, &argt, &def, &flags, &compl,
			       &compl_arg, &compl_opt, &addr_type_arg) == FAIL)
	    ਜਾਓ theend;
	p = skipwhite(ਸਿਰਾ);
    }

    // Get the name (if any) and skip to the following argument
    name = p;
    ਜੇ (ASCII_ISALPHA(*p))
	ਜਦੋਂ (ASCII_ISALNUM(*p))
	    ++p;
    ਜੇ (!ਸਿਰੇ_excmd2(eap->arg, p) && !VIM_ISWHITE(*p))
    {
	emsg(_(e_invalid_command_name));
	ਜਾਓ theend;
    }
    ਸਿਰਾ = p;
    name_len = (ਪੂਰਨ)(ਸਿਰਾ - name);

    // If there is nothing after the name, and no attributes were specified,
    // we are listing commands
    p = skipwhite(ਸਿਰਾ);
    ਜੇ (!has_attr && ਸਿਰੇ_excmd2(eap->arg, p))
    {
	uc_list(name, name_len);
    }
    ਨਹੀਂਤਾਂ ਜੇ (!ASCII_ISUPPER(*name))
    {
	emsg(_(e_user_defined_commands_must_ਸ਼ੁਰੂ_with_an_uppercase_letter));
    }
    ਨਹੀਂਤਾਂ ਜੇ ((name_len == 1 && *name == 'X')
	  || (name_len <= 4
		  && STRNCMP(name, "Next", name_len > 4 ? 4 : name_len) == 0))
    {
	emsg(_(e_reserved_name_cannot_be_ਵਰਤਿਆ_for_user_defined_command));
    }
    ਨਹੀਂਤਾਂ ਜੇ (compl > 0 && (argt & EX_EXTRA) == 0)
    {
	// Some plugins rely on silently ignoring the mistake, only make this
	// an error in Vim9 script.
	ਜੇ (ਵਿੱਚ_vim9script())
	    emsg(_(e_complete_ਵਰਤਿਆ_without_allowing_arguments));
	ਨਹੀਂਤਾਂ
	    give_warning_with_ਸਰੋਤ(
		       (char_u *)_(e_complete_ਵਰਤਿਆ_without_allowing_arguments),
								   TRUE, TRUE);
    }
    ਨਹੀਂਤਾਂ ਜੇ ((compl_opt & UCC_ESCAPE) && (argt & EX_ARGSPACE))
    {
	// -nargs=_ disables the argument splitter, so escaping spaces in
	// inserted matches has no effect.  Reject the combination instead of
	// silently ignoring it.
	emsg(_(e_completeopt_escape_cannot_be_ਵਰਤਿਆ_with_nargs_underscore));
    }
    ਨਹੀਂਤਾਂ
    {
	char_u *tofree = ਨਿਰਾ;

	p = may_get_cmd_ਖੰਡ(eap, p, &tofree, &flags);

	uc_ਜੋੜੋ_command(name, ਸਿਰਾ - name, p, argt, def, flags, compl, compl_arg,
					compl_opt, addr_type_arg, eap->forceit);
	vim_free(tofree);

	ਵਾਪਸ;  // success
    }

theend:
    vim_free(compl_arg);
}

/*
 * ":comclear" implementation
 * Clear all user commands, global and for current buffer.
 */
    ਖਾਲੀ
ex_comclear(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    uc_clear(&ucmds);
    ਜੇ (curbuf != ਨਿਰਾ)
	uc_clear(&curbuf->b_ucmds);
}

/*
 * If ucmd_locked is set give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ਸਥਿਰ ਪੂਰਨ
is_ucmd_locked(ਖਾਲੀ)
{
    ਜੇ (ucmd_locked > 0)
    {
	emsg(_(e_cannot_change_user_commands_while_listing));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Clear all user commands for "gap".
 */
    ਖਾਲੀ
uc_clear(garray_T *gap)
{
    ਪੂਰਨ		i;
    ucmd_T	*cmd;

    ਜੇ (is_ucmd_locked())
	ਵਾਪਸ;

    ਲਈ (i = 0; i < gap->ga_len; ++i)
    {
	cmd = USER_CMD_GA(gap, i);
	vim_free(cmd->uc_name);
	cmd->uc_namelen = 0;
	vim_free(cmd->uc_rep);
#ਜੇ defined(FEAT_EVAL)
	vim_free(cmd->uc_compl_arg);
#ਜੇ_ਅੰਤ
    }
    ga_clear(gap);
}

/*
 * ":delcommand" implementation
 */
    ਖਾਲੀ
ex_delcommand(exarg_T *eap)
{
    ਪੂਰਨ		i = 0;
    ucmd_T	*cmd = ਨਿਰਾ;
    ਪੂਰਨ		res = -1;
    garray_T	*gap;
    char_u	*arg = eap->arg;
    ਪੂਰਨ		ਬਫਰ_only = FALSE;

    ਜੇ (STRNCMP(arg, "-buffer", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	ਬਫਰ_only = TRUE;
	arg = skipwhite(arg + 7);
    }

    gap = &curbuf->b_ucmds;
    ਲਈ (;;)
    {
	ਲਈ (i = 0; i < gap->ga_len; ++i)
	{
	    cmd = USER_CMD_GA(gap, i);
	    res = STRCMP(arg, cmd->uc_name);
	    ਜੇ (res <= 0)
		ਰੋਕੋ;
	}
	ਜੇ (gap == &ucmds || res == 0 || ਬਫਰ_only)
	    ਰੋਕੋ;
	gap = &ucmds;
    }

    ਜੇ (res != 0)
    {
	semsg(_(ਬਫਰ_only
		    ? e_no_such_user_defined_command_ਵਿੱਚ_current_ਬਫਰ_str
		    : e_no_such_user_defined_command_str), arg);
	ਵਾਪਸ;
    }

    ਜੇ (is_ucmd_locked())
	ਵਾਪਸ;

    vim_free(cmd->uc_name);
    vim_free(cmd->uc_rep);
#ਜੇ defined(FEAT_EVAL)
    vim_free(cmd->uc_compl_arg);
#ਜੇ_ਅੰਤ

    --gap->ga_len;

    ਜੇ (i < gap->ga_len)
	mch_memmove(cmd, cmd + 1, (gap->ga_len - i) * ਆਕਾਰ(ucmd_T));
}

/*
 * Split and quote args for <f-args>.
 */
    ਸਥਿਰ char_u *
uc_ਵੰਡੋ_ਦਲੀਲਾਂ(char_u *arg, size_t *lenp)
{
    char_u *buf;
    char_u *p;
    char_u *q;
    ਪੂਰਨ len;

    // Precalculate length
    p = arg;
    len = 2; // Initial and final quotes

    ਜਦੋਂ (*p)
    {
	ਜੇ (p[0] == '\\' && p[1] == '\\')
	{
	    len += 2;
	    p += 2;
	}
	ਨਹੀਂਤਾਂ ਜੇ (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    len += 1;
	    p += 2;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '\\' || *p == '"')
	{
	    len += 2;
	    p += 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    ਜੇ (*p == NUL)
		ਰੋਕੋ;
	    len += 4; // ", "
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ charlen = (*mb_ptr2len)(p);

	    len += charlen;
	    p += charlen;
	}
    }

    buf = alloc(len + 1);
    ਜੇ (buf == ਨਿਰਾ)
    {
	*lenp = 0;
	ਵਾਪਸ buf;
    }

    p = arg;
    q = buf;
    *q++ = '"';
    ਜਦੋਂ (*p)
    {
	ਜੇ (p[0] == '\\' && p[1] == '\\')
	{
	    *q++ = '\\';
	    *q++ = '\\';
	    p += 2;
	}
	ਨਹੀਂਤਾਂ ਜੇ (p[0] == '\\' && VIM_ISWHITE(p[1]))
	{
	    *q++ = p[1];
	    p += 2;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '\\' || *p == '"')
	{
	    *q++ = '\\';
	    *q++ = *p++;
	}
	ਨਹੀਂਤਾਂ ਜੇ (VIM_ISWHITE(*p))
	{
	    p = skipwhite(p);
	    ਜੇ (*p == NUL)
		ਰੋਕੋ;
	    *q++ = '"';
	    *q++ = ',';
	    *q++ = ' ';
	    *q++ = '"';
	}
	ਨਹੀਂਤਾਂ
	{
	    MB_COPY_CHAR(p, q);
	}
    }
    *q++ = '"';
    *q = NUL;

    *lenp = len;
    ਵਾਪਸ buf;
}

    ਸਥਿਰ size_t
ਜੋੜੋ_cmd_modifier(
    char_u	*buf,
    size_t	buflen,
    ਅੱਖਰ	*mod_str,
    size_t	mod_strlen,
    ਪੂਰਨ		*multi_mods)
{
    ਜੇ (buf != ਨਿਰਾ)
    {
	ਜੇ (*multi_mods)
	{
	    STRCPY(buf + buflen, " ");	// the separating space
	    ++buflen;
	}
	STRCPY(buf + buflen, mod_str);
    }

    ਜੇ (*multi_mods)
	++mod_strlen;			// +1 for the separating space
    ਨਹੀਂਤਾਂ
	*multi_mods = 1;

    ਵਾਪਸ mod_strlen;
}

/*
 * Add modifiers from "cmod->cmod_split" to "buf".  Set "multi_mods" when one
 * was added.  Return the number of bytes added.
 */
    size_t
ਜੋੜੋ_win_cmd_modifiers(char_u *buf, cmdmod_T *cmod, ਪੂਰਨ *multi_mods)
{
    size_t buflen = 0;

    // :aboveleft and :leftabove
    ਜੇ (cmod->cmod_ਵੰਡੋ & WSP_ABOVE)
	buflen += ਜੋੜੋ_cmd_modifier(buf, buflen, "aboveleft", STRLEN_LITERAL("aboveleft"), multi_mods);
    // :belowright and :rightbelow
    ਜੇ (cmod->cmod_ਵੰਡੋ & WSP_ਹੇਠਾਂ)
	buflen += ਜੋੜੋ_cmd_modifier(buf, buflen, "belowright", STRLEN_LITERAL("belowright"), multi_mods);
    // :botright
    ਜੇ (cmod->cmod_ਵੰਡੋ & WSP_BOT)
	buflen += ਜੋੜੋ_cmd_modifier(buf, buflen, "botright", STRLEN_LITERAL("botright"), multi_mods);

    // :tab
    ਜੇ (cmod->cmod_tab > 0)
    {
	ਪੂਰਨ tabnr = cmod->cmod_tab - 1;

	ਜੇ (tabnr == tabpage_ਸੂਚਕਾਂਕ(curtab))
	{
	    // For compatibility, don't add a tabpage number if it is the same
	    // as the default number for :tab.
	    buflen += ਜੋੜੋ_cmd_modifier(buf, buflen, "tab", STRLEN_LITERAL("tab"), multi_mods);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਅੱਖਰ tab_buf[NUMBUFLEN + 3];
	    size_t tab_buflen;

	    tab_buflen = vim_snprintf(tab_buf, ਆਕਾਰ(tab_buf), "%dtab", tabnr);
	    buflen += ਜੋੜੋ_cmd_modifier(buf, buflen, tab_buf, tab_buflen, multi_mods);
	}
    }

    // :topleft
    ਜੇ (cmod->cmod_ਵੰਡੋ & WSP_ਸਿਖਰ)
	buflen += ਜੋੜੋ_cmd_modifier(buf, buflen, "topleft", STRLEN_LITERAL("topleft"), multi_mods);
    // :vertical
    ਜੇ (cmod->cmod_ਵੰਡੋ & WSP_VERT)
	buflen += ਜੋੜੋ_cmd_modifier(buf, buflen, "vertical", STRLEN_LITERAL("vertical"), multi_mods);
    // :horizontal
    ਜੇ (cmod->cmod_ਵੰਡੋ & WSP_HOR)
	buflen += ਜੋੜੋ_cmd_modifier(buf, buflen, "horizontal", STRLEN_LITERAL("horizontal"), multi_mods);

    ਵਾਪਸ buflen;
}

/*
 * Generate text for the "cmod" command modifiers.
 * If "buf" is NULL just return the length.
 */
    size_t
produce_cmdmods(char_u *buf, cmdmod_T *cmod, ਪੂਰਨ quote)
{
    size_t  buflen = 0;
    ਪੂਰਨ	    multi_mods = 0;
    ਪੂਰਨ	    i;
    ਸਥਿਰ keyvalue_T mod_ਇੰਦਰਾਜ_tab[] =
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE_CMD
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_BROWSE, "browse"),
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_CONFIRM, "confirm"),
#ਜੇ_ਅੰਤ
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_HIDE, "hide"),
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_KEEPALT, "keepalt"),
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_KEEPJUMPS, "keepjumps"),
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_KEEPMARKS, "keepmarks"),
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_KEEPPATTERNS, "keeppatterns"),
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_LOCKMARKS, "lockmarks"),
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_NOSWAPFILE, "noswapfile"),
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_UNSILENT, "unsilent"),
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_NOAUTOCMD, "noautocmd"),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_SANDBOX, "sandbox"),
#ਜੇ_ਅੰਤ
	KEYVALUE_ਇੰਦਰਾਜ(CMOD_LEGACY, "legacy")
    };

    ਜੇ (quote)
    {
	++buflen;
	ਜੇ (buf != ਨਿਰਾ)
	{
	    *buf = '"';
	    *(buf + buflen) = NUL;
	}
    }
    ਨਹੀਂਤਾਂ
    ਜੇ (buf != ਨਿਰਾ)
	*buf = NUL;

    // the modifiers that are simple flags
    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(mod_ਇੰਦਰਾਜ_tab); ++i)
	ਜੇ (cmod->cmod_flags & mod_ਇੰਦਰਾਜ_tab[i].ਕੁੰਜੀ)
	    buflen += ਜੋੜੋ_cmd_modifier(buf, buflen,
		    (ਅੱਖਰ *)mod_ਇੰਦਰਾਜ_tab[i].ਮੁੱਲ.string,
		    mod_ਇੰਦਰਾਜ_tab[i].ਮੁੱਲ.ਲੰਮਾਈ, &multi_mods);

    // :silent
    ਜੇ (cmod->cmod_flags & CMOD_SILENT)
    {
	ਜੇ (cmod->cmod_flags & CMOD_ERRSILENT)
	    buflen += ਜੋੜੋ_cmd_modifier(buf, buflen, "silent!",
		    STRLEN_LITERAL("silent!"), &multi_mods);
	ਨਹੀਂਤਾਂ
	    buflen += ਜੋੜੋ_cmd_modifier(buf, buflen, "silent",
		    STRLEN_LITERAL("silent"), &multi_mods);
    }

    // :verbose
    ਜੇ (cmod->cmod_verbose > 0)
    {
	ਪੂਰਨ verbose_ਮੁੱਲ = cmod->cmod_verbose - 1;

	ਜੇ (verbose_ਮੁੱਲ == 1)
	    buflen += ਜੋੜੋ_cmd_modifier(buf, buflen, "verbose", STRLEN_LITERAL("verbose"), &multi_mods);
	ਨਹੀਂਤਾਂ
	{
	    ਅੱਖਰ verbose_buf[NUMBUFLEN];
	    size_t verbose_buflen;

	    verbose_buflen = vim_snprintf(verbose_buf, ਆਕਾਰ(verbose_buf), "%dverbose", verbose_ਮੁੱਲ);
	    buflen += ਜੋੜੋ_cmd_modifier(buf, buflen, verbose_buf, verbose_buflen, &multi_mods);
	}
    }

    // flags from cmod->cmod_split
    buflen += ਜੋੜੋ_win_cmd_modifiers((buf == ਨਿਰਾ) ? ਨਿਰਾ : buf + buflen, cmod, &multi_mods);

    ਜੇ (quote)
    {
	ਜੇ (buf == ਨਿਰਾ)
	    ++buflen;
	ਨਹੀਂਤਾਂ
	{
	    *(buf + buflen) = '"';
	    ++buflen;
	    *(buf + buflen) = NUL;
	}
    }

    ਵਾਪਸ buflen;
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
    ਸਥਿਰ size_t
uc_check_ਕੋਡ(
    char_u	*ਕੋਡ,
    size_t	len,
    char_u	*buf,
    ucmd_T	*cmd,		// the user command we're expanding
    exarg_T	*eap,		// ex arguments
    char_u	**ਵੰਡੋ_buf,
    size_t	*ਵੰਡੋ_len)
{
    size_t	ਨਤੀਜਾ = 0;
    char_u	*p = ਕੋਡ + 1;
    size_t	l = len - 2;
    ਪੂਰਨ		quote = 0;
    ਗਿਣਤੀ {
	ct_ਦਲੀਲਾਂ,
	ct_BANG,
	ct_ਗਿਣਤੀ,
	ct_LINE1,
	ct_LINE2,
	ct_RANGE,
	ct_MODS,
	ct_ਰਜਿਸਟਰ,
	ct_LT,
	ct_NONE
    } type = ct_NONE;

    ਜੇ ((vim_strchr((char_u *)"qQfF", *p) != ਨਿਰਾ) && p[1] == '-')
    {
	quote = (*p == 'q' || *p == 'Q') ? 1 : 2;
	p += 2;
	l -= 2;
    }

    ++l;
    ਜੇ (l <= 1)
	type = ct_NONE;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "args>", l) == 0)
	type = ct_ਦਲੀਲਾਂ;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "bang>", l) == 0)
	type = ct_BANG;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "count>", l) == 0)
	type = ct_ਗਿਣਤੀ;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "line1>", l) == 0)
	type = ct_LINE1;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "line2>", l) == 0)
	type = ct_LINE2;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "range>", l) == 0)
	type = ct_RANGE;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "lt>", l) == 0)
	type = ct_LT;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "reg>", l) == 0 || STRNICMP(p, "register>", l) == 0)
	type = ct_ਰਜਿਸਟਰ;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "mods>", l) == 0)
	type = ct_MODS;

    ਚੋਣ (type)
    {
    ਹਾਲਤ ct_ਦਲੀਲਾਂ:
	// Simple case first
	ਜੇ (*eap->arg == NUL)
	{
	    ਜੇ (quote == 1)
	    {
		ਨਤੀਜਾ = 2;
		ਜੇ (buf != ਨਿਰਾ)
		    STRCPY(buf, "''");
	    }
	    ਨਹੀਂਤਾਂ
		ਨਤੀਜਾ = 0;
	    ਰੋਕੋ;
	}

	// When specified there is a single argument don't split it.
	// Works for ":Cmd %" when % is "a b c".
	ਜੇ ((eap->argt & EX_NOSPC) && quote == 2)
	    quote = 1;

	ਚੋਣ (quote)
	{
	ਹਾਲਤ 0: // No quoting, no splitting
	    ਨਤੀਜਾ = STRLEN(eap->arg);
	    ਜੇ (buf != ਨਿਰਾ)
		STRCPY(buf, eap->arg);
	    ਰੋਕੋ;
	ਹਾਲਤ 1: // Quote, but don't split
	{
	    // For -completeopt=escape give <q-args> the logical value: a
	    // backslash before a space, tab or backslash is collapsed.
	    ਪੂਰਨ	unesc = (cmd->uc_compl_opt & UCC_ESCAPE) != 0;

	    ਨਤੀਜਾ = STRLEN(eap->arg) + 2;
	    ਲਈ (p = eap->arg; *p; ++p)
	    {
		ਜੇ (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
		    // DBCS can contain \ in a trail byte, skip the
		    // double-byte character.
		    ++p;
		ਨਹੀਂਤਾਂ ਜੇ (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		{
		    ਜੇ (VIM_ISWHITE(p[1]))
			--ਨਤੀਜਾ;
		    ++p;
		}
		ਨਹੀਂਤਾਂ
		     ਜੇ (*p == '\\' || *p == '"')
		    ++ਨਤੀਜਾ;
	    }

	    ਜੇ (buf != ਨਿਰਾ)
	    {
		*buf++ = '"';
		ਲਈ (p = eap->arg; *p; ++p)
		{
		    ਜੇ (enc_dbcs != 0 && (*mb_ptr2len)(p) == 2)
			// DBCS can contain \ in a trail byte, copy the
			// double-byte character to avoid escaping.
			*buf++ = *p++;
		    ਨਹੀਂਤਾਂ ਜੇ (unesc && *p == '\\'
				     && (VIM_ISWHITE(p[1]) || p[1] == '\\'))
		    {
			++p;	// drop the escaping backslash
			ਜੇ (*p == '\\')
			    *buf++ = '\\';	// re-escape for the quotes
		    }
		    ਨਹੀਂਤਾਂ
			 ਜੇ (*p == '\\' || *p == '"')
			*buf++ = '\\';
		    *buf++ = *p;
		}
		*buf = '"';
	    }

	    ਰੋਕੋ;
	}
	ਹਾਲਤ 2: // Quote and split (<f-args>)
	    // This is hard, so only do it once, and cache the result
	    ਜੇ (*ਵੰਡੋ_buf == ਨਿਰਾ)
		*ਵੰਡੋ_buf = uc_ਵੰਡੋ_ਦਲੀਲਾਂ(eap->arg, ਵੰਡੋ_len);

	    ਨਤੀਜਾ = *ਵੰਡੋ_len;
	    ਜੇ (buf != ਨਿਰਾ && ਨਤੀਜਾ != 0)
		STRCPY(buf, *ਵੰਡੋ_buf);

	    ਰੋਕੋ;
	}
	ਰੋਕੋ;

    ਹਾਲਤ ct_BANG:
	ਨਤੀਜਾ = eap->forceit ? 1 : 0;
	ਜੇ (quote)
	    ਨਤੀਜਾ += 2;
	ਜੇ (buf != ਨਿਰਾ)
	{
	    ਜੇ (quote)
		*buf++ = '"';
	    ਜੇ (eap->forceit)
		*buf++ = '!';
	    ਜੇ (quote)
		*buf = '"';
	}
	ਰੋਕੋ;

    ਹਾਲਤ ct_LINE1:
    ਹਾਲਤ ct_LINE2:
    ਹਾਲਤ ct_RANGE:
    ਹਾਲਤ ct_ਗਿਣਤੀ:
    {
	ਅੱਖਰ ਅੰਕ_buf[NUMBUFLEN];
	ਲੰਮਾ ਅੰਕ = (type == ct_LINE1) ? eap->line1 :
		   (type == ct_LINE2) ? eap->line2 :
		   (type == ct_RANGE) ? eap->addr_ਗਿਣਤੀ :
		   (eap->addr_ਗਿਣਤੀ > 0) ? eap->line2 : cmd->uc_def;
	size_t ਅੰਕ_len;

	ਅੰਕ_len = vim_snprintf(ਅੰਕ_buf, ਆਕਾਰ(ਅੰਕ_buf), "%ld", ਅੰਕ);
	ਨਤੀਜਾ = ਅੰਕ_len;

	ਜੇ (quote)
	    ਨਤੀਜਾ += 2;

	ਜੇ (buf != ਨਿਰਾ)
	{
	    ਜੇ (quote)
		*buf++ = '"';
	    STRCPY(buf, ਅੰਕ_buf);
	    buf += ਅੰਕ_len;
	    ਜੇ (quote)
		*buf = '"';
	}

	ਰੋਕੋ;
    }

    ਹਾਲਤ ct_MODS:
    {
	ਨਤੀਜਾ = produce_cmdmods(buf, &cmdmod, quote);
	ਰੋਕੋ;
    }

    ਹਾਲਤ ct_ਰਜਿਸਟਰ:
	ਨਤੀਜਾ = eap->regname ? 1 : 0;
	ਜੇ (quote)
	    ਨਤੀਜਾ += 2;
	ਜੇ (buf != ਨਿਰਾ)
	{
	    ਜੇ (quote)
		*buf++ = '\'';
	    ਜੇ (eap->regname)
		*buf++ = eap->regname;
	    ਜੇ (quote)
		*buf = '\'';
	}
	ਰੋਕੋ;

    ਹਾਲਤ ct_LT:
	ਨਤੀਜਾ = 1;
	ਜੇ (buf != ਨਿਰਾ)
	    *buf = '<';
	ਰੋਕੋ;

    ਮੂਲ:
	// Not recognized: just copy the '<' and return -1.
	ਨਤੀਜਾ = (size_t)-1;
	ਜੇ (buf != ਨਿਰਾ)
	    *buf = '<';
	ਰੋਕੋ;
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Execute a user defined command.
 */
    ਖਾਲੀ
do_ucmd(exarg_T *eap)
{
    char_u	*buf;
    char_u	*p;
    char_u	*q;

    char_u	*ਸ਼ੁਰੂ;
    char_u	*ਸਿਰਾ = ਨਿਰਾ;
    char_u	*ksp;
    size_t	len, totlen;

    size_t	ਵੰਡੋ_len = 0;
    char_u	*ਵੰਡੋ_buf = ਨਿਰਾ;
    ucmd_T	*cmd;
    sctx_T	save_current_sctx;
    ਪੂਰਨ		restore_current_sctx = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ		restore_script_ਸੰਸਕਰਣ = 0;
#ਜੇ_ਅੰਤ

    ਜੇ (eap->cmdidx == CMD_USER)
	cmd = USER_CMD(eap->useridx);
    ਨਹੀਂਤਾਂ
	cmd = USER_CMD_GA(&prevwin_curwin()->w_ਬਫਰ->b_ucmds, eap->useridx);

    /*
     * Replace <> in the command by the arguments.
     * First round: "buf" is NULL, compute length, allocate "buf".
     * Second round: copy result into "buf".
     */
    buf = ਨਿਰਾ;
    ਲਈ (;;)
    {
	p = cmd->uc_rep;    //ਸਰੋਤ
	q = buf;	    // destination
	totlen = 0;

	ਲਈ (;;)
	{
	    ਸ਼ੁਰੂ = vim_strchr(p, '<');
	    ਜੇ (ਸ਼ੁਰੂ != ਨਿਰਾ)
		ਸਿਰਾ = vim_strchr(ਸ਼ੁਰੂ + 1, '>');
	    ਜੇ (buf != ਨਿਰਾ)
	    {
		ਲਈ (ksp = p; *ksp != NUL && *ksp != K_SPECIAL; ++ksp)
		    ;
		ਜੇ (*ksp == K_SPECIAL
			&& (ਸ਼ੁਰੂ == ਨਿਰਾ || ksp < ਸ਼ੁਰੂ || ਸਿਰਾ == ਨਿਰਾ)
			&& ((ksp[1] == KS_SPECIAL && ksp[2] == KE_FILLER)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			    || (ksp[1] == KS_EXTRA && ksp[2] == (ਪੂਰਨ)KE_CSI)
#ਜੇ_ਅੰਤ
			    ))
		{
		    // K_SPECIAL has been put in the buffer as K_SPECIAL
		    // KS_SPECIAL KE_FILLER, like for mappings, but
		    // do_cmdline() doesn't handle that, so convert it back.
		    // Also change K_SPECIAL KS_EXTRA KE_CSI into CSI.
		    len = ksp - p;
		    ਜੇ (len > 0)
		    {
			mch_memmove(q, p, len);
			q += len;
		    }
		    *q++ = ksp[1] == KS_SPECIAL ? K_SPECIAL : CSI;
		    p = ksp + 3;
		    ਜਾਰੀ;
		}
	    }

	    // break if no <item> is found
	    ਜੇ (ਸ਼ੁਰੂ == ਨਿਰਾ || ਸਿਰਾ == ਨਿਰਾ)
		ਰੋਕੋ;

	    // Include the '>'
	    ++ਸਿਰਾ;

	    // Take everything up to the '<'
	    len = ਸ਼ੁਰੂ - p;
	    ਜੇ (buf == ਨਿਰਾ)
		totlen += len;
	    ਨਹੀਂਤਾਂ
	    {
		mch_memmove(q, p, len);
		q += len;
	    }

	    len = uc_check_ਕੋਡ(ਸ਼ੁਰੂ, ਸਿਰਾ - ਸ਼ੁਰੂ, q, cmd, eap,
			     &ਵੰਡੋ_buf, &ਵੰਡੋ_len);
	    ਜੇ (len == (size_t)-1)
	    {
		// no match, continue after '<'
		p = ਸ਼ੁਰੂ + 1;
		len = 1;
	    }
	    ਨਹੀਂਤਾਂ
		p = ਸਿਰਾ;
	    ਜੇ (buf == ਨਿਰਾ)
		totlen += len;
	    ਨਹੀਂਤਾਂ
		q += len;
	}
	ਜੇ (buf != ਨਿਰਾ)	    // second time here, finished
	{
	    STRCPY(q, p);
	    ਰੋਕੋ;
	}

	totlen += STRLEN(p);	    // Add on the trailing characters
	buf = alloc(totlen + 1);
	ਜੇ (buf == ਨਿਰਾ)
	{
	    vim_free(ਵੰਡੋ_buf);
	    ਵਾਪਸ;
	}
    }

    ਜੇ ((cmd->uc_argt & EX_KEEPSCRIPT) == 0)
    {
	restore_current_sctx = TRUE;
	save_current_sctx = current_sctx;
	current_sctx.sc_ਸੰਸਕਰਣ = cmd->uc_script_ctx.sc_ਸੰਸਕਰਣ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	current_sctx.sc_sid = cmd->uc_script_ctx.sc_sid;
	ਜੇ (cmd->uc_flags & UC_VIM9)
	{
	    // In a {} block variables use Vim9 script rules, even in a legacy
	    // script.
	    restore_script_ਸੰਸਕਰਣ =
				  SCRIPT_ਵਸਤੂ(current_sctx.sc_sid)->sn_ਸੰਸਕਰਣ;
	    SCRIPT_ਵਸਤੂ(current_sctx.sc_sid)->sn_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_VIM9;
	}
#ਜੇ_ਅੰਤ
    }

    (ਖਾਲੀ)do_cmdline(buf, eap->ea_getline, eap->cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);

    // Careful: Do not use "cmd" here, it may have become invalid if a user
    // command was added.
    ਜੇ (restore_current_sctx)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (restore_script_ਸੰਸਕਰਣ != 0)
	    SCRIPT_ਵਸਤੂ(current_sctx.sc_sid)->sn_ਸੰਸਕਰਣ =
							restore_script_ਸੰਸਕਰਣ;
#ਜੇ_ਅੰਤ
	current_sctx = save_current_sctx;
    }
    vim_free(buf);
    vim_free(ਵੰਡੋ_buf);
}
